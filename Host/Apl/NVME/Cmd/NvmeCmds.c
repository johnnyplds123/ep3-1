// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2012  -  All rights reserved
//
//  This computer program contains confidential and proprietary information,
//  and  may NOT  be reproduced or transmitted, in whole or in part,  in any
//  form,  or by any means electronic, mechanical, photo-optical, or  other-
//  wise, and  may NOT  be translated  into  another  language  without  the
//  express written permission from Marvell Corporation.
//
// ============================================================================
// =      C O M P A N Y      P R O P R I E T A R Y      M A T E R I A L       =
// ============================================================================
//
//! @file
//! @brief NVMe Command handler implementation
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"
#include "SharePkg.h"
#include "RegNvmePkg.h"
#include "NvmeCmdPkg.h"

// REG

// HAL
#include "HalPm.h"
#include "HalPcieLtr.h"
#include "HalNvmeXfer.h"

// APL
#include "HldNvme.h"
#include "HldHost.h"
#include "SysInfo.h"
#include "NvmeGetLogPage.h"
#include "NvmeSecurityCmd.h"
#include "NvmeIdentify.h"
#include "Cache.h"
#include "NvmeDirective.h"

// Code Section
#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static HcmdQ_t* FetchCommand(void);
static void     DecodeCommand(HcmdQ_t* pHcmdQ);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
NvmeIfMgr_t         gNvmeIfMgr;                             ///< NVMe I/F protocol and command service manager
NvmeCdb_t           sNvmeCdb;                               ///< NVMe CDB(command data block) for last command
NvmeSdb_t           sNvmeSdb[MAX_HCMD_Q_COUNT];             ///< NVMe SDB(status data block) per command

static NvmeCmdTbl_t sAdminCmdTbl[cNvmeAdminCmdTableSize];   ///< NVMe Admin command handler table
NvmeCmdTbl_t        sIoCmdTbl[cNvmeIoCmdTableSize];         ///< NVMe I/O command handler table

volatile uint32_t   gBootNewFwFlag;
volatile uint32_t   gMiscEvnet;
uint32_t            gDetectFwSize = 0;

uint32_t            gResetFlag = 0;
uint8_t*            gNvmeIfMgrTrimBuffer;
uint8_t             gIfBuffer[4 * KBYTE];
uint8_t*            gPreSubmitBuffer;
OTFUpdateInfo_t     gOTFUpdateInfo;
AsyEventBackup_t    gAsyEventBackup;
UpdateFWCmdBackup_t gUpFwCmdBackup;

// Interface Setting Information
NvmeHwIoCmdDw0_t    ghwIocmdDw0_ids[NVME_NUM_OF_NAMESPACES];
NvmeHwIoCmdDw1_t    ghwIocmdDw1_ids[NVME_NUM_OF_NAMESPACES];
NvmeHwIoCmdDw0_t    ghwIocmdDw0_ssd[NVME_NUM_OF_NAMESPACES];
NvmeHwIoCmdDw1_t    ghwIocmdDw1_ssd[NVME_NUM_OF_NAMESPACES];

const NvmeIfMgrLbaFormat_t cLBAFTable[HW_MAX_LBA_FORMATS] =
{
    // dataSize   metaSize, rp                          MBU_Mask    MBU_Shift  hbu_shift
    {HLBU_DATA_SIZE  ,   0,  0,  (MBU_SIZE / HLBU_DATA_SIZE - 1),           3,        0},
    {4096            ,   0,  0,  (MBU_SIZE / 4096           - 1),           0,        3},
    {4096            ,   8,  2,  (MBU_SIZE / 4096           - 1),           0,        0},
    {0               ,   0,  0,                                0,           0,        0}
};

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
extern PowerStateDescriptor_t tPSD[];

//-----------------------------------------------------------------------------
//  Imported function proto-tpye without header include
//-----------------------------------------------------------------------------
extern void NvmeCmd_RegisterIo(NvmeCmdTbl_t* pHcmdTbl);
extern void NvmeCmd_RegisterAdmin(NvmeCmdTbl_t* pHcmdTbl);
extern void NvmeCmd_NvmespaceInit(InitBootMode_t initMode);
extern void Asynchronous_EventBackup(void);
extern void Asynchronous_EventRestore(void);
extern void Nvme_HandleCmdReadyEvent(void);
extern U16  NvmeCmd_AsynchronousEventPeek(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    NvmeCmd::OneTimeInit

    @param[in]  initMode    initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t NvmeCmd_OneTimeInit(InitBootMode_t initMode)
{
    uint32_t idx;

    SET_CPU_0_DEBUG_FLAG(DBG_DUMP_HOST_NONRW | DBG_DUMP_ERRHANDLE | DBG_DUMP_ALL_INFO_RST);

    if (((U32)&gNvmeIfMgr.rsvd1 + sizeof(gNvmeIfMgr.rsvd1) - (U32)&gNvmeIfMgr) != OTF_SECTION_NVME_SIZE)
    {
        // OTF section size is invalid
        ASSERT(0);
    }

    // Setup I/F buffer
    gNvmeIfMgr.pScratchBuffer = (uint8_t*)smSSD[SSD_IF_START].startAddr;
    gNvmeIfMgrTrimBuffer      = MEM_AllocBuffer(sizeof(FioTrimFmt_t) * 256 * HW_MAX_IO_CMD_INDEX, 512);
    gPreSubmitBuffer          = MEM_AllocBuffer(HOST_PAGE_SIZE, 16);

    // Setup LBA format
    for (idx = 0; idx < NVME_NUM_OF_LBAF; idx++)
    {
        HldNvme_SetHlbaFormat(idx, cNvmeFormatDataSize, cLBAFTable[idx].dataSize, NULL);
        HldNvme_SetHlbaFormat(idx, cNvmeFormatMetaSize, cLBAFTable[idx].metaSize, NULL);
    }

#if (CO_SUPPORT_DIRECTIVES == ENABLE)
    Directive_Init(initMode);
#endif

    // Setup Namespace
    NvmeCmd_NvmespaceInit(initMode);

    // cache status
    gNvmeIfMgr.readCacheEnable  = ENABLE;
    gNvmeIfMgr.writeCacheEnable = ENABLE;

    NvmeCmd_RegisterAdmin(sAdminCmdTbl);

    NvmeCmd_RegisterIo(sIoCmdTbl);

    NvmeCmd_Reset(cResetHard);

    NvmeCmd_OTFUpdateInit();

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    NvmeCmd::Reset

    @param[in]  resetMode    reset mode

    @return error code
**/
//-----------------------------------------------------------------------------
void NvmeCmd_Reset(ResetMode_t initMode)
{
    uint32_t sNsid;

    // revert to default
    MEM_COPY(currentFeatureValue, defaultFeatureValue, sizeof(currentFeatureValue));

    /*#if CO_SUPPORT_FEATURES_SELECT_SAVE
    if(smSysInfo->d.NVMeData.d.Feature.Tag == SI_TAG_FEATURE)
    {
        MEM_COPY(currentFeatureValue, smSysInfo->d.NVMeData.d.Feature.SaveFeature, sizeof(currentFeatureValue));
    }
    #endif*/

    // clear asynchronous releated
    gNvmeIfMgr.asyncEventMask = 0;
    gNvmeIfMgr.queuedError = cEcNoError;

    gNvmeIfMgr.PM_OperaState = 0;
    if (tPSD[currentFeatureValue[cPowerManagement]].NOPS == 0)  gNvmeIfMgr.PM_OperaState = currentFeatureValue[cPowerManagement];

#if (SUP_DIRE_STREAMS == ENABLE)
    sNsid = 1;
    do {
        MEM_CLR(&gDirectiveMgr.streamInfo[sNsid - 1], sizeof(StreamInfo_t));
    } while (++sNsid <= NVME_NUM_OF_NAMESPACES);
#endif

}

//-----------------------------------------------------------------------------
/**
    Get command handler function pointer from command code (indirect mode)

    @param[in]  pHcmdTbl host command decoding table
    @param[in]  pHcmdQ   host command queue pointer
    @param[in]  cmdCode  host command code

    @return host command handler function pointer
**/
//-----------------------------------------------------------------------------
static void GetCommandHandlerIndirect(NvmeCmdTbl_t* pHcmdTbl, HcmdQ_t* pHcmdQ, uint32_t cmdCode)
{
    while (pHcmdTbl->fptr != NULL)
    {
        if (pHcmdTbl->cmdCode == cmdCode)
        {
            pHcmdQ->fptr     = pHcmdTbl->fptr;
            pHcmdQ->attr.all = pHcmdTbl->attr;
            return;
        }
        pHcmdTbl++;
    }
    pHcmdQ->error    = MK_NVME_ERROR_CODE(cInvalidCommandOpcode, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    pHcmdQ->attr.all = 0;

    gNvmeIfMgr.lastErrorBitOffset  = 0;
    gNvmeIfMgr.lastErrorByteOffset = ERR_DW0_OFFSET; // DW0, Op code
}

//-----------------------------------------------------------------------------
/**
    NvmeCmd::Fetch

    @param[in]  pHcmdQ   host command queue pointer

    @return host command queue pointer
            if NULL, Command fetching is failed.
**/
//-----------------------------------------------------------------------------
static HcmdQ_t* FetchCommand(void)
{
    HldCmdId_t cmdId;
    NvmeCdb_t* pCdb;
    NvmeSdb_t* pSdb;
    HcmdQ_t* pHcmdQ = NULL;

    pCdb = NvmeCmd_GetCdbPointer(pHcmdQ);

    BEGIN_CS1
    cmdId = AplHost_PopCommand(pCdb, FALSE); // partial CDB for speed
    END_CS1

    // no command is fetched, return false
    if (cmdId == HLD_INVALID_CID)
    {
        return(NULL);
    }

    pHcmdQ = GetHcmdQ(cmdId);

    BEGIN_CS1
    NvmeCmd_ReturnCacheMsg(pHcmdQ);
    END_CS1

    Host_FetchCmd(pHcmdQ);

    pHcmdQ->error = cEcNoError;

    pSdb = NvmeCmd_GetSdbPointer(pHcmdQ);

    // initialize status data
    pSdb->DW0 = 0;         // DW0 = 0
    pSdb->DW3 = pCdb->CID; // DW3 CID

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_CDB) // print dw0 (CID, OPC), NSID(dw1), SLBA(dw10,11) NLB(dw12), PRP, HLD commandId
        D_PRINTF("[HCMD_I:%4X] %X L%2X:%X_%X N:%X\n", GetHcmdId(pHcmdQ), pCdb->DW0, pCdb->NSID, pCdb->DW11, pCdb->DW10, pCdb->DW12);
        //Debug_DumpMemU32((uint32_t)pCdb, sizeof(NvmeCdb_t));
    END_HOST_DBG_DUMP

    return (pHcmdQ);
}

//-----------------------------------------------------------------------------
/**
    NvmeCmd::Decode

    Decode command and parameters.

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
static void DecodeLba64(HcmdQ_t* pHcmdQ)
{
    uint32_t nsid0;
    NvmeIoRwCommand_t* pCdb;
    AddressInfo_t* pAddrInfo;

    pCdb = (NvmeIoRwCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);

    nsid0 = pCdb->NSID - 1; // convert to zero based NSID

    pAddrInfo = GetHcmdAddressInfoPtr(pHcmdQ);
    pAddrInfo->lba                  = pCdb->SLBA;

    pAddrInfo->allFlags             = 0;
    pAddrInfo->nb                   = pCdb->NLB + 1; // NLB is zero based value
    pAddrInfo->nsId0                = nsid0;
    pAddrInfo->fua                  = pCdb->FUA;
    pAddrInfo->limitedRetry         = pCdb->LR;
#if 0
    pAddrInfo->accessFrequency      = pCdb->accessFrequency;
    pAddrInfo->accessLatency        = pCdb->accessLatency;
    pAddrInfo->sequentialRequest    = pCdb->sequentialRequest;
    pAddrInfo->incompressible       = pCdb->incompressible;
#endif

    if ((nsid0 >= NVME_NUM_OF_NAMESPACES) || (!gNvmeIfMgr.ns[nsid0].Attach))
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidNamespaceOrFormat, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);

        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW1_OFFSET; // DW1, Namespaces

        return;
    }

    if (pCdb->SLBA + pCdb->NLB >= gNvmeIfMgr.ns[nsid0].NSZE)       // check HLBA range
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cLbaOutOfRange, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);

        gNvmeIfMgr.lastErrorBitOffset = 0;
        gNvmeIfMgr.lastErrorByteOffset = (pCdb->SLBA >= gNvmeIfMgr.ns[nsid0].NSZE) ? ERR_DW10_OFFSET : ERR_DW12_OFFSET; // error parameter, SLBA or NLB ?

        return;
    }

    if (pAddrInfo->nb > BYTE2LBU(NVME_MAX_DATA_TRANSFER_SIZE))         // check transfer size
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);

        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW12_OFFSET; // error parameter in NLB

        return;
    }

#if CO_CHECK_PRP_FEATURE_SUPPORT            // test PRP alignment
    if (!TEST_ALIGNED(pCdb->PRP1_SGL1, 4) || !TEST_ALIGNED(pCdb->PRP2_SGL1, 4))
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cDataTransferError, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);

        gNvmeIfMgr.lastErrorBitOffset = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW6_OFFSET; // error parameter in PRP

        return;
    }
#endif

}

//-----------------------------------------------------------------------------
/**
    NvmeCmd::Decode

    Decode command and parameters.

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
static void DecodeCommand(HcmdQ_t* pHcmdQ)
{
    uint32_t cmdCode;

    cmdCode = NvmeCmd_GetCdbPointer(pHcmdQ)->OPC;

    if (IS_ADMIN_COMMAND_ID(pHcmdQ->cmdId))
    {
        GetCommandHandlerIndirect(sAdminCmdTbl, pHcmdQ, cmdCode);
    }

    if (pHcmdQ->attr.all & ATTR_LBA64)
    {
        DecodeLba64(pHcmdQ);
    }

    #if CO_SUPPORT_ATA_SECURITY
    if (pHcmdQ->error == cEcNoError)
    {
        pHcmdQ->error = Nvme_CMD_CheckSecurity(1, cmdCode);  // 1:AdminCmd
    }
    #endif
}

//-----------------------------------------------------------------------------
/**
    Convert error code to NVMe specific status codes

    @param[out] pSdb        NVMe SDB pointer
    @param[in]  error       error code
**/
//-----------------------------------------------------------------------------
static void SetStatusFromErrorCode(NvmeSdb_t* pSdb, Error_t error)
{
    if (IS_NVME_ERROR_CODE(error))
    {
        pSdb->SC  = (error>> 0) & 0xFF;
        pSdb->SCT = (error>> 8) & 0x07;
        pSdb->M   = (error>>14) & 0x01;
        pSdb->DNR = (error>>15) & 0x01;
    }
    else
    {
        pSdb->SC  = error;
        pSdb->SCT = cVendorSpecific;
        pSdb->M   = cMoreStatusInformation;
        pSdb->DNR = cDoNotRetry;
    }

    D_PRINTF("\tErr SCT[%02x], SC[%02x]\n", pSdb->SCT, pSdb->SC);
}

//-----------------------------------------------------------------------------
/**
    Complete command with error.\n
     - Send EoC to host wiht error code. \n
     - Release host command queue.\n

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
static void ErrorComplete(HcmdQ_t* pHcmdQ)
{
    NvmeSdb_t* pSdb;

    pSdb = NvmeCmd_GetSdbPointer(pHcmdQ);

    SetStatusFromErrorCode(pSdb, pHcmdQ->error);

    AplHost_SendStatus(GetHcmdId(pHcmdQ), pSdb, 1, 1);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_ERROR|DBG_DUMP_HOST_EOC)
        D_PRINTF("[HCMD_E:%4X] %X %X ERR=%X\n", GetHcmdId(pHcmdQ), pSdb->DW0, pSdb->DW3, pHcmdQ->error);
    END_HOST_DBG_DUMP

    gNvmeIfMgr.lastError = pHcmdQ->error;

    // TODO: Add additional error handling code here (error log or ...)
    // Check M bit to add extend error information in Error Log
    if (pSdb->M)
    {
        HcmdQ_t* pAsyHcmdQ = NULL;

        HandleSaveErrLog(pHcmdQ);
        pAsyHcmdQ = Host_PopFromHead(cHcsAsyncWait);
        if (pAsyHcmdQ!=NULL)
        {
            pAsyHcmdQ->asycType    = cErrorStatus;
            Host_PushToTail(cHcsAsyncReport, pAsyHcmdQ); // move to ready list to notify asynchronous Event
        }
    }

    Host_CommandComplete(pHcmdQ);
}

//-----------------------------------------------------------------------------
/**
    Complete command.\n
     - Send EoC to host if auto EoC is disabled or in case of any error.\n
     - Release host command queue.\n

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_Complete(HcmdQ_t* pHcmdQ)
{
    NvmeSdb_t* pSdb;

    if (pHcmdQ->error != cEcNoError)
    {
        ErrorComplete(pHcmdQ);
        return;
    }

    // check HW already sent EoC
    if (pHcmdQ->attr.b.autoEoC) return;

    pSdb = NvmeCmd_GetSdbPointer(pHcmdQ);

    AplHost_SendStatus(GetHcmdId(pHcmdQ), pSdb, 1, 1);

    Host_CommandComplete(pHcmdQ);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_EOC)
        D_PRINTF("[HCMD_C:%4X] %X %X\n", GetHcmdId(pHcmdQ), pSdb->DW0, pSdb->DW3);
    END_HOST_DBG_DUMP
}

//-----------------------------------------------------------------------------
/**
    Process a command.
      - Allocate a command queue entry.
      - Fetch CDB
      - Decode CDB
      - Execute Command
**/
//-----------------------------------------------------------------------------
bool NvmeCmd_ProcessOneCommand(void)
{
    HcmdQ_t* pHcmdQ;
    NvmeCdb_t* pCdb;

    pHcmdQ = FetchCommand();
    if (pHcmdQ == NULL)
    {
        return (FALSE);
    }

#if (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED)
    HalPcieLtr_CommandActive();
#endif

    DecodeCommand(pHcmdQ);

BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_ADMIN_INFO)
    pCdb = &sNvmeCdb;
    D_PRINTF("[ADM HCMD_I:%4X] %X L%2X:%X_%X N:%X\n", GetHcmdId(pHcmdQ), pCdb->DW0, pCdb->NSID, pCdb->DW11, pCdb->DW10, pCdb->DW12);
END_HOST_DBG_DUMP

    // execute function if no error.
    if (pHcmdQ->error == cEcNoError)
    {
        pHcmdQ->fptr(pHcmdQ);
    }

    // complete command if non-overlapped command or error
    if (pHcmdQ->attr.b.overlapped == 0 || pHcmdQ->error != cEcNoError)
    {
        NvmeCmd_Complete(pHcmdQ);
    }

    gNvmeIfMgr.LastNvmeCmd = NvmeCmd_GetCdbPointer(pHcmdQ)->OPC;

    if (gBootNewFwFlag & BOOT_FW_NOW)
    {
        MEM_COPY(&gUpFwCmdBackup.updateFwSdb, NvmeCmd_GetSdbPointer(pHcmdQ), sizeof(NvmeSdb_t));
        NvmeCmd_BootFromNewFW();
    }

    return(TRUE);
}

//-----------------------------------------------------------------------------
/**
    NVMe invalid opcode, aborted command

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_Invalid(HcmdQ_t* pHcmdQ)
{
    D_PRINTF("Invalid cmd\n");

    pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidCommandOpcode, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    pHcmdQ->attr.all = 0;

    gNvmeIfMgr.lastErrorBitOffset  = 0;
    gNvmeIfMgr.lastErrorByteOffset = ERR_DW0_OFFSET; // DW0, Op code
}

//-----------------------------------------------------------------------------
/**
    NVMe boot new FW function

**/
//-----------------------------------------------------------------------------
void NvmeCmd_BootFromNewFW(void)
{
    if (NvmeCmd_AsynchronousEventPeek() == NvmeCmd_ASY_RETURN_INFO_TYPE(cFirmwareActivationStarting, cNotice))
    {
        Nvme_HandleCmdReadyEvent();
    }

    rNvmeController.controller[cDefaultFunctionId].CSTS.b.PP  = 1;

    gBootNewFwFlag = FALSE;

#if (_MEDIA_==MEDIA_NAND)
    Msg_t* pMsg;
    pMsg = (Msg_t*) Ipc_GetExtendMsg(cMcShutdown);
    MSG_SET_EXTEND_INIT(pMsg, cMcShutdown, TRUE);
    IPC_SendMsgQ(cH2cReqQueue, pMsg);
    while (pMsg->hdr.b.status != cMsgCompleted);
#endif

    M_PRINTF("\nReboot now\n");
    MemWriteU32(SHARED_TCM_OTF_FLG_ADDR, REBOOT_FLAG);

    NvmeCmd_OTFUpdateBackup();
    HalTimer_DelayMsec(100); // Wait for all TX FIFO empty.

    // Backup all information then call reboot
    HalCpu_Reboot(0);
}

//-----------------------------------------------------------------------------
/**
    NVMe reboot information init

**/
//-----------------------------------------------------------------------------
void NvmeCmd_OTFUpdateInit(void)
{
    uint32_t totalSize;

    gOTFUpdateInfo.count = OTF_SECTION_COUNT;

    gOTFUpdateInfo.entry[OTF_SECTION_HEADER].srcAddr        = (uint32_t) &gOTFUpdateInfo;
    gOTFUpdateInfo.entry[OTF_SECTION_HOST].srcAddr          = (uint32_t) &gHostIfMgr;
    gOTFUpdateInfo.entry[OTF_SECTION_NVME].srcAddr          = (uint32_t) &gNvmeIfMgr;
    gOTFUpdateInfo.entry[OTF_SECTION_HBA].srcAddr           = (uint32_t) &NvmeHba;
    gOTFUpdateInfo.entry[OTF_SECTION_NVME_CTRL].srcAddr     = (uint32_t) &rNvmeController;
    gOTFUpdateInfo.entry[OTF_SECTION_ASYNC_CMD].srcAddr     = (uint32_t) &gAsyEventBackup;
    gOTFUpdateInfo.entry[OTF_SECTION_FW_CMD].srcAddr        = (uint32_t) &gUpFwCmdBackup;

    gOTFUpdateInfo.entry[OTF_SECTION_HEADER].size           = OTF_SECTION_HEADER_SIZE;
    gOTFUpdateInfo.entry[OTF_SECTION_HOST].size             = OTF_SECTION_HOST_SIZE;
    gOTFUpdateInfo.entry[OTF_SECTION_NVME].size             = OTF_SECTION_NVME_SIZE;
    gOTFUpdateInfo.entry[OTF_SECTION_HBA].size              = OTF_SECTION_HBA_SIZE;
    gOTFUpdateInfo.entry[OTF_SECTION_NVME_CTRL].size        = OTF_SECTION_NVME_CTRL_SIZE;
    gOTFUpdateInfo.entry[OTF_SECTION_ASYNC_CMD].size        = OTF_SECTION_ASYNC_CMD_SIZE;
    gOTFUpdateInfo.entry[OTF_SECTION_FW_CMD].size           = OTF_SECTION_UPFW_CMD_SIZE;

    gOTFUpdateInfo.entry[OTF_SECTION_HEADER].backupAddr     = (uint32_t) SHARED_SPM_REBOOT_ADDR;
    gOTFUpdateInfo.entry[OTF_SECTION_HOST].backupAddr       = gOTFUpdateInfo.entry[OTF_SECTION_HEADER].backupAddr    + OTF_SECTION_HEADER_SIZE;
    gOTFUpdateInfo.entry[OTF_SECTION_NVME].backupAddr       = gOTFUpdateInfo.entry[OTF_SECTION_HOST].backupAddr      + OTF_SECTION_HOST_SIZE;
    gOTFUpdateInfo.entry[OTF_SECTION_HBA].backupAddr        = gOTFUpdateInfo.entry[OTF_SECTION_NVME].backupAddr      + OTF_SECTION_NVME_SIZE;
    gOTFUpdateInfo.entry[OTF_SECTION_NVME_CTRL].backupAddr  = gOTFUpdateInfo.entry[OTF_SECTION_HBA].backupAddr       + OTF_SECTION_HBA_SIZE;
    gOTFUpdateInfo.entry[OTF_SECTION_ASYNC_CMD].backupAddr  = gOTFUpdateInfo.entry[OTF_SECTION_NVME_CTRL].backupAddr + OTF_SECTION_NVME_CTRL_SIZE;
    gOTFUpdateInfo.entry[OTF_SECTION_FW_CMD].backupAddr     = gOTFUpdateInfo.entry[OTF_SECTION_ASYNC_CMD].backupAddr + OTF_SECTION_ASYNC_CMD_SIZE;

    totalSize = (U32)gOTFUpdateInfo.entry[OTF_SECTION_ASYNC_CMD].backupAddr + OTF_SECTION_ASYNC_CMD_SIZE - SHARED_SPM_REBOOT_ADDR;

    if (totalSize > SHARED_SPM_REBOOT_SIZE)   ASSERT(0);
}

//-----------------------------------------------------------------------------
/**
    NVMe relate all information backup
**/
//-----------------------------------------------------------------------------
void NvmeCmd_OTFUpdateBackup(void)
{
    HcmdQ_t* pHcmdQ = NULL;
    uint32_t index = 0;
    uint32_t bitValue;

    Asynchronous_EventBackup();
    // reset all state list, except AsycWait
    for (index = cHcsFree; index < cHcsCount; index++)
    {
        DlInit(&gHostIfMgr.stateList[index].headTail);
        gHostIfMgr.stateList[index].count = 0;
    }

    for (index = 0; index < OTF_SECTION_COUNT; index++)
    {
        MEM_COPY((void *) gOTFUpdateInfo.entry[index].backupAddr, (void *) gOTFUpdateInfo.entry[index].srcAddr, gOTFUpdateInfo.entry[index].size);
    }

    bitValue = rNvmeStatMngr.statManagerIntrVectorValidGroup[0].b.IV_VALID;
    MemCopyU32((uint32_t *) SHARED_TCM_OTF_VVG_ADDR, (uint32_t *) &bitValue, 4);
}

//-----------------------------------------------------------------------------
/**
    NVMe resotre from bckup
**/
//-----------------------------------------------------------------------------
void NvmeCmd_OTFUpdateRestore(void)
{
    OTFUpdateInfo_t* pOTFBackupInfo;
    Error_t errorCode = cEcNoError;
    uint32_t index;
    uint32_t bitValue;

    pOTFBackupInfo = (OTFUpdateInfo_t*)SHARED_SPM_REBOOT_ADDR;

    if (pOTFBackupInfo->count != gOTFUpdateInfo.count)
    {
        D_PRINTF("OTFW Cnt Err\n");
        errorCode = cEcError;
    }
    else
    {
        for (index = 0; index < OTF_SECTION_COUNT; index++)
        {
            if (pOTFBackupInfo->entry[index].size != gOTFUpdateInfo.entry[index].size)
            {
                D_PRINTF("OTFW Err Idx[%d] BK:%08X New:%08X\n", index, pOTFBackupInfo->entry[index].size, gOTFUpdateInfo.entry[index].size);
                errorCode = cEcError;
            }
        }
    }

    if (errorCode == cEcNoError)
    {
        for (index = 0; index < OTF_SECTION_COUNT; index++)
        {
            MEM_COPY((void *) gOTFUpdateInfo.entry[index].srcAddr, (void *) pOTFBackupInfo->entry[index].backupAddr, pOTFBackupInfo->entry[index].size);
        }

        Asynchronous_EventRestore();

        MemCopyU32((uint32_t *) &bitValue, (uint32_t *) SHARED_TCM_OTF_VVG_ADDR, 4);
        rNvmeStatMngr.statManagerIntrVectorValidGroup[0].b.IV_VALID = bitValue;

        NvmeHba.mcqInUseBitmap = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    NVMe reboot info dump
**/
//-----------------------------------------------------------------------------
void NvmeCmd_OTFUpdateDump(uint32_t option)
{
    uint32_t index;

    D_PRINTF("Cnt[%d]\n", gOTFUpdateInfo.count);
    for (index = 0; index < OTF_SECTION_COUNT; index ++)
    {
        D_PRINTF("[%d] SA[%08x] BA[%08X] SZ[%08d]\n", index, gOTFUpdateInfo.entry[index].srcAddr, gOTFUpdateInfo.entry[index].backupAddr, gOTFUpdateInfo.entry[index].size);
        if (option == 1)
        {
            Debug_DumpMemU32(gOTFUpdateInfo.entry[index].srcAddr,    gOTFUpdateInfo.entry[index].size);
            Debug_DumpMemU32(gOTFUpdateInfo.entry[index].backupAddr, gOTFUpdateInfo.entry[index].size);
        }
    }
}

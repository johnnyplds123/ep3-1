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
//! @brief AHCI I/F protocol service code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"
#include "SharePkg.h"
#include "HostPkg.h"
#include "RegNvmePkg.h"
#include "NvmeCmdPkg.h"

// REG
#include "RegNvmeController.h"
#include "RegArmVic.h"

// HAL
#include "HalNvmeXfer.h"
#include "HalPm.h"
#include "HalPcieLtr.h"
#include "HalIrqSrc.h"
#include "HalMc5DdrInit.h"

// APL
#include "HldNvme.h"
#include "SysInfo.h"
#include "NvmeGetLogPage.h"
#include "NvmeSecurityCmd.h"
#include "NvmeDeviceSelfTest.h"
#include "NvmeDirective.h"

#include "PLP.h"
#include "AdaptiveVoltageScaling.h"
#include "IDS.h"
#include "BcmClient.h"
#include "Cache.h"
#include "UncTable.h"
#include "SmBus.h"

// Code Section
#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
enum
{
    NVME_EVENT_NONE      = 0,
    NVME_EVENT_RESET     = BIT(0),
    NVME_EVENT_CMD       = BIT(1),
    NVME_EVENT_BUFWAIT   = BIT(2),
    NVME_EVENT_XFERDONE  = BIT(3),
    NVME_EVENT_MEDIADONE = BIT(4),
    NVME_EVENT_FUA       = BIT(5),
    NVME_EVENT_ASYNC     = BIT(6),
    NVME_EVENT_MISC      = BIT(7),
    NVME_EVENT_PLP       = BIT(8)
};

#define IDLELOOP_THRESHOLD              500
#define IDLFLUSH_THRESHOLD              20

#define NVME_INIT_FUNC_COUNT            15

/// @brief NVMe Init Mode Cnt
typedef enum {
    NVME_INIT_ONLINE = 0,
    NVME_INIT_OFFLINE,
    NVME_INIT_MODE_COUNT
} NvmeInitMode_t;

#define DRAM_PAD_CAL_TIMES        0x25
#define DRAM_TAG_TEST_START       0x4D415244
#define DRAM_TAG_TEST_FAIL        0x4D445245
#define DRAM_TAG_TUNE_START       0x41505244
#define DRAM_TAG_TUNE_END         0x534E4946

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
// RTOS Task
static          void        NvmeStartTask(void);
static          void        Nvme_ServiceLoop(void);

static          Error_t     Nvme_OneTimeInit(InitBootMode_t initMode);
static          Error_t     Nvme_SyncCPU2(InitBootMode_t initMode);
static          Error_t     Nvme_FeatureInit(InitBootMode_t initMode);
static          Error_t     Nvme_IfMgrOneTimeInit(InitBootMode_t initMode);
static          Error_t     Nvme_OneTimeInitRegIsrCtl(InitBootMode_t initMode);
static          void        Nvme_RegisterIsr(void);
static          void        Nvme_ResetComplete(Error_t error);

static __inline void        Nvme_ReleaseCmdIdx(MsgHostIO_t* pHcmdMsg);

/// Isr call back
static          void        Nvme_IsrCmd(uint32_t resetMode);
static          void        Nvme_IsrReset(uint32_t resetMode);
static          void        Nvme_IsrShutdown(uint32_t shutdownValue);
static          void        Nvme_IsrAdminXferDone(uint32_t param);
static          void        Nvme_IsrIoXferDone(uint32_t param);
static          void        Nvme_IsrIfError(uint32_t param);

// Event Sub Function
static __inline void        Nvme_DecodeIoCmdHandler(void);

// Media Done function list
static __inline void        Nvme_ProcessDataXfer(MsgHostIO_t* pHcmdMsg);
static          void        Nvme_ProcessSendStatus(MsgHostIO_t* pHcmdMsg);
static          void        Nvme_ProcessShutdown(MsgHostIO_t* pHcmdMsg);
static          void        Nvme_ProcessSmartOp(MsgHostIO_t* pHcmdMsg);
static          void        Nvme_TransferAbort(HcmdQ_t* pHcmdQ);

/// Event handler
static          void        Nvme_HandleHardwareReset(void);
static          void        Nvme_HandleResetMsg(void);
static          void        Nvme_HandleResetEvent(void);
static          void        Nvme_HandleShutdownEvent(void);
static          void        Nvme_HandleErrorEvent(void);
static __inline void        Nvme_HandleIoCmdEvent(uint32_t Cnt);
static __inline void        Nvme_HandleMediaDone(MsgHostIO_t* pHcmdMsg);
static __inline void        Nvme_HandleMediaDoneEvent(void);
static __inline void        Nvme_HandleCmdAbortEvent(void);
static __inline void        Nvme_HandleFlushEvent(void);
static          void        Nvme_HandleMiscEvent(void);
static          uint32_t    Nvme_HandleBackGroundEvent(void);
static __inline void        Nvme_HandlePowerDoneEvent(void);
static __inline void        Nvme_HandleIdleFlushEvent(void);

/// Aging Dram Test
static          void        Nvme_AgingDRAMRoutine(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
uint32_t gIdleEventTimer;
uint32_t gPcieRstRedirected = FALSE;
NfInfo_t gNfInfo;

//-----------------------------------------------------------------------------
//  Imported data prototype
//-----------------------------------------------------------------------------
extern NvmeCdb_t    sNvmeCdb;
extern NvmeCmdTbl_t sIoCmdTbl[];

//-----------------------------------------------------------------------------
//  Imported function prototype
//-----------------------------------------------------------------------------
extern void         Nvme_StartBgTask(void);
extern void         Host_DumpInfo(U32 allFlag);
extern void         NvmeSetFeatureInit(InitBootMode_t initMode);
extern void         Nvme_HandleCmdReadyEvent(void);
extern void         NvmeCmd_GetLogPageDone(HcmdQ_t* pHcmdQ);
extern void         SmartBackupData(void);
extern void         TcgInit8PwrOn(void);
extern HcmdQ_t*     GetHcmdQ(HldCmdId_t cmdId);
extern HldCmdId_t   AplHost_PopIoCommandMode0(void* pCdbBuf, bool wholeCdb);
extern Error_t      HalNvme_RequestDataTransferIoQuick(MsgHostIO_t* pHcmdMsg);
extern Error_t      HalNvme_EnableNvmeIrq(void);
extern U16          NvmeCmd_AsynchronousEventPeek(void);
extern void         Compare_ReadMediaDone(HcmdQ_t* pHcmdQ);
extern Error_t      HalIrq_InitPcieInterrupt(void);
extern uint32_t     HalNvme_IrqEnDisable(IrqId_t irqId, uint32_t state);
extern void         HandlePCIeResetIrq(void);
extern void         HandlePcieResetNonIrq(IrqHandler_t irqHandler[]);
extern void         HalDdrDll_Routine(void);

//-------------------------------------------------------------
// NVMe One Time Init Table
//-------------------------------------------------------------
Error_t (*gNvmeOneTimeInitTable[NVME_INIT_FUNC_COUNT][NVME_INIT_MODE_COUNT])(InitBootMode_t initMode) =
{
    {Host_OneTimeInit,                  NULL                        },  // Init 1
    {AplHost_OneTimeInit,               NULL                        },  // Init 2

#if (CO_SUPPORT_PLP == TRUE)
    {AplPlp_OneTimeInit,                AplPlp_OneTimeInit          },  // Init 3
#else
    {NULL,                              NULL                        },  // Init 3
#endif

#if (_MEDIA_ == MEDIA_DRAM)
    {SyIn_OneTimeInit,                  SyIn_OneTimeInit            },  // Init 4
#else
    {NULL,                              NULL                        },  // Init 4
#endif

#if (CHK_FW_TYPE(FW_DNLD) == FALSE)
    {AplLoadFirmware_OneTimeInit,       AplLoadFirmware_OneTimeInit },  // Init 5
#else
    {NULL,                              NULL                        },  // Init 5
#endif

#if (_TCG_ == TCG_NONE) && (CO_SUPPORT_BCM == TRUE)
    {BCM_OneTimeInit,                   NULL                        },  // Init 6
#else
    {NULL,                              NULL                        },  // Init 6
#endif

#if (CO_SUPPORT_READSOC == TRUE)
    {AplAVS_OneTimeInit,                NULL                        },  // Init 7
#else
    {NULL,                              NULL                        },  // Init 7
#endif

    {Nvme_SyncCPU2,                     Nvme_SyncCPU2               },  // Init 8
    {AplNamingRule_OneTimeInit,         AplNamingRule_OneTimeInit   },  // Init 9

#if (CO_SUPPORT_SMBUS == TRUE)
    {AplSmbus_OneTimeInitVpd,           AplSmbus_OneTimeInitVpd     },  // Init 10
#else
    {NULL,                              NULL                        },  // Init 10
#endif

    {Nvme_IfMgrOneTimeInit,             NULL                        },  // Init 11
    {NvmeCmd_OneTimeInit,               NULL                        },  // Init 12
    {Nvme_FeatureInit,                  NULL                        },  // Init 13
    {Nvme_OneTimeInitRegIsrCtl,         NULL                        },  // Init 14
    {UNCT_OneTimeInit,                  UNCT_OneTimeInit            }   // Init 15
};

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Start NVMe protocol service.
**/
//-----------------------------------------------------------------------------
void Nvme_StartTask(void)
{
    InitBootMode_t initMode;

    initMode = Hal_GetInitBootMode();

    gNvmeIfMgr.state = cNvmeIfPowerUp;   // Set power up state

    if (initMode != cInitBootOffLine)
    {
        HalIrq_InitPcieInterrupt();
    }

    Nvme_OneTimeInit(initMode);

    Nvme_ServiceLoop();
}

//-----------------------------------------------------------------------------
/**
    NVMe wait media done
**/
//-----------------------------------------------------------------------------
void Nvme_WaitMediaDone(void)
{
    while (IPC_MsgQFastPeek(cC2hComQueue))
    {
        BEGIN_CS1
        Nvme_HandleMediaDoneEvent();
        END_CS1
    }
}

//-----------------------------------------------------------------------------
/**
    One time initialization for Sync CPU1 init

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Nvme_SyncCPU2(InitBootMode_t initMode)
{
#if _MEDIA_!=MEDIA_DRAM
    U32 timeout = 0;

    while (smCpu0WaitCpu2Flag != SYNC_CPU0_W_CPU2_FLAG) // Wait Nand info
    {
        HalTimer_DelayUsec(10);
        timeout++;
        ASSERT(timeout < (SYNC_TIMEOUT_US / 10));
    }

    MEM_COPY(&gNfInfo,  (PVOID)(smNfInfoCpu1Addr | AXI_B0TCM_CPU1_ADDR), sizeof(NfInfo_t));

    D_PRINTF("[NVMe] SycnDone Core\n");
#endif

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
     NVMe I/F protocol and command service loop
**/
//-----------------------------------------------------------------------------
static void Nvme_ServiceLoop(void)
{
    uint32_t IoCmdCnt;
    uint32_t idleLoopCount = 0;

    D_PRINTF("[NVMe] ServiceLoop\n");
    *(volatile U32*)(0xF5011008) = 0x00000001;
    //D_PRINTF("[NVMe] SetWChannel|0x%X\n", *(volatile U32*)(0xF5011008));

    gMiscEvnet = MISC_EVENT_NONE;

#if CO_SUPPORT_DEVICE_POWER_MANAGEMENT
    gPmInfo.bypassTimerValue = 5 * KERNEL_TICK_PER_SEC;
#endif

    BEGIN_CS1
    gPcieRstRedirected = TRUE;
    VIC_SET_HANDLER(0, cIrqSourcePcieVRstN, HandlePcieResetNonIrq);    // Register PCIE Reset irq handler
    END_CS1

    while (TRUE)
    {
        // Step1. Reset/Error Hanlder
        if (gNvmeIfMgr.state != cNvmeIfReady)
        {
            Nvme_HandleResetMsg();
            Nvme_HandleHardwareReset();
            Nvme_HandleErrorEvent();
            Host_IoXferReset();
            Host_AdminCmdReset();

            if (gNvmeIfMgr.state == cNvmeIfShutdown)
            {
                Nvme_HandleShutdownEvent();
            }
            else
            {
                Nvme_HandleResetEvent();
                if (gBootNewFwFlag & BOOT_FW_AFT_RBT)    // Reboot system now
                {
                    NvmeCmd_BootFromNewFW();
                }
            }

            idleLoopCount = 0;
        }
        else
        {
            // Step2. Command Handler
            if (ADMIN_CMD_FIFO_CNT())
            {
                if (!gHostIfMgr.info.adminOverlapCnt)
                {
                    NvmeCmd_ProcessOneCommand();
                }

                idleLoopCount = 0;
            }
            else
            {
                IoCmdCnt = IO_CMD_FIFO_CNT();

                if (IoCmdCnt)
                {
                    if (!gHostIfMgr.info.adminOverlapCnt)
                    {
                        Nvme_HandleIoCmdEvent(IoCmdCnt);
                    }

                    idleLoopCount = 0;
                }
            }

            // Step3. Handle Media Done (Error Case only)
            if (IPC_MsgQFastPeek(cC2hComQueue))
            {
                Nvme_HandleMediaDoneEvent();
                idleLoopCount = 0;
            }

            // Step4. MISC Event
            if (gMiscEvnet)
            {
                Nvme_HandleMiscEvent();
                idleLoopCount = 0;
            }

            #if (CO_SUPPORT_PLP == TRUE)
            if (PLP_GET_STATUS() == PLP_DISCHARGE)
            {
                AplHost_ReleaseDataCachePLP();
                idleLoopCount = 0;
            }
            #endif

            #if (CHK_CODE_TYPE(CODE_MAIN) && CHK_FW_TYPE(FW_AGING))
                Nvme_AgingDRAMRoutine();
            #endif
        }

        if (++idleLoopCount >= IDLELOOP_THRESHOLD)
        {
            if (idleLoopCount == IDLELOOP_THRESHOLD)
            {
                gIdleEventTimer = 0;
            }

            if (Nvme_HandleBackGroundEvent())
            {
                idleLoopCount = 0;
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Register host interrupt service routine.
**/
//-----------------------------------------------------------------------------
static void Nvme_RegisterIsr(void)
{
    // NVMe ISR
    HalIrq_RegisterHandler(cIrqHostReset,       Nvme_IsrReset,            ENABLE);
    HalIrq_RegisterHandler(cIrqHostShutdown,    Nvme_IsrShutdown,         ENABLE);
    HalIrq_RegisterHandler(cIrqHostCommandIn,   Nvme_IsrCmd,              DISABLE);

    // Xfer Done ISR
    HalIrq_RegisterHandler(cIrqHostXferDone,    Nvme_IsrAdminXferDone,    ENABLE);
    HalIrq_RegisterHandler(cIrqHostIoXferDone,  Nvme_IsrIoXferDone,       ENABLE);

    // Error ISR
    HalIrq_RegisterHandler(cIrqHostIfError,     Nvme_IsrIfError,          ENABLE);

    HalTimer_SetSmartTimerInterruptFrequency(SMARTCHECK_TICK_PER_SEC);
    HalIrq_RegisterHandler(cIrqTimerSmartCheck, HalTimer_SmartCheckTimer, ENABLE);
}

//-----------------------------------------------------------------------------
/**
    One time initialization for Host sub-system.\n

    @param  initMode    initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Nvme_FeatureInit(InitBootMode_t initMode)
{
    HandleLogInfoInit(initMode);

#if CO_SUPPORT_ATA_SECURITY
    SATA_Security_Init(initMode);
#endif

    NvmeSetFeatureInit(initMode);

    if (smSysInfo->d.Header.d.SubHeader.d.NVMe.d.dwValidTag != SI_TAG_NVME)
    {
        D_PRINTF("NVMe SysInfo init\n");

        smSysInfo->d.Header.d.SubHeader.d.NVMe.d.dwValidTag = SI_TAG_NVME;
        SyIn_Synchronize(SI_AREA_NVME, SYSINFO_WRITE, SI_SYNC_BY_NVME);
    }

#if _TCG_ != TCG_NONE
    TcgInit8PwrOn();
#endif

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Nvme_HostOneTimeInit

    @param  initMode    initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Nvme_IfMgrOneTimeInit(InitBootMode_t initMode)
{
    Host_MediaJobReset();
    Host_IoXferReset();
    Host_AdminCmdReset();

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Nvme_HostOneTimeInit

    @param  initMode    initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Nvme_OneTimeInitRegIsrCtl(InitBootMode_t initMode)
{
    Nvme_RegisterIsr();
    HalNvme_EnableNvmeIrq();

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    One time initialization for Host sub-system.\n

    @param  initMode    initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Nvme_OneTimeInit(InitBootMode_t initMode)
{
    NvmeInitMode_t  nvmeInitMode;
    uint32_t        nsid;
    uint8_t         initCnt;

    if (initMode == cInitBootOffLine)
    {
        nvmeInitMode = NVME_INIT_OFFLINE;
    }
    else
    {
        nvmeInitMode = NVME_INIT_ONLINE;
    }

    for (initCnt = 0; initCnt < NVME_INIT_FUNC_COUNT; initCnt++)
    {
        if (gNvmeOneTimeInitTable[initCnt][nvmeInitMode] != NULL)
        {
             if (gNvmeOneTimeInitTable[initCnt][nvmeInitMode](initMode))
             {
                 D_PRINTF("[NVMe] Func:%d Err\n", initCnt);
             }
        }
    }

    if (initMode == cInitBootOffLine)
    {
        gNvmeIfMgr.state = cNvmeIfReady;
    }
    else if (initMode == cInitBootFwUpdated)
    {
        D_PRINTF("[NVMe] Restore\n");
        NvmeCmd_OTFUpdateRestore();

        BEGIN_CS1
        Nvme_ResetComplete(cEcNoError);
        END_CS1

        rNvmeController.controller[0].CSTS.b.NSSRO = 0;

        // Clear Init UpPate Fw Tag
        MemWriteU32(SHARED_TCM_OTF_FLG_ADDR, 0);

        if (gUpFwCmdBackup.updateFwTag == UPDATE_FW_NVME)
        {
            AplHost_SendStatus(GetHcmdId(((HcmdQ_t *)&gUpFwCmdBackup.updateFwHcmdQ)), &gUpFwCmdBackup.updateFwSdb, 1, 1);
        }
    }

    gBootNewFwFlag = FALSE;

    for (nsid = 0; nsid < gNvmeIfMgr.numOfNamespacesCreated; nsid++)
    {
        D_PRINTF("[NVMe] NS|%d LBA[0x%08X_%08X]\n", nsid, U64_TO_U32_H(gNvmeIfMgr.ns[nsid].NSZE), U64_TO_U32_L(gNvmeIfMgr.ns[nsid].NSZE));
    }

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Completion of NVMe reset.
      - ready to service host command.

    @param  error   error code
**/
//-----------------------------------------------------------------------------
static void Nvme_ResetComplete(Error_t error)
{
    gNvmeIfMgr.state = cNvmeIfReady;   // Ready to service

    AplHost_ResetComplete(error);
}

//-----------------------------------------------------------------------------
/**
    NVMe IO command handler.
**/
//-----------------------------------------------------------------------------
static __inline void Nvme_DecodeIoCmdHandler(void)
{
    HcmdQ_t* pHcmdQ = NULL;

    // Overall command info
    HldCmdId_t cmdId;
    NvmeCdb_t* pCdb;
    NvmeSdb_t* pSdb;

    // R/W command decode
    uint32_t nsId0;
    NvmeIoRwCommand_t* pCdb_rw;

    // Step1. Fetch command
    {
        pCdb  = &sNvmeCdb;
        cmdId = AplHost_PopIoCommandMode0(pCdb, FALSE);
        // no command is fetched, return false
        if (cmdId == HLD_INVALID_CID)
        {
            return;
        }
        else
        {
            pHcmdQ = GetHcmdQ(cmdId);
            pHcmdQ->error = cEcNoError;
            pSdb = NvmeCmd_GetSdbPointer(pHcmdQ);
            pSdb->DW0 = 0;         // DW0 = 0
            pSdb->DW3 = pCdb->CID; // DW3 CID
            gHostIfMgr.info.cmdFetchCnt++;
        }
    }

    // Step 2. Handle misc jobs
    {
        // Enable I/O busy Timer check
        gSmartIOTimerFlag = TRUE;

        // Release stream buffer if necessary
    #if (SUP_DIRE_STREAMS == ENABLE)
        if (pCdb->OPC != cWrite)
        {
            rNvmeCmdFetch.cmdFetcherAxiBufCntrDecrease.b.CMDF_BUF_CNTR_DEC = 1;
            gCmdfPtr++;
        }
    #endif
    }

    // Step3. Decode command
    {
        if (pCdb->OPC<cNvmeIoCmdTableSize)
        {
            pHcmdQ->fptr     = sIoCmdTbl[pCdb->OPC].fptr;
            pHcmdQ->attr.all = sIoCmdTbl[pCdb->OPC].attr;
        }
        else // invalid I/O command
        {
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidCommandOpcode, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            pHcmdQ->attr.all = 0;

            gNvmeIfMgr.lastErrorBitOffset  = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW0_OFFSET; // DW0, Op code
            goto Decode_Error;
        }

        if (pCdb->NSID == 0 || pCdb->NSID > NVME_NUM_OF_NAMESPACES || !gNvmeIfMgr.ns[pCdb->NSID - 1].Attach)
        {
            if (!((pCdb->NSID == cNvmeGlobalNamespaceId) && (pHcmdQ->attr.all & ATTR_GLOBAL_NS)))
            {
                D_PRINTF("Nsid Err:%X]!!\n", pCdb->NSID);

                pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidNamespaceOrFormat, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);

                gNvmeIfMgr.lastErrorBitOffset  = 0;
                gNvmeIfMgr.lastErrorByteOffset = ERR_DW1_OFFSET; // DW1, NSID
                goto Decode_Error;
            }
        }

        if (pHcmdQ->attr.all & ATTR_LBA64)
        {
            pCdb_rw = (NvmeIoRwCommand_t *)pCdb;

            nsId0 = pCdb_rw->NSID - 1; // convert to zero based NSID

            pHcmdQ->addrInfo.lba          = pCdb_rw->SLBA;
            pHcmdQ->addrInfo.allFlags     = 0;
            pHcmdQ->addrInfo.nb           = pCdb_rw->NLB + 1; // NLB is zero based value
            pHcmdQ->addrInfo.nsId0        = nsId0;
            pHcmdQ->addrInfo.fua          = pCdb_rw->FUA;
            pHcmdQ->addrInfo.limitedRetry = pCdb_rw->LR;

            #if _CUSTOMER_ == CUSTOMER_MICROSOFT
            // I/O Unalign 4KB, except write uncorrectble cmd(0x04)
            if ((pCdb_rw->OPC != 0x04) && (pHcmdQ->addrInfo.lba & HLBU_ALIGN_4KB))
            {
                if (gNvmeIfMgr.UnalignCnt != U64_MAX)
                {
                    gNvmeIfMgr.UnalignCnt++;
                }
            }
            #endif

        #if CO_FUA_WRITE
            // FUA Case
            if (pHcmdQ->attr.b.xferDir == cXferH2d)
            {
                if ((pHcmdQ->addrInfo.fua == TRUE) || (gNvmeIfMgr.writeCacheEnable == FALSE))
                {
                    pHcmdQ->attr.b.autoEoC = FALSE;
                }
            }
        #endif

        #if CO_CHECK_PRP_FEATURE_SUPPORT
            // Check PRP alignmen
            if ((pCdb->PRP1_SGL_LO & 0x03) || (pCdb->PRP2_SGL_LO & 0x03))
            {
                D_PRINTF("PRP Err PRP1[%08X], PRP2[%08X]\n", pCdb->PRP1_SGL_LO, pCdb->PRP2_SGL_LO);

                pHcmdQ->error = MK_NVME_ERROR_CODE(cDataTransferError, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);

                gNvmeIfMgr.lastErrorBitOffset  = 0;
                gNvmeIfMgr.lastErrorByteOffset = ERR_DW6_OFFSET; // error parameter in PRP
                goto Decode_Error;
            }
        #endif

            // Check HLBA out of range
            if ( ((pHcmdQ->addrInfo.lba + pHcmdQ->addrInfo.nb) > gNvmeIfMgr.ns[nsId0].NSZE) || (pHcmdQ->addrInfo.lba > gNvmeIfMgr.ns[nsId0].NSZE) )
            {
                D_PRINTF("Range Err lba[%08X], nb[%08X]\n", pHcmdQ->addrInfo.lba, pHcmdQ->addrInfo.nb);

                pHcmdQ->error = MK_NVME_ERROR_CODE(cLbaOutOfRange, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);

                gNvmeIfMgr.lastErrorBitOffset  = 0;
                gNvmeIfMgr.lastErrorByteOffset = (pCdb_rw->SLBA >= gNvmeIfMgr.ns[nsId0].NSZE) ? ERR_DW10_OFFSET : ERR_DW12_OFFSET; // error parameter, SLBA or NLB ?
                goto Decode_Error;
            }

            // Check maximum data transfer size
            if ((pHcmdQ->addrInfo.nb > BYTE2LBU(NVME_MAX_DATA_TRANSFER_SIZE)) && (!(pHcmdQ->attr.all & ATTR_MDTS)))
            {
                D_PRINTF("MDTS Err[%08X]\n", pHcmdQ->addrInfo.nb);

                pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);

                gNvmeIfMgr.lastErrorBitOffset  = 0;
                gNvmeIfMgr.lastErrorByteOffset = ERR_DW12_OFFSET; // error parameter in NLB
                goto Decode_Error;
            }

            // Convert LBA with nsid and lba formt
            {
                pHcmdQ->addrInfo.lba += (gNvmeIfMgr.ns[nsId0].mbu_start << cLBAFTable[gNvmeIfMgr.ns[nsId0].LBAF].mbu_shift);
            }

            if (pHcmdQ->attr.all & ATTR_UNC && smUncTblFlag)
            {
                if (pHcmdQ->attr.b.xferDir == cXferD2h)
                {
                    if (UNCT_CHECK_DO_SEARCH(pHcmdQ->addrInfo.lba << gNvmeIfMgr.ns[nsId0].hbu_shift, pHcmdQ->addrInfo.nb << gNvmeIfMgr.ns[nsId0].hbu_shift))
                    {
                        pHcmdQ->error = MK_NVME_ERROR_CODE(cUnrecoveredReadError, cMediaErrors, cNoMoreStatusInformation, cDoNotRetry);
                    }
                }
                else
                {
                    UNCT_CHECK_DO_DELETE(pHcmdQ->addrInfo.lba << gNvmeIfMgr.ns[nsId0].hbu_shift, pHcmdQ->addrInfo.nb << gNvmeIfMgr.ns[nsId0].hbu_shift, ENABLE);
                }
            }
        }
    }

    #if CO_SUPPORT_ATA_SECURITY
    if (pHcmdQ->error == cEcNoError)
    {
        pHcmdQ->error = Nvme_CMD_CheckSecurity(0, pCdb->OPC);  // 0:IoCmd
    }
    #endif

Decode_Error:

    // Final Step (call back function or return status
    {
        // execute function if no error.
        if (pHcmdQ->error == cEcNoError)
        {
            pHcmdQ->fptr(pHcmdQ);
        }

        if (pHcmdQ->error != cEcNoError)
        {
            NvmeCmd_Complete(pHcmdQ);
        }

        gNvmeIfMgr.LastNvmeCmd = 0xFF;  // 0xFF:IoCmd
    }
}

//-----------------------------------------------------------------------------
/**
    Check enter power down condition
**/
//-----------------------------------------------------------------------------
static U32 Nvme_IsSafeToPowerDown(void)
{
    U32 x;

    if ( gPmInfo.bypassTimerValue
     || (CIABM_FREE_CNT() != HW_MAX_IO_CMD_INDEX)
     || (ADMIN_CMD_FIFO_CNT())
     //|| (IO_CMD_FIFO_CNT())
     || (gNvmeIfMgr.state != cNvmeIfReady)
     || (gHostIfMgr.info.mediaReqJobCnt != gHostIfMgr.info.mediaReqDoneCnt)
     || (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] != FIO_MAX_JOB)
     || (smCacheMgr.ssdCnt[SSD_POOL_FREE] != CACHE_RW_COUNT))
    {
        return FALSE;
    }

    for (x = 0; x < cMsgPoolLast; x++)
    {
        if (IPC_MsgQFastPeek(x)) return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/**
    Transfer abort handler
**/
//-----------------------------------------------------------------------------
static void Nvme_TransferAbort(HcmdQ_t* pHcmdQ)
{
    MsgHostIO_t* pHcmdMsg;
    uint16_t     cmdIndex;
    uint32_t     GroupId;
    uint32_t     Bitmap;

    pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);
    cmdIndex = pHcmdQ->index;
    GroupId  = cmdIndex >> 5;
    Bitmap   = BIT(cmdIndex & 0x1F);

    HalNvme_IoCmdAbort(cmdIndex, GroupId, Bitmap);

    BEGIN_CS1
    Host_IoXferDone();
    BIT_CLR(ACTIVATED_DW_FW(GroupId), Bitmap);          // Clear FW CABM
    pHcmdMsg->bufDesc.cssTemp = CACHE_ABORT;
    NvmeCmd_ReturnCacheMsg(pHcmdQ);
    pHcmdMsg->laa = DWORD_MASK;
    pHcmdQ->error = MK_NVME_ERROR_CODE(cUnrecoveredReadError, cMediaErrors, cNoMoreStatusInformation, cDoNotRetry);
    END_CS1

    NvmeCmd_Complete(pHcmdQ);
}

//-----------------------------------------------------------------------------
/**
    Nvme_ReleaseCmdIdx
**/
//-----------------------------------------------------------------------------
static __inline void Nvme_ReleaseCmdIdx(MsgHostIO_t* pHcmdMsg)
{
    HcmdQ_t* pHcmdQ;
    uint32_t cmdIndex;

    pHcmdQ   = GetHcmdQuePtr(pHcmdMsg);
    cmdIndex = GetHcmdId(pHcmdQ);

    BEGIN_CS1
    BIT_CLR(ACTIVATED_DW_FW(cmdIndex >> 5), BIT(cmdIndex & 0x1F));
    pHcmdMsg->bufDesc.cssTemp = CACHE_ABORT;
    NvmeCmd_ReturnCacheMsg(pHcmdQ);
    END_CS1
}

//-----------------------------------------------------------------------------
/**
    Reset Interrupt service routine.
     - Make Host I/F state reset.
     - Set event for deferred Reset process.

    @param[in]  resetMode  reset mode (HARD or SOFT)

**/
//-----------------------------------------------------------------------------
static void Nvme_IsrReset(uint32_t resetMode)
{
    M_PRINTF("[RSET] Mode: %1X\n", resetMode);
    if (resetMode >= cNvmeSubsystemReset)
    {
        gHostIfMgr.info.hardResetCnt++;
        gNvmeIfMgr.state = cNvmeIfSubsystemReset;

        if (resetMode == cNvmeSubsystemReset)
        {
            gResetFlag |= BIT(cNvmeSubsystemReset);
        }
        else
        {
            gResetFlag |= BIT(cNvmePCIeReset);
        }
    }
    else
    {
        gHostIfMgr.info.softResetCnt++;
        gNvmeIfMgr.state = cNvmeIfControllerReset;

        if (resetMode == cNvmeControllerResetClr)
        {
            gResetFlag |= BIT(cNvmeControllerResetClr);
        }
        else
        {
            gResetFlag |= BIT(cNvmeControllerResetSet);
        }
    }

    //Kernel_SetEventFlags(EVENT_OBJ_HOST, EVENT_HOST_RESET|EVENT_HOST_XFER_ABORTED);
}

//-----------------------------------------------------------------------------
/**
    Nvme commad in interrupt callback function

**/
//-----------------------------------------------------------------------------
static void Nvme_IsrCmd(uint32_t resetMode)
{
    HalNvme_IrqEnDisable(cIrqHostCommandIn, DISABLE);
}

//-----------------------------------------------------------------------------
/**
    Shutdown notification Interrupt service routine.

    @param[in]  shutdownValue  shutdown notification value (1=normal, 2=Abrupt)

**/
//-----------------------------------------------------------------------------
static void Nvme_IsrShutdown(uint32_t shutdownValue)
{
    gNvmeIfMgr.state = cNvmeIfShutdown;
}

//-----------------------------------------------------------------------------
/**
    Firmware data transfer request done interrupt service routine

    @param[in]  param  Transfer done call back parameter (Host command queue pointer)
**/
//-----------------------------------------------------------------------------
static void Nvme_IsrAdminXferDone(uint32_t param)
{
    HcmdQ_t* pHcmdQ;

    pHcmdQ = (HcmdQ_t *)param;

    if (pHcmdQ==NULL)
    {
        return;
    }

    if (pHcmdQ->attr.b.xferNotify)
    {
        pHcmdQ->attr.b.xferNotify=FALSE;
    }

    // call transfer done call back function
    if (pHcmdQ->fptr != NULL)
    {
        pHcmdQ->fptr(pHcmdQ);
    }

    // release command queue if EoC is handled by HW.
    if (pHcmdQ->attr.b.autoEoC)
    {
        Host_CommandComplete(pHcmdQ);
    }
}

//-----------------------------------------------------------------------------
/**
    Firmware data transfer request done interrupt service routine

    @param[in]  param  Transfer done call back parameter (Host command queue pointer)
**/
//-----------------------------------------------------------------------------
static void Nvme_IsrIoXferDone(uint32_t param)
{
    HcmdQ_t* pHcmdQ;

    pHcmdQ = (HcmdQ_t *)param;

    if (pHcmdQ==NULL)
    {
        return;
    }

    // call transfer done call back function
    if (pHcmdQ->fptr != NULL)
    {
        pHcmdQ->fptr(pHcmdQ);
        Host_IoXferDone();
    }

    // release command queue if EoC is handled by HW.
    if (pHcmdQ->attr.b.autoEoC)
    {
        Host_CommandComplete(pHcmdQ);
    }
}

//-----------------------------------------------------------------------------
/**
    NVMe interface error exception interrupt service routine.
     - Catch the errored command queue.
     - Move to error state if I/F error is not recoverable.

    @param[in]  errorInfo  error Information pointer
**/
//-----------------------------------------------------------------------------
static void Nvme_IsrIfError(uint32_t errorInfo)
{
    HostErrorInfo_t* pErrorInfo;

    pErrorInfo = (HostErrorInfo_t*)errorInfo;

    E_PRINTF(pErrorInfo->error, "[IF_ERR][CID:%4X]\n", pErrorInfo->cmdId);

    if (IS_NVME_ASYNC_ERROR_CODE(pErrorInfo->error))
    {
        HcmdQ_t* pHcmdQ;

        pHcmdQ = Host_PopFromHead(cHcsAsyncWait);
        if (pHcmdQ != NULL)
        {
            HandleSaveNoSpecErrLog();
            if (gErrLogSysInfoUpdate == FALSE)
            {
                gErrLogSysInfoUpdate = TRUE;
                SetHcmdHandler((&gSysInfoCmdQ), HandleErrLogUpdateSysInfo);
                Host_PushToTail(cHcsCmdWait, &gSysInfoCmdQ);
            }

            pHcmdQ->error    = pErrorInfo->error;
            pHcmdQ->asycType = cErrorStatus;
            Host_PushToTail(cHcsAsyncReport, pHcmdQ); // move to ready list to notify asynchronous Event
        }
        else
        {
            gNvmeIfMgr.queuedError = pErrorInfo->error; // deferred error status notification
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Handle media done job : Data Xfer (Read, Write)
**/
//-----------------------------------------------------------------------------
__inline void Nvme_ProcessDataXfer(MsgHostIO_t* pHcmdMsg)
{
    HcmdQ_t* pHcmdQ;

    if (pHcmdMsg->error != cEcNoError)      // (1) NVMe Reset
    {
        pHcmdQ   = GetHcmdQuePtr(pHcmdMsg);
        Nvme_ReleaseCmdIdx(pHcmdMsg);

        if (pHcmdMsg->error != cEcNoError)
        {
            //D_PRINTF("Media Done Error, FwCabm[%02d]_MAP[%08x]\n", GroupId, Bitmap);
            if (pHcmdQ->attr.b.xferDir == cXferD2h)
            {
                pHcmdQ->error = MK_NVME_ERROR_CODE(cUnrecoveredReadError, cMediaErrors, cNoMoreStatusInformation, cDoNotRetry);
            }
            else
            {
                pHcmdQ->error = MK_NVME_ERROR_CODE(cWriteFault, cMediaErrors, cNoMoreStatusInformation, cDoNotRetry);
            }

            NvmeCmd_Complete(pHcmdQ);
            smSysInfo->d.LogInfo.d.SmrLog.medAndDataErrCnt++;
        }
    }
    else if (pHcmdMsg->hdr.b.forceAbort)                                           // (2) CMD Abort
    {
        pHcmdQ = GetHcmdQuePtr(pHcmdMsg);

        gHostIfMgr.info.currAbortCnt--;

        if (pHcmdMsg->bufDesc.css == CACHE_R_IDS)
        {
            pHcmdMsg->bufDesc.prpRdy = cMcPrpError;
        }

        Nvme_ReleaseCmdIdx(pHcmdMsg);
        pHcmdQ->error = MK_NVME_ERROR_CODE(cCommandAbortRequested, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
        NvmeCmd_Complete(pHcmdQ);
    }
    else                                                                            // (3) Data Xfer
    {
        if (pHcmdMsg->bufDesc.css == CACHE_R_IDS)
        {
            pHcmdMsg->bufDesc.flowCtrl = cXferIds;
        }

        if (HalNvme_RequestDataTransferIoQuick(pHcmdMsg) == cEcNoError)
        {
            Host_IoXferReq();
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Handle media done job : Send Status (Flush, FUA, TRIM)
**/
//-----------------------------------------------------------------------------
static void Nvme_ProcessSendStatus(MsgHostIO_t* pHcmdMsg)
{
    HcmdQ_t* pHcmdQ;

    pHcmdQ = GetHcmdQuePtr(pHcmdMsg);

    pHcmdQ->error = pHcmdMsg->error;

    if (pHcmdMsg->hdr.b.forceAbort)
    {
        gHostIfMgr.info.currAbortCnt--;
    }

    if (pHcmdMsg->hdr.b.opCode == cMcFormat)
    {
        Host_AdminCmdOverlapDone();

        #if CO_SUPPORT_ATA_SECURITY
        MediaEraseAllRequestDone(pHcmdQ);
        #endif
    }
    else if (pHcmdMsg->hdr.b.opCode == cMcCompareCmd)
    {
        #if CO_SUPPORT_COMPARE
        Compare_ReadMediaDone(pHcmdQ);
        #endif
    }

    NvmeCmd_Complete(pHcmdQ);
}

//-----------------------------------------------------------------------------
/**
    Handle media done job : Update Shutdown Information
**/
//-----------------------------------------------------------------------------
static void Nvme_ProcessShutdown(MsgHostIO_t* pHcmdMsg)
{
    smSysInfo->d.LogInfo.d.LogFlags.unsafeFlag = FALSE;
    SmartBackupData();

    M_PRINTF("Shutdown Complete\n");
    HalNvme_ShutdownComplete();

    gPmInfo.bypassTimerValue = 5 * KERNEL_TICK_PER_SEC;
}

//-----------------------------------------------------------------------------
/**
    Handle media done job : Update Shutdown Information
**/
//-----------------------------------------------------------------------------
static void Nvme_ProcessSmartOp(MsgHostIO_t* pHcmdMsg)
{
    HcmdQ_t* pHcmdQ;
    MsgSmart_t*  pSmartMsg;

    pSmartMsg = (MsgSmart_t*)pHcmdMsg;

    switch (pSmartMsg->subOp)
    {
#if CO_SUPPORT_DEVICE_SELF_TEST
        case cSmrSelfTestOp:
            pHcmdQ = GetHcmdQuePtr(pHcmdMsg);
            pHcmdQ->error = pHcmdMsg->error;
            if (pHcmdQ->error != cEcNoError)
            {
                gCurrDSTOperation = 0;
                gCurrDSTCompletion = 0;
            }
            NvmeCmd_Complete(pHcmdQ);
            break;
#endif
        case cSmrAttri:
            pHcmdQ = GetHcmdQuePtr(pHcmdMsg);
            pHcmdQ->error = pHcmdMsg->error;
            Host_AdminCmdOverlapDone();

            SetHcmdHandler(pHcmdQ, NvmeCmd_GetLogPageDone);
            Host_PushToTail(cHcsCmdWait, pHcmdQ);

            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------
/**
    Handle Controller Reset Event
**/
//-----------------------------------------------------------------------------
static void Nvme_HandleHardwareReset(void)
{
    BEGIN_HOST_DBG_DUMP(DBG_DUMP_ERRHANDLE)
        D_PRINTF("[RSET] (1)Reset HW\n");
    END_HOST_DBG_DUMP

    BEGIN_CS1
    if (gResetFlag & BIT(cNvmePCIeReset))
    {
        HandlePCIeResetIrq();
        gResetFlag = FALSE;
    }

    if (gResetFlag & BIT(cNvmeControllerResetClr))
    {
        HalNvme_ResetController();
        gResetFlag &= ~BIT(cNvmeControllerResetClr);
    }

    if (gResetFlag & BIT(cNvmeControllerResetSet))
    {
        // Initialize NVMe Controller Per Queue after PCIe setup
        HalNvme_InitAdminQueue();

        // Ready to process commands
        if ((gBootNewFwFlag & BOOT_FW_AFT_RST) || (gBootNewFwFlag & BOOT_FW_AFT_SUBRST))
        {
            gBootNewFwFlag = BOOT_FW_AFT_RBT;
        }
        else
        {
            HalNvme_SetReadyState();
        }

        gResetFlag &= ~BIT(cNvmeControllerResetSet);
    }

    if (gResetFlag & BIT(cNvmeSubsystemReset))
    {
        uint32_t bNvmeCrlInitCnt;

        for (bNvmeCrlInitCnt = 0; bNvmeCrlInitCnt < cNvmeNumOfFuncSupport; bNvmeCrlInitCnt++)
        {
            rNvmeController.controller[bNvmeCrlInitCnt].NSSR = 0;
        }

        HalNvme_ResetSubsystem();
        if (gBootNewFwFlag & BOOT_FW_AFT_RST)
        {
            gBootNewFwFlag = BOOT_FW_AFT_SUBRST;
        }

        gResetFlag &= ~BIT(cNvmeSubsystemReset);
    }
    END_CS1
}

//-----------------------------------------------------------------------------
/**
    Error Reset Msg.
**/
//-----------------------------------------------------------------------------
static void Nvme_HandleResetMsg(void)
{
    MsgHostIO_t* pMsg;

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_ERRHANDLE)
        D_PRINTF("[RSET] NVME:Currnet State = %d\n", gNvmeIfMgr.state);
        D_PRINTF("[RSET] (0)Reset Msg\n");
    END_HOST_DBG_DUMP

    pMsg = (MsgHostIO_t*) Ipc_GetExtendMsg(cMcHostReset);
    Host_MediaReq();
    IPC_SendMsgQ(cH2cReqQueue, (Msg_t*)pMsg);

    // Wait All media job done --> All cache read/ IDS read are done here, cought by IPC
    while (!Host_MediaAllDone())
    {
        if (IPC_MsgQFastPeek(cC2hComQueue))
        {
            pMsg = (MsgHostIO_t*)IPC_GetMsgQ(cC2hComQueue);

            if (pMsg->error != cEcIdsReadFail)
            {
                Host_MediaDone();
            }

            if (pMsg->hdr.b.opCode == cMcHostReset)
            {
                pMsg->hdr.b.status = cMsgCompleted;
                break;
            }

            if (pMsg->error == cEcIdsReadFail)     // For IDS error with trk_err case only
            {
                D_PRINTF("resetMsg :%d\n", pMsg->attrInfo.b.cmdId);
                if ((pMsg->hdr.b.status >= cMsgExecuted) &&
                    (ARRAY_INDEX_FROM_PTR(smHcmdMsg, pMsg) < MAX_HCMD_Q_COUNT))
                {
                    Nvme_ReleaseCmdIdx(pMsg);
                }
            }
            else
            {
                Nvme_HandleMediaDone(pMsg);
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Host reset event handler.
**/
//-----------------------------------------------------------------------------
static void Nvme_HandleResetEvent(void)
{
    ResetMode_t resetMode;

    resetMode = cResetHard;

    BEGIN_CS1
    NvmeCmd_Reset(resetMode);
    Host_Reset(cHcIniReset);      // Reset Base class

    #if CO_SUPPORT_DEVICE_SELF_TEST
    DST_Completion(cDSTAbortReset);
    #endif

    if ((!(gBootNewFwFlag > BOOT_FW_NOW)) && (gResetFlag == FALSE))
    {
        Nvme_ResetComplete(cEcNoError);
    }
    END_CS1
}

//-----------------------------------------------------------------------------
/**
    Host shutdown event handler.
**/
//-----------------------------------------------------------------------------
static void Nvme_HandleShutdownEvent(void)
{
    Msg_t* pMsg;

    pMsg = (Msg_t*) Ipc_GetExtendMsg(cMcShutdown);
    MSG_SET_HOST_INIT(pMsg, cMcShutdown);
    Host_MediaReq();
    IPC_SendMsgQ(cH2cReqQueue,  pMsg);

    BEGIN_CS1
    if (gResetFlag == FALSE)
    {
        gNvmeIfMgr.state = cNvmeIfReady;
    }
    END_CS1
}

//-----------------------------------------------------------------------------
/**
    Error Event handler.
**/
//-----------------------------------------------------------------------------
static void Nvme_HandleErrorEvent(void)
{
#if _MEDIA_ >= MEDIA_NAND
    uint32_t idx;
    uint32_t cnt = 0;
    HcmdQ_t* pHcmdQ;
    HcmdStateId_t  state;

    AplHost_ReleaseDataCache();

BEGIN_HOST_DBG_DUMP(DBG_DUMP_ERRHANDLE)
    D_PRINTF("[RSET] (2)Return Cahce Done\n");
END_HOST_DBG_DUMP

    cnt = 0;
    while (cnt != (HW_MAX_IO_CMD_INDEX / 32))
    {
        cnt = 0;
        for (idx = 0; idx < (HW_MAX_IO_CMD_INDEX / 32); idx++)
        {
            if (NvmeHba.bitmapFwCabm[idx] == 0)
            {
                cnt ++;
            }
        }
    }

BEGIN_HOST_DBG_DUMP(DBG_DUMP_ERRHANDLE)
    D_PRINTF("[RSET] (3)Xfer All Done\n");
END_HOST_DBG_DUMP

    for (idx = cHcsFree; idx < cHcsCount; idx ++)
    {
        D_PRINTF("[RSET] Link List [%02d], Cnt[%04d]\n", idx, gHostIfMgr.stateList[idx].count);
    }

    // Pop from each wait handle link list and return cnt code
    if (gNvmeIfMgr.state == cNvmeIfShutdown)
    {
        for (state = cHcsCmdWait; state <= cHcsCmdAbort; state ++)
        {
            if ((state != cHcsAsyncWait) && (state != cHcsAsyncReport))
            {
                while (gHostIfMgr.stateList[state].count != 0)
                {
                    pHcmdQ = Host_PopFromHead(state);
                    if (pHcmdQ == NULL)
                    {
                        break;
                    }
                    pHcmdQ->error = MK_NVME_ERROR_CODE(cCommandsAbortedDueToPowerLossNotification, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
                    NvmeCmd_Complete(pHcmdQ);
                }
            }
        }
    }
    else
    {
        Host_Reset(cHcIniReset);
    }

BEGIN_HOST_DBG_DUMP(DBG_DUMP_ERRHANDLE)
    D_PRINTF("[RSET] (4)Link List All Complete\n");
END_HOST_DBG_DUMP


    if (gNvmeIfMgr.state != cNvmeIfShutdown)
    {
        D_PRINTF("[RSET] (5)Reset HWLL\n");
        HalNvme_ResetHwLinkList();
    }
#endif
}

//-----------------------------------------------------------------------------
/**
    IO Command In Event handler.
**/
//-----------------------------------------------------------------------------
static __inline void Nvme_HandleIoCmdEvent(uint32_t Cnt)
{
    // If have FUA command don't handle new command (otherwise cache can't flush complete)
    if (gHostIfMgr.stateList[cHcsFlushWait].count)
    {
        return;
    }

#if (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED)
    HalPcieLtr_CommandActive();
#endif

    BEGIN_CS1
    if (NvmeHba.ready)
    {
        while (Cnt)
        {
            Nvme_DecodeIoCmdHandler();

            if (IPC_MsgQFastPeek(cC2hComQueue))
            {
                Nvme_HandleMediaDoneEvent();
            }
            Cnt --;
        }
    }
    END_CS1
}

//-----------------------------------------------------------------------------
/**
    Handle CMDs media done.
**/
//----------------------------------------------------------------------------
static __inline void Nvme_HandleMediaDone(MsgHostIO_t* pHcmdMsg)
{
    if (pHcmdMsg->hdr.b.opCode <= cMcWrite)
    {
        Nvme_ProcessDataXfer(pHcmdMsg);
    }
    else
    {
        switch (pHcmdMsg->hdr.b.opCode)
        {
            case cMcTrim:
            case cMcFlush:
            case cMcFuaFlush:
            case cMcTcg:
            case cMcFormat:
            case cMcWriteZero:
            case cMcCompareCmd:
                Nvme_ProcessSendStatus(pHcmdMsg);
                break;
            case cMcShutdown:
                Nvme_ProcessShutdown(pHcmdMsg);
                break;
            case cMcSmartOp:
                Nvme_ProcessSmartOp(pHcmdMsg);
                break;
            default:
                D_PRINTF("OP[%d]\n", pHcmdMsg->hdr.b.opCode);
                ASSERT(0);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    IO Command In Event handler.
**/
//-----------------------------------------------------------------------------
static __inline void Nvme_HandleMediaDoneEvent(void)
{
    MsgHostIO_t* pHcmdMsg;

    BEGIN_CS1
    pHcmdMsg = (MsgHostIO_t*) IPC_GetMsgQ(cC2hComQueue);

    if (pHcmdMsg->error == cEcIdsReadFail)     // For IDS error with trk_err case only
    {
        HcmdQ_t* pHcmdQ;

        pHcmdQ = GetHcmdQuePtr(pHcmdMsg);
        SetHcmdHandler(pHcmdQ, Nvme_TransferAbort);
        Host_PushToTail(cHcsCmdWait, pHcmdQ);
    }
    else
    {
        Host_MediaDone();
        Nvme_HandleMediaDone(pHcmdMsg);
    }

    END_CS1
}

//-----------------------------------------------------------------------------
/**
    Command abort event handler.
**/
//-----------------------------------------------------------------------------
static __inline void Nvme_HandleCmdAbortEvent(void)
{
    do
    {
        Nvme_TransferAbort(Host_PopFromHead(cHcsCmdAbort));
    } while (gHostIfMgr.stateList[cHcsCmdAbort].count);
}

//-----------------------------------------------------------------------------
/**
    Flush Event handler. (Both FUA write or flush)
**/
//-----------------------------------------------------------------------------
static __inline void Nvme_HandleFlushEvent(void)
{
    HcmdQ_t* pHcmdQ;
    MsgHostIO_t* pHcmdMsg;

    pHcmdQ = Host_PopFromHead(cHcsFlushWait);
    pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);

    // Flush event could't call MSG_SET_HOST_INIT()
    if (pHcmdMsg->hdr.b.opCode == cMcWrite)
    {
        pHcmdMsg->hdr.b.opCode = cMcFuaFlush;
    }

    pHcmdMsg->error = cEcNoError;
    pHcmdMsg->hdr.b.status = cMsgPosted;
    Host_MediaReq();
    IPC_SendMsgQ(cH2cReqQueue, (Msg_t*)pHcmdMsg);
}

//-----------------------------------------------------------------------------
/**
    Command ready list fill event handler.
      - Pop a ready queue and execute callback function.
**/
//-----------------------------------------------------------------------------
void Nvme_HandleCmdReadyEvent(void)
{
    HcmdQ_t* pHcmdQ;

    pHcmdQ = Host_PopFromHead(cHcsAsyncReport);

    ASSERT(pHcmdQ!=NULL);

    pHcmdQ->fptr(pHcmdQ);
}

//-----------------------------------------------------------------------------
/**
    Handle Command wait event
      - Pop a ready queue and execute callback function.
**/
//-----------------------------------------------------------------------------
static __inline void Nvme_HandleCmdWaitEvent(void)
{
    HcmdQ_t* pHcmdQ;

    pHcmdQ = Host_PopFromHead(cHcsCmdWait);

    if (pHcmdQ != NULL)
    {
        pHcmdQ->fptr(pHcmdQ);
    }
}

//-----------------------------------------------------------------------------
/**
    MISC Event handler.
**/
//-----------------------------------------------------------------------------
static void Nvme_HandleMiscEvent(void)
{
    if (gHostIfMgr.stateList[cHcsCmdAbort].count)
    {
        Nvme_HandleCmdAbortEvent();
    }
    else
    {
        gMiscEvnet &= ~MISC_EVENT_ABORT;
    }

    if (gHostIfMgr.stateList[cHcsFlushWait].count)
    {
        Nvme_HandleFlushEvent();
    }
    else
    {
        gMiscEvnet &= ~MISC_EVENT_FLUSH;
    }

    if (gHostIfMgr.stateList[cHcsAsyncReport].count)
    {
        Nvme_HandleCmdReadyEvent();
    }
    else
    {
        gMiscEvnet &= ~MISC_EVENT_ASYNC;
    }

    if (gHostIfMgr.stateList[cHcsCmdWait].count)
    {
        Nvme_HandleCmdWaitEvent();
    }
    else
    {
        gMiscEvnet &= ~MISC_EVENT_CMDWAIT;
    }
}

//-----------------------------------------------------------------------------
/**
    Power Done event handler.
      - Pop a ready queue and execute callback function.
**/
//-----------------------------------------------------------------------------
static __inline void Nvme_HandlePowerDoneEvent(void)
{
    if (Nvme_IsSafeToPowerDown())
    {
        U32 idx, powerState = POWER_STATE_0;
        CpuCs_t orgState;

        if (currentFeatureValue[cAutonomousPowerStateTransition])
        {
            for (idx = 0; idx < NVME_NUM_OF_POWER_STATES; idx++)
            {
                if ((gNvmeIfMgr.AutoPST[idx].IdleMs) && (gIdleEventTimer >= gNvmeIfMgr.AutoPST[idx].IdleMs))
                {
                    powerState = gNvmeIfMgr.AutoPST[idx].PS;
                }
            }
        }
        else
        {
            powerState = currentFeatureValue[cPowerManagement];
        }

        switch (powerState)
        {
            case POWER_STATE_0:
                break;
            case POWER_STATE_1:
                break;
            case POWER_STATE_2:
                break;
            case POWER_STATE_3:
                BEGIN_CS(orgState);
                if (Nvme_IsSafeToPowerDown())
                {
                    M_PRINTF("_");
                    HalPm_EnterExitPowerDownMode();
                }
                END_CS(orgState);

                break;
            case POWER_STATE_4:
                BEGIN_CS(orgState);
                if (Nvme_IsSafeToPowerDown() && HalPm_ReadyToEnterDeepPowerDownMode())
                {
                    M_PRINTF(".");

                    if (HalPm_MediaCoreSetPowerMode(cDevicePmDeepSleep) == cPmDeviceLock)
                    {
                        if (HalPm_EnterDeepPowerDownMode() != cEcNoError)
                        {
                            HalPm_ExitDeepPowerDownSequence();

                            HalIpc_SetPowerMode(cIpcPmActive);
                            HalIrq_SetPowerMode(cIrqPmActive);
                        }
                    }
                    else
                    {
                        gIdleEventTimer = 0;
                    }

                    HalPm_MediaCoreSetPowerMode(cDevicePmActive);
                }
                END_CS(orgState);

                break;
            default:
                HALT();
                break;
        }

        if (powerState == gPmInfo.maxPowerState)
        {
            gIdleEventTimer = 0;
        }
    }
    else
    {
        gIdleEventTimer = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Idle flush handler.

**/
//-----------------------------------------------------------------------------
static __inline void Nvme_HandleIdleFlushEvent(void)
{
    Msg_t* pMsg;

    pMsg = Ipc_GetExtendMsg(cMcIdleFlush);
    if (pMsg != NULL)
    {
        IPC_SendMsgQ(cH2cReqQueue,  pMsg);
    }
}

#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    Background Event handler.
**/
//-----------------------------------------------------------------------------
static uint32_t Nvme_HandleBackGroundEvent(void)
{
    // For tnvme group 14:3.0.0: Test: Delete an IOCQ that is full of cmds.
#if (FIX_NVME_FETCH_CMD_DEADLOCK_CQ_FULL == ENABLE)
    if (gHostIfMgr.info.mediaReqJobCnt != gHostIfMgr.info.mediaReqDoneCnt)
    {
        if ((gIdleEventTimer > 100) && DATA_XFER_DONE_BIT_MAP())
        {
            U32 gpidx, cmdIndex, newBM, i;
            MsgHostIO_t* pHcmdMsg;
            HcmdQ_t* pHcmdQ = NULL;

            if (rNvmeController.queueStatus[0].sq.b.headPtr != rNvmeController.queueStatus[0].sq.b.tailDbPtr)
            {
                uint32_t qid;

                #if 0
                rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all = 0x03010300;
                rNvmeQueueArbiter.queueArbMonitorSgnSel.b.QARB_MON_SEL = 0x1;

                if (rNvmeCentralCtrl.centralCtrlModuleMonitorData.b.CENT_MON_DATA & BIT(13))
                #else
                if (CIABM_FREE_CNT() == 0)
                #endif
                {
                    rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all = 0x03010300;
                    rNvmeQueueArbiter.queueArbMonitorSgnSel.b.QARB_MON_SEL = 0x10;

                    qid = rNvmeCentralCtrl.centralCtrlModuleMonitorData.b.CENT_MON_DATA & 0x1F;

                    rNvmeController.queueReg[qid].sq.b.Error = 1;
                    rNvmeController.queueReg[qid].sq.b.Error = 0;
                }
            }

            // For tnvme group 5:5.8.0: Test: Create IOCQ/IOSQ pairs while mismatch Q sizes.
            for (gpidx = 0; gpidx < cNvmeNumOfBitmapEntries; gpidx++)
            {
                newBM = DTDBM_DW(gpidx) & (~CCBM_DW(gpidx));

                i = 0;
                while (i < 32)
                {
                    if (newBM && BIT(i))
                    {
                        cmdIndex = (gpidx * 32) + i;
                        pHcmdQ   = &gHcmdQ[cmdIndex];
                        pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);
                        if ((pHcmdMsg->hdr.b.status >= cMsgXfered) && (pHcmdMsg->hdr.b.status != cMsgCompleted))
                        {
                            BEGIN_CS1
                            NvmeCmd_ReturnCacheMsg(pHcmdQ);
                            END_CS1
                        }
                    }

                    i++;
                }
            }
        }

        return FALSE;
    }
#endif

    // Absolute Idle Case
    if (CIABM_FREE_CNT() == HW_MAX_IO_CMD_INDEX)
    {
        gSmartIOTimerFlag = FALSE;

    #if CO_SUPPORT_DEVICE_SELF_TEST
        if (gCurrDSTOperation)
        {
            DSTOperationImmediate();
            return FALSE;
        }
    #endif

        if (gNvmeIfMgr.AsyncEventEnable && NvmeCmd_AsynchronousEventPeek())
        {
            return TRUE;
        }

    #if (CO_SUPPORT_IDLEFLUSH == TRUE)
        if ((smCacheMgr.ssdCnt[SSD_POOL_FREE] + smCacheMgr.ssdCnt[SSD_POOL_GC]) != CACHE_RW_COUNT)
        {
            if (gIdleEventTimer >= IDLFLUSH_THRESHOLD)
            {
                Nvme_HandleIdleFlushEvent();
                return TRUE;
            }

            return FALSE;
        }
    #endif

    #if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
        if (Nvme_IsSafeToPowerDown())
        {
            Nvme_HandlePowerDoneEvent();
        }
        else
        {
            return TRUE;
        }
    #endif

    }
    else
    {
        // TODO: CCBM set but no xferdone!
    }

    return FALSE;
}

#if (CHK_CODE_TYPE(CODE_MAIN) && CHK_FW_TYPE(FW_AGING))
//-----------------------------------------------------------------------------
/**
    Nvme_AgingDRAMRoutine
**/
//-----------------------------------------------------------------------------
static void Nvme_AgingDRAMRoutine(void)
{
    volatile U32 *pCurAddr;
    U32 count;
    U32 readVal;
    U32 RereadVal;
    U32 writeVal;
    U32 errorCount;

    if (smAgingDramParameterTag == DRAM_TAG_TUNE_START)     //DramTune Parameter
    {
        HalMc5_PadCalibrationRoutine(DRAM_PAD_CAL_TIMES);
        HalDdrDll_Routine();
        smAgingDramParameterTag = DRAM_TAG_TUNE_END;
    }

    if (smAgingDramInfo.AgingDramTag == DRAM_TAG_TEST_START) //Dram W/R Test
    {
        errorCount         = 0;
        count              = smAgingDramInfo.AgingDramTestSize;
        pCurAddr           = (volatile U32 *)smAgingDramInfo.AgingL2PPoint;

        D_PRINTF("[DRAM] Test Start Addr[%x]\n", pCurAddr);

        // Write address pattern forward.
        srand(0x12345678);

        while (count-- > 0)
        {
            writeVal    =   rand()%(U32_MAX);
            *pCurAddr   =   writeVal;
            pCurAddr++;
        }

        count              = smAgingDramInfo.AgingDramTestSize;
        pCurAddr           = (volatile U32 *)smAgingDramInfo.AgingL2PPoint;

        srand(0x12345678);

        // Compare address pattern forward.
        while (count-- > 0)
        {
            writeVal = rand()%(U32_MAX);
            readVal  = *pCurAddr;

            if (readVal != writeVal)
            {
                RereadVal                                             = *pCurAddr;
                smSysInfo->d.AgingData.d.AgingDram.DramErrorTemp.SOC  = (U16)HalTempSensor_GetTemperature();

                if(errorCount < 5)
                {
                    D_PRINTF("[DRAM] FAIL ADDRESS[%x] WPattern[%x] RPattern[%x] ReRPattern[%x]\n", pCurAddr, writeVal, readVal, RereadVal);
                    smSysInfo->d.AgingData.d.AgingDram.DramAddr  = (U32)pCurAddr;
                    smSysInfo->d.AgingData.d.AgingDram.DramWVal  = writeVal;
                    smSysInfo->d.AgingData.d.AgingDram.DramRVal  = readVal;
                    smSysInfo->d.AgingData.d.AgingDram.DramRRVal = RereadVal;
                }
                errorCount++;
            }
            pCurAddr++;
        }

        if (errorCount > 0)
        {
            smSysInfo->d.AgingData.d.AgingDram.DramErrorCnt = errorCount;
            smAgingDramInfo.AgingDramStatus                 = DRAM_TAG_TEST_FAIL;
            D_PRINTF("[DRAM] FAIL\n");
        }

        D_PRINTF("[DRAM] Test End\n");
        HalTimer_DelayMsec(60000);
    }
}
#endif

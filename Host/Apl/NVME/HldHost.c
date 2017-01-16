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
//! \file
//! @brief Generic Host I/F High level driver API codes
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
#include "RegPciePhy.h"
#include "RegCp15.h"

// HAL
#include "HalCpu.h"
#include "HalIrq.h"
#include "HalPcieLink.h"
#include "HalPm.h"
#include "HalNvmeXfer.h"
#include "HalRegConfig.h"
#include "HalTimer.h"

// APL
#include "HldNvme.h"
#include "Cache.h"
#include "NvmeDirective.h"
// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define GET_AXI_ADDR_FROM_PTR(X)        ((uint32_t)(X))

#define GET_HOST_SSD_PTR_FROM_INDEX(I)  (&smSSD[I])
#define GET_HOST_SSD_INDEX_FROM_PTR(P)  ARRAY_INDEX_FROM_PTR(smSSD, P)

#define IO_CMD_FETCHED(N)               (rNvmeCmdSram.cmdSramFwIOCmdCntDecrease.all = N)       // FW fetched I/O command from FIFO
#define ADMIN_CMD_FETCHED(N)            (rNvmeCmdSram.cmdSramFwAdminCmdCntDecrease.all = N)    // FW fetched Admin command from FIFO
#define IO_CMD_AVAILABLE(N)             (rNvmeBitmapSram.bitmapSramCmdIndexAvailablBitmapCntrIncrease.all = N)

#define GET_SEGMENT_NUM(ID)             (ID)        ///< get segment index from flow control Id

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
extern       IrqHandler_t         sIrqHandler[cIrqNumOfId];
extern       uint8_t*             gPreSubmitBuffer;

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern uint16_t HalNvme_GetAvailableAcmdIndex(void);
extern uint32_t HalNvme_SetMcqPostingFifo(HldCmdId_t hldCmdId, void* pSdbBuf );
extern uint32_t HalNvme_SaveAdminCommandId(uint16_t cmdIndex, void* pvAcmdDw0, void* pvAcmdCdb);
extern void     AplHost_IoCommandCompleted(void);
extern Error_t  HalNvme_RequestDataTransferAdmin(XferRequest_t* pRequest);
extern void     HalNvme_PushToMcqFifo(StatManagerFifoWritePort_t fifo[], uint32_t bitMask);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    One time Initialization of HOST I/F HW.

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t AplHost_OneTimeInit(InitBootMode_t initMode)
{
    // Wait for PCIe clock from host
    HalRci_WaitValueU32((uint32_t)&rPciePhy.laneStat1, BIT(0), BIT(0), RCI_NO_TIMEOUT);

    HalNvme_OneTimeInit(initMode);

    HalNvme_SsdSetup();

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Completion of Reset.

    @param[in] error    reset error code
**/
//-----------------------------------------------------------------------------
void AplHost_ResetComplete(Error_t error)
{
    NvmeHba.ready = true;
    HalNvme_SetReadyState();
}

//-----------------------------------------------------------------------------
/**
    Fill contents of a HW admin command data block.

    @param[in] cmdIndex     Commnad Index to index HW entries
    @param[in] pCdb         Pointer to a NVMe command CDB
**/
//-----------------------------------------------------------------------------
__inline void AplHost_FillHwAdcmdCdb(uint16_t cmdIndex, NvmeCdb_t* pCdb)
{
    NvmeHwAdmCmd_t* pHwAcmd;

    pHwAcmd = (NvmeHwAdmCmd_t*) &rNvmeHwAdmCmd[cmdIndex];

    pHwAcmd->PRP1_LO = pCdb->PRP1_SGL_LO;
    pHwAcmd->PRP1_HI = pCdb->PRP1_SGL_HI;
    pHwAcmd->PRP2_LO = pCdb->PRP2_SGL_LO;
    pHwAcmd->PRP2_HI = pCdb->PRP2_SGL_HI;
}

//-----------------------------------------------------------------------------
/**
    Get a admin command data block from Command FIFO.

    @param[in] pCdbBuf      CDB buffer pointer
    @param[in] wholeCdb     copy whole CDB or partial for speed

    @return HLD command Id
**/
//-----------------------------------------------------------------------------
static __inline HldCmdId_t AplHost_PopAdminCommand(void* pCdbBuf, bool wholeCdb)
{
    uint16_t  cmdIndex;
    uint32_t* pdwCmdFifo;
    uint32_t* pdwCdbBuf = pCdbBuf;
    NvmeFwAdmCmdDw0_t   acmdDw0;
    uint16_t functionId;

    if(0xFFFF == (cmdIndex = HalNvme_GetAvailableAcmdIndex()))
    {
        return HLD_INVALID_CID; // Return HLD_INVALID_CID(-1) if no slot is available.
    }

    pdwCmdFifo = (uint32_t*)(rNvmeFwAdmCmd);

    // When it is capable to use a burst mode copy, need to implement using MemCopy8U32.
    acmdDw0.all = *pdwCmdFifo;
    MemCopy16U32(pdwCdbBuf, pdwCmdFifo);    // Burst mode copy for 16 DWs

    // Save command IDs to use them in a Hal Layer (FID, SQID, CID)
    functionId = HalNvme_SaveAdminCommandId(cmdIndex, (void*) &acmdDw0, (void*) pdwCdbBuf);

    HalNvme_LockAcmdBitmap(cmdIndex);// Lock the bitmap for the admin command index
    ADMIN_CMD_FETCHED(1);

    // Update CMD Buffer for Stream ID
    #if (SUP_DIRE_STREAMS == ENABLE)
    rNvmeCmdFetch.cmdFetcherAxiBufCntrDecrease.b.CMDF_BUF_CNTR_DEC = 1;
    #endif

    // Fill all hardware admin command contents which will be used later.
    AplHost_FillHwAdcmdCdb(cmdIndex, (NvmeCdb_t*) pdwCdbBuf);

    // Retrun a cmdIndex(bit[11:0], a function id(bit[14:12]) and an admin mark(bit15).
    return (HldCmdId_t) (ADMIN_COMMAND_MASK | (functionId << 12) | cmdIndex);
}

#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
/**
    Get an NVMe Command structure from FW/HW IO CMD SRAM for a FW IO Mode 0.

    @param[in] pCdb         CDB buffer pointer
    @param[in] pFwIocmd     Pointer to a FW IO CMD data of a fetched command
    @param[in] pHwIocmd     Pointer to a HW IO CMD data of a fetched command
    @param[in] wholeCdb     copy whole CDB or partial for speed

    return function id
**/
//-----------------------------------------------------------------------------
__inline uint16_t AplHost_GetIocmdCdb0(NvmeCdb_t* pCdb, NvmeFwIoCmdMode0_t* pFwIocmd, NvmeHwIoCmd_t* pHwIocmd, bool wholeCdb)
{
    uint16_t cmdIndex = pFwIocmd->CID;

    // Save command IDs to use them in a Hal Layer (FID, SQID, CID)
    SAVE_IO_XFER_ID(cmdIndex);
    SET_IO_NSID_FROM_INDEX(cmdIndex, pFwIocmd->NSID);

#if 1
    MEM_U64(&pCdb->DW0)  = MEM_U64(&pFwIocmd->DW0);             // 64bit copy, DW0_1 for performance
    pCdb->CID = GET_IO_CID_FROM_INDEX(cmdIndex);
    MEM_U64(&pCdb->DW10) = MEM_U64(&pFwIocmd->SLBA_LO_CDW10);   // 64bit copy, DW10_11 for performance

    // Since HW_IO_CMD only supports multiple number of block transfer, convert transfer length in DSM command to block size.
    if (pCdb->OPC == 0x09) // clear LBA field in HW_IO_CMD entry for Data set management command
    {
        pHwIocmd->SLBA_LO_CDW10 = 0;
        pHwIocmd->SLBA_HI_CDW11 = 0;
        pHwIocmd->NRB = 4096/HLBU_DATA_SIZE; // 4KB transfer regardless of Number of Range
        pCdb->PRP1_SGL_LO = pHwIocmd->PRP1_LO;
        pCdb->PRP1_SGL_HI = pHwIocmd->PRP1_HI;
        pCdb->PRP2_SGL_LO = pHwIocmd->PRP2_LO;
        pCdb->PRP2_SGL_HI = pHwIocmd->PRP2_HI;
    }
#else
    pCdb->OPC  = pFwIocmd->OPC;             // DW0
    pCdb->FUSE = pFwIocmd->FUSE;            // DW0
    pCdb->CID  = pHwIocmd->CID;             // DW0
    pCdb->NSID = pFwIocmd->NSID;            // DW1

    pCdb->DW10 = pFwIocmd->SLBA_LO_CDW10;   // DW10 or SLBA LOW
    pCdb->DW11 = pFwIocmd->SLBA_HI_CDW11;   // DW11 or SLBA HIGH
#endif

    pCdb->DW12 = pFwIocmd->DW4.all;         // DW12 or NLB
    pCdb->DW13 = pFwIocmd->DW4.b.DATM;      // DW13 or DSM

    #if CO_CHECK_PRP_FEATURE_SUPPORT//To pass IOL test 5.4
    {

        pCdb->PRP1_SGL_LO = pHwIocmd->PRP1_LO;
        //pCdb->PRP1_SGL_HI = pHwIocmd->PRP1_HI;
        pCdb->PRP2_SGL_LO = pHwIocmd->PRP2_LO;
        //pCdb->PRP2_SGL_HI = pHwIocmd->PRP2_HI;
    }
    #endif

    return(GET_IO_FID_FROM_INDEX(cmdIndex));
}

//-----------------------------------------------------------------------------
/**
    Get a Hld command id and an IO command data block from Command FIFO.

    When Manual CQ Posting is used for data transfer command, command indices can be available before posting.
    If comand index is used before it is released by FW, then FW needs not to proceed the rest of fetching command.

    @param[in] pCdbBuf      CDB buffer pointer
    @param[in] wholeCdb     copy whole CDB or partial for speed

    @return Hld command Id
**/
//-----------------------------------------------------------------------------
HldCmdId_t AplHost_PopIoCommandMode0(void* pCdbBuf, bool wholeCdb)
{
    uint16_t cmdIndex;
    uint16_t functionId;
    NvmeFwIoCmdMode0_t  fwIocmd;

    MemCopy5U32((uint32_t*)&fwIocmd, (uint32_t*)(rNvmeFwIoCmd));   // Burst mode copy for 5 DWs

    // Fill NVMe Command Structure
    cmdIndex   = fwIocmd.CID;
    functionId = AplHost_GetIocmdCdb0((NvmeCdb_t*)pCdbBuf, &fwIocmd, (NvmeHwIoCmd_t *)&rNvmeHwIoCmd[cmdIndex], wholeCdb);

    IO_CMD_FETCHED(1); // Decrease one command from FIFO.

    if (!IS_SQ_CREATED(GET_IO_SQID_FROM_INDEX(cmdIndex))) // HW already fetched but SQ is deleted, do not service
    {
        E_PRINTF(cEcNvmeErrorBase, "INVALID SQID:%4X - aborted command CID:%X\n", GET_IO_SQID_FROM_INDEX(cmdIndex), GET_IO_CID_FROM_INDEX(cmdIndex));

        // Update the CMD fetcher copy pointer
        #if (SUP_DIRE_STREAMS == ENABLE)
        rNvmeCmdFetch.cmdFetcherAxiBufCntrDecrease.b.CMDF_BUF_CNTR_DEC = 1;
        gCmdfPtr++;
        #endif

        BA_WR1_U32_L2M(rNvmeBitmapW1c.CIABM, cmdIndex);               //  Command available Bitmap
        IO_CMD_AVAILABLE(1);

        return HLD_INVALID_CID;
    }

    CLR_IOCMD_XFERABLE(cmdIndex);

    // Retrun a cmdId(bit[11:0] with a function id(bit[14:12]).
    return (HldCmdId_t) ((functionId<<12) | cmdIndex);
}

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
/**
    Pop a command data block from Command FIFO.

    @param[in] pCdbBuf      CDB buffer pointer
    @param[in] wholeCdb     copy whole CDB or partial for speed

    @return HLD command Id
**/
//-----------------------------------------------------------------------------
HldCmdId_t AplHost_PopCommand(void* pCdbBuf, bool wholeCdb)
{
    if (!NvmeHba.ready) return HLD_INVALID_CID; // no service if NVMe I/F is not ready

    // Pop Admin command first
    if(ADMIN_CMD_FIFO_CNT())
    {
        if(HalNvme_IsAcmdIndexAvailable())
        {
            return AplHost_PopAdminCommand(pCdbBuf, wholeCdb);
        }
        return HLD_INVALID_CID;
    }
    return HLD_INVALID_CID;
}

//-----------------------------------------------------------------------------
/**
    Convert NVMe status data to HW specific FIFO data.
    MCQ index is not assigned at this time.

    @param[in]  hldCmdId     Host Hld command identifier
    @param[in]  pSdbBuf      Status data buffer pointer
    @param[out] fifo         HW FIFO data
**/
//-----------------------------------------------------------------------------
static void AplHost_ConvertSdbToMcqFifoData(HldCmdId_t hldCmdId, void* pSdbBuf, StatManagerFifoWritePort_t fifo[])
{
    NvmeCqeModeId_t cqeMode;
    NvmeSdb_t* pSdb = (NvmeSdb_t*) pSdbBuf;
    uint32_t cmdIndex = GET_COMMAND_INDEX(hldCmdId);

    fifo[0].all = 0;
#if (_SOC_REV_>=0x0200)
    cqeMode = (NvmeCqeModeId_t) (CO_AUTO_CABM_CLEAR && !IS_ADMIN_COMMAND_ID(hldCmdId));
    fifo[0].dw0.CQE_MODE = (uint32_t) cqeMode; // CQE Transfer mode
    if(cqeMode == cCqeHwIo)    // HW_I/O CQE mode
    {
        fifo[0].dw0.MCQ_FUNC_ID = cmdIndex;
    }
    else    // Default CQE mode
#endif
    {
        fifo[0].dw0.MCQ_FUNC_ID = GET_FUNCTION_INDEX(hldCmdId);
        fifo[2].dw2.MCQ_SQ_ID = IS_ADMIN_COMMAND_ID(hldCmdId) ? 0 : GET_IO_SQID_FROM_INDEX(cmdIndex);
        fifo[2].dw2.MCQ_CMD_ID = pSdb->CID; // NVMe Host Command Id
    }

    fifo[0].dw0.MCQ_STATUS = (pSdb->DW3)>>17;  // Status field
    fifo[1].dw1.MCQ_CMD_SPC = pSdb->DW0;// Command Specific
                                        // BIT[02:00] 0: ?
                                        // BIT[15:08] ?: Asynchronous Event Information
                                        // BIT[23:16] ?: Associated Log Page, Error Information
    fifo[3].all = 0;
}

//-----------------------------------------------------------------------------
/**
    Check whether a specific CQ FIFO is empty.

    @param[in] cqId         Completion queue Id

    @return TRUE or FALSE
**/
//-----------------------------------------------------------------------------
static bool AplHost_IsCqEmpty(uint32_t cqId, uint32_t notUsed1, uint32_t notUsed2)
{
    uint32_t headTailPtr;

    headTailPtr = rNvmeController.queueStatus[cqId].cq.headTailPtr;

    return (GET_B15_00(headTailPtr) == GET_B31_16(headTailPtr));
}

//-----------------------------------------------------------------------------
/**
    Wait for the specific CQ empty

    @param[in] cqId         Completion queue Id
    @param[in] timeOutUsec  time out in usec unit

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t AplHost_WaitForCqEmpty(uint32_t cqId, uint32_t timeOutUsec)
{
    return HalRci_WaitEvent(AplHost_IsCqEmpty, cqId, 0, 0, timeOutUsec);
}

//-----------------------------------------------------------------------------
/**
    Convert Queue Status to NVMe status data when error happens.

    @param[in]  hldCmdId     Host Hld command identifier
    @param[in]  fifo         HW FIFO data
    @param[out] pSdbBuf      Status data buffer pointer
**/
//-----------------------------------------------------------------------------
static void AplHost_ConvertQueueStatusToErrorSdb(HldCmdId_t hldCmdId, void* pSdbBuf, StatManagerFifoWritePort_t fifo[])
{
    uint32_t sqId;
    uint32_t cqId;
    uint32_t TmpBitmap[cNvmeNumOfBitmapEntries];
    NvmeSdb_t* pSdb = (NvmeSdb_t*) pSdbBuf;
    MsgHostIO_t* pHcmdMsg;
    HwTimerTick_t tick;
    U16 cnt;
    U8 idx;
    U16 emptyBitCount = 0;

    sqId = fifo[2].dw2.MCQ_SQ_ID;
    cqId = rNvmeController.queueStatus[sqId].sq.b.cqId; // Get an associative CQ ID

    // a. Wait till there is no outstanding (activated) command
    pHcmdMsg = (MsgHostIO_t*)smHcmdMsg;

    tick = HalTimer_ReadTick();

    if (!Host_XferAllDone())
    {
        for (idx=0; idx < cNvmeNumOfBitmapEntries; idx++)
        {
            if (HalTimer_GetElapsedTimeUsec(tick) > 1000*1000)        break;// timeout

            if (CCBM_DW(idx) ^ CABM_DW(idx))
            {

                TmpBitmap[idx] = CCBM_DW(idx) ^ CABM_DW(idx);

                while (emptyBitCount != 32)
                {

                    emptyBitCount = 0;

                    for (cnt = 0; cnt < 32; cnt++)
                    {
                        if ((TmpBitmap[idx] & BIT(cnt)) && (pHcmdMsg[(idx<<5)+cnt].bufDesc.flowCtrl == cXferIds && pHcmdMsg[(idx<<5)+cnt].error == cEcIdsReadFail))
                        {
                            emptyBitCount++;

                        }else if (!(TmpBitmap[idx] & BIT(cnt))){

                            emptyBitCount++;
                        }
                    }

                    if (HalTimer_GetElapsedTimeUsec(tick) > 1000*1000)        break;// timeout
                }
            }
        }
    }

    D_PRINTF("Final  \n");

    // b. Wait for the Status Manager FIFO to have no CQE entry with same CQ Id of the error command
    //    - Method 1: Wait for the Empty status of the Status Manager FIFO Status (0xF5015004[5:4])
    //    - Method 2: Check empty status for same CQ Id.
    AplHost_WaitForCqEmpty(cqId, 1000*1000);            // Method 2

    // c. Save the Phase(PH) bit of the Queue Status Register table for the same CQ Id
    pSdb->P = rNvmeController.queueReg[cqId].cq.b.PH;   // Get a phase tag which will be posted to Host.

    // d. Post CQE for the error command later.
}

//-----------------------------------------------------------------------------
/**
    Send status to host, Completion of Host command.

    @param[in] hldCmdId     Host Hld command identifier
    @param[in] pSdbBuf      Status data buffer pointer
    @param[in] wholeSdb     update whole SDB
    @param[in] hostIrq      Host interrupt request
**/
//-----------------------------------------------------------------------------
void AplHost_SendStatus(HldCmdId_t hldCmdId, void* pSdbBuf, bool wholeSdb, uint32_t hostIrq)
{
    StatManagerFifoWritePort_t fifo[4] = { 0, 0, 0, 0 };

    if (!NvmeHba.ready) return; // no service if NVMe I/F is not ready

    AplHost_ConvertSdbToMcqFifoData(hldCmdId, pSdbBuf, fifo);

    if (fifo[0].dw0.MCQ_STATUS & 0xFF) // For the error command
    {
        AplHost_ConvertQueueStatusToErrorSdb(hldCmdId, pSdbBuf, fifo);
    }

    HalNvme_PushToMcqFifo(fifo, cAppMcqIndexMask);

    if (IS_ADMIN_COMMAND_ID(hldCmdId))
    {
        HalNvme_ReleaseAdminCommand(GET_COMMAND_INDEX(hldCmdId));
    }
    else
    {
        HalNvme_ReleaseIoCommand(GET_COMMAND_INDEX(hldCmdId));
    }
}

//-----------------------------------------------------------------------------
/**
    Release data cache

**/
//-----------------------------------------------------------------------------
void AplHost_ReleaseDataCache(void)
{
    uint32_t idx;
    uint32_t GroupId;
    uint32_t Bitmap;
    HcmdQ_t* pHcmdQ;
    MsgHostIO_t* pHcmdMsg;

    D_PRINTF("[RSET] NVMe Err Job[%x-%x]\n", smFioInfo.jobIssueCnt, smFioInfo.jobDoneCnt);

    BEGIN_CS1;
    for (idx = 0; idx < HW_MAX_IO_CMD_INDEX; idx++)
    {
        pHcmdQ = &gHcmdQ[idx];
        pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);
        // Release cnt cache
        if ((pHcmdMsg->hdr.b.status >= cMsgExecuted) && (pHcmdMsg->hdr.b.status <= cMsgCmdAbort))
        {
            pHcmdMsg->bufDesc.prpRdy = cMcPrpError;
            pHcmdMsg->hdr.b.status   = cMsgCompleted;
            GroupId = idx >> 5;
            Bitmap  = (1 << (idx & 0x1F));

            BIT_CLR(ACTIVATED_DW_FW(GroupId), Bitmap);          // Clear FW
            if (!(Bitmap & CCBM_DW(GroupId)))
            {
                pHcmdMsg->bufDesc.cssTemp = CACHE_ABORT;
            }
            NvmeCmd_ReturnCacheMsg(pHcmdQ);
            Host_CommandComplete(pHcmdQ);
        }
    }
    END_CS1;

BEGIN_HOST_DBG_DUMP(DBG_DUMP_ERRHANDLE)
    D_PRINTF("[RSET] (A)Return Executed cache Done\n");
END_HOST_DBG_DUMP

    // Wait All media job done --> All cache read/ IDS read are done here, cought by IPC
    while (!Host_MediaAllDone())
    {
        while (IPC_MsgQFastPeek(cC2hComQueue))
        {
            pHcmdMsg = (MsgHostIO_t*)IPC_GetMsgQ(cC2hComQueue);

            if (pHcmdMsg->error != cEcIdsReadFail)
            {
                Host_MediaDone();
            }

            if (ARRAY_INDEX_FROM_PTR(smHcmdMsg, pHcmdMsg) < MAX_HCMD_Q_COUNT)
            {
                pHcmdQ = GetHcmdQuePtr(pHcmdMsg);

                if (pHcmdMsg->hdr.b.status >= cMsgExecuted)
                {
                    #if 0
                    BEGIN_HOST_DBG_DUMP(DBG_DUMP_ERRHANDLE)
                    D_PRINTF("\t[%3d][%2d]MDR[%03d-%2d]", cnt, pHcmdMsg->hdr.b.status, pHcmdMsg->attrInfo.b.cmdId, pHcmdMsg->bufDesc.css);
                    if((cnt++)%5==0)  D_PRINTF("\n");
                    END_HOST_DBG_DUMP
                    #endif

                    BEGIN_CS1;
                    pHcmdMsg->bufDesc.cssTemp = CACHE_ABORT;
                    NvmeCmd_ReturnCacheMsg(pHcmdQ);
                    END_CS1;
                }

                idx = pHcmdMsg->attrInfo.b.cmdId;
                pHcmdMsg->hdr.b.status = cMsgCompleted;
                GroupId = idx >> 5;
                Bitmap  = (1 << (idx & 0x1F));

                BEGIN_CS1;
                BIT_CLR(ACTIVATED_DW_FW(GroupId), Bitmap);          // Clear FW
                END_CS1;
            }
        }
    }

BEGIN_HOST_DBG_DUMP(DBG_DUMP_ERRHANDLE)
    D_PRINTF("[RSET] (B)Return C2H cache Done, R[%d], D[%d]\n",
        gHostIfMgr.info.mediaReqJobCnt, gHostIfMgr.info.mediaReqDoneCnt);
END_HOST_DBG_DUMP
}

//-----------------------------------------------------------------------------
/**
    Release data cache when PLP discharge

**/
//-----------------------------------------------------------------------------
void AplHost_ReleaseDataCachePLP(void)
{
    uint32_t     GroupId;
    uint32_t     Bitmap;
    uint32_t     idx;
    HcmdQ_t*     pHcmdQ;
    MsgHostIO_t* pHcmdMsg;

    //D_PRINTF("[PLP] FE detect PLP\n");

    BEGIN_CS1;
    for (idx = 0; idx < HW_MAX_IO_CMD_INDEX; idx++)
    {
        pHcmdQ = &gHcmdQ[idx];
        pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);
        // Release cnt cache
        if ((pHcmdMsg->hdr.b.status >= cMsgExecuted) && (pHcmdMsg->hdr.b.status <= cMsgCmdAbort))
        {
            GroupId = idx >> 5;
            Bitmap  = (1 << (idx & 0x1F));

            if (Bitmap & CCBM_DW(GroupId))
            {
                NvmeCmd_ReturnCacheMsg(pHcmdQ);
            }
        }
    }
    END_CS1;
}

//-----------------------------------------------------------------------------
/**
    Transfer data from device to host or host to device.

    @param[in]  pRequest      data transfer request structure pointer

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t AplHost_TransferAdminData(XferRequest_t* pRequest)
{
    uint32_t nsId0;
    uint16_t cmdIndex;

    if (!NvmeHba.ready) return cEcError; // no service if NVMe I/F is not ready

    cmdIndex = GET_COMMAND_INDEX(pRequest->attr.b.cmdId);

    if (IS_ADMIN_COMMAND_ID(pRequest->attr.b.cmdId))
    {
        SET_ADM_XFER_PARAM_FROM_INDEX(cmdIndex, pRequest->xferDoneParam);
    }

    HalNvme_RequestDataTransferAdmin(pRequest);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_XFER)
        M_PRINTF("[HCMD_X:%4X] A:%X B:%X,%X X:%X,%X\n", pRequest->attr.b.cmdId, pRequest->attr.all,
                  pRequest->pBuffer, pRequest->xferDoneParam, pRequest->xferByteCount, pRequest->vbbc);
    END_HOST_DBG_DUMP

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Transfer whole data from or to host when transfer length is over 16K.
      - Non-streaming whole data transfer. (no bus disconnection)

    @param[in]  pHcmdQ              host command queue pointer
    @param[in]  pBuffer             buffer pointer to transfer (non-scattered buffer only)
    @param[in]  xferByteCount       transfer byte count
    @param[in]  waitTransferDone    wait transfer done if TRUE

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t AplHost_TransferWholeData_WithPRPList(HcmdQ_t* pHcmdQ, void* pBuffer, uint32_t xferByteCount, bool waitTransferDone)
{
    //======================== Transfer multi 4K =====================================
    //
    // Important: Since PRP1 align at 4X ,the buffer length should be over (xferByteCount+0x1000-1)
    //
    Error_t    error;
    volatile   HcmdAttr_t cmd_attr_bak;        ///< command attribute
    uint16_t   cmdIndex;
    uint32_t   Over16k_preSubmit_Cnt;

    uint32_t   prp1_LO_bak;
    uint32_t   prp1_HI_bak;
    uint32_t   cnt;
    uint32_t   transfered_cnt = 0;
    uint32_t   transfer_bytecnt;

    volatile   NvmeHwAdmCmd_t* pHwAcmd;
    uint8_t*   Over16k_preSubmit_ptr;
    uint8_t*   pSubmit_Buf;
    uint32_t   prp1_4kAlign_offset;

    cmdIndex = GET_COMMAND_INDEX(GetHcmdId(pHcmdQ));
    pHwAcmd  = (NvmeHwAdmCmd_t*) &rNvmeHwAdmCmd[cmdIndex];

    prp1_LO_bak      = pHwAcmd->PRP1_LO;
    prp1_HI_bak      = pHwAcmd->PRP1_HI;
    prp1_4kAlign_offset = prp1_LO_bak & (HOST_PAGE_SIZE - 1);

    if (xferByteCount > (HOST_PAGE_SIZE - prp1_4kAlign_offset + HOST_PAGE_SIZE))   // if PRP2List exist ?
    {
        cmd_attr_bak.b.autoEoC = pHcmdQ->attr.b.autoEoC;  // backup autoEoC
        pHcmdQ->attr.b.autoEoC = FALSE;
        Over16k_preSubmit_Cnt = (xferByteCount - (HOST_PAGE_SIZE - prp1_4kAlign_offset) + (HOST_PAGE_SIZE - 1)) / HOST_PAGE_SIZE;   // 4K unit
        ASSERT((Over16k_preSubmit_Cnt * 8) <= (HOST_PAGE_SIZE - prp1_4kAlign_offset));   // Max transfer is 0x200*0x1000 + 0x1000 = 0x201000

        // ----------- preSubmit -----------
        cmd_attr_bak.b.xferDir = pHcmdQ->attr.b.xferDir;
        pHcmdQ->attr.b.xferDir = cXferH2d;
        pHwAcmd->PRP1_LO = pHwAcmd->PRP2_LO;
        pHwAcmd->PRP1_HI = pHwAcmd->PRP2_HI;
        pHwAcmd->PRP2_LO = 0;
        pHwAcmd->PRP2_HI = 0;
        ASSERT(gPreSubmitBuffer != NULL);   // stop if ScratchBuffer used.

        pHcmdQ->bufDesc.ssdIndex = SSD_ADMIN_START;
        smSSD[SSD_ADMIN_START].startAddr =   (uint32_t)gPreSubmitBuffer                   >> SSD_ADDR_SHIFT;
        smSSD[SSD_ADMIN_START].endAddr   = (((uint32_t)gPreSubmitBuffer + HOST_PAGE_SIZE) >> SSD_ADDR_SHIFT) - 1;
        error = AplHost_TransferWholeData(pHcmdQ, gPreSubmitBuffer, Over16k_preSubmit_Cnt * 8, TRUE);

        if (error != cEcNoError) goto Over16k_Exit;
        pHcmdQ->attr.b.xferDir         = cmd_attr_bak.b.xferDir;

        // --------- Submit 1'st 4K ---------
        pSubmit_Buf                    = (uint8_t*)pBuffer;
        Over16k_preSubmit_ptr          = gPreSubmitBuffer;
        cnt                            = Over16k_preSubmit_Cnt;

        pHwAcmd->PRP1_LO               = prp1_LO_bak;
        pHwAcmd->PRP1_HI               = prp1_HI_bak;
        pHwAcmd->PRP2_LO               = 0;
        pHwAcmd->PRP2_HI               = 0;

        pHcmdQ->bufDesc.ssdIndex       = SSD_ADMIN_START;
        smSSD[SSD_ADMIN_START].startAddr =   (uint32_t)pSubmit_Buf                   >> SSD_ADDR_SHIFT;
        smSSD[SSD_ADMIN_START].endAddr   = (((uint32_t)pSubmit_Buf + HOST_PAGE_SIZE) >> SSD_ADDR_SHIFT) - 1;
        BIT_CLR(rNvmeDataMngr.dataManagerCtrl0.b.DATM_ACMD_CABM, BIT(cmdIndex));
        error = AplHost_TransferWholeData(pHcmdQ, pSubmit_Buf, HOST_PAGE_SIZE, TRUE);
        if(error != cEcNoError) goto Over16k_Exit;

        // -------- Submit 2'nd 4K ~ last 4K --------------
        pSubmit_Buf                       += (HOST_PAGE_SIZE - prp1_4kAlign_offset);
        transfered_cnt                    += (HOST_PAGE_SIZE - prp1_4kAlign_offset);
        do
        {
            pHwAcmd->PRP1_LO               = (*((uint32_t*)((uint8_t*)Over16k_preSubmit_ptr+0)));
            pHwAcmd->PRP1_HI               = (*((uint32_t*)((uint8_t*)Over16k_preSubmit_ptr+4)));
            pHwAcmd->PRP2_LO               = 0;
            pHwAcmd->PRP2_HI               = 0;
            smSSD[SSD_ADMIN_START].startAddr =  (uint32_t)pSubmit_Buf                       >> SSD_ADDR_SHIFT;
            smSSD[SSD_ADMIN_START].endAddr   = ((uint32_t)pSubmit_Buf + HOST_PAGE_SIZE - 1) >> SSD_ADDR_SHIFT;

            if (cnt == 1)
            {  //last ?
                pHcmdQ->attr.b.autoEoC     = cmd_attr_bak.b.autoEoC;
                transfer_bytecnt           = xferByteCount - transfered_cnt;
            }
            else
            {
                transfer_bytecnt           = HOST_PAGE_SIZE;                           //transfer length 4K
            }

            BIT_CLR(rNvmeDataMngr.dataManagerCtrl0.b.DATM_ACMD_CABM, BIT(cmdIndex));
            error = AplHost_TransferWholeData(pHcmdQ, pSubmit_Buf, transfer_bytecnt, TRUE);

            if (error != cEcNoError) goto Over16k_Exit;

            Over16k_preSubmit_ptr         += 8;
            pSubmit_Buf                   += HOST_PAGE_SIZE;
            transfered_cnt                += HOST_PAGE_SIZE;
        } while(--cnt);
    }
    else
    {
        error = AplHost_TransferWholeData(pHcmdQ, pBuffer, xferByteCount, waitTransferDone);
    }

Over16k_Exit:
    return(error);
}

//-----------------------------------------------------------------------------
/**
    Transfer whole data from or to host.
      - Non-streaming whole data transfer. (no bus disconnection)

    @param[in]  pHcmdQ              host command queue pointer
    @param[in]  pBuffer             buffer pointer to transfer (non-scattered buffer only)
    @param[in]  xferByteCount       transfer byte count
    @param[in]  waitTransferDone    wait transfer done if TRUE

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t AplHost_TransferWholeData(HcmdQ_t* pHcmdQ, void* pBuffer, uint32_t xferByteCount, bool waitTransferDone)
{
    XferRequest_t xferRequest;
    Error_t error;

    SetHcmdHandler(pHcmdQ, NULL);

    if (!xferByteCount) return (cEcNoError);

    pHcmdQ->attr.b.xferNotify = TRUE;

    // set transfer attribute
    xferRequest.attr.all            = 0;
    xferRequest.attr.b.hostIrq      = 1;
    xferRequest.attr.b.cmdId        = GetHcmdId(pHcmdQ);
    xferRequest.attr.b.direction    = pHcmdQ->attr.b.xferDir;
    xferRequest.attr.b.autoEoC      = pHcmdQ->attr.b.autoEoC;
    xferRequest.attr.b.mode         = pHcmdQ->attr.b.xferMode;

    // set buffer descriptor
    xferRequest.pBuffer             = pBuffer;
    xferRequest.ssdIndex            = pHcmdQ->bufDesc.ssdIndex;
    xferRequest.flowCtrl            = cXferNonStreaming;
    xferRequest.xferDoneParam       = (uint32_t) pHcmdQ;
    xferRequest.xferByteCount       = xferByteCount;
    xferRequest.hlba                = 0;

    BEGIN_CS1
    error = AplHost_TransferAdminData(&xferRequest);
    END_CS1

    while (TRUE)
    {
        if (pHcmdQ->attr.b.xferNotify == FALSE)
        {
            break;
        }
        if (gNvmeIfMgr.state != cNvmeIfReady)
        {
            pHcmdQ->error = MK_NVME_ERROR_CODE(cDataTransferError, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            break;
        }
    }

    // release command queue if EoC is handled by HW.
    if (pHcmdQ->attr.b.autoEoC)
    {
        Host_CommandComplete(pHcmdQ);
    }

    return (error);
}

//-----------------------------------------------------------------------------
/**
    Data Xfer for Non Read/Write command only path (TRIM)

    @param[in]  pHcmdQ   Host command queue pointer
                uint32_t Xfer size
**/
//-----------------------------------------------------------------------------
void AplHost_TransferOneNonMedia(HcmdQ_t* pHcmdQ, uint32_t xferByteCount)
{
    XferRequest_t pRequest;
    pRequest.xferByteCount = xferByteCount;
    pRequest.attr.b.direction = pHcmdQ->attr.b.xferDir;
    pRequest.attr.b.cmdId = cReservedAdminIndexForIoCmd;
    pRequest.ssdIndex = pHcmdQ->bufDesc.ssdIndex;
    pRequest.xferByteCount = xferByteCount;
    pRequest.mode = ADMIN_TRIM;
    SET_ADM_XFER_PARAM_FROM_INDEX(pRequest.attr.b.cmdId, (uint32_t) pHcmdQ);
    HalNvme_LockAcmdBitmap(pRequest.attr.b.cmdId);// Lock the bitmap for the admin command index

    pRequest.PRP1_LO = rNvmeHwIoCmd[pHcmdQ->cmdId].PRP1_LO;
    pRequest.PRP1_HI = rNvmeHwIoCmd[pHcmdQ->cmdId].PRP1_HI;
    pRequest.PRP2_LO = rNvmeHwIoCmd[pHcmdQ->cmdId].PRP2_LO;
    pRequest.PRP2_HI = rNvmeHwIoCmd[pHcmdQ->cmdId].PRP2_HI;

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_TRIM_MSG)
    D_PRINTF("PRP1 LO[%08x], HI[%08x]\n", pRequest.PRP1_LO, pRequest.PRP1_HI);
    D_PRINTF("PRP2 LO[%08x], HI[%08x]\n", pRequest.PRP2_LO, pRequest.PRP2_HI);
    D_PRINTF("Xfer Bytes[%08d]\n", pRequest.xferByteCount);
    END_HOST_DBG_DUMP

    HalNvme_RequestDataTransferAdmin(&pRequest);
}

#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING
#if _MEDIA_== MEDIA_DRAM
//-----------------------------------------------------------------------------
/**
    Ramdisk Xfer Setup

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void AplHost_RamdiskIoTrasnferSim(MsgHostIO_t* pHcmdMsg)
{
    uint16_t cmdIndex;

    cmdIndex                   = ARRAY_INDEX_FROM_PTR(smHcmdMsg, pHcmdMsg);
    pHcmdMsg->bufDesc.ssdIndex = cmdIndex;
    smSSD[cmdIndex].startAddr  = ((U32)smCacheRamDrive + pHcmdMsg->laa * MBU_SIZE) >> SSD_ADDR_SHIFT;
}
#endif

//-----------------------------------------------------------------------------
/**
    Data Xfer for Read/Write command only path

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void AplHost_TransferOneQuick(HcmdQ_t* pHcmdQ)
{
    MsgHostIO_t* pHcmdMsg;
    XferAttr_t attrInfo;
    uint32_t groupId;
    uint32_t bitmap;
    uint16_t cmdIndex;

    cmdIndex = GET_COMMAND_INDEX(GetHcmdId(pHcmdQ));
    groupId  = cmdIndex >> 5;
    bitmap   = (1 << (cmdIndex & 0x1F));

    pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);
    MSG_SET_HOST_INIT(pHcmdMsg, pHcmdQ->attr.b.xferDir);

    attrInfo.all = 0;
    attrInfo.b.cmdId     = GetHcmdId(pHcmdQ);
    attrInfo.b.direction = pHcmdQ->attr.b.xferDir;
    attrInfo.b.autoEoC   = pHcmdQ->attr.b.autoEoC;

    SET_IO_XFER_PARAM_FROM_INDEX(cmdIndex, (uint32_t) pHcmdQ);
    SET_IO_ATTR_FROM_INDEX(cmdIndex, attrInfo.all);

    pHcmdMsg->attrInfo.all  = attrInfo.all;

    pHcmdMsg->bufDesc.allFlags      = pHcmdQ->bufDesc.allFlags;
    pHcmdMsg->bufDesc.flowCtrl      = cXferNonStreaming;
    pHcmdMsg->bufDesc.prpRdy        = cMcPrpWait;

    if (gNvmeIfMgr.ns[pHcmdQ->addrInfo.nsId0].hbu_shift == 0)
    {
        if (pHcmdQ->attr.b.xferDir == cXferH2d)
        {
            #if (SUP_DIRE_STREAMS == ENABLE)
            Directive_WriteHandling(pHcmdMsg, pHcmdQ->addrInfo.nsId0);
            #endif

            pHcmdMsg->bufDesc.laaStartBmp = gSLaaBitMask[pHcmdQ->addrInfo.lba & HBU_PER_MBU_MASK];
            pHcmdMsg->bufDesc.laaEndBmp   = gELaaBitMask[(pHcmdQ->addrInfo.lba + pHcmdQ->addrInfo.nb - 1) & HBU_PER_MBU_MASK];
            pHcmdMsg->laa                 =   pHcmdQ->addrInfo.lba                                           >> HBU_PER_MBU_SFT;
            pHcmdMsg->bufDesc.ssdCnt      = ((pHcmdQ->addrInfo.lba + pHcmdQ->addrInfo.nb + HBU_PER_MBU_MASK) >> HBU_PER_MBU_SFT) - pHcmdMsg->laa;

            if (pHcmdMsg->bufDesc.ssdCnt == 1) pHcmdMsg->bufDesc.laaStartBmp &= pHcmdMsg->bufDesc.laaEndBmp;
        }
        else
        {
            pHcmdMsg->laa                 =   pHcmdQ->addrInfo.lba                                           >> HBU_PER_MBU_SFT;
            pHcmdMsg->bufDesc.ssdCnt      = ((pHcmdQ->addrInfo.lba + pHcmdQ->addrInfo.nb + HBU_PER_MBU_MASK) >> HBU_PER_MBU_SFT) - pHcmdMsg->laa;
        }
    }
    else
    {
        if (pHcmdQ->attr.b.xferDir == cXferH2d)
        {
            #if (SUP_DIRE_STREAMS == ENABLE)
            Directive_WriteHandling(pHcmdMsg, pHcmdQ->addrInfo.nsId0);
            #endif
            pHcmdMsg->bufDesc.laaStartBmp = pHcmdMsg->bufDesc.laaEndBmp = CACHE_BMP_ALL_VALID;
        }

        pHcmdMsg->laa                 = pHcmdQ->addrInfo.lba;
        pHcmdMsg->bufDesc.ssdCnt      = pHcmdQ->addrInfo.nb;
    }

    // Set a bitmap whether a command is transferable.
    NvmeHba.bitmapXferable[groupId] |= bitmap;
    // Set a Cabm fw bitmaps to check transfer done or command completion
    NvmeHba.bitmapFwCabm[groupId]   |= bitmap;
    NvmeHba.bitmapFwCabmGroup       |= BIT(groupId);

    Host_MediaReq();

#if _MEDIA_== MEDIA_DRAM
    AplHost_RamdiskIoTrasnferSim(pHcmdMsg);
#endif
    IPC_SendMsgQ(cH2cReqQueue, (Msg_t*) pHcmdMsg);
}

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
/**
    Dump Host HLD information
**/
//-----------------------------------------------------------------------------
void AplHost_DumpInfo(void)
{
    M_PRINTF("\n[NVMe]\n");

    M_PRINTF("\n[PCIe]\n");
    HalLink_DumpInfo();
}

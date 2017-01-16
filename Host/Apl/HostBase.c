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
//! @brief Host I/F base code
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

// HAL
#include "HalNvmeXfer.h"

// APL
#include "HostBase.h"
#include "Cache.h"

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
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


//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
HostIfMgr_t gHostIfMgr;                 ///< host interface manager (base class)
HcmdQ_t     gHcmdQ[MAX_HCMD_Q_COUNT];                     ///< host command queue
HcmdQ_t     gSysInfoCmdQ;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern void HalNvmeDebug_DumpNvmeHwInfo(uint32_t flag);
extern void DumpHwIoCmd(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    One time initialization for Host sub-system.
     - Register monitor command.

    @param  initMode    initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t Host_OneTimeInit(InitBootMode_t initMode)
{
    Host_Reset(cHcIniPowerOn);

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Pop a Host command queue from specified list.

    @param[in]  state    host command state list identifier
    @param[in]  head     1=pop from head, 0=pop from tail

    @return host command queue pointer
**/
//-----------------------------------------------------------------------------
HcmdQ_t* Host_PopFrom(HcmdStateId_t state, bool head)
{
    DlinkEntry_t* pEntry;
    CpuCs_t orgState;
    HcmdQ_t* pHcmdQ;

    if (gHostIfMgr.stateList[state].count==0) return(NULL);

    BEGIN_CS(orgState);

    gHostIfMgr.stateList[state].count--;

    pEntry = (head) ? DlPopFromHead(&gHostIfMgr.stateList[state].headTail)
                    : DlPopFromTail(&gHostIfMgr.stateList[state].headTail);

    pHcmdQ = STR_PTR_FROM_MEMBER(pEntry, HcmdQ_t, dlink);
    pHcmdQ->dlink.prev = NULL;          // NULL mark for debugging
    pHcmdQ->dlink.next = (void*)state;  // state id  for debugging

    END_CS(orgState);

    return (pHcmdQ);
}

//-----------------------------------------------------------------------------
/**
    Push a Host command queue entry to specified list

    @param[in]  state    host command state list identifier
    @param[in]  pHcmdQ   host command queue pointer
    @param[in]  head     1=push to head, 0=push to tail
**/
//-----------------------------------------------------------------------------
void Host_PushTo(HcmdStateId_t state, HcmdQ_t* pHcmdQ, bool head)
{
    CpuCs_t orgState;

    BEGIN_CS(orgState);

    // make sure the cmd queue is not in any list
    ASSERT2(pHcmdQ->dlink.prev==NULL, state);

    gHostIfMgr.stateList[state].count++;

    if (head)   DlAddToHead(&gHostIfMgr.stateList[state].headTail, &pHcmdQ->dlink);
    else        DlAddToTail(&gHostIfMgr.stateList[state].headTail, &pHcmdQ->dlink);

    if (state == cHcsFlushWait)         gMiscEvnet |= MISC_EVENT_FLUSH;
    else if (state == cHcsAsyncReport)  gMiscEvnet |= MISC_EVENT_ASYNC;
    else if (state == cHcsCmdAbort)     gMiscEvnet |= MISC_EVENT_ABORT;
    else if (state == cHcsCmdWait)      gMiscEvnet |= MISC_EVENT_CMDWAIT;

    END_CS(orgState);
}

//-----------------------------------------------------------------------------
/**
    Reset Host sub-system.\n

    @param[in]  resetMode    reset mode
**/
//-----------------------------------------------------------------------------
void Host_Reset(HcInitialMode_t hcIniMode)
{
    uint32_t    index;
    uint32_t    msgResetCnt;
    MsgHostIO_t *pHcmdMsg;

    if (hcIniMode == cHcIniPowerOn)
    {
        msgResetCnt = MAX_HCMD_Q_COUNT + MAX_OTHER_MSG_Q_COUNT;
    }
    else
    {
        msgResetCnt = MAX_HCMD_Q_COUNT;
    }

    // reset all state list
    for (index = cHcsFree; index < cHcsCount; index++)
    {
        DlInit(&gHostIfMgr.stateList[index].headTail);
        gHostIfMgr.stateList[index].count = 0;
    }

    // add all command queue to free list
    for (index = 0; index < MAX_HCMD_Q_COUNT; index++)
    {
        gHcmdQ[index].dlink.prev = NULL;
        gHcmdQ[index].index = index;
    }

    for (index = 0; index < msgResetCnt; index++)
    {
        smHcmdMsg[index].hdr.b.status = cMsgCompleted;
        MEM_CLR((void*) &smHcmdMsg[index].param[0], sizeof(smHcmdMsg[index].param));
    }

    for (index = 0; index < MAX_CACHE_REL_Q_COUNT; index++)
    {
        smCacheRelMsg[index].hdr.b.status = cMsgCompleted;
    }

    // Init abort cmd counter
    gHostIfMgr.info.currAbortCnt = 0;
}

//-----------------------------------------------------------------------------
/**
    Dump Host Information.
**/
//-----------------------------------------------------------------------------
void Host_DumpInfo(U32 allFlag)
{
    uint32_t idx;
    MsgHostIO_t* pMsg;
    MsgCacheRelObj_t* cacheMsg;
    HcmdQ_t* pHcmdQ = NULL;

    AplHost_DumpInfo();

    M_PRINTF("\n[HOST I/F]\n");
    M_PRINTF("Reset H=%10u, S=%10u\n", gHostIfMgr.info.hardResetCnt, gHostIfMgr.info.softResetCnt);
    //M_PRINTF("Command   event=%10u\n", gHostIfMgr.info.cmdEventCnt);
    M_PRINTF("Cmd   F=%10u, D=%10u\n", gHostIfMgr.info.cmdFetchCnt, gHostIfMgr.info.cmdDoneCnt);
    M_PRINTF("Media J=%10u, D=%10u, *[%u]\n", gHostIfMgr.info.mediaReqJobCnt, gHostIfMgr.info.mediaReqDoneCnt,gHostIfMgr.info.mediaReqJobCnt-gHostIfMgr.info.mediaReqDoneCnt);
    M_PRINTF("IF Buf[%08x]\n", smSSD[SSD_IF_START].startAddr);
    M_PRINTF("ADC:%04d IOC:%04d\n", ADMIN_CMD_FIFO_CNT(), IO_CMD_FIFO_CNT());

    for (idx = cHcsFree; idx < cHcsCount; idx ++)
    {
        M_PRINTF("LL[%d]:%d\n", idx, gHostIfMgr.stateList[idx].count);
    }

    if (allFlag == 1)
    {
        for (idx = 0; idx < HW_MAX_IO_CMD_INDEX; idx ++)
        {
            pMsg = (MsgHostIO_t*) &smHcmdMsg[idx];

            pHcmdQ = GetHcmdQuePtr(pMsg);

            M_PRINTF("Msg[%04d]Qid[%2d] OP[%04x], LBA[0x%08X_%08X], SecCnt[%08x], SSD_Cnt[%04d], PrpRdy[%02d], status[%02d]FC[%2d]Css[%2d]\n",
                idx, rNvmeHwIoCmd[idx].SQID, pMsg->hdr.b.opCode, U64_TO_U32_H(pHcmdQ->addrInfo.lba), U64_TO_U32_L(pHcmdQ->addrInfo.lba),
                pHcmdQ->addrInfo.nb, pMsg->bufDesc.ssdCnt, pMsg->bufDesc.prpRdy,
                pMsg->hdr.b.status, pMsg->bufDesc.flowCtrl, pMsg->bufDesc.css);
        }

        for (idx = MAX_HCMD_Q_COUNT; idx < (MAX_HCMD_Q_COUNT + MAX_OTHER_MSG_Q_COUNT); idx ++)
        {
            pMsg = (MsgHostIO_t*) &smHcmdMsg[idx];

            M_PRINTF("OtherMsg[%04d] OP[%04x], status[%02d]\n",
                idx, pMsg->hdr.b.opCode, pMsg->hdr.b.status);
        }

        #if 0
            for (idx = 0; idx < HW_MAX_IO_CMD_INDEX; idx ++)
            {
                cacheMsg = (MsgCacheRelObj_t*) &smCacheRelMsg[idx];
                M_PRINTF("ShareMsg[%04d] OP[%04x], SSD_Idx[%05d], CSS[%04x], SSD_Cnt[%04d], status[%02d]\n",
                    idx, cacheMsg->hdr.b.opCode, cacheMsg->bufDesc.ssdIndex, cacheMsg->bufDesc.css,
                    cacheMsg->bufDesc.ssdCnt, cacheMsg->hdr.b.status);
            }
        #endif

        M_PRINTF("CacheRelMsg Curr Idx = [%04d]\n", smCacheRelMsgIdx);

        M_PRINTF(" ========== Dump SRAM Bitmap ===========\n");


        M_PRINTF("GP[%02x], Cnt[%04d]\n",
            rNvmeBitmapSram.bitmapSramCmdCompletiBitmapGroupBitmap.b.CCBM_GBM,
            rNvmeBitmapSram.bitmapSramCmdCompletiBitmapCntr.b.CCBM_CNTR);


        M_PRINTF("[IDX][CIABM][CABM0][CCBM][FwCabm][DTDBM]\n");
        for (idx = 0; idx < cNvmeNumOfBitmapEntries; idx ++)
        {
            M_PRINTF("%d->%08X|%08x|%08X|%08X|%08x\n",
            idx, rNvmeBitmap.CIABM[idx], CABM_DW(idx), CCBM_DW(idx), NvmeHba.bitmapFwCabm[idx], rNvmeBitmap.DTDBM[idx]);
        }

        DumpHwIoCmd();

    #if 0
        D_PRINTF("HW_IO_CMD SRAM\n");
        Debug_DumpMemU32(0xF5401000, 0x4000);

        D_PRINTF("Bitmap SRAM\n");
        Debug_DumpMemU32(0xF541E000, 0x200);

        D_PRINTF("All Status manager registers\n");
        Debug_DumpMemU32(0xF5015000, 0x248);
    #endif
    }
    else if (allFlag == 2)
    {
        #if (NVME_REGISTER_DUMP == TRUE)
        HalNvmeDebug_DumpNvmeHwInfo(0);
        #endif
    }

    for (idx = 0; idx < cNvmeNumOfQueueFunc; idx ++)
    {
        if (rNvmeController.queueStatus[idx].sq.b.headPtr != rNvmeController.queueStatus[idx].sq.b.tailDbPtr)
        {
            M_PRINTF("%02d:SQ H/T:[%08x|%08]\n",
                idx, rNvmeController.queueStatus[idx].sq.b.headPtr, rNvmeController.queueStatus[idx].sq.b.tailDbPtr);
        }
    }

    for (idx = 0;idx < cMsgPoolLast; idx++)
    {
        if (smMsgQueue[idx].readCount != smMsgQueue[idx].writeCount)
        {
            M_PRINTF("MSG[%d] R[%X]W[%X]\n",idx, smMsgQueue[idx].readCount, smMsgQueue[idx].writeCount);
        }
    }

    /*
    M_PRINTF("--- PCIe Core registers -------------\n");
    Debug_DumpMemU32((uint32_t)0xF0080000, 0x104);
    M_PRINTF("--- PCIe PHY registers 1 ------------\n");
    Debug_DumpMemU32((uint32_t)0xF00C0000, 0x520);
    M_PRINTF("--- PCIe PHY registers 2 ------------\n");
    Debug_DumpMemU32((uint32_t)0xF00C0600, 0x184);
    M_PRINTF("--- Clock and PLL registers ---------\n");
    Debug_DumpMemU32((uint32_t)0xF0112000, 0x98);
    M_PRINTF("--- PCIe 3.0 registers 2 ------------\n");
    Debug_DumpMemU32((uint32_t)0xF0020010, 0x1C4);
    M_PRINTF("--- APB Registers -------------------\n");
    Debug_DumpMemU32((uint32_t)0xF0115000, 0x124);

    M_PRINTF("--- NVMe Registers -------------------\n");

    Debug_DumpMemU32((uint32_t)0xF5000000, 0x251);

    Debug_DumpMemU32((uint32_t)0xF5010000, 0x42);

    Debug_DumpMemU32((uint32_t)0xF5011000, 0x79);

    Debug_DumpMemU32((uint32_t)0xF5012000, 0x27);

    Debug_DumpMemU32((uint32_t)0xF5013000, 0x5F);

    Debug_DumpMemU32((uint32_t)0xF5014000, 0xE);

    Debug_DumpMemU32((uint32_t)0xF5015000, 0x92);

    Debug_DumpMemU32((uint32_t)0xF5016000, 0x2A);

    Debug_DumpMemU32((uint32_t)0xF5017000, 0x29);

    Debug_DumpMemU32((uint32_t)0xF5018000, 0x41);

    M_PRINTF("--- PCIe 3.0 registers 1 ------------\n");
    Debug_DumpMemU32((uint32_t)0xF0000000, 0xB48);
    */
}

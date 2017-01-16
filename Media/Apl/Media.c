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
//! @brief MEDIA SIDE Firmware Partition Main Entry Point
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "MediaPkg.h"

#include "AplNfif.h"
#include "AplSeq.h"
#include "AplSch.h"
#include "AplDesc.h"
#include "AplReadRetry.h"
#include "Fio.h"
#include "Heap.h"
#include "SysInfo.h"
#include "Cache.h"
#include "IDS.h"
#include "PLP.h"

#include "Media.h"

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
static          Error_t Media_OneTimeInit(InitBootMode_t initMode);
static          void    Media_InitPatrolRead(void);
static __inline void    Media_PatrolReadBlock(uint16_t die, uint16_t block);
static          Error_t Media_WaitCPU2(void);
static          void    Media_SetPowerMode(DevicePowerMode_t powerMode);
static __inline void    Media_ScheduleJob(void);
static          void    Media_IssueJob(tFIO_JOB* pJob);
static          void    Media_HandleTrkDone(U32 trkId, PVOID pStatus);

#if _NAND_ == NAND_MLC
    static      void    Media_HandleTrkError_MLC(U32 trkId, TrkErrInfo_t* pErrInfo);
#elif _NAND_ == NAND_3DTLCG2
    static      void    Media_HandleTrkError_3DTLCG2(U32 trkId, TrkErrInfo_t* pErrInfo);
#endif

static          void    Media_ServiceLoop(void);
static          void    Media_ServiceLoopPLP(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
         uint32_t    gCurrPatrolBlock;
volatile uint32_t    gTargetPatrolBlock;
         uint32_t    gPatrolDie;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern uint32_t        gSchActivePowerWeight[FIO_JOB_CMD_COUNT];
extern U16           (*gCbfProcDesc[FIO_JOB_CMD_COUNT])(tFIO_JOB* pJob);
extern FioJobPool_t (*gCbfFioState[SCH_STATE_COUNT])(void);
extern FioJobPool_t (*gCbfPLPFioState[SCH_STATE_COUNT])(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    One time initialization for NAND media sub-system.

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Media_OneTimeInit(InitBootMode_t initMode)
{
    #if (FIO_JOB_HEAP_MEMORY == TRUE)
    Heap_OneTimeInit(initMode);
    #endif

    #if (CO_SUPPORT_PLP == TRUE)
    AplPlp_OneTimeInit(initMode);
    #endif

    Cache_OneTimeInit(initMode);

    #if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
    HalPm_OneTimeInit(initMode);
    #endif

    AplNfif_OneTimeInit(initMode);

    CPU1_ACK_CPU2();

    AplSch_OneTimeInit(initMode);

    AplReadRetry_OneTimeInit(initMode);

    Media_WaitCPU2();

    // Register callback functions
    HalSeq_RegiserCallback(SEQ_CB_TRK_DONE,  (SeqAppCallback_t)Media_HandleTrkDone);

    #if _NAND_ == NAND_MLC
    HalSeq_RegiserCallback(SEQ_CB_TRK_ERROR, (SeqAppCallback_t)Media_HandleTrkError_MLC);
    #elif _NAND_ == NAND_3DTLCG2
    HalSeq_RegiserCallback(SEQ_CB_TRK_ERROR, (SeqAppCallback_t)Media_HandleTrkError_3DTLCG2);
    #endif

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    media patrol read initialization
**/
//-----------------------------------------------------------------------------
static void Media_InitPatrolRead(void)
{
    RRJob_t* pRRJob;
    uint32_t block = 0;
    uint32_t cnt;
    uint32_t tick;
    uint16_t ch, ce, lun;

    tick = HalTimer_ReadTick();

    gCurrPatrolBlock   = 0;
    gTargetPatrolBlock = 0;
    gPatrolDie         = 0;

    while (block < gNfInfo.blocksPerPlane)
    {
        if (gRRInfo.jobCnt[RR_JOB_POOL_FREE] > (gNfInfo.lunsPerDevice * gNfInfo.devicesPerChannel))
        {
            for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
            {
                for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
                {
                    cnt = 0;
                    pRRJob = AplReadRetry_PopJob(RR_JOB_POOL_FREE);
                    MemCopy8U32((U32*)&pRRJob->pMsgHost, (U32*)&gRRJobTpl);

                    for (ch = 0; ch < gNfInfo.numChannels; ch++)
                    {
                        gFastPaaBuf[cnt].all32   = 0;
                        gFastPaaBuf[cnt].b.ch    = ch;
                        gFastPaaBuf[cnt].b.ce    = ce;
                        gFastPaaBuf[cnt].b.lun   = lun;
                        gFastPaaBuf[cnt].b.block = block;
                        gFastPaaBuf[cnt].b.page  = gNfInfo.pagesPerBlock - 1;
                        cnt++;
                    }

                    MemCopyNU32((U32*)pRRJob->paa, (U32*)gFastPaaBuf, sizeof(tPAA) * cnt);

                    pRRJob->cmd    = FIO_JOB_CMD_READ_SLC_TOUCH;
                    pRRJob->plane  = gNfInfo.planesPerLun;
                    pRRJob->paaCnt = cnt;

                    while ((gSchInfo.issueDieCnt - gSchInfo.doneDieCnt) >= SCH_MAX_ACTIVE_READ_DIES || !AplSeq_HasFreeTrk());
                    Media_IssueJob(pRRJob);
                }
            }

            block++;
        }
    }

    HalTimer_DelayMsec(100);

    D_PRINTF("[SLCR] %d ms\n", HalTimer_GetElapsedTimeMsec(tick));
}

//-----------------------------------------------------------------------------
/**
    media patrol read block
**/
//-----------------------------------------------------------------------------
static __inline void Media_PatrolReadBlock(uint16_t die, uint16_t block)
{
    RRJob_t* pRRJob;
    uint8_t ch;

    pRRJob = AplReadRetry_PopJob(RR_JOB_POOL_FREE);
    MemCopy8U32((U32*)&pRRJob->pMsgHost, (U32*)&gRRJobTpl);

    gFastPaaBuf[0].all32   = 0;
    gFastPaaBuf[0].b.ce    =  die                        % gNfInfo.devicesPerChannel;
    gFastPaaBuf[0].b.lun   = (die >> gNfInfo.bitsDevice) % gNfInfo.lunsPerDevice;
    gFastPaaBuf[0].b.block = block;
    gFastPaaBuf[0].b.page  = gNfInfo.pagesPerBlock - 1;

    for (ch = 1; ch < gNfInfo.numChannels; ch++)
    {
        gFastPaaBuf[ch]      = gFastPaaBuf[0];
        gFastPaaBuf[ch].b.ch = ch;
    }

    MemCopyNU32((U32*)pRRJob->paa, (U32*)gFastPaaBuf, sizeof(tPAA) * gNfInfo.numChannels);

    pRRJob->cmd    = FIO_JOB_CMD_READ_SLC_TOUCH;
    pRRJob->plane  = gNfInfo.planesPerLun;
    pRRJob->paaCnt = gNfInfo.numChannels;

    FIO_PushJob(FIO_JOB_POOL_WAIT, (tFIO_JOB*)pRRJob);
}

//-----------------------------------------------------------------------------
/**
    medai wait CPU2 fio initialized

    @return cEcNoError
**/
//-----------------------------------------------------------------------------
static Error_t Media_WaitCPU2(void)
{
    U32 timeout = 0;

    D_PRINTF("[MEDA] Wait Core\n");

    while (smCpu1WaitCpu2Flag != SYNC_CPU1_W_CPU2_FLAG && (timeout++) < 0xF0000000); // Wait Nand info
    ASSERT(timeout < 0xF0000000);

    return(cEcNoError);
}

#if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
//-----------------------------------------------------------------------------
/**
    MediaMain Set Power Mode
**/
//-----------------------------------------------------------------------------
static void Media_SetPowerMode(DevicePowerMode_t powerMode)
{
    switch (powerMode)
    {
        case cDevicePmSleep:
            HalPm_EnterExitPowerDownMode();
            break;
        case cDevicePmDeepSleep:
            HalPm_EnterDeepPowerDownMode();
            break;
        case cDevicePmActive:
            break;
        default:
            HALT();
            break;
    }
}
#endif

//-----------------------------------------------------------------------------
/**
    media flash I/O scheduler
**/
//-----------------------------------------------------------------------------
static __inline void Media_ScheduleJob(void)
{
    if (AplSeq_HasFreeTrk())
    {
        FioJobPool_t pool;
        pool = gCbfFioState[gSchInfo.state]();

        if (pool != FIO_JOB_POOL_NULL && smFioInfo.jobCnt[pool])
        {
            tFIO_JOB* pJob;

            pJob = FIO_PopJob(pool);

            if (pJob->css == CACHE_R_IDS)
            {
                if (pJob->pMsgHost->bufDesc.prpRdy == cMcPrpReady) Media_IssueJob(pJob);
                else                                               FIO_PushHeadJob(FIO_JOB_POOL_WAIT, pJob);
            }
            else
            {
                Media_IssueJob(pJob);
            }

            if (pool <= FIO_JOB_POOL_READ)          gSchInfo.issueReadJobs++;
            else if (pool == FIO_JOB_POOL_WRITE)    gSchInfo.issueWriteJobs++;
            else if (pool == FIO_JOB_POOL_ERASE)    gSchInfo.issueEraseJobs++;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    media flash I/O scheduler

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
static void Media_IssueJob(tFIO_JOB* pJob)
{
    tPAA* pBakPaa;

    if (pJob->flag & FIO_F_AUX)
    {
        Media_WaitComplete();
        HalSeq_SwitchTrackOrder(SEQ_TRK_IN_ORDER);
    }

    // --- Setup flash seqment ---------------------------
    if (pJob->cmd < FIO_JOB_CMD_WRITE)
    {
        RR_REC_IO_CNT(pJob->fid);
        pJob->segNum = gSchInfo.currReadFS;
        if (++gSchInfo.currReadFS > FS_R_END)  gSchInfo.currReadFS = FS_R_START;
    }
    else if (pJob->cmd == FIO_JOB_CMD_WRITE)
    {
        pJob->segNum = gSchInfo.currWriteFS;
        if (++gSchInfo.currWriteFS > FS_W_END) gSchInfo.currWriteFS = FS_W_START;
    }

    if (pJob->fid < FIO_MAX_JOB && smFioInfo.errPaaCnt[pJob->fid])
    {
        FIO_RST_ERR_BMP(pJob->fid);
    }

    //--------------------------------------------------
    MEM_COPY(gFastPaaBuf, pJob->paa, sizeof(tPAA) * pJob->paaCnt);
    gSchInfo.issueDieCnt += gSchActivePowerWeight[pJob->cmd];

    //--------------------------------------------------
    pJob->tid = AplSeq_AllocTrkId(SEQ_TIMEOUT_WAIT);

    FIO_RunTimeCheck(pJob);

    smFioInfo.tid2PaaIdx[pJob->tid] = 0;
    smFioInfo.tid2Job[pJob->tid]    = pJob;
    gAplSeqInfo.trkIssueCnt++;

    pJob->trkAllocCnt++;
    pJob->timeStamp = smFioInfo.currTimeStamp++;

    //--------------------------------------------------
    pBakPaa   = pJob->paa;

    pJob->paa = gFastPaaBuf;
    gCbfProcDesc[pJob->cmd](pJob);
    pJob->paa = pBakPaa;

	ASSERT((pJob->descCnt <= MAX_DESC_PER_TRK) || (pJob->trkAllocCnt != 1));

    HalSeq_AddDescFifoCount(pJob->descCnt);

    if (pJob->flag & FIO_F_AUX)
    {
        Media_WaitComplete();
        HalSeq_SwitchTrackOrder(SEQ_TRK_OUT_OF_ORDER);
    }
}

//-----------------------------------------------------------------------------
/**
    media wait fio job done
**/
//-----------------------------------------------------------------------------
void Media_WaitComplete(void)
{
    U32 cnt = 0;

    while ((gAplSeqInfo.trkIssueCnt != gAplSeqInfo.trkDoneCnt) && cnt++ < SEQ_TIMEOUT_VALUE)
    {
        HalTimer_DelayUsec(10);
    }

    if (cnt >= SEQ_TIMEOUT_VALUE)
    {
        ASSERT(FALSE);
    }
}

//-----------------------------------------------------------------------------
/**
    media process track done

    @param[in]  track id
    @param[in]  pointer of TrkErrInfo_t
**/
//-----------------------------------------------------------------------------
static void Media_HandleTrkDone(U32 trkId, PVOID pStatus)
{
    tFIO_JOB* pJob;

    pJob = smFioInfo.tid2Job[trkId];

    AplSeq_FreeTrkId((U16)trkId);
    pJob->trkFreeCnt++;
    gAplSeqInfo.trkDoneCnt++;

    if (pJob->trkAllocCnt == pJob->trkFreeCnt)
    {
        gSchInfo.doneDieCnt += gSchActivePowerWeight[pJob->cmd];

        DESC_FIFO_UPDATE_R_PTR((pJob->descCnt << DESC_BYTE_SHIFT) + (pJob->descCnt << DESC_IDS_SHIFT));

        #if (CO_SUPPORT_NAND_BUSYTIME == TRUE)
        if (pJob->flag & FIO_F_NANDTIME)
        {
            U32 NandBusyTime[2];
            HalSeq_PopNandBusyTimeMonitors((PVOID)NandBusyTime,8);
            smNandBusyTime = ((NandBusyTime[0] * 320 + 500) / 1000);
        }
        #endif

        if (pJob->fid < FIO_MAX_JOB)
        {
            if (pJob->flag & FIO_F_AUX)
            {
                U32 idx;

                for (idx = 0; idx < pJob->paaCnt; idx++)
                {
                    if (FIO_CHK_ERR_BMP(pJob->fid, idx))
                    {
                        MEM_SET(&pJob->aux[(idx * 2)], BYTE_MASK, sizeof(U32) * 2);
                    }
                    else
                    {
                        MemCopy2U32((uint32_t*)&pJob->aux[(idx * 2)], (uint32_t*)&gSeqFifoInfo.auxExtract.pStart[(idx * AUX_SPM_MAX_CNT + AUX_SPM_IDX_LAA) * sizeof(U32)]);
                    }
                }

                ASM_DMB();
            }

            if ((pJob->status & FIO_ERR_RETRY_MASK) && ((pJob->flag & FIO_F_NO_RR) == 0))
            {
                AplReadRetry_PushJob(RR_JOB_POOL_BUSY, pJob);
            }
            else
            {
                IPC_SendMsgQ(cM2cComQueue, (Msg_t*)pJob);
            }
        }
        else
        {
            if (pJob->cmd == FIO_JOB_CMD_RR)
            {
                if (pJob->flag & FIO_F_AUX)
                {
                    tFIO_JOB* pErrJob;
                    uint32_t idx;
                    uint16_t srcIdx;

                    pErrJob = gRRInfo.RRJob2FioJob[pJob->fid - FIO_MAX_JOB];

                    for (idx = 0; idx < pJob->paaCnt; idx++)
                    {
                        srcIdx = pJob->paa[idx * RR_PAA_UNIT].rr.srcIdx;

                        if (RR_CHK_ERR_BMP(pJob->fid, idx) == 0)
                        {
                            MemCopy2U32((uint32_t*)&pErrJob->aux[srcIdx * 2], (uint32_t*)&gSeqFifoInfo.auxExtract.pStart[(idx * AUX_SPM_MAX_CNT + AUX_SPM_IDX_LAA) * sizeof(U32)]);
                        }
                    }

                    ASM_DMB();
                }

                AplReadRetry_PushJob(RR_JOB_POOL_BUSY, pJob);
            }
            else
            {
                AplReadRetry_PushJob(RR_JOB_POOL_FREE, pJob);
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    media process track error

    @param[in]  track id
    @param[in]  pointer of TrkErrInfo_t
**/
//-----------------------------------------------------------------------------
static void Media_HandleTrkError_MLC(U32 trkId, TrkErrInfo_t* pErrInfo)
{
    tFIO_JOB* pJob;
    tPAA      paa;
    U32       errStatus = 0;
    U16       paaIdx;
    U16       descIdx;

    paa.all32 = 0;
    pJob = smFioInfo.tid2Job[trkId];

    if (pErrInfo->trkLos)  // READ error
    {
        if      (pErrInfo->errStatus & EC_DEC_CUR_AU_UNCORR) errStatus = FIO_S_UNC_ERR;
        else if (pErrInfo->errStatus & EC_DEC_CUR_AU_ERASE)  errStatus = FIO_S_BLANK_ERR;
        else if (pErrInfo->errStatus & EC_DEC_CUR_AU_MCRC)   errStatus = FIO_S_BLANK_ERR;
        else if (pErrInfo->errStatus & EC_DEC_CUR_AU_OVRLMT) errStatus = FIO_S_OVER_LMT;
        else                                                 errStatus = FIO_S_UXP_ERR;

        if ((pErrInfo->errStatus & (EC_DEC_CUR_AU_ERASE | EC_DEC_CUR_AU_OVRLMT)) == (EC_DEC_CUR_AU_ERASE | EC_DEC_CUR_AU_OVRLMT))
        {
           errStatus = FIO_S_UNC_ERR;
           D_PRINTF("BLANK+OVERLMT=>UNC!!! TE\n");
        }

        paaIdx = 0;

        if (pJob->cmd == FIO_JOB_CMD_RR)
        {
            if (errStatus & FIO_ERR_RETRY_MASK)
            {
                paaIdx  = pErrInfo->errAuDid / 3;
                paaIdx += smFioInfo.tid2PaaIdx[trkId];
                paa     = pJob->paa[paaIdx * 2 + 1];

                RR_SET_ERR_BMP(pJob->fid, paaIdx);
                pJob->status |= errStatus;
            }
        }
        else
        {
            U16 cntOffset;

            descIdx   = 0;
            cntOffset = (pJob->cmd == FIO_JOB_CMD_READ_IDS_PAGE) ? 4 : 1;

            if ((pJob->cmd == FIO_JOB_CMD_READ) || (pJob->cmd == FIO_JOB_CMD_READ_IDS_PAGE))
            {
                while (descIdx < pErrInfo->errAuDid)
                {
                    if (DESC_CHK_OPEN_PG_BMP(trkId, descIdx) == 0)
                    {
                        paaIdx += cntOffset;
                    }
                    descIdx++;
                }

                if (pJob->cmd == FIO_JOB_CMD_READ_IDS_PAGE)
                {
                    paaIdx = paaIdx + pErrInfo->errAuNum;
                }
            }
            else
            {
                paaIdx = pErrInfo->errAuDid;
                if (pJob->flag & FIO_F_SLC)
                {
                    paaIdx = (pErrInfo->errAuDid << 2) + pErrInfo->errAuNum;
                }
            }

            paaIdx += smFioInfo.tid2PaaIdx[trkId];
            paa = pJob->paa[paaIdx];

            FIO_SET_ERR_BMP(pJob->fid, paaIdx);
            pJob->status |= errStatus;
        }

        if ((pJob->flag & FIO_F_MUTE) == 0)
        {
            if (pJob->flag & FIO_F_MUTE_BLANK_ERR)
            {
                errStatus &= ~FIO_S_BLANK_ERR;
            }

            if (errStatus)
            {
                M_PRINTF("[R] S%1X F/T[%3d.%3d] CSS%2d PI/C[%3d.%3d] [%d.%d.%d.%d.%4X.%4X.%3X]\n",
                      errStatus, pJob->fid, trkId,
                      pJob->css, paaIdx, pJob->paaCnt,
                      paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, ((paa.b.block << gNfInfo.bitsPlane) + paa.b.plane), paa.b.page);
            }
        }
    }
    else if (pErrInfo->trkPos)
    {
        U16 descOffet;
        U16 descPerPlane;
        U8  planeOffet;
        U8  errplaneshift;
        U8  ds = 0;

        for (descIdx = 0; descIdx < pJob->descCnt; descIdx++)
        {
            ds = pErrInfo->trkDescStatus[descIdx >> NUM_DS_PER_DWORD_SHIFT] >> (NUM_BITS_PER_DS * (descIdx & NUM_DS_PER_DWORD_MASK));
            if (ds)
            {
                descOffet     = 0;
                descPerPlane  = 0;
                planeOffet    = 0;
                errplaneshift = 0;

                if (pJob->cmd == FIO_JOB_CMD_WRITE)
                {
                    descOffet     = ((descIdx & ((pJob->descCnt / pJob->plane) - 1)) << 2);
                    descPerPlane  = 0;
                    errplaneshift = (pJob->plane == FIO_CMD_1PLANE) ? 0 : 1;
                }
                else
                {
                    descOffet     = (pJob->plane == FIO_CMD_1PLANE) ? descIdx : (descIdx & ((pJob->descCnt / pJob->plane) - 1));
                    descPerPlane  = (pJob->plane == FIO_CMD_1PLANE) ? 1 : (pJob->descCnt / pJob->plane);
                    errplaneshift = (pJob->plane == FIO_CMD_1PLANE) ? 0 : 1;
                }

                for (planeOffet = 0; planeOffet < pJob->plane; planeOffet++)
                {
                    if (ds & BIT(errplaneshift + planeOffet))
                    {
                        paaIdx = (pJob->cmd == FIO_JOB_CMD_WRITE) ? (descOffet + pJob->descCnt * planeOffet) : (descOffet + descPerPlane * planeOffet);

                        paaIdx = paaIdx + smFioInfo.tid2PaaIdx[trkId];

                        FIO_SET_ERR_BMP(pJob->fid, paaIdx);

                        paa = pJob->paa[paaIdx];

                        if ((pJob->flag & FIO_F_MUTE) == 0)
                        {
                            M_PRINTF("[%c] DS%2X F/T[%3d.%3d] CSS%2d PI/D/C[%3d.%3d.%3d] [%d.%d.%d.%d.%4X.%4X.%3X]\n",
                            (pJob->cmd == FIO_JOB_CMD_ERASE) ? 'E' : 'W',
                            ds, pJob->fid, trkId, pJob->css, paaIdx, descIdx, pJob->descCnt,
                            paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, ((paa.b.block << gNfInfo.bitsPlane) + paa.b.plane), paa.b.page);
                        }
                    }
                }
            }
        }

        pJob->status |= ((pJob->cmd == FIO_JOB_CMD_ERASE) ? FIO_S_ERASE_ERR : FIO_S_PROG_ERR);
    }
    else
    {
        M_PRINTF("Undef TrkERR: [%s] S[0x%08X]\n",(pErrInfo->errAuType == 0) ? "DEC": "ENC", pErrInfo->errStatus);
        ASSERT(FALSE);
    }
}

//-----------------------------------------------------------------------------
/**
    media process track error

    @param[in]  track id
    @param[in]  pointer of TrkErrInfo_t
**/
//-----------------------------------------------------------------------------
static void Media_HandleTrkError_3DTLCG2(U32 trkId, TrkErrInfo_t* pErrInfo)
{
    tFIO_JOB* pJob;
    tPAA      paa;
    U32       errStatus = 0;
    U16       paaIdx;
    U16       descIdx;

    paa.all32 = 0;

    pJob = smFioInfo.tid2Job[trkId];

    if (pErrInfo->trkLos)  // READ error
    {
        if      (pErrInfo->errStatus & EC_DEC_CUR_AU_UNCORR) errStatus = FIO_S_UNC_ERR;
        else if (pErrInfo->errStatus & EC_DEC_CUR_AU_ERASE)  errStatus = FIO_S_BLANK_ERR;
        else if (pErrInfo->errStatus & EC_DEC_CUR_AU_MCRC)   errStatus = FIO_S_BLANK_ERR;
        else if (pErrInfo->errStatus & EC_DEC_CUR_AU_OVRLMT) errStatus = FIO_S_OVER_LMT;
        else                                                 errStatus = FIO_S_UXP_ERR;

        if ((pErrInfo->errStatus & EC_DEC_CUR_AU_ERASE) && (pErrInfo->errStatus & EC_DEC_CUR_AU_OVRLMT))
        {
            errStatus = FIO_S_UNC_ERR;
            D_PRINTF("BLANK+OVERLMT=>UNC!\n");
        }

        paaIdx  = 0;

        if (pJob->cmd == FIO_JOB_CMD_RR)
        {
            if (errStatus & FIO_ERR_RETRY_MASK)
            {
                paaIdx  = pErrInfo->errAuDid / 3;
                paaIdx += smFioInfo.tid2PaaIdx[trkId];
                paa     = pJob->paa[paaIdx * 2 + 1];

                RR_SET_ERR_BMP(pJob->fid, paaIdx);
                pJob->status |= errStatus;
            }
        }
        else
        {
            U16 cntOffset;

            descIdx = 0;
            cntOffset = (pJob->cmd == FIO_JOB_CMD_READ_IDS_PAGE || pJob->flag & FIO_F_PAGE_READ) ? 4 : 1;

            if (pJob->cmd == FIO_JOB_CMD_READ || pJob->cmd == FIO_JOB_CMD_READ_IDS || pJob->cmd == FIO_JOB_CMD_READ_IDS_PAGE)
            {
                while (descIdx < pErrInfo->errAuDid)
                {
                    if (DESC_CHK_OPEN_PG_BMP(trkId, descIdx) == 0)
                    {
                        paaIdx += cntOffset;
                    }
                    descIdx++;
                }

                if (pJob->cmd == FIO_JOB_CMD_READ_IDS_PAGE || pJob->flag & FIO_F_PAGE_READ)
                {
                    paaIdx = paaIdx + pErrInfo->errAuNum;
                }
            }
            else
            {
                paaIdx = pErrInfo->errAuDid;
                if (pJob->flag & FIO_F_SLC)
                {
                    paaIdx = (pErrInfo->errAuDid << 2) + pErrInfo->errAuNum;
                }
            }

            paaIdx += smFioInfo.tid2PaaIdx[trkId];
            paa     = pJob->paa[paaIdx];

            FIO_SET_ERR_BMP(pJob->fid, paaIdx);
            pJob->status |= errStatus;
        }

        if ((pJob->flag & FIO_F_MUTE) == 0)
        {
            if (pJob->flag & FIO_F_MUTE_BLANK_ERR)
            {
                errStatus &= ~FIO_S_BLANK_ERR;
            }

            if (errStatus & FIO_S_ERR_MASK)
            {
                M_PRINTF("[R] S%1X F/T[%3d.%3d] CSS%2d PI/C[%3d.%3d] [%d.%d.%d.%d.%4X.%4X.%3X(%3d.%2d)]\n",
                          errStatus, pJob->fid, trkId,
                          pJob->css, paaIdx, pJob->paaCnt,
                          paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, ((paa.b.block << gNfInfo.bitsPlane) + paa.b.plane), paa.b.page, paa.b.page/3, paa.b.page/12);
            }
        }
    }
    else if (pErrInfo->trkPos)
    {
        U8  errplanebit;
        U8  chIdx;
        U8  ds;
        U8  planeOffet;

        ds = 0;
        chIdx = 0;

        for (descIdx = 0; descIdx < pJob->descCnt; descIdx++)
        {
            planeOffet  = 0;
            errplanebit = 0;

            ds = pErrInfo->trkDescStatus[descIdx >> NUM_DS_PER_DWORD_SHIFT] >> (NUM_BITS_PER_DS * (descIdx & NUM_DS_PER_DWORD_MASK));
            if (ds)
            {
                chIdx = (pJob->cmd == FIO_JOB_CMD_ERASE) ? (descIdx - pJob->paaCnt) : (descIdx - (pJob->paaCnt >> 2));

                for (planeOffet = 0; planeOffet < pJob->plane; planeOffet++)
                {
                    errplanebit = (pJob->flag & FIO_F_SLC) ? 0x2 : planeOffet;

                    if (ds & BIT(errplanebit))
                    {
                        if (pJob->flag & FIO_F_SLC)
                        {
                            paaIdx = (pJob->cmd == FIO_JOB_CMD_WRITE) ? ((descIdx - planeOffet) << 2) : (descIdx - planeOffet);
                        }
                        else
                        {
                            paaIdx = (pJob->cmd == FIO_JOB_CMD_WRITE) ? ((chIdx * pJob->plane + planeOffet) << 2) : chIdx * pJob->plane + planeOffet;
                        }

                        paaIdx = paaIdx + smFioInfo.tid2PaaIdx[trkId];

                        FIO_SET_ERR_BMP(pJob->fid, paaIdx);

                        paa = pJob->paa[paaIdx];

                        if ((pJob->flag & FIO_F_MUTE) == 0)
                        {
                            M_PRINTF("[%c] DS%2X F/T[%3d.%3d] CSS%2d PI/D/C[%3d.%3d.%3d] [%d.%d.%d.%d.%4X.%4X.%3X]\n",
                            (pJob->cmd == FIO_JOB_CMD_ERASE) ? 'E' : 'W',
                            ds, pJob->fid, trkId, pJob->css, paaIdx, descIdx, pJob->descCnt,
                            paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, ((paa.b.block << gNfInfo.bitsPlane) + paa.b.plane), paa.b.page);
                        }
                    }
                }
            }
        }

        pJob->status |= ((pJob->cmd == FIO_JOB_CMD_ERASE) ? FIO_S_ERASE_ERR : FIO_S_PROG_ERR);
    }
    else
    {
        M_PRINTF("Undef TrkERR: [%s] S[0x%08X]\n",(pErrInfo->errAuType == 0) ? "DEC": "ENC", pErrInfo->errStatus);
        ASSERT(FALSE);
    }
}

//-----------------------------------------------------------------------------
/**
    media service task startup
**/
//-----------------------------------------------------------------------------
void Media_StartTask(void)
{
    InitBootMode_t initMode;

    initMode = Hal_GetInitBootMode();

    Media_OneTimeInit(initMode);

    #if CO_SUPPORT_PATROL_READ == TRUE
    Media_InitPatrolRead();
    #endif

    Media_ServiceLoop();
}

/*____________________________________________________________________________________
  __  __          _ _         ____                  _            _
 |  \/  | ___  __| (_) __ _  / ___|  ___ _ ____   ___) ___ ___  | |    ___   ___  _ __
 | |\/| |/ _ \/ _` | |/ _` | \___ \ / _ \ '__\ \ / / |/ __/ _ \ | |   / _ \ / _ \| '_ \
 | |  | |  __/ (_| | | (_| |  ___) |  __/ |   \ V /| | (__  __/ | |___ (_) | (_) | |_) |
 |_|  |_|\___|\__,_|_|\__,_| |____/ \___|_|    \_/ |_|\___\___| |_____\___/ \___/| .__/
                                                                                 |_|
 ____________________________________________________________________________________*/
// Please do NOT break this while loop
static void Media_ServiceLoop(void)
{
    D_PRINTF("[MEDA] ServiceLoop\n");

    while (TRUE)
    {
        #if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
        while (PM_MEDIA_LOCK_REQUEST())
        {
            Media_SetPowerMode(smPmCtrl[CPU_ID1].PowerMode);
        }
        #endif

        if (IPC_MsgQFastPeek(cC2mReqQueue) && PLP_GET_STATUS())
        {
            FIO_PushJob(FIO_JOB_POOL_WAIT, (tFIO_JOB*) IPC_GetMsgQ(cC2mReqQueue));
        }

        if (gSchInfo.queuingJobs && PLP_GET_STATUS())
        {
            Media_ScheduleJob();
        }

        if (gRRInfo.jobCnt[RR_JOB_POOL_BUSY] && PLP_GET_STATUS())
        {
            AplReadRetry_Entrance();
        }

        #if CO_SUPPORT_PATROL_READ == TRUE
        if (gCurrPatrolBlock != gTargetPatrolBlock)
        {
            if (gRRInfo.jobCnt[RR_JOB_POOL_FREE] > RR_MIN_JOB && AplSeq_HasFreeTrk())
            {
                Media_PatrolReadBlock(gPatrolDie++, gCurrPatrolBlock);

                if (gPatrolDie == (gNfInfo.lunsPerDevice * gNfInfo.devicesPerChannel))
                {
                    gPatrolDie = 0;
                    if (++gCurrPatrolBlock >= gNfInfo.blocksPerPlane)
                    {
                        gCurrPatrolBlock = 0;
                    }
                }
            }
        }
        #endif

        #if (CO_SUPPORT_PLP == TRUE)
        if (PLP_GET_STATUS() == PLP_DISCHARGE)
        {
            Media_ServiceLoopPLP();
        }
        #endif
    }
}

/*____________________________________________________________________________________
  _____  _      _____     _____                 _          _
 |  __ \| |    |  __ \   / ____|               (_)        | |
 | |__) | |    | |__) | | (___   ___ _ ____   ___  ___ ___| |     ___   ___  _ __
 |  ___/| |    |  ___/   \___ \ / _ \ '__\ \ / / |/ __/ _ \ |    / _ \ / _ \| '_ \
 | |    | |____| |       ____) |  __/ |   \ V /| | (_|  __/ |___| (_) | (_) | |_) |
 |_|    |______|_|      |_____/ \___|_|    \_/ |_|\___\___|______\___/ \___/| .__/
                                                                            | |
                                                                            |_|
____________________________________________________________________________________*/
// For PLP Only, please do NOT jump call this service loop arbitrary!!!
static void Media_ServiceLoopPLP(void)
{
    tFIO_JOB* pJob = NULL;

    FIO_PLPFreezeJobs();

    if (gSchInfo.state != SCH_STATE_PROC_READ) gSchInfo.state = SCH_STATE_WAIT_READ;

    while (TRUE)
    {
        // Get Fio Job
        if (IPC_MsgQFastPeek(cC2mReqQueue))
        {
            pJob = (tFIO_JOB*) IPC_GetMsgQ(cC2mReqQueue);

            if (pJob->css == CACHE_LASTPAGE || pJob->cmd == FIO_JOB_CMD_ERASE)
            {
                FIO_PushJob(FIO_JOB_POOL_FREEZE, pJob);
            }
            else
            {
                FIO_PushJob(FIO_JOB_POOL_WAIT, pJob);
            }
        }

        // SCH & Issue Job
        if (AplSeq_HasFreeTrk())
        {
            FioJobPool_t pool;

            pool = gCbfPLPFioState[gSchInfo.state]();

            if (pool != FIO_JOB_POOL_NULL && smFioInfo.jobCnt[pool])
            {
                pJob = FIO_PopJob(pool);

                Media_IssueJob(pJob);
            }
        }

        // ReadRetry
        if (gRRInfo.jobCnt[RR_JOB_POOL_BUSY])
        {
            AplReadRetry_Entrance();
        }

        // Check PLP Done
        if (PLP_GET_STATUS())
        {
            FIO_PLPUnfreezeJobs();
            gSchInfo.state = SCH_STATE_IDLE;

            break;
        }
    }
}

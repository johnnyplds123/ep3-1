// ============================================================================
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
/*
    This computer program contains confidential and proprietary information,
    and may NOT be reproduced or transmitted, in whole or in part, in any form,
    or by any means electronic, mechanical, photo-optical, or otherwise, and
    may NOT be translated into another language without the express written
    permission from LITEON Corporation.
*/
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
//=============================================================================
//
//! @brief Read Retry
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "Media.h"
#include "Fio.h"
#include "Cache.h"

#include "AplReadRetry.h"

#include "PerfSection.h"
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
static      uint16_t ReadRetry_BuildSSD(RRJob_t* pRRJob, uint16_t errPaaIdx, uint16_t otherIdx);
static __inline void ReadRetry_ReleaseSSD(RRJob_t* pRRJob);
static __inline void ReadRetry_RestoreErrMap(RRJob_t* pRRJob, tFIO_JOB* pErrJob);

static RRRetStatus_t ReadRetry_CreateJob(RRJob_t* pRRJob);
static RRRetStatus_t ReadRetry_IssueJob(RRJob_t* pRRJob);
static RRRetStatus_t ReadRetry_ReleaseJob(RRJob_t* pRRJob);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
RRInfo_t    gRRInfo;
RRJob_t     gRRJobBuf[RR_MAX_JOB];
FioJobTpl_t gRRJobTpl;

#if RR_STATISTIC == ENABLE
RRStat_t    gRRStat;
#endif

static RRRetStatus_t (*gCbfReadRetryState[RR_STATE_COUNT])(RRJob_t* pRRJob) =
{
    ReadRetry_CreateJob,
    ReadRetry_IssueJob,
    ReadRetry_ReleaseJob
};

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    Initial and allocate readretry variables

    @param[in]  initMode

    @return cEcNoError
**/
//-----------------------------------------------------------------------------
Error_t AplReadRetry_OneTimeInit(InitBootMode_t initMode)
{
    tPAA*     pPaaBuf;
    uint32_t* pAuxBuf;
    uint16_t  idx;

    MEM_CLR(gRRJobBuf, sizeof(RRJob_t) * RR_MAX_JOB);
    MEM_CLR(&gRRInfo, sizeof(RRInfo_t));

    pPaaBuf = MEM_AllocBuffer(sizeof(tPAA) * (RR_MAX_JOB * MAX_PAA_PER_JOB), 512);
    pAuxBuf = MEM_AllocBuffer(sizeof(U32) *  (RR_MAX_JOB * MAX_AUX_PER_JOB), 512);

    for (idx = 0; idx < RR_MAX_JOB; idx++)
    {
        gRRJobBuf[idx].fid = FIO_MAX_JOB + idx;

        if (idx == 0)
        {
            gRRJobBuf[idx].prev = NULL;
            gRRJobBuf[idx].next = &gRRJobBuf[idx + 1];
        }
        else if (idx == RR_MAX_JOB - 1)
        {
            gRRJobBuf[idx].prev = &gRRJobBuf[idx - 1];
            gRRJobBuf[idx].next = NULL;
        }
        else
        {
            gRRJobBuf[idx].prev = &gRRJobBuf[idx - 1];
            gRRJobBuf[idx].next = &gRRJobBuf[idx + 1];
        }

        gRRJobBuf[idx].paa = &pPaaBuf[idx * MAX_PAA_PER_JOB];
        gRRJobBuf[idx].aux = &pAuxBuf[idx * MAX_AUX_PER_JOB];
    }

    gRRInfo.head[RR_JOB_POOL_FREE]   = &gRRJobBuf[0];
    gRRInfo.tail[RR_JOB_POOL_FREE]   = &gRRJobBuf[RR_MAX_JOB - 1];
    gRRInfo.jobCnt[RR_JOB_POOL_FREE] = RR_MAX_JOB;

    MEM_CLR(&gRRJobTpl, sizeof(FioJobTpl_t));
    gRRJobTpl.pMsgHost            = NULL;
    gRRJobTpl.ssdIdx[SSD_W_CACHE] = SSD_NULL_START;
    gRRJobTpl.ssdIdx[SSD_WO_CACHE]= SSD_NULL_START;
    gRRJobTpl.css                 = CACHE_AUTO;
    gRRJobTpl.cmd                 = FIO_JOB_CMD_RR;
    gRRJobTpl.auFmt               = gEcuInfo.auFormat;
    gRRJobTpl.ssdList             = SSD_WO_CACHE;
    gRRJobTpl.plane               = 1;
    gRRJobTpl.segNum              = NUM_OF_FS_SEG;

    RR_REC_RESET();

    return cEcNoError;
}

#include "PerfSection.h"
//-----------------------------------------------------------------------------
/**
    Read retry entrance point
**/
//-----------------------------------------------------------------------------
void AplReadRetry_Entrance(void)
{
    RRJob_t* pRRJob;
    RRRetStatus_t ret;

    pRRJob = AplReadRetry_PopJob(RR_JOB_POOL_BUSY);

    if (pRRJob->fid < FIO_MAX_JOB)
    {
        tFIO_JOB* pErrJob;

        pErrJob = pRRJob;
        if ((pErrJob->status & FIO_S_BLANK_ERR) == FIO_S_BLANK_ERR)
        {
            // TODO: Skip read retry when Aux GC and FTL scanpage hit blank page (not best solution)
            if (pErrJob->css == CACHE_GCREAD || pErrJob->css == CACHE_SCANPAGE || pErrJob->css == CACHE_GCREADAUX)
            {
                BEGIN_CS1
                IPC_SendMsgQ(cM2cComQueue, (Msg_t*)pErrJob);
                END_CS1

                return;
            }
        }

        if (gRRInfo.jobCnt[RR_JOB_POOL_FREE])
        {
            pRRJob = AplReadRetry_PopJob(RR_JOB_POOL_FREE);
            pRRJob->rrState  = RR_STATE_CREATE;
            pRRJob->rrCount  = 0;
            pErrJob->rrCount = 0;
            gRRInfo.scanPaaStartIdx[pRRJob->fid - FIO_MAX_JOB] = 0;
            gRRInfo.RRJob2FioJob[pRRJob->fid - FIO_MAX_JOB] = pErrJob;
        }
        else
        {
            AplReadRetry_PushJob(RR_JOB_POOL_BUSY, (RRJob_t*)pErrJob);
            return;
        }
    }

    do {
        ret = gCbfReadRetryState[pRRJob->rrState](pRRJob);

        if (ret != RR_RET_CONTINUE)
        {
            if (ret == RR_RET_POSTPONE)
            {
                AplReadRetry_PushJob(RR_JOB_POOL_BUSY, pRRJob);
            }

            break;
        }
    } while(true);
}

//-----------------------------------------------------------------------------
/**
    Pop a read retry job from specificed pool

    @param[in]  pool

    @return read retry job
**/
//-----------------------------------------------------------------------------
RRJob_t* AplReadRetry_PopJob(RRPool_t pool)
{
    RRJob_t* pJob;

    ASSERT(gRRInfo.jobCnt[pool]);

    BEGIN_CS1
    pJob = gRRInfo.head[pool];

    if (pJob == gRRInfo.tail[pool])
    {
        gRRInfo.head[pool] = gRRInfo.tail[pool] = NULL;
    }
    else
    {
        gRRInfo.head[pool] = pJob->next;
        pJob->next->prev   = NULL;
    }

    pJob->next = pJob->prev = NULL;

    gRRInfo.jobCnt[pool]--;
    END_CS1

    return pJob;
}

//-----------------------------------------------------------------------------
/**
    Push a read retry job into specified pool

    @param[in]  pool
    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void AplReadRetry_PushJob(RRPool_t pool, RRJob_t* pJob)
{
    ASSERT(gRRInfo.jobCnt[pool] < FIO_MAX_JOB + RR_MAX_JOB);

    BEGIN_CS1
    if (gRRInfo.tail[pool] == NULL)
    {
        gRRInfo.head[pool] = gRRInfo.tail[pool] = pJob;
    }
    else
    {
        gRRInfo.tail[pool]->next = pJob;
        pJob->prev               = gRRInfo.tail[pool];
        gRRInfo.tail[pool]       = pJob;
    }

    gRRInfo.jobCnt[pool]++;
    END_CS1
}

#include "NormSection.h"
//-----------------------------------------------------------------------------
/**
    Dump ReadRetry information
**/
//-----------------------------------------------------------------------------
void AplReadRetry_DumpInfo(uint32_t level, uint32_t param)
{
    RRPool_t pool;
    RRJob_t* pJob;
    uint32_t idx, idx2;

    M_PRINTF("[RR Info]\n");

    #if RR_STATISTIC == ENABLE
    {
        uint32_t errIOs    = 0;
        uint32_t errIOsRRC = 0;

        if (level == 0xC)   RR_REC_RESET();

        //#if _NAND_ == NAND_3DTLCG2
        #if 0
        D_PRINTF("\nPage:");
        for (idx = 0; idx < gNfInfo.pagesPerBlock; idx++)
        {
            if ((idx % 9) == 0) D_PRINTF("\nP(%3d ~ %3d): ", idx, idx + 8);
            D_PRINTF("%8d ", gRRStat.errPage[idx]);
            errPages += gRRStat.errPage[idx];
        }

        D_PRINTF("\nWL:");
        for (idx = 0; idx < gNfInfo.pagesPerBlock / 3; idx++)
        {
            if ((idx & 7) == 0) D_PRINTF("\nW(%3d ~ %3d): ", idx, idx + 7);
            D_PRINTF("%8d ", (gRRStat.errPage[idx * 3] + gRRStat.errPage[idx * 3 + 1] + gRRStat.errPage[idx * 3 + 2]));
        }

        D_PRINTF("\nLayer:");
        for (idx = 0; idx < gNfInfo.pagesPerBlock / 12; idx++)
        {
            if ((idx & 3) == 0) D_PRINTF("\nL(%2d ~ %2d): ", idx, idx + 3);

            errIOs = 0;
            for (idx2 = 0; idx2 < 12; idx2++)
            {
                errIOs += gRRStat.errPage[idx * 12 + idx2];
            }

            D_PRINTF("%8d ", errIOs);
        }

        D_PRINTF("\n");
        #endif

        D_PRINTF("RRC:\n");
        errIOs = 0;
        for (idx = 1; idx <= RR_MAX_RETRY_COUNT; idx++)
        {
            errIOs    += gRRStat.errIO[idx];
            errIOsRRC += (gRRStat.errIO[idx] * idx);
            D_PRINTF("[%2d]:%d\n", idx, gRRStat.errIO[idx]);
        }
        D_PRINTF("[ F]:%d\n", gRRStat.errIO[0]);

        D_PRINTF("TotalIOs:0x%X%X, RRIOs:%d, RRIOs(W/ RRC):%d\n",
                        U64_TO_U32_H(gRRStat.totalReadIO), U64_TO_U32_L(gRRStat.totalReadIO), errIOs, errIOsRRC);
    }
    #endif

    for (pool = RR_JOB_POOL_FREE; pool < RR_JOB_POOL_COUNT; pool++)
    {
        M_PRINTF("Pool(%d) Cnt:%d\n", pool, gRRInfo.jobCnt[pool]);

        if (level == 1)
        {
            pJob = gRRInfo.head[pool];

            for (idx = 0; idx < gRRInfo.jobCnt[pool]; idx++)
            {
                M_PRINTF("%3d: STS:%04X Fid:%3d CSS:%2d PC:%3d RC:%2d, RS:%d\n", idx, pJob->status, pJob->fid, pJob->css, pJob->paaCnt, pJob->rrCount, pJob->rrState);

                for (idx2 = 0; idx2 < (pJob->paaCnt + RR_PAA_BMP_MASK); idx2 += RR_PAA_BMP_BITS_CNT)
                {
                    M_PRINTF(" %08X\n", RR_GET_ERR_BMP(pJob->fid, idx2));
                }

                pJob = pJob->next;
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    IDS read retry fail abort handler

    @param[in]  Retry Fail IDS Job
**/
//-----------------------------------------------------------------------------
static void ReadRetry_IDSAbortHandler(tFIO_JOB* pErrJob)
{
    tFIO_JOB*    pJob;
    U32          jobCnt;
    U32          timeout;
    FioJobPool_t pool;

    // Scan all read pools
    pool = (FioJobPool_t)(FIO_JOB_POOL_READ + FIO_R_POOL_CNT_NORMAL);
    while (pool < FIO_JOB_POOL_WRITE)
    {
        jobCnt = smFioInfo.jobCnt[pool];

        while (jobCnt--)
        {
            pJob = FIO_PopJob(pool);

            if (pJob->pMsgHost && (pJob->pMsgHost->attrInfo.b.cmdId == pErrJob->pMsgHost->attrInfo.b.cmdId))
            {
                if (pJob->fid >= FIO_MAX_JOB)
                {
                    AplReadRetry_PushJob(RR_JOB_POOL_FREE, pJob);
                    pJob = gRRInfo.RRJob2FioJob[pJob->fid - FIO_MAX_JOB];
                }

                pJob->status &= ~FIO_ERR_RETRY_MASK;

                BEGIN_CS1
                IPC_SendMsgQ(cM2cComQueue, (Msg_t*)pJob);
                END_CS1
            }
            else
            {
                FIO_PushJob(FIO_JOB_POOL_WAIT, pJob);
            }
        }

        pool++;
    }

    // Scan C2m queue
    while (IPC_MsgQFastPeek(cC2mReqQueue))
    {
        pJob = (tFIO_JOB*) IPC_GetMsgQ(cC2mReqQueue);
        if (pJob->pMsgHost && (pJob->pMsgHost->attrInfo.b.cmdId == pErrJob->pMsgHost->attrInfo.b.cmdId))
        {
            BEGIN_CS1
            IPC_SendMsgQ(cM2cComQueue, (Msg_t*)pJob);
            END_CS1
        }
        else
        {
            FIO_PushJob(FIO_JOB_POOL_WAIT, pJob);
        }
    }

    Media_WaitComplete();

    // Scan RR pool
    jobCnt = gRRInfo.jobCnt[RR_JOB_POOL_BUSY];

    while (jobCnt--)
    {
        pJob = AplReadRetry_PopJob(RR_JOB_POOL_BUSY);

        if (pJob->pMsgHost && (pJob->pMsgHost->attrInfo.b.cmdId == pErrJob->pMsgHost->attrInfo.b.cmdId))
        {
            if (pJob->fid >= FIO_MAX_JOB)
            {
                AplReadRetry_PushJob(RR_JOB_POOL_FREE, pJob);
                pJob = gRRInfo.RRJob2FioJob[pJob->fid - FIO_MAX_JOB];
            }

            pJob->status &= ~FIO_ERR_RETRY_MASK;

            BEGIN_CS1
            IPC_SendMsgQ(cM2cComQueue, (Msg_t*)pJob);
            END_CS1
        }
        else
        {
            AplReadRetry_PushJob(RR_JOB_POOL_BUSY, pJob);
        }
    }
}

#include "PerfSection.h"
//-----------------------------------------------------------------------------
/**
    Generate read retry job

    @param[in]  RRJob

    @return RRRetStatus_t
**/
//-----------------------------------------------------------------------------
static RRRetStatus_t ReadRetry_CreateJob(RRJob_t* pRRJob)
{
    tFIO_JOB* pErrJob;
    tPAA prevPaa;
    uint16_t paaIdx;
    uint16_t currSsdIdx = 0;
    uint16_t tailSsdIdx = 0;
    uint16_t minPaaCnt  = RR_PAA_UNIT;
    uint16_t minSsdCnt  = 1;

    pErrJob = gRRInfo.RRJob2FioJob[pRRJob->fid - FIO_MAX_JOB];
    MemCopy8U32((U32*)&pRRJob->pMsgHost, (U32*)&gRRJobTpl);

    //-----------------------------------------------------------------------------
    if (pErrJob->css != CACHE_R_IDS)
    {
        pRRJob->ssdList              = SSD_WO_CACHE;
        pRRJob->ssdIdx[SSD_WO_CACHE] = GET_SSD_INDEX_FROM_PTR(smCacheMgr.head[SSD_POOL_RR]);
        currSsdIdx                   = pRRJob->ssdIdx[SSD_WO_CACHE];

        if (pErrJob->css == CACHE_R_FILLUP_HIT || pErrJob->css == CACHE_FILLUP)
        {
            minPaaCnt = RR_PAA_UNIT * MAX_PAA_PER_FILLUP;
            minSsdCnt = MAX_SSD_PER_FILLUP;
        }

        if (smCacheMgr.ssdCnt[SSD_POOL_RR] < minSsdCnt)
        {
            pRRJob->rrState = RR_STATE_CREATE;
            return RR_RET_POSTPONE;
        }
    }

    RR_RST_ERR_BMP(pRRJob->fid);
    pRRJob->cmd       = FIO_JOB_CMD_RR;
    pRRJob->pMsgHost  = pErrJob->pMsgHost;
    pRRJob->paaOffset = pErrJob->paaOffset;
    pRRJob->poolNo    = pErrJob->poolNo;
    pRRJob->flag      = (pErrJob->flag & (FIO_F_AUX | FIO_F_MUTE | FIO_F_SLC | FIO_F_SCHEDULE));
    pRRJob->css       = pErrJob->css;

    //-----------------------------------------------------------------------------
    paaIdx = gRRInfo.scanPaaStartIdx[pRRJob->fid - FIO_MAX_JOB];
    prevPaa.all32 = DWORD_MASK;

    while (paaIdx < pErrJob->paaCnt && (pRRJob->paaCnt + minPaaCnt) <= MAX_PAA_PER_JOB)
    {
        if (pRRJob->css != CACHE_R_IDS)
        {
            if (smCacheMgr.ssdCnt[SSD_POOL_RR] < (pRRJob->ssdCnt[SSD_WO_CACHE] + minSsdCnt))
            {
                break;
            }
        }

        if ((paaIdx & FIO_ERR_PAA_BMP_MASK) == 0 && FIO_GET_ERR_BMP(pErrJob->fid, paaIdx) == 0)
        {
            paaIdx += FIO_ERR_PAA_BMP_BITS_COUNT;
            continue;
        }

        if (FIO_CHK_ERR_BMP(pErrJob->fid, paaIdx))
        {
            FIO_CLR_ERR_BMP(pErrJob->fid, paaIdx);

            //-----------------------------------------------------------------------------
            if (pRRJob->css == CACHE_R_IDS)
            {
                gFastPaaBuf[pRRJob->paaCnt].all32 = 0;
                gFastPaaBuf[pRRJob->paaCnt].rr.srcIdx  = paaIdx;
                gFastPaaBuf[pRRJob->paaCnt].rr.voltage = pRRJob->rrCount;
                gFastPaaBuf[pRRJob->paaCnt + 1]        = pErrJob->paa[paaIdx];

                pRRJob->paaCnt += RR_PAA_UNIT;
            }
            else
            {
                if ((pRRJob->css != CACHE_R_FILLUP_HIT && pRRJob->css != CACHE_FILLUP) ||
                    prevPaa.all32 != pErrJob->paa[paaIdx].all32)
                {
                    tailSsdIdx = ReadRetry_BuildSSD(pRRJob, paaIdx, currSsdIdx);
                    currSsdIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(tailSsdIdx);
                }

                prevPaa = pErrJob->paa[paaIdx];
            }
        }

        paaIdx++;
    }

    gRRInfo.scanPaaCurrIdx[pRRJob->fid - FIO_MAX_JOB] = paaIdx;
    pRRJob->rrState = RR_STATE_RELEASE;

    if (pRRJob->paaCnt)
    {
        if (pRRJob->ssdCnt[SSD_WO_CACHE])
        {
            Cache_PopSSD_Other(SSD_POOL_RR, smCacheMgr.head[SSD_POOL_RR], GET_SSD_PTR_FROM_INDEX(tailSsdIdx),  pRRJob->ssdCnt[SSD_WO_CACHE]);
        }

        MEM_COPY(pRRJob->paa, gFastPaaBuf, sizeof(tPAA) * pRRJob->paaCnt);
        FIO_PushJob(FIO_JOB_POOL_WAIT, pRRJob);
        pRRJob->rrCount++;
    }
    else
    {
        // Something wrong with ErroJob, didn't find any error bitmap for first RRJob creation
        if (gRRInfo.scanPaaStartIdx[pRRJob->fid - FIO_MAX_JOB] == 0)
        {
            D_PRINTF("RR F[%3d.%3d] CSS:%d\n", pRRJob->fid, pErrJob->fid, pErrJob->css);
            ASSERT(0);
        }

        return RR_RET_CONTINUE;
    }

    return RR_RET_ESCAPE;
}

//-----------------------------------------------------------------------------
/**
    Issue read retry job

    @param[in]  RRJob

    @return RRRetStatus_t
**/
//-----------------------------------------------------------------------------
static RRRetStatus_t ReadRetry_IssueJob(RRJob_t* pRRJob)
{
    uint32_t idx;

    //-----------------------------------------------------------------------------
    if (RR_GET_ERR_CNT(pRRJob->fid) != (pRRJob->paaCnt / RR_PAA_UNIT))
    {
        // force re-create RR job, don't need to update scanPaaStartIdx
        ReadRetry_RestoreErrMap(pRRJob, gRRInfo.RRJob2FioJob[pRRJob->fid - FIO_MAX_JOB]);
        ReadRetry_ReleaseSSD(pRRJob);
        pRRJob->rrState = RR_STATE_CREATE;
        pRRJob->rrCount = 0;    // not a best solution

        return RR_RET_CONTINUE;
    }
    else if ((pRRJob->status & FIO_S_BLANK_ERR) == FIO_S_BLANK_ERR)
    {
        // TODO:: redirect paa to zero paa if blank error
    }

    //-----------------------------------------------------------------------------
    pRRJob->status      = 0;
    pRRJob->descCnt     = 0;
    pRRJob->trkAllocCnt = 0;
    pRRJob->trkFreeCnt  = 0;

    for (idx = 0; idx < pRRJob->paaCnt / RR_PAA_UNIT; idx++)
    {
        if (RR_CHK_ERR_BMP(pRRJob->fid, idx))
        {
            RR_CLR_ERR_BMP(pRRJob->fid, idx);
            pRRJob->paa[idx * RR_PAA_UNIT].rr.voltage = pRRJob->rrCount;
        }
    }

    FIO_PushJob(FIO_JOB_POOL_WAIT, pRRJob);
    pRRJob->rrCount++;
    pRRJob->rrState = RR_STATE_RELEASE;

    return RR_RET_ESCAPE;
}

//-----------------------------------------------------------------------------
/**
    Release read retry job

    @param[in]  RRJob

    @return RRRetStatus_t
**/
//-----------------------------------------------------------------------------
static RRRetStatus_t ReadRetry_ReleaseJob(RRJob_t* pRRJob)
{
    tFIO_JOB* pErrJob;

    pErrJob = gRRInfo.RRJob2FioJob[pRRJob->fid - FIO_MAX_JOB];

    if (pRRJob->status & FIO_ERR_RETRY_MASK)
    {
        uint16_t rrMaxCount = RR_MAX_RETRY_COUNT;

        if (pRRJob->flag & FIO_F_SLC)   rrMaxCount = RR_MAX_RETRY_COUNT_SLC;

        if (pRRJob->rrCount < rrMaxCount)
        {
            pRRJob->rrState = RR_STATE_ISSUE;
            return RR_RET_CONTINUE;
        }
        else
        {
            ReadRetry_RestoreErrMap(pRRJob, pErrJob);
            ReadRetry_ReleaseSSD(pRRJob);

            if (pRRJob->css == CACHE_R_IDS)
            {
                ReadRetry_IDSAbortHandler(pErrJob);
            }
            else if (gRRInfo.scanPaaCurrIdx[pRRJob->fid - FIO_MAX_JOB] < pErrJob->paaCnt)
            {
                if (pRRJob->rrCount > pErrJob->rrCount) pErrJob->rrCount = pRRJob->rrCount;

                gRRInfo.scanPaaStartIdx[pRRJob->fid - FIO_MAX_JOB] = gRRInfo.scanPaaCurrIdx[pRRJob->fid - FIO_MAX_JOB];
                pRRJob->rrState = RR_STATE_CREATE;
                pRRJob->rrCount = 0;

                return RR_RET_CONTINUE;
            }
        }
    }
    else
    {
        ReadRetry_ReleaseSSD(pRRJob);

        if (gRRInfo.scanPaaCurrIdx[pRRJob->fid - FIO_MAX_JOB] < pErrJob->paaCnt)
        {
            if (pRRJob->rrCount > pErrJob->rrCount) pErrJob->rrCount = pRRJob->rrCount;

            gRRInfo.scanPaaStartIdx[pRRJob->fid - FIO_MAX_JOB] = gRRInfo.scanPaaCurrIdx[pRRJob->fid - FIO_MAX_JOB];
            pRRJob->rrState = RR_STATE_CREATE;
            pRRJob->rrCount = 0;

            return RR_RET_CONTINUE;
        }
    }

    // need to record max RRC since RR job will be re-created
    if (pRRJob->rrCount > pErrJob->rrCount) pErrJob->rrCount = pRRJob->rrCount;

    if (pRRJob->status & FIO_ERR_RETRY_MASK)
    {
        RR_REC_FAIL_CNT();
        D_PRINTF("RR F[%3d.%3d] \033[7;31mFAIL\033[m CSS%2d C%d\n", pRRJob->fid, pErrJob->fid, pErrJob->css, pRRJob->rrCount);
    }
    else
    {
        pErrJob->status &= ~FIO_ERR_RETRY_MASK;
        RR_REC_PASS_CNT(pErrJob->rrCount);

        if (pRRJob->rrCount > 1)
        {
            D_PRINTF("RR F[%3d.%3d] \033[7;32mPASS\033[m CSS%2d C%d\n", pRRJob->fid, pErrJob->fid, pErrJob->css, pRRJob->rrCount);
        }
    }

    AplReadRetry_PushJob(RR_JOB_POOL_FREE, pRRJob);

    BEGIN_CS1
    IPC_SendMsgQ(cM2cComQueue, (Msg_t*)pErrJob);
    END_CS1

    return RR_RET_ESCAPE;
}

//-----------------------------------------------------------------------------
/**
    Retry build SSD

    @param[in]  RRJob
    @param[in]  error paa index
    @param[in]  current other index

    @return current read retry other index
**/
//-----------------------------------------------------------------------------
static uint16_t ReadRetry_BuildSSD(RRJob_t* pRRJob, uint16_t errPaaIdx, uint16_t otherIdx)
{
    tFIO_JOB* pErrJob;
    uint16_t cacheIdx = 0;
    uint16_t count = 0;

    pErrJob = gRRInfo.RRJob2FioJob[pRRJob->fid - FIO_MAX_JOB];

    switch (pRRJob->css)
    {
        case CACHE_R_NO_HIT:
        case CACHE_GCREAD:
        case CACHE_GCREADAUX:
        case CACHE_PREREAD:
            cacheIdx = pErrJob->ssdIdx[SSD_W_CACHE];
            while (count < pErrJob->ssdCnt[SSD_W_CACHE])
            {
                if (count == errPaaIdx)
                {
                    smSSD[otherIdx].prevIndex = cacheIdx;
                    smSSD[otherIdx].startAddr = smSSD[cacheIdx].startAddr;
                    smSSD[otherIdx].endAddr   = smSSD[cacheIdx].endAddr;
                    break;
                }

                cacheIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(cacheIdx);
                count++;
            }

            gFastPaaBuf[pRRJob->paaCnt].all32 = 0;
            gFastPaaBuf[pRRJob->paaCnt].rr.srcIdx = errPaaIdx;
            gFastPaaBuf[pRRJob->paaCnt + 1]       = pErrJob->paa[errPaaIdx];

            pRRJob->ssdCnt[SSD_WO_CACHE]++;
            pRRJob->paaCnt += RR_PAA_UNIT;

            break;
        case CACHE_R_PARTIAL_HIT:
    #if (_NAND_ == NAND_MLC)
        case CACHE_GCLASTPAGE:
        case CACHE_LASTPAGE:
        case CACHE_SCANPAGE:
    #endif
            cacheIdx = pErrJob->ssdIdx[SSD_WO_CACHE];
            while (count < pErrJob->ssdCnt[SSD_WO_CACHE])
            {
                if (count == errPaaIdx)
                {
                    if (pErrJob->css == CACHE_R_PARTIAL_HIT)
                        cacheIdx = smSSD[cacheIdx].prevIndex;

                    smSSD[otherIdx].prevIndex = cacheIdx;
                    smSSD[otherIdx].startAddr = smSSD[cacheIdx].startAddr;
                    smSSD[otherIdx].endAddr   = smSSD[cacheIdx].endAddr;
                    break;
                }

                cacheIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(cacheIdx);
                count++;
            }

            gFastPaaBuf[pRRJob->paaCnt].all32 = 0;
            gFastPaaBuf[pRRJob->paaCnt].rr.srcIdx = errPaaIdx;
            gFastPaaBuf[pRRJob->paaCnt + 1]       = pErrJob->paa[errPaaIdx];

            pRRJob->ssdCnt[SSD_WO_CACHE]++;
            pRRJob->paaCnt += RR_PAA_UNIT;

            break;
        case CACHE_R_FILLUP_HIT:
        case CACHE_FILLUP:
        {
            U32 currDataSize = 0;
            U16 paaPerFillup = 0;
            U16 idx;

            cacheIdx = pErrJob->ssdIdx[SSD_WO_CACHE];

            while (currDataSize < errPaaIdx * MBU_SIZE)
            {
                currDataSize += ((smSSD[cacheIdx].endAddr + 1 - smSSD[cacheIdx].startAddr) << SSD_ADDR_SHIFT);
                cacheIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(cacheIdx);
            }

            while (smSSD[cacheIdx].prevIndex == SSD_DUMMY_START)
            {
                cacheIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(cacheIdx);
            }

            otherIdx = Cache_Alloc_FillupSSD(pRRJob, smSSD[cacheIdx].prevIndex, otherIdx, &paaPerFillup);

            #ifdef LIMIT_NUM_OF_SSD_PER_AU_WORKAROUND
            for (idx = 0; idx < paaPerFillup; idx++)
            {
                gFastPaaBuf[pRRJob->paaCnt].all32 = 0;
                gFastPaaBuf[pRRJob->paaCnt].rr.srcIdx = errPaaIdx;
                gFastPaaBuf[pRRJob->paaCnt + 1]       = pErrJob->paa[errPaaIdx];

                pRRJob->paaCnt += RR_PAA_UNIT;
            }
            #endif

            break;
        }
    #if (_NAND_ == NAND_3DTLCG2)
        case CACHE_GCLASTPAGE:
        case CACHE_LASTPAGE:
        case CACHE_SCANPAGE:
    #endif
        case CACHE_SYSINFO:
        case CACHE_AUTO:
            cacheIdx = pErrJob->ssdIdx[SSD_WO_CACHE];

            smSSD[otherIdx].prevIndex = cacheIdx;
            smSSD[otherIdx].startAddr = smSSD[cacheIdx].startAddr + errPaaIdx * (MBU_SIZE >> SSD_ADDR_SHIFT);
            smSSD[otherIdx].endAddr   = smSSD[otherIdx].startAddr + (MBU_SIZE >> SSD_ADDR_SHIFT) - 1;

            gFastPaaBuf[pRRJob->paaCnt].all32 = 0;
            gFastPaaBuf[pRRJob->paaCnt].rr.srcIdx = errPaaIdx;
            gFastPaaBuf[pRRJob->paaCnt + 1]       = pErrJob->paa[errPaaIdx];

            pRRJob->ssdCnt[SSD_WO_CACHE]++;
            pRRJob->paaCnt += RR_PAA_UNIT;

            // address overflow, please review other ssd usage
            if (smSSD[otherIdx].endAddr > smSSD[cacheIdx].endAddr)
            {
                D_PRINTF("[RR] ERR Addr, CSS:%d \n", pRRJob->css);
                //smSSD[otherIdx].startAddr = smSSD[cacheIdx].startAddr;
                //smSSD[otherIdx].endAddr   = smSSD[cacheIdx].endAddr;
                ASSERT(0);
            }

            break;
        case CACHE_TCG:
        default:
            D_PRINTF("CSS:%d\n", pRRJob->css);
            ASSERT(0);
            break;
    }

    return otherIdx;
}

//-----------------------------------------------------------------------------
/**
    Release read retry SSD

    @param[in]  RRJob
**/
//-----------------------------------------------------------------------------
static __inline void ReadRetry_ReleaseSSD(RRJob_t* pRRJob)
{
    if (pRRJob->ssdCnt[SSD_WO_CACHE])
    {
        tSSD *pHead;
        tSSD *pTail;
        U32  cacheIdx;
        U16  count;

        cacheIdx = pRRJob->ssdIdx[SSD_WO_CACHE];
        pHead    = GET_SSD_PTR_FROM_INDEX(cacheIdx);
        count    = 0;

        while (count < pRRJob->ssdCnt[SSD_WO_CACHE])
        {
            pTail    = &smSSD[cacheIdx];
            cacheIdx = pTail->nextIndex;
            count++;
        }

        Cache_PushSSD_Other(SSD_POOL_RR, pHead, pTail, pRRJob->ssdCnt[SSD_WO_CACHE]);
    }
}

//-----------------------------------------------------------------------------
/**
    Restore error job's error map from read retry error map

    @param[in]  RRJob
    @param[in]  ErrJob
**/
//-----------------------------------------------------------------------------
static __inline void ReadRetry_RestoreErrMap(RRJob_t* pRRJob, tFIO_JOB* pErrJob)
{
    uint16_t idx;

    for (idx = 0; idx < pRRJob->paaCnt / RR_PAA_UNIT; idx++)
    {
        if (RR_CHK_ERR_BMP(pRRJob->fid, idx))
        {
            FIO_SET_ERR_BMP(pErrJob->fid, pRRJob->paa[idx * RR_PAA_UNIT].rr.srcIdx);
        }
    }
}

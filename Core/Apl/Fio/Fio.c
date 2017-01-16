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
//! @brief Inter Processor Communication code
//!
//=============================================================================
//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "Cache.h"
#if (_CPU_ == CPU_ID1)
    #include "AplSeq.h"
    #include "AplSch.h"
#elif (_CPU_ == CPU_ID2)
    #include "Core.h"
    #include "Ftl.h"
#endif

#include "Fio.h"

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
#if _CPU_==CPU_ID2
    FioJobTpl_t gTplFioInfo;
#endif

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
#if _CPU_==CPU_ID2
//-----------------------------------------------------------------------------
/**
    Initialization of flash I/O driver

    @param[in]  initMode

    @return cEcNoError
**/
//-----------------------------------------------------------------------------
Error_t FIO_OneTimeInit(InitBootMode_t initMode)
{
    tPAA* pPaaBuf;
    U32* pAuxBuf;
    U16 idx;

    pPaaBuf = MEM_AllocBuffer(sizeof(tPAA) * (FIO_MAX_JOB * MAX_PAA_PER_JOB), 512);
    pAuxBuf = MEM_AllocBuffer(sizeof(U32) *  (FIO_MAX_JOB * MAX_AUX_PER_JOB), 512);

    MEM_CLR(smFioJobBuf, sizeof(tFIO_JOB) * FIO_MAX_JOB);
    MEM_CLR(&smFioInfo, sizeof(FioInfo_t));

    smFioInfo.pDramPaa = pPaaBuf;
    smFioInfo.pDramAux = pAuxBuf;

    for (idx = 0; idx < FIO_MAX_JOB; idx++)
    {
        smFioJobBuf[idx].fid = idx;

        if (idx < FIO_MAX_JOB)
        {
            if (idx == 0)
            {
                smFioJobBuf[idx].prev = NULL;
                smFioJobBuf[idx].next = &smFioJobBuf[idx + 1];
            }
            else if (idx == FIO_MAX_JOB - 1)
            {
                smFioJobBuf[idx].prev = &smFioJobBuf[idx - 1];
                smFioJobBuf[idx].next = NULL;
            }
            else
            {
                smFioJobBuf[idx].prev = &smFioJobBuf[idx - 1];
                smFioJobBuf[idx].next = &smFioJobBuf[idx + 1];
            }
        }

        smFioJobBuf[idx].paa = &pPaaBuf[idx * MAX_PAA_PER_JOB];
        smFioJobBuf[idx].aux = &pAuxBuf[idx * MAX_AUX_PER_JOB];
    }

    smFioInfo.head[FIO_JOB_POOL_FREE]   = &smFioJobBuf[0];
    smFioInfo.tail[FIO_JOB_POOL_FREE]   = &smFioJobBuf[FIO_MAX_JOB - 1];
    smFioInfo.jobCnt[FIO_JOB_POOL_FREE] = FIO_MAX_JOB;

    MEM_CLR(&gTplFioInfo, sizeof(FioJobTpl_t));
    gTplFioInfo.pMsgHost            = NULL;
    gTplFioInfo.ssdIdx[SSD_W_CACHE] = SSD_NULL_START;
    gTplFioInfo.ssdIdx[SSD_WO_CACHE]= SSD_NULL_START;
    gTplFioInfo.css                 = CACHE_AUTO;
    gTplFioInfo.cmd                 = FIO_JOB_CMD_NULL;
    gTplFioInfo.auFmt               = gEcuInfo.auFormat;
    gTplFioInfo.ssdList             = SSD_W_CACHE;
    gTplFioInfo.plane               = gNfInfo.planesPerLun;
    gTplFioInfo.header              = FTL_BLOCK_HEADER_MAX;
    gTplFioInfo.segNum              = NUM_OF_FS_SEG;

    return cEcNoError;
}
#endif

//-----------------------------------------------------------------------------
/**
    Freeze fio jobs when PLP happened
**/
//-----------------------------------------------------------------------------
void FIO_PLPFreezeJobs(void)
{
    tFIO_JOB* pJob;
    U32 idx;
    U32 oriCnt;
    U16 pool;

    for (pool = (FIO_JOB_POOL_READ + FIO_R_POOL_CNT_NORMAL); pool < FIO_JOB_POOL_WRITE; pool++)
    {
        oriCnt = smFioInfo.jobCnt[pool];
        for (idx = 0; idx < oriCnt; idx++)
        {
            pJob = FIO_PopJob((FioJobPool_t)pool);

            if (pJob->fid >= FIO_MAX_JOB || pJob->css == CACHE_FILLUP || pJob->css == CACHE_R_FILLUP_HIT)
            {
                FIO_PushJob(FIO_JOB_POOL_WAIT, pJob);
            }
            else
            {
                FIO_PushJob(FIO_JOB_POOL_FREEZE, pJob);
            }
        }
    }

    for (pool = FIO_JOB_POOL_WRITE; pool <= FIO_JOB_POOL_ERASE; pool++)
    {
        oriCnt = smFioInfo.jobCnt[pool];
        for (idx = 0; idx < oriCnt; idx++)
        {
            pJob = FIO_PopJob((FioJobPool_t)pool);

            if (pJob->css == CACHE_LASTPAGE)
            {
                FIO_PushJob(FIO_JOB_POOL_FREEZE, pJob);
            }
            else
            {
                FIO_PushJob(FIO_JOB_POOL_WAIT, pJob);
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Unfreeze fio jobs while fake PLP happened
**/
//-----------------------------------------------------------------------------
void FIO_PLPUnfreezeJobs(void)
{
    tFIO_JOB* pJob;

    while (smFioInfo.jobCnt[FIO_JOB_POOL_FREEZE])
    {
        pJob = FIO_PopJob(FIO_JOB_POOL_FREEZE);
        FIO_PushJob(FIO_JOB_POOL_WAIT, pJob);
    }

    ASSERT(smFioInfo.jobCnt[FIO_JOB_POOL_FREEZE] == 0);
}

#include "NormSection.h"   // CODE AND DATA MEMORY MAPPING
#if _CPU_==CPU_ID1
//-----------------------------------------------------------------------------
/**
    dump flash I/O driver info

    @param[in]  level
**/
//-----------------------------------------------------------------------------
void FIO_DumpInfo(U32 level)
{
    tFIO_JOB* pJob;
    U16 error = 0;
    U16 pool;
    U16 count;
    U16 cid;

    #define FIO_ERR_COUNT_MISMATCH    B_0
    #define FIO_ERR_PUHS_JOB_ERROR    B_1
    #define FIO_ERR_JOB_BUSY          B_2

    #define FIO_DUMP_ALL              0xA

    M_PRINTF("\n[FIO ]\n");

    for (pool = 0; pool < FIO_JOB_POOL_COUNT; pool++)
    {
        if (smFioInfo.jobCnt[pool])
        {
            count = 0;
            pJob  = smFioInfo.head[pool];

            while (pJob && count < smFioInfo.jobCnt[pool])
            {
                count++;
                if (pool == FIO_JOB_POOL_FREE)
                {
                    if (pJob->fid >= FIO_MAX_JOB)
                    {
                        error |= FIO_ERR_PUHS_JOB_ERROR;
                        break;
                    }
                }

                if (pJob->trkAllocCnt != pJob->trkFreeCnt)
                {
                    error |= FIO_ERR_JOB_BUSY;
                }

                if (level && ((pool > FIO_JOB_POOL_FREE) || pJob->status & FIO_S_ERR_MASK))
                {
                    if (pJob->css == CACHE_R_IDS)   cid = pJob->pMsgHost->attrInfo.b.cmdId;
                    else                            cid = WORD_MASK;

                    M_PRINTF("== %3d: F/T/C[%3d.%3d.%5d] CSS%2d Cmd%2d P/D/E[%3d.%3d.%3d] S[%04X] T[%08X] TA/F[%2d.%2d]\n",
                                    count, pJob->fid, pJob->tid, cid, pJob->css, pJob->cmd,
                                    pJob->paaCnt, pJob->descCnt, smFioInfo.errPaaCnt[pJob->fid], pJob->status, pJob->timeStamp,
                                    pJob->trkAllocCnt, pJob->trkFreeCnt);
                }

                pJob = pJob->next;
            }

            if (count != smFioInfo.jobCnt[pool]) error |= FIO_ERR_COUNT_MISMATCH;

            if (error)  M_PRINTF("P[%2d]:%2d  ERR:%04X\n", pool, smFioInfo.jobCnt[pool], error);
            else        M_PRINTF("P[%2d]:%2d  OK\n", pool, smFioInfo.jobCnt[pool]);
        }
    }

    M_PRINTF("JIC[%10d] JDC[%10d] *[%10d]\n", smFioInfo.jobIssueCnt, smFioInfo.jobDoneCnt, smFioInfo.jobIssueCnt - smFioInfo.jobDoneCnt);
    M_PRINTF("TIC[%10d] TDC[%10d] *[%10d]\n", gAplSeqInfo.trkIssueCnt, gAplSeqInfo.trkDoneCnt, gAplSeqInfo.trkIssueCnt - gAplSeqInfo.trkDoneCnt);

    if (level == FIO_DUMP_ALL)
    {
        CacheEntry_t* pCacheEntry;
        U32 hostLaa;
        U16 cnt, cacheIdx, otherIdx;

        for (count = 0; count < FIO_MAX_JOB; count++)
        {
            pJob = &smFioJobBuf[count];

            if ((pJob->trkAllocCnt != pJob->trkFreeCnt) || (pJob->status & FIO_S_ERR_MASK))
            {
                if (pJob->css == CACHE_R_IDS)   cid = pJob->pMsgHost->attrInfo.b.cmdId;
                else                            cid = WORD_MASK;

                M_PRINTF("== %3d: F/T/C[%3d.%3d.%5d] CSS%2d Cmd%2d P/D/E[%3d.%3d.%3d] S[%04X] T[%08X] TA/F[%2d.%2d]\n",
                            count, pJob->fid, pJob->tid, cid, pJob->css, pJob->cmd,
                            pJob->paaCnt, pJob->descCnt, smFioInfo.errPaaCnt[pJob->fid], pJob->status, pJob->timeStamp,
                            pJob->trkAllocCnt, pJob->trkFreeCnt);

                if (pJob->css == CACHE_R_IDS)
                {
                    for (cnt = 0; cnt < pJob->paaCnt; cnt++)
                    {
                        hostLaa = pJob->pMsgHost->laa + cnt;

                        M_PRINTF("  %3d: HL[%08X] [%d.%d.%d.%d.%4X.%3X]\n",
                                    cnt, hostLaa,
                                    pJob->paa[cnt].b.ch,
                                    pJob->paa[cnt].b.ce,
                                    pJob->paa[cnt].b.lun,
                                    pJob->paa[cnt].b.plane,
                                    pJob->paa[cnt].b.block,
                                    pJob->paa[cnt].b.page);
                    }
                }

                cacheIdx = pJob->ssdIdx[SSD_W_CACHE];
                for (cnt = 0; cnt < pJob->ssdCnt[SSD_W_CACHE]; cnt++)
                {
                    pCacheEntry = &smCacheMgr.cacheEntry[cacheIdx];

                    if (pJob->pMsgHost) hostLaa = pJob->pMsgHost->laa + cnt;
                    else                hostLaa = DWORD_MASK;

                    M_PRINTF("  %3d: HL[%08X]CL[%08X]CI[%4d] [%d.%d.%d.%d.%4X.%3X] B[%02X] S[%04X]\n",
                                cnt, hostLaa, pCacheEntry->laa, cacheIdx,
                                pJob->paa[cnt].b.ch,
                                pJob->paa[cnt].b.ce,
                                pJob->paa[cnt].b.lun,
                                pJob->paa[cnt].b.plane,
                                pJob->paa[cnt].b.block,
                                pJob->paa[cnt].b.page,
                                pCacheEntry->bitmap, pCacheEntry->status);

                    cacheIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(cacheIdx);
                }
                M_PRINTF("---\n");

                otherIdx = pJob->ssdIdx[SSD_WO_CACHE];
                for (cnt = 0; cnt < pJob->ssdCnt[SSD_WO_CACHE]; cnt++)
                {
                    M_PRINTF("  %3d: O/P Idx[%4d.%4d]\n", cnt, otherIdx, smSSD[otherIdx].prevIndex);
                    otherIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(cacheIdx);
                }

                if (pJob->css == CACHE_AUTO)
                {
                    for (cnt = 0; cnt < pJob->paaCnt; cnt++)
                    {
                        M_PRINTF("%3d: P[%08X] ", cnt, pJob->paa[cnt].all32);
                    }
                    M_PRINTF("\n");
                }
            }
        }
    }
}
#endif

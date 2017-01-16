//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "Core.h"
#include "Gc.h"
#include "Gc_3DTLCG2.h"

#include "PerfSection.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#if (_NAND_ == NAND_3DTLCG2 && ENABLE_GC == TRUE)

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-------------------------------------------------------------------
// Function     : void gc_setPaaLP(U8 p2lIdx, U8 lun, U8 ce)
// Description  : For 3DTLC
// Input        : U8 p2lIdx, U8 lun, U8 ce
// return       : N/A
//-------------------------------------------------------------------
void gc_setPaaLP(U8 p2lIdx, U8 lun, U8 ce)
{
    U8 ch, plane, frag;
    U8 paaCnt = 0;

    for (ch = 0; ch < GC_NF_NUM_CHANNELS; ch++)
    {
        for (plane = 0; plane < GC_NF_PLANE_PER_LUN; plane++)
        {
            for (frag = 0; frag < GC_ECU_AU_PER_PAGE; frag++)
            {
                gFastPaaBuf[paaCnt].b.ce    = ce;
                gFastPaaBuf[paaCnt].b.lun   = lun;
                gFastPaaBuf[paaCnt].b.ch    = ch;
                gFastPaaBuf[paaCnt].b.plane = plane;
                gFastPaaBuf[paaCnt].b.frag  = frag;
                gFastPaaBuf[paaCnt].b.page  = GC_LP_PAGE_IDX;
                gFastPaaBuf[paaCnt].b.block = gGCppBuf[p2lIdx].blk_Info.blk;
                gFastPaaBuf[paaCnt].b.block = GC_GetDtcmRmp(gFastPaaBuf[paaCnt], p2lIdx);
                paaCnt++;
            }
        }
    }
}

//-------------------------------------------------------------------
// Function     : void gc_issueJobLP(U8 curP2lIdx)
// Description  : For 3DTLC
// Input        : U8 curP2lIdx
// return       : N/A
//-------------------------------------------------------------------
void gc_issueJobLP(U8 curP2lIdx)
{
    tSSD     *pHead;
    tSSD     *pTail;
    tFIO_JOB *pJob;
    U32       idx;
    U32       cacheIdx;
    U8        die, lun, ce;

    // ----Pop & Issue Job
    pHead    = smCacheMgr.head[SSD_POOL_OTHER];
    cacheIdx = GET_SSD_INDEX_FROM_PTR(pHead);

    pJob                       = FIO_PopJob(FIO_JOB_POOL_FREE);
    pJob->ssdList              = SSD_WO_CACHE;
    pJob->ssdIdx[SSD_WO_CACHE] = cacheIdx;
    pJob->ssdCnt[SSD_WO_CACHE] = GC_REQ_SSD_COUNT_LP; // Only need one ssd for other cache case
    pJob->paaCnt               = GC_REQ_CACHE_ENTRY_LP;
    pJob->cmd                  = FIO_JOB_CMD_READ;
    pJob->css                  = CACHE_GCLASTPAGE;
    pJob->plane                = GC_NF_PLANE_PER_LUN;
    pJob->flag                 = FIO_F_MUTE;

    // Seeking for un-issued Die
    ce  = GET_CE_FROM_IDX(gGCppBuf[curP2lIdx].lp_Info.issueCnt);
    lun = GET_LN_FROM_IDX(gGCppBuf[curP2lIdx].lp_Info.issueCnt);

    // Fill Last Page PAA
    gc_setPaaLP(curP2lIdx, lun, ce);
    MemCopyNU32((U32*)pJob->paa, (U32*)gFastPaaBuf, sizeof(tPAA) * pJob->paaCnt);

    // P2L Table
    die = FTL_GET_DIE_NUM(lun, ce, 0);
    idx = FTL_GET_LP_IDX(0, curP2lIdx, die, 0, 0, 0);

    pTail            = GET_SSD_PTR_FROM_INDEX(cacheIdx);
    pTail->startAddr = (U32)&gdwGCLastPageBuf[idx];                                   // P2L Table Start Address
    pTail->endAddr   = (pTail->startAddr + (GC_FTL_LP_SIZE >> FTL_BANK_SHIFT) - 1);   // P2L Table End Address - 1;

    Cache_PopSSD_Other(SSD_POOL_OTHER, pHead, pTail, pJob->ssdCnt[SSD_WO_CACHE]);

    /*if (((GC_FTL_LP_SIZE >> FTL_BANK_SHIFT) / gEcuInfo.auSize) < pJob->paaCnt) // Avoid dummy LP
    {
        D_PRINTF_GC("Read LP dummy[%x]\n", ((GC_FTL_LP_SIZE >> FTL_BANK_SHIFT) / gEcuInfo.auSize));
        pTail->nextIndex = SSD_DUMMY_START;
    }*/

    Core_JobIssue(pJob);
}

//-------------------------------------------------------------------
// Function     : U8 gc_setPaa(U8 curP2lIdx)
// Description  : For 3DTLC
// Input        : U8 curP2lIdx
// return       : N/A
//-------------------------------------------------------------------
U8 gc_setPaa(U8 curP2lIdx)
{
    U16 scanPaaCnt = 0;
    U16 phyBlk;
    U8  ch, pl, frag;
    U8  gcDone = FALSE;

    gGCInfo.jobPaaCnt = 0;

    while ((gGCInfo.jobPaaCnt == 0) && (gcDone == FALSE))
    {
        for (ch = 0; ch < GC_NF_NUM_CHANNELS; ch++)
        {
            for (pl = 0; pl < GC_NF_PLANE_PER_LUN; pl++)
            {
                for (frag = 0; frag < GC_XFER_ORDER; frag++)
                {
                    gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.ch    = ch;
                    gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.plane = pl;
                    gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.frag  = frag;

                    phyBlk = GC_GetDtcmRmp(gGCppBuf[curP2lIdx].recyc_Info.curPaa, curP2lIdx);

                    gFastPaaBuf[gGCInfo.jobPaaCnt]         = gGCppBuf[curP2lIdx].recyc_Info.curPaa;
                    gFastPaaBuf[gGCInfo.jobPaaCnt].b.block = phyBlk;

                    if (gGCppBuf[curP2lIdx].mode_Info.type == GC_LP)    // LP type
                    {
                        gFastLaaBuf[gGCInfo.jobPaaCnt] = gc_checkLAA(gGCppBuf[curP2lIdx].recyc_Info.curPaa, curP2lIdx, phyBlk);
                    }

                    if ((gFastLaaBuf[gGCInfo.jobPaaCnt] != DWORD_MASK) || (gGCppBuf[curP2lIdx].mode_Info.type == GC_AUX))
                    {
                        gGCInfo.jobPaaCnt++;
                    }
                }
            }
        }

        scanPaaCnt += GC_ONE_SHOT_PAA_CNT;

        gcDone = GC_Get_NxtDie(curP2lIdx);

        if ((gGCInfo.jobPaaCnt == 0) && (scanPaaCnt >= GC_SCAN_PAA_CNT_TH))
        {
            if ((gGCppBuf[curP2lIdx].mode_Info.mode == GC_ST_FORCE) || (gGCppBuf[curP2lIdx].mode_Info.mode == GC_ST_STATIC_WL))
            {
                if (gGCppBuf[curP2lIdx].qos_Info.hostCacheCnt >= gGCppBuf[curP2lIdx].qos_Info.hostCacheTH)
                {
                    if (gGCppBuf[curP2lIdx].qos_Info.hostCacheCnt > GC_SCAN_PAA_CNT_TH)
                       gGCppBuf[curP2lIdx].qos_Info.hostCacheCnt -= GC_SCAN_PAA_CNT_TH;
                    else
                       gGCppBuf[curP2lIdx].qos_Info.hostCacheCnt = 0;
                }
            }
            return(gcDone);
        }
    }

    return (gcDone);
}

//-------------------------------------------------------------------
// Function     : U8 gc_issueJob(U8 curP2lIdx)
// Description  : For 3DTLC
// Input        : U8 curP2lIdx
// return       : U8
//-------------------------------------------------------------------
U8 gc_issueJob(U8 curP2lIdx)
{
    tSSD     *pHead  = NULL;
    tSSD     *pTail  = NULL;
    tFIO_JOB *pJob   = NULL;
    U32       cacheIdx;
    U16       idx;
    U8        gcDone;

    // ----Valid bit map check
    gcDone = gc_setPaa(curP2lIdx);

    // ----Pop & Issue Job
    if (gGCInfo.jobPaaCnt > 0)
    {
        gGCppBuf[curP2lIdx].qos_Info.gcCacheCnt    += gGCInfo.jobPaaCnt;
        gGCppBuf[curP2lIdx].recyc_Info.recycPaaCnt += gGCInfo.jobPaaCnt;

        pHead    = smCacheMgr.head[SSD_POOL_FREE];
        cacheIdx = GET_SSD_INDEX_FROM_PTR(pHead);

        pJob                      = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob->ssdIdx[SSD_W_CACHE] = cacheIdx;
        pJob->ssdCnt[SSD_W_CACHE] = gGCInfo.jobPaaCnt;
        pJob->paaCnt              = gGCInfo.jobPaaCnt;
        pJob->cmd                 = FIO_JOB_CMD_READ;
        pJob->css                 = CACHE_GCREAD;
        pJob->plane               = GC_NF_PLANE_PER_LUN;

        MemCopyNU32((U32*)pJob->paa, (U32*)gFastPaaBuf, sizeof(tPAA) * pJob->paaCnt);

        for (idx = 0; idx < pJob->paaCnt; idx++)
        {
            if (smCacheMgr.cacheEntry[cacheIdx].laa != DWORD_MASK)
            {
                Cache_Delete(cacheIdx);
                smCacheMgr.cacheEntry[cacheIdx].laa = DWORD_MASK;
            }
            smCacheMgr.cacheEntry[cacheIdx].status = CACHE_S_SEQ_READ;
            smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_FREE;
            smCacheMgr.cacheEntry[cacheIdx].laa    = (gFastLaaBuf[idx]);
            smCacheMgr.cacheEntry[cacheIdx].bitmap = (CACHE_BMP_ALL_VALID);
            Cache_Insert(cacheIdx);

            pTail    = &smSSD[cacheIdx];
            cacheIdx = pTail->nextIndex;
        }

        Cache_PopSSD(SSD_POOL_FREE, pHead, pTail, pJob->ssdCnt[SSD_W_CACHE]);
        Core_JobIssue(pJob);
    }

    return (gcDone);
}

//-------------------------------------------------------------------
// Function     : U8 gc_issueJob_AUX(U8 curP2lIdx)
// Description  : For 3DTLC
// Input        : U8 curP2lIdx
// return       : U8
//-------------------------------------------------------------------
U8 gc_issueJob_Aux(U8 curP2lIdx)
{
    tSSD     *pHead  = NULL;
    tSSD     *pTail  = NULL;
    tFIO_JOB *pJob   = NULL;
    U32       cacheIdx;
    U16       idx;
    U8        gcDone;
    gGCAux.pJobbuf[gGCAux.issuecnt] = NULL;

    // ----Valid bit map check
    gcDone = gc_setPaa(curP2lIdx);

    // ----Pop & Issue Job
    if (gGCInfo.jobPaaCnt > 0)
    {
        gGCppBuf[curP2lIdx].qos_Info.gcCacheCnt    += gGCInfo.jobPaaCnt;
        gGCppBuf[curP2lIdx].recyc_Info.recycPaaCnt += gGCInfo.jobPaaCnt;

        pHead    = smCacheMgr.head[SSD_POOL_FREE];
        cacheIdx = GET_SSD_INDEX_FROM_PTR(pHead);

        gGCAux.pJobbuf[gGCAux.issuecnt]                      = FIO_PopJob(FIO_JOB_POOL_FREE);
        gGCAux.pJobbuf[gGCAux.issuecnt]->ssdIdx[SSD_W_CACHE] = cacheIdx;
        gGCAux.pJobbuf[gGCAux.issuecnt]->ssdCnt[SSD_W_CACHE] = gGCInfo.jobPaaCnt;
        gGCAux.pJobbuf[gGCAux.issuecnt]->paaCnt              = gGCInfo.jobPaaCnt;
        gGCAux.pJobbuf[gGCAux.issuecnt]->cmd                 = FIO_JOB_CMD_READ;
        gGCAux.pJobbuf[gGCAux.issuecnt]->css                 = CACHE_GCREADAUX;
        gGCAux.pJobbuf[gGCAux.issuecnt]->plane               = GC_NF_PLANE_PER_LUN;
        gGCAux.pJobbuf[gGCAux.issuecnt]->flag                = (FIO_F_AUX | FIO_F_MUTE_BLANK_ERR | FIO_F_PAGE_READ);  // If last page mode plz turn it off
        if (GC_MODE_CHECK_BOOT_BLK(curP2lIdx))
        {
            gGCAux.pJobbuf[gGCAux.issuecnt]->flag           |= (FIO_F_MUTE | FIO_F_NO_MKERR | FIO_F_BOOT);
        }
        MemCopyNU32((U32*)gGCAux.pJobbuf[gGCAux.issuecnt]->paa, (U32*)gFastPaaBuf, sizeof(tPAA) * gGCAux.pJobbuf[gGCAux.issuecnt]->paaCnt);

        for (idx = 0; idx < gGCAux.pJobbuf[gGCAux.issuecnt]->paaCnt; idx++)
        {
            if (smCacheMgr.cacheEntry[cacheIdx].laa != DWORD_MASK)
            {
                Cache_Delete(cacheIdx);
                smCacheMgr.cacheEntry[cacheIdx].laa = DWORD_MASK;
            }
            smCacheMgr.cacheEntry[cacheIdx].status = CACHE_S_SEQ_READ;

            pTail    = &smSSD[cacheIdx];
            cacheIdx = pTail->nextIndex;
        }

        Cache_PopSSD(SSD_POOL_FREE, pHead, pTail, gGCAux.pJobbuf[gGCAux.issuecnt]->ssdCnt[SSD_W_CACHE]);
        Core_JobIssue(gGCAux.pJobbuf[gGCAux.issuecnt]);

        gGCAux.issuecnt++;
        if (gGCAux.issuecnt > (GC_MAX_AUX_JOB - 1))
        {
            Core_JobWait();
            for (idx = 0; idx < GC_MAX_AUX_JOB; idx++)
            {
                if (gGCAux.pJobbuf[idx]->status & FIO_S_BLANK_ERR)
                {
                    gcDone = TRUE;
                    break;
                }
            }
            gGCAux.issuecnt = 0;
        }
    }
    return (gcDone);
}

//-------------------------------------------------------------------
// Function     : U8 GC_Get_NxtDie(U8 curP2lIdx)
// Description  : Get NXT WL_Page for GC
// Input        : U8 curP2lIdx
// return       : U8
//-------------------------------------------------------------------
U8 GC_Get_NxtDie(U8 curP2lIdx)
{
    U8 scDone = FALSE;

    gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.frag  = 0;  // Reset plane & frag. don't move this line over to the end of this function
    gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.plane = 0;

    if ((gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.page++ % GC_NF_PAGE_PER_PROG_UNIT) == (GC_NF_PAGE_PER_PROG_UNIT - 1))
    {
        gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.page -= (GC_NF_PAGE_PER_PROG_UNIT);
        if (gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.ce++ == (GC_NF_DEVICE_PER_CHANNEL - 1))
        {
    		gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.ce = 0;
            if (gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.lun++ == (GC_NF_LUN_PER_DEVICE - 1))
            {
    			gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.lun = 0;
                if ((gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.page += GC_NF_PAGE_PER_PROG_UNIT) == (GC_NF_USER_WL_PER_BLK * GC_NF_PAGE_PER_PROG_UNIT))
                {
                    gGCppBuf[curP2lIdx].recyc_Info.curPaa.all32 = DWORD_MASK;
                    scDone = TRUE;
                }
            }
        }
    }

    return (scDone);
}

//-------------------------------------------------------------------
// Function     : void GC_Test_ReadLP(U16 blk)
// Description  :
// Input        : U16 blk
// return       : N/A
//-------------------------------------------------------------------
void GC_Test_ReadLP(U16 blk)
{
#if (DBG_GC_LP == TRUE)

    tSSD     *pHead  = NULL;
    tSSD     *pTail  = NULL;
    tFIO_JOB *pJob   = NULL;
    U32       idx;
    U32       auPerBlock;
    U32       cacheIdx;
    U8        ch, ce, lun, die, plane, frag;
    U8        paaCnt;

    auPerBlock = GC_ECU_AU_PER_PAGE * GC_NF_PLANE_PER_LUN * GC_NF_PAGE_PER_BLK;

    MEM_CLR(gdwGCTestBuf, sizeof(U32) * auPerBlock * GC_NF_TOT_LUN * FTL_GROUP_NUM );

    for (lun = 0; lun < GC_NF_LUN_PER_DEVICE; lun++)
    {
        for (ce = 0; ce < GC_NF_DEVICE_PER_CHANNEL; ce++)
        {
            paaCnt = 0;

            // ----Pop & Issue Job
            pHead    = smCacheMgr.head[SSD_POOL_OTHER];
            cacheIdx = GET_SSD_INDEX_FROM_PTR(pHead);

            pJob                       = FIO_PopJob(FIO_JOB_POOL_FREE);
            pJob->ssdList              = SSD_WO_CACHE;
            pJob->ssdIdx[SSD_WO_CACHE] = cacheIdx;
            pJob->ssdCnt[SSD_WO_CACHE] = GC_REQ_SSD_COUNT_LP;
            pJob->paaCnt               = GC_REQ_CACHE_ENTRY_LP;
            pJob->cmd                  = FIO_JOB_CMD_READ;
            pJob->css                  = CACHE_LASTPAGE;
            pJob->plane                = GC_NF_PLANE_PER_LUN;

            // Fill last page PAA
            for (ch = 0; ch < GC_NF_NUM_CHANNELS; ch++)
            {
                for (plane = 0; plane < GC_NF_PLANE_PER_LUN; plane++)
                {
                    for (frag = 0; frag < GC_ECU_AU_PER_PAGE; frag++)
                    {
                        gFastPaaBuf[paaCnt].b.ce    = ce;
                        gFastPaaBuf[paaCnt].b.lun   = lun;
                        gFastPaaBuf[paaCnt].b.ch    = ch;
                        gFastPaaBuf[paaCnt].b.frag  = frag;
                        gFastPaaBuf[paaCnt].b.plane = plane;
                        gFastPaaBuf[paaCnt].b.page  = GC_LP_PAGE_IDX;
                        gFastPaaBuf[paaCnt].b.block = blk;
                        gFastPaaBuf[paaCnt].b.block = GC_Get_RmpBlk(gFastPaaBuf[paaCnt]);
                        paaCnt++;
                    }
                }
            }

            MEM_COPY(pJob->paa, gFastPaaBuf, sizeof(tPAA) * pJob->paaCnt);

            pTail = GET_SSD_PTR_FROM_INDEX(cacheIdx);
            die = FTL_GET_DIE_NUM(lun, ce, 0);//die = FTL_GET_DIE_NUM(lun, ce, ch);
            idx = FTL_GET_LP_IDX(0, 0, die, 0, 0, 0);

            pTail->startAddr = (U32)&gdwGCTestBuf[idx];         // P2L Table Start Address
            pTail->endAddr   = (pTail->startAddr + (GC_FTL_LP_SIZE >> FTL_BANK_SHIFT) - 1); // P2L Table End Address - 1;

            Cache_PopSSD_Other(SSD_POOL_OTHER, pHead, pTail, pJob->ssdCnt[SSD_WO_CACHE]);

            if (((GC_FTL_LP_SIZE >> FTL_BANK_SHIFT) / gEcuInfo.auSize) < pJob->paaCnt) // Avoid dummy
            {
                pTail->nextIndex = SSD_DUMMY_START;
            }
            cacheIdx = pTail->nextIndex;

            Core_JobIssue(pJob);
        }
    }
    Core_JobWait();
    D_PRINTF_GC("-SBK[%x]\n", blk);
    GC_P2lDumpInfo(FALSE, TRUE, blk);
#endif
}

//--------------------------------------------------------------------------
// Function     : void GC_P2lDumpInfo(U8 bufferSelect)
// Description  :
// Input        : U8 bufferSelect   (0: gdwGCTestBuf  || 1: gdwGCLastPageBuf)
// return       : N/A
//---------------------------------------------------------------------------
void GC_P2lDumpInfo(U8 bufferSelect, U8 l2pcheck, U16 block)
{
#if (DBG_GC_LP == TRUE)
    tPAA phyPaa;
    U32 *buffer;
    U32  idx, vac = 0;
    U16  page;
    U8   ch, ce, lun, die, plane, wl, frag;;
    U8   curP2lIdx, ppNum;

    if (bufferSelect)
    {
        buffer = gdwGCLastPageBuf;
        ppNum  = GC_PING_PONG_NUM;
    }
    else
    {
        buffer = gdwGCTestBuf;
        ppNum  = 1;
    }

    for (curP2lIdx = 0; curP2lIdx < ppNum; curP2lIdx++)
    {
        D_PRINTF("P2L Table[%d]\n", curP2lIdx);
        for (wl = 0; wl < GC_NF_WL_PER_BLK; wl++)
        {
            for (lun = 0; lun < GC_NF_LUN_PER_DEVICE; lun++)
            {
                for (ce = 0; ce < GC_NF_DEVICE_PER_CHANNEL; ce++)
                {
                    for (page = 0; page < GC_NF_PAGE_PER_PROG_UNIT; page++)
                    {
                        D_PRINTF("- WL[%d]LN[%d]CE[%d]CH[%d]\n", wl, lun, ce, ch);
                        for (ch = 0; ch < GC_NF_NUM_CHANNELS; ch++)
                        {
                            for (plane = 0; plane < GC_NF_PLANE_PER_LUN; plane++)
                            {
                                for (frag = 0; frag < GC_XFER_ORDER; frag++)
                                {
                                    die = FTL_GET_DIE_NUM(lun, ce, ch);
                                    idx = FTL_GET_LP_IDX(0, curP2lIdx, die, (wl * GC_NF_PAGE_PER_PROG_UNIT + page), plane, frag);
                                    phyPaa.b.ch    = ch;
                                    phyPaa.b.ce    = ce;
                                    phyPaa.b.lun   = lun;
                                    phyPaa.b.block = block;
                                    phyPaa.b.plane = plane;
                                    phyPaa.b.page  = page;
                                    phyPaa.b.frag  = frag;

                                    if (!l2pcheck)
                                    {
                                        D_PRINTF("PG[%2x]I[%x]L[%x]  ", page, idx, buffer[idx]);
                                    }
                                    else
                                    {
                                        if (gFtlL2PTbl[buffer[idx]].all32 == phyPaa.all32 && phyPaa.all32 != DWORD_MASK)
                                        {
                                            vac++;
                                        }
                                    }
                                }
                                if (!l2pcheck) D_PRINTF("\n");
                            }
                        }
                    }
                }
            }
        }
    }
    D_PRINTF("\n");
    D_PRINTF("B[%4x]VacGC[%8x]VacL2P[%8x]\n", block, vac, gFtlBlkListTbl[block].vs.b.vac);
#endif
}

#endif

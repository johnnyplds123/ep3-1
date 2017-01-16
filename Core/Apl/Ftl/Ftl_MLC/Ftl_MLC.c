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
//! @file
//! @brief Flash Translation Layer MLC code
//!
//=============================================================================
//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "Cache.h"
#include "SysInfo.h"
#include "ErrHandle.h"
#include "Ftl.h"
#include "Gc.h"
#include "Core.h"

#include "Ftl_MLC.h"

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
//  Private function proto-type definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
#include "PerfSection.h"
//-------------------------------------------------------------------
// Function     : void FTL_FlushUpdate(tFIO_JOB* pJob, BOOL copyPaaAux)
// Description  : Normal flow write paa
// Input        : tFIO_JOB* pJob, BOOL copyPaaAux
// return       : N/A
//-------------------------------------------------------------------
void FTL_FlushUpdate(tFIO_JOB* pJob, BOOL copyPaaAux)
{
    tPAA    paa, paaOld, paaLogic;
    U32     laa     = 0, laaCnt = 0, idx = 0, cachePool = 0, sn;
    U16     die     = 0, bank   = 0;
    U8      pp = 0, lpGroup = 0;
    U8      header  = pJob->header;
    U8      group   = pJob->group;

    if (copyPaaAux && (pJob->css == CACHE_FLUSH))
    {
        cachePool = smCacheMgr.cacheEntry[pJob->ssdIdx[SSD_W_CACHE]].pool;
    }

    paaOld.all32    = DWORD_MASK;
    paaLogic.all32  = 0;
    paa             = FTL_GetFreePage(pJob);
    paaLogic.b.block= paa.b.block;
    paaLogic.b.page = paa.b.page;

    pp      = FTL_GET_LP_PP_IDX(paa.b.lun, paa.b.ce, header, group);
    lpGroup = header * FTL_GROUP_NUM + group;
    sn      = FTL_JOB_SN_SET(header, gGCInfo.boot.flag, gFtlCurrHeader[header][group].sn);

    for (laaCnt = 0; laaCnt < pJob->paaCnt; laaCnt++)
    {
        paa = FTL_NANDWriteProcess(laaCnt, paa, paaLogic.b.page);

        //D_PRINTF("[%3d]ch[%d]ce[%d]ln[%d]pn[%d]bk[%4x]pg[%3x]fg[%d]\n", laaCnt, paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, paa.b.page, paa.b.frag);

        //PhyBlock -> SBlk -> LogBlock
        //LogBlock -> SBlk -> PhyBlock
        paa.b.block = FTL_FastGetRMPBlk(paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paaLogic.b.block);

        //D_PRINTF("[%03d]ch[%d]ce[%d]ln[%d]pn[%d]bk[%4x]pg[%2x]fg[%d]\n", laaCnt, paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, paa.b.page, paa.b.frag);

        gFastPaaBuf[laaCnt] = paa;

        laa = gFastLaaBuf[laaCnt];

        if ((laa != DWORD_MASK) && (laa < L2P_NUM))
        {
            //D_PRINTF_FTL("[%03d]laa:0x%X paa:0x%X\n", laaCnt, laa, paa);

            paaOld = gFtlL2PTbl[laa];

            if ((paaOld.all32 != DWORD_MASK) &&    //Overwrite
                (paaOld.all32 != paa.all32) &&     //for NBT
                (header == FTL_BLOCK_HEADER_HOST)) //exclude GC over write
            {
                #if (FTL_DRAM_CHEK == TRUE)
                    if (paaOld.b.block >= gNfInfo.blocksPerPlane)
                    {
                        D_PRINTF_FTL("[0x%X]laa[%X]paaO[%X]ch[%1X]ce[%1X]l[%1X]pn[%1X]b[%4x]pg[%2X]f[%1X]\n", &gFtlL2PTbl[laa], laa, paaOld, paaOld.b.ch, paaOld.b.ce, paaOld.b.lun, paaOld.b.plane, paaOld.b.block, paaOld.b.page, paaOld.b.frag);
                        FTL_DumpL2P(laa - 8, 16);
                        ASSERT(paaOld.b.block < gNfInfo.blocksPerPlane);
                    }
                #endif
                FTL_DecreaseVC(paaOld);
            }

            #if (FTL_DRAM_CHEK == TRUE)
                if (paa.b.block >= gNfInfo.blocksPerPlane)
                {
                    D_PRINTF_FTL("laa[%X]paa[%X]ch[%1X]ce[%1X]l[%1X]pn[%1X]b[%4x]pg[%2X]fg[%1X]\n", laa, paa, paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, paa.b.page, paa.b.frag);
                    FTL_DumpL2P(laa - 8, 16);
                    ASSERT(paa.b.block < gNfInfo.blocksPerPlane);
                }
            #endif

            gFtlL2PTbl[laa] = paa;

            //clear Trim BMP
            if (cachePool != SSD_POOL_GC)
            {
                //D_PRINTF("laa[%x]chk[%x][%x]BMP[%x]\n", laa, FTL_TRIM_CHK_BMP(laa), gFtlTrimInfo.trimBMP[laa >> 5] & BIT(laa & 31), gFtlTrimInfo.trimBMP[laa >> 5]);
                //if (FTL_TRIM_CHK_BMP(laa))
                //    D_PRINTF("Wr:laa[%x]chk[%x]cnt[%x]\n", laa, FTL_TRIM_CHK_BMP(laa), gFtlTrimInfo.trimBMPcnt);
                FTL_TRIM_CLR_BMP(laa);
            }
        }
        else if (laa == DWORD_MASK)    //Dummy Write
        {
            if (gFtlBlkListTbl[paaLogic.b.block].vs.b.vac)
            	gFtlBlkListTbl[paaLogic.b.block].vs.b.vac--;
        }
        else
        {
            D_PRINTF_FTL("nLAA[%d]:%X\n", laaCnt, laa);
            Debug_DumpMemU32((U32)gFastLaaBuf, sizeof(U32) * pJob->paaCnt);

            ASSERT(FALSE);
        }

        //update last page buffer
        //idx: Group | Ping-Pong | die | page | plane | frag
        die = FTL_GET_DIE_NUM(paa.b.lun, paa.b.ce, paa.b.ch);
        idx = FTL_GET_LP_IDX(lpGroup, pp, die, paa.b.page, paa.b.plane, paa.b.frag);
        gFtlLastPageBuf.lastPageBufFtlP2L[idx] = laa;

        if (pJob->css == CACHE_LASTPAGE)    gFastAuxBuf[laaCnt * AUX_BUF_MAX_CNT + AUX_BUF_IDX_LAA] = FTL_LAST_PAGE_TAG;
        else                                gFastAuxBuf[laaCnt * AUX_BUF_MAX_CNT + AUX_BUF_IDX_LAA] = laa;

        gFastAuxBuf[laaCnt * AUX_BUF_MAX_CNT + AUX_BUF_IDX_SN] = sn;
        //D_PRINTF("D[%2d]G[%d]pp[%d]idx[%x]\n", die, lpGroup, pp, idx);

        //D_PRINTF_FTL("FUD:L[%x]P[%x]PO[%x]\n", laa, paa.all32, paaOld.all32);
    }

    if (copyPaaAux)
    {
        MemCopyNU32((U32*)pJob->paa, (U32*)gFastPaaBuf, pJob->paaCnt * sizeof(tPAA));
        MemCopyNU32((U32*)pJob->aux, (U32*)gFastAuxBuf, pJob->paaCnt * META_DATA_SIZE);
    }

    //last page
    if (pJob->css == CACHE_LASTPAGE)
    {
        bank = (paa.b.lun << FTL_CE_SHIFT) | paa.b.ce;
        gFtlMgr.LastPageBufIdx[bank][header][group]++;
        gFtlMgr.LastPageBufIdx[bank][header][group] %= 2;

        if (FTL_GET_CURRENT_BANK(header, group) == 0)
        {
            FTL_CopyFtlInfoLastPageBuf();
        }
    }
}

//-------------------------------------------------------------------
// Function     : __inline tPAA FTL_GetFreePage(tFIO_JOB* pJob)
// Description  : Get Current Header
// Input        : tFIO_JOB* pJob
// return       : tPAA
//-------------------------------------------------------------------
__inline tPAA FTL_GetFreePage(tFIO_JOB* pJob)
{
    tPAA    paa;
    U16     bank, newBlk, blk, page, group, header;

    //die = lun | ce;

    paa.all32 = 0;

    header = pJob->header;
    group  = pJob->group;
    //ASSERT(group == ((gFastLaaBuf[0] >> FTL_LAA2GROUP_SHIFT) & FTL_GROUP_MASK));

    bank  = gFtlCurrHeader[header][group].bank;
    blk   = gFtlCurrHeader[header][group].block;
    page  = gFtlCurrHeader[header][group].page;


    //if ((blk == WORD_MASK) || ((page == gNfInfo.pagesPerBlock) && (die == 0))) // no current header or write finish a block
    if (FTL_CURRENT_HEADER_NEW(header, group))
    {
        if (blk != WORD_MASK) // Write finsh a block
        {
            if (header == FTL_BLOCK_HEADER_TABLE)
            {
                FTL_BlockPushList(FTL_BLOCK_POOL_PBT, blk, FTL_SORT_NONE);
            }
            else
            {
                if (gFtlBlkListTbl[blk].vs.b.bad)
                {
                    #if (ENABLE_GC == TRUE)
                    GC_PushBlk(blk, gFtlBlkListTbl[blk].vs.b.status, TRUE);
                    D_PRINTF_FTL("DF Blk[%4x]PUSH2GC\n", blk);
                    #else
                    D_PRINTF_FTL("DF Blk[%4x]PUSH2BAD\n", blk);
                    FTL_BlockPushList(FTL_BLOCK_POOL_BAD, blk, FTL_SORT_NONE);
                    #endif

                }
                else
                {
                    FTL_BlockPushList(FTL_BLOCK_POOL_USER, blk, FTL_SORT_NONE);
                }
            }
        }

        newBlk = FTL_BlockPopHead(FTL_BLOCK_POOL_FREE); // NewHeaderBlock: Header=0x%04x, blk=0x%04x

        if (newBlk != WORD_MASK)
        {
            if (header == FTL_BLOCK_HEADER_TABLE) // partial block
            {
                //D_PRINTF("lop[%d]\n", gPbtInfo.loopCnt);
                if ((++gFtlPbtInfo.loopCnt > FTL_QBT_BLOCK_COUNT) || (gFtlPbtInfo.block == WORD_MASK)) // program count of PBT over threshold or first time program PBT
                {
                    if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT] > FTL_QBT_BLOCK_COUNT)
                    {
                        FTL_PurgePbt2Free();

                        //PBT closed,push QBT block to Free pool.
                    #if (FTL_PARTIAL_LOAD_L2P != FALSE)
                        if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_QBT])
                        {
                            FTL_PurgePool2Free(FTL_BLOCK_POOL_QBT);
                        }
                    #endif

                        gFtlMgr.TableSN = gFtlSNTable[gFtlBlkListInfo.head[FTL_BLOCK_POOL_PBT]] - 1;
                    }
                    //gFtlPbtInfo.loopCnt = gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT] + 1;
                    gFtlPbtInfo.loopCnt = 1;
                }

                gFtlPbtInfo.sn                      = gFtlMgr.SerialNumber;
                gFtlPbtInfo.block                   = newBlk;
                gFtlBlkListTbl[newBlk].vs.b.vac     = 0;
                D_PRINTF_FTL("PBTnewB[%4x]SN[%x]loopCnt[%d]poolCnt[%d]trimCnt[%x]TBLSn[%x]\n", newBlk, gFtlPbtInfo.sn, gFtlPbtInfo.loopCnt, gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT], gFtlTrimInfo.trimBMPcnt,gFtlMgr.TableSN);
                //D_PRINTF("PBtimC[%x]laa[%x]\n", gFtlTrimInfo.trimBMPcnt, FTL_LAA_BANK_PER_PAGE);

                gFtlCurrHeader[header][group].sn    = gFtlPbtInfo.sn;

                if ((FTL_TRIM_UPDATE != FALSE) && (gFtlTrimInfo.trimUpdate != TRUE) && (gFtlTrimInfo.trimBMPcnt >= FTL_LAA_BANK_PER_PAGE) && (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] < gFtlTrimRatio) && (gFtlPbtInfo.loopCnt == 1))
                {
                    D_PRINTF("PBT-updateTrim\n");
                    gFtlTrimInfo.trimUpdate = TRUE;
                }
            }
            else // new block for user data
            {
                gFtlCurrHeader[header][group].sn    = gFtlMgr.SerialNumber;
                gFtlBlkListTbl[newBlk].vs.b.vac 	= FTL_MAX_VAC;

            }
            //D_PRINTF_FTL("pop new blk:0%4x\n", newBlk);
            gFtlCurrHeader[header][group].bank  = bank = 0;
            gFtlCurrHeader[header][group].block = blk  = newBlk;
            gFtlCurrHeader[header][group].page  = page = 0;

            if (header == FTL_BLOCK_HEADER_HOST)
                gFtlBlkListTbl[newBlk].vs.b.host = 1;
            else
                gFtlBlkListTbl[newBlk].vs.b.host = 0;

            if (gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE] == WORD_MASK)
            {
                D_PRINTF("NfBk\n"); // GetFreePage(). No free block!!!
                ASSERT(FALSE);
            }
            else
            {
                gFtlMgr.forceErase = FTL_BANK_BIT_MASK;
                gFtlBlkListTbl[newBlk].eraseCnt++;
                gFtlMgr.AllEraseCnt++;
            }

            gFtlSNTable[newBlk] = gFtlCurrHeader[header][group].sn;
            gFtlMgr.SerialNumber++;
        }
        else
        {
            ASSERT(FALSE);
        }
    }

    if (++gFtlCurrHeader[header][group].bank == FTL_BANK_NUM)
    {
        gFtlCurrHeader[header][group].bank = 0;
        gFtlCurrHeader[header][group].page++;
    }

    paa.b.lun   = bank >> FTL_CE_SHIFT;
    paa.b.ce    = bank & FTL_CE_MASK;
    paa.b.page  = page;
    paa.b.block = blk;

    //D_PRINTF("free:H[%1x]G[%1x]D[%2x]B[%4x]P[%3x]\n", header, group, die, blk, page);

    return (paa);
}

//-------------------------------------------------------------------
// Function     : __inline tPAA FTL_NANDWriteProcess(U32 idx, tPAA paa, U16 logicpage)
// Description  : Process NAND write PAA
// Input        : U32 idx, tPAA paa, U16 logicpage
// return       : tPAA
//-------------------------------------------------------------------
__inline tPAA FTL_NANDWriteProcess(U32 idx, tPAA paa, U16 logicpage)
{
    paa.b.frag  = ((idx & FTL_FLUSH_FRAG_MASK) >> FTL_FLUSH_FRAG_SHIFT);
    paa.b.ch    = ((idx & FTL_FLUSH_CH_MASK) >> FTL_FLUSH_CH_SHIFT);
    paa.b.plane = ((idx & FTL_FLUSH_PLANE_MASK) >> FTL_FLUSH_PLANE_SHIFT);

    return paa;
}

//-------------------------------------------------------------------
// Function     : __inline void FTL_NANDEraseProcess(tFIO_JOB* pJob, U8 ce, U8 lun, U16 block)
// Description  : Proecess NAND erase PAA
// Input        : tFIO_JOB* pJob, U8 ce, U8 lun, U16 block
// return       : N/A
//-------------------------------------------------------------------
__inline void FTL_NANDEraseProcess(tFIO_JOB* pJob, U8 ce, U8 lun, U16 block)
{
    U16 rmpBlk, idx = 0;
    U8  plane, ch;

    for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
    {
        for (ch = 0; ch < gNfInfo.numChannels; ch++)
        {
            if (block >= gNfInfo.blocksPerPlane)
            {
                D_PRINTF("idx[%d]PAACNT[%d]PL[%d]gNfinfoPlane[%d]B[%4x]\n", idx, pJob->paaCnt, plane, gNfInfo.planesPerLun, block);
                ASSERT(0);
            }

            rmpBlk = FTL_FastGetRMPBlk(ch, ce, lun, plane, block);

            gFastPaaBuf[idx].all32      = 0;
            gFastPaaBuf[idx].b.ch       = ch;
            gFastPaaBuf[idx].b.ce       = ce;
            gFastPaaBuf[idx].b.lun      = lun;
            gFastPaaBuf[idx].b.block    = rmpBlk;
            gFastPaaBuf[idx].b.plane    = plane;
            idx++;

            //D_PRINTF("E[%2d]CH[%d]CE[%d]LN[%d]PN[%d]BK[%4x]rB[%4x]\n", idx, ch, ce, lun, plane, block, rmpBlk);
        }
    }

    MEM_COPY(pJob->paa, gFastPaaBuf, sizeof(tPAA) * idx);
}

//-------------------------------------------------------------------
// Function     : U32 FTL_LastPageCacheList(tFIO_JOB* pJob, U8 ce, U8 lun, U8 copyBuf)
// Description  :
// Input        :
// return       : N/A
//-------------------------------------------------------------------
U32 FTL_LastPageCacheList(tFIO_JOB* pJob, U8 ce, U8 lun, U8 copyBuf)
{
    tPAA paa;
    U32 idx, ssdCnt, cacheIdx;
    U8  die, group, pp;

    MEM_CLR(gFtlLastPageP2LIdx, sizeof(U8) * MAX_SUPPORTED_CHANNELS);

    cacheIdx        = pJob->ssdIdx[SSD_WO_CACHE];
    gFtlBtSSDHead   = GET_SSD_PTR_FROM_INDEX(cacheIdx);

    paa.all32 = 0;

    for (ssdCnt = 0; ssdCnt < FTL_FLUSH_BLT_P2L_PER_PAGE; ssdCnt++)
    {
        paa = FTL_NANDWriteProcess(ssdCnt, paa, 0);
        //D_PRINTF("[%3d]H[%d]CH[%d]CE[%d]LN[%d]PN[%d]FG[%d]\n", paaCnt, pJob->header, paa.b.ch, ce, lun, paa.b.plane, paa.b.frag);

        gFtlBtSSDTail = GET_SSD_PTR_FROM_INDEX(cacheIdx);

        if (paa.b.plane == 0)
        {
            //P2L Table
            //idx: Group | Ping-Pong | die | plane | page | frag
            die     = FTL_GET_DIE_NUM(lun, ce, paa.b.ch);

            //if (copyBuf == TRUE)
            //{
            //    pp      = group = 0;
            //}
            //else
            {
                pp      = FTL_GET_LP_PP_IDX(lun, ce, pJob->header, pJob->group);
                group   = pJob->header * FTL_GROUP_NUM + pJob->group;
            }

            idx = FTL_GET_LP_IDX(group, pp, die, gFtlLastPageP2LIdx[paa.b.ch] * FTL_GET_LP_PAGE_PER_4K, 0, 0);

            gFtlLastPageP2LIdx[paa.b.ch]++;

            if (gFtlLastPageP2LIdx[paa.b.ch] * FTL_GET_LP_PAGE_PER_4K >= gNfInfo.pagesPerBlock)
                gFtlLastPageP2LIdx[paa.b.ch] = 0;

            //if (copyBuf == TRUE)
            //{
            //    gFtlBtSSDTail->startAddr = (U32)&gFtlLastPageBuf.p2lBuf[idx];                   //P2L Table Start Address
            //    //D_PRINTF("P2L-W:idx[%x]die[%2d]ch[%d]ce[%d]ln[%d]pn[%d]fg[%d]\n", idx, die, paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.frag);
            //}
            //else
            {
                gFtlBtSSDTail->startAddr = (U32)&gFtlLastPageBuf.lastPageBufFtlP2L[idx];        //P2L Table Start Address
                //D_PRINTF("P2L:die[%d]idx[%x]\n", die, idx);
            }
        }
        else if (paa.b.plane == 1)
        {
            //Block Table
            //pTail->startAddr = (U32)&gbLastPageBufFtlInfo[(ch%2)*0x4000 + (fragH * gEcuInfo->auPerHalfPage + fragL)*0x1000];
            gFtlBtSSDTail->startAddr = (U32)&gFtlLastPageBuf.lastPageBufFtlInfo[paa.b.ch * 0x4000 + paa.b.frag * 0x1000];
            //D_PRINTF("W- ch[%d]pl[%d]fg[%d]id[%X]\n", paa.b.ch, paa.b.plane, paa.b.frag, paa.b.ch * 0x4000 + paa.b.frag * 0x1000);
        }

        gFtlBtSSDTail->endAddr   = (gFtlBtSSDTail->startAddr + 0x1000 - 1);
        cacheIdx = gFtlBtSSDTail->nextIndex;
    }

    pJob->ssdCnt[SSD_WO_CACHE] = ssdCnt;
    Cache_PopSSD_Other(SSD_POOL_OTHER, gFtlBtSSDHead, gFtlBtSSDTail, pJob->ssdCnt[SSD_WO_CACHE]);

    if (pJob->ssdCnt[SSD_WO_CACHE] < pJob->paaCnt)
    {
        gFtlBtSSDTail->nextIndex = SSD_ZERO_START;
    }

    return (gFtlBtSSDTail->nextIndex);
}

#include "FastSection.h"
//-------------------------------------------------------------------
// Function     : void FTL_LastPageP2LUpdateL2P(U16 group, U16 block)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_LastPageP2LUpdateL2P(U16 group, U16 block)
{
    tPAA paa, paaOld;
    U32  laa = DWORD_MASK;
    U32  idx1 = 0, idx2 = 0, snlast;
    U16  bank = 0;
    U16  die  = 0, ch, plane, page, frag;

    if (gFtlNbtType.existFQBT)
    {
        snlast = gFtlPbtInfo.sn;
        if (gFtlSNTable[block] > snlast)
	    {
	    	gFtlBlkListTbl[block].vs.b.vac = FTL_MAX_VAC;
	    }
    }
	else
	{
        snlast = (gFtlPbtInfo.sn + ((FTL_PBT_PERIOD_BLK + 1) * FTL_QBT_BLOCK_COUNT - 2));
	    if ((gFtlNbtType.existPBT != TRUE) || ((gFtlNbtType.existPBT != FALSE) && (gFtlSNTable[block] > snlast)))
	    {
	    	gFtlBlkListTbl[block].vs.b.vac = FTL_MAX_VAC;

            if ((gFtlNbtType.existPBT != TRUE) && (gFtlPbtInfo.sn == 0))
            {
                snlast = 0;
            }
	    }
	}

    //D_PRINTF("B[%4x]sn[%x]-snlast[%x] VAC[%x]->", block, gFtlSNTable[block], snlast, gFtlBlkListTbl[block].vs.b.vac);
    //update L2P table
    for (page = 0; page < gNfInfo.pagesPerBlock; page++)
    {
        paa.all32   = 0;
        paa.b.page  = page;

        for (bank = 0; bank < FTL_BANK_NUM; bank++)
        {
            paa.b.ce    = bank & FTL_CE_MASK;
            paa.b.lun   = bank >> FTL_CE_SHIFT;

            for (ch = 0; ch < gNfInfo.numChannels; ch++)
            {
                paa.b.ch = ch;
                die = FTL_GET_DIE_NUM(paa.b.lun, paa.b.ce, paa.b.ch);

                idx1 = FTL_GET_LP_IDX(group, 0, die, paa.b.page, 0, 0);
                MemCopyNU32(gFtlFastVacBuf, &gFtlLastPageBuf.lastPageBufFtlP2L[idx1], sizeof(U32) * gNfInfo.planesPerLun * gEcuInfo.auPerPage);

                idx2 = 0;
                for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
                {
                    paa.b.plane = plane;
                    paa.b.block = FTL_FastGetRMPBlk(paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, block);

                    //optimal code
                    for (frag = 0; frag < gEcuInfo.auPerPage; frag++)
                    {
                        paa.b.frag = frag;
                        laa = gFtlFastVacBuf[idx2++];

                        if((laa != DWORD_MASK) && (laa > L2P_MASK))
                        {
                            D_PRINTF_FTL("laaEr:BK[%4X]PN[%d]PG[%2x]laa[0x%x]\n", paa.b.block, paa.b.plane, paa.b.page, laa);
                            ////DBG_LOG_LIT(0x46, 0x18, 3, page, laa>>16, laa);// laaErr: page=0x%04x, laa=0x%08X
                            ASSERT(0);
                        }

                        if (laa != DWORD_MASK)
						{
                            if (gFtlSNTable[block] > snlast)
                            {
						        paaOld = gFtlL2PTbl[laa];
						        if ((paaOld.all32 != DWORD_MASK) &&    //Overwrite
                                   (paaOld.all32 != paa.all32))     //for NBT
					            {
                                    //D_PRINTF("Rewrite laa[%x]:\n", laa);
								    FTL_DecreaseVC(paaOld);
						        }
							}

						    gFtlL2PTbl[laa] = paa;
                        }
						else  //laa == DWORD_MASK
						{
						    if (gFtlSNTable[block] > snlast)
							{
							    if (gFtlBlkListTbl[block].vs.b.vac)
							    {
                                    gFtlBlkListTbl[block].vs.b.vac--;
							    }
						    }
                            //D_PRINTF("dumy B[%4x]vac[%x]\n", block, gFtlBlkListTbl[block].vs.b.vac);
						}
                    }
                }
            }
        }
    }
    //D_PRINTF("VAC1[%x]\n", gFtlBlkListTbl[block].vs.b.vac);

    MEM_CLR(gFtlFastVacBuf, sizeof(U32) * FTL_SUPPORT_MAX_BLK);
}

//-------------------------------------------------------------------
// Function     : void FTL_ReleaseFreeBlock(U16 blk, U8 bTrimData, U32 sn)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_ReleaseFreeBlock(U16 blk, U8 bTrimData, U32 sn)
{
    tPAA paaL2P,paaP2L;
    U32 laa = DWORD_MASK;
    U32 idx1 = 0,idx2;
    U16 ch   = 0, ce   = 0, die  = 0, lun = 0, plane = 0, page = 0, frag = 0;

    D_PRINTF_FTL("FTL_RELFreeBlk B[%4x]vac[%x],sts[%4x]\n", blk, gFtlBlkListTbl[blk].vs.b.vac, gFtlBlkListTbl[blk].vs.b.status);

    if(FTL_NANDReadP2LBuildL2P(0, blk, (gNfInfo.pagesPerBlock - 1)))
    {
        gFtlBlkListTbl[blk].vs.b.vac = 0;

        idx1 = 0;
		paaP2L.all32 = 0;

        for (page = 0; page < gNfInfo.pagesPerBlock; page++)
    	{
    	    paaP2L.b.page = page;
	        for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
	        {
	            paaP2L.b.lun = lun;
	            for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
	            {
	                paaP2L.b.ce = ce;
	                for (ch = 0; ch < gNfInfo.numChannels; ch++)
	                {
	                    paaP2L.b.ch = ch;
	                    die = FTL_GET_DIE_NUM(lun, ce, ch);

	                    idx1 = FTL_GET_LP_IDX(0, 0, die, page, 0, 0);

	                    MemCopyNU32(gFtlFastVacBuf, &gFtlLastPageBuf.lastPageBufFtlP2L[idx1], sizeof(U32) * gNfInfo.planesPerLun * gEcuInfo.auPerPage);

	                    idx2 = 0;
	                    for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
	                    {
	                        paaP2L.b.plane = plane;
                            paaP2L.b.block = FTL_FastGetRMPBlk(paaP2L.b.ch, paaP2L.b.ce, paaP2L.b.lun, paaP2L.b.plane, blk);

	                        for (frag = 0; frag < gEcuInfo.auPerPage; frag++)
                        	{
                        	    paaP2L.b.frag = frag;
								laa = gFtlFastVacBuf[idx2++];

								if (laa != DWORD_MASK)
								{

									paaL2P = gFtlL2PTbl[laa];
                                    //D_PRINTF("laa [%x], paaL2P[%x]paaP2L[%x]\n",laa,paaL2P.all32,paaP2L.all32);
									if (paaP2L.all32 == paaL2P.all32)
									{   //valid data
									    if (bTrimData)
                                            gFtlL2PTbl[laa].all32 = DWORD_MASK;
                                        else
										    gFtlBlkListTbl[blk].vs.b.vac++;
									}
								}
								else
								{
								    if ((laa != DWORD_MASK) && (laa > L2P_NUM))
										D_PRINTF("laa [%x], idx1 [%x]\n", laa,idx1);
								}
	                        }
	                    }
	                }
	            }
	        }
        }

		if (gFtlBlkListTbl[blk].vs.b.vac < FTL_MAX_VAC)
        {
            if ((!gFtlBlkListTbl[blk].vs.b.vac) &&
    			((gFtlBlkListTbl[blk].vs.b.status == FTL_BLOCK_POOL_USER) ||
    			 (gFtlBlkListTbl[blk].vs.b.status == FTL_BLOCK_POOL_GC)||
    			 (gFtlBlkListTbl[blk].vs.b.status == FTL_BLOCK_POOL_EMPTY))) // user block vac
            {
                if (gFtlSNTable[blk] < (sn-1))
                    FTL_BlockPopPushList(FTL_BLOCK_POOL_FREE, blk, FTL_SORT_BY_EC);
			}
        }
    }
    else
    {
        gFtlBlkListTbl[blk].vs.b.vac = 0;
    }
}

//-------------------------------------------------------------------
// Function     : U8 FTL_BuildOpenBlockP2L(U16 startPage, U16 startBank, U16 block)
// Description  : Scan open block aux to rebuild P2L buffer
// Input        :
// return       : blank error
//-------------------------------------------------------------------
U8 FTL_BuildOpenBlockP2L(U16 startPage, U16 startBank, U16 block)
{
    tPAA paa;
    U32 pp, p2lIdx, paaCnt, laa, die;
    U16 page, bank;
    U8  ce, lun, plane, blankError = FTL_OPEN_BLOCK_EMPTY_MAX;

    for (page = startPage; page < FTL_NAND_LAST_PAGE; page++)
    {
        for (bank = startBank; bank < FTL_BANK_NUM; bank++)
        {
            ce  = bank & FTL_CE_MASK;
            lun = bank >> FTL_CE_SHIFT;

            pp  = FTL_GET_LP_PP_IDX(lun, ce, FTL_BLOCK_HEADER_HOST, 0);

            for (plane = 0; plane < FTL_PLANE_NUM; plane++)
            {
                gFtlBTJob[0] = FTL_NANDReadPageBuild(CACHE_SCANPAGE, 0,
                    ce, lun, plane, block, page, FTL_SCAN_PAGE_AUX, FALSE);

                Core_JobWait();

                if (gFtlBTJob[0]->status & FIO_S_UNC_ERR)
                {
                	D_PRINTF_FTL("UNC-CE[%d]LN[%d]pn[%4x]BK[%4x]pg[%4x]\n", ce, lun, plane, block, page);
                }

                if (gFtlBTJob[0]->status & FIO_S_BLANK_ERR)
                {
                    D_PRINTF_FTL("EMP-CE[%d]LN[%d]pn[%4x]BK[%4x]pg[%4x]\n", ce, lun, plane, block, page);
                }

                for (paaCnt = 0; paaCnt < gFtlBTJob[0]->paaCnt; paaCnt++)
                {
                	laa = (FIO_CHK_ERR_BMP(gFtlBTJob[0]->fid, paaCnt)) ? DWORD_MASK : gFtlBTJob[0]->aux[paaCnt << 1];


                    paa = gFtlBTJob[0]->paa[paaCnt];
                    //if(laa != DWORD_MASK)
                        //D_PRINTF("[%x]ch[%d]ce[%d]ln[%d]pn[%d]pg[%3x]fg[%d]\n", laa, paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.page, paa.b.frag);

                    die  = FTL_GET_DIE_NUM(lun, ce, paa.b.ch);
                    p2lIdx = FTL_GET_LP_IDX(0, pp, die, page, plane, paa.b.frag);

                    gFtlLastPageBuf.lastPageBufFtlP2L[p2lIdx] = (laa < L2P_NUM) ? laa : DWORD_MASK;
                }
            }

            if ((bank == 0) && (gFtlBTJob[0]->status & FIO_S_BLANK_ERR))
            {
                blankError = FTL_OPEN_BLOCK_BANK_EMPTY;
                break;
            }
        }

        if (blankError == FTL_OPEN_BLOCK_BANK_EMPTY)
        {
            D_PRINTF("B[%4x]BN[0]PG[%3x] empty\n", block, page);

            if (page == 0)
            {
                blankError = FTL_OPEN_BLOCK_BANK_PAGE_EMPTY;
            }
            break;
        }
    }

    return blankError;
}

//-------------------------------------------------------------------
// Function     : tFIO_JOB* FTL_NANDReadPageBuild(U16 css, U8 group, U8 ce, U8 lun, U8 plane, U16 block, U16 page, U8 readAux, U8 readP2L)
// Description  : Scan page of paa and read aux at same time or not
// Input        : paa, AUX(TRUE or FALSE)
// return       : JOB
//-------------------------------------------------------------------
tFIO_JOB* FTL_NANDReadPageBuild(U16 css, U8 group, U8 ce, U8 lun, U8 plane, U16 block, U16 page, U8 readAux, U8 readP2L)
{
    tFIO_JOB *pJob = NULL;
    tPAA paa;
    U32 idx, idx1 = 0;
    U16 cacheIdx, die = 0;

    gFtlBtSSDHead    = smCacheMgr.head[SSD_POOL_OTHER];
    cacheIdx = GET_SSD_INDEX_FROM_PTR(gFtlBtSSDHead);

    pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
    pJob->ssdList              = SSD_WO_CACHE;
    pJob->ssdIdx[SSD_WO_CACHE] = cacheIdx;
    pJob->ssdCnt[SSD_WO_CACHE] = (1 << (FTL_CH_SHIFT + FTL_FRAG_SHIFT));//FTL_FLUSH_BLT_L2P_PER_PAGE >> FTL_PLANE_HALF_SHIFT;
    pJob->paaCnt               = (1 << (FTL_CH_SHIFT + FTL_FRAG_SHIFT));//FTL_FLUSH_BLT_L2P_PER_PAGE >> FTL_PLANE_HALF_SHIFT;
    pJob->cmd                  = FIO_JOB_CMD_READ;
    pJob->css                  = css;
    pJob->plane                = gNfInfo.planesPerLun;
    pJob->flag                 = (FIO_F_NO_MKERR | FIO_F_MUTE);
    if (readAux)
        pJob->flag            |= (FIO_F_AUX);

    paa.all32   = 0;
    paa.b.ce    = ce;
    paa.b.lun   = lun;
    paa.b.page  = page;

    MEM_CLR(gFtlLastPageP2LIdx, sizeof(U8) * MAX_SUPPORTED_CHANNELS);

    for (idx = 0; idx < pJob->paaCnt; idx++)
    {
        paa = FTL_NANDWriteProcess(idx, paa, paa.b.page);

        paa.b.plane = plane;
        paa.b.block = FTL_FastGetRMPBlk(paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, block);

        gFastPaaBuf[idx] = paa;
        //D_PRINTF("[%d]ch[%d]ce[%d]ln[%d]pn[%d]B[%4x]pg[%3x]fg[%d]\n", idx, paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, paa.b.page, paa.b.frag);

        gFtlBtSSDTail = GET_SSD_PTR_FROM_INDEX(cacheIdx);

        if ((plane == 1) && (page == FTL_NAND_FTL_INFO_PAGE))
        {
            //Block Table
            gFtlBtSSDTail->startAddr = (U32)&gFtlLastPageBuf.lastPageBufFtlInfo[paa.b.ch * 0x4000 + paa.b.frag * 0x1000];

            //D_PRINTF("R- ch[%d]pl[%d]fg[%d]id[%X]\n", paa.b.ch, paa.b.plane, paa.b.frag, paa.b.ch * 0x4000 + paa.b.frag * 0x1000);
        }
        else if ((plane == 0) && (readP2L == TRUE))
        {
            //P2L Table
            //load last page buffer
            //idx: Group | Ping-Pong | die | page | plane | frag
            die  = FTL_GET_DIE_NUM(paa.b.lun, paa.b.ce, paa.b.ch);
            idx1 = FTL_GET_LP_IDX(group, 0, die, gFtlLastPageP2LIdx[paa.b.ch]*FTL_GET_LP_PAGE_PER_4K, 0, 0);

            gFtlLastPageP2LIdx[paa.b.ch]++;

            if (gFtlLastPageP2LIdx[paa.b.ch] * FTL_GET_LP_PAGE_PER_4K >= gNfInfo.pagesPerBlock)
                gFtlLastPageP2LIdx[paa.b.ch] = 0;

            //D_PRINTF("P2L-R:idx[%x]die[%2d]ch[%d]ce[%d]ln[%d]pn[%d]fg[%d]\n", idx1, die, paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.frag);
            gFtlBtSSDTail->startAddr = (U32)&gFtlLastPageBuf.lastPageBufFtlP2L[idx1];       //P2L Table Start Address
        }
        else
        {
            gFtlBtSSDTail->startAddr = smSSD[SSD_DUMMY_START].startAddr;
        }

        gFtlBtSSDTail->endAddr  = (gFtlBtSSDTail->startAddr + 0x1000 - 1);

        cacheIdx        = gFtlBtSSDTail->nextIndex;
    }

    MEM_COPY(pJob->paa, gFastPaaBuf, sizeof(tPAA) * pJob->paaCnt);

    Cache_PopSSD_Other(SSD_POOL_OTHER, gFtlBtSSDHead, gFtlBtSSDTail, pJob->ssdCnt[SSD_WO_CACHE]);

    Core_JobIssue(pJob);

    return (pJob);
}

//-------------------------------------------------------------------
// Function     : void FTL_NANDReadPageAux(U16 block, U16 group, U16 scanpage)
// Description  : Read scan aux.
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_NANDReadPageAux(U16 block, U16 group, U16 scanpage)
{
    tPAA  paa;
    U32   idx = 0, idx1 = 0, laa;
    U16   page = 0, plane = 0;
    U16   ce = 0, lun = 0, die = 0;

    D_PRINTF_FTL("FTL_ReadPageAux\n");
    // scan page get aux
    for (page = 0; page < scanpage; page++)
    {
        for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
        {
            for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
            {
                for (page = 0; page < gNfInfo.pagesPerBlock; page++)
                {
                	for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
                    {
                        gFtlBTJob[0] = FTL_NANDReadPageBuild(CACHE_SCANPAGE, group,
                            ce, lun, plane, block, page,
                            FTL_SCAN_PAGE_AUX, FALSE);

                        Core_JobWait();

                        if (gFtlBTJob[0]->status & FIO_S_UNC_ERR)
                        {
                        	D_PRINTF_FTL("Aux unc:Page[%4x]plane[%4x]\n", page, plane);
                        }

                        for (idx = 0; idx < gFtlBTJob[0]->paaCnt; idx++)
                        {
                        	laa = (FIO_CHK_ERR_BMP(gFtlBTJob[0]->fid, idx)) ? DWORD_MASK : gFtlBTJob[0]->aux[idx << 1];

                            paa.all32 = 0;
                            paa = FTL_NANDWriteProcess(idx, paa, page);

                            die  = FTL_GET_DIE_NUM(lun, ce, paa.b.ch);
                            idx1 = FTL_GET_LP_IDX(0, 0, die, page, plane, paa.b.frag);

            				if(page == (gNfInfo.pagesPerBlock -1))
            					gFtlLastPageBuf.lastPageBufFtlP2L[idx1] = DWORD_MASK;
            				else
                            {
                            	gFtlLastPageBuf.lastPageBufFtlP2L[idx1] = (laa < L2P_NUM) ? laa : DWORD_MASK;
                            }
                        }
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------
// Function     : U8 FTL_NANDReadP2LBuildL2P(tFIO_JOB *pJob)
// Description  : Read P2L build P2L table, if page ecc fail , scan aux.
// Input        : pJob
// return       : Finish(TRUE : finish,FALSE : empty)
//-------------------------------------------------------------------
U8 FTL_NANDReadP2LBuildL2P(U16 group, U16 block, U16 page)
{
    U16   bank = 0;
    U8    ScanAux = FALSE;

    D_PRINTF("LP_L2P:B[%4x]sn[%x]\n", block, gFtlSNTable[block]);

    for (bank = 0; bank < FTL_BANK_NUM; bank++)
    {
        gFtlBTJob[bank] = FTL_NANDReadPageBuild(CACHE_LASTPAGE, group,
            bank & FTL_CE_MASK, bank >> FTL_CE_SHIFT, 0, block, page,
            FTL_SCAN_PAGE_NONE, TRUE);

        Core_JobWait();

		if (gFtlBTJob[bank]->status & FIO_S_UNC_ERR)
        {
            //FIO_RST_ERR_BMP(gFtlBTJob[bank]->fid);
            D_PRINTF_FTL("nbt3 unc:B[%4x]\n", block);

            ScanAux = TRUE; // scan page get aux
            break;
        }
		else if (gFtlBTJob[bank]->status & FIO_S_BLANK_ERR)
        {
            FIO_RST_ERR_BMP(gFtlBTJob[bank]->fid);
            D_PRINTF_FTL("nbt3 ety:B[%4x]\n", block);
            if (gFtlBTJob[bank]->status & FIO_S_UNC_ERR)
                D_PRINTF_FTL("nbt3 empty, unc:B[%4x]\n", block);

            gFtlBlkListTbl[block].vs.b.vac = 0;
            return FALSE;
        }
        else
        {
            if (page != FTL_NAND_FTL_P2L_PAGE)
            {
                D_PRINTF_FTL("aux1[%x],aux0[%x]\n", (gFtlBTJob[bank]->aux[1] >> FTL_META_HEADER_SHIFT), gFtlBTJob[bank]->aux[0]);
                if ((((gFtlBTJob[bank]->aux[1] >> FTL_META_HEADER_SHIFT) != FTL_BLOCK_HEADER_HOST)) ||
                    (gFtlBTJob[bank]->aux[0] != FTL_POR_PAGE_TAG))
                {
                    ScanAux = TRUE;
                }
            }
        }
    }

    if (ScanAux) FTL_NANDReadPageAux(block, group, FTL_NAND_LAST_PAGE);

	return TRUE;
}


//-------------------------------------------------------------------
// Function     : U8 FTL_NANDReadL2P(U16 logicalBlk, U16 loop, U16 pageStart, U16 pageEnd, U32 l2pIdx)
// Description  : Read L2P Table
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
U8 FTL_NANDReadL2P(U16 logicalBlk, U16 loop, U16 pageStart, U16 pageEnd, U32 l2pIdx)
{
    tPAA paa;
    U32  idx      = 0;
    U16  cacheIdx = 0;
    U16  ce       = 0;
    U16  lun      = 0;
    U16  bank     = 0;
    U16  plane    = 0;
    U16  page     = 0;

    if (logicalBlk >= gNfInfo.blocksPerPlane)
    {
        ASSERT(logicalBlk < gNfInfo.blocksPerPlane);
    }

    for (page = pageStart; page < pageEnd; page++)
    {
        if ((logicalBlk == gFtlBlkListInfo.tail[FTL_BLOCK_POOL_QBT]) && (page >= (gNfInfo.pagesPerBlock - (FTL_GROUP_NUM*2))))
        {
            D_PRINTF("loop[%d]pg[%3x]\n", loop, page);
            break;
        }

        if ((loop == (FTL_QBT_BLOCK_COUNT - 1)) && (page >= (gNfInfo.pagesPerBlock - (FTL_GROUP_NUM*2))))
        {
            //D_PRINTF("loop[%d]pg[%3x]\n", loop, page);
            break;
        }

        for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
        {
            for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
            {
                for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
                {
                    bank     = (lun << (FTL_CE_SHIFT + FTL_PLANE_SHIFT)) | (ce << FTL_PLANE_SHIFT) | plane;

                    gFtlBtSSDHead = smCacheMgr.head[SSD_POOL_OTHER];
                    cacheIdx = GET_SSD_INDEX_FROM_PTR(gFtlBtSSDHead);

                    gFtlBTJob[bank] = FIO_PopJob(FIO_JOB_POOL_FREE);
                    gFtlBTJob[bank]->ssdList              = SSD_WO_CACHE;
                    gFtlBTJob[bank]->ssdIdx[SSD_WO_CACHE] = cacheIdx;
                    gFtlBTJob[bank]->ssdCnt[SSD_WO_CACHE] = 1;
                    gFtlBTJob[bank]->paaCnt               = (CACHE_FLUSH_ONE_CE_CNT >> FTL_PLANE_SHIFT);
                    gFtlBTJob[bank]->cmd                  = FIO_JOB_CMD_READ;
                    gFtlBTJob[bank]->css                  = CACHE_SCANPAGE;
                    gFtlBTJob[bank]->plane                = gNfInfo.planesPerLun;
                    gFtlBTJob[bank]->flag                 = FIO_F_NO_MKERR | FIO_F_MUTE;

                    paa.all32  = 0;
                    paa.b.ce   = ce;
                    paa.b.lun  = lun;
                    paa.b.page = page;

                    for (idx = 0; idx < gFtlBTJob[bank]->paaCnt; idx++)
                    {
                        paa = FTL_NANDWriteProcess(idx, paa, paa.b.page); // get paa

                        paa.b.plane = plane;
                        paa.b.block = FTL_FastGetRMPBlk(paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, logicalBlk);

                        gFastPaaBuf[idx] = paa;
                        //D_PRINTF("[%d]ch[%d]ce[%d]ln[%d]pn[%d]B[%4x]pg[%2x]fg[%2x]\n", idx, paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, paa.b.page, paa.b.frag);
                    }

                    MEM_COPY(gFtlBTJob[bank]->paa, gFastPaaBuf, sizeof(tPAA) * gFtlBTJob[bank]->paaCnt);

                    gFtlBtSSDTail = GET_SSD_PTR_FROM_INDEX(cacheIdx);
                    //D_PRINTF("R[%X]ch[%d]ce[%d]ln[%d]pn[%d]B[%4x]pg[%2x]\n", l2pIdx, gFtlBTJob[bank]->paa[0].b.ch, ce, lun, plane, gFtlBTJob[bank]->paa[0].b.block, page);

                    if (l2pIdx >= L2P_NUM)
                    {
                        //D_PRINTF("[%d]ch[%d]ce[%d]ln[%d]pn[%d]B[%4x]pg[%2x]\n", idx, ch, ce, lun, plane, logBlk, page);
                        gFtlBtSSDTail->startAddr = smSSD[SSD_DUMMY_START].startAddr;
                        gFtlBtSSDTail->endAddr   = (gFtlBtSSDTail->startAddr + 0x1000 - 1);
                        Cache_PopSSD_Other(SSD_POOL_OTHER, gFtlBtSSDHead, gFtlBtSSDTail, gFtlBTJob[bank]->ssdCnt[SSD_WO_CACHE]);
                        gFtlBtSSDTail->nextIndex = SSD_DUMMY_START;
                    }
                    else
                    {
                        //D_PRINTF("R:idx2[%x]-ce[%d]ln[%d]pn[%d]B[%4x]pg[%2x]\n", l2pIdx, ce, lun, plane, logicalBlk, page);
                        //pTail->startAddr = (U32)&gL2PMapTbl[idx2];
                        //idx2 += (0x1000 >> (LOG2(sizeof(U32))));

                        gFtlBtSSDTail->startAddr = (U32)&gFtlL2PTbl[l2pIdx];
                        gFtlBtSSDTail->endAddr   = (gFtlBtSSDTail->startAddr + (FTL_QBT_SIZE_PER_FLUSH >> FTL_PLANE_SHIFT) - 1);
                        l2pIdx += (FTL_LAA_BANK_PER_PAGE >> FTL_PLANE_SHIFT);
                        Cache_PopSSD_Other(SSD_POOL_OTHER, gFtlBtSSDHead, gFtlBtSSDTail, gFtlBTJob[bank]->ssdCnt[SSD_WO_CACHE]);
                    }
                    Core_JobIssue(gFtlBTJob[bank]);
                }
            }
        }
        Core_JobWait();

        //check read fail here
        for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
        {
            for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
            {
                for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
                {
                    bank = (lun << (FTL_CE_SHIFT + FTL_PLANE_SHIFT)) | (ce << FTL_PLANE_SHIFT) | plane;

                    if (gFtlBTJob[bank]->status & (FIO_S_UNC_ERR | FIO_S_BLANK_ERR))
                    {
                        D_PRINTF_FTL("s[%2x]ce[%d]ln[%d]pl[%d]pg[%2x]\n", gFtlBTJob[bank]->status, ce, lun, plane, page);
                        //MEM_SET(gFtlL2PTbl, BYTE_MASK, sizeof(tPAA)*L2P_NUM);
						//FTL_BlockPopPushList(FTL_BLOCK_POOL_EMPTY, logicalBlk, FTL_SORT_BY_EC);
            			gFtlNbtType.existFQBT = FALSE;
                        return FALSE;
                    }
                }
            }
        }
    }

    D_PRINTF("end l2pidx[%x]\n", l2pIdx);
    return TRUE;
}

//-------------------------------------------------------------------
// Function     : U8 FTL_NANDWriteL2P(U16 logicalBlk, U16 loop, U32 type)
// Description  :
// Input        :
// return       : N/A
//-------------------------------------------------------------------
U8 FTL_NANDWriteL2P(U16 logicalBlk, U16 loop, U32 type)
{
    tPAA paa;
    U32 cacheIdx, l2pIdx = 0, idx, jobidx = 0, aux = 0, sn = 0;
    U16 page;
    U8  storeFtlStage = FALSE, bank;

    paa.all32 = 0;

    l2pIdx = FTL_GET_QBTL2PIDX(loop);
    D_PRINTF_FTL("QBT loop[%d]IDX[%x]\n", loop, l2pIdx);

    for (page = 0; page < (gNfInfo.pagesPerBlock); page++)
    {
        if ((loop == (FTL_QBT_BLOCK_COUNT - 1)) && (page >= (gNfInfo.pagesPerBlock - 2)))
        {
            storeFtlStage = TRUE;
            gFtlMgr.QbtBackup = FALSE;

            FTL_CopyFtlInfoLastPageBuf();
        }
        //D_PRINTF("QBT blk[%4x]page[%3x]store[%d]\n", logicalBlk, page, storeFtlStage);

        for (bank = 0; bank < FTL_BANK_NUM; bank++)
        {
            while ((smFioInfo.jobCnt[FIO_JOB_POOL_FREE] == 0) ||
                   (smCacheMgr.ssdCnt[SSD_POOL_OTHER] < CACHE_FLUSH_ONE_CE_CNT))
            {
                HalTimer_DelayUsec(10);
                //D_PRINTF("Wait page:%x, job:%x, ssd:%x\n", page, gFioInfo.jobCnt[FIO_JOB_POOL_FREE], smCacheMgr.ssdCnt[SSD_POOL_OTHER]);
            };

            gFtlBtSSDHead   = smCacheMgr.head[SSD_POOL_OTHER];
            cacheIdx        = GET_SSD_INDEX_FROM_PTR(gFtlBtSSDHead);

            gFtlBTJob[jobidx] = FIO_PopJob(FIO_JOB_POOL_FREE);
            gFtlBTJob[jobidx]->ssdList              = SSD_WO_CACHE;
            gFtlBTJob[jobidx]->ssdIdx[SSD_WO_CACHE] = cacheIdx;
            gFtlBTJob[jobidx]->paaCnt               = CACHE_FLUSH_ONE_CE_CNT;
            gFtlBTJob[jobidx]->cmd                  = FIO_JOB_CMD_WRITE;
            gFtlBTJob[jobidx]->css                  = CACHE_LASTPAGE;
            gFtlBTJob[jobidx]->plane                = gNfInfo.planesPerLun;
            gFtlBTJob[jobidx]->flag                 = FIO_F_NO_MKERR;
            gFtlBTJob[jobidx]->header               = FTL_BLOCK_HEADER_TABLE;
            gFtlBTJob[jobidx]->group                = 0;

            if (type == FTL_QBT_TABLE_TAG)
            {
                sn  = FTL_JOB_SN_SET(FTL_BLOCK_HEADER_TABLE, 0, gFtlQbtInfo[loop].sn);
                aux = (FTL_QBT_TABLE_TAG | loop);
            }
            else if (type == FTL_FQBT_TABLE_TAG)
            {
                sn  = FTL_JOB_SN_SET(FTL_BLOCK_HEADER_TABLE, 0, gFtlPbtInfo.sn);
                aux = (FTL_FQBT_TABLE_TAG | gFtlPbtInfo.loopCnt);
            }
            else
            {
                D_PRINTF_FTL("type[%x]\n", type);
                ASSERT(0);
            }

            paa.all32  = 0;
            paa.b.ce   = bank & FTL_CE_MASK;
            paa.b.lun  = bank >> FTL_CE_SHIFT;
            paa.b.page = page;

            for (idx = 0; idx < gFtlBTJob[jobidx]->paaCnt; idx++)
            {
                paa = FTL_NANDWriteProcess(idx, paa, page);

                paa.b.block = FTL_FastGetRMPBlk(paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, logicalBlk);

                //D_PRINTF("[%2d]ch[%d]ce[%d]ln[%d]pn[%d]bk[%4x]pg[%2x]fg[%d]\n", idx, paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, paa.b.page, paa.b.frag);

                gFastPaaBuf[idx] = paa;

                gFastAuxBuf[idx * AUX_BUF_MAX_CNT + AUX_BUF_IDX_LAA] = aux;
                gFastAuxBuf[idx * AUX_BUF_MAX_CNT + AUX_BUF_IDX_SN ] = sn;
            }

            MemCopyNU32((U32*)gFtlBTJob[jobidx]->paa, (U32*)gFastPaaBuf, gFtlBTJob[jobidx]->paaCnt * sizeof(tPAA));
            MemCopyNU32((U32*)gFtlBTJob[jobidx]->aux, (U32*)gFastAuxBuf, gFtlBTJob[jobidx]->paaCnt * META_DATA_SIZE);

            gFtlBtSSDTail = GET_SSD_PTR_FROM_INDEX(cacheIdx);

            if (storeFtlStage)
            {
                FTL_LastPageCacheList(gFtlBTJob[jobidx], paa.b.ce, paa.b.lun, FALSE);

                if (((bank & FTL_CE_MASK) == (FTL_CE_NUM - 1)) && ((bank >> FTL_CE_SHIFT) == (FTL_LUN_NUM - 1)) && (page == (gNfInfo.pagesPerBlock - 1)))
                {
                    D_PRINTF("QBT:D[%d-%d]B[%4x]P[%2x]C[%x]\n", bank & FTL_CE_MASK, bank >> FTL_CE_SHIFT, logicalBlk, page, idx);
                }
            }
            else
            {
                gFtlBTJob[jobidx]->ssdCnt[SSD_WO_CACHE] = 1;

                if (l2pIdx >= L2P_NUM)
                {
                    gFtlBtSSDTail->startAddr = smSSD[SSD_DUMMY_START].startAddr;
                    gFtlBtSSDTail->endAddr   = (gFtlBtSSDTail->startAddr + 0x1000 - 1);
                    Cache_PopSSD_Other(SSD_POOL_OTHER, gFtlBtSSDHead, gFtlBtSSDTail, gFtlBTJob[jobidx]->ssdCnt[SSD_WO_CACHE]);
                    gFtlBtSSDTail->nextIndex = SSD_DUMMY_START;
                    //D_PRINTF("QBT_DUMMY: loop:%x, pg:%x\n", loop, page);
                }
                else
                {
                    //D_PRINTF("W:idx2[%x]-ce[%d]ln[%d]pn[%d]B[%4x]pg[%2x]\n", l2pIdx, paa.b.ce, paa.b.lun, paa.b.plane, logicalBlk, paa.b.page);
                    //D_PRINTF("idx2:%d crc:0x%X\n", idx2, MemCrcU32(0xFFFFFFFF, (PVOID)&gL2PMapTbl[idx2], FTL_QBT_SIZE_PER_FLUSH/4));
                    gFtlBtSSDTail->startAddr = (U32)&gFtlL2PTbl[l2pIdx];
                    gFtlBtSSDTail->endAddr   = (gFtlBtSSDTail->startAddr + FTL_QBT_SIZE_PER_FLUSH - 1);
                    l2pIdx += FTL_LAA_BANK_PER_PAGE;
                    Cache_PopSSD_Other(SSD_POOL_OTHER, gFtlBtSSDHead, gFtlBtSSDTail, gFtlBTJob[jobidx]->ssdCnt[SSD_WO_CACHE]);
                }
            }
            Core_JobIssue(gFtlBTJob[jobidx]);
            jobidx++;

            if (jobidx == FIO_MAX_JOB)
            {
                Core_JobWait();

                for (jobidx = 0; jobidx < FIO_MAX_JOB; jobidx++)
                {
                    if (gFtlBTJob[jobidx]->status & FIO_S_PROG_ERR)
                    {
                        return FALSE;
                    }
                }

                jobidx = 0;
            }
        }
    }

    return TRUE;
}

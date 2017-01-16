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
//! @brief Error Handle
//=============================================================================
//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "SysInfo.h"
#include "Fio.h"
#include "Cache.h"
#include "Gc.h"
#include "UncTable.h"
#include "ErrHandle.h"

#include "FastSection.h"
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

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    init glist

    @param[in]  init
**/
//-----------------------------------------------------------------------------
void ErrHandle_OneTimeInit(InitBootMode_t initMode)
{
    if (smSysInfo->d.Header.d.SubHeader.d.GList.d.dwValidTag != SI_TAG_GLIST)
    {
        smSysInfo->d.Header.d.SubHeader.d.GList.d.dwValidTag = SI_TAG_GLIST;
        MEM_CLR(&smSysInfo->d.GList, sizeof(tSI_PAYLOAD_GLIST));
    }
}

//-----------------------------------------------------------------------------
/**
    mark cache error and ecc table

    @param[in]  error job
**/
//-----------------------------------------------------------------------------
void ErrHandle_MarkError(tFIO_JOB* pErrJob)
{
    uint16_t cacheIdx;
    uint16_t idx;

    switch (pErrJob->css)
    {
        case CACHE_R_IDS:
            for (idx = 0; idx < pErrJob->paaCnt; idx++)
            {
                if (FIO_CHK_ERR_BMP(pErrJob->fid, idx))
                {
                    UNCT_CHECK_DO_INSERT((pErrJob->pMsgHost->laa + pErrJob->paaOffset + idx) << HBU_PER_MBU_SFT, HBU_PER_MBU, UNCT_NDTP_MEDIA);
                }
            }
            break;
        case CACHE_R_NO_HIT:
        case CACHE_GCREAD:
        case CACHE_GCREADAUX:
        case CACHE_PREREAD:
        {
            cacheIdx = pErrJob->ssdIdx[SSD_W_CACHE];
            for (idx = 0; idx < pErrJob->paaCnt; idx++)
            {
                if (FIO_CHK_ERR_BMP(pErrJob->fid, idx))
                {
                    smCacheMgr.cacheEntry[cacheIdx].status |= CACHE_S_ERROR;
                    UNCT_CHECK_DO_INSERT(smCacheMgr.cacheEntry[cacheIdx].laa << HBU_PER_MBU_SFT, HBU_PER_MBU, UNCT_NDTP_MEDIA);
                }

                cacheIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(cacheIdx);
            }

            break;
        }
        case CACHE_R_PARTIAL_HIT:
        {
            uint16_t otherIdx;

            otherIdx = pErrJob->ssdIdx[SSD_WO_CACHE];
            for (idx = 0; idx < pErrJob->paaCnt; idx++)
            {
                if (FIO_CHK_ERR_BMP(pErrJob->fid, idx))
                {
                    cacheIdx = smSSD[otherIdx].prevIndex;
                    smCacheMgr.cacheEntry[cacheIdx].status |= CACHE_S_ERROR;
                    UNCT_CHECK_DO_INSERT(smCacheMgr.cacheEntry[cacheIdx].laa << HBU_PER_MBU_SFT, HBU_PER_MBU, UNCT_NDTP_MEDIA);
                }

                otherIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(otherIdx);
            }
            break;
        }
        case CACHE_R_FILLUP_HIT:
        case CACHE_FILLUP:
        {
            uint32_t currDataSize = 0;
            uint16_t otherIdx;

            otherIdx = pErrJob->ssdIdx[SSD_WO_CACHE];
            for (idx = 0; idx < pErrJob->paaCnt; idx++)
            {
                while (currDataSize < idx * MBU_SIZE)
                {
                    currDataSize += ((smSSD[otherIdx].endAddr + 1 - smSSD[otherIdx].startAddr) << SSD_ADDR_SHIFT);
                    otherIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(otherIdx);
                }

                while (smSSD[otherIdx].prevIndex == SSD_DUMMY_START)
                {
                    currDataSize += ((smSSD[otherIdx].endAddr + 1 - smSSD[otherIdx].startAddr) << SSD_ADDR_SHIFT);
                    otherIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(otherIdx);
                }

                cacheIdx = smSSD[otherIdx].prevIndex;
                smCacheMgr.cacheEntry[cacheIdx].status |= CACHE_S_ERROR;

                #if (UNC_TABLE_ENABLE == ENABLE)
                {
                    U64 groupMap;
                    U16 bitType;
                    U16 currGroupBits;
                    U16 checkedBits = 0;

                    groupMap = Cache_Convert_Bitmap(smCacheMgr.cacheEntry[cacheIdx].bitmap);
                    bitType  = (smCacheMgr.cacheEntry[cacheIdx].bitmap & B_0);

                    do {
                        currGroupBits = groupMap & CACHE_FIILUP_B2C_MASK;

                        if (bitType)
                        {
                            bitType = FALSE;
                        }
                        else
                        {
                            UNCT_CHECK_DO_INSERT((smCacheMgr.cacheEntry[cacheIdx].laa << HBU_PER_MBU_SFT) + checkedBits, currGroupBits, UNCT_NDTP_MEDIA);
                            bitType = TRUE;
                        }

                        groupMap   >>= CACHE_FIILUP_B2C_SHIFT;
                        checkedBits += currGroupBits;

                    } while (checkedBits < HBU_PER_MBU);
                }
                #endif
            }

            break;
        }
        case CACHE_LASTPAGE:
        case CACHE_TCG:
        case CACHE_GCLASTPAGE:
        case CACHE_SCANPAGE:
        case CACHE_AUTO:
        case CACHE_SYSINFO:
            break;
        default:
            ASSERT(0);
            break;
    }
}

//-----------------------------------------------------------------------------
/**
    mark glist and request gc to move defect block

    @param[in]  error job
**/
//-----------------------------------------------------------------------------
void ErrHandle_MarkDefect(tFIO_JOB* pJob)
{
    U32 skipRegGList = FALSE;
    tPAA  errPaa;
    U16   paaIdx;
    U16   glistCntOld;
    U16   glistCntNew;
    U16   rawBlk;
    U16   logBlk;
    U16   die;
    U16   pool;

    //-----------------------------------------------------------------------------
    glistCntOld = glistCntNew = (smSysInfo->d.GList.d.GListBrief.wCntErase +
                                 smSysInfo->d.GList.d.GListBrief.wCntWrite +
                                 smSysInfo->d.GList.d.GListBrief.wCntRead);

    paaIdx = 0;

    while (paaIdx < pJob->paaCnt)
    {
        if (FIO_CHK_ERR_BMP(pJob->fid, paaIdx))
        {
            FIO_CLR_ERR_BMP(pJob->fid, paaIdx);

            //-----------------------------------------------------------------------------
            errPaa = pJob->paa[paaIdx];
            logBlk = FTL_GetRemapBlock(errPaa.b.ch, errPaa.b.ce, errPaa.b.lun, errPaa.b.plane, errPaa.b.block);
            pool   = gFtlBlkListTbl[logBlk].vs.b.status;
            die    = FTL_GET_DIE_NUM(errPaa.b.lun, errPaa.b.ce, errPaa.b.ch);
            rawBlk = (errPaa.b.block << gNfInfo.bitsPlane) + errPaa.b.plane;

            switch (pJob->cmd)
            {
                case FIO_JOB_CMD_ERASE:
                    if (gFtlBlkListTbl[logBlk].vs.b.bad == FALSE)
                    {
                        FTL_BlockPopList(pool, logBlk);

                        if (FTL_RemapBadBlock(die, errPaa.b.plane, logBlk))
                        {
                            FTL_BlockPushList(FTL_BLOCK_POOL_FREE, logBlk, FTL_SORT_BY_EC);
                        }
                        else
                        {
                            gFtlBlkListTbl[logBlk].vs.b.bad = TRUE;

                            FTL_BlockPushList(FTL_BLOCK_POOL_BAD, logBlk, FTL_SORT_NONE);

                            #if (ENABLE_GC == TRUE)
                            GC_OverProvion();
                            #endif
                        }
                    }
                    break;
                case FIO_JOB_CMD_WRITE:
                    if (gFtlBlkListTbl[logBlk].vs.b.bad == FALSE)
                    {
                        gFtlBlkListTbl[logBlk].vs.b.bad = TRUE;

                        #if (ENABLE_GC == TRUE)
                            if (logBlk == gFtlCurrHeader[pJob->header][pJob->group].block)
                            {
                                // ### RESET current header if program error is open block ***
                                //gFtlCurrHeader[pJob->header][pJob->group].block = WORD_MASK;
                                //GC_PushBlk(logBlk, pool, TRUE);
                            }
                            else
                            {
                                //D_PRINTF("\033[7;31m--MarkDf\033[m BK|0x%x\n", logBlk);
                                GC_PushBlk(logBlk, pool, FALSE);
                            }
                        #else
                             FTL_BlockPopList(pool, logBlk);
                             FTL_BlockPushList(FTL_BLOCK_POOL_BAD, logBlk, FTL_SORT_NONE);
                        #endif
                    }
                    break;
                case FIO_JOB_CMD_READ_IDS:
                case FIO_JOB_CMD_READ:
                case FIO_JOB_CMD_READ_FAST:
                case FIO_JOB_CMD_READ_IDS_PAGE:
                {
                    U8 header = 0;
                    U8 group  = 0;

                    // ### Skip error handle for read disturb
                    if (gFtlBlkListTbl[logBlk].vs.b.rdd)
                    {
                        goto MKDF_NEXT_LOOP;
                    }

                    // ### Skip register glist for open block
                    for (group = 0; group < FTL_GROUP_NUM; group++)
                    {
                        for (header = 0; header < FTL_BLOCK_HEADER_MAX; header++)
                        {
                            if (logBlk == gFtlCurrHeader[header][group].block)
                            {
                                goto MKDF_NEXT_LOOP;
                            }
                        }
                    }

                    if (pJob->css == CACHE_GCLASTPAGE)
                    {
                        skipRegGList = TRUE;
                    }

                    //-----------------------------------------------------------------------------
                    if (gFtlBlkListTbl[logBlk].vs.b.bad == FALSE)
                    {
                        // ### Skip GC move when css is GCREAD, GCLASTPAGE or GCREADAUX
                        // ### But still need to set bad flag that gc will move this block into bad pool
                        if (pJob->css == CACHE_GCREAD || pJob->css == CACHE_GCLASTPAGE || pJob->css == CACHE_GCREADAUX)
                        {
                            gFtlBlkListTbl[logBlk].vs.b.bad = TRUE;

                            #if (GC_REMAP_MODE == TRUE)
                              GC_Set_Remap_Blk(logBlk);
                              GC_Set_Remap_Bmp(errPaa);
                            #endif
                        }
                        else
                        {
                            if ((pool == FTL_BLOCK_POOL_USER) || (pool == FTL_BLOCK_POOL_FREE))
                            {
                                gFtlBlkListTbl[logBlk].vs.b.bad = TRUE;

                                #if (ENABLE_GC == TRUE)
                                    GC_PushBlk(logBlk, pool, FALSE);
                                #else
                                    FTL_BlockPopList(pool, logBlk);
                                    FTL_BlockPushList(FTL_BLOCK_POOL_BAD, logBlk, FTL_SORT_NONE);
                                #endif
                            }
                        }
                    }

                    break;
                }
                default:
                    D_PRINTF("Cmd:%x\n", pJob->cmd);
                    ASSERT(0);
                    break;
            }

            //-----------------------------------------------------------------------------
            if (skipRegGList == FALSE && glistCntNew < SI_GLIST_MAX_COUNT)
            {
                U32  idx;

                for (idx = 0; idx < glistCntNew; idx++)
                {
                    if ((smSysInfo->d.GList.d.GList[idx].bDie == die) && (smSysInfo->d.GList.d.GList[idx].wPhyBlock == rawBlk))
                    {
                        skipRegGList = TRUE;
                        break;
                    }
                }

                if (skipRegGList == FALSE)
                {
                    /*
                    smSysInfo->d.GList.d.GList[glistCntNew].bDie      = die;
                    smSysInfo->d.GList.d.GList[glistCntNew].wPhyBlock = rawBlk;
                    smSysInfo->d.GList.d.GList[glistCntNew].bType     = GLIST_DEFECT_READ;
                    smSysInfo->d.GList.d.GListBrief.wCntRead++;
                    glistCntNew++;
                    */

                    D_PRINTF_ERH("S%2X F/PI[%3d.%3d] CSS%2d P%d [%d.%d.%d.%d.%4x.%4x.%3x]\n",
                                pJob->status,
                                pJob->fid,
                                paaIdx,
                                pJob->css,
                                pool,
                                errPaa.b.ch,
                                errPaa.b.ce,
                                errPaa.b.lun,
                                errPaa.b.plane,
                                errPaa.b.block,
                                ((errPaa.b.block << gNfInfo.bitsPlane) + errPaa.b.plane),
                                errPaa.b.page);
                }
            }
        }

MKDF_NEXT_LOOP:
        paaIdx++;
    }

    //-----------------------------------------------------------------------------
    if (glistCntNew != glistCntOld)
    {
        //SyIn_Synchronize(SI_AREA_GLIST, SYSINFO_WRITE, SI_SYNC_BY_GLIST);
        // TODO: need to update sysinfo.
    }
}

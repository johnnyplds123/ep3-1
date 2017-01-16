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
//! @brief Flash Translation Layer Manager code
//!
//=============================================================================
//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "Fio.h"
#include "Heap.h"
#include "Cache.h"
#include "SysInfo.h"
#include "Gc.h"
#include "ErrHandle.h"
#include "PLP.h"
#include "Core.h"
#include "Purge.h"

#include "Ftl.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define FTL_NBT_SAVE_FQBT(eccBlk, unsvBlk)                 ((!eccBlk) && ((unsvBlk>(FTL_PBT_PERIOD_BLK-1))))

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function proto-type definitions:
//-----------------------------------------------------------------------------
static void FTL_EraseBuildTable(tFTL_BUILD_TABLE_INFO *bt);
static void FTL_NormalBuildTable(tFTL_BUILD_TABLE_INFO *bt);
static void FTL_ScanSerialNumber(tFTL_BUILD_TABLE_INFO *bt);
static void FTL_LoadBlockList(tFTL_BUILD_TABLE_INFO *bt);
static void FTL_RebuildBlockList(tFTL_BUILD_TABLE_INFO *bt);
static void FTL_LoadL2PTable(tFTL_BUILD_TABLE_INFO *bt);
static void FTL_P2LBuildTable(tFTL_BUILD_TABLE_INFO *bt);
static void FTL_ScanOpenBlock(tFTL_BUILD_TABLE_INFO *bt);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
__align(32) tFTL_BLOCK_INFO     gFtlBlkListInfo;    // 44B
__align(32) tFTL_HEADER_INFO    gFtlCurrHeader[FTL_BLOCK_HEADER_MAX][FTL_GROUP_NUM]; // 12 * 3 * 1 = 36B
__align(32) tFTL_MANAGER        gFtlMgr;            // 32B
__align(32) tFTL_PBT_INFO       gFtlPbtInfo;        // 12B
__align(32) tFTL_RMP_TABLE      gFtlRemap;          // 8B
__align(32) tFTL_LAST_PAGE_BUFF gFtlLastPageBuf;    // 23B
__align(32) tFTL_TRIM_INFO      gFtlTrimInfo;       // 24B
__align(32) tFTL_NBT_TYPE       gFtlNbtType;        // 4B
__align(32) tFTL_POR_PBT_INFO   gFtlPorPbtInfo;     // 12B
__align(32) tFTL_NAND_INFO      gFtlNandInfo;       // 16B
__align(32) tFTL_SPOR_INFO      gFtlSporInfo;       // 4B

tFIO_JOB**                      gFtlBTJob;          //max_ce * max_plane
tFTL_BLOCK_NODE*                gFtlBlkListTbl;
tFTL_BLOCK_NODE*                gFtlPBTBlkListTbl;
tFTL_QBT_INFO*                  gFtlQbtInfo;
tSSD*                           gFtlBtSSDHead;
tSSD*                           gFtlBtSSDTail;
tPAA*                           gFtlL2PTbl;
U32*                            gFtlFastVacBuf;
U32*                            gFtlQbtSn; // 0: QBT(or FQBT) block, 1: serial number
U32*                            gFtlSNTable;
U32*                            gFtlHeadReadCnt;
U16*                            gFtlFreeIdx;
U8*                             gFtlNbtEmptyBlk;
U8*                             gFtlLastPageP2LIdx;

U32                             gFtlLastPageP2LSize;
U32                             gFtlTrimRatio;          // FTL trim laa count threshold to start update trim bitmap
U8                              gFtlPowerOn;
U32                             gExtLaaStart;

static void (*gCbfBuildTableState[FTL_BUILD_TABLE_COUNT])(tFTL_BUILD_TABLE_INFO *bt) =
{
    FTL_EraseBuildTable,
    FTL_NormalBuildTable,
    FTL_ScanSerialNumber,
    FTL_LoadBlockList,
    FTL_RebuildBlockList,
    FTL_LoadL2PTable,
    FTL_P2LBuildTable,
    FTL_ScanOpenBlock
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
//-------------------------------------------------------------------
// Function     : void FTL_GlobalVarAlloc(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_GlobalVarAlloc(void)
{
    U32 auPerBlock, lpbufsz;

    //gFtlBTJob               = (tFIO_JOB**)MEM_AllocBuffer(sizeof(tFIO_JOB*) * MAX_SUPPORTED_DEVICES * MAX_SUPPORTED_LUNS * MAX_SUPPORTED_PLANES, 512);
    gFtlBTJob               = (tFIO_JOB**)MEM_AllocBuffer(sizeof(tFIO_JOB*) * FIO_MAX_JOB, 512);

    gFtlBlkListTbl          = (tFTL_BLOCK_NODE*)MEM_AllocDtcmDram(sizeof(tFTL_BLOCK_NODE) * FTL_SUPPORT_MAX_BLK, 512);
    gFtlQbtInfo             = (tFTL_QBT_INFO*)MEM_AllocDtcmDram(sizeof(tFTL_QBT_INFO) * FTL_MAX_QBT_CNT, 512);

    //L2P Table
    D_PRINTF("L2P_NUM_RAW[%x]+L2P_EXT_NUM_TOTAL[%x]=[%x] -> L2P_NUM[%x]\n", L2P_NUM_RAW, L2P_EXT_NUM_TOTAL, L2P_NUM_RAW+L2P_EXT_NUM_TOTAL, L2P_NUM);
    gFtlL2PTbl              = (tPAA*)MEM_AllocBuffer(sizeof(tPAA)*L2P_NUM, 512);

    gFtlFastVacBuf          = (U32*)MEM_AllocDtcmDram(sizeof(U32) * FTL_SUPPORT_MAX_BLK, 32);
    gFtlQbtSn               = (U32*)MEM_AllocDtcmDram(sizeof(U32) * 2, 32);

    //Remap
    gFtlRemap.rmpSuperBlkTbl= (U16*)MEM_AllocBuffer(sizeof(U16) * gNfInfo.blocksPerLun * gNfInfo.totalLun, 512);
    gFtlRemap.rmpBlkBMP     = (U8* )MEM_AllocDtcmDram(sizeof(U8) * FTL_SUPPORT_MAX_BLK, 32);

    //Last Page Buffer: P2L
    auPerBlock              = 1 << (gFtlNandInfo.frag_shift + gFtlNandInfo.plane_shift + gFtlNandInfo.pages_shift);
    //auPerBlock              = gEcuInfo.auPerPage * gNfInfo.planesPerLun * gNfInfo.pagesPerBlock; //4096
    gFtlLastPageP2LSize     = auPerBlock * gNfInfo.totalLun * sizeof(U32); // 131072 Bytes

    //gFtlLastPageBuf.p2lBuf  = (U32*)MEM_AllocBuffer(gFtlLastPageP2LSize, 512);
    //MEM_SET(gFtlLastPageBuf.p2lBuf, BYTE_MASK, gFtlLastPageP2LSize);

    lpbufsz = gFtlLastPageP2LSize * LP_P2L_PP_SIZE * (FTL_GROUP_NUM * (FTL_BLOCK_HEADER_HOST + 1)); // 524288 Bytes
    D_PRINTF("size-p2lBuf[%x]lastPageBufFtlP2L[%x]lastPageBufFtlInfo[%x]\n", gFtlLastPageP2LSize, lpbufsz, LP_FTL_INFO_SIZE);
    gFtlLastPageBuf.lastPageBufFtlP2L = (U32*)MEM_AllocBuffer(lpbufsz, 512);
    MEM_SET(gFtlLastPageBuf.lastPageBufFtlP2L, BYTE_MASK, lpbufsz);

    //Last Page Buffer: Ftl info
    gFtlLastPageBuf.lastPageBufFtlInfo = (U8*)MEM_AllocBuffer(LP_FTL_INFO_SIZE, 512);//32KB
    ASSERT(LP_FTL_INFO_SIZE >= LP_SIZE_MAX);

    gFtlNbtEmptyBlk = (U8*)MEM_AllocBuffer(FTL_SUPPORT_MAX_BLK, 32);

    //Serial Number
    gFtlSNTable = (U32*)MEM_AllocBuffer(sizeof(U32) * FTL_SUPPORT_MAX_BLK, 512);

    //NBT get vac of PBT
    gFtlPBTBlkListTbl = (tFTL_BLOCK_NODE*)MEM_AllocBuffer(sizeof(tFTL_BLOCK_NODE) * FTL_SUPPORT_MAX_BLK, 16);

    //Erase build remap
    gFtlFreeIdx = (U16*)MEM_AllocBuffer(sizeof(U16) * gNfInfo.totalLun * gNfInfo.planesPerLun, 32);

    //Trim Table
    gFtlTrimInfo.trimBMP    = (U32*)MEM_AllocBuffer((L2P_NUM >> 5) * sizeof(U32), 32);
    gFtlTrimInfo.trimPbtPG  = (U32*)MEM_AllocBuffer((((gNfInfo.pagesPerBlock << FTL_BANK_SHIFT) * FTL_QBT_BLOCK_COUNT) >> 5) * sizeof(U32), 32);
    gFtlTrimInfo.trimBlocks = (U8*)MEM_AllocBuffer(sizeof(U8)*FTL_SUPPORT_MAX_BLK, 32);

    #if (CO_SUPPORT_EARLY_MOVE_BLK == TRUE)
    gFtlHeadReadCnt         = (U32*)MEM_AllocBuffer(sizeof(U32)*gNfInfo.totalLun*gNfInfo.blocksPerLun, 16);
    #endif

    gFtlPorPbtInfo.zoneBMP  = (U8*)MEM_AllocDtcmDram(sizeof(U8) * FTL_POR_PBT_ZONE_NUM, 16);

    gFtlLastPageP2LIdx = (U8*)MEM_AllocDtcmDram(sizeof(U8) * FTL_CH_NUM, 32);
}

//-------------------------------------------------------------------
// Function     : void FTL_NandInfoInit(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_NandInfoInit(void)
{
    U16 pages;

    gFtlNandInfo.ch_shift       = gNfInfo.bitsCh;
    gFtlNandInfo.ch_mask        = gNfInfo.numChannels - 1;

    gFtlNandInfo.ce_shift       = gNfInfo.bitsDevice;
    gFtlNandInfo.ce_mask        = gNfInfo.devicesPerChannel - 1;

    gFtlNandInfo.lun_shift      = gNfInfo.bitsLun;
    gFtlNandInfo.lun_mask       = gNfInfo.lunsPerDevice- 1;

    gFtlNandInfo.bank_shift     = gNfInfo.bitsDevice + gNfInfo.bitsLun;
    gFtlNandInfo.bank_num       = (1 << gFtlNandInfo.bank_shift);
    gFtlNandInfo.bank_mask      = gNfInfo.devicesPerChannel - gNfInfo.lunsPerDevice - 1;

    gFtlNandInfo.bank_bits_mask = ((U32)1 << FTL_BANK_NUM) - 1;

    gFtlNandInfo.die_shift      = LOG2(gNfInfo.totalLun);

    gFtlNandInfo.plane_shift    = gNfInfo.bitsPlane;
    gFtlNandInfo.plane_mask     = gNfInfo.planesPerLun - 1;

    gFtlNandInfo.frag_shift     = LOG2(gEcuInfo.auPerPage);
    gFtlNandInfo.frag_mask      = gEcuInfo.auPerPage - 1;

    pages = 1 << LOG2(gNfInfo.pagesPerBlock);
    gFtlNandInfo.pages_shift    = (gNfInfo.pagesPerBlock > pages) ? (LOG2(gNfInfo.pagesPerBlock) + 1) : LOG2(gNfInfo.pagesPerBlock);

    gFtlNandInfo.max_valid_count    = (U32)(gEcuInfo.auPerPage * gNfInfo.pagesPerBlock * gNfInfo.planesPerLun * gNfInfo.totalLun);
}

//-------------------------------------------------------------------
// Function     : void FTL_VarInit(U8 initial)
// Description  :
// Input        : U8 flag = TRUE:eraseCnt reset, FALSE:eraseCnt keep
// return       : N/A
//-------------------------------------------------------------------
void FTL_GlobalVarInit(U8 initial)
{
    U32     ec = 0;
    U16     blk, idx, group;

    if (initial == FTL_INITIAL_POWER_ON)
    {
        //Block List Table
        for (blk = 0; blk < FTL_SUPPORT_MAX_BLK; blk++)
        {
            if (blk == FTL_SUPPORT_MAX_BLK - 1)
                gFtlBlkListTbl[blk].block    = WORD_MASK;
            else
                gFtlBlkListTbl[blk].block    = blk + 1;

            gFtlBlkListTbl[blk].vs.b.status  = FTL_BLOCK_POOL_FREE;
            gFtlBlkListTbl[blk].vs.b.vac     = 0;
            gFtlBlkListTbl[blk].vs.b.host    = 0;
            gFtlBlkListTbl[blk].vs.b.bad     = 0;
            gFtlBlkListTbl[blk].vs.b.rdd     = 0;

            if (smSysInfo->d.Header.d.SubHeader.d.ECnt.d.dwValidTag != SI_TAG_ECNT)
            {
                gFtlBlkListTbl[blk].eraseCnt     = 0;
            }
            else
            {
                gFtlBlkListTbl[blk].eraseCnt     = smSysInfo->d.ECnt.d.EraseCount[blk];
                ec                              += smSysInfo->d.ECnt.d.EraseCount[blk];
            }

            gFtlPBTBlkListTbl[blk].block        = WORD_MASK;
            gFtlPBTBlkListTbl[blk].vs.b.status  = FTL_BLOCK_POOL_FREE;
            gFtlPBTBlkListTbl[blk].vs.b.vac     = 0;
            gFtlPBTBlkListTbl[blk].vs.b.host    = 0;
            gFtlPBTBlkListTbl[blk].vs.b.bad     = 0;
            gFtlPBTBlkListTbl[blk].eraseCnt     = 0;
        }

        for (idx = 0; idx < FTL_BLOCK_POOL_MAX; idx++)
        {
            gFtlBlkListInfo.head[idx]   = WORD_MASK;
            gFtlBlkListInfo.tail[idx]   = WORD_MASK;
            gFtlBlkListInfo.blkCnt[idx] = 0;
        }

        //Header Info
        for (idx = 0; idx < FTL_BLOCK_HEADER_MAX; idx++)
        {
            for (group = 0; group < FTL_GROUP_NUM; group++)
            {
                FTL_ClearHeaderBlock(idx,group);
            }
        }

        //Super Block Table
        MEM_SET(gFtlRemap.rmpSuperBlkTbl, BYTE_MASK, sizeof(U16) * gNfInfo.blocksPerLun * gNfInfo.totalLun);
        //Remap Bitmap
        MEM_CLR(gFtlRemap.rmpBlkBMP, sizeof(U8)*FTL_SUPPORT_MAX_BLK);

        //Manager Info
        MEM_CLR(&gFtlMgr, sizeof(tFTL_MANAGER));
        gFtlMgr.QbtBackup      = TRUE;
        gFtlMgr.AllEraseCnt    = ec;
        gFtlMgr.SerialNumber   = 1;
        gFtlMgr.ReleaseBlk     = FALSE;
        gFtlMgr.ReleaseGCBlk   = FALSE;
        gFtlMgr.flushDummy     = FALSE;
        gFtlMgr.TableSN        = 0;

        //Zero PAA
        gExtLaaStart        = L2P_EXT_ZERO_START;

        //FTL initial
        gFtlTrimRatio       = 0;
        gFtlPowerOn         = FALSE;

        //Remap Extern Index
        MEM_CLR(gFtlFreeIdx, sizeof(U16) * gNfInfo.totalLun * gNfInfo.planesPerLun);

        //SPOR info
        MEM_CLR(&gFtlSporInfo, sizeof(gFtlSporInfo));
    }
    else if (initial == FTL_INITIAL_PREFORMAT)
    {
        gFtlMgr.SerialNumber   = 1;
        gFtlMgr.ReleaseBlk     = FALSE;
        gFtlMgr.ReleaseGCBlk   = FALSE;
        gFtlMgr.flushDummy     = FALSE;
        gFtlMgr.TableSN        = 0;

        gFtlPowerOn            = FALSE;
    }
    else
    {
        D_PRINTF("What do you mean? A joke? Initial[%d]\n", initial);
        ASSERT(0);
    }

    //QBT Info
    MEM_SET(gFtlQbtInfo, BYTE_MASK, sizeof(tFTL_QBT_INFO) * FTL_MAX_QBT_CNT);

    //PBT Info
    MEM_CLR(&gFtlPbtInfo, sizeof(tFTL_PBT_INFO));
    gFtlPbtInfo.block = WORD_MASK;

    //Fast copy L2P
    MEM_CLR(gFtlFastVacBuf, sizeof(U32) * FTL_SUPPORT_MAX_BLK);

    //SN scan type
    MEM_CLR(gFtlNbtEmptyBlk, sizeof(U8)*FTL_SUPPORT_MAX_BLK);

    //FQBT block SN
    MEM_SET(gFtlQbtSn, BYTE_MASK, sizeof(U32) * 2);

    //Serial Number Table
    MEM_SET(gFtlSNTable, BYTE_MASK, sizeof(U32) * FTL_SUPPORT_MAX_BLK);

    //Trim Table
    //Trim Bitmap unit:LAA
    MEM_CLR(gFtlTrimInfo.trimBMP, (L2P_NUM >> 5) * sizeof(U32));

    //Need to update Trim L2P unit:die
    MEM_CLR(gFtlTrimInfo.trimPbtPG, (((gNfInfo.pagesPerBlock << FTL_BANK_SHIFT) * FTL_QBT_BLOCK_COUNT) >> 5) * sizeof(U32));

    //UnsaveBlk Bitmap
    MEM_CLR(gFtlTrimInfo.trimBlocks, sizeof(U8)*FTL_SUPPORT_MAX_BLK);
    gFtlTrimInfo.trimBMPcnt         = 0;
    gFtlTrimInfo.trimBlockcnt       = 0;
    gFtlTrimInfo.trimSkipBlockcnt   = 0;
    gFtlTrimInfo.trimUpdate         = FALSE;

    //POR after PBT
    gFtlPorPbtInfo.finishBuild  = !(FTL_PARTIAL_LOAD_L2P);
    gFtlPorPbtInfo.zoneCnt      = 0;
    MEM_CLR(gFtlPorPbtInfo.zoneBMP, sizeof(U8) * FTL_POR_PBT_ZONE_NUM);

#if (CO_SUPPORT_EARLY_MOVE_BLK == TRUE)
    MEM_CLR(gFtlHeadReadCnt, sizeof(U32) * gNfInfo.totalLun * gNfInfo.blocksPerLun);
#endif
}

//-------------------------------------------------------------------
// Function     : U8 FTL_GetVacFromBlock(U16 block)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
U8 FTL_GetVacFromBlock(U16 block)
{
    U16 blk;
    U8  exist = TRUE, readaux = FTL_SCAN_PAGE_NONE;

    //==========load last page of PBT block to get VAC ====///
    //blk = gFtlBlkListInfo.head[FTL_BUILD_TYPE_PBT];

    if (gFtlNbtEmptyBlk[block] == FTL_SN_TYPE_EMPTY)
    {
        readaux = FTL_SCAN_PAGE_AUX;
        D_PRINTF_FTL("GetB[%4x]vac AUX\n", block);
    }
    else
    {
        D_PRINTF_FTL("GetB[%4x]vac\n", block);
    }

    gFtlBTJob[0] = FTL_NANDReadPageBuild(CACHE_LASTPAGE, 0,
        FTL_CE_NUM - 1, FTL_LUN_NUM - 1, 1, block, FTL_NAND_FTL_INFO_PAGE,
        readaux, FALSE);

    Core_JobWait();

    if (gFtlBTJob[0]->status & (FIO_S_UNC_ERR | FIO_S_BLANK_ERR))
    {
        if (gFtlBTJob[0]->status & FIO_S_BLANK_ERR)
        {
            D_PRINTF_FTL("Vac etyB[%4x]\n", block);
        }
        else
        {
            D_PRINTF_FTL("PBT readFails[%x]\n",gFtlBTJob[0]->status);
        }

        MEM_SET(gFtlL2PTbl, BYTE_MASK, sizeof(tPAA)*L2P_NUM);
        exist                   = FALSE;
        gFtlNbtType.existFQBT   = FALSE;
    }
    else
    {
        if (gFtlNbtEmptyBlk[block] == FTL_SN_TYPE_EMPTY)
        {
            U32 aux[AUX_BUF_MAX_CNT] = {0};

            MemCopy2U32(aux, &gFtlBTJob[0]->aux[(gFtlBTJob[0]->paaCnt - 1) << 1]);

            if ((aux[AUX_BUF_IDX_SN] >> FTL_META_HEADER_SHIFT) == FTL_BLOCK_HEADER_TABLE)
            {
                if ((aux[AUX_BUF_IDX_LAA] & ~FTL_TABLE_TAG_MASK) == FTL_PBT_TABLE_TAG)
                {
                    gFtlNbtEmptyBlk[block] = FTL_SN_TYPE_PBT;
                }
                else if ((aux[AUX_BUF_IDX_LAA] & ~BYTE_MASK) == FTL_FQBT_TABLE_TAG)
                {
                    gFtlNbtEmptyBlk[block] = FTL_SN_TYPE_FQBT;
                }
                else if ((aux[AUX_BUF_IDX_LAA] & ~BYTE_MASK) == FTL_QBT_TABLE_TAG)
                {
                    gFtlNbtEmptyBlk[block] = FTL_SN_TYPE_QBT;
                }
                else
                {
                    D_PRINTF("auxErr[SN]:%x[LAA]%x\n", aux[AUX_BUF_IDX_SN], aux[AUX_BUF_IDX_LAA]);
                    MEM_SET(gFtlL2PTbl, BYTE_MASK, sizeof(tPAA)*L2P_NUM);
                    exist                   = FALSE;
                    gFtlNbtType.existFQBT   = FALSE;
                }
            }
        }
    }

    if (exist)
    {
        MemCopyNU32((U32*)gFtlPBTBlkListTbl,   (U32*)&gFtlLastPageBuf.lastPageBufFtlInfo[LP_OFFSET_BLKTBL], LP_SIZE_BLKTBL);

        for (blk=smSysInfo->d.FtlData.d.StartIdx; blk < (smSysInfo->d.FtlData.d.EndIdx + 1); blk++)
        {
            if (gFtlBlkListTbl[blk].vs.b.vac != gFtlPBTBlkListTbl[blk].vs.b.vac)
            {
                //D_PRINTF("B[%4x]vc[%x]->vc[%x]\n", blk, gFtlBlkListTbl[blk].vs.b.vac, gFtlPBTBlkListTbl[blk].vs.b.vac);
                gFtlBlkListTbl[blk].vs.b.vac = gFtlPBTBlkListTbl[blk].vs.b.vac;
            }
        }
    }
    return (exist);
}

//-------------------------------------------------------------------
// Function     : static void FTL_ScanSerialNumber(tFTL_BUILD_TABLE_INFO *bt)
// Description  : FTL get sn of last page of all blocks
// Input        : tFTL_BUILD_TABLE_INFO *bt
// return       : N/A
//-------------------------------------------------------------------
static void FTL_ScanSerialNumber(tFTL_BUILD_TABLE_INFO *bt)
{
    tPAA paa, errPaa;
    U32  aux[AUX_BUF_MAX_CNT] = {0};
    U32  currTick;
    U32  maxSn  = DWORD_MASK;
    U32  maxPBTSn  = DWORD_MASK;
    U16  maxPBTBlk  = WORD_MASK;
    U16  logicBlk = WORD_MASK;
    U16  blk    = 0;
    U16  page;
    U8   header, rrB = FALSE;

    currTick = HalTimer_ReadTick();
    D_PRINTF_FTL("NBT1\n");

    MEM_SET(gFtlL2PTbl, BYTE_MASK, sizeof(tPAA)*L2P_NUM);

    for (blk = smSysInfo->d.FtlData.d.StartIdx; blk < (smSysInfo->d.FtlData.d.EndIdx + 1); blk++)
    {
        page = FTL_NAND_FTL_INFO_PAGE;

        gFtlBTJob[0] = FTL_NANDReadPageBuild(CACHE_SCANPAGE, 0,
            gNfInfo.devicesPerChannel - 1,
            gNfInfo.lunsPerDevice - 1,
            gNfInfo.planesPerLun - 1,
            blk,
            page,
            FTL_SCAN_PAGE_AUX, //read aux
            FALSE);

        Core_JobWait();

        while (gFtlBTJob[0]->status & FIO_S_UNC_ERR)
        {
            page--;

            D_PRINTF_FTL("nbt1-b[%4x]PG[%3x]unc RR\n", blk, page);

            gFtlBTJob[0] = FTL_NANDReadPageBuild(CACHE_SCANPAGE, 0,
            gNfInfo.devicesPerChannel - 1,
            gNfInfo.lunsPerDevice - 1,
            gNfInfo.planesPerLun - 1,
            blk,
            page,
            FTL_SCAN_PAGE_AUX, //read aux
            FALSE);
            Core_JobWait();
            //FIO_RST_ERR_BMP(gFtlBTJob[0]->fid);

            if (page == 0) break;
            if (rrB != TRUE) rrB = TRUE;
        }

        //check read fail here
        if (gFtlBTJob[0]->status & FIO_S_BLANK_ERR)
        {
            //  empty ==> nothing to do
            D_PRINTF_FTL("nbt1ety:b[%4x]\n", blk);
            gFtlNbtEmptyBlk[blk] = FTL_SN_TYPE_EMPTY;
            //FIO_RST_ERR_BMP(gFtlBTJob[0]->fid);
        }
        else if (gFtlBTJob[0]->status & FIO_S_UNC_ERR)
        {
            D_PRINTF_FTL("nbt1unc:b[%4x]\n", blk);
            gFtlNbtEmptyBlk[blk] = FTL_SN_TYPE_EMPTY;
            //FIO_RST_ERR_BMP(gFtlBTJob[0]->fid);
        }
        else
        {
            MemCopy2U32(aux, &gFtlBTJob[0]->aux[(gFtlBTJob[0]->paaCnt - 1) << 1]);

            //  not empty ==> build serial number table
            //group = (aux[AUX_BUF_IDX_LAA] >> FTL_LAA2GROUP_SHIFT) & FTL_GROUP_MASK;
            gFtlSNTable[blk] = aux[AUX_BUF_IDX_SN] & FTL_META_SN_MASK;
            header = (aux[AUX_BUF_IDX_SN] >> FTL_META_HEADER_SHIFT);

            // User block
            if ((header < FTL_BLOCK_HEADER_TABLE) &&
                ((rrB != FALSE) || (aux[AUX_BUF_IDX_LAA] == FTL_LAST_PAGE_TAG)))
            {
                D_PRINTF_FTL("nbt1usr:b[%4x]s[%x]\n", blk, gFtlSNTable[blk]);
                if ((gFtlSNTable[blk] > maxSn) || (maxSn == DWORD_MASK))
                {
                    maxSn = gFtlSNTable[blk];
                    logicBlk = blk;
                }
                gFtlNbtEmptyBlk[blk] = FTL_SN_TYPE_USER;
            }
            // PBT block
            else if ((header == FTL_BLOCK_HEADER_TABLE) &&
                ((aux[AUX_BUF_IDX_LAA] & ~FTL_TABLE_TAG_MASK) == FTL_PBT_TABLE_TAG))
            {
                D_PRINTF_FTL("nbt1pbt:b[%4x]t[%x]\n", blk, aux[AUX_BUF_IDX_SN] & FTL_META_SN_MASK);
                gFtlSNTable[blk] = gFtlPbtInfo.sn = aux[AUX_BUF_IDX_SN] & FTL_META_SN_MASK;

                /*if ((maxPBTSn == DWORD_MASK) || (gFtlPbtInfo.sn > maxPBTSn))
                {
                    maxPBTSn = gFtlPbtInfo.sn;
                    maxPBTBlk = blk;
                }*/
                gFtlNbtEmptyBlk[blk]  = FTL_SN_TYPE_PBT;
            }
            // QBT block
            else if ((header == FTL_BLOCK_HEADER_TABLE) &&
                (((aux[AUX_BUF_IDX_LAA] & ~FTL_TABLE_TAG_MASK) == FTL_QBT_TABLE_TAG) ||
                ((aux[AUX_BUF_IDX_LAA] & ~FTL_TABLE_TAG_MASK) == FTL_FQBT_TABLE_TAG)))
            {
                U16 loop = (aux[AUX_BUF_IDX_LAA] & FTL_TABLE_TAG_MASK);

                if ((aux[AUX_BUF_IDX_LAA] & ~BYTE_MASK) == FTL_FQBT_TABLE_TAG)
                {
                    gFtlNbtEmptyBlk[blk] = FTL_SN_TYPE_FQBT;
                    loop = ((loop - 1) % FTL_QBT_BLOCK_COUNT);
                }
                else
                {
                    gFtlNbtEmptyBlk[blk] = FTL_SN_TYPE_QBT;
                    loop = (loop % FTL_QBT_BLOCK_COUNT);
                }

                if ((loop == (FTL_QBT_BLOCK_COUNT - 1)) && (((aux[AUX_BUF_IDX_SN] & FTL_META_SN_MASK) > gFtlQbtSn[1]) || (gFtlQbtSn[1] == DWORD_MASK)) && (bt->readBlistBlock != blk))
                {
                    gFtlQbtSn[0] = blk;
                    gFtlQbtSn[1] = (aux[AUX_BUF_IDX_SN] & FTL_META_SN_MASK);
                }

                gFtlSNTable[blk] = gFtlPbtInfo.sn = aux[AUX_BUF_IDX_SN] & FTL_META_SN_MASK;

                D_PRINTF_FTL("nbt1qbt:b[%4x]t[%x]\n", blk, aux[AUX_BUF_IDX_SN] & FTL_META_SN_MASK);
            }
            else
            {
                U32 idx=0;
                gFtlSNTable[blk]        = 0; //Don't update L2P in NBT_3.
                gFtlNbtEmptyBlk[blk]    = FTL_SN_TYPE_MAX;
                D_PRINTF_FTL("nbt1auxEr:b[%4x]s[%x]laa[%x]\n", blk, aux[AUX_BUF_IDX_SN], aux[AUX_BUF_IDX_LAA]);
                for (idx = 0; idx < gFtlBTJob[0]->paaCnt; idx++)
                {
                    D_PRINTF("ch[%d]ce[%d]ln[%d]pl[%d]bk[%4x]pg[%3x]fg[%d]-LAA[%x]SN[%x]\n",
                        gFtlBTJob[0]->paa[idx].b.ch, gFtlBTJob[0]->paa[idx].b.ce, gFtlBTJob[0]->paa[idx].b.lun,
                        gFtlBTJob[0]->paa[idx].b.plane, gFtlBTJob[0]->paa[idx].b.block, gFtlBTJob[0]->paa[idx].b.page, gFtlBTJob[0]->paa[idx].b.frag,
                        gFtlBTJob[0]->aux[idx<<1], gFtlBTJob[0]->aux[(idx<<1)+1]);
                }
            }
        }
    }

    if (maxSn == DWORD_MASK)
    {
        D_PRINTF_FTL("BkT n ex\n");

        gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE]   = smSysInfo->d.FtlData.d.StartIdx;
        gFtlBlkListInfo.tail[FTL_BLOCK_POOL_FREE]   = smSysInfo->d.FtlData.d.EndIdx;
        gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] = gFtlBlkListInfo.tail[FTL_BLOCK_POOL_FREE] - gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE] + 1;
        gFtlBlkListTbl[gFtlBlkListInfo.tail[FTL_BLOCK_POOL_FREE]].block = WORD_MASK;

        bt->buildState       = FTL_BUILD_TABLE_READ_OPENBK;
        bt->prevState        = FTL_BUILD_TABLE_READ_SN;
        bt->readBlistBlock   = WORD_MASK;

        currTick = HalTimer_GetElapsedTimeMsec(currTick);
        D_PRINTF_FTL("NBT1 T[%d]ms\n", currTick);

        return;
    }

    D_PRINTF("sn[%x],qbtsn[%x],bk[%4x],PBTBlk[%4x]\n", maxSn, gFtlQbtSn[1], logicBlk, maxPBTBlk);

    if (gFtlQbtSn[1] != DWORD_MASK)
    {
        if ((maxSn < gFtlQbtSn[1]) && (gFtlQbtInfo[FTL_QBT_BLOCK_COUNT - 1].block == WORD_MASK) && (gFtlQbtInfo[FTL_QBT_BLOCK_COUNT - 1].sn == DWORD_MASK)) // FQBT for fast build from SPOR
        {
            logicBlk = gFtlQbtSn[0];
            gFtlNbtType.existQBT = TRUE;
            D_PRINTF_FTL("Us B[%4x]QBT\n", logicBlk);
        }
    }

    bt->prevState        = FTL_BUILD_TABLE_READ_SN;
    bt->buildState       = FTL_BUILD_TABLE_READ_BLIST;
    bt->readBlistBlock   = logicBlk;

    currTick = HalTimer_GetElapsedTimeMsec(currTick);
    D_PRINTF_FTL("NBT1 T[%d]ms\n", currTick);

    return; // lastest block
}

//-------------------------------------------------------------------
// Function     : static void FTL_LoadBlockList(tFTL_BUILD_TABLE_INFO *bt)
// Description  : FTL read block list info
// Input        : tFTL_BUILD_TABLE_INFO *bt
// return       : N/A
//-------------------------------------------------------------------
static void FTL_LoadBlockList(tFTL_BUILD_TABLE_INFO *bt)
{
    U32 currTick, sn = 0;
    U16 blk = WORD_MASK;

    currTick = HalTimer_ReadTick();

    D_PRINTF_FTL("NBT2:B[%4X]qEx[%d]pEx[%d]\n", bt->readBlistBlock, gFtlNbtType.existQBT, gFtlNbtType.existPBT);//load FTL block table

    gFtlBTJob[0] = FTL_NANDReadPageBuild(CACHE_SCANPAGE, 0,
        FTL_CE_NUM - 1,
        FTL_LUN_NUM - 1,
        1,
        bt->readBlistBlock,
        FTL_NAND_FTL_INFO_PAGE,
        FTL_SCAN_PAGE_NONE,
        FALSE);

    Core_JobWait();

    //check read fail here
    if ((gFtlBTJob[0]->status & FIO_S_BLANK_ERR) || (gFtlBTJob[0]->status & FIO_S_UNC_ERR))
    {
        D_PRINTF_FTL("nbt2 ety:B[%4x]s[%x]\n", bt->readBlistBlock, gFtlBTJob[0]->status);

        if (gFtlNbtType.existQBT)
        {
            gFtlNbtType.existQBT = FALSE;

            if (bt->prevState != FTL_BUILD_TABLE_READ_SN)
            {
                D_PRINTF("BTS[%d]goto[%d]\n", bt->buildState, FTL_BUILD_TABLE_READ_SN);

                bt->prevState        = bt->buildState;
                bt->buildState       = FTL_BUILD_TABLE_READ_SN;
            }
            else
            {
                sn = gFtlSNTable[bt->readBlistBlock];

                if (sn != DWORD_MASK)  //search previous block.
                {
                    for (blk = smSysInfo->d.FtlData.d.StartIdx; blk < (smSysInfo->d.FtlData.d.EndIdx + 1); blk++)
                    {
                        if ((sn - 1) == gFtlSNTable[blk])
                        {
                            bt->prevState        = bt->buildState;
                            bt->buildState       = FTL_BUILD_TABLE_READ_BLIST;
                            bt->readBlistBlock   = blk;
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            bt->prevState        = bt->buildState;
            bt->buildState       = FTL_BUILD_TABLE_REBUILD_BLIST;
            bt->readBlistBlock   = WORD_MASK;
        }
    }
    else
    {
        //D_PRINTF_FTL("nbt2 load FTL info blk:0x%4x\n", blk);

        MemCopyNU32((U32*)gFtlBlkListTbl,      (U32*)&gFtlLastPageBuf.lastPageBufFtlInfo[LP_OFFSET_BLKTBL],          LP_SIZE_BLKTBL);
        MemCopyNU32((U32*)&gFtlBlkListInfo,    (U32*)&gFtlLastPageBuf.lastPageBufFtlInfo[LP_OFFSET_BLKINFO],         LP_SIZE_BLKINFO);
        MemCopyNU32((U32*)gFtlCurrHeader,      (U32*)&gFtlLastPageBuf.lastPageBufFtlInfo[LP_OFFSET_HEADINFO],        LP_SIZE_HEADINFO);
        MemCopyNU32((U32*)&gFtlMgr,            (U32*)&gFtlLastPageBuf.lastPageBufFtlInfo[LP_OFFSET_MGRVAR],          LP_SIZE_MGRVAR);

        D_PRINTF("gQbtSn[0]:%4x BTprev[%d]\n", gFtlQbtSn[0], bt->prevState);

        if ((gFtlNbtType.existQBT == TRUE) && (bt->prevState == FTL_BUILD_TABLE_NORMAL))
        {
            MemCopyNU32((U32*)gFtlSNTable, (U32*)&gFtlLastPageBuf.lastPageBufFtlInfo[LP_OFFSET_SERIAL_NUMBER],   LP_SIZE_SERIAL_NUMBER);
        }

        //-------------------------------------------------------------------------------
        //--------------------

        while (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_GC])
        {
            FTL_BlockPopPushList(FTL_BLOCK_POOL_USER, gFtlBlkListInfo.head[FTL_BLOCK_POOL_GC], FTL_SORT_BY_SN);
        }

        while (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_GCD])
        {
            FTL_BlockPopPushList(FTL_BLOCK_POOL_USER, gFtlBlkListInfo.head[FTL_BLOCK_POOL_GCD], FTL_SORT_BY_SN);
        }

        while (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_EMPTY])
        {
            FTL_BlockPopPushList(FTL_BLOCK_POOL_USER, gFtlBlkListInfo.head[FTL_BLOCK_POOL_EMPTY], FTL_SORT_BY_SN);
        }
    }

    blk = WORD_MASK;

    if ((gFtlNbtType.existQBT == TRUE) && (gFtlQbtSn[0] == DWORD_MASK) && (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_QBT] < FTL_QBT_BLOCK_COUNT))
    {
        FTL_PurgePool2Free(FTL_BLOCK_POOL_QBT);
        gFtlNbtType.existQBT = FALSE;
    }

    // -----------------------------------
    // Drop PBT Block
    D_PRINTF_FTL("PBTcnt[%d]tail[%4x]\n", gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT], gFtlBlkListInfo.tail[FTL_BLOCK_POOL_PBT]);
    if (gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].block != WORD_MASK)
    {
        FTL_BlockPushList(FTL_BLOCK_POOL_FREE, gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].block, FTL_SORT_BY_EC);
        FTL_ClearHeaderBlock(FTL_BLOCK_HEADER_TABLE, 0);
    }

    if ((gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].block!= WORD_MASK) && (gFtlNbtEmptyBlk[gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].block] == FTL_SN_TYPE_PBT)) // push close PBT to PBT pool
    {
        D_PRINTF("PBTTailblk[%4x]\n", gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].block);
        FTL_BlockPushList(FTL_BLOCK_POOL_PBT, gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].block, FTL_SORT_BY_SN);
        FTL_ClearHeaderBlock(FTL_BLOCK_HEADER_TABLE, 0);
    }

    if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT] < FTL_QBT_BLOCK_COUNT)
    {
        FTL_PurgePool2Free(FTL_BLOCK_POOL_PBT);
        gFtlNbtType.existPBT    = FALSE;
        gFtlPbtInfo.sn          = 0;
    }

    if ((gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT] == (FTL_QBT_BLOCK_COUNT + 1)) && (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT] != (FTL_QBT_BLOCK_RELEASE)))
    {
        blk = gFtlBlkListInfo.tail[FTL_BLOCK_POOL_PBT];
        FTL_BlockPopPushList(FTL_BLOCK_POOL_FREE, blk, FTL_SORT_BY_EC);

        if ((gFtlNbtType.existPBT != FALSE) && (gFtlQbtSn[0] == gFtlBlkListInfo.tail[FTL_BLOCK_POOL_PBT]))
        {
            gFtlNbtType.existQBT = TRUE;
            gFtlNbtType.existPBT = FALSE;
        }
    }

    if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT] >= FTL_QBT_BLOCK_RELEASE)
    {
        FTL_PurgePbt2Free();
    }

    D_PRINTF_FTL("PBTcnt[%d]tail[%4x]\n", gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT], gFtlBlkListInfo.tail[FTL_BLOCK_POOL_PBT]);
    // -----------------------------------

    if (bt->prevState == FTL_BUILD_TABLE_READ_SN) // Scan SN
    {
        D_PRINTF("Scan QE[%d]QS[0]:%4x QS[1]:%x PH[%4x] HPBT[%4x]PBCnt[%d]\n",
                   gFtlNbtType.existQBT, gFtlQbtSn[0], gFtlQbtSn[1],
                   gFtlBlkListInfo.head[FTL_BLOCK_POOL_PBT],
                   gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].block,
                   gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT]);

        // ----- QBT as FQBT
        if (gFtlSNTable[gFtlBlkListInfo.tail[FTL_BLOCK_POOL_QBT]] > gFtlSNTable[gFtlBlkListInfo.tail[FTL_BLOCK_POOL_PBT]])
        {
            FTL_PurgePool2Free(FTL_BLOCK_POOL_PBT);

            while (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_QBT])
            {
                FTL_BlockPopPushList(FTL_BLOCK_POOL_PBT, gFtlBlkListInfo.head[FTL_BLOCK_POOL_QBT], FTL_SORT_BY_SN);
            }

            gFtlQbtSn[0] = gFtlBlkListInfo.tail[FTL_BLOCK_POOL_PBT];
            gFtlNbtType.existQBT = TRUE;
            gFtlNbtType.existPBT = FALSE;
        }
        else
        {
            if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_QBT] > 0)
                FTL_PurgePool2Free(FTL_BLOCK_POOL_QBT);

            if ((gFtlQbtSn[1] != DWORD_MASK) && (gFtlQbtSn[1] == gFtlSNTable[gFtlBlkListInfo.tail[FTL_BLOCK_POOL_PBT]]))
            {
                gFtlQbtSn[0] = gFtlBlkListInfo.tail[FTL_BLOCK_POOL_PBT];
                gFtlNbtType.existQBT = TRUE;
                gFtlNbtType.existPBT = FALSE;
            }
            else
            {
                gFtlNbtType.existQBT = FALSE;
                gFtlNbtType.existPBT = (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT] == FTL_QBT_BLOCK_COUNT) ? TRUE : FALSE;
            }
        }
    }

    if (bt->prevState != FTL_BUILD_TABLE_READ_BLIST)
    {
        if ((gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].block != WORD_MASK) && FTL_CURRENT_HEADER_NEW(FTL_BLOCK_HEADER_HOST, 0))
        {
            D_PRINTF_FTL("CurHeader-D[%d-%d]B[%4x]PG[%3x]s[%x]\n",
                gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].bank & FTL_CE_MASK,
                gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].bank >> FTL_CE_SHIFT,
                gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].block,
                gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].page,
                gFtlBlkListTbl[blk].vs.b.status);

            FTL_BlockPushList(FTL_BLOCK_POOL_USER, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].block, FTL_SORT_NONE);
            FTL_ClearHeaderBlock(FTL_BLOCK_HEADER_HOST, 0);
        }

        bt->prevState        = bt->buildState;
        bt->buildState       = FTL_BUILD_TABLE_READ_L2P;
        bt->readBlistBlock   = WORD_MASK;
    }

    currTick = HalTimer_GetElapsedTimeMsec(currTick);
    D_PRINTF_FTL("NBT2 T[%d]ms\n", currTick);

    //return bt;
}

//-------------------------------------------------------------------
// Function     : static void FTL_RebuildBlockList(tFTL_BUILD_TABLE_INFO *bt)
// Description  : FTL rebuild block list: gFtlBlkListTbl, gFtlBlkListInfo, gFtlCurrHeader, gFtlMgr,
// Input        : tFTL_BUILD_TABLE_INFO *bt
// return       : N/A
//-------------------------------------------------------------------
static void FTL_RebuildBlockList(tFTL_BUILD_TABLE_INFO *bt)
{
    U32 idx, group, ec = 0, currTick;
    U16 blk;

    currTick = HalTimer_ReadTick();
    D_PRINTF_FTL("Re-FTL\n");
    MEM_SET(gFtlL2PTbl, BYTE_MASK, sizeof(tPAA) * L2P_NUM);

    // Clear gFtlCurrHeader
    for (idx = 0; idx < FTL_BLOCK_HEADER_MAX; idx++)
    {
        for (group = 0; group < FTL_GROUP_NUM; group++)
        {
            FTL_ClearHeaderBlock(idx, group);
        }
    }

    // Rebuild gFtlBlkListTbl and gFtlBlkListInfo
    for (idx = 0; idx < FTL_BLOCK_POOL_MAX; idx++)
    {
        gFtlBlkListInfo.head[idx]   = WORD_MASK;
        gFtlBlkListInfo.tail[idx]   = WORD_MASK;
        gFtlBlkListInfo.blkCnt[idx] = 0;
    }

    MEM_CLR(gFtlBlkListTbl, sizeof(tFTL_BLOCK_NODE) * FTL_SUPPORT_MAX_BLK);

    for (blk = smSysInfo->d.FtlData.d.StartIdx; blk < (smSysInfo->d.FtlData.d.EndIdx + 1); blk++)
    {
        if (blk == smSysInfo->d.FtlData.d.EndIdx)
            gFtlBlkListTbl[blk].block    = WORD_MASK;
        else
            gFtlBlkListTbl[blk].block    = blk + 1;

        gFtlBlkListTbl[blk].vs.b.status  = FTL_BLOCK_POOL_EMPTY;
        gFtlBlkListTbl[blk].vs.b.vac     = 0;
        gFtlBlkListTbl[blk].vs.b.host    = 0;
        gFtlBlkListTbl[blk].vs.b.bad     = 0;

        if (smSysInfo->d.Header.d.SubHeader.d.ECnt.d.dwValidTag != SI_TAG_ECNT)
        {
            gFtlBlkListTbl[blk].eraseCnt    = 0;
        }
        else
        {
            gFtlBlkListTbl[blk].eraseCnt    = smSysInfo->d.ECnt.d.EraseCount[blk];
        }

        ec                                  += gFtlBlkListTbl[blk].eraseCnt;
    }

    gFtlBlkListInfo.head[FTL_BLOCK_POOL_EMPTY]      = smSysInfo->d.FtlData.d.StartIdx;
    gFtlBlkListInfo.tail[FTL_BLOCK_POOL_EMPTY]      = smSysInfo->d.FtlData.d.EndIdx;
    gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_EMPTY]    = gFtlBlkListInfo.tail[FTL_BLOCK_POOL_EMPTY] - gFtlBlkListInfo.head[FTL_BLOCK_POOL_EMPTY] + 1;

    while (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_EMPTY])
    {
        blk = FTL_BlockPopHead(FTL_BLOCK_POOL_EMPTY);

        if (gFtlNbtEmptyBlk[blk] == FTL_SN_TYPE_USER)
        {
            gFtlBlkListTbl[blk].vs.b.host   = 1;

            FTL_BlockPushList(FTL_BLOCK_POOL_USER, blk, FTL_SORT_BY_SN);
        }
        else
        {
            FTL_BlockPushList(FTL_BLOCK_POOL_FREE, blk, FTL_SORT_BY_EC);
        }
    }

    // Reset gFtlMgr
    MEM_CLR(&gFtlMgr, sizeof(tFTL_MANAGER));
    gFtlMgr.QbtBackup      = TRUE;
    gFtlMgr.AllEraseCnt    = ec;
    gFtlMgr.SerialNumber   = gFtlSNTable[gFtlBlkListInfo.tail[FTL_BLOCK_POOL_USER]];
    gFtlMgr.ReleaseBlk     = FALSE;
    gFtlMgr.ReleaseGCBlk   = FALSE;
    gFtlMgr.flushDummy     = FALSE;
    gFtlMgr.TableSN        = 0;

    bt->prevState        = bt->buildState;
    bt->buildState       = FTL_BUILD_TABLE_READ_P2L;
    bt->readBlistBlock   = WORD_MASK;

    currTick = HalTimer_GetElapsedTimeMsec(currTick);
    D_PRINTF_FTL("ReBuild T[%d]ms\n", currTick);

    //return bt;
}

//-------------------------------------------------------------------
// Function     : static void FTL_LoadL2PTable(tFTL_BUILD_TABLE_INFO *bt)
// Description  : FTL read L2P
// Input        : tFTL_BUILD_TABLE_INFO *bt
// return       : N/A
//-------------------------------------------------------------------
static void FTL_LoadL2PTable(tFTL_BUILD_TABLE_INFO *bt)
{
    U32 currTick;
    U16 blk;
    U8  error = FALSE; // FOR L2P Read fail

    currTick = HalTimer_ReadTick();
    D_PRINTF_FTL("NBT3-L2P\n");//load L2P table

    //idx = idx1 = idx2 = 0;
    D_PRINTF("QE[%d]PE[%d]FQE[%d]QS[0]:%4x QS[1]:%x PH[%4x]\n", gFtlNbtType.existQBT, gFtlNbtType.existPBT, gFtlNbtType.existFQBT, gFtlQbtSn[0], gFtlQbtSn[1], gFtlBlkListInfo.head[FTL_BLOCK_POOL_PBT]);

    if ((gFtlNbtType.existQBT == TRUE) && (gFtlQbtSn[0] == DWORD_MASK)) // QBT
    {
        #if (FTL_PARTIAL_LOAD_L2P != FALSE)
        gFtlNbtType.existQBT = FTL_BuildL2PZone(0, 1); // Zone 0
        if (gFtlNbtType.existQBT)
            gFtlNbtType.existQBT = FTL_BuildL2PZone(FTL_POR_PBT_ZONE_NUM - 1, FTL_POR_PBT_ZONE_NUM); // Zone Last
        #else
        gFtlNbtType.existQBT = FTL_QuickOrPartialBuildL2P(FTL_BUILD_TYPE_QBT, 0);
        #endif

        if (gFtlNbtType.existQBT != TRUE)
        {
            FTL_PurgePool2Free(FTL_BLOCK_POOL_QBT);
            MEM_SET(gFtlL2PTbl, BYTE_MASK, sizeof(tPAA)*L2P_NUM);
            error = TRUE;
        }
        else
        {
            gFtlMgr.TableSN = gFtlSNTable[gFtlBlkListInfo.head[FTL_BLOCK_POOL_QBT]] - 1;
            gFtlQbtSn[0]    = gFtlBlkListInfo.head[FTL_BLOCK_POOL_QBT];
        }

        D_PRINTF("QE[%d]QBT[%4x]\n", gFtlNbtType.existQBT, gFtlBlkListInfo.tail[FTL_BLOCK_POOL_QBT]);
    }
    else if ((gFtlNbtType.existQBT & gFtlNbtType.existPBT) == FALSE)
    {
        if ((gFtlNbtType.existQBT == TRUE) && (gFtlQbtSn[0] == gFtlBlkListInfo.tail[FTL_BLOCK_POOL_PBT])) // FQBT
        {
            gFtlNbtType.existQBT = FTL_QuickOrPartialBuildL2P(FTL_BUILD_TYPE_PBT, 0);

            if (gFtlNbtType.existQBT != FALSE)
            {
                gFtlNbtType.existQBT = FTL_GetVacFromBlock(gFtlQbtSn[0]);

                if (gFtlNbtType.existQBT != FALSE)
                {
                    gFtlNbtType.existFQBT = TRUE;
                    gFtlPbtInfo.sn  = gFtlSNTable[gFtlBlkListInfo.head[FTL_BLOCK_POOL_PBT]];
                    gFtlMgr.TableSN = gFtlSNTable[gFtlBlkListInfo.head[FTL_BLOCK_POOL_PBT]] - 1;
                    gFtlQbtSn[0]    = gFtlBlkListInfo.head[FTL_BLOCK_POOL_PBT];
                }
            }
            else
            {
                MEM_SET(gFtlL2PTbl, BYTE_MASK, sizeof(tPAA)*L2P_NUM);
                FTL_PurgePool2Free(FTL_BLOCK_POOL_PBT);
                gFtlMgr.TableSN = gFtlPbtInfo.sn = 0;

                error = TRUE;
            }
        }
    }

    if ((gFtlNbtType.existQBT == FALSE) && (gFtlBlkListInfo.tail[FTL_BLOCK_POOL_PBT] != WORD_MASK))
    {
        error = FALSE;

        if (gFtlSNTable[gFtlBlkListInfo.tail[FTL_BUILD_TYPE_PBT]] >= FTL_PBT_PERIOD_BLK)
        {
            gFtlNbtType.existPBT = FTL_QuickOrPartialBuildL2P(FTL_BUILD_TYPE_PBT, 0);
            if (gFtlNbtType.existPBT != TRUE) error = TRUE;
        }
        else
        {
            gFtlNbtType.existPBT = FALSE;
        }

        if (gFtlNbtType.existPBT != FALSE)
        {
            blk = gFtlBlkListInfo.tail[FTL_BUILD_TYPE_PBT];
            gFtlNbtType.existPBT = FTL_GetVacFromBlock(blk);

            if (gFtlNbtType.existPBT != FALSE)
            {
                gFtlPbtInfo.sn  = gFtlSNTable[gFtlBlkListInfo.head[FTL_BUILD_TYPE_PBT]];
                gFtlMgr.TableSN = gFtlSNTable[gFtlBlkListInfo.head[FTL_BLOCK_POOL_PBT]] - 1;

                if ((gFtlNbtEmptyBlk[blk] == FTL_SN_TYPE_QBT) || (gFtlNbtEmptyBlk[blk] == FTL_SN_TYPE_FQBT))
                {
                    gFtlNbtType.existPBT  = FALSE;
                    gFtlNbtType.existQBT  = gFtlNbtType.existFQBT = TRUE; // P2L same as FQBT
                    gFtlQbtSn[0] = gFtlBlkListInfo.head[FTL_BLOCK_POOL_PBT];
                }
            }
        }
        else
        {
            MEM_SET(gFtlL2PTbl, BYTE_MASK, sizeof(tPAA)*L2P_NUM);
            FTL_PurgePool2Free(FTL_BLOCK_POOL_PBT);
            gFtlMgr.TableSN = gFtlPbtInfo.sn = 0;
        }
    }

    if ((gFtlNbtType.existQBT != FALSE) && (gFtlNbtType.existFQBT != TRUE)) // QBT
    {
        bt->prevState        = bt->buildState;
        bt->buildState       = FTL_BUILD_TABLE_READ_OPENBK;
        bt->readBlistBlock   = WORD_MASK;
    }
    else if (error != FALSE)
    {
        bt->prevState        = bt->buildState;
        bt->buildState       = FTL_BUILD_TABLE_REBUILD_BLIST;
        bt->readBlistBlock   = WORD_MASK;
    }
    else// if (((gFtlNbtType.existQBT != FALSE) && (gFtlNbtType.existFQBT != FALSE)) || (gFtlNbtType.existPBT != FALSE) || ) // FQBT or PBT
    {
        bt->prevState        = bt->buildState;
        bt->buildState       = FTL_BUILD_TABLE_READ_P2L;
        bt->readBlistBlock   = WORD_MASK;
    }

    currTick = HalTimer_GetElapsedTimeMsec(currTick);
    D_PRINTF_FTL("NBT3-L2P T[%d]ms QE[%d]FQE[%d]PE[%d]err[%d]go2S[%d]\n", currTick, gFtlNbtType.existQBT, gFtlNbtType.existFQBT, gFtlNbtType.existPBT, error, bt->buildState);
}

//-------------------------------------------------------------------
// Function     : static void FTL_P2LBuildTable(tFTL_BUILD_TABLE_INFO *bt)
// Description  : FTL read P2L and build L2P
// Input        : tFTL_BUILD_TABLE_INFO *bt
// return       : N/A
//-------------------------------------------------------------------
static void FTL_P2LBuildTable(tFTL_BUILD_TABLE_INFO *bt)
{
    U32 sn, currTick;
    U16 blk;
    U8  group, scanStage;

    currTick = HalTimer_ReadTick();
    D_PRINTF_FTL("NBT3-P2L\n");//load P2L

    if ((gFtlPbtInfo.sn != 0) && (gFtlNbtType.existQBT != FALSE) && (gFtlNbtType.existFQBT != FALSE)) // FQBT
    {
        sn = gFtlPbtInfo.sn;
    }
    else if ((gFtlPbtInfo.sn != 0) && (gFtlNbtType.existPBT != FALSE) && (gFtlNbtType.existFQBT != TRUE)) // PBT
    {
        sn = gFtlPbtInfo.sn - 1;
    }
    else
    {
        sn = 0;
    }

    D_PRINTF("QE[%d]PE[%d]FQ[%d]sn[%x]Psn[%x]\n", gFtlNbtType.existQBT, gFtlNbtType.existPBT, gFtlNbtType.existFQBT, sn, gFtlPbtInfo.sn);

    //scan last page to rebuild L2P table
    group = 0;//for (group = 0; group < FTL_GROUP_NUM; group++)
    {
        for (scanStage = FTL_SCAN_L2P_STAGE_HOST; scanStage < FTL_SCAN_L2P_STAGE_HEADER; scanStage++)
        {
            blk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_USER];

            while (blk != WORD_MASK)
            {
                if (gFtlSNTable[blk] == DWORD_MASK)
                {
                    blk = gFtlBlkListTbl[blk].block;//get next user block
                    continue;
                }

                if ((gFtlNbtType.existPBT || gFtlNbtType.existFQBT) && (gFtlSNTable[blk] < sn))
                {
                    blk = gFtlBlkListTbl[blk].block;//get next user block
                    continue;
                }

                if ((scanStage == FTL_SCAN_L2P_STAGE_GC) && (gFtlBlkListTbl[blk].vs.b.host == 1))
                {
                    blk = gFtlBlkListTbl[blk].block;//get next user block
                    continue;
                }
                else if ((scanStage == FTL_SCAN_L2P_STAGE_HOST) && (gFtlBlkListTbl[blk].vs.b.host == 0))
                {
                    blk = gFtlBlkListTbl[blk].block;//get next user block
                    continue;
                }

                //update L2P table
                if (FTL_NANDReadP2LBuildL2P(group, blk, FTL_NAND_FTL_P2L_PAGE))
                {
                    FTL_LastPageP2LUpdateL2P(group, blk);

                    gFtlTrimInfo.trimBlockcnt++;
                }
                blk = gFtlBlkListTbl[blk].block;//get next user block
            }
        }
    }

    bt->prevState        = bt->buildState;
    bt->buildState       = FTL_BUILD_TABLE_READ_OPENBK;
    bt->readBlistBlock   = WORD_MASK;

    currTick = HalTimer_GetElapsedTimeMsec(currTick);
    D_PRINTF_FTL("NBT3-P2L T[%d]ms\n", currTick);

    //return bt;
}

//-------------------------------------------------------------------
// Function     : static void FTL_ScanOpenBlock(tFTL_BUILD_TABLE_INFO *bt)
// Description  : FTL read open block
// Input        : tFTL_BUILD_TABLE_INFO *bt
// return       : N/A
//-------------------------------------------------------------------
static void FTL_ScanOpenBlock(tFTL_BUILD_TABLE_INFO *bt)
{
    U32 aux[2], header, sn; // laa, sn
    U32 currTick;
    U16 blk, nxblk = WORD_MASK, opblk = WORD_MASK, page, bank, die, blkcnt, loopcnt = 0;
    U8  openType;

    currTick = HalTimer_ReadTick();

    blk     = gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE];
    blkcnt  = gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE];

    D_PRINTF_FTL("NBT4-1 UTailB[%4x]sn[%x] FHeadB[%4x]cnt[%d]\n", gFtlBlkListInfo.tail[FTL_BLOCK_POOL_USER], gFtlSNTable[gFtlBlkListInfo.tail[FTL_BLOCK_POOL_USER]], blk, blkcnt);

    while ((blk != WORD_MASK) && ((loopcnt++) < blkcnt))
    {
        nxblk = gFtlBlkListTbl[blk].block;

        if ((gFtlSNTable[blk] < gFtlSNTable[gFtlBlkListInfo.tail[FTL_BLOCK_POOL_USER]]) || (gFtlNbtEmptyBlk[blk] == FTL_SN_TYPE_PBT) || (gFtlNbtEmptyBlk[blk] == FTL_SN_TYPE_QBT) || (gFtlNbtEmptyBlk[blk] == FTL_SN_TYPE_FQBT))
        {
            FTL_BlockPopPushList(FTL_BLOCK_POOL_EMPTY, blk, FTL_SORT_NONE);
            blk = nxblk;
            continue;
        }

        MEM_SET(aux, BYTE_MASK, sizeof(aux));

        // Get SN and Block Header
        for (page = 0; page < FTL_NBT_GET_SN_HEADER_RPAGES; page++)
        {
            gFtlBTJob[0] = FTL_NANDReadPageBuild(CACHE_SCANPAGE, 0, 0, 0, 0, blk, page, FTL_SCAN_PAGE_AUX, FALSE);
            Core_JobWait();

            //check read job status
            if (gFtlBTJob[0]->status & FIO_S_BLANK_ERR)
            {
                if (page != 0)
                    D_PRINTF("NBT41scB[%4X]P[%3x]empty\n", blk, page);
                break;
            }
            else if (gFtlBTJob[0]->status & FIO_S_UNC_ERR)
            {
                //gFtlBTJob[0]->header = FTL_BLOCK_HEADER_MAX;//unknown header

                D_PRINTF("NBT41scB[%4X]RRf\n", blk);

                if (page == (FTL_NBT_GET_SN_HEADER_RPAGES - 1))
                {
                    gFtlSNTable[blk]                = DWORD_MASK;
                    gFtlNbtEmptyBlk[blk]            = FTL_SN_TYPE_EMPTY;
                    gFtlBlkListTbl[blk].vs.b.vac    = 0;
                    FTL_BlockPopPushList(FTL_BLOCK_POOL_EMPTY, blk, FTL_SORT_NONE);
                }
            }
            else
            {
                aux[AUX_BUF_IDX_LAA] = gFtlBTJob[0]->aux[((gFtlBTJob[0]->paaCnt - 1) << 1)];        // laa
                aux[AUX_BUF_IDX_SN]  = gFtlBTJob[0]->aux[((gFtlBTJob[0]->paaCnt - 1) << 1) + 1];    // sn
                break;
            }
        }

        if (gFtlBTJob[0]->status & FIO_S_BLANK_ERR)
        {
            gFtlSNTable[blk]     = DWORD_MASK;
            gFtlNbtEmptyBlk[blk] = FTL_SN_TYPE_EMPTY;
            FTL_BlockPopPushList(FTL_BLOCK_POOL_FREE, blk, FTL_SORT_NONE);
            // do nothing and to next block
        }
        else
        {
            header = aux[AUX_BUF_IDX_SN] >> FTL_META_HEADER_SHIFT;
            sn     = aux[AUX_BUF_IDX_SN] & FTL_META_SN_MASK;

            if ((header == FTL_BLOCK_HEADER_TABLE) ||
                ((aux[AUX_BUF_IDX_LAA] & ~FTL_TABLE_TAG_MASK) == FTL_PBT_TABLE_TAG) ||
                ((aux[AUX_BUF_IDX_LAA] & ~FTL_TABLE_TAG_MASK) == FTL_QBT_TABLE_TAG) ||
                ((aux[AUX_BUF_IDX_LAA] & ~FTL_TABLE_TAG_MASK) == FTL_FQBT_TABLE_TAG))
            {
                gFtlSNTable[blk] = 0;

                if ((aux[AUX_BUF_IDX_LAA] & ~FTL_TABLE_TAG_MASK) == FTL_PBT_TABLE_TAG)
                {
                    gFtlNbtEmptyBlk[blk] = FTL_SN_TYPE_PBT;
                }
                else if ((aux[AUX_BUF_IDX_LAA] & ~FTL_TABLE_TAG_MASK) == FTL_QBT_TABLE_TAG)
                {
                    gFtlNbtEmptyBlk[blk] = FTL_SN_TYPE_QBT;
                }
                else if ((aux[AUX_BUF_IDX_LAA] & ~FTL_TABLE_TAG_MASK) == FTL_FQBT_TABLE_TAG)
                {
                    gFtlNbtEmptyBlk[blk] = FTL_SN_TYPE_FQBT;
                }
                else
                {
                    D_PRINTF("B[%4x]H[%d]TAG[%x]SN[%x]\n", blk, header, aux[AUX_BUF_IDX_LAA], aux[AUX_BUF_IDX_SN]);
                }

                FTL_BlockPopPushList(FTL_BLOCK_POOL_EMPTY, blk, FTL_SORT_NONE);
                //FTL_PushList(FTL_BLOCK_POOL_EMPTY, blk, FTL_SORT_NONE);
            }
            else if ((header == FTL_BLOCK_HEADER_HOST) || (header == FTL_BLOCK_HEADER_GC))
            {
                gFtlSNTable[blk] = sn;

                if (sn < gFtlSNTable[gFtlBlkListInfo.tail[FTL_BLOCK_POOL_USER]])
                {
                    FTL_BlockPopPushList(FTL_BLOCK_POOL_EMPTY, blk, FTL_SORT_NONE);
                }
                else
                {
                    if ((aux[AUX_BUF_IDX_SN] >> FTL_META_BOOTGC_WRITE_SHIFT) & FTL_META_BOOTGC_WRITE_MUSK)
                    {
                        gFtlNbtEmptyBlk[blk] = FTL_SN_TYPE_BOOTGC;
                        FTL_BlockPopPushList(FTL_BLOCK_POOL_EMPTY, blk, FTL_SORT_NONE);
                        D_PRINTF("B[%4x]sn[%x]-boot gc\n", blk, sn);
                    }
                    else
                    {
                        gFtlNbtEmptyBlk[blk] = FTL_SN_TYPE_OPENHOST;

                        if (opblk != WORD_MASK)
                        {
                            FTL_BlockPopPushList(FTL_BLOCK_POOL_FREE, blk, FTL_SORT_BY_SN);
                            D_PRINTF("Multi-openBK\n");
                        }

                        opblk = blk;
                        D_PRINTF("B[%4x]sn[%x]-host open\n", blk, aux[AUX_BUF_IDX_SN]);
                    }
                }
            }
            else // something else is error
            {
                D_PRINTF("H[%d]AUX[%x][%x]SN[%X]\n", header, aux[AUX_BUF_IDX_LAA], aux[AUX_BUF_IDX_SN], sn);
            }
        } // page 0 or 1 is not empty

        blk = nxblk;
    }

    currTick = HalTimer_GetElapsedTimeMsec(currTick);

    D_PRINTF_FTL("NBT4-1 T[%d]ms\n", currTick);

#if 1
    D_PRINTF_FTL("NBT4-2 Run FreeCnt[%4x]sn[%x]\n",
        gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE], gFtlSNTable[gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE]]);

    currTick = HalTimer_ReadTick();

    blk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE];

    while (blk != WORD_MASK)
    {
        nxblk = gFtlBlkListTbl[nxblk].block;

        if (gFtlNbtEmptyBlk[blk] == FTL_SN_TYPE_OPENHOST)
        {
            D_PRINTF("B[%4x]sn[%x]pushGC\n", blk, gFtlSNTable[blk]);
            FTL_BlockPopPushList(FTL_BLOCK_POOL_GC, blk, FTL_SORT_BY_SN);

            gFtlSporInfo.openBlkCount++;
        }

        blk = nxblk;
    }

    currTick = HalTimer_GetElapsedTimeMsec(currTick);
    D_PRINTF_FTL("NBT4-2 T[%d]ms\n", currTick);
#else
    // ------------------ set last sn of close block ------------------
    blk     = gFtlBlkListInfo.tail[FTL_BLOCK_POOL_USER];
    page    = 0;
    bank    = 0;

    if (blk == WORD_MASK)   sn = 0;
    else                    sn = gFtlSNTable[blk];

    // ------------------ free header ------------------
    blk     = gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE];

    MEM_SET(gFtlLastPageBuf.lastPageBufFtlP2L, BYTE_MASK, (gFtlLastPageP2LSize * LP_P2L_PP_SIZE * (FTL_GROUP_NUM * 2)));

    D_PRINTF("NBT4-2 Run FreeCnt[%4x]sn[%x]TailSn[%x]\n",
        gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE], gFtlSNTable[blk], sn);

    currTick = HalTimer_ReadTick();

    while (blk != WORD_MASK)
    {
        if (gFtlSNTable[blk] < sn)
        {
            FTL_BlockPopPushList(FTL_BLOCK_POOL_EMPTY, blk, FTL_SORT_NONE);
            FTL_ClearHeaderBlock(FTL_BLOCK_HEADER_HOST,0);

            blk     = gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE];
            page    = 0;
            bank    = 0;
            continue;
        }

        if ((page == 0) && (bank == 0))  D_PRINTF("NBT4-2 B[%4x]\n", blk);

        openType = FTL_BuildOpenBlockP2L(page, bank, blk);

        if ((openType == FTL_OPEN_BLOCK_BANK_EMPTY) || (openType == FTL_OPEN_BLOCK_EMPTY_MAX))
        {
            if (openType == FTL_OPEN_BLOCK_EMPTY_MAX)
            {
                D_PRINTF("Check log\n");
            }

            FTL_BlockPopPushList(FTL_BLOCK_POOL_GC, blk, FTL_SORT_NONE);
            gFtlSporInfo.openBlkCount++;
            blk     = gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE];
            page    = 0;
            bank    = 0;

            for (die = 0; die < FTL_BANK_NUM; die++)
            {
                gFtlMgr.LastPageBufIdx[die][FTL_BLOCK_HEADER_HOST][0]++;
                gFtlMgr.LastPageBufIdx[die][FTL_BLOCK_HEADER_HOST][0] %= 2;
            }
        }
        else
        {
            break;
        }
    }

    currTick = HalTimer_GetElapsedTimeMsec(currTick);
    D_PRINTF_FTL("NBT4-2 T[%d]ms\n", currTick);
#endif

    // Release Empty Pool
    blk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_EMPTY];

    while (blk != WORD_MASK)
    {
        nxblk = gFtlBlkListTbl[blk].block;

        if ((gFtlSNTable[blk] < gFtlMgr.TableSN) || (gFtlNbtEmptyBlk[blk] != FTL_SN_TYPE_USER))
            FTL_BlockPopPushList(FTL_BLOCK_POOL_FREE, blk, FTL_SORT_BY_EC);

        blk = nxblk;
    }

    if (gFtlSporInfo.openBlkCount != 0) gFtlSporInfo.flag = TRUE;

    bt->prevState        = bt->buildState;
    bt->buildState       = FTL_BUILD_TABLE_COUNT;
    bt->readBlistBlock   = WORD_MASK;
}

///////////////////////////////////////////////////////////
///                                                     ///
///       ***        ***   *********    *********       ///
///       ***     ** ***   ***   ***    *********       ///
///       ***   **   ***   **********      ***          ///
///       *** **     ***   ***    ***      ***          ///
///       ***        ***   **********      ***          ///
///                                                     ///
///////////////////////////////////////////////////////////
//-------------------------------------------------------------------
// Function     : static void FTL_NormalBuildTable(tFTL_BUILD_TABLE_INFO *bt)
// Description  : Build L2P after SPOR
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void FTL_NormalBuildTable(tFTL_BUILD_TABLE_INFO *bt)
{
    // Load SBLK & Free Idx from EEPROM
    MEM_COPY((U32*)gFtlRemap.rmpSuperBlkTbl, (U32*)smSysInfo->d.FtlRmpData.d.SBlkRmpInfo, sizeof(U16) * gNfInfo.blocksPerLun * gNfInfo.totalLun);
    D_PRINTF_FTL("Get Remapblk\n");

    FTL_DumpRemap(TRUE);

    D_PRINTF_FTL("Tok:S[0x%4x]E[0x%4x]\n", smSysInfo->d.FtlData.d.StartIdx, smSysInfo->d.FtlData.d.EndIdx);

    MEM_CLR(&gFtlNbtType, sizeof(gFtlNbtType));

#if (FTL_NBT_BUILD == TRUE)
    if ((smSysInfo->d.Header.d.SubHeader.d.FTL.d.dwValidTag != SI_TAG_FTL) ||
        (smSysInfo->d.FtlData.d.QBTBlockTag != SI_QBTINFO_TAG))
    {
        bt->prevState       = bt->buildState;
        bt->buildState      = FTL_BUILD_TABLE_READ_SN;
        bt->readBlistBlock  = WORD_MASK;

        D_PRINTF_FTL("QBTng=>NBT:vT[0x%x]qT[0x%x]\n", smSysInfo->d.Header.d.SubHeader.d.FTL.d.dwValidTag, smSysInfo->d.FtlData.d.QBTBlockTag);
    }
    else
    {
        #if (FTL_QBT_BUILD == TRUE)
            U8 qbtcnt, error = FALSE;
            MEM_COPY(&gFtlQbtInfo[0], &smSysInfo->d.FtlData.d.QBTInfo[0], sizeof(tFTL_QBT_INFO) * FTL_QBT_BLOCK_COUNT);

            for (qbtcnt = 0; qbtcnt < FTL_QBT_BLOCK_COUNT; qbtcnt++)
            {
                if ((gFtlQbtInfo[qbtcnt].block < smSysInfo->d.FtlData.d.StartIdx) || (gFtlQbtInfo[qbtcnt].block > smSysInfo->d.FtlData.d.EndIdx))
                {
                    D_PRINTF_FTL("QBTng:B[%4x]s[0x%x]\n", gFtlQbtInfo[qbtcnt].block, gFtlQbtInfo[qbtcnt].sn);
                    error = TRUE;
                }
                else
                {
                    D_PRINTF_FTL("QBTok:B[%4x]s[0x%x]\n", gFtlQbtInfo[qbtcnt].block, gFtlQbtInfo[qbtcnt].sn);
                }
            }

            if (error != TRUE)
            {
                bt->prevState       = bt->buildState;
                bt->buildState      = FTL_BUILD_TABLE_READ_BLIST;
                bt->readBlistBlock  = gFtlQbtInfo[qbtcnt-1].block;

                gFtlNbtType.existQBT = TRUE;
            }
            else
            {
                bt->prevState       = bt->buildState;
                bt->buildState      = FTL_BUILD_TABLE_READ_SN;
                bt->readBlistBlock  = WORD_MASK;

                smSysInfo->d.FtlData.d.QBTBlockTag = DWORD_MASK;
                MEM_SET(gFtlQbtInfo, BYTE_MASK, sizeof(tFTL_QBT_INFO) * FTL_MAX_QBT_CNT);
                MEM_SET(&smSysInfo->d.FtlData.d.QBTInfo[0], BYTE_MASK, sizeof(tFTL_QBT_INFO) * FTL_QBT_BLOCK_COUNT);
                SyIn_Synchronize(SI_AREA_FTL , SYSINFO_WRITE, SI_SYNC_BY_FTL);
            }
        #else
            bt->prevState       = bt->buildState;
            bt->buildState      = FTL_BUILD_TABLE_READ_SN;
            bt->readBlistBlock  = WORD_MASK;
        #endif
    }
#else
    bt->prevState       = bt->buildState;
    bt->buildState      = FTL_BUILD_TABLE_ERASE;
    bt->readBlistBlock  = WORD_MASK;
#endif
}

//-------------------------------------------------------------------
// Function     : void FTL_CheckUserBlocks(void)
// Description  : FTL rlease blocks
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_CheckUserBlocks(void)
{
    U32 currTick;
    U16 nxblk, crblk;

    currTick = HalTimer_ReadTick();
    //D_PRINTF_FTL("CHKVc\n");

    nxblk = crblk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_USER];

    while (nxblk != WORD_MASK)
    {
        nxblk = gFtlBlkListTbl[crblk].block;

        if (gFtlBlkListTbl[crblk].vs.b.vac == 0)
        {
			D_PRINTF_FTL("FrB[%4x]sn[%x]TS[%x]\n",crblk, gFtlSNTable[crblk], gFtlMgr.TableSN);

            if (gFtlSNTable[crblk] < gFtlMgr.TableSN)
                FTL_BlockPopPushList(FTL_BLOCK_POOL_FREE, crblk, FTL_SORT_BY_EC);
            else
                FTL_BlockPopPushList(FTL_BLOCK_POOL_EMPTY, crblk, FTL_SORT_BY_EC);
        }

        crblk = nxblk;
    }

    if (HalTimer_GetElapsedTimeMsec(currTick) != 0)
    {
        D_PRINTF_FTL("CHKVcT[%d]ms\n", HalTimer_GetElapsedTimeMsec(currTick));
    }
}

//-------------------------------------------------------------------
// Function     : void FTL_SaveFakeQBT(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_SaveFakeQBT(void)
{
    U16 logicBlk = WORD_MASK, loop;
    U8  eccBKstatus = TRUE, eccBKcnt = 0;

    // restore Fake QBT for fast build from SPOR
    #if (FTL_FAKE_QBT == TRUE)

    FTL_PurgePool2Free(FTL_BLOCK_POOL_PBT);

    D_PRINTF_FTL("SaveFQBT\n");

    gFtlQbtSn[0] = gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE];

    for (loop = 0; loop < FTL_QBT_BLOCK_COUNT; loop++)
    {
        if (eccBKstatus != TRUE)
        {
            FTL_EraseBankBlocks(logicBlk);
            FTL_BlockPopPushList(FTL_BLOCK_POOL_FREE, logicBlk, FTL_SORT_BY_EC);
            loop--;
            eccBKcnt++;
            D_PRINTF("B[%4x]error loop[%d]eccBKcnt[%d]\n", logicBlk, loop, eccBKcnt);
        }

        if (eccBKcnt > FTL_QBT_BLOCK_COUNT)
        {
            D_PRINTF_FTL("ProgFailcnt[%d]\n", eccBKcnt);
            return;
        }

        // Erase Free Header
        FTL_EraseBankBlocks(gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE]);

        logicBlk = FTL_BlockPopHead(FTL_BLOCK_POOL_FREE);
        D_PRINTF("loop[%d]B[%4x]\n", loop, logicBlk);

        FTL_BlockPushList(FTL_BLOCK_POOL_PBT, logicBlk, FTL_SORT_NONE);

        gFtlPbtInfo.loopCnt                 = loop + 1;
        gFtlPbtInfo.block                   = logicBlk;
        gFtlBlkListTbl[logicBlk].vs.b.vac   = 0;
        gFtlSNTable[logicBlk]               = gFtlPbtInfo.sn = gFtlMgr.SerialNumber;
        gFtlMgr.SerialNumber++;

        // Flush L2P as PBT
        eccBKstatus = FTL_NANDWriteL2P(logicBlk, loop, FTL_FQBT_TABLE_TAG);
    }

    gFtlPbtInfo.block   = WORD_MASK;
    gFtlPbtInfo.loopCnt = gFtlPbtInfo.pageCnt = gFtlPbtInfo.sn = 0;

    if (smSysInfo->d.FtlData.d.QBTBlockTag == SI_QBTINFO_TAG)
    {
        FTL_PurgePool2Free(FTL_BLOCK_POOL_QBT);
        smSysInfo->d.FtlData.d.QBTBlockTag = DWORD_MASK;
        SyIn_Synchronize(SI_AREA_MISC , SYSINFO_WRITE, SI_SYNC_BY_FTL);
    }

    // erase next free block

    FTL_EraseBankBlocks(gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE]);
    #endif
}

//-------------------------------------------------------------------
// Function     : void FTL_RebuildVac(U32 currLaa, U32 laaEnd)
// Description  : Rebuild VAC from L2P table,for inverse Trim.
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_RebuildVac(U32 laaS, U32 laaEnd)
{
    tPAA    paaOld;
    U32     currTickAll, currLaa = laaS;
    U16     paaIdx = FTL_SUPPORT_MAX_BLK, blk;

    currTickAll = HalTimer_ReadTick();

    while (currLaa < laaEnd)   // should work with len = 0 case
    {
        if (paaIdx == FTL_SUPPORT_MAX_BLK)
        {
            if ((currLaa + FTL_SUPPORT_MAX_BLK) < L2P_NUM)
            {
                MemCopyNU32(gFtlFastVacBuf, (U32*)&gFtlL2PTbl[currLaa], sizeof(U32) * FTL_SUPPORT_MAX_BLK);
            }
            else
            {
                MemCopyNU32(gFtlFastVacBuf, (U32*)&gFtlL2PTbl[currLaa], sizeof(U32) * (L2P_NUM - currLaa));
            }
            paaIdx = 0;
        }

        if (gFtlFastVacBuf[paaIdx] != DWORD_MASK)
        {
            paaOld.all32 = gFtlFastVacBuf[paaIdx];

            if (paaOld.b.block >= gNfInfo.blocksPerPlane)
            {
                D_PRINTF("RvcB[%4x]\n", paaOld.b.block);
                ASSERT(0);
            }

            blk = FTL_FastGetRMPBlk(paaOld.b.ch, paaOld.b.ce, paaOld.b.lun, paaOld.b.plane, paaOld.b.block);

            gFtlBlkListTbl[blk].vs.b.vac++;
        }
        paaIdx++;
        currLaa++;
    }

    currTickAll = HalTimer_GetElapsedTimeMsec(currTickAll);
    D_PRINTF_FTL("FTL_RebuildVAC T[%d]ms\n", currTickAll);
}

//-------------------------------------------------------------------
// Function     : void FTL_PurgeCurrentHeader2Free(void)
// Description  : TCG LAA need to restore for Trim full disc.
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_PurgeCurrentHeader2Free(void)
{
    U16 paaIdx, blk;

    for (paaIdx = 0; paaIdx < FTL_GROUP_NUM; paaIdx++) // group
    {
        if (!FTL_CURRENT_HEADER_NEW(FTL_BLOCK_HEADER_HOST, paaIdx)) // not new or close current block
        {
            FTL_FlushDummyCloseBlock(FTL_BLOCK_HEADER_HOST, paaIdx, gNfInfo.pagesPerBlock/*, FALSE*/);
        }

        if (FTL_CURRENT_HEADER_NEW(FTL_BLOCK_HEADER_HOST, paaIdx) && (gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][paaIdx].block != WORD_MASK))
        {
            FTL_BlockPushList(FTL_BLOCK_POOL_USER, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][paaIdx].block, FTL_SORT_NONE);
            FTL_ClearHeaderBlock(FTL_BLOCK_HEADER_HOST, paaIdx);
        }
    }

    if (!FTL_CURRENT_HEADER_NEW(FTL_BLOCK_HEADER_TABLE, 0)) // clear pbt current header
    {
        FTL_BlockPushList(FTL_BLOCK_POOL_FREE, gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].block, FTL_SORT_BY_EC);
        FTL_ClearHeaderBlock(FTL_BLOCK_HEADER_TABLE, 0);
    }

    for (paaIdx = FTL_BLOCK_POOL_USER; paaIdx < FTL_BLOCK_POOL_FREE; paaIdx++) // push all table, host, gc block back to free
    {
        while (gFtlBlkListInfo.blkCnt[paaIdx] > 0)
        {
            blk = FTL_BlockPopHead(paaIdx);
            gFtlBlkListTbl[blk].vs.b.vac = 0;
            FTL_BlockPushList(FTL_BLOCK_POOL_FREE, blk, FTL_SORT_BY_EC);
        }
    }
}

//-------------------------------------------------------------------
// Function     : void FTL_SetExternLAA(void)
// Description  : LAA extern need to restore for Trim full disc.
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_SetExternLAA(void)
{
    tPAA paaOld;
    U32 laaExt;
    U16 blk;
    for (laaExt = L2P_NUM_RAW; laaExt < L2P_NUM; laaExt++)
    {
        paaOld = gFtlL2PTbl[laaExt];

        if (paaOld.all32 != DWORD_MASK)
        {
            blk = FTL_FastGetRMPBlk(paaOld.b.ch, paaOld.b.ce, paaOld.b.lun, paaOld.b.plane, paaOld.b.block);

            D_PRINTF("T:B[%4x]LAA[%x]S[%d]\n", blk, laaExt, gFtlBlkListTbl[blk].vs.b.status);

            if ((gFtlBlkListTbl[blk].vs.b.status == FTL_BLOCK_POOL_FREE) && (gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].block != blk) && (gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].block != blk) && (gFtlCurrHeader[FTL_BLOCK_HEADER_GC][0].block != blk))
            {
                FTL_BlockPopPushList(FTL_BLOCK_POOL_USER, blk, FTL_SORT_BY_SN);
            }
            gFtlBlkListTbl[blk].vs.b.vac++;
        }
    }
}

//-------------------------------------------------------------------
// Function     : void FTL_EraseFreePool(void)
// Description  : Build Remap Table
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_EraseFreePool(void)
{
    U16 blk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE];
    D_PRINTF_FTL("ErsB ", blk);

    if (smSysInfo->d.FtlData.d.QBTBlockTag != DWORD_MASK)
    {
        smSysInfo->d.FtlData.d.QBTBlockTag = DWORD_MASK;
        gFtlMgr.QbtBackup = TRUE;
        SyIn_Synchronize(SI_AREA_MISC , SYSINFO_WRITE, SI_SYNC_BY_FTL);
    }

    while(blk != WORD_MASK)
    {
        if ((blk & 0xF) == 0)
        {
            D_PRINTF(".", blk);
        }

        FTL_EraseBankBlocks(blk);
        gFtlBlkListTbl[blk].vs.b.vac = 0;

        blk = gFtlBlkListTbl[blk].block;
    }
    D_PRINTF(" Finished\n");
}

#include "NormSection.h"
///////////////////////////////////////////////////////////
///                                                     ///
///          ********   *********    *********          ///
///          ***        ***   ***    *********          ///
///          ********   **********      ***             ///
///          ***        ***    ***      ***             ///
///          ********   **********      ***             ///
///                                                     ///
///////////////////////////////////////////////////////////
//-------------------------------------------------------------------
// Function     : static void FTL_EraseBuildTable(tFTL_BUILD_TABLE_INFO *bt)
// Description  : Build Remap Table
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void FTL_EraseBuildTable(tFTL_BUILD_TABLE_INFO *bt)
{
    tPAA errPaa;
    U32 Idx, Idx1;
    U16 paaCnt, defectcnt;
    U16 AllFreeIdx, cacheIdx;//max_die, max_plane
    U16 ch, ce, lun, blk, die, plane, plistExist = FALSE, plistCnt, page, logicpage, frag;

    //L2P Table
    MEM_SET(gFtlL2PTbl, BYTE_MASK, sizeof(tPAA) * L2P_NUM);
     //Super Block Table
    MEM_SET(gFtlRemap.rmpSuperBlkTbl, BYTE_MASK, sizeof(U16) * gNfInfo.blocksPerLun * gNfInfo.totalLun);
    //Remap Bitmap
    MEM_CLR(gFtlRemap.rmpBlkBMP, sizeof(U8)*FTL_SUPPORT_MAX_BLK);

    //erase all blocks or read P-list from SPI
    Core_JobWait();

    if (smSysInfo->d.Header.d.SubHeader.d.PList1.d.dwValidTag == SI_TAG_PLIST1 ||
        smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwValidTag == SI_TAG_PLIST2)
    {
        plistExist = TRUE;
        D_PRINTF_FTL("PLstOK\n");
    }
    else
    {
        plistExist = FALSE;
        D_PRINTF_FTL("PLstNG\n");
    }

#if (FTL_EBT_BUILD == TRUE)
    if (plistExist == FALSE)
    {
        //Erase Build Super Block Table
        D_PRINTF("MxB[%4x]\n", gNfInfo.blocksPerPlane);
        for (blk = FTL_MAX_SYSINFO_CNT; blk < gNfInfo.blocksPerPlane; blk++)
        {
            if ((blk & 0xFF) == 0)
            {
                D_PRINTF_FTL("ErsB[%4x]\n", blk);
            }

            for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
            {
                for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
                {
                    Idx = 0;
                    gFtlBTJob[ce] = FIO_PopJob(FIO_JOB_POOL_FREE);
                    gFtlBTJob[ce]->cmd   = FIO_JOB_CMD_ERASE;
                    gFtlBTJob[ce]->plane = gNfInfo.planesPerLun;
                    gFtlBTJob[ce]->paaCnt= gNfInfo.numChannels * gNfInfo.planesPerLun;
                    gFtlBTJob[ce]->flag  = FIO_F_NO_MKERR | FIO_F_MUTE;

                    FTL_NANDEraseProcess(gFtlBTJob[ce], ce, lun, blk);
                    Core_JobIssue(gFtlBTJob[ce]);
                }

                Core_JobWait();

                //check erase fail here
                for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
                {
                    for (paaCnt = 0; paaCnt < gFtlBTJob[ce]->paaCnt; paaCnt++)
                    {
                        if (FIO_CHK_ERR_BMP(gFtlBTJob[ce]->fid, paaCnt))
                        {
                            errPaa = gFtlBTJob[ce]->paa[paaCnt];
                            die = FTL_GET_DIE_NUM(errPaa.b.lun, errPaa.b.ce, errPaa.b.ch);

                            if (blk != errPaa.b.block)
                            {
                                D_PRINTF_FTL("esfatal:D[%1x]B[%4x]pB[%4x]\n", die, blk, errPaa.b.block);
                                ASSERT(FALSE);
                            }

                            Idx = FTL_GET_SBLK_IDX(errPaa.b.plane, blk, die);
                            gFtlRemap.rmpSuperBlkTbl[Idx] = FTL_BAD_BLOCK_TAG;
                            //D_PRINTF_FTL("Erase Fail:D[%2x]pn[%d]bk[%4x]I[%x]rmp[%4x]\n", die, errPaa.b.plane, blk, Idx, gFtlRemap.rmpSuperBlkTbl[Idx]);
                            FIO_CLR_ERR_BMP(gFtlBTJob[ce]->fid, paaCnt);
                        }
                    }
                }
            }
        } //for (blk = FTL_MAX_SYSINFO_CNT; blk < gNfInfo.blocksPerPlane; blk++)
    } //if (plistExist == FALSE)

    else
    {
        //Plist Build Super Block Table
        for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
        {
            for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
            {
                for (ch = 0; ch < gNfInfo.numChannels; ch++)
                {
                    if (smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwValidTag == SI_TAG_PLIST2)
                    {
                        plistCnt = smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[ch][ce][lun];

                        for (paaCnt = 0; paaCnt < plistCnt; paaCnt++)
                        {
                            blk = smSysInfo->d.PList2.wList[ch][ce][lun][paaCnt];

                            plane = blk % gNfInfo.planesPerLun;
                            blk = blk / gNfInfo.planesPerLun;

                            die = FTL_GET_DIE_NUM(lun, ce, ch);
                            Idx = FTL_GET_SBLK_IDX(plane, blk, die);
                            gFtlRemap.rmpSuperBlkTbl[Idx] = FTL_BAD_BLOCK_TAG;
                        }
                    }
                    else
                    {
                        plistCnt = smSysInfo->d.PlistCnt.d.PList1Cnt.wCount[ch][ce][lun];

                        for (paaCnt = 0; paaCnt < plistCnt; paaCnt++)
                        {
                            blk = smSysInfo->d.PList1.wList[ch][ce][lun][paaCnt];

                           //D_PRINTF("build:cnt[%d]ch[%d]ce[%d]ln[%d]blk[0x%4X]\n", paaCnt, ch, ce, lun, blk);

                            plane = blk % gNfInfo.planesPerLun;
                            blk = blk / gNfInfo.planesPerLun;

                            die = FTL_GET_DIE_NUM(lun, ce, ch);
                            Idx = FTL_GET_SBLK_IDX(plane, blk, die);
                            gFtlRemap.rmpSuperBlkTbl[Idx] = FTL_BAD_BLOCK_TAG;
                        }
                    }
                }
            }
        }

        //Glist
        if (smSysInfo->d.Header.d.SubHeader.d.GList.d.dwValidTag == SI_TAG_GLIST)
        {
            defectcnt = (smSysInfo->d.GList.d.GListBrief.wCntErase +
                         smSysInfo->d.GList.d.GListBrief.wCntWrite +
                         smSysInfo->d.GList.d.GListBrief.wCntRead);

            for (paaCnt = 0; paaCnt < defectcnt; paaCnt++)
            {
                blk = smSysInfo->d.GList.d.GList[paaCnt].wPhyBlock;
                plane = blk % gNfInfo.planesPerLun;
                blk = blk / gNfInfo.planesPerLun;

                die = smSysInfo->d.GList.d.GList[paaCnt].bDie;
                Idx = FTL_GET_SBLK_IDX(plane, blk, die);
                gFtlRemap.rmpSuperBlkTbl[Idx] = FTL_BAD_BLOCK_TAG;
            }
        }
    }

#endif

    //build super block map & update free block list
    for (die = 0; die < gNfInfo.totalLun; die++)
    {
        for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
        {
            gFtlFreeIdx[(die << gNfInfo.bitsPlane) + plane] = gNfInfo.blocksPerPlane - 1;
            //FreeIdx[die][plane] = gNfInfo.blocksPerPlane - 1;
            for (blk = FTL_MAX_SYSINFO_CNT; blk < gNfInfo.blocksPerPlane; blk++)
            {
                if(blk > gFtlFreeIdx[(die << gNfInfo.bitsPlane) + plane]) break;
                //if(blk > FreeIdx[die][plane]) break;

                Idx = FTL_GET_SBLK_IDX(plane, blk, die);
                if(gFtlRemap.rmpSuperBlkTbl[Idx] == FTL_BAD_BLOCK_TAG)
                {
                    //Idx1 = FTL_GET_SBLK_IDX(plane, FreeIdx[die][plane], die);
                    Idx1 = FTL_GET_SBLK_IDX(plane, gFtlFreeIdx[(die << gNfInfo.bitsPlane) + plane], die);
                    while(gFtlRemap.rmpSuperBlkTbl[Idx1] == FTL_BAD_BLOCK_TAG)
                    {
                        //FreeIdx[die][plane]--;
                        gFtlFreeIdx[(die << gNfInfo.bitsPlane) + plane]--;

                        //if(blk > FreeIdx[die][plane]) goto REMAP_DONE;
                        if(blk > gFtlFreeIdx[(die << gNfInfo.bitsPlane) + plane]) goto REMAP_DONE;
                        //Idx1 = FTL_GET_SBLK_IDX(plane, FreeIdx[die][plane], die);
                        Idx1 = FTL_GET_SBLK_IDX(plane, gFtlFreeIdx[(die << gNfInfo.bitsPlane) + plane], die);
                    }
                    //gFtlRemap.rmpSuperBlkTbl[Idx] = FreeIdx[die][plane];
                    gFtlRemap.rmpSuperBlkTbl[Idx]  = gFtlFreeIdx[(die << gNfInfo.bitsPlane) + plane];
                    gFtlRemap.rmpSuperBlkTbl[Idx1] = blk;//reverse map
                    gFtlRemap.rmpBlkBMP[blk] = gFtlRemap.rmpBlkBMP[gFtlFreeIdx[(die << gNfInfo.bitsPlane) + plane]] = 1;
                    //FreeIdx[die][plane]--;
                    gFtlFreeIdx[(die << gNfInfo.bitsPlane) + plane]--;
                }
                else
                {
                    if(gFtlRemap.rmpSuperBlkTbl[Idx] != WORD_MASK)
                    {
                        D_PRINTF_FTL("SBkEr:D[%2x]B[%4x]P[%1x]I[%x]R[%x]\n", die, blk, plane, Idx, gFtlRemap.rmpSuperBlkTbl[Idx]);
                        ASSERT(FALSE);
                    }
                }
REMAP_DONE:
                //if(blk > FreeIdx[die][plane]) break;
                if(blk > gFtlFreeIdx[(die << gNfInfo.bitsPlane) + plane]) break;

            }
            //D_PRINTF_FTL("Free Idx:D[%2x]PN[%d]FRIDX[%4x]\n", die, plane, gFtlFreeIdx[(die << gNfInfo.bitsPlane) + plane]);
        }
    }

    //set free block index
    AllFreeIdx = WORD_MASK;
    for (die = 0; die < gNfInfo.totalLun; die++)
    {
        for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
        {
            //AllFreeIdx = MIN(AllFreeIdx, FreeIdx[die][plane]);
            AllFreeIdx = MIN(AllFreeIdx, gFtlFreeIdx[(die << gNfInfo.bitsPlane) + plane]);
            //D_PRINTF("AllFreeIdx:%x\n", AllFreeIdx);
        }
    }

    //check FTL total super blocks
    if ((AllFreeIdx - FTL_MAX_SYSINFO_CNT - SI_MAX_TCGINFO_CNT) < ((L2P_NUM * sizeof(U32)) / (FTL_SUPER_BLOCK_SIZE)))
    {
        D_PRINTF_FTL("SBLKcnt[%d] < DEFcnt[%d]\n", (AllFreeIdx - FTL_MAX_SYSINFO_CNT - SI_MAX_TCGINFO_CNT), ((L2P_NUM << sizeof(U32)) / (FTL_SUPER_BLOCK_SIZE)));
        ASSERT(0);
    }

    gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE]   = FTL_MAX_SYSINFO_CNT;
    gFtlBlkListInfo.tail[FTL_BLOCK_POOL_FREE]   = AllFreeIdx;
    gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] = gFtlBlkListInfo.tail[FTL_BLOCK_POOL_FREE] - gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE] + 1;
    gFtlBlkListTbl[AllFreeIdx].block            = WORD_MASK;
    D_PRINTF_FTL("Free-head[%4x]tail[%4x]cnt[%d]\n", gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE], gFtlBlkListInfo.tail[FTL_BLOCK_POOL_FREE], gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE]);

    //Create TCG block
    for (Idx = 0; Idx < SI_MAX_TCGINFO_CNT; Idx++)
    {
        if (Idx >= (SI_MAX_TCGINFO_CNT >> 1) && (gNfInfo.userCapacity >= 512))
        {
            smSysInfo->d.TcgData.d.TCGBlockNo[Idx] = WORD_MASK;
        }
        else
        {
            smSysInfo->d.TcgData.d.TCGBlockNo[Idx] = FTL_BlockPopHead(FTL_BLOCK_POOL_FREE);
        }
    }
    smSysInfo->d.TcgData.d.TCGBlockTag = SI_TCGINFO_TAG;


    //Store SBLK & Free Idx to EEPROM
    MEM_COPY(smSysInfo->d.FtlRmpData.d.SBlkRmpInfo, (U8*)gFtlRemap.rmpSuperBlkTbl, sizeof(U16) * gNfInfo.blocksPerLun * gNfInfo.totalLun);

    for (die = 0; die < gNfInfo.totalLun; die++)
    {
        for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
        {
             smSysInfo->d.FtlData.d.FreeIdx[die][plane] = gFtlFreeIdx[(die << gNfInfo.bitsPlane) + plane];
        }
    }

    // Reset zeroTag is needed, once the tag reset, core will re-generate zero laa itself.
    smSysInfo->d.FtlData.d.zeroTag     = 0;
    smSysInfo->d.FtlData.d.StartIdx    = gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE];
    smSysInfo->d.FtlData.d.EndIdx      = gFtlBlkListInfo.tail[FTL_BLOCK_POOL_FREE];

    smSysInfo->d.Header.d.SubHeader.d.FTLRmp.d.dwValidTag   = SI_TAG_FTLRMP;
    smSysInfo->d.Header.d.SubHeader.d.FTLRmp.d.dwVerNo      = SI_VER_FTL;

    SyIn_Synchronize(SI_AREA_FTLRMP | SI_AREA_FTL | SI_AREA_MISC, SYSINFO_WRITE_FORCE, SI_SYNC_BY_FTL);

    if (plistExist)
        gFtlMgr.forceErase = FTL_BANK_BIT_MASK;

    #if (FTL_PARTIAL_LOAD_L2P != FALSE)
    gFtlPorPbtInfo.finishBuild = TRUE;
    #endif

    bt->prevState       = bt->buildState;
    bt->buildState      = FTL_BUILD_TABLE_COUNT;
    bt->readBlistBlock  = WORD_MASK;
}

//-------------------------------------------------------------------
// Function     : U8 FTL_RemapBadBlock(U16 logBlk)
// Description  : runtime remap bad block
// Input        : U8 die, U8 plane, U16 logBlk
// return       : TRUE: remap succeed, FLASE: can't remap
//-------------------------------------------------------------------
U8 FTL_RemapBadBlock(U8 die, U8 plane, U16 logBlk)
{
    U32 badBlkIdx = 0, blkMapIdx = 0, reMapIdx = 0;
    U16 mFreeIdx = WORD_MASK;
    U8  flg = TRUE;

    if(smSysInfo->d.FtlData.d.EndIdx >= smSysInfo->d.FtlData.d.FreeIdx[die][plane])
    {
        flg = FALSE; // Can't remap
        D_PRINTF_FTL("NO-Fidx[%4x] MAX_Blk[%4x]\n", smSysInfo->d.FtlData.d.FreeIdx[die][plane], smSysInfo->d.FtlData.d.EndIdx);
    }
    else
    {
        //build super block map & update free block list
        badBlkIdx = FTL_GET_SBLK_IDX(plane, logBlk, die);

        blkMapIdx = FTL_GET_SBLK_IDX(plane, smSysInfo->d.FtlData.d.FreeIdx[die][plane], die);

        while(gFtlRemap.rmpSuperBlkTbl[blkMapIdx] != WORD_MASK) // remaped to other block and find not remaped block(skip 0xAADD tag)
        {
            //D_PRINTF("Bad block need skip[%04x]\n", gwRmpSuperBlkTbl[idx1]);
            smSysInfo->d.FtlData.d.FreeIdx[die][plane]--;

            if(smSysInfo->d.FtlData.d.EndIdx >= smSysInfo->d.FtlData.d.FreeIdx[die][plane]) // can't remap
            {
                flg = FALSE;
                break;
            }

            blkMapIdx = FTL_GET_SBLK_IDX(plane, smSysInfo->d.FtlData.d.FreeIdx[die][plane], die);
        }

        if (flg == TRUE)
        {
            reMapIdx = blkMapIdx;

            if(gFtlRemap.rmpSuperBlkTbl[badBlkIdx] != WORD_MASK) // Remaped and mark bad
            {
                //D_PRINTF("AL-Bad block need skip[%04x]\n", gwRmpSuperBlkTbl[idx]);

                mFreeIdx                            = gFtlRemap.rmpSuperBlkTbl[badBlkIdx];
                blkMapIdx                           = FTL_GET_SBLK_IDX(plane, mFreeIdx, die);
                gFtlRemap.rmpSuperBlkTbl[blkMapIdx] = FTL_BAD_BLOCK_TAG;
            }

            gFtlRemap.rmpSuperBlkTbl[badBlkIdx] = smSysInfo->d.FtlData.d.FreeIdx[die][plane];
            gFtlRemap.rmpSuperBlkTbl[reMapIdx]  = logBlk; // reverse map
            smSysInfo->d.FtlData.d.FreeIdx[die][plane]--;

            gFtlBlkListTbl[logBlk].vs.b.bad = FALSE;

            MEM_COPY(smSysInfo->d.FtlRmpData.d.SBlkRmpInfo, (U8*)gFtlRemap.rmpSuperBlkTbl, sizeof(U16) * gNfInfo.blocksPerLun * gNfInfo.totalLun);

            D_PRINTF_FTL("D[%d]PL[%d]RMP[%4x]<->[%4x]\n", die, plane, smSysInfo->d.FtlData.d.FreeIdx[die][plane], logBlk);

            SyIn_Synchronize(SI_AREA_FTLRMP, SYSINFO_WRITE, SI_SYNC_BY_FTL);
        }
    }

    return flg;
}

//-------------------------------------------------------------------
// Function     : void FTL_BLOCK_DumpInfo(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_DumpInfo(void)
{
    U32 pool = FTL_BLOCK_POOL_MAX;
    U16 idx, i, j, blk;

#if (FTL_DUMP_BKLS == TRUE)
    for (pool = FTL_BLOCK_POOL_USER; pool < FTL_BLOCK_POOL_MAX; pool++)
    {
        if (gFtlBlkListInfo.blkCnt[pool])
        {
            blk = gFtlBlkListInfo.head[pool];

            D_PRINTF_FTL("\nPol[%2x]\n", pool);

            for (idx = 0; idx < gFtlBlkListInfo.blkCnt[pool]; idx++)
            {
                D_PRINTF(" -B[%4x]VC[%x]EC[%x]Sn[%x]Trim[%d]Typ[%d]\n", blk,
                    gFtlBlkListTbl[blk].vs.b.vac,
                    gFtlBlkListTbl[blk].eraseCnt,
                    gFtlSNTable[blk],
                    gFtlTrimInfo.trimBlocks[blk],
                    gFtlNbtEmptyBlk[blk]);

                blk = gFtlBlkListTbl[blk].block;
            }
        }
    }
#endif

#if (FTL_DUMP_INFO == TRUE)
    D_PRINTF_FTL("==FTLINFO==\n");

    D_PRINTF_FTL("-BLK-\n");
    //for (idx = 0; idx < FTL_GROUP_NUM; idx++)
    {
        D_PRINTF(" UR\tC[%4x]", gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_USER]);

        if(gFtlBlkListInfo.head[FTL_BLOCK_POOL_USER] != WORD_MASK)
        {
            D_PRINTF(" B[%4x]", gFtlBlkListInfo.head[FTL_BLOCK_POOL_USER]);

        }
        if(gFtlBlkListInfo.tail[FTL_BLOCK_POOL_USER] != WORD_MASK)
        {
            D_PRINTF(" E[%4x]", gFtlBlkListInfo.tail[FTL_BLOCK_POOL_USER]);
        }
        D_PRINTF("\n");
    }
    //for (idx = 0; idx < FTL_GROUP_NUM; idx++)
    {
        D_PRINTF(" GC\tC[%4x]", gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_GC]);

        if(gFtlBlkListInfo.head[FTL_BLOCK_POOL_GC] != WORD_MASK)
        {
            D_PRINTF(" B[%4x]", gFtlBlkListInfo.head[FTL_BLOCK_POOL_GC]);

        }
        if(gFtlBlkListInfo.tail[FTL_BLOCK_POOL_GC] != WORD_MASK)
        {
            D_PRINTF(" E[%4x]", gFtlBlkListInfo.tail[FTL_BLOCK_POOL_GC]);

        }
        D_PRINTF("\n");
    }
    D_PRINTF(" GCD\tC[%4x]", gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_GCD]);
    if(gFtlBlkListInfo.head[FTL_BLOCK_POOL_GCD] != WORD_MASK)
    {
        D_PRINTF(" B[%4x]", gFtlBlkListInfo.head[FTL_BLOCK_POOL_GCD]);
    }
    if(gFtlBlkListInfo.tail[FTL_BLOCK_POOL_GCD] != WORD_MASK)
    {
        D_PRINTF(" E[%4x]", gFtlBlkListInfo.tail[FTL_BLOCK_POOL_GCD]);
    }
    D_PRINTF("\n");
    D_PRINTF(" QBT\tC[%4x]", gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_QBT]);
    if(gFtlBlkListInfo.head[FTL_BLOCK_POOL_QBT] != WORD_MASK)
    {
        D_PRINTF(" B[%4x]", gFtlBlkListInfo.head[FTL_BLOCK_POOL_QBT]);
    }
    if(gFtlBlkListInfo.tail[FTL_BLOCK_POOL_QBT] != WORD_MASK)
    {
        D_PRINTF(" E[%4x]", gFtlBlkListInfo.tail[FTL_BLOCK_POOL_QBT]);
    }
    D_PRINTF("\n");

    D_PRINTF(" PBT\tC[%4x]", gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT]);
    if(gFtlBlkListInfo.head[FTL_BLOCK_POOL_PBT] != WORD_MASK)
    {
        D_PRINTF(" B[%4x]", gFtlBlkListInfo.head[FTL_BLOCK_POOL_PBT]);
    }
    if(gFtlBlkListInfo.tail[FTL_BLOCK_POOL_PBT] != WORD_MASK)
    {
        D_PRINTF(" E[%4x]", gFtlBlkListInfo.tail[FTL_BLOCK_POOL_PBT]);
    }
    D_PRINTF("\n");

    D_PRINTF(" Emp\tC[%4x]", gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_EMPTY]);
    if(gFtlBlkListInfo.head[FTL_BLOCK_POOL_EMPTY] != WORD_MASK)
    {
        D_PRINTF(" B[%4x]", gFtlBlkListInfo.head[FTL_BLOCK_POOL_EMPTY]);
    }
    if(gFtlBlkListInfo.tail[FTL_BLOCK_POOL_EMPTY] != WORD_MASK)
    {
        D_PRINTF(" E[%4x]", gFtlBlkListInfo.tail[FTL_BLOCK_POOL_EMPTY]);
    }
    D_PRINTF("\n");

    D_PRINTF(" FR\tC[%4x]", gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE]);
    if(gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE] != WORD_MASK)
    {
        D_PRINTF(" B[%4x]", gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE]);
    }
    if(gFtlBlkListInfo.tail[FTL_BLOCK_POOL_FREE] != WORD_MASK)
    {
        D_PRINTF(" E[%4x]", gFtlBlkListInfo.tail[FTL_BLOCK_POOL_FREE]);
    }
    D_PRINTF("\n");

    D_PRINTF(" BD\tC[%4x]", gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_BAD]);
    if(gFtlBlkListInfo.head[FTL_BLOCK_POOL_BAD] != WORD_MASK)
    {
        D_PRINTF(" B[%4x]", gFtlBlkListInfo.head[FTL_BLOCK_POOL_BAD]);
    }
    if(gFtlBlkListInfo.tail[FTL_BLOCK_POOL_BAD] != WORD_MASK)
    {
        D_PRINTF(" E[%4x]", gFtlBlkListInfo.tail[FTL_BLOCK_POOL_BAD]);
    }
    D_PRINTF("\n");

    D_PRINTF_FTL("-HED-\n");

    for (i=0; i<FTL_BLOCK_HEADER_MAX; i++)
    {
        for (j=0; j<FTL_GROUP_NUM; j++)
        {
            idx = (gFtlCurrHeader[i][j].block == WORD_MASK) ? 0 : gFtlCurrHeader[i][j].block;
            D_PRINTF(" Tp[%2x]BN[%2x]BK[%4x]PG[%3x]s[%4x]VC1[%x]EC[%x]\n",
                i,
                gFtlCurrHeader[i][j].bank,
                gFtlCurrHeader[i][j].block,
                gFtlCurrHeader[i][j].page,
                gFtlCurrHeader[i][j].sn,
                gFtlBlkListTbl[idx].vs.b.vac,
                gFtlBlkListTbl[idx].eraseCnt);
        }
    }

    D_PRINTF_FTL("-MGR-\n");
    D_PRINTF(" SN[%x]\n", gFtlMgr.SerialNumber);
    D_PRINTF(" TS[%x]\n", gFtlMgr.TableSN);
    D_PRINTF(" EC[%x]\n", gFtlMgr.AllEraseCnt);
    D_PRINTF(" FE[%x]\n", gFtlMgr.forceErase);
    D_PRINTF(" QB[%x]\n", gFtlMgr.QbtBackup);
    D_PRINTF(" EL[%x]\n", gExtLaaStart);

        D_PRINTF_FTL("-FTL INFO-\n");

    #if 0
        D_PRINTF_FTL(" sIdx:%x\n", smSysInfo->d.FtlData.d.StartIdx);
        D_PRINTF_FTL(" eIdx:%x\n", smSysInfo->d.FtlData.d.EndIdx);
    #endif

    #if (FTL_PBT_BUILD == TRUE)
        D_PRINTF_FTL("-PBT INFO-\n");
        D_PRINTF(" s[%x]\n", gFtlPbtInfo.sn);
        D_PRINTF(" b[%4x]\n", gFtlPbtInfo.block);
        D_PRINTF(" lopC:%d\n", gFtlPbtInfo.loopCnt);
        D_PRINTF(" pgeC:%d\n", gFtlPbtInfo.pageCnt);
    #endif

    {
        tPAA paa;
        uint32_t laa;

        D_PRINTF(" zTag:%x\n", smSysInfo->d.FtlData.d.zeroTag);

        for (laa = L2P_EXT_ZERO_START; laa < (L2P_EXT_ZERO_START + L2P_EXT_NUM_ZERO); laa++)
        {
            paa = FTL_PhyAddrSearch(laa);
            D_PRINTF(" ZLAA[%X]:%08X\n", laa, paa.all32);
        }
    }

    D_PRINTF_FTL("=DONE=\n");
#endif
}

//-------------------------------------------------------------------
// Function     : void FTL_DumpRemap(U8 buildBitmap)
// Description  :
// Input        : SetReMap = TRUE,set remap bit map.
// return       : N/A
//-------------------------------------------------------------------
void FTL_DumpRemap(U8 buildBitmap)
{
    U32 idx;
    U16 blk;
    U8  die, plane, ch, ce, lun;

    D_PRINTF_FTL("=FTL RMP=\n");
    for (die = 0; die < gNfInfo.totalLun; die++)
    {
        for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
        {
            for (blk = 0; blk < gNfInfo.blocksPerPlane; blk++)
            {
                idx = FTL_GET_SBLK_IDX(plane, blk, die);

                if (gFtlRemap.rmpSuperBlkTbl[idx] != WORD_MASK)
                {
                    if (buildBitmap == TRUE)
                    {
                        if (gFtlRemap.rmpSuperBlkTbl[idx] < FTL_SUPPORT_MAX_BLK)
                        {
                            gFtlRemap.rmpBlkBMP[blk] = TRUE;
                        }
                        else if ((gFtlRemap.rmpSuperBlkTbl[idx] >= FTL_SUPPORT_MAX_BLK) && (gFtlRemap.rmpSuperBlkTbl[idx] != FTL_BAD_BLOCK_TAG))
                        {
                            ch  = die & FTL_CH_MASK;
                            ce  = (die >> FTL_CH_SHIFT) & FTL_CE_MASK;
                            lun = (die >> gNfInfo.bitsChDev) & FTL_LUN_MASK;
                            D_PRINTF("Err:CH[%d]CE[%d]LN[%d]PN[%1x]B[%4x]I[%X]=[%4x]\n", ch, ce, lun, plane, blk, idx, gFtlRemap.rmpSuperBlkTbl[idx]);
                        }
                    }
                    else
                    {
                        ch  = die & FTL_CH_MASK;
                        ce  = (die >> FTL_CH_SHIFT) & FTL_CE_MASK;
                        lun = (die >> gNfInfo.bitsChDev) & FTL_LUN_MASK;
                        D_PRINTF("Rmp:CH[%d]CE[%d]LN[%d]PN[%1x]B[%4x]I[%X]=[%4x]\n", ch, ce, lun, plane, blk, idx, gFtlRemap.rmpSuperBlkTbl[idx]);
                    }
                }
            }
        }
    }

}

//-------------------------------------------------------------------
// Function     : void FTL_DumpL2P(U32 laa, U32 count)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_DumpL2P(U32 laa, U32 count)
{
    tPAA    paa;
    U32     i;
    U16     blk;

    for (i = laa; i < (laa + count); i++)
    {
        paa = gFtlL2PTbl[i];
        if (paa.all32 != DWORD_MASK)
        {
            blk = FTL_FastGetRMPBlk(paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block);
            //if(gReMapBlk[paa.b.block])
            //    blk = FTL_GetRemapBlock(paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block);
            //else
            //    blk = paa.b.block;
            //die = (paa.b.lun << (FTL_CE_SHIFT + FTL_CH_SHIFT)) | (paa.b.ce << (FTL_CH_SHIFT)) | paa.b.ch;
            D_PRINTF("L2P[%x<=>%x]CH[%1x]CE[%1x]L[%1X]PL[%1x]B[%4x]PA[%3x]F[%1x]LogiB[%4x]\n", i, paa.all32, paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, paa.b.page, paa.b.frag,blk);
        }
        //die = (paa.b.lun << (FTL_CE_SHIFT + FTL_CH_SHIFT)) | (paa.b.ce << (FTL_CH_SHIFT)) | paa.b.ch;
       // D_PRINTF("L2P[%x = %x] CH:%1x, CE:%1x, L:%1X, PL:%4x, B:%4x, PA:%4x, F:%4x\n", i, paa.all32, paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, paa.b.page, paa.b.frag);
    }
}

//-------------------------------------------------------------------
// Function     : void FTL_DumpVAC(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_DumpVAC(void)
{
    tPAA paa;
    U32 currLaa, totalVac = 0;
    U16 blk;

    if ((gFtlPorPbtInfo.finishBuild != TRUE) && (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_QBT] > 0))
    {
        FTL_PorPbtLaaZone(0, L2P_MASK);
    }

    for (blk = 0; blk < FTL_SUPPORT_MAX_BLK; blk++)
    {
        gFtlFastVacBuf[blk] = 0;
    }

    for (currLaa = 0; currLaa < L2P_NUM; currLaa++)
    {
        if ((currLaa & 0x00FFFFFF) == 0)
            D_PRINTF_FTL("Laa:%x\n", currLaa);

        paa = gFtlL2PTbl[currLaa];
        if (paa.all32 != DWORD_MASK)
        {
            blk = FTL_FastGetRMPBlk(paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block);

            if (blk > smSysInfo->d.FtlData.d.EndIdx)
            {
                D_PRINTF_FTL("Err B[%4x]currLaa[%x]-PyB[%4x]CH[%d]CE[%d]LN[%d]PN[%d]\n", blk, currLaa, paa.b.block, paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane);
            }
            else
            {
                gFtlFastVacBuf[blk]++;
            }
        }
    }

    for (blk=smSysInfo->d.FtlData.d.StartIdx; blk < (smSysInfo->d.FtlData.d.EndIdx + 1); blk++)
    {
            if ((gFtlFastVacBuf[blk] != gFtlBlkListTbl[blk].vs.b.vac) ||
                (gFtlBlkListTbl[blk].vs.b.vac > FTL_MAX_VAC) ||
                (gFtlFastVacBuf[blk] > FTL_MAX_VAC))
            {
                D_PRINTF_FTL("VAC ChkEr:B[%4x]vc[%x]act[%x]mx[%x]\n", blk,
                    gFtlBlkListTbl[blk].vs.b.vac,
                    gFtlFastVacBuf[blk],
                    FTL_MAX_VAC);

                //if (gdwPerfVacBuf[blk] < FTL_MAX_VAC)
                //  gFtlBlkListTbl[blk].vs.b.vac = gdwPerfVacBuf[blk];
            }
            else
            {
                totalVac += gFtlFastVacBuf[blk];
            }
    }

    D_PRINTF_FTL("VAC ChkDn total[%x] L2p[%x]\n", totalVac, L2P_NUM);

}

//-------------------------------------------------------------------
// Function     : void FTL_DumpTrimInfo(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_DumpTrimInfo(void)
{
    U32 laaidx, count = 0;

    D_PRINTF_FTL("Trim Info\n");

    for (laaidx = 0; laaidx < (L2P_NUM >> 5); laaidx++)
    {
        count += BIT_COUNT(gFtlTrimInfo.trimBMP[laaidx]);
    }

    D_PRINTF_FTL("BMPc[%d]:[%d]BKc[%d]update[%d]\n", gFtlTrimInfo.trimBMPcnt, count, gFtlTrimInfo.trimBlockcnt, gFtlTrimInfo.trimUpdate);

    /*D_PRINTF_FTL("BMP:\n");
    for (laaidx = 0; laaidx < (L2P_NUM >> 5); laaidx++)
    {
        if (gFtlTrimInfo.trimBMP[laaidx] != 0)
        {
            D_PRINTF("BMP[%x]:[%x]\n", laaidx, gFtlTrimInfo.trimBMP[laaidx]);
        }
    }*/

    D_PRINTF_FTL("PBTpg\n");
    //D_PRINTF("[%x]vs[%x]\n", (((gNfInfo.pagesPerBlock << FTL_BANK_SHIFT) * FTL_QBT_BLOCK_COUNT) >> 5), (L2P_NUM/FTL_LAA_BANK_PER_PAGE));
    for (laaidx = 0; laaidx < (((gNfInfo.pagesPerBlock << FTL_BANK_SHIFT) * FTL_QBT_BLOCK_COUNT) >> 5); laaidx++)
    {
        if (gFtlTrimInfo.trimPbtPG[laaidx] != 0)
        {
            D_PRINTF("pg[%x]:[%x]\n", laaidx, gFtlTrimInfo.trimPbtPG[laaidx]);
        }
    }

    /*D_PRINTF_FTL("Blocks\n");
    for (blk = 0; blk < FTL_SUPPORT_MAX_BLK; blk++)
    {
        if (gFtlTrimInfo.trimBlocks[blk] == TRUE)
        {
            D_PRINTF("TrimBlock[%4x]\n", blk);
        }
    }*/
}

#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    One time initialization for FTL sub-system.\n

    @param[in]  initMode   boot mode option

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t FTL_OneTimeInit(InitBootMode_t initMode)
{
#if (_MEDIA_ == MEDIA_NAND)
    tFTL_BUILD_TABLE_INFO ftlBTinfo;
    U32 currTickAll;

    // SSD count check
    ASSERT2(FTL_MAX_CACHE_SSD_USAGE < SSD_OTHER_COUNT, FTL_MAX_CACHE_SSD_USAGE);
    // PBT period more than 2 times of bank number
    ASSERT2(FTL_PBT_PERIOD_BLK >= (FTL_BANK_NUM * 2), FTL_PBT_PERIOD_BLK);

    currTickAll = HalTimer_ReadTick();

    FTL_NandInfoInit();
    FTL_GlobalVarAlloc();
    FTL_GlobalVarInit(FTL_INITIAL_POWER_ON);

    if ((smSysInfo->d.Header.d.SubHeader.d.FTL.d.dwValidTag) != SI_TAG_FTL || (smSysInfo->d.Header.d.SubHeader.d.FTLRmp.d.dwValidTag != SI_TAG_FTLRMP) || (smOverrideFTL == FTL_OVERRIDE_TAG))
    {
        D_PRINTF_FTL("Tng=>EBT tag:FTL[%x]RMP[%x]OVR[%x]\n", smSysInfo->d.Header.d.SubHeader.d.FTL.d.dwValidTag, smSysInfo->d.Header.d.SubHeader.d.FTLRmp.d.dwValidTag, smOverrideFTL);

        ftlBTinfo.buildState        = FTL_BUILD_TABLE_ERASE;
        ftlBTinfo.prevState         = FTL_BUILD_TABLE_COUNT;
        ftlBTinfo.readBlistBlock    = WORD_MASK;
    }
    else
    {
        if ((smSysInfo->d.FtlData.d.StartIdx < FTL_MAX_SYSINFO_CNT) || (smSysInfo->d.FtlData.d.EndIdx >= FTL_SUPPORT_MAX_BLK))
        {
            D_PRINTF_FTL("SI B[S:%4x-E:%4x]ng=>EBT\n", smSysInfo->d.FtlData.d.StartIdx, smSysInfo->d.FtlData.d.EndIdx);

            ftlBTinfo.buildState        = FTL_BUILD_TABLE_ERASE;
            ftlBTinfo.prevState         = FTL_BUILD_TABLE_COUNT;
            ftlBTinfo.readBlistBlock    = WORD_MASK;
        }
        else
        {
            ftlBTinfo.buildState        = FTL_BUILD_TABLE_NORMAL;
            ftlBTinfo.prevState         = FTL_BUILD_TABLE_COUNT;
            ftlBTinfo.readBlistBlock    = WORD_MASK;
        }
    }

    gFtlPowerOn = TRUE;

    while (ftlBTinfo.buildState != FTL_BUILD_TABLE_COUNT)
    {
        gCbfBuildTableState[ftlBTinfo.buildState](&ftlBTinfo);
    }

    gFtlPowerOn = FALSE;

    if (ftlBTinfo.prevState == FTL_BUILD_TABLE_ERASE)
    {
        while (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE])
        {
            FTL_BlockPopPushList(FTL_BLOCK_POOL_EMPTY, gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE], FTL_SORT_BY_EC);
        }

        while (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_EMPTY])
        {
            FTL_BlockPopPushList(FTL_BLOCK_POOL_FREE, gFtlBlkListInfo.head[FTL_BLOCK_POOL_EMPTY], FTL_SORT_BY_EC);
        }

        FTL_EraseFreePool();

        smSysInfo->d.Header.d.SubHeader.d.FTL.d.dwValidTag      = SI_TAG_FTL;
        smSysInfo->d.Header.d.SubHeader.d.FTL.d.dwVerNo         = SI_VER_FTL;
        SyIn_Synchronize(SI_AREA_MISC, SYSINFO_WRITE_FORCE, SI_SYNC_BY_FTL);
    }

    #if (FTL_DUMP_SBLK == TRUE) //dump SBlk Info
        FTL_DumpRemap(FALSE);
    #endif
    #if (FTL_DUMP_FTL == TRUE) //dump SBlk Info
        //if (smSysInfo->d.FtlData.d.QBTBlockTag != SI_QBTINFO_TAG)
            FTL_DumpInfo();
    #endif

    if (ftlBTinfo.prevState == FTL_BUILD_TABLE_READ_OPENBK)
    {
        U16 blk;

        gFtlMgr.QbtBackup = TRUE;

        for (blk = 0; blk < FTL_SUPPORT_MAX_BLK; blk++)
        {
            smSysInfo->d.ECnt.d.EraseCount[blk] = gFtlBlkListTbl[blk].eraseCnt;
        }
        SyIn_Synchronize(SI_AREA_ECNT, SYSINFO_WRITE_FORCE, SI_SYNC_BY_FTL);

        MEM_CLR(gFtlNbtEmptyBlk, sizeof(U8)*FTL_SUPPORT_MAX_BLK);

        D_PRINTF_FTL("Build Total[%d]ms\n", HalTimer_GetElapsedTimeMsec(currTickAll));
        gFtlSporInfo.scantime_sec = (HalTimer_GetElapsedTimeMsec(currTickAll)/ 1000);// msec-> sec
    }

    if (smOverrideFTL == FTL_OVERRIDE_TAG)
    {
        smOverrideFTL = DWORD_MASK;
        Purge_Center(PURGE_NOR_FTLOVERRIDE);
    }
#endif

    gFtlMgr.forceErase = FTL_BANK_BIT_MASK;

    return(cEcNoError);
}

//-------------------------------------------------------------------
// Function     : void FTL_ClearHeaderBlock(U8 Header,U8 Group)
// Description  : Reset FTL current header.
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_ClearHeaderBlock(U8 header,U8 group)
{
    gFtlCurrHeader[header][group].sn     = 0;
    gFtlCurrHeader[header][group].block  = WORD_MASK;
    gFtlCurrHeader[header][group].bank   = 0;
    gFtlCurrHeader[header][group].page   = 0;
}

#include "PerfSection.h"
//-------------------------------------------------------------------
// Function     : U16 FTL_GetRemapBlock(U8 ch, U8 ce, U8 lun, U8 plane, U16 block)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
U16 FTL_GetRemapBlock(U8 ch, U8 ce, U8 lun, U8 plane, U16 block)
{
    U32 idx;
    U16 rmpBK, die;

    die     = FTL_GET_DIE_NUM(lun, ce, ch);
    idx     = FTL_GET_SBLK_IDX(plane, block, die);
    rmpBK   = gFtlRemap.rmpSuperBlkTbl[idx];

    rmpBK   = (rmpBK == WORD_MASK) ? block : rmpBK;

    return rmpBK;
}

//-------------------------------------------------------------------
// Function     : U16 FTL_BLOCK_PopHead(U16 pool)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
U16 FTL_BlockPopHead(U16 pool)
{
    U16     block = WORD_MASK;

    if (gFtlBlkListInfo.blkCnt[pool])
    {
        block = gFtlBlkListInfo.head[pool];

        if (block == gFtlBlkListInfo.tail[pool])
        {
            gFtlBlkListInfo.head[pool] = gFtlBlkListInfo.tail[pool] = WORD_MASK;
        }
        else
        {
            gFtlBlkListInfo.head[pool] = gFtlBlkListTbl[block].block;
        }

        gFtlBlkListTbl[block].block = WORD_MASK;
        gFtlBlkListInfo.blkCnt[pool]--;
    }

    if (pool == FTL_BLOCK_POOL_FREE)
    {
        if (gFtlBlkListTbl[block].vs.b.vac)
        {
            D_PRINTF("POPHD:B[%4x]vac[%x]\n", block, gFtlBlkListTbl[block].vs.b.vac);
        }

        #if (ENABLE_GC == TRUE)
        GC_ModeAssert(pool, block);
        #else
        D_PRINTF_FTL("FB[%4x]UB[%4x]B[%4x]P[%d]EC[%2d]T[%d]\n",
            gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE],
            gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_USER],
            block,
            pool,
            gFtlBlkListTbl[block].eraseCnt,
            HalTempSensor_GetTemperature()
        );
        #endif
    }

    return block;
}

//-------------------------------------------------------------------
// Function     : void FTL_BlockPopList(U16 pool, U16 block)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_BlockPopList(U16 pool, U16 block)
{
    U16     prevBlk, currBlk;

    if (gFtlBlkListTbl[block].vs.b.status != pool)
    {
        D_PRINTF_FTL("PopEr:B[%4x]s[%d]pol[%d]\n", block, gFtlBlkListTbl[block].vs.b.status, pool);
        ASSERT(FALSE);
    }

    if (block >= gNfInfo.blocksPerPlane)
    {
        D_PRINTF_FTL("Pop-pol[%d]b[%4x]hd[%4x]tl[%4x]\n", pool, block, gFtlBlkListInfo.head[pool], gFtlBlkListInfo.tail[pool]);
        ASSERT(block < gNfInfo.blocksPerPlane);
    }

    if (gFtlBlkListInfo.blkCnt[pool])
    {
        if (block == gFtlBlkListInfo.head[pool] && block == gFtlBlkListInfo.tail[pool])
        {
            gFtlBlkListInfo.head[pool] = gFtlBlkListInfo.tail[pool] = WORD_MASK;
        }
        else if (block == gFtlBlkListInfo.head[pool])
        {
            gFtlBlkListInfo.head[pool]  = gFtlBlkListTbl[block].block;
            gFtlBlkListTbl[block].block = WORD_MASK;
        }
        else
        {
            prevBlk = gFtlBlkListInfo.head[pool];
            currBlk = gFtlBlkListTbl[prevBlk].block;
            while(block != currBlk)
            {
                if (currBlk == gFtlBlkListInfo.tail[pool])
                {
                    FTL_DumpInfo();
                    D_PRINTF_FTL("\nPop-pol[%d]b[%4x]hd[%4x]tl[%4x]\n", pool, block, gFtlBlkListInfo.head[pool], gFtlBlkListInfo.tail[pool]);
                    ASSERT(FALSE);
                    //return;//not found
                }
                prevBlk = currBlk;
                currBlk = gFtlBlkListTbl[currBlk].block;
            }
            gFtlBlkListTbl[prevBlk].block = gFtlBlkListTbl[currBlk].block;

            if (block == gFtlBlkListInfo.tail[pool])
            {
                gFtlBlkListInfo.tail[pool] = prevBlk;
            }
        }

        gFtlBlkListTbl[block].block = WORD_MASK;
        gFtlBlkListInfo.blkCnt[pool]--;
    }
}

//-------------------------------------------------------------------
// Function     : void FTL_BlockPushList(U16 pool, U16 block, U16 sortRule, Cstr_t funcName)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_BlockPushList(U16 pool, U16 block, U16 sortRule)
{
    U32     compValue, currValue;
    U16     prevBlk, currBlk, i = 0;

    #if DBG_GC_VAC
    if(pool == FTL_BLOCK_POOL_FREE)
    {
        D_PRINTF_FTL("Psh-FreePool[%d]b[%4x]hd[%4x]tl[%4x]stRl[%d]vac[%x]\n",
                        pool, block, gFtlBlkListInfo.head[pool], gFtlBlkListInfo.tail[pool],
                        sortRule,gFtlBlkListTbl[block].vs.b.vac);
    }
    #endif
    if (block >= gNfInfo.blocksPerPlane)
    {
        D_PRINTF_FTL("Psh-pol[%d]b[%4x]hd[%4x]tl[%4x]stRl[%d]\n", pool, block, gFtlBlkListInfo.head[pool], gFtlBlkListInfo.tail[pool], sortRule);
        FTL_DumpInfo();
        ASSERT(block < gNfInfo.blocksPerPlane);
    }

    if (sortRule >= FTL_SORT_MAX)
    {
        D_PRINTF_FTL("ErSt:%d\n", sortRule);
        ASSERT(sortRule < FTL_SORT_MAX);
    }

    if ((gFtlBlkListInfo.blkCnt[pool] > 0) && (gFtlPowerOn == TRUE))
    {
        currBlk = gFtlBlkListInfo.head[pool];
        while (currBlk != WORD_MASK)
        {
            if (currBlk == block)
            {
                D_PRINTF("B[%4x]inP[%d]\n", block, pool);
                return;
            }
            currBlk = gFtlBlkListTbl[currBlk].block;
        }
    }

    if (gFtlBlkListInfo.blkCnt[pool] < gNfInfo.blocksPerDevice)
    {
        if (gFtlBlkListInfo.tail[pool] == WORD_MASK)//empty list
        {
            gFtlBlkListInfo.head[pool] = gFtlBlkListInfo.tail[pool] = block;
            gFtlBlkListTbl[block].block = WORD_MASK;
        }
        else
        {
            if (sortRule == FTL_SORT_NONE)//push tail
            {
                gFtlBlkListTbl[gFtlBlkListInfo.tail[pool]].block = block;
                gFtlBlkListTbl[block].block = WORD_MASK;
                gFtlBlkListInfo.tail[pool]  = block;
                #if DBG_GC_VAC
                if(pool == FTL_BLOCK_POOL_FREE)D_PRINTF("\tBlk[%X] push2Tail\n",block);
                #endif
            }
            else if(sortRule == FTL_SORT_BY_EC)//skip head block
            {
                prevBlk = gFtlBlkListInfo.head[pool];

                if (prevBlk != gFtlBlkListInfo.tail[pool])
                {
                    currBlk = gFtlBlkListTbl[prevBlk].block;

                    compValue = gFtlBlkListTbl[block].eraseCnt;
                    i++;

                    while(1)
                    {
                        currValue = gFtlBlkListTbl[currBlk].eraseCnt; // ec of block on the pool

                        if ((i >= FTL_INSERT_PROTECT_AREA) && (compValue < currValue)) // ec ascending sort
                        {
                            break;
                        }
                        else if (currBlk == gFtlBlkListInfo.tail[pool]) // the end of pool
                        {
                            prevBlk = gFtlBlkListInfo.tail[pool];
                            break;
                        }
                        prevBlk = currBlk;
                        currBlk = gFtlBlkListTbl[currBlk].block;
                        i++;
                    }
                }

                if (prevBlk == gFtlBlkListInfo.tail[pool])//push tail
                {
                    gFtlBlkListTbl[gFtlBlkListInfo.tail[pool]].block = block;
                    gFtlBlkListTbl[block].block = WORD_MASK;
                    gFtlBlkListInfo.tail[pool] = block;
                }
                else // insert
                {
                    gFtlBlkListTbl[prevBlk].block = block;
                    gFtlBlkListTbl[block].block = currBlk;
                }
            }
            else if (sortRule == FTL_SORT_BY_HEAD)
            {
                gFtlBlkListTbl[block].block = gFtlBlkListInfo.head[pool];
                gFtlBlkListInfo.head[pool] = block;
            }
            else if(sortRule == FTL_SORT_BY_SN)//not skip head block
            {
                prevBlk     = gFtlBlkListInfo.head[pool];
                currBlk     = gFtlBlkListTbl[prevBlk].block;
                compValue   = gFtlSNTable[block]; // push block

                if (compValue < gFtlSNTable[prevBlk])
                {
                    gFtlBlkListTbl[block].block = prevBlk;
                    gFtlBlkListInfo.head[pool] = block;
                }
                else
                {
                    if (prevBlk != gFtlBlkListInfo.tail[pool])
                    {
                        while(1)
                        {
                            currValue = gFtlSNTable[currBlk];

                            if (compValue < currValue)
                            {
                                break;
                            }
                            else if (currBlk == gFtlBlkListInfo.tail[pool])
                            {
                                prevBlk = gFtlBlkListInfo.tail[pool];
                                break;
                            }
                            else
                            {
                                prevBlk = currBlk;
                                currBlk = gFtlBlkListTbl[currBlk].block;
                            }
                        }
                    }

                    if (prevBlk == gFtlBlkListInfo.tail[pool]) //push tail
                    {
                        gFtlBlkListTbl[gFtlBlkListInfo.tail[pool]].block = block;
                        gFtlBlkListTbl[block].block = WORD_MASK;
                        gFtlBlkListInfo.tail[pool] = block;
                    }
                    else
                    {
                        gFtlBlkListTbl[prevBlk].block = block;
                        gFtlBlkListTbl[block].block = currBlk;
                    }
                }
            }
            else
            {
                FTL_DumpInfo();
                D_PRINTF_FTL("Psh-pol[%d]b[%4x]hd[%4x]tl[%4x]stRl[%d]\n", pool, block, gFtlBlkListInfo.head[pool], gFtlBlkListInfo.tail[pool], sortRule);
                ASSERT(FALSE);
            }
        }

        //D_PRINTF("pool[%d]BK[%4x]s[%x]>>", pool, block, gFtlBlkListTbl[block].vs.b.status);
        gFtlBlkListInfo.blkCnt[pool]++;
        gFtlBlkListTbl[block].vs.b.status = pool;
        //D_PRINTF("s[%x]\n", gFtlBlkListTbl[block].vs.b.status);
    }
    else
    {
        FTL_DumpInfo();
        D_PRINTF_FTL("Psh-pol[%d]b[%4x]hd[%4x]tl[%4x]stRl[%d]\n", pool, block, gFtlBlkListInfo.head[pool], gFtlBlkListInfo.tail[pool], sortRule);
        ASSERT(FALSE);
    }

    if ((pool == FTL_BLOCK_POOL_FREE) && (GC_MODE_CHECK))
    {
        #if (ENABLE_GC == TRUE)
        GC_ModeDeassert();
        #endif
    }
}

//-------------------------------------------------------------------
// Function     : void FTL_BlockPopPushList(U16 pool, U16 block, U16 sortRule)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_BlockPopPushList(U16 pool, U16 block, U16 sortRule)
{
    FTL_BlockPopList(gFtlBlkListTbl[block].vs.b.status, block);
    FTL_BlockPushList(pool, block, sortRule);
}

//-------------------------------------------------------------------
// Function     : void FTL_PurgePool2Free(U16 pool)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_PurgePool2Free(U16 pool)
{
    U16 blk;

    D_PRINTF_FTL("Pol[%d]cnt[%04d] to free\n", pool, gFtlBlkListInfo.blkCnt[pool]);

    while (gFtlBlkListInfo.blkCnt[pool])
    {
        blk = FTL_BlockPopHead(pool);

        if (pool == FTL_BLOCK_POOL_USER)
        {
            gFtlBlkListTbl[blk].vs.b.vac    = 0;
            gFtlBlkListTbl[blk].vs.b.host   = 0;
        }

        FTL_BlockPushList(FTL_BLOCK_POOL_FREE, blk, FTL_SORT_BY_EC);
    }
    //D_PRINTF_FTL("Pol[%d]cnt[%d]\n", pool, gFtlBlkListInfo.blkCnt[pool]);
}

//-------------------------------------------------------------------
// Function     : void FTL_PurgePbt2Free(void)
// Description  : Release blocks of parial build table
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_PurgePbt2Free(void)
{
    U16 blk;

    while (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT] > FTL_QBT_BLOCK_COUNT)
    {
        blk = FTL_BlockPopHead(FTL_BLOCK_POOL_PBT);
        FTL_BlockPushList(FTL_BLOCK_POOL_FREE, blk, FTL_SORT_BY_EC);
        D_PRINTF_FTL("PugPbt2F[%4x]\n", blk);
    }
    D_PRINTF("PBT[%d]\n", gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT]);
}

//-------------------------------------------------------------------
// Function     : U8 FTL_QuickOrPartialBuildL2P(U32 buildType, U8 zone)
// Description  : L2P Table build by QBT or PBT
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
U8 FTL_QuickOrPartialBuildL2P(U32 buildType, U8 zone)
{
    U32 l2pidx = 0, Idx = 0;
    U16 blk, loop, pageS, pageE, range;
    U8  err = TRUE;

    ASSERT2(buildType > FTL_BUILD_TYPE_NONE && buildType < FTL_BUILD_TYPE_MAX, buildType);

    Core_JobWait();

    if (buildType == FTL_BUILD_TYPE_QBT)
    {
        blk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_QBT];
    }
    else
    {
        blk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_PBT];
    }

    for (loop = 0; loop < FTL_QBT_BLOCK_COUNT; loop++)
    {
        if ((buildType == FTL_BUILD_TYPE_QBT) && (FTL_PARTIAL_LOAD_L2P != FALSE))
        {
            range   = (gNfInfo.pagesPerBlock * FTL_QBT_BLOCK_COUNT) / FTL_POR_PBT_ZONE_NUM;
            //pageS   = (range * zone) & (gNfInfo.pagesPerBlock - 1);
            Idx   = (range * zone);
            l2pidx  = FTL_LAA_PER_PAGE * Idx;

            if (FTL_QBT_BLOCK_COUNT > 1)
            {
                if (zone >= (FTL_POR_PBT_ZONE_NUM >> 1))
                {
                    zone = zone - (FTL_POR_PBT_ZONE_NUM >> 1);
                    blk = gFtlBlkListTbl[blk].block;
                }
            }
            pageS = range * zone;
            pageE = pageS + range;
            //l2pidx  = (loop + 1) * FTL_LAA_PER_PAGE * pageS;
        }
        else
        {
            pageS   = 0;
            pageE   = gNfInfo.pagesPerBlock;
            l2pidx  = FTL_GET_QBTL2PIDX(loop);
        }

        D_PRINTF_FTL("BT[%d]B[%4x]pageS[%3x]pageE[%3x]loop[%d]l2pIdx[%x]zone[%2x]\n", buildType, blk, pageS, pageE, loop, l2pidx, zone);
        err = FTL_NANDReadL2P(blk, loop, pageS, pageE, l2pidx);

        if (err == FALSE) break;

        blk = gFtlBlkListTbl[blk].block;

        if ((FTL_PARTIAL_LOAD_L2P != FALSE) && (gFtlPorPbtInfo.finishBuild != TRUE))
            break;
    }

    return err;
}

//-------------------------------------------------------------------
// Function     : U8 FTL_DecreaseVC(tPAA paa)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
U8 FTL_DecreaseVC(tPAA paa)
{
    U16 blk;

    //D_PRINTF("table sn[%x]\n", sn);

    blk = FTL_FastGetRMPBlk(paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block);

    if (gFtlBlkListTbl[blk].vs.b.vac)
    {
        if (--gFtlBlkListTbl[blk].vs.b.vac == 0)
        {
            #ifdef DBG_GC_CMD_SENSOR
            D_PRINTF_FTL("VC[0]B[%4x]FBC[%d]\n", blk,gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE]);
            #endif

            #if (ENABLE_GC == TRUE)
            if (gFtlPowerOn != TRUE)
            {
                if ((gFtlBlkListTbl[blk].vs.b.status == FTL_BLOCK_POOL_USER))  //exclude GC & GCD list)
                {
                    if ((gFtlTrimInfo.trimBlocks[blk] != TRUE) && (gFtlSNTable[blk] < gFtlMgr.TableSN))
                    {
                        D_PRINTF("B[%4x]sn[%x]toFree Tblsn[%x]\n", blk, gFtlSNTable[blk], gFtlMgr.TableSN);
                        FTL_BlockPopPushList(FTL_BLOCK_POOL_FREE, blk, FTL_SORT_BY_EC);
                    }
                    else
                    {
                        D_PRINTF("B[%4x]sn[%x]toEmpty Tblsn[%x]\n", blk, gFtlSNTable[blk], gFtlMgr.TableSN);
                        FTL_BlockPopPushList(FTL_BLOCK_POOL_EMPTY, blk, FTL_SORT_NONE);
                    }
                }
                else if (gFtlBlkListTbl[blk].vs.b.status == FTL_BLOCK_POOL_GCD)
                {
                    if ((blk != gGCppBuf[0].blk_Info.blk) && (blk != gGCppBuf[1].blk_Info.blk))
                    {
                        //D_PRINTF("R2 [%4x]\n", blk);
                        GC_FreeBlk(blk);
                    }
                }
            }
            #else
            FTL_BlockPopList(gFtlBlkListTbl[blk].vs.b.status, blk);
            FTL_BlockPushList(FTL_BLOCK_POOL_FREE, blk, FTL_SORT_BY_EC);
            #endif
        }
        //D_PRINTF("blk[%4x]vac[%x]\n", blk, gFtlBlkListTbl[blk].vs.b.vac);
    }
    #if (FTL_VAC_CHEK == TRUE)
    else
    {
        if (gFtlPowerOn != TRUE)
        {
            D_PRINTF_FTL("VCer:B[%4x]PAA[%x]VC[%x]\n", blk, paa.all32, gFtlBlkListTbl[blk].vs.b.vac);
            ASSERT(0);
            return TRUE;
        }
    }
    #endif

    //D_PRINTF("DVAC:B[%4x]vac[%x]\n", blk, gFtlBlkListTbl[blk].vs.b.vac);

    return FALSE;
}

//-------------------------------------------------------------------
// Function     : FTL_ReleaseEmptyPool(U32 tablesn)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_ReleaseEmptyPool(void)
{
    U32 releaseCnt = 0, limitation = 0;
    U16 blk, nextblk;

    blk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_EMPTY];

    while (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_EMPTY] && (blk <= smSysInfo->d.FtlData.d.EndIdx))
    {
        nextblk = gFtlBlkListTbl[blk].block;

        if ((gFtlTrimInfo.trimBlocks[blk] != TRUE) && (gFtlSNTable[blk] < gFtlMgr.TableSN))
        {
            FTL_BlockPopPushList(FTL_BLOCK_POOL_FREE, blk, FTL_SORT_BY_EC);
            D_PRINTF_FTL("RelFreeBK[%4x],sn[%x],sn1[%x]\n", blk, gFtlSNTable[blk], gFtlMgr.TableSN);
        }

        if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] < gFtlTrimRatio)
        {
            limitation = ((gFtlTrimRatio - gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE]) >> FTL_BANK_SHIFT);
        }
        else
        {
            limitation = GC_BLKCNT_DEACTIVE;
        }

        if (releaseCnt++ > limitation) break;
        blk = nextblk;
    }

    if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_EMPTY] == 0)
    {
        gFtlMgr.ReleaseBlk = FALSE;
    }

    //D_PRINTF("tablesn[%x]BC[%4x]rc[%4x]rb[%d]\n", tablesn, gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_EMPTY], releaseCnt, gFtlMgr.ReleaseBlk);
}

//-------------------------------------------------------------------
// Function     : void FTL_CopyFtlInfoLastPageBuf(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_CopyFtlInfoLastPageBuf(void)
{
    MemCopyNU32((U32*)&gFtlLastPageBuf.lastPageBufFtlInfo[LP_OFFSET_BLKTBL],         (U32*)gFtlBlkListTbl,   LP_SIZE_BLKTBL);
    MemCopyNU32((U32*)&gFtlLastPageBuf.lastPageBufFtlInfo[LP_OFFSET_BLKINFO],        (U32*)&gFtlBlkListInfo, LP_SIZE_BLKINFO);
    MemCopyNU32((U32*)&gFtlLastPageBuf.lastPageBufFtlInfo[LP_OFFSET_HEADINFO],       (U32*)gFtlCurrHeader,   LP_SIZE_HEADINFO);
    MemCopyNU32((U32*)&gFtlLastPageBuf.lastPageBufFtlInfo[LP_OFFSET_MGRVAR],         (U32*)&gFtlMgr,         LP_SIZE_MGRVAR);
    MemCopyNU32((U32*)&gFtlLastPageBuf.lastPageBufFtlInfo[LP_OFFSET_SERIAL_NUMBER],  (U32*)gFtlSNTable,  LP_SIZE_SERIAL_NUMBER);
}

//-------------------------------------------------------------------
// Function     : void FTL_LastPageUpdate(tFIO_JOB* pJob)
// Description  : Normal flow write last page
// Input        : tFIO_JOB* pJob
// return       : N/A
//-------------------------------------------------------------------
void FTL_LastPageUpdate(tFIO_JOB* pJob)
{
    U8   die, ce, lun;

    if (pJob->ssdList != SSD_WO_CACHE) ASSERT(0);

    die = FTL_GET_CURRENT_BANK(pJob->header, pJob->group);
    lun = die >> FTL_CE_SHIFT;
    ce  = die & FTL_CE_MASK;

    FTL_LastPageCacheList(pJob, ce, lun, CO_SUPPORT_PLP);
}

//-------------------------------------------------------------------
// Function     : U8 FTL_TrimUpdate(U32 laaS,U32 laaEnd)
// Description  : Update L2P and VAC by Trim BMP.
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
U8 FTL_TrimUpdate(U32 currLaa, U32 laaEnd)
{
    tPAA paaOld;
    U32 currTick, laaS = currLaa;
    U16 paaIdx = 0, blk, timeout = FTL_TRIM_UPDATE_TIMEOUT;
    U8  bfinish = TRUE;

    if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] > GC_BLKCNT_ACTIVE)
    {
        timeout += 15;
    }

    //D_PRINTF("tmud=laa[%X-%X]sn[%x]\n", currLaa, laaEnd, gFtlMgr.TableSN);
    //----- Skip the least vac blocks
    if ((gFtlTrimInfo.trimSkipBlockcnt < FTL_SKIP_TRIM_BLOCKS_COUNT) && (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] < FTL_PBT_PERIOD_BLK))
    {
        for (paaIdx =0; paaIdx < FTL_SKIP_TRIM_BLOCKS_COUNT; paaIdx++)
        {
            U32 ComCnt;
            U32 VicCnt;
            U16 TailBlk;
            U16 VicBlk;
            U16 ComBlk;

            VicBlk  = ComBlk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_USER];
            TailBlk = gFtlBlkListInfo.tail[FTL_BLOCK_POOL_USER];

            // Find first block meet not unsavebock and old data block.
            while (ComBlk != TailBlk)
            {
                if (!gFtlNbtEmptyBlk[ComBlk] && (gFtlSNTable[ComBlk] < gFtlMgr.TableSN))
                {
                    VicBlk = ComBlk;
                    break;
                }
                ComBlk = gFtlBlkListTbl[ComBlk].block;
            }

            while (ComBlk != TailBlk)
            {
                if (!(gFtlNbtEmptyBlk[ComBlk]) && (gFtlSNTable[ComBlk] < gFtlMgr.TableSN))
                {
                    ComCnt = gFtlBlkListTbl[ComBlk].vs.b.vac;
                    VicCnt = gFtlBlkListTbl[VicBlk].vs.b.vac;
                    if (ComCnt < VicCnt)
                    {
                        VicBlk = ComBlk;
                    }
                }
                ComBlk = gFtlBlkListTbl[ComBlk].block;
            }
            //D_PRINTF("VicBlk B[%4x]\n", VicBlk);
            if (gFtlNbtEmptyBlk[VicBlk] != TRUE)
            {
                gFtlTrimInfo.trimSkipBlockcnt++;
                gFtlNbtEmptyBlk[VicBlk] = TRUE;
            }
        }
    }

    //D_PRINTF("tmud=laa[%X-%X]sn[%x]table[%4x]\n", currLaa, laaEnd, sn, gFtlBlkListInfo.head[FTL_BLOCK_POOL_PBT + FTL_GROUP_NUM - 1]);
    if ((laaEnd - currLaa) == FTL_DIRECT_TRIM_LAA_COUNT)
    {
        timeout = FTL_TRIM_UPDATE_TIMEOUT * 6;
    }

    currTick    = HalTimer_ReadTick();
    paaIdx      = FTL_SUPPORT_MAX_BLK;

    while (currLaa <= laaEnd)   // should work with len = 0 case
    {
        if ((currLaa & AU_SIZE) && (HalTimer_GetElapsedTimeMsec(currTick) > timeout))
        {
            bfinish = FALSE;
            break;
        }

        // copy LAA from L2P to fast buffer
        if (paaIdx == FTL_SUPPORT_MAX_BLK)
        {
            if ((currLaa + FTL_SUPPORT_MAX_BLK) < L2P_NUM)
            {
                MemCopyNU32(gFtlFastVacBuf, (U32*)&gFtlL2PTbl[currLaa], sizeof(tPAA) * FTL_SUPPORT_MAX_BLK);
            }
            else
            {
                MemCopyNU32(gFtlFastVacBuf, (U32*)&gFtlL2PTbl[currLaa], sizeof(tPAA) * (L2P_NUM - currLaa));
            }
            paaIdx = 0;
        }

        if (FTL_TRIM_CHK_BMP(currLaa))
        {
            //D_PRINTF("Tu:laa[%x]chk[%x]cnt[%x]\n", currLaa, FTL_TRIM_CHK_BMP(currLaa), gFtlTrimInfo.trimBMPcnt);
            if (gFtlFastVacBuf[paaIdx] != DWORD_MASK)
            {
                paaOld.all32 = gFtlFastVacBuf[paaIdx];

                blk = FTL_FastGetRMPBlk(paaOld.b.ch, paaOld.b.ce, paaOld.b.lun, paaOld.b.plane, paaOld.b.block);

                if ((gFtlSNTable[blk] < gFtlMgr.TableSN) && (gFtlBlkListTbl[blk].vs.b.status == FTL_BLOCK_POOL_USER) && (gFtlNbtEmptyBlk[blk] != TRUE))
                {
                    if (gFtlBlkListTbl[blk].vs.b.vac)
                    {
                        gFtlBlkListTbl[blk].vs.b.vac--;

                        //D_PRINTF("Trim update:BK[%4x]sn[%x]laa[%x]vac[%x]\n", blk, gFtlSNTable[blk], currLaa, gFtlBlkListTbl[blk].vs.b.vac);

                        if (gFtlBlkListTbl[blk].vs.b.vac == 0)
                        {
                            FTL_BlockPopPushList(FTL_BLOCK_POOL_EMPTY, blk, FTL_SORT_NONE);
                        }

                        gFtlL2PTbl[currLaa].all32 = DWORD_MASK;

                        FTL_TRIM_CLR_BMP(currLaa);

                        if (gFtlTrimInfo.trimBlocks[blk] != TRUE)
                        {
                            gFtlTrimInfo.trimBlocks[blk] = TRUE;
                            gFtlTrimInfo.trimBlockcnt++;
                        }
                    }
                    else
                        D_PRINTF("VAC ERR! B[%4X] laa[%X]\n", blk, currLaa);

                }
                else
                    bfinish = FALSE;
            }
            else
            {
                FTL_TRIM_CLR_BMP(currLaa);
                bfinish = FALSE;
            }
        }
        paaIdx++;
        currLaa++;
    }

#if 1
    // check whether finished all laa at bank page
    currLaa = laaS;
    // head
    while (((currLaa & 31) != 0) && (currLaa < laaEnd) && (bfinish != FALSE))
    {
        if (FTL_TRIM_CHK_BMP(currLaa))
        {
            bfinish = FALSE;
            break;
        }
        currLaa++;
    }
    // middle
    if (((currLaa & 31) == 0) && (currLaa < ((laaEnd >> 5) << 5)) && (bfinish != FALSE))
    {
        U32 chkbmpS, chkbmpE;

        chkbmpS = currLaa >> 5;
        chkbmpE = laaEnd >> 5;

        while (chkbmpS < chkbmpE)
        {
            if (BIT_COUNT(gFtlTrimInfo.trimBMP[chkbmpS]) != 0)
            {
                bfinish = FALSE;
                break;
            }
            chkbmpS++;
        }
        currLaa += (((laaEnd >> 5) << 5) - currLaa);
    }
    //tail
    while ((currLaa < laaEnd) && (bfinish != FALSE))
    {
        if (FTL_TRIM_CHK_BMP(currLaa))
        {
            bfinish = FALSE;
            break;
        }
        currLaa++;
    }
#endif
    //D_PRINTF("UTrm[%d]ms-TrmBkcnt[%x]F[%d]\n", HalTimer_GetElapsedTimeMsec(currTick), gFtlTrimInfo.trimBlockcnt, bfinish);
    return bfinish;
}

//-------------------------------------------------------------------
// Function     : void FTL_DirectTrimUpdate(U32 laaS,U32 laaE)
// Description  :
// Input        : LAA
// return       : N/A
//-------------------------------------------------------------------
void FTL_DirectTrimUpdate(U32 laaS, U32 laaE)
{
    U32 pbtlaaS;
    U16 bank, bankpg, page;

    if (gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].block == WORD_MASK)
    {
        if (laaE >= L2P_NUM)
        {
            laaE = L2P_NUM - 1;
        }

        if ((laaS < L2P_NUM) && (laaE < L2P_NUM))
        {
            //D_PRINTF("laaS[%x],pbtlaaE[%x],page[%4x],bankpg[%4x]\n", laaS, pbtlaaE, page, bankpg);
            FTL_TrimUpdate(laaS, laaE);
        }
    }
    else
    {
        page    = gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].page;
        bank    = gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].bank;

        //pbtloop = gFtlPbtInfo.loopCnt - 1;
        //bankpg = (((pbtloop * gNfInfo.pagesPerBlock) + page) << FTL_BANK_SHIFT) + bank;
        bankpg = FTL_GET_PBT_PG_BN_IDX((bank & FTL_CE_MASK), (bank >> FTL_CE_SHIFT), page);

        //D_PRINTF("Pbt=BNK[%d-%d]PG[%2x]BP[%x]ck[%x]\n", bank & FTL_CE_MASK, bank >> FTL_CE_SHIFT, page, bankpg, FTL_TRIM_CHK_PBT_PG(bankpg));

        if (FTL_TRIM_CHK_PBT_PG(bankpg))
        {
            pbtlaaS = bankpg * FTL_LAA_BANK_PER_PAGE;
            //pbtlaaE = pbtlaaS + FTL_LAA_BANK_PER_PAGE -1;

            if (laaE >= L2P_NUM)
            {
                laaE = L2P_NUM - 1;
            }

            //D_PRINTF("laa[%x-%x]BNK[%d-%d]PG[%3x]loopCnt[%d]bnpg[%x]\n", laaS, laaE, bank & FTL_CE_MASK, bank >> FTL_CE_SHIFT, page, gFtlPbtInfo.loopCnt, bankpg);

            if ((laaS >= pbtlaaS) && (laaS < L2P_NUM) && (laaE < L2P_NUM))
            {
                //D_PRINTF("laaS[%x]pbtlaaS[%x]pbtlp[%d]bk[%d]page[%3x]bankpg[%x]\n", laaS, pbtlaaS, gFtlPbtInfo.loopCnt, bank, page, bankpg);
                FTL_TrimUpdate(laaS, laaE);
            }
        }
    }
}

//-------------------------------------------------------------------
// Function     : void FTL_PbtTrimUpdate(U16 page, U8 lun, U8 ce)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_PbtTrimUpdate(U16 page, U8 lun, U8 ce)
{
    U32 laaS, laaE;
    U16 bankpg;

    //bankpg = (((pbtloop * gNfInfo.pagesPerBlock) + page) << FTL_BANK_SHIFT) + (lun << FTL_CE_SHIFT) + ce;
    bankpg = FTL_GET_PBT_PG_BN_IDX(ce, lun, page);

    //D_PRINTF("Pbt[%d]=BNK[%d-%d]PG[%3x]BP[%x][%x]ck[%x]\n", pbtloop, ce, lun, page, bankpg, bankpg >> 5, FTL_TRIM_CHK_PBT_PG(bankpg));

    if (FTL_TRIM_CHK_PBT_PG(bankpg))
    {
        laaS = bankpg * FTL_LAA_BANK_PER_PAGE;
        laaE = laaS + FTL_LAA_BANK_PER_PAGE - 1;

        if (laaE >= L2P_NUM)
        {
            laaE = L2P_NUM - 1;
        }

        //D_PRINTF("laa[%x-%x]BNK[%d-%d]PG[%3x]loopCnt[%d]bnpg[%x]\n", laaS, laaE, bank & FTL_CE_MASK, bank >> FTL_CE_SHIFT, page, gFtlPbtInfo.loopCnt, bankpg);

        if ((laaS < L2P_NUM) && (laaE < L2P_NUM))
        {
            if (FTL_TrimUpdate(laaS, laaE))
            {
                FTL_TRIM_CLR_PBT_PG(bankpg);
            }
        }
    }

}

//-------------------------------------------------------------------
// Function     : void FTL_PbtPageUpdate(tFIO_JOB* pJob)
// Description  : Normal flow write PBT page
// Input        : tFIO_JOB* pJob
// return       : N/A
//-------------------------------------------------------------------
void FTL_PbtPageUpdate(tFIO_JOB* pJob)
{
    U32 cacheIdx;
    U32 l2pIdx = 0;
    U16 page;
    U8  ce, lun;

    if ((pJob->ssdList != SSD_WO_CACHE) || (pJob->ssdCnt[SSD_WO_CACHE] > 1)) ASSERT(0);

    cacheIdx        = pJob->ssdIdx[SSD_WO_CACHE];
    gFtlBtSSDHead   = GET_SSD_PTR_FROM_INDEX(cacheIdx);
    gFtlBtSSDTail   = GET_SSD_PTR_FROM_INDEX(cacheIdx);

    ce  = pJob->paa[0].b.ce;
    lun = pJob->paa[0].b.lun;
    page= pJob->paa[0].b.page;

    //l2pIdx = (((((gFtlPbtInfo.loopCnt-1) % FTL_QBT_BLOCK_COUNT) * (gNfInfo.pagesPerBlock / gNfInfo.pagesPerProgUnit)) + (page / gNfInfo.pagesPerProgUnit)) << FTL_BANK_SHIFT) + (lun << FTL_CE_SHIFT) + ce;
    l2pIdx = FTL_GET_PBT_PG_BN_IDX(ce, lun, page);

    l2pIdx = l2pIdx * FTL_LAA_BANK_PER_PAGE;
    //D_PRINTF("W[%X]ch[%d]ce[%d]ln[%d]pn[%d]B[%4x]pg[%3x]\n", l2pIdx, pJob->paa[0].b.ch, ce, lun, pJob->paa[0].b.plane, pJob->paa[0].b.block, pJob->paa[0].b.page);

    pJob->ssdCnt[SSD_WO_CACHE] = 1;

    if (l2pIdx >= L2P_NUM)
    {
        //D_PRINTF("[%d]ch[%d]ce[%d]ln[%d]pn[%d]B[%4x]pg[%2x]\n", l2pIdx, ch, paa.b.ce, paa.b.lun, plane, phyBlk, paa.b.page);
        gFtlBtSSDTail->startAddr = smSSD[SSD_DUMMY_START].startAddr;
        gFtlBtSSDTail->endAddr   = (gFtlBtSSDTail->startAddr + 0x1000 - 1);
        Cache_PopSSD_Other(SSD_POOL_OTHER, gFtlBtSSDHead, gFtlBtSSDTail, pJob->ssdCnt[SSD_WO_CACHE]);
        gFtlBtSSDTail->nextIndex = SSD_DUMMY_START;
    }
    else
    {
        //D_PRINTF("W[%d]ch[%d]ce[%d]ln[%d]pn[%d]B[%4x]pg[%2x]\n", l2pIdx, pJob->paa[0].b.ch, ce, lun, pJob->paa[0].b.plane, pJob->paa[0].b.block, pJob->paa[0].b.page);
        gFtlBtSSDTail->startAddr = (U32)&gFtlL2PTbl[l2pIdx];
        gFtlBtSSDTail->endAddr   = (gFtlBtSSDTail->startAddr + FTL_QBT_SIZE_PER_FLUSH - 1);
        Cache_PopSSD_Other(SSD_POOL_OTHER, gFtlBtSSDHead, gFtlBtSSDTail, pJob->ssdCnt[SSD_WO_CACHE]);
    }
}

//-------------------------------------------------------------------
// Function     : void FTL_EraseUpdate(tFIO_JOB* pJob)
// Description  : Normal flow erase paa
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_EraseUpdate(tFIO_JOB* pJob)
{
    U16     logicBlk;
    U8      bank, lun, ce;

    if (gFtlMgr.forceErase)
    {
        for (bank = 0; bank < FTL_BANK_NUM; bank++)
        {
            if(gFtlMgr.forceErase & (1 << bank))
                break;
        }

        logicBlk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE];
        // check Freed Head block VAC

        ce      = bank & FTL_CE_MASK;
        lun     = bank >> FTL_CE_SHIFT;

        if (gFtlBlkListTbl[logicBlk].vs.b.vac)
        {
            D_PRINTF_FTL("Erase errB[%4x]vac[%x]\n",logicBlk,gFtlBlkListTbl[logicBlk].vs.b.vac);
            //ASSERT(0);
        }

        FTL_NANDEraseProcess(pJob, ce, lun, logicBlk);

        //D_PRINTF("EU: bank:%x, blk:%x\n", bank, logicBlk);

        gFtlMgr.forceErase &= ~(1 << bank);
    }
    else
    {
        D_PRINTF_FTL("EUer\n");
        ASSERT(FALSE);
    }
}

//-------------------------------------------------------------------
// Function     : void FTL_ZoneBmpUpdate(U8 zone)
// Description  :
// Input        : zone
// return       : N/A
//-------------------------------------------------------------------
void FTL_ZoneBmpUpdate(U8 zone)
{
    gFtlPorPbtInfo.zoneBMP[zone] = TRUE;
    gFtlPorPbtInfo.zoneCnt++;

    if (gFtlPorPbtInfo.zoneCnt == FTL_POR_PBT_ZONE_NUM)
    {
        gFtlPorPbtInfo.finishBuild = TRUE;
    }
}

//-------------------------------------------------------------------
// Function     : FTL_BuildL2PZone(U8 zoneStart, U8 zoneEnd)
// Description  :
// Input        : Start and End zone
// return       : TRUE:pass FALSE:error
//-------------------------------------------------------------------
U8 FTL_BuildL2PZone(U8 zoneStart, U8 zoneEnd)
{
    U32 currTick;
    U16 blk;
    U8  status = FALSE, zone = 0, buildzone = FTL_POR_PBT_ZONE_NUM;

    currTick = HalTimer_ReadTick();
    D_PRINTF_FTL("Zone[%d]Zone1[%d]ZoneBMPs[%d]ZoneBMPe[%d]\n",
        zoneStart, zoneEnd, gFtlPorPbtInfo.zoneBMP[zoneStart], gFtlPorPbtInfo.zoneBMP[zoneEnd]);

    while (zone < FTL_POR_PBT_ZONE_NUM)
    {
        if ((gFtlPorPbtInfo.zoneBMP[zone] == FALSE) && (zone >= zoneStart) && (zone < zoneEnd))
        {
            status = FTL_QuickOrPartialBuildL2P(FTL_BUILD_TYPE_QBT, zone);

            if (status)
            {
                FTL_ZoneBmpUpdate(zone);
            }
            else // block error
            {
                break;
            }
        }

        if (gFtlPorPbtInfo.zoneBMP[zone] == FALSE)
        {
            buildzone = zone;
        }
        zone++;
    }

    if ((zone == FTL_POR_PBT_ZONE_NUM) && (status == FALSE) && (buildzone != FTL_POR_PBT_ZONE_NUM))
    {
        D_PRINTF("Build z[%d]\n", buildzone);
        status = FTL_QuickOrPartialBuildL2P(FTL_BUILD_TYPE_QBT, buildzone);

        if (status)
        {
            FTL_ZoneBmpUpdate(buildzone);
        }
    }

    if (status != TRUE)
    {
        //error handle
        //Erase QBT block and push QBT to free pool clear QBT tag,call normal build table.
        D_PRINTF_FTL("QBT ECC\n");
        blk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_QBT];
        FTL_EraseBankBlocks(blk);

        FTL_PurgePool2Free(FTL_BLOCK_POOL_QBT);
        smSysInfo->d.FtlData.d.QBTBlockTag = DWORD_MASK;
        SyIn_Synchronize(SI_AREA_MISC , SYSINFO_WRITE, SI_SYNC_BY_FTL);
    }

    currTick = HalTimer_GetElapsedTimeMsec(currTick);

    D_PRINTF_FTL("buildzone T[%d]ms, ZoneCnt[%4x], FinishBuild[%4x]\n", currTick, gFtlPorPbtInfo.zoneCnt, gFtlPorPbtInfo.finishBuild);
    return status;
}

#include "FastSection.h"
//-------------------------------------------------------------------
// Function     : void FTL_SaveQuickBuildTable(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_SaveQuickBuildTable(void)
{
#if (FTL_QBT_BUILD == TRUE)
    U32 currTick = 0;
    U16 page = 0, logicalBlk = 0, loop = 0;
    U8  header = 0, group = 0, eccBKstatus = TRUE, eccBKcnt = 0;

    if (gFtlMgr.QbtBackup)
    {
        D_PRINTF("Save Meta\n");
        currTick = HalTimer_ReadTick();

        if ((gFtlPorPbtInfo.finishBuild == FALSE) && (FTL_PARTIAL_LOAD_L2P != FALSE))
        {
            FTL_BuildL2PZone(1, FTL_POR_PBT_ZONE_NUM - 1);
        }

        if ((FTL_SELF_TEST_SPOR == TRUE) && (FTL_TestSpor(gFtlMgr.SerialNumber) == FTL_SELF_TEST_T0))
        {
            D_PRINTF_FTL("Self Test[%x]\n", gFtlMgr.SerialNumber);
            return;
        }

#if (FTL_SHUT_DOWN_CLOSE_BLK == TRUE)
        header = FTL_BLOCK_HEADER_HOST;

        for (group = 0; group < FTL_GROUP_NUM; group++)
        {
            if (!FTL_CURRENT_HEADER_NEW(header, group))
            {
                D_PRINTF("QBT ClsB-H[%d]G[%d]BN[%2X]BK[%4X]P[%3X]\n", header, group, gFtlCurrHeader[header][group].bank, gFtlCurrHeader[header][group].block, gFtlCurrHeader[header][group].page);

                page = gNfInfo.pagesPerBlock;

                #if (FTL_CLOSE_HEADER_BLOCK == FALSE)
                if (gFtlCurrHeader[header][group].page == 0)
                {
                    page = 6;
                }
                else if (gFtlCurrHeader[header][group].page < (gNfInfo.pagesPerBlock - 7))
                {
                    page = (gFtlCurrHeader[header][group].page % 2) ? (gFtlCurrHeader[header][group].page + 7) : (gFtlCurrHeader[header][group].page + 4);
                }
                #endif

                FTL_FlushDummyCloseBlock(header, group, page);

                D_PRINTF("QBT FshB-H[%d]H[%d]BN[%2X]BK[%4X]P[%3X]\n", header, group, gFtlCurrHeader[header][group].bank, gFtlCurrHeader[header][group].block, gFtlCurrHeader[header][group].page);
                if ((gFtlCurrHeader[header][group].block != WORD_MASK) && FTL_CURRENT_HEADER_NEW(header, group))
                {
                    FTL_BlockPushList(FTL_BLOCK_POOL_USER, gFtlCurrHeader[header][group].block, FTL_SORT_NONE);
                    FTL_ClearHeaderBlock(header,group);
                }
            }
        }
        page = 0;
#endif

        if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_QBT])
        {
            FTL_PurgePool2Free(FTL_BLOCK_POOL_QBT);
        }

        if (gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].block != WORD_MASK)
        {
            FTL_BlockPushList(FTL_BLOCK_POOL_FREE, gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].block, FTL_SORT_BY_EC);
            FTL_ClearHeaderBlock(FTL_BLOCK_HEADER_TABLE,0);
        }

        if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT] >= FTL_QBT_BLOCK_RELEASE)
        {
            FTL_PurgePbt2Free();
        }

        //flush table block
        D_PRINTF("FTL_QBT_BLOCK_COUNT[%d]\n", FTL_QBT_BLOCK_COUNT);

        for (loop = 0; loop < FTL_QBT_BLOCK_COUNT; loop++)
        {
            if (PLP_GET_STATUS() == PLP_DISCHARGE)
            {
                D_PRINTF_FTL("Power lose skip QBT\n");
                return;
            }

            if (eccBKstatus != TRUE)
            {
                FTL_EraseBankBlocks(logicalBlk);
                FTL_BlockPopPushList(FTL_BLOCK_POOL_FREE, logicalBlk, FTL_SORT_BY_EC);
                loop--;
                eccBKcnt++;
                D_PRINTF("B[%4x]error loop[%d]eccBKcnt[%d]\n", logicalBlk, loop, eccBKcnt);
            }

            if (eccBKcnt > FTL_QBT_BLOCK_COUNT)
            {
                D_PRINTF_FTL("ProgFailcnt[%d]\n", eccBKcnt);
                return;
            }

            //logicalBlk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE];
            logicalBlk = FTL_BlockPopHead(FTL_BLOCK_POOL_FREE);
            FTL_BlockPushList(FTL_BLOCK_POOL_QBT, logicalBlk, FTL_SORT_NONE);

            gFtlBlkListTbl[logicalBlk].vs.b.vac = 0;

            gFtlSNTable[logicalBlk] = gFtlQbtInfo[loop].sn = gFtlMgr.SerialNumber;
            gFtlQbtInfo[loop].block  = logicalBlk;
            gFtlMgr.SerialNumber++;

            D_PRINTF("QBT[%d]BK[%4x]sn[%x]\n", loop, logicalBlk, gFtlSNTable[logicalBlk]);

            if ((FTL_SELF_TEST_SPOR == TRUE) && (FTL_TestSpor(gFtlMgr.SerialNumber - 1 - loop) == FTL_SELF_TEST_T1))
            {
                D_PRINTF_FTL("Self Test[%x]\n", gFtlMgr.SerialNumber - 1 - loop);
                return;
            }

            //erase free header
            FTL_EraseBankBlocks(logicalBlk);

            eccBKstatus = FTL_NANDWriteL2P(logicalBlk, loop, FTL_QBT_TABLE_TAG);
        }

        if (gFtlMgr.forceErase)
        {
            if (PLP_GET_STATUS() == PLP_DISCHARGE)
            {
                D_PRINTF_FTL("Power lose skip erase\n");
                return;
            }

            logicalBlk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_FREE];
            D_PRINTF("erase[%x]B[%4X]\n", gFtlMgr.forceErase, logicalBlk);
            FTL_EraseBankBlocks(logicalBlk);
            gFtlMgr.forceErase = 0;
            gFtlBlkListTbl[logicalBlk].vs.b.vac = 0;
        }
        else
        {
            gFtlMgr.forceErase = FTL_BANK_BIT_MASK;
        }

        //store QBT info into sysinfo area
        #if (FTL_QBT_BUILD == TRUE)
        MEM_COPY(&smSysInfo->d.FtlData.d.QBTInfo[0], &gFtlQbtInfo[0], sizeof(tFTL_QBT_INFO)*FTL_QBT_BLOCK_COUNT);
        smSysInfo->d.FtlData.d.QBTBlockTag = SI_QBTINFO_TAG;
        SyIn_Synchronize(SI_AREA_FTL, SYSINFO_WRITE_FORCE, SI_SYNC_BY_FTL);
        #endif

        D_PRINTF("QBT[%d]ms\n", HalTimer_GetElapsedTimeMsec(currTick));
    }
    gFtlMgr.QbtBackup = FALSE;

    if (gFtlTrimInfo.trimBlockcnt > 0)
    {
        MEM_CLR(gFtlTrimInfo.trimBlocks, sizeof(U8) * FTL_SUPPORT_MAX_BLK);
        gFtlTrimInfo.trimBlockcnt = 0;
    }

    gFtlMgr.TableSN = gFtlSNTable[gFtlBlkListInfo.head[FTL_BLOCK_POOL_QBT]] - 1;
#endif

    if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_EMPTY])
    {
        FTL_PurgePool2Free(FTL_BLOCK_POOL_EMPTY);
    }

    FTL_CheckUserBlocks();
}

#include "NormSection.h"
//-------------------------------------------------------------------
// Function     : void FTL_EraseBank(U16 block)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_EraseBankBlocks(U16 block)
{
    U8 bank;

    for (bank = 0; bank < FTL_BANK_NUM; bank++)
    {
        gFtlBTJob[bank]            = FIO_PopJob(FIO_JOB_POOL_FREE);
        gFtlBTJob[bank]->cmd       = FIO_JOB_CMD_ERASE;
        gFtlBTJob[bank]->plane     = gNfInfo.planesPerLun;
        gFtlBTJob[bank]->paaCnt    = gNfInfo.numChannels * gNfInfo.planesPerLun;
        gFtlBTJob[bank]->flag      = FIO_F_NO_MKERR;

        FTL_NANDEraseProcess(gFtlBTJob[bank], bank & FTL_CE_MASK, bank >> FTL_CE_SHIFT, block);

        Core_JobIssue(gFtlBTJob[bank]);
    }
    Core_JobWait();

    gFtlBlkListTbl[block].eraseCnt++;
    gFtlMgr.AllEraseCnt++;
}

//-------------------------------------------------------------------
// Function     : void FTL_FlushDummyCloseBlock(U8 header, U8 group, U16 dmpage)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void FTL_FlushDummyCloseBlock(U8 header, U8 group, U16 dmpage)
{
    tPAA paa;
    U16 i, j, page, bank, cacheIdx;
    // ---- close user block
    //Core_JobWait();

    page = gFtlCurrHeader[header][group].page;

    D_PRINTF("FDMY-D[%d-%d]B[%04x]PG[%03x->%03x]\n",
        gFtlCurrHeader[header][group].bank & FTL_CE_MASK, gFtlCurrHeader[header][group].bank >> FTL_CE_SHIFT,
        gFtlCurrHeader[header][group].block, gFtlCurrHeader[header][group].page, dmpage);

    for (i = page; i < dmpage; i+=gNfInfo.pagesPerProgUnit)
    {
        bank = gFtlCurrHeader[header][group].bank;
        //D_PRINTF("die:%d\n",die);
        for (j = bank; j < FTL_BANK_NUM; j++)
        {
            // D_PRINTF("Dumy: die[%02x] ce[%01x] ln[%01x] bk[%04x] pg[%02x]\n", j, (j & FTL_CE_MASK), (j >> FTL_CE_SHIFT), gFtlCurrHeader[header][group].block, gFtlCurrHeader[header][group].page);
            if ((i == FTL_NAND_LAST_PAGE) && (smCacheMgr.ssdCnt[SSD_POOL_OTHER] < FTL_FLUSH_BLT_P2L_PER_PAGE)) // wait other ssd
            {
                D_PRINTF_FTL("N oCache\n");
                return;
            }

            if (PLP_GET_STATUS() == PLP_DISCHARGE)
            {
                D_PRINTF_FTL("Power lose skip dummy\n");
                return;
            }

            gFtlBtSSDHead = smCacheMgr.head[SSD_POOL_OTHER];
            cacheIdx      = GET_SSD_INDEX_FROM_PTR(gFtlBtSSDHead);

            gFtlBTJob[0] = FIO_PopJob(FIO_JOB_POOL_FREE);
            gFtlBTJob[0]->ssdList              = SSD_WO_CACHE;
            gFtlBTJob[0]->ssdIdx[SSD_WO_CACHE] = cacheIdx;
            gFtlBTJob[0]->paaCnt               = CACHE_FLUSH_ONE_CE_CNT;
            gFtlBTJob[0]->cmd                  = FIO_JOB_CMD_WRITE;
            gFtlBTJob[0]->plane                = gNfInfo.planesPerLun;
            gFtlBTJob[0]->flag                 = FIO_F_NO_MKERR;
            gFtlBTJob[0]->header               = header;
            gFtlBTJob[0]->group                = group;

            smFioInfo.flushPaaCnt += gFtlBTJob[0]->paaCnt;

            if (i == FTL_NAND_LAST_PAGE) // Flush last page of last user block
            {
                gFtlBTJob[0]->css = CACHE_LASTPAGE;

                paa.all32 = 0;
                paa.b.lun = j >> FTL_CE_SHIFT;
                paa.b.ce  = j & FTL_CE_MASK;

                FTL_LastPageCacheList(gFtlBTJob[0], paa.b.ce, paa.b.lun, FALSE);
            }
            else // Flush dummy to close user block
            {
                gFtlBTJob[0]->ssdCnt[SSD_WO_CACHE] = 1;
                gFtlBTJob[0]->css                  = CACHE_WRDUMMY;
                Cache_PopSSD_Other(SSD_POOL_OTHER, gFtlBtSSDHead, gFtlBtSSDHead, gFtlBTJob[0]->ssdCnt[SSD_WO_CACHE]);

                gFtlBtSSDHead->startAddr = smSSD[SSD_ZERO_START].startAddr;
                gFtlBtSSDHead->endAddr   = smSSD[SSD_ZERO_START].endAddr;
                gFtlBtSSDHead->nextIndex = GET_SSD_INDEX_FROM_PTR(gFtlBtSSDHead);
            }

            MEM_SET(gFastLaaBuf, BYTE_MASK, sizeof(tPAA) * gFtlBTJob[0]->paaCnt);

            FTL_FlushUpdate(gFtlBTJob[0], TRUE);

            Core_JobIssue(gFtlBTJob[0]);

            if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] == 0)
            {
                Core_JobWait();
            }
        }
    }

    Core_JobWait();
}

//-----------------------------------------------------------------------------
/**
    Device internal format operaton
**/
//-----------------------------------------------------------------------------
void FTL_Preformat(void)
{
    U16 blk = 0;
    U8  header, group = 0;

    // ------------------------------------------------
    Core_JobWait();

    // ------------------------------------------------
    // Push block to free pool
    // current header
    for (group = 0; group < FTL_GROUP_NUM; group++)
    {
        for (header = FTL_BLOCK_HEADER_GC; header < FTL_BLOCK_HEADER_MAX; header++)
        {
            if (gFtlCurrHeader[header][group].block != WORD_MASK)
            {
                gFtlBlkListTbl[gFtlCurrHeader[header][group].block].vs.b.vac = 0;

                if (header == FTL_BLOCK_HEADER_HOST)
                {
                    gFtlBlkListTbl[gFtlCurrHeader[header][group].block].vs.b.host = 0;
                }

                FTL_BlockPushList(FTL_BLOCK_POOL_FREE, gFtlCurrHeader[header][group].block, FTL_SORT_BY_EC);
            }

            FTL_ClearHeaderBlock(header, group);
        }
    }

    // blocks of block list
    for (header = FTL_BLOCK_POOL_USER; header < FTL_BLOCK_POOL_FREE; header++)
    {
        FTL_PurgePool2Free(header);
    }

    FTL_EraseFreePool();

    // keep ec
    if (smSysInfo->d.Header.d.SubHeader.d.ECnt.d.dwValidTag != SI_TAG_ECNT)
    {
        smSysInfo->d.Header.d.SubHeader.d.ECnt.d.dwValidTag = SI_TAG_ECNT;
    }

    MEM_CLR(smSysInfo->d.ECnt.all, sizeof(tSI_PAYLOAD_ECNT));

    for (blk = 0; blk < FTL_SUPPORT_MAX_BLK; blk++)
    {
        smSysInfo->d.ECnt.d.EraseCount[blk] = gFtlBlkListTbl[blk].eraseCnt;
    }

    SyIn_Synchronize(SI_AREA_ECNT, SYSINFO_WRITE, SI_SYNC_BY_FTL);

    // -------------------------------------------------
    Cache_Reset(CACHE_RST_MODE_FORCE);

    // Self generate reset cache msg since there is no other way to call this function after performat operation.
    Core_SendResetCacheMsg();

    // Reset L2P and initial FTL
    MEM_SET((PVOID)gFtlL2PTbl, BYTE_MASK, sizeof(tPAA)*L2P_NUM);
    FTL_GlobalVarInit(FTL_INITIAL_PREFORMAT);

    // ------------------------------------------------
    #if (ENABLE_GC == TRUE)
    GC_GlobalVarInit();
    #endif
}

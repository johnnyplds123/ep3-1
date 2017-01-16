#pragma once
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
//! @brief Flash Translation Layer MLC API header
//!
//=============================================================================
//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define FTL_FLUSH_FRAG_SHIFT        (0)
#define FTL_FLUSH_FRAG_MASK         (FTL_FRAG_MASK) // 3

#define FTL_FLUSH_CH_SHIFT          (FTL_FRAG_SHIFT) // 2
#define FTL_FLUSH_CH_MASK           ((1 << (FTL_FLUSH_CH_SHIFT + FTL_CH_SHIFT)) - 1) // 1 << (2 + 3) - 1 = 31

#define FTL_FLUSH_PLANE_SHIFT       (FTL_FLUSH_CH_SHIFT + FTL_CH_SHIFT) // 2 + 3 = 5
#define FTL_FLUSH_PLANE_MASK        ((1 << (FTL_FLUSH_PLANE_SHIFT + FTL_PLANE_SHIFT)) - 1) // 1 << (5 + 2) - 1 = 127
//#define FTL_FLUSH_HALF_PLANE_MASK   ((1 << (FTL_FLUSH_PLANE_SHIFT + FTL_PLANE_HALF_SHIFT)) - 1) // 1 << (5 + 1) - 1 = 63

//! @brief: FTL flush P2L per 4K
#define FTL_GET_LP_PAGE_PER_4K      (4 * KBYTE / (sizeof(U32) * gNfInfo.planesPerLun * gEcuInfo.auPerPage)) // 4plane:64, 2plane:128

//! @brief: FTL last page info
#define LP_FTL_INFO_SIZE            (gNfInfo.numChannels * 1 * gEcuInfo.auPerPage * gEcuInfo.auSize) //128KB = 8ch * 1plane * 4frag * 4KB

//! @brief: FTL flush block list and L2P per page
#define FTL_FLUSH_BLT_P2L_PER_PAGE  (1 << (FTL_FRAG_SHIFT + FTL_CH_SHIFT + 1)) // 8ch * 4frag * 2 = 64

#define FTL_NAND_LAST_PAGE          (gNfInfo.pagesPerBlock - 1)
#define FTL_NAND_FTL_INFO_PAGE      FTL_NAND_LAST_PAGE
#define FTL_NAND_FTL_P2L_PAGE       FTL_NAND_LAST_PAGE
#define FTL_NAND_FTL_DUMMY_PAGE     FTL_NAND_LAST_PAGE

//-----------------------------------------------------------------------------
//  Structure definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define FTL_GET_LP_IDX(GP, PP, DI, PA, PL, FR)  (((U32)GP << (LOG2(LP_P2L_PP_SIZE) + FTL_DIE_SHIFT + gFtlNandInfo.pages_shift + FTL_PLANE_SHIFT + FTL_FRAG_SHIFT)) | \
                                                 ((U32)PP << (FTL_DIE_SHIFT + gFtlNandInfo.pages_shift + FTL_PLANE_SHIFT + FTL_FRAG_SHIFT)) | \
                                                 ((U32)DI << (gFtlNandInfo.pages_shift + FTL_PLANE_SHIFT + FTL_FRAG_SHIFT)) | \
                                                 ((U32)PA << (FTL_PLANE_SHIFT + FTL_FRAG_SHIFT)) | \
                                                 ((U32)PL << FTL_FRAG_SHIFT) | (U32)FR)

#define FTL_GET_QBTL2PIDX(loop)                 (loop * FTL_LAA_PER_PAGE * gNfInfo.pagesPerBlock)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void FTL_FlushUpdate(tFIO_JOB* pJob, BOOL copyPaaAux);
extern __inline tPAA FTL_GetFreePage(tFIO_JOB* pJob);
extern __inline tPAA FTL_NANDWriteProcess(U32 idx, tPAA paa, U16 logicpage);
extern __inline void FTL_NANDEraseProcess(tFIO_JOB* pJob, U8 ce, U8 lun, U16 block);
U32  FTL_LastPageCacheList(tFIO_JOB* pJob, U8 ce, U8 lun, U8 copyBuf);
void FTL_LastPageP2LUpdateL2P(U16 group, U16 block);
void FTL_ReleaseFreeBlock(U16 blk, U8 bTrimData, U32 sn);
U8   FTL_BuildOpenBlockP2L(U16 startPage, U16 startBank, U16 block);
tFIO_JOB* FTL_NANDReadPageBuild(U16 css, U8 group, U8 ce, U8 lun, U8 plane, U16 block, U16 page, U8 readAux, U8 readP2L);
U8   FTL_NANDReadP2LBuildL2P(U16 group, U16 block, U16 page);
U8   FTL_NANDReadL2P(U16 logicalBlk, U16 loop, U16 pageStart, U16 pageEnd, U32 l2pIdx);
U8   FTL_NANDWriteL2P(U16 logicalBlk, U16 loop, U32 type);

//-----------------------------------------------------------------------------
//  Inline function reference
//-----------------------------------------------------------------------------


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
//! @brief Application level descriptor manager
//!
//=============================================================================
#include "Fio.h"

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#if CHK_FW_TYPE(FW_AGING)
    #define READ_LEVEL_CHANGE           DISABLE
#else
    #if _NAND_ == NAND_3DTLCG2
        #define READ_LEVEL_CHANGE       DISABLE
    #else
        #define READ_LEVEL_CHANGE       ENABLE
    #endif
#endif

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#if DESC_SUPPORT_MULTI_TRK_PER_JOB == TRUE
    #define CHK_OFFSET_OVER_TH(DESCOFFSET, DESCTH) (DESCOFFSET >= (DESCTH - 1))
    #define APLDESC_ALLOC_MULTI_TRK(DESCOFFSET, DESCTH, JOB, DESCTPL, PAAIDX) {     \
        if (DESCOFFSET >= DESCTH) {                                                 \
            JOB->tid = AplSeq_AllocTrkId(SEQ_TIMEOUT_WAIT);                         \
            smFioInfo.tid2PaaIdx[JOB->tid] = PAAIDX;                                \
            smFioInfo.tid2Job[JOB->tid] = JOB;                                      \
            gAplSeqInfo.trkIssueCnt++;                                              \
            JOB->trkAllocCnt++;                                                     \
            DESCTPL.field.TRACK_ID = JOB->tid;                                      \
            MEM_CLR(gOpenPgMap[JOB->tid], sizeof(U32) * (OPEN_PG_BMP_ENTRY_COUNT)); \
            DESCOFFSET = 0;                                                         \
        }                                                                           \
    }
#else
    #define CHK_OFFSET_OVER_TH(DESCOFFSET, DESCTH) FALSE
    #define APLDESC_ALLOC_MULTI_TRK(DESCOFFSET, DESCTH, JOB, DESCTPL, PAAIDX)
#endif

#define DESC_SET_OPEN_PG_BMP(T, I)  {(gOpenPgMap[T][(I >> OPEN_PG_BMP_SHIFT)] |= BIT(I & OPEN_PG_BMP_MASK)); gOpenPgCnt[T]++;}
#define DESC_CHK_OPEN_PG_BMP(T, I)   (gOpenPgMap[T][(I >> OPEN_PG_BMP_SHIFT)] &  BIT(I & OPEN_PG_BMP_MASK))
#define DESC_RST_OPEN_PG_BMP(T)      if (gOpenPgCnt[pJob->tid]) {MEM_CLR(gOpenPgMap[T], sizeof(U32) * (OPEN_PG_BMP_ENTRY_COUNT)); gOpenPgCnt[T] = 0;}

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern U32 gOpenRow[MAX_SUPPORTED_CHANNELS * MAX_SUPPORTED_DEVICES * MAX_SUPPORTED_LUNS * MAX_SUPPORTED_PLANES];
extern U32 gOpenPgMap[MAX_TRK_NUM][OPEN_PG_BMP_ENTRY_COUNT];
extern U32 gOpenPgCnt[MAX_TRK_NUM];
extern U32 gAuxData[META_DATA_SIZE / 4];

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------

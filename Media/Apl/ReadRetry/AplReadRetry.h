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
//! @brief Read Retry
//=============================================================================

//-----------------------------------------------------------------------------
//  include header files:
//-----------------------------------------------------------------------------
#if _VENDOR_ == VENDOR_TSB
    #if _NAND_ == NAND_MLC
        #include "AplDesc_TSB_MLC.h"
    #elif _NAND_ == NAND_3DTLCG2
        #include "AplDesc_TSB_3DTLCG2.h"
    #endif
#elif __VENDOR_ == VENDOR_SNDK
        #include "AplDesc_SNDK_MLC.h"
#else
    #error "not supported!"
#endif

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define RR_STATISTIC            ENABLE

#define RR_MAX_JOB              256
#define RR_MIN_JOB              32
COMPILE_ASSERT(RR_MIN_JOB * 2 <= RR_MAX_JOB, "MIN * 2 > MAX");

#define RR_PAA_UNIT             2
#define RR_MAX_RETRY_COUNT      MAX_READRETRY_ROW
#define RR_MAX_RETRY_COUNT_SLC  MAX_READRETRY_SLC

#define RR_PAA_BMP_SHIFT        5
#define RR_PAA_BMP_BITS_CNT     (1 << RR_PAA_BMP_SHIFT)
#define RR_PAA_BMP_MASK         (RR_PAA_BMP_BITS_CNT - 1)
#define RR_PAA_BMP_ENTRY_CNT    ((MAX_PAA_PER_JOB + RR_PAA_BMP_MASK) / RR_PAA_BMP_BITS_CNT)

typedef enum{
    RR_JOB_POOL_FREE = 0,
    RR_JOB_POOL_BUSY,
    RR_JOB_POOL_COUNT
}RRPool_t;

typedef enum{
    RR_STATE_CREATE = 0,
    RR_STATE_ISSUE,
    RR_STATE_RELEASE,
    RR_STATE_COUNT
}RRJobState_t;

typedef enum{
    RR_RET_CONTINUE = 0,
    RR_RET_POSTPONE,
    RR_RET_ESCAPE
}RRRetStatus_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define RR_SET_ERR_BMP(F,I)        {(gRRInfo.errMap[F - FIO_MAX_JOB][I >> RR_PAA_BMP_SHIFT] |=  BIT(I & RR_PAA_BMP_MASK)); gRRInfo.errCnt[F - FIO_MAX_JOB]++;}  // set error bitmap
#define RR_CLR_ERR_BMP(F,I)        {(gRRInfo.errMap[F - FIO_MAX_JOB][I >> RR_PAA_BMP_SHIFT] &= ~BIT(I & RR_PAA_BMP_MASK)); gRRInfo.errCnt[F - FIO_MAX_JOB]--;}  // clear error bitmap
#define RR_CHK_ERR_BMP(F,I)        ( gRRInfo.errMap[F - FIO_MAX_JOB][I >> RR_PAA_BMP_SHIFT] &   BIT(I & RR_PAA_BMP_MASK))                                       // check error bitmap
#define RR_GET_ERR_BMP(F,I)        ( gRRInfo.errMap[F - FIO_MAX_JOB][I >> RR_PAA_BMP_SHIFT])                                                                    // get error bitmap
#define RR_RST_ERR_BMP(F)          {MEM_CLR(&gRRInfo.errMap[F - FIO_MAX_JOB][0], sizeof(U32) * RR_PAA_BMP_ENTRY_CNT); gRRInfo.errCnt[F - FIO_MAX_JOB] = 0;}     // reset error bitmap
#define RR_GET_ERR_CNT(F)          ( gRRInfo.errCnt[F - FIO_MAX_JOB])                                                                                           // get error count

#if RR_STATISTIC == ENABLE
    #define RR_REC_IO_CNT(F)        if (F < FIO_MAX_JOB) gRRStat.totalReadIO++;
    #define RR_REC_PASS_CNT(X)      gRRStat.errIO[X]++;
    #define RR_REC_FAIL_CNT()       gRRStat.errIO[0]++;
    #define RR_REC_PAGE_CNT(W)      gRRStat.errPage[W]++;
    #define RR_REC_RESET()          MEM_CLR(&gRRStat, sizeof(gRRStat));
#else
    #define RR_REC_IO_CNT(F)        {}
    #define RR_REC_PASS_CNT(X)      {}
    #define RR_REC_FAIL_CNT()       {}
    #define RR_REC_PAGE_CNT(W)      {}
    #define RR_REC_RESET()          {}
#endif

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef tFIO_JOB RRJob_t;

typedef struct{
    RRJob_t*          head[RR_JOB_POOL_COUNT];                  // RR job linked list head/tail pointer
    RRJob_t*          tail[RR_JOB_POOL_COUNT];                  // RR job linked list head/tail pointer
    volatile uint32_t jobCnt[RR_JOB_POOL_COUNT];                // RR job list count
    tFIO_JOB*         RRJob2FioJob[RR_MAX_JOB];                 // RR job to Fio job mapping table
    uint32_t          errMap[RR_MAX_JOB][RR_PAA_BMP_ENTRY_CNT]; // RR paa error flag
    uint16_t          errCnt[RR_MAX_JOB];                       // RR error paa count
    uint16_t          scanPaaStartIdx[RR_MAX_JOB];              // start scan paa index
    uint16_t          scanPaaCurrIdx[RR_MAX_JOB];               // current scan paa index
}RRInfo_t;

typedef struct{
    uint64_t         totalReadIO;
    uint32_t         errIO[RR_MAX_RETRY_COUNT + 1];
    //uint32_t        errPage[576];
}RRStat_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern RRInfo_t    gRRInfo;
extern RRStat_t    gRRStat;
extern FioJobTpl_t gRRJobTpl;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t  AplReadRetry_OneTimeInit(InitBootMode_t initMode);
void     AplReadRetry_Entrance(void);
RRJob_t* AplReadRetry_PopJob(RRPool_t pool);
void     AplReadRetry_PushJob(RRPool_t pool, RRJob_t* pJob);
void     AplReadRetry_DumpInfo(uint32_t level, uint32_t param);

//-----------------------------------------------------------------------------
//  Inline function reference
//-----------------------------------------------------------------------------

#pragma once
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
//! @brief
//!
//=============================================================================
//-----------------------------------------------------------------------------
//  include header files:
//-----------------------------------------------------------------------------
#if _CPU_ == CPU_ID1
#include "AplSch.h"
#endif

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define FIO_MAX_JOB                     192

#if CO_SUPPORT_PATROL_READ == TRUE
    #define FIO_R_POOL_CNT_NORMAL       1
#else
    #define FIO_R_POOL_CNT_NORMAL       0
#endif

#define FIO_R_POOL_SCHEDULE_COUNT       64
#define FIO_R_POOL_SCHEDULE_MASK        (FIO_R_POOL_SCHEDULE_COUNT - 1)

COMPILE_ASSERT(FIO_MAX_JOB<=256, "MUST less or equal to 256");
// need to review message queue (ex. C2M_REQ_QUEUE_DEPTH) if fio job count > 256

#define MAX_PAA_PER_JOB                 (MAX_DESC_PER_TRK * 4)
#define MAX_AUX_PER_JOB                 (MAX_PAA_PER_JOB * (META_DATA_SIZE / 4))

#define NUM_DS_PER_DWORD_SHIFT          2
#define NUM_DS_PER_DWORD                (1 << NUM_DS_PER_DWORD_SHIFT)
#define NUM_DS_PER_DWORD_MASK           (NUM_DS_PER_DWORD - 1)

#define NUM_BITS_PER_DS                 8

#define FIO_JOB_MAX_FILLUP_CNT          (MAX_DESC_PER_TRK / 2)  // consider SLC read

#define OPEN_PG_BMP_SHIFT               5
#define OPEN_PG_BMP_SIZE                (1 << OPEN_PG_BMP_SHIFT)
#define OPEN_PG_BMP_MASK                (OPEN_PG_BMP_SIZE - 1)
#define OPEN_PG_BMP_ENTRY_COUNT         ((MAX_PAA_PER_JOB + OPEN_PG_BMP_MASK) / OPEN_PG_BMP_SIZE)

#define FIO_ERR_PAA_BMP_SHIFT           5
#define FIO_ERR_PAA_BMP_BITS_COUNT      (1 << FIO_ERR_PAA_BMP_SHIFT)
#define FIO_ERR_PAA_BMP_MASK            (FIO_ERR_PAA_BMP_BITS_COUNT - 1)
#define FIO_ERR_PAA_BMP_ENTRY_COUNT     ((MAX_PAA_PER_JOB + FIO_ERR_PAA_BMP_MASK) / FIO_ERR_PAA_BMP_BITS_COUNT)

#define FIO_S_NO_ERR                    0
#define FIO_S_UNC_ERR                   B_0
#define FIO_S_OVER_LMT                  B_1
#define FIO_S_BLANK_ERR                 B_2
#define FIO_S_PROG_ERR                  B_3
#define FIO_S_ERASE_ERR                 B_4
#define FIO_S_UXP_ERR                   B_5
#define FIO_S_MCRC_ERR                  B_6

#define FIO_S_ERR_MASK                  (B_0|B_1|B_2|B_3|B_4|B_5|B_6)

#define FIO_S_ISSUED                    B_14
#define FIO_S_COMPLETE                  B_15

#if CHK_FW_TYPE(FW_AGING)
    #define FIO_ERR_HANDLE_MASK         (FIO_S_OVER_LMT|FIO_S_UNC_ERR|FIO_S_PROG_ERR|FIO_S_ERASE_ERR|FIO_S_BLANK_ERR)
#else
    #define FIO_ERR_HANDLE_MASK         (FIO_S_UNC_ERR|FIO_S_PROG_ERR|FIO_S_ERASE_ERR|FIO_S_BLANK_ERR)
#endif

#define FIO_ERR_RETRY_MASK              (FIO_S_UNC_ERR|FIO_S_BLANK_ERR)

// List of Flag options
#define FIO_F_AUX                       B_0
#define FIO_F_SLC                       B_1
#define FIO_F_MUTE                      B_2
#define FIO_F_MUTE_BLANK_ERR            B_3
#define FIO_F_SCHEDULE                  B_4
#define FIO_F_BOOT                      B_5
#define FIO_F_NANDTIME                  B_6
#define FIO_F_NO_RR                     B_7
#define FIO_F_NO_MKERR                  B_8
#define FIO_F_PAGE_READ                 B_9

// FIO Job Complete Time Out
#define FIO_TIMEOUT_US                  (10 * 1000 * 1000)  // 10s

/*####################################################################
##      ##    ###    ########  ##    ## #### ##    ##  ######   ####
##  ##  ##   ## ##   ##     ## ###   ##  ##  ###   ## ##    ##  ####
##  ##  ##  ##   ##  ##     ## ####  ##  ##  ####  ## ##        ####
##  ##  ## ##     ## ########  ## ## ##  ##  ## ## ## ##   ####  ##
##  ##  ## ######### ##   ##   ##  ####  ##  ##  #### ##    ##
##  ##  ## ##     ## ##    ##  ##   ###  ##  ##   ### ##    ##  ####
 ###  ###  ##     ## ##     ## ##    ## #### ##    ##  ######   ####
#####################################################################*/
// Change enumeration order at your own risk!
enum
{
    FIO_JOB_CMD_READ = 0,
    FIO_JOB_CMD_READ_FAST,
    FIO_JOB_CMD_READ_SLC,
    FIO_JOB_CMD_READ_SLC_TOUCH,
    FIO_JOB_CMD_READ_IDS_PAGE,
    FIO_JOB_CMD_READ_IDS,
    FIO_JOB_CMD_PREOPEN,
    FIO_JOB_CMD_RR,
	FIO_JOB_CMD_READ_DISTRIBUTION,
    //--- intend to be separated
    FIO_JOB_CMD_WRITE,
    FIO_JOB_CMD_ERASE,
    FIO_JOB_CMD_COUNT,
    FIO_JOB_CMD_NULL
};

enum
{
    FIO_CMD_1PLANE = 1,
    FIO_CMD_2PLANE = 2,
    FIO_CMD_4PLANE = 4,
    FIO_CMD_8PLANE = 8
};

typedef enum
{
    FIO_JOB_POOL_FREE = 0,  // 0
    FIO_JOB_POOL_HEAP,      // 1
    //--- break point -----------
    FIO_JOB_POOL_WAIT,      // 2
    FIO_JOB_POOL_READ,      // 3 ~ 67
    FIO_JOB_POOL_WRITE = FIO_JOB_POOL_READ + FIO_R_POOL_CNT_NORMAL + FIO_R_POOL_SCHEDULE_COUNT,     // 68
    FIO_JOB_POOL_ERASE,     // 69
    //--- break point -----------
    FIO_JOB_POOL_FREEZE,    // 70
    FIO_JOB_POOL_COUNT,     // 71
    FIO_JOB_POOL_NULL       // illegal
}FioJobPool_t;

enum
{
    SSD_W_CACHE=0,
    SSD_WO_CACHE,
    SSD_LIST_COUNT,
};

enum{
    AUX_BUF_IDX_LAA = 0,
    AUX_BUF_IDX_SN,
    AUX_BUF_IDX_TIME,
    AUX_BUF_MAX_CNT
};

enum {
    AUX_SPM_IDX_DUMMY = 0,
    AUX_SPM_IDX_LAA,
    AUX_SPM_IDX_SN,
    AUX_SPM_IDX_TIME,
    AUX_SPM_MAX_CNT
};

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define FIO_SET_ERR_BMP(F,I)        {(smFioInfo.errPaaMap[F][I >> FIO_ERR_PAA_BMP_SHIFT] |=  BIT(I & FIO_ERR_PAA_BMP_MASK)); smFioInfo.errPaaCnt[F]++;} // set error bit map
#define FIO_CLR_ERR_BMP(F,I)        {(smFioInfo.errPaaMap[F][I >> FIO_ERR_PAA_BMP_SHIFT] &= ~BIT(I & FIO_ERR_PAA_BMP_MASK)); smFioInfo.errPaaCnt[F]--;} // clear error bit map
#define FIO_CHK_ERR_BMP(F,I)        (smFioInfo.errPaaMap[F][I >> FIO_ERR_PAA_BMP_SHIFT]  &   BIT(I & FIO_ERR_PAA_BMP_MASK))                             // check error bit map
#define FIO_GET_ERR_BMP(F,I)        (smFioInfo.errPaaMap[F][I >> FIO_ERR_PAA_BMP_SHIFT])                                                                // get error bit map
#define FIO_RST_ERR_BMP(F)          {MEM_CLR(&smFioInfo.errPaaMap[F][0], sizeof(U32) * FIO_ERR_PAA_BMP_ENTRY_COUNT); smFioInfo.errPaaCnt[F] = 0;}       // reset error bit map

#if (FIO_JOB_HEAP_MEMORY == TRUE)
    #define FIO_GET_HEAP_POOL_CNT() smFioInfo.jobCnt[FIO_JOB_POOL_HEAP]
#else
    #define FIO_GET_HEAP_POOL_CNT() 0
#endif

#define FIO_GET_SCH_POOL_NO(paa)    ((((paa.b.lun << gNfInfo.bitsChDev) | (paa.b.ce << gNfInfo.bitsCh) | paa.b.ch) & FIO_R_POOL_SCHEDULE_MASK) + (FIO_JOB_POOL_READ + FIO_R_POOL_CNT_NORMAL))

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct
{
    MsgHostIO_t*        pMsgHost;              // NULL                 | host msg pointer
    U16                 status;                // 0                    | Track Status                     (auto)
    U16                 ssdCnt[SSD_LIST_COUNT];// 0                    |
    U16                 ssdIdx[SSD_LIST_COUNT];// SSD_NULL_START       |
    U16                 descCnt;               // 0                    | Descriptor count                 (auto)
    U16                 paaCnt;                // 0                    | paa count
    U16                 flag;                  // 0                    | flag for AUX, SLC ...
    U16                 paaOffset;             // 0                    | Cmd job break down for IDS read
    U8                  css;                   // CACHE_AUTO           | cache search status
    U8                  cmd;                   // FIO_JOB_CMD_NULL     | Sequencer command
    U8                  auFmt;                 // AuFmt 0              | AU format
    U8                  ssdList;               // SSD_W_CACHE          | SSD type
    U8                  plane;                 // gNfInfo.planesPerLun | plane number
    U8                  header;                // FTL_BLOCK_HEADER_MAX | flush header type
    U8                  group;                 // FTL_GROUP_NUM        | flush group number
    U8                  segNum;                // NUM_OF_FS_SEG        | segment number                   (auto)
    U8                  trkAllocCnt;           // 0                    | Track allocation count
    U8                  trkFreeCnt;            // 0                    | Track Free Count
}FioJobTpl_t;
COMPILE_ASSERT(sizeof(FioJobTpl_t)==32, "MUST Fixed at 32 bytes");

/// @brief Core & Media fio jobs
typedef struct _FioJob_t
{
//---------- Variable ---------------------- Default Value -------- Description ------------------- Attributes
    struct _FioJob_t*   next;                  //                      | next pointer                     (Auto)
    struct _FioJob_t*   prev;                  //                      | prev pointer                     (Auto)
    U8                  timeStamp;             //                      | execution time                   (auto)
    U8                  rrState;               //                      | read retry state
    U8                  rrCount;               //                      | read retry count
    U8                  poolNo;                //                      | pool number
    U16                 fid;                   //                      | fio job id                       (fixed)
    U16                 tid;                   //                      | track id
    tPAA*               paa;                   //                      | paa list pointer
    U32*                aux;                   //                      | aux list pointer
    U32                 serialNumber;          //                      | SN in Aux                        (auto)
    U8*                 pBuffer;               // NULL                 | data buffer for none cache i/o
    //----------------------- MEM BURST COPY START (32B)------------------------------------------------
    MsgHostIO_t*        pMsgHost;              // NULL                 | host msg pointer
    U16                 status;                // 0                    | Track Status                     (auto)
    U16                 ssdCnt[SSD_LIST_COUNT];// 0                    |
    U16                 ssdIdx[SSD_LIST_COUNT];// SSD_NULL_START       |
    U16                 descCnt;               // 0                    | Descriptor count                 (auto)
    U16                 paaCnt;                // 0                    | paa count
    U16                 flag;                  // 0                    | flag for AUX, SLC ...
    U16                 paaOffset;             // 0                    | Cmd job break down for IDS read
    U8                  css;                   // CACHE_AUTO           | cache search status
    U8                  cmd;                   // FIO_JOB_CMD_NULL     | Sequencer command
    U8                  auFmt;                 // AuFmt 0              | AU format
    U8                  ssdList;               // SSD_W_CACHE          | SSD type
    U8                  plane;                 // gNfInfo.planesPerLun | plane number
    U8                  header;                // FTL_BLOCK_HEADER_MAX | flush header type
    U8                  group;                 // FTL_GROUP_NUM        | flush group number
    U8                  segNum;                // NUM_OF_FS_SEG        | segment number                   (auto)
    U8                  trkAllocCnt;           // 0                    | Track allocation count
    U8                  trkFreeCnt;            // 0                    | Track Free Count
    //----------------------- MEM BURST COPY END (32B)--------------------------------------------------
//-------------------------------- 64 Bytes -------------------------------------------------------------------
}tFIO_JOB;
COMPILE_ASSERT(sizeof(tFIO_JOB)==64, "MUST Fixed at 64 bytes");

typedef struct
{
    volatile U32    jobIssueCnt;
    volatile U32    jobDoneCnt;

    volatile U32    fillupJobCnt;                       // for PLP check fillup Job Complete
    U32             currTimeStamp;
    U64             flushPaaCnt;

    tPAA*           pDramPaa;
    U32*            pDramAux;

    tFIO_JOB*       tid2Job[MAX_TRK_NUM];               // track to fio job
    U32             tid2PaaIdx[MAX_TRK_NUM];            // paa offset by track
    U32             errPaaMap[FIO_MAX_JOB][FIO_ERR_PAA_BMP_ENTRY_COUNT];
    U16             errPaaCnt[FIO_MAX_JOB];

    tFIO_JOB*       head[FIO_JOB_POOL_COUNT];           // FIO Job linked list head/tail pointer
    tFIO_JOB*       tail[FIO_JOB_POOL_COUNT];           // FIO Job linked list head/tail pointer
    volatile U32    jobCnt[FIO_JOB_POOL_COUNT];         // FIO Job list count
    U32             currTMLevel;                        // current NFIF timing level (dynamic thermal throttling)
    U32             reqTMLevel;                         // request NFIF timing level (dynamic thermal throttling)
}FioInfo_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
#if  _CPU_==CPU_ID2
    extern FioJobTpl_t   gTplFioInfo;
#endif

extern tFIO_JOB             smFioJobBuf[FIO_MAX_JOB];
extern FioInfo_t            smFioInfo;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t FIO_OneTimeInit(InitBootMode_t initMode);
void    FIO_DumpInfo(U32 level);
void    FIO_PLPFreezeJobs(void);
void    FIO_PLPUnfreezeJobs(void);

//-----------------------------------------------------------------------------
//  Inline Function
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    Pop fio job from specific pool

    @param[in]  pool number
    @return     pJob
**/
//-----------------------------------------------------------------------------
static __inline tFIO_JOB* FIO_PopJob(FioJobPool_t pool)
{
    tFIO_JOB* pJob;

    ASSERT (smFioInfo.jobCnt[pool]);

    pJob = smFioInfo.head[pool];

    if (pJob == smFioInfo.tail[pool])
    {
        smFioInfo.head[pool] = smFioInfo.tail[pool] = NULL;
    }
    else
    {
        smFioInfo.head[pool] = pJob->next;
        pJob->next->prev    = NULL;
    }

    pJob->next = pJob->prev = NULL;

    smFioInfo.jobCnt[pool]--;

    #if (_CPU_==CPU_ID2)
    if (pool == FIO_JOB_POOL_FREE)
    {
        MemCopy8U32((U32*)&pJob->pMsgHost, (U32*)&gTplFioInfo);
    }
    #endif

    #if _CPU_ == CPU_ID1
    if (pool >= FIO_JOB_POOL_READ && pool  < FIO_JOB_POOL_WRITE)    gSchInfo.schJobCnt--;
    if (pool >= FIO_JOB_POOL_READ && pool <= FIO_JOB_POOL_ERASE)    gSchInfo.queuingJobs--;
    #endif

    return pJob;
}

//-----------------------------------------------------------------------------
/**
    Push fio job into specific pool

    @param[in]  pool number
    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
static __inline void FIO_PushJob(FioJobPool_t pool, tFIO_JOB* pJob)
{
    #if _CPU_ == CPU_ID1
    if (pool == FIO_JOB_POOL_WAIT)
    {
        if (pJob->cmd < FIO_JOB_CMD_WRITE)
        {
            if (pJob->flag & FIO_F_SCHEDULE)                    pool = (FioJobPool_t)pJob->poolNo;
        #if CO_SUPPORT_PATROL_READ == TRUE
            else if (pJob->cmd == FIO_JOB_CMD_READ_SLC_TOUCH)   pool = FIO_JOB_POOL_READ;
        #endif
            else                                                pool = (FioJobPool_t)FIO_GET_SCH_POOL_NO(pJob->paa[0]);

            gSchInfo.schJobCnt++;
        }
        else if (pJob->cmd == FIO_JOB_CMD_WRITE)    pool = FIO_JOB_POOL_WRITE;
        else if (pJob->cmd == FIO_JOB_CMD_ERASE)    pool = FIO_JOB_POOL_ERASE;
        else ASSERT(0);

        gSchInfo.queuingJobs++;
    }
    #endif

    if (smFioInfo.tail[pool] == NULL)
    {
        smFioInfo.head[pool] = smFioInfo.tail[pool] = pJob;
    }
    else
    {
        smFioInfo.tail[pool]->next = pJob;
        pJob->prev                 = smFioInfo.tail[pool];
        smFioInfo.tail[pool]       = pJob;
    }

    smFioInfo.jobCnt[pool]++;
}

//-----------------------------------------------------------------------------
/**
    Push fio job into specific pool head

    @param[in]  pool number
    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
static __inline void FIO_PushHeadJob(FioJobPool_t pool, tFIO_JOB* pJob)
{
    #if _CPU_ == CPU_ID1
    if (pool == FIO_JOB_POOL_WAIT)
    {
        if (pJob->cmd < FIO_JOB_CMD_WRITE)
        {
            if (pJob->flag & FIO_F_SCHEDULE)                    pool = (FioJobPool_t)pJob->poolNo;
        #if CO_SUPPORT_PATROL_READ == TRUE
            else if (pJob->cmd == FIO_JOB_CMD_READ_SLC_TOUCH)   pool = FIO_JOB_POOL_READ;
        #endif
            else                                                pool = (FioJobPool_t)FIO_GET_SCH_POOL_NO(pJob->paa[0]);

            gSchInfo.schJobCnt++;
        }
        else if (pJob->cmd == FIO_JOB_CMD_WRITE)    pool = FIO_JOB_POOL_WRITE;
        else if (pJob->cmd == FIO_JOB_CMD_ERASE)    pool = FIO_JOB_POOL_ERASE;
        else ASSERT(0);

        gSchInfo.queuingJobs++;
    }
    #endif

    if (smFioInfo.tail[pool] == NULL)
    {
        smFioInfo.head[pool] = smFioInfo.tail[pool] = pJob;
    }
    else
    {
        pJob->next                 = smFioInfo.head[pool];
        smFioInfo.head[pool]->prev = pJob;
        smFioInfo.head[pool]       = pJob;
    }

    smFioInfo.jobCnt[pool]++;
}

//-----------------------------------------------------------------------------
/**
    Runtime check fio job parameter, assert firmware if any error

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
__inline void FIO_RunTimeCheck(tFIO_JOB* pJob)
{
    U16 ErrCode = 0;

    if      (pJob->cmd >= FIO_JOB_CMD_COUNT) ErrCode = 1;
    else if (!pJob->paaCnt)                  ErrCode = 2;
    else if (pJob->paaCnt > MAX_PAA_PER_JOB) ErrCode = 3;

    if (ErrCode)
    {
        D_PRINTF("[MEDA] ERR:%08X --> F/T[%3d.%3d] Cmd%2d P/D/E[%3d.%3d.%3d] S[%04X] CSS%2d T[%08X]\n", ErrCode, pJob->fid, pJob->tid, pJob->cmd, pJob->paaCnt, pJob->descCnt, smFioInfo.errPaaCnt[pJob->fid], pJob->status, pJob->css, pJob->timeStamp);
        ASSERT(ErrCode == 0);
    }
}

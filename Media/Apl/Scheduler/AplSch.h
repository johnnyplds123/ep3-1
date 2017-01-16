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
//! @brief Flash I/O Scheduler
//=============================================================================

//-----------------------------------------------------------------------------
//  include header files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define SCH_READ_JOB_THRESHOLD          64
#define SCH_WRITE_JOB_THRESHOLD         10
#define SCH_ERASE_JOB_THRESHOLD         8

#define SCH_MAX_ACTIVE_READ_DIES        48
#define FIO_MAX_ACTIVE_ERASE_JOBS       4

#if (_GENERATION_ == GENERATION_EP2)
    #define SCH_WRITE_RMS_9W            (5)
    #define SCH_WRITE_RMS_8W            (4)
    #define SCH_WRITE_RMS_7W            (3)
    #define SCH_MAX_ACTIVE_WRITE_JOBS   (SCH_WRITE_RMS_7W)
#elif (_GENERATION_ == GENERATION_EP3)
    #define SCH_WRITE_RMS_7W            (6)
    #define SCH_WRITE_RMS_6W            (5)
    #define SCH_WRITE_RMS_5W            (4)
    #define SCH_MAX_ACTIVE_WRITE_JOBS   (SCH_WRITE_RMS_6W)
#else
    #define SCH_MAX_ACTIVE_WRITE_JOBS   4
#endif

#define SCH_MAX_ACTIVE_ERASE_JOBS_PLP   2
#define SCH_MAX_ACTIVE_WRITE_JOBS_PLP   2
#define SCH_MAX_ACTIVE_READ_DIES_PLP    48

//-----------------------------------------------------------------------------
#define FS_R_START                      0
#define FS_R_COUNT                      (NUM_OF_FS_SEG / 2)
#define FS_R_END                        (FS_R_START + FS_R_COUNT - 1)

#define FS_W_START                      (FS_R_END + 1)
#define FS_W_COUNT                      (NUM_OF_FS_SEG - FS_R_COUNT)
#define FS_W_END                        (NUM_OF_FS_SEG - 1)

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
enum
{
    SCH_STATE_WAIT_READ = 0,
    SCH_STATE_WAIT_WRITE,
    SCH_STATE_WAIT_ERASE,
    SCH_STATE_IDLE,
    SCH_STATE_PROC_READ,
    SCH_STATE_PROC_WRITE,
    SCH_STATE_PROC_ERASE,
    SCH_STATE_COUNT
};

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct{
    volatile uint32_t   issueDieCnt;    // total issued die count (for power saving, ring-over is acceptable)
    volatile uint32_t   doneDieCnt;     // total done die count (for power saving, ring-over is acceptable)
    volatile uint32_t   queuingJobs;    // total queuing job count
    uint32_t            issueReadJobs;  // total issued read job count (reset when scheudler state changed)
    uint32_t            issueWriteJobs; // total issued write job count (reset when scheudler state changed)
    uint32_t            issueEraseJobs; // total issued erase job count (reset when scheudler state changed)
    uint32_t            maxLookaheadCnt;// max scheduling read job lookahead count
    uint32_t            schJobCnt;      // total scheduling read job count
    uint32_t            currSchPool;    // current scheduling read pool
    uint32_t            maxSchPool;     // max scheduling read pool
    uint32_t            currReadFS;     // current flash segment for read job
    uint32_t            currWriteFS;    // current write segment for write job
    uint32_t            state;          // scheduler state
}SchInfo_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern SchInfo_t gSchInfo;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
extern Error_t AplSch_OneTimeInit(InitBootMode_t initMode);

//-----------------------------------------------------------------------------
//  Inline function reference
//-----------------------------------------------------------------------------

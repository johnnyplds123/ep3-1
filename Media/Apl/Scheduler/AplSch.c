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
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "AplSeq.h"
#include "Fio.h"

#include "AplSch.h"

#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING
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
FioJobPool_t Sch_WaitRead(void);
FioJobPool_t Sch_WaitWrite(void);
FioJobPool_t Sch_WaitErase(void);
FioJobPool_t Sch_Idle(void);
FioJobPool_t Sch_ProcRead(void);
FioJobPool_t Sch_ProcWrite(void);
FioJobPool_t Sch_ProcErase(void);

// PLP
FioJobPool_t Sch_ProcPLPRead(void);
FioJobPool_t Sch_ProcPLPWrite(void);
FioJobPool_t Sch_ProcPLPErase(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
/*####################################################################
##      ##    ###    ########  ##    ## #### ##    ##  ######   ####
##  ##  ##   ## ##   ##     ## ###   ##  ##  ###   ## ##    ##  ####
##  ##  ##  ##   ##  ##     ## ####  ##  ##  ####  ## ##        ####
##  ##  ## ##     ## ########  ## ## ##  ##  ## ## ## ##   ####  ##
##  ##  ## ######### ##   ##   ##  ####  ##  ##  #### ##    ##
##  ##  ## ##     ## ##    ##  ##   ###  ##  ##   ### ##    ##  ####
 ###  ###  ##     ## ##     ## ##    ## #### ##    ##  ######   ####
#####################################################################*/
// Change "gCbfFioState" function pointer order at your own risk!
FioJobPool_t (*gCbfFioState[SCH_STATE_COUNT])(void) =
{
    Sch_WaitRead,
    Sch_WaitWrite,
    Sch_WaitErase,
    Sch_Idle,
    Sch_ProcRead,
    Sch_ProcWrite,
    Sch_ProcErase,
};

// Change "gCbfPLPFioState" function pointer order at your own risk!
FioJobPool_t (*gCbfPLPFioState[SCH_STATE_COUNT])(void) =
{
    Sch_WaitRead,
    Sch_WaitWrite,
    Sch_WaitErase,
    Sch_Idle,
    Sch_ProcPLPRead,
    Sch_ProcPLPWrite,
    Sch_ProcPLPErase,
};

uint32_t gSchActivePowerWeight[FIO_JOB_CMD_COUNT] =
{
    16, //FIO_JOB_CMD_READ
    8,  //FIO_JOB_CMD_READ_FAST
    8,  //FIO_JOB_CMD_READ_SLC
    16, //FIO_JOB_CMD_READ_SLC_TOUCH
    2,  //FIO_JOB_CMD_READ_IDS_PAGE
    1,  //FIO_JOB_CMD_READ_IDS
    16, //FIO_JOB_CMD_PREOPEN
    2,  //FIO_JOB_CMD_RR
    1,  //FIO_JOB_CMD_READ_DISTRIBUTION
    8,  //FIO_JOB_CMD_WRITE
    16  //FIO_JOB_CMD_ERASE
};

#if 0
uint32_t gSchActivePowerWeight[CACHE_STATUS_COUNT] =
{
    12, //CACHE_R_NO_HIT
    8,  //CACHE_R_PARTIAL_HIT,
    0,  //CACHE_R_FULL_HIT,
    0,  //CACHE_R_FULL_HIT_BLANK,
    1,  //CACHE_R_FILLUP_HIT,
    1,  //CACHE_R_IDS
    0,  //CACHE_W_NO_HIT,
    0,  //CACHE_W_PARTIAL_HIT,
    1,  //CACHE_W_FULL_HIT,
    16, //CACHE_W_FUA,
    16, //CACHE_FLUSH
    16, //CACHE_FILLUP,
    16, //CACHE_GCREADAUX,
    16, //CACHE_GCREAD,
    16, //CACHE_GCLASTPAGE,
    16, //CACHE_PREREAD
    16, //CACHE_PREOPEN,
    16, //CACHE_LASTPAGE,
    16, //CACHE_SCANPAGE,
    8,  //CACHE_TCG,
    16, //CACHE_SYSINFO
    16, //CACHE_WRDUMMY,
    0,  //CACHE_AUTO,
    0,  //CACHE_ABORT,
};
#endif

SchInfo_t gSchInfo;


//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    One time initialization for Fio Scheudler

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t AplSch_OneTimeInit(InitBootMode_t initMode)
{
    MEM_CLR(&gSchInfo, sizeof(gSchInfo));

    gSchInfo.state           = SCH_STATE_IDLE;
    gSchInfo.currReadFS      = FS_R_START;
    gSchInfo.currWriteFS     = FS_W_START;
    gSchInfo.currSchPool     = FIO_JOB_POOL_READ;
    gSchInfo.maxLookaheadCnt = gNfInfo.devicesPerChannel * gNfInfo.lunsPerDevice;

    if (gNfInfo.totalLun > FIO_R_POOL_SCHEDULE_COUNT)
    {
        gSchInfo.maxSchPool = FIO_R_POOL_CNT_NORMAL + FIO_R_POOL_SCHEDULE_COUNT;
    }
    else
    {
        gSchInfo.maxSchPool = FIO_R_POOL_CNT_NORMAL + gNfInfo.totalLun;
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Fio read job waiting state

    @return  fio job pool
**/
//-----------------------------------------------------------------------------
FioJobPool_t Sch_WaitRead(void)
{
    if (gAplSeqInfo.trkIssueCnt == gAplSeqInfo.trkDoneCnt)
    {
        #if (THERMAL_THROTTLE_ADAPTIVE_MODE == ENABLE)
        if (smFioInfo.reqTMLevel != smFioInfo.currTMLevel)
        {
            smFioInfo.currTMLevel = smFioInfo.reqTMLevel;
            HalNfif_ChangeTiming(smFioInfo.currTMLevel);
        }
        #endif

        gSchInfo.state = SCH_STATE_PROC_READ;
        return FIO_JOB_POOL_READ;
    }

    return FIO_JOB_POOL_NULL;
}

//-----------------------------------------------------------------------------
/**
    Fio write job waiting state

    @return  fio job pool
**/
//-----------------------------------------------------------------------------
FioJobPool_t Sch_WaitWrite(void)
{
    if (gAplSeqInfo.trkIssueCnt == gAplSeqInfo.trkDoneCnt)
    {
        #if (THERMAL_THROTTLE_ADAPTIVE_MODE == ENABLE)
        if (smFioInfo.reqTMLevel != smFioInfo.currTMLevel)
        {
            smFioInfo.currTMLevel = smFioInfo.reqTMLevel;
            HalNfif_ChangeTiming(smFioInfo.currTMLevel);
        }
        #endif

        gSchInfo.state = SCH_STATE_PROC_WRITE;
        return FIO_JOB_POOL_WRITE;
    }

    return FIO_JOB_POOL_NULL;
}

//-----------------------------------------------------------------------------
/**
    Fio erase job waiting state

    @return  fio job pool
**/
//-----------------------------------------------------------------------------
FioJobPool_t Sch_WaitErase(void)
{
    if (gAplSeqInfo.trkIssueCnt == gAplSeqInfo.trkDoneCnt)
    {
        gSchInfo.state = SCH_STATE_PROC_ERASE;
        return FIO_JOB_POOL_ERASE;
    }

    return FIO_JOB_POOL_NULL;
}

//-----------------------------------------------------------------------------
/**
    Fio job scheduler for idle state

    @return  fio job pool
**/
//-----------------------------------------------------------------------------
FioJobPool_t Sch_Idle(void)
{
    if      (gSchInfo.schJobCnt)                    gSchInfo.state = SCH_STATE_WAIT_READ;
    else if (smFioInfo.jobCnt[FIO_JOB_POOL_ERASE])  gSchInfo.state = SCH_STATE_WAIT_ERASE;
    else if (smFioInfo.jobCnt[FIO_JOB_POOL_WRITE])  gSchInfo.state = SCH_STATE_WAIT_WRITE;

    if (gSchInfo.state < SCH_STATE_IDLE) return gCbfFioState[gSchInfo.state]();

    return FIO_JOB_POOL_NULL;
}

//-----------------------------------------------------------------------------
/**
    Fio read job scheduler

    @return  fio job pool
**/
//-----------------------------------------------------------------------------
FioJobPool_t Sch_ProcRead(void)
{
    #if (THERMAL_THROTTLE_ADAPTIVE_MODE == ENABLE)
    if (smFioInfo.reqTMLevel != smFioInfo.currTMLevel)
    {
        gSchInfo.state = SCH_STATE_WAIT_READ;
        return FIO_JOB_POOL_NULL;
    }
    #endif

    if (gSchInfo.issueReadJobs < SCH_READ_JOB_THRESHOLD && gSchInfo.schJobCnt)
    {
        if ((gSchInfo.issueDieCnt - gSchInfo.doneDieCnt) < SCH_MAX_ACTIVE_READ_DIES)
        {
            U32 cnt = 0;

            do
            {
                gSchInfo.currSchPool = (++gSchInfo.currSchPool == FIO_JOB_POOL_WRITE) ? FIO_JOB_POOL_READ : gSchInfo.currSchPool;

                if (smFioInfo.jobCnt[gSchInfo.currSchPool])
                {
                    return (FioJobPool_t)gSchInfo.currSchPool;
                }
            } while (++cnt < gSchInfo.maxLookaheadCnt);
        }
    }
    else
    {
        gSchInfo.issueReadJobs = 0;

        if      (smFioInfo.jobCnt[FIO_JOB_POOL_ERASE])    gSchInfo.state = SCH_STATE_WAIT_ERASE;
        else if (smFioInfo.jobCnt[FIO_JOB_POOL_WRITE])    gSchInfo.state = SCH_STATE_WAIT_WRITE;

        if (gSchInfo.state < SCH_STATE_IDLE)              return gCbfFioState[gSchInfo.state]();
    }

    return FIO_JOB_POOL_NULL;
}

//-----------------------------------------------------------------------------
/**
    Fio write job scheduler

    @return  fio job pool
**/
//-----------------------------------------------------------------------------
FioJobPool_t Sch_ProcWrite(void)
{
    #if (THERMAL_THROTTLE_ADAPTIVE_MODE == ENABLE)
    if (smFioInfo.reqTMLevel != smFioInfo.currTMLevel)
    {
        gSchInfo.state = SCH_STATE_WAIT_WRITE;
        return FIO_JOB_POOL_NULL;
    }
    #endif

    if (gSchInfo.issueWriteJobs >= SCH_WRITE_JOB_THRESHOLD || (smFioInfo.jobCnt[FIO_JOB_POOL_WRITE] == 0))
    {
        gSchInfo.issueWriteJobs = 0;

        if      (gSchInfo.schJobCnt)                    gSchInfo.state = SCH_STATE_WAIT_READ;
        else if (smFioInfo.jobCnt[FIO_JOB_POOL_ERASE])  gSchInfo.state = SCH_STATE_WAIT_ERASE;

        if (gSchInfo.state < SCH_STATE_IDLE)            return gCbfFioState[gSchInfo.state]();
    }

    if (smFioInfo.jobCnt[FIO_JOB_POOL_ERASE])
    {
        gSchInfo.state = SCH_STATE_WAIT_ERASE;
        return gCbfFioState[gSchInfo.state]();
    }
    else if (smFioInfo.jobCnt[FIO_JOB_POOL_WRITE])
    {
        if ((gAplSeqInfo.trkIssueCnt - gAplSeqInfo.trkDoneCnt) < SCH_MAX_ACTIVE_WRITE_JOBS)
        {
            return FIO_JOB_POOL_WRITE;
        }
    }

    return FIO_JOB_POOL_NULL;
}

//-----------------------------------------------------------------------------
/**
    Fio erase job scheduler

    @return  fio job pool
**/
//-----------------------------------------------------------------------------
FioJobPool_t Sch_ProcErase(void)
{
    if (smFioInfo.jobCnt[FIO_JOB_POOL_ERASE])
    {
        if ((gAplSeqInfo.trkIssueCnt - gAplSeqInfo.trkDoneCnt) < FIO_MAX_ACTIVE_ERASE_JOBS)
        {
            return FIO_JOB_POOL_ERASE;
        }
    }
    else if (smFioInfo.jobCnt[FIO_JOB_POOL_WRITE])  gSchInfo.state = SCH_STATE_WAIT_WRITE;
    else if (gSchInfo.schJobCnt)                    gSchInfo.state = SCH_STATE_WAIT_READ;

    if (gSchInfo.state < SCH_STATE_IDLE)            return gCbfFioState[gSchInfo.state]();

    return FIO_JOB_POOL_NULL;
}

//-----------------------------------------------------------------------------
/**
    Fio read job scheduler (for PLP)

    @return  fio job pool
**/
//-----------------------------------------------------------------------------
FioJobPool_t Sch_ProcPLPRead(void)
{
    if (gSchInfo.schJobCnt && (gSchInfo.issueDieCnt - gSchInfo.doneDieCnt) < SCH_MAX_ACTIVE_READ_DIES)
    {
        U32 cnt = 0;

        do
        {
            gSchInfo.currSchPool = (++gSchInfo.currSchPool == FIO_JOB_POOL_WRITE) ? FIO_JOB_POOL_READ : gSchInfo.currSchPool;

            if (smFioInfo.jobCnt[gSchInfo.currSchPool])
            {
                return (FioJobPool_t)gSchInfo.currSchPool;
            }
        } while (++cnt < gSchInfo.maxLookaheadCnt);
    }
    else if (smFioInfo.jobCnt[FIO_JOB_POOL_ERASE])
    {
        gSchInfo.state = SCH_STATE_WAIT_ERASE;
    }
    else if (smFioInfo.jobCnt[FIO_JOB_POOL_WRITE])
    {
        gSchInfo.state = SCH_STATE_WAIT_WRITE;
    }

    if (gSchInfo.state < SCH_STATE_IDLE)          return gCbfFioState[gSchInfo.state]();

    return FIO_JOB_POOL_NULL;
}

//-----------------------------------------------------------------------------
/**
    Fio write job scheduler (for PLP)

    @return  fio job pool
**/
//-----------------------------------------------------------------------------
FioJobPool_t Sch_ProcPLPWrite(void)
{
    if (gSchInfo.schJobCnt)
    {
        gSchInfo.state = SCH_STATE_WAIT_READ;
    }
    else if (smFioInfo.jobCnt[FIO_JOB_POOL_ERASE])
    {
        gSchInfo.state = SCH_STATE_WAIT_ERASE;
    }
    else if (smFioInfo.jobCnt[FIO_JOB_POOL_WRITE])
    {
        if ((gAplSeqInfo.trkIssueCnt - gAplSeqInfo.trkDoneCnt) < SCH_MAX_ACTIVE_WRITE_JOBS_PLP)
        {
            return FIO_JOB_POOL_WRITE;
        }
    }

    if (gSchInfo.state < SCH_STATE_IDLE)          return gCbfFioState[gSchInfo.state]();

    return FIO_JOB_POOL_NULL;
}

//-----------------------------------------------------------------------------
/**
    Fio erase job scheduler (for PLP)

    @return  fio job pool
**/
//-----------------------------------------------------------------------------
FioJobPool_t Sch_ProcPLPErase(void)
{
    if (gSchInfo.schJobCnt)
    {
        gSchInfo.state = SCH_STATE_WAIT_READ;
    }
    else if (smFioInfo.jobCnt[FIO_JOB_POOL_ERASE])
    {
        if ((gAplSeqInfo.trkIssueCnt - gAplSeqInfo.trkDoneCnt) < SCH_MAX_ACTIVE_ERASE_JOBS_PLP)
        {
            return FIO_JOB_POOL_ERASE;
        }
    }
    else if (smFioInfo.jobCnt[FIO_JOB_POOL_WRITE])
    {
        gSchInfo.state = SCH_STATE_WAIT_WRITE;
    }

    if (gSchInfo.state < SCH_STATE_IDLE)  return gCbfPLPFioState[gSchInfo.state]();

    return FIO_JOB_POOL_NULL;
}

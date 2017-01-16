#pragma once

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define AGING_MLC_ERASE_JOB   4
#define AGING_MLC_WRITE_JOB   4
#define AGING_MLC_READ_JOB    2

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define AGING_ERASE_MLC_SPEED_DOWN(JOBCNT) {if (JOBCNT == AGING_MLC_ERASE_JOB) {JOBCNT = 0; Aging_JobWait();} else {JOBCNT++;}}
#define AGING_WRITE_MLC_SPEED_DOWN(JOBCNT) {if (JOBCNT == AGING_MLC_WRITE_JOB) {JOBCNT = 0; Aging_JobWait();} else {JOBCNT++;}}

#define AGING_READ_MLC_SPEED_DOWN(JOBCNT) { \
    if (JOBCNT == AGING_MLC_READ_JOB)   \
    {                                   \
        JOBCNT = 0;                     \
        Aging_JobWait();                \
        if (gAgingLoop.LoopErrReadCnt)  \
        {                               \
            Aging_MLC_ProcECC();        \
        }                               \
    }                                   \
    else                                \
    {                                   \
        JOBCNT++;                       \
    }                                   \
}

#define AGING_MLC_START_UPDATE_STATE(BLOCK, STATE) {                                                                                                \
    gAgingInfo.LogBlock = BLOCK;                                                                                                                    \
    gAgingInfo.State    = STATE;                                                                                                                    \
    smSysInfo->d.AgingData.d.AgingLoop.LoopDefectCnt[gAgingInfo.CurrLoopCnt + 1].BadBlockCnt = gAgingInfo.State;                                    \
    Aging_JobWait();                                                                                                                                \
    if (BLOCK == AGING_START_BLK)                                                                                                                   \
    {                                                                                                                                               \
        Aging_Util_ReBuildTable();                                                                                                                  \
        SyIn_Synchronize(SI_AREA_MISC | SI_AREA_PLIST2 | SI_AREA_AGING | SI_AREA_MPINFO, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);                    \
    }                                                                                                                                               \
}                                                                                                                                                   \

#define AGING_MLC_END_UPDATE_STATE(BLOCK, STATE) {                                                                                                  \
    gAgingInfo.LogBlock = BLOCK;                                                                                                                    \
    gAgingInfo.State    = STATE;                                                                                                                    \
    smSysInfo->d.AgingData.d.AgingLoop.LoopDefectCnt[gAgingInfo.CurrLoopCnt + 1].BadBlockCnt = gAgingInfo.State;                                    \
    Aging_JobWait();                                                                                                                                \
    if (BLOCK == (AGING_END_BLK - 1))                                                                                                               \
    {                                                                                                                                               \
        smSysInfo->d.AgingEI.d.ErrToggleStart = smSysInfo->d.AgingEI.d.ErrPaaCnt;                                                                   \
        smSysInfo->d.AgingEI.d.ErrEccStart    = smSysInfo->d.AgingEI.d.ErrPaaCnt;                                                                   \
        smSysInfo->d.AgingEI.d.ErrRetryStart  = smSysInfo->d.AgingEI.d.ErrPaaCnt;                                                                   \
        if (gAgingLoop.LoopDefectCnt)                                                                                                               \
        {                                                                                                                                           \
            Aging_Util_BubbleSort();                                                                                                                \
            Aging_MLC_ProcECC();                                                                                                                    \
        }                                                                                                                                           \
        SyIn_Synchronize(SI_AREA_MISC | SI_AREA_PLIST2 | SI_AREA_AGING | SI_AREA_MPINFO | SI_AREA_AGINGEI, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);  \
    }                                                                                                                                               \
}
//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------


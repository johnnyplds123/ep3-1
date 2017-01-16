#pragma once

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define AGING_BLOCK_LEVEL   DISABLE

#define AGING_WRITE_G_LEVEL_LOOP 30
#define AGING_REFRESH_TIME       179999

#define AGING_3DTLC_ERASE_JOB    4
#define AGING_3DTLC_WRITE_JOB    5
#define AGING_3DTLC_READ_JOB     4
#define AGING_3DTLC_SLCTOUCH_JOB 4
//-----------------------------------------------------------------------------

#define AGING_NAND_CS1          0
#define AGING_NAND_CS1_REFINE   1
#define AGING_NAND_CS2          2

enum
{
    AGING_FLASHID_CS1_CS1REFINE=0x7,
    AGING_FLASHID_CS2_CS1=0x20,         ///BIT(5)
};

#define AIGNG_NAND_STATE AGING_NAND_CS2

//-----------------------------------------------------------------------------

#define AGING_WL_PER_LAYER  4
#define AGING_FIRST_LAYER   0
#define AGING_LAST_LAYER    ((gNfInfo.wlPerBlock - 1) / AGING_WL_PER_LAYER)

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define AGING_SLCTOUCH_3DTLC_SPEED_DOWN(JOBCNT) {if (JOBCNT == AGING_3DTLC_SLCTOUCH_JOB) {JOBCNT = 0; Aging_JobWait();} else {JOBCNT++;}}
#define AGING_ERASE_3DTLC_SPEED_DOWN(JOBCNT) {if (JOBCNT == AGING_3DTLC_ERASE_JOB) {JOBCNT = 0; Aging_JobWait();} else {JOBCNT++;}}
#define AGING_WRITE_3DTLC_SPEED_DOWN(JOBCNT) {if (JOBCNT == AGING_3DTLC_WRITE_JOB) {JOBCNT = 0; Aging_JobWait();} else {JOBCNT++;}}

#define AGING_READ_3DTLC_SPEED_DOWN(JOBCNT) {   \
    if (JOBCNT == AGING_3DTLC_READ_JOB)   \
    {                                     \
        JOBCNT = 0;                       \
        Aging_JobWait();                  \
        if (gAgingLoop.LoopErrReadCnt)    \
        {                                 \
            Aging_3DTLC_ProcToggle();     \
            Aging_3DTLC_ProcECC();        \
            Aging_3DTLC_ProcReadRetry();  \
            Aging_3DTLC_ProcReadDistribution(); \
        }                                 \
    }                                     \
    else                                  \
    {                                     \
        JOBCNT++;                         \
    }                                     \
}

#define AGING_3DTLC_START_UPDATE_STATE(BLOCK, STATE) {                                                                            \
    gAgingInfo.LogBlock = BLOCK;                                                                                                  \
    gAgingInfo.State    = STATE;                                                                                                  \
    smSysInfo->d.AgingData.d.AgingLoop.LoopDefectCnt[gAgingInfo.CurrLoopCnt + 1].BadBlockCnt = gAgingInfo.State;                  \
    Aging_JobWait();                                                                                                              \
    if (BLOCK == AGING_START_BLK)                                                                                                 \
    {                                                                                                                             \
        Aging_Util_ReBuildTable();                                                                                                \
        SyIn_Synchronize(SI_AREA_MISC | SI_AREA_PLIST2 | SI_AREA_AGING | SI_AREA_MPINFO, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);  \
    }                                                                                                                             \
}


#define AGING_3DTLC_END_UPDATE_STATE(BLOCK, STATE) {                                                                                                \
    gAgingInfo.LogBlock = BLOCK;                                                                                                                    \
    gAgingInfo.State    = STATE;                                                                                                                    \
    smSysInfo->d.AgingData.d.AgingLoop.LoopDefectCnt[gAgingInfo.CurrLoopCnt + 1].BadBlockCnt = gAgingInfo.State;                                    \
    Aging_JobWait();                                                                                                                                \
    if (BLOCK == (AGING_END_BLK - 1))                                                                                                               \
    {                                                                                                                                               \
        if (gAgingLoop.LoopDefectCnt)                                                                                                               \
        {                                                                                                                                           \
            Aging_Util_BubbleSort();                                                                                                                \
            Aging_3DTLC_ProcECC();                                                                                                                  \
            Aging_3DTLC_ProcReadDistribution();                                                                                                     \
        }                                                                                                                                           \
        smSysInfo->d.AgingEI.d.ErrToggleStart = smSysInfo->d.AgingEI.d.ErrPaaCnt;                                                                   \
        smSysInfo->d.AgingEI.d.ErrEccStart    = smSysInfo->d.AgingEI.d.ErrPaaCnt;                                                                   \
        smSysInfo->d.AgingEI.d.ErrRetryStart  = smSysInfo->d.AgingEI.d.ErrPaaCnt;                                                                   \
        smSysInfo->d.AgingEI.d.ErrDisStart    = smSysInfo->d.AgingEI.d.ErrPaaCnt;                                                                   \
        SyIn_Synchronize(SI_AREA_MISC | SI_AREA_PLIST2 | SI_AREA_AGING | SI_AREA_MPINFO | SI_AREA_AGINGEI, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);  \
    }                                                                                                                                               \
}

#define AGING_3DTLCG2_DUMMY(STARTBLOCK, ENDBLOCK) {      \
    Aging_JobWait();                                     \
    Aging_3DTLC_SLCTouch(STARTBLOCK, ENDBLOCK);          \
    Aging_JobWait();                                     \
    HalTimer_DelayMsec(100);                             \
}

#define AGING_3DTLCG2_REFRESH(TICK, STARTBLOCK, ENDBLOCK) { \
    if (HalTimer_GetElapsedTimeMsec(TICK) >= AGING_REFRESH_TIME) \
    {                                                            \
        Aging_JobWait();                                         \
        Aging_3DTLC_SLCTouch(STARTBLOCK, ENDBLOCK);              \
        Aging_JobWait();                                         \
        TICK = HalTimer_ReadTick();                              \
    }                                                            \
}

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------

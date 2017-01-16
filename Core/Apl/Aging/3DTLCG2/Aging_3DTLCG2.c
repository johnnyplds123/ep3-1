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
//! \file
//! @brief FIO(Flash Input/Output) control API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#if _CPU_==CPU_ID1
    #include "MediaPkg.h"
#endif

#include "RegEcu.h"
#include "RegApb.h"
#include "RegDmac.h"
#include "RegSequencer.h"

#include "Fio.h"
#include "Cache.h"
#include "CoreDbg.h"
#include "SysInfo.h"
#include "Heap.h"
#include "Core.h"
#include "Thermal.h"

#include "Aging.h"
#include "Aging_3DTLCG2_Addons.h"
#include "Aging_3DTLCG2.h"

#include "FastSection.h"

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
U8 gAg_3DTLC_FlashID[3][8] = {{0x98, 0x3E, 0x99, 0xB3, 0x7A, 0xF1, 0x0D, 0x33},  //3DTLC CS1
                              {0x98, 0x3E, 0x99, 0xB3, 0x7A, 0xF1, 0x0E, 0x33},  //3DTLC CS1 Refine
                              {0x98, 0x3E, 0x99, 0xB3, 0x7A, 0xF1, 0x00, 0x13}}; //3DTLC CS2

//-----------------------------------------------------------------------------
//  Private function proto-type definitions:
//-----------------------------------------------------------------------------
static tERROR_AGING Aging_3DTLC_Block_Level(void);
static tERROR_AGING Aging_3DTLC_Dummy(void);
static tERROR_AGING Aging_3DTLC_Refresh(void);
static tERROR_AGING Aging_3DTLC_Maunal(void);
static tERROR_AGING Aging_3DTLC_Erase(U16 block);
static tERROR_AGING Aging_3DTLC_Read(U16 block);
static tERROR_AGING Aging_3DTLC_Write(U16 block);
static tERROR_AGING Aging_3DTLC_ManualWrite(U16 block);
static tERROR_AGING Aging_3DTLC_SLCTouch(U16 startBlk, U16 endBlk);
static void Aging_3DTLC_Pattern_Random(U8 *wBuf, U32 totalSector);
static void Aging_3DTLC_Pattern_fix(U8 *wBuf, U32 totalSector, U16 parameter);
static void Aging_3DTLC_ProcErase(U16 block, U8 ce, U8 lun);
static void Aging_3DTLC_ProcWrite(U16 block, U8 ce, U8 lun, U16 wl);
static void Aging_3DTLC_ProcRead(U16 block, U8 ce, U8 lun, U16 page);
static void Aging_3DTLC_ProcSLCTouch(U16 block, U8 ce, U8 lun);
static void Aging_3DTLC_ProcManualWrite(U16 block, U8 ce, U8 lun, U16 wl);
static void Aging_3DTLC_ProcToggle(void);
static void Aging_3DTLC_ProcReadRetry(void);
static void Aging_3DTLC_ProcECC(void);
static void Aging_3DTLC_ProcReadDistribution(void);
//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
U32 onenum[512] = {0};
//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
#if CHK_FW_TYPE(FW_AGING)
//-------------------------------------------------------------------
// Function     : tERROR_AGING Aging_Operator(U16 loop)
// Description  :
// Input        : N/A
// return       : gAgingInfo.ErrorCode
//-------------------------------------------------------------------
tERROR_AGING Aging_Operator(U16 loop)
{
    #if (AGING_BLOCK_LEVEL == ENABLE)
    if (Aging_3DTLC_Block_Level()!= ERR_NO_ERROR) return gAgingInfo.ErrorCode;
    #else
    if ((loop + 1) % AGING_WRITE_G_LEVEL_LOOP == 0)
    {
        if (Aging_3DTLC_Maunal() != ERR_NO_ERROR)  return gAgingInfo.ErrorCode;
    }
    if (Aging_3DTLC_Refresh() != ERR_NO_ERROR)  return gAgingInfo.ErrorCode;
    #endif
    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_3DTLC_Block_Level(void)
// Description  :
// Input        : N/A
// return       : gAgingInfo.ErrorCode;
//-------------------------------------------------------------------
static tERROR_AGING Aging_3DTLC_Block_Level(void)
{
    U32 tick;
    U16 block;
    U8  type;

    for (type = 0; type < 2; type++)
    {
        for (block = AGING_START_BLK; block < AGING_END_BLK; block++)
        {
            tick = HalTimer_ReadTick();
            switch (type)
            {
                case 0: ///Erase
                    Aging_3DTLC_Erase(block);
                    break;
                case 1: ///Write/Read
                    Aging_3DTLC_Write(block);
                    Aging_3DTLC_Read(block);
                    break;
            }
            gAgingInfo.TotalTime += HalTimer_GetElapsedTimeMsec(tick);
            AGING_SAVELOOPTIME(gAgingInfo.ReRunCnt, gAgingInfo.CurrLoopCnt, HalTimer_GetElapsedTimeMsec(tick));
        }
    }

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_3DTLC_Dummy(void))
// Description  : Write -> Dummy Read
// Input        : N/A
// return       : ErrorCode
//-------------------------------------------------------------------
static tERROR_AGING Aging_3DTLC_Dummy(void)
{
    U32 tick;
    U16 block;
    U8  type;

    for (type = 0; type < 3; type++)
    {
        for (block = AGING_START_BLK; block < AGING_END_BLK; block++)
        {
            tick = HalTimer_ReadTick();
            switch (type)
            {
                case 0: ///Erase
                    Aging_3DTLC_Erase(block);
                    break;
                case 1: ///Write
                    Aging_3DTLC_Write(block);
                    break;
                case 2: ///Read + Dummy
                    AGING_3DTLCG2_DUMMY(block, block+1)
                    Aging_3DTLC_Read(block);
                    break;
            }
            gAgingInfo.TotalTime += HalTimer_GetElapsedTimeMsec(tick);
            AGING_SAVELOOPTIME(gAgingInfo.ReRunCnt, gAgingInfo.CurrLoopCnt, HalTimer_GetElapsedTimeMsec(tick));
        }
    }

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_3DTLC_Refresh(void)
// Description  : write -> read refresh
// Input        :
// return       : ErrorCode
//-------------------------------------------------------------------
static tERROR_AGING Aging_3DTLC_Refresh(void)
{
    U32 tick, totalTick;
    U16 block;
    U8  type;

    tick = HalTimer_ReadTick();

    for (type = 0; type < 3; type++)
    {
        for (block = AGING_START_BLK; block < AGING_END_BLK; block++)
        {
            totalTick = HalTimer_ReadTick();
            switch (type)
            {
                case 0: ///Erase
                    Aging_3DTLC_Erase(block);
                    break;
                case 1: ///Write + Refresh
                    Aging_3DTLC_Write(block);
                    AGING_3DTLCG2_REFRESH(tick, AGING_START_BLK, block+1);
                    break;
                case 2: ///Read + Refresh
                    AGING_3DTLCG2_REFRESH(tick, block, AGING_END_BLK);
                    Aging_3DTLC_Read(block);
                    break;
            }
            gAgingInfo.TotalTime += HalTimer_GetElapsedTimeMsec(totalTick);
            AGING_SAVELOOPTIME(gAgingInfo.ReRunCnt, gAgingInfo.CurrLoopCnt, HalTimer_GetElapsedTimeMsec(totalTick));
        }
    }

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_3DTLC_Maunal(void)
// Description  : All cell write to pattern G
// Input        : N/A
// return       : ErrorCode
//-------------------------------------------------------------------
static tERROR_AGING Aging_3DTLC_Maunal(void)
{
    U32 tick;
    U16 block;
    U8  type;

    for (type = 0; type < 2; type++)
    {
        for (block = AGING_START_BLK; block < AGING_END_BLK; block++)
        {
            tick = HalTimer_ReadTick();
            switch (type)
            {
                case 0: ///Erase
                    Aging_3DTLC_Erase(block);
                    break;
                case 1: ///ManualWrite
                    Aging_3DTLC_ManualWrite(block);
                    break;
            }
            gAgingInfo.TotalTime += HalTimer_GetElapsedTimeMsec(tick);
            AGING_SAVELOOPTIME(gAgingInfo.ReRunCnt, gAgingInfo.CurrLoopCnt, HalTimer_GetElapsedTimeMsec(tick));
        }
    }

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : Aging_3DTLC_SLCTouch(U16 startBlk, U16 endBlk)
// Description  : slc touch -> no toggle data
// Input        : startBlk, endBlk
// return       : ErrorCode
//-------------------------------------------------------------------
static tERROR_AGING Aging_3DTLC_SLCTouch(U16 startBlk, U16 endBlk)
{
    U16 block, jobCnt = 0;
    U8  lun, ce;

    D_PRINTF("[SLC ] StartBlk|0x%3x EndBlk|0x%3x\n", startBlk, endBlk);

    for (block = startBlk; block < endBlk; block++)
    {
        if (block >= gNfInfo.blocksPerPlane) break;
        for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
        {
            for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
            {
                Aging_3DTLC_ProcSLCTouch(block, ce, lun);
            }
        }
        AGING_SLCTOUCH_3DTLC_SPEED_DOWN(jobCnt);
    }
    Aging_JobWait();

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_3DTLC_Erase(U16 block)
// Description  :
// Input        : block
// return       : ErrorCode
//-------------------------------------------------------------------
static tERROR_AGING Aging_3DTLC_Erase(U16 block)
{
    U16 jobCnt = 0, maxSOC = 0, maxI2C = 0;
    U8  lun, ce;

    AGING_3DTLC_START_UPDATE_STATE(block, STATE_ERASE_FAIL);

    for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
    {
        for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
        {
            switch (Aging_CheckIssueSBlk(block, ce, lun))
            {
                case STATUS_ISSUE_SBLK:
                    if  ((block % 0xFF) == 0) D_PRINTF("[AG E] SUPERBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                    Aging_3DTLC_ProcErase(block, ce, lun);
                    break;
                case STATUS_ISSUE_NON_SBLK:
                    D_PRINTF_AG("[AG E] NonSUBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                    Aging_3DTLC_ProcErase(block, ce, lun);
                    break;
                default:
                    D_PRINTF_AG("[AG E] NonIssue PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                    break;
            }
            AGING_ERASE_3DTLC_SPEED_DOWN(jobCnt);
        }
    }
    AGING_TEMPERATURE(maxSOC, maxI2C);

    AGING_3DTLC_END_UPDATE_STATE(block, STATE_ERASE_PASS);

    gAgingInfo.TotalLoopErase = gAgingInfo.CurrLoopCnt;

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_3DTLC_Write(U16 block)
// Description  :
// Input        : block
// return       : ErrorCode
//-------------------------------------------------------------------
static tERROR_AGING Aging_3DTLC_Write(U16 block)
{
    U16 die, wl, jobCnt = 0, maxSOC = 0, maxI2C = 0;
    U8  lun, ce;

    AGING_3DTLC_START_UPDATE_STATE(block, STATE_WRITE_FAIL);

    Aging_3DTLC_Pattern_Random(gCoreDbgBuf.wBuffer, 192);

    for (wl = 0; wl < gNfInfo.wlPerBlock; wl++)
    {
        for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
        {
            for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
            {
                switch (Aging_CheckIssueSBlk(block, ce, lun))
                {
                    case STATUS_ISSUE_SBLK:
                        if (((block % 100) == 0) && wl == 0) D_PRINTF("[AG W] SUPERBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                        Aging_3DTLC_ProcWrite(block, ce, lun, wl);
                        break;
                    case STATUS_ISSUE_NON_SBLK:
                        if  (wl == 0) D_PRINTF("[AG W] NonSUBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                        Aging_3DTLC_ProcWrite(block, ce, lun, wl);
                        break;
                    default:
                        if  (wl == 0) D_PRINTF("[AG W] NonIssue PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                        break;
                }
                AGING_WRITE_3DTLC_SPEED_DOWN(jobCnt);
            }
        }
        AGING_TEMPERATURE(maxSOC, maxI2C);
    }
    AGING_3DTLC_END_UPDATE_STATE(block, STATE_WRITE_PASS);

    gAgingInfo.TotalLoopWrite = gAgingInfo.CurrLoopCnt;

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_3DTLC_Read(U16 block)
// Description  :
// Input        : block
// return       : ErrorCode
//-------------------------------------------------------------------
static tERROR_AGING Aging_3DTLC_Read(U16 block)
{
    U16 die, page, jobCnt = 0, maxSOC = 0, maxI2C = 0;
    U8  lun, ce, readUnit;

    AGING_3DTLC_START_UPDATE_STATE(block, STATE_READ_FAIL);

    readUnit = (block < AGING_SLC_BLOCK) ? 3 : 1;
    for (page = 0; page < gNfInfo.pagesPerBlock; page+=readUnit)
    {
        for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
        {
            for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
            {
                switch (Aging_CheckIssueSBlk(block, ce, lun))
                {
                    case STATUS_ISSUE_SBLK:
                        if (((block % 100) == 0) && page == 0) D_PRINTF("[AG R] SUPERBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                        Aging_3DTLC_ProcRead(block, ce, lun, page);
                        break;
                    case STATUS_ISSUE_NON_SBLK:
                        if (page == 0) D_PRINTF("[AG R] NonSUBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                        Aging_3DTLC_ProcRead(block, ce, lun, page);
                        break;
                    default:
                        if (page == 0) D_PRINTF("[AG R] NonIssue PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                        break;
                }
                AGING_READ_3DTLC_SPEED_DOWN(jobCnt);
            }
        }
        AGING_TEMPERATURE(maxSOC, maxI2C);
    }
    AGING_3DTLC_END_UPDATE_STATE(block, STATE_READ_PASS);

    gAgingInfo.TotalLoopRead = gAgingInfo.CurrLoopCnt;

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : Aging_3DTLC_ManualWrite(U16 block)
// Description  :
// Input        : block
// return       : ErrorCode
//-------------------------------------------------------------------
static tERROR_AGING Aging_3DTLC_ManualWrite(U16 block)
{
    U16 wl, jobCnt = 0;
    U8  lun, ce;

    AGING_3DTLC_START_UPDATE_STATE(block, STATE_WRITE_FAIL);

    if (block < gAgingInfo.MinFreeIdx) Aging_3DTLC_Pattern_fix(gCoreDbgBuf.wBuffer, 192, cGLevel);

    for (wl = 0; wl < gNfInfo.wlPerBlock; wl++)
    {
        for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
        {
            for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
            {
                switch (Aging_CheckIssueSBlk(block, ce, lun))
                {
                    case STATUS_ISSUE_SBLK:
                        if (((block % 100) == 0) && wl == 0) D_PRINTF("[AG W] Manual SUPERBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                        Aging_3DTLC_ProcManualWrite(block, ce, lun, wl);
                        break;
                    case STATUS_ISSUE_NON_SBLK:
                        if  (wl == 0) D_PRINTF("[AG W] Manual NonSUBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                        Aging_3DTLC_ProcManualWrite(block, ce, lun, wl);
                        break;
                    default:
                        if  (wl == 0) D_PRINTF("[AG W] Manual NonIssue PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                }
                AGING_WRITE_3DTLC_SPEED_DOWN(jobCnt);
            }
        }
    }
    AGING_3DTLC_END_UPDATE_STATE(block, STATE_WRITE_PASS);

    gAgingInfo.TotalLoopWrite = gAgingInfo.CurrLoopCnt;

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static void Aging_3DTLC_ProcErase(U16 block, U8 ce, U8 lun)
// Description  :
// Input        : block, ce, lun
// return       : N/A
//-------------------------------------------------------------------
static void Aging_3DTLC_ProcErase(U16 block, U8 ce, U8 lun)
{
    tFIO_JOB *pJob[2];
    U32 idx;
    U16 die, rmpblk, cnt1, cnt2;
    U8  ch, plane;

    cnt1 = cnt2 = 0;

    for (ch = 0; ch < gNfInfo.numChannels; ch++)
    {
        for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
        {
            die = AGING_GET_DIE(lun, ce ,ch);
            idx = AGING_GET_SBLK_IDX(plane, block, die);
            rmpblk  = gwAgRemapTbl[idx];

            // --- MultiPlane Erase ------------------------------------------------------------------------
            if  ((gbAgflagTbl[die] == 0) && (rmpblk != AGING_BAD_BLOCK))
            {
                gAgingFastPaa1Buf[cnt1].all32   = 0;
                gAgingFastPaa1Buf[cnt1].b.ch    = ch;
                gAgingFastPaa1Buf[cnt1].b.ce    = ce;
                gAgingFastPaa1Buf[cnt1].b.lun   = lun;
                gAgingFastPaa1Buf[cnt1].b.plane = plane;
                gAgingFastPaa1Buf[cnt1].b.block = rmpblk;
                cnt1++;
            }
            // --- SinglePlane Erase ------------------------------------------------------------------------
            else if (rmpblk != AGING_BAD_BLOCK)
            {
                gAgingFastPaa2Buf[cnt2].all32   = 0;
                gAgingFastPaa2Buf[cnt2].b.ch    = ch;
                gAgingFastPaa2Buf[cnt2].b.ce    = ce;
                gAgingFastPaa2Buf[cnt2].b.lun   = lun;
                gAgingFastPaa2Buf[cnt2].b.plane = plane;
                gAgingFastPaa2Buf[cnt2].b.block = rmpblk;
                cnt2++;
            }
        }
    }
    // --- Issue MultiPlane Erase -------------------------------------------------------------------------------------------------------------
    if (cnt1)
    {
        pJob[0]         = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob[0]->plane  = gNfInfo.planesPerLun;
        pJob[0]->cmd    = FIO_JOB_CMD_ERASE;
        pJob[0]->paaCnt = cnt1;
        pJob[0]->css    = CACHE_AUTO;
        pJob[0]->flag   = FIO_F_NO_MKERR;
        if (block < AGING_SLC_BLOCK) pJob[0]->flag |= FIO_F_SLC;

        MEM_COPY(pJob[0]->paa, gAgingFastPaa1Buf, sizeof(tPAA) * cnt1);

        Aging_JobIssue(pJob[0]);
    }
    // --- Issue SinglePlane Erase ----------------------------------------------------------------------------------
    if (cnt2)
    {
        pJob[1]         = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob[1]->plane  = FIO_CMD_1PLANE;
        pJob[1]->cmd    = FIO_JOB_CMD_ERASE;
        pJob[1]->css    = CACHE_AUTO;
        pJob[1]->paaCnt = cnt2;
        pJob[1]->flag   = FIO_F_NO_MKERR;
        if (block < AGING_SLC_BLOCK) pJob[1]->flag |= FIO_F_SLC;

        MEM_COPY(pJob[1]->paa, gAgingFastPaa2Buf, sizeof(tPAA) * cnt2);

        Aging_JobIssue(pJob[1]);
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_3DTLC_ProcWrite(U16 block, U8 ce, U8 lun, U16 wl)
// Description  :
// Input        : block, ce, lun, wl
// return       : N/A
//-------------------------------------------------------------------
static void Aging_3DTLC_ProcWrite(U16 block, U8 ce, U8 lun, U16 wl)
{
    tFIO_JOB *pJob[2];
    U32 idx;
    U16 die, rmpblk, cnt1, cnt2;
    U8  ch, plane, frag, lmu, progUnit;

    cnt1 = cnt2 = 0;

    progUnit = (block < AGING_SLC_BLOCK) ? 3 : 1;

    for (lmu = 0; lmu < gNfInfo.bitsPerCell; lmu+=progUnit)
    {
        for (ch = 0; ch < gNfInfo.numChannels; ch++)
        {
            for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
            {
                die = AGING_GET_DIE(lun, ce ,ch);
                idx = AGING_GET_SBLK_IDX(plane, block, die);
                rmpblk  = gwAgRemapTbl[idx];
                for (frag = 0; frag < gEcuInfo.auPerPage; frag++)
                {
                    // --- MultiPlane Write ------------------------------------------------------------------------
                    if ((gbAgflagTbl[die] == 0) && (rmpblk != AGING_BAD_BLOCK))
                    {
                        gAgingFastPaa1Buf[cnt1].all32   = 0;
                        gAgingFastPaa1Buf[cnt1].b.ch    = ch;
                        gAgingFastPaa1Buf[cnt1].b.ce    = ce;
                        gAgingFastPaa1Buf[cnt1].b.lun   = lun;
                        gAgingFastPaa1Buf[cnt1].b.plane = plane;
                        gAgingFastPaa1Buf[cnt1].b.block = rmpblk;
                        gAgingFastPaa1Buf[cnt1].b.page  = wl * gNfInfo.bitsPerCell + lmu;
                        gAgingFastPaa1Buf[cnt1].b.frag  = frag;
                        cnt1++;
                    }
                    // --- SinglePlane Write ------------------------------------------------------------------------
                    else if (rmpblk != AGING_BAD_BLOCK)
                    {
                        gAgingFastPaa2Buf[cnt2].all32   = 0;
                        gAgingFastPaa2Buf[cnt2].b.ch    = ch;
                        gAgingFastPaa2Buf[cnt2].b.ce    = ce;
                        gAgingFastPaa2Buf[cnt2].b.lun   = lun;
                        gAgingFastPaa2Buf[cnt2].b.plane = plane;
                        gAgingFastPaa2Buf[cnt2].b.block = rmpblk;
                        gAgingFastPaa2Buf[cnt2].b.page  = wl * gNfInfo.bitsPerCell + lmu;
                        gAgingFastPaa2Buf[cnt2].b.frag  = frag;
                        cnt2++;
                    }
                    else break;
                }
            }
        }
    }
    // --- Issue MultiPlane Write -------------------------------------------------------------------------
    if (cnt1)
    {
        pJob[0]            = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob[0]->plane     = gNfInfo.planesPerLun;
        pJob[0]->pBuffer   = gCoreDbgBuf.wBuffer;
        pJob[0]->cmd       = FIO_JOB_CMD_WRITE;
        pJob[0]->css       = CACHE_AUTO;
        pJob[0]->auFmt     = 0;
        pJob[0]->paaCnt    = cnt1;
        pJob[0]->flag      = FIO_F_NO_MKERR;
        if (block < AGING_SLC_BLOCK) pJob[0]->flag |= FIO_F_SLC;

        MEM_COPY(pJob[0]->paa, gAgingFastPaa1Buf, sizeof(tPAA) * cnt1);

        Aging_JobIssue(pJob[0]);
    }
    // --- Issue SinglePlane Write ---------------------------------------------------------------------
    if (cnt2)
    {
        pJob[1]            = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob[1]->plane     = FIO_CMD_1PLANE;
        pJob[1]->pBuffer   = gCoreDbgBuf.wBuffer;
        pJob[1]->cmd       = FIO_JOB_CMD_WRITE;
        pJob[1]->css       = CACHE_AUTO;
        pJob[1]->auFmt     = 0;
        pJob[1]->paaCnt    = cnt2;
        pJob[1]->flag      = FIO_F_NO_MKERR;
        if (block < AGING_SLC_BLOCK) pJob[1]->flag |= FIO_F_SLC;

        MEM_COPY(pJob[1]->paa, gAgingFastPaa2Buf, sizeof(tPAA) * cnt2);

        Aging_JobIssue(pJob[1]);
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_3DTLC_ProcRead(U16 block, U8 ce, U8 lun, U16 page)
// Description  :
// Input        : block, ce, lun, page
// return       : N/A
//-------------------------------------------------------------------
static void Aging_3DTLC_ProcRead(U16 block, U8 ce, U8 lun, U16 page)
{
    tFIO_JOB *pJob[2];
    U32 idx;
    U16 die, rmpblk, cnt1, cnt2;
    U8  ch, plane, frag;

    cnt1 = cnt2 = 0;

    for (ch = 0; ch < gNfInfo.numChannels; ch++)
    {
        for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
        {
            die = AGING_GET_DIE(lun, ce ,ch);
            idx = AGING_GET_SBLK_IDX(plane, block, die);
            rmpblk  = gwAgRemapTbl[idx];
            for (frag = 0; frag < gEcuInfo.auPerPage; frag++)
            {
                // --- MultiPlane Read ------------------------------------------------------------------------
                if ((gbAgflagTbl[die] == 0) && (rmpblk != AGING_BAD_BLOCK))
                {
                    gAgingFastPaa1Buf[cnt1].all32   = 0;
                    gAgingFastPaa1Buf[cnt1].b.ch    = ch;
                    gAgingFastPaa1Buf[cnt1].b.ce    = ce;
                    gAgingFastPaa1Buf[cnt1].b.lun   = lun;
                    gAgingFastPaa1Buf[cnt1].b.plane = plane;
                    gAgingFastPaa1Buf[cnt1].b.block = rmpblk;
                    gAgingFastPaa1Buf[cnt1].b.page  = page;
                    gAgingFastPaa1Buf[cnt1].b.frag  = frag;
                    cnt1++;
                }
                // --- SinglePlane Read ------------------------------------------------------------------------
                else if (rmpblk != AGING_BAD_BLOCK)
                {
                    gAgingFastPaa2Buf[cnt2].all32   = 0;
                    gAgingFastPaa2Buf[cnt2].b.ch    = ch;
                    gAgingFastPaa2Buf[cnt2].b.ce    = ce;
                    gAgingFastPaa2Buf[cnt2].b.lun   = lun;
                    gAgingFastPaa2Buf[cnt2].b.plane = plane;
                    gAgingFastPaa2Buf[cnt2].b.block = rmpblk;
                    gAgingFastPaa2Buf[cnt2].b.page  = page;
                    gAgingFastPaa2Buf[cnt2].b.frag  = frag;
                    cnt2++;
                }
                else break;
            }
        }
    }
    // --- Issue MultiPlane Open Read -------------------------------------------------------------------------
    if (cnt1)
    {
        pJob[0]            = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob[0]->plane     = gNfInfo.planesPerLun;
        pJob[0]->pBuffer   = gCoreDbgBuf.rBuffer;
        pJob[0]->css       = CACHE_AUTO;
        pJob[0]->paaCnt    = cnt1;
        pJob[0]->flag      = FIO_F_NO_MKERR | FIO_F_NO_RR;
        if (gAgingInfo.CurrLoopCnt % 2) pJob[0]->flag |= FIO_F_PAGE_READ;

        if (block < AGING_SLC_BLOCK)
        {
            pJob[0]->flag |= FIO_F_SLC;
            pJob[0]->cmd   = FIO_JOB_CMD_READ_SLC;
        }
        else
        {
            pJob[0]->cmd   = FIO_JOB_CMD_READ;
        }

        MEM_COPY(pJob[0]->paa, gAgingFastPaa1Buf, sizeof(tPAA) * cnt1);

        Aging_JobIssue(pJob[0]);
    }
    // --- Issue SinglePlane Open Read -----------------------------------------------------------------
    if (cnt2)
    {
        pJob[1]            = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob[1]->plane     = FIO_CMD_1PLANE;
        pJob[1]->pBuffer   = gCoreDbgBuf.rBuffer;
        pJob[1]->css       = CACHE_AUTO;
        pJob[1]->paaCnt    = cnt2;
        pJob[1]->flag      = FIO_F_NO_MKERR | FIO_F_NO_RR;
        if (gAgingInfo.CurrLoopCnt % 2) pJob[1]->flag |= FIO_F_PAGE_READ;

        if (block < AGING_SLC_BLOCK)
        {
            pJob[1]->flag |= FIO_F_SLC;
            pJob[1]->cmd   = FIO_JOB_CMD_READ_SLC;
        }
        else
        {
            pJob[1]->cmd   = FIO_JOB_CMD_READ;
        }

        MEM_COPY(pJob[1]->paa, gAgingFastPaa2Buf, sizeof(tPAA) * cnt2);

        Aging_JobIssue(pJob[1]);
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_3DTLC_ProcSLCTouch(U16 block, U8 ce, U8 lun)
// Description  :
// Input        : block, ce, lun
// return       : N/A
//-------------------------------------------------------------------
static void Aging_3DTLC_ProcSLCTouch(U16 block, U8 ce, U8 lun)
{
    tFIO_JOB* pJob = NULL;
    U32 idx;
    U16 die, rmpblk, page, cnt = 0;
    U8  ch, plane;

    page = (gNfInfo.pagesPerBlock - 3);

    // --- MultiPlane Read ------------------------------------------------------------------------
    for (ch = 0; ch < gNfInfo.numChannels; ch++)
    {
        for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
        {
            die = AGING_GET_DIE(lun, ce ,ch);
            idx = AGING_GET_SBLK_IDX(plane, block, die);
            rmpblk  = gwAgRemapTbl[idx];
            if (rmpblk != AGING_BAD_BLOCK)
            {
                gAgingFastPaa1Buf[cnt].all32   = 0;
                gAgingFastPaa1Buf[cnt].b.ch    = ch;
                gAgingFastPaa1Buf[cnt].b.ce    = ce;
                gAgingFastPaa1Buf[cnt].b.lun   = lun;
                gAgingFastPaa1Buf[cnt].b.plane = plane;
                gAgingFastPaa1Buf[cnt].b.block = rmpblk;
                gAgingFastPaa1Buf[cnt].b.page  = page;
                cnt++;
            }
        }
    }
        // --- Issue SLC MultiPlane Read -------------------------------------------------------------------------
    if (cnt)
    {
        pJob            = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob->plane     = gNfInfo.planesPerLun;
        pJob->pBuffer   = gCoreDbgBuf.rBuffer;
        pJob->css       = CACHE_AUTO;
        pJob->paaCnt    = cnt;
        pJob->cmd       = FIO_JOB_CMD_READ_SLC_TOUCH;
        pJob->flag      = FIO_F_NO_MKERR | FIO_F_NO_RR | FIO_F_SLC;

        MEM_COPY(pJob->paa, gAgingFastPaa1Buf, sizeof(tPAA) * cnt);
        Aging_JobIssue(pJob);
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_3DTLC_ProcManualWrite(U16 block, U8 ce, U8 lun, U16 wl)
// Description  : ///Disable ECC INV & Scramble
// Input        : block, ce, lun, wl
// return       : N/A
//-------------------------------------------------------------------
static void Aging_3DTLC_ProcManualWrite(U16 block, U8 ce, U8 lun, U16 wl)
{
    tFIO_JOB *pJob[2];
    U32 idx;
    U16 die, rmpblk, cnt1, cnt2;
    U8  ch, plane, frag, lmu;

    cnt1 = cnt2 = 0;

    for (lmu = 0; lmu < gNfInfo.bitsPerCell; lmu++)
    {
        for (ch = 0; ch < gNfInfo.numChannels; ch++)
        {
            for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
            {
                die = AGING_GET_DIE(lun, ce ,ch);
                idx = AGING_GET_SBLK_IDX(plane, block, die);
                rmpblk  = gwAgRemapTbl[idx];
                for (frag = 0; frag < gEcuInfo.auPerPage; frag++)
                {
                    // --- MultiPlane Write ------------------------------------------------------------------------
                    if ((gbAgflagTbl[die] == 0) && (rmpblk != AGING_BAD_BLOCK))
                    {
                        gAgingFastPaa1Buf[cnt1].all32   = 0;
                        gAgingFastPaa1Buf[cnt1].b.ch    = ch;
                        gAgingFastPaa1Buf[cnt1].b.ce    = ce;
                        gAgingFastPaa1Buf[cnt1].b.lun   = lun;
                        gAgingFastPaa1Buf[cnt1].b.plane = plane;
                        gAgingFastPaa1Buf[cnt1].b.block = rmpblk;
                        gAgingFastPaa1Buf[cnt1].b.page  = wl * gNfInfo.bitsPerCell + lmu;
                        gAgingFastPaa1Buf[cnt1].b.frag  = frag;
                        cnt1++;
                    }
                    // --- SinglePlane Write ------------------------------------------------------------------------
                    else if (rmpblk != AGING_BAD_BLOCK)
                    {
                        gAgingFastPaa2Buf[cnt2].all32   = 0;
                        gAgingFastPaa2Buf[cnt2].b.ch    = ch;
                        gAgingFastPaa2Buf[cnt2].b.ce    = ce;
                        gAgingFastPaa2Buf[cnt2].b.lun   = lun;
                        gAgingFastPaa2Buf[cnt2].b.plane = plane;
                        gAgingFastPaa2Buf[cnt2].b.block = rmpblk;
                        gAgingFastPaa2Buf[cnt2].b.page  = wl * gNfInfo.bitsPerCell + lmu;
                        gAgingFastPaa2Buf[cnt2].b.frag  = frag;
                        cnt2++;
                    }
                    else break;
                }
            }
        }
    }
    // --- Issue MultiPlane Write -------------------------------------------------------------------------
    if (cnt1)
    {
        if (wl == 0 && block >= gAgingInfo.MinFreeIdx) Aging_3DTLC_Pattern_fix(gCoreDbgBuf.wBuffer, cnt1, cGLevel);
        pJob[0]            = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob[0]->plane     = gNfInfo.planesPerLun;
        pJob[0]->pBuffer   = gCoreDbgBuf.wBuffer;
        pJob[0]->cmd       = FIO_JOB_CMD_WRITE;
        pJob[0]->css       = CACHE_AUTO;
        pJob[0]->auFmt     = AU_FORMAT_DISSCAMBLE;
        pJob[0]->paaCnt    = cnt1;
        pJob[0]->flag      = FIO_F_NO_MKERR;

        MEM_COPY(pJob[0]->paa, gAgingFastPaa1Buf, sizeof(tPAA) * cnt1);

        Aging_JobIssue(pJob[0]);
    }
    // --- Issue SinglePlane Write -------------------------------------------------------------------------
    if (cnt2)
    {
        if (wl == 0) Aging_3DTLC_Pattern_fix(gCoreDbgBuf.wBuffer, cnt2, cGLevel);
        pJob[1]            = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob[1]->plane     = FIO_CMD_1PLANE;
        pJob[1]->pBuffer   = gCoreDbgBuf.wBuffer;
        pJob[1]->cmd       = FIO_JOB_CMD_WRITE;
        pJob[1]->css       = CACHE_AUTO;
        pJob[1]->auFmt     = AU_FORMAT_DISSCAMBLE;
        pJob[1]->paaCnt    = cnt2;
        pJob[1]->flag      = FIO_F_NO_MKERR;

        MEM_COPY(pJob[1]->paa, gAgingFastPaa2Buf, sizeof(tPAA) * cnt2);

        Aging_JobIssue(pJob[1]);
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_3DTLC_ProcToggle(void)
// Description  : No Busy, Toggle Only
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_3DTLC_ProcToggle(void)
{
    volatile EcuDecStat2_t* ecuDecStat2   = (EcuDecStat2_t*)0xF200C054;
    volatile EcuDecStat3_t* ecuDecStat3   = (EcuDecStat3_t*)0xF200C056;
    volatile EcuDecStat4_t* ecuDecStat4   = (EcuDecStat4_t*)0xF200C058;
    volatile EcuDecStat21_t* ecuDecStat21 = (EcuDecStat21_t*)0xF200C192;

    tFIO_JOB* pJob = NULL;
    U32  cnt;
    U16  read_ptr;
    U8   frag, aucnt, cwxcnt, eccValue = 0;

    for (cnt = smSysInfo->d.AgingEI.d.ErrToggleStart; cnt < smSysInfo->d.AgingEI.d.ErrPaaCnt; cnt++)
    {
        if (smSysInfo->d.AgingEI.d.Data[cnt].ErrType != ERASE && smSysInfo->d.AgingEI.d.Data[cnt].ErrType != PROGRAM)
        {
            pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
            pJob->css     = CACHE_AUTO;
            pJob->plane   = FIO_CMD_1PLANE;
            pJob->paaCnt  = gEcuInfo.auPerPage;
            pJob->pBuffer = gCoreDbgBuf.rBuffer;
            pJob->cmd     = FIO_JOB_CMD_READ;
            pJob->flag    = FIO_F_NO_MKERR | FIO_F_NO_RR | FIO_F_MUTE;

            for (frag = 0; frag < pJob->paaCnt; frag++)
            {
                pJob->paa[frag].all32   = 0;
                pJob->paa[frag].b.ch    = smSysInfo->d.AgingEI.d.Data[cnt].Ch;
                pJob->paa[frag].b.ce    = smSysInfo->d.AgingEI.d.Data[cnt].Ce;
                pJob->paa[frag].b.lun   = smSysInfo->d.AgingEI.d.Data[cnt].Lun;
                pJob->paa[frag].b.plane = (smSysInfo->d.AgingEI.d.Data[cnt].Block) % gNfInfo.planesPerLun;
                pJob->paa[frag].b.block = (smSysInfo->d.AgingEI.d.Data[cnt].Block) / gNfInfo.planesPerLun;
                pJob->paa[frag].b.page  = smSysInfo->d.AgingEI.d.Data[cnt].Page;
                pJob->paa[frag].b.frag  = frag;
            }

            Aging_JobIssue(pJob);
            Aging_JobWait();

            smSysInfo->d.AgingEI.d.Data[cnt].ToggleStatus = pJob->status;

            D_PRINTF("[TOGG] S|%c PAA[CH%d|CE%d|LN%d|PN%d|BK0x%3x|PBK0x%4x|PG|0x%3x]\n",
                pJob->status,
                smSysInfo->d.AgingEI.d.Data[cnt].Ch,
                smSysInfo->d.AgingEI.d.Data[cnt].Ce,
                smSysInfo->d.AgingEI.d.Data[cnt].Lun,
                smSysInfo->d.AgingEI.d.Data[cnt].Block % gNfInfo.planesPerLun,
                smSysInfo->d.AgingEI.d.Data[cnt].Block / gNfInfo.planesPerLun,
                smSysInfo->d.AgingEI.d.Data[cnt].Block,
                smSysInfo->d.AgingEI.d.Data[cnt].Page);
            read_ptr = 0;
            read_ptr = (ecuDecStat21->b.CUR_16HISTORY_READ_PTR);

            //--- Read ECC------------------------------------------------------------------------
            for (aucnt = 0; aucnt < gEcuInfo.auPerPage; aucnt++)
            {
                ecuDecStat2->b.AU_DEC_STATUS_PTR = (16 + read_ptr - (gEcuInfo.auPerPage - aucnt - 1)) & BYTE_MASK;

                for (cwxcnt = 0; cwxcnt < gEcuInfo.cwPerAu; cwxcnt++)
                {
                    if (ecuDecStat3->all & 0x0040)      eccValue = 0xE0;  //Empty                                                                                                                //Uncorrectable
                    else if (ecuDecStat3->all & 0x0002) eccValue = 0xF0;  //Uncorrectable
                    else
                    {
                        ecuDecStat2->b.CW_DEC_STAT_PTR = cwxcnt;
                        eccValue = ecuDecStat4->b.CWX_CORR_CNT;
                    }
                    smSysInfo->d.AgingEI.d.Data[cnt].ToggleCw[aucnt * gEcuInfo.cwPerAu + cwxcnt] = eccValue;

                    D_PRINTF("[TOGG] CW%d|%x \n",(aucnt * gEcuInfo.cwPerAu + cwxcnt), eccValue);
                }
            }
        }
    }
    smSysInfo->d.AgingEI.d.ErrToggleStart = smSysInfo->d.AgingEI.d.ErrPaaCnt;
}

//-------------------------------------------------------------------
// Function     : static void Aging_3DTLC_ProcReadDistribution(void)
// Description  : Read Distribution
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_3DTLC_ProcReadDistribution(void)
{
    volatile SeqCfg3_t*   SeqCfg3 = (SeqCfg3_t*)0xF200D004;
    tFIO_JOB* pJob = NULL;
    U32  cnt, idx, xferSize;
    U16  showbit;

    xferSize = gEcuInfo.xferSizePerAu * gEcuInfo.auPerPage;
    SeqCfg3->b.AUX_SIZE = 0;

    for (cnt = smSysInfo->d.AgingEI.d.ErrDisStart; cnt < smSysInfo->d.AgingEI.d.ErrPaaCnt; cnt++)
    {
        pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob->css     = CACHE_AUTO;
        pJob->plane   = FIO_CMD_1PLANE;
        pJob->paaCnt  = 1;
        pJob->pBuffer = gbAgBuffer;
        pJob->cmd     = FIO_JOB_CMD_READ_DISTRIBUTION;
        pJob->flag    = FIO_F_NO_MKERR | FIO_F_MUTE | FIO_F_NO_RR;
        pJob->auFmt   = AU_FORMAT_READ_TOTAL_AU;

        pJob->paa[0].all32    = 0;
        pJob->paa[0].b.ch     = smSysInfo->d.AgingEI.d.Data[cnt].Ch;
        pJob->paa[0].b.ce     = smSysInfo->d.AgingEI.d.Data[cnt].Ce;
        pJob->paa[0].b.lun    = smSysInfo->d.AgingEI.d.Data[cnt].Lun;
        pJob->paa[0].b.plane  = (smSysInfo->d.AgingEI.d.Data[cnt].Block) % gNfInfo.planesPerLun;
        pJob->paa[0].b.block  = (smSysInfo->d.AgingEI.d.Data[cnt].Block) / gNfInfo.planesPerLun;
        pJob->paa[0].b.page   = smSysInfo->d.AgingEI.d.Data[cnt].Page;
        pJob->paa[0].b.frag   = 0;

        Aging_JobIssue(pJob);
        Aging_JobWait();

        for (idx = 0; idx < 512; idx++)
        {
            onenum[idx] = 0;

            if(HalDmac_StartCount0((U32)&gbAgBuffer[xferSize*idx], xferSize) != cEcNoError)  break;
            while(!rDmac.mskdDmacStat.b.DMAC_SDRAM_TST_CMPLT);
            if(HalDmac_WaitDone()!=cEcNoError) break;
            onenum[idx] = rDmac.ZERO_COUNT;
            rDmac.mskdDmacStat.b.DMAC_SDRAM_TST_CMPLT = 0;
        }

        for (idx = 0; idx < 512; idx++)
        {
			showbit = Aging_3DTLC_DisOut(idx, (U32*)onenum);
            smSysInfo->d.AgingEI.d.Data[cnt].Distribution[idx] = showbit;
        }

        D_PRINTF("[DIS ] PAA[CH%d|CE%d|LN%d|PN%d|BK0x%3x|PBK0x%4x|PG|0x%3x]\n",
            smSysInfo->d.AgingEI.d.Data[cnt].Ch,
            smSysInfo->d.AgingEI.d.Data[cnt].Ce,
            smSysInfo->d.AgingEI.d.Data[cnt].Lun,
            smSysInfo->d.AgingEI.d.Data[cnt].Block % gNfInfo.planesPerLun,
            smSysInfo->d.AgingEI.d.Data[cnt].Block / gNfInfo.planesPerLun,
            smSysInfo->d.AgingEI.d.Data[cnt].Block,
            smSysInfo->d.AgingEI.d.Data[cnt].Page);
    }
    SeqCfg3->b.AUX_SIZE = 3;
    smSysInfo->d.AgingEI.d.ErrDisStart = smSysInfo->d.AgingEI.d.ErrPaaCnt;
}

//-------------------------------------------------------------------
// Function     : static void Aging_3DTLC_ProcReadRetry(void)
// Description  : Read Retry
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_3DTLC_ProcReadRetry(void)
{
    tFIO_JOB* pJob = NULL;
    U32  cnt;
    U8   frag = 0;

    for (cnt = smSysInfo->d.AgingEI.d.ErrRetryStart; cnt < smSysInfo->d.AgingEI.d.ErrPaaCnt; cnt++)
    {
        if (smSysInfo->d.AgingEI.d.Data[cnt].ErrType != ERASE &&
            smSysInfo->d.AgingEI.d.Data[cnt].ErrType != PROGRAM &&
            smSysInfo->d.AgingEI.d.Data[cnt].EccStatus != PASS)
        {
            pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
            pJob->css     = CACHE_AUTO;
            pJob->plane   = FIO_CMD_1PLANE;
            pJob->paaCnt  = gEcuInfo.auPerPage;
            pJob->pBuffer = gCoreDbgBuf.rBuffer;
            pJob->cmd     = FIO_JOB_CMD_READ;
            pJob->flag    = FIO_F_NO_MKERR | FIO_F_MUTE;

            for (frag = 0; frag < pJob->paaCnt; frag++)
            {
                pJob->paa[frag].all32    = 0;
                pJob->paa[frag].b.ch     = smSysInfo->d.AgingEI.d.Data[cnt].Ch;
                pJob->paa[frag].b.ce     = smSysInfo->d.AgingEI.d.Data[cnt].Ce;
                pJob->paa[frag].b.lun    = smSysInfo->d.AgingEI.d.Data[cnt].Lun;
                pJob->paa[frag].b.plane  = (smSysInfo->d.AgingEI.d.Data[cnt].Block) % gNfInfo.planesPerLun;
                pJob->paa[frag].b.block  = (smSysInfo->d.AgingEI.d.Data[cnt].Block) / gNfInfo.planesPerLun;
                pJob->paa[frag].b.page   = smSysInfo->d.AgingEI.d.Data[cnt].Page;
                pJob->paa[frag].b.frag   = frag;
            }
            Aging_JobIssue(pJob);
            Aging_JobWait();

            smSysInfo->d.AgingEI.d.Data[cnt].RetryCnt = pJob->rrCount;

            D_PRINTF("[RR  ] S|%1X RetryCnt|%d PAA[CH%d|CE%d|LN%d|PN%d|BK0x%3x|PBK0x%4x|PG|0x%3x]\n",
                pJob->status,
                smSysInfo->d.AgingEI.d.Data[cnt].RetryCnt,
                smSysInfo->d.AgingEI.d.Data[cnt].Ch,
                smSysInfo->d.AgingEI.d.Data[cnt].Ce,
                smSysInfo->d.AgingEI.d.Data[cnt].Lun,
                smSysInfo->d.AgingEI.d.Data[cnt].Block % gNfInfo.planesPerLun,
                smSysInfo->d.AgingEI.d.Data[cnt].Block / gNfInfo.planesPerLun,
                smSysInfo->d.AgingEI.d.Data[cnt].Block,
                smSysInfo->d.AgingEI.d.Data[cnt].Page);
        }
    }
    smSysInfo->d.AgingEI.d.ErrRetryStart = smSysInfo->d.AgingEI.d.ErrPaaCnt;
}

//-------------------------------------------------------------------
// Function     : static void Aging_3DTLC_ProcECC(void)
// Description  : Read Ecc
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_3DTLC_ProcECC(void)
{
    tPAA paa, errPaa;
    U32  cnt;
    U16  status, wl, layer, errWL, errLayer, scanStartLayer, scanLayerCnt, errType = 0, type = 0;
    U8   lmu = 0, group;

    for (cnt = smSysInfo->d.AgingEI.d.ErrEccStart; cnt < smSysInfo->d.AgingEI.d.ErrPaaCnt; cnt++)
    {
        type           = 0;
        errPaa.all32   = paa.all32 = 0;
        errPaa.b.ch    = smSysInfo->d.AgingEI.d.Data[cnt].Ch;
        errPaa.b.ce    = smSysInfo->d.AgingEI.d.Data[cnt].Ce;
        errPaa.b.lun   = smSysInfo->d.AgingEI.d.Data[cnt].Lun;
        errPaa.b.page  = smSysInfo->d.AgingEI.d.Data[cnt].Page;
        errPaa.b.plane = (smSysInfo->d.AgingEI.d.Data[cnt].Block) % gNfInfo.planesPerLun;
        errPaa.b.block = (smSysInfo->d.AgingEI.d.Data[cnt].Block) / gNfInfo.planesPerLun;
        paa.all32      = errPaa.all32;

        if (smSysInfo->d.AgingEI.d.Data[cnt].Type != ERASE)
        {
            errWL = (errPaa.b.page / gNfInfo.bitsPerCell);
            errLayer = (errPaa.b.page / gNfInfo.bitsPerCell) / AGING_WL_PER_LAYER;

            if (smSysInfo->d.AgingEI.d.Data[cnt].Type == PROGRAM)
            {
                scanLayerCnt   = (errLayer != AGING_FIRST_LAYER && errLayer != AGING_LAST_LAYER) ? 3 : 2;
                scanStartLayer = (errLayer == AGING_FIRST_LAYER)                                 ? errLayer : (errLayer - 1);
            }
            else
            {
                scanLayerCnt   = (errLayer != AGING_LAST_LAYER) ? 2 : 1;
                scanStartLayer = errLayer;
            }
            for (layer = scanStartLayer; layer < (scanStartLayer + scanLayerCnt); layer++)
            {
                status = 0;
                for (wl = 0; wl < AGING_WL_PER_LAYER; wl++)
                {
                    for (lmu = 0; lmu < gNfInfo.bitsPerCell; lmu++)
                    {
                        paa.b.page = (layer * AGING_WL_PER_LAYER + wl) * gNfInfo.bitsPerCell + lmu;
                        if (smSysInfo->d.AgingEI.d.Data[cnt].Type == PROGRAM)
                        {
                            if (errWL == (paa.b.page / gNfInfo.bitsPerCell))    type = PROGRAM;         /// errwl == scan wl
                            else if (errLayer == layer)                         type = PROG_WL_OPEN;    /// errlayer == scan layer
                            else                                                type = PROG_WL_SHORT;   /// scan layer == pre errlayer || post errlayer
                        }
                        else
                        {
                            if (paa.b.page < errPaa.b.page) continue;
                            if (errWL == (paa.b.page / gNfInfo.bitsPerCell))    type = READ;            /// errwl == scan wl
                            else if (errLayer == layer)                         type = READ_WL_OPEN;    /// errlayer == scan layer
                            else                                                type = READ_WL_SHORT;   /// scan layer == pre errlayer || post errlayer
                        }
                        D_PRINTF("[ECC ] %c PAA[CH%d|CE%d|LN%d|PN%d|BK0x%3x|PBK0x%4x|PG|0x%3x]\n",
                                    (smSysInfo->d.AgingEI.d.Data[cnt].Type == PROGRAM) ? 'P' : 'R',
                                    paa.b.ch,
                                    paa.b.ce,
                                    paa.b.lun,
                                    paa.b.plane,
                                    paa.b.block,
                                    smSysInfo->d.AgingEI.d.Data[cnt].Block,
                                    paa.b.page);
                        status |= Aging_Nand_EccScan(paa, type, cnt);
                    }
                }
                if (status != 0)
                {
                    errType = type;
                }
            }
            if (errType != 0 && (smSysInfo->d.AgingEI.d.Data[cnt].Type != PROG_WL_SHORT || smSysInfo->d.AgingEI.d.Data[cnt].Type != READ_WL_SHORT))
            {
                switch (errType)
                {
                    case PROG_WL_SHORT:
                        smSysInfo->d.AgingEI.d.Data[cnt].Type                   = PROG_WL_SHORT;
                        smSysInfo->d.AgingData.d.AgingPdinfo.LastFail_Prog_WL_Short = gAgingInfo.CurrLoopCnt;
                        break;
                    case PROG_WL_OPEN:
                        smSysInfo->d.AgingEI.d.Data[cnt].Type                   = PROG_WL_OPEN;
                        smSysInfo->d.AgingData.d.AgingPdinfo.LastFail_Prog_WL_Open  = gAgingInfo.CurrLoopCnt;
                        break;
                    case READ_WL_SHORT:
                        smSysInfo->d.AgingEI.d.Data[cnt].Type                   = READ_WL_SHORT;
                        smSysInfo->d.AgingData.d.AgingPdinfo.LastFail_Read_WL_Short = gAgingInfo.CurrLoopCnt;
                        break;
                    case READ_WL_OPEN:
                        smSysInfo->d.AgingEI.d.Data[cnt].Type                   = READ_WL_OPEN;
                        smSysInfo->d.AgingData.d.AgingPdinfo.LastFail_Read_WL_Open  = gAgingInfo.CurrLoopCnt;
                        break;
                    default:
                        break;
                }
            }
        }
        else
        {
            D_PRINTF("[ECC ] E PAA[CH%d|CE%d|LN%d|PN%d|BK0x%3x|PBK0x%4x]\n",
                paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, smSysInfo->d.AgingEI.d.Data[cnt].Block);
            Aging_Nand_EccScan(paa, type, cnt);
        }
    }
    smSysInfo->d.AgingEI.d.ErrEccStart = smSysInfo->d.AgingEI.d.ErrPaaCnt;
}

//-------------------------------------------------------------------
// Function     : static void Aging_3DTLC_Pattern_Random(U8 *wBuf, U32 totalSector)
// Description  :
// Input        : wBuf, tBuf, totalSector
// return       : N/A
//-------------------------------------------------------------------
static void Aging_3DTLC_Pattern_Random(U8 *wBuf, U32 totalSector)
{
    U32 sector, cnt, tick;

    tick = HalTimer_ReadTick();
    srand(tick);

    for (sector = 0; sector < totalSector ; sector++)  // 2 * Track size
    {
        for (cnt = 0; cnt < 0x1000UL; cnt++)  //1Sectors=512bytes=0x200bytes, 0x1000=4kbytes
        {
             wBuf[sector*0x1000UL + cnt] = (U8) rand();
        }
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_3DTLC_Pattern_fix(U8 *wBuf, U32 totalSector, U16 parameter)
// Description  :
// Input        : wBuf, totalSector
// return       : N/A
//-------------------------------------------------------------------
static void Aging_3DTLC_Pattern_fix(U8 *wBuf, U32 totalSector, U16 parameter)
{
    U32 *wBuffer, sector, sectorPart, cnt, pattern_L, pattern_M, pattern_U;

    pattern_L = pattern_M = pattern_U = 0;

    switch(parameter)
    {
        case cALevel:
            pattern_L = 0x00;
            pattern_M = 0xFFFFFFFF;
            pattern_U = 0xFFFFFFFF;
            break;
        case cBLevel:
            pattern_L = 0x00;
            pattern_M = 0x00;
            pattern_U = 0xFFFFFFFF;
            break;
        case cCLevel:
            pattern_L = 0x00;
            pattern_M = 0x00;
            pattern_U = 0x00;
            break;
        case cDLevel:
            pattern_L = 0x00;
            pattern_M = 0xFFFFFFFF;
            pattern_U = 0x00;
            break;
        case cELevel:
            pattern_L = 0xFFFFFFFF;
            pattern_M = 0xFFFFFFFF;
            pattern_U = 0x00;
            break;
        case cFLevel:
            pattern_L = 0xFFFFFFFF;
            pattern_M = 0x00;
            pattern_U = 0x00;
            break;
        case cGLevel:
            pattern_L = 0xFFFFFFFF;
            pattern_M = 0x00;
            pattern_U = 0xFFFFFFFF;
            break;
        default:
            break;
    }

    wBuffer = (U32 *)wBuf;
    sectorPart = totalSector / gNfInfo.bitsPerCell;

    for (sector = 0; sector < totalSector; sector++)
    {
        if (sector < sectorPart)
        {
            for (cnt = 0; cnt < 0x1000UL; cnt++)
            {
                 wBuffer[sector*0x1000UL + cnt] = pattern_L;
            }
        }
        else if (sector < sectorPart * 2)
        {
            for (cnt = 0; cnt < 0x1000UL; cnt++)
            {
                 wBuffer[sector*0x1000UL + cnt] = pattern_M;
            }
        }
        else if (sector < sectorPart * 3)
        {
            for (cnt = 0; cnt < 0x1000UL; cnt++)
            {
                 wBuffer[sector*0x1000UL + cnt] = pattern_U;
            }
        }
    }
}

//-------------------------------------------------------------------
// Function     : void Aging_EraseAll(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Aging_EraseAll(void)
{
    U16 block, jobCnt = 0;
    U8  lun, ce;

    Aging_Util_ReBuildTable();

    for (block = AGING_START_BLK; block < gNfInfo.blocksPerPlane; block++)
    {
        for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
        {
            for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
            {
                switch (Aging_CheckIssueSBlk(block, ce, lun))
                {
                    case STATUS_ISSUE_SBLK:
                        if  ((block % 0xFF) == 0) D_PRINTF("[AG E] SUPERBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d\n", ce, lun, block, HalTempSensor_GetTemperature());
                        Aging_3DTLC_ProcErase(block, ce, lun);
                        break;
                    case STATUS_ISSUE_NON_SBLK:
                        D_PRINTF("[AG E] NonSUBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d\n", ce, lun, block, HalTempSensor_GetTemperature());
                        Aging_3DTLC_ProcErase(block, ce, lun);
                        break;
                    default:
                        D_PRINTF("[AG E] NonIssue PAA[CE%d|LN|%d|BK0x%3x] SOC|%d\n", ce, lun, block, HalTempSensor_GetTemperature());
                        break;
                }
                AGING_ERASE_3DTLC_SPEED_DOWN(jobCnt);
            }
        }
    }
    Aging_JobWait();
}

//-------------------------------------------------------------------
// Function     : tERROR_AGING Aging_Nand_FlashID(void)
// Description  : NandFlashId when Aging
// Input        :
// return       : AgingInfo.ErrorCode
//-------------------------------------------------------------------
tERROR_AGING Aging_Nand_FlashID(void)
{
    MsgPhyRwe_t     pHcmdMsg;
    tAGING_FLASHID  *FlashID;
    U8 ch, ce, lun, type, error = 0;

    Core_JobWait();

    for (type = 0; type < 2; type++)   ///type 0 : Check Flash ID type 1 : Check Nand State
    {
        MEM_CLR(&pHcmdMsg, sizeof(MsgPhyRwe_t));
        pHcmdMsg.pBuffer  = (uint8_t*)smSSD[SSD_IF_START].startAddr;
        FlashID           = (tAGING_FLASHID *)pHcmdMsg.pBuffer;
        pHcmdMsg.param[0] = (type == 0) ? 0 : 3;
        MSG_SET_SERVICE_INIT(pHcmdMsg, cSfnNfimReadId);
        Ipc_SendWaitMsg(cC2mOtherQueue, (Msg_t *)&pHcmdMsg, (Msg_t *)&pHcmdMsg, IPC_NO_TIMEOUT);

        for (ch = 0; ch < gNfInfo.numChannels; ch++)
        {
            for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
            {
                for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
                {
                    if (type == 0)
                    {
                        MEM_COPY(&smSysInfo->d.AgingData.d.AgingFlashID.FlashID[ce][ch][lun][0], &FlashID->ID[0], 8);
                        error = MEM_CMP(&gAg_3DTLC_FlashID[AIGNG_NAND_STATE][0],&FlashID->ID[0],6);
                    }
                    else
                    {
                        MEM_COPY(&smSysInfo->d.AgingData.d.AgingFlashID.FlashID[ce][ch][lun][6], &FlashID->ID[6], 2);
                        FlashID->ID[7]          = FlashID->ID[7] & (AGING_FLASHID_CS2_CS1);
                        gAg_3DTLC_FlashID[AIGNG_NAND_STATE][7] = gAg_3DTLC_FlashID[AIGNG_NAND_STATE][7] & (AGING_FLASHID_CS2_CS1);
                        error = MEM_CMP(&gAg_3DTLC_FlashID[AIGNG_NAND_STATE][7],&FlashID->ID[7],1);
                        #if (AIGNG_NAND_STATE != AGING_NAND_CS2)
                        if (error == 0)
                        {
                            FlashID->ID[6]          = FlashID->ID[6] & (AGING_FLASHID_CS1_CS1REFINE);
                            gAg_3DTLC_FlashID[AIGNG_NAND_STATE][6] = gAg_3DTLC_FlashID[AIGNG_NAND_STATE][6] & (AGING_FLASHID_CS1_CS1REFINE);
                            error = MEM_CMP(&gAg_3DTLC_FlashID[AIGNG_NAND_STATE][6],&FlashID->ID[6],1);
                        }
                        #endif
                    }
                    if  (error != 0)
                    {
                        gAgingInfo.ErrorCode = ERR_FLASH_ID;
                        smSysInfo->d.AgingData.d.AgingFlashID.FlashIDErrorCode = ERR_FLASH_ID;
                        smSysInfo->d.AgingData.d.AgingFlashID.FlashIDErrorCnt++;
                        smSysInfo->d.AgingData.d.AgingFlashID.FlashIDmap[ch] |= BIT((lun << gNfInfo.bitsDevice)|ce);
                    }
                    FlashID++;
                }
            }
        }
    }
    if((smSysInfo->d.AgingData.d.AgingFlashID.FlashIDErrorCnt) && (gAgingInfo.ErrorCode == ERR_NO_ERROR))
    {
        gAgingInfo.ErrorCode = ERR_FLASH_ID;
    }

    SyIn_Synchronize(SI_AREA_AGING | SI_AREA_MPINFO, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : void Aging_Nand_BusyTime(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Aging_Nand_BusyTime(void)
{
    tPAA paa;
    U32  busyTimeTemp, busyTimeTotal, busyTimeMax, idx;
    U16  die, rmpblk, block, page;
    U8   lun, ce, ch, plane, type;

    paa.all32 = 0;
    block     = 1;
    plane     = 0;
    busyTimeTemp = busyTimeMax = busyTimeTotal = 0;

    if (smSysInfo->d.AgingData.d.AgingNandBusyTime.NBTimeValidTag != AGING_NBTI)
    {
        MEM_CLR(&smSysInfo->d.AgingData.d.AgingNandBusyTime, sizeof(tAG_NAND_BUSYTIME));

        smSysInfo->d.Header.d.SubHeader.d.Aging.d.dwValidTag      = SI_TAG_AGING;
        smSysInfo->d.Header.d.SubHeader.d.Aging.d.dwVerNo         = SI_VER_AGING;
        smSysInfo->d.AgingData.d.AgingNandBusyTime.NBTimeValidTag = AGING_NBTI;

        Aging_Util_ReBuildTable();

        Aging_JobWait();

        for (type = 0; type < 4; type++, busyTimeMax = 0, busyTimeTotal = 0)
        {
            for (die = 0; die < gNfInfo.totalLun; die++, busyTimeTemp = 0)
            {
                idx = AGING_GET_SBLK_IDX(plane, block, die);
                rmpblk  = gwAgRemapTbl[idx];

                ch  = die & AGING_CH_MASK;
                ce  = (die >> gNfInfo.bitsCh) & AGING_CE_MASK;
                lun = (die >> gNfInfo.bitsChDev) & AGING_LUN_MASK;
                paa.all32    = 0;
                paa.b.block  = rmpblk;
                paa.b.ch     = ch;
                paa.b.ce     = ce;
                paa.b.lun    = lun;
                paa.b.plane  = plane;

                Core_JobWait();

                if (type == 0)
                {
                    CoreDbg_Center(COREDBG_CMD_ERASE, 0, paa, 0, 0, 0);
                    Core_JobWait();
                }
                else if (type == 1)
                {
                    for (page = 0; page < gNfInfo.pagesPerBlock; page+=gNfInfo.pagesPerProgUnit)  //WL
                    {
                        paa.b.page = page;
                        CoreDbg_Center(COREDBG_CMD_WRITE, 0, paa, 0, 0, 0);
                        Core_JobWait();
                        busyTimeTemp += smNandBusyTime;
                    }
                    smSysInfo->d.AgingData.d.AgingNandBusyTime.Program[die] = (busyTimeTemp / gNfInfo.wlPerBlock);

                    busyTimeTotal +=  smSysInfo->d.AgingData.d.AgingNandBusyTime.Program[die];
                    if (busyTimeMax < smSysInfo->d.AgingData.d.AgingNandBusyTime.Program[die])
                    {
                        busyTimeMax = smSysInfo->d.AgingData.d.AgingNandBusyTime.Program[die];
                    }
                    if (die == (gNfInfo.totalLun - 1))
                    {
                        smSysInfo->d.AgingData.d.AgingPdinfo.TpAvg = (U16)(busyTimeTotal / gNfInfo.totalLun);
                        smSysInfo->d.AgingData.d.AgingPdinfo.TpMax = (U16)busyTimeMax;
                    }
                }
                else if (type == 2)
                {
                    for (page = 0; page < gNfInfo.pagesPerBlock; page++)  //Page0---->0
                    {
                        paa.b.page  = page;
                        CoreDbg_Center(COREDBG_CMD_READ, 0, paa, 0, 0, 0);
                        Core_JobWait();
                        busyTimeTemp += smNandBusyTime;
                    }

                    smSysInfo->d.AgingData.d.AgingNandBusyTime.Read[die] = (busyTimeTemp / gNfInfo.pagesPerBlock);

                    busyTimeTotal += smSysInfo->d.AgingData.d.AgingNandBusyTime.Read[die];

                    if (busyTimeMax < smSysInfo->d.AgingData.d.AgingNandBusyTime.Read[die])
                    {
                        busyTimeMax = smSysInfo->d.AgingData.d.AgingNandBusyTime.Read[die];
                    }
                    if (die == (gNfInfo.totalLun - 1))
                    {
                        smSysInfo->d.AgingData.d.AgingPdinfo.TrAvg = (U16)(busyTimeTotal / gNfInfo.totalLun);
                        smSysInfo->d.AgingData.d.AgingPdinfo.TrMax = (U16)busyTimeMax;
                    }
                }
                else if (type == 3)
                {
                    CoreDbg_Center(COREDBG_CMD_ERASE, 0, paa, 0, 0, 0);
                    Core_JobWait();
                    smSysInfo->d.AgingData.d.AgingNandBusyTime.Erase[die] = smNandBusyTime;
                    busyTimeTotal += smSysInfo->d.AgingData.d.AgingNandBusyTime.Erase[die];

                    if (busyTimeMax < smSysInfo->d.AgingData.d.AgingNandBusyTime.Erase[die])
                    {
                        busyTimeMax = smSysInfo->d.AgingData.d.AgingNandBusyTime.Erase[die];
                    }

                    if (die == (gNfInfo.totalLun - 1))
                    {
                        smSysInfo->d.AgingData.d.AgingPdinfo.TeAvg = (U16)(busyTimeTotal / gNfInfo.totalLun);
                        smSysInfo->d.AgingData.d.AgingPdinfo.TeMax = (U16)busyTimeMax;
                    }
                }
            }
        }
        SyIn_Synchronize(SI_AREA_MISC | SI_AREA_PLIST2 | SI_AREA_AGING | SI_AREA_MPINFO, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);

        D_PRINTF("\n");
        for (die = 0 ; die < gNfInfo.totalLun; die++)
        {
            D_PRINTF("DIE|%2d = E|%4d R|%2d P|%4d\n", die, smSysInfo->d.AgingData.d.AgingNandBusyTime.Erase[die], smSysInfo->d.AgingData.d.AgingNandBusyTime.Read[die], smSysInfo->d.AgingData.d.AgingNandBusyTime.Program[die]);
        }
        D_PRINTF("AVG_E|%4d MAX_E|%4d AVG_P|%4d MAX_P|%4d AVG_R|%2d MAX_R|%2d\n",
                    smSysInfo->d.AgingData.d.AgingPdinfo.TeAvg, smSysInfo->d.AgingData.d.AgingPdinfo.TeMax,
                    smSysInfo->d.AgingData.d.AgingPdinfo.TpAvg, smSysInfo->d.AgingData.d.AgingPdinfo.TpMax,
                    smSysInfo->d.AgingData.d.AgingPdinfo.TrAvg, smSysInfo->d.AgingData.d.AgingPdinfo.TrMax);
    }
}
#endif

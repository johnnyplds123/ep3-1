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

#include "RegApb.h"

#include "Fio.h"
#include "Cache.h"
#include "CoreDbg.h"
#include "SysInfo.h"
#include "Heap.h"
#include "Core.h"
#include "Thermal.h"

#include "Aging.h"
#include "Aging_MLC.h"

#include "FastSection.h"

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function proto-type definitions:
//-----------------------------------------------------------------------------
static tERROR_AGING Aging_MLC_Erase(void);
static tERROR_AGING Aging_MLC_Write(void);
static tERROR_AGING Aging_MLC_Read(void);
static void Aging_MLC_ProcErase(U16 block, U8 ce, U8 lun);
static void Aging_MLC_ProcWrite(U16 block, U8 ce, U8 lun, U16 page);
static void Aging_MLC_ProcRead(U16 block, U8 ce, U8 lun, U16 page);
static void Aging_MLC_ProcECC(void);
static void Aging_MLC_RandomBuffer(U8 *pBuf, U32 totalSector);
//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
U8 gAg_MLC_FlashID[3][6] = {{0x98, 0x3A, 0xA4, 0x93, 0x7A, 0xD1},
                            {0x98, 0x3C, 0xA5, 0x93, 0x7E, 0xD1},
                            {0x45, 0x3A, 0xA5, 0x93, 0x7E, 0x51}};
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
// return       : N/A
//-------------------------------------------------------------------
tERROR_AGING Aging_Operator(U16 loop)
{
    if (Aging_MLC_Erase() != ERR_NO_ERROR)  return gAgingInfo.ErrorCode;
    if (Aging_MLC_Write() != ERR_NO_ERROR)  return gAgingInfo.ErrorCode;
    if (Aging_MLC_Read() != ERR_NO_ERROR)  return gAgingInfo.ErrorCode;
    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_MLC_Erase(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static tERROR_AGING Aging_MLC_Erase(void)
{
    U16 block, maxSOC, maxI2C, jobCnt = 0;
    U8  lun, ce;

    maxSOC = maxI2C = 0;

    gAgingInfo.Tick = HalTimer_ReadTick();

    AGING_MLC_START_UPDATE_STATE(AGING_START_BLK, STATE_ERASE_FAIL);

    for (block = AGING_START_BLK; block < gNfInfo.blocksPerPlane; block++)
    {
        for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
        {
            for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
            {
                switch (Aging_CheckIssueSBlk(block, ce, lun))
                {
                    case STATUS_ISSUE_SBLK:
                        if  (block % 100 == 0) D_PRINTF("[AG E] SUPERBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                        Aging_MLC_ProcErase(block, ce, lun);
                        break;
                    case STATUS_ISSUE_NON_SBLK:
                        D_PRINTF("[AG E] NonSUBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                        Aging_MLC_ProcErase(block, ce, lun);
                        break;
                    default:
                        D_PRINTF("[AG E] NonIssue PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                        break;
                }
                AGING_ERASE_MLC_SPEED_DOWN(jobCnt);
            }
        }
        AGING_TEMPERATURE(maxSOC, maxI2C);
    }
    AGING_MLC_END_UPDATE_STATE(gNfInfo.blocksPerPlane, STATE_ERASE_PASS);

    gAgingInfo.TotalLoopErase = gAgingInfo.CurrLoopCnt;
    gAgingInfo.TotalTime += HalTimer_GetElapsedTimeMsec(gAgingInfo.Tick);
    AGING_SAVELOOPTIME(gAgingInfo.ReRunCnt, gAgingInfo.CurrLoopCnt, HalTimer_GetElapsedTimeMsec(gAgingInfo.Tick));

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_MLC_Write(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static tERROR_AGING Aging_MLC_Write(void)
{
    U16 block, page, maxSOC, maxI2C, jobCnt = 0;
    U8  lun, ce;

    maxSOC = maxI2C = 0;

    gAgingInfo.Tick = HalTimer_ReadTick();

    AGING_MLC_START_UPDATE_STATE(AGING_START_BLK, STATE_WRITE_FAIL);

    for (block = AGING_START_BLK; block < gNfInfo.blocksPerPlane; block++)
    {
        Aging_MLC_RandomBuffer(gCoreDbgBuf.wBuffer, 256);
        for (page = 0; page < gNfInfo.pagesPerBlock; page++)
        {
            for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
            {
                for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
                {
                    switch (Aging_CheckIssueSBlk(block, ce, lun))
                    {
                        case STATUS_ISSUE_SBLK:
                            if  ((block % 20 == 0) && page == 0) D_PRINTF("[AG W] SUPERBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                            Aging_MLC_ProcWrite(block, ce, lun, page);
                            break;
                        case STATUS_ISSUE_NON_SBLK:
                            if  (page == 0) D_PRINTF("[AG W] NonSUBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                            Aging_MLC_ProcWrite(block, ce, lun, page);
                            break;
                        default:
                            if  (page == 0) D_PRINTF("[AG W] NonIssue PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                            break;
                    }
                    AGING_WRITE_MLC_SPEED_DOWN(jobCnt);
                }
            }
        }
        AGING_TEMPERATURE(maxSOC, maxI2C);
    }
    AGING_MLC_END_UPDATE_STATE(gNfInfo.blocksPerPlane, STATE_WRITE_PASS);

    gAgingInfo.TotalLoopWrite = gAgingInfo.CurrLoopCnt;
    gAgingInfo.TotalTime += HalTimer_GetElapsedTimeMsec(gAgingInfo.Tick);
    AGING_SAVELOOPTIME(gAgingInfo.ReRunCnt, gAgingInfo.CurrLoopCnt, HalTimer_GetElapsedTimeMsec(gAgingInfo.Tick));

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_MLC_Read(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static tERROR_AGING Aging_MLC_Read(void)
{
    U16 block, page, maxSOC, maxI2C, jobCnt = 0;
    U8  lun, ce;

    maxSOC = maxI2C = 0;

    gAgingInfo.Tick = HalTimer_ReadTick();

    AGING_MLC_START_UPDATE_STATE(AGING_START_BLK, STATE_READ_FAIL);

    for (block = AGING_START_BLK; block < gNfInfo.blocksPerPlane; block++)
    {
        for (page = 0; page < gNfInfo.pagesPerBlock; page++)
        {
            for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
            {
                for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
                {
                    switch (Aging_CheckIssueSBlk(block, ce, lun))
                    {
                        case STATUS_ISSUE_SBLK:
                            if  ((block % 20 == 0) && page == 0) D_PRINTF("[AG R] SUPERBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce, lun, block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                            Aging_MLC_ProcRead(block, ce, lun, page);
                            break;
                        case STATUS_ISSUE_NON_SBLK:
                            if  (page == 0) D_PRINTF("[AG R] NonSUBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce,lun,block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                            Aging_MLC_ProcRead(block, ce, lun, page);
                            break;
                        default:
                            if  (page == 0) D_PRINTF("[AG R] NonIssue PAA[CE%d|LN|%d|BK0x%3x] SOC|%d I2C|%d\n", ce,lun,block, HalTempSensor_GetTemperature(), AplThermal_GetI2cTemperature());
                            break;
                    }
                    AGING_READ_MLC_SPEED_DOWN(jobCnt);
                }
            }
        }
        AGING_TEMPERATURE(maxSOC, maxI2C);
    }
    AGING_MLC_END_UPDATE_STATE(gNfInfo.blocksPerPlane, STATE_READ_PASS);

    gAgingInfo.TotalLoopRead = gAgingInfo.CurrLoopCnt;
    gAgingInfo.TotalTime += HalTimer_GetElapsedTimeMsec(gAgingInfo.Tick);
    AGING_SAVELOOPTIME(gAgingInfo.ReRunCnt, gAgingInfo.CurrLoopCnt, HalTimer_GetElapsedTimeMsec(gAgingInfo.Tick));

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static void Aging_MLC_ProcErase(U16 block, U8 ce, U8 lun)
// Description  :
// Input        : block, ce, lun
// return       : N/A
//-------------------------------------------------------------------
static void Aging_MLC_ProcErase(U16 block, U8 ce, U8 lun)
{
    tFIO_JOB *pJob[2];
    U32 idx;
    U16 die, rmpblk, cnt1, cnt2;
    U8  ch, plane;

    cnt1 = cnt2 = 0;

    for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
    {
        for (ch = 0; ch < gNfInfo.numChannels; ch++)
        {
            die    = AGING_GET_DIE(lun, ce ,ch);
            idx    = AGING_GET_SBLK_IDX(plane, block, die);
            rmpblk = gwAgRemapTbl[idx];

            // --- MultiPlane Erase ------------------------------------------------------------------------
            if  ((gbAgflagTbl[die] == 0) && (rmpblk != AGING_BAD_BLOCK))
            {
                gAgingFastPaa1Buf[cnt1].all32   = 0;
                gAgingFastPaa1Buf[cnt1].b.block = rmpblk;
                gAgingFastPaa1Buf[cnt1].b.ce    = ce;
                gAgingFastPaa1Buf[cnt1].b.lun   = lun;
                gAgingFastPaa1Buf[cnt1].b.ch    = ch;
                gAgingFastPaa1Buf[cnt1].b.plane = plane;
                cnt1++;
            }
            // --- SinglePlane Erase ------------------------------------------------------------------------
            else if (rmpblk != AGING_BAD_BLOCK)
            {
                gAgingFastPaa2Buf[cnt2].all32   = 0;
                gAgingFastPaa2Buf[cnt2].b.block = rmpblk;
                gAgingFastPaa2Buf[cnt2].b.ce    = ce;
                gAgingFastPaa2Buf[cnt2].b.lun   = lun;
                gAgingFastPaa2Buf[cnt2].b.ch    = ch;
                gAgingFastPaa2Buf[cnt2].b.plane = plane;
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
        MEM_COPY(pJob[0]->paa, gAgingFastPaa1Buf, sizeof(tPAA) * cnt1);

        Aging_JobIssue(pJob[0]);
    }
    // --- Issue SinglePlane Erase -------------------------------------------------------------------------------------------------------------
    if (cnt2)
    {
        pJob[1]         = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob[1]->plane  = FIO_CMD_1PLANE;
        pJob[1]->cmd    = FIO_JOB_CMD_ERASE;
        pJob[1]->paaCnt = cnt2;
        pJob[1]->css    = CACHE_AUTO;
        pJob[1]->flag   = FIO_F_NO_MKERR;
        MEM_COPY(pJob[1]->paa, gAgingFastPaa2Buf, sizeof(tPAA) * cnt2);

        Aging_JobIssue(pJob[1]);
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_MLC_ProcWrite(U16 block, U8 ce, U8 lun, U16 page)
// Description  :
// Input        : block, ce, lun, page
// return       : N/A
//-------------------------------------------------------------------
static void Aging_MLC_ProcWrite(U16 block, U8 ce, U8 lun, U16 page)
{
    tFIO_JOB *pJob[2];
    U32 idx;
    U16 die, rmpblk, cnt1, cnt2;
    U8  ch, plane, frag;

    cnt1 = cnt2 = 0;

    for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
    {
        for (ch = 0; ch < gNfInfo.numChannels; ch++)
        {
            for (frag = 0; frag < gEcuInfo.auPerPage; frag++)
            {
                die    = AGING_GET_DIE(lun, ce ,ch);
                idx    = AGING_GET_SBLK_IDX(plane, block, die);
                rmpblk = gwAgRemapTbl[idx];

                // --- MultiPlane Write ------------------------------------------------------------------------
                if ((gbAgflagTbl[die] == 0) && (rmpblk != AGING_BAD_BLOCK))
                {
                    gAgingFastPaa1Buf[cnt1].all32   = 0;
                    gAgingFastPaa1Buf[cnt1].b.block = rmpblk;
                    gAgingFastPaa1Buf[cnt1].b.ce    = ce;
                    gAgingFastPaa1Buf[cnt1].b.lun   = lun;
                    gAgingFastPaa1Buf[cnt1].b.ch    = ch;
                    gAgingFastPaa1Buf[cnt1].b.plane = plane;
                    gAgingFastPaa1Buf[cnt1].b.page  = page;
                    gAgingFastPaa1Buf[cnt1].b.frag  = frag;
                    cnt1++;
                }
                // --- SinglePlane Write ------------------------------------------------------------------------
                else if (rmpblk != AGING_BAD_BLOCK)
                {
                    gAgingFastPaa2Buf[cnt2].all32   = 0;
                    gAgingFastPaa2Buf[cnt2].b.block = rmpblk;
                    gAgingFastPaa2Buf[cnt2].b.ce    = ce;
                    gAgingFastPaa2Buf[cnt2].b.lun   = lun;
                    gAgingFastPaa2Buf[cnt2].b.ch    = ch;
                    gAgingFastPaa2Buf[cnt2].b.plane = plane;
                    gAgingFastPaa2Buf[cnt2].b.page  = page;
                    gAgingFastPaa2Buf[cnt2].b.frag  = frag;
                    cnt2++;
                }
                else break;
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
        pJob[0]->paaCnt    = cnt1;
        pJob[0]->flag      = FIO_F_NO_MKERR;

        MEM_COPY(pJob[0]->paa, gAgingFastPaa1Buf, sizeof(tPAA) * cnt1);

        Aging_JobIssue(pJob[0]);
    }
    // --- Issue SinglePlane Write -------------------------------------------------------------------------
    if (cnt2)
    {
        pJob[1]            = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob[1]->plane     = FIO_CMD_1PLANE;
        pJob[1]->pBuffer   = gCoreDbgBuf.wBuffer;
        pJob[1]->cmd       = FIO_JOB_CMD_WRITE;
        pJob[1]->paaCnt    = cnt2;
        pJob[1]->flag      = FIO_F_NO_MKERR;

        MEM_COPY(pJob[1]->paa, gAgingFastPaa2Buf, sizeof(tPAA) * cnt2);

        Aging_JobIssue(pJob[1]);
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_MLC_ProcRead(U16 block, U8 ce, U8 lun, U16 page)
// Description  :
// Input        : block, ce, lun, page
// return       : N/A
//-------------------------------------------------------------------
static void Aging_MLC_ProcRead(U16 block, U8 ce, U8 lun, U16 page)
{
    tFIO_JOB *pJob[2];
    U32 idx;
    U16 die, rmpblk, cnt1, cnt2;
    U8  ch, plane, frag;

    cnt1 = cnt2 = 0;

    for (plane = 0; plane < gNfInfo.planesPerLun; plane++, cnt1 = 0, cnt2 = 0)
    {
        for (ch = 0; ch < gNfInfo.numChannels; ch++)
        {
            for (frag = 0; frag < gEcuInfo.auPerPage; frag++)
            {
                die    = AGING_GET_DIE(lun, ce ,ch);
                idx    = AGING_GET_SBLK_IDX(plane, block, die);
                rmpblk = gwAgRemapTbl[idx];

                // --- MultiPlane Read ------------------------------------------------------------------------
                if  ((gbAgflagTbl[die] == 0) && (rmpblk != AGING_BAD_BLOCK))
                {
                    gAgingFastPaa1Buf[cnt1].all32   = 0;
                    gAgingFastPaa1Buf[cnt1].b.block = rmpblk;
                    gAgingFastPaa1Buf[cnt1].b.ce    = ce;
                    gAgingFastPaa1Buf[cnt1].b.lun   = lun;
                    gAgingFastPaa1Buf[cnt1].b.ch    = ch;
                    gAgingFastPaa1Buf[cnt1].b.plane = plane;
                    gAgingFastPaa1Buf[cnt1].b.page  = page;
                    gAgingFastPaa1Buf[cnt1].b.frag  = frag;
                    cnt1++;
                }
                // --- SinglePlane Read ------------------------------------------------------------------------
                else if (rmpblk != AGING_BAD_BLOCK)
                {
                    gAgingFastPaa2Buf[cnt2].all32        = 0;
                    gAgingFastPaa2Buf[cnt2].b.block      = rmpblk;
                    gAgingFastPaa2Buf[cnt2].b.ce         = ce;
                    gAgingFastPaa2Buf[cnt2].b.lun        = lun;
                    gAgingFastPaa2Buf[cnt2].b.ch         = ch;
                    gAgingFastPaa2Buf[cnt2].b.plane      = plane;
                    gAgingFastPaa2Buf[cnt2].b.frag       = frag;
                    gAgingFastPaa2Buf[cnt2].b.page       = page;
                    cnt2++;
                }
                else break;
            }
        }
        // --- Issue MultiPlane Reaad -------------------------------------------------------------------------
        if (cnt1)
        {
            pJob[0]            = FIO_PopJob(FIO_JOB_POOL_FREE);
            pJob[0]->plane     = gNfInfo.planesPerLun;
            pJob[0]->pBuffer   = gCoreDbgBuf.rBuffer;
            pJob[0]->cmd       = FIO_JOB_CMD_READ;
            pJob[0]->paaCnt    = cnt1;
            pJob[0]->flag      = FIO_F_NO_MKERR | FIO_F_NO_RR;

            MEM_COPY(pJob[0]->paa, gAgingFastPaa1Buf, sizeof(tPAA) * cnt1);

            Aging_JobIssue(pJob[0]);
        }
        // --- Issue SinglePlane Reaad -------------------------------------------------------------------------
        if (cnt2)
        {
            pJob[1]            = FIO_PopJob(FIO_JOB_POOL_FREE);
            pJob[1]->plane     = FIO_CMD_1PLANE;
            pJob[1]->pBuffer   = gCoreDbgBuf.rBuffer;
            pJob[1]->cmd       = FIO_JOB_CMD_READ;
            pJob[1]->paaCnt    = cnt2;
            pJob[1]->flag      = FIO_F_NO_MKERR | FIO_F_NO_RR;

            MEM_COPY(pJob[1]->paa, gAgingFastPaa2Buf, sizeof(tPAA) * cnt2);

            Aging_JobIssue(pJob[1]);
        }
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_MLC_ProcECC(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_MLC_ProcECC(void)
{
    tPAA paa;
    U16  cnt, pageUpper, pageLower, pageTempU = 0, pageTempL = 0, type = 0;

    for (cnt = smSysInfo->d.AgingEI.d.ErrEccStart; cnt < smSysInfo->d.AgingEI.d.ErrPaaCnt; cnt++)
    {
        type        = 0;
        paa.all32   = 0;
        paa.b.ch    = smSysInfo->d.AgingEI.d.Data[cnt].Ch;
        paa.b.ce    = smSysInfo->d.AgingEI.d.Data[cnt].Ce;
        paa.b.lun   = smSysInfo->d.AgingEI.d.Data[cnt].Lun;
        paa.b.page  = smSysInfo->d.AgingEI.d.Data[cnt].Page;
        paa.b.plane = (smSysInfo->d.AgingEI.d.Data[cnt].Block) % gNfInfo.planesPerLun;
        paa.b.block = (smSysInfo->d.AgingEI.d.Data[cnt].Block) / gNfInfo.planesPerLun;

        D_PRINTF("[ECC ] PAA[CH%d|CE%d|LN%d|PN%d|BK0x%3x|PBK0x%4x|PG|0x%3x]\n", paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, smSysInfo->d.AgingEI.d.Data[cnt].Block, paa.b.page);

        Aging_Nand_EccScan(paa, type, cnt);

        if ((smSysInfo->d.AgingEI.d.Data[cnt].Type == PROGRAM) && (paa.b.page != 0) && (paa.b.page != 2))
        {
            type = PROG_WL_LEAKAGE;
            if (((paa.b.page % 2) == 0) && (paa.b.page != 4))  //Case1----> WL2~WL126 Upper Page and != 4
            {
                pageUpper = paa.b.page - 2;    //Upper
                pageLower = paa.b.page - 5;    //Lower
            }
            else if (paa.b.page == 1)                          //Case2----> WL1   Lower Page
            {
                pageUpper = paa.b.page + 1;    //Upper
                pageLower = paa.b.page - 1;    //Lower
            }
            else if (paa.b.page == 4)                          //Case3----> WL1   Upper Page
            {
                pageUpper = paa.b.page - 4;    //Upper
                pageLower = paa.b.page - 2;    //Lower
            }
            else if (paa.b.page == 255)                        //Case4----> WL127 Upper Page
            {
                pageUpper = paa.b.page - 4;    //Upper
                pageLower = paa.b.page - 1;    //Lower
            }
            else                                               //Case5----> W2~WL127 Upper Page
            {
                pageUpper = paa.b.page + 1;    //Upper
                pageLower = paa.b.page - 2;    //Lower
            }
            D_PRINTF("[ECC ] ORI|0x%3x PAGEU|0x%3x PAGEL|0x%3x \n", paa.b.page, pageUpper, pageLower);
            paa.b.page  = pageUpper;
            pageTempU  = Aging_Nand_EccScan(paa, type, cnt);

            paa.b.page  = pageLower;
            pageTempL  = Aging_Nand_EccScan(paa, type, cnt);

            if ((pageTempU == 1) && (pageTempL == 1))
            {
                smSysInfo->d.AgingData.d.AgingPdinfo.LastFail_WLLK = smSysInfo->d.AgingData.d.AgingLoop.LoopCurrent;
                smSysInfo->d.AgingEI.d.Data[cnt].Type          = PROG_WL_LEAKAGE;
            }
        }
    }
    smSysInfo->d.AgingEI.d.ErrEccStart = smSysInfo->d.AgingEI.d.ErrPaaCnt;
}

//-------------------------------------------------------------------
// Function     : static void Aging_MLC_RandomBuffer(U8 *pBuf, U32 totalSector)
// Description  :
// Input        : pBuf, totalSector
// return       : N/A
//-------------------------------------------------------------------
static void Aging_MLC_RandomBuffer(U8 *pBuf, U32 totalSector)
{
    U32 sector, cnt;

    srand(gAgingInfo.Tick);

    for (sector = 0; sector < totalSector ; sector++)  // 2 * Track size
    {
        for (cnt = 0; cnt < 0x1000UL; cnt++)  //1Sectors=512bytes=0x200bytes, 0x1000=4kbytes
        {
             pBuf[sector*0x1000UL + cnt] = (U8) rand();
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
                        if  (block % 100 == 0) D_PRINTF("[AG E] SUPERBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d\n", ce, lun, block, HalTempSensor_GetTemperature());
                        Aging_MLC_ProcErase(block, ce, lun);
                        break;
                    case STATUS_ISSUE_NON_SBLK:
                        D_PRINTF("[AG E] NonSUBLK PAA[CE%d|LN|%d|BK0x%3x] SOC|%d\n", ce, lun, block, HalTempSensor_GetTemperature());
                        Aging_MLC_ProcErase(block, ce, lun);
                        break;
                    default:
                        D_PRINTF("[AG E] NonIssue PAA[CE%d|LN|%d|BK0x%3x] SOC|%d\n", ce, lun, block, HalTempSensor_GetTemperature());
                        break;
                }
                AGING_ERASE_MLC_SPEED_DOWN(jobCnt);
            }
        }
    }
    Aging_JobWait();
}

//-------------------------------------------------------------------
// Function     : tERROR_AGING Aging_Nand_FlashID(void)
// Description  : NandFlashId when AGING
// Input        :
// return       : AgingInfo.ErrorCode
//-------------------------------------------------------------------
tERROR_AGING Aging_Nand_FlashID(void)
{
    MsgPhyRwe_t     pHcmdMsg;
    tAGING_FLASHID  *FlashID;
    U8 ch, ce, error = 0;

    MEM_CLR(&pHcmdMsg, sizeof(MsgPhyRwe_t));
    pHcmdMsg.pBuffer=(uint8_t*)smSSD[SSD_IF_START].startAddr;
    FlashID    = (tAGING_FLASHID *)pHcmdMsg.pBuffer;
    MSG_SET_SERVICE_INIT(pHcmdMsg, cSfnNfimReadId);
    Ipc_SendWaitMsg(cC2mOtherQueue, (Msg_t *)&pHcmdMsg, (Msg_t *)&pHcmdMsg, IPC_NO_TIMEOUT);

    for (ch = 0; ch < gNfInfo.numChannels; ch++)
    {
        for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
        {
            MEM_COPY(&smSysInfo->d.AgingData.d.AgingFlashID.FlashID[ce][ch][0], &FlashID->ID[0], 8);
            if  (gNfInfo.lunsPerDevice == 0x1)
                error = MEM_CMP(&gAg_MLC_FlashID[0][0],&FlashID->ID[0],6);
            else
                error = MEM_CMP(&gAg_MLC_FlashID[1][0],&FlashID->ID[0],6);

            if  (error != 0)
            {
                gAgingInfo.ErrorCode = ERR_FLASH_ID;
                smSysInfo->d.AgingData.d.AgingFlashID.FlashIDErrorCode = ERR_FLASH_ID;
                smSysInfo->d.AgingData.d.AgingFlashID.FlashIDErrorCnt++;
                smSysInfo->d.AgingData.d.AgingFlashID.FlashIDmap[ch] |= BIT(ce);
            }
            FlashID++;
        }
    }
    #if 0
    if((smSysInfo->d.AgingData.d.AgingFlashID.FlashIDErrorCnt) && (gAgingInfo.ErrorCode == ERR_NO_ERROR))
    {
        gAgingInfo.ErrorCode = ERR_FLASH_ID;
    }
    #else
    gAgingInfo.ErrorCode = ERR_NO_ERROR;
    #endif

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
    U32  busyTimeTemp, busyTimeTotal, busyTimeMax;
    U16  die, blk, page;
    U8   lun, ce, ch, plane, type;

    paa.all32 = 0;
    blk       = 0;
    plane     = 0;
    busyTimeTemp = busyTimeMax = busyTimeTotal = 0;

    smSysInfo->d.AgingData.d.AgingNandBusyTime.NBTimeTag = AGING_NBTI;
    smSysInfo->d.Header.d.SubHeader.d.Aging.d.dwValidTag = SI_TAG_AGING;
    smSysInfo->d.Header.d.SubHeader.d.MPInfo.d.dwValidTag= SI_TAG_MPINFO;

    Aging_JobWait();

    for (type = 0; type < 4; type++, busyTimeMax = 0, busyTimeTotal = 0)
    {
        for (die = 0; die < gNfInfo.totalLun; die++, busyTimeTemp = 0)
        {
            ch  = die & AGING_CH_MASK;
            ce  = (die >> gNfInfo.bitsCh) & AGING_CE_MASK;
            lun = (die >> gNfInfo.bitsChDev) & AGING_LUN_MASK;
            paa.all32    = 0;
            paa.b.block  = blk;
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
                for (page = 0; page < gNfInfo.pagesPerBlock; page++)  //WL
                {
                    paa.b.page = page;
                    CoreDbg_Center(COREDBG_CMD_WRITE, 0, paa, 0, 0, 0);
                    Core_JobWait();
                    busyTimeTemp += smNandBusyTime;
                }

                smSysInfo->d.AgingData.d.AgingNandBusyTime.Program[die] = (busyTimeTemp / gNfInfo.pagesPerBlock);

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
    for (die = 0 ; die < gNfInfo.totalLun; die++)
    {
        D_PRINTF("DIE|%2d = E|%4d R|%2d P|%4d\n", die, smSysInfo->d.AgingData.d.AgingNandBusyTime.Erase[die], smSysInfo->d.AgingData.d.AgingNandBusyTime.Read[die], smSysInfo->d.AgingData.d.AgingNandBusyTime.Program[die]);
    }
    D_PRINTF("AVG_E|%4d MAX_E|%4d AVG_P|%4d MAX_P|%4d AVG_R|%2d MAX_R|%2d\n",
                smSysInfo->d.AgingData.d.AgingPdinfo.TeAvg, smSysInfo->d.AgingData.d.AgingPdinfo.TeMax,
                smSysInfo->d.AgingData.d.AgingPdinfo.TpAvg, smSysInfo->d.AgingData.d.AgingPdinfo.TpMax,
                smSysInfo->d.AgingData.d.AgingPdinfo.TrAvg, smSysInfo->d.AgingData.d.AgingPdinfo.TrMax);

}
#endif

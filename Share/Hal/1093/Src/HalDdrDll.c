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
//! @brief ddr dll tuning & sweeping
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "RegDdrMc.h"

#include "MemTest.h"
#include "HalMc5DdrInit.h"
#include "HalMc5DdrTables.h"

#if ((CHK_CODE_TYPE(CODE_MAIN)))
#include "SysInfo.h"
#endif
#include "FastSection.h"
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define DLL_VALID_TAG           0x4C445244 // DRDL
#define DLL_OVERLIMIT_TAG       0x4D4C564F // OVLM

#define DLL_PHSEL_MAX_BIT       6
#define DLL_PHSEL_COUNT         BIT(DLL_PHSEL_MAX_BIT)
#define DLL_PHSEL_MAX           (DLL_PHSEL_COUNT - 1)

#define DLL_WINDOW_SIZE_SPAN    2
#define DLL_WINDOW_SIZE_MIN     (DLL_WINDOW_SIZE_SPAN * 2 + 1) // 2(lower) + 1 + 2(upper)

#define DLL_PATTERN_COUNT        112
#define DLL_PATTERN_RANGE        4
#define DLL_OVERLIMIT_THRESHOLD  (DLL_TEST_TIMES / 10)
#define DLL_TEST_TIMES           30
#define DLL_TUNE_BOUNDARY        10

#define DLL_DELAY_SWITCH_ON      TRUE
#define DLL_DELAY_SWITCH_OFF     FALSE

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define DLL_BYTE_TO_MASK(b)     (BYTE_MASK << (b * BYTE_SHIFT))

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
enum{
    COMPARE_OK      = 0,
    COMPARE_ERROR   = 1
};

enum{
    EDGE_BOTH       = 0,
    EDGE_NEGATIVE   = 1,
    EDGE_POSITIVE   = 2
};

enum{
    BOUNDARY_LOWER  = 0,
    BOUNDARY_UPPER  = 1,
    BOUNDARY_COUNT  = 2
};

const U32 gTunePattern[DLL_PATTERN_COUNT] =
{
    0x00000000, 0xffffffff, 0x00000000, 0xffffffff,
    0x00000000, 0xffffffff, 0x00000000, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
    0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, // 6

    0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa,
    0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa,
    0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555,
    0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555,
    0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa,
    0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, // 6

    0xffff0000, 0x5555aaaa, 0xaaaa5555, 0x0000ffff,
    0x5555aaaa, 0xaaaa5555, 0x0000ffff, 0xffff0000, // 2

    0xf0f0f0f0, 0x0f0f0f0f, 0xf0f0f0f0, 0x0f0f0f0f,
    0x0f0f0f0f, 0xf0f0f0f0, 0x0f0f0f0f, 0xf0f0f0f0,
    0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000,
    0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff,
    0x0000000f, 0x000000f0, 0x00000f00, 0x0000f000,
    0x000f0000, 0x00f00000, 0x0f000000, 0xf0000000, // 6

    0x00000001, 0x00000002, 0x00000004, 0x00000008,
    0x00000010, 0x00000020, 0x00000040, 0x00000080,
    0x00000100, 0x00000200, 0x00000400, 0x00000800,
    0x00001000, 0x00002000, 0x00004000, 0x00008000,
    0x00010000, 0x00020000, 0x00040000, 0x00080000,
    0x00100000, 0x00200000, 0x00400000, 0x00800000,
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000  // 8
};

#if CHK_FW_TYPE(FW_AGING)
U8  gDllWindow[DLL_PHSEL_COUNT][BOUNDARY_COUNT];
U8  gDllBitMap[DLL_PHSEL_COUNT >> 3];
#endif

U8  gDllPositive[MC5_MAX_SUPPORT_BYTE];
U8  gDllNegative[MC5_MAX_SUPPORT_BYTE];


//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
U32     DdrDll_DataVerify(U32 startAddr, U32 cycles, U32 mask);
void    DdrDll_SetRegDll(U8 dllPositive, U8 dllNegative, U8 byteIndex, bool delaySwitch);
void    DdrDll_GetRegDll(U8 byteIndex);

void    DdrDll_StoreDll(U8 dllPositive, U8 dllNegative, U8 byteIndex);
U8      DdrDll_SweepRefine(U8 yIndex);
void    DdrDll_Sweep(U8 byteIndex, U8 store, U8 update);
void    DdrDll_Routine(U8 cycle);

U8      DdrDll_DiagonalScan(U8 byteIndex);
U8      DdrDll_CentralScan(U32 center, U32 edge, U8 byteIndex);
void    DdrDll_Tune(U8 byteIndex);

//-----------------------------------------------------------------------------
//  Code
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    DdrDll_SetRegDll
**/
//-----------------------------------------------------------------------------
void DdrDll_SetRegDll(U8 dllPositive, U8 dllNegative, U8 byteIndex, bool delaySwitch)
{
    U32 delayL;

    rDdrMc.chan0PhyDllCtrlB[byteIndex].b.DLL_PHSEL_Bx  = dllPositive;
    rDdrMc.chan0PhyDllCtrlB[byteIndex].b.DLL_PHSEL1_Bx = dllNegative;

    /* reset PHY DLL. Dll_reset_timer
       ([31:24] of PHY Control Register 8, Offset 0x41C/0xC1C) is set to 0x10,
       reset DLL for 128 * 32 = 4096 MCLK cycles.
    // updates DLL master. Block read / MMR for 4096 MCLK cycles
       to guarantee DLL lock.
       Either wait 4096 MCLK (memPll / 4) cycles, or check DLL lock status*/

    // DLL reset
    rDdrMc.chan0PhyCtrl9.all = 0x20000000;
    //rDdrMc.chan0PhyCtrl9.b.PHY_DLL_RST = 1;

    if (delaySwitch == TRUE) for (delayL = 100000; delayL > 0; delayL--);
    /* Update Dll delayL. When Dll_update_timer
       ([20:16] of PHY Control Register 8, Offset 0x41C/0xC1C) is 0x11,
       assert DLL_UPDATE_EN pin for 0x11 * 16 = 272 MCLK cycles.
    // copy DLL master to DLL slave. Slave controls the actual delayL.
       Both DLL bypass and DLL needs 'update'.*/

    // Update DLL
    rDdrMc.chan0PhyCtrl9.all = 0x40000000;
    //rDdrMc.chan0PhyCtrl9.b.DLL_UPD_EN_PULSE = 1;

    if (delaySwitch == TRUE) for (delayL = 5000; delayL > 0; delayL--);
    // Finish DLL update
    rDdrMc.chan0PhyCtrl9.all = 0x00000000;
}

//-----------------------------------------------------------------------------
/**
    DdrDll_GetRegDll
**/
//-----------------------------------------------------------------------------
void DdrDll_GetRegDll(U8 byteIndex)
{
    gDllPositive[byteIndex] = rDdrMc.chan0PhyDllCtrlB[byteIndex].b.DLL_PHSEL_Bx;
    gDllNegative[byteIndex] = rDdrMc.chan0PhyDllCtrlB[byteIndex].b.DLL_PHSEL1_Bx;
}

#if (CHK_FW_TYPE(FW_AGING) && (CHK_CODE_TYPE(CODE_MAIN)))
//-----------------------------------------------------------------------------
/**
    DdrDll_StoreDll
**/
//-----------------------------------------------------------------------------
void DdrDll_StoreDll(U8 dllPositive, U8 dllNegative, U8 byteIndex)
{
    smHwInfo.d.dramDll.tag                    = DLL_VALID_TAG;
    smHwInfo.d.dramDll.dllPositive[byteIndex] = dllPositive;
    smHwInfo.d.dramDll.dllNegative[byteIndex] = dllNegative;

    // Cal CheckSum
    smHwInfo.d.dramDll.checkSum = MemSumU8((U8*)&smHwInfo.d.dramDll.dllPositive[0], 8);
    D_PRINTF("-[DLL] checkSum|%4X\n", smHwInfo.d.dramDll.checkSum);

    HalSflash_MiscHwInfo_Flush();
}

//-----------------------------------------------------------------------------
/**
    DdrDll_SweepRefine
**/
//-----------------------------------------------------------------------------
U8 DdrDll_SweepRefine(U8 yIndex)
{
    U8  xCenter;
    U8  i;

    // check center
    xCenter = (gDllWindow[yIndex][BOUNDARY_LOWER] + gDllWindow[yIndex][BOUNDARY_UPPER]) / 2;
    if (gDllBitMap[xCenter / BITS_PER_BYTE] & BIT(xCenter % BITS_PER_BYTE))
    {
        return FALSE;
    }
    else
    {
        // check isolated hole
        // lower
        for (i = xCenter - 1; i > 0; i--)
        {
            if (gDllBitMap[i / BITS_PER_BYTE] & BIT(i % BITS_PER_BYTE))
            {
                gDllWindow[yIndex][BOUNDARY_LOWER] = i + 1;
                break;
            }
        }

        // upper
        for (i = xCenter + 1; i < DLL_PHSEL_COUNT; i++)
        {
            if (gDllBitMap[i / BITS_PER_BYTE] & BIT(i % BITS_PER_BYTE))
            {
                gDllWindow[yIndex][BOUNDARY_UPPER] = i - 1;
                break;
            }
        }

        // check center span
        if ((gDllWindow[yIndex][BOUNDARY_UPPER] - xCenter) < DLL_WINDOW_SIZE_SPAN) return FALSE;
        if ((xCenter - gDllWindow[yIndex][BOUNDARY_LOWER]) < DLL_WINDOW_SIZE_SPAN) return FALSE;

        // check hole size
        if ((gDllWindow[yIndex][BOUNDARY_UPPER] - gDllWindow[yIndex][BOUNDARY_LOWER] + 1) < DLL_WINDOW_SIZE_MIN)
        {
            return FALSE;
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/**
    DdrDll_Sweep
**/
//-----------------------------------------------------------------------------
void DdrDll_Sweep(U8 byteIndex, U8 store, U8 update)
{
    HwTimerTick_t tick;
    U8  yStart, yEnd;
    U8  x, y;

    tick  = HalTimer_ReadTick();

    // reset result table
    for (x = 0; x < DLL_PHSEL_COUNT; x++)
    {
        *((U16*)&gDllWindow[x][0]) = DLL_PHSEL_MAX; // 0x003F
    }

    yStart = DLL_PHSEL_MAX;
    yEnd   = 0x00;

    for (y = 0; y < DLL_PHSEL_COUNT; y++)
    {
        D_PRINTF("[N|%2d] ", y);

        MEM_CLR(&gDllBitMap[0], BIT2BYTE(DLL_PHSEL_COUNT));

        for (x = 0; x < DLL_PHSEL_COUNT; x++)
        {
            DdrDll_SetRegDll(x, y, byteIndex, DLL_DELAY_SWITCH_ON);

            //if (MemTestRandU32((U32 *)DIRECT_DRAM_ADDR, 8 * KBYTE, x + y, NULL, 0) == 0)
            //if (MemTestRandU32NoMsg((U32 *)DIRECT_DRAM_ADDR, 8 * KBYTE, x + y) == 0)
            if ((DdrDll_DataVerify(DIRECT_DRAM_ADDR, 5, DLL_BYTE_TO_MASK(byteIndex)) == COMPARE_OK) &&
                (MemTestRandMaskNoMsg((U32 *)DIRECT_DRAM_ADDR, 8 * KBYTE, x + y, DLL_BYTE_TO_MASK(byteIndex)) == COMPARE_OK))
            {
                if (x < gDllWindow[y][BOUNDARY_LOWER]) gDllWindow[y][BOUNDARY_LOWER] = x;
                if (x > gDllWindow[y][BOUNDARY_UPPER]) gDllWindow[y][BOUNDARY_UPPER] = x;
            }
            else
            {
                gDllBitMap[x / BITS_PER_BYTE] |= BIT(x % BITS_PER_BYTE);
            }
        }

        if (DdrDll_SweepRefine(y) == TRUE)
        {
            if (y < yStart) yStart = y;
            if (y > yEnd)   yEnd   = y;
        }
        else
        {
            // reset
            gDllWindow[y][BOUNDARY_LOWER] = DLL_PHSEL_MAX;
            gDllWindow[y][BOUNDARY_UPPER] = 0x00;
        }

        // window
        for (x = 0; x < DLL_PHSEL_COUNT; x++)
        {
            if (gDllBitMap[x / BITS_PER_BYTE] & BIT(x % BITS_PER_BYTE)) D_PRINTF("X");
            else                                                        D_PRINTF(" ");
        }

        // result
        if (gDllWindow[y][BOUNDARY_LOWER] != DLL_PHSEL_MAX)
        {
            D_PRINTF(" [P|%2d-%2d]\n", gDllWindow[y][BOUNDARY_LOWER],
                                       gDllWindow[y][BOUNDARY_UPPER]);
        }
        else
        {
            D_PRINTF("\n");
        }
    }

    //--------------------------
    // algorithm average
    y = (yEnd + yStart) / 2;
    x = (gDllWindow[y][BOUNDARY_LOWER] + gDllWindow[y][BOUNDARY_UPPER]) / 2;

    if (store)
    {
        DdrDll_StoreDll(x, y, byteIndex);
    }

    D_PRINTF("-[DLL] Sweep B%d|P%2X|N%2X %dms\n", byteIndex, x, y, HalTimer_GetElapsedTimeMsec(tick));

    if (update)
    {
        DdrDll_SetRegDll(x, y, byteIndex, DLL_DELAY_SWITCH_ON);
    }
    else
    {
        DdrDll_SetRegDll(gDllPositive[byteIndex], gDllNegative[byteIndex], byteIndex, DLL_DELAY_SWITCH_ON);
    }
}

//-----------------------------------------------------------------------------
/**
    DdrDll_Routine
**/
//-----------------------------------------------------------------------------
void DdrDll_Routine(U8 cycle)
{
    #if CHK_FW_TYPE(FW_AGING)
    U32  dllPositive[MC5_MAX_SUPPORT_BYTE],     dllNegative[MC5_MAX_SUPPORT_BYTE];
    U8   dllPositive_Max[MC5_MAX_SUPPORT_BYTE], dllNegative_Max[MC5_MAX_SUPPORT_BYTE];
    U8   dllPositive_Min[MC5_MAX_SUPPORT_BYTE], dllNegative_Min[MC5_MAX_SUPPORT_BYTE];
    U8   dllPositive_Over[MC5_MAX_SUPPORT_BYTE], dllNegative_Over[MC5_MAX_SUPPORT_BYTE];
    U8   loop, byteIndex, byteOverLimitFlag, totalOverLimitCnt;

    MEM_CLR((PVOID)(&dllPositive), (sizeof(U32) * MC5_MAX_SUPPORT_BYTE));
    MEM_CLR((PVOID)(&dllNegative), (sizeof(U32) * MC5_MAX_SUPPORT_BYTE));
    MEM_CLR((PVOID)(&dllPositive_Over), (sizeof(U8) * MC5_MAX_SUPPORT_BYTE));
    MEM_CLR((PVOID)(&dllNegative_Over), (sizeof(U8) * MC5_MAX_SUPPORT_BYTE));
    MEM_CLR((PVOID)(&dllPositive_Max),  (sizeof(U8) * MC5_MAX_SUPPORT_BYTE));
    MEM_CLR((PVOID)(&dllNegative_Max),  (sizeof(U8) * MC5_MAX_SUPPORT_BYTE));
    MEM_SET((PVOID)(&dllPositive_Min),  BYTE_MASK, (sizeof(U8) * MC5_MAX_SUPPORT_BYTE));
    MEM_SET((PVOID)(&dllNegative_Min),  BYTE_MASK, (sizeof(U8) * MC5_MAX_SUPPORT_BYTE));

    //TUNE//
    totalOverLimitCnt = 0;
    byteOverLimitFlag = 0;
    for (loop = 0; loop < cycle; loop++)
    {
        for (byteIndex = 0; byteIndex < MC5_MAX_SUPPORT_BYTE; byteIndex++)
        {
            DdrDll_Sweep(byteIndex, FALSE, TRUE);
            DdrDll_GetRegDll(byteIndex);
            if (gDllPositive[byteIndex] > dllPositive_Max[byteIndex]) dllPositive_Max[byteIndex] = gDllPositive[byteIndex];
            if (gDllNegative[byteIndex] > dllNegative_Max[byteIndex]) dllNegative_Max[byteIndex] = gDllNegative[byteIndex];
            if (gDllPositive[byteIndex] < dllPositive_Min[byteIndex]) dllPositive_Min[byteIndex] = gDllPositive[byteIndex];
            if (gDllNegative[byteIndex] < dllNegative_Min[byteIndex]) dllNegative_Min[byteIndex] = gDllNegative[byteIndex];

            if ((gDllNegative[byteIndex] <= (mc5DllDataDefault.dllNegative[byteIndex] + DLL_PATTERN_RANGE)) &&
                (gDllNegative[byteIndex] >= (mc5DllDataDefault.dllNegative[byteIndex] - DLL_PATTERN_RANGE)) &&
                (gDllPositive[byteIndex] <= (mc5DllDataDefault.dllPositive[byteIndex] + DLL_PATTERN_RANGE)) &&
                (gDllPositive[byteIndex] >= (mc5DllDataDefault.dllPositive[byteIndex] - DLL_PATTERN_RANGE))
                )
            {
                dllPositive[byteIndex] += gDllPositive[byteIndex];
                dllNegative[byteIndex] += gDllNegative[byteIndex];
            }
            else
            {
                dllPositive_Over[byteIndex] = gDllPositive[byteIndex];
                dllNegative_Over[byteIndex] = gDllNegative[byteIndex];
                byteOverLimitFlag          |= BIT(byteIndex);
            }
        }
        if (byteOverLimitFlag)
        {
            totalOverLimitCnt++;
            byteOverLimitFlag = 0;
        }
    }

    for (byteIndex = 0; byteIndex < MC5_MAX_SUPPORT_BYTE; byteIndex++)
    {
        if (totalOverLimitCnt > DLL_OVERLIMIT_THRESHOLD)
        {
            smHwInfo.d.dramDll.tag = DLL_OVERLIMIT_TAG;
            if ((byteOverLimitFlag & BIT(byteIndex)))
            {
                smHwInfo.d.dramDll.dllPositive[byteIndex] = dllPositive_Over[byteIndex];
                smHwInfo.d.dramDll.dllNegative[byteIndex] = dllNegative_Over[byteIndex];
            }
            else
            {
                smHwInfo.d.dramDll.dllPositive[byteIndex] = gDllPositive[byteIndex];
                smHwInfo.d.dramDll.dllNegative[byteIndex] = gDllNegative[byteIndex];

            }
            smHwInfo.d.dramDll.checkSum = MemSumU8((U8*)&smHwInfo.d.dramDll.dllPositive[0], 8);
            DdrDll_SetRegDll(mc5DllDataDefault.dllPositive[byteIndex],
                             mc5DllDataDefault.dllNegative[byteIndex],
                             byteIndex, DLL_DELAY_SWITCH_ON);
        }
        else
        {
            smHwInfo.d.dramDll.tag                    = DLL_VALID_TAG;
            smHwInfo.d.dramDll.dllPositive[byteIndex] = (dllPositive[byteIndex] / (cycle - totalOverLimitCnt));
            smHwInfo.d.dramDll.dllNegative[byteIndex] = (dllNegative[byteIndex] / (cycle - totalOverLimitCnt));
            smHwInfo.d.dramDll.checkSum               = MemSumU8((U8*)&smHwInfo.d.dramDll.dllPositive[0], 8);
            DdrDll_SetRegDll(smHwInfo.d.dramDll.dllPositive[byteIndex],
                             smHwInfo.d.dramDll.dllNegative[byteIndex],
                             byteIndex, DLL_DELAY_SWITCH_ON);
        }

        if (byteIndex == 0) smSysInfo->d.AgingData.d.AgingDram.DramDLLFailCnt = totalOverLimitCnt;

        smSysInfo->d.AgingData.d.AgingDram.DramDLLMax.DramDllPositive[byteIndex] = dllPositive_Max[byteIndex];
        smSysInfo->d.AgingData.d.AgingDram.DramDLLMax.DramDllNegative[byteIndex] = dllNegative_Max[byteIndex];
        smSysInfo->d.AgingData.d.AgingDram.DramDLLMin.DramDllPositive[byteIndex] = dllPositive_Min[byteIndex];
        smSysInfo->d.AgingData.d.AgingDram.DramDLLMin.DramDllNegative[byteIndex] = dllNegative_Min[byteIndex];
    }
    #endif
}
#endif

//-----------------------------------------------------------------------------
/**
    DdrDll_DataVerify
**/
//-----------------------------------------------------------------------------
U32 DdrDll_DataVerify(U32 startAddr, U32 cycles, U32 mask)
{
    U32 cycle;
    volatile U32 *addr;

    // Write
    addr = (volatile U32*)startAddr;
    for (cycle = 0; cycle < cycles; cycle++)
    {
        MEM_COPY((PVOID)addr, &gTunePattern[0], sizeof(gTunePattern));
        addr += DLL_PATTERN_COUNT;
    }

    // Read & Compare
    addr = (volatile U32*)startAddr;
    for (cycle = 0; cycle < cycles; cycle++)
    {
        if (mask)
        {
            U32 loop;
            for (loop = 0; loop < DLL_PATTERN_COUNT; loop++)
            {
                if ((gTunePattern[loop] & mask) != (*addr & mask))
                {
                    return COMPARE_ERROR;
                }
                addr++;
            }
        }
        else
        {
            if (MEM_CMP((PVOID)addr, &gTunePattern[0], sizeof(gTunePattern)))
            {
                return COMPARE_ERROR;
            }
            addr += DLL_PATTERN_COUNT;
        }
    }

    return COMPARE_OK;
}

//-----------------------------------------------------------------------------
/**
    DdrDll_DiagonalScan

    quick finding central dll value by diagonal scanning
**/
//-----------------------------------------------------------------------------
U8 DdrDll_DiagonalScan(U8 byteIndex)
{
    U16 idx;
    U16 lower = 0xFF, upper = 0xFF;

    for (idx = 0; idx < DLL_PHSEL_COUNT; idx++)
    {
        DdrDll_SetRegDll(idx, idx, byteIndex, DLL_DELAY_SWITCH_OFF);

        if (DdrDll_DataVerify(DIRECT_DRAM_ADDR, 1, DLL_BYTE_TO_MASK(byteIndex)) == COMPARE_OK)
        {
            if (lower == 0xFF)
            {
                lower = idx;
            }
            upper = idx;
        }
    }

    return (lower + upper) / 2;
}

//-----------------------------------------------------------------------------
/**
    Scan and find best dll by specificed edge

    @param[in]  center         dll center value
    @param[in]  edge           positive or negative edge

    @return     optimized dll value for specificed edge
**/
//-----------------------------------------------------------------------------
U8 DdrDll_CentralScan(U32 center, U32 edge, U8 byteIndex)
{
    U16 lower = center;
    U16 upper = center;
    S16 value;

    value = center;
    while (value >= (center + DLL_TUNE_BOUNDARY))
    {
        if (edge == EDGE_NEGATIVE)  DdrDll_SetRegDll(center, value, byteIndex, DLL_DELAY_SWITCH_OFF);
        else                        DdrDll_SetRegDll(value, center, byteIndex, DLL_DELAY_SWITCH_OFF);
        if (DdrDll_DataVerify(DIRECT_DRAM_ADDR, 1, DLL_BYTE_TO_MASK(byteIndex)) == COMPARE_ERROR)
        {
            break;
        }

        lower = value;
        value--;
    }

    value = center + 1;
    while (value < (center + DLL_TUNE_BOUNDARY))
    {
        if (edge == EDGE_NEGATIVE)  DdrDll_SetRegDll(center, value, byteIndex, DLL_DELAY_SWITCH_OFF);
        else                        DdrDll_SetRegDll(value, center, byteIndex, DLL_DELAY_SWITCH_OFF);

        if (DdrDll_DataVerify(DIRECT_DRAM_ADDR, 1, DLL_BYTE_TO_MASK(byteIndex)) == COMPARE_ERROR)
        {
            break;
        }

        upper = value;
        value++;
    }
    return (lower + upper) / 2;
}

//-----------------------------------------------------------------------------
/**
    Quick tuning dll value

    @return     optimal dll
**/
//-----------------------------------------------------------------------------
void DdrDll_Tune(U8 byteIndex)
{
    U8  dllCenter, dllNegative, dllPositive;

    dllCenter = DdrDll_DiagonalScan(byteIndex);

    if (dllCenter != 0xFF)
    {
        dllPositive = DdrDll_CentralScan(dllCenter, EDGE_POSITIVE, byteIndex);
        dllNegative = DdrDll_CentralScan(dllCenter, EDGE_NEGATIVE, byteIndex);
    }
    else
    {
        dllPositive =  gDllPositive[byteIndex];
        dllNegative =  gDllNegative[byteIndex];
    }
    DdrDll_SetRegDll(dllPositive, dllNegative, byteIndex, DLL_DELAY_SWITCH_OFF);

    D_PRINTF("-[DLL] Tune B%d|P%2X|N%2X\n", byteIndex, dllPositive, dllNegative);
}

//-----------------------------------------------------------------------------
/**
    HalDdrDll_Sweep
**/
//-----------------------------------------------------------------------------
#if (CHK_CODE_TYPE(CODE_MAIN))
void HalDdrDll_Sweep(U8 byteIndex, U8 store, U8 update)
{
    #if CHK_FW_TYPE(FW_AGING)
    if (byteIndex < MC5_MAX_SUPPORT_BYTE)
    {
        DdrDll_GetRegDll(byteIndex);
        DdrDll_Sweep(byteIndex, store, update);
    }
    else
    {
        for (byteIndex = 0; byteIndex < MC5_MAX_SUPPORT_BYTE; byteIndex++)
        {
            DdrDll_GetRegDll(byteIndex);
            DdrDll_Sweep(byteIndex, store, update);
        }
    }
    #endif
}
#endif

//-----------------------------------------------------------------------------
/**
    HalDdrDll_Tune
**/
//-----------------------------------------------------------------------------
void HalDdrDll_Tune(U8 byteIndex)
{
    if (byteIndex < MC5_MAX_SUPPORT_BYTE)
    {
        DdrDll_GetRegDll(byteIndex);
        DdrDll_Tune(byteIndex);
    }
    else
    {
        for (byteIndex = 0; byteIndex < MC5_MAX_SUPPORT_BYTE; byteIndex++)
        {
            DdrDll_GetRegDll(byteIndex);
            DdrDll_Tune(byteIndex);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    HalDdrDll_Routine
**/
//-----------------------------------------------------------------------------
#if ((CHK_CODE_TYPE(CODE_MAIN)) && CHK_FW_TYPE(FW_AGING))
void HalDdrDll_Routine(void)
{
    DdrDll_Routine(DLL_TEST_TIMES);
}
#endif

//-----------------------------------------------------------------------------
/**
    HalDdrDll_DumpInfo
**/
//-----------------------------------------------------------------------------
void HalDdrDll_DumpInfo(void)
{
    U8 i;

    D_PRINTF("-[DLL] DumpInfo\n");

    for (i = 0; i < MC5_MAX_SUPPORT_BYTE; i++)
    {
        DdrDll_GetRegDll(i);
        D_PRINTF("-[DLL] B%d|P%2X|N%2X\n", i, gDllPositive[i], gDllNegative[i]);
    }
}

//-----------------------------------------------------------------------------
/**
    HalDdrDll_VerifyHwInfo
**/
//-----------------------------------------------------------------------------
bool HalDdrDll_VerifyHwInfo(void)
{
    U32 checkSum = 0;

    checkSum = MemSumU8((U8*)&smHwInfo.d.dramDll.dllPositive[0], 8);

    if (smHwInfo.d.dramDll.tag == DLL_VALID_TAG)
    {
        if (smHwInfo.d.dramDll.checkSum == checkSum) return TRUE;
    }

    return FALSE;
}

/* End of File */

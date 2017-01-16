// ============================================================================
//
//     Copyright (c) Marvell Corporation 2015  -  All rights reserved
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
//! @file HalMc5DdrInit.c
//!
//! @brief Initialization code for MC5 DDR controller
//
//=============================================================================

/******************************************************************************
*                                                                             *
* Dependencies                                                                *
*                                                                             *
******************************************************************************/
#include "BuildPkg.h"
#include "SharePkg.h"

#include "RegClockPLL.h"
#include "RegDdrMc.h"
#include "HalClock.h"
#include "HalTimer.h"
#include "HalDram.h"
#include "HalCpu.h"

#include "MemIo.h"
#include "Debug.h"

#include "HalMc5DdrInit.h"
#include "HalMc5DdrConfig.h"
#include "HalMc5DdrTables.h"

//-----------------------------------------------------------------------------
//  Private data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: private or public
//-----------------------------------------------------------------------------
//#pragma arm section rwdata = "data_fast_ui"
//#pragma arm section zidata = "data_fast_ui"

static Mc5GlobalData_t mc5GlobalData;
static Mc5HwInfoData_t mc5HwInfoData;
static DramType_t      gDramType;

static Mc5RegForRecoveryOdt_t   mc5RegForRecoveryOdt;
static Mc5RegForRecoverySize_t  mc5RegForRecoverySize;
static Mc5RegForRecoveryJedec_t mc5RegForRecoveryJedec;

#include "FastSection.h"

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void Mc5_SetPllA             (U32 frequencyMT);
static void Mc5_PhyDllResetAndUpdate(void);
static void Mc5_PhyInitSequenceSync2(volatile U8 ldPhase, volatile U8 wrstSel,
                                     volatile U8 wckgDly, volatile U8 wckEn);
static bool Mc5_PadCalVerifyHwInfo  (void);
static void Mc5_PadCalibration      (bool isLpddr, bool override);
static void Mc5_SetRegPadCal        (bool isLpddr, U8 LsbVal, U8 MsbVal);
static void Mc5_GetRegPadCal        (void);
static void Mc5_StoreRegPadCal      (void);
static void Mc5_SetRegReadLeveling  (void);
static void Mc5_SetRegWriteLeveling (void);
static void Mc5_SetRegMemAddrMap    (void);
static void Mc5_SetRegJedecParams   (U32 dramFrequencyMHz, bool updateDensityOnly);
static void Mc5_SetRegOdt           (bool isRecovery);
static void Mc5_SetRegPhyCtrl       (U32 dramFrequencyMHz);

static void Mc5_StoreRestoreRegMemAddr(Mc5StoreRestoreReg_t operation);
static void Mc5_StoreRestoreRegJedec  (Mc5StoreRestoreReg_t operation);
static void Mc5_StoreRestoreRegOdt    (Mc5StoreRestoreReg_t operation);
static U8   Mc5_GetMemIndexBySize     (U32 size);
static U8   Mc5_GetOdtValue           (Mc5OdtOperation_t operation, U8 part);
static bool Mc5_Cs1IsEnabled          (void);

#if (MC5_AUTODETECT_MEMORY_SIZE == MC5_ENABLED)
static void     Mc5_MemSizeDetect(bool isLpddr);
static U32      Mc5_GetSingleDeviceDensity(bool isLpddr, U32 totalMemSize);
static void     Mc5_SetBlckAddrBoundary(U32 totalMemSize, U32 singleDeviceDensity);
static Error_t  Mc5_TestMemSizeConfig(U8 index);
#endif

static void Mc5_InitDdrSequence(bool isLpddr, bool isRecovery);
static void Mc5_InitDdrPhy  (U32 dramFrequencyMHz, bool isLpddr, bool isRecovery);
static void Mc5_InitDdrBasic(U32 dramFrequencyMHz, bool isLpddr, bool isRecovery);
static void Mc5_InitGlobalData(U32 frequencyMT, bool isLpddr, bool initDone);
static void Mc5_InitHwInfoData(void);
static void Mc5_AutoDetectRoutine(bool isLpddr);
static void Mc5_OneTimeInit(U32 frequencyMT, bool isLpddr);
static void Mc5_Recovery   (U32 frequencyMT, bool isLpddr);
static void Mc5_DumpInfo(void);
static void Mc5_DumpRegs(void);
static void Mc5_UpdateDramInfo(void);
static void Mc5_Lpddr3MrRead(void);

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern void HalDdrDll_Sweep(U8 byte, U8 store, U8 update);
extern void HalDdrDll_Tune(U8 byte);
extern bool HalDdrDll_VerifyHwInfo(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Mc5_SetPllA - Set PLLA value
**/
//-----------------------------------------------------------------------------
static void Mc5_SetPllA(U32 frequencyMT)
{
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)

    U32 i;

    // Turn on bypass mode
    rClockPll.pllaCtrl1.b.PLLA_BYPASS_EN = 1;

    // Assert PLL Reset
    rClockPll.pllaCtrl3.b.PLLA_RST = 1;

    for (i = 0; i < MC5_NUM_OF_PLLA_CTRL1_VALUES; i++)
    {
        if (mc5PllACtrl1Values[i].frequency == frequencyMT)
        {
            rClockPll.pllaCtrl1.all = mc5PllACtrl1Values[i].value;
            break;
        }
    }

    // If unknown clock - set default PLLA
    if (i == MC5_NUM_OF_PLLA_CTRL1_VALUES)
    {
        rClockPll.pllaCtrl1.all = mc5PllACtrl1Values[MC5_NUM_OF_PLLA_CTRL1_DEFAULT].value;
    }

    rClockPll.pllaCtrl3.all = 0x04040080;

    rClockPll.pllaCtrl2.all = 0x80000120;

    // De-assert PLL Reset
    rClockPll.pllaCtrl3.b.PLLA_RST = 0;

    // Wait 50 usec
    HalTimer_DelayUsec(50);

    // Turn off bypass mode
    rClockPll.pllaCtrl1.b.PLLA_BYPASS_EN = 0;

    // Wait 50 usec
    HalTimer_DelayUsec(50);

    #endif
}

//-----------------------------------------------------------------------------
/**
    Mc5_PhyDllResetAndUpdate - Reset and update DDR Phy DLL
**/
//-----------------------------------------------------------------------------
static void Mc5_PhyDllResetAndUpdate(void)
{
    U32 delayCnt;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    // ddr phy init sequence
    Mc5_PhyInitSequenceSync2(1, 3, 2, 0);

    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
    rDdrMc.chan0PhyCtrl6.all            = MC5_SET_TO_ZERO;
    rDdrMc.chan0PhyCtrl15.all           = MC5_SET_TO_ZERO;
    rDdrMc.chan0PhyCtrl9.all            = MC5_SET_TO_ZERO;
    // PHY Synchronize Enable
    rDdrMc.chan0PhyCtrl9.b.PHY_SYNC_EN  = MC5_ENABLED;

    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
    // ddr phy init sequence
    Mc5_PhyInitSequenceSync2(1, 3, 2, 0);
    #endif

    // DLL reset
    rDdrMc.chan0PhyCtrl9.b.PHY_DLL_RST = 1;
    // Wait for dll lock
    for (delayCnt = 0; delayCnt < 10; delayCnt++);

    // DLL update
    rDdrMc.chan0PhyCtrl9.b.DLL_UPD_EN_PULSE = 1;
    // Wait for dll update
    for (delayCnt = 0; delayCnt < 10; delayCnt++);
}

//-----------------------------------------------------------------------------
/**
    Mc5_PhyInitSequenceSync2 - Sync sequence 2 for DDR phy, used for LPDDR3
**/
//-----------------------------------------------------------------------------
static void Mc5_PhyInitSequenceSync2(volatile U8 ldPhase, volatile U8 wrstSel,
                                     volatile U8 wckgDly, volatile U8 wckEn)
{
    // PHY Control Register 6
    rDdrMc.chan0PhyCtrl6.all = MC5_SET_TO_ZERO;
    rDdrMc.chan0PhyCtrl6.b.MC_PHY_WFIFO_LD_PHASE = ldPhase;
    rDdrMc.chan0PhyCtrl6.b.MC_PHY_SYNC_EN_SEL    = 1;

    // PHY Control Register 15
    rDdrMc.chan0PhyCtrl15.all = MC5_SET_TO_ZERO;
    rDdrMc.chan0PhyCtrl15.b.PHY_WRST_SEL = wrstSel;

    // PHY Control Register 16
    rDdrMc.chan0PhyCtrl16.all = MC5_SET_TO_ZERO;
    rDdrMc.chan0PhyCtrl16.b.PHY_WCKG_DLY = wckgDly;
    rDdrMc.chan0PhyCtrl16.b.PHY_WCK_EN   = wckEn;

    // PHY Control Register 9 Enable sync
    rDdrMc.chan0PhyCtrl9.all = MC5_SET_TO_ZERO;
    rDdrMc.chan0PhyCtrl9.b.PHY_SYNC_EN = 1;
}

//-----------------------------------------------------------------------------
/**
    Mc5_PadCalVerifyHwInfo
**/
//-----------------------------------------------------------------------------
static bool Mc5_PadCalVerifyHwInfo(void)
{
    U32 checkSum = 0;

    checkSum = MemSumU8((U8*)&smHwInfo.d.dramCal.phyCalLsb, 2);

    if (smHwInfo.d.dramCal.tag == PAD_CAL_TAG_VAILD)
    {
        if (smHwInfo.d.dramCal.checkSum == checkSum)
        {
            mc5HwInfoData.phyCalData.phyCalLsb = smHwInfo.d.dramCal.phyCalLsb;
            mc5HwInfoData.phyCalData.phyCalMsb = smHwInfo.d.dramCal.phyCalMsb;
            return TRUE;
        }
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
/**
    Mc5_PadCalibration - main Pad calibration routine.
    The purpose of this function is to calibrate the PAD driver and termination
    PMOS, NMOS minidrivers to external calibration resistors.
    This step can be bypassed and set manual fixed
    values determined from chip characterization.
**/
//-----------------------------------------------------------------------------
static void Mc5_PadCalibration(bool isLpddr, bool override)
{
    U32 chan0PhyCtrl5;
    U32 waitTime;

    D_PRINTF("-[MC5] PadCal|R%d|V%d\n", override, mc5HwInfoData.dataValid[cMc5HwInfoCal]);

    if (override || (mc5HwInfoData.dataValid[cMc5HwInfoCal] == FALSE))
    {
        waitTime = 0;

        // Store chan0PhyCtrl5 value
        chan0PhyCtrl5 = rDdrMc.chan0PhyCtrl5.all;

        // Setup Pad Cal parameters:
        // Set Pad Cal interval (64cycles),
        // PAD_CAL_AUTO_SEL = 0x0 and
        // PAD_CAL_AUTO     = 0x0
        rDdrMc.chan0PhyCtrl5.all = 0x30011F;

        // Run Pad calibration once.
        // Run the h/w based pad calibration and check the DONE.
        rDdrMc.chan0PhyCtrl5.b.PAD_CAL_ONCE = 0x1;
        HalTimer_DelayUsec(20);

        // Wait until calibration finished
        while ((rDdrMc.chan0PhyCtrl9.b.PHY_CAL_DONE != 1) &&
               (rDdrMc.chan0PhyCtrl9.b.PHY_CAL_ERR  != 1))
        {
            HalTimer_DelayUsec(1);
            waitTime++;
            if (waitTime > PAD_CAL_MAX_WAITTIME) break;
        }

        Mc5_GetRegPadCal();

        // Restore chan0PhyCtrl5 value
        rDdrMc.chan0PhyCtrl5.all = chan0PhyCtrl5;
    }

    D_PRINTF("-[MC5] PadCal|L%2X|M%2X\n", mc5HwInfoData.phyCalData.phyCalLsb,
                                          mc5HwInfoData.phyCalData.phyCalMsb);

    Mc5_SetRegPadCal(isLpddr, 0, 0);
}

//-----------------------------------------------------------------------------
/**
    Mc5_GetRegPadCal
**/
//-----------------------------------------------------------------------------
static void Mc5_GetRegPadCal(void)
{
    U16 phyCalZnr;
    U16 phyCalZpr;
    U16 phyCalLsb;
    U16 phyCalMsb;
    U16 phyCal;
    U8  phyCalValid;

    phyCalValid = TRUE;

    // Check timeout & error status - if it's an error don't change anything
    if ((rDdrMc.chan0PhyCtrl9.b.PHY_CAL_DONE != 1) ||
        (rDdrMc.chan0PhyCtrl9.b.PHY_CAL_ERR  == 1) )
    {
        // If this is an error hardcode midpoint value (0x10)
        phyCal      = PAD_CAL_MID_VALUE;
        phyCalValid = FALSE;
    }
    else
    {
        phyCalZnr = rDdrMc.chan0PhyCtrl9.b.PHY_CAL_ZNR;
        phyCalZpr = rDdrMc.chan0PhyCtrl9.b.PHY_CAL_ZPR;

        // (CAL_ZPR + CAL_ZNR) * 0.5 + OFFSET
        phyCal = (phyCalZpr + phyCalZnr) / 2 * MC5_PAD_CAL_RESISTOR_DIVISOR + PAD_CAL_DEFAULT_OFFSET;

        if ((phyCalZpr == PAD_CAL_MAX_VALUE) || (phyCalZnr == PAD_CAL_MAX_VALUE))
        {
            // If calibration result is == 0x1F, ignore the results and hardcode midpoint value + 4 (0x14)
            phyCal      = PAD_CAL_MID_VALUE + 4;
            phyCalValid = FALSE;
        }
        else if ((phyCalZpr < 0x3) || (phyCalZnr < 0x3))
        {
            // If calibration result is less than 3, ignore the results and hardcode midpoint value (0x10)
            phyCal      = PAD_CAL_MID_VALUE;
            phyCalValid = FALSE;
        }
    }

    phyCalLsb = 0xF &  phyCal;
    phyCalMsb = 0x1 & (phyCal >> 4);

    // Store Pad Calibration Result
    mc5HwInfoData.phyCalData.phyCalLsb   = phyCalLsb;
    mc5HwInfoData.phyCalData.phyCalMsb   = phyCalMsb;
    mc5HwInfoData.phyCalData.phyCalValid = phyCalValid;
}

//-----------------------------------------------------------------------------
/**
    Mc5_SetRegPadCal - restore result of Pad Calibration
**/
//-----------------------------------------------------------------------------
static void Mc5_SetRegPadCal(bool isLpddr, U8 LsbVal, U8 MsbVal)
{
    // reduced argument
    LsbVal = mc5HwInfoData.phyCalData.phyCalLsb;
    MsbVal = mc5HwInfoData.phyCalData.phyCalMsb;

    // Copy calibration value to all io pads
    rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZNR       = LsbVal;
    rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZNR_MSB   = MsbVal;
    rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_ZNR     = LsbVal;
    rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_ZNR_MSB = MsbVal;
    rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_ZPR     = LsbVal;
    rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_ZPR_MSB = MsbVal;
    rDdrMc.chan0PhyCtrl4.b.PHY_CK_ZNR       = LsbVal;
    rDdrMc.chan0PhyCtrl4.b.PHY_CK_ZNR_MSB   = MsbVal;
    rDdrMc.chan0PhyCtrl4.b.PHY_CK_ZPR       = LsbVal;
    rDdrMc.chan0PhyCtrl4.b.PHY_CK_ZPR_MSB   = MsbVal;

    // For LPDDR if phyCal greater than midpoint set DQ ZPR to mid point value
    if (MsbVal && isLpddr)
    {
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZPR     = 0xF & PAD_CAL_MID_VALUE;
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZPR_MSB = 0x1 & (PAD_CAL_MID_VALUE >> 4);
    }
    else
    {
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZPR     = LsbVal;
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZPR_MSB = MsbVal;
    }
}

//-----------------------------------------------------------------------------
/**
    Mc5_StoreRegPadCal - Store result of Pad Calibration
**/
//-----------------------------------------------------------------------------
static void Mc5_StoreRegPadCal(void)
{
    mc5HwInfoData.phyCalData.phyCalLsb = rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZNR;
    mc5HwInfoData.phyCalData.phyCalMsb = rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZNR_MSB;
}

//-----------------------------------------------------------------------------
/**
    Mc5_SetRegReadLeveling
**/
//-----------------------------------------------------------------------------
static void Mc5_SetRegReadLeveling(void)
{
    #if (MC5_PHY_RD_LVL_DLY == MC5_ENABLED)
    Mc5PhyRLCtrl_t* pPhyRLCtrlCsTable[MC5_MAX_SUPPORT_CS];
    U8 i;

    // table assign
    pPhyRLCtrlCsTable[MC5_CS0] = (Mc5PhyRLCtrl_t*)phyRLCtrlCs0Table;
    #if (MC5_CS1_ENABLED == MC5_ENABLED)
    pPhyRLCtrlCsTable[MC5_CS1] = (Mc5PhyRLCtrl_t*)phyRLCtrlCs1Table;
    #else
    pPhyRLCtrlCsTable[MC5_CS1] = NULL;
    #endif

    // PHY Read Leveling for CS0
    for(i = 0; i < MC5_MAX_SUPPORT_BYTE; i++)
    {
        rDdrMc.chan0PhyRdLvlCtrlCs0B[i].all = MC5_SET_TO_ZERO;
        rDdrMc.chan0PhyRdLvlCtrlCs0B[i].b.PHY_RL_CYCLE_DLY_CS0_Bx = (pPhyRLCtrlCsTable[MC5_CS0][i].phyRLCycleDelay & 0x0F);
        rDdrMc.chan0PhyRdLvlCtrlCs0B[i].b.PHY_RL_TAP_DLY_CS0_Bx   = (pPhyRLCtrlCsTable[MC5_CS0][i].phyRLTapDelay   & 0xFF);
    }

    #if (MC5_CS1_ENABLED == MC5_ENABLED)
    // PHY Read Leveling for CS1
    for(i = 0; i < MC5_MAX_SUPPORT_BYTE; i++)
    {
        rDdrMc.chan0PhyRdLvlCtrlCs1B[i].all = MC5_SET_TO_ZERO;
        rDdrMc.chan0PhyRdLvlCtrlCs1B[i].b.PHY_RL_CYCLE_DLY_CS1_Bx = (pPhyRLCtrlCsTable[MC5_CS1][i].phyRLCycleDelay & 0x0F);
        rDdrMc.chan0PhyRdLvlCtrlCs1B[i].b.PHY_RL_TAP_DLY_CS1_Bx   = (pPhyRLCtrlCsTable[MC5_CS1][i].phyRLTapDelay   & 0xFF);
    }
    #endif

    #else
    U8 i;

    for(i = 0; i < MC5_MAX_SUPPORT_BYTE; i++)
    {
        rDdrMc.chan0PhyRdLvlCtrlCs0B[i].all = MC5_SET_TO_ZERO;
        rDdrMc.chan0PhyRdLvlCtrlCs1B[i].all = MC5_SET_TO_ZERO;
    }
    #endif
}

//-----------------------------------------------------------------------------
/**
    Mc5_SetRegWriteLeveling
**/
//-----------------------------------------------------------------------------
static void Mc5_SetRegWriteLeveling(void)
{
    #if (MC5_PHY_WR_LVL_DLY == MC5_ENABLED)
    Mc5PhyWLCtrl_t* pPhyWLCtrlCsTable[MC5_MAX_SUPPORT_CS];
    U8 i;

    // table assign
    pPhyWLCtrlCsTable[MC5_CS0] = (Mc5PhyWLCtrl_t*)phyWLCtrlCs0Table;
    #if (MC5_CS1_ENABLED == MC5_ENABLED)
    pPhyWLCtrlCsTable[MC5_CS1] = (Mc5PhyWLCtrl_t*)phyWLCtrlCs1Table;
    #else
    pPhyWLCtrlCsTable[MC5_CS1] = NULL;
    #endif

    // PHY Write Leveling for CS0
    for(i = 0; i < MC5_MAX_SUPPORT_BYTE; i++)
    {
        rDdrMc.chan0PhyWrLvlCtrlDataCs0[i].all = MC5_SET_TO_ZERO;
        rDdrMc.chan0PhyWrLvlCtrlDataCs0[i].b.PHY_WL_WCK_DQ_DLY_CS0_Bx  = (pPhyWLCtrlCsTable[MC5_CS0][i].phyWLWckDqDelay  & 0x7F);
        rDdrMc.chan0PhyWrLvlCtrlDataCs0[i].b.PHY_WL_WCK_DQC_DLY_CS0_Bx = (pPhyWLCtrlCsTable[MC5_CS0][i].phyWLWckDqcDelay & 0x7F);
        rDdrMc.chan0PhyWrLvlCtrlDataCs0[i].b.PHY_WL_WCK_QS_DLY_CS0_Bx  = (pPhyWLCtrlCsTable[MC5_CS0][i].phyWLWckQsDelay  & 0x7F);
        rDdrMc.chan0PhyWrLvlCtrlDataCs0[i].b.PHY_WL_WCK_QSC_DLY_CS0_Bx = (pPhyWLCtrlCsTable[MC5_CS0][i].phyWLWckQscDelay & 0x7F);
    }

    #if (MC5_CS1_ENABLED == MC5_ENABLED)
    // PHY Write Leveling for CS1
    for(i = 0; i < MC5_MAX_SUPPORT_BYTE; i++)
    {
        rDdrMc.chan0PhyWrLvlCtrlDataCs1[i].all = MC5_SET_TO_ZERO;
        rDdrMc.chan0PhyWrLvlCtrlDataCs1[i].b.PHY_WL_WCK_DQ_DLY_CS1_Bx  = (pPhyWLCtrlCsTable[MC5_CS1][i].phyWLWckDqDelay  & 0x7F);
        rDdrMc.chan0PhyWrLvlCtrlDataCs1[i].b.PHY_WL_WCK_DQC_DLY_CS1_Bx = (pPhyWLCtrlCsTable[MC5_CS1][i].phyWLWckDqcDelay & 0x7F);
        rDdrMc.chan0PhyWrLvlCtrlDataCs1[i].b.PHY_WL_WCK_QS_DLY_CS1_Bx  = (pPhyWLCtrlCsTable[MC5_CS1][i].phyWLWckQsDelay  & 0x7F);
        rDdrMc.chan0PhyWrLvlCtrlDataCs1[i].b.PHY_WL_WCK_QSC_DLY_CS1_Bx = (pPhyWLCtrlCsTable[MC5_CS1][i].phyWLWckQscDelay & 0x7F);
    }
    #endif

    #else
    U8 i;

    for(i = 0; i < MC5_MAX_SUPPORT_BYTE; i++)
    {
        rDdrMc.chan0PhyWrLvlCtrlDataCs0[i].all = MC5_SET_TO_ZERO;
        rDdrMc.chan0PhyWrLvlCtrlDataCs1[i].all = MC5_SET_TO_ZERO;
    }
    #endif
}

//-----------------------------------------------------------------------------
/**
    Mc5_SetRegMemAddrMap
**/
//-----------------------------------------------------------------------------
static void Mc5_SetRegMemAddrMap(void)
{
    Mc5MemSizeConfig_t* memSizeConfig;
    U32                 memSizeIndex;

    memSizeConfig = mc5GlobalData.memSizeConfig;

    // Set using TABLE 5 above -
    // KevinKCChiu: Use MC5_SIZE_MAX to config
    memSizeIndex = Mc5_GetMemIndexBySize(MC5_DDR_SIZE);

    rDdrMc.chan0MmapCs0.all = memSizeConfig[memSizeIndex].chan0MmapCs0Config;

    if (memSizeConfig[memSizeIndex].chan0MmapCs1Config == 0)
    {
        rDdrMc.chan0MmapCs1.all = MC5_CS1_DEFAULT_VALUE;
    }
    else
    {
        rDdrMc.chan0MmapCs1.all = memSizeConfig[memSizeIndex].chan0MmapCs1Config;
    }

    mc5GlobalData.cs1StartAddress = (memSizeConfig[memSizeIndex].chan0MmapCs1Config) & 0xF0000000;

    // The following settings will be overwritten by set_ba_boundary()
    // in case of MC5_DDR_SIZE == MC5_SIZE_AUTO
    rDdrMc.chan0MemControllPhysicalAddrMapCs0.all = MC5_CHAN0_MEM_CTRL_PHYS_ADDR_MAP;

    #if (MC5_CS1_ENABLED == MC5_ENABLED)
    rDdrMc.chan0MemControllPhysicalAddrMapCs1.all = MC5_CHAN0_MEM_CTRL_PHYS_ADDR_MAP;
    #else
    rDdrMc.chan0MemControllPhysicalAddrMapCs1.all = MC5_SET_TO_ZERO;
    #endif

    // If code get here, it's definitely NOT recovery
    // Store all registers for further use in recovery mode
    Mc5_StoreRestoreRegMemAddr(cStoreReg);
}

//-----------------------------------------------------------------------------
/**
    Mc5_SetRegJedecParams -
    Set JEDEC parameters - registers 0x300 and 0x390-0x3AC, according to
    JESD79-3F  for DDR3
    JESD209-3C for LPDDR3.
**/
//-----------------------------------------------------------------------------
static void Mc5_SetRegJedecParams(U32 dramFrequencyMHz, bool updateDensityOnly)
{
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
    Mc5MemoryDevice_t memoryDeviceDdr3;
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
    Mc5MemoryDeviceDdr4_t memoryDeviceDdr4;
    #endif

    Mc5SpeedBin_t speedBinJedec;
    U32           speedBinJedecIndex;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    U32 nDerating;
    U32 nCke;
    #endif

    // 0xF1000390
    U32 tRfc;
    U32 tRefi;

    U32 nRas;
    U32 nRcd;

    // 0xF10003A4
    U32 nRp;
    U32 nRpa;

    // 0xF10003A8
    U32 nRrd;

    // 0xF1000394
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    U32 tXsrd;
    #endif
    U32 nCksrx;
    U32 nCksre;

    // 0xF1000398
    U32 nCkesr;

    U32 nCwl;

    U8  i;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    // Temperature Derating
    nDerating = MC5_PS_TO_NCK(dramFrequencyMHz, MC5_TEMP_DERATING_VAL);
    // Store value of tCKE in nCK because it used few times
    nCke = MC5_GET_MAX_NCK_PS_TO_NCK(dramFrequencyMHz, mc5JedecParams.T_CKE_L);
    #endif

    if (!updateDensityOnly)
    {
        // Set ZQ Calibration timing parameters
        rDdrMc.chan0Zqc0.all = MC5_SET_TO_ZERO;
        rDdrMc.chan0Zqc1.all = MC5_SET_TO_ZERO;

        #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
            rDdrMc.chan0Zqc0.b.T_ZQINIT = MC5_PS_TO_NCK_WITH_COEF            (dramFrequencyMHz, mc5JedecParams.T_ZQINIT_L_ps);
            rDdrMc.chan0Zqc0.b.T_ZQCR   = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_ZQCR);
            rDdrMc.chan0Zqc1.b.T_ZQCS   = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_ZQCS_L_ps);
            rDdrMc.chan0Zqc1.b.T_ZQOPER = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_ZQOPER_L_ps);
        #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
            rDdrMc.chan0Zqc0.b.T_ZQINIT = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_ZQINIT);
            rDdrMc.chan0Zqc1.b.T_ZQCS   = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_ZQCS);
            rDdrMc.chan0Zqc1.b.T_ZQOPER = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_ZQOPER);
        #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
            rDdrMc.chan0Zqc0.b.T_ZQINIT = mc5JedecParams.T_ZQINIT_4_nck;
            rDdrMc.chan0Zqc1.b.T_ZQCS   = mc5JedecParams.T_ZQCS_4_nck;
            rDdrMc.chan0Zqc1.b.T_ZQOPER = mc5JedecParams.T_ZQOPER_4_nck;
        #endif
    }

    // tRefi
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        #if (MC5_LPDDR_PER_BANK_MODE == MC5_ENABLED)
        tRefi = MC5_DEFAULT_LPDDR_T_REFI_PB;
        #else
        // tREFI of LPDDR is the same as DDR3 if density >= 2Gb
        if (mc5GlobalData.deviceDensity < MC5_SIZE_256_MB) tRefi = MC5_DEFAULT_DDR_T_REFI;
        else                                               tRefi = MC5_DEFAULT_LPDDR_T_REFI;
        #endif
    #else // DDR3 & DDR4
        tRefi = MC5_DEFAULT_DDR_T_REFI;
    #endif

    // tRfc
    tRfc = 0;
    for (i = 0; i < MC5_NUM_OF_TRFC_VALUES; i++)
    {
        if (tRfcValues[i].deviceDensity == mc5GlobalData.deviceDensity)
        {
            #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
                #if (MC5_LPDDR_PER_BANK_MODE == MC5_ENABLED)
                tRfc = tRfcValues[i].trfcValLpddrPb_ps;
                #else
                tRfc = tRfcValues[i].trfcValLpddr_ps;
                #endif
            #else // DDR3 & DDR4
                tRfc = tRfcValues[i].trfcValDdr_ps;
            #endif
            break;
        }
    }

    // restore default
    if (i == MC5_NUM_OF_TRFC_VALUES)
    {
        #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
            #if (MC5_LPDDR_PER_BANK_MODE == MC5_ENABLED)
            tRfc = MC5_DEFAULT_TRFC_VAL_PB_LPDDR;
            #else
            tRfc = MC5_DEFAULT_TRFC_VAL_LPDDR;
            #endif
        #else // DDR3 & DDR4
            tRfc = MC5_DEFAULT_TRFC_VAL;
        #endif
    }

    rDdrMc.chan0RefreshTm.all = MC5_SET_TO_ZERO;

    /* tRefi uses FCLK, which can be a low-speed clock or the same as the
       DRAM clock, depending on the SoC implementation.
       This ensures consistent refresh intervals regardless of the DRAM clock. */
    rDdrMc.chan0RefreshTm.b.T_REFI = tRefi / MC5_MHZ_TO_PS(MC5_DDR_FCLK_DEFAULT);

    // This timing setting is calculated in DRAM clock cycles
    rDdrMc.chan0RefreshTm.b.T_RFC = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, tRfc);

    if (updateDensityOnly)
    {
        Mc5_StoreRestoreRegJedec(cStoreReg);
        return;
    }

    speedBinJedec.speedBin = mc5GlobalData.frequencyMT;
    speedBinJedecIndex = 0;

    #if (MC5_DDR_TYPE != MC5_DDR_TYPE_DDR4)
    for (i = 0; i < MC5_FREQ_NUM_OF_OPTIONS; i++)
    {
        if (mc5TypicalDevice[i].speedBin == speedBinJedec.speedBin)
        {
            speedBinJedec.clNrcdNrp = mc5TypicalDevice[i].clNrcdNrp;
            speedBinJedecIndex      = i;
            break;
        }
    }
    // restore default
    if (i == MC5_FREQ_NUM_OF_OPTIONS)
    {
        speedBinJedec.clNrcdNrp = MC5_SPEED_BIN_1600_N_CL;
        speedBinJedecIndex      = cIndex1600;
    }

    #else // DDR4
    for (i = 0; i < MC5_FREQ_NUM_OF_OPTIONS_DDR4; i++)
    {
        if (mc5TypicalDeviceDdr4[i].speedBin == speedBinJedec.speedBin)
        {
            speedBinJedec.clNrcdNrp = mc5TypicalDeviceDdr4[i].clNrcdNrp;
            speedBinJedecIndex      = i + cIndex1600;
            break;
        }
    }
    // restore default
    if (i == MC5_FREQ_NUM_OF_OPTIONS_DDR4)
    {
        speedBinJedec.clNrcdNrp = MC5_SPEED_BIN_1600_N_CL_DDR4;
        speedBinJedecIndex      = cIndex1600;
    }
    #endif

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
        // Find proper memory device
        for (i = 0; i < MC5_SPEED_BIN_OPTIONS; i++)
        {
            if ((mc5MemoryDevice[i].deviceData.speedBin  == speedBinJedec.speedBin) &&
                (mc5MemoryDevice[i].deviceData.clNrcdNrp == speedBinJedec.clNrcdNrp))
            {
                MEM_COPY(&memoryDeviceDdr3, (void*)&(mc5MemoryDevice[i]), sizeof(Mc5MemoryDevice_t));
                break;
            }
        }

        // restore default
        if (i == MC5_SPEED_BIN_OPTIONS)
        {
            // Setting to default - 1600 11-11-11
            MEM_COPY(&memoryDeviceDdr3, (void*)&(mc5MemoryDevice[MC5_SPEED_BIN_DEFAULT]), sizeof(Mc5MemoryDevice_t));
        }

        // mis-match speedBinJedecIndex handle
        if (memoryDeviceDdr3.clTimingParam[speedBinJedecIndex].CAS_LATENCY == 0)
        {
            ASSERT(FALSE);
        }

    #elif  (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
        for (i = 0; i < MC5_SPEED_BIN_OPTIONS_DDR4; i++)
        {
            if ((mc5MemoryDeviceDdr4[i].deviceData.speedBin  == speedBinJedec.speedBin ) &&
                (mc5MemoryDeviceDdr4[i].deviceData.clNrcdNrp == speedBinJedec.clNrcdNrp))
            {
                D_PRINTF("-[MC5] speedBinJedec|%d\n", i);
                MEM_COPY(&memoryDeviceDdr4, (void*)&(mc5MemoryDeviceDdr4[i]), sizeof(Mc5MemoryDeviceDdr4_t));
                break;
            }
        }
        if (i == MC5_SPEED_BIN_OPTIONS_DDR4)
        {
            // Setting to defaut - Should be 1600 12-12-12
            MEM_COPY(&memoryDeviceDdr4, (void*)&(mc5MemoryDeviceDdr4[MC5_SPEED_BIN_DEFAULT_DDR4]), sizeof(Mc5MemoryDeviceDdr4_t));
        }
    #endif

    // 0x394 Self-Refresh Timing - values are in nCK
    rDdrMc.chan0SelfRefreshTm.all = MC5_SET_TO_ZERO;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        // For LPDDR tXSR is tRFC + 10ns
        tXsrd = tRfc + 10000;
        rDdrMc.chan0SelfRefreshTm.b.T_XSRD = MC5_PS_TO_NCK(dramFrequencyMHz, tXsrd);
    #else // DDR3 & DDR4
        rDdrMc.chan0SelfRefreshTm.b.T_XSRD = mc5JedecParams.T_XSRD_nck;
    #endif

    // For LPDDR both nCksrx and nCksre should be the same as nCkesr = tCKE + 1nCK
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        nCksrx  = nCke + 1;
    #else // DDR3 & DDR4
        nCksrx = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_CKSRX);
    #endif
    rDdrMc.chan0SelfRefreshTm.b.T_CKSRX   =  nCksrx & 0xF;
    rDdrMc.chan0SelfRefreshTm.b.T_CKSRX_H = (nCksrx & 0x10) >> 4;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        nCksre = nCke + 1;
    #else // DDR3 & DDR4
        nCksre = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_CKSRE);
    #endif
    rDdrMc.chan0SelfRefreshTm.b.T_CKSRE   =  nCksre & 0xF;
    rDdrMc.chan0SelfRefreshTm.b.T_CKSRE_H = (nCksre & 0x10) >> 4;

    // tRFC + 10ns always bigger that 5nCK - so don't check. Value is in nCK.
    rDdrMc.chan0SelfRefreshTm.b.T_XSNR = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, (mc5JedecParams.T_XSNR.ps + tRfc));

    // 0x398 Power-Down Timing - all values in nCK
    rDdrMc.chan0PowerDnTm.all = MC5_SET_TO_ZERO;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        rDdrMc.chan0PowerDnTm.b.T_XARDS = MC5_GET_MAX_NCK_PS_TO_NCK(dramFrequencyMHz, mc5JedecParams.T_XP_L);
        rDdrMc.chan0PowerDnTm.b.T_XP    = MC5_GET_MAX_NCK_PS_TO_NCK(dramFrequencyMHz, mc5JedecParams.T_XP_L);
        rDdrMc.chan0PowerDnTm.b.T_CPDED = 1;    // For LPDDR always 1 nCK
        rDdrMc.chan0PowerDnTm.b.T_PDEN  = 1;    // For LPDDR always 1 nCK
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
        rDdrMc.chan0PowerDnTm.b.T_XARDS = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_XARDS);
        rDdrMc.chan0PowerDnTm.b.T_XP    = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_XP[speedBinJedecIndex]);
        rDdrMc.chan0PowerDnTm.b.T_CPDED = mc5JedecParams.T_CPDED_nck[speedBinJedecIndex];
        rDdrMc.chan0PowerDnTm.b.T_PDEN  = mc5JedecParams.T_PDEN_nck[speedBinJedecIndex];
    #elif  (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
        rDdrMc.chan0PowerDnTm.b.T_XARDS = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_XARDS);
        rDdrMc.chan0PowerDnTm.b.T_XP    = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_XP_4);
        rDdrMc.chan0PowerDnTm.b.T_CPDED = 4;    // For DDR4 always 4 nCK
        rDdrMc.chan0PowerDnTm.b.T_PDEN  = mc5JedecParams.T_PDEN_nck[speedBinJedecIndex];
    #endif

    // T_CKESR = max of T_CKE + 1 nCK
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        nCkesr = nCke;
    #else // DDR3 & DDR4
        nCkesr = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_CKE[speedBinJedecIndex]);
    #endif
    rDdrMc.chan0PowerDnTm.b.T_CKESR = nCkesr + 1;

    // 0x39c - Mode Register Set Timing - all values in nCK
    rDdrMc.chan0ModeRegisterSetTm.all = MC5_SET_TO_ZERO;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        rDdrMc.chan0ModeRegisterSetTm.b.T_MRD = MC5_GET_MAX_NCK_PS_TO_NCK(dramFrequencyMHz, mc5JedecParams.T_MRD_L);
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
        rDdrMc.chan0ModeRegisterSetTm.b.T_MRD = mc5JedecParams.T_MRD_nck;
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
        rDdrMc.chan0ModeRegisterSetTm.b.T_MRD = mc5JedecParams.T_MRD_4_nck;
    #endif

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        // For LPDDR always 2
        rDdrMc.chan0ModeRegisterSetTm.b.T_MOD = 2;
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
        rDdrMc.chan0ModeRegisterSetTm.b.T_MOD = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_MOD);
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
        rDdrMc.chan0ModeRegisterSetTm.b.T_MOD = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_MOD_4);
    #endif

    // 0x3A0 - Activate Timing - all values in nCK
    rDdrMc.chan0ActivateTm.all = MC5_SET_TO_ZERO;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        nRas = MC5_GET_MAX_NCK_PS_TO_NCK(dramFrequencyMHz, mc5LpddrTrasVal);
        nRcd = MC5_GET_MAX_NCK_PS_TO_NCK(dramFrequencyMHz, mc5LpddrMemoryDevice[MC5_LPDDR_SPEED].T_RCD);
        nRp  = MC5_GET_MAX_NCK_PS_TO_NCK(dramFrequencyMHz, mc5LpddrMemoryDevice[MC5_LPDDR_SPEED].T_RP);
        nRpa = MC5_GET_MAX_NCK_PS_TO_NCK(dramFrequencyMHz, mc5LpddrMemoryDevice[MC5_LPDDR_SPEED].T_RPA);

        rDdrMc.chan0ActivateTm.b.T_FAW = MC5_PS_TO_NCK(dramFrequencyMHz, mc5JedecParams.T_FAW_L);
        rDdrMc.chan0ActivateTm.b.T_RAS = nRas + nDerating;
        rDdrMc.chan0ActivateTm.b.T_RCD = nRcd + nDerating;
        #if (MC5_LPDDR_PER_BANK_MODE == MC5_ENABLED)
        rDdrMc.chan0ActivateTm.b.T_RC  = nRas + nRp + nDerating;
        #else
        rDdrMc.chan0ActivateTm.b.T_RC  = nRas + nRpa + nDerating;
        #endif
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
        rDdrMc.chan0ActivateTm.b.T_FAW = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_FAW_ps[speedBinJedecIndex]);
        rDdrMc.chan0ActivateTm.b.T_RAS = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, memoryDeviceDdr3.T_RAS);
        rDdrMc.chan0ActivateTm.b.T_RC  = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, memoryDeviceDdr3.T_RC);
        rDdrMc.chan0ActivateTm.b.T_RCD = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, memoryDeviceDdr3.T_RCD_RP_RPA);
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
        rDdrMc.chan0ActivateTm.b.T_FAW = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_FAW_4_ps);
        rDdrMc.chan0ActivateTm.b.T_RAS = memoryDeviceDdr4.T_RAS;
        rDdrMc.chan0ActivateTm.b.T_RC  = memoryDeviceDdr4.T_RC;
        rDdrMc.chan0ActivateTm.b.T_RCD = memoryDeviceDdr4.T_RCD_RP_RPA;
    #endif

    // 0x3A4 - Pre-charge Timing - all values in nCK
    rDdrMc.chan0PreChargeTm.all     = MC5_SET_TO_ZERO;
    rDdrMc.chan0PreChargeTm.b.T_RTP = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_RTP);

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        rDdrMc.chan0PreChargeTm.b.T_WR  = MC5_GET_MAX_NCK_PS_TO_NCK(dramFrequencyMHz, mc5JedecParams.T_WR_L);
        rDdrMc.chan0PreChargeTm.b.T_RP  = nRp  + nDerating;
        rDdrMc.chan0PreChargeTm.b.T_RPA = nRpa + nDerating;
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
        rDdrMc.chan0PreChargeTm.b.T_WR  = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_WR_ps);
        rDdrMc.chan0PreChargeTm.b.T_RP  = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, memoryDeviceDdr3.T_RCD_RP_RPA);
        rDdrMc.chan0PreChargeTm.b.T_RPA = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, memoryDeviceDdr3.T_RCD_RP_RPA);
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
        rDdrMc.chan0PreChargeTm.b.T_WR  = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_WR_ps);
        rDdrMc.chan0PreChargeTm.b.T_RP  = memoryDeviceDdr4.T_RCD_RP_RPA;
        rDdrMc.chan0PreChargeTm.b.T_RPA = memoryDeviceDdr4.T_RCD_RP_RPA;
    #endif

    // 0x3A8 - CAS/RAS Timing - all values in nCK
    rDdrMc.chan0CasRasTm.all     = MC5_SET_TO_ZERO;
    rDdrMc.chan0CasRasTm.b.T_WTR = MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5JedecParams.T_WTR);

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        nRrd = MC5_GET_MAX_NCK_PS_TO_NCK(dramFrequencyMHz, mc5JedecParams.T_RRD_L);
        rDdrMc.chan0CasRasTm.b.T_RRD = nRrd + nDerating;
    #else // DDR3 & DDR4
        rDdrMc.chan0CasRasTm.b.T_RRD = MC5_GET_MAX_NCK_PS_TO_NCK(dramFrequencyMHz, mc5JedecParams.T_RRD[speedBinJedecIndex]);
    #endif

    rDdrMc.chan0CasRasTm.b.T_CCD = mc5JedecParams.T_CCD_nck;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4) // DDR4 only
        rDdrMc.chan0CasRasTm.b.T_WTR_S = mc5JedecParams.T_WTR_S_nck[speedBinJedecIndex];
        rDdrMc.chan0CasRasTm.b.T_RRD_S = MC5_GET_MAX_NCK_PS_TO_NCK(dramFrequencyMHz, mc5JedecParams.T_RRD_S[speedBinJedecIndex]);
        rDdrMc.chan0CasRasTm.b.T_CCD_S = mc5JedecParams.T_CCD_S_nck;
    #endif

    // DRAM Read Timing

    // 0x3AC - Off-spec Timing - all values in nCK
    rDdrMc.chan0OffSpecTm.all = MC5_SET_TO_ZERO;
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        rDdrMc.chan0OffSpecTm.b.RD_GAP_EXT           = mc5OffSpecTimingParams[cDramCategoryLpDdr].RD_GAP_EXT;
        rDdrMc.chan0OffSpecTm.b.T_CCD_CCS_EXT_DLY    = mc5OffSpecTimingParams[cDramCategoryLpDdr].T_CCD_CCS_EXT_DLY;
        rDdrMc.chan0OffSpecTm.b.T_CCD_CCS_WR_EXT_DLY = mc5OffSpecTimingParams[cDramCategoryLpDdr].T_CCD_CCS_WR_EXT_DLY;
        rDdrMc.chan0OffSpecTm.b.TRWD_EXT_DLY         = mc5OffSpecTimingParams[cDramCategoryLpDdr].TRWD_EXT_DLY;
        rDdrMc.chan0OffSpecTm.b.TWL_EARLY            = mc5OffSpecTimingParams[cDramCategoryLpDdr].TWL_EARLY;
    #else // DDR3 & DDR4
        rDdrMc.chan0OffSpecTm.b.RD_GAP_EXT           = mc5OffSpecTimingParams[cDramCategoryDdr].RD_GAP_EXT;
        rDdrMc.chan0OffSpecTm.b.T_CCD_CCS_EXT_DLY    = mc5OffSpecTimingParams[cDramCategoryDdr].T_CCD_CCS_EXT_DLY;
        rDdrMc.chan0OffSpecTm.b.T_CCD_CCS_WR_EXT_DLY = mc5OffSpecTimingParams[cDramCategoryDdr].T_CCD_CCS_WR_EXT_DLY;
        rDdrMc.chan0OffSpecTm.b.TRWD_EXT_DLY         = mc5OffSpecTimingParams[cDramCategoryDdr].TRWD_EXT_DLY;
        rDdrMc.chan0OffSpecTm.b.TWL_EARLY            = mc5OffSpecTimingParams[cDramCategoryDdr].TWL_EARLY;
    #endif

    // DRAM configuration 1 - don't zero it here - just update values !!!
    rDdrMc.chan0DramCfg1.b.DDR_BURST_LEN = MC5_DDR_BURST_LEN;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        rDdrMc.chan0DramCfg1.b.CAS_LATENCY    = mc5ClTimingParamLpddr3[speedBinJedecIndex].CAS_LATENCY;
        rDdrMc.chan0DramCfg1.b.CA_LATENCY     = 0;
        rDdrMc.chan0DramCfg1.b.WL_SEL         = 0;
        rDdrMc.chan0DramCfg1.b.RL3_OPTION     = 0;
        rDdrMc.chan0DramCfg1.b.SDRAM_MEM_TYPE = MC5_DDR_TYPE_LPDDR3;
        nCwl                                  = mc5ClTimingParamLpddr3[speedBinJedecIndex].CWL;
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
        rDdrMc.chan0DramCfg1.b.CAS_LATENCY    = memoryDeviceDdr3.clTimingParam[speedBinJedecIndex].CAS_LATENCY;
        rDdrMc.chan0DramCfg1.b.SDRAM_MEM_TYPE = MC5_DDR_TYPE_DDR3;
        nCwl                                  = memoryDeviceDdr3.clTimingParam[speedBinJedecIndex].CWL;
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
        rDdrMc.chan0DramCfg1.b.CAS_LATENCY    = memoryDeviceDdr4.clTimingParam.CAS_LATENCY;
        rDdrMc.chan0DramCfg1.b.SDRAM_MEM_TYPE = MC5_DDR_TYPE_DDR4;
        nCwl                                  = memoryDeviceDdr4.clTimingParam.CWL;
    #endif

    rDdrMc.chan0DramCfg1.b.CWL   =  nCwl & 0xF;
    rDdrMc.chan0DramCfg1.b.CWL_4 = (nCwl & 0x10) >> 4;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        // 0x3B0 - DRAM Read Timing
        rDdrMc.chan0DramRdTm.all       = MC5_SET_TO_ZERO;
        rDdrMc.chan0DramRdTm.b.T_DQSCK = MC5_PS_TO_NCK(dramFrequencyMHz, mc5JedecParams.T_DQSCK_ps);

        // 0x3B4 - DRAM Train Timing
        rDdrMc.chan0DramTrainTm.all        = MC5_SET_TO_ZERO;
        rDdrMc.chan0DramTrainTm.b.T_CACKEL = mc5JedecParams.T_CACKEL_nck;
        rDdrMc.chan0DramTrainTm.b.T_CAEXT  = mc5JedecParams.T_CAEXT_nck;
    #endif

    // If code get here, it's definitely NOT recovery
    // Store all registers for further use in recovery mode
    Mc5_StoreRestoreRegJedec(cStoreReg);
}

//-----------------------------------------------------------------------------
/**
    Mc5_SetRegOdt -
    Set ODT (On-Die Termination) registers for CS0 and CS1
    according to the ODT table
**/
//-----------------------------------------------------------------------------
static void Mc5_SetRegOdt(bool isRecovery)
{
    #if (MC5_PHY_ODT_EN == MC5_ENABLED)
    U32 odtVal;
    #endif

    // reg reset
    rDdrMc.chan0DramCfg5Cs0.all = MC5_SET_TO_ZERO;
    rDdrMc.chan0DramCfg5Cs1.all = MC5_SET_TO_ZERO;
    rDdrMc.chan0DramCtrl1.all   = MC5_SET_TO_ZERO;
    rDdrMc.chan0DramCtrl2.all   = MC5_SET_TO_ZERO;

    #if (MC5_PHY_ODT_EN == MC5_ENABLED)
        if (isRecovery)
        {
            Mc5_StoreRestoreRegOdt(cRestoreReg);
            return;
        }
    #else
        return;
    #endif

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)

        #if (MC5_USE_SDK_7_ODT == MC5_ENABLED)
            rDdrMc.chan0DramCfg5Cs0.all = MC5_CHAN0_DRAM_CFG5_CS0_LPDDR;
            rDdrMc.chan0DramCfg5Cs1.all = MC5_CHAN0_DRAM_CFG5_CS1_LPDDR;
            rDdrMc.chan0DramCtrl1.all   = MC5_CHAN0_DRAM_CTRL1_LPDDR;
            rDdrMc.chan0DramCtrl2.all   = MC5_CHAN0_DRAM_CTRL2_LPDDR;

        #else
            rDdrMc.chan0DramCfg5Cs0.all = MC5_CHAN0_DRAM_CFG5_CS0;

            #if (MC5_CS1_ENABLED == MC5_ENABLED)
            if (mc5GlobalData.isCs1Enabled)
            {
                rDdrMc.chan0DramCfg5Cs1.all = MC5_CHAN0_DRAM_CFG5_CS1;
                rDdrMc.chan0DramCtrl1.all   = MC5_CHAN0_DRAM_CTRL1_2CS;
                rDdrMc.chan0DramCtrl2.all   = MC5_CHAN0_DRAM_CTRL2_2CS;
            }
            else
            #endif
            {
                rDdrMc.chan0DramCtrl1.all = MC5_CHAN0_DRAM_CTRL1_1CS;
                rDdrMc.chan0DramCtrl2.all = MC5_CHAN0_DRAM_CTRL2_1CS;
            }
        #endif

    #else // DDR3 & DDR4
        #if (MC5_USE_SDK_7_ODT == MC5_ENABLED)
            rDdrMc.chan0DramCfg5Cs0.all = MC5_CHAN0_DRAM_CFG5_CS0_DDR;
            rDdrMc.chan0DramCfg5Cs1.all = MC5_CHAN0_DRAM_CFG5_CS1_DDR;
            rDdrMc.chan0DramCtrl1.all   = MC5_CHAN0_DRAM_CTRL1_DDR;
            rDdrMc.chan0DramCtrl2.all   = MC5_CHAN0_DRAM_CTRL2_DDR;

        #else
            rDdrMc.chan0DramCfg5Cs0.b.RTT_WR_CS0  = Mc5_GetOdtValue(cWriteToCs0,  ODT_PART_CONTROLLER);
            rDdrMc.chan0DramCfg5Cs0.b.ODS_CS0     = Mc5_GetOdtValue(cReadFromCs0, ODT_PART_CS0);
            rDdrMc.chan0DramCfg5Cs0.b.RTT_NOM_CS0 = Mc5_GetOdtValue(cReadFromCs0, ODT_PART_CONTROLLER);

            odtVal = 0;
            if (Mc5_GetOdtValue(cWriteToCs0, ODT_PART_CS0) > 0) odtVal |= ODT_PART_CS0;
            if (Mc5_GetOdtValue(cWriteToCs0, ODT_PART_CS1) > 0) odtVal |= ODT_PART_CS1;
            rDdrMc.chan0DramCtrl1.b.ODT0_WR_EN = odtVal;

            odtVal = 0;
            if (Mc5_GetOdtValue(cReadFromCs0, ODT_PART_CS0) > 0) odtVal |= ODT_PART_CS0;
            if (Mc5_GetOdtValue(cReadFromCs0, ODT_PART_CS1) > 0) odtVal |= ODT_PART_CS1;
            rDdrMc.chan0DramCtrl1.b.ODT0_RD_EN = odtVal;

            #if (MC5_CS1_ENABLED == MC5_ENABLED)
            if (mc5GlobalData.isCs1Enabled)
            {
                rDdrMc.chan0DramCfg5Cs1.b.RTT_WR_CS1  = Mc5_GetOdtValue(cWriteToCs1,  ODT_PART_CONTROLLER);
                rDdrMc.chan0DramCfg5Cs1.b.ODS_CS1     = Mc5_GetOdtValue(cReadFromCs1, ODT_PART_CS1);
                rDdrMc.chan0DramCfg5Cs1.b.RTT_NOM_CS1 = Mc5_GetOdtValue(cReadFromCs1, ODT_PART_CONTROLLER);

                odtVal = 0;
                if (Mc5_GetOdtValue(cWriteToCs1, ODT_PART_CS0) > 0) odtVal |= ODT_PART_CS0;
                if (Mc5_GetOdtValue(cWriteToCs1, ODT_PART_CS1) > 0) odtVal |= ODT_PART_CS1;
                rDdrMc.chan0DramCtrl1.b.ODT1_WR_EN = odtVal;

                odtVal = 0;
                if (Mc5_GetOdtValue(cReadFromCs1, ODT_PART_CS0) > 0) odtVal |= ODT_PART_CS0;
                if (Mc5_GetOdtValue(cReadFromCs1, ODT_PART_CS1) > 0) odtVal |= ODT_PART_CS1;
                rDdrMc.chan0DramCtrl1.b.ODT1_RD_EN = odtVal;
            }
            #endif

            // Enable termination during READS ONLY
            rDdrMc.chan0DramCtrl2.b.ODT0_SWITCH_MODE     = MC5_ODT_SWITCH_MODE;
            rDdrMc.chan0DramCtrl2.b.ODT1_SWITCH_MODE     = MC5_ODT_SWITCH_MODE;
            rDdrMc.chan0DramCtrl2.b.PAD_TERM_SWITCH_MODE = MC5_PAD_TERM_SWITCH_MODE;
        #endif //#if (MC5_USE_SDK_7_ODT == MC5_ENABLED)
    #endif

    // If code get here, it's definitely NOT recovery
    // Store registers for recovery
    Mc5_StoreRestoreRegOdt(cStoreReg);
}

#if (MC5_USE_SDK_7_PHY_CTRL != MC5_ENABLED)
//-----------------------------------------------------------------------------
/**
    Mc5_SetRegPhyCtrl
**/
//-----------------------------------------------------------------------------
static void Mc5_SetRegPhyCtrl(U32 dramFrequencyMHz)
{
    // PHY Control Register 1
    #if (MC5_PHY_SINGLE_VALUE_INIT == TRUE)
        #if   (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        rDdrMc.chan0PhyCtrl1.all = MC5_CHAN0_PHY_CTRL1_LPDDR; /* set default value */
        #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
        rDdrMc.chan0PhyCtrl1.all = MC5_CHAN0_PHY_CTRL1_DDR3;
        #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
        #error "DDR4 MC5_USE_SDK_7_PHY_CTRL ERROR"
        #endif

    #else
        rDdrMc.chan0PhyCtrl1.all  = MC5_SET_TO_ZERO;
        rDdrMc.chan0PhyCtrl1.b.PHY_RFIFO_RDRST_EN    = MC5_PHY_RFIFO_RESET_EN;
        rDdrMc.chan0PhyCtrl1.b.PHY_RFIFO_RDRST_EARLY = MC5_PHY_RFIFO_RESET_EARLY;

        #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
            rDdrMc.chan0PhyCtrl1.b.DQ_EXT_DLY             = MC5_PHY_DQ_OFFCHIP_DLY_LPDDR;
            rDdrMc.chan0PhyCtrl1.b.PHY_RFIFO_RPTR_DLY_VAL = MC5_PHY_RFIFO_RD_PTR_DLY_LPDDR;
        #else // DDR3 & DDR4
            rDdrMc.chan0PhyCtrl1.b.DQ_EXT_DLY             = MC5_PHY_DQ_OFFCHIP_DLY;
            rDdrMc.chan0PhyCtrl1.b.PHY_RFIFO_RPTR_DLY_VAL = MC5_PHY_RFIFO_RD_PTR_DLY;
        #endif
    #endif

    // PHY Control Register 2 - ZPR/ZNR values will be overwritten by pad calibration
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        rDdrMc.chan0PhyCtrl2.all = MC5_CHAN0_PHY_CTRL2_LPDDR;
    #else // DDR3
        #if (MC5_PHY_SINGLE_VALUE_INIT == TRUE)
        rDdrMc.chan0PhyCtrl2.all = MC5_CHAN0_PHY_CTRL2_DDR3; /* set default value */
        #else
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_MD       = MC5_PHY_DQ_MD;
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZD       = MC5_PHY_DQ_ZD;
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZNDRV    = MC5_PHY_DQ_ZNDRV;
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZNR      = MC5_PHY_DQ_ZNR;
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZNR_MSB  = MC5_PHY_DQ_ZNR_MSB;
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZNTRM    = MC5_PHY_DQ_ZNTRM;
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZPDRV    = MC5_PHY_DQ_ZPDRV;
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZPR      = MC5_PHY_DQ_ZPR;
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZPR_MSB  = MC5_PHY_DQ_ZPR_MSB;
        rDdrMc.chan0PhyCtrl2.b.PHY_DQ_ZPTRM    = MC5_PHY_DQ_ZPTRM;
        rDdrMc.chan0PhyCtrl2.b.PHY_QS_VREF_SEL = MC5_PHY_QS_VREF_SEL;
        #endif
    #endif

    // PHY Control Register 3 - ZPR/ZNR values will be overwritten by pad calibration

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        rDdrMc.chan0PhyCtrl3.all = MC5_CHAN0_PHY_CTRL3_LPDDR;
    #else // DDR3
        #if (MC5_PHY_SINGLE_VALUE_INIT == TRUE)
        if (dramFrequencyMHz < MC5_FREQ_800_MHZ) rDdrMc.chan0PhyCtrl3.all = MC5_CHAN0_PHY_CTRL3_667;
        else                                     rDdrMc.chan0PhyCtrl3.all = MC5_CHAN0_PHY_CTRL3_800;
        #else
        rDdrMc.chan0PhyCtrl3.all                = MC5_SET_TO_ZERO;
        rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_MODE    = MC5_PHY_ADCM_MODE;
        rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_RCVTYPE = MC5_PHY_ADCM_RCVTYPE;
        rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_ZD      = MC5_PHY_ADCM_ZD;
        rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_ZNR     = MC5_PHY_ADCM_ZNR;
        rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_ZNR_MSB = MC5_PHY_ADCM_ZNR_MSB;
        rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_ZPR     = MC5_PHY_ADCM_ZPR;
        rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_ZPR_MSB = MC5_PHY_ADCM_ZPR_MSB;
        if (dramFrequencyMHz < MC5_FREQ_800_MHZ)
        {
            rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_ZNDRV = MC5_PHY_ADCM_ZNDRV_667;
            rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_ZPDRV = MC5_PHY_ADCM_ZPDRV_667;
        }
        else
        {
            rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_ZNDRV = MC5_PHY_ADCM_ZNDRV;
            rDdrMc.chan0PhyCtrl3.b.PHY_ADCM_ZPDRV = MC5_PHY_ADCM_ZPDRV;
        }
        #endif
    #endif

    // PHY Control Register 4 - ZPR/ZNR values will be overwritten by pad calibration
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        rDdrMc.chan0PhyCtrl4.all = MC5_CHAN0_PHY_CTRL4_LPDDR;
    #else // DDR3
        #if (MC5_PHY_SINGLE_VALUE_INIT == TRUE)
        rDdrMc.chan0PhyCtrl4.all = MC5_CHAN0_PHY_CTRL4_DDR3;
        #else
        rDdrMc.chan0PhyCtrl4.b.PHY_CK_ZD           = MC5_PHY_CK_ZD;
        rDdrMc.chan0PhyCtrl4.b.PHY_CK_ZNDRV        = MC5_PHY_CK_ZNDRV;
        rDdrMc.chan0PhyCtrl4.b.PHY_CK_ZNR          = MC5_PHY_CK_ZNR;
        rDdrMc.chan0PhyCtrl4.b.PHY_CK_ZNR_MSB      = MC5_PHY_CK_ZNR_MSB;
        rDdrMc.chan0PhyCtrl4.b.PHY_CK_ZPDRV        = MC5_PHY_CK_ZPDRV;
        rDdrMc.chan0PhyCtrl4.b.PHY_CK_ZPR          = MC5_PHY_CK_ZPR;
        rDdrMc.chan0PhyCtrl4.b.PHY_CK_ZPR_MSB      = MC5_PHY_CK_ZPR_MSB;
        rDdrMc.chan0PhyCtrl4.b.PHY_DQ_RCV_DIS_LATE = MC5_PHY_DQ_RCV_DIS_LATE;
        rDdrMc.chan0PhyCtrl4.b.PHY_DQ_RCVEN        = MC5_PHY_DQ_RCVEN;
        rDdrMc.chan0PhyCtrl4.b.PHY_DQ_RCVEP        = MC5_PHY_DQ_RCVEP;
        rDdrMc.chan0PhyCtrl4.b.PHY_DQ_RCVTYPE      = MC5_PHY_DQ_RCVTYPE;
        rDdrMc.chan0PhyCtrl4.b.PHY_QS_RCVTYPE      = MC5_PHY_QS_RCVTYPE;
        #endif
    #endif
}
#endif

//-----------------------------------------------------------------------------
/**
    Mc5_StoreRestoreRegMemAddr -
    store mem size registers to static variable for use after recovery
**/
//-----------------------------------------------------------------------------
static void Mc5_StoreRestoreRegMemAddr(Mc5StoreRestoreReg_t operation)
{
    if (operation == cRestoreReg)
    {
        rDdrMc.chan0MemControllPhysicalAddrMapCs0.all = mc5RegForRecoverySize.chan0MemControllPhysicalAddrMapCs0;
        rDdrMc.chan0MemControllPhysicalAddrMapCs1.all = mc5RegForRecoverySize.chan0MemControllPhysicalAddrMapCs1;
        rDdrMc.chan0MmapCs0.all = mc5RegForRecoverySize.chan0MmapCs0;
        rDdrMc.chan0MmapCs1.all = mc5RegForRecoverySize.chan0MmapCs1;
    }
    else
    {
        mc5RegForRecoverySize.chan0MemControllPhysicalAddrMapCs0 = rDdrMc.chan0MemControllPhysicalAddrMapCs0.all;
        mc5RegForRecoverySize.chan0MemControllPhysicalAddrMapCs1 = rDdrMc.chan0MemControllPhysicalAddrMapCs1.all;
        mc5RegForRecoverySize.chan0MmapCs0 = rDdrMc.chan0MmapCs0.all;
        mc5RegForRecoverySize.chan0MmapCs1 = rDdrMc.chan0MmapCs1.all;
    }
}

//---------------------------------------------------------------------------------------
/**
    Mc5_StoreRestoreRegJedec - Store / restore JEDEC parameters for recovery
**/
//----------------------------------------------------------------------------------------
static void Mc5_StoreRestoreRegJedec(Mc5StoreRestoreReg_t operation)
{
    // If it's recovery mode and we have all registers initialized we can skip tables lookup
    if (operation == cRestoreReg)
    {
        rDdrMc.chan0Zqc0.all              = mc5RegForRecoveryJedec.chan0Zqc0;
        rDdrMc.chan0Zqc1.all              = mc5RegForRecoveryJedec.chan0Zqc1;
        rDdrMc.chan0RefreshTm.all         = mc5RegForRecoveryJedec.chan0RefreshTm;
        rDdrMc.chan0SelfRefreshTm.all     = mc5RegForRecoveryJedec.chan0SelfRefreshTm;
        rDdrMc.chan0PowerDnTm.all         = mc5RegForRecoveryJedec.chan0PowerDnTm;
        rDdrMc.chan0ModeRegisterSetTm.all = mc5RegForRecoveryJedec.chan0ModeRegisterSetTm;
        rDdrMc.chan0ActivateTm.all        = mc5RegForRecoveryJedec.chan0ActivateTm;
        rDdrMc.chan0PreChargeTm.all       = mc5RegForRecoveryJedec.chan0PreChargeTm;
        rDdrMc.chan0CasRasTm.all          = mc5RegForRecoveryJedec.chan0CasRasTm;
        rDdrMc.chan0OffSpecTm.all         = mc5RegForRecoveryJedec.chan0OffSpecTm;
        rDdrMc.chan0DramCfg1.all          = mc5RegForRecoveryJedec.chan0DramCfg1;
        rDdrMc.chan0DramRdTm.all          = mc5RegForRecoveryJedec.chan0DramRdTm;
        rDdrMc.chan0DramTrainTm.all       = mc5RegForRecoveryJedec.chan0DramTrainTm;
    }
    else
    {
        // Store all registers for further use in recovery mode
        mc5RegForRecoveryJedec.chan0Zqc0              = rDdrMc.chan0Zqc0.all;
        mc5RegForRecoveryJedec.chan0Zqc1              = rDdrMc.chan0Zqc1.all;
        mc5RegForRecoveryJedec.chan0RefreshTm         = rDdrMc.chan0RefreshTm.all;
        mc5RegForRecoveryJedec.chan0SelfRefreshTm     = rDdrMc.chan0SelfRefreshTm.all;
        mc5RegForRecoveryJedec.chan0PowerDnTm         = rDdrMc.chan0PowerDnTm.all;
        mc5RegForRecoveryJedec.chan0ModeRegisterSetTm = rDdrMc.chan0ModeRegisterSetTm.all;
        mc5RegForRecoveryJedec.chan0ActivateTm        = rDdrMc.chan0ActivateTm.all;
        mc5RegForRecoveryJedec.chan0PreChargeTm       = rDdrMc.chan0PreChargeTm.all;
        mc5RegForRecoveryJedec.chan0CasRasTm          = rDdrMc.chan0CasRasTm.all;
        mc5RegForRecoveryJedec.chan0OffSpecTm         = rDdrMc.chan0OffSpecTm.all;
        mc5RegForRecoveryJedec.chan0DramCfg1          = rDdrMc.chan0DramCfg1.all;
        mc5RegForRecoveryJedec.chan0DramRdTm          = rDdrMc.chan0DramRdTm.all;
        mc5RegForRecoveryJedec.chan0DramTrainTm       = rDdrMc.chan0DramTrainTm.all;
    }
}

//-----------------------------------------------------------------------------
/**
    Mc5_StoreRestoreRegOdt - Store / restore ODT registers for recovery
**/
//-----------------------------------------------------------------------------
static void Mc5_StoreRestoreRegOdt(Mc5StoreRestoreReg_t operation)
{
    // If it's recovery mode and we have all registers initialized we can skip tables lookup
    if (operation == cRestoreReg)
    {
        rDdrMc.chan0DramCfg5Cs0.all = mc5RegForRecoveryOdt.chan0DramCfg5Cs0;
        rDdrMc.chan0DramCfg5Cs1.all = mc5RegForRecoveryOdt.chan0DramCfg5Cs1;
        rDdrMc.chan0DramCtrl1.all   = mc5RegForRecoveryOdt.chan0DramCtrl1;
        rDdrMc.chan0DramCtrl2.all   = mc5RegForRecoveryOdt.chan0DramCtrl2;
    }
    else
    {
        // Store registers for recovery
        mc5RegForRecoveryOdt.chan0DramCfg5Cs0 = rDdrMc.chan0DramCfg5Cs0.all;
        mc5RegForRecoveryOdt.chan0DramCfg5Cs1 = rDdrMc.chan0DramCfg5Cs1.all;
        mc5RegForRecoveryOdt.chan0DramCtrl1   = rDdrMc.chan0DramCtrl1.all;
        mc5RegForRecoveryOdt.chan0DramCtrl2   = rDdrMc.chan0DramCtrl2.all;
    }
}

//-----------------------------------------------------------------------------
/**
    Mc5_GetMemIndexBySize -
    find index in memory configuration table for given size
**/
//-----------------------------------------------------------------------------
static U8 Mc5_GetMemIndexBySize(U32 size)
{
    U8 loopEnd;
    U8 i;

    #if   (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    loopEnd = MAX_NUM_OF_SIZE_CONFIGS_LPDDR3;
    #else // DDR3 & DDR4
    loopEnd = MAX_NUM_OF_SIZE_CONFIGS_DDR;
    #endif

    for (i = 0; i < loopEnd; i++)
    {
        if (mc5GlobalData.memSizeConfig[i].size == size)
        {
            if ((mc5GlobalData.memSizeConfig[i].chan0MmapCs0Config & 0xF0000000) == 0)
            {
                continue; // to handle 2GB per CS case
            }
            else
            {
                return i;
            }
        }
    }
    HALT(); // KevinKCChiu: should not get here

    // In case of error return minimal index
    return 0;
}

//-----------------------------------------------------------------------------
/**
    Mc5_GetOdtValue - Get ODT (On-Die Termination) value from the table
**/
//-----------------------------------------------------------------------------
static U8 Mc5_GetOdtValue(Mc5OdtOperation_t operation, U8 part)
{
    // LPDDR3 should not get here
    #if ((MC5_PHY_ODT_EN == MC5_ENABLED) && (MC5_DDR_TYPE != MC5_DDR_TYPE_LPDDR3))

    switch (operation) // no "break"s because every case ends with "return"
    {
    case cWriteToCs0:
        #if (MC5_CS1_ENABLED == MC5_ENABLED)
        if (mc5GlobalData.isCs1Enabled)
        {
            return ODT_Table_CS0_2[ODT_OP_WRITE][part];
        }
        else
        #endif
        {
            return ODT_Table_CS0_1[ODT_OP_WRITE][part];
        }
    case cReadFromCs0:
        #if (MC5_CS1_ENABLED == MC5_ENABLED)
        if (mc5GlobalData.isCs1Enabled)
        {
            return ODT_Table_CS0_2[ODT_OP_READ][part];
        }
        else
        #endif
        {
            return ODT_Table_CS0_1[ODT_OP_READ][part];
        }
    #if (MC5_CS1_ENABLED == MC5_ENABLED)
    case cWriteToCs1:
        return ODT_Table_CS1[ODT_OP_WRITE][part];
    case cReadFromCs1:
        return ODT_Table_CS1[ODT_OP_READ][part];
    #endif
    default:
        return 0;
    }

    #else
    return 0;
    #endif
}

#if (MC5_CS_AUTODETECT == MC5_ENABLED)
//-----------------------------------------------------------------------------
/**
    Mc5_Cs1IsEnabled -
    Check if CS1 is connected on PCB - write 3 different patterns
    to the CS1 start address and verify it
**/
//-----------------------------------------------------------------------------
static bool Mc5_Cs1IsEnabled(void)
{
    volatile U32 address;
             U32 delay;

    address = (mc5GlobalData.cs1StartAddress) & 0xF0000000;
    delay   = MC5_MEM_TEST_DELAY_US;

    // KevinKCChiu: Since force 32bit width, skip 16bit test; SAVE Time!!!
    /*
    MC5_SIMPLE_PATTERN_TEST_16(address + MC5_MEM_TEST_OFFSET, MC5_PATTERN_TEST_16,  delay);
    MC5_SIMPLE_PATTERN_TEST_16(address + MC5_MEM_TEST_OFFSET, MC5_PATTERN_FULL_16,  delay);
    MC5_SIMPLE_PATTERN_TEST_16(address + MC5_MEM_TEST_OFFSET, MC5_PATTERN_EMPTY_16, delay);
    */

    MC5_SIMPLE_PATTERN_TEST_32(address + MC5_MEM_TEST_OFFSET, MC5_PATTERN_TEST_32,  delay);
    MC5_SIMPLE_PATTERN_TEST_32(address + MC5_MEM_TEST_OFFSET, MC5_PATTERN_FULL_32,  delay);
    MC5_SIMPLE_PATTERN_TEST_32(address + MC5_MEM_TEST_OFFSET, MC5_PATTERN_EMPTY_32, delay);

    return TRUE;
}
#endif

#if (MC5_AUTODETECT_MEMORY_SIZE == MC5_ENABLED)
//-----------------------------------------------------------------------------
/**
    Mc5_MemSizeDetect - detect memory size and update global DB
**/
//-----------------------------------------------------------------------------
static void Mc5_MemSizeDetect(bool isLpddr)
{
    Mc5MemSizeConfig_t* memSizeConfig;
    Error_t             status;
    U32 singleDeviceDensity;
    U8  maxIndex;
    U8  i, j;

    memSizeConfig       = mc5GlobalData.memSizeConfig;
    status              = cEcNoError;
    singleDeviceDensity = MC5_SINGLE_DEVICE_DENSITY_DEFAULT;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    maxIndex = MAX_NUM_OF_SIZE_CONFIGS_LPDDR3 - 1;
    #else // DDR3 & DDR4
    maxIndex = MAX_NUM_OF_SIZE_CONFIGS_DDR - 1;
    #endif

    // Start with maximum possible size
    for (i = 0; i <= maxIndex; i++)
    {
        j = maxIndex - i;

        // Ignore mis-match setting
        if ((!mc5GlobalData.isCs1Enabled) && (memSizeConfig[j].chan0MmapCs1Config != 0))
        {
            continue;
        }

        // Assign test chan0Mmap setting
        rDdrMc.chan0MmapCs0.all = memSizeConfig[j].chan0MmapCs0Config;
        rDdrMc.chan0MmapCs1.all = memSizeConfig[j].chan0MmapCs1Config;

        singleDeviceDensity = Mc5_GetSingleDeviceDensity(isLpddr, memSizeConfig[j].size);
        Mc5_SetBlckAddrBoundary(memSizeConfig[j].size, singleDeviceDensity);

        status = Mc5_TestMemSizeConfig(j);
        if (status == cEcNoError)
        {
            D_PRINTF("-[MC5] DetectSize S%d|T%d\n", singleDeviceDensity   >> 20,
                                                    memSizeConfig[j].size >> 20);
            break;
        }
    }

    // If no configuration found - LoadTypical
    if (status == cEcError)
    {
        for (i = 0; i <= maxIndex; i++)
        {
            j = maxIndex - i;

            // Ignore mis-match setting
            if ((!mc5GlobalData.isCs1Enabled) && (memSizeConfig[j].chan0MmapCs1Config != 0))
            {
                continue;
            }

            if (memSizeConfig[j].size == halDramInfo.size)
            {
                break;
            }
        }
        D_PRINTF("-[MC5] DetectSize LoadTypical|%d\n", j);
        rDdrMc.chan0MmapCs0.all = memSizeConfig[j].chan0MmapCs0Config;
        rDdrMc.chan0MmapCs1.all = memSizeConfig[j].chan0MmapCs1Config;
    }
    // Update global database - single device density, only if proper configuration found
    else
    {
        mc5GlobalData.deviceDensity = singleDeviceDensity;
    }

    // Update global database
    mc5GlobalData.totalSize       =  memSizeConfig[j].size;
    mc5GlobalData.cs1StartAddress = (memSizeConfig[j].chan0MmapCs1Config) & 0xF0000000;
}

//-----------------------------------------------------------------------------
/**
    Mc5_GetSingleDeviceDensity -
    Calculate single device density from total memory size,
    number of CSs and data width.
    Please note -
    the code below assumed that 8-bit devices will NOT be used!!!
**/
//-----------------------------------------------------------------------------
static U32 Mc5_GetSingleDeviceDensity(bool isLpddr, U32 totalMemSize)
{
    #if (MC5_SINGLE_DEVICE_DENSITY == MC5_SINGLE_DEVICE_DENSITY_AUTO)
    U32 singleDeviceDensity;

    // KevinKCChiu: set singleDeviceDensity to max for single CS
    singleDeviceDensity = totalMemSize;

    // KevinKCChiu: handle if 2 CS are enabled
    if (mc5GlobalData.isCs1Enabled)
    {
        if (totalMemSize > MC5_SIZE_1_GB) singleDeviceDensity = MC5_SIZE_1_GB;
        else                              singleDeviceDensity = MC5_SIZE_1_GB / 2;
    }

    if (!isLpddr)
    {
        singleDeviceDensity = singleDeviceDensity / 2;
    }

    return singleDeviceDensity;

    #else
    return MC5_SINGLE_DEVICE_DENSITY_DEFAULT;
    #endif
}

//-----------------------------------------------------------------------------
/**
    Mc5_SetBlckAddrBoundary -
    Set proper configuration -
    number of columns, rows and BA boundary to the register
    For more information see Table 6
**/
//-----------------------------------------------------------------------------
static void Mc5_SetBlckAddrBoundary(U32 totalMemSize, U32 singleDeviceDensity)
{
    Mc5BlckAddrBoundary_t* boundaryTable;
    U8 i;

    boundaryTable = mc5GlobalData.boundaryTable;

    for (i = 0; i < MC5_NUM_OF_BA_OPTIONS; i++)
    {
        //D_PRINTF("-[MC5] SetBlckAddr %x|%x\n", boundaryTable[i].singleDeviceDensity, singleDeviceDensity);
        if (boundaryTable[i].singleDeviceDensity == singleDeviceDensity)
        {
            rDdrMc.chan0MemControllPhysicalAddrMapCs0.all                 = MC5_SET_TO_ZERO;
            rDdrMc.chan0MemControllPhysicalAddrMapCs0.b.BANK_ADDR_MAP_CH0 = boundaryTable[i].blckAddrBoundary;
            rDdrMc.chan0MemControllPhysicalAddrMapCs0.b.NUM_COL_ADDR_CH0  = MC5_NUM_OF_COLUMNS;
            rDdrMc.chan0MemControllPhysicalAddrMapCs0.b.NUM_ROW_ADDR_CH0  = boundaryTable[i].numOfRows;
            #if (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
                rDdrMc.chan0MemControllPhysicalAddrMapCs0.b.NUM_BANKS_CH0     = MC5_NUM_OF_BANKS_DDR4;
                rDdrMc.chan0MemControllPhysicalAddrMapCs0.b.NUM_BANK_GRP_CH0  = MC5_NUM_OF_BANK_GROUPS_DDR4;
                rDdrMc.chan0MemControllPhysicalAddrMapCs0.b.DDR4_DEVICE_TYPE_CH0 = MC5_DDR4_DEVICE_TYPE;
            #else // != DDR4
                rDdrMc.chan0MemControllPhysicalAddrMapCs0.b.NUM_BANKS_CH0     = MC5_NUM_OF_BANKS;
                rDdrMc.chan0MemControllPhysicalAddrMapCs0.b.NUM_BANK_GRP_CH0  = MC5_NUM_OF_BANK_GROUPS;
            #endif

            if (mc5GlobalData.isCs1Enabled)
            {
                rDdrMc.chan0MemControllPhysicalAddrMapCs1.all = MC5_SET_TO_ZERO;
                rDdrMc.chan0MemControllPhysicalAddrMapCs1.all = rDdrMc.chan0MemControllPhysicalAddrMapCs0.all;
                // Special case - for 1.5GB size CS0 is 1GB and CS1 is a half of it - 512MB
                if (totalMemSize == MC5_SIZE_1_5_GB)
                {
                    rDdrMc.chan0MemControllPhysicalAddrMapCs1.b.BANK_ADDR_MAP_CH0 = boundaryTable[i - 1].blckAddrBoundary;
                    rDdrMc.chan0MemControllPhysicalAddrMapCs1.b.NUM_ROW_ADDR_CH0  = boundaryTable[i - 1].numOfRows;
                }
            }
            //break;
            return;
        }
    }
    // restore default
    rDdrMc.chan0MemControllPhysicalAddrMapCs1.all = MC5_CHAN0_MEM_CTRL_PHYS_ADDR_MAP;
}

//---------------------------------------------------------------------------------------
/**
    Mc5_TestMemSizeConfig - check memory configuration given by index.
**/
//----------------------------------------------------------------------------------------
static Error_t Mc5_TestMemSizeConfig(U8 index)
{
    Mc5MemSizeConfig_t* memSizeConfig;
    volatile U32 address;

    memSizeConfig = mc5GlobalData.memSizeConfig;

    // Zero all possible memory end addresses with 16MB step
    for (address =  MC5_MEM_START_ADDRESS + MC5_MEM_TEST_OFFSET;
         address <  MC5_MEM_START_ADDRESS + memSizeConfig[index].size;
         address += MC5_SIZE_16_MB)
    {
        MC5_WRITE_16(address, MC5_PATTERN_EMPTY_32);
    }

    // Set RAM base address + MC5_MEM_TEST_OFFSET to known pattern
    MC5_WRITE_16((MC5_MEM_START_ADDRESS + MC5_MEM_TEST_OFFSET), MC5_PATTERN_TEST_16);

    // Wait before reading data back
    HalTimer_DelayUsec(MC5_MEM_TEST_DELAY_US);

    // If data cannot be readed back the configuration is wrong
    if (MC5_READ_16(MC5_MEM_START_ADDRESS + MC5_MEM_TEST_OFFSET) != MC5_PATTERN_TEST_16)
    {
        return cEcError;
    }

    // Check if pattern can be readed from addresses within defined memory with 16MB step
    for (address =  MC5_MEM_START_ADDRESS + MC5_MEM_TEST_OFFSET + MC5_SIZE_16_MB;
         address <  MC5_MEM_START_ADDRESS + memSizeConfig[index].size;
         address += MC5_SIZE_16_MB)
    {
        if ((MC5_READ_16(address) == MC5_PATTERN_TEST_16) ||
            (MC5_READ_16(address) != MC5_PATTERN_EMPTY_32))
        {
            return cEcError;
        }
    }
    return cEcNoError;
}
#endif

//-----------------------------------------------------------------------------
/**
    Mc5_InitDdrSequence - init sequence after setting DDR parameters.
**/
//-----------------------------------------------------------------------------
static void Mc5_InitDdrSequence(bool isLpddr, bool isRecovery)
{
    if ((isRecovery) && (!isLpddr))
    {
        rDdrMc.ddrmcUserInitiateCmd0.all = MC5_DDR_MC_USER_INIT_CMD0_RCVR;
    }
    else
    {
        rDdrMc.ddrmcUserInitiateCmd0.all = MC5_DDR_MC_USER_INITIATE_CMD0;
        rDdrMc.ddrmcUserInitiateCmd1.all = MC5_DDR_MC_USER_INITIATE_CMD1;
    }

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    rDdrMc.memControllPowerCtrl.all = MC5_MEM_CONTROLL_POWER_CTRL_LPDDR;
    #else // DDR3 & DDR4
    rDdrMc.memControllPowerCtrl.all = MC5_MEM_CONTROLL_POWER_CTRL;
    #endif

    if ((!isRecovery) || (isLpddr))
    {
        rDdrMc.ddrmcPerformaCntrCfg0.all = MC5_SET_TO_ZERO;
        rDdrMc.ddrmcTstMode0.all         = MC5_SET_TO_ZERO;
    }

    rDdrMc.ddrmcTstMode1.all = MC5_DDR_MC_TST_MODE1;

    while (rDdrMc.dramStat.b.INIT_DONE00 != 1)
    {
        // Waiting until DDR3 initialization is done
    }

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    // Write MA address for MR#1
    rDdrMc.ddrmcUserInitiateCmd1.all = MC5_DDR_MC_USER_INITIATE_CMD1_LPDDR + 1;
    // Write MA address for MR#2
    rDdrMc.ddrmcUserInitiateCmd1.all = MC5_DDR_MC_USER_INITIATE_CMD1_LPDDR + 2;
    // Write MA address for MR#3
    rDdrMc.ddrmcUserInitiateCmd1.all = MC5_DDR_MC_USER_INITIATE_CMD1_LPDDR + 3;
    #endif

    if (isRecovery)
    {
        // Unmask CKE and RESET
        #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        // KevinKCChiu
        // 20161205:Errata SSDD-1263
        // Add this delay to make sure first Refresh ouccurs 1us after CKE pin goes high
        HalCpu_DelayUsec(207);
        rDdrMc.chan0MemCtrl1.all = MC5_CHAN0_MEM_CTRL1_LPDDR;
        #else // DDR3 & DDR4
        rDdrMc.chan0MemCtrl1.all = MC5_SET_TO_ZERO;
        #endif
    }
}

//-----------------------------------------------------------------------------
/**
    Mc5_InitDdrPhy - Initialization of DDR phy.
**/
//-----------------------------------------------------------------------------
static void Mc5_InitDdrPhy(U32 dramFrequencyMHz, bool isLpddr, bool isRecovery)
{
    Mc5PhyCtrlRegVal_t* pMc5PhyCtrlReg;
    U8 i;

    // Enable DQ/DM/DQS/DQSB Power
    // Disable DQ, DM, DQS termination
    // Disable DQ, DM, DQS power down

    rDdrMc.chan0PhyDataByteCtrlB0.all = MC5_SET_TO_ZERO;
    rDdrMc.chan0PhyDataByteCtrlB1.all = MC5_SET_TO_ZERO;
    rDdrMc.chan0PhyDataByteCtrlB2.all = MC5_SET_TO_ZERO;
    rDdrMc.chan0PhyDataByteCtrlB3.all = MC5_SET_TO_ZERO;

    // Read Leveling & Write Leveling
    // In recovery mode set RL only for LPDDR
    if ((!isRecovery) || (isLpddr))
    {
        Mc5_SetRegReadLeveling();
        Mc5_SetRegWriteLeveling();
    }

    // PHY DLL Control Register
    for (i = 0; i < MC5_MAX_SUPPORT_BYTE; i++)
    {
        rDdrMc.chan0PhyDllCtrlB[i].all = MC5_SET_TO_ZERO;

        if (mc5HwInfoData.dataValid[cMc5HwInfoDll] == TRUE)
        {
            rDdrMc.chan0PhyDllCtrlB[i].b.DLL_PHSEL_Bx  = mc5HwInfoData.phyDllData.dllPositive[i];
            rDdrMc.chan0PhyDllCtrlB[i].b.DLL_PHSEL1_Bx = mc5HwInfoData.phyDllData.dllNegative[i];
        }
        else
        {
            rDdrMc.chan0PhyDllCtrlB[i].b.DLL_PHSEL_Bx  = mc5DllDataDefault.dllPositive[i];
            rDdrMc.chan0PhyDllCtrlB[i].b.DLL_PHSEL1_Bx = mc5DllDataDefault.dllNegative[i];
        }
    }

    pMc5PhyCtrlReg = (Mc5PhyCtrlRegVal_t*)&phyCtrlRegValTable;

    #if (MC5_USE_SDK_7_PHY_CTRL == MC5_ENABLED)
    // PHY Control Register 1
    // PHY Control Register 2 - ZPR/ZNR values will be overwritten by pad calibration
    // PHY Control Register 3 - ZPR/ZNR values will be overwritten by pad calibration
    // PHY Control Register 4 - ZPR/ZNR values will be overwritten by pad calibration
    rDdrMc.chan0PhyCtrl1.all = pMc5PhyCtrlReg->phyCtrl1Val;
    rDdrMc.chan0PhyCtrl2.all = pMc5PhyCtrlReg->phyCtrl2Val;
    rDdrMc.chan0PhyCtrl3.all = pMc5PhyCtrlReg->phyCtrl3Val;
    rDdrMc.chan0PhyCtrl4.all = pMc5PhyCtrlReg->phyCtrl4Val;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
    // KevinKCChiu
    // 20161205: change receive type for low temperature handle
    rDdrMc.chan0PhyCtrl4.b.PHY_DQ_RCVTYPE = 0;
    rDdrMc.chan0PhyCtrl4.b.PHY_QS_RCVTYPE = 0;
    #endif

    #else
    Mc5_SetRegPhyCtrl(dramFrequencyMHz);
    #endif

    if (isRecovery)
    {
        Mc5_SetRegPadCal(isLpddr, 0, 0);
    }

    // PHY Control Register 6
    #if (MC5_USE_SDK_7_PHY_CTRL == MC5_ENABLED)
    rDdrMc.chan0PhyCtrl6.all = pMc5PhyCtrlReg->phyCtrl6Val;
    #else
        #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        rDdrMc.chan0PhyCtrl6.all = MC5_CHAN0_PHY_CTRL6_LPDDR;
        #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
        rDdrMc.chan0PhyCtrl6.all = MC5_CHAN0_PHY_CTRL6_DDR3;
        #else // DDR4
        #error "DDR4 only support MC5_USE_SDK_7_PHY_CTRL"
        #endif
    #endif

    // PHY Control Register 15
    rDdrMc.chan0PhyCtrl15.all = MC5_SET_TO_ZERO;

    // PHY Control Register 8
    rDdrMc.chan0PhyCtrl8.all = MC5_CHAN0_PHY_CTRL8;
    // Disable DLL fast lock for DDR4
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
    rDdrMc.chan0PhyCtrl8.b.DLL_FAST_LCK = 0;
    #endif

    // PHY Control Register 5
    #if (MC5_USE_SDK_7_PHY_CTRL == MC5_ENABLED)
    rDdrMc.chan0PhyCtrl5.all = pMc5PhyCtrlReg->phyCtrl5Val;
    #else
        // To disable DQ pull down
        #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        rDdrMc.chan0PhyCtrl5.all = MC5_CHAN0_PHY_CTRL5_LPDDR;
        #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
        rDdrMc.chan0PhyCtrl5.all = MC5_CHAN0_PHY_CTRL5_DDR3;
        #else // DDR4
        #error "DDR4 only support MC5_USE_SDK_7_PHY_CTRL"
        #endif
    #endif

    // PHY Control Register 9
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    rDdrMc.chan0PhyCtrl9.all = MC5_PHY_CTRL9_DLL_RST;
    rDdrMc.chan0PhyCtrl9.all = MC5_PHY_CTRL9_UPD_EN_PULSE;
    rDdrMc.chan0PhyCtrl9.all = MC5_PHY_CTRL9_SYNC_EN;
    #else // DDR3 & DDR4
    // PHY Sync enable & DLL Update
    rDdrMc.chan0PhyCtrl9.all = MC5_CHAN0_PHY_CTRL9;
    #endif

    // PHY Control Registers 13-14
    rDdrMc.chan0PhyCtrl13.all = MC5_SET_TO_ZERO;  // Enable BA & AD pad power
    rDdrMc.chan0PhyCtrl14.all = MC5_SET_TO_ZERO;  // Enable OTHER PADS Power

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
    rDdrMc.chan0PhyWrAndRdLvlCtrl.all = 0x90000000;
    #else
    rDdrMc.chan0PhyWrAndRdLvlCtrl.all = MC5_SET_TO_ZERO;
    #endif
}

//-----------------------------------------------------------------------------
/**
    Mc5_InitDdrBasic -
    Basic init of memory to allow auto detection of different values,
    pad calibration and dll tune.
    In case everything set to manual this is final configuration.
**/
//-----------------------------------------------------------------------------
static void Mc5_InitDdrBasic(U32 dramFrequencyMHz, bool isLpddr, bool isRecovery)
{
    if (isRecovery)
    {
        // Mask CKE and RESET
        rDdrMc.chan0MemCtrl1.all = MC5_CHAN0_MEM_CTRL1_RCVR;
    }

    Mc5_InitDdrPhy(dramFrequencyMHz, isLpddr, isRecovery);

    rDdrMc.memControllCtrl0.all = MC5_SET_TO_ZERO;

    // RRB Starvation Timer value
    rDdrMc.rrbStarvatiPreventi0.all = MC5_RRB_STARVATION_PREVENTION_0;
    MC5_REG_WRITE(0x8C, MC5_RRB_STARVATION_PREVENTION_1);

    if (isRecovery) Mc5_StoreRestoreRegMemAddr(cRestoreReg);
    else            Mc5_SetRegMemAddrMap();

    if (isRecovery) Mc5_SetRegOdt(TRUE);
    else            rDdrMc.chan0DramCtrl1.all = MC5_SET_TO_ZERO; // Start with ODT disabled

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    rDdrMc.chan0DramCtrl3.all = MC5_CHAN0_DRAM_CTRL3;
    #endif

    // Channel 0 Memory Control 1
    if (!isRecovery)
    {
        #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        rDdrMc.chan0MemCtrl1.all = MC5_CHAN0_MEM_CTRL1_LPDDR;
        #else // DDR3 & DDR4
        rDdrMc.chan0MemCtrl1.all = MC5_SET_TO_ZERO;
        #endif
    }

    // Channel 0 Memory Control 2
    #if   (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    rDdrMc.chan0MemCtrl2.all = MC5_CHAN0_MEM_CTRL2_LPDDR;
    #else // DDR3 & DDR4
    rDdrMc.chan0MemCtrl2.all = MC5_SET_TO_ZERO;
    #endif

    // Channel 0 Memory Control 3
    rDdrMc.chan0MemCtrl3.all = MC5_SET_TO_ZERO;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    Mc5_PhyInitSequenceSync2(1, 3, 1, 0);
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
    Mc5_PhyInitSequenceSync2(1, 3 ,2, 0);
    #endif

    rDdrMc.chan0DramCfg2.all = MC5_SET_TO_ZERO;
    rDdrMc.chan0DramCfg1.all = MC5_SET_TO_ZERO;

    // Set DDR Data Width - 16 or 32 bit.
    rDdrMc.chan0DramCfg1.b.DDR_DATA_WIDTH = MC5_DDR_DATA_WIDTH;

    rDdrMc.chan0DdrInitialiTmCtrl0.all = MC5_SET_TO_ZERO;
    rDdrMc.chan0DdrInitialiTmCtrl1.all = MC5_SET_TO_ZERO;

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    rDdrMc.chan0DdrInitialiTmCtrl0.b.INIT_CNT_NOP = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5CountValPs.initNop[cDramCategoryLpDdr]);
    rDdrMc.chan0DdrInitialiTmCtrl1.b.INIT_CNT     = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5CountValPs.coldReset[cDramCategoryLpDdr]);
    rDdrMc.chan0DdrInitialiTmCtrl1.b.RST_CNT      = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5CountValPs.warmReset[cDramCategoryLpDdr]);
    #else // DDR3 & DDR4
    rDdrMc.chan0DdrInitialiTmCtrl0.b.INIT_CNT_NOP = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5CountValPs.initNop[cDramCategoryDdr]);
    rDdrMc.chan0DdrInitialiTmCtrl1.b.INIT_CNT     = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5CountValPs.coldReset[cDramCategoryDdr]);
    rDdrMc.chan0DdrInitialiTmCtrl1.b.RST_CNT      = MC5_PS_TO_NCK_WITH_COEF(dramFrequencyMHz, mc5CountValPs.warmReset[cDramCategoryDdr]);
    #endif

    if (isRecovery) Mc5_StoreRestoreRegJedec(cRestoreReg);
    else            Mc5_SetRegJedecParams(dramFrequencyMHz, FALSE);

    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    rDdrMc.ddrmcExclusivMonitorCtrl.all = MC5_DDR_MC_EXCLUSIV_MONITOR_CTRL_LPDDR;
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
    rDdrMc.ddrmcExclusivMonitorCtrl.all = MC5_DDR_MC_EXCLUSIV_MONITOR_CTRL_DDR3;
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
    rDdrMc.ddrmcExclusivMonitorCtrl.all = MC5_DDR_MC_EXCLUSIV_MONITOR_CTRL_DDR4;
    #endif

    Mc5_InitDdrSequence(isLpddr, isRecovery);
}

//-----------------------------------------------------------------------------
/**
    Mc5_InitGlobalData(U32 frequencyMT, bool isLpddr, bool initDone)
**/
//-----------------------------------------------------------------------------
static void Mc5_InitGlobalData(U32 frequencyMT, bool isLpddr, bool initDone)
{
    // KevinKCChiu: if initDone, get everything from register
    if (!initDone)
    {
        gDramType = MC5_DRAM_TYPE_T;
    }
    else
    {
        switch (rDdrMc.chan0DramCfg1.b.SDRAM_MEM_TYPE)
        {
        case MC5_DDR_TYPE_LPDDR3:
            gDramType = cDramLpDdr3;
            break;
        case MC5_DDR_TYPE_DDR3:
            gDramType = cDramDdr3;
            break;
        case MC5_DDR_TYPE_DDR4:
            gDramType = cDramDdr4;
            break;
        default:
            HALT(); // KevinKCChiu: should not get here for current HW config.
            break;
        }
    }

    mc5GlobalData.frequencyMT      = frequencyMT;                   // CPU    frequency = data rate
    mc5GlobalData.dramFrequencyMHz = MC5_DR_TO_FREQ(frequencyMT);   // Memory frequency
    mc5GlobalData.dramType         = gDramType;                     // DRAM type
    mc5GlobalData.isLpddr          = isLpddr;                       // Is it LPDDR memory?
    mc5GlobalData.deviceDensity    = MC5_SINGLE_DEVICE_DENSITY_DEFAULT;

    if (!initDone)
    {
        // Set everything to default; force 32 bit for Eldora
        #if   (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        mc5GlobalData.boundaryTable = (Mc5BlckAddrBoundary_t*)blckAddrBoundaryTable32;
        #else // DDR3 & DDR4
        mc5GlobalData.boundaryTable = (Mc5BlckAddrBoundary_t*)blckAddrBoundaryTable16;
        #endif
        mc5GlobalData.dataWidth     = MC5_DDR_DATA_WIDTH;

        // KevinKCChiu: Assume two CS config
        #if   (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        mc5GlobalData.memSizeConfig = (Mc5MemSizeConfig_t*)memSizeConfigLpddr3;
        #else // DDR3 & DDR4
        mc5GlobalData.memSizeConfig = (Mc5MemSizeConfig_t*)memSizeConfigDdr3;
        #endif
        mc5GlobalData.isCs1Enabled    = TRUE;
        mc5GlobalData.cs1StartAddress = MC5_SET_TO_ZERO;
        mc5GlobalData.totalSize       = MC5_DDR_SIZE; // Total memory size
    }
    else
    {
        U32 i;

        // Get config. from register
        if (rDdrMc.chan0DramCfg1.b.DDR_DATA_WIDTH == MC5_DDR_DATA_WIDTH_32)
        {
            #if   (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
            mc5GlobalData.boundaryTable = (Mc5BlckAddrBoundary_t*)blckAddrBoundaryTable32;
            #else // DDR3 & DDR4
            mc5GlobalData.boundaryTable = (Mc5BlckAddrBoundary_t*)blckAddrBoundaryTable16;
            #endif
            mc5GlobalData.dataWidth     = MC5_DDR_DATA_WIDTH_32;
        }
        else
        {
            mc5GlobalData.boundaryTable = (Mc5BlckAddrBoundary_t*)blckAddrBoundaryTable16;
            mc5GlobalData.dataWidth     = MC5_DDR_DATA_WIDTH_16;
            HALT(); // KevinKCChiu: should not get here for Eldora
        }

        #if   (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        mc5GlobalData.memSizeConfig = (Mc5MemSizeConfig_t*)memSizeConfigLpddr3;
        #else // DDR3 & DDR4
        mc5GlobalData.memSizeConfig = (Mc5MemSizeConfig_t*)memSizeConfigDdr3;
        #endif
        if ((rDdrMc.chan0MmapCs1.all != 0) &&
            (rDdrMc.chan0MmapCs1.all != MC5_CS1_NON_VALID_VALUE))
        {
            mc5GlobalData.isCs1Enabled  = TRUE;

            for (i = 0; i < MAX_NUM_OF_SIZE_CONFIGS_DRAM; i++)
            {
                if (rDdrMc.chan0MmapCs1.all == mc5GlobalData.memSizeConfig[i].chan0MmapCs1Config)
                {
                    mc5GlobalData.cs1StartAddress = mc5GlobalData.memSizeConfig[i].chan0MmapCs1Config & 0xF0000000;
                    mc5GlobalData.totalSize       = mc5GlobalData.memSizeConfig[i].size;
                    break;
                }
            }
        }
        else
        {
            mc5GlobalData.isCs1Enabled    = FALSE;
            mc5GlobalData.cs1StartAddress = MC5_SET_TO_ZERO;

            // Get CS0 setting
            for (i = 0; i < MAX_NUM_OF_SIZE_CONFIGS_DRAM; i++)
            {
                if (rDdrMc.chan0MmapCs0.all == mc5GlobalData.memSizeConfig[i].chan0MmapCs0Config)
                {
                    mc5GlobalData.totalSize = mc5GlobalData.memSizeConfig[i].size;
                    break;
                }
            }
        }
        #if (MC5_AUTODETECT_MEMORY_SIZE == MC5_ENABLED)
        mc5GlobalData.deviceDensity = Mc5_GetSingleDeviceDensity(isLpddr, mc5GlobalData.totalSize);
        #endif
    }
}

//-----------------------------------------------------------------------------
/**
    Mc5_InitHwInfoData
**/
//-----------------------------------------------------------------------------
static void Mc5_InitHwInfoData(void)
{
    mc5HwInfoData.dataValid[cMc5HwInfoDll] = HalDdrDll_VerifyHwInfo();
    mc5HwInfoData.dataValid[cMc5HwInfoCal] = Mc5_PadCalVerifyHwInfo();

    if (mc5HwInfoData.dataValid[cMc5HwInfoDll] == TRUE)
    {
        MEM_COPY((PVOID)&mc5HwInfoData.phyDllData.dllPositive[0],
                 (PVOID)&smHwInfo.d.dramDll.dllPositive[0], 8);
        #if CHK_CODE_TYPE(CODE_BOOT) || CHK_FW_TYPE(FW_DNLD)
        D_PRINTF_DRAM("-[MC5] DLL P%2X|%2X|%2X|%2X N%2X|%2X|%2X|%2X\n",
                        mc5HwInfoData.phyDllData.dllPositive[0],
                        mc5HwInfoData.phyDllData.dllPositive[1],
                        mc5HwInfoData.phyDllData.dllPositive[2],
                        mc5HwInfoData.phyDllData.dllPositive[3],
                        mc5HwInfoData.phyDllData.dllNegative[0],
                        mc5HwInfoData.phyDllData.dllNegative[1],
                        mc5HwInfoData.phyDllData.dllNegative[2],
                        mc5HwInfoData.phyDllData.dllNegative[3]);
        #endif
    }
}

//-----------------------------------------------------------------------------
/**
    Mc5_AutoDetectRoutine
**/
//-----------------------------------------------------------------------------
static void Mc5_AutoDetectRoutine(bool isLpddr)
{
    /*
    Current Setting:
    1. Since 2CS is forced on; update CS config here
    2. Mc5_SetRegOdt (CS1 may turn off)
    3. Detect total memory size & Update single <device density>
     - Update Mc5_SetRegJedecParams since some params are <device density> depended
    4. Update rDdrMc.chan0MmapCs1.all to NON_VALID_VALUE if CS1 not enabled
    5. Perform init sequence again
    */

    bool settingChanged = FALSE;

    #if (MC5_CS_AUTODETECT == MC5_ENABLED)
    mc5GlobalData.isCs1Enabled = Mc5_Cs1IsEnabled();
    D_PRINTF("-[MC5] DetectCs %1X\n", mc5GlobalData.isCs1Enabled + 1);

    if (!mc5GlobalData.isCs1Enabled) // CS1 not enabled
    {
        settingChanged = TRUE;
    }
    #else
    // Force 2CS on handle
    #endif

    Mc5_SetRegOdt(FALSE);

    // Size detect
    #if (MC5_AUTODETECT_MEMORY_SIZE == MC5_ENABLED)
    Mc5_MemSizeDetect(isLpddr);

    #if (MC5_AUTODETECT_DEVICE_DENSITY == MC5_ENABLED)
    // If detected device density different from the default - have to update JEDEC parameters
    if (mc5GlobalData.deviceDensity != MC5_SINGLE_DEVICE_DENSITY_DEFAULT)
    {
        Mc5_SetRegJedecParams(mc5GlobalData.dramFrequencyMHz, TRUE);
        settingChanged = TRUE;
    }
    #endif
    #endif

    if (!mc5GlobalData.isCs1Enabled)  // CS1 not enabled
    {
        rDdrMc.chan0MmapCs1.all = MC5_CS1_NON_VALID_VALUE;
    }

    if (settingChanged)
    {
        // Perform init sequence again in case something changed
        Mc5_InitDdrSequence(isLpddr, FALSE);
    }
}

//-----------------------------------------------------------------------------
/**
    Mc5_OneTimeInit -
    Initialization of memory, first of all basic init and then auto detection
    of different values, pad calibration and dll tune.
**/
//-----------------------------------------------------------------------------
static void Mc5_OneTimeInit(U32 frequencyMT, bool isLpddr)
{
    #if CHK_CODE_TYPE(CODE_BOOT) || CHK_FW_TYPE(FW_DNLD)

        // Set PLLA
        //Mc5_SetPllA(frequencyMT);

        // Data prepare
        Mc5_InitGlobalData(frequencyMT, isLpddr, FALSE);

        // HwInfo data prepare
        Mc5_InitHwInfoData();

        // Phy operation
        Mc5_InitDdrBasic(mc5GlobalData.dramFrequencyMHz, isLpddr, FALSE);

        #if (MC5_PAD_CALIBRATION == MC5_ENABLED)
        // Calibrate the PAD driver and termination PMOS, NMOS minidrivers
        Mc5_PadCalibration(isLpddr, FALSE);
        #endif

        // AutoDetectRoutine
        Mc5_AutoDetectRoutine(isLpddr);

        #if (MC5_DLL_TUNING == MC5_ENABLED)
        // DLL calibration to ensure balanced setup, hold margins for read data.
        //HalDdrDll_Sweep(MC5_MAX_SUPPORT_BYTE, FALSE, FALSE);
        HalDdrDll_Tune(MC5_MAX_SUPPORT_BYTE);
        #endif

    #else
        D_PRINTF("-[MC5] QuickInit\n");

        // Data prepare
        Mc5_InitGlobalData(frequencyMT, isLpddr, TRUE);

        // HwInfo data prepare
        Mc5_InitHwInfoData();

        Mc5_StoreRestoreRegMemAddr(cStoreReg);
        Mc5_StoreRestoreRegJedec(cStoreReg);
        Mc5_StoreRestoreRegOdt(cStoreReg);

        #if (MC5_PAD_CALIBRATION == MC5_ENABLED)
        Mc5_StoreRegPadCal();
        #endif

    #endif
}

//-----------------------------------------------------------------------------
/**
    Mc5_Recovery
**/
//-----------------------------------------------------------------------------
static void Mc5_Recovery(U32 frequencyMT, bool isLpddr)
{
    D_PRINTF("-[MC5] Recovery\n");

    Mc5_InitDdrBasic(frequencyMT, isLpddr, TRUE);

    #if (MC5_PAD_CALIBRATION == MC5_ENABLED)
    // Calibrate the PAD driver and termination PMOS, NMOS minidrivers
    //Mc5_PadCalibration(isLpddr, FALSE);
    Mc5_SetRegPadCal(isLpddr, 0, 0);
    #endif
}

//-----------------------------------------------------------------------------
/**
    Dump MC5 information
**/
//-----------------------------------------------------------------------------
static void Mc5_DumpInfo(void)
{
    D_PRINTF("-[MC5] DumpInfo\n");
    D_PRINTF("-[MC5] %x|%x|%2x|%d|%d\n", mc5GlobalData.memSizeConfig,
                                         mc5GlobalData.boundaryTable,
                                         mc5GlobalData.dramType,
                                         mc5GlobalData.isLpddr,
                                         mc5GlobalData.isCs1Enabled);
    D_PRINTF("-[MC5] %d|%d|%d|%d\n",     mc5GlobalData.totalSize     >> 20,
                                         mc5GlobalData.deviceDensity >> 20,
                                         mc5GlobalData.frequencyMT,
                                         mc5GlobalData.dramFrequencyMHz);
}

//-----------------------------------------------------------------------------
/**
    Dump MC5 registers
**/
//-----------------------------------------------------------------------------
static void Mc5_DumpRegs(void)
{
    D_PRINTF("-[MC5] DumpRegs\n");
    Debug_DumpMemU32((U32)&mc5RegForRecoveryOdt,   sizeof(mc5RegForRecoveryOdt));
    Debug_DumpMemU32((U32)&mc5RegForRecoverySize,  sizeof(mc5RegForRecoverySize));
    Debug_DumpMemU32((U32)&mc5RegForRecoveryJedec, sizeof(mc5RegForRecoveryJedec));
    D_PRINTF("\n");
    Debug_DumpMemU32(MC5_BASE, 0x087C); // MC5 last register: 0xF100087C
}

//-----------------------------------------------------------------------------
/**
    Mc5_UpdateDramInfo
**/
//-----------------------------------------------------------------------------
static void Mc5_UpdateDramInfo(void)
{
    #if (MC5_AUTODETECT_CS == MC5_ENABLED)
    // KevinKCChiu: Since 2CS is forced on
    if (halDramInfo.numberOfCs != (mc5GlobalData.isCs1Enabled + 1))
    {
        D_PRINTF("-[MC5] ChangeCs %d|%d\n", halDramInfo.numberOfCs,
                                            mc5GlobalData.isCs1Enabled + 1);
        halDramInfo.numberOfCs = mc5GlobalData.isCs1Enabled + 1;
    }
    #endif

    #if (MC5_AUTODETECT_MEMORY_SIZE == MC5_ENABLED)
    if (halDramInfo.size != mc5GlobalData.totalSize)
    {
        D_PRINTF("-[MC5] ChangeSize %d|%d\n", halDramInfo.size        >> 20,
                                              mc5GlobalData.totalSize >> 20);
        halDramInfo.size = mc5GlobalData.totalSize;
    }
    #endif
}

//-----------------------------------------------------------------------------
/**
    Mc5_Lpddr3MrRead
**/
//-----------------------------------------------------------------------------
static void Mc5_Lpddr3MrRead(void)
{
    DdrmcUserInitiateCmd1_t ddrmcUserInitiateCmd1;
    U8 csToRead;
    U8 byteToRead;
    U8 mrIndex;

    D_PRINTF("-[MRR] Init|%8x\n", rDdrMc.chan0ModeRdData.all);

    for (csToRead = 0; csToRead < MC5_NUM_OF_CS; csToRead++)
    {
        for (byteToRead = 0; byteToRead < MC5_MAX_SUPPORT_BYTE; byteToRead++)
        {
            rDdrMc.chan0DramCfg2.all             = MC5_SET_TO_ZERO;
            rDdrMc.chan0DramCfg2.b.MRR_DATA_BYTE = byteToRead;

            D_PRINTF("-[MRR] Cs%d|B%d|0x%8x\n", csToRead, byteToRead, rDdrMc.chan0DramCfg2.all);

            for (mrIndex = 4; mrIndex <= 8; mrIndex++)
            {
                ddrmcUserInitiateCmd1.all                  = MC5_SET_TO_ZERO;
                ddrmcUserInitiateCmd1.b.MR_ADDR            = mrIndex;
                ddrmcUserInitiateCmd1.b.DDRMC_MRR          = MC5_ENABLED;
                if      (csToRead == MC5_CS0)
                    ddrmcUserInitiateCmd1.b.DDRMC_CS0_CMD1 = MC5_ENABLED;
                else if (csToRead == MC5_CS1)
                    ddrmcUserInitiateCmd1.b.DDRMC_CS1_CMD1 = MC5_ENABLED;
                ddrmcUserInitiateCmd1.b.DDRMC_CH0_CMD1     = MC5_ENABLED;

                rDdrMc.ddrmcUserInitiateCmd1.all = ddrmcUserInitiateCmd1.all;
                HalTimer_DelayUsec(1);

                while (rDdrMc.chan0ModeRdData.b.MRR_DATA_VALID == 0);

                D_PRINTF("-[MRR] Cs%d|B%d|Mr%d|%2x\n", csToRead,
                                                       byteToRead,
                                                       mrIndex,
                                                       rDdrMc.chan0ModeRdData.b.MRR_DATA);
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    HalMc5_OneTimeInit
**/
//-----------------------------------------------------------------------------
Error_t HalMc5_OneTimeInit(U32 frequencyMT)
{
    HwTimerTick_t timeTick;

    timeTick = HalTimer_ReadTick();

    Mc5_OneTimeInit(frequencyMT, MC5_IS_LPDDR);
    Mc5_UpdateDramInfo();

    D_PRINTF("-[MC5] InitUs %u\n", HalTimer_GetElapsedTimeUsec(timeTick));
    //Mc5_DumpInfo();
    //Mc5_DumpRegs();
    //Debug_DumpMemU32((U32)&mc5GlobalData, sizeof(mc5GlobalData));
    //Mc5_Lpddr3MrRead();

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    HalMc5_Recovery
**/
//-----------------------------------------------------------------------------
Error_t HalMc5_Recovery(U32 frequencyMT)
{
    Mc5_Recovery(frequencyMT, MC5_IS_LPDDR);
    return cEcNoError;
}
//-----------------------------------------------------------------------------
/**
    HalMc5_PadCalibrationRoutine
**/
//-----------------------------------------------------------------------------
Error_t HalMc5_PadCalibrationRoutine(U8 targetLoop)
{
    #if CHK_FW_TYPE(FW_AGING)
    U32 phyCalLsbCnt, phyCalMsbCnt;
    U8  phyCalLsbOri, phyCalMsbOri;
    U8  loop, passCount;

    if (targetLoop < PAD_CAL_PASS_THRESHOLD)
    {
        return cEcError;
    }

    phyCalLsbCnt = 0;
    phyCalMsbCnt = 0;
    loop         = 0;
    passCount    = 0;
    phyCalLsbOri = mc5HwInfoData.phyCalData.phyCalLsb;
    phyCalMsbOri = mc5HwInfoData.phyCalData.phyCalMsb;

    while ((loop < targetLoop) && (passCount < PAD_CAL_PASS_THRESHOLD))
    {
        Mc5_PadCalibration(MC5_IS_LPDDR, TRUE);
        if (mc5HwInfoData.phyCalData.phyCalValid == TRUE)
        {
            #if (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
            if ((mc5HwInfoData.phyCalData.phyCalLsb <= (PAD_CAL_LSB_DEFAULT + PAD_CAL_DEFAULT_INTERVAL)) &&
                (mc5HwInfoData.phyCalData.phyCalLsb >= (PAD_CAL_LSB_DEFAULT - PAD_CAL_DEFAULT_INTERVAL)) &&
                (mc5HwInfoData.phyCalData.phyCalMsb == (PAD_CAL_MSB_DEFAULT)))
            #endif
            {
                phyCalLsbCnt += mc5HwInfoData.phyCalData.phyCalLsb;
                phyCalMsbCnt += mc5HwInfoData.phyCalData.phyCalMsb;
                passCount++;
            }
        }
        loop++;
    }

    if (passCount == PAD_CAL_PASS_THRESHOLD)
    {
        smHwInfo.d.dramCal.tag       = PAD_CAL_TAG_VAILD;
        smHwInfo.d.dramCal.phyCalLsb = (phyCalLsbCnt / PAD_CAL_PASS_THRESHOLD);
        smHwInfo.d.dramCal.phyCalMsb = (phyCalMsbCnt / PAD_CAL_PASS_THRESHOLD);
        smHwInfo.d.dramCal.checkSum  = MemSumU8((const U8*)&smHwInfo.d.dramCal.phyCalLsb, 2);
        // force update average value if pass
        mc5HwInfoData.phyCalData.phyCalLsb = smHwInfo.d.dramCal.phyCalLsb;
        mc5HwInfoData.phyCalData.phyCalMsb = smHwInfo.d.dramCal.phyCalMsb;
    }
    else
    {
        smHwInfo.d.dramCal.tag       = PAD_CAL_TAG_FAIL;
        smHwInfo.d.dramCal.phyCalLsb = mc5HwInfoData.phyCalData.phyCalLsb;
        smHwInfo.d.dramCal.phyCalMsb = mc5HwInfoData.phyCalData.phyCalMsb;
        smHwInfo.d.dramCal.checkSum  = MemSumU8((const U8*)&smHwInfo.d.dramCal.phyCalLsb, 2);
        // restore
        mc5HwInfoData.phyCalData.phyCalLsb = phyCalLsbOri;
        mc5HwInfoData.phyCalData.phyCalMsb = phyCalMsbOri;
    }

    Mc5_SetRegPadCal(MC5_IS_LPDDR, 0, 0);
    #endif
    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    HalMc5_DllResetAndUpdate
**/
//-----------------------------------------------------------------------------
Error_t HalMc5_DllResetAndUpdate(void)
{
    Mc5_PhyDllResetAndUpdate();

    return cEcNoError;
}

/* End of File */


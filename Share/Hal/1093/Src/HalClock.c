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
//! @brief Clock and TBG(Time Base Generator) HW control codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "HalClock.h"
#include "HalCpu.h"
#include "HalUart.h"
#include "HalRegConfig.h"
#include "ClockTree.h"
#include "RegClockPLL.h"
#include "HalPm.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
/// @brief On & Off Power Off mode
typedef enum
{
    cModeEnter=1,       ///< Enter Power Off mode
    cModeExit           ///< Exit Power Off mode
} PllPowerOffMode_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define GET_TBG_FREQ(Fref, M, N, DIV)   ((4 * (Fref) * (N))/ (M) / (DIV))

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief TBG clock setting table
typedef struct
{
    uint32_t BW_SEL:1;
    uint32_t REFDIV:8;
    uint32_t FBDIV:8;
    uint32_t ICP:4;
} TbgClockTable_t;

/// @brief registers
typedef struct
{
    uint32_t    clkSelectioCtrl;        ///< in rClockPll
    uint32_t    clkEnCtrl;              ///< in rClockPll
} SaveClockReg_t;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//
//  VARIABLE DECLARATION WITH INITIAL VALUE DOESN'T WORK !!!
//  UNINTIALIZED VARIABLE DECLARATION ONLY !!!
//-----------------------------------------------------------------------------
#pragma arm section rwdata = "data_fast_ui"
#pragma arm section zidata = "data_fast_ui"

// Fout = 4 x OSC * FBDIV / REFDIV
static const TbgClockTable_t tbgClockTable[] =
{   // BW_SEL, REFDIV, FBDIV, ICP     40Mhz OSC           N=1..127
    {       0,      3,    20, 4 }, // 1066, 533, ... 1066/N
    {       0,      3,    25, 4 }, // 1333, 666, ... 1333/N
    {       1,      2,    20, 9 }, // 1600, 800, ... 1600/N
    {       0,      3,    35, 3 }, // 1866, 933, ... 1800/N
    {       1,      2,    25, 9 }, // 2000,1000, ... 2000/N
    {       0,      3,    40, 4 }, // 2133,1066, ... 2133/N
    /*
    {       0,      4,    26, 5 }, // 1040, 520, ... 1040/N
    {       0,      3,    20, 4 }, // 1066, 533, ... 1066/N
    {       1,      2,    15, 9 }, // 1200, 800, ... 1200/N
    {       0,      3,    25, 4 }, // 1333, 666, ... 1333/N
    {       1,      2,    18, 9 }, // 1440, 720, ... 1440/N
    {       1,      2,    19, 9 }, // 1520, 760, ... 1520/N
    {       1,      2,    20, 9 }, // 1600, 800, ... 1600/N
    {       0,      3,    35, 3 }, // 1866, 933, ... 1800/N
    {       1,      2,    25, 9 }, // 2000,1000, ... 2000/N
    */
};

// clock frequency
static const uint16_t clockFreq[] =
{
    OSC_FREQ_MHZ,
    TBG_A_FREQ_MHZ,
    TBG_B_FREQ_MHZ,
    TBG_C_FREQ_MHZ,
    TBG_D_FREQ_MHZ,
};
static SaveClockReg_t saveClockReg;
volatile ClockPowerMode_t currentClockPowerMode;

// ---------- Register 0xF0112090 const values ----------
// The const value for the reserved mask bits
static const ClkEnCtrl_t cClkEnCtrlReservedMask =
{
    .b.RF0112090_3_RSVD     = 1,       // BIT [3] Reserved.
    .b.PCIE_BUTTON_RESET    = 1,       // BIT [4] PCIE_BUTTON_RESET
    .b.RF0112090_8_5_RSVD   = 0xF,     // BIT [8:5] Reserved.
    .b.RF0112090_13_RSVD    = 1,       // BIT [13] Reserved.
    .b.RF0112090_15_RSVD    = 1,       // BIT [15] Reserved.
    .b.RF0112090_16_RSVD    = 1,       // BIT [16] Reserved.
    .b.RF0112090_19_RSVD    = 1,       // BIT [19] Reserved.
    .b.RF0112090_28_RSVD    = 1        // BIT [28] Reserved.
};

// The setting value for the sleep partial mode
static const ClkEnCtrl_t cClkEnCtrlSettingForSleepPartial =
{
    .b.SYSAXI_CLK_EN        = 1,       // BIT [ 9] SYSAXI_CLK Enable or Disable.
    .b.PCIAXI_CLK_EN        = 1,       // BIT [10] PCIAXI_CLK Enable or Disable.
                                       //          If need more power reduction at L1.x, turn off this clock and exit S2 by PCIe interrupt of L1/L0 transition.
    .b.PDBG_CLK_EN          = 1,       // BIT [18] Processor Debug Clock Enable or Disable.
    .b.AMBA_CLK_EN          = 1,       // BIT [20] AMBA_CLK Enable or Disable.
    .b.CPU0_CLK_EN          = 1,       // BIT [26] CPU0 Clock Enable.
    .b.MC5_CLK_EN           = 1,       // BIT [27] MC5 Clock Enable
    .b.RFSH_CLK_EN          = 1,       // BIT [30] RFSH_CLK Enable
    .b.OSC_CLK_EN           = 1        // BIT [31] OSC Clock Enable or Disable.
};

// The setting value for the sleep full mode
static const ClkEnCtrl_t cClkEnCtrlSettingForSleepFull =
{
    .b.SYSAXI_CLK_EN        = 1,       // BIT [ 9] SYSAXI_CLK Enable or Disable.
    .b.PCIAXI_CLK_EN        = 1,       // BIT [10] PCIAXI_CLK Enable or Disable.
                                       //          If need more power reduction at L1.x, turn off this clock and exit S2 by PCIe interrupt of L1/L0 transition.
    .b.PDBG_CLK_EN          = 1,       // BIT [18] Processor Debug Clock Enable or Disable.
    .b.AMBA_CLK_EN          = 1,       // BIT [20] AMBA_CLK Enable or Disable.
    .b.CPU0_CLK_EN          = 1,       // BIT [26] CPU0 Clock Enable.
    .b.OSC_CLK_EN           = 1        // BIT [31] OSC Clock Enable or Disable.
};

// The setting value for the deep sleep mode
static const ClkEnCtrl_t cClkEnCtrlSettingForDeepSleep =
{
    .b.SYSAXI_CLK_EN        = 1,       // BIT [ 9] SYSAXI_CLK Enable or Disable.
    .b.AMBA_CLK_EN          = 1,       // BIT [20] AMBA_CLK Enable or Disable.
    .b.CPU0_CLK_EN          = 1,       // BIT [26] CPU0 Clock Enable.
    .b.OSC_CLK_EN           = 1        // BIT [31] OSC Clock Enable or Disable.
};
// ------------------------------------------------------

// ---------- Register 0xF0112094 const values ----------
// The const value for the reserved mask bits
static const ClkSelectioCtrl_t cClkSelCtrlReservedMask =
{
    .b.RF0112094_3_RSVD     = 1,       // BIT [ 3] Reserved
    .b.RF0112094_7_RSVD     = 1,       // BIT [ 7] Reserved
    .b.RF0112094_15_11_RSVD = 0x1F,    // BIT [15:11] Reserved.
    .b.RF0112094_19_17_RSVD = 0x7      // BIT [19:17] Reserved.
};

// The setting value for the sleep partial mode
static const ClkSelectioCtrl_t cClkSelCtrlSettingForSleepPartial =
{
    .b.CLK_SEL_SYSAXI_OSC   = 1,       // BIT [25] Clock Selection SYSAXI.
    .b.CLK_SEL_PCIAXI_OSC   = 1,       // BIT [26] Clock Selection PCIe AXI.
    .b.CLK_SEL_ECC_OSC      = 1,       // BIT [27] Clock Selection ECC.
    .b.CLK_SEL_PLLC_OSC     = 1,       // BIT [28] Clock Selection PLL C.
    .b.CLK_SEL_PLLB_OSC     = 1        // BIT [29] Clock Selection PLL B.
};

// The setting value for the sleep full mode
static const ClkSelectioCtrl_t cClkSelCtrlSettingForSleepFull =
{
    .b.CLK_SEL_SYSAXI_OSC   = 1,       // BIT [25] Clock Selection SYSAXI.
    .b.CLK_SEL_PCIAXI_OSC   = 1,       // BIT [26] Clock Selection PCIe AXI.
    .b.CLK_SEL_ECC_OSC      = 1,       // BIT [27] Clock Selection ECC.
    .b.CLK_SEL_PLLC_OSC     = 1,       // BIT [28] Clock Selection PLL C.
    .b.CLK_SEL_PLLB_OSC     = 1,       // BIT [29] Clock Selection PLL B.
    .b.CLK_SEL_PLLA_OSC     = 1        // BIT [30] Clock Selection PLL A.
};

// The setting value for the deep sleep mode
static const ClkSelectioCtrl_t cClkSelCtrlSettingForDeepSleep =
{
    .b.CLK_SEL_SYSAXI_OSC   = 1,       // BIT [25] Clock Selection SYSAXI.
    .b.CLK_SEL_PCIAXI_OSC   = 1,       // BIT [26] Clock Selection PCIe AXI.
    .b.CLK_SEL_ECC_OSC      = 1,       // BIT [27] Clock Selection ECC.
    .b.CLK_SEL_PLLC_OSC     = 1,       // BIT [28] Clock Selection PLL C.
    .b.CLK_SEL_PLLB_OSC     = 1,       // BIT [29] Clock Selection PLL B.
    .b.CLK_SEL_PLLA_OSC     = 1        // BIT [30] Clock Selection PLL A.
    //.b.CLK_SEL_OSC_1M     = 1        //// BIT [31] Clock Selection OSC or 1M.      <<< If enable, drive doesn't wake-up
};

// The setting value for the 1 Mhz
static const ClkSelectioCtrl_t cClkSelCtrlSettingFor1MhzB20 =
{
    .b.CLK_SEL_PHY_AON          = 1,       // BIT [20] Clock Selection Between 100MHzCLK and AON.
};

static const ClkSelectioCtrl_t cClkSelCtrlSettingFor1MhzB23B31 =
{
    .b.CLK_SEL_PCISYAXI_OSC_AON = 1,       // BIT [23] Clock Selection PCISYSAXI.
    .b.CLK_SEL_OSC_1M           = 1        // BIT [31] Clock Selection OSC or 1M.
};

// ------------------------------------------------------
//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
static Error_t HalClock_SaveContext(void);
static Error_t HalClock_RestoreContextForSleepMode(void);
static Error_t HalClock_RestoreContextForDeepPowerDown(void);
static void HalClock_EnterDeepPowerDownSequence(void);
static void HalClock_ExitDeepPowerDownSequence(void);
static Error_t HalClock_SleepMode(PllPowerOffMode_t PowerMode, bool isFullDown);
static Error_t HalClock_PllBlockOff(PllPowerOffMode_t PowDownMode, bool isFullDown);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
#if _CPU_==CPU_ID0    // CPU0 only
//-----------------------------------------------------------------------------
/**
    One-time initialization of Clock H/W.\n

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalClock_OneTimeInit(InitBootMode_t initMode)
{
    if (initMode != cInitBootCpu0Boot)
    {
      #if (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED)
        // Always 1MHz for PCIe OSC. It's for power reduction at S3.
        rClockPll.powerCtrl2.b.SEL_OSC0_1M1 = 1;               // 0xF011307C[27]
      #endif

        HalClock_SetFrequency(cClockTbgA, TBG_A_FREQ_MHZ); // set MC5
        HalClock_SetFrequency(cClockTbgB, TBG_B_FREQ_MHZ); // set CPU
        HalClock_SetFrequency(cClockTbgC, TBG_C_FREQ_MHZ); // set FLASH
        HalClock_SetFrequency(cClockTbgD, TBG_D_FREQ_MHZ); // set ECU


        // for DRAM log
        D_PRINTF("[TBGA] %dMHz\n", GET_TBG_FREQ(HalClock_GetOscFrequency(),
                                   rClockPll.pllaCtrl3.b.PLLA_REFDIV,
                                   rClockPll.pllaCtrl1.b.PLLA_FBDIV,
                                   rClockPll.pllaCtrl1.b.PLLA_CLKOUT_SE_DIV_SEL));
    }

    currentClockPowerMode = cClockPmActive;

    return cEcNoError;
}
#endif

#if (CHK_CODE_TYPE(CODE_MAIN) && _CPU_ == CPU_ID0)
#if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
//-----------------------------------------------------------------------------
/**
    H/W block level power management API

    @param[in]  powerMode        targeting pm mode to enter
    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalClock_SetPowerMode(ClockPowerMode_t powerMode)
{
    bool isFullDown;

    // no set up if power mode is not changed.
    if (currentClockPowerMode == powerMode) return cEcNoError;

    switch(powerMode)
    {
    case cClockPmDeepSleepAnalogOn:
    case cClockPmDeepSleep:
        switch(currentClockPowerMode)
        {
        case cClockPmActive:
            currentClockPowerMode = powerMode;
            HalClock_EnterDeepPowerDownSequence();
            break;
        case cClockPmPartialSleep:
        case cClockPmSleep:
            HALT(); // Does not support
            break;
        }
        break;

    case cClockPmPartialSleep:
    case cClockPmSleep:
        switch(currentClockPowerMode)
        {
        case cClockPmActive:
            currentClockPowerMode = powerMode;
            HalClock_SleepMode(cModeEnter, (powerMode == cClockPmSleep));
            break;
        case cClockPmDeepSleepAnalogOn:
        case cClockPmDeepSleep:
            HALT(); // Does not support
            break;
        }
        break;

    case cClockPmActive:
        switch(currentClockPowerMode)
        {
        case cClockPmPartialSleep:
        case cClockPmSleep:
            isFullDown = (currentClockPowerMode == cClockPmSleep);
            currentClockPowerMode = powerMode;
            HalClock_SleepMode(cModeExit, isFullDown);
            break;
        case cClockPmDeepSleepAnalogOn:
        case cClockPmDeepSleep:
            currentClockPowerMode = powerMode;
            HalClock_ExitDeepPowerDownSequence();
            break;
        }
        break;

    default:
        break;
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Clock control sequence to enter the deep power down mode
    @param[in]  isAnalogOn      TRUE if Analog block should be turned on
    @return     none
**/
//-----------------------------------------------------------------------------
static void HalClock_EnterDeepPowerDownSequence(void)
{
    ClkSelectioCtrl_t   clkSelCtrl;
    ClkEnCtrl_t         clkEnCtrl;

    HalClock_SaveContext();

    rClockPll.powerCtrl2.b.CLK_REQ_N_RDET_INT = 1;          // 0xF011307C[29] CLK_REQ_N_RDET Interrupt.

    // Hold the default value to some reserved bits(Register 0xF0112094).
    clkSelCtrl.all                  = (rClockPll.clkSelectioCtrl.all & cClkSelCtrlReservedMask.all);
    clkSelCtrl.all                 |= cClkSelCtrlSettingForDeepSleep.all;
    rClockPll.clkSelectioCtrl       = clkSelCtrl;

    rClockPll.analogCtrl1.b.SEL_CLKDIG_DIV1 = 1;            // 0xF0112000[29:28] Select CLKOUT_DIG Clock Division Ratio 1. (1h: Divided by 2)     <<< If enable, drive doesn't wake-up

    HalClock_PllBlockOff(cModeEnter, true);

    #if 0
    clkEnCtrl.all           = (rClockPll.clkEnCtrl.all & cClkEnCtrlReservedMask.all);
    clkEnCtrl.all          |= cClkEnCtrlSettingForDeepSleep.all;
    rClockPll.clkEnCtrl     = clkEnCtrl;
    #endif

    rClockPll.WSIGN = HalPm_WakeupDeepPowerDownMode();      // set up the wake up sign (Func() addr)

    rClockPll.powerCtrl2.b.ANAGRP_PU   = 0;                  // 0xF011307C[30]: =0 Set Analog Group Power Down.

    // After Change to OSC_CLK (RF011_2094h[29] := 1) and power Down PLL, Prepare to switch PCIEPHY clock.
    rClockPll.clkSelectioCtrl.all = 0x7E400000;             // 0xF0112094

    rClockPll.powerCtrl2.b.SEL_NOA_FCT = 0;                 // 0xF011307C[28]: Set to Deep Power Down mode

    //Right before going to power down, switch to 1MHz clock by writing 1 into RF011_2094h[31].
    rClockPll.clkSelectioCtrl.all = 0xFEC00000;             // 0xF0112094

    rClockPll.powerCtrl2.b.NOA_PD_REQ  = 1;                 // 0xF011307C[31]: NOA Power Down Request

    HalCpu_WaitForInterrupt();
}

//-----------------------------------------------------------------------------
/**
    Clock control sequence to exit the deep power down mode
    @param[in]  none
    @return     none
**/
//-----------------------------------------------------------------------------
static void HalClock_ExitDeepPowerDownSequence(void)
{
    rClockPll.analogCtrl1.b.SEL_CLKDIG_DIV1 = 0;            // 0xF0112000[29:28] Select CLKOUT_DIG Clock Division Ratio 1. (1h: Divided by 2)
    rClockPll.powerCtrl2.b.NOA_PD_REQ = 0;                  // 0xF011307C[31]: NOA Power Down Request

    HalClock_PllBlockOff(cModeExit, true);

    HalClock_RestoreContextForDeepPowerDown();

    rClockPll.WSIGN = 0;                        // clear the wake up sign
}

//-----------------------------------------------------------------------------
/**
    Save the Clock setting register valaue

    @param[in]  none
    @return     Error_t
**/
//-----------------------------------------------------------------------------
static Error_t HalClock_SaveContext(void)
{
    saveClockReg.clkSelectioCtrl    = rClockPll.clkSelectioCtrl.all;
    saveClockReg.clkEnCtrl          = rClockPll.clkEnCtrl.all;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Save the Clock setting register valaue

    @param[in]  none
    @return     Error_t
**/
//-----------------------------------------------------------------------------
static Error_t HalClock_RestoreContextForSleepMode(void)
{
    rClockPll.clkSelectioCtrl.all = saveClockReg.clkSelectioCtrl;                 // Restore 0xF0112094

    rClockPll.clkEnCtrl.all       = saveClockReg.clkEnCtrl;                       // Restore 0xF0112090

    rClockPll.clkEnCtrl.b.PCIAXI_CLK_EN = 1;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Restore the Clock setting register valaue for Deep Power Down (S3)

    @param[in]  none
    @return     Error_t
**/
//-----------------------------------------------------------------------------
static Error_t HalClock_RestoreContextForDeepPowerDown(void)
{
    rClockPll.clkSelectioCtrl.all = 0x7E400000;
    rClockPll.clkSelectioCtrl.all = 0x7E400262;
    rClockPll.clkSelectioCtrl.all = saveClockReg.clkSelectioCtrl;                 // Restore 0xF0112094

    rClockPll.clkEnCtrl.all       = saveClockReg.clkEnCtrl;                       // Restore 0xF0112090

    rClockPll.clkEnCtrl.b.PCIAXI_CLK_EN = 1;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Pll clock swith to/from Osc

    @param[in]  PowDownMode            Power down mode
    @param[in]  isFullDown             true if all PLL turn off

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t HalClock_SleepMode(PllPowerOffMode_t PowerMode, bool isFullDown)
{
    ClkSelectioCtrl_t   clkSelectioCtrl;
    ClkEnCtrl_t         ClkEnCtrl;

    if (PowerMode == cModeEnter)
    {
        HalClock_SaveContext();

//        rCoutTimers.cntr0Ctrl.b.CNTR_0_EN = 0;                                                  // Counter 0 Disable

        clkSelectioCtrl.all                  = (rClockPll.clkSelectioCtrl.all & cClkSelCtrlReservedMask.all);
        //clkSelectioCtrl.all                  = 0;
//        clkSelectioCtrl.b.CLK_SEL_OSC_1M     = 1;       // 0xF0112094[31] Clock Selection OSC or 1M.      <<< If enable, Uart doesn't work.
        if (isFullDown)     /// If not full, keep PLLA since DDR is not in SRF
        {
            clkSelectioCtrl.all             |= cClkSelCtrlSettingForSleepFull.all;
        }
        else
        {
            clkSelectioCtrl.all             |= cClkSelCtrlSettingForSleepPartial.all;
        }
        rClockPll.clkSelectioCtrl = clkSelectioCtrl;

        #if 0
        ClkEnCtrl.all           = (rClockPll.clkEnCtrl.all & cClkEnCtrlReservedMask.all);
        if (isFullDown)
        {
            ClkEnCtrl.all      |= cClkEnCtrlSettingForSleepFull.all;
        }
        else
        {
            ClkEnCtrl.all      |= cClkEnCtrlSettingForSleepPartial.all;
        }
        rClockPll.clkEnCtrl     = ClkEnCtrl;
        #endif

        HalClock_PllBlockOff(cModeEnter, isFullDown);
    }
    else    // PowerMode == cModeExit
    {
        HalClock_PllBlockOff(cModeExit, isFullDown);


//        rCoutTimers.cntr0Ctrl.b.CNTR_0_EN = 1;                                                  // Counter 0 Enable

        HalClock_RestoreContextForSleepMode();
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Soc Pll Block power-up and power-down process .\n

    @param[in]  PowDownMode            Power down mode
    @param[in]  isFullDown             true if all PLL turn off

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t HalClock_PllBlockOff(PllPowerOffMode_t PowerMode, bool isFullDown)
{
    uint32_t    delayCnt;
    PllaCtrl3_t pllaCtrl3;
    PllbCtrl3_t pllbCtrl3;
    PllcCtrl3_t pllcCtrl3;
    PlldCtrl3_t plldCtrl3;

    if (PowerMode == cModeEnter)
    {
        if (isFullDown)     /// If not full, keep PLLA since DDR is not in SRF
        {
            rClockPll.pllaCtrl3.b.PLLA_PU = 0;                  // 0xF0112018[26] PLL A Power Up
        }
        rClockPll.pllbCtrl3.b.PLLB_PU = 0;                      // 0xF0112038[26] PLL B Power Up
        rClockPll.pllcCtrl3.b.PLLC_PU = 0;                      // 0xF0112058[26] PLL C Power Up
        rClockPll.plldCtrl3.b.PLLD_PU = 0;                      // 0xF0112078[26] PLL D Power Up
    }
    else    // PowDownMode == cModeExit
    {
        if (isFullDown)
        {
            // Set PLLA PU
            pllaCtrl3 = rClockPll.pllaCtrl3;
            pllaCtrl3.b.PLLA_RST = 1;                               // 0xF0112018[7] PLL A Reset
            rClockPll.pllaCtrl3 = pllaCtrl3;

            pllaCtrl3.b.PLLA_PU = 1;                                // 0xF0112018[26] PLL A Power Up
            rClockPll.pllaCtrl3 = pllaCtrl3;

            pllaCtrl3.b.PLLA_RST = 0;                               // 0xF0112018[7] PLL A Reset
            rClockPll.pllaCtrl3 = pllaCtrl3;
            for (delayCnt = 0; delayCnt < 1; delayCnt++);           // Wait 1us
        }

        // Set PLLB PU
        pllbCtrl3 = rClockPll.pllbCtrl3;
        pllbCtrl3.b.PLLB_RST = 1;                               // 0xF0112038[7] PLL B Reset
        rClockPll.pllbCtrl3 = pllbCtrl3;

        pllbCtrl3.b.PLLB_PU = 1;                                // 0xF0112038[26] PLL B Power Up
        rClockPll.pllbCtrl3 = pllbCtrl3;

        pllbCtrl3.b.PLLB_RST = 0;                               // 0xF0112038[7] PLL B Reset
        rClockPll.pllbCtrl3 = pllbCtrl3;
        for (delayCnt = 0; delayCnt < 1; delayCnt++);           // Wait 1us

        // Set PLLC PU
        pllcCtrl3 = rClockPll.pllcCtrl3;
        pllcCtrl3.b.PLLC_RST = 1;                               // 0xF0112058[7] PLL C Reset
        rClockPll.pllcCtrl3 = pllcCtrl3;

        pllcCtrl3.b.PLLC_PU = 1;                                // 0xF0112058[26] PLL C Power Up
        rClockPll.pllcCtrl3 = pllcCtrl3;

        pllcCtrl3.b.PLLC_RST = 0;                               // 0xF0112058[7] PLL C Reset
        rClockPll.pllcCtrl3 = pllcCtrl3;
        for (delayCnt = 0; delayCnt < 1; delayCnt++);           // Wait 1us

        // Set PLLD PU
        plldCtrl3 = rClockPll.plldCtrl3;
        plldCtrl3.b.PLLD_RST = 1;                               // 0xF0112078[7] PLL D Reset
        rClockPll.plldCtrl3 = plldCtrl3;

        plldCtrl3.b.PLLD_PU = 1;                                // 0xF0112078[26] PLL D Power Up
        rClockPll.plldCtrl3 = plldCtrl3;

        plldCtrl3.b.PLLD_RST = 0;                               // 0xF0112078[7] PLL D Reset
        rClockPll.plldCtrl3 = plldCtrl3;
        for (delayCnt = 0; delayCnt < 500; delayCnt++);         // Wait more than 50us

    #if (_SOC_REV_ >= 0x0210)
        // TODO: Put an additional delay for R2.1 temporarily, or IPC message corruption happens.
        // Remove this temporary code when apply the concrete solution.
        // TBG B Clock / OSC clock = 500 / 40 = 12.5
        // delay 200 usec for locking PLL  (16 = 200 / 12.5)
        HalCpu_DelayUsec(16);
    #endif

        while ((!rClockPll.pllaCtrl1.b.PLLA_LOCK)
            || (!rClockPll.pllbCtrl1.b.PLLB_LOCK)
            || (!rClockPll.pllcCtrl1.b.PLLC_LOCK)
            || (!rClockPll.plldCtrl1.b.PLLD_LOCK))
        {
            for (delayCnt = 0; delayCnt < 85; delayCnt++);      // Wait 10us
        }
    }

    return cEcNoError;
}
#endif
#endif

//-----------------------------------------------------------------------------
/**
    Compute and return frequency TBG register value.

    @param[in]  freqMhz   clock frequency in Mhz to set

    @return HW dependent TBG register setting value
**/
//-----------------------------------------------------------------------------
static uint32_t HalClock_GetFreqDivisor(uint32_t freqMhz, TbgClockTable_t* pTable)
{
    uint32_t indexTbgClock;
    uint32_t divider;
    uint32_t outputFreqMhz;
    uint32_t oscFreqMhz;

    oscFreqMhz = HalClock_GetOscFrequency();

    for (indexTbgClock = 0; indexTbgClock < sizeof(tbgClockTable) / sizeof(TbgClockTable_t); indexTbgClock++)
    {
        for (divider = 1; divider < 128; divider++)
        {
            outputFreqMhz = GET_TBG_FREQ(oscFreqMhz, tbgClockTable[indexTbgClock].REFDIV, tbgClockTable[indexTbgClock].FBDIV, divider);

            if (outputFreqMhz < freqMhz)
            {
                break;
            }

            if (outputFreqMhz == freqMhz)
            {
                //D_PRINTF("outputFreqMhz %d freqMhz %d\n", outputFreqMhz, freqMhz);
                //D_PRINTF("TbgClock %d divider %d\n", indexTbgClock, divider);
                *pTable = tbgClockTable[indexTbgClock];
                return (divider);
            }
        }
    }

    return(0);
}

//-----------------------------------------------------------------------------
/**
    Turn the clock on or off

    @param[in]  clockId     clock Id
    @param[in]  freqMhz     clock frequency in Mhz to set

    @return actual set frequency in Mhz unit. (always less or equal)
**/
//-----------------------------------------------------------------------------
static volatile PllCtrl_t* GetPllRegisterBase(ClockId_t clockId)
{
    // select clock registers
    return (volatile PllCtrl_t *)((uint32_t)&rClockPll.pllaCtrl1 + (clockId - 1) * 0x20);
}

//-----------------------------------------------------------------------------
/**
    Set TBG clock frequency

    @param[in]  clockId     clock Id
    @param[in]  freqMhz     clock frequency in Mhz to set

    @return actual set frequency in Mhz unit. (always less or equal)
**/
//-----------------------------------------------------------------------------
uint32_t HalClock_SetFrequency(ClockId_t clockId, uint32_t freqMhz)
{
    volatile PllCtrl_t* rpPllCtrl;
    TbgClockTable_t clockTable;
    uint32_t divider;

    if (clockId == cClockOsc)
    {
        return HalClock_GetFrequency(clockId);
    }

    rpPllCtrl = GetPllRegisterBase(clockId);

    divider = HalClock_GetFreqDivisor(freqMhz, &clockTable);

    if (divider == 0)
    {
        E_PRINTF(cEcTbgWrongFreq, "Couldn't setup desired frequency %U Mhz, TBG_%c\n\n\n", freqMhz, 'A' + (clockId - 1));

        return(HalClock_GetFrequency(clockId));
    }

    rpPllCtrl->ctrl1.b.PLL_BYPASS_EN = PLL_BYPASS;  // set clock source to OSC
    rpPllCtrl->ctrl3.b.PLL_RST       = 1;

    // setup TBG/PLL
    rpPllCtrl->ctrl1.b.PLL_BW_SEL            = clockTable.BW_SEL;
    rpPllCtrl->ctrl1.b.PLL_FBDIV             = clockTable.FBDIV;
    rpPllCtrl->ctrl1.b.PLL_CLKOUT_SE_DIV_SEL = divider;
    rpPllCtrl->ctrl3.b.PLL_REFDIV            = clockTable.REFDIV;
    rpPllCtrl->ctrl2.b.PLL_ICP               = clockTable.ICP;

    rpPllCtrl->ctrl3.b.PLL_RST = 0;

    HalCpu_DelayUsec(100);  // delay 100 usec for locking PLL

    rpPllCtrl->ctrl1.b.PLL_BYPASS_EN = PLL_CLOCK;  // set clock source to PLL

#if _CPU_ == CPU_ID0          // CPU0 only
    // reconfigure UART clock dividor if UART source clock was changed.
    if (clockId == cClockTbgB)
    {
        HalUart_SetBaudRate(0, UART_BAUD_RATE);
        //HalUart_SetBaudRate(1, UART_BAUD_RATE);
    }
#endif

    return(freqMhz);
}

//-----------------------------------------------------------------------------
/**
    Get Clock frequency in MHZ unit.

    @param[in]  clockId     clock Id

    @return clock frequency in Mhz unit.
**/
//-----------------------------------------------------------------------------
uint32_t HalClock_GetFrequency(ClockId_t clockId)
{
    uint32_t oscFrequency;
    volatile PllCtrl_t* rpPllCtrl;

    oscFrequency = HalClock_GetOscFrequency();

    if (clockId==cClockOsc)
    {
        return(oscFrequency);
    }

    if ((rpPllCtrl = GetPllRegisterBase(clockId)) == NULL)
    {
        return 0;
    }

    if (rpPllCtrl->ctrl1.b.PLL_BYPASS_EN)
    {
        return(oscFrequency);
    }

    return GET_TBG_FREQ(oscFrequency, rpPllCtrl->ctrl3.b.PLL_REFDIV, rpPllCtrl->ctrl1.b.PLL_FBDIV, rpPllCtrl->ctrl1.b.PLL_CLKOUT_SE_DIV_SEL);
}

//-----------------------------------------------------------------------------
/**
    Turn the clock on or off

    @param[in]  clockId     clock identifier
    @param[in]  onOff       on or off

    @return   previous status
**/
//-----------------------------------------------------------------------------
bool HalClock_TurnOnOff(ClockId_t clockId, bool onOff)
{
    //Not implemented yet.

    return ON;
}

//-----------------------------------------------------------------------------
/**
    set SSC amount

    @param[in]  clockId     clock identifier
    @param[in]  sscAmount   ssc ppm value

    @return   Actual set ppm value
**/
//-----------------------------------------------------------------------------
uint32_t HalClock_SetSsc(ClockId_t clockId, uint32_t sscAmount)
{
    //Not implemented yet.

    return 0;
}

//-----------------------------------------------------------------------------
/**
    Turn the SSC on or off

    @param[in]  clockId     clock identifier
    @param[in]  onOff       on or off

    @return   previous status
**/
//-----------------------------------------------------------------------------
bool HalClock_SscOnOff(ClockId_t clockId, bool onOff)
{
    //Not implemented yet.

    return OFF;
}

//-----------------------------------------------------------------------------
/**
    Return oscillator clock fequency in MHZ unit\n
    This is not a auto-detection, read latched test-pin jumper
    and return OSC frequency.\n

    @param void.

    @return oscillator clock frequency in MHZ unit
**/
//-----------------------------------------------------------------------------
uint32_t HalClock_GetOscFrequency(void)
{
    return OSC_FREQ_MHZ;
}

//-----------------------------------------------------------------------------
/**
    Return PLL control address.\n

    @param clock index.

    @return address of PLL control based on clock index
**/
//-----------------------------------------------------------------------------
volatile PllCtrl_t* HalClock_GetPllRegisterBase(ClockId_t clockId)
{
    // select clock registers
    return (volatile PllCtrl_t *)((U32)&rClockPll.pllaCtrl1 + (clockId - 1) * 0x20);
}

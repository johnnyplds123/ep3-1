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
//! @brief Host I/F Phy layer API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"

// HAL
#include "RegPciePhy.h"
#include "HalIrq.h"
#include "HalPciePhy.h"
#include "HalPcieLink.h"
#include "HalRegConfig.h"
#include "HalIrqSrc.h"
#include "RegIntController.h"
#include "RegArmVic.h"

// APL

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
static const uint32_t cPerstReleaseTimeOutUs = 30*1000*1000;   ///< Perst release

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void ReleasePipeReset(void);
static void ConfigurePcieLanes(void);
static void ConfigurePciePhyPipe(void);
static void EnablePcieBlockReset(void);
//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

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
    One-time initialization for PCIe I/F Phy Layer H/W\n

    @param  initMode    initialization mode

    @return
**/
//-----------------------------------------------------------------------------
Error_t HalPhy_OneTimeInit(InitBootMode_t initMode)
{
    D_PRINTF( "[PCIe] PhyInit\n" );

    HalPhy_Reset(cResetHard);

    return(cEcNoError);
}


//-----------------------------------------------------------------------------
/**
    Configure PCIe Clock

    @param  none
**/
//-----------------------------------------------------------------------------
static void ConfigurePciePhyPipe(void)
{
    // lane to lane bundle
    rPciePhy.clkSrcLow.all = 0x01e7;   // bundle_pll_rdy = 1
    
#if TRY4_SETTING
    REG_U16(0xF00C070C) = 0x33E7 ;//RDT - try#4
#endif
    // Cfg update polarity
    REG_U32(0xF00C06A0) |= BIT(12);

    // PHY operation mode
    rPciePhy.powerAndPllCtrl.b.PHY_MODE = 3;

    // Reference clock frequency
    rPciePhy.powerAndPllCtrl.b.REF_FREF_SEL = 0;

    // Maximum supported data rate
    rPciePhy.syncMskAndMaximalGenSetting.b.PHY_GEN_MAX = 0x2;

    // Core clock output frequency
  #if (PCIE_SRIS == ENABLE)
    //3/17/2015     comment it for using external refclk
    REG_U16(0xF00C013C) |= BIT(10);  //REFCLK_SEL to select Osc clock
  #else
    rPciePhy.miscCtrl0.b.CLK500M_EN = 1;
    rPciePhy.miscCtrl0.b.TXDCLK_2X_SEL = 1;
  #endif

    rPciePhy.rstAndClkCtrl.b.MODE_CORE_CLK_FREQ_SEL = 0;

  #if (PCIE_SRIS == DISABLE)
    // Enable lane alignment
    rPciePhy.laneAlignmenCtrl.all = 0x0B00;
  #endif
#if TRY4_SETTING
    REG_U16(0xF00C0388) = 0x0250 ;//RDT - try#4
#endif
    // Set Tx adapt
    REG_U16(0xF00C026C) = 0x6B93;

    // Enable PIPE4
    REG_U16(0xF00C049C) = 0x00A8;

    // Tx gen3 training preset
    rPciePhy.txTrainingPreset.b.TX_COE_FM_PIN_PCIE3_EN = 0x1;

    // Disable DC wander
    rPciePhy.gen3Setting1.b.RF00C0048_14_RSVD = 0;      // g3_rxdcw_en = 0

  #if (PCIE_SRIS == ENABLE)
    rPciePhy.dtlCtrl.b.DTL_FLOOP_EN = 1;                // Enable DTL Frequency loop: DTL_FLOOP_EN = 1
  #else
    rPciePhy.dtlCtrl.b.DTL_FLOOP_EN = 0;                // DTL_FLOOP_EN = 0
  #endif

    // Gen1 FFE
    rPciePhy.g1Setting3.b.G1_FFE_RES_SEL = 0x7;         // G1_FFE_RES_SEL = 7

    // Gen2 FFE
    rPciePhy.g2Setting3.b.G2_FFE_RES_SEL = 0x7;         // G2_FFE_RES_SEL = 7

    // Gen3 FFE
#define USE_AUTOFFE
#ifdef USE_AUTOFFE
    rPciePhy.g3Setting3.b.G3_FFE_SETTING_FORCE = 0;
#else
    rPciePhy.g3Setting3.b.G3_FFE_SETTING_FORCE = 1;
#endif

    rPciePhy.g3Setting3.b.G3_FFE_RES_SEL = 0x7;
    rPciePhy.g3Setting3.b.G3_FFE_CAP_SEL = 0x4;

    // Gen3 CDR mupf
    rPciePhy.gen3Setting1.b.G3_RX_SELMUPF = 0x2;        // G3_RX_SELMUPF = 1/4

    // Gen3 CDR mupi
    rPciePhy.gen3Setting1.b.G3_RX_SELMUPI = 0x2;        // G3_RX_SELMUPI = 1/4

#if (PCIE_FINE_TUNE_ACERR14 == TRUE)
    REG_U16(0xF00C0438) = 0x1ECB;
    // For R14
    rPciePhy.gen3Setting1.b.G3_RX_SELMUFI = 0x1;
#endif
    // Timer
    rPciePhy.dfeTmr.b.RF00C01AC_8_6_RSVD = 0x1;         // Timer_dfe1 = 1

    REG_U16(0xF00C01AC) = 0x558;

    REG_U16(0xF00C06F4) &= ~BIT(15);                    // cfg_blind_tran_en = 0
#if TRY4_SETTING
    REG_U16(0xF00C0048) = 0x1610 ;//RDT - try#4
#endif
    // DFE adapt
    REG_U16(0xF00C0418) = (REG_U32(0xF00C0418) & (~0xFFC0)) | BIT(6);

    REG_U16(0xF00C0470) &= ~(0xC000);
    REG_U16(0xF00C049C) |= BIT(7);
    rPciePhy.txTrainingCtrl0.b.TX_TRAIN_P2P_HOLD = 0x1;
    REG_U16(0xF00C026C) = (REG_U32(0xF00C026C) & ~BIT(2)) | 0x3;
    REG_U16(0xF00C0438) = (REG_U32(0xF00C0438) & ~BIT(14)) | BIT(15);

    // Tx train
    rPciePhy.txTrainingCtrl4.b.TRX_TRAIN_TIMER = 0x17;
    rPciePhy.txTrainingCtrl4.b.TRX_TRAIN_TIMEOUT_EN = 0;

    // CDR phase
    REG_U16(0xF00C0284) |= 0xC;                         // cdr_phase_step_size = 3.

    // Rx FFE test
    REG_U16(0xF00C024C) &= ~(BIT(14) | BIT(13) | BIT(12));                                      //rx_ffe_test_r4[2:0]=0
    REG_U16(0xF00C024C) = REG_U16(0xF00C024C) & ~(BIT(6) | BIT(5) | BIT(4)) | BIT(5);           //rx_ffe_test_r5[2:0]=2
    REG_U16(0xF00C0250) = REG_U16(0xF00C0250) & ~(BIT(14) | BIT(13) | BIT(12)) | BIT(14);       //rx_ffe_test_r6[2:0]=4
    REG_U16(0xF00C0250) = REG_U16(0xF00C0250) & ~(BIT(6) | BIT(5) | BIT(4)) | BIT(6) | BIT(5);  //rx_ffe_test_r7[2:0]=6
    REG_U16(0xF00C0240) = REG_U16(0xF00C0240) & (~0xF000) | 0x4000;                             //rx_ffe_test_index_init=4

    REG_U16(0xF00C0498) &= ~BIT(3);

    // EQ ctrl
    REG_U16(0xF00C0624) |= BIT(2);

    // Det timeout
    REG_U16(0xF00C0334) &= ~BIT(8);

  #if (PCIE_SRIS == ENABLE)
    // 0xF00C_0004[4:0] = 0x3: select reference clock frequency = 40mhz
    REG_U16(0xF00C0004) = (REG_U16(0xF00C0004) & (~0x1F)) | 0x3;

  #if (PCIE_LIMIT_WIDTH != PCIE_X1)
    REG_U32(0xF00C0124) = 0x2D00;// Phy_Addr: 'h049 enable lane alignment
  #endif

    REG_U32(0xF00C0620) |= BIT(7); //enable SSC
  #endif

    // enhancement       monitor digital test bus      03/10/2015
    // 1. 0xF0h[13:8] = 0Dh, 0xF0h[5:0] = 00h. (SoC address F00C03C0)
    REG_U16(0xF00C03C0) = (REG_U16(0xF00C03C0) & 0xC0C0) | 0xD00;

    /***Fix Wake up from CLKPM stuck at Pre-Detect_Quiet***/
    // 2. RF00C070C[13] = 1     Use pin_lane_align_ready signal
    //rPciePhy.clkSrcLow.b.CFG_USE_LANE_ALIGN_RDY = 1;	//Jimmy, PCIE LINK failure with CodeDate:1609 SOC
    rPciePhy.clkSrcLow.b.CFG_USE_ALIGN_CLK = 1;

    /***Fix Wake up from CLKPM stuck at Pre-Detect_Quiet***/
    //lane_align_phase_diff_thresh = 4  Shaobing 04272015
    REG_U16(0xF00C0514) = (REG_U16(0xF00C0514) | BIT(10)) & ~BIT(11);

    /***Fix Wake up from CLKPM stuck at Pre-Detect_Quiet***/
    //lane_align_ready_window[1:0]=3 Shaobing 5/12/2015
    //REG_U16(0xF00C0514) = (REG_U16(0xF00C0514) | BIT(7));
#if TRY4_SETTING
    REG_U16(0xF00C0514) = 0x54CF ;//RDT  //Try 4
#endif
    /***Fix Wake up from CLKPM stuck at Pre-Detect_Quiet***/
    //lane_align_valid_width_force = 0 Shaobing 5/12/2015
    REG_U16(0xF00C0520) = (REG_U16(0xF00C0520) & ~BIT(13));

    /***Fix Wake up from CLKPM stuck at Pre-Detect_Quiet***/
    //pll_clk_ready_time[2:0] = 7, shaobing 5/12/2015
    REG_U16(0xF00C03C8) = REG_U16(0xF00C03C8) | BIT(7) | BIT(6);

    /***Fix Wake up from CLKPM stuck at Pre-Detect_Quiet - Digital lane alignment, fast step 3***/
    //Try lane alignment digital sample fast step 3
    REG_U16(0xF00C0124) = 0x2b00;

    /***Fix Wake up from CLKPM stuck at Pre-Detect_Quiet - PLL Tuning***/
    //Set Lane0 vco_cal_vth_sel[2:0]=4 Shaobing 5/21/2015
    REG_U16(0xF00C0134) = (REG_U16(0xF00C0134) | BIT(5)) & ~BIT(4);

    /***Fix Wake up from CLKPM stuck at Pre-Detect_Quiet - PLL Tuning***/
    //0x70[13:7] sllp_dac[6:0] = "30" Shaobing 5/21/2015
    REG_U16(0xF00C01C0) = 0x1800;

    /***Fix Wake up from CLKPM stuck at Pre-Detect_Quiet - PLL Tuning***/
    //0x73[5:4] vcon_sel[1:0](default 1) = 0 Shaobing 5/22/2015
    rPciePhy.miscCtrl1.all = (rPciePhy.miscCtrl1.all & ~(uint16_t)BIT(4));

    /***Reduce L1.2 Wake up time***/
    //0xf5[6] tx_idlecm_cal_disable = 1 (default 0)Fen 5/28/2015
    //REG_U16(0xF00C03D4) = REG_U16(0xF00C03D4) | BIT(6);

    /***Fix AER error during Wake up test - Add RX_INIT time***/
    //0x6b[14] RX_FAST_ACQ_EN = 0 (default 1) Shaobing 6/9/2015
    rPciePhy.dfeTmr.b.RX_FAST_ACQ_EN = 0;

    /***Fix AER error during Wake up test - Add RX_INIT time***/
    //0x6C[7] rx_fast_acq_en_force = 1 (default 0) Shaobing 6/9/2015
    REG_U16(0xF00C01B0) = REG_U16(0xF00C01B0) | BIT(7);

    /***Reduce L1.2 Wake up time***/
    //0xf2[11:10] short_loz_idle_time = 1(default 0) Shaobing 6/1/2015
    //REG_U16(0xF00C03C8) = REG_U16(0xF00C03C8) | BIT(10);

    //TRX Training Side Shaobing 6/10/2015
    /***Fix EQ2 Timeout issue - reduce train time - Kuang-Yu***/
    //0xa1[12] cdr_max_f0p_en = 0 (default 1)
    REG_U16(0xF00C0284) = REG_U16(0xF00C0284) & (~BIT(12));

    /***Fix EQ2 Timeout issue - reduce train time - Kuang-Yu***/
    //0xd8[15] tx_gn1_maxf0t_en = 0 (default 1)
    REG_U16(0xF00C0360) = REG_U16(0xF00C0360) & (~BIT(15));

    /***Fix EQ2 Timeout issue - reduce train time - Kuang-Yu***/
    //0xd9[15] tx_g1_maxf0t_en = 0 (default 1)
    REG_U16(0xF00C0364) = REG_U16(0xF00C0364) & (~BIT(15));

    /***Fix EQ2 Timeout issue - reduce train time - Kuang-Yu***/
    //0x10e[15] tx_sweep_preset_en = 0 (default 1)
    //rPciePhy rsvd_RF00C041C[36];                                      ///< 0xF00C041C - 0xF00C043F : Reserved.
    REG_U16(0xF00C0438) = REG_U16(0xF00C0438) & (~BIT(15));

    /***Fix EQ2 Timeout issue - reduce train time - Kuang-Yu***/
    //0xa1[3:2] cdr_phase_step_size[1:0] = 2 (from 3)
    REG_U16(0xF00C0284) = REG_U16(0xF00C0284) & (~BIT(2)) | BIT(3);

    /***Fix EQ2 Timeout issue - reduce train time - Kuang-Yu***/
    //0x11a[3:0] TX_PRESET_INDEX[3:0] = 1 (default 5)
    rPciePhy.pcieTxPresetIndex.b.TX_PRESET_INDEX = 1;

    /***Fix EQ2 Timeout issue - reduce train time - Kuang-Yu***/
    //0x9e[10] TRX_TRAIN_TIMEOUT_EN = 1 (from 0)
    rPciePhy.txTrainingCtrl4.b.TRX_TRAIN_TIMEOUT_EN = 1;

#if (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED)
    /***Fix L1.2 wake up stuck at L1.Idle***/
    //REFCLK_RESTORE_DLY = 2
    {
        ClkreqTmCtrl_t clkreqTmCtrl;

        clkreqTmCtrl.all = rPciePhy.clkreqTmCtrl.all;
        clkreqTmCtrl.b.CLKREQ_N_SRC = 0;
        clkreqTmCtrl.b.CLKREQ_N_OVERRIDE = 0;
      #if (PCIE_REF_CLOCK_AUTO_DETECTION == ENABLE)
        clkreqTmCtrl.b.REFCLK_RESTORE_DLY = 2;
      #else
        clkreqTmCtrl.b.REFCLK_RESTORE_DLY = 0x3F;
      #endif
        clkreqTmCtrl.b.REFCLK_SHUTOFF_DLY = 2;
        clkreqTmCtrl.b.REFCLK_DISABLE_DLY_3_0 = 5;
        clkreqTmCtrl.b.REFCLK_DISABLE_DLY_5_4 = 0;

        rPciePhy.clkreqTmCtrl.all = clkreqTmCtrl.all;
    }

    /***Fix L1.2 wake up stuck at L1.Idle***/
    // OSCCLK Count Scale=2, Counts every 2 OSCCLK cycles
    REG_U16(0xF00C0718) |= BIT(8);

    //Reduce L1.2 Wake up time, Long Lo-Z Idle Time from 100us(default) to 40us
  #if (PCIE_REF_CLOCK_AUTO_DETECTION == ENABLE)
    rPciePhy.lowFrqCfg.b.LOW_FREQ_PERIOD_MAX = 0x46;        // RF00C075Ch[13:7]
    rPciePhy.lowFrqCfg.b.LOW_FREQ_PERIOD_MIN = 0x2C;        // RF00C075Ch[6:0]
  #else
    rPciePhy.lowFrqCfg.b.LOW_FREQ_PERIOD_MAX = 0x67;        // RF00C075Ch[13:7]
    rPciePhy.lowFrqCfg.b.LOW_FREQ_PERIOD_MIN = 0x5F;        // RF00C075Ch[6:0]
  #endif

    /***Reduce L1.2 Wake up time***/
    //Reduce L1.2 Wake up time, Long Lo-Z Idle Time from 100us(default) to 40us
    REG_U16(0xF00C03C8) = REG_U16(0xF00C03C8) | BIT(8);

    /***Disable Long Lo-Z (40us) By Shaobing 7/6/2015***/
    REG_U16(0xF00C03D4) = REG_U16(0xF00C03D4) & ~BIT(4);
    REG_U16(0xF00C075C) = 0x232C;
#endif

    REG_U16(0xF00C0624) &= ~BIT(2);

    //Fix PCIe register access hang issue when PERST# occurred
    REG_U32(0xF0115090) = REG_U32(0xF0115090) | BIT(30);

    // Max TX AMP

#if TX_AMP
    REG_U32(0xF00C06F0) = 0x188E;
    REG_U32(0xF00C0708) = 0x0004;
    REG_U32(0xF00C0600) = 0x1002;
#endif
}

//-----------------------------------------------------------------------------
/**
    Turnon phy/pipe

    @param  none
**/
//-----------------------------------------------------------------------------
static void ReleasePipeReset(void)
{
    // Release soft reset
    rPciePhy.rstAndClkCtrl.b.PIPE_SFT_RESET = 0;
}

//-----------------------------------------------------------------------------
/**
    Configure PCIe lanes

    @param  none
**/
//-----------------------------------------------------------------------------
static void ConfigurePcieLanes(void)
{
    uint16_t    lane;
    volatile ClkSrcHigh_t *laneClkSrcHigh;
    volatile RstAndClkCtrl_t *rstAndClkCtrl;

    // Multi-lane configuration
    // (master, break, start) = (1, 0, 1), (0, 0, 0), (0, 0, 0), (0, 1, 0)
#if (PCIE_LIMIT_WIDTH == PCIE_X1)
    laneClkSrcHigh = (ClkSrcHigh_t *)(0xF00C0710);
    laneClkSrcHigh->b.LANE_MASTER = 1;
    laneClkSrcHigh->b.LANE_BREAK = 1;
    laneClkSrcHigh->b.LANE_START = 1;
    laneClkSrcHigh->b.MODE_PIPE4_IF = 1;
#endif
    laneClkSrcHigh = (ClkSrcHigh_t *)(0xF00C0710);
    laneClkSrcHigh->b.LANE_MASTER = 1;
    laneClkSrcHigh->b.LANE_BREAK = 0;
    laneClkSrcHigh->b.LANE_START = 1;
    laneClkSrcHigh->b.MODE_PIPE4_IF = 1;

    laneClkSrcHigh = (ClkSrcHigh_t *)(0xF00C0F10);
    laneClkSrcHigh->b.LANE_MASTER = 0;
    laneClkSrcHigh->b.LANE_BREAK = 0;
    laneClkSrcHigh->b.LANE_START = 0;
    laneClkSrcHigh->b.MODE_PIPE4_IF = 1;

    laneClkSrcHigh = (ClkSrcHigh_t *)(0xF00C1710);
    laneClkSrcHigh->b.LANE_BREAK = 0;
    laneClkSrcHigh->b.LANE_MASTER = 0;
    laneClkSrcHigh->b.LANE_START = 0;
    laneClkSrcHigh->b.MODE_PIPE4_IF = 1;

    laneClkSrcHigh = (ClkSrcHigh_t *)(0xF00C1F10);
    laneClkSrcHigh->b.LANE_BREAK = 1;
    laneClkSrcHigh->b.LANE_MASTER = 0;
    laneClkSrcHigh->b.LANE_START = 0;
    laneClkSrcHigh->b.MODE_PIPE4_IF = 1;

    // Multicast
    rstAndClkCtrl = (RstAndClkCtrl_t *)&rPciePhy.rstAndClkCtrl.all;
    for(lane = 0; lane < 4; lane++)
    {
        rstAndClkCtrl = (RstAndClkCtrl_t *)(0xF00C0704 + lane * 0x800);
        rstAndClkCtrl->b.MODE_MULTICAST = 1;
        rstAndClkCtrl->b.MODE_PIPE_WIDTH32 = 1;
        rstAndClkCtrl->b.MODE_FIXED_PCLK = 0;
        rstAndClkCtrl->b.MODE_REFDIV_1_0 = 3;
    }
}

//-----------------------------------------------------------------------------
/**
    Wait for PERST# de-assert\n

    @param  timeOutUsec Reset Release timeout
**/
//-----------------------------------------------------------------------------
Error_t HalPhy_WaitForPerstRelease(uint32_t timeOutUsec)
{
    // wait for PERST# release
    if (HalRci_WaitValueU32((uint32_t)&VIC_READ_RAW_STATUS(0), 0, BIT(cIrqSourcePcieVRstN), timeOutUsec) != cEcNoError)
    {
        E_PRINTF(cEcTimeOut, "PERST release timeout !\n");
        return cEcTimeOut;
    }
    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Reset PCIe I/F Phy Layer H/W\n

    @param  resetMode    reset mode
**/
//-----------------------------------------------------------------------------
void HalPhy_Reset(ResetMode_t resetMode)
{
    ConfigurePcieLanes();
    ConfigurePciePhyPipe();
    EnablePcieBlockReset();
    if (HalPhy_WaitForPerstRelease(cPerstReleaseTimeOutUs) != cEcNoError)
    {
        D_PRINTF( "Timeout !!!!\n" );
        return;
    }
    ReleasePipeReset(); // reset is released, turnon the phy
}

//-----------------------------------------------------------------------------
/**
    Enable Reset of PCIe at PERST#

    @param  none
**/
//-----------------------------------------------------------------------------
static void EnablePcieBlockReset()
{
  #if CO_RESET_PCIE_PHY_ON_PERST || CO_REBOOT_DEVICE_ON_PERST
    REG_U32(0xF0112090) |= BIT(6);  // REV2.1 change to allow HW PCIe block reset at PERST
  #endif
}

//-----------------------------------------------------------------------------
/**
    PCIe I/F Phy power management API

    @param[in]  mode        targeting pm mode to enter
**/
//-----------------------------------------------------------------------------
void HalPhy_SetPowerMode(PciePhyPowerMode_t mode)
{
    switch(mode)
    {
    case cPciePhyPmDeepSleep:          ///< Target Mode
        // Turn off PCIe PHY PLL
        rPciePhy.a150mClkCtrl2.b.PU_PLL150M = 0;                // 0xF00C01DC[12] Power-Down PLL 150M
        break;

    case cPciePhyPmActive:             ///< Target Mode
        // Turn on PCIe PHY PLL
        rPciePhy.a150mClkCtrl2.b.PU_PLL150M = 1;                // 0xF00C01DC[12] Power-Up PLL 150M
        break;

    default:
        break;
    }
}

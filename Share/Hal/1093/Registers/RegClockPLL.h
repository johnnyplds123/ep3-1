#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2015  -  All rights reserved
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
//! @brief Clock and PLL Registers
//!
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  Clock and PLL Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF0112000 : Analog Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TEST_ANA            : 4;         ///<BIT [3:0] Test Analog.
        uint32_t SPEED_OSC           : 2;         ///<BIT [5:4] Speed Select for CLKOUT_OSC Frequency.
        uint32_t RF0112000_15_6_RSVD :10;         ///<BIT [15:6] Reserved.
        uint32_t PU_XTL              : 1;         ///<BIT [16] Power-Up Crystal.
        uint32_t PU_OSC              : 1;         ///<BIT [17] Free-Running Ring Oscillator.
        uint32_t PU                  : 1;         ///<BIT [18] Analog Group Power-Up.
        uint32_t IXTAL               : 2;         ///<BIT [20:19] Crystal Block Current Change.
        uint32_t GAINX2              : 1;         ///<BIT [21] Oscillator Circuit Gain Control.
        uint32_t SEL_CLKDIG_DIV4     : 2;         ///<BIT [23:22] Select CLKOUT_DIG Clock Division Ratio 4.
        uint32_t SEL_CLKDIG_DIV3     : 2;         ///<BIT [25:24] Select CLKOUT_DIG Clock Division Ratio 3.
        uint32_t SEL_CLKDIG_DIV2     : 2;         ///<BIT [27:26] Select CLKOUT_DIG Clock Division Ratio 2.
        uint32_t SEL_CLKDIG_DIV1     : 2;         ///<BIT [29:28] Select CLKOUT_DIG Clock Division Ratio 1.
        uint32_t XTAL_OSC_BYPASS     : 1;         ///<BIT [30] XTAL_OSC Bypass Control Signal.
        uint32_t AVDD1815_SEL        : 1;         ///<BIT [31] AVDD Select.
    } b;
} AnalogCtrl1_t;

///  @brief 0xF0112004 : Analog Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112004_19_0_RSVD  :20;         ///<BIT [19:0] Reserved.
        uint32_t ICC_ADJ              : 2;         ///<BIT [21:20] ICC_ADJ Current Select.
        uint32_t IPP_ADJ              : 2;         ///<BIT [23:22] IPP_ADJ Current Select.
        uint32_t RF0112004_26_24_RSVD : 3;         ///<BIT [26:24] Reserved.
        uint32_t VREG_1P2V_SEL        : 2;         ///<BIT [28:27] Crystal 1.2V Select.
        uint32_t VREG_0P9V_SEL_XTL    : 3;         ///<BIT [31:29] Regulator 0.9V Select.
    } b;
} AnalogCtrl2_t;

///  @brief 0xF0112010 : PLLA Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PLLA_LOCK              : 1;         ///<BIT [0] PLL A Lock Detect.
        uint32_t RF0112010_1_RSVD       : 1;         ///<BIT [1] Reserved.
        uint32_t PLLA_CTUNE             : 2;         ///<BIT [3:2] PLL A VCO Capacitor Select.
        uint32_t PLLA_CLK_DET_EN        : 1;         ///<BIT [4] PLL A Clock Detect Enable.
        uint32_t PLLA_CLKOUT_SRC_SEL    : 1;         ///<BIT [5] PLL A Clock Source Select.
        uint32_t PLLA_CLKOUT_SE_GATE_EN : 1;         ///<BIT [6] PLL A Lock Control.
        uint32_t PLLA_BYPASS_EN         : 1;         ///<BIT [7] PLL A Clock Bypass Control.
        uint32_t PLLA_BW_SEL            : 1;         ///<BIT [8] PLL A Bandwidth Select.
        uint32_t PLLA_AVDD1815_SEL      : 1;         ///<BIT [9] PLL A AVDD Select.
        uint32_t PLLA_FBDIV             : 9;         ///<BIT [18:10] PLL A Feedback Clock Divider.
        uint32_t PLLA_KVCO              : 4;         ///<BIT [22:19] PLL A KVCO Selection.
        uint32_t PLLA_CLKOUT_SE_DIV_SEL : 9;         ///<BIT [31:23] PLL A Post-Divider for Single-Ended Output Clock Control.
    } b;
} PllaCtrl1_t;

///  @brief 0xF0112014 : PLLA Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112014_0_RSVD          : 1;         ///<BIT [0] Reserved.
        uint32_t PLLA_INTPI                : 4;         ///<BIT [4:1] PLL A Phase Interpolator Bias Current Select.
        uint32_t PLLA_ICP                  : 4;         ///<BIT [8:5] PLL A Charge-Pump Current Control.
        uint32_t PLLA_FREQ_OFFSET_VALID    : 1;         ///<BIT [9] PLL A Frequency Offset Valid.
        uint32_t PLLA_FREQ_OFFSET_MODE_SEL : 1;         ///<BIT [10] PLL A Frequency Offset Mode Select.
        uint32_t PLLA_FREQ_OFFSET_EN       : 1;         ///<BIT [11] PLL A Frequency Offset Enable.
        uint32_t PLLA_FREQ_OFFSET          :17;         ///<BIT [28:12] PLL A Frequency Offset Value Control.
        uint32_t PLLA_FD                   : 3;         ///<BIT [31:29] PLL A Tune Frequency Detector.
    } b;
} PllaCtrl2_t;

///  @brief 0xF0112018 : PLLA Control 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112018_6_0_RSVD  : 7;         ///<BIT [6:0] Reserved.
        uint32_t PLLA_RST            : 1;         ///<BIT [7] PLL A Reset.
        uint32_t PLLA_RST_PI         : 1;         ///<BIT [8] PLL A External Interpolator Reset.
        uint32_t RF0112018_16_9_RSVD : 8;         ///<BIT [16:9] Reserved.
        uint32_t PLLA_REFDIV         : 9;         ///<BIT [25:17] PLL A Reference Clock Divider.
        uint32_t PLLA_PU             : 1;         ///<BIT [26] PLL A Power Up.
        uint32_t PLLA_PI_LOOP_MD     : 1;         ///<BIT [27] PLL A Phase Interpolator Loop Mode Control.
        uint32_t PLLA_PI_EN          : 1;         ///<BIT [28] PLL A Phase Interpolator Enable.
        uint32_t PLLA_INTPR          : 3;         ///<BIT [31:29] PLL A Phase Interpolator Resistor Select.
    } b;
} PllaCtrl3_t;

///  @brief 0xF011201C : PLLA Control 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF011201C_1_0_RSVD : 2;         ///<BIT [1:0] Reserved.
        uint32_t PLLA_SSC_RNGE      :11;         ///<BIT [12:2] PLL A Spread Spectrum Clocking Range Selection.
        uint32_t PLLA_SSC_MD        : 1;         ///<BIT [13] PLL A Spread Spectrum Clocking Mode.
        uint32_t PLLA_SSC_FREQ_DIV  :16;         ///<BIT [29:14] PLL A SSC Frequency Selection.
        uint32_t PLLA_SSC_CLK_EN    : 1;         ///<BIT [30] PLL A Spread Spectrum Clocking Clock Enable.
        uint32_t PLLA_RST_SSC       : 1;         ///<BIT [31] PLL A Spread Spectrum Clocking Reset Enable.
    } b;
} PllaCtrl4_t;

///  @brief 0xF0112020 : PLLA Control 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112020_22_0_RSVD :23;         ///<BIT [22:0] Reserved.
        uint32_t PLLA_VDDM           : 2;         ///<BIT [24:23] PLL A VCO Supply Control.
        uint32_t PLLA_VDDL           : 3;         ///<BIT [27:25] PLL A Internal VDD Supply Control.
        uint32_t PLLA_TEST_ANA       : 4;         ///<BIT [31:28] PLL A Test-out Select.
    } b;
} PllaCtrl5_t;

///  @brief 0xF0112030 : PLLB Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PLLB_LOCK              : 1;         ///<BIT [0] PLL B Lock Detect.
        uint32_t RF0112030_1_RSVD       : 1;         ///<BIT [1] Reserved.
        uint32_t PLLB_CTUNE             : 2;         ///<BIT [3:2] PLL B VCO Capacitor Select.
        uint32_t PLLB_CLK_DET_EN        : 1;         ///<BIT [4] PLL B Clock Detect Enable.
        uint32_t PLLB_CLKOUT_SRC_SEL    : 1;         ///<BIT [5] PLL B Clock Source Select.
        uint32_t PLLB_CLKOUT_SE_GATE_EN : 1;         ///<BIT [6] PLL B Lock Control.
        uint32_t PLLB_BYPASS_EN         : 1;         ///<BIT [7] PLL B Clock Bypass Control.
        uint32_t PLLB_BW_SEL            : 1;         ///<BIT [8] PLL B Bandwidth Select.
        uint32_t PLLB_AVDD1815_SEL      : 1;         ///<BIT [9] PLL B AVDD Select.
        uint32_t PLLB_FBDIV             : 9;         ///<BIT [18:10] PLL B Feedback Clock Divider.
        uint32_t PLLB_KVCO              : 4;         ///<BIT [22:19] PLL B KVCO Selection.
        uint32_t PLLB_CLKOUT_SE_DIV_SEL : 9;         ///<BIT [31:23] PLL B Post-Divider for Single-Ended Output Clock Control.
    } b;
} PllbCtrl1_t;

///  @brief 0xF0112034 : PLLB Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112034_0_RSVD          : 1;         ///<BIT [0] Reserved.
        uint32_t PLLB_INTPI                : 4;         ///<BIT [4:1] PLL B Phase Interpolator Bias Current Select.
        uint32_t PLLB_ICP                  : 4;         ///<BIT [8:5] PLL B Charge-Pump Current Control.
        uint32_t PLLB_FREQ_OFFSET_VALID    : 1;         ///<BIT [9] PLL B Frequency Offset Valid.
        uint32_t PLLB_FREQ_OFFSET_MODE_SEL : 1;         ///<BIT [10] PLL B Frequency Offset Mode Select.
        uint32_t PLLB_FREQ_OFFSET_EN       : 1;         ///<BIT [11] PLL B Frequency Offset Enable.
        uint32_t PLLB_FREQ_OFFSET          :17;         ///<BIT [28:12] PLL B Frequency Offset Value Control.
        uint32_t PLLB_FD                   : 3;         ///<BIT [31:29] PLL B Tune Frequency Detector.
    } b;
} PllbCtrl2_t;

///  @brief 0xF0112038 : PLLB Control 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112038_6_0_RSVD  : 7;         ///<BIT [6:0] Reserved.
        uint32_t PLLB_RST            : 1;         ///<BIT [7] PLL B Reset.
        uint32_t PLLB_RST_PI         : 1;         ///<BIT [8] PLL B External Interpolator Reset.
        uint32_t RF0112038_16_9_RSVD : 8;         ///<BIT [16:9] Reserved.
        uint32_t PLLB_REFDIV         : 9;         ///<BIT [25:17] PLL B Reference Clock Divider.
        uint32_t PLLB_PU             : 1;         ///<BIT [26] PLL B Power Up.
        uint32_t PLLB_PI_LOOP_MD     : 1;         ///<BIT [27] PLL B Phase Interpolator Loop Mode Control.
        uint32_t PLLB_PI_EN          : 1;         ///<BIT [28] PLL B Phase Interpolator Enable.
        uint32_t PLLB_INTPR          : 3;         ///<BIT [31:29] PLL B Phase Interpolator Resistor Select.
    } b;
} PllbCtrl3_t;

///  @brief 0xF011203C : PLLB Control 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF011203C_1_0_RSVD : 2;         ///<BIT [1:0] Reserved.
        uint32_t PLLB_SSC_RNGE      :11;         ///<BIT [12:2] PLL B Spread Spectrum Clocking Range Selection.
        uint32_t PLLB_SSC_MD        : 1;         ///<BIT [13] PLL B Spread Spectrum Clocking Mode.
        uint32_t PLLB_SSC_FREQ_DIV  :16;         ///<BIT [29:14] PLL B SSC Frequency Selection.
        uint32_t PLLB_SSC_CLK_EN    : 1;         ///<BIT [30] PLL B Spread Spectrum Clocking Clock Enable.
        uint32_t PLLB_RST_SSC       : 1;         ///<BIT [31] PLL B Spread Spectrum Clocking Reset Enable.
    } b;
} PllbCtrl4_t;

///  @brief 0xF0112040 : PLLB Control 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112040_22_0_RSVD :23;         ///<BIT [22:0] Reserved.
        uint32_t PLLB_VDDM           : 2;         ///<BIT [24:23] PLL B VCO Supply Control.
        uint32_t PLLB_VDDL           : 3;         ///<BIT [27:25] PLL B Internal VDD Supply Control.
        uint32_t PLLB_TEST_ANA       : 4;         ///<BIT [31:28] PLL B Test-out Select.
    } b;
} PllbCtrl5_t;

///  @brief 0xF0112050 : PLLC Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PLLC_LOCK              : 1;         ///<BIT [0] PLL C Lock Detect.
        uint32_t RF0112050_1_RSVD       : 1;         ///<BIT [1] Reserved.
        uint32_t PLLC_CTUNE             : 2;         ///<BIT [3:2] PLL C VCO Capacitor Select.
        uint32_t PLLC_CLK_DET_EN        : 1;         ///<BIT [4] PLL C Clock Detect Enable.
        uint32_t PLLC_CLKOUT_SRC_SEL    : 1;         ///<BIT [5] PLL C Clock Source Select.
        uint32_t PLLC_CLKOUT_SE_GATE_EN : 1;         ///<BIT [6] PLL C Lock Control.
        uint32_t PLLC_BYPASS_EN         : 1;         ///<BIT [7] PLL C Clock Bypass Control.
        uint32_t PLLC_BW_SEL            : 1;         ///<BIT [8] PLL C Bandwidth Select.
        uint32_t PLLC_AVDD1815_SEL      : 1;         ///<BIT [9] PLL C AVDD Select.
        uint32_t PLLC_FBDIV             : 9;         ///<BIT [18:10] PLL C Feedback Clock Divider.
        uint32_t PLLC_KVCO              : 4;         ///<BIT [22:19] PLL C KVCO Selection.
        uint32_t PLLC_CLKOUT_SE_DIV_SEL : 9;         ///<BIT [31:23] PLL C Post-Divider for Single-Ended Output Clock Control.
    } b;
} PllcCtrl1_t;

///  @brief 0xF0112054 : PLLC Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112054_0_RSVD          : 1;         ///<BIT [0] Reserved.
        uint32_t PLLC_INTPI                : 4;         ///<BIT [4:1] PLL C Phase Interpolator Bias Current Select.
        uint32_t PLLC_ICP                  : 4;         ///<BIT [8:5] PLL C Charge-Pump Current Control.
        uint32_t PLLC_FREQ_OFFSET_VALID    : 1;         ///<BIT [9] PLL C Frequency Offset Valid.
        uint32_t PLLC_FREQ_OFFSET_MODE_SEL : 1;         ///<BIT [10] PLL C Frequency Offset Mode Select.
        uint32_t PLLC_FREQ_OFFSET_EN       : 1;         ///<BIT [11] PLL C Frequency Offset Enable.
        uint32_t PLLC_FREQ_OFFSET          :17;         ///<BIT [28:12] PLL C Frequency Offset Value Control.
        uint32_t PLLC_FD                   : 3;         ///<BIT [31:29] PLL C Tune Frequency Detector.
    } b;
} PllcCtrl2_t;

///  @brief 0xF0112058 : PLLC Control 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112058_6_0_RSVD  : 7;         ///<BIT [6:0] Reserved.
        uint32_t PLLC_RST            : 1;         ///<BIT [7] PLL C Reset.
        uint32_t PLLC_RST_PI         : 1;         ///<BIT [8] PLL C External Interpolator Reset.
        uint32_t RF0112058_16_9_RSVD : 8;         ///<BIT [16:9] Reserved.
        uint32_t PLLC_REFDIV         : 9;         ///<BIT [25:17] PLL C Reference Clock Divider.
        uint32_t PLLC_PU             : 1;         ///<BIT [26] PLL C Power Up.
        uint32_t PLLC_PI_LOOP_MD     : 1;         ///<BIT [27] PLL C Phase Interpolator Loop Mode Control.
        uint32_t PLLC_PI_EN          : 1;         ///<BIT [28] PLL C Phase Interpolator Enable.
        uint32_t PLLC_INTPR          : 3;         ///<BIT [31:29] PLL C Phase Interpolator Resistor Select.
    } b;
} PllcCtrl3_t;

///  @brief 0xF011205C : PLLC Control 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF011205C_1_0_RSVD : 2;         ///<BIT [1:0] Reserved.
        uint32_t PLLC_SSC_RNGE      :11;         ///<BIT [12:2] PLL C Spread Spectrum Clocking Range Selection.
        uint32_t PLLC_SSC_MD        : 1;         ///<BIT [13] PLL C Spread Spectrum Clocking Mode.
        uint32_t PLLC_SSC_FREQ_DIV  :16;         ///<BIT [29:14] PLL C SSC Frequency Selection.
        uint32_t PLLC_SSC_CLK_EN    : 1;         ///<BIT [30] PLL C Spread Spectrum Clocking Clock Enable.
        uint32_t PLLC_RST_SSC       : 1;         ///<BIT [31] PLL C Spread Spectrum Clocking Reset Enable.
    } b;
} PllcCtrl4_t;

///  @brief 0xF0112060 : PLLC Control 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112060_22_0_RSVD :23;         ///<BIT [22:0] Reserved.
        uint32_t PLLC_VDDM           : 2;         ///<BIT [24:23] PLL C VCO Supply Control.
        uint32_t PLLC_VDDL           : 3;         ///<BIT [27:25] PLL C Internal VDD Supply Control.
        uint32_t PLLC_TEST_ANA       : 4;         ///<BIT [31:28] PLL C Test-out Select.
    } b;
} PllcCtrl5_t;

///  @brief 0xF0112070 : PLLD Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PLLD_LOCK              : 1;         ///<BIT [0] PLL D Lock Detect.
        uint32_t RF0112070_1_RSVD       : 1;         ///<BIT [1] Reserved.
        uint32_t PLLD_CTUNE             : 2;         ///<BIT [3:2] PLL D VCO Capacitor Select.
        uint32_t PLLD_CLK_DET_EN        : 1;         ///<BIT [4] PLL D Clock Detect Enable.
        uint32_t PLLD_CLKOUT_SRC_SEL    : 1;         ///<BIT [5] PLL D Clock Source Select.
        uint32_t PLLD_CLKOUT_SE_GATE_EN : 1;         ///<BIT [6] PLL D Lock Control.
        uint32_t PLLD_BYPASS_EN         : 1;         ///<BIT [7] PLL D Clock Bypass Control.
        uint32_t PLLD_BW_SEL            : 1;         ///<BIT [8] PLL D Bandwidth Select.
        uint32_t PLLD_AVDD1815_SEL      : 1;         ///<BIT [9] PLL D AVDD Select.
        uint32_t PLLD_FBDIV             : 9;         ///<BIT [18:10] PLL D Feedback Clock Divider.
        uint32_t PLLD_KVCO              : 4;         ///<BIT [22:19] PLL D KVCO Selection.
        uint32_t PLLD_CLKOUT_SE_DIV_SEL : 9;         ///<BIT [31:23] PLL D Post-Divider for Single-Ended Output Clock Control.
    } b;
} PlldCtrl1_t;

///  @brief 0xF0112074 : PLLD Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112074_0_RSVD          : 1;         ///<BIT [0] Reserved.
        uint32_t PLLD_INTPI                : 4;         ///<BIT [4:1] PLL D Phase Interpolator Bias Current Select.
        uint32_t PLLD_ICP                  : 4;         ///<BIT [8:5] PLL D Charge-Pump Current Control.
        uint32_t PLLD_FREQ_OFFSET_VALID    : 1;         ///<BIT [9] PLL D Frequency Offset Valid.
        uint32_t PLLD_FREQ_OFFSET_MODE_SEL : 1;         ///<BIT [10] PLL D Frequency Offset Mode Select.
        uint32_t PLLD_FREQ_OFFSET_EN       : 1;         ///<BIT [11] PLL D Frequency Offset Enable.
        uint32_t PLLD_FREQ_OFFSET          :17;         ///<BIT [28:12] PLL D Frequency Offset Value Control.
        uint32_t PLLD_FD                   : 3;         ///<BIT [31:29] PLL D Tune Frequency Detector.
    } b;
} PlldCtrl2_t;

///  @brief 0xF0112078 : PLLD Control 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112078_6_0_RSVD  : 7;         ///<BIT [6:0] Reserved.
        uint32_t PLLD_RST            : 1;         ///<BIT [7] PLL D Reset.
        uint32_t PLLD_RST_PI         : 1;         ///<BIT [8] PLL D External Interpolator Reset.
        uint32_t RF0112078_16_9_RSVD : 8;         ///<BIT [16:9] Reserved.
        uint32_t PLLD_REFDIV         : 9;         ///<BIT [25:17] PLL D Reference Clock Divider.
        uint32_t PLLD_PU             : 1;         ///<BIT [26] PLL D Power Up.
        uint32_t PLLD_PI_LOOP_MD     : 1;         ///<BIT [27] PLL D Phase Interpolator Loop Mode Control.
        uint32_t PLLD_PI_EN          : 1;         ///<BIT [28] PLL D Phase Interpolator Enable.
        uint32_t PLLD_INTPR          : 3;         ///<BIT [31:29] PLL D Phase Interpolator Resistor Select.
    } b;
} PlldCtrl3_t;

///  @brief 0xF011207C : PLLD Control 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF011207C_1_0_RSVD : 2;         ///<BIT [1:0] Reserved.
        uint32_t PLLD_SSC_RNGE      :11;         ///<BIT [12:2] PLL D Spread Spectrum Clocking Range Selection.
        uint32_t PLLD_SSC_MD        : 1;         ///<BIT [13] PLL D Spread Spectrum Clocking Mode.
        uint32_t PLLD_SSC_FREQ_DIV  :16;         ///<BIT [29:14] PLL D SSC Frequency Selection.
        uint32_t PLLDC_SSC_CLK_EN   : 1;         ///<BIT [30] PLL D Spread Spectrum Clocking Clock Enable.
        uint32_t PLLD_RST_SSC       : 1;         ///<BIT [31] PLL D Spread Spectrum Clocking Reset Enable.
    } b;
} PlldCtrl4_t;

///  @brief 0xF0112080 : PLLD Control 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112080_22_0_RSVD :23;         ///<BIT [22:0] Reserved.
        uint32_t PLLD_VDDM           : 2;         ///<BIT [24:23] PLL D VCO Supply Control.
        uint32_t PLLD_VDDL           : 3;         ///<BIT [27:25] PLL D Internal VDD Supply Control.
        uint32_t PLLD_TEST_ANA       : 4;         ///<BIT [31:28] PLL D Test-out Select.
    } b;
} PlldCtrl5_t;

///  @brief 0xF0112090 : Clock Enable Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MC5_MC_DCLK_REG_EN_DIS : 1;         ///<BIT [0] MC5_MC_DCLK_REG_EN Disable.
        uint32_t MC5_MC_DCLK_EN_DIS     : 1;         ///<BIT [1] MC5_MC_DCLK_EN Disable.
        uint32_t MC5_PHY_CLK_EN_CH0_DIS : 1;         ///<BIT [2] MC5_PHY_CLK_EN Disable.
        uint32_t RF0112090_3_RSVD       : 1;         ///<BIT [3] Reserved.
        uint32_t PCIE_BUTTON_RESET      : 1;         ///<BIT [4] PCIE_BUTTON_RESET
        uint32_t RF0112090_8_5_RSVD     : 4;         ///<BIT [8:5] Reserved.
        uint32_t SYSAXI_CLK_EN          : 1;         ///<BIT [9] SYSAXI_CLK Enable or Disable.
        uint32_t PCIAXI_CLK_EN          : 1;         ///<BIT [10] PCIAXI_CLK Enable or Disable.
        uint32_t ECC_CLK_EN             : 1;         ///<BIT [11] ECC_CLK Enable or Disable.
        uint32_t FLASH_CLK_EN           : 1;         ///<BIT [12] FLASH_CLK_EN Enable or Disable.
        uint32_t RF0112090_13_RSVD      : 1;         ///<BIT [13] Reserved.
        uint32_t I2C_CLK_EN             : 1;         ///<BIT [14] I2C_CLK Enable or Disable.
        uint32_t RF0112090_15_RSVD      : 1;         ///<BIT [15] Reserved.
        uint32_t RF0112090_16_RSVD      : 1;         ///<BIT [16] Reserved.
        uint32_t RF0112090_17_RSVD      : 1;         ///<BIT [17] Reserved.
        uint32_t PDBG_CLK_EN            : 1;         ///<BIT [18] Processor Debug Clock Enable or Disable.
        uint32_t RF0112090_19_RSVD      : 1;         ///<BIT [19] Reserved.
        uint32_t AMBA_CLK_EN            : 1;         ///<BIT [20] AMBA_CLK Enable or Disable.
        uint32_t TRACE_TPIU_CLK_EN      : 1;         ///<BIT [21] TRACE_TPIU_CLK Enable or Disable.
        uint32_t TRACE_CLK_EN           : 1;         ///<BIT [22] TRACE_CLK Enable or Disable.
        uint32_t AT_CLK_EN              : 1;         ///<BIT [23] AT_CLK Enable or Disable.
        uint32_t CPU2_CLK_EN            : 1;         ///<BIT [24] CPU2 Clock Enable.
        uint32_t CPU1_CLK_EN            : 1;         ///<BIT [25] CPU1 Clock Enable.
        uint32_t CPU0_CLK_EN            : 1;         ///<BIT [26] CPU0 Clock Enable.
        uint32_t MC5_CLK_EN             : 1;         ///<BIT [27] MC5 Clock Enable.
        uint32_t RF0112090_28_RSVD      : 1;         ///<BIT [28] Reserved.
        uint32_t AVS_CLK_EN             : 1;         ///<BIT [29] AVS Clock Enable or Disable.
        uint32_t RFSH_CLK_EN            : 1;         ///<BIT [30] RFSH_CLK Enable or Disable.
        uint32_t OSC_CLK_EN             : 1;         ///<BIT [31] OSC Clock Enable or Disable.
    } b;
} ClkEnCtrl_t;

///  @brief 0xF0112094 : Clock Selection Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ECC_CLK_SEL              : 3;         ///<BIT [2:0] ECC_CLK Divider Selection.
        uint32_t RF0112094_3_RSVD         : 1;         ///<BIT [3] Reserved.
        uint32_t PCIAXI_CLK_DIV_SEL       : 3;         ///<BIT [6:4] PCIAXI_CLK Divider Selection.
        uint32_t RF0112094_7_RSVD         : 1;         ///<BIT [7] Reserved.
        uint32_t SYSAXI_CLK_DIV_SEL       : 3;         ///<BIT [10:8] SYSAXI_CLK Divider Selection.
        uint32_t RF0112094_15_11_RSVD     : 5;         ///<BIT [15:11] Reserved.
        uint32_t CLK_SEL_AUX_FW           : 1;         ///<BIT [16] Clock Selection AUXCLK.
        uint32_t RF0112094_19_17_RSVD     : 3;         ///<BIT [19:17] Reserved.
        uint32_t CLK_SEL_PHY_AON          : 1;         ///<BIT [20] Clock Selection Between 100MHzCLK and AON.
        uint32_t CLK_SEL_PHY_100_PCORE    : 1;         ///<BIT [21] Clock Selection PHY Between 100HzCLK and PCLK/CORECLK.
        uint32_t CLK_SEL_PHY_P_CORE       : 1;         ///<BIT [22] Clock Selection PHY Between PCLK And CORECLK.
        uint32_t CLK_SEL_PCISYAXI_OSC_AON : 1;         ///<BIT [23] Clock Selection PCISYSAXI.
        uint32_t CLK_SEL_DDR_CLK          : 1;         ///<BIT [24] Clock Selection DDR_CLK.
        uint32_t CLK_SEL_SYSAXI_OSC       : 1;         ///<BIT [25] Clock Selection SYSAXI.
        uint32_t CLK_SEL_PCIAXI_OSC       : 1;         ///<BIT [26] Clock Selection PCIe AXI.
        uint32_t CLK_SEL_ECC_OSC          : 1;         ///<BIT [27] Clock Selection ECC.
        uint32_t CLK_SEL_PLLC_OSC         : 1;         ///<BIT [28] Clock Selection PLL C.
        uint32_t CLK_SEL_PLLB_OSC         : 1;         ///<BIT [29] Clock Selection PLL B.
        uint32_t CLK_SEL_PLLA_OSC         : 1;         ///<BIT [30] Clock Selection PLL A.
        uint32_t CLK_SEL_OSC_1M           : 1;         ///<BIT [31] Clock Selection OSC or 1M.
    } b;
} ClkSelectioCtrl_t;

///  @brief 0xF0113078 : Power Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLKREQ_N_RAW        : 1;         ///<BIT [0] CLKREQ# Raw Source.
        uint32_t CLKREQ_N_RDET       : 1;         ///<BIT [1] CLKREQ# Rising Edge Event AON.
        uint32_t CLKREQ_N_RDET_PS    : 1;         ///<BIT [2] CLKREQ# Rising Edge Event PCLK.
        uint32_t RF0113078_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;
} PowerCtrl1_t;

///  @brief 0xF011307C : Power Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PMU_PU_T3_SEL_3     : 4;         ///<BIT [3:0] Power Management Unit Power Up Time 3 Select.
        uint32_t PMU_PU_T2_SEL       : 4;         ///<BIT [7:4] Power Management Unit Power Up Time 2 Select.
        uint32_t PMU_PU_T1_SEL       : 4;         ///<BIT [11:8] Power Management Unit Power Up Time 1 Select.
        uint32_t PMU_PD_T1_SEL       : 4;         ///<BIT [15:12] Power Management Unit Power Down Time 1 Select.
        uint32_t CLK_REQ_N_FILTER_EN : 1;         ///<BIT [16] CLK_REQ_N Filter Enable.
        uint32_t TRST_GPIO1_EN       : 1;         ///<BIT [17] JTAG TRST Debugging Enable With GPIO1.
        uint32_t FORCE_AHCI_NVME_SEL : 1;         ///<BIT [18] Force Selection Between AHCI and NVMe Mode.
        uint32_t AHCI_NVME_SEL       : 1;         ///<BIT [19] AHCI or NVMe Mode Select.
        uint32_t FCT_PU_ACK          : 1;         ///<BIT [20] Flash Controller Power Up ACK.
        uint32_t FCT_PD_ACK          : 1;         ///<BIT [21] Flash Controller Power Down ACK.
        uint32_t FCT_PU_REQ          : 1;         ///<BIT [22] Flash Controller Power Up Request.
        uint32_t FCT_PD_REQ          : 1;         ///<BIT [23] Flash Controller Power Down Request.
        uint32_t GPIO_0_OE           : 1;         ///<BIT [24] PIN_GPIO_0 Output Enable.
        uint32_t GPIO_1_OE           : 1;         ///<BIT [25] PIN_GPIO_1 Output Enable.
        uint32_t GPIO_2_OE           : 1;         ///<BIT [26] PIN_GPIO_2 Output Enable.
        uint32_t SEL_OSC0_1M1        : 1;         ///<BIT [27] Select 1-MHz or OSC Clock.
        uint32_t SEL_NOA_FCT         : 1;         ///<BIT [28] Select NOA or FCT Power Down.
        uint32_t CLK_REQ_N_RDET_INT  : 1;         ///<BIT [29] CLK_REQ_N_RDET Interrupt.
        uint32_t ANAGRP_PU           : 1;         ///<BIT [30] Analog Group Power Up.
        uint32_t NOA_PD_REQ          : 1;         ///<BIT [31] NOA Power Down Request.
    } b;
} PowerCtrl2_t;


///  @brief 0xF0112010 : PLLControl 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PLL_LOCK              : 1;         ///<BIT [0]     PLL  Lock Detect.
        uint32_t RF0112010_1_RSVD      : 1;         ///<BIT [1]     Reserved.
        uint32_t PLL_CTUNE             : 2;         ///<BIT [3:2]   PLL  VCO Capacitor Select.
        uint32_t PLL_CLK_DET_EN        : 1;         ///<BIT [4]     PLL  Clock Detect Enable.
        uint32_t PLL_CLKOUT_SRC_SEL    : 1;         ///<BIT [5]     PLL  Clock Source Select.
        uint32_t PLL_CLKOUT_SE_GATE_EN : 1;         ///<BIT [6]     PLL  Lock Control.
        uint32_t PLL_BYPASS_EN         : 1;         ///<BIT [7]     PLL  Clock Bypass Control.
        uint32_t PLL_BW_SEL            : 1;         ///<BIT [8]     PLL  Bandwidth Select.
        uint32_t PLL_AVDD1815_SEL      : 1;         ///<BIT [9]     PLL  AVDD Select.
        uint32_t PLL_FBDIV             : 9;         ///<BIT [18:10] PLL  Feedback Clock Divider.
        uint32_t PLL_KVCO              : 4;         ///<BIT [22:19] PLL  KVCO Selection.
        uint32_t PLL_CLKOUT_SE_DIV_SEL : 9;         ///<BIT [31:23] PLL  Post-Divider for Single-Ended Output Clock Control.
    } b;
} PllCtrl1_t;

///  @brief 0xF0112014 : PLLControl 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112014_0_RSVD         : 1;      ///<BIT [0]      Reserved.
        uint32_t PLL_INTPI                : 4;      ///<BIT [4:1]    PLL  Phase Interpolator Bias Current Select.
        uint32_t PLL_ICP                  : 4;      ///<BIT [8:5]    PLL  Charge-Pump Current Control.
        uint32_t PLL_FREQ_OFFSET_VALID    : 1;      ///<BIT [9]      PLL  Frequency Offset Valid.
        uint32_t PLL_FREQ_OFFSET_MODE_SEL : 1;      ///<BIT [10]     PLL  Frequency Offset Mode Select.
        uint32_t PLL_FREQ_OFFSET_EN       : 1;      ///<BIT [11]     PLL  Frequency Offset Enable.
        uint32_t PLL_FREQ_OFFSET          :17;      ///<BIT [28:12]  PLL  Frequency Offset Value Control.
        uint32_t PLL_FD                   : 3;      ///<BIT [31:29]  PLL  Tune Frequency Detector.
    } b;
} PllCtrl2_t;

///  @brief 0xF0112018 : PLLControl 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112018_6_0_RSVD : 7;         ///<BIT [6:0] Reserved.
        uint32_t PLL_RST            : 1;         ///<BIT [7] PLL  Reset.
        uint32_t PLL_RST_PI         : 1;         ///<BIT [8] PLL  External Interpolator Reset.
        uint32_t RF0112018_16_9_RSVD: 8;         ///<BIT [16:9] Reserved.
        uint32_t PLL_REFDIV         : 9;         ///<BIT [25:17] PLL  Reference Clock Divider.
        uint32_t PLL_PU             : 1;         ///<BIT [26] PLL  Power Up.
        uint32_t PLL_PI_LOOP_MD     : 1;         ///<BIT [27] PLL  Phase Interpolator Loop Mode Control.
        uint32_t PLL_PI_EN          : 1;         ///<BIT [28] PLL  Phase Interpolator Enable.
        uint32_t PLL_INTPR          : 3;         ///<BIT [31:29] PLL  Phase Interpolator Resistor Select.
    } b;
} PllCtrl3_t;

///  @brief 0xF011201C : PLLControl 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF011201C_1_0_RSVD: 2;         ///<BIT [1:0] Reserved.
        uint32_t PLL_SSC_RNGE      :11;         ///<BIT [12:2] PLL  Spread Spectrum Clocking Range Selection.
        uint32_t PLL_SSC_MD        : 1;         ///<BIT [13] PLL  Spread Spectrum Clocking Mode.
        uint32_t PLL_SSC_FREQ_DIV  :16;         ///<BIT [29:14] PLL  SSC Frequency Selection.
        uint32_t PLL_SSC_CLK_EN    : 1;         ///<BIT [30] PLL  Spread Spectrum Clocking Clock Enable.
        uint32_t PLL_RST_SSC       : 1;         ///<BIT [31] PLL  Spread Spectrum Clocking Reset Enable.
    } b;
} PllCtrl4_t;

///  @brief 0xF0112020 : PLLControl 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0112020_22_0_RSVD:23;         ///<BIT [22:0] Reserved.
        uint32_t PLL_VDDM           : 2;         ///<BIT [24:23] PLL  VCO Supply Control.
        uint32_t PLL_VDDL           : 3;         ///<BIT [27:25] PLL  Internal VDD Supply Control.
        uint32_t PLL_TEST_ANA       : 4;         ///<BIT [31:28] PLL  Test-out Select.
    } b;
} PllCtrl5_t;

typedef struct
{
    PllCtrl1_t ctrl1;
    PllCtrl2_t ctrl2;
    PllCtrl3_t ctrl3;
    PllCtrl4_t ctrl4;
    PllCtrl5_t ctrl5;
    uint8_t reserved[12];
} PllCtrl_t;

/// @brief 0xF0112000-0xF0113080 : ClockPll_t MODULE
typedef struct
{
    AnalogCtrl1_t analogCtrl1;             ///< 0xF0112000 : Analog Control 1
    AnalogCtrl2_t analogCtrl2;             ///< 0xF0112004 : Analog Control 2
    uint8_t rsvd_RF0112008[8];             ///< 0xF0112008 - 0xF011200F : Reserved.
    PllaCtrl1_t pllaCtrl1;                 ///< 0xF0112010 : PLLA Control 1
    PllaCtrl2_t pllaCtrl2;                 ///< 0xF0112014 : PLLA Control 2
    PllaCtrl3_t pllaCtrl3;                 ///< 0xF0112018 : PLLA Control 3
    PllaCtrl4_t pllaCtrl4;                 ///< 0xF011201C : PLLA Control 4
    PllaCtrl5_t pllaCtrl5;                 ///< 0xF0112020 : PLLA Control 5
    uint8_t rsvd_RF0112024[12];            ///< 0xF0112024 - 0xF011202F : Reserved.
    PllbCtrl1_t pllbCtrl1;                 ///< 0xF0112030 : PLLB Control 1
    PllbCtrl2_t pllbCtrl2;                 ///< 0xF0112034 : PLLB Control 2
    PllbCtrl3_t pllbCtrl3;                 ///< 0xF0112038 : PLLB Control 3
    PllbCtrl4_t pllbCtrl4;                 ///< 0xF011203C : PLLB Control 4
    PllbCtrl5_t pllbCtrl5;                 ///< 0xF0112040 : PLLB Control 5
    uint8_t rsvd_RF0112044[12];            ///< 0xF0112044 - 0xF011204F : Reserved.
    PllcCtrl1_t pllcCtrl1;                 ///< 0xF0112050 : PLLC Control 1
    PllcCtrl2_t pllcCtrl2;                 ///< 0xF0112054 : PLLC Control 2
    PllcCtrl3_t pllcCtrl3;                 ///< 0xF0112058 : PLLC Control 3
    PllcCtrl4_t pllcCtrl4;                 ///< 0xF011205C : PLLC Control 4
    PllcCtrl5_t pllcCtrl5;                 ///< 0xF0112060 : PLLC Control 5
    uint8_t rsvd_RF0112064[12];            ///< 0xF0112064 - 0xF011206F : Reserved.
    PlldCtrl1_t plldCtrl1;                 ///< 0xF0112070 : PLLD Control 1
    PlldCtrl2_t plldCtrl2;                 ///< 0xF0112074 : PLLD Control 2
    PlldCtrl3_t plldCtrl3;                 ///< 0xF0112078 : PLLD Control 3
    PlldCtrl4_t plldCtrl4;                 ///< 0xF011207C : PLLD Control 4
    PlldCtrl5_t plldCtrl5;                 ///< 0xF0112080 : PLLD Control 5
    uint8_t rsvd_RF0112084[12];            ///< 0xF0112084 - 0xF011208F : Reserved.
    ClkEnCtrl_t clkEnCtrl;                 ///< 0xF0112090 : Clock Enable Control
    ClkSelectioCtrl_t clkSelectioCtrl;     ///< 0xF0112094 : Clock Selection Control
    uint8_t rsvd_RF0112098[3944];          ///< 0xF0112098 - 0xF0112FFF : Reserved.
    uint32_t WSIGN;                        ///< 0xF0113000 : Wake-up Signature
    uint8_t rsvd_RF0113004[116];           ///< 0xF0113004 - 0xF0113077 : Reserved.
    PowerCtrl1_t powerCtrl1;               ///< 0xF0113078 : Power Control 1
    PowerCtrl2_t powerCtrl2;               ///< 0xF011307C : Power Control 2
} ClockPll_t;
COMPILE_ASSERT(offsetof(ClockPll_t,powerCtrl2)==0xF011307C-0xF0112000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile ClockPll_t rClockPll;                ///< 0xF0112000-0xF0113080 : ClockPll_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

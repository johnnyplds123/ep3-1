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
//! @brief PCIe Express Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  PCIe Express Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF00C0000 : Chip ID
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CID1_3_0             : 4;         ///<BIT [3:0] Minor Revision.
        uint32_t CID1_7_4             : 4;         ///<BIT [7:4] Major Revision.
        uint32_t CID0_3_0             : 4;         ///<BIT [11:8] PHY Type.
        uint32_t CID0_7_4             : 4;         ///<BIT [15:12] PHY Technology.
        uint32_t RF00C0000_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} ChipId11_t;

///  @brief 0xF00C0004 : Power and PLL Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t REF_FREF_SEL         : 5;         ///<BIT [4:0] Reference Frequency Select.
        uint32_t PHY_MODE             : 3;         ///<BIT [7:5] PHY Mode.
        uint32_t PLL_LOCK             : 1;         ///<BIT [8] PLL Lock Indicator.
        uint32_t RF00C0004_9_RSVD     : 1;         ///<BIT [9] Reserved.
        uint32_t PU_DFE               : 1;         ///<BIT [10] Power-Up Decision Feedback Equalization (DFE).
        uint32_t RF00C0004_11_RSVD    : 1;         ///<BIT [11] Reserved.
        uint32_t PU_TX                : 1;         ///<BIT [12] Power-Up Transmitter.
        uint32_t PU_RX                : 1;         ///<BIT [13] Power-Up Receiver.
        uint32_t PU_PLL               : 1;         ///<BIT [14] Power-Up PLL.
        uint32_t RF00C0004_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C0004_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PowerAndPllCtrl_t;

///  @brief 0xF00C0008 : KVCO Calibration Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0008_0_RSVD     : 1;         ///<BIT [0] Reserved.
        uint32_t RF00C0008_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t SPEED                : 6;         ///<BIT [7:2] SPEED_PLL External Overwrite Value.
        uint32_t SPEED_EXT_EN         : 1;         ///<BIT [8] SPEED_PLL External Value Enable.
        uint32_t EXT_FORCE_CAL_DONE   : 1;         ///<BIT [9] Force Calibration Done.
        uint32_t RF00C0008_10_RSVD    : 1;         ///<BIT [10] Reserved.
        uint32_t RF00C0008_11_RSVD    : 1;         ///<BIT [11] Reserved.
        uint32_t USE_MAX_PLL_RATE     : 1;         ///<BIT [12] Maximum PLL Rate.
        uint32_t RF00C0008_13_RSVD    : 1;         ///<BIT [13] Reserved.
        uint32_t CAL_DONE             : 1;         ///<BIT [14] Calibration Done.
        uint32_t CAL_START            : 1;         ///<BIT [15] Calibration Start.
        uint32_t RF00C0008_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} KvcoCalCtrl_t;

///  @brief 0xF00C000C : Impedance Calibration Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SSC_DSPREAD_RX       : 1;         ///<BIT [0] RX Spread Spectrum Clock Down-Spread.
        uint32_t SSC_DSPREAD_TX       : 1;         ///<BIT [1] TX Spread Spectrum Clock Down-Spread.
        uint32_t RF00C000C_2_RSVD     : 1;         ///<BIT [2] Reserved.
        uint32_t RF00C000C_3_RSVD     : 1;         ///<BIT [3] Reserved.
        uint32_t EXT_RXIMP_EN         : 1;         ///<BIT [4] External Impedance Enable for RX.
        uint32_t TXIMP                : 5;         ///<BIT [9:5] TX Impedance Auto-calibration Result.
        uint32_t EXT_TXIMP            : 5;         ///<BIT [14:10] External Transmitter Impedance.
        uint32_t EXT_TXIMP_EN         : 1;         ///<BIT [15] External Impedance Enable for TX.
        uint32_t RF00C000C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} ImpCalCtrl_t;

///  @brief 0xF00C0010 : Impedance and Transmitter SSC Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0010_5_0_RSVD   : 6;         ///<BIT [5:0] Reserved.
        uint32_t RXIMP                : 5;         ///<BIT [10:6] Receiver Impedance Auto-calibration Result.
        uint32_t EXT_RXIMP            : 5;         ///<BIT [15:11] External Receiver Impedance.
        uint32_t RF00C0010_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} ImpAndTransmitSscCtrl_t;

///  @brief 0xF00C0014 : Transmitter Frequency Offset
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0014_13_0_RSVD  :14;         ///<BIT [13:0] Reserved.
        uint32_t SG_SATA_MODE         : 1;         ///<BIT [14] Special SATA Mode.
        uint32_t SG_USB3_MODE         : 1;         ///<BIT [15] Special USB 3.0 Mode.
        uint32_t RF00C0014_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TransmitFrqOs_t;

///  @brief 0xF00C0018 : Squelch and FFE Setting
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0018_7_0_RSVD   : 8;         ///<BIT [7:0] Reserved.
        uint32_t SQ_THRESH_IN         : 4;         ///<BIT [11:8] Squelch Detector Threshold Setting.
        uint32_t RF00C0018_13_12_RSVD : 2;         ///<BIT [13:12] Reserved.
        uint32_t SQ_DETECTED          : 1;         ///<BIT [14] Squelch Detected.
        uint32_t RF00C0018_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C0018_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} SqlAndFfeSetting_t;

///  @brief 0xF00C001C : DFE Resolution Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DFE_STEP_FINE_FX     : 4;         ///<BIT [3:0] Decision Feedback Equalization (DFE) Fine Step.
        uint32_t DFE_STEP_COARSE_FX   : 4;         ///<BIT [7:4] Decision Feedback Equalization (DFE) Coarse Step.
        uint32_t RF00C001C_9_8_RSVD   : 2;         ///<BIT [9:8] Reserved.
        uint32_t DFE_LOCK_LEN         : 2;         ///<BIT [11:10] Decision Feedback Equalizer (DFE) Lock Length.
        uint32_t DFE_LOCK             : 1;         ///<BIT [12] Decision Feedback Equalization (DFE) Lock.
        uint32_t DFE_PAT_DIS          : 1;         ///<BIT [13] Decision Feedback Equalization (DFE) Pattern Protection Disable.
        uint32_t DFE_SQ_EN            : 1;         ///<BIT [14] Decision Feedback Equalizer (DFE) Squelch Enable.
        uint32_t DFE_RES_FORCE        : 1;         ///<BIT [15] Force DFE Resolution.
        uint32_t RF00C001C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DfeResolutiCtrl_t;

///  @brief 0xF00C0020 : DFE Update Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DFE_FX_FORCE         : 6;         ///<BIT [5:0] Decision Feedback Equalization (DFE) FX Force.
        uint32_t DFE_UPDATE_EN        : 6;         ///<BIT [11:6] Decision Feedback Equalization (DFE) Update Enable.
        uint32_t F0_SAT_HOLD_THRES    : 2;         ///<BIT [13:12] F0 Saturation Hold Threshold.
        uint32_t F0_SAT_HOLD          : 1;         ///<BIT [14] F0 Saturation Hold.
        uint32_t F0_SAT_EN            : 1;         ///<BIT [15] F0 Saturate Enable.
        uint32_t RF00C0020_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DfeUpdateCtrl_t;

///  @brief 0xF00C0024 : DFE Coefficient Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DFE_F3_EXT           : 4;         ///<BIT [3:0] Decision Feedback Equalization (DFE) F3.
        uint32_t DFE_F1_EXT           : 6;         ///<BIT [9:4] Decision Feedback Equalization (DFE) F1.
        uint32_t DFE_F0_EXT           : 6;         ///<BIT [15:10] Decision Feedback Equalization (DFE) F0.
        uint32_t RF00C0024_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DfeCoeffCtrl0_t;

///  @brief 0xF00C0028 : DFE Coefficient Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DFE_F5_EXT           : 4;         ///<BIT [3:0] Decision Feedback Equalization (DFE) F5.
        uint32_t DFE_F4_EXT           : 4;         ///<BIT [7:4] Decision Feedback Equalization (DFE) F4.
        uint32_t DFE_F2_EXT           : 5;         ///<BIT [12:8] Decision Feedback Equalization (DFE) F2.
        uint32_t RF00C0028_13_RSVD    : 1;         ///<BIT [13] Reserved.
        uint32_t RF00C0028_14_RSVD    : 1;         ///<BIT [14] Reserved.
        uint32_t RF00C0028_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C0028_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DfeCoeffCtrl1_t;

///  @brief 0xF00C002C : Adapted DFE Coefficient 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DFE_F3               : 4;         ///<BIT [3:0] Decision Feedback Equalization (DFE) F3.
        uint32_t DFE_F1               : 6;         ///<BIT [9:4] Decision Feedback Equalization (DFE) F1.
        uint32_t DFE_F0               : 6;         ///<BIT [15:10] Decision Feedback Equalization (DFE) F0.
        uint32_t RF00C002C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} AdtDfeCoeff0_t;

///  @brief 0xF00C0030 : Adapted DFE Coefficient 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DFE_F5               : 4;         ///<BIT [3:0] Decision Feedback Equalization (DFE) F5.
        uint32_t DFE_F4               : 4;         ///<BIT [7:4] Decision Feedback Equalization (DFE) F4.
        uint32_t DFE_F2               : 5;         ///<BIT [12:8] Decision Feedback Equalization (DFE) F2.
        uint32_t ADAPTED_DFE_RES      : 2;         ///<BIT [14:13] Adapted Decision Feedback Equalization (DFE) Resolution.
        uint32_t RF00C0030_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C0030_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} AdtDfeCoeff1_t;

///  @brief 0xF00C0034 : Generation 1 Setting 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G1_TX_SSC_EN         : 1;         ///<BIT [0] Gen1 TX SSC Enable.
        uint32_t G1_TX_AMP            : 5;         ///<BIT [5:1] Gen1 Transmitter Amplitude.
        uint32_t G1_TX_AMP_ADJ        : 1;         ///<BIT [6] Gen1 Transmitter Amplitude Range Select.
        uint32_t G1_TX_EMPH1          : 4;         ///<BIT [10:7] Gen1 Transmitter Post Cursor Value.
        uint32_t G1_TX_EMPH1_EN       : 1;         ///<BIT [11] Gen1 Transmitter Post Emphasis Enable.
        uint32_t G1_TX_SLEW_RATE_SEL  : 3;         ///<BIT [14:12] Gen1 Transmitter Slew Rate Select.
        uint32_t G1_TX_SLEW_CTRL_EN   : 1;         ///<BIT [15] Gen1 Transmitter Slew Control Enable.
        uint32_t RF00C0034_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Gen1Setting0_t;

///  @brief 0xF00C0038 : Generation 1 Setting 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G1_RX_SELMUPI        : 3;         ///<BIT [2:0] Gen1 Initial CDR Phase Loop Bandwidth.
        uint32_t G1_RX_SELMUPF        : 3;         ///<BIT [5:3] Gen1 Final CDR Phase Loop Bandwidth.
        uint32_t G1_RX_SELMUFI        : 2;         ///<BIT [7:6] Gen1 Initial CDR Frequency Loop Bandwidth.
        uint32_t G1_RX_SELMUFF        : 2;         ///<BIT [9:8] Gen1 Final CDR Frequency Loop Bandwidth.
        uint32_t G1_RX_DFE_EN         : 1;         ///<BIT [10] Gen1 RX DFE Enable.
        uint32_t RF00C0038_12_11_RSVD : 2;         ///<BIT [12:11] Reserved.
        uint32_t RF00C0038_13_RSVD    : 1;         ///<BIT [13] Reserved.
        uint32_t RF00C0038_14_RSVD    : 1;         ///<BIT [14] Reserved.
        uint32_t G1_TX_AMP_SHFT       : 1;         ///<BIT [15] Gen1 TX Amplitude Shift.
        uint32_t RF00C0038_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Gen1Setting1_t;

///  @brief 0xF00C003C : Generation 2 Setting 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G2_TX_SSC_EN         : 1;         ///<BIT [0] Gen2 TX SSC Enable.
        uint32_t G2_TX_AMP            : 5;         ///<BIT [5:1] Gen2 TX Amplitude.
        uint32_t G2_TX_AMP_ADJ        : 1;         ///<BIT [6] Gen2 TX Amplitude Range Select.
        uint32_t G2_TX_EMPH1          : 4;         ///<BIT [10:7] Gen2 TX Post Cursor Value.
        uint32_t G2_TX_EMPH1_EN       : 1;         ///<BIT [11] Gen2 TX Post Cursor Enable.
        uint32_t G2_TX_SLEW_RATE_SEL  : 3;         ///<BIT [14:12] Gen2 TX Slew Rate Select.
        uint32_t G2_TX_SLEW_CTRL_EN   : 1;         ///<BIT [15] Gen2 TX Slew Control Enable.
        uint32_t RF00C003C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Gen2Setting0_t;

///  @brief 0xF00C0040 : Generation 2 Setting 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G2_RX_SELMUPI        : 3;         ///<BIT [2:0] Gen2 Initial CDR Phase Loop Bandwidth.
        uint32_t G2_RX_SELMUPF        : 3;         ///<BIT [5:3] Gen2 Final CDR Phase Loop Bandwidth.
        uint32_t G2_RX_SELMUFI        : 2;         ///<BIT [7:6] Gen2 Initial CDR Frequency Loop Bandwidth.
        uint32_t G2_RX_SELMUFF        : 2;         ///<BIT [9:8] Gen2 Final CDR Frequency Loop Bandwidth.
        uint32_t G2_RX_DFE_EN         : 1;         ///<BIT [10] Gen2 RX DFE Enable.
        uint32_t RF00C0040_12_11_RSVD : 2;         ///<BIT [12:11] Reserved.
        uint32_t RF00C0040_13_RSVD    : 1;         ///<BIT [13] Reserved.
        uint32_t RF00C0040_14_RSVD    : 1;         ///<BIT [14] Reserved.
        uint32_t G2_TX_AMP_SHFT       : 1;         ///<BIT [15] Gen2 TX Amplitude Shift.
        uint32_t RF00C0040_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Gen2Setting1_t;

///  @brief 0xF00C0044 : Generation 3 Setting 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G3_TX_SSC_EN         : 1;         ///<BIT [0] Gen3 TX SSC Enable.
        uint32_t G3_TX_AMP            : 5;         ///<BIT [5:1] Gen3 TX Amplitude.
        uint32_t G3_TX_AMP_ADJ        : 1;         ///<BIT [6] Gen3 TX Amplitude Range Select.
        uint32_t G3_TX_EMPH1          : 4;         ///<BIT [10:7] Gen3 TX Post Cursor Value.
        uint32_t G3_TX_EMPH1_EN       : 1;         ///<BIT [11] Gen3 TX Post Cursor Enable.
        uint32_t G3_TX_SLEW_RATE_SEL  : 3;         ///<BIT [14:12] Gen3 TX Slew Rate Select.
        uint32_t G3_TX_SLEW_CTRL_EN   : 1;         ///<BIT [15] Gen3 TX Slew Control Enable.
        uint32_t RF00C0044_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Gen3Setting0_t;

///  @brief 0xF00C0048 : Generation 3 Setting 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G3_RX_SELMUPI        : 3;         ///<BIT [2:0] Gen3 Initial CDR Phase Loop Bandwidth.
        uint32_t G3_RX_SELMUPF        : 3;         ///<BIT [5:3] Gen3 Final CDR Phase Loop Bandwidth.
        uint32_t G3_RX_SELMUFI        : 2;         ///<BIT [7:6] Gen3 Initial CDR Frequency Loop Bandwidth.
        uint32_t G3_RX_SELMUFF        : 2;         ///<BIT [9:8] Gen3 Final CDR Frequency Loop Bandwidth.
        uint32_t G3_RX_DFE_EN         : 1;         ///<BIT [10] Gen3 RX DFE Enable.
        uint32_t RF00C0048_12_11_RSVD : 2;         ///<BIT [12:11] Reserved.
        uint32_t RF00C0048_13_RSVD    : 1;         ///<BIT [13] Reserved.
        uint32_t RF00C0048_14_RSVD    : 1;         ///<BIT [14] Reserved.
        uint32_t G3_TX_AMP_SHFT       : 1;         ///<BIT [15] Gen3 TX Amplitude Shift.
        uint32_t RF00C0048_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Gen3Setting1_t;

///  @brief 0xF00C004C : Generation 4 Setting 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G4_TX_SSC_EN         : 1;         ///<BIT [0] Gen4 TX SSC Enable.
        uint32_t G4_TX_AMP            : 5;         ///<BIT [5:1] Gen4 TX Amplitude.
        uint32_t G4_TX_AMP_ADJ        : 1;         ///<BIT [6] Gen4 TX Amplitude Range Select.
        uint32_t G4_TX_EMPH1          : 4;         ///<BIT [10:7] Gen4 TX Post Cursor Value.
        uint32_t G4_TX_EMPH1_EN       : 1;         ///<BIT [11] Gen4 TX Post Cursor Enable.
        uint32_t G4_TX_SLEW_RATE_SEL  : 3;         ///<BIT [14:12] Gen4 TX Slew Rate Select.
        uint32_t G4_TX_SLEW_CTRL_EN   : 1;         ///<BIT [15] Gen4 TX Slew Control Enable.
        uint32_t RF00C004C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Gen4Setting0_t;

///  @brief 0xF00C0050 : Generation 4 Setting 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G4_RX_SELMUPI        : 3;         ///<BIT [2:0] Gen4 Initial CDR Phase Loop Bandwidth.
        uint32_t G4_RX_SELMUPF        : 3;         ///<BIT [5:3] Gen4 Final CDR Phase Loop Bandwidth.
        uint32_t G4_RX_SELMUFI        : 2;         ///<BIT [7:6] Gen4 Initial CDR Frequency Loop Bandwidth.
        uint32_t G4_RX_SELMUFF        : 2;         ///<BIT [9:8] Gen4 Final CDR Frequency Loop Bandwidth.
        uint32_t G4_RX_DFE_EN         : 1;         ///<BIT [10] Gen4 RX DFE Enable.
        uint32_t RF00C0050_12_11_RSVD : 2;         ///<BIT [12:11] Reserved.
        uint32_t RF00C0050_13_RSVD    : 1;         ///<BIT [13] Reserved.
        uint32_t RF00C0050_14_RSVD    : 1;         ///<BIT [14] Reserved.
        uint32_t G4_TX_AMP_SHFT       : 1;         ///<BIT [15] Gen4 TX Amplitude Shift.
        uint32_t RF00C0050_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Gen4Setting1_t;

///  @brief 0xF00C0054 : PHY Test Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PT_LOCK_CNT          : 4;         ///<BIT [3:0] Pattern Lock Count.
        uint32_t PT_PATTERN_SEL       : 4;         ///<BIT [7:4] PHY Test Pattern Select.
        uint32_t PT_LONG_SHORT        : 1;         ///<BIT [8] PHY Test Long Patterns.
        uint32_t PT_START_RD          : 1;         ///<BIT [9] PHY Test Start Running Disparity.
        uint32_t PT_PATSYN_EN         : 1;         ///<BIT [10] PHY Test Pattern Sync Enable.
        uint32_t PT_XFER_DIFF         : 1;         ///<BIT [11] PHY Test Pattern Selection.
        uint32_t PT_LOCK              : 1;         ///<BIT [12] PHY Test Pattern Lock.
        uint32_t PT_PASS              : 1;         ///<BIT [13] PHY Test Pass.
        uint32_t PT_RST               : 1;         ///<BIT [14] PHY Test Reset.
        uint32_t PT_EN                : 1;         ///<BIT [15] PHY Test Enable.
        uint32_t RF00C0054_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyTstCtrl0_t;

///  @brief 0xF00C0058 : PHY Test Data 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PT_DATA_95_80        :16;         ///<BIT [15:0] PHY Test Data[95:80].
        uint32_t RF00C0058_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyTstData0_t;

///  @brief 0xF00C005C : PHY Test Data 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PT_DATA_79_64        :16;         ///<BIT [15:0] PHY Test Data[79:64].
        uint32_t RF00C005C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyTstData111_t;

///  @brief 0xF00C0060 : PHY Test Data 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PT_DATA_63_48        :16;         ///<BIT [15:0] PHY Test Data[63:48].
        uint32_t RF00C0060_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyTstData212_t;

///  @brief 0xF00C0064 : PHY Test Data 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PT_DATA_47_32        :16;         ///<BIT [15:0] PHY Test Data[47:32].
        uint32_t RF00C0064_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyTstData3_t;

///  @brief 0xF00C0068 : PHY Test Data 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PT_DATA_31_16        :16;         ///<BIT [15:0] PHY Test Data[31:16].
        uint32_t RF00C0068_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyTstData4_t;

///  @brief 0xF00C006C : PHY Test Data 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PT_DATA_15_0         :16;         ///<BIT [15:0] PHY Test Data[15:0].
        uint32_t RF00C006C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyTstData5_t;

///  @brief 0xF00C0070 : PHY Test PRBS Counter 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PT_PRBS_CNT_47_32    :16;         ///<BIT [15:0] PHY Test Pattern Count [47:32].
        uint32_t RF00C0070_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyTstPrbsCntr0_t;

///  @brief 0xF00C0074 : PHY Test PRBS Counter 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PT_PRBS_CNT_31_16    :16;         ///<BIT [15:0] PHY Test Pattern Count [31:16].
        uint32_t RF00C0074_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyTstPrbsCntr1_t;

///  @brief 0xF00C0078 : PHY Test PRBS Counter 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PT_PRBS_CNT_15_0     :16;         ///<BIT [15:0] PHY Test Pattern Count [15:0].
        uint32_t RF00C0078_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyTstPrbsCntr2_t;

///  @brief 0xF00C007C : PHY Test PRBS Error Counter 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PT_PRBS_ERR_CNT_31_16 :16;         ///<BIT [15:0] PHY Test Error Count [31:16].
        uint32_t RF00C007C_31_16_RSVD  :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyTstPrbsErrCntr0_t;

///  @brief 0xF00C0080 : PHY Test PRBS Error Counter 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PT_PRBS_ERR_CNT_15_0 :16;         ///<BIT [15:0] PHY Test Error Count [15:0].
        uint32_t RF00C0080_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyTstPrbsErrCntr1_t;

///  @brief 0xF00C0084 : PHY Test OOB 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0084_7_0_RSVD   : 8;         ///<BIT [7:0] Reserved.
        uint32_t RF00C0084_9_8_RSVD   : 2;         ///<BIT [9:8] Reserved.
        uint32_t RF00C0084_11_10_RSVD : 2;         ///<BIT [11:10] Reserved.
        uint32_t PT_TESTMODE          : 2;         ///<BIT [13:12] PHY Test Mode Selection.
        uint32_t PT_PHYREADY_FORCE    : 1;         ///<BIT [14] PHY Test PHY Ready Force.
        uint32_t RF00C0084_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C0084_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyTstOob0_t;

///  @brief 0xF00C0088 : Reserved_022h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0088_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0088_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd022h_t;

///  @brief 0xF00C008C : Digital Loopback Enable
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C008C_0_RSVD        : 1;         ///<BIT [0] Reserved.
        uint32_t SEL_BITS                : 3;         ///<BIT [3:1] Select Data Bit Width.
        uint32_t PLL_READY_TX            : 1;         ///<BIT [4] PLL Ready for TX.
        uint32_t PLL_READY_RX            : 1;         ///<BIT [5] PLL Ready for RX.
        uint32_t NORMAL_STATE_READY      : 1;         ///<BIT [6] Normal State Ready.
        uint32_t RF00C008C_9_7_RSVD      : 3;         ///<BIT [9:7] Reserved.
        uint32_t RF00C008C_10_RSVD       : 1;         ///<BIT [10] Reserved.
        uint32_t PT_CNT_RST              : 1;         ///<BIT [11] PHY Test Pattern and Error Counter Reset.
        uint32_t RXPHER_TO_TX_EN         : 1;         ///<BIT [12] RX Phase Offset to TX Enable.
        uint32_t LOCAL_ANA_TX2RX_LPBK_EN : 1;         ///<BIT [13] Local Analog TX to RX Loopback Enable.
        uint32_t LOCAL_DIG_TX2RX_LPBK_EN : 1;         ///<BIT [14] Local Parallel TX to RX Loopback Enable.
        uint32_t LOCAL_DIG_RX2TX_LPBK_EN : 1;         ///<BIT [15] Far End Parallel Loopback Enable Receiver to Transmitter.
        uint32_t RF00C008C_31_16_RSVD    :16;         ///<BIT [31:16] Reserved.
    } b;    
} DigLoopbackEn_t;

///  @brief 0xF00C0090 : Sync Pattern
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0090_9_0_RSVD   :10;         ///<BIT [9:0] Reserved.
        uint32_t TXD_INV              : 1;         ///<BIT [10] Transmitter Polarity Inversion.
        uint32_t RXD_INV              : 1;         ///<BIT [11] Receiver Polarity Inversion.
        uint32_t RF00C0090_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t RF00C0090_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} SyncPat_t;

///  @brief 0xF00C0094 : Sync Mask and Maximal Generation Setting
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0094_9_0_RSVD   :10;         ///<BIT [9:0] Reserved.
        uint32_t PHY_GEN_MAX          : 2;         ///<BIT [11:10] Maximum PHY Generation Setting.
        uint32_t RF00C0094_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t RF00C0094_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} SyncMskAndMaximalGenSetting_t;

///  @brief 0xF00C0098 : PHY Isolation Mode Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_GEN_RX           : 4;         ///<BIT [3:0] RX Generation Select.
        uint32_t PHY_GEN_TX           : 4;         ///<BIT [7:4] TX Generation Select.
        uint32_t TX_DRV_IDLE          : 1;         ///<BIT [8] TX Driver Idle.
        uint32_t RF00C0098_9_RSVD     : 1;         ///<BIT [9] Reserved.
        uint32_t RX_INIT              : 1;         ///<BIT [10] Receiver Initialization.
        uint32_t AUTO_RX_INIT_DIS     : 1;         ///<BIT [11] Automatic RX_INIT Disable.
        uint32_t RF00C0098_12_RSVD    : 1;         ///<BIT [12] Reserved.
        uint32_t REFCLK_ON_DCLK_DIS   : 1;         ///<BIT [13] Reference Clock on Data Clock Disable.
        uint32_t RF00C0098_14_RSVD    : 1;         ///<BIT [14] Reserved.
        uint32_t PHY_ISOLATE_MODE     : 1;         ///<BIT [15] PHY Isolate Mode.
        uint32_t RF00C0098_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PhyIsolatioModeCtrl_t;

///  @brief 0xF00C009C : EOM Circuit Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C009C_0_RSVD     : 1;         ///<BIT [0] Reserved.
        uint32_t RF00C009C_3_1_RSVD   : 3;         ///<BIT [3:1] Reserved.
        uint32_t EOM_SAM_VLD_TIME     : 3;         ///<BIT [6:4] EOM Valid Time.
        uint32_t RF00C009C_9_7_RSVD   : 3;         ///<BIT [9:7] Reserved.
        uint32_t EOM_MEASURE_STATE    : 2;         ///<BIT [11:10] EOM Measure State.
        uint32_t RF00C009C_12_RSVD    : 1;         ///<BIT [12] Reserved.
        uint32_t EOM_READY            : 1;         ///<BIT [13] EOM Ready.
        uint32_t RF00C009C_14_RSVD    : 1;         ///<BIT [14] Reserved.
        uint32_t EOM_EN               : 1;         ///<BIT [15] Eye-Opening Measurement (EOM) Circuit Enable Signal.
        uint32_t RF00C009C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomCircuitCtrl_t;

///  @brief 0xF00C00A0 : EOM Set 1 Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_VOLTAGE     : 6;         ///<BIT [5:0] EOM Voltage.
        uint32_t EOM_SET1_SIGN_BIT    : 2;         ///<BIT [7:6] EOM Sign Bits.
        uint32_t RF00C00A0_12_8_RSVD  : 5;         ///<BIT [12:8] Reserved.
        uint32_t EOM_SET1_VLD         : 1;         ///<BIT [13] EOM Valid.
        uint32_t EOM_SET1_STOP_EN     : 2;         ///<BIT [15:14] EOM Stop Mode Selection.
        uint32_t RF00C00A0_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1Ctrl0_t;

///  @brief 0xF00C00A4 : EOM Set 1 Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_PHASE       : 7;         ///<BIT [6:0] EOM Phase.
        uint32_t RF00C00A4_11_7_RSVD  : 5;         ///<BIT [11:7] Reserved.
        uint32_t EOM_SET1_STOP_STS    : 3;         ///<BIT [14:12] EOM Stop Status.
        uint32_t EOM_SET1_RDY         : 1;         ///<BIT [15] EOM Done for the Current Measurement.
        uint32_t RF00C00A4_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1Ctrl1_t;

///  @brief 0xF00C00A8 : EOM Set 1 Population Counter 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_POP_CNT_63_48 :16;         ///<BIT [15:0] EOM Population Threshold Counter.
        uint32_t RF00C00A8_31_16_RSVD   :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1PopCntr0_t;

///  @brief 0xF00C00AC : EOM Set 1 Population Counter 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_POP_CNT_47_32 :16;         ///<BIT [15:0] EOM Population Threshold Counter.
        uint32_t RF00C00AC_31_16_RSVD   :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1PopCntr1_t;

///  @brief 0xF00C00B0 : EOM Set 1 Population Counter 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_POP_CNT_31_16 :16;         ///<BIT [15:0] EOM Population Threshold Counter.
        uint32_t RF00C00B0_31_16_RSVD   :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1PopCntr2_t;

///  @brief 0xF00C00B4 : EOM Set 1 Population Counter 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_POP_CNT_15_0 :16;         ///<BIT [15:0] EOM Population Threshold Counter.
        uint32_t RF00C00B4_31_16_RSVD  :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1PopCntr3_t;

///  @brief 0xF00C00B8 : EOM Set 1 Threshold Counter 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_THRE_CNT_63_48 :16;         ///<BIT [15:0] EOM Error Threshold Counter.
        uint32_t RF00C00B8_31_16_RSVD    :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1ThrCntr0_t;

///  @brief 0xF00C00BC : EOM Set 1 Threshold Counter 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_THRE_CNT_47_32 :16;         ///<BIT [15:0] EOM Error Threshold Counter.
        uint32_t RF00C00BC_31_16_RSVD    :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1ThrCntr1_t;

///  @brief 0xF00C00C0 : EOM Set 1 Threshold Counter 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_THRE_CNT_31_16 :16;         ///<BIT [15:0] EOM Error Threshold Counter.
        uint32_t RF00C00C0_31_16_RSVD    :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1ThrCntr2_t;

///  @brief 0xF00C00C4 : EOM Set 1 Threshold Counter 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_THRE_CNT_15_0 :16;         ///<BIT [15:0] EOM Error Threshold Counter.
        uint32_t RF00C00C4_31_16_RSVD   :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1ThrCntr3_t;

///  @brief 0xF00C00C8 : EOM Set 1 Sample Counter 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_SAMPLE_CNT_63_48 :16;         ///<BIT [15:0] EOM Sample Counter.
        uint32_t RF00C00C8_31_16_RSVD      :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1SampleCntr0_t;

///  @brief 0xF00C00CC : EOM Set 1 Sample Counter 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_SAMPLE_CNT_47_32 :16;         ///<BIT [15:0] EOM Sample Counter.
        uint32_t RF00C00CC_31_16_RSVD      :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1SampleCntr1_t;

///  @brief 0xF00C00D0 : EOM Set 1 Sample Counter 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_SAMPLE_CNT_31_16 :16;         ///<BIT [15:0] EOM Sample Counter.
        uint32_t RF00C00D0_31_16_RSVD      :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1SampleCntr2_t;

///  @brief 0xF00C00D4 : EOM Set 1 Sample Counter 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_SAMPLE_CNT_15_0 :16;         ///<BIT [15:0] EOM Sample Counter.
        uint32_t RF00C00D4_31_16_RSVD     :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1SampleCntr3_t;

///  @brief 0xF00C00D8 : EOM Set 1 Error Counter 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_ERROR_CNT_63_48 :16;         ///<BIT [15:0] EOM Error Counter.
        uint32_t RF00C00D8_31_16_RSVD     :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1ErrCntr0_t;

///  @brief 0xF00C00DC : EOM Set 1 Error Counter 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_ERROR_CNT_47_32 :16;         ///<BIT [15:0] EOM Error Counter.
        uint32_t RF00C00DC_31_16_RSVD     :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1ErrCntr1_t;

///  @brief 0xF00C00E0 : EOM Set 1 Error Counter 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_ERROR_CNT_31_16 :16;         ///<BIT [15:0] EOM Error Counter.
        uint32_t RF00C00E0_31_16_RSVD     :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1ErrCntr2_t;

///  @brief 0xF00C00E4 : EOM Set 1 Error Counter 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EOM_SET1_ERROR_CNT_15_0 :16;         ///<BIT [15:0] EOM Error Counter.
        uint32_t RF00C00E4_31_16_RSVD    :16;         ///<BIT [31:16] Reserved.
    } b;    
} EomSet1ErrCntr3_t;

///  @brief 0xF00C00E8 : External Digital Reserved
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C00E8_15_0_RSVD  :16;         ///<BIT [15:0] Digital Reserved.
        uint32_t RF00C00E8_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} ExtDigRsvd_t;

///  @brief 0xF00C00EC : DFE Step Size 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DFE_STEP_COARSE_F0   : 4;         ///<BIT [3:0] Decision Feedback Equalization (DFE) Step Coarse for F0.
        uint32_t DFE_STEP_FINE_F0     : 4;         ///<BIT [7:4] Decision Feedback Equalization (DFE) Step Fine for F0.
        uint32_t DFE_STEP_FINE_EO_UP  : 4;         ///<BIT [11:8] DFE Step Fine for F0 Up Counting Weight.
        uint32_t DFE_STEP_FINE_EO_DN  : 4;         ///<BIT [15:12] DFE Step Fine for F0 Down Counting Weight.
        uint32_t RF00C00EC_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DfeStepSize0_t;

///  @brief 0xF00C00F0 : DFE Step Size 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DFE_STEP_ACCURATE_FX  : 4;         ///<BIT [3:0] DFE Step Accurate for F0.
        uint32_t DFE_STEP_ACCURATE_F0  : 4;         ///<BIT [7:4] DFE Step Fine for F0.
        uint32_t DFE_STEP_COARSE_EO_UP : 4;         ///<BIT [11:8] DFE Step Coarse for F0 Up Counting Weight.
        uint32_t DFE_STEP_COARSE_EO_DN : 4;         ///<BIT [15:12] DFE Step Coarse for F0 Down Counting Weight.
        uint32_t RF00C00F0_31_16_RSVD  :16;         ///<BIT [31:16] Reserved.
    } b;    
} DfeStepSize1_t;

///  @brief 0xF00C00F4 : Generation 1 Setting 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G1_TX_EMPH0           : 4;         ///<BIT [3:0] Gen1 TX Pre-Cursor Value.
        uint32_t G1_TX_EMPH0_EN        : 1;         ///<BIT [4] Gen1 TX Pre-Cursor Enable.
        uint32_t G1_TX_EMPH_MODE       : 2;         ///<BIT [6:5] Gen1 TX Emphasis Mode.
        uint32_t G1_VREG_RXTX_MAS_ISET : 2;         ///<BIT [8:7] Gen1 RX/TX Register Master Current Select.
        uint32_t G1_TX_SSC_AMP         : 7;         ///<BIT [15:9] Gen1 Spread Spectrum Clock Amplitude.
        uint32_t RF00C00F4_31_16_RSVD  :16;         ///<BIT [31:16] Reserved.
    } b;    
} Gen1Setting2_t;

///  @brief 0xF00C00F8 : Generation 2 Setting 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G2_TX_EMPH0           : 4;         ///<BIT [3:0] Gen2 TX Pre-Cursor Value.
        uint32_t G2_TX_EMPH0_EN        : 1;         ///<BIT [4] Gen2 TX Pre-Cursor Enable.
        uint32_t G2_TX_EMPH_MODE       : 2;         ///<BIT [6:5] Gen2 TX Emphasis Mode.
        uint32_t G2_VREG_RXTX_MAS_ISET : 2;         ///<BIT [8:7] Gen2 RX/TX Register Master Current Select.
        uint32_t G2_TX_SSC_AMP         : 7;         ///<BIT [15:9] Gen2 Spread Spectrum Clock Amplitude.
        uint32_t RF00C00F8_31_16_RSVD  :16;         ///<BIT [31:16] Reserved.
    } b;    
} Gen2Setting2_t;

///  @brief 0xF00C00FC : Generation 3 Setting 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G3_TX_EMPH0           : 4;         ///<BIT [3:0] Gen3 TX Pre-Cursor Value.
        uint32_t G3_TX_EMPH0_EN        : 1;         ///<BIT [4] Gen3 TX Pre-Cursor Enable.
        uint32_t G3_TX_EMPH_MODE       : 2;         ///<BIT [6:5] Gen3 TX Emphasis Mode.
        uint32_t G3_VREG_RXTX_MAS_ISET : 2;         ///<BIT [8:7] Gen3 RX/TX Register Master Current Select.
        uint32_t G3_TX_SSC_AMP         : 7;         ///<BIT [15:9] Gen3 Spread Spectrum Clock Amplitude.
        uint32_t RF00C00FC_31_16_RSVD  :16;         ///<BIT [31:16] Reserved.
    } b;    
} Gen3Setting2_t;

///  @brief 0xF00C0100 : Generation 4 Setting 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G4_TX_EMPH0           : 4;         ///<BIT [3:0] Gen4 TX Pre-Cursor Value.
        uint32_t G4_TX_EMPH0_EN        : 1;         ///<BIT [4] Gen4 TX Pre-Cursor Enable.
        uint32_t G4_TX_EMPH_MODE       : 2;         ///<BIT [6:5] Gen4 TX Emphasis Mode.
        uint32_t G4_VREG_RXTX_MAS_ISET : 2;         ///<BIT [8:7] Gen4 RX/TX Register Master Current Select.
        uint32_t G4_TX_SSC_AMP         : 7;         ///<BIT [15:9] Gen4 Spread Spectrum Clock Amplitude.
        uint32_t RF00C0100_31_16_RSVD  :16;         ///<BIT [31:16] Reserved.
    } b;    
} Gen4Setting2_t;

///  @brief 0xF00C0118 : Analog Reserved 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0118_15_0_RSVD  :16;         ///<BIT [15:0] Analog Reserved 0.
        uint32_t RF00C0118_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} AnalogRsvd1_t;

///  @brief 0xF00C011C : Analog Reserved 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C011C_15_0_RSVD  :16;         ///<BIT [15:0] Analog Reserved 1.
        uint32_t RF00C011C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} AnalogRsvd2_t;

///  @brief 0xF00C0124 : Lane Alignment Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0124_11_0_RSVD  :12;         ///<BIT [11:0] Reserved.
        uint32_t LANE_ALIGN_OFF       : 1;         ///<BIT [12] Lane Alignment Disable.
        uint32_t RF00C0124_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t RF00C0124_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} LaneAlignmenCtrl_t;

///  @brief 0xF00C013C : Miscellaneous Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ICP                  : 4;         ///<BIT [3:0] PLL Charge-Pump Current Control.
        uint32_t CLK100M_125M_EN      : 1;         ///<BIT [4] PIN_CLK100M_125M Clock Enable.
        uint32_t ICP_FORCE            : 1;         ///<BIT [5] ICP Force.
        uint32_t TXDCLK_2X_SEL        : 1;         ///<BIT [6] PIN_TXDCLK_2X Clock Frequency Selection.
        uint32_t CLK500M_EN           : 1;         ///<BIT [7] 500 MHz Clock Enable.
        uint32_t CLK100M_125M_SEL     : 1;         ///<BIT [8] PIN_CLK100M_125M Clock Frequency Selection.
        uint32_t RF00C013C_9_RSVD     : 1;         ///<BIT [9] Reserved.
        uint32_t REFCLK_SEL           : 1;         ///<BIT [10] Reference Clock Selection.
        uint32_t RF00C013C_12_11_RSVD : 2;         ///<BIT [12:11] Reserved.
        uint32_t RF00C013C_13_RSVD    : 1;         ///<BIT [13] Reserved.
        uint32_t RX_INIT_SPD_CHG_DIS  : 1;         ///<BIT [14] RX Initialization Speed Change Disable.
        uint32_t RF00C013C_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C013C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} MiscCtrl0_t;

///  @brief 0xF00C0140 : Reserved_050h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0140_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t RXCLK2X_SEL          : 1;         ///<BIT [11] PIN_RXDCLK_2X Source Selection.
        uint32_t CLK8T_EN             : 1;         ///<BIT [12] Enable RX and TX 8T Clock.
        uint32_t RF00C0140_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t RF00C0140_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd050h_t;

///  @brief 0xF00C0144 : Interface and Digital Reference Clock Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t REF1M_GEN_DIV        : 8;         ///<BIT [7:0] 1 MHz Clock Generation Divider.
        uint32_t REF1M_GEN_DIV_FORCE  : 1;         ///<BIT [8] 1 MHz Clock Generation Divider Force.
        uint32_t RF00C0144_9_RSVD     : 1;         ///<BIT [9] Reserved.
        uint32_t RF00C0144_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t RF00C0144_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} InterfacAndDigRefClkCtrl_t;

///  @brief 0xF00C0148 : Soft Reset
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0148_8_0_RSVD   : 9;         ///<BIT [8:0] Reserved.
        uint32_t SFT_RST              : 1;         ///<BIT [9] Soft Reset for PHY.
        uint32_t SFT_RST_NO_REG       : 1;         ///<BIT [10] Soft Reset For Internal Logic.
        uint32_t RF00C0148_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
        uint32_t RF00C0148_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} SoftRst_t;

///  @brief 0xF00C0158 : Speed Change Clock On
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0158_14_0_RSVD  :15;         ///<BIT [14:0] Reserved.
        uint32_t SPEED_CHANGE_CLK_ON  : 1;         ///<BIT [15] Speed Change Clock On.
        uint32_t RF00C0158_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} SpeedChangeClkOn_t;

///  @brief 0xF00C015C : Reserved_057h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C015C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C015C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd057h_t;

///  @brief 0xF00C0160 : Transmitter DCC Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0160_2_0_RSVD   : 3;         ///<BIT [2:0] Reserved.
        uint32_t TXDCC_EN             : 1;         ///<BIT [3] Transmitter DCC Circuit Enable.
        uint32_t RF00C0160_13_4_RSVD  :10;         ///<BIT [13:4] Reserved.
        uint32_t TXDCC_CAL_FREEZE     : 1;         ///<BIT [14] TX DCC Calibration Freeze.
        uint32_t TXDCC_CAL_STOP_SEL   : 1;         ///<BIT [15] TX Duty Cycle Correction (DCC) Calibration Stop Select.
        uint32_t RF00C0160_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TransmitDccCtrl_t;

///  @brief 0xF00C0170 : Reserved_05Ch
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0170_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0170_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd05ch_t;

///  @brief 0xF00C017C : SSC Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C017C_13_0_RSVD  :14;         ///<BIT [13:0] Reserved.
        uint32_t SSC_EN_FROM_PIN      : 1;         ///<BIT [14] SSC Enable Control From Pin.
        uint32_t RF00C017C_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C017C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} SscCtrl_t;

///  @brief 0xF00C0184 : DTL Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0184_1_0_RSVD   : 2;         ///<BIT [1:0] Reserved.
        uint32_t DTL_FLOOP_EN         : 1;         ///<BIT [2] DTL Frequency Loop Enable.
        uint32_t RF00C0184_15_3_RSVD  :13;         ///<BIT [15:3] Reserved.
        uint32_t RF00C0184_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DtlCtrl_t;

///  @brief 0xF00C019C : DFE Miscellaneous Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C019C_14_0_RSVD  :15;         ///<BIT [14:0] Reserved.
        uint32_t DFE_INIT_DIS         : 1;         ///<BIT [15] Decision Feedback Equalization (DFE) Initialization Disable.
        uint32_t RF00C019C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DfeMiscCtrl_t;

///  @brief 0xF00C01A8 : Fast DFE Timer Enable
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C01A8_12_0_RSVD  :13;         ///<BIT [12:0] Reserved.
        uint32_t FAST_DFE_TIMER_EN    : 1;         ///<BIT [13] Fast DFE Timer Enable.
        uint32_t RF00C01A8_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t RF00C01A8_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} FastDfeTmrEn_t;

///  @brief 0xF00C01AC : DFE Timer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C01AC_2_0_RSVD   : 3;         ///<BIT [2:0] Reserved.
        uint32_t TIMER_DFE2           : 3;         ///<BIT [5:3] DFE Timer 2.
        uint32_t RF00C01AC_8_6_RSVD   : 3;         ///<BIT [8:6] Reserved.
        uint32_t TIMER_ACQ            : 3;         ///<BIT [11:9] Timer for Acquisition.
        uint32_t RX_FAST_ACQ_SEL      : 2;         ///<BIT [13:12] RX Fast Acquisition Select.
        uint32_t RX_FAST_ACQ_EN       : 1;         ///<BIT [14] RX Fast Acquisition Enable.
        uint32_t DFE_INIT_REG         : 1;         ///<BIT [15] DFE Initial Register.
        uint32_t RF00C01AC_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DfeTmr_t;

///  @brief 0xF00C01C4 : PLL Calibration Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SPEED_PLL            : 6;         ///<BIT [5:0] SPEED_PLL Auto-Calibration Result.
        uint32_t RF00C01C4_15_6_RSVD  :10;         ///<BIT [15:6] Reserved.
        uint32_t RF00C01C4_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PllCalCtrl1_t;

///  @brief 0xF00C01C8 : Squelch Glitch Filter Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C01C8_12_0_RSVD  :13;         ///<BIT [12:0] Reserved.
        uint32_t SQ_GATE_RXDATA_EN    : 1;         ///<BIT [13] Squelch Gate RXDATA Enable.
        uint32_t RF00C01C8_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t RF00C01C8_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} SqlGlitchFilterCtrl_t;

///  @brief 0xF00C01CC : Miscellaneous Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BG_RDY               : 1;         ///<BIT [0] BG Ready.
        uint32_t RF00C01CC_15_1_RSVD  :15;         ///<BIT [15:1] Reserved.
        uint32_t RF00C01CC_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} MiscCtrl1_t;

///  @brief 0xF00C01D0 : Miscellaneous Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C01D0_2_0_RSVD   : 3;         ///<BIT [2:0] Reserved.
        uint32_t BG_RDY_FORCE         : 1;         ///<BIT [3] BG_RDY Force.
        uint32_t RF00C01D0_15_4_RSVD  :12;         ///<BIT [15:4] Reserved.
        uint32_t RF00C01D0_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} MiscCtrl2_t;

///  @brief 0xF00C01DC : 150M Clock Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C01DC_11_0_RSVD  :12;         ///<BIT [11:0] Reserved.
        uint32_t PU_PLL150M           : 1;         ///<BIT [12] Power-Up PLL 150M.
        uint32_t RF00C01DC_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t RF00C01DC_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} A150mClkCtrl2_t;

///  @brief 0xF00C0200 : Reserved_080h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0200_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0200_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd080h_t;

///  @brief 0xF00C0204 : ACJTAG Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0204_0_RSVD     : 1;         ///<BIT [0] Reserved.
        uint32_t RF00C0204_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t RF00C0204_2_RSVD     : 1;         ///<BIT [2] Reserved.
        uint32_t RF00C0204_3_RSVD     : 1;         ///<BIT [3] Reserved.
        uint32_t RF00C0204_6_4_RSVD   : 3;         ///<BIT [6:4] Reserved.
        uint32_t RF00C0204_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint32_t RF00C0204_8_RSVD     : 1;         ///<BIT [8] Reserved.
        uint32_t RF00C0204_15_9_RSVD  : 7;         ///<BIT [15:9] Reserved.
        uint32_t RF00C0204_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} AcjtagCtrl0_t;

///  @brief 0xF00C0208 : DFE F1P5 and F2P5 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DFE_F1P5_EXT         : 5;         ///<BIT [4:0] DFE F1P5 External.
        uint32_t DFE_F2P5_EXT         : 4;         ///<BIT [8:5] DFE F2P5 External.
        uint32_t DFE_FXP5_FORCE       : 2;         ///<BIT [10:9] DFE FXP5 Force.
        uint32_t RF00C0208_11_RSVD    : 1;         ///<BIT [11] Reserved.
        uint32_t RF00C0208_12_RSVD    : 1;         ///<BIT [12] Reserved.
        uint32_t RF00C0208_13_RSVD    : 1;         ///<BIT [13] Reserved.
        uint32_t RF00C0208_14_RSVD    : 1;         ///<BIT [14] Reserved.
        uint32_t FFE_OS_INV           : 1;         ///<BIT [15] Invert FFE OS.
        uint32_t RF00C0208_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DfeF1p5AndF2p5Ctrl_t;

///  @brief 0xF00C020C : FFE Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C020C_0_RSVD     : 1;         ///<BIT [0] Reserved.
        uint32_t FFE_OS_EXT           : 5;         ///<BIT [5:1] FFE Offset External.
        uint32_t RF00C020C_6_RSVD     : 1;         ///<BIT [6] Reserved.
        uint32_t RF00C020C_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint32_t EDGE_EQ_EN           : 1;         ///<BIT [8] Edge Equalizer Enable.
        uint32_t FFE_OS_FORCE         : 1;         ///<BIT [9] FFE Offset Force Control.
        uint32_t RF00C020C_10_RSVD    : 1;         ///<BIT [10] Reserved.
        uint32_t RF00C020C_11_RSVD    : 1;         ///<BIT [11] Reserved.
        uint32_t RF00C020C_12_RSVD    : 1;         ///<BIT [12] Reserved.
        uint32_t RF00C020C_13_RSVD    : 1;         ///<BIT [13] Reserved.
        uint32_t RF00C020C_14_RSVD    : 1;         ///<BIT [14] Reserved.
        uint32_t RF00C020C_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C020C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} FfeCtrl_t;

///  @brief 0xF00C0210 : Reserved_084h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0210_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0210_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd084h_t;

///  @brief 0xF00C022C : TRX Training Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RX_TRAIN_ENABLE      : 1;         ///<BIT [0] RX Training Enable.
        uint32_t TX_TRAIN_ENABLE      : 1;         ///<BIT [1] TX Training Enable.
        uint32_t RF00C022C_2_RSVD     : 1;         ///<BIT [2] Reserved.
        uint32_t RF00C022C_3_RSVD     : 1;         ///<BIT [3] Reserved.
        uint32_t RF00C022C_4_RSVD     : 1;         ///<BIT [4] Reserved.
        uint32_t RF00C022C_5_RSVD     : 1;         ///<BIT [5] Reserved.
        uint32_t RF00C022C_6_RSVD     : 1;         ///<BIT [6] Reserved.
        uint32_t RX_NO_INIT           : 1;         ///<BIT [7] RX No Initialize.
        uint32_t TX_NO_INIT           : 1;         ///<BIT [8] TX No Initialize.
        uint32_t RF00C022C_9_RSVD     : 1;         ///<BIT [9] Reserved.
        uint32_t RF00C022C_10_RSVD    : 1;         ///<BIT [10] Reserved.
        uint32_t RX_TEST_FFE_MODE     : 1;         ///<BIT [11] RX Test FFE Mode.
        uint32_t RF00C022C_13_12_RSVD : 2;         ///<BIT [13:12] Reserved.
        uint32_t RF00C022C_14_RSVD    : 1;         ///<BIT [14] Reserved.
        uint32_t RF00C022C_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C022C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TrxTrainingCtrl0_t;

///  @brief 0xF00C0230 : TRX Training Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0230_12_0_RSVD     :13;         ///<BIT [12:0] Reserved.
        uint32_t PIN_TX_TRAIN_ENABLE_SEL : 1;         ///<BIT [13] PIN_TX_TRAIN_ENABLE Select.
        uint32_t RF00C0230_14_RSVD       : 1;         ///<BIT [14] Reserved.
        uint32_t PIN_TRAIN_COMPLETE_TYPE : 1;         ///<BIT [15] Pin Training Complete Type.
        uint32_t RF00C0230_31_16_RSVD    :16;         ///<BIT [31:16] Reserved.
    } b;    
} TrxTrainingCtrl1_t;

///  @brief 0xF00C0234 : Reserved_08Dh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0234_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0234_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd08dh_t;

///  @brief 0xF00C0238 : Reserved_08Eh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0238_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0238_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd08eh_t;

///  @brief 0xF00C023C : Reserved_08Fh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C023C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C023C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd08fh_t;

///  @brief 0xF00C0240 : RX Training Control 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0240_1_0_RSVD        : 2;         ///<BIT [1:0] Reserved.
        uint32_t CDRPHASE_OPT_ONLY_IN_LAST : 1;         ///<BIT [2] CDR Phase Optimization Only In Last.
        uint32_t CDRPHASE_OPT_EN           : 1;         ///<BIT [3] CDR Phase Optimization Enable.
        uint32_t RF00C0240_15_4_RSVD       :12;         ///<BIT [15:4] Reserved.
        uint32_t RF00C0240_31_16_RSVD      :16;         ///<BIT [31:16] Reserved.
    } b;    
} RxTrainingCtrl3_t;

///  @brief 0xF00C0244 : Reserved_091h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0244_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0244_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd091h_t;

///  @brief 0xF00C0248 : Reserved_092h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0248_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0248_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd092h_t;

///  @brief 0xF00C024C : Reserved_093h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C024C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C024C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd093h_t;

///  @brief 0xF00C0250 : Reserved_094h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0250_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0250_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd094h_t;

///  @brief 0xF00C0254 : Reserved_095h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0254_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0254_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd095h_t;

///  @brief 0xF00C0258 : Reserved_096h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0258_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0258_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd096h_t;

///  @brief 0xF00C025C : Reserved_097h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C025C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C025C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd097h_t;

///  @brief 0xF00C0260 : Reserved_098h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0260_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0260_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd098h_t;

///  @brief 0xF00C0264 : Reserved_099h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0264_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0264_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd099h_t;

///  @brief 0xF00C0268 : TX Training Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0268_14_0_RSVD  :15;         ///<BIT [14:0] Reserved.
        uint32_t TX_TRAIN_P2P_HOLD    : 1;         ///<BIT [15] Transmitter Training Peak-To-Peak Hold Enable.
        uint32_t RF00C0268_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TxTrainingCtrl0_t;

///  @brief 0xF00C026C : Reserved_09Bh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C026C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C026C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd09bh_t;

///  @brief 0xF00C0270 : TX Training Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0270_14_0_RSVD  :15;         ///<BIT [14:0] Reserved.
        uint32_t EYE_CHECK_DISABLE    : 1;         ///<BIT [15] Eye Check Disable.
        uint32_t RF00C0270_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TxTrainingCtrl2_t;

///  @brief 0xF00C0278 : TX Training Control 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TRX_TRAIN_TIMER      :10;         ///<BIT [9:0] TRX Training Timer.
        uint32_t TRX_TRAIN_TIMEOUT_EN : 1;         ///<BIT [10] TRX Training Timeout Enable.
        uint32_t RF00C0278_14_11_RSVD : 4;         ///<BIT [14:11] Reserved.
        uint32_t RF00C0278_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C0278_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TxTrainingCtrl4_t;

///  @brief 0xF00C027C : Reserved_09Fh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C027C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C027C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd09fh_t;

///  @brief 0xF00C0280 : Reserved_0A0h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0280_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0280_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0a0h_t;

///  @brief 0xF00C0284 : Reserved_0A1h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0284_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0284_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0a1h_t;

///  @brief 0xF00C0288 : Reserved_0A2h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0288_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0288_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0a2h_t;

///  @brief 0xF00C0290 : Reserved_0A4h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0290_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0290_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0a4h_t;

///  @brief 0xF00C0294 : TX Amplitude Adjustment and TX Emphasis Enable
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0294_3_0_RSVD   : 4;         ///<BIT [3:0] Reserved.
        uint32_t TX_EMPH1_MAX         : 4;         ///<BIT [7:4] Transmitter Emphasis 1 Maximum.
        uint32_t TX_EMPH0_MAX         : 4;         ///<BIT [11:8] Transmitter Emphasis 0 Maximum.
        uint32_t RF00C0294_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t RF00C0294_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TxAmpAdjustmeAndTxEmphasisEn_t;

///  @brief 0xF00C029C : TX Amplitude Index Limit
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TX_AMP_MAX           : 6;         ///<BIT [5:0] Transmitter Amplitude Maximum.
        uint32_t TX_AMP_MIN           : 6;         ///<BIT [11:6] Transmitter Amplitude Minimum.
        uint32_t RF00C029C_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t RF00C029C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TxAmpIndexLimit_t;

///  @brief 0xF00C02A0 : TX Emphasis Index Minimum Value
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TX_EMPH0_MIN             : 4;         ///<BIT [3:0] Transmitter Emphasis 0 Minimum.
        uint32_t TX_EMPH1_MIN             : 4;         ///<BIT [7:4] Transmitter Emphasis 1 Minimum.
        uint32_t TX_TRAIN_START_WAIT_TIME : 2;         ///<BIT [9:8] Transmitter Training Start Wait Time.
        uint32_t RF00C02A0_15_10_RSVD     : 6;         ///<BIT [15:10] Reserved.
        uint32_t RF00C02A0_31_16_RSVD     :16;         ///<BIT [31:16] Reserved.
    } b;    
} TxEmphasisIndexMinVal_t;

///  @brief 0xF00C02A4 : TRX Training Control 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RX_TRAIN_TIMER              :10;         ///<BIT [9:0] Receiver Training Timer.
        uint32_t TX_TRAIN_START_NO_WAIT      : 1;         ///<BIT [10] Transmitter Training Start No Wait.
        uint32_t TX_TRAIN_START_SQ_EN        : 1;         ///<BIT [11] Transmitter Training Start SQ Enable.
        uint32_t RF00C02A4_13_12_RSVD        : 2;         ///<BIT [13:12] Reserved.
        uint32_t TX_TRAIN_START_WAIT_TIME_EN : 1;         ///<BIT [14] Transmitter Training Start Wait Time Enable.
        uint32_t RF00C02A4_15_RSVD           : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C02A4_31_16_RSVD        :16;         ///<BIT [31:16] Reserved.
    } b;    
} TrxTrainingCtrl5_t;

///  @brief 0xF00C02AC : Interrupt Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TX_TRAIN_COMPLETE_INT  : 1;         ///<BIT [0] TX Training Complete Interrupt.
        uint32_t RX_TRAIN_COMPLETE_INT  : 1;         ///<BIT [1] RX Training Complete Interrupt.
        uint32_t STATUS_DET_TIMEOUT_INT : 1;         ///<BIT [2] Status Detection Timeout Interrupt.
        uint32_t DFE_DONE_INT           : 1;         ///<BIT [3] DFE Done Interrupt.
        uint32_t TRX_TRAIN_DONE_INT     : 1;         ///<BIT [4] TX/RX Training Done Interrupt.
        uint32_t TRX_TRAIN_TIMEOUT_INT  : 1;         ///<BIT [5] TX/RX Training Timeout Interrupt.
        uint32_t TRX_TRAIN_FAILED_INT   : 1;         ///<BIT [6] TX/RX Training Failed Interrupt.
        uint32_t RF00C02AC_15_7_RSVD    : 9;         ///<BIT [15:7] Reserved.
        uint32_t RF00C02AC_31_16_RSVD   :16;         ///<BIT [31:16] Reserved.
    } b;    
} IntrCtrl1_t;

///  @brief 0xF00C02B4 : Interrupt Control 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TX_TRAIN_COMPLETE_INT_EN  : 1;         ///<BIT [0] TX Training Complete Interrupt Enable.
        uint32_t RX_TRAIN_COMPLETE_INT_EN  : 1;         ///<BIT [1] RX Training Complete Interrupt Enable.
        uint32_t STATUS_DET_TIMEOUT_INT_EN : 1;         ///<BIT [2] Status Detection Timeout Interrupt Enable.
        uint32_t DFE_DONE_INT_EN           : 1;         ///<BIT [3] DFE Done Interrupt Enable.
        uint32_t TRX_TRAIN_DONE_INT_EN     : 1;         ///<BIT [4] TRX Training Done Interrupt Enable.
        uint32_t TRX_TRAIN_TIMEOUT_INT_EN  : 1;         ///<BIT [5] TRX Training Timeout Interrupt Enable.
        uint32_t TRX_TRAIN_FAILED_INT_EN   : 1;         ///<BIT [6] TRX Training Failed Interrupt Enable.
        uint32_t RF00C02B4_15_7_RSVD       : 9;         ///<BIT [15:7] Reserved.
        uint32_t RF00C02B4_31_16_RSVD      :16;         ///<BIT [31:16] Reserved.
    } b;    
} IntrCtrl3_t;

///  @brief 0xF00C02BC : Interrupt Control 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TX_TRAIN_COMPLETE_INT_CLEAR  : 1;         ///<BIT [0] TX Training Complete Interrupt Clear.
        uint32_t RX_TRAIN_COMPLETE_INT_CLEAR  : 1;         ///<BIT [1] RX Training Complete Interrupt Clear.
        uint32_t STATUS_DET_TIMEOUT_INT_CLEAR : 1;         ///<BIT [2] Status Detection Timeout Interrupt Clear.
        uint32_t DFE_DONE_INT_CLEAR           : 1;         ///<BIT [3] DFE Done Interrupt Clear.
        uint32_t TRX_TRAIN_DONE_INT_CLEAR     : 1;         ///<BIT [4] TRX Training Done Interrupt Clear.
        uint32_t TRX_TRAIN_TIMEOUT_INT_CLEAR  : 1;         ///<BIT [5] TRX Training Timeout Interrupt Clear.
        uint32_t TRX_TRAIN_FAILED_INT_CLEAR   : 1;         ///<BIT [6] TX/RX Training Failed Interrupt Clear.
        uint32_t RF00C02BC_15_7_RSVD          : 9;         ///<BIT [15:7] Reserved.
        uint32_t RF00C02BC_31_16_RSVD         :16;         ///<BIT [31:16] Reserved.
    } b;    
} IntrCtrl5_t;

///  @brief 0xF00C02DC : Reserved_0B7h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C02DC_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C02DC_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0b7h_t;

///  @brief 0xF00C02E0 : Value of Signals to Analog Selection
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C02E0_7_0_RSVD   : 8;         ///<BIT [7:0] Reserved.
        uint32_t ANA_VIRTUAL_CTRL     : 8;         ///<BIT [15:8] Analog Virtual Control.
        uint32_t RF00C02E0_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} ValOfSignalsToAnalogSelectio_t;

///  @brief 0xF00C02EC : Reserved_0BBh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C02EC_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C02EC_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0bbh_t;

///  @brief 0xF00C0308 : Reserved_0C2h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0308_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0308_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0c2h_t;

///  @brief 0xF00C030C : Reserved_0C3h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C030C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C030C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0c3h_t;

///  @brief 0xF00C0310 : Reserved_0C4h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0310_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0310_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0c4h_t;

///  @brief 0xF00C0314 : TX Training Result 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TRX_TRAIN_TIMEOUT    : 1;         ///<BIT [0] TRX Training Timeout.
        uint32_t TX_TRAIN_ERROR       : 2;         ///<BIT [2:1] TX Training Error.
        uint32_t RX_TRAIN_FAILED      : 1;         ///<BIT [3] Receiver Training Failed.
        uint32_t RX_TRAIN_COMPLETE    : 1;         ///<BIT [4] Receiver Training Complete.
        uint32_t TX_TRAIN_FAILED      : 1;         ///<BIT [5] Transmitter Training Failed.
        uint32_t TX_TRAIN_COMPLETE    : 1;         ///<BIT [6] Transmitter Training Complete.
        uint32_t RF00C0314_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint32_t RF00C0314_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t RF00C0314_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TxTrainingResult1_t;

///  @brief 0xF00C0318 : Adapted Edge Result
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0318_0_RSVD     : 1;         ///<BIT [0] Reserved.
        uint32_t FFE_OS               : 5;         ///<BIT [5:1] FFE Offset.
        uint32_t DFE_F2P5             : 4;         ///<BIT [9:6] DFE F2P5.
        uint32_t DFE_F1P5             : 5;         ///<BIT [14:10] DFE F1P5.
        uint32_t RF00C0318_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C0318_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} AdtEdgeResult_t;

///  @brief 0xF00C031C : TX Training Preset
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C031C_6_0_RSVD     : 7;         ///<BIT [6:0] Reserved.
        uint32_t TX_COE_FM_PIN_PCIE3_EN : 1;         ///<BIT [7] TX Coefficient from Pin Enable For PCIe3 Mode.
        uint32_t RF00C031C_15_8_RSVD    : 8;         ///<BIT [15:8] Reserved.
        uint32_t RF00C031C_31_16_RSVD   :16;         ///<BIT [31:16] Reserved.
    } b;    
} TxTrainingPreset_t;

///  @brief 0xF00C0320 : Reserved_0C8h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0320_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0320_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0c8h_t;

///  @brief 0xF00C0324 : Reserved_0C9h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0324_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0324_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0c9h_t;

///  @brief 0xF00C0328 : Reserved_0CAh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0328_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0328_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0cah_t;

///  @brief 0xF00C032C : Reserved_0CBh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C032C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C032C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0cbh_t;

///  @brief 0xF00C0330 : RX Training Default Value
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0330_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0330_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} RxTrainingDefaultVal_t;

///  @brief 0xF00C0334 : Reserved_0CDh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0334_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0334_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0cdh_t;

///  @brief 0xF00C0338 : Reserved_0CEh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0338_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0338_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0ceh_t;

///  @brief 0xF00C033C : Reserved_0CFh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C033C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C033C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0cfh_t;

///  @brief 0xF00C0340 : Reserved_0D0h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0340_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0340_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0d0h_t;

///  @brief 0xF00C0348 : Reserved_0D2h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0348_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0348_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0d2h_t;

///  @brief 0xF00C034C : Reserved_0D3h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C034C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C034C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0d3h_t;

///  @brief 0xF00C0350 : Reserved_0D4h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0350_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0350_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0d4h_t;

///  @brief 0xF00C035C : Reserved_0D7h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C035C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C035C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0d7h_t;

///  @brief 0xF00C0360 : Reserved_0D8h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0360_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0360_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0d8h_t;

///  @brief 0xF00C0364 : Reserved_0D9h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0364_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0364_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0d9h_t;

///  @brief 0xF00C036C : Reserved_0DBh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C036C_8_0_RSVD    : 9;         ///<BIT [8:0] Reserved.
        uint32_t DFE_RES_TRAIN_INIT_EN : 1;         ///<BIT [9] DFE Resolution Training Enable During TRX Training Initializati.
        uint32_t RF00C036C_10_RSVD     : 1;         ///<BIT [10] Reserved.
        uint32_t DFE_RES_TRAIN_END_EN  : 1;         ///<BIT [11] DFE Resolution Training Enable During TRX Training End State.
        uint32_t RF00C036C_15_12_RSVD  : 4;         ///<BIT [15:12] Reserved.
        uint32_t RF00C036C_31_16_RSVD  :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0dbh_t;

///  @brief 0xF00C0370 : Reserved_0DCh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0370_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0370_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0dch_t;

///  @brief 0xF00C0374 : Reserved_0DDh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0374_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0374_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0ddh_t;

///  @brief 0xF00C0378 : Reserved_0DEh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0378_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0378_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0deh_t;

///  @brief 0xF00C037C : Reserved_0DFh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C037C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C037C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0dfh_t;

///  @brief 0xF00C0380 : Reserved_0E0h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0380_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0380_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0e0h_t;

///  @brief 0xF00C0384 : Reserved_0E1h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0384_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0384_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0e1h_t;

///  @brief 0xF00C0388 : Reserved_0E2h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0388_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0388_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0e2h_t;

///  @brief 0xF00C038C : Reserved_0E3h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C038C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C038C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0e3h_t;

///  @brief 0xF00C0390 : Reserved_0E4h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0390_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C0390_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0e4h_t;

///  @brief 0xF00C039C : Reserved_0E7h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C039C_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C039C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0e7h_t;

///  @brief 0xF00C03A0 : Reserved_0E8h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C03A0_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C03A0_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0e8h_t;

///  @brief 0xF00C03A4 : Reserved_0E9h
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C03A4_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C03A4_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0e9h_t;

///  @brief 0xF00C03A8 : Reserved_0EAh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C03A8_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C03A8_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0eah_t;

///  @brief 0xF00C03AC : Reserved_0EBh
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C03AC_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF00C03AC_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Rsvd0ebh_t;

///  @brief 0xF00C03B0 : DFE Step Size
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DFE_STEP_FINE_OS     : 4;         ///<BIT [3:0] DFE Fine Step For FFE_OS Measurement.
        uint32_t DFE_STEP_COARSE_OS   : 4;         ///<BIT [7:4] DFE Coarse Step For FFE_OS Measurement.
        uint32_t DFE_STEP_FINE_FXP5   : 4;         ///<BIT [11:8] DFE Fine Step For F1P5 And F2P5 Measurement.
        uint32_t DFE_STEP_COARSE_FXP5 : 4;         ///<BIT [15:12] DFE Coarse Step For F1P5 And F2P5 Measurement.
        uint32_t RF00C03B0_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DfeStepSize_t;

///  @brief 0xF00C03E4 : Squelch Detector Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C03E4_8_0_RSVD   : 9;         ///<BIT [8:0] Reserved.
        uint32_t SQ_HIRANGE           : 1;         ///<BIT [9] Squelch Vth Range Control.
        uint32_t RF00C03E4_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t RF00C03E4_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} SqlDetCtrl_t;

///  @brief 0xF00C0418 : CDR Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0418_5_0_RSVD   : 6;         ///<BIT [5:0] Reserved.
        uint32_t CDR_MAX_DFE_ADAPT_1  : 3;         ///<BIT [8:6] Phase Training DFE Adaptation Average if Tx Training is Enabled.
        uint32_t CDR_MAX_DFE_ADAPT_0  : 3;         ///<BIT [11:9] Phase Training DFE Adaptation Average if Tx Training is Disable.
        uint32_t RX_MAX_DFE_ADAPT_1   : 2;         ///<BIT [13:12] Rx Training DFE Adaptation Average if Tx Training is Enabled.
        uint32_t RX_MAX_DFE_ADAPT_0   : 2;         ///<BIT [15:14] Rx Training DFE Adaptation Average if Tx Training is Disabled.
        uint32_t RF00C0418_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} CdrCtrl0_t;

///  @brief 0xF00C0440 : G1 Setting 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G1_FFE_CAP_SEL            : 4;         ///<BIT [3:0] G1 Feed Forward Equalization (FFE) Capacitor Select.
        uint32_t G1_FFE_RES_SEL            : 3;         ///<BIT [6:4] G1 Feed Forward Equalization (FFE) Resistor Select.
        uint32_t G1_FFE_SETTING_FORCE      : 1;         ///<BIT [7] G1 Feed Forward Equalization (FFE) Setting Force.
        uint32_t G1_DFE_FREEZE_AFTER_TRAIN : 1;         ///<BIT [8] G1 Freeze DFE after TX/RX training.
        uint32_t G1_FBCK_SEL               : 1;         ///<BIT [9] G1 Feedback Select.
        uint32_t RF00C0440_11_10_RSVD      : 2;         ///<BIT [11:10] Reserved.
        uint32_t G1_FFE_DEG_RES_LEVEL      : 2;         ///<BIT [13:12] FFE Degeneration Resistor Tuning for Different Bandwidth.
        uint32_t G1_FFE_LOAD_RES_LEVEL     : 2;         ///<BIT [15:14] FFE Loading Resistor Tuning for Different Bandwidth.
        uint32_t RF00C0440_31_16_RSVD      :16;         ///<BIT [31:16] Reserved.
    } b;    
} G1Setting3_t;

///  @brief 0xF00C0444 : G1 Setting 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0444_2_0_RSVD   : 3;         ///<BIT [2:0] Reserved.
        uint32_t G1_SQ_THRESH_RATIO   : 5;         ///<BIT [7:3] G1 Squelch Detector Threshold ratio.
        uint32_t G1_DFE_RES           : 2;         ///<BIT [9:8] G1 Decision Feedback Equalization (DFE) Resolution.
        uint32_t RF00C0444_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t RF00C0444_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} G1Setting4_t;

///  @brief 0xF00C0448 : G2 Setting 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G2_FFE_CAP_SEL            : 4;         ///<BIT [3:0] G2 Feed Forward Equalization (FFE) Capacitor Select.
        uint32_t G2_FFE_RES_SEL            : 3;         ///<BIT [6:4] G2 Feed Forward Equalization (FFE) Resistor Select.
        uint32_t G2_FFE_SETTING_FORCE      : 1;         ///<BIT [7] G2 Feed Forward Equalization (FFE) Setting Force.
        uint32_t G2_DFE_FREEZE_AFTER_TRAIN : 1;         ///<BIT [8] G2 Freeze DFE after TX/RX training.
        uint32_t G2_FBCK_SEL               : 1;         ///<BIT [9] G2 Feedback Select.
        uint32_t RF00C0448_11_10_RSVD      : 2;         ///<BIT [11:10] Reserved.
        uint32_t G2_FFE_DEG_RES_LEVEL      : 2;         ///<BIT [13:12] G2 FFE Degeneration Resistor Tuning for Different Bandwidth.
        uint32_t G2_FFE_LOAD_RES_LEVEL     : 2;         ///<BIT [15:14] G2 FFE Loading Resistor Tuning for Different Bandwidth.
        uint32_t RF00C0448_31_16_RSVD      :16;         ///<BIT [31:16] Reserved.
    } b;    
} G2Setting3_t;

///  @brief 0xF00C044C : G2 Setting 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C044C_2_0_RSVD   : 3;         ///<BIT [2:0] Reserved.
        uint32_t G2_SQ_THRESH_RATIO   : 5;         ///<BIT [7:3] G2 Squelch Detector Threshold ratio.
        uint32_t G2_DFE_RES           : 2;         ///<BIT [9:8] G2 Decision Feedback Equalization (DFE) Resolution.
        uint32_t RF00C044C_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t RF00C044C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} G2Setting4_t;

///  @brief 0xF00C0450 : G3 Setting 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G3_FFE_CAP_SEL            : 4;         ///<BIT [3:0] G3 Feed Forward Equalization (FFE) Capacitor Select.
        uint32_t G3_FFE_RES_SEL            : 3;         ///<BIT [6:4] G3 Feed Forward Equalization (FFE) Resistor Select.
        uint32_t G3_FFE_SETTING_FORCE      : 1;         ///<BIT [7] G3 Feed Forward Equalization (FFE) Setting Force.
        uint32_t G3_DFE_FREEZE_AFTER_TRAIN : 1;         ///<BIT [8] G3 Freeze DFE after TX/RX training.
        uint32_t G3_FBCK_SEL               : 1;         ///<BIT [9] G3 Feedback Select.
        uint32_t RF00C0450_11_10_RSVD      : 2;         ///<BIT [11:10] Reserved.
        uint32_t G3_FFE_DEG_RES_LEVEL      : 2;         ///<BIT [13:12] G3 FFE Degeneration Resistor Tuning for Different Bandwidth.
        uint32_t G3_FFE_LOAD_RES_LEVEL     : 2;         ///<BIT [15:14] G3 FFE Loading Resistor Tuning for Different Bandwidth.
        uint32_t RF00C0450_31_16_RSVD      :16;         ///<BIT [31:16] Reserved.
    } b;    
} G3Setting3_t;

///  @brief 0xF00C0454 : G3 Setting 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0454_2_0_RSVD   : 3;         ///<BIT [2:0] Reserved.
        uint32_t G3_SQ_THRESH_RATIO   : 5;         ///<BIT [7:3] G3 Squelch Detector Threshold ratio.
        uint32_t G3_DFE_RES           : 2;         ///<BIT [9:8] G3 Decision Feedback Equalization (DFE) Resolution.
        uint32_t RF00C0454_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t RF00C0454_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} G3Setting4_t;

///  @brief 0xF00C0458 : G4 Setting 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t G4_FFE_CAP_SEL            : 4;         ///<BIT [3:0] G4 Feed Forward Equalization (FFE) Capacitor Select.
        uint32_t G4_FFE_RES_SEL            : 3;         ///<BIT [6:4] G4 Feed Forward Equalization (FFE) Resistor Select.
        uint32_t G4_FFE_SETTING_FORCE      : 1;         ///<BIT [7] G4 Feed Forward Equalization (FFE) Setting Force.
        uint32_t G4_DFE_FREEZE_AFTER_TRAIN : 1;         ///<BIT [8] G4 Freeze DFE after TX/RX training.
        uint32_t G4_FBCK_SEL               : 1;         ///<BIT [9] G4 Feedback Select.
        uint32_t RF00C0458_11_10_RSVD      : 2;         ///<BIT [11:10] Reserved.
        uint32_t G4_FFE_DEG_RES_LEVEL      : 2;         ///<BIT [13:12] G4 FFE Degeneration Resistor Tuning for Different Bandwidth.
        uint32_t G4_FFE_LOAD_RES_LEVEL     : 2;         ///<BIT [15:14] G4 FFE Loading Resistor Tuning for Different Bandwidth.
        uint32_t RF00C0458_31_16_RSVD      :16;         ///<BIT [31:16] Reserved.
    } b;    
} G4Setting3_t;

///  @brief 0xF00C045C : G4 Setting 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C045C_2_0_RSVD   : 3;         ///<BIT [2:0] Reserved.
        uint32_t G4_SQ_THRESH_RATIO   : 5;         ///<BIT [7:3] G4 Squelch Detector Threshold ratio.
        uint32_t G4_DFE_RES           : 2;         ///<BIT [9:8] G4 Decision Feedback Equalization (DFE) Resolution.
        uint32_t RF00C045C_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t RF00C045C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} G4Setting4_t;

///  @brief 0xF00C0468 : PCIE TX Preset Index
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TX_PRESET_INDEX      : 4;         ///<BIT [3:0] TX Preset Index.
        uint32_t RF00C0468_15_4_RSVD  :12;         ///<BIT [15:4] Reserved.
        uint32_t RF00C0468_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PcieTxPresetIndex_t;

///  @brief 0xF00C046C : TX Power Protection Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C046C_7_0_RSVD   : 8;         ///<BIT [7:0] Reserved.
        uint32_t TX_POWER_MAX         : 6;         ///<BIT [13:8] TX Maximum Power.
        uint32_t TX_POWER_PROTECT_EN  : 1;         ///<BIT [14] TX Power Protection Enable.
        uint32_t RF00C046C_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C046C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TxPowerProtectiCtrl_t;

///  @brief 0xF00C0470 : DFE Control 27
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0470_13_0_RSVD  :14;         ///<BIT [13:0] Reserved.
        uint32_t TX_MAX_DFE_ADAPT     : 2;         ///<BIT [15:14] Number of DFE Adaptation Average in Tx Training.
        uint32_t RF00C0470_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DfeCtrl27_t;

///  @brief 0xF00C0498 : PCIE RX Preset Hint 7
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0498_3_0_RSVD   : 4;         ///<BIT [3:0] Reserved.
        uint32_t TX_AMP_OFFSET        : 6;         ///<BIT [9:4] TX Amplitude Offset for PCIE Gen 3.
        uint32_t RF00C0498_10_RSVD    : 1;         ///<BIT [10] Reserved.
        uint32_t TX_VMA_PROTECT_EN    : 1;         ///<BIT [11] Tx Voltage Modulation Amplitude Protection Enable.
        uint32_t TX_VMA_MIN           : 4;         ///<BIT [15:12] Tx Minimum Voltage Modulation Amplitude.
        uint32_t RF00C0498_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PcieRxPresetHint7_t;

///  @brief 0xF00C0600 : Lane Configuration 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PRD_TXDEEMPH0        : 1;         ///<BIT [0] Replaces MAC_PHY_TXDEEMPH[0].
        uint32_t PRD_TXMARGIN         : 3;         ///<BIT [3:1] Replaces MAC_PHY_TXMARGIN.
        uint32_t PRD_TXSWING          : 1;         ///<BIT [4] Replaces MAC_PHY_TXSWING.
        uint32_t CFG_TX_ALIGN_POS     : 4;         ///<BIT [8:5] TX Alignment Shift Position.
        uint32_t CFG_ELB_THRESHOLD    : 4;         ///<BIT [12:9] Elastic Buffer Quiescent Threshold for PCIe Mode.
        uint32_t CFG_FAST_SYNCH       : 1;         ///<BIT [13] Fast Synchronization at 125 MHz PCLK Frequency.
        uint32_t CFG_FORCE_RXPRESENT  : 2;         ///<BIT [15:14] Receiver Detection Override.
        uint32_t RF00C0600_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} LaneCfg0_t;

///  @brief 0xF00C0604 : Lane Configuration 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CFG_TXELECIDLE_MODE  : 1;         ///<BIT [0] TX Electrical Idle Mode Enable.
        uint32_t CFG_GEN1_TXDATA_DLY  : 2;         ///<BIT [2:1] Generation 1 TX Datapath Delayed Latency.
        uint32_t CFG_GEN2_TXDATA_DLY  : 2;         ///<BIT [4:3] Generation 2 TX Datapath Delayed Latency.
        uint32_t CFG_ALIGN_IDLE_HIZ   : 1;         ///<BIT [5] Align Idle Hi-Z and Lo-Z Control Signals.
        uint32_t CFG_TXDETRX_MODE     : 1;         ///<BIT [6] TX Detect RX Mode.
        uint32_t CFG_DISABLE_TXDETVAL : 1;         ///<BIT [7] Disable TX-Detect-RX Valid Signal During TX Detect RX.
        uint32_t CFG_SPD_CHANGE_WAIT  : 1;         ///<BIT [8] Speed Change Wait Period When Max PLL Rate Mode.
        uint32_t CFG_USE_MAX_PLL_RATE : 1;         ///<BIT [9] Use Max PLL Rate Mode at Common PHY.
        uint32_t CFG_USE_GEN2_PLL_CAL : 1;         ///<BIT [10] Use Gen 2PLL Calibration.
        uint32_t CFG_USE_GEN3_PLL_CAL : 1;         ///<BIT [11] Use Gen 3 PLL Calibration.
        uint32_t CFG_GEN_DLY_TXDATA   : 1;         ///<BIT [12] Gen Delay TX Data.
        uint32_t CFG_BUNDLE_FIFO      : 1;         ///<BIT [13] Bundle FIFO.
        uint32_t RF00C0604_14_RSVD    : 1;         ///<BIT [14] Reserved.
        uint32_t PRD_TXDEEMPH1        : 1;         ///<BIT [15] Replaces MAC_PHY_TXDEEMPH[1].
        uint32_t RF00C0604_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} LaneCfg1_t;

///  @brief 0xF00C0608 : Lane Status 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PM_RESET             : 1;         ///<BIT [0] PM Reset.
        uint32_t PM_PU_PLL            : 1;         ///<BIT [1] Power Up PLL.
        uint32_t PM_PU_TX             : 1;         ///<BIT [2] Power Up Transmitter.
        uint32_t PM_PU_RX             : 1;         ///<BIT [3] Power Up Receiver.
        uint32_t PM_TX_RATE_SEL       : 2;         ///<BIT [5:4] Tx Signaling Rate Control.
        uint32_t PM_RX_RATE_SEL       : 2;         ///<BIT [7:6] Rx Signaling Rate Control.
        uint32_t PM_RX_INIT           : 1;         ///<BIT [8] Rx Initialization Control.
        uint32_t PM_TX_IDLE_LOZ       : 1;         ///<BIT [9] Tx Driver Control.
        uint32_t PM_TX_IDLE_HIZ       : 1;         ///<BIT [10] Tx Driver Idle in High Impedance Mode Control.
        uint32_t PM_TXDETECTRX_EN     : 1;         ///<BIT [11] Tx Receiver Detection Control.
        uint32_t PM_PU_IVREF          : 1;         ///<BIT [12] Power Up COMPHY Current and Voltage Reference.
        uint32_t PM_BEACON_TX_EN      : 1;         ///<BIT [13] Beacon Transmission Enable.
        uint32_t PM_BEACON_RX_EN      : 1;         ///<BIT [14] Low Frequency Beacon Detection Enable.
        uint32_t PM_TX_VCMHOLD_EN     : 1;         ///<BIT [15] Control COMPHY Tx Common Mode.
        uint32_t RF00C0608_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} LaneStat0_t;

///  @brief 0xF00C060C : Lane Status 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PM_TXDCLK_PCLK_EN    : 1;         ///<BIT [0] PCLK Output Enable Status.
        uint32_t PM_PCLK_DPCLK_EN     : 1;         ///<BIT [1] PIPE Internal PCS Data Path Clock Status.
        uint32_t PM_OSCCLK_PCLK_EN    : 1;         ///<BIT [2] PCLK Status Switch Between TXDCLK and OSCCLK.
        uint32_t PM_OSCCLK_AUX_CLK_EN : 1;         ///<BIT [3] Auxiliary Clock Status Switch Between TXDCLK and OSCCLK.
        uint32_t PM_DP_RST_N          : 1;         ///<BIT [4] PIPE Internal PCS Data Path Reset.
        uint32_t PM_ASYNC_RST_N       : 1;         ///<BIT [5] PIPE Internal Power Management Reset.
        uint32_t PM_PIPE_8B           : 1;         ///<BIT [6] Operate PIPE in 8-bit Mode.
        uint32_t PM_PIPE_32B          : 1;         ///<BIT [7] Operate PIPE in 32-bit Mode.
        uint32_t PM_RPCLK_125         : 1;         ///<BIT [8] DPCLK Rate.
        uint32_t PM_CLK_REQ_N         : 1;         ///<BIT [9] CLKREQ Control Status.
        uint32_t PM_STATE             : 6;         ///<BIT [15:10] PM State Select.
        uint32_t RF00C060C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} LaneStat1_t;

///  @brief 0xF00C0610 : Lane Status 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ANA_DPHY_RXPRESENT          : 1;         ///<BIT [0] TX Receiver Detection Status.
        uint32_t ANA_DPHY_SQ_DETECTED        : 1;         ///<BIT [1] Squelch Detector Output Status.
        uint32_t ANA_DPHY_RX_INIT_DONE       : 1;         ///<BIT [2] Rx Initialization Done Status.
        uint32_t ANA_DPHY_TXDETRX_VALID      : 1;         ///<BIT [3] Tx Detect Rx Output Valid.
        uint32_t ANA_DPHY_PLL_READY_RX       : 1;         ///<BIT [4] Analog PLL Ready for Rx.
        uint32_t MAC_PHY_RX_TERMINATION      : 1;         ///<BIT [5] Receiver Termination Control from MAC for USB3 Mode.
        uint32_t ANA_DPHY_PLL_READY_TX       : 1;         ///<BIT [6] Analog PLL Ready for Tx.
        uint32_t PHY_MAC_RXELECIDLE          : 1;         ///<BIT [7] Rx Electrical Idle Status at PIPE Interface.
        uint32_t PHY_MAC_RXVALID             : 1;         ///<BIT [8] Rx Data Valid Status at PIPE Interface.
        uint32_t PHY_MAC_PHYSTATUS           : 1;         ///<BIT [9] PHY Status at PIPE Interface.
        uint32_t MAC_PHY_RATE                : 2;         ///<BIT [11:10] MAC Signaling Rate Control.
        uint32_t MAC_PHY_POWERDOWN           : 2;         ///<BIT [13:12] Power State from MAC Control.
        uint32_t MAC_PHY_TXELECIDLE          : 1;         ///<BIT [14] Tx Electrical Idle from MAC Control.
        uint32_t MAC_PHY_TXDETECTRX_LOOPBACK : 1;         ///<BIT [15] Tx Receiver Detection or Loopback from MAC Control.
        uint32_t RF00C0610_31_16_RSVD        :16;         ///<BIT [31:16] Reserved.
    } b;    
} LaneStat2_t;

///  @brief 0xF00C0614 : Lane Beacon Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CFG_BEACON_TX_EN      : 1;         ///<BIT [0] TX Beacon Mode Enable.
        uint32_t CFG_BEACON_RX_EN      : 1;         ///<BIT [1] RX Beacon Mode Enable.
        uint32_t CFG_BEACON_TXLOZ_WAIT : 4;         ///<BIT [5:2] Beacon Transmit Lo-Z Wait Period.
        uint32_t CFG_BEACON_MODE       : 1;         ///<BIT [6] Beacon Protocol Control Mode.
        uint32_t CFG_IVREF_MODE        : 1;         ///<BIT [7] SerDes Analog IVREF Control Mode.
        uint32_t CFG_RXEIDETECT_DLY    : 6;         ///<BIT [13:8] PIPE Signal RX Electrical Idle Turn-On Delay Timer Value.
        uint32_t CFG_POWER_SETTLE_WAIT : 1;         ///<BIT [14] Power Settle Wait.
        uint32_t BEACON_DETECTED       : 1;         ///<BIT [15] Low-Frequency Beacon Detected.
        uint32_t RF00C0614_31_16_RSVD  :16;         ///<BIT [31:16] Reserved.
    } b;    
} LaneBeaconCtrl_t;

///  @brief 0xF00C0618 : Lane Configuration 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0618_4_0_RSVD      : 5;         ///<BIT [4:0] Reserved.
        uint32_t CFG_GEN3_TXELECIDLE_DLY : 2;         ///<BIT [6:5] Common PHY TX_IDLE_HIZ Timing Generation 3.
        uint32_t CFG_GEN3_TXDATA_DLY     : 2;         ///<BIT [8:7] Generation 3 TX Datapath Delayed Latency.
        uint32_t RF00C0618_10_9_RSVD     : 2;         ///<BIT [10:9] Reserved.
        uint32_t CFG_BLK_ALIGN_CTRL_1_0  : 2;         ///<BIT [12:11] Block Alignment Logic Control Enable.
        uint32_t CFG_BLK_ALIGN_CTRL_2    : 1;         ///<BIT [13] Block Alignment Control.
        uint32_t RF00C0618_15_14_RSVD    : 2;         ///<BIT [15:14] Reserved.
        uint32_t RF00C0618_31_16_RSVD    :16;         ///<BIT [31:16] Reserved.
    } b;    
} LaneCfg2_t;

///  @brief 0xF00C0620 : Lane Configuration 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CFG_DFE_CTRL         : 3;         ///<BIT [2:0] PHY DFE Signal Control.
        uint32_t CFG_DFE_EN_SEL       : 1;         ///<BIT [3] PHY PIN_DFE_EN Control Select.
        uint32_t CFG_DFE_PAT_SEL      : 1;         ///<BIT [4] PHY PIN_DFE_PAT_DIS Control Select.
        uint32_t CFG_DFE_UPDATE_SEL   : 1;         ///<BIT [5] PHY PIN_DFE_UPDATE_DIS Control Select.
        uint32_t CFG_DFE_OVERRIDE     : 1;         ///<BIT [6] Override PHY DFE Control Pins.
        uint32_t CFG_SSC_CTRL         : 1;         ///<BIT [7] Spread Spectrum Clock Enable.
        uint32_t CFG_REF_FREF_SEL     : 5;         ///<BIT [12:8] CFG Reference Frequency Select (MHz) For USB 3 Mode.
        uint32_t RF00C0620_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t RF00C0620_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} LaneCfg4_t;

///  @brief 0xF00C0624 : Lane Configuration 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CFG_RX_INIT_SEL      : 1;         ///<BIT [0] RX_INIT Control Select.
        uint32_t CFG_SQ_DET_SEL       : 1;         ///<BIT [1] Squelch Detect Signal Select for Rx Initialization Control.
        uint32_t CFG_RX_EQ_CTRL       : 1;         ///<BIT [2] Rx Training Control.
        uint32_t CFG_RXEIDET_DG_EN    : 1;         ///<BIT [3] Rx Electrical Idle De-glitch Enable.
        uint32_t CFG_RXEI_DG_WEIGHT   : 1;         ///<BIT [4] Rx Electrical Idle De-glitching Tap Weight.
        uint32_t RF00C0624_15_5_RSVD  :11;         ///<BIT [15:5] Reserved.
        uint32_t RF00C0624_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} LaneCfg5_t;

///  @brief 0xF00C0628 : Lane Status 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ANA_RESERVED_STATUS  :10;         ///<BIT [9:0] Analog Reserved Status.
        uint32_t PM_PU_SQ             : 1;         ///<BIT [10] Power Up COMPHY Squelch Detector.
        uint32_t PM_REFCLK_DIS        : 1;         ///<BIT [11] COMPHY REFCLK Disabling Control.
        uint32_t ANA_REFCLK_DIS_ACK   : 1;         ///<BIT [12] COMPHY REFCLK Disable Acknowledgement.
        uint32_t PM_REFCLK_VALID      : 1;         ///<BIT [13] External REFCLK Detection Status.
        uint32_t MAC_PHY_TXCOMPLIANCE : 2;         ///<BIT [15:14] MAC PHY TX Compliance.
        uint32_t RF00C0628_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} LaneStat3_t;

///  @brief 0xF00C062C : Lane TX Common Mode Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CFG_TXCMN_DIS_DLY    : 6;         ///<BIT [5:0] TX Common Mode Disable Delay.
        uint32_t CFG_DELAY_TDR_PHYST  : 1;         ///<BIT [6] Delay TX Detect RX PHY Status.
        uint32_t CFG_DELAY_P12_PHYST  : 1;         ///<BIT [7] Delay P1 and P2 PHY Status.
        uint32_t CFG_HIZ_CAL_WAIT     : 4;         ///<BIT [11:8] Hi-Z Calibration Wait Timer.
        uint32_t CFG_HIZ_CAL_TIMER_EN : 1;         ///<BIT [12] Hi-Z Calibration Timer Enable.
        uint32_t CFG_P0S_IDLE_HIZ_DIS : 1;         ///<BIT [13] P0S Tx Idle Hi-Z Disable.
        uint32_t CFG_P1_WAKEUP        : 1;         ///<BIT [14] P1 Wake-Up.
        uint32_t RF00C062C_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C062C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} LaneTxCommonModeCfg_t;

///  @brief 0xF00C069C : Lane Equalization Configuration 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CFG_EQ_FS            : 6;         ///<BIT [5:0] Transmitter Full Swing Parameter.
        uint32_t CFG_EQ_LF            : 6;         ///<BIT [11:6] Transmitter Low Frequency Parameter.
        uint32_t CFG_PHY_RC_EP        : 1;         ///<BIT [12] PHY Operation Mode Select.
        uint32_t CFG_SEL_EQ_STATUS    : 2;         ///<BIT [14:13] Select Equalization Status.
        uint32_t CFG_USE_CTRL_FLD_RST : 1;         ///<BIT [15] PHY CTRL_FIELD_RESET Command Select.
        uint32_t RF00C069C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} LaneEqualizaCfg0_t;

///  @brief 0xF00C0700 : Pipe Revision
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SUB_REVISION         : 8;         ///<BIT [7:0] Sub-Revision (PIPE Revision ID).
        uint32_t MAIN_REVISION        : 8;         ///<BIT [15:8] Main-Revision (PCIe PIPE PHY Top Revision ID).
        uint32_t RF00C0700_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PipeRevision_t;

///  @brief 0xF00C0704 : Reset and Clock Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PIPE_SFT_RESET         : 1;         ///<BIT [0] PIPE Soft Reset.
        uint32_t REG_RESET              : 1;         ///<BIT [1] PIPE Register Reset.
        uint32_t MODE_FIXED_PCLK        : 1;         ///<BIT [2] PHY Datapath Width Mode for PCIe.
        uint32_t MODE_PIPE_WIDTH32      : 1;         ///<BIT [3] PIPE Data Bus Width.
        uint32_t MODE_REFDIV_1_0        : 2;         ///<BIT [5:4] Reference Clock Divisor.
        uint32_t MODE_CORE_CLK_CTRL     : 1;         ///<BIT [6] CORE_CLK Output Control.
        uint32_t MODE_MULTICAST         : 1;         ///<BIT [7] Select Multicast Register Mode.
        uint32_t PHY_RESET              : 1;         ///<BIT [8] Common PHY Reset.
        uint32_t MODE_CORE_CLK_FREQ_SEL : 1;         ///<BIT [9] CORE_CLK Output Frequency Select.
        uint32_t RF00C0704_15_10_RSVD   : 6;         ///<BIT [15:10] Reserved.
        uint32_t RF00C0704_31_16_RSVD   :16;         ///<BIT [31:16] Reserved.
    } b;    
} RstAndClkCtrl_t;

///  @brief 0xF00C0708 : Test Mode Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MODE_BIST            : 1;         ///<BIT [0] BIST Mode Enable.
        uint32_t MODE_PM_OVERRIDE     : 1;         ///<BIT [1] Override PM Control Outputs.
        uint32_t MODE_MARGIN_OVERRIDE : 1;         ///<BIT [2] Override Margining Controls From the MAC.
        uint32_t DBG_TESTBUS_SEL_3_0  : 4;         ///<BIT [6:3] Test Bus Lane Select.
        uint32_t DBG_TESTBUS_SEL_4    : 1;         ///<BIT [7] PIPE or PHY Test Bus Select.
        uint32_t DBG_TESTBUS_SEL_5    : 1;         ///<BIT [8] Test Bus Output Enable.
        uint32_t RF00C0708_9_RSVD     : 1;         ///<BIT [9] Reserved.
        uint32_t MODE_LB_SHALLOW      : 1;         ///<BIT [10] Enable Shallow Loopback.
        uint32_t MODE_LB_DEEP         : 1;         ///<BIT [11] Enable Deep Loopback.
        uint32_t MODE_LB_SERDES       : 1;         ///<BIT [12] Enable SerDes Loopback.
        uint32_t MODE_RST_OVERRIDE    : 1;         ///<BIT [13] Override PHY Reset.
        uint32_t MODE_STATE_OVERRIDE  : 1;         ///<BIT [14] Override PM State Machine.
        uint32_t RF00C0708_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C0708_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TstModeCtrl_t;

///  @brief 0xF00C070C : Clock Source Low
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MODE_CLK_SRC           : 1;         ///<BIT [0] Mode Clock Source.
        uint32_t BUNDLE_PERIOD_SEL      : 1;         ///<BIT [1] Select Unit Of Lane-to-lane Bundle Clock Sampling Period.
        uint32_t BUNDLE_PERIOD_SCALE    : 2;         ///<BIT [3:2] Lane-to-Lane Bundle Sampling Period Scale.
        uint32_t BUNDLE_SAMPLE_CTRL     : 1;         ///<BIT [4] Bundle Signal Sampling Control.
        uint32_t PLL_READY_DLY          : 3;         ///<BIT [7:5] PLL Ready Delay.
        uint32_t PRD_BUNDLE_PLL_RDY     : 1;         ///<BIT [8] Bundle PLL Ready.
        uint32_t CFG_USE_ALIGN_CLK      : 1;         ///<BIT [9] Use LANE_ALIGN_CLOCK to Reset TXCLK Divider.
        uint32_t MODE_P2_OFF            : 1;         ///<BIT [10] P2 Off Mode Enable.
        uint32_t CFG_FORCE_OCLK_EN      : 1;         ///<BIT [11] CFG_FORCE_OCLK_EN.
        uint32_t CFG_SLOW_LANE_ALIGN    : 1;         ///<BIT [12] CFG Slow Lane Align.
        uint32_t CFG_USE_LANE_ALIGN_RDY : 1;         ///<BIT [13] CFG Use Lane Align Ready.
        uint32_t CFG_CLK_SRC_MASK       : 1;         ///<BIT [14] CFG Clock Source Mask.
        uint32_t RF00C070C_15_RSVD      : 1;         ///<BIT [15] Reserved.
        uint32_t RF00C070C_31_16_RSVD   :16;         ///<BIT [31:16] Reserved.
    } b;    
} ClkSrcLow_t;

///  @brief 0xF00C0710 : Clock Source High
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LANE_START            : 1;         ///<BIT [0] Lane Start.
        uint32_t LANE_BREAK            : 1;         ///<BIT [1] Lane Break.
        uint32_t LANE_MASTER           : 1;         ///<BIT [2] Lane Master.
        uint32_t BIFURCATION_SEL       : 2;         ///<BIT [4:3] Bifurcation Multiplexer Selection.
        uint32_t RF00C0710_5_RSVD      : 1;         ///<BIT [5] Reserved.
        uint32_t RF00C0710_6_RSVD      : 1;         ///<BIT [6] Reserved.
        uint32_t MODE_PIPE4_IF         : 1;         ///<BIT [7] PIPE Version 4 Mode Enable.
        uint32_t CFG_LANE_TURN_OFF_DIS : 1;         ///<BIT [8] Lane Turn Off Signaling Disable.
        uint32_t RF00C0710_9_RSVD      : 1;         ///<BIT [9] Reserved.
        uint32_t RF00C0710_10_RSVD     : 1;         ///<BIT [10] Reserved.
        uint32_t RF00C0710_11_RSVD     : 1;         ///<BIT [11] Reserved.
        uint32_t RF00C0710_12_RSVD     : 1;         ///<BIT [12] Reserved.
        uint32_t RF00C0710_13_RSVD     : 1;         ///<BIT [13] Reserved.
        uint32_t CFG_RXTERM_ENABLE     : 1;         ///<BIT [14] Receiver Termination Control Select.
        uint32_t CFG_VBUS_PRESENT_SRC  : 1;         ///<BIT [15] VBUS Present Source for USB3 Mode.
        uint32_t RF00C0710_31_16_RSVD  :16;         ///<BIT [31:16] Reserved.
    } b;    
} ClkSrcHigh_t;

///  @brief 0xF00C0714 : Trigger
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PULSE_LENGTH_4_0     : 5;         ///<BIT [4:0] Length of Trigger Pulse in APB3 Clock Cycles (1-32).
        uint32_t RF00C0714_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t CFG_UPDATE           : 1;         ///<BIT [8] Update PM Registers.
        uint32_t BIST_START           : 1;         ///<BIT [9] Trigger Start of BIST Sequence.
        uint32_t COUNTER_SAMPLE       : 1;         ///<BIT [10] Sample Counter and Continue Counting.
        uint32_t COUNTER_SAMPLE_CLEAR : 1;         ///<BIT [11] Sample and Clear Counter.
        uint32_t PMO_POWER_VALID      : 1;         ///<BIT [12] PM Override Power State Valid Signal.
        uint32_t RF00C0714_14_13_RSVD : 2;         ///<BIT [14:13] Reserved.
        uint32_t PULSE_DONE           : 1;         ///<BIT [15] Trigger Pulse Status.
        uint32_t RF00C0714_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Trig_t;

///  @brief 0xF00C0718 : Miscellaneous Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MODE_P1_CLK_REQ_N    : 1;         ///<BIT [0] P1 CLKREQ Mode Enable.
        uint32_t MODE_P2_PHYSTATUS    : 1;         ///<BIT [1] P2 Mode PHY Status.
        uint32_t MODE_PCLK_CTRL       : 1;         ///<BIT [2] PCLK Output Control.
        uint32_t SQ_DETECT_SRC        : 1;         ///<BIT [3] SQ_DETECTED Signal Source.
        uint32_t SQ_DETECT_OVERRIDE   : 1;         ///<BIT [4] SQ_DETECTED Override.
        uint32_t CFG_RX_HIZ_SRC       : 1;         ///<BIT [5] Receiver Hi-Z Source of Reference Clock Buffer.
        uint32_t MODE_P1_SNOOZ        : 1;         ///<BIT [6] P1 Snooz Mode Enable.
        uint32_t MODE_P1_OFF          : 1;         ///<BIT [7] P1 Off Mode Enable.
        uint32_t OSC_COUNT_SCALE      : 3;         ///<BIT [10:8] OSCCLK Count Scale.
        uint32_t RCB_RXEN_SRC         : 1;         ///<BIT [11] Receiver Enable Source of Reference Clock Buffer.
        uint32_t CFG_FREE_OSC_SEL     : 1;         ///<BIT [12] Free Running OSCCLK Select.
        uint32_t MODE_REFCLK_DIS      : 1;         ///<BIT [13] PHY REFCLK Disable Sequence Enable.
        uint32_t CFG_REFCLK_DET_TYPE  : 1;         ///<BIT [14] REFCLK Activity Detection Type.
        uint32_t CFG_CLK_ACK_TIMER_EN : 1;         ///<BIT [15] Clock Request Acknowledgement Time Enable.
        uint32_t RF00C0718_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} MiscCtrl_t;

///  @brief 0xF00C071C : CLKREQ# Timing Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLKREQ_N_SRC           : 1;         ///<BIT [0] CLKREQ# Signal Source.
        uint32_t CLKREQ_N_OVERRIDE      : 1;         ///<BIT [1] Override CLKREQ# Signal.
        uint32_t REFCLK_RESTORE_DLY     : 6;         ///<BIT [7:2] REFCLK Restore Delay.
        uint32_t REFCLK_SHUTOFF_DLY     : 2;         ///<BIT [9:8] REFCLK Shut Off Delay.
        uint32_t REFCLK_DISABLE_DLY_3_0 : 4;         ///<BIT [13:10] REFCLK Disable Delay.
        uint32_t REFCLK_DISABLE_DLY_5_4 : 2;         ///<BIT [15:14] REFCLK Disable Delay.
        uint32_t RF00C071C_31_16_RSVD   :16;         ///<BIT [31:16] Reserved.
    } b;    
} ClkreqTmCtrl_t;

///  @brief 0xF00C0720 : Datapath Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CFG_IGNORE_PHY_RDY      : 1;         ///<BIT [0] Common PHY PHY_RDY Handling.
        uint32_t CFG_GEN1_TXELECIDLE_DLY : 2;         ///<BIT [2:1] Common PHY TX_IDLE_HIZ Timing.
        uint32_t CFG_PASS_RXINFO         : 1;         ///<BIT [3] RX Data Mode.
        uint32_t CFG_NO_DISPERROR        : 1;         ///<BIT [4] Disparity Error Handling Mode.
        uint32_t CFG_DISABLE_EDB         : 1;         ///<BIT [5] EDB Replacement of Error Symbols.
        uint32_t CFG_MASK_ERRORS         : 1;         ///<BIT [6] 8B10B Decoder Error Masking.
        uint32_t CFG_DISABLE_SKP         : 1;         ///<BIT [7] SKP Ordered Set Handling Mode.
        uint32_t CFG_ALWAYS_ALIGN        : 1;         ///<BIT [8] Symbol Alignment Mode.
        uint32_t CFG_SAL_FREEZE          : 1;         ///<BIT [9] Symbol Alignment State Machine Freeze.
        uint32_t CFG_GEN2_TXELECIDLE_DLY : 2;         ///<BIT [11:10] Common PHY TX_IDLE_HIZ Timing Generation 2.
        uint32_t CFG_TXELECIDLE_ASSERT   : 1;         ///<BIT [12] Common PHY TX_IDLE_HIZ Assert Timing.
        uint32_t CFG_SAL_IGNORE_SQ       : 1;         ///<BIT [13] Squelch Detected Signal Handling On Symbol Alignment.
        uint32_t RF00C0720_15_14_RSVD    : 2;         ///<BIT [15:14] Reserved.
        uint32_t RF00C0720_31_16_RSVD    :16;         ///<BIT [31:16] Reserved.
    } b;    
} DatapathCfg_t;

///  @brief 0xF00C0740 : Power Management Timing Parameter 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CFG_PM_RXDLOZ_WAIT   : 8;         ///<BIT [7:0] PM RX LoZ Wait.
        uint32_t RF00C0740_11_8_RSVD  : 4;         ///<BIT [11:8] Reserved.
        uint32_t CFG_PM_OSCCLK_WAIT   : 4;         ///<BIT [15:12] PM OSCCLK Wait.
        uint32_t RF00C0740_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PowerMngTmParam1_t;

///  @brief 0xF00C0748 : Counter Type
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t COUNTER_LANE         : 5;         ///<BIT [4:0] Counter Lane.
        uint32_t RF00C0748_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t COUNTER_TYPE         : 4;         ///<BIT [11:8] Counter Type.
        uint32_t PMO_PU_SQ            : 1;         ///<BIT [12] PMO Power-Up Squelch.
        uint32_t PMO_REFCLK_DIS       : 1;         ///<BIT [13] PMO REFCLK Disable.
        uint32_t RF00C0748_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t RF00C0748_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} CntrType_t;

///  @brief 0xF00C074C : Counter Low
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t COUNTER_SAMPLED_15_0 :16;         ///<BIT [15:0] Low 16 Bits of Sampled Counter Value.
        uint32_t RF00C074C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} CntrLow_t;

///  @brief 0xF00C0750 : Counter High
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t COUNTER_SAMPLED_31_16 :16;         ///<BIT [15:0] High 16 Bits of Sampled Counter Value.
        uint32_t RF00C0750_31_16_RSVD  :16;         ///<BIT [31:16] Reserved.
    } b;    
} CntrHigh_t;

///  @brief 0xF00C0758 : PM Datapath and Clock Control Override
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00C0758_6_0_RSVD   : 7;         ///<BIT [6:0] Reserved.
        uint32_t PMO_PIPE_32B         : 1;         ///<BIT [7] Datapath Width Mode 32-Bit Select.
        uint32_t RF00C0758_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t RF00C0758_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PmDatapathAndClkCtrlOverride_t;

///  @brief 0xF00C075C : Low Frequency Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LOW_FREQ_PERIOD_MIN  : 7;         ///<BIT [6:0] Low Frequency Minimum Period.
        uint32_t LOW_FREQ_PERIOD_MAX  : 7;         ///<BIT [13:7] Low Frequency Maximum Period.
        uint32_t LOW_FREQ_CNT_SCALE   : 2;         ///<BIT [15:14] Low Frequency Counter Scale.
        uint32_t RF00C075C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} LowFrqCfg_t;

///  @brief 0xF00C0780 : BIST Control Input
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BIST_POWERDOWN           : 2;         ///<BIT [1:0] BIST Power State Control.
        uint32_t BIST_RATE                : 2;         ///<BIT [3:2] BIST Signal Rate Control.
        uint32_t BIST_TXDETECTRX_LOOPBACK : 1;         ///<BIT [4] BIST Tx Receiver Detection or Loopback Control.
        uint32_t BIST_TXELECIDLE          : 1;         ///<BIT [5] BIST Tx Electrical Idle Control.
        uint32_t BIST_TXCOMPLIANCE        : 1;         ///<BIT [6] BIST Tx Compliance Signaling Control.
        uint32_t BIST_RXPOLARITY          : 1;         ///<BIT [7] BIST Rx Polarity Inversion Select.
        uint32_t BIST_TXDATAK             : 2;         ///<BIT [9:8] BIST TXDATAK.
        uint32_t BIST_CLK_REQ_N           : 1;         ///<BIT [10] BIST Clock Request Enable.
        uint32_t RF00C0780_15_11_RSVD     : 5;         ///<BIT [15:11] Reserved.
        uint32_t RF00C0780_31_16_RSVD     :16;         ///<BIT [31:16] Reserved.
    } b;    
} BistCtrlIn_t;


/// @brief 0xF00C0000-0xF00C07B0 : PciePhy_t MODULE
typedef struct
{
    ChipId11_t chipId1_1;                                            ///< 0xF00C0000 : Chip ID
    PowerAndPllCtrl_t powerAndPllCtrl;                               ///< 0xF00C0004 : Power and PLL Control
    KvcoCalCtrl_t kvcoCalCtrl;                                       ///< 0xF00C0008 : KVCO Calibration Control
    ImpCalCtrl_t impCalCtrl;                                         ///< 0xF00C000C : Impedance Calibration Control
    ImpAndTransmitSscCtrl_t impAndTransmitSscCtrl;                   ///< 0xF00C0010 : Impedance and Transmitter SSC Control
    TransmitFrqOs_t transmitFrqOs;                                   ///< 0xF00C0014 : Transmitter Frequency Offset
    SqlAndFfeSetting_t sqlAndFfeSetting;                             ///< 0xF00C0018 : Squelch and FFE Setting
    DfeResolutiCtrl_t dfeResolutiCtrl;                               ///< 0xF00C001C : DFE Resolution Control
    DfeUpdateCtrl_t dfeUpdateCtrl;                                   ///< 0xF00C0020 : DFE Update Control
    DfeCoeffCtrl0_t dfeCoeffCtrl0;                                   ///< 0xF00C0024 : DFE Coefficient Control 0
    DfeCoeffCtrl1_t dfeCoeffCtrl1;                                   ///< 0xF00C0028 : DFE Coefficient Control 1
    AdtDfeCoeff0_t adtDfeCoeff0;                                     ///< 0xF00C002C : Adapted DFE Coefficient 0
    AdtDfeCoeff1_t adtDfeCoeff1;                                     ///< 0xF00C0030 : Adapted DFE Coefficient 1
    Gen1Setting0_t gen1Setting0;                                     ///< 0xF00C0034 : Generation 1 Setting 0
    Gen1Setting1_t gen1Setting1;                                     ///< 0xF00C0038 : Generation 1 Setting 1
    Gen2Setting0_t gen2Setting0;                                     ///< 0xF00C003C : Generation 2 Setting 0
    Gen2Setting1_t gen2Setting1;                                     ///< 0xF00C0040 : Generation 2 Setting 1
    Gen3Setting0_t gen3Setting0;                                     ///< 0xF00C0044 : Generation 3 Setting 0
    Gen3Setting1_t gen3Setting1;                                     ///< 0xF00C0048 : Generation 3 Setting 1
    Gen4Setting0_t gen4Setting0;                                     ///< 0xF00C004C : Generation 4 Setting 0
    Gen4Setting1_t gen4Setting1;                                     ///< 0xF00C0050 : Generation 4 Setting 1
    PhyTstCtrl0_t phyTstCtrl0;                                       ///< 0xF00C0054 : PHY Test Control 0
    PhyTstData0_t phyTstData0;                                       ///< 0xF00C0058 : PHY Test Data 0
    PhyTstData111_t phyTstData11_1;                                  ///< 0xF00C005C : PHY Test Data 1
    PhyTstData212_t phyTstData21_2;                                  ///< 0xF00C0060 : PHY Test Data 2
    PhyTstData3_t phyTstData3;                                       ///< 0xF00C0064 : PHY Test Data 3
    PhyTstData4_t phyTstData4;                                       ///< 0xF00C0068 : PHY Test Data 4
    PhyTstData5_t phyTstData5;                                       ///< 0xF00C006C : PHY Test Data 5
    PhyTstPrbsCntr0_t phyTstPrbsCntr0;                               ///< 0xF00C0070 : PHY Test PRBS Counter 0
    PhyTstPrbsCntr1_t phyTstPrbsCntr1;                               ///< 0xF00C0074 : PHY Test PRBS Counter 1
    PhyTstPrbsCntr2_t phyTstPrbsCntr2;                               ///< 0xF00C0078 : PHY Test PRBS Counter 2
    PhyTstPrbsErrCntr0_t phyTstPrbsErrCntr0;                         ///< 0xF00C007C : PHY Test PRBS Error Counter 0
    PhyTstPrbsErrCntr1_t phyTstPrbsErrCntr1;                         ///< 0xF00C0080 : PHY Test PRBS Error Counter 1
    PhyTstOob0_t phyTstOob0;                                         ///< 0xF00C0084 : PHY Test OOB 0
    Rsvd022h_t rsvd022h;                                             ///< 0xF00C0088 : Reserved_022h
    DigLoopbackEn_t digLoopbackEn;                                   ///< 0xF00C008C : Digital Loopback Enable
    SyncPat_t syncPat;                                               ///< 0xF00C0090 : Sync Pattern
    SyncMskAndMaximalGenSetting_t syncMskAndMaximalGenSetting;       ///< 0xF00C0094 : Sync Mask and Maximal Generation Setting
    PhyIsolatioModeCtrl_t phyIsolatioModeCtrl;                       ///< 0xF00C0098 : PHY Isolation Mode Control
    EomCircuitCtrl_t eomCircuitCtrl;                                 ///< 0xF00C009C : EOM Circuit Control
    EomSet1Ctrl0_t eomSet1Ctrl0;                                     ///< 0xF00C00A0 : EOM Set 1 Control 0
    EomSet1Ctrl1_t eomSet1Ctrl1;                                     ///< 0xF00C00A4 : EOM Set 1 Control 1
    EomSet1PopCntr0_t eomSet1PopCntr0;                               ///< 0xF00C00A8 : EOM Set 1 Population Counter 0
    EomSet1PopCntr1_t eomSet1PopCntr1;                               ///< 0xF00C00AC : EOM Set 1 Population Counter 1
    EomSet1PopCntr2_t eomSet1PopCntr2;                               ///< 0xF00C00B0 : EOM Set 1 Population Counter 2
    EomSet1PopCntr3_t eomSet1PopCntr3;                               ///< 0xF00C00B4 : EOM Set 1 Population Counter 3
    EomSet1ThrCntr0_t eomSet1ThrCntr0;                               ///< 0xF00C00B8 : EOM Set 1 Threshold Counter 0
    EomSet1ThrCntr1_t eomSet1ThrCntr1;                               ///< 0xF00C00BC : EOM Set 1 Threshold Counter 1
    EomSet1ThrCntr2_t eomSet1ThrCntr2;                               ///< 0xF00C00C0 : EOM Set 1 Threshold Counter 2
    EomSet1ThrCntr3_t eomSet1ThrCntr3;                               ///< 0xF00C00C4 : EOM Set 1 Threshold Counter 3
    EomSet1SampleCntr0_t eomSet1SampleCntr0;                         ///< 0xF00C00C8 : EOM Set 1 Sample Counter 0
    EomSet1SampleCntr1_t eomSet1SampleCntr1;                         ///< 0xF00C00CC : EOM Set 1 Sample Counter 1
    EomSet1SampleCntr2_t eomSet1SampleCntr2;                         ///< 0xF00C00D0 : EOM Set 1 Sample Counter 2
    EomSet1SampleCntr3_t eomSet1SampleCntr3;                         ///< 0xF00C00D4 : EOM Set 1 Sample Counter 3
    EomSet1ErrCntr0_t eomSet1ErrCntr0;                               ///< 0xF00C00D8 : EOM Set 1 Error Counter 0
    EomSet1ErrCntr1_t eomSet1ErrCntr1;                               ///< 0xF00C00DC : EOM Set 1 Error Counter 1
    EomSet1ErrCntr2_t eomSet1ErrCntr2;                               ///< 0xF00C00E0 : EOM Set 1 Error Counter 2
    EomSet1ErrCntr3_t eomSet1ErrCntr3;                               ///< 0xF00C00E4 : EOM Set 1 Error Counter 3
    ExtDigRsvd_t extDigRsvd;                                         ///< 0xF00C00E8 : External Digital Reserved
    DfeStepSize0_t dfeStepSize0;                                     ///< 0xF00C00EC : DFE Step Size 0
    DfeStepSize1_t dfeStepSize1;                                     ///< 0xF00C00F0 : DFE Step Size 1
    Gen1Setting2_t gen1Setting2;                                     ///< 0xF00C00F4 : Generation 1 Setting 2
    Gen2Setting2_t gen2Setting2;                                     ///< 0xF00C00F8 : Generation 2 Setting 2
    Gen3Setting2_t gen3Setting2;                                     ///< 0xF00C00FC : Generation 3 Setting 2
    Gen4Setting2_t gen4Setting2;                                     ///< 0xF00C0100 : Generation 4 Setting 2
    uint8_t rsvd_RF00C0104[20];                                      ///< 0xF00C0104 - 0xF00C0117 : Reserved.
    AnalogRsvd1_t analogRsvd1;                                       ///< 0xF00C0118 : Analog Reserved 1
    AnalogRsvd2_t analogRsvd2;                                       ///< 0xF00C011C : Analog Reserved 2
    uint8_t rsvd_RF00C0120[4];                                       ///< 0xF00C0120 - 0xF00C0123 : Reserved.
    LaneAlignmenCtrl_t laneAlignmenCtrl;                             ///< 0xF00C0124 : Lane Alignment Control
    uint8_t rsvd_RF00C0128[20];                                      ///< 0xF00C0128 - 0xF00C013B : Reserved.
    MiscCtrl0_t miscCtrl0;                                           ///< 0xF00C013C : Miscellaneous Control 0
    Rsvd050h_t rsvd050h;                                             ///< 0xF00C0140 : Reserved_050h
    InterfacAndDigRefClkCtrl_t interfacAndDigRefClkCtrl;             ///< 0xF00C0144 : Interface and Digital Reference Clock Control
    SoftRst_t softRst;                                               ///< 0xF00C0148 : Soft Reset
    uint8_t rsvd_RF00C014C[12];                                      ///< 0xF00C014C - 0xF00C0157 : Reserved.
    SpeedChangeClkOn_t speedChangeClkOn;                             ///< 0xF00C0158 : Speed Change Clock On
    Rsvd057h_t rsvd057h;                                             ///< 0xF00C015C : Reserved_057h
    TransmitDccCtrl_t transmitDccCtrl;                               ///< 0xF00C0160 : Transmitter DCC Control
    uint8_t rsvd_RF00C0164[12];                                      ///< 0xF00C0164 - 0xF00C016F : Reserved.
    Rsvd05ch_t rsvd05ch;                                             ///< 0xF00C0170 : Reserved_05Ch
    uint8_t rsvd_RF00C0174[8];                                       ///< 0xF00C0174 - 0xF00C017B : Reserved.
    SscCtrl_t sscCtrl;                                               ///< 0xF00C017C : SSC Control
    uint8_t rsvd_RF00C0180[4];                                       ///< 0xF00C0180 - 0xF00C0183 : Reserved.
    DtlCtrl_t dtlCtrl;                                               ///< 0xF00C0184 : DTL Control
    uint8_t rsvd_RF00C0188[20];                                      ///< 0xF00C0188 - 0xF00C019B : Reserved.
    DfeMiscCtrl_t dfeMiscCtrl;                                       ///< 0xF00C019C : DFE Miscellaneous Control
    uint8_t rsvd_RF00C01A0[8];                                       ///< 0xF00C01A0 - 0xF00C01A7 : Reserved.
    FastDfeTmrEn_t fastDfeTmrEn;                                     ///< 0xF00C01A8 : Fast DFE Timer Enable
    DfeTmr_t dfeTmr;                                                 ///< 0xF00C01AC : DFE Timer
    uint8_t rsvd_RF00C01B0[20];                                      ///< 0xF00C01B0 - 0xF00C01C3 : Reserved.
    PllCalCtrl1_t pllCalCtrl1;                                       ///< 0xF00C01C4 : PLL Calibration Control 1
    SqlGlitchFilterCtrl_t sqlGlitchFilterCtrl;                       ///< 0xF00C01C8 : Squelch Glitch Filter Control
    MiscCtrl1_t miscCtrl1;                                           ///< 0xF00C01CC : Miscellaneous Control 1
    MiscCtrl2_t miscCtrl2;                                           ///< 0xF00C01D0 : Miscellaneous Control 2
    uint8_t rsvd_RF00C01D4[8];                                       ///< 0xF00C01D4 - 0xF00C01DB : Reserved.
    A150mClkCtrl2_t a150mClkCtrl2;                                   ///< 0xF00C01DC : 150M Clock Control 2
    uint8_t rsvd_RF00C01E0[32];                                      ///< 0xF00C01E0 - 0xF00C01FF : Reserved.
    Rsvd080h_t rsvd080h;                                             ///< 0xF00C0200 : Reserved_080h
    AcjtagCtrl0_t acjtagCtrl0;                                       ///< 0xF00C0204 : ACJTAG Control 0
    DfeF1p5AndF2p5Ctrl_t dfeF1p5AndF2p5Ctrl;                         ///< 0xF00C0208 : DFE F1P5 and F2P5 Control
    FfeCtrl_t ffeCtrl;                                               ///< 0xF00C020C : FFE Control
    Rsvd084h_t rsvd084h;                                             ///< 0xF00C0210 : Reserved_084h
    uint8_t rsvd_RF00C0214[24];                                      ///< 0xF00C0214 - 0xF00C022B : Reserved.
    TrxTrainingCtrl0_t trxTrainingCtrl0;                             ///< 0xF00C022C : TRX Training Control 0
    TrxTrainingCtrl1_t trxTrainingCtrl1;                             ///< 0xF00C0230 : TRX Training Control 1
    Rsvd08dh_t rsvd08dh;                                             ///< 0xF00C0234 : Reserved_08Dh
    Rsvd08eh_t rsvd08eh;                                             ///< 0xF00C0238 : Reserved_08Eh
    Rsvd08fh_t rsvd08fh;                                             ///< 0xF00C023C : Reserved_08Fh
    RxTrainingCtrl3_t rxTrainingCtrl3;                               ///< 0xF00C0240 : RX Training Control 3
    Rsvd091h_t rsvd091h;                                             ///< 0xF00C0244 : Reserved_091h
    Rsvd092h_t rsvd092h;                                             ///< 0xF00C0248 : Reserved_092h
    Rsvd093h_t rsvd093h;                                             ///< 0xF00C024C : Reserved_093h
    Rsvd094h_t rsvd094h;                                             ///< 0xF00C0250 : Reserved_094h
    Rsvd095h_t rsvd095h;                                             ///< 0xF00C0254 : Reserved_095h
    Rsvd096h_t rsvd096h;                                             ///< 0xF00C0258 : Reserved_096h
    Rsvd097h_t rsvd097h;                                             ///< 0xF00C025C : Reserved_097h
    Rsvd098h_t rsvd098h;                                             ///< 0xF00C0260 : Reserved_098h
    Rsvd099h_t rsvd099h;                                             ///< 0xF00C0264 : Reserved_099h
    TxTrainingCtrl0_t txTrainingCtrl0;                               ///< 0xF00C0268 : TX Training Control 0
    Rsvd09bh_t rsvd09bh;                                             ///< 0xF00C026C : Reserved_09Bh
    TxTrainingCtrl2_t txTrainingCtrl2;                               ///< 0xF00C0270 : TX Training Control 2
    uint8_t rsvd_RF00C0274[4];                                       ///< 0xF00C0274 - 0xF00C0277 : Reserved.
    TxTrainingCtrl4_t txTrainingCtrl4;                               ///< 0xF00C0278 : TX Training Control 4
    Rsvd09fh_t rsvd09fh;                                             ///< 0xF00C027C : Reserved_09Fh
    Rsvd0a0h_t rsvd0a0h;                                             ///< 0xF00C0280 : Reserved_0A0h
    Rsvd0a1h_t rsvd0a1h;                                             ///< 0xF00C0284 : Reserved_0A1h
    Rsvd0a2h_t rsvd0a2h;                                             ///< 0xF00C0288 : Reserved_0A2h
    uint8_t rsvd_RF00C028C[4];                                       ///< 0xF00C028C - 0xF00C028F : Reserved.
    Rsvd0a4h_t rsvd0a4h;                                             ///< 0xF00C0290 : Reserved_0A4h
    TxAmpAdjustmeAndTxEmphasisEn_t txAmpAdjustmeAndTxEmphasisEn;     ///< 0xF00C0294 : TX Amplitude Adjustment and TX Emphasis Enable
    uint8_t rsvd_RF00C0298[4];                                       ///< 0xF00C0298 - 0xF00C029B : Reserved.
    TxAmpIndexLimit_t txAmpIndexLimit;                               ///< 0xF00C029C : TX Amplitude Index Limit
    TxEmphasisIndexMinVal_t txEmphasisIndexMinVal;                   ///< 0xF00C02A0 : TX Emphasis Index Minimum Value
    TrxTrainingCtrl5_t trxTrainingCtrl5;                             ///< 0xF00C02A4 : TRX Training Control 5
    uint8_t rsvd_RF00C02A8[4];                                       ///< 0xF00C02A8 - 0xF00C02AB : Reserved.
    IntrCtrl1_t intrCtrl1;                                           ///< 0xF00C02AC : Interrupt Control 1
    uint8_t rsvd_RF00C02B0[4];                                       ///< 0xF00C02B0 - 0xF00C02B3 : Reserved.
    IntrCtrl3_t intrCtrl3;                                           ///< 0xF00C02B4 : Interrupt Control 3
    uint8_t rsvd_RF00C02B8[4];                                       ///< 0xF00C02B8 - 0xF00C02BB : Reserved.
    IntrCtrl5_t intrCtrl5;                                           ///< 0xF00C02BC : Interrupt Control 5
    uint8_t rsvd_RF00C02C0[28];                                      ///< 0xF00C02C0 - 0xF00C02DB : Reserved.
    Rsvd0b7h_t rsvd0b7h;                                             ///< 0xF00C02DC : Reserved_0B7h
    ValOfSignalsToAnalogSelectio_t valOfSignalsToAnalogSelectio;     ///< 0xF00C02E0 : Value of Signals to Analog Selection
    uint8_t rsvd_RF00C02E4[8];                                       ///< 0xF00C02E4 - 0xF00C02EB : Reserved.
    Rsvd0bbh_t rsvd0bbh;                                             ///< 0xF00C02EC : Reserved_0BBh
    uint8_t rsvd_RF00C02F0[24];                                      ///< 0xF00C02F0 - 0xF00C0307 : Reserved.
    Rsvd0c2h_t rsvd0c2h;                                             ///< 0xF00C0308 : Reserved_0C2h
    Rsvd0c3h_t rsvd0c3h;                                             ///< 0xF00C030C : Reserved_0C3h
    Rsvd0c4h_t rsvd0c4h;                                             ///< 0xF00C0310 : Reserved_0C4h
    TxTrainingResult1_t txTrainingResult1;                           ///< 0xF00C0314 : TX Training Result 1
    AdtEdgeResult_t adtEdgeResult;                                   ///< 0xF00C0318 : Adapted Edge Result
    TxTrainingPreset_t txTrainingPreset;                             ///< 0xF00C031C : TX Training Preset
    Rsvd0c8h_t rsvd0c8h;                                             ///< 0xF00C0320 : Reserved_0C8h
    Rsvd0c9h_t rsvd0c9h;                                             ///< 0xF00C0324 : Reserved_0C9h
    Rsvd0cah_t rsvd0cah;                                             ///< 0xF00C0328 : Reserved_0CAh
    Rsvd0cbh_t rsvd0cbh;                                             ///< 0xF00C032C : Reserved_0CBh
    RxTrainingDefaultVal_t rxTrainingDefaultVal;                     ///< 0xF00C0330 : RX Training Default Value
    Rsvd0cdh_t rsvd0cdh;                                             ///< 0xF00C0334 : Reserved_0CDh
    Rsvd0ceh_t rsvd0ceh;                                             ///< 0xF00C0338 : Reserved_0CEh
    Rsvd0cfh_t rsvd0cfh;                                             ///< 0xF00C033C : Reserved_0CFh
    Rsvd0d0h_t rsvd0d0h;                                             ///< 0xF00C0340 : Reserved_0D0h
    uint8_t rsvd_RF00C0344[4];                                       ///< 0xF00C0344 - 0xF00C0347 : Reserved.
    Rsvd0d2h_t rsvd0d2h;                                             ///< 0xF00C0348 : Reserved_0D2h
    Rsvd0d3h_t rsvd0d3h;                                             ///< 0xF00C034C : Reserved_0D3h
    Rsvd0d4h_t rsvd0d4h;                                             ///< 0xF00C0350 : Reserved_0D4h
    uint8_t rsvd_RF00C0354[8];                                       ///< 0xF00C0354 - 0xF00C035B : Reserved.
    Rsvd0d7h_t rsvd0d7h;                                             ///< 0xF00C035C : Reserved_0D7h
    Rsvd0d8h_t rsvd0d8h;                                             ///< 0xF00C0360 : Reserved_0D8h
    Rsvd0d9h_t rsvd0d9h;                                             ///< 0xF00C0364 : Reserved_0D9h
    uint8_t rsvd_RF00C0368[4];                                       ///< 0xF00C0368 - 0xF00C036B : Reserved.
    Rsvd0dbh_t rsvd0dbh;                                             ///< 0xF00C036C : Reserved_0DBh
    Rsvd0dch_t rsvd0dch;                                             ///< 0xF00C0370 : Reserved_0DCh
    Rsvd0ddh_t rsvd0ddh;                                             ///< 0xF00C0374 : Reserved_0DDh
    Rsvd0deh_t rsvd0deh;                                             ///< 0xF00C0378 : Reserved_0DEh
    Rsvd0dfh_t rsvd0dfh;                                             ///< 0xF00C037C : Reserved_0DFh
    Rsvd0e0h_t rsvd0e0h;                                             ///< 0xF00C0380 : Reserved_0E0h
    Rsvd0e1h_t rsvd0e1h;                                             ///< 0xF00C0384 : Reserved_0E1h
    Rsvd0e2h_t rsvd0e2h;                                             ///< 0xF00C0388 : Reserved_0E2h
    Rsvd0e3h_t rsvd0e3h;                                             ///< 0xF00C038C : Reserved_0E3h
    Rsvd0e4h_t rsvd0e4h;                                             ///< 0xF00C0390 : Reserved_0E4h
    uint8_t rsvd_RF00C0394[8];                                       ///< 0xF00C0394 - 0xF00C039B : Reserved.
    Rsvd0e7h_t rsvd0e7h;                                             ///< 0xF00C039C : Reserved_0E7h
    Rsvd0e8h_t rsvd0e8h;                                             ///< 0xF00C03A0 : Reserved_0E8h
    Rsvd0e9h_t rsvd0e9h;                                             ///< 0xF00C03A4 : Reserved_0E9h
    Rsvd0eah_t rsvd0eah;                                             ///< 0xF00C03A8 : Reserved_0EAh
    Rsvd0ebh_t rsvd0ebh;                                             ///< 0xF00C03AC : Reserved_0EBh
    DfeStepSize_t dfeStepSize;                                       ///< 0xF00C03B0 : DFE Step Size
    uint8_t rsvd_RF00C03B4[48];                                      ///< 0xF00C03B4 - 0xF00C03E3 : Reserved.
    SqlDetCtrl_t sqlDetCtrl;                                         ///< 0xF00C03E4 : Squelch Detector Control
    uint8_t rsvd_RF00C03E8[48];                                      ///< 0xF00C03E8 - 0xF00C0417 : Reserved.
    CdrCtrl0_t cdrCtrl0;                                             ///< 0xF00C0418 : CDR Control 0
    uint8_t rsvd_RF00C041C[36];                                      ///< 0xF00C041C - 0xF00C043F : Reserved.
    G1Setting3_t g1Setting3;                                         ///< 0xF00C0440 : G1 Setting 3
    G1Setting4_t g1Setting4;                                         ///< 0xF00C0444 : G1 Setting 4
    G2Setting3_t g2Setting3;                                         ///< 0xF00C0448 : G2 Setting 3
    G2Setting4_t g2Setting4;                                         ///< 0xF00C044C : G2 Setting 4
    G3Setting3_t g3Setting3;                                         ///< 0xF00C0450 : G3 Setting 3
    G3Setting4_t g3Setting4;                                         ///< 0xF00C0454 : G3 Setting 4
    G4Setting3_t g4Setting3;                                         ///< 0xF00C0458 : G4 Setting 3
    G4Setting4_t g4Setting4;                                         ///< 0xF00C045C : G4 Setting 4
    uint8_t rsvd_RF00C0460[8];                                       ///< 0xF00C0460 - 0xF00C0467 : Reserved.
    PcieTxPresetIndex_t pcieTxPresetIndex;                           ///< 0xF00C0468 : PCIE TX Preset Index
    TxPowerProtectiCtrl_t txPowerProtectiCtrl;                       ///< 0xF00C046C : TX Power Protection Control
    DfeCtrl27_t dfeCtrl27;                                           ///< 0xF00C0470 : DFE Control 27
    uint8_t rsvd_RF00C0474[36];                                      ///< 0xF00C0474 - 0xF00C0497 : Reserved.
    PcieRxPresetHint7_t pcieRxPresetHint7;                           ///< 0xF00C0498 : PCIE RX Preset Hint 7
    uint8_t rsvd_RF00C049C[356];                                     ///< 0xF00C049C - 0xF00C05FF : Reserved.
    LaneCfg0_t laneCfg0;                                             ///< 0xF00C0600 : Lane Configuration 0
    LaneCfg1_t laneCfg1;                                             ///< 0xF00C0604 : Lane Configuration 1
    LaneStat0_t laneStat0;                                           ///< 0xF00C0608 : Lane Status 0
    LaneStat1_t laneStat1;                                           ///< 0xF00C060C : Lane Status 1
    LaneStat2_t laneStat2;                                           ///< 0xF00C0610 : Lane Status 2
    LaneBeaconCtrl_t laneBeaconCtrl;                                 ///< 0xF00C0614 : Lane Beacon Control
    LaneCfg2_t laneCfg2;                                             ///< 0xF00C0618 : Lane Configuration 2
    uint8_t rsvd_RF00C061C[4];                                       ///< 0xF00C061C - 0xF00C061F : Reserved.
    LaneCfg4_t laneCfg4;                                             ///< 0xF00C0620 : Lane Configuration 4
    LaneCfg5_t laneCfg5;                                             ///< 0xF00C0624 : Lane Configuration 5
    LaneStat3_t laneStat3;                                           ///< 0xF00C0628 : Lane Status 3
    LaneTxCommonModeCfg_t laneTxCommonModeCfg;                       ///< 0xF00C062C : Lane TX Common Mode Configuration
    uint8_t rsvd_RF00C0630[108];                                     ///< 0xF00C0630 - 0xF00C069B : Reserved.
    LaneEqualizaCfg0_t laneEqualizaCfg0;                             ///< 0xF00C069C : Lane Equalization Configuration 0
    uint8_t rsvd_RF00C06A0[96];                                      ///< 0xF00C06A0 - 0xF00C06FF : Reserved.
    PipeRevision_t pipeRevision;                                     ///< 0xF00C0700 : Pipe Revision
    RstAndClkCtrl_t rstAndClkCtrl;                                   ///< 0xF00C0704 : Reset and Clock Control
    TstModeCtrl_t tstModeCtrl;                                       ///< 0xF00C0708 : Test Mode Control
    ClkSrcLow_t clkSrcLow;                                           ///< 0xF00C070C : Clock Source Low
    ClkSrcHigh_t clkSrcHigh;                                         ///< 0xF00C0710 : Clock Source High
    Trig_t trig;                                                     ///< 0xF00C0714 : Trigger
    MiscCtrl_t miscCtrl;                                             ///< 0xF00C0718 : Miscellaneous Control
    ClkreqTmCtrl_t clkreqTmCtrl;                                     ///< 0xF00C071C : CLKREQ# Timing Control
    DatapathCfg_t datapathCfg;                                       ///< 0xF00C0720 : Datapath Configuration
    uint8_t rsvd_RF00C0724[28];                                      ///< 0xF00C0724 - 0xF00C073F : Reserved.
    PowerMngTmParam1_t powerMngTmParam1;                             ///< 0xF00C0740 : Power Management Timing Parameter 1
    uint8_t rsvd_RF00C0744[4];                                       ///< 0xF00C0744 - 0xF00C0747 : Reserved.
    CntrType_t cntrType;                                             ///< 0xF00C0748 : Counter Type
    CntrLow_t cntrLow;                                               ///< 0xF00C074C : Counter Low
    CntrHigh_t cntrHigh;                                             ///< 0xF00C0750 : Counter High
    uint8_t rsvd_RF00C0754[4];                                       ///< 0xF00C0754 - 0xF00C0757 : Reserved.
    PmDatapathAndClkCtrlOverride_t pmDatapathAndClkCtrlOverride;     ///< 0xF00C0758 : PM Datapath and Clock Control Override
    LowFrqCfg_t lowFrqCfg;                                           ///< 0xF00C075C : Low Frequency Configuration
    uint8_t rsvd_RF00C0760[32];                                      ///< 0xF00C0760 - 0xF00C077F : Reserved.
    BistCtrlIn_t bistCtrlIn;                                         ///< 0xF00C0780 : BIST Control Input
} PciePhy_t;
COMPILE_ASSERT(offsetof(PciePhy_t,bistCtrlIn)==0xF00C0780-0xF00C0000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile PciePhy_t rPciePhy;                  ///< 0xF00C0000-0xF00C07B0 : PciePhy_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

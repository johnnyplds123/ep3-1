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
//! @brief SETM3 PHY Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  SETM3 PHY Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xD0020000 : Chip ID
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CID1 : 8;         ///<BIT [7:0] CID1[7:0]: Revision ID.
        uint16_t CID0 : 8;         ///<BIT [15:8] CID0[7:0]: PHY ID.
    } b;    
} ChipId00_t;

///  @brief 0xD0020004 : Power and PLL Control Register
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t REFCLK_SEL         : 1;         ///<BIT [0] Reference Clock Selection.
        uint16_t REFCLK_ON_DCLK_DIS : 1;         ///<BIT [1] Disable Reference Clock on PIN_TXCLK.
        uint16_t RD0020004_2_RSVD   : 1;         ///<BIT [2] Reserved.
        uint16_t REF_FREF_SEL       : 5;         ///<BIT [7:3] Reference Frequency Select.
        uint16_t RD0020004_8_RSVD   : 1;         ///<BIT [8] Reserved.
        uint16_t PLL_LOCK           : 1;         ///<BIT [9] PLL Lock Indicator.
        uint16_t RD0020004_10_RSVD  : 1;         ///<BIT [10] Reserved.
        uint16_t RD0020004_11_RSVD  : 1;         ///<BIT [11] Reserved.
        uint16_t PU_TX_B            : 1;         ///<BIT [12] Power Up Transmitter B.
        uint16_t PU_TX_A            : 1;         ///<BIT [13] Power Up Transmitter A.
        uint16_t PU_PLL             : 1;         ///<BIT [14] Power Up PLL.
        uint16_t PU_IVREF           : 1;         ///<BIT [15] Power Up Reference.
    } b;    
} PowerAndPllCtrlRegister_t;

///  @brief 0xD0020008 : Calibration Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RD0020008_1_0_RSVD : 2;         ///<BIT [1:0] Reserved.
        uint16_t RD0020008_2_RSVD   : 1;         ///<BIT [2] Reserved.
        uint16_t RD0020008_3_RSVD   : 1;         ///<BIT [3] Reserved.
        uint16_t RD0020008_4_RSVD   : 1;         ///<BIT [4] Reserved.
        uint16_t RD0020008_5_RSVD   : 1;         ///<BIT [5] Reserved.
        uint16_t RD0020008_7_6_RSVD : 2;         ///<BIT [7:6] Reserved.
        uint16_t RD0020008_8_RSVD   : 1;         ///<BIT [8] Reserved.
        uint16_t RD0020008_9_RSVD   : 1;         ///<BIT [9] Reserved.
        uint16_t RD0020008_10_RSVD  : 1;         ///<BIT [10] Reserved.
        uint16_t RD0020008_11_RSVD  : 1;         ///<BIT [11] Reserved.
        uint16_t RD0020008_12_RSVD  : 1;         ///<BIT [12] Reserved.
        uint16_t EXT_FORCE_CAL_DONE : 1;         ///<BIT [13] Force Calibration Done.
        uint16_t CAL_DONE           : 1;         ///<BIT [14] Calibration Done.
        uint16_t CAL_START          : 1;         ///<BIT [15] Calibration Start.
    } b;    
} CalCtrl_t;

///  @brief 0xD0020010 : PLL Calibration Controls and Results 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RD0020010_2_0_RSVD : 3;         ///<BIT [2:0] Reserved.
        uint16_t SPEED_PLL          : 6;         ///<BIT [8:3] SPEED_PLL Autocalibration Result.
        uint16_t SPEED              : 6;         ///<BIT [14:9] SPEED_PLL External Overwrite Value.
        uint16_t SPEED_EXT_EN       : 1;         ///<BIT [15] SPEED_PLL External Value Enable.
    } b;    
} PllCalControlsAndResults1_t;

///  @brief 0xD002003C : Impedance Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RD002003C_5_0_RSVD : 6;         ///<BIT [5:0] Reserved.
        uint16_t TX_IMP_B           : 5;         ///<BIT [10:6] Impedance Value for Transmitter B.
        uint16_t TX_IMP_A           : 5;         ///<BIT [15:11] Impedance Value For Transmitter A.
    } b;    
} ImpCtrl_t;

///  @brief 0xD0020040 : Impedance Control and PHY Settings
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RD0020040_3_0_RSVD   : 4;         ///<BIT [3:0] Reserved.
        uint16_t PHY_ISOLATE_MODE     : 1;         ///<BIT [4] PHY Isolation Mode.
        uint16_t RD0020040_5_RSVD     : 1;         ///<BIT [5] Reserved.
        uint16_t USE_MAX_PLL_RATE     : 1;         ///<BIT [6] Use Maximum PLL Rate.
        uint16_t RD0020040_10_7_RSVD  : 4;         ///<BIT [10:7] Reserved.
        uint16_t RD0020040_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
    } b;    
} ImpCtrlAndPhySettings_t;

///  @brief 0xD0020044 : PHY Test Control and Data 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PT_DATA_39_32        : 8;         ///<BIT [7:0] PHY Test Data[39:32].
        uint16_t PT_PRBS_INV          : 1;         ///<BIT [8] PHY Test PRBS Inversion.
        uint16_t PT_PATTERN_SEL       : 3;         ///<BIT [11:9] PHY Test Pattern Select.
        uint16_t RD0020044_13_12_RSVD : 2;         ///<BIT [13:12] Reserved.
        uint16_t PT_EN_B              : 1;         ///<BIT [14] PHY B Test Enable.
        uint16_t PT_EN_A              : 1;         ///<BIT [15] PHY A Test Enable.
    } b;    
} PhyTstCtrlAndData0_t;

///  @brief 0xD0020050 : Transmitter Control Register 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RD0020050_1_0_RSVD : 2;         ///<BIT [1:0] Reserved.
        uint16_t RD0020050_2_RSVD   : 1;         ///<BIT [2] Reserved.
        uint16_t RD0020050_3_RSVD   : 1;         ///<BIT [3] Reserved.
        uint16_t TX_DRV_IDLE        : 1;         ///<BIT [4] Transmitter Driver Idle.
        uint16_t RD0020050_5_RSVD   : 1;         ///<BIT [5] Reserved.
        uint16_t RD0020050_6_RSVD   : 1;         ///<BIT [6] Reserved.
        uint16_t TXD_INV            : 1;         ///<BIT [7] Transmit Polarity Invert.
        uint16_t PHY_GEN_MAX        : 4;         ///<BIT [11:8] Maximum PHY Generation Setting.
        uint16_t TX_RATE            : 3;         ///<BIT [14:12] Transmitter Data Rate Select.
        uint16_t SEL_BITS           : 1;         ///<BIT [15] Select Data Bit Width.
    } b;    
} TransmitCtrlRegister0_t;

///  @brief 0xD0020054 : Transmitter Control Register 1 - Transmitter Slew Rate, Slicer, and Lane Sync Controls
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RD0020054_0_RSVD   : 1;         ///<BIT [0] Reserved.
        uint16_t RD0020054_1_RSVD   : 1;         ///<BIT [1] Reserved.
        uint16_t RD0020054_2_RSVD   : 1;         ///<BIT [2] Reserved.
        uint16_t RD0020054_8_3_RSVD : 6;         ///<BIT [8:3] Reserved.
        uint16_t TX_SLEW_CTRL_EN    : 1;         ///<BIT [9] Transmitter Slew Control Enable.
        uint16_t TX_SLEWRATE_EXT    : 6;         ///<BIT [15:10] Transmitter Slew Rate.
    } b;    
} TransmitCtrlRegister1TransmitSlewRateSlicer_t;

///  @brief 0xD0020058 : Transmitter Control Register 2 - Transmitter Amplitude and VCO Controls
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RD0020058_4_0_RSVD : 5;         ///<BIT [4:0] Reserved.
        uint16_t VCOCLK_DIVR        : 6;         ///<BIT [10:5] VCO Clock Divider Ratio.
        uint16_t TX_AMP             : 5;         ///<BIT [15:11] Transmitter Amplitude Control.
    } b;    
} TransmitCtrlRegister2TransmitAmpAndVco_t;

///  @brief 0xD002005C : Transmitter Control Register 3 - Transmitter Emphasis
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RD002005C_10_0_RSVD :11;         ///<BIT [10:0] Reserved.
        uint16_t TX_EMPH_EN          : 1;         ///<BIT [11] Transmitter Emphasis Enable.
        uint16_t TX_EMPH_AMP         : 4;         ///<BIT [15:12] Transmitter Emphasis Amplitude Control.
    } b;    
} TransmitCtrlRegister3TransmitEmphasis_t;

///  @brief 0xD0020090 : Reserved_24h
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RD0020090_3_0_RSVD  : 4;         ///<BIT [3:0] Reserved.
        uint16_t ICP                 : 4;         ///<BIT [7:4] PLL Charge-Pump Current Control.
        uint16_t ICP_FORCE           : 1;         ///<BIT [8] ICP Force.
        uint16_t RD0020090_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
    } b;    
} Rsvd24h_t;


/// @brief 0xD0020000-0xD00200C0 : Setm3Phy_t MODULE
typedef struct
{
    ChipId00_t chipId0_0;                                                                          ///< 0xD0020000 : Chip ID
    uint8_t rsvd_RD0020002[2];                                                                     ///< 0xD0020002 - 0xD0020003 : Reserved.
    PowerAndPllCtrlRegister_t powerAndPllCtrlRegister;                                             ///< 0xD0020004 : Power and PLL Control Register
    uint8_t rsvd_RD0020006[2];                                                                     ///< 0xD0020006 - 0xD0020007 : Reserved.
    CalCtrl_t calCtrl;                                                                             ///< 0xD0020008 : Calibration Control
    uint8_t rsvd_RD002000A[2];                                                                     ///< 0xD002000A - 0xD002000B : Reserved.
    uint16_t RD002000C_15_0_RSVD;                                                                  ///< 0xD002000C : Reserved_03h
    uint8_t rsvd_RD002000E[2];                                                                     ///< 0xD002000E - 0xD002000F : Reserved.
    PllCalControlsAndResults1_t pllCalControlsAndResults1;                                         ///< 0xD0020010 : PLL Calibration Controls and Results 1
    uint8_t rsvd_RD0020012[2];                                                                     ///< 0xD0020012 - 0xD0020013 : Reserved.
    uint16_t RD0020014_15_0_RSVD;                                                                  ///< 0xD0020014 : Reserved_05h
    uint8_t rsvd_RD0020016[2];                                                                     ///< 0xD0020016 - 0xD0020017 : Reserved.
    uint16_t RD0020018_15_0_RSVD;                                                                  ///< 0xD0020018 : Reserved_06h
    uint8_t rsvd_RD002001A[2];                                                                     ///< 0xD002001A - 0xD002001B : Reserved.
    uint16_t RD002001C_15_0_RSVD;                                                                  ///< 0xD002001C : Reserved_07h
    uint8_t rsvd_RD002001E[2];                                                                     ///< 0xD002001E - 0xD002001F : Reserved.
    uint16_t RD0020020_15_0_RSVD;                                                                  ///< 0xD0020020 : Reserved_08h
    uint8_t rsvd_RD0020022[2];                                                                     ///< 0xD0020022 - 0xD0020023 : Reserved.
    uint16_t RD0020024_15_0_RSVD;                                                                  ///< 0xD0020024 : Reserved_09h
    uint8_t rsvd_RD0020026[2];                                                                     ///< 0xD0020026 - 0xD0020027 : Reserved.
    uint16_t RD0020028_15_0_RSVD;                                                                  ///< 0xD0020028 : Reserved_0Ah
    uint8_t rsvd_RD002002A[2];                                                                     ///< 0xD002002A - 0xD002002B : Reserved.
    uint16_t RD002002C_15_0_RSVD;                                                                  ///< 0xD002002C : Reserved_0Bh
    uint8_t rsvd_RD002002E[10];                                                                    ///< 0xD002002E - 0xD0020037 : Reserved.
    uint16_t RD0020038_15_0_RSVD;                                                                  ///< 0xD0020038 : Reserved_0Eh
    uint8_t rsvd_RD002003A[2];                                                                     ///< 0xD002003A - 0xD002003B : Reserved.
    ImpCtrl_t impCtrl;                                                                             ///< 0xD002003C : Impedance Control
    uint8_t rsvd_RD002003E[2];                                                                     ///< 0xD002003E - 0xD002003F : Reserved.
    ImpCtrlAndPhySettings_t impCtrlAndPhySettings;                                                 ///< 0xD0020040 : Impedance Control and PHY Settings
    uint8_t rsvd_RD0020042[2];                                                                     ///< 0xD0020042 - 0xD0020043 : Reserved.
    PhyTstCtrlAndData0_t phyTstCtrlAndData0;                                                       ///< 0xD0020044 : PHY Test Control and Data 0
    uint8_t rsvd_RD0020046[2];                                                                     ///< 0xD0020046 - 0xD0020047 : Reserved.
    uint16_t PT_DATA_31_16;                                                                        ///< 0xD0020048 : PHY Test Data 1
    uint8_t rsvd_RD002004A[2];                                                                     ///< 0xD002004A - 0xD002004B : Reserved.
    uint16_t PT_DATA_15_0;                                                                         ///< 0xD002004C : PHY Test Data 2
    uint8_t rsvd_RD002004E[2];                                                                     ///< 0xD002004E - 0xD002004F : Reserved.
    TransmitCtrlRegister0_t transmitCtrlRegister0;                                                 ///< 0xD0020050 : Transmitter Control Register 0
    uint8_t rsvd_RD0020052[2];                                                                     ///< 0xD0020052 - 0xD0020053 : Reserved.
    TransmitCtrlRegister1TransmitSlewRateSlicer_t transmitCtrlRegister1TransmitSlewRateSlicer;     ///< 0xD0020054 : Transmitter Control Register 1 - Transmitter Slew Rate, Slicer, and Lane Sync Controls
    uint8_t rsvd_RD0020056[2];                                                                     ///< 0xD0020056 - 0xD0020057 : Reserved.
    TransmitCtrlRegister2TransmitAmpAndVco_t transmitCtrlRegister2TransmitAmpAndVco;               ///< 0xD0020058 : Transmitter Control Register 2 - Transmitter Amplitude and VCO Controls
    uint8_t rsvd_RD002005A[2];                                                                     ///< 0xD002005A - 0xD002005B : Reserved.
    TransmitCtrlRegister3TransmitEmphasis_t transmitCtrlRegister3TransmitEmphasis;                 ///< 0xD002005C : Transmitter Control Register 3 - Transmitter Emphasis
    uint8_t rsvd_RD002005E[2];                                                                     ///< 0xD002005E - 0xD002005F : Reserved.
    uint16_t RD0020060_15_0_RSVD;                                                                  ///< 0xD0020060 : Reserved_18h
    uint8_t rsvd_RD0020062[2];                                                                     ///< 0xD0020062 - 0xD0020063 : Reserved.
    uint16_t RD0020064_15_0_RSVD;                                                                  ///< 0xD0020064 : Reserved _19h
    uint8_t rsvd_RD0020066[2];                                                                     ///< 0xD0020066 - 0xD0020067 : Reserved.
    uint16_t RD0020068_15_0_RSVD;                                                                  ///< 0xD0020068 : Reserved_1Ah
    uint8_t rsvd_RD002006A[2];                                                                     ///< 0xD002006A - 0xD002006B : Reserved.
    uint16_t RD002006C_15_0_RSVD;                                                                  ///< 0xD002006C : Reserved_1Bh
    uint8_t rsvd_RD002006E[2];                                                                     ///< 0xD002006E - 0xD002006F : Reserved.
    uint16_t RD0020070_15_0_RSVD;                                                                  ///< 0xD0020070 : Reserved_1Ch
    uint8_t rsvd_RD0020072[2];                                                                     ///< 0xD0020072 - 0xD0020073 : Reserved.
    uint16_t RD0020074_15_0_RSVD;                                                                  ///< 0xD0020074 : Reserved_1Dh
    uint8_t rsvd_RD0020076[2];                                                                     ///< 0xD0020076 - 0xD0020077 : Reserved.
    uint16_t RD0020078_15_0_RSVD;                                                                  ///< 0xD0020078 : Reserved_1Eh
    uint8_t rsvd_RD002007A[2];                                                                     ///< 0xD002007A - 0xD002007B : Reserved.
    uint16_t RD002007C_15_0_RSVD;                                                                  ///< 0xD002007C : Reserved_1Fh
    uint8_t rsvd_RD002007E[2];                                                                     ///< 0xD002007E - 0xD002007F : Reserved.
    uint16_t RD0020080_15_0_RSVD;                                                                  ///< 0xD0020080 : Reserved_20h
    uint8_t rsvd_RD0020082[2];                                                                     ///< 0xD0020082 - 0xD0020083 : Reserved.
    uint16_t RD0020084_15_0_RSVD;                                                                  ///< 0xD0020084 : Reserved_21h
    uint8_t rsvd_RD0020086[2];                                                                     ///< 0xD0020086 - 0xD0020087 : Reserved.
    uint16_t RD0020088_15_0_RSVD;                                                                  ///< 0xD0020088 : Reserved_22h
    uint8_t rsvd_RD002008A[2];                                                                     ///< 0xD002008A - 0xD002008B : Reserved.
    uint16_t RD002008C_15_0_RSVD;                                                                  ///< 0xD002008C : Reserved_23h
    uint8_t rsvd_RD002008E[2];                                                                     ///< 0xD002008E - 0xD002008F : Reserved.
    Rsvd24h_t rsvd24h;                                                                             ///< 0xD0020090 : Reserved_24h
    uint8_t rsvd_RD0020092[2];                                                                     ///< 0xD0020092 - 0xD0020093 : Reserved.
    uint16_t RD0020094_15_0_RSVD;                                                                  ///< 0xD0020094 : Reserved_25h
    uint8_t rsvd_RD0020096[2];                                                                     ///< 0xD0020096 - 0xD0020097 : Reserved.
    uint16_t RD0020098_15_0_RSVD;                                                                  ///< 0xD0020098 : Reserved_26h
    uint8_t rsvd_RD002009A[2];                                                                     ///< 0xD002009A - 0xD002009B : Reserved.
    uint16_t RD002009C_15_0_RSVD;                                                                  ///< 0xD002009C : Reserved_27h
    uint8_t rsvd_RD002009E[2];                                                                     ///< 0xD002009E - 0xD002009F : Reserved.
    uint16_t RD00200A0_15_0_RSVD;                                                                  ///< 0xD00200A0 : Reserved_28h
    uint8_t rsvd_RD00200A2[2];                                                                     ///< 0xD00200A2 - 0xD00200A3 : Reserved.
    uint16_t RD00200A4_15_0_RSVD;                                                                  ///< 0xD00200A4 : Reserved_29h
    uint8_t rsvd_RD00200A6[2];                                                                     ///< 0xD00200A6 - 0xD00200A7 : Reserved.
    uint16_t RD00200A8_15_0_RSVD;                                                                  ///< 0xD00200A8 : Reserved_2Ah
    uint8_t rsvd_RD00200AA[2];                                                                     ///< 0xD00200AA - 0xD00200AB : Reserved.
    uint16_t RD00200AC_15_0_RSVD;                                                                  ///< 0xD00200AC : Reserved_2Bh
    uint8_t rsvd_RD00200AE[2];                                                                     ///< 0xD00200AE - 0xD00200AF : Reserved.
    uint16_t RD00200B0_15_0_RSVD;                                                                  ///< 0xD00200B0 : Reserved_2Ch
} Setm3Phy_t;
COMPILE_ASSERT(offsetof(Setm3Phy_t,RD00200B0_15_0_RSVD)==0xD00200B0-0xD0020000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile Setm3Phy_t rSetm3Phy;                ///< 0xD0020000-0xD00200C0 : Setm3Phy_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2013  -  All rights reserved
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
//! @brief Always On Registers
//!
//! Doc Version: NONE
//! Doc Date: NONE
//=============================================================================
/// @cond HW_REG

//-----------------------------------------------------------------------------
//  Always On Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0x1A113020 : Storage 0
typedef union
{
    uint16_t all;                                                                                                                                                                                                                                                                      ///< All bits.
    struct
    {
        uint16_t STORAGE0_L : 8;         ///<BIT [7:0] Signature to distinguish from normal wake up.
        uint16_t STORAGE0_H : 8;         ///<BIT [15:8] General Purpose Storage.
    } b;
} Storage0_t;


///  @brief 0x1A113056 : Clock Generator Control 0
typedef union
{
    uint16_t all;                                                                                                                                                                                                                                                                      ///< All bits.
    struct
    {
        uint16_t FBDIV         :11;         ///<BIT [10:0] TBG Feedback Divider.
        uint16_t GPIO24_O      : 1;         ///<BIT [11] GPIO24 Data Output.
        uint16_t GPIO24_OEN    : 1;         ///<BIT [12] GPIO24 Output Enable.
        uint16_t GPIO22_PE_DET : 1;         ///<BIT [13] GPIO22 Positive Edge Detection Status.
        uint16_t GPIO23_NE_DET : 1;         ///<BIT [14] GPIO23 Negative Edge Detection Status.
        uint16_t GPIO24_PE_DET : 1;         ///<BIT [15] GPIO24 Positive Edge Detection Status.
    } b;
} ClkGeneratoCtrl0_t;


///  @brief 0x1A113058 : Clock Generator Control 1
typedef union
{
    uint16_t all;                                                                                                                                                                                                                                                                      ///< All bits.
    struct
    {
        uint16_t RESERVED_IN       : 8;         ///<BIT [7:0] Reserved.
        uint16_t TEST_ANA          : 3;         ///<BIT [10:8] TESTMON selection.
        uint16_t RESERVED_OUT      : 4;         ///<BIT [14:11] Reserved.
        uint16_t R1A113058_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;
} ClkGeneratoCtrl1_t;


///  @brief 0x1A11305A : Always-On Block Control 0
typedef union
{
    uint16_t all;                                                                                                                                                                                                                                                                      ///< All bits.
    struct
    {
        uint16_t GPIO22_O           : 1;         ///<BIT [0] GPIO22 Data Output.
        uint16_t GPIO23_O           : 1;         ///<BIT [1] GPIO23 Data Output.
        uint16_t GPIO21_O_DEVSLP    : 1;         ///<BIT [2] GPIO21 Output (DEVSLP).
        uint16_t NPOR_O             : 1;         ///<BIT [3] nPOR Data Output.
        uint16_t GPIO22_OEN         : 1;         ///<BIT [4] GPIO22 Output Enable.
        uint16_t GPIO23_OEN         : 1;         ///<BIT [5] GPIO23 Output Enable.
        uint16_t GPIO21_OEN_DEVSLP  : 1;         ///<BIT [6] GPIO21 Output Enable (DEVSLP).
        uint16_t DEVSLP_FLT_DISABLE : 1;         ///<BIT [7] Device Sleep Detection Filter Disable.
        uint16_t GPIO22_I           : 1;         ///<BIT [8] GPIO22 Input.
        uint16_t GPIO23_I           : 1;         ///<BIT [9] GPIO23 Input.
        uint16_t GPIO21_I_DEVSLP    : 1;         ///<BIT [10] GPIO21 Input (Device Sleep Input).
        uint16_t NPOR_I             : 1;         ///<BIT [11] nPOR Input.
        uint16_t DEVSLP_FDET_EN     : 1;         ///<BIT [12] Device Sleep Falling Detect Enable Status.
        uint16_t GPIO24_I           : 1;         ///<BIT [13] GPIO24 Input.
        uint16_t DEVSLP_MAN_RDET    : 1;         ///<BIT [14] Device Sleep Rising Edge Detect.
        uint16_t DEVSLP_MAN_FDET    : 1;         ///<BIT [15] Device Sleep Falling Edge Detect.
    } b;
} AlwaysOnBlkCtrl0_t;


///  @brief 0x1A11305C : Always-On Block Control 1
typedef union
{
    uint16_t all;                                                                                                                                                                                                                                                                      ///< All bits.
    struct
    {
        uint16_t NPOR_SOC_TIMEOUT_SEL : 5;         ///<BIT [4:0] nPOR SOC Timeout Select.
        uint16_t NPOR_SOC_MODE        : 2;         ///<BIT [6:5] nPOR SOC Mode:.
        uint16_t DEVSLP_DET_MODE      : 2;         ///<BIT [8:7] Device Sleep Detection Mode.
        uint16_t GPIO24_PD_EN         : 1;         ///<BIT [9] GPIO24 Pull-Down Enable.
        uint16_t GPIO24_PU_EN         : 1;         ///<BIT [10] GPIO24 Pull-Up Enable.
        uint16_t DEVSLP_LV_MD         : 1;         ///<BIT [11] Device Sleep Input Level for Detection.
        uint16_t AON_BLK_EN           : 1;         ///<BIT [12] Always-On Block Enable.
        uint16_t GPIO20_OEN_NPWRDN    : 1;         ///<BIT [13] GPIO20 Output Enable (nPWRDN).
        uint16_t GPIO20_NPWRDN        : 1;         ///<BIT [14] GPIO20 Data Output (nPWRDN).
        uint16_t GPIO20_I_NPWRDN      : 1;         ///<BIT [15] GPIO20 Input (nPWRDN).
    } b;
} AlwaysOnBlkCtrl1_t;


///  @brief 0x1A11305E : Always-On Block Control 2
typedef union
{
    uint16_t all;                                                                                                                                                                                                                                                                      ///< All bits.
    struct
    {
        uint16_t REFDIV              : 8;         ///<BIT [7:0] Reference Divider Value.
        uint16_t GPIO22_PU_EN        : 1;         ///<BIT [8] GPIO22 Pull-Down Enable.
        uint16_t GPIO22_PD_EN        : 1;         ///<BIT [9] GPIO22 Pull-Down Enable.
        uint16_t GPIO23_PU_EN        : 1;         ///<BIT [10] GPIO23 Pull-Up Enable.
        uint16_t GPIO23_PD_EN        : 1;         ///<BIT [11] GPIO23 Pull-Down Enable.
        uint16_t GPIO20_PU_EN_NPWRDN : 1;         ///<BIT [12] GPIO20 Pull-Up Enable (nPWRDN).
        uint16_t GPIO20_PD_EN_NPDWN  : 1;         ///<BIT [13] GPIO20 Pull-Down Enable (nPWRDN).
        uint16_t ASYNC_EN            : 1;         ///<BIT [14] Asynchronous Detection Mode Enable.
        uint16_t R1A11305E_15_RSVD   : 1;         ///<BIT [15] Reserved.
    } b;
} AlwaysOnBlkCtrl2_t;


///  @brief 0x1A113060 : Always-On Status and Interrupt Control
typedef union
{
    uint16_t all;                                                                                                                                                                                                                                                                      ///< All bits.
    struct
    {
        uint16_t R1A113060_7_0_RSVD : 8;         ///<BIT [7:0] Reserved.
        uint16_t INT_EN_DEVSLP_RDET : 1;         ///<BIT [8] Interrupt Enable Device Sleep Rising Detect.
        uint16_t INT_EN_GPIO22_I    : 1;         ///<BIT [9] Interrupt Enable GPIO22.
        uint16_t INT_EN_GPIO23_I    : 1;         ///<BIT [10] Interrupt Enable GPIO23.
        uint16_t GPIO22_DET_MD      : 1;         ///<BIT [11] GPIO22 Detect Mode.
        uint16_t GPIO23_DET_MD      : 1;         ///<BIT [12] GPIO23 Detect Mode.
        uint16_t DEVSLP_RDET_STS    : 1;         ///<BIT [13] Device Sleep Rising Detect Status.
        uint16_t GPIO0_22_I_STS     : 1;         ///<BIT [14] GPIO22 Interrupt Status.
        uint16_t GPIO23_I_STS       : 1;         ///<BIT [15] GPIO23 Interrupt Status.
    } b;
} AlwaysOnStatAndIntrCtrl_t;


/// @brief 0x1A113020-0x1A113FFF : AlwaysOn_t MODULE
typedef struct
{
    Storage0_t storage0;                                       ///< 0x1A113020 : Storage 0
    uint16_t STORAGE1;                                              ///< 0x1A113022 : Storage 1
    uint8_t rsvd_R1A113024[48];                                     ///< 0x1A113024 - 0x1A113053 : Reserved.
    uint16_t STORAGE1A;                                             ///< 0x1A113054 : Storage 1A
    ClkGeneratoCtrl0_t clkGeneratoCtrl0;                     ///< 0x1A113056 : Clock Generator Control 0
    ClkGeneratoCtrl1_t clkGeneratoCtrl1;                     ///< 0x1A113058 : Clock Generator Control 1
    AlwaysOnBlkCtrl0_t alwaysOnBlkCtrl0;                    ///< 0x1A11305A : Always-On Block Control 0
    AlwaysOnBlkCtrl1_t alwaysOnBlkCtrl1;                    ///< 0x1A11305C : Always-On Block Control 1
    AlwaysOnBlkCtrl2_t alwaysOnBlkCtrl2;                    ///< 0x1A11305E : Always-On Block Control 2
    AlwaysOnStatAndIntrCtrl_t alwaysOnStatAndIntrCtrl;     ///< 0x1A113060 : Always-On Status and Interrupt Control
} AlwaysOn_t;

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile AlwaysOn_t rAlwaysOn;           ///< 0x1A113020-0x1A113FFF : AlwaysOn_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
extern AlwaysOn_t mAlwaysOn;           ///< 0x1A113020-0x1A113FFF : AlwaysOn_t MODULE

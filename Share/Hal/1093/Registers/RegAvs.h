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
//! @brief Avs Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  Avs Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF0114400 : VDD Limit Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AVS_EN_AVDD_DET      : 1;         ///<BIT [0] Adaptive Voltage Scaling Enable AVDD Detect.
        uint32_t AVS_EN               : 1;         ///<BIT [1] Adaptive Voltage Scaling Enable.
        uint32_t RF0114400_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t LOW_VDD_LIM          : 6;         ///<BIT [17:12] Adaptive Voltage Scaling Low VDD Limit.
        uint32_t RF0114400_19_18_RSVD : 2;         ///<BIT [19:18] Reserved.
        uint32_t HIGH_VDD_LMT         : 6;         ///<BIT [25:20] Adaptive Voltage Scaling High VDD Limit.
        uint32_t RF0114400_30_26_RSVD : 5;         ///<BIT [30:26] Reserved.
        uint32_t RF0114400_31_RSVD    : 1;         ///<BIT [31] Reserved.
    } b;    
} VddLimitCtrl_t;

///  @brief 0xF0114404 : Speed Target Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0114404_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SPEED_TARGET        :16;         ///<BIT [31:16] Adaptive Voltage Scaling Speed Target.
    } b;    
} SpeedTgtCtrl_t;

///  @brief 0xF0114418 : Intermediate DRO Count Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0114418_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t INTER_DRO_COUNT     :16;         ///<BIT [31:16] Adaptive Voltage Scaling Intermediate DRO Count.
    } b;    
} IntermedDroCntStat_t;


/// @brief 0xF0114400-0xF0114424 : Avs_t MODULE
typedef struct
{
    VddLimitCtrl_t vddLimitCtrl;                 ///< 0xF0114400 : VDD Limit Control
    SpeedTgtCtrl_t speedTgtCtrl;                 ///< 0xF0114404 : Speed Target Control
    uint8_t rsvd_RF0114408[8];                   ///< 0xF0114408 - 0xF011440F : Reserved.
    uint32_t RF0114410_31_0_RSVD;                ///< 0xF0114410 : Reserved
    uint32_t RF0114414_31_0_RSVD;                ///< 0xF0114414 : Reserved
    IntermedDroCntStat_t intermedDroCntStat;     ///< 0xF0114418 : Intermediate DRO Count Status
    uint32_t RF011441C_31_0_RSVD;                ///< 0xF011441C : Reserved
    uint32_t RF0114420_31_0_RSVD;                ///< 0xF0114420 : Reserved
} Avs_t;
COMPILE_ASSERT(offsetof(Avs_t,RF0114420_31_0_RSVD)==0xF0114420-0xF0114400,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile Avs_t rAvs;                          ///< 0xF0114400-0xF0114424 : Avs_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

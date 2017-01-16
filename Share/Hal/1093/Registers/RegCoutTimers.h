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
//! @brief COUTTIMERS Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  COUTTIMERS Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF1020000 : Counter 0 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CNTR_0_EN            : 1;         ///<BIT [0] Counter 0 Enable.
        uint32_t CNTR_0_ACT           : 1;         ///<BIT [1] Counter 0 Active.
        uint32_t CNTR_0_RW            : 2;         ///<BIT [3:2] Counter 0 Read/Write.
        uint32_t CNTR_0_TRIGGER_SRC   : 4;         ///<BIT [7:4] Counter 0 Trigger Source.
        uint32_t CNTR_0_CLK_PRE       : 8;         ///<BIT [15:8] Counter 0 Clock Prescaler.
        uint32_t RF1020000_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Cntr0Ctrl_t;

///  @brief 0xF1020008 : Interrupt Pending Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CNTR_0_INT_PEND     : 1;         ///<BIT [0] Counter 0 Interrupt Pending Status.
        uint32_t CNTR_1_INT_PEND     : 1;         ///<BIT [1] Counter 1 Interrupt Pending Status.
        uint32_t CNTR_2_INT_PEND     : 1;         ///<BIT [2] Counter 2 Interrupt Pending Status.
        uint32_t CNTR_3_INT_PEND     : 1;         ///<BIT [3] Counter 3 Interrupt Pending Status.
        uint32_t CNTR_4_INT_PEND     : 1;         ///<BIT [4] Counter 4 Interrupt Pending Status.
        uint32_t RF1020008_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} IntrPendingStat_t;

///  @brief 0xF1020010 : Counter 1 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CNTR_1_EN            : 1;         ///<BIT [0] Counter 1 Enable.
        uint32_t CNTR_1_ACT           : 1;         ///<BIT [1] Counter 1 Active.
        uint32_t CNTR_1_RW            : 2;         ///<BIT [3:2] Counter 1 Read/Write.
        uint32_t CNTR_1_TRIGGER_SRC   : 4;         ///<BIT [7:4] Counter 1 Trigger Source.
        uint32_t CNTR_1_CLK_PRE       : 8;         ///<BIT [15:8] Counter 1 Clock Prescaler.
        uint32_t RF1020010_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Cntr1Ctrl_t;

///  @brief 0xF1020020 : Counter 2 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CNTR_2_EN            : 1;         ///<BIT [0] Counter 2 Enable.
        uint32_t CNTR_2_ACT           : 1;         ///<BIT [1] Counter 2 Active.
        uint32_t CNTR_2_RW            : 2;         ///<BIT [3:2] Counter 2 Read/Write.
        uint32_t CNTR_2_TRIGGER_SRC   : 4;         ///<BIT [7:4] Counter 2 Trigger Source.
        uint32_t CNTR_2_CLK_PRE       : 8;         ///<BIT [15:8] Counter 2 Clock Prescaler.
        uint32_t RF1020020_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Cntr2Ctrl_t;

///  @brief 0xF1020030 : Counter 3 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CNTR_3_EN            : 1;         ///<BIT [0] Counter 3 Enable.
        uint32_t CNTR_3_ACT           : 1;         ///<BIT [1] Counter 3 Active.
        uint32_t CNTR_3_RW            : 2;         ///<BIT [3:2] Counter 3 Read/Write.
        uint32_t CNTR_3_TRIGGER_SRC   : 4;         ///<BIT [7:4] Counter 3 Trigger Source.
        uint32_t CNTR_3_CLK_PRE       : 8;         ///<BIT [15:8] Counter 3 Clock Prescaler.
        uint32_t RF1020030_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Cntr3Ctrl_t;

///  @brief 0xF1020040 : Counter 4 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CNTR_4_EN            : 1;         ///<BIT [0] Counter 4 Enable.
        uint32_t CNTR_4_ACT           : 1;         ///<BIT [1] Counter 4 Active.
        uint32_t CNTR_4_RW            : 2;         ///<BIT [3:2] Counter 4 Read/Write.
        uint32_t CNTR_4_TRIG_SRC      : 4;         ///<BIT [7:4] Counter 4 Trigger Source.
        uint32_t CNTR_4_CLK_PRE       : 8;         ///<BIT [15:8] Counter 4 Clock Prescaler.
        uint32_t RF1020040_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Cntr4Ctrl_t;

///  @brief 0xF1020080 : CPU 0 Interrupt Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CNTR_0_INT_MSK      : 1;         ///<BIT [0] Timer 0 Interrupt Mask.
        uint32_t CNTR_1_INT_MSK      : 1;         ///<BIT [1] Timer 1 Interrupt Mask.
        uint32_t CNTR_2_INT_MSK      : 1;         ///<BIT [2] Timer 2 Interrupt Mask.
        uint32_t CNTR_3_INT_MSK      : 1;         ///<BIT [3] Timer 3 Interrupt Mask.
        uint32_t CNTR_4_INT_MSK      : 1;         ///<BIT [4] Timer 4 Interrupt Mask.
        uint32_t RF1020080_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} Cpu0IntrMsk_t;

///  @brief 0xF1020084 : CPU 1 Interrupt Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CNTR_0_INT_MSK      : 1;         ///<BIT [0] Timer 0 Interrupt Mask.
        uint32_t CNTR_1_INT_MSK      : 1;         ///<BIT [1] Timer 1 Interrupt Mask.
        uint32_t CNTR_2_INT_MSK      : 1;         ///<BIT [2] Timer 2 Interrupt Mask.
        uint32_t CNTR_3_INT_MSK      : 1;         ///<BIT [3] Timer 3 Interrupt Mask.
        uint32_t CNTR_4_INT_MSK      : 1;         ///<BIT [4] Timer 4 Interrupt Mask.
        uint32_t RF1020084_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} Cpu1IntrMsk_t;

///  @brief 0xF1020088 : CPU 2 Interrupt Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CNTR_0_INT_MSK      : 1;         ///<BIT [0] Timer 0 Interrupt Mask.
        uint32_t CNTR_1_INT_MSK      : 1;         ///<BIT [1] Timer 1 Interrupt Mask.
        uint32_t CNTR_2_INT_MSK      : 1;         ///<BIT [2] Timer 2 Interrupt Mask.
        uint32_t CNTR_3_INT_MSK      : 1;         ///<BIT [3] Timer 3 Interrupt Mask.
        uint32_t CNTR_4_INT_MSK      : 1;         ///<BIT [4] Timer 4 Interrupt Mask.
        uint32_t RF1020088_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} Cpu2IntrMsk_t;

///  @brief 0xF1020090 : Interrupt clr Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CNTR_0_INT_CLR_MSK  : 1;         ///<BIT [0] Timer 0 Interrupt clr Mask.
        uint32_t CNTR_1_INT_CLR_MSK  : 1;         ///<BIT [1] Timer 1 Interrupt clr Mask.
        uint32_t CNTR_2_INT_CLR_MSK  : 1;         ///<BIT [2] Timer 2 Interrupt clr Mask.
        uint32_t CNTR_3_INT_CLR_MSK  : 1;         ///<BIT [3] Timer 3 Interrupt clr Mask.
        uint32_t CNTR_4_INT_CLR_MSK  : 1;         ///<BIT [4] Timer 4 Interrupt clr Mask.
        uint32_t RF1020090_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} IntrClrMsk_t;


/// @brief 0xF1020000-0xF1020090 : CoutTimers_t MODULE
typedef struct
{
    Cntr0Ctrl_t cntr0Ctrl;                 ///< 0xF1020000 : Counter 0 Control
    uint32_t CNTR_0_INIT_CURR_CNT;         ///< 0xF1020004 : Counter 0 Initial Count/Current Count
    uint32_t RF1020008_31_0_RSVD;          ///< 0xF1020008 : Reserved for R2.0
    uint32_t RF102000C_31_0_RSVD;          ///< 0xF102000C : Reserved
    Cntr1Ctrl_t cntr1Ctrl;                 ///< 0xF1020010 : Counter 1 Control
    uint32_t CNTR_1_INIT_CURR_CNT;         ///< 0xF1020014 : Counter 1 Initial Count/Current Count
    uint32_t RF1020018_31_0_RSVD;          ///< 0xF1020018 : Reserved
    uint32_t RF102001C_31_0_RSVD;          ///< 0xF102001C : Reserved
    Cntr2Ctrl_t cntr2Ctrl;                 ///< 0xF1020020 : Counter 2 Control
    uint32_t CNTR_2_INIT_CURR_CNT;         ///< 0xF1020024 : Counter 2 Initial Count/Current Count
    uint32_t RF1020028_31_0_RSVD;          ///< 0xF1020028 : Reserved
    uint32_t RF102002C_31_0_RSVD;          ///< 0xF102002C : Reserved
    Cntr3Ctrl_t cntr3Ctrl;                 ///< 0xF1020030 : Counter 3 Control
    uint32_t CNTR_3_INIT_CURR_CNT;         ///< 0xF1020034 : Counter 3 Initial Count/Current Count
    uint32_t RF1020038_31_0_RSVD;          ///< 0xF1020038 : Reserved
    uint32_t RF102003C_31_0_RSVD;          ///< 0xF102003C : Reserved
    Cntr4Ctrl_t cntr4Ctrl;                 ///< 0xF1020040 : Counter 4 Control
    uint32_t CNTR_4_INIT_CURR_CNT;         ///< 0xF1020044 : Counter 4 Initial Count/Current Count
    uint32_t RF1020048_31_0_RSVD;          ///< 0xF1020048 : Reserved
    uint32_t RF102004C_31_0_RSVD;          ///< 0xF102004C : Reserved
    uint8_t rsvd_RF1020050[48];            ///< 0xF1020050 - 0xF102007F : Reserved.

    Cpu0IntrMsk_t cpuIntrMsk[3];           ///< 0xF1020080 : Interrupt mask (R1.x) and set mask (R2.0)
    uint32_t RF102008C_31_0_RSVD;          ///< 0xF102008C : Reserved
    
    IntrClrMsk_t cpuIntrClrMsk[3];
    uint32_t RF102009C_31_0_RSVD;          ///< 0xF102009C : Reserved

    IntrPendingStat_t intrPendingStat[3];    ///< 0xF10200A0 : CPU 0,1,2 Interrupt Pending status
} CoutTimers_t;
COMPILE_ASSERT(offsetof(CoutTimers_t,RF102008C_31_0_RSVD)==0xF102008C-0xF1020000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile CoutTimers_t rCoutTimers;            ///< 0xF1020000-0xF10200B0 : CoutTimers_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

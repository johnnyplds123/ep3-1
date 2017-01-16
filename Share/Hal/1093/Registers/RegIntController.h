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
//! @brief Interrupt Controller Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  Interrupt Controller Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF010C00C : Interrupt Controller FIQ Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FIQ_TM               : 4;         ///<BIT [3:0] FIQ Trigger Mode.
        uint32_t FIQ_PLR              : 4;         ///<BIT [7:4] FIQ Polarity.
        uint32_t FIQ_IRP              : 4;         ///<BIT [11:8] FIQ Interrupt Request Pending.
        uint32_t FIQ_IRS              : 4;         ///<BIT [15:12] FIQ Interrupt Request Sent.
        uint32_t FIQ_MK               : 4;         ///<BIT [19:16] FIQ Mask.
        uint32_t SYNC_FIQ             : 4;         ///<BIT [23:20] Synchronous FIQ.
        uint32_t RF010C00C_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} IntrControllFiqCtrl_t;

typedef union
{
    uint32_t all;         ///< All bits.
    struct
    {
        uint32_t R1A10C010_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL               : 5;         ///<BIT [15:11] Priority Level.
        uint32_t TM                   : 1;         ///<BIT [16] Trigger Mode.
        uint32_t PLR                  : 1;         ///<BIT [17] Polarity.
        uint32_t SYNCIRQ              : 1;         ///<BIT [18] Synchronous IRQ.
        uint32_t R1A10C010_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP                  : 1;         ///<BIT [29] Interrupt Request Pending.
        uint32_t IRS                  : 1;         ///<BIT [30] Interrupt Request Sent.
        uint32_t MK                   : 1;         ///<BIT [31] Mask 0.
    } b;
} IntrControllIrqCtrl_t;

///  @brief 0xF010C010 : Interrupt Controller IRQ 0 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C010_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_0             : 5;         ///<BIT [15:11] Priority Level 0.
        uint32_t TM_0                 : 1;         ///<BIT [16] Trigger Mode 0.
        uint32_t PLR_0                : 1;         ///<BIT [17] Polarity 0.
        uint32_t SYNCIRQ_0            : 1;         ///<BIT [18] Synchronous IRQ 0.
        uint32_t RF010C010_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_0                : 1;         ///<BIT [29] Interrupt Request Pending 0.
        uint32_t IRS_0                : 1;         ///<BIT [30] Interrupt Request Sent 0.
        uint32_t MK_0                 : 1;         ///<BIT [31] Mask 0.
    } b;    
} IntrControllIrq0Ctrl_t;

///  @brief 0xF010C014 : Interrupt Controller IRQ 1 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C014_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_1             : 5;         ///<BIT [15:11] Priority Level 1.
        uint32_t TM_1                 : 1;         ///<BIT [16] Trigger Mode 1.
        uint32_t PLR_1                : 1;         ///<BIT [17] Polarity 1.
        uint32_t SYNCIRQ_1            : 1;         ///<BIT [18] Synchronous IRQ 1.
        uint32_t RF010C014_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_1                : 1;         ///<BIT [29] Interrupt Request Pending 1.
        uint32_t IRS_1                : 1;         ///<BIT [30] Interrupt Request Sent 1.
        uint32_t MK_1                 : 1;         ///<BIT [31] Mask 1.
    } b;    
} IntrControllIrq1Ctrl_t;

///  @brief 0xF010C018 : Interrupt Controller IRQ 2 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C018_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_2             : 5;         ///<BIT [15:11] Priority Level 2.
        uint32_t TM_2                 : 1;         ///<BIT [16] Trigger Mode 2.
        uint32_t PLR_2                : 1;         ///<BIT [17] Polarity 2.
        uint32_t SYNCIRQ_2            : 1;         ///<BIT [18] Synchronous IRQ 2.
        uint32_t RF010C018_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_2                : 1;         ///<BIT [29] Interrupt Request Pending 2.
        uint32_t IRS_2                : 1;         ///<BIT [30] Interrupt Request Sent 2.
        uint32_t MK_2                 : 1;         ///<BIT [31] Mask 2.
    } b;    
} IntrControllIrq2Ctrl_t;

///  @brief 0xF010C01C : Interrupt Controller IRQ 3 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C01C_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_3             : 5;         ///<BIT [15:11] Priority Level 3.
        uint32_t TM_3                 : 1;         ///<BIT [16] Trigger Mode 3.
        uint32_t PLR_3                : 1;         ///<BIT [17] Polarity 3.
        uint32_t SYNCIRQ_3            : 1;         ///<BIT [18] Synchronous IRQ 3.
        uint32_t RF010C01C_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_3                : 1;         ///<BIT [29] Interrupt Request Pending 3.
        uint32_t IRS_3                : 1;         ///<BIT [30] Interrupt Request Sent 3.
        uint32_t MK_3                 : 1;         ///<BIT [31] Mask 3.
    } b;    
} IntrControllIrq3Ctrl_t;

///  @brief 0xF010C020 : Interrupt Controller IRQ 4 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C020_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_4             : 5;         ///<BIT [15:11] Priority Level 4.
        uint32_t TM_4                 : 1;         ///<BIT [16] Trigger Mode 4.
        uint32_t PLR_4                : 1;         ///<BIT [17] Polarity 4.
        uint32_t SYNCIRQ_4            : 1;         ///<BIT [18] Synchronous IRQ 4.
        uint32_t RF010C020_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_4                : 1;         ///<BIT [29] Interrupt Request Pending 4.
        uint32_t IRS_4                : 1;         ///<BIT [30] Interrupt Request Sent 4.
        uint32_t MK_4                 : 1;         ///<BIT [31] Mask 4.
    } b;    
} IntrControllIrq4Ctrl_t;

///  @brief 0xF010C024 : Interrupt Controller IRQ 5 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C024_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_5             : 5;         ///<BIT [15:11] Priority Level 5.
        uint32_t TM_5                 : 1;         ///<BIT [16] Trigger Mode 5.
        uint32_t PLR_5                : 1;         ///<BIT [17] Polarity 5.
        uint32_t SYNCIRQ_5            : 1;         ///<BIT [18] Synchronous IRQ 5.
        uint32_t RF010C024_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_5                : 1;         ///<BIT [29] Interrupt Request Pending 5.
        uint32_t IRS_5                : 1;         ///<BIT [30] Interrupt Request Sent 5.
        uint32_t MK_5                 : 1;         ///<BIT [31] Mask 5.
    } b;    
} IntrControllIrq5Ctrl_t;

///  @brief 0xF010C028 : Interrupt Controller IRQ 6 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C028_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_6             : 5;         ///<BIT [15:11] Priority Level 6.
        uint32_t TM_6                 : 1;         ///<BIT [16] Trigger Mode 6.
        uint32_t PLR_6                : 1;         ///<BIT [17] Polarity 6.
        uint32_t SYNCIRQ_6            : 1;         ///<BIT [18] Synchronous IRQ 6.
        uint32_t RF010C028_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_6                : 1;         ///<BIT [29] Interrupt Request Pending 6.
        uint32_t IRS_6                : 1;         ///<BIT [30] Interrupt Request Sent 6.
        uint32_t MK_6                 : 1;         ///<BIT [31] Mask 6.
    } b;    
} IntrControllIrq6Ctrl_t;

///  @brief 0xF010C02C : Interrupt Controller IRQ 7 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C02C_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_7             : 5;         ///<BIT [15:11] Priority Level 7.
        uint32_t TM_7                 : 1;         ///<BIT [16] Trigger Mode 7.
        uint32_t PLR_7                : 1;         ///<BIT [17] Polarity 7.
        uint32_t SYNCIRQ_7            : 1;         ///<BIT [18] Synchronous IRQ 7.
        uint32_t RF010C02C_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_7                : 1;         ///<BIT [29] Interrupt Request Pending 7.
        uint32_t IRS_7                : 1;         ///<BIT [30] Interrupt Request Sent 7.
        uint32_t MK_7                 : 1;         ///<BIT [31] Mask 7.
    } b;    
} IntrControllIrq7Ctrl_t;

///  @brief 0xF010C030 : Interrupt Controller IRQ 8 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C030_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_8             : 5;         ///<BIT [15:11] Priority Level 8.
        uint32_t TM_8                 : 1;         ///<BIT [16] Trigger Mode 8.
        uint32_t PLR_8                : 1;         ///<BIT [17] Polarity 8.
        uint32_t SYNCIRQ_8            : 1;         ///<BIT [18] Synchronous IRQ 8.
        uint32_t RF010C030_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_8                : 1;         ///<BIT [29] Interrupt Request Pending 8.
        uint32_t IRS_8                : 1;         ///<BIT [30] Interrupt Request Sent 8.
        uint32_t MK_8                 : 1;         ///<BIT [31] Mask 8.
    } b;    
} IntrControllIrq8Ctrl_t;

///  @brief 0xF010C034 : Interrupt Controller IRQ 9 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C034_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_9             : 5;         ///<BIT [15:11] Priority Level 9.
        uint32_t TM_9                 : 1;         ///<BIT [16] Trigger Mode 9.
        uint32_t PLR_9                : 1;         ///<BIT [17] Polarity 9.
        uint32_t SYNCIRQ_9            : 1;         ///<BIT [18] Synchronous IRQ 9.
        uint32_t RF010C034_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_9                : 1;         ///<BIT [29] Interrupt Request Pending 9.
        uint32_t IRS_9                : 1;         ///<BIT [30] Interrupt Request Sent 9.
        uint32_t MK_9                 : 1;         ///<BIT [31] Mask 9.
    } b;    
} IntrControllIrq9Ctrl_t;

///  @brief 0xF010C038 : Interrupt Controller IRQ 10 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C038_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_10            : 5;         ///<BIT [15:11] Priority Level 10.
        uint32_t TM_10                : 1;         ///<BIT [16] Trigger Mode 10.
        uint32_t PLR_10               : 1;         ///<BIT [17] Polarity 10.
        uint32_t SYNCIRQ_10           : 1;         ///<BIT [18] Synchronous IRQ 10.
        uint32_t RF010C038_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_10               : 1;         ///<BIT [29] Interrupt Request Pending 10.
        uint32_t IRS_10               : 1;         ///<BIT [30] Interrupt Request Sent 10.
        uint32_t MK_10                : 1;         ///<BIT [31] Mask 10.
    } b;    
} IntrControllIrq10Ctrl_t;

///  @brief 0xF010C03C : Interrupt Controller IRQ 11 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C03C_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_11            : 5;         ///<BIT [15:11] Priority Level 11.
        uint32_t TM_11                : 1;         ///<BIT [16] Trigger Mode 11.
        uint32_t PLR_11               : 1;         ///<BIT [17] Polarity 11.
        uint32_t SYNCIRQ_11           : 1;         ///<BIT [18] Synchronous IRQ 11.
        uint32_t RF010C03C_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_11               : 1;         ///<BIT [29] Interrupt Request Pending 11.
        uint32_t IRS_11               : 1;         ///<BIT [30] Interrupt Request Sent 11.
        uint32_t MK_11                : 1;         ///<BIT [31] Mask 11.
    } b;    
} IntrControllIrq11Ctrl_t;

///  @brief 0xF010C040 : Interrupt Controller IRQ 12 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C040_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_12            : 5;         ///<BIT [15:11] Priority Level 12.
        uint32_t TM_12                : 1;         ///<BIT [16] Trigger Mode 12.
        uint32_t PLR_12               : 1;         ///<BIT [17] Polarity 12.
        uint32_t SYNCIRQ_12           : 1;         ///<BIT [18] Synchronous IRQ 12.
        uint32_t RF010C040_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_12               : 1;         ///<BIT [29] Interrupt Request Pending 12.
        uint32_t IRS_12               : 1;         ///<BIT [30] Interrupt Request Sent 12.
        uint32_t MK_12                : 1;         ///<BIT [31] Mask 12.
    } b;    
} IntrControllIrq12Ctrl_t;

///  @brief 0xF010C044 : Interrupt Controller IRQ 13 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C044_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_13            : 5;         ///<BIT [15:11] Priority Level 13.
        uint32_t TM_13                : 1;         ///<BIT [16] Trigger Mode 13.
        uint32_t PLR_13               : 1;         ///<BIT [17] Polarity 13.
        uint32_t SYNCIRQ_13           : 1;         ///<BIT [18] Synchronous IRQ 13.
        uint32_t RF010C044_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_13               : 1;         ///<BIT [29] Interrupt Request Pending 13.
        uint32_t IRS_13               : 1;         ///<BIT [30] Interrupt Request Sent 13.
        uint32_t MK_13                : 1;         ///<BIT [31] Mask 13.
    } b;    
} IntrControllIrq13Ctrl_t;

///  @brief 0xF010C048 : Interrupt Controller IRQ 14 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C048_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_14            : 5;         ///<BIT [15:11] Priority Level 14.
        uint32_t TM_14                : 1;         ///<BIT [16] Trigger Mode 14.
        uint32_t PLR_14               : 1;         ///<BIT [17] Polarity 14.
        uint32_t SYNCIRQ_14           : 1;         ///<BIT [18] Synchronous IRQ 14.
        uint32_t RF010C048_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_14               : 1;         ///<BIT [29] Interrupt Request Pending 14.
        uint32_t IRS_14               : 1;         ///<BIT [30] Interrupt Request Sent 14.
        uint32_t MK_14                : 1;         ///<BIT [31] Mask 14.
    } b;    
} IntrControllIrq14Ctrl_t;

///  @brief 0xF010C04C : Interrupt Controller IRQ 15 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C04C_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_15            : 5;         ///<BIT [15:11] Priority Level 15.
        uint32_t TM_15                : 1;         ///<BIT [16] Trigger Mode 15.
        uint32_t PLR_15               : 1;         ///<BIT [17] Polarity 15.
        uint32_t SYNCIRQ_15           : 1;         ///<BIT [18] Synchronous IRQ 15.
        uint32_t RF010C04C_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_15               : 1;         ///<BIT [29] Interrupt Request Pending 15.
        uint32_t IRS_15               : 1;         ///<BIT [30] Interrupt Request Sent 15.
        uint32_t MK_15                : 1;         ///<BIT [31] Mask 15.
    } b;    
} IntrControllIrq15Ctrl_t;

///  @brief 0xF010C050 : Interrupt Controller IRQ 16 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C050_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_16            : 5;         ///<BIT [15:11] Priority Level 16.
        uint32_t TM_16                : 1;         ///<BIT [16] Trigger Mode 16.
        uint32_t PLR_16               : 1;         ///<BIT [17] Polarity 16.
        uint32_t SYNCIRQ_16           : 1;         ///<BIT [18] Synchronous IRQ 16.
        uint32_t RF010C050_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_16               : 1;         ///<BIT [29] Interrupt Request Pending 16.
        uint32_t IRS_16               : 1;         ///<BIT [30] Interrupt Request Sent 16.
        uint32_t MK_16                : 1;         ///<BIT [31] Mask 16.
    } b;    
} IntrControllIrq16Ctrl_t;

///  @brief 0xF010C054 : Interrupt Controller IRQ 17 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C054_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_17            : 5;         ///<BIT [15:11] Priority Level 17.
        uint32_t TM_17                : 1;         ///<BIT [16] Trigger Mode 17.
        uint32_t PLR_17               : 1;         ///<BIT [17] Polarity 17.
        uint32_t SYNCIRQ_17           : 1;         ///<BIT [18] Synchronous IRQ 17.
        uint32_t RF010C054_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_17               : 1;         ///<BIT [29] Interrupt Request Pending 17.
        uint32_t IRS_17               : 1;         ///<BIT [30] Interrupt Request Sent 17.
        uint32_t MK_17                : 1;         ///<BIT [31] Mask 17.
    } b;    
} IntrControllIrq17Ctrl_t;

///  @brief 0xF010C058 : Interrupt Controller IRQ 18 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C058_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_18            : 5;         ///<BIT [15:11] Priority Level 18.
        uint32_t TM_18                : 1;         ///<BIT [16] Trigger Mode 18.
        uint32_t PLR_18               : 1;         ///<BIT [17] Polarity 18.
        uint32_t SYNCIRQ_18           : 1;         ///<BIT [18] Synchronous IRQ 18.
        uint32_t RF010C058_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_18               : 1;         ///<BIT [29] Interrupt Request Pending 18.
        uint32_t IRS_18               : 1;         ///<BIT [30] Interrupt Request Sent 18.
        uint32_t MK_18                : 1;         ///<BIT [31] Mask 18.
    } b;    
} IntrControllIrq18Ctrl_t;

///  @brief 0xF010C05C : Interrupt Controller IRQ 19 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C05C_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_19            : 5;         ///<BIT [15:11] Priority Level 19.
        uint32_t TM_19                : 1;         ///<BIT [16] Trigger Mode 19.
        uint32_t PLR_19               : 1;         ///<BIT [17] Polarity 19.
        uint32_t SYNCIRQ_19           : 1;         ///<BIT [18] Synchronous IRQ 19.
        uint32_t RF010C05C_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_19               : 1;         ///<BIT [29] Interrupt Request Pending 19.
        uint32_t IRS_19               : 1;         ///<BIT [30] Interrupt Request Sent 19.
        uint32_t MK_19                : 1;         ///<BIT [31] Mask 19.
    } b;    
} IntrControllIrq19Ctrl_t;

///  @brief 0xF010C060 : Interrupt Controller IRQ 20 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C060_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_20            : 5;         ///<BIT [15:11] Priority Level 20.
        uint32_t TM_20                : 1;         ///<BIT [16] Trigger Mode 20.
        uint32_t PLR_20               : 1;         ///<BIT [17] Polarity 20.
        uint32_t SYNCIRQ_20           : 1;         ///<BIT [18] Synchronous IRQ 20.
        uint32_t RF010C060_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_20               : 1;         ///<BIT [29] Interrupt Request Pending 20.
        uint32_t IRS_20               : 1;         ///<BIT [30] Interrupt Request Sent 20.
        uint32_t MK_20                : 1;         ///<BIT [31] Mask 20.
    } b;    
} IntrControllIrq20Ctrl_t;

///  @brief 0xF010C064 : Interrupt Controller IRQ 21 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C064_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_21            : 5;         ///<BIT [15:11] Priority Level 21.
        uint32_t TM_21                : 1;         ///<BIT [16] Trigger Mode 21.
        uint32_t PLR_21               : 1;         ///<BIT [17] Polarity 21.
        uint32_t SYNCIRQ_21           : 1;         ///<BIT [18] Synchronous IRQ 21.
        uint32_t RF010C064_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_21               : 1;         ///<BIT [29] Interrupt Request Pending 21.
        uint32_t IRS_21               : 1;         ///<BIT [30] Interrupt Request Sent 21.
        uint32_t MK_21                : 1;         ///<BIT [31] Mask 21.
    } b;    
} IntrControllIrq21Ctrl_t;

///  @brief 0xF010C068 : Interrupt Controller IRQ 22 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C068_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_22            : 5;         ///<BIT [15:11] Priority Level 22.
        uint32_t TM_22                : 1;         ///<BIT [16] Trigger Mode 22.
        uint32_t PLR_22               : 1;         ///<BIT [17] Polarity 22.
        uint32_t SYNCIRQ_22           : 1;         ///<BIT [18] Synchronous IRQ 22.
        uint32_t RF010C068_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_22               : 1;         ///<BIT [29] Interrupt Request Pending 22.
        uint32_t IRS_22               : 1;         ///<BIT [30] Interrupt Request Sent 22.
        uint32_t MK_22                : 1;         ///<BIT [31] Mask 22.
    } b;    
} IntrControllIrq22Ctrl_t;

///  @brief 0xF010C06C : Interrupt Controller IRQ 23 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C06C_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_23            : 5;         ///<BIT [15:11] Priority Level 23.
        uint32_t TM_23                : 1;         ///<BIT [16] Trigger Mode 23.
        uint32_t PLR_23               : 1;         ///<BIT [17] Polarity 23.
        uint32_t SYNCIRQ_23           : 1;         ///<BIT [18] Synchronous IRQ 23.
        uint32_t RF010C06C_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_23               : 1;         ///<BIT [29] Interrupt Request Pending 23.
        uint32_t IRS_23               : 1;         ///<BIT [30] Interrupt Request Sent 23.
        uint32_t MK_23                : 1;         ///<BIT [31] Mask 23.
    } b;    
} IntrControllIrq23Ctrl_t;

///  @brief 0xF010C070 : Interrupt Controller IRQ 24 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C070_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_24            : 5;         ///<BIT [15:11] Priority Level 24.
        uint32_t TM_24                : 1;         ///<BIT [16] Trigger Mode 24.
        uint32_t PLR_24               : 1;         ///<BIT [17] Polarity 24.
        uint32_t SYNCIRQ_24           : 1;         ///<BIT [18] Synchronous IRQ 24.
        uint32_t RF010C070_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_24               : 1;         ///<BIT [29] Interrupt Request Pending 24.
        uint32_t IRS_24               : 1;         ///<BIT [30] Interrupt Request Sent 24.
        uint32_t MK_24                : 1;         ///<BIT [31] Mask 24.
    } b;    
} IntrControllIrq24Ctrl_t;

///  @brief 0xF010C074 : Interrupt Controller IRQ 25 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C074_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_25            : 5;         ///<BIT [15:11] Priority Level 25.
        uint32_t TM_25                : 1;         ///<BIT [16] Trigger Mode 25.
        uint32_t PLR_25               : 1;         ///<BIT [17] Polarity 25.
        uint32_t SYNCIRQ_25           : 1;         ///<BIT [18] Synchronous IRQ 25.
        uint32_t RF010C074_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_25               : 1;         ///<BIT [29] Interrupt Request Pending 25.
        uint32_t IRS_25               : 1;         ///<BIT [30] Interrupt Request Sent 25.
        uint32_t MK_25                : 1;         ///<BIT [31] Mask 25.
    } b;    
} IntrControllIrq25Ctrl_t;

///  @brief 0xF010C078 : Interrupt Controller IRQ 26 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C078_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_26            : 5;         ///<BIT [15:11] Priority Level 26.
        uint32_t TM_26                : 1;         ///<BIT [16] Trigger Mode 26.
        uint32_t PLR_26               : 1;         ///<BIT [17] Polarity 26.
        uint32_t SYNCIRQ_26           : 1;         ///<BIT [18] Synchronous IRQ 26.
        uint32_t RF010C078_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_26               : 1;         ///<BIT [29] Interrupt Request Pending 26.
        uint32_t IRS_26               : 1;         ///<BIT [30] Interrupt Request Sent 26.
        uint32_t MK_26                : 1;         ///<BIT [31] Mask 26.
    } b;    
} IntrControllIrq26Ctrl_t;

///  @brief 0xF010C07C : Interrupt Controller IRQ 27 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C07C_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_27            : 5;         ///<BIT [15:11] Priority Level 27.
        uint32_t TM_27                : 1;         ///<BIT [16] Trigger Mode 27.
        uint32_t PLR_27               : 1;         ///<BIT [17] Polarity 27.
        uint32_t SYNCIRQ_27           : 1;         ///<BIT [18] Synchronous IRQ 27.
        uint32_t RF010C07C_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_27               : 1;         ///<BIT [29] Interrupt Request Pending 27.
        uint32_t IRS_27               : 1;         ///<BIT [30] Interrupt Request Sent 27.
        uint32_t MK_27                : 1;         ///<BIT [31] Mask 27.
    } b;    
} IntrControllIrq27Ctrl_t;

///  @brief 0xF010C080 : Interrupt Controller IRQ 28 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C080_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_28            : 5;         ///<BIT [15:11] Priority Level 28.
        uint32_t TM_28                : 1;         ///<BIT [16] Trigger Mode 28.
        uint32_t PLR_28               : 1;         ///<BIT [17] Polarity 28.
        uint32_t SYNCIRQ_28           : 1;         ///<BIT [18] Synchronous IRQ 28.
        uint32_t RF010C080_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_28               : 1;         ///<BIT [29] Interrupt Request Pending 28.
        uint32_t IRS_28               : 1;         ///<BIT [30] Interrupt Request Sent 28.
        uint32_t MK_28                : 1;         ///<BIT [31] Mask 28.
    } b;    
} IntrControllIrq28Ctrl_t;

///  @brief 0xF010C084 : Interrupt Controller IRQ 29 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C084_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_29            : 5;         ///<BIT [15:11] Priority Level 29.
        uint32_t TM_29                : 1;         ///<BIT [16] Trigger Mode 29.
        uint32_t PLR_29               : 1;         ///<BIT [17] Polarity 29.
        uint32_t SYNCIRQ_29           : 1;         ///<BIT [18] Synchronous IRQ 29.
        uint32_t RF010C084_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_29               : 1;         ///<BIT [29] Interrupt Request Pending 29.
        uint32_t IRS_29               : 1;         ///<BIT [30] Interrupt Request Sent 29.
        uint32_t MK_29                : 1;         ///<BIT [31] Mask 29.
    } b;    
} IntrControllIrq29Ctrl_t;

///  @brief 0xF010C088 : Interrupt Controller IRQ 30 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C088_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_30            : 5;         ///<BIT [15:11] Priority Level 30.
        uint32_t TM_30                : 1;         ///<BIT [16] Trigger Mode 30.
        uint32_t PLR_30               : 1;         ///<BIT [17] Polarity 30.
        uint32_t SYNCIRQ_30           : 1;         ///<BIT [18] Synchronous IRQ 30.
        uint32_t RF010C088_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_30               : 1;         ///<BIT [29] Interrupt Request Pending 30.
        uint32_t IRS_30               : 1;         ///<BIT [30] Interrupt Request Sent 30.
        uint32_t MK_30                : 1;         ///<BIT [31] Mask 30.
    } b;    
} IntrControllIrq30Ctrl_t;

///  @brief 0xF010C08C : Interrupt Controller IRQ 31 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010C08C_10_0_RSVD  :11;         ///<BIT [10:0] Reserved.
        uint32_t PRILVL_31            : 5;         ///<BIT [15:11] Priority Level 31.
        uint32_t TM_31                : 1;         ///<BIT [16] Trigger Mode 31.
        uint32_t PLR_31               : 1;         ///<BIT [17] Polarity 31.
        uint32_t SYNCIRQ_31           : 1;         ///<BIT [18] Synchronous IRQ 31.
        uint32_t RF010C08C_28_19_RSVD :10;         ///<BIT [28:19] Reserved.
        uint32_t IRP_31               : 1;         ///<BIT [29] Interrupt Request Pending 31.
        uint32_t IRS_31               : 1;         ///<BIT [30] Interrupt Request Sent 31.
        uint32_t MK_31                : 1;         ///<BIT [31] Mask 31.
    } b;    
} IntrControllIrq31Ctrl_t;

///  @brief 0xF010C0A0 : Interrupt Controller Test Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EN_IRQ_TST_SRC      : 1;         ///<BIT [0] Enable IRQ Test Source.
        uint32_t EN_FIQ_TST_SRC      : 1;         ///<BIT [1] Enable FIQ Test Source.
        uint32_t RF010C0A0_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} IntrControllTstCtrl_t;

///  @brief 0xF010C0A8 : Interrupt Controller FIQ Current Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FIQ_SOURCE_0        : 1;         ///<BIT [0] FIQ Source [0].
        uint32_t FIQ_SOURCE_1        : 1;         ///<BIT [1] FIQ Source [1].
        uint32_t FIQ_SOURCE_2        : 1;         ///<BIT [2] FIQ Source [2].
        uint32_t FIQ_SOURCE_3        : 1;         ///<BIT [3] FIQ Source [3].
        uint32_t RF010C0A8_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} IntrControllFiqCurIntr_t;

///  @brief 0xF010C0B0 : Interrupt Controller Set FIQ Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FIQ_INT_MSK_SET     : 4;         ///<BIT [3:0] FIQ Interrupt masks.
        uint32_t RF010C0B0_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} IntrControllSetFiqMsk_t;

///  @brief 0xF010C0B4 : Interrupt Controller Clear FIQ Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FIQ_INT_MSK_CLR     : 4;         ///<BIT [3:0] FIQ Interrupt Mask Clear.
        uint32_t RF010C0B4_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} IntrControllClrFiqMsk_t;

///  @brief 0xF010C0B8 : FIQ End of Interrupt (EOI)
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLR_FIQ_IRS_0       : 1;         ///<BIT [0] Clear FIQ Interrupt Sent [0].
        uint32_t CLR_FIQ_IRS_1       : 1;         ///<BIT [1] Clear FIQ Interrupt Sent [1].
        uint32_t CLR_FIQ_IRS_2       : 1;         ///<BIT [2] Clear FIQ Interrupt Sent [2].
        uint32_t CLR_FIQ_IRS_3       : 1;         ///<BIT [3] Clear FIQ Interrupt Sent [3].
        uint32_t RF010C0B8_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} FiqEndOfIntrEoi_t;

///  @brief 0xF010C0BC : FIQ Raw Interrupt Source
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FIQ_RAW_SOURCE      : 4;         ///<BIT [3:0] FIQ Raw Source.
        uint32_t RF010C0BC_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} FiqRawIntrSrc_t;

///  @brief 0xF010C0C0 : Interrupt Controller Software Interrupt 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SWINT_0             : 1;         ///<BIT [0] Software Interrupt Control 0.
        uint32_t RF010C0C0_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} IntrControllSwIntr0_t;

///  @brief 0xF010C0D4 : Interrupt Controller Software Interrupt 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SWINT_5             : 1;         ///<BIT [0] Software Interrupt Control 5.
        uint32_t RF010C0D4_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} IntrControllSwIntr5_t;

///  @brief 0xF010C0D8 : FIQ Interrupt Request Pending
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FIQIRP_0            : 1;         ///<BIT [0] State Element for the FIQ_SOURCE[0] Interrupt.
        uint32_t FIQIRP_1            : 1;         ///<BIT [1] State Element for the FIQ_SOURCE[1] Interrupt.
        uint32_t FIQIRP_2            : 1;         ///<BIT [2] State Element for the FIQ_SOURCE[2] Interrupt.
        uint32_t FIQIRP_3            : 1;         ///<BIT [3] State Element for the FIQ_SOURCE[3] Interrupt.
        uint32_t RF010C0D8_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} FiqIntrReqPending_t;

///  @brief 0xF010C0DC : FIQ Interrupt Request Sent
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FIQ_IRS_0           : 1;         ///<BIT [0] Interrupt Sent State Element for the FIQ_SOURCE[0] (RF010C0A8h .
        uint32_t FIQ_IRS_1           : 1;         ///<BIT [1] Interrupt Sent State Element for the FIQ_SOURCE[1] (RF010C0A8h .
        uint32_t FIQ_IRS_2           : 1;         ///<BIT [2] Interrupt Sent State Element for the FIQ_SOURCE[2] (RF010C0A8h .
        uint32_t FIQ_IRS_3           : 1;         ///<BIT [3] Interrupt Sent State Element for the FIQ_SOURCE[3] (RF010C0A8h .
        uint32_t RF010C0DC_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} FiqIntrReqSent_t;

///  @brief 0xF010C0E0 : Test FIQ Source
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FIQ_TST_SOURCE      : 4;         ///<BIT [3:0] FIQ Test Source.
        uint32_t RF010C0E0_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} TstFiqSrc_t;


/// @brief 0xF010C000-0xF010C180 : Intc_t MODULE
typedef struct
{
    uint32_t IC_IRQ_INT_BASE;                            ///< 0xF010C000 : Interrupt Controller IRQ Current Interrupt
    uint32_t IRQ_INT_MSK_SET;                            ///< 0xF010C004 : Interrupt Controller Set Interrupt Mask
    uint32_t IRQ_INT_MSK_CLR;                            ///< 0xF010C008 : Interrupt Controller Clear Interrupt Mask
    IntrControllFiqCtrl_t intrControllFiqCtrl;           ///< 0xF010C00C : Interrupt Controller FIQ Control
    IntrControllIrqCtrl_t intrControllIrqCtrl[32];
    uint32_t CLR_IRQ_IRS;                                ///< 0xF010C090 : Interrupt Controller End of Interrupt
    uint32_t IRQ_RAW_SOURCE;                             ///< 0xF010C094 : Interrupt Controller Raw Interrupt
    uint32_t IRQ_SOURCE_IRP;                             ///< 0xF010C098 : Interrupt Controller Interrupt Request Pending
    uint32_t IRQ_SOURCE_IRS;                             ///< 0xF010C09C : Interrupt Controller Interrupt Request Sent
    IntrControllTstCtrl_t intrControllTstCtrl;           ///< 0xF010C0A0 : Interrupt Controller Test Control
    uint32_t IRQ_TST_SOURCE;                             ///< 0xF010C0A4 : Interrupt Controller Test Source
    IntrControllFiqCurIntr_t intrControllFiqCurIntr;     ///< 0xF010C0A8 : Interrupt Controller FIQ Current Interrupt
    uint8_t rsvd_RF010C0AC[4];                           ///< 0xF010C0AC - 0xF010C0AF : Reserved.
    IntrControllSetFiqMsk_t intrControllSetFiqMsk;       ///< 0xF010C0B0 : Interrupt Controller Set FIQ Mask
    IntrControllClrFiqMsk_t intrControllClrFiqMsk;       ///< 0xF010C0B4 : Interrupt Controller Clear FIQ Mask
    FiqEndOfIntrEoi_t fiqEndOfIntrEoi;                   ///< 0xF010C0B8 : FIQ End of Interrupt (EOI)
    FiqRawIntrSrc_t fiqRawIntrSrc;                       ///< 0xF010C0BC : FIQ Raw Interrupt Source
    IntrControllSwIntr0_t intrControllSwIntr0;           ///< 0xF010C0C0 : Interrupt Controller Software Interrupt 0
    uint8_t rsvd_RF010C0C4[16];                          ///< 0xF010C0C4 - 0xF010C0D3 : Reserved.
    IntrControllSwIntr5_t intrControllSwIntr5;           ///< 0xF010C0D4 : Interrupt Controller Software Interrupt 5
    FiqIntrReqPending_t fiqIntrReqPending;               ///< 0xF010C0D8 : FIQ Interrupt Request Pending
    FiqIntrReqSent_t fiqIntrReqSent;                     ///< 0xF010C0DC : FIQ Interrupt Request Sent
    TstFiqSrc_t tstFiqSrc;                               ///< 0xF010C0E0 : Test FIQ Source
    uint8_t rsvd_RF010C0E4[28];                          ///< 0xF010C0E4 - 0xF010C0FF : Reserved.
    uint32_t intrControllIrqVector[32];                  ///< 0x1A10C100 : Interrupt Controller IRQ 0 - 31 Vectors
} Intc_t;
COMPILE_ASSERT(offsetof(Intc_t,intrControllIrqVector[31])==0xF010C17C-0xF010C000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile Intc_t rIntc0;                        ///< 0xF010C000-0xF010C180 : Intc_t MODULE 0
extern volatile Intc_t rIntc1;                        ///< 0xF010C400-0xF010C580 : Intc_t MODULE 1
extern volatile Intc_t rIntc2;                        ///< 0xF010C800-0xF010C980 : Intc_t MODULE 2


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
///  @brief 0xF0020010 : BAR0 And BAR Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BAR0_ENABLE : 1;         ///<BIT [0] BAR0_ENABLE.
        uint32_t BAR0_MASK   :31;         ///<BIT [31:1] BAR0_MASK.
    } b;    
} Bar0AndBarMsk_t;

///  @brief 0xF0020014 : BAR1 And BAR Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BAR1_ENABLE : 1;         ///<BIT [0] BAR1_ENABLE.
        uint32_t BAR1_MASK   :31;         ///<BIT [31:1] BAR1_MASK.
    } b;    
} Bar1AndBarMsk_t;

///  @brief 0xF0020018 : BAR2 And BAR Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BAR2_ENABLE : 1;         ///<BIT [0] BAR2_ENABLE.
        uint32_t BAR2_MASK   :31;         ///<BIT [31:1] BAR2_MASK.
    } b;    
} Bar2AndBarMsk_t;

///  @brief 0xF002001C : BAR3 And BAR Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BAR3_ENABLE : 1;         ///<BIT [0] BAR3_ENABLE.
        uint32_t BAR3_MASK   :31;         ///<BIT [31:1] BAR3_MASK.
    } b;    
} Bar3AndBarMsk_t;

///  @brief 0xF0020020 : BAR4 And BAR Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BAR4_ENABLE : 1;         ///<BIT [0] BAR4_ENABLE.
        uint32_t BAR4_MASK   :31;         ///<BIT [31:1] BAR4_MASK.
    } b;    
} Bar4AndBarMsk_t;

///  @brief 0xF0020024 : BAR5 And BAR Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BAR5_ENABLE : 1;         ///<BIT [0] BAR5_ENABLE.
        uint32_t BAR5_MASK   :31;         ///<BIT [31:1] BAR5_MASK.
    } b;    
} Bar5AndBarMsk_t;

///  @brief 0xF0020030 : Expansion ROM BAR And BAR Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ROM_ENABLE : 1;         ///<BIT [0] ROM_ENABLE.
        uint32_t ROM_MASK   :31;         ///<BIT [31:1] rom enable.
    } b;    
} ExpansioRomBarAndBarMsk_t;

///  @brief 0xF00201C4 : TotalVFs And InitialVFs
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INITIALVFS :16;         ///<BIT [15:0] InitialVFs.
        uint32_t TOTALVFS   :16;         ///<BIT [31:16] TotalVFs.
    } b;    
} TotalvfsAndInitialv11_t;

///  @brief 0xF00201CC : VF Stride And Offset
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VF_OFFSET :16;         ///<BIT [15:0] First VF Offset.
        uint32_t VF_STRIDE :16;         ///<BIT [31:16] VF Stride.
    } b;    
} VfStrideAndOs11_t;

///  @brief 0xF00201DC : VF BAR0 And BAR Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ENABLE_0 : 1;         ///<BIT [0] ENABLE.
        uint32_t MASK     :31;         ///<BIT [31:1] MASK.
    } b;    
} VfBar0AndBarMsk_t;

///  @brief 0xF00201E0 : VF BAR1 And BAR Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ENABLE_1 : 1;         ///<BIT [0] ENABLE.
        uint32_t MASK     :31;         ///<BIT [31:1] MASK.
    } b;    
} VfBar1AndBarMsk_t;

///  @brief 0xF00201E4 : VF BAR2 And BAR Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ENABLE_2 : 1;         ///<BIT [0] ENABLE.
        uint32_t MASK     :31;         ///<BIT [31:1] MASK.
    } b;    
} VfBar2AndBarMsk_t;

///  @brief 0xF00201E8 : VF BAR3 And BAR Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ENABLE_3 : 1;         ///<BIT [0] ENABLE.
        uint32_t MASK     :31;         ///<BIT [31:1] MASK.
    } b;    
} VfBar3AndBarMsk_t;

///  @brief 0xF00201EC : VF BAR4 And BAR Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ENABLE_4 : 1;         ///<BIT [0] ENABLE.
        uint32_t MASK     :31;         ///<BIT [31:1] MASK.
    } b;    
} VfBar4AndBarMsk_t;

///  @brief 0xF00201F0 : VF BAR5 And BAR Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ENABLE_5 : 1;         ///<BIT [0] ENABLE.
        uint32_t MASK     :31;         ///<BIT [31:1] MASK.
    } b;    
} VfBar5AndBarMsk_t;


/// @brief 0xF0020010-0xF0020200 : PcieShadow_t MODULE
typedef struct
{
    Bar0AndBarMsk_t bar0AndBarMsk;                         ///< 0xF0020010 : BAR0 And BAR Mask
    Bar1AndBarMsk_t bar1AndBarMsk;                         ///< 0xF0020014 : BAR1 And BAR Mask
    Bar2AndBarMsk_t bar2AndBarMsk;                         ///< 0xF0020018 : BAR2 And BAR Mask
    Bar3AndBarMsk_t bar3AndBarMsk;                         ///< 0xF002001C : BAR3 And BAR Mask
    Bar4AndBarMsk_t bar4AndBarMsk;                         ///< 0xF0020020 : BAR4 And BAR Mask
    Bar5AndBarMsk_t bar5AndBarMsk;                         ///< 0xF0020024 : BAR5 And BAR Mask
    uint8_t rsvd_RF0020028[8];                             ///< 0xF0020028 - 0xF002002F : Reserved.
    ExpansioRomBarAndBarMsk_t expansioRomBarAndBarMsk;     ///< 0xF0020030 : Expansion ROM BAR And BAR Mask
    uint8_t rsvd_RF0020034[368];                           ///< 0xF0020034 - 0xF00201A3 : Reserved.
    TotalvfsAndInitialv11_t totalvfsAndInitialv1_1;        ///< 0xF00201A4 : Total VF and Initial VF 1
    uint8_t rsvd_RF00201C8[4];                             ///< 0xF00201A8 - 0xF00201AB : Reserved.
    VfStrideAndOs11_t vfStrideAndOs1_1;                    ///< 0xF00201AC : VF Stride and Offset 1
    uint8_t rsvd_RF00201D0[12];                            ///< 0xF00201B0 - 0xF00201BB : Reserved.
    VfBar0AndBarMsk_t vfBar0AndBarMsk;                     ///< 0xF00201BC : VF BAR0 and BAR Mask
    VfBar1AndBarMsk_t vfBar1AndBarMsk;                     ///< 0xF00201C0 : VF BAR1 and BAR Mask
    VfBar2AndBarMsk_t vfBar2AndBarMsk;                     ///< 0xF00201C4 : VF BAR2 and BAR Mask
    VfBar3AndBarMsk_t vfBar3AndBarMsk;                     ///< 0xF00201C8 : VF BAR3 and BAR Mask
    VfBar4AndBarMsk_t vfBar4AndBarMsk;                     ///< 0xF00201CC : VF BAR4 and BAR Mask
    VfBar5AndBarMsk_t vfBar5AndBarMsk;                     ///< 0xF00201D0 : VF BAR5 and BAR Mask
} PcieShadow_t;
COMPILE_ASSERT(offsetof(PcieShadow_t,vfBar5AndBarMsk)==0xF00201D0-0xF0020010,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile PcieShadow_t rPcieShadow;            ///< 0xF0020010-0xF0020200 : PcieShadow_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

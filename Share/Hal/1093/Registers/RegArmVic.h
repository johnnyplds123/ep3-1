#pragma once
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
//! @file
//! @brief Interrupt Controller Registers
//! 
//! Doc Version: NONE
//! Doc Date: NONE
//=============================================================================
/// @cond HW_REG


//-----------------------------------------------------------------------------
//  Interrupt Controller Register structure definition
//-----------------------------------------------------------------------------
/// @brief 0xF0120000 / 0xF0122000 / 0xF0124000 : IRQ Status Register
/// @brief 0xF0120004 / 0xF0122004 / 0xF0124004 : FiQ Status Register
/// @brief 0xF0120008 / 0xF0122008 / 0xF0124008 : Raw Interrupt Status Register
/// @brief 0xF012000C / 0xF012200C / 0xF012400C : Interrupt Select Register
/// @brief 0xF0120010 / 0xF0120010 / 0xF0124010 : Interrupt Enable Register
/// @brief 0xF0120014 / 0xF0122014 / 0xF0124014 : Interrupt Enable Clear Register
/// @brief 0xF0120018 / 0xF0122018 / 0xF0124018 : Software Interrupt Register
/// @brief 0xF012001C / 0xF012201C / 0xF012401C : Software Interrupt Clear Register


/// @brief 0xF0120020 / 0xF0122020 / 0xF0124020 : Protection Enable Register
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t PROTECTION          : 1;        ///<BIT [0]
        uint32_t RF0120020_31_1_RSVD :31;        ///<BIT [31:1]
    } b;
} VicProtection_t;


/// @brief 0xF0120024 / 0xF0122024 / 0xF0124024 : Software Priority Mask Register
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t SW_PRIORITY_MASK    :16;        ///<BIT[15:0]
        uint32_t RF0120024_31_16_RSVD:16;        ///<BIT[31:16]
    } b;
} VicSwPriorityMask_t;


/// @brief 0xF0120028 / 0xF0122028 / 0xF0124028 : Vector Priority Register for Daisy Chain
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t VECT_PRIORITY       : 4;        ///<BIT [3:0]
        uint32_t RF0120028_31_4_RSVD :28;        ///<BIT [31:4]
    } b;
} VicVectPriority_t;


/// @brief 0xF0120100-0xF0120217F / 0xF0122100-0xF012217F / 0xF0124100-0xF012417F : Vector Address Register
/// @brief 0xF0120200-0xF0120227F / 0xF0122200-0xF012227F / 0xF0124200-0xF012427F : Vector Priority Register
/// @brief 0xF0120F00 / 0xF0122F00 / 0xF0124F00 : Currently Active Vector Address Register


/// @brief 0xF0120FE0-0xF0120FEF / 0xF0122FE0-0xF0122FEF / 0xF0124FE0-0xF0124FEF : Peripheral Identification Register


/// @brief 0xF0120FF0-0xF0120FFF / 0xF0122FF0-0xF0122FFF / 0xF0124FF0-0xF0124FFF : Primecell Identification Register


/// @brief 0xF0120000-0xF0120FFF / 0xF0122000-0xF0122FFF / 0xF0124000-0xF0124FFF : tVINTC MODULE (ARM Vectored Interrupt Controller PL192)
typedef struct
{
    uint32_t irqStatus;                                          ///< 0xF0120000 / 0xF0122000 / 0xF0124000 : IRQ Status
    uint32_t fiqStatus;                                          ///< 0xF0120004 / 0xF0122004 / 0xF0124004 : FIQ Status
    uint32_t rawIntr;                                            ///< 0xF0120008 / 0xF0122008 / 0xF0124008 : Raw Interrupt Status
    uint32_t intSelect;                                          ///< 0xF012000C / 0xF012200C / 0xF012400C : Interrupt Select (0 for IRQ, 1 for FIQ)
    uint32_t intEnable;                                          ///< 0xF0120010 / 0xF0122010 / 0xF0124010 : Interrupt Enable
    uint32_t intEnableClear;                                     ///< 0xF0120014 / 0xF0122014 / 0xF0124014 : Interrupt Enable Clear
    uint32_t softInt;                                            ///< 0xF0120018 / 0xF0122018 / 0xF0124018 : Software Interrupt
    uint32_t softIntClear;                                       ///< 0xF012001C / 0xF012201C / 0xF012401C : Software Interrupt Clear
    VicProtection_t protection;                                  ///< 0xF0120020 / 0xF0122020 / 0xF0124020 : Protection Enable
    VicSwPriorityMask_t swPriorityMask;                          ///< 0xF0120024 / 0xF0122024 / 0xF0124024 : Software Priority Mask
    VicVectPriority_t vectPriorityDaisy;                         ///< 0xF0120028 / 0xF0122028 / 0xF0124028: Vector Priority for Daisy Chain
    uint8_t rsvd_RF012002C[212];                                 ///< 0xF012002C-0xF01200FF / 0xF012202C-0xF01220FF / 0xF012402C-0xF01240FF : Reserved
    uint32_t vectAddr[32];                                       ///< 0xF0120100-0xF012017F / 0xF0122100-0xF012217F / 0xF0124100-0xF012417F : Vector Address
    uint8_t rsvd_RF0120180[128];                                 ///< 0xF0120180-0xF01201FF / 0xF0122180-0xF01221FF / 0xF0124180-0xF01241FF : Reserved
    VicVectPriority_t vectPriority[32];                          ///< 0xF0120200-0xF012027F / 0xF0122200-0xF012227F / 0xF0124200-0xF012427F : Vector Priority
    uint8_t rsvd_RF0120280[3200];                                ///< 0xF0120280-0xF0120EFF / 0xF0122280-0xF0122EFF / 0xF0124280-0xF0124EFF : Reserved
    uint32_t activeVectAddr;                                     ///< 0xF0120F00 / 0xF0122F00 / 0xF0124F00 : Currently Active Vector Address
    uint8_t rsvd_RF0120F04[220];                                 ///< 0xF0120F04-0xF0120FDF / 0xF0122F04-0xF0122FdF / 0xF0124F04-0xF0124FdF : Reserved
} Vintc_t;

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile Vintc_t rVintc0;                            ///< 0xF0120000-0xF0120FFF
extern volatile Vintc_t rVintc1;                            ///< 0xF0122000-0xF0122FFF
extern volatile Vintc_t rVintc2;                            ///< 0xF0124000-0xF0124FFF


//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

/// @endcond


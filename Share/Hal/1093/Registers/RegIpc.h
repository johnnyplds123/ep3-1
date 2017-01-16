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
//! @brief IPC Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

#define HW_IPC_NUM_OF_DESCRIPTOR   32

//-----------------------------------------------------------------------------
//  IPC Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF0114080 : Descriptor 00 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_00_MODE         : 2;         ///<BIT [1:0] Descriptor 00 Mode.
        uint32_t RF0114080_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_00_CNT          : 4;         ///<BIT [15:12] Descriptor 00 Count.
        uint32_t RF0114080_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript00Ctrl_t;

///  @brief 0xF0114084 : Descriptor 01 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_01_MODE         : 2;         ///<BIT [1:0] Descriptor 01 Mode.
        uint32_t RF0114084_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_01_CNT          : 4;         ///<BIT [15:12] Descriptor 01 Count.
        uint32_t RF0114084_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript01Ctrl_t;

///  @brief 0xF0114088 : Descriptor 02 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_02_MODE         : 2;         ///<BIT [1:0] Descriptor 02 Mode.
        uint32_t RF0114088_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_02_CNT          : 4;         ///<BIT [15:12] Descriptor 02 Count.
        uint32_t RF0114088_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript02Ctrl_t;

///  @brief 0xF011408C : Descriptor 03 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_03_MODE         : 2;         ///<BIT [1:0] Descriptor 03 Mode.
        uint32_t RF011408C_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_03_CNT          : 4;         ///<BIT [15:12] Descriptor 03 Count.
        uint32_t RF011408C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript03Ctrl_t;

///  @brief 0xF0114090 : Descriptor 04 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_04_MODE         : 2;         ///<BIT [1:0] Descriptor 04 Mode.
        uint32_t RF0114090_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_04_CNT          : 4;         ///<BIT [15:12] Descriptor 04 Count.
        uint32_t RF0114090_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript04Ctrl_t;

///  @brief 0xF0114094 : Descriptor 05 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_05_MODE         : 2;         ///<BIT [1:0] Descriptor 05 Mode.
        uint32_t RF0114094_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_05_CNT          : 4;         ///<BIT [15:12] Descriptor 05 Count.
        uint32_t RF0114094_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript05Ctrl_t;

///  @brief 0xF0114098 : Descriptor 06 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_06_MODE         : 2;         ///<BIT [1:0] Descriptor 06 Mode.
        uint32_t RF0114098_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_06_CNT          : 4;         ///<BIT [15:12] Descriptor 06 Count.
        uint32_t RF0114098_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript06Ctrl_t;

///  @brief 0xF011409C : Descriptor 07 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_07_MODE         : 2;         ///<BIT [1:0] Descriptor 07 Mode.
        uint32_t RF011409C_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_07_CNT          : 4;         ///<BIT [15:12] Descriptor 07 Count.
        uint32_t RF011409C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript07Ctrl_t;

///  @brief 0xF01140A0 : Descriptor 08 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_08_MODE         : 2;         ///<BIT [1:0] Descriptor 08 Mode.
        uint32_t RF01140A0_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_08_CNT          : 4;         ///<BIT [15:12] Descriptor 08 Count.
        uint32_t RF01140A0_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript08Ctrl_t;

///  @brief 0xF01140A4 : Descriptor 09 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_09_MODE         : 2;         ///<BIT [1:0] Descriptor 09 Mode.
        uint32_t RF01140A4_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_09_CNT          : 4;         ///<BIT [15:12] Descriptor 09 Count.
        uint32_t RF01140A4_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript09Ctrl_t;

///  @brief 0xF01140A8 : Descriptor 10 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_10_MODE         : 2;         ///<BIT [1:0] Descriptor 10 Mode.
        uint32_t RF01140A8_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_10_CNT          : 4;         ///<BIT [15:12] Descriptor 10 Count.
        uint32_t RF01140A8_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript10Ctrl_t;

///  @brief 0xF01140AC : Descriptor 11 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_11_MODE         : 2;         ///<BIT [1:0] Descriptor 11 Mode.
        uint32_t RF01140AC_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_11_CNT          : 4;         ///<BIT [15:12] Descriptor 11 Count.
        uint32_t RF01140AC_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript11Ctrl_t;

///  @brief 0xF01140B0 : Descriptor 12 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_12_MODE         : 2;         ///<BIT [1:0] Descriptor 12 Mode.
        uint32_t RF01140B0_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_12_CNT          : 4;         ///<BIT [15:12] Descriptor 12 Count.
        uint32_t RF01140B0_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript12Ctrl_t;

///  @brief 0xF01140B4 : Descriptor 13 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_13_MODE         : 2;         ///<BIT [1:0] Descriptor 13 Mode.
        uint32_t RF01140B4_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_13_CNT          : 4;         ///<BIT [15:12] Descriptor 13 Count.
        uint32_t RF01140B4_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript13Ctrl_t;

///  @brief 0xF01140B8 : Descriptor 14 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_14_MODE         : 2;         ///<BIT [1:0] Descriptor 14 Mode.
        uint32_t RF01140B8_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_14_CNT          : 4;         ///<BIT [15:12] Descriptor 14 Count.
        uint32_t RF01140B8_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript14Ctrl_t;

///  @brief 0xF01140BC : Descriptor 15 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_15_MODE         : 2;         ///<BIT [1:0] Descriptor 15 Mode.
        uint32_t RF01140BC_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_15_CNT          : 4;         ///<BIT [15:12] Descriptor 15 Count.
        uint32_t RF01140BC_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript15Ctrl_t;

///  @brief 0xF01140C0 : Descriptor 16 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_16_MODE         : 2;         ///<BIT [1:0] Descriptor 16 Mode.
        uint32_t RF01140C0_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_16_CNT          : 4;         ///<BIT [15:12] Descriptor 16 Count.
        uint32_t RF01140C0_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript16Ctrl_t;

///  @brief 0xF01140C4 : Descriptor 17 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_17_MODE         : 2;         ///<BIT [1:0] Descriptor 17 Mode.
        uint32_t RF01140C4_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_17_CNT          : 4;         ///<BIT [15:12] Descriptor 17 Count.
        uint32_t RF01140C4_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript17Ctrl_t;

///  @brief 0xF01140C8 : Descriptor 18 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_18_MODE         : 2;         ///<BIT [1:0] Descriptor 18 Mode.
        uint32_t RF01140C8_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_18_CNT          : 4;         ///<BIT [15:12] Descriptor 18 Count.
        uint32_t RF01140C8_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript18Ctrl_t;

///  @brief 0xF01140CC : Descriptor 19 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_19_MODE         : 2;         ///<BIT [1:0] Descriptor 19 Mode.
        uint32_t RF01140CC_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_19_CNT          : 4;         ///<BIT [15:12] Descriptor 19 Count.
        uint32_t RF01140CC_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript19Ctrl_t;

///  @brief 0xF01140D0 : Descriptor 20 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_20_MODE         : 2;         ///<BIT [1:0] Descriptor 20 Mode.
        uint32_t RF01140D0_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_20_CNT          : 4;         ///<BIT [15:12] Descriptor 20 Count.
        uint32_t RF01140D0_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript20Ctrl_t;

///  @brief 0xF01140D4 : Descriptor 21 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_21_MODE         : 2;         ///<BIT [1:0] Descriptor 21 Mode.
        uint32_t RF01140D4_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_21_CNT          : 4;         ///<BIT [15:12] Descriptor 21 Count.
        uint32_t RF01140D4_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript21Ctrl_t;

///  @brief 0xF01140D8 : Descriptor 22 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_22_MODE         : 2;         ///<BIT [1:0] Descriptor 22 Mode.
        uint32_t RF01140D8_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_22_CNT          : 4;         ///<BIT [15:12] Descriptor 22 Count.
        uint32_t RF01140D8_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript22Ctrl_t;

///  @brief 0xF01140DC : Descriptor 23 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_23_MODE         : 2;         ///<BIT [1:0] Descriptor 23 Mode.
        uint32_t RF01140DC_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_23_CNT          : 4;         ///<BIT [15:12] Descriptor 23 Count.
        uint32_t RF01140DC_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript23Ctrl_t;

///  @brief 0xF01140E0 : Descriptor 24 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_24_MODE         : 2;         ///<BIT [1:0] Descriptor 24 Mode.
        uint32_t RF01140E0_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_24_CNT          : 4;         ///<BIT [15:12] Descriptor 24 Count.
        uint32_t RF01140E0_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript24Ctrl_t;

///  @brief 0xF01140E4 : Descriptor 25 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_25_MODE         : 2;         ///<BIT [1:0] Descriptor 25 Mode.
        uint32_t RF01140E4_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_25_CNT          : 4;         ///<BIT [15:12] Descriptor 25 Count.
        uint32_t RF01140E4_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript25Ctrl_t;

///  @brief 0xF01140E8 : Descriptor 26 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_26_MODE         : 2;         ///<BIT [1:0] Descriptor 26 Mode.
        uint32_t RF01140E8_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_26_CNT          : 4;         ///<BIT [15:12] Descriptor 26 Count.
        uint32_t RF01140E8_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript26Ctrl_t;

///  @brief 0xF01140EC : Descriptor 27 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_27_MODE         : 2;         ///<BIT [1:0] Descriptor 27 Mode.
        uint32_t RF01140EC_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_27_CNT          : 4;         ///<BIT [15:12] Descriptor 27 Count.
        uint32_t RF01140EC_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript27Ctrl_t;

///  @brief 0xF01140F0 : Descriptor 28 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_28_MODE         : 2;         ///<BIT [1:0] Descriptor 28 Mode.
        uint32_t RF01140F0_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_28_CNT          : 4;         ///<BIT [15:12] Descriptor 28 Count.
        uint32_t RF01140F0_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript28Ctrl_t;

///  @brief 0xF01140F4 : Descriptor 29 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_29_MODE         : 2;         ///<BIT [1:0] Descriptor 29 Mode.
        uint32_t RF01140F4_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_29_CNT          : 4;         ///<BIT [15:12] Descriptor 29 Count.
        uint32_t RF01140F4_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript29Ctrl_t;

///  @brief 0xF01140F8 : Descriptor 30 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_30_MODE         : 2;         ///<BIT [1:0] Descriptor 30 Mode.
        uint32_t RF01140F8_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_30_CNT          : 4;         ///<BIT [15:12] Descriptor 30 Count.
        uint32_t RF01140F8_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript30Ctrl_t;

///  @brief 0xF01140FC : Descriptor 31 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DESC_31_MODE         : 2;         ///<BIT [1:0] Descriptor 31 Mode.
        uint32_t RF01140FC_11_2_RSVD  :10;         ///<BIT [11:2] Reserved.
        uint32_t DESC_31_CNT          : 4;         ///<BIT [15:12] Descriptor 31 Count.
        uint32_t RF01140FC_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} Descript31Ctrl_t;


/// @brief 0xF0114000-0xF0114180 : Ipc_t MODULE
typedef struct
{

    //    ARRAY DECLARATION
    uint32_t INT_PEND_CLR[3];
    //    uint32_t INT_PEND_CLR_0;             ///< 0xF0114000 : CPU 0 Interrupt Pending Clear
    //    uint32_t INT_PEND_CLR_1;             ///< 0xF0114004 : CPU 1 Interrupt Pending Clear
    //    uint32_t INT_PEND_CLR_2;             ///< 0xF0114008 : CPU 2 Interrupt Pending Clear
    uint8_t rsvd_RF011400C[20];          ///< 0xF011400C - 0xF011401F : Reserved.

    //    ARRAY DECLARATION
    uint32_t INT_MASK_CLR[3];
    //    uint32_t INT_MASK_CLR_0;             ///< 0xF0114020 : CPU 0 Interrupt Mask Clear
    //    uint32_t INT_MASK_CLR_1;             ///< 0xF0114024 : CPU 1 Interrupt Mask Clear
    //    uint32_t INT_MASK_CLR_2;             ///< 0xF0114028 : CPU 2 Interrupt Mask Clear
    uint8_t rsvd_RF011402C[4];           ///< 0xF011402C - 0xF011402F : Reserved.

    //    ARRAY DECLARATION
    uint32_t INT_MASK_SET[3];
    //    uint32_t INT_MASK_SET_0;             ///< 0xF0114030 : CPU 0 Interrupt Mask Set
    //    uint32_t INT_MASK_SET_1;             ///< 0xF0114034 : CPU 1 Interrupt Mask Set
    //    uint32_t INT_MASK_SET_2;             ///< 0xF0114038 : CPU 2 Interrupt Mask Set
    uint8_t rsvd_RF011403C[4];           ///< 0xF011403C - 0xF011403F : Reserved.

    //    ARRAY DECLARATION
    uint32_t INT_ENAB_CLR[3];
    //    uint32_t INT_ENAB_CLR_0;             ///< 0xF0114040 : CPU 0 Interrupt Enable Clear
    //    uint32_t INT_ENAB_CLR_1;             ///< 0xF0114044 : CPU 1 Interrupt Enable Clear
    //    uint32_t INT_ENAB_CLR_2;             ///< 0xF0114048 : CPU 2 Interrupt Enable Clear
    uint8_t rsvd_RF011404C[4];           ///< 0xF011404C - 0xF011404F : Reserved.

    //    ARRAY DECLARATION
    uint32_t INT_ENAB_SET[3];
    //    uint32_t INT_ENAB_SET_0;             ///< 0xF0114050 : CPU 0 Interrupt Enable Set
    //    uint32_t INT_ENAB_SET_1;             ///< 0xF0114054 : CPU 1 Interrupt Enable Set
    //    uint32_t INT_ENAB_SET_2;             ///< 0xF0114058 : CPU 2 Interrupt Enable Set
    uint8_t rsvd_RF011405C[36];          ///< 0xF011405C - 0xF011407F : Reserved.

    //    ARRAY DECLARATION
    Descript00Ctrl_t descriptCtrl[32];
    //    Descript00Ctrl_t descript00Ctrl;     ///< 0xF0114080 : Descriptor 00 Control
    //    Descript01Ctrl_t descript01Ctrl;     ///< 0xF0114084 : Descriptor 01 Control
    //    Descript02Ctrl_t descript02Ctrl;     ///< 0xF0114088 : Descriptor 02 Control
    //    Descript03Ctrl_t descript03Ctrl;     ///< 0xF011408C : Descriptor 03 Control
    //    Descript04Ctrl_t descript04Ctrl;     ///< 0xF0114090 : Descriptor 04 Control
    //    Descript05Ctrl_t descript05Ctrl;     ///< 0xF0114094 : Descriptor 05 Control
    //    Descript06Ctrl_t descript06Ctrl;     ///< 0xF0114098 : Descriptor 06 Control
    //    Descript07Ctrl_t descript07Ctrl;     ///< 0xF011409C : Descriptor 07 Control
    //    Descript08Ctrl_t descript08Ctrl;     ///< 0xF01140A0 : Descriptor 08 Control
    //    Descript09Ctrl_t descript09Ctrl;     ///< 0xF01140A4 : Descriptor 09 Control
    //    Descript10Ctrl_t descript10Ctrl;     ///< 0xF01140A8 : Descriptor 10 Control
    //    Descript11Ctrl_t descript11Ctrl;     ///< 0xF01140AC : Descriptor 11 Control
    //    Descript12Ctrl_t descript12Ctrl;     ///< 0xF01140B0 : Descriptor 12 Control
    //    Descript13Ctrl_t descript13Ctrl;     ///< 0xF01140B4 : Descriptor 13 Control
    //    Descript14Ctrl_t descript14Ctrl;     ///< 0xF01140B8 : Descriptor 14 Control
    //    Descript15Ctrl_t descript15Ctrl;     ///< 0xF01140BC : Descriptor 15 Control
    //    Descript16Ctrl_t descript16Ctrl;     ///< 0xF01140C0 : Descriptor 16 Control
    //    Descript17Ctrl_t descript17Ctrl;     ///< 0xF01140C4 : Descriptor 17 Control
    //    Descript18Ctrl_t descript18Ctrl;     ///< 0xF01140C8 : Descriptor 18 Control
    //    Descript19Ctrl_t descript19Ctrl;     ///< 0xF01140CC : Descriptor 19 Control
    //    Descript20Ctrl_t descript20Ctrl;     ///< 0xF01140D0 : Descriptor 20 Control
    //    Descript21Ctrl_t descript21Ctrl;     ///< 0xF01140D4 : Descriptor 21 Control
    //    Descript22Ctrl_t descript22Ctrl;     ///< 0xF01140D8 : Descriptor 22 Control
    //    Descript23Ctrl_t descript23Ctrl;     ///< 0xF01140DC : Descriptor 23 Control
    //    Descript24Ctrl_t descript24Ctrl;     ///< 0xF01140E0 : Descriptor 24 Control
    //    Descript25Ctrl_t descript25Ctrl;     ///< 0xF01140E4 : Descriptor 25 Control
    //    Descript26Ctrl_t descript26Ctrl;     ///< 0xF01140E8 : Descriptor 26 Control
    //    Descript27Ctrl_t descript27Ctrl;     ///< 0xF01140EC : Descriptor 27 Control
    //    Descript28Ctrl_t descript28Ctrl;     ///< 0xF01140F0 : Descriptor 28 Control
    //    Descript29Ctrl_t descript29Ctrl;     ///< 0xF01140F4 : Descriptor 29 Control
    //    Descript30Ctrl_t descript30Ctrl;     ///< 0xF01140F8 : Descriptor 30 Control
    //    Descript31Ctrl_t descript31Ctrl;     ///< 0xF01140FC : Descriptor 31 Control

    //    ARRAY DECLARATION
    uint32_t DESC_REG[32];
    //    uint32_t DESC_REG_00;                ///< 0xF0114100 : Descriptor 00
    //    uint32_t DESC_REG_01;                ///< 0xF0114104 : Descriptor 01
    //    uint32_t DESC_REG_02;                ///< 0xF0114108 : Descriptor 02
    //    uint32_t DESC_REG_03;                ///< 0xF011410C : Descriptor 03
    //    uint32_t DESC_REG_04;                ///< 0xF0114110 : Descriptor 04
    //    uint32_t DESC_REG_05;                ///< 0xF0114114 : Descriptor 05
    //    uint32_t DESC_REG_06;                ///< 0xF0114118 : Descriptor 06
    //    uint32_t DESC_REG_07;                ///< 0xF011411C : Descriptor 07
    //    uint32_t DESC_REG_08;                ///< 0xF0114120 : Descriptor 08
    //    uint32_t DESC_REG_09;                ///< 0xF0114124 : Descriptor 09
    //    uint32_t DESC_REG_10;                ///< 0xF0114128 : Descriptor 10
    //    uint32_t DESC_REG_11;                ///< 0xF011412C : Descriptor 11
    //    uint32_t DESC_REG_12;                ///< 0xF0114130 : Descriptor 12
    //    uint32_t DESC_REG_13;                ///< 0xF0114134 : Descriptor 13
    //    uint32_t DESC_REG_14;                ///< 0xF0114138 : Descriptor 14
    //    uint32_t DESC_REG_15;                ///< 0xF011413C : Descriptor 15
    //    uint32_t DESC_REG_16;                ///< 0xF0114140 : Descriptor 16
    //    uint32_t DESC_REG_17;                ///< 0xF0114144 : Descriptor 17
    //    uint32_t DESC_REG_18;                ///< 0xF0114148 : Descriptor 18
    //    uint32_t DESC_REG_19;                ///< 0xF011414C : Descriptor 19
    //    uint32_t DESC_REG_20;                ///< 0xF0114150 : Descriptor 20
    //    uint32_t DESC_REG_21;                ///< 0xF0114154 : Descriptor 21
    //    uint32_t DESC_REG_22;                ///< 0xF0114158 : Descriptor 22
    //    uint32_t DESC_REG_23;                ///< 0xF011415C : Descriptor 23
    //    uint32_t DESC_REG_24;                ///< 0xF0114160 : Descriptor 24
    //    uint32_t DESC_REG_25;                ///< 0xF0114164 : Descriptor 25
    //    uint32_t DESC_REG_26;                ///< 0xF0114168 : Descriptor 26
    //    uint32_t DESC_REG_27;                ///< 0xF011416C : Descriptor 27
    //    uint32_t DESC_REG_28;                ///< 0xF0114170 : Descriptor 28
    //    uint32_t DESC_REG_29;                ///< 0xF0114174 : Descriptor 29
    //    uint32_t DESC_REG_30;                ///< 0xF0114178 : Descriptor 30
    //    uint32_t DESC_REG_31;                ///< 0xF011417C : Descriptor 31
} Ipc_t;
COMPILE_ASSERT(offsetof(Ipc_t,DESC_REG[31])==0xF011417C-0xF0114000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile Ipc_t rIpc;                          ///< 0xF0114000-0xF0114180 : Ipc_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

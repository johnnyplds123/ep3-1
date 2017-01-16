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
//! @brief SPI Bridge Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  SPI Bridge Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xC2000000 : Device Descriptor 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DV_SPI_CLK_PERIOD_0 : 2;         ///<BIT [1:0] Device SPI Clock Period 0.
        uint32_t DV_CHIP_SEL_SETUP_0 : 3;         ///<BIT [4:2] Device Chip Select To First Clock 0.
        uint32_t DV_CHIP_SEL_HOLD_0  : 3;         ///<BIT [7:5] Device Chip Select Hold 0.
        uint32_t DV_CHIP_SEL_WAIT_0  : 6;         ///<BIT [13:8] Device Chip Select Wait 0.
        uint32_t W_FIFO_INT_EN_0     : 1;         ///<BIT [14] Write FIFO Interrupt Enable 0.
        uint32_t DV_SPI_CLK_DIV2     : 1;         ///<BIT [15] Device SPI Clock Divide by 2.
        uint32_t DT_RD_FLSH_CMD_0    : 8;         ///<BIT [23:16] Data Transfer Read Instruction 0.
        uint32_t DT_W_FLSH_CMD_0     : 8;         ///<BIT [31:24] Data Transfer Write Instruction 0.
    } b;    
} DeviceDescript0_t;

///  @brief 0xC2000004 : Device Descriptor 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DV_SPI_CLK_PERIOD_1 : 2;         ///<BIT [1:0] Device SPI Clock Period 1.
        uint32_t DV_CHIP_SEL_SETUP_1 : 3;         ///<BIT [4:2] Device Chip Select to First Clock.
        uint32_t DV_CHIP_SEL_HOLD_1  : 3;         ///<BIT [7:5] Device Chip Select Hold 1.
        uint32_t DV_CHIP_SEL_WAIT_1  : 6;         ///<BIT [13:8] Device Chip Select Wait 1.
        uint32_t W_FIFO_INT_EN_1     : 1;         ///<BIT [14] Write FIFO Interrupt Enable 1.
        uint32_t RC2000004_15_RSVD   : 1;         ///<BIT [15] Reserved.
        uint32_t DT_RD_FLSH_CMD_1    : 8;         ///<BIT [23:16] Data Transfer Read Instruction 1.
        uint32_t DT_W_FLSH_CMD_1     : 8;         ///<BIT [31:24] Data Transfer Write Instruction 1.
    } b;    
} DeviceDescript1_t;

///  @brief 0xC2000008 : Device Descriptor 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DV_SPI_CLK_PERIOD_2 : 2;         ///<BIT [1:0] Device SPI Clock Period 2.
        uint32_t DV_CHIP_SEL_SETUP_2 : 3;         ///<BIT [4:2] Device Chip Select To First Clock 2.
        uint32_t DV_CHIP_SEL_HOLD_2  : 3;         ///<BIT [7:5] Device Chip Select Hold 2.
        uint32_t DV_CHIP_SEL_WAIT_2  : 6;         ///<BIT [13:8] Device Chip Select Wait 2.
        uint32_t W_FIFO_INT_EN_2     : 1;         ///<BIT [14] Write FIFO Interrupt Enable 2.
        uint32_t RC2000008_15_RSVD   : 1;         ///<BIT [15] Reserved.
        uint32_t DT_RD_FLSH_CMD_2    : 8;         ///<BIT [23:16] Data Transfer Read Instruction 2.
        uint32_t DT_W_FLSH_CMD_2     : 8;         ///<BIT [31:24] Data Transfer Write Instruction 2.
    } b;    
} DeviceDescript2_t;

///  @brief 0xC200000C : Device Descriptor 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DV_SPI_CLK_PERIOD_3 : 2;         ///<BIT [1:0] Device SPI Clock Period 3.
        uint32_t DV_CHIP_SEL_SETUP_3 : 3;         ///<BIT [4:2] Device Chip Select To First Clock 3.
        uint32_t DV_CHIP_SEL_HOLD_3  : 3;         ///<BIT [7:5] Device Chip Select Hold 3.
        uint32_t DV_CHIP_SEL_WAIT_3  : 6;         ///<BIT [13:8] Device Chip Select Wait 3.
        uint32_t W_FIFO_INT_EN_3     : 1;         ///<BIT [14] Write FIFO Interrupt Enable 3.
        uint32_t RC200000C_15_RSVD   : 1;         ///<BIT [15] Reserved.
        uint32_t DT_RD_FLSH_CMD_3    : 8;         ///<BIT [23:16] Data Transfer Read Instruction 3.
        uint32_t DT_W_FLSH_CMD_3     : 8;         ///<BIT [31:24] Data Transfer Write Instruction 3.
    } b;    
} DeviceDescript3_t;

///  @brief 0xC2000010 : Device Range 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DV_RNG_0             :10;         ///<BIT [9:0] Device Range 0.
        uint32_t RC2000010_14_10_RSVD : 5;         ///<BIT [14:10] Reserved.
        uint32_t EN_0                 : 1;         ///<BIT [15] Enable 0.
        uint32_t RC2000010_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DeviceRng0_t;

///  @brief 0xC2000014 : Device Range 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DV_RNG_1             :10;         ///<BIT [9:0] Device Range 1.
        uint32_t RC2000014_14_10_RSVD : 5;         ///<BIT [14:10] Reserved.
        uint32_t EN_1                 : 1;         ///<BIT [15] Enable 1.
        uint32_t RC2000014_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DeviceRng1_t;

///  @brief 0xC2000018 : Device Range 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DV_RNG_2             :10;         ///<BIT [9:0] Device Range 2.
        uint32_t RC2000018_14_10_RSVD : 5;         ///<BIT [14:10] Reserved.
        uint32_t EN_2                 : 1;         ///<BIT [15] Enable 2.
        uint32_t RC2000018_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DeviceRng2_t;

///  @brief 0xC200001C : Device Range 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DV_RNG_3             :10;         ///<BIT [9:0] Device Range 3.
        uint32_t RC200001C_14_10_RSVD : 5;         ///<BIT [14:10] Reserved.
        uint32_t EN_3                 : 1;         ///<BIT [15] Enable 3.
        uint32_t RC200001C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DeviceRng3_t;

///  @brief 0xC2000020 : SPI Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DV_ALLCT            : 2;         ///<BIT [1:0] Device Allocation.
        uint32_t RC2000020_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} SpiCfg_t;

///  @brief 0xC2000024 : SPI Reset
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SPI_RESETN          : 4;         ///<BIT [3:0] SPI Device Reset.
        uint32_t RC2000024_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} SpiRst_t;

///  @brief 0xC200002C : Device Command Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EXT_DV_SEL           : 2;         ///<BIT [1:0] External Device Select.
        uint32_t DV_CMD_LEN           : 3;         ///<BIT [4:2] Device Command Length.
        uint32_t DV_RD_W              : 1;         ///<BIT [5] Device Read or Write.
        uint32_t DV_RD_LEN            : 2;         ///<BIT [7:6] Device Read Length.
        uint32_t DV_CMD               : 8;         ///<BIT [15:8] Device Command.
        uint32_t RC200002C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DeviceCmdCtrl_t;

///  @brief 0xC2000034 : Device Write FIFO Hold
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DV_W_FIFO_HLD       : 1;         ///<BIT [0] Device Write FIFO Hold.
        uint32_t RC2000034_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} DeviceWrFifoHold_t;

///  @brief 0xC2000038 : Auto Address Increment Command Timer Value 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BYT_PRGM_VAL_0       :16;         ///<BIT [15:0] Byte Program Timer Value 0.
        uint32_t RC2000038_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} AutoAddrIncremenCmdTmrVal0_t;

///  @brief 0xC2000044 : Auto Address Increment Command Timer Value 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BYT_PRGM_VAL_3       :16;         ///<BIT [15:0] Byte Program Timer Value 3.
        uint32_t RC2000044_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} AutoAddrIncremenCmdTmrVal3_t;

///  @brief 0xC2000048 : SPI Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SPI_W_FIFO_INT      : 1;         ///<BIT [0] SPI Write FIFO Interrupt.
        uint32_t RC2000048_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} SpiIntr_t;


/// @brief 0xC2000000-0xC2040000 : SpiBridge_t MODULE
typedef struct
{
    DeviceDescript0_t deviceDescript0;                           ///< 0xC2000000 : Device Descriptor 0
    DeviceDescript1_t deviceDescript1;                           ///< 0xC2000004 : Device Descriptor 1
    DeviceDescript2_t deviceDescript2;                           ///< 0xC2000008 : Device Descriptor 2
    DeviceDescript3_t deviceDescript3;                           ///< 0xC200000C : Device Descriptor 3
    DeviceRng0_t deviceRng0;                                     ///< 0xC2000010 : Device Range 0
    DeviceRng1_t deviceRng1;                                     ///< 0xC2000014 : Device Range 1
    DeviceRng2_t deviceRng2;                                     ///< 0xC2000018 : Device Range 2
    DeviceRng3_t deviceRng3;                                     ///< 0xC200001C : Device Range 3
    SpiCfg_t spiCfg;                                             ///< 0xC2000020 : SPI Configuration
    SpiRst_t spiRst;                                             ///< 0xC2000024 : SPI Reset
    uint32_t CMD_DPNDNT;                                         ///< 0xC2000028 : Device Command Address Data
    DeviceCmdCtrl_t deviceCmdCtrl;                               ///< 0xC200002C : Device Command Control
    uint32_t DV_CMD_RT_DATA;                                     ///< 0xC2000030 : Device Command Read Data
    DeviceWrFifoHold_t deviceWrFifoHold;                         ///< 0xC2000034 : Device Write FIFO Hold
    AutoAddrIncremenCmdTmrVal0_t autoAddrIncremenCmdTmrVal0;     ///< 0xC2000038 : Auto Address Increment Command Timer Value 0
    uint8_t rsvd_RC200003C[8];                                   ///< 0xC200003C - 0xC2000043 : Reserved.
    AutoAddrIncremenCmdTmrVal3_t autoAddrIncremenCmdTmrVal3;     ///< 0xC2000044 : Auto Address Increment Command Timer Value 3
    SpiIntr_t spiIntr;                                           ///< 0xC2000048 : SPI Interrupt
    uint32_t RC200004C_31_0_RSVD;                                ///< 0xC200004C : Reserved
    uint8_t rsvd_RC2000050[262060];                              ///< 0xC2000050 - 0xC203FFFB : Reserved.
    uint32_t RC203FFFC_31_0_RSVD;                                ///< 0xC203FFFC : Reserved
} SpiBridge_t;
COMPILE_ASSERT(offsetof(SpiBridge_t,RC203FFFC_31_0_RSVD)==0xC203FFFC-0xC2000000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile SpiBridge_t rSpiBridge;              ///< 0xC2000000-0xC2040000 : SpiBridge_t MODULE


//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

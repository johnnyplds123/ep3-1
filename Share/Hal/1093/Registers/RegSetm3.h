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
//! @brief SETM3 Registers
//!
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  SETM3 Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xD001B000 : Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PORT_SEL    : 1;         ///<BIT [0] Port Select.
        uint32_t DM          : 1;         ///<BIT [1] Dual Channel Mode.
        uint32_t TD          : 1;         ///<BIT [2] Send Trace Disable.
        uint32_t TX_CLKA_INV : 1;         ///<BIT [3] TX Clock Invert Port A.
        uint32_t TX_CLKB_INV : 1;         ///<BIT [4] TX Clock Invert for Port B.
        uint32_t ETM_0       : 1;         ///<BIT [5] ETM 0.
        uint32_t ETM_1       : 1;         ///<BIT [6] ETM 1.
        uint32_t DIG_RSTN    : 1;         ///<BIT [7] Digital Reset Negative.
        uint32_t PAS_EN      : 1;         ///<BIT [8] Port A Size Enable.
        uint32_t PORTA_SIZE  : 4;         ///<BIT [12:9] Port A Size.
        uint32_t PBS_EN      : 1;         ///<BIT [13] Port B Size Enable.
        uint32_t PORTB_SIZE  : 4;         ///<BIT [17:14] Port B Size.
        uint32_t SBZ         :14;         ///<BIT [31:18] Should Be Zero.
    } b;
} Cfg_t;

///  @brief 0xD001B004 : Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SETM_FIFO_OF   : 1;         ///<BIT [0] SETM FIFO Overflow.
        uint32_t TEST_CH_READY  : 1;         ///<BIT [1] Test Channel Ready.
        uint32_t TRACE_CH_READY : 1;         ///<BIT [2] Trace Channel Ready.
        uint32_t SBZ            :29;         ///<BIT [31:3] Should Be Zero.
    } b;
} Stat_t;

///  @brief 0xD001B00C : JTAG Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CNTRL : 1;         ///<BIT [0] JTAG Control.
        uint32_t RESET : 1;         ///<BIT [1] JTAG in Reset.
        uint32_t SBZ   :30;         ///<BIT [31:2] Should Be Zero.
    } b;
} JtagStat_t;

///  @brief 0xD001B140 : PHY Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TX_RATE              : 3;         ///<BIT [2:0] Transmitter Data Rate Select.
        uint32_t RD001B140_3_RSVD     : 1;         ///<BIT [3] Reserved.
        uint32_t REF_FREF_SEL         : 5;         ///<BIT [8:4] Reference Frequency Select.
        uint32_t REF_FREF_SEL_EN      : 1;         ///<BIT [9] Reference Frequency Select Enable.
        uint32_t RD001B140_12_10_RSVD : 3;         ///<BIT [12:10] Reserved.
        uint32_t PU_TX_B              : 1;         ///<BIT [13] Power Up Transmitter B.
        uint32_t PU_TX_A              : 1;         ///<BIT [14] Power Up Transmitter A.
        uint32_t PU_PLL               : 1;         ///<BIT [15] Power Up PLL.
        uint32_t RD001B140_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;
} PhyCtrl0_t;

///  @brief 0xD001B160 : PHY Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t STEM_TRACE_STALL     : 1;         ///<BIT [0] SETM Trace Stall Status.
        uint32_t SETM_TEST_STALL      : 1;         ///<BIT [1] SETM Test Stall Status.
        uint32_t RD001B160_2_RSVD     : 1;         ///<BIT [2] Reserved.
        uint32_t RD001B160_23_3_RSVD  :21;         ///<BIT [23:3] Reserved.
        uint32_t RD001B160_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;
} PhyStat_t;

///  @brief 0xD001BF00 : Integration Mode Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INTEGRATION_MODE    : 1;         ///<BIT [0] Integration Mode.
        uint32_t RD001BF00_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;
} IntegratModeCtrl_t;

///  @brief 0xD001BFA0 : Claim Tag Set
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLAIMSET            : 4;         ///<BIT [3:0] Claim Set.
        uint32_t RD001BFA0_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;
} ClaimTagSet_t;

///  @brief 0xD001BFA4 : Claim Tag Clear
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLAIMCLR            : 4;         ///<BIT [3:0] Claim Clear.
        uint32_t RD001BFA4_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;
} ClaimTagClr_t;

///  @brief 0xD001BFB4 : Lock Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LOCK_STATUS_PRESENT : 1;         ///<BIT [0] Lock Control Present.
        uint32_t ACCESS_PERMISSION   : 1;         ///<BIT [1] Access Permission.
        uint32_t LOCK_STATUS_8BIT    : 1;         ///<BIT [2] Lock Status Register 8-bit Mode.
        uint32_t RD001BFB4_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;
} LockStat_t;

///  @brief 0xD001BFB8 : Authentication Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t NSEC_INVASIVE       : 2;         ///<BIT [1:0] Non-Secure Invasive Debug.
        uint32_t NSEC_N_INVASIVE     : 2;         ///<BIT [3:2] Non-Secure, Non-Invasive Debug.
        uint32_t SEC_INVASIVE        : 2;         ///<BIT [5:4] Secure Invasive Debug.
        uint32_t SEC_N_INVASIVE      : 2;         ///<BIT [7:6] Secure Non-Invasive Debug.
        uint32_t RD001BFB8_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} AuthentiStat_t;

///  @brief 0xD001BFC8 : Device Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RD001BFC8_7_0_RSVD  : 8;         ///<BIT [7:0] Reserved.
        uint32_t RD001BFC8_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} DeviceCfg_t;

///  @brief 0xD001BFCC : Device Type Identifier
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MAJOR_TYPE          : 4;         ///<BIT [3:0] Major Type.
        uint32_t SUB_TYPE            : 4;         ///<BIT [7:4] Sub Type.
        uint32_t RD001BFCC_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} DeviceTypeIdentifi_t;

///  @brief 0xD001BFD0 : Peripheral ID 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t JEP106              : 4;         ///<BIT [3:0] JEP106 Continuation Code.
        uint32_t A4KB_COUNT          : 4;         ///<BIT [7:4] 4KB Count.
        uint32_t RD001BFD0_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} PeripherId4_t;

///  @brief 0xD001BFE0 : Peripheral ID 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PART_NUM_LOW        : 8;         ///<BIT [7:0] Part Number [7:0].
        uint32_t RD001BFE0_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} PeripherId0_t;

///  @brief 0xD001BFE4 : Peripheral ID 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PART_NUM_HIGH       : 4;         ///<BIT [3:0] Part Number [11:8].
        uint32_t JEP106_3_0          : 4;         ///<BIT [7:4] JEP106 Identity Code [3:0].
        uint32_t RD001BFE4_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} PeripherId1_t;

///  @brief 0xD001BFE8 : Peripheral ID 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t JEP106_6_4          : 3;         ///<BIT [2:0] JEP106 Identity Code [6:4].
        uint32_t JEDEC               : 1;         ///<BIT [3] JEDEC.
        uint32_t REVISION            : 4;         ///<BIT [7:4] Revision.
        uint32_t RD001BFE8_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} PeripherId2_t;

///  @brief 0xD001BFEC : Peripheral ID 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CUSTOMER_MOD        : 4;         ///<BIT [3:0] Customer Modified.
        uint32_t REVAND              : 4;         ///<BIT [7:4] RevAnd.
        uint32_t RD001BFEC_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} PeripherId3_t;

///  @brief 0xD001BFF0 : Component ID 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PREAMBLE_0          : 8;         ///<BIT [7:0] Preamble 0.
        uint32_t RD001BFF0_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} ComponenId0_t;

///  @brief 0xD001BFF4 : Component ID 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PREAMBLE_1          : 4;         ///<BIT [3:0] Preamble 1.
        uint32_t COMPONENT_CLASS     : 4;         ///<BIT [7:4] Component Class.
        uint32_t RD001BFF4_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} ComponenId1_t;

///  @brief 0xD001BFF8 : Component ID 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PREAMBLE_2          : 8;         ///<BIT [7:0] Preamble 2.
        uint32_t RD001BFF8_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} ComponenId2_t;

///  @brief 0xD001BFFC : Component ID 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PREAMBLE_3          : 8;         ///<BIT [7:0] Preamble 3.
        uint32_t RD001BFFC_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} ComponenId3_t;


/// @brief 0xD001B000-0xD001C000 : Setm3_t MODULE
typedef struct
{
    Cfg_t cfg;                                   ///< 0xD001B000 : Configuration
    Stat_t stat;                                 ///< 0xD001B004 : Status
    uint8_t rsvd_RD001B008[4];                   ///< 0xD001B008 - 0xD001B00B : Reserved.
    JtagStat_t jtagStat;                         ///< 0xD001B00C : JTAG Status
    uint8_t rsvd_RD001B010[304];                 ///< 0xD001B010 - 0xD001B13F : Reserved.
    PhyCtrl0_t phyCtrl0;                         ///< 0xD001B140 : PHY Control 0
    uint8_t rsvd_RD001B144[28];                  ///< 0xD001B144 - 0xD001B15F : Reserved.
    PhyStat_t phyStat;                           ///< 0xD001B160 : PHY Status
    uint8_t rsvd_RD001B164[3484];                ///< 0xD001B164 - 0xD001BEFF : Reserved.
    IntegratModeCtrl_t integratModeCtrl;         ///< 0xD001BF00 : Integration Mode Control
    uint8_t rsvd_RD001BF04[156];                 ///< 0xD001BF04 - 0xD001BF9F : Reserved.
    ClaimTagSet_t claimTagSet;                   ///< 0xD001BFA0 : Claim Tag Set
    ClaimTagClr_t claimTagClr;                   ///< 0xD001BFA4 : Claim Tag Clear
    uint8_t rsvd_RD001BFA8[8];                   ///< 0xD001BFA8 - 0xD001BFAF : Reserved.
    uint32_t LOCK_ACCESS;                        ///< 0xD001BFB0 : Lock Access
    LockStat_t lockStat;                         ///< 0xD001BFB4 : Lock Status
    AuthentiStat_t authentiStat;                 ///< 0xD001BFB8 : Authentication Status
    uint8_t rsvd_RD001BFBC[4];                   ///< 0xD001BFBC - 0xD001BFBF : Reserved.
    uint32_t RD001BFC0_31_0_RSVD;                ///< 0xD001BFC0 : Reserved
    uint32_t RD001BFC4_31_0_RSVD;                ///< 0xD001BFC4 : Reserved
    DeviceCfg_t deviceCfg;                       ///< 0xD001BFC8 : Device Configuration
    DeviceTypeIdentifi_t deviceTypeIdentifi;     ///< 0xD001BFCC : Device Type Identifier
    PeripherId4_t peripherId4;                   ///< 0xD001BFD0 : Peripheral ID 4
    uint32_t RD001BFD4_31_0_RSVD;                ///< 0xD001BFD4 : Peripheral ID 5
    uint32_t RD001BFD8_31_0_RSVD;                ///< 0xD001BFD8 : Peripheral ID 6
    uint32_t RD001BFDC_31_0_RSVD;                ///< 0xD001BFDC : Peripheral ID 7
    PeripherId0_t peripherId0;                   ///< 0xD001BFE0 : Peripheral ID 0
    PeripherId1_t peripherId1;                   ///< 0xD001BFE4 : Peripheral ID 1
    PeripherId2_t peripherId2;                   ///< 0xD001BFE8 : Peripheral ID 2
    PeripherId3_t peripherId3;                   ///< 0xD001BFEC : Peripheral ID 3
    ComponenId0_t componenId0;                   ///< 0xD001BFF0 : Component ID 0
    ComponenId1_t componenId1;                   ///< 0xD001BFF4 : Component ID 1
    ComponenId2_t componenId2;                   ///< 0xD001BFF8 : Component ID 2
    ComponenId3_t componenId3;                   ///< 0xD001BFFC : Component ID 3
} Setm3_t;
COMPILE_ASSERT(offsetof(Setm3_t,componenId3)==0xD001BFFC-0xD001B000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile Setm3_t rSetm3;                      ///< 0xD001B000-0xD001C000 : Setm3_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

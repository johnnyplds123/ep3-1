#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2015  -  All rights reserved
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
//! @file   HalMc5DdrConfig.h
//!
//! @brief  DDR Configurations and general definitions/macros
//
//=============================================================================

/*  NOTE: DDR initialization requirements
    1. Full automatic detection of CS number, memory size, data width,
       device density
    2. Ability to remove unnecessary code from compilation by disable
       autodetection and define exact configuration
    3. All relevant tables (JEDEC etc.) should be included in the code
       and comments
    4. The only manual parameter that have to be defined - DDR device speed bin
*/

//-----------------------------------------------------------------------------
//  constant definitions
//-----------------------------------------------------------------------------
#define MC5_ENABLED             ENABLE
#define MC5_DISABLED            DISABLE

//-----------------------------------------------------------------------------
//  constant definitions - LITEON reduced key feature:
//-----------------------------------------------------------------------------
#if (_DRAM_ == DRAM_DDR4)
#define MC5_USE_SDK_7_SETTING           MC5_DISABLED
#define MC5_USE_SDK_7_ODT               MC5_DISABLED
#define MC5_USE_SDK_7_PHY_CTRL          MC5_ENABLED // KeivnKCChiu: DDR4 ONLY support enabled
#else
#define MC5_USE_SDK_7_SETTING           MC5_DISABLED
#define MC5_USE_SDK_7_ODT               MC5_DISABLED
#define MC5_USE_SDK_7_PHY_CTRL          MC5_ENABLED // KeivnKCChiu: DDR4 ONLY support enabled
#endif

#define MC5_AUTODETECT_DATA_WIDTH       MC5_DISABLED // KeivnKCChiu: disable data width detect
#define MC5_AUTODETECT_DEVICE_DENSITY   MC5_ENABLED
#define MC5_AUTODETECT_MEMORY_SIZE      MC5_ENABLED
#define MC5_AUTODETECT_CS               MC5_ENABLED

#define MC5_FORCE_ENABLE_CS1            MC5_ENABLED

// Enable if non-zero write leveling delay needed
#define MC5_PHY_RD_LVL_DLY      MC5_DISABLED//MC5_ENABLED
#define MC5_PHY_WR_LVL_DLY      MC5_DISABLED//MC5_ENABLED

//-----------------------------------------------------------------------------
//  constant definitions - feature:
//-----------------------------------------------------------------------------

// In case values below set to MC5_DISABLED
// pre-defined hard coded values will be used

// When set pad calibration procedure will be executed
#define MC5_PAD_CALIBRATION     MC5_ENABLED

/* ODT enable/disable. Affected registers :
   x310, 0x314, 0x340, 0x344 for all and 0x348 only for LPDDR3
   ODT is supported for DDR3 and LPDDR3; LPDDR2 doesn't support ODT.
   Please note: Termination will consume power.
   Optimize ODT and OBT based on SI analysis! */
#define MC5_PHY_ODT_EN          MC5_ENABLED

// When set DLL tuning procedure will be executed
#if CHK_FW_TYPE(FW_AGING)
#define MC5_DLL_TUNING          MC5_ENABLED
#else
#define MC5_DLL_TUNING          MC5_DISABLED
#endif

/* Operating Conditions according to the Speed Bins -
   set it in case vendor specific values used.
   It's recommended to set only possible options in vendor specific
   mc5MemoryDevice table in order to save RAM. */
/*
#define MC5_DDR_OPERATING_CONDITIONS_JEDEC      0
#define MC5_DDR_OPERATING_CONDITIONS_VENDOR     1
#define MC5_DDR_OPERATING_CONDITIONS            MC5_DDR_OPERATING_CONDITIONS_JEDEC
*/

//-----------------------------------------------------------------------------
//  constant definitions - feature: LPDDR
//-----------------------------------------------------------------------------
/* Temperature Derating for LPDDR -
   enabled to be on the safe side at any conditions */
#define MC5_TEMP_DERATING           MC5_ENABLED

/* Refresh per bank mode for LPDDR.
   By default automatic refresh is issued for all banks.
   In order to enable per bank refresh set to MC5_ENABLED. */
#define MC5_LPDDR_PER_BANK_MODE     MC5_DISABLED

//-----------------------------------------------------------------------------
/* When DDR type set to MC5_DDR_TYPE_AUTO it defined by upper layers
   - not recommended.
   Set to specific configuration to reduce footprint */
// Warning: value defined in register spec, DO NOT change the define
#define MC5_DDR_TYPE_AUTO       0x0
#define MC5_DDR_TYPE_DDR3       0x2
#define MC5_DDR_TYPE_DDR4       0x3
#define MC5_DDR_TYPE_LPDDR2     0x9
#define MC5_DDR_TYPE_LPDDR3     0xA
//#define MC5_DDR_TYPE            MC5_DDR_TYPE_AUTO

#if ((_DRAM_ & DRAM_TYPE_MASK) == DRAM_DDR)
    // DDR section
    #if     (_DRAM_ == DRAM_DDR3)
        #define MC5_DDR_TYPE        MC5_DDR_TYPE_DDR3
        #define MC5_DRAM_TYPE_T     cDramDdr3
    #elif   (_DRAM_ == DRAM_DDR4)
        #define MC5_DDR_TYPE        MC5_DDR_TYPE_DDR4
        #define MC5_DRAM_TYPE_T     cDramDdr4
    #else
        #error "Unsupport DDR type"
    #endif
    #define MC5_IS_LPDDR        FALSE

#elif ((_DRAM_ & DRAM_TYPE_MASK) == DRAM_LPDDR)
    // LPDDR section
    #if (_DRAM_ == DRAM_LPDDR3)
        #define MC5_DDR_TYPE        MC5_DDR_TYPE_LPDDR3
        #define MC5_DRAM_TYPE_T     cDramLpDdr3
    #else
        #error "Unsupport LPDDR type"
    #endif
    #define MC5_IS_LPDDR        TRUE
#endif

//-----------------------------------------------------------------------------
// Better to manually set number of ChipSelects if possible
#define MC5_CS0_ENABLED         MC5_ENABLED
#define MC5_CS_AUTODETECT       MC5_AUTODETECT_CS

#if (MC5_CS_AUTODETECT == MC5_ENABLED)
    #define MC5_CS1_ENABLED     MC5_ENABLED
#else
    /* Set this to MC5_ENABLED if you have CS1 enabled and not need autodetect.
       In this case MC5_CS_AUTODETECT should be MC5_DISABLED. */
    #define MC5_CS1_ENABLED     MC5_FORCE_ENABLE_CS1
#endif

#define MC5_NUM_OF_CS           (MC5_CS0_ENABLED + MC5_CS1_ENABLED)

//-----------------------------------------------------------------------------
//  constant definitions - MC5 parameter:
//-----------------------------------------------------------------------------

/* DDR burst length should be
   BL4 = 2 for LPDDR2 and
   BL8 = 3 for others*/
#define MC5_DDR_BURST_LEN_LPDDR2    2
#define MC5_DDR_BURST_LEN           3

// When set to MC5_SIZE_AUTO RAM size calculated automatically
#define MC5_SIZE_AUTO       0x00000000
#define MC5_SIZE_16_MB      0x01000000
#define MC5_SIZE_32_MB      0x02000000
#define MC5_SIZE_64_MB      0x04000000
#define MC5_SIZE_128_MB     0x08000000
#define MC5_SIZE_256_MB     0x10000000
#define MC5_SIZE_512_MB     0x20000000
#define MC5_SIZE_1_GB       0x40000000
#define MC5_SIZE_1_5_GB     0x60000000
#define MC5_SIZE_2_GB       0x80000000
#define MC5_SIZE_MAX        MC5_SIZE_2_GB

#if (MC5_AUTODETECT_MEMORY_SIZE == MC5_ENABLED)
#define MC5_DDR_SIZE        MC5_SIZE_MAX
#else
#define MC5_DDR_SIZE        (_DRAM_SIZE_ * MBYTE)
#endif

/* Single DDR device density -
   when set to MC5_SINGLE_DEVICE_DENSITY_AUTO calculated automatically*/
#define MC5_SINGLE_DEVICE_DENSITY_AUTO      0
#define MC5_SINGLE_DEVICE_DENSITY_DEFAULT   MC5_SIZE_512_MB
#define MC5_SINGLE_DEVICE_DENSITY           MC5_SINGLE_DEVICE_DENSITY_AUTO

/* When MC5_DDR_DATA_WIDTH_AUTO selected data width
   automatically set to 16 or 32 bit.
   For final customer product it's recommended to set
   data width to manual because it will reduce code footprint.
   Also for LPDDR3 only 32 bit supported so detection is not relevant. */
#define MC5_DDR_DATA_WIDTH_AUTO     1
#define MC5_DDR_DATA_WIDTH_16       2
#define MC5_DDR_DATA_WIDTH_32       3
#define MC5_DDR_DATA_WIDTH_DEFAULT  MC5_DDR_DATA_WIDTH_32 /* don't change this */

// In case of LPDDR3 only 32-bit data width supported
#if (MC5_AUTODETECT_DATA_WIDTH == MC5_ENABLED)
#define MC5_DDR_DATA_WIDTH  MC5_DDR_DATA_WIDTH_AUTO
#else
#define MC5_DDR_DATA_WIDTH  MC5_DDR_DATA_WIDTH_32
#endif

/* Assumption taken that if we have 32 bit in regular (non-LPDDR) memory,
   it organized in 2 dies x16.
   In case there is single die x32 DDR3
   set this define to MC5_32BIT_ORGANIZATION_1X32. */
#define MC5_32BIT_ORGANIZATION_2X16     1
#define MC5_32BIT_ORGANIZATION_1X32     2
#define MC5_32BIT_ORGANIZATION_DEFAULT  MC5_32BIT_ORGANIZATION_2X16

/* DDR4 device type set to x16 by default.
   Update it in case of different configuration.
   It's not a must but allows the PDA or
   MPR read instructions to work properly. */
#define MC5_DDR4_DEVICE_TYPE_X4       0
#define MC5_DDR4_DEVICE_TYPE_X8       1
#define MC5_DDR4_DEVICE_TYPE_X16      2
#define MC5_DDR4_DEVICE_TYPE_X32      3

#define MC5_DDR4_DEVICE_TYPE          MC5_DDR4_DEVICE_TYPE_X16

//-----------------------------------------------------------------------------
/* Please  note:
   the calibration pad uses a double-strength driver so
   connect to a 120 ohm or 150 ohm resistor to the pad
   for a 240 or 300 ohm result.
   Don't use 240 ohm resistor. */
#define MC5_CALIBRATION_RESISTOR_120    1
#define MC5_CALIBRATION_RESISTOR_240    2
#define MC5_PAD_CAL_RESISTOR_DIVISOR    MC5_CALIBRATION_RESISTOR_120

//-----------------------------------------------------------------------------
// Base of MC5 registers
#define MC5_BASE                0xF1000000
#define MC5_END                 0xF1000880

//-----------------------------------------------------------------------------
// In Eldora DDR memory starts from address 0x40000000
#define MC5_MEM_START_ADDRESS   DIRECT_DRAM_ADDR

// Offset taken from memory base in order to pass over possible data
#define MC5_MEM_TEST_OFFSET     0x100000
#define MC5_MEM_TEST_DELAY_US   1000

#define MC5_PATTERN_EMPTY_32    0x00000000
#define MC5_PATTERN_EMPTY_16    0x0000
#define MC5_PATTERN_FULL_32     0xFFFFFFFF
#define MC5_PATTERN_FULL_16     0xFFFF
#define MC5_PATTERN_TEST_32     0xCAFECAFE
#define MC5_PATTERN_TEST_16     0xCAFE

//-----------------------------------------------------------------------------
// Default value of FCLK used for tREFI register value calculation
#define MC5_DDR_FCLK_DEFAULT    40

//-----------------------------------------------------------------------------
//  constant definitions - JEDEC parameter:
//-----------------------------------------------------------------------------

// JEDEC related defines
//#define MC5_DEFAULT_DDR_T_REFI          7800000 // tREFI in picoSec - same for all densities

// KevinKCCuiu: 85C < TCASE < 95C
#define MC5_DEFAULT_DDR_T_REFI          3900000
#define MC5_DEFAULT_LPDDR_T_REFI        3900000 // tREFI for LPDDR refresh
#define MC5_DEFAULT_LPDDR_T_REFI_PB     487500  // tREFI for LPDDR refresh, PerBank

// Frequency
#define MC5_FREQ_533_MHZ                533  // Data Rate 1066 Mhz
#define MC5_FREQ_667_MHZ                667  // Data Rate 1333 Mhz
#define MC5_FREQ_800_MHZ                800  // Data Rate 1600 Mhz
#define MC5_FREQ_933_MHZ                933  // Data Rate 1866 Mhz
#define MC5_FREQ_1067_MHZ               1067 // Data Rate 2133 Mhz
#define MC5_FREQ_NUM_OF_OPTIONS         5
#define MC5_FREQ_NUM_OF_OPTIONS_DDR4    3

// Possible speed bins
#define MC5_SPEED_BIN_1066          MC5_FREQ_TO_DR(MC5_FREQ_533_MHZ)  // 1066
#define MC5_SPEED_BIN_1333          MC5_FREQ_TO_DR(MC5_FREQ_667_MHZ)  // 1333
#define MC5_SPEED_BIN_1600          MC5_FREQ_TO_DR(MC5_FREQ_800_MHZ)  // 1600
#define MC5_SPEED_BIN_1866          MC5_FREQ_TO_DR(MC5_FREQ_933_MHZ)  // 1866
#define MC5_SPEED_BIN_2133          MC5_FREQ_TO_DR(MC5_FREQ_1067_MHZ) // 2133

/* When speed bin set to MC5_SPEED_BIN_DEFAULT it means that it's taken
   from memory frequency. In case it's not the case set it explicitly.
   The same is for CL-nRCD-nRP. */
// KevinKCChiu: empty this define
#define MC5_SPEED_BIN               //MC5_SPEED_BIN_DEFAULT

// Typical CL-nRCD-nRP parameters for every speed bin
#define MC5_SPEED_BIN_1066_N_CL     7
#define MC5_SPEED_BIN_1333_N_CL     9
#define MC5_SPEED_BIN_1600_N_CL     11
#define MC5_SPEED_BIN_1866_N_CL     12
#define MC5_SPEED_BIN_2133_N_CL     13

// Typical CL-nRCD-nRP parameters for every speed bin - DDR4
#define MC5_SPEED_BIN_1600_N_CL_DDR4    12
#define MC5_SPEED_BIN_1866_N_CL_DDR4    14
#define MC5_SPEED_BIN_2133_N_CL_DDR4    16

/* LPDDR speed set to typical by default -
   can be updated if device speed is different */
#define MC5_LPDDR_SPEED_SLOW        0
#define MC5_LPDDR_SPEED_TYPICAL     1
#define MC5_LPDDR_SPEED_FAST        2
#define MC5_LPDDR_SPEED             MC5_LPDDR_SPEED_TYPICAL

// Block Address boundary
#define MC5_BA_BOUNDARY_16MB    0x11
#define MC5_BA_BOUNDARY_32MB    0x12
#define MC5_BA_BOUNDARY_64MB    0x13
#define MC5_BA_BOUNDARY_128MB   0x14
#define MC5_BA_BOUNDARY_256MB   0x15

// Number of rows
#define MC5_11_ROWS             0x1
#define MC5_12_ROWS             0x2
#define MC5_13_ROWS             0x3
#define MC5_14_ROWS             0x4
#define MC5_15_ROWS             0x5
#define MC5_16_ROWS             0x6
#define MC5_17_ROWS             0x7

/* End of File */


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
//! @file   HalMc5DdrTables.h
//!
//! @brief  Data tables used for MC5 DDR controller initialization
//
//=============================================================================

/******************************************************************************
*                                                                             *
* Dependencies                                                                *
*                                                                             *
******************************************************************************/

/******************************************************************************
*                                                                             *
* Private Function Declarations                                               *
*                                                                             *
******************************************************************************/

/******************************************************************************
*                                                                             *
* External Function Declarations                                              *
*                                                                             *
******************************************************************************/

// Drive Strength

/* When setting ZPDRV/ZNDRV and ZPTRM/ZNTRM, it is as follows:
Setting b0100: (CAL_RES=240)/1 =240  ohm
Setting b0010: (CAL_RES=240)/2 =120  ohm
Setting b0110: (CAL_RES=240)/3 =80   ohm
Setting b0001: (CAL_RES=240)/4 =60   ohm
Setting b0101: (CAL_RES=240)/5 =48   ohm
Setting b0011: (CAL_RES=240)/6 =40   ohm
Setting b0111: (CAL_RES=240)/7 =34.3 ohm
Setting b1011: (CAL_RES=240)/8 =30   ohm
Setting b1111: (CAL_RES=240)/9 =26.7 ohm
** NOTE CAL_RES is based on resistor on board.
    Check to make sure the CALIBRATION PIN is connected to 120 or 150 ohms
    calibration resistor (CAL_RES/2) on PCB,
    else above procedure will result in invalid results.
    The calibration pad uses a double-strength driver
    so connect to a 120 ohm or 150 ohm resistor to the
    pad for a 240 or 300 ohm result.
    To get +/-10% accuracy, Pad calibration needs to be performed

-- TABLE 1: Controller DRIVE STRENGTH for DDR3 (1.5V) --
|---------------------------------------------------|
|                   | DQ/DQS  | ADDR/CTRL | DDR DIC |
|-------------------|---------|-----------|---------|
|1 CS               | 50      | 50        | ZQ/6    |
|2 CS (y-topology)  | 35      | 35        | ZQ/7    |
|---------------------------------------------------|

-- TABLE 2: Controller DRIVE STRENGTH for DDR3L (1.35V) --
|---------------------------------------------------|
|                   | DQ/DQS  | ADDR/CTRL | DDR DIC |
|-------------------|---------|-----------|---------|
|1 CS               | 50      | 50        | ZQ/6    |
|2 CS (y-topology)  | 35      | 35        | ZQ/7    |
|---------------------------------------------------|
*/

/* ---------------- Start of ODT Section ---------------- */
/* ODT Settings for Single CS and two CSs with y-topology single sided.
   Two CSs clam shell configuration should not be used since
   Eldora doesn't have the address mirroring feature,
   so it defeats the purpose of the clamshell assembly.

-- TABLE 3: SINGLE CS --
|-------------------------------------------|
|          target DQ ODT RTT(ohm)           |
|-------------------------------------------|
|                     | controller | memory |
|---------------------|------------|--------|
|write to memory CS0  | off        | 60     |
|read from memory CS0 | 60         | off    |
|-------------------------------------------|

-- TABLE 4: TWO CSs (y - topology single side) --
|----------------------------------------------|
|          target DQ ODT RTT(ohm)              |
|----------------------------------------------|
|                     | controller | CS0 | CS1 |
|---------------------|------------|-----|-----|
|write to memory CS0  | off        | off | off |
|write to memory CS1  | off        | 40  | 40  |
|read from memory CS0 | 60         | off | off |
|read from memory CS1 | 60         | off | off |
|----------------------------------------------|

 b0001 - RZQ/4 = 60
 b0011 - RZQ/6 = 40
*/

/*  Size of ODT table have to be 2 always -
    entry 0 is write to CS and entry 1 read from CS.
    Inside the array entry 0 is for controller,
    entry 1 for CS0 and entry 2 for CS1.
*/
#if (MC5_PHY_ODT_EN == MC5_ENABLED)
#define ODT_OP_WRITE        0
#define ODT_OP_READ         1
#define ODT_OP_TABLE_SIZE   2

#define ODT_PART_CONTROLLER 0
#define ODT_PART_CS0        1
#define ODT_PART_CS1        2
#define ODT_PART_TABLE_SIZE 3

#if (MC5_DDR_TYPE != MC5_DDR_TYPE_LPDDR3)
static const Mc5DivisorValues_t ODT_Table_CS0_1[ODT_OP_TABLE_SIZE][ODT_PART_TABLE_SIZE] =
{
    #if 1//(MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
    {cValOff,    cVal60_Ohm, cValNa},
    {cVal60_Ohm, cValOff,    cValNa}
    #else // DDR4
    {cValOff,    cVal48_Ohm, cValNa},
    {cVal48_Ohm, cValOff,    cValNa}
    #endif
};
    #if (MC5_CS1_ENABLED == MC5_ENABLED)
    static const Mc5DivisorValues_t ODT_Table_CS0_2[ODT_OP_TABLE_SIZE][ODT_PART_TABLE_SIZE] =
    {
        #if 1//(MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
        {cValOff,    cValOff, cValOff},
        {cVal60_Ohm, cValOff, cValOff}
        #else // DDR4
        {cValOff,    cValOff, cValOff},
        {cVal48_Ohm, cValOff, cValOff}
        #endif
    };

    static const Mc5DivisorValues_t ODT_Table_CS1[ODT_OP_TABLE_SIZE][ODT_PART_TABLE_SIZE] =
    {
        #if (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
        {cValOff,    cVal40_Ohm, cVal40_Ohm},
        {cVal60_Ohm, cValOff,    cValOff   }
        #else // DDR4
        {cValOff,    cVal34_Ohm, cVal34_Ohm},
        {cVal48_Ohm, cValOff,    cValOff   }
        #endif
    };
    #endif
#endif

/*  Pad Termination Switch Mode.
    This field enables termination on the SOC pad
    when receiving data from the DRAM.
    The pad termination is not optimized to
    perfectly cover the valid DQS window.
0h: Always enable termination. Strength is controlled by the PHY registers.
1h: Enable Termination during any read or write data requests.
The PHY automatically disables this field on all writes.
2h: Enable termination during all reads.
3h: Disable termination.
*/
#if (USE_ACTIVE_POWER_OPTIMIZED_IF_SETTING == FEATURE_SUPPORTED)
#define MC5_PAD_TERM_SWITCH_MODE 2
#else
// KevinKCChiu: DDR3 only; force on for power saving; this is SDK5.0 setting
//#define MC5_PAD_TERM_SWITCH_MODE 0
#define MC5_PAD_TERM_SWITCH_MODE 2
#endif

#define MC5_ODT_SWITCH_MODE      2

#endif /* MC5_PHY_ODT_EN == MC5_ENABLED */

/* ---------------- End of ODT Section ---------------- */

/*
-- TABLE 5: Eldora Memory configuration table - maximum 2GB with 1 or 2 Chip Selects --
|-------------------------------------------------------------------------------------------------------------------------------------------|
|Total Memory       | 128 MB       | 256 MB       | 512 MB       | 1.0 GB       | 1.0 GB       | 1.5 GB       | 2.0 GB       | 2.0 GB       |
|-------------------------------------------------------------------------------------------------------------------------------------------|
|Chip Select 0      | 128 MB       | 256 MB       | 512 MB       | 0.5 GB       | 1.0 GB       | 1.0 GB       | 1.0 GB       | 2.0 GB*      |
|-------------------------------------------------------------------------------------------------------------------------------------------|
|RF100_0200h        | 0x400B_0001h | 0x400C_0001h | 0x400D_0001h | 0x400D_0001h | 0x400E_0001h | 0x400E_0001h | 0x400E_0001h | 0x0010_0001h |
|RF100_0200h[31:23] | 0x080h       | 0x080h       | 0x080h       | 0x080h       | 0x080h       | 0x080h       | 0x080h       | 0x000h       |
|RF100_0200h[20:16] | 0x0Bh        | 0x0Ch        | 0x0Dh        | 0x0Dh        | 0x0Eh        | 0x0Eh        | 0x0Eh        | 0x10h        |
|RF100_0200h[15:8]  | 0x00h        | 0x00h        | 0x00h        | 0x00h        | 0x00h        | 0x00h        | 0x00h        | 0x00h        |
|RF100_0200h[0]     | 1            | 1            | 1            | 1            | 1            | 1            | 1            | 1            |
|-------------------------------------------------------------------------------------------------------------------------------------------|
|Chip Select 1      | 0 GB         | 0 GB         | 0 GB         | 0.5 GB       | 0 GB         | 0.5 GB       | 1.0 GB       | 0 GB         |
|-------------------------------------------------------------------------------------------------------------------------------------------|
|RF100_0204h        | 0x0000_0000h | 0x0000_0000h | 0x0000_0000h | 0x600D_0001h | 0x0000_0000h | 0x800D_0001h | 0x800E_0001h | 0x0000_0000h |
|RF100_0204h[31:23] | 0x000h       | 0x000h       | 0x000h       | 0xC00h       | 0x000h       | 0x100h       | 0x100h       | 0x000h       |
|RF100_0204h[20:16] | 0x00h        | 0x00h        | 0x00h        | 0x0Dh        | 0x00h        | 0x0Dh        | 0x0Eh        | 0x00h        |
|RF100_0204h[15:8]  | 0x00h        | 0x00h        | 0x00h        | 0x00h        | 0x00h        | 0x00h        | 0x00h        | 0x00h        |
|RF100_0204h[0]     | 0            | 0            | 0            | 1            | 0            | 1            | 1            | 0            |
|-------------------------------------------------------------------------------------------------------------------------------------------|

* - Settings for 2GB on single CS are non-intuitive,
    we are counting on CPU address mapping to pass on
    any AXI address to the DDR Controller.
    With the DDR controller set to the entire CPU address space,
    all transactions with SDRAM will access CS0
*/

#define MC5_NUM_OF_DRAM_CATEGORY        3

// Possible size options for any CS configuration
//#define MAX_NUM_OF_SIZE_CONFIGS_CS0     5
#define MAX_NUM_OF_SIZE_CONFIGS_CS0     MC5_NUM_OF_DRAM_CATEGORY
#define MAX_NUM_OF_SIZE_CONFIGS_CS1     3

// Single CS values from "Eldora Memory configuration table" above
static const Mc5MemSizeConfig_t memSizeConfigCs0[MAX_NUM_OF_SIZE_CONFIGS_CS0] =
{
    //{0x400B0001, 0x00000000, MC5_SIZE_128_MB},  /* 128 MB */
    //{0x400C0001, 0x00000000, MC5_SIZE_256_MB},  /* 256 MB */
    {0x400D0001, 0x00000000, MC5_SIZE_512_MB},  /* 512 MB */
    {0x400E0001, 0x00000000, MC5_SIZE_1_GB  },  /*   1 GB */
    {0x00100001, 0x00000000, MC5_SIZE_2_GB  }   /*   2 GB */
};

#if (MC5_CS1_ENABLED == MC5_ENABLED)
#define MC5_CS1_DEFAULT_CONFIG          2 // Used for CS1 auto-detection

// Dual CS values from "Eldora Memory configuration table" above
static const Mc5MemSizeConfig_t memSizeConfigCs1[MAX_NUM_OF_SIZE_CONFIGS_CS1] =
{
    {0x400D0001, 0x600D0001, MC5_SIZE_1_GB  },  /*   1 GB */
    {0x400E0001, 0x800D0001, MC5_SIZE_1_5_GB},  /* 1.5 GB */
    {0x400E0001, 0x800E0001, MC5_SIZE_2_GB  }   /*   2 GB */
};
#endif

/* If CS1 is not exists set it to minimal size (8MB)
   and to unused address (0x25000000) and enable.
   Otherwise we will fail in exit from
   self refresh mode. For auto-detect only. */
#define MC5_CS1_NON_VALID_VALUE     0x25070001

//=============================================================================
// KevinKCChiu: Limit SizeConfig by dram type instead of size
#define MAX_NUM_OF_SIZE_CONFIGS_LPDDR3      5
#define MAX_NUM_OF_SIZE_CONFIGS_DDR         4
#define MAX_NUM_OF_SIZE_CONFIGS_DRAM        MAX(MAX_NUM_OF_SIZE_CONFIGS_LPDDR3, \
                                                MAX_NUM_OF_SIZE_CONFIGS_DDR)

// LPDDR3 CS values
static const Mc5MemSizeConfig_t memSizeConfigLpddr3[MAX_NUM_OF_SIZE_CONFIGS_LPDDR3] =
{
    {0x400D0001, 0x00000000, MC5_SIZE_512_MB},  /* 512 MB */
    {0x400E0001, 0x00000000, MC5_SIZE_1_GB  },  /*   1 GB */
    {0x400D0001, 0x600D0001, MC5_SIZE_1_GB  },  /*   1 GB */
    {0x400E0001, 0x800D0001, MC5_SIZE_1_5_GB},  /* 1.5 GB */
    {0x400E0001, 0x800E0001, MC5_SIZE_2_GB  }   /*   2 GB */
};

// DDR3/DDR4 CS values
static const Mc5MemSizeConfig_t memSizeConfigDdr3[MAX_NUM_OF_SIZE_CONFIGS_DDR] =
{
    {0x400D0001, 0x00000000, MC5_SIZE_512_MB},  /* 512 MB */
    {0x400E0001, 0x00000000, MC5_SIZE_1_GB  },  /*   1 GB */
    {0x00100001, 0x00000000, MC5_SIZE_2_GB  },  /*   2 GB */
    {0x400E0001, 0x800E0001, MC5_SIZE_2_GB  }   /*   2 GB */
};

#define MC5_CS1_DEFAULT_VALUE   0x800E0001

//=============================================================================
#define PHY_LEVELING_CTRL_TABLE_SIZE    4

// PHY RL Data Delay - should be tuned according to lab characterization
#if (MC5_PHY_RD_LVL_DLY == MC5_ENABLED)

static const Mc5PhyRLCtrl_t phyRLCtrlCs0Table[PHY_LEVELING_CTRL_TABLE_SIZE] =
{
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    {0x1, 0x5},
    {0x1, 0x5},
    {0x1, 0x5},
    {0x1, 0x5}
    #else // DDR3 & DDR4
    {0x1, 0x9},
    {0x1, 0x9},
    {0x1, 0x9},
    {0x1, 0x9}
    #endif
};
    #if (MC5_CS1_ENABLED == MC5_ENABLED)
    static const Mc5PhyRLCtrl_t phyRLCtrlCs1Table[PHY_LEVELING_CTRL_TABLE_SIZE] =
    {
        #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        {0x1, 0x5},
        {0x1, 0x5},
        {0x1, 0x5},
        {0x1, 0x5}
        #else // DDR3 & DDR4
        {0x1, 0xC},
        {0x1, 0xC},
        {0x1, 0xC},
        {0x1, 0xC}
        #endif
    };
    #endif

#endif

// PHY WL Data Delay - should be tuned according to lab characterization
#if (MC5_PHY_WR_LVL_DLY == MC5_ENABLED)

static const Mc5PhyWLCtrl_t phyWLCtrlCs0Table[PHY_LEVELING_CTRL_TABLE_SIZE] =
{
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    {0x3, 0x8, 0x0, 0x5},
    {0x3, 0x8, 0x0, 0x5},
    {0x3, 0x8, 0x0, 0x5},
    {0x3, 0x8, 0x0 ,0x5}
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
    {0x0, 0x0, 0x0, 0x0},
    {0x0, 0x0, 0x0, 0x0},
    {0x0, 0x0, 0x0, 0x0},
    {0x0, 0x0, 0x0, 0x0}
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
    {0x3, 0x0, 0x0, 0x1},
    {0x0, 0x0, 0x0, 0x0},
    {0x0, 0x0, 0x0, 0x0},
    //{0x5, 0x6, 0x3, 0x4},
    {0x0, 0x0, 0x0, 0x0}
    #endif
};
    #if (MC5_CS1_ENABLED == MC5_ENABLED)
    static const Mc5PhyWLCtrl_t phyWLCtrlCs1Table[PHY_LEVELING_CTRL_TABLE_SIZE] =
    {
        #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
        {0x3 ,0x8, 0x0, 0x5},
        {0x3 ,0x8, 0x0, 0x5},
        {0x3 ,0x8, 0x0, 0x5},
        {0x3 ,0x8, 0x0, 0x5}
        #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
        {0x0, 0x0, 0x0, 0x0},
        {0x0, 0x0, 0x0, 0x0},
        {0x0, 0x0, 0x0, 0x0},
        {0x0, 0x0, 0x0, 0x0}
        #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
        {0x3, 0x0, 0x0, 0x1},
        {0x0, 0x0, 0x0, 0x0},
        {0x0, 0x0, 0x0, 0x0},
        //{0x0, 0x1, 0x2, 0x0},
        //{0x2, 0x3, 0x2, 0x1},
        //{0x4, 0x5, 0x5, 0x4},
        //{0x5, 0x6, 0x3, 0x4},
        {0x0, 0x0, 0x0, 0x0}
        #endif
    };
    #endif

#endif

//=============================================================================
static const Mc5PhyCtrlRegVal_t phyCtrlRegValTable =
{
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    MC5_CHAN0_PHY_CTRL1_LPDDR,
    MC5_CHAN0_PHY_CTRL2_LPDDR,
    MC5_CHAN0_PHY_CTRL3_LPDDR,
    MC5_CHAN0_PHY_CTRL4_LPDDR,
    MC5_CHAN0_PHY_CTRL5_LPDDR,
    MC5_CHAN0_PHY_CTRL6_LPDDR
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
    MC5_CHAN0_PHY_CTRL1_DDR3,
    MC5_CHAN0_PHY_CTRL2_DDR3,
    MC5_CHAN0_PHY_CTRL3_DDR3,
    MC5_CHAN0_PHY_CTRL4_DDR3,
    MC5_CHAN0_PHY_CTRL5_DDR3,
    MC5_CHAN0_PHY_CTRL6_DDR3
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
    MC5_CHAN0_PHY_CTRL1_DDR4,
    MC5_CHAN0_PHY_CTRL2_DDR4,
    MC5_CHAN0_PHY_CTRL3_DDR4,
    MC5_CHAN0_PHY_CTRL4_DDR4,
    MC5_CHAN0_PHY_CTRL5_DDR4,
    MC5_CHAN0_PHY_CTRL6_DDR4
    #endif
};

/*
    BA bits should not be beyond total Row + Column + DeviceWidth bits.
    For example for 1Gb device on 16 bit data bus
    1  bit  for 16 bit data bus
    10 bits for column
    13 bits for row
    Total: 24 bits, therefore BA boundary must not be larger than 0x11(16MB).
    NOTE: This is setting for every CS
    (the CS1 if exists should be the same as CS0)
    So "Total Memory" is total memory for one CS.

-- TABLE 6: Memory BA boundary --
For x16 devices (so x32 = 2 * x16)
|------------|--------------|----------------|-------------------|-----------|------------------|
|Device Size | Total Memory | Number of Rows | Number of Columns | Bus Width |   BA Boundary    |
|------------|--------------|----------------|-------------------|-----------|------------------|
|    1Gb     |   128MB      |   13  (0x3)    |     10 (0x3)      |   1(x16)  |    0x11(16MB)    |
|    1Gb     |   256MB      |   13  (0x3)    |     10 (0x3)      |   2(x32)  |    0x12(32MB)    |
|    2Gb     |   256MB      |   14  (0x4)    |     10 (0x3)      |   1(x16)  |    0x12(32MB)    |
|    2Gb     |   512MB      |   14  (0x4)    |     10 (0x3)      |   2(x32)  |    0x13(64MB)    |
|    4Gb     |   512MB      |   15  (0x5)    |     10 (0x3)      |   1(x16)  |    0x13(64MB)    |
|    4Gb     |   1GB        |   15  (0x5)    |     10 (0x3)      |   2(x32)  |    0x14(128MB)   |
|    8Gb     |   1GB        |   16  (0x6)    |     10 (0x3)      |   1(x16)  |    0x14(128MB)   |
|    8Gb     |   2GB        |   16  (0x6)    |     10 (0x3)      |   2(x32)  |    0x15(256MB)   |
|    16Gb    |   2GB        |   17  (0x7)    |     10 (0x3)      |   1(x16)  |    0x15(256MB)   |
|------------|--------------|----------------|-------------------|-----------|------------------|

For x32 devices
|------------|--------------|----------------|-------------------|-----------|------------------|
|Device Size | Total Memory | Number of Rows | Number of Columns | Bus Width |   BA Boundary    |
|------------|--------------|----------------|-------------------|-----------|------------------|
|    1Gb     |   128MB      |   12  (0x2)    |     10 (0x3)      |   2(x32)  |    0x11(16MB)    |
|    2Gb     |   256MB      |   13  (0x3)    |     10 (0x3)      |   2(x32)  |    0x12(32MB)    |
|    4Gb     |   512MB      |   14  (0x4)    |     10 (0x3)      |   2(x32)  |    0x13(64MB)    |
|    8Gb     |   1GB        |   15  (0x5)    |     10 (0x3)      |   2(x32)  |    0x14(128MB)   |
|    16Gb    |   2GB        |   16  (0x6)    |     10 (0x3)      |   2(x32)  |    0x15(256MB)   |
|------------|--------------|----------------|-------------------|-----------|------------------|
*/


#define MC5_NUM_OF_COLUMNS      0x3 // Number of columns     is always 10 (0x3)
#define MC5_NUM_OF_BANKS        0x2 // Number of banks       is always 8  (0x2)
#define MC5_NUM_OF_BANK_GROUPS  0x0 // Number of bank groups is always 1  (0x0)

// for DDR4
#define MC5_NUM_OF_BANKS_DDR4           0x1 // Number of banks       is always 4 (0x1)
#define MC5_NUM_OF_BANK_GROUPS_DDR4     0x1 // Number of bank groups is always 2 (0x1)

//#define MC5_NUM_OF_BA_OPTIONS           5
#define MC5_NUM_OF_BA_OPTIONS           MC5_NUM_OF_DRAM_CATEGORY

static const Mc5BlckAddrBoundary_t blckAddrBoundaryTable16[MC5_NUM_OF_BA_OPTIONS] =
{
    //{MC5_SIZE_128_MB, MC5_13_ROWS, MC5_BA_BOUNDARY_16MB },
    //{MC5_SIZE_256_MB, MC5_14_ROWS, MC5_BA_BOUNDARY_32MB },
    {MC5_SIZE_512_MB, MC5_15_ROWS, MC5_BA_BOUNDARY_64MB }, // 0x13000532
    {MC5_SIZE_1_GB,   MC5_16_ROWS, MC5_BA_BOUNDARY_128MB}, // 0x14000632
    {MC5_SIZE_2_GB,   MC5_17_ROWS, MC5_BA_BOUNDARY_256MB}
};

static const Mc5BlckAddrBoundary_t blckAddrBoundaryTable32[MC5_NUM_OF_BA_OPTIONS] =
{
    //{MC5_SIZE_128_MB, MC5_12_ROWS, MC5_BA_BOUNDARY_16MB },
    //{MC5_SIZE_256_MB, MC5_13_ROWS, MC5_BA_BOUNDARY_32MB },
    {MC5_SIZE_512_MB, MC5_14_ROWS, MC5_BA_BOUNDARY_64MB }, // 0x13000432
    {MC5_SIZE_1_GB,   MC5_15_ROWS, MC5_BA_BOUNDARY_128MB}, // 0x14000532
    {MC5_SIZE_2_GB,   MC5_16_ROWS, MC5_BA_BOUNDARY_256MB}
};

/*
-- TABLE 7: Refresh parameters by device density for DDR/LPDDR from JEDEC --
|----------------------|--------|-------|-----|-----|-----|-----|-------|
|Parameter             | Symbol | 512Mb | 1Gb | 2Gb | 4Gb | 8Gb | Units |
|----------------------|--------|-------|-----|-----|-----|-----|-------|
|REF command to ACT or |  DDR3  |       |     |     |     |     |       |
|REF command time      |  tRFC  |  90   | 110 | 160 | 260 | 350 |   ns  |
|----------------------|--------|-------|-----|-----|-----|-----|-------|
|REF command to ACT or |LPDDR2/3|       |     |     |     |     |       |
|REF command time      |  tRFC  |  90   | 130 | 130 | 130 | 210 |   ns  |
|----------------------|--------|-------|-----|-----|-----|-----|-------|
|REF command to ACT    |LPDDR2/3|       |     |     |     |     |       |
|PB - per bank refresh |  tRFC  |  60   |  60 |  6  |  60 |  90 |   ns  |
|----------------------|--------|-------|-----|-----|-----|-----|-------|
|Average pereodic      |  DDR   |       |     |     |     |     |       |
|refresh interval      |  tREFI | 7800  | 7800| 7800| 7800| 7800|   ns  |
|----------------------|--------|-------|-----|-----|-----|-----|-------|
|Average pereodic      |  LPDDR |       |     |     |     |     |       |
|refresh interval      |  tREFI | 7800  | 7800| 3900| 3900| 3900|   ns  |
|----------------------|--------|-------|-----|-----|-----|-----|-------|
|Average pereodic      |  LPDDR |       |     |     |     |     |       |
|refresh interval -PB  |  tREFI | 487.5 |487.5|487.5|487.5|487.5|   ns  |
|----------------------|--------|-------|-----|-----|-----|-----|-------|
 */

//=============================================================================
// tRFC values stored in ps (picoSec) in order to avoid usage of "float" type
// Using 4Gb as default
#define MC5_DEFAULT_TRFC_VAL            260000  // Used in case of failure for DDR
#define MC5_DEFAULT_TRFC_VAL_LPDDR      130000  // Used in case of failure for LPDDR
#define MC5_DEFAULT_TRFC_VAL_PB_LPDDR   60000   // Used in case of failure for LPDDR, per bank mode

//#define MC5_NUM_OF_TRFC_VALUES          5
#define MC5_NUM_OF_TRFC_VALUES          MC5_NUM_OF_DRAM_CATEGORY
static const Mc5TrfcValues_t tRfcValues[MC5_NUM_OF_TRFC_VALUES] =
{
    //{MC5_SIZE_64_MB,  90000,  90000,  60000}, // 512Mb
    //{MC5_SIZE_128_MB, 110000, 130000, 60000}, // 1Gb
    {MC5_SIZE_256_MB, 160000, 130000, 60000}, // 2Gb
    {MC5_SIZE_512_MB, 260000, 130000, 60000}, // 4Gb
    {MC5_SIZE_1_GB,   350000, 210000, 90000}  // 8Gb
};

//=============================================================================
/*
-- TABLE 8: Rest of JEDEC parameters by DDR frequency from
JESD79-3F (DDR3),
JESD209-2B (LPDDR2) and
JESD209-3C (LPDDR3) --
    !!Please note -
    values in this table are typical taken from JEDEC and may
    not be optimum values for specific device.
    Please refer to the data sheet of the
    specific device and update the table.

    !!Please note: 2KB page size only supported!

Star "*" over "Symbol" means that value is the same for all frequencies

                                         |-----------|-----------|-----------|-----------|-----------|
                                         |                      Frequency in MHz                     |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Parameter                     | Symbol  |    533    |    677    |    800    |    933    |   1067    | Field Name | Comments     |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Exit Self Refresh to commands |   *     |           |           |           |           |           |            |              |
|requiring a locked DLL        |  tXSDLL |  512nCK   |  512nCK   |  512nCK   |  512nCK   |  512nCK   | T_XSRD     | DDR          |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Valid Clock Requirement before|         |           |           |           |           |           |            | For LPDDR    |
|Self Refresh Entry or         |   *     |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     | T_CKSRX_H  | should be    |
|Power-Down Entry              |  tCKSRX | 5nCK/10ns | 5nCK/10ns | 5nCK/10ns | 5nCK/10ns | 5nCK/10ns | T_CKSRX    | as T_CKESR   |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Exit Self Refresh to commands |   *     | MAX 5nCK /| MAX 5nCK /| MAX 5nCK /| MAX 5nCK /| MAX 5nCK /|            |              |
|not requiring a locked DLL    |  tXS    |tRFC + 10ns|tRFC + 10ns|tRFC + 10ns|tRFC + 10ns|tRFC + 10ns| T_XSNR     |              |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Valid Clock Requirement after |         |           |           |           |           |           |            | For LPDDR    |
|Self Refresh Entry or         |   *     |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     | T_CKSRE_H  | should be    |
|Power-Down Entry              |  tCKSRE | 5nCK/10ns | 5nCK/10ns | 5nCK/10ns | 5nCK/10ns | 5nCK/10ns | T_CKSRE    | as T_CKESR   |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Exit PreCharge PowerDown with |         |           |           |           |           |           |            | DDR          |
|DLL frozen to commands        |   *     |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     |            | For LPDDR    |
|requiring a locked DLL        |  tXPDLL | 10nCK/24ns| 10nCK/24ns| 10nCK/24ns| 10nCK/24ns| 10nCK/24ns| T_XARDS    | same as tXP  |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Exit PowerDown to next        |         |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     |            |              |
|valid command delay           |  tXP    | 3nCK/7.5ns| 3nCK/6ns  | 3nCK/6ns  | 3nCK/6ns  | 3nCK/6ns  | T_XP       | DDR          |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Exit PowerDown to next        |         |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     |            |              |
|valid command delay           |  tXP    | 3nCK/7.5ns| 3nCK/7.5ns| 3nCK/7.5ns| 3nCK/7.5ns| 3nCK/7.5ns| T_XP_L     | LPDDR        |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|                              |         | MAX 3nCK /| MAX 3nCK /| MAX 3nCK /| MAX 3nCK /| MAX 3nCK /|            | Used for     |
|CKE minimum pulse width       |  tCKE   | 5.625ns   | 5.625ns   | 5ns       | 5ns       | 5ns       | T_CKE      | tCKESR calc. |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|                              |         |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     |            |              |
|CKE minimum pulse width       |  tCKE   | 3nCK/7.5ns| 3nCK/7.5ns| 3nCK/7.5ns| 3nCK/7.5ns| 3nCK/7.5ns| T_CKE_L    | LPDDR        |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Minimum CKE low width for Self|         |           |           |           |           |           |            |              |
|Refresh entry to exit timing  |  tCKESR |tCKE + 1nCK|tCKE + 1nCK|tCKE + 1nCK|tCKE + 1nCK|tCKE + 1nCK| T_CKESR    |              |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Command path disable delay    |  tCPDED |   1nCK    |   1nCK    |   1nCK    |   2nCK    |   2nCK    | T_CPDED    | l for LPDDR  |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Timing of PRE or PREA command |         |           |           |           |           |           |            |              |
|to Power Down entry           |  tPRPDEN|   1nCK    |   1nCK    |   1nCK    |   1nCK    |   2nCK    | T_PDEN     | l for LPDDR  |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Mode Register Set command     |   *     |           |           |           |           |           |            |              |
|cycle time                    |  tMRD   |   4nCK    |   4nCK    |   4nCK    |   4nCK    |   4nCK    | T_MRD      | DDR          |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Mode Register Set command     |   *     |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     |            |              |
|cycle time                    |  tMRD   | 10nCK/14ns| 10nCK/14ns| 10nCK/14ns| 10nCK/14ns| 10nCK/14ns| T_MRD_L    | LPDDR        |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Mode Register Set command     |   *     |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     |            |              |
|update delay                  |  tMOD   | 12nCK/15ns| 12nCK/15ns| 12nCK/15ns| 12nCK/15ns| 12nCK/15ns| T_MOD      | 2 for LPDDR  |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|4-bank ACTIVATE window        |  tFAW   |   50ns    |   45ns    |   40ns    |   35ns    |   35ns    | T_FAW      | DDR          |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|4-bank ACTIVATE window        |  tFAW   |   50ns    |   50ns    |   50ns    |   50ns    |   50ns    | T_FAW_L    | LPDDR        |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Internal READ Command to      |   *     |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     |            |              |
|PRECHARGE Command delay       |  tRTP   | 4nCK/7.5ns| 4nCK/7.5ns| 4nCK/7.5ns| 4nCK/7.5ns| 4nCK/7.5ns| T_RTP      |              |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|                              |   *     |           |           |           |           |           |            |              |
|WRITE recovery time           |  tWR    |   15ns    |   15ns    |   15ns    |   15ns    |   15ns    | T_WR       | DDR          |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|                              |   *     |           |           |           |           |           |            |              |
|WRITE recovery time           |  tWR_L  | 4nCK/15ns | 4nCK/15ns | 4nCK/15ns | 4nCK/15ns | 4nCK/15ns | T_WR_L     | LPDDR        |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|Internal Write-2-Read         |   *     |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     |            |              |
|command delay                 |  tWTR   | 4nCK/7.5ns| 4nCK/7.5ns| 4nCK/7.5ns| 4nCK/7.5ns| 4nCK/7.5ns| T_WTR      |              |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|                              |   *     |           |           |           |           |           |            |              |
|CAS-to-CAS command delay      |  tCCD   |   4nCK    |   4nCK    |   4nCK    |   4nCK    |   4nCK    | T_CCD      |              |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|                              |         |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     |            |              |
|ACTIVE2ACTIVE command period  |  tRRD   | 4nCK/10ns | 4nCK/7.5ns| 4nCK/7.5ns| 4nCK/6ns  | 4nCK/6ns  | T_RRD      | DDR          |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|                              |         |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     |            |              |
|ACTIVE2ACTIVE command period  |  tRRD   | 2nCK/10ns | 2nCK/10ns | 2nCK/10ns | 2nCK/10ns | 2nCK/10ns | T_RRD_L    | LPDDR        |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|DQS output access time from   |   *     | MIN 2.5ns | MIN 2.5ns | MIN 2.5ns | MIN 2.5ns | MIN 2.5ns |            | AVG 4ns      |
|CK_t/CK_c                     |  tDQSCK | MAX 5.5ns | MAX 5.5ns | MAX 5.5ns | MAX 5.5ns | MAX 5.5ns | T_DQSCK    | LPDDR only   |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|CKE LOW after CA calibration  |   *     |           |           |           |           |           |            |              |
|mode is programmed            |  tCACKEL|   10nCK   |   10nCK   |   10nCK   |   10nCK   |   10nCK   | T_CACKEL   | LPDDR only   |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|CA calibration exit command   |   *     |           |           |           |           |           |            |              |
|after CKE is HIGH             |  tCAEXT |   10nCK   |   10nCK   |   10nCK   |   10nCK   |   10nCK   | T_CAEXT    | LPDDR only   |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|ZQ Initialization (Power Up   |   *     | MAX 512nCK| MAX 512nCK| MAX 512nCK| MAX 512nCK| MAX 512nCK|            |              |
|and Reset) calibration time   |  tZQINIT|   /640ns  |   /640ns  |   /640ns  |   /640ns  |   /640ns  | T_ZQINIT   | DDR          |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|ZQ Initialization (Power Up   |   *     |           |           |           |           |           |            |              |
|and Reset) calibration time   |  tZQINIT|   1000ns  |   1000ns  |   1000ns  |   1000ns  |   1000ns  | T_ZQINIT_L | LPDDR        |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|ZQ Long calibration time      |   *     | MAX 256nCK| MAX 256nCK| MAX 256nCK| MAX 256nCK| MAX 256nCK|            |              |
|                              |  tZQCL  |   /320ns  |   /320ns  |   /320ns  |   /320ns  |   /320ns  | T_ZQOPER   | DDR          |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|ZQ Long calibration time      |   *     |           |           |           |           |           |            |              |
|                              |  tZQCL  |   360ns   |   360ns   |   360ns   |   360ns   |   360ns   | T_ZQOPER_L | LPDDR        |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|ZQ Short calibration time     |   *     |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     |            |              |
|                              |  tZQCS  | 64nCK/80ns| 64nCK/80ns| 64nCK/80ns| 64nCK/80ns| 64nCK/80ns| T_ZQCS     | DDR          |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|ZQ Short calibration time     |   *     |           |           |           |           |           |            |              |
|                              |  tZQCS  |   90ns    |   90ns    |   90ns    |   90ns    |   90ns    | T_ZQCS_L   | LPDDR        |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
|ZQ Calibration RESET time     |   *     |   MAX     |   MAX     |   MAX     |   MAX     |   MAX     |            |              |
|                              |  tZQRES | 3nCK/50ns | 3nCK/50ns | 3nCK/50ns | 3nCK/50ns | 3nCK/50ns | T_ZQCR     | LPDDR only   |
|------------------------------|---------|-----------|-----------|-----------|-----------|-----------|------------|--------------|
*/
static const Mc5JedecParams_t mc5JedecParams =
{
    512,                // T_XSRD_nck
    {5,  10000},        // T_CKSRX
    {5,  10000},        // T_XSNR
    {5,  10000},        // T_CKSRE
    {10, 24000},        // T_XARDS
    {{3, 7500}, {3, 6000}, {3, 6000}, {3, 6000}, {3, 6000}},  // T_XP
    {4, 6000},          // T_XP_4
    {3, 7500},          // T_XP_L
    {{3, 5625}, {3, 5625}, {3, 5000}, {3, 5000}, {3, 5000}},  // T_CKE
    {3, 7500},          // T_CKE_L
    {1, 1, 1, 2, 2},    // T_CPDED_nck
    {1, 1, 1, 1, 2},    // T_PDEN_nck
    4,                  // T_MRD_nck
    8,                  // T_MRD_4_nck
    {10, 14000},        // T_MRD_L
    {12, 15000},        // T_MOD
    {24, 15000},        // T_MOD_4
    {50000, 45000, 40000, 35000, 35000}, // T_FAW_ps
    30000,          // T_FAW_4_ps
    50000,          // T_FAW_L
    {4, 7500},      // T_RTP
    15000,          // T_WR_ps
    {4, 15000},     // T_WR_L
    {4, 7500},      // T_WTR
    4,              // T_CCD_nck
    {{4, 10000}, {4, 7500}, {4, 7500}, {4, 6000}, {4, 6000}},  // T_RRD
    {2, 10000},     // T_RRD_L
    4000,           // T_DQSCK_ps
    10,             // T_CACKEL_nck
    10,             // T_CAEXT_nck
    {512, 640000},  // T_ZQINIT
    1024,           // T_ZQINIT_4_nck
    1000000,        // T_ZQINIT_L_ps
    {256, 320000},  // T_ZQOPER
    512,            // T_ZQOPER_4_nck
    360000,         // T_ZQOPER_L_ps
    {64, 80000},    // T_ZQCS
    128,            // T_ZQCS_4_nck
    90000,          // T_ZQCS_L_ps
    {3, 50000},     // T_ZQCR
    {0, 0, 2, 3, 3},// T_WTR_S_nck
    4,              // T_CCD_S_nck
    {{0, 0}, {0, 0}, {4, 6000}, {4, 5300}, {4, 5300}}   // T_RRD_S
};

//=============================================================================
/*
-- TABLE 9: Speed Bins and Operating Conditions from JEDEC --
|---------------------|---------|-----------|----------------|
|       Speed Bin     |   tRAS  |    tRC    | tRCD/tRP/tRPA  |
|---------------------|---------|-----------|----------------|
| DDR3-1066E 6-6-6    |  37.5ns |  48.75ns  |       11.25ns  |
|---------------------|---------|-----------|----------------|
| DDR3-1066F 7-7-7    |  37.5ns | 50.625ns  |      13.125ns  |
|---------------------|---------|-----------|----------------|
| DDR3-1066G 8-8-8    |  37.5ns |   52.5ns  |          15ns  |
|---------------------|---------|-----------|----------------|
| DDR3-1333G 8-8-8    |    36ns |     48ns  |          12ns  |
|---------------------|---------|-----------|----------------|
| DDR3-1333H 9-9-9    |    36ns |   49.5ns  |        13.5ns  |
|---------------------|---------|-----------|----------------|
| DDR3-1600H 9-9-9    |    35ns |  46.25ns  |       11.25ns  |
|---------------------|---------|-----------|----------------|
| DDR3-1600J 10-10-10 |    35ns |   47.5ns  |        12.5ns  |
|---------------------|---------|-----------|----------------|
| DDR3-1600K 11-11-11 |    35ns |  48.75ns  |       13.75ns  |
|---------------------|---------|-----------|----------------|
| DDR3-1866K 11-11-11 |    34ns |  45.77ns  |       11.77ns  |
|---------------------|---------|-----------|----------------|
| DDR3-1866L 12-12-12 |    34ns |  46.84ns  |       12.84ns  |
|---------------------|---------|-----------|----------------|
| DDR3-2133L 12-12-12 |    33ns |  44.22ns  |       11.22ns  |
|---------------------|---------|-----------|----------------|
| DDR3-2133M 13-13-13 |    33ns | 45.155ns  |      12.155ns  |
|---------------------|---------|-----------|----------------|

-- TABLE 10: Vendor specific Speed Bins and Operating Conditions --
SK HYNIX is following JEDEC, NANYA and MICRON have values different from JEDEC
|---------------------|---------|----------|---------------|
|       Speed Bin     |   tRAS  |    tRC   | tRCD/tRP/tRPA |
|---------------------|---------|----------|---------------|
|   MICRON / NANYA    |         |          |               |
| DDR3-1333 9-9-9     |   24ns  |   34ns   |     10ns      |
|---------------------|---------|----------|---------------|
|   MICRON / NANYA    |         |          |               |
| DDR3-1600 11-11-11  |   28ns  |   39ns   |     11ns      |
|---------------------|---------|----------|---------------|

-- TABLE 11: DDR3 CAS Latency and CW Latency by DDR speed and speed bin --
Given in format CL/CWL in nCK.
Supported CL/CWL for given Speed Bin and Latency should have value for
specific memory frequency (given in nCK = 1000/mem_freq).
Please refer to JEDEC spec - Table "Speed Bins and Operating Conditions".
|---------------------|--------|--------|--------|--------|--------|--------|---------|---------|---------|---------|---------|---------|
|       Speed Bin     |  1066  |  1066  |  1066  |  1333  |  1333  |  1600  |  1600   |  1600   |  1866   |  1866   |  2133   |  2133   |
|---------------------| 6-6-6  | 7-7-7  | 8-8-8  | 8-8-8  | 9-9-9  | 9-9-9  | 10-10-10| 11-11-11| 11-11-11| 12-12-12| 12-12-12| 13-13-13|
|    Memory frequency |        |        |        |        |        |        |         |         |         |         |         |         |
|---------------------|--------|--------|--------|--------|--------|--------|---------|---------|---------|---------|---------|---------|
|        533MHz       |  6/6   |  7/6   |  8/6   |  7/6   |  7/6   |  6/6   |   7/6   |   7/6   |   7/6   |   7/6   |   6/6   |   7/6   |
|---------------------|--------|--------|--------|--------|--------|--------|---------|---------|---------|---------|---------|---------|
|        667MHz       |   NA   |   NA   |   NA   |  8/7   |  9/7   |  8/7   |   9/7   |   9/7   |   8/7   |   9/7   |   8/7   |   9/7   |
|---------------------|--------|--------|--------|--------|--------|--------|---------|---------|---------|---------|---------|---------|
|        800MHz       |   NA   |   NA   |   NA   |   NA   |   NA   |  9/8   |  10/7   |  11/8   |  10/8   |  11/8   |   9/8   |  10/8   |
|---------------------|--------|--------|--------|--------|--------|--------|---------|---------|---------|---------|---------|---------|
|        933MHz       |   NA   |   NA   |   NA   |   NA   |   NA   |   NA   |    NA   |    NA   |  11/9   |  12/9   |  11/9   |  12/9   |
|---------------------|--------|--------|--------|--------|--------|--------|---------|---------|---------|---------|---------|---------|
|       1067MHz       |   NA   |   NA   |   NA   |   NA   |   NA   |   NA   |    NA   |    NA   |    NA   |    NA   | 13/10   | 14/10   |
|---------------------|--------|--------|--------|--------|--------|--------|---------|---------|---------|---------|---------|---------|
*/

#if (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
#define MC5_SPEED_BIN_JEDEC_OPTIONS     12
#define MC5_SPEED_BIN_OPTIONS           MC5_SPEED_BIN_JEDEC_OPTIONS
#define MC5_SPEED_BIN_DEFAULT           7 // Should be 1600 11-11-11

// Values from tables 9 and 11. tRAS/tRC/tRP values in ps (picoSec), CL and CWL in nCK
static const Mc5MemoryDevice_t mc5MemoryDevice[MC5_SPEED_BIN_JEDEC_OPTIONS] =
{
    {{MC5_SPEED_BIN_1066,  6}, 37500, 48750, 11250, {{6, 6}, {0, 0}, {0, 0}, {0, 0}, {0,  0}}},
    {{MC5_SPEED_BIN_1066,  7}, 37500, 50625, 13125, {{7, 6}, {0, 0}, {0, 0}, {0, 0}, {0,  0}}},
    {{MC5_SPEED_BIN_1066,  8}, 37500, 52500, 15000, {{8, 6}, {0, 0}, {0, 0}, {0, 0}, {0,  0}}},
    {{MC5_SPEED_BIN_1333,  8}, 36000, 48000, 12000, {{7, 6}, {8, 7}, {0, 0}, {0, 0}, {0,  0}}},
    {{MC5_SPEED_BIN_1333,  9}, 36000, 49500, 13500, {{7, 6}, {9, 7}, {0, 0}, {0, 0}, {0,  0}}},
    {{MC5_SPEED_BIN_1600,  9}, 35000, 46250, 11250, {{6, 6}, {8, 7}, {9, 8}, {0, 0}, {0,  0}}},
    {{MC5_SPEED_BIN_1600, 10}, 35000, 47500, 12500, {{7, 6}, {9, 7}, {10,7}, {0, 0}, {0,  0}}},
    {{MC5_SPEED_BIN_1600, 11}, 35000, 48750, 13750, {{7, 6}, {9, 7}, {11,8}, {0, 0}, {0,  0}}},
    {{MC5_SPEED_BIN_1866, 11}, 34000, 45770, 11770, {{7, 6}, {8, 7}, {10,8}, {11,9}, {0,  0}}},
    {{MC5_SPEED_BIN_1866, 12}, 34000, 46840, 12840, {{7, 6}, {9, 7}, {11,8}, {12,9}, {0,  0}}},
    {{MC5_SPEED_BIN_2133, 12}, 33000, 44220, 11220, {{6, 6}, {8, 7}, {9, 8}, {11,9}, {13,10}}},
    {{MC5_SPEED_BIN_2133, 13}, 33000, 45155, 12155, {{7, 6}, {9, 7}, {10,8}, {12,9}, {14,10}}}
};

/*
-- TABLE 11A: DDR4 CAS Latency and CW Latency by DDR speed and speed bin --
Given in format CL/CWL in nCK.
Supported CL/CWL for given Speed Bin and Latency should have value for specific memory frequency (given in nCK = 1000/mem_freq).
|---------------------|--------|--------|--------|--------|--------|--------|---------|---------|---------|
|       Speed Bin     |  1600  |  1600  |  1600  |  1866  |  1866  |  1866  |  2133   |  2133   |  2133   |
|---------------------|10-10-10|11-11-11|12-12-12|12-12-12|13-13-13|14-14-14| 14-14-14| 15-15-15| 16-16-16|
|    Memory frequency |        |        |        |        |        |        |         |         |         |
|---------------------|--------|--------|--------|--------|--------|--------|---------|---------|---------|
|        800MHz       |  10/9  |  11/11 | 12/11  |   NA   |   NA   |   NA   |    NA   |    NA   |    NA   |
|---------------------|--------|--------|--------|--------|--------|--------|---------|---------|---------|
|        933MHz       |   NA   |   NA   |   NA   |  12/10 |  13/12 |  14/12 |    NA   |    NA   |    NA   |
|---------------------|--------|--------|--------|--------|--------|--------|---------|---------|---------|
|       1067MHz       |   NA   |   NA   |   NA   |   NA   |   NA   |   NA   |  14/11  |  15/14  |  16/14  |
|---------------------|--------|--------|--------|--------|--------|--------|---------|---------|---------|
*/

#elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
#define MC5_SPEED_BIN_OPTIONS_DDR4      9
#define MC5_SPEED_BIN_DEFAULT_DDR4      2 // Should be 1600 12-12-12

// tRAS/tRC/tRP/CL/CLW values in nCK
static const Mc5MemoryDeviceDdr4_t mc5MemoryDeviceDdr4[MC5_SPEED_BIN_OPTIONS_DDR4] =
{
    {{MC5_SPEED_BIN_1600, 10}, 28, 38, 10, {10, 9 }},
    {{MC5_SPEED_BIN_1600, 11}, 28, 39, 11, {11, 11}},
    {{MC5_SPEED_BIN_1600, 12}, 28, 40, 12, {12, 11}},
    {{MC5_SPEED_BIN_1866, 12}, 32, 44, 12, {12, 10}},
    {{MC5_SPEED_BIN_1866, 13}, 32, 45, 13, {13, 12}},
    {{MC5_SPEED_BIN_1866, 14}, 32, 46, 14, {14, 12}},
    {{MC5_SPEED_BIN_2133, 14}, 36, 50, 14, {14, 11}},
    {{MC5_SPEED_BIN_2133, 15}, 36, 51, 15, {15, 14}},
    {{MC5_SPEED_BIN_2133, 16}, 36, 52, 16, {16, 14}}
};
#endif

//=============================================================================
// Typical (default) CAS latency for every speed bin
static const Mc5SpeedBin_t mc5TypicalDevice[MC5_FREQ_NUM_OF_OPTIONS] =
{
    {MC5_SPEED_BIN_1066, MC5_SPEED_BIN_1066_N_CL},
    {MC5_SPEED_BIN_1333, MC5_SPEED_BIN_1333_N_CL},
    {MC5_SPEED_BIN_1600, MC5_SPEED_BIN_1600_N_CL},
    {MC5_SPEED_BIN_1866, MC5_SPEED_BIN_1866_N_CL},
    {MC5_SPEED_BIN_2133, MC5_SPEED_BIN_2133_N_CL}
};

static const Mc5SpeedBin_t mc5TypicalDeviceDdr4[MC5_FREQ_NUM_OF_OPTIONS_DDR4] =
{
    {MC5_SPEED_BIN_1600, MC5_SPEED_BIN_1600_N_CL_DDR4},
    {MC5_SPEED_BIN_1866, MC5_SPEED_BIN_1866_N_CL_DDR4},
    {MC5_SPEED_BIN_2133, MC5_SPEED_BIN_2133_N_CL_DDR4}
};

//=============================================================================
// LPDDR
// RL (read latency) and WR (write latency) for LPDDR
static const Mc5ClTimingParams_t mc5ClTimingParamLpddr3[MC5_FREQ_NUM_OF_OPTIONS] =
{
    {8,  4},
    {10, 6},
    {12, 6},
    {14, 8},
    {16, 8}
};

// tRAS value for LPDDR
static const Mc5NckPs_t mc5LpddrTrasVal = {3, 42000};

// Values of tRC/tRP
#define MC5_LPDDR_NUM_OF_SPEED_OPTIONS      3
static const Mc5LpddrMemoryDevice_t mc5LpddrMemoryDevice[MC5_LPDDR_NUM_OF_SPEED_OPTIONS] =
{
    {{3, 24000}, {3, 24000}, {3, 27000}}, // Slow
    {{3, 18000}, {3, 18000}, {3, 21000}}, // Typical
    {{3, 15000}, {3, 15000}, {3, 18000}}  // Fast
};

//=============================================================================
// Initialization Count NOP - 500 us for DDR, 200 us for LPDDR
// Cold Reset Count - RESET# is asserted for this many cycles during a cold reset 200us/100ns
// Warm Reset Count - RESET# is asserted for this many cycles during a warm reset 100ns/10us
static const Mc5CountValPs_t mc5CountValPs =
{
    {500 * MC5_US_TO_PS, 200 * MC5_US_TO_PS},
    {200 * MC5_US_TO_PS, 100 * MC5_NS_TO_PS},
    {100 * MC5_NS_TO_PS, 10  * MC5_US_TO_PS}
};

/* Off-Spec timing parameters:
   Extra CAS-to-CAS Read Delay,
   Extra CAS-to-CAS Write Delay,
   READ to WRITE Command Delay,
   Write Leveling Early and Read Gap Extend for DDR and LPDDR
*/
static const Mc5OffSpecTimingParams_t mc5OffSpecTimingParams[cDramCategoryCount] =
{
    {1, 1, 4, 0, 0},
    {3, 2, 1, 0, 5}
};

//=============================================================================
static const phyDllData_t mc5DllDataDefault =
{
    #if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
    {0x10, 0x10, 0x10, 0x10},
    {0x1E, 0x1E, 0x1E, 0x1E}
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR3)
    {0x12, 0x12, 0x12, 0x12},
    {0x1A, 0x1A, 0x1A, 0x1A}
    #elif (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
    // 2133MT
    {0x13, 0x12, 0x0C, 0x0D},
    {0x21, 0x20, 0x19, 0x1C}
    #endif
};

#if (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
#define MC5_NUM_OF_PLLA_CTRL1_VALUES    10
#define MC5_NUM_OF_PLLA_CTRL1_DEFAULT   (MC5_NUM_OF_PLLA_CTRL1_VALUES - 1) // Set 1000 as default

static const Mc5PllACtrl1Values_t mc5PllACtrl1Values[MC5_NUM_OF_PLLA_CTRL1_VALUES] =
{
    {0x012853F0,  400,  800}, // [8]: 1; [9]: 1; [18:10] :20; [22:19]: 5; [31:23]: 2
    {0x00886AF0,  533, 1067}, // [8]: 0; [9]: 1; [18:10] :26; [22:19]: 1; [31:23]: 1
    {0x00903FF0,  600, 1200}, // [8]: 1; [9]: 1; [18:10] :15; [22:19]: 2; [31:23]: 1
    {0x00A853F0,  800, 1333}, // [8]: 1; [9]: 1; [18:10] :20; [22:19]: 5; [31:23]: 1
    {0x00A04BF0,  720, 1440},
    {0x00A04FF0,  760, 1520},
    {0x00A853F0,  800, 1600},
    {0x00B05BF0,  900, 1800},
    {0x00B85FF0,  933, 1866}, // [8]: 1; [9]: 1; [18:10] :23; [22:19]: 7; [31:23]: 1
    {0x00C067F0, 1000, 2133}  // [8]: 1; [9]: 1; [18:10] :25; [22:19]: 8; [31:23]: 1
};

///  @brief 0xF0112010 : PLLA Control 1
///<BIT [0] PLL A Lock Detect.
///<BIT [1] Reserved.
///<BIT [3:2] PLL A VCO Capacitor Select.
///<BIT [4] PLL A Clock Detect Enable.
///<BIT [5] PLL A Clock Source Select.
///<BIT [6] PLL A Lock Control.
///<BIT [7] PLL A Clock Bypass Control.
///<BIT [8] PLL A Bandwidth Select.
///<BIT [9] PLL A AVDD Select.
///<BIT [18:10] PLL A Feedback Clock Divider.
///<BIT [22:19] PLL A KVCO Selection.
///<BIT [31:23] PLL A Post-Divider for Single-Ended Output Clock Control.
#endif

/* End of File */


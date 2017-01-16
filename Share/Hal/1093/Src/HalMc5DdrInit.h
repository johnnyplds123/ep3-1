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
//! @file   HalMc5DdrInit.h
//!
//! @brief  DDR Init values and typedefs
//
//=============================================================================

/**********************************************************
 * GUIDE from MC5 Manual
 * The following are minimum settings required to bring up
 * the DDR Memory Controller.
 *
 * 1) Using the DRAM vendor specification, program all
 *    twelve timing registers by taking the required
 *    minimum value and dividing it by the DRAM clock
 *    period, rounded up. Refer to Section 1.4.4, Timing
 *    Registers for more information.
 *
 *    - 0x380 DDR_INIT_TIMING_CONTROL_0
 *    - 0x384 DDR_INIT_TIMING_CONTROL_1
 *    - 0x388 ZQC_TIMING_0
 *    - 0x38C ZQC_TIMING_1
 *    - 0x390 REFRESH_TIMING
 *    - 0x394 SELFREFRESH_TIMING
 *    - 0x398 POWER_DOWN_TIMING
 *    - 0x39C MRS_TIMING
 *    - 0x3A0 ACT_TIMING
 *    - 0x3A4 PRE_CHARGE_TIMING
 *    - 0x3A8 CAS_RAS_TIMING
 *    - 0x3AC OFF_SPEC_TIMING
 *
 * 2) Program DRAM Configuration Register 1 Note that
 *    several fields are noted with *Requires Mode
 *    Register update*. The other control registers
 *    contain non-essential yet potentially-useful features.
 *
 * 3) If ODT is desired, program DRAM Control Register 1
 *    and DRAM Control Register 2. The most common way is
 *    to only enable ODT# when writing to CS#. Set the
 *    switch mode to 0x2.
 *
 * 4) Program PHY RL Control Register, PHY WL Data Control
 *    Register with read/write leveling and PHY DLL Control
 *    Registerfor each data subphy.
 *
 * 5) Program DRAM Configuration Register 5 with the
 *    correct parameters for each chip-select in the system.
 *    If ODT is desired, make sure to program the RTT
 *    values.
 *
 * 6) Values for the PHY Control Registers need to be
 *    determined in the lab. Also make sure to program PHY
 *    Control Register 1 correctly to allow enough setup
 *    time when reading from DRAM.
 *
 * 7) Program Memory Address Map Register for each
 *    chip-select. This affects all future transactions to
 *    the system bus decoder. Register access should
 *    already be non-bufferable and non-cacheable, but also
 *    make sure to allow enough NOP’s for the decoder to
 *    settle.
 *
 * 8) Write 1 to User Initiated Command Register 0 to start
 *    DRAM initialization. The initialization done status
 *    can be read in DRAM Status Register.
 *
 * At this point, the DRAM is considered ready for data
 * access according to JEDEC.
 *
 * 9) Many other registers are optional, but can be used to
 *    enhance the DDR Memory Controller. All registers
 *    labeled as *Requires Mode Register update* should
 *    be done before initialization, or else they will
 *    require an extra Mode Register Set command. Other
 *    registers may be programmed before or after
 *    initialization, but as a general rule, try to program
 *    everything before initialization.
 *********************************************************/

//#include "HalDram.h"
#include "HalMc5DdrConfig.h"

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define MC5_SET_TO_ZERO         0x00000000

#define MC5_MAX_SUPPORT_CH      1
#define MC5_MAX_SUPPORT_BIT     32
#define MC5_MAX_SUPPORT_BYTE    (MC5_MAX_SUPPORT_BIT >> 3)
#define MC5_SET_ALL_BYTE        (MC5_MAX_SUPPORT_BYTE + 1)

#define MC5_CS0                 0
#define MC5_CS1                 1
#define MC5_MAX_SUPPORT_CS      2

#define MC5_NS_TO_PS            (1000)
#define MC5_US_TO_PS            (1000 * 1000)

#define USE_ACTIVE_POWER_OPTIMIZED_IF_SETTING   FALSE

/* ----------------------- PHY INIT VALUES START ----------------------- */

/*  The definition below relevant for PHY Control Registers 2, 3 and 4.
    When defined whole register will be initialized by single value
    instead of bit fields.
    Recomended to enable it for the release -
    will reduce the code footprint.
*/
#define MC5_PHY_SINGLE_VALUE_INIT   TRUE

// DQ driver strength
#define MC5_PHY_DQ_ZD               0x0 // 1 bit  [1]
#define MC5_PHY_DQ_ZNDRV            0x7 // 4 bits [23:20]
#define MC5_PHY_DQ_ZPDRV            0x7 // 4 bits [27:24]
#define MC5_PHY_DQ_MD               0x1
#define MC5_PHY_DQ_ZNR_MSB          0x1
#define MC5_PHY_DQ_ZNTRM            0x2 // 4 bits [15:12]
#define MC5_PHY_DQ_ZPR_MSB          0x1
#define MC5_PHY_DQ_ZPTRM            0x2 // 4 bits [19:16]
#define MC5_PHY_QS_VREF_SEL         0x1
#define MC5_PHY_DQ_ZNR              0x4 // 5 bits [31, 11:8]
#define MC5_PHY_DQ_ZPR              0x4 // 5 bits [30, 7:4]

// Address/Command driver strength
#define MC5_PHY_ADCM_MODE           0x1 // 1 bit  [0]
#define MC5_PHY_ADCM_RCVTYPE        0x0 // 2 bits [29:28]
#define MC5_PHY_ADCM_ZD             0x0 // 1 bit  [1]
#define MC5_PHY_ADCM_ZNR            0xA // 5 bits [31, 11:8]
#define MC5_PHY_ADCM_ZNR_MSB        0x1
#define MC5_PHY_ADCM_ZNDRV          0xF // 4 bits [23:20]
#define MC5_PHY_ADCM_ZPDRV          0xF // 4 bits [27:24]
#define MC5_PHY_ADCM_ZNDRV_667      0x7 // 4 bits [23:20]
#define MC5_PHY_ADCM_ZPDRV_667      0x7 // 4 bits [27:24]
#define MC5_PHY_ADCM_ZPR            0xA // 5 bits [30, 7:4]
#define MC5_PHY_ADCM_ZPR_MSB        0x1

// Clock driver strength
#define MC5_PHY_CK_ZD               0x1
#define MC5_PHY_CK_ZNR              0x2
#define MC5_PHY_CK_ZNR_MSB          0x1
#define MC5_PHY_CK_ZNDRV            0xF
#define MC5_PHY_CK_ZPDRV            0x7
#define MC5_PHY_CK_ZPR              0x2
#define MC5_PHY_CK_ZPR_MSB          0x1
#define MC5_PHY_DQ_RCV_DIS_LATE     0x0
#define MC5_PHY_DQ_RCVEN            0x7 // 3 bits Data receiver gain settings for Npair
#define MC5_PHY_DQ_RCVEP            0x7 // 3 bits Data receiver gain settings for Ppair

//  0x1 : uses differential receiver            (PAD TRM should be enabled)
//  0x0 : uses pseudo differential against VREF (PAD TRM should be enabled)
#define MC5_PHY_DQ_RCVTYPE          0x1 // 2 bits
#define MC5_PHY_QS_RCVTYPE          0x1 // 2 bits

//  PAD Termination (Address/Command is not available)
//  TRM (termination strength) value {ZPTRM, ZNTRM}
//  is recommended to 0x84, 0xC2, 0xE1.
//  Mode : 0x3 (disabled) / 0x2 (enabled for all reads)
//         0x1 (enabled any read/write request)
//         0x0 (always enabled)
#define MC5_PHY_DQ_TRM_MODE         0x0 // 2 bits

// Calibration
#define MC5_PHY_PAD_CAL_INTERVAL    0x1 // 0x3(64clk) / 0x2(32) / 0x1(16) / 0x0(8)
#define MC5_PHY_PAD_CAL_AUTO_SEL    0x0 // bit2(DQ/DQS), bit1(AD/CM), bit0(Clock)
                                        // This mode is not recommended
#define MC5_PHY_PAD_CAL_AUTO_EN     0x0

#define MC5_PHY_WRITE_DQSB_EN       0x1 // Fixed
#define MC5_PHY_DQSN_PULLUP         0x0 // DQSN pull-up
#define MC5_PHY_DQSP_PULLDOWN       0x0 // DQSP pull-down
#define MC5_PHY_DQ_PULLDOWN         0x0
#define MC5_PHY_CK_PULLDOWN         0x0
#define MC5_PHY_ADCM_PULLDOWN       0x1

// Delay and DLL configuration
// Negative edge dll phase
#define MC5_PHY_DLL_NPHASE          0x15 // 6bits 0x20 (90degree)
#define MC5_PHY_DLL_NPHASE_LPDDR    0x11

// Positive edge dll phase
#define MC5_PHY_DLL_PPHASE          0x15 // 6bits 0x20 (90degree)
#define MC5_PHY_DLL_PPHASE_LPDDR    0x11

//> END   : PHY Configurations

// PHY Control

/* PHY Control 1
   [18:17] Extend the read FIFO reset duty cycle
   [15]    If [14] is enabled, PHY FIFO reset will come 1 cycle earlier
   [14]    Enable automatically resetting the read FIFO wr ptr
   [7:4]   Sets when to begin reading data from the PHY FIFO,
           relative to read_leveling_window
   [3:0]   Read data round-trip off-chip delay */
#define MC5_PHY_RFIFO_RESET_EARLY       0   // 1 bit
#define MC5_PHY_RFIFO_RESET_EN          1   // 1 bit
#define MC5_PHY_RFIFO_RD_PTR_DLY        3   // 4 bits
#define MC5_PHY_RFIFO_RD_PTR_DLY_LPDDR  4   // 4 bits
#define MC5_PHY_DQ_OFFCHIP_DLY_LPDDR    0   // 4 bits
#if (USE_ACTIVE_POWER_OPTIMIZED_IF_SETTING == FEATURE_SUPPORTED)
#define MC5_PHY_DQ_OFFCHIP_DLY          2   // 4 bits
#else
#define MC5_PHY_DQ_OFFCHIP_DLY          1   // 4 bits
#endif

#if (USE_ACTIVE_POWER_OPTIMIZED_IF_SETTING == FEATURE_SUPPORTED)
/*
#define MC5_CHAN0_PHY_CTRL1_DDR3    0x00004032
#define MC5_CHAN0_PHY_CTRL2_DDR3    0xD7722449
#define MC5_CHAN0_PHY_CTRL3_800     0xCFF00AA9
#define MC5_CHAN0_PHY_CTRL3_667     0xC7700AA9
*/
#else
#define MC5_CHAN0_PHY_CTRL1_DDR3    0x00004031
#define MC5_CHAN0_PHY_CTRL2_DDR3    0xD7722441
#define MC5_CHAN0_PHY_CTRL3_667     0xC7700AA1
#define MC5_CHAN0_PHY_CTRL3_800     0xCFF00AA1
    #if ((TBG_A_FREQ_MHZ / 2) < MC5_FREQ_800_MHZ)
    #define MC5_CHAN0_PHY_CTRL3_DDR3    MC5_CHAN0_PHY_CTRL3_667
    #else
    #define MC5_CHAN0_PHY_CTRL3_DDR3    MC5_CHAN0_PHY_CTRL3_800
    #endif
#endif

#define MC5_CHAN0_PHY_CTRL1_LPDDR   0x00004040
#define MC5_CHAN0_PHY_CTRL2_LPDDR   0xC77C2A03

// KevinKCChiu: (SDK6.0) 'AA' should be reserved part
// KevinKCChiu: (SDK7.0) still keep 'AA'
#if (MC5_USE_SDK_7_SETTING == MC5_ENABLED)
#define MC5_CHAN0_PHY_CTRL3_LPDDR   0xD77AA773
#else
#define MC5_CHAN0_PHY_CTRL3_LPDDR   0xD7700773
#endif

#define MC5_CHAN0_PHY_CTRL1_DDR4    0x00004032
#define MC5_CHAN0_PHY_CTRL2_DDR4    0xD7722551
#define MC5_CHAN0_PHY_CTRL3_DDR4    0xCFF00FF9

//#define MC5_CHAN0_PHY_CTRL4_DDR3    0x3F21DF22

/* KevinKCChiu: PHY_DQ_RCVTYPE[23:22] = 1
                PHY_QS_RCVTYPE[31:30] = 1
   this is SDK5.0 setting
*/
#define MC5_CHAN0_PHY_CTRL4_DDR3    0x7F61DF22

#define MC5_CHAN0_PHY_CTRL4_LPDDR   0x3F03FF77
#define MC5_CHAN0_PHY_CTRL4_DDR4    0x7F63DF22

#define MC5_CHAN0_PHY_CTRL5_DDR3    0x0010011C
#define MC5_CHAN0_PHY_CTRL5_LPDDR   0x000001D8
#define MC5_CHAN0_PHY_CTRL5_DDR4    0x00100118

#define MC5_CHAN0_PHY_CTRL6_DDR3    0x00000000
#define MC5_CHAN0_PHY_CTRL6_LPDDR   0x00300008
#define MC5_CHAN0_PHY_CTRL6_DDR4    0x00080200

#define MC5_CHAN0_PHY_CTRL8         0x90118010

#define MC5_CHAN0_PHY_CTRL9         0xC0000000
#define MC5_PHY_CTRL9_DLL_RST       0x20000000
#define MC5_PHY_CTRL9_UPD_EN_PULSE  0x40000000
#define MC5_PHY_CTRL9_SYNC_EN       0x80000000

/* ----------------------- PHY INIT VALUES END ----------------------- */

//-----------------------------------------------------------------------------
//  constant definitions - MC5 register:
//-----------------------------------------------------------------------------

#if (USE_ACTIVE_POWER_OPTIMIZED_IF_SETTING == FEATURE_SUPPORTED)
/*
#define ACTIVE_POWER_OPTIMIZED_COEF (isLpddr ? 0 : 17) // 933/800 = 1.166 ~ +17% for DDR3 only
*/
#else
#define ACTIVE_POWER_OPTIMIZED_COEF 0
#endif

#if (MC5_USE_SDK_7_SETTING == MC5_ENABLED)
#define MC5_RRB_STARVATION_PREVENTION_0     0x66666666
#else
#define MC5_RRB_STARVATION_PREVENTION_0     0x00666666
#endif

#define MC5_RRB_STARVATION_PREVENTION_1     0x66666666

#define MC5_CHAN0_ZQC0          0x00000200
#define MC5_CHAN0_ZQC1          0x00400100
#define MC5_CHAN0_ZQC0_LPDDR    0x00280320
#define MC5_CHAN0_ZQC1_LPDDR    0x00480120

// Default value for ba boundary - 4Gb x 2
#if (MC5_DDR_TYPE == MC5_DDR_TYPE_LPDDR3)
#define MC5_CHAN0_MEM_CTRL_PHYS_ADDR_MAP    0x13000432
#else
#define MC5_CHAN0_MEM_CTRL_PHYS_ADDR_MAP    0x13000532
#endif
//#define MC5_CHAN0_MEM_CTRL_PHYS_ADDR_MAP    0x13000532

// Channel 0 DRAM Control 3 - LPDDR only
#if (MC5_USE_SDK_7_SETTING == MC5_ENABLED)
#define MC5_CHAN0_DRAM_CTRL3    0x00000000 // no ODT delays
#else
#define MC5_CHAN0_DRAM_CTRL3    0x00003C23
#endif

#if (MC5_LPDDR_PER_BANK_MODE == MC5_ENABLED)
#define MC5_CHAN0_MEM_CTRL1_LPDDR   0x0000E800
#else
#define MC5_CHAN0_MEM_CTRL1_LPDDR   0x0000E000
#endif

#define MC5_CHAN0_MEM_CTRL1_RCVR    0x00000008
#define MC5_CHAN0_MEM_CTRL2_LPDDR   0x00000200

// Enable Exclusive Access Monitoring for Masters 0, 1 and 2
#define MC5_DDR_MC_EXCLUSIV_MONITOR_CTRL_DDR3   0x00007000
#define MC5_DDR_MC_EXCLUSIV_MONITOR_CTRL_LPDDR  0x000FF000
#define MC5_DDR_MC_EXCLUSIV_MONITOR_CTRL_DDR4   0x000FF1FC

#define MC5_DDR_MC_USER_INIT_CMD0_RCVR          0x10000001
#define MC5_DDR_MC_USER_INITIATE_CMD0           0x13000001
#define MC5_DDR_MC_USER_INITIATE_CMD1           0x13000000
#define MC5_DDR_MC_USER_INITIATE_CMD1_LPDDR     0x13020000
#define MC5_MEM_CONTROLL_POWER_CTRL             0x00000463
#define MC5_MEM_CONTROLL_POWER_CTRL_LPDDR       0x00000440
#define MC5_DDR_MC_TST_MODE1                    0x01300000

// ODT configuration for LPDDR3
#define MC5_CHAN0_DRAM_CFG5_CS0     0x00210000 // ODS_CSO = 1; RTT_NOM_CS0 = 2;
#define MC5_CHAN0_DRAM_CFG5_CS1     0x00210000 // ODS_CS1 = 1; RTT_NOM_CS1 = 2;
#define MC5_CHAN0_DRAM_CTRL1_1CS    0x00000010 // ODT0 CS0 read enabled
#define MC5_CHAN0_DRAM_CTRL1_2CS    0x00000030 // ODT0 CS0 and CS1 read enabled
#if (USE_ACTIVE_POWER_OPTIMIZED_IF_SETTING == FEATURE_SUPPORTED)
/*
#define MC5_CHAN0_DRAM_CTRL2_1CS    0x02000002 // ODT0 enabled
#define MC5_CHAN0_DRAM_CTRL2_2CS    0x0200000A // ODT0 and ODT1 enabled
*/
#else
#define MC5_CHAN0_DRAM_CTRL2_1CS    0x00000002 // ODT0 enabled
#define MC5_CHAN0_DRAM_CTRL2_2CS    0x0000000A // ODT0 and ODT1 enabled
#endif

// ---SDK7.0---
// ODT configuration for DDR3/DDR4
#define MC5_CHAN0_DRAM_CFG5_CS0_DDR     0x02210000
#define MC5_CHAN0_DRAM_CFG5_CS1_DDR     0x02210000
#define MC5_CHAN0_DRAM_CTRL1_DDR        0x00000101
#define MC5_CHAN0_DRAM_CTRL2_DDR        0x0200000A

// ODT configuration for LPDDR3
#define MC5_CHAN0_DRAM_CFG5_CS0_LPDDR   0x00020000
#define MC5_CHAN0_DRAM_CFG5_CS1_LPDDR   0x00020000
#define MC5_CHAN0_DRAM_CTRL1_LPDDR      0x00000000
#define MC5_CHAN0_DRAM_CTRL2_LPDDR      0x02000000
// ---SDK7.0---

// When Temperature Derating taken in account 1.875ns will be added to
// tRC, tRCD, tRAS, tRP, tRRD for LPDDR
#if (MC5_TEMP_DERATING == MC5_ENABLED)
#define MC5_TEMP_DERATING_VAL   1875
#else
#define MC5_TEMP_DERATING_VAL   0
#endif

// PadCalibration
#define PAD_CAL_DEFAULT_OFFSET  0x0
#define PAD_CAL_MAX_VALUE       0x1F
#define PAD_CAL_MID_VALUE       0x10
#define PAD_CAL_MAX_WAITTIME    0x1E
#define PAD_CAL_PASS_THRESHOLD  0x5
#define PAD_CAL_TAG_VAILD       0x4C435244  //DRCL
#define PAD_CAL_TAG_FAIL        0x4C494146  //FACL

#if (MC5_DDR_TYPE == MC5_DDR_TYPE_DDR4)
#define PAD_CAL_LSB_DEFAULT         0x7
#define PAD_CAL_MSB_DEFAULT         0x1
#define PAD_CAL_DEFAULT_INTERVAL    0x2
#endif

//-----------------------------------------------------------------------------
//  MACROs
//-----------------------------------------------------------------------------
#define MC5_READ(offset)                *(volatile U32*)(offset)
#define MC5_WRITE(offset, value)        *(volatile U32*)(offset) = value

#define MC5_READ_16(offset)             *(volatile U16*)(offset)
#define MC5_WRITE_16(offset, value)     *(volatile U16*)(offset) = value

#define MC5_REG_READ(offset)            *(volatile U32*)(offset + MC5_BASE)
#define MC5_REG_WRITE(offset, value)    *(volatile U32*)(offset + MC5_BASE) = value

// Convert data rate to frequency and frequency to data rate
#define MC5_DR_TO_FREQ(dr)              (dr   % 2  == 0) ? (dr   / 2) : (dr   / 2) + 1
#define MC5_FREQ_TO_DR(freq)            (freq % 10 != 7) ? (freq * 2) : (freq * 2) - 1

// Add active power optimized coefficient
#define MC5_ADD_ACTIVE_PW_OPT_COEF(val) (val + (val / 100) * ACTIVE_POWER_OPTIMIZED_COEF)

// Convert frequency in MHZ to nanoseconds
#define MC5_MHZ_TO_NS(freq)             (1000 / freq)

// Convert frequency in MHZ to picoSec
#define MC5_MHZ_TO_PS(freq)             (1000000 / freq)

// Convert picoseconds to nanoseconds
#define MC5_PS_TO_NS(ps)                (ps / 1000)

// Convert nCK (number of tCK cycles) to nanoSec by frequency in Mhz
#define MC5_NCK_TO_NS(freq, nck)        (MC5_MHZ_TO_PS(freq) * nck) / 1000

// Convert nCK (number of tCK cycles) to picoSec by frequency in Mhz
#define MC5_NCK_TO_PS(freq, nck)        (MC5_MHZ_TO_PS(freq) * nck)

// Round up to the whole number
#define MC5_ROUND_UP(a, b)              (a % b == 0) ? (a / b) : (a / b + 1)

// Convert picoSec to nCK (number of tCK cycles) by frequency in Mhz
#define MC5_PS_TO_NCK(freq, ps)         MC5_ROUND_UP(ps, MC5_MHZ_TO_PS(freq))

// Convert picoSec to nCK (number of tCK cycles) by frequency in Mhz
// with active power optimized coefficient
#define MC5_PS_TO_NCK_WITH_COEF(freq, ps) \
        MC5_ROUND_UP(MC5_ADD_ACTIVE_PW_OPT_COEF(ps), MC5_MHZ_TO_PS(freq))

// Add active power optimized coefficient to value in nCK
#define MC5_NCK_ADD_COEF(freq, nck) \
        MC5_ROUND_UP(MC5_ADD_ACTIVE_PW_OPT_COEF(MC5_NCK_TO_PS(freq, nck)), MC5_MHZ_TO_PS(freq))

// Get maximum delay value in picoSec between
// nCK and given delay in picoSec
#define MC5_GET_MAX_NCK_PS(freq,nck,ps) \
        (MC5_NCK_TO_PS(freq, nck) > ps) ? MC5_NCK_TO_PS(freq, nck) : ps

// Get maximum delay value between nCK and delay in picoSec
// given by Mc5NckNs_t structure and convert to nanoSec
#define MC5_GET_MAX_NCK_PS_TO_NS(freq, mc5NckNs)                \
            (MC5_NCK_TO_PS(freq, mc5NckNs.nck) > mc5NckNs.ps) ? \
            MC5_NCK_TO_PS(freq, mc5NckNs.nck)/1000 :            \
            mc5NckNs.ps/1000

// Get maximum delay value between nCK and delay in picoSec
// given by Mc5NckNs_t structure and convert to nCK
#define MC5_GET_MAX_NCK_PS_TO_NCK(freq, mc5NckNs)               \
            (MC5_NCK_TO_PS(freq, mc5NckNs.nck) > mc5NckNs.ps) ? \
            mc5NckNs.nck : MC5_PS_TO_NCK(freq, mc5NckNs.ps)

// Get maximum delay value between nCK and delay in picoSec
// given by Mc5NckNs_t structure and convert to nCK
// with active power optimized coefficient
#define MC5_GET_MAX_NCK_PS_TO_NCK_WITH_COEF(freq, mc5NckNs)     \
            (MC5_NCK_TO_PS(freq, mc5NckNs.nck) > mc5NckNs.ps) ? \
            MC5_NCK_ADD_COEF(freq, mc5NckNs.nck) :              \
            MC5_PS_TO_NCK_WITH_COEF(freq, mc5NckNs.ps)


#define MC5_SIMPLE_PATTERN_TEST_16(address, pattern, delay)  \
            {MC5_WRITE_16(address, pattern);                 \
            HalTimer_DelayUsec(delay);                       \
            if (MC5_READ_16(address) != pattern)             \
            {return FALSE;}}

#define MC5_SIMPLE_PATTERN_TEST_32(address, pattern, delay)  \
            {MC5_WRITE(address, pattern);                    \
            HalTimer_DelayUsec(delay);                       \
            if (MC5_READ(address) != pattern)                \
            {return FALSE;}}

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief DRAM type definition
typedef enum DramType_t
{
    cDramNone   = 0x00,
    cDramDdr0   = 0x10,
    cDramDdr1   = 0x11,
    cDramDdr2   = 0x12,
    cDramDdr3   = 0x13,
    cDramDdr4   = 0x14,
    cDramLpDdr1 = 0x21,
    cDramLpDdr2 = 0x22,
    cDramLpDdr3 = 0x23
} DramType_t;

typedef enum DramCategory_t
{
    cDramCategoryDdr   = 0,
    cDramCategoryLpDdr = 1,
    cDramCategoryCount = 2
} DramCategory_t;

typedef enum Mc5DivisorValues_t
{
    cValNa      = 0, // NA - not available
    cValOff     = 0,
    cVal60_Ohm  = 1,
    cVal120_Ohm = 2,
    cVal40_Ohm  = 3,
    cVal240_Ohm = 4,
    cVal48_Ohm  = 5,
    cVal80_Ohm  = 6,
    cVal34_Ohm  = 7,
    cVal30_Ohm  = 11,
    cVal27_Ohm  = 15
} Mc5DivisorValues_t;

typedef enum Mc5OdtOperation_t
{
    cWriteToCs0  = 0,
    cWriteToCs1  = 1,
    cReadFromCs0 = 2,
    cReadFromCs1 = 3
} Mc5OdtOperation_t;

typedef enum Mc5StoreRestoreReg_t
{
    cStoreReg   = 0,
    cRestoreReg = 1
} Mc5StoreRestoreReg_t;

typedef enum Mc5HwInfo_t
{
    cMc5HwInfoDll = 0,
    cMc5HwInfoCal,
    cMc5HwInfoCnt
} Mc5HwInfo_t;

typedef enum Mc5SpeedBinIndex_t
{
    cIndex1066 = 0,
    cIndex1333 = 1,
    cIndex1600 = 2,
    cIndex1866 = 3,
    cIndex2133 = 4
} Mc5SpeedBinIndex_t;

// PHY Read Leveling Control
typedef struct Mc5PhyRLCtrl_t   // size opt.
{
    U8  phyRLCycleDelay;    // PHY Read Leveling Cycle Delay
    U8  phyRLTapDelay;      // PHY Read Leveling TAP   Delay
} Mc5PhyRLCtrl_t;

// PHY Write Leveling Control
typedef struct Mc5PhyWLCtrl_t   // size opt.
{
    U8  phyWLWckDqDelay;    // PHY Write Leveling WCK DQ  Delay
    U8  phyWLWckQsDelay;    // PHY Write Leveling WCK QS  Delay
    U8  phyWLWckDqcDelay;   // PHY Write Leveling WCK DQC Delay
    U8  phyWLWckQscDelay;   // PHY Write Leveling WCK QSC Delay
} Mc5PhyWLCtrl_t;

typedef struct Mc5PhyCtrlRegVal_t
{
    U32 phyCtrl1Val;        // rDdrMc.chan0PhyCtrl1
    U32 phyCtrl2Val;        // rDdrMc.chan0PhyCtrl2
    U32 phyCtrl3Val;        // rDdrMc.chan0PhyCtrl3
    U32 phyCtrl4Val;        // rDdrMc.chan0PhyCtrl4
    U32 phyCtrl5Val;        // rDdrMc.chan0PhyCtrl5
    U32 phyCtrl6Val;        // rDdrMc.chan0PhyCtrl6
} Mc5PhyCtrlRegVal_t;

typedef struct Mc5MemSizeConfig_t
{
    U32 chan0MmapCs0Config;
    U32 chan0MmapCs1Config;
    U32 size;
} Mc5MemSizeConfig_t;

/* This structure used for storage of
   ODT parameters for use after recovery*/
typedef struct Mc5RegForRecoveryOdt_t
{
    U32 chan0DramCfg5Cs0;
    U32 chan0DramCfg5Cs1;
    U32 chan0DramCtrl1;
    U32 chan0DramCtrl2;
} Mc5RegForRecoveryOdt_t;

/* This structure used for storage of
   memory size parameters for use after recovery*/
typedef struct Mc5RegForRecoverySize_t
{
    U32 chan0MemControllPhysicalAddrMapCs0;
    U32 chan0MemControllPhysicalAddrMapCs1;
    U32 chan0MmapCs0;
    U32 chan0MmapCs1;
} Mc5RegForRecoverySize_t;

typedef struct Mc5RegForRecoveryJedec_t
{
    U32 chan0Zqc0;
    U32 chan0Zqc1;
    U32 chan0RefreshTm;
    U32 chan0SelfRefreshTm;
    U32 chan0PowerDnTm;
    U32 chan0ModeRegisterSetTm;
    U32 chan0ActivateTm;
    U32 chan0PreChargeTm;
    U32 chan0CasRasTm;
    U32 chan0OffSpecTm;
    U32 chan0DramRdTm;
    U32 chan0DramTrainTm;
    U32 chan0DramCfg1;
} Mc5RegForRecoveryJedec_t;

// Bank address assignment boundary
typedef struct Mc5BlckAddrBoundary_t
{
    U32 singleDeviceDensity;
    U16 numOfRows;              // size opt.
    U16 blckAddrBoundary;       // size opt.
} Mc5BlckAddrBoundary_t;

//-----------------------------------------------------------------------------
//  Data type definitions: JEDEC section
//-----------------------------------------------------------------------------
// tRFC values by device density - from JEDEC
typedef struct Mc5TrfcValues_t
{
    U32 deviceDensity;      // Device Density
    U32 trfcValDdr_ps;      // DDR3     tRFC in picoSec
    U32 trfcValLpddr_ps;    // LPDDR2/3 tRFC in picoSec
    U32 trfcValLpddrPb_ps;  // LPDDR2/3 tRFC in PerBank refresh mode in picoSec
} Mc5TrfcValues_t;

// Counter values for DDR and LPDDR - in pico seconds
typedef struct Mc5CountValPs_t
{
    U32 initNop[cDramCategoryCount];
    U32 coldReset[cDramCategoryCount];
    U32 warmReset[cDramCategoryCount];
} Mc5CountValPs_t;

/* Used for store timing parameters value because
   it usually given in JEDEC in format "max(4nCK, 7.5ns)"
   In order to avoid usage of "float"
   ps (picoSec) used instead of ns (nanoSec) */
typedef struct Mc5NckPs_t
{
    U32 nck;                // Value in nCK
    U32 ps;                 // Value in picoSec - in order to avoid "float" type
} Mc5NckPs_t;

/* JEDEC parameters -
   fields names taken from corresponding structures in RegDdrMc.h
   where it set according to "88SS1093 R2.1 Registers" document.
   Parameters of type U32 with suffix "ps" given in picoSec,
   with "nck" given in nCK. */
typedef struct Mc5JedecParams_t
{
    // Self-Refresh Timing
    U32         T_XSRD_nck                          ;  // Exit Self-refresh Locked DLL Timing.
    Mc5NckPs_t  T_CKSRX                             ;  // Stable TCK Cycles Before Exiting Self-refresh. -
    Mc5NckPs_t  T_XSNR                              ;  // Exit Self-Refresh No DLL Timing.
    Mc5NckPs_t  T_CKSRE                             ;  // TCK Before Clock Change After Self-refresh.

    // Power-Down Timing
    Mc5NckPs_t  T_XARDS                             ;  // XARDS Timing.
    Mc5NckPs_t  T_XP[MC5_FREQ_NUM_OF_OPTIONS]       ;  // Exit Power-Down Timing - DDR3.
    Mc5NckPs_t  T_XP_4                              ;  // Exit Power-Down Timing - DDR4.
    Mc5NckPs_t  T_XP_L                              ;  // Exit Power-Down Timing - LPDDR.
    Mc5NckPs_t  T_CKE[MC5_FREQ_NUM_OF_OPTIONS]      ;  // CKE minimum pulse width - used for calculation of T_CKESR - DDR.
    Mc5NckPs_t  T_CKE_L                             ;  // CKE minimum pulse width - used for calculation of T_CKESR - LPDDR.
    U32         T_CPDED_nck[MC5_FREQ_NUM_OF_OPTIONS];  // SDRAM Command Path Disable Delay Timing.
    U32         T_PDEN_nck[MC5_FREQ_NUM_OF_OPTIONS] ;  // Power-Down Entry Delay Timing.

    // Mode Register Set Timing
    U32         T_MRD_nck                           ;  // Mode Register Set Command Cycle Time - DDR3.
    U32         T_MRD_4_nck                         ;  // Mode Register Set Command Cycle Time - DDR4.
    Mc5NckPs_t  T_MRD_L                             ;  // Mode Register Set Command Cycle Time - LPDDR.
    Mc5NckPs_t  T_MOD                               ;  // Mode Timing.
    Mc5NckPs_t  T_MOD_4                             ;  // Mode Timing - DDR4.

    // Activate Timing
    U32         T_FAW_ps[MC5_FREQ_NUM_OF_OPTIONS]   ;  // FAW Timing - DDR3.
    U32         T_FAW_4_ps                          ;  // FAW Timing - DDR4.
    U32         T_FAW_L                             ;  // FAW Timing - LPDDR.

    // Pre-charge Timing
    Mc5NckPs_t  T_RTP                               ;  // Read-to-Precharge Timing.
    U32         T_WR_ps                             ;  // Write Recovery Timing (Effective Value in DRAM Clock) - DDR.
    Mc5NckPs_t  T_WR_L                              ;  // Write Recovery Timing (Effective Value in DRAM Clock) - LPDDR.

    // CAS/RAS Timing
    Mc5NckPs_t  T_WTR                               ;  // Write-to-Read Delay Timing (Same Group).
    U32         T_CCD_nck                           ;  // CAS to CAS Command Delay (Same Group).
    Mc5NckPs_t  T_RRD[MC5_FREQ_NUM_OF_OPTIONS]      ;  // Bank-to-Bank Command Period Timing (Same Group) - DDR.
    Mc5NckPs_t  T_RRD_L                             ;  // Bank-to-Bank Command Period Timing (Same Group) - LPDDR.

    // DRAM Read Timing
    U32         T_DQSCK_ps                          ;  // tDQSCK  - for LPDDR only.

    // DRAM Train Timing
    U32         T_CACKEL_nck                        ;  // tCACKEL - for LPDDR only.
    U32         T_CAEXT_nck                         ;  // tCAEXT  - for LPDDR only.

    // ZQ Calibration Timing
    Mc5NckPs_t  T_ZQINIT                            ;  // tZQINIT - ZQ Power-up and Reset Calibration Time - DDR3
    U32         T_ZQINIT_4_nck                      ;  // tZQINIT - ZQ Power-up and Reset Calibration Time - DDR4
    U32         T_ZQINIT_L_ps                       ;  // tZQINIT - ZQ Power-up and Reset Calibration Time - LPDDR

    Mc5NckPs_t  T_ZQOPER                            ;  // tZQCL - ZQ Normal Operation Full Calibration Time - DDR3
    U32         T_ZQOPER_4_nck                      ;  // tZQCL - ZQ Normal Operation Full Calibration Time - DDR4
    U32         T_ZQOPER_L_ps                       ;  // tZQCL - ZQ Normal Operation Full Calibration Time - LPDDR

    Mc5NckPs_t  T_ZQCS                              ;  // tZQCS - ZQ Normal Operation Short Calibration Time - DDR3
    U32         T_ZQCS_4_nck                        ;  // tZQCS - ZQ Normal Operation Short Calibration Time - DDR4
    U32         T_ZQCS_L_ps                         ;  // tZQCS - ZQ Normal Operation Short Calibration Time - LPDDR

    Mc5NckPs_t  T_ZQCR                              ;  // tZQCR - ZQ Calibration Reset Time - LPDDR only

    U32         T_WTR_S_nck[MC5_FREQ_NUM_OF_OPTIONS];  // Write-to-Read Delay Timing (Different Group)         - DDR4 only.
    U32         T_CCD_S_nck                         ;  // CAS to CAS Command Delay (Different Group)           - DDR4 only.
    Mc5NckPs_t  T_RRD_S[MC5_FREQ_NUM_OF_OPTIONS]    ;  // Bank-to-Bank Command Period Timing (Different Group) - DDR4 only.
} Mc5JedecParams_t;

typedef struct Mc5SpeedBin_t
{
    U32 speedBin            ; // Speed Bin from 1033 to 2133
    U32 clNrcdNrp           ; // CL-nRCD-nRP value, for example 11-11-11
} Mc5SpeedBin_t;

typedef struct Mc5ClTimingParams_t  // size opt.
{
    U8  CAS_LATENCY         ; // CAS Latency Value.
    U8  CWL                 ; // CAS Write Latency.
} Mc5ClTimingParams_t;

typedef struct Mc5MemoryDevice_t
{
    Mc5SpeedBin_t deviceData; // Speed Bin and CL-nRCD-nRP data
    U32 T_RAS               ; // Active-to-Precharge Command Timing.
    U32 T_RC                ; // RC Timing.
    U32 T_RCD_RP_RPA        ; // PreCharge Command Period AND RAS-to-CAS Delay Timing.
    Mc5ClTimingParams_t clTimingParam[MC5_FREQ_NUM_OF_OPTIONS];
                              // CL and CWL parameters
} Mc5MemoryDevice_t;

typedef struct Mc5MemoryDeviceDdr4_t
{
    Mc5SpeedBin_t deviceData; // Speed Bin and CL-nRCD-nRP data
    U8  T_RAS               ; // Active-to-Precharge Command Timing.
    U8  T_RC                ; // RC Timing.
    U8  T_RCD_RP_RPA        ; // PreCharge Command Period AND RAS-to-CAS Delay Timing.
    Mc5ClTimingParams_t clTimingParam; // CL and CWL parameters
} Mc5MemoryDeviceDdr4_t;

typedef struct Mc5LpddrMemoryDevice_t
{
    Mc5NckPs_t T_RCD        ; // RAS-to-CAS Delay Timing
    Mc5NckPs_t T_RP         ; // PreCharge Command Period
    Mc5NckPs_t T_RPA        ; // PreCharge for All Command Period
} Mc5LpddrMemoryDevice_t;

// Those parameters are not part of the DDR specification.
// It is a physical system requirement.
typedef struct Mc5OffSpecTimingParams_t
{
    // Off-Spec Timing
    U8  T_CCD_CCS_EXT_DLY   ; // Extra CAS-to-CAS Read Delay.
    U8  T_CCD_CCS_WR_EXT_DLY; // Extra CAS-to-CAS Write Delay.
    U8  TRWD_EXT_DLY        ; // READ to WRITE Command Delay.
    U8  TWL_EARLY           ; // Write Leveling Early.
    U8  RD_GAP_EXT          ; // Read Gap Extend.
} Mc5OffSpecTimingParams_t;

// Per clock-frequency values for PLLA Control 1 register
typedef struct Mc5PllACtrl1Values_t
{
    U32 value;
    U16 clock;          // size opt.
    U16 frequency;      // size opt.
} Mc5PllACtrl1Values_t;

// DLL parameters for PHY DLL Control Registers 0x500-0x50C
typedef struct phyDllData_t
{
    U8  dllPositive[MC5_MAX_SUPPORT_BYTE];
    U8  dllNegative[MC5_MAX_SUPPORT_BYTE];
} phyDllData_t;

typedef struct phyCalData_t
{
    U8  phyCalLsb;
    U8  phyCalMsb;
    U8  phyCalValid;
    U8  phyCalRsvd;
} phyCalData_t;

//-----------------------------------------------------------------------------
//  Data type definitions: global database
//-----------------------------------------------------------------------------
// Type of global database
typedef struct Mc5GlobalData_t
{
    Mc5MemSizeConfig_t      *memSizeConfig;     // Pointer to memory configuration
    Mc5BlckAddrBoundary_t   *boundaryTable;     // Pointer to BA boundary table
    U32                     dataWidth;          // Data bus width - 16 or 32 bit
    U32                     totalSize;          // Total memory size
    U32                     deviceDensity;      // Single device density
    U32                     cs1StartAddress;    // Physical start address of CS1
    U32                     frequencyMT;        // CPU frequency == data rate
    U32                     dramFrequencyMHz;   // Memory frequency
    DramType_t              dramType;           // DRAM type
    bool                    isLpddr;            // Is it LPDDR memory ?
    bool                    isCs1Enabled;       // Is CS1 enabled ?
} Mc5GlobalData_t;

typedef struct Mc5HwInfoData_t
{
    U8              dataValid[cMc5HwInfoCnt];
    phyDllData_t    phyDllData;
    phyCalData_t    phyCalData;

} Mc5HwInfoData_t;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t HalMc5_OneTimeInit(U32 frequency);
Error_t HalMc5_Recovery   (U32 frequency);
Error_t HalMc5_DllResetAndUpdate(void);
Error_t HalMc5_PadCalibrationRoutine(U8 targetLoop);

/* End of File */


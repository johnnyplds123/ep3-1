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
//! @brief NAND Access High Level Driver API definition
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
/// @name NAND flash time out timing (usec unit) - good for all NAND flash
/// @{
#define NF_RESET_TIME_OUT_US            1000    ///< NAND flash reset timeout
#define NF_READ_TIME_OUT_US             10000   ///< NAND flash page read timeout
#define NF_WRITE_TIME_OUT_US            1000000 ///< NAND flash page write timeout
#define NF_ERASE_TIME_OUT_US            2000000 ///< NAND flash block erase timeout
/// @}

/// @name NAND flash timing (usec unit) - good for all NAND flash
/// @{
#define NF_tADL_USEC                    4       ///< ALE to data loading time
#define NF_tRST_USEC                    5000    ///< Device reset time
#define NF_tR_USEC                      200     ///< Data transfer from Cell to Register
#define NF_tWHR_USEC                    2       ///< WE_n high to RE_n low
#define NF_tFEAT_USEC                   10      ///< Busy time for Set Features and Get Features
#define NF_tRP_USEC                     1       ///< RE_n/RE_t low level width
#define NF_tWP_USEC                     1       ///< WE_n pulse width
#define NF_tDQSRE_USEC                  1       ///< RE_n to DQS delay time
/// @}

/// @name NAND Interface type for JEDEC
/// @{
#define NF_JEDEC_IF_TOGGLE              0x00    ///< Toggle mode
#define NF_JEDEC_IF_LEGACY              0x01    ///< Legacy mode
/// @}

/// @name NAND Flash command codes
/// @{
#define FC_RESET                        0xFF    ///< reset command
#define FC_SYNC_RESET                   0xFC    ///< synchronous reset command
#define FC_RESET_LUN                    0xFA    ///< reset LUN command
#define FC_READ_ID                      0x90    ///< read id command
#define FC_ID_ADDR_MANFUACTURER         0x00    ///<    Manufacturer id address
#define FC_ID_ADDR_ONFI                 0x20    ///<    ONFI signature id address
#define FC_ID_ADDR_JEDEC                0x40    ///<    JEDECT signature id address
#define FC_READ_STATUS                  0x70    ///< read status
#define FC_READ_1ST                     0x00    ///< read, 1st cycle, etc.
#define FC_READ_2ND                     0x30    ///< read, 2nd cycle
#define FC_READ_MP_JEDEC                0x32    ///< read, Multi plane
#define FC_READ_CACHE_END_JEDEC         0x3F    ///< read cache end
#define FC_RND_DO_1ST                   0x05    ///< random data output 1st cycle
#define FC_RND_DO_2ND                   0xE0    ///< random data output 2nd cycle
#define FC_PROGRAM_1ST                  0x80    ///< program 1st cycle
#define FC_PROGRAM_2ND                  0x10    ///< program 2nd cycle
#define FC_PROGRAM_MP_JEDEC             0x11    ///< program, Multi plane
#define FC_RND_DI                       0x85    ///< random data input
#define FC_ERASE_FLASH_1ST              0x60    ///< erase 1st cycle
#define FC_ERASE_FLASH_2ND              0xD0    ///< erase 2nd cycle
#define FC_READ_PARAMETER_PAGE          0xEC    ///< read parameter page
#define FC_PAGE_ADDR_ONFI               0x00    ///<    ONFI parameter page address
#define FC_PAGE_ADDR_JEDEC              0x40    ///<    JEDEC parameter page address
#define FC_GET_FEATURES                 0xEE    ///< get feature
#define FC_SET_FEATURES                 0xEF    ///< set feature
#define FC_GET_FEATURES_LUN             0xD4    ///< get feature by lun
#define FC_SET_FEATURES_LUN             0xD5    ///< set feature by lun
#define FC_FEATURE_ADDR_TIMING          0x01    ///<    Timing mode
#define FC_FEATURE_ADDR_NVDDR2_TOGGLE2  0x02    ///<    NV-DDR2/Toggle2 configuration
#define FC_FEATURE_ADDR_IF_CHANGE       0x80    ///<    SDR/Toggle configuration
#define FC_FEATURE_ADDR_DRIVE_STRENGTH  0x10    ///<    Drive strength
#define FC_FEATURE_ADDR_EXT_VPP         0x30    ///<    External Vpp configuration
#define FC_FEATURE_ADDR_EZ_NAND         0x50    ///<    EZ NAND control
#define FC_FEATURE_ADDR_VOLUME          0x58    ///<    Volume Configuration
#define FC_FEATURE_ADDR_BA_NAND_ERROR   0x60    ///<    BA NAND Error information
#define FC_FEATURE_ADDR_BA_NAND         0x61    ///<    BA NAND Configuration

#define FC_READ_BYTE0                   0x00
#define FC_READ_BYTE1                   0x30
#define FC_READ_BYTE1_CACHE             0x31
#define FC_READ_BYTE2                   0x00
#define FC_READ_BYTE3_ONFI              0x00
#define FC_READ_BYTE3_JEDEC             0x60
#define FC_READ_BYTE4_ONFI              0x06
#define FC_READ_BYTE4_JEDEC             0x05
#define FC_READ_BYTE5                   0xE0
#define FC_READ_BYTE6                   0x35
#define FC_READ_BYTE7_ONFI              0x32    ///< ONFI only
#define FC_READ_BYTE8                   0x05
#define FC_PROG_BYTE0                   0x80
#define FC_PROG_BYTE1                   0x10
#define FC_PROG_BYTE2                   0x11
#define FC_PROG_BYTE3_ONFI              0x80
#define FC_PROG_BYTE3_JEDEC             0x81
#define FC_PROG_BYTE4                   0x85
#define FC_PROG_BYTE5                   0x1A
#define FC_ERASE_BYTE0                  0x60
#define FC_ERASE_BYTE1                  0xD0
#define FC_ERASE_BYTE2_ONFI             0xD1    ///< ONFI only
#define FC_READ_STATUS_BYTE0            0x70
#define FC_READ_STATUS_BYTE1            0x78    ///< ONFI only
#define FC_READ_STATUS_BYTE2            0xF1    ///< JEDEC only
#define FC_READ_STATUS_BYTE3            0xF2    ///< JEDEC only
#define FC_STATUS_READY_BIT0            6       ///< READ_STATUS command
#define FC_STATUS_FAIL_BIT0             0       ///< READ_STATUS command
#define FC_STATUS_READY_BIT1            6       ///< ONFI only, READ_STATUS_ENHANCED command
#define FC_STATUS_FAIL_BIT1             0       ///< ONFI only, READ_STATUS_ENHANCED command
#define FC_STATUS_READY_BIT2            6       ///< JEDEC only, F1_READ_STATUS/F2_READ_STATUS command
#define FC_STATUS_FAIL_BIT2             0       ///< JEDEC only, F1_READ_STATUS/F2_READ_STATUS command
#define FC_STATUS_FAIL_BIT3             1       ///< JEDEC only, F1_READ_STATUS/F2_READ_STATUS command
#define FC_STATUS_FAIL_BIT4             2       ///< JEDEC only, F1_READ_STATUS/F2_READ_STATUS command
#define FC_STATUS_FAIL_BIT5             3       ///< JEDEC only, F1_READ_STATUS/F2_READ_STATUS command
#define FC_STATUS_FAIL_BIT6             4       ///< JEDEC only, F1_READ_STATUS/F2_READ_STATUS command
#define FC_READ_STATUS_BYTE             FC_READ_STATUS_BYTE0
#define FC_STATUS_READY_BIT             FC_STATUS_READY_BIT0
#define FC_STATUS_FAIL_BYTE             (1 << FC_STATUS_FAIL_BIT0)
#define FC_READ_STATUS_ENH_BYTE_ONFI    FC_READ_STATUS_BYTE1
#define FC_READ_STATUS_ENH_BYTE_JEDEC(LUN) (LUN == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3
#define FC_STATUS_ENH_READY_BIT_ONFI    FC_STATUS_READY_BIT1
#define FC_STATUS_ENH_READY_BIT_JEDEC   FC_STATUS_READY_BIT2
#define FC_STATUS_ENH_FAIL_BYTE_ONFI    (1 << FC_STATUS_FAIL_BIT1)
#define FC_STATUS_ENH_FAIL_BYTE_JEDEC   ((1 << FC_STATUS_FAIL_BIT2) | (1 << FC_STATUS_FAIL_BIT3) | (1 << FC_STATUS_FAIL_BIT4))
#define FC_STATUS_ENH_FAIL_JEDEC_4PL    ((1 << FC_STATUS_FAIL_BIT2) | (1 << FC_STATUS_FAIL_BIT3) | (1 << FC_STATUS_FAIL_BIT4) | (1 << FC_STATUS_FAIL_BIT5) | (1 << FC_STATUS_FAIL_BIT6))
#define FC_PREFIX_SLC_BYTE              0xA2     ///< Toshiba and Sandisk only
#define FC_PREFIX_TLC_LOW_BYTE          0x01     ///< Toshiba and Sandisk only
#define FC_PREFIX_TLC_MID_BYTE          0x02     ///< Toshiba and Sandisk only
#define FC_PREFIX_TLC_UP_BYTE           0x03     ///< Toshiba and Sandisk only
#define FC_PREFIX_TLC_1ST_PS_BYTE       0x09     ///< Toshiba and Sandisk only
#define FC_PREFIX_TLC_2ND_PS_BYTE       0x0D     ///< Toshiba and Sandisk only
#define FC_PREFIX_TLC_PS_SHIFT          8        ///< Toshiba and Sandisk only
#define FC_PREFIX_TLC_MASK              0xFFFF00 ///< Toshiba and Sandisk only
/// @}

/// @name NAND Flash status register
/// @{
#define FC_STATUS_FAIL                  0x01    ///< the last command failed
#define FC_STATUS_FAILC                 0x02    ///< the command issued prior to the last command failed
#define FC_STATUS_ARDY                  0x20    ///< no array operation in progress
#define FC_STATUS_RDY                   0x40    ///< ready for another command
#define FC_STATUS_WP_N                  0x80    ///< write protoected
/// @}

/// @name NAND Flash Parameter Page Address
/// @{
#define FC_ONFI_PARAM_PAGE_SIG          0x49464E4F  ///< ONFI Parameter page signature "ONFI"
#define FC_JEDEC_PARAM_PAGE_SIG         0x4453454A  ///< JEDEC Parameter page signature "JESD"
/// @}

/// @name NAND Flash Feature address (Set/Get feature command)
/// @{
#define FC_FEATURE_TIMING_MODE          0x01        ///< Timing Mode
#define FC_FEATURE_DRIVER_STRENGTH      0x10        ///< I/O Driver strength
/// @}

/// @name NAND Flash Manufactor ID definitions
/// @{
#define NF_ID_SAMSUNG                   0xEC
#define NF_ID_MICRON                    0x2C
#define NF_ID_TOSHIBA                   0x98
#define NF_ID_HYNIX                     0xAD
/// @}

// Indirect mode -  configuration plus start execution bit (HW dependent)
#define IM_RESET_GO              0x0190      ///< reset flash configuration/sync reset plus start bit
#define IM_RESET_LUN_GO          0x0191      ///< reset LUN plus start bit
#define IM_RD_STATUS_GO          0x0180      ///< read status configuration plus start bit
#define IM_RD_ID_GO              0x0181      ///< read ID configuration plus start bit
#define IM_RND_DO_1ST_GO         0x01A6      ///< random data output 1st command sequence configuration plus start bit
#define IM_RD_SOUT_GO            0x0180      ///< read serial out plus start bit
#define IM_RND_DO_2ND_GO         0x01A3      ///< random data output 2nd command sequence configuration plus start bit
#define IM_PRG_GO                0x01B6      ///< program flash configuration plus start bit
#define IM_ERASE_GO              0x01B4      ///< erase flash configuration plus start bit
#define IM_RD_PARAMETER_PAGE_GO  0x0081      ///< read parameter page plus start bit
#define IM_GET_FEATURE_GO        0x0081      ///< get feature plus start bit
#define IM_SET_FEATURE_GO        0x0191      ///< set feature plus start bit
#define FC_DIRINDIR_FLASH_RDY    0xE0        ///< direct/indirect mode flash ready


/// @brief NAND Flash Transfer mode
typedef enum
{
    NF_XFER_MODE_0=0,   ///< Transfer mode #0
    NF_XFER_MODE_1,     ///< Transfer mode #1
    NF_XFER_MODE_2,     ///< Transfer mode #2
    NF_XFER_MODE_3,     ///< Transfer mode #3
    NF_XFER_MODE_4,     ///< Transfer mode #4
    NF_XFER_MODE_5,     ///< Transfer mode #5
    NF_XFER_MODE_6,     ///< Transfer mode #6
    NF_XFER_MODE_7,     ///< Transfer mode #7
    NF_XFER_MODE_8,     ///< Transfer mode #8
    NF_XFER_MODE_NULL=0x7FFFFFFF
} NfXferMode_t;

/// @brief NAND Flash Interface Spec. type
typedef enum
{
    NF_UNKNOWN=0,       ///< Unknown NAND I/F
    NF_ONFI,            ///< ONFI (Don't change order.)
    NF_JEDEC,           ///< JEDEC (Don't change order.)
    NF_NULL=0x7FFFFFFF
} NfIfType_t;

/// @brief NAND Flash data interface (DO NOT change order)
typedef enum
{
    NF_ASYNC=0,                 ///< Asynchronous mode
    NF_ONFI_SDR,                ///< ONFI asynchronous
    NF_ONFI_NVDDR,              ///< ONFI synchronous
    NF_JEDEC_TOGGLE1,           ///< JEDEC toggle1
    NF_ONFI_NVDDR2,             ///< ONFI3 synchronous
    NF_JEDEC_TOGGLE2,           ///< JEDEC toggle2
    NF_NOT_REGISTERED=0xF,      ///< Unknown NAND.
    NF_NOT_NULL=0x7FFFFFFF
} NfIfData_t;

/// @brief NAND Flash HW access modes
typedef enum
{
    NF_DIRECT=0,      ///< direct access mode
    NF_INDIRECT,    ///< indirect access mode
    NF_SEQUENCER,   ///< sequencer access mode
    NF_TEST,
    NF_MODE_NULL=0x7FFFFFFF
} NfAccessMode_t;

/// @brief NAND HW access modes
typedef enum
{
    DIRECT=0,               ///< direct access mode
    INDIRECT=1,             ///< indirect access mode
    SEQUENCER=2,            ///< sequencer access mode
    TEST=3,
    MODE_NONE=0x7FFFFFFF
}FcAccessMode_t;

/// @brief CE mode
typedef enum
{
    CE_MODE0 =0,          ///< SOC default setting including GPIO (nothing to do)
    CE_MODE1,             ///< Mode1; Native 4-CE support
    CE_MODE2,             ///< Mode2; Encoded 8-CE support
    CE_MODE3,             ///< Mode3; Encoded 16-CE support
    CE_MODE4,             ///< Mode4; Encoded 15-CE support
    CE_MODE5,             ///< Mode5; Native 8-CE support
    CE_NULL=0x7FFFFFFF
} tCE_MODE;

// the data sub PHY DQ/DQS ODT value during a read data cycle in the DDR2 mode.
static const uint16_t   cOdt0Ohm                 = 0;
static const uint16_t   cOdt150Ohm               = 1;
static const uint16_t   cOdt100Ohm               = 2;
static const uint16_t   cOdt75Ohm                = 3;
static const uint16_t   cOdt50Ohm                = 4;
static const uint16_t   cOdt37p5Ohm              = 5;
static const uint16_t   cOdt33Ohm                = 6;
static const uint16_t   cOdt30Ohm                = 7;

// Driven strength
static const uint16_t   cInitNandPhyPadSelect    = 6;         ///< initial NAND PHY Pad select value
static const uint8_t    cDriverValue             = 5;         ///< initial NAND Drive select value
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief ONFI 3.0 Parameter Page structure
typedef struct
{
    uint32_t signature;                                  ///< 000 Parameter page signature ("ONFI") 0x49464E4F
    uint16_t revision;                                   ///< 004 Revision number
    uint16_t featuresSupport;                            ///< 006 Features supported
    uint16_t optionalCmdSupport;                         ///< 008 Optional commands supported
    uint8_t  advancedCmdSupport;                         ///< 010 ONFI-JEDEC JTG primary advanced command support
    uint8_t  reserved011[12-11];                         ///< 011 reserved
    uint8_t  parameterPageLength;                        ///< 012 Extended parameter page length
    uint8_t  parameterPages;                             ///< 014 Number of parameter pages
    uint8_t  reserved015[32-15];                         ///< 015 reserved
    uint8_t  manufacturer[12];                           ///< 032 Device manufacturer (12 ASCII characters)
    uint8_t  model[20];                                  ///< 044 Device model (20 ASCII characters)
    uint8_t  jedecId;                                    ///< 064 JEDEC manufacturer ID
    __packed uint16_t dateCode;                          ///< 065 Date code
    uint8_t  reserved067[80-67];                         ///< 067 reserved
    uint32_t dataBytesPerPage;                           ///< 080 Number of data bytes per page
    uint16_t spareBytesPerPage;                          ///< 084 Number of spare bytes per page
    __packed uint32_t dataBytesPerPpage;                 ///< 086 Number of data bytes per partial page
    uint16_t spareBytesPerPpage;                         ///< 090 Number of spare bytes per partial page
    __packed uint32_t pagesPerBlock;                     ///< 092 Number of pages per block
    __packed uint32_t blocksPerLun;                      ///< 096 Number of blocks per logical unit (LUN)
    uint8_t  lunCount;                                   ///< 100 Number of logical units (LUNs)
    uint8_t  addrCycles;                                 ///< 101 Number of address cycles
    uint8_t  bitsPerCell;                                ///< 102 Number of bits per cell
    __packed uint16_t badBlockPperLun;                   ///< 103 Bad blocks maximum per LUN
    __packed uint16_t blockEndurance;                    ///< 105 Block endurance
    uint8_t  guaranteedGoodBlocks;                       ///< 107 Guaranteed valid blocks at beginning of target
    uint16_t guaranteedBlockEndurance;                   ///< 108 Block endurance for guaranteed valid blocks
    uint8_t  programsPerPage;                            ///< 110 Number of programs per page
    uint8_t  partialProgAttr;                            ///< 111 Partial programming attributes
    uint8_t  eccCorrectabilityBits;                      ///< 112 Number of bits ECC correctability
    uint8_t  planeAddressBits;                           ///< 113 Number of plane address bits
    uint8_t  multiPlaneOpAttr;                           ///< 114 Multi-plane operation attributes
    uint8_t  ezNandSupport;                              ///< 115 EZ NAND support
    uint8_t  reserved116[128-116];                       ///< 116 reserved
    uint8_t  ioPinCapacitanceMax;                        ///< 128 I/O pin capacitance, maximum
    __packed uint16_t sdrTimingModeSupport;              ///< 129 SDR timing mode support
    __packed uint16_t sdrProgCacheTimingModeSupport;     ///< 131 SDR program cache timing mode support
    __packed uint16_t tPROG;                             ///< 133 tPROG Maximum page program time (us)
    __packed uint16_t tBERS;                             ///< 135 tBERS Maximum block erase time (us)
    __packed uint16_t tR;                                ///< 137 tR Maximum page read time (us)
    __packed uint16_t tCCS;                              ///< 139 tCCS Minimum change column setup time (ns)
    uint8_t  ddrTimingModeSupport;                       ///< 141 NV-DDR timing mode support
    uint8_t  ddr2TimingModeSupport;                      ///< 142 NV-DDR2 timing mode support
    uint8_t  ddrFeatures;                                ///< 143 NV-DDR / NV-DDR2 features
    uint16_t clkPinCapacitance;                          ///< 144 CLK input pin capacitance, typical
    uint16_t ioPinCapacitance;                           ///< 146 I/O pin capacitance, typical
    uint16_t inputPinCapacitance;                        ///< 148 Input pin capacitance, typical
    uint8_t  inputPinCapacitanceMax;                     ///< 150 Input pin capacitance, maximum
    uint8_t  driverStrenghtSupport;                      ///< 151 Driver strength support
    uint16_t tRMP;                                       ///< 152 tR Maximum multi-plane page read time
    uint16_t tADL;                                       ///< 154 tADL Program page register clear enhancement tADL value (ns)
    uint16_t tR_EZ_NAND;                                 ///< 156 tR Typical page read time for EZ NAND
    uint8_t  ddr2Features;                               ///< 158 NV-DDR2 features
    uint8_t  ddr2WarmUpCycles;                           ///< 159 NV-DDR2 warmup cycles
    uint8_t  reserved160[164-160];                       ///< 160 reserved
    uint16_t vendorRevision;                             ///< 164 Vendor specific Revision number
    uint8_t  reserved166[254-166];                       ///< 166 reserved
    uint16_t crc;                                        ///< 254 Integrity CRC
} OnfiParamPage_t;
COMPILE_ASSERT(offsetof(OnfiParamPage_t, ioPinCapacitance)==146,"check structure offset");
COMPILE_ASSERT(sizeof(OnfiParamPage_t)==256, "size must be 256 bytes");

/// @brief JEDEC ECC and Endurance information structure
typedef struct
{
    uint8_t eccCorrectabilityBits;                       ///< 0 Number of bits ECC correctability
    uint8_t codeWordSize;                                ///< 1 Codeword size
    __packed uint16_t badBlockPperLun;                   ///< 3 Bad blocks maximum per LUN
    __packed uint16_t blockEndurance;                    ///< 5 Block endurance
    uint8_t reserved[2];                                 ///< 7 reserved
} JedecEccInfo_t;
COMPILE_ASSERT(sizeof(JedecEccInfo_t)==8, "size must be 8 bytes");

/// @brief JEDEC Parameter Page structure
typedef struct
{
    uint32_t signature;                                  ///< 000 Parameter page signature ("JESD") 0x4453454A
    uint16_t revision;                                   ///< 004 Revision number
    uint16_t featuresSupport;                            ///< 006 Features supported
    uint16_t optionalCmdSupport;                         ///< 008 Optional commands supported
    uint8_t  advancedCmdSupport;                         ///< 010 ONFI-JEDEC JTG primary advanced command support
    __packed uint16_t secondaryCmdSupport;               ///< 011 Secondary command supported
    uint8_t  numberOfParamPages;                         ///< 013 Number of Parameter Pages
    uint8_t  reserved014[32-14];                         ///< 014 reserved
    uint8_t  manufacturer[12];                           ///< 032 Device manufacturer (12 ASCII characters)
    uint8_t  model[20];                                  ///< 044 Device model (20 ASCII characters)
    uint8_t  jedecId[6];                                 ///< 064 JEDEC manufacturer ID
    uint8_t  reserved070[80-70];                         ///< 070 reserved
    uint32_t dataBytesPerPage;                           ///< 080 Number of data bytes per page
    uint16_t spareBytesPerPage;                          ///< 084 Number of spare bytes per page
    __packed uint32_t dataBytesPerPpage;                 ///< 086 Number of data bytes per partial page
    uint16_t spareBytesPerPpage;                         ///< 090 Number of spare bytes per partial page
    __packed uint32_t pagesPerBlock;                     ///< 092 Number of pages per block
    __packed uint32_t blocksPerLun;                      ///< 096 Number of blocks per logical unit (LUN)
    uint8_t  lunCount;                                   ///< 100 Number of logical units (LUNs)
    uint8_t  addrCycles;                                 ///< 101 Number of address cycles
    uint8_t  bitsPerCell;                                ///< 102 Number of bits per cell
    uint8_t  programsPerPage;                            ///< 103 Number of programs per page
    uint8_t  planeAddressBits;                           ///< 104 Number of plane address bits
    uint8_t  multiPlaneOpAttr;                           ///< 105 Multi-plane operation attributes
    uint8_t  reserved106[144-106];                       ///< 106 reserved
    uint16_t asyncSdrSpeedGrade;                         ///< 144 Asynchronous SDR speed grade
    uint16_t toggleDdrSpeedGrade;                        ///< 146 Toggle DDR speed grade
    uint16_t syncDdrSpeedGrade;                          ///< 148 Synchronous DDR speed grade
    uint8_t  asyncSdrFeatures;                           ///< 150 Asynchronous SDR features
    uint8_t  toggleDdrFeatures;                          ///< 151 Toggle DDR features
    uint8_t  syncDdrFeatures;                            ///< 152 Synchronous DDR features
    __packed uint16_t tPROG;                             ///< 153 tPROG Maximum page program time (us)
    __packed uint16_t tBERS;                             ///< 155 tBERS Maximum block erase time (us)
    __packed uint16_t tR;                                ///< 157 tR Maximum page read time (us)
    __packed uint16_t tRMP;                              ///< 159 tR Maximum multi-plane page read time (us)
    __packed uint16_t tCCS;                              ///< 161 tCCS Minimum change column setup time (ns)
    __packed uint16_t ioPinCapacitance;                  ///< 163 I/O pin capacitance, typical
    __packed uint16_t inputPinCapacitance;               ///< 165 Input pin capacitance, typical
    __packed uint16_t clkPinCapacitance;                 ///< 167 CLK input pin capacitance, typical
    uint8_t  driverStrenghtSupport;                      ///< 169 Driver strength support
    __packed uint16_t tADL;                              ///< 170 tADL Program page register clear enhancemet tADL value (ns)
    uint8_t  reserved172[208-172];                       ///< 172 reserved
    uint8_t  guaranteedGoodBlocks;                       ///< 208 Guaranteed valid blocks at beginning of target
    __packed uint16_t guaranteedBlockEndurance;          ///< 209 Block endurance for guaranteed valid blocks
    JedecEccInfo_t eccInfo[4];                     ///< 211 ECC and endurance information block 0 .. 3
    uint8_t  reserved243[420-243];                       ///< 243 reserved
    uint16_t venderSpesificRevisionNumber;               ///< 420 Vender Specific Revision Number
    uint8_t  venderSpesific[510-422];                    ///< 422 Vender Specific
    uint16_t crc;                                        ///< 510 Integrity CRC
} JedecParamPage_t;
COMPILE_ASSERT(offsetof(JedecParamPage_t,guaranteedBlockEndurance)==209,"check structure offset");
COMPILE_ASSERT(sizeof(JedecParamPage_t)==512, "size must be 512 bytes");

/// @brief NAND PHY Information
typedef struct
{
    uint8_t CMD_ODT;             ///< Command Syb-PHY ODT value
    uint8_t REB_ODT;             ///< REB Sub-PHY ODT value
    uint8_t WEB_ODT;             ///< WEB Sub-PHY ODT value
    uint8_t DATA_ODT[8];         ///< Data(DQ/DQS) Syb-PHY ODT value
    uint8_t PHSEL0[8];           ///< DLL Phase select(QS falling edge delay cell for DQ)
    uint8_t PHSEL1[8];           ///< DLL Phase select(QS rising edge delay cell for DQ)
    uint8_t RESERVED[5];         ///< RESERVED
    //32B---------------------------------------------------------------
} NfPhyInfo_t;
COMPILE_ASSERT((sizeof(NfPhyInfo_t) & 0x3) == 0, "please align to 4 bytes!");
/// @brief NAND Timing Information
typedef struct
{
    uint8_t  T1;                 ///< tRP/tCLK/tDQSL (ns)
    uint8_t  T2;                 ///< tREH/tCLH/tDQSH (ns)
    uint8_t  T3;                 ///< tWP/tCAD (ns)
    uint8_t  T4;                 ///< tWH/tRHW (ns)
    uint8_t  T5;                 ///< tCCS/tWHR (ns)
    uint8_t  T7[8];              ///< tDQSQ (ns)
    uint8_t  T8;                 ///< tADL (ns)
    uint8_t  T9;                 ///< tWHR (ns)
    uint8_t  tCS;                ///< tCS (ns)
    uint8_t  tWPRE;              ///< tWPRE (ns)
    uint8_t  tWPST;              ///< tWPST (ns)
    uint8_t  tWPSTH;             ///< tWPSTH (ns)
    uint8_t  tRHW;               ///< tRHW (ns)
    uint8_t  tRPRE;              ///< tRPRE (ns)
    uint8_t  tRPST;              ///< tRPST (ns)
    uint8_t  tRPSTH;             ///< tRPSTH (ns)
    uint8_t  tCWAW;              ///< tCWAW (ns)
    uint8_t  tCEH;               ///< tCEH (ns)
    uint8_t  tWB;                ///< tWB (ns)
    uint8_t  reserved[6];        ///< reserved
    //32B---------------------------------------------------------------
} NfTimingInfo_t;
COMPILE_ASSERT((sizeof(NfTimingInfo_t) & 0x3) == 0, "please align to 4 bytes!");
/// @brief Phyiscal NAND Flash media parameters (Assume all NANDs are same)
typedef struct
{
    //-------------------------------------------------------------------
    NfIfType_t ifType;   ///< Interface spec type (fixed)
    NfIfData_t ifData;   ///< Max. data interface (fixed)
    NfIfData_t socIfData;///< Max. data interface supported by SoC (fixed)
    NfIfData_t curIfData;///< current data interface (variable)
    NfIfData_t reqIfData;///< requested data interface (variable)
    NfTimingInfo_t timingInfo;
    NfPhyInfo_t phyInfo;
    uint16_t tPROG;              ///< tPROG Maximum page program time (us)
    uint16_t tBERS;              ///< tBERS Maximum block erase time (us)
    uint16_t tR;                 ///< tR Maximum page read time (us)
    uint16_t tRMP;               ///< tR Maximum multi-plane page read time (us)
    uint16_t tCCS;               ///< tCCS Minimum change column setup time (us)
    uint16_t updatePhyInfo;
    uint16_t updateTimingInfo;
    uint8_t xferMode;            ///< Max. transfer mode number (fixed)
    uint8_t curXferMode;         ///< current transfer mode number (variable)
    uint8_t reqXferMode;         ///< requested transfer mode number (variable)
    uint8_t manufacturer[12];    ///< 032 Device manufacturer (12 ASCII characters)
    uint8_t model[20];           ///< 044 Device model (20 ASCII characters)
    //136B-------------------------------------------------------------------
}NfCfgInfo_t;
COMPILE_ASSERT((sizeof(NfCfgInfo_t) & 0x3) == 0, "please align to 4 bytes!");
/// @brief Phyiscal NAND Flash media parameters (Assume all NANDs are same)
typedef struct
{
    //-------------------------------------------------------------------
    uint32_t dataBytesPerPage;   ///< number of data bytes per Page
    uint32_t spareBytesPerPage;  ///< number of spare bytes per Page
    uint32_t pagesPerBlock;      ///< pages per block
    uint32_t blocksPerLun;       ///< blocks per Lun
    uint32_t wlPerBlock;         ///< Number of WLs per Block
    uint32_t userBlkPerLun;      ///< exclude spared block
    uint32_t blocksPerPlane;     ///< blocks per Plane  (derivative value)
    uint32_t blocksPerDevice;    ///< blocks per Device (derivative value)
    uint32_t blocksPerChannel;   ///< blocks per Channel (derivative value)
    uint32_t totalBlocks;        ///< total number of blocks (derivative value)
    uint32_t pagesPerLun;        ///< Pages per Lun (derivative value)
    uint32_t pagesPerDevice;     ///< Pages per Device (derivative value)
    uint32_t pagesPerChannel;    ///< Pages per Channel (derivative value)
    uint32_t totalPages;         ///< total number of pages (derivative value)
    uint32_t snpn;               ///< Simultaneously accessible NAND Page Number (derivative value)
    uint32_t userCapacity;       ///< unit:GB
    uint32_t rawNANDCapacity;    ///< unit:GiB
    //68B-------------------------------------------------------------------
    uint16_t totalLun;           ///< ch * device * lun per device
    uint16_t dataBytesPerFullPage;   ///< dataBytePerPage + spareBytesPerPage
    uint16_t sparedBlockPerLun;  ///< blocksPerLun - userBlkPerLun
    uint16_t pagesPerProgUnit;   /// flush program unit
    //74B-------------------------------------------------------------------
    uint8_t bitsPerCell;         ///< Bits per cell
    uint8_t numChannels;         ///< total number of channels
    uint8_t devicesPerChannel;   ///< devices per channel
    uint8_t lunsPerDevice;       ///< LUNs per device
    uint8_t lunsPerChannel;      ///< LUNs per Channel
    uint8_t planeAddressBits;    ///< number of plane(interleave) address bits
    uint8_t numPseudoPlane;      ///< Number of pesudo-Plane
    uint8_t bitsDevice;
    uint8_t bitsCh;
    uint8_t bitsLun;
    uint8_t bitsPlane;
    uint8_t bitsChDev;
    uint8_t bitsPagesPerBlock;   ///< Bits pages per block (derivative value)
    uint8_t bitsBlocksPerLun;    ///< Bits blocks per Lun (derivative value)
    uint8_t planeLsbPosition;    ///< plane address LSB poisition (derivative value)
    uint8_t lunLsbPosition;      ///< LUN address LSB position    (derivative value)
    uint8_t pairedBlkLsbPos;     ///< Paired block address LSB position (derivative value)
    uint8_t planesPerLun;        ///< planes per LUN (derivative value)
    uint8_t oriPlanesPerLun;     ///< original plane per lun
    uint8_t id[8];               ///< Manufacturer Id
    uint8_t reserved[25];        ///< Reserved
    //128B-------------------------------------------------------------------
} NfInfo_t;
COMPILE_ASSERT((sizeof(NfInfo_t) & 0x3) == 0, "please align to 4 bytes!");

typedef struct
{
    U8 ivWCNT;
    U8 ivDiv;
    U8 ofstWCNT;
    U8 ofstDiv;
} TMCtrlInfo_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern NfInfo_t gNfInfo;
extern NfCfgInfo_t gCfgInfo;
//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void     HalNfif_ChangeTiming(U32 level);
void     HalNfif_SetDataIf(NfIfData_t ifData);
void     HalNfif_EnDisableWriteProtect(bool enDisable);
void     HalNfif_DumpInfo(void);
Error_t  HalNfif_OneTimeInit(InitBootMode_t initMode);
Error_t  HalNfif_Configure(NfCfgInfo_t* pCfgInfo);
uint32_t HalNfif_SetFlashOperationMode(FcAccessMode_t mode);

// Indirect mode
void     HalNfif_ImSetupCmdRowAddrCount(Pna_t* pAddr, uint8_t command1, uint8_t command2, uint32_t count);
void     HalNfif_ImSetupCmdAddrCount(Pna_t* pAddr, uint8_t command1, uint8_t command2, uint32_t count);
void     HalNfif_ImExecuteCommand(uint16_t cmd);
void     HalNfif_ImSetDummyReady(void);
Error_t  HalNfif_ImWriteData(uint32_t* pBuffer, uint32_t count);
Error_t  HalNfif_ImWaitForCommandStart(void);
Error_t  HalNfif_ImReadData(uint32_t* pBuffer, uint32_t count);
Error_t  HalNfif_ImWriteData(uint32_t* pBuffer, uint32_t count);
Error_t  HalNfif_ImWaitForFifoReady(void);
Error_t  HalNfif_ImReadDataDdr(uint8_t *pBuffer, uint32_t count);
uint32_t HalNfif_ImReadDataU32(void);

// Direct mode
void     HalNfif_DmBackToDefault(void);
void     HalNfif_DmWaitReToDqsDelay(void);
void     HalNfif_DmSetDeviceChannel(Pna_t* pAddr);
void     HalNfif_DmWriteCommandU8(uint16_t cmd);
void     HalNfif_DmWriteAddressU8(uint8_t value);
void     HalNfif_DmWriteToggleDataU16(uint16_t word);
void     HalNfif_DmWriteDataU16(uint16_t word);
void     HalNfif_DmWriteToggleData(uint16_t word, uint16_t mode);
uint16_t HalNfif_DmReadDataU16(void);
uint16_t HalNfif_DmReadToggleDataU16(void);

// NFMode NAND access APIs
Error_t  HalNfif_ReadPage(Pna_t* pAddr, void* pBuffer, uint32_t count, uint8_t mode, uint8_t slc);
Error_t  HalNfif_WritePage(Pna_t* pAddr, void* pBuffer, uint32_t count, uint8_t mode, uint8_t slc);
Error_t  HalNfif_EraseBlock(Pna_t* pAddr, uint8_t mode, uint8_t slc);
Error_t  HalNfif_Reset(Pna_t* pAddr, uint8_t resetCmd, uint8_t mode);
uint8_t  HalNfif_ReadStatus(Pna_t* pAddr, uint8_t mode);
Error_t  HalNfif_GetFeature(Pna_t* pAddr, uint8_t param[], uint8_t mode);
Error_t  HalNfif_SetFeature(Pna_t* pAddr, uint8_t param[], uint8_t mode);
Error_t  HalNfif_ReadId(Pna_t* pAddr, void* pBuffer, uint32_t count, uint8_t mode);
Error_t  HalNfif_WaitForStatusSet(Pna_t* pAddr, uint32_t value, uint32_t timeOutUsec, uint8_t mode);
Error_t  HalNfif_ReadParameterPage(Pna_t* pAddr, void* pBuffer, uint32_t count, uint8_t mode);
Error_t  HalNfif_GetVoltageOffset(Pna_t *pAddr, uint8_t featureAddr, uint8_t param[], uint8_t mode);
Error_t  HalNfif_SetVoltageOffset(Pna_t *pAddr, uint8_t featureAddr, uint8_t param[], uint8_t mode);


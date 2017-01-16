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
//! @brief Pure HAL API definition for DRAM H/W control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Constant definitions:
//-----------------------------------------------------------------------------
// DRAM Organization
#define DRAM_DATA_WIDTH_IN_BIT      32
#define DRAM_DATA_WIDTH_IN_BYTE     (DRAM_DATA_WIDTH_IN_BIT >> 3)

#define DRAM_NUMBER_OF_CS_MAX       2
#define DRAM_NUMBER_OF_CS           DRAM_NUMBER_OF_CS_MAX

//-----------------------------------------------------------------------------
// DRAM Test Parameter
#define DRAM_TEST_SIZE              (32 * KBYTE)
#define DRAM_TEST_START_ADDRESS0    (DIRECT_DRAM_ADDR)

#define DRAM_TEST_START_ADDRESS1    (CACHED_DRAM_ADDR + halDramInfo.size - DRAM_TEST_SIZE)

//-----------------------------------------------------------------------------
// AVS
#define AVS_VDD_096V    0x31F1F803
#define AVS_VDD_090V    0x31919803

//-----------------------------------------------------------------------------
//Timer
#define USEC2SEC        (1000 * 1000)

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define D_PRINTF_DRAM(F, args...)   D_PRINTF(F, ##args)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief DRAM mode
typedef enum
{
    cDramPmActive       = 0x01,     ///< DRAM active       mode - highest power consumption state
    cDramPmSelfRefresh  = 0x02,     ///< DRAM self-refresh mode
    cDramPmDeepSleep    = 0x0F      ///< DRAM deep sleep   mode - lowest  power consumption state
} DramPowerMode_t;

/// @brief enter & exit Self-Refresh mode
typedef enum
{
    cModeEnter = 1,     ///< Enter Self-Refresh mode
    cModeExit           ///< Exit  Self-Refresh mode
} SelfRefreshMode_t;

//-----------------------------------------------------------------------------
/// @brief DRAM information
typedef struct
{
    U32             clockFreq;          ///< SDRAM clock frequency in hertz unit
    U32             size;               ///< SDRAM size in byte count
    U8              type;               ///< SDRAM type
    U8              mode;               ///< SDRAM current mode (see DramPowerMode_t)
    U8              dataWidth;          ///< SDRAM data bus width (8, 16, 32bit)
    U8              numberOfColumnBit;  ///< SDRAM number of column bit
    U8              numberOfRowBit;     ///< SDRAM number of row    bit
    U8              numberOfBankBit;    ///< SDRAM number of bank   bit
    U8              numberOfCs;         ///< SDRAM number of cs
    U8              reserved;           ///< for alignment (to 16 bytes)
} DramInfo_t;

//-----------------------------------------------------------------------------
// HW info element
typedef struct {
    U32     tag;
    U8      dllPositive[DRAM_DATA_WIDTH_IN_BYTE];
    U8      dllNegative[DRAM_DATA_WIDTH_IN_BYTE];
    U32     checkSum;
} DramDll_t;
COMPILE_ASSERT(sizeof(DramDll_t) == 16, "DramDll_t size shall be 16 bytes");

typedef struct {
    U32     tag;
    U8      phyCalLsb;
    U8      phyCalMsb;
    U8      reserved[2];
    U32     checkSum;
    U32     reserved2;
} DramCal_t;
COMPILE_ASSERT(sizeof(DramCal_t) == 16, "DramCal_t size shall be 16 bytes");

typedef struct {
    U32     tag;
    U32     avs;
} SocType_t;
COMPILE_ASSERT(sizeof(SocType_t) == 8, "SocType_t size shall be 8 bytes");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern DramInfo_t halDramInfo;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t HalDram_OneTimeInit(InitBootMode_t initMode);
Error_t HalDram_Recovery(void);
Error_t HalDram_SetPowerMode(DramPowerMode_t powerMode);

/* End of File */


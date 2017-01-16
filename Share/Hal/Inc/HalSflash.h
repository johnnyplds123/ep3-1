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
//! @brief Pure HAL API definition for Serial Flash HW control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define CHIP_ERASE_TIMEOUT_US           9900000         ///< Chip erase time out
#define SECTOR_ERASE_TIMEOUT_US         1000000         ///< Sector erase time out
#define PAGE_WRITE_TIMEOUT_US           1000000         ///< Page write time out

#define SFLASH_BLOCK_SIZE_32K           (32 * KBYTE)    // WINBOND
#define SFLASH_BLOCK_SIZE               (16 * KBYTE)    // M25P40
#define WIP                             (1)             // Write In Progess Bit of Status Register
#define SPI_BASE_ADDRESS                (0x21000000)
#define SFLASH_PAGE_SIZE                (256)
#define SFLASH_4BYTE_SIZE               (32)
#define SFLASH_SECTOR_SIZE              (4 * KBYTE)
#define SFLASH_SECTOR_PER_BLOCK         (SFLASH_BLOCK_SIZE_32K / SFLASH_SECTOR_SIZE)

#define SFLASH_RETRY_COUNT              3

//
// M25P40
//
//#define SFLASH_DEVICE_CAPACITY         (512 * KBYTE)
//
// WINBOND
//
//NOR Address//
#define SFH_FW_START                   0
#define SFH_FW_START_BYTE              (SFH_FW_START * SFLASH_SECTOR_SIZE)
#define SFH_FW_COUNT                   360

#define SFH_HWINFO_START               (SFH_FW_START + SFH_FW_COUNT)
#define SFH_HWINFO_START_BYTE          (SFH_HWINFO_START * SFLASH_SECTOR_SIZE)
#define SFH_HWINFO_COUNT               1

#define SFH_OVERRIDRFTL_START           (SFH_HWINFO_START + SFH_HWINFO_COUNT)
#define SFH_OVERRIDRFTL_START_BYTE      (SFH_OVERRIDRFTL_START * SFLASH_SECTOR_SIZE)
#define SFH_OVERRIDRFTL_COUNT           1

#define SFH_RESERVED1_START            (SFH_OVERRIDRFTL_START + SFH_OVERRIDRFTL_COUNT)
#define SFH_RESERVED1_START_BYTE       (SFH_RESERVED1_START * SFLASH_SECTOR_SIZE)
#define SFH_RESERVED1_COUNT            2

#define SFH_QUICK_IDX_START            (SFH_RESERVED1_START + SFH_RESERVED1_COUNT)
#define SFH_QUICK_IDX_BYTE             (SFH_QUICK_IDX_START * SFLASH_SECTOR_SIZE)
#define SFH_QUICK_IDX_COUNT            8

#define SFH_CRC_START                  (SFH_QUICK_IDX_START + SFH_QUICK_IDX_COUNT)
#define SFH_CRC_START_BYTE             (SFH_CRC_START * SFLASH_SECTOR_SIZE)
#define SFH_CRC_COUNT                  1

#define SFH_HEADER_START               (SFH_CRC_START + SFH_CRC_COUNT)
#define SFH_HEADER_BYTE                (SFH_HEADER_START * SFLASH_SECTOR_SIZE)
#define SFH_HEADER_COUNT               (SI_PAYLOAD_SIZE_HEADER / SFLASH_SECTOR_SIZE)

#define SFH_PLTCNT_START               (SFH_HEADER_START + SFH_HEADER_COUNT)
#define SFH_PLTCNT_BYTE                (SFH_PLTCNT_START * SFLASH_SECTOR_SIZE)
#define SFH_PLTCNT_COUNT               (SI_PAYLOAD_SIZE_PLSTCNT / SFLASH_SECTOR_SIZE)

#define SFH_RESERVED2_START            (SFH_PLTCNT_START + SFH_PLTCNT_COUNT)
#define SFH_RESERVED2_START_BYTE       (SFH_RESERVED2_START * SFLASH_SECTOR_SIZE)
#define SFH_RESERVED2_COUNT            1

#define SFH_TCG_START                  (SFH_RESERVED2_START + SFH_RESERVED2_COUNT)
#define SFH_TCG_START_BYTE             (SFH_TCG_START * SFLASH_SECTOR_SIZE)
#define SFH_TCG_COUNT                  4

#define SFH_AGING_START                (SFH_TCG_START + SFH_TCG_COUNT)
#define SFH_AGING_START_BYTE           (SFH_AGING_START * SFLASH_SECTOR_SIZE)
#define SFH_AGING_COUNT                (SI_PAYLOAD_SIZE_AGING / SFLASH_SECTOR_SIZE)

#define SFH_PLIST1_START               (SFH_AGING_START + SFH_AGING_COUNT)
#define SFH_PLIST1_START_BYTE          (SFH_PLIST1_START * SFLASH_SECTOR_SIZE)
#define SFH_PLIST1_COUNT               (SI_PAYLOAD_SIZE_PLIST1 / SFLASH_SECTOR_SIZE)

#define SFH_PLIST2_START               (SFH_PLIST1_START + SFH_PLIST1_COUNT)
#define SFH_PLIST2_START_BYTE          (SFH_PLIST2_START * SFLASH_SECTOR_SIZE)
#define SFH_PLIST2_COUNT               (SI_PAYLOAD_SIZE_PLIST2 / SFLASH_SECTOR_SIZE)

//NOR Address//

#define SFLASH_DEVICE_CAPACITY         (2048 * KBYTE)

typedef uint32_t SflashSector_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define NUM_BLOCKS(capacity)    (capacity / SFLASH_BLOCK_SIZE_32K)

/// @brief Sflash power mode definition
typedef enum
{
    cSflashPmActive=1,        ///< Active mode - highest power consumption state
    cSflashPmDeepSleep=15     ///< Deep sleep mode - Lowest power consumption state
} SflashPowerMode_t;

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief serial flash device parameter
typedef struct HalSflash_t
{
    uint32_t              device;     ///< Index of current device in device table
    SflashPowerMode_t     powerMode;
} HalSflash_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t  HalSflash_OneTimeInit(InitBootMode_t initMode);
Error_t  HalSflash_Compare(void* pBuffer, uint32_t flashAddr, uint32_t size);
Error_t  HalSflash_Program(void* pBuffer, uint32_t flashAddr, uint32_t size);
Error_t  HalSflash_ProgramU32(void* pBuffer, uint32_t flashAddr, uint32_t size);
Error_t  HalSflash_EraseSector(SflashSector_t sectorAddr, uint32_t sectorCount ,uint8_t retryCount);
Error_t  HalSflash_EraseChip(uint32_t retryCount);
Error_t  HalSflash_SetPowerMode(SflashPowerMode_t powerMode);
uint32_t HalSflash_ReadId(uint8_t idCommand);
uint32_t HalSflash_Copy(void* pBuffer, uint32_t flashAddr, uint32_t size);
uint32_t HalSflash_ReadU32(uint32_t flashAddr);
uint8_t  HalSflash_ReadU8(uint32_t flashAddr);

void HalSflash_MiscHwInfo_OneTimeInit(void);
void HalSflash_MiscHwInfo_Flush(void);
void HalSflash_OverRideFTL_Sync(void);

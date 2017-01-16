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
//! \file
//! @brief Serial Flash HW control codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "MemIo.h"
#include "HalTimer.h"
#include "HalSflash.h"
#include "ClockTree.h"
#include "RegSpiBridge.h"
#include "HalCpu.h"
#include "SharedVars.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief command table for serial flash
typedef struct
{
    uint8_t readDataBytes;                    ///< 0
    uint8_t readStatusReg;                    ///<.
    uint8_t writeStatusReg;                   ///<
    uint8_t writeEnable;                      ///<
    uint8_t writeDisable;                     ///<
    uint8_t programByte;                      ///<
    uint8_t sectorErase;                      ///< commands
    uint8_t bulkErase;                        ///<
    uint8_t readMid;                          ///<
    uint8_t numberOfSectors;                  ///<
    uint8_t pageSizeLow;                      ///<
    uint8_t pageSizeHi;                       ///<
    uint8_t deviceTag;                        ///<
    uint8_t ReadIDflags;                      ///< data
    uint8_t deviceMid;                        ///<
    uint8_t deviceId;                         ///<
    uint8_t powerdown;                        ///<
    uint8_t releasepowerdown;                 ///<
} SflashData_t;

/// @brief serial flash device parameter
typedef struct
{
    const SflashData_t*   pDevicePtr;///< Ptr to current device data table
    uint8_t              deviceMid;       ///< Serial Memory Manufacture ID
    uint8_t              deviceId;        ///< Serial Memory Device ID
    uint16_t             maxSndWait;      ///< max waiting time for sending command
    uint16_t             maxGetWait;      ///< max waiting time for receiving command
    uint16_t             maxSFEnWait;     ///<
} SflashParam_t;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static Error_t FindDevice(SflashParam_t* pFlashParam);
static Error_t WaitWhileBusy(uint32_t timeOutUsec);
static void    IssueCommand(uint8_t command, uint8_t readLength, uint8_t readWrite, uint8_t commandLength);
static Error_t EraseChip(void);
static Error_t EraseSectors(SflashSector_t sectorAddr, uint32_t sectorCount);
static Error_t EraseBlocks(SflashSector_t sectorAddr, uint32_t blockCount);
static Error_t PageWrite( uint32_t* pBuffer, uint32_t baseAddress, uint32_t length );
static Error_t PageRead( uint32_t* pBuffer, uint32_t baseAddress, uint32_t length );
static void    HalSflash_OverRideFTL_OneTimeInit(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//
//  VARIABLE DECLARATION WITH INITIAL VALUE DOESN'T WORK !!!
//  UNINTIALIZED VARIABLE DECLARATION ONLY !!!
//-----------------------------------------------------------------------------
#pragma arm section rwdata = "data_fast_ui"
uint32_t    spiwbuf[256/4];
#pragma arm section zidata = "data_fast_ui"

const SflashData_t defaultDevice =
{
    // ST_M25P40
    //0x0B,0x05,0x01,0x06,0x04,0x02,0xD8,0xC7,0xAB,0x08,0x00,0x01,0x04,1,0x12,0x00,

    // 20140826 CHECK
    0x0B,0x05,0x01,0x06,0x04,0x02,0x20,0x52,0xAB,0x08,0x00,0x01,0x04,1,0x12,0x00,0xB9,0xAB
};

#if 0
const SflashData_t serMemDataTable[NUMBER_OF_DEVICES] =
{
    // AT_F251024N
    {0x03,0x05,0x01,0x06,0x04,0x02,0x52,0x62,0x15,0x04,0x00,0x01,0x02,0,0x1F,0x00},
    // ST_M25P10
    {0x03,0x05,0x01,0x06,0x04,0x02,0xD8,0xC7,0xAB,0x04,0x80,0x00,0x01,1,0x10,0x00},
    // ST_M25P20
    {0x03,0x05,0x01,0x06,0x04,0x02,0xD8,0xC7,0xAB,0x04,0x00,0x01,0x03,1,0x11,0x00},
    // ST_M25P40
    {0x03,0x05,0x01,0x06,0x04,0x02,0xD8,0xC7,0xAB,0x08,0x00,0x01,0x04,1,0x12,0x00},
    // SST_25VF010
    {0x03,0x05,0x01,0x06,0x04,0x02,0x20,0x60,0xAB,0x20,0x00,0x00,0x05,1,0xBF,0x49},
    // SST_25VF020
    {0x03,0x05,0x01,0x06,0x04,0x02,0x20,0x60,0xAB,0x40,0x00,0x00,0x06,1,0xBF,0x43},
    // SST_25VF040, SST_25WF040
    {0x03,0x05,0x01,0x06,0x04,0x02,0x20,0x60,0xAB,0x80,0x00,0x00,0x07,1,0xBF,0x44}
};
#endif

static SflashParam_t sflashVariables;
static SflashPowerMode_t currentSflashPowerMode;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
/**
    One-time initialization of Serial Flash H/W.\n

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalSflash_OneTimeInit(InitBootMode_t initMode)
{
    FindDevice(&sflashVariables);

    #if _CPU_==CPU_ID0

    if (initMode != cInitBootCpu0Boot)
    {
        rSpiBridge.spiCfg.b.DV_ALLCT = 1;   // 1 device

        rSpiBridge.deviceRng0.b.EN_0 = 1;   // Enable device 0
        rSpiBridge.deviceRng0.b.DV_RNG_0 = NUM_BLOCKS(SFLASH_DEVICE_CAPACITY * 2);  // 512KB for M45PE40

        rSpiBridge.deviceDescript0.b.DT_W_FLSH_CMD_0 = sflashVariables.pDevicePtr->programByte;     //SFLASH_CMD_PAGE_PROGRAM;
        rSpiBridge.deviceDescript0.b.DT_RD_FLSH_CMD_0 = sflashVariables.pDevicePtr->readDataBytes;  //SFLASH_CMD_FAST_READ;
        rSpiBridge.deviceDescript0.b.W_FIFO_INT_EN_0 = 1;

        rSpiBridge.deviceDescript0.b.DV_SPI_CLK_PERIOD_0 = 0;               // Workaround

        HalSflash_MiscHwInfo_OneTimeInit();
    }

    #if CHK_CODE_TYPE(CODE_MAIN)
    HalSflash_OverRideFTL_OneTimeInit();
    #endif

    currentSflashPowerMode = cSflashPmActive;

    #endif

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    H/W block level power management API

    @param[in]  powerMode        targeting pm mode to enter
    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalSflash_SetPowerMode(SflashPowerMode_t powerMode)
{
    // no set up if power mode is not changed.
    if (currentSflashPowerMode == powerMode) return cEcNoError;

    switch (powerMode)
    {
        case cSflashPmDeepSleep:
            break;
        case cSflashPmActive:
            HalSflash_OneTimeInit(cInitBootDeepPowerDown);
            break;
        default:
            break;
    }

    currentSflashPowerMode = powerMode;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Read Id of Serial Flash.\n

    @param[in]  idCommand       Id command opcode (90h, 9Fh)

    @return 4 bytes Ids
**/
//-----------------------------------------------------------------------------
uint32_t HalSflash_ReadId(uint8_t idCommand)
{
    return 0;
}

//-----------------------------------------------------------------------------
/**
    Read a byte(uint8_t) data from serial flash.

    @param[in]  flashAddr   flash address

    @return   byte data in serial flash
**/
//-----------------------------------------------------------------------------
uint8_t HalSflash_ReadU8(uint32_t flashAddr)
{
    uint8_t data;
    BEGIN_MULTI_CS2(cSpinLockSFlash)
    data = REG_U8(flashAddr + SPI_BASE_ADDRESS);
    END_MULTI_CS2(cSpinLockSFlash)
    return data;
}

//-----------------------------------------------------------------------------
/**
    Read a byte(uint32_t) data from serial flash.

    @param[in]  flashAddr   flash address

    @return   byte data in serial flash
**/
//-----------------------------------------------------------------------------
uint32_t HalSflash_ReadU32(uint32_t flashAddr)
{
    uint32_t data;
    BEGIN_MULTI_CS2(cSpinLockSFlash)
    data = REG_U32(flashAddr + SPI_BASE_ADDRESS);
    END_MULTI_CS2(cSpinLockSFlash)
    return data;
}

//-----------------------------------------------------------------------------
/**
    Copy data from source serial flash to destination memory buffer pointer

    @param[out] pBuffer     destination buffer pointer
    @param[in]  flashAddr   source serial flash address
    @param[in]  size        byte size to copy

    @return   0 if no error, else remain bytes
**/
//-----------------------------------------------------------------------------
uint32_t HalSflash_Copy(void* pBuffer, uint32_t flashAddr, uint32_t size)
{
    BEGIN_MULTI_CS2(cSpinLockSFlash)
    MEM_COPY(pBuffer, (void*)(flashAddr + SPI_BASE_ADDRESS), size);
    END_MULTI_CS2(cSpinLockSFlash)
    return (0);
}

#if CHK_CODE_TYPE(CODE_MAIN)
//-----------------------------------------------------------------------------
/**
    Compare data in memory buffer and serial flash.

    @param[in]  pBffer      buffer pointer
    @param[in]  flashAddr   serial flash address
    @param[in]  size        byte size to compare

    @return   cEcNoError if same, else if not same
**/
//-----------------------------------------------------------------------------
Error_t HalSflash_Compare(void* pBuffer, uint32_t flashAddr, uint32_t size)
{
    uint32_t result;

    BEGIN_MULTI_CS2(cSpinLockSFlash)
    result = MEM_CMP(pBuffer, (void*)(flashAddr + SPI_BASE_ADDRESS), size);
    END_MULTI_CS2(cSpinLockSFlash)

    return result==0 ? cEcNoError : cEcMisCompare;
}

//-----------------------------------------------------------------------------
/**
    ADD CODE AND DESCRIPTION

    @param[in]  pBuffer     source buffer pointer
    @param[out] flashAddr   destination serial flash address
    @param[in]  size        byte size to program

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t HalSflash_Program(void* pBuffer, uint32_t flashAddr, uint32_t size)
{
    uint32_t count = 0;
    uint32_t* pBuf = pBuffer;

    for (count = 0; count < size; count += SFLASH_PAGE_SIZE)
    {
        memcpy(spiwbuf, &pBuf[count/4], SFLASH_PAGE_SIZE);
        PageWrite(spiwbuf, flashAddr + count, SFLASH_PAGE_SIZE);
        //PageWrite(&pBuf[count/4], flashAddr + count, SFLASH_PAGE_SIZE);
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
Error_t HalSflash_ProgramU32(void* pBuffer, uint32_t flashAddr, uint32_t size)
{
    uint32_t count = 0;
    uint32_t* pBuf = pBuffer;

    for(count = 0; count < size; count += 4)
    {
        //memcpy(spiwbuf, &pBuf[count/4], SFLASH_PAGE_SIZE);
        //PageWrite(spiwbuf, flashAddr + count, SFLASH_PAGE_SIZE);
        PageWrite(&pBuf[count / 4], flashAddr + count, 4);
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    This function erases specified sector of flash.

    @param[in]  sectorAddr  serial flash sector address to erase
    @param[in]  sectorCount sector count to erase
    @param[in]  retryCount  maximum retry count

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t HalSflash_EraseSector(SflashSector_t sectorAddr, uint32_t sectorCount ,uint8_t retryCount)
{
    Error_t error = cEcNoError;

    while (retryCount--)
    {
        if (EraseSectors(sectorAddr, sectorCount) == cEcNoError) return cEcNoError;
    }
    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    This function erases the entire flash\n

    @param[in]  retryCount      maximum retry count

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t HalSflash_EraseChip(uint32_t retryCount)
{
    while (retryCount--)
    {
        if (EraseChip() == cEcNoError) return cEcNoError;
    }

    return (cEcError);
}

//-----------------------------------------------------------------------------
/**
    This function erases the entire flash.
    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t EraseChip(void)
{
    Error_t error;

    BEGIN_MULTI_CS2(cSpinLockSFlash)

    rSpiBridge.CMD_DPNDNT = 0;
    IssueCommand(sflashVariables.pDevicePtr->writeEnable, 0, 0, 0);
    IssueCommand(sflashVariables.pDevicePtr->bulkErase, 0, 0, 0);

    error = WaitWhileBusy(CHIP_ERASE_TIMEOUT_US);
    END_MULTI_CS2(cSpinLockSFlash)
    return (error);
}

//-----------------------------------------------------------------------------
/**
    This function erases specified sectors

    @param    sectorAddr    start sector address to be erased
    @param    sectorCount   sector count to erase

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t EraseSectors(uint32_t sectorAddr, uint32_t sectorCount)
{
    Error_t  error = cEcNoError;
    uint32_t byteAddr;

    if ((sectorAddr + sectorCount) * SFLASH_SECTOR_SIZE > SFLASH_DEVICE_CAPACITY)
    {
        return (cEcInvalidParameter);
    }

    BEGIN_MULTI_CS2(cSpinLockSFlash)
    while (sectorCount--)
    {
        byteAddr = sectorAddr * SFLASH_SECTOR_SIZE;

        if (byteAddr >= SFLASH_DEVICE_CAPACITY)
        {
            error = cEcInvalidParameter;
            break;
        }

        rSpiBridge.CMD_DPNDNT = byteAddr;
        IssueCommand(sflashVariables.pDevicePtr->writeEnable, 0, 0, 0);
        IssueCommand(sflashVariables.pDevicePtr->sectorErase, 0, 0, 3);
        if (WaitWhileBusy(SECTOR_ERASE_TIMEOUT_US) != cEcNoError)
        {
            error = cEcError;
            break;
        }
        sectorAddr++;
    }
    END_MULTI_CS2(cSpinLockSFlash)

    return (error);
}

//-----------------------------------------------------------------------------
/**
    This function erases specified sectors

    @param    sectorAddr    start sector address to be erased
    @param    sectorCount   sector count to erase

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t EraseBlocks(uint32_t sectorAddr, uint32_t blockCount)
{
    Error_t  error = cEcNoError;
    uint32_t byteAddr;

    if ((sectorAddr * SFLASH_SECTOR_SIZE) + (blockCount * SFLASH_BLOCK_SIZE_32K) > SFLASH_DEVICE_CAPACITY)
    {
        return (cEcInvalidParameter);
    }

    BEGIN_MULTI_CS2(cSpinLockSFlash)
    while (blockCount--)
    {
        byteAddr = sectorAddr * SFLASH_SECTOR_SIZE;

        if (byteAddr >= SFLASH_DEVICE_CAPACITY)
        {
            error = cEcInvalidParameter;
            break;
        }

        rSpiBridge.CMD_DPNDNT = byteAddr;
        IssueCommand(sflashVariables.pDevicePtr->writeEnable, 0, 0, 0);
        IssueCommand(sflashVariables.pDevicePtr->bulkErase, 0, 0, 3);

        if (WaitWhileBusy(SECTOR_ERASE_TIMEOUT_US) != cEcNoError)
        {
            error = cEcError;
            break;
        }
        sectorAddr += SFLASH_SECTOR_PER_BLOCK;
    }
    END_MULTI_CS2(cSpinLockSFlash)

    return (error);
}
#endif

//-----------------------------------------------------------------------------
/**
    Find flash device\n
    Issues command to serial flash to read the Manufactures ID number.\n
    The ID is compared against the current  selected device from the device table.\n

    @param[out] pFlashParam     Flash device parameter

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t FindDevice(SflashParam_t* pFlashParam)
{
    pFlashParam->pDevicePtr = &defaultDevice;

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Issue a command

    @param[in]  command         command
    @param[in]  readLength      read length
    @param[in]  readWrite       read or write (0 : write, 1 : read)
    @param[in]  commandLength   length of command
**/
//-----------------------------------------------------------------------------
void IssueCommand(uint8_t command, uint8_t readLength, uint8_t readWrite, uint8_t commandLength)
{
    DeviceCmdCtrl_t deviceCommandControl;

    deviceCommandControl.all = 0;

    deviceCommandControl.b.DV_CMD = command;
    deviceCommandControl.b.DV_RD_LEN = readLength;
    deviceCommandControl.b.DV_RD_W = readWrite;
    deviceCommandControl.b.DV_CMD_LEN = commandLength;
    deviceCommandControl.b.EXT_DV_SEL = 0;

    rSpiBridge.deviceCmdCtrl.all = deviceCommandControl.all;
}

//-----------------------------------------------------------------------------
/**
    Wait While Busy.

    @param[in]  timeOutUsec     wait time out

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t WaitWhileBusy(uint32_t timeOutUsec)
{
    HwTimerTick_t tick;

    tick = HalTimer_ReadTick();

    while (HalTimer_GetElapsedTimeUsec(tick) <= timeOutUsec)
    {
        IssueCommand( sflashVariables.pDevicePtr->readStatusReg, 0, 1, 0);

        if ((rSpiBridge.DV_CMD_RT_DATA & WIP) == 0)
        {
            return(cEcNoError);
        }
    }
    return(cEcTimeOut);
}

#if CHK_CODE_TYPE(CODE_MAIN)

//-----------------------------------------------------------------------------
/**
    Page Write

    @param[in]  address     target address
    @param[in]  length      length in bytes but must be multiple of 4
**/
//-----------------------------------------------------------------------------
Error_t PageWrite(uint32_t* pBuffer, uint32_t baseAddress, uint32_t length)
{
    Error_t  error = cEcNoError;
    uint32_t offset;
    uint32_t loopCount;
    uint32_t unitSize = 4;
    uint32_t targetAddr;

    if ((baseAddress + length) > SFLASH_DEVICE_CAPACITY)
    {
        return(cEcInvalidParameter);
    }

    if (length > SFLASH_PAGE_SIZE)
    {
        return(cEcInvalidParameter);
    }

    BEGIN_MULTI_CS1(cSpinLockSFlash)
    loopCount = length / unitSize;

    IssueCommand(sflashVariables.pDevicePtr->writeEnable, 0, 0, 0);

    targetAddr = baseAddress + SPI_BASE_ADDRESS;

    for (offset = 0; offset < loopCount; offset++, targetAddr+=unitSize )
    {
        REG_U32(targetAddr) = pBuffer[offset];
    }

    while(rSpiBridge.spiIntr.b.SPI_W_FIFO_INT != 1);
    rSpiBridge.spiIntr.b.SPI_W_FIFO_INT = 1;    // clear
    error = WaitWhileBusy(PAGE_WRITE_TIMEOUT_US);
    END_MULTI_CS1(cSpinLockSFlash)

    return (error);
}

void HalSflash_OverRideFTL_Sync(void)
{
    Error_t error;
    U32     currTag = DWORD_MASK;
    U8      retry, tagIdx;

    error = cEcNoError;
    retry = SFLASH_RETRY_COUNT;

    HalSflash_Copy((PVOID)&currTag, SFH_OVERRIDRFTL_START_BYTE, sizeof(U32));

    if (smOverrideFTL != currTag)
    {
        D_PRINTF("[BDR ] Sync\n");
        while(retry--)
        {
            error = HalSflash_EraseSector(SFH_OVERRIDRFTL_START, SFH_OVERRIDRFTL_COUNT, SFLASH_RETRY_COUNT);
            if (error != cEcNoError) continue;

            error = HalSflash_ProgramU32((PVOID)&smOverrideFTL, SFH_OVERRIDRFTL_START_BYTE, sizeof(U32));
            if (error != cEcNoError) continue;

            error = HalSflash_Compare((PVOID)&smOverrideFTL, SFH_OVERRIDRFTL_START_BYTE, sizeof(U32));
            if (error == cEcNoError) break;
        }
    }
}

static void HalSflash_OverRideFTL_OneTimeInit(void)
{
    MEM_CLR((PVOID)&smOverrideFTL, sizeof(U32));

    HalSflash_Copy((PVOID)&smOverrideFTL, SFH_OVERRIDRFTL_START_BYTE, sizeof(U32));
}

void HalSflash_MiscHwInfo_Flush(void)
{
    Error_t error;
    U8      retry;

    error = cEcNoError;
    retry = SFLASH_RETRY_COUNT;

    while(retry--)
    {
        error = HalSflash_EraseSector(SFH_HWINFO_START, SFH_HWINFO_COUNT, SFLASH_RETRY_COUNT);
        if (error != cEcNoError) continue;

        error = HalSflash_Program((PVOID)&smHwInfo.all[0], SFH_HWINFO_START_BYTE, sizeof(tHW_INFO));
        if (error != cEcNoError) continue;

        error = HalSflash_Compare((PVOID)&smHwInfo.all[0], SFH_HWINFO_START_BYTE, sizeof(tHW_INFO));
        if (error == cEcNoError) break;
    }
}

#endif

void HalSflash_MiscHwInfo_OneTimeInit(void)
{
    MEM_CLR((PVOID)&smHwInfo.all[0], sizeof(tHW_INFO));

    HalSflash_Copy((PVOID)&smHwInfo.all[0], SFH_HWINFO_START_BYTE, sizeof(tHW_INFO));
}

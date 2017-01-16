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
//! @brief NAND flash interface Highlevel driver codes.
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"

#include "MemIo.h"
#include "ClockTree.h"
#include "HalTimer.h"
#include "HalClock.h"
#include "HalEcu.h"
#include "HalRegConfig.h"
#include "RegNandFlashIf.h"
#include "HalGpio.h"
#include "SysInfo.h"
#include "HalNfif.h"

#include "HalNfCtrl_TSB_3DTLCG2.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Temporary definitions for code merging - will be replaced all
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void NfCtrl_ExitTm(Pna_t* pAddr);
static void NfCtrl_PreCondition(Pna_t* pAddr);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
const uint8_t gNfifTimingInfoListIndex[6] = {0, 0, 1, 2, 3, 3};

const NfTimingInfo_t gNfifTimingInfoList[4][8] =
{
    // ASYNC/ONFI_SDR
    {
        {4, 4, 4, 4, 0, {0x00, 0x00, 0x00 ,0x00 ,0x00, 0x00, 0x00, 0x00},  1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, {0, 0, 0, 0, 0, 0}},  ///< MODE#0,10MT,FC_CLK=100MHz
        {0},
    },
    // ONFI_NVDDR
    {
        {0},
        {0},
        {0},
        {0},
        {0, 0, 3, 4, 6, {0x0C, 0x0C, 0x0C ,0x0C ,0x0C, 0x0C, 0x0C, 0x0C},  1, 0, 1, 0, 3, 0, 9, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0}},  ///< MODE#4,166MT,FC_CLK=166MHz
        {0, 0, 4, 4, 5, {0x0C, 0x0C, 0x0C ,0x0C ,0x0C, 0x0C, 0x0C, 0x0C},  1, 0, 1, 0, 3, 0, 9, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0}},  ///< MODE#5,200MT,FC_CLK=200MHz
    },
    // JEDEC_TOGGLE1
    {
        {0},
        {0},
        {0},
        {0, 0, 1, 1, 0, {0x09, 0x0C, 0x0A ,0x09 ,0x0A, 0x0C, 0x0C, 0x09},  9, 9, 0, 0, 1, 0, 9, 1, 3, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0}},  ///< MODE#3,133MT,FC_CLK=133MHz
        {0, 0, 2, 1, 0, {0x09, 0x0C, 0x0A ,0x09 ,0x0A, 0x0C, 0x0C, 0x09},  9, 9, 0, 0, 1, 0, 9, 1, 3, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0}},  ///< MODE#4,166MT,FC_CLK=166MHz
        {0, 0, 2, 2, 0, {0x0A, 0x0A, 0x0A ,0x0A ,0x10, 0x0A, 0x0A, 0x0A},  9, 9, 0, 0, 1, 0, 9, 1, 3, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0}},  ///< MODE#5,200MT,FC_CLK=200MHz
    },
    // JEDEC_TOGGLE2/ONFI_NVDDR2
    {
        {0},
        {0},
        {0},
        {0},
        {0, 0, 2, 2, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  9, 9, 0, 0, 0, 0, 9, 0, 3, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0}},  ///< MODE#4,200MT,FC_CLK=100MHz
        {0, 0, 1, 1, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  9, 9, 0, 0, 0, 0, 9, 0, 3, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0}},  ///< MODE#5,266MT,FC_CLK=133MHz
#if (TOGGLE_533 == TRUE)
        {0, 0, 4, 4, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 15, 5, 0, 0, 0, 0, 9, 1, 3, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0}},  ///< MODE#6,333MT,FC_CLK=166MHz
        {0, 0, 4, 4, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 15, 5, 0, 0, 0, 0, 9, 1, 3, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0}},  ///< MODE#7,400MT,FC_CLK=200MHz
#else
        {0, 0, 6, 6, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 15, 5, 0, 0, 0, 0, 9, 1, 3, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0}},  ///< MODE#6,333MT,FC_CLK=166MHz
        {0, 0, 6, 6, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 15, 5, 0, 0, 0, 0, 9, 1, 3, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0}},  ///< MODE#7,400MT,FC_CLK=200MHz
#endif
        //{0, 0, 2, 2, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  9, 9, 0, 0, 0, 0, 9, 1, 3, 0, 0, 0, 0},  ///< MODE#7,400MT,FC_CLK=200MHz
    },
};
//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_WaitForStatusSetIm(Pna_t* pAddr, uint32_t value, uint32_t timeOutUsec);
Error_t HalNfCtrl_WaitForStatusSetDm(Pna_t* pAddr, uint32_t value, uint32_t timeOutUsec);
//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    NAND flash NfifCtrl Initial.

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_OneTimeInit(void)
{
    //--- Read
    // WCNT61 + PCNT52
    mNandFlashIf.flashSeqInterfacTmCtrl5.b.PCNT52               = 1;
    mNandFlashIf.flashSeqInterfacTmCtrl5.b.DIV5                 = 2;
    //--- Write
    // WCNT31 + PCNT51
    mNandFlashIf.flashSeqInterfacTmCtrl5.b.PCNT51               = 0x0F;
    mNandFlashIf.flashSeqInterfacTmCtrl3.b.WCNT31               = 0x3C;

    //---Voltage
    //WCNT71 160ns
    mNandFlashIf.flashSeqInterfacTmCtrl7.b.WCNT71               = 0x00;
    mNandFlashIf.flashSeqInterfacTmCtrl7.b.DIV7                 = 0x00;
    return cEcNoError;
}
//-----------------------------------------------------------------------------
/**
    NAND flash NfifCtrl UpdateTimingInfo.

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_UpdateTimingInfo(NfCfgInfo_t* pCfgInfo)
{
    MEM_COPY(&pCfgInfo->timingInfo, &gNfifTimingInfoList[gNfifTimingInfoListIndex[pCfgInfo->reqIfData]][pCfgInfo->reqXferMode], sizeof(pCfgInfo->timingInfo));

    return cEcNoError;
}
//-----------------------------------------------------------------------------
/**
    Indirect mode - NAND flash erase block operation.

    @param[in]  pAddr      Logical NAND Flash address pointer

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_EraseBlockIm(Pna_t* pAddr)
{
    Error_t error;
    Pna_t pna;

    MEM_COPY(&pna, pAddr, sizeof(Pna_t));

    HalNfif_ImSetupCmdRowAddrCount(&pna, FC_ERASE_FLASH_1ST, FC_ERASE_FLASH_2ND, 0);

    HalNfif_EnDisableWriteProtect(DISABLE);                     //    disable write protect
    HalNfif_ImExecuteCommand(IM_ERASE_GO);                      //    execute erase flash block command
    HalNfif_ImWaitForCommandStart();                            //    wait controller start

    // wait for flash to become ready, erase block done
    error = HalNfCtrl_WaitForStatusSetIm(&pna, FC_DIRINDIR_FLASH_RDY, NF_ERASE_TIME_OUT_US);

    HalNfif_EnDisableWriteProtect(ENABLE);                      //    enable write protect
    HalNfif_SetFlashOperationMode(SEQUENCER);

    return error;
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - NAND flash page write operation.

    @param[in]  pAddr      Logical NAND Flash address pointer
    @param[in]  pBuffer    write data buffer pointer
    @param[in]  count      byte count to write

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_WritePageIm(Pna_t* pAddr, void* pBuffer, uint32_t count)
{
    Error_t error;
    Pna_t pna;

    MEM_COPY(&pna, pAddr, sizeof(Pna_t));

    HalNfif_SetFlashOperationMode(INDIRECT);
    HalNfif_ImSetupCmdAddrCount(&pna, FC_PROGRAM_1ST, FC_PROGRAM_2ND, count);
    HalNfif_EnDisableWriteProtect(DISABLE);                     // disable write protect
    HalNfif_ImExecuteCommand(IM_PRG_GO);                        // execute program flash command
    HalNfif_ImWriteData((uint32_t *)pBuffer, count);                 // transfer data
    HalNfif_ImWaitForCommandStart();                            // wait controller start

    // wait for flash to become ready, erase block done
    error = HalNfCtrl_WaitForStatusSetIm(&pna, FC_DIRINDIR_FLASH_RDY, NF_WRITE_TIME_OUT_US);

    HalNfif_EnDisableWriteProtect(ENABLE);                      // enable write protect
    HalNfif_SetFlashOperationMode(SEQUENCER);                   // sequencer access mode, default mode

    return error;
}
//-----------------------------------------------------------------------------
/**
    Indirect mode - NAND flash page read operation.

    @param[in]  pAddr      Logical NAND Flash address pointer
    @param[out] pBuffer    read data buffer pointer
    @param[in]  count      byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_WritePageDm(Pna_t* pAddr, void* pBuffer, uint32_t count, uint8_t prefix)
{
    Error_t error = cEcNoError;
    U32 datasize;
    uint8_t* pBufferU8 = (uint8_t *)pBuffer;
    U8 lmu, progUnit, prefixCmd;;

    datasize = count;
    progUnit = (prefix == FC_PREFIX_SLC_BYTE) ? 1 : (gNfInfo.pagesPerProgUnit);

    HalNfif_SetFlashOperationMode(DIRECT);

    HalNfif_DmSetDeviceChannel(pAddr);

    for (lmu = 0; lmu < progUnit; lmu++)
    {
        prefixCmd = (prefix == FC_PREFIX_SLC_BYTE) ? FC_PREFIX_SLC_BYTE : (lmu + 1);
        HalNfif_DmWriteCommandU8(prefixCmd);
        HalNfif_DmWriteCommandU8(FC_PROG_BYTE0);
        HalNfif_DmWriteAddressU8(pAddr->col);
        HalNfif_DmWriteAddressU8(pAddr->col >> 8);
        HalNfif_DmWriteAddressU8(pAddr->row);
        HalNfif_DmWriteAddressU8(pAddr->row >> 8);
        HalNfif_DmWriteAddressU8(pAddr->row >> 16);
        HalTimer_DelayUsec(NF_tADL_USEC);
        HalNfif_DmWriteToggleData(0x00, DISABLE);
        HalTimer_DelayUsec(NF_tADL_USEC);

        datasize  = count;
        pBufferU8 = (uint8_t *)pBuffer;
        while (datasize--)
        {
            HalNfif_DmWriteToggleData(*pBufferU8++, ENABLE);
            HalNfif_DmWriteToggleData(*pBufferU8++, DISABLE);
        }
        HalTimer_DelayUsec(NF_tFEAT_USEC);

        if (prefixCmd == 0x3 || prefixCmd == FC_PREFIX_SLC_BYTE)
        {
            HalNfif_DmWriteCommandU8(FC_PROG_BYTE1);
            error = HalNfCtrl_WaitForStatusSetDm(pAddr, FC_STATUS_RDY, NF_WRITE_TIME_OUT_US);
        }
        else
        {
            HalNfif_DmWriteCommandU8(FC_PROG_BYTE5);
            HalTimer_DelayUsec(50);
        }
    }

    HalNfif_DmSetDeviceChannel(pAddr);
    HalNfif_DmWaitReToDqsDelay();
    HalNfif_DmBackToDefault();

    Debug_DumpMemU32((U32)pBuffer, 32);
    HalNfif_SetFlashOperationMode(SEQUENCER);

    return (error);
}
//-----------------------------------------------------------------------------
/**
    Indirect mode - NAND flash page read operation.

    @param[in]  pAddr      Logical NAND Flash address pointer
    @param[out] pBuffer    read data buffer pointer
    @param[in]  count      byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_ReadPageIm(Pna_t* pAddr, void* pBuffer, uint32_t count)
{
    Error_t error;
    Pna_t pna;

    MEM_COPY(&pna, pAddr, sizeof(Pna_t));

    HalNfif_SetFlashOperationMode(INDIRECT);

    // set command for reading flash data with random data output, 1st sequence
    HalNfif_ImSetupCmdAddrCount(&pna, FC_READ_1ST, FC_READ_2ND, 0);
    HalNfif_ImExecuteCommand(IM_RND_DO_1ST_GO);                 // execute read flash data command, first sequence
    HalNfif_ImWaitForCommandStart();                            // wait controller start

    // wait for flash to become ready
    error = HalNfCtrl_WaitForStatusSetIm(&pna, FC_DIRINDIR_FLASH_RDY, NF_READ_TIME_OUT_US);

    // random data output
    HalNfif_ImSetupCmdAddrCount(&pna, FC_READ_1ST, 0x00, count);
    HalNfif_ImExecuteCommand(IM_RD_SOUT_GO);                    // execute read flash data command, second sequence

    HalNfif_ImReadData((uint32_t *)pBuffer, count);                  // read data

    HalNfif_SetFlashOperationMode(SEQUENCER);

    return (error);
}

//-----------------------------------------------------------------------------
/**
    Direct mode - NAND flash page read operation.

    @param[in]  pAddr      Logical NAND Flash address pointer
    @param[out] pBuffer    read data buffer pointer
    @param[in]  count      byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_ReadPageDm(Pna_t* pAddr, void* pBuffer, uint32_t count, uint8_t prefix)
{
    Error_t error;
    uint8_t* pBufferU8 = (uint8_t *)pBuffer;

    pAddr->row = (pAddr->row & 0xFFFFFF00) | (pAddr->row & 0x000000FF);

    HalNfif_SetFlashOperationMode(DIRECT);

    HalNfif_DmSetDeviceChannel(pAddr);
    HalNfif_DmWriteCommandU8(prefix + 1);
    HalNfif_DmWriteCommandU8(FC_READ_BYTE0);
    HalNfif_DmWriteAddressU8(pAddr->col);
    HalNfif_DmWriteAddressU8(pAddr->col >> 8);
    HalNfif_DmWriteAddressU8(pAddr->row);
    HalNfif_DmWriteAddressU8(pAddr->row >> 8);
    HalNfif_DmWriteAddressU8(pAddr->row >> 16);
    HalNfif_DmWriteCommandU8(FC_READ_BYTE1);

    HalTimer_DelayUsec(NF_tWHR_USEC);

    error = HalNfCtrl_WaitForStatusSetDm(pAddr, FC_STATUS_RDY, 1000);

    HalNfif_DmSetDeviceChannel(pAddr);

    HalNfif_DmWriteCommandU8(FC_READ_BYTE8);
    HalNfif_DmWriteAddressU8(pAddr->col);
    HalNfif_DmWriteAddressU8(pAddr->col >> 8);
    HalNfif_DmWriteAddressU8(pAddr->row);
    HalNfif_DmWriteAddressU8(pAddr->row >> 8);
    HalNfif_DmWriteAddressU8(pAddr->row >> 16);
    HalNfif_DmWriteCommandU8(FC_READ_BYTE5);

    HalTimer_DelayUsec(NF_tWHR_USEC);

    HalNfif_DmWaitReToDqsDelay();

    while (count--)
    {
        *pBufferU8++ = (uint8_t)HalNfif_DmReadToggleDataU16();
    }

    HalNfif_DmBackToDefault();

    Debug_DumpMemU32((U32)pBuffer, gNfInfo.dataBytesPerFullPage);
    HalNfif_SetFlashOperationMode(SEQUENCER);

    return (error);
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - Read a status

    @param[in]  pAddr    Phyiscal NAND Flash address pointer
                pAddr->ch    channel number
                pAddr->dev  device number

    @return value of NAND flash status register
**/
//-----------------------------------------------------------------------------
uint8_t HalNfCtrl_ReadStatusIm(Pna_t* pAddr)
{
    HalNfif_ImSetupCmdAddrCount(pAddr, FC_READ_STATUS, 0, 2);

    HalNfif_ImExecuteCommand(IM_RD_STATUS_GO);

    HalNfif_ImWaitForCommandStart();
    HalNfif_ImWaitForFifoReady();

    return HalNfif_ImReadDataU32();
}
//-----------------------------------------------------------------------------
/**
    Direct mode - Read a status

    @param[in]  pAddr    Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number

    @return NAND flash status
**/
//-----------------------------------------------------------------------------
uint8_t HalNfCtrl_ReadStatusDm(Pna_t* pAddr)
{
    uint8_t status;

    HalNfif_DmSetDeviceChannel(pAddr);
    HalNfif_DmWriteCommandU8(FC_READ_STATUS);

    HalTimer_DelayUsec(NF_tWHR_USEC);

    status = (uint8_t)HalNfif_DmReadDataU16();

    HalNfif_DmBackToDefault();

    return status;
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - Reset a NAND flash.

    @param[in]  pAddr    Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
    @param[in]  resetCmd   reset command (FFh=reset, FAh=reset LUN, FC=sync reset)

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_ResetIm(Pna_t* pAddr, uint8_t resetCmd)
{
    uint16_t cmd;

    HalNfif_ImSetupCmdAddrCount(pAddr, resetCmd, 0, 0);

    cmd = (resetCmd == FC_RESET_LUN) ? IM_RESET_LUN_GO : IM_RESET_GO;
    HalNfif_ImExecuteCommand(cmd);

    HalNfif_ImWaitForCommandStart();

    HalNfif_SetFlashOperationMode(SEQUENCER);

    return (cEcNoError);
}
//-----------------------------------------------------------------------------
/**
    Direct mode - Reset a NAND flash.

    @param[in]  pAddr    Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
    @param[in]  resetCmd   reset command

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_ResetDm(Pna_t* pAddr, uint8_t resetCmd)
{
    HalNfif_DmSetDeviceChannel(pAddr);
    HalNfif_DmWriteCommandU8(resetCmd);

    HalNfif_DmBackToDefault();

    return cEcNoError;
}
//-----------------------------------------------------------------------------
/**
    Indirect mode - Get 4bytes feature parameter.\n

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
                pAddr->row      feature address
    @param[out] param      4byte feature parameters

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_GetFeatureIm(Pna_t* pAddr, uint8_t param[])
{
    uint32_t count = 4;

    HalNfif_ImSetupCmdRowAddrCount(pAddr, FC_GET_FEATURES, 0, count);

    HalNfif_ImExecuteCommand(IM_GET_FEATURE_GO);               // execute set feature command
    HalTimer_DelayUsec(NF_tFEAT_USEC);                // delay tFEAT
    HalNfif_ImSetDummyReady();

    HalNfif_ImReadData((uint32_t *)param, count);

    HalNfif_SetFlashOperationMode(SEQUENCER);                  // sequencer access mode, default mode

    return (cEcNoError);
}
//-----------------------------------------------------------------------------
/**
    Indirect mode - Get 4byte feature parameter.\n

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch       channel number
                pAddr->dev      device number
                pAddr->row      feature address
    @param[out] param      4byte feature parameters

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_GetFeatureDm(Pna_t* pAddr, uint8_t param[])
{
    HalNfif_DmSetDeviceChannel(pAddr);
    HalNfif_DmWriteCommandU8(FC_GET_FEATURES);
    HalNfif_DmWriteAddressU8(pAddr->row);

    HalTimer_DelayUsec(NF_tFEAT_USEC);

    if ((gCfgInfo.ifType == NF_JEDEC) && (gCfgInfo.curIfData >= NF_JEDEC_TOGGLE1))
    {
        uint8_t count;
        HalNfif_DmWaitReToDqsDelay();
        for(count = 0; count < 4; count++)
        {
            param[count] = (uint8_t)HalNfif_DmReadToggleDataU16();
            HalNfif_DmReadToggleDataU16();    // Dummy read
        }
    }
    else
    {
        param[0] = (uint8_t)HalNfif_DmReadDataU16();
        param[1] = (uint8_t)HalNfif_DmReadDataU16();
        param[2] = (uint8_t)HalNfif_DmReadDataU16();
        param[3] = (uint8_t)HalNfif_DmReadDataU16();
    }
    HalNfif_DmBackToDefault();

    return (cEcNoError);
}
//-----------------------------------------------------------------------------
/**
    Indirect mode - Set 4bytes feature parameter.\n

    @param[in]  pAddr    Phyiscal NAND Flash address pointer
                pAddr->ch       channel number
                pAddr->dev      device number
                pAddr->row      feature address
    @param[in]  param    4byte feature parameters

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_SetFeatureIm(Pna_t* pAddr, uint8_t param[])
{
    Error_t error;
    uint32_t count = 4;

    HalNfif_ImSetupCmdRowAddrCount(pAddr, FC_SET_FEATURES, 0, count);

    HalNfif_ImExecuteCommand(IM_SET_FEATURE_GO);             // execute set feature command

    error = HalNfif_ImWriteData((uint32_t *)param, count);

    HalTimer_DelayUsec(NF_tFEAT_USEC);              // delay tFEAT

    HalNfif_SetFlashOperationMode(SEQUENCER);                // sequencer access mode, default mode

    return (error);
}
//-----------------------------------------------------------------------------
/**
    Direct mode - Set 4byte feature parameter.\n

    @param[in]  pAddr    Phyiscal NAND Flash address pointer
                pAddr->ch       channel number
                pAddr->dev      device number
                pAddr->row      feature address
    @param[in]  param    4byte feature parameters

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_SetFeatureDm(Pna_t* pAddr, uint8_t param[])
{
    HalNfif_DmSetDeviceChannel(pAddr);
    HalNfif_DmWriteCommandU8(FC_SET_FEATURES);
    HalNfif_DmWriteAddressU8(pAddr->row);
    HalTimer_DelayUsec(NF_tADL_USEC);

    if((gCfgInfo.ifType == NF_JEDEC) && (gCfgInfo.curIfData >= NF_JEDEC_TOGGLE1))
    {
        HalNfif_DmWriteToggleDataU16(param[0]);
        HalNfif_DmWriteToggleDataU16(param[1]);
        HalNfif_DmWriteToggleDataU16(param[2]);
        HalNfif_DmWriteToggleDataU16(param[3]);
        HalNfif_DmWriteToggleDataU16(0);    // Toggling RE_N

        HalTimer_DelayUsec(NF_tFEAT_USEC);
    }
    else
    {
        HalTimer_DelayUsec(NF_tFEAT_USEC);

        HalNfif_DmWriteDataU16(param[0]);
        HalNfif_DmWriteDataU16(param[1]);
        HalNfif_DmWriteDataU16(param[2]);
        HalNfif_DmWriteDataU16(param[3]);
    }
    HalNfif_DmBackToDefault();

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Direct mode - Get 4byte GetVoltageOffset.\n

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch       channel number
                pAddr->dev      device number
                pAddr->row      feature address
    @param[out] param      4byte feature parameters

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_GetVoltageOffsetDm(Pna_t* pAddr, uint8_t featureAddr, uint8_t param[])
{
    HalNfif_DmSetDeviceChannel(pAddr);
    HalNfif_DmWriteCommandU8(0xD4);
    HalNfif_DmWriteAddressU8((pAddr->row >> gNfInfo.lunLsbPosition) & 0xFF);
    HalNfif_DmWriteAddressU8(featureAddr & 0xFF);

    HalTimer_DelayUsec(NF_tFEAT_USEC);

    if ((gCfgInfo.ifType == NF_JEDEC) && (gCfgInfo.curIfData >= NF_JEDEC_TOGGLE1))
    {
        uint8_t count;
        HalNfif_DmWaitReToDqsDelay();
        for(count = 0; count < 4; count++)
        {
            param[count] = (uint8_t)HalNfif_DmReadToggleDataU16();
            HalNfif_DmReadToggleDataU16();    // Dummy read
        }
    }
    else
    {
        param[0] = (uint8_t)HalNfif_DmReadDataU16();
        param[1] = (uint8_t)HalNfif_DmReadDataU16();
        param[2] = (uint8_t)HalNfif_DmReadDataU16();
        param[3] = (uint8_t)HalNfif_DmReadDataU16();
    }
    HalNfif_DmBackToDefault();

    return (cEcNoError);
}
//-----------------------------------------------------------------------------
/**
    Direct mode - Set 4byte feature parameter.\n

    @param[in]  pAddr    Phyiscal NAND Flash address pointer
                pAddr->ch       channel number
                pAddr->dev      device number
                pAddr->row      feature address
    @param[in]  param    4byte feature parameters

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_SetVoltageOffsetDm(Pna_t* pAddr, uint8_t featureAddr, uint8_t param[])
{
    HalNfif_DmSetDeviceChannel(pAddr);
    HalNfif_DmWriteCommandU8(0xD5);
    HalNfif_DmWriteAddressU8((pAddr->row >> gNfInfo.lunLsbPosition) & 0xFF);
    HalNfif_DmWriteAddressU8(featureAddr & 0xFF);

    if((gCfgInfo.ifType == NF_JEDEC) && (gCfgInfo.curIfData >= NF_JEDEC_TOGGLE1))
    {
        HalNfif_DmWriteToggleDataU16(param[0]);
        HalNfif_DmWriteToggleDataU16(param[1]);
        HalNfif_DmWriteToggleDataU16(param[2]);
        HalNfif_DmWriteToggleDataU16(param[3]);
        HalNfif_DmWriteToggleDataU16(0);    // Toggling RE_N

        HalTimer_DelayUsec(NF_tFEAT_USEC);
    }
    else
    {
        HalTimer_DelayUsec(NF_tFEAT_USEC);

        HalNfif_DmWriteDataU16(param[0]);
        HalNfif_DmWriteDataU16(param[1]);
        HalNfif_DmWriteDataU16(param[2]);
        HalNfif_DmWriteDataU16(param[3]);
    }
    HalNfif_DmBackToDefault();

    return (cEcNoError);
}
//-----------------------------------------------------------------------------
/**
    Indirect mode - Read parameter page.\n

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch       channel number
                pAddr->dev      device number
                pAddr->row      parameter page address (00h=ONFI, 40h=JEDEC)
    @param[out] pBuffer    Parameter buffer pointer
    @param[in]  count      byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_ReadParameterPageIm(Pna_t* pAddr, void* pBuffer, uint32_t count)
{
    HalNfif_ImSetupCmdRowAddrCount(pAddr, FC_READ_PARAMETER_PAGE, 0, count);

    HalNfif_ImExecuteCommand(IM_RD_PARAMETER_PAGE_GO);    // execute read parameter command
    HalTimer_DelayUsec(NF_tR_USEC);
    HalNfif_ImSetDummyReady();

    if(gCfgInfo.ifType == NF_JEDEC)
    {
        HalNfif_ImReadDataDdr((uint8_t *)pBuffer, count);
    }
    else
    {
        HalNfif_ImReadData((uint32_t *)pBuffer, count);
    }

    HalNfif_SetFlashOperationMode(SEQUENCER);

    return (cEcNoError);
}
//-----------------------------------------------------------------------------
/**
    Direct mode - Read parameter page.\n

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
                pAddr->row      parameter page address (00h=ONFI, 40h=JEDEC)
    @param[out] pBuffer    Parameter buffer pointer
    @param[in]  count      byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_ReadParameterPageDm(Pna_t* pAddr, void* pBuffer, uint32_t count)
{
    Error_t  error     = cEcNoError;
    uint8_t* pBufferU8 = (uint8_t *)pBuffer;
    uint16_t (*pfDmReadData)();
    #if CHK_FW_TYPE(FW_AGING)
    uint8_t  temp_Data[4];
    uint8_t  temp;
    MEM_CLR(temp_Data, 4);
    #endif

    HalNfif_DmSetDeviceChannel(pAddr);
    HalNfif_DmWriteCommandU8(FC_READ_PARAMETER_PAGE);
    HalNfif_DmWriteAddressU8(pAddr->row);

    HalTimer_DelayUsec(NF_tR_USEC);

    if(gCfgInfo.ifType == NF_JEDEC)
    {
        HalNfif_DmWaitReToDqsDelay();
        pfDmReadData = &HalNfif_DmReadToggleDataU16;
    }
    else
    {
        pfDmReadData = &HalNfif_DmReadDataU16;
    }

    while (count--)
    {
        *pBufferU8++ = (uint8_t)pfDmReadData();
    }

    HalNfif_DmBackToDefault();

    #if CHK_FW_TYPE(FW_AGING)
    // TSB Initial  2015/10/01 Frank
    pBufferU8 = (U8 *)pBuffer;
    MEM_COPY(temp_Data, pBufferU8, 2);
    MEM_COPY((temp_Data + 2), (pBufferU8 + 510), 2);

    *(pBufferU8) ^= 0x4F;
    *(pBufferU8 + 1) ^= 0x4E;
    temp = *(pBufferU8 + 510);
    *(pBufferU8 + 510)= *(pBufferU8 + 511);
    *(pBufferU8 + 511) = temp;

    if(!(CRC_Calculate(0x0000, pBufferU8, 512)))
    {
        error = cEcNoError;
        D_PRINTF("[RDPP] PASS \n");
    }
    else
    {
        error = cEcError;
        D_PRINTF("[RDPP] FAIL \n");
    }
    MEM_COPY(pBufferU8, temp_Data, 2);
    MEM_COPY((pBufferU8 + 510), (temp_Data + 2), 2);
    #endif

    return error;
}
//-----------------------------------------------------------------------------
/**
    Indirect mode - Read flash id.\n

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
                pAddr->row      id address (00h=manufacturer id, 20h=ONFI, 40h=JEDEC signature)
    @param[out] pBuffer    Id data pointer
    @param[in]  count      id byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_ReadIdIm(Pna_t* pAddr, void* pBuffer, uint32_t count)
{
    Error_t error;

    HalNfif_ImSetupCmdRowAddrCount(pAddr, FC_READ_ID, 0, count);

    HalNfif_ImExecuteCommand(IM_RD_ID_GO);        // execute read parameter command
    HalNfif_ImWaitForCommandStart();              // wait controller start

    if(gCfgInfo.ifType == NF_JEDEC)
    {
        error = HalNfif_ImReadDataDdr((uint8_t *)pBuffer, count);
    }
    else
    {
        error = HalNfif_ImReadData((uint32_t *)pBuffer, count);
    }

    HalNfif_SetFlashOperationMode(SEQUENCER);

    return (error);
}
//-----------------------------------------------------------------------------
/**
    Direct mode - NAND flash read Id.

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
                pAddr->row      id address (00h=manufacturer id, 20h=ONFI, 40h=JEDEC signature)
    @param[out] pBuffer    Id data pointer
    @param[in]  count      id byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_ReadIdDm(Pna_t* pAddr, void* pBuffer, uint32_t count)
{
    uint8_t* pBufferU8 = (uint8_t *)pBuffer;

    HalNfif_DmSetDeviceChannel(pAddr);
    HalNfif_DmWriteCommandU8(FC_READ_ID);
    HalNfif_DmWriteAddressU8(pAddr->row);

    HalTimer_DelayUsec(NF_tWHR_USEC);

    while (count--)
    {
        *pBufferU8++ = (uint8_t)HalNfif_DmReadDataU16();
    }

    HalNfif_DmBackToDefault();

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Test mode - NAND flash read Id.

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
                pAddr->row      id address (00h=manufacturer id, 20h=ONFI, 40h=JEDEC signature)
    @param[out] pBuffer    Id data pointer
    @param[in]  count      id byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_ReadIdTm(Pna_t* pAddr, void* pBuffer, uint32_t count)
{
    uint8_t* pBufferU8 = (uint8_t *)pBuffer;
    U8 tmpOutput;
    U8 lunCmd;

    lunCmd = 0xF0 + ((pAddr->row >> (gNfInfo.bitsBlocksPerLun + gNfInfo.bitsPagesPerBlock) & 0xFF)+1);

    NfCtrl_PreCondition(pAddr);

    HalNfif_DmSetDeviceChannel(pAddr);
    HalNfif_DmWriteCommandU8(lunCmd);
    HalNfif_DmWriteCommandU8(0x56);
    HalNfif_DmWriteAddressU8(0x01);
    HalNfif_DmWriteDataU16(0x06);
    HalNfif_DmWriteCommandU8(0x5F);
    HalNfif_DmWriteCommandU8(0x00);
    HalNfif_DmWriteAddressU8(0x9F);
    HalTimer_DelayMsec(1);
    HalNfif_DmWriteCommandU8(0x5F);
    HalTimer_DelayUsec(NF_tWHR_USEC);
    HalNfif_DmWaitReToDqsDelay();
    tmpOutput = (uint8_t)HalNfif_DmReadDataU16();

    HalNfif_DmWriteCommandU8(0x5F);
    HalNfif_DmWriteCommandU8(0x00);
    HalNfif_DmWriteCommandU8(0x5F);
    HalNfif_DmWriteCommandU8(0x5F);
    if (tmpOutput & 0x20)  /// [IO:5]-CS1:1 / CS2:0
    {
        count = 0x7;
        HalNfif_DmSetDeviceChannel(pAddr);
        HalNfif_DmWriteCommandU8(lunCmd);
        HalNfif_DmWriteCommandU8(0x56);
        HalNfif_DmWriteAddressU8(0xF7);
        HalNfif_DmWriteDataU16(0x36);
        HalTimer_DelayMsec(1);

        HalNfif_DmSetDeviceChannel(pAddr);
        HalNfif_DmWriteCommandU8(FC_READ_ID);
        HalNfif_DmWriteAddressU8(0x00);
        HalTimer_DelayUsec(NF_tWHR_USEC);
        HalNfif_DmWaitReToDqsDelay();
        while (count--)
        {
            *pBufferU8++ = (uint8_t)HalNfif_DmReadDataU16();
        }
        *pBufferU8 = tmpOutput;
        NfCtrl_ExitTm(pAddr);
    }
    else
    {
        count = 0x7;
        NfCtrl_ExitTm(pAddr);
        HalNfif_DmSetDeviceChannel(pAddr);
        HalNfif_DmWriteCommandU8(FC_READ_ID);
        HalNfif_DmWriteAddressU8(0x00);
        HalTimer_DelayUsec(NF_tWHR_USEC);
        HalNfif_DmWaitReToDqsDelay();
        while (count--)
        {
            *pBufferU8++ = (uint8_t)HalNfif_DmReadDataU16();
        }
        *pBufferU8 = tmpOutput;
    }
    HalNfif_DmBackToDefault();

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - Wait for ready status.

    @param[in]  pAddr       Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
    @param[in]  value       bit value
    @param[in]  timeOutUsec time out usec

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_WaitForStatusSetIm(Pna_t* pAddr, uint32_t value, uint32_t timeOutUsec)
{
    U8 temp8;
    HwTimerTick_t tick;

    tick = HalTimer_ReadTick();
    while (HalTimer_GetElapsedTimeUsec(tick)<=timeOutUsec)
    {
        temp8 = HalNfif_ReadStatus(pAddr, INDIRECT);
        if (temp8 == (U8) value || temp8 == 0x60)
            return(cEcNoError);
    }

    return cEcTimeOut;
}
//-----------------------------------------------------------------------------
/**
    Direct mode - Wait for ready status.

    @param[in]  pAddr       Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
    @param[in]  value       bit value
    @param[in]  timeOutUsec time out usec

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfCtrl_WaitForStatusSetDm(Pna_t* pAddr, uint32_t value, uint32_t timeOutUsec)
{
    HwTimerTick_t tick;

    tick = HalTimer_ReadTick();

    while (HalTimer_GetElapsedTimeUsec(tick)<=timeOutUsec)
    {
        if (HalNfif_ReadStatus((pAddr), DIRECT) & value)
        {
            return(cEcNoError);
        }
    }

    return cEcTimeOut;
}

//-----------------------------------------------------------------------------
/**
    Test mode - Exit NandFlash TestMode.

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
                pAddr->row      id address (00h=manufacturer id, 20h=ONFI, 40h=JEDEC signature)
    @param[out] pBuffer    Id data pointer
    @param[in]  count      id byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
static void NfCtrl_ExitTm(Pna_t* pAddr)
{
    U8 lunCmd;
    lunCmd = 0xF0 + ((pAddr->row >> (gNfInfo.bitsBlocksPerLun + gNfInfo.bitsPagesPerBlock) & 0xFF)+1);

    HalNfif_SetFlashOperationMode(DIRECT);

    HalNfif_DmSetDeviceChannel(pAddr);
    HalNfif_DmWriteCommandU8(lunCmd);
    HalNfif_DmWriteCommandU8(0x56);
    HalNfif_DmWriteAddressU8(0x01);
    HalNfif_DmWriteDataU16(0x00);

    HalNfif_DmWriteCommandU8(lunCmd);
    HalNfif_DmWriteCommandU8(0x56);
    HalNfif_DmWriteAddressU8(0x00);
    HalNfif_DmWriteDataU16(0x00);
    HalTimer_DelayUsec(NF_tRST_USEC);
    HalNfif_DmWriteCommandU8(0xFF);
    HalTimer_DelayUsec(NF_tRST_USEC);
    HalNfif_DmBackToDefault();
    HalNfif_SetFlashOperationMode(SEQUENCER);
}

//-----------------------------------------------------------------------------
/**
    Test mode - Exit NandFlash TestMode.

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
                pAddr->row      id address (00h=manufacturer id, 20h=ONFI, 40h=JEDEC signature)
    @param[out] pBuffer    Id data pointer
    @param[in]  count      id byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
static void NfCtrl_PreCondition(Pna_t* pAddr)
{
    U8 lunCmd;

    lunCmd = 0xF0 + ((pAddr->row>>(gNfInfo.bitsBlocksPerLun + gNfInfo.bitsPagesPerBlock) & 0xFF) + 1);

    HalNfif_SetFlashOperationMode(DIRECT);
    HalNfif_DmSetDeviceChannel(pAddr);
    HalNfif_DmWriteCommandU8(0x5C);
    HalNfif_DmWriteCommandU8(0xC5);
    HalNfif_DmWriteCommandU8(lunCmd);
    HalNfif_DmWriteCommandU8(0x56);
    HalNfif_DmWriteAddressU8(0x00);
    HalNfif_DmWriteDataU16(0x01);

    HalNfif_DmBackToDefault();
    HalNfif_SetFlashOperationMode(SEQUENCER);
}

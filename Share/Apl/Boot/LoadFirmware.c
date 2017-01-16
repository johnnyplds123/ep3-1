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
//! @brief Firmware loader and burner.
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "LoadFirmware.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define NEEDPRINT(b, F, args...)        if((b)!=0) { M_PRINTF(F, ##args); }

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
DfwBlkHeader_t      gLdFw_hdr;
DfwSelectTag_t      gLdFw_selinfo;
DfwDescript_t       gLdFw_desc;
DfwModuleInfo_t     gLdFw_module;
DfwImageTag_t       gLdFw_ImgTag[FW_MAIN_BK_NUM];
DfwInfo_t           gLdFw_FwInfo[FW_MAIN_BK_NUM];
//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
static fwError_t LoadFirmware_Verify(fwError_t mode, uint32_t* srcAddress, uint32_t* desAddress, uint32_t* size);
static void      LoadFirmware_InsertFwList(DfwInfo_t* pfwInfo, uint8_t prevNode, uint8_t nextNode);
static void      LoadFirmware_InitFwList(DfwInfo_t* pfwInfo);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Load Firmware code and data from non-volatile memory (serial flash).

        All codes are loaded by SoC boot ROM, so this code is not needed now.
        For the DRAM code and data load, we have to implement this loader API.
**/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    LoadFirmware onetime init function

    @param[in]  initMode
**/
//-----------------------------------------------------------------------------
void AplLoadFirmware_GetFwInfo(uint32_t sflashAddress, uint8_t fwSetValid, DfwInfo_t* pfwInfo)
{
    uint8_t  fw_num = 0;
    uint8_t  fwBlkCnt;

    HalSflash_Copy(&gLdFw_desc, sflashAddress + DFW_FW_SELECT_SIZE, DFW_FW_DESC_SIZE);
    sflashAddress = sflashAddress + DFW_FW_SELECT_SIZE + DFW_FW_DESC_SIZE;

    for (fwBlkCnt = 0; fwBlkCnt < gLdFw_desc.sec_cnt; fwBlkCnt++)
    {
        HalSflash_Copy(&gLdFw_module, sflashAddress, sizeof(DfwModuleInfo_t));

        if ((gLdFw_module.tag & 0xFFFF) == DFW_FW_MAFW_TAG)
        {
            if (gLdFw_module.tag == DFW_FW_BKFW_TAG)
            {
                fw_num = FW_MAIN_BK_VALID;
            }
            else
            {
                fw_num = DFW_GET_FWNUM(gLdFw_module) - 1;
            }
            // critical! beacause when access address out of range, ASM code will be broken
            if (fw_num < FW_MAIN_BK_NUM)
            {
                pfwInfo[fw_num].address = gLdFw_module.offset;
                pfwInfo[fw_num].size    = gLdFw_module.size;
                HalSflash_Copy(&gLdFw_ImgTag[fw_num], gLdFw_module.offset, sizeof(DfwImageTag_t));

                if (gLdFw_ImgTag[fw_num].id == DFW_FW_IMAGE_TAG && fwSetValid == TRUE)
                {
                    BIT_SET(smFwValidStatus, BIT(fw_num));
                }
            }
        }

        sflashAddress += sizeof(DfwModuleInfo_t);
    }
}

#if CHK_CODE_TYPE(CODE_MAIN)
//-----------------------------------------------------------------------------
/**
    LoadFirmware onetime init function

    @param[in]  initMode
**/
//-----------------------------------------------------------------------------
Error_t AplLoadFirmware_OneTimeInit(InitBootMode_t initMode)
{
    uint8_t* pBuffer;
    Error_t  error = cEcNoError;
    fwError_t fwStatus = FW_NO_ERR;
    uint32_t sfSrcAddress = 0;
    uint32_t sfDesAddress = 0;
    uint32_t sfSize = 0;
    uint8_t  fwCheckNum = 0;

    pBuffer = (uint8_t *) (DIRECT_DRAM_ADDR);

    for (fwCheckNum = 1; fwCheckNum < FW_CHECK_NUM; fwCheckNum++)
    {
        fwStatus = LoadFirmware_Verify((fwError_t)fwCheckNum, &sfSrcAddress, &sfDesAddress, &sfSize);
        // Remedy
        if (fwStatus != FW_NO_ERR)
        {
            DFW_PRINTF("\nFw Checker:%s\n", (fwStatus ==  FW_HEAD_ERR)  ? "HEAD_ERR"    :
                                          (fwStatus ==  FW_HEAD_BK_ERR) ? "HEAD_BK_ERR" :
                                          (fwStatus ==  FW_MIAN1_ERR)   ? "MAIN1_ERR"   :
                                          (fwStatus ==  FW_MIAN2_ERR)   ? "MAIN2_ERR"   :
                                          (fwStatus ==  FW_MAIN_BK_ERR) ? "MAIN_BK_ERR" : "UNEXPECTED");

            if (fwStatus != FW_UNEXPECT_NUM)
            {
                MEM_CLR(pBuffer, sfSize);
                HalSflash_Copy(pBuffer, sfSrcAddress, sfSize);
                HalSflash_EraseSector(sfDesAddress / SFLASH_SECTOR_SIZE, ALIGN_CEIL(sfSize, SFLASH_SECTOR_SIZE)/ SFLASH_SECTOR_SIZE, 3);
                error = AplLoadFirmware_SaveToSPI(sfDesAddress, ALIGN_CEIL(sfSize, SFLASH_SECTOR_SIZE), pBuffer, SFLASH_PAGE_SIZE, 5);

                if (fwStatus == FW_MIAN1_ERR)       BIT_SET(smFwValidStatus, BIT(FW_MAIN1_VALID));
                else if(fwStatus == FW_MIAN2_ERR)   BIT_SET(smFwValidStatus, BIT(FW_MAIN2_VALID));
                else if(fwStatus == FW_MAIN_BK_ERR) BIT_SET(smFwValidStatus, BIT(FW_MAIN_BK_VALID));
            }
        }
    }

    return error;
}

//-----------------------------------------------------------------------------
/**
    Save firmware to non-volaitle memory (serial flash)

    @param[in]  bPrintOn     verbose mode
    @param[in]  fw_addr      firmware offset
    @param[in]  fw_size      firmware image size

    @param[in]  pBuffer      pointer to code buffer
    @param[in]  codeSize     code size in byte count
    @param[in]  retry        retry count

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t AplLoadFirmware_SaveToSPI(uint32_t fw_addr, uint32_t fw_size, uint8_t* pBuffer, uint16_t byteMode, uint32_t retry)
{
    Error_t (* pFunSpiPrgm)(void*, uint32_t, uint32_t);
    uint32_t sflash_addr, sflash_sec;
    uint32_t cnt, err_cnt;

    if (byteMode == SFLASH_4BYTE_SIZE)    pFunSpiPrgm = HalSflash_ProgramU32;
    else                                  pFunSpiPrgm = HalSflash_Program;

    sflash_addr = fw_addr / SFLASH_SECTOR_SIZE;
    sflash_sec  = fw_size / SFLASH_SECTOR_SIZE;
    err_cnt = 0;
    DFW_PRINTF("pBuffer[%08x], IMG_Offset[%08x], IMG_Size[%08x]\n", pBuffer, fw_addr, fw_size);

    for (cnt = 0; cnt < retry; cnt ++)
    {
        if ((*pFunSpiPrgm)(pBuffer, fw_addr, fw_size) != cEcNoError)
        {
            DFW_PRINTF("Program Fail\n");
            err_cnt++;
            continue;
        }

        if (MEM_CMP(pBuffer, (void*)(fw_addr + SPI_BASE_ADDRESS), fw_size) == cEcNoError)
        {
            DFW_PRINTF("Compare Pass\n");
            break;
        }
        else
        {
            DFW_PRINTF("Compare Fail\n");
            err_cnt++;
        }

        if (HalSflash_EraseSector(sflash_addr, sflash_sec, 3) != cEcNoError)
        {
            DFW_PRINTF("Erase Fail\n");
        }
    }

    if (err_cnt >= retry)
    {
        return cEcError;
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Verify firmware content

    @return   error code
**/
//-----------------------------------------------------------------------------
static fwError_t LoadFirmware_Verify(fwError_t mode, uint32_t* srcAddress, uint32_t* desAddress, uint32_t* size)
{
    fwError_t err_fw = FW_NO_ERR;

    switch (mode)
    {
        case FW_HEAD_ERR:
            if (HalSflash_ReadU8(DFW_CURRENT_HEAD_OFFSET) != DFW_FW_SELECT_TAG)
            {
                *size = DFW_FW_HEAD_SIZE;
                *srcAddress = DFW_BACKUP_HEAD_OFFSET;
                *desAddress = DFW_CURRENT_HEAD_OFFSET;
                err_fw = mode;
            }
            break;
        case FW_HEAD_BK_ERR:
            if (HalSflash_ReadU8(DFW_BACKUP_HEAD_OFFSET) != DFW_FW_SELECT_TAG)
            {;
                *size = DFW_FW_HEAD_SIZE;
                *srcAddress = DFW_CURRENT_HEAD_OFFSET;
                *desAddress = DFW_BACKUP_HEAD_OFFSET;
                err_fw = mode;
            }
            break;
        case FW_MIAN1_ERR:
        case FW_MIAN2_ERR:
            if (!(smFwValidStatus & BIT(FW_MAIN1_VALID)) || !(smFwValidStatus & BIT(FW_MAIN2_VALID)))
            {
                AplLoadFirmware_GetFwInfo(DFW_CURRENT_HEAD_OFFSET, DISABLE, &gLdFw_FwInfo[0]);
                if (smFwValidStatus & BIT(FW_MAIN_BK_VALID))
                {
                    *srcAddress = gLdFw_FwInfo[FW_MAIN_BK_VALID].address;
                }
                else
                {
                    if (smFwValidStatus & BIT(FW_MAIN1_VALID))      *srcAddress = gLdFw_FwInfo[FW_MAIN1_VALID].address;
                    else if (smFwValidStatus & BIT(FW_MAIN2_VALID)) *srcAddress = gLdFw_FwInfo[FW_MAIN2_VALID].address;
                }

                if (!(smFwValidStatus & BIT(FW_MAIN1_VALID)))
                {
                    *size = gLdFw_FwInfo[FW_MAIN1_VALID].size;
                    *desAddress = gLdFw_FwInfo[FW_MAIN1_VALID].address;
                    err_fw = FW_MIAN1_ERR;
                }
                else if(!(smFwValidStatus & BIT(FW_MAIN2_VALID)))
                {
                    *size = gLdFw_FwInfo[FW_MAIN2_VALID].size;
                    *desAddress = gLdFw_FwInfo[FW_MAIN2_VALID].address;
                    err_fw = FW_MIAN2_ERR;
                }
            }
            break;
        #if (FW_BK_MECH == ENABLE)
        case FW_MAIN_BK_ERR:
            if (!(smFwValidStatus & BIT(FW_MAIN_BK_VALID)))
            {
                AplLoadFirmware_GetFwInfo(DFW_CURRENT_HEAD_OFFSET, DISABLE, &gLdFw_FwInfo[0]);
                if (smActiveFwSlot < FW_MAIN_BK_NUM && smActiveFwSlot > 0)
                {
                    *size = gLdFw_FwInfo[smActiveFwSlot - 1].size;
                    *desAddress = gLdFw_FwInfo[FW_MAIN_BK_VALID].address;
                    *srcAddress = gLdFw_FwInfo[smActiveFwSlot - 1].address;
                    err_fw = mode;
                }
                else
                {
                    err_fw = FW_UNEXPECT_NUM;
                }
            }
            break;
        #endif
        default:
            break;
    }

    return err_fw;
}
#endif

#if CHK_CODE_TYPE(CODE_BOOT)
//-----------------------------------------------------------------------------
/**
    LoadFirmware entry point
**/
//-----------------------------------------------------------------------------
void LoadFirmware(void)
{
    DfwInfo_t*          pfwInfo;
    uint8_t*            pBuffer;
    uint32_t            sflashAddress = 0;
    uint32_t            srcAddress = 0;
    uint32_t            cpu0Address;
    uint32_t            fw_history = U32_MAX;
    uint32_t            mainCrc = 0;
    uint32_t            select_address;
    uint8_t             fw_num = 0;
    uint8_t             fwBlkCnt;
    uint8_t             sum;

    pBuffer = (uint8_t *) (DIRECT_DRAM_ADDR);

    smFwValidStatus = 0;
    //------------------------------------
    // Check Select FW Slot
    //------------------------------------
    if (MEM_U8(SPI_BASE_ADDRESS + DFW_CURRENT_HEAD_OFFSET) == DFW_FW_SELECT_TAG)
    {
        sflashAddress = DFW_CURRENT_HEAD_OFFSET;
    }
    else
    {
        sflashAddress = DFW_BACKUP_HEAD_OFFSET;
    }

    HalSflash_Copy(&gLdFw_selinfo, sflashAddress, DFW_FW_SELECT_SIZE);

    //------------------------------------
    // Get FW Header and Tag, Record main code address & size
    //------------------------------------
    AplLoadFirmware_GetFwInfo(sflashAddress, ENABLE, &gLdFw_FwInfo[0]);

    //------------------------------------
    // Init FW list
    //------------------------------------
    LoadFirmware_InitFwList(&gLdFw_FwInfo[0]);
    LoadFirmware_InsertFwList(&gLdFw_FwInfo[0], gLdFw_selinfo.select - 1, FW_MAIN_BK_NUM - 1);

    //------------------------------------
    // Fimrware CRC check & Fail swap
    //------------------------------------
    pfwInfo = &gLdFw_FwInfo[gLdFw_selinfo.select - 1];

    for (fwBlkCnt = 0; fwBlkCnt < FW_MAIN_BK_NUM; fwBlkCnt++)
    {
        if (smFwValidStatus == 0)
        {
            DFW_PRINTF("Fw All Fail\n");
            break;
        }

        if (BIT(pfwInfo->select - 1) & smFwValidStatus)
        {
            sflashAddress = pfwInfo->address;
            HalSflash_Copy(pBuffer, (sflashAddress + sizeof(DfwImageTag_t)), pfwInfo->size);
            mainCrc = ~(MemCrcU32(U32_MAX, pBuffer, pfwInfo->size - 4 - sizeof(DfwImageTag_t)));

            if (mainCrc != gLdFw_ImgTag[pfwInfo->select - 1].mfw_crc)
            {
                DFW_PRINTF("Fw Slot|%x CRC Fail\n", (pfwInfo->select == FW_MAIN_BK_NUM) ? 0xB : pfwInfo->select);
                BIT_CLR(smFwValidStatus, BIT(pfwInfo->select - 1));
            }
            else
            {
                if (pfwInfo->select != FW_MAIN_BK_NUM)
                {
                    smActiveFwSlot = pfwInfo->select;
                }
                else
                {
                    smActiveFwSlot = gLdFw_selinfo.select;
                }

                sflashAddress += sizeof(DfwImageTag_t);
                smFwHistoryNumber = gLdFw_ImgTag[pfwInfo->select - 1].historyNum;
                DFW_PRINTF("Fw Slot|%x CRC PASS\n", (pfwInfo->select == FW_MAIN_BK_NUM) ? 0xB : pfwInfo->select);

                break;
            }
        }
        else
        {
            DFW_PRINTF("Fw Slot|%x Invalid Tag\n", (pfwInfo->select == FW_MAIN_BK_NUM) ? 0xB : pfwInfo->select);
        }

        pfwInfo = (DfwInfo_t *)pfwInfo->nextslot;
        sflashAddress = 0;
    }

    while (1)
    {
        HalSflash_Copy(&gLdFw_hdr, sflashAddress, sizeof(gLdFw_hdr));
        sum = MemSumU8((uint8_t *)&gLdFw_hdr, sizeof(gLdFw_hdr)-1);
        if ((sum != gLdFw_hdr.checkSum || gLdFw_hdr.id != DFW_FW_ID)) break; // &&  (gLdFw_hdr.id != DFW_HEADER_ID)

        srcAddress = gLdFw_hdr.srcAddress + sflashAddress;
        HalCpu_ConvertAddress(gLdFw_hdr.cpu, gLdFw_hdr.destAddress, 0, &cpu0Address);
        HalSflash_Copy((void*)cpu0Address, srcAddress, gLdFw_hdr.packetSize);
        sflashAddress = srcAddress + ALIGN_CEIL(gLdFw_hdr.srcBlockSize, 32);
    }
}

//-----------------------------------------------------------------------------
/**
    Init Load Firmware Table List

    @param[in] Fw Table
**/
//-----------------------------------------------------------------------------
static void LoadFirmware_InitFwList(DfwInfo_t* pfwInfo)
{
    uint8_t fwBlkCnt;

    for (fwBlkCnt = 0; fwBlkCnt < FW_MAIN_BK_NUM; fwBlkCnt++)
    {
        pfwInfo[fwBlkCnt].select = fwBlkCnt + 1;

        if (fwBlkCnt == FW_MAIN_BK_NUM - 1)
        {
            pfwInfo[fwBlkCnt].nextslot = (uint8_t *)&pfwInfo[0];
            pfwInfo[fwBlkCnt].prevslot = (uint8_t *)&pfwInfo[fwBlkCnt - 1];
        }
        else if (fwBlkCnt == 0)
        {
            pfwInfo[fwBlkCnt].prevslot = NULL;
            pfwInfo[fwBlkCnt].nextslot = (uint8_t *)&pfwInfo[fwBlkCnt + 1];
        }
        else
        {
            pfwInfo[fwBlkCnt].nextslot = (uint8_t *)&pfwInfo[fwBlkCnt + 1];
            pfwInfo[fwBlkCnt].prevslot = (uint8_t *)&pfwInfo[fwBlkCnt - 1];
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Insert Load Firmware Table List

    @param[in] KeepLink : Keep link for backup fw case
**/
//-----------------------------------------------------------------------------
static void LoadFirmware_InsertFwList(DfwInfo_t* pfwInfo, uint8_t prevNode, uint8_t nextNode)
{
    DfwInfo_t* pfwslot;

    pfwslot = (DfwInfo_t*) pfwInfo[prevNode].nextslot;

    if (pfwInfo[nextNode].prevslot != (uint8_t *)&pfwInfo[prevNode])
    {
        if (pfwInfo[nextNode].nextslot == NULL)
        {
            pfwslot->nextslot = NULL;
        }
        else
        {
            pfwslot->nextslot = pfwInfo[nextNode].nextslot;
        }

        if (pfwInfo[prevNode].nextslot == NULL)
        {
            pfwInfo[nextNode].nextslot = NULL;
        }
        else
        {
            pfwInfo[nextNode].nextslot = pfwInfo[prevNode].nextslot;
        }

        pfwInfo[prevNode].nextslot = (uint8_t *)&pfwInfo[nextNode];
        pfwInfo[nextNode].prevslot = (uint8_t *)&pfwInfo[prevNode];
    }

}
#endif

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
//! @brief
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"
#include "SharePkg.h"
#include "HostPkg.h"
#include "NvmeCmdPkg.h"

// REG

// HAL
#include "HalSflash.h"

// APL
#include "HldNvme.h"

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define DOWNLOAD_FW_NO_ACTIVE                   0
#define DOWNLOAD_FW_ACTIVE_AFT_RESET            1
#define FW_ACTIVE_AFT_NEXTRESET                 2
#define FW_ACTIVE_IMMEIDATE                     3

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define ASSING_FW_ACT_SCT(s,sct,sc)      {(s)->SCT = sct; (s)->SC = sc;}
//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe Admin Command structure - Firmware Activate
typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t FS:3;            ///< Firmware Slot (DW10 bits[2:0])
    uint32_t CA:2;            ///< Commit Action (DW10 bits[04:03])
    uint32_t reserved10:27;   ///< Reserved (DW10 bits[31:05])
    uint32_t updateBootCode;
    uint32_t reserved12[4];    ///< DW15..12

} NvmeAdminFirmwareActivateCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminFirmwareActivateCommand_t)==64, "NVMe command size shall be 64");


//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-tpye
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Firmware Activate - Admin 10h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_FirmwareActivate(HcmdQ_t* pHcmdQ)
{
    NvmeAdminFirmwareActivateCommand_t* pCdb;
    NvmeSdb_t*          pSdb;
    DfwModuleInfo_t*    pFwModule;
    DfwDescript_t*      pFwDesc;
    DfwHeader_t*        fwHeader;
    uint8_t*            pBuffer;
    DfwImageTag_t       Img_Tag;
    Error_t             fwPrgError = cEcNoError;
    uint32_t            fw_addr = 0;
    uint32_t            receivedSize = 0;
    uint32_t            calculatedCRC32 = 0;
    uint32_t            receivedCRC32 = 0;
    uint8_t             FwDownloadFlg = FALSE;
    uint8_t             BootNewFwFlg  = FALSE;
    uint8_t             ActiveFwLater = FALSE;
    uint8_t             ActiveFwKnow  = FALSE;
    uint8_t             select = 1;
    uint8_t             slot_Num = 0;
    uint8_t             fwBlkCnt = 0;

    pCdb = (NvmeAdminFirmwareActivateCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    pSdb = (NvmeSdb_t *)NvmeCmd_GetSdbPointer(pHcmdQ);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s FS=%1u CA=%1u\n", __FUNCTION__, pCdb->FS, pCdb->CA);
    END_HOST_DBG_DUMP

    pHcmdQ->error = cEcNoError;

    switch (pCdb->CA)
    {
        case DOWNLOAD_FW_NO_ACTIVE:
            FwDownloadFlg = TRUE;
            break;
        case DOWNLOAD_FW_ACTIVE_AFT_RESET:
            FwDownloadFlg = TRUE;
            BootNewFwFlg  = TRUE;
            ActiveFwLater = TRUE;
            ASSING_FW_ACT_SCT(pSdb, cGenericCommandStatus, cSuccessfulCompletion);
            break;
        case FW_ACTIVE_AFT_NEXTRESET:
            BootNewFwFlg  = TRUE;
            ActiveFwLater = TRUE;
            ASSING_FW_ACT_SCT(pSdb, cCommandSpecificStatus, cFirmwareApplicationRequiresConventionalReset);
            break;
        case FW_ACTIVE_IMMEIDATE:
            //FwDownloadFlg = TRUE;
            BootNewFwFlg  = TRUE;
            ActiveFwKnow  = TRUE;
            break;
        default:
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            return;
    }

    if (pCdb->FS > NVME_NUM_OF_FW_SLOTS)
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFirmwareSlot, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
        return;
    }

    if (pCdb->FS == 0)                  // For 0 case update to next slot (keep current slot)
    {
        DFW_PRINTF("Not in FS[%d]\n", pCdb->FS );
        if (smActiveFwSlot == 1)
        {
            select = 2;
        }
        else
        {
            select = 1;
        }
    }
    else
    {
        DFW_PRINTF("FS fir[%d]\n",  pCdb->FS);
        select = pCdb->FS;
    }

    // Handle special CA=3 case
    if (pCdb->CA == FW_ACTIVE_IMMEIDATE && gDetectFwSize > 0)
    {
        FwDownloadFlg = TRUE;
    }
    //----------------------------------------
    //  Get fw discription & fw module
    //----------------------------------------
    pFwDesc = (DfwDescript_t *)((uint8_t *)(DIRECT_DRAM_ADDR + DFW_FW_CSS_SIZE));
    pFwModule = (DfwModuleInfo_t *)((uint8_t *)(DIRECT_DRAM_ADDR + DFW_FW_CSS_SIZE + DFW_FW_DESC_SIZE));

    if (FwDownloadFlg)
    {
        uint8_t* pTagAddress = NULL;
        char*    fwTag[4] = {""};
        
        pBuffer = (uint8_t*) DIRECT_DRAM_ADDR;

        receivedSize = gDetectFwSize;
        gDetectFwSize = 0;

        // Handle CA=0 or 1 case if no payload abort CMD
        if ((pCdb->CA == DOWNLOAD_FW_NO_ACTIVE || pCdb->CA == DOWNLOAD_FW_ACTIVE_AFT_RESET) && receivedSize == 0)
        {
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFirmwareImage, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
            return;
        }

        //CRC Calculated
        receivedCRC32 = *(uint32_t *)((uint8_t *)pBuffer + receivedSize - 4);
        calculatedCRC32 = ~(MemCrcU32(U32_MAX, pBuffer, receivedSize - 4));

        //Firmware Error Check
        fwPrgError |= DFW_GET_ERROR_ST(pBuffer, receivedCRC32, calculatedCRC32);
        if (fwPrgError && receivedSize > 0)
        {
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFirmwareImage, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
            DFW_PRINTF("FW Bin Error:%x\n", fwPrgError);
            return;
        }

        //----------------------------------------
        //  Save firmware to SPI
        //----------------------------------------
        pBuffer = (uint8_t *)(DIRECT_DRAM_ADDR + DFW_FW_HEAD_SIZE);
        MEM_CLR(&Img_Tag, sizeof(Img_Tag));

        for (fwBlkCnt = 0; fwBlkCnt < pFwDesc->sec_cnt; fwBlkCnt++)
        {
            if ((pFwModule[fwBlkCnt].tag & 0xFFFF) == DFW_FW_MAFW_TAG)
            {
                slot_Num = DFW_GET_FWNUM(pFwModule[fwBlkCnt]);
                if (slot_Num == select)
                {
                    pFwModule[fwBlkCnt].update = ENABLE;
                }
            }

            if (pFwModule[fwBlkCnt].update)
            {
                *(uint32_t *)fwTag = pFwModule[fwBlkCnt].tag;
                DFW_PRINTF("FW %s\n", fwTag);
                HalSflash_EraseSector(pFwModule[fwBlkCnt].offset / SFLASH_SECTOR_SIZE, ALIGN_CEIL(pFwModule[fwBlkCnt].size, SFLASH_SECTOR_SIZE) / SFLASH_SECTOR_SIZE, 3);
                fwPrgError = AplLoadFirmware_SaveToSPI(pFwModule[fwBlkCnt].offset , ALIGN_CEIL(pFwModule[fwBlkCnt].size, SFLASH_SECTOR_SIZE) , pBuffer, SFLASH_PAGE_SIZE, 5);
                if ((pFwModule[fwBlkCnt].tag & 0xFFFF) == DFW_FW_MAFW_TAG && fwPrgError == cEcNoError)
                {
                    // Program FW Tag
                    if (slot_Num == select)
                    {
                        MEM_CLR(&gIfBuffer, DFW_FW_TAG_SIZE);
                        // copy 4K for tag becase handle by program fail case
                        MEM_COPY(&gIfBuffer, pBuffer, SFLASH_SECTOR_SIZE);
                        Img_Tag.mfw_crc = ~(MemCrcU32(U32_MAX, pBuffer + sizeof(DfwImageTag_t), pFwModule[fwBlkCnt].size - 4 - sizeof(DfwImageTag_t)));
                        Img_Tag.id = DFW_FW_IMAGE_TAG;
                        pTagAddress = (uint8_t *)((DIRECT_DRAM_ADDR) + DFW_FW_HEAD_SIZE + pFwModule[0].size - 0x10);    // FW revision information is mark at boot code
                        MEM_COPY((void *) &Img_Tag.fw_revision[0], pTagAddress, 8);
                        smFwHistoryNumber ++;
                        Img_Tag.historyNum = smFwHistoryNumber;
                        MEM_COPY(&gIfBuffer, &Img_Tag, sizeof(Img_Tag));
                    }
                    DFW_PRINTF("FW Tag\n");
                    fwPrgError = AplLoadFirmware_SaveToSPI(pFwModule[fwBlkCnt].offset, SFLASH_SECTOR_SIZE, gIfBuffer, SFLASH_PAGE_SIZE, 5);
                }
            }

            // DRAM offset
            if ((pFwModule[fwBlkCnt].tag & 0xFFFF) != DFW_FW_MAFW_TAG || pFwModule[fwBlkCnt].tag == DFW_FW_BKFW_TAG)
            {
                pBuffer += pFwModule[fwBlkCnt].size;
            }

            if (fwPrgError != cEcNoError)
            {
                pHcmdQ->error = MK_NVME_ERROR_CODE(cInternalDeviceError, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
                break;
            }
        }
    }

    //----------------------------------------
    //  If program pass then update Select
    //----------------------------------------
    if (fwPrgError == cEcNoError)
    {
        if (BootNewFwFlg)
        {
            // chose pCdb->FS as boot FW at next time
            fw_addr = DFW_CURRENT_HEAD_OFFSET;
            fwHeader = (DfwHeader_t *)gIfBuffer;
            MEM_CLR(fwHeader, DFW_FW_HEAD_SIZE);
            // if download flg enable refinfo update by dram else keep SPI original refinfo
            if (FwDownloadFlg)
            {
                MEM_COPY(&fwHeader[0].DfwDescript, pFwDesc, sizeof(DfwDescript_t));
                MEM_COPY(&fwHeader[1], pFwModule, sizeof(DfwModuleInfo_t) * pFwDesc->sec_cnt);
            }
            else
            {
                if (select != smActiveFwSlot && (smFwValidStatus & BIT(select - 1)))
                {
                    pBuffer = (uint8_t *) (DIRECT_DRAM_ADDR);
                    AplLoadFirmware_GetFwInfo(DFW_CURRENT_HEAD_OFFSET, DISABLE, &gLdFw_FwInfo[0]);
                    MEM_CLR(pBuffer, gLdFw_FwInfo[select - 1].size);
                    HalSflash_Copy(pBuffer, gLdFw_FwInfo[select - 1].address, gLdFw_FwInfo[select - 1].size);
                    HalSflash_EraseSector(gLdFw_FwInfo[FW_MAIN_BK_NUM - 1].address/ SFLASH_SECTOR_SIZE, ALIGN_CEIL(gLdFw_FwInfo[select - 1].size, SFLASH_SECTOR_SIZE)/ SFLASH_SECTOR_SIZE, 3);
                    DFW_PRINTF("Main Fw BackUp Update\n");
                    fwPrgError |= AplLoadFirmware_SaveToSPI(gLdFw_FwInfo[FW_MAIN_BK_NUM - 1].address, ALIGN_CEIL(gLdFw_FwInfo[select - 1].size, SFLASH_SECTOR_SIZE), pBuffer, SFLASH_PAGE_SIZE, 5);
                }

                HalSflash_Copy(fwHeader, fw_addr, DFW_FW_HEAD_SIZE);
            }

            fwHeader->DfwSelectTag.id = DFW_FW_SELECT_TAG;
            fwHeader->DfwSelectTag.select = select;
            DFW_PRINTF("FW Header BackUp\n");
            // Backup
            HalSflash_EraseSector(DFW_BACKUP_HEAD_OFFSET / SFLASH_SECTOR_SIZE, DFW_FW_HEAD_SIZE / SFLASH_SECTOR_SIZE, 3);
            fwPrgError |= AplLoadFirmware_SaveToSPI(DFW_BACKUP_HEAD_OFFSET, DFW_FW_HEAD_SIZE, (uint8_t *)fwHeader, SFLASH_PAGE_SIZE, 5);
            DFW_PRINTF("Fw Header Main\n");
            // Current
            HalSflash_EraseSector(DFW_CURRENT_HEAD_OFFSET / SFLASH_SECTOR_SIZE, DFW_FW_HEAD_SIZE / SFLASH_SECTOR_SIZE, 3);
            fwPrgError |= AplLoadFirmware_SaveToSPI(DFW_CURRENT_HEAD_OFFSET, DFW_FW_HEAD_SIZE, (uint8_t *)fwHeader, SFLASH_PAGE_SIZE, 5);
        }

        if (ActiveFwLater)
        {
            gBootNewFwFlag |= BOOT_FW_AFT_RST;
        }

        if (ActiveFwKnow)
        {
            gBootNewFwFlag |= BOOT_FW_NOW;
        }
    }

    if (fwPrgError != cEcNoError)
    {
        DFW_PRINTF("Program FW Fail\n");
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInternalDeviceError, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
    else if(pCdb->CA == FW_ACTIVE_IMMEIDATE)
    {
        pHcmdQ->attr.b.overlapped  = TRUE;
        gUpFwCmdBackup.updateFwTag = UPDATE_FW_NVME;
        MEM_COPY(&gUpFwCmdBackup.updateFwHcmdQ, pHcmdQ, sizeof(HcmdQ_t));
    }
}

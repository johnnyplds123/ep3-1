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

// APL
#include "HldNvme.h"
#include "Cache.h"

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
static const uint32_t cFirmwareImazeSize = 0x00100000;      //1Mbyte

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe Admin Command structure - Firmware Image Download
typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t NUMD;          ///< Number of Dwords (DW10)
    uint32_t OFST;          ///< Offset (DW11)

    uint32_t reserved12[4];  ///< DW15..12

} NvmeAdminFirmwareImageDownloadCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminFirmwareImageDownloadCommand_t)==64, "NVMe command size shall be 64");


//-----------------------------------------------------------------------------
//  Private function prototype definitions:
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
    Firmware Image download - Admin 11h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_FirmwareImageDownload(HcmdQ_t* pHcmdQ)
{
    NvmeAdminFirmwareImageDownloadCommand_t* pCdb;
    uint32_t receivedSize;
    uint32_t offset;
    uint8_t* pBuffer;

    pCdb = (NvmeAdminFirmwareImageDownloadCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s 0x%4X 0x%8X\n", __FUNCTION__, pCdb->NUMD, pCdb->OFST);
    END_HOST_DBG_DUMP

    receivedSize  = (pCdb->NUMD + 1) * sizeof(uint32_t); // convert to 1 based byte size
    offset        =  pCdb->OFST      * sizeof(uint32_t); // convert to byte size

    if (offset == 0)
    {
        gDetectFwSize = 0;                  // Reset DetecFwSize
        HalDmac_StartFill(DIRECT_DRAM_ADDR, 0, FW_IMAGE_SIZE);
        HalDmac_WaitDone();
    }

    gDetectFwSize += receivedSize;
    if (receivedSize > cFirmwareImazeSize)
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);

        gNvmeIfMgr.lastErrorBitOffset = 0;
        gNvmeIfMgr.lastErrorByteOffset = ( offset == 0) ? ERR_DW10_OFFSET : ERR_DW11_OFFSET; // DW10, NUMD or offset

        return;
    }

    pBuffer  = (uint8_t *) (DIRECT_DRAM_ADDR + offset);

    DFW_PRINTF("Download Addr [%08x]\n", pBuffer);

    pHcmdQ->attr.b.xferDir   = cXferH2d;                  // Receive (H -> D)
    pHcmdQ->bufDesc.ssdIndex = SSD_ADMIN_START;

    smSSD[SSD_ADMIN_START].startAddr =  (uint32_t)pBuffer                  >> SSD_ADDR_SHIFT;
    smSSD[SSD_ADMIN_START].endAddr   = ((DIRECT_DRAM_ADDR + FW_IMAGE_SIZE) >> SSD_ADDR_SHIFT) - 1;

    AplHost_TransferWholeData_WithPRPList(pHcmdQ, pBuffer, receivedSize, TRUE);
}

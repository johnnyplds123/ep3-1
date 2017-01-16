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
#if _TCG_!=TCG_NONE
#include "TcgCommon.h"
#endif
#include "SysInfo.h"
#include "UncTable.h"

// Code Section
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
/// @brief NVMe I/O Command structure - Write Uncorrectable
typedef struct
{
    IoCommandCommon_t DW9_0;    ///< DW9..0
    uint64_t sLba;              ///< Starting LBA (DW11..10)
    uint32_t nLb:16;            ///< Number of Logical Blocks (DW12 bits[15:00)
    uint32_t reserved12:16;     ///< Reserved (DW12 bits[31:16])
    uint32_t reserved13[3];     ///< DW15..13
} NvmeIoWriteUncorrectableCommand_t;
COMPILE_ASSERT(sizeof(NvmeIoWriteUncorrectableCommand_t)==64, "NVMe command size shall be 64");

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
void NvmeCmd_WriteUncorrectable(HcmdQ_t* pHcmdQ);

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
    Write uncorrectable command - I/O 04h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_WriteUncorrectable(HcmdQ_t* pHcmdQ)
{
#if _TCG_!=TCG_NONE
    if (mWriteLockedStatus || (mTcgStatus&MBR_SHADOW_MODE))
    {
        U16 tcgRangeStatus;
        tcgRangeStatus = TcgRangeCheck(pHcmdQ->addrInfo.lba, pHcmdQ->addrInfo.nb, TRUE);

        if (tcgRangeStatus)
        {   // return error when tcgRangeStatus != TCG_DOMAIN_NORMAL.
            pHcmdQ->error = MK_NVME_ERROR_CODE(cAccessDenied, cMediaErrors, cNoMoreStatusInformation, cDoNotRetry);
            return;
        }
    }
#endif

#if (CO_SUPPORT_WRITE_UNCORRECTABLE)
    uint16_t lba_shfit;
    
    lba_shfit = (HBU_PER_MBU_SFT - cLBAFTable[gNvmeIfMgr.ns[pHcmdQ->addrInfo.nsId0].LBAF].mbu_shift);
    if (UNCT_CHECK_DO_INSERT(pHcmdQ->addrInfo.lba << lba_shfit, pHcmdQ->addrInfo.nb << lba_shfit, UNCT_NDTP_WUNC) == UNCT_ERR_LBA_OVERFLOW)
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
#endif
    NvmeCmd_Complete(pHcmdQ);
}

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

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe Admin Command structure - Abort
typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t SQID:16;       ///< Submission Queue Identifier (DW10 bits[15:0])
    uint32_t CID: 16;       ///< Command Identifier to be aborted (DW10 bits[31:16])

    uint32_t reserved11[5]; ///< DW15..11

} NvmeAdminAbortCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminAbortCommand_t)==64, "NVMe command size shall be 64");

/// @brief NVMe Admin Abort Completion Queue Entry CQ Dword0
typedef struct
{
    uint32_t commandAborted:1;    ///< indicates whether the command was successfully aborted. [00]
    uint32_t reserved1:31;        ///< Reserved [31:01]

} AbortCompletionQDword0_t;
COMPILE_ASSERT(sizeof(AbortCompletionQDword0_t) == 4, "NVMe Abort DW0 size shall be 4");


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
    Abort I/O command - Admin 08h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_Abort(HcmdQ_t* pHcmdQ)
{
    NvmeAdminAbortCommand_t* pCdb;
    NvmeSdb_t* pSdb;

    pCdb = (NvmeAdminAbortCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    pSdb = (NvmeSdb_t *)NvmeCmd_GetSdbPointer(pHcmdQ);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s CID=%X SQID=%X\n", __FUNCTION__, pCdb->CID, pCdb->SQID);
    END_HOST_DBG_DUMP

    // TODO: abort application level command instance

    if (gHostIfMgr.info.currAbortCnt == NVME_ABORT_COMMAND_LIMIT)
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cAbortCommandLimitExceeded, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
        return;
    }

    // Assume FID is 0, it can be given when we implement virtual function.
    if (HldNvme_AbortCommand(0, pCdb->CID, pCdb->SQID) != cEcNoError)
    {
        pSdb->DW0 = 1;  // command was not aborted
    }
}


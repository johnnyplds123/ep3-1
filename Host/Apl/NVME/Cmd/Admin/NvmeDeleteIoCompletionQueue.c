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
/// @brief NVMe Admin Command structure - Delete I/O Completion Queue
typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t QID:16;          ///< Queue Identifier (DW10 bits[15:0])
    uint32_t reserved10:16;   ///< Reserved (DW10 bits[31:16])

    uint32_t reserved11[5];   ///< DW15..11

} NvmeAdminDeleteIoCqCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminDeleteIoCqCommand_t)==64, "NVMe command size shall be 64");


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
    Delete I/O completion queue - Admin 04h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_DeleteIoCompletionQueue(HcmdQ_t* pHcmdQ)
{
    NvmeAdminDeleteIoCqCommand_t* pCdb;

    pCdb = (NvmeAdminDeleteIoCqCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);

    pHcmdQ->error = HldNvme_DeleteIoCompletionQueue(pCdb->QID);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s CQID=%4X\n", __FUNCTION__, pCdb->QID);
    END_HOST_DBG_DUMP
}


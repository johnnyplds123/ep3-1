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
/// @brief NVMe Admin Command structure - Create I/O Submission Queue
typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t QID:16;         ///< Queue Identifier (DW10 bits[15:0])
    uint32_t QSIZE:16;       ///< Queue Size (DW10 bits[31:16])

    uint32_t PC:1;           ///< Physically Contiguous (DW11 bits[00])
    uint32_t QPRIO:2;        ///< Queue Priority (DW11 bits[02:01])
    uint32_t reserved11:13;  ///< Reserved (DW11 bits[15:03])
    uint32_t CQID:16;        ///< Completion Queue Identifier (DW11 bits[31:16])

    uint32_t reserved12[4]; ///< DW15..12

} NvmeAdminCreateIoSqCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminCreateIoSqCommand_t)==64, "NVMe command size shall be 64");


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
    Create I/O submission queue - Admin 01h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_CreateIoSubmissionQueue(HcmdQ_t* pHcmdQ)
{
    NvmeAdminCreateIoSqCommand_t* pCdb;

    pCdb = (NvmeAdminCreateIoSqCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);

    pHcmdQ->error = HldNvme_CreateIoSubmissionQueue(pCdb->DW9_0.PRP1, pCdb->QID, pCdb->QSIZE, pCdb->PC, pCdb->QPRIO, pCdb->CQID);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s PRP=%2X:%X CQID=%4X SQID=%4X SIZE=%4X\n", __FUNCTION__, GET_B63_32(pCdb->DW9_0.PRP1), GET_B31_00(pCdb->DW9_0.PRP1), pCdb->CQID, pCdb->QID, pCdb->QSIZE);
    END_HOST_DBG_DUMP
}


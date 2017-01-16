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
/// @brief NVMe Admin Command structure - Create I/O Completion Queue
typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t QID:16;        ///< Queue Identifier (DW10 bits[15:0])
    uint32_t QSIZE:16;      ///<Queue Size (DW10 bits[31:16])

    uint32_t PC:1;          ///< Physically Contiguous (DW11[0])
    uint32_t IEN:1;         ///< Interrupts Enabled (DW11[1])
    uint32_t reserved11:14; ///< Reserved (DW11 bits[15:2])
    uint32_t IV:16;         ///< Interrupt Vector (DW11 bits[31:16])

    uint32_t reserved12[4]; ///< DW15..12

} NvmeAdminCreateIoCqCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminCreateIoCqCommand_t)==64, "NVMe command size shall be 64");

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
    Create I/O completion queue - Admin 05h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_CreateIoCompletionQueue(HcmdQ_t* pHcmdQ)
{
    NvmeAdminCreateIoCqCommand_t* pCdb;

    pCdb = (NvmeAdminCreateIoCqCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
       D_PRINTF("%s PRP=%2X:%X CQID=%4X SIZE=%4X\n", __FUNCTION__, GET_B63_32(pCdb->DW9_0.PRP1), GET_B31_00(pCdb->DW9_0.PRP1), pCdb->QID, pCdb->QSIZE);
    END_HOST_DBG_DUMP

    if ((pCdb->DW9_0.NSID == 0x01)||(pCdb->DW9_0.NSID == 0xFFFFFFFF))  //Devx nIOQ
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidQueueIdentifier, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
        return;
    }
    pHcmdQ->error = HldNvme_CreateIoCompletionQueue(pCdb->DW9_0.PRP1, pCdb->QID, pCdb->QSIZE, pCdb->PC, pCdb->IEN, pCdb->IV);
}


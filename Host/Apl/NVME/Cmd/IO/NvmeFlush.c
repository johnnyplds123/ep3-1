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
//! @brief NVMe flush command handler
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"
#include "SharePkg.h"
#include "HostPkg.h"

// REG

// HAL

// APL
#include "HldNvme.h"
#include "NvmeIoCommands.h"

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
/// @brief NVMe I/O Command structure - Flush
typedef struct
{
    IoCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t reserved10[6]; ///< DW15..10

} NvmeIoFlushCommand_t;
COMPILE_ASSERT(sizeof(NvmeIoFlushCommand_t)==64, "NVMe command size shall be 64");

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
    NVMe flush command handler (OPC=00h)

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_Flush(HcmdQ_t* pHcmdQ)
{
    Msg_t * pMsg;
    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s\n", __FUNCTION__);
    END_HOST_DBG_DUMP

    pMsg = (Msg_t *)GetHcmdMsgPtr(pHcmdQ);

    MSG_SET_HOST_INIT(pMsg, cMcFlush);

    Host_PushToTail(cHcsFlushWait, pHcmdQ);
}

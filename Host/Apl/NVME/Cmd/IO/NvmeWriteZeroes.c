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
/// @brief NVMe I/O Command structure - Write Zeroes
typedef struct
{
    IoCommandCommon_t DW9_0;    ///< DW9..0

    uint64_t SLBA;          ///< Starting LBA (DW11..10)

    uint32_t NLB:16;        ///< Number of Logical Blocks (DW12 bits[15:00])
    uint32_t reserved12:10; ///< Reserved (DW12 bits[25:16])
    uint32_t PRINFO:4;      ///< Protection Information Field (DW12 bits[29:26])
    uint32_t FUA:1;         ///< Force Unit Access (DW12 bits[30])
    uint32_t LR:1;          ///< Limited Retry (DW12 bits[31])

    uint32_t reserved13;    ///< Reserved  (DW13)

    uint32_t ILBRT;         ///< Initial Logical Block Reference Tag (DW14)

    uint32_t LBAT:16;       ///< Logical Block Application Tag (DW15 bits[15:0])
    uint32_t LBATM:16;      ///< Logical Block Application Tag Mask (DW15 bits[31:16])

} NvmeIoWriteZeroesCommand_t;
COMPILE_ASSERT(sizeof(NvmeIoWriteZeroesCommand_t)==64, "Nvme Io Write Zeroes command size shall be 64");


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
    Write zeros command - I/O 08h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_WriteZeroes(HcmdQ_t* pHcmdQ)
{
#if _TCG_!=TCG_NONE
    if (mWriteLockedStatus || (mTcgStatus&MBR_SHADOW_MODE))
    {
        U16 tcgRangeStatus;
        tcgRangeStatus = TcgRangeCheck(pHcmdQ->addrInfo.lba, pHcmdQ->addrInfo.nb, TRUE);

        if (tcgRangeStatus) // return error when tcgRangeStatus != TCG_DOMAIN_NORMAL.
        {
            pHcmdQ->error = MK_NVME_ERROR_CODE(cAccessDenied, cMediaErrors, cNoMoreStatusInformation, cDoNotRetry);
            return;
        }
    }
#endif

    if (pHcmdQ->addrInfo.nb)
    {
        MsgWrtZero_t* pWriteZeroMsg;
        uint16_t      lbaf;
        uint16_t      nsId0;

        nsId0 = pHcmdQ->addrInfo.nsId0;
        lbaf  = gNvmeIfMgr.ns[nsId0].LBAF;

        pWriteZeroMsg = (MsgWrtZero_t*) GetHcmdMsgPtr(pHcmdQ);
        MSG_SET_HOST_INIT(pWriteZeroMsg, cMcWriteZero);

        pWriteZeroMsg->lba       = pHcmdQ->addrInfo.lba;
        pWriteZeroMsg->nb        = pHcmdQ->addrInfo.nb;
        pWriteZeroMsg->mbu_shift = cLBAFTable[lbaf].mbu_shift;
        pWriteZeroMsg->mbu_mask  = cLBAFTable[lbaf].mbu_mask;

        Host_MediaReq();
        IPC_SendMsgQ(cH2cReqQueue, (Msg_t*) pWriteZeroMsg);
    }
    else
    {
        NvmeCmd_Complete(pHcmdQ);
    }
}

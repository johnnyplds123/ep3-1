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
//! \file
//! @brief NVMe controller error handling code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"
#include "RegNvmePkg.h"

// REG

// HAL
#include "HalCpu.h"
#include "HalIrqSrc.h"
#include "HalIrq.h"
#include "HalRegConfig.h"
#include "HalNvmeXfer.h"
// APL
#include "MemIo.h"
#include "MemAlloc.h"

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
///  @brief Nvme FIFO Error Module Code definition
typedef enum
{
    cStsm       = 0,
    cCmdf       = 0,
    cArbi       = 0,
    cCreg       = 0,
    cPrpFetcher = 2,
    cRdEngine   = 3,
    cWrEngine   = 4,
    cPktm       = 5,
    cHwll       = 6
} NvmeErrorFifoId_t;

///  @brief Nvme STSM FIFO Error Sub Code definition
typedef enum
{
    cStsmBsrmParityError= 0x01, /// NVMe BSRM parity error
    cStsmCsrmParityError= 0x05, /// NVMe CSRM parity error
    cStsmPcieWriteError = 0x18, /// PCIe write error
    cStsmPcieReadError  = 0x19, /// PCIe read error
    cStsmIrqFailed      = 0x20, /// IRQ failed
    cStsmInvalidQueue   = 0x21, /// Invalid Queuue
    cStsmPrpNotReady    = 0x23, /// PRP Not Ready
    cStsmAbortedCommand = 0x24, /// Aborted Command
    cStsmCqFull         = 0x25  /// CQ Full
} NvmeStsmError_t;

///  @brief Nvme CREG FIFO Error Code definition
typedef enum
{
    cCregQstPerr       = 0x00, /// Queue Status Table parity error
    cCreNvmPerr        = 0x04, /// NVMe config Register / MSI-X table parity error
    cCreInvalidQueue   = 0x05, /// Queue not created when doorbell ring
    cCreInvalidDb      = 0x06, /// Invalid doorbell value
} NvmeCregError_t;

///  @brief Nvme Error Message structure (64bit)
typedef union
{
    uint64_t all;   ///< All 64 bits
    uint32_t dw[2]; ///< Access to each dwords
    struct
    {
        uint64_t VALUE       :32;   ///<BIT [31:0]  depends on error code
        uint64_t QID         :16;   ///<BIT [47:32] Queue ID
        uint64_t FID         : 8;   ///<BIT [55:48] Functio ID
        uint64_t ERROR_CODE  : 8;   ///<BIT [63:56] ErrorCode
    } b;
} NvmeErrorMsg64_t;

///  @brief Nvme Error Message structure (32bit)
typedef union
{
    uint32_t all;   ///< All 32 bits
    struct
    {
        uint32_t VALUE       :23;   ///<BIT [22: 0] depends on error code
        uint32_t ERROR_CODE  : 9;   ///<BIT [31:23] error code
    } b;
} NvmeErrorMsg32_t;

///  @brief Nvme Error Message structure (for 18h, 20h, 21h, 23h, 25h error code)
typedef union
{
    uint32_t all;   ///< All 32 bits
    struct
    {
        uint32_t ID           : 8;   ///<BIT [7:0] Manual request Id or Command index
        uint32_t reserved5    :12;   ///<BIT [19:8] Reserved
        uint32_t AM           : 1;   ///<BIT [20] Auto/Manual CQ (0=Manual)
        uint32_t reserved21   : 1;   ///<BIT [21] Reserved
        uint32_t QT           : 1;   ///<BIT [22] Queue type (0=CQ, 1=SQ)
        uint32_t ERROR_CODE   : 9;   ///<BIT [31:23] error code
    } b;
} NvmeStsmErrorMsg_t;

typedef NvmeErrorMsg64_t NvmeCmdfErrorMsg_t;    // Command fetcher error message
typedef NvmeErrorMsg64_t NvmePktmErrorMsg_t;    // Packet manager error message
typedef NvmeErrorMsg64_t NvmeDatmErrorMsg_t;    // Data manager error message
typedef NvmeErrorMsg64_t NvmeCregErrorMsg_t;    // Controller Register error message

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Status Manager Error Handler

    @param[in]  param       software interrupt handler table.
**/
//-----------------------------------------------------------------------------
void HalNvme_HandleStsmError(IrqHandler_t irqHandler[])
{
    uint16_t mcqIndex;
    NvmeStsmErrorMsg_t errorMsg;
    StatManagerErrFifoStat_t errorFifoStatus;

    errorFifoStatus = rNvmeStatMngr.statManagerErrFifoStat;

    if(errorFifoStatus.b.ERR_RPT_FIFO_STAT == 2)  // TODO: Error FIFO is empty.
    {   // How could this interrupt be triggered?
    }
    if(errorFifoStatus.b.ERR_RPT_FIFO_STAT == 3)  // TODO: Error FIFO is error.
    {   // Do I have to initialize something to fix the FIFO error?
    }

    D_PRINTF("[STSM_ERR:%X]", errorFifoStatus.all);

    while(errorFifoStatus.b.ERR_RPT_VAL_ENT_CNT--)
    {
        errorMsg.all = rNvmeStatMngr.ERR_RPT_FIFO_RD_PORT;

        mcqIndex = errorMsg.b.ID & 0x1F; // manual CQ index

        D_PRINTF(" EC=%2X VAL=%X", errorMsg.b.ERROR_CODE, errorMsg.all);

        switch(errorMsg.b.ERROR_CODE)
        {
        case cStsmPcieWriteError:
        case cStsmPrpNotReady:
        case cStsmCqFull:
            if (NvmeHba.mcqInUseBitmap & BIT(mcqIndex))
            {
                // retry, Repost status
                rNvmeStatMngr.statManagerFifoWritePort = NvmeHba.mcqFifoBackup[mcqIndex][0];
                rNvmeStatMngr.statManagerFifoWritePort = NvmeHba.mcqFifoBackup[mcqIndex][1];
                rNvmeStatMngr.statManagerFifoWritePort = NvmeHba.mcqFifoBackup[mcqIndex][2];
                rNvmeStatMngr.statManagerFifoWritePort = NvmeHba.mcqFifoBackup[mcqIndex][3];
                D_PRINTF(" REPOST MCQ:%2X", mcqIndex);
            }
            break;
        case cStsmIrqFailed:
        case cStsmInvalidQueue:
            if (errorMsg.b.AM==0)
            {
                BIT_CLR(NvmeHba.mcqInUseBitmap, BIT(mcqIndex)); // abort mcq posting
                D_PRINTF(" ABORT MCQ:%2X", mcqIndex);
            }
            break;
       }
    }

    D_PRINTF("\n");
}

//-----------------------------------------------------------------------------
/**
    Packet Manager Error Handler

    @param[in]  param       software interrupt handler table.
**/
//-----------------------------------------------------------------------------
void HalNvme_HandlePktmError(IrqHandler_t irqHandler[])
{
    uint32_t errorFifoCount;
    NvmePktmErrorMsg_t errMsg;

    errorFifoCount = rNvmePktMngr.packetManagerErrFifoCnt.b.PKTM_ERR_CNTR;

    D_PRINTF("[PKTM_ERR:%X]", errorFifoCount);

    while(errorFifoCount--)
    {
        errMsg.dw[1] = rNvmePktMngr.PKTM_ERR_FIFO_DW[1];
        errMsg.dw[0] = rNvmePktMngr.PKTM_ERR_FIFO_DW[0];

        D_PRINTF(" EC=%2X FID=%2X QID=%4X VAL=%X", errMsg.b.ERROR_CODE, errMsg.b.FID, errMsg.b.QID, errMsg.b.VALUE);

        // TODO: Add code to handle error
    }

    D_PRINTF("\n");
}

//-----------------------------------------------------------------------------
/**
    Command fetcher Error Handler

    @param[in]  param       software interrupt handler table.
**/
//-----------------------------------------------------------------------------
void HalNvme_HandleCmdfError(IrqHandler_t irqHandler[])
{
    uint32_t errorFifoCount;
    NvmeCmdfErrorMsg_t errMsg;

    errorFifoCount = rNvmeCmdFetch.cmdFetcherErrFifoCnt.b.CMDF_ERR_CNTR;

    D_PRINTF("[CMDF_ERR:%X]", errorFifoCount);

    while(errorFifoCount--)
    {
        errMsg.dw[1] = rNvmeCmdFetch.CMDF_ERR_FIFO_DW[1];
        errMsg.dw[0] = rNvmeCmdFetch.CMDF_ERR_FIFO_DW[0];

        D_PRINTF(" EC=%2X FID=%2X QID=%4X VAL=%X", errMsg.b.ERROR_CODE, errMsg.b.FID, errMsg.b.QID, errMsg.b.VALUE);

        // TODO: Add code to handle error
    }

    D_PRINTF("\n");
}

//-----------------------------------------------------------------------------
/**
    Data Manager Error Handler

    @param[in]  param       software interrupt handler table.
**/
//-----------------------------------------------------------------------------
void HalNvme_HandleDatmError(IrqHandler_t irqHandler[])
{
    uint32_t errorFifoCount;
    NvmeDatmErrorMsg_t errMsg;

    errorFifoCount = rNvmeDataMngr.dataManagerErrInformat.b.ERR_CNT;

    D_PRINTF("[DATM_ERR]Cnt[%X]", errorFifoCount);

    while(errorFifoCount--)
    {
        errMsg.dw[1] = rNvmeDataMngr.DATM_ERR_DWORD[1];
        errMsg.dw[0] = rNvmeDataMngr.DATM_ERR_DWORD[0];

        D_PRINTF("\n[%x] EC=%2X FID=%2X QID=%4X VAL=%X",errorFifoCount,errMsg.b.ERROR_CODE, errMsg.b.FID, errMsg.b.QID, errMsg.b.VALUE);

        // TODO: Add code to handle error
    }

    D_PRINTF("\n");
}

//-----------------------------------------------------------------------------
/**
    Controller Register Error Handler

    @param[in]  param       software interrupt handler table.
**/
//-----------------------------------------------------------------------------
void HalNvme_HandleCregError(IrqHandler_t irqHandler[])
{
    uint32_t errorFifoCount;
    NvmeCregErrorMsg_t errMsg;

    errorFifoCount = rNvme.ctrlRegisterModuleErrFifoCnt.b.CREG_ERR_FIFO_CTR;

    D_PRINTF("[CREG_ERR:%X]", errorFifoCount);

    while(errorFifoCount--)
    {
        errMsg.dw[1] = rNvme.CREG_ERR_FIFO_DW[1];
        errMsg.dw[0] = rNvme.CREG_ERR_FIFO_DW[0];

        D_PRINTF(" EC=%2X FID=%2X QID=%4X VAL=%X", errMsg.b.ERROR_CODE, errMsg.b.FID, errMsg.b.QID, errMsg.b.VALUE);

        // TODO: Add code to handle error
    }

    D_PRINTF("\n");
}




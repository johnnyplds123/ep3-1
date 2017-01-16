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
#include "SharePkg.h"
#include "RegNvmePkg.h"
#include "HostPkg.h"
#include "NvmeCmdPkg.h"

// REG

// HAL
#include "HalCpu.h"
#include "HalIrqSrc.h"
#include "HalIrq.h"
#include "HalRegConfig.h"
#include "HalNvmeXfer.h"

// APL
#include "Cache.h"

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define IO_CMD_AVAILABLE(N)  (rNvmeBitmapSram.bitmapSramCmdIndexAvailablBitmapCntrIncrease.all = N)
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

///  @brief Nvme Controller FIFO Error Sub Code definition
typedef enum
{
    cCregCmdfQarbQueueStatusParity       = 0x00, /// 0x00 CREG, CMDF, QARB - Queue Status Table Parity
    cCregCmdPrpTableParity               = 0x01, /// 0x01 CREG - CMD PRP Table Parity Error
    cCmdfCurPrpTableParity               = 0x02, /// 0x02 CMDF - Current PRP Table Parity Error
    cStsmNvmeRegParityError              = 0x04, /// 0x04 CREG, STSM - NVMe Config Register / MSI-X Table Parity Error
    cCregDbOnNonExistQueue               = 0x05, /// 0x05 CREG - Doorbell on non-exist Queue NVMe CSRM Parity Error
    cCregInvalidDoorbell                 = 0x06, /// 0x06 CREG - Invalid Doorbell
    cCmdfCiabmReadParity                 = 0x07, /// 0x07 CMDF - CIABM Read Parity Error
    cCmdfCiabmRmwParity                  = 0x08, /// 0x08 CMDF - CIABM Read-Modify-Write Parity Error
    cStsmDtdbmReadParityError            = 0x09, /// 0x09 STSM - NVMe DTDBM read parity error
    cStsmDtdbmReadModifyWriteParityError = 0x0A, /// 0x0A STSM - NVMe DTDBM read modify write parity error
    cStsmCcbmReadModifyWriteParityError  = 0x0B, /// 0x0B STSM - NVMe CCBM read modify write parity error
    cCmdfPcieTransferError               = 0x0C, /// 0x0C CMDF - PCIE Transfer Error
    cStsmPcieReadError                   = 0x0D, /// 0x0D STSM - PCIe Read error
    cCmdfAxiTransferError                = 0x0E, /// 0x0E CMDF - AXI Transfer Error
    cStsmCabmReadModifyWriteParityError  = 0x0F, /// 0x0F STSM - NVMe CABM read modify write parity error
    cStsmAbortedCommand                  = 0x10, /// 0x10 STSM - Aborted Command
    cStsmCsrmParityError                 = 0x11, /// 0x11 STSM - NVMe Command SRAM parity error
    cStsmSeqIdNotValid                   = 0x12, /// 0x12 STSM - Sequence ID Not Valid
    cStsmInvalidQueue                    = 0x13, /// 0x13 STSM - Invalid Queuue
    cStsmCqFull                          = 0x14, /// 0x14 STSM - CQ Full
    cStsmPrpNotReady                     = 0x15, /// 0x15 STSM - PRP Not Ready
    cStsmPcieWriteError                  = 0x16, /// 0x16 STSM - PCIe write error
    cStsmIrqFailed                       = 0x1F, /// 0x1F STSM - IRQ failed

    cDatmCabm0ParityError                = 0x20, /// 0x20 DATM (PRP Setup Engine) - CABM0 Parity Error
    cDatmCabm1DeactParityError           = 0x21, /// 0x21 DATM (PRP Setup Engine) - CABM1 (Deactivate) Parity Error
    cDatmCabm1ActParityError             = 0x22, /// 0x22 DATM (PRP Setup Engine) - CABM1 (Activate) Parity Error
    cDatmCiabmRmwParityError             = 0x23, /// 0x23 DATM (PRP Setup Engine) - CIAMB Read-Modify-Write Parity Error
    cDatmCabm1MskRmwParityError          = 0x24, /// 0x24 DATM (PRP Setup Engine) - CABM1 MSK Read-Modify-Write Parity Error
    cDatmCabm1MskReadParityError         = 0x25, /// 0x25 DATM (PRP Setup Engine) - CABM1 MSK Read Parity Error
    cDatmDtdbmParityError                = 0x26, /// 0x26 DATM (PRP Setup Engine) - DTDBM Parity Error
    cDatmPrbmR0ParityError               = 0x27, /// 0x27 DATM (PRP Setup Engine) - PRPBM R0 Parity Error
    cDatmPrbmW0ParityError               = 0x28, /// 0x28 DATM (PRP Setup Engine) - PRPBM W0 Parity Error
    cDatmCmdSramParityError              = 0x29, /// 0x29 DATM (PRP Setup Engine) - CMD SRAM Parity Error
    cDatmLceSramParityError              = 0x2A, /// 0x2A DATM (PRP Setup Engine) - LCE SRAM Parity Error
    cDatmCabm0RcaFifoParityError         = 0x2B, /// 0x2B DATM (PRP Setup Engine) - CAMB0 RCA FIFO CMD Parity Error
    cDatmCabm1RcaFifoParityError         = 0x2C, /// 0x2C DATM (PRP Setup Engine) - CAMB1 RCA FIFO CMD Parity Error
    cDatmCabm1MskRcaFifoParityError      = 0x2D, /// 0x2D DATM (PRP Setup Engine) - CAMB1 MSK RCA FIFO CMD Parity Error

    cDatmPrpfPrbmR0ParityError           = 0x40, /// 0x40 DATM (PRP Fetcher) - PRBM R0 Parity Error
    cDatmPrpfPrbmR1ParityError           = 0x41, /// 0x41 DATM (PRP Fetcher) - PRBM R1 Parity Error
    cDatmPrpfCmdSramReadCmdParityError   = 0x42, /// 0x42 DATM (PRP Fetcher) - CMD SRAM Read Cmd Parity Error
    cDatmPrpfPrbmClrR0ParityError        = 0x45, /// 0x45 DATM (PRP Fetcher) - PRBM CLR R0 Parity Error
    cDatmPrpfPrbmW0ParityError           = 0x48, /// 0x48 DATM (PRP Fetcher) - PRBM W0 Parity Error
    cDatmPrpfPrbmW1ParityError           = 0x49, /// 0x49 DATM (PRP Fetcher) - PRBM W1 Parity Error
    cDatmPrpfCmdSramWriteCmdParityError  = 0x4A, /// 0x4A DATM (PRP Fetcher) - CMD SRAM Write Cmd Parity Error
    cDatmPrpfPacketManagerError          = 0x4B, /// 0x4B DATM (PRP Fetcher) - Packet Manager Error
    cDatmPrpfPrbmClrW0ParityError        = 0x4D, /// 0x4D DATM (PRP Fetcher) - PRBM CLR W0 Parity Error
    cDatmPrpfDtdbmParityError            = 0x4E, /// 0x4E DATM (PRP Fetcher) - DTDBM Parity Error
    cDatmPrpfCabm1MskWcaFifoParityError  = 0x50, /// 0x50 DATM (PRP Fetcher) - CABM1 MSK (WCA FIFO CMD Parity Error)
    cDatmPrpfCabm1WcaFifoParityError     = 0x51, /// 0x51 DATM (PRP Fetcher) - CABM1 (WCA FIFO CMD Parity Error)
    cDatmPrpfCabm0WcaFifoParityError     = 0x52, /// 0x52 DATM (PRP Fetcher) - CABM0 (WCA FIFO CMD Parity Error)

    cDatmReRdrbmMskParityError           = 0x60, /// 0x60 DATM (Read Engine) - RDRBM0, RDRBM1, RDRBM1 MSK Parity Error
    cDatmReCmdSramParityError            = 0x62, /// 0x62 DATM (Read Engine) - CMD SRAM Parity Error
    cDatmReLceSramParityError            = 0x63, /// 0x63 DATM (Read Engine) - LCE SRAM Parity Error
    cDatmReAxiIfErrorFromSsdFetcher      = 0x68, /// 0x68 DATM (Read Engine) - AXI Interface Error from SSD Fetcher
    cDatmReDtdbmParityError              = 0x6A, /// 0x6A DATM (Read Engine) - DTDBM Parity Error
    cDatmReE2eDppErrors                  = 0x6C, /// 0x6C DATM (Read Engine) - End-to-end / Data Path Protection Errors Error
    cDatmReE2eDppAesErrors               = 0x6D, /// 0x6D DATM (Read Engine) - End-to-end / Data Path Protection Errors (AES)

    cDatmWeCmdSramParityError            = 0x80, /// 0x80 DATM (Write Engine) - CMD SRAM Parity Error
    cDatmWeAxiErrorFromSsdFetcher        = 0x81, /// 0x81 DATM (Write Engine) - AXI Error from SSD Fetcher
    cDatmWeBadbmParityError              = 0x82, /// 0x82 DATM (Write Engine) - BADBM Parity Error
    cDatmWePacketManagerIfError          = 0x83, /// 0x83 DATM (Write Engine) - Packet Manager Interface Error
    cDatmWeE2eDppErrors                  = 0x85, /// 0x85 DATM (Write Engine) - End-to-end / Data Path Protection Errors
    cDatmWeAxiErrorFromUmc               = 0x87, /// 0x87 DATM (Write Engine) - AXI Error from UMC
    cDatmWeDataFifoParityError           = 0x88, /// 0x88 DATM (Write Engine) - Data FIFO Parity Error

    cPktmReadReqTableParityError         = 0xA0, /// 0xA0 PKTM - Read Request Table Parity Error
    cPktmPcieError                       = 0xA1, /// 0xA1 PKTM - PCIe Error
    cPktmPcieWriteError                  = 0xA2, /// 0xA2 PKTM - PCIe Write Error

    cHwllActiveReqTableParityError       = 0xC1, /// 0xC1 HWLL - Active Request Table Parity Error
    cHwllWtdHeadTailTableParityError     = 0xC2, /// 0xC2 HWLL - Head Tail Table Parity Error (write transfer done phase)
    cHwllWtdLinkArrayParityError         = 0xC3, /// 0xC3 HWLL - Link Array Parity Error (write transfer done phase)
    cHwllRdrbmParityError                = 0xC4, /// 0xC4 HWLL - RDRBM Parity Error
    cHwllLkHeadTailTableParityError      = 0xC5, /// 0xC5 HWLL - Head Tail Table Parity Error (link kill operation)
    cHwllLkLinkArrayParityError          = 0xC6, /// 0xC6 HWLL - Link Array parity error (link kill operation)
    cHwllHrHeadTailTableParityError      = 0xC7, /// 0xC7 HWLL - Head Tail Table Parity Error (host request phase)
    cHwllHrLinkArrayParityError          = 0xC8, /// 0xC8 HWLL - Link Array Parity Error (host request phase)
    cHwllRtdHeadTailTableParityError     = 0xC9, /// 0xC9 HWLL - Head Tail Parity Error (read transfer done phase)
    cHwllRtdLinkArrayParityError         = 0xCA, /// 0xCA HWLL - Link Array Parity Error (read transfer done phase)
    cHwllLinkArrayEntryNotValid          = 0xCB, /// 0xCB HWLL - Link Array Entry Not Valid
    cHwllAhbAccessparityError            = 0xCC, /// 0xCC HWLL - AHB Access Parity Error
    cHwllFatalError                      = 0xCD, /// 0xCD HWLL - Fatal Error
} NvmeControllerError_t;

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

// Error Code 0x01: CMDF - CMD PRP Table Parity Error
typedef struct
{
    uint64_t reserved00      :30;   ///<BIT [29:00] reserved
    uint64_t QT              : 1;   ///<BIT [30]    Queue Type (1=SQ, 0=CQ)
    uint64_t CI              : 1;   ///<BIT [31]    Correction Information (0: Uncorrectable, 1: Correctable)
    uint64_t QID             :16;   ///<BIT [47:32] Queue ID
    uint64_t FID             : 8;   ///<BIT [55:48] Functio ID
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeCmdfCmdPrpTableParityError_t;

// Error Code 0x02: CMDF - Current PRP Table Parity Error
typedef struct
{
    uint64_t reserved00      :15;   ///<BIT [14:00] reserved
    uint64_t HW_AUTO_RECOVER : 1;   ///<BIT [15] HW Auto Recover (0: No HW Auto Recover, 1: HW Auto Recover)
    uint64_t reserved16      :14;   ///<BIT [29:16] reserved
    uint64_t QT              : 1;   ///<BIT [30]    Queue Type (1=SQ, 0=CQ)
    uint64_t CI              : 1;   ///<BIT [31]    Correction Information (0: Uncorrectable, 1: Correctable)
    uint64_t QID             :16;   ///<BIT [47:32] Queue ID
    uint64_t FID             : 8;   ///<BIT [55:48] Functio ID
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeCmdfCurrentPrpTableParityError_t;

// Error Code 0x04: CREG, STSM - NVMe Config Register / MSI-X Table Parity Error
typedef struct
{
    uint64_t ENTRY_OFFSEST   :16;   ///<BIT [15:00] Table Entry Offset
    uint64_t TID             : 8;   ///<BIT [23:16] Table ID
    uint64_t reserved21      : 7;   ///<BIT [30:24] reserved
    uint64_t CI              : 1;   ///<BIT [31]    Correction Information (0: Uncorrectable, 1: Correctable)
    uint64_t reserved32      :16;   ///<BIT [47:32] reserved
    uint64_t FID             : 8;   ///<BIT [55:48] Functio ID
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmConfigRegParityError_t;

// Error Code 0x05 : CREG - Doorbell on non-exist Queue – NVMe CSRM Parity Error
typedef struct
{
    uint64_t reserved00      :30;   ///<BIT [29:00] reserved
    uint64_t QT              : 1;   ///<BIT [30]    Queue Type (1=SQ, 0=CQ)
    uint64_t CI              : 1;   ///<BIT [31]    Correction Information (0: Uncorrectable, 1: Correctable)
    uint64_t QID             :16;   ///<BIT [47:32] Queue ID
    uint64_t FID             : 8;   ///<BIT [55:48] Functio ID
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeCregDbOnNonExistQueue_t;

// Error Code 0x06: CREG - Invalid Doorbell
typedef struct
{
    uint64_t ASYNC_EVENT_TYPE: 4;   ///<BIT [03:00] Async Evnet Type
                                    ///<            4'b0001: Queue size violation
                                    ///<            4'b0010: Pointer violation
                                    ///<            4'b0001: CQ empty violation
                                    ///<            4'b0001: SQ full violation
    uint64_t reserved04      :26;   ///<BIT [29:04] reserved
    uint64_t QT              : 1;   ///<BIT [30]    Queue Type (1=SQ, 0=CQ)
    uint64_t CI              : 1;   ///<BIT [31]    Correction Information (0: Uncorrectable, 1: Correctable)
    uint64_t QID             :16;   ///<BIT [47:32] Queue ID
    uint64_t FID             : 8;   ///<BIT [55:48] Functio ID
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeCregInvalidDoorbell_t;

// Error Code 0x09: STSM - DTDBM Read Parity Error
typedef struct
{
    uint64_t ENTRY_OFFSEST   :16;   ///<BIT [15:00] Bitmap Entry Offset
    uint64_t reserved16      :15;   ///<BIT [30:16] reserved
    uint64_t CI              : 1;   ///<BIT [31]    Correction Information (0: Uncorrectable, 1: Correctable)
    uint64_t reserved32      :24;   ///<BIT [55:32] reserved
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeCmdfCiabmReadParityError_t;

// Error Code 0x0A: STSM - DTDBM Read-Modify-Write Parity Error
typedef struct
{
    uint64_t CMD_INDEX       :16;   ///<BIT [15:00] Command Index
    uint64_t reserved16      :14;   ///<BIT [29:16] reserved
    uint64_t SC              : 1;   ///<BIT [30]    Set Clear SRAM (0: Clear, 1: Set)
    uint64_t CI              : 1;   ///<BIT [31]    Correction Information (0: Uncorrectable, 1: Correctable)
    uint64_t reserved32      :24;   ///<BIT [55:32] reserved
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeCmdfCiabmReadModifyWriteParityError_t;

// Error Code 0x0B: STSM - CCBM Read-Modify-Write Parity Error
typedef struct
{
    uint64_t CMD_INDEX       :16;   ///<BIT [15:00] Command Index
    uint64_t reserved16      :40;   ///<BIT [55:16] reserved
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmDtdbmReadParityError_t;

// Error Code 0x0A: STSM - DTDBM Read-Modify-Write Parity Error
typedef struct
{
    uint64_t CMD_INDEX       : 8;   ///<BIT [07:00] Command Index
    uint64_t reserved8       :48;   ///<BIT [55:08] reserved
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmDtdbmReadModifyWriteParityError_t;

// Error Code 0x0B: STSM - CCBM Read-Modify-Write Parity Error
typedef struct
{
    uint64_t CMD_INDEX       : 8;   ///<BIT [07:00] Command Index
    uint64_t reserved8       :48;   ///<BIT [55:08] reserved
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmCcbmReadModifyWriteParityError_t;

// Error Code 0x0C: CMDF - PCIE Transfer Error
typedef struct
{
    uint64_t PCI_CMD_TOUT    : 1;   ///<BIT [00]    The time-out error when CMDF reqeusts for the Command.
    uint64_t PCI_CMD_ERR     : 2;   ///<BIT [02:01] Command Fetching error.
                                    ///<            Bit[1] = Error occurs during data transfer phase.
                                    ///<            Bit[0] = Error occurs during request phase.
    uint64_t reserved3       : 1;   ///<BIT [03]    reserved
    uint64_t PCI_UGT_TOUT    : 1;   ///<BIT [04]    The time-out error when CMDF requests for the PRP.
    uint64_t PCI_UGT_ERR     : 2;   ///<BIT [06:05] PRP Fetching error
                                    ///<            Bit[1] = Error occurs during data transfer phase.
                                    ///<            Bit[0] = Error occurs during request phase.
    uint64_t reserved7       : 1;   ///<BIT [07]    reserved
    uint64_t RETRY_COUNT     : 3;   ///<BIT [10:08] Retry Count (0: Never retry)
    uint64_t RI              : 1;   ///<BIT [11]    Retry Indicator (0:Fail, 1: Succeed)
    uint64_t PI              : 2;   ///<BIT [13:12] Phase Indicator (0:Read Data Tx, 1: Write Data Tx, 2: Request)
    uint64_t reserved14      : 1;   ///<BIT [14]    reserved
    uint64_t HW_AUTO_RECOVER : 1;   ///<BIT [15]    HW Auto Recover (0: No HW Auto Recover, 1: HW Auto Recover)
    uint64_t reserved16      :14;   ///<BIT [29:16] reserved
    uint64_t QT              : 1;   ///<BIT [30]    Queue Type (1=SQ, 0=CQ)
    uint64_t reserved31      : 1;   ///<BIT [31]    reserved
    uint64_t QID             :16;   ///<BIT [47:32] Queue ID
    uint64_t FID             : 8;   ///<BIT [55:48] Functio ID
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeCmdfPcieTransferError_t;

// Error Code 0x0D: STSM - PCIE Read Error
typedef struct
{
    uint64_t PRP_OFFSET      : 5;   ///<BIT [04:00] PRP Offset STSM needs to read from PKTM.
    uint64_t reserved05_55   :51;   ///<BIT [55:05] reserved
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmPcieReadError_t;

// Error Code 0x0E: CMDF - AXI Transfer Error
typedef struct
{
    uint64_t reserved0       :12;   ///<BIT [11:00]    reserved
    uint64_t PI              : 2;   ///<BIT [13:12] Phase Indicator (0:Read Data Tx, 1: Write Data Tx, 2: Request)
    uint64_t reserved14      : 1;   ///<BIT [14]    reserved
    uint64_t HW_AUTO_RECOVER : 1;   ///<BIT [15]    HW Auto Recover (0: No HW Auto Recover, 1: HW Auto Recover)
    uint64_t reserved16      :14;   ///<BIT [29:16] reserved
    uint64_t QT              : 1;   ///<BIT [30]    Queue Type (1=SQ, 0=CQ)
    uint64_t reserved31      : 1;   ///<BIT [31]    reserved
    uint64_t QID             :16;   ///<BIT [47:32] Queue ID
    uint64_t FID             : 8;   ///<BIT [55:48] Functio ID
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeCmdfAxiTransferError_t;

// Error Code 0x0F: STSM - CABM Read-Modify-Write Parity Error
typedef struct
{
    uint64_t CMD_INDEX       : 8;   ///<BIT [07:00] Command Index
    uint64_t reserved8       :48;   ///<BIT [55:08] reserved
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmCabmReadModifyWriteParityError_t;

// Error Code 0x10: STSM - Aborted Command
typedef struct
{
    uint64_t CMD_INDEX       :16;   ///<BIT [15:00] Command Index
    uint64_t reserved16_55   :40;   ///<BIT [55:16] reserved
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmAbortedCommand_t;

// Error Code 0x11: STSM - Command SRAM Parity Error
typedef struct
{
    uint64_t CMD_INDEX       :16;   ///<BIT [15:00] Command Index
    uint64_t reserved16      :16;   ///<BIT [31:16] reserved
    uint64_t OFFSET          : 5;   ///<BIT [36:32] CSRM DWord Offset
    uint64_t reserved37      :19;   ///<BIT [55:37] reserved
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmCommandSramParityError_t;

// Error Code 0x12: STSM - Sequence ID Not Valid
typedef struct
{
    uint64_t SEQ_ID_VALID    : 8;   ///<BIT [07:00] Sequence ID Valid
    uint64_t reserved8       : 8;   ///<BIT [15:08] Reserved
    uint64_t OFFSET          : 8;   ///<BIT [23:16] Sequence Queue Offset
    uint64_t reserved16      : 8;   ///<BIT [31:24] reserved
    uint64_t PKT_SEQ_NUM     : 3;   ///<BIT [34:32] CSRM DWord Offset
    uint64_t reserved37      :21;   ///<BIT [55:35] reserved
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmSeqIdNotValid_t;

// Error Code 0x13: STSM - Invalid Queue
typedef struct
{
    uint64_t CMD_INDEX       :16;   ///<BIT [15:00] Command Index
    uint64_t MAN_REQ_ID      : 5;   ///<BIT [20:16] Manual Request ID
    uint64_t reserved21      :10;   ///<BIT [30:21] reserved
    uint64_t MAN_REQ_FLAG    : 1;   ///<BIT [31]    Manual Request Flag
    uint64_t QSTATUS         : 1;   ///<BIT [32]    Queue Status (1: not created, 0:has error)
    uint64_t reserved33      :22;   ///<BIT [54:33] reserved
    uint64_t QT              : 1;   ///<BIT [55]    Queue Type (1=SQ, 0=CQ)
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmInvalidQueue_t;

// Error Code 0x14: STSM - CQ Full
typedef struct
{
    uint64_t MAN_REQ_ID      : 5;   ///<BIT [04:00] Manual Request ID
    uint64_t reserved05      :51;   ///<BIT [05:55] reserved
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmCqFull_t;

// Error Code 0x15: STSM - PRP Not Ready
typedef struct
{
    uint64_t MAN_REQ_ID      : 5;   ///<BIT [04:00] Manual Request ID
    uint64_t reserved05      :51;   ///<BIT [05:55] reserved
    uint64_t ERROR_CODE      : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmPrpNotReady_t;

// Error Code 0x16: STSM - PCIE Write Error
typedef struct
{
    uint64_t CMD_INDEX    : 8;   ///<BIT [07:00] Command Index
    uint64_t reserved08   : 8;   ///<BIT [15:08] reserved
    uint64_t MAN_REQ_ID   : 5;   ///<BIT [20:16] Manual Request ID
    uint64_t reserved21   :10;   ///<BIT [21:30] reserved
    uint64_t MAN_REQ_FLAG : 1;   ///<BIT [31]    Manual Request Flag
    uint64_t reserved32   :24;   ///<BIT [55:32] reserved
    uint64_t ERROR_CODE   : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmPcieWriteError_t;

// Error Code 0x17: STSM - IRQ Failed
typedef struct
{
    uint64_t CMD_INDEX    :16;   ///<BIT [15:00] Command Index
    uint64_t MAN_REQ_ID   : 5;   ///<BIT [20:16] Manual Request ID
    uint64_t reserved21   :10;   ///<BIT [21:30] reserved
    uint64_t MAN_REQ_FLAG : 1;   ///<BIT [31]    Manual Request Flag
    uint64_t reserved32   :24;   ///<BIT [55:32] reserved
    uint64_t ERROR_CODE   : 8;   ///<BIT [63:56] ErrorCode
} NvmeStsmIrqFailed_t;

///  @brief Nvme Error Message structure for Status Manager
#pragma anon_unions // unnamed union
typedef union
{
    uint64_t all;   ///< All 64 bits
    uint32_t dw[2]; ///< Access to each dwords
    struct
    {
        uint64_t CMD_INDEX    :16;   ///<BIT [15:00] Command Index
        uint64_t MAN_REQ_ID   : 5;   ///<BIT [20:16] Manual Request ID
        uint64_t reserved21   :10;   ///<BIT [21:30] reserved
        uint64_t MAN_REQ_FLAG : 1;   ///<BIT [31]    Manual Request Flag
        uint64_t QSTATUS      : 1;   ///<BIT [32]    Queue Status (1: not created, 0:has error)
        uint64_t reserved     :22;   ///<BIT [54:33] reserved
        uint64_t QT           : 1;   ///<BIT [55]    Queue Type (1=SQ, 0=CQ)
        uint64_t ERROR_CODE   : 8;   ///<BIT [63:56] ErrorCode
    } b;

    NvmeStsmConfigRegParityError_t cfgParity;                 ///< 0x04: CREG, STSM - NVMe Config Register / MSI-X Table Parity Error
    NvmeStsmDtdbmReadParityError_t dtdbmParity;               ///< 0x09: STSM - DTDBM Read Parity Error
    NvmeStsmDtdbmReadModifyWriteParityError_t dtdbmRmwParity; ///< 0x0A: STSM - DTDBM Read-Modify-Write Parity Error
    NvmeStsmCcbmReadModifyWriteParityError_t ccbmRmwParity;   ///< 0x0B: STSM - CCBM Read-Modify-Write Parity Error
    NvmeStsmPcieReadError_t pcieReadferError;                 ///< 0x0D: STSM - PCIE Transfer Error
    NvmeStsmCabmReadModifyWriteParityError_t cabmRmwParity;   ///< 0x0F: STSM - CABM Read-Modify-Write Parity Error
    NvmeStsmAbortedCommand_t abortedCommand;                  ///< 0x10: STSM - Aborted Command
    NvmeStsmCommandSramParityError_t commandSramParity;       ///< 0x11: STSM - Command SRAM Parity Error
    NvmeStsmSeqIdNotValid_t seqIdNotValid;                    ///< 0x12: STSM - Sequence ID Not Valid
    NvmeStsmInvalidQueue_t invalidQueue;                      ///< 0x13: STSM - Invalid Queue
    NvmeStsmCqFull_t cqFull;                                  ///< 0x14: STSM - CQ Full
    NvmeStsmPrpNotReady_t prpNotReady;                        ///< 0x15: STSM - PRP Not Ready
    NvmeStsmPcieWriteError_t pcieWriteError;                  ///< 0x16: STSM - PCIE Write Error
    NvmeStsmIrqFailed_t irqFailed;                            ///< 0x17: STSM - IRQ Failed
} NvmeStsmErrorMsg_t;

// Error Code 0x2A – DATM (PRP Setup Engine) - LCE SRAM Parity Error
#pragma anon_unions // unnamed union
typedef union
{
    struct                              ///< Packet Type 0, 1 or 2
    {
        uint64_t CMD_INDEX         :16;   ///<BIT [15:00] Command Index
        uint64_t CSRM_DW_OFFSET    : 4;   ///<BIT [19:16] CSRM DWord Offset
        uint64_t reserved19        :11;   ///<BIT [30:20] Reserved
        uint64_t CI                : 1;   ///<BIT [31]    Correction Information (0: Uncorrectable, 1: Correctable)
        uint64_t reserved32        :24;   ///<BIT [55:32] Reserved
        uint64_t ERROR_CODE        : 8;   ///<BIT [63:56] ErrorCode
    };
} NvmeDatmPrpLceSramParityError_t;


// Error Code 0x4B, /// 0x4B DATM (PRP Fetcher) - Packet Manager Error
typedef union
{
    struct                              ///< Packet Type 0, 1 or 2
    {
        uint64_t CMD_INDEX         :16;   ///<BIT [15:00] Command Index
        uint64_t PktMStat          : 3;   ///<BIT [18:16] Pkt Mgr Status
        uint64_t reserved19        :37;   ///<BIT [55:19] reserved
        uint64_t ERROR_CODE        : 8;   ///<BIT [63:56] ErrorCode
    };
} NvmeDatmPrpFetchError_t;

// Error Code 0x6C – DATM (Read Engine) - End-to-end / Data Path Protection Errors
#pragma anon_unions // unnamed union
typedef union
{
    struct                              ///< Packet Type 0, 1 or 2
    {
        uint64_t CMD_INDEX         :16;   ///<BIT [15:00] Command Index
        uint64_t AU_INDEX          :16;   ///<BIT [31:16] AU Index
        uint64_t MPECC_ERR         : 1;   ///<BIT [32]    MPECC Check Error
        uint64_t HLBA_ERR          : 1;   ///<BIT [33]    HLBA Check Error
        uint64_t BCRC_ERR          : 1;   ///<BIT [34]    MPECC Check Error
        uint64_t E2E_GUARD_ERR     : 1;   ///<BIT [35]    T10 End-To-End Guard Check Error
        uint64_t E2E_LBA_ERR       : 1;   ///<BIT [36]    T10 End-To-End Logical Block Application Tag Check Error
        uint64_t E2E_LBR_ERR       : 1;   ///<BIT [37]    T10 End-To-End Logical Block Reference Tag Check Error
        uint64_t MPECC_ERR_SEC_NUM : 4;   ///<BIT [47:44] MPECC Error Sector Number
        uint64_t HLBA_ERR_SEC_NUM  : 4;   ///<BIT [51:48] HLBA Error Sector Number
        uint64_t BCRC_ERR_SEC_NUM  : 4;   ///<BIT [55:52] BCRC Error Sector Number
        uint64_t ERROR_CODE        : 8;   ///<BIT [63:56] ErrorCode
    };
} NvmeDatmReE2eDppError_t;

// Error Code 0x83 – DATM (Write Engine) - Packet Manafer Inferface Error
#pragma anon_unions // unnamed union
typedef union
{
    struct                              ///< Packet Type 0, 1 or 2
    {
        uint64_t CMD_INDEX         :16;   ///<BIT [15:00] Command Index
        uint64_t PktMStat          : 3;   ///<BIT [18:16] Pkt Mgr Status
        uint64_t reserved19        :37;   ///<BIT [55:19] reserved
        uint64_t ERROR_CODE        : 8;   ///<BIT [63:56] ErrorCode
    };
} NvmeDatmWePacketManIfError_t;

///  @brief Nvme Error Message structure for Data Manager
#pragma anon_unions // unnamed union
typedef union
{
    uint64_t all;   ///< All 64 bits
    uint32_t dw[2]; ///< Access to each dwords
    struct
    {
        uint64_t VALUE       :32;                   ///<BIT [31:0]  depends on error code
        uint64_t QID         :16;                   ///<BIT [47:32] Queue ID
        uint64_t FID         : 8;                   ///<BIT [55:48] Functio ID
        uint64_t ERROR_CODE  : 8;                   ///<BIT [63:56] ErrorCode
    } b;

    NvmeDatmPrpLceSramParityError_t prpLceSramparity; ///< 0x2A: DATM (PRP Setup Engine) - LCE SRAM Parity Error
    NvmeDatmPrpFetchError_t         prpFetchErr;      ///< 0x4B: DATM (PRP Fetcher) - Packet Manager Error
    NvmeDatmReE2eDppError_t         reE2eDpp;         ///< 0x6C: DATM (Read Engine) - End-to-end / Data Path Protection Errors
    NvmeDatmWePacketManIfError_t    wePktMgrStatus;   ///< 0x83: DATM (Write Engine) - Packet Manafer Inferface Error
} NvmeDatmErrorMsg_t;

typedef NvmeErrorMsg64_t NvmeCmdfErrorMsg_t;    // Command fetcher error message
typedef NvmeErrorMsg64_t NvmePktmErrorMsg_t;    // Packet manager error message
typedef NvmeErrorMsg64_t NvmeCregErrorMsg_t;    // Controller Register error message

//-----------------------------------------------------------------------------
//  Private function proto-type definitions:
//-----------------------------------------------------------------------------
static void NotifyPersistentInternalError(IrqHandler_t irqHandler[], uint32_t errorCode);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data proto-type without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-type without header include
//-----------------------------------------------------------------------------
extern void HalNvme_AddMcqIntoRepostFifo(uint16_t mcqIndex);

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
    uint16_t cmdIndex;
    NvmeStsmErrorMsg_t errMsg;
    StatManagerErrFifoStat_t errorFifoStatus;
    HcmdQ_t* pHcmdQ;
    MsgHostIO_t* pHcmdMsg;

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
        errMsg.dw[1] = rNvmeStatMngr.ERR_RPT_FIFO_RD_PORT; // high DWORD
        errMsg.dw[0] = rNvmeStatMngr.ERR_RPT_FIFO_RD_PORT; // low  DWORD

        D_PRINTF(" EC=[%2X %X %X] Cnt[%4d]\n", errMsg.b.ERROR_CODE, errMsg.dw[0], errMsg.dw[1],errorFifoStatus.b.ERR_RPT_VAL_ENT_CNT);

        mcqIndex = errMsg.b.MAN_REQ_ID;

        switch(errMsg.b.ERROR_CODE)
        {
        case cStsmCqFull:
        case cStsmPrpNotReady:
            mcqIndex = errMsg.cqFull.MAN_REQ_ID;
            HalNvme_AddMcqIntoRepostFifo(mcqIndex);
            break;
        case cStsmPcieWriteError:
            // TODO: Resend status
            break;
        case cStsmIrqFailed:
            // TODO: Resend status
            break;
        case cStsmInvalidQueue:
            if (errMsg.invalidQueue.MAN_REQ_FLAG==1)
            {
                BIT_CLR(NvmeHba.mcqInUseBitmap, BIT(mcqIndex)); // abort mcq posting
                D_PRINTF(" ABORT MCQ:%2X", mcqIndex);
            }

            BIT_CLR(ACTIVATED_DW_FW(errMsg.invalidQueue.CMD_INDEX/32), BIT(errMsg.invalidQueue.CMD_INDEX%32));
            rNvmeBitmapW1c.CCBM[errMsg.invalidQueue.CMD_INDEX/32] = BIT(errMsg.invalidQueue.CMD_INDEX%32);
            DECREASE_COMPLETED_COUNT(1);


            rNvmeBitmapW1c.CABM0[errMsg.invalidQueue.CMD_INDEX/32] = BIT(errMsg.invalidQueue.CMD_INDEX%32);

            pHcmdQ = &gHcmdQ[errMsg.invalidQueue.CMD_INDEX];
            pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);
            if ((pHcmdMsg->hdr.b.status >= cMsgExecuted) && (pHcmdMsg->hdr.b.status != cMsgCompleted))
            {
                BEGIN_CS1
                pHcmdMsg->bufDesc.cssTemp = CACHE_ABORT;
                NvmeCmd_ReturnCacheMsg(pHcmdQ);
                END_CS1
                D_PRINTF("\nErr--ResCache: CMD[%04d]\n", errMsg.invalidQueue.CMD_INDEX);
            }

            break;
        case cStsmAbortedCommand:
            cmdIndex = errMsg.abortedCommand.CMD_INDEX;
            //D_PRINTF(" ABORT CMD_INDEX %2X\n", errMsg.abortedCommand.CMD_INDEX);
            D_PRINTF("\tAbort CID chk[%3d - %3d], act[%X]\n",cmdIndex,gNvmeAbortCmdInfo.Id,gNvmeAbortCmdInfo.active);
            if (gNvmeAbortCmdInfo.active == TRUE)
            {
                if(cmdIndex == gNvmeAbortCmdInfo.Id)
                {
                    gNvmeAbortCmdInfo.active = FALSE;
                }
            }
            break;
        default: // persitent internal error for all others
            NotifyPersistentInternalError(irqHandler, errMsg.b.ERROR_CODE);
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

        switch(errMsg.b.ERROR_CODE)
        {
            default: // persitent internal error for all
                NotifyPersistentInternalError(irqHandler, errMsg.b.ERROR_CODE);
                break;
        }
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

        switch(errMsg.b.ERROR_CODE)
        {
        case cCmdfAxiTransferError:
            // FALL THROUGH
        case cCmdfCurPrpTableParity:
            // TODO: Log only, retry by HW
            break;

        default: // persitent internal error for all others
            NotifyPersistentInternalError(irqHandler, errMsg.b.ERROR_CODE);
            break;
        }
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
    uint16_t cmdIndex;
    HcmdQ_t* pHcmdQ;
    MsgHostIO_t* pHcmdMsg;

    errorFifoCount = rNvmeDataMngr.dataManagerErrInformat.b.ERR_CNT;

    D_PRINTF("[DATM_ERR]Cnt[%X]", errorFifoCount);
    while(errorFifoCount--)
    {
        errMsg.dw[1] = rNvmeDataMngr.DATM_ERR_DWORD[1];
        errMsg.dw[0] = rNvmeDataMngr.DATM_ERR_DWORD[0];
        D_PRINTF("\n\tEC=%2X %8X %8X", errMsg.b.ERROR_CODE, errMsg.dw[0], errMsg.dw[1]);

        switch(errMsg.b.ERROR_CODE)
        {
        case cDatmReE2eDppErrors:
            if(errMsg.reE2eDpp.HLBA_ERR)
            {
                D_PRINTF(" CMDIDX=%4X AU_INDEX=%4X HLBA_ERR_SEC_NUM=%X\n",
                    errMsg.reE2eDpp.CMD_INDEX, errMsg.reE2eDpp.AU_INDEX, errMsg.reE2eDpp.HLBA_ERR_SEC_NUM);
                cmdIndex = errMsg.reE2eDpp.CMD_INDEX;

                // Read command Abort without Streaming (When HWLL is not involved)
                // 1. Set "AB" bit, and associated bit in RDRBM in read
                // 2. The controller reports the abort completion through the Status Manager Error FIFO.
                rNvmeHwIoCmd[cmdIndex].dw0.b.AB = 1;
                BA_WR1_U32_L2M(rNvmeBitmapW1s.RDRBM0, cmdIndex);

                // Read command Abort with Streaming (When HWLL is involved)
                // TODO: Add a read command abort when HWLL is involved

            }
            break;
        case cDatmReAxiIfErrorFromSsdFetcher:
        case cDatmReE2eDppAesErrors:
            // TODO: Retransmit data
            break;

        case cDatmWeE2eDppErrors:
        case cDatmWeAxiErrorFromSsdFetcher:
        case cDatmWeDataFifoParityError:
            // TODO: Retransmit data
            break;

        case cDatmPrpfPacketManagerError:
            cmdIndex                = errMsg.prpFetchErr.CMD_INDEX;
            pHcmdQ                  = &gHcmdQ[cmdIndex];
            pHcmdMsg                = (MsgHostIO_t*) &smHcmdMsg[cmdIndex];
            if (pHcmdMsg->hdr.b.status != cMsgCmdAbort)
            {
                Host_PushToTail(cHcsCmdAbort, pHcmdQ);
            }
            pHcmdMsg->hdr.b.status  = cMsgCmdAbort;
            break;

        case cDatmWePacketManagerIfError:
            cmdIndex                  = errMsg.wePktMgrStatus.CMD_INDEX;
            pHcmdQ                    = &gHcmdQ[cmdIndex];
            pHcmdMsg                  = (MsgHostIO_t*) &smHcmdMsg[cmdIndex];
            pHcmdMsg->bufDesc.cssTemp = CACHE_ABORT;
            NvmeCmd_ReturnCacheMsg(pHcmdQ);
            if (pHcmdMsg->hdr.b.status != cMsgCmdAbort)
            {
                Host_PushToTail(cHcsCmdAbort, pHcmdQ);
            }
            pHcmdMsg->hdr.b.status  = cMsgCmdAbort;
            break;

        default: // persitent internal error for all others
            NotifyPersistentInternalError(irqHandler, errMsg.b.ERROR_CODE);
            break;
        }
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
    HostErrorInfo_t errorInfo;

    errorFifoCount = rNvme.ctrlRegisterModuleErrFifoCnt.b.CREG_ERR_FIFO_CTR;

    D_PRINTF("[CREG_ERR:%X]", errorFifoCount);

    while(errorFifoCount--)
    {
        errMsg.dw[1] = rNvme.CREG_ERR_FIFO_DW[1];
        errMsg.dw[0] = rNvme.CREG_ERR_FIFO_DW[0];

        D_PRINTF(" EC=%2X FID=%2X QID=%4X VAL=%X", errMsg.b.ERROR_CODE, errMsg.b.FID, errMsg.b.QID, errMsg.b.VALUE);

        // TODO: Add code to handle error

        switch(errMsg.b.ERROR_CODE)
        {
        case cCregInvalidDoorbell:   //
            errorInfo.error = cEcNvmeInvalidDoorbellWriteValue;
            errorInfo.cmdId = 0;
            errorInfo.value = 0;
            irqHandler[cIrqHostIfError]((uint32_t)&errorInfo);
            break;
        case cCregDbOnNonExistQueue: // Doorbell on non-exist Queue
            errorInfo.error = cEcNvmeWriteToInvalidDoorbell;
            errorInfo.cmdId = 0;
            errorInfo.value = 0;
            irqHandler[cIrqHostIfError]((uint32_t)&errorInfo);
            break;
        default: // persitent internal error for all others
            NotifyPersistentInternalError(irqHandler, errMsg.b.ERROR_CODE);
            break;
        }
    }

    D_PRINTF("\n");
}

//-----------------------------------------------------------------------------
/**
    Notify to host persistent internal error.
    Host should perform a reset in order to recovery.

    @param[in]  irqHandler      software interrupt handler table.
    @param[in]  errorCode       internal error code
**/
//-----------------------------------------------------------------------------
static void NotifyPersistentInternalError(IrqHandler_t irqHandler[], uint32_t errorCode)
{
    HostErrorInfo_t errorInfo;

    errorInfo.error = cEcNvmePersistentInternal; // + ((errorCode&0xFF)<<8);
    errorInfo.cmdId = 0;
    errorInfo.value = 0;
    irqHandler[cIrqHostIfError]((uint32_t)&errorInfo);

    rNvmeController.controller[0].CSTS.b.CFS = 1; // set controller fatal error
}


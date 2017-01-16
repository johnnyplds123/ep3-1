#pragma once
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
//! @brief <b> System standard error code definitions </b>
//!
//
//  THIS IS FOR PURE DEFINITION ONLY, DO NOT USE ANY CONDITIONAL COMPILE OPTION !!
//
//=============================================================================

//-----------------------------------------------------------------------------
// System error code enumeration
//-----------------------------------------------------------------------------
//
// RULES
//
// 1. GLOBALLY UNIQUE ERROR CODES
//    Don't reuse error code for different meaning.
// 2. ERROR CODES
//    xxxx_0000h               : DO NOT USE except 0000_0000h for no error.
//    xxxx_0001h               : Reserved for general failure.
//    0000_0000h .. 0FFF_FFFFh : Reserved for system generic error codes.
//    1000_0000h .. 1FFF_FFFFh : Reserved for CORE HAL error codes.
//    2010_0000h .. 201x_xxxxh : Reserved for SATA I/F error codes. (Phy/Link/Transport)
//    2020_0000h .. 202x_xxxxh : Reserved for PCIe I/F error codes. (Phy/Link/Transport)
//    2030_0000h .. 203x_xxxxh : Reserved for AHCI protocol error codes.
//    3000_0000h .. 3FFF_FFFFh : Reserved for general media error codes.
//    4000_0000h .. 4FFF_FFFFh : Reserved for NAND Media specific error codes.
//

/// @brief System error codes
typedef enum
{
    // SYSTEM GENERIC ERROR CODE                    // 0000_xxxxh
    cEcNoError                      = 0x00000000,   ///< No error, OK or Passed
    cEcError                        = 0x00000001,   ///< Error, Not OK or Failed
    cEcNotAligned                   = 0x00000002,   ///< Address, size or value is not properly aligned
    cEcTimeOut                      = 0x00000003,   ///< Time out error
    cEcHalt                         = 0x00000004,   ///< Fimrware Halt
    cEcAssertFail                   = 0x00000005,   ///< Fimrware Assertion failed
    cEcOutOfMemory                  = 0x00000006,   ///< Out of memory
    cEcMisCompare                   = 0x00000007,   ///< Data mis-compare
    cEcFifoFull                     = 0x00000008,   ///< FIFO full
    cEcFifoEmpty                    = 0x00000009,   ///< FIFO Empty

    // HAL TBG                                      // 1000_xxxxh
    cEcTbgGeneral                   = 0x10000001,   ///< General TBG HW related failure
    cEcTbgAUnlock                   = 0x10000002,   ///< TBG A Unlocked
    cEcTbgBUnlock                   = 0x10000003,   ///< TBG B Unlocked
    cEcTbgCUnlock                   = 0x10000004,   ///< TBG C Unlocked
    cEcTbgDUnlock                   = 0x10000005,   ///< TBG D Unlocked
    cEcTbgWrongFreq                 = 0x10000006,   ///< Wrong frequency setup chosen.

    // HAL CPU                                      // 1001_xxxxh
    cEcCpuGeneral                   = 0x10010001,   ///< General CPU related failure
    cEcCpuDabort                    = 0x10010002,   ///< Data abort
    cEcCpuPabort                    = 0x10010003,   ///< Prefetch abort
    cEcCpuUndefInst                 = 0x10010004,   ///< Undefined instruction
    cEcCpuSwi                       = 0x10010005,   ///< Software Interrupt exception

    // HAL SRAM/TCM                                 // 1002_xxxxh
    cEcSramGeneral                  = 0x10020001,   ///< General SRAM/TCM related failure

    // HAL DRAM                                     // 1003_xxxxh
    cEcDramGeneral                  = 0x10030001,   ///< General DRAM related failure
    cEcDramNotReady                 = 0x10030002,   ///< DRAM not ready
    cEcDramInitFail                 = 0x10030003,   ///< DRAM initialization failure
    cEcDramAddrDetectionFail        = 0x10030004,   ///< DRAM address configuration detection failure
    cEcDramCapDetectionFail         = 0x10030005,   ///< DRAM capacity detection failure

    // HAL Timer                                    // 1004_xxxxh
    cEcTimerGeneral                 = 0x10040001,   ///< General Timer HW related failure

    // HAL UART                                     // 1005_xxxxh
    cEcUartGeneral                  = 0x10050001,   ///< General UART HW related failure
    cEcUartSendInitRetry            = 0x10050010,   ///< General UART HW related failure
    cEcUartSendInitAbort            = 0x10050011,   ///< General UART HW related failure
    cEcUartSendCancelled            = 0x10050012,   ///< General UART HW related failure
    cEcUartSendEOT                  = 0x10050013,   ///< General UART HW related failure
    cEcUartSendPacketRetry          = 0x10050014,   ///< General UART HW related failure
    cEcUartSendPacketFail           = 0x10050015,   ///< General UART HW related failure

    // HAL Serial Flash                             // 1006_xxxxh
    cEcSflashGeneral                = 0x10060001,   ///< General Serial flash HW related failure
    cEcSflashBusyTimeOut            = 0x10060002,   ///< Busy time out
    cEcSflashProgramTimeOut         = 0x10060003,   ///< Program timeout
    cEcSflashEraseTimeOut           = 0x10060004,   ///< Erase timeout
    cEcSflashCantSendCommand        = 0x10060010,   ///< Can't send command
    cEcSflashCantGetStatus          = 0x10060011,   ///< Can't get status
    cEcSflashCantSendAddress        = 0x10060012,   ///< Can't send address
    cEcSflashWriteProtected         = 0x10060020,   ///< Write protected
    cEcSflashInvalidateFirmware     = 0x10060030,   ///< Bad Firmware image from/to Serial flash.
    cEcSflashCheckSumError          = 0x10060031,   ///< Checksum error of Firmware (either FW_MAIN or FW_NAND)

    // HAL IPC                                      // 1007_xxxxh
    cEcIpcGeneral                   = 0x10070001,   ///< General IPC HW related failure

    // HAL DMA Controller                           // 1008_xxxxh
    cEcDmacGeneral                  = 0x10080001,   ///< General DMAC related failure
    cEcDmacCompare                  = 0x10080002,   ///< DMAC compare error

    // HAL I2C                                      // 1009_xxxxh
    cEcI2cGeneral                   = 0x10090001,   ///< General I2C HW related failure
    cEcI2cTimeout                   = 0x10090002,   ///< I2C time out

    // PCIe general errors                          // 202x_xxxxh
    cEcPcieGeneral                  = 0x20200001,   ///< PCIe General error
    cEcPcieCorrInt                  = 0x20200002,   ///< PCIe corrected internal - ERR_COR
    cEcPcieUcorrInt                 = 0x20200003,   ///< PCIe uncorrected internal - ERR_FATAL
    cEcPcieHdrLogOverflow           = 0x20200004,   ///< PCIe header log overflow - ERR_COR

    // PCIe PL errors                               // 2021_xxxxh
    cEcPciePlGeneral                = 0x20210001,   ///< PCIe PL General error
    cEcPcieRcvrErr                  = 0x20210002,   ///< PCIe receiver error - ERR_COR

    // PCIe DLL errors                              // 2022_xxxxh
    cEcPcieDllGeneral               = 0x20220001,   ///< PCIe DLL General error
    cEcPcieBadTlp                   = 0x20220002,   ///< PCIe bad TLP - ERR_COR
    cEcPcieBadDllp                  = 0x20220003,   ///< PCIe bad DLLP - ERR_COR
    cEcPcieReplayTimerTo            = 0x20220004,   ///< PCIe replay timer timed out - ERR_COR
    cEcPcieReplayCntrEx             = 0x20220005,   ///< PCIe replay counter exhausted - ERR_COR
    cEcPcieDlProtoErr               = 0x20220006,   ///< PCIe data link protocol error - ERR_FATAL
    cEcPcieSurpriseDown             = 0x20220007,   ///< PCIe surprise down - ERR_FATAL

    // HAL PCIe TL errors                           // 2023_xxxxh
    cEcPcieTlGeneral                = 0x20230001,   ///< PCIe TL General error
    cEcPciePoisonedTlpRcv           = 0x20230002,   ///< PCIe poisoned TLP received - ERR_NONFATAL
    cEcPcieEcrcFail                 = 0x20230003,   ///< PCIe ECRC failed - ERR_NONFATAL
    cEcPcieUnsuppReq                = 0x20230004,   ///< PCIe unsupported request - ERR_NONFATAL
    cEcPcieCompTo                   = 0x20230005,   ///< PCIe completion timeout - ERR_NONFATAL
    cEcPcieCompAbort                = 0x20230006,   ///< PCIe completion abort - ERR_NONFATAL
    cEcPcieUnexpComp                = 0x20230007,   ///< PCIe unexpected completion - ERR_CORR
    cEcPcieAcsViol                  = 0x20230008,   ///< PCIe ACS violation - ERR_NONFATAL
    cEcPcieMcBlckdTlp               = 0x20230009,   ///< PCIe multicast blocked TLP - ERR_NONFATAL
    cEcPcieAtmOpEgBlckd             = 0x20230010,   ///< PCIe atomic operation egress blocked - ERR_CORR
    cEcPcieRcvrOverflow             = 0x20230011,   ///< PCIe receiver overflow - ERR_FATAL
    cEcPcieFcProtoErr               = 0x20230012,   ///< PCIe flow control protocol error - ERR_FATAL
    cEcPcieMalformedTlp             = 0x20230013,   ///< PCIe mal formed TLP - ERR_FATAL
    cEcPcieTlpPrefBlckd             = 0x20230014,   ///< PCIe TLP prefix blocked - ERR_NONFATAL

    // MONITOR/SATA COMMAND HANDLING                // 30xx_xxVVh (VV = SATA 8bit error register value)
    cEcInvalidCommand               = 0x30000004,   ///< Invalid command code
    cEcInvalidLba                   = 0x30000110,   ///< Invalid Logical Block Address
    cEcInvalidParameter             = 0x30000204,   ///< Invalid parameter but valid command code
    cEcMissingParam                 = 0x30000302,   ///< Missing parameters, command aborted
    cEcSecurityLock                 = 0x30000440,   ///< Security locked, command aborted
    cEcSecurityFrozen               = 0x30000450,   ///< Frozen check command aborted
    cEcNcqTagCollision              = 0x30000504,   ///< NCQ tag collision
    cEcNcqMixedCommand              = 0x30000604,   ///< NCQ mixed command (NCQ and non-queue)
    cEcSataError                    = 0x30000704,   ///< sata error

    // SATA I/F CRC                                 // 301x_xxVVh (VV = SATA 8bit error register value)
    cEcCrc                          = 0x30100084,   ///< General CRC error
    cEcRxDataCrc                    = 0x30100184,   ///< Rx data FIS CRC error
    cEcRxDataDisparity              = 0x30100284,   ///< Rx data FIS disparity error (and no CRC)
    cEcRxT10Crc                     = 0x30100384,   ///< Rx T10-CRC error
    cEcRxDataSync                   = 0x30100484,   ///< Rx data FIS SYNC error
    cEcRxFisSize                    = 0x30100584,   ///< Rx data FIS size error
    cEcDpp                          = 0x30100684,   ///< DPP Error
    cEcTxD2hFis                     = 0x30100784,   ///< Tx R_ERR of D2H Reg FIS
    cEcTxDataFis                    = 0x30100884,   ///< Tx R_ERR of Data FIS
    cEcTxDmaSetupFis                = 0x30100984,   ///< Tx R_ERR of DMA setup FIS
    cEcTxDmaActive                  = 0x30100A84,   ///< Tx R_ERR of D2H Reg FIS/DMA activate FIS
    cEcLinkError                    = 0x30100B84,   ///< Link code or disparity error
    cEcTxDataSyncError              = 0x30100C84,   ///< Tx Data Sync error
    cEcRxDataSyncError              = 0x30100D84,   ///< Rx Data Sync error
    cEcFifoError                    = 0x30100E84,   ///< Tm FIFO error detected
    cEcRerrDError                   = 0x30100F84,   ///< R_ERR in Rx Data FIS
    cEcTRerrError                   = 0x30101084,   ///< Tx R_ERR Error
    cEcRLError                      = 0x30101184,   ///< Rx Length Error
    cEcRFlowError                   = 0x30101284,   ///< Rx Flow Error
    cEcFisRerrError                 = 0x30101384,   ///< RX Bad FIS R_ERR
    cEcCrcError                     = 0x30101484,   ///< Rx CRC Error


    // NVME I/F Error code                          // 31xx_xxVVh (StatusCode[7:0] + StatusType[11:8] + More[14] + NDR[15])
    cEcNvmeErrorBase                = 0x31000000,   // see NvmeStatusCodes.h for NVMe specific error code
    cEcNvmeAsyncErrorBase           = 0x31800000,   // Asynchronous error code - 318x_xxxxh
    cEcNvmeWriteToInvalidDoorbell   = 0x31800000,   // Host software wrote the doorbell of queue that was not created
    cEcNvmeInvalidDoorbellWriteValue= 0x31800001,   // Host software attempted to write an invalid doorbell value. Some possible causes of this error are
    cEcNvmeDiagnosticFailure        = 0x31800002,   // A diagnostic failure was detected.
    cEcNvmePersistentInternal       = 0x31800003,   // Persistent Internal Error
    cEcNvmeTransientInternal        = 0x31800004,   // Transient Internal Error
    cEcNvmeFirmwareImageLoad        = 0x31800005,   // The firmware image could not be loaded

    // Translation of mioErrors
    cEcMioErrorBase                 = 0x32000000,
    cEcMioErasedAU                  = 0x32000001,
    cEcMioUncorrectable             = 0x32000002,
    cEcMioOverlimit                 = 0x32000003,
    cEcMioProgramOpErr              = 0x32000010,
    cEcMioEraseOpErr                = 0x32000011,
    cEcMioGeneralErr                = 0x32000020,

    // NAND I/F                                     // 4000_xxxxh
    cEcNfifGeneral                  = 0x40000001,   ///< General failure
    cEcNfifUnknownTarget            = 0x40000002,   ///< Unknown target NAND flash
    cEcNfifFeatureMismatch          = 0x40000003,   ///< feature mismatch
    cEcNfifNoFound                  = 0x40000004,   ///< No found
    cEcNfifNotSupport               = 0x40000005,   ///< Not supported DataIf and XferMode

    // NAND Indirect I/F                            // 4002_xxxxh
    cEcNfimGeneral                  = 0x40020001,   ///< General Indirect mode related failure
    cEcNfimFifoRdTimeOut            = 0x40020001,   ///< Indirect mode Read FIFO ready time out
    cEcNfimFifoWrTimeOut            = 0x40020002,   ///< Indirect mode Write FIFO ready time out

    // NAND SEQ I/F                                 // 4003_xxxxh
    cEcSeqGeneral                   = 0x40030001,   ///< General Sequencer mode related failure
    cEcSeqInvalidCfn                = 0x40030002,   ///< Invalid SEQ Configuration function number
    cEcSeqInvalidCfnParam           = 0x40030003,   ///< Invalid SEQ Configuration function parameters
    cEcSeqConfigError               = 0x40030004,   ///< SEQ Configuration error
    cEcSeqFatalError                = 0x40030005,   ///< SEQ fetal error

    // NAND ECU                                     // 4004_xxxxh
    cEcEcuGeneral                   = 0x40040001,   ///< General ECU related failure
    cEcEcuInvalidCfn                = 0x40040002,   ///< Invalid ECU Configuration function number
    cEcEcuInvalidCfnParam           = 0x40040003,   ///< Invalid ECU Configuration function parameters
    cEcEcuConfigError               = 0x40040004,   ///< ECU Configuration error
    cEcEcuPowerInfoError            = 0x40040005,   ///< ECU Power information error
    cEcEcuXferError                 = 0x40040006,   ///< ECU Transfer count error
    cEcEcuFatalError                = 0x40040007,   ///< ECU fetal error

    // NAND FLC                                     // 4005_xxxxh
    cEcFlcGeneral                   = 0x40050001,   ///< General FLC related failure
    cEcFlcConfigError               = 0x40050002,   ///< FLC Configuration error

    // NAND PKG                                     // 4006_xxxxh
    cEcPkgGeneral                   = 0x40060001,   ///< General PKG related failure
    cEcPkgConfigError               = 0x40060002,   ///< Configuration error

    // NAND DGE                                     // 4007_xxxxh
    cEcDgeGeneral                   = 0x40070001,   ///< General DGE related failure
    cEcDgeConfigError               = 0x40070002,   ///< Configuration error

    // NAND RRD                                     // 4008_xxxxh
    cEcRrdGeneral                   = 0x40080001,   ///< General RRD related failure
    cEcRrdConfigError               = 0x40080002,   ///< Configuration error

    // NAND RTM                                     // 4009_xxxxh
    cEcRtmGeneral                   = 0x40090001,   ///< General RTM related failure
    cEcRtmConfigError               = 0x40090002,   ///< Configuration error

    // FCT                                          // 400A_xxxxh
    cEcFctGeneral                   = 0x400A0001,   ///< General FCT related failure
    cEcFctConfigError               = 0x400A0002,   ///< Configuration error

    // NAND HLD MIO                                 // 401x_xxxxh
    cEcMioGeneral                   = 0x40100001,   ///< General MIO related failure
    cEcMioInvalidJob                = 0x40100002,   ///< Invalid MIO JOB
    cEcMioInvalidDie                = 0x40100003,   ///< Invalid
    cEcMioInvalidBlk                = 0x40100004,   ///< Invalid
    cEcMioInvalidPage               = 0x40100005,   ///< Invalid
    cEcMioInvalidJobCmd             = 0x40100006,   ///< Invalid
    cEcMioInvalidJobStatus          = 0x40100007,   ///< Invalid
    cEcMioInvalidJobPriority        = 0x40100008,   ///< Invalid
    cEcMioInvalidEcuMode            = 0x40100009,   ///< Invalid
    cEcMioPushJobError              = 0x40100010,   ///< Push MIO JOB to list has an error
    cEcMioPopJobError               = 0x40100011,   ///< Pop MIO JOB from list has an error
    cEcNoMioInternalResources       = 0x40100012,   ///< MIO is running out of internal resources
    cEcMioInAbortState              = 0x40100013,   ///< MIO is performing an abort sequence
    cEcMioNotIdle                   = 0x40100014,   ///< MIO is not IDLE state
    cEcNoOutstandingCmd             = 0x40100015,   ///< No outstanding command (Mio is in IDLE state)
    cEcMioAbortPendingAlready       = 0x40100016,   ///< MIO is already in the abort state
    cEcUnsupportedAbort             = 0x40100017,   ///< Incorrect Abort Request

    // NAND HLD NIO                                 // 4011_xxxxh
    cEcNioGeneral                   = 0x40110001,   ///< General NIO related failure

    // NAND FTL                                     // 4020_xxxxh
    cEcFtlGeneral                   = 0x40200001,   ///< General FTL related failure
    cEcFtlInvalidPaa                = 0x40200002,   ///< Invalid LAA error.
    cEcFtlJobBufferError            = 0x40200003,   ///< MIO Job buffer has an error.
    cEcFtlJobAllocError             = 0x40200004,   ///< MIO Job buffer has an error.
    cEcFtlJobOverflowError          = 0x40200005,   ///< Try to allocate MIO Job over the maximum number.

    // NAND FTL FILE                                // 4021_xxxxh
    cEcFtlFileGeneral               = 0x40210001,   ///< General FTL related failure
    cEcFtlFileIdMismatch            = 0x40210002,   ///< File Id mismatched
    cEcFtlFileRevMismatch           = 0x40210003,   ///< File Revision mismatched
    cEcFtlFileCrcChksum             = 0x40210004,   ///< CRC or checksum error
    cEcFtlFileIdefectLoadFail       = 0x40210010,   ///< I Defect file load failed
    cEcFtlFileIdefectWriteFail      = 0x40210011,   ///< I Defect file write failed
    cEcFtlFileAnchorLoadFail        = 0x40210012,   ///< Anchor file load failed
    cEcFtlFileAnchorWriteFail       = 0x40210013,   ///< Anchor file write failed

    // NAND DM                                      // 403x_xxxxh
    cEcDmGeneral                    = 0x40300001,   ///< General Defect Manager related failure
    cEcDmIdefectOverflow            = 0x40300002,   ///< Initial Bad blocks exceeds its expectation
    cEcDmFactoryListFull            = 0x40300003,   ///< factory list full
    cEcDmWorkingListFull            = 0x40300004,   ///< working list full
    cEcDmGrownListFull              = 0x40300005,   ///< grown list full
    cEcDmInvalidWorkingList         = 0x40300006,   ///< Invalid working list
    cEcDmInvalidFactoryList         = 0x40300007,   ///< Invalid factory list
    cEcDmInvalidCommand             = 0x40300010,   ///< Invalid BBM command

    // Address Mapping                              // 404x_xxxxh
    cEcAmGeneral                    = 0x40400001,   ///< weru number exceeds limit
    cEcAmWeruNumberTooHigh          = 0x40400002,   ///< weru number exceeds limit
    cEcAmBlockOffsetTooHigh         = 0x40400003,   ///< block offset exceeds limit

    // FTL Allocation                               // 405x_xxxxh
    cEcFtlAllocationFailed          = 0x40500001,   ///< can't alloaction PMAs

    // Power Down                                   // 406x_xxxxh
    cEcPmDeviceBusy                 = 0x40600001,   ///< can't enter power down mode

    // IDS Read Error                               // 407x_xxxxh
    cEcIdsReadFail                  = 0x40700001,   ///< IDS Read Retry Fail

} Error_t;


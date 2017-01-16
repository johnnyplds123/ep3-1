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
//! @brief NVMe Submission/Completion queue header
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
/// Common Command Values (4. System Memory Structures)
/// @brief NVMe Fused operation (FUSE)
typedef enum
{
    cNormalOperation = 0,
    cFusedFirstCommand = 1,
    cFusedSecondCommand = 2,

} FusedOperation_t;

/// @brief NVMe SGL Descriptor Type
typedef enum
{
    cSglDataBlockDescriptor = 0,
    cSglBitBucketDescriptor = 1,
    cSglSegmentDescriptor = 2,
    SGL_LAST_SEGMENT_DESCRIPTOR = 3,

} SglDescriptorType_t;

/// Admin Command Values (5. Admin Command Set)
/// @brief Create I/O Submission Queue Value Dword 11 - Queue Priority (QPRIO)
typedef enum
{
    cUrgent = 0,
    cHigh = 1,
    cMedium = 2,
    cLow = 3,

}  CreateIoSubmissionQDword11QueuePriority_t;


/// @brief Firmware Activate Command Dword 10 - Activate Action (AA)
typedef enum
{
    cDownloadedImageReplacesFirmwareSlotNotActivate = 0,
    cDownloadedImageReplacesFirmwareSlotActivateNextReset = 1,
    cFirmwareSlotActivateNextReset = 2,

} FirmwareActivateCommandDword10ActivateAction_t;

/// @brief Get Features Command Dword 10 - Select (SEL)
typedef enum
{
    cCurrent = 0,
    cDefault = 1,
    cSaved = 2,
    cSupportedCapabilities = 3,

} GetFeaturesCmdDword10Select_t;

/// @brief Identify LBA Format Data Structure, NVM Command Set Specific - Relative Performance (RP)
typedef enum
{
    cBestPerformance = 0x00,
    cBetterPerformance = 0x01,
    cGoodPerformance = 0x02,
    cDegradedPerformance = 0x03,

} IdentifyLBAFormatDataStructureRelativePerformance_t;

/// @brief Format NVM Command Value Dword 10 - Secure Erase Settings (SES)
typedef enum
{
    cNoSecureEraseOperationRequested = 0,
    cUserDataErase = 1,
    cCryptographicErase = 2,

} FormatNVMCmdDword10SecureEraseSettings_t;

/// @brief Format NVM Command Value Dword 10 - Protection Information (PI)
typedef enum
{
    cNotEnabled = 0,
    cEnabledType1 = 1,
    cEnabledType2 = 2,
    cEnabledType3 = 3,

} FormatNVMCmdDword10ProtectionInformation_t;

/// NVM Command Values (6. NVM Command Set)
/// @brief Protection Information Check (PRCHK)
typedef enum
{
    cLogicalBlockReferenceTagProtection = 0,
    cApplicationTagProtection = 1,
    cGuardfieldProtection = 2,

} ProtectionInformationCheck_t;

/// @brief DATAset Management Access Latency
typedef enum
{
    cNoneLatencyInformation = 0,
    cIdleLongerLatencyAcceptable = 1,
    cNormalTypicalLatecy = 2,
    cLowSmallestPossibleLatecy = 3,

} DataSetManagemantAccessLatency_t;

/// @brief DATAset Management Access Frequency
typedef enum
{
    cNoFrequencyInformationProvided = 0,
    cTypicalNumberReadsWritesExpected = 1,
    cInfrequentWritesInfrequentReads = 2,
    cInfrequentWritesFrequentReads = 3,
    cFrequentWritesInfrequentReads = 4,
    cFrequentWritesFrequentReads = 5,
    cOneTimeWrite = 6,
    cSpeculativeRead = 7,
    cLbaRangeOverwrittenNearFuture = 8,

} DataSetManagemantAccessFrequency_t;

/// @brief Reservation Acquire Action - RACQA Value
typedef enum
{
    cAquire = 0,
    cPreempt = 1,
    cPreemptAndAbort = 2,

} ReservationAcquireActionRACQAValue_t;

/// @brief Reservation Type Encoding - Value
typedef enum
{
    cWriteExclusive = 1,
    cExclusiveAccess = 2,
    cWriteExclusiveRegistrantsOnly = 3,
    cExclusiveAccessRegistrantsOnly = 4,
    cWriteExclusiveAllRegistrants = 5,
    cExclusiveAccessAllRegistrants = 6,

} ReservationTypeEncodingValue_t;

/// @brief Reservation Register - Change Persist Through Power Loss State (CPTPL) Value
typedef enum
{
    cNoChangeToPtplState = 0,
    cPtplState0ReleasedClearedOnPower = 2,
    cPtplState1PersistAcrossPowerLoss = 3,

} ReservationRegisterCPTPLStateValue_t;

/// @brief Reservation Register - Reservation Register Action (RREGA) Value
typedef enum
{
    cRegisterReservationKey = 0,
    cUnregisterReservationKey = 1,
    cReplaceReservationKey = 2,

} ReservationRegisterActionValue_t;

/// @brief Reservation Release - Reservation Release Action (RRELA) Value
typedef enum
{
    cRelease = 0,
    cClear = 1,

} ReservationReleaseActionValue_t;


//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------



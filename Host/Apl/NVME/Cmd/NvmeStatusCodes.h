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
//! @brief NVMe Status code definitions
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
/// @brief Common Status Value definitions (4. System Memory Structures)
/// @brief NVMe Status Code Type(SCT, Page 57, Nvme 1.1a)

static const bool cMayRetry=0;                  // NVMe status field (NDR)
static const bool cDoNotRetry=1;

static const bool cNoMoreStatusInformation=0;   // NVMe status field (M)
static const bool cMoreStatusInformation=1;

typedef enum
{
    cGenericCommandStatus = 0,
    cCommandSpecificStatus = 1,
    cMediaErrors = 2,
    cVendorSpecific = 7
} NvmeStatusCodeType_t;

/// @brief NVMe Status Code - Generic command
typedef enum
{
    /// Admin Command Set
    cSuccessfulCompletion = 0x00,
    cInvalidCommandOpcode = 0x01,
    cInvalidFieldIncommand = 0x02,
    cCommandIdConflict = 0x03,
    cDataTransferError = 0x04,
    cCommandsAbortedDueToPowerLossNotification = 0x05,
    cInternalDeviceError = 0x06,
    cCommandAbortRequested = 0x07,
    cCommandAbortedDueToSqDeletion = 0x08,
    cCommandAbortedDueToFailedFusedCommand = 0x09,
    cCommandAbortedDueToMissingFusedCommand = 0x0A,
    cInvalidNamespaceOrFormat = 0x0B,
    cCommandSequenceError = 0x0C,
    cInvalidSglLastSegmentDescriptor = 0x0D,
    cInvalidNumberOfSglDescriptors = 0x0E,
    cDataSglLengthInvalid = 0x0F,
    cMetadataSglLengthInvalid = 0x10,
    cSglDescriptorTypeInvalid = 0x11,

    /// NVM Command Set
    cLbaOutOfRange = 0x80,
    cCapacityExceeded = 0x81,
    cNamespaceNotReady = 0x82,
    cReservationConflict = 0x83,

    /// Vendor Specific Set

} NvmeGenericCommandStatusCode_t;

/// @brief NVMe Status Code - Command specific
typedef enum
{
    /// Admin Command Set
    cCompletionQueueInvalid = 0x00,
    cInvalidQueueIdentifier = 0x01,
    cMaximumQueueSizeExceeded = 0x02,
    cAbortCommandLimitExceeded = 0x03,
    cAsynchronousEventRequestLimitExceeded = 0x05,
    cInvalidFirmwareSlot = 0x06,
    cInvalidFirmwareImage = 0x07,
    cInvalidInterruptVector = 0x08,
    cInvalidLogPage = 0x09,
    cInvalidFormat = 0x0A,
    cFirmwareApplicationRequiresConventionalReset = 0x0B,
    cInvalidQueueDeletion = 0x0C,
    cFeatureIdentifierNotSaveable = 0x0D,
    cFeatureNotChangeable = 0x0E,
    cFeatureNotNamespaceSpecific = 0x0F,
    cFirmwareApplicationRequiresNvmSubsystemReset = 0x10,
    cNamespaceInsufficientCapacity = 0x15,   // Namespace Management
    cNamespaceIdentifierUnavailable = 0x16,  // Namespace Management
    cNamespaceAlreadyAttached = 0x18,        // Namespace Attachment
    cNamespaceIsPrivate = 0x19,              // Namespace Attachment
    cNamespaceNotAttached = 0x1A,            // Namespace Attachment
    cThinProvisioningNotSupported = 0x1B,    // Namespace Management
    cControllerListInvalid = 0x1C,           // Namespace Attachment
    cDeviceSelftestInProgress = 0x1D,    //Device Self-test
    cDirectiveResourceAllocFailed = 0x7F,

    /// NVM Comand Set
    cConflictingAttributes = 0x80,
    cInvalidProtectionInformation = 0x81,
    cAttemptedWriteToReadOnlyRange = 0x82,

    /// Vendor Specific Set

} NvmeCommandSpecificStatusCode_t;


/// @brief NVMe Status Code - MEdia error
typedef enum
{
    /// Admin Command Set

    /// NVM Comand Sets
    cWriteFault = 0x80,
    cUnrecoveredReadError = 0x81,
    cEndToEndGuardCheckError = 0x82,
    cEndToEnApplicationTagCheckError = 0x83,
    cEndToEnReferenceTagCheckError = 0x84,
    cCompareFailure = 0x85,
    cAccessDenied = 0x86,

    /// Vendor Specific Set

} NvmeMediaErrorStatusCode_t;

/// @brief Admin Status Value definitions (5. Admin Command Set)
/// @brief Asynchronous Event Request CompletionQ Dword 0 - Asynchronous Event Type
typedef enum
{
    cErrorStatus = 0,
    cSmartHealthStatus = 1,
    cNotice = 2,
    cIoCommandSetSpecificStatus = 6,

} AsynchronousEventCompletionQEventType_t;

/// @brief Asynchronous Event Information Error Status
typedef enum
{
    cWriteToInvalidDoorbellRegister = 0,
    cInvalidDoorbellWriteValue = 1,
    cDiagnosticFailure = 2,
    cPersistentInternalDeviceError = 3,
    cTransientInternalDeviceError = 4,
    cFirmwareImageLoadError = 5,

} AsynchronousEventInformationErrorStatus_t;

/// @brief Asynchronous Event Information SMART / Health Status
typedef enum
{
    cDeviceReliability = 0,
    cTemperatureAboveThreshold = 1,
    cSpareBelowThreshold = 2,

} AsynchronousEventInformationSMARTHealthStatus_t;

/// @brief Asynchronous Event Information Notice Status
typedef enum
{
    cNamespaceAttributeChanged = 0,
    cFirmwareActivationStarting = 1,

} AsynchronousEventInformationNoticeStatus_t;

/// @brief Asynchronous Event Information NVM Command Set Specific Status
typedef enum
{
    cReservationLogPageAvailable = 0,

} AsynchronousEventInformationNVMCommandSetSpecificStatus_t;

/// @brief SMART Health Critical Warnings
typedef enum
{
    cCriticalWarnSpare       = 0x01,
    cCriticalWarnTemperature = 0x02,
    cCriticalWarnReliability = 0x04,
    cCriticalWarnMedia       = 0x08
} SMARTHealthCriticalWarnings_t;

/// @brief Asynchronous Event Configuration typed
typedef enum
{
   cAsNamespaceAttributeNotices     = 0x100,
   cAsFirmwareActivationNotices     = 0x200
} AsynchronousEventConfiguration_t;

/// @brief NVMe Error Status Byte Location define
#define ERR_DW0_OFFSET      (0 * 4)
#define ERR_DW1_OFFSET      (1 * 4)
#define ERR_DW2_OFFSET      (2 * 4)
#define ERR_DW3_OFFSET      (3 * 4)
#define ERR_DW4_OFFSET      (4 * 4)
#define ERR_DW5_OFFSET      (5 * 4)
#define ERR_DW6_OFFSET      (6 * 4)
#define ERR_DW7_OFFSET      (7 * 4)
#define ERR_DW8_OFFSET      (8 * 4)
#define ERR_DW9_OFFSET      (9 * 4)
#define ERR_DW10_OFFSET     (10 * 4)
#define ERR_DW11_OFFSET     (11 * 4)
#define ERR_DW12_OFFSET     (12 * 4)

/// NVM Status Value definitions (6. NVM Command Set)

//TODO

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define IS_NVME_ERROR_CODE(EC)              (((EC)&0xFF000000)==cEcNvmeErrorBase)
#define MK_NVME_ERROR_CODE(SC, SCT, M, DNR) (Error_t)(cEcNvmeErrorBase|(SC)|((SCT)<<8)|((M)<<14)|((DNR)<<15))
#define IS_NVME_ASYNC_ERROR_CODE(EC)        (((EC)&0xFF800000)==cEcNvmeAsyncErrorBase)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------

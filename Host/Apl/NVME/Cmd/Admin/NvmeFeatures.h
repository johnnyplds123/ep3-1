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
//! @brief
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define Feature_03_LBARange                 DISABLE

/// @brief Set/Get Features Command Dword 10 - Feature Identifiers
typedef enum
{
    cArbitration = 0x01,
    cPowerManagement = 0x02,
    cLbaRangeType = 0x03,
    cTemperatureThreshold = 0x04,
    cErrorRecovery = 0x05,
    cVolatileWriteCache = 0x06,
    cNumberOfQueues = 0x07,
    cInterruptCoalescing = 0x08,
    cInterruptVectorConfiguration = 0x09,
    cWriteAtomicity = 0x0A,
    cAsynchronousEventConfiguration = 0x0B,
    cAutonomousPowerStateTransition = 0x0C,

    /// Feature Identifiers - NVM Command Set Specific
    cSoftwareProgressMarker = 0x80,
    cHostIdentifier = 0x81,
    cReservationNotificationMask = 0x82,
    cReservationPersistance = 0x83,

    /// Feature Identifiers - Vendor Specific
    cManagementTemperature = 0xD4,

} FeatureIdentifier_t;

/// @brief Set Features - LBA Range Type Command Dword 11  - Type (Type)
typedef enum
{
    cFilesystem = 0x01,
    cRaid = 0x02,
    cCache = 0x03,
    cPageSwapFile = 0x04,

} SetFeaturesCmdLbaRangeTypeDword11Type_t;

static const uint16_t cLastSupportedFid = cAutonomousPowerStateTransition;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

/// @brief NVMe Admin Get/Set Features - Data Structure Entry - LBA Range Type
typedef struct
{
    uint8_t  Type;              ///< Type of the LBA range Byte[0]
    uint8_t  Attributes;        ///< attributes of the LBA range Byte[1]
    uint8_t  reserved2[14];     ///< Reserved Byte[15..2]
    uint64_t SLBA;              ///< Starting LBA Byte[23..16]
    uint64_t NLB;               ///< Starting LBA Byte[31..24]
    uint64_t GUID[2];           ///< Unique Identifier Byte[47..32]
    uint8_t  reserved48[16];    ///< Reserved [63:48]

} SetFeaturesDataStructureLbaRangeType_t;
COMPILE_ASSERT(sizeof(SetFeaturesDataStructureLbaRangeType_t) == 64, "Set Features DataStructure LbaRangeType size shall be 64");

/// @brief NVMe Admin Get/Set Features - Data Structure - Autonomous Power State Transition
typedef struct
{
    uint64_t reserved0:3;       ///< Reserved bits[02:00]
    uint64_t ITPS:5;            ///< Idle Transition Power State bits[07:03]
    uint64_t ITPT:24;           ///< Idle Time Prior to Transition bits[31:08]
    uint64_t reserved32:32;     ///< Reserved bits[63:32]

} NvmeAdminSetFeaturesDataStructureAutonomousPowerStateTransition_t;
COMPILE_ASSERT(sizeof(NvmeAdminSetFeaturesDataStructureAutonomousPowerStateTransition_t) == 8, "SetFeaturesDataStructureAutonomousPowerStateTransition size shall be 8");


/// @brief NVMe Admin Get/Set Features - Data Structure - Host Identifier
typedef struct
{
    uint64_t HOSTID:8;           ///< Host Identifier bits[07:00]
    uint64_t reserved:56;           ///< Host Identifier bits[63:08]

} NvmeAdminSetFeaturesDataStructureHostIdentifier_t;
COMPILE_ASSERT(sizeof(NvmeAdminSetFeaturesDataStructureHostIdentifier_t) == 8, "SetFeaturesDataStructureHostIdentifier size shall be 8");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - Reservation Notification Mask
typedef struct
{
    uint32_t reserved0:1;        ///< Reserved [00]
    uint32_t REGPRE:1;           ///< Mask Registration Preempted Notification bits[01]
    uint32_t RESREL:1;           ///< Mask Reservation Released Notification bits[02]
    uint32_t RESPRE:1;           ///< Mask Reservation Preempted Notification bits[03]
    uint32_t reserved4:28;       ///< Reserved bits[31:04]

} Dw11ReservationNotificationMask_t;
COMPILE_ASSERT(sizeof(Dw11ReservationNotificationMask_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Get Features - Select (SEL) - Supported capabilities CQ Dword0
typedef struct
{
    uint32_t savable:1;            ///< indicates Feature Identifier is saveable bits[00]
    uint32_t namespaceSpecific:1;  ///< indicates Feature Identifier is namespace specific bits[01]
    uint32_t changeable:1;         ///< indicates Feature Identifier is changeable bits[02]
    uint32_t reserved3:29;         ///< Reserved bits[31:03]

} GetFeaturesCapabilitiesCqDword0_t;
COMPILE_ASSERT(sizeof(GetFeaturesCapabilitiesCqDword0_t) == 4, "GetFeatures DW0 size shall be 4");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - Arbitraion
typedef struct
{
    uint32_t AB:3;          ///< Arbitration Burst [2:0]
    uint32_t reserved3:5;   ///< Reserved [7:3]
    uint32_t LPW:8;         ///< Low Priority Weight [15:8]
    uint32_t MPW:8;         ///< Medium Priority Weight [23:16]
    uint32_t HPW:8;         ///< High Priority Weight [31:24]

} Dw11Arbitration_t;
COMPILE_ASSERT(sizeof(Dw11Arbitration_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - Power Management
typedef struct
{
    uint32_t PS:5;           ///< Power State bits[04:00]
    uint32_t WH:3;           ///< Power State bits[07:05]
    uint32_t reserved24:24;  ///< Reserved bits[31:08]

} Dw11PowerManagement_t;
COMPILE_ASSERT(sizeof(Dw11PowerManagement_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - LBA Range Type
typedef struct
{
    uint32_t NUM:6;           ///< Number of LBA Ranges bits[05:00]
    uint32_t reserved6:26;    ///< Reserved bits[31:06]

} Dw11LbaRangeType_t;
COMPILE_ASSERT(sizeof(Dw11LbaRangeType_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - Temperature Threshold
typedef struct
{
    uint32_t TMPTH:16;           ///< Temperature Threshold bits[15:00]
    uint32_t TMPSEL:4;           ///< Threshold Temperature Select[19:16]
    uint32_t THSEL:2;            ///< Threshold Type Select[21:20]
    uint32_t reserved10:10;      ///< Reserved bits[31:22]

} Dw11TemperatureThreshold_t;
COMPILE_ASSERT(sizeof(Dw11TemperatureThreshold_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - Error Recovery
typedef struct
{
    uint32_t TLER:16;            ///< Time Limited Error Recovery bits[15:00]
    uint32_t DULBE:1;            ///< Deallocated or Unwritten Logical Block Error Enable[16]
    uint32_t reserved15:15;      ///< Reserved bits[31:17]

} Dw11ErrorRecovery_t;
COMPILE_ASSERT(sizeof(Dw11ErrorRecovery_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - Volatile Write Cache
typedef struct
{
    uint32_t WCE:1;             ///< Volatile Write Cache Enable bits[00]
    uint32_t reserved1:31;      ///< Reserved bits[31:01]

} Dw11VolatileWriteCache_t;
COMPILE_ASSERT(sizeof(Dw11VolatileWriteCache_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - Number of Queues
typedef struct
{
    uint32_t NSQR:16;           ///< Number of I/O Submission Queues Requested bits[15:00]
    uint32_t NCQR:16;           ///< Number of I/O Completion Queues Requested bits[31:16]

} Dw11NumberOfQueues_t;
COMPILE_ASSERT(sizeof(Dw11NumberOfQueues_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Number of queues allocated
typedef struct
{
    uint32_t NSQA:16;           ///< Number of I/O Submission Queues Allocated bits[15:00]
    uint32_t NCQA:16;           ///< Number of I/O Completion Queues Allocated bits[31:16]

} Dw0NumberOfQueues_t;
COMPILE_ASSERT(sizeof(Dw0NumberOfQueues_t) == 4, "NVMe feature size shall be 4");


/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - Interrupt Coalescing
typedef struct
{
    uint32_t THR:8;             ///< Aggregation Threshold bits[07:00]
    uint32_t TIME:8;            ///< Aggregation Time bits[15:08]
    uint32_t reserved16:16;     ///< Reserved bits[31:16]

} Dw11InterruptCoalescing_t;
COMPILE_ASSERT(sizeof(Dw11InterruptCoalescing_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - Interrupt Vector Configuration
typedef struct
{
    uint32_t IV:16;             ///< Interrupt Vector bits[15:00]
    uint32_t CD:1;              ///< Coalescing Disable bits[16]
    uint32_t reserved17:15;     ///< Reserved bits[31:17]

} Dw11InterruptConfiguration_t;
COMPILE_ASSERT(sizeof(Dw11InterruptConfiguration_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - Write Atomicity
typedef struct
{
    uint32_t DN:1;              ///< Disable Normal bits[00]
    uint32_t reserved1:31;      ///< Reserved bits[31:01]

} Dw11WriteAtomicity_t;
COMPILE_ASSERT(sizeof(Dw11WriteAtomicity_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - Asynchronous Event Configuration
typedef struct
{
    uint32_t SMARTHealthCriticalWarnings:8;       ///< SMART / Health Critical Warnings bits[07:00]
    uint32_t NamespaceAttributeNotices:1;         ///< Namespace Attribute Notices[8]
    uint32_t FirmwareActivationNotices:1;         ///< Firmware Activation Notices[9]
    uint32_t reserved22:22;                        ///< Reserved bits[31:10]

} Dw11AsynchronousEventConfiguration_t;
COMPILE_ASSERT(sizeof(Dw11AsynchronousEventConfiguration_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - Autonomous Power State Transition
typedef struct
{
    uint32_t APSTE:1;       ///< Autonomous Power State Transition Enable bits[00]
    uint32_t reserved1:31;                        ///< Reserved bits[31:01]

} Dw11AutonomousPowerStateTransition_t;
COMPILE_ASSERT(sizeof(Dw11AutonomousPowerStateTransition_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 - Software Progress Marker
typedef struct
{
    uint32_t PBSLC:8;           ///< Pre-boot Software Load Count bits[07:00]
    uint32_t reserved8:24;      ///< Reserved bits[31:08]

} Dw11SoftwareProgressMarker_t;
COMPILE_ASSERT(sizeof(Dw11SoftwareProgressMarker_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Set Features SQ Dword11 CQ Dword0 - Reservation Persistence
typedef struct
{
    uint32_t PTPL:1;           ///< Persist Through Power Loss bits[00]
    uint32_t reserved1:31;     ///< Reserved bits[31:01]

} Dw11ReservationPersistenceMask_t;
COMPILE_ASSERT(sizeof(Dw11ReservationPersistenceMask_t) == 4, "NVMe feature size shall be 4");

/// @brief NVMe Admin Set Features SQ Dword11 CQ Dword0 - Autonomous Power State Transition
typedef struct
{
    uint32_t APSTE:1;             ///< Autonomous Power State Transition Enable [00]
    uint32_t reserved1:31;        ///< Reserved [31:01]

} Dw11AutoPowerStateTransition_t;
COMPILE_ASSERT(sizeof(Dw11AutoPowerStateTransition_t) == 4, "NVMe Set feature DW0 size shall be 4");

/// @brief NVMe Admin Get/Set Features SQ Dword11 CQ Dword0 -  Dell HD-DITM \ Lenovo DITT
typedef struct
{
    uint16_t Heavy;               ///< Heavy Throttling temperature threshold in degrees K[15:00]
    uint16_t Light;               ///< Light Throttling temperature threshold in degrees K[31:16]
} Dw11ManaTemperature_t;
COMPILE_ASSERT(sizeof(Dw11ManaTemperature_t) == 4, "NVMe feature size shall be 4");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern const uint32_t defaultFeatureValue[cLastSupportedFid+1];
extern       uint32_t currentFeatureValue[cLastSupportedFid+1];
extern const uint16_t defaTMPTH[2][2];

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------

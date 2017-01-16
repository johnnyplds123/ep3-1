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
/// @brief Get Log Page Command Dword 10 - Log Page Identifiers (LID)
typedef enum
{
    cLidReserved = 0x00,
    cLidErrorInformation = 0x01,
    cLidSmartHealthInformation = 0x02,
    cLidFirmwareSlotInformation = 0x03,
    cLidCommandEffectsLog = 0x05,
    cLidDeviceSelfTestLog = 0x06,

    ///< NVM Command Set Specific
    cLidReservationNotification = 0x80,

    /// Vendor specific
    cLidWCSSmartHealthInfo = 0xC0,  //WCS-customized SMART / Health Information
    cLidWCSDebugC1Log = 0xC1,  //WCS-customized Debugging Logs/C1 error Information
    cLidWCSDebugC2Log = 0xC2,  //WCS-customized Debugging Logs/C2 error Information

    /// Vendor specific
    cLidDellSmartHealthInfo = 0xCA,  //Dell-customized SMART / Health Information

} GetLogPageCmdDword10LogPageIdentifiers_t;

typedef enum
{
    cLogCritWarnCurr = 0,
    cLogCritWarnPost,
    cLogCritWarnCount
} LogCritWarnType_t;

/// @brief NVMe Admin Get Log Page - WCS SMART / Health Information (Log Identifier C0h)
#pragma pack(1)
typedef struct
{
    uint64_t    MediaUnitsWritten[2];
    uint8_t     CapacitorHealth;
    uint32_t    EccIterations[4];
    uint8_t     SupportedFeatures;
    uint8_t     TemperatureThrottlingWarn[5];
    uint8_t     TemperatureThrottlingCrit[3];
    uint8_t     PowerConsumption;
    uint8_t     WearRangeDelta;
    uint8_t     UnalignedIO[6];
    uint32_t    MappedLBAs;
    uint8_t     ProgramFailCount;
    uint8_t     EraseFailCount;
    uint8_t     reserved55[456];                       ///< Reserved (bytes[511:55])

} LogPageWcsSmartHealthInfo_t;
#pragma pack()
COMPILE_ASSERT(sizeof(LogPageWcsSmartHealthInfo_t) == 512, " Smart Attributes Log Entry size shall be 512");

typedef struct
{
    uint8_t     LogPageId;
    uint8_t     reserved0;              //Must be 0
    uint16_t    Flags0:1;               //Bit 0,log page contains [0]Dynamic [1]Static
    uint16_t    Flags1:1;               //Bit 1,log page contains [0]Namespace specific data [1]Device data
    uint16_t    reserved1:14;           //Must be 0
    uint32_t    LogPageSize;            //Size of log page in bytes
    uint8_t     DataChangeIntervals[8]; //Change interval of data in milliseconds;If static data value is 0
    uint8_t     reserved2[16];          //Must be 0
} LogPageWcsDebugLogHeader_t;
COMPILE_ASSERT(sizeof(LogPageWcsDebugLogHeader_t) == 32, " Debugging Logs Entry size shall be 32");

/// @brief NVMe Admin Get Log Page - Error Information Log Entry (Log Identifier 01h)
typedef struct
{
    uint64_t errorCount;                         /// a 64-bit incrementing error count (bytes[07:00])
    uint16_t submissionQueueID;                  ///< the Submission Queue Identifier of command the error information is associated with (bytes[09:08])
    uint16_t commandId;                          ///< indicates the Command Identifier of the command that the error is associated with (bytes[11:10])
    uint16_t statusField;                        ///< the Status Field for the command that completed (bytes[13:12])

                                                 /// Parameter Error Location: indicates byte and bit of command parameter error is associated with byte[15:14])
    uint16_t parameterErrorLocationByte:8;       ///< Byte in command that contained the error bits[7:0]
    uint16_t parameterErrorLocationBit:3;        ///< Bit in command that contained the error bits[10:8]
    uint16_t reserved:5;                         ///< reserved bits[15:11]

    uint64_t lba;                                ///< indicates the first LBA that experienced the error condition (bytes[23:16])
    uint32_t namespace;                          ///< indicates the namespace that the error is associated with (bytes[27:24])
    uint8_t  vendorSpecificInformationAvailable; ///< provides the log page identifier associated with that page (bytes[28])
    uint8_t  reserved29[3];                      ///< Reserved (bytes[31:29])
    uint64_t commandSpecificInformation;         ///< 00h: No addtional information, Valid value: 80h to FFh
    uint8_t  reserved40[24];                     ///< Reserved (bytes[63:40])

} LogPageErrorInfoEntry_t;
COMPILE_ASSERT(sizeof(LogPageErrorInfoEntry_t) == 64, " Error Information Log Entry size shall be 64");

/// @brief NVMe Admin Get Log Page - SMART / Health Information (Log Identifier 02h)
typedef struct
{
                                                    /// indicates critical warnings for the state of the controller (bytes[00])
    uint32_t criticalWarningSpareSpace:1;           ///< available spare space has fallen below the threshold (bits[00])
    uint32_t criticalWarningTemperature:1;          ///< temperature has exceeded a critical threshold (bits[01])
    uint32_t criticalWarningMediaInternalError:1;   ///< device reliability degraded due to media related errors or internal error (bits[02])
    uint32_t criticalWarningReadOnlyMode:1;         ///< media has been placed in read only mode (bits[03])
    uint32_t criticalWarningVolatileFail:1;         ///< volatile memory backup device has failed (bits[04])
    uint32_t reserved:3;                            ///< Reserved (bits[07:05])
    uint32_t temperature:16;                        ///< Contains the temperature of the overall device (bytes[2:1])
    uint32_t availableSpare:8;                      ///< Contains normalized percentage of the remaining spare capacity available with (bytes[3])

    uint8_t  availableSpareThreshold;               ///< When the Available Spare falls below the threshold indicated in this field  (bytes[4])
    uint8_t  percentageUsed;                        ///< Contains a vendor specific estimate of the percentage of device life used (bytes[5])
    uint8_t  reserved6[26];                         ///<  Reserved (bytes[31:6])

    uint64_t dataUnitsRead[2];                      ///<  Contains the number of 512 byte data units the host has read from the controller (bytes[47:32])

    uint64_t dataUnitsWritten[2];                   ///< the number of 512 byte data units the host has written to the controller (bytes[63:48])
    uint64_t hostReadCommands[2];                   ///< the number of read commands completed by the controller (bytes[79:64])
    uint64_t hostWriteCommands[2];                  ///< the number of write commands completed by the controller (bytes[95:80])
    uint64_t controllerBusyTime[2];                 ///< the amount of time the controller is busy with I/O commands (bytes[111:96])
    uint64_t powerCycles[2];                        ///< the number of power cycles (bytes[127:112])
    uint64_t powerOnHours[2];                       ///< the number of power-on hours (bytes[143:128])
    uint64_t unsafeShutdowns[2];                    ///< the number of unsafe shutdowns (bytes[159:144])
    uint64_t mediaErrors[2];                        ///< the number of occurrences where controller detected unrecovered data integrity error (bytes[175:160])
    uint64_t numberofErrorInformationLogEntries[2]; ///< the number of Error Information log entries over life of controller (bytes[191:176])
    uint32_t WarningTempTime;                       ///< Warning Composite Temperature Time (bytes[195:192])
    uint32_t CriticalTempTime;                      ///< Critical Composite Temperature Time (bytes[199:196])
    uint16_t TempSensor1;                           ///< Temperature Sensor 1 (bytes[201:200])
    uint8_t  reserved202[310];                      ///< Reserved (bytes[511:202])
} LogPageSmartHealthInfoEntry_t;
COMPILE_ASSERT(sizeof(LogPageSmartHealthInfoEntry_t) == 512, " Smart/Health Information Log Entry size shall be 512");

/// @brief NVMe Admin Get Log Page - Firmware Slot Information (Log Identifier 03h)
typedef struct
{
                                     /// Active Firmware Info (AFI): Specifies information about the active firmware revision (bytes[00])
    uint8_t  AFI_SLOT_ACTIVE:3;      ///< the firmware slot that contains the actively running firmware revision (bits[2:0])
    uint8_t  reserved3:1;            ///< Reserved (bits[3])
    uint8_t  AFI_SLOT_ACTIVE_NEXT:3; ///< the firmware slot that is going to be activated at the next controller reset (bits[6:4])
    uint8_t  reserved7:1;            ///< Reserved (bits[7])

    uint8_t  reserved1[7];           ///< Reserved (bytes[07:01])
    uint8_t  FRS[7][8];              ///< Firmware Revision for Slot 1 to 7
    uint8_t  reserved64[448];        ///< Reserved (bytes[511:64])

} LogPageFirmwareSlotInfoEntry_t;
COMPILE_ASSERT(sizeof(LogPageFirmwareSlotInfoEntry_t) == 512, " Firmware Slot Information Log Entry size shall be 512");

/// @brief NVMe Admin Get Log Page - NVM Command Set Specific Log Page Identifiers - Reservation Notification (Log Identifier 80h)
typedef struct
{
    uint8_t   LogPageCount[8];                     ///< Reserved (bytes[07:00])
    uint8_t   ReservationNotificationLogPageType;  /// Reservation Notification Log Page Type: indicates Reservation Notification type (bytes[08])
    uint8_t   NumberofAvailableLogPages;            ///< Firmware Revision for Slot 1 (bytes[09])
    uint8_t   reserved10[2];                    ///< Reserved (bytes[11:10])
    uint32_t  NamespaceID;                      ///< Firmware Revision for Slot 3 (bytes[15:12])
    uint8_t   reserved12[48];                   ///< Reserved (bytes[63:16])

} LogPageRsvNotificationInfoEntry_t;
COMPILE_ASSERT(sizeof(LogPageRsvNotificationInfoEntry_t) == 64, " Reservation Notification Information Log Entry size shall be 64");


/// @brief NVMe Admin Command structure - Get Log Page
typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t LID:8;               ///< Log Page Identifier (DW10 bits[7:0])
    uint32_t reserved10_8:8;      ///< Reserved (DW10 bits[15:8])
    uint32_t NUMD:12;             ///< Number of Dwords (DW10[ bits[27:16])
    uint32_t reserved10_28:4;      ///< Reserved (DW10 bits[31:28])

    uint32_t reserved11[5];       ///< DW15..11

} NvmeAdminGetLogPageCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminGetLogPageCommand_t)==64, "NVMe command size shall be 64");

/// @brief NVMe Admin Get Log Page - Command Effects Log (Log Identifier 05h)
typedef struct
{
    uint32_t CSUPP:1;         ///< Command Supported (bits[0])
    uint32_t LBCC:1;          ///< Logical Block Content Change (bits[1])
    uint32_t NCC:1;           ///< Namespace Capability Change (bits[2])
    uint32_t NIC:1;           ///< Namespace Inventory Change (bits[3])
    uint32_t CCC:1;           ///< Controller Capability Change (bits[4])
    uint32_t reserved11:11;   ///< Reserved (bits[15:5])
    uint32_t CSE:3;           ///< Command Submission and Execution (bits[18:16])
    uint32_t reserved13:13;   ///< Reserved (bits[31:19])

} EffectData_t;
COMPILE_ASSERT(sizeof(EffectData_t) == 4, " Effect Data size shall be 4");

typedef struct
{
    EffectData_t ACS[256];    ///< Admin Command Supported
    EffectData_t IOCS[256];   ///< I/O Command Supported
    uint32_t reserved512[512];

} LogPageCommandEffectsEntry_t;
COMPILE_ASSERT(sizeof(LogPageCommandEffectsEntry_t) == 4096, " Firmware Slot Information Log Entry size shall be 4096");

/// @brief NVMe Admin Get Log Page - Device Self Test (Log Identifier 06h)
typedef struct
{
    uint8_t         Operation;          ///< Current Device Self-Test Operation
    uint8_t         Completion;         ///< Current Device Self-Test Completion
    uint16_t        reserved2;          ///< reserved
    tDST_LOG_ENTRY  DSTResultData[20];  ///< Newest Self-test Result Data Structure

} LogPageDeviceSelfTestEntry_t;
COMPILE_ASSERT(sizeof(LogPageDeviceSelfTestEntry_t) == 564, " Firmware Slot Information Log Entry size shall be 564");

/// @brief NVMe Admin Get Log Page - Dell SMART / Health Information (Log Identifier CAh)
typedef struct
{
    uint8_t   ReAssignedSectorCount;
    uint8_t   ProgramFailCountWorst;
    uint8_t   ProgramFailCountTotal;
    uint8_t   EraseFailCountWorst;
    uint8_t   EraseFailCountTotal;
    uint8_t   WearLevelingCount;
    uint8_t   UsedReservedBlockCountWorst;
    uint8_t   UsedReservedBlockCountTotal;
    uint32_t  ReservedBlockCount;
    uint8_t   reserved500[500];                       ///< Reserved (bytes[511:12])

} LogPageDellSmartHealthInfo_t;
COMPILE_ASSERT(sizeof(LogPageDellSmartHealthInfo_t) == 512, " Smart Attributes Log Entry size shall be 512");

typedef struct
{
    U64  warnTime;
    U32  critTime;
    U32  rsvd;
} LogCritWarnTime_t;
COMPILE_ASSERT(sizeof(LogCritWarnTime_t) == 16, "LogCriticalWarningTime_t size shall be 16 bytes");

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#if (_CUSTOMER_ == CUSTOMER_DELL)
    #define AvaiSpareThreshold  50
#else
    #define AvaiSpareThreshold  0  //(0 to 100%)
#endif
//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern U64  gGetLogPowerOnHours;
extern U64  gGetLogIOBusyTime;
extern U32  gSmartIOTimerFlag;
extern U32  gSmartTimerMil;
extern U8   gErrLogSysInfoUpdate;
//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
extern void    HandleSaveErrLog(HcmdQ_t* pHcmdQ);
extern void    HandleSaveNoSpecErrLog(void);
extern void    HandleErrLogUpdateSysInfo(HcmdQ_t* pHcmdQ);
extern void    HandleLogInfoInit(InitBootMode_t initMode);
extern Error_t HandleTemperatureC2K(uint16_t* temp);
extern void    CriticalWarningUpdate(void);

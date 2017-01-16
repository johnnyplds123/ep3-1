#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2014  -  All rights reserved
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
//! @brief NVMe Controller Registers
//!
//! Doc Version: NONE
//! Doc Date: NONE
//=============================================================================
/// @cond SPEC and HW_REG

static const uint32_t cNvmeNumOfFuncSupport = 3;
static const uint32_t cNvmeNumOfQueueSupport = 16;  // Including Admin Queue and I/O Queues.
static const uint32_t cNvmeNumOfQueueFunc = cNvmeNumOfQueueSupport + cNvmeNumOfFuncSupport;

//-----------------------------------------------------------------------------
//  NVMe Controller registers (Page36 ~ 43, NVMe spec 1.1a)
//-----------------------------------------------------------------------------
/// @brief NVMe controller capabilities
typedef union
{
    uint64_t all;
    struct
    {
        uint64_t MQES:16;       ///< BIT [15:0] Maximum Queue Entries Supported
        uint64_t CQR:1;         ///< BIT [16] Contiguous Queues Required
        uint64_t AMS:2;         ///< BIT [18:17] Arbitration Mechanism Supported
        uint64_t reserved19:5;  ///< BIT [23:19] Reserved
        uint64_t TO:8;          ///< BIT [31:24] Timeout
        uint64_t DSTRD:4;       ///< BIT [35:32] Doorbell Stride
        uint64_t NSSRS:1;       ///< BIT [36] NVM Subsystem Reset Supported
        uint64_t CSS:8;         ///< BIT [44:37] Command Sets Supported
        uint64_t reserved45:3;  ///< BIT [47:45] Reserved
        uint64_t MPSMIN:4;      ///< BIT [51:48] Memory Page Size Minimum
        uint64_t MPSMAX:4;      ///< BIT [55:52] Memory Page Size Maximum
        uint64_t reserved56:8;  ///< BIT [56:63] Reserved
    } b;
} NvmeCap_t;
COMPILE_ASSERT(sizeof(NvmeCap_t)==8, "NvmeCap_t size shall be 8");

/// @brief NVMe Version
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t MNR:16;        ///< BIT [15:0] Minor Version Number
        uint32_t MJR:16;        ///< BIT [31:16] Major Version Number
    } b;
} NvmeVersion_t;
COMPILE_ASSERT(sizeof(NvmeVersion_t)==4, "NvmeVersion_t size shall be 4");

/// @brief NVMe Interrupt Mask Set
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t IVMS:32;       ///< BIT [31:0] Interrupt Vector Mask Set
    } b;
} NvmeIntrMaskSet_t;

/// @brief NVMe Interrupt Mask Clear
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t IVMC:32;       ///< BIT [31:0] Interrupt Vector Mask Clear
    } b;
} NvmeIntrMaskClr_t;

/// @brief NVMe Controller Configuration
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t EN:1;          ///< BIT[0] Enable
        uint32_t reserved1:3;   ///< BIT[3:1] Reserved
        uint32_t CSS:3;         ///< BIT[6:4] I/O Command Set Selected
        uint32_t MPS:4;         ///< BIT[10:7] Memory Page Size
        uint32_t AMS:3;         ///< BIT[13:11] Arbitration Mechanism Selected
        uint32_t SHN:2;         ///< BIT[15:14] Shutdown Notification
        uint32_t IOSQES:4;      ///< BIT[19:16] I/O Submission Queue Entry Size
        uint32_t IOCQES:4;      ///< BIT[23:20] I/O Completion Queue Entry Size
        uint32_t reserved24:8;  ///< BIT[31:24] Reserved

    } b;
} NvmeConfiguration_t;
COMPILE_ASSERT(sizeof(NvmeConfiguration_t)==4, "NvmeConfiguration_t size shall be 4");

/// @brief NVMe Controller Status
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t RDY:1;         ///< BIT[0] Ready
        uint32_t CFS:1;         ///< BIT[1] Controller Fatal Status
        uint32_t SHST:2;        ///< BIT[3:2] Shutdown Status
        uint32_t NSSRO:1;       ///< BIT[4] NVM Subsystem Reset Occurred
        uint32_t PP:1;          ///< BIT[5] Processing Paused
    } b;
} NvmeStatus_t;

static const uint32_t cNvmeResetSignature = 0x4E564D64; // "NVMe"

/// @brief NVMe Admin Queue Attributes
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t ASQS:12;       ///< BIT[11:0] Admin Submission Queue Size
        uint32_t reserved12:4;  ///< BIT[12:15] Reserved
        uint32_t ACQS:12;       ///< BIT[27:16] Admin Completion Queue Size
    } b;
} NvmeAdminQueueAttr_t;

/// @brief NVMe Doorbell registers
typedef struct
{
    uint32_t SQTDBL;       ///< Submission Queue Tail Doorbell
    uint32_t CQHDBL;       ///< Competion Queue Head Doorbell
} NvmeDoorbell_t;

/// @brief NVMe Controller register
typedef struct
{
    NvmeCap_t CAP;                ///< 00h  Controller Capabilities
    NvmeVersion_t VS;             ///< 08h  Version
    NvmeIntrMaskSet_t INTMS;      ///< 0Ch  Interrupt Mask Set
    NvmeIntrMaskClr_t INTMC;      ///< 10h  Interrupt Mask Clear
    NvmeConfiguration_t CC;       ///< 14h  Controller Configuration
    uint32_t reserved18h;         ///< 18h  Reserved
    NvmeStatus_t CSTS;            ///< 1Ch  Controller Status
    uint32_t NSSR;                ///< 20h  NVM Subsystem Reset (Optional)
    NvmeAdminQueueAttr_t AQA;     ///< 24h  Admin Queue Attributes
    uint64_t ASQB;                ///< 28h  Admin Submission Queue Base
    uint64_t ACQB;                ///< 30h  Admin Completion Queue Base
} NvmeController_t;
COMPILE_ASSERT(sizeof(NvmeController_t)==56, "NvmeController_t size shall be 56");

//-----------------------------------------------------------------------------
//  NVME_CREG Registers structure definitions
//-----------------------------------------------------------------------------
/// @brief NVMe Queue Status Table Entry SQ
#pragma anon_unions // unnamed union
typedef union
{
    uint64_t all;   // All bits
    struct
    {
        uint32_t headTailPtr;
        uint32_t cqIdAndSize;
    };
    struct
    {
        uint32_t tailDbPtr    :16;      ///<BIT [15:0] SQ Tail Pointer (DoorBell)
        uint32_t headPtr      :16;      ///<BIT [31:16] SQ Head Pointer
        uint32_t cqId         :16;      ///<BIT [47:32] Associative CQ ID
        uint32_t size         :16;      ///<BIT [63:48] SQ Size
    } b;
} NvmeQueueStatusEntrySq_t;

/// @brief NVMe Queue Status Table Entry CQ
#pragma anon_unions // unnamed union
typedef union
{
    uint64_t all;   // All bits
    struct
    {
        uint32_t headTailPtr;
        uint32_t vectorAndSize;
    };
    struct
    {
        uint32_t headDbPtr    :16;      ///<BIT [15:0] CQ Head Pointer (DoorBell)
        uint32_t tailPtr      :16;      ///<BIT [31:16] CQ Tail Pointer
        uint32_t vector       :16;      ///<BIT [47:32] Interrupt Vector
        uint32_t size         :16;      ///<BIT [63:48] CQ Size
    } b;
} NvmeQueueStatusEntryCq_t;

/// @brief NVMe Queue Status Table SRAM Portion
typedef struct
{
    NvmeQueueStatusEntrySq_t sq;
    NvmeQueueStatusEntryCq_t cq;
} NvmeQueueStatusSram_t;


/// @brief NVMe MSI-X Table Entry Structure
typedef struct
{
    uint64_t messageAddr;       ///< Host-specified message address (The LSB 2bit must be zero).
    uint64_t messageData :32;   ///<BIT [31:0] Host-specified message data
    uint64_t maskBit     : 1;   ///<BIT [32] Mask bit for this MSI-X table entry
    uint64_t revserved33 :31;   ///<BIT [63:33] Reserved
} NvmeMsixEntry_t;

/// @brief NVMe Command PRP Table Entry Structure
/// @notes If it is an admin queue, this field stores the queue base address.
///        If it is an I/O queue, this field stores the PRP1 from the host command
typedef struct
{
    uint64_t sqPrp;       ///< The PRP1 from the host command when the I/O SQ is created.
    uint64_t cqPrp;       ///< The PRP1 from the host command when the I/O CQ is created.
} NvmeCmpPrp_t;

/// @brief NVMe Current PRP Table Entry Structure
typedef struct
{
    uint64_t sqPrp;       ///< The most recently used PRP or a pointer to a specific SQ etnry in the host memory.
    uint64_t cqPrp;       ///< The most recently used PRP or a pointer to a specific CQ etnry in the host memory.
} NvmeCurrentPrp_t;

/// @brief NVMe Queue Status Table Register SQ Entry
typedef union
{
    uint64_t all;   // All bits
    struct
    {
        uint32_t QC      : 1; ///<BIT[0] Queue Created (QC)
        uint32_t CT      : 1; ///<BIT[1] Contiguous (CT),
                              ///        Indicates whether the SQ is physically contiguous.
        uint32_t Priority: 2; ///<BIT[3:2] Priority, Indicates the priority of the SQ
        uint32_t Init    : 1; ///<BIT[4] Init, Indicates whether the SQ is just created
                              ///              and is cleared by the Command Fetcher.
        uint32_t PV      : 1; ///<BIT[5] PRP Valid, The current PRP is  valid in the Queue PRP Table.
        uint32_t reserv6 : 1; ///<BIT[6] Reserved
        uint32_t Error   : 1; ///<BIT[7] Error, the SQ releated table entry has error.
        uint32_t reserv8 :24; ///<BIT[31:8] Reserved
        uint32_t reservDW1;   ///<BIT[63:32] Reserved
    } b;
} NvmeQueueStatusRegSqEntry_t;

/// @brief NVMe Queue Status Table Register CQ Entry
typedef union
{
    uint64_t all;   // All bits
    struct
    {
        uint32_t QC      : 1; ///<BIT[0] Queue Created, cleared when the CQ is deleted
        uint32_t CT      : 1; ///<BIT[1] Contiguous (CT),
                              ///        Indicates whether the CQ is physically contiguous.
        uint32_t IE      : 1; ///<BIT[2] Interrupt Enable, the CQ's interrupt is enabled.
        uint32_t NPV     : 1; ///<BIT[3] Next PRP Valid
        uint32_t Init    : 1; ///<BIT[4] Init, Indicates whether the CQ is just created or deleted
                              ///              and is cleared by the Status Manager.
        uint32_t PV      : 1; ///<BIT[5] PRP Valid, The current PRP is  valid in the Queue PRP Table.
        uint32_t PH      : 1; ///<BIT[6] Phase, Keeps the posting status of the CQ.
        uint32_t Error   : 1; ///<BIT[7] Error, the CQ releated table entry has error.
        uint32_t reserv8 :24; ///<BIT[31:8] Reserved
        uint32_t reservDW1;   ///<BIT[63:32] Reserved
    } b;
} NvmeQueueStatusRegCqEntry_t;

/// @brief NVMe Queue Status Table Register Entry
typedef struct
{
    NvmeQueueStatusRegSqEntry_t sq;
    NvmeQueueStatusRegCqEntry_t cq;
} NvmeQueueStatusReg_t;

/// @brief 0xF5000180-0xF5000947 : NvmeControllerSram_t MODULE
typedef struct
{
    /// CREG_DIR_ACC_DATA
    /// Control Register Storage Data Structure (0xF5000180 - 0xF5000947)
    NvmeController_t controller[cNvmeNumOfFuncSupport];     // 0xF5000180: NVMe Controller Register
    NvmeQueueStatusSram_t queueStatus[cNvmeNumOfQueueFunc]; // 0xF5000228: Queue Status Table (SRAM Portion)
    NvmeMsixEntry_t msix[cNvmeNumOfQueueFunc];              // 0xF5000358: MSI-X Table
    NvmeCmpPrp_t cmdPrp[cNvmeNumOfQueueFunc];               // 0xF5000488: Queue PRP Table (CMD PRP)
    NvmeCurrentPrp_t currentPrp[cNvmeNumOfQueueFunc];       // 0xF50005B8: Queue PRP Table (Current PRP)
    uint64_t nextPrp[cNvmeNumOfQueueFunc];                  // 0xF50006E8: Queue PRP Table (Next PRP)
    uint64_t msixPba[cNvmeNumOfQueueFunc];                  // 0xF5000780: MSI-X PBA Table (Bit0: Pending)
    NvmeQueueStatusReg_t queueReg[cNvmeNumOfQueueFunc];     // 0xF5000818-0xF5000947: Queue Status Table (Register Portion)
} NvmeControllerSram_t;
COMPILE_ASSERT(sizeof(NvmeControllerSram_t)==0x7C8, "OTF update fail");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile NvmeControllerSram_t rNvmeController;///< 0xF5000180-0xF5000947 : NvmeControllerSram_t MODULE

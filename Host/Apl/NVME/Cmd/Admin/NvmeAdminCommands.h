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
//! @brief NVMe Admin command header
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
/// @brief Opcodes for admin commands
static const uint8_t cDeleteIoSubmissionQueue = 0x00;
static const uint8_t cCreateIoSubmissionQueue = 0x01;
static const uint8_t cGetLogpage = 0x02;
static const uint8_t cDeleteIoCompletionQueue = 0x04;
static const uint8_t cCreateIoCompletionQueue = 0x05;
static const uint8_t cIdentify = 0x06;
static const uint8_t cAbort = 0x08;
static const uint8_t cSetFeatures = 0x09;
static const uint8_t cGetFeatures = 0x0A;
static const uint8_t cAsynchronousEventRequest = 0x0c;
static const uint8_t cNamespaceManagement = 0x0d;
static const uint8_t cFirmwareActivate = 0x10;
static const uint8_t cFirmwareImageDownload = 0x11;
static const uint8_t cDeviceSelfTest = 0x14;
static const uint8_t cNamespaceAttachment = 0x15;
static const uint8_t cDirectiveSend = 0x19;
static const uint8_t cDirectiveReceive = 0x1A;

/// @brief Opcodes for admin commands - NVM Command Set Specific
static const uint8_t cFormatNvm = 0x80;
static const uint8_t cSecuritySend= 0x81;
static const uint8_t cSecurityReceive = 0x82;

static const uint8_t cVendorCmdNoData = 0xFC;
static const uint8_t cVendorCmdWr = 0xFD;
static const uint8_t cVendorCmdRd = 0xFE;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

/// @brief NVMe Physical Region Page Entry and List
typedef struct
{
    uint32_t Reserved0:2;   ///< Page Base Address and Offset bits[01:00]
    uint32_t PBAO:30;       ///< Reserved bits[31:02]

} NvmePrpEntryLayout_t;
COMPILE_ASSERT(sizeof(NvmePrpEntryLayout_t) == 4, "NVMe PRP Entry Layout size shall be 4");

/// @brief NVMe Admin Command structure - Common Definitions Dword9..0
typedef struct
{
    uint32_t OPC:8;         ///< Opcode (DW0 bits[7:0])
    uint32_t FUSE:2;        ///< Fused Operation (00=normal, 01=fused 1st command, 10=fused last command, 11=reserved) (DW0[9:8]
    uint32_t reserved0:5;   ///< Reserved (DW0[14:10]
    uint32_t PSDT:1;        ///< PRP or SGL for Data Transfer(PSDT) (DW0[bits(15)]
    uint32_t CID:16;        ///< Command Identifier (DW0[[bits(31:16)]

    uint32_t NSID;          ///< Name space Identifier (DW1)
    uint32_t reserved2[2];  ///< Reserved (DW3..2
    uint64_t MPTR;          ///< Meta data Pointer (DW5..4)
    uint64_t PRP1;          ///< PRP Entry 1 (DW7..6)
    uint64_t PRP2;          ///< PRP Entry 2 (DW9..8)

} AdminCommandCommon_t;
COMPILE_ASSERT(sizeof(AdminCommandCommon_t) == 40, "NVMe command common part size shall be 40");

/// @brief NVMe Admin Command structure - NVM Command Specific - Format NVM - Protection Information Field Definition
typedef  struct
{
    uint32_t PRCHK:3;          ///< Protection Information Check (bits bits[02:00])
    uint32_t PRACT:1;          ///< Protection Information Action (bits bits[03])
    uint32_t reserved4:28;      ///< Reserved (bits bits[31:04])

} EndToEndProtectionInformationField_t;
COMPILE_ASSERT(sizeof(EndToEndProtectionInformationField_t) == 4, "EndToEndProtectionInformationField_t size shall be 4");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------



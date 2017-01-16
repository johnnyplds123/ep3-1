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
//! @brief NVMe I/O command header
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
/// Opcodes for I/O commands
static const uint8_t cFlush = 0x00;
static const uint8_t cWrite = 0x01;
static const uint8_t cRead = 0x02;
static const uint8_t cWriteUncorrectable = 0x04;
static const uint8_t cCompare = 0x05;
static const uint8_t cWriteZeroes = 0x08;
static const uint8_t cDatasetManagement = 0x09;
static const uint8_t cReservationRegister = 0x0D;
static const uint8_t cReservationReport = 0x0E;
static const uint8_t cReservationAcquire = 0x11;
static const uint8_t cReservationRelease = 0x15;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe SGL Descriptor Format
typedef struct
{
    /// Descriptor Type Specific
    uint64_t descriptorTypeSpecificL;      ///< Descriptor Type Specific (bytes[7:0])
    uint64_t descriptorTypeSpecificH:56;   ///< Descriptor Type Specific (bytes[14:8])

    /// SGL Identifier
    uint64_t descriptorTypeSpecific:4;   ///< Descriptor Type Specific (byte[15] bits[3:0])
    uint64_t sglDescriptorType:4;        ///< SGL Descriptor Type (byte[15] bits[7:4])

} NvmeSglDescriptorFormat_t;
COMPILE_ASSERT(sizeof(NvmeSglDescriptorFormat_t) == 16, "NVMe SGL Descriptor Format size shall be 16");

/// @brief NVMe Type Specific SGL Descriptor Format - Data Block
typedef struct
{
    uint64_t address;       ///< The Address field specifies the starting 64-bit memory byte address of the data block (bytes[7:0])
    uint32_t length;        ///< length in bytes of the data block (bytes[11:8])
    uint8_t  reserved[3];    ///< Reserved (bytes[14:12])

    /// SGL Identifier
    uint8_t zero:4;        ///< The Zero field shall have the value of 0h (byte[15] bits[3:0])
    uint8_t sglDescriptorType:4;    ///< SGL Descriptor Type (byte[15] bits[7:4])

} NvmeSglDescriptorFormatDataBlock_t;
COMPILE_ASSERT(sizeof(NvmeSglDescriptorFormatDataBlock_t) == 16, "NVMe SGL Descriptor Format size shall be 16");

/// @brief NVMe Type Specific SGL Descriptor Format - Bit Bucket
typedef struct
{
    uint64_t reserved0;      ///< Reserved (bytes[7:0])
    uint32_t length;         ///< the number of bytes to be discarded (bytes[11:8])
    uint8_t  reserved12[3];  ///< Reserved (bytes[14:12])

    /// SGL Identifier
    uint8_t  zero:4;              ///< The Zero field shall have the value of 0h (byte[15] bits[3:0])
    uint8_t  sglDescriptorType:4;        ///< SGL Descriptor Type (byte[15] bits[7:4])

} NvmeSglDescriptorFormatBitBucket_t;
COMPILE_ASSERT(sizeof(NvmeSglDescriptorFormatBitBucket_t) == 16, "NVMe SGL Descriptor Format size shall be 16");

/// @brief NVMe Type Specific SGL Descriptor Format - Segment descriptor
typedef struct
{
    uint64_t address;        ///< the starting 64-bit memory byte address of the next SGL segment (bytes[7:0])
    uint32_t length;         ///< the length in bytes of the next SGL segment (bytes[11:8])
    uint8_t  reserved12[3];  ///< Reserved (bytes[14:12])

    /// SGL Identifier
    uint8_t  zero:4;              ///< The Zero field shall have the value of 0h (byte[15] bits[3:0])
    uint8_t  sglDescriptorType:4;        ///< SGL Descriptor Type (byte[15] bits[7:4])

} NvmeSglDescriptorFormatSegmentdescriptor_t;
COMPILE_ASSERT(sizeof(NvmeSglDescriptorFormatSegmentdescriptor_t) == 16, "NVMe SGL Descriptor Format size shall be 16");

/// @brief NVMe Type Specific SGL Descriptor Format - Last Segment descriptor
typedef struct
{
    uint64_t address;        ///< the starting 64-bit memory byte address of the next(last) SGL segment (bytes[7:0])
    uint32_t length;         ///< the length in bytes of the next SGL segment (bytes[11:8])
    uint8_t  reserved12[3];  ///< Reserved (bytes[14:12])

    /// SGL Identifier
    uint8_t  zero:4;              ///< The Zero field shall have the value of 0h (byte[15] bits[3:0])
    uint8_t  sglDescriptorType:4;        ///< SGL Descriptor Type (byte[15] bits[7:4])

} NVMeSGLDescriptorFormatLastSegmentdescriptor_t;
COMPILE_ASSERT(sizeof(NVMeSGLDescriptorFormatLastSegmentdescriptor_t) == 16, "NVMe SGL Descriptor Format size shall be 16");

/// @brief NVMe I/O Command structure - Common Definitions Dword9..0
typedef struct
{
    uint32_t OPC:8;         ///< Opcode DW0[7:0]
    uint32_t FUSE:2;        ///< Fused Operation (00=normal, 01=fused 1st command, 10=fused last command, 11=reserved) DW0[9:8]
    uint32_t reserved0:5;   ///< Reserved DW0[14:10]
    uint32_t PSDT:1;        ///< PRP or SGL for Data Transfer(PSDT) DW0[15]
    uint32_t CID:16;        ///< Command Identifier DW0[31:16]

    uint32_t NSID;          ///< Namespace Identifier (DW1)
    uint32_t reserved2[2];  ///< Reserved DW3..2
    uint64_t MPTR_MSGLP;    ///< Metadata Pointer (DW5..4)
    uint64_t PRP1_SGL1;     ///< PRP Entry 1 (DW7..6)
    uint64_t PRP2_SGL1;     ///< PRP Entry 2 (DW9..8)

} IoCommandCommon_t;
COMPILE_ASSERT(sizeof(IoCommandCommon_t)==40, "NVMe command common part size shall be 40");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------



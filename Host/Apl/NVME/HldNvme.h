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
//! @brief Standard NVMe I/F spec and NVMe specific high level driver APIs definitions
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG

// REG

// HAL

// APL
#include "HostBase.h"

// Code Section

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define XCC_CNT                         16

/// @brief NVMe Reset type
typedef enum
{
    cNvmeControllerResetClr = 0,    ///< NVMe controller reset (CC.EN 1 to 0 or NSSR = "NVMe")
    cNvmeControllerResetSet,        ///< NVMe controller reset (CC.EN 0 to 1 or NSSR = "NVMe")
    cNvmeSubsystemReset,            ///< NVMe sub-system reset (NSSR = "NVMe")
    cNvmePCIeReset,                 ///< PCIe reset
} NvmeReset_t;

/// @brief NVMe Command type
typedef enum
{
    cNvmeAdminCmd=0,        ///< NVMe Admin command
    cNvmeIoCmd,             ///< NVMe IO command
} NvmeCmdType_t;

/// @brief NVMe data transfer mode
typedef enum
{
    cNvmeBlockTransfer=0,       ///< NVMe block size data transfer
    cNvmeDwordTransfer=1        ///< NVMe dword size data transfer
} NvmeTransferMode_t;

/// @brief NVMe LBA Format control command
typedef enum
{
    cNvmeFormatDataSize=0,      ///< user data size per block (byte size, should be power of 2 and larger than 512)
    cNvmeFormatMetaSize,        ///< meta data size per block (byte size)
    cNvmeFormatHlbaSize,        ///< host LBA size
    cNvmeFormatHlba,            ///< host LBA insertion (enable or disable)
    cNvmeFormatMpecc,           ///< memory protection ECC (enable or disable)
    cNvmeFormatBcrc,            ///< buffer CRC (enable or disable)
    cNvmeFormatAes,             ///< AES (enable or disable)
} NvmeHlbaFormatControl_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define ADMIN_COMMAND_MASK      0x8000
#define COMMAND_INDEX_MASK      0x0FFF
#define FUNCTION_INDEX_MASK     0x7000

#define IS_ADMIN_COMMAND_ID(ID) ( (ID) & ADMIN_COMMAND_MASK)
#define GET_COMMAND_INDEX(ID)   ( (ID) & COMMAND_INDEX_MASK)
#define GET_FUNCTION_INDEX(ID)  (((ID) & FUNCTION_INDEX_MASK) >> 12)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef uint16_t NvmeQueueId_t;     ///< NVMe queue identifier (by spec)
typedef uint16_t NvmeCmdId_t;       ///< NVMe command Identifier (by Spec)
typedef uint16_t NvmeFunctionId_t;  ///< NVMe function Identifier (by Spec)

/// @brief NVMe Command structure by Spec.
#pragma anon_unions // unnamed union
typedef struct
{
    union
    {
        uint32_t DW0;       ///< all flags
        struct
        {
            uint32_t OPC:8;         ///< Opcode DW0 bits[7:0]
            uint32_t FUSE:2;        ///< Fused Operation (00=normal, 01=fused 1st command, 10=fused last command, 11=reserved) DW0 bits[9:8]
            uint32_t reserved0:5;   ///< Reserved DW0 bits[14:10]
            uint32_t PSDT:1;        ///< PRP or SGL for Data Transfer(PSDT) DW0 bits[15]
            uint32_t CID:16;        ///< Command Identifier DW0 bits[31:16]
        };
    };

    uint32_t NSID;          ///< Namespace Identifier (DW1)
    uint64_t reserved2;     ///< Reaserved (DW3..2)
    uint64_t MPTR_MSGLP;    ///< Metadata pointer (DW5..4)
    uint32_t PRP1_SGL_LO;   ///< PRP Entry 1 or SGL(6)
    uint32_t PRP1_SGL_HI;   ///< PRP Entry 1 or SGL(7)
    uint32_t PRP2_SGL_LO;   ///< PRP Entry 2 or SGL(8)
    uint32_t PRP2_SGL_HI;   ///< PRP Entry 2 or SGL(9)
    uint32_t DW10;          ///< Command specific
    uint32_t DW11;          ///< Command specific
    uint32_t DW12;          ///< Command specific
    uint32_t DW13;          ///< Command specific
    uint32_t DW14;          ///< Command specific
    uint32_t DW15;          ///< Command specific
} NvmeCdb_t;
COMPILE_ASSERT(sizeof(NvmeCdb_t)==64, "NVMe command size shall be 64");

/// @brief NVMe Status structure by Spec.
#pragma anon_unions // unnamed union
typedef struct
{
    uint32_t DW0;

    uint32_t reserved1;     /// Reserved (Dword1)

    union
    {
        uint32_t DW2;       ///< all flags
        struct
        {
            uint32_t SQHD:16;       ///< SQ Head pointer (Dword2 bits[15:00])
            uint32_t SQID:16;       ///< SQ Identifier (Dword2 bits[31:16])
        };
    };

    union
    {
        uint32_t DW3;       ///< all flags
        struct
        {
            uint32_t CID:16;        ///< command Identifier Dword3[15:00])
            uint32_t P:1;           ///< Phase tag (Dword3 bits[16])
            uint32_t SC:8;          ///< Status Code (Dword3 bits[24:17])
            uint32_t SCT:3;         ///< Status Code Type (Dword3 bits[27:25])
            uint32_t reserved:2;    ///< Status field (Dword3 bits[29:28])
            uint32_t M:1;           ///< More (Dword3 bits[30])
            uint32_t DNR:1;         ///< Do Not Ready (Dword3 bits[31])
        };
    };
} NvmeSdb_t;
COMPILE_ASSERT(sizeof(NvmeSdb_t)==16, "NVMe status data size shall be 16");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern uint32_t gResetFlag;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t HldNvme_CreateIoCompletionQueue(uint64_t prp, uint32_t cdId, uint32_t qSize, uint32_t contiguous, uint32_t interruptEnable, uint32_t interruptVector);
Error_t HldNvme_CreateIoSubmissionQueue(uint64_t prp, uint32_t sqId, uint32_t qSize, uint32_t contiguous, uint32_t priority, uint32_t cqId);
Error_t HldNvme_DeleteIoCompletionQueue(NvmeQueueId_t cqId);
Error_t HldNvme_DeleteIoSubmissionQueue(NvmeQueueId_t sqId);
Error_t HldNvme_AbortCommand(NvmeFunctionId_t fid, NvmeCmdId_t cmdId, NvmeQueueId_t sqId);
Error_t HldNvme_SetHlbaFormat(uint32_t index, NvmeHlbaFormatControl_t code, uint32_t param, uint32_t *pParam);
Error_t HldNvme_SetNamespace(uint32_t nsid0);
Error_t HldNvme_GetId(uint32_t hldCmdId, NvmeQueueId_t* pNvmeSqId, NvmeCmdId_t* pNvmeCmdId);

uint64_t HldNvme_GetControllerBusyTime(NvmeCmdType_t cmdType);
NvmeCdb_t* NvmeCmd_GetCdbPointer(HcmdQ_t* pHcmdQ);
NvmeSdb_t* NvmeCmd_GetSdbPointer(HcmdQ_t* pHcmdQ);

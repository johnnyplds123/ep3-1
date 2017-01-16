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
// PKG
#include "BuildPkg.h"
#include "SharePkg.h"
#include "HostPkg.h"
#include "NvmeCmdPkg.h"

// REG

// HAL

// APL
#include "HldNvme.h"
#include "CmdSensor.h"
#include "TcgCommon.h"

// Code Section
#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe I/O Command structure - Read
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

    uint64_t SLBA;          ///< Starting LBA (DW11..10)

    uint32_t NLB:16;        ///< Number of Logical Blocks (DW12 bits[15:00])
    uint32_t reserved12:10; ///< Reserved (DW12 bits[25:16])
    uint32_t PRINFO:4;      ///< Protection Information Field (DW12 bits[29:26])
    uint32_t FUA:1;         ///< Force Unit Access (DW12 bits[30])
    uint32_t LR:1;          ///< Limited Retry (DW12 bits[31])

                                   /// Dataset Management (DSM): attributes for the dataset the LBA(s) being read are associated with
    uint32_t accessFrequency:4;    ///< DSM Access Frequency  (DW13 bits[03:00])
    uint32_t accessLatency:2;      ///< DSM Access Latency    (DW13 bits[05:04])
    uint32_t sequentialRequest:1;  ///< indicates this command is part of sequential write (DW13 bits[06])
    uint32_t incompressible:1;     ///< indicates data is not compressible (DW13 bits[07])
    uint32_t reserved13:24;        ///< Reserved (DW13 bits[31:08])

    uint32_t EILBRT;         ///< (Expected) Initial Logical Block Reference Tag (DW14)

    uint32_t ELBAT:16;       ///< (Expected) Logical Block Application Tag (DW15 bits[15:0])
    uint32_t ELBATM:16;      ///< (Expected) Logical Block Application Tag Mask (DW15 bits[31:16])

} NvmeIoReadCommand_t;
COMPILE_ASSERT(sizeof(NvmeIoReadCommand_t)==64, "NVMe read command size shall be 64");

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void SetupReadTransfer(HcmdQ_t* pHcmdQ);
static void ReadTransferDone(HcmdQ_t* pHcmdQ);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype
//-----------------------------------------------------------------------------
extern uint32_t  TcgMsgOpCode;

//-----------------------------------------------------------------------------
//  Imported function proto-tpye
//-----------------------------------------------------------------------------
extern void TCG_ReadNormal(HcmdQ_t* pHcmdQ, uint16_t RngSts);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    NVMe set up read data transfer

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
static void SetupReadTransfer(HcmdQ_t* pHcmdQ)
{
    SetHcmdHandler(pHcmdQ, ReadTransferDone);

    AplHost_TransferOneQuick(pHcmdQ);
}

//-----------------------------------------------------------------------------
/**
    NVMe Read transfer done call back function

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
static void ReadTransferDone(HcmdQ_t* pHcmdQ)
{
    NvmeCmd_ReturnCacheMsg(pHcmdQ);
}

//-----------------------------------------------------------------------------
/**
    Read command - I/O 02h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_Read(HcmdQ_t* pHcmdQ)
{
#if _TCG_!=TCG_NONE
    if (mReadLockedStatus || (mTcgStatus & MBR_SHADOW_MODE))
    {
        U16 tcgRangeStatus;
        tcgRangeStatus = TcgRangeCheck(pHcmdQ->addrInfo.lba, pHcmdQ->addrInfo.nb, FALSE);

        if (tcgRangeStatus)
        {
            if (tcgRangeStatus == TCG_DOMAIN_ERROR)
            {
                pHcmdQ->error = MK_NVME_ERROR_CODE(cAccessDenied, cMediaErrors, cNoMoreStatusInformation, cDoNotRetry);
                return;
            }
            else
            {
                A_HERE;
                TCG_ReadNormal(pHcmdQ, tcgRangeStatus);
            }
            return;
        }
    }
#endif

    gNvmeIfMgr.hostReadCommands++;
    gNvmeIfMgr.dataUnitsRead += (pHcmdQ->addrInfo.nb << gNvmeIfMgr.ns[pHcmdQ->addrInfo.nsId0].hbu_shift);

    SetupReadTransfer(pHcmdQ);

    AplCmdSensor_ReadCmdDetect(pHcmdQ);
}

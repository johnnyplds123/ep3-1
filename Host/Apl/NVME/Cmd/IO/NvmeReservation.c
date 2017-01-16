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
//! @brief NVMe Reservation related command handler
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"
#include "HostPkg.h"

// REG

// HAL

// APL
#include "HldNvme.h"
#include "NvmeIoCommands.h"

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe I/O Command structure - Reservation Register
typedef struct
{
    IoCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t RREGA:3;         ///< Reservation Register Action (DW10 bits[02:00])
    uint32_t IEKEY:1;         ///< Ignore Existing Key (DW10 bits[03])
    uint32_t reserved10:26;   ///< Reserved (DW10 bits[29:04])
    uint32_t CPTPL:2;         ///< Change Persist Through Power Loss State (DW10 bits[31:30])

    uint32_t reserved11[5];   ///< DW15..11

} NvmeIoReservationRegisterCommand_t;
COMPILE_ASSERT(sizeof(NvmeIoReservationRegisterCommand_t)==64, "NVMe command size shall be 64");

/// @brief NVMe I/O Command Data structure - Reservation Register
typedef struct
{
    uint64_t CRKEY;           ///< Current Reservation Key (Byte[07:00])
    uint64_t NRKEY;           ///< New Reservation Key (Byte[15:08])

} NvmeIoReservationRegisterCommandDataStructure_t;

/// @brief NVMe I/O Command structure - Reservation Report
typedef struct
{
    IoCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t NUMD;              ///< Number of Dwords (DW10)

    uint32_t reserved10[5];      ///< DW15..10

} NvmeIoReservationReportCommand_t;
COMPILE_ASSERT(sizeof(NvmeIoReservationReportCommand_t)==64, "NVMe command size shall be 64");

/// @brief NVMe I/O Command Data structure - Reservation Status
typedef struct
{
    uint32_t GEN;           ///< Generation (Byte[3:0])
    uint32_t RTYPE:8;       ///< Reservation Type (Byte[4])
    uint32_t REGCTL:16;     ///< Number of Registered Controllers (Byte[6:5])
    uint32_t reserved:8;    ///< Reserved (Byte[8:7])
    uint8_t  PTPLS;         ///< Persist Through Power Loss State (Byte[9])
    uint8_t  reserved10[14];    ///< New Reservation Key (Byte[23:10])

} NvmeIoReservationStatusCommandDataStructure_t;
COMPILE_ASSERT(sizeof(NvmeIoReservationStatusCommandDataStructure_t)==24, "Reservation Status Data structure size shall be 24");

/// @brief NVMe I/O Command Data structure - Reservation Status - Registered Controller
typedef struct
{
    uint16_t CNTLID;        ///< Controller ID[1:0])

                            ///Reservation Status (RCSTS)
    uint8_t  RCSTS_Bit0:1;  ///< Reservation Status (Byte[2])
    uint8_t  reserved2:7;   ///< Reservation Status (Byte[2])

    uint8_t reserved3[5];   ///< Reserved (Byte[7:3])
    uint64_t HOSTID;        ///< Host Identifier (Byte[15:8])
    uint64_t RKEY;          ///< Reservation Key (Byte[23:16])

} NvmeIoReservationStatusRegisteredControllerCommandDataStructure_t;

/// @brief NVMe I/O Command structure - Reservation Acquire
typedef struct
{
    IoCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t RACQA:3;           ///< Reservation Acquire Action (DW10[02:00])
    uint32_t IEKEY:1;           ///< Ignore Existing Key (DW10[03])
    uint32_t reserved10_4:4;    ///< Reserved (DW10[07:04])
    uint32_t RTYPE:8;           ///< Reservation Type (DW10[15:08])
    uint32_t reserved10_16:16;   ///< Reserved (DW10[31:16])

    uint32_t reserved11[5];      ///< DW15..11

} NvmeIoReservationAcquireCommand_t;
COMPILE_ASSERT(sizeof(NvmeIoReservationAcquireCommand_t)==64, "NVMe command size shall be 64");

/// @brief NVMe I/O Command Data structure - Reservation Acquire
typedef struct
{
    uint64_t CRKEY;           ///< Current Reservation Key (Byte[07:00])
    uint64_t PRKEY;           ///< Preempt Reservation Key (Byte[15:08])

} NvmeIoReservationAcquireCommandDataStructure_t;

/// @brief NVMe I/O Command structure - Reservation Release
typedef struct
{
    IoCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t RRELA:3;           ///< Reservation Release Action (DW10[02:00])
    uint32_t IEKEY:1;           ///< Ignore Existing Key (DW10[03])
    uint32_t reserved10_4:4;    ///< Reserved (DW10[07:04])
    uint32_t RTYPE:8;           ///< Reservation Type (DW10[15:08])
    uint32_t reserved10_16:16;   ///< Reserved (DW10[31:16])

    uint32_t reserved11[5];      ///< DW15..11

} NvmeIoReservationReleaseCommand_t;
COMPILE_ASSERT(sizeof(NvmeIoReservationReleaseCommand_t)==64, "NVMe command size shall be 64");

/// @brief NVMe I/O Command Data structure - Reservation Release
typedef struct
{
    uint64_t CRKEY;           ///< Current Reservation Key (Byte[07:00])

} NvmeIoReservationReleaseCommandDataStructure_t;


//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-tpye
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**


    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_ReservationRegister(HcmdQ_t* pHcmdQ)
{
    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s\n", __FUNCTION__);
    END_HOST_DBG_DUMP
}

//-----------------------------------------------------------------------------
/**


    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_ReservationReport(HcmdQ_t* pHcmdQ)
{
    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s\n", __FUNCTION__);
    END_HOST_DBG_DUMP
}

//-----------------------------------------------------------------------------
/**


    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_ReservationAcquire(HcmdQ_t* pHcmdQ)
{
    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s\n", __FUNCTION__);
    END_HOST_DBG_DUMP
}

//-----------------------------------------------------------------------------
/**


    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_ReservationRelease(HcmdQ_t* pHcmdQ)
{
    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s\n", __FUNCTION__);
    END_HOST_DBG_DUMP
}


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
//! @brief NVMe Command table
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

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
// register command indirect mode
#define _RC_I(C,F,A)    { \
                            extern void F(HcmdQ_t* pHcmdQ); \
                            pHcmdTbl[i].cmdCode = C; \
                            pHcmdTbl[i].attr = A; \
                            pHcmdTbl[i].fptr = (HcmdHandler_t)(F); \
                            i++; \
                        }

// register command direct mode (command opcode is index of table)
#define _RC_D(C,F,A)    { \
                            extern void F(HcmdQ_t* pHcmdQ);  \
                            COMPILE_ASSERT(C<cNvmeIoCmdTableSize, "command code is larger than table size"); \
                            pHcmdTbl[C].cmdCode = C; \
                            pHcmdTbl[C].attr = A; \
                            pHcmdTbl[C].fptr = (HcmdHandler_t)(F); \
                        }

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-tpye without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Register NVMe Admin commands - setup supported commands and attributes

    @param[out] pHcmdTbl    NVMe command decoding table pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_RegisterAdmin(NvmeCmdTbl_t* pHcmdTbl)
{
    uint32_t i=0;

    // Set NVMe command attribute and bind hanlder function (use indirect mode to save memory)
    _RC_I(cDeleteIoSubmissionQueue , NvmeCmd_DeleteIoSubmissionQueue, 0);
    _RC_I(cCreateIoSubmissionQueue , NvmeCmd_CreateIoSubmissionQueue, 0);
    _RC_I(cGetLogpage              , NvmeCmd_GetLogPage, 0);
    _RC_I(cDeleteIoCompletionQueue , NvmeCmd_DeleteIoCompletionQueue, 0);
    _RC_I(cCreateIoCompletionQueue , NvmeCmd_CreateIoCompletionQueue, 0);
    _RC_I(cIdentify                , NvmeCmd_Identify, 0);
    _RC_I(cAbort                   , NvmeCmd_Abort, 0);
    _RC_I(cSetFeatures             , NvmeCmd_SetFeatures, 0);
    _RC_I(cGetFeatures             , NvmeCmd_GetFeatures, 0);
    _RC_I(cAsynchronousEventRequest, NvmeCmd_AsynchronousEventRequest, ATTR_OVERLAPPED);

#if CO_SUPPORT_FW_DOWNLOAD
    _RC_I(cFirmwareActivate        , NvmeCmd_FirmwareActivate, 0);
    _RC_I(cFirmwareImageDownload   , NvmeCmd_FirmwareImageDownload, ATTR_XFER_H2D);
#endif

#if CO_SUPPORT_NAMESPACE_MANAGEMENT
    _RC_I(cNamespaceManagement     , NvmeCmd_NamespaceManagement, 0);
    _RC_I(cNamespaceAttachment     , NvmeCmd_NamespaceAttachment, 0);
#endif

#if CO_SUPPORT_DEVICE_SELF_TEST
    _RC_I(cDeviceSelfTest          , NvmeCmd_DeviceSelfTest, 0);
#endif

#if CO_SUPPORT_FORMAT
    _RC_I(cFormatNvm               , NvmeCmd_Format, 0);
#endif

#if CO_SUPPORT_SECURITY
    _RC_I(cSecurityReceive         , NvmeCmd_SecurityReceive, 0);
    _RC_I(cSecuritySend            , NvmeCmd_SecuritySend, ATTR_XFER_H2D);
#endif

#if CO_SUPPORT_DIRECTIVES
    _RC_I(cDirectiveSend            , NvmeCmd_DirectiveSend, ATTR_XFER_H2D);
    _RC_I(cDirectiveReceive         , NvmeCmd_DirectiveReceive, 0);
#endif

#if CO_SUPPORT_VENDORCMD
    _RC_I(cVendorCmdNoData          , NvmeCmd_VscMain, 0);
    _RC_I(cVendorCmdWr              , NvmeCmd_VscMain, ATTR_XFER_H2D);
    _RC_I(cVendorCmdRd              , NvmeCmd_VscMain, 0);//ATTR_XFER_D2H
#endif

    pHcmdTbl[i].fptr = NULL; // end of table

    ASSERT(i<cNvmeAdminCmdTableSize);  // check table overflow
}

//-----------------------------------------------------------------------------
/**
    Register NVMe I/O commands - setup supported commands and attributes

    @param[out] pHcmdTbl    NVMe command decoding table pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_RegisterIo(NvmeCmdTbl_t* pHcmdTbl)
{
    uint32_t i;

    // At first, all commands are invalid
    for (i = 0; i < cNvmeIoCmdTableSize; i++)
    {
        pHcmdTbl[i].fptr = (HcmdHandler_t)NvmeCmd_Invalid;
        pHcmdTbl[i].attr = 0;
    }

    // Set NVMe I/O command attribute and bind hanlder function (use direct mode for performance)
    _RC_D(cRead,  NvmeCmd_Read,  ATTR_PERF_CMD | ATTR_AUTO_EOC | ATTR_LBA64 | ATTR_OVERLAPPED | ATTR_ALOC_CACHE | ATTR_UNC);
    _RC_D(cWrite, NvmeCmd_Write, ATTR_PERF_CMD | ATTR_AUTO_EOC | ATTR_LBA64 | ATTR_OVERLAPPED | ATTR_ALOC_CACHE | ATTR_UNC | ATTR_XFER_H2D);
    _RC_D(cFlush, NvmeCmd_Flush, ATTR_OVERLAPPED | ATTR_GLOBAL_NS);

#if CO_SUPPORT_COMPARE
    _RC_D(cCompare, NvmeCmd_Compare, ATTR_XFER_H2D | ATTR_LBA64 | ATTR_ALOC_CACHE);
#endif

#if CO_SUPPORT_WRITE_ZEROES
    _RC_D(cWriteZeroes, NvmeCmd_WriteZeroes, ATTR_LBA64 | ATTR_MDTS | ATTR_UNC);
#endif

#if CO_SUPPORT_DATASET_MANAGEMENT
    _RC_D(cDatasetManagement, NvmeCmd_DatasetManagement, ATTR_XFER_H2D | ATTR_OVERLAPPED);
#endif

#if CO_SUPPORT_RESERVATIONS
    _RC_D(cReservationRegister, NvmeCmd_ReservationRegister, 0);
    _RC_D(cReservationReport,   NvmeCmd_ReservationReport, 0);
    _RC_D(cReservationAcquire,  NvmeCmd_ReservationAcquire, 0);
    _RC_D(cReservationRelease,  NvmeCmd_ReservationRelease, 0);
#endif

#if CO_SUPPORT_WRITE_UNCORRECTABLE
    _RC_D(cWriteUncorrectable, NvmeCmd_WriteUncorrectable, ATTR_LBA64|ATTR_MDTS);
#endif
}

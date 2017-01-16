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
//! @brief Host High Level Driver API definition
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
/// @brief Host data transfer status
typedef enum
{
    cHostTransferInprogress = 0,
    cHostTransferDone=1
} HostTransferStatus_t;

#define HLD_INVALID_CID (0xFFFF)    ///< Invalid command Id
#define ADMIN_TRIM      (0x09)
#define HOST_PAGE_SIZE  (0x1000)

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define HBU2BYTE(X)     ((X)<<9)    ///< convert byte count to host block unit count
#define BYTE2HBU(X)     ((X)>>9)    ///< convert host block unit to byte count

#define HSU2BYTE(X)     ((X)<<9)    ///< convert byte count to host segment unit count
#define BYTE2HSU(X)     ((X)>>9)    ///< convert host segment unit to byte count

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

/// @brief Host command data packet transfer request structure
typedef struct
{
    void*       pBuffer;                ///< buffer pointer (start address)
    XferAttr_t  attr;                   ///< transfer attribute
    uint32_t    xferDoneParam;          ///< transfer done call back parameter (no call back if NULL)
    uint32_t    xferByteCount;          ///< transfer byte count
    int32_t     vbbc;                   ///< valid buffer byte count
    Hlba_t      hlba;                   ///< Host LBA
    uint32_t    mode;                   ///< IO command but need Admin Xfer (TRIM...)
    uint32_t    PRP1_LO;                ///< PRP Entry 1 Low DW
    uint32_t    PRP1_HI;                ///< PRP Entry 1 High DW
    uint32_t    PRP2_LO;                ///< PRP Entry 2 Low DW
    uint32_t    PRP2_HI;                ///< PRP Entry 2 High DW
    uint16_t    ssdIndex;               ///< SSD index
    uint16_t    flowCtrl;               ///< XferFlowCtrl_t flow control (non-streaming, IDS, streamingId1..7)
} XferRequest_t;

typedef uint16_t HldCmdId_t;        ///< command Identifier (by HLD)

/// @brief Host I/F error information
typedef struct
{
    Error_t error;                  ///< error code
    HldCmdId_t cmdId;               ///< command identifier
    uint16_t value;                 ///< interface specific value
} HostErrorInfo_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t     AplHost_PreOneTimeInit(InitBootMode_t initMode);
Error_t     AplHost_OneTimeInit(InitBootMode_t initMode);
void        AplHost_Emulation(uint32_t argc, uint32_t argv[]);
void        AplHost_ResetComplete(Error_t error);
void        AplHost_HandleCmdReceiveError(void);
void        AplHost_HandleEcFifoError(void);
HldCmdId_t  AplHost_PopCommand(void* pCdbBuf, bool wholeCdb);
HldCmdId_t  AplHost_GetCommandFromStaging(void* pCdbBuf, bool wholeCdb);
void        AplHost_CopyCdb(HldCmdId_t hldCmdId, void* pCdbBuf, bool wholeCdb);
void        AplHost_SendStatus(HldCmdId_t cmdId, void* pSdbBuf, bool wholeSdb, uint32_t hostIrq);
void        AplHost_SetSegmentRegion(uint32_t segIndex, uint32_t ssdIndexBeg, uint32_t ssdIndexEnd1, uint8_t* pStart, uint8_t* pEnd1);
void        AplHost_ReleaseDataCache(void);
void        AplHost_ReleaseDataCachePLP(void);
Error_t     AplHost_TransferWholeData_WithPRPList(HcmdQ_t* pHcmdQ, void* pBuffer, uint32_t xferByteCount, bool waitTransferDone);
Error_t     AplHost_TransferWholeData(HcmdQ_t* pHcmdQ, void* pBuffer, uint32_t xferByteCount, bool waitXferDone);
void        AplHost_TransferOneQuick(HcmdQ_t* pHcmdQ);
void        AplHost_TransferOneNonMedia(HcmdQ_t* pHcmdQ, uint32_t xferByteCount);
void        AplHost_DumpInfo(void);

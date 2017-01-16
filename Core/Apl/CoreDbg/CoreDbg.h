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
//! @brief SEQ(Sequencer) Read/Write API codes
//
//=============================================================================
//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
enum
{
    COREDBG_CMD_ERASE = 0,
    COREDBG_CMD_WRITE,
    COREDBG_CMD_READ,
    COREDBG_CMD_ECCSCAN,
    COREDBG_CMD_ERASEALL,
    COREDBG_CMD_WRITE_BLK,
    COREDBG_CMD_READ_BLK,
    COREDBG_CMD_SCANDEFECT,
    COREDBG_CMD_NAND_BUSYTIME,
    COREDBG_CMD_COUNT,
};

typedef struct{
    U8*     wBuffer;
    U8*     rBuffer;
    U8*     eccScanTbl;
}tCOREDBG_BUF;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern tCOREDBG_BUF gCoreDbgBuf;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void CoreDbg_OneTimeInit(InitBootMode_t initMode);
void CoreDbg_Center(U16 opCode, U16 plane, tPAA paa, U16 pairblk1, U16 pairblk2, U16 pairblk3);

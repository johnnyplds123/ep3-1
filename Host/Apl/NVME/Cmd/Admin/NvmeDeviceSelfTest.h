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
#define ShortDSTTime   60     // Short Device Self-test Time
#define ExtenDSTTime   480    // Extended Device Self-test Time (EDSTT):

typedef enum
{
    cDST_SHORT    = 1,
    cDST_EXTENDED = 2,
    cDST_ABORT    = 0x0F,
} Self_test_Code_t;

typedef enum
{
    cDSTCompleted           = 0,  // Operation completed without error
    cDSTAbort               = 1,  // Operation was aborted by a Device Self-test command
    cDSTAbortReset          = 2,  // Operation was aborted by a Controller Level Reset
    cDSTAbortNamespace      = 3,  // Operation was aborted due to a removal of a namespace from the namespace inventory
    cDSTAbortFormat         = 4,  // Operation was aborted due to the processing of a Format NVM command
    cDSTFatalErr            = 5,  // A fatal error or unknown test error occurred while the controller was executing the device self-test operation and the operation did not complete
    cDSTCompletedFail       = 6,  // Operation completed with a segment that failed and the segment that failed is not known
    cDSTCompletedSegmentNum = 7,  // Operation completed with one or more failed segments and the first segment that failed is indicated in the Segment Number field
    cDSTEntryNotUsed        = 0x0F,  //Entry not used
} Self_test_Result_t;

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern uint8_t  gCurrDSTOperation;  // Device Self-test Options (DSTO):
extern uint8_t  gCurrDSTCompletion;
extern uint32_t gCurrDSTTime;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
extern void NvmeDeviceSelfTestInit(InitBootMode_t initMode);
extern void DST_Completion(uint8_t Result);
extern void DSTOperationImmediate(void);
extern void HandleSaveDSTLog(void);

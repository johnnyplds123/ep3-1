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
//! @brief Header file for Media sub-system
//!
//=============================================================================
#include "Fio.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define AU_CNT_SMALL_CMD            64  ///< Small commands have 64 or fewer AUs
#define AU_CNT_LARGE_CMD            128 ///< Large commands have more than 64 AUs and less than 129 AUs
                                        ///< Extra Large commands have more than 128 AUs

#define CORE_PRP_COUNT              0x100
#define CORE_PRP_COUNT_128_ENTRIES  0x80

#define CORE_MSG_ATTR_NVME_RESET    B_0

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define CPU2_ACK_CPU0()     {D_PRINTF("[Core] Sync Host\n");  smCpu0WaitCpu2Flag = SYNC_CPU0_W_CPU2_FLAG;}
#define CPU2_ACK_CPU1()     {D_PRINTF("[Core] Sync Media\n"); smCpu1WaitCpu2Flag = SYNC_CPU1_W_CPU2_FLAG;}

#define CORE_FREE_PRP_CNT_ADD(x) (gCoreFreePrpCnt += x)
#define CORE_FREE_PRP_CNT_DEL(x) (gCoreFreePrpCnt -= x)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern NfInfo_t     gNfInfo;
extern EcuInfo_t    gEcuInfo;
extern U32          gCoreFreePrpCnt;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void CoreMon_StartTask(void);
void Core_StartTask(void);
void Core_JobIssue(tFIO_JOB* pJob);
void Core_JobWait(void);
void Core_ProcHostXfer(MsgHostIO_t* pMsg);
void Core_ProcJobComplete(void);
void Core_ProcForceFlush(void);
void Core_ProcWaitHeapJob(void);
void Core_SendResetCacheMsg(void);

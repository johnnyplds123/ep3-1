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
//! @brief  Shared variable between CPUs
//!
//=============================================================================
#include "HalNfif.h"
#include "HalEcu.h"
#include "HalDram.h"

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef union {
    U8  all[48];
    struct {
        DramDll_t   dramDll;    // 16B
        DramCal_t   dramCal;    // 16B
        SocType_t   socType;    // 8B
        U32         reserved[2];// 8B
    } d;
} tHW_INFO;
COMPILE_ASSERT(sizeof(tHW_INFO) == 48,  "tHW_INFO size shall be 48 bytes");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern          U32*            smHeapB0TcmCpu1;

extern volatile U32             smPlpFlag[];

extern          U32             smActiveFwSlot;
extern          U32             smFwHistoryNumber;
extern          U32             smFwValidStatus;

extern          U16             smcThermalSource[];

extern volatile U32             smThermalTempUpdate;

extern volatile U32             smCpu2WaitCpu1Flag;       ///< Sync CPU2 & CPU1
extern volatile U32             smCpu0WaitCpu2Flag;       ///< Sync CPU0 & CPU2
extern volatile U32             smCpu1WaitCpu2Flag;       ///< Sync CPU0 & CPU1

extern volatile U32             smNfInfoCpu1Addr;
extern volatile U32             smEcuInfoCpu1Addr;

extern volatile PowerControl_t  smPmCtrl[];

extern volatile U32             smAvailableSpare;
extern volatile tHW_INFO        smHwInfo;
extern volatile U32             smOverrideFTL;
extern volatile U32             smNandBusyTime;

extern volatile U32             smUncTblFlag;

extern volatile U32             smSmBusVpdInfo;
extern volatile U32             smHoldCpuX;

extern volatile tDRAMTEST_INFO  smAgingDramInfo;
extern volatile tNAND_INFO      smAgingNandInfo;
extern volatile U32             smAgingDramParameterTag;            ///<

/* End of File */

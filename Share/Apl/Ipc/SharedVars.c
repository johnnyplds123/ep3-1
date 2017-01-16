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
//! @brief Shared variables declaration
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "SharedVars.h"
#include "Fio.h"
#include "CmdSensor.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
__DATA_PERF_SH volatile U32             smDebugFlags[DBGCPU_CNT];           ///< debug Monitor dump flags for Share, Host, Media and Core

// Kernel
__DATA_PERF_SH KernelMetex_t            smMutexPrintf;                      ///< Mutex object for printf synchonization.
__DATA_PERF_SH KernelEvent_t            smEventMonitor;                     ///< Event object for monitor
__DATA_PERF_SH KernelEvent_t            smEventMediaMon;                    ///< Event object for media
__DATA_PERF_SH KernelEvent_t            smEventCoreMon;                     ///< Event object for core

// Allocator
__DATA_PERF_SH AllocatorAo_t            smStcmAllocatorAo;                  ///< shared TCM memory allocator
__DATA_PERF_SH AllocatorAo_t            smSspmAllocatorAo;                  ///< shared SPM memory allocator
__DATA_PERF_SH AllocatorVs_t            smDramAllocatorVs;                  ///< shared DRAM memory allocator

// IPC Message Queue
__align(32)
__DATA_PERF_SH CirBuf_t                 smMsgQueue[cMsgPoolLast];                               ///< message queue container
__DATA_PERF_SH Msg_t                    smMsgEntry[cMsgPoolLast];                               ///< message entry
__align(32)
__DATA_PERF_SH Msg_t                    smHcmdMsg[MAX_HCMD_Q_COUNT + MAX_OTHER_MSG_Q_COUNT];    ///< host command message pool
__DATA_PERF_SH MsgCacheRelObj_t         smCacheRelMsg[MAX_CACHE_REL_Q_COUNT];                   ///< host release cache message pool
volatile __DATA_PERF_SH U32             smCacheRelMsgIdx;

// Boot Mode
volatile __DATA_PERF_SH U32             smInitBootMode;

// FIO Job
__align(32)
__DATA_PERF_SH tFIO_JOB                 smFioJobBuf[FIO_MAX_JOB];
__align(32)
__DATA_PERF_SH FioInfo_t                smFioInfo;
__align(32)
__DATA_PERF_SH U32*                     smHeapB0TcmCpu1;

// PLP
volatile __DATA_PERF_SH U32             smPlpFlag[SOC_NUM_OF_CPU];

// Update FW
         __DATA_PERF_SH U32             smActiveFwSlot;
         __DATA_PERF_SH U32             smFwHistoryNumber;
         __DATA_PERF_SH U32             smFwValidStatus;

// Thermal
         __DATA_PERF_SH U16             smcThermalSource[cThermalSourceCount];

volatile __DATA_PERF_SH U32             smThermalTempUpdate;

// IDS
volatile __DATA_PERF_SH PVOID           smIdsErrHandle[SOC_NUM_OF_CPU];     ///< IDS ErrHandle

// Sync Multi CPU Boot Flag
volatile __DATA_PERF_SH U32             smCpu2WaitCpu1Flag;                 ///< Sync CPU2 & CPU1
volatile __DATA_PERF_SH U32             smCpu0WaitCpu2Flag;                 ///< Sync CPU0 & CPU1
volatile __DATA_PERF_SH U32             smCpu1WaitCpu2Flag;                 ///< Sync CPU0 & CPU1

// NAND Flash related
volatile __DATA_PERF_SH U32             smNfInfoCpu1Addr;                   ///< address of CPU1's gNfInfo
volatile __DATA_PERF_SH U32             smEcuInfoCpu1Addr;                  ///< address of CPU1's gEcuInfo

__align(32)
volatile __DATA_PERF_SH PowerControl_t  smPmCtrl[SOC_NUM_OF_CPU];           ///< Power Control

volatile __DATA_PERF_SH UartDynamic_t   smUartDynamic;

         __DATA_PERF_SH CmdSensor_t     smCmdSensor;                        ///< Command Sensor

volatile __DATA_PERF_SH U32             smAvailableSpare;

volatile __DATA_PERF_SH tHW_INFO        smHwInfo;                           ///< HWInfo
volatile __DATA_PERF_SH U32             smOverrideFTL;                      ///< SWBackDoor
volatile __DATA_PERF_SH U32             smNandBusyTime;                     ///<

volatile __DATA_PERF_SH U32             smUncTblFlag;

volatile __DATA_PERF_SH U32             smSmBusVpdInfo;
volatile __DATA_PERF_SH U32             smHoldCpuX;

#if CHK_FW_TYPE(FW_AGING)
volatile __DATA_PERF_SH tDRAMTEST_INFO  smAgingDramInfo;                    ///<
volatile __DATA_PERF_SH tNAND_INFO      smAgingNandInfo;                    ///<
volatile __DATA_PERF_SH U32             smAgingDramParameterTag;            ///<

#endif

/* End of File */

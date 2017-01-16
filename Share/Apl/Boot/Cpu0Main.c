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
//! @brief C main() routine for CPU0
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "Monitor.h"
#include "Cache.h"
#include "HldNvme.h"
#include "RegPciePhy.h"
#include "CmdSensor.h"

#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void Cpu0Main_OneTimeInit(InitBootMode_t initMode);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
__DATA_PERF_UI __align(16) uint8_t MonTaskStack[2048];   ///< Stack and TCB for monitor task
__DATA_PERF_UI __align(16) uint8_t NvmeTaskStack[3072];  ///< Stack and TCB for NVMe command service task

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
extern uint32_t gIdleEventTimer;

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern void Nvme_StartTask(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    main() for CPU0\n
      - start Core codes.\n
      - One time initialization for all Core modules.\n
      - Create tasks for CPU0\n
      - Start CPU1\n
      - Start Kernel for CPU0\n

   @callgraph
**/
//-----------------------------------------------------------------------------
int main(void)
{
    InitBootMode_t initMode;
    uint32_t cpuId;

    initMode = Hal_GetInitBootMode();
    Debug_DumpVersion();

    Cpu0Main_OneTimeInit(initMode);

    #if CHK_FW_TYPE(FW_AGING)
    HalTimer_SetSmartTimerInterruptFrequency(SMARTCHECK_TICK_PER_SEC);
   	HalIrq_RegisterHandler(cIrqTimerSmartCheck, HalTimer_SmartCheckTimer, ENABLE);
    #endif

    Cache_OneTimeInit(initMode);

    smCpu0WaitCpu2Flag = smCpu1WaitCpu2Flag = smCpu2WaitCpu1Flag = 0;

#if _MEDIA_!=MEDIA_DRAM // ready to run all other CPUs
    for ( cpuId=CPU_ID1; cpuId<SOC_NUM_OF_CPU; cpuId++)
    {
        HalIpc_WriteCmdTo(cpuId, cIpcCmdReadyToRun, 0, 0);
    }
#endif

    Kernel_Start();

    return 0;
}

//-----------------------------------------------------------------------------
/**
   Timer for APST
**/
//-----------------------------------------------------------------------------
void Cpu0Main_PeriodicTimerHandler(void)
{
    #if CO_SUPPORT_DEVICE_POWER_MANAGEMENT
    if (gPmInfo.bypassTimerValue)     gPmInfo.bypassTimerValue--;
    #endif

    if (gIdleEventTimer < DWORD_MASK) gIdleEventTimer++;
}

//-----------------------------------------------------------------------------
/**
   One Shot Timer for Power State Change
**/
//-----------------------------------------------------------------------------
void Cpu0OneshotTimer(uint32_t para)
{
    gIdleEventTimer += gPmInfo.lastEsplasedTime;
}

//-----------------------------------------------------------------------------
/**
    One time initialization for core modules.\n

     - Serial Flash module\n
     - IRQ(Interrupt Request) modue\n
     - IPC(Inter Process Comuniation) module\n
     - Kernel and Kernel Synchronization objects\n
     - Create tasks\n

    @param[in]  initMode        initialization mode
**/
//-----------------------------------------------------------------------------
static void Cpu0Main_OneTimeInit(InitBootMode_t initMode)
{
    Hal_OneTimeInit(initMode);

    Ipc_OneTimeInit(initMode);

    // kernel initialization
    Kernel_OneTimeInit(initMode);
    Kernel_InitMutex(MUTEX_PRINTF, cSpinLockPrintf);

    HalTimer_SetPeriodicInterruptFrequency(KERNEL_TICK_PER_SEC);
    HalIrq_RegisterHandler(cIrqTimerPeriodic, Kernel_TimerIsr, ENABLE);
    Kernel_SetTimerCallBack(Cpu0Main_PeriodicTimerHandler);
    HalIrq_RegisterHandler(cIrqTimerOneshot, Cpu0OneshotTimer, ENABLE);

    Kernel_InitEventFlags(EVENT_OBJ_MONITOR, KERNEL_NO_SPINLOCK);
    Kernel_CreateTask(Mon_StartTask,  MonTaskStack,   sizeof(MonTaskStack),  0, "Monitor");
    Kernel_CreateTask(Nvme_StartTask, NvmeTaskStack,  sizeof(NvmeTaskStack), 1, "Nvme");
}

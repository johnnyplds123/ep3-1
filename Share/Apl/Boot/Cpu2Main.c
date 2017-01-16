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
//! @brief C main() routine for CPU1
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "Core.h"

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
static void Cpu2Main_OneTimeInit(InitBootMode_t initMode);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
__DATA_PERF_UI __align(16) uint8_t CoreMonTaskStack[3072];      ///< Stack and TCB for media emulation task
__DATA_PERF_UI __align(16) uint8_t CoreTaskStack[3072];         ///< Stack and TCB for media emulation task

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
static void Cpu2Main_OneTimeInit(InitBootMode_t initMode);
extern void Core_CreateTasks(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Actual main() for CPU2 to avoid link error in case of shared architecture\n
      - start media task code.\n

   @callgraph
**/
//-----------------------------------------------------------------------------
int main(void)
{
    InitBootMode_t initMode;

    D_PRINTF("[CPU2] Start\n");

    initMode = Hal_GetInitBootMode();

    Cpu2Main_OneTimeInit(initMode);

    Kernel_Start();

    return 0;
}

//-----------------------------------------------------------------------------
/**
    One time initialization for shared modules.\n

     - IRQ(Interrupt Request) modue\n
     - IPC(Inter Process Comuniation) module\n
     - Kernel and Kernel Synchronization objects\n
     - Create tasks\n

    @param[in]  initMode        initialization mode
**/
//-----------------------------------------------------------------------------
static void Cpu2Main_OneTimeInit(InitBootMode_t initMode)
{
    HalIrq_OneTimeInit(initMode);

    // kernel initialization
    Kernel_OneTimeInit(initMode);
    //HalIrq_RegisterHandler(cIrqTimerPeriodic, Kernel_TimerIsr, ENABLE);

    Kernel_CreateTask(CoreMon_StartTask, CoreMonTaskStack,  sizeof(CoreMonTaskStack),   0, "CoreMon");
    Kernel_CreateTask(Core_StartTask,    CoreTaskStack,     sizeof(CoreTaskStack),      1, "Core");
}

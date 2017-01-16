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
#include "Media.h"
#include "AplReadRetry.h"

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
static void Cpu1Main_OneTimeInit(InitBootMode_t initMode);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
__DATA_PERF_UI __align(16) uint8_t MediaMonTaskStack[1024];      ///< Stack and TCB for media emulation task
__DATA_PERF_UI __align(16) uint8_t MediaTaskStack[3072];         ///< Stack and TCB for media emulation task
uint32_t gCpu1TimerTick;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
extern uint32_t gTargetPatrolBlock;

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Actual main() for CPU1 to avoid link error in case of shared architecture\n
      - start media task code.\n

   @callgraph
**/
//-----------------------------------------------------------------------------
int main(void)
{
    InitBootMode_t initMode;

    D_PRINTF("[CPU1] Start\n");
    initMode = Hal_GetInitBootMode();

    Cpu1Main_OneTimeInit(initMode);

    Kernel_Start();

    return 0;
}

//-----------------------------------------------------------------------------
/**
    Cpu1 periodic timer handler
**/
//-----------------------------------------------------------------------------
void Cpu1Main_PeriodicTimerHandler(void)
{
    gCpu1TimerTick++;

    if (gCpu1TimerTick == 100)
    {
        gCpu1TimerTick = 0;
        if (++gTargetPatrolBlock >= gNfInfo.blocksPerPlane)
        {
            gTargetPatrolBlock = 0;
        }
    }
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
static void Cpu1Main_OneTimeInit(InitBootMode_t initMode)
{
    HalIrq_OneTimeInit(initMode);

    // kernel initialization
    Kernel_OneTimeInit(initMode);
    HalIrq_RegisterHandler(cIrqTimerPeriodic, Kernel_TimerIsr, ENABLE);
    Kernel_SetTimerCallBack(Cpu1Main_PeriodicTimerHandler);

    Kernel_CreateTask(MediaMon_StartTask, MediaMonTaskStack, sizeof(MediaMonTaskStack), 0, "MediaMon");
    Kernel_CreateTask(Media_StartTask,    MediaTaskStack,    sizeof(MediaTaskStack),    1, "Media");

    gCpu1TimerTick = 0;
}

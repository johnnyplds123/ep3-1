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
//! @brief Header file for RTOS Kernel API
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define KERNEL_TICK_PER_MSEC             1
#define KERNEL_TICK_PER_SEC           1000    ///< Kernel time tick resolution

#define KERNEL_NO_TIMEOUT     (0xFFFFFFFF)    ///< no time out
#define KERNEL_NO_SPINLOCK    (0xFFFFFFFF)    ///< no spinlock (for single CPU sync object)

#define KERNEL_SYNC_OBJ_NOT_READY     0       ///< synchronization object not ready
#define KERNEL_WAIT_TIMEDOUT          0       ///< synchronization object wait timed out

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define KERNEL_RESCHEDULE(X)        Kernel_StateChanged()

#define Kernel_SleepMsec(T)           Kernel_WaitSyncObj(NULL, 0, T)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef void* KernelTaskId_t;            ///< task identifier
typedef uint32_t   KernelTimerTick_t;    ///< time tick

typedef void (*KernelTimerFunc_t)(void); ///< kernel timer call back function
typedef void (*KernelTaskFunc_t)(void);  ///< kernel task function
typedef bool (*KernelPollFunc_t)(uint32_t param);  ///< wait for call back function (polling)
typedef uint32_t  (*KernelSyncObjVptr_t)(KernelTaskId_t, void*, uint32_t); ///< Kernel Synchronazation object handler

/// @brief Kernel Synchronization object sturcture - to emulate C++ abstract class
typedef struct
{
    KernelSyncObjVptr_t vptrCheck;   ///< virtual function pointer to check Sync Object
    uint32_t spinLockId;             ///< For multi-processor synchronization
} KernelSyncObj_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t Kernel_OneTimeInit(InitBootMode_t initMode);
void   Kernel_Start(void);
void   Kernel_IrqEntry(void);
void   Kernel_FiqEntry(void);
void   Kernel_TimerIsr(uint32_t param);

KernelTaskId_t Kernel_CreateTask(KernelTaskFunc_t taskFunc, void* stackPointer, uint32_t stackSize, uint8_t priority, Cstr_t taskName);
KernelTaskId_t Kernel_GetCurrentTaskId(void);

uint32_t Kernel_WaitSyncObj(KernelSyncObj_t* pSyncObj, uint32_t value, uint32_t timeOutMsec);
uint32_t Kernel_WaitPollObj(KernelPollFunc_t testCallBack, uint32_t testParam, uint32_t yield, uint32_t timeOutUsec);

void Kernel_Yield(void);
void Kernel_StateChanged(void);

KernelTimerFunc_t Kernel_SetTimerCallBack(KernelTimerFunc_t userCallBackFunction);

KernelTimerTick_t Kernel_ReadTick(void);
uint64_t Kernel_GetElapsedTimeMsec(KernelTimerTick_t lastTick);

void Kernel_SetTimeSec(uint32_t second);
uint32_t  Kernel_GetTimeSec(void);
uint64_t  Kernel_GetTimeMsec(void);

void Kernel_DumpInfo(void);
void Kernel_DumpTcb(KernelTaskId_t taskId);

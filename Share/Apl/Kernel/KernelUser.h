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
//! @brief Header file for RTOS Kernel User Parameter
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Compile time options
//-----------------------------------------------------------------------------
/// @name Kernel compile options
/// @{
#define CO_KRNL_IPS             DISABLE       ///< Inter Processor Synchronization object support
/// @}

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
                                        ///  MONITOR EVENT FLAGS definition
#define EVENT_MONITOR_CMD_IN        (1<<0)  ///< bit 0, Monitor command in

                                        ///  HOST EVENT FLAGS definition
#define EVENT_HOST_RESET            (1<<0)  ///< Host reset
#define EVENT_HOST_CMD_IN           (1<<1)  ///< Host commands in (not fetched yet)
#define EVENT_HOST_EXE_READY        (1<<2)  ///< Host command (ready list filled)
#define EVENT_HOST_XFER_DONE        (1<<3)  ///< Host transfer done
#define EVENT_HOST_XFER_ABORTED     (1<<4)  ///< Host transfer aborted by error or reset
#define EVENT_HOST_SHUTDOWN         (1<<5)  ///< Host shutdown notification
#define EVENT_HOST_COLLISION        (1<<6)  ///< Host command collision resolved
#define EVENT_HOST_RESOURCE         (1<<7)  ///< Host resource is available
#define EVENT_HOST_ERROR            (1<<8)  ///< Host error
#define EVENT_HOST_POWER_MANAGEMENT (1<<9)  ///< Host power management

                                        ///  CORE EVENT FLAGS definition
#define EVENT_COREMON_RESET         (1<<0)  ///< Core reset
#define EVENT_COREMON_MSG_IN        (1<<1)  ///< Core message in (not fetched yet)

                                        ///  MEDIA EVENT FLAGS definition
#define EVENT_MEDIAMON_RESET        (1<<0)  ///< Media reset
#define EVENT_MEDIAMON_MSG_IN       (1<<1)  ///< Media message in (not fetched yet)
#define EVENT_MEDIA_JOB_IN          (1<<2)  ///< MIO JOB in
#define EVENT_MEDIA_JOB_DONE        (1<<3)  ///< MIO JOB done
#define EVENT_MEDIA_JOB_PENDING     (1<<4)  ///< MIO JOB pending

/// @brief Spin lock Identifer for multi CPU synchronization
typedef enum
{
    cSpinLockPrintf=0,                ///< printf
    cSpinLockTrackMgr,                ///< track manager
    cSpinLockMemAlloc,                ///< memory allocation
    cSpinLockUncTable,                ///< ecc table insert or delete
    cSpinLockSFlash,                  ///< sflash op
} SpinLockId_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define MUTEX_PRINTF        (&smMutexPrintf)
#define EVENT_OBJ_MONITOR   (&smEventMonitor)
#define EVENT_OBJ_MEDIAMON  (&smEventMediaMon)
#define EVENT_OBJ_COREMON   (&smEventCoreMon)

#if CO_KRNL_IPS
    #define SYNC_OBJ_BEGIN_CS(X,S)      BEGIN_MULTI_CS(X,(S)->syncObj.spinLockId)
    #define SYNC_OBJ_END_CS(X,S)        END_MULTI_CS(X,(S)->syncObj.spinLockId)
    #define SYNC_OBJ_SPIN_LOCK(S)       HalCpu_SpinLock((S)->syncObj.spinLockId)
    #define SYNC_OBJ_SPIN_UNLOCK(S)     HalCpu_SpinUnlock((S)->syncObj.spinLockId)
#else
    #define SYNC_OBJ_BEGIN_CS(X,S)      BEGIN_CS(X)
    #define SYNC_OBJ_END_CS(X,S)        END_CS(X)
    #define SYNC_OBJ_SPIN_LOCK(S)
    #define SYNC_OBJ_SPIN_UNLOCK(S)
#endif

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef uint32_t   KernelEventFlags_t;             ///< event bits

/// @brief EVENT object - KernelSyncObj_t is base class
typedef struct
{
    KernelSyncObj_t syncObj;             ///< synchronization object
    KernelEventFlags_t   flags;          ///< event flags, 32 individual events
} KernelEvent_t;

/// @brief MUTEX object - KernelSyncObj_t is base class
typedef struct
{
    KernelSyncObj_t syncObj;             ///< synchronization object
    uint32_t            count;           ///< lock count
    KernelTaskId_t  owner;               ///< mutex owner
} KernelMetex_t;

/// @brief Semaphore object, KernelSyncObj_t is base class
typedef struct
{
    KernelSyncObj_t syncObj;             ///< synchronization object
    uint32_t            count;           ///< available count
} KernelSemaphore_t;

/// @brief Doubly Linked list queue object, KernelSyncObj_t is base class
typedef struct
{
    KernelSyncObj_t syncObj;             ///< synchronization object
    LinkHeadTail_t list;                 ///< queue head and tail pointer
    uint32_t count;                      ///< queue fill count
} KernelDlQueue_t;

/// @brief Message object, KernelSyncObj_t is base class
typedef struct
{
    KernelSyncObj_t syncObj;             ///< synchronization object
    void* pMsg;                          ///< message pointer
} KernelMsg_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern KernelMetex_t  smMutexPrintf;      ///< Mutex Object for print
extern KernelEvent_t  smEventMonitor;     ///< Event Object for monitor
extern KernelEvent_t  smEventMediaMon;    ///< Event object for media
extern KernelEvent_t  smEventCoreMon;     ///< Event object for core

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void Kernel_IdleTask(void);

void Kernel_InitEventFlags(KernelEvent_t* pEvent, uint32_t spinLockId);
void Kernel_SetEventFlags(KernelEvent_t* pEvent, KernelEventFlags_t eventFlags);
void Kernel_ClearEventFlags(KernelEvent_t* pEvent, KernelEventFlags_t eventFlags);
KernelEventFlags_t Kernel_TestEventFlags(KernelEvent_t* pEvent, KernelEventFlags_t eventFlags);
KernelEventFlags_t Kernel_WaitEventFlags(KernelEvent_t* pEvent, KernelEventFlags_t eventFlags, uint32_t timeOutMsec);

void Kernel_InitMutex(KernelMetex_t* pMutex, uint32_t spinLockId);
void Kernel_UnlockMutex(KernelMetex_t* pMutex);
bool Kernel_LockMutex(KernelMetex_t* pMutex, uint32_t timeOutMsec);

void Kernel_InitSemaphore(KernelSemaphore_t* pSemaphore, uint32_t spinLockId, uint32_t count);
void Kernel_ReleaseSemaphore(KernelSemaphore_t* pSemaphore, uint32_t count);
bool Kernel_AcquireSemaphore(KernelSemaphore_t* pSemaphore, uint32_t count, uint32_t timeOutMsec);

void Kernel_InitDlQueue(KernelDlQueue_t* pDlQueue, uint32_t spinLockId);
void Kernel_PushToHead(KernelDlQueue_t* pDlQueue, DlinkEntry_t* pEntry);
void Kernel_PushToTail(KernelDlQueue_t* pDlQueue, DlinkEntry_t* pEntry);
DlinkEntry_t* Kernel_GetDlQueue(KernelDlQueue_t* pDlQueue, uint32_t timeOutMsec);

void Kernel_InitMessage(KernelMsg_t* pKrnlMsg, uint32_t spinLockId, void* pMsg);
bool Kernel_WaitMessage(KernelMsg_t* pKrnlMsg, uint32_t desiredState, uint32_t timeOutMsec);



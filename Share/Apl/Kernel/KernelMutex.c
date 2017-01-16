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
//! @brief  Mutex Synchronization object code
//!
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Compile time options
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define MUTEX_NO_OWNER  ((void*)(1))

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

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    check if mutex object has an owner.\n

    @param[in]  taskId          Kernel Task Id
    @param[in]  pMutex          Kernel mutex object pointer
    @param[in]  notUsed         not used (to match prototype)

    @return zero for mutex was already owned by other task, one for owned by this task.
**/
//-----------------------------------------------------------------------------
static uint32_t Kernel_CheckMutex(KernelTaskId_t taskId, KernelMetex_t* pMutex, uint32_t notUsed)
{
    uint32_t result;

    result = KERNEL_SYNC_OBJ_NOT_READY;

    SYNC_OBJ_SPIN_LOCK(pMutex);

    if (pMutex->owner==taskId || pMutex->count==0)
    {
        pMutex->count++;
        pMutex->owner = taskId;
        result=1;
    }

    SYNC_OBJ_SPIN_UNLOCK(pMutex);

    return(result);
}

//-----------------------------------------------------------------------------
/**
    Initialize mutex object.\n
      - clear mutex lock count.\n

    @param[in]  pMutex          Kernel mutex object pointer
    @param[in]  spinLockId      spin lock Id (KERNEL_NO_SPINLOCK for single CPU mutex)
**/
//-----------------------------------------------------------------------------
void Kernel_InitMutex(KernelMetex_t* pMutex, uint32_t spinLockId)
{
    pMutex->count = 0;
    pMutex->owner = MUTEX_NO_OWNER;
    pMutex->syncObj.vptrCheck = (KernelSyncObjVptr_t)Kernel_CheckMutex;
    pMutex->syncObj.spinLockId = spinLockId;
}

//-----------------------------------------------------------------------------
/**
    Unlock mutex.\n
      - Only owner task can unlock mutex.

    @param[in]  pMutex          Kernel mutex object pointer
**/
//-----------------------------------------------------------------------------
void Kernel_UnlockMutex(KernelMetex_t* pMutex)
{
    CpuCs_t orgState;

    SYNC_OBJ_BEGIN_CS(orgState, pMutex);

    if (pMutex->owner==Kernel_GetCurrentTaskId())
    {
        pMutex->count--;
        if (pMutex->count==0)
        {
            pMutex->owner = MUTEX_NO_OWNER;
            KERNEL_RESCHEDULE(TRUE);
        }
    }

    SYNC_OBJ_END_CS(orgState, pMutex);
}

//-----------------------------------------------------------------------------
/**
    Lock mutex.\n
      - mutex can be locked multiple times by owner task.\n

    @param[in]  pMutex          Kernel mutex object pointer
    @param[in]  timeOutMsec     lock timeout in milli-second unit

    @return  zero if timed out, else if mutex is successfully locked.

    @remark Task scheduling is always involved regardless of semaphore state.
**/
//-----------------------------------------------------------------------------
bool Kernel_LockMutex(KernelMetex_t* pMutex, uint32_t timeOutMsec)
{
    return Kernel_WaitSyncObj((KernelSyncObj_t *)pMutex, 1, timeOutMsec);
}

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
//! @brief  Event Synchronization object code
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
    check if semaphore can be acquired.\n

    @param[in]  taskId          Kernel Task Id
    @param[in]  pSemaphore      Kernel semaphore object pointer
    @param[in]  count           initial semaphore count

    @return 0 if request semaphore count was not aquired, else acquired.
**/
//-----------------------------------------------------------------------------
static uint32_t Kernel_CheckSemaphore(KernelTaskId_t taskId, KernelSemaphore_t* pSemaphore, uint32_t count)
{
    uint32_t result;

    result = KERNEL_SYNC_OBJ_NOT_READY;

    SYNC_OBJ_SPIN_LOCK(pSemaphore);

    if (pSemaphore->count>=count)
    {
        pSemaphore->count -= count;
        result = 1;
    }

    SYNC_OBJ_SPIN_UNLOCK(pSemaphore);

    return(result);
}

//-----------------------------------------------------------------------------
/**
    Initialize semaphore object.\n
      - set initial semaphore count.\n

    @param[in]  pSemaphore      Kernel semaphore object pointer
    @param[in]  spinLockId      spin lock Id (KERNEL_NO_SPINLOCK for single CPU semaphore)
    @param[in]  count           initial semaphore count
**/
//-----------------------------------------------------------------------------
void Kernel_InitSemaphore(KernelSemaphore_t* pSemaphore, uint32_t spinLockId, uint32_t count)
{
    pSemaphore->count = count;
    pSemaphore->syncObj.vptrCheck = (KernelSyncObjVptr_t)Kernel_CheckSemaphore;
    pSemaphore->syncObj.spinLockId = spinLockId;
}

//-----------------------------------------------------------------------------
/**
    Release semaphore count.\n

    @param[in]  pSemaphore      Kernel semaphore object pointer
    @param[in]  count           semaphore count to release
**/
//-----------------------------------------------------------------------------
void Kernel_ReleaseSemaphore(KernelSemaphore_t* pSemaphore, uint32_t count)
{
    CpuCs_t orgState;

    SYNC_OBJ_BEGIN_CS(orgState, pSemaphore);

    pSemaphore->count += count;
    KERNEL_RESCHEDULE(TRUE);

    SYNC_OBJ_END_CS(orgState, pSemaphore);
}

//-----------------------------------------------------------------------------
/**
    Acquire semaphore count.\n

    @param[in]  pSemaphore      Kernel semaphore object pointer
    @param[in]  count           semaphore count to acquire
    @param[in]  timeOutMsec     wait timeout in milli-second unit

    @return  zero if timed out, else if semaphore count is successfully acquired.

    @remark Task scheduling is always involved regardless of semaphore state.
**/
//-----------------------------------------------------------------------------
bool Kernel_AcquireSemaphore(KernelSemaphore_t* pSemaphore, uint32_t count, uint32_t timeOutMsec)
{
    return Kernel_WaitSyncObj((KernelSyncObj_t *)pSemaphore, count, timeOutMsec);
}


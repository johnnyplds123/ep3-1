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
//! @brief  Doubly Linked List queue object
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
#define HEAD_OF_QUEUE_OP    0   ///< Get queue from head of queue operation
#define TAIL_OF_QUEUE_OP    1   ///< Get queue from tail of queue operation

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
    Check if Doubly Linked Queue is empty.\n

    @param[in]  taskId          Kernel Task Id
    @param[in]  pDlQueue        Kernel Doubly linked queue object pointer
    @param[in]  operation       operation code (0=head of queue, tail of queue)

    @return 0 if empty, else queue entry pointer
**/
//-----------------------------------------------------------------------------
static uint32_t Kernel_CheckDlQueue(KernelTaskId_t taskId, KernelDlQueue_t* pDlQueue, uint32_t operation)
{
    uint32_t result;

    result = KERNEL_SYNC_OBJ_NOT_READY;

    SYNC_OBJ_SPIN_LOCK(pDlQueue);

    // queue is not empty
    if (pDlQueue->count)
    {
        pDlQueue->count--;
        result = (uint32_t)DlPopFromHead(&pDlQueue->list);
    }

    SYNC_OBJ_SPIN_UNLOCK(pDlQueue);

    return(result);
}

//-----------------------------------------------------------------------------
/**
    Initialize Doubly Linked Queue object.\n

    @param[in]  pDlQueue        Kernel Doubly linked queue object pointer
    @param[in]  spinLockId      spin lock Id (KERNEL_NO_SPINLOCK for single CPU DlQueue)
**/
//-----------------------------------------------------------------------------
void Kernel_InitDlQueue(KernelDlQueue_t* pDlQueue, uint32_t spinLockId)
{
    pDlQueue->count = 0;
    DlInit(&pDlQueue->list);
    pDlQueue->syncObj.vptrCheck = (KernelSyncObjVptr_t)Kernel_CheckDlQueue;
    pDlQueue->syncObj.spinLockId = spinLockId;
}

//-----------------------------------------------------------------------------
/**
    Get queue entry form head of queue pointer.

    @param[in]  pDlQueue        Kernel Doubly linked queue object pointer
    @param[in]  timeOutMsec     time out in milli-seconds unit.

    @return     zero if timed out, else queue entry poiner

    @remark     Task scheduling is always involved regardless of queue state.
**/
//-----------------------------------------------------------------------------
DlinkEntry_t* Kernel_GetDlQueue(KernelDlQueue_t* pDlQueue, uint32_t timeOutMsec)
{
    return (DlinkEntry_t*)Kernel_WaitSyncObj((KernelSyncObj_t *)pDlQueue, HEAD_OF_QUEUE_OP, timeOutMsec);
}

//-----------------------------------------------------------------------------
/**
    Push queue entry to head of queue pointer

    @param[in]  pDlQueue        Kernel Doubly linked queue object pointer
    @param[in]  pEntry          queue entry pointer
**/
//-----------------------------------------------------------------------------
void Kernel_PushToHead(KernelDlQueue_t* pDlQueue, DlinkEntry_t* pEntry)
{
    CpuCs_t orgState;

    SYNC_OBJ_BEGIN_CS(orgState, pDlQueue);

    pDlQueue->count++;
    DlAddToHead(&pDlQueue->list, pEntry);

    KERNEL_RESCHEDULE(TRUE);

    SYNC_OBJ_END_CS(orgState, pDlQueue);
}

//-----------------------------------------------------------------------------
/**
    Push queue entry to tail of queue pointer

    @param[in]  pDlQueue        Kernel Doubly linked queue object pointer
    @param[in]  pEntry          queue entry pointer
**/
//-----------------------------------------------------------------------------
void Kernel_PushToTail(KernelDlQueue_t* pDlQueue, DlinkEntry_t* pEntry)
{
    CpuCs_t orgState;

    SYNC_OBJ_BEGIN_CS(orgState, pDlQueue);

    pDlQueue->count++;
    DlAddToTail(&pDlQueue->list, pEntry);

    KERNEL_RESCHEDULE(TRUE);

    SYNC_OBJ_END_CS(orgState, pDlQueue);
}


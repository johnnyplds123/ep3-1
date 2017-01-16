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
//! @brief  Message Synchronization object code
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
#define Message_NO_OWNER  ((void*)(1))

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
    check if Message object has an owner.\n

    @param[in]  taskId          Kernel Task Id
    @param[in]  pKrnlMsg        Kernel message object pointer
    @param[in]  desiredState    desired message state

    @return zero for Message was already owned by other task, one for owned by this task.
**/
//-----------------------------------------------------------------------------
static uint32_t Kernel_CheckMessag(KernelTaskId_t taskId, KernelMsg_t* pKrnlMsg, uint32_t desiredState)
{
    Msg_t* pMsg;

    pMsg = (Msg_t *)pKrnlMsg->pMsg;

    if (pMsg->hdr.b.status == desiredState)
    {
        return 1;
    }

    return(KERNEL_SYNC_OBJ_NOT_READY);
}

//-----------------------------------------------------------------------------
/**
    Initialize Message object.\n
      - clear Message lock count.\n

    @param[in]  pKrnlMsg      Kernel message object pointer
    @param[in]  spinLockId    spin lock Id (KERNEL_NO_SPINLOCK for single CPU Message)
**/
//-----------------------------------------------------------------------------
void Kernel_InitMessage(KernelMsg_t* pKrnlMsg, uint32_t spinLockId, void* pMsg)
{
    pKrnlMsg->syncObj.vptrCheck = (KernelSyncObjVptr_t)Kernel_CheckMessag;
    pKrnlMsg->syncObj.spinLockId = spinLockId;
    pKrnlMsg->pMsg = pMsg;
}

//-----------------------------------------------------------------------------
/**
    Wait until desired message state was set

    @param[in]  pKrnlMsg        Kernel message object pointer
    @param[in]  desiredState    desired message state
    @param[in]  timeOutMsec     lock timeout in milli-second unit

    @return  zero if timed out, else if Message is successfully locked.

    @remark Task scheduling is always involved regardless of semaphore state.
**/
//-----------------------------------------------------------------------------
bool Kernel_WaitMessage(KernelMsg_t* pKrnlMsg, uint32_t desiredState, uint32_t timeOutMsec)
{
    Msg_t* pMsg;

    pMsg = (Msg_t *)pKrnlMsg->pMsg;

    if (pMsg->hdr.b.status == desiredState)
    {
        return TRUE;
    }

    return Kernel_WaitSyncObj((KernelSyncObj_t *)pKrnlMsg, desiredState, timeOutMsec);
}

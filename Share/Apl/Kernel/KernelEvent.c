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
    check if event flags are set.\n

    @param[in]  taskId          Task identifier
    @param[in]  pEvent          Kernel event object pointer
    @param[in]  waitFlags       flag bits for waiting

    @return 0 if no matched events, else matched event bit flags.
**/
//-----------------------------------------------------------------------------
static KernelEventFlags_t Kernel_CheckEventFlags(KernelTaskId_t taskId, KernelEvent_t* pEvent, KernelEventFlags_t waitFlags)
{
    KernelEventFlags_t matchedEventFlags;

    matchedEventFlags = (pEvent->flags & waitFlags);

    if (matchedEventFlags!=0)
    {
        pEvent->flags ^= matchedEventFlags; // clear matched event flags
    }

    return(matchedEventFlags);
}

//-----------------------------------------------------------------------------
/**
    Initialize event flag object

    @param[in]  pEvent          Kernel event object pointer
    @param[in]  spinLockId      Kernel Spin lock Id
**/
//-----------------------------------------------------------------------------
void Kernel_InitEventFlags(KernelEvent_t* pEvent, uint32_t spinLockId)
{
    pEvent->flags=0;
    pEvent->syncObj.vptrCheck = (KernelSyncObjVptr_t)Kernel_CheckEventFlags;
    pEvent->syncObj.spinLockId = spinLockId;
}

//-----------------------------------------------------------------------------
/**
    Test event flags

    @param[in]  pEvent          Kernel event object pointer
    @param[in]  eventFlags      Event flag bits to set

    @return     matched event bits or flags
**/
//-----------------------------------------------------------------------------
KernelEventFlags_t Kernel_TestEventFlags(KernelEvent_t* pEvent, KernelEventFlags_t eventFlags)
{
    return (pEvent->flags) & eventFlags;
}

//-----------------------------------------------------------------------------
/**
    Set event flags in critical section

    @param[in]  pEvent          Kernel event object pointer
    @param[in]  eventFlags      Event flag bits to set
**/
//-----------------------------------------------------------------------------
void Kernel_SetEventFlags(KernelEvent_t* pEvent, KernelEventFlags_t eventFlags)
{
    CpuCs_t orgState;

    SYNC_OBJ_BEGIN_CS(orgState, pEvent);

    KERNEL_RESCHEDULE(TRUE);

    BIT_SET(pEvent->flags, eventFlags);

    SYNC_OBJ_END_CS(orgState, pEvent);
}

//-----------------------------------------------------------------------------
/**
    Clear event flags in critical section

    @param[in]  pEvent          Kernel event object pointer
    @param[in]  eventFlags      event bits to clear
**/
//-----------------------------------------------------------------------------
void Kernel_ClearEventFlags(KernelEvent_t* pEvent, KernelEventFlags_t eventFlags)
{
    CpuCs_t orgState;

    SYNC_OBJ_BEGIN_CS(orgState, pEvent);

    BIT_CLR(pEvent->flags, eventFlags);

    SYNC_OBJ_END_CS(orgState, pEvent);
}

//-----------------------------------------------------------------------------
/**
    Wait for any event flags set\n

    @param[in]  pEvent          Kernel event object pointer
    @param[in]  eventFlags      event flags to wait
    @param[in]  timeOutMsec     timeout in milli-second unit

    @return     zero if timed out, else matched event bit flags.

    @remark Task scheduling is always involved regardless of event state.
**/
//-----------------------------------------------------------------------------
KernelEventFlags_t Kernel_WaitEventFlags(KernelEvent_t* pEvent, KernelEventFlags_t eventFlags, uint32_t timeOutMsec)
{
    return (KernelEventFlags_t) Kernel_WaitSyncObj((KernelSyncObj_t *)pEvent, eventFlags, timeOutMsec);
}

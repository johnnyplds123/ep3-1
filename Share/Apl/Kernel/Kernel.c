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
//! @brief  Very tiny preemptive kernel codes.
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
/// @name Kernel compile options
/// @{
#define CO_KRNL_TIME_SHARING    DISABLE     ///< time sharing scheduling between same priority task
#define CO_KRNL_CHECK_STACK     ENABLE      ///< check stack overflow in case of task switching
/// @}

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

/// @name Kernel task state types
/// @{
typedef enum
{
    cKernelTaskStateActive    = 'A',     ///< active(running) state
    cKernelTaskStateReady     = 'R',     ///< ready state
    cKernelTaskStatePreempted = 'P',     ///< ready but preempted
    cKernelTaskStateYileded   = 'Y',     ///< ready but yielded
    cKernelTaskStateTimeout   = 'T',     ///< ready from timeout wakeup
    cKernelTaskStateCreated   = 'C',     ///< created state
    cKernelTaskStateWait      = 'W'      ///< wait state
} KernelState_t;
/// @}

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define GET_TCB_PTR_FROM_TASK_ID(id)    ((KernelTcb_t *)(id))
#define GET_TASK_ID_FROM_TCB(tcb)       ((KernelTaskId_t)(tcb))

#if (KERNEL_TICK_PER_SEC<=1000)
#define KERNEL_MSEC_PER_TICK          (1000/KERNEL_TICK_PER_SEC)
#define KERNEL_TICK_TO_MSEC(TICK)     ((TICK)*KERNEL_MSEC_PER_TICK)
#define KERNEL_MSEC_TO_TICK(MSEC)     ((MSEC)/KERNEL_MSEC_PER_TICK)
#define KERNEL_TICK_TO_SEC(TICK)      ((TICK)/KERNEL_TICK_PER_SEC)
#define KERNEL_SEC_TO_TICK(SEC)       ((SEC)*KERNEL_TICK_PER_SEC)
#else
#define KERNEL_TICK_PER_MSEC          (KERNEL_TICK_PER_SEC/1000)
#define KERNEL_TICK_TO_MSEC(TICK)     ((TICK)/KERNEL_TICK_PER_MSEC)
#define KERNEL_MSEC_TO_TICK(MSEC)     ((MSEC)*KERNEL_TICK_PER_MSEC)
#define KERNEL_TICK_TO_SEC(TICK)      ((TICK)/KERNEL_TICK_PER_SEC)
#define KERNEL_SEC_TO_TICK(SEC)       ((SEC)*KERNEL_TICK_PER_SEC)
#endif

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

/// @brief Task list
typedef struct
{
    LinkHeadTail_t list;   ///< list head and tail pointer
    uint32_t       count;  ///< number of entries in list
} KernelTaskList_t;

/// @brief ARM register context
typedef struct
{
    uint32_t  cpsr;      ///< current program status register
    uint32_t  pc;        ///< program counter (r15), return address
    uint32_t  r0;        ///< register r0
    uint32_t  r1;        ///< register r1
    uint32_t  r2_12[11]; ///< register r2 .. r12
    uint32_t  sp;        ///< stack pointer (r13)
    uint32_t  lr;        ///< link register (r14)
} KernelContext_t;

/// @brief Task Control block
typedef struct
{
    DlinkEntry_t     dlink;            ///< doubly linked list
    KernelSyncObj_t* pSyncObj;         ///< Synchronization object pointer
    uint32_t         syncValue;        ///< synchronization value
    KernelContext_t  context;          ///< register context
    uint32_t         timerCountTick;   ///< timer count
    uint8_t          timerActive;      ///< timer is activated
    uint8_t          state;            ///< task state - ready, wait
    uint8_t          priority;         ///< task priority, lower value is higher priority
    uint8_t          reserved;         ///< reserved
    Cstr_t           pName;            ///< task Name
    uint8_t*         pStack;           ///< base stack pointer
    uint32_t         stackSize;        ///< stack byte size
} KernelTcb_t;

/// @brief Kernel variables
typedef struct
{
    bool            reschedule;     ///< Task resheduling is needed.
    bool            started;        ///< Kernel initialized and started
    KernelTimerTick_t timeTick;     ///< time tick
    KernelTaskFunc_t timerCallBack; ///< User timer call back function
    KernelTcb_t*      pCurrentTcb;  ///< current TCB pointer
    KernelTaskList_t taskList;      ///< task list
} KernelVar_t;

#define TCB_REG_OFFSET          __cpp(offsetof(KernelTcb_t, context))
#define TCB_REG_PC_OFFSET       __cpp(offsetof(KernelContext_t, pc))
#define TCB_REG_RO_OFFSET       __cpp(offsetof(KernelContext_t, r0))
#define TCB_REG_R1_OFFSET       __cpp(offsetof(KernelContext_t, r1))
#define TCB_STATE_OFFSET        __cpp(offsetof(KernelTcb_t, state))

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void AddToTaskList(DlinkEntry_t* pEntry, KernelTcb_t* pAddedTcb, uint32_t priority);

void AsmScheduling(void);
uint32_t  SuspendInSysMode(KernelContext_t* pContext);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
KernelVar_t sKrnlVar;

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
    Get Current CPU's kernel variable pointer
**/
//-----------------------------------------------------------------------------
__inline KernelVar_t* GetCurrentKrnlVar(void)
{
    return &sKrnlVar;
}

//-----------------------------------------------------------------------------
/**
    Kernel one time initialization.\n
      - initialize kernel global variables.\n
      - initialize task list.\n

    @param[in]  initMode        initialization mode
**/
//-----------------------------------------------------------------------------
Error_t Kernel_OneTimeInit(InitBootMode_t initMode)
{
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    pCurrKrnl->pCurrentTcb = NULL;
    pCurrKrnl->started = FALSE;
    pCurrKrnl->timeTick = 0;
    pCurrKrnl->timerCallBack = NULL;

    DlInit(&pCurrKrnl->taskList.list);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Start kernel scheduling.\n
      - set global gKrnlStarted and jump to scheduler \n
**/
//-----------------------------------------------------------------------------
void Kernel_Start(void)
{
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    pCurrKrnl->started = TRUE;

    // never return to here
    AsmScheduling();
}

//-----------------------------------------------------------------------------
/**
    Add TCB to Task list.\n
      - Task is sorted by priority level. (higher to lower)\n

    @param[in]  pEntry          List entry pointer
    @param[in]  pAddedTcb       Added TCB pointer
    @param[in]  priority        priority level

    @return  none
**/
//-----------------------------------------------------------------------------
static void AddToTaskList(DlinkEntry_t* pEntry, KernelTcb_t* pAddedTcb, uint32_t priority)
{
    KernelTcb_t* pTcb;
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    DL_FOR_EACH_FROM(pEntry, &pCurrKrnl->taskList.list)
    {
        pTcb = STR_PTR_FROM_MEMBER(pEntry, KernelTcb_t, dlink);

        if (pTcb->priority>=priority)
        {
            DlAddToPrev(&pAddedTcb->dlink, &pTcb->dlink);
            return;
        }
    }

    DlAddToTail(&pCurrKrnl->taskList.list, &pAddedTcb->dlink);
}

//-----------------------------------------------------------------------------
/**
    Register Task - Add TCB to task list for future execution.\n

    @param[in]  pTcb       TCB(Task control block) pointer

    @return     assigned task id
**/
//-----------------------------------------------------------------------------
static KernelTaskId_t Kernel_RegisterTask(KernelTcb_t* pTcb)
{
    CpuCs_t orgState;
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    BEGIN_CS(orgState);
    AddToTaskList(pCurrKrnl->taskList.list.head, pTcb, pTcb->priority+1);
    pCurrKrnl->taskList.count++;
    END_CS(orgState);

    return GET_TASK_ID_FROM_TCB(pTcb);
}

//-----------------------------------------------------------------------------
/**
    Create Task - Initialize the task's various properties.\n

    @param[in]  taskFunc       task function address to be excuted.
    @param[in]  stackPointer   stack pointer
    @param[in]  stackSize      stack size
    @param[in]  priority       task priority
    @param[in]  taskName       name of task

    @return     Created task id
**/
//-----------------------------------------------------------------------------
KernelTaskId_t Kernel_CreateTask(KernelTaskFunc_t taskFunc, void* stackPointer, uint32_t stackSize, uint8_t priority, Cstr_t taskName)
{
    KernelTcb_t *pTcb;

    pTcb = (KernelTcb_t *)stackPointer;

    MEM_CLR(stackPointer, stackSize);

    // Initialize CPU register context
    pTcb->context.sp   = (uint32_t)stackPointer+stackSize-8;
    pTcb->context.cpsr = ((uint32_t)taskFunc & 1) ? MODE_SYS|MODE_THUMB : MODE_SYS;
    pTcb->context.pc   = (uint32_t)taskFunc;

    // Initialize other TCB
    pTcb->priority   = priority;
    pTcb->timerActive= FALSE;
    pTcb->state      = cKernelTaskStateCreated;

    pTcb->syncValue = 0;
    pTcb->pSyncObj  = NULL;

    pTcb->pName = taskName;

    pTcb->pStack     = stackPointer;
    pTcb->stackSize  = stackSize;

    return Kernel_RegisterTask(pTcb);

}

//-----------------------------------------------------------------------------
/**
    Get current running task id

    @return     task id
**/
//-----------------------------------------------------------------------------
KernelTaskId_t Kernel_GetCurrentTaskId(void)
{
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    return GET_TASK_ID_FROM_TCB(pCurrKrnl->pCurrentTcb);
}

//-----------------------------------------------------------------------------
/**
    Wait for synchronization object ready.\n

    @param[in]  pSyncObj       Kernel synchronization object pointer
    @param[in]  syncValue      synchronization value (object specific value)
    @param[in]  timeOutMsec    timeout in milli-second unit

    @return     zero for timed out, else object specific value.

    @remark     Task scheduling is always involved regardless of sync object's state.
**/
//-----------------------------------------------------------------------------
uint32_t Kernel_WaitSyncObj(KernelSyncObj_t* pSyncObj, uint32_t syncValue, uint32_t timeOutMsec)
{
    CpuCs_t orgState;
    uint32_t retValue;
    KernelTcb_t* pTcb;
    DlinkEntry_t* pEntry;
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    // return always zero if Kernel is not started.
    if (!pCurrKrnl->started) return(0);

    BEGIN_CS(orgState);

    pTcb = pCurrKrnl->pCurrentTcb;

    if (timeOutMsec!=KERNEL_NO_TIMEOUT)
    {
        pTcb->timerActive = TRUE;
        pTcb->timerCountTick = KERNEL_MSEC_TO_TICK(timeOutMsec);
    }

    pTcb->pSyncObj  = pSyncObj;
    pTcb->syncValue = syncValue;
    pTcb->state     = cKernelTaskStateWait;

    pEntry = pTcb->dlink.next;

    DlRemove(&pTcb->dlink);
    AddToTaskList(pEntry, pTcb, pTcb->priority+1);

    retValue = SuspendInSysMode(&pTcb->context);

    END_CS(orgState);

    return retValue;
}


//-----------------------------------------------------------------------------
/**
    Wait for object by polling.\n

    @param[in]  testCallBack   sync object test call back function
    @param[in]  testParam      call back function parameter
    @param[in]  timeOutUsec    timeout in micro-second unit
    @param[in]  yield          task switching with Yield method if true, else sleep

    @return     zero for timed out, else test call back specific value.

    @remark
**/
//-----------------------------------------------------------------------------
uint32_t Kernel_WaitPollObj(KernelPollFunc_t testCallBack, uint32_t testParam, uint32_t yield, uint32_t timeOutUsec)
{
    uint32_t result;
    HwTimerTick_t tick;

    tick = HalTimer_ReadTick();

    while ((result = testCallBack(testParam))==KERNEL_SYNC_OBJ_NOT_READY)
    {
        if (HalTimer_GetElapsedTimeUsec(tick)>timeOutUsec)
        {
            return (KERNEL_WAIT_TIMEDOUT);
        }

        if (yield)
        {
            Kernel_Yield();
        }
        else
        {
            Kernel_SleepMsec(1);
        }
    }

    return(result);
}

//-----------------------------------------------------------------------------
/**
    Yield to higher or equal priority task.\n
**/
//-----------------------------------------------------------------------------
void Kernel_Yield(void)
{
    CpuCs_t orgState;
    DlinkEntry_t* pEntry;
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    // return always zero if Kernel is not started.
    if (!pCurrKrnl->started) return;

    BEGIN_CS(orgState);

    pCurrKrnl->pCurrentTcb->state = cKernelTaskStateYileded;

    pEntry = pCurrKrnl->pCurrentTcb->dlink.next;
    DlRemove(&pCurrKrnl->pCurrentTcb->dlink);
    AddToTaskList(pEntry, pCurrKrnl->pCurrentTcb, pCurrKrnl->pCurrentTcb->priority+1);

    SuspendInSysMode(&pCurrKrnl->pCurrentTcb->context);

    END_CS(orgState);
}

//-----------------------------------------------------------------------------
/**
    Sync object state is changed, so rescheduling is needed.
**/
//-----------------------------------------------------------------------------
void Kernel_StateChanged(void)
{
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    pCurrKrnl->reschedule = TRUE;
}

//-----------------------------------------------------------------------------
/**
    Set periodic timer call back function.
    the registered function was called by Kernel every periodic timer interrupt.

    @param[in] userCallBackFunction  timer call back function pointer

    @return previous call back function
**/
//-----------------------------------------------------------------------------
KernelTaskFunc_t Kernel_SetTimerCallBack(KernelTaskFunc_t userCallBackFunction)
{
    KernelTaskFunc_t prevCallBack;
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    prevCallBack = pCurrKrnl->timerCallBack;
    pCurrKrnl->timerCallBack = userCallBackFunction;

    return prevCallBack;
}


//-----------------------------------------------------------------------------
/**
    Check if task is ready or not.\n

    @param[in]  pTcb   task pointer

    @return     task readiness = TRUE(1) or FALSE(0)
**/
//-----------------------------------------------------------------------------
static bool Kernel_CheckTaskReady(KernelTcb_t* pTcb)
{
    uint32_t retValue;

    if (pTcb->state != cKernelTaskStateWait) return TRUE;
    if (pTcb->pSyncObj==NULL) return FALSE;

    // call Sync Object call back function to check whether task is ready or not

    retValue = pTcb->pSyncObj->vptrCheck(pTcb, pTcb->pSyncObj, pTcb->syncValue);

    if (retValue!=KERNEL_SYNC_OBJ_NOT_READY)
    {
        pTcb->context.r0 = retValue;            // return value of Kernel_WaitSyncObj(...)
        pTcb->timerActive = 0;                  // task is ready, so stop timer
        pTcb->state = cKernelTaskStateReady;
    }

    return(retValue);

}

//-----------------------------------------------------------------------------
/**
    Scan task list to find ready task\n

    return  ready TCB pointer which has the highest priority.\n
**/
//-----------------------------------------------------------------------------
static KernelTcb_t* Kernel_ScanTaskList(void)
{
    KernelTcb_t* pTcb;
    DlinkEntry_t* pEntry;
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    // scan all TCB in task list
    DL_FOR_EACH(pEntry, &pCurrKrnl->taskList.list)
    {
        pTcb = STR_PTR_FROM_MEMBER(pEntry, KernelTcb_t, dlink);

        if (Kernel_CheckTaskReady(pTcb)) return pTcb;
    }

    return NULL;
}

//-----------------------------------------------------------------------------
/**
    Select next running task.\n
**/
//-----------------------------------------------------------------------------
void Kernel_Scheduling(void)
{
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    pCurrKrnl->pCurrentTcb = Kernel_ScanTaskList();

    ASSERT(pCurrKrnl->pCurrentTcb != NULL);
}

//-----------------------------------------------------------------------------
/**
    Check stack overflow

    @param[in]  pTcb   task pointer
    @param[in]  sp     stack pointer
**/
//-----------------------------------------------------------------------------
static void Kernel_CheckStackOverflow(KernelTcb_t* pTcb, uint32_t sp)
{
    if (sp>(uint32_t)pTcb->pStack + sizeof(KernelTcb_t) + 32) return;

    E_PRINTF(cEcOutOfMemory, "STACK OVERFLOW !, SP=%X, BASE=%X\n", sp, (uint32_t)pTcb->pStack + sizeof(KernelTcb_t));
    Kernel_DumpTcb(pTcb);
}

//-----------------------------------------------------------------------------
/**
    Get current task context pointer,

    @return     context of the task
**/
//-----------------------------------------------------------------------------
void* Kernel_GetCurrentTaskContext(void)
{
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    // check stack pointer to catch stack overflow.
#if  CO_KRNL_CHECK_STACK
    Kernel_CheckStackOverflow(pCurrKrnl->pCurrentTcb, pCurrKrnl->pCurrentTcb->context.sp);
#endif

    pCurrKrnl->pCurrentTcb->state = cKernelTaskStateActive;

    //M_PRINTF(" >>TCB:%X %X\n",pCurrKrnl->pCurrentTcb, pCurrKrnl->pCurrentTcb->context.pc);

    return (&pCurrKrnl->pCurrentTcb->context);
}

//-----------------------------------------------------------------------------
/**
    Check the need for preemption.\n

    @return     preempted task's context pointer, else NULL
**/
//-----------------------------------------------------------------------------
void* Kernel_CheckPreemption(void)
{
    KernelTcb_t* pTcb;
    KernelContext_t* pContext;
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();
    CpuCs_t orgState;

    // return NULL if kernel is not started
    if (!pCurrKrnl->started) return NULL;

    // return NULL if any kernel sync objects are not changed.
    if (!pCurrKrnl->reschedule) return NULL;

    pTcb = Kernel_ScanTaskList();

#if CO_KRNL_TIME_SHARING==ENABLE
    if (pTcb==pCurrKrnl->pCurrentTcb) return NULL;
#else
    if (pTcb->priority>=pCurrKrnl->pCurrentTcb->priority) return NULL;

    BEGIN_CS(orgState);

    pCurrKrnl->pCurrentTcb->state = cKernelTaskStatePreempted;
    DlRemove(&pCurrKrnl->pCurrentTcb->dlink);
    AddToTaskList(pCurrKrnl->taskList.list.head, pCurrKrnl->pCurrentTcb, pCurrKrnl->pCurrentTcb->priority);

    END_CS(orgState);
#endif

    pContext = &pCurrKrnl->pCurrentTcb->context;
    pCurrKrnl->pCurrentTcb = pTcb;

    return pContext;
}

//-----------------------------------------------------------------------------
/**
    Wake up task upon time out
**/
//-----------------------------------------------------------------------------
static void Kernel_WakeupTask(void)
{
    KernelTcb_t* pTcb;
    DlinkEntry_t* pEntry;
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    DL_FOR_EACH(pEntry, &pCurrKrnl->taskList.list)
    {
        pTcb = STR_PTR_FROM_MEMBER(pEntry, KernelTcb_t, dlink);

        if (pTcb->timerActive)
        {
            if (pTcb->timerCountTick<=1)
            {
                pTcb->timerActive = 0;
                // zero return value for timed out task
                if (pTcb->state == cKernelTaskStateWait) pTcb->context.r0 = 0;
                pTcb->state = cKernelTaskStateTimeout;
                KERNEL_RESCHEDULE(TRUE);
            }
            else
            {
                pTcb->timerCountTick--;
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Kernel periodic timer interrupt handler.\n
      Increase Kernel time tick.\n
      Wake up timed out Task.\n
      Call user timer callback function.\n

    @param[in]  interrupt call back parameter
**/
//-----------------------------------------------------------------------------
void Kernel_TimerIsr(uint32_t param)
{
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    pCurrKrnl->timeTick++;

    if (pCurrKrnl->timerCallBack!=NULL)
    {
        pCurrKrnl->timerCallBack();
    }

    Kernel_WakeupTask();

#if CO_KRNL_TIME_SHARING==ENABLE
    KERNEL_RESCHEDULE(TRUE);
    DlRemove(&pCurrKrnl->pCurrentTcb->dlink);
    AddToTaskList(pCurrKrnl->taskList.list.head, pCurrKrnl->pCurrentTcb, pCurrKrnl->pCurrentTcb->priority+1);
    pCurrKrnl->pCurrentTcb->state = cKernelTaskStateYileded;
#endif
}

//-----------------------------------------------------------------------------
/**
    Read current kernel software time tick.\n

    @return     kernel time tick value
**/
//-----------------------------------------------------------------------------
KernelTimerTick_t Kernel_ReadTick(void)
{
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    return pCurrKrnl->timeTick;
}

//-----------------------------------------------------------------------------
/**
    Compute elapsed time in milli-second unit by using Kernel SW time tick.\n

    @param[in]  lastTick    kernel timer tick value before this function is called.

    @return    epalsed time in milli-second unit
**/
//-----------------------------------------------------------------------------
uint64_t Kernel_GetElapsedTimeMsec(KernelTimerTick_t lastTick)
{
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    return KERNEL_TICK_TO_MSEC((uint64_t)pCurrKrnl->timeTick-lastTick);
}

//-----------------------------------------------------------------------------
/**
    Set kernel time in second unit

    @param[in]  second    time to set in second unit
**/
//-----------------------------------------------------------------------------
void  Kernel_SetTimeSec(uint32_t second)
{
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    pCurrKrnl->timeTick = KERNEL_SEC_TO_TICK(second);
}

//-----------------------------------------------------------------------------
/**
    Get kernel time in second unit

    @return    time in second unit
**/
//-----------------------------------------------------------------------------
uint32_t  Kernel_GetTimeSec(void)
{
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    return KERNEL_TICK_TO_SEC(pCurrKrnl->timeTick);
}

//-----------------------------------------------------------------------------
/**
    Get kernel time in mill-second unit

    @return    time in mill-second unit
**/
//-----------------------------------------------------------------------------
uint64_t  Kernel_GetTimeMsec(void)
{
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    return KERNEL_TICK_TO_MSEC((uint64_t)pCurrKrnl->timeTick);
}


//-----------------------------------------------------------------------------
/**
    Dump TCB information to serial monitor.

    @param[in]  pTcb        TCB pointer to dump

**/
//-----------------------------------------------------------------------------
void Kernel_DumpTcb(KernelTaskId_t taskId)
{
    KernelTcb_t* pTcb;

    pTcb = GET_TCB_PTR_FROM_TASK_ID(taskId);

    // tcb ptr, name, sync value, priority, state, PC, SP, stack base
    M_PRINTF("TCB:%X %8s SV:%X ST:%c PR:%1X PC=%X SP=%X,%X\n", pTcb, pTcb->pName,
              pTcb->syncValue, pTcb->state, pTcb->priority, pTcb->context.pc, pTcb->context.sp,pTcb->pStack);
}

//-----------------------------------------------------------------------------
/**
    Dump Kernel Information.\n
      - Dump all TCB information.\n
**/
//-----------------------------------------------------------------------------
void Kernel_DumpInfo(void)
{
    KernelVar_t* pCurrKrnl = GetCurrentKrnlVar();

    M_PRINTF("\n[TASK][CPU%1X][TICK:%X]\n", HalCpu_GetId(), Kernel_ReadTick());

    DlForEachCallBack(&pCurrKrnl->taskList.list, (LlCallBack_t)Kernel_DumpTcb, offsetof(KernelTcb_t, dlink));
}

//-----------------------------------------------------------------------------
/**
    Task scheduling, resume selected task (pKrnlNextTcb)
**/
//-----------------------------------------------------------------------------
__asm void AsmScheduling(void)
{
    PRESERVE8

    MSR     CPSR_c, #MODE_IRQ:OR:I_BIT:OR:F_BIT // enter irq mode for scheduling

    IMPORT  Kernel_Scheduling
    BLX     Kernel_Scheduling

ResumeNextTask
    IMPORT  Kernel_GetCurrentTaskContext
    BLX     Kernel_GetCurrentTaskContext

    LDMIA   R0!, {R12, R14}                     // Get "READY" task's CPSR and return address in SPSR_irq and lr_irq.

    ANDS    R1, R14, #1                         // Returning from Thumb mode?
    ORRNE   R12, R12, #MODE_THUMB               // Set Thumb mode bit for returning CPSR
    MSR     SPSR_fsxc, R12                      //
    LDMIA   R0, {R0-R14}^                       // Load ready task's r0 - r14
    NOP                                         // Cannot use banked registers immediately after user mode LDM.

    MOVS    PC, R14                             // Return to address in r14_irq.
}

//-----------------------------------------------------------------------------
/**
    Suspend and reschedule task in system mode\n
     - Save context\n
     - Rescheduling task.\n

    @param[in]  pContext    CPU register context pointer

    @return     zero for timed out, else object specific value.

    @remark     IRQ must be disabled.
**/
//-----------------------------------------------------------------------------
__asm uint32_t SuspendInSysMode(KernelContext_t* pContext)
{
    PRESERVE8

    STMFD   SP!, {R0-R1}

    MRS     R1, CPSR
    STR     R1,  [R0], #TCB_REG_RO_OFFSET   // save CPSR

    LDMFD   SP!, {R1}                       //
    STMIA   R0!, {R1}                       // save R0

    LDMFD   SP!, {R1}                       //
    STR     R14, [R0, #TCB_REG_PC_OFFSET - TCB_REG_R1_OFFSET]   // save PC
    STMIA   R0, {R1-R14}

    B       AsmScheduling
}

//-----------------------------------------------------------------------------
/**
    Kernel Interrupt service routine\n
     - Call IRQ handler\n
     - Reschedule task if event state has changed\n
**/
//-----------------------------------------------------------------------------
__asm void  Kernel_IrqEntry(void)
{
    PRESERVE8

    SUB     LR,LR,#4                        // return address

    STMFD   SP!, {R0-R3, R12, LR}           // save the registers that may be corrupted by called function.

    IMPORT  HalIrq_Handler
    BLX     HalIrq_Handler                  // Branch to the real IRQ handler.
    CMP     R0,#0
    LDMFDEQ SP!, {R0-R3, R12, PC}^          // return to interrupted code

CheckPreemption
    IMPORT  Kernel_CheckPreemption          //
    BLX     Kernel_CheckPreemption
    CMP     R0,#0
    LDMFDEQ SP!, {R0-R3, R12, PC}^          // return to interrupted code

SuspendTask
    MRS     R12, SPSR                       //
    STR     R12, [R0], #TCB_REG_RO_OFFSET   // save PSR and move pointer to pCurrentTcb->context.r0

    LDMFD   SP!, {R2, R3}                   //
    STMIA   R0!, {R2, R3}                   // save R0 and R1

    LDMFD   SP!, {R2, R3, R12, LR}          //
    STR     LR, [R0, #-3*4]                 // save return address

    STMIA   R0, {R2-R14}^                   // save {R2..R14}

    B       ResumeNextTask                  // jump to scheduler to select a task to run
}

//-----------------------------------------------------------------------------
/**
    Kernel Fast Interrupt service routine\n
     - Call FIQ handler\n
**/
//-----------------------------------------------------------------------------
__asm void  Kernel_FiqEntry(void)
{
    PRESERVE8

    SUB     LR,LR,#4                        // return address

    STMFD   SP!, {R0-R3, R12, LR}           // save the registers that may be corrupted by called function.

    IMPORT  HalFiq_Handler
    BLX     HalFiq_Handler                  // call FIQ handler.

    MRS     r0,SPSR
    AND     r0,r0,#MODE_MASK
    CMP     r0,#MODE_SYS
    BEQ     CheckPreemption                 // task switching if interrupted mode is SYS

    LDMFD   SP!, {R0-R3, R12, PC}^
}

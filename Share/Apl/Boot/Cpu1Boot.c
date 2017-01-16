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
//! @brief Power on boot, interrupt and exception handler for CPU1
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"


#include "PerfSection.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define STACK_FIQ_SIZE  16      ///< Shall be multiple of 4
#define STACK_IRQ_SIZE  2048    ///< Shall be multiple of 4
#define STACK_ABT_SIZE  16      ///< Shall be multiple of 4
#define STACK_UDF_SIZE  16      ///< Shall be multiple of 4
#define STACK_SVC_SIZE  16      ///< Shall be multiple of 4
#define STACK_SYS_SIZE  1024    ///< Shall be multiple of 4

#define STACK_MARGIN    4       ///<

#define STACK_FIQ_SIG   0x77    ///< Stack signature for FIQ
#define STACK_IRQ_SIG   0x88    ///< Stack signature for IRQ
#define STACK_ABT_SIG   0x99    ///< Stack signature for SUPERVISOR
#define STACK_UDF_SIG   0xAA    ///< Stack signature for ABORT
#define STACK_SVC_SIG   0xBB    ///< Stack signature for UNDEFINED
#define STACK_SYS_SIG   0xCC    ///< Stack signature for SYSTEM
#define STACK_ARM_SIG   0xDD    ///< Stack signature for ARM Lib

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
__DATA_PERF_UI __align(16) uint8_t FIQ_Stack1[STACK_FIQ_SIZE];
__DATA_PERF_UI __align(16) uint8_t IRQ_Stack1[STACK_IRQ_SIZE];
__DATA_PERF_UI __align(16) uint8_t ABT_Stack1[STACK_ABT_SIZE];
__DATA_PERF_UI __align(16) uint8_t UDF_Stack1[STACK_UDF_SIZE];
__DATA_PERF_UI __align(16) uint8_t SVC_Stack1[STACK_SVC_SIZE];
__DATA_PERF_UI __align(16) uint8_t SYS_Stack1[STACK_SYS_SIZE];

__DATA_PERF_UI AllocatorAo_t itcmAllocatorAo;
__DATA_PERF_UI AllocatorAo_t dtcmAllocatorAo;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
#pragma push
#pragma arm section code   = "code_vector1"
//-----------------------------------------------------------------------------
/**
    ARM vector table (Power on reset, Undefined instruction and so on)
    This code must be located at address 0.
**/
//-----------------------------------------------------------------------------
 __asm void VectorTable1(void)
{
    PRESERVE8                           // Perserve 8 byte alignment on the stacks
    ARM                                 // Start in ARM mode

    LDR     pc,=PowerOnResetEntry1
    LDR     pc,=UndefInstEntry1
    LDR     pc,=SwiEntry1
    LDR     pc,=PrefetchAbortEntry1
    LDR     pc,=DataAbortEntry1
    NOP
#if _SHARE_==SHARE_RTOS
    IMPORT  Kernel_IrqEntry               // Kernel IRQ
    LDR     pc,=Kernel_IrqEntry
    IMPORT  Kernel_FiqEntry               // Kernel FIQ
    LDR     pc,=Kernel_FiqEntry
#else
    IMPORT  IrqEntry1                   // IRQ
    LDR     pc,=IrqEntry1
    IMPORT  FiqEntry1
    LDR     pc,=FiqEntry1               // FIQ
#endif
}
#pragma pop

//-----------------------------------------------------------------------------
/**
    Power on reset entry point.
     - disable all interrupt and setup stack pointer.
     - pre one time initialization for core components.
**/
//-----------------------------------------------------------------------------
__asm void PowerOnResetEntry1(void)
{
    PRESERVE8

    IMPORT  FIQ_Stack1
    IMPORT  IRQ_Stack1
    IMPORT  ABT_Stack1
    IMPORT  UDF_Stack1
    IMPORT  SVC_Stack1
    IMPORT  SYS_Stack1

    // --- Initialize stack pointer registers
    // Enter FIQ mode and set up the stack pointer
    MSR     CPSR_c, #MODE_FIQ:OR:I_BIT:OR:F_BIT
    LDR     sp,=FIQ_Stack1+STACK_FIQ_SIZE-STACK_MARGIN

    // Enter IRQ mode and set up the stack pointer
    MSR     CPSR_c, #MODE_IRQ:OR:I_BIT:OR:F_BIT
    LDR     sp,=IRQ_Stack1+STACK_IRQ_SIZE-STACK_MARGIN

    // Enter prefetch and data Abort mode and set up the stack pointer
    MSR     CPSR_c, #MODE_ABT:OR:I_BIT:OR:F_BIT
    LDR     sp,=ABT_Stack1+STACK_ABT_SIZE-STACK_MARGIN

    // Enter Undefined mode and set up the stack pointer
    MSR     CPSR_c, #MODE_UDF:OR:I_BIT:OR:F_BIT
    LDR     sp,=UDF_Stack1+STACK_UDF_SIZE-STACK_MARGIN

    // Enter IRQ mode and set up the stack pointer
    MSR     CPSR_c, #MODE_SVC:OR:I_BIT:OR:F_BIT
    LDR     sp,=SVC_Stack1+STACK_SVC_SIZE-STACK_MARGIN

    // Enter System mode and set up the stack pointer
    MSR     CPSR_c, #MODE_SYS:OR:I_BIT:OR:F_BIT
    LDR     sp,=SYS_Stack1+STACK_SYS_SIZE-STACK_MARGIN

    // Setup system configuration register
    MRC     p15, 0, r0, c1, c0, 0
    AND     r0, r0, #0xFFFFDFFF     // use RAM vector
    ORR     r0, r0, #0x400          // enable SWP instruction
    MCR     p15, 0, r0, c1, c0, 0

    // Fill pattern data to stack for easy debugging
    IMPORT  FillStack1
    BL      FillStack1

    // Wait for HW and FW load done.
    IMPORT  WaitForReadyToRun
    BL      WaitForReadyToRun

    // Pre initialize Hardware before call main
    IMPORT  PreOneTimeInit1
    BL      PreOneTimeInit1

    IMPORT  __main
    B       __main
}

//-----------------------------------------------------------------------------
/**
    Undefined instruction excpetion handler
**/
//-----------------------------------------------------------------------------
__asm void UndefInstEntry1(void)
{
    SUBS    lr, lr, #4
    PUSH    {r0-r3,r12,lr}

    MOV     r0,lr                           ; exception address
    MOV     r1,sp                           ; stack pointer
    MOV     r2,#0                           ;

    IMPORT  Debug_UndefInst
    BLX     Debug_UndefInst

    POP     {r0-r3,r12,pc}^
}

//-----------------------------------------------------------------------------
/**
    Software interrupt(SWI) handler
**/
//-----------------------------------------------------------------------------
__asm void SwiEntry1(void)
{
    PUSH    {r0-r3,r12,lr}

    MOV     r0,lr                           ; r0 = exception address
    MOV     r1,sp                           ; r1 = stack pointer

    MRS     r2,spsr                         ; test mode
    AND     r2,r2,#MODE_THUMB               ;
    CMP     r2,#0
    BEQ     %F0

    LDR     r2,[r0,#-2]                     ; r2 = SWI # for thumb mode
    BIC     r2,r2,#0xFFFFFF00               ;
    B       %F1
0
    LDR     r2,[r2,#-4]                     ; r2 = SWI # for arm mode
    BIC     r2,r2,#0xFF000000
1

    IMPORT  Debug_Swi
    BLX     Debug_Swi

    POP     {r0-r3,r12,pc}^
}

//-----------------------------------------------------------------------------
/**
    Prefetch abort exception handler
**/
//-----------------------------------------------------------------------------
__asm void PrefetchAbortEntry1(void)
{
    SUBS    lr, lr, #4
    PUSH    {r0-r3,r12,lr}

    MOV     r0,lr                           ; r0 = exception address
    MOV     r1,sp                           ; r1 = stack pointer
    MOV     r2,#0                           ; r2 = not used

    IMPORT  Debug_PrefetchAbort
    BLX     Debug_PrefetchAbort

    POP     {r0-r3,r12,pc}^
}

//-----------------------------------------------------------------------------
/**
    Prefetch abort exception handler
**/
//-----------------------------------------------------------------------------
__asm void DataAbortEntry1(void)
{
    SUBS    lr, lr, #8
    PUSH    {r0-r3,r12,lr}

    MOV     r0,lr                           ; r0 = exception address
    MOV     r1,sp                           ; r1 = stack pointer
    MRC     p15,0,r2,c5,c0,0                ; r2 = data fault status

    IMPORT  Debug_DataAbort
    BLX     Debug_DataAbort

    POP     {r0-r3,r12,pc}^
}

//-----------------------------------------------------------------------------
/**
    Interrupt request(IRQ) handler (NOT USED)
**/
//-----------------------------------------------------------------------------
__irq void IrqEntry1(void)
{
    HalIrq_Handler();
}

//-----------------------------------------------------------------------------
/**
    Fast Interrupt request(FIQ) handler (NOT USED)
**/
//-----------------------------------------------------------------------------
__irq void FiqEntry1(void)
{
    HalFiq_Handler();
}

//-----------------------------------------------------------------------------
/**
    Fill stack with specified signature to catch stack overflow easily.
**/
//-----------------------------------------------------------------------------
void FillStack1(void)
{
    extern unsigned int Image$$ARM_LIB_STACKHEAP$$ZI$$Base;

    MemFillU32(FIQ_Stack1, STACK_FIQ_SIG, sizeof(FIQ_Stack1));
    MemFillU32(IRQ_Stack1, STACK_IRQ_SIG, sizeof(IRQ_Stack1));
    MemFillU32(ABT_Stack1, STACK_ABT_SIG, sizeof(ABT_Stack1));
    MemFillU32(UDF_Stack1, STACK_UDF_SIG, sizeof(UDF_Stack1));
    MemFillU32(SVC_Stack1, STACK_SVC_SIG, sizeof(SVC_Stack1));
    MemFillU32(SYS_Stack1, STACK_SYS_SIG, sizeof(SYS_Stack1)-128);
    MemFillU32(&Image$$ARM_LIB_STACKHEAP$$ZI$$Base, STACK_ARM_SIG, ARM_LIB_STACKHEAP_SIZE);
}

//-----------------------------------------------------------------------------
/**
    Wait for ready to run firmware.
      - shared core HW is ready to use.
      - full fimrware is loaded.
**/
//-----------------------------------------------------------------------------
void WaitForReadyToRun(void)
{
    uint16_t cmd;
    uint16_t wParam;
    uint32_t lParam;

    do
    {
        HalIpc_ReadCmdFrom(CPU_ID0, &cmd, &wParam, &lParam);
    } while (cmd!=cIpcCmdReadyToRun);
}

//-----------------------------------------------------------------------------
/**
    Pre Software and Hardware One time initialization before C main() is called.\n

    Initialize ITCM, DTCM allocation only allocator

**/
//-----------------------------------------------------------------------------
void PreOneTimeInit1(void)
{
    InitBootMode_t initMode;
    extern unsigned int Image$$ITCM_USED_LAST$$ZI$$Limit;
    extern unsigned int Image$$DTCM_USED_LAST$$ZI$$Limit;

    initMode = Hal_GetInitBootMode();

    Debug_OneTimeInit(initMode);

    // setup free memory space to use dynamic allocator
    MemAlloc_InitAo(&itcmAllocatorAo, &Image$$ITCM_USED_LAST$$ZI$$Limit, (void*)(CPU1_ITCM_ADDR+CPU1_ITCM_SIZE), CPU1_ITCM_SIZE);
    MemAlloc_InitAo(&dtcmAllocatorAo, &Image$$DTCM_USED_LAST$$ZI$$Limit, (void*)(CPU1_DTCM_ADDR+CPU1_DTCM_SIZE), CPU1_DTCM_SIZE);

    Hal_PreOneTimeInit(initMode);
}

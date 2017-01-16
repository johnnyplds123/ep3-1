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
//! @brief Power on boot, interrupt and exception handler for CPU0
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "HalPciePhy.h"
#include "HalPcieLink.h"

#include "PerfSection.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define STACK_FIQ_SIZE  512     ///< Shall be multiple of 4
#define STACK_IRQ_SIZE  1024    ///< Shall be multiple of 4
#define STACK_ABT_SIZE  16      ///< Shall be multiple of 4
#define STACK_UDF_SIZE  16      ///< Shall be multiple of 4
#define STACK_SVC_SIZE  16      ///< Shall be multiple of 4
#define STACK_SYS_SIZE  1024    ///< Shall be multiple of 4
#define STACK_MARGIN    4       ///<

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
typedef void    (*pVoidFuncVoid)(void);

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
__DATA_PERF_UI __align(16) uint8_t FIQ_Stack0[STACK_FIQ_SIZE];
__DATA_PERF_UI __align(16) uint8_t IRQ_Stack0[STACK_IRQ_SIZE];
__DATA_PERF_UI __align(16) uint8_t ABT_Stack0[STACK_ABT_SIZE];
__DATA_PERF_UI __align(16) uint8_t UDF_Stack0[STACK_UDF_SIZE];
__DATA_PERF_UI __align(16) uint8_t SVC_Stack0[STACK_SVC_SIZE];
__DATA_PERF_UI __align(16) uint8_t SYS_Stack0[STACK_SYS_SIZE];

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern void LoadFirmware(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

#pragma push
#pragma arm section code   = "code_vector0"
//-----------------------------------------------------------------------------
/**
    ARM vector table (Power on reset, Undefined instruction and so on)
    This code must be located at address 0.
**/
//-----------------------------------------------------------------------------
 __asm void VectorTable(void)
{
    PRESERVE8                           // Perserve 8 byte alignment on the stacks
    ARM                                 // Start in ARM mode

    LDR     pc,=PowerOnResetEntry       // power on reset
    LDR     pc,=UndefInstEntry          // undefined instruction
    LDR     pc,=SwiEntry                // software interrupt
    LDR     pc,=PrefetchAbortEntry      // prefetch abort
    LDR     pc,=DataAbortEntry          // data abort
    NOP

    IMPORT  IrqEntry                    // IRQ
    LDR     pc,=IrqEntry
    IMPORT  FiqEntry
    LDR     pc,=FiqEntry                // FIQ
}
#pragma pop

//-----------------------------------------------------------------------------
/**
    Power on reset entry point.
     - disable all interrupt and setup stack pointer.
     - pre one time initialization for core components.
**/
//-----------------------------------------------------------------------------
__asm void PowerOnResetEntry(void)
{
    PRESERVE8

    IMPORT  FIQ_Stack0
    IMPORT  IRQ_Stack0
    IMPORT  ABT_Stack0
    IMPORT  UDF_Stack0
    IMPORT  SVC_Stack0
    IMPORT  SYS_Stack0

    // --- Initialize stack pointer registers
    // Enter FIQ mode and set up the stack pointer
    MSR     CPSR_c, #MODE_FIQ:OR:I_BIT:OR:F_BIT
    LDR     sp,=FIQ_Stack0+STACK_FIQ_SIZE-STACK_MARGIN

    // Enter IRQ mode and set up the stack pointer
    MSR     CPSR_c, #MODE_IRQ:OR:I_BIT:OR:F_BIT
    LDR     sp,=IRQ_Stack0+STACK_IRQ_SIZE-STACK_MARGIN

    // Enter prefetch and data Abort mode and set up the stack pointer
    MSR     CPSR_c, #MODE_ABT:OR:I_BIT:OR:F_BIT
    LDR     sp,=ABT_Stack0+STACK_ABT_SIZE-STACK_MARGIN

    // Enter Undefined mode and set up the stack pointer
    MSR     CPSR_c, #MODE_UDF:OR:I_BIT:OR:F_BIT
    LDR     sp,=UDF_Stack0+STACK_UDF_SIZE-STACK_MARGIN

    // Enter IRQ mode and set up the stack pointer
    MSR     CPSR_c, #MODE_SVC:OR:I_BIT:OR:F_BIT
    LDR     sp,=SVC_Stack0+STACK_SVC_SIZE-STACK_MARGIN

    // Enter System mode and set up the stack pointer
    MSR     CPSR_c, #MODE_SYS:OR:I_BIT:OR:F_BIT
    LDR     sp,=SYS_Stack0+STACK_SYS_SIZE-STACK_MARGIN

    // Setup system configuration register
    MRC     p15, 0, r0, c1, c0, 0
    AND     r0, r0, #0xFFFFDFFF     // use RAM vector
    ORR     r0, r0, #0x400          // enable SWP instruction
    MCR     p15, 0, r0, c1, c0, 0

    // Pre initialize Hardware before call main
    IMPORT  PreOneTimeInit
    BL      PreOneTimeInit

    IMPORT  __main
    B       __main
}

//-----------------------------------------------------------------------------
/**
    Undefined instruction excpetion handler
**/
//-----------------------------------------------------------------------------
__asm void UndefInstEntry(void)
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
__asm void SwiEntry(void)
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
__asm void PrefetchAbortEntry(void)
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
    Data abort exception handler
**/
//-----------------------------------------------------------------------------
__asm void DataAbortEntry(void)
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
    Interrupt request(IRQ) handler
**/
//-----------------------------------------------------------------------------
__irq void IrqEntry(void)
{
#if _SHARE_==SHARE_IRQ
    HalIrq_Handler();
#endif
}

//-----------------------------------------------------------------------------
/**
    Fast Interrupt request(FIQ) handler (NOT USED)
**/
//-----------------------------------------------------------------------------
__irq void FiqEntry(void)
{
#if _SHARE_==SHARE_IRQ
    HalFiq_Handler();
#endif
}

//-----------------------------------------------------------------------------
/**
    Pre Software and Hardware One time initialization before C main() is called.\n

    Initialize ITCM, DTCM, STCM, SSPM and DRAM allocation only allocator\n

    Initialize HAL
**/
//-----------------------------------------------------------------------------
void PreOneTimeInit(void)
{
    InitBootMode_t initMode;

    initMode = Hal_GetInitBootMode();

    if (initMode == cInitBootFwUpdated)
    {
        ///  @brief 0xF5000150 : Control Register Module Access Control
        REG_U32(0xF5000150) &= ~BIT(0); // clean <BIT [0] Memory Lock.

        #if CO_RESET_PCIE_PHY_ON_PERST
            REG_U32(0xF0112090) |= BIT(6);  // REV2.1 change to allow HW PCIe block reset at PERST
        #endif
    }

    Debug_OneTimeInit(initMode);

    Hal_PreOneTimeInit(initMode);
}

//-----------------------------------------------------------------------------
/**
    main function for boot code
**/
//-----------------------------------------------------------------------------
int main(void)
{
    InitBootMode_t initMode;
    pVoidFuncVoid FuncVect;

    initMode = Hal_GetInitBootMode();
    M_PRINTF("\n[BOOT] InitMode|%d\n", initMode);

    #if (_HOST_ == HOST_NVME)
    if (initMode != cInitBootFwUpdated)
    {
        HalPhy_OneTimeInit(initMode);
        HalLink_OneTimeInit(initMode);
    }
    #endif

    LoadFirmware();

    FuncVect = (pVoidFuncVoid)0x0;
    FuncVect();
}

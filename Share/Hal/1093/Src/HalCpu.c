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
//! \file
//! @brief CPU control API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "HalCpu.h"
#include "HalClock.h"
#include "ClockTree.h"
#include "RegClockPLL.h"
#include "RegCp15.h"
#include "RegApb.h"

#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define SPIN_LOCK_ID_BAR    SPIN_LOCK_ADDR      // spinlock base address
#define SPIN_LOCK_ID_SIZE   7                   // size per one spin lock Id (actaul size is 2^7)

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void EnablePerformanceCounter(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//
//  VARIABLE DECLARATION WITH INITIAL VALUE DOESN'T WORK!!!
//  UNINITIALZED VARIABLE DECLARATION ONLY !!!
//-----------------------------------------------------------------------------
#pragma arm section rodata = "data_perf_ui"
#pragma arm section rwdata = "data_perf_ui"
#pragma arm section zidata = "data_perf_ui"

static CpuPowerMode_t currentCpuPowerMode;
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
    Boot up pre initialization of CPU core HW at Power on.\n

    @return error code
**/
//-----------------------------------------------------------------------------
__asm Error_t HalCpu_DefaultMpuRegionSetting(void)
{
    ;; Disable the MPU before the region setting
    MRC p15, 0, r0, c1, c0, 0   ; read CP15 register 1
    BIC r0, r0, #0x1
    DSB
    MCR p15, 0, r0, c1, c0, 0   ; disable MPU
    ISB

    ;; --- Program MPU region
    ; setup MPU background region 0 - device, shared, noncacheabe, executable never, 4GB
    ldr  r0, =0x0
    MCR  p15, 0, r0, c6, c2, 0   ; select region 0
    ldr  r3, =0x00000000         ; Region 0 base address = 0x00000000
    MCR  p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ldr  r1, =0x1305             ; XN=not executable, AP=full access, TEX,C,B=Device, XN, S=shared
    MCR  p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr  r2, =0x003F             ; all sub-regions part of region0, region size=4GB, enable memory region
    MCR  p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; setup MPU region 12 - normal, nonshared, noncacheable, instruction executable, 64KB
    ldr  r0, =12
    MCR  p15, 0, r0, c6, c2, 0   ; select region 12
    ldr  r3, =0xFFFF0000         ; Region 12 base address = 0xFFFF0000
    MCR  p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ldr  r1, =0x0308             ; XN=executable, AP=full access, TEX,C,B=outer and inner non-cachable, S=non-shared
    MCR  p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr  r2, =0x001F             ; all sub-regions part of region 12, region size=64KB, enable memory region
    MCR  p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; setup MPU region 11 - normal, nonshared, noncacheable, instruction executable, 128MB
    ldr  r0, =11
    MCR  p15, 0, r0, c6, c2, 0   ; select region 11
    ldr  r3, =0x00000000         ; Region 11 base address = 0x00000000
    MCR  p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ldr  r1, =0x0308             ; XN=executable, AP=full access, TEX,C,B=outer and inner non-cachable, S=non-shared
    MCR  p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr  r2, =0x0035             ; all sub-regions part of region 11, region size=128MB, enable memory region
    MCR  p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; setup MPU region 10 - normal, shared, noncacheable, instruction executable, 128MB
    ldr  r0, =10
    MCR  p15, 0, r0, c6, c2, 0   ; select region 10
    ldr  r3, =0x08000000         ; Region 10 base address = 0x08000000
    MCR  p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ldr  r1, =0x030C             ; XN=executable, AP=full access, TEX,C,B=outer and inner non-cachable, S=shared
    MCR  p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr  r2, =0x0035             ; all sub-regions part of region 10, region size=128MB, enable memory region
    MCR  p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; setup MPU region 9 - normal WT, Shared, cacheable, instruction executable, 128MB
    ldr r0, =9
    MCR p15, 0, r0, c6, c2, 0   ; select region 9
    ldr r3, =0x10000000         ; Region 9 base address = 0x10000000
    MCR p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ldr r1, =0x0306             ; XN=executable, AP=full access, TEX,C,B=Normal WT cacehable, read allocate only, S=shared
    MCR p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr r2, =0x0035             ; all sub-regions part of region 9, region size=128MB, enable memory region
    MCR p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; setup MPU region 8 - Strongly ordered, Shared, X, 128MB
    ldr r0, =8
    MCR p15, 0, r0, c6, c2, 0   ; select region 8
    ldr r3, =0x18000000         ; Region 8 base address = 0x18000000
    MCR p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ldr r1, =0x0304             ; XN=executable, AP=full access, TEX,C,B=strongly ordered, S=shared
    MCR p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr r2, =0x0035             ; all sub-regions part of region 8, region size=128MB, enable memory region
    MCR p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; setup MPU region 7 - Strongly ordered, Shared, X, 256MB
    ldr r0, =7
    MCR p15, 0, r0, c6, c2, 0   ; select region 7
    ldr r3, =0x20000000         ; Region 7 base address = 0x20000000
    MCR p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ldr r1, =0x0304             ; XN=executable, AP=full access, TEX,C,B=strongly ordered, S=shared
    MCR p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr r2, =0x0037             ; all sub-regions part of region 7, region size=256MB, enable memory region
    MCR p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; setup MPU region 6 - Device, Shared, noncacheable, executable never, 256MB
    ldr r0, =6
    MCR p15, 0, r0, c6, c2, 0   ; select region 6
    ldr r3, =0x30000000         ; Region 6 base address = 0x30000000
    MCR p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ldr r1, =0x1305             ; XN=not executable, AP=full access, TEX,C,B=Device, XN, S=shared
    MCR p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr r2, =0x0037             ; all sub-regions part of region 6, region size=256MB, enable memory region
    MCR p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; setup MPU region 5 - normal WT, Shared, noncacheable, instruction executable, 1024MB
    ldr r0, =5
    MCR p15, 0, r0, c6, c2, 0   ; select region 5
    ldr r3, =0x40000000         ; Region 5 base address = 0x40000000
    MCR p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ;ldr r1, =0x0302             ; Write-Through, no write-allocation
    ;ldr r1, =0x0303             ; Write-Back, no write-allocation
    ldr r1, =0x030C             ; XN=executable, AP=full access, TEX,C,B=outer and inner non-cacehable, S=shared
    MCR p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr r2, =0x003B             ; all sub-regions part of region 5, region size=1024MB, enable memory region
    MCR p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; setup MPU region 4 - normal WT, Shared, noncacheable, instruction executable, 1024MB
    ldr r0, =4
    MCR p15, 0, r0, c6, c2, 0   ; select region 4
    ldr r3, =0x80000000         ; Region 4 base address = 0x80000000
    MCR p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ldr r1, =0x030C             ; XN=executable, AP=full access, TEX,C,B=outer and inner non-cacehable, S=shared
    MCR p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr r2, =0x003B             ; all sub-regions part of region 4, region size=1024MB, enable memory region
    MCR p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; setup MPU region 3 - normal, nonshared, noncacheable, executable never, 32MB
    ldr r0, =3
    MCR p15, 0, r0, c6, c2, 0   ; select region 2
    ldr r3, =0xC8000000         ; Region 2 base address = 0xC8000000
    MCR p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ldr r1, =0x130C             ; XN=not executable, AP=full access, TEX,C,B=outer and inner non-cachable, S=shared
    MCR p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr r2, =0x0031             ; all sub-regions part of region 3, region size=32MB, enable memory region
    MCR p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; setup MPU region 2 - normal, nonshared, noncacheable, instruction executable, 256MB
    ldr r0, =2
    MCR p15, 0, r0, c6, c2, 0   ; select region 2
    ldr r3, =0xC0000000         ; Region 2 base address = 0xC0000000
    MCR p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ldr r1, =0x0308             ; XN=executable, AP=full access, TEX,C,B=outer and inner non-cachable, S=non-shared
    MCR p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr r2, =0x0037             ; all sub-regions part of region 2, region size=256MB, enable memory region
    MCR p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; setup MPU region 1 - Strongly ordered, Shared, X, 1GB
    ldr r0, =1
    MCR p15, 0, r0, c6, c2, 0   ; select region 1
    ldr r3, =0xC0000000         ; Region 1 base address = 0xC0000000
    MCR p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ldr r1, =0x0304             ; XN=executable, AP=full access, TEX,C,B=strongly ordered, S=shared
    MCR p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr r2, =0x003B             ; all sub-regions part of region 1, region size=1GB, enable memory region
    MCR p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; Enable the MPU
    MRC p15, 0, r0, c1, c0, 0   ; read CP15 register 1
    ORR r0, r0, #0x1
    DSB
    MCR p15, 0, r0, c1, c0, 0   ; enable MPU
    ISB
    ;; --- End of Enable the MPU

    BX  lr
}

//-----------------------------------------------------------------------------
/**
    One-time initialization of CPU core HW.\n

     - flush and invalidate all caches.\n
     - Reset all SpinLock Synchronize objects.\n

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalCpu_OneTimeInit(InitBootMode_t initMode)
{
    if (initMode != cInitBootCpu0Boot)
    {
    #if _CPU_==CPU_ID0    // CPU0 only
        uint32_t id;

        for (id = 0; id < MAX_SPIN_LOCK; id++)
        {
            HalCpu_SpinUnlock(id);
        }
    #endif

        HalCpu_InvalidateCache();
        EnablePerformanceCounter();
    }

    currentCpuPowerMode = cCpuPmActive;

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    H/W block level power management API

    @param[in]  powerMode        targeting pm mode to enter
    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalCpu_SetPowerMode(CpuPowerMode_t powerMode)
{
    // no set up if power mode is not changed.
    if (currentCpuPowerMode == powerMode) return cEcNoError;

    switch (powerMode)
    {
        case cCpuPmDeepSleep:
            break;
        case cCpuPmActive:
            HalCpu_OneTimeInit(cInitBootDeepPowerDown);
            break;
        default:
            break;
    }

    currentCpuPowerMode = powerMode;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Return a CPU core id number

    @param  void

    @return CPU core id number (0 for CPU0, 1 for CPU1, 2 for CPU2)
**/
//-----------------------------------------------------------------------------
uint32_t HalCpu_GetId(void)
{
    return(_CPU_);   // defined by external make file
}

#if _CPU_==CPU_ID0    // CPU0 only
//-----------------------------------------------------------------------------
/**
    Reboot CPU\n.

    @param[in] reserved    shall be zero

    @return Error_t
**/
//-----------------------------------------------------------------------------
Error_t HalCpu_Reboot(uint32_t reserved)
{
    HalCpu_DisableIrq();

    rApb.rstCtrl.b.WARM_RST_FW = 1;

    return cEcNoError;
}
#endif

//-----------------------------------------------------------------------------
/**
    Delay for the given number of loop count.\n

    average 4 clock per loop for coretex R4\n

    @param[in]  loopCount   loop count to delay
**/
//-----------------------------------------------------------------------------
#define CLOCK_PER_LOOP    (4)
__asm void HalCpu_DelayLoop(uint32_t loopCount)
{
0
    SUBS    r0,r0,#1
    BCS     %B0

    BX      lr
}

//-----------------------------------------------------------------------------
/**
    Delay for the given number of micro-seconds by CPU looping\n.

    @param[in]  uSec        delay time in uSec
**/
//-----------------------------------------------------------------------------
void HalCpu_DelayUsec(uint32_t uSec)
{
    uint32_t cpuClockMhz;
    uint32_t loopCount;

    if (rClockPll.pllbCtrl1.b.PLLB_BYPASS_EN) // OSC clock source
    {
        cpuClockMhz = OSC_FREQ_MHZ;
    }
    else
    {
        cpuClockMhz = CPU_CLOCK_SET_FREQ_MHZ;
    }

    loopCount = (uSec * cpuClockMhz)/(CLOCK_PER_LOOP*HW_CLOCK_FREQ_RATIO); //
    HalCpu_DelayLoop(loopCount);
}

//-----------------------------------------------------------------------------
/**
    Wait for interrupt (CPU stopped and wake up for new interrupt)\n.

**/
//-----------------------------------------------------------------------------
void HalCpu_WaitForInterrupt(void)
{
    __wfi();
}

//-----------------------------------------------------------------------------
/**
    Enable interrupt request

**/
//-----------------------------------------------------------------------------
void HalCpu_EnableIrq(void)
{
    __enable_irq();
}

//-----------------------------------------------------------------------------
/**
    Disable interrupt request.

    @return previous CPSR register value
**/
//-----------------------------------------------------------------------------
__asm uint32_t HalCpu_DisableIrq(void)
{
    MRS      r0,CPSR
    ORR      r1,r0,#I_BIT
    MSR      CPSR_c,r1
    BX       lr
}

//-----------------------------------------------------------------------------
/**
    Restore interrupt status


    @param[in]  value       previous CPSR register value
**/
//-----------------------------------------------------------------------------
__asm void HalCpu_RestoreIrq(uint32_t value)
{
    MSR     CPSR_c, r0
    BX      lr
}

//-----------------------------------------------------------------------------
/**
    Fluash all CPU cache

    @param  void
**/
//-----------------------------------------------------------------------------
__asm void HalCpu_FlushCache(void)
{
        PRESERVE8

        ; Clean Data/Unified Caches
        MRC     p15, 1, r0, c0, c0, 1      ; Read CLIDR
        ANDS    r3, r0, #0x07000000        ; Extract coherency level
        MOV     r3, r3, LSR #23            ; Total cache levels << 1
        BXEQ    lr

        PUSH    {r4-r11}

        MOV     r10, #0                    ; R10 holds current cache level << 1
Loop1   ADD     r2, r10, r10, LSR #1       ; R2 holds cache "Set" position
        MOV     r1, r0, LSR r2             ; Bottom 3 bits are the Cache-type for this level
        AND     r1, r1, #7                 ; Isolate those lower 3 bits
        CMP     r1, #2
        BLT     Skip                       ; No cache or only instruction cache at this level

        MCR     p15, 2, r10, c0, c0, 0     ; Write the Cache Size selection register
        ISB                                ; ISB to sync the change to the CacheSizeID reg
        MRC     p15, 1, r1, c0, c0, 0      ; Reads current Cache Size ID register
        AND     r2, r1, #7                 ; Extract the line length field
        ADD     r2, r2, #4                 ; Add 4 for the line length offset (log2 16 bytes)
        LDR     r4, =0x3FF
        ANDS    r4, r4, r1, LSR #3         ; R4 is the max number on the way size (right aligned)
        CLZ     r5, r4                     ; R5 is the bit position of the way size increment
        LDR     r7, =0x7FFF
        ANDS    r7, r7, r1, LSR #13        ; R7 is the max number of the index size (right aligned)

Loop2   MOV     r9, r4                     ; R9 working copy of the max way size (right aligned)

Loop3   ORR     r11, r10, r9, LSL r5       ; Factor in the Way number and cache number into R11
        ORR     r11, r11, r7, LSL r2       ; Factor in the Set number
        MCR     p15, 0, r11, c7, c10, 2    ; Clean by Set/Way
        SUBS    r9, r9, #1                 ; Decrement the Way number
        BGE     Loop3
        SUBS    r7, r7, #1                 ; Decrement the Set number
        BGE     Loop2
Skip    ADD     r10, r10, #2               ; Increment the cache number
        CMP     r3, r10
        BGT     Loop1

        POP     {r4-r11}

        BX    lr
}

//-----------------------------------------------------------------------------
/**
    Invalidate all CPU cache

    @param  void
**/
//-----------------------------------------------------------------------------
__asm void HalCpu_InvalidateCache(void)
{
    MOV     r0, #0
    DSB
    MCR     p15, 0, r0, c15, c5, 0      ; Invalidate entire data cache
    MCR     p15, 0, r0, c7, c5, 0       ; Invalidate Instruction Cache
    MCR     p15, 0, r0, c7, c5, 6       ; Invalidate branch prediction array
    ISB
    BX      lr
}

//-----------------------------------------------------------------------------
/**
    Fluash and invalidate all CPU cache

    @param  void
**/
//-----------------------------------------------------------------------------
void HalCpu_FlushAndInvalidateCache(void)
{
    HalCpu_FlushCache();
    HalCpu_InvalidateCache();
}

//-----------------------------------------------------------------------------
/**
    Enable all CPU cache

    @param  void
**/
//-----------------------------------------------------------------------------
__asm void HalCpu_EnableCache(void)
{
    ;; Enable cache
    MRC     p15, 0, r0, c1, c0, 0       ; read CP15 register 1 into r0
    BIC     r0,  r0, #(0x1 << 13)       ; normal exception vectors selected
    ORR     r0,  r0, #(0x1 << 12)       ; ensure I Cache enabled
    ORR     r0,  r0, #(0x1 << 2)        ; ensure D Cache enabled
    MOV     r1, #0
    DSB
    MCR     p15, 0, r1, c15, c5, 0      ; Invalidate entire data cache
    MCR     p15, 0, r1, c7, c5, 0       ; Invalidate entire instruction cache
    MCR     p15, 0, r1, c7, c5, 6       ; Invalidate branch prediction array
    MCR     p15, 0, r0, c1, c0, 0       ; write value back
    ISB

    ;; Disable the MPU before the region setting
    MRC p15, 0, r0, c1, c0, 0   ; read CP15 register 1
    BIC r0, r0, #0x1
    DSB
    MCR p15, 0, r0, c1, c0, 0   ; disable MPU
    ISB

    ;; setup MPU region 5 - NonShared, cacheable, instruction executable, 1024MB
    ldr r0, =5
    MCR p15, 0, r0, c6, c2, 0   ; select region 5
    ldr r3, =0x40000000         ; Region 5 base address = 0x40000000
    MCR p15, 0, r3, c6, c1, 0   ; Write MPU region Base address register
    ldr r1, =0x0302             ; Write-Through, no write-allocation
    ;ldr r1, =0x0303             ; Write-Back, no write-allocation
    MCR p15, 0, r1, c6, c1, 4   ; write region access control register
    ldr r2, =0x003B             ; all sub-regions part of region 5, region size=1024MB, enable memory region
    MCR p15, 0, r2, c6, c1, 2   ; write data MPU region size and enable register

    ;; Enable the MPU
    MRC p15, 0, r0, c1, c0, 0   ; read CP15 register 1
    ORR r0, r0, #0x1
    DSB
    MCR p15, 0, r0, c1, c0, 0   ; enable MPU
    ISB
    ;; --- End of Enable the MPU

    BX  lr
}

//-----------------------------------------------------------------------------
/**
    Cpu context save(Set Jump) API function for ARM

    @param  buffer address
**/
//-----------------------------------------------------------------------------
__asm uint32_t HalCpu_SetJump(void* pBuffAddr)
{
    // Enter FIQ mode and back up the stack pointer
    MSR     CPSR_c, #MODE_FIQ:OR:I_BIT:OR:F_BIT
    STMIA   r0!, {sp}

    // Enter IRQ mode and back up the stack pointer
    MSR     CPSR_c, #MODE_IRQ:OR:I_BIT:OR:F_BIT
    STMIA   r0!, {sp}

    // Enter prefetch and data Abort mode and set up the stack pointer
    MSR     CPSR_c, #MODE_ABT:OR:I_BIT:OR:F_BIT
    STMIA   r0!, {sp}

    // Enter Undefined mode and set up the stack pointer
    MSR     CPSR_c, #MODE_UDF:OR:I_BIT:OR:F_BIT
    STMIA   r0!, {sp}

    // Enter SVC mode and set up the stack pointer
    MSR     CPSR_c, #MODE_SVC:OR:I_BIT:OR:F_BIT
    STMIA   r0!, {sp}

    // Enter System mode and back up all registers
    MSR     CPSR_c, #MODE_SYS:OR:I_BIT:OR:F_BIT
    MRS     r3, CPSR
    STMIA   r0!, {r3-r14}

    // Backup system configuration register
    MRC     p15, 0, r2, c1, c0, 0
    STMIA   r0!, {r2}

    MOV     r0, #0
    BX      lr
}

//-----------------------------------------------------------------------------
/**
    Cpu context restore(Long Jump) API function for ARM

    @param  buffer address, param value
**/
//-----------------------------------------------------------------------------
__asm void HalCpu_LongJump(void* pBuffAddr, uint32_t value)
{
    // Enter FIQ mode and restore the stack pointer
    MSR     CPSR_c, #MODE_FIQ:OR:I_BIT:OR:F_BIT
    LDMIA   r0!, {sp}

    // Enter IRQ mode and restore the stack pointer
    MSR     CPSR_c, #MODE_IRQ:OR:I_BIT:OR:F_BIT
    LDMIA   r0!, {sp}

    // Enter prefetch and data Abort mode and set up the stack pointer
    MSR     CPSR_c, #MODE_ABT:OR:I_BIT:OR:F_BIT
    LDMIA   r0!, {sp}

    // Enter Undefined mode and set up the stack pointer
    MSR     CPSR_c, #MODE_UDF:OR:I_BIT:OR:F_BIT
    LDMIA   r0!, {sp}

    // Enter SVC mode and set up the stack pointer
    MSR     CPSR_c, #MODE_SVC:OR:I_BIT:OR:F_BIT
    LDMIA   r0!, {sp}

    // Enter System mode and restore the all registers
    MSR     CPSR_c, #MODE_SYS:OR:I_BIT:OR:F_BIT
    LDMIA   r0!, {r3-r14}
    MSR     CPSR_c, r3

    // Restore system configuration register
    LDMIA   r0!, {r2}
    MCR     p15, 0, r2, c1, c0, 0

    MOV     r0, r1
    BX      lr
}

//-----------------------------------------------------------------------------
/**
    Spin unlock by using LDREX/STREX instruction.

    @param[in]  spinLockId      Spin lock id
**/
//-----------------------------------------------------------------------------
__asm void HalCpu_SpinLock(uint32_t spinLockId)
{
    CMP     r0, #MAX_SPIN_LOCK      // return if spinlock id is not vaild
    BXCS    lr

    LDR     r1, =SPIN_LOCK_ID_BAR           // convert spin lock id to actual address
    ADD     r0,r1,r0,LSL #SPIN_LOCK_ID_SIZE // SPIN_LOCK_ID_BAR + id*sizeof(spinlockId)

    MOV     r1,#1
1
    LDREX   r2, [r0]                // load the lock value
    CMP     r2, r1                  // already locked ?
    BEQ     %B1                     // retry
    STREXNE r2, r1, [r0]            // not locked, attemp to lock
    CMPNE   r2, #1                  // locked ?
    BEQ     %B1

    DMB
    BX      lr
}

//-----------------------------------------------------------------------------
/**
    Spin unlock by using LDR/STR instruction.

    @param[in]  spinLockId      Spin lock id
**/
//-----------------------------------------------------------------------------
__asm void HalCpu_SpinUnlock(uint32_t spinLockId)
{
    CMP     r0, #MAX_SPIN_LOCK      // return if spinlock id is not vaild
    BXCS    lr

    LDR     r1, =SPIN_LOCK_ID_BAR           // convert spin lock id to actual address
    ADD     r0,r1,r0,LSL #SPIN_LOCK_ID_SIZE // SPIN_LOCK_ID_BAR + id*sizeof(spinlockId)

    MOV     r1,#0
    DMB
    STR     r1,[r0]

    BX      lr
}

//-----------------------------------------------------------------------------
/**
    Get a current CPU's mode.
**/
//-----------------------------------------------------------------------------
__asm uint32_t HalCpu_GetMode(void)
{
    MRS     r0,CPSR
    AND     r0,r0,#MODE_MASK
    BX      lr
}

//-----------------------------------------------------------------------------
/**
    Get a current stack pointer

    @param[in]  mode      CPU mode to get stack pointer
**/
//-----------------------------------------------------------------------------
__asm uint32_t HalCpu_GetStackPointer(uint32_t mode)
{
    PRESERVE8

    MRS     R1, CPSR        // save CPSR
    MSR     CPSR_c, R0      // change mode
    NOP
    MOV     R0, SP          // get stack pointer
    MSR     CPSR_c, R1      // return to orginal mode

    BX      LR
}

#if _CPU_==CPU_ID0    // CPU0 only
//-----------------------------------------------------------------------------
/**
    Convert address between CPUs

    @param[in]  cpuId0      CPU id
    @param[in]  address0    cpuId0's address
    @param[in]  cpuId1      CPU id to be converted
    @param[out] address1    converted address (cpuId1's address)
**/
//-----------------------------------------------------------------------------
Error_t HalCpu_ConvertAddress(uint32_t cpuId0, uint32_t address0, uint32_t cpuId1, uint32_t* address1)
{

    *address1 = address0;

    if (cpuId0==CPU_ID0)
    {
        if (address0<CPU0_ITCM_ADDR+CPU0_ITCM_SIZE)
        {
            *address1 = AXI_ATCM_CPU0_ADDR + address0 - CPU0_ITCM_ADDR;
        }

        if (CPU0_DTCM_ADDR<=address0 && address0<CPU0_DTCM_ADDR+CPU0_DTCM_SIZE)
        {
            *address1 = AXI_B0TCM_CPU0_ADDR + address0 - CPU0_DTCM_ADDR;
        }
    }

    if (cpuId0==CPU_ID1)
    {
        if (address0<CPU1_ITCM_ADDR+CPU1_ITCM_SIZE)
        {
            *address1 = AXI_ATCM_CPU1_ADDR + address0 - CPU1_ITCM_ADDR;
        }

        if (CPU1_DTCM_ADDR<=address0 && address0<CPU1_DTCM_ADDR+CPU1_DTCM_SIZE)
        {
            *address1 = AXI_B0TCM_CPU1_ADDR + address0 - CPU1_DTCM_ADDR;
        }
    }

    if (cpuId0==CPU_ID2)
    {
        if (address0<CPU2_ITCM_ADDR+CPU2_ITCM_SIZE)
        {
            *address1 = AXI_ATCM_CPU2_ADDR + address0 - CPU2_ITCM_ADDR;
        }

        if (CPU2_DTCM_ADDR<=address0 && address0<CPU2_DTCM_ADDR+CPU2_DTCM_SIZE)
        {
            *address1 = AXI_B0TCM_CPU2_ADDR + address0 - CPU2_DTCM_ADDR;
        }
    }

    return (cEcNoError);
}
#endif

//-----------------------------------------------------------------------------
/**
    Enable Performance Moniter Counter
**/
//-----------------------------------------------------------------------------
static void EnablePerformanceCounter(void)
{
    rEventPerformanceMonitorControl.enable = 1; // Enable performance monitor block

    rEventCountEnableSet.cycleCounter = 1; // enable cycle counter
}

//-----------------------------------------------------------------------------
/**
    Read Performance monitor cycle counter

    @return Performance monitor cycle counter
**/
//-----------------------------------------------------------------------------
uint32_t HalCpu_ReadPerformanceCounter(void)
{
    return rEventCycleCount;
}

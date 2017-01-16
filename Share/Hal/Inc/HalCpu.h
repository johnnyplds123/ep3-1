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
//! @brief Pure HAL API definition for CPU HW control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define MODE_USER       0x10    ///< ARM USER
#define MODE_FIQ        0x11    ///< ARM FIQ
#define MODE_IRQ        0x12    ///< ARM IRQ
#define MODE_SVC        0x13    ///< ARM SUPERVISOR
#define MODE_ABT        0x17    ///< ARM ABORT
#define MODE_UDF        0x1B    ///< ARM UNDEFINED
#define MODE_SYS        0x1F    ///< ARM SYSTEM
#define MODE_MASK       0x1F    ///< ARM mode mask

#define MODE_THUMB      0x20    ///< THUMB Intruction mode

#define I_BIT           0x80    ///< ARM IRQ BIT (set means disable)
#define F_BIT           0x40    ///< ARM FIQ BIT (set means disable)

#define MAX_SPIN_LOCK   8       ///< Maximum number of spinlock
#define SPIN_LOCK_NULL  0xFFFF  ///< NULL spinlock, no spinlock synchronization.

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
/// Critical section for single CPU access region
#define BEGIN_CS(X)                 { X=HalCpu_DisableIrq(); }
#define END_CS(X)                   { HalCpu_RestoreIrq(X); }

/// Critical section for multi-CPU access region
#define BEGIN_MULTI_CS(X,S)         { X=HalCpu_DisableIrq(); HalCpu_SpinLock(S); }
#define END_MULTI_CS(X,S)           { HalCpu_SpinUnlock(S); HalCpu_RestoreIrq(X); }

/// Critical section for single CPU access region
#define BEGIN_CS1                   { uint32_t _org_state_; _org_state_=HalCpu_DisableIrq();
#define END_CS1                       HalCpu_RestoreIrq(_org_state_); }

/// Critical section for multi-CPU access region
#define BEGIN_MULTI_CS1(S)          { uint32_t _org_state_; _org_state_=HalCpu_DisableIrq(); HalCpu_SpinLock(S);
#define END_MULTI_CS1(S)              HalCpu_SpinUnlock(S); HalCpu_RestoreIrq(_org_state_); }

/// Critical section for multi-CPU spinlock
#define BEGIN_MULTI_CS2(S)          HalCpu_SpinLock(S);
#define END_MULTI_CS2(S)            HalCpu_SpinUnlock(S);

/// Convert DRAM address, cached or direct
#define GET_DIRECT_DRAM_ADDR(X)     (((uint32_t)(X)&(~CACHED_DRAM_ADDR))|(DIRECT_DRAM_ADDR))
#define GET_CACHED_DRAM_ADDR(X)     (((uint32_t)(X)&(~DIRECT_DRAM_ADDR))|(CACHED_DRAM_ADDR))

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef uint32_t CpuCs_t;        /// CPU critical session

/// @brief Cpu power mode definition
typedef enum
{
    cCpuPmActive=1,        ///< Active mode - highest power consumption state
    cCpuPmDeepSleep=15     ///< Deep sleep mode - Lowest power consumption state
} CpuPowerMode_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t     HalCpu_OneTimeInit(InitBootMode_t initMode);
uint32_t    HalCpu_GetId(void);
Error_t     HalCpu_Reboot(uint32_t reserved);

void        HalCpu_DelayLoop(uint32_t loopCount);
void        HalCpu_DelayUsec(uint32_t uSec);

void        HalCpu_FlushCache(void);
void        HalCpu_InvalidateCache(void);
void        HalCpu_FlushAndInvalidateCache(void);

void        HalCpu_SpinLock(uint32_t spinLockId);
void        HalCpu_SpinUnlock(uint32_t spinLockId);

void        HalCpu_EnableIrq(void);
uint32_t    HalCpu_DisableIrq(void);
void        HalCpu_RestoreIrq(uint32_t value);
void        HalCpu_WaitForInterrupt(void);

uint32_t    HalCpu_GetMode(void);
uint32_t    HalCpu_GetStackPointer(uint32_t mode);

Error_t     HalCpu_ConvertAddress(uint32_t cpuId, uint32_t address, uint32_t cpuId1, uint32_t* convertedAddress);

uint32_t    HalCpu_ReadPerformanceCounter(void);

uint32_t    HalCpu_SetJump(void* pBuffAddr);
void        HalCpu_LongJump(void* pBuffAddr, uint32_t value);
Error_t     HalCpu_SetPowerMode(CpuPowerMode_t powerMode);
Error_t     HalCpu_DefaultMpuRegionSetting(void);

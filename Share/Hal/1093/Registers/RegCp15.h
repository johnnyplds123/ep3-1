#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2013  -  All rights reserved
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
//! @brief Pure HAL API definition for L1 Cache HW control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  System Control Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

typedef struct SystemControl_t
{
    uint32_t mpuEnable                   : 1;
    uint32_t strictDataAlignmentEnable   : 1;
    uint32_t l1DataCacheEnable           : 1;
    uint32_t reserved                    : 4;
    uint32_t reserved2                   : 4;
    uint32_t branchPredictEnable         : 1;
    uint32_t l1InstructionCacheEnable    : 1;
    uint32_t exceptionVectorHigh         : 1;
    uint32_t l1CacheRoundRobinReplacement: 1;
    uint32_t reserved3                   : 1;
    uint32_t reserved4                   : 1;
    uint32_t mpuBackgrounRegionEnable    : 1;
    uint32_t reserved5                   : 1;
    uint32_t divideByZeroExceptionEnable : 1;
    uint32_t reserved6                   : 1;
    uint32_t fastInterruptEnable         : 1;
    uint32_t reserved7                   : 1;
    uint32_t irqOffsetFromVic            : 1;
    uint32_t endiannessOnException       : 1;
    uint32_t reserved8                   : 1;
    uint32_t fastInterruptNonMaskable    : 1;
    uint32_t texRemapEnable              : 1;
    uint32_t accessFlasgEnable           : 1;
    uint32_t thumbOnException            : 1;
    uint32_t bigEndianInstructions       : 1;
} SystemControl_t;

register SystemControl_t rSystemControl __asm("cp15:0:c1:c0:0");  //lint !e117

//-----------------------------------------------------------------------------
//  MPU Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

typedef enum MpuAccessPermission_t
{
    cMpuAccessNoAccessAllowed = 0,
    cMpuAccessNoAccessInUserMode = 1,
    cMpuAccessNoWriteInUserMode = 2,
    cMpuAccessFullAccess = 3,
    cMpuAccessPriviligedReadOnly = 5,
    cMpuAccessReadOnly = 6
} MpuAccessPermission_t;

typedef struct MpuRegionAccessControl_t
{
    uint32_t                  b                   : 1;
    uint32_t                  c                   : 1;
    uint32_t                  shared              : 1;
    uint32_t                  typeExtension       : 3;
    uint32_t                  reserved            : 2;
    MpuAccessPermission_t     accessPermission    : 3;
    uint32_t                  reserved2           : 1;
    uint32_t                  executeNever        : 1;
    uint32_t                  reserved3           : 19;
} MpuRegionAccessControl_t;

typedef enum MpuRegionSize_t
{
    cMpuRegionSize32B = 4,
    cMpuRegionSize64B,     //5
    cMpuRegionSize128B,    //6
    cMpuRegionSize256B,    //7
    cMpuRegionSize512B,    //8
    cMpuRegionSize1KB,     //9
    cMpuRegionSize2KB,     //10
    cMpuRegionSize4KB,     //11
    cMpuRegionSize8KB,     //12
    cMpuRegionSize16KB,    //13
    cMpuRegionSize32KB,    //14
    cMpuRegionSize64KB,    //15
    cMpuRegionSize128KB,   //16
    cMpuRegionSize256KB,   //17
    cMpuRegionSize512KB,   //18
    cMpuRegionSize1MB,     //19
    cMpuRegionSize2MB,     //20
    cMpuRegionSize4MB,     //21
    cMpuRegionSize8MB,     //22
    cMpuRegionSize16MB,    //23
    cMpuRegionSize32MB,    //24
    cMpuRegionSize64MB,    //25
    cMpuRegionSize128MB,   //26
    cMpuRegionSize256MB,   //27
    cMpuRegionSize512MB,   //28
    cMpuRegionSize1GB,     //29
    cMpuRegionSize2GB,     //30
    cMpuRegionSize4GB      //31
} MpuRegionSize_t;

typedef struct MpuMemoryRegionSizeAndEnable_t
{
    uint32_t            enabled             : 1;
    MpuRegionSize_t     size                : 5;
    uint32_t            reserved            : 2;
    uint32_t            subregionDisable    : 8;
    uint32_t            reserved2           : 16;
} MpuMemoryRegionSizeAndEnable_t;

//lint -e117
register uint32_t                           rMpuMemoryRegionNumber          __asm("cp15:0:c6:c2:0");
register MpuRegionAccessControl_t           rMpuMemoryRegionAccessControl   __asm("cp15:0:c6:c1:4");
register MpuMemoryRegionSizeAndEnable_t     rMpuMemoryRegionSizeAndEnable   __asm("cp15:0:c6:c1:2");
register uint32_t                           rMpuMemoryRegionBaseAddress     __asm("cp15:0:c6:c1:0");
register uint32_t                           rMpuDataFaultAddress            __asm("cp15:0:c6:c0:0");
//lint +e117

//-----------------------------------------------------------------------------
//  MPU Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

typedef struct CacheSetWaySelection_t
{
    uint32_t reserved        : 5;
    uint32_t set             : 9;
    uint32_t reserved2       : 16;
    uint32_t way             : 2;
} CacheSetWaySelection_t;

typedef uint32_t ShouldBeZero_t;

//lint -e117
register ShouldBeZero_t              rCacheInvalidateAllICaches                     __asm("cp15:0:c7:c5:0");
register uint32_t                    rCacheInvalidateICacheToPouByAddress           __asm("cp15:0:c7:c5:1");
register ShouldBeZero_t              rCacheFlushPrefetchBuffer                      __asm("cp15:0:c7:c5:4");
register uint32_t                    rCacheInvalidateDCacheToPocByAddress           __asm("cp15:0:c7:c6:1");
register CacheSetWaySelection_t      rCacheInvalidateDCacheBySetWay                 __asm("cp15:0:c7:c6:2");
register uint32_t                    rCacheCleanDCacheToPocByAddress                __asm("cp15:0:c7:c10:1");
register CacheSetWaySelection_t      rCacheCleanDCacheBySetWay                      __asm("cp15:0:c7:c10:2");
register ShouldBeZero_t              rCacheDataSynchronizationBarrier               __asm("cp15:0:c7:c10:4");
register ShouldBeZero_t              rCacheDataMemoryBarrier                        __asm("cp15:0:c7:c10:5");
register uint32_t                    rCacheCleanDCacheToPouByAddress                __asm("cp15:0:c7:c11:1");
register uint32_t                    rCacheCleanAndInvalidateDCacheToPouByAddress   __asm("cp15:0:c7:c14:1");
register CacheSetWaySelection_t      rCacheCleanAndInvalidateDCacheBySetWay         __asm("cp15:0:c7:c14:2");
register ShouldBeZero_t              rCacheInvalidateAllDCaches                     __asm("cp15:0:c15:c5:0");
//lint +e117

//-----------------------------------------------------------------------------
//  Performance Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

typedef enum PerformanceEventSelection_t
{
    cEventSoftwareIncrement                                 = 0x00,
    cEventInstructionCacheMiss                              = 0x01,
    cEventDataCacheMiss                                     = 0x03,
    cEventDataCacheAccess                                   = 0x04,
    cEventDataReadEexecuted                                 = 0x06,
    cEventDataWriteExecuted                                 = 0x07,
    cEventInstructionExecuted                               = 0x08,
    cEventDualIssuedPairOfInstructionsExecuted              = 0x5E,
    cEventExceptionTaken                                    = 0x09,
    cEventExceptionReturnExecuted                           = 0x0A,
    cEventChangeToContextIDExecuted                         = 0x0B,
    cEventSoftwareChangeOfPCExecuted                        = 0x0C,
    cEventBranchInstructionExecuted                         = 0x0D,
    cEventProcedureReturnExecuted                           = 0x0E,
    cEventUnalignedAccessExecuted                           = 0x0F,
    cEventBranchMispredictedOrNotPredicted                  = 0x10,
    cEventCycleCount                                        = 0x11,
    cEventBranchPredicted                                   = 0x12,
    cEventStallOnInstructionBuffer                          = 0x40,
    cEventStallOnDataDependency                             = 0x41,
    cEventDataCacheWriteBack                                = 0x42,
    cEventExternalMemoryRequest                             = 0x43,
    cEventStallOnLoadStoreUnit                              = 0x44,
    cEventStoreBufferForcedDrain                            = 0x45,
    cEventFiqDisabledCycles                                 = 0x46,
    cEventIrqDisabledCycles                                 = 0x47,
    cEventStoreBufferMerge                                  = 0x50,
    cEventLoadStoreUnitStallByBufferFull                    = 0x51,
    cEventLoadStoreUnitStallByQueueFull                     = 0x52,
    cEventIntegerDivideExecuted                             = 0x53,
    cEventStallByIntegerDivide                              = 0x54,
    cEventLinefillByPldInstruction                          = 0x55,
    cEventNoLinefillByPldInstruction                        = 0x56,
    cEventNonCacheableAccessOnAxi                           = 0x57,
    cEventInstructionCacheAccess                            = 0x58,
    cEventDualIssueCaseA                                    = 0x5A,
    cEventDualIssueCaseB1B2F2F2D                            = 0x5B,
    cEventDualIssueOther                                    = 0x5C,
    cEventDoublePrecisionFloatingPointInstructionExecuted   = 0x5D,
    cEventProcessorLivelock                                 = 0x62,
    cEventCycleCount2                                       = 0xFF
} PerformanceEventSelection_t;

typedef struct EventPerformanceMonitorControl_t
{
    uint32_t enable : 1;
    uint32_t eventCounterReset : 1;
    uint32_t cycleCounterReset : 1;
    uint32_t cycleCountEvery64Cycles : 1;
    uint32_t exportEnable : 1;
    uint32_t cycleCountAlwaysEnabled : 1;
    uint32_t reserved : 5;
    uint32_t numberOfCounters : 5;
    uint32_t IdCode : 8;
    uint32_t ImplementerCode : 8;
} EventPerformanceMonitorControl_t;

typedef struct EventCounterEnable_t
{
    uint32_t counter : 3;
    uint32_t reserved : 28;
    uint32_t cycleCounter : 1;
} EventCounterEnable_t;

typedef struct EventPerformanceCounterSelection_t
{
    uint32_t counter : 5;
    uint32_t reserved : 27;
} EventPerformanceCounterSelection_t;

typedef struct EventTypeSelection_t
{
    PerformanceEventSelection_t selection : 8;
    uint32_t reserved : 24;
} rEventTypeSelection_t;

typedef struct EventUserEnable_t
{
    uint32_t enable : 1;
    uint32_t reserved : 31;
} EventUserEnable_t;

//lint -e117
register EventPerformanceMonitorControl_t   rEventPerformanceMonitorControl     __asm("cp15:0:c9:c12:0");
register EventCounterEnable_t               rEventCountEnableSet                __asm("cp15:0:c9:c12:1");
register EventCounterEnable_t               rEventCountEnableClear              __asm("cp15:0:c9:c12:2");
register EventCounterEnable_t               rEventOverflowFlagStatus            __asm("cp15:0:c9:c12:3");
register EventCounterEnable_t               rEventSoftwareIncrement             __asm("cp15:0:c9:c12:4");
register EventPerformanceCounterSelection_t rEventPerformanceCounterSelection   __asm("cp15:0:c9:c12:5");
register uint32_t                           rEventCycleCount                    __asm("cp15:0:c9:c13:0");
register rEventTypeSelection_t              rEventTypeSelection                 __asm("cp15:0:c9:c13:1");
register uint32_t                           rEventCount                         __asm("cp15:0:c9:c13:2");
register EventUserEnable_t                  rEventUserEnable                    __asm("cp15:0:c9:c14:0");
register EventCounterEnable_t               rEventInterruptEnableSet            __asm("cp15:0:c9:c14:1");
register EventCounterEnable_t               rEventInterruptEnableClear          __asm("cp15:0:c9:c14:2");
//lint +e117


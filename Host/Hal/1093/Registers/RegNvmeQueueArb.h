#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2015  -  All rights reserved     
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
//! @brief NVMe Queue Arbiter Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  NVMe Queue Arbiter Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF5014000 : Queue Arbiter Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t QARB_DIS            : 1;         ///<BIT [0] Queue Arbiter Disable.
        uint32_t RF5014000_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} QueueArbCfg_t;

///  @brief 0xF501400C : Queue Arbiter Engine Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF501400C_0_RSVD     : 1;         ///<BIT [0] Reserved.
        uint32_t QARB_ENG_BSY         : 1;         ///<BIT [1] Queue Arbiter Engine Busy.
        uint32_t RF501400C_3_2_RSVD   : 2;         ///<BIT [3:2] Reserved.
        uint32_t QARB_EMPTY           : 1;         ///<BIT [4] Queue Arbiter Empty.
        uint32_t RF501400C_15_5_RSVD  :11;         ///<BIT [15:5] Reserved.
        uint32_t QARB_STATE           : 3;         ///<BIT [18:16] Queue Arbiter Controller State.
        uint32_t RF501400C_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} QueueArbEngineStat_t;

///  @brief 0xF5014020 : Queue Arbiter Error FIFO Count
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t QARB_ERR_FIFO_CNT   : 3;         ///<BIT [2:0] Queue Arbiter Error Status Counter.
        uint32_t RF5014020_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} QueueArbErrFifoCnt_t;

///  @brief 0xF501402C : Queue Arbiter Monitor Signal Select
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t QARB_MON_SEL        : 5;         ///<BIT [4:0] Queue Arbiter Monitor Signal Select.
        uint32_t RF501402C_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} QueueArbMonitorSgnSel_t;

///  @brief 0xF5014030  - 0xF5014038: Queue Arbiter Function 0/1/2 Arbitration Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t QARB_ARB_BST       : 3;         ///<BIT [2:0] Arbitration Burst for Function.
        uint32_t RF5014030_7_3_RSVD : 5;         ///<BIT [7:3] Reserved.
        uint32_t QARB_LP_WT         : 8;         ///<BIT [15:8] Low Priority Weight for Function.
        uint32_t QARB_MP_WT         : 8;         ///<BIT [23:16] Medium Priority Weight for Function.
        uint32_t QARB_HP_WT         : 8;         ///<BIT [31:24] High Priority Weight for Function.
    } b;    
} QueueArbFuncArbCfg_t;

///  @brief 0xF5014030 : Queue Arbiter Function 0 Arbitration Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t QARB_ARB_BST_F0    : 3;         ///<BIT [2:0] Arbitration Burst for Function 0.
        uint32_t RF5014030_7_3_RSVD : 5;         ///<BIT [7:3] Reserved.
        uint32_t QARB_LP_WT_F0      : 8;         ///<BIT [15:8] Low Priority Weight for Function 0.
        uint32_t QARB_MP_WT_F0      : 8;         ///<BIT [23:16] Medium Priority Weight for Function 0.
        uint32_t QARB_HP_WT_F0      : 8;         ///<BIT [31:24] High Priority Weight for Function 0.
    } b;    
} QueueArbFunc0ArbCfg_t;

///  @brief 0xF5014034 : Queue Arbiter Function 1 Arbitration Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t QARB_ARB_BST_F1    : 3;         ///<BIT [2:0] Arbitration Burst for Function 1.
        uint32_t RF5014034_7_3_RSVD : 5;         ///<BIT [7:3] Reserved.
        uint32_t QARB_LP_WT_F1      : 8;         ///<BIT [15:8] Low Priority Weight for Function 1.
        uint32_t QARB_MP_WT_F1      : 8;         ///<BIT [23:16] Medium Priority Weight for Function 1.
        uint32_t QARB_HP_WT_F1      : 8;         ///<BIT [31:24] High Priority Weight for Function 1.
    } b;    
} QueueArbFunc1ArbCfg_t;

///  @brief 0xF5014038 : Queue Arbiter Function 2 Arbitration Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t QARB_ARB_BST_F2    : 3;         ///<BIT [2:0] Arbitration Burst for Function 2.
        uint32_t RF5014038_7_3_RSVD : 5;         ///<BIT [7:3] Reserved.
        uint32_t QARB_LP_WT_F2      : 8;         ///<BIT [15:8] Low Priority Weight for Function 2.
        uint32_t QARB_MP_WT_F2      : 8;         ///<BIT [23:16] Medium Priority Weight for Function 2.
        uint32_t QARB_HP_WT_F2      : 8;         ///<BIT [31:24] High Priority Weight for Function 2.
    } b;    
} QueueArbFunc2ArbCfg_t;


/// @brief 0xF5014000-0xF501403C : NvmeQueueArbiter_t MODULE
typedef struct
{
    QueueArbCfg_t queueArbCfg;                         ///< 0xF5014000 : Queue Arbiter Configuration
    uint8_t rsvd_RF5014004[8];                         ///< 0xF5014004 - 0xF501400B : Reserved.
    QueueArbEngineStat_t queueArbEngineStat;           ///< 0xF501400C : Queue Arbiter Engine Status
    uint8_t rsvd_RF5014010[16];                        ///< 0xF5014010 - 0xF501401F : Reserved.
    QueueArbErrFifoCnt_t queueArbErrFifoCnt;           ///< 0xF5014020 : Queue Arbiter Error FIFO Count
    uint32_t QARB_ERR_FIFO_DW[2];                      ///< 0xF5014024 - 0xF5014028 : Queue Arbiter Error FIFO Dword 0/1
    QueueArbMonitorSgnSel_t queueArbMonitorSgnSel;     ///< 0xF501402C : Queue Arbiter Monitor Signal Select
    QueueArbFuncArbCfg_t queueArbFuncArbCfg[3];        ///< 0xF5014030 - 0xF501403B: Queue Arbiter Function 0/1/2 Arbitration Configuration
} NvmeQueueArbiter_t;
COMPILE_ASSERT(offsetof(NvmeQueueArbiter_t,queueArbFuncArbCfg[2])==0xF5014038-0xF5014000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile NvmeQueueArbiter_t rNvmeQueueArbiter;///< 0xF5014000-0xF501403C : NvmeQueueArbiter_t MODULE

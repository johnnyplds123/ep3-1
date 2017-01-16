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
//! @brief NVMe Status Manager Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  NVMe Status Manager Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF5015000 : Status Manager Fifo Write Port
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MCQ_REQ_ID             : 5;    ///<BIT [4:0] DW0.REQ_ID
        uint32_t R10015000_5_5_RSVD     : 1;    ///<BIT [5] Reserved
        uint32_t WT                     : 1;    ///<BIT [6] Wait for CQ available
        uint32_t CQE_MODE               : 1;    ///<BIT [7] DW0.CQE_MODE (0=Default CQE, 1=HW I/O CQE)
        uint32_t MCQ_FUNC_ID            : 8;    ///<BIT [15:8] DW0.FUNC_ID
        uint32_t R10015000_16_14_RSVD   : 1;    ///<BIT [16] Reserved
        uint32_t MCQ_STATUS             :15;    ///<BIT [31:17] DW0.STAUS
    } dw0;
    struct
    {
        uint32_t MCQ_CMD_SPC;                   ///<BIT {31:0] DW1.CMD_SPC
    } dw1;
    struct
    {
        uint32_t MCQ_CMD_ID             :16;    ///<BIT [15:0] DW2.CMD_ID
        uint32_t MCQ_SQ_ID              :16;    ///<BIT [31:16] DW2.SQ_ID
    } dw2;
} StatManagerFifoWritePort_t;

///  @brief 0xF5015004 : Status Manager FIFO Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MN_CQ_REQ_AVL_ENTRY_CNT : 4;         ///<BIT [3:0] Manual CQ Request Available Entry Counter.
        uint32_t MN_CQ_REQ_FIFO_STAT     : 2;         ///<BIT [5:4] Manual Completion Queue Request FIFO Status.
        uint32_t RF5015004_31_6_RSVD     :26;         ///<BIT [31:6] Reserved.
    } b;    
} StatManagerFifoStat_t;

///  @brief 0xF5015008 : Status Manager FIFO Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FLUSH_MN_REQ_FIFO   : 1;         ///<BIT [0] Reset Manual Completion Queue Request FIFO Pointers.
        uint32_t RF5015008_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} StatManagerFifoCtrl_t;

///  @brief 0xF5015014 : Status Manager Error FIFO Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ERR_RPT_VAL_ENT_CNT : 4;         ///<BIT [3:0] Valid Entry Counter.
        uint32_t ERR_RPT_FIFO_STAT   : 2;         ///<BIT [5:4] Error Report FIFO Status.
        uint32_t RF5015014_31_6_RSVD :26;         ///<BIT [31:6] Reserved.
    } b;    
} StatManagerErrFifoStat_t;

///  @brief 0xF5015018 : Status Manager Error FIFO Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FLUSH_ERR_RPT_FIFO  : 1;         ///<BIT [0] Reset Error Report FIFO Pointers.
        uint32_t RF5015018_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} StatManagerErrFifoCtrl_t;

///  @brief 0xF5015020 - 0xF501504C : Status Manager Interrupt Vector Valid Group 0-11
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_VALID        :19;              ///<BIT [18:0] Interrupt Vector Valid Bits.
        uint32_t RF5015020_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorValidGroup_t;

///  @brief 0xF5015020 : Status Manager Interrupt Vector Valid Group 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_VALID_GRP0        :19;         ///<BIT [18:0] Interrupt Vector Valid Bits 0.
        uint32_t RF5015020_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorValidGroup0_t;

///  @brief 0xF5015024 : Status Manager Interrupt Vector Valid Group 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_VALID_GRP1        :19;         ///<BIT [18:0] Interrupt Vector Valid Bits 1.
        uint32_t RF5015024_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorValidGroup1_t;

///  @brief 0xF5015028 : Status Manager Interrupt Vector Valid Group 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_VALID_GRP2        :19;         ///<BIT [18:0] Interrupt Vector Valid Bits 2.
        uint32_t RF5015028_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorValidGroup2_t;

///  @brief 0xF501502C : Status Manager Interrupt Vector Valid Group 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_VALID_GRP3        :19;         ///<BIT [18:0] Interrupt Vector Valid Bits 3.
        uint32_t RF501502C_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorValidGroup3_t;

///  @brief 0xF5015030 : Status Manager Interrupt Vector Valid Group 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_VALID_GRP4        :19;         ///<BIT [18:0] Interrupt Vector Valid Bits 4.
        uint32_t RF5015030_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorValidGroup4_t;

///  @brief 0xF5015034 : Status Manager Interrupt Vector Valid Group 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_VALID_GRP5        :19;         ///<BIT [18:0] Interrupt Vector Valid Bits 5.
        uint32_t RF5015034_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorValidGroup5_t;

///  @brief 0xF5015038 : Status Manager Interrupt Vector Valid Group 6
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_VALID_GRP6        :19;         ///<BIT [18:0] Interrupt Vector Valid Bits 6.
        uint32_t RF5015038_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorValidGroup6_t;

///  @brief 0xF501503C : Status Manager Interrupt Vector Valid Group 7
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_VALID_GRP7        :19;         ///<BIT [18:0] Interrupt Vector Valid Bits 7.
        uint32_t RF501503C_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorValidGroup7_t;

///  @brief 0xF5015040 : Status Manager Interrupt Vector Valid Group 8
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_VALID_GRP8        :19;         ///<BIT [18:0] Interrupt Vector Valid Bits 8.
        uint32_t RF5015040_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorValidGroup8_t;

///  @brief 0xF5015044 : Status Manager Interrupt Vector Valid Group 9
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_VALID_GRP9        :19;         ///<BIT [18:0] Interrupt Vector Valid Bits 9.
        uint32_t RF5015044_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorValidGroup9_t;

///  @brief 0xF5015048 : Status Manager Interrupt Vector Valid Group 10
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_VALID_GRP10       :19;         ///<BIT [18:0] Interrupt Vector Valid Bits 10.
        uint32_t RF5015048_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorValidGroup10_t;

///  @brief 0xF501504C : Status Manager Interrupt Vector Valid Group 11
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_VALID_GRP11       :19;         ///<BIT [18:0] Interrupt Vector Valid Bits 11.
        uint32_t RF501504C_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorValidGroup11_t;

///  @brief 0xF5015050 - 0xF501507C : Status Manager Interrupt Vector Status Group 0-11
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_STAT         :19;              ///<BIT [18:0] Interrupt Vector Status Bits 0.
        uint32_t RF5015050_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorStatGroup_t;

///  @brief 0xF5015050 : Status Manager Interrupt Vector Status Group 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_STAT_GRP0         :19;         ///<BIT [18:0] Interrupt Vector Status Bits 0.
        uint32_t RF5015050_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorStatGroup0_t;

///  @brief 0xF5015054 : Status Manager Interrupt Vector Status Group 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_STAT_GRP1         :19;         ///<BIT [18:0] Interrupt Vector Status Bits 1.
        uint32_t RF5015054_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorStatGroup1_t;

///  @brief 0xF5015058 : Status Manager Interrupt Vector Status Group 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_STAT_GRP2         :19;         ///<BIT [18:0] Interrupt Vector Status Bits 2.
        uint32_t RF5015058_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorStatGroup2_t;

///  @brief 0xF501505C : Status Manager Interrupt Vector Status Group 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_STAT_GRP3         :19;         ///<BIT [18:0] Interrupt Vector Status Bits 3.
        uint32_t RF501505C_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorStatGroup3_t;

///  @brief 0xF5015060 : Status Manager Interrupt Vector Status Group 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_STAT_GRP4         :19;         ///<BIT [18:0] Interrupt Vector Status Bits 4.
        uint32_t RF5015060_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorStatGroup4_t;

///  @brief 0xF5015064 : Status Manager Interrupt Vector Status Group 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_STAT_GRP5         :19;         ///<BIT [18:0] Interrupt Vector Status Bits 5.
        uint32_t RF5015064_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorStatGroup5_t;

///  @brief 0xF5015068 : Status Manager Interrupt Vector Status Group 6
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_STAT_GRP6         :19;         ///<BIT [18:0] Interrupt Vector Status Bits 6.
        uint32_t RF5015068_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorStatGroup6_t;

///  @brief 0xF501506C : Status Manager Interrupt Vector Status Group 7
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_STAT_GRP7         :19;         ///<BIT [18:0] Interrupt Vector Status Bits 7.
        uint32_t RF501506C_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorStatGroup7_t;

///  @brief 0xF5015070 : Status Manager Interrupt Vector Status Group 8
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_STAT_GRP8         :19;         ///<BIT [18:0] Interrupt Vector Status Bits 8.
        uint32_t RF5015070_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorStatGroup8_t;

///  @brief 0xF5015074 : Status Manager Interrupt Vector Status Group 9
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_STAT_GRP9         :19;         ///<BIT [18:0] Interrupt Vector Status Bits 9.
        uint32_t RF5015074_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorStatGroup9_t;

///  @brief 0xF5015078 : Status Manager Interrupt Vector Status Group 10
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_STAT_GRP10        :19;         ///<BIT [18:0] Interrupt Vector Status Bits 10.
        uint32_t RF5015078_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorStatGroup10_t;

///  @brief 0xF501507C : Status Manager Interrupt Vector Status Group 11
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_STAT_GRP11        :19;         ///<BIT [18:0] Interrupt Vector Status Bits 11.
        uint32_t RF501507C_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorStatGroup11_t;

///  @brief 0xF5015080 - 0xF50150AC : Status Manager Interrupt Vector Coalescing Disable Group 0-11
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_COAL_DIS          :19;         ///<BIT [18:0] Interrupt Vector Coalescing Disable.
        uint32_t RF5015080_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorCoalesciDisGroup_t;

///  @brief 0xF5015080 : Status Manager Interrupt Vector Coalescing Disable Group 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_GRP0_COAL_DIS     :19;         ///<BIT [18:0] Interrupt Vector Coalescing Disable 0.
        uint32_t RF5015080_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorCoalesciDisGroup0_t;

///  @brief 0xF5015084 : Status Manager Interrupt Vector Coalescing Disable Group 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_GRP1_COAL_DIS     :19;         ///<BIT [18:0] Interrupt Vector Coalescing Disable 1.
        uint32_t RF5015084_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorCoalesciDisGroup1_t;

///  @brief 0xF5015088 : Status Manager Interrupt Vector Coalescing Disable Group 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_GRP2_COAL_DIS     :19;         ///<BIT [18:0] Interrupt Vector Coalescing Disable 2.
        uint32_t RF5015088_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorCoalesciDisGroup2_t;

///  @brief 0xF501508C : Status Manager Interrupt Vector Coalescing Disable Group 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_GRP3_COAL_DIS     :19;         ///<BIT [18:0] Interrupt Vector Coalescing Disable 3.
        uint32_t RF501508C_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorCoalesciDisGroup3_t;

///  @brief 0xF5015090 : Status Manager Interrupt Vector Coalescing Disable Group 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_GRP4_COAL_DIS     :19;         ///<BIT [18:0] Interrupt Vector Coalescing Disable 4.
        uint32_t RF5015090_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorCoalesciDisGroup4_t;

///  @brief 0xF5015094 : Status Manager Interrupt Vector Coalescing Disable Group 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_GRP5_COAL_DIS     :19;         ///<BIT [18:0] Interrupt Vector Coalescing Disable 5.
        uint32_t RF5015094_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorCoalesciDisGroup5_t;

///  @brief 0xF5015098 : Status Manager Interrupt Vector Coalescing Disable Group 6
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_GRP6_COAL_DIS     :19;         ///<BIT [18:0] Interrupt Vector Coalescing Disable 6.
        uint32_t RF5015098_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorCoalesciDisGroup6_t;

///  @brief 0xF501509C : Status Manager Interrupt Vector Coalescing Disable Group 7
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_GRP7_COAL_DIS     :19;         ///<BIT [18:0] Interrupt Vector Coalescing Disable 7.
        uint32_t RF501509C_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorCoalesciDisGroup7_t;

///  @brief 0xF50150A0 : Status Manager Interrupt Vector Coalescing Disable Group 8
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_GRP8_COAL_DIS     :19;         ///<BIT [18:0] Interrupt Vector Coalescing Disable 8.
        uint32_t RF50150A0_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorCoalesciDisGroup8_t;

///  @brief 0xF50150A4 : Status Manager Interrupt Vector Coalescing Disable Group 9
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_GRP9_COAL_DIS     :19;         ///<BIT [18:0] Interrupt Vector Coalescing Disable 9.
        uint32_t RF50150A4_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorCoalesciDisGroup9_t;

///  @brief 0xF50150A8 : Status Manager Interrupt Vector Coalescing Disable Group 10
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_GRP10_COAL_DIS    :19;         ///<BIT [18:0] Interrupt Vector Coalescing Disable 10.
        uint32_t RF50150A8_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorCoalesciDisGroup10_t;

///  @brief 0xF50150AC : Status Manager Interrupt Vector Coalescing Disable Group 11
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IV_GRP11_COAL_DIS    :19;         ///<BIT [18:0] Interrupt Vector Coalescing Disable 11.
        uint32_t RF50150AC_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} StatManagerIntrVectorCoalesciDisGroup11_t;

///  @brief 0xF50150F0 : Status Manager Aggregation Monitor
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CUR_TS_AGG          : 8;         ///<BIT [7:0] Current Time Stamp for the Interrupt Aggregation.
        uint32_t RF50150F0_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} StatManagerAggregatMonitor_t;

///  @brief 0xF50150F4 : Status Manager Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t STSM_MAX_RETRY      : 3;         ///<BIT [2:0] Status Manager Retry Threshold.
        uint32_t STSM_CACHE_EN       : 1;         ///<BIT [3] Status Manager Cache Enable.
        uint32_t STSM_AUTO_ABM_CLR_EN: 1;         ///<BIT [4] Status Manager AUTO_ABM_CLR_EN
        uint32_t RF50150F4_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} StatManagerCtrl_t;

///  @brief 0xF5015100 - 0xF5015108: Status Manager Interrupt Aggregation Threshold for Function 0/1/2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INTR_AGG_CNTR_THR    : 8;         ///<BIT [7:0] Interrupt Aggregation Counter Threshold 0.
        uint32_t INT_AGG_TMR_THR      : 8;         ///<BIT [15:8] Interrupt Aggregation Timer Threshold 0.
        uint32_t RF5015100_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerIntrAggregatThr_t;

///  @brief 0xF5015100 : Status Manager Interrupt Aggregation Threshold for Function 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_THR0     : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Threshold 0.
        uint32_t INTR_AGG_CNTR_THR0   : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Threshold 0.
        uint32_t RF5015100_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerIntrAggregatThrForFunc0_t;

///  @brief 0xF5015104 : Status Manager Interrupt Aggregation Threshold for Function 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_THR1     : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Threshold 1.
        uint32_t INTR_AGG_CNTR_THR1   : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Threshold 1.
        uint32_t RF5015104_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerIntrAggregatThrForFunc1_t;

///  @brief 0xF5015108 : Status Manager Interrupt Aggregation Threshold for Function 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_THR2     : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Threshold 2.
        uint32_t INTR_AGG_CNTR_THR2   : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Threshold 2.
        uint32_t RF5015108_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerIntrAggregatThrForFunc2_t;

///  @brief 0xF5015200 - 0xF5015248 : Status Manager Aggregation Status for Interrupt Vector 0-18
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT     : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status.
        uint32_t INT_AGG_CNTR_STAT    : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status.
        uint32_t RF5015200_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector_t;

///  @brief 0xF5015200 : Status Manager Aggregation Status for Interrupt Vector 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT0    : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 0.
        uint32_t INT_AGG_CNTR_STAT0   : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 0.
        uint32_t RF5015200_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector0_t;

///  @brief 0xF5015204 : Status Manager Aggregation Status for Interrupt Vector 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT1    : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 1.
        uint32_t INT_AGG_CNTR_STAT1   : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 1.
        uint32_t RF5015204_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector1_t;

///  @brief 0xF5015208 : Status Manager Aggregation Status for Interrupt Vector 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT2    : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 2.
        uint32_t INT_AGG_CNTR_STAT2   : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 2.
        uint32_t RF5015208_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector2_t;

///  @brief 0xF501520C : Status Manager Aggregation Status for Interrupt Vector 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT3    : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 3.
        uint32_t INT_AGG_CNTR_STAT3   : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 3.
        uint32_t RF501520C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector3_t;

///  @brief 0xF5015210 : Status Manager Aggregation Status for Interrupt Vector 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT4    : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 4.
        uint32_t INT_AGG_CNTR_STAT4   : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 4.
        uint32_t RF5015210_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector4_t;

///  @brief 0xF5015214 : Status Manager Aggregation Status for Interrupt Vector 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT5    : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 5.
        uint32_t INT_AGG_CNTR_STAT5   : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 5.
        uint32_t RF5015214_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector5_t;

///  @brief 0xF5015218 : Status Manager Aggregation Status for Interrupt Vector 6
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT6    : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 6.
        uint32_t INT_AGG_CNTR_STAT6   : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 6.
        uint32_t RF5015218_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector6_t;

///  @brief 0xF501521C : Status Manager Aggregation Status for Interrupt Vector 7
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT7    : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 7.
        uint32_t INT_AGG_CNTR_STAT7   : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 7.
        uint32_t RF501521C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector7_t;

///  @brief 0xF5015220 : Status Manager Aggregation Status for Interrupt Vector 8
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT8    : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 8.
        uint32_t INT_AGG_CNTR_STAT8   : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 8.
        uint32_t RF5015220_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector8_t;

///  @brief 0xF5015224 : Status Manager Aggregation Status for Interrupt Vector 9
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT9    : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 9.
        uint32_t INT_AGG_CNTR_STAT9   : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 9.
        uint32_t RF5015224_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector9_t;

///  @brief 0xF5015228 : Status Manager Aggregation Status for Interrupt Vector 10
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT10   : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 10.
        uint32_t INT_AGG_CNTR_STAT10  : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 10.
        uint32_t RF5015228_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector10_t;

///  @brief 0xF501522C : Status Manager Aggregation Status for Interrupt Vector 11
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT11   : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 11.
        uint32_t INT_AGG_CNTR_STAT11  : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 11.
        uint32_t RF501522C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector11_t;

///  @brief 0xF5015230 : Status Manager Aggregation Status for Interrupt Vector 12
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT12   : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 12.
        uint32_t INT_AGG_CNTR_STAT12  : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 12.
        uint32_t RF5015230_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector12_t;

///  @brief 0xF5015234 : Status Manager Aggregation Status for Interrupt Vector 13
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT13   : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 13.
        uint32_t INT_AGG_CNTR_STAT13  : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 13.
        uint32_t RF5015234_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector13_t;

///  @brief 0xF5015238 : Status Manager Aggregation Status for Interrupt Vector 14
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT14   : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 14.
        uint32_t INT_AGG_CNTR_STAT14  : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 14.
        uint32_t RF5015238_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector14_t;

///  @brief 0xF501523C : Status Manager Aggregation Status for Interrupt Vector 15
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT15   : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 15.
        uint32_t INT_AGG_CNTR_STAT15  : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 15.
        uint32_t RF501523C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector15_t;

///  @brief 0xF5015240 : Status Manager Aggregation Status for Interrupt Vector 16
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT16   : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 16.
        uint32_t INT_AGG_CNTR_STAT16  : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 16.
        uint32_t RF5015240_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector16_t;

///  @brief 0xF5015244 : Status Manager Aggregation Status for Interrupt Vector 17
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT17   : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 17.
        uint32_t INT_AGG_CNTR_STAT17  : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 17.
        uint32_t RF5015244_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector17_t;

///  @brief 0xF5015248 : Status Manager Aggregation Status for Interrupt Vector 18
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_AGG_TMR_STAT18   : 8;         ///<BIT [7:0] Interrupt Aggregation Timer Status 18.
        uint32_t INT_AGG_CNTR_STAT18  : 8;         ///<BIT [15:8] Interrupt Aggregation Counter Status 18.
        uint32_t RF5015248_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} StatManagerAggregatStatForIntrVector18_t;

/// @brief 0xF5015000-0xF501524C : NvmeStatMngr_t MODULE
typedef struct
{
    uint32_t statManagerFifoWritePort;                                                   ///< 0xF5015000 : Status Manager FIFO Write Port
    StatManagerFifoStat_t statManagerFifoStat;                                           ///< 0xF5015004 : Status Manager FIFO Status
    StatManagerFifoCtrl_t statManagerFifoCtrl;                                           ///< 0xF5015008 : Status Manager FIFO Control
    uint32_t MAN_CQ_REQ_DONE_BM;                                                         ///< 0xF501500C : Status Manager Manual Request Done Bitmap
    uint32_t ERR_RPT_FIFO_RD_PORT;                                                       ///< 0xF5015010 : Status Manager Error FIFO Read Port
    StatManagerErrFifoStat_t statManagerErrFifoStat;                                     ///< 0xF5015014 : Status Manager Error FIFO Status
    StatManagerErrFifoCtrl_t statManagerErrFifoCtrl;                                     ///< 0xF5015018 : Status Manager Error FIFO Control
    uint8_t rsvd_RF501501C[4];                                                           ///< 0xF501501C - 0xF501501F : Reserved.
    StatManagerIntrVectorValidGroup_t statManagerIntrVectorValidGroup[12];               ///< 0xF5015020 - 0xF501504C : Status Manager Interrupt Vector Valid Group 0-11
    StatManagerIntrVectorStatGroup_t statManagerIntrVectorStatGroup[12];                 ///< 0xF5015050 - 0xF501507C : Status Manager Interrupt Vector Status Group 0-11
    StatManagerIntrVectorCoalesciDisGroup_t statManagerIntrVectorCoalesciDisGroup[12];   ///< 0xF5015080 - 0xF50150AC : Status Manager Interrupt Vector Coalescing Disable Group 0-11
    uint8_t rsvd_RF50150B0[64];                                                          ///< 0xF50150B0 - 0xF50150EF : Reserved.
    StatManagerAggregatMonitor_t statManagerAggregatMonitor;                             ///< 0xF50150F0 : Status Manager Aggregation Monitor
    StatManagerCtrl_t statManagerCtrl;                                                   ///< 0xF50150F4 : Status Manager Control
    uint8_t rsvd_RF50150F8[8];                                                           ///< 0xF50150F8 - 0xF50150FF : Reserved.
    StatManagerIntrAggregatThr_t statManagerIntrAggregatThr[3];                          ///< 0xF5015100 - 0xF5015108: Status Manager Interrupt Aggregation Threshold for Function 0/1/2
    uint8_t rsvd_RF501510C[244];                                                         ///< 0xF501510C - 0xF50151FF : Reserved.
    StatManagerAggregatStatForIntrVector_t statManagerAggregatStatForIntrVector[19];     ///< 0xF5015200 - 0xF5015248 : Status Manager Aggregation Status for Interrupt Vector 0-18
    uint32_t rsvd_R1001524C;                                                             ///< 0xF501524C : Reserved.
    uint32_t STSM_DBG_PRP_FETCH_SEQID_VALID;                                             ///< 0xF5015250 : STSM_DBG_PRP_FETCH_SEQID_VALID
    uint32_t STSM_DBG_PRP_FETCH_CURPRP_FSM;                                              ///< 0xF5015254 : STSM_DBG_PRP_FETCH_CURPRP_FSM
    uint32_t STSM_DBG_PRP_FETCH_NEXPRP_FSM;                                              ///< 0xF5015258 : STSM_DBG_PRP_FETCH_NEXPRP_FSM
    uint32_t STSM_DBG_PRP_FETCH_GETPRP_FSM;                                              ///< 0xF5001525C : STSM_DBG_PRP_FETCH_GETPRP_FSM
    uint32_t STSM_DBG_PKT_CTRL_MAIN_FSM;                                                 ///< 0xF5015260 : STSM_DBG_PKT_CTRL_MAIN_FSM
    uint32_t STSM_DBG_PKT_CTRL_AUTO_CQ_COUNT;                                            ///< 0xF5015264 : STSM_DBG_PKT_CTRL_AUTO_CQ_COUNT
    uint32_t STSM_DBG_PKT_CTRL_MANU_CQ_COUNT;                                            ///< 0xF5015268 : STSM_DBG_PKT_CTRL_MANU_CQ_COUNT
    uint32_t STSM_DBG_INT_MAN_CQ_STS_FSM;                                                ///< 0xF501526C : STSM_DBG_INT_MAN_CQ_STS_FSM
    uint32_t STSM_DBG_INT_MAN_MSI_STS_FSM;                                               ///< 0xF5015270 : STSM_DBG_INT_MAN_MSI_STS_FSM
    uint32_t STSM_DBG_INT_MAN_TIMER_STS_FSM;                                             ///< 0xF5015274 : STSM_DBG_INT_MAN_TIMER_STS_FSM
} NvmeStatMngr_t;
COMPILE_ASSERT(offsetof(NvmeStatMngr_t,statManagerAggregatStatForIntrVector[18])==0xF5015248-0xF5015000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile NvmeStatMngr_t rNvmeStatMngr;        ///< 0xF5015000-0xF501524C : NvmeStatMngr_t MODULE

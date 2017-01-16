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
//! @brief NVMe Controller Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  NVMe Controller Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF5000000 : Control Register Module Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_ARB_MD         : 2;         ///<BIT [1:0] Control Register Arbitration Mode.
        uint32_t RF5000000_3_2_RSVD  : 2;         ///<BIT [3:2] Reserved.
        uint32_t ZEROIZE_MD          : 1;         ///<BIT [4] Memory Zeroize Mode.
        uint32_t RF5000000_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} CtrlRegisterModuleCfg_t;

///  @brief 0xF5000004 : Control Register Module Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FORCE_R_PERR        : 2;         ///<BIT [1:0] Force Memory Read Parity Error.
        uint32_t RF5000004_3_2_RSVD  : 2;         ///<BIT [3:2] Reserved.
        uint32_t IGNR_R_PERR         : 2;         ///<BIT [5:4] Ignore Memory Read Parity Error.
        uint32_t RF5000004_31_6_RSVD :26;         ///<BIT [31:6] Reserved.
    } b;    
} CtrlRegisterModuleCtrl_t;

///  @brief 0xF5000008 : Control Register Module Command
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MEM_ZERO_START      : 1;         ///<BIT [0] Memory Zeroize Start.
        uint32_t CXT_RSTR_START      : 1;         ///<BIT [1] Context Restore Start.
        uint32_t CXT_SAVE_START      : 1;         ///<BIT [2] Context Save Start.
        uint32_t RF5000008_3_RSVD    : 1;         ///<BIT [3] Reserved.
        uint32_t AHB_SHUT_START      : 1;         ///<BIT [4] AHB Shutdown Start.
        uint32_t RF5000008_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} CtrlRegisterModuleCmd_t;

///  @brief 0xF500000C : Control Register Module Engine Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ENG_STAT_ZERO_DONE   : 1;         ///<BIT [0] Zeroize Done.
        uint32_t CXT_STAT             : 3;         ///<BIT [3:1] Context Save and Restore Status.
        uint32_t AHB_SHUT_DONE        : 1;         ///<BIT [4] AHB Shutdown Done.
        uint32_t RF500000C_15_5_RSVD  :11;         ///<BIT [15:5] Reserved.
        uint32_t CPU_ACCEL_STATE      : 3;         ///<BIT [18:16] CPU Memory Accelerator State.
        uint32_t RF500000C_23_19_RSVD : 5;         ///<BIT [23:19] Reserved.
        uint32_t HOST_ACCEL_STATE     : 8;         ///<BIT [31:24] Host Memory Accelerator State.
    } b;    
} CtrlRegisterModuleEngineStat_t;

///  @brief 0xF5000010 : Control Register Module Function Region Size
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FUNC_REGION_SZ      : 2;         ///<BIT [1:0] Function Region Size.
        uint32_t RF5000010_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} CtrlRegisterModuleFuncRegisterionSize_t;

///  @brief 0xF5000030 : Control Register Module Error FIFO Count
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_ERR_FIFO_CTR   : 3;         ///<BIT [2:0] Control Register Monitor Error FIFO Status Counter.
        uint32_t RF5000030_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} CtrlRegisterModuleErrFifoCnt_t;

///  @brief 0xF5000040 - 0xF500004B: Control Register Module Queue Allocation Function
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FTBI                 : 5;         ///<BIT [4:0] Function Table Entry Base Index for Function.
        uint32_t RF5000040_15_5_RSVD  :11;         ///<BIT [15:5] Reserved.
        uint32_t FTS                  : 5;         ///<BIT [20:16] Function Table Size for Functio.
        uint32_t RF5000040_31_21_RSVD :11;         ///<BIT [31:21] Reserved.
    } b;    
} CtrlRegisterModuleQueueAllocatiFunc_t;

///  @brief 0xF5000040 : Control Register Module Queue Allocation Function 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FTBI_FN0             : 5;         ///<BIT [4:0] Function Table Entry Base Index for Function 0.
        uint32_t RF5000040_15_5_RSVD  :11;         ///<BIT [15:5] Reserved.
        uint32_t FTS_FN0              : 5;         ///<BIT [20:16] Function Table Size for Function 0.
        uint32_t RF5000040_31_21_RSVD :11;         ///<BIT [31:21] Reserved.
    } b;    
} CtrlRegisterModuleQueueAllocatiFunc0_t;

///  @brief 0xF5000044 : Control Register Module Queue Allocation Function 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FTBI_FN1             : 5;         ///<BIT [4:0] Function Table Entry Base Index for Function 1.
        uint32_t RF5000044_15_5_RSVD  :11;         ///<BIT [15:5] Reserved.
        uint32_t FTS_FN1              : 5;         ///<BIT [20:16] Function Table Size for Function 1.
        uint32_t RF5000044_31_21_RSVD :11;         ///<BIT [31:21] Reserved.
    } b;    
} CtrlRegisterModuleQueueAllocatiFunc1_t;

///  @brief 0xF5000048 : Control Register Module Queue Allocation Function 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FTBI_FN2             : 5;         ///<BIT [4:0] Function Table Entry Base Index for Function 2.
        uint32_t RF5000048_15_5_RSVD  :11;         ///<BIT [15:5] Reserved.
        uint32_t FTS_FN2              : 5;         ///<BIT [20:16] Function Table Size for Function 2.
        uint32_t RF5000048_31_21_RSVD :11;         ///<BIT [31:21] Reserved.
    } b;    
} CtrlRegisterModuleQueueAllocatiFunc2_t;

///  @brief 0xF5000150 : Control Register Module Access Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_MEM_LCK        : 1;         ///<BIT [0] Memory Lock.
        uint32_t CREG_MEM_MD         : 1;         ///<BIT [1] Memory Mode.
        uint32_t RF5000150_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} CtrlRegisterModuleAccessCtrl_t;

///  @brief 0xF5000154 : Control Register Module Access Status 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_ERR_ADDR       : 8;         ///<BIT [7:0] Control Register Module Error Address.
        uint32_t RF5000154_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} CtrlRegisterModuleAccessStat0_t;

///  @brief 0xF5000158 : Control Register Module Access Status 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_ERR_STS        : 2;         ///<BIT [1:0] Control Register Module Error Status.
        uint32_t RF5000158_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} CtrlRegisterModuleAccessStat1_t;

///  @brief 0xF5000160 : Control Register Module Access Status Read Parity
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_PAR            : 8;         ///<BIT [7:0] Control Register Module Parity Bit.
        uint32_t RF5000160_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} CtrlRegisterModuleAccessStatRdPar_t;

///  @brief 0xF500016C : Control Register Module Monitor Select
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_MON_SEL        : 3;         ///<BIT [2:0] Control Register Module Monitor Signal Select.
        uint32_t RF500016C_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} CtrlRegisterModuleMonitorSel_t;

///  @brief 0xF5000170 : Control Register Module Indirect Access Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_IND_ACC_ADDR   : 9;         ///<BIT [8:0] Control Register Monitor Indirect Access Address.
        uint32_t RF5000170_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;    
} CtrlRegisterModuleIndAccessAddr_t;


/// @brief 0xF5000000-0xF5000184 : Nvme_t MODULE
typedef struct
{
    CtrlRegisterModuleCfg_t ctrlRegisterModuleCfg;                                   ///< 0xF5000000 : Control Register Module Configuration
    CtrlRegisterModuleCtrl_t ctrlRegisterModuleCtrl;                                 ///< 0xF5000004 : Control Register Module Control
    CtrlRegisterModuleCmd_t ctrlRegisterModuleCmd;                                   ///< 0xF5000008 : Control Register Module Command
    CtrlRegisterModuleEngineStat_t ctrlRegisterModuleEngineStat;                     ///< 0xF500000C : Control Register Module Engine Status
    CtrlRegisterModuleFuncRegisterionSize_t ctrlRegisterModuleFuncRegisterionSize;   ///< 0xF5000010 : Control Register Module Function Region Size
    uint8_t rsvd_RF5000014[28];                                                      ///< 0xF5000014 - 0xF500002F : Reserved.
    CtrlRegisterModuleErrFifoCnt_t ctrlRegisterModuleErrFifoCnt;                     ///< 0xF5000030 : Control Register Module Error FIFO Count
    uint32_t CREG_ERR_FIFO_DW[2];                                                    ///< 0xF5000034 - 0xF5000038 : Control Register Module Error FIFO DW0/1
    uint8_t rsvd_RF500003C[4];                                                       ///< 0xF500003C - 0xF500003F : Reserved.
    CtrlRegisterModuleQueueAllocatiFunc_t ctrlRegisterModuleQueueAllocatiFunc[3];    ///< 0xF5000040 - 0xF500004B : Control Register Module Queue Allocation Function 0/1/2
    uint8_t rsvd_RF500004C[260];                                                     ///< 0xF500004C - 0xF500014F : Reserved.
    CtrlRegisterModuleAccessCtrl_t ctrlRegisterModuleAccessCtrl;                     ///< 0xF5000150 : Control Register Module Access Control
    CtrlRegisterModuleAccessStat0_t ctrlRegisterModuleAccessStat0;                   ///< 0xF5000154 : Control Register Module Access Status 0
    CtrlRegisterModuleAccessStat1_t ctrlRegisterModuleAccessStat1;                   ///< 0xF5000158 : Control Register Module Access Status 1
    uint8_t rsvd_RF500015C[4];                                                       ///< 0xF500015C - 0xF500015F : Reserved.
    CtrlRegisterModuleAccessStatRdPar_t ctrlRegisterModuleAccessStatRdPar;           ///< 0xF5000160 : Control Register Module Access Status Read Parity
    uint32_t CREG_ACC_STATUS_DW[2];                                                  ///< 0xF5000164 - 0xF5000168 : Control Register Module Access Status Dword 0/1
    CtrlRegisterModuleMonitorSel_t ctrlRegisterModuleMonitorSel;                     ///< 0xF500016C : Control Register Module Monitor Select
    CtrlRegisterModuleIndAccessAddr_t ctrlRegisterModuleIndAccessAddr;               ///< 0xF5000170 : Control Register Module Indirect Access Address
    uint32_t CREG_IND_ACC_DATA;                                                      ///< 0xF5000174 : Control Register Module Indirect Access Data
    uint32_t CREG_IND_ACC_MSK;                                                       ///< 0xF5000178 : Control Register Module Indirect Access Mask
    uint8_t rsvd_RF500017C[4];                                                       ///< 0xF500017C - 0xF500017F : Reserved.
                                                                                     ///< 0xF5000180 : Control Register Module Direct Access Data
} Nvme_t;
COMPILE_ASSERT(sizeof(Nvme_t)==(0xF5000180-0xF5000000),"check the size of a register structure");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile Nvme_t rNvme;                        ///< 0xF5000000-0xF5000184 : Nvme_t MODULE

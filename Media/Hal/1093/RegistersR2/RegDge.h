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
//! @brief Descriptor Generation Engine Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  Descriptor Generation Engine Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF200E808 : DGE Status FIFO Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DGE_CMD_DONE_CNT    : 8;         ///<BIT [7:0] DGE Command Done Count.
        uint32_t RF200E808_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} DgeStatFifoCtrl_t;

///  @brief 0xF200E828 : DGE NAND Flash Address Offset
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t NAND_LUN_ADDR_OS     : 6;         ///<BIT [5:0] DGE NAND LUN Address Offset.
        uint32_t RF200E828_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint32_t NAND_BLOCK_ADDR_OS   : 6;         ///<BIT [13:8] DGE NAND Block Address Offset.
        uint32_t RF200E828_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t NAND_PAGE_ADDR_OS    : 6;         ///<BIT [21:16] DGE NAND Page Address Offset.
        uint32_t RF200E828_31_22_RSVD :10;         ///<BIT [31:22] Reserved.
    } b;    
} DgeNandFlashAddrOs_t;

///  @brief 0xF200E82C : DGE Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DGE_SOFT_RESET        : 1;         ///<BIT [0] DGE Soft Reset.
        uint32_t DGE_STOP_ON_ERR       : 1;         ///<BIT [1] DGE Stop-on-Error.
        uint32_t DGE_ABORT_ERR         : 1;         ///<BIT [2] DGE Abort Error.
        uint32_t DGE_QLF_STAT_FIFO_RDY : 1;         ///<BIT [3] DGE Qualify Status FIFO Ready.
        uint32_t RF200E82C_31_4_RSVD   :28;         ///<BIT [31:4] Reserved.
    } b;    
} DgeCtrl_t;

///  @brief 0xF200E830 : DGE Interrupt Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DGE_INT_CMD_DONE       : 1;         ///<BIT [0] DGE Command Done Interrupt.
        uint32_t RF200E830_1_RSVD       : 1;         ///<BIT [1] Reserved.
        uint32_t DGE_INT_CMD_DES_DV_ERR : 1;         ///<BIT [2] DGE Descriptor Template DV Bit Error.
        uint32_t DGE_INT_PARAM_OOB_ERR  : 1;         ///<BIT [3] DGE Parameter Out-of-Bounds Error.
        uint32_t RF200E830_31_4_RSVD    :28;         ///<BIT [31:4] Reserved.
    } b;    
} DgeIntrStat_t;

///  @brief 0xF200E834 : DGE Interrupt Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_CMD_DONE_MSK       : 1;         ///<BIT [0] Mask the DGE Descriptor Command Done Interrupt.
        uint32_t RF200E834_1_RSVD       : 1;         ///<BIT [1] Reserved.
        uint32_t INT_CMD_DES_DV_ERR_MSK : 1;         ///<BIT [2] Mask the DGE Descriptor Template DV Bit Error.
        uint32_t INT_PARAM_OOB_ERR_MSK  : 1;         ///<BIT [3] Mask the DGE Interrupt Parameter Out-of-Bounds Error.
        uint32_t RF200E834_7_4_RSVD     : 4;         ///<BIT [7:4] Reserved.
        uint32_t DGE_CMD_DONE_THR       : 8;         ///<BIT [15:8] DGE Command Done Threshold.
        uint32_t DGE_CMD_DONE_THR_EN    : 1;         ///<BIT [16] DGE Command Done Threshold Enable.
        uint32_t RF200E834_31_17_RSVD   :15;         ///<BIT [31:17] Reserved.
    } b;    
} DgeIntrCtrl_t;

///  @brief 0xF200E854 : DGE Command Memory Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DGE_CMD_SPACE_CNT   : 7;         ///<BIT [6:0] DGE Command Space Count in Dwords.
        uint32_t RF200E854_31_7_RSVD :25;         ///<BIT [31:7] Reserved.
    } b;    
} DgeCmdMemStat_t;

///  @brief 0xF200E85C : DGE Monitor Bus
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DGE_MON_BUS          :16;         ///<BIT [15:0] DGE Monitor Output.
        uint32_t RF200E85C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DgeMonitorBus_t;

///  @brief 0xF200E860 : DGE Monitor Select
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DGE_MON_SEL         : 6;         ///<BIT [5:0] DGE Monitor Output Select.
        uint32_t RF200E860_31_6_RSVD :26;         ///<BIT [31:6] Reserved.
    } b;    
} DgeMonitorSel_t;


/// @brief 0xF200E800-0xF200E900 : Dge_t MODULE
typedef struct
{
    uint32_t DGE_CMD_FIFO;                       ///< 0xF200E800 : DGE Command FIFO Access
    uint32_t DGE_STAT_FIFO;                      ///< 0xF200E804 : DGE Status FIFO Access
    DgeStatFifoCtrl_t dgeStatFifoCtrl;           ///< 0xF200E808 : DGE Status FIFO Control
    uint32_t DGE_DES_TEMPL_BASE;                 ///< 0xF200E80C : DGE Descriptor Template Base Address
    uint32_t DGE_DES_FIFO_START_ADDR;            ///< 0xF200E810 : DGE Descriptor FIFO Start Address
    uint32_t DGE_DES_FIFO_END_ADDR;              ///< 0xF200E814 : DGE Descriptor FIFO End Address
    uint32_t DGE_FAST_DES_FIFO_START_ADDR;       ///< 0xF200E818 : DGE Fast Descriptor FIFO Start Address
    uint32_t DGE_FAST_DES_FIFO_END_ADDR;         ///< 0xF200E81C : DGE Fast Descriptor FIFO End Address
    uint32_t DGE_RRD_DES_FIFO_START_ADDR;        ///< 0xF200E820 : DGE RRD Descriptor FIFO Start Address
    uint32_t DGE_RRD_DES_FIFO_END_ADDR;          ///< 0xF200E824 : DGE RRD Descriptor FIFO End Address
    DgeNandFlashAddrOs_t dgeNandFlashAddrOs;     ///< 0xF200E828 : DGE NAND Flash Address Offset
    DgeCtrl_t dgeCtrl;                           ///< 0xF200E82C : DGE Control
    DgeIntrStat_t dgeIntrStat;                   ///< 0xF200E830 : DGE Interrupt Status
    DgeIntrCtrl_t dgeIntrCtrl;                   ///< 0xF200E834 : DGE Interrupt Control
    uint8_t rsvd_RF200E838[20];                  ///< 0xF200E838 - 0xF200E84B : Reserved.
    uint32_t DGE_CUR_STAT_LOW;                   ///< 0xF200E84C : DGE Current Status, Low Word
    uint32_t DGE_CUR_STAT_HIGH;                  ///< 0xF200E850 : DGE Current Status, High Word
    DgeCmdMemStat_t dgeCmdMemStat;               ///< 0xF200E854 : DGE Command Memory Status
    uint8_t rsvd_RF200E858[4];                   ///< 0xF200E858 - 0xF200E85B : Reserved.
    DgeMonitorBus_t dgeMonitorBus;               ///< 0xF200E85C : DGE Monitor Bus
    DgeMonitorSel_t dgeMonitorSel;               ///< 0xF200E860 : DGE Monitor Select
} Dge_t;
COMPILE_ASSERT(offsetof(Dge_t,dgeMonitorSel)==0xF200E860-0xF200E800,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile Dge_t rDge;                          ///< 0xF200E800-0xF200E900 : Dge_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
extern Dge_t mDge;                          ///< 0xF200E800-0xF200E900 : Dge_t MODULE

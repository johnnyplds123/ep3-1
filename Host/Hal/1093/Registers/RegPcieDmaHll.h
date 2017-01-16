#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2014  -  All rights reserved     
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
//! @brief PCIe DMA Hardware Link List Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 7/8/2014
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  PCIe DMA Hardware Link List Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF010D024 : Hardware Link List Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_AU_MEM_BLK_SIZE   :13;         ///<BIT [12:0] Hardware Link List AU Memory Block Size.
        uint32_t HWLL_APB_ECU_ON        : 1;         ///<BIT [13] Hardware Link List APB ECU Access Port on/off.
        uint32_t HWLL_APB_HOST_ACC      : 1;         ///<BIT [14] Hardware Link List APB Host Access Port.
        uint32_t HWLL_RDDAT_RDY_BM_UPDT : 1;         ///<BIT [15] Hardware Link List Read data ready bitmap update control.
        uint32_t HWLL_REG_PRI           : 1;         ///<BIT [16] Hardware Link List Internal and External Memory Region Priority.
        uint32_t HWLL_LNK_SRCH_MD       : 1;         ///<BIT [17] Hardware Link List Link Search Mode.
        uint32_t HWLL_NUM_LINK          : 8;         ///<BIT [25:18] Hardware Link List Number Link Entry.
        uint32_t RF010D024_31_26_RSVD   : 6;         ///<BIT [31:26] Reserved.
    } b;    
} HardwareLnkListCfg00_t;

///  @brief 0xF010D028 : Hardware Link List ECU Access Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_ECU_REQ          : 1;         ///<BIT [0] Hardware Link List ECU Request.
        uint32_t HWLL_ABT_REQ          : 1;         ///<BIT [1] Hardware Link List ECU Abort Request.
        uint32_t HWLL_WR_XFER_DONE     : 1;         ///<BIT [2] Hardware Link List ECU Write Transfer Done.
        uint32_t HWLL_ADDR_VLD_REQ_ACK : 1;         ///<BIT [3] Hardware Link List Address Valid Request ACK.
        uint32_t HWLL_WR_XFER_DONE_ACK : 1;         ///<BIT [4] Hardware Link List ECU Write Transfer Done ACK.
        uint32_t HWLL_ABRT_REQ_ACK     : 1;         ///<BIT [5] Hardware Link List Abort Request ACK.
        uint32_t FW_INIT_HT_REQ        : 1;         ///<BIT [6] Firmware Head/Tail Table Initialization Request.
        uint32_t FW_INIT_HT_DONE       : 1;         ///<BIT [7] Firmware Head/Tail Initialization Done.
        uint32_t ECU_ACK_STAT          :11;         ///<BIT [18:8] ECU ACK Status.
        uint32_t RF010D028_31_19_RSVD  :13;         ///<BIT [31:19] Reserved.
    } b;    
} HardwareLnkListEcuAccessCtrl000_t;

///  @brief 0xF010D02C : Hardware Link List ECU Access Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_AU_IDX_ECU      :16;         ///<BIT [15:0] Hardware Link List AU Index for ECU.
        uint32_t HWLL_CMD_IDX_ECU     : 9;         ///<BIT [24:16] Hardware Link List Command Index for ECU.
        uint32_t RF010D02C_31_25_RSVD : 7;         ///<BIT [31:25] Reserved.
    } b;    
} HardwareLnkListEcuAccessCtrl101_t;

///  @brief 0xF010D030 : Hardware Link List ECU Access Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_ART_PTR        : 6;         ///<BIT [5:0] Hardware Link List Active Request Table Pointer.
        uint32_t RF010D030_31_6_RSVD :26;         ///<BIT [31:6] Reserved.
    } b;    
} HardwareLnkListEcuAccessCtrl202_t;

///  @brief 0xF010D034 : Hardware Link List Host Access Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_HOST_REQ              : 1;         ///<BIT [0] Hardware Link List Host Request.
        uint32_t HWLL_HOST_LNK_KILL_REQ     : 1;         ///<BIT [1] Hardware Link List Host Link Kill Request.
        uint32_t HWLL_HOST_INT_EXT_ADDR     : 1;         ///<BIT [2] Hardware Link List Internal or External Address.
        uint32_t HWLL_HOST_ADDR_VLD_REQ_ACK : 1;         ///<BIT [3] Hardware Link List Host Address Valid Request ACK.
        uint32_t HWLL_HOST_RXFER_DONE       : 1;         ///<BIT [4] Hardware Link List Host Read Transfer Done.
        uint32_t HWLL_HOST_ABRT_REQ         : 1;         ///<BIT [5] Hardware Link List Host Abort Request.
        uint32_t HWLL_HOST_RXFER_DONE_ACK   : 1;         ///<BIT [6] Hardware Link List Host Read Transfer Done ACK.
        uint32_t HWLL_HOST_ABRT_REQ_ACK     : 1;         ///<BIT [7] Hardware Link List Host Abort Request ACK.
        uint32_t HWLL_HOST_ART_PTR          : 2;         ///<BIT [9:8] Hardware Link List Host Active Request Table Pointer.
        uint32_t HWLL_HOST_ACK_STAT         : 3;         ///<BIT [12:10] Hardware Link List Host ACK Status.
        uint32_t RF010D034_15_13_RSVD       : 3;         ///<BIT [15:13] Reserved.
        uint32_t HWLL_HOST_MD1_AU_IDX       :16;         ///<BIT [31:16] Hardware Link List Host Mode 1 AU index.
    } b;    
} HardwareLnkListHostAccessCtrl000_t;

///  @brief 0xF010D038 : Hardware Link List Host Access Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_HOST_AU_IDX     :16;         ///<BIT [15:0] Host Request AU Index.
        uint32_t HWLL_HOST_CMD_IDX    : 9;         ///<BIT [24:16] Host Command Index.
        uint32_t RF010D038_31_25_RSVD : 7;         ///<BIT [31:25] Reserved.
    } b;    
} HardwareLnkListHostAccessCtrl101_t;

///  @brief 0xF010D040 : Hardware Link List Table Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_FW_LNK_AVL_SET      : 1;         ///<BIT [0] Hardware Link List Firmware Link Available Bit Set Request.
        uint32_t HWLL_FW_LNK_AVL_RST      : 1;         ///<BIT [1] Hardware Link List Firmware Link Reset Request.
        uint32_t HWLL_FW_LNK_AVL_SET_GNT  : 1;         ///<BIT [2] Hardware Link List Firmware Link Available Set Request Grant.
        uint32_t HWLL_FW_LNK_AVL_RST_GNT  : 1;         ///<BIT [3] Hardware Link List Firmware Link Available Reset Request Grant.
        uint32_t HWLL_FW_ART_AVL_SET      : 1;         ///<BIT [4] Hardware Link List Firmware ECU Active Request Table Set Reques.
        uint32_t HWLL_FW_ART_AVL_RST      : 1;         ///<BIT [5] Hardware Link List Firmware ECU Active Request Table Reset Requ.
        uint32_t HWLL_FW_ART_AVL_SET_GNT  : 1;         ///<BIT [6] Hardware Link List Firmware ECU Active Request Table Set Reques.
        uint32_t HWLL_FW_ART_AVL_RST_GNT  : 1;         ///<BIT [7] Hardware Link List Firmware ECU Active Request Table Reset Requ.
        uint32_t HWLL_FW_HART_AVL_SET     : 1;         ///<BIT [8] Hardware Link List Firmware Host Active Req Table Set Request.
        uint32_t HWLL_FW_HART_AVL_RST     : 1;         ///<BIT [9] Hardware Link List Firmware Host Active Request Table Reset Req.
        uint32_t HWLL_FW_HART_AVL_SET_GNT : 1;         ///<BIT [10] Hardware Link List Firmware Host Active Request Table Set Reque.
        uint32_t HWLL_FW_HART_AVL_RST_GNT : 1;         ///<BIT [11] Hardware Link List Firmware Host Active Request Table Reset Req.
        uint32_t HWLL_FW_HART_AVL_ADDR    : 2;         ///<BIT [13:12] Hardware Link List Firmware Host Active Request Table Address.
        uint32_t RF010D040_15_14_RSVD     : 2;         ///<BIT [15:14] Reserved.
        uint32_t HWLL_FW_ART_AVL_ADDR     : 6;         ///<BIT [21:16] Hardware Link List Firmware Active Request Table Address.
        uint32_t RF010D040_23_22_RSVD     : 2;         ///<BIT [23:22] Reserved.
        uint32_t HWLL_FW_LNK_VLD_ADDR     : 7;         ///<BIT [30:24] Hardware Link List Firmware Link Valid Set Reset Address.
        uint32_t RF010D040_31_RSVD        : 1;         ///<BIT [31] Reserved.
    } b;    
} HardwareLnkListTblCtrl00_t;

///  @brief 0xF010D04C : Hardware Link List Error FIFO Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010D04C_0_RSVD    : 1;         ///<BIT [0] Reserved.
        uint32_t HWLL_ERR_FIFO_PUSH  : 1;         ///<BIT [1] Hardware Link List Error FIFO Push.
        uint32_t HWLL_ERR_FIFO_POP   : 1;         ///<BIT [2] Hardware Link List Error FIFO Pop.
        uint32_t HWLL_ERR_FIFO_FULL  : 1;         ///<BIT [3] Hardware Link List Error FIFO Full.
        uint32_t HWLL_ERR_FIFO_EMPTY : 1;         ///<BIT [4] Hardware Link List Error FIFO Empty.
        uint32_t HWLL_ERR_FIFO_CNT   : 3;         ///<BIT [7:5] Hardware Link List Error FIFO Count.
        uint32_t HWLL_ERR_INT_EN     : 1;         ///<BIT [8] Hardware Link List Error Interrupt Enable.
        uint32_t RF010D04C_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;    
} HardwareLnkListErrFifoCtrl00_t;

///  @brief 0xF010D050 : Hardware Link List Memory Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_ECC_ERR_INJ_MD  : 1;         ///<BIT [0] Hardware Link List ECC Error Injection Mode.
        uint32_t HWLL_ECC_CHK_ART     : 1;         ///<BIT [1] Hardware Link List ECC Check on ECU Active Request Table Memory.
        uint32_t HWLL_ECC_CHK_HT      : 1;         ///<BIT [2] Hardware Link List ECC Check on Head/Tail Memory.
        uint32_t HWLL_ECC_CHK_LA      : 1;         ///<BIT [3] Hardware Link List ECC Check on Link Array Memory.
        uint32_t HWLL_ECC_1BIT_EN_ART : 1;         ///<BIT [4] Hardware Link List ECC 1-Bit Error Report for Active Request Ta.
        uint32_t HWLL_ECC_1BIT_EN_HT  : 1;         ///<BIT [5] Hardware Link List ECC 1-Bit Error Report for Head/Tail Memory.
        uint32_t HWLL_ECC_1BIT_EN_LA  : 1;         ///<BIT [6] Hardware Link List ECC 1-Bit Error Report for Link Array Memory.
        uint32_t HWLL_ARTMEM_CPU_ACC  : 1;         ///<BIT [7] Hardware Link List Active Request Table Memory CPU Access.
        uint32_t HWLL_HTMEM_CPU_ACC   : 1;         ///<BIT [8] Hardware Link List Head/Tail Memory CPU Access.
        uint32_t HWLL_LAMEM_CPU_ACC   : 1;         ///<BIT [9] Hardware Link List Link Array Memory CPU Access.
        uint32_t RF010D050_31_10_RSVD :22;         ///<BIT [31:10] Reserved.
    } b;    
} HardwareLnkListMemCtrl00_t;

///  @brief 0xF010D054 : Hardware Link List Memory Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_ERR_INJ_ART    : 2;         ///<BIT [1:0] Hardware Link List Error Injection for Active Request Table Mem.
        uint32_t HWLL_ERR_INJ_HT     : 2;         ///<BIT [3:2] Hardware Link List Error Injection for Head/Tail Memory.
        uint32_t HWLL_ERR_INJ_LA     : 2;         ///<BIT [5:4] Hardware Link List Error Injection for Link Array Memory.
        uint32_t RF010D054_31_6_RSVD :26;         ///<BIT [31:6] Reserved.
    } b;    
} HardwareLnkListMemCfg00_t;

///  @brief 0xF010D05C : Hardware Link List Active Request Table Memory Access Data 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_ARTMEM_RAW_RDAT : 2;         ///<BIT [1:0] Hardware Link List Active Request Memory Raw Read Data 2.
        uint32_t HWLL_ARTMEM_PAR_RDAT : 8;         ///<BIT [9:2] Hardware Link List Active Request Table Memory Parity Read Data.
        uint32_t RF010D05C_31_10_RSVD :22;         ///<BIT [31:10] Reserved.
    } b;    
} HardwareLnkListActiveReqTblMemAccess11_t;

///  @brief 0xF010D060 : Hardware Link List Head Tail Memory Access Data
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_HTMEM_RAW_RDAT  :16;         ///<BIT [15:0] Hardware Link List Head/Tail Memory Raw Read Data.
        uint32_t HWLL_HTMEM_PAR_RDAT  : 6;         ///<BIT [21:16] Hardware Link List Head/Tail Memory Parity Read Data.
        uint32_t RF010D060_31_22_RSVD :10;         ///<BIT [31:22] Reserved.
    } b;    
} HardwareLnkListHeadTailMemAccessData00_t;

///  @brief 0xF010D068 : Hardware Link List Link Array Memory Access Data 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_LAMEM_RAW_RDAT  : 2;         ///<BIT [1:0] Hardware Link List Link Array Memory Raw Read Data.
        uint32_t HWLL_LAMEM_PAR_RDAT  : 8;         ///<BIT [9:2] Hardware Link List Link Array Memory Parity Read Data.
        uint32_t RF010D068_31_10_RSVD :22;         ///<BIT [31:10] Reserved.
    } b;    
} HardwareLnkListLnkArrayMemAccessData11_t;

///  @brief 0xF010D070 : Hardware Link List Monitor Signal Select
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_MON_SEL        : 3;         ///<BIT [2:0] Hardware Link List Monitor Signal Select.
        uint32_t RF010D070_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} HardwareLnkListMonitorSgnSel00_t;

///  @brief 0xF010D080 : Hardware Link List Host Active Request Table Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HART_ADDR           : 2;         ///<BIT [1:0] Host Active Request Table Address.
        uint32_t RF010D080_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} HardwareLnkListHostActiveReqTblAddr00_t;

///  @brief 0xF010D084 : Hardware Link List Host Active Request Table Access 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_NXT_PTR         : 8;         ///<BIT [7:0] Hardware Link List Next Pointer.
        uint32_t RF010D084_9_8_RSVD   : 2;         ///<BIT [9:8] Reserved.
        uint32_t HWLL_CUR_PTR         : 8;         ///<BIT [17:10] Hardware Link List Current Pointer.
        uint32_t RF010D084_19_18_RSVD : 2;         ///<BIT [19:18] Reserved.
        uint32_t HWLL_PRE_PTR         : 8;         ///<BIT [27:20] Hardware Link List Previous Pointer.
        uint32_t RF010D084_29_28_RSVD : 2;         ///<BIT [29:28] Reserved.
        uint32_t HWLL_LAST_LNK        : 1;         ///<BIT [30] Hardware Link List Last Link.
        uint32_t HWLL_HART_VAL        : 1;         ///<BIT [31] Hardware Link List Host Active Request Table Valid Bit.
    } b;    
} HardwareLnkListHostActiveReqTblAccess00_t;

///  @brief 0xF010D088 : Hardware Link List Host Active Request Table Access 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_CMD_IDX_ART    : 9;         ///<BIT [8:0] Hardware Link List Command Index for Active Request Table.
        uint32_t RF010D088_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
        uint32_t HWLL_AU_IDX_ART     :16;         ///<BIT [31:16] Hardware Link List AU Index for Active Request Table.
    } b;    
} HardwareLnkListHostActiveReqTblAccess11_t;


/// @brief 0xF010D000-0xF010D090 : PcieDmaHll_t MODULE
typedef struct
{
    uint32_t HWLL_LNK_AVL_0_31;                                                             ///< 0xF010D000 : Hardware Link List Link Available Slots 0
    uint32_t HWLL_LNK_AVL_32_63;                                                            ///< 0xF010D004 : Hardware Link List Link Available Slots 1
    uint32_t HWLL_LNK_AVL_64_95;                                                            ///< 0xF010D008 : Hardware Link List Link Available Slots 3
    uint32_t HWLL_LNK_AVL_96_127;                                                           ///< 0xF010D00C : Hardware Link List Link Available Slots 4
    uint32_t HWLL_INT_MEM_BASE_ADDR;                                                        ///< 0xF010D010 : Hardware Link List Internal Memory Base Address
    uint32_t HWLL_EXT_MEM_BASE_ADDR;                                                        ///< 0xF010D014 : Hardware Link List External Memory Base Address
    uint32_t HWLL_WR_DEST_ADDR;                                                             ///< 0xF010D018 : Hardware Link List ECU Access Address
    uint8_t rsvd_RF010D01C[8];                                                              ///< 0xF010D01C - 0xF010D023 : Reserved.
    HardwareLnkListCfg00_t hardwareLnkListCfg0_0;                                           ///< 0xF010D024 : Hardware Link List Configuration
    HardwareLnkListEcuAccessCtrl000_t hardwareLnkListEcuAccessCtrl00_0;                     ///< 0xF010D028 : Hardware Link List ECU Access Control 0
    HardwareLnkListEcuAccessCtrl101_t hardwareLnkListEcuAccessCtrl10_1;                     ///< 0xF010D02C : Hardware Link List ECU Access Control 1
    HardwareLnkListEcuAccessCtrl202_t hardwareLnkListEcuAccessCtrl20_2;                     ///< 0xF010D030 : Hardware Link List ECU Access Control 2
    HardwareLnkListHostAccessCtrl000_t hardwareLnkListHostAccessCtrl00_0;                   ///< 0xF010D034 : Hardware Link List Host Access Control 0
    HardwareLnkListHostAccessCtrl101_t hardwareLnkListHostAccessCtrl10_1;                   ///< 0xF010D038 : Hardware Link List Host Access Control 1
    uint32_t HWLL_RD_DEST_ADDR;                                                             ///< 0xF010D03C : Hardware Link List Host Access Address
    HardwareLnkListTblCtrl00_t hardwareLnkListTblCtrl0_0;                                   ///< 0xF010D040 : Hardware Link List Table Control
    uint32_t HWLL_ERR_FIFO_DW0;                                                             ///< 0xF010D044 : Hardware Link List Error FIFO Dword 0
    uint32_t HWLL_ERR_FIFO_DW1;                                                             ///< 0xF010D048 : Hardware Link List Error FIFO Dword 1
    HardwareLnkListErrFifoCtrl00_t hardwareLnkListErrFifoCtrl0_0;                           ///< 0xF010D04C : Hardware Link List Error FIFO Control
    HardwareLnkListMemCtrl00_t hardwareLnkListMemCtrl0_0;                                   ///< 0xF010D050 : Hardware Link List Memory Control
    HardwareLnkListMemCfg00_t hardwareLnkListMemCfg0_0;                                     ///< 0xF010D054 : Hardware Link List Memory Configuration
    uint32_t HWLL_ARTMEM_CPUACC1;                                                           ///< 0xF010D058 : Hardware Link List Active Request Table Memory Access Data 0
    HardwareLnkListActiveReqTblMemAccess11_t hardwareLnkListActiveReqTblMemAccess1_1;       ///< 0xF010D05C : Hardware Link List Active Request Table Memory Access Data 1
    HardwareLnkListHeadTailMemAccessData00_t hardwareLnkListHeadTailMemAccessData0_0;       ///< 0xF010D060 : Hardware Link List Head Tail Memory Access Data
    uint32_t HWLL_LAMEM_CPUACC1;                                                            ///< 0xF010D064 : Hardware Link List Link Array Memory Access Data 0
    HardwareLnkListLnkArrayMemAccessData11_t hardwareLnkListLnkArrayMemAccessData1_1;       ///< 0xF010D068 : Hardware Link List Link Array Memory Access Data 1
    uint8_t rsvd_RF010D06C[4];                                                              ///< 0xF010D06C - 0xF010D06F : Reserved.
    HardwareLnkListMonitorSgnSel00_t hardwareLnkListMonitorSgnSel0_0;                       ///< 0xF010D070 : Hardware Link List Monitor Signal Select
    uint8_t rsvd_RF010D074[12];                                                             ///< 0xF010D074 - 0xF010D07F : Reserved.
    HardwareLnkListHostActiveReqTblAddr00_t hardwareLnkListHostActiveReqTblAddr0_0;         ///< 0xF010D080 : Hardware Link List Host Active Request Table Address
    HardwareLnkListHostActiveReqTblAccess00_t hardwareLnkListHostActiveReqTblAccess0_0;     ///< 0xF010D084 : Hardware Link List Host Active Request Table Access 0
    HardwareLnkListHostActiveReqTblAccess11_t hardwareLnkListHostActiveReqTblAccess1_1;     ///< 0xF010D088 : Hardware Link List Host Active Request Table Access 1
} PcieDmaHll_t;
COMPILE_ASSERT(offsetof(PcieDmaHll_t,hardwareLnkListHostActiveReqTblAccess1_1)==0xF010D088-0xF010D000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile PcieDmaHll_t rPcieDmaHll;            ///< 0xF010D000-0xF010D090 : PcieDmaHll_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
extern PcieDmaHll_t mPcieDmaHll;            ///< 0xF010D000-0xF010D090 : PcieDmaHll_t MODULE

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
//! @brief NVMe HW Link List Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  NVMe HW Link List Registers structure definitions
//-----------------------------------------------------------------------------

///  @brief 0xF5012020 : Hardware Link List Mem Allocation (_SOC_REV_>=0x0200)
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_PRI_MEM_SIZE      : 8;         ///<BIT [07:00] Hardware Link List Primary Memory Size (AUs).
        uint32_t RF5012020_15_08_RSVD   : 8;         ///<BIT [15:08] Reserved (Do not change the default value).
        uint32_t HWLL_SEC_MEM_SIZE      : 8;         ///<BIT [23:16] Hardware Link List Secondary Memory Size (AUs).
        uint32_t RF5012020_31_24_RSVD   : 8;         ///<BIT [31:24] Reserved (Do not change the default value).
    } b;    
} HardwareLnkListMemAllocation_t;

///  @brief 0xF5012024 : Hardware Link List Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_AU_MEM_BLK_SIZE   :16;         ///<BIT [15:0] Hardware Link List AU Memory Block Size.
        uint32_t HWLL_RDDAT_RDY_BM_UPDT : 1;         ///<BIT [16] Hardware Link List Read Data Ready Bitmap Update Control.
        uint32_t HWLL_LNK_SRCH_MD       : 1;         ///<BIT [17] Hardware Link List Link Search Mode.
        uint32_t AHB_ACC_ERR_CTRL       : 1;         ///<BIT [18] AHB Access Error Control.
        uint32_t HWLL_HW_HT_RECOVER     : 1;         ///<BIT [19] Hardware Link List Haredware Head Tail Recovery.
        uint32_t RF5012024_31_20_RSVD   :12;         ///<BIT [31:20] Reserved.
    } b;    
} HardwareLnkListCfg11_t;

///  @brief 0xF5012028 : Hardware Link List ECU Access Control 0
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
        uint32_t RF5012028_31_19_RSVD  :13;         ///<BIT [31:19] Reserved.
    } b;    
} HardwareLnkListEcuAccessCtrl010_t;

///  @brief 0xF501202C : Hardware Link List ECU Access Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_AU_IDX_ECU      :16;         ///<BIT [15:0] Hardware Link List AU Index for ECU.
        uint32_t HWLL_CMD_IDX_ECU     : 9;         ///<BIT [24:16] Hardware Link List Command Index for ECU.
        uint32_t RF501202C_31_25_RSVD : 7;         ///<BIT [31:25] Reserved.
    } b;    
} HardwareLnkListEcuAccessCtrl111_t;

///  @brief 0xF5012030 : Hardware Link List ECU Access Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_ART_PTR        : 7;         ///<BIT [6:0] Hardware Link List Active Request Table Pointer.
        uint32_t RF5012030_31_7_RSVD :25;         ///<BIT [31:7] Reserved.
    } b;    
} HardwareLnkListEcuAccessCtrl212_t;

///  @brief 0xF5012034 : Hardware Link List Host Access Control 0
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
        uint32_t RF5012034_15_13_RSVD       : 3;         ///<BIT [15:13] Reserved.
        uint32_t HWLL_HOST_MD1_AU_IDX       :16;         ///<BIT [31:16] Hardware Link List Host Mode 1 AU index.
    } b;    
} HardwareLnkListHostAccessCtrl010_t;

///  @brief 0xF5012038 : Hardware Link List Host Access Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_HOST_AU_IDX     :16;         ///<BIT [15:0] Host Request AU Index.
        uint32_t HWLL_HOST_CMD_IDX    : 9;         ///<BIT [24:16] Host Command Index.
        uint32_t RF5012038_31_25_RSVD : 7;         ///<BIT [31:25] Reserved.
    } b;    
} HardwareLnkListHostAccessCtrl111_t;

///  @brief 0xF5012040 : Hardware Link List Table Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_FW_LNK_AVL_SET      : 1;         ///<BIT [0] Hardware Link List Firmware Link Available Bit Set Request.
        uint32_t HWLL_FW_LNK_AVL_RST      : 1;         ///<BIT [1] Hardware Link List Firmware Link Reset Request.
        uint32_t HWLL_FW_LNK_AVL_SET_GNT  : 1;         ///<BIT [2] Hardware Link List Firmware Link Available Set Request Grant.
        uint32_t HWLL_FW_LNK_AVL_RST_GNT  : 1;         ///<BIT [3] Hardware Link List Firmware Link Available Reset Request Grant.
    uint32_t RF5012040_7_4_RSVD           : 4;         ///<BIT [7:4] Reserved.
        uint32_t HWLL_FW_HART_AVL_SET     : 1;         ///<BIT [8] Hardware Link List Firmware Host Active Req Table Set Request.
        uint32_t HWLL_FW_HART_AVL_RST     : 1;         ///<BIT [9] Hardware Link List Firmware Host Active Request Table Reset Req.
        uint32_t HWLL_FW_HART_AVL_SET_GNT : 1;         ///<BIT [10] Hardware Link List Firmware Host Active Request Table Set Reque.
        uint32_t HWLL_FW_HART_AVL_RST_GNT : 1;         ///<BIT [11] Hardware Link List Firmware Host Active Request Table Reset Req.
        uint32_t HWLL_FW_HART_AVL_ADDR    : 2;         ///<BIT [13:12] Hardware Link List Firmware Host Active Request Table Address.
        uint32_t RF5012040_23_14_RSVD     :10;         ///<BIT [23:14] Reserved.
        uint32_t HWLL_FW_LNK_VLD_ADDR     : 7;         ///<BIT [30:24] Hardware Link List Firmware Link Valid Set Reset Address.
        uint32_t RF5012040_31_RSVD        : 1;         ///<BIT [31] Reserved.
    } b;    
} HardwareLnkListTblCtrl11_t;

///  @brief 0xF501204C : Hardware Link List Error FIFO Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF501204C_0_RSVD    : 1;         ///<BIT [0] Reserved.
        uint32_t HWLL_ERR_FIFO_PUSH  : 1;         ///<BIT [1] Hardware Link List Error FIFO Push.
        uint32_t HWLL_ERR_FIFO_POP   : 1;         ///<BIT [2] Hardware Link List Error FIFO Pop.
        uint32_t HWLL_ERR_FIFO_FULL  : 1;         ///<BIT [3] Hardware Link List Error FIFO Full.
        uint32_t HWLL_ERR_FIFO_EMPTY : 1;         ///<BIT [4] Hardware Link List Error FIFO Empty.
        uint32_t HWLL_ERR_FIFO_CNT   : 3;         ///<BIT [7:5] Hardware Link List Error FIFO Count.
        uint32_t HWLL_ERR_INT_EN     : 1;         ///<BIT [8] Hardware Link List Error Interrupt Enable.
        uint32_t RF501204C_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;    
} HardwareLnkListErrFifoCtrl11_t;

///  @brief 0xF5012050 : Hardware Link List Memory Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_ECC_ERR_INJ_MD  : 1;         ///<BIT [0] Hardware Link List ECC Error Injection Mode.
        uint32_t RF5012050_1_RSVD :1;              ///<BIT [1] Reserved.
        uint32_t HWLL_ECC_CHK_HT      : 1;         ///<BIT [2] Hardware Link List ECC Check on Head/Tail Memory.
        uint32_t HWLL_ECC_CHK_LA      : 1;         ///<BIT [3] Hardware Link List ECC Check on Link Array Memory.
        uint32_t RF5012050_4_RSVD :1;              ///<BIT [4] Reserved.
        uint32_t HWLL_ECC_1BIT_EN_HT  : 1;         ///<BIT [5] Hardware Link List ECC 1-Bit Error Report for Head/Tail Memory.
        uint32_t HWLL_ECC_1BIT_EN_LA  : 1;         ///<BIT [6] Hardware Link List ECC 1-Bit Error Report for Link Array Memory.
        uint32_t RF5012050_7_RSVD :1;              ///<BIT [7] Reserved.
        uint32_t HWLL_HTMEM_CPU_ACC   : 1;         ///<BIT [8] Hardware Link List Head/Tail Memory CPU Access.
        uint32_t HWLL_LAMEM_CPU_ACC   : 1;         ///<BIT [9] Hardware Link List Link Array Memory CPU Access.
        uint32_t RF5012050_31_10_RSVD :22;         ///<BIT [31:10] Reserved.
    } b;    
} HardwareLnkListMemCtrl11_t;

///  @brief 0xF5012054 : Hardware Link List Memory Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_ERR_INJ_ART    : 2;         ///<BIT [1:0] Hardware Link List Error Injection for Active Request Table Mem.
        uint32_t HWLL_ERR_INJ_HT     : 2;         ///<BIT [3:2] Hardware Link List Error Injection for Head/Tail Memory.
        uint32_t HWLL_ERR_INJ_LA     : 2;         ///<BIT [5:4] Hardware Link List Error Injection for Link Array Memory.
        uint32_t RF5012054_31_6_RSVD :26;         ///<BIT [31:6] Reserved.
    } b;    
} HardwareLnkListMemCfg11_t;

///  @brief 0xF501205C : Hardware Link List Active Request Table Memory Access Data 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_ARTMEM_RAW_RDAT : 2;         ///<BIT [1:0] Hardware Link List Active Request Memory Raw Read Data 2.
        uint32_t HWLL_ARTMEM_PAR_RDAT : 8;         ///<BIT [9:2] Hardware Link List Active Request Table Memory Parity Read Data.
        uint32_t RF501205C_31_10_RSVD :22;         ///<BIT [31:10] Reserved.
    } b;    
} HardwareLnkListActiveReqTblMemAccess31_t;

///  @brief 0xF5012060 : Hardware Link List Head Tail Memory Access Data
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_HTMEM_RAW_RDAT  :16;         ///<BIT [15:0] Hardware Link List Head/Tail Memory Raw Read Data.
        uint32_t RF5012060_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} HardwareLnkListHeadTailMemAccessData11_t;

///  @brief 0xF5012068 : Hardware Link List Link Array Memory Access Data 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_LAMEM_RAW_RDAT  : 2;         ///<BIT [1:0] Hardware Link List Link Array Memory Raw Read Data.
        uint32_t HWLL_LAMEM_PAR_RDAT  : 8;         ///<BIT [9:2] Hardware Link List Link Array Memory Parity Read Data.
        uint32_t RF5012068_31_10_RSVD :22;         ///<BIT [31:10] Reserved.
    } b;    
} HardwareLnkListLnkArrayMemAccessData31_t;

///  @brief 0xF5012070 : Hardware Link List Monitor Signal Select
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_MON_SEL        : 3;         ///<BIT [2:0] Hardware Link List Monitor Signal Select.
        uint32_t RF5012070_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} HardwareLnkListMonitorSgnSel11_t;

///  @brief 0xF5012080 : Hardware Link List Host Active Request Table Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HART_ADDR           : 2;         ///<BIT [1:0] Host Active Request Table Address.
        uint32_t RF5012080_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} HardwareLnkListHostActiveReqTblAddr11_t;

///  @brief 0xF5012084 : Hardware Link List Host Active Request Table Access 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_NXT_PTR         : 8;         ///<BIT [7:0] Hardware Link List Next Pointer.
        uint32_t RF5012084_9_8_RSVD   : 2;         ///<BIT [9:8] Reserved.
        uint32_t HWLL_CUR_PTR         : 8;         ///<BIT [17:10] Hardware Link List Current Pointer.
        uint32_t RF5012084_19_18_RSVD : 2;         ///<BIT [19:18] Reserved.
        uint32_t HWLL_PRE_PTR         : 8;         ///<BIT [27:20] Hardware Link List Previous Pointer.
        uint32_t RF5012084_29_28_RSVD : 2;         ///<BIT [29:28] Reserved.
        uint32_t HWLL_LAST_LNK        : 1;         ///<BIT [30] Hardware Link List Last Link.
        uint32_t HWLL_HART_VAL        : 1;         ///<BIT [31] Hardware Link List Host Active Request Table Valid Bit.
    } b;    
} HardwareLnkListHostActiveReqTblAccess20_t;

///  @brief 0xF5012088 : Hardware Link List Host Active Request Table Access 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_CMD_IDX_ART    : 9;         ///<BIT [8:0] Hardware Link List Command Index for Active Request Table.
        uint32_t RF5012088_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
        uint32_t HWLL_AU_IDX_ART     :16;         ///<BIT [31:16] Hardware Link List AU Index for Active Request Table.
    } b;    
} HardwareLnkListHostActiveReqTblAccess31_t;

///  @brief 0xF5012090 : Hardware Link List Error Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_RST_ERR_TBL_0  : 1;         ///<BIT [0] Hardware Link List Reset Error Table Entry 0 Reset.
        uint32_t HWLL_RST_ERR_TBL_1  : 1;         ///<BIT [1] Hardware Link List Reset Error Table Entry 1 Reset.
        uint32_t HWLL_RST_ERR_TBL_2  : 1;         ///<BIT [2] Hardware Link List Reset Error Table Entry 2 Reset.
        uint32_t HWLL_RST_ERR_TBL_3  : 1;         ///<BIT [3] Hardware Link List Reset Error Table Entry 3 Reset.
        uint32_t RF5012090_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
        uint32_t HWLL_CMD_IDX        : 9;         ///<BIT [24:16] Hardware Link List Command Index.
        uint32_t RF5012090_30_25_RSVD: 6;         ///<BIT [30:25] Reserved.
        uint32_t HWLL_CMD_CLR        : 1;         ///<BIT [31] Hardware Link List Clear Request.
    } b;    
} HardwareLnkListErrorControl_t;

///  @brief 0xF5012094 : Hardware Link List Error Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_CMD_CLR_DONE   : 1;         ///<BIT [0] Hardware Link List Command Clear Done.
        uint32_t RF5012094_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} HardwareLnkListErrorStatus_t;

///  @brief 0xF5012098 : Hardware Link List Error Table Dword 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_ERR_TBL_0       :13;         ///<BIT [12:0] Hardware Link List Error Table Entry 0.
        uint32_t RF5012098_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t HWLL_ERR_TBL_1       :13;         ///<BIT [28:16] Hardware Link List Error Table Entry 1.
        uint32_t RF5012098_31_29_RSVD : 3;         ///<BIT [31:29] Reserved.
    } b;    
} HardwareLnkListErrTblDword0_t;

///  @brief 0xF501209C : Hardware Link List Error Table Dword 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HWLL_ERR_TBL_2       :13;         ///<BIT [12:0] Hardware Link List Error Table Entry 3.
        uint32_t RF501209C_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t HWLL_ERR_TBL_3       :13;         ///<BIT [28:16] Hardware Link List Error Table Entry 3.
        uint32_t RF501209C_31_29_RSVD : 3;         ///<BIT [31:29] Reserved.
    } b;    
} HardwareLnkListErrTblDword1_t;

/// @brief 0xF5012000-0xF50120A0 : NvmeHwll_t MODULE
typedef struct
{
    uint32_t HWLL_LNK_AVL_0_31;                                                             ///< 0xF5012000 : Hardware Link List Link Available Slots 0
    uint32_t HWLL_LNK_AVL_32_63;                                                            ///< 0xF5012004 : Hardware Link List Link Available Slots 1
    uint32_t HWLL_LNK_AVL_64_95;                                                            ///< 0xF5012008 : Hardware Link List Link Available Slots 3
    uint32_t HWLL_LNK_AVL_96_127;                                                           ///< 0xF501200C : Hardware Link List Link Available Slots 4
    uint32_t HWLL_INT_MEM_BASE_ADDR;                                                        ///< 0xF5012010 : Hardware Link List Internal Memory Base Address
    uint32_t HWLL_EXT_MEM_BASE_ADDR;                                                        ///< 0xF5012014 : Hardware Link List External Memory Base Address
    uint32_t HWLL_WR_DEST_ADDR;                                                             ///< 0xF5012018 : Hardware Link List ECU Access Address
    uint8_t rsvd_RF501201C[4];                                                              ///< 0xF501201C - 0xF501201F : Reserved.
    HardwareLnkListMemAllocation_t hardwareLnkListMemAllocation;                            ///< 0xF5012020 : Hardware Link List Memory Allocation
    HardwareLnkListCfg11_t hardwareLnkListCfg1_1;                                           ///< 0xF5012024 : Hardware Link List Configuration
    HardwareLnkListEcuAccessCtrl010_t hardwareLnkListEcuAccessCtrl01_0;                     ///< 0xF5012028 : Hardware Link List ECU Access Control 0
    HardwareLnkListEcuAccessCtrl111_t hardwareLnkListEcuAccessCtrl11_1;                     ///< 0xF501202C : Hardware Link List ECU Access Control 1
    HardwareLnkListEcuAccessCtrl212_t hardwareLnkListEcuAccessCtrl21_2;                     ///< 0xF5012030 : Hardware Link List ECU Access Control 2
    HardwareLnkListHostAccessCtrl010_t hardwareLnkListHostAccessCtrl01_0;                   ///< 0xF5012034 : Hardware Link List Host Access Control 0
    HardwareLnkListHostAccessCtrl111_t hardwareLnkListHostAccessCtrl11_1;                   ///< 0xF5012038 : Hardware Link List Host Access Control 1
    uint32_t HWLL_RD_DEST_ADDR;                                                             ///< 0xF501203C : Hardware Link List Host Access Address
    HardwareLnkListTblCtrl11_t hardwareLnkListTblCtrl1_1;                                   ///< 0xF5012040 : Hardware Link List Table Control
    uint32_t HWLL_ERR_FIFO_DW0;                                                             ///< 0xF5012044 : Hardware Link List Error FIFO Dword 0
    uint32_t HWLL_ERR_FIFO_DW1;                                                             ///< 0xF5012048 : Hardware Link List Error FIFO Dword 1
    HardwareLnkListErrFifoCtrl11_t hardwareLnkListErrFifoCtrl1_1;                           ///< 0xF501204C : Hardware Link List Error FIFO Control
    HardwareLnkListMemCtrl11_t hardwareLnkListMemCtrl1_1;                                   ///< 0xF5012050 : Hardware Link List Memory Control
    HardwareLnkListMemCfg11_t hardwareLnkListMemCfg1_1;                                     ///< 0xF5012054 : Hardware Link List Memory Configuration
    uint32_t HWLL_ARTMEM_CPUACC1;                                                           ///< 0xF5012058 : Hardware Link List Active Request Table Memory Access Data 0
    HardwareLnkListActiveReqTblMemAccess31_t hardwareLnkListActiveReqTblMemAccess3_1;       ///< 0xF501205C : Hardware Link List Active Request Table Memory Access Data 1
    HardwareLnkListHeadTailMemAccessData11_t hardwareLnkListHeadTailMemAccessData1_1;       ///< 0xF5012060 : Hardware Link List Head Tail Memory Access Data
    uint32_t HWLL_LAMEM_CPUACC1;                                                            ///< 0xF5012064 : Hardware Link List Link Array Memory Access Data 0
    HardwareLnkListLnkArrayMemAccessData31_t hardwareLnkListLnkArrayMemAccessData3_1;       ///< 0xF5012068 : Hardware Link List Link Array Memory Access Data 1
    uint8_t rsvd_RF501206C[4];                                                              ///< 0xF501206C - 0xF501206F : Reserved.
    HardwareLnkListMonitorSgnSel11_t hardwareLnkListMonitorSgnSel1_1;                       ///< 0xF5012070 : Hardware Link List Monitor Signal Select
    uint8_t rsvd_RF5012074[12];                                                             ///< 0xF5012074 - 0xF501207F : Reserved.
    HardwareLnkListHostActiveReqTblAddr11_t hardwareLnkListHostActiveReqTblAddr1_1;         ///< 0xF5012080 : Hardware Link List Host Active Request Table Address
    HardwareLnkListHostActiveReqTblAccess20_t hardwareLnkListHostActiveReqTblAccess2_0;     ///< 0xF5012084 : Hardware Link List Host Active Request Table Access 0
    HardwareLnkListHostActiveReqTblAccess31_t hardwareLnkListHostActiveReqTblAccess3_1;     ///< 0xF5012088 : Hardware Link List Host Active Request Table Access 1
    uint8_t rsvd_RF501208C[4];                                                              ///< 0xF501208C - 0xF501208F : Reserved.
    HardwareLnkListErrorControl_t hardwareLnkListErrorControl;                              ///< 0xF5012090 : Hardware Link List Error Control
    HardwareLnkListErrorStatus_t hardwareLnkListErrorStatus;                                ///< 0xF5012094 : Hardware Link List Error Status
    HardwareLnkListErrTblDword0_t hardwareLnkListErrTblDword0;                              ///< 0xF5012098 : Hardware Link List Error Table Dword 0
    HardwareLnkListErrTblDword1_t hardwareLnkListErrTblDword1;                              ///< 0xF501209C : Hardware Link List Error Table Dword 1
} NvmeHwll_t;
COMPILE_ASSERT(offsetof(NvmeHwll_t,hardwareLnkListHostActiveReqTblAccess3_1)==0xF5012088-0xF5012000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile NvmeHwll_t rNvmeHwll;                ///< 0xF5012000-0xF501208C : NvmeHwll_t MODULE

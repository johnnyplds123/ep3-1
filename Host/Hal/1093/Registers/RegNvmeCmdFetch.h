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
//! @brief NVMe Command Fetcher Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  NVMe Command Fetcher Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF5010000 : Command Fetcher Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ARB_OP_MD            : 1;         ///<BIT [0] Arbitration Operation Mode (0:Auto Fetching, 1: Manual).
        uint32_t RF5010000_3_1_RSVD   : 3;         ///<BIT [3:1] Reserved.
        uint32_t CMDF_SIZE_MD         : 1;         ///<BIT [4] Command Fetcher Size Mode.
        uint32_t RF5010000_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t CMDF_DEST_MD         : 2;         ///<BIT [9:8] Destination Mode.
        uint32_t RF5010000_11_10_RSVD : 2;         ///<BIT [11:10] Reserved.
        uint32_t CMD_FMT_MD           : 1;         ///<BIT [12] Command Format Mode.
        uint32_t RF5010000_31_13_RSVD :19;         ///<BIT [31:13] Reserved.
    } b;    
} CmdFetcherCfg_t;

///  @brief 0xF5010008 : Command Fetcher Command
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CMDF_START          : 1;         ///<BIT [0] Command Fetcher Start.
        uint32_t RF5010008_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} CmdFetcherCmd_t;

///  @brief 0xF501000C : Command Fetcher Engine Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DONE                 : 1;         ///<BIT [0] Command Fetch Done.
        uint32_t RF501000C_15_1_RSVD  :15;         ///<BIT [15:1] Reserved.
        uint32_t CMDF_CTRL_ST         : 5;         ///<BIT [20:16] Command Fetcher Controller State.
        uint32_t RF501000C_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t CMB_ST               : 7;         ///<BIT [30:24] Command Builder State.
        uint32_t RF501000C_31_RSVD    : 1;         ///<BIT [31] Reserved.
    } b;    
} CmdFetcherEngineStat_t;

///  @brief 0xF5010014 : Command Fetcher Firmware Arbitration-IDs
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ARB_SQID             : 5;         ///<BIT [4:0] Submission Queue ID.
        uint32_t RF5010014_15_5_RSVD  :11;         ///<BIT [15:5] Reserved.
        uint32_t ARB_FID              : 2;         ///<BIT [17:16] Function ID.
        uint32_t RF5010014_31_18_RSVD :14;         ///<BIT [31:18] Reserved.
    } b;    
} CmdFetcherFwArbIds_t;

///  @brief 0xF5010018 : Command Fetcher Firmware Arbitration-Fetch Number
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FET_NUM              :17;         ///<BIT [16:0] Fetch Number.
        uint32_t RF5010018_31_17_RSVD :15;         ///<BIT [31:17] Reserved.
    } b;    
} CmdFetcherFwArbFetchNum_t;

///  @brief 0xF5010024 : Command Fetcher Fetch Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CMDF_DROP_NUM        :17;         ///<BIT [16:0] Drop Number.
        uint32_t RF5010024_31_17_RSVD :15;         ///<BIT [31:17] Reserved.
    } b;    
} CmdFetcherFetchStat_t;

///  @brief 0xF5010030 : Command Fetcher PCIe Timer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CMDF_TO_THR          :16;         ///<BIT [15:0] Command Fetcher PCIe Time-Out Threshold.
        uint32_t RF5010030_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} CmdFetcherPcieTmr_t;

///  @brief 0xF5010034 : Command Fetcher PCIe Retry
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PCIE_RETRY_NUM      : 3;         ///<BIT [2:0] PCIe Retry Number.
        uint32_t RF5010034_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} CmdFetcherPcieRetry_t;

///  @brief 0xF5010038 : Command Fetcher PCIe Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t URG_PRP_PCIE_REQ     : 4;         ///<BIT [3:0] Urgent PRP PCIe Request Status.
        uint32_t NRG_PRP_PCIE_REQ     : 4;         ///<BIT [7:4] Non-Urgent PRP PCIe Request Status.
        uint32_t CMD_PCIE_REQ_ST      : 4;         ///<BIT [11:8] Command PCIe Request Status.
        uint32_t RF5010038_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t PCIE_REQ_MOD_ST      : 2;         ///<BIT [17:16] PCIe Request Module State.
        uint32_t RF5010038_31_18_RSVD :14;         ///<BIT [31:18] Reserved.
    } b;    
} CmdFetcherPcieStat_t;

///  @brief 0xF5010044 : Command Fetcher AXI Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AXI_REQ_ABT         : 1;         ///<BIT [0] AXI Request Abort.
        uint32_t RF5010044_3_1_RSVD  : 3;         ///<BIT [3:1] Reserved.
        uint32_t AXI_W_PAR_POL       : 1;         ///<BIT [4] AXI Write Parity Polarity.
        uint32_t RF5010044_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} CmdFetcherAxiCtrl_t;

///  @brief 0xF501004C : Command Fetcher AXI Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AXI_REQ_ABT_DONE    : 1;         ///<BIT [0] AXI Request Abort Done.
        uint32_t RF501004C_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} CmdFetcherAxiStat_t;

///  @brief 0xF5010054 : Command Fetcher AXI Buffer Size
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CMDF_BUF_SIZE        :16;         ///<BIT [15:0] Command Fetcher Ring Buffer Size.
        uint32_t RF5010054_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} CmdFetcherAxiBufSize_t;

///  @brief 0xF5010058 : Command Fetcher AXI Buffer Counter
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CMDF_BUF_CMD_CNTR    :16;         ///<BIT [15:0] Ring Buffer Command Available Counter.
        uint32_t RF5010058_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} CmdFetcherAxiBufCntr_t;

///  @brief 0xF501005C : Command Fetcher AXI Buffer Counter Decrease
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CMDF_BUF_CNTR_DEC    :16;         ///<BIT [15:0] Ring Buffer Counter Decrease Number.
        uint32_t RF501005C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} CmdFetcherAxiBufCntrDecrease_t;

///  @brief 0xF5010064 : Command Fetcher AXI Buffer Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CMDF_BUF_W_PTR       :16;         ///<BIT [15:0] Ring Buffer Write Pointer.
        uint32_t RF5010064_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} CmdFetcherAxiBufPtr_t;

///  @brief 0xF5010070 : Command Fetcher Error FIFO Count
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CMDF_ERR_CNTR       : 3;         ///<BIT [2:0] Command Fetcher Error Status Counter.
        uint32_t RF5010070_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} CmdFetcherErrFifoCnt_t;

///  @brief 0xF501007C : Command Fetcher Monitor Select
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TOP_SELECT           : 3;         ///<BIT [2:0] Top Monitor Signal Select.
        uint32_t RF501007C_7_3_RSVD   : 5;         ///<BIT [7:3] Reserved.
        uint32_t AXI_MON_SGN_SEL      : 7;         ///<BIT [14:8] AXI Monitor Signal Select.
        uint32_t RF501007C_31_15_RSVD :17;         ///<BIT [31:15] Reserved.
    } b;    
} CmdFetcherMonitorSel_t;

///  @brief 0xF5010100 : Command Fetcher Namespace Base LBA Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t NS_BLBA_AUTO_ADDR_INC : 1;         ///<BIT [0] Namespace Base LBA Automatic Address Increment.
        uint32_t RF5010100_31_1_RSVD   :31;         ///<BIT [31:1] Reserved.
    } b;    
} CmdFetcherNamespacBaseLbaCtrl_t;

///  @brief 0xF5010104 : Command Fetcher Namespace Base LBA Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t NS_BLBA_ADDR        : 3;         ///<BIT [2:0] Namespace Base Logical Block Address Table Address.
        uint32_t RF5010104_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} CmdFetcherNamespacBaseLbaAddr_t;


/// @brief 0xF5010000-0xF501010C : NvmeCmdFetch_t MODULE
typedef struct
{
    CmdFetcherCfg_t cmdFetcherCfg;                                     ///< 0xF5010000 : Command Fetcher Configuration
    uint8_t rsvd_RF5010004[4];                                         ///< 0xF5010004 - 0xF5010007 : Reserved.
    CmdFetcherCmd_t cmdFetcherCmd;                                     ///< 0xF5010008 : Command Fetcher Command
    CmdFetcherEngineStat_t cmdFetcherEngineStat;                       ///< 0xF501000C : Command Fetcher Engine Status
    uint8_t rsvd_RF5010010[4];                                         ///< 0xF5010010 - 0xF5010013 : Reserved.
    CmdFetcherFwArbIds_t cmdFetcherFwArbIds;                           ///< 0xF5010014 : Command Fetcher Firmware Arbitration-IDs
    CmdFetcherFwArbFetchNum_t cmdFetcherFwArbFetchNum;                 ///< 0xF5010018 : Command Fetcher Firmware Arbitration-Fetch Number
    uint8_t rsvd_RF501001C[8];                                         ///< 0xF501001C - 0xF5010023 : Reserved.
    CmdFetcherFetchStat_t cmdFetcherFetchStat;                         ///< 0xF5010024 : Command Fetcher Fetch Status
    uint8_t rsvd_RF5010028[8];                                         ///< 0xF5010028 - 0xF501002F : Reserved.
    CmdFetcherPcieTmr_t cmdFetcherPcieTmr;                             ///< 0xF5010030 : Command Fetcher PCIe Timer
    CmdFetcherPcieRetry_t cmdFetcherPcieRetry;                         ///< 0xF5010034 : Command Fetcher PCIe Retry
    CmdFetcherPcieStat_t cmdFetcherPcieStat;                           ///< 0xF5010038 : Command Fetcher PCIe Status
    uint8_t rsvd_RF501003C[8];                                         ///< 0xF501003C - 0xF5010043 : Reserved.
    CmdFetcherAxiCtrl_t cmdFetcherAxiCtrl;                             ///< 0xF5010044 : Command Fetcher AXI Control
    uint8_t rsvd_RF5010048[4];                                         ///< 0xF5010048 - 0xF501004B : Reserved.
    CmdFetcherAxiStat_t cmdFetcherAxiStat;                             ///< 0xF501004C : Command Fetcher AXI Status
    uint32_t CMDF_AXI_BUF_ADDR;                                        ///< 0xF5010050 : Command Fetcher AXI Buffer Address
    CmdFetcherAxiBufSize_t cmdFetcherAxiBufSize;                       ///< 0xF5010054 : Command Fetcher AXI Buffer Size
    CmdFetcherAxiBufCntr_t cmdFetcherAxiBufCntr;                       ///< 0xF5010058 : Command Fetcher AXI Buffer Counter
    CmdFetcherAxiBufCntrDecrease_t cmdFetcherAxiBufCntrDecrease;       ///< 0xF501005C : Command Fetcher AXI Buffer Counter Decrease
    uint8_t rsvd_RF5010060[4];                                         ///< 0xF5010060 - 0xF5010063 : Reserved.
    CmdFetcherAxiBufPtr_t cmdFetcherAxiBufPtr;                         ///< 0xF5010064 : Command Fetcher AXI Buffer Pointer
    uint8_t rsvd_RF5010068[8];                                         ///< 0xF5010068 - 0xF501006F : Reserved.
    CmdFetcherErrFifoCnt_t cmdFetcherErrFifoCnt;                       ///< 0xF5010070 : Command Fetcher Error FIFO Count
    uint32_t CMDF_ERR_FIFO_DW[2];                                      ///< 0xF5010074 - 0xF5010078 : Command Fetcher Error FIFO Dword 0/1
    //    uint32_t CMDF_ERR_FIFO_DW0;                                  ///< 0xF5010074 : Command Fetcher Error FIFO Dword 0
    //    uint32_t CMDF_ERR_FIFO_DW1;                                  ///< 0xF5010078 : Command Fetcher Error FIFO Dword 1
    CmdFetcherMonitorSel_t cmdFetcherMonitorSel;                       ///< 0xF501007C : Command Fetcher Monitor Select
    uint8_t rsvd_RF5010080[128];                                       ///< 0xF5010080 - 0xF50100FF : Reserved.
    CmdFetcherNamespacBaseLbaCtrl_t cmdFetcherNamespacBaseLbaCtrl;     ///< 0xF5010100 : Command Fetcher Namespace Base LBA Control
    CmdFetcherNamespacBaseLbaAddr_t cmdFetcherNamespacBaseLbaAddr;     ///< 0xF5010104 : Command Fetcher Namespace Base LBA Address
    uint32_t CMDF_NS_BLBA_DATA;                                        ///< 0xF5010108 : Command Fetcher Namespace Base LBA Data
} NvmeCmdFetch_t;
COMPILE_ASSERT(offsetof(NvmeCmdFetch_t,CMDF_NS_BLBA_DATA)==0xF5010108-0xF5010000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile NvmeCmdFetch_t rNvmeCmdFetch;        ///< 0xF5010000-0xF501010C : NvmeCmdFetch_t MODULE

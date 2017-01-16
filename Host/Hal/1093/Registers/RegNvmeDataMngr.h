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
//! @brief NVMe Data Manager Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  NVMe Data Manager Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF5011000 : Data Manager Configuration 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DATM_RE_MAX_RETRY   : 2;         ///<BIT [1:0] Data Manager Read Engine Maximum Retry Count.
        uint32_t RF5011000_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} DataManagerCfg0_t;

///  @brief 0xF5011008 : Data Manager Configuration 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t WRITE_CMD_CH_ENABLE : 4;         ///<BIT [3:0] Write CMD Channel Enable.
        uint32_t RF5011008_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} DataManagerCfg1_t;

///  @brief 0xF5011010 : Data Manager Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DATM_ACMD_CABM      : 8;         ///<BIT [7:0] Data Manager Admin Command Activation Bitmap.
        uint32_t RF5011010_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
        uint32_t TD64_BM_MODE        : 1;         ///<BIT [16] 
        uint32_t RF5011010_23_17_RSVD: 7;         ///<BIT [23:17] Reserved.
        uint32_t MEM_WR_MODE         : 1;         ///<BIT [24] 
        uint32_t WAIT_DATA           : 1;         ///<BIT [25] 
        uint32_t RF5011010_31_26_RSVD: 6;         ///<BIT [31:26] Reserved.
    } b;    
} DataManagerCtrl0_t;

///  @brief 0xF5011014 : Data Manager Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RD_ERR_INT_MSK_DPP_ERR      : 1;         ///<BIT [0] Data Manager Read Engine DPP Error CPU Interrupt Mask.
        uint32_t RD_ERR_INT_MSK_MPECC_ERR    : 1;         ///<BIT [1] Read Error MPECC Error Interrupt Mask.
        uint32_t RD_ERR_INT_MSK_BCRC_ERR     : 1;         ///<BIT [2] Read Error BCRC Error Interrupt Mask.
        uint32_t RD_ERR_INT_MSK_HLBA_MMATCH  : 1;         ///<BIT [3] Read Error HLBA Mismatch Error Interrupt Mask.
        uint32_t WR_ERR_INT_MSK_DPP          : 1;         ///<BIT [4] Data Manager Write Engine DPP Error CPU Interrupt Mask.
        uint32_t WR_ERR_INT_MSK_MPECC_ERR    : 1;         ///<BIT [5] Write Error Interrupt Mask MPECC Error.
        uint32_t WR_ERR_INT_MSK_BCRC_ERR     : 1;         ///<BIT [6] Write Error Interrupt Mask BCRC Error.
        uint32_t WR_ERR_INT_MSK_HLBA_MMATCH  : 1;         ///<BIT [7] Write Error Interrupt Mask HLBA Mismatch.
        uint32_t RD_ERR_INT_MSK_E2E          : 1;         ///<BIT [8] Data Manager Read Engine E2E Error CPU Interrupt Mask.
        uint32_t RD_ERR_INT_MSK_GUARD_ERR    : 1;         ///<BIT [9] Read E2E Guard Error Interrupt Mask.
        uint32_t RD_ERR_INT_MSK_LBAT_ERR     : 1;         ///<BIT [10] Read E2E LBAT Error Interrupt Mask.
        uint32_t RD_ERR_INT_MSK_LBRT_ERR     : 1;         ///<BIT [11] Read E2E LBRT Error Interrupt Mask.
        uint32_t WR_ERR_INT_MSK_E2E          : 1;         ///<BIT [12] Data Manager Write Engine E2E Error CPU Interrupt Mask.
        uint32_t WR_ERR_INT_MSK_GUARD_ERR    : 1;         ///<BIT [13] Write E2E Guard Error Interrupt Mask.
        uint32_t WR_ERR_INT_MSK_LBAT_ERR     : 1;         ///<BIT [14] Write E2E LBAT Error Interrupt Mask.
        uint32_t WR_ERR_INT_MSK_LBRT_ERR     : 1;         ///<BIT [15] Write E2E LBRT Error Interrupt Mask.
        uint32_t RF5011014_31_16_RSVD        :16;         ///<BIT [31:16] Reserved.
    } b;    
} DataManagerCtrl1_t;

///  @brief 0xF5011018 : Data Manager Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PRP_ADDR_FORCEREAD_PERR  : 2;         ///<BIT [1:0] Force PRP SRAM Address Field Read Parity Error.
        uint32_t RF5011018_3_2_RSVD       : 2;         ///<BIT [3:2] Reserved.
        uint32_t PRP_ADDR_IGNOREREAD_PERR : 1;         ///<BIT [4] Ignore PRP SRAM Address Field Read Parity Error.
        uint32_t RF5011018_15_5_RSVD      :11;         ///<BIT [15:5] Reserved.
        uint32_t PRP_BYTE_FORCEREAD_PERR  : 2;         ///<BIT [17:16] Force PRP SRAM Byte Field Read Parity Error.
        uint32_t RF5011018_19_18_RSVD     : 2;         ///<BIT [19:18] Reserved.
        uint32_t PRP_BYTE_IGNORREAD_PERR  : 1;         ///<BIT [20] Ignore PRP SRAM Byte Field Read Parity Error.
        uint32_t RF5011018_23_21_RSVD     : 3;         ///<BIT [23:21] Reserved.
        uint32_t LCE_SRAM_FORCEREADPERR   : 2;         ///<BIT [25:24] Force LCE SRAM Byte Field Read Parity Error.
        uint32_t RF5011018_27_26_RSVD     : 2;         ///<BIT [27:26] Reserved.
        uint32_t LCE_SRAM_IGNORREAD_PERR  : 1;         ///<BIT [28] Ignore LCE SRAM Byte Field Read Parity Error.
        uint32_t RF5011018_31_29_RSVD     : 3;         ///<BIT [31:29] Reserved.
    } b;    
} DataManagerCtrl2_t;

///  @brief 0xF5011020 : Data Manager Status 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DATM_ACMD_ATDBM     : 8;         ///<BIT [7:0] Data Manager Admin Command Transfer Done.
        uint32_t RF5011020_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} DataManagerStat0_t;

///  @brief 0xF5011024 : Data Manager Status 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t XFER_DONE_64_MAP1    : 8;         ///<BIT [7:0] Transfer Done 64 AU Bitmap 1.
        uint32_t XFER_DONE_64_MAP2    : 8;         ///<BIT [15:8] Transfer Done 64 AU Bitmap 2.
        uint32_t RF5011024_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DataManagerStat1_t;

///  @brief 0xF5011040 : LBA Format
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t META_SIZE            :16;         ///<BIT [15:0] Metadata Size.
        uint32_t DATA_SIZE            : 3;         ///<BIT [18:16] LBA Data Size.
        uint32_t AU_RATIO             : 3;         ///<BIT [21:19] Allocation Unit (AU) Ratio.
        uint32_t MPECC_EN             : 1;         ///<BIT [22] MPECC Enable.
        uint32_t HLBA_EN              : 1;         ///<BIT [23] Host LBA Enable.
        uint32_t HLBA_SIZE            : 2;         ///<BIT [25:24] Host LBA Size.
        uint32_t BCRC_EN              : 1;         ///<BIT [26] BCRC Enable.
        uint32_t AES_EN               : 1;         ///<BIT [27] AES Enable.
        uint32_t BCRC_LBA_EMB_MODE    : 2;         ///<BIT [29:28] BCRC LBA EMB.
        uint32_t RF5011040_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;    
} LbaFmt_t;

///  @brief 0xF5011040 : LBA Format 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LBA0_META_SIZE       :16;         ///<BIT [15:0] LBA0 Metadata Size.
        uint32_t LBA0_DATA_SIZE       : 3;         ///<BIT [18:16] LBA0 Data Size.
        uint32_t LBA0_AU_RATIO        : 3;         ///<BIT [21:19] LBA0 Allocation Unit (AU) Ratio.
        uint32_t LBA0_MPECC_EN        : 1;         ///<BIT [22] LBA0 MPECC Enable.
        uint32_t LBA0_HLBA_EN         : 1;         ///<BIT [23] Host LBA 0 Enable.
        uint32_t LBA0_HLBA_SIZE       : 2;         ///<BIT [25:24] LBA0 Host LBA Size.
        uint32_t LBA0_BCRC_EN         : 1;         ///<BIT [26] LBA0 BCRC Enable.
        uint32_t LBA0_AES_EN          : 1;         ///<BIT [27] LBA0 AES Enable.
        uint32_t BCRC_LBA0_EMB_MODE   : 2;         ///<BIT [29:28] BCRC LBA0 EMB.
        uint32_t RF5011040_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;    
} LbaFmt0_t;

///  @brief 0xF5011044 : LBA Format 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LBA1_META_SIZE       :16;         ///<BIT [15:0] LBA1 Metadata Size.
        uint32_t LBA1_DATA_SIZE       : 3;         ///<BIT [18:16] LBA1 Data Size.
        uint32_t LBA1_AU_RATIO        : 3;         ///<BIT [21:19] LBA1 Allocation Unit (AU) Ratio.
        uint32_t LBA1_MPECC_EN        : 1;         ///<BIT [22] LBA1 MPECC Enable.
        uint32_t LBA1_HLBA_EN         : 1;         ///<BIT [23] Host LBA 1 Enable.
        uint32_t LBA1_HLBA_SIZE       : 2;         ///<BIT [25:24] LBA1 Host LBA Size.
        uint32_t LBA1_BCRC_EN         : 1;         ///<BIT [26] LBA1 BCRC Enable.
        uint32_t LBA1_AES_EN          : 1;         ///<BIT [27] LBA1 AES Enable.
        uint32_t BCRC_LBA1_EMB_MODE   : 2;         ///<BIT [29:28] BCRC LBA1 EMB.
        uint32_t RF5011044_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;    
} LbaFmt1_t;

///  @brief 0xF5011048 : LBA Format 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LBA2_META_SIZE       :16;         ///<BIT [15:0] LBA2 Metadata Size.
        uint32_t LBA2_DATA_SIZE       : 3;         ///<BIT [18:16] LBA2 Data Size.
        uint32_t LBA2_AU_RATIO        : 3;         ///<BIT [21:19] LBA2 Allocation Unit (AU) Ratio.
        uint32_t LBA2_MPECC_EN        : 1;         ///<BIT [22] LBA2 MPECC Enable.
        uint32_t LBA2_HLBA_EN         : 1;         ///<BIT [23] Host LBA 2 Enable.
        uint32_t LBA2_HLBA_SIZE       : 2;         ///<BIT [25:24] LBA2 Host LBA Size.
        uint32_t LBA2_BCRC_EN         : 1;         ///<BIT [26] LBA2 BCRC Enable.
        uint32_t LBA2_AES_EN          : 1;         ///<BIT [27] LBA2 AES Enable.
        uint32_t BCRC_LBA2_EMB_MODE   : 2;         ///<BIT [29:28] BCRC LBA2 EMB.
        uint32_t RF5011048_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;    
} LbaFmt2_t;

///  @brief 0xF501104C : LBA Format 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LBA3_META_SIZE       :16;         ///<BIT [15:0] LBA3 Metadata Size.
        uint32_t LBA3_DATA_SIZE       : 3;         ///<BIT [18:16] LBA3 Data Size.
        uint32_t LBA3_AU_RATIO        : 3;         ///<BIT [21:19] LBA3 Allocation Unit (AU) Ratio.
        uint32_t LBA3_MPECC_EN        : 1;         ///<BIT [22] LBA3 MPECC Enable.
        uint32_t LBA3_HLBA_EN         : 1;         ///<BIT [23] Host LBA 3 Enable.
        uint32_t LBA3_HLBA_SIZE       : 2;         ///<BIT [25:24] LBA3 Host LBA Size.
        uint32_t LBA3_BCRC_EN         : 1;         ///<BIT [26] LBA3 BCRC Enable.
        uint32_t LBA3_AES_EN          : 1;         ///<BIT [27] LBA3 AES Enable.
        uint32_t BCRC_LBA3_EMB_MODE   : 2;         ///<BIT [29:28] BCRC LBA3 EMB.
        uint32_t RF501104C_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;    
} LbaFmt3_t;

///  @brief 0xF5011080 : PRP Setup Engine Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MAX_RETRY_CNT       : 2;         ///<BIT [1:0] Maximum Retry Count.
        uint32_t RF5011080_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} PrpSetupEngineCfg_t;

///  @brief 0xF5011084 : Data Manager Error Information
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ERR_CNT             : 4;         ///<BIT [3:0] Error Count.
        uint32_t RF5011084_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} DataManagerErrInformat_t;

///  @brief 0xF5011090 : Data Manager Write Engine Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DATM_WR_PAUSE       : 4;         ///<BIT [3:0] Data Manager Write Engine Pause Vector.
        uint32_t RF5011090_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} DataManagerWrEngineStat_t;

///  @brief 0xF5011094 : Data Manager Write Engine Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DATM_WR_PAUSE_CLR    : 4;        ///<BIT [3:0] DATM Write Engine Pause Clear.
        uint32_t RF5011094_31_4_RSVD  :28;        ///<BIT [31:4] Reserved.
    } b;    
} DataManagerWrEngineCtrl_t;

///  @brief 0xF50110A0 : Streaming Controller Control Register 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FIRST_AU_LBA_SKP    : 1;         ///<BIT [0] LBA Skip For First AU.
        uint32_t LAST_AU_LBA_SKP     : 1;         ///<BIT [1] LBA Skip For Last AU.
        uint32_t RF50110A0_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} StreaminControllCtrlRegister0_t;

///  @brief 0xF50110D0 : Data Manager SSD Fetcher Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SUBSEG_BLK_CNT  :16;              ///<BIT [15:0] Sub Segment Block Control.
        uint32_t SEG_SEL              : 8;         ///<BIT [23:16] Current Segment Number Selector.
        uint32_t RF50110D0_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} DataManagerSsdFetcherCtrl_t;

///  @brief 0xF50110DC : SSD List Maximum Entry Count
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RD_SSD_MAX_CNT :16;         ///<BIT [15:0] Read CMD SSD Max Count.
        uint32_t WR_SSD_MAX_CNT :16;         ///<BIT [31:16] Write CMD SSD Max Count.
    } b;    
} SsdListMaxEntryCnt_t;

///  @brief 0xF50110F0 : Data Manager AXI Abort Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ABT                 : 1;         ///<BIT [0] DATM AXI Abort.
        uint32_t RF50110F0_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} DataManagerAxiAbortCtrl_t;

///  @brief 0xF50110F4 : Data Manager AXI Abort Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DDR_RD_DONE         : 1;         ///<BIT [0] DATM DDR AXI Read Abort Done.
        uint32_t DDR_WR_DONE         : 1;         ///<BIT [1] DATM DDR AXI Write Abort Done.
        uint32_t SSDF_RD_DONE        : 1;         ///<BIT [2] DATM SSDF AXI Read Abort Done.
        uint32_t SRAM_RD_DONE        : 1;         ///<BIT [3] DATM SRAM AXI Read Abort Done.
        uint32_t RF50110F4_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} DataManagerAxiAbortStat_t;


///  @brief 0xF5011110 : Data Manager DAB Write Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF5011110_3_0_RSVD   : 4;         ///<BIT [3:0] Reserved.
        uint32_t DAB_CW_LEN           : 4;         ///<BIT [7:4] DAB CW Write Burst Length.
        uint32_t DAB_CW_LOCK          : 2;         ///<BIT [9:8] DAB CW Lock Type.
        uint32_t DAB_CW_BUFF_WR       : 1;         ///<BIT [10] DAB CW Bufferable Write Access.
        uint32_t RF5011110_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
        uint32_t DAB_CW_PROT          : 3;         ///<BIT [18:16] DAB CW Protection Type.
        uint32_t DAB_CW_MAX_WR        : 5;         ///<BIT [23:19] DAB CW Maximum Outstanding Write Response.
        uint32_t RF5011110_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} DataManagerDabWrCfg_t;

///  @brief 0xF5011114 : Data Manager DAB Read Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF5011114_3_0_RSVD   : 4;         ///<BIT [3:0] Reserved.
        uint32_t DAB_CR_LEN           : 4;         ///<BIT [7:4] DAB CR Read Burst Length.
        uint32_t DAB_CR_LOCK          : 2;         ///<BIT [9:8] DAB CR Lock Type.
        uint32_t RF5011114_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t DAB_CR_PROT          : 3;         ///<BIT [18:16] DAB1 CR Protection Type.
        uint32_t DAB_CR_MAX_WR        : 5;         ///<BIT [23:19] DAB CR Maximum Outstanding Read Request.
        uint32_t RF5011114_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} DataManagerDabRdCfg_t;

///  @brief 0xF5011120 : Data Manager Monitor Signal Select
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DATM_STATE_MON_SEL  : 8;         ///<BIT [7:0] Data Manager State Monitor Signal Select.
        uint32_t RF5011120_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} DataManagerMonitorSgnSel_t;

///  @brief 0xF5011124 : Data Manager Write Engine PCIe Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PCIE_RD_SM           : 6;        ///<BIT [5:0] PCIe Read State Machine.
        uint32_t RF5011124_27_6_RSVD :22;         ///<BIT [27:6] Reserved.
        uint32_t PCIE_RD_Q_STAT      : 4;         ///<BIT [31:28] PCIe Read Queue Status.
    } b;    
} DataManagerWrEnginePcieStat_t;

///  @brief 0xF5011128 : Data Manager Write Engine PCIe Debug Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PCIE_RD_Q_SEL       : 4;         ///<BIT [3:0] Select Write Engine PCIe Read Status Queue.
        uint32_t RF5011128_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} DataManagerWrEnginePcieDbgCtrl_t;

///  @brief 0xF501112C : PCIe Read Debug Dword 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t WR_CMD_IDX          : 9;         ///<BIT [8:0] Command Index.
        uint32_t RF501112C_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
        uint32_t PCIE_RD_BLK_CNT     :16;         ///<BIT [31:16] Pending PCIe Read Block Count.
    } b;    
} PcieRdDbgDword0_t;

///  @brief 0xF5011140 : PCIe Read Debug Dword 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PCIE_REQ_CNT         :16;         ///<BIT [15:0] PCIe Request Count.
        uint32_t RF5011140_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PcieRdDbgDword5_t;

///  @brief 0xF5011150 : PCIe Read Debug Dword 9
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LLBA_CNT             : 4;         ///<BIT [3:0] Last LBA Count.
        uint32_t RF5011150_7_4_RSVD   : 4;         ///<BIT [7:4] Reserved.
        uint32_t NEW_BLOCK            : 1;         ///<BIT [8] New Block.
        uint32_t RF5011150_15_9_RSVD  : 7;         ///<BIT [15:9] Reserved.
        uint32_t NSID                 : 3;         ///<BIT [18:16] Namespace ID.
        uint32_t RF5011150_23_19_RSVD : 5;         ///<BIT [23:19] Reserved.
        uint32_t PRINFO               : 4;         ///<BIT [27:24] Protection Information Field for NVMe End-to-End Mode.
        uint32_t RF5011150_31_28_RSVD : 4;         ///<BIT [31:28] Reserved.
    } b;    
} PcieRdDbgDword9_t;

///  @brief 0xF501115C : PCIe Read Debug Dword 12
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LBAT     :16;         ///<BIT [15:0] Logical Block Application Tag Value.
        uint32_t LBAT_MSK :16;         ///<BIT [31:16] Logical Block Application Tag Mask Value.
    } b;    
} PcieRdDbgDword12_t;

///  @brief 0xF5011160 : Data Manager Write Engine Data Formatter Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DATA_FMT_SM         : 3;         ///<BIT [2:0] Data Formatter State Machine.
        uint32_t RF5011160_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} DataManagerWrEngineDataFormatteStat_t;

///  @brief 0xF5011164 : Data Formatter Debug Dword 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DATA_CMD_IDX        : 9;         ///<BIT [8:0] Data Command Index.
        uint32_t RF5011164_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
        uint32_t WR_DATA_IN_CNT      :16;         ///<BIT [31:16] Write Command Data Input Count.
    } b;    
} DataFormatteDbgDword0_t;

///  @brief 0xF5011170 : Data Formatter Debug Dword 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LBA_XFER_CNT       : 6;         ///<BIT [5:0] Number of LBAs to Transfer.
        uint32_t RF5011170_7_6_RSVD : 2;         ///<BIT [7:6] Reserved.
        uint32_t START_SEQ          : 8;         ///<BIT [15:8] Starting Sequence Number.
        uint32_t MAX_SEQ_CNT        : 8;         ///<BIT [23:16] Maximum Sequence Count.
        uint32_t LAST_SEQ_NUM       : 8;         ///<BIT [31:24] Last Sequence Number.
    } b;    
} DataFormatteDbgDword3_t;

///  @brief 0xF5011174 : Data Formatter Debug Dword 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t WRDF_RDY             : 1;         ///<BIT [0] Write Data Formatter Ready.
        uint32_t DPP_PEND             : 1;         ///<BIT [1] DPP Pending.
        uint32_t BCRC_PEND            : 1;         ///<BIT [2] BCRC Generation Pending.
        uint32_t HLBA_PEND            : 1;         ///<BIT [3] HLBA Insertion Pending.
        uint32_t MPECC_PEND           : 1;         ///<BIT [4] MPECC Generation Pending.
        uint32_t RF5011174_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DPP_BCRC_EN          : 1;         ///<BIT [8] DPP BCRC Enable.
        uint32_t DPP_HLBA_EN          : 1;         ///<BIT [9] DPP HLBA Enable.
        uint32_t DPP_HLBA_MODE        : 2;         ///<BIT [11:10] DPP HLBA Mode.
        uint32_t DPP_MPECC_EN         : 1;         ///<BIT [12] DPP MPECC Enable.
        uint32_t DPP_MAES_EN          : 1;         ///<BIT [13] DPP MAES Enable.
        uint32_t RF5011174_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t DPP_OUT_RDY          : 1;         ///<BIT [16] DPP Output Ready.
        uint32_t DPP_OUT_ACK          : 1;         ///<BIT [17] DPP Output Acknowledge.
        uint32_t E2E_OUT_RDY          : 1;         ///<BIT [18] End-to-End Output Ready.
        uint32_t E2E_OUT_ACK          : 1;         ///<BIT [19] End-to-End Output Acknowledge.
        uint32_t RF5011174_31_20_RSVD :12;         ///<BIT [31:20] Reserved.
    } b;    
} DataFormatteDbgDword4_t;

///  @brief 0xF5011178 : Data Formatter Debug Dword 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IN_FIFO_CNT          : 7;         ///<BIT [6:0] Data In FIFO Count.
        uint32_t RF5011178_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint32_t OUT_FIFO_CNT         : 7;         ///<BIT [14:8] Data Out FIFO Count.
        uint32_t RF5011178_31_15_RSVD :17;         ///<BIT [31:15] Reserved.
    } b;    
} DataFormatteDbgDword5_t;

///  @brief 0xF501117C : Data Formatter Debug Dword 6
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DPP_CMD_IDX         : 9;         ///<BIT [8:0] DPP Command Index.
        uint32_t RF501117C_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
        uint32_t DPP_BYTE_CNT        :16;         ///<BIT [31:16] DPP Byte Count.
    } b;    
} DataFormatteDbgDword6_t;

///  @brief 0xF5011180 : Data Formatter Debug Dword 7
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DPP_LBA_CNT          : 6;         ///<BIT [5:0] DPP LBA Count.
        uint32_t RF5011180_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint32_t DPP_LBAF             : 4;         ///<BIT [11:8] DPP LBA Format.
        uint32_t RF5011180_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t DPP_NSID             : 3;         ///<BIT [18:16] DPP Namespace ID.
        uint32_t RF5011180_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} DataFormatteDbgDword7_t;

///  @brief 0xF5011190 : Data Manager Write Engine AXI Request Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AXI_REQ_SM          : 5;         ///<BIT [4:0] AXI Request State Machine.
        uint32_t RF5011190_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} DataManagerWrEngineAxiReqStat_t;

///  @brief 0xF5011194 : AXI Request Debug Dword 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LBA_CMD_IDX         : 9;         ///<BIT [8:0] LBA Command Index.
        uint32_t RF5011194_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
        uint32_t LBA_DATA_IN_CNT     :16;         ///<BIT [31:16] Write Command Transfer LBA Block Count.
    } b;    
} AxiReqDbgDword0_t;

///  @brief 0xF501119C : AXI Request Debug Dword 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SSD_SEG_BLK_CNT      :16;         ///<BIT [15:0] Subsegment Descriptor Segment Block Count.
        uint32_t SLBA                 : 8;         ///<BIT [23:16] Subsegment LBA.
        uint32_t RF501119C_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} AxiReqDbgDword2_t;

///  @brief 0xF50111A4 : AXI Request Debug Dword 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INSERT_PI           : 1;         ///<BIT [0] Insert PI.
        uint32_t SSD_EN              : 1;         ///<BIT [1] Subsegment Descriptor Enable.
        uint32_t SC_EN               : 1;         ///<BIT [2] SC Enable.
        uint32_t RF50111A4_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} AxiReqDbgDword4_t;

///  @brief 0xF50111A8 : AXI Write Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AXI_WR_SM            : 5;         ///<BIT [4:0] AXI Read State Machine.
        uint32_t RF50111A8_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t AXI_WR_Q_STAT        : 4;         ///<BIT [11:8] AXI WR Queue Status.
        uint32_t RF50111A8_31_12_RSVD :20;         ///<BIT [31:12] Reserved.
    } b;    
} AxiWrStat_t;

///  @brief 0xF50111AC : Data Manager Write Engine AXI Write Debug Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PCIE_Q_SEL          : 4;         ///<BIT [3:0] Select PCIe Read Status Queue.
        uint32_t RF50111AC_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} DataManagerWrEngineAxiWrDbgCtrl_t;

///  @brief 0xF50111B0 : AXI Write Debug Dword 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AXI_CMD_IDX         : 9;         ///<BIT [8:0] AXI Command Index.
        uint32_t RF50111B0_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
        uint32_t AXI_DATA_IN_CNT     :16;         ///<BIT [31:16] AXI Write Command Transfer LBA Block Count.
    } b;    
} AxiWrDbgDword0_t;

///  @brief 0xF50111B8 : AXI Write Debug Dword 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AXI_SSD_SEG_BLK_CNT  :16;         ///<BIT [15:0] AXI Subsegment Descriptor Segment Block Count.
        uint32_t AXI_SLBA             : 8;         ///<BIT [23:16] AXI Subsegment LBA.
        uint32_t RF50111B8_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} AxiWrDbgDword2_t;

///  @brief 0xF50111C0 : AXI Write Debug Dword 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AXI_INSERT_PI       : 1;         ///<BIT [0] AXI Insert PI.
        uint32_t AXI_SSD_EN          : 1;         ///<BIT [1] AXU Subsegment Descriptor Enable.
        uint32_t AXI_SC_EN           : 1;         ///<BIT [2] AXI SC Enable.
        uint32_t RF50111C0_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} AxiWrDbgDword4_t;

///  @brief 0xF50111C4 : AXI Write Debug Dword 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AXI_SEG_NUM          : 8;         ///<BIT [7:0] AXI Segment Number.
        uint32_t LBAF                 : 4;         ///<BIT [11:8] LBA Format of the Currently Running Write Command.
        uint32_t RF50111C4_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t NEW                  : 1;         ///<BIT [16] New Command.
        uint32_t RF50111C4_31_17_RSVD :15;         ///<BIT [31:17] Reserved.
    } b;    
} AxiWrDbgDword5_t;

///  @brief 0xF50111D8 : Write Command In Order FIFO Write Port
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t WR_CMD_INDEX        : 8;         ///<BIT [7:0] Write Command Index.
        uint32_t RF50111D8_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} WrCmdInOrderFifoWrPort_t;

///  @brief 0xF50111DC : Write Command In Order FIFO Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FIFO_EMPTY          : 1;         ///<BIT [0] Fifo Empty.
        uint32_t FIFO_FULL           : 1;         ///<BIT [1] Fifo Full.
        uint32_t FIFO_COUNT          : 5;         ///<BIT [6:2] Fifo Count.
        uint32_t RF50111DC_31_7_RSVD :25;         ///<BIT [31:7] Reserved.
    } b;    
} WrCmdInOrderFifoStat_t;

///  @brief 0xF50111E0 : Read Command In Order FIFO Write Port
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RD_CMD_INDEX        : 8;         ///<BIT [7:0] Read Command Index.
        uint32_t RF50111E0_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} RdCmdInOrderFifoWrPort_t;

///  @brief 0xF50111E4 : Read Command In Order FIFO Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FIFO_EMPTY          : 1;         ///<BIT [0] Fifo Empty.
        uint32_t FIFO_FULL           : 1;         ///<BIT [1] Fifo Full.
        uint32_t FIFO_COUNT          : 5;         ///<BIT [6:2] Fifo Count.
        uint32_t RF50111E4_31_7_RSVD :25;         ///<BIT [31:7] Reserved.
    } b;    
} RdCmdInOrderFifoStat_t;

/// @brief 0xF5011000-0xF50111C8 : NvmeDataMngr_t MODULE
typedef struct
{
    DataManagerCfg0_t dataManagerCfg0;                                             ///< 0xF5011000 : Data Manager Configuration 0
    uint8_t rsvd_RF5011004[4];                                                    ///< 0xF5011004 : Reserved.
    DataManagerCfg1_t dataManagerCfg1;                                            ///< 0xF5011008 : Data Manager Configuration 0
    uint8_t rsvd_RF501100C[4];                                                    ///< 0xF501100C : Reserved.
    DataManagerCtrl0_t dataManagerCtrl0;                                           ///< 0xF5011010 : Data Manager Control 0
    DataManagerCtrl1_t dataManagerCtrl1;                                           ///< 0xF5011014 : Data Manager Control 1
    DataManagerCtrl2_t dataManagerCtrl2;                                           ///< 0xF5011018 : Data Manager Control 2
    uint8_t rsvd_RF501101C[4];                                                     ///< 0xF501101C - 0xF501101F : Reserved.
    DataManagerStat0_t dataManagerStat0;                                           ///< 0xF5011020 : Data Manager Status 0
    DataManagerStat1_t dataManagerStat1;                                           ///< 0xF5011024 : Data Manager Status 1
    uint8_t rsvd_RF5011028[24];                                                    ///< 0xF5011028 - 0xF501103F : Reserved.
    LbaFmt_t lbaFmt[4];                                                            ///< 0xF5011040 - 0xF501104F : LBA Format 0-3
    uint8_t rsvd_RF5011050[48];                                                    ///< 0xF5011050 - 0xF501107F : Reserved.
    PrpSetupEngineCfg_t prpSetupEngineCfg;                                         ///< 0xF5011080 : PRP Setup Engine Configuration
    DataManagerErrInformat_t dataManagerErrInformat;                               ///< 0xF5011084 : Data Manager Error Information
    uint32_t DATM_ERR_DWORD[2];                                                    ///< 0xF5011088 - 0xF501108C : Data Manager Error Message Dword 0/1
    DataManagerWrEngineStat_t dataManagerWrEngineStat;                             ///< 0xF5011090 : Data Manager Write Engine Status
    DataManagerWrEngineCtrl_t dataManagerWrEngineCtrl;                             ///< 0xF5011094 : Data Manager Write Engine Control
    uint8_t rsvd_RF5011098[56];                                                    ///< 0xF5011098 - 0xF50110CF : Reserved.
    DataManagerSsdFetcherCtrl_t dataManagerSsdFetcherCtrl;                         ///< 0xF50110D0 : Data Manager SSD Fetcher Control
    uint32_t DATM_SSD_WR_CMD_BASE_ADDR;                                            ///< 0xF50110D4 : Write CMD SSD Base Address
    uint32_t DATM_SSD_RD_CMD_BASE_ADDR;                                            ///< 0xF50110D8 : Read CMD SSD Base Address
    SsdListMaxEntryCnt_t ssdListMaxEntryCnt;                                       ///< 0xF50110DC : SSD List Maximum Entry Count
    uint32_t DATM_SSD_CURR_PTR;                                                    ///< 0xF50110E0 : Data Manager Current Segment SSD Pointer
    uint32_t DATM_SSD_CURR_OFF;                                                    ///< 0xF50110E4 : Data Manager Current Segment SSD Offset
    uint32_t DATM_SSD_SET_PTR;                                                     ///< 0xF50110E8 : Data Manager Current Set SSD Pointer
    uint32_t DATM_SSD_SET_OFF;                                                     ///< 0xF50110EC : Data Manager Current Set SSD Offset
    DataManagerAxiAbortCtrl_t dataManagerAxiAbortCtrl;                             ///< 0xF50110F0 : Data Manager AXI Abort Control
    DataManagerAxiAbortStat_t dataManagerAxiAbortStat;                             ///< 0xF50110F4 : Data Manager AXI Abort Status
    uint8_t rsvd_RF50110F8[8];                                                     ///< 0xF50110F8 - 0xF50110FF : Reserved.
    uint32_t DATM_RD_CMD_CNT;                                                      ///< 0xF5011100 : Data Manager Read Command Count
    uint32_t DATM_RD_DATA_CNT;                                                     ///< 0xF5011104 : Data Manager Read Data Count
    uint32_t DATM_WR_CMD_CNT;                                                      ///< 0xF5011108 : Data Manager Write Command Count
    uint32_t DATM_WR_DATA_CNT;                                                     ///< 0xF501110C : Data Manager Write Data Count
    DataManagerDabWrCfg_t dataManagerDabWrCfg;                                     ///< 0xF5011110 : Data Manager DAB Write Configuration
    DataManagerDabRdCfg_t dataManagerDabRdCfg;                                     ///< 0xF5011114 : Data Manager DAB Read Configuration
    uint32_t DATM_DEBUG0;                                                          ///< 0xF5011118 : Data Manager Debug 0
    uint32_t DATM_DEBUG1;                                                          ///< 0xF501111C : Data Manager Debug 1
    DataManagerMonitorSgnSel_t dataManagerMonitorSgnSel;                           ///< 0xF5011120 : Data Manager Monitor Signal Select
    DataManagerWrEnginePcieStat_t dataManagerWrEnginePcieStat;                     ///< 0xF5011124 : Data Manager Write Engine PCIe Status
    DataManagerWrEnginePcieDbgCtrl_t dataManagerWrEnginePcieDbgCtrl;               ///< 0xF5011128 : Data Manager Write Engine PCIe Debug Control
    PcieRdDbgDword0_t pcieRdDbgDword0;                                             ///< 0xF501112C : PCIe Read Debug Dword 0
    uint32_t PCIE_META_PTR_DW0;                                                    ///< 0xF5011130 : PCIe Read Debug Dword 1
    uint32_t PCIE_META_PTR_DW1;                                                    ///< 0xF5011134 : PCIe Read Debug Dword 2
    uint32_t PCIE_RD_ADDR_DW0;                                                     ///< 0xF5011138 : PCIe Read Debug Dword 3
    uint32_t PCIE_RD_ADDR_DW1;                                                     ///< 0xF501113C : PCIe Read Debug Dword 4
    PcieRdDbgDword5_t pcieRdDbgDword5;                                             ///< 0xF5011140 : PCIe Read Debug Dword 5
    uint32_t PCIE_RD_DATA_CNT;                                                     ///< 0xF5011144 : PCIe Read Debug Dword 6
    uint32_t LBA_DW0;                                                              ///< 0xF5011148 : PCIe Read Debug Dword 7
    uint32_t LBA_DW1;                                                              ///< 0xF501114C : PCIe Read Debug Dword 8
    PcieRdDbgDword9_t pcieRdDbgDword9;                                             ///< 0xF5011150 : PCIe Read Debug Dword 9
    uint32_t PCIE_ACMD_DW_SIZE;                                                    ///< 0xF5011154 : PCIe Read Debug Dword 10
    uint32_t LBRT;                                                                 ///< 0xF5011158 : PCIe Read Debug Dword 11
    PcieRdDbgDword12_t pcieRdDbgDword12;                                           ///< 0xF501115C : PCIe Read Debug Dword 12
    DataManagerWrEngineDataFormatteStat_t dataManagerWrEngineDataFormatteStat;     ///< 0xF5011160 : Data Manager Write Engine Data Formatter Status
    DataFormatteDbgDword0_t dataFormatteDbgDword0;                                 ///< 0xF5011164 : Data Formatter Debug Dword 0
    uint32_t START_LBA_DW0;                                                        ///< 0xF5011168 : Data Formatter Debug Dword 1
    uint32_t START_LBA_DW1;                                                        ///< 0xF501116C : Data Formatter Debug Dword 2
    DataFormatteDbgDword3_t dataFormatteDbgDword3;                                 ///< 0xF5011170 : Data Formatter Debug Dword 3
    DataFormatteDbgDword4_t dataFormatteDbgDword4;                                 ///< 0xF5011174 : Data Formatter Debug Dword 4
    DataFormatteDbgDword5_t dataFormatteDbgDword5;                                 ///< 0xF5011178 : Data Formatter Debug Dword 5
    DataFormatteDbgDword6_t dataFormatteDbgDword6;                                 ///< 0xF501117C : Data Formatter Debug Dword 6
    DataFormatteDbgDword7_t dataFormatteDbgDword7;                                 ///< 0xF5011180 : Data Formatter Debug Dword 7
    uint32_t DPP_SLBA_DW0;                                                         ///< 0xF5011184 : Data Formatter Debug Dword 8
    uint32_t DPP_SLBA_DW1;                                                         ///< 0xF5011188 : Data Formatter Debug Dword 9
    uint32_t DPP_REMAIN_CNT;                                                       ///< 0xF501118C : Data Formatter Debug Dword 10
    DataManagerWrEngineAxiReqStat_t dataManagerWrEngineAxiReqStat;                 ///< 0xF5011190 : Data Manager Write Engine AXI Request Status
    AxiReqDbgDword0_t axiReqDbgDword0;                                             ///< 0xF5011194 : AXI Request Debug Dword 0
    uint32_t AXI_REQ_ACMD_DW_SIZE;                                                 ///< 0xF5011198 : AXI Request Debug Dword 1
    AxiReqDbgDword2_t axiReqDbgDword2;                                             ///< 0xF501119C : AXI Request Debug Dword 2
    uint32_t RF50111A0_31_0_RSVD;                                                  ///< 0xF50111A0 : AXI Request Debug Dword 3
    AxiReqDbgDword4_t axiReqDbgDword4;                                             ///< 0xF50111A4 : AXI Request Debug Dword 4
    AxiWrStat_t axiWrStat;                                                         ///< 0xF50111A8 : AXI Write Status
    DataManagerWrEngineAxiWrDbgCtrl_t dataManagerWrEngineAxiWrDbgCtrl;             ///< 0xF50111AC : Data Manager Write Engine AXI Write Debug Control
    AxiWrDbgDword0_t axiWrDbgDword0;                                               ///< 0xF50111B0 : AXI Write Debug Dword 0
    uint32_t AXI_ACMD_DW_SIZE;                                                     ///< 0xF50111B4 : AXI Write Debug Dword 1
    AxiWrDbgDword2_t axiWrDbgDword2;                                               ///< 0xF50111B8 : AXI Write Debug Dword 2
    uint32_t RF50111BC_31_0_RSVD;                                                  ///< 0xF50111BC : AXI Write Debug Dword 3
    AxiWrDbgDword4_t axiWrDbgDword4;                                               ///< 0xF50111C0 : AXI Write Debug Dword 4
    AxiWrDbgDword5_t axiWrDbgDword5;                                               ///< 0xF50111C4 : AXI Write Debug Dword 5
    uint8_t rsvd_RF50111C8[16];                                                    ///< 0xF50111C8 - 0xF50111D7 : Reserved.
    WrCmdInOrderFifoWrPort_t wrCmdInOrderFifoWrPort;                               ///< 0xF50111D8 : Write Command In Order FIFO Write Port
    WrCmdInOrderFifoStat_t wrCmdInOrderFifoStat;                                   ///< 0xF50111DC : Write Command In Order FIFO Status
    RdCmdInOrderFifoWrPort_t rdCmdInOrderFifoWrPort;                               ///< 0xF50111E0 : Read Command In Order FIFO Write Port
    RdCmdInOrderFifoStat_t rdCmdInOrderFifoStat;                                   ///< 0xF50111E4 : Read Command In Order FIFO Status
} NvmeDataMngr_t;
COMPILE_ASSERT(offsetof(NvmeDataMngr_t,axiWrDbgDword5)==0xF50111C4-0xF5011000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile NvmeDataMngr_t rNvmeDataMngr;        ///< 0xF5011000-0xF50111C8 : NvmeDataMngr_t MODULE

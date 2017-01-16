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
//! @brief PCIeDMA Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  PCIeDMA Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF0104000 : PCIe DMA Control 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF0104000_3_0_RSVD  : 4;         ///<BIT [3:0] Reserved.
        uint16_t RF0104000_4_RSVD    : 1;         ///<BIT [4] Reserved.
        uint16_t RF0104000_5_RSVD    : 1;         ///<BIT [5] Reserved.
        uint16_t PDMA_FMT_BYPASS_EN  : 1;         ///<BIT [6] PDMA Data Formatter Bypass Enable for Cut-Through Mode.
        uint16_t RF0104000_7_RSVD    : 1;         ///<BIT [7] Reserved.
        uint16_t PDMA_DMASETUPFIS_EN : 1;         ///<BIT [8] PDMA Send DMA Setup FIS Enable.
        uint16_t PDMA_PIOSETUPFIS_EN : 1;         ///<BIT [9] PDMA PIO Setup FIS Enable.
        uint16_t RF0104000_10_RSVD   : 1;         ///<BIT [10] Reserved.
        uint16_t PRD_FETCH_BYPASS    : 1;         ///<BIT [11] PDMA PRD Fetch Bypass.
        uint16_t RF0104000_12_RSVD   : 1;         ///<BIT [12] Reserved.
        uint16_t RF0104000_13_RSVD   : 1;         ///<BIT [13] Reserved.
        uint16_t RF0104000_14_RSVD   : 1;         ///<BIT [14] Reserved.
        uint16_t RF0104000_15_RSVD   : 1;         ///<BIT [15] Reserved.
    } b;    
} PcieDmaCtrl0_t;

///  @brief 0xF0104002 : PCIe DMA Control 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PDMA_SRESET           : 1;         ///<BIT [0] PCIe DMA Soft Reset.
        uint16_t PDMA_RST_CMD_DONE_CNT : 1;         ///<BIT [1] PCIe DMA Reset Command Done Count.
        uint16_t PDMA_ABT_EN           : 1;         ///<BIT [2] PCIe DMA Abort Enable.
        uint16_t RF0104002_3_RSVD      : 1;         ///<BIT [3] Reserved.
        uint16_t RF0104002_4_RSVD      : 1;         ///<BIT [4] Reserved.
        uint16_t RF0104002_7_5_RSVD    : 3;         ///<BIT [7:5] Reserved.
        uint16_t SDB_DISABLE           : 1;         ///<BIT [8] PCIe DMA SDB Disable.
        uint16_t RF0104002_11_9_RSVD   : 3;         ///<BIT [11:9] Reserved.
        uint16_t ERR_FIRST_SECNUM_SEL  : 4;         ///<BIT [15:12] Select error status for NUM_SECTOR_AT_FIRST_ERR. (Rx040h[31:0].
    } b;    
} PcieDmaCtrl1_t;

///  @brief 0xF0104004 : PCIe DMA Control 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t MPECC_EN             : 1;         ///<BIT [0] Memory Protection ECC Enable.
        uint16_t RF0104004_3_1_RSVD   : 3;         ///<BIT [3:1] Reserved.
        uint16_t HLBA_EN              : 1;         ///<BIT [4] Host LBA Enable.
        uint16_t RF0104004_5_RSVD     : 1;         ///<BIT [5] Reserved.
        uint16_t HLBA_SIZE            : 2;         ///<BIT [7:6] Host LBA Size.
        uint16_t BCRC_EN              : 1;         ///<BIT [8] BCRC Enable.
        uint16_t RF0104004_11_9_RSVD  : 3;         ///<BIT [11:9] Reserved.
        uint16_t MPECC_CMB0_SEP1      : 1;         ///<BIT [12] MPECC Combine Separate Mode Select.
        uint16_t RF0104004_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
    } b;    
} PcieDmaCtrl2_t;

///  @brief 0xF0104006 : PCIe DMA Control 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_SECTOR_SIZE     :13;         ///<BIT [12:0] Host Sector Size in Bytes.
        uint16_t RF0104006_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
    } b;    
} PcieDmaCtrl3_t;

///  @brief 0xF0104008 : PCIe DMA Control 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t AES_EN                : 1;         ///<BIT [0] AES Enable.
        uint16_t AES_EXTERNAL_KEYIV_EN : 1;         ///<BIT [1] AES External Key Initialization Vector Enable.
        uint16_t R1A103008_3_2_RSVD    : 2;         ///<BIT [3:2] Reserved.
        uint16_t AES_SLF_CHK_EN        : 1;         ///<BIT [4] AES Self-Check Enable.
        uint16_t R1A103008_7_5_RSVD    : 3;         ///<BIT [7:5] Reserved.
        uint16_t AES_IV_SWAP_CTRL      : 2;         ///<BIT [9:8] AES IV Byte Swap Control.
        uint16_t AES_KEY_SWAP_CTRL     : 2;         ///<BIT [11:10] AES Key Byte Swap Control.
        uint16_t AES_DATA_SWAP_CTRL    : 2;         ///<BIT [13:12] AES Data Byte Swap Control.
        uint16_t R1A103008_15_14_RSVD  : 2;         ///<BIT [15:14] Reserved.
    } b;    
} PcieDmaCtrl4_t;

///  @brief 0xF010400E : PCIe DMA Control 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DDRMC_AXI_PARITY_CHK_EN  : 1;         ///<BIT [0] PCIe DMA DDRMC AXI Parity Check Enable.
        uint16_t PCIE_AXI_PARITY_CHK_EN   : 1;         ///<BIT [1] PCIe DMA AXI Parity Check Enable.
        uint16_t UMC_AXI_PARITY_POLARITY  : 1;         ///<BIT [2] UMC AXI Bus Parity Polarity.
        uint16_t PCIE_AXI_PARITY_POLARITY : 1;         ///<BIT [3] PCIe AXI Bus Parity Polarity.
        uint16_t RF010400E_15_4_RSVD      :12;         ///<BIT [15:4] Reserved.
    } b;    
} PcieDmaCtrl7_t;

///  @brief 0xF0104020 : PCIe DMA Status 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERR_INT_EN              : 1;         ///<BIT [0] PCIe DMA Interrupt by Error Enable.
        uint16_t INT_EN_LAST_NOSTOP      : 1;         ///<BIT [1] PCIe DMA CPU Interrupt on Non-Stop Last Command Enable.
        uint16_t RF0104020_2_RSVD        : 1;         ///<BIT [2] Reserved.
        uint16_t RF0104020_3_RSVD        : 1;         ///<BIT [3] Reserved.
        uint16_t ERRMASK_MPECC_ERR       : 1;         ///<BIT [4] Error Mask for MPECC Error.
        uint16_t ERRMASK_HLBA_MMATCH     : 1;         ///<BIT [5] Error Mask for Host LBA Mismatch Error.
        uint16_t ERRMASK_BCRC_ERR        : 1;         ///<BIT [6] Error Mask for BCRC Error.
        uint16_t ERRMASK_ECU_ERR         : 1;         ///<BIT [7] Error Mask for ECU Error passed through Cut-Through Mode.
        uint16_t RF0104020_8_RSVD        : 1;         ///<BIT [8] Reserved.
        uint16_t ERRMASK_IV_FIFO_DPP_ERR : 1;         ///<BIT [9] Error Mask for IV FIFO DPP Error.
        uint16_t RF0104020_15_10_RSVD    : 6;         ///<BIT [15:10] Reserved.
    } b;    
} PcieDmaStat0_t;

///  @brief 0xF0104022 : PCIe DMA Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CLEAR_STATUS        : 1;         ///<BIT [0] Clear PCIe DMA Status.
        uint16_t CLEAR_INT_NOSTOP    : 1;         ///<BIT [1] Clear the PCIe DMA No Stop at Last Command interrupt.
        uint16_t RF0104022_2_RSVD    : 1;         ///<BIT [2] Reserved.
        uint16_t RF0104022_15_3_RSVD :13;         ///<BIT [15:3] Reserved.
    } b;    
} PcieDmaStat1_t;

///  @brief 0xF0104024 : PCIe DMA Status 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF0104024_2_0_RSVD   : 3;         ///<BIT [2:0] Reserved.
        uint16_t PDMA_ERROR           : 1;         ///<BIT [3] PCIe DMA Error.
        uint16_t MPECC_ERR            : 1;         ///<BIT [4] MPECC Error Status.
        uint16_t HLBA_MMATCH          : 1;         ///<BIT [5] Host LBA Mismatch Error Status.
        uint16_t BCRC_ERR             : 1;         ///<BIT [6] BCRC Error Status.
        uint16_t ECU_ERR              : 1;         ///<BIT [7] ECU Error Status passed through Cut-Through Mode.
        uint16_t RF0104024_8_RSVD     : 1;         ///<BIT [8] Reserved.
        uint16_t PRD_FETCH_ERR        : 1;         ///<BIT [9] PRD Fetch Error Status.
        uint16_t XFERCNT_MMATCH       : 1;         ///<BIT [10] Transfer Count Mismatch Error Status.
        uint16_t AES_ERROR            : 1;         ///<BIT [11] AES Error Status.
        uint16_t AXI1_RD_TO_ERR       : 1;         ///<BIT [12] PCIe AXI Read Time Out Error Status.
        uint16_t AXI0_RD_TO_ERR       : 1;         ///<BIT [13] UMC AXI Read Time Out Error Status.
        uint16_t RF0104024_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} PcieDmaStat2_t;

///  @brief 0xF0104026 : PCIe DMA Status 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF0104026_0_RSVD              : 1;         ///<BIT [0] Reserved.
        uint16_t RF0104026_1_RSVD              : 1;         ///<BIT [1] Reserved.
        uint16_t PDMA_ABT_AXI_PENDING_COMPLETE : 1;         ///<BIT [2] PCIe DMA Abort AXI Pending Request Complete Status.
        uint16_t RF0104026_7_3_RSVD            : 5;         ///<BIT [7:3] Reserved.
        uint16_t INT_ERR                       : 1;         ///<BIT [8] PCIe DMA Interrupt Due to Error.
        uint16_t INT_LAST_CMD                  : 1;         ///<BIT [9] Interrupt with Last Command.
        uint16_t INT_SDB_EN                    : 1;         ///<BIT [10] Interrupt with SDB Enable.
        uint16_t RF0104026_11_RSVD             : 1;         ///<BIT [11] Reserved.
        uint16_t INT_LAST_NOSTOP               : 1;         ///<BIT [12] PCIe DMA Last Command Interrupt.
        uint16_t RF0104026_13_RSVD             : 1;         ///<BIT [13] Reserved.
        uint16_t RF0104026_15_14_RSVD          : 2;         ///<BIT [15:14] Reserved.
    } b;    
} PcieDmaStat3_t;

///  @brief 0xF0104028 : PCIe DMA Status 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF0104028_1_0_RSVD      : 2;         ///<BIT [1:0] Reserved.
        uint16_t ERRMASK_AXI0_PARITY_ERR : 1;         ///<BIT [2] Error Mask for UMC AXI Data Bus Parity Error.
        uint16_t RF0104028_4_3_RSVD      : 2;         ///<BIT [4:3] Reserved.
        uint16_t ERRMASK_AXI1_PARITY_ERR : 1;         ///<BIT [5] Error Mask for PCIe AXI Data Bus Parity Error.
        uint16_t RF0104028_7_6_RSVD      : 2;         ///<BIT [7:6] Reserved.
        uint16_t ERR_NONCRITICAL         : 1;         ///<BIT [8] Non-Critical Error.
        uint16_t ERR_CRITICAL            : 1;         ///<BIT [9] Critical Error.
        uint16_t RF0104028_15_10_RSVD    : 6;         ///<BIT [15:10] Reserved.
    } b;    
} PcieDmaStat4_t;

///  @brief 0xF010402A : PCIe DMA Status 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t AXI0_RD_RESP_ERR   : 2;         ///<BIT [1:0] UMC Axi RESP Error Status.
        uint16_t AXI0_RD_PARITY_ERR : 1;         ///<BIT [2] UMC AXI READ Parity Error Status.
        uint16_t AXI1_RD_RESP_ERR   : 2;         ///<BIT [4:3] PCIe AXI RESP Error Status.
        uint16_t AXI1_RD_PARITY_ERR : 1;         ///<BIT [5] PCIe AXI READ Parity Error Status.
        uint16_t AXI0_WR_BRESP_ERR  : 2;         ///<BIT [7:6] UMC AXI BRESP Error Status.
        uint16_t AXI1_WR_BRESP_ERR  : 2;         ///<BIT [9:8] PCIe AXI BRESP Error Status.
        uint16_t AXI0_WID_ERR       : 1;         ///<BIT [10] UMC AXI WID Error Status.
        uint16_t AXI0_RID_ERR       : 1;         ///<BIT [11] UMC AXI RID Error Status.
        uint16_t AXI1_WID_ERR       : 1;         ///<BIT [12] PCIe AXI WID Error Status.
        uint16_t AXI1_RID_ERR       : 1;         ///<BIT [13] PCIe AXI RID Error Status.
        uint16_t CMDQ_DPP_ERR       : 1;         ///<BIT [14] Command Queue DPP Error Status.
        uint16_t PRD_FIFO_DPP_ERR   : 1;         ///<BIT [15] PRD FIFO DPP Error Status.
    } b;    
} PcieDmaStat5_t;

///  @brief 0xF010402E : PCIe DMA Status 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FREE_CMD_SLOT_CNT    : 6;         ///<BIT [5:0] Number of Available Command Slots.
        uint16_t RF010402E_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint16_t PRD_FIFO_FULL        : 1;         ///<BIT [8] PRD FIFO Full Status.
        uint16_t PRD_FIFO_EMPTY       : 1;         ///<BIT [9] PRD FIFO Empty Status.
        uint16_t RF010402E_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} PcieDmaStat7_t;

///  @brief 0xF010404C : PCIe DMA Error Tag
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERR_TAG             : 5;         ///<BIT [4:0] Error Command Tag ID.
        uint16_t RF010404C_15_5_RSVD :11;         ///<BIT [15:5] Reserved.
    } b;    
} PcieDmaErrTag_t;

///  @brief 0xF01040A0 : PCIe DMA Current Segment Select Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t READ_SEG_SEL        : 3;         ///<BIT [2:0] Segment Select Read Pointer.
        uint16_t RF01040A0_15_3_RSVD :13;         ///<BIT [15:3] Reserved.
    } b;    
} PcieDmaCurSgmSelStat_t;

///  @brief 0xF01040C0 : PCIe DMA Setup FIS Configuration 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DMASETUPFIS_A        : 1;         ///<BIT [0] DMA Setup FIS A Field Value.
        uint16_t DMASETUPFIS_I        : 1;         ///<BIT [1] DMA Setup FIS I Field Value.
        uint16_t SETUPFIS_PM          : 4;         ///<BIT [5:2] DMA Setup FIS PM Field Value.
        uint16_t SETUPFIS_R           : 1;         ///<BIT [6] DMA Setup FIS or PIO Setup FIS R Field Value.
        uint16_t RF01040C0_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint16_t SDBFIS_R             : 1;         ///<BIT [8] SDB FIS R Field Value.
        uint16_t SDBFIS_PMPORT        : 4;         ///<BIT [12:9] SDB FIS PM Port Field Value.
        uint16_t RF01040C0_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
    } b;    
} PcieDmaSetupFisCfg0_t;

///  @brief 0xF01040C2 : PCIe DMA FIS Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PIO_E_STATUS      : 8;         ///<BIT [7:0] PIO SETUP FIS E_STATUS Field Value.
        uint16_t PIO_E_STATUS_LAST : 8;         ///<BIT [15:8] PIO SETUP FIS STATUS_LAST Field Value.
    } b;    
} PcieDmaFisCfg1_t;

///  @brief 0xF01040C4 : FIS Host Base Address 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PIO_STATUS          : 8;         ///<BIT [7:0] PIO FIS Status Field Value.
        uint16_t RF01040C4_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} FisHostBaseAddr0_t;

///  @brief 0xF01040D0 : PCIe DMA Buffer CPU Access Control 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PDMA_BUF_CPU_EN               : 1;         ///<BIT [0] PCIe DMA Memory CPU Access Active Enable.
        uint16_t PDMA_BUF_CPU_ADDR_AUTO_INC_EN : 1;         ///<BIT [1] PCIe DMA Memory CPU Access Address Auto Incremental Enable.
        uint16_t PDMA_BUF_CPU_DIR              : 1;         ///<BIT [2] PCIe DMA Memory CPU Access Direction.
        uint16_t RF01040D0_6_3_RSVD            : 4;         ///<BIT [6:3] Reserved.
        uint16_t PRD_FIFO_PUSH                 : 1;         ///<BIT [7] PRD FIFO Push.
        uint16_t PDMA_BUF_CPU_SEL              : 3;         ///<BIT [10:8] PCIe DMA Memory CPU Access Memory Select.
        uint16_t RF01040D0_15_11_RSVD          : 5;         ///<BIT [15:11] Reserved.
    } b;    
} PcieDmaBufCpuAccessCtrl0_t;

///  @brief 0xF010410E : PCIe DMA Setup FIS Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t SDBFIS_I            : 1;         ///<BIT [0] SDB FIS I Field Value.
        uint16_t SDBFIS_N            : 1;         ///<BIT [1] SDB FIS N Field Value.
        uint16_t SDBFIS_STATUS       : 6;         ///<BIT [7:2] SDB FIS Status Field Value.
        uint16_t RF010410E_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} PcieDmaSetupFisCfg_t;

///  @brief 0xF0104116 : PCIe DMA SSD Segment Configuration 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF0104116_0_RSVD        : 1;         ///<BIT [0] Reserved.
        uint16_t CPU_ASSIGNED_INDX_VALID : 1;         ///<BIT [1] CPU Assigned Index Offset Valid.
        uint16_t RF0104116_3_2_RSVD      : 2;         ///<BIT [3:2] Reserved.
        uint16_t RF0104116_6_4_RSVD      : 3;         ///<BIT [6:4] Reserved.
        uint16_t RF0104116_15_7_RSVD     : 9;         ///<BIT [15:7] Reserved.
    } b;    
} PcieDmaSsdSgmCfg0_t;

///  @brief 0xF0104130 : PCIe DMA Command Configuration 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PDMA_CMD_START     : 1;         ///<BIT [0] PCIe DMA Command Start.
        uint16_t PDMA_CMD_DIRECTION : 1;         ///<BIT [1] PCIe DMA Command Direction.
        uint16_t PDMA_CMD_LAST      : 1;         ///<BIT [2] PCIe DMA Last Command.
        uint16_t STREAM0_CUTTHRU1   : 1;         ///<BIT [3] PCIe DMA Streaming Cut-Through Mode Select on Read Path.
        uint16_t PDMA_SC_BYPASS_EN  : 1;         ///<BIT [4] PDMA Bypass Streaming Control.
        uint16_t PDMA_SDBFIS_EN     : 1;         ///<BIT [5] PCIe DMA SDB FIS Enable.
        uint16_t RF0104130_6_RSVD   : 1;         ///<BIT [6] Reserved.
        uint16_t CMD_LAST_STOP      : 1;         ///<BIT [7] PCIe DMA Stop At Last Command.
        uint16_t TAG_ID             : 5;         ///<BIT [12:8] PCIe DMA Command Tag ID Select.
        uint16_t SEG_SEL            : 3;         ///<BIT [15:13] Segment Select.
    } b;    
} PcieDmaCmdCfg0_t;

///  @brief 0xF0104132 : PCIe DMA Command DMA Setup FIS Tag
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF0104132_7_0_RSVD : 8;         ///<BIT [7:0] Reserved.
        uint16_t SETUPFIS_DEVICE    : 8;         ///<BIT [15:8] PIO SETUP FIS Device Field Value.
    } b;    
} PcieDmaCmdDmaSetupFisTag_t;

///  @brief 0xF01041A0 : PCIe DMA DAB0 Write Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DAB0_CW_ID           : 4;         ///<BIT [3:0] Write ID DAB0.
        uint16_t DAB0_CW_LEN          : 4;         ///<BIT [7:4] Write Burst Length DAB0.
        uint16_t DAB0_CW_LOCK         : 2;         ///<BIT [9:8] Write Lock Type DAB0.
        uint16_t DAB0_CW_BUFFERABLE   : 1;         ///<BIT [10] Bufferable Write Access DAB0.
        uint16_t RF01041A0_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
    } b;    
} PcieDmaDab0WrCfg1_t;

///  @brief 0xF01041A2 : PCIe DMA DAB0 Write Configuration 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DAB0_CW_PROT        : 3;         ///<BIT [2:0] Write Protection Type DAB0.
        uint16_t DAB0_CW_MAXRSP      : 5;         ///<BIT [7:3] Maximum Outstanding Write Response DAB0.
        uint16_t RF01041A2_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} PcieDmaDab0WrCfg2_t;

///  @brief 0xF01041A8 : PCIe DMA DAB0 Read Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DAB0_CR_ID_DATA      : 4;         ///<BIT [3:0] AXI Read ID for Data.
        uint16_t DAB0_CR_LEN          : 4;         ///<BIT [7:4] Read Burst Length DAB0.
        uint16_t DAB0_CR_LOCK         : 2;         ///<BIT [9:8] Read Lock Type DAB0.
        uint16_t RF01041A8_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} PcieDmaDab0RdCfg1_t;

///  @brief 0xF01041AA : PCIe DMA DAB0 Read Configuration 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DAB0_CR_PROT         : 3;         ///<BIT [2:0] Read Protection Type DAB0.
        uint16_t DAB0_CR_MAXREQ       : 4;         ///<BIT [6:3] Maximum Outstanding Read Request DAB0.
        uint16_t RF01041AA_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint16_t RF01041AA_11_8_RSVD  : 4;         ///<BIT [11:8] Reserved.
        uint16_t RF01041AA_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} PcieDmaDab0RdCfg2_t;

///  @brief 0xF01041B0 : PCIe DMA DAB1 Write Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DAB1_CW_ID           : 4;         ///<BIT [3:0] Write ID DAB1.
        uint16_t DAB1_CW_LEN          : 4;         ///<BIT [7:4] Write Burst Length DAB1.
        uint16_t DAB1_CW_LOCK         : 2;         ///<BIT [9:8] Write Lock Type DAB1.
        uint16_t DAB1_CW_BUFFERABLE   : 1;         ///<BIT [10] Bufferable Write Access DAB1.
        uint16_t RF01041B0_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
    } b;    
} PcieDmaDab1WrCfg1_t;

///  @brief 0xF01041B2 : PCIe DMA DAB1 Write Configuration 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DAB1_CW_PROT        : 3;         ///<BIT [2:0] Write Protection Type DAB1.
        uint16_t DAB1_CW_MAXRSP      : 6;         ///<BIT [8:3] Maximum Outstanding Write Response DAB1.
        uint16_t RF01041B2_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
    } b;    
} PcieDmaDab1WrCfg2_t;

///  @brief 0xF01041B6 : PCIe DMA DAB CTO Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DAB_CTO_CYCLE       : 8;         ///<BIT [7:0] DAB Timeout Control Cycle.
        uint16_t RF01041B6_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} PcieDmaDabCtoCfg_t;

///  @brief 0xF01041B8 : PCIe DMA DAB1 Read Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DAB1_CR_ID           : 4;         ///<BIT [3:0] Read ID DAB1.
        uint16_t DAB1_CR_LEN          : 4;         ///<BIT [7:4] Read Burst Length DAB1.
        uint16_t DAB1_CR_LOCK         : 2;         ///<BIT [9:8] Read Lock Type DAB1.
        uint16_t RF01041B8_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} PcieDmaDab1RdCfg1_t;

///  @brief 0xF01041BA : PCIe DMA DAB1 Read Configuration 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DAB1_CR_PROT        : 3;         ///<BIT [2:0] Read Protection Type DAB1.
        uint16_t DAB1_CR_MAXREQ      : 6;         ///<BIT [8:3] Maximum Outstanding Read Request DAB1.
        uint16_t RF01041BA_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
    } b;    
} PcieDmaDab1RdCfg2_t;

///  @brief 0xF01041C0 : PCIe DMA PCIe Side Band Signal Configuration 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PDMA_AWUSER_PCIE_TYPE : 4;         ///<BIT [3:0] PCIe DMA AXI AWUSER Type Field Value.
        uint16_t PDMA_AWUSER_PCIE_IDO  : 1;         ///<BIT [4] PCIe DMA AXI AWUSER IDO Field Value.
        uint16_t PDMA_AWUSER_PCIE_EP   : 1;         ///<BIT [5] PCIe DMA AXI AWUSER EP Field Value.
        uint16_t PDMA_AWUSER_PCIE_ATTR : 2;         ///<BIT [7:6] PCIe DMA AXI AWUSER Attribute Field Value.
        uint16_t PDMA_AWUSER_PCIE_TC   : 3;         ///<BIT [10:8] PCIe DMA AXI AWUSER TC Field Value.
        uint16_t RF01041C0_15_11_RSVD  : 5;         ///<BIT [15:11] Reserved.
    } b;    
} PcieDmaPcieSideBandSgnCfg0_t;

///  @brief 0xF01041C4 : PCIe DMA PCIe Side Band Signal Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PDMA_ARUSER_PCIE_TYPE : 4;         ///<BIT [3:0] PCIe DMA AXI ARUSER Type Field Value.
        uint16_t PDMA_ARUSER_PCIE_IDO  : 1;         ///<BIT [4] PCIe DMA AXI ARUSER IDO Field Value.
        uint16_t PDMA_ARUSER_PCIE_EP   : 1;         ///<BIT [5] PCIe DMA AXI ARUSER EP Field Value.
        uint16_t PDMA_ARUSER_PCIE_ATTR : 2;         ///<BIT [7:6] PCIe DMA AXI ARUSER Attribute Field Value.
        uint16_t PDMA_ARUSER_PCIE_TC   : 3;         ///<BIT [10:8] PCIe DMA AXI ARUSER TC Field Value.
        uint16_t RF01041C4_15_11_RSVD  : 5;         ///<BIT [15:11] Reserved.
    } b;    
} PcieDmaPcieSideBandSgnCfg1_t;

///  @brief 0xF01041CA : PCIe DMA PCIe Side Band Signal Status 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PDMA_BUSER_PCIE_RSP_BCM         : 1;         ///<BIT [0] PCIe DMA BUSER RSP BCM Field Status.
        uint16_t RF01041CA_1_RSVD                : 1;         ///<BIT [1] Reserved.
        uint16_t PDMA_BUSER_PCIE_TC              : 3;         ///<BIT [4:2] PCIe DMA BUSER TC Field Status.
        uint16_t PDMA_BUSER_PCIE_ATTR            : 2;         ///<BIT [6:5] PCIe DMA BUSER Attribute Field Status.
        uint16_t PDMA_BUSER_PCIE_COMPLETE_STATUS : 3;         ///<BIT [9:7] PCIe DMA BUSER Complete Status Field Status.
        uint16_t PDMA_BUSER_PCIE_NONPOSTED_REQ   : 1;         ///<BIT [10] PCIe DMA BUSER Non-posted REQ Field Status.
        uint16_t RF01041CA_15_11_RSVD            : 5;         ///<BIT [15:11] Reserved.
    } b;    
} PcieDmaPcieSideBandSgnStat0_t;

///  @brief 0xF01041CC : PCIe DMA PCIe Side Band Signal Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PDMA_RUSER_PCIE_RSP_BCM         : 1;         ///<BIT [0] PCIe DMA RUSER RSP BCM Field Status.
        uint16_t PDMA_RUSER_PCIE_TD              : 1;         ///<BIT [1] PCIe DMA RUSER TD Field Status.
        uint16_t PDMA_RUSER_PCIE_TC              : 3;         ///<BIT [4:2] PCIe DMA RUSER TC Field Status.
        uint16_t PDMA_RUSER_PCIE_ATTR            : 2;         ///<BIT [6:5] PCIe DMA RUSER Attribute Field Status.
        uint16_t PDMA_RUSER_PCIE_COMPLETE_STATUS : 3;         ///<BIT [9:7] PCIe DMA RUSER Complete Status Field Status.
        uint16_t PDMA_RUSER_PCIE_NONPOSTED_REQ   : 1;         ///<BIT [10] PCIe DMA RUSER Non-posted REQ Field Status.
        uint16_t RF01041CC_15_11_RSVD            : 5;         ///<BIT [15:11] Reserved.
    } b;    
} PcieDmaPcieSideBandSgnStat1_t;

///  @brief 0xF01041E0 : PCIe DMA Memory Pin Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF01041E0_7_0_RSVD   : 8;         ///<BIT [7:0] Reserved.
        uint16_t PDMA_MEM_PDWN        : 1;         ///<BIT [8] Memory PDWN Pin Value.
        uint16_t PDMA_MEM_PDLVMC      : 1;         ///<BIT [9] Memory PDLVMC Pin Value.
        uint16_t PDMA_MEM_PDFVSSM     : 1;         ///<BIT [10] Memory PDFVSSM Pin Value.
        uint16_t RF01041E0_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
    } b;    
} PcieDmaMemPinCfg_t;

///  @brief 0xF01041E2 : PCIe DMA FIFO Threshold Configuration 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PRD_PREFETCH_THRESHOLD : 5;         ///<BIT [4:0] PRD Pre-Fetch Threshold.
        uint16_t RF01041E2_7_5_RSVD     : 3;         ///<BIT [7:5] Reserved.
        uint16_t IV_PREFETCH_THRESHOLD  : 5;         ///<BIT [12:8] Initialization Vertex Pre-Fetch Threshold.
        uint16_t RF01041E2_15_13_RSVD   : 3;         ///<BIT [15:13] Reserved.
    } b;    
} PcieDmaFifoThrCfg0_t;

///  @brief 0xF01041E4 : PCIe DMA FIFO Threshold Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t SSD_PREFETCH_THRESHOLD : 5;         ///<BIT [4:0] SSD Pre-Fetch Threshold.
        uint16_t RF01041E4_15_5_RSVD    :11;         ///<BIT [15:5] Reserved.
    } b;    
} PcieDmaFifoThrCfg1_t;

///  @brief 0xF01041F8 : PCIe DMA Command Queue Read
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CMD_Q_READ_MODE : 1;         ///<BIT [0] Command Queue Read Mode.
        uint16_t CMD_Q_READ_PTR  : 5;         ///<BIT [5:1] Reserved.
        uint16_t CUR_CMD_RD_PTR  : 5;         ///<BIT [10:6] Current Command Queue Read Pointer.
        uint16_t CUR_CMD_WR_PTR  : 5;         ///<BIT [15:11] Current Command Queue Write Pointer.
    } b;    
} PcieDmaCmdQueueRd_t;


/// @brief 0xF0104000-0xF0104200 : PcieDma_t MODULE
typedef struct
{
    PcieDmaCtrl0_t pcieDmaCtrl0;                                   ///< 0xF0104000 : PCIe DMA Control 0
    PcieDmaCtrl1_t pcieDmaCtrl1;                                   ///< 0xF0104002 : PCIe DMA Control 1
    PcieDmaCtrl2_t pcieDmaCtrl2;                                   ///< 0xF0104004 : PCIe DMA Control 2
    PcieDmaCtrl3_t pcieDmaCtrl3;                                   ///< 0xF0104006 : PCIe DMA Control 3
    PcieDmaCtrl4_t pcieDmaCtrl4;                                   ///< 0xF0104008 : PCIe DMA Control 4
    uint16_t PIO_XFER_SIZE;                                        ///< 0xF010400A : PCIe DMA Control 5
    uint8_t rsvd_RF010400C[2];                                     ///< 0xF010400C - 0xF010400D : Reserved.
    PcieDmaCtrl7_t pcieDmaCtrl7;                                   ///< 0xF010400E : PCIe DMA Control 7
    uint8_t rsvd_RF0104010[16];                                    ///< 0xF0104010 - 0xF010401F : Reserved.
    PcieDmaStat0_t pcieDmaStat0;                                   ///< 0xF0104020 : PCIe DMA Status 0
    PcieDmaStat1_t pcieDmaStat1;                                   ///< 0xF0104022 : PCIe DMA Status 1
    PcieDmaStat2_t pcieDmaStat2;                                   ///< 0xF0104024 : PCIe DMA Status 2
    PcieDmaStat3_t pcieDmaStat3;                                   ///< 0xF0104026 : PCIe DMA Status 3
    PcieDmaStat4_t pcieDmaStat4;                                   ///< 0xF0104028 : PCIe DMA Status 4
    PcieDmaStat5_t pcieDmaStat5;                                   ///< 0xF010402A : PCIe DMA Status 5
    uint8_t rsvd_RF010402C[2];                                     ///< 0xF010402C - 0xF010402D : Reserved.
    PcieDmaStat7_t pcieDmaStat7;                                   ///< 0xF010402E : PCIe DMA Status 7
    uint8_t rsvd_RF0104030[4];                                     ///< 0xF0104030 - 0xF0104033 : Reserved.
    uint16_t CMD_START_CNT;                                        ///< 0xF0104034 : PCIe DMA Status Command Start
    uint16_t CMD_DONE_CNT;                                         ///< 0xF0104036 : PCIe DMA Status Command Done
    uint8_t rsvd_RF0104038[12];                                    ///< 0xF0104038 - 0xF0104043 : Reserved.
    uint32_t COMPLETE_TAG_VECTOR;                                  ///< 0xF0104044 : PCIe DMA SDB FIS Completed Tag Vector
    uint32_t PENDING_TAG_VECTOR;                                   ///< 0xF0104048 : PCIe DMA Data Transfer Pending Tag Vector
    PcieDmaErrTag_t pcieDmaErrTag;                                 ///< 0xF010404C : PCIe DMA Error Tag
    uint8_t rsvd_RF010404E[2];                                     ///< 0xF010404E - 0xF010404F : Reserved.
    uint32_t ECU_READ_PORT_ADDRESS;                                ///< 0xF0104050 : PCIe DMA ECU Address Configuration 0
    uint8_t rsvd_RF0104054[12];                                    ///< 0xF0104054 - 0xF010405F : Reserved.
    uint32_t SEG_BASE_ADDR[8];                                     ///< 0xF0104060 - 0xF010407F: PCIe DMA Segment 0-7 Base Address
    uint8_t rsvd_RF0104080[32];                                    ///< 0xF0104080 - 0xF010409F : Reserved.
    PcieDmaCurSgmSelStat_t pcieDmaCurSgmSelStat;                   ///< 0xF01040A0 : PCIe DMA Current Segment Select Status
    uint8_t rsvd_RF01040A2[2];                                     ///< 0xF01040A2 - 0xF01040A3 : Reserved.
    uint32_t CURRENT_OFFSET;                                       ///< 0xF01040A4 : PCIe DMA Current Segment Select
    uint32_t CURRENT_XFER_PTR;                                     ///< 0xF01040A8 : PCIe DMA Current Transfer Pointer Low
    uint16_t CURRENT_INDEXN;                                       ///< 0xF01040AC : PCIe DMA Current SSD Index
    uint8_t rsvd_RF01040AE[18];                                    ///< 0xF01040AE - 0xF01040BF : Reserved.
    PcieDmaSetupFisCfg0_t pcieDmaSetupFisCfg0;                     ///< 0xF01040C0 : PCIe DMA Setup FIS Configuration 0
    PcieDmaFisCfg1_t pcieDmaFisCfg1;                               ///< 0xF01040C2 : PCIe DMA FIS Configuration 1
    FisHostBaseAddr0_t fisHostBaseAddr0;                           ///< 0xF01040C4 : FIS Host Base Address 0
    uint8_t rsvd_RF01040C6[6];                                     ///< 0xF01040C6 - 0xF01040CB : Reserved.
    uint32_t DMASETUPFIS_BUF_OFFSET;                               ///< 0xF01040CC : PCIe DMA Setup FIS Buffer Offset 0
    PcieDmaBufCpuAccessCtrl0_t pcieDmaBufCpuAccessCtrl0;           ///< 0xF01040D0 : PCIe DMA Buffer CPU Access Control 0
    uint16_t PDMA_BUF_CPU_ACC_ADDR;                                ///< 0xF01040D2 : PCIe DMA Buffer CPU Access Control 1
                                                                   ///< 0xF01040D4 - 0xF01040E4 : PCIe DMA Buffer CPU Access Data (Control 2-10)
    uint32_t DBA;                                                  ///< 0xF01040D4 : PCIe DMA Memory CPU Access Data [31:00]
    uint32_t DBAU;                                                 ///< 0xF01040D8 : PCIe DMA Memory CPU Access Data [63:32]
    uint32_t PDMA_BUF_CPU_ACC_DATA_95_64;                          ///< 0xF01040DC : PCIe DMA Memory CPU Access Data [95:64]
    uint32_t DBC;                                                  ///< 0xF01040E0 : PCIe DMA Memory CPU Access Data [127:96]
    uint16_t PDMA_BUF_CPU_ACC_DATA_143_128;                        ///< 0xF01040E4 : PCIe DMA Memory CPU Access Data [143:128]
    uint8_t rsvd_RF01040E6[38];                                    ///< 0xF01040E6 - 0xF010410B : Reserved.
    uint16_t NUM_SECTOR_IN_CMD;                                    ///< 0xF010410C : PCIe DMA Command Sector Number
    PcieDmaSetupFisCfg_t pcieDmaSetupFisCfg;                       ///< 0xF010410E : PCIe DMA Setup FIS Configuration
    uint8_t rsvd_RF0104110[6];                                     ///< 0xF0104110 - 0xF0104115 : Reserved.
    PcieDmaSsdSgmCfg0_t pcieDmaSsdSgmCfg0;                         ///< 0xF0104116 : PCIe DMA SSD Segment Configuration 0
    uint16_t CPU_ASSIGNED_SSD_INDEX;                               ///< 0xF0104118 : PCIe DMA SSD Segment Configuration 1
    uint16_t CPU_ASSIGNED_SSD_OFFSET_15_0;                         ///< 0xF010411A : PCIe DMA SSD Segment Configuration 2
    uint16_t CPU_ASSIGNED_SSD_OFFSET_31_16;                        ///< 0xF010411C : PCIe DMA SSD Segment Configuration 3
    uint8_t rsvd_RF010411E[2];                                     ///< 0xF010411E - 0xF010411F : Reserved.
    uint32_t AES_IV_TABLE_ADDRESS_31_00;                           ///< 0xF0104120 : PCIe DMA AES Initialization Vector Table Address 0
    uint32_t AES_IV_TABLE_ADDRESS_63_32;                           ///< 0xF0104124 : PCIe DMA AES Initialization Vector Table Address 2
    uint32_t REF_HLBA_31_00;                                       ///< 0xF0104128 : PCIe DMA Reference Host LBA 31..0
    uint32_t REF_HLBA_63_32;                                       ///< 0xF010412C : PCIe DMA Reference Host LBA 63..32
    PcieDmaCmdCfg0_t pcieDmaCmdCfg0;                               ///< 0xF0104130 : PCIe DMA Command Configuration 0
    PcieDmaCmdDmaSetupFisTag_t pcieDmaCmdDmaSetupFisTag;           ///< 0xF0104132 : PCIe DMA Command DMA Setup FIS Tag
    uint8_t rsvd_RF0104134[8];                                     ///< 0xF0104134 - 0xF010413B : Reserved.
    uint32_t SDBFIS_PS;                                            ///< 0xF010413C : PCIe DMA Setup FIS PS Field Value
    uint16_t AES_KEY_1[8];                                         ///< 0xF0104140 - 0xF010414F : PCIe DMA AES Key 1 Register 0..7
    uint8_t rsvd_RF0104150[16];                                    ///< 0xF0104150 - 0xF010415F : Reserved.
    uint16_t AES_KEY_2[8];                                         ///< 0xF0104160 - 0xF010416F : PCIe DMA AES Key 2 Register 0..7
    uint8_t rsvd_RF0104170[16];                                    ///< 0xF0104170 - 0xF010417F : Reserved.
    uint16_t AES_IV[8];                                            ///< 0xF0104180 : PCIe DMA AES Initialization Vector 0..7
    uint32_t MPECC_BASE_ADDR;                                      ///< 0xF0104190 : PCIe DMA MPECC Base Address for Separate Mode
    uint32_t AHCI_CMD_MEM_BASE_ADDR;                               ///< 0xF0104194 : AHCI Command Memory Base Address
    uint32_t FIS_BASE_ADDR_31_00;                                  ///< 0xF0104198 : FIS Base Address 0
    uint32_t FIS_BASE_ADDR_63_32;                                  ///< 0xF010419C : FIS Base Address 2
    PcieDmaDab0WrCfg1_t pcieDmaDab0WrCfg1;                         ///< 0xF01041A0 : PCIe DMA DAB0 Write Configuration 1
    PcieDmaDab0WrCfg2_t pcieDmaDab0WrCfg2;                         ///< 0xF01041A2 : PCIe DMA DAB0 Write Configuration 2
    uint8_t rsvd_RF01041A4[2];                                     ///< 0xF01041A4 - 0xF01041A5 : Reserved.
    uint16_t DAB0_USEC_TICK_CYCLE;                                 ///< 0xF01041A6 : PCIe DMA DAB0 Write Configuration 3
    PcieDmaDab0RdCfg1_t pcieDmaDab0RdCfg1;                         ///< 0xF01041A8 : PCIe DMA DAB0 Read Configuration 1
    PcieDmaDab0RdCfg2_t pcieDmaDab0RdCfg2;                         ///< 0xF01041AA : PCIe DMA DAB0 Read Configuration 2
    uint8_t rsvd_RF01041AC[4];                                     ///< 0xF01041AC - 0xF01041AF : Reserved.
    PcieDmaDab1WrCfg1_t pcieDmaDab1WrCfg1;                         ///< 0xF01041B0 : PCIe DMA DAB1 Write Configuration 1
    PcieDmaDab1WrCfg2_t pcieDmaDab1WrCfg2;                         ///< 0xF01041B2 : PCIe DMA DAB1 Write Configuration 2
    uint8_t rsvd_RF01041B4[2];                                     ///< 0xF01041B4 - 0xF01041B5 : Reserved.
    PcieDmaDabCtoCfg_t pcieDmaDabCtoCfg;                           ///< 0xF01041B6 : PCIe DMA DAB CTO Configuration
    PcieDmaDab1RdCfg1_t pcieDmaDab1RdCfg1;                         ///< 0xF01041B8 : PCIe DMA DAB1 Read Configuration 1
    PcieDmaDab1RdCfg2_t pcieDmaDab1RdCfg2;                         ///< 0xF01041BA : PCIe DMA DAB1 Read Configuration 2
    uint8_t rsvd_RF01041BC[4];                                     ///< 0xF01041BC - 0xF01041BF : Reserved.
    PcieDmaPcieSideBandSgnCfg0_t pcieDmaPcieSideBandSgnCfg0;       ///< 0xF01041C0 : PCIe DMA PCIe Side Band Signal Configuration 0
    uint8_t rsvd_RF01041C2[2];                                     ///< 0xF01041C2 - 0xF01041C3 : Reserved.
    PcieDmaPcieSideBandSgnCfg1_t pcieDmaPcieSideBandSgnCfg1;       ///< 0xF01041C4 : PCIe DMA PCIe Side Band Signal Configuration 1
    uint8_t rsvd_RF01041C6[4];                                     ///< 0xF01041C6 - 0xF01041C9 : Reserved.
    PcieDmaPcieSideBandSgnStat0_t pcieDmaPcieSideBandSgnStat0;     ///< 0xF01041CA : PCIe DMA PCIe Side Band Signal Status 0
    PcieDmaPcieSideBandSgnStat1_t pcieDmaPcieSideBandSgnStat1;     ///< 0xF01041CC : PCIe DMA PCIe Side Band Signal Status 1
    uint8_t rsvd_RF01041CE[18];                                    ///< 0xF01041CE - 0xF01041DF : Reserved.
    PcieDmaMemPinCfg_t pcieDmaMemPinCfg;                           ///< 0xF01041E0 : PCIe DMA Memory Pin Configuration
    PcieDmaFifoThrCfg0_t pcieDmaFifoThrCfg0;                       ///< 0xF01041E2 : PCIe DMA FIFO Threshold Configuration 0
    PcieDmaFifoThrCfg1_t pcieDmaFifoThrCfg1;                       ///< 0xF01041E4 : PCIe DMA FIFO Threshold Configuration 1
    uint8_t rsvd_RF01041E6[18];                                    ///< 0xF01041E6 - 0xF01041F7 : Reserved.
    PcieDmaCmdQueueRd_t pcieDmaCmdQueueRd;                         ///< 0xF01041F8 : PCIe DMA Command Queue Read
} PcieDma_t;
COMPILE_ASSERT(offsetof(PcieDma_t,pcieDmaCmdQueueRd)==0xF01041F8-0xF0104000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile PcieDma_t rPcieDma;                  ///< 0xF0104000-0xF0104200 : PcieDma_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
extern PcieDma_t mPcieDma;                  ///< 0xF0104000-0xF0104200 : PcieDma_t MODULE

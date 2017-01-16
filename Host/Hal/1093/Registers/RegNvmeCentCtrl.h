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
//! @brief NVMe Central Control Module Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  NVMe Central Control Module Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF5018000 : Central Control Module Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CENT_TMR_EN         : 1;         ///<BIT [0] Central Control Module Timer Enable.
    uint32_t RF5018000_27_1_RSVD     :27;         ///<BIT [27:1] Reserved.
    uint32_t APB_ERR_EN              : 1;         ///<BIT [28] APB Slave Error Enable.
                                                  ///<         Enable the APB bus error signal (HRESP) from the NVMe Controller to the CPU.
    uint32_t AHB_SLAVE_ERR_EN_CPU    : 1;         ///<BIT [29] AHB Slave Error Enable CPU.
                                                  ///<         Enable the AHB bus error signal (HRESP) from the NVMe Controller to the CPU.
    uint32_t AHB_SLAVE_ERR_EN_H      : 1;         ///<BIT [30] AHB Slave Error Enable Host.
                                                  ///<         Enable the AHB bus error signal (HRESP) from the NVMe Controller to the host.
    uint32_t RF5018000_31_RSVD       : 1;         ///<BIT [31] Reserved.
    } b;    
} CentralCtrlModuleCfg_t;

///  @brief 0xF5018004 : Central Control Module Reset Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_RST             : 1;         ///<BIT [0] Control Register Soft Reset.
        uint32_t CSRAM_RST            : 1;         ///<BIT [1] Command SRAM Soft Reset.
        uint32_t BSRM_RST             : 1;         ///<BIT [2] Bitmap SRAM Soft Reset.
        uint32_t RF5018004_3_RSVD     : 1;         ///<BIT [3] Reserved.
        uint32_t DATM_RST             : 1;         ///<BIT [4] Data Manager Soft Reset.
        uint32_t CMDF_RST             : 1;         ///<BIT [5] Command Fetcher Soft Reset.
        uint32_t STSM_RST             : 1;         ///<BIT [6] Status Manager Soft Reset.
        uint32_t RF5018004_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint32_t QARB_RST             : 1;         ///<BIT [8] Queue Arbiter Soft Reset.
        uint32_t PCIE_PKTM_RST_NVME   : 1;         ///<BIT [9] PCIe Packet Manager Soft Reset (NVMe Controller Clock Domain).
        uint32_t HWLL_RST             : 1;         ///<BIT [10] Hardware Linked List Builder Soft Reset.
        uint32_t RF5018004_12_11_RSVD : 2;         ///<BIT [12:11] Reserved.
        uint32_t CENT_RST             : 1;         ///<BIT [13] Central Control Module Soft Reset.
        uint32_t RF5018004_29_14_RSVD :16;         ///<BIT [29:14] Reserved.
        uint32_t PCIE_CORE_RST        : 1;         ///<BIT [30] PCIe Core Clock Domain Soft Reset.
        uint32_t PCIE_PKTM_RST_PCIE   : 1;         ///<BIT [31] PCIe Packet Manager Soft Reset (PCIe Clock Domain).
    } b;    
} CentralCtrlModuleRstCtrl_t;

///  @brief 0xF5018008 : Central Control Module Timer Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TCK_THR              :16;         ///<BIT [15:0] Ticker Threshold.
        uint32_t RF5018008_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} CentralCtrlModuleTmrCtrl_t;

///  @brief 0xF5018010 : Central Control Module Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CC_EN_CLR            : 1;         ///<BIT [0] NVMe Enable Bit Clear Request Interrupt.
        uint32_t CC_EN_SET            : 1;         ///<BIT [1] NVMe Enable Bit Set Request Interrupt.
        uint32_t CC_SHDN_SET          : 1;         ///<BIT [2] NVMe Shutdown Notification Interrupt.
        uint32_t RF5018010_3_RSVD     : 1;         ///<BIT [3] Reserved.
        uint32_t SUB_SYS              : 1;         ///<BIT [4] NVMe Subsystem Reset Request Interrupt.
        uint32_t RF5018010_5_RSVD     : 1;         ///<BIT [5] Reserved.
        uint32_t SQ_DB                : 1;         ///<BIT [6] NVMe Submission Queue Doorbell Event Interrupt.
        uint32_t RF5018010_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint32_t CSRM_G_STS           : 2;         ///<BIT [9:8] Command SRAM Good Status Interrupt.
                                                   ///<    [0] Admin command SRAM FIFO contains at least one available command.
                                                   ///<    [1] I/O command SRAM FIFO contains at least one available command.
        uint32_t CBC_G_STS            : 1;         ///<BIT [10] Command Base Completion Good Status Interrupt.
        uint32_t BSRM_G_STS           : 1;         ///<BIT [11] Bitmap SRAM Good Status Interrupt.
        uint32_t RF5018010_12_RSVD    : 1;         ///<BIT [12] Reserved.
        uint32_t DATM_G_STS           : 2;         ///<BIT [14:13] Data Manager Good Status Interrupt.
                                                   ///<    [0] The Admin Transfer Done interrupt for ATDBM
                                                   ///<    [1] The Transfer Done 64 interrupt (TD64BM), 
                                                   ///<        which indicates the completion of 64 AU transfers of large read commands.
        uint32_t STSM_G_STS           : 1;         ///<BIT [15] Status Manager Good Status Interrupt.
        uint32_t RF5018010_23_16_RSVD : 8;         ///<BIT [23:16] Reserved.
        uint32_t CREG_ERR_STS         : 1;         ///<BIT [24] Control Register Error Status.
        uint32_t RF5018010_25_RSVD    : 1;         ///<BIT [25] Reserved.
        uint32_t DATM_ERR_STS         : 1;         ///<BIT [26] Data Manager Error Status.
        uint32_t CMDF_ERR_STS         : 1;         ///<BIT [27] Command Fetcher Error Status.
        uint32_t STSM_ERR_STS         : 1;         ///<BIT [28] Status Manager Error Status.
        uint32_t QARB_ERR_STS         : 1;         ///<BIT [29] Queue Arbiter Error Status.
        uint32_t PKTM_ERR_STS         : 1;         ///<BIT [30] PCIe Packet Manager Error Status.
        uint32_t HWLL_ERR_STS         : 1;         ///<BIT [31] Hardware Linked List Builder Error Status.
    } b;    
} CentralCtrlModuleIntr_t;

///  @brief 0xF5018020 : Central Control Module Interrupt Enable
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CCEN_CLR_EN          : 1;         ///<BIT [0] NVMe Enable Bit Clear Request Interrupt Enable.
        uint32_t CCEN_SET_EN          : 1;         ///<BIT [1] NVMe Enable Bit Set Request Interrupt Enable.
        uint32_t CCSHN_SET_EN         : 1;         ///<BIT [2] NVMe Shutdown Notification Interrupt Enable.
        uint32_t RF5018020_3_RSVD     : 1;         ///<BIT [3] Reserved.
        uint32_t SUB_SYS_EN           : 1;         ///<BIT [4] NVMe Subsystem Reset Request Interrupt Enable.
        uint32_t RF5018020_5_RSVD     : 1;         ///<BIT [5] Reserved.
        uint32_t SQ_DB_EN             : 1;         ///<BIT [6] NVMe Submission Queue Doorbell Event Interrupt Enable.
        uint32_t RF5018020_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint32_t CSRM_G_STS_EN        : 2;         ///<BIT [9:8] Command SRAM Good Status Interrupt Enable.
                                                   ///<    [0] Admin command SRAM FIFO contains at least one available command.
                                                   ///<    [1] I/O command SRAM FIFO contains at least one available command.
        uint32_t CBC_G_STS_EN         : 1;         ///<BIT [10] Command Base Completion Good Status Interrupt Enable.
        uint32_t BSRM_G_STS_EN        : 1;         ///<BIT [11] Bitmap SRAM Good Status Interrupt Enable.
        uint32_t RF5018020_12_RSVD    : 1;         ///<BIT [12] Reserved.
        uint32_t DATM_G_STS_EN        : 2;         ///<BIT [14:13] Data Manager Good Status Interrupt Enable.
                                                   ///<    [0] The Admin Transfer Done interrupt for ATDBM
                                                   ///<    [1] The Transfer Done 64 interrupt (TD64BM), 
                                                   ///<        which indicates the completion of 64 AU transfers of large read commands.
        uint32_t STSM_G_STS_EN        : 1;         ///<BIT [15] Status Manager Good Status Interrupt Enable.
        uint32_t RF5018020_23_16_RSVD : 8;         ///<BIT [23:16] Reserved.
        uint32_t CREG_ERR_STS_EN      : 1;         ///<BIT [24] Control Register Error Status Interrupt Enable.
        uint32_t RF5018020_25_RSVD    : 1;         ///<BIT [25] Reserved.
        uint32_t DATM_ERR_STS_EN      : 1;         ///<BIT [26] Data Manager Error Status Interrupt Enable.
        uint32_t CMDF_ERR_STS_EN      : 1;         ///<BIT [27] Command Fetcher Error Status Interrupt Enable.
        uint32_t STSM_ERR_STS_EN      : 1;         ///<BIT [28] Status Manager Error Status Interrupt Enable.
        uint32_t QARB_ERR_STS_EN      : 1;         ///<BIT [29] Queue Arbiter Error Status Interrupt Enable.
        uint32_t PKTM_ERR_STS_EN      : 1;         ///<BIT [30] PCIe Packet Manager Error Status Interrupt Enable.
        uint32_t HWLL_ERR_STS_EN      : 1;         ///<BIT [31] Hardware Linked List Builder Error Status Interrupt Enable.
    } b;    
} CentralCtrlModuleIntrEn_t;

///  @brief 0xF5018030 : Central Control Module Memory Clock Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_MEM_CLK_CTRL    : 1;         ///<BIT [0] Control Register Memory Clock Control.
        uint32_t RF5018030_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_MEM_CLK_CTRL    : 2;         ///<BIT [3:2] Command SRAM Memory Clock Control.
        uint32_t BSRM_MEM_CLK_CTRL    : 1;         ///<BIT [4] Bitmap SRAM Memory Clock Control.
        uint32_t RF5018030_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_MEM_CLK_CTRL    : 3;         ///<BIT [10:8] Data Manager Memory Clock Control.
        uint32_t RF5018030_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_MEM_CLK_CTRL    : 1;         ///<BIT [20] PCIe Packet Manager Memory Clock Control.
        uint32_t RF5018030_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_MEM_CLK_CTRL    : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory Clock Control.
                                                   ///<            Disable (gate) the memory clock for the HWLL Builder module.
                                                   ///<    [0]     HT table memory
                                                   ///<    [1]     Link array memory
        uint32_t RF5018030_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleMemClkCtrl_t;

///  @brief 0xF5018034 : Central Control Module Memory External Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_MEM_EXT_CTRL    : 1;         ///<BIT [0] Control Register Memory External Control.
        uint32_t RF5018034_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_MEM_EXT_CTRL    : 2;         ///<BIT [3:2] Command SRAM Memory External Control.
        uint32_t BSRM_MEM_EXT_CTRL    : 1;         ///<BIT [4] Bitmap SRAM Memory External Control.
        uint32_t RF5018034_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_MEM_EXT_CTRL    : 3;         ///<BIT [10:8] Data Manager Memory External Control.
        uint32_t RF5018034_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_MEM_EXT_CTRL    : 1;         ///<BIT [20] PCIe Packet Manager Memory External Control.
        uint32_t RF5018034_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_MEM_EXT_CTRL    : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory External Control.
                                                   ///<            Enable external control of the indicated memory in the HWLL module.
                                                   ///<    [0]     HT table memory
                                                   ///<    [1]     Link array memory
        uint32_t RF5018034_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleMemExtCtrl_t;

///  @brief 0xF5018040 : Central Control Module Memory Power Down
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_PDWN            : 1;         ///<BIT [0] Control Register Memory Power Down.
        uint32_t RF5018040_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_PDWN            : 2;         ///<BIT [3:2] Command SRAM Memory Power Down.
        uint32_t BSRM_PDWN            : 1;         ///<BIT [4] Bitmap SRAM Memory Power Down.
        uint32_t RF5018040_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_PDWN            : 3;         ///<BIT [10:8] Data Manager Memory Power Down.
        uint32_t RF5018040_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_PDWN            : 1;         ///<BIT [20] PCIe Packet Manager Memory Power Down.
        uint32_t RF5018040_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_PDWN            : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory Power Down.
                                                   ///<    [0]     HT table memory
                                                   ///<    [1]     Link array memory
        uint32_t RF5018040_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleMemPowerDn_t;

///  @brief 0xF5018044 : Central Control Module Memory PDLVMC
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_PDLVMC          : 1;         ///<BIT [0] Control Register Memory PDLVMC.
        uint32_t RF5018044_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_PDLVMC          : 2;         ///<BIT [3:2] Command SRAM PDLVMC.
        uint32_t BSRM_PDLVMC          : 1;         ///<BIT [4] Bitmap SRAM PDLVMC.
        uint32_t RF5018044_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_PDLVMC          : 3;         ///<BIT [10:8] Data Manager Memory PDLVMC.
        uint32_t RF5018044_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_PDLVMC          : 1;         ///<BIT [20] PCIe Packet Manager Memory PDLVMC.
        uint32_t RF5018044_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_PDLVMC          : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory PDLVMC.
        uint32_t RF5018044_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleMemPdlvmc_t;

///  @brief 0xF5018048 : Central Control Module Memory PDFVSSM
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_PDFVSSM         : 1;         ///<BIT [0] Control Register Memory PDFVSSM.
        uint32_t RF5018048_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_PDFVSSM         : 2;         ///<BIT [3:2] Command SRAM PDFVSSM.
        uint32_t BSRM_PDFVSSM         : 1;         ///<BIT [4] Bitmap SRAM PDFVSSM.
        uint32_t RF5018048_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_PDFVSSM         : 3;         ///<BIT [10:8] Data Manager Memory PDFVSSM.
        uint32_t RF5018048_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_PDFVSSM         : 1;         ///<BIT [20] PCIe Packet Manager Memory PDFVSSM.
        uint32_t RF5018048_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_PDFVSSM         : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory PDFVSSM.
        uint32_t RF5018048_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleMemPdfvssm_t;

///  @brief 0xF5018050 : Central Control Module Memory Write Timing Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_WTC             : 2;         ///<BIT [1:0] Control Register Memory Write Timing Control.
        uint32_t RF5018050_3_2_RSVD   : 2;         ///<BIT [3:2] Reserved.
        uint32_t CSRM_WTC             : 4;         ///<BIT [7:4] Command SRAM Write Timing Control.
        uint32_t BSRM_WTC             : 2;         ///<BIT [9:8] Bitmap SRAM Write Timing Control.
        uint32_t RF5018050_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t DATM_WTC             : 6;         ///<BIT [21:16] Data Manager Memory Write Timing Control.
        uint32_t RF5018050_31_22_RSVD :10;         ///<BIT [31:22] Reserved.
    } b;    
} CentralCtrlModuleMemWrTmCtrl0_t;

///  @brief 0xF5018054 : Central Control Module Memory Write Timing Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF5018054_7_0_RSVD   : 8;         ///<BIT [7:0] Reserved.
        uint32_t PKTM_WTC             : 2;         ///<BIT [9:8] PCIe Packet Manager Memory Write Timing Control.
        uint32_t RF5018054_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t HWLL_WTC             : 4;         ///<BIT [19:16] Hardware Linked List Builder Memory Write Timing Control.
        uint32_t RF5018054_31_20_RSVD :12;         ///<BIT [31:20] Reserved.
    } b;    
} CentralCtrlModuleMemWrTmCtrl1_t;

///  @brief 0xF5018058 : Central Control Module Memory Read Timing Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_RTC             : 2;         ///<BIT [1:0] Control Register Memory Read Timing Control.
        uint32_t RF5018058_3_2_RSVD   : 2;         ///<BIT [3:2] Reserved.
        uint32_t CSRM_RTC             : 4;         ///<BIT [7:4] Command SRAM Read Timing Control.
        uint32_t BSRM_RTC             : 2;         ///<BIT [9:8] Bitmap SRAM Read Timing Control.
        uint32_t RF5018058_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t DATM_RTC             : 6;         ///<BIT [21:16] Data Manager Memory Read Timing Control.
        uint32_t RF5018058_31_22_RSVD :10;         ///<BIT [31:22] Reserved.
    } b;    
} CentralCtrlModuleMemRdTmCtrl0_t;

///  @brief 0xF501805C : Central Control Module Memory Read Timing Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF501805C_7_0_RSVD   : 8;         ///<BIT [7:0] Reserved.
        uint32_t PKTM_RTC             : 2;         ///<BIT [9:8] PCIe Packet Manager Memory Read Timing Control.
        uint32_t RF501805C_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t HWLL_RTC             : 4;         ///<BIT [19:16] Hardware Linked List Builder Memory Read Timing Control.
        uint32_t RF501805C_31_20_RSVD :12;         ///<BIT [31:20] Reserved.
    } b;    
} CentralCtrlModuleMemRdTmCtrl1_t;

///  @brief 0xF5018060 : Central Control Module Memory Negative Chip Enable
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_NCE             : 1;         ///<BIT [0] Control Register Memory Negative Chip Enable.
        uint32_t RF5018060_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_NCE             : 2;         ///<BIT [3:2] Command SRAM Memory Negative Chip Enable.
        uint32_t BSRM_NCE             : 1;         ///<BIT [4] Bitmap SRAM Memory Negative Chip Enable.
        uint32_t RF5018060_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_NCE             : 3;         ///<BIT [10:8] Data Manager Memory Negative Chip Enable.
        uint32_t RF5018060_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_NCE             : 1;         ///<BIT [20] PCIe Packet Manager Memory Negative Chip Enable.
        uint32_t RF5018060_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_NCE             : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory Negative Chip Enable.
        uint32_t RF5018060_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleMemNegativeChipEn_t;

///  @brief 0xF5018064 : Central Control Module Memory Negative Write Enable
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_NWRT            : 1;         ///<BIT [0] Control Register Memory Negative Write Enable.
        uint32_t RF5018064_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_NWRT            : 2;         ///<BIT [3:2] Command SRAM Memory Negative Write Enable.
        uint32_t BSRM_NWRT            : 1;         ///<BIT [4] Bitmap SRAM Memory Negative Write Enable.
        uint32_t RF5018064_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_NWRT            : 3;         ///<BIT [10:8] Data Manager Memory Negative Write Enable.
        uint32_t RF5018064_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_NWRT            : 1;         ///<BIT [20] PCIe Packet Manager Memory Negative Write Enable.
        uint32_t RF5018064_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_NWRT            : 2;         ///<BIT [25:24] Hardware Link List Builder Memory Negative Chip Enable.
        uint32_t RF5018064_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleMemNegativeWrEn_t;

///  @brief 0xF5018080 : Central Control Module BIST Mode
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_BIST_MD         : 1;         ///<BIT [0] Control Register Memory BIST Mode.
        uint32_t RF5018080_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_BIST_MD         : 2;         ///<BIT [3:2] Command SRAM Memory BIST Mode.
        uint32_t BSRM_BIST_MD         : 1;         ///<BIT [4] Bitmap SRAM Memory BIST Mode.
        uint32_t RF5018080_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_BIST_MD         : 3;         ///<BIT [10:8] Data Manager Memory BIST Mode.
        uint32_t RF5018080_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_BIST_MD         : 1;         ///<BIT [20] PCIe Packet Manager Memory BIST Mode.
        uint32_t RF5018080_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_BIST_MD         : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory BIST Mode.
        uint32_t RF5018080_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleBistMode_t;

///  @brief 0xF5018084 : Central Control Module BIST Row-Column Switch
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_BIST_RC_SW      : 1;         ///<BIT [0] Control Register Memory BIST RowColSwitch.
        uint32_t RF5018084_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_BIST_RC_SW      : 2;         ///<BIT [3:2] Command SRAM Memory BIST RowColSwitch.
        uint32_t BSRM_BIST_RC_SW      : 1;         ///<BIT [4] Bitmap SRAM Memory BIST RowColSwitch.
        uint32_t RF5018084_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_BIST_RC_SW      : 3;         ///<BIT [10:8] Data Manager Memory BIST RowColSwitch.
        uint32_t RF5018084_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_BIST_RC_SW      : 1;         ///<BIT [20] PCIe Packet Manager Memory BIST RowColSwitch.
        uint32_t RF5018084_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_BIST_RC_SW      : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory BIST RowColSwitch.
        uint32_t RF5018084_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleBistRowColumnSw_t;

///  @brief 0xF5018088 : Central Control Module BIST Run Stop
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_RUN_STOP        : 1;         ///<BIT [0] Control Register Memory BIST RunStop.
        uint32_t RF5018088_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_RUN_STOP        : 2;         ///<BIT [3:2] Command SRAM Memory BIST RunStop.
        uint32_t BSRM_RUN_STOP        : 1;         ///<BIT [4] Bitmap SRAM Memory BIST RunStop.
        uint32_t RF5018088_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_RUN_STOP        : 3;         ///<BIT [10:8] Data Manager Memory BIST RunStop.
        uint32_t RF5018088_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_RUN_STOP        : 1;         ///<BIT [20] PCIe Packet Manager Memory BIST RunStop.
        uint32_t RF5018088_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_RUN_STOP        : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory BIST RunStop.
        uint32_t RF5018088_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleBistRunStop_t;

///  @brief 0xF501808C : Central Control Module BIST Reset
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_BIST_RST        : 1;         ///<BIT [0] Control Register Memory BIST Reset.
        uint32_t RF501808C_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_BIST_RST        : 2;         ///<BIT [3:2] Command SRAM Memory BIST Reset.
        uint32_t BSRM_BIST_RST        : 1;         ///<BIT [4] Bitmap SRAM Memory BIST Reset.
        uint32_t RF501808C_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_BIST_RST        : 3;         ///<BIT [10:8] Data Manager Memory BIST Reset.
        uint32_t RF501808C_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_BIST_RST        : 1;         ///<BIT [20] PCIe Packet Manager Memory BIST Reset.
        uint32_t RF501808C_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_BIST_RST        : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory BIST Reset.
        uint32_t RF501808C_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleBistRst_t;

///  @brief 0xF5018090 : Central Control Module BIST Error
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_BIST_ERR        : 1;         ///<BIT [0] Control Register Memory BIST Error.
        uint32_t RF5018090_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_BIST_ERR        : 2;         ///<BIT [3:2] Command SRAM Memory BIST Error.
        uint32_t BSRM_BIST_ERR        : 1;         ///<BIT [4] Bitmap SRAM Memory BIST Error.
        uint32_t RF5018090_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_BIST_ERR        : 3;         ///<BIT [10:8] Data Manager Memory BIST Error.
        uint32_t RF5018090_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_BIST_ERR        : 1;         ///<BIT [20] PCIe Packet Manager Memory BIST Error.
        uint32_t RF5018090_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_BIST_ERR        : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory BIST Error.
        uint32_t RF5018090_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleBistErr_t;

///  @brief 0xF5018094 : Central Control Module BIST Fail
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_BIST_FL         : 1;         ///<BIT [0] Control Register Memory BIST Fail.
        uint32_t RF5018094_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_BIST_FL         : 2;         ///<BIT [3:2] Command SRAM Memory BIST Fail.
        uint32_t BSRM_BIST_FL         : 1;         ///<BIT [4] Bitmap SRAM Memory BIST Fail.
        uint32_t RF5018094_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_BIST_FL         : 3;         ///<BIT [10:8] Data Manager Memory BIST Fail.
        uint32_t RF5018094_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_BIST_FL         : 1;         ///<BIT [20] PCIe Packet Manager Memory BIST Fail.
        uint32_t RF5018094_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_BIST_FL         : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory BIST Fail.
        uint32_t RF5018094_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleBistFail_t;

///  @brief 0xF5018098 : Central Control Module BIST Finish
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_BIST_FIN        : 1;         ///<BIT [0] Control Register Memory BIST Finish.
        uint32_t RF5018098_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_BIST_FIN        : 2;         ///<BIT [3:2] Command SRAM Memory BIST Finish.
        uint32_t BSRM_BIST_FIN        : 1;         ///<BIT [4] Bitmap SRAM Memory BIST Finish.
        uint32_t RF5018098_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_BIST_FIN        : 3;         ///<BIT [10:8] Data Manager Memory BIST Finish.
        uint32_t RF5018098_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_BIST_FIN        : 1;         ///<BIT [20] PCIe Packet Manager Memory BIST Finish.
        uint32_t RF5018098_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_BIST_FIN        : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory BIST Finish.
        uint32_t RF5018098_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleBistFinish_t;

///  @brief 0xF50180A0 : Central Control Module BIST Status Enable
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_BIST_STS_EN     : 1;         ///<BIT [0] Control Register Memory BIST Status Enable.
        uint32_t RF50180A0_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_BIST_STS_EN     : 2;         ///<BIT [3:2] Command SRAM BIST Status Enable.
        uint32_t BSRM_BIST_STS_EN     : 1;         ///<BIT [4] Bitmap SRAM BIST Status Enable.
        uint32_t RF50180A0_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_BIST_STS_EN     : 3;         ///<BIT [10:8] Data Manager Memory BIST Status Enable.
        uint32_t RF50180A0_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_BIST_STS_EN     : 1;         ///<BIT [20] PCIe Packet Manager Memory BIST Status Enable.
        uint32_t RF50180A0_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_BIST_STS_EN     : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory BIST Status Enable.
        uint32_t RF50180A0_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleBistStatEn_t;

///  @brief 0xF50180A4 : Central Control Module BIST Status Clock
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_BIST_STS_CLK    : 1;         ///<BIT [0] Control Register Memory BIST Status Clock.
        uint32_t RF50180A4_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_BIST_STS_CLK    : 2;         ///<BIT [3:2] Command SRAM BIST Status Clock.
        uint32_t BSRM_BIST_STS_CLK    : 1;         ///<BIT [4] Bitmap SRAM BIST Status Clock.
        uint32_t RF50180A4_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_BIST_STS_CLK    : 3;         ///<BIT [10:8] Data Manager Memory BIST Status Clock.
        uint32_t RF50180A4_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_BIST_STS_CLK    : 1;         ///<BIT [20] PCIe Packet Manager Memory BIST Status Clock.
        uint32_t RF50180A4_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_BIST_STS_CLK    : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory BIST Status Clock.
        uint32_t RF50180A4_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleBistStatClk_t;

///  @brief 0xF50180A8 : Central Control Module BIST Status Out
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CREG_BIST_STS_O      : 1;         ///<BIT [0] Control Register Memory BIST Status Out.
        uint32_t RF50180A8_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t CSRM_BIST_STS_O      : 2;         ///<BIT [3:2] Command SRAM BIST Status Out.
        uint32_t BSRM_BIST_STS_O      : 1;         ///<BIT [4] Bitmap SRAM BIST Status Out.
        uint32_t RF50180A8_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DATM_BIST_STS_O      : 3;         ///<BIT [10:8] Data Manager Memory BIST Status Out.
        uint32_t RF50180A8_19_11_RSVD : 9;         ///<BIT [19:11] Reserved.
        uint32_t PKTM_BIST_STS_O      : 1;         ///<BIT [20] PCIe Packet Manager Memory BIST Status Out.
        uint32_t RF50180A8_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t HWLL_BIST_STS_O      : 2;         ///<BIT [25:24] Hardware Linked List Builder Memory BIST Status Out.
        uint32_t RF50180A8_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} CentralCtrlModuleBistStatOut_t;

///  @brief 0xF50180B0 : Central Control Module Interrupt Information Select
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t NVME_INT_INFO_SEL   : 3;         ///<BIT [2:0] NVMe Host Interrupt Information Select.
        uint32_t RF50180B0_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} CentralCtrlModuleIntrInformatSel_t;

///  @brief 0xF50180B4 : Central Control Module Interrupt Information Data
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t NVME_INT_INFO_DATA  : 3;         ///<BIT [2:0] NVMe Host Interrupt Information Data.
        uint32_t RF50180B4_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} CentralCtrlModuleIntrInformatData_t;

///  @brief 0xF50180F0 : Central Control Module Monitor Select
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LSB_B_SEL            : 1;         ///<BIT [0] Source Byte Select For Lower Monitor Byte.
        uint32_t RF50180F0_7_1_RSVD   : 7;         ///<BIT [7:1] Reserved.
        uint32_t LSB_MOD_SEL          : 4;         ///<BIT [11:8] Source Module Select for Lower Monitor Byte.
        uint32_t RF50180F0_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t MSB_B_SEL            : 1;         ///<BIT [16] Source Byte Select for Upper Monitor Byte.
        uint32_t RF50180F0_23_17_RSVD : 7;         ///<BIT [23:17] Reserved.
        uint32_t MSB_MOD_SEL          : 4;         ///<BIT [27:24] Source Module Select for Upper Monitor Byte.
        uint32_t RF50180F0_31_28_RSVD : 4;         ///<BIT [31:28] Reserved.
    } b;    
} CentralCtrlModuleMonitorSel_t;

///  @brief 0xF50180F4 : Central Control Module Monitor Data
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CENT_MON_DATA        :16;         ///<BIT [15:0] Central Control Module Monitor Data.
        uint32_t RF50180F4_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} CentralCtrlModuleMonitorData_t;

///  @brief 0xF5018104 : Central Control Module FPGA Timer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CENT_FPGA_TO_THR     :16;         ///<BIT [15:0] Central Control Module FPGA Time-Out Threshold.
        uint32_t RF5018104_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} CentralCtrlModuleFpgaTmr_t;


/// @brief 0xF5018000-0xF5018108 : NvmeCentralCtrl_t MODULE
typedef struct
{
    CentralCtrlModuleCfg_t centralCtrlModuleCfg;                                 ///< 0xF5018000 : Central Control Module Configuration
    CentralCtrlModuleRstCtrl_t centralCtrlModuleRstCtrl;                         ///< 0xF5018004 : Central Control Module Reset Control
    CentralCtrlModuleTmrCtrl_t centralCtrlModuleTmrCtrl;                         ///< 0xF5018008 : Central Control Module Timer Control
    uint8_t rsvd_RF501800C[4];                                                   ///< 0xF501800C - 0xF501800F : Reserved.
    CentralCtrlModuleIntr_t centralCtrlModuleIntr;                               ///< 0xF5018010 : Central Control Module Interrupt
    uint8_t rsvd_RF5018014[12];                                                  ///< 0xF5018014 - 0xF501801F : Reserved.
    CentralCtrlModuleIntrEn_t centralCtrlModuleIntrEn;                           ///< 0xF5018020 : Central Control Module Interrupt Enable
    uint8_t rsvd_RF5018024[12];                                                  ///< 0xF5018024 - 0xF501802F : Reserved.
    CentralCtrlModuleMemClkCtrl_t centralCtrlModuleMemClkCtrl;                   ///< 0xF5018030 : Central Control Module Memory Clock Control
    CentralCtrlModuleMemExtCtrl_t centralCtrlModuleMemExtCtrl;                   ///< 0xF5018034 : Central Control Module Memory External Control
    uint8_t rsvd_RF5018038[8];                                                   ///< 0xF5018038 - 0xF501803F : Reserved.
    CentralCtrlModuleMemPowerDn_t centralCtrlModuleMemPowerDn;                   ///< 0xF5018040 : Central Control Module Memory Power Down
    CentralCtrlModuleMemPdlvmc_t centralCtrlModuleMemPdlvmc;                     ///< 0xF5018044 : Central Control Module Memory PDLVMC
    CentralCtrlModuleMemPdfvssm_t centralCtrlModuleMemPdfvssm;                   ///< 0xF5018048 : Central Control Module Memory PDFVSSM
    uint8_t rsvd_RF501804C[4];                                                   ///< 0xF501804C - 0xF501804F : Reserved.
    CentralCtrlModuleMemWrTmCtrl0_t centralCtrlModuleMemWrTmCtrl0;               ///< 0xF5018050 : Central Control Module Memory Write Timing Control 0
    CentralCtrlModuleMemWrTmCtrl1_t centralCtrlModuleMemWrTmCtrl1;               ///< 0xF5018054 : Central Control Module Memory Write Timing Control 1
    CentralCtrlModuleMemRdTmCtrl0_t centralCtrlModuleMemRdTmCtrl0;               ///< 0xF5018058 : Central Control Module Memory Read Timing Control 0
    CentralCtrlModuleMemRdTmCtrl1_t centralCtrlModuleMemRdTmCtrl1;               ///< 0xF501805C : Central Control Module Memory Read Timing Control 1
    CentralCtrlModuleMemNegativeChipEn_t centralCtrlModuleMemNegativeChipEn;     ///< 0xF5018060 : Central Control Module Memory Negative Chip Enable
    CentralCtrlModuleMemNegativeWrEn_t centralCtrlModuleMemNegativeWrEn;         ///< 0xF5018064 : Central Control Module Memory Negative Write Enable
    uint8_t rsvd_RF5018068[24];                                                  ///< 0xF5018068 - 0xF501807F : Reserved.
    CentralCtrlModuleBistMode_t centralCtrlModuleBistMode;                       ///< 0xF5018080 : Central Control Module BIST Mode
    CentralCtrlModuleBistRowColumnSw_t centralCtrlModuleBistRowColumnSw;         ///< 0xF5018084 : Central Control Module BIST Row-Column Switch
    CentralCtrlModuleBistRunStop_t centralCtrlModuleBistRunStop;                 ///< 0xF5018088 : Central Control Module BIST Run Stop
    CentralCtrlModuleBistRst_t centralCtrlModuleBistRst;                         ///< 0xF501808C : Central Control Module BIST Reset
    CentralCtrlModuleBistErr_t centralCtrlModuleBistErr;                         ///< 0xF5018090 : Central Control Module BIST Error
    CentralCtrlModuleBistFail_t centralCtrlModuleBistFail;                       ///< 0xF5018094 : Central Control Module BIST Fail
    CentralCtrlModuleBistFinish_t centralCtrlModuleBistFinish;                   ///< 0xF5018098 : Central Control Module BIST Finish
    uint8_t rsvd_RF501809C[4];                                                   ///< 0xF501809C - 0xF501809F : Reserved.
    CentralCtrlModuleBistStatEn_t centralCtrlModuleBistStatEn;                   ///< 0xF50180A0 : Central Control Module BIST Status Enable
    CentralCtrlModuleBistStatClk_t centralCtrlModuleBistStatClk;                 ///< 0xF50180A4 : Central Control Module BIST Status Clock
    CentralCtrlModuleBistStatOut_t centralCtrlModuleBistStatOut;                 ///< 0xF50180A8 : Central Control Module BIST Status Out
    uint8_t rsvd_RF50180AC[4];                                                   ///< 0xF50180AC - 0xF50180AF : Reserved.
    CentralCtrlModuleIntrInformatSel_t centralCtrlModuleIntrInformatSel;         ///< 0xF50180B0 : Central Control Module Interrupt Information Select
    CentralCtrlModuleIntrInformatData_t centralCtrlModuleIntrInformatData;       ///< 0xF50180B4 : Central Control Module Interrupt Information Data
    uint8_t rsvd_RF50180B8[56];                                                  ///< 0xF50180B8 - 0xF50180EF : Reserved.
    CentralCtrlModuleMonitorSel_t centralCtrlModuleMonitorSel;                   ///< 0xF50180F0 : Central Control Module Monitor Select
    CentralCtrlModuleMonitorData_t centralCtrlModuleMonitorData;                 ///< 0xF50180F4 : Central Control Module Monitor Data
    uint8_t rsvd_RF50180F8[8];                                                   ///< 0xF50180F8 - 0xF50180FF : Reserved.
    uint32_t CENT_DEVICE_ID;                                                     ///< 0xF5018100 : Central Control Module Device ID
    CentralCtrlModuleFpgaTmr_t centralCtrlModuleFpgaTmr;                         ///< 0xF5018104 : Central Control Module FPGA Timer
} NvmeCentralCtrl_t;
COMPILE_ASSERT(offsetof(NvmeCentralCtrl_t,centralCtrlModuleFpgaTmr)==0xF5018104-0xF5018000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile NvmeCentralCtrl_t rNvmeCentralCtrl;  ///< 0xF5018000-0xF5018108 : NvmeCentralCtrl_t MODULE

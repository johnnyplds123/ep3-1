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
//! @brief APB Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  APB Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF0115000 : Fast Interrupt Request Selection for CPU0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPU0_FIQ0_SEL        : 5;         ///<BIT [4:0] Fiq[0] Select for CPU0.
        uint32_t RF0115000_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t CPU0_FIQ1_SEL        : 5;         ///<BIT [12:8] Fiq[1] Select for CPU0.
        uint32_t RF0115000_14_13_RSVD : 2;         ///<BIT [14:13] Reserved.
        uint32_t CPU0_FIQ2_SEL        : 5;         ///<BIT [19:15] Fiq[2] Select for CPU0.
        uint32_t RF0115000_22_20_RSVD : 3;         ///<BIT [22:20] Reserved.
        uint32_t CPU0_FIQ3_SEL        : 5;         ///<BIT [27:23] Fiq[3] Select for CPU0.
        uint32_t RF0115000_31_28_RSVD : 4;         ///<BIT [31:28] Reserved.
    } b;    
} FastIntrReqSelectioForCpu0_t;

///  @brief 0xF0115004 : Fast Interrupt Request Selection for CPU1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPU1_FIQ0_SEL        : 5;         ///<BIT [4:0] Fiq[0] Select for CPU1.
        uint32_t RF0115004_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t CPU1_FIQ1_SEL        : 5;         ///<BIT [12:8] Fiq[1] Select for CPU1.
        uint32_t RF0115004_14_13_RSVD : 2;         ///<BIT [14:13] Reserved.
        uint32_t CPU1_FIQ2_SEL        : 5;         ///<BIT [19:15] Fiq[2] Select for CPU1.
        uint32_t RF0115004_22_20_RSVD : 3;         ///<BIT [22:20] Reserved.
        uint32_t CPU1_FIQ3_SEL        : 5;         ///<BIT [27:23] Fiq[3] Select for CPU1.
        uint32_t RF0115004_31_28_RSVD : 4;         ///<BIT [31:28] Reserved.
    } b;    
} FastIntrReqSelectioForCpu1_t;

///  @brief 0xF0115008 : Fast Interrupt Request Selection for CPU2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPU2_FIQ0_SEL        : 5;         ///<BIT [4:0] Fiq[0] Select for CPU2.
        uint32_t RF0115008_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t CPU2_FIQ1_SEL        : 5;         ///<BIT [12:8] Fiq[1] Select for CPU2.
        uint32_t RF0115008_14_13_RSVD : 2;         ///<BIT [14:13] Reserved.
        uint32_t CPU2_FIQ2_SEL        : 5;         ///<BIT [19:15] Fiq[2] Select for CPU2.
        uint32_t RF0115008_22_20_RSVD : 3;         ///<BIT [22:20] Reserved.
        uint32_t CPU2_FIQ3_SEL        : 5;         ///<BIT [27:23] Fiq[3] Select for CPU2.
        uint32_t RF0115008_31_28_RSVD : 4;         ///<BIT [31:28] Reserved.
    } b;    
} FastIntrReqSelectioForCpu2_t;

///  @brief 0xF011500C : Vectored Interrupt Controller Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VIC_EN              : 1;         ///<BIT [0] Vectored Interrupt Controller Enable.
        uint32_t RF011500C_3_1_RSVD  : 3;         ///<BIT [3:1] Reserved.
        uint32_t VIC_SYNC_EN         : 3;         ///<BIT [6:4] Vectored Interrupt Controller Synchronous Enable.
        uint32_t RF011500C_31_7_RSVD :25;         ///<BIT [31:7] Reserved.
    } b;    
} VectoredIntrControllCtrl_t;

///  @brief 0xF0115010 : ROM0 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPU0_ROM_TST_RD_H    : 6;         ///<BIT [5:0] CPU0 ROM ECC Test Data Input High.
        uint32_t RF0115010_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint32_t CPU0_ROM_ECC_CORR    : 1;         ///<BIT [8] CPU0 ROM ECC Error Corrected.
        uint32_t CPU0_ROM_ECC_ERR     : 1;         ///<BIT [9] CPU0 ROM ECC Error Detected.
        uint32_t RF0115010_12_10_RSVD : 3;         ///<BIT [12:10] Reserved.
        uint32_t CPU0_ROM_ERR_STT_RST : 1;         ///<BIT [13] CPU0 ROM Error Status Reset.
        uint32_t CPU0_ROM_RD_SEL      : 1;         ///<BIT [14] CPU0 ROM Test Data Read Select.
        uint32_t CPU0_ROM_ECC_EN      : 1;         ///<BIT [15] CPU0 ROM ECC Enable.
        uint32_t RF0115010_19_16_RSVD : 4;         ///<BIT [19:16] Reserved.
        uint32_t CPU0_ROM_RTC_REF     : 2;         ///<BIT [21:20] CPU0 ROM Read Timing Control Reference.
        uint32_t RF0115010_23_22_RSVD : 2;         ///<BIT [23:22] Reserved.
        uint32_t CPU0_ROM_RTC         : 3;         ///<BIT [26:24] CPU0 ROM Read Timing Control.
        uint32_t RF0115010_30_27_RSVD : 4;         ///<BIT [30:27] Reserved.
        uint32_t CPU0_ROM_PDWN_EN     : 1;         ///<BIT [31] CPU0 ROM Power Down Enable.
    } b;    
} Rom0Ctrl_t;

///  @brief 0xF0115020 : ROM1 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPU1_ROM_TST_RD_H    : 6;         ///<BIT [5:0] CPU1 ROM ECC Test Data Input High.
        uint32_t RF0115020_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint32_t CPU1_ROM_ECC_CORR    : 1;         ///<BIT [8] CPU1 ROM ECC Error Corrected.
        uint32_t CPU1_ROM_ECC_ERR     : 1;         ///<BIT [9] CPU1 ROM ECC Error Detected.
        uint32_t RF0115020_12_10_RSVD : 3;         ///<BIT [12:10] Reserved.
        uint32_t CPU1_ROM_ERR_STT_RST : 1;         ///<BIT [13] CPU1 ROM Error Status Reset.
        uint32_t CPU1_ROM_RD_SEL      : 1;         ///<BIT [14] CPU1 ROM Test Data Read Select.
        uint32_t CPU1_ROM_ECC_EN      : 1;         ///<BIT [15] CPU1 ROM ECC Enable.
        uint32_t RF0115020_19_16_RSVD : 4;         ///<BIT [19:16] Reserved.
        uint32_t CPU1_ROM_RTC_REF     : 2;         ///<BIT [21:20] CPU1 ROM Read Timing Control Reference.
        uint32_t RF0115020_23_22_RSVD : 2;         ///<BIT [23:22] Reserved.
        uint32_t CPU1_ROM_RTC         : 3;         ///<BIT [26:24] CPU1 ROM Read Timing Control.
        uint32_t RF0115020_30_27_RSVD : 4;         ///<BIT [30:27] Reserved.
        uint32_t CPU1_ROM_PDWN_EN     : 1;         ///<BIT [31] CPU1 ROM Power Down Enable.
    } b;    
} Rom1Ctrl_t;

///  @brief 0xF0115030 : ROM2 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPU2_ROM_TST_RD_H    : 6;         ///<BIT [5:0] CPU2 ROM ECC Test Data Input High.
        uint32_t RF0115030_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint32_t CPU2_ROM_ECC_CORR    : 1;         ///<BIT [8] CPU2 ROM ECC Error Corrected.
        uint32_t CPU2_ROM_ECC_ERR     : 1;         ///<BIT [9] CPU2 ROM ECC Error Detected.
        uint32_t RF0115030_12_10_RSVD : 3;         ///<BIT [12:10] Reserved.
        uint32_t CPU2_ROM_ERR_STT_RST : 1;         ///<BIT [13] CPU2 ROM Error Status Reset.
        uint32_t ROM2_RD_SEL          : 1;         ///<BIT [14] CPU2 ROM Test Data Read Select.
        uint32_t CPU2_ROM_ECC_EN      : 1;         ///<BIT [15] CPU2 ROM ECC Enable.
        uint32_t RF0115030_19_16_RSVD : 4;         ///<BIT [19:16] Reserved.
        uint32_t CPU2_ROM_RTC_REF     : 2;         ///<BIT [21:20] CPU2 ROM Read Timing Control Reference.
        uint32_t RF0115030_23_22_RSVD : 2;         ///<BIT [23:22] Reserved.
        uint32_t CPU2_ROM_RTC         : 3;         ///<BIT [26:24] CPU2 ROM Read Timing Control.
        uint32_t RF0115030_30_27_RSVD : 4;         ///<BIT [30:27] Reserved.
        uint32_t CPU2_ROM_PDWN_EN     : 1;         ///<BIT [31] CPU2 ROM Power Down Enable.
    } b;    
} Rom2Ctrl_t;

///  @brief 0xF0115050 : MC DDR Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MC5_SRAM_PDWN                 : 1;         ///<BIT [0] SRAM Power Down Control.
        uint32_t MC5_SRAM_PDLVMC               : 1;         ///<BIT [1] SRAM PDLVMC Control.
        uint32_t MC5_SRAM_PDFVSSM              : 1;         ///<BIT [2] SRAM PDFVSSM Control.
        uint32_t MC5_PHY_4TO1_EN               : 1;         ///<BIT [3] Memory Controller PHY 4:1 Mode Enable.
        uint32_t MC5_PHY_RST_MSK               : 1;         ///<BIT [4] Memory Controller PHY Reset Mask.
        uint32_t MC5_TZ_DIS                    : 1;         ///<BIT [5] Memory Controller Trust Zone Disable.
        uint32_t MC5_TZ_LCK_PORT               : 1;         ///<BIT [6] Memory Controller Trust Zone Lock Port.
        uint32_t MC5_PWON_CAL_MD_CH0           : 1;         ///<BIT [7] MC Power On Command/Address Mode for Channel 0.
        uint32_t MC5_PWON_DPD_ST_CH0           : 2;         ///<BIT [9:8] MC Power On Deep Power Down State for Channel 0.
        uint32_t MC5_PWON_MPD_ST_CH0           : 2;         ///<BIT [11:10] MC Power On Maximum Power Down State for Channel 0.
        uint32_t MC5_PWON_SR_ST_CH0            : 2;         ///<BIT [13:12] MC Power On Self-Refresh State for Channel 0.
        uint32_t MC5_PWON_CRC_MD_CH0           : 1;         ///<BIT [14] MC Power On CRC On Mode for Channel 0.
        uint32_t MC5_PWON_PAR_MD_CH0           : 1;         ///<BIT [15] MC Power On Parity On Mode for Channel 0.
        uint32_t MC5_MC_HW_WCB_DRN_REQ         : 1;         ///<BIT [16] MC Hardware WCB Drain Request.
        uint32_t MC5_MC_HW_WCB_DRN_ACK         : 1;         ///<BIT [17] MC Hardware WCB Drain Acknowledge.
        uint32_t MC5_MC_HW_PHY_DLL_UPD_REQ_CH0 : 1;         ///<BIT [18] MC Hardware PHY DLL Update Request.
        uint32_t RF0115050_19_RSVD             : 1;         ///<BIT [19] Reserved.
        uint32_t MC5_LB_RDATA_VLD_DLY          : 4;         ///<BIT [23:20] MC Loopback Read Data Valid Delay.
        uint32_t MC5_MC_LB_TEST_MD             : 1;         ///<BIT [24] Memory Controller Loopback Test Mode.
        uint32_t MC5_MC_PRBS_FL_CH0            : 1;         ///<BIT [25] Memory Controller PRBS Fail for Channel 0.
        uint32_t MC5_MC_PRBS_DONE_CH0          : 1;         ///<BIT [26] Memory Controller PRBS Done for Channel 0.
        uint32_t RF0115050_28_27_RSVD          : 2;         ///<BIT [28:27] Reserved.
        uint32_t DDRPHY_PDB_CORE               : 1;         ///<BIT [29] DDRPHY PDB Core.
        uint32_t DDRPHY_PD_CORE                : 1;         ///<BIT [30] DDRPHY Power Down Core.
        uint32_t MC5_MC_IDLE_CH0               : 1;         ///<BIT [31] Memory Controller Idle.
    } b;    
} McDdrCtrl0_t;

///  @brief 0xF0115080 : Pad Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PAD_ZN               : 5;         ///<BIT [4:0] Pad ZN.
        uint32_t RF0115080_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t PAD_ZP               : 5;         ///<BIT [12:8] Pad ZP.
        uint32_t RF0115080_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t PAD_DCS              : 2;         ///<BIT [17:16] Pad DCS.
        uint32_t PAD_DSR              : 2;         ///<BIT [19:18] Pad DSR.
        uint32_t PAD_I2C_SEL          : 2;         ///<BIT [21:20] Pad I2C Select.
        uint32_t RF0115080_31_22_RSVD :10;         ///<BIT [31:22] Reserved.
    } b;    
} PadCfg_t;

///  @brief 0xF0115090 : Reset Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RST_OSC               : 1;         ///<BIT [0] Reset OSC.
        uint32_t RST_RFSH              : 1;         ///<BIT [1] Reset Refresh.
        uint32_t RST_CPU0              : 1;         ///<BIT [2] Reset CPU0.
        uint32_t RST_CPU1              : 1;         ///<BIT [3] Reset CPU1.
        uint32_t RST_CPU2              : 1;         ///<BIT [4] Reset CPU2.
        uint32_t RST_CPUDIV2           : 1;         ///<BIT [5] Reset CPUDIV2.
        uint32_t RST_CPU_JT            : 1;         ///<BIT [6] Reset CPU JTAG.
        uint32_t RST_PDBG              : 1;         ///<BIT [7] Reset Processor Debug.
        uint32_t RST_TRACE             : 1;         ///<BIT [8] Reset Trace.
        uint32_t RST_MC5               : 1;         ///<BIT [9] Reset MC5.
        uint32_t RST_FLASH             : 1;         ///<BIT [10] Reset Flash.
        uint32_t RST_SYSAXI            : 1;         ///<BIT [11] Reset SYS AXI.
        uint32_t RST_PCIAXI            : 1;         ///<BIT [12] Reset PCI AXI.
        uint32_t RST_ECC               : 1;         ///<BIT [13] Reset ECC.
        uint32_t RF0115090_15_14_RSVD  : 2;         ///<BIT [15:14] Reserved.
        uint32_t WDT_RBOOT_TMR_MAX_VAL :12;         ///<BIT [27:16] Watchdog Reboot Timer Maximum Value.
        uint32_t RF0115090_30_28_RSVD  : 3;         ///<BIT [30:28] Reserved.
        uint32_t WARM_RST_FW           : 1;         ///<BIT [31] Warm Reset.
    } b;    
} RstCtrl_t;

///  @brief 0xF01150A0 : SRAM Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AHCI_SRAM_WTC            : 2;         ///<BIT [1:0] AHCI SRAM Write Timing Control Port.
        uint32_t AHCI_SRAM_RTC            : 2;         ///<BIT [3:2] AHCI SRAM Read Timing Control Port.
        uint32_t RF01150A0_5_4_RSVD       : 2;         ///<BIT [5:4] Reserved.
        uint32_t AHCI_SRAM_PDWN           : 1;         ///<BIT [6] AHCI SRAM Power Down Mode Control.
        uint32_t AHCI_SRAM_PDLVMC         : 1;         ///<BIT [7] AHCI SRAM PDLVMC.
        uint32_t AHCI_CMDFIFO_SRAM_WTC    : 2;         ///<BIT [9:8] AHCI Command FIFO SRAM Write Timing Control Port.
        uint32_t AHCI_CMDFIFO_SRAM_RTC    : 2;         ///<BIT [11:10] AHCI Command FIFO SRAM Read Timing Control Port.
        uint32_t RF01150A0_13_12_RSVD     : 2;         ///<BIT [13:12] Reserved.
        uint32_t AHCI_CMDFIFO_SRAM_PDWN   : 1;         ///<BIT [14] AHCI Command FIFO SRAM Power Down Mode Control.
        uint32_t AHCI_CMDFIFO_SRAM_PDLVMC : 1;         ///<BIT [15] AHCI Command FIFO SRAM PDLVMC.
        uint32_t RF01150A0_16_RSVD        : 1;         ///<BIT [16] Reserved.
        uint32_t IDS_SRAM_PDFVSSM         : 1;         ///<BIT [17] IDS SRAM PDFVSSM.
        uint32_t IDS_SRAM_PDWN            : 1;         ///<BIT [18] IDS SRAM Power Down Mode Control.
        uint32_t IDS_SRAM_PDLVMC          : 1;         ///<BIT [19] IDS SRAM PDLVMC.
        uint32_t IDS_RTC                  : 2;         ///<BIT [21:20] IDS Read Timing Control Port.
        uint32_t IDS_WTC                  : 2;         ///<BIT [23:22] Internal Data SRAM Write Timing Control Port.
        uint32_t SPI_SRAM_PWDN            : 1;         ///<BIT [24] SPI SRAM Power Down Mode Control.
        uint32_t RF01150A0_27_25_RSVD     : 3;         ///<BIT [27:25] Reserved.
        uint32_t SPI_SRAM_RTC             : 2;         ///<BIT [29:28] SPI SRAM Read Timing Control Port.
        uint32_t SPI_SRAM_WTC             : 2;         ///<BIT [31:30] SPI SRAM Write Timing Control Port.
    } b;    
} SramCtrl1_t;

///  @brief 0xF01150A4 : SRAM Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SPM_SRAM_PD_TH       :12;         ///<BIT [11:0] SPM Power Down Threshold.
        uint32_t SPM_SRAM_PD_EXIT_CLK : 3;         ///<BIT [14:12] SPM Power Down Exit Clock.
        uint32_t RF01150A4_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t SPM_PARITY_EN        : 1;         ///<BIT [16] SPM Parity Enable.
        uint32_t RF01150A4_20_17_RSVD : 4;         ///<BIT [20:17] Reserved.
        uint32_t SPM_SRAM_PD_EN       : 1;         ///<BIT [21] SPM Power Down Enable.
        uint32_t SPM_SRAM_FW_PDWN     : 1;         ///<BIT [22] SPM Firmware Power Down.
        uint32_t RF01150A4_23_RSVD    : 1;         ///<BIT [23] Reserved.
        uint32_t SPM_SRAM_PDWN        : 1;         ///<BIT [24] SPM SRAM Power Down Mode Control.
        uint32_t SPM_SRAM_PDLVMC      : 1;         ///<BIT [25] SPM SRAM PDLVMC.
        uint32_t RF01150A4_27_26_RSVD : 2;         ///<BIT [27:26] Reserved.
        uint32_t SPM_SRAM_RTC         : 2;         ///<BIT [29:28] SPM SRAM Read Timing Control Port.
        uint32_t SPM_SRAM_WTC         : 2;         ///<BIT [31:30] SPM SRAM Write Timing Control Port.
    } b;    
} SramCtrl2_t;

///  @brief 0xF01150B0 : CPU SRAM Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF01150B0_0_RSVD  : 1;         ///<BIT [0] Reserved.
        uint32_t ETB_PDFVSSM       : 1;         ///<BIT [1] CoreSight Embedded Trace Buffer (ETB) PDFVSSM.
        uint32_t ETB_PDLVMC        : 1;         ///<BIT [2] CoreSight Embedded Trace Buffer (ETB) PDLVMC.
        uint32_t ETB_PDWN          : 1;         ///<BIT [3] CoreSight Embedded Trace Buffer (ETB) SRAM Power Down Mode Cont.
        uint32_t RF01150B0_4_RSVD  : 1;         ///<BIT [4] Reserved.
        uint32_t CACHE_PDFVSSM     : 1;         ///<BIT [5] Cache PDFVSSM.
        uint32_t CACHE_PDLVMC      : 1;         ///<BIT [6] Cache PDLVMC.
        uint32_t CACHE_PDWN        : 1;         ///<BIT [7] Cache SRAM Power Down Mode Control.
        uint32_t RF01150B0_8_RSVD  : 1;         ///<BIT [8] Reserved.
        uint32_t BTCM_PDFVSSM      : 1;         ///<BIT [9] BTCM PDFVSSM.
        uint32_t BTCM_PDLVMC       : 1;         ///<BIT [10] BTCM PDLVMC.
        uint32_t BTCM_PDWN         : 1;         ///<BIT [11] BTCM SRAM Power Down Mode Control.
        uint32_t RF01150B0_12_RSVD : 1;         ///<BIT [12] Reserved.
        uint32_t ATCM_PDFVSSM      : 1;         ///<BIT [13] ATCM PDFVSSM.
        uint32_t ATCM_PDLVMC       : 1;         ///<BIT [14] ATCM PDLVMC.
        uint32_t ATCM_PDWN         : 1;         ///<BIT [15] ATCM SRAM Power Down Mode Control.
        uint32_t ETB_WTC           : 2;         ///<BIT [17:16] CoreSight Embedded Trace Buffer (ETB) Write Timing Control Port.
        uint32_t ETB_RTC           : 2;         ///<BIT [19:18] CoreSight Embedded Trace Buffer (ETB) Read Timing Control Port.
        uint32_t CACHE_WTC         : 2;         ///<BIT [21:20] Cache SRAM Write Timing Control Port.
        uint32_t CACHE_RTC         : 2;         ///<BIT [23:22] Cache SRAM Read Timing Control Port.
        uint32_t BTCM_WTC          : 2;         ///<BIT [25:24] BTCM SRAM Write Timing Control Port.
        uint32_t BTCM_RTC          : 2;         ///<BIT [27:26] BTCM SRAM Read Timing Control Port.
        uint32_t ATCM_WTC          : 2;         ///<BIT [29:28] ATCM SRAM Write Timing Control Port.
        uint32_t ATCM_RTC          : 2;         ///<BIT [31:30] ATCM SRAM Read Timing Control Port.
    } b;    
} CpuSramCtrl_t;

///  @brief 0xF01150D0 : BCM Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BCM_SOC_INT_IN      : 4;         ///<BIT [3:0] BCM SOC Interrupt.
        uint32_t RF01150D0_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} BcmCtrl_t;

///  @brief 0xF01150E8 : PCIe Control 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF01150E8_7_0_RSVD   : 8;         ///<BIT [7:0] PCIe PHY RESERVED_IN0.
        uint32_t RF01150E8_15_8_RSVD  : 8;         ///<BIT [15:8] PCIe PHY RESERVED_IN1.
        uint32_t RF01150E8_23_16_RSVD : 8;         ///<BIT [23:16] PCIe PHY RESERVED_IN2.
        uint32_t RF01150E8_31_24_RSVD : 8;         ///<BIT [31:24] PCIe PHY RESERVED_IN3.
    } b;    
} PcieCtrl3_t;

///  @brief 0xF01150EC : PCIe Control 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF01150EC_7_0_RSVD   : 8;         ///<BIT [7:0] PCIe PHY RESERVED_OUT0.
        uint32_t RF01150EC_15_8_RSVD  : 8;         ///<BIT [15:8] PCIe PHY RESERVED_OUT1.
        uint32_t RF01150EC_23_16_RSVD : 8;         ///<BIT [23:16] PCIe PHY RESERVED_OUT2.
        uint32_t RF01150EC_31_24_RSVD : 8;         ///<BIT [31:24] PCIe PHY RESERVED_OUT3.
    } b;    
} PcieCtrl4_t;

///  @brief 0xF0115100 : Testpin Output Enable
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TESTPIN_OE           :16;         ///<BIT [15:0] Testpin Output Enable.
        uint32_t RF0115100_23_16_RSVD : 8;         ///<BIT [23:16] Reserved.
        uint32_t PIN_MUX_GRP_SEL      : 8;         ///<BIT [31:24] PIN_MUX Group Select.
    } b;    
} TestpinOutputEn_t;

///  @brief 0xF0115104 : Testpin Output
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TESTPIN_OUT          :16;         ///<BIT [15:0] Testpin Out.
        uint32_t RF0115104_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TestpinOutput_t;

///  @brief 0xF0115108 : Testpin Input
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TESTPIN_IN           :16;         ///<BIT [15:0] Testpin In.
        uint32_t RF0115108_19_16_RSVD : 4;         ///<BIT [19:16] Reserved.
        uint32_t TESTPIN10_EDG_SEL    : 1;         ///<BIT [20] PIN_TEST[10] Interrupt Edge Select.
        uint32_t TESTPIN10_INT_CLR    : 1;         ///<BIT [21] PIN_TEST[10] Interrupt Clear.
        uint32_t TESTPIN11_EDG_SEL    : 1;         ///<BIT [22] PIN_TEST[11] Interrupt Edge Select.
        uint32_t TESTPIN11_INT_CLR    : 1;         ///<BIT [23] PIN_TEST[11] Interrupt Clear.
        uint32_t RF0115108_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TestpinIn_t;

///  @brief 0xF011510C : Testpin Latched Input
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TESTPIN_LATCH_IN     :16;         ///<BIT [15:0] Testpin Latched In Value.
        uint32_t RF011510C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TestpinLatchedIn_t;

///  @brief 0xF0115110 : UART Pin Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t UART_CTRL_FW_RX1     : 1;         ///<BIT [0] UART Control Firmware.
        uint32_t UART_CTRL_FW_TX1     : 1;         ///<BIT [1] UART1 Control Firmware.
        uint32_t RF0115110_2_RSVD     : 1;         ///<BIT [2] Reserved.
        uint32_t UART1_TX_OUT         : 1;         ///<BIT [3] UART1 Tx Output.
        uint32_t UART1_RX_OUT         : 1;         ///<BIT [4] UART1 Rx Output.
        uint32_t UART1_TX_OE          : 1;         ///<BIT [5] UART1 Tx Output Enable.
        uint32_t UART1_RX_OE          : 1;         ///<BIT [6] UART1 Rx Output Enable.
        uint32_t UART1_DIS            : 1;         ///<BIT [7] UART1 Disable.
        uint32_t UART_STRL_FW_RX2     : 1;         ///<BIT [8] UART2 Control FW Rx2.
        uint32_t UART_CTRL_FW_TX2     : 1;         ///<BIT [9] UART2 Control FW Tx.
        uint32_t UART2_TX_OUT         : 1;         ///<BIT [10] UART2 Tx Output.
        uint32_t UART2_RX_OUT         : 1;         ///<BIT [11] UART2 Rx Output.
        uint32_t UART2_TX_OE          : 1;         ///<BIT [12] UART2 Tx Output Enable.
        uint32_t UART2_RX_OE          : 1;         ///<BIT [13] UART2 Rx Output Enable.
        uint32_t UART2_DIS            : 1;         ///<BIT [14] UART2 Disable.
        uint32_t RF0115110_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t UART1_RX_FR_PAD      : 1;         ///<BIT [16] UART1 Rx Input.
        uint32_t UART1_TX_FR_PAD      : 1;         ///<BIT [17] UART1 Tx Input.
        uint32_t UART2_RX_FR_PAD      : 1;         ///<BIT [18] UART2 Rx Input.
        uint32_t UART2_TX_FR_PAD      : 1;         ///<BIT [19] UART2 Tx Input.
        uint32_t RF0115110_31_20_RSVD :12;         ///<BIT [31:20] Reserved.
    } b;    
} UartPinCtrl_t;

///  @brief 0xF0115120 : Debug Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PETM_EN             : 1;         ///<BIT [0] Parallel Embedded Trace Macrocell (ETM) Mode Enable.
        uint32_t FCT_MCP_AHB_EN      : 1;         ///<BIT [1] Enable Micron Co-processor.
        uint32_t RF0115120_15_2_RSVD :14;         ///<BIT [15:2] Reserved.
        uint32_t C_REV               :16;         ///<BIT [31:16] Chip Revision Number.
    } b;    
} DbgCtrl1_t;


/// @brief 0xF0115000-0xF0115124 : Apb_t MODULE
typedef struct
{
    FastIntrReqSelectioForCpu0_t fastIntrReqSelectioForCpu0;     ///< 0xF0115000 : Fast Interrupt Request Selection for CPU0
    FastIntrReqSelectioForCpu1_t fastIntrReqSelectioForCpu1;     ///< 0xF0115004 : Fast Interrupt Request Selection for CPU1
    FastIntrReqSelectioForCpu2_t fastIntrReqSelectioForCpu2;     ///< 0xF0115008 : Fast Interrupt Request Selection for CPU2
    VectoredIntrControllCtrl_t vectoredIntrControllCtrl;         ///< 0xF011500C : Vectored Interrupt Controller Control
    Rom0Ctrl_t rom0Ctrl;                                         ///< 0xF0115010 : ROM0 Control
    uint32_t CPU0_ROM_TEST_RD_L;                                 ///< 0xF0115014 : ROM0 Test Read
    uint8_t rsvd_RF0115018[8];                                   ///< 0xF0115018 - 0xF011501F : Reserved.
    Rom1Ctrl_t rom1Ctrl;                                         ///< 0xF0115020 : ROM1 Control
    uint32_t CPU1_ROM_TEST_RD_L;                                 ///< 0xF0115024 : ROM1 Test Read
    uint8_t rsvd_RF0115028[8];                                   ///< 0xF0115028 - 0xF011502F : Reserved.
    Rom2Ctrl_t rom2Ctrl;                                         ///< 0xF0115030 : ROM2 Control
    uint32_t CPU2_ROM_TEST_RD_L;                                 ///< 0xF0115034 : ROM2 Test Read
    uint8_t rsvd_RF0115038[24];                                  ///< 0xF0115038 - 0xF011504F : Reserved.
    McDdrCtrl0_t mcDdrCtrl0;                                     ///< 0xF0115050 : MC DDR Control 0
    uint8_t rsvd_RF0115054[4];                                   ///< 0xF0115054 - 0xF0115057 : Reserved.
    uint32_t MC5_MC_PC_EV_INC_31_0;                              ///< 0xF0115058 : MC DDR Control 2
    uint32_t MC5_MC_PC_EV_INC_63_32;                             ///< 0xF011505C : MC DDR Control 3
    uint32_t MC5_DEBUG_PORT_MC_31_0;                             ///< 0xF0115060 : MC DDR Control 4
    uint32_t MC5_DEBUG_PORT_MC_63_32;                            ///< 0xF0115064 : MC DDR Control 5
    uint32_t MC5_DC_MMAP0_CH0;                                   ///< 0xF0115068 : MC DDR Control 6
    uint32_t MC5_DC_MMAP1_CH0;                                   ///< 0xF011506C : MC DDR Control 7
    uint8_t rsvd_RF0115070[16];                                  ///< 0xF0115070 - 0xF011507F : Reserved.
    PadCfg_t padCfg;                                             ///< 0xF0115080 : Pad Configuration
    uint8_t rsvd_RF0115084[12];                                  ///< 0xF0115084 - 0xF011508F : Reserved.
    RstCtrl_t rstCtrl;                                           ///< 0xF0115090 : Reset Control
    uint8_t rsvd_RF0115094[12];                                  ///< 0xF0115094 - 0xF011509F : Reserved.
    SramCtrl1_t sramCtrl1;                                       ///< 0xF01150A0 : SRAM Control 1
    SramCtrl2_t sramCtrl2;                                       ///< 0xF01150A4 : SRAM Control 2
    uint8_t rsvd_RF01150A8[8];                                   ///< 0xF01150A8 - 0xF01150AF : Reserved.
    CpuSramCtrl_t cpuSramCtrl;                                   ///< 0xF01150B0 : CPU SRAM Control
    uint8_t rsvd_RF01150B4[28];                                  ///< 0xF01150B4 - 0xF01150CF : Reserved.
    BcmCtrl_t bcmCtrl;                                           ///< 0xF01150D0 : BCM Control
    uint8_t rsvd_RF01150D4[20];                                  ///< 0xF01150D4 - 0xF01150E7 : Reserved.
    PcieCtrl3_t pcieCtrl3;                                       ///< 0xF01150E8 : PCIe Control 3
    PcieCtrl4_t pcieCtrl4;                                       ///< 0xF01150EC : PCIe Control 4
    uint8_t rsvd_RF01150F0[16];                                  ///< 0xF01150F0 - 0xF01150FF : Reserved.
    TestpinOutputEn_t testpinOutputEn;                           ///< 0xF0115100 : Testpin Output Enable
    TestpinOutput_t testpinOutput;                               ///< 0xF0115104 : Testpin Output
    TestpinIn_t testpinIn;                                       ///< 0xF0115108 : Testpin Input
    TestpinLatchedIn_t testpinLatchedIn;                         ///< 0xF011510C : Testpin Latched Input
    UartPinCtrl_t uartPinCtrl;                                   ///< 0xF0115110 : UART Pin Control
    uint8_t rsvd_RF0115114[12];                                  ///< 0xF0115114 - 0xF011511F : Reserved.
    DbgCtrl1_t dbgCtrl1;                                         ///< 0xF0115120 : Debug Control 1
} Apb_t;
COMPILE_ASSERT(offsetof(Apb_t,dbgCtrl1)==0xF0115120-0xF0115000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile Apb_t rApb;                          ///< 0xF0115000-0xF0115124 : Apb_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

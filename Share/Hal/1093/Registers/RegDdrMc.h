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
//! @brief DDR Memory Controller Registers
//!
//! Doc Version: STANDARD
//! Doc Date: 2/19/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  DDR Memory Controller Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF1000000 : Memory Controller ID and Revision
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t REV_ECO              : 4;         ///<BIT [3:0] Memory Controller Branch or ECO Revision Number.
        uint32_t REL_DATE             : 8;         ///<BIT [11:4] Release Date.
        uint32_t REL_MO               : 8;         ///<BIT [19:12] Release Month.
        uint32_t REL_YR               : 4;         ///<BIT [23:20] Release Year.
        uint32_t ARCH_VER             : 4;         ///<BIT [27:24] Memory Controller Architecture Generation.
        uint32_t DUAL_CH              : 1;         ///<BIT [28] Dual DDR Channel Support.
        uint32_t SHADOW_MEM           : 1;         ///<BIT [29] Shadow Memory Support.
        uint32_t RF1000000_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;
} MemControllIdAndRevision_t;

///  @brief 0xF1000004 : Memory Controller Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MC_IDLE              : 1;         ///<BIT [0] Memory Controller Idle.
        uint32_t RRB_EMPTY            : 1;         ///<BIT [1] RRB Empty.
        uint32_t ROB_EMPTY            : 1;         ///<BIT [2] ROB Empty.
        uint32_t WCB_EMPTY            : 1;         ///<BIT [3] WCB Empty.
        uint32_t RF1000004_7_4_RSVD   : 4;         ///<BIT [7:4] Reserved.
        uint32_t CH_IDLE_0            : 1;         ///<BIT [8] Channel 0 Idle.
        uint32_t RF1000004_9_RSVD     : 1;         ///<BIT [9] Reserved.
        uint32_t SMTQ_IDLE_0          : 1;         ///<BIT [10] Channel 0 SMTQ Idle.
        uint32_t BQ_EMPTY_0           : 1;         ///<BIT [11] Channel 0 BQ Empty.
        uint32_t TC_IDLE_0            : 1;         ///<BIT [12] Channel 0 TC Idle.
        uint32_t DPCR_IDLE_0          : 1;         ///<BIT [13] Channel 0 DPCR Idle.
        uint32_t DPCW_IDLE_0          : 1;         ///<BIT [14] Channel 0 DPCW Idle.
        uint32_t RF1000004_31_15_RSVD :17;         ///<BIT [31:15] Reserved.
    } b;
} MemControllStat_t;

///  @brief 0xF1000008 : DRAM Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INIT_DONE00         : 1;         ///<BIT [0] Channel 0 CS0 DDR Initialization Status.
        uint32_t STATE_PD00          : 1;         ///<BIT [1] Channel 0 CS0 DDR Power Down Status.
        uint32_t STATE_SR00          : 1;         ///<BIT [2] Channel 0 CS0 DDR Self-Refresh Status.
        uint32_t STATE_DPD00         : 1;         ///<BIT [3] Channel 0 CS0 DDR Deep Power Down Status.
        uint32_t INIT_DONE01         : 1;         ///<BIT [4] Channel 0 CS1 DDR Initialization Status.
        uint32_t STATE_PD01          : 1;         ///<BIT [5] Channel 0 CS1 DDR Power Down Status.
        uint32_t STATE_SR01          : 1;         ///<BIT [6] Channel 0 CS1 DDR Self-Refresh Status.
        uint32_t STATE_DPD01         : 1;         ///<BIT [7] Channel 0 CS1 DDR Deep Power Down Status.
        uint32_t RF1000008_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} DramStat_t;

///  @brief 0xF100000C : DRAM Mode
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SDRAM_CAL_MODE_CH0    : 1;         ///<BIT [0] Channel 0 DDR in Calibration (CS to CMD/ADDR Latency) Mode.
        uint32_t RF100000C_1_RSVD      : 1;         ///<BIT [1] Reserved.
        uint32_t SDRAM_PAR_MODE_CH0    : 1;         ///<BIT [2] Channel 0 DDR is in Command/Address Parity Mode.
        uint32_t RF100000C_3_RSVD      : 1;         ///<BIT [3] Reserved.
        uint32_t SDRAM_W_CRC_MODE_CH0  : 1;         ///<BIT [4] Channel 0 is in DDR Write CRC Mode.
        uint32_t RF100000C_5_RSVD      : 1;         ///<BIT [5] Reserved.
        uint32_t SDRAM_RD_DBI_MODE_CH0 : 1;         ///<BIT [6] Channel 0 is in DDR Read DBI Mode.
        uint32_t RF100000C_31_7_RSVD   :25;         ///<BIT [31:7] Reserved.
    } b;
} DramMode_t;

///  @brief 0xF1000010 : Memory Controller Write Path Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AXI_IF_WD_FIFO_EMPTY_P0 : 1;         ///<BIT [0] Port 0 AXI Interface Write Data FIFO Empty.
        uint32_t AXI_IF_WD_FIFO_EMPTY_P1 : 1;         ///<BIT [1] Port 1 AXI Interface Write Data FIFO Empty.
        uint32_t AXI_IF_WD_FIFO_EMPTY_P2 : 1;         ///<BIT [2] Port 2 AXI Interface Write Data FIFO Empty.
        uint32_t RF1000010_7_3_RSVD      : 5;         ///<BIT [7:3] Reserved.
        uint32_t AXI_IF_WA_FIFO_EMPTY_P0 : 1;         ///<BIT [8] Port 0 AXI Interface Write Address FIFO Empty.
        uint32_t AXI_IF_WA_FIFO_EMPTY_P1 : 1;         ///<BIT [9] Port 1 AXI Interface Write Address FIFO Empty.
        uint32_t AXI_IF_WA_FIFO_EMPTY_P2 : 1;         ///<BIT [10] Port 2 AXI Interface Write Address FIFO Empty.
        uint32_t RF1000010_15_11_RSVD    : 5;         ///<BIT [15:11] Reserved.
        uint32_t AXI_IF_BR_FIFO_EMPTY_P0 : 1;         ///<BIT [16] Port 0 AXI Interface BRESP FIFO Empty.
        uint32_t AXI_IF_BR_FIFO_EMPTY_P1 : 1;         ///<BIT [17] Port 1 AXI Interface BRESP FIFO Empty.
        uint32_t AXI_IF_BR_FIFO_EMPTY_P2 : 1;         ///<BIT [18] Port 2 AXI Interface BRESP FIFO Empty.
        uint32_t RF1000010_23_19_RSVD    : 5;         ///<BIT [23:19] Reserved.
        uint32_t WDB_IF_PORT             : 3;         ///<BIT [26:24] WDB Current Line Fill Port.
        uint32_t WDB_IF_FIFO_ENTRY       : 1;         ///<BIT [27] WDB Line Fill FIFO Empty Status.
        uint32_t WCB_DRAINING            : 1;         ///<BIT [28] WCB Drain Status.
        uint32_t RF1000010_31_29_RSVD    : 3;         ///<BIT [31:29] Reserved.
    } b;
} MemControllWrPathStat_t;

///  @brief 0xF1000014 : Memory Controller Read Path Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AXI_IF_RD_FIFO_EMPTY_P0 : 1;         ///<BIT [0] Port 0 AXI Interface Read Data FIFO Empty.
        uint32_t AXI_IF_RD_FIFO_EMPTY_P1 : 1;         ///<BIT [1] Port 1 AXI Interface Read Data FIFO Empty.
        uint32_t AXI_IF_RD_FIFO_EMPTY_P2 : 1;         ///<BIT [2] Port 2 AXI Interface Read Data FIFO Empty.
        uint32_t RF1000014_7_3_RSVD      : 5;         ///<BIT [7:3] Reserved.
        uint32_t AXI_IF_RA_FIFO_EMPTY_P0 : 1;         ///<BIT [8] Port 0 AXI Interface Read Address FIFO Empty.
        uint32_t AXI_IF_RA_FIFO_EMPTY_P1 : 1;         ///<BIT [9] Port 1 AXI Interface Read Address FIFO Empty.
        uint32_t AXI_IF_RA_FIFO_EMPTY_P2 : 1;         ///<BIT [10] Port 2 AXI Interface Read Address FIFO Empty.
        uint32_t RF1000014_15_11_RSVD    : 5;         ///<BIT [15:11] Reserved.
        uint32_t RRB_HAZARD_P0           : 1;         ///<BIT [16] Port 0 RRB in Hazard Condition.
        uint32_t RRB_HAZARD_P1           : 1;         ///<BIT [17] Port 1 RRB in Hazard Condition.
        uint32_t RRB_HAZARD_P2           : 1;         ///<BIT [18] Port 2 RRB in Hazard Condition.
        uint32_t RF1000014_23_19_RSVD    : 5;         ///<BIT [23:19] Reserved.
        uint32_t ROB_NOT_FULL_P0         : 1;         ///<BIT [24] Port 0 ROB Not Full.
        uint32_t ROB_NOT_FULL_P1         : 1;         ///<BIT [25] Port 1ROB Not Full.
        uint32_t ROB_NOT_FULL_P2         : 1;         ///<BIT [26] Port 2 ROB Not Full.
        uint32_t RF1000014_31_27_RSVD    : 5;         ///<BIT [31:27] Reserved.
    } b;
} MemControllRdPathStat_t;

///  @brief 0xF1000020 : DDRMC User Initiated Command 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SDRAM_INIT_REQ       : 1;         ///<BIT [0] SDRAM Initialization Request.
        uint32_t WCB_DRAIN_REQ        : 1;         ///<BIT [1] WCB Drain Request.
        uint32_t RF1000020_3_2_RSVD   : 2;         ///<BIT [3:2] Reserved.
        uint32_t APD_REQ              : 1;         ///<BIT [4] Initiate Active Power-Down Request.
        uint32_t PPD_REQ              : 1;         ///<BIT [5] Initiate Precharge Power-Down Request.
        uint32_t SR_REQ               : 2;         ///<BIT [7:6] Self-Refresh Request.
        uint32_t LMR0_REQ             : 1;         ///<BIT [8] Initiate Load Mode Command to Extended Mode Register 0.
        uint32_t LMR1_REQ             : 1;         ///<BIT [9] Initiate Load Mode Command to Extended Mode Register 1.
        uint32_t LMR2_REQ             : 1;         ///<BIT [10] Initiate Load Mode Command to Extended Mode Register 2.
        uint32_t LMR3_REQ             : 1;         ///<BIT [11] Initiate Load Mode Command to Extended Mode Register 3.
        uint32_t ZQC_LONG             : 1;         ///<BIT [12] Initiate ZQ Long Calibration.
        uint32_t ZQC_SHORT            : 1;         ///<BIT [13] Initiate ZQ Short Calibration.
        uint32_t ZQ_RESET             : 1;         ///<BIT [14] Initiate ZQ Reset.
        uint32_t CONT_INIT            : 1;         ///<BIT [15] Continue Initialization.
        uint32_t DPD_REQ              : 2;         ///<BIT [17:16] Deep Power Down Request.
        uint32_t RF1000020_19_18_RSVD : 2;         ///<BIT [19:18] Reserved.
        uint32_t LRM4_REQ             : 1;         ///<BIT [20] Initiate a Mode Register Set (MRS) to Mode Register 4.
        uint32_t LRM5_REQ             : 1;         ///<BIT [21] Initiate a Mode Register Set (MRS) to Mode Register 5.
        uint32_t LRM6_REQ             : 1;         ///<BIT [22] Initiate a Mode Register Set (MRS) to Mode Register 6.
        uint32_t RF1000020_23_RSVD    : 1;         ///<BIT [23] Reserved.
        uint32_t DDRMC_CS0_CMD0       : 1;         ///<BIT [24] DDRMC Chip Select 0 Command 0.
        uint32_t DDRMC_CS1_CMD0       : 1;         ///<BIT [25] DDRMC Chip Select 1 Command 0.
        uint32_t RF1000020_27_26_RSVD : 2;         ///<BIT [27:26] Reserved.
        uint32_t DDRMC_CH0_CMD0       : 1;         ///<BIT [28] DDRMC Channel 0 Command 0.
        uint32_t RF1000020_31_29_RSVD : 3;         ///<BIT [31:29] Reserved.
    } b;
} DdrmcUserInitiateCmd0_t;

///  @brief 0xF1000024 : DDRMC User Initiated Command 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MR_ADDR              : 8;         ///<BIT [7:0] 8-Bit Mode Register Address Used for MRW and MRR.
        uint32_t RF1000024_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t DDRMC_MRR            : 1;         ///<BIT [16] Mode Register Read (MRR).
        uint32_t DDRMC_MRW            : 1;         ///<BIT [17] Mode Register Write (MRW).
        uint32_t RF1000024_23_18_RSVD : 6;         ///<BIT [23:18] Reserved.
        uint32_t DDRMC_CS0_CMD1       : 1;         ///<BIT [24] DDRMC Chip Select 0 Command 1.
        uint32_t DDRMC_CS1_CMD1       : 1;         ///<BIT [25] DDRMC Chip Select 1 Command 1.
        uint32_t RF1000024_27_26_RSVD : 2;         ///<BIT [27:26] Reserved.
        uint32_t DDRMC_CH0_CMD1       : 1;         ///<BIT [28] DDRMC Channel 0 Command 1.
        uint32_t RF1000024_31_29_RSVD : 3;         ///<BIT [31:29] Reserved.
    } b;
} DdrmcUserInitiateCmd1_t;

///  @brief 0xF1000028 : DDRMC User Initiated Command 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MPRRD_PAGE1_REQ      : 1;         ///<BIT [0] MPRRD Page 1 Request.
        uint32_t MPRRD_PAGE2_REQ      : 1;         ///<BIT [1] MPRRD Page 2 Request.
        uint32_t MPRRD_PAGE3_REQ      : 1;         ///<BIT [2] MPRRD Page 3 Request.
        uint32_t DEVICE_SEL           : 5;         ///<BIT [7:3] Device Select.
        uint32_t RF1000028_23_8_RSVD  :16;         ///<BIT [23:8] Reserved.
        uint32_t DDRMC_CS0_CMD2       : 1;         ///<BIT [24] DDRMC Chip Select 0 Command 2.
        uint32_t DDRMC_CS1_CMD2       : 1;         ///<BIT [25] DDRMC Chip Select 1 Command 2.
        uint32_t RF1000028_27_26_RSVD : 2;         ///<BIT [27:26] Reserved.
        uint32_t DDRMC_CH0_CMD2       : 1;         ///<BIT [28] DDRMC Channel 0 Command 2.
        uint32_t RF1000028_31_29_RSVD : 3;         ///<BIT [31:29] Reserved.
    } b;
} DdrmcUserInitiateCmd2_t;

///  @brief 0xF1000040 : Internal SRAM Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ROB_RTC                 : 4;         ///<BIT [3:0] RTC for ROB SRAM.
        uint32_t ROB_WTC                 : 4;         ///<BIT [7:4] WTC for ROB SRAM.
        uint32_t SRAM_PD_EXIT_WAIT_CYCLE : 6;         ///<BIT [13:8] SRAM Power-Down Exit Wait Cycles.
        uint32_t RF1000040_15_14_RSVD    : 2;         ///<BIT [15:14] Reserved.
        uint32_t WDB_RTC                 : 4;         ///<BIT [19:16] RTC for WDB SRAM and AXI WDATA SRAM.
        uint32_t WDB_WTC                 : 4;         ///<BIT [23:20] WTC for WDB SRAM and AXI WDATA SRAM.
        uint32_t RTC_REG_TBL             : 4;         ///<BIT [27:24] RTC for Register Table SRAM.
        uint32_t WTC_REG_TBL             : 4;         ///<BIT [31:28] WTC for Register Table SRAM.
    } b;
} IntSramCtrl_t;

///  @brief 0xF1000044 : Memory Controller Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DFC_MODE               : 1;         ///<BIT [0] DFC Sequence Mode.
        uint32_t HALT_SCDLR             : 1;         ///<BIT [1] Halt the Scheduler.
        uint32_t RF1000044_3_2_RSVD     : 2;         ///<BIT [3:2] Reserved.
        uint32_t WR_LVL_MODE            : 1;         ///<BIT [4] Write Level Mode.
        uint32_t DDR_DIR_ACC_TST        : 1;         ///<BIT [5] DDR Direct Access Test Mode.
        uint32_t ACC_SR_ERR_EN          : 1;         ///<BIT [6] Enable Access when there is a Self-Refresh Error.
        uint32_t RF1000044_7_RSVD       : 1;         ///<BIT [7] Reserved.
        uint32_t BLOCK_MSTR_DATA_REQ_P0 : 1;         ///<BIT [8] Block Master Data Requests from Master Port 0.
        uint32_t BLOCK_MSTR_DATA_REQ_P1 : 1;         ///<BIT [9] Block Master Data Requests from Master Port 1.
        uint32_t BLOCK_MSTR_DATA_REQ_P2 : 1;         ///<BIT [10] Block Master Data Requests from Master Port 2.
        uint32_t RF1000044_15_11_RSVD   : 5;         ///<BIT [15:11] Reserved.
        uint32_t BLOCK_ALL_DATA_REQ     : 1;         ///<BIT [16] Block All Data Requests.
        uint32_t RF1000044_23_17_RSVD   : 7;         ///<BIT [23:17] Reserved.
        uint32_t AXI_CHOP               : 3;         ///<BIT [26:24] AXI Chop.
        uint32_t CHOP_CHANGE            : 1;         ///<BIT [27] Boundary Configuration Change.
        uint32_t RF1000044_29_28_RSVD   : 2;         ///<BIT [29:28] Reserved.
        uint32_t ADDR_MAP_LOCK          : 1;         ///<BIT [30] Address Mapping Lock.
        uint32_t TZ_LOCK                : 1;         ///<BIT [31] Trust Zone Lock.
    } b;
} MemControllCtrl0_t;

///  @brief 0xF1000048 : DDRMC Exclusive Monitor Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF1000048_11_0_RSVD  :12;         ///<BIT [11:0] Reserved.
        uint32_t EXCLU_EN_M0          : 1;         ///<BIT [12] Enable Exclusive Access Monitoring for Master 0.
        uint32_t EXCLU_EN_M1          : 1;         ///<BIT [13] Enable Exclusive Access Monitoring for Master 1.
        uint32_t EXCLU_EN_M2          : 1;         ///<BIT [14] Enable Exclusive Access Monitoring for Master 2.
        uint32_t RF1000048_31_15_RSVD :17;         ///<BIT [31:15] Reserved.
    } b;
} DdrmcExclusivMonitorCtrl_t;

///  @brief 0xF100004C : DDRMC Data Coherency Check Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t COH_CHK_M0_DIS      : 1;         ///<BIT [0] Data Coherency Check for Master 0 Disable.
        uint32_t COH_CHK_M1_DIS      : 1;         ///<BIT [1] Data Coherency Check for Master 1 Disable.
        uint32_t COH_CHK_M2_DIS      : 1;         ///<BIT [2] Data Coherency Check for Master 2 Disable.
        uint32_t RF100004C_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;
} DdrmcDataCoherencChkCtrl_t;

///  @brief 0xF1000050 : DDRMC Restrict Zone Access Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RZ_ACCESS_EN_P0     : 1;         ///<BIT [0] Enable Access to the Restricted Zone for Master 0.
        uint32_t RZ_ACCESS_EN_P1     : 1;         ///<BIT [1] Enable Access to the Restricted Zone for Master 1.
        uint32_t RZ_ACCESS_EN_P2     : 1;         ///<BIT [2] Enable Access to the Restricted Zone for Master 2.
        uint32_t RF1000050_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;
} DdrmcRestrictZoneAccessCtrl_t;

///  @brief 0xF1000054 : Memory Controller Power Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_AUTO_AC_OFF_EN   : 1;         ///<BIT [0] PHY Automatic Address and Command Off Enable.
        uint32_t PHY_CLK_GATING_EN    : 1;         ///<BIT [1] Enable PHY Clocks Automatic Gating.
        uint32_t RF1000054_3_2_RSVD   : 2;         ///<BIT [3:2] Reserved.
        uint32_t AC_OFF_DLY           : 4;         ///<BIT [7:4] AC Off Delay.
        uint32_t AC_ON_DLY            : 4;         ///<BIT [11:8] AC On Delay.
        uint32_t RF1000054_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t DIS_BQ_CK_GATE       : 1;         ///<BIT [16] Disable BQ Local Clock Gating Feature.
        uint32_t DIS_DPCR_CK_GATE     : 1;         ///<BIT [17] Disable DPCR Local Clock Gating Feature.
        uint32_t DIS_ROB_CK_GATE      : 1;         ///<BIT [18] Disable ROB Local Clock Gating Feature.
        uint32_t DIS_ROB_SRAM_CK_GATE : 1;         ///<BIT [19] Disable ROB SRAM Local Clock Gating Feature.
        uint32_t DIS_WDB_SRAM_CK_GATE : 1;         ///<BIT [20] Disable WDB SRAM Local Clock Gating Feature.
        uint32_t RF1000054_31_21_RSVD :11;         ///<BIT [31:21] Reserved.
    } b;
} MemControllPowerCtrl_t;

///  @brief 0xF1000058 : DDRMC Write Command Buffer Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AUTO_WCB_DRAIN_EN   : 1;         ///<BIT [0] Enable Automatic WCB Drain.
        uint32_t WCB_DRAIN_LVL       : 2;         ///<BIT [2:1] WCB Drain Level.
        uint32_t RF1000058_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;
} DdrmcWrCmdBufCtrl_t;

///  @brief 0xF100005C : Bank Queue Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BQ_DEPTH            : 2;         ///<BIT [1:0] Depth of Band Queue, First Stage.
        uint32_t RF100005C_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;
} BankQueueCtrl_t;

///  @brief 0xF1000060 : ROB Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ROB_DELAY_READ_EN   : 1;         ///<BIT [0] ROB Delay Read Enable.
        uint32_t ROB_DATA_INTLV_DIS  : 1;         ///<BIT [1] ROB Data Interleave Disable.
        uint32_t RF1000060_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;
} RobCtrl_t;

///  @brief 0xF1000080 : Port RRB Weight
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t WEIGHT_P0            : 4;         ///<BIT [3:0] Port 0 RRB Weight.
        uint32_t WEIGHT_P1            : 4;         ///<BIT [7:4] Port 1 RRB Weight.
        uint32_t WEIGHT_P2            : 4;         ///<BIT [11:8] Port 2 RRB Weight.
        uint32_t RF1000080_31_12_RSVD :20;         ///<BIT [31:12] Reserved.
    } b;
} PortRrbWt_t;

///  @brief 0xF1000084 : Port Priority Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PRIORITY_P0          : 2;         ///<BIT [1:0] Port 0 Priority.
        uint32_t PRIORITY_P1          : 2;         ///<BIT [3:2] Port 1 Priority.
        uint32_t PRIORITY_P2          : 2;         ///<BIT [5:4] Port 2 Priority.
        uint32_t RF1000084_23_6_RSVD  :18;         ///<BIT [23:6] Reserved.
        uint32_t PORT_PRIORITY_EN_P0  : 1;         ///<BIT [24] Port 0 Priority Enable.
        uint32_t PORT_PRIORITY_EN_P1  : 1;         ///<BIT [25] Port 1 Priority Enable.
        uint32_t PORT_PRIORITY_EN_P2  : 1;         ///<BIT [26] Port 2 Priority Enable.
        uint32_t RF1000084_31_27_RSVD : 5;         ///<BIT [31:27] Reserved.
    } b;
} PortPriorityCtrl_t;

///  @brief 0xF1000088 : RRB Starvation Prevention 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RRB_STARV_P0         : 8;         ///<BIT [7:0] RRB Starvation Value for Port 0.
        uint32_t RRB_STARV_P1         : 8;         ///<BIT [15:8] RRB Starvation Value for Port 1.
        uint32_t RRB_STARV_P2         : 8;         ///<BIT [23:16] RRB Starvation Value for Port 2.
        uint32_t RF1000088_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;
} RrbStarvatiPreventi0_t;

///  @brief 0xF10000C0 : DDRMC Register Table Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DDRMC_REG_TBL_ADDR   : 5;         ///<BIT [4:0] DDRMC Register Table Address.
        uint32_t DDRMC_REG_TBL_SEL    : 5;         ///<BIT [9:5] DDRMC Register Table Select.
        uint32_t RF10000C0_31_10_RSVD :22;         ///<BIT [31:10] Reserved.
    } b;
} DdrmcRegisterTblCtrl_t;

///  @brief 0xF10000C8 : DDRMC Register Table Data 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t REG_TABLE_OFFSET     :12;         ///<BIT [11:0] Register Table Register Offset.
        uint32_t RF10000C8_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t TABLE_REG_PAUSE      : 1;         ///<BIT [16] Register Table Pause Program.
        uint32_t TBL_REG_EOP          : 1;         ///<BIT [17] End of Program Register Table.
        uint32_t RF10000C8_31_18_RSVD :14;         ///<BIT [31:18] Reserved.
    } b;
} DdrmcRegisterTblData1_t;

///  @brief 0xF1000100 : DDRMC Performance Counter Configuration 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PC_EVENT_SEL0     : 6;         ///<BIT [5:0] Event Select for Counter 0.
        uint32_t RF1000100_6_RSVD  : 1;         ///<BIT [6] Reserved.
        uint32_t PC_REG_EN0        : 1;         ///<BIT [7] Enable Performance Counter 0.
        uint32_t PC_EVENT_SEL1     : 6;         ///<BIT [13:8] Event Select for Counter 1.
        uint32_t RF1000100_14_RSVD : 1;         ///<BIT [14] Reserved.
        uint32_t PC_REG_EN1        : 1;         ///<BIT [15] Enable Performance Counter 1.
        uint32_t PC_EVENT_SEL2     : 6;         ///<BIT [21:16] Event Select for Counter 2.
        uint32_t RF1000100_22_RSVD : 1;         ///<BIT [22] Reserved.
        uint32_t PC_REG_EN2        : 1;         ///<BIT [23] Enable Performance Counter 2.
        uint32_t PC_EVENT_SEL3     : 6;         ///<BIT [29:24] Event Select for Counter 3.
        uint32_t RF1000100_30_RSVD : 1;         ///<BIT [30] Reserved.
        uint32_t PC_REG_EN3        : 1;         ///<BIT [31] Enable Performance Counter 3.
    } b;
} DdrmcPerformaCntrCfg0_t;

///  @brief 0xF1000104 : DDRMC Performance Counter Configuration 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PC_EVENT_SEL4     : 6;         ///<BIT [5:0] Event Select for Counter 4.
        uint32_t RF1000104_6_RSVD  : 1;         ///<BIT [6] Reserved.
        uint32_t PC_REG_EN4        : 1;         ///<BIT [7] Enable Performance Counter 4.
        uint32_t PC_EVENT_SEL5     : 6;         ///<BIT [13:8] Event Select for Counter 5.
        uint32_t RF1000104_14_RSVD : 1;         ///<BIT [14] Reserved.
        uint32_t PC_REG_EN5        : 1;         ///<BIT [15] Enable Performance Counter 5.
        uint32_t PC_EVENT_SEL6     : 6;         ///<BIT [21:16] Event Select for Counter 6.
        uint32_t RF1000104_22_RSVD : 1;         ///<BIT [22] Reserved.
        uint32_t PC_REG_EN6        : 1;         ///<BIT [23] Enable Performance Counter 6.
        uint32_t PC_EVENT_SEL7     : 6;         ///<BIT [29:24] Event Select for Counter 7.
        uint32_t RF1000104_30_RSVD : 1;         ///<BIT [30] Reserved.
        uint32_t PC_REG_EN7        : 1;         ///<BIT [31] Enable Performance Counter 7.
    } b;
} DdrmcPerformaCntrCfg1_t;

///  @brief 0xF1000108 : DDRMC Performance Counter Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PC_OVERFLOW         : 8;         ///<BIT [7:0] Performance Counter Overflow Flag.
        uint32_t RF1000108_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} DdrmcPerformaCntrStat_t;

///  @brief 0xF100010C : DDRMC Performance Counter Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PC_START_COND        : 1;         ///<BIT [0] Performance Counter Start Condition.
        uint32_t RF100010C_3_1_RSVD   : 3;         ///<BIT [3:1] Reserved.
        uint32_t PC_STOP_COND         : 1;         ///<BIT [4] Performance Clock Stop Condition.
        uint32_t RF100010C_15_5_RSVD  :11;         ///<BIT [15:5] Reserved.
        uint32_t PC_CLK_DIV           : 3;         ///<BIT [18:16] Performance Clock DIvider.
        uint32_t RF100010C_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;
} DdrmcPerformaCntrCtrl_t;

///  @brief 0xF1000140 : DDRMC Interrupt Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PC_INT                  : 1;         ///<BIT [0] Performance Counter Interrupt Status.
        uint32_t RF1000140_1_RSVD        : 1;         ///<BIT [1] Reserved.
        uint32_t ADC_INT                 : 1;         ///<BIT [2] Address Decode Error Interrupt Status.
        uint32_t AM_LOW_INT              : 1;         ///<BIT [3] Activity Monitor Low Threshold Interrupt Status.
        uint32_t AM_HIGH_INT             : 1;         ///<BIT [4] Activity Monitor High Threshold Interrupt Status.
        uint32_t TZ_INT                  : 1;         ///<BIT [5] Trust Zone Error Interrupt Status.
        uint32_t RETRY_BUFFER_INT_STATUS : 1;         ///<BIT [6] Parity/CRC Error Interrupt Status.
        uint32_t RF1000140_31_7_RSVD     :25;         ///<BIT [31:7] Reserved.
    } b;
} DdrmcIntrStat_t;

///  @brief 0xF1000144 : DDRMC Interrupt Enable
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PC_INT_EN           : 1;         ///<BIT [0] Performance Counter Interrupt Enable.
        uint32_t RF1000144_1_RSVD    : 1;         ///<BIT [1] Reserved.
        uint32_t ADC_INT_EN          : 1;         ///<BIT [2] Address Decode Error Interrupt Enable.
        uint32_t AM_INT_EN           : 1;         ///<BIT [3] Activity Monitor Threshold Interrupt Enable.
        uint32_t RETRY_BUFFER_INT_EN : 1;         ///<BIT [4] Parity/CRC Error Interrupt Enable.
        uint32_t TZ_INT_EN           : 1;         ///<BIT [5] Trust Zone Error Interrupt Enable.
        uint32_t RF1000144_31_6_RSVD :26;         ///<BIT [31:6] Reserved.
    } b;
} DdrmcIntrEn_t;

///  @brief 0xF1000150 : DDRMC Address Decode Error Master ID
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ERR_TRANS_ID   :28;         ///<BIT [27:0] Address Decode Error Transaction ID.
        uint32_t ERR_MSTR_ID    : 3;         ///<BIT [30:28] Address Decode Error Master Port ID.
        uint32_t ERR_TRANS_TYPE : 1;         ///<BIT [31] Address Decode Error Transaction Type.
    } b;
} DdrmcAddrDecodeErrMstrId_t;

///  @brief 0xF1000158 : DDRMC Address Decode Error Address High
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DEC_ERR_ADDR_H      : 8;         ///<BIT [7:0] Address Decode Error High Address.
        uint32_t RF1000158_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} DdrmcAddrDecodeErrAddrHigh_t;

///  @brief 0xF1000164 : Activity Monitor Threshold
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AM_TH_LOW            : 7;         ///<BIT [6:0] Activity Monitor Low Threshold In Percentage.
        uint32_t RF1000164_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint32_t AM_TH_HIGH           : 7;         ///<BIT [14:8] Activity Monitor High Threshold In Percentage.
        uint32_t RF1000164_31_15_RSVD :17;         ///<BIT [31:15] Reserved.
    } b;
} ActivityMonitorThr_t;

///  @brief 0xF1000170 : Trust Zone Error Master ID
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TZ_ERR_TRANS_ID   :28;         ///<BIT [27:0] Trust Zone Error Transaction ID.
        uint32_t TZ_ERR_MSTR_ID    : 3;         ///<BIT [30:28] Trust Zone Error Master Port ID.
        uint32_t TZ_ERR_TRANS_TYPE : 1;         ///<BIT [31] Trust Zone Error Transaction Type.
    } b;
} TrustZoneErrMstrId_t;

///  @brief 0xF1000190 : Parity CRC Error Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PAR_ERR_CH0         : 1;         ///<BIT [0] CH0 Parity Error Status.
        uint32_t CRC_ERR_CH0         : 1;         ///<BIT [1] CH0 CRC Error Status.
        uint32_t RF1000190_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;
} ParCrcErrStat_t;

///  @brief 0xF10001C4 : DDRMC Test Mode 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DDR_CMD             : 3;         ///<BIT [2:0] DDR Command.
        uint32_t RF10001C4_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;
} DdrmcTstMode0_t;

///  @brief 0xF10001C8 : DDRMC Test Mode 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DDR_CA               :20;         ///<BIT [19:0] DDR Command/Address.
        uint32_t CS0_SEL              : 1;         ///<BIT [20] Select Chip Select 0.
        uint32_t CS1_SEL              : 1;         ///<BIT [21] Select Chip Select 1.
        uint32_t RF10001C8_23_22_RSVD : 2;         ///<BIT [23:22] Reserved.
        uint32_t DDR_CH0_SEL          : 1;         ///<BIT [24] Select DDR Channel 0.
        uint32_t RF10001C8_31_25_RSVD : 7;         ///<BIT [31:25] Reserved.
    } b;
} DdrmcTstMode1_t;

///  @brief 0xF1000200 : Channel 0 MMAP CS0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CS0_VALID_ADDR       : 1;         ///<BIT [0] Chip Select 0 Valid Start Address.
        uint32_t RF1000200_7_1_RSVD   : 7;         ///<BIT [7:1] Reserved.
        uint32_t CS0_START_ADDR       : 8;         ///<BIT [15:8] Chip Select 0 Start Address.
        uint32_t AREA_LEN_CS0         : 5;         ///<BIT [20:16] Length of Memory Block CS0.
        uint32_t RF1000200_22_21_RSVD : 2;         ///<BIT [22:21] Reserved.
        uint32_t MEM_START_ADDR_CS0   : 9;         ///<BIT [31:23] Starting Address [31:23] of Memory Block for CS0.
    } b;
} Chan0MmapCs0_t;

///  @brief 0xF1000204 : Channel 0 MMAP CS1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CS1_VALID_ADDR       : 1;         ///<BIT [0] Chip Select 1 Valid Start Address.
        uint32_t RF1000204_7_1_RSVD   : 7;         ///<BIT [7:1] Reserved.
        uint32_t CS1_START_ADDR       : 8;         ///<BIT [15:8] Chip Select 1 Start Address.
        uint32_t AREA_LEN_CS1         : 5;         ///<BIT [20:16] Length of Memory Block CS1.
        uint32_t RF1000204_22_21_RSVD : 2;         ///<BIT [22:21] Reserved.
        uint32_t MEM_START_ADDR_CS1   : 9;         ///<BIT [31:23] Starting Address [31:23] of Memory Block for CS1.
    } b;
} Chan0MmapCs1_t;

///  @brief 0xF1000210 : Channel 0 Memory Controller Physical Address Map CS0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t NUM_BANKS_CH0        : 2;         ///<BIT [1:0] Number of Banks, Channel 0.
        uint32_t NUM_BANK_GRP_CH0     : 2;         ///<BIT [3:2] Number of Bank Groups, Channel 0.
        uint32_t NUM_COL_ADDR_CH0     : 4;         ///<BIT [7:4] Number of Column Address Bits, Channel 0.
        uint32_t NUM_ROW_ADDR_CH0     : 4;         ///<BIT [11:8] Number of Row Address Bits, Channel 0.
        uint32_t NUM_STACK_CHIP_CH0   : 2;         ///<BIT [13:12] Number of Stacked Chips, Channel 0.
        uint32_t RF1000210_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t DDR4_DEVICE_TYPE_CH0 : 2;         ///<BIT [17:16] DDR4 Device Physical Width, Channel 0.
        uint32_t RF1000210_23_18_RSVD : 6;         ///<BIT [23:18] Reserved.
        uint32_t BANK_ADDR_MAP_CH0    : 5;         ///<BIT [28:24] Bank Address Assignment Boundary, Channel 0.
        uint32_t RF1000210_31_29_RSVD : 3;         ///<BIT [31:29] Reserved.
    } b;
} Chan0MemControllPhysicalAddrMapCs0_t;

///  @brief 0xF1000214 : Channel 0 Memory Controller Physical Address Map CS1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t NUM_BANKS_CH0        : 2;         ///<BIT [1:0] Number of Banks, Channel 0.
        uint32_t NUM_BANK_GRP_CH0     : 2;         ///<BIT [3:2] Number of Bank Groups, Channel 0.
        uint32_t NUM_COL_ADDR_CH0     : 4;         ///<BIT [7:4] Number of Column Address Bits, Channel 0.
        uint32_t NUM_ROW_ADDR_CH0     : 4;         ///<BIT [11:8] Number of Row Address Bits, Channel 0.
        uint32_t NUM_STACK_CHIP_CH0   : 2;         ///<BIT [13:12] Number of Stacked Chips, Channel 0.
        uint32_t RF1000214_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t DDR4_DEVICE_TYPE_CH0 : 2;         ///<BIT [17:16] DDR4 Device Physical Width, Channel 0.
        uint32_t RF1000214_23_18_RSVD : 6;         ///<BIT [23:18] Reserved.
        uint32_t BANK_ADDR_MAP_CH0    : 5;         ///<BIT [28:24] Bank Address Assignment Boundary, Channel 0.
        uint32_t RF1000214_31_29_RSVD : 3;         ///<BIT [31:29] Reserved.
    } b;
} Chan0MemControllPhysicalAddrMapCs1_t;

///  @brief 0xF10002C0 : Channel 0 Memory Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF10002C0_0_RSVD    : 1;         ///<BIT [0] Reserved.
        uint32_t CA_TRAIN_WHEN_INIT  : 1;         ///<BIT [1] CA Training During Initialization.
        uint32_t FORCE_COLD_BOOT     : 1;         ///<BIT [2] Force Cold Boot.
        uint32_t PHY_MASK            : 1;         ///<BIT [3] PHY Mask.
        uint32_t TW2R_DIS            : 1;         ///<BIT [4] Disable tWT2R.
        uint32_t AVOID_CA_CONTENTION : 1;         ///<BIT [5] Avoid LPDDR3 CA Bus Contention.
        uint32_t RDIMM_MODE          : 1;         ///<BIT [6] Registered DIMM Mode.
        uint32_t A2T_MODE            : 1;         ///<BIT [7] 2T Mode.
        uint32_t RF10002C0_8_RSVD    : 1;         ///<BIT [8] Reserved.
        uint32_t SRX_AUTO_ZQCL_DIS   : 1;         ///<BIT [9] Self-Refresh Exit Auto ZQCL Disable.
        uint32_t RETRY_MODE          : 1;         ///<BIT [10] Retry Mode.
        uint32_t REFPB_MODE          : 1;         ///<BIT [11] Refresh Per Bank Mode.
        uint32_t ACS_EXIT_DLY        : 3;         ///<BIT [14:12] Automatic Clock Stop (ACS) Exit Delay.
        uint32_t ACS_EN              : 1;         ///<BIT [15] Automatic Clock Stop (ACS) Enable.
        uint32_t APS_VALUE           :12;         ///<BIT [27:16] Automatic Power Saving (APS) Value.
        uint32_t APS_TYPE            : 3;         ///<BIT [30:28] Automatic Power Saving (APS) Type.
        uint32_t RF10002C0_31_RSVD   : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0MemCtrl1_t;

///  @brief 0xF10002C4 : Channel 0 Memory Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BQ_STARV_TIME        :16;         ///<BIT [15:0] Initial Value for BQ Starvation Timer.
        uint32_t RF10002C4_27_16_RSVD :12;         ///<BIT [27:16] Reserved.
        uint32_t LOW_LATENCY_DIS      : 1;         ///<BIT [28] Low Latency Disable.
        uint32_t BLOCK_HIGH_REQ       : 1;         ///<BIT [29] Block High Priority Requests.
        uint32_t BLOCK_LOW_REQ        : 1;         ///<BIT [30] Block Low Request.
        uint32_t RF10002C4_31_RSVD    : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0MemCtrl2_t;

///  @brief 0xF10002C8 : Channel 0 Memory Control 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_IN_FF_BYPASS     : 8;         ///<BIT [7:0] PHY Input Interface Timing Control.
        uint32_t PHY_OUT_FF_BYPASS    : 8;         ///<BIT [15:8] PHY Output Interface Timing Control.
        uint32_t RF10002C8_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;
} Chan0MemCtrl3_t;

///  @brief 0xF1000300 : Channel 0 DRAM Configuration 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DDR_DATA_WIDTH    : 3;         ///<BIT [2:0] DDR Data Bus Width.
        uint32_t S4_TYPE           : 1;         ///<BIT [3] S4 Type.
        uint32_t SDRAM_MEM_TYPE    : 4;         ///<BIT [7:4] SDRAM Memory Type.
        uint32_t CWL               : 4;         ///<BIT [11:8] CAS Write Latency.
        uint32_t CAS_LATENCY       : 5;         ///<BIT [16:12] CAS Latency Value.
        uint32_t RF1000300_17_RSVD : 1;         ///<BIT [17] Reserved.
        uint32_t CWL_4             : 1;         ///<BIT [18] CAS Write Latency Value Bit 4.
        uint32_t RL3_OPTION        : 1;         ///<BIT [19] RL3 Option Support.
        uint32_t DDR_BURST_LEN     : 3;         ///<BIT [22:20] DDR Burst Length (BL).
        uint32_t WL_SEL            : 1;         ///<BIT [23] WL Select.
        uint32_t CA_LATENCY        : 4;         ///<BIT [27:24] CS to Command/Address Latency.
        uint32_t CAP_LATENCY       : 4;         ///<BIT [31:28] Command/Address Parity Latency.
    } b;
} Chan0DramCfg1_t;

///  @brief 0xF1000304 : Channel 0 DRAM Configuration 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SDRAM_DLL_RESET      : 1;         ///<BIT [0] SDRAM DLL Reset.
        uint32_t SDRAM_PD_MODE        : 1;         ///<BIT [1] SDRAM Power Down Mode.
        uint32_t CAS_BT               : 1;         ///<BIT [2] DRAM Burst Type.
        uint32_t RF1000304_3_RSVD     : 1;         ///<BIT [3] Reserved.
        uint32_t SDRAM_DLL_DIS        : 1;         ///<BIT [4] SDRAM DLL Disable.
        uint32_t WRITE_LEVEL_EN       : 1;         ///<BIT [5] Write Level Enable MR Bit for DDR3, DDR4, and LPDDR3.
        uint32_t RF1000304_6_RSVD     : 1;         ///<BIT [6] Reserved.
        uint32_t DRAM_OUT_DIS         : 1;         ///<BIT [7] Output Disable Bit in DRAM Mode Register.
        uint32_t ASR                  : 2;         ///<BIT [9:8] Automatic Self-Refresh (ASR).
        uint32_t RF1000304_10_RSVD    : 1;         ///<BIT [10] Reserved.
        uint32_t WD_CRC               : 1;         ///<BIT [11] Write Data CRC Enable.
        uint32_t DDR_MPR              : 1;         ///<BIT [12] DDR Multi-Purpose Register.
        uint32_t MPR_PAGE_SEL         : 2;         ///<BIT [14:13] Select MPR Page for Read/Write.
        uint32_t RF1000304_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t GEARDOWN_EN          : 1;         ///<BIT [16] Geardown Mode Enable.
        uint32_t PER_DRAM_ADDR        : 1;         ///<BIT [17] Enable Per DRAM Addressability.
        uint32_t WL_CRC_DM            : 2;         ///<BIT [19:18] Write CMD Latency when CRC and DM are Enabled.
        uint32_t FINE_REF             : 3;         ///<BIT [22:20] Fine Reference.
        uint32_t RF1000304_23_RSVD    : 1;         ///<BIT [23] Reserved.
        uint32_t MPR_FORMAT           : 2;         ///<BIT [25:24] MPR Format.
        uint32_t RF1000304_27_26_RSVD : 2;         ///<BIT [27:26] Reserved.
        uint32_t MRR_DATA_BYTE        : 2;         ///<BIT [29:28] MRR Data Byte.
        uint32_t RF1000304_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;
} Chan0DramCfg2_t;

///  @brief 0xF1000308 : Channel 0 DRAM Configuration 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TEMP_CTRL_REF_RNG    : 1;         ///<BIT [0] Temperature Controlled Refresh Range.
        uint32_t RF1000308_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t TEMP_CTRL_REF_MODE   : 1;         ///<BIT [2] Temperature Controlled Refresh Mode.
        uint32_t INT_VREF_MON         : 1;         ///<BIT [3] Internal Vref Monitor.
        uint32_t SR_ABT_MODE          : 1;         ///<BIT [4] Self Refresh Abort.
        uint32_t RD_PRE_TRAIN         : 1;         ///<BIT [5] Read Preamble Training Mode.
        uint32_t RF1000308_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint32_t RD_PRE               : 1;         ///<BIT [8] Read Preamble.
        uint32_t RF1000308_9_RSVD     : 1;         ///<BIT [9] Reserved.
        uint32_t WR_PRE               : 1;         ///<BIT [10] Write Preamble.
        uint32_t RF1000308_11_RSVD    : 1;         ///<BIT [11] Reserved.
        uint32_t DATA_MSK             : 1;         ///<BIT [12] Data Mask.
        uint32_t WR_DBI_EN            : 1;         ///<BIT [13] Write DBI.
        uint32_t READ_DBI_EN          : 1;         ///<BIT [14] Read DBI.
        uint32_t RF1000308_15_RSVD    : 1;         ///<BIT [15] Reserved.
        uint32_t VREF_TRAIN_VAL       : 6;         ///<BIT [21:16] Vref DQ Training Value.
        uint32_t RF1000308_23_22_RSVD : 2;         ///<BIT [23:22] Reserved.
        uint32_t VREF_TRAIN_EN        : 1;         ///<BIT [24] Vref DQ Training Enable.
        uint32_t VREF_TRAIN_RNG       : 1;         ///<BIT [25] Vref DQ Training Range.
        uint32_t RF1000308_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;
} Chan0DramCfg3_t;

///  @brief 0xF1000310 : Channel 0 DRAM Configuration 5 CS0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PASR_CS0             :16;         ///<BIT [15:0] Partial Array Self-Refresh PASR, CS0.
        uint32_t ODS_CS0              : 4;         ///<BIT [19:16] Output Driver Strength, CS0.
        uint32_t RTT_NOM_CS0          : 3;         ///<BIT [22:20] RTT On-Die Termination Nominal Termination Value, CS0.
        uint32_t RF1000310_23_RSVD    : 1;         ///<BIT [23] Reserved.
        uint32_t RTT_WR_CS0           : 2;         ///<BIT [25:24] ODT Termination Value for Write, CS0.
        uint32_t RF1000310_27_26_RSVD : 2;         ///<BIT [27:26] Reserved.
        uint32_t RTT_PARK_CS0         : 3;         ///<BIT [30:28] RTT Park, CS0.
        uint32_t PD_CTRL_CS0          : 1;         ///<BIT [31] Power Down Control, CS0.
    } b;
} Chan0DramCfg5Cs0_t;

///  @brief 0xF1000314 : Channel 0 DRAM Configuration 5 CS1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PASR_CS1             :16;         ///<BIT [15:0] Partial Array Self-Refresh PASR, CS1.
        uint32_t ODS_CS1              : 4;         ///<BIT [19:16] Output Driver Strength, CS1.
        uint32_t RTT_NOM_CS1          : 3;         ///<BIT [22:20] RTT On-Die Termination Nominal Termination Value, CS1.
        uint32_t RF1000314_23_RSVD    : 1;         ///<BIT [23] Reserved.
        uint32_t RTT_WR_CS1           : 2;         ///<BIT [25:24] ODT Termination Value for Write, CS1.
        uint32_t RF1000314_27_26_RSVD : 2;         ///<BIT [27:26] Reserved.
        uint32_t RTT_PARK_CS1         : 3;         ///<BIT [30:28] RTT Park, CS1.
        uint32_t PD_CTRL_CS1          : 1;         ///<BIT [31] Power Down Control, CS1.
    } b;
} Chan0DramCfg5Cs1_t;

///  @brief 0xF1000340 : Channel 0 DRAM Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ODT0_WR_EN : 4;         ///<BIT [3:0] ODT0 Write Enable.
        uint32_t ODT0_RD_EN : 4;         ///<BIT [7:4] ODT0 Read Enable.
        uint32_t ODT1_WR_EN : 4;         ///<BIT [11:8] ODT1 Write Enable.
        uint32_t ODT1_RD_EN : 4;         ///<BIT [15:12] ODT1 Read Enable.
        uint32_t ODT2_WR_EN : 4;         ///<BIT [19:16] ODT2 Write Enable.
        uint32_t ODT2_RD_EN : 4;         ///<BIT [23:20] ODT2 Read Enable.
        uint32_t ODT3_WR_EN : 4;         ///<BIT [27:24] ODT3 Write Enable.
        uint32_t ODT3_RD_EN : 4;         ///<BIT [31:28] ODT3 Read Enable.
    } b;
} Chan0DramCtrl1_t;

///  @brief 0xF1000344 : Channel 0 DRAM Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ODT0_SWITCH_MODE     : 2;         ///<BIT [1:0] ODT0 Switch Mode.
        uint32_t ODT1_SWITCH_MODE     : 2;         ///<BIT [3:2] ODT1 Switch Mode.
        uint32_t ODT2_SWITCH_MODE     : 2;         ///<BIT [5:4] ODT2 Switch Mode.
        uint32_t ODT3_SWITCH_MODE     : 2;         ///<BIT [7:6] ODT3 Switch Mode.
        uint32_t RF1000344_23_8_RSVD  :16;         ///<BIT [23:8] Reserved.
        uint32_t PAD_TERM_SWITCH_MODE : 2;         ///<BIT [25:24] Pad Termination Switch Mode.
        uint32_t RF1000344_27_26_RSVD : 2;         ///<BIT [27:26] Reserved.
        uint32_t ODT0_FORCE           : 1;         ///<BIT [28] ODT0.
        uint32_t ODT1_FORCE           : 1;         ///<BIT [29] ODT1.
        uint32_t RF1000344_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;
} Chan0DramCtrl2_t;

///  @brief 0xF1000348 : Channel 0 DRAM Control 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ODT_RD_EXTEND_VAL    : 4;         ///<BIT [3:0] ODT Level Extension After ODT Start for Read.
        uint32_t ODT_WR_EXTEND_VAL    : 4;         ///<BIT [7:4] ODT Level Extension After ODT Start for Write.
        uint32_t ODT_RD_START_DLY_VAL : 4;         ///<BIT [11:8] Delay Cycles After Read Command to Assert ODT.
        uint32_t ODT_WR_START_DLY_VAL : 4;         ///<BIT [15:12] Delay Cycles After Write Command to Assert ODT.
        uint32_t ODT_RD_DLY_SEL       : 2;         ///<BIT [17:16] Select Read ODT Internal Delay to PHY.
        uint32_t ODT_WR_DLY_SEL       : 2;         ///<BIT [19:18] Select Write ODT Internal Delay to PHY.
        uint32_t RF1000348_30_20_RSVD :11;         ///<BIT [30:20] Reserved.
        uint32_t ODT_OVERRIDE         : 1;         ///<BIT [31] Software Program Override Mode.
    } b;
} Chan0DramCtrl3_t;

///  @brief 0xF1000370 : Channel 0 Mode Read Data
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MRR_DATA            : 8;         ///<BIT [7:0] MRR Data.
        uint32_t RF1000370_30_8_RSVD :23;         ///<BIT [30:8] Reserved.
        uint32_t MRR_DATA_VALID      : 1;         ///<BIT [31] MRR Data Valid.
    } b;
} Chan0ModeRdData_t;

///  @brief 0xF1000374 : Channel 0 Multi-Purpose Read Data 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MPR_DATA0           : 8;         ///<BIT [7:0] MPR Data 0.
        uint32_t RF1000374_30_8_RSVD :23;         ///<BIT [30:8] Reserved.
        uint32_t MPR_DATA_VALID0     : 1;         ///<BIT [31] MPR Data Valid 0.
    } b;
} Chan0MultiPurposeRdData0_t;

///  @brief 0xF1000378 : Channel 0 Multi-Purpose Read Data 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MPR_DATA1           : 8;         ///<BIT [7:0] MPR Data 1.
        uint32_t RF1000378_30_8_RSVD :23;         ///<BIT [30:8] Reserved.
        uint32_t MPR_DATA_VALID1     : 1;         ///<BIT [31] MPR Data Valid 1.
    } b;
} Chan0MultiPurposeRdData1_t;

///  @brief 0xF1000380 : Channel 0 DDR Initialization Timing Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INIT_CNT_NOP         :20;         ///<BIT [19:0] Initialization Count NOP.
        uint32_t RF1000380_31_20_RSVD :12;         ///<BIT [31:20] Reserved.
    } b;
} Chan0DdrInitialiTmCtrl0_t;

///  @brief 0xF1000384 : Channel 0 DDR Initialization Timing Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INIT_CNT          :18;         ///<BIT [17:0] Initial Count.
        uint32_t RST_CNT           :13;         ///<BIT [30:18] Reset Count.
        uint32_t RF1000384_31_RSVD : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0DdrInitialiTmCtrl1_t;

///  @brief 0xF1000388 : Channel 0 ZQC 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_ZQINIT             :11;         ///<BIT [10:0] ZQ Power-up and Reset Calibration Time.
        uint32_t RF1000388_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
        uint32_t T_ZQCR               : 7;         ///<BIT [22:16] ZQ Calibration Reset Time.
        uint32_t RF1000388_31_23_RSVD : 9;         ///<BIT [31:23] Reserved.
    } b;
} Chan0Zqc0_t;

///  @brief 0xF100038C : Channel 0 ZQC 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_ZQOPER             :10;         ///<BIT [9:0] ZQ Normal Operation Full Calibration Time.
        uint32_t RF100038C_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t T_ZQCS               : 8;         ///<BIT [23:16] ZQ Normal Operation Short Calibration Time.
        uint32_t RF100038C_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;
} Chan0Zqc1_t;

///  @brief 0xF1000390 : Channel 0 Refresh Timing
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_REFI               :14;         ///<BIT [13:0] Auto-Refresh Interval Time.
        uint32_t RF1000390_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t T_RFC                :11;         ///<BIT [26:16] Refresh Time.
        uint32_t RF1000390_31_27_RSVD : 5;         ///<BIT [31:27] Reserved.
    } b;
} Chan0RefreshTm_t;

///  @brief 0xF1000394 : Channel 0 Self-Refresh Timing
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_XSRD            :10;         ///<BIT [9:0] Exit Self-refresh Locked DLL Timing.
        uint32_t RF1000394_10_RSVD : 1;         ///<BIT [10] Reserved.
        uint32_t T_CKSRX_H         : 1;         ///<BIT [11] T_CKSRX Bit Extension.
        uint32_t T_CKSRX           : 4;         ///<BIT [15:12] Stable TCK Cycles Before Exiting Self-refresh.
        uint32_t T_XSNR            :11;         ///<BIT [26:16] Exit Self-Refresh No DLL Timing.
        uint32_t T_CKSRE_H         : 1;         ///<BIT [27] T_CKSRE Bit Extension.
        uint32_t T_CKSRE           : 4;         ///<BIT [31:28] TCK Before Clock Change After Self-refresh.
    } b;
} Chan0SelfRefreshTm_t;

///  @brief 0xF1000398 : Channel 0 Power-Down Timing
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_XARDS              : 5;         ///<BIT [4:0] XARDS Timing. .
        uint32_t RF1000398_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t T_XP                 : 4;         ///<BIT [11:8] Exit Power-Down Timing.
        uint32_t RF1000398_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t T_CKESR              : 5;         ///<BIT [20:16] Self-refresh Timing.
        uint32_t RF1000398_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t T_CPDED              : 3;         ///<BIT [26:24] SDRAM Command Pass Disable Delay Timing.
        uint32_t RF1000398_27_RSVD    : 1;         ///<BIT [27] Reserved.
        uint32_t T_PDEN               : 3;         ///<BIT [30:28] Power-Down Entry Delay Timing.
        uint32_t RF1000398_31_RSVD    : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0PowerDnTm_t;

///  @brief 0xF100039C : Channel 0 Mode Register Set Timing
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_MRD               : 4;         ///<BIT [3:0] Mode Register Set Command Cycle Time.
        uint32_t T_MOD               : 5;         ///<BIT [8:4] Mode Timing.
        uint32_t RF100039C_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;
} Chan0ModeRegisterSetTm_t;

///  @brief 0xF10003A0 : Channel 0 Activate Timing
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_RAS                : 6;         ///<BIT [5:0] Active-to-Precharge Command Timing.
        uint32_t RF10003A0_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint32_t T_RCD                : 5;         ///<BIT [12:8] RAS-to-CAS Delay Timing.
        uint32_t RF10003A0_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t T_RC                 : 7;         ///<BIT [22:16] RC Timing.
        uint32_t RF10003A0_23_RSVD    : 1;         ///<BIT [23] Reserved.
        uint32_t T_FAW                : 6;         ///<BIT [29:24] FAW Timing.
        uint32_t RF10003A0_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;
} Chan0ActivateTm_t;

///  @brief 0xF10003A4 : Channel 0 Pre-charge Timing
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_RP                 : 5;         ///<BIT [4:0] PreCharge Command Period.
        uint32_t RF10003A4_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t T_RTP                : 4;         ///<BIT [11:8] Read-to-Precharge Timing.
        uint32_t T_WR                 : 5;         ///<BIT [16:12] Write Recovery Timing (Effective Value in DRAM Clock).
        uint32_t RF10003A4_19_17_RSVD : 3;         ///<BIT [19:17] Reserved.
        uint32_t T_RPA                : 5;         ///<BIT [24:20] PreCharge for All Command Period.
        uint32_t RF10003A4_31_25_RSVD : 7;         ///<BIT [31:25] Reserved.
    } b;
} Chan0PreChargeTm_t;

///  @brief 0xF10003A8 : Channel 0 CAS/RAS Timing
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_WTR_S             : 4;         ///<BIT [3:0] Write-to-Read Delay Timing (Different Group).
        uint32_t T_WTR               : 4;         ///<BIT [7:4] Write-to-Read Delay Timing (Same Group).
        uint32_t RF10003A8_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
        uint32_t T_CCD_S             : 3;         ///<BIT [18:16] CAS to CAS Command Delay (Different Group).
        uint32_t RF10003A8_19_RSVD   : 1;         ///<BIT [19] Reserved.
        uint32_t T_CCD               : 3;         ///<BIT [22:20] CAS to CAS Command Delay (Same Group).
        uint32_t RF10003A8_23_RSVD   : 1;         ///<BIT [23] Reserved.
        uint32_t T_RRD_S             : 4;         ///<BIT [27:24] Bank-to-Bank Command Period Timing (Different Group).
        uint32_t T_RRD               : 4;         ///<BIT [31:28] Bank-to-Bank Command Period Timing (Same Group).
    } b;
} Chan0CasRasTm_t;

///  @brief 0xF10003AC : Channel 0 Off-Spec Timing
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_CCD_CCS_EXT_DLY    : 4;         ///<BIT [3:0] Extra CAS-to-CAS Read Delay.
        uint32_t T_CCD_CCS_WR_EXT_DLY : 3;         ///<BIT [6:4] Extra CAS-to-CAS Write Delay.
        uint32_t RF10003AC_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint32_t TRWD_EXT_DLY         : 3;         ///<BIT [10:8] READ to WRITE Command Delay.
        uint32_t RF10003AC_11_RSVD    : 1;         ///<BIT [11] Reserved.
        uint32_t TWL_EARLY            : 2;         ///<BIT [13:12] Write Leveling Early.
        uint32_t RF10003AC_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t RD_GAP_EXT           : 4;         ///<BIT [19:16] Read Gap Extend.
        uint32_t RF10003AC_31_20_RSVD :12;         ///<BIT [31:20] Reserved.
    } b;
} Chan0OffSpecTm_t;

///  @brief 0xF10003B0 : Channel 0 DRAM Read Timing
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_DQSCK             : 3;         ///<BIT [2:0] tDQSCK.
        uint32_t RF10003B0_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;
} Chan0DramRdTm_t;

///  @brief 0xF10003B4 : Channel 0 DRAM Train Timing
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_CACKEL             : 4;         ///<BIT [3:0] tCACKEL.
        uint32_t RF10003B4_7_4_RSVD   : 4;         ///<BIT [7:4] Reserved.
        uint32_t T_CAEXT              : 4;         ///<BIT [11:8] tCAEXT.
        uint32_t RF10003B4_31_12_RSVD :20;         ///<BIT [31:12] Reserved.
    } b;
} Chan0DramTrainTm_t;

///  @brief 0xF10003B8 : Channel 0 DRAM CRC and Parity Timing
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_PAR_UNKN           : 4;         ///<BIT [3:0] tPAR_UNKNOWN.
        uint32_t T_PAR_ALRT_ON        : 5;         ///<BIT [8:4] tPAR_ALERT_ON.
        uint32_t T_PAR_ALRT_PW        : 7;         ///<BIT [15:9] tPAR_ALERT_PW.
        uint32_t T_CRC_ALERT          : 5;         ///<BIT [20:16] tCRC_ALERT.
        uint32_t RF10003B8_31_21_RSVD :11;         ///<BIT [31:21] Reserved.
    } b;
} Chan0DramCrcAndParTm_t;

///  @brief 0xF10003BC : Channel 0 DRAM Maximum Power Down Timing
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t T_XMP                :10;         ///<BIT [9:0] tXMP.
        uint32_t T_MPX_LH             : 4;         ///<BIT [13:10] tMPX_LH.
        uint32_t T_MPX_S              : 3;         ///<BIT [16:14] tMPX_S.
        uint32_t RF10003BC_31_17_RSVD :15;         ///<BIT [31:17] Reserved.
    } b;
} Chan0DramMaxPowerDnTm_t;

///  @brief 0xF1000400 : Channel 0 PHY Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DQ_EXT_DLY             : 4;         ///<BIT [3:0] DQ External Delay.
        uint32_t PHY_RFIFO_RPTR_DLY_VAL : 4;         ///<BIT [7:4] PHY Read FIFO Read Pointer Delay Value.
        uint32_t RD_EXT_DLY             : 3;         ///<BIT [10:8] Read Extra Delay.
        uint32_t RF1000400_11_RSVD      : 1;         ///<BIT [11] Reserved.
        uint32_t DQ_EN_EXTEND           : 1;         ///<BIT [12] DQ Output Enable Extend.
        uint32_t RF1000400_13_RSVD      : 1;         ///<BIT [13] Reserved.
        uint32_t PHY_RFIFO_RDRST_EN     : 1;         ///<BIT [14] PHY Read FIFO Reset Enable.
        uint32_t PHY_RFIFO_RDRST_EARLY  : 1;         ///<BIT [15] PHY Read FIFO Reset Early.
        uint32_t RF1000400_16_RSVD      : 1;         ///<BIT [16] Reserved.
        uint32_t PHY_RFIFO_RDRST_EXPAND : 2;         ///<BIT [18:17] PHY Read FIFO Reset Expand.
        uint32_t RF1000400_31_19_RSVD   :13;         ///<BIT [31:19] Reserved.
    } b;
} Chan0PhyCtrl1_t;

///  @brief 0xF1000404 : Channel 0 PHY Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_DQ_MD          : 1;         ///<BIT [0] PHY DQ Mode.
        uint32_t PHY_DQ_ZD          : 1;         ///<BIT [1] DQ/DQS Common Per-finger Drive Strength.
        uint32_t RF1000404_3_2_RSVD : 2;         ///<BIT [3:2] Reserved.
        uint32_t PHY_DQ_ZPR         : 4;         ///<BIT [7:4] DQ PMOS Driver Strength Fine Configuration.
        uint32_t PHY_DQ_ZNR         : 4;         ///<BIT [11:8] DQ NMOS Driver Strength Fine Configuration.
        uint32_t PHY_DQ_ZNTRM       : 4;         ///<BIT [15:12] DQ NMOS Termination Strength Configuration.
        uint32_t PHY_DQ_ZPTRM       : 4;         ///<BIT [19:16] DQ PMOS Termination Strength Configuration.
        uint32_t PHY_DQ_ZNDRV       : 4;         ///<BIT [23:20] DQ NMOS Driver Strength Coarse Configuration.
        uint32_t PHY_DQ_ZPDRV       : 4;         ///<BIT [27:24] DQ PMOS Driver Strength Coarse Configuration.
        uint32_t PHY_QS_VREF_SEL    : 2;         ///<BIT [29:28] DQS Reference Voltage Selector.
        uint32_t PHY_DQ_ZPR_MSB     : 1;         ///<BIT [30] DQ PMOS Driver Strength Fine Configuration MSB.
        uint32_t PHY_DQ_ZNR_MSB     : 1;         ///<BIT [31] DQ NMOS Driver Strength Fine Configuration MSB.
    } b;
} Chan0PhyCtrl2_t;

///  @brief 0xF1000408 : Channel 0 PHY Control 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_ADCM_MODE        : 1;         ///<BIT [0] PHY Address/Command Mode.
        uint32_t PHY_ADCM_ZD          : 1;         ///<BIT [1] Address/Command Common Per-finger Drive Strength.
        uint32_t RF1000408_3_2_RSVD   : 2;         ///<BIT [3:2] Reserved.
        uint32_t PHY_ADCM_ZPR         : 4;         ///<BIT [7:4] Address/Command PMOS Driver Strength Fine Configuration.
        uint32_t PHY_ADCM_ZNR         : 4;         ///<BIT [11:8] Address/Command NMOS Driver Strength Fine Configuration.
        uint32_t RF1000408_19_12_RSVD : 8;         ///<BIT [19:12] Reserved.
        uint32_t PHY_ADCM_ZNDRV       : 4;         ///<BIT [23:20] Address/Command NMOS Driver Strength Coarse Configuration.
        uint32_t PHY_ADCM_ZPDRV       : 4;         ///<BIT [27:24] Address/Command PMOS Driver Strength Coarse Configuration.
        uint32_t PHY_ADCM_RCVTYPE     : 2;         ///<BIT [29:28] Address/Command Receiver Type.
        uint32_t PHY_ADCM_ZPR_MSB     : 1;         ///<BIT [30] Address/Command PMOS Driver Strength Fine Configuration MSB.
        uint32_t PHY_ADCM_ZNR_MSB     : 1;         ///<BIT [31] Address/Command NMOS Driver Strength Fine Configuration MSB.
    } b;
} Chan0PhyCtrl3_t;

///  @brief 0xF100040C : Channel 0 PHY Control 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_CK_ZPR            : 4;         ///<BIT [3:0] Clock PMOS Driver Strength Fine Configuration.
        uint32_t PHY_CK_ZNR            : 4;         ///<BIT [7:4] Clock NMOS Driver Strength Fine Configuration.
        uint32_t PHY_CK_ZNR_MSB        : 1;         ///<BIT [8] Clock NMOS Driver Strength Fine Configuration MSB.
        uint32_t PHY_CK_ZPR_MSB        : 1;         ///<BIT [9] Clock PMOS Driver Strength Fine Configuration MSB.
        uint32_t PHY_CK_ZPDRV          : 4;         ///<BIT [13:10] Clock PMOS Driver Strength Coarse Configuration.
        uint32_t PHY_CK_ZNDRV          : 4;         ///<BIT [17:14] Clock NMOS Driver Strength Coarse Configuration.
        uint32_t RF100040C_18_RSVD     : 1;         ///<BIT [18] Reserved.
        uint32_t PHY_DQ_RCV_DIS_LATE   : 1;         ///<BIT [19] PHY DQ Receiver Disable Late.
        uint32_t PHY_USE_REGISTER_TEST : 1;         ///<BIT [20] PHY Use Register Test.
        uint32_t PHY_CK_ZD             : 1;         ///<BIT [21] Clock Driver Strength Fine Configuration Enable.
        uint32_t PHY_DQ_RCVTYPE        : 2;         ///<BIT [23:22] PHY DQ Receiver Type.
        uint32_t PHY_DQ_RCVEN          : 3;         ///<BIT [26:24] Data Receiver Gain Settings for N Pair.
        uint32_t PHY_DQ_RCVEP          : 3;         ///<BIT [29:27] Data Receiver Gain Settings for P Pair.
        uint32_t PHY_QS_RCVTYPE        : 2;         ///<BIT [31:30] PHY QS Receiver Type.
    } b;
} Chan0PhyCtrl4_t;

///  @brief 0xF1000410 : Channel 0 PHY Control 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MC_AC_PD             : 1;         ///<BIT [0] Memory Controller Address/Command Pulldown.
        uint32_t MC_CK_PD             : 1;         ///<BIT [1] Memory Controller Clock Pulldown.
        uint32_t MC_DQ_PD             : 1;         ///<BIT [2] Memory Controller DQ Pulldown.
        uint32_t MC_QSP_PD            : 1;         ///<BIT [3] Memory Controller QSP Pulldown.
        uint32_t MC_QSN_PD            : 1;         ///<BIT [4] Memory Controller QSN Pullup.
        uint32_t RF1000410_5_RSVD     : 1;         ///<BIT [5] Reserved.
        uint32_t ENABLE_ALL_BYTE_OE   : 1;         ///<BIT [6] Enable All Byte Output Enable.
        uint32_t EN_ALL_BYTE_PD       : 1;         ///<BIT [7] Enable All Byte Pulldowns.
        uint32_t WR_DQSB_EN           : 1;         ///<BIT [8] PHY Write DQSB Enable.
        uint32_t RF1000410_9_RSVD     : 1;         ///<BIT [9] Reserved.
        uint32_t WR_DQSB_ONE          : 1;         ///<BIT [10] PHY Write DQSB One.
        uint32_t RF1000410_14_11_RSVD : 4;         ///<BIT [14:11] Reserved.
        uint32_t PAD_CAL_ONCE         : 1;         ///<BIT [15] Pad Calibrate Once.
        uint32_t PAD_CAL_AUTO         : 1;         ///<BIT [16] Pad Auto-calibration.
        uint32_t PAD_CAL_AUTO_SEL     : 3;         ///<BIT [19:17] Pad Auto-calibration Select.
        uint32_t PAD_CAL_INTVAL       : 2;         ///<BIT [21:20] Pad Calibration Interval.
        uint32_t RF1000410_31_22_RSVD :10;         ///<BIT [31:22] Reserved.
    } b;
} Chan0PhyCtrl5_t;

///  @brief 0xF1000414 : Channel 0 PHY Control 6
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF1000414_2_0_RSVD    : 3;         ///<BIT [2:0] Reserved.
        uint32_t MC_PHY_SYNC_MODE      : 1;         ///<BIT [3] Memory Controller/PHY Sync Mode.
        uint32_t RF1000414_8_4_RSVD    : 5;         ///<BIT [8:4] Reserved.
        uint32_t MC_PHY_WFIFO_LD_PHASE : 1;         ///<BIT [9] Memory Controller/PHY Write FIFO LD Phase.
        uint32_t RF1000414_18_10_RSVD  : 9;         ///<BIT [18:10] Reserved.
        uint32_t MC_PHY_SYNC_EN_SEL    : 1;         ///<BIT [19] Memory Controller/PHY Clock Sync En Selection.
        uint32_t RF1000414_31_20_RSVD  :12;         ///<BIT [31:20] Reserved.
    } b;
} Chan0PhyCtrl6_t;

///  @brief 0xF100041C : Channel 0 PHY Control 8
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DLL_UPD_BY_REF_EN    : 1;         ///<BIT [0] PHY DLL Auto Update by Auto-refresh Enable.
        uint32_t RF100041C_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint32_t DLL_AUTO_UPD_EN      : 1;         ///<BIT [2] PHY DLL Auto Update Enable.
        uint32_t DLL_AUTO_MAN_UPD     : 1;         ///<BIT [3] PHY DLL Auto or Manual Update.
        uint32_t DLL_FAST_LCK         : 1;         ///<BIT [4] PHY DLL Fast Lock.
        uint32_t RF100041C_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t DLL_AUTO_UPD_INTVAL  : 8;         ///<BIT [15:8] PHY DLL Auto-update Interval.
        uint32_t DLL_UPD_TMR          : 5;         ///<BIT [20:16] PHY DLL Update Timer.
        uint32_t RF100041C_23_21_RSVD : 3;         ///<BIT [23:21] Reserved.
        uint32_t DLL_RST_TMR          : 8;         ///<BIT [31:24] PHY DLL Reset Timer.
    } b;
} Chan0PhyCtrl8_t;

///  @brief 0xF1000420 : Channel 0 PHY Control 9
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF1000420_1_0_RSVD   : 2;         ///<BIT [1:0] Reserved.
        uint32_t PHY_CAL_DONE         : 1;         ///<BIT [2] PHY Pad Calibration Done.
        uint32_t PHY_CAL_ERR          : 1;         ///<BIT [3] PHY Pad Calibration Error.
        uint32_t RF1000420_14_4_RSVD  :11;         ///<BIT [14:4] Reserved.
        uint32_t PHY_CAL_ZNR          : 5;         ///<BIT [19:15] PHY Calibration NMOS Fine Configuration.
        uint32_t PHY_CAL_ZPR          : 5;         ///<BIT [24:20] PHY Calibration PMOS Fine Configuration.
        uint32_t RF1000420_27_25_RSVD : 3;         ///<BIT [27:25] Reserved.
        uint32_t PHY_PLL_RST          : 1;         ///<BIT [28] PHY PLL Reset.
        uint32_t PHY_DLL_RST          : 1;         ///<BIT [29] PHY DLL Reset.
        uint32_t DLL_UPD_EN_PULSE     : 1;         ///<BIT [30] PHY DLL Update Enable Pulse.
        uint32_t PHY_SYNC_EN          : 1;         ///<BIT [31] PHY Synchronize Enable.
    } b;
} Chan0PhyCtrl9_t;

///  @brief 0xF1000424 : Channel 0 PHY Control 10
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_DQ_OUT       : 9;         ///<BIT [8:0] PHY Write-Leveling DQ Out.
        uint32_t RF1000424_30_9_RSVD :22;         ///<BIT [30:9] Reserved.
        uint32_t PHY_WL_DQS_PULSE    : 1;         ///<BIT [31] PHY Write-Leveling DQS Pulse.
    } b;
} Chan0PhyCtrl10_t;

///  @brief 0xF1000428 : Channel 0 PHY Control 11
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_RD_GAP_RST_EN    : 1;         ///<BIT [0] PHY Read Gap Reset Enable.
        uint32_t RF1000428_3_1_RSVD   : 3;         ///<BIT [3:1] Reserved.
        uint32_t PHY_RD_GAP_RST_CYCLE : 3;         ///<BIT [6:4] PHY Read Gap Reset Cycle.
        uint32_t RF1000428_31_7_RSVD  :25;         ///<BIT [31:7] Reserved.
    } b;
} Chan0PhyCtrl11_t;

///  @brief 0xF100042C : Channel 0 PHY Control 12
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RESERVE_AC0 : 8;         ///<BIT [7:0] Reserved Pins for ADCM Sub-PHY 0.
        uint32_t RESERVE_AC1 : 8;         ///<BIT [15:8] Reserved Pins for ADCM Sub-PHY 1.
        uint32_t RESERVE_AC2 : 8;         ///<BIT [23:16] Reserved Pins for ADCM Sub-PHY 2.
        uint32_t RESERVE_CK  : 8;         ///<BIT [31:24] Reserved Pins for Clock.
    } b;
} Chan0PhyCtrl12_t;

///  @brief 0xF1000430 : Channel 0 PHY Control 13
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_AD_PD            :18;         ///<BIT [17:0] PHY Address Power Down.
        uint32_t RF1000430_19_18_RSVD : 2;         ///<BIT [19:18] Reserved.
        uint32_t PHY_BA_PD            : 3;         ///<BIT [22:20] PHY Bank Address Power Down.
        uint32_t PHY_SA_PD            : 3;         ///<BIT [25:23] PHY Stack Address Power Down.
        uint32_t RF1000430_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;
} Chan0PhyCtrl13_t;

///  @brief 0xF1000434 : Channel 0 PHY Control 14
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_CK_PD            : 4;         ///<BIT [3:0] PHY CK Pad Power Down.
        uint32_t PHY_CSN_PD           : 4;         ///<BIT [7:4] PHY CSn Pad Power Down.
        uint32_t PHY_CKE_PD           : 4;         ///<BIT [11:8] PHY CKE Pad Power Down.
        uint32_t PHY_ODT_PD           : 4;         ///<BIT [15:12] PHY ODT Pad Power Down.
        uint32_t PHY_RASN_PD          : 1;         ///<BIT [16] PHY RASn Pad Power Down.
        uint32_t PHY_CASN_PD          : 1;         ///<BIT [17] PHY CASn Pad Power Down.
        uint32_t PHY_WEN_PD           : 1;         ///<BIT [18] PHY WEn Pad Power Down.
        uint32_t PHY_RSTN_PD          : 1;         ///<BIT [19] PHY RESETn Pad Power Down.
        uint32_t PHY_PAR_PD           : 1;         ///<BIT [20] PHY Parity Pad Power Down.
        uint32_t PHY_ALERTN_PD        : 1;         ///<BIT [21] PHY Alertn Pad Power Down.
        uint32_t RF1000434_31_22_RSVD :10;         ///<BIT [31:22] Reserved.
    } b;
} Chan0PhyCtrl14_t;

///  @brief 0xF1000438 : Channel 0 PHY Control 15
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WRST_SEL         : 2;         ///<BIT [1:0] PHY Write Reset Select.
        uint32_t RF1000438_3_2_RSVD   : 2;         ///<BIT [3:2] Reserved.
        uint32_t PHY_REG_PD           : 1;         ///<BIT [4] Charge-Pump Power Down.
        uint32_t PHY_REG_SLP          : 1;         ///<BIT [5] Charge-Pump Sleep.
        uint32_t PHY_VREFINT_EN       : 1;         ///<BIT [6] VREF enable. Use the external VREF.
        uint32_t PHY_CP_TEST_EN       : 1;         ///<BIT [7] PHY Charge-Pump Test Enable.
        uint32_t PHY_CP_REF_SEL       : 4;         ///<BIT [11:8] PHY Charge-Pump VREF Select.
        uint32_t RF1000438_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t PHY_PU               : 1;         ///<BIT [16] VDAC Power Up.
        uint32_t PHY_REFBUF_BYP       : 1;         ///<BIT [17] PHY Refresh Buffer Bypass.
        uint32_t PHY_REFBUF_TRI       : 1;         ///<BIT [18] PHY Refresh Buffer TRI.
        uint32_t RF1000438_19_RSVD    : 1;         ///<BIT [19] Reserved.
        uint32_t PHY_TSTPT_SEL        : 2;         ///<BIT [21:20] Test Point Select.
        uint32_t RF1000438_23_22_RSVD : 2;         ///<BIT [23:22] Reserved.
        uint32_t PHY_VREF_CTRL        : 6;         ///<BIT [29:24] PHY VREF Control.
        uint32_t RF1000438_30_RSVD    : 1;         ///<BIT [30] Reserved.
        uint32_t PHY_VREF_RNGE        : 1;         ///<BIT [31] PHY VREF Range.
    } b;
} Chan0PhyCtrl15_t;

///  @brief 0xF100043C : Channel 0 PHY Control 16
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WCK_EN           : 1;         ///<BIT [0] PHY WCK Enable.
        uint32_t RF100043C_3_1_RSVD   : 3;         ///<BIT [3:1] Reserved.
        uint32_t PHY_WCKG_DLY         : 4;         ///<BIT [7:4] PHY WCKG Delay.
        uint32_t RF100043C_11_8_RSVD  : 4;         ///<BIT [11:8] PHY DMSYNC Reserved.
        uint32_t RF100043C_31_12_RSVD :20;         ///<BIT [31:12] Reserved.
    } b;
} Chan0PhyCtrl16_t;

///  @brief 0xF1000440 : Channel 0 PHY Control 17
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VREG_VSEL            : 3;         ///<BIT [2:0] VREG Voltage Select.
        uint32_t RF1000440_3_RSVD     : 1;         ///<BIT [3] Reserved.
        uint32_t VREG_TESTMON         : 3;         ///<BIT [6:4] VREG Test Monitor.
        uint32_t RF1000440_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint32_t VREG_TSTMON_EN       : 1;         ///<BIT [8] VREG Test Monitor Enable.
        uint32_t VREG_LP              : 1;         ///<BIT [9] VREG LP.
        uint32_t VREG_PU              : 1;         ///<BIT [10] VREG Power Up.
        uint32_t VREG_BYPASS          : 1;         ///<BIT [11] VREG Bypass.
        uint32_t RF1000440_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t PHY_ALERTN_ZPTRM     : 4;         ///<BIT [19:16] PHY Alertn PAD PMOS Termination Strength Configuration.
        uint32_t PHY_ALERTN_ZNTRM     : 4;         ///<BIT [23:20] PHY Alertn PAD NMOS Termination Strength Configuration.
        uint32_t PHY_ALERTN_PU        : 1;         ///<BIT [24] PHY Alertn PAD Pull Up Enable.
        uint32_t PHY_ALERTN_PD_EN     : 1;         ///<BIT [25] PHY Alertn PAD Pull Down Enable.
        uint32_t PHY_ALERTN_RX_EN     : 1;         ///<BIT [26] PHY Alertn PAD Receiver Enable.
        uint32_t RF1000440_31_27_RSVD : 5;         ///<BIT [31:27] Reserved.
    } b;
} Chan0PhyCtrl17_t;

///  @brief 0xF1000500 : Channel 0 PHY DLL Control B0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DLL_BYPASS_EN_B0     : 1;         ///<BIT [0] DLL Bypass Enable B0.
        uint32_t DLL_TST_EN_B0        : 1;         ///<BIT [1] DLL Test Mode Enable B0.
        uint32_t RF1000500_2_RSVD     : 1;         ///<BIT [2] Reserved.
        uint32_t DLL_GAIN2X_B0        : 1;         ///<BIT [3] DLL Gain2x B0.
        uint32_t DLL_DLY_TEST_B0      :10;         ///<BIT [13:4] DLL Delay Test B0.
        uint32_t RF1000500_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t DLL_PHSEL_B0         : 6;         ///<BIT [21:16] DLL Phase Select B0.
        uint32_t RF1000500_23_22_RSVD : 2;         ///<BIT [23:22] Reserved.
        uint32_t DLL_PHSEL1_B0        : 6;         ///<BIT [29:24] DLL Phase Select 1 B0.
        uint32_t RF1000500_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;
} Chan0PhyDllCtrlB0_t;

///  @brief 0xF1000504 : Channel 0 PHY DLL Control B1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DLL_BYPASS_EN_B1     : 1;         ///<BIT [0] DLL Bypass Enable B1.
        uint32_t DLL_TST_EN_B1        : 1;         ///<BIT [1] DLL Test Mode Enable B1.
        uint32_t RF1000504_2_RSVD     : 1;         ///<BIT [2] Reserved.
        uint32_t DLL_GAIN2X_B1        : 1;         ///<BIT [3] DLL Gain2x B1.
        uint32_t DLL_DLY_TEST_B1      :10;         ///<BIT [13:4] DLL Delay Test B1.
        uint32_t RF1000504_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t DLL_PHSEL_B1         : 6;         ///<BIT [21:16] DLL Phase Select B1.
        uint32_t RF1000504_23_22_RSVD : 2;         ///<BIT [23:22] Reserved.
        uint32_t DLL_PHSEL1_B1        : 6;         ///<BIT [29:24] DLL Phase Select 1 B1.
        uint32_t RF1000504_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;
} Chan0PhyDllCtrlB1_t;

///  @brief 0xF1000508 : Channel 0 PHY DLL Control B2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DLL_BYPASS_EN_B2     : 1;         ///<BIT [0] DLL Bypass Enable B2.
        uint32_t DLL_TST_EN_B2        : 1;         ///<BIT [1] DLL Test Mode Enable B2.
        uint32_t RF1000508_2_RSVD     : 1;         ///<BIT [2] Reserved.
        uint32_t DLL_GAIN2X_B2        : 1;         ///<BIT [3] DLL Gain2x B2.
        uint32_t DLL_DLY_TEST_B2      :10;         ///<BIT [13:4] DLL Delay Test B2.
        uint32_t RF1000508_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t DLL_PHSEL_B2         : 6;         ///<BIT [21:16] DLL Phase Select B2.
        uint32_t RF1000508_23_22_RSVD : 2;         ///<BIT [23:22] Reserved.
        uint32_t DLL_PHSEL1_B2        : 6;         ///<BIT [29:24] DLL Phase Select 1 B2.
        uint32_t RF1000508_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;
} Chan0PhyDllCtrlB2_t;

///  @brief 0xF100050C : Channel 0 PHY DLL Control B3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DLL_BYPASS_EN_B3     : 1;         ///<BIT [0] DLL Bypass Enable B3.
        uint32_t DLL_TST_EN_B3        : 1;         ///<BIT [1] DLL Test Mode Enable B3.
        uint32_t RF100050C_2_RSVD     : 1;         ///<BIT [2] Reserved.
        uint32_t DLL_GAIN2X_B3        : 1;         ///<BIT [3] DLL Gain2x B3.
        uint32_t DLL_DLY_TEST_B3      :10;         ///<BIT [13:4] DLL Delay Test B3.
        uint32_t RF100050C_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t DLL_PHSEL_B3         : 6;         ///<BIT [21:16] DLL Phase Select B3.
        uint32_t RF100050C_23_22_RSVD : 2;         ///<BIT [23:22] Reserved.
        uint32_t DLL_PHSEL1_B3        : 6;         ///<BIT [29:24] DLL Phase Select 1 B3.
        uint32_t RF100050C_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;
} Chan0PhyDllCtrlB3_t;

///  @brief 0xF1000500 : Channel 0 PHY DLL Control B0
///  @brief 0xF100050C : Channel 0 PHY DLL Control B3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DLL_BYPASS_EN_Bx     : 1;         ///<BIT [0] DLL Bypass Enable Bx.
        uint32_t DLL_TST_EN_Bx        : 1;         ///<BIT [1] DLL Test Mode Enable Bx.
        uint32_t RF100050x_2_RSVD     : 1;         ///<BIT [2] Reserved.
        uint32_t DLL_GAIN2X_Bx        : 1;         ///<BIT [3] DLL Gain2x Bx.
        uint32_t DLL_DLY_TEST_Bx      :10;         ///<BIT [13:4] DLL Delay Test Bx.
        uint32_t RF100050x_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t DLL_PHSEL_Bx         : 6;         ///<BIT [21:16] DLL Phase Select Bx.
        uint32_t RF100050x_23_22_RSVD : 2;         ///<BIT [23:22] Reserved.
        uint32_t DLL_PHSEL1_Bx        : 6;         ///<BIT [29:24] DLL Phase Select 1 Bx.
        uint32_t RF100050x_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;
} Chan0PhyDllCtrlBx_t;

///  @brief 0xF1000528 : Channel 0 PHY DLL Byte Select
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DLL_BYTE_SEL        : 4;         ///<BIT [3:0] DLL Byte Select.
        uint32_t RF1000528_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;
} Chan0PhyDllByteSel_t;

///  @brief 0xF100052C : Channel 0 PHY DLL Delay
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DLL_MSTR_DLY_OUT     :10;         ///<BIT [9:0] Master DLL Delay Value.
        uint32_t RF100052C_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t DLL_SLV_DLY_OUT      :10;         ///<BIT [25:16] Slave DLL Delay Value.
        uint32_t RF100052C_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;
} Chan0PhyDllDly_t;

///  @brief 0xF1000540 : Channel 0 PHY Data Byte Control B0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF1000540_7_0_RSVD   : 8;         ///<BIT [7:0] PHY DQ Reserved B0.
        uint32_t PHY_DQS_PD_B0        : 1;         ///<BIT [8] PHY DQS Power Down B0.
        uint32_t PHY_DM_PD_B0         : 1;         ///<BIT [9] PHY DM Power Down B0.
        uint32_t PHY_DQ_PD_B0         : 1;         ///<BIT [10] PHY DQ Power Down B0.
        uint32_t PHY_DQS_DIS_TRM_B0   : 1;         ///<BIT [11] PHY DQS Disable Termination B0.
        uint32_t PHY_DM_DIS_TRM_B0    : 1;         ///<BIT [12] PHY DM Disable Termination B0.
        uint32_t PHY_DQ_DIS_TRM_B0    : 1;         ///<BIT [13] PHY DQ Disable Termination B0.
        uint32_t RF1000540_31_14_RSVD :18;         ///<BIT [31:14] Reserved.
    } b;
} Chan0PhyDataByteCtrlB0_t;

///  @brief 0xF1000544 : Channel 0 PHY Data Byte Control B1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF1000544_7_0_RSVD   : 8;         ///<BIT [7:0] PHY DQ Reserved B1.
        uint32_t PHY_DQS_PD_B1        : 1;         ///<BIT [8] PHY DQS Power Down B1.
        uint32_t PHY_DM_PD_B1         : 1;         ///<BIT [9] PHY DM Power Down B1.
        uint32_t PHY_DQ_PD_B1         : 1;         ///<BIT [10] PHY DQ Power Down B1.
        uint32_t PHY_DQS_DIS_TRM_B1   : 1;         ///<BIT [11] PHY DQS Disable Termination B1.
        uint32_t PHY_DM_DIS_TRM_B1    : 1;         ///<BIT [12] PHY DM Disable Termination B1.
        uint32_t PHY_DQ_DIS_TRM_B1    : 1;         ///<BIT [13] PHY DQ Disable Termination B1.
        uint32_t RF1000544_31_14_RSVD :18;         ///<BIT [31:14] Reserved.
    } b;
} Chan0PhyDataByteCtrlB1_t;

///  @brief 0xF1000548 : Channel 0 PHY Data Byte Control B2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF1000548_7_0_RSVD   : 8;         ///<BIT [7:0] PHY DQ Reserved B2.
        uint32_t PHY_DQS_PD_B2        : 1;         ///<BIT [8] PHY DQS Power Down B2.
        uint32_t PHY_DM_PD_B2         : 1;         ///<BIT [9] PHY DM Power Down B2.
        uint32_t PHY_DQ_PD_B2         : 1;         ///<BIT [10] PHY DQ Power Down B2.
        uint32_t PHY_DQS_DIS_TRM_B2   : 1;         ///<BIT [11] PHY DQS Disable Termination B2.
        uint32_t PHY_DM_DIS_TRM_B2    : 1;         ///<BIT [12] PHY DM Disable Termination B2.
        uint32_t PHY_DQ_DIS_TRM_B2    : 1;         ///<BIT [13] PHY DQ Disable Termination B2.
        uint32_t RF1000548_31_14_RSVD :18;         ///<BIT [31:14] Reserved.
    } b;
} Chan0PhyDataByteCtrlB2_t;

///  @brief 0xF100054C : Channel 0 PHY Data Byte Control B3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF100054C_7_0_RSVD   : 8;         ///<BIT [7:0] PHY DQ Reserved B3.
        uint32_t PHY_DQS_PD_B3        : 1;         ///<BIT [8] PHY DQS Power Down B3.
        uint32_t PHY_DM_PD_B3         : 1;         ///<BIT [9] PHY DM Power Down B3.
        uint32_t PHY_DQ_PD_B3         : 1;         ///<BIT [10] PHY DQ Power Down B3.
        uint32_t PHY_DQS_DIS_TRM_B3   : 1;         ///<BIT [11] PHY DQS Disable Termination B3.
        uint32_t PHY_DM_DIS_TRM_B3    : 1;         ///<BIT [12] PHY DM Disable Termination B3.
        uint32_t PHY_DQ_DIS_TRM_B3    : 1;         ///<BIT [13] PHY DQ Disable Termination B3.
        uint32_t RF100054C_31_14_RSVD :18;         ///<BIT [31:14] Reserved.
    } b;
} Chan0PhyDataByteCtrlB3_t;

///  @brief 0xF1000570 : Channel 0 PHY Write and Read Leveling Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_RL_EN            : 2;         ///<BIT [1:0] PHY Read Leveling Enable.
        uint32_t RF1000570_3_2_RSVD   : 2;         ///<BIT [3:2] Reserved.
        uint32_t PHY_WL_DQS_RX_EN_DIS : 1;         ///<BIT [4] PHY Write Leveling DQS Receiver Enable or Disable.
        uint32_t RF1000570_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t PHY_WL_CS_SEL        : 4;         ///<BIT [11:8] PHY Write-Leveling Chip-Select.
        uint32_t RF1000570_27_12_RSVD :16;         ///<BIT [27:12] Reserved.
        uint32_t DQS_PREAMBLE_H       : 1;         ///<BIT [28] DQS Preamble High.
        uint32_t DQS_POSTAMBLE_H      : 1;         ///<BIT [29] DQS Postamble High.
        uint32_t PHY_DQS_TERM_ON      : 1;         ///<BIT [30] PHY DQS Termination On.
        uint32_t PHY_RFIFO_SPLIT_EN   : 1;         ///<BIT [31] PHY Read FIFO Split Enable.
    } b;
} Chan0PhyWrAndRdLvlCtrl_t;

///  @brief 0xF1000580 : Channel 0 PHY Write Leveling Control Data CS0 B0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_DQ_DLY_CS0_B0  : 7;         ///<BIT [6:0] PHY Write Leveling WCK DQ Delay CS0 B0.
        uint32_t RF1000580_7_RSVD          : 1;         ///<BIT [7] Reserved.
        uint32_t PHY_WL_WCK_QS_DLY_CS0_B0  : 7;         ///<BIT [14:8] PHY Write Leveling WCK QS Delay CS0 B0.
        uint32_t RF1000580_15_RSVD         : 1;         ///<BIT [15] Reserved.
        uint32_t PHY_WL_WCK_DQC_DLY_CS0_B0 : 7;         ///<BIT [22:16] PHY Write Leveling WCK DQC Delay CS0 B0.
        uint32_t RF1000580_23_RSVD         : 1;         ///<BIT [23] Reserved.
        uint32_t PHY_WL_WCK_QSC_DLY_CS0_B0 : 7;         ///<BIT [30:24] PHY Write Leveling WCK QSC Delay CS0 B0.
        uint32_t RF1000580_31_RSVD         : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0PhyWrLvlCtrlDataCs000_t;

///  @brief 0xF1000584 : Channel 0 PHY Write Leveling Control Data CS0 B1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_DQ_DLY_CS0_B1  : 7;         ///<BIT [6:0] PHY Write Leveling WCK DQ Delay CS0 B1.
        uint32_t RF1000584_7_RSVD          : 1;         ///<BIT [7] Reserved.
        uint32_t PHY_WL_WCK_QS_DLY_CS0_B1  : 7;         ///<BIT [14:8] PHY Write Leveling WCK QS Delay CS0 B1.
        uint32_t RF1000584_15_RSVD         : 1;         ///<BIT [15] Reserved.
        uint32_t PHY_WL_WCK_DQC_DLY_CS0_B1 : 7;         ///<BIT [22:16] PHY Write Leveling WCK DQC Delay CS0 B1.
        uint32_t RF1000584_23_RSVD         : 1;         ///<BIT [23] Reserved.
        uint32_t PHY_WL_WCK_QSC_DLY_CS0_B1 : 7;         ///<BIT [30:24] PHY Write Leveling WCK QSC Delay CS0 B1.
        uint32_t RF1000584_31_RSVD         : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0PhyWrLvlCtrlDataCs011_t;

///  @brief 0xF1000588 : Channel 0 PHY Write Leveling Control Data CS0 B2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_DQ_DLY_CS0_B2  : 7;         ///<BIT [6:0] PHY Write Leveling WCK DQ Delay CS0 B2.
        uint32_t RF1000588_7_RSVD          : 1;         ///<BIT [7] Reserved.
        uint32_t PHY_WL_WCK_QS_DLY_CS0_B2  : 7;         ///<BIT [14:8] PHY Write Leveling WCK QS Delay CS0 B2.
        uint32_t RF1000588_15_RSVD         : 1;         ///<BIT [15] Reserved.
        uint32_t PHY_WL_WCK_DQC_DLY_CS0_B2 : 7;         ///<BIT [22:16] PHY Write Leveling WCK DQC Delay CS0 B2.
        uint32_t RF1000588_23_RSVD         : 1;         ///<BIT [23] Reserved.
        uint32_t PHY_WL_WCK_QSC_DLY_CS0_B2 : 7;         ///<BIT [30:24] PHY Write Leveling WCK QSC Delay CS0 B2.
        uint32_t RF1000588_31_RSVD         : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0PhyWrLvlCtrlDataCs022_t;

///  @brief 0xF100058C : Channel 0 PHY Write Leveling Control Data CS0 B3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_DQ_DLY_CS0_B3  : 7;         ///<BIT [6:0] PHY Write Leveling WCK DQ Delay CS0 B3.
        uint32_t RF100058C_7_RSVD          : 1;         ///<BIT [7] Reserved.
        uint32_t PHY_WL_WCK_QS_DLY_CS0_B3  : 7;         ///<BIT [14:8] PHY Write Leveling WCK QS Delay CS0 B3.
        uint32_t RF100058C_15_RSVD         : 1;         ///<BIT [15] Reserved.
        uint32_t PHY_WL_WCK_DQC_DLY_CS0_B3 : 7;         ///<BIT [22:16] PHY Write Leveling WCK DQC Delay CS0 B3.
        uint32_t RF100058C_23_RSVD         : 1;         ///<BIT [23] Reserved.
        uint32_t PHY_WL_WCK_QSC_DLY_CS0_B3 : 7;         ///<BIT [30:24] PHY Write Leveling WCK QSC Delay CS0 B3.
        uint32_t RF100058C_31_RSVD         : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0PhyWrLvlCtrlDataCs033_t;

///  @brief 0xF1000580 : Channel 0 PHY Write Leveling Control Data CS0 B0
///  @brief 0xF100058C : Channel 0 PHY Write Leveling Control Data CS0 B3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_DQ_DLY_CS0_Bx  : 7;         ///<BIT [6:0] PHY Write Leveling WCK DQ Delay CS0 Bx.
        uint32_t RF100058x_7_RSVD          : 1;         ///<BIT [7] Reserved.
        uint32_t PHY_WL_WCK_QS_DLY_CS0_Bx  : 7;         ///<BIT [14:8] PHY Write Leveling WCK QS Delay CS0 Bx.
        uint32_t RF100058x_15_RSVD         : 1;         ///<BIT [15] Reserved.
        uint32_t PHY_WL_WCK_DQC_DLY_CS0_Bx : 7;         ///<BIT [22:16] PHY Write Leveling WCK DQC Delay CS0 Bx.
        uint32_t RF100058x_23_RSVD         : 1;         ///<BIT [23] Reserved.
        uint32_t PHY_WL_WCK_QSC_DLY_CS0_Bx : 7;         ///<BIT [30:24] PHY Write Leveling WCK QSC Delay CS0 Bx.
        uint32_t RF100058x_31_RSVD         : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0PhyWrLvlCtrlDataCs0xx_t;

///  @brief 0xF10005A0 : Channel 0 PHY Write Leveling Control Data CS1 B0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_DQ_DLY_CS1_B0  : 7;         ///<BIT [6:0] PHY Write Leveling WCK DQ Delay CS1 B0.
        uint32_t RF10005A0_7_RSVD          : 1;         ///<BIT [7] Reserved.
        uint32_t PHY_WL_WCK_QS_DLY_CS1_B0  : 7;         ///<BIT [14:8] PHY Write Leveling WCK QS Delay CS1 B0.
        uint32_t RF10005A0_15_RSVD         : 1;         ///<BIT [15] Reserved.
        uint32_t PHY_WL_WCK_DQC_DLY_CS1_B0 : 7;         ///<BIT [22:16] PHY Write Leveling WCK DQC Delay CS1 B0.
        uint32_t RF10005A0_23_RSVD         : 1;         ///<BIT [23] Reserved.
        uint32_t PHY_WL_WCK_QSC_DLY_CS1_B0 : 7;         ///<BIT [30:24] PHY Write Leveling WCK QSC Delay CS1 B0.
        uint32_t RF10005A0_31_RSVD         : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0PhyWrLvlCtrlDataCs100_t;

///  @brief 0xF10005A4 : Channel 0 PHY Write Leveling Control Data CS1 B1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_DQ_DLY_CS1_B1  : 7;         ///<BIT [6:0] PHY Write Leveling WCK DQ Delay CS1 B1.
        uint32_t RF10005A4_7_RSVD          : 1;         ///<BIT [7] Reserved.
        uint32_t PHY_WL_WCK_QS_DLY_CS1_B1  : 7;         ///<BIT [14:8] PHY Write Leveling WCK QS Delay CS1 B1.
        uint32_t RF10005A4_15_RSVD         : 1;         ///<BIT [15] Reserved.
        uint32_t PHY_WL_WCK_DQC_DLY_CS1_B1 : 7;         ///<BIT [22:16] PHY Write Leveling WCK DQC Delay CS1 B1.
        uint32_t RF10005A4_23_RSVD         : 1;         ///<BIT [23] Reserved.
        uint32_t PHY_WL_WCK_QSC_DLY_CS1_B1 : 7;         ///<BIT [30:24] PHY Write Leveling WCK QSC Delay CS1 B1.
        uint32_t RF10005A4_31_RSVD         : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0PhyWrLvlCtrlDataCs111_t;

///  @brief 0xF10005A8 : Channel 0 PHY Write Leveling Control Data CS1 B2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_DQ_DLY_CS1_B2  : 7;         ///<BIT [6:0] PHY Write Leveling WCK DQ Delay CS1 B2.
        uint32_t RF10005A8_7_RSVD          : 1;         ///<BIT [7] Reserved.
        uint32_t PHY_WL_WCK_QS_DLY_CS1_B2  : 7;         ///<BIT [14:8] PHY Write Leveling WCK QS Delay CS1 B2.
        uint32_t RF10005A8_15_RSVD         : 1;         ///<BIT [15] Reserved.
        uint32_t PHY_WL_WCK_DQC_DLY_CS1_B2 : 7;         ///<BIT [22:16] PHY Write Leveling WCK DQC Delay CS1 B2.
        uint32_t RF10005A8_23_RSVD         : 1;         ///<BIT [23] Reserved.
        uint32_t PHY_WL_WCK_QSC_DLY_CS1_B2 : 7;         ///<BIT [30:24] PHY Write Leveling WCK QSC Delay CS1 B2.
        uint32_t RF10005A8_31_RSVD         : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0PhyWrLvlCtrlDataCs122_t;

///  @brief 0xF10005AC : Channel 0 PHY Write Leveling Control Data CS1 B3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_DQ_DLY_CS1_B3  : 7;         ///<BIT [6:0] PHY Write Leveling WCK DQ Delay CS1 B3.
        uint32_t RF10005AC_7_RSVD          : 1;         ///<BIT [7] Reserved.
        uint32_t PHY_WL_WCK_QS_DLY_CS1_B3  : 7;         ///<BIT [14:8] PHY Write Leveling WCK QS Delay CS1 B3.
        uint32_t RF10005AC_15_RSVD         : 1;         ///<BIT [15] Reserved.
        uint32_t PHY_WL_WCK_DQC_DLY_CS1_B3 : 7;         ///<BIT [22:16] PHY Write Leveling WCK DQC Delay CS1 B3.
        uint32_t RF10005AC_23_RSVD         : 1;         ///<BIT [23] Reserved.
        uint32_t PHY_WL_WCK_QSC_DLY_CS1_B3 : 7;         ///<BIT [30:24] PHY Write Leveling WCK QSC Delay CS1 B3.
        uint32_t RF10005AC_31_RSVD         : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0PhyWrLvlCtrlDataCs133_t;

///  @brief 0xF10005A0 : Channel 0 PHY Write Leveling Control Data CS1 B0
///  @brief 0xF10005AC : Channel 0 PHY Write Leveling Control Data CS1 B3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_DQ_DLY_CS1_Bx  : 7;         ///<BIT [6:0] PHY Write Leveling WCK DQ Delay CS1 Bx.
        uint32_t RF10005Ax_7_RSVD          : 1;         ///<BIT [7] Reserved.
        uint32_t PHY_WL_WCK_QS_DLY_CS1_Bx  : 7;         ///<BIT [14:8] PHY Write Leveling WCK QS Delay CS1 Bx.
        uint32_t RF10005Ax_15_RSVD         : 1;         ///<BIT [15] Reserved.
        uint32_t PHY_WL_WCK_DQC_DLY_CS1_Bx : 7;         ///<BIT [22:16] PHY Write Leveling WCK DQC Delay CS1 Bx.
        uint32_t RF10005Ax_23_RSVD         : 1;         ///<BIT [23] Reserved.
        uint32_t PHY_WL_WCK_QSC_DLY_CS1_Bx : 7;         ///<BIT [30:24] PHY Write Leveling WCK QSC Delay CS1 Bx.
        uint32_t RF10005Ax_31_RSVD         : 1;         ///<BIT [31] Reserved.
    } b;
} Chan0PhyWrLvlCtrlDataCs1xx_t;

///  @brief 0xF1000610 : Channel 0 PHY Write Leveling Clock Control CS0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_CK_DLY_CS0 : 5;         ///<BIT [4:0] PHY Write Leveling WCK Clock Delay CS0.
        uint32_t RF1000610_31_5_RSVD   :27;         ///<BIT [31:5] Reserved.
    } b;
} Chan0PhyWrLvlClkCtrlCs0_t;

///  @brief 0xF1000614 : Channel 0 PHY Write Leveling Clock Control CS1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_CK_DLY_CS1 : 5;         ///<BIT [4:0] PHY Write Leveling WCK Clock Delay CS1.
        uint32_t RF1000614_31_5_RSVD   :27;         ///<BIT [31:5] Reserved.
    } b;
} Chan0PhyWrLvlClkCtrlCs1_t;

///  @brief 0xF1000620 : Channel 0 PHY Write Leveling Control AC 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_AC_DLY_0 : 5;         ///<BIT [4:0] PHY Write Leveling WCK Address/Command Delay 0.
        uint32_t RF1000620_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;
} Chan0PhyWrLvlCtrlAc0_t;

///  @brief 0xF1000624 : Channel 0 PHY Write Leveling Control AC 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_AC_DLY_1 : 5;         ///<BIT [4:0] PHY Write Leveling WCK Address/Command Delay 1.
        uint32_t RF1000624_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;
} Chan0PhyWrLvlCtrlAc1_t;

///  @brief 0xF1000628 : Channel 0 PHY Write Leveling Control AC 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_WL_WCK_AC_DLY_2 : 5;         ///<BIT [4:0] PHY Write Leveling WCK Address/Command Delay 2.
        uint32_t RF1000628_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;
} Chan0PhyWrLvlCtrlAc2_t;

///  @brief 0xF1000640 : Channel 0 PHY Read Leveling Control CS0 B0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_RL_TAP_DLY_CS0_B0   : 8;         ///<BIT [7:0] PHY Read Leveling TAP Delay CS0 B0.
        uint32_t PHY_RL_CYCLE_DLY_CS0_B0 : 4;         ///<BIT [11:8] PHY Read Leveling Cycle Delay CS0 B0.
        uint32_t RF1000640_31_12_RSVD    :20;         ///<BIT [31:12] Reserved.
    } b;
} Chan0PhyRdLvlCtrlCs0B0_t;

///  @brief 0xF1000644 : Channel 0 PHY Read Leveling Control CS0 B1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_RL_TAP_DLY_CS0_B1   : 8;         ///<BIT [7:0] PHY Read Leveling TAP Delay CS0 B1.
        uint32_t PHY_RL_CYCLE_DLY_CS0_B1 : 4;         ///<BIT [11:8] PHY Read Leveling Cycle Delay CS0 B1.
        uint32_t RF1000644_31_12_RSVD    :20;         ///<BIT [31:12] Reserved.
    } b;
} Chan0PhyRdLvlCtrlCs0B1_t;

///  @brief 0xF1000648 : Channel 0 PHY Read Leveling Control CS0 B2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_RL_TAP_DLY_CS0_B2   : 8;         ///<BIT [7:0] PHY Read Leveling TAP Delay CS0 B2.
        uint32_t PHY_RL_CYCLE_DLY_CS0_B2 : 4;         ///<BIT [11:8] PHY Read Leveling Cycle Delay CS0 B2.
        uint32_t RF1000648_31_12_RSVD    :20;         ///<BIT [31:12] Reserved.
    } b;
} Chan0PhyRdLvlCtrlCs0B2_t;

///  @brief 0xF100064C : Channel 0 PHY Read Leveling Control CS0 B3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_RL_TAP_DLY_CS0_B3   : 8;         ///<BIT [7:0] PHY Read Leveling TAP Delay CS0 B3.
        uint32_t PHY_RL_CYCLE_DLY_CS0_B3 : 4;         ///<BIT [11:8] PHY Read Leveling Cycle Delay CS0 B3.
        uint32_t RF100064C_31_12_RSVD    :20;         ///<BIT [31:12] Reserved.
    } b;
} Chan0PhyRdLvlCtrlCs0B3_t;

///  @brief 0xF1000640 : Channel 0 PHY Read Leveling Control CS0 B0
///  @brief 0xF100064C : Channel 0 PHY Read Leveling Control CS0 B3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_RL_TAP_DLY_CS0_Bx   : 8;         ///<BIT [7:0] PHY Read Leveling TAP Delay CS0 Bx.
        uint32_t PHY_RL_CYCLE_DLY_CS0_Bx : 4;         ///<BIT [11:8] PHY Read Leveling Cycle Delay CS0 Bx.
        uint32_t RF100064x_31_12_RSVD    :20;         ///<BIT [31:12] Reserved.
    } b;
} Chan0PhyRdLvlCtrlCs0Bx_t;

///  @brief 0xF1000660 : Channel 0 PHY Read Leveling Control CS1 B0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_RL_TAP_DLY_CS1_B0   : 8;         ///<BIT [7:0] PHY Read Leveling TAP Delay CS1 B0.
        uint32_t PHY_RL_CYCLE_DLY_CS1_B0 : 4;         ///<BIT [11:8] PHY Read Leveling Cycle Delay CS1 B0.
        uint32_t RF1000660_31_12_RSVD    :20;         ///<BIT [31:12] Reserved.
    } b;
} Chan0PhyRdLvlCtrlCs1B0_t;

///  @brief 0xF1000664 : Channel 0 PHY Read Leveling Control CS1 B1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_RL_TAP_DLY_CS1_B1   : 8;         ///<BIT [7:0] PHY Read Leveling TAP Delay CS1 B1.
        uint32_t PHY_RL_CYCLE_DLY_CS1_B1 : 4;         ///<BIT [11:8] PHY Read Leveling Cycle Delay CS1 B1.
        uint32_t RF1000664_31_12_RSVD    :20;         ///<BIT [31:12] Reserved.
    } b;
} Chan0PhyRdLvlCtrlCs1B1_t;

///  @brief 0xF1000668 : Channel 0 PHY Read Leveling Control CS1 B2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_RL_TAP_DLY_CS1_B2   : 8;         ///<BIT [7:0] PHY Read Leveling TAP Delay CS1 B2.
        uint32_t PHY_RL_CYCLE_DLY_CS1_B2 : 4;         ///<BIT [11:8] PHY Read Leveling Cycle Delay CS1 B2.
        uint32_t RF1000668_31_12_RSVD    :20;         ///<BIT [31:12] Reserved.
    } b;
} Chan0PhyRdLvlCtrlCs1B2_t;

///  @brief 0xF100066C : Channel 0 PHY Read Leveling Control CS1 B3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_RL_TAP_DLY_CS1_B3   : 8;         ///<BIT [7:0] PHY Read Leveling TAP Delay CS1 B3.
        uint32_t PHY_RL_CYCLE_DLY_CS1_B3 : 4;         ///<BIT [11:8] PHY Read Leveling Cycle Delay CS1 B3.
        uint32_t RF100066C_31_12_RSVD    :20;         ///<BIT [31:12] Reserved.
    } b;
} Chan0PhyRdLvlCtrlCs1B3_t;

///  @brief 0xF1000660 : Channel 0 PHY Read Leveling Control CS1 B0
///  @brief 0xF100066C : Channel 0 PHY Read Leveling Control CS1 B3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_RL_TAP_DLY_CS1_Bx   : 8;         ///<BIT [7:0] PHY Read Leveling TAP Delay CS1 Bx.
        uint32_t PHY_RL_CYCLE_DLY_CS1_Bx : 4;         ///<BIT [11:8] PHY Read Leveling Cycle Delay CS1 Bx.
        uint32_t RF100066x_31_12_RSVD    :20;         ///<BIT [31:12] Reserved.
    } b;
} Chan0PhyRdLvlCtrlCs1Bx_t;

///  @brief 0xF1000700 : Channel 0 PHY CA Delay 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CA0_DLY     : 4;         ///<BIT [3:0] CA0 Delay.
        uint32_t CA0_WCK_DLY : 4;         ///<BIT [7:4] CA0 WCK Chain Delay.
        uint32_t CA1_DLY     : 4;         ///<BIT [11:8] CA1 Delay.
        uint32_t CA1_WCK_DLY : 4;         ///<BIT [15:12] CA1 WCK Chain Delay.
        uint32_t CA2_DLY     : 4;         ///<BIT [19:16] CA2 Delay.
        uint32_t CA2_WCK_DLY : 4;         ///<BIT [23:20] CA2 WCK Chain Delay.
        uint32_t CA3_DLY     : 4;         ///<BIT [27:24] CA3 Delay.
        uint32_t CA3_WCK_DLY : 4;         ///<BIT [31:28] CA3 WCK Chain Delay.
    } b;
} Chan0PhyCaDly0_t;

///  @brief 0xF1000704 : Channel 0 PHY CA Delay 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CA5_DLY     : 4;         ///<BIT [3:0] CA5 Delay.
        uint32_t CA5_WCK_DLY : 4;         ///<BIT [7:4] CA5 WCK Chain Delay.
        uint32_t CA6_DLY     : 4;         ///<BIT [11:8] CA6 Delay.
        uint32_t CA6_WCK_DLY : 4;         ///<BIT [15:12] CA6 WCK Chain Delay.
        uint32_t CA7_DLY     : 4;         ///<BIT [19:16] CA7 Delay.
        uint32_t CA7_WCK_DLY : 4;         ///<BIT [23:20] CA7 WCK Chain Delay.
        uint32_t CA8_DLY     : 4;         ///<BIT [27:24] CA8 Delay.
        uint32_t CA8_WCK_DLY : 4;         ///<BIT [31:28] CA8 WCK Chain Delay.
    } b;
} Chan0PhyCaDly1_t;

///  @brief 0xF1000708 : Channel 0 PHY CA Delay 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CA4_DLY     : 4;         ///<BIT [3:0] CA4 Delay.
        uint32_t CA4_WCK_DLY : 4;         ///<BIT [7:4] CA4 WCK Chain Delay.
        uint32_t CA9_DLY     : 4;         ///<BIT [11:8] CA9 Delay.
        uint32_t CA9_WCK_DLY : 4;         ///<BIT [15:12] CA9 WCK Chain Delay.
        uint32_t CKN_DLY     : 4;         ///<BIT [19:16] CKN Delay.
        uint32_t CKN_WCK_DLY : 4;         ///<BIT [23:20] CKN WCK Chain Delay.
        uint32_t CKP_DLY     : 4;         ///<BIT [27:24] CKP Delay.
        uint32_t CKP_WCK_DLY : 4;         ///<BIT [31:28] CKP WCK Chain Delay.
    } b;
} Chan0PhyCaDly2_t;

///  @brief 0xF100070C : Channel 0 PHY CA Delay 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CKE_DLY          : 4;         ///<BIT [3:0] CKE PAD Output Delay.
        uint32_t CKE_WCK_DLY      : 4;         ///<BIT [7:4] CKE PAD WCK Chain Delay.
        uint32_t CSN_DLY          : 4;         ///<BIT [11:8] CSN PAD Output Delay.
        uint32_t CSN_WCK_DLY      : 4;         ///<BIT [15:12] CSN PAD WCK Chain Delay.
        uint32_t ODT_DLY          : 4;         ///<BIT [19:16] ODT PAD Output Delay.
        uint32_t ODT_WCK_DLY      : 4;         ///<BIT [23:20] ODT PAD WCK Chain Delay.
        uint32_t OTHR_PAD_DLY     : 4;         ///<BIT [27:24] Other PAD Output Delay.
        uint32_t OTHR_PAD_WCK_DLY : 4;         ///<BIT [31:28] Other PAD WCK Chain Delay.
    } b;
} Chan0PhyCaDly3_t;

///  @brief 0xF1000740 : Channel 0 PHY CA Train Result
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CA_TRAIN_RSLT        :16;         ///<BIT [15:0] CA Train Result.
        uint32_t RF1000740_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;
} Chan0PhyCaTrainResult_t;

///  @brief 0xF1000760 : Channel 0 PHY PRBS Status and Mask B0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SUBPHY_PRBS_MSK_B0      : 9;         ///<BIT [8:0] Sub-PHY PRBS Mask B0.
        uint32_t RF1000760_15_9_RSVD     : 7;         ///<BIT [15:9] Reserved.
        uint32_t SUBPHY_PRBS_ERR_STAT_B0 : 9;         ///<BIT [24:16] Sub-PHY PRBS Error Status B0.
        uint32_t RF1000760_31_25_RSVD    : 7;         ///<BIT [31:25] Reserved.
    } b;
} Chan0PhyPrbsStatAndMskB0_t;

///  @brief 0xF1000764 : Channel 0 PHY PRBS Status and Mask B1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SUBPHY_PRBS_MSK_B1      : 9;         ///<BIT [8:0] Sub-PHY PRBS Mask B1.
        uint32_t RF1000764_15_9_RSVD     : 7;         ///<BIT [15:9] Reserved.
        uint32_t SUBPHY_PRBS_ERR_STAT_B1 : 9;         ///<BIT [24:16] Sub-PHY PRBS Error Status B1.
        uint32_t RF1000764_31_25_RSVD    : 7;         ///<BIT [31:25] Reserved.
    } b;
} Chan0PhyPrbsStatAndMskB1_t;

///  @brief 0xF1000768 : Channel 0 PHY PRBS Status and Mask B2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SUBPHY_PRBS_MSK_B2      : 9;         ///<BIT [8:0] Sub-PHY PRBS Mask B2.
        uint32_t RF1000768_15_9_RSVD     : 7;         ///<BIT [15:9] Reserved.
        uint32_t SUBPHY_PRBS_ERR_STAT_B2 : 9;         ///<BIT [24:16] Sub-PHY PRBS Error Status B2.
        uint32_t RF1000768_31_25_RSVD    : 7;         ///<BIT [31:25] Reserved.
    } b;
} Chan0PhyPrbsStatAndMskB2_t;

///  @brief 0xF100076C : Channel 0 PHY PRBS Status and Mask B3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SUBPHY_PRBS_MSK_B3      : 9;         ///<BIT [8:0] Sub-PHY PRBS Mask B3.
        uint32_t RF100076C_15_9_RSVD     : 7;         ///<BIT [15:9] Reserved.
        uint32_t SUBPHY_PRBS_ERR_STAT_B3 : 9;         ///<BIT [24:16] Sub-PHY PRBS Error Status B3.
        uint32_t RF100076C_31_25_RSVD    : 7;         ///<BIT [31:25] Reserved.
    } b;
} Chan0PhyPrbsStatAndMskB3_t;

///  @brief 0xF1000784 : Channel 0 PHY PRBS Status and Mask C0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SUBPHY_PRBS_MSK_C0      : 9;         ///<BIT [8:0] Sub-PHY PRBS Mask C0.
        uint32_t RF1000784_15_9_RSVD     : 7;         ///<BIT [15:9] Reserved.
        uint32_t SUBPHY_PRBS_ERR_STAT_C0 : 9;         ///<BIT [24:16] Sub-PHY PRBS Error Status C0.
        uint32_t RF1000784_31_25_RSVD    : 7;         ///<BIT [31:25] Reserved.
    } b;
} Chan0PhyPrbsStatAndMskC0_t;

///  @brief 0xF1000788 : Channel 0 PHY PRBS Status and Mask C1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SUBPHY_PRBS_MSK_C1      : 9;         ///<BIT [8:0] Sub-PHY PRBS Mask C1.
        uint32_t RF1000788_15_9_RSVD     : 7;         ///<BIT [15:9] Reserved.
        uint32_t SUBPHY_PRBS_ERR_STAT_C1 : 9;         ///<BIT [24:16] Sub-PHY PRBS Error Status C1.
        uint32_t RF1000788_31_25_RSVD    : 7;         ///<BIT [31:25] Reserved.
    } b;
} Chan0PhyPrbsStatAndMskC1_t;

///  @brief 0xF100078C : Channel 0 PHY PRBS Status and Mask C2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SUBPHY_PRBS_MSK_C2      : 9;         ///<BIT [8:0] Sub-PHY PRBS Mask C2.
        uint32_t RF100078C_15_9_RSVD     : 7;         ///<BIT [15:9] Reserved.
        uint32_t SUBPHY_PRBS_ERR_STAT_C2 : 9;         ///<BIT [24:16] Sub-PHY PRBS Error Status C2.
        uint32_t RF100078C_31_25_RSVD    : 7;         ///<BIT [31:25] Reserved.
    } b;
} Chan0PhyPrbsStatAndMskC2_t;

///  @brief 0xF1000790 : Channel 0 PHY PRBS Status and Mask C3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SUBPHY_PRBS_MSK_C3      : 9;         ///<BIT [8:0] Sub-PHY PRBS Mask C3.
        uint32_t RF1000790_15_9_RSVD     : 7;         ///<BIT [15:9] Reserved.
        uint32_t SUBPHY_PRBS_ERR_STAT_C3 : 9;         ///<BIT [24:16] Sub-PHY PRBS Error Status C3.
        uint32_t RF1000790_31_25_RSVD    : 7;         ///<BIT [31:25] Reserved.
    } b;
} Chan0PhyPrbsStatAndMskC3_t;

///  @brief 0xF1000798 : Channel 0 PHY PRBS Sub-PHY Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DATA_SUBPHY_POS_ERROR : 9;         ///<BIT [8:0] Data Sub-PHY Positive Error.
        uint32_t CMD_SUBPHY_POS_ERR    : 5;         ///<BIT [13:9] Command Sub-PHY Positive Error.
        uint32_t RF1000798_15_14_RSVD  : 2;         ///<BIT [15:14] Reserved.
        uint32_t DATA_SUBPHY_NEG_ERR   : 9;         ///<BIT [24:16] Data Sub-PHY Negative Error.
        uint32_t CMD_SUBPHY_NEG_ERR    : 5;         ///<BIT [29:25] Command Sub-PHY Negative Error.
        uint32_t RF1000798_31_30_RSVD  : 2;         ///<BIT [31:30] Reserved.
    } b;
} Chan0PhyPrbsSubPhyStat_t;

///  @brief 0xF10007A0 : Channel 0 PHY PRBS Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PRBS_QSP_DEBUG_VAL   : 9;         ///<BIT [8:0] PRBS QSP Debug Value.
        uint32_t PRBS_CKP_DEBUG_VAL   : 5;         ///<BIT [13:9] PRBS CKP Debug Value.
        uint32_t RF10007A0_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t PRBS_START           : 1;         ///<BIT [16] PRBS Start.
        uint32_t PRBS_USER_MD         : 1;         ///<BIT [17] PRBS User Mode.
        uint32_t PRBS_CK_DEBUG_MD     : 1;         ///<BIT [18] PRBS CK Debug Mode.
        uint32_t PRBS_INV_MSK_SEL     : 1;         ///<BIT [19] PRBS Inverter or Mask Select.
        uint32_t PRBS_RFIFO_RDPTR_DLY : 4;         ///<BIT [23:20] PRBS Read FIFO Read Pointer Delay.
        uint32_t PRBS_ADCM_SDR        : 1;         ///<BIT [24] PRBS Address/Command SDR.
        uint32_t PRBS_ADCM_DDR_MD     : 1;         ///<BIT [25] PRBS Address/Command DDR Mode.
        uint32_t RF10007A0_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;
} Chan0PhyPrbsCtrl0_t;

///  @brief 0xF10007A4 : Channel 0 PHY PRBS Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PRBS_DATA_DLY        : 5;         ///<BIT [4:0] PRBS Data Delay.
        uint32_t PRBS_CMD0_DLY        : 5;         ///<BIT [9:5] PRBS Command 0 Delay.
        uint32_t PRBS_CMD1_DLY        : 5;         ///<BIT [14:10] PRBS Command 1 Delay.
        uint32_t PRBS_CMD2_DLY        : 5;         ///<BIT [19:15] PRBS Command 2 Delay.
        uint32_t PRBS_CMD3_DLY        : 5;         ///<BIT [24:20] PRBS Command 3 Delay.
        uint32_t PRBS_CMD4_DLY        : 5;         ///<BIT [29:25] PRBS Command 4 Delay.
        uint32_t RF10007A4_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;
} Chan0PhyPrbsCtrl1_t;

///  @brief 0xF1000800 : MC Trustzone Range 0 Low
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TZ_VAL_0             : 1;         ///<BIT [0] Trust Zone Valid 0.
        uint32_t TZ_PERM_0            : 2;         ///<BIT [2:1] Trust Zone Permission 0.
        uint32_t RZ_EN_0              : 1;         ///<BIT [3] Restrict Zone Enable 0.
        uint32_t TZ_PERM_UR           : 2;         ///<BIT [5:4] Trust Zone Permission for Undefined Region.
        uint32_t RZ_EN_UR             : 1;         ///<BIT [6] Restrict Zone Enable for Undefined Region.
        uint32_t RF1000800_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint32_t TZ_AREA_LEN_0        : 5;         ///<BIT [12:8] Trust Zone Area Length 0.
        uint32_t RF1000800_19_13_RSVD : 7;         ///<BIT [19:13] Reserved.
        uint32_t TZ_START_ADDR_L_0    :12;         ///<BIT [31:20] Trust Zone Start Address Low 0.
    } b;
} McTrustzonRng0Low_t;

///  @brief 0xF1000804 : MC Trustzone Range 0 High
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TZ_START_ADDR_H_0   : 8;         ///<BIT [7:0] Trust Zone Start Address High 0.
        uint32_t RF1000804_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} McTrustzonRng0High_t;

///  @brief 0xF1000808 : MC Trustzone Range 1 Low
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TZ_VAL_1             : 1;         ///<BIT [0] Trust Zone Valid 1.
        uint32_t TZ_PERM_1            : 2;         ///<BIT [2:1] Trust Zone Permission 1.
        uint32_t RZ_EN_1              : 1;         ///<BIT [3] Restrict Zone Enable 1.
        uint32_t RF1000808_7_4_RSVD   : 4;         ///<BIT [7:4] Reserved.
        uint32_t TZ_AREA_LEN_1        : 5;         ///<BIT [12:8] Trust Zone Area Length 1.
        uint32_t RF1000808_19_13_RSVD : 7;         ///<BIT [19:13] Reserved.
        uint32_t TZ_START_ADDR_L_1    :12;         ///<BIT [31:20] Trust Zone Start Address Low 1.
    } b;
} McTrustzonRng1Low_t;

///  @brief 0xF100080C : MC Trustzone Range 1 High
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TZ_START_ADDR_H_1   : 8;         ///<BIT [7:0] Trust Zone Start Address High 1.
        uint32_t RF100080C_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} McTrustzonRng1High_t;

///  @brief 0xF1000878 : MC Trustzone Range 15 Low
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TZ_VAL_15            : 1;         ///<BIT [0] Trust Zone Valid 15.
        uint32_t TZ_PERM_15           : 2;         ///<BIT [2:1] Trust Zone Permission 15.
        uint32_t RZ_EN_15             : 1;         ///<BIT [3] Restrict Zone Enable 15.
        uint32_t RF1000878_7_4_RSVD   : 4;         ///<BIT [7:4] Reserved.
        uint32_t TZ_AREA_LEN_15       : 5;         ///<BIT [12:8] Trust Zone Area Length 15.
        uint32_t RF1000878_19_13_RSVD : 7;         ///<BIT [19:13] Reserved.
        uint32_t TZ_START_ADDR_L_15   :12;         ///<BIT [31:20] Trust Zone Start Address Low 15.
    } b;
} McTrustzonRng15Low_t;

///  @brief 0xF100087C : MC Trustzone Range 15 High
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TZ_START_ADDR_H_15  : 8;         ///<BIT [7:0] Trust Zone Start Address High 15.
        uint32_t RF100087C_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;
} McTrustzonRng15High_t;


/// @brief 0xF1000000-0xF1000880 : DdrMc_t MODULE
typedef struct
{
    MemControllIdAndRevision_t memControllIdAndRevision;                         ///< 0xF1000000 : Memory Controller ID and Revision
    MemControllStat_t memControllStat;                                           ///< 0xF1000004 : Memory Controller Status
    DramStat_t dramStat;                                                         ///< 0xF1000008 : DRAM Status
    DramMode_t dramMode;                                                         ///< 0xF100000C : DRAM Mode
    MemControllWrPathStat_t memControllWrPathStat;                               ///< 0xF1000010 : Memory Controller Write Path Status
    MemControllRdPathStat_t memControllRdPathStat;                               ///< 0xF1000014 : Memory Controller Read Path Status
    uint8_t rsvd_RF1000018[8];                                                   ///< 0xF1000018 - 0xF100001F : Reserved.
    DdrmcUserInitiateCmd0_t ddrmcUserInitiateCmd0;                               ///< 0xF1000020 : DDRMC User Initiated Command 0
    DdrmcUserInitiateCmd1_t ddrmcUserInitiateCmd1;                               ///< 0xF1000024 : DDRMC User Initiated Command 1
    DdrmcUserInitiateCmd2_t ddrmcUserInitiateCmd2;                               ///< 0xF1000028 : DDRMC User Initiated Command 2
    uint8_t rsvd_RF100002C[20];                                                  ///< 0xF100002C - 0xF100003F : Reserved.
    IntSramCtrl_t intSramCtrl;                                                   ///< 0xF1000040 : Internal SRAM Control
    MemControllCtrl0_t memControllCtrl0;                                         ///< 0xF1000044 : Memory Controller Control 0
    DdrmcExclusivMonitorCtrl_t ddrmcExclusivMonitorCtrl;                         ///< 0xF1000048 : DDRMC Exclusive Monitor Control
    DdrmcDataCoherencChkCtrl_t ddrmcDataCoherencChkCtrl;                         ///< 0xF100004C : DDRMC Data Coherency Check Control
    DdrmcRestrictZoneAccessCtrl_t ddrmcRestrictZoneAccessCtrl;                   ///< 0xF1000050 : DDRMC Restrict Zone Access Control
    MemControllPowerCtrl_t memControllPowerCtrl;                                 ///< 0xF1000054 : Memory Controller Power Control
    DdrmcWrCmdBufCtrl_t ddrmcWrCmdBufCtrl;                                       ///< 0xF1000058 : DDRMC Write Command Buffer Control
    BankQueueCtrl_t bankQueueCtrl;                                               ///< 0xF100005C : Bank Queue Control
    RobCtrl_t robCtrl;                                                           ///< 0xF1000060 : ROB Control
    uint8_t rsvd_RF1000064[28];                                                  ///< 0xF1000064 - 0xF100007F : Reserved.
    PortRrbWt_t portRrbWt;                                                       ///< 0xF1000080 : Port RRB Weight
    PortPriorityCtrl_t portPriorityCtrl;                                         ///< 0xF1000084 : Port Priority Control
    RrbStarvatiPreventi0_t rrbStarvatiPreventi0;                                 ///< 0xF1000088 : RRB Starvation Prevention 0
    uint8_t rsvd_RF100008C[52];                                                  ///< 0xF100008C - 0xF10000BF : Reserved.
    DdrmcRegisterTblCtrl_t ddrmcRegisterTblCtrl;                                 ///< 0xF10000C0 : DDRMC Register Table Control
    uint32_t TBL_REG_DATA_0;                                                     ///< 0xF10000C4 : DDRMC Register Table Data 0
    DdrmcRegisterTblData1_t ddrmcRegisterTblData1;                               ///< 0xF10000C8 : DDRMC Register Table Data 1
    uint8_t rsvd_RF10000CC[52];                                                  ///< 0xF10000CC - 0xF10000FF : Reserved.
    DdrmcPerformaCntrCfg0_t ddrmcPerformaCntrCfg0;                               ///< 0xF1000100 : DDRMC Performance Counter Configuration 0
    DdrmcPerformaCntrCfg1_t ddrmcPerformaCntrCfg1;                               ///< 0xF1000104 : DDRMC Performance Counter Configuration 1
    DdrmcPerformaCntrStat_t ddrmcPerformaCntrStat;                               ///< 0xF1000108 : DDRMC Performance Counter Status
    DdrmcPerformaCntrCtrl_t ddrmcPerformaCntrCtrl;                               ///< 0xF100010C : DDRMC Performance Counter Control
    uint32_t PC_COUNTER0;                                                        ///< 0xF1000110 : DDRMC Performance Counter 0
    uint32_t PC_COUNTER1;                                                        ///< 0xF1000114 : DDRMC Performance Counter 1
    uint32_t PC_COUNTER2;                                                        ///< 0xF1000118 : DDRMC Performance Counter 2
    uint32_t PC_COUNTER3;                                                        ///< 0xF100011C : DDRMC Performance Counter 3
    uint32_t PC_COUNTER4;                                                        ///< 0xF1000120 : DDRMC Performance Counter 4
    uint32_t PC_COUNTER5;                                                        ///< 0xF1000124 : DDRMC Performance Counter 5
    uint32_t PC_COUNTER6;                                                        ///< 0xF1000128 : DDRMC Performance Counter 6
    uint32_t PC_COUNTER7;                                                        ///< 0xF100012C : DDRMC Performance Counter 7
    uint8_t rsvd_RF1000130[16];                                                  ///< 0xF1000130 - 0xF100013F : Reserved.
    DdrmcIntrStat_t ddrmcIntrStat;                                               ///< 0xF1000140 : DDRMC Interrupt Status
    DdrmcIntrEn_t ddrmcIntrEn;                                                   ///< 0xF1000144 : DDRMC Interrupt Enable
    uint8_t rsvd_RF1000148[8];                                                   ///< 0xF1000148 - 0xF100014F : Reserved.
    DdrmcAddrDecodeErrMstrId_t ddrmcAddrDecodeErrMstrId;                         ///< 0xF1000150 : DDRMC Address Decode Error Master ID
    uint32_t DEC_ERR_ADDR_L;                                                     ///< 0xF1000154 : DDRMC Address Decode Error Address Low
    DdrmcAddrDecodeErrAddrHigh_t ddrmcAddrDecodeErrAddrHigh;                     ///< 0xF1000158 : DDRMC Address Decode Error Address High
    uint8_t rsvd_RF100015C[4];                                                   ///< 0xF100015C - 0xF100015F : Reserved.
    uint32_t AM_PERIOD;                                                          ///< 0xF1000160 : Activity Monitor Check Period
    ActivityMonitorThr_t activityMonitorThr;                                     ///< 0xF1000164 : Activity Monitor Threshold
    uint8_t rsvd_RF1000168[8];                                                   ///< 0xF1000168 - 0xF100016F : Reserved.
    TrustZoneErrMstrId_t trustZoneErrMstrId;                                     ///< 0xF1000170 : Trust Zone Error Master ID
    uint32_t TZ_ERR_ADDR_L;                                                      ///< 0xF1000174 : Trust Zone Error Address Low
    uint32_t TZ_ERR_ADDR_H;                                                      ///< 0xF1000178 : Trust Zone Error Address High
    uint8_t rsvd_RF100017C[20];                                                  ///< 0xF100017C - 0xF100018F : Reserved.
    ParCrcErrStat_t parCrcErrStat;                                               ///< 0xF1000190 : Parity CRC Error Status
    uint8_t rsvd_RF1000194[48];                                                  ///< 0xF1000194 - 0xF10001C3 : Reserved.
    DdrmcTstMode0_t ddrmcTstMode0;                                               ///< 0xF10001C4 : DDRMC Test Mode 0
    DdrmcTstMode1_t ddrmcTstMode1;                                               ///< 0xF10001C8 : DDRMC Test Mode 1
    uint8_t rsvd_RF10001CC[52];                                                  ///< 0xF10001CC - 0xF10001FF : Reserved.
    Chan0MmapCs0_t chan0MmapCs0;                                                 ///< 0xF1000200 : Channel 0 MMAP CS0
    Chan0MmapCs1_t chan0MmapCs1;                                                 ///< 0xF1000204 : Channel 0 MMAP CS1
    uint8_t rsvd_RF1000208[8];                                                   ///< 0xF1000208 - 0xF100020F : Reserved.
    Chan0MemControllPhysicalAddrMapCs0_t chan0MemControllPhysicalAddrMapCs0;     ///< 0xF1000210 : Channel 0 Memory Controller Physical Address Map CS0
    Chan0MemControllPhysicalAddrMapCs1_t chan0MemControllPhysicalAddrMapCs1;     ///< 0xF1000214 : Channel 0 Memory Controller Physical Address Map CS1
    uint8_t rsvd_RF1000218[168];                                                 ///< 0xF1000218 - 0xF10002BF : Reserved.
    Chan0MemCtrl1_t chan0MemCtrl1;                                               ///< 0xF10002C0 : Channel 0 Memory Control 1
    Chan0MemCtrl2_t chan0MemCtrl2;                                               ///< 0xF10002C4 : Channel 0 Memory Control 2
    Chan0MemCtrl3_t chan0MemCtrl3;                                               ///< 0xF10002C8 : Channel 0 Memory Control 3
    uint32_t AUTO_SR_IDLE_TIME;                                                  ///< 0xF10002CC : Channel 0 Memory Control 4
    uint8_t rsvd_RF10002D0[48];                                                  ///< 0xF10002D0 - 0xF10002FF : Reserved.
    Chan0DramCfg1_t chan0DramCfg1;                                               ///< 0xF1000300 : Channel 0 DRAM Configuration 1
    Chan0DramCfg2_t chan0DramCfg2;                                               ///< 0xF1000304 : Channel 0 DRAM Configuration 2
    Chan0DramCfg3_t chan0DramCfg3;                                               ///< 0xF1000308 : Channel 0 DRAM Configuration 3
    uint8_t rsvd_RF100030C[4];                                                   ///< 0xF100030C - 0xF100030F : Reserved.
    Chan0DramCfg5Cs0_t chan0DramCfg5Cs0;                                         ///< 0xF1000310 : Channel 0 DRAM Configuration 5 CS0
    Chan0DramCfg5Cs1_t chan0DramCfg5Cs1;                                         ///< 0xF1000314 : Channel 0 DRAM Configuration 5 CS1
    uint8_t rsvd_RF1000318[40];                                                  ///< 0xF1000318 - 0xF100033F : Reserved.
    Chan0DramCtrl1_t chan0DramCtrl1;                                             ///< 0xF1000340 : Channel 0 DRAM Control 1
    Chan0DramCtrl2_t chan0DramCtrl2;                                             ///< 0xF1000344 : Channel 0 DRAM Control 2
    Chan0DramCtrl3_t chan0DramCtrl3;                                             ///< 0xF1000348 : Channel 0 DRAM Control 3
    uint8_t rsvd_RF100034C[36];                                                  ///< 0xF100034C - 0xF100036F : Reserved.
    Chan0ModeRdData_t chan0ModeRdData;                                           ///< 0xF1000370 : Channel 0 Mode Read Data
    Chan0MultiPurposeRdData0_t chan0MultiPurposeRdData0;                         ///< 0xF1000374 : Channel 0 Multi-Purpose Read Data 0
    Chan0MultiPurposeRdData1_t chan0MultiPurposeRdData1;                         ///< 0xF1000378 : Channel 0 Multi-Purpose Read Data 1
    uint8_t rsvd_RF100037C[4];                                                   ///< 0xF100037C - 0xF100037F : Reserved.
    Chan0DdrInitialiTmCtrl0_t chan0DdrInitialiTmCtrl0;                           ///< 0xF1000380 : Channel 0 DDR Initialization Timing Control 0
    Chan0DdrInitialiTmCtrl1_t chan0DdrInitialiTmCtrl1;                           ///< 0xF1000384 : Channel 0 DDR Initialization Timing Control 1
    Chan0Zqc0_t chan0Zqc0;                                                       ///< 0xF1000388 : Channel 0 ZQC 0
    Chan0Zqc1_t chan0Zqc1;                                                       ///< 0xF100038C : Channel 0 ZQC 1
    Chan0RefreshTm_t chan0RefreshTm;                                             ///< 0xF1000390 : Channel 0 Refresh Timing
    Chan0SelfRefreshTm_t chan0SelfRefreshTm;                                     ///< 0xF1000394 : Channel 0 Self-Refresh Timing
    Chan0PowerDnTm_t chan0PowerDnTm;                                             ///< 0xF1000398 : Channel 0 Power-Down Timing
    Chan0ModeRegisterSetTm_t chan0ModeRegisterSetTm;                             ///< 0xF100039C : Channel 0 Mode Register Set Timing
    Chan0ActivateTm_t chan0ActivateTm;                                           ///< 0xF10003A0 : Channel 0 Activate Timing
    Chan0PreChargeTm_t chan0PreChargeTm;                                         ///< 0xF10003A4 : Channel 0 Pre-charge Timing
    Chan0CasRasTm_t chan0CasRasTm;                                               ///< 0xF10003A8 : Channel 0 CAS/RAS Timing
    Chan0OffSpecTm_t chan0OffSpecTm;                                             ///< 0xF10003AC : Channel 0 Off-Spec Timing
    Chan0DramRdTm_t chan0DramRdTm;                                               ///< 0xF10003B0 : Channel 0 DRAM Read Timing
    Chan0DramTrainTm_t chan0DramTrainTm;                                         ///< 0xF10003B4 : Channel 0 DRAM Train Timing
    Chan0DramCrcAndParTm_t chan0DramCrcAndParTm;                                 ///< 0xF10003B8 : Channel 0 DRAM CRC and Parity Timing
    Chan0DramMaxPowerDnTm_t chan0DramMaxPowerDnTm;                               ///< 0xF10003BC : Channel 0 DRAM Maximum Power Down Timing
    uint8_t rsvd_RF10003C0[64];                                                  ///< 0xF10003C0 - 0xF10003FF : Reserved.
    Chan0PhyCtrl1_t chan0PhyCtrl1;                                               ///< 0xF1000400 : Channel 0 PHY Control 1
    Chan0PhyCtrl2_t chan0PhyCtrl2;                                               ///< 0xF1000404 : Channel 0 PHY Control 2
    Chan0PhyCtrl3_t chan0PhyCtrl3;                                               ///< 0xF1000408 : Channel 0 PHY Control 3
    Chan0PhyCtrl4_t chan0PhyCtrl4;                                               ///< 0xF100040C : Channel 0 PHY Control 4
    Chan0PhyCtrl5_t chan0PhyCtrl5;                                               ///< 0xF1000410 : Channel 0 PHY Control 5
    Chan0PhyCtrl6_t chan0PhyCtrl6;                                               ///< 0xF1000414 : Channel 0 PHY Control 6
    uint8_t rsvd_RF1000418[4];                                                   ///< 0xF1000418 - 0xF100041B : Reserved.
    Chan0PhyCtrl8_t chan0PhyCtrl8;                                               ///< 0xF100041C : Channel 0 PHY Control 8
    Chan0PhyCtrl9_t chan0PhyCtrl9;                                               ///< 0xF1000420 : Channel 0 PHY Control 9
    Chan0PhyCtrl10_t chan0PhyCtrl10;                                             ///< 0xF1000424 : Channel 0 PHY Control 10
    Chan0PhyCtrl11_t chan0PhyCtrl11;                                             ///< 0xF1000428 : Channel 0 PHY Control 11
    Chan0PhyCtrl12_t chan0PhyCtrl12;                                             ///< 0xF100042C : Channel 0 PHY Control 12
    Chan0PhyCtrl13_t chan0PhyCtrl13;                                             ///< 0xF1000430 : Channel 0 PHY Control 13
    Chan0PhyCtrl14_t chan0PhyCtrl14;                                             ///< 0xF1000434 : Channel 0 PHY Control 14
    Chan0PhyCtrl15_t chan0PhyCtrl15;                                             ///< 0xF1000438 : Channel 0 PHY Control 15
    Chan0PhyCtrl16_t chan0PhyCtrl16;                                             ///< 0xF100043C : Channel 0 PHY Control 16
    Chan0PhyCtrl17_t chan0PhyCtrl17;                                             ///< 0xF1000440 : Channel 0 PHY Control 17
    uint8_t rsvd_RF1000444[188];                                                 ///< 0xF1000444 - 0xF10004FF : Reserved.
    /*
    Chan0PhyDllCtrlB0_t chan0PhyDllCtrlB0;                                       ///< 0xF1000500 : Channel 0 PHY DLL Control B0
    Chan0PhyDllCtrlB1_t chan0PhyDllCtrlB1;                                       ///< 0xF1000504 : Channel 0 PHY DLL Control B1
    Chan0PhyDllCtrlB2_t chan0PhyDllCtrlB2;                                       ///< 0xF1000508 : Channel 0 PHY DLL Control B2
    Chan0PhyDllCtrlB3_t chan0PhyDllCtrlB3;                                       ///< 0xF100050C : Channel 0 PHY DLL Control B3
    */
    Chan0PhyDllCtrlBx_t chan0PhyDllCtrlB[4];
    uint8_t rsvd_RF1000510[24];                                                  ///< 0xF1000510 - 0xF1000527 : Reserved.
    Chan0PhyDllByteSel_t chan0PhyDllByteSel;                                     ///< 0xF1000528 : Channel 0 PHY DLL Byte Select
    Chan0PhyDllDly_t chan0PhyDllDly;                                             ///< 0xF100052C : Channel 0 PHY DLL Delay
    uint8_t rsvd_RF1000530[16];                                                  ///< 0xF1000530 - 0xF100053F : Reserved.
    Chan0PhyDataByteCtrlB0_t chan0PhyDataByteCtrlB0;                             ///< 0xF1000540 : Channel 0 PHY Data Byte Control B0
    Chan0PhyDataByteCtrlB1_t chan0PhyDataByteCtrlB1;                             ///< 0xF1000544 : Channel 0 PHY Data Byte Control B1
    Chan0PhyDataByteCtrlB2_t chan0PhyDataByteCtrlB2;                             ///< 0xF1000548 : Channel 0 PHY Data Byte Control B2
    Chan0PhyDataByteCtrlB3_t chan0PhyDataByteCtrlB3;                             ///< 0xF100054C : Channel 0 PHY Data Byte Control B3
    uint8_t rsvd_RF1000550[32];                                                  ///< 0xF1000550 - 0xF100056F : Reserved.
    Chan0PhyWrAndRdLvlCtrl_t chan0PhyWrAndRdLvlCtrl;                             ///< 0xF1000570 : Channel 0 PHY Write and Read Leveling Control
    uint8_t rsvd_RF1000574[12];                                                  ///< 0xF1000574 - 0xF100057F : Reserved.
    /*
    Chan0PhyWrLvlCtrlDataCs000_t chan0PhyWrLvlCtrlDataCs00_0;                    ///< 0xF1000580 : Channel 0 PHY Write Leveling Control Data CS0 B0
    Chan0PhyWrLvlCtrlDataCs011_t chan0PhyWrLvlCtrlDataCs01_1;                    ///< 0xF1000584 : Channel 0 PHY Write Leveling Control Data CS0 B1
    Chan0PhyWrLvlCtrlDataCs022_t chan0PhyWrLvlCtrlDataCs02_2;                    ///< 0xF1000588 : Channel 0 PHY Write Leveling Control Data CS0 B2
    Chan0PhyWrLvlCtrlDataCs033_t chan0PhyWrLvlCtrlDataCs03_3;                    ///< 0xF100058C : Channel 0 PHY Write Leveling Control Data CS0 B3
    */
    Chan0PhyWrLvlCtrlDataCs0xx_t chan0PhyWrLvlCtrlDataCs0[4];
    uint8_t rsvd_RF1000590[16];                                                  ///< 0xF1000590 - 0xF100059F : Reserved.
    /*
    Chan0PhyWrLvlCtrlDataCs100_t chan0PhyWrLvlCtrlDataCs10_0;                    ///< 0xF10005A0 : Channel 0 PHY Write Leveling Control Data CS1 B0
    Chan0PhyWrLvlCtrlDataCs111_t chan0PhyWrLvlCtrlDataCs11_1;                    ///< 0xF10005A4 : Channel 0 PHY Write Leveling Control Data CS1 B1
    Chan0PhyWrLvlCtrlDataCs122_t chan0PhyWrLvlCtrlDataCs12_2;                    ///< 0xF10005A8 : Channel 0 PHY Write Leveling Control Data CS1 B2
    Chan0PhyWrLvlCtrlDataCs133_t chan0PhyWrLvlCtrlDataCs13_3;                    ///< 0xF10005AC : Channel 0 PHY Write Leveling Control Data CS1 B3
    */
    Chan0PhyWrLvlCtrlDataCs1xx_t chan0PhyWrLvlCtrlDataCs1[4];
    uint8_t rsvd_RF10005B0[96];                                                  ///< 0xF10005B0 - 0xF100060F : Reserved.
    Chan0PhyWrLvlClkCtrlCs0_t chan0PhyWrLvlClkCtrlCs0;                           ///< 0xF1000610 : Channel 0 PHY Write Leveling Clock Control CS0
    Chan0PhyWrLvlClkCtrlCs1_t chan0PhyWrLvlClkCtrlCs1;                           ///< 0xF1000614 : Channel 0 PHY Write Leveling Clock Control CS1
    uint8_t rsvd_RF1000618[8];                                                   ///< 0xF1000618 - 0xF100061F : Reserved.
    Chan0PhyWrLvlCtrlAc0_t chan0PhyWrLvlCtrlAc0;                                 ///< 0xF1000620 : Channel 0 PHY Write Leveling Control AC 0
    Chan0PhyWrLvlCtrlAc1_t chan0PhyWrLvlCtrlAc1;                                 ///< 0xF1000624 : Channel 0 PHY Write Leveling Control AC 1
    Chan0PhyWrLvlCtrlAc2_t chan0PhyWrLvlCtrlAc2;                                 ///< 0xF1000628 : Channel 0 PHY Write Leveling Control AC 2
    uint8_t rsvd_RF100062C[20];                                                  ///< 0xF100062C - 0xF100063F : Reserved.
    /*
    Chan0PhyRdLvlCtrlCs0B0_t chan0PhyRdLvlCtrlCs0B0;                             ///< 0xF1000640 : Channel 0 PHY Read Leveling Control CS0 B0
    Chan0PhyRdLvlCtrlCs0B1_t chan0PhyRdLvlCtrlCs0B1;                             ///< 0xF1000644 : Channel 0 PHY Read Leveling Control CS0 B1
    Chan0PhyRdLvlCtrlCs0B2_t chan0PhyRdLvlCtrlCs0B2;                             ///< 0xF1000648 : Channel 0 PHY Read Leveling Control CS0 B2
    Chan0PhyRdLvlCtrlCs0B3_t chan0PhyRdLvlCtrlCs0B3;                             ///< 0xF100064C : Channel 0 PHY Read Leveling Control CS0 B3
    */
    Chan0PhyRdLvlCtrlCs0Bx_t chan0PhyRdLvlCtrlCs0B[4];
    uint8_t rsvd_RF1000650[16];                                                  ///< 0xF1000650 - 0xF100065F : Reserved.
    /*
    Chan0PhyRdLvlCtrlCs1B0_t chan0PhyRdLvlCtrlCs1B0;                             ///< 0xF1000660 : Channel 0 PHY Read Leveling Control CS1 B0
    Chan0PhyRdLvlCtrlCs1B1_t chan0PhyRdLvlCtrlCs1B1;                             ///< 0xF1000664 : Channel 0 PHY Read Leveling Control CS1 B1
    Chan0PhyRdLvlCtrlCs1B2_t chan0PhyRdLvlCtrlCs1B2;                             ///< 0xF1000668 : Channel 0 PHY Read Leveling Control CS1 B2
    Chan0PhyRdLvlCtrlCs1B3_t chan0PhyRdLvlCtrlCs1B3;                             ///< 0xF100066C : Channel 0 PHY Read Leveling Control CS1 B3
    */
    Chan0PhyRdLvlCtrlCs1Bx_t chan0PhyRdLvlCtrlCs1B[4];
    uint8_t rsvd_RF1000670[144];                                                 ///< 0xF1000670 - 0xF10006FF : Reserved.
    Chan0PhyCaDly0_t chan0PhyCaDly0;                                             ///< 0xF1000700 : Channel 0 PHY CA Delay 0
    Chan0PhyCaDly1_t chan0PhyCaDly1;                                             ///< 0xF1000704 : Channel 0 PHY CA Delay 1
    Chan0PhyCaDly2_t chan0PhyCaDly2;                                             ///< 0xF1000708 : Channel 0 PHY CA Delay 2
    Chan0PhyCaDly3_t chan0PhyCaDly3;                                             ///< 0xF100070C : Channel 0 PHY CA Delay 3
    uint8_t rsvd_RF1000710[48];                                                  ///< 0xF1000710 - 0xF100073F : Reserved.
    Chan0PhyCaTrainResult_t chan0PhyCaTrainResult;                               ///< 0xF1000740 : Channel 0 PHY CA Train Result
    uint8_t rsvd_RF1000744[28];                                                  ///< 0xF1000744 - 0xF100075F : Reserved.
    Chan0PhyPrbsStatAndMskB0_t chan0PhyPrbsStatAndMskB0;                         ///< 0xF1000760 : Channel 0 PHY PRBS Status and Mask B0
    Chan0PhyPrbsStatAndMskB1_t chan0PhyPrbsStatAndMskB1;                         ///< 0xF1000764 : Channel 0 PHY PRBS Status and Mask B1
    Chan0PhyPrbsStatAndMskB2_t chan0PhyPrbsStatAndMskB2;                         ///< 0xF1000768 : Channel 0 PHY PRBS Status and Mask B2
    Chan0PhyPrbsStatAndMskB3_t chan0PhyPrbsStatAndMskB3;                         ///< 0xF100076C : Channel 0 PHY PRBS Status and Mask B3
    uint8_t rsvd_RF1000770[20];                                                  ///< 0xF1000770 - 0xF1000783 : Reserved.
    Chan0PhyPrbsStatAndMskC0_t chan0PhyPrbsStatAndMskC0;                         ///< 0xF1000784 : Channel 0 PHY PRBS Status and Mask C0
    Chan0PhyPrbsStatAndMskC1_t chan0PhyPrbsStatAndMskC1;                         ///< 0xF1000788 : Channel 0 PHY PRBS Status and Mask C1
    Chan0PhyPrbsStatAndMskC2_t chan0PhyPrbsStatAndMskC2;                         ///< 0xF100078C : Channel 0 PHY PRBS Status and Mask C2
    Chan0PhyPrbsStatAndMskC3_t chan0PhyPrbsStatAndMskC3;                         ///< 0xF1000790 : Channel 0 PHY PRBS Status and Mask C3
    uint8_t rsvd_RF1000794[4];                                                   ///< 0xF1000794 - 0xF1000797 : Reserved.
    Chan0PhyPrbsSubPhyStat_t chan0PhyPrbsSubPhyStat;                             ///< 0xF1000798 : Channel 0 PHY PRBS Sub-PHY Status
    uint32_t PRBS_USER_PATT;                                                     ///< 0xF100079C : Channel 0 PHY PRBS User Pattern
    Chan0PhyPrbsCtrl0_t chan0PhyPrbsCtrl0;                                       ///< 0xF10007A0 : Channel 0 PHY PRBS Control 0
    Chan0PhyPrbsCtrl1_t chan0PhyPrbsCtrl1;                                       ///< 0xF10007A4 : Channel 0 PHY PRBS Control 1
    uint8_t rsvd_RF10007A8[88];                                                  ///< 0xF10007A8 - 0xF10007FF : Reserved.
    McTrustzonRng0Low_t mcTrustzonRng0Low;                                       ///< 0xF1000800 : MC Trustzone Range 0 Low
    McTrustzonRng0High_t mcTrustzonRng0High;                                     ///< 0xF1000804 : MC Trustzone Range 0 High
    McTrustzonRng1Low_t mcTrustzonRng1Low;                                       ///< 0xF1000808 : MC Trustzone Range 1 Low
    McTrustzonRng1High_t mcTrustzonRng1High;                                     ///< 0xF100080C : MC Trustzone Range 1 High
    uint8_t rsvd_RF1000810[104];                                                 ///< 0xF1000810 - 0xF1000877 : Reserved.
    McTrustzonRng15Low_t mcTrustzonRng15Low;                                     ///< 0xF1000878 : MC Trustzone Range 15 Low
    McTrustzonRng15High_t mcTrustzonRng15High;                                   ///< 0xF100087C : MC Trustzone Range 15 High
} DdrMc_t;
COMPILE_ASSERT(offsetof(DdrMc_t, mcTrustzonRng15High) == 0xF100087C - 0xF1000000, "check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile DdrMc_t rDdrMc;     ///< 0xF1000000-0xF1000880 : DdrMc_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
extern DdrMc_t mDdrMc;              ///< 0xF1000000-0xF1000880 : DdrMc_t MODULE


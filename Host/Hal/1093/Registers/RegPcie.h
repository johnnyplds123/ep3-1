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
//! @brief PCIe Express Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  PCIe Express Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF0000000 : Vendor ID and Device ID
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VEND_ID :16;         ///<BIT [15:0] Vendor ID.
        uint32_t DEV_ID  :16;         ///<BIT [31:16] Device ID.
    } b;    
} VendorIdAndDeviceId_t;

///  @brief 0xF0000004 : Command and Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IO_ACCESS_EN                      : 1;         ///<BIT [0] IO Access Enable.
        uint32_t MEM_ACCESS_EN                     : 1;         ///<BIT [1] Memory Access Enable.
        uint32_t MEM_IO_REQ                        : 1;         ///<BIT [2] Memory IO Request.
        uint32_t PCI_TYPE0_SPECIAL_CYCLE_OPERATION : 1;         ///<BIT [3] Special Cycle Operation.
        uint32_t PCI_TYPE_MWI_ENABLE               : 1;         ///<BIT [4] MWI Enable.
        uint32_t PCI_TYPE_VGA_PALETTE_SNOOP        : 1;         ///<BIT [5] VGA Palette Snoop.
        uint32_t MASTER_DATA_PAR_ERR               : 1;         ///<BIT [6] Master Data Parity Error.
        uint32_t RF0000004_7_RSVD                  : 1;         ///<BIT [7] Reserved.
        uint32_t ERR_REPORTING_EN                  : 1;         ///<BIT [8] Error Reporting Enable.
        uint32_t RF0000004_9_RSVD                  : 1;         ///<BIT [9] Reserved.
        uint32_t IMD                               : 1;         ///<BIT [10] INTx Message Disable.
        uint32_t RF0000004_15_11_RSVD              : 5;         ///<BIT [15:11] Reserved.
        uint32_t RF0000004_18_16_RSVD              : 3;         ///<BIT [18:16] Reserved.
        uint32_t IS                                : 1;         ///<BIT [19] Interrupt Status.
        uint32_t PCIE_EXT_CAP                      : 1;         ///<BIT [20] PCIe Extended Capability.
        uint32_t RF0000004_23_21_RSVD              : 3;         ///<BIT [23:21] Reserved.
        uint32_t MDPE                              : 1;         ///<BIT [24] Master Data Parity Error.
        uint32_t DEV_SEL_TIMING                    : 2;         ///<BIT [26:25] Device Select Timing.
        uint32_t SENT_ABRT_STS                     : 1;         ///<BIT [27] Sent Completion Abort Status.
        uint32_t RCVD_CMPLTN_ABRT_STS              : 1;         ///<BIT [28] Received Completion Abort Status.
        uint32_t RCVD_CMPLTN_UNSUPP_REQ_STS        : 1;         ///<BIT [29] Received Completion Unsupported Request Status.
        uint32_t MULT_ERR_FAT_NFAT_REC             : 1;         ///<BIT [30] Multiple Error Fatal/Non-Fatal Received.
        uint32_t DEV_PAR_ERR                       : 1;         ///<BIT [31] Device Parity Error.
    } b;    
} CmdAndStat_t;

///  @brief 0xF0000008 : Revision ID and Class Code
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t REV_ID     : 8;         ///<BIT [7:0] Revision ID.
        uint32_t REG_LAYOUT : 8;         ///<BIT [15:8] Register Layout.
        uint32_t SUB_FUNC   : 8;         ///<BIT [23:16] Sub-Function.
        uint32_t DEV_FUNC   : 8;         ///<BIT [31:24] Device Function.
    } b;    
} RevisionIdAndClassCmem_t;

///  @brief 0xF000000C : BIST, Header Type, Latency Timer and Cache Line Size
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CACHE_LINE_SIZE : 8;         ///<BIT [7:0] Cache Line Size.
        uint32_t LAT_TIMER       : 8;         ///<BIT [15:8] Latency Timer.
        uint32_t HDR_TYPE        : 7;         ///<BIT [22:16] Header Type.
        uint32_t NUM_FUNC_SUPP   : 1;         ///<BIT [23] Number of Functions Supported.
        uint32_t BIST_CTRL       : 8;         ///<BIT [31:24] BIST Control.
    } b;    
} BistHdrTypeLatencyTmrAndCacheLine_t;

///  @brief 0xF0000010 : Base Address 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ADDR_TYP_0       : 1;         ///<BIT [0] Address Range Type 0.
        uint32_t BAR0_TYPE        : 2;         ///<BIT [2:1] BAR0 Type.
        uint32_t PREFETCH_0       : 1;         ///<BIT [3] Prefetch 0.
        uint32_t BASE_MEM_ADDR_0  :28;         ///<BIT [31:4] Base Address 0.
    } b;    
} BaseAddr0_t;

///  @brief 0xF0000014 : Base Address 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ADDR_TYP_1       : 1;         ///<BIT [0] Address Range Type 1.
        uint32_t BAR1_TYPE        : 2;         ///<BIT [2:1] BAR1 Type.
        uint32_t PREFETCH_1       : 1;         ///<BIT [3] Prefetch 1.
        uint32_t BASE_MEM_ADDR_1  :28;         ///<BIT [31:4] Base Address 1.
    } b;    
} BaseAddr1_t;

///  @brief 0xF0000018 : Base Address 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ADDR_TYP_2       : 1;         ///<BIT [0] Address Range Type 2.
        uint32_t BAR2_TYPE        : 2;         ///<BIT [2:1] BAR2 Type.
        uint32_t PREFETCH_2       : 1;         ///<BIT [3] Prefetch 2.
        uint32_t BASE_MEM_ADDR_2  :28;         ///<BIT [31:4] Base Address 2.
    } b;    
} BaseAddr2_t;

///  @brief 0xF000001C : Base Address 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ADDR_TYP_3       : 1;         ///<BIT [0] Address Range Type 3.
        uint32_t BAR3_TYPE        : 2;         ///<BIT [2:1] BAR3 Type.
        uint32_t PREFETCH_3       : 1;         ///<BIT [3] Prefetch 3.
        uint32_t BASE_MEM_ADDR_3  :28;         ///<BIT [31:4] Base Address 3.
    } b;    
} BaseAddr3_t;

///  @brief 0xF0000020 : Base Address 4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ADDR_TYP_4       : 1;         ///<BIT [0] Address Range Type 4.
        uint32_t BAR4_TYPE        : 2;         ///<BIT [2:1] BAR4 Type.
        uint32_t PREFETCH_4       : 1;         ///<BIT [3] Prefetch 4.
        uint32_t BASE_MEM_ADDR_4  :28;         ///<BIT [31:4] Base Address 4.
    } b;    
} BaseAddr4_t;

///  @brief 0xF0000024 : Base Address 5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ADDR_TYP_5       : 1;         ///<BIT [0] Address Range Type 5.
        uint32_t BAR5_TYPE        : 2;         ///<BIT [2:1] BAR5 Type.
        uint32_t PREFETCH_5       : 1;         ///<BIT [3] Prefetch 5.
        uint32_t BASE_MEM_ADDR_5  :28;         ///<BIT [31:4] Base Address 5.
    } b;    
} BaseAddr5_t;

///  @brief 0xF000002C : Subsystem Vendor ID and Subsystem ID
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SUBSYS_VEND_ID :16;         ///<BIT [15:0] Sub-system Vendor ID.
        uint32_t SUBSYS_ID      :16;         ///<BIT [31:16] Sub-system ID.
    } b;    
} SubsysteVendorIdAndSubsysteId_t;

///  @brief 0xF0000030 : Expansion ROM Base Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EXPAND_ROM_EN       : 1;         ///<BIT [0] Expansion ROM Enable.
        uint32_t RF0000030_10_1_RSVD :10;         ///<BIT [10:1] Reserved.
        uint32_t ROM_BASE_ADDR       :21;         ///<BIT [31:11] ROM Base Address and Range.
    } b;    
} ExpansioRomBaseAddr_t;

///  @brief 0xF0000034 : Capabilities Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_POINTR          : 8;         ///<BIT [7:0] Capability Structure Pointer.
        uint32_t RF0000034_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} CapabiliPtr_t;

///  @brief 0xF000003C : Interrupt Line and Interrupt Pin
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INT_LINE             : 8;         ///<BIT [7:0] Interrupt Line.
        uint32_t INT_POINTR           : 8;         ///<BIT [15:8] Interrupt Pointer.
        uint32_t RF000003C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} IntrLineAndIntrPin_t;

///  @brief 0xF0000040 : Power Management Capabilities
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PM_CAP_ID         : 8;         ///<BIT [7:0] PM_CAP_ID.
        uint32_t NEXT_PTR          : 8;         ///<BIT [15:8] NEXT_PTR.
        uint32_t PM_SPEC_VER       : 3;         ///<BIT [18:16] PM_SPEC_VER.
        uint32_t PME_CLK           : 1;         ///<BIT [19] PME_CLK.
        uint32_t RF0000040_20_RSVD : 1;         ///<BIT [20] Reserved.
        uint32_t DSI               : 1;         ///<BIT [21] Device-Specific Initialization Bit.
        uint32_t AUX_CURR          : 3;         ///<BIT [24:22] AUX_CURR.
        uint32_t D1_SUPPORT        : 1;         ///<BIT [25] PM State D1 Support.
        uint32_t D2_SUPPORT        : 1;         ///<BIT [26] PM State D2 Support.
        uint32_t PME_SUPPORT       : 5;         ///<BIT [31:27] PME Support.
    } b;    
} PowerMngCapabili_t;

///  @brief 0xF0000044 : Power Management Control And Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t POWER_STATE          : 2;         ///<BIT [1:0] POWER_STATE.
        uint32_t RF0000044_2_2_RSVD   : 1;         ///<BIT [2] Reserved.
        uint32_t NSFRST               : 1;         ///<BIT [3] No Soft Reset
        uint32_t RF0000044_7_4_RSVD   : 4;         ///<BIT [7:4] Reserved.
        uint32_t PME_ENABLE           : 1;         ///<BIT [8] PME_ENABLE.
        uint32_t DATA_SELECT          : 4;         ///<BIT [12:9] DATA_SELECT.
        uint32_t DATA_SCALE           : 2;         ///<BIT [14:13] DATA_SCALE.
        uint32_t PME_STATUS           : 1;         ///<BIT [15] PME_STATUS.
        uint32_t RF0000044_21_16_RSVD : 6;         ///<BIT [21:16] Reserved.
        uint32_t B2_B3_SUPPORT        : 1;         ///<BIT [22] B2_B3_SUPPORT.
        uint32_t BUS_PWR_CLK_CON_EN   : 1;         ///<BIT [23] BUS_PWR_CLK_CON_EN.
        uint32_t DATA_REG             : 8;         ///<BIT [31:24] DATA REG.
    } b;    
} PowerMngCtrlAndStat_t;

///  @brief 0xF0000050 : MSI Capability Header
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MSI_CAP_ID           : 8;         ///<BIT [7:0] MSI_CAP_ID.
        uint32_t NEXT_PTR             : 8;         ///<BIT [15:8] NEXT_PTR.
        uint32_t MSI_EN               : 1;         ///<BIT [16] MSI_EN.
        uint32_t MM_CAP               : 3;         ///<BIT [19:17] MM_CAP.
        uint32_t MM_EN                : 3;         ///<BIT [22:20] MM_EN.
        uint32_t A64B_CAP             : 1;         ///<BIT [23] 64B_CAP.
        uint32_t PVM_CAP              : 1;         ///<BIT [24] PVM_CAP.
        uint32_t RF0000050_31_25_RSVD : 7;         ///<BIT [31:25] Reserved.
    } b;    
} MsiCapabiliHdr_t;

///  @brief 0xF0000054 : MSI Capability Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0000054_1_0_RSVD : 2;         ///<BIT [1:0] Reserved.
        uint32_t MSI_OFF_04         :30;         ///<BIT [31:2] MSI_OFF_04.
    } b;    
} MsiCapabiliAddr_t;

///  @brief 0xF000005C : MSI Capability Data
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MSI_OFF_0C           :16;         ///<BIT [15:0] MSI_OFF_0C.
        uint32_t RF000005C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} MsiCapabiliData_t;


///  @brief 0xF0000070 : PCI Express Capabilities
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_ID               : 8;         ///<BIT [7:0] CAP_ID.
        uint32_t NEXT_PTR             : 8;         ///<BIT [15:8] NEXT_PTR.
        uint32_t CAP_VER              : 4;         ///<BIT [19:16] CAP_VER.
        uint32_t DEV_PORT_TYPE        : 4;         ///<BIT [23:20] DEV_PORT_TYPE.
        uint32_t SLOT_IMPL            : 1;         ///<BIT [24] SLOT_IMPL.
        uint32_t INT_MSG_NUM          : 5;         ///<BIT [29:25] INT_MSG_NUM.
        uint32_t RF0000070_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;    
} PciExpressCapabili_t;

///  @brief 0xF0000074 : Device Capabilities
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MAX_PAYLOAD          : 3;         ///<BIT [2:0] MAX_PAYLOAD.
        uint32_t PHANTOM_FUNC_SUPPORT : 2;         ///<BIT [4:3] PHANTOM_FUNC_SUPPORT.
        uint32_t EXTENDED_TAG         : 1;         ///<BIT [5] EXTENDED_TAG.
        uint32_t L0S_ACC_LATENCY      : 3;         ///<BIT [8:6] L0S_ACC_LATENCY.
        uint32_t L1_ACC_LATENCY       : 3;         ///<BIT [11:9] L1_ACC_LATENCY.
        uint32_t RF0000074_14_12_RSVD : 3;         ///<BIT [14:12] Reserved.
        uint32_t ROLE_BASED_ERR_RPT   : 1;         ///<BIT [15] ROLE_BASED_ERR_RPT.
        uint32_t RF0000074_27_16_RSVD :12;         ///<BIT [27:16] Reserved.
        uint32_t FLR_CAP              : 1;         ///<BIT [28] FLR_CAP.
        uint32_t RF0000074_31_29_RSVD : 3;         ///<BIT [31:29] Reserved.
    } b;    
} DeviceCapabili_t;

///  @brief 0xF0000078 : Device Control And Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CORR_ERR_RPT_EN      : 1;         ///<BIT [0] CORR_ERR_RPT_EN.
        uint32_t NON_FATAL_ERR_RPT_EN : 1;         ///<BIT [1] NON_FATAL_ERR_RPT_EN.
        uint32_t FATAL_ERR_RPT_EN     : 1;         ///<BIT [2] FATAL_ERR_RPT_EN.
        uint32_t UR_RPT_EN            : 1;         ///<BIT [3] UR_RPT_EN.
        uint32_t REL_ORDER_EN         : 1;         ///<BIT [4] REL_ORDER_EN.
        uint32_t MAX_PAYLOAD_SZ       : 3;         ///<BIT [7:5] MAX_PAYLOAD_SZ.
        uint32_t EXT_TAG_EN           : 1;         ///<BIT [8] EXT_TAG_EN.
        uint32_t PHANTOM_FUNC_EN      : 1;         ///<BIT [9] PHANTOM_FUNC_EN.
        uint32_t AUX_PWR_PM_EN        : 1;         ///<BIT [10] AUX_PWR_PM_EN.
        uint32_t EN_NO_SNOOP          : 1;         ///<BIT [11] EN_NO_SNOOP.
        uint32_t MAX_RD_SEQ_SZ        : 3;         ///<BIT [14:12] MAX_RD_SEQ_SZ.
        uint32_t INITIATE_FLR         : 1;         ///<BIT [15] INITIATE_FLR.
        uint32_t CORR_ERR_DET         : 1;         ///<BIT [16] CORR_ERR_DET.
        uint32_t NON_FATAL_ERR_DET    : 1;         ///<BIT [17] NON_FATAL_ERR_DET.
        uint32_t FATAL_ERR_DET        : 1;         ///<BIT [18] FATAL_ERR_DET.
        uint32_t UR_DET               : 1;         ///<BIT [19] UR_DET.
        uint32_t AUX_PWR_DET          : 1;         ///<BIT [20] AUX_PWR_DET.
        uint32_t TRANS_PEND           : 1;         ///<BIT [21] TRANS_PEND.
        uint32_t RF0000078_31_22_RSVD :10;         ///<BIT [31:22] Reserved.
    } b;    
} DeviceCtrlAndStat_t;

///  @brief 0xF000007C : Link Capabilities
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MAX_LINK_SPD      : 4;         ///<BIT [3:0] MAX_LINK_SPD.
        uint32_t MAX_LINK_WIDTH    : 6;         ///<BIT [9:4] MAX_LINK_WIDTH.
        uint32_t ASPM_SUPT         : 2;         ///<BIT [11:10] ASPM_SUPT.
        uint32_t L0S_EXIT_LTC      : 3;         ///<BIT [14:12] L0S_EXIT_LTC.
        uint32_t L1_EXIT_LTC       : 3;         ///<BIT [17:15] L1_EXIT_LTC.
        uint32_t CLK_PM            : 1;         ///<BIT [18] CLK_PM.
        uint32_t SUPR_DOWN_ERR_RPT : 1;         ///<BIT [19] SUPR_DOWN_ERR_RPT.
        uint32_t DLL_ACTIVE_REP    : 1;         ///<BIT [20] DLL_ACTIVE_REP.
        uint32_t LINK_BW_NTF       : 1;         ///<BIT [21] LINK_BW_NTF.
        uint32_t ASPM_OPT_CMPL     : 1;         ///<BIT [22] ASPM_OPT_CMPL.
        uint32_t RF000007C_23_RSVD : 1;         ///<BIT [23] Reserved.
        uint32_t PORT_NUM          : 8;         ///<BIT [31:24] PORT_NUM.
    } b;    
} LnkCapabili_t;

///  @brief 0xF0000080 : Link Control And Link Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ASPM_CTRL            : 2;         ///<BIT [1:0] ASPM_CTRL.
        uint32_t RF0000080_2_RSVD     : 1;         ///<BIT [2] Reserved.
        uint32_t RCB                  : 1;         ///<BIT [3] RCB.
        uint32_t LINK_DIS             : 1;         ///<BIT [4] LINK_DIS.
        uint32_t RETRAIN_LINK         : 1;         ///<BIT [5] RETRAIN_LINK.
        uint32_t COMMON_CLK_CFG       : 1;         ///<BIT [6] COMMON_CLK_CFG.
        uint32_t EXT_SYNCH            : 1;         ///<BIT [7] EXT_SYNCH.
        uint32_t EN_CLK_PM            : 1;         ///<BIT [8] EN_CLK_PM.
        uint32_t RF0000080_9_RSVD     : 1;         ///<BIT [9] Reserved.
        uint32_t LINK_BW_IE           : 1;         ///<BIT [10] LINK_BW_IE.
        uint32_t LINK_AUTO_BW_IE      : 1;         ///<BIT [11] LINK_AUTO_BW_IE.
        uint32_t RF0000080_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
        uint32_t LINK_SPEED           : 4;         ///<BIT [19:16] LINK_SPEED.
        uint32_t NEGO_LW              : 5;         ///<BIT [24:20] NEGO_LW.
        uint32_t RF0000080_26_25_RSVD : 2;         ///<BIT [26:25] Reserved.
        uint32_t LINK_TRAINING        : 1;         ///<BIT [27] LINK_TRAINING.
        uint32_t SLOT_CLK_CFG         : 1;         ///<BIT [28] SLOT_CLK_CFG.
        uint32_t DLL_ACTIVE           : 1;         ///<BIT [29] DLL_ACTIVE.
        uint32_t LINK_BW_STS          : 1;         ///<BIT [30] LINK_BW_STS.
        uint32_t LINK_AUTO_BW_STS     : 1;         ///<BIT [31] LINK_AUTO_BW_STS.
    } b;    
} LnkCtrlAndLnkStat_t;

///  @brief 0xF0000094 : Device Capabilities 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPL_TO_RANGE         : 4;         ///<BIT [3:0] CPL_TO_RANGE.
        uint32_t CPL_TO_DIS_UPT       : 1;         ///<BIT [4] CPL_TO_DIS_UPT.
        uint32_t ARI_FWD_SUPT         : 1;         ///<BIT [5] ARI_FWD_SUPT.
        uint32_t ATM_RT_SUPT          : 1;         ///<BIT [6] ATM_RT_SUPT.
        uint32_t ATM_32_CPL_SUPT      : 1;         ///<BIT [7] ATM_32_CPL_SUPT.
        uint32_t ATM_64_CPL_SUPT      : 1;         ///<BIT [8] ATM_64_CPL_SUPT.
        uint32_t CAS_128_CPL_SUPT     : 1;         ///<BIT [9] CAS_128_CPL_SUPT.
        uint32_t NO_RO_EN_PR2PR_PASS  : 1;         ///<BIT [10] NO_RO_EN_PR2PR_PASS.
        uint32_t LTR_SUPT             : 1;         ///<BIT [11] LTR_SUPT.
        uint32_t TPH_CPL_SUPT         : 2;         ///<BIT [13:12] TPH_CPL_SUPT.
        uint32_t RF0000094_17_14_RSVD : 4;         ///<BIT [17:14] Reserved.
        uint32_t OBFF_SUPT            : 2;         ///<BIT [19:18] OBFF_SUPT.
        uint32_t EXT_FMT_SUPT         : 1;         ///<BIT [20] EXT_FMT_SUPT.
        uint32_t E2E_TP_SUPT          : 1;         ///<BIT [21] E2E_TP_SUPT.
        uint32_t MAX_E2E_TP           : 2;         ///<BIT [23:22] MAX_E2E_TP.
        uint32_t RF0000094_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} DeviceCapabili2_t;

///  @brief 0xF0000098 : Device Control 2 And Status 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPL_TO_VAL           : 4;         ///<BIT [3:0] CPL_TO_VAL.
        uint32_t CPL_TO_DIS           : 1;         ///<BIT [4] CPL_TO_DIS.
        uint32_t ARI_FWD_EN           : 1;         ///<BIT [5] ARI_FWD_EN.
        uint32_t RF0000098_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint32_t IDO_REQ_EN           : 1;         ///<BIT [8] IDO_REQ_EN.
        uint32_t IPO_CPL_EN           : 1;         ///<BIT [9] IPO_CPL_EN.
        uint32_t LTR_EN               : 1;         ///<BIT [10] LTR_EN.
        uint32_t RF0000098_12_11_RSVD : 2;         ///<BIT [12:11] Reserved.
        uint32_t OBFF_EN              : 2;         ///<BIT [14:13] OBFF_EN.
        uint32_t CFG_E2E_TP_BLOCK     : 1;         ///<BIT [15] CFG_E2E_TP_BLOCK.
        uint32_t RF0000098_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} DeviceCtrl2AndStat2_t;

///  @brief 0xF000009C : Link Capabilities 2 Register
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF000009C_0_RSVD    : 1;         ///<BIT [0] Reserved.
        uint32_t LINK_SPD_V          : 7;         ///<BIT [7:1] LINK_SPD_V.
        uint32_t XLINK_SUPT          : 1;         ///<BIT [8] XLINK_SUPT.
        uint32_t RF000009C_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;    
} LnkCapabili2Register_t;

///  @brief 0xF00000A0 : Link Control 2 And Link Status 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TRGT_LINK_SPD        : 4;         ///<BIT [3:0] TRGT_LINK_SPD.
        uint32_t ENTER_CMPL           : 1;         ///<BIT [4] ENTER_CMPL.
        uint32_t HW_AUTO_SPD_DIS      : 1;         ///<BIT [5] HW_AUTO_SPD_DIS.
        uint32_t SEL_DEEMPH           : 1;         ///<BIT [6] SEL_DEEMPH.
        uint32_t TX_MARGIN            : 3;         ///<BIT [9:7] TX_MARGIN.
        uint32_t ENTER_MOD_CMPL       : 1;         ///<BIT [10] ENTER_MOD_CMPL.
        uint32_t CMPL_SOS             : 1;         ///<BIT [11] CMPL_SOS.
        uint32_t CMPL_PRESET          : 4;         ///<BIT [15:12] CMPL_PRESET.
        uint32_t CUR_DEEMPH           : 1;         ///<BIT [16] CUR_DEEMPH.
        uint32_t EQ_CPL               : 1;         ///<BIT [17] EQ_CPL.
        uint32_t EQP1_CPL             : 1;         ///<BIT [18] EQP1_CPL.
        uint32_t EQP2_CPL             : 1;         ///<BIT [19] EQP2_CPL.
        uint32_t EQP3_CPL             : 1;         ///<BIT [20] EQP3_CPL.
        uint32_t LINK_EQ_REQ          : 1;         ///<BIT [21] LINK_EQ_REQ.
        uint32_t RF00000A0_31_22_RSVD :10;         ///<BIT [31:22] Reserved.
    } b;    
} LnkCtrl2AndLnkStat2_t;

///  @brief 0xF00000B0 : MSI-X Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_ID               : 8;         ///<BIT [7:0] Capability ID.
        uint32_t CAP_POINTR           : 8;         ///<BIT [15:8] Capability Pointer.
        uint32_t MSI_X_TABLE_SIZE     :11;         ///<BIT [26:16] MSI-X Table Size.
        uint32_t RF00000B0_29_27_RSVD : 3;         ///<BIT [29:27] Reserved.
        uint32_t GLOBAL_MSK_INTR      : 1;         ///<BIT [30] Global Mask to All Interrupts.
        uint32_t MSI_X_EN             : 1;         ///<BIT [31] MSI-X Enable.
    } b;    
} MsiXCfg_t;

///  @brief 0xF00000B4 : MSI-X Table Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BASE_ADDR_MSI_X : 3;         ///<BIT [2:0] MSI-X Base Address.
        uint32_t TABLE_OFFSET    :29;         ///<BIT [31:3] Table Offset.
    } b;    
} MsiXTblAddr_t;

///  @brief 0xF00000B8 : MSI-X Pending Bit Array
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BASE_ADDR_PBA : 3;         ///<BIT [2:0] PBA Base Address.
        uint32_t PBA_OFFSET    :29;         ///<BIT [31:3] Pending Bit Array Offset.
    } b;    
} MsiXPendingBitArray_t;

///  @brief 0xF0000100 : Advanced Error Reporting (AER) Enhanced Capability Header
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_ID_AER      :16;         ///<BIT [15:0] Capability ID AER Extended Capability.
        uint32_t CAP_VAL         : 4;         ///<BIT [19:16] Capability Value.
        uint32_t NEXT_CAP_OFFSET :12;         ///<BIT [31:20] Next Capability Offset.
    } b;    
} AdvancedErrReportinAerEnhancedCapabiliHdr_t;

///  @brief 0xF0000104 : Uncorrectable Error Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0000104_3_0_RSVD   : 4;         ///<BIT [3:0] Reserved.
        uint32_t DATA_LINK_PRTCL_ERR  : 1;         ///<BIT [4] Data Link Protocol Error.
        uint32_t SUR_DN_ERR           : 1;         ///<BIT [5] Surprise Down Error Status.
        uint32_t RF0000104_11_6_RSVD  : 6;         ///<BIT [11:6] Reserved.
        uint32_t POISONED_TLP         : 1;         ///<BIT [12] Poisoned TLP.
        uint32_t FLOW_CTRL_PROTOCOL   : 1;         ///<BIT [13] Flow Control Protocol.
        uint32_t COMPLT_TIMER_TIMEOUT : 1;         ///<BIT [14] Completion Timer Timeout.
        uint32_t COMP_ABRT_STS        : 1;         ///<BIT [15] Completer Abort Status.
        uint32_t UNEXPCT_COMP_PACKT   : 1;         ///<BIT [16] Unexpected Completion Packet.
        uint32_t REC_CRDT_VIOLATN     : 1;         ///<BIT [17] Receive Credit Violation.
        uint32_t MAL_FORMED_TLP       : 1;         ///<BIT [18] Malformed TLP.
        uint32_t ECRC_ERR             : 1;         ///<BIT [19] ECRC Error.
        uint32_t REQ_NON_SUPP_LINK    : 1;         ///<BIT [20] Request From a Non-supported Link.
        uint32_t RF0000104_21_RSVD    : 1;         ///<BIT [21] Reserved.
        uint32_t UNCORR_INT           : 1;         ///<BIT [22] Uncorrectable Internal Error.
        uint32_t MC_BLCK_TLP          : 1;         ///<BIT [23] MC_BLCK_TLP.
        uint32_t ATO_EGR_BLCK         : 1;         ///<BIT [24] ATO_EGR_BLCK.
        uint32_t TP_BLCK_ERR          : 1;         ///<BIT [25] TP_BLCK_ERR.
        uint32_t RF0000104_31_26_RSVD : 6;         ///<BIT [31:26] Reserved.
    } b;    
} UncorrecErrStat_t;

///  @brief 0xF0000108 : Uncorrectable Error Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0000108_3_0_RSVD        : 4;         ///<BIT [3:0] Reserved.
        uint32_t DATA_LINK_PRTCL_ERR_MASK  : 1;         ///<BIT [4] Data Link Protocol Error Mask.
        uint32_t SUR_DN_ERR_MASK           : 1;         ///<BIT [5] Surprise Down Error Mask.
        uint32_t RF0000108_11_6_RSVD       : 6;         ///<BIT [11:6] Reserved.
        uint32_t POISONED_TLP_MASK         : 1;         ///<BIT [12] Poisoned TLP Mask.
        uint32_t FLOW_CTRL_PROTOCOL_MASK   : 1;         ///<BIT [13] Flow Control Protocol Mask.
        uint32_t COMPLT_TIMER_TIMEOUT_MASK : 1;         ///<BIT [14] Completion Timer Timeout Mask.
        uint32_t COMP_ABRT_STS_MASK        : 1;         ///<BIT [15] Completer Abort Status Mask.
        uint32_t UNEXPCT_COMP_PACKT_MASK   : 1;         ///<BIT [16] Unexpected Completion Packet Mask.
        uint32_t REC_CRDT_VIOLATN_MASK     : 1;         ///<BIT [17] Receive Credit Violation Mask.
        uint32_t MAL_FORMED_TLP_MASK       : 1;         ///<BIT [18] Mal-formed TLP Mask.
        uint32_t ECRC_ERR_MASK             : 1;         ///<BIT [19] ECRC Error Mask.
        uint32_t REQ_NON_SUPP_LINK_MASK    : 1;         ///<BIT [20] Request From A Non-supported Link Mask.
        uint32_t RF0000108_21_RSVD         : 1;         ///<BIT [21] Reserved.
        uint32_t UNCORR_INT_MASK           : 1;         ///<BIT [22] Uncorrectable Internal Error Mask.
        uint32_t MC_BLCK_TLP_MASK          : 1;         ///<BIT [23] MC_BLCK_TLP Mask.
        uint32_t ATO_EGR_BLCK_MASK         : 1;         ///<BIT [24] ATO_EGR_BLCK Mask.
        uint32_t TP_BLCK_ERR_MASK          : 1;         ///<BIT [25] TP_BLCK_ERR Mask.
        uint32_t RF0000108_31_26_RSVD      : 6;         ///<BIT [31:26] Reserved.
    } b;    
} UncorrecErrMsk_t;

///  @brief 0xF000010C : Uncorrectable Error Severity
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF000010C_3_0_RSVD            : 4;         ///<BIT [3:0] Reserved.
        uint32_t DATA_LINK_PRTCL_ERR_SEVERITY  : 1;         ///<BIT [4] Data Link Protocol Error Severity.
        uint32_t SUR_DN_SEV                    : 1;         ///<BIT [5] Surprise Down Error Severity.
        uint32_t RF000010C_11_6_RSVD           : 6;         ///<BIT [11:6] Reserved.
        uint32_t POISONED_TLP_SEVERITY         : 1;         ///<BIT [12] Poisoned TLP Severity.
        uint32_t FLOW_CTRL_PROTOCOL_SEVERITY   : 1;         ///<BIT [13] Flow Control Protocol Severity.
        uint32_t COMPLT_TIMER_TIMEOUT_SEVERITY : 1;         ///<BIT [14] Completion Timer Timeout Severity.
        uint32_t COMP_ABRT_STS_SEVERITY        : 1;         ///<BIT [15] Completer Abort Status Severity.
        uint32_t UNEXPCT_COMP_PACKT_SEVERITY   : 1;         ///<BIT [16] Unexpected Completion Packet Severity.
        uint32_t REC_CRDT_VIOLATN_SEVERITY     : 1;         ///<BIT [17] Receive Credit Violation Severity.
        uint32_t MAL_FORMED_TLP_SEVERITY       : 1;         ///<BIT [18] Mal-formed TLP Severity.
        uint32_t ECRC_ERR_SEVERITY             : 1;         ///<BIT [19] ECRC Error Severity.
        uint32_t UNEXPCT_RSLT_SEVERITY         : 1;         ///<BIT [20] Unexpected Result Severity.
        uint32_t RF000010C_21_RSVD             : 1;         ///<BIT [21] Reserved.
        uint32_t INT_ERR_SEVERITY              : 1;         ///<BIT [22] Internal Error Severity.
        uint32_t RF000010C_23_RSVD             : 1;         ///<BIT [23] Reserved.
        uint32_t ATO_EGR_BLCK_SEVERITY         : 1;         ///<BIT [24] ATO_EGR_BLCK Severity.
        uint32_t TP_BLCK_ERR_SEVERITY          : 1;         ///<BIT [25] TP_BLCK_ERR Severity.
        uint32_t RF000010C_31_26_RSVD          : 6;         ///<BIT [31:26] Reserved.
    } b;    
} UncorrecErrSeverity_t;

///  @brief 0xF0000110 : Correctable Error Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RCV_ERR                : 1;         ///<BIT [0] Receive-side Error.
        uint32_t RF0000110_5_1_RSVD     : 5;         ///<BIT [5:1] Reserved.
        uint32_t TLP_ERR                : 1;         ///<BIT [6] TLP Error.
        uint32_t LCRC_ERR               : 1;         ///<BIT [7] LCRC Error in DLLP.
        uint32_t REPLAY_CNT_RLLOVR_ERR  : 1;         ///<BIT [8] Replay Count Rollover Error.
        uint32_t RF0000110_11_9_RSVD    : 3;         ///<BIT [11:9] Reserved.
        uint32_t REPLAY_TMR_TIMEOUT_ERR : 1;         ///<BIT [12] Replay Timer Timeout.
        uint32_t UNCORR_ERR             : 1;         ///<BIT [13] Uncorrectable Error.
        uint32_t INT_CORR_ERR           : 1;         ///<BIT [14] Internal Correctable Error.
        uint32_t RF0000110_31_15_RSVD   :17;         ///<BIT [31:15] Reserved.
    } b;    
} CorrectaErrStat_t;

///  @brief 0xF0000114 : Correctable Error Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RCV_ERR_MESS_MASK                : 1;         ///<BIT [0] Receive-side Error Message Mask.
        uint32_t RF0000114_5_1_RSVD               : 5;         ///<BIT [5:1] Reserved.
        uint32_t TLP_ERR_MESS_MASK                : 1;         ///<BIT [6] TLP Error Message Mask.
        uint32_t LCRC_ERR_MESS_MASK               : 1;         ///<BIT [7] LCRC Error Message Mask.
        uint32_t REPLAY_CNT_RLLOVR_ERR_MASK       : 1;         ///<BIT [8] Replay Count Rollover Error Message Mask.
        uint32_t RF0000114_11_9_RSVD              : 3;         ///<BIT [11:9] Reserved.
        uint32_t REPLAY_TMR_TIMEOUT_ERR_MESS_MASK : 1;         ///<BIT [12] Replay Timer Timeout Error Message Mask.
        uint32_t UNCORR_ERR_MESS_MASK             : 1;         ///<BIT [13] Uncorrectable Error Message Mask.
        uint32_t INT_CORR_ERR_MESS_MASK           : 1;         ///<BIT [14] Internal Correctable Error Message Mask.
        uint32_t RF0000114_31_15_RSVD             :17;         ///<BIT [31:15] Reserved.
    } b;    
} CorrectaErrMsk_t;

///  @brief 0xF0000118 : Advanced Error Capabilities And Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t POINTR_UNCORR_ERR_STS : 5;         ///<BIT [4:0] Pointer to Bit Position in Uncorrectable Status.
        uint32_t ECRC_CHK_TX           : 1;         ///<BIT [5] ECRC in Transmit Packets.
        uint32_t ECRC_GEN_TX_EN        : 1;         ///<BIT [6] ECRC Generation on Transmit-side Enable.
        uint32_t ECRC_CHCK             : 1;         ///<BIT [7] ECRC Checking.
        uint32_t ECRC_CHCK_RCV         : 1;         ///<BIT [8] ECRC Checking on Receive-side.
        uint32_t RF0000118_10_9_RSVD   : 2;         ///<BIT [10:9] Reserved.
        uint32_t TP_LOG_PRSNT          : 1;         ///<BIT [11] TP_LOG_PRSNT.
        uint32_t RF0000118_31_12_RSVD  :20;         ///<BIT [31:12] Reserved.
    } b;    
} AdvancedErrCapabiliAndCtrl_t;

///  @brief 0xF0000148 : Serial Number Extended Capabilities
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_ID      :16;         ///<BIT [15:0] PCI Express Extended Capability ID.
        uint32_t CAP_VER     : 4;         ///<BIT [19:16] Capability Version.
        uint32_t NEXT_OFFSET :12;         ///<BIT [31:20] Next Capability Offset.
    } b;    
} SerialNumExtCapabili_t;

///  @brief 0xF0000158 : Power Budgeting Extended Capability Header
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_ID      :16;         ///<BIT [15:0] PCI Express Extended Capability ID.
        uint32_t CAP_VER     : 4;         ///<BIT [19:16] Capability Version.
        uint32_t NEXT_OFFSET :12;         ///<BIT [31:20] Next Capability Offset.
    } b;    
} PowerBudgetinExtCapabiliHdr_t;

///  @brief 0xF0000160 : Power Budgeting Data
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BASE_PWR             : 8;         ///<BIT [7:0] Base Power.
        uint32_t DATA_SCALE           : 2;         ///<BIT [9:8] Data Scale.
        uint32_t PM_SUB_STATE         : 3;         ///<BIT [12:10] PM Sub State.
        uint32_t PM_STATE             : 2;         ///<BIT [14:13] PM State.
        uint32_t TYPE                 : 3;         ///<BIT [17:15] Type.
        uint32_t PWR_RAIL             : 3;         ///<BIT [20:18] Power Rail.
        uint32_t RF0000160_31_21_RSVD :11;         ///<BIT [31:21] Reserved.
    } b;    
} PowerBudgetinData_t;

///  @brief 0xF0000164 : Power Budget Capability
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SYS_ALLOC           : 1;         ///<BIT [0] System Allocated.
        uint32_t RF0000164_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} PowerBudgetCapabili_t;

///  @brief 0xF0000168 : ARI Capability Header
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_ID      :16;         ///<BIT [15:0] PCI Express Capability ID.
        uint32_t CAP_VER     : 4;         ///<BIT [19:16] Capabilty Version.
        uint32_t NEXT_OFFSET :12;         ///<BIT [31:20] Next Capability Offset.
    } b;    
} AriCapabiliHdr_t;

///  @brief 0xF000016C : ARI Capabilty And Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MFVC_FUNC_GRP_CAP    : 1;         ///<BIT [0] MFVC Function Groups Capability.
        uint32_t ACS_FUNC_GRP_CAP     : 1;         ///<BIT [1] ACS Function Groups Capability.
        uint32_t RF000016C_7_2_RSVD   : 6;         ///<BIT [7:2] Reserved.
        uint32_t NXT_FUNC             : 8;         ///<BIT [15:8] Next Function Number.
        uint32_t MFVC_FUNC_GRP_EN     : 1;         ///<BIT [16] MFVC Function Groups Enable.
        uint32_t ACS_FUNC_GRP_EN      : 1;         ///<BIT [17] ACK Function Groups Enable.
        uint32_t RF000016C_19_18_RSVD : 2;         ///<BIT [19:18] Reserved.
        uint32_t FUNC_GRP             : 3;         ///<BIT [22:20] Function Group.
        uint32_t RF000016C_31_23_RSVD : 9;         ///<BIT [31:23] Reserved.
    } b;    
} AriCapabiltAndCtrl_t;

///  @brief 0xF0000178 : Secondary PCI Express Extended Capability Header
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_ID      :16;         ///<BIT [15:0] PCI Express Extended Capability ID.
        uint32_t CAP_VER     : 4;         ///<BIT [19:16] Capability Version.
        uint32_t NEXT_OFFSET :12;         ///<BIT [31:20] Next Capability Offset.
    } b;    
} SecondaryPciExpressExtCapabiliHdr_t;

///  @brief 0xF000017C : Link Control 3 Register
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PERFORM_EQ          : 1;         ///<BIT [0] Perform Equalization.
        uint32_t EQ_REQ_IE           : 1;         ///<BIT [1] Link Equalization Request Interrupt Enable.
        uint32_t RF000017C_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} LnkCtrl3_t;

///  @brief 0xF0000180 : Lane Error Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LANE_ERR            : 4;         ///<BIT [3:0] Lane Error Status Bits.
        uint32_t RF0000180_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} LaneErrStat_t;

///  @brief 0xF0000184 : Lane Equalization Control 0 And 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DSP_TX_PST0       : 4;         ///<BIT [3:0] Downstream Port Transmitter Preset.
        uint32_t DSP_RX_PH0        : 3;         ///<BIT [6:4] Downstream Port Receiver Preset Hint.
        uint32_t RF0000184_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint32_t USP_TX_PST0       : 4;         ///<BIT [11:8] Upstream Port Transmitter Preset.
        uint32_t USP_RX_PH0        : 3;         ///<BIT [14:12] Upstream Port Receiver Preset Hint.
        uint32_t RF0000184_15_RSVD : 1;         ///<BIT [15] Reserved.
        uint32_t DSP_TX_PST1       : 4;         ///<BIT [19:16] Downstream Port Transmitter Reset.
        uint32_t DSP_RX_PH1        : 3;         ///<BIT [22:20] Downstream Port Receiver Preset Hing.
        uint32_t RF0000184_23_RSVD : 1;         ///<BIT [23] Reserved.
        uint32_t USP_TX_PST1       : 4;         ///<BIT [27:24] Upstream Port Transmitter Preset.
        uint32_t USP_RX_PH1        : 3;         ///<BIT [30:28] Upstream Port Receiver Preset Hint.
        uint32_t RF0000184_31_RSVD : 1;         ///<BIT [31] Reserved.
    } b;    
} LaneEqualizaCtrl0And1_t;

///  @brief 0xF0000188 : Lane Equalization Control 2 And 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DSP_TX_PST2       : 4;         ///<BIT [3:0] Downstream Port Transmitter Preset.
        uint32_t DSP_RX_PH2        : 3;         ///<BIT [6:4] Downstream Port Receiver Preset Hint.
        uint32_t RF0000188_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint32_t USP_TX_PST2       : 4;         ///<BIT [11:8] Upstream Port Transmitter Preset.
        uint32_t USP_RX_PH2        : 3;         ///<BIT [14:12] Upstream Port Receiver Preset Hint.
        uint32_t RF0000188_15_RSVD : 1;         ///<BIT [15] Reserved.
        uint32_t DSP_TX_PST3       : 4;         ///<BIT [19:16] Downstream Port Transmitter Reset.
        uint32_t DSP_RX_PH3        : 3;         ///<BIT [22:20] Downstream Port Receiver Preset Hing.
        uint32_t RF0000188_23_RSVD : 1;         ///<BIT [23] Reserved.
        uint32_t USP_TX_PST3       : 4;         ///<BIT [27:24] Upstream Port Transmitter Preset.
        uint32_t USP_RX_PH3        : 3;         ///<BIT [30:28] Upstream Port Receiver Preset Hint.
        uint32_t RF0000188_31_RSVD : 1;         ///<BIT [31] Reserved.
    } b;    
} LaneEqualizaCtrl2And3_t;

///  @brief 0xF0000198 : SR-IOV Extended Capability Header
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_ID      :16;         ///<BIT [15:0] PCI Express Capability ID.
        uint32_t CAP_VER     : 4;         ///<BIT [19:16] Capabilty Version.
        uint32_t NEXT_OFFSET :12;         ///<BIT [31:20] Next Capability Offset.
    } b;    
} SrIovExtCapabiliHdr_t;

///  @brief 0xF000019C : SR-IOV Capability Field
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VF_MIGR_CAP              : 1;         ///<BIT [0] VF Migration Capable.
        uint32_t ARI_CAP_HP               : 1;         ///<BIT [1] ARI Capable Hierarchy Preserve.
        uint32_t RF000019C_20_2_RSVD      :19;         ///<BIT [20:2] Reserved.
        uint32_t VF_MIGRATION_INT_MSG_NUM :10;         ///<BIT [30:21] VF Migration Interrupt Message Number.
        uint32_t RF000019C_31_RSVD        : 1;         ///<BIT [31] Reserved.
    } b;    
} SrIovCapabiliField_t;

///  @brief 0xF00001A0 : SR-IOV Control And Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VF_EN               : 1;         ///<BIT [0] VF Enable.
        uint32_t VF_MIGR_EN          : 1;         ///<BIT [1] VF Migration Enable.
        uint32_t VF_MIGR_IE          : 1;         ///<BIT [2] VF Migration Interrupt Enable.
        uint32_t VF_MSE              : 1;         ///<BIT [3] VF Memory Space Enable.
        uint32_t ARI_CAP_HIER        : 1;         ///<BIT [4] ARI Capable Hierarchy.
        uint32_t RF00001A0_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} SrIovCtrlAndStat_t;

///  @brief 0xF00001A4 : TotalVFs And InitialVFs
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INITALVFS :16;         ///<BIT [15:0] InitalVFs.
        uint32_t TOTALVFS  :16;         ///<BIT [31:16] TotalVFs.
    } b;    
} TotalvfsAndInitialv00_t;

///  @brief 0xF00001A8 : NumVFs And Function Dependency Link
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t NUMVFS               :16;         ///<BIT [15:0] Num VFs.
        uint32_t FDL                  : 4;         ///<BIT [19:16] Function Dependency Link.
        uint32_t RF00001A8_31_20_RSVD :12;         ///<BIT [31:20] Reserved.
    } b;    
} NumvfsAndFuncDependenLnk_t;

///  @brief 0xF00001AC : VF Stride And Offset
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VF_OFFSET :16;         ///<BIT [15:0] First VF Offset.
        uint32_t VF_STRIDE :16;         ///<BIT [31:16] VF Stride.
    } b;    
} VfStrideAndOs00_t;

///  @brief 0xF00001B0 : VF Device ID
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00001B0_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t VF_DEVID            :16;         ///<BIT [31:16] VF Device ID.
    } b;    
} VfDeviceId_t;

///  @brief 0xF00001BC : VF BAR0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00001BC_0_RSVD : 1;         ///<BIT [0] Reserved.
        uint32_t TYPE             : 2;         ///<BIT [2:1] Type.
        uint32_t PREFETCH         : 1;         ///<BIT [3] Prefetch.
        uint32_t START            :28;         ///<BIT [31:4] Start.
    } b;    
} VfBar0_t;

///  @brief 0xF00001C0 : VF BAR1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00001C0_0_RSVD : 1;         ///<BIT [0] Reserved.
        uint32_t TYPE             : 2;         ///<BIT [2:1] Type.
        uint32_t PREFETCH         : 1;         ///<BIT [3] Prefetch.
        uint32_t START            :28;         ///<BIT [31:4] Start.
    } b;    
} VfBar1_t;

///  @brief 0xF00001C4 : VF BAR2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00001C4_0_RSVD : 1;         ///<BIT [0] Reserved.
        uint32_t TYPE             : 2;         ///<BIT [2:1] Type.
        uint32_t PREFETCH         : 1;         ///<BIT [3] Prefetch.
        uint32_t START            :28;         ///<BIT [31:4] Start.
    } b;    
} VfBar2_t;

///  @brief 0xF00001C8 : VF BAR3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00001C8_0_RSVD : 1;         ///<BIT [0] Reserved.
        uint32_t TYPE             : 2;         ///<BIT [2:1] Type.
        uint32_t PREFETCH         : 1;         ///<BIT [3] Prefetch.
        uint32_t START            :28;         ///<BIT [31:4] Start.
    } b;    
} VfBar3_t;

///  @brief 0xF00001CC : VF BAR4
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00001CC_0_RSVD : 1;         ///<BIT [0] Reserved.
        uint32_t TYPE             : 2;         ///<BIT [2:1] Type.
        uint32_t PREFETCH         : 1;         ///<BIT [3] Prefetch.
        uint32_t START            :28;         ///<BIT [31:4] Start.
    } b;    
} VfBar4_t;

///  @brief 0xF00001D0 : VF BAR5
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00001D0_0_RSVD : 1;         ///<BIT [0] Reserved.
        uint32_t TYPE             : 2;         ///<BIT [2:1] Type.
        uint32_t PREFETCH         : 1;         ///<BIT [3] Prefetch.
        uint32_t START            :28;         ///<BIT [31:4] Start.
    } b;    
} VfBar5_t;

///  @brief 0xF00001D4 : VF Migration State Array Offset
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VF_MIGR_ST_BIR      : 3;         ///<BIT [2:0] VF Migration State BIR.
        uint32_t VF_MIGR_ST_OFF      : 2;         ///<BIT [4:3] VF Migration State Offset.
        uint32_t RF00001D4_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} VfMigratioStateArrayOs_t;

///  @brief 0xF00001D8 : TPH Requester Extended Capability Header
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_ID      :16;         ///<BIT [15:0] PCI Express Capability ID.
        uint32_t CAP_VER     : 4;         ///<BIT [19:16] Capabilty Version.
        uint32_t NEXT_OFFSET :12;         ///<BIT [31:20] Next Capability Offset.
    } b;    
} TphRequesteExtCapabiliHdr_t;

///  @brief 0xF00001DC : TPH Requester Extended Capability
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t NO_ST_MODE           : 1;         ///<BIT [0] No ST Mode Supported.
        uint32_t INT_VEC              : 1;         ///<BIT [1] Interrupt Vector Mode Supported.
        uint32_t DEV_SPEC             : 1;         ///<BIT [2] Device Specific Mode Supported.
        uint32_t RF00001DC_7_3_RSVD   : 5;         ///<BIT [7:3] Reserved.
        uint32_t TPH_SUPT             : 1;         ///<BIT [8] Extended TPH Requester Supported.
        uint32_t ST_TBL_LOC           : 2;         ///<BIT [10:9] ST Table Location.
        uint32_t RF00001DC_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
        uint32_t ST_TBL_SZ            :11;         ///<BIT [26:16] ST Table Size.
        uint32_t RF00001DC_31_27_RSVD : 5;         ///<BIT [31:27] Reserved.
    } b;    
} TphRequesteExtCapabili_t;

///  @brief 0xF00001E0 : TPH Requester Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t STMODE_SEL           : 3;         ///<BIT [2:0] ST Mode Select.
        uint32_t RF00001E0_7_3_RSVD   : 5;         ///<BIT [7:3] Reserved.
        uint32_t REQ_EN               : 2;         ///<BIT [9:8] TPH Requester Enable.
        uint32_t RF00001E0_31_10_RSVD :22;         ///<BIT [31:10] Reserved.
    } b;    
} TphRequesteCtrl_t;

///  @brief 0xF00001E4 : TPH ST Table 00 And 01
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_00            : 8;         ///<BIT [7:0] ST Lower 00.
        uint32_t RF00001E4_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_01            : 8;         ///<BIT [23:16] ST Lower 01.
        uint32_t RF0000204_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl00And01_t;

///  @brief 0xF00001E8 : TPH ST Table 02 And 03
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_02            : 8;         ///<BIT [7:0] ST Lower 02.
        uint32_t RF00001E8_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_03            : 8;         ///<BIT [23:16] ST Lower 03.
        uint32_t RF00001E8_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl02And03_t;

///  @brief 0xF00001EC : TPH ST Table 04 And 05
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_04            : 8;         ///<BIT [7:0] ST Lower 04.
        uint32_t RF00001EC_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_05            : 8;         ///<BIT [23:16] ST Lower 05.
        uint32_t RF00001EC_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl04And05_t;

///  @brief 0xF00001F0 : TPH ST Table 06 And 07
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_06            : 8;         ///<BIT [7:0] ST Lower 06.
        uint32_t RF00001F0_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_07            : 8;         ///<BIT [23:16] ST Lower 07.
        uint32_t RF00001F0_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl06And07_t;

///  @brief 0xF00001F4 : TPH ST Table 08 And 09
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_08            : 8;         ///<BIT [7:0] ST Lower 08.
        uint32_t RF00001F4_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_09            : 8;         ///<BIT [23:16] ST Lower 09.
        uint32_t RF00001F4_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl08And09_t;

///  @brief 0xF00001F8 : TPH ST Table 0A And 0B
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_0A            : 8;         ///<BIT [7:0] ST Lower 0A.
        uint32_t RF00001F8_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_0B            : 8;         ///<BIT [23:16] ST Lower 0B.
        uint32_t RF00001F8_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl0aAnd0b_t;

///  @brief 0xF00001FC : TPH ST Table 0C And 0D
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_0C            : 8;         ///<BIT [7:0] ST Lower 0C.
        uint32_t RF00001FC_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_0D            : 8;         ///<BIT [23:16] ST Lower 0D.
        uint32_t RF00001FC_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl0cAnd0d_t;

///  @brief 0xF0000200 : TPH ST Table 0E And 0F
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_0E            : 8;         ///<BIT [7:0] ST Lower 0E.
        uint32_t RF0000200_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_0F            : 8;         ///<BIT [23:16] ST Lower 0F.
        uint32_t RF0000200_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl0eAnd0f_t;

///  @brief 0xF0000204 : TPH ST Table 10 And 11
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_10            : 8;         ///<BIT [7:0] ST Lower 10.
        uint32_t RF0000204_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_11            : 8;         ///<BIT [23:16] ST Lower 11.
        uint32_t RF0000204_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl10And11_t;

///  @brief 0xF0000208 : TPH ST Table 12 And 13
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_12            : 8;         ///<BIT [7:0] ST Lower 12.
        uint32_t RF0000208_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_13            : 8;         ///<BIT [23:16] ST Lower 13.
        uint32_t RF0000208_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl12And13_t;

///  @brief 0xF000020C : TPH ST Table 14 And 15
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_14            : 8;         ///<BIT [7:0] ST Lower 14.
        uint32_t RF000020C_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_15            : 8;         ///<BIT [23:16] ST Lower 15.
        uint32_t RF000020C_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl14And15_t;

///  @brief 0xF0000210 : TPH ST Table 16 And 17
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_16            : 8;         ///<BIT [7:0] ST Lower 16.
        uint32_t RF0000210_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_17            : 8;         ///<BIT [23:16] ST Lower 17.
        uint32_t RF0000210_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl16And17_t;

///  @brief 0xF0000214 : TPH ST Table 18 And 19
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_18            : 8;         ///<BIT [7:0] ST Lower 18.
        uint32_t RF0000214_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_19            : 8;         ///<BIT [23:16] ST Lower 19.
        uint32_t RF0000214_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl18And19_t;

///  @brief 0xF0000218 : TPH ST Table 1A And 1B
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_1A            : 8;         ///<BIT [7:0] ST Lower 1A.
        uint32_t RF0000218_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_1B            : 8;         ///<BIT [23:16] ST Lower 1B.
        uint32_t RF0000218_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl1aAnd1b_t;

///  @brief 0xF000021C : TPH ST Table 1C And 1D
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_1C            : 8;         ///<BIT [7:0] ST Lower 1C.
        uint32_t RF000021C_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t ST_LWR_1D            : 8;         ///<BIT [23:16] ST Lower 1D.
        uint32_t RF000021C_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TphStTbl1cAnd1d_t;

///  @brief 0xF0000220 : TPH ST Table 1E And 1F
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ST_LWR_1E           : 8;         ///<BIT [7:0] ST Lower 1E.
        uint32_t RF0000220_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} TphStTbl1eAnd1f_t;

///  @brief 0xF00002A8 : Address Translation Service Capability Header
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_ID      :16;         ///<BIT [15:0] PCI Express Capability ID.
        uint32_t CAP_VER     : 4;         ///<BIT [19:16] Capabilty Version.
        uint32_t NEXT_OFFSET :12;         ///<BIT [31:20] Next Capability Offset.
    } b;    
} AddrTranslatServiceCapabiliHdr_t;

///  @brief 0xF00002AC : ATS Capability And Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INVQ_DPTH            : 5;         ///<BIT [4:0] Invalidate Queue Depth.
        uint32_t PG_ALGN_REQ          : 1;         ///<BIT [5] Page Aligned Request.
        uint32_t GBL_INV_SUPT         : 1;         ///<BIT [6] Global Invaldate Supported.
        uint32_t RF00002AC_15_7_RSVD  : 9;         ///<BIT [15:7] Reserved.
        uint32_t STU                  : 5;         ///<BIT [20:16] Smallest Translation Unit.
        uint32_t RF00002AC_30_21_RSVD :10;         ///<BIT [30:21] Reserved.
        uint32_t EN                   : 1;         ///<BIT [31] Enable.
    } b;    
} AtsCapabiliAndCtrl_t;

///  @brief 0xF00002B8 : Latency Tolerance Reporting Extended Capability Header
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_ID      :16;         ///<BIT [15:0] PCI Express Capability ID.
        uint32_t CAP_VER     : 4;         ///<BIT [19:16] Capabilty Version.
        uint32_t NEXT_OFFSET :12;         ///<BIT [31:20] Next Capability Offset.
    } b;    
} LatencyTolReportinExtCapabiliHdr_t;

///  @brief 0xF00002BC : LTR Max Snoop and No-Snoop Latency
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SNP_VAL              :10;         ///<BIT [9:0] Max Snoop Latency Value.
        uint32_t SNP_SCALE            : 3;         ///<BIT [12:10] Max Snoop Latency Scale.
        uint32_t RF00002BC_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t NO_SNP_VAL           :10;         ///<BIT [25:16] Max No-Snoop Latency Value.
        uint32_t NO_SNP_SCALE         : 3;         ///<BIT [28:26] Max No-Snoop Latnecy Scale.
        uint32_t RF00002BC_31_29_RSVD : 3;         ///<BIT [31:29] Reserved.
    } b;    
} LtrMaxSnoopAndNoSnoopLatency_t;

///  @brief 0xF00002C0 : L1 Substates Extended Capability Header
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_ID      :16;         ///<BIT [15:0] PCI Express Capability ID.
        uint32_t CAP_VER     : 4;         ///<BIT [19:16] Capabilty Version.
        uint32_t NEXT_OFFSET :12;         ///<BIT [31:20] Next Capability Offset.
    } b;    
} L1SubstateExtCapabiliHdr_t;

///  @brief 0xF00002C4 : L1 Substate Capability
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PCPM_L12_SUPT        : 1;         ///<BIT [0] PCPM L1.2 Supported.
        uint32_t PCPM_L11_SUPT        : 1;         ///<BIT [1] PCPM L1.1 Supported.
        uint32_t ASPM_L12_SUPT        : 1;         ///<BIT [2] ASPM L1.2 Supported.
        uint32_t ASPM_L11_SUPT        : 1;         ///<BIT [3] ASPM L1.1 Supported.
        uint32_t L1PM_SPUT            : 1;         ///<BIT [4] L1 PM Substates Supported.
        uint32_t RF00002C4_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t CMM_MODE_SUPT        : 8;         ///<BIT [15:8] Port Common Mode Restore Time.
        uint32_t PWRON_SCALE          : 2;         ///<BIT [17:16] T Power On Scale.
        uint32_t RF00002C4_18_RSVD    : 1;         ///<BIT [18] Reserved.
        uint32_t PWRON_VAL            : 5;         ///<BIT [23:19] T Power On.
        uint32_t RF00002C4_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} L1SubstateCapabili_t;

///  @brief 0xF00002C8 : L1 Substates Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PCIPM_L12_EN         : 1;         ///<BIT [0] PCI-PM L1.2 Enable.
        uint32_t PCIPM_L11_EN         : 1;         ///<BIT [1] PCI-PM L1.1 Enable.
        uint32_t ASPM_L12_EN          : 1;         ///<BIT [2] ASPM L1.2 Enable.
        uint32_t ASPM_L11_EN          : 1;         ///<BIT [3] ASPM L1.1 Enable.
        uint32_t RF00002C8_7_4_RSVD   : 4;         ///<BIT [7:4] Reserved.
        uint32_t T_CMM_MODE           : 8;         ///<BIT [15:8] Common Mode Restore Time.
        uint32_t L12_TH_VAL           :10;         ///<BIT [25:16] LTR L1.2 THRESHOLD Value.
        uint32_t RF00002C8_28_26_RSVD : 3;         ///<BIT [28:26] Reserved.
        uint32_t L12_TH_SCALE         : 3;         ///<BIT [31:29] LTR L1.2 THRESHOLD Scale.
    } b;    
} L1SubstateCtrl1_t;

///  @brief 0xF00002CC : L1 Substates Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PWRON_SCALE         : 2;         ///<BIT [1:0] T POWER ON Scale.
        uint32_t RF00002CC_2_RSVD    : 1;         ///<BIT [2] Reserved.
        uint32_t PWRON_VAL           : 5;         ///<BIT [7:3] T POWER ON Value.
        uint32_t RF00002CC_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} L1SubstateCtrl2_t;

///  @brief 0xF00002D0 : Dynamic Power Allocation Capability Header
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CAP_ID      :16;         ///<BIT [15:0] PCI Express Capability ID.
        uint32_t CAP_VER     : 4;         ///<BIT [19:16] Capabilty Version.
        uint32_t NEXT_OFFSET :12;         ///<BIT [31:20] Next Capability Offset.
    } b;    
} DynamicPowerAllocatiCapabiliHdr_t;

///  @brief 0xF00002D4 : DPA Capability
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SUBSTATE_MAX         : 5;         ///<BIT [4:0] Substate Max.
        uint32_t RF00002D4_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t TLUNIT               : 2;         ///<BIT [9:8] Transition Latency Unit.
        uint32_t RF00002D4_11_10_RSVD : 2;         ///<BIT [11:10] Reserved.
        uint32_t PAS                  : 2;         ///<BIT [13:12] Power Allocation Scale.
        uint32_t RF00002D4_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
        uint32_t XLCY0                : 8;         ///<BIT [23:16] Transition Latency Value 0.
        uint32_t XLCY1                : 8;         ///<BIT [31:24] Transition Latnecy Value 1.
    } b;    
} DpaCapabili_t;

///  @brief 0xF00002DC : DPA Status And Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SUB_STS              : 5;         ///<BIT [4:0] Substate Status.
        uint32_t RF00002DC_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t SUB_CTRL_EN          : 1;         ///<BIT [8] Substate Control Enabled.
        uint32_t RF00002DC_15_9_RSVD  : 7;         ///<BIT [15:9] Reserved.
        uint32_t SUBSTATE_CTRL        : 5;         ///<BIT [20:16] Substate Control.
        uint32_t RF00002DC_31_21_RSVD :11;         ///<BIT [31:21] Reserved.
    } b;    
} DpaStatAndCtrl_t;

///  @brief 0xF00002E0 : DPA Power Allocation Array 00-03
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VAL00 : 8;         ///<BIT [7:0] Substate Power Allocation 00.
        uint32_t VAL01 : 8;         ///<BIT [15:8] Substate Power Allocation 01.
        uint32_t VAL02 : 8;         ///<BIT [23:16] Substate Power Allocation 02.
        uint32_t VAL03 : 8;         ///<BIT [31:24] Substate Power Allocation 03.
    } b;    
} DpaPowerAllocatiArray0003_t;

///  @brief 0xF00002E4 : DPA Power Allocation Array 04-07
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VAL04 : 8;         ///<BIT [7:0] Substate Power Allocation 04.
        uint32_t VAL05 : 8;         ///<BIT [15:8] Substate Power Allocation 05.
        uint32_t VAL06 : 8;         ///<BIT [23:16] Substate Power Allocation 06.
        uint32_t VAL07 : 8;         ///<BIT [31:24] Substate Power Allocation 07.
    } b;    
} DpaPowerAllocatiArray0407_t;

///  @brief 0xF00002E8 : DPA Power Allocation Array 08-0B
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VAL08 : 8;         ///<BIT [7:0] Substate Power Allocation 08.
        uint32_t VAL09 : 8;         ///<BIT [15:8] Substate Power Allocation 09.
        uint32_t VAL0A : 8;         ///<BIT [23:16] Substate Power Allocation 0A.
        uint32_t VAL0B : 8;         ///<BIT [31:24] Substate Power Allocation 0B.
    } b;    
} DpaPowerAllocatiArray080b_t;

///  @brief 0xF00002EC : DPA Power Allocation Array 0C-0F
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VAL0C : 8;         ///<BIT [7:0] Substate Power Allocation 0C.
        uint32_t VAL0D : 8;         ///<BIT [15:8] Substate Power Allocation 0D.
        uint32_t VAL0E : 8;         ///<BIT [23:16] Substate Power Allocation 0E.
        uint32_t VAL0F : 8;         ///<BIT [31:24] Substate Power Allocation 0F.
    } b;    
} DpaPowerAllocatiArray0c0f_t;

///  @brief 0xF00002F0 : DPA Power Allocation Array 10-13
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VAL10 : 8;         ///<BIT [7:0] Substate Power Allocation 10.
        uint32_t VAL11 : 8;         ///<BIT [15:8] Substate Power Allocation 11.
        uint32_t VAL12 : 8;         ///<BIT [23:16] Substate Power Allocation 12.
        uint32_t VAL13 : 8;         ///<BIT [31:24] Substate Power Allocation 13.
    } b;    
} DpaPowerAllocatiArray1013_t;

///  @brief 0xF00002F4 : DPA Power Allocation Array 14-17
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VAL14 : 8;         ///<BIT [7:0] Substate Power Allocation 14.
        uint32_t VAL15 : 8;         ///<BIT [15:8] Substate Power Allocation 15.
        uint32_t VAL16 : 8;         ///<BIT [23:16] Substate Power Allocation 16.
        uint32_t VAL17 : 8;         ///<BIT [31:24] Substate Power Allocation 17.
    } b;    
} DpaPowerAllocatiArray1417_t;

///  @brief 0xF00002F8 : DPA Power Allocation Array 18-1B
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VAL18 : 8;         ///<BIT [7:0] Substate Power Allocation 18.
        uint32_t VAL19 : 8;         ///<BIT [15:8] Substate Power Allocation 19.
        uint32_t VAL1A : 8;         ///<BIT [23:16] Substate Power Allocation 1A.
        uint32_t VAL1B : 8;         ///<BIT [31:24] Substate Power Allocation 1B.
    } b;    
} DpaPowerAllocatiArray181b_t;

///  @brief 0xF00002FC : DPA Power Allocation Array 1C-1F
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VAL1C : 8;         ///<BIT [7:0] Substate Power Allocation 1C.
        uint32_t VAL1D : 8;         ///<BIT [15:8] Substate Power Allocation 1D.
        uint32_t VAL1E : 8;         ///<BIT [23:16] Substate Power Allocation 1E.
        uint32_t VAL1F : 8;         ///<BIT [31:24] Substate Power Allocation 1F.
    } b;    
} DpaPowerAllocatiArray1c1f_t;

///  @brief 0xF0000700 : Ack Latency Timer And Replay Timer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ROUND_TRIP_LAT_TIME_LIMIT :16;         ///<BIT [15:0] Ack Latency Timer Limit.
        uint32_t REPLAY_TIME_LIMIT         :16;         ///<BIT [31:16] Replay Timer Limit.
    } b;    
} AckLatencyTmrAndReplayTmr_t;

///  @brief 0xF0000708 : Port Force Link
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LINK_NUM             : 8;         ///<BIT [7:0] Link Number.
        uint32_t FORCED_LTSSM         : 4;         ///<BIT [11:8] Forced Link Command.
        uint32_t RF0000708_14_12_RSVD : 3;         ///<BIT [14:12] Reserved.
        uint32_t FORCE_EN             : 1;         ///<BIT [15] Force Link.
        uint32_t LINK_STATE           : 6;         ///<BIT [21:16] Forced LTSSM State.
        uint32_t RF0000708_23_22_RSVD : 2;         ///<BIT [23:22] Reserved.
        uint32_t CPL_SENT_CNT         : 8;         ///<BIT [31:24] Low Power Entrance Count.
    } b;    
} PortForceLnk_t;

///  @brief 0xF000070C : Ack Frequency And L0s L1 ASPM Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ACK_FREQ          : 8;         ///<BIT [7:0] Ack Frequency.
        uint32_t ACK_NFTS          : 8;         ///<BIT [15:8] N_FTS.
        uint32_t CMCLK_NFTS        : 8;         ///<BIT [23:16] Comon Clock N_FTS.
        uint32_t L0S_ENTER_LTC     : 3;         ///<BIT [26:24] L0s Entrance Latency.
        uint32_t L1_ENTER_LTC      : 3;         ///<BIT [29:27] L1 Entrance Latency.
        uint32_t ENTER_ASPM        : 1;         ///<BIT [30] Enter ASPM L1.
        uint32_t RF000070C_31_RSVD : 1;         ///<BIT [31] Reserved.
    } b;    
} AckFrqAndL0sL1AspmCtrl_t;

///  @brief 0xF0000710 : Port Link Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VEN_DLLP_REQ         : 1;         ///<BIT [0] Vendor-Specific DLLP Request.
        uint32_t SCRM_DIS             : 1;         ///<BIT [1] Scramble Disable.
        uint32_t LPBK_EN              : 1;         ///<BIT [2] Loopback Enable.
        uint32_t RF0000710_6_3_RSVD   : 4;         ///<BIT [6:3] Reserved.
        uint32_t FAST_LINK_MODE       : 1;         ///<BIT [7] Fast Link Mode.
        uint32_t RF0000710_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t LINK_CAPABLE         : 6;         ///<BIT [21:16] Link Mode Enable.
        uint32_t RF0000710_31_22_RSVD :10;         ///<BIT [31:22] Reserved.
    } b;    
} PortLnkCtrl_t;

///  @brief 0xF0000714 : Lane Skew
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INS_LANE_SKEW        :24;         ///<BIT [23:0] Insert Lane Skew For Transmit.
        uint32_t FC_DIS               : 1;         ///<BIT [24] Flow Control Disable.
        uint32_t ACK_NAK_DIS          : 1;         ///<BIT [25] Ack/Nak Disable.
        uint32_t RF0000714_30_26_RSVD : 5;         ///<BIT [30:26] Reserved.
        uint32_t DIS_L2L_DESKEW       : 1;         ///<BIT [31] Disable Lane-to-Lane Deskew.
    } b;    
} LaneSkew_t;

///  @brief 0xF0000718 : Timer Control And Max Function
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MAX_FN               : 8;         ///<BIT [7:0] Maximum Function Number.
        uint32_t RF0000718_13_8_RSVD  : 6;         ///<BIT [13:8] Reserved.
        uint32_t TM_RPL               : 5;         ///<BIT [18:14] Replay Timer Limit Modifier.
        uint32_t TM_ACK               : 5;         ///<BIT [23:19] Ack Latency Timer Modifier.
        uint32_t RF0000718_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} TmrCtrlAndMaxFunc_t;

///  @brief 0xF000071C : Symbol Timer And Filter Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SKIP_INT_VAL         :11;         ///<BIT [10:0] SKP Interval Value.
        uint32_t RF000071C_14_11_RSVD : 4;         ///<BIT [14:11] Reserved.
        uint32_t DIS_FC_WD            : 1;         ///<BIT [15] Disable FC Watchdog Timer.
        uint32_t MASK_RADM_1          :16;         ///<BIT [31:16] Mask RADM 1.
    } b;    
} SymTmrAndFilterMsk_t;

///  @brief 0xF0000720 : Filter Mask 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MASK_RADM           : 4;         ///<BIT [3:0] MASK_RADM.
        uint32_t RF0000720_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} FilterMsk2_t;

///  @brief 0xF0000724 : AMBA Multiple Outboyund Decomposed NP SubRequests Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RD_SPLIT_EN         : 2;         ///<BIT [1:0] Enable AMBA Multiple Outbound Decomposed NP Subrequests.
        uint32_t RF0000724_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} AmbaMultipleOutboyunDecomposNpSubrequeCtrl_t;

///  @brief 0xF0000730 : Transmit Posted FC Credit Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DATA_FC_CD           :12;         ///<BIT [11:0] Transmit Posted Data FC Credits.
        uint32_t HDR_FC_CD            : 8;         ///<BIT [19:12] Transmit Posted Header FC Credits.
        uint32_t RF0000730_31_20_RSVD :12;         ///<BIT [31:20] Reserved.
    } b;    
} TxPostedFcCreditStat_t;

///  @brief 0xF0000734 : Transmit Non-Posted FC Credit Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DATA_FC_CD           :12;         ///<BIT [11:0] Transmit Non-Posted Data FC Credits.
        uint32_t HDR_FC_CD            : 8;         ///<BIT [19:12] Transmit Non-Posted Header FC Credits.
        uint32_t RF0000734_31_20_RSVD :12;         ///<BIT [31:20] Reserved.
    } b;    
} TxNonPostedFcCreditStat_t;

///  @brief 0xF0000738 : Transmit Completion FC Credit Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DATA_FC_CD           :12;         ///<BIT [11:0] Transmit Completion Data FC Credits.
        uint32_t HDR_FC_CD            : 8;         ///<BIT [19:12] Transmit Completion Header FC Credits.
        uint32_t RF0000738_31_20_RSVD :12;         ///<BIT [31:20] Reserved.
    } b;    
} TxCompletiFcCreditStat_t;

///  @brief 0xF000073C : Queue Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RX_FC_NR             : 1;         ///<BIT [0] Received TLP FC Credit Not Returned.
        uint32_t RETRYB_NE            : 1;         ///<BIT [1] Transmit Retry Buffer Not Empty.
        uint32_t RXQ_NE               : 1;         ///<BIT [2] Received Queue Not Empty.
        uint32_t RF000073C_15_3_RSVD  :13;         ///<BIT [15:3] Reserved.
        uint32_t TIMER_FC             :13;         ///<BIT [28:16] FC Latency Timer Override Value.
        uint32_t RF000073C_30_29_RSVD : 2;         ///<BIT [30:29] Reserved.
        uint32_t TIMER_FC_EN          : 1;         ///<BIT [31] FC Latency Timer Override Enable.
    } b;    
} QueueStat_t;

///  @brief 0xF000080C : Link Width And Speed Change Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t N_FTS                : 8;         ///<BIT [7:0] Number Of Fast Training Sequences.
        uint32_t N_LANES              : 9;         ///<BIT [16:8] Predetermined Number Of Lanes.
        uint32_t DIRECT_SC            : 1;         ///<BIT [17] Directed Speed Change.
        uint32_t CFG_PHY_TX           : 1;         ///<BIT [18] Config PHY Tx Swing.
        uint32_t CFG_TXCOMP_RX        : 1;         ///<BIT [19] Config TX Compliance Receive Bit.
        uint32_t SEL_DEEMPH           : 1;         ///<BIT [20] SEL_DEEMPH.
        uint32_t G1_EII               : 1;         ///<BIT [21] Electrical Idle Inference Mode At Gen1.
        uint32_t RF000080C_31_22_RSVD :10;         ///<BIT [31:22] Reserved.
    } b;    
} LnkWidthAndSpeedChangeCtrl_t;

///  @brief 0xF0000810 : PHY Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PHY_STATUS          : 4;         ///<BIT [3:0] PHY Status.
        uint32_t RF0000810_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} PhyStat11_t;

///  @brief 0xF0000890 : Gen3 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ZRXDC_NCPL           : 1;         ///<BIT [0] Gen3 Receiver Impedance ZRX-DC Not Compliant.
        uint32_t RF0000890_7_1_RSVD   : 7;         ///<BIT [7:1] Reserved.
        uint32_t G3_SCR_DIS           : 1;         ///<BIT [8] Disable Scrambler for Gen3 Data Rate.
        uint32_t EQ_PH23              : 1;         ///<BIT [9] Equalization Phase 2 and 3 Disable.
        uint32_t EIEOS_CNT            : 1;         ///<BIT [10] Equalization EIEOS Count Reset Disable.
        uint32_t EQ_REDO              : 1;         ///<BIT [11] Equalization Redo Disable.
        uint32_t RXEQ_PH01_HOLD       : 1;         ///<BIT [12] Rx Equalization Phase0/Phase1 Hold Enable.
        uint32_t RF0000890_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t DC_DIS               : 1;         ///<BIT [16] DC Balance Disable.
        uint32_t DLLP_DLY_DIS         : 1;         ///<BIT [17] DLLP Transmission Delay Disable.
        uint32_t EQ_DIS               : 1;         ///<BIT [18] Equalization Disable.
        uint32_t RF0000890_31_19_RSVD :13;         ///<BIT [31:19] Reserved.
    } b;    
} Gen3Ctrl_t;

///  @brief 0xF00008A8 : Gen3 Equalization Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FB_MODEL             : 4;         ///<BIT [3:0] Feedback Mode.
        uint32_t PHASE23_EXIT_MODE    : 1;         ///<BIT [4] Behavior After 24ms Timeout.
        uint32_t EVAL_2MS_DIS         : 1;         ///<BIT [5] Phase2/3 2ms Timeout Disable.
        uint32_t RF00008A8_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint32_t PSET_REQ_VEC         :16;         ///<BIT [23:8] Preset Request Vector.
        uint32_t FOM_INC_INITIAL_EVAL : 1;         ///<BIT [24] Include Initial FOM.
        uint32_t RF00008A8_31_25_RSVD : 7;         ///<BIT [31:25] Reserved.
    } b;    
} Gen3EqualizaCtrl_t;

///  @brief 0xF00008AC : Gen3 EQ Direction Change Feedback Mode Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FMDC_T_MIN_PHASE23         : 5;         ///<BIT [4:0] Minimum Time To Remain in EQ Master Phase.
        uint32_t FMDC_N_EVALS               : 5;         ///<BIT [9:5] Convergence Window Depth.
        uint32_t FMDC_MAX_PRE_CURSOR_DELTA  : 4;         ///<BIT [13:10] Convergence Window Aperture for C-1.
        uint32_t FMDC_MAX_POST_CURSOR_DELTA : 4;         ///<BIT [17:14] Convergence Window Aperture for C-1.
        uint32_t RF00008AC_31_18_RSVD       :14;         ///<BIT [31:18] Reserved.
    } b;    
} Gen3EqDirChangeFbModeCtrl_t;

///  @brief 0xF00008B8 : PIPE Loopback Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00008B8_30_0_RSVD :31;         ///<BIT [30:0] Reserved.
        uint32_t PIPE_LPK            : 1;         ///<BIT [31] PIPE Loopback Enable.
    } b;    
} PipeLoopbackCtrl_t;

///  @brief 0xF00008BC : DBI Read-Only Write Enable
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DBI_WE              : 1;         ///<BIT [0] Write To RO Registers Using DBI.
        uint32_t RF00008BC_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} DbiRdOnlyWrEn_t;

///  @brief 0xF00008D0 : AXI Bridge Slave Error Response
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t GLOBAL              : 1;         ///<BIT [0] Global Slave Error Response Mapping.
        uint32_t RF00008D0_1_RSVD    : 1;         ///<BIT [1] Reserved.
        uint32_t VENDORID            : 1;         ///<BIT [2] Vendor ID Non-existent Slave Error Response Mapping.
        uint32_t Report_Axi_Err      : 1;         ///<BIT [3] PCIe IP report AXI error
        uint32_t RF00008D0_31_3_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;
} AxiBdgSlaveErrResponse_t;

///  @brief 0xF00008D4 : Link Down AXI Bridge Slave Timeout
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LT_PERIOD           : 8;         ///<BIT [7:0] Timeout Value.
        uint32_t LT_EN               : 1;         ///<BIT [8] Disable Flush.
        uint32_t RF00008D4_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;    
} LnkDnAxiBdgSlaveTmo_t;

///  @brief 0xF00008DC : AXI Ordering Manager Watchdog Timer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t WATCHDOG             :16;         ///<BIT [15:0] Watchdog Timer Value.
        uint32_t RF00008DC_30_16_RSVD :15;         ///<BIT [30:16] Reserved.
        uint32_t WD_EN                : 1;         ///<BIT [31] AXI Ordering Manager Watchdog Timer Enable.
    } b;    
} AxiOrderingManagerWdgTmr_t;

///  @brief 0xF0000900 : iATU Index
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t INDEX               : 3;         ///<BIT [2:0] Region Index.
        uint32_t RF0000900_30_3_RSVD :28;         ///<BIT [30:3] Reserved.
        uint32_t DIR                 : 1;         ///<BIT [31] Region Direction.
    } b;    
} IatuIndex_t;

///  @brief 0xF0000904 : iATU Region Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TYPE                 : 5;         ///<BIT [4:0] TYPE.
        uint32_t TC                   : 3;         ///<BIT [7:5] TC.
        uint32_t TD                   : 1;         ///<BIT [8] TD.
        uint32_t ATTR                 : 2;         ///<BIT [10:9] ATTR.
        uint32_t IDO                  : 1;         ///<BIT [11] IDO.
        uint32_t TH                   : 1;         ///<BIT [12] TH.
        uint32_t RF0000904_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t AT                   : 2;         ///<BIT [17:16] AT.
        uint32_t PH                   : 2;         ///<BIT [19:18] PH.
        uint32_t FNUM                 : 5;         ///<BIT [24:20] Function Number.
        uint32_t RF0000904_31_25_RSVD : 7;         ///<BIT [31:25] Reserved.
    } b;    
} IatuRegisterionCtrl1_t;

///  @brief 0xF0000908 : iATU Region Control 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0000908_7_0_RSVD   : 8;         ///<BIT [7:0] Reserved.
        uint32_t BAR                  : 3;         ///<BIT [10:8] BAR Number.
        uint32_t RF0000908_13_11_RSVD : 3;         ///<BIT [13:11] Reserved.
        uint32_t TC_EN                : 1;         ///<BIT [14] TC Match Enable.
        uint32_t TD_EN                : 1;         ///<BIT [15] TD Match Enable.
        uint32_t ATTR_EN              : 1;         ///<BIT [16] ATTR Match Enable.
        uint32_t TH_EN                : 1;         ///<BIT [17] TH Match Enable.
        uint32_t AT_EN                : 1;         ///<BIT [18] AT Match Enable.
        uint32_t FN_EN                : 1;         ///<BIT [19] Function Number Match Enable.
        uint32_t VF_EN                : 1;         ///<BIT [20] Virtual Function Number Match Enable.
        uint32_t RF0000908_21_RSVD    : 1;         ///<BIT [21] Reserved.
        uint32_t PH_EN                : 1;         ///<BIT [22] PH Match Enable.
        uint32_t RF0000908_23_RSVD    : 1;         ///<BIT [23] Reserved.
        uint32_t RESP                 : 2;         ///<BIT [25:24] Response Code.
        uint32_t RF0000908_28_26_RSVD : 3;         ///<BIT [28:26] Reserved.
        uint32_t INVERT               : 1;         ///<BIT [29] Invert Mode.
        uint32_t MATCH                : 1;         ///<BIT [30] Match Mode.
        uint32_t EN                   : 1;         ///<BIT [31] Region Enable.
    } b;    
} IatuRegisterionCtrl2_t;

///  @brief 0xF000090C : iATU Lower Base Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF000090C_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t LWR_BASE            :16;         ///<BIT [31:16] LWR_BASE.
    } b;    
} IatuLowerBaseAddr_t;

///  @brief 0xF0000914 : iATU Limit Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0000914_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t LIMIT               :16;         ///<BIT [31:16] LIMIT.
    } b;    
} IatuLimitAddr_t;

///  @brief 0xF0000918 : iATU Region N Offset Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0000918_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t TARGET              :16;         ///<BIT [31:16] TARGET.
    } b;    
} IatuRegisterionNOsAddr_t;

///  @brief 0xF0000920 : iATU Region Control 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VFN                 : 1;         ///<BIT [0] Virtual Function Number.
        uint32_t RF0000920_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} IatuRegisterionCtrl3_t;


/// @brief 0xF0000000-0xF0000924 : Pcie_t MODULE
typedef struct
{
    VendorIdAndDeviceId_t vendorIdAndDeviceId;                                                   ///< 0xF0000000 : Vendor ID and Device ID
    CmdAndStat_t cmdAndStat;                                                                     ///< 0xF0000004 : Command and Status
    RevisionIdAndClassCmem_t revisionIdAndClassCmem;                                             ///< 0xF0000008 : Revision ID and Class Code
    BistHdrTypeLatencyTmrAndCacheLine_t bistHdrTypeLatencyTmrAndCacheLine;                       ///< 0xF000000C : BIST, Header Type, Latency Timer and Cache Line Size
    BaseAddr0_t baseAddr0;                                                                       ///< 0xF0000010 : Base Address 0
    BaseAddr1_t baseAddr1;                                                                       ///< 0xF0000014 : Base Address 1
    BaseAddr2_t baseAddr2;                                                                       ///< 0xF0000018 : Base Address 2
    BaseAddr3_t baseAddr3;                                                                       ///< 0xF000001C : Base Address 3
    BaseAddr4_t baseAddr4;                                                                       ///< 0xF0000020 : Base Address 4
    BaseAddr5_t baseAddr5;                                                                       ///< 0xF0000024 : Base Address 5
    uint32_t CARDBUS_CIS_PTR;                                                                    ///< 0xF0000028 : CardBus CIS Pointer
    SubsysteVendorIdAndSubsysteId_t subsysteVendorIdAndSubsysteId;                               ///< 0xF000002C : Subsystem Vendor ID and Subsystem ID
    ExpansioRomBaseAddr_t expansioRomBaseAddr;                                                   ///< 0xF0000030 : Expansion ROM Base Address
    CapabiliPtr_t capabiliPtr;                                                                   ///< 0xF0000034 : Capabilities Pointer
    uint32_t RF0000038_31_0_RSVD;                                                                ///< 0xF0000038 : Reserved
    IntrLineAndIntrPin_t intrLineAndIntrPin;                                                     ///< 0xF000003C : Interrupt Line and Interrupt Pin
    PowerMngCapabili_t powerMngCapabili;                                                         ///< 0xF0000040 : Power Management Capabilities
    PowerMngCtrlAndStat_t powerMngCtrlAndStat;                                                   ///< 0xF0000044 : Power Management Control And Status
    uint8_t rsvd_RF0000048[8];                                                                   ///< 0xF0000048 - 0xF000004F : Reserved.
    MsiCapabiliHdr_t msiCapabiliHdr;                                                             ///< 0xF0000050 : MSI Capability Header
    MsiCapabiliAddr_t msiCapabiliAddr;                                                           ///< 0xF0000054 : MSI Capability Address
    uint32_t MSI_OFF_08;                                                                         ///< 0xF0000058 : MSI Capability Address Or Data
    MsiCapabiliData_t msiCapabiliData;                                                           ///< 0xF000005C : MSI Capability Data
    uint32_t MSI_OFF_10;                                                                         ///< 0xF0000060 : MSI Capability Address or Data 10h
    uint32_t MSI_OFF_14;                                                                         ///< 0xF0000064 : MSI Capability Address or Data 14h
    uint8_t rsvd_RF0000068[8];                                                                   ///< 0xF0000068 - 0xF000006F : Reserved.
    PciExpressCapabili_t pciExpressCapabili;                                                     ///< 0xF0000070 : PCI Express Capabilities
    DeviceCapabili_t deviceCapabili;                                                             ///< 0xF0000074 : Device Capabilities
    DeviceCtrlAndStat_t deviceCtrlAndStat;                                                       ///< 0xF0000078 : Device Control And Status
    LnkCapabili_t lnkCapabili;                                                                   ///< 0xF000007C : Link Capabilities
    LnkCtrlAndLnkStat_t lnkCtrlAndLnkStat;                                                       ///< 0xF0000080 : Link Control And Link Status
    uint8_t rsvd_RF0000084[16];                                                                  ///< 0xF0000084 - 0xF0000093 : Reserved.
    DeviceCapabili2_t deviceCapabili2;                                                           ///< 0xF0000094 : Device Capabilities 2
    DeviceCtrl2AndStat2_t deviceCtrl2AndStat2;                                                   ///< 0xF0000098 : Device Control 2 And Status 2
    LnkCapabili2Register_t lnkCapabili2Register;                                                 ///< 0xF000009C : Link Capabilities 2 Register
    LnkCtrl2AndLnkStat2_t lnkCtrl2AndLnkStat2;                                                   ///< 0xF00000A0 : Link Control 2 And Link Status 2
    uint8_t rsvd_RF00000A4[12];                                                                  ///< 0xF00000A4 - 0xF00000AF : Reserved.
    MsiXCfg_t msiXCfg;                                                                           ///< 0xF00000B0 : MSI-X Configuration
    MsiXTblAddr_t msiXTblAddr;                                                                   ///< 0xF00000B4 : MSI-X Table Address
    MsiXPendingBitArray_t msiXPendingBitArray;                                                   ///< 0xF00000B8 : MSI-X Pending Bit Array
    uint8_t rsvd_RF00000BC[68];                                                                  ///< 0xF00000BC - 0xF00000FF : Reserved.
    AdvancedErrReportinAerEnhancedCapabiliHdr_t advancedErrReportinAerEnhancedCapabiliHdr;       ///< 0xF0000100 : Advanced Error Reporting (AER) Enhanced Capability Header
    UncorrecErrStat_t uncorrecErrStat;                                                           ///< 0xF0000104 : Uncorrectable Error Status
    UncorrecErrMsk_t uncorrecErrMsk;                                                             ///< 0xF0000108 : Uncorrectable Error Mask
    UncorrecErrSeverity_t uncorrecErrSeverity;                                                   ///< 0xF000010C : Uncorrectable Error Severity
    CorrectaErrStat_t correctaErrStat;                                                           ///< 0xF0000110 : Correctable Error Status
    CorrectaErrMsk_t correctaErrMsk;                                                             ///< 0xF0000114 : Correctable Error Mask
    AdvancedErrCapabiliAndCtrl_t advancedErrCapabiliAndCtrl;                                     ///< 0xF0000118 : Advanced Error Capabilities And Control
    uint32_t HEADER_0;                                                                           ///< 0xF000011C : Header Log 0
    uint32_t HEADER_1;                                                                           ///< 0xF0000120 : Header Log 1
    uint32_t HEADER_2;                                                                           ///< 0xF0000124 : Header Log 2
    uint32_t HEADER_3;                                                                           ///< 0xF0000128 : Header Log 3
    uint32_t RF000012C_31_0_RSVD;                                                                ///< 0xF000012C : Reserved
    uint8_t rsvd_RF0000130[8];                                                                   ///< 0xF0000130 - 0xF0000137 : Reserved.
    uint32_t TLP_PFX_LOG;                                                                        ///< 0xF0000138 : TLP Prefix Log
    uint8_t rsvd_RF000013C[12];                                                                  ///< 0xF000013C - 0xF0000147 : Reserved.
    SerialNumExtCapabili_t serialNumExtCapabili;                                                 ///< 0xF0000148 : Serial Number Extended Capabilities
    uint32_t DEV_SN_FIRST;                                                                       ///< 0xF000014C : Device Serial Number First DWORD
    uint32_t DEV_SN_SECOND;                                                                      ///< 0xF0000150 : Device Serial Number Second DWORD
    uint8_t rsvd_RF0000154[4];                                                                   ///< 0xF0000154 - 0xF0000157 : Reserved.
    PowerBudgetinExtCapabiliHdr_t powerBudgetinExtCapabiliHdr;                                   ///< 0xF0000158 : Power Budgeting Extended Capability Header
    uint8_t rsvd_RF000015C[4];                                                                   ///< 0xF000015C - 0xF000015F : Reserved.
    PowerBudgetinData_t powerBudgetinData;                                                       ///< 0xF0000160 : Power Budgeting Data
    PowerBudgetCapabili_t powerBudgetCapabili;                                                   ///< 0xF0000164 : Power Budget Capability
    AriCapabiliHdr_t ariCapabiliHdr;                                                             ///< 0xF0000168 : ARI Capability Header
    AriCapabiltAndCtrl_t ariCapabiltAndCtrl;                                                     ///< 0xF000016C : ARI Capabilty And Control
    uint8_t rsvd_RF0000170[8];                                                                   ///< 0xF0000170 - 0xF0000177 : Reserved.
    SecondaryPciExpressExtCapabiliHdr_t secondaryPciExpressExtCapabiliHdr;                       ///< 0xF0000178 : Secondary PCI Express Extended Capability Header
    LnkCtrl3_t lnkCtrl3;                                                                         ///< 0xF000017C : Link Control 3 Register
    LaneErrStat_t laneErrStat;                                                                   ///< 0xF0000180 : Lane Error Status
    LaneEqualizaCtrl0And1_t laneEqualizaCtrl0And1;                                               ///< 0xF0000184 : Lane Equalization Control 0 And 1
    LaneEqualizaCtrl2And3_t laneEqualizaCtrl2And3;                                               ///< 0xF0000188 : Lane Equalization Control 2 And 3
    uint8_t rsvd_RF000018C[12];                                                                  ///< 0xF000018C - 0xF0000197 : Reserved.
    SrIovExtCapabiliHdr_t srIovExtCapabiliHdr;                                                   ///< 0xF0000198 : SR-IOV Extended Capability Header
    SrIovCapabiliField_t srIovCapabiliField;                                                     ///< 0xF000019C : SR-IOV Capability Field
    SrIovCtrlAndStat_t srIovCtrlAndStat;                                                         ///< 0xF00001A0 : SR-IOV Control And Status
    TotalvfsAndInitialv00_t totalvfsAndInitialv0_0;                                              ///< 0xF00001A4 : TotalVFs And InitialVFs
    NumvfsAndFuncDependenLnk_t numvfsAndFuncDependenLnk;                                         ///< 0xF00001A8 : NumVFs And Function Dependency Link
    VfStrideAndOs00_t vfStrideAndOs0_0;                                                          ///< 0xF00001AC : VF Stride And Offset
    VfDeviceId_t vfDeviceId;                                                                     ///< 0xF00001B0 : VF Device ID
    uint32_t SUP_PAGE_SIZE;                                                                      ///< 0xF00001B4 : Supported Page Sizes
    uint32_t SYS_PAGE_SIZE;                                                                      ///< 0xF00001B8 : System Page Size
    VfBar0_t vfBar0;                                                                             ///< 0xF00001BC : VF BAR0
    VfBar1_t vfBar1;                                                                             ///< 0xF00001C0 : VF BAR1
    VfBar2_t vfBar2;                                                                             ///< 0xF00001C4 : VF BAR2
    VfBar3_t vfBar3;                                                                             ///< 0xF00001C8 : VF BAR3
    VfBar4_t vfBar4;                                                                             ///< 0xF00001CC : VF BAR4
    VfBar5_t vfBar5;                                                                             ///< 0xF00001D0 : VF BAR5
    VfMigratioStateArrayOs_t vfMigratioStateArrayOs;                                             ///< 0xF00001D4 : VF Migration State Array Offset
    TphRequesteExtCapabiliHdr_t tphRequesteExtCapabiliHdr;                                       ///< 0xF00001D8 : TPH Requester Extended Capability Header
    TphRequesteExtCapabili_t tphRequesteExtCapabili;                                             ///< 0xF00001DC : TPH Requester Extended Capability
    TphRequesteCtrl_t tphRequesteCtrl;                                                           ///< 0xF00001E0 : TPH Requester Control
    TphStTbl00And01_t tphStTbl00And01;                                                           ///< 0xF00001E4 : TPH ST Table 00 And 01
    TphStTbl02And03_t tphStTbl02And03;                                                           ///< 0xF00001E8 : TPH ST Table 02 And 03
    TphStTbl04And05_t tphStTbl04And05;                                                           ///< 0xF00001EC : TPH ST Table 04 And 05
    TphStTbl06And07_t tphStTbl06And07;                                                           ///< 0xF00001F0 : TPH ST Table 06 And 07
    TphStTbl08And09_t tphStTbl08And09;                                                           ///< 0xF00001F4 : TPH ST Table 08 And 09
    TphStTbl0aAnd0b_t tphStTbl0aAnd0b;                                                           ///< 0xF00001F8 : TPH ST Table 0A And 0B
    TphStTbl0cAnd0d_t tphStTbl0cAnd0d;                                                           ///< 0xF00001FC : TPH ST Table 0C And 0D
    TphStTbl0eAnd0f_t tphStTbl0eAnd0f;                                                           ///< 0xF0000200 : TPH ST Table 0E And 0F
    TphStTbl10And11_t tphStTbl10And11;                                                           ///< 0xF0000204 : TPH ST Table 10 And 11
    TphStTbl12And13_t tphStTbl12And13;                                                           ///< 0xF0000208 : TPH ST Table 12 And 13
    TphStTbl14And15_t tphStTbl14And15;                                                           ///< 0xF000020C : TPH ST Table 14 And 15
    TphStTbl16And17_t tphStTbl16And17;                                                           ///< 0xF0000210 : TPH ST Table 16 And 17
    TphStTbl18And19_t tphStTbl18And19;                                                           ///< 0xF0000214 : TPH ST Table 18 And 19
    TphStTbl1aAnd1b_t tphStTbl1aAnd1b;                                                           ///< 0xF0000218 : TPH ST Table 1A And 1B
    TphStTbl1cAnd1d_t tphStTbl1cAnd1d;                                                           ///< 0xF000021C : TPH ST Table 1C And 1D
    TphStTbl1eAnd1f_t tphStTbl1eAnd1f;                                                           ///< 0xF0000220 : TPH ST Table 1E And 1F
    uint8_t rsvd_RF0000224[132];                                                                 ///< 0xF0000224 - 0xF00002A7 : Reserved.
    AddrTranslatServiceCapabiliHdr_t addrTranslatServiceCapabiliHdr;                             ///< 0xF00002A8 : Address Translation Service Capability Header
    AtsCapabiliAndCtrl_t atsCapabiliAndCtrl;                                                     ///< 0xF00002AC : ATS Capability And Control
    uint8_t rsvd_RF00002B0[8];                                                                   ///< 0xF00002B0 - 0xF00002B7 : Reserved.
    LatencyTolReportinExtCapabiliHdr_t latencyTolReportinExtCapabiliHdr;                         ///< 0xF00002B8 : Latency Tolerance Reporting Extended Capability Header
    LtrMaxSnoopAndNoSnoopLatency_t ltrMaxSnoopAndNoSnoopLatency;                                 ///< 0xF00002BC : LTR Max Snoop and No-Snoop Latency
    L1SubstateExtCapabiliHdr_t l1SubstateExtCapabiliHdr;                                         ///< 0xF00002C0 : L1 Substates Extended Capability Header
    L1SubstateCapabili_t l1SubstateCapabili;                                                     ///< 0xF00002C4 : L1 Substate Capability
    L1SubstateCtrl1_t l1SubstateCtrl1;                                                           ///< 0xF00002C8 : L1 Substates Control 1
    L1SubstateCtrl2_t l1SubstateCtrl2;                                                           ///< 0xF00002CC : L1 Substates Control 2
    DynamicPowerAllocatiCapabiliHdr_t dynamicPowerAllocatiCapabiliHdr;                           ///< 0xF00002D0 : Dynamic Power Allocation Capability Header
    DpaCapabili_t dpaCapabili;                                                                   ///< 0xF00002D4 : DPA Capability
    uint32_t X1_IND;                                                                             ///< 0xF00002D8 : DPA Latency Indicator
    DpaStatAndCtrl_t dpaStatAndCtrl;                                                             ///< 0xF00002DC : DPA Status And Control
    DpaPowerAllocatiArray0003_t dpaPowerAllocatiArray0003;                                       ///< 0xF00002E0 : DPA Power Allocation Array 00-03
    DpaPowerAllocatiArray0407_t dpaPowerAllocatiArray0407;                                       ///< 0xF00002E4 : DPA Power Allocation Array 04-07
    DpaPowerAllocatiArray080b_t dpaPowerAllocatiArray080b;                                       ///< 0xF00002E8 : DPA Power Allocation Array 08-0B
    DpaPowerAllocatiArray0c0f_t dpaPowerAllocatiArray0c0f;                                       ///< 0xF00002EC : DPA Power Allocation Array 0C-0F
    DpaPowerAllocatiArray1013_t dpaPowerAllocatiArray1013;                                       ///< 0xF00002F0 : DPA Power Allocation Array 10-13
    DpaPowerAllocatiArray1417_t dpaPowerAllocatiArray1417;                                       ///< 0xF00002F4 : DPA Power Allocation Array 14-17
    DpaPowerAllocatiArray181b_t dpaPowerAllocatiArray181b;                                       ///< 0xF00002F8 : DPA Power Allocation Array 18-1B
    DpaPowerAllocatiArray1c1f_t dpaPowerAllocatiArray1c1f;                                       ///< 0xF00002FC : DPA Power Allocation Array 1C-1F
    uint8_t rsvd_RF0000300[1024];                                                                ///< 0xF0000300 - 0xF00006FF : Reserved.
    AckLatencyTmrAndReplayTmr_t ackLatencyTmrAndReplayTmr;                                       ///< 0xF0000700 : Ack Latency Timer And Replay Timer
    uint32_t VENDOR_SPEC_DLLP;                                                                   ///< 0xF0000704 : Vendor Specific DLLP
    PortForceLnk_t portForceLnk;                                                                 ///< 0xF0000708 : Port Force Link
    AckFrqAndL0sL1AspmCtrl_t ackFrqAndL0sL1AspmCtrl;                                             ///< 0xF000070C : Ack Frequency And L0s L1 ASPM Control
    PortLnkCtrl_t portLnkCtrl;                                                                   ///< 0xF0000710 : Port Link Control
    LaneSkew_t laneSkew;                                                                         ///< 0xF0000714 : Lane Skew
    TmrCtrlAndMaxFunc_t tmrCtrlAndMaxFunc;                                                       ///< 0xF0000718 : Timer Control And Max Function
    SymTmrAndFilterMsk_t symTmrAndFilterMsk;                                                     ///< 0xF000071C : Symbol Timer And Filter Mask
    FilterMsk2_t filterMsk2;                                                                     ///< 0xF0000720 : Filter Mask 2
    AmbaMultipleOutboyunDecomposNpSubrequeCtrl_t ambaMultipleOutboyunDecomposNpSubrequeCtrl;     ///< 0xF0000724 : AMBA Multiple Outboyund Decomposed NP SubRequests Control
    uint32_t CXPL_DEBUG_INFO_0;                                                                  ///< 0xF0000728 : Debug 0
    uint32_t CXPL_DEBUG_INFO_1;                                                                  ///< 0xF000072C : Debug 1
    TxPostedFcCreditStat_t txPostedFcCreditStat;                                                 ///< 0xF0000730 : Transmit Posted FC Credit Status
    TxNonPostedFcCreditStat_t txNonPostedFcCreditStat;                                           ///< 0xF0000734 : Transmit Non-Posted FC Credit Status
    TxCompletiFcCreditStat_t txCompletiFcCreditStat;                                             ///< 0xF0000738 : Transmit Completion FC Credit Status
    QueueStat_t queueStat;                                                                       ///< 0xF000073C : Queue Status
    uint8_t rsvd_RF0000740[204];                                                                 ///< 0xF0000740 - 0xF000080B : Reserved.
    LnkWidthAndSpeedChangeCtrl_t lnkWidthAndSpeedChangeCtrl;                                     ///< 0xF000080C : Link Width And Speed Change Control
    PhyStat11_t phyStat1_1;                                                                      ///< 0xF0000810 : PHY Status
    uint8_t rsvd_RF0000814[124];                                                                 ///< 0xF0000814 - 0xF000088F : Reserved.
    Gen3Ctrl_t gen3Ctrl;                                                                         ///< 0xF0000890 : Gen3 Control
    uint8_t rsvd_RF0000894[20];                                                                  ///< 0xF0000894 - 0xF00008A7 : Reserved.
    Gen3EqualizaCtrl_t gen3EqualizaCtrl;                                                         ///< 0xF00008A8 : Gen3 Equalization Control
    Gen3EqDirChangeFbModeCtrl_t gen3EqDirChangeFbModeCtrl;                                       ///< 0xF00008AC : Gen3 EQ Direction Change Feedback Mode Control
    uint8_t rsvd_RF00008B0[8];                                                                   ///< 0xF00008B0 - 0xF00008B7 : Reserved.
    PipeLoopbackCtrl_t pipeLoopbackCtrl;                                                         ///< 0xF00008B8 : PIPE Loopback Control
    DbiRdOnlyWrEn_t dbiRdOnlyWrEn;                                                               ///< 0xF00008BC : DBI Read-Only Write Enable
    uint8_t rsvd_RF00008C0[16];                                                                  ///< 0xF00008C0 - 0xF00008CF : Reserved.
    AxiBdgSlaveErrResponse_t axiBdgSlaveErrResponse;                                             ///< 0xF00008D0 : AXI Bridge Slave Error Response
    LnkDnAxiBdgSlaveTmo_t lnkDnAxiBdgSlaveTmo;                                                   ///< 0xF00008D4 : Link Down AXI Bridge Slave Timeout
    uint8_t rsvd_RF00008D8[4];                                                                   ///< 0xF00008D8 - 0xF00008DB : Reserved.
    AxiOrderingManagerWdgTmr_t axiOrderingManagerWdgTmr;                                         ///< 0xF00008DC : AXI Ordering Manager Watchdog Timer
    uint8_t rsvd_RF00008E0[32];                                                                  ///< 0xF00008E0 - 0xF00008FF : Reserved.
    IatuIndex_t iatuIndex;                                                                       ///< 0xF0000900 : iATU Index
    IatuRegisterionCtrl1_t iatuRegisterionCtrl1;                                                 ///< 0xF0000904 : iATU Region Control 1
    IatuRegisterionCtrl2_t iatuRegisterionCtrl2;                                                 ///< 0xF0000908 : iATU Region Control 2
    IatuLowerBaseAddr_t iatuLowerBaseAddr;                                                       ///< 0xF000090C : iATU Lower Base Address
    uint32_t UPPER_BASE;                                                                         ///< 0xF0000910 : iATU Upper Base Address
    IatuLimitAddr_t iatuLimitAddr;                                                               ///< 0xF0000914 : iATU Limit Address
    IatuRegisterionNOsAddr_t iatuRegisterionNOsAddr;                                             ///< 0xF0000918 : iATU Region N Offset Address
    uint8_t rsvd_RF000091C[4];                                                                   ///< 0xF000091C - 0xF000091F : Reserved.
    IatuRegisterionCtrl3_t iatuRegisterionCtrl3;                                                 ///< 0xF0000920 : iATU Region Control 3
} PciePf_t;
COMPILE_ASSERT(offsetof(PciePf_t,iatuRegisterionCtrl3)==0xF0000920-0xF0000000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile PciePf_t rPciePf;                        ///< 0xF0000000-0xF0000924 : Pcie_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

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
//! @brief PCIe Core Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  PCIe Core Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF0080000 : Pciecore Global 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SMLH_LTSSM_STATE_5_0   : 6;         ///<BIT [5:0] SMLH_LTSSM_STATE_5_0.
        uint32_t SMLH_LINK_UP           : 1;         ///<BIT [6] SMLH_LINK_UP.
        uint32_t RDLH_LINK_UP           : 1;         ///<BIT [7] RDLH_LINK_UP.
        uint32_t PM_XTLH_BLOCK_TLP      : 1;         ///<BIT [8] PM_XTLH_BLOCK_TLP.
        uint32_t PM_STATUS              : 1;         ///<BIT [9] PM_STATUS.
        uint32_t PM_LINKST_IN_L0S       : 1;         ///<BIT [10] PM_LINKST_IN_L0S.
        uint32_t PM_CURNT_STATE_2_0     : 3;         ///<BIT [13:11] PM_CURNT_STATE_2_0.
        uint32_t OUTBAND_PWRUP_CMD      : 1;         ///<BIT [14] OUTBAND_PWRUP_CMD.
        uint32_t LINK_TIMEOUT_FLUSH_NOT : 1;         ///<BIT [15] LINK_TIMEOUT_FLUSH_NOT.
        uint32_t APPS_PM_XMT_PME        : 1;         ///<BIT [16] APPS_PM_XMT_PME.
        uint32_t APP_SRIS_MODE          : 1;         ///<BIT [17] APP_SRIS_MODE.
        uint32_t APP_REQ_RETRY_EN       : 1;         ///<BIT [18] APP_REQ_RETRY_EN.
        uint32_t APP_REQ_EXIT_L1        : 1;         ///<BIT [19] APP_REQ_EXIT_L1.
        uint32_t APP_REQ_ENTR_L1        : 1;         ///<BIT [20] APP_REQ_ENTR_L1.
        uint32_t APP_READY_ENTR_L23     : 1;         ///<BIT [21] APP_READY_ENTR_L23.
        uint32_t APP_LTSSM_ENABLE       : 1;         ///<BIT [22] APP_LTSSM_ENABLE.
        uint32_t APP_LTR_MSG_REQ        : 1;         ///<BIT [23] APP_LTR_MSG_REQ.
        uint32_t APP_LTR_MSG_GRANT      : 1;         ///<BIT [24] APP_LTR_MSG_GRANT.
        uint32_t APP_LTR_MSG_FUNC_NUM   : 1;         ///<BIT [25] APP_LTR_MSG_FUNC_NUM.
        uint32_t APP_INIT_RST           : 1;         ///<BIT [26] APP_INIT_RST.
        uint32_t APP_FLR_VF_DONE_1_0    : 2;         ///<BIT [28:27] APP_FLR_VF_DONE_1_0.
        uint32_t APP_FLR_PF_DONE        : 1;         ///<BIT [29] APP_FLR_PF_DONE.
        uint32_t APP_CLK_PM_EN          : 1;         ///<BIT [30] APP_CLK_PM_EN.
        uint32_t RF0080000_31_RSVD      : 1;         ///<BIT [31] Reserved.
    } b;    
} PciecoreGlobal0_t;

///  @brief 0xF0080004 : Pciecore Global 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t APP_CLK_REQ_N       : 1;         ///<BIT [0] APP_CLK_REQ_N.
        uint32_t RF0080004_1_RSVD    : 1;         ///<BIT [1] Reserved.
        uint32_t SMLH_IN_RL0S        : 1;         ///<BIT [2] SMLH_IN_RL0S.
        uint32_t WAKE                : 1;         ///<BIT [3] WAKE.
        uint32_t SYS_AUX_PWR_DET     : 1;         ///<BIT [4] SYS_AUX_PWR_DET.
        uint32_t RF0080004_29_5_RSVD :25;         ///<BIT [29:05] Reserved.
        uint32_t AUTO_L1_EXIT_MSI_EN : 1;         ///<BIT [30] AUTO_L1_EXIT_MSI_EN
        uint32_t AUTO_L1_EXIT_EN     : 1;         ///<BIT [31] AUTO_L1_EXIT_EN
    } b;    
} PciecoreGlobal1_t;

///  @brief 0xF0080010 : Pciecore Cfg
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0080010_3_0_RSVD                     : 4;         ///<BIT [3:0] Reserved.
        uint32_t CFG_VF_EN                              : 1;         ///<BIT [4] CFG_VF_EN.
        uint32_t RF0080010_5_RSVD                       : 1;         ///<BIT [5] Reserved.
        uint32_t CFG_TPH_REQ_EN_1_0                     : 2;         ///<BIT [7:6] CFG_TPH_REQ_EN_1_0.
        uint32_t RF0080010_8_RSVD                       : 1;         ///<BIT [8] Reserved.
        uint32_t CFG_START_VFI                          : 1;         ///<BIT [9] CFG_START_VFI.
        uint32_t CFG_RCB                                : 1;         ///<BIT [10] CFG_RCB.
        uint32_t CFG_PM_NO_SOFT_RST                     : 1;         ///<BIT [11] CFG_PM_NO_SOFT_RST.
        uint32_t CFG_MEM_SPACE_EN                       : 1;         ///<BIT [12] CFG_MEM_SPACE_EN.
        uint32_t RF0080010_13_RSVD                      : 1;         ///<BIT [13] Reserved.
        uint32_t CFG_MAX_RD_REQ_SIZE_2_0                : 3;         ///<BIT [16:14] CFG_MAX_RD_REQ_SIZE_2_0.
        uint32_t RF0080010_17_RSVD                      : 1;         ///<BIT [17] Reserved.
        uint32_t CFG_MAX_PAYLOAD_SIZE_2_0               : 3;         ///<BIT [20:18] CFG_MAX_PAYLOAD_SIZE_2_0.
        uint32_t RF0080010_21_RSVD                      : 1;         ///<BIT [21] Reserved.
        uint32_t CFG_LTR_M_EN                           : 1;         ///<BIT [22] CFG_LTR_M_EN.
        uint32_t RF0080010_23_RSVD                      : 1;         ///<BIT [23] Reserved.
        uint32_t CFG_L1SUB_EN                           : 1;         ///<BIT [24] CFG_L1SUB_EN.
        uint32_t CFG_L1_AUX_CLK_SWITCH_CORE_CLK_GATE_EN : 1;         ///<BIT [25] CFG_L1_AUX_CLK_SWITCH_CORE_CLK_GATE_EN.
        uint32_t RF0080010_26_RSVD                      : 1;         ///<BIT [26] Reserved.
        uint32_t CFG_FLR_VF_ACTIVE_1_0                  : 2;         ///<BIT [28:27] CFG_FLR_VF_ACTIVE_1_0.
        uint32_t RF0080010_29_RSVD                      : 1;         ///<BIT [29] Reserved.
        uint32_t CFG_FLR_PF_ACTIVE                      : 1;         ///<BIT [30] CFG_FLR_PF_ACTIVE.
        uint32_t CFG_BUS_MASTER_EN                      : 1;         ///<BIT [31] CFG_BUS_MASTER_EN.
    } b;    
} PciecoreCfg_t;

///  @brief 0xF0080068 : Pciecore Cfg General
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0080068_0_RSVD     : 1;         ///<BIT [0] Reserved.
        uint32_t CFG_PBUS_NUM_7_0     : 8;         ///<BIT [8:1] CFG_PBUS_NUM_7_0.
        uint32_t CFG_PBUS_DEV_NUM_4_0 : 5;         ///<BIT [13:9] CFG_PBUS_DEV_NUM_4_0.
        uint32_t CFG_OBFF_EN_1_0      : 2;         ///<BIT [15:14] CFG_OBFF_EN_1_0.
        uint32_t CFG_NUM_VF_15_0      :16;         ///<BIT [31:16] CFG_NUM_VF_15_0.
    } b;    
} PciecoreCfgGeneral_t;

///  @brief 0xF0080074 : Pciecore Cxpl Debug 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0080074_15_0_RSVD     :16;         ///<BIT [15:0] Reserved.
        uint32_t CXPL_DEBUG_INFO_EI_15_0 :16;         ///<BIT [31:16] CXPL_DEBUG_INFO_EI_15_0.
    } b;    
} PciecoreCxplDbg2_t;

///  @brief 0xF0080080 : Pciecore Radm General 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RADM_TIMEOUT_CPL_LEN_11_0 :12;         ///<BIT [11:0] RADM_TIMEOUT_CPL_LEN_11_0.
        uint32_t RADM_TIMEOUT_CPL_ATTR_1_0 : 2;         ///<BIT [13:12] RADM_TIMEOUT_CPL_ATTR_1_0.
        uint32_t RADM_QOVERFLOW            : 1;         ///<BIT [14] RADM_QOVERFLOW.
        uint32_t RADM_Q_NOT_EMPTY          : 1;         ///<BIT [15] RADM_Q_NOT_EMPTY.
        uint32_t RADM_MSG_REQ_ID_15_0      :16;         ///<BIT [31:16] RADM_MSG_REQ_ID_15_0.
    } b;    
} PciecoreRadmGeneral0_t;

///  @brief 0xF0080084 : Pciecore Radm General 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0080084_13_0_RSVD       :14;         ///<BIT [13:0] Reserved.
        uint32_t RF0080084_14_RSVD         : 1;         ///<BIT [14] Reserved.
        uint32_t RADMX_CMPOSER_LOOKUP_ERR  : 1;         ///<BIT [15] RADMX_CMPOSER_LOOKUP_ERR.
        uint32_t RADM_CPL_TIMEOUT          : 1;         ///<BIT [16] RADM_CPL_TIMEOUT.
        uint32_t RADM_VENDOR_MSG           : 1;         ///<BIT [17] RADM_VENDOR_MSG.
        uint32_t RADM_TIMEOUT_VFUNC_NUM    : 1;         ///<BIT [18] RADM_TIMEOUT_VFUNC_NUM.
        uint32_t RADM_TIMEOUT_VFUNC_ACTIVE : 1;         ///<BIT [19] RADM_TIMEOUT_VFUNC_ACTIVE.
        uint32_t RADM_TIMEOUT_FUNC_NUM     : 1;         ///<BIT [20] RADM_TIMEOUT_FUNC_NUM.
        uint32_t RADM_TIMEOUT_CPL_TC_2_0   : 3;         ///<BIT [23:21] RADM_TIMEOUT_CPL_TC_2_0.
        uint32_t RADM_TIMEOUT_CPL_TAG_7_0  : 8;         ///<BIT [31:24] RADM_TIMEOUT_CPL_TAG_7_0.
    } b;    
} PciecoreRadmGeneral1_t;

///  @brief 0xF0080088 : Pciecore Trgt General 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0080088_13_0_RSVD        :14;         ///<BIT [13:0] Reserved.
        uint32_t TRGT_LOOKUP_ID_7_0         : 8;         ///<BIT [21:14] TRGT_LOOKUP_ID_7_0.
        uint32_t TRGT_TIMEOUT_LOOKUP_ID_7_0 : 8;         ///<BIT [29:22] TRGT_TIMEOUT_LOOKUP_ID_7_0.
        uint32_t TRGT_LOOKUP_EMPTY          : 1;         ///<BIT [30] TRGT_LOOKUP_EMPTY.
        uint32_t TRGT_CPL_TIMEOUT           : 1;         ///<BIT [31] TRGT_CPL_TIMEOUT.
    } b;    
} PciecoreTrgtGeneral0_t;

///  @brief 0xF008008C : Pciecore Trgt General 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF008008C_7_0_RSVD            : 8;         ///<BIT [7:0] Reserved.
        uint32_t TRGT_TIMEOUT_CPL_VFUNC_NUM    : 1;         ///<BIT [8] TRGT_TIMEOUT_CPL_VFUNC_NUM.
        uint32_t TRGT_TIMEOUT_CPL_VFUNC_ACTIVE : 1;         ///<BIT [9] TRGT_TIMEOUT_CPL_VFUNC_ACTIVE.
        uint32_t RF008008C_10_RSVD             : 1;         ///<BIT [10] Reserved.
        uint32_t TRGT_TIMEOUT_CPL_TC_2_0       : 3;         ///<BIT [13:11] TRGT_TIMEOUT_CPL_TC_2_0.
        uint32_t RF008008C_14_RSVD             : 1;         ///<BIT [14] Reserved.
        uint32_t TRGT_TIMEOUT_CPL_LEN_11_0     :12;         ///<BIT [26:15] TRGT_TIMEOUT_CPL_LEN_11_0.
        uint32_t RF008008C_27_RSVD             : 1;         ///<BIT [27] Reserved.
        uint32_t TRGT_TIMEOUT_CPL_FUNC_NUM     : 1;         ///<BIT [28] TRGT_TIMEOUT_CPL_FUNC_NUM.
        uint32_t RF008008C_29_RSVD             : 1;         ///<BIT [29] Reserved.
        uint32_t TRGT_TIMEOUT_CPL_ATTR_1_0     : 2;         ///<BIT [31:30] TRGT_TIMEOUT_CPL_ATTR_1_0.
    } b;    
} PciecoreTrgtGeneral1_t;

///  @brief 0xF00800A0 : Pciecore Ven Msg General 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VEN_MSG_TAG_7_0   : 8;         ///<BIT [7:0] VEN_MSG_TAG_7_0.
        uint32_t VEN_MSG_REQ       : 1;         ///<BIT [8] VEN_MSG_REQ.
        uint32_t VEN_MSG_LEN_9_0   :10;         ///<BIT [18:9] VEN_MSG_LEN_9_0.
        uint32_t RF00800A0_19_RSVD : 1;         ///<BIT [19] Reserved.
        uint32_t VEN_MSG_FUNC_NUM  : 1;         ///<BIT [20] VEN_MSG_FUNC_NUM.
        uint32_t VEN_MSG_FMT_1_0   : 2;         ///<BIT [22:21] VEN_MSG_FMT_1_0.
        uint32_t VEN_MSG_EP        : 1;         ///<BIT [23] VEN_MSG_EP.
        uint32_t VEN_MSG_CODE_7_0  : 8;         ///<BIT [31:24] VEN_MSG_CODE_7_0.
    } b;    
} PciecoreVenMsgGeneral0_t;

///  @brief 0xF00800A4 : Pciecore Ven Msg General 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00800A4_18_0_RSVD  :19;         ///<BIT [18:0] Reserved.
        uint32_t VEN_MSG_ATTR_1_0     : 2;         ///<BIT [20:19] VEN_MSG_ATTR_1_0.
        uint32_t VEN_MSG_VFUNC_NUM    : 1;         ///<BIT [21] VEN_MSG_VFUNC_NUM.
        uint32_t VEN_MSG_VFUNC_ACTIVE : 1;         ///<BIT [22] VEN_MSG_VFUNC_ACTIVE.
        uint32_t VEN_MSG_TYPE_4_0     : 5;         ///<BIT [27:23] VEN_MSG_TYPE_4_0.
        uint32_t VEN_MSG_TD           : 1;         ///<BIT [28] VEN_MSG_TD.
        uint32_t VEN_MSG_TC_2_0       : 3;         ///<BIT [31:29] VEN_MSG_TC_2_0.
    } b;    
} PciecoreVenMsgGeneral1_t;

///  @brief 0xF00800A8 : Pciecore Global 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CFG_FLR_VF_ACTIVE_STATUS_1_0 : 2;         ///<BIT [1:0] CFG_FLR_VF_ACTIVE_STATUS_1_0.
        uint32_t RF00800A8_7_2_RSVD           : 6;         ///<BIT [7:2] Reserved.
        uint32_t CFG_FLR_PF_ACTIVE_STATUS     : 1;         ///<BIT [8] CFG_FLR_PF_ACTIVE_STATUS.
        uint32_t RF00800A8_11_9_RSVD          : 3;         ///<BIT [11:9] Reserved.
        uint32_t AMBA_ORDR_MGR_WDT_INT_STATUS : 1;         ///<BIT [12] AMBA_ORDR_MGR_WDT_INT_STATUS.
        uint32_t CFG_LINK_EQ_REQ_INT_STATUS   : 1;         ///<BIT [13] CFG_LINK_EQ_REQ_INT_STATUS.
        uint32_t CFG_VPD_INT_STATUS           : 1;         ///<BIT [14] CFG_VPD_INT_STATUS.
        uint32_t SYS_INT_STATUS               : 1;         ///<BIT [15] SYS_INT_STATUS.
        uint32_t CFG_FLR_VF_ACTIVE_1_0        : 2;         ///<BIT [17:16] CFG_FLR_VF_ACTIVE_1_0.
        uint32_t RF00800A8_23_18_RSVD         : 6;         ///<BIT [23:18] Reserved.
        uint32_t CFG_FLR_PF_ACTIVE            : 1;         ///<BIT [24] CFG_FLR_PF_ACTIVE.
        uint32_t RF00800A8_27_25_RSVD         : 3;         ///<BIT [27:25] Reserved.
        uint32_t AMBA_ORDR_MGR_WDT_INT        : 1;         ///<BIT [28] AMBA_ORDR_MGR_WDT_INT.
        uint32_t CFG_LINK_EQ_REQ_INT          : 1;         ///<BIT [29] CFG_LINK_EQ_REQ_INT.
        uint32_t CFG_VPD_INT                  : 1;         ///<BIT [30] CFG_VPD_INT.
        uint32_t RF00800A8_31_RSVD            : 1;         ///<BIT [31] Reserved.
    } b;    
} PciecoreGlobal2_t;

///  @brief 0xF00800AC : Pciecore Global 3
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00800AC_15_0_RSVD        :16;         ///<BIT [15:0] Reserved.
        uint32_t CFG_FLR_VF_ACTIVE_MASK_1_0 : 2;         ///<BIT [17:16] CFG_FLR_VF_ACTIVE_MASK_1_0.
        uint32_t RF00800AC_23_18_RSVD       : 6;         ///<BIT [23:18] Reserved.
        uint32_t CFG_FLR_PF_ACTIVE_MASK     : 1;         ///<BIT [24] CFG_FLR_PF_ACTIVE_MASK.
        uint32_t RF00800AC_27_25_RSVD       : 3;         ///<BIT [27:25] Reserved.
        uint32_t AMBA_ORDR_MGR_WDT_INT_MASK : 1;         ///<BIT [28] AMBA_ORDR_MGR_WDT_INT_MASK.
        uint32_t CFG_LINK_EQ_REQ_INT_MASK   : 1;         ///<BIT [29] CFG_LINK_EQ_REQ_INT_MASK.
        uint32_t CFG_VPD_INT_MASK           : 1;         ///<BIT [30] CFG_VPD_INT_MASK.
        uint32_t RF00800AC_31_RSVD          : 1;         ///<BIT [31] Reserved.
    } b;    
} PciecoreGlobal3_t;

///  @brief 0xF00800C0 : Pciecore Refclk Rx
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00800C0_1_0_RSVD        : 2;         ///<BIT [1:0] Reserved.
        uint32_t REFCLK_RX_EN              : 1;         ///<BIT [2] REFCLK_RX_EN.
        uint32_t REFCLK_RX_EN_FWCTRL       : 1;         ///<BIT [3] REFCLK_RX_EN_FWCTRL.
        uint32_t RF00800C0_7_4_RSVD        : 4;         ///<BIT [7:4] Reserved.
        uint32_t REFCLK_RX_LOWOFFSET_MODE  : 1;         ///<BIT [8] REFCLK_RX_LOWOFFSET_MODE.
        uint32_t REFCLK_RX_OFFSET_POLARITY : 1;         ///<BIT [9] REFCLK_RX_OFFSET_POLARITY.
        uint32_t REFCLK_RX_OFFSET_EN       : 1;         ///<BIT [10] REFCLK_RX_OFFSET_EN.
        uint32_t REFCLK_RX_SEL_EXT         : 1;         ///<BIT [11] REFCLK_RX_SEL_EXT.
        uint32_t REFCLK_RX_PU              : 1;         ///<BIT [12] REFCLK_RX_PU.
        uint32_t REFCLK_RX_TEST_ANA_2_0    : 3;         ///<BIT [15:13] REFCLK_RX_TEST_ANA_2_0.
        uint32_t RF00800C0_31_16_RSVD      :16;         ///<BIT [31:16] Reserved.
    } b;    
} PciecoreRefclkRx_t;

///  @brief 0xF00800C4 : Pciecore Etc
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SEL_DIR0_FOM1       : 1;         ///<BIT [0] SEL_DIR0_FOM1.
        uint32_t RF00800C4_31_1_RSVD :31;         ///<BIT [31:01] Reserved.
    } b;    
} PciecoreEtc_t;

///  @brief 0xF00800D0 : Pciecore Memory Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF00800D0_15_0_RSVD    :16;         ///<BIT [15:0] Reserved.
        uint32_t PCORE_MEM_RTC_SR2P_1_0 : 2;         ///<BIT [17:16] PCORE_MEM_RTC_SR2P_1_0.
        uint32_t PCORE_MEM_WTC_SR2P_1_0 : 2;         ///<BIT [19:18] PCORE_MEM_WTC_SR2P_1_0.
        uint32_t PCORE_MEM_RTC_RF2P_1_0 : 2;         ///<BIT [21:20] PCORE_MEM_RTC_RF2P_1_0.
        uint32_t PCORE_MEM_WTC_RF2P_1_0 : 2;         ///<BIT [23:22] PCORE_MEM_WTC_RF2P_1_0.
        uint32_t PCORE_MEM_RTC_RF1P_1_0 : 2;         ///<BIT [25:24] PCORE_MEM_RTC_RF1P_1_0.
        uint32_t PCORE_MEM_WTC_RF1P_1_0 : 2;         ///<BIT [27:26] PCORE_MEM_WTC_RF1P_1_0.
        uint32_t RF00800D0_28_RSVD      : 1;         ///<BIT [28] Reserved.
        uint32_t PCORE_MEM_PDFVSSM      : 1;         ///<BIT [29] PCORE_MEM_PDFVSSM.
        uint32_t PCORE_MEM_PDLVMC       : 1;         ///<BIT [30] PCORE_MEM_PDLVMC.
        uint32_t PCORE_MEM_PDWN         : 1;         ///<BIT [31] PCORE_MEM_PDWN.
    } b;    
} PciecoreMemCtrl_t;

///  @brief 0xF00800D4 : Pciecore Is (Interrupt Status)
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FLR_PF_ACTIVE             : 1;       ///<BIT [0] Set when software initiates an FLR on a physical function.
        uint32_t FLR_VF_ACTIVE_0           : 1;       ///<BIT [1] Set when software initiates an FLR on a virtual function 0.
        uint32_t FLR_VF_ACTIVE_1           : 1;       ///<BIT [2] Set when software initiates an FLR on a virtual function 1.
        uint32_t LINK_REQ_RST_NOT          : 1;       ///<BIT [3] The Falling Edge Detection of Link Requester Edge Detector.
        uint32_t RADM_PM_TURNOFF           : 1;       ///<BIT [4] Received PM_TURNOFF.
        uint32_t D3HOT_EXIT                : 1;       ///<BIT [5] The Falling Edge Detection of Power Management Edge Detector.
        uint32_t D3HOT_ENTER               : 1;       ///<BIT [6] The Rising Edge Detection of Power Management Edge Detector.
        uint32_t RDLH_BAD_DLLP_ERR         : 1;       ///<BIT [7] Data link layer receive bad DLLP error.
        uint32_t RDLH_BAD_TLP_ERR          : 1;       ///<BIT [8] Data link layer receive bad TLP error.
        uint32_t RMLH_RCVD_ERR             : 1;       ///<BIT [9] PHY receiving error reported from pipe interface.
        uint32_t XDLH_REPLAY_TIMEOUT_ERR   : 1;       ///<BIT [10] Data link layer transmit replay timeout.
        uint32_t XDLH_REPLAY_NUM_RLOVER_ERR: 1;       ///<BIT [11] Data link layer transmit replay number rollover.
        uint32_t RTLH_FC_PROT_ERR          : 1;       ///<BIT [12] FC protocal error. Currently it is asserted only when watch dog timer expired.
        uint32_t RDLH_PROT_ERR             : 1;       ///<BIT [13] Data link layer protocol error.
        uint32_t RADM_CPL_TIMEOUT2         : 1;       ///<BIT [14] CPL is timeout.
        uint32_t RADM_QOVERFLOW2           : 1;       ///<BIT [15] Received Queue overflow.
        uint32_t RADM_UNEXP_CPL_ERR        : 1;       ///<BIT [16] timeout CPL tc (CPL -> cdm).
        uint32_t RADM_RCVD_CPL_UR          : 1;       ///<BIT [17] Received CPL Unsupported request error (CPL -> cdm).
        uint32_t RADM_RCVD_CPL_CA          : 1;       ///<BIT [18] Received CPL completion abort (CPL -> cdm).
        uint32_t RADM_RCVD_REQ_CA          : 1;       ///<BIT [19] Received completion abort (EP's CA generated for dwlen>1).
        uint32_t RADM_RCVD_REQ_UR          : 1;       ///<BIT [20] Received unsupported Request.
        uint32_t RADM_ECRC_ERR             : 1;       ///<BIT [21] Received ECRC error (in absence of dllp error).
        uint32_t RADM_MLF_TLP_ERR          : 1;       ///<BIT [22] Received malformed error.
        uint32_t RADM_RCVD_CPL_POISONED    : 1;       ///<BIT [23] Received posted poisoned cpl tlp request.
        uint32_t RADM_RCVD_WREQ_POISONED   : 1;       ///<BIT [24] Received posted poisoned wr request.
        uint32_t LTSSM_L2_EXIT             : 1;       ///<BIT [25] LTSSM Exit from L2 State 0.
        uint32_t LTSSM_L1_2_EXIT           : 1;       ///<BIT [26] LTSSM Exit from L1.2 State.
        uint32_t LTSSM_L1_2_ENTER          : 1;       ///<BIT [27] LTSSM Transition to L1.2 State.
        uint32_t LTSSM_L1_1_EXIT           : 1;       ///<BIT [28] LTSSM Exit from L1.1 State.
        uint32_t LTSSM_L1_1_ENTER          : 1;       ///<BIT [29] LTSSM Transition to L1.1 State.
        uint32_t LTSSM_L1_EXIT             : 1;       ///<BIT [30] LTSSM Exit from L1 State.
        uint32_t LTSSM_L1_ENTER            : 1;       ///<BIT [31] LTSSM Transition to L1 State.
    } b;    
} PciecoreIs_t;

///  @brief 0xF00800D8 : Pciecore Ie (Interrupt Enable)
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FLR_PF_ACTIVE             : 1;       ///<BIT [0] Set when software initiates an FLR on a physical function.
        uint32_t FLR_VF_ACTIVE_0           : 1;       ///<BIT [1] Set when software initiates an FLR on a virtual function 0.
        uint32_t FLR_VF_ACTIVE_1           : 1;       ///<BIT [2] Set when software initiates an FLR on a virtual function 1.
        uint32_t LINK_REQ_RST_NOT          : 1;       ///<BIT [3] The Falling Edge Detection of Link Requester Edge Detector.
        uint32_t RADM_PM_TURNOFF           : 1;       ///<BIT [4] Received PM_TURNOFF.
        uint32_t D3HOT_EXIT                : 1;       ///<BIT [5] The Falling Edge Detection of Power Management Edge Detector.
        uint32_t D3HOT_ENTER               : 1;       ///<BIT [6] The Rising Edge Detection of Power Management Edge Detector.
        uint32_t RDLH_BAD_DLLP_ERR         : 1;       ///<BIT [7] Data link layer receive bad DLLP error.
        uint32_t RDLH_BAD_TLP_ERR          : 1;       ///<BIT [8] Data link layer receive bad TLP error.
        uint32_t RMLH_RCVD_ERR             : 1;       ///<BIT [9] PHY receiving error reported from pipe interface.
        uint32_t XDLH_REPLAY_TIMEOUT_ERR   : 1;       ///<BIT [10] Data link layer transmit replay timeout.
        uint32_t XDLH_REPLAY_NUM_RLOVER_ERR: 1;       ///<BIT [11] Data link layer transmit replay number rollover.
        uint32_t RTLH_FC_PROT_ERR          : 1;       ///<BIT [12] FC protocal error. Currently it is asserted only when watch dog timer expired.
        uint32_t RDLH_PROT_ERR             : 1;       ///<BIT [13] Data link layer protocol error.
        uint32_t RADM_CPL_TIMEOUT2         : 1;       ///<BIT [14] CPL is timeout.
        uint32_t RADM_QOVERFLOW2           : 1;       ///<BIT [15] Received Queue overflow.
        uint32_t RADM_UNEXP_CPL_ERR        : 1;       ///<BIT [16] timeout CPL tc (CPL -> cdm).
        uint32_t RADM_RCVD_CPL_UR          : 1;       ///<BIT [17] Received CPL Unsupported request error (CPL -> cdm).
        uint32_t RADM_RCVD_CPL_CA          : 1;       ///<BIT [18] Received CPL completion abort (CPL -> cdm).
        uint32_t RADM_RCVD_REQ_CA          : 1;       ///<BIT [19] Received completion abort (EP's CA generated for dwlen>1).
        uint32_t RADM_RCVD_REQ_UR          : 1;       ///<BIT [20] Received unsupported Request.
        uint32_t RADM_ECRC_ERR             : 1;       ///<BIT [21] Received ECRC error (in absence of dllp error).
        uint32_t RADM_MLF_TLP_ERR          : 1;       ///<BIT [22] Received malformed error.
        uint32_t RADM_RCVD_CPL_POISONED    : 1;       ///<BIT [23] Received posted poisoned cpl tlp request.
        uint32_t RADM_RCVD_WREQ_POISONED   : 1;       ///<BIT [24] Received posted poisoned wr request.
        uint32_t LTSSM_L2_EXIT             : 1;       ///<BIT [25] LTSSM Exit from L2 State 0.
        uint32_t LTSSM_L1_2_EXIT           : 1;       ///<BIT [26] LTSSM Exit from L1.2 State.
        uint32_t LTSSM_L1_2_ENTER          : 1;       ///<BIT [27] LTSSM Transition to L1.2 State.
        uint32_t LTSSM_L1_1_EXIT           : 1;       ///<BIT [28] LTSSM Exit from L1.1 State.
        uint32_t LTSSM_L1_1_ENTER          : 1;       ///<BIT [29] LTSSM Transition to L1.1 State.
        uint32_t LTSSM_L1_EXIT             : 1;       ///<BIT [30] LTSSM Exit from L1 State.
        uint32_t LTSSM_L1_ENTER            : 1;       ///<BIT [31] LTSSM Transition to L1 State.
    } b;    
} PciecoreIe_t;

///  @brief 0xF00800DC : Pciecore Ltr Msg Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LTR_FW_REQ          : 1;         ///<BIT [0] LTR_FW_REQ.
        uint32_t LTR_FW_VECTOR_SEL   : 2;         ///<BIT [2:1] LTR_FW_VECTOR_SEL.
        uint32_t RF00800DC_29_3_RSVD :27;         ///<BIT [29:3] Reserved.
        uint32_t LTR_L1_2_EN         : 1;         ///<BIT [30] LTR_L1_2_EN.
        uint32_t LTR_EN_EN           : 1;         ///<BIT [31] LTR_EN_EN.
    } b;    
} PciecoreLtrMsgCtrl_t;


/// @brief 0xF0080000-0xF00800F8 : PcieCore_t MODULE
typedef struct
{
    PciecoreGlobal0_t pciecoreGlobal0;                   ///< 0xF0080000 : Pciecore Global 0
    PciecoreGlobal1_t pciecoreGlobal1;                   ///< 0xF0080004 : Pciecore Global 1
    uint32_t APP_LTR_LATENCY_31_0;                       ///< 0xF0080008 : Pciecore Ltr Latency
    uint32_t APP_LTR_MSG_LATENCY_31_0;                   ///< 0xF008000C : Pciecore Ltr Msg Latency
    PciecoreCfg_t pciecoreCfg;                           ///< 0xF0080010 : Pciecore Cfg
    uint32_t CFG_BAR0_LIMIT_63_32;                       ///< 0xF0080014 : Pciecore Cfg Bar0 Limit 0
    uint32_t CFG_BAR0_LIMIT_31_0;                        ///< 0xF0080018 : Pciecore Cfg Bar0 Limit 1
    uint32_t CFG_BAR0_START_63_32;                       ///< 0xF008001C : Pciecore Cfg Bar0 Start 0
    uint32_t CFG_BAR0_START_31_0;                        ///< 0xF0080020 : Pciecore Cfg Bar0 Start 1
    uint32_t CFG_BAR1_LIMIT_31_0;                        ///< 0xF0080024 : Pciecore Cfg Bar1 Limit
    uint32_t CFG_BAR1_START_31_0;                        ///< 0xF0080028 : Pciecore Cfg Bar1 Start
    uint32_t CFG_BAR2_LIMIT_63_32;                       ///< 0xF008002C : Pciecore Cfg Bar2 Limit 0
    uint32_t CFG_BAR2_LIMIT_31_0;                        ///< 0xF0080030 : Pciecore Cfg Bar2 Limit 1
    uint32_t CFG_BAR2_START_63_32;                       ///< 0xF0080034 : Pciecore Cfg Bar2 Start 0
    uint32_t CFG_BAR2_START_31_0;                        ///< 0xF0080038 : Pciecore Cfg Bar2 Start 1
    uint32_t CFG_BAR3_LIMIT_31_0;                        ///< 0xF008003C : Pciecore Cfg Bar3 Limit
    uint32_t CFG_BAR3_START_31_0;                        ///< 0xF0080040 : Pciecore Cfg Bar3 Start
    uint32_t CFG_BAR4_LIMIT_63_32;                       ///< 0xF0080044 : Pciecore Cfg Bar4 Limit 0
    uint32_t CFG_BAR4_LIMIT_31_0;                        ///< 0xF0080048 : Pciecore Cfg Bar4 Limit 1
    uint32_t CFG_BAR4_START_63_32;                       ///< 0xF008004C : Pciecore Cfg Bar4 Start 0
    uint32_t CFG_BAR4_START_31_0;                        ///< 0xF0080050 : Pciecore Cfg Bar4 Start 1
    uint32_t CFG_BAR5_LIMIT_31_0;                        ///< 0xF0080054 : Pciecore Cfg Bar5 Limit
    uint32_t CFG_BAR5_START_31_0;                        ///< 0xF0080058 : Pciecore Cfg Bar5 Start
    uint32_t CFG_EXP_ROM_LIMIT_31_0;                     ///< 0xF008005C : Pciecore Cfg Exp Rom Limit
    uint32_t CFG_EXP_ROM_START_31_0;                     ///< 0xF0080060 : Pciecore Cfg Exp Rom Start
    uint32_t CFG_LTR_MAX_LATENCY_31_0;                   ///< 0xF0080064 : Pciecore Cfg Ltr Max Latency
    PciecoreCfgGeneral_t pciecoreCfgGeneral;             ///< 0xF0080068 : Pciecore Cfg General
    uint32_t CXPL_DEBUG_INFO_63_32;                      ///< 0xF008006C : Pciecore Cxpl Debug 0
    uint32_t CXPL_DEBUG_INFO_31_0;                       ///< 0xF0080070 : Pciecore Cxpl Debug 1
    PciecoreCxplDbg2_t pciecoreCxplDbg2;                 ///< 0xF0080074 : Pciecore Cxpl Debug 2
    uint32_t RADM_MSG_PAYLOAD_63_32;                     ///< 0xF0080078 : Pciecore Radm Msg Payload 0
    uint32_t RADM_MSG_PAYLOAD_31_0;                      ///< 0xF008007C : Pciecore Radm Msg Payload 1
    PciecoreRadmGeneral0_t pciecoreRadmGeneral0;         ///< 0xF0080080 : Pciecore Radm General 0
    PciecoreRadmGeneral1_t pciecoreRadmGeneral1;         ///< 0xF0080084 : Pciecore Radm General 1
    PciecoreTrgtGeneral0_t pciecoreTrgtGeneral0;         ///< 0xF0080088 : Pciecore Trgt General 0
    PciecoreTrgtGeneral1_t pciecoreTrgtGeneral1;         ///< 0xF008008C : Pciecore Trgt General 1
    uint32_t VEN_MSG_DATA_63_32;                         ///< 0xF0080090 : Pciecore Ven Msg Data 0
    uint8_t rsvd_RF0080094[8];                           ///< 0xF0080094 - 0xF008009B : Reserved.
    uint32_t VEN_MSG_DATA_31_0;                          ///< 0xF008009C : Pciecore Ven Msg Data 1
    PciecoreVenMsgGeneral0_t pciecoreVenMsgGeneral0;     ///< 0xF00800A0 : Pciecore Ven Msg General 0
    PciecoreVenMsgGeneral1_t pciecoreVenMsgGeneral1;     ///< 0xF00800A4 : Pciecore Ven Msg General 1
    PciecoreGlobal2_t pciecoreGlobal2;                   ///< 0xF00800A8 : Pciecore Global 2
    PciecoreGlobal3_t pciecoreGlobal3;                   ///< 0xF00800AC : Pciecore Global 3
    uint8_t rsvd_RF00800B0[16];                          ///< 0xF00800B0 - 0xF00800BF : Reserved.
    PciecoreRefclkRx_t pciecoreRefclkRx;                 ///< 0xF00800C0 : Pciecore Refclk Rx
    PciecoreEtc_t pciecoreEtc;                           ///< 0xF00800C4 : Pciecore Etc
    uint8_t rsvd_RF00800C8[8];                           ///< 0xF00800C8 - 0xF00800CF : Reserved.
    PciecoreMemCtrl_t pciecoreMemCtrl;                   ///< 0xF00800D0 : Pciecore Memory Control
    PciecoreIs_t pciecoreIs;                             ///< 0xF00800D4 : Pciecore Is
    PciecoreIe_t pciecoreIe;                             ///< 0xF00800D8 : Pciecore Ie
    PciecoreLtrMsgCtrl_t pciecoreLtrMsgCtrl;             ///< 0xF00800DC : Pciecore Ltr Msg Control
    uint32_t PCIECORE_LTR_EN_VALUE;                      ///< 0xF00800E0 : Pciecore Ltr En Value
    uint32_t PCIECORE_LTR_L1_2_VALUE;                    ///< 0xF00800E4 : Pciecore Ltr L1 2 Value
    uint32_t PCIECORE_LTR_FW0_VALUE;                     ///< 0xF00800E8 : Pciecore Ltr Fw0 Value
    uint32_t PCIECORE_LTR_FW1_VALUE;                     ///< 0xF00800EC : Pciecore Ltr Fw1 Value
    uint32_t PCIECORE_LTR_FW2_VALUE;                     ///< 0xF00800F0 : Pciecore Ltr Fw2 Value
    uint32_t PCIECORE_LTR_FW3_VALUE;                     ///< 0xF00800F4 : Pciecore Ltr Fw3 Value
} PcieCore_t;
COMPILE_ASSERT(offsetof(PcieCore_t,PCIECORE_LTR_FW3_VALUE)==0xF00800F4-0xF0080000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile PcieCore_t rPcieCore;                ///< 0xF0080000-0xF00800F8 : PcieCore_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

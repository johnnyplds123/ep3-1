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
//! @brief MBIST Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  MBIST Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF0115180 : Temperature Sensor Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF0115180_7_0_RSVD  : 8;         ///<BIT [7:0] Reserved.
        uint32_t RF0115180_11_8_RSVD : 4;         ///<BIT [11:8] Temperature Sensor Hold.
        uint32_t TSEN_RESET          : 1;         ///<BIT [12] Temperature Sensor Reset.
        uint32_t TSEN_ATEST_SEL      : 2;         ///<BIT [14:13] Temperature Sensor Analog Test Select.
        uint32_t TSEN_RAW_SEL        : 2;         ///<BIT [16:15] Temperature Sensor Raw Data Select.
        uint32_t TSEN_CAL            : 2;         ///<BIT [18:17] Temperature Sensor Calibration Selection.
        uint32_t TSEN_BG_TRIM        : 4;         ///<BIT [22:19] Temperature Sensor Band-Gap Trim.
        uint32_t TSEN_CHOP_EN        : 2;         ///<BIT [24:23] Temperature Sensor Chopper Enable.
        uint32_t TSEN_EXT_EN         : 1;         ///<BIT [25] Temperature Sensor External Enable.
        uint32_t TSEN_AVG_NUM        : 3;         ///<BIT [28:26] Temperature Sensor Average.
        uint32_t TSEN_START          : 1;         ///<BIT [29] Temperature Sensor Start Control.
        uint32_t TSEN_EN             : 1;         ///<BIT [30] Temperature Sensor Enable Control.
        uint32_t TSEN_ISO_EN         : 1;         ///<BIT [31] Temperature Sensor Domain Isolation Control.
    } b;    
} TempSensorCtrl0_t;

///  @brief 0xF0115184 : Temperature Sensor Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TSEN_2_HOT_INT_THR       :12;         ///<BIT [11:0] Temperature Sensor Too Hot Interrupt Threshold.
        uint32_t RF0115184_12_RSVD        : 1;         ///<BIT [12] Reserved.
        uint32_t TSEN_2_CLD_INT_THR       :12;         ///<BIT [24:13] Temperature Sensor Too Cold Interrupt Threshold.
        uint32_t RF0115184_25_RSVD        : 1;         ///<BIT [25] Reserved.
        uint32_t TSEN_SAMPL_DONE_PEND_CLR : 1;         ///<BIT [26] Sample Done Interrupt Pending Clear.
        uint32_t TSEN_2_CLD_PEND_CLR      : 1;         ///<BIT [27] Temperature Sensor Too Cold Interrupt Pending Clear.
        uint32_t TSEN_2_HOT_PEND_CLR      : 1;         ///<BIT [28] Too Hot Interrupt Pending Clear.
        uint32_t RF0115184_31_29_RSVD     : 3;         ///<BIT [31:29] Reserved.
    } b;    
} TempSensorCtrl1_t;

///  @brief 0xF0115188 : Temperature Sensor Status 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TSEN_DATA_PA_CAL     :12;         ///<BIT [11:0] Temperature Sensor Post-Average and Calibration Data.
        uint32_t RF0115188_12_RSVD    : 1;         ///<BIT [12] Reserved.
        uint32_t TSEN_DATA_RAW        :12;         ///<BIT [24:13] Temperature Sensor Raw Data.
        uint32_t RF0115188_30_25_RSVD : 6;         ///<BIT [30:25] Reserved.
        uint32_t TSEN_RDY             : 1;         ///<BIT [31] Temperature Sensor Ready.
    } b;    
} TempSensorStat0_t;

///  @brief 0xF011518C : Temperature Sensor Status 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TSEN_DATA_RDY        :12;         ///<BIT [11:0] Temperature Sensor Ready Data.
        uint32_t RF011518C_12_RSVD    : 1;         ///<BIT [12] Reserved.
        uint32_t TSEN_2_HOT_PEND      : 1;         ///<BIT [13] TSEN Too Hot Interrupt Pending.
        uint32_t TSEN_2_CLD_PEND      : 1;         ///<BIT [14] TSEN Too Cold Interrupt Pending.
        uint32_t TSEN_SAMPL_DONE_PEND : 1;         ///<BIT [15] TSEN Sample Done Interrupt Pending.
        uint32_t RF011518C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} TempSensorStat1_t;

///  @brief 0xF0115200 : DDR MBIST Control 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t P0_WDATA_BIST_RST_N   : 1;         ///<BIT [0] DDR P0_WDATA BIST Reset.
        uint32_t P0_WDATA_RUNSTOP      : 1;         ///<BIT [1] DDR P0_WDATA BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t P0_WDATA_ROWCOLSWITCH : 1;         ///<BIT [2] DDR P0_WDATA BIST Row-Column Switch.
        uint32_t P0_WDATA_BISTSTT_IN   : 1;         ///<BIT [3] DDR P0_WDATA BIST Status Input.
        uint32_t P0_WDATA_BISTSTT_CEN  : 1;         ///<BIT [4] DDR P0_WDATA BIST Status Enable.
        uint32_t P0_WDATA_BISTSTT_CLK  : 1;         ///<BIT [5] DDR P0_WDATA BIST Status Clock.
        uint32_t P0_WDATA_BISTMODE     : 1;         ///<BIT [6] DDR P0_WDATA BIST Mode Enable.
        uint32_t RF0115200_7_RSVD      : 1;         ///<BIT [7] Reserved.
        uint32_t P0_ROB_BIST_RST_N     : 1;         ///<BIT [8] DDR P0_ROB BIST Reset.
        uint32_t P0_ROB_RUNSTOP        : 1;         ///<BIT [9] DDR P0_ROB BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t P0_ROB_ROWCOLSWITCH   : 1;         ///<BIT [10] DDR P0_ROB BIST Row-Column Switch.
        uint32_t P0_ROB_BISTSTT_IN     : 1;         ///<BIT [11] DDR P0_ROB BIST Status Input.
        uint32_t P0_ROB_BISTSTT_CEN    : 1;         ///<BIT [12] DDR P0_ROB BIST Status Enable.
        uint32_t P0_ROB_BISTSTT_CLK    : 1;         ///<BIT [13] DDR P0_ROB BIST Status Clock.
        uint32_t P0_ROB_BISTMODE       : 1;         ///<BIT [14] DDR P0_ROB BIST Mode Enable.
        uint32_t RF0115200_15_RSVD     : 1;         ///<BIT [15] Reserved.
        uint32_t P1_WDATA_BIST_RST_N   : 1;         ///<BIT [16] DDR P1_WDATA BIST Reset.
        uint32_t P1_WDATA_RUNSTOP      : 1;         ///<BIT [17] DDR P1_WDATA BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t P1_WDATA_ROWCOLSWITCH : 1;         ///<BIT [18] DDR P1_WDATA BIST Row-Column Switch.
        uint32_t P1_WDATA_BISTSTT_IN   : 1;         ///<BIT [19] DDR P1_WDATA BIST Status Input.
        uint32_t P1_WDATA_BISTSTT_CEN  : 1;         ///<BIT [20] DDR P1_WDATA BIST Status Enable.
        uint32_t P1_WDATA_BISTSTT_CLK  : 1;         ///<BIT [21] DDR P1_WDATA BIST Status Clock.
        uint32_t P1_WDATA_BISTMODE     : 1;         ///<BIT [22] DDR P1_WDATA BIST Mode Enable.
        uint32_t RF0115200_23_RSVD     : 1;         ///<BIT [23] Reserved.
        uint32_t P1_ROB_BIST_RST_N     : 1;         ///<BIT [24] DDR P1_ROB BIST Reset.
        uint32_t P1_ROB_RUNSTOP        : 1;         ///<BIT [25] DDR P1_ROB BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t P1_ROB_ROWCOLSWITCH   : 1;         ///<BIT [26] DDR P1_ROB BIST Row Column Switch.
        uint32_t P1_ROB_BISTSTT_IN     : 1;         ///<BIT [27] DDR P1_ROB BIST Status Input.
        uint32_t P1_ROB_BISTSTT_CEN    : 1;         ///<BIT [28] DDR P1_ROB BIST Status Enable.
        uint32_t P1_ROB_BISTSTT_CLK    : 1;         ///<BIT [29] DDR P1_ROB BIST Status Clock.
        uint32_t P1_ROB_BISTMD         : 1;         ///<BIT [30] DDR P1_ROB BIST Mode Enable.
        uint32_t RF0115200_31_RSVD     : 1;         ///<BIT [31] Reserved.
    } b;    
} DdrMbistCtrl0_t;

///  @brief 0xF0115204 : DDR MBIST Control 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t WDB_BIST_RST_N        : 1;         ///<BIT [0] DDR WDB BIST Reset.
        uint32_t WDB_RUNSTOP           : 1;         ///<BIT [1] DDR WDB BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t WDB_ROWCOLSWITCH      : 1;         ///<BIT [2] DDR WDB BIST Row-Column Switch.
        uint32_t WDB_BISTSTT_IN        : 1;         ///<BIT [3] DDR WDB BIST Status Input.
        uint32_t WDB_BISTSTT_CEN       : 1;         ///<BIT [4] DDR WDB BIST Status Enable.
        uint32_t WDB_BISTSTT_CLK       : 1;         ///<BIT [5] DDR WDB BIST Status Clock.
        uint32_t WDB_BISTMODE          : 1;         ///<BIT [6] DDR WDB BIST Mode Enable.
        uint32_t RF0115204_7_RSVD      : 1;         ///<BIT [7] Reserved.
        uint32_t RU_BIST_RST_N         : 1;         ///<BIT [8] DDR RU BIST Reset.
        uint32_t RU_RUNSTOP            : 1;         ///<BIT [9] DDR RU BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t RU_ROWCOLSWITCH       : 1;         ///<BIT [10] DDR RU BIST Row-Column Switch.
        uint32_t RU_BISTSTT_IN         : 1;         ///<BIT [11] DDR RU BIST Status Input.
        uint32_t RU_BISTSTT_CEN        : 1;         ///<BIT [12] DDR RU BIST Status Enable.
        uint32_t RU_BISTSTT_CLK        : 1;         ///<BIT [13] DDR RU BIST Status Clock.
        uint32_t RU_BISTMODE           : 1;         ///<BIT [14] DDR RU BIST Mode Enable.
        uint32_t RF0115204_15_RSVD     : 1;         ///<BIT [15] Reserved.
        uint32_t P2_WDATA_BIST_RST_N   : 1;         ///<BIT [16] DDR P2_WDATA BIST Reset.
        uint32_t P2_WDATA_RUNSTOP      : 1;         ///<BIT [17] DDR P2_WDATA BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t P2_WDATA_ROWCOLSWITCH : 1;         ///<BIT [18] DDR P2_WDATA BIST Row-Column Switch.
        uint32_t P2_WDATA_BISTSTT_IN   : 1;         ///<BIT [19] DDR P2_WDATA BIST Status Input.
        uint32_t P2_WDATA_BISTSTT_CEN  : 1;         ///<BIT [20] DDR P2_WDATA BIST Status Enable.
        uint32_t P2_WDATA_BISTSTT_CLK  : 1;         ///<BIT [21] DDR P2_WDATA BIST Status Clock.
        uint32_t P2_WDATA_BISTMODE     : 1;         ///<BIT [22] DDR P2_WDATA BIST Mode Enable.
        uint32_t RF0115204_23_RSVD     : 1;         ///<BIT [23] Reserved.
        uint32_t P2_ROB_BIST_RST_N     : 1;         ///<BIT [24] DDR P2_ROB BIST Reset.
        uint32_t P2_ROB_RUNSTOP        : 1;         ///<BIT [25] DDR P2_ROB BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t P2_ROB_ROWCOLSWITCH   : 1;         ///<BIT [26] DDR P2_ROB BIST Row-Column Switch.
        uint32_t P2_ROB_BISTSTT_IN     : 1;         ///<BIT [27] DDR P2_ROB BIST Status Input.
        uint32_t P2_ROB_BISTSTT_CEN    : 1;         ///<BIT [28] DDR P2_ROB BIST Status Enable.
        uint32_t P2_ROB_BISTSTT_CLK    : 1;         ///<BIT [29] DDR P2_ROB BIST Status Clock.
        uint32_t P2_ROB_BISTMD         : 1;         ///<BIT [30] DDR P2_ROB BIST Mode Enable.
        uint32_t RF0115204_31_RSVD     : 1;         ///<BIT [31] Reserved.
    } b;    
} DdrMbistCtrl1_t;

///  @brief 0xF0115208 : DDR MBIST Status 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t P0_WDATA_BIST_STT_OUT : 1;         ///<BIT [0] DDR P0_WDATA BIST Status Output.
        uint32_t P0_WDATA_FIN          : 1;         ///<BIT [1] DDR P0_WDATA BIST Finish.
        uint32_t P0_WDATA_ERR          : 1;         ///<BIT [2] DDR P0_WDATA BIST Error.
        uint32_t P0_WDATA_BIST_FL      : 1;         ///<BIT [3] DDR P0_WDATA BIST Fail.
        uint32_t RF0115208_7_4_RSVD    : 4;         ///<BIT [7:4] Reserved.
        uint32_t P0_ROB_BIST_STT_OUT   : 1;         ///<BIT [8] DDR P01_ROB BIST Status Output.
        uint32_t P0_ROB_FIN            : 1;         ///<BIT [9] DDR P0_ROB BIST Finish.
        uint32_t P0_ROB_ERR            : 1;         ///<BIT [10] DDR P0_ROB BIST Error.
        uint32_t P0_ROB_BIST_FL        : 2;         ///<BIT [12:11] DDR P0_ROB BIST Fail.
        uint32_t RF0115208_15_13_RSVD  : 3;         ///<BIT [15:13] Reserved.
        uint32_t P1_WDATA_BIST_STT_OUT : 1;         ///<BIT [16] DDR P1_WDATA BIST Status Output.
        uint32_t P1_WDATA_FIN          : 1;         ///<BIT [17] DDR P1_WDATA BIST Finish.
        uint32_t P1_WDATA_ERR          : 1;         ///<BIT [18] DDR P1_WDATA BIST Error.
        uint32_t P1_WDATA_BIST_FL      : 1;         ///<BIT [19] DDR P1_WDATA BIST Fail.
        uint32_t RF0115208_23_20_RSVD  : 4;         ///<BIT [23:20] Reserved.
        uint32_t P1_ROB_BIST_STT_OUT   : 1;         ///<BIT [24] DDR P1_ROB BIST Status Output.
        uint32_t P1_ROB_FIN            : 1;         ///<BIT [25] DDR P1_ROB BIST Finish.
        uint32_t P1_ROB_ERR            : 1;         ///<BIT [26] DDR P1_ROB BIST Error.
        uint32_t P1_ROB_BIST_FL        : 2;         ///<BIT [28:27] DDR P1_ROB BIST Fail.
        uint32_t RF0115208_31_29_RSVD  : 3;         ///<BIT [31:29] Reserved.
    } b;    
} DdrMbistStat0_t;

///  @brief 0xF011520C : DDR MBIST Status 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t WDB_BIST_STT_OUT      : 1;         ///<BIT [0] DDR WDB BIST Status Output.
        uint32_t WDB_FIN               : 1;         ///<BIT [1] DDR WDB BIST Finish.
        uint32_t WDB_ERR               : 1;         ///<BIT [2] DDR WDB BIST Error.
        uint32_t WDB_BIST_FL           : 2;         ///<BIT [4:3] DDR WDB BIST Fail.
        uint32_t RF011520C_7_5_RSVD    : 3;         ///<BIT [7:5] Reserved.
        uint32_t RU_BIST_STT_OUT       : 1;         ///<BIT [8] DDR RU BIST Status Output.
        uint32_t RU_FIN                : 1;         ///<BIT [9] DDR RU BIST Finish.
        uint32_t RU_ERR                : 1;         ///<BIT [10] DDR RU BIST Error.
        uint32_t RU_BIST_FL            : 1;         ///<BIT [11] DDR RU BIST Fail.
        uint32_t RF011520C_15_12_RSVD  : 4;         ///<BIT [15:12] Reserved.
        uint32_t P2_WDATA_BIST_STT_OUT : 1;         ///<BIT [16] DDR P2_WDATA BIST Status Output.
        uint32_t P2_WDATA_FIN          : 1;         ///<BIT [17] DDR P2_WDATA BIST Finish.
        uint32_t P2_WDATA_ERR          : 1;         ///<BIT [18] DDR P2_WDATA BIST Error.
        uint32_t P2_WDATA_BIST_FL      : 1;         ///<BIT [19] DDR P2_WDATA BIST Fail.
        uint32_t RF011520C_23_20_RSVD  : 4;         ///<BIT [23:20] Reserved.
        uint32_t P2_ROB_BIST_STT_OUT   : 1;         ///<BIT [24] DDR P2_ROB BIST Status Output.
        uint32_t P2_ROB_FIN            : 1;         ///<BIT [25] DDR P2_ROB BIST Finish.
        uint32_t P2_ROB_ERR            : 1;         ///<BIT [26] DDR P2_ROB BIST Error.
        uint32_t P2_ROB_BIST_FL        : 2;         ///<BIT [28:27] DDR P2_ROB BIST Fail.
        uint32_t RF011520C_31_29_RSVD  : 3;         ///<BIT [31:29] Reserved.
    } b;    
} DdrMbistStat1_t;

///  @brief 0xF0115210 : Miscellaneous MBIST Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SPM_BIST_RST                : 1;         ///<BIT [0] SPM BIST Reset.
        uint32_t SPM_BIST_RUNSTOP            : 1;         ///<BIT [1] SPM BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t SPM_BIST_ROWCOL_SWITCH      : 1;         ///<BIT [2] SPM BIST Row Column Switch.
        uint32_t SPM_BIST_STT_IN             : 1;         ///<BIT [3] SPM BIST status Input.
        uint32_t SPM_BIST_STT_EN             : 1;         ///<BIT [4] SPM BIST status Enable.
        uint32_t SPM_BIST_STT_CLK            : 1;         ///<BIT [5] SPM BIST status Clock.
        uint32_t SPM_BIST_MODE               : 1;         ///<BIT [6] SPM BIST Mode Enable.
        uint32_t RF0115210_7_RSVD            : 1;         ///<BIT [7] Reserved.
        uint32_t SPI_BIST_ROWCOL_SWITCH      : 1;         ///<BIT [8] SPI BIST Row-Column Switch.
        uint32_t SPI_BIST_RUNSTOP            : 1;         ///<BIT [9] SPI BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t SPI_BIST_STT_IN             : 1;         ///<BIT [10] SPI BIST status Input.
        uint32_t SPI_BIST_STT_EN             : 1;         ///<BIT [11] SPI BIST status Enable.
        uint32_t SPI_BIST_MODE               : 1;         ///<BIT [12] SPI BIST Mode Enable.
        uint32_t RF0115210_15_13_RSVD        : 3;         ///<BIT [15:13] Reserved.
        uint32_t IDS_BIST_RST_N              : 1;         ///<BIT [16] IDS BIST reset.
        uint32_t IDS_BIST_RUNSTOP            : 1;         ///<BIT [17] IDS BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t IDS_BIST_ROWCOL_SWITCH      : 1;         ///<BIT [18] IDS BIST Row-Column Switch.
        uint32_t IDS_BIST_STT_IN             : 1;         ///<BIT [19] IDS BIST status Input.
        uint32_t IDS_BIST_STT_EN             : 1;         ///<BIT [20] IDS BIST status Enable.
        uint32_t IDS_BIST_STT_CLK            : 1;         ///<BIT [21] IDS BIST status Clock.
        uint32_t IDS_BIST_MD                 : 1;         ///<BIT [22] IDS BIST Mode Enable.
        uint32_t RF0115210_23_RSVD           : 1;         ///<BIT [23] Reserved.
        uint32_t ETB_BIST_RST_CSTCM_RAM      : 1;         ///<BIT [24] ETB BIST Reset.
        uint32_t ETB_RUNSTOP_CSTCM_RAM       : 1;         ///<BIT [25] ETB BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t ETB_ROWCOL_SWITCH_CSTCM_RAM : 1;         ///<BIT [26] ETB BIST Row-Column Switch.
        uint32_t ETB_BIST_STT_IN_CSTCMRAM    : 1;         ///<BIT [27] ETB BIST Status Input.
        uint32_t ETB_BIST_STT_EN_CSTCM_RAM   : 1;         ///<BIT [28] ETB BIST Status Enable.
        uint32_t ETB_BIST_STT_CLK_CSTCM_RAM  : 1;         ///<BIT [29] ETB BIST Status Clock.
        uint32_t ETB_BISTMODE_CSTCM_RAM      : 1;         ///<BIT [30] ETB BIST Mode Enable.
        uint32_t RF0115210_31_RSVD           : 1;         ///<BIT [31] Reserved.
    } b;    
} MiscMbistCtrl_t;

///  @brief 0xF0115214 : Miscellaneous MBIST Status 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SPM_BIST_STT_OUT            : 1;         ///<BIT [0] SPM BIST Status Output.
        uint32_t SPM_BIST_FIN                : 1;         ///<BIT [1] SPM BIST Finish.
        uint32_t SPM_BIST_ERR                : 1;         ///<BIT [2] SPM BIST Error.
        uint32_t SPM_BIST_ANY_FL             : 1;         ///<BIT [3] SPM BIST Any Fail.
        uint32_t SPM_BIST_FL                 : 4;         ///<BIT [7:4] SPM BIST Fail.
        uint32_t RF0115214_15_8_RSVD         : 8;         ///<BIT [15:8] Reserved.
        uint32_t IDS_BIST_STT_OUT            : 1;         ///<BIT [16] IDS BIST Status Output.
        uint32_t IDS_BIST_FIN                : 1;         ///<BIT [17] IDS BIST Finish.
        uint32_t IDS_BIST_ERR                : 1;         ///<BIT [18] IDS BIST Error.
        uint32_t IDS_BIST_FL                 : 2;         ///<BIT [20:19] IDS BIST Fail.
        uint32_t RF0115214_23_21_RSVD        : 3;         ///<BIT [23:21] Reserved.
        uint32_t ETB_BISTSTATUS_OUT_CSTCMRAM : 1;         ///<BIT [24] ETB BIST Status Output.
        uint32_t ETB_FIN_CSTCM_RAM           : 1;         ///<BIT [25] ETB BIST Finish.
        uint32_t ETB_ERR_CSTCM_RAM           : 1;         ///<BIT [26] ETB BIST Error.
        uint32_t ETB_BIST_FL_CSTCM_RAM       : 1;         ///<BIT [27] ETB BIST Fail.
        uint32_t RF0115214_31_28_RSVD        : 4;         ///<BIT [31:28] Reserved.
    } b;    
} MiscMbistStat0_t;

///  @brief 0xF0115218 : Miscellaneous MBIST Status 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SPI_BIST_FIN        : 1;         ///<BIT [0] SPI BIST Finish.
        uint32_t SPI_BIST_FL         : 1;         ///<BIT [1] SPI BIST Fail.
        uint32_t RF0115218_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} MiscMbistStat1_t;

///  @brief 0xF011521C : AHCI MBIST Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AHCI_CMDFIFO_BISTRUNSTOP        : 1;         ///<BIT [0] AHCI Command FIFO BIST Run/Stop Control for MBIST Diagnostic Mo.
        uint32_t AHCI_CMDFIFO_BIST_ROWCOL_SWITCH : 1;         ///<BIT [1] AHCI Command FIFO BIST Row-Column Switch.
        uint32_t AHCI_CMDFIFO_BIST_STT_IN        : 1;         ///<BIT [2] AHCI Command FIFO BIST status Input.
        uint32_t AHCI_CMDFIFO_BIST_STT_EN        : 1;         ///<BIT [3] AHCI Command FIFO BIST status Enable.
        uint32_t AHCI_CMDFIFO_BIST_STT_CLK       : 1;         ///<BIT [4] AHC Command FIFO BIST status Clock.
        uint32_t AHCI_CMDFIFO_BIST_MD            : 1;         ///<BIT [5] AHCI Command FIFO BIST Mode Enable.
        uint32_t RF011521C_7_6_RSVD              : 2;         ///<BIT [7:6] Reserved.
        uint32_t AHCI_BISTRUNSTOP                : 1;         ///<BIT [8] AHCI BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t AHCI_BIST_ROWCOL_SWITCH         : 1;         ///<BIT [9] AHCI BIST Row-Column Switch.
        uint32_t AHCI_BIST_STT_IN                : 1;         ///<BIT [10] AHCI BIST status Input.
        uint32_t AHCI_BIST_STT_EN                : 1;         ///<BIT [11] AHCI BIST status Enable.
        uint32_t AHCI_BIST_STT_CLK               : 1;         ///<BIT [12] AHCI BIST status Clock.
        uint32_t AHCI_BIST_MD                    : 1;         ///<BIT [13] AHCI BIST Mode Enable.
        uint32_t RF011521C_31_14_RSVD            :18;         ///<BIT [31:14] Reserved.
    } b;    
} AhciMbistCtrl_t;

///  @brief 0xF0115220 : AHCI MBIST Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AHCI_CMDIFO_BIST_STT_OUT : 1;         ///<BIT [0] AHCI Command FIFO BIST Status Output.
        uint32_t AHCI_CMDIFO_BIST_FIN     : 1;         ///<BIT [1] AHCI Command FIFO BIST Finish.
        uint32_t AHCI_CMDIFO_BIST_ERR     : 1;         ///<BIT [2] AHCI Command FIFO BIST Error.
        uint32_t AHCI_CMDIFO_BIST_FL      : 1;         ///<BIT [3] AHCI Command FIFO BIST Fail.
        uint32_t RF0115220_7_4_RSVD       : 4;         ///<BIT [7:4] Reserved.
        uint32_t AHCI_BIST_STT_OUT        : 1;         ///<BIT [8] AHCI BIST Status Output.
        uint32_t AHCI_BIST_FIN            : 1;         ///<BIT [9] AHCI BIST Finish.
        uint32_t AHCI_BIST_ERR            : 1;         ///<BIT [10] AHCI BIST Error.
        uint32_t AHCI_BIST_FL             : 1;         ///<BIT [11] AHCI BIST Fail.
        uint32_t RF0115220_31_12_RSVD     :20;         ///<BIT [31:12] Reserved.
    } b;    
} AhciMbistStat_t;

///  @brief 0xF0115224 : BCM MBIST Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BCM_1P_0_RST_B              : 1;         ///<BIT [0] BCM SR1P BIST Reset.
        uint32_t BCM_1P_0_BIST_RUNSTOP       : 1;         ///<BIT [1] BCM SR1P BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t BCM_1P_0_BIST_ROWCOL_SWITCH : 1;         ///<BIT [2] BCM SR1P BIST Row-Column Switch.
        uint32_t BCM_1P_0_BIST_STT_IN        : 1;         ///<BIT [3] BCM SR1P BIST Status Input.
        uint32_t BCM_1P_0_BIST_STT_EN        : 1;         ///<BIT [4] BCM SR1P BIST Status Enable.
        uint32_t BCM_1P_0_BIST_STT_CLK       : 1;         ///<BIT [5] BCM SR1P BIST Status Clock.
        uint32_t BCM_1P_0_BIST_MD            : 1;         ///<BIT [6] BCM SR1P BIST Mode Enable.
        uint32_t RF0115224_7_RSVD            : 1;         ///<BIT [7] Reserved.
        uint32_t BCM_2P_0_RST_B              : 1;         ///<BIT [8] BCM SR2P BIST Reset.
        uint32_t BCM_2P_0_BIST_RUNSTOP       : 1;         ///<BIT [9] BCM SR2P BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t BCM_2P_0_BIST_ROWCOL_SWITCH : 1;         ///<BIT [10] BCM SR2P BIST Row-Column Switch.
        uint32_t BCM_2P_0_BIST_STT_IN        : 1;         ///<BIT [11] BCM SR2P BIST status Input.
        uint32_t BCM_2P_0_BIST_STT_EN        : 1;         ///<BIT [12] BCM SR2P BIST status Enable.
        uint32_t BCM_2P_0_BIST_STT_CLK       : 1;         ///<BIT [13] BCM SR2P BIST status Clock.
        uint32_t BCM_2P_0_BIST_MD            : 1;         ///<BIT [14] BCM SR2P BIST Mode Enable.
        uint32_t RF0115224_15_RSVD           : 1;         ///<BIT [15] Reserved.
        uint32_t BCM_ROM_BIST_MD             : 1;         ///<BIT [16] BCM ROM BIST Mode Enable.
        uint32_t BCM_ROM_RST_B               : 1;         ///<BIT [17] BCM ROM Reset.
        uint32_t RF0115224_19_18_RSVD        : 2;         ///<BIT [19:18] Reserved.
        uint32_t BCM_XTC_SEL                 : 1;         ///<BIT [20] BCM XTC Select.
        uint32_t BCM_WTC                     : 2;         ///<BIT [22:21] BCM Write Timing Control.
        uint32_t BCM_RTC                     : 2;         ///<BIT [24:23] BCM Read Timing Control.
        uint32_t RF0115224_25_RSVD           : 1;         ///<BIT [25] Reserved.
        uint32_t BCM_ROM_RTC_REF             : 2;         ///<BIT [27:26] BCM ROM Read Timing Control Reference.
        uint32_t BCM_ROM_RTC                 : 3;         ///<BIT [30:28] BCM ROM Read Timing Control.
        uint32_t RF0115224_31_RSVD           : 1;         ///<BIT [31] Reserved.
    } b;    
} BcmMbistCtrl_t;

///  @brief 0xF0115228 : BCM MBIST Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BCM_1P_0_BIST_STT_OUT : 1;         ///<BIT [0] BCM SR1P BIST Status Output.
        uint32_t BCM_1P_0_FIN          : 1;         ///<BIT [1] BCM SR1P BIST Finish.
        uint32_t BCM_1P_0_ERR          : 1;         ///<BIT [2] BCM SR1P BIST Error.
        uint32_t BCM_1P_0_ANY_BIST_FL  : 1;         ///<BIT [3] BCM SR2P Any BIST Fail.
        uint32_t BCM_1P_0_BIST_FL      : 2;         ///<BIT [5:4] BCM SR2P BIST Fail.
        uint32_t RF0115228_7_6_RSVD    : 2;         ///<BIT [7:6] Reserved.
        uint32_t BCM_2P_0_BIST_STT_OUT : 1;         ///<BIT [8] BCM SR2P BIST Status Output.
        uint32_t BCM_2P_0_BIST_FIN     : 1;         ///<BIT [9] BCM SR2P BIST Finish.
        uint32_t BCM_2P_0_BIST_ERR     : 1;         ///<BIT [10] BCM SR2P BIST Error.
        uint32_t BCM_2P_0_BIST_FL      : 1;         ///<BIT [11] BCM SR2P BIST Fail.
        uint32_t RF0115228_15_12_RSVD  : 4;         ///<BIT [15:12] Reserved.
        uint32_t BCM_ROM_BIST_FIN      : 1;         ///<BIT [16] BCM ROM BIST Finish.
        uint32_t BCM_ROM_BIST_FL       : 1;         ///<BIT [17] BCM ROM BIST Fail.
        uint32_t RF0115228_31_18_RSVD  :14;         ///<BIT [31:18] Reserved.
    } b;    
} BcmMbistStat_t;

///  @brief 0xF011522C : CPU MBIST Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPU0_BIST_RST_N    : 1;         ///<BIT [0] CPU0 BIST Reset.
        uint32_t CPU0_RUNSTOP       : 1;         ///<BIT [1] CPU0 BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t CPU0_ROWCOL_SWITCH : 1;         ///<BIT [2] CPU0 BIST Row-Column Switch.
        uint32_t CPU0_BIST_STT_IN   : 1;         ///<BIT [3] CPU0 BIST Status Input.
        uint32_t CPU0_BIST_STT_EN   : 1;         ///<BIT [4] CPU0 BIST Status Enable.
        uint32_t CPU0_BIST_STT_CLK  : 1;         ///<BIT [5] CPU0 BIST Status Clock.
        uint32_t CPU0_BIST_SEL_EN   : 1;         ///<BIT [6] CPU0 Memory Group Selection Enable CPU1.
        uint32_t CPU0_MEM_GRP_SEL_O : 1;         ///<BIT [7] CPU0 Memory Group Select Output.
        uint32_t CPU0_MEM_GRP_SEL_I : 1;         ///<BIT [8] CPU0 Memory Group Select Input.
        uint32_t CPU0_BIST_MD       : 1;         ///<BIT [9] CPU0 BIST Mode Enable.
        uint32_t RF011522C_10_RSVD  : 1;         ///<BIT [10] Reserved.
        uint32_t CPU1_BIST_RST_N    : 1;         ///<BIT [11] CPU1 BIST Reset.
        uint32_t CPU1_RUNSTOP       : 1;         ///<BIT [12] CPU1 BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t CPU1_ROWCOL_SWITCH : 1;         ///<BIT [13] CPU1 BIST Row-Column Switch.
        uint32_t CPU1_BIST_STT_IN   : 1;         ///<BIT [14] CPU1 BIST Status Input.
        uint32_t CPU1_BIST_STT_EN   : 1;         ///<BIT [15] CPU1 BIST Status Enable.
        uint32_t CPU1_BIST_STT_CLK  : 1;         ///<BIT [16] CPU1 BIST Status Clock.
        uint32_t CPU1_BIST_SEL_EN   : 1;         ///<BIT [17] CPU1 Memory Group Selection Enable CPU1.
        uint32_t CPU1_MEM_GRP_SEL_O : 1;         ///<BIT [18] CPU1 Memory Group Select Output.
        uint32_t CPU1_MEM_GRP_SEL_I : 1;         ///<BIT [19] CPU1 Memory Group Select Input.
        uint32_t CPU1_BIST_MD       : 1;         ///<BIT [20] CPU1 BIST Mode Enable.
        uint32_t RF011522C_21_RSVD  : 1;         ///<BIT [21] Reserved.
        uint32_t CPU2_BIST_RST_N    : 1;         ///<BIT [22] CPU2 BIST Reset.
        uint32_t CPU2_RUNSTOP       : 1;         ///<BIT [23] CPU2 BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t CPU2_ROWCOL_SWITCH : 1;         ///<BIT [24] CPU2 BIST Row-Column Switch.
        uint32_t CPU2_BIST_STT_IN   : 1;         ///<BIT [25] CPU2 BIST status Input.
        uint32_t CPU2_BIST_STT_EN   : 1;         ///<BIT [26] CPU2 BIST Status Enable.
        uint32_t CPU2_BIST_STT_CLK  : 1;         ///<BIT [27] CPU2 BIST Status Clock.
        uint32_t CPU2_BIST_SEL_EN   : 1;         ///<BIT [28] CPU2 Memory Group Selection Enable.
        uint32_t CPU2_MEM_GRP_SEL_O : 1;         ///<BIT [29] CPU2 Memory Group Select Output.
        uint32_t CPU2_MEM_GRP_SEL_I : 1;         ///<BIT [30] CPU2 Memory Group Select Input.
        uint32_t CPU2_BIST_MD       : 1;         ///<BIT [31] CPU2 BIST Mode Enable.
    } b;    
} CpuMbistCtrl_t;

///  @brief 0xF0115230 : CPU MBIST Status 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPU0_BIST_STT_OUT : 1;         ///<BIT [0] CPU0 BIST Status Output.
        uint32_t CPU0_BIST_FIN     : 1;         ///<BIT [1] CPU0 BIST Finish.
        uint32_t CPU0_BIST_ERR     : 1;         ///<BIT [2] CPU0 BIST Error.
        uint32_t CPU0_BIST_FL_0    : 1;         ///<BIT [3] CPU0 Ddata0 BIST Fail.
        uint32_t CPU0_BIST_FL_1    : 1;         ///<BIT [4] CPU0 Ddata1 BIST Fail.
        uint32_t CPU0_BIST_FL_2    : 1;         ///<BIT [5] CPU0 Ddata2 BIST Fail.
        uint32_t CPU0_BIST_FL_3    : 1;         ///<BIT [6] CPU0 Ddata3 BIST Fail.
        uint32_t CPU0_BIST_FL_4    : 1;         ///<BIT [7] CPU0 Ddata4 BIST Fail.
        uint32_t CPU0_BIST_FL_5    : 1;         ///<BIT [8] CPU0 Ddata5 BIST Fail.
        uint32_t CPU0_BIST_FL_6    : 1;         ///<BIT [9] CPU0 Ddata6 BIST Fail.
        uint32_t CPU0_BIST_FL_7    : 1;         ///<BIT [10] CPU0 Ddata7 BIST Fail.
        uint32_t CPU0_BIST_FL_8    : 1;         ///<BIT [11] CPU0 Dtag0 BIST Fail.
        uint32_t CPU0_BIST_FL_9    : 1;         ///<BIT [12] CPU0 Dtag1 BIST Fail.
        uint32_t CPU0_BIST_FL_10   : 1;         ///<BIT [13] CPU0 Dtag2 BIST Fail.
        uint32_t CPU0_BIST_FL_11   : 1;         ///<BIT [14] CPU0 Dtag3 BIST Fail.
        uint32_t CPU0_BIST_FL_12   : 1;         ///<BIT [15] CPU0 Ddirty BIST Fail.
        uint32_t CPU0_BIST_FL_13   : 1;         ///<BIT [16] CPU0 Idata0 BIST Fail.
        uint32_t CPU0_BIST_FL_14   : 1;         ///<BIT [17] CPU0 Idata1 BIST Fail.
        uint32_t CPU0_BIST_FL_15   : 1;         ///<BIT [18] CPU0 Idata2 BIST Fail.
        uint32_t CPU0_BIST_FL_16   : 1;         ///<BIT [19] CPU0 Idata3 BIST Fail.
        uint32_t CPU0_BIST_FL_17   : 1;         ///<BIT [20] CPU0 Itag0 BIST Fail.
        uint32_t CPU0_BIST_FL_18   : 1;         ///<BIT [21] CPU0 Itag1 BIST Fail.
        uint32_t CPU0_BIST_FL_19   : 1;         ///<BIT [22] CPU0 Itag2 BIST Fail.
        uint32_t CPU0_BIST_FL_20   : 1;         ///<BIT [23] CPU0 Itag3 BIST Fail.
        uint32_t CPU0_BIST_FL_21   : 1;         ///<BIT [24] CPU0 BTCM0 BIST Fail.
        uint32_t CPU0_BIST_FL_22   : 1;         ///<BIT [25] CPU0 BTCM1 BIST Fail.
        uint32_t CPU0_BIST_FL_23   : 1;         ///<BIT [26] CPU0 BTCM2 BIST Fail.
        uint32_t CPU0_BIST_FL_24   : 1;         ///<BIT [27] CPU0 BTCM3 BIST Fail.
        uint32_t CPU0_BIST_FL_25   : 1;         ///<BIT [28] CPU0 ATCM0 BIST Fail.
        uint32_t CPU0_BIST_FL_26   : 1;         ///<BIT [29] CPU0 ATCM1 BIST Fail.
        uint32_t CPU0_BIST_FL_27   : 1;         ///<BIT [30] CPU0 ATCM2 BIST Fail.
        uint32_t CPU0_BIST_FL_28   : 1;         ///<BIT [31] CPU0 ATCM3 BIST Fail.
    } b;    
} CpuMbistStat0_t;

///  @brief 0xF0115234 : CPU MBIST Status 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPU1_BIST_STT_OUT : 1;         ///<BIT [0] CPU1 BIST Status Output.
        uint32_t CPU1_BIST_FIN     : 1;         ///<BIT [1] CPU1 BIST Finish.
        uint32_t CPU1_BIST_ERR     : 1;         ///<BIT [2] CPU1 BIST Error.
        uint32_t CPU1_BIST_FL_0    : 1;         ///<BIT [3] CPU1 Ddata0 BIST Fail.
        uint32_t CPU1_BIST_FL_1    : 1;         ///<BIT [4] CPU1 Ddata1 BIST Fail.
        uint32_t CPU1_BIST_FL_2    : 1;         ///<BIT [5] CPU1 Ddata2 BIST Fail.
        uint32_t CPU1_BIST_FL_3    : 1;         ///<BIT [6] CPU1 Ddata3 BIST Fail.
        uint32_t CPU1_BIST_FL_4    : 1;         ///<BIT [7] CPU1 Ddata4 BIST Fail.
        uint32_t CPU1_BIST_FL_5    : 1;         ///<BIT [8] CPU1 Ddata5 BIST Fail.
        uint32_t CPU1_BIST_FL_6    : 1;         ///<BIT [9] CPU1 Ddata6 BIST Fail.
        uint32_t CPU1_BIST_FL_7    : 1;         ///<BIT [10] CPU1 Ddata7 BIST Fail.
        uint32_t CPU1_BIST_FL_8    : 1;         ///<BIT [11] CPU1 Dtag0 BIST Fail.
        uint32_t CPU1_BIST_FL_9    : 1;         ///<BIT [12] CPU1 Dtag1 BIST Fail.
        uint32_t CPU1_BIST_FL_10   : 1;         ///<BIT [13] CPU1 Dtag2 BIST Fail.
        uint32_t CPU1_BIST_FL_11   : 1;         ///<BIT [14] CPU1 Dtag3 BIST Fail.
        uint32_t CPU1_BIST_FL_12   : 1;         ///<BIT [15] CPU1 Ddirty BIST Fail.
        uint32_t CPU1_BIST_FL_13   : 1;         ///<BIT [16] CPU1 Idata0 BIST Fail.
        uint32_t CPU1_BIST_FL_14   : 1;         ///<BIT [17] CPU1 Idata1 BIST Fail.
        uint32_t CPU1_BIST_FL_15   : 1;         ///<BIT [18] CPU1 Idata2 BIST Fail.
        uint32_t CPU1_BIST_FL_16   : 1;         ///<BIT [19] CPU1 Idata3 BIST Fail.
        uint32_t CPU1_BIST_FL_17   : 1;         ///<BIT [20] CPU1 Itag0 BIST Fail.
        uint32_t CPU1_BIST_FL_18   : 1;         ///<BIT [21] CPU1 Itag1 BIST Fail.
        uint32_t CPU1_BIST_FL_19   : 1;         ///<BIT [22] CPU1 Itag2 BIST Fail.
        uint32_t CPU1_BIST_FL_20   : 1;         ///<BIT [23] CPU1 Itag3 BIST Fail.
        uint32_t CPU1_BIST_FL_21   : 1;         ///<BIT [24] CPU1 B0TCM0 BIST Fail.
        uint32_t CPU1_BIST_FL_22   : 1;         ///<BIT [25] CPU1 B0TCM1 BIST Fail.
        uint32_t CPU1_BIST_FL_23   : 1;         ///<BIT [26] CPU1 ATCM0 BIST Fail.
        uint32_t CPU1_BIST_FL_24   : 1;         ///<BIT [27] CPU1 ATCM1 BIST Fail.
        uint32_t CPU1_BIST_FL_25   : 1;         ///<BIT [28] CPU1 ATCM2 BIST Fail.
        uint32_t CPU1_BIST_FL_26   : 1;         ///<BIT [29] CPU1 ATCM3 BIST Fail.
        uint32_t CPU1_BIST_FL_27   : 1;         ///<BIT [30] Unused [30].
        uint32_t CPU1_BIST_FL_28   : 1;         ///<BIT [31] Unused [31].
    } b;    
} CpuMbistStat1_t;

///  @brief 0xF0115238 : CPU MBIST Status 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPU2_BIST_STT_OUT : 1;         ///<BIT [0] CPU2 BIST Status Output.
        uint32_t CPU2_BIST_FIN     : 1;         ///<BIT [1] CPU2 BIST Finish.
        uint32_t CPU2_BIST_ERR     : 1;         ///<BIT [2] CPU2 BIST Error.
        uint32_t CPU2_BIST_FL_0    : 1;         ///<BIT [3] CPU2 Ddata0 BIST Fail.
        uint32_t CPU2_BIST_FL_1    : 1;         ///<BIT [4] CPU2 Ddata1 BIST Fail.
        uint32_t CPU2_BIST_FL_2    : 1;         ///<BIT [5] CPU2 Ddata2 BIST Fail.
        uint32_t CPU2_BIST_FL_3    : 1;         ///<BIT [6] CPU2 Ddata3 BIST Fail.
        uint32_t CPU2_BIST_FL_4    : 1;         ///<BIT [7] CPU2 Ddata4 BIST Fail.
        uint32_t CPU2_BIST_FL_5    : 1;         ///<BIT [8] CPU2 Ddata5 BIST Fail.
        uint32_t CPU2_BIST_FL_6    : 1;         ///<BIT [9] CPU2 Ddata6 BIST Fail.
        uint32_t CPU2_BIST_FL_7    : 1;         ///<BIT [10] CPU2 Ddata7 BIST Fail.
        uint32_t CPU2_BIST_FL_8    : 1;         ///<BIT [11] CPU2 Dtag0 BIST Fail.
        uint32_t CPU2_BIST_FL_9    : 1;         ///<BIT [12] CPU2 Dtag1 BIST Fail.
        uint32_t CPU2_BIST_FL_10   : 1;         ///<BIT [13] CPU2 Dtag2 BIST Fail.
        uint32_t CPU2_BIST_FL_11   : 1;         ///<BIT [14] CPU2 Dtag3 BIST Fail.
        uint32_t CPU2_BIST_FL_12   : 1;         ///<BIT [15] CPU2 Ddirty BIST Fail.
        uint32_t CPU2_BIST_FL_13   : 1;         ///<BIT [16] CPU2 Idata0 BIST Fail.
        uint32_t CPU2_BIST_FL_14   : 1;         ///<BIT [17] CPU2 Idata1 BIST Fail.
        uint32_t CPU2_BIST_FL_15   : 1;         ///<BIT [18] CPU2 Idata2 BIST Fail.
        uint32_t CPU2_BIST_FL_16   : 1;         ///<BIT [19] CPU2 Idata3 BIST Fail.
        uint32_t CPU2_BIST_FL_17   : 1;         ///<BIT [20] CPU2 Itag0 BIST Fail.
        uint32_t CPU2_BIST_FL_18   : 1;         ///<BIT [21] CPU2 Itag1 BIST Fail.
        uint32_t CPU2_BIST_FL_19   : 1;         ///<BIT [22] CPU2 Itag2 BIST Fail.
        uint32_t CPU2_BIST_FL_20   : 1;         ///<BIT [23] CPU2 Itag3 BIST Fail.
        uint32_t CPU2_BIST_FL_21   : 1;         ///<BIT [24] CPU2 B0TCM0 BIST Fail.
        uint32_t CPU2_BIST_FL_22   : 1;         ///<BIT [25] CPU2 B0TCM1 BIST Fail.
        uint32_t CPU2_BIST_FL_23   : 1;         ///<BIT [26] CPU2 ATCM0 BIST Fail.
        uint32_t CPU2_BIST_FL_24   : 1;         ///<BIT [27] CPU2 ATCM1 BIST Fail.
        uint32_t CPU2_BIST_FL_25   : 1;         ///<BIT [28] CPU2 ATCM2 BIST Fail.
        uint32_t CPU2_BIST_FL_26   : 1;         ///<BIT [29] CPU2 ATCM3 BIST Fail.
        uint32_t CPU2_BIST_FL_27   : 1;         ///<BIT [30] CPU2 Unused [30].
        uint32_t CPU2_BIST_FL_28   : 1;         ///<BIT [31] CPU2 Unused [31].
    } b;    
} CpuMbistStat2_t;

///  @brief 0xF0115240 : PCIe MBIST Core Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PCIE_SR2P_ASYN_BIST_RSTN           : 1;         ///<BIT [0] PCIe SR2P BIST Reset.
        uint32_t PCIE_SR2P_ASYN_BISTRUNSTOP         : 1;         ///<BIT [1] PCIe SR2P BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t PCIE_SR2P_ASYN_BIST_ROWCOL_SWITCH  : 1;         ///<BIT [2] PCIe SR2P BIST Row-Column Switch.
        uint32_t PCIE_SR2P_ASYN_BIST_STT_IN         : 1;         ///<BIT [3] PCIe SR2P BIST Status Input.
        uint32_t PCIE_SR2P_ASYN_BIST_STT_EN         : 1;         ///<BIT [4] PCIe SR2P BIST Status Enable.
        uint32_t PCIE_SR2P_ASYN_BIST_STT_CLK        : 1;         ///<BIT [5] PCIe SR2P BIST Status Clock.
        uint32_t PCIE_SR2P_ASYN_BIST_MD             : 1;         ///<BIT [6] PCIe SR2P BIST Mode Enable.
        uint32_t RF0115240_7_RSVD                   : 1;         ///<BIT [7] Reserved.
        uint32_t PCIE_RADMX_ASYN_BIST_RSTN          : 1;         ///<BIT [8] PCIe RADMX_ASYN BIST Reset.
        uint32_t PCIE_RADMX_ASYN_BISTRUNSTOP        : 1;         ///<BIT [9] PCIe RADMX_ASYN BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t PCIE_RADMX_ASYN_BIST_ROWCOL_SWITCH : 1;         ///<BIT [10] PCIe RADMX_ASYN BIST Row-Column Switch.
        uint32_t PCIE_RADMX_ASYN_BIST_STT_IN        : 1;         ///<BIT [11] PCIe RADMX_ASYN BIST Status Input.
        uint32_t PCIE_RADMX_ASYN_BIST_STT_EN        : 1;         ///<BIT [12] PCIe RADMX_ASYN BIST Status Enable.
        uint32_t PCIE_RADMX_ASYN_BIST_STT_CLK       : 1;         ///<BIT [13] PCIe RADMX_ASYN BIST Status Clock.
        uint32_t PCIE_RADMX_ASYN_BIST_MD            : 1;         ///<BIT [14] PCIe RADMX_ASYN BIST Mode enable.
        uint32_t RF0115240_15_RSVD                  : 1;         ///<BIT [15] Reserved.
        uint32_t PCIE_MCB_SB_BIST_RSTN              : 1;         ///<BIT [16] PCIe MCB_SB BIST Reset.
        uint32_t PCIE_MCB_SB_BISTRUNSTOP            : 1;         ///<BIT [17] PCIe MCB_SB BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t PCIE_MCB_SB_BIST_ROWCOL_SWITCH     : 1;         ///<BIT [18] PCIe MCB_SB BIST Row-Column Switch.
        uint32_t PCIE_MCB_SB_BIST_STT_IN            : 1;         ///<BIT [19] PCIe MCB_SB BIST Status Input.
        uint32_t PCIE_MCB_SB_BIST_STT_EN            : 1;         ///<BIT [20] PCIe MCB_SB BIST Status Enable.
        uint32_t PCIE_MCB_SB_BIST_STT_CLK           : 1;         ///<BIT [21] PCIe MCB_SB BIST Status Clock.
        uint32_t PCIE_MCB_SB_BIST_MD                : 1;         ///<BIT [22] PCIe MCB_SB BIST Mode Enable.
        uint32_t RF0115240_23_RSVD                  : 1;         ///<BIT [23] Reserved.
        uint32_t PCIE_XDLH_BIST_RSTN                : 1;         ///<BIT [24] PCIe XDLH BIST Reset.
        uint32_t PCIE_XDLH_BISTRUNSTOP              : 1;         ///<BIT [25] PCIe XDLH BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t PCIE_XDLH_BIST_ROWCOL_SWITCH       : 1;         ///<BIT [26] PCIe XDLH BIST Row-Column Switch.
        uint32_t PCIE_XDLH_BIST_STT_IN              : 1;         ///<BIT [27] PCIe XDLH BIST Status Input.
        uint32_t PCIE_XDLH_BIST_STT_EN              : 1;         ///<BIT [28] PCIe XDLH BIST Status Enable.
        uint32_t PCIE_XDLH_BIST_STT_CLK             : 1;         ///<BIT [29] PCIe XDLH BIST Status Clock.
        uint32_t PCIE_XDLH_BIST_MD                  : 1;         ///<BIT [30] PCIe XDLH BIST Mode Enable.
        uint32_t RF0115240_31_RSVD                  : 1;         ///<BIT [31] Reserved.
    } b;    
} PcieMbistCoreCtrl_t;

///  @brief 0xF0115244 : PCIe MBIST Master Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PCIE_RADMX_BIST_RSTN      : 1;         ///<BIT [0] PCIe RADMX BIST Reset.
        uint32_t PCIE_RADMX_RUNSTOP        : 1;         ///<BIT [1] PCIe RADMX BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t PCIE_RADMX_ROWCOLSWITCH   : 1;         ///<BIT [2] PCIe RADMX BIST Row-Column Switch.
        uint32_t PCIE_RADMX_BIST_STT_IN    : 1;         ///<BIT [3] PCIe RADMX BIST Status Input.
        uint32_t PCIE_RADMX_BIST_STT_EN    : 1;         ///<BIT [4] PCIe RADMX BIST Status Enable.
        uint32_t PCIE_RADMX_BIST_STT_CLK   : 1;         ///<BIT [5] PCIe RADMX BIST Status Clock.
        uint32_t PCIE_RADMX_BISTMD         : 1;         ///<BIT [6] PCIe RADMX BIST Mode Enable.
        uint32_t RF0115244_7_RSVD          : 1;         ///<BIT [7] Reserved.
        uint32_t PCIE_MCB_A2C_BIST_RSTN    : 1;         ///<BIT [8] PCIe MCB_A2C BIST Reset.
        uint32_t PCIE_MCB_A2C_RUNSTOP      : 1;         ///<BIT [9] PCIe MCB_A2C BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t PCIE_MCB_A2C_ROWCOLSWITCH : 1;         ///<BIT [10] PCIe MCB_A2C BIST Row-Column Switch.
        uint32_t PCIE_MCB_A2C_BIST_STT_IN  : 1;         ///<BIT [11] PCIe MCB_A2C BIST Status Input.
        uint32_t PCIE_MCB_A2C_BIST_STT_EN  : 1;         ///<BIT [12] PCIe MCB_A2C BIST Status Enable.
        uint32_t PCIE_MCB_A2C_BIST_STT_CLK : 1;         ///<BIT [13] PCIe MCB_A2C BIST Status Clock.
        uint32_t PCIE_MCB_A2C_BISTMD       : 1;         ///<BIT [14] PCIe MCB_A2C BIST Mode Enable.
        uint32_t RF0115244_15_RSVD         : 1;         ///<BIT [15] Reserved.
        uint32_t PCIE_MBIST_CLK_SEL        : 1;         ///<BIT [16] PCIE BIST Clock Enable.
        uint32_t RF0115244_31_17_RSVD      :15;         ///<BIT [31:17] Reserved.
    } b;    
} PcieMbistMstrCtrl_t;

///  @brief 0xF0115248 : PCIe MBIST Slave Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PCIE_XADMX1_HDR_BIST_RSTN     : 1;         ///<BIT [0] PCIe XADMX1_HDR BIST Reset.
        uint32_t PCIE_XADMX1_HDR_RUNSTOP       : 1;         ///<BIT [1] PCIe XADMX1_HDR BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t PCIE_XADMX1_HDR_ROWCOLSWITCH  : 1;         ///<BIT [2] PCIe XADMX1_HDR BIST Row-Column Switch.
        uint32_t PCIE_XADMX1_HDR_BIST_STT_IN   : 1;         ///<BIT [3] PCIe XADMX1_HDR BIST Status Input.
        uint32_t PCIE_XADMX1_HDR_BIST_STT_EN   : 1;         ///<BIT [4] PCIe XADMX1_HDR BIST Status Enable.
        uint32_t PCIE_XADMX1_HDR_BIST_STT_CLK  : 1;         ///<BIT [5] PCIe XADMX1_HDR BIST Status Clock.
        uint32_t PCIE_XADMX1_HDR_BISTMD        : 1;         ///<BIT [6] PCIe XADMX1_HDR BIST Mode Enable.
        uint32_t RF0115248_7_RSVD              : 1;         ///<BIT [7] Reserved.
        uint32_t PCIE_XADMX1_DATA_BIST_RSTN    : 1;         ///<BIT [8] PCIe XADMX1_DATA BIST Reset.
        uint32_t PCIE_XADMX1_DATA_RUNSTOP      : 1;         ///<BIT [9] PCIe XADMX1_DATA BIST Run/Stop Control for MBIST Diagnostic Mod.
        uint32_t PCIE_XADMX1_DATA_ROWCOLSWITCH : 1;         ///<BIT [10] PCIe XADMX1_DATA BIST Row-Column Switch.
        uint32_t PCIE_XADMX1_DATA_BIST_STT_IN  : 1;         ///<BIT [11] PCIe XADMX1_DATA BIST Status Input.
        uint32_t PCIE_XADMX1_DATA_BIST_STT_EN  : 1;         ///<BIT [12] PCIe XADMX1_DATA BIST Status Enable.
        uint32_t PCIE_XADMX1_DATA_BIST_STT_CLK : 1;         ///<BIT [13] PCIe XADMX1_DATA BIST Status Clock.
        uint32_t PCIE_XADMX1_DATA_BISTMD       : 1;         ///<BIT [14] PCIe XADMX1_DATA BIST Mode Enable.
        uint32_t RF0115248_15_RSVD             : 1;         ///<BIT [15] Reserved.
        uint32_t PCIE_SLV_MGR_BIST_RSTN        : 1;         ///<BIT [16] PCIe SLV_MGR_ BIST Reset.
        uint32_t PCIE_SLV_MGR_RUNSTOP          : 1;         ///<BIT [17] PCIe SLV_MGR_ BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t PCIE_SLV_MGR_ROWCOLSWITCH     : 1;         ///<BIT [18] PCIe SLV_MGR_ BIST Row-Column Switch.
        uint32_t PCIE_SLV_MGR_BIST_STT_IN      : 1;         ///<BIT [19] PCIe SLV_MGR_ BIST Status Input.
        uint32_t PCIE_SLV_MGR_BIST_STT_EN      : 1;         ///<BIT [20] PCIe SLV_MGR_ BIST Status Enable.
        uint32_t PCIE_SLV_MGR_BIST_STT_CLK     : 1;         ///<BIT [21] PCIe SLV_MGR_ BIST Status Clock.
        uint32_t PCIE_SLV_MGR_BISTMD           : 1;         ///<BIT [22] PCIe SLV_MGR_ BIST Mode enable.
        uint32_t RF0115248_23_RSVD             : 1;         ///<BIT [23] Reserved.
        uint32_t PCIE_SLV_AFIFO_BIST_RSTN      : 1;         ///<BIT [24] PCIe SLV_AFIFO BIST Reset.
        uint32_t PCIE_SLV_AFIFO_RUNSTOP        : 1;         ///<BIT [25] PCIe SLV_AFIFO BIST Run/Stop Control for MBIST Diagnostic Mode.
        uint32_t PCIE_SLV_AFIFO_ROWCOLSWITCH   : 1;         ///<BIT [26] PCIe SLV_AFIFO BIST Row-Column Switch.
        uint32_t PCIE_SLV_AFIFO_BIST_STT_IN    : 1;         ///<BIT [27] PCIe SLV_AFIFO BIST Status Input.
        uint32_t PCIE_SLV_AFIFO_BIST_STT_EN    : 1;         ///<BIT [28] PCIe SLV_AFIFO BIST Status Enable.
        uint32_t PCIE_SLV_AFIFO_BIST_STT_CLK   : 1;         ///<BIT [29] PCIe SLV_AFIFO BIST Status Clock.
        uint32_t PCIE_SLV_AFIFO_BISTMD         : 1;         ///<BIT [30] PCIe SLV_AFIFO BIST Mode Enable.
        uint32_t RF0115248_31_RSVD             : 1;         ///<BIT [31] Reserved.
    } b;    
} PcieMbistSlaveCtrl_t;

///  @brief 0xF011524C : PCIe MBIST Core Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PCIE_SR2P_BIST_STT_OUT       : 1;         ///<BIT [0] PCIe SR2P BIST Status Output.
        uint32_t PCIE_SR2P_BIST_FIN           : 1;         ///<BIT [1] PCIe SR2P BIST Finish.
        uint32_t PCIE_SR2P_BIST_ERR           : 1;         ///<BIT [2] PCIe SR2P BIST Error.
        uint32_t PCIE_SR2P_BIST_FL            : 4;         ///<BIT [6:3] PCIe SR2P BIST Fail.
        uint32_t RF011524C_7_RSVD             : 1;         ///<BIT [7] Reserved.
        uint32_t PCIE_RADMX_ASYN_BIST_STT_OUT : 1;         ///<BIT [8] PCIe RADMX_ASYN BIST Status Output.
        uint32_t PCIE_RADMX_ASYN_BIST_FIN     : 1;         ///<BIT [9] PCIe RADMX_ASYN BIST Finish.
        uint32_t PCIE_RADMX_ASYN_BIST_ERR     : 1;         ///<BIT [10] PCIe RADMX_ASYN BIST Error.
        uint32_t PCIE_RADMX_ASYN_BIST_FL      : 1;         ///<BIT [11] PCIe RADMX_ASYN BIST Fail.
        uint32_t RF011524C_15_12_RSVD         : 4;         ///<BIT [15:12] Reserved.
        uint32_t PCIE_MCB_SB_BIST_STT_OUT     : 1;         ///<BIT [16] PCIe MCB_SB BIST Status Output.
        uint32_t PCIE_MCB_SB_BIST_FIN         : 1;         ///<BIT [17] PCIe MCB_SB BIST Finish.
        uint32_t PCIE_MCB_SB_BIST_ERR         : 1;         ///<BIT [18] PCIe MCB_SB BIST Error.
        uint32_t PCIE_MCB_SB_BIST_FL          : 1;         ///<BIT [19] PCIe MCB_SB BIST Fail.
        uint32_t RF011524C_23_20_RSVD         : 4;         ///<BIT [23:20] Reserved.
        uint32_t PCIE_XDLH_BIST_STT_OUT       : 1;         ///<BIT [24] PCIe XDLH BIST Status Output.
        uint32_t PCIE_XDLH_BIST_FIN           : 1;         ///<BIT [25] PCIe XDLH BIST Finish.
        uint32_t PCIE_XDLH_BIST_ERR           : 1;         ///<BIT [26] PCIe XDLH BIST Error.
        uint32_t PCIE_XDLH_BIST_FL            : 1;         ///<BIT [27] PCIe XDLH BIST Fail.
        uint32_t RF011524C_31_28_RSVD         : 4;         ///<BIT [31:28] Reserved.
    } b;    
} PcieMbistCoreStat_t;

///  @brief 0xF0115250 : PCIe MBIST Master Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PCIE_RADMX_DATA_BIST_STT_OUT : 1;         ///<BIT [0] PCIe RADMX BIST Status Output.
        uint32_t PCIE_RADMX_DATA_BIST_FIN     : 1;         ///<BIT [1] PCIe RADMX BIST Finish.
        uint32_t PCIE_RADMX_DATA_BIST_ERR     : 1;         ///<BIT [2] PCIe RADMX BIST Error.
        uint32_t PCIE_RADMX_DATA_BIST_FL      : 4;         ///<BIT [6:3] PCIe RADMX BIST Fail.
        uint32_t RF0115250_7_RSVD             : 1;         ///<BIT [7] Reserved.
        uint32_t PCIE_MCB_A2C_BIST_STT_OUT    : 1;         ///<BIT [8] PCIe MCB_A2C BIST Status Output.
        uint32_t PCIE_MCB_A2C_BIST_FIN        : 1;         ///<BIT [9] PCIe MCB_A2C BIST Finish.
        uint32_t PCIE_MCB_A2C_BIST_ERR        : 1;         ///<BIT [10] PCIe MCB_A2C BIST Error.
        uint32_t PCIE_MCB_A2C_BIST_FL         : 1;         ///<BIT [11] PCIe MCB_A2C BIST Fail.
        uint32_t RF0115250_31_12_RSVD         :20;         ///<BIT [31:12] Reserved.
    } b;    
} PcieMbistMstrStat_t;

///  @brief 0xF0115254 : PCIe MBIST Slave Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PCIE_XADMX1_HDR_BIST_STT_OUT : 1;         ///<BIT [0] PCIe XADMX1_HDR BIST Status Output.
        uint32_t PCIE_XADMX1_HDR_BIST_FIN     : 1;         ///<BIT [1] PCIe XADMX1_HDR BIST Finish.
        uint32_t PCIE_XADMX1_HDR_BIST_ERR     : 1;         ///<BIT [2] PCIe XADMX1_HDR BIST Error.
        uint32_t PCIE_XADMX1_HDR_BIST_FL      : 1;         ///<BIT [3] PCIe XADMX1_HDR BIST Fail.
        uint32_t RF0115254_7_4_RSVD           : 4;         ///<BIT [7:4] Reserved.
        uint32_t PCIE_XADMX1_DATABIST_STT_OUT : 1;         ///<BIT [8] PCIe XADMX1_DATA BIST Status Output.
        uint32_t PCIE_XADMX1_DATA_BIST_FIN    : 1;         ///<BIT [9] PCIe XADMX1_DATA BIST Finish.
        uint32_t PCIE_XADMX1_DATA_BIST_ERR    : 1;         ///<BIT [10] PCIe XADMX1_DATA BIST Error.
        uint32_t PCIE_XADMX1_DATA_BIST_FL     : 1;         ///<BIT [11] PCIe XADMX1_DATA BIST Fail.
        uint32_t RF0115254_15_12_RSVD         : 4;         ///<BIT [15:12] Reserved.
        uint32_t PCIE_SLV_MGR_BIST_STT_OUT    : 1;         ///<BIT [16] PCIe SLV_MGR BIST Status Output.
        uint32_t PCIE_SLV_MGR_BIST_FIN        : 1;         ///<BIT [17] PCIe SLV_MGR BIST Finish.
        uint32_t PCIE_SLV_MGR_BIST_ERR        : 1;         ///<BIT [18] PCIe SLV_MGR BIST Error.
        uint32_t PCIE_SLV_MGR_BIST_FL         : 1;         ///<BIT [19] PCIe SLV_MGR BIST Fail.
        uint32_t RF0115254_23_20_RSVD         : 4;         ///<BIT [23:20] Reserved.
        uint32_t PCIE_SLV_AFIFO_BIST_STT_OUT  : 1;         ///<BIT [24] PCIe SLV_AFIFO BIST Status Output.
        uint32_t PCIE_SLV_AFIFO_BIST_FIN      : 1;         ///<BIT [25] PCIe SLV_AFIFO BIST Finish.
        uint32_t PCIE_SLV_AFIFO_BIST_ERR      : 1;         ///<BIT [26] PCIe SLV_AFIFO BIST Error.
        uint32_t PCIE_SLV_AFIFO_BIST_FL       : 1;         ///<BIT [27] PCIe SLV_AFIFO BIST Fail.
        uint32_t RF0115254_31_28_RSVD         : 4;         ///<BIT [31:28] Reserved.
    } b;    
} PcieMbistSlaveStat_t;

///  @brief 0xF0115280 : CPU Event Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPU0_CPU1_EVENT_EN   : 1;         ///<BIT [0] CPU0 Event 0 to CPU1 Event 1.
        uint32_t CPU0_CPU2_EVENT_EN   : 1;         ///<BIT [1] CPU0 Event 0 to CPU2 Event 1.
        uint32_t RF0115280_3_2_RSVD   : 2;         ///<BIT [3:2] Reserved.
        uint32_t CPU1_CPU0_EVENT_EN   : 1;         ///<BIT [4] CPU1 Event 0 to CPU0 Event 1.
        uint32_t CPU1_CPU2_EVENT_EN   : 1;         ///<BIT [5] CPU1 Event 0 to CPU2 Event 1.
        uint32_t RF0115280_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint32_t CPU2_CPU0_EVENT_EN   : 1;         ///<BIT [8] CPU2 Event 0 to CPU0 Event 1.
        uint32_t CPU2_CPU1_EVENT_EN   : 1;         ///<BIT [9] CPU2 Event 0 To CPU1 Event 1.
        uint32_t RF0115280_31_10_RSVD :22;         ///<BIT [31:10] Reserved.
    } b;    
} CpuEvtCtrl_t;


/// @brief 0xF0115180-0xF0115190 : TempSensor_t MODULE
typedef struct
{
    TempSensorCtrl0_t tempSensorCtrl0;     ///< 0xF0115180 : Temperature Sensor Control 0
    TempSensorCtrl1_t tempSensorCtrl1;     ///< 0xF0115184 : Temperature Sensor Control 1
    TempSensorStat0_t tempSensorStat0;     ///< 0xF0115188 : Temperature Sensor Status 0
    TempSensorStat1_t tempSensorStat1;     ///< 0xF011518C : Temperature Sensor Status 1
} TempSensor_t;
COMPILE_ASSERT(offsetof(TempSensor_t,tempSensorStat1)==0xF011518C-0xF0115180,"check register structure offset");


/// @brief 0xF0115200-0xF011528C : Mbist_t MODULE
typedef struct
{
    DdrMbistCtrl0_t ddrMbistCtrl0;               ///< 0xF0115200 : DDR MBIST Control 0
    DdrMbistCtrl1_t ddrMbistCtrl1;               ///< 0xF0115204 : DDR MBIST Control 1
    DdrMbistStat0_t ddrMbistStat0;               ///< 0xF0115208 : DDR MBIST Status 0
    DdrMbistStat1_t ddrMbistStat1;               ///< 0xF011520C : DDR MBIST Status 1
    MiscMbistCtrl_t miscMbistCtrl;               ///< 0xF0115210 : Miscellaneous MBIST Control
    MiscMbistStat0_t miscMbistStat0;             ///< 0xF0115214 : Miscellaneous MBIST Status 0
    MiscMbistStat1_t miscMbistStat1;             ///< 0xF0115218 : Miscellaneous MBIST Status 1
    AhciMbistCtrl_t ahciMbistCtrl;               ///< 0xF011521C : AHCI MBIST Control
    AhciMbistStat_t ahciMbistStat;               ///< 0xF0115220 : AHCI MBIST Status
    BcmMbistCtrl_t bcmMbistCtrl;                 ///< 0xF0115224 : BCM MBIST Control
    BcmMbistStat_t bcmMbistStat;                 ///< 0xF0115228 : BCM MBIST Status
    CpuMbistCtrl_t cpuMbistCtrl;                 ///< 0xF011522C : CPU MBIST Control
    CpuMbistStat0_t cpuMbistStat0;               ///< 0xF0115230 : CPU MBIST Status 0
    CpuMbistStat1_t cpuMbistStat1;               ///< 0xF0115234 : CPU MBIST Status 1
    CpuMbistStat2_t cpuMbistStat2;               ///< 0xF0115238 : CPU MBIST Status 2
    uint8_t rsvd_RF011523C[4];                   ///< 0xF011523C - 0xF011523F : Reserved.
    PcieMbistCoreCtrl_t pcieMbistCoreCtrl;       ///< 0xF0115240 : PCIe MBIST Core Control
    PcieMbistMstrCtrl_t pcieMbistMstrCtrl;       ///< 0xF0115244 : PCIe MBIST Master Control
    PcieMbistSlaveCtrl_t pcieMbistSlaveCtrl;     ///< 0xF0115248 : PCIe MBIST Slave Control
    PcieMbistCoreStat_t pcieMbistCoreStat;       ///< 0xF011524C : PCIe MBIST Core Status
    PcieMbistMstrStat_t pcieMbistMstrStat;       ///< 0xF0115250 : PCIe MBIST Master Status
    PcieMbistSlaveStat_t pcieMbistSlaveStat;     ///< 0xF0115254 : PCIe MBIST Slave Status
    uint8_t rsvd_RF0115258[8];                   ///< 0xF0115258 - 0xF011525F : Reserved.
    uint32_t OTP_FUSE_LCS_OUT_L;                 ///< 0xF0115260 : OTP Fuse Monitor 0
    uint32_t OTP_FUSE_LCS_OUT_H;                 ///< 0xF0115264 : OTP Fuse Monitor 1
    uint8_t rsvd_RF0115268[24];                  ///< 0xF0115268 - 0xF011527F : Reserved.
    CpuEvtCtrl_t cpuEvtCtrl;                     ///< 0xF0115280 : CPU Event Control
} Mbist_t;
COMPILE_ASSERT(offsetof(Mbist_t,cpuEvtCtrl)==0xF0115280-0xF0115200,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile TempSensor_t rTempSensor;            ///< 0xF0115180-0xF0115190 : TempSensor_t MODULE
extern volatile Mbist_t rMbist;                      ///< 0xF0115200-0xF011526C : Mbist_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

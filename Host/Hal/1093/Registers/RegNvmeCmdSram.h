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
//! @brief NVMe Command SRAM Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  NVMe Command SRAM Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF5017000 : Command SRAM Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CSRM_ARB_MD         : 2;         ///<BIT [1:0] Command SRAM Arbitration Mode.
        uint32_t RF5017000_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} CmdSramCfg_t;

///  @brief 0xF5017004 : Command SRAM Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HM_FORCE_READ_PERR   : 2;         ///<BIT [1:0] Force Hardware Command SRAM Read Parity Error.
        uint32_t RF5017004_3_2_RSVD   : 2;         ///<BIT [3:2] Reserved.
        uint32_t HM_IGNOR_READ_PERR   : 1;         ///<BIT [4] Ignore Hardware Command SRAM Read Parity Error.
        uint32_t RF5017004_15_5_RSVD  :11;         ///<BIT [15:5] Reserved.
        uint32_t FM_FORCE_RD_PERR     : 2;         ///<BIT [17:16] Force Firmware Command SRAM Read Parity Error.
        uint32_t RF5017004_19_18_RSVD : 2;         ///<BIT [19:18] Reserved.
        uint32_t FM_IGNOR_RD_PERR     : 1;         ///<BIT [20] Ignore Firmware Command SRAM Read Parity Error.
        uint32_t RF5017004_31_21_RSVD :11;         ///<BIT [31:21] Reserved.
    } b;    
} CmdSramCtrl_t;

///  @brief 0xF5017008 : Command SRAM Command
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HM_ZERO_START       : 1;         ///<BIT [0] Hardware Command SRAM Zeroize Start.
        uint32_t FM_ZERO_START       : 1;         ///<BIT [1] Firmware Command SRAM Zeroize Start.
        uint32_t RF5017008_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} CmdSramCmd_t;

///  @brief 0xF501700C : Command SRAM Engine Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HM_ZERO_DONE         : 1;         ///<BIT [0] Hardware Command SRAM Zeroize Done.
        uint32_t FM_ZERO_DONE         : 1;         ///<BIT [1] Firmware Command SRAM Zeroize Done.
        uint32_t RF501700C_15_2_RSVD  :14;         ///<BIT [15:2] Reserved.
        uint32_t APB_ACC_STATE        : 3;         ///<BIT [18:16] CPU Memory Accelerator State.
        uint32_t RF501700C_23_19_RSVD : 5;         ///<BIT [23:19] Reserved.
        uint32_t AHB_ACC_STATE        : 4;         ///<BIT [27:24] Command SRAM AHB Access State.
        uint32_t RF501700C_31_28_RSVD : 4;         ///<BIT [31:28] Reserved.
    } b;    
} CmdSramEngineStat_t;

///  @brief 0xF5017020 : Command SRAM Firmware Admin Command Count
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FW_AD_FIFO_CNTR     : 4;         ///<BIT [3:0] Firmware Admin Command Available Counter.
        uint32_t RF5017020_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} CmdSramFwAdminCmdCnt_t;

///  @brief 0xF5017024 : Command SRAM Firmware Admin Command Count Decrease
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FW_AD_FIFO_CNTR_DEC : 4;         ///<BIT [3:0] Firmware Admin Command Available Counter Decrease.
        uint32_t RF5017024_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} CmdSramFwAdminCmdCntDecrease_t;

///  @brief 0xF5017030 : Command SRAM Firmware Admin Command Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FW_AD_FIFO_RDPTR     : 8;         ///<BIT [7:0] Firmware Admin Command Read Pointer.
        uint32_t RF5017030_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t FW_AD_FIFO_WRPTR     : 8;         ///<BIT [23:16] Firmware Admin Command Write Pointer.
        uint32_t RF5017030_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} CmdSramFwAdminCmdPtr_t;

///  @brief 0xF5017040 : Command SRAM Firmware I/O Command Count
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FW_IO_FIFO_CNTR     : 5;         ///<BIT [4:0] Firmware I/O Command Available Counter.
        uint32_t RF5017040_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} CmdSramFwIOCmdCnt_t;

///  @brief 0xF5017044 : Command SRAM Firmware I/O Command Count Decrease
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FW_IO_FIFO_CNTR_DEC : 5;         ///<BIT [4:0] Firmware I/O Command Counter Decrease.
        uint32_t RF5017044_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} CmdSramFwIOCmdCntDecrease_t;

///  @brief 0xF5017050 : Command SRAM Firmware I/O Command Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FW_IO_FIFO_RDPTR     : 8;         ///<BIT [7:0] Firmware I/O Command Read Pointer.
        uint32_t RF5017050_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t FW_IO_FIFO_WRPTR     : 8;         ///<BIT [23:16] Firmware I/O Command Write Pointer.
        uint32_t RF5017050_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} CmdSramFwIOCmdPtr_t;

///  @brief 0xF5017060 : Command SRAM Access Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HW_CSRM_LCK          : 1;         ///<BIT [0] Hardware Command SRAM Memory Lock.
        uint32_t RF5017060_15_1_RSVD  :15;         ///<BIT [15:1] Reserved.
        uint32_t F_CSRM_LCK           : 1;         ///<BIT [16] Firmware Command SRAM Memory Lock.
        uint32_t RF5017060_31_17_RSVD :15;         ///<BIT [31:17] Reserved.
    } b;    
} CmdSramAccessCtrl_t;

///  @brief 0xF5017064 : Command SRAM Access Status 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HM_ERR_ADDR          :13;         ///<BIT [12:0] Hardware Command SRAM Error Address.
        uint32_t RF5017064_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t FM_ERR_ADDR          : 9;         ///<BIT [24:16] Firmware Command SRAM Error Address.
        uint32_t RF5017064_31_25_RSVD : 7;         ///<BIT [31:25] Reserved.
    } b;    
} CmdSramAccessStat0_t;

///  @brief 0xF5017068 : Command SRAM Access Status 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HM_ERR_STAT          : 2;         ///<BIT [1:0] Hardware Command SRAM Error Status.
        uint32_t RF5017068_15_2_RSVD  :14;         ///<BIT [15:2] Reserved.
        uint32_t FM_ERR_STAT          : 2;         ///<BIT [17:16] Firmware Command SRAM Error Status.
        uint32_t RF5017068_31_18_RSVD :14;         ///<BIT [31:18] Reserved.
    } b;    
} CmdSramAccessStat1_t;

///  @brief 0xF5017070 : Hardware Command SRAM Access Status Parity
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t HW_CSRM_ACC_PAR     : 7;         ///<BIT [6:0] Hardware Command SRAM Access Parity.
        uint32_t RF5017070_31_7_RSVD :25;         ///<BIT [31:7] Reserved.
    } b;    
} HardwareCmdSramAccessStatPar_t;

///  @brief 0xF5017080 : Firmware Command SRAM Access Status Parity
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FW_CSRM_ACC_PAR     : 7;         ///<BIT [6:0] Firmware Command SRAM Access Parity.
        uint32_t RF5017080_31_7_RSVD :25;         ///<BIT [31:7] Reserved.
    } b;    
} FwCmdSramAccessStatPar_t;

///  @brief 0xF501708C : Command SRAM Monitor Signal Select
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CSRM_SEL            : 2;         ///<BIT [1:0] Command SRAM Monitor Signal Select.
        uint32_t RF501708C_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} CmdSramMonitorSgnSel_t;

///  @brief 0xF5017090 : Command SRAM HM Indirect Access Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CSRM_HM_IND_ACC_ADDR :13;         ///<BIT [12:0] Command SRAM Indirect Hardware Memory Access Address.
        uint32_t RF5017090_31_13_RSVD :19;         ///<BIT [31:13] Reserved.
    } b;    
} CmdSramHmIndAccessAddr_t;

///  @brief 0xF50170A0 : Command SRAM FM Indirect Access Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CSRM_FM_IND_ACC_ADDR : 9;         ///<BIT [8:0] Firmware Command SRAM Indirect Memory Access Address.
        uint32_t RF50170A0_31_9_RSVD  :23;         ///<BIT [31:9] Reserved.
    } b;    
} CmdSramFmIndAccessAddr_t;


/// @brief 0xF5017000-0xF50170A8 : NvmeCmdSram_t MODULE
typedef struct
{
    CmdSramCfg_t cmdSramCfg;                                         ///< 0xF5017000 : Command SRAM Configuration
    CmdSramCtrl_t cmdSramCtrl;                                       ///< 0xF5017004 : Command SRAM Control
    CmdSramCmd_t cmdSramCmd;                                         ///< 0xF5017008 : Command SRAM Command
    CmdSramEngineStat_t cmdSramEngineStat;                           ///< 0xF501700C : Command SRAM Engine Status
    uint8_t rsvd_RF5017010[16];                                      ///< 0xF5017010 - 0xF501701F : Reserved.
    CmdSramFwAdminCmdCnt_t cmdSramFwAdminCmdCnt;                     ///< 0xF5017020 : Command SRAM Firmware Admin Command Count
    CmdSramFwAdminCmdCntDecrease_t cmdSramFwAdminCmdCntDecrease;     ///< 0xF5017024 : Command SRAM Firmware Admin Command Count Decrease
    uint8_t rsvd_RF5017028[8];                                       ///< 0xF5017028 - 0xF501702F : Reserved.
    CmdSramFwAdminCmdPtr_t cmdSramFwAdminCmdPtr;                     ///< 0xF5017030 : Command SRAM Firmware Admin Command Pointer
    uint8_t rsvd_RF5017034[12];                                      ///< 0xF5017034 - 0xF501703F : Reserved.
    CmdSramFwIOCmdCnt_t cmdSramFwIOCmdCnt;                           ///< 0xF5017040 : Command SRAM Firmware I/O Command Count
    CmdSramFwIOCmdCntDecrease_t cmdSramFwIOCmdCntDecrease;           ///< 0xF5017044 : Command SRAM Firmware I/O Command Count Decrease
    uint8_t rsvd_RF5017048[8];                                       ///< 0xF5017048 - 0xF501704F : Reserved.
    CmdSramFwIOCmdPtr_t cmdSramFwIOCmdPtr;                           ///< 0xF5017050 : Command SRAM Firmware I/O Command Pointer
    uint8_t rsvd_RF5017054[12];                                      ///< 0xF5017054 - 0xF501705F : Reserved.
    CmdSramAccessCtrl_t cmdSramAccessCtrl;                           ///< 0xF5017060 : Command SRAM Access Control
    CmdSramAccessStat0_t cmdSramAccessStat0;                         ///< 0xF5017064 : Command SRAM Access Status 0
    CmdSramAccessStat1_t cmdSramAccessStat1;                         ///< 0xF5017068 : Command SRAM Access Status 1
    uint8_t rsvd_RF501706C[4];                                       ///< 0xF501706C - 0xF501706F : Reserved.
    HardwareCmdSramAccessStatPar_t hardwareCmdSramAccessStatPar;     ///< 0xF5017070 : Hardware Command SRAM Access Status Parity
    uint32_t HW_CSRM_ACC_STAT_DW0;                                   ///< 0xF5017074 : Hardware Command SRAM Access Status Dword 0
    uint8_t rsvd_RF5017078[8];                                       ///< 0xF5017078 - 0xF501707F : Reserved.
    FwCmdSramAccessStatPar_t fwCmdSramAccessStatPar;                 ///< 0xF5017080 : Firmware Command SRAM Access Status Parity
    uint32_t FW_CSRM_ACC_STAT_DW0;                                   ///< 0xF5017084 : Firmware Command SRAM Access Status Dword 0
    uint8_t rsvd_RF5017088[4];                                       ///< 0xF5017088 - 0xF501708B : Reserved.
    CmdSramMonitorSgnSel_t cmdSramMonitorSgnSel;                     ///< 0xF501708C : Command SRAM Monitor Signal Select
    CmdSramHmIndAccessAddr_t cmdSramHmIndAccessAddr;                 ///< 0xF5017090 : Command SRAM HM Indirect Access Address
    uint32_t CSRM_HM_IND_ACC_DATA;                                   ///< 0xF5017094 : Command SRAM HM Indirect Access Data
    uint32_t CSRM_HM_IND_ACC_MSK;                                    ///< 0xF5017098 : Command SRAM HM Indirect Access Mask
    uint8_t rsvd_RF501709C[4];                                       ///< 0xF501709C - 0xF501709F : Reserved.
    CmdSramFmIndAccessAddr_t cmdSramFmIndAccessAddr;                 ///< 0xF50170A0 : Command SRAM FM Indirect Access Address
    uint32_t CSRM_FM_IND_ACC_DATA;                                   ///< 0xF50170A4 : Command SRAM FM Indirect Access Data
} NvmeCmdSram_t;
COMPILE_ASSERT(offsetof(NvmeCmdSram_t,CSRM_FM_IND_ACC_DATA)==0xF50170A4-0xF5017000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile NvmeCmdSram_t rNvmeCmdSram;          ///< 0xF5017000-0xF50170A8 : NvmeCmdSram_t MODULE

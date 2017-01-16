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
//! @brief NVMe BitMap SRAM Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  NVMe BitMap SRAM Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF5016000 : Bitmap SRAM Arbitration Mode
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BSRM_ARB_MD         : 2;         ///<BIT [1:0] Bitmap SRAM Arbitration Mode.
        uint32_t RF5016000_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} BitmapSramArbMode_t;

///  @brief 0xF5016004 : Bitmap SRAM Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FORCE_RD_PERR       : 2;         ///<BIT [1:0] Force BSRAM Read Parity Error.
        uint32_t RF5016004_3_2_RSVD  : 2;         ///<BIT [3:2] Reserved.
        uint32_t IGNOR_RD_PERR       : 1;         ///<BIT [4] Ignore Memory Read Parity Error.
        uint32_t RF5016004_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} BitmapSramCtrl_t;

///  @brief 0xF5016008 : Bitmap SRAM Command
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BSRM_ZERO_START     : 1;         ///<BIT [0] Bitmap SRAM Zeroize Start.
        uint32_t RF5016008_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} BitmapSramCmd_t;

///  @brief 0xF501600C : Bitmap SRAM Engine Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BSRM_ZERO_DONE       : 1;         ///<BIT [0] Bitmap SRAM Zeroize Done.
        uint32_t RF501600C_15_1_RSVD  :15;         ///<BIT [15:1] Reserved.
        uint32_t CPU_ACC_STATE        : 3;         ///<BIT [18:16] CPU Memory Accelerator State.
        uint32_t RF501600C_23_19_RSVD : 5;         ///<BIT [23:19] Reserved.
        uint32_t AHB_ACC_STATE        : 5;         ///<BIT [28:24] Bitmap SRAM AHB Access State.
        uint32_t RF501600C_31_29_RSVD : 3;         ///<BIT [31:29] Reserved.
    } b;    
} BitmapSramEngineStat_t;

///  @brief 0xF5016020 : Bitmap SRAM Command Index Available Bitmap Counter
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CIABM_CNTR          : 9;         ///<BIT [8:0] Command Index Available Bitmap Counter.
        uint32_t RF5016020_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;    
} BitmapSramCmdIndexAvailablBitmapCntr_t;

///  @brief 0xF5016024 : Bitmap SRAM Command Index Available Bitmap Counter Decrease
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CIABM_CNTR_DEC      : 9;         ///<BIT [8:0] Command Index Available Bitmap Counter Decrease.
        uint32_t RF5016024_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;    
} BitmapSramCmdIndexAvailablBitmapCntrDecrease_t;

///  @brief 0xF5016028 : Bitmap SRAM Command Index Available Bitmap Counter Increase
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CIABM_CNTR_INC      : 9;         ///<BIT [8:0] Command Index Available Bitmap Counter Increase.
        uint32_t RF5016028_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;    
} BitmapSramCmdIndexAvailablBitmapCntrIncrease_t;

///  @brief 0xF5016030 : Bitmap SRAM Command Completion Bitmap Counter
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CCBM_CNTR           : 9;         ///<BIT [8:0] Command Completion Bitmap Counter.
        uint32_t RF5016030_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;    
} BitmapSramCmdCompletiBitmapCntr_t;

///  @brief 0xF5016034 : Bitmap SRAM Command Completion Bitmap Counter Decrease
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CCBM_CNTR_DEC       : 9;         ///<BIT [8:0] Command Completion Bitmap Counter Decrease.
        uint32_t RF5016034_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;    
} BitmapSramCmdCompletiBitmapCntrDecrease_t;

///  @brief 0xF5016038 : Bitmap SRAM Command Completion Bitmap Counter Increase
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CCBM_CNTR_INC       : 9;         ///<BIT [8:0] Command Completion Bitmap Counter Increase.
        uint32_t RF5016038_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;    
} BitmapSramCmdCompletiBitmapCntrIncrease_t;

///  @brief 0xF5016050 : Bitmap SRAM Command Completion Bitmap Timer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CCBM_TO_THR          :16;         ///<BIT [15:0] Command Completion Bitmap Time-Out Threshold.
        uint32_t RF5016050_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} BitmapSramCmdCompletiBitmapTmr_t;

///  @brief 0xF5016054 : Bitmap SRAM Command Completion Bitmap Threshold
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CCBM_INTR_THR       : 9;         ///<BIT [8:0] Command Completion Bitmap Interrupt Threshold.
        uint32_t RF5016054_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;    
} BitmapSramCmdCompletiBitmapThr_t;

///  @brief 0xF5016060 : Bitmap SRAM Command Index Available Bitmap Group Bitmap
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CIABM_GBM           : 8;         ///<BIT [7:0] Command Index Available Bitmap Group Bitmap.
        uint32_t RF5016060_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} BitmapSramCmdIndexAvailablBitmapGroupBitmap_t;

///  @brief 0xF5016064 : Bitmap SRAM Command Activation Bitmap Group Bitmap
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CABM_0_GBM           : 8;         ///<BIT [7:0] Command Activation Group Bitmap 0.
        uint32_t CABM_1_GBM           : 8;         ///<BIT [15:8] Command Activation Group Bitmap 1.
        uint32_t CABM_1_MSK_GBM       : 8;         ///<BIT [23:16] Command Activation Bitmap 1 Mask Group Bitmap.
        uint32_t RF5016064_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} BitmapSramCmdActivatiBitmapGroupBitmap_t;

///  @brief 0xF5016068 : Bitmap SRAM PRP Request Bitmap Group Bitmap
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PRBM_W0_GBM : 8;         ///<BIT [7:0] PRP Request Bitmap for Write Commands 0 Group Bitmap.
        uint32_t PRBM_W1_GBM : 8;         ///<BIT [15:8] PRP Request Bitmap for Write Commands 1 Group Bitmap.
        uint32_t PRBM_R0_GBM : 8;         ///<BIT [23:16] PRP Request Bitmap for Read Commands 0 Group Bitmap.
        uint32_t PRBM_R1_GBM : 8;         ///<BIT [31:24] PRP Request Bitmap for Read Commands 1 Group Bitmap.
    } b;    
} BitmapSramPrpReqBitmapGroupBitmap_t;

///  @brief 0xF501606C : Bitmap SRAM Read Data Ready Bitmap Group Bitmap
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RDRBM_0_GBM          : 8;         ///<BIT [7:0] Read Data Ready Bitmap 0 Group Bitmap.
        uint32_t RDRBM_1_GBM          : 8;         ///<BIT [15:8] Read Data Ready Bitmap 1 Group Bitmap.
        uint32_t RDRBM_1_MSK_GBM      : 8;         ///<BIT [23:16] Read Data Ready Bitmap 1 Mask Group Bitmap.
        uint32_t RF501606C_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} BitmapSramRdDataRdyBitmapGroupBitmap_t;

///  @brief 0xF5016070 : Bitmap SRAM Data Transfer Done Bitmap Group Bitmap
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DTDBM_GBM           : 8;         ///<BIT [7:0] Data Transfer Done Bitmap Group Bitmap.
        uint32_t RF5016070_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} BitmapSramDataXfrDoneBitmapGroupBitmap_t;

///  @brief 0xF5016074 : Bitmap SRAM Backend Abort Done Bitmap Group Bitmap
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BADBM_GBM           : 8;         ///<BIT [7:0] Backend Abort Done Bitmap Group Bitmap.
        uint32_t RF5016074_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} BitmapSramBackendAbortDoneBitmapGroupBitmap_t;

///  @brief 0xF5016078 : Bitmap SRAM Command Completion Bitmap Group Bitmap
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CCBM_GBM            : 8;         ///<BIT [7:0] Command Completion Bitmap Group Bitmap.
        uint32_t RF5016078_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} BitmapSramCmdCompletiBitmapGroupBitmap_t;

///  @brief 0xF5016080 : Bitmap SRAM Access Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BSRAM_MEM_LOCK      : 1;         ///<BIT [0] Bitmap SRAM Memory Lock.
        uint32_t RF5016080_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} BitmapSramAccessCtrl_t;

///  @brief 0xF5016084 : Bitmap SRAM Access Status 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BSRAM_ERR_ADDR      : 7;         ///<BIT [6:0] Bitmap SRAM Error Address.
        uint32_t RF5016084_31_7_RSVD :25;         ///<BIT [31:7] Reserved.
    } b;    
} BitmapSramAccessStat0_t;

///  @brief 0xF5016088 : Bitmap SRAM Access Status 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BSRAM_ERR_STS       : 2;         ///<BIT [1:0] Bitmap SRAM Error Status.
        uint32_t RF5016088_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} BitmapSramAccessStat1_t;

///  @brief 0xF5016090 : Bitmap SRAM Access Status Parity
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BSRAM_ACC_STS_PAR   : 7;         ///<BIT [6:0] Bitmap SRAM Access Status Parity.
        uint32_t RF5016090_31_7_RSVD :25;         ///<BIT [31:7] Reserved.
    } b;    
} BitmapSramAccessStatPar_t;

///  @brief 0xF501609C : Bitmap SRAM Monitor Signal Select
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BSRAM_MON_SIG_SEL   : 1;         ///<BIT [0] BSRAM Monitor Signal Select.
        uint32_t RF501609C_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} BitmapSramMonitorSgnSel_t;

///  @brief 0xF50160A0 : Bitmap SRAM Indirect Memory Access Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BSRM_IND_ACC_ADDR   : 7;         ///<BIT [6:0] Bitmap SRAM Indirect Memory Access Address.
        uint32_t RF50160A0_31_7_RSVD :25;         ///<BIT [31:7] Reserved.
    } b;    
} BitmapSramIndMemAccessAddr_t;


/// @brief 0xF5016000-0xF50160AC : NvmeBitmapSram_t MODULE
typedef struct
{
    BitmapSramArbMode_t bitmapSramArbMode;                                                           ///< 0xF5016000 : Bitmap SRAM Arbitration Mode
    BitmapSramCtrl_t bitmapSramCtrl;                                                                 ///< 0xF5016004 : Bitmap SRAM Control
    BitmapSramCmd_t bitmapSramCmd;                                                                   ///< 0xF5016008 : Bitmap SRAM Command
    BitmapSramEngineStat_t bitmapSramEngineStat;                                                     ///< 0xF501600C : Bitmap SRAM Engine Status
    uint8_t rsvd_RF5016010[16];                                                                      ///< 0xF5016010 - 0xF501601F : Reserved.
    BitmapSramCmdIndexAvailablBitmapCntr_t bitmapSramCmdIndexAvailablBitmapCntr;                     ///< 0xF5016020 : Bitmap SRAM Command Index Available Bitmap Counter
    BitmapSramCmdIndexAvailablBitmapCntrDecrease_t bitmapSramCmdIndexAvailablBitmapCntrDecrease;     ///< 0xF5016024 : Bitmap SRAM Command Index Available Bitmap Counter Decrease
    BitmapSramCmdIndexAvailablBitmapCntrIncrease_t bitmapSramCmdIndexAvailablBitmapCntrIncrease;     ///< 0xF5016028 : Bitmap SRAM Command Index Available Bitmap Counter Increase
    uint8_t rsvd_RF501602C[4];                                                                       ///< 0xF501602C - 0xF501602F : Reserved.
    BitmapSramCmdCompletiBitmapCntr_t bitmapSramCmdCompletiBitmapCntr;                               ///< 0xF5016030 : Bitmap SRAM Command Completion Bitmap Counter
    BitmapSramCmdCompletiBitmapCntrDecrease_t bitmapSramCmdCompletiBitmapCntrDecrease;               ///< 0xF5016034 : Bitmap SRAM Command Completion Bitmap Counter Decrease
    BitmapSramCmdCompletiBitmapCntrIncrease_t bitmapSramCmdCompletiBitmapCntrIncrease;               ///< 0xF5016038 : Bitmap SRAM Command Completion Bitmap Counter Increase
    uint8_t rsvd_RF501603C[20];                                                                      ///< 0xF501603C - 0xF501604F : Reserved.
    BitmapSramCmdCompletiBitmapTmr_t bitmapSramCmdCompletiBitmapTmr;                                 ///< 0xF5016050 : Bitmap SRAM Command Completion Bitmap Timer
    BitmapSramCmdCompletiBitmapThr_t bitmapSramCmdCompletiBitmapThr;                                 ///< 0xF5016054 : Bitmap SRAM Command Completion Bitmap Threshold
    uint8_t rsvd_RF5016058[8];                                                                       ///< 0xF5016058 - 0xF501605F : Reserved.
    BitmapSramCmdIndexAvailablBitmapGroupBitmap_t bitmapSramCmdIndexAvailablBitmapGroupBitmap;       ///< 0xF5016060 : Bitmap SRAM Command Index Available Bitmap Group Bitmap
    BitmapSramCmdActivatiBitmapGroupBitmap_t bitmapSramCmdActivatiBitmapGroupBitmap;                 ///< 0xF5016064 : Bitmap SRAM Command Activation Bitmap Group Bitmap
    BitmapSramPrpReqBitmapGroupBitmap_t bitmapSramPrpReqBitmapGroupBitmap;                           ///< 0xF5016068 : Bitmap SRAM PRP Request Bitmap Group Bitmap
    BitmapSramRdDataRdyBitmapGroupBitmap_t bitmapSramRdDataRdyBitmapGroupBitmap;                     ///< 0xF501606C : Bitmap SRAM Read Data Ready Bitmap Group Bitmap
    BitmapSramDataXfrDoneBitmapGroupBitmap_t bitmapSramDataXfrDoneBitmapGroupBitmap;                 ///< 0xF5016070 : Bitmap SRAM Data Transfer Done Bitmap Group Bitmap
    BitmapSramBackendAbortDoneBitmapGroupBitmap_t bitmapSramBackendAbortDoneBitmapGroupBitmap;       ///< 0xF5016074 : Bitmap SRAM Backend Abort Done Bitmap Group Bitmap
    BitmapSramCmdCompletiBitmapGroupBitmap_t bitmapSramCmdCompletiBitmapGroupBitmap;                 ///< 0xF5016078 : Bitmap SRAM Command Completion Bitmap Group Bitmap
    uint8_t rsvd_RF501607C[4];                                                                       ///< 0xF501607C - 0xF501607F : Reserved.
    BitmapSramAccessCtrl_t bitmapSramAccessCtrl;                                                     ///< 0xF5016080 : Bitmap SRAM Access Control
    BitmapSramAccessStat0_t bitmapSramAccessStat0;                                                   ///< 0xF5016084 : Bitmap SRAM Access Status 0
    BitmapSramAccessStat1_t bitmapSramAccessStat1;                                                   ///< 0xF5016088 : Bitmap SRAM Access Status 1
    uint8_t rsvd_RF501608C[4];                                                                       ///< 0xF501608C - 0xF501608F : Reserved.
    BitmapSramAccessStatPar_t bitmapSramAccessStatPar;                                               ///< 0xF5016090 : Bitmap SRAM Access Status Parity
    uint32_t BSRM_ACC_STATUS_DW0;                                                                    ///< 0xF5016094 : Bitmap SRAM Access Status Dword 0
    uint8_t rsvd_RF5016098[4];                                                                       ///< 0xF5016098 - 0xF501609B : Reserved.
    BitmapSramMonitorSgnSel_t bitmapSramMonitorSgnSel;                                               ///< 0xF501609C : Bitmap SRAM Monitor Signal Select
    BitmapSramIndMemAccessAddr_t bitmapSramIndMemAccessAddr;                                         ///< 0xF50160A0 : Bitmap SRAM Indirect Memory Access Address
    uint32_t BSRM_IND_ACC_DATA;                                                                      ///< 0xF50160A4 : Bitmap SRAM Indirect Memory Access Data
    uint32_t BSRM_IND_ACC_MSK;                                                                       ///< 0xF50160A8 : Bitmap SRAM Indirect Memory Access Mask
} NvmeBitmapSram_t;
COMPILE_ASSERT(offsetof(NvmeBitmapSram_t,BSRM_IND_ACC_MSK)==0xF50160A8-0xF5016000,"check register structure offset");

//-----------------------------------------------------------------------------
//  NVME_BSRM Bitmap SRAM Data structure definitions
//-----------------------------------------------------------------------------
static const uint32_t cNvmeNumOfBitmapEntries = 8;  // Per bits for 256 HW IO Commands (8 DWs)

/// @brief NVMe Bitmap Data structure
typedef struct
{
    uint32_t CIABM[cNvmeNumOfBitmapEntries];    // Command Index Abailable Bitmap
    uint32_t CABM0[cNvmeNumOfBitmapEntries];    // Command Active Bitmap (The CPU shall only update the CABM0)
    uint32_t CABM1[cNvmeNumOfBitmapEntries];    // Command Active Bitmap (The hardware working copy)
    uint32_t CABM1MSK[cNvmeNumOfBitmapEntries]; // Command Active Bitmap (To resolve the fairness issue by the Data Manager)
    uint32_t PRBMW0[cNvmeNumOfBitmapEntries];   // PRP Request Bitmap (For a write command)
    uint32_t PRBMW1[cNvmeNumOfBitmapEntries];   // PRP Request Bitmap (For a write command and a HW working copy)
    uint32_t PRBMR0[cNvmeNumOfBitmapEntries];   // PRP Request Bitmap (For a read command)
    uint32_t PRBMR1[cNvmeNumOfBitmapEntries];   // PRP Request Bitmap (For a read command and a HW working copy)
    uint32_t RDRBM0[cNvmeNumOfBitmapEntries];   // Read Data Ready Bitmap (The backend has transferred any data to the DDR)
    uint32_t RDRBM1[cNvmeNumOfBitmapEntries];   // Read Data Ready Bitmap (The hardware working copy)
    uint32_t RDRBMMSK[cNvmeNumOfBitmapEntries]; // Read Data Ready Bitmap (To resolve the fairness issue by the Data Manager)
    uint32_t DTDBM[cNvmeNumOfBitmapEntries];    // Data Transfer Done Bitmap
    uint32_t BADBM[cNvmeNumOfBitmapEntries];    // Backend Abort Done Bitmap
    uint32_t CCBM[cNvmeNumOfBitmapEntries];     // Command Completion Bitmap
} NvmeBitmapSramData_t;
COMPILE_ASSERT(sizeof(NvmeBitmapSramData_t)==(14*8*4), "NvmeBitmapSramData_t size shall be 0x1C0");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile NvmeBitmapSram_t rNvmeBitmapSram;    ///< 0xF5016000-0xF50160AC : NvmeBitmapSram_t MODULE
extern volatile NvmeBitmapSramData_t rNvmeBitmap;    ///< 0xF501E000-0x2001E1BF : NvmeBitmapSramData_t MODULE
extern volatile NvmeBitmapSramData_t rNvmeBitmapW1s; ///< 0xF501E400-0x2001E5BF : NvmeBitmapSramData_t MODULE (Write 1 to Set)
extern volatile NvmeBitmapSramData_t rNvmeBitmapW1c; ///< 0xF501E800-0x2001E9BF : NvmeBitmapSramData_t MODULE (Write 1 to Clear)

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
//! @brief Sequencer Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  Sequencer Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF200D000 : Sequencer Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200D000_0_RSVD  : 1;         ///<BIT [0] Reserved.
        uint16_t MQ_SW_EN_P0       : 1;         ///<BIT [1] Port 0 Media Queue Software Mode Enable.
        uint16_t MQ_SW_EN_P1       : 1;         ///<BIT [2] Port 1 Media Queue Software Mode Enable.
        uint16_t RF200D000_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t MQ_SW_EN_P2       : 1;         ///<BIT [4] Port 2 Media Queue Software Mode Enable.
        uint16_t MQ_SW_EN_P3       : 1;         ///<BIT [5] Port 3 Media Queue Software Mode Enable.
        uint16_t RF200D000_6_RSVD  : 1;         ///<BIT [6] Reserved.
        uint16_t EN_ERR_MQ         : 1;         ///<BIT [7] Enable Error MQ Report.
        uint16_t MQ_ARB_MODE       : 2;         ///<BIT [9:8] Media Queue (MQ) Arbitration Mode.
        uint16_t RF200D000_10_RSVD : 1;         ///<BIT [10] Reserved.
        uint16_t DIS_HOLD_TD       : 1;         ///<BIT [11] TRK_DONE/TRK_ERR Hold Disable.
        uint16_t TRK_CONF          : 3;         ///<BIT [14:12] Track Configuration.
        uint16_t TRK_ORDR         : 1;         ///<BIT [15] Track Report Order.
    } b;    
} SeqCfg1_t;

///  @brief 0xF200D002 : Sequencer Configuration 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CLK_AUTOPD           : 1;         ///<BIT [0] Enable Sequencer Clock Auto Power-Down.
        uint16_t RF200D002_1_RSVD     : 1;         ///<BIT [1] Reserved.
        uint16_t RF200D002_2_RSVD     : 1;         ///<BIT [2] Reserved.
        uint16_t AXI_PAR_SEL          : 1;         ///<BIT [3] AXI Parity Selection.
        uint16_t LOS_CLR_DIS          : 1;         ///<BIT [4] Logical Overall Status (LOS) Clear Disable.
        uint16_t RF200D002_5_RSVD     : 1;         ///<BIT [5] Reserved.
        uint16_t RF200D002_6_RSVD     : 1;         ///<BIT [6] Reserved.
        uint16_t TRK_CCL_DIS          : 1;         ///<BIT [7] Track Cancellation Disable.
        uint16_t RAID_ITLV_DIST       : 4;         ///<BIT [11:8] Raid Interleave Distance.
        uint16_t RF200D002_13_12_RSVD : 2;         ///<BIT [13:12] Reserved.
        uint16_t RF200D002_14_RSVD    : 1;         ///<BIT [14] Reserved.
        uint16_t SQCR_CLK_DIS         : 1;         ///<BIT [15] Sequencer Clock Disable.
    } b;    
} SeqCfg2_t;

///  @brief 0xF200D004 : Sequencer Configuration 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TWO_LBA_EN          : 1;         ///<BIT [0] Enable Two LBAs.
        uint16_t AUX_SIZE            : 2;         ///<BIT [2:1] Auxiliary Data Size.
        uint16_t AUX_META_SEL        : 1;         ///<BIT [3] Auxiliary Data/Metadata Transfer Mode.
        uint16_t LBA_4B_MODE         : 1;         ///<BIT [4] LBA 4-Byte Mode.
#if (_SOC_REV_ >= 0x0210)
        uint16_t EN_ST_EN            : 1;         ///<BIT [5] Streaming Tag Streaming Port Select.
        uint16_t RF200D004_6_RSVD    : 1;         ///<BIT [6] Reserved.
#else
        uint16_t RF200D004_6_5_RSVD  : 2;         ///<BIT [6:5] Reserved.
#endif
        uint16_t AXI_PARCHK_EN       : 1;         ///<BIT [7] AXI Read Parity Check Enable.
        uint16_t RF200D004_10_8_RSVD : 3;         ///<BIT [10:8] Reserved.
        uint16_t PRP_CHK_EN          : 1;         ///<BIT [11] PRP Check Enable.
        uint16_t ALL_AU_COLLECT      : 1;         ///<BIT [12] ALL AU Collection.
        uint16_t RF200D004_13_RSVD   : 1;         ///<BIT [13] Reserved.
        uint16_t ERR_CNT_EN          : 1;         ///<BIT [14] Error Count Log Enable.
        uint16_t RF200D004_15_RSVD   : 1;         ///<BIT [15] Reserved.
    } b;    
} SeqCfg3_t;

///  @brief 0xF200D010 : Sequencer Control 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t SEQ_BUSY            : 1;         ///<BIT [0] Sequencer Busy.
        uint16_t NM_PAUSE            : 1;         ///<BIT [1] Normal Descriptor FIFO Pause.
        uint16_t RF200D010_4_2_RSVD  : 3;         ///<BIT [4:2] Reserved.
        uint16_t IO_AUX_XTRCT_EN     : 1;         ///<BIT [5] In Order Auxiliary Extract Enable.
        uint16_t TLC_SCRAMBLE_EN     : 1;         ///<BIT [6] TLC Scramble Enable.
        uint16_t RF200D010_11_7_RSVD : 5;         ///<BIT [11:7] Reserved.
        uint16_t SEQ_ENABLE          : 1;         ///<BIT [12] Enable Sequencer.
        uint16_t SEQ_ABT_MODE        : 2;         ///<BIT [14:13] Sequencer Abort Mode.
        uint16_t SEQ_RESET           : 1;         ///<BIT [15] Reserved.
    } b;    
} SeqCtrl1_t;

///  @brief 0xF200D014 : Sequencer Interrupt Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TRK_DONE           : 1;         ///<BIT [0] Track Done.
        uint16_t TRK_ERR            : 1;         ///<BIT [1] Track Error.
        uint16_t ABT_DONE           : 1;         ///<BIT [2] Abort Done.
        uint16_t RF200D014_4_3_RSVD : 2;         ///<BIT [4:3] Reserved.
        uint16_t DES_INV            : 1;         ///<BIT [5] Descriptor Invalid.
        uint16_t DNUM_OVER          : 1;         ///<BIT [6] Descriptor Maximum Overflow.
        uint16_t TNUM_OVER          : 1;         ///<BIT [7] Track Maximum Overflow.
        uint16_t ADEC_OVER          : 1;         ///<BIT [8] Decoder Done Count Overflow.
        uint16_t AENC_OVER          : 1;         ///<BIT [9] Encoder Done Count Overflow.
        uint16_t MQP3_FULL          : 1;         ///<BIT [10] MQ Port 3 Full.
        uint16_t MQP2_FULL          : 1;         ///<BIT [11] MQ Port 2 Full.
        uint16_t MQP1_FULL          : 1;         ///<BIT [12] MQ Port 1 Full.
        uint16_t MQP0_FULL          : 1;         ///<BIT [13] MQ Port 0 Full.
        uint16_t SAC_R_ERR          : 1;         ///<BIT [14] Sequencer AXI Read Error.
        uint16_t SAC_W_ERR          : 1;         ///<BIT [15] Sequencer AXI Write Error.
    } b;    
} SeqIntrStat_t;

///  @brief 0xF200D016 : Sequencer Interrupt Mask
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TRK_DONE_M         : 1;         ///<BIT [0] Track Done Mask.
        uint16_t TRK_ERR_M          : 1;         ///<BIT [1] Track Error Mask.
        uint16_t ABT_DONE_M         : 1;         ///<BIT [2] Abort Done Mask.
        uint16_t RF200D016_4_3_RSVD : 2;         ///<BIT [4:3] Reserved.
        uint16_t DES_INV_M          : 1;         ///<BIT [5] Descriptor Invalid Mask.
        uint16_t DNUM_OVER_M        : 1;         ///<BIT [6] Descriptor Maximum Overflow Mask.
        uint16_t TNUM_OVER_M        : 1;         ///<BIT [7] Track Maximum Overflow Mask.
        uint16_t ADEC_OVER_M        : 1;         ///<BIT [8] Decoder Done Count Overflow Mask.
        uint16_t AENC_OVER_M        : 1;         ///<BIT [9] Encoder Done Count Overflow Mask.
        uint16_t MQP3_FULL_M        : 1;         ///<BIT [10] MQ Port 3 Full Mask.
        uint16_t MQP2_FULL_M        : 1;         ///<BIT [11] MQ Port 2 Full Mask.
        uint16_t MQP1_FULL_M        : 1;         ///<BIT [12] MQ Port 1 Full Mask.
        uint16_t MQP0_FULL_M        : 1;         ///<BIT [13] MQ Port 0 Full Mask.
        uint16_t SAC_R_ERR_M        : 1;         ///<BIT [14] Sequencer AXI Read Error Mask.
        uint16_t SAC_W_ERR_M        : 1;         ///<BIT [15] Sequencer AXI Write Error Mask.
    } b;    
} SeqIntrMsk_t;

///  @brief 0xF200D018 : Track Status ID
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TRK_ID               :11;         ///<BIT [10:0] Track Status ID.
        uint16_t RF200D018_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
    } b;    
} TrkStatId_t;

///  @brief 0xF200D01E : Track Pointer
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DS_PTR            : 8;         ///<BIT [7:0] Track Descriptor Status Pointer.
        uint16_t DD_PTR            : 7;         ///<BIT [14:8] Track Descriptor Done Pointer.
        uint16_t RF200D01E_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} TrkPtr_t;

///  @brief 0xF200D020 : Track Overall Status 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TRK_POS              : 1;         ///<BIT [0] Track NAND Flash Overall Status.
        uint32_t TRK_LOS              : 1;         ///<BIT [1] Track ECU Overall Status.
        uint32_t DES_PROC             :11;         ///<BIT [12:2] Processed Descriptor Number.
        uint32_t RF200D020_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t CANCEL_MARK          : 1;         ///<BIT [16] Track Cancel Mark.
        uint32_t RF200D020_17_RSVD    : 1;         ///<BIT [17] Reserved.
        uint32_t DES_TOT              :11;         ///<BIT [28:18] Total Descriptor Number.
        uint32_t RF200D020_31_29_RSVD : 3;         ///<BIT [31:29] Reserved.
    } b;    
} TrkOverallStat1_t;

///  @brief 0xF200D024 : Track Overall Status 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ERR_AU_NUM           : 3;         ///<BIT [2:0] Failed AU Number.
        uint32_t ERR_MP_NUM           : 3;         ///<BIT [5:3] Failed Multi-Plane Number.
        uint32_t RF200D024_15_6_RSVD  :10;         ///<BIT [15:6] Reserved.
        uint32_t ERR_AU_DID           :10;         ///<BIT [25:16] Failed AU Descriptor ID.
        uint32_t RF200D024_30_26_RSVD : 5;         ///<BIT [30:26] Reserved.
        uint32_t ERR_D0E1             : 1;         ///<BIT [31] Failed AU Type.
    } b;    
} TrkOverallStat2_t;

///  @brief 0xF200D1D0 : Physical Channel Mapping for Channel 0 to 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PHY_MAP_CH0 : 4;         ///<BIT [3:0] Physical Channel Mapping for Channel 0.
        uint16_t PHY_MAP_CH1 : 4;         ///<BIT [7:4] Physical Channel Mapping for Channel 1.
        uint16_t PHY_MAP_CH2 : 4;         ///<BIT [11:8] Physical Channel Mapping for Channel 2.
        uint16_t PHY_MAP_CH3 : 4;         ///<BIT [15:12] Physical Channel Mapping for Channel 3.
    } b;    
} PhysicalChanMappingForChan0To3_t;

///  @brief 0xF200D1D2 : Physical Channel Mapping for Channel 4 to 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PHY_MAP_CH4 : 4;         ///<BIT [3:0] Physical Channel Mapping for Channel 4.
        uint16_t PHY_MAP_CH5 : 4;         ///<BIT [7:4] Physical Channel Mapping for Channel 5.
        uint16_t PHY_MAP_CH6 : 4;         ///<BIT [11:8] Physical Channel Mapping for Channel 6.
        uint16_t PHY_MAP_CH7 : 4;         ///<BIT [15:12] Physical Channel Mapping for Channel 7.
    } b;    
} PhysicalChanMappingForChan4To7_t;

#if (_SOC_REV_ < 0x0210)
///  @brief 0xF200D1D4 : Physical Channel Mapping for Channel 8 to 11
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PHY_MAP_CH8  : 4;         ///<BIT [3:0] Physical Channel Mapping for Channel 8.
        uint16_t PHY_MAP_CH9  : 4;         ///<BIT [7:4] Physical Channel Mapping for Channel 9.
        uint16_t PHY_MAP_CH10 : 4;         ///<BIT [11:8] Physical Channel Mapping for Channel 10.
        uint16_t PHY_MAP_CH11 : 4;         ///<BIT [15:12] Physical Channel Mapping for Channel 11.
    } b;    
} PhysicalChanMappingForChan8To11_t;

///  @brief 0xF200D1D6 : Physical Channel Mapping for Channel 12 to 15
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PHY_MAP_CH12 : 4;         ///<BIT [3:0] Physical Channel Mapping for Channel 12.
        uint16_t PHY_MAP_CH13 : 4;         ///<BIT [7:4] Physical Channel Mapping for Channel 13.
        uint16_t PHY_MAP_CH14 : 4;         ///<BIT [11:8] Physical Channel Mapping for Channel 14.
        uint16_t PHY_MAP_CH15 : 4;         ///<BIT [15:12] Physical Channel Mapping for Channel 15.
    } b;    
} PhysicalChanMappingForChan12To15_t;
#endif

///  @brief 0xF200D200 : Track Cancellation Control 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TRK_CCL_SET          :11;         ///<BIT [10:0] Track Cancel List Set.
        uint16_t RF200D200_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
    } b;    
} TrkCancellaCtrl1_t;

///  @brief 0xF200D202 : Track Cancellation Control 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TRK_CCL_ID           :11;         ///<BIT [10:0] Track Cancel ID.
        uint16_t TRK_CCL_CLEAR        : 1;         ///<BIT [11] Track Cancel List Clear.
        uint16_t TRK_CCL_CLEAR_SNG    : 1;         ///<BIT [12] Track Cancel List Single Clear.
        uint16_t RF200D202_14_13_RSVD : 2;         ///<BIT [14:13] Reserved.
        uint16_t TRK_CCL_STATUS       : 1;         ///<BIT [15] Track Cancel List Status.
    } b;    
} TrkCancellaCtrl2_t;

///  @brief 0xF200D204 : Track Cancellation Control 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200D204_3_0_RSVD   : 4;         ///<BIT [3:0] Reserved.
        uint16_t TRK_LOS_CID          : 8;         ///<BIT [11:4] Track LOS Clear.
        uint16_t RF200D204_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} TrkCancellaCtrl3_t;

///  @brief 0xF200D210 : Histogram Group 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HISTO_COLLECT_MODE    : 2;         ///<BIT [1:0] Mode for Histogram Collection.
        uint16_t HISTO_COLLECT_CH_ID   : 4;         ///<BIT [5:2] Channel ID for Histogram Collection.
        uint16_t HISTO_COLLECT_DEV_ID  : 4;         ///<BIT [9:6] Device ID for Histogram Collection.
        uint16_t HISTO_COLLECT_DIE_ID  : 2;         ///<BIT [11:10] Die ID for Histogram Collection.
#if (_SOC_REV_ >= 0x0210)
        uint16_t HISTO_COLLECT_ROW_SEL : 4;         ///<BIT [15:12] Row Select for Histogram Collection.
#else
        uint16_t RF200D210_15_12_RSVD  : 4;         ///<BIT [15:12] Reserved.
#endif
    } b;    
} HistograGroup1_t;

///  @brief 0xF200D212 : Histogram Group 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERR_HISTO_THR0       :12;         ///<BIT [11:0] Histogram Collection Threshold 0.
        uint16_t RF200D212_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} HistograGroup2_t;

///  @brief 0xF200D214 : Histogram Group 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERR_HISTO_THR1       :12;         ///<BIT [11:0] Histogram Collection Threshold 1.
        uint16_t RF200D214_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} HistograGroup3_t;

///  @brief 0xF200D216 : Histogram Group 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERR_HISTO_THR2       :12;         ///<BIT [11:0] Histogram Collection Threshold 2.
        uint16_t RF200D216_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} HistograGroup4_t;

///  @brief 0xF200D218 : Histogram Group 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERR_HISTO_THR3       :12;         ///<BIT [11:0] Histogram Collection Threshold 3.
        uint16_t RF200D218_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} HistograGroup5_t;

///  @brief 0xF200D21A : Histogram Group 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERR_HISTO_THR4       :12;         ///<BIT [11:0] Histogram Collection Threshold 4.
        uint16_t RF200D21A_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} HistograGroup6_t;

///  @brief 0xF200D21C : Histogram Group 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERR_HISTO_THR5       :12;         ///<BIT [11:0] Histogram Collection Threshold 5.
        uint16_t RF200D21C_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} HistograGroup7_t;

///  @brief 0xF200D21E : Histogram Group 8
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERR_HISTO_THR6       :12;         ///<BIT [11:0] Histogram Collection Threshold 6.
        uint16_t RF200D21E_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} HistograGroup8_t;

///  @brief 0xF200D220 : Histogram Group 9
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERR_HISTO_THR7       :12;         ///<BIT [11:0] Histogram Collection Threshold 7.
        uint16_t RF200D220_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} HistograGroup9_t;

///  @brief 0xF200D248 : Histogram Group 19
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERR_HISTO_CNT_CLR   : 9;         ///<BIT [8:0] Clears the Histogram Total Count.
        uint16_t RF200D248_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
    } b;    
} HistograGroup19_t;

///  @brief 0xF200D24A : Histogram Group 20
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERR_HISTO_CNT_EN    : 9;         ///<BIT [8:0] Enables Histogram Collection.
        uint16_t RF200D24A_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
    } b;    
} HistograGroup20_t;

///  @brief 0xF200D24C : Histogram Group 21
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERR_HISTO_FLAG      : 8;         ///<BIT [7:0] Error Flag Histogram.
        uint16_t RF200D24C_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} HistograGroup21_t;

///  @brief 0xF200D2C4 : Physical Region Page Available Count
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PRP_AV_CNT           :10;         ///<BIT [9:0] Physical Region Page Available Count.
        uint16_t RF200D2C4_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} PrpAvailableCnt_t;

///  @brief 0xF200D2C6 : Physical Region Page Available Count Adjust
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PRP_AV_CNT_ADJ       :10;         ///<BIT [9:0] Physical Region Page Available Count Adjust.
        uint16_t RF200D2C6_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} PrpAvailableCntAdj_t;

#if (_SOC_REV_ >= 0x0210)
///  @brief 0xF200D2C8 : Minimum Physical Region Page Threshold
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t MIN_PRP_THRSHD       :10;         ///<BIT [9:0] Minimum Physical Region Page Threshold.
        uint16_t RF200D2C8_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} MinPhysicalRegisterionPageThr_t;
#endif

///  @brief 0xF200D404 : Sequencer LUT Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200D404_2_0_RSVD  : 3;         ///<BIT [2:0] Reserved.
        uint16_t RF200D404_14_3_RSVD :12;         ///<BIT [14:3] Reserved.
        uint16_t SAC_R_ERR_RSP_H     : 1;         ///<BIT [15] Sequencer AXI Read Error Response (MSB).
    } b;    
} SeqLutCfg_t;

///  @brief 0xF200D406 : Sequencer AXI Error Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200D406_3_0_RSVD  : 4;         ///<BIT [3:0] Reserved.
        uint16_t SAC_R_ERR_RSP_L     : 2;         ///<BIT [5:4] Sequencer AXI Read Error Response (LSB).
        uint16_t SAC_R_CHK_DIS       : 1;         ///<BIT [6] Sequencer AXI Read Check Disable.
        uint16_t SAC_R_ERR_RSM       : 1;         ///<BIT [7] Sequencer AXI Read Error Resume.
        uint16_t RF200D406_11_8_RSVD : 4;         ///<BIT [11:8] Reserved.
        uint16_t SAC_W_ERR_RSP       : 2;         ///<BIT [13:12] Sequencer AXI Write Error Response.
        uint16_t SAC_W_CHK_DIS       : 1;         ///<BIT [14] Sequencer AXI Write Check Disable.
        uint16_t SAC_W_ERR_RSM       : 1;         ///<BIT [15] Sequencer AXI Write Error Resume.
    } b;    
} SeqAxiErrCtrl_t;


/// @brief 0xF200D000-0xF200D500 : Seq_t MODULE
typedef struct
{
    SeqCfg1_t seqCfg1;                                                                 ///< 0xF200D000 : Sequencer Configuration 1
    SeqCfg2_t seqCfg2;                                                                 ///< 0xF200D002 : Sequencer Configuration 2
    SeqCfg3_t seqCfg3;                                                                 ///< 0xF200D004 : Sequencer Configuration 3
    uint16_t MAX_ACT_TRK;                                                              ///< 0xF200D006 : Sequencer Configuration 4
    uint8_t rsvd_RF200D008[4];                                                         ///< 0xF200D008 - 0xF200D00B : Reserved.
    uint16_t MAX_ACT_DES;                                                              ///< 0xF200D00C : Sequencer Configuration 6
    uint8_t rsvd_RF200D00E[2];                                                         ///< 0xF200D00E - 0xF200D00F : Reserved.
    SeqCtrl1_t seqCtrl1;                                                               ///< 0xF200D010 : Sequencer Control 1
    uint8_t rsvd_RF200D012[2];                                                         ///< 0xF200D012 - 0xF200D013 : Reserved.
    SeqIntrStat_t seqIntrStat;                                                         ///< 0xF200D014 : Sequencer Interrupt Status
    SeqIntrMsk_t seqIntrMsk;                                                           ///< 0xF200D016 : Sequencer Interrupt Mask
    TrkStatId_t trkStatId;                                                             ///< 0xF200D018 : Track Status ID
    uint8_t rsvd_RF200D01A[4];                                                         ///< 0xF200D01A - 0xF200D01D : Reserved.
    TrkPtr_t trkPtr;                                                                   ///< 0xF200D01E : Track Pointer
    TrkOverallStat1_t trkOverallStat1;                                                 ///< 0xF200D020 : Track Overall Status 1
    TrkOverallStat2_t trkOverallStat2;                                                 ///< 0xF200D024 : Track Overall Status 2
    uint32_t TRK_DD;                                                                   ///< 0xF200D028 : Track Descriptor Done
    uint32_t TRK_DS;                                                                   ///< 0xF200D02C : Track Descriptor Status
    uint32_t DF_START;                                                                 ///< 0xF200D030 : Descriptor FIFO Start Address
    uint32_t DF_END;                                                                   ///< 0xF200D034 : Descriptor FIFO End Address
    uint32_t DF_PTR;                                                                   ///< 0xF200D038 : Descriptor FIFO Pointer
    uint8_t rsvd_RF200D03C[8];                                                         ///< 0xF200D03C - 0xF200D043 : Reserved.
    uint16_t DF_CNT;                                                                   ///< 0xF200D044 : Descriptor FIFO Count
    uint16_t D_DONE_CNT;                                                               ///< 0xF200D046 : Descriptor Done Count
    uint16_t FC_SET_CNT;                                                               ///< 0xF200D048 : NAND Flash Interface Driver Set Descriptor Count
    uint8_t rsvd_RF200D04A[6];                                                         ///< 0xF200D04A - 0xF200D04F : Reserved.
    uint32_t AIF_START;                                                                ///< 0xF200D050 : Auxiliary Insert FIFO Start Address
    uint32_t AIF_END;                                                                  ///< 0xF200D054 : Auxiliary Insert FIFO End Address
    uint32_t AIF_PTR;                                                                  ///< 0xF200D058 : Auxiliary Insert FIFO Pointer
    uint8_t rsvd_RF200D05C[4];                                                         ///< 0xF200D05C - 0xF200D05F : Reserved.
    uint32_t ACF_START;                                                                ///< 0xF200D060 : Auxiliary Compare FIFO Start Address
    uint32_t ACF_END;                                                                  ///< 0xF200D064 : Auxiliary Compare FIFO End Address
    uint32_t ACF_PTR;                                                                  ///< 0xF200D068 : Auxiliary Compare FIFO Pointer
    uint8_t rsvd_RF200D06C[4];                                                         ///< 0xF200D06C - 0xF200D06F : Reserved.
    uint32_t AEF_START;                                                                ///< 0xF200D070 : Auxiliary Extract FIFO Start Address
    uint32_t AEF_END;                                                                  ///< 0xF200D074 : Auxiliary Extract FIFO End Address
    uint32_t AEF_PTR;                                                                  ///< 0xF200D078 : Auxiliary Extract FIFO Pointer
    uint8_t rsvd_RF200D07C[4];                                                         ///< 0xF200D07C - 0xF200D07F : Reserved.
    uint32_t HCF_START;                                                                ///< 0xF200D080 : HLBA Compare FIFO Start Address
    uint32_t HCF_END;                                                                  ///< 0xF200D084 : HLBA Compare FIFO End Address
    uint32_t HCF_PTR;                                                                  ///< 0xF200D088 : HLBA Compare FIFO Pointer
    uint8_t rsvd_RF200D08C[4];                                                         ///< 0xF200D08C - 0xF200D08F : Reserved.
    uint16_t AENC_DONE_CNT;                                                            ///< 0xF200D090 : AU Encoder Done Count
    uint16_t ADEC_DONE_CNT;                                                            ///< 0xF200D092 : AU Decoder Done Count
    uint16_t AENC_START_CNT;                                                           ///< 0xF200D094 : AU Encoder Start Count
    uint16_t ADEC_START_CNT;                                                           ///< 0xF200D096 : AU Decoder Start Count
#if (_SOC_REV_ >= 0x0210)
    uint8_t rsvd_RF200D098[2];                                                         ///< 0xF200D098 - 0xF200D099 : Reserved.
#else
    uint16_t AENC_RTR_START_CNT;                                                       ///< 0xF200D098 : AU Encoder Retry Start Count
#endif
    uint16_t ADEC_RTR_START_CNT;                                                       ///< 0xF200D09A : AU Decoder Retry Start Count
    uint16_t AENC_RCV_CNT;                                                             ///< 0xF200D09C : AU Encoder Receive Count
    uint16_t ADEC_RCV_CNT;                                                             ///< 0xF200D09E : AU Decoder Receive Count
    uint32_t ECF_START;                                                                ///< 0xF200D0A0 : Error Count FIFO Start Address
    uint32_t ECF_END;                                                                  ///< 0xF200D0A4 : Error Count FIFO End Address
    uint32_t ECF_PTR;                                                                  ///< 0xF200D0A8 : Error Count FIFO Pointer
    uint8_t rsvd_RF200D0AC[4];                                                         ///< 0xF200D0AC - 0xF200D0AF : Reserved.
    uint32_t ROW_INC;                                                                  ///< 0xF200D0B0 : Row Increment
    uint16_t CF_XFCNT_S0;                                                              ///< 0xF200D0B4 : Compact Descriptor Format Transfer Count Select 0
    uint16_t CF_XFCNT_S1;                                                              ///< 0xF200D0B6 : Compact Descriptor Format Transfer Count Select 1
    uint16_t CF_COL_S0;                                                                ///< 0xF200D0B8 : Compact Descriptor Format Column Address Select 0
    uint16_t CF_COL_S1;                                                                ///< 0xF200D0BA : Compact Descriptor Format Column Address Select 1
    uint8_t rsvd_RF200D0BC[20];                                                        ///< 0xF200D0BC - 0xF200D0CF : Reserved.
    uint32_t REF_MASK_1;                                                               ///< 0xF200D0D0 : Reference Auxiliary Data Mask 1
    uint32_t REF_MASK_2;                                                               ///< 0xF200D0D4 : Reference Auxiliary Data Mask 2
    uint32_t REF_MASK_3;                                                               ///< 0xF200D0D8 : Reference Auxiliary Data Mask 3
    uint8_t rsvd_RF200D0DC[68];                                                        ///< 0xF200D0DC - 0xF200D11F : Reserved.
    uint32_t AENC_START;                                                               ///< 0xF200D120 : AU Encoder Command FIFO Start Address
    uint16_t AENC_SIZE;                                                                ///< 0xF200D124 : AU Encoder Command FIFO Size
    uint8_t rsvd_RF200D126[26];                                                        ///< 0xF200D126 - 0xF200D13F : Reserved.
    uint32_t ADEC_START;                                                               ///< 0xF200D140 : AU Decoder Command FIFO Start Address
    uint16_t ADEC_SIZE;                                                                ///< 0xF200D144 : AU Decoder Command FIFO Size
    uint8_t rsvd_RF200D146[26];                                                        ///< 0xF200D146 - 0xF200D15F : Reserved.
    uint32_t MQP0_START;                                                               ///< 0xF200D160 : MQ Port 0 FIFO Start Address
    uint16_t MQP0_SIZE;                                                                ///< 0xF200D164 : MQ Port 0 FIFO Size
    uint8_t rsvd_RF200D166[10];                                                        ///< 0xF200D166 - 0xF200D16F : Reserved.
    uint32_t MQP0_PUSH_PTR;                                                            ///< 0xF200D170 : MQ Port 0 FIFO Push Pointer
    uint8_t rsvd_RF200D174[12];                                                        ///< 0xF200D174 - 0xF200D17F : Reserved.
    uint32_t MQP1_START;                                                               ///< 0xF200D180 : MQ Port 1 FIFO Start Address
    uint16_t MQP1_SIZE;                                                                ///< 0xF200D184 : MQ Port 1 FIFO Size
    uint8_t rsvd_RF200D186[10];                                                        ///< 0xF200D186 - 0xF200D18F : Reserved.
    uint32_t MQP1_PUSH_PTR;                                                            ///< 0xF200D190 : MQ Port 1 FIFO Push Pointer
    uint8_t rsvd_RF200D194[12];                                                        ///< 0xF200D194 - 0xF200D19F : Reserved.
    uint32_t TRKM_START;                                                               ///< 0xF200D1A0 : Track Memory Start Address
    uint8_t rsvd_RF200D1A4[12];                                              ///< 0xF200D1A4 - 0xF200D1AF : Reserved.
    uint32_t hif_start;                                                      ///< 0xF200D1B0 : HLBA Insert FIFO Start Address
    uint32_t hif_end;                                                        ///< 0xF200D1B4 : HLBA Insert FIFO End Address
    uint32_t hif_ptr;                                                        ///< 0xF200D1B8 : HLBA Insert FIFO Pointer
    uint8_t rsvd_RF200D1BC[4];                                               ///< 0xF200D1BC - 0xF200D1BF : Reserved.
    uint32_t GETP_START;                                                               ///< 0xF200D1C0 : NAND Parameter FIFO Start Address
    uint32_t GETP_END;                                                                 ///< 0xF200D1C4 : NAND Parameter FIFO End Address
    uint32_t GETP_PTR;                                                                 ///< 0xF200D1C8 : NAND Parameter FIFO Pointer
    uint8_t rsvd_RF200D1CC[4];                                                         ///< 0xF200D1CC - 0xF200D1CF : Reserved.
    PhysicalChanMappingForChan0To3_t physicalChanMappingForChan0To3;                   ///< 0xF200D1D0 : Physical Channel Mapping for Channel 0 to 3
    PhysicalChanMappingForChan4To7_t physicalChanMappingForChan4To7;                   ///< 0xF200D1D2 : Physical Channel Mapping for Channel 4 to 7
#if (_SOC_REV_ >= 0x0210)
    uint8_t rsvd_RF200D1D4[12];                                                        ///< 0xF200D1D4 - 0xF200D1DF : Reserved.
#else
    PhysicalChanMappingForChan8To11_t physicalChanMappingForChan8To11;                 ///< 0xF200D1D4 : Physical Channel Mapping for Channel 8 to 11
    PhysicalChanMappingForChan12To15_t physicalChanMappingForChan12To15;               ///< 0xF200D1D6 : Physical Channel Mapping for Channel 12 to 15
    uint8_t rsvd_RF200D1D8[8];                                                         ///< 0xF200D1D8 - 0xF200D1DF : Reserved.
#endif
    uint32_t BTMF_START;                                                               ///< 0xF200D1E0 : NAND Busy Time Monitor FIFO Start Address
    uint32_t BTMF_END;                                                                 ///< 0xF200D1E4 : NAND Busy Time Monitor FIFO End Address
    uint32_t BTMF_PTR;                                                                 ///< 0xF200D1E8 : NAND Busy Time Monitor FIFO Pointer
    uint8_t rsvd_RF200D1EC[20];                                                        ///< 0xF200D1EC - 0xF200D1FF : Reserved.
    TrkCancellaCtrl1_t trkCancellaCtrl1;                                               ///< 0xF200D200 : Track Cancellation Control 1
    TrkCancellaCtrl2_t trkCancellaCtrl2;                                               ///< 0xF200D202 : Track Cancellation Control 2
    TrkCancellaCtrl3_t trkCancellaCtrl3;                                               ///< 0xF200D204 : Track Cancellation Control 3
    uint8_t rsvd_RF200D206[10];                                                        ///< 0xF200D206 - 0xF200D20F : Reserved.
    HistograGroup1_t histograGroup1;                                                   ///< 0xF200D210 : Histogram Group 1
    HistograGroup2_t histograGroup2;                                                   ///< 0xF200D212 : Histogram Group 2
    HistograGroup3_t histograGroup3;                                                   ///< 0xF200D214 : Histogram Group 3
    HistograGroup4_t histograGroup4;                                                   ///< 0xF200D216 : Histogram Group 4
    HistograGroup5_t histograGroup5;                                                   ///< 0xF200D218 : Histogram Group 5
    HistograGroup6_t histograGroup6;                                                   ///< 0xF200D21A : Histogram Group 6
    HistograGroup7_t histograGroup7;                                                   ///< 0xF200D21C : Histogram Group 7
    HistograGroup8_t histograGroup8;                                                   ///< 0xF200D21E : Histogram Group 8
    HistograGroup9_t histograGroup9;                                                   ///< 0xF200D220 : Histogram Group 9
    uint8_t rsvd_RF200D222[2];                                                         ///< 0xF200D222 - 0xF200D223 : Reserved.
    uint32_t ERR_HISTO_CNT0;                                                           ///< 0xF200D224 : Histogram Group 10
    uint32_t ERR_HISTO_CNT1;                                                           ///< 0xF200D228 : Histogram Group 11
    uint32_t ERR_HISTO_CNT2;                                                           ///< 0xF200D22C : Histogram Group 12
    uint32_t ERR_HISTO_CNT3;                                                           ///< 0xF200D230 : Histogram Group 13
    uint32_t ERR_HISTO_CNT4;                                                           ///< 0xF200D234 : Histogram Group 14
    uint32_t ERR_HISTO_CNT5;                                                           ///< 0xF200D238 : Histogram Group 15
    uint32_t ERR_HISTO_CNT6;                                                           ///< 0xF200D23C : Histogram Group 16
    uint32_t ERR_HISTO_CNT7;                                                           ///< 0xF200D240 : Histogram Group 17
    uint32_t HISTO_COLLECT_TOT_AU_CNT;                                                 ///< 0xF200D244 : Histogram Group 18
    HistograGroup19_t histograGroup19;                                                 ///< 0xF200D248 : Histogram Group 19
    HistograGroup20_t histograGroup20;                                                 ///< 0xF200D24A : Histogram Group 20
    HistograGroup21_t histograGroup21;                                                 ///< 0xF200D24C : Histogram Group 21
    uint8_t rsvd_RF200D24E[2];                                                         ///< 0xF200D24E - 0xF200D24F : Reserved.
    uint32_t MQP2_START;                                                               ///< 0xF200D250 : MQ Port 2 FIFO Start Address
    uint16_t MQP2_SIZE;                                                                ///< 0xF200D254 : MQ Port 2 FIFO Size
    uint8_t rsvd_RF200D256[10];                                                        ///< 0xF200D256 - 0xF200D25F : Reserved.
    uint32_t MQP2_PUSH_PTR;                                                            ///< 0xF200D260 : MQ Port 2 FIFO Push Pointer
    uint8_t rsvd_RF200D264[12];                                                        ///< 0xF200D264 - 0xF200D26F : Reserved.
    uint32_t MQP3_START;                                                               ///< 0xF200D270 : MQ Port 3 FIFO Start Address
    uint16_t MQP3_SIZE;                                                                ///< 0xF200D274 : MQ Port 3 FIFO Size
    uint8_t rsvd_RF200D276[10];                                                        ///< 0xF200D276 - 0xF200D27F : Reserved.
    uint32_t MQP3_PUSH_PTR;                                                            ///< 0xF200D280 : MQ Port 3 FIFO Push Pointer
#if (_SOC_REV_ >= 0x0210)
    uint8_t rsvd_RF200D284[44];                                                        ///< 0xF200D284 - 0xF200D2AF : Reserved.
#else
    uint8_t rsvd_RF200D284[12];                                                        ///< 0xF200D284 - 0xF200D28F : Reserved.
    uint32_t COPY_FIFO_START;                                                          ///< 0xF200D290 : Copy FIFO Start Address Low/High
    uint32_t COPY_FIFO_END;                                                            ///< 0xF200D294 : Copy FIFO End Address Low/High
    uint32_t COPY_FIFO_POINTER;                                                        ///< 0xF200D298 : Copy FIFO Pointer Address Low/High
    uint8_t rsvd_RF200D29C[20];                                                        ///< 0xF200D29C - 0xF200D2AF : Reserved.
#endif
    uint32_t ADEC_HPRJ_START;                                                          ///< 0xF200D2B0 : Decode High Priority Job Start Address
    uint16_t ADEC_HPRJ_SIZE;                                                           ///< 0xF200D2B4 : Decode High Priority Job Size
    uint16_t ADEC_HPRJ_POP_CNT;                                                        ///< 0xF200D2B6 : Decode High Priority Job Pop Counter
    uint32_t ADEC_HPRJ_PUSH_PTR;                                                       ///< 0xF200D2B8 : Decode High Priority Job Push Pointer
    uint32_t ADEC_HPRJ_POP_PTR;                                                        ///< 0xF200D2BC : Decode High Priority Job Pop Pointer
    uint16_t HWLL_AVAU_CNT;                                                            ///< 0xF200D2C0 : Hardware linked List Available AU Count
    uint16_t HWLL_AVAU_CNT_ADJ;                                                        ///< 0xF200D2C2 : Hardware linked List Available AU Count Adjust
    PrpAvailableCnt_t prpAvailableCnt;                                       ///< 0xF200D2C4 : Physial Region Page Availiable Count
    PrpAvailableCntAdj_t prpAvailableCntAdj;                                 ///< 0xF200D2C6 : Physial Region Page Availiable Count Adjust
#if (_SOC_REV_ >= 0x0210)
    MinPhysicalRegisterionPageThr_t minPhysicalRegisterionPageThr;                     ///< 0xF200D2C8 : Minimum Physical Region Page Threshold
    uint8_t rsvd_RF200D2CA[310];                                                       ///< 0xF200D2CA - 0xF200D3FF : Reserved.
#else
    uint8_t rsvd_RF200D2C8[312];                                                       ///< 0xF200D2C8 - 0xF200D3FF : Reserved.
#endif
    uint32_t SEQ_LUT_SADDR;                                                            ///< 0xF200D400 : Sequencer LUT Start Address
    SeqLutCfg_t seqLutCfg;                                                             ///< 0xF200D404 : Sequencer LUT Configuration
    SeqAxiErrCtrl_t seqAxiErrCtrl;                                                     ///< 0xF200D406 : Sequencer AXI Error Control
    uint32_t SAC_R_ERR_ADDR;                                                           ///< 0xF200D408 : Sequencer AXI Read Address
    uint32_t SAC_W_ERR_ADDR;                                                           ///< 0xF200D40C : Sequencer AXI Write Address
    uint32_t SEQ_AENC_LUT_SADDR;                                                       ///< 0xF200D410 : Sequencer AU Encoder LUT Start Address
    uint8_t rsvd_RF200D414[4];                                                         ///< 0xF200D414 - 0xF200D417 : Reserved.
    uint32_t SEQ_ADEC_LUT_SADDR;                                                       ///< 0xF200D418 : Sequencer AU Decoder LUT Start Address
    uint8_t rsvd_RF200D41C[4];                                                         ///< 0xF200D41C - 0xF200D41F : Reserved.
    uint32_t FDF_START;                                                                ///< 0xF200D420 : Fast Descriptor FIFO Start Address
    uint32_t FDF_END;                                                                  ///< 0xF200D424 : Fast Descriptor FIFO End Address
    uint32_t FDF_PTR;                                                                  ///< 0xF200D428 : Fast Descriptor FIFO Pointer
    uint16_t FDF_CNT;                                                                  ///< 0xF200D42C : Fast Descriptor FIFO Count
    uint8_t rsvd_RF200D42E[2];                                                         ///< 0xF200D42E - 0xF200D42F : Reserved.
    uint32_t FAIF_START;                                                               ///< 0xF200D430 : Fast Auxiliary Insert FIFO Start Address
    uint32_t FAIF_END;                                                                 ///< 0xF200D434 : Fast Auxiliary Insert FIFO End Address
    uint32_t FAIF_PTR;                                                                 ///< 0xF200D438 : Fast Auxiliary Insert FIFO Pointer
    uint8_t rsvd_RF200D43C[4];                                                         ///< 0xF200D43C - 0xF200D43F : Reserved.
    uint32_t FACF_START;                                                               ///< 0xF200D440 : Fast Auxiliary Compare FIFO Start Address
    uint32_t FACF_END;                                                                 ///< 0xF200D444 : Fast Auxiliary Compare FIFO End Address
    uint32_t FACF_PTR;                                                                 ///< 0xF200D448 : Fast Auxiliary Compare FIFO Pointer
    uint8_t rsvd_RF200D44C[4];                                                         ///< 0xF200D44C - 0xF200D44F : Reserved.
    uint32_t FHCF_START;                                                               ///< 0xF200D450 : Fast HLBA Compare FIFO Start Address
    uint32_t FHCF_END;                                                                 ///< 0xF200D454 : Fast HLBA Compare FIFO End Address
    uint32_t FHCF_PTR;                                                                 ///< 0xF200D458 : Fast HLBA Compare FIFO Pointer
    uint8_t rsvd_RF200D45C[20];                                                        ///< 0xF200D45C - 0xF200D46F : Reserved.
    uint32_t CEF_START;                                                                ///< 0xF200D470 : Compression Encoder FIFO Start Address
    uint32_t CEF_END;                                                                  ///< 0xF200D474 : Compression Encoder FIFO End Address
    uint32_t CEF_PTR1;                                                                 ///< 0xF200D478 : Compression Encoder FIFO Pointer 1
    uint32_t CEF_PTR2;                                                                 ///< 0xF200D47C : Compression Encoder FIFO Pointer 2
    uint32_t FCEF_START;                                                               ///< 0xF200D480 : Fast Compression Encoder FIFO Start Address
    uint32_t FCEF_END;                                                                 ///< 0xF200D484 : Fast Compression Encoder FIFO End Address
    uint32_t FCEF_PTR1;                                                                ///< 0xF200D488 : Fast Compression Encoder FIFO Pointer 1
    uint32_t FCEF_PTR2;                                                                ///< 0xF200D48C : Fast Compression Encoder FIFO Pointer 2
    uint32_t CDF_START;                                                                ///< 0xF200D490 : Compression Decoder FIFO Start Address
    uint32_t CDF_END;                                                                  ///< 0xF200D494 : Compression Decoder FIFO End Address
    uint32_t CDF_PTR1;                                                                 ///< 0xF200D498 : Compression Decoder FIFO Pointer 1
    uint32_t CDF_PTR2;                                                                 ///< 0xF200D49C : Compression Decoder FIFO Pointer 2
    uint32_t FCDF_START;                                                               ///< 0xF200D4A0 : Fast Compression Decoder FIFO Start Address
    uint32_t FCDF_END;                                                                 ///< 0xF200D4A4 : Fast Compression Decoder FIFO End Address
    uint32_t FCDF_PTR1;                                                                ///< 0xF200D4A8 : Fast Compression Decoder FIFO Pointer 1
    uint32_t FCDF_PTR2;                                                                ///< 0xF200D4AC : Fast Compression Decoder FIFO Pointer 2
    uint32_t CXF_START;                                                                ///< 0xF200D4B0 : Compression Extract FIFO Start Address
    uint32_t CXF_END;                                                                  ///< 0xF200D4B4 : Compression Extract FIFO End Address
    uint32_t CXF_PTR;                                                                  ///< 0xF200D4B8 : Compression Extract FIFO Pointer
    uint8_t rsvd_RF200D4BC[4];                                                         ///< 0xF200D4BC - 0xF200D4BF : Reserved.
    uint32_t SRF_START;                                                      ///< 0xF200D4C0 : Slow Retry FIFO Start Address
    uint32_t SRF_END;                                                        ///< 0xF200D4C4 : Slow Retry FIFO End Address
    uint32_t SRF_PTR;                                                        ///< 0xF200D4C8 : Slow Retry FIFO Pointer
    uint8_t rsvd_RF200D4CC[4];                                               ///< 0xF200D4CC - 0xF200D4CF : Reserved.
    uint32_t FSRF_START;                                                     ///< 0xF200D4D0 : Fast Slow Retry FIFO Start Address
    uint32_t FSRF_END;                                                       ///< 0xF200D4D4 : Fast Slow Retry FIFO End Address
    uint32_t FSRF_PTR;                                                       ///< 0xF200D4D8 : Fast Slow Retry FIFO Pointer
} Seq_t;
COMPILE_ASSERT(offsetof(Seq_t,FSRF_PTR)==0xF200D4D8-0xF200D000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile Seq_t rSeq;                          ///< 0xF200D000-0xF200D500 : Seq_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
extern Seq_t mSeq;                          ///< 0xF200D000-0xF200D500 : Seq_t MODULE

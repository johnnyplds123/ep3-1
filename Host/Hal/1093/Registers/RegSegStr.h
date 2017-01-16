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
//! @brief Streaming Controller Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  Streaming Controller Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF0114800 : STC Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_MQ_INIT        : 1;         ///<BIT [0] Host MQ Initialization.
        uint16_t RF0114800_1_RSVD    : 1;         ///<BIT [1] Reserved.
        uint16_t FLASH_MQ_INIT       : 1;         ///<BIT [2] Flash MQ Initialization.
        uint16_t EXTEND_MQ_INIT      : 1;         ///<BIT [3] Extend MQ Initialization.
        uint16_t RF0114800_4_RSVD    : 1;         ///<BIT [4] Reserved.
        uint16_t RF0114800_5_RSVD    : 1;         ///<BIT [5] Reserved.
        uint16_t RF0114800_6_RSVD    : 1;         ///<BIT [6] Reserved.
        uint16_t RF0114800_7_RSVD    : 1;         ///<BIT [7] Reserved.
        uint16_t RF0114800_12_8_RSVD : 5;         ///<BIT [12:8] Reserved.
        uint16_t CK_GATE_DIS         : 1;         ///<BIT [13] Clock Gating Control Disable.
        uint16_t INT_MODE            : 1;         ///<BIT [14] STC Interrupt Mode.
        uint16_t STC_MODE            : 1;         ///<BIT [15] STC Mode Select.
    } b;    
} StcCtrl_t;

///  @brief 0xF0114802 : Streaming Port Initialization Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t INIT_SEGMENT_SELECT : 5;         ///<BIT [4:0] Initialization Segment Select.
        uint16_t RF0114802_11_5_RSVD : 7;         ///<BIT [11:5] Reserved.
        uint16_t STRM_PORT_SELECT    : 2;         ///<BIT [13:12] Initialization Streaming Port Select.
        uint16_t RF0114802_14_RSVD   : 1;         ///<BIT [14] Reserved.
        uint16_t EXEC_STRM_PORT_INIT : 1;         ///<BIT [15] Execute Streaming Port Initialization.
    } b;    
} StreaminPortInitialiCtrl_t;

///  @brief 0xF0114808 : Host Ratio
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_RATIO          : 8;         ///<BIT [7:0] Host Ratio.
        uint16_t RF0114808_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} HostRatio_t;

///  @brief 0xF011480A : SSC Match Detect Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_SSC_MATCH      : 2;         ///<BIT [1:0] Host SSC Match.
        uint16_t RF011480A_3_2_RSVD  : 2;         ///<BIT [3:2] Reserved.
        uint16_t FLSH_SSC_MATCH      : 2;         ///<BIT [5:4] Flash SSC Match.
        uint16_t EXTD_SSC_MATCH      : 2;         ///<BIT [7:6] Extend SSC Match.
        uint16_t RF011480A_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} SscMatchDetectCtrl_t;

///  @brief 0xF0114810 : MQ RAM Address
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t MQ_RAM_ADDR          :10;         ///<BIT [9:0] MQ RAM Address.
        uint16_t RF0114810_11_10_RSVD : 2;         ///<BIT [11:10] Reserved.
        uint16_t MQ_RAM_SELECT        : 2;         ///<BIT [13:12] MQ Port Select.
        uint16_t RF0114810_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} MqRamAddr_t;

///  @brief 0xF0114814 : MQ RAM Depth Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_MQ_DEPTH      : 4;         ///<BIT [3:0] Host MQ RAM Depth.
        uint16_t RF0114814_7_4_RSVD : 4;         ///<BIT [7:4] Reserved.
        uint16_t FLSH_MQ_DEPTH      : 4;         ///<BIT [11:8] Flash MQ RAM Depth.
        uint16_t EXTD_MQ_DEPTH      : 4;         ///<BIT [15:12] Extend MQ RAM Depth.
    } b;    
} MqRamDepthCtrl_t;

///  @brief 0xF011481C : Flash MQ RAM Reserved Size
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF011481C_7_0_RSVD  : 8;         ///<BIT [7:0] Flash MQ RAM Reserved Size.
        uint16_t RF011481C_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} FlashMqRamRsvdSize_t;

///  @brief 0xF011481E : Extend MQ RAM Reserved Size
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF011481E_7_0_RSVD  : 8;         ///<BIT [7:0] Extend MQ RAM Reserved Size.
        uint16_t RF011481E_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} ExtendMqRamRsvdSize_t;

///  @brief 0xF0114820 : Host Segment Status 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_SEGMENT0_FULL  : 1;         ///<BIT [0] Host Segment 0 Full.
        uint16_t HOST_SEGMENT1_FULL  : 1;         ///<BIT [1] Host Segment 1 Full.
        uint16_t HOST_SEGMENT2_FULL  : 1;         ///<BIT [2] Host Segment 2 Full.
        uint16_t HOST_SEGMENT3_FULL  : 1;         ///<BIT [3] Host Segment 3 Full.
        uint16_t HOST_SEGMENT4_FULL  : 1;         ///<BIT [4] Host Segment 4 Full.
        uint16_t HOST_SEGMENT5_FULL  : 1;         ///<BIT [5] Host Segment 5 Full.
        uint16_t HOST_SEGMENT6_FULL  : 1;         ///<BIT [6] Host Segment 6 Full.
        uint16_t HOST_SEGMENT7_FULL  : 1;         ///<BIT [7] Host Segment 7 Full.
        uint16_t HOST_SEGMENT8_FULL  : 1;         ///<BIT [8] Host Segment 8 Full.
        uint16_t HOST_SEGMENT9_FULL  : 1;         ///<BIT [9] Host Segment 9 Full.
        uint16_t HOST_SEGMENT10_FULL : 1;         ///<BIT [10] Host Segment 10 Full.
        uint16_t HOST_SEGMENT11_FULL : 1;         ///<BIT [11] Host Segment 11 Full.
        uint16_t HOST_SEGMENT12_FULL : 1;         ///<BIT [12] Host Segment 12 Full.
        uint16_t HOST_SEGMENT13_FULL : 1;         ///<BIT [13] Host Segment 13 Full.
        uint16_t HOST_SEGMENT14_FULL : 1;         ///<BIT [14] Host Segment 14 Full.
        uint16_t HOST_SEGMENT15_FULL : 1;         ///<BIT [15] Host Segment 15 Full.
    } b;    
} HostSgmStat0_t;

///  @brief 0xF0114822 : Host Segment Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_SEGMENT16_FULL : 1;         ///<BIT [0] Host Segment 16 Full.
        uint16_t HOST_SEGMENT17_FULL : 1;         ///<BIT [1] Host Segment 17 Full.
        uint16_t HOST_SEGMENT18_FULL : 1;         ///<BIT [2] Host Segment 18 Full.
        uint16_t HOST_SEGMENT19_FULL : 1;         ///<BIT [3] Host Segment 19 Full.
        uint16_t HOST_SEGMENT20_FULL : 1;         ///<BIT [4] Host Segment 20 Full.
        uint16_t HOST_SEGMENT21_FULL : 1;         ///<BIT [5] Host Segment 21 Full.
        uint16_t HOST_SEGMENT22_FULL : 1;         ///<BIT [6] Host Segment 22 Full.
        uint16_t HOST_SEGMENT23_FULL : 1;         ///<BIT [7] Host Segment 23 Full.
        uint16_t HOST_SEGMENT24_FULL : 1;         ///<BIT [8] Host Segment 24 Full.
        uint16_t HOST_SEGMENT25_FULL : 1;         ///<BIT [9] Host Segment 25 Full.
        uint16_t HOST_SEGMENT26_FULL : 1;         ///<BIT [10] Host Segment 26 Full.
        uint16_t HOST_SEGMENT27_FULL : 1;         ///<BIT [11] Host Segment 27 Full.
        uint16_t HOST_SEGMENT28_FULL : 1;         ///<BIT [12] Host Segment 28 Full.
        uint16_t HOST_SEGMENT29_FULL : 1;         ///<BIT [13] Host Segment 29 Full.
        uint16_t HOST_SEGMENT30_FULL : 1;         ///<BIT [14] Host Segment 30 Full.
        uint16_t HOST_SEGMENT31_FULL : 1;         ///<BIT [15] Host Segment 31 Full.
    } b;    
} HostSgmStat1_t;

///  @brief 0xF0114824 : Host Segment Status 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_SEGMENT0_EMPTY  : 1;         ///<BIT [0] Host Segment 0 Empty.
        uint16_t HOST_SEGMENT1_EMPTY  : 1;         ///<BIT [1] Host Segment 1 Empty.
        uint16_t HOST_SEGMENT2_EMPTY  : 1;         ///<BIT [2] Host Segment 2 Empty.
        uint16_t HOST_SEGMENT3_EMPTY  : 1;         ///<BIT [3] Host Segment 3 Empty.
        uint16_t HOST_SEGMENT4_EMPTY  : 1;         ///<BIT [4] Host Segment 4 Empty.
        uint16_t HOST_SEGMENT5_EMPTY  : 1;         ///<BIT [5] Host Segment 5 Empty.
        uint16_t HOST_SEGMENT6_EMPTY  : 1;         ///<BIT [6] Host Segment 6 Empty.
        uint16_t HOST_SEGMENT7_EMPTY  : 1;         ///<BIT [7] Host Segment 7 Empty.
        uint16_t HOST_SEGMENT8_EMPTY  : 1;         ///<BIT [8] Host Segment 8 Empty.
        uint16_t HOST_SEGMENT9_EMPTY  : 1;         ///<BIT [9] Host Segment 9 Empty.
        uint16_t HOST_SEGMENT10_EMPTY : 1;         ///<BIT [10] Host Segment 10 Empty.
        uint16_t HOST_SEGMENT11_EMPTY : 1;         ///<BIT [11] Host Segment 11 Empty.
        uint16_t HOST_SEGMENT12_EMPTY : 1;         ///<BIT [12] Host Segment 12 Empty.
        uint16_t HOST_SEGMENT13_EMPTY : 1;         ///<BIT [13] Host Segment 13 Empty.
        uint16_t HOST_SEGMENT14_EMPTY : 1;         ///<BIT [14] Host Segment 14 Empty.
        uint16_t HOST_SEGMENT15_EMPTY : 1;         ///<BIT [15] Host Segment 15 Empty.
    } b;    
} HostSgmStat2_t;

///  @brief 0xF0114826 : Host Segment Status 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_SEGMENT16_EMPTY : 1;         ///<BIT [0] Host Segment 16 Empty.
        uint16_t HOST_SEGMENT17_EMPTY : 1;         ///<BIT [1] Host Segment 17 Empty.
        uint16_t HOST_SEGMENT18_EMPTY : 1;         ///<BIT [2] Host Segment 18 Empty.
        uint16_t HOST_SEGMENT19_EMPTY : 1;         ///<BIT [3] Host Segment 19 Empty.
        uint16_t HOST_SEGMENT20_EMPTY : 1;         ///<BIT [4] Host Segment 20 Empty.
        uint16_t HOST_SEGMENT21_EMPTY : 1;         ///<BIT [5] Host Segment 21 Empty.
        uint16_t HOST_SEGMENT22_EMPTY : 1;         ///<BIT [6] Host Segment 22 Empty.
        uint16_t HOST_SEGMENT23_EMPTY : 1;         ///<BIT [7] Host Segment 23 Empty.
        uint16_t HOST_SEGMENT24_EMPTY : 1;         ///<BIT [8] Host Segment 24 Empty.
        uint16_t HOST_SEGMENT25_EMPTY : 1;         ///<BIT [9] Host Segment 25 Empty.
        uint16_t HOST_SEGMENT26_EMPTY : 1;         ///<BIT [10] Host Segment 26 Empty.
        uint16_t HOST_SEGMENT27_EMPTY : 1;         ///<BIT [11] Host Segment 27 Empty.
        uint16_t HOST_SEGMENT28_EMPTY : 1;         ///<BIT [12] Host Segment 28 Empty.
        uint16_t HOST_SEGMENT29_EMPTY : 1;         ///<BIT [13] Host Segment 29 Empty.
        uint16_t HOST_SEGMENT30_EMPTY : 1;         ///<BIT [14] Host Segment 30 Empty.
        uint16_t HOST_SEGMENT31_EMPTY : 1;         ///<BIT [15] Host Segment 31 Empty.
    } b;    
} HostSgmStat3_t;

///  @brief 0xF0114830 : Flash Segment Status 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FLASH_SEGMENT0_FULL  : 1;         ///<BIT [0] Flash Segment 0 Full.
        uint16_t FLASH_SEGMENT1_FULL  : 1;         ///<BIT [1] Flash Segment 1 Full.
        uint16_t FLASH_SEGMENT2_FULL  : 1;         ///<BIT [2] Flash Segment 2 Full.
        uint16_t FLASH_SEGMENT3_FULL  : 1;         ///<BIT [3] Flash Segment 3 Full.
        uint16_t FLASH_SEGMENT4_FULL  : 1;         ///<BIT [4] Flash Segment 4 Full.
        uint16_t FLASH_SEGMENT5_FULL  : 1;         ///<BIT [5] Flash Segment 5 Full.
        uint16_t FLASH_SEGMENT6_FULL  : 1;         ///<BIT [6] Flash Segment 6 Full.
        uint16_t FLASH_SEGMENT7_FULL  : 1;         ///<BIT [7] Flash Segment 7 Full.
        uint16_t FLASH_SEGMENT8_FULL  : 1;         ///<BIT [8] Flash Segment 8 Full.
        uint16_t FLASH_SEGMENT9_FULL  : 1;         ///<BIT [9] Flash Segment 9 Full.
        uint16_t FLASH_SEGMENT10_FULL : 1;         ///<BIT [10] Flash Segment 10 Full.
        uint16_t FLASH_SEGMENT11_FULL : 1;         ///<BIT [11] Flash Segment 11 Full.
        uint16_t FLASH_SEGMENT12_FULL : 1;         ///<BIT [12] Flash Segment 12 Full.
        uint16_t FLASH_SEGMENT13_FULL : 1;         ///<BIT [13] Flash Segment 13 Full.
        uint16_t FLASH_SEGMENT14_FULL : 1;         ///<BIT [14] Flash Segment 14 Full.
        uint16_t FLASH_SEGMENT15_FULL : 1;         ///<BIT [15] Flash Segment 15 Full.
    } b;    
} FlashSgmStat0_t;

///  @brief 0xF0114832 : Flash Segment Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FLASH_SEGMENT16_FULL : 1;         ///<BIT [0] Flash Segment 16 Full.
        uint16_t FLASH_SEGMENT17_FULL : 1;         ///<BIT [1] Flash Segment 17 Full.
        uint16_t FLASH_SEGMENT18_FULL : 1;         ///<BIT [2] Flash Segment 18 Full.
        uint16_t FLASH_SEGMENT19_FULL : 1;         ///<BIT [3] Flash Segment 19 Full.
        uint16_t FLASH_SEGMENT20_FULL : 1;         ///<BIT [4] Flash Segment 20 Full.
        uint16_t FLASH_SEGMENT21_FULL : 1;         ///<BIT [5] Flash Segment 21 Full.
        uint16_t FLASH_SEGMENT22_FULL : 1;         ///<BIT [6] Flash Segment 22 Full.
        uint16_t FLASH_SEGMENT23_FULL : 1;         ///<BIT [7] Flash Segment 23 Full.
        uint16_t FLASH_SEGMENT24_FULL : 1;         ///<BIT [8] Flash Segment 24 Full.
        uint16_t FLASH_SEGMENT25_FULL : 1;         ///<BIT [9] Flash Segment 25 Full.
        uint16_t FLASH_SEGMENT26_FULL : 1;         ///<BIT [10] Flash Segment 26 Full.
        uint16_t FLASH_SEGMENT27_FULL : 1;         ///<BIT [11] Flash Segment 27 Full.
        uint16_t FLASH_SEGMENT28_FULL : 1;         ///<BIT [12] Flash Segment 28 Full.
        uint16_t FLASH_SEGMENT29_FULL : 1;         ///<BIT [13] Flash Segment 29 Full.
        uint16_t FLASH_SEGMENT30_FULL : 1;         ///<BIT [14] Flash Segment 30 Full.
        uint16_t FLASH_SEGMENT31_FULL : 1;         ///<BIT [15] Flash Segment 31 Full.
    } b;    
} FlashSgmStat1_t;

///  @brief 0xF0114834 : Flash Segment Status 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FLASH_SEGMENT0_EMPTY  : 1;         ///<BIT [0] Flash Segment 0 Empty.
        uint16_t FLASH_SEGMENT1_EMPTY  : 1;         ///<BIT [1] Flash Segment 1 Empty.
        uint16_t FLASH_SEGMENT2_EMPTY  : 1;         ///<BIT [2] Flash Segment 2 Empty.
        uint16_t FLASH_SEGMENT3_EMPTY  : 1;         ///<BIT [3] Flash Segment 3 Empty.
        uint16_t FLASH_SEGMENT4_EMPTY  : 1;         ///<BIT [4] Flash Segment 4 Empty.
        uint16_t FLASH_SEGMENT5_EMPTY  : 1;         ///<BIT [5] Flash Segment 5 Empty.
        uint16_t FLASH_SEGMENT6_EMPTY  : 1;         ///<BIT [6] Flash Segment 6 Empty.
        uint16_t FLASH_SEGMENT7_EMPTY  : 1;         ///<BIT [7] Flash Segment 7 Empty.
        uint16_t FLASH_SEGMENT8_EMPTY  : 1;         ///<BIT [8] Flash Segment 8 Empty.
        uint16_t FLASH_SEGMENT9_EMPTY  : 1;         ///<BIT [9] Flash Segment 9 Empty.
        uint16_t FLASH_SEGMENT10_EMPTY : 1;         ///<BIT [10] Flash Segment 10 Empty.
        uint16_t FLASH_SEGMENT11_EMPTY : 1;         ///<BIT [11] Flash Segment 11 Empty.
        uint16_t FLASH_SEGMENT12_EMPTY : 1;         ///<BIT [12] Flash Segment 12 Empty.
        uint16_t FLASH_SEGMENT13_EMPTY : 1;         ///<BIT [13] Flash Segment 13 Empty.
        uint16_t FLASH_SEGMENT14_EMPTY : 1;         ///<BIT [14] Flash Segment 14 Empty.
        uint16_t FLASH_SEGMENT15_EMPTY : 1;         ///<BIT [15] Flash Segment 15 Empty.
    } b;    
} FlashSgmStat2_t;

///  @brief 0xF0114836 : Flash Segment Status 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FLASH_SEGMENT16_EMPTY : 1;         ///<BIT [0] Flash Segment 16 Empty.
        uint16_t FLASH_SEGMENT17_EMPTY : 1;         ///<BIT [1] Flash Segment 17 Empty.
        uint16_t FLASH_SEGMENT18_EMPTY : 1;         ///<BIT [2] Flash Segment 18 Empty.
        uint16_t FLASH_SEGMENT19_EMPTY : 1;         ///<BIT [3] Flash Segment 19 Empty.
        uint16_t FLASH_SEGMENT20_EMPTY : 1;         ///<BIT [4] Flash Segment 20 Empty.
        uint16_t FLASH_SEGMENT21_EMPTY : 1;         ///<BIT [5] Flash Segment 21 Empty.
        uint16_t FLASH_SEGMENT22_EMPTY : 1;         ///<BIT [6] Flash Segment 22 Empty.
        uint16_t FLASH_SEGMENT23_EMPTY : 1;         ///<BIT [7] Flash Segment 23 Empty.
        uint16_t FLASH_SEGMENT24_EMPTY : 1;         ///<BIT [8] Flash Segment 24 Empty.
        uint16_t FLASH_SEGMENT25_EMPTY : 1;         ///<BIT [9] Flash Segment 25 Empty.
        uint16_t FLASH_SEGMENT26_EMPTY : 1;         ///<BIT [10] Flash Segment 26 Empty.
        uint16_t FLASH_SEGMENT27_EMPTY : 1;         ///<BIT [11] Flash Segment 27 Empty.
        uint16_t FLASH_SEGMENT28_EMPTY : 1;         ///<BIT [12] Flash Segment 28 Empty.
        uint16_t FLASH_SEGMENT29_EMPTY : 1;         ///<BIT [13] Flash Segment 29 Empty.
        uint16_t FLASH_SEGMENT30_EMPTY : 1;         ///<BIT [14] Flash Segment 30 Empty.
        uint16_t FLASH_SEGMENT31_EMPTY : 1;         ///<BIT [15] Flash Segment 31 Empty.
    } b;    
} FlashSgmStat3_t;

///  @brief 0xF0114838 : Extend Segment Status 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXTEND_SEGMENT0_FULL  : 1;         ///<BIT [0] Extend Segment 0 Full.
        uint16_t EXTEND_SEGMENT1_FULL  : 1;         ///<BIT [1] Extend Segment 1 Full.
        uint16_t EXTEND_SEGMENT2_FULL  : 1;         ///<BIT [2] Extend Segment 2 Full.
        uint16_t EXTEND_SEGMENT3_FULL  : 1;         ///<BIT [3] Extend Segment 3 Full.
        uint16_t EXTEND_SEGMENT4_FULL  : 1;         ///<BIT [4] Extend Segment 4 Full.
        uint16_t EXTEND_SEGMENT5_FULL  : 1;         ///<BIT [5] Extend Segment 5 Full.
        uint16_t EXTEND_SEGMENT6_FULL  : 1;         ///<BIT [6] Extend Segment 6 Full.
        uint16_t EXTEND_SEGMENT7_FULL  : 1;         ///<BIT [7] Extend Segment 7 Full.
        uint16_t EXTEND_SEGMENT8_FULL  : 1;         ///<BIT [8] Extend Segment 8 Full.
        uint16_t EXTEND_SEGMENT9_FULL  : 1;         ///<BIT [9] Extend Segment 9 Full.
        uint16_t EXTEND_SEGMENT10_FULL : 1;         ///<BIT [10] Extend Segment 10 Full.
        uint16_t EXTEND_SEGMENT11_FULL : 1;         ///<BIT [11] Extend Segment 11 Full.
        uint16_t EXTEND_SEGMENT12_FULL : 1;         ///<BIT [12] Extend Segment 12 Full.
        uint16_t EXTEND_SEGMENT13_FULL : 1;         ///<BIT [13] Extend Segment 13 Full.
        uint16_t EXTEND_SEGMENT14_FULL : 1;         ///<BIT [14] Extend Segment 14 Full.
        uint16_t EXTEND_SEGMENT15_FULL : 1;         ///<BIT [15] Extend Segment 15 Full.
    } b;    
} ExtendSgmStat0_t;

///  @brief 0xF011483A : Extend Segment Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXTEND_SEGMENT16_FULL : 1;         ///<BIT [0] Extend Segment 16 Full.
        uint16_t EXTEND_SEGMENT17_FULL : 1;         ///<BIT [1] Extend Segment 17 Full.
        uint16_t EXTEND_SEGMENT18_FULL : 1;         ///<BIT [2] Extend Segment 18 Full.
        uint16_t EXTEND_SEGMENT19_FULL : 1;         ///<BIT [3] Extend Segment 19 Full.
        uint16_t EXTEND_SEGMENT20_FULL : 1;         ///<BIT [4] Extend Segment 20 Full.
        uint16_t EXTEND_SEGMENT21_FULL : 1;         ///<BIT [5] Extend Segment 21 Full.
        uint16_t EXTEND_SEGMENT22_FULL : 1;         ///<BIT [6] Extend Segment 22 Full.
        uint16_t EXTEND_SEGMENT23_FULL : 1;         ///<BIT [7] Extend Segment 23 Full.
        uint16_t EXTEND_SEGMENT24_FULL : 1;         ///<BIT [8] Extend Segment 24 Full.
        uint16_t EXTEND_SEGMENT25_FULL : 1;         ///<BIT [9] Extend Segment 25 Full.
        uint16_t EXTEND_SEGMENT26_FULL : 1;         ///<BIT [10] Extend Segment 26 Full.
        uint16_t EXTEND_SEGMENT27_FULL : 1;         ///<BIT [11] Extend Segment 27 Full.
        uint16_t EXTEND_SEGMENT28_FULL : 1;         ///<BIT [12] Extend Segment 28 Full.
        uint16_t EXTEND_SEGMENT29_FULL : 1;         ///<BIT [13] Extend Segment 29 Full.
        uint16_t EXTEND_SEGMENT30_FULL : 1;         ///<BIT [14] Extend Segment 30 Full.
        uint16_t EXTEND_SEGMENT31_FULL : 1;         ///<BIT [15] Extend Segment 31 Full.
    } b;    
} ExtendSgmStat1_t;

///  @brief 0xF011483C : Extend Segment Status 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXTEND_SEGMENT0_EMPTY  : 1;         ///<BIT [0] Extend Segment 0 Empty.
        uint16_t EXTEND_SEGMENT1_EMPTY  : 1;         ///<BIT [1] Extend Segment 1 Empty.
        uint16_t EXTEND_SEGMENT2_EMPTY  : 1;         ///<BIT [2] Extend Segment 2 Empty.
        uint16_t EXTEND_SEGMENT3_EMPTY  : 1;         ///<BIT [3] Extend Segment 3 Empty.
        uint16_t EXTEND_SEGMENT4_EMPTY  : 1;         ///<BIT [4] Extend Segment 4 Empty.
        uint16_t EXTEND_SEGMENT5_EMPTY  : 1;         ///<BIT [5] Extend Segment 5 Empty.
        uint16_t EXTEND_SEGMENT6_EMPTY  : 1;         ///<BIT [6] Extend Segment 6 Empty.
        uint16_t EXTEND_SEGMENT7_EMPTY  : 1;         ///<BIT [7] Extend Segment 7 Empty.
        uint16_t EXTEND_SEGMENT8_EMPTY  : 1;         ///<BIT [8] Extend Segment 8 Empty.
        uint16_t EXTEND_SEGMENT9_EMPTY  : 1;         ///<BIT [9] Extend Segment 9 Empty.
        uint16_t EXTEND_SEGMENT10_EMPTY : 1;         ///<BIT [10] Extend Segment 10 Empty.
        uint16_t EXTEND_SEGMENT11_EMPTY : 1;         ///<BIT [11] Extend Segment 11 Empty.
        uint16_t EXTEND_SEGMENT12_EMPTY : 1;         ///<BIT [12] Extend Segment 12 Empty.
        uint16_t EXTEND_SEGMENT13_EMPTY : 1;         ///<BIT [13] Extend Segment 13 Empty.
        uint16_t EXTEND_SEGMENT14_EMPTY : 1;         ///<BIT [14] Extend Segment 14 Empty.
        uint16_t EXTEND_SEGMENT15_EMPTY : 1;         ///<BIT [15] Extend Segment 15 Empty.
    } b;    
} ExtendSgmStat2_t;

///  @brief 0xF011483E : Extend Segment Status 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXTEND_SEGMENT16_EMPTY : 1;         ///<BIT [0] Extend Segment 16 Empty.
        uint16_t EXTEND_SEGMENT17_EMPTY : 1;         ///<BIT [1] Extend Segment 17 Empty.
        uint16_t EXTEND_SEGMENT18_EMPTY : 1;         ///<BIT [2] Extend Segment 18 Empty.
        uint16_t EXTEND_SEGMENT19_EMPTY : 1;         ///<BIT [3] Extend Segment 19 Empty.
        uint16_t EXTEND_SEGMENT20_EMPTY : 1;         ///<BIT [4] Extend Segment 20 Empty.
        uint16_t EXTEND_SEGMENT21_EMPTY : 1;         ///<BIT [5] Extend Segment 21 Empty.
        uint16_t EXTEND_SEGMENT22_EMPTY : 1;         ///<BIT [6] Extend Segment 22 Empty.
        uint16_t EXTEND_SEGMENT23_EMPTY : 1;         ///<BIT [7] Extend Segment 23 Empty.
        uint16_t EXTEND_SEGMENT24_EMPTY : 1;         ///<BIT [8] Extend Segment 24 Empty.
        uint16_t EXTEND_SEGMENT25_EMPTY : 1;         ///<BIT [9] Extend Segment 25 Empty.
        uint16_t EXTEND_SEGMENT26_EMPTY : 1;         ///<BIT [10] Extend Segment 26 Empty.
        uint16_t EXTEND_SEGMENT27_EMPTY : 1;         ///<BIT [11] Extend Segment 27 Empty.
        uint16_t EXTEND_SEGMENT28_EMPTY : 1;         ///<BIT [12] Extend Segment 28 Empty.
        uint16_t EXTEND_SEGMENT29_EMPTY : 1;         ///<BIT [13] Extend Segment 29 Empty.
        uint16_t EXTEND_SEGMENT30_EMPTY : 1;         ///<BIT [14] Extend Segment 30 Empty.
        uint16_t EXTEND_SEGMENT31_EMPTY : 1;         ///<BIT [15] Extend Segment 31 Empty.
    } b;    
} ExtendSgmStat3_t;

///  @brief 0xF0114860 : Host Segment Select
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_SEG_SEL        : 5;         ///<BIT [4:0] Host Segment Select.
        uint16_t RF0114860_14_5_RSVD :10;         ///<BIT [14:5] Reserved.
        uint16_t HOST_SEG_SEL_EN     : 1;         ///<BIT [15] Host Segment Select Enable.
    } b;    
} HostSgmSel_t;

///  @brief 0xF0114864 : Flash Segment Select
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FLASH_SEG_SEL       : 3;         ///<BIT [2:0] Flash Segment Select.
        uint16_t RF0114864_14_3_RSVD :12;         ///<BIT [14:3] Reserved.
        uint16_t FLASH_SEG_SEL_EN    : 1;         ///<BIT [15] Flash Segment Select Enable.
    } b;    
} FlashSgmSel_t;

///  @brief 0xF0114866 : Extend Segment Select
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXTEND_SEG_SEL      : 3;         ///<BIT [2:0] Extend Segment Select.
        uint16_t RF0114866_14_3_RSVD :12;         ///<BIT [14:3] Reserved.
        uint16_t EXTEND_SEG_SEL_EN   : 1;         ///<BIT [15] Extend Segment Select Enable.
    } b;    
} ExtendSgmSel_t;

///  @brief 0xF0114868 : Host Segment Streaming Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HS_FSSC_SEL          : 4;         ///<BIT [3:0] HS FSSC Destination Select.
        uint16_t RF0114868_6_4_RSVD   : 3;         ///<BIT [6:4] Reserved.
        uint16_t FSSC_TO_HSSC         : 1;         ///<BIT [7] Flash SSC (FSSC) to Host SSC (HSSC).
        uint16_t HS_ESSC_SEL          : 4;         ///<BIT [11:8] HS ESSC Destination Select.
        uint16_t RF0114868_14_12_RSVD : 3;         ///<BIT [14:12] Reserved.
        uint16_t ESSC_TO_HSSC         : 1;         ///<BIT [15] Extend SSC (ESSC) to Host SSC (HSSC).
    } b;    
} HostSgmStreaminCtrl_t;

///  @brief 0xF011486C : Flash Segment Streaming Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FS_HSSC_SEL          : 6;         ///<BIT [5:0] FS HSSC Destination Select.
        uint16_t RF011486C_6_RSVD     : 1;         ///<BIT [6] Reserved.
        uint16_t HSSC_TO_FSSC         : 1;         ///<BIT [7] Host SSC (HSSC) to Flash SSC (FSSC).
        uint16_t FS_ESSC_SEL          : 4;         ///<BIT [11:8] FS ESSC Destination Select.
        uint16_t RF011486C_14_12_RSVD : 3;         ///<BIT [14:12] Reserved.
        uint16_t ESSC_TO_FSSC         : 1;         ///<BIT [15] Extend SSC (ESSC) to Flash SSC (FSSC).
    } b;    
} FlashSgmStreaminCtrl_t;

///  @brief 0xF011486E : Extend Segment Streaming Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ES_HSSC_SEL          : 6;         ///<BIT [5:0] ES HSSC Destination Select.
        uint16_t RF011486E_6_RSVD     : 1;         ///<BIT [6] Reserved.
        uint16_t HSSC_TO_ESSC         : 1;         ///<BIT [7] Host SSC (HSSC) to Extend SSC (ESSC).
        uint16_t ES_FSSC_SEL          : 4;         ///<BIT [11:8] ES FSSC Destination Select.
        uint16_t RF011486E_14_12_RSVD : 3;         ///<BIT [14:12] Reserved.
        uint16_t FSSC_TO_ESSC         : 1;         ///<BIT [15] Flash SSC (FSSC) to Extend SSC (ESSC).
    } b;    
} ExtendSgmStreaminCtrl_t;

///  @brief 0xF0114870 : Host Adjust Segment Select
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_ADJ_SEG_SEL    : 5;         ///<BIT [4:0] Host Adjust Segment Select.
        uint16_t RF0114870_15_5_RSVD :11;         ///<BIT [15:5] Reserved.
    } b;    
} HostAdjSgmSel_t;

///  @brief 0xF0114872 : Host1 Adjust Segment Select
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_ADJ_SEG_SEL    : 5;         ///<BIT [4:0] Host1 Adjust Segment Select.
        uint16_t RF0114872_15_5_RSVD :11;         ///<BIT [15:5] Reserved.
    } b;    
} Host1AdjSgmSel_t;

///  @brief 0xF0114874 : Flash Adjust Segment Select
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FLASH_ADJ_SEG_SEL   : 5;         ///<BIT [4:0] Flash Adjust Segment Select.
        uint16_t RF0114874_15_5_RSVD :11;         ///<BIT [15:5] Reserved.
    } b;    
} FlashAdjSgmSel_t;

///  @brief 0xF0114876 : Extend Adjust Segment Select
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXTEND_ADJ_SEG_SEL  : 5;         ///<BIT [4:0] Extend Adjust Segment Select.
        uint16_t RF0114876_15_5_RSVD :11;         ///<BIT [15:5] Reserved.
    } b;    
} ExtendAdjSgmSel_t;

///  @brief 0xF01148A0 : Host SSC Comp Status 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_SSC0_MATCH  : 1;         ///<BIT [0] Host SSC0 Match.
        uint16_t HOST_SSC1_MATCH  : 1;         ///<BIT [1] Host SSC1 Match.
        uint16_t HOST_SSC2_MATCH  : 1;         ///<BIT [2] Host SSC2 Match.
        uint16_t HOST_SSC3_MATCH  : 1;         ///<BIT [3] Host SSC3 Match.
        uint16_t HOST_SSC4_MATCH  : 1;         ///<BIT [4] Host SSC4 Match.
        uint16_t HOST_SSC5_MATCH  : 1;         ///<BIT [5] Host SSC5 Match.
        uint16_t HOST_SSC6_MATCH  : 1;         ///<BIT [6] Host SSC6 Match.
        uint16_t HOST_SSC7_MATCH  : 1;         ///<BIT [7] Host SSC7 Match.
        uint16_t HOST_SSC8_MATCH  : 1;         ///<BIT [8] Host SSC8 Match.
        uint16_t HOST_SSC9_MATCH  : 1;         ///<BIT [9] Host SSC9 Match.
        uint16_t HOST_SSC10_MATCH : 1;         ///<BIT [10] Host SSC10 Match.
        uint16_t HOST_SSC11_MATCH : 1;         ///<BIT [11] Host SSC11 Match.
        uint16_t HOST_SSC12_MATCH : 1;         ///<BIT [12] Host SSC12 Match.
        uint16_t HOST_SSC13_MATCH : 1;         ///<BIT [13] Host SSC13 Match.
        uint16_t HOST_SSC14_MATCH : 1;         ///<BIT [14] Host SSC14 Match.
        uint16_t HOST_SSC15_MATCH : 1;         ///<BIT [15] Host SSC15 Match.
    } b;    
} HostSscCompStat0_t;

///  @brief 0xF01148A2 : Host SSC Comp Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_SSC16_MATCH : 1;         ///<BIT [0] Host SSC16 Match.
        uint16_t HOST_SSC17_MATCH : 1;         ///<BIT [1] Host SSC17 Match.
        uint16_t HOST_SSC18_MATCH : 1;         ///<BIT [2] Host SSC18 Match.
        uint16_t HOST_SSC19_MATCH : 1;         ///<BIT [3] Host SSC19 Match.
        uint16_t HOST_SSC20_MATCH : 1;         ///<BIT [4] Host SSC20 Match.
        uint16_t HOST_SSC21_MATCH : 1;         ///<BIT [5] Host SSC21 Match.
        uint16_t HOST_SSC22_MATCH : 1;         ///<BIT [6] Host SSC22 Match.
        uint16_t HOST_SSC23_MATCH : 1;         ///<BIT [7] Host SSC23 Match.
        uint16_t HOST_SSC24_MATCH : 1;         ///<BIT [8] Host SSC24 Match.
        uint16_t HOST_SSC25_MATCH : 1;         ///<BIT [9] Host SSC25 Match.
        uint16_t HOST_SSC26_MATCH : 1;         ///<BIT [10] Host SSC26 Match.
        uint16_t HOST_SSC27_MATCH : 1;         ///<BIT [11] Host SSC27 Match.
        uint16_t HOST_SSC28_MATCH : 1;         ///<BIT [12] Host SSC28 Match.
        uint16_t HOST_SSC29_MATCH : 1;         ///<BIT [13] Host SSC29 Match.
        uint16_t HOST_SSC30_MATCH : 1;         ///<BIT [14] Host SSC30 Match.
        uint16_t HOST_SSC31_MATCH : 1;         ///<BIT [15] Host SSC31 Match.
    } b;    
} HostSscCompStat1_t;

///  @brief 0xF01148A8 : Flash SSC Comp Status 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FLASH_SSC0_MATCH  : 1;         ///<BIT [0] Flash SSC0 Match.
        uint16_t FLASH_SSC1_MATCH  : 1;         ///<BIT [1] Flash SSC1 Match.
        uint16_t FLASH_SSC2_MATCH  : 1;         ///<BIT [2] Flash SSC2 Match.
        uint16_t FLASH_SSC3_MATCH  : 1;         ///<BIT [3] Flash SSC3 Match.
        uint16_t FLASH_SSC4_MATCH  : 1;         ///<BIT [4] Flash SSC4 Match.
        uint16_t FLASH_SSC5_MATCH  : 1;         ///<BIT [5] Flash SSC5 Match.
        uint16_t FLASH_SSC6_MATCH  : 1;         ///<BIT [6] Flash SSC6 Match.
        uint16_t FLASH_SSC7_MATCH  : 1;         ///<BIT [7] Flash SSC7 Match.
        uint16_t FLASH_SSC8_MATCH  : 1;         ///<BIT [8] Flash SSC8 Match.
        uint16_t FLASH_SSC9_MATCH  : 1;         ///<BIT [9] Flash SSC9 Match.
        uint16_t FLASH_SSC10_MATCH : 1;         ///<BIT [10] Flash SSC10 Match.
        uint16_t FLASH_SSC11_MATCH : 1;         ///<BIT [11] Flash SSC11 Match.
        uint16_t FLASH_SSC12_MATCH : 1;         ///<BIT [12] Flash SSC12 Match.
        uint16_t FLASH_SSC13_MATCH : 1;         ///<BIT [13] Flash SSC13 Match.
        uint16_t FLASH_SSC14_MATCH : 1;         ///<BIT [14] Flash SSC14 Match.
        uint16_t FLASH_SSC15_MATCH : 1;         ///<BIT [15] Flash SSC15 Match.
    } b;    
} FlashSscCompStat0_t;

///  @brief 0xF01148AA : Flash SSC Comp Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FLASH_SSC16_MATCH : 1;         ///<BIT [0] Flash SSC16 Match.
        uint16_t FLASH_SSC17_MATCH : 1;         ///<BIT [1] Flash SSC17 Match.
        uint16_t FLASH_SSC18_MATCH : 1;         ///<BIT [2] Flash SSC18 Match.
        uint16_t FLASH_SSC19_MATCH : 1;         ///<BIT [3] Flash SSC19 Match.
        uint16_t FLASH_SSC20_MATCH : 1;         ///<BIT [4] Flash SSC20 Match.
        uint16_t FLASH_SSC21_MATCH : 1;         ///<BIT [5] Flash SSC21 Match.
        uint16_t FLASH_SSC22_MATCH : 1;         ///<BIT [6] Flash SSC22 Match.
        uint16_t FLASH_SSC23_MATCH : 1;         ///<BIT [7] Flash SSC23 Match.
        uint16_t FLASH_SSC24_MATCH : 1;         ///<BIT [8] Flash SSC24 Match.
        uint16_t FLASH_SSC25_MATCH : 1;         ///<BIT [9] Flash SSC25 Match.
        uint16_t FLASH_SSC26_MATCH : 1;         ///<BIT [10] Flash SSC26 Match.
        uint16_t FLASH_SSC27_MATCH : 1;         ///<BIT [11] Flash SSC27 Match.
        uint16_t FLASH_SSC28_MATCH : 1;         ///<BIT [12] Flash SSC28 Match.
        uint16_t FLASH_SSC29_MATCH : 1;         ///<BIT [13] Flash SSC29 Match.
        uint16_t FLASH_SSC30_MATCH : 1;         ///<BIT [14] Flash SSC30 Match.
        uint16_t FLASH_SSC31_MATCH : 1;         ///<BIT [15] Flash SSC31 Match.
    } b;    
} FlashSscCompStat1_t;

///  @brief 0xF01148AC : Extend SSC Comp Status 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXTEND_SSC0_MATCH  : 1;         ///<BIT [0] Extend SSC0 Match.
        uint16_t EXTEND_SSC1_MATCH  : 1;         ///<BIT [1] Extend SSC1 Match.
        uint16_t EXTEND_SSC2_MATCH  : 1;         ///<BIT [2] Extend SSC2 Match.
        uint16_t EXTEND_SSC3_MATCH  : 1;         ///<BIT [3] Extend SSC3 Match.
        uint16_t EXTEND_SSC4_MATCH  : 1;         ///<BIT [4] Extend SSC4 Match.
        uint16_t EXTEND_SSC5_MATCH  : 1;         ///<BIT [5] Extend SSC5 Match.
        uint16_t EXTEND_SSC6_MATCH  : 1;         ///<BIT [6] Extend SSC6 Match.
        uint16_t EXTEND_SSC7_MATCH  : 1;         ///<BIT [7] Extend SSC7 Match.
        uint16_t EXTEND_SSC8_MATCH  : 1;         ///<BIT [8] Extend SSC8 Match.
        uint16_t EXTEND_SSC9_MATCH  : 1;         ///<BIT [9] Extend SSC9 Match.
        uint16_t EXTEND_SSC10_MATCH : 1;         ///<BIT [10] Extend SSC10 Match.
        uint16_t EXTEND_SSC11_MATCH : 1;         ///<BIT [11] Extend SSC11 Match.
        uint16_t EXTEND_SSC12_MATCH : 1;         ///<BIT [12] Extend SSC12 Match.
        uint16_t EXTEND_SSC13_MATCH : 1;         ///<BIT [13] Extend SSC13 Match.
        uint16_t EXTEND_SSC14_MATCH : 1;         ///<BIT [14] Extend SSC14 Match.
        uint16_t EXTEND_SSC15_MATCH : 1;         ///<BIT [15] Extend SSC15 Match.
    } b;    
} ExtendSscCompStat0_t;

///  @brief 0xF01148AE : Extend SSC Comp Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXTEND_SSC16_MATCH : 1;         ///<BIT [0] Extend SSC16 Match.
        uint16_t EXTEND_SSC17_MATCH : 1;         ///<BIT [1] Extend SSC17 Match.
        uint16_t EXTEND_SSC18_MATCH : 1;         ///<BIT [2] Extend SSC18 Match.
        uint16_t EXTEND_SSC19_MATCH : 1;         ///<BIT [3] Extend SSC19 Match.
        uint16_t EXTEND_SSC20_MATCH : 1;         ///<BIT [4] Extend SSC20 Match.
        uint16_t EXTEND_SSC21_MATCH : 1;         ///<BIT [5] Extend SSC21 Match.
        uint16_t EXTEND_SSC22_MATCH : 1;         ///<BIT [6] Extend SSC22 Match.
        uint16_t EXTEND_SSC23_MATCH : 1;         ///<BIT [7] Extend SSC23 Match.
        uint16_t EXTEND_SSC24_MATCH : 1;         ///<BIT [8] Extend SSC24 Match.
        uint16_t EXTEND_SSC25_MATCH : 1;         ///<BIT [9] Extend SSC25 Match.
        uint16_t EXTEND_SSC26_MATCH : 1;         ///<BIT [10] Extend SSC26 Match.
        uint16_t EXTEND_SSC27_MATCH : 1;         ///<BIT [11] Extend SSC27 Match.
        uint16_t EXTEND_SSC28_MATCH : 1;         ///<BIT [12] Extend SSC28 Match.
        uint16_t EXTEND_SSC29_MATCH : 1;         ///<BIT [13] Extend SSC29 Match.
        uint16_t EXTEND_SSC30_MATCH : 1;         ///<BIT [14] Extend SSC30 Match.
        uint16_t EXTEND_SSC31_MATCH : 1;         ///<BIT [15] Extend SSC31 Match.
    } b;    
} ExtendSscCompStat1_t;

///  @brief 0xF01148B0 : Host SSC Comp Interrupt Mask 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_SSC0_MATCH_INT_MK  : 1;         ///<BIT [0] Host SSC0 Match.
        uint16_t HOST_SSC1_MATCH_INT_MK  : 1;         ///<BIT [1] Host SSC1 Match.
        uint16_t HOST_SSC2_MATCH_INT_MK  : 1;         ///<BIT [2] Host SSC2 Match.
        uint16_t HOST_SSC3_MATCH_INT_MK  : 1;         ///<BIT [3] Host SSC3 Match.
        uint16_t HOST_SSC4_MATCH_INT_MK  : 1;         ///<BIT [4] Host SSC4 Match.
        uint16_t HOST_SSC5_MATCH_INT_MK  : 1;         ///<BIT [5] Host SSC5 Match.
        uint16_t HOST_SSC6_MATCH_INT_MK  : 1;         ///<BIT [6] Host SSC6 Match.
        uint16_t HOST_SSC7_MATCH_INT_MK  : 1;         ///<BIT [7] Host SSC7 Match.
        uint16_t HOST_SSC8_MATCH_INT_MK  : 1;         ///<BIT [8] Host SSC8 Match.
        uint16_t HOST_SSC9_MATCH_INT_MK  : 1;         ///<BIT [9] Host SSC9 Match.
        uint16_t HOST_SSC10_MATCH_INT_MK : 1;         ///<BIT [10] Host SSC10 Match.
        uint16_t HOST_SSC11_MATCH_INT_MK : 1;         ///<BIT [11] Host SSC11 Match.
        uint16_t HOST_SSC12_MATCH_INT_MK : 1;         ///<BIT [12] Host SSC12 Match.
        uint16_t HOST_SSC13_MATCH_INT_MK : 1;         ///<BIT [13] Host SSC13 Match.
        uint16_t HOST_SSC14_MATCH_INT_MK : 1;         ///<BIT [14] Host SSC14 Match.
        uint16_t HOST_SSC15_MATCH_INT_MK : 1;         ///<BIT [15] Host SSC15 Match.
    } b;    
} HostSscCompIntrMsk0_t;

///  @brief 0xF01148B2 : Host SSC Comp Interrupt Mask 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_SSC16_MATCH_INT_MK : 1;         ///<BIT [0] Host SSC16 Match.
        uint16_t HOST_SSC17_MATCH_INT_MK : 1;         ///<BIT [1] Host SSC17 Match.
        uint16_t HOST_SSC18_MATCH_INT_MK : 1;         ///<BIT [2] Host SSC18 Match.
        uint16_t HOST_SSC19_MATCH_INT_MK : 1;         ///<BIT [3] Host SSC19 Match.
        uint16_t HOST_SSC20_MATCH_INT_MK : 1;         ///<BIT [4] Host SSC20 Match.
        uint16_t HOST_SSC21_MATCH_INT_MK : 1;         ///<BIT [5] Host SSC21 Match.
        uint16_t HOST_SSC22_MATCH_INT_MK : 1;         ///<BIT [6] Host SSC22 Match.
        uint16_t HOST_SSC23_MATCH_INT_MK : 1;         ///<BIT [7] Host SSC23 Match.
        uint16_t HOST_SSC24_MATCH_INT_MK : 1;         ///<BIT [8] Host SSC24 Match.
        uint16_t HOST_SSC25_MATCH_INT_MK : 1;         ///<BIT [9] Host SSC25 Match.
        uint16_t HOST_SSC26_MATCH_INT_MK : 1;         ///<BIT [10] Host SSC26 Match.
        uint16_t HOST_SSC27_MATCH_INT_MK : 1;         ///<BIT [11] Host SSC27 Match.
        uint16_t HOST_SSC28_MATCH_INT_MK : 1;         ///<BIT [12] Host SSC28 Match.
        uint16_t HOST_SSC29_MATCH_INT_MK : 1;         ///<BIT [13] Host SSC29 Match.
        uint16_t HOST_SSC30_MATCH_INT_MK : 1;         ///<BIT [14] Host SSC30 Match.
        uint16_t HOST_SSC31_MATCH_INT_MK : 1;         ///<BIT [15] Host SSC31 Match.
    } b;    
} HostSscCompIntrMsk1_t;

///  @brief 0xF01148B8 : Flash SSC Comp Interrupt Mask 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FLASH_SSC0_MATCH_INT_MK  : 1;         ///<BIT [0] Flash SSC0 Match.
        uint16_t FLASH_SSC1_MATCH_INT_MK  : 1;         ///<BIT [1] Flash SSC1 Match.
        uint16_t FLASH_SSC2_MATCH_INT_MK  : 1;         ///<BIT [2] Flash SSC2 Match.
        uint16_t FLASH_SSC3_MATCH_INT_MK  : 1;         ///<BIT [3] Flash SSC3 Match.
        uint16_t FLASH_SSC4_MATCH_INT_MK  : 1;         ///<BIT [4] Flash SSC4 Match.
        uint16_t FLASH_SSC5_MATCH_INT_MK  : 1;         ///<BIT [5] Flash SSC5 Match.
        uint16_t FLASH_SSC6_MATCH_INT_MK  : 1;         ///<BIT [6] Flash SSC6 Match.
        uint16_t FLASH_SSC7_MATCH_INT_MK  : 1;         ///<BIT [7] Flash SSC7 Match.
        uint16_t FLASH_SSC8_MATCH_INT_MK  : 1;         ///<BIT [8] Flash SSC8 Match.
        uint16_t FLASH_SSC9_MATCH_INT_MK  : 1;         ///<BIT [9] Flash SSC9 Match.
        uint16_t FLASH_SSC10_MATCH_INT_MK : 1;         ///<BIT [10] Flash SSC10 Match.
        uint16_t FLASH_SSC11_MATCH_INT_MK : 1;         ///<BIT [11] Flash SSC11 Match.
        uint16_t FLASH_SSC12_MATCH_INT_MK : 1;         ///<BIT [12] Flash SSC12 Match.
        uint16_t FLASH_SSC13_MATCH_INT_MK : 1;         ///<BIT [13] Flash SSC13 Match.
        uint16_t FLASH_SSC14_MATCH_INT_MK : 1;         ///<BIT [14] Flash SSC14 Match.
        uint16_t FLASH_SSC15_MATCH_INT_MK : 1;         ///<BIT [15] Flash SSC15 Match.
    } b;    
} FlashSscCompIntrMsk0_t;

///  @brief 0xF01148BA : Flash SSC Comp Interrupt Mask 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FLASH_SSC16_MATCH_INT_MK : 1;         ///<BIT [0] Flash SSC16 Match.
        uint16_t FLASH_SSC17_MATCH_INT_MK : 1;         ///<BIT [1] Flash SSC17 Match.
        uint16_t FLASH_SSC18_MATCH_INT_MK : 1;         ///<BIT [2] Flash SSC18 Match.
        uint16_t FLASH_SSC19_MATCH_INT_MK : 1;         ///<BIT [3] Flash SSC19 Match.
        uint16_t FLASH_SSC20_MATCH_INT_MK : 1;         ///<BIT [4] Flash SSC20 Match.
        uint16_t FLASH_SSC21_MATCH_INT_MK : 1;         ///<BIT [5] Flash SSC21 Match.
        uint16_t FLASH_SSC22_MATCH_INT_MK : 1;         ///<BIT [6] Flash SSC22 Match.
        uint16_t FLASH_SSC23_MATCH_INT_MK : 1;         ///<BIT [7] Flash SSC23 Match.
        uint16_t FLASH_SSC24_MATCH_INT_MK : 1;         ///<BIT [8] Flash SSC24 Match.
        uint16_t FLASH_SSC25_MATCH_INT_MK : 1;         ///<BIT [9] Flash SSC25 Match.
        uint16_t FLASH_SSC26_MATCH_INT_MK : 1;         ///<BIT [10] Flash SSC26 Match.
        uint16_t FLASH_SSC27_MATCH_INT_MK : 1;         ///<BIT [11] Flash SSC27 Match.
        uint16_t FLASH_SSC28_MATCH_INT_MK : 1;         ///<BIT [12] Flash SSC28 Match.
        uint16_t FLASH_SSC29_MATCH_INT_MK : 1;         ///<BIT [13] Flash SSC29 Match.
        uint16_t FLASH_SSC30_MATCH_INT_MK : 1;         ///<BIT [14] Flash SSC30 Match.
        uint16_t FLASH_SSC31_MATCH_INT_MK : 1;         ///<BIT [15] Flash SSC31 Match.
    } b;    
} FlashSscCompIntrMsk1_t;

///  @brief 0xF01148BC : Extend SSC Comp Interrupt Mask 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXTEND_SSC0_MATCH_INT_MK  : 1;         ///<BIT [0] Extend SSC0 Match.
        uint16_t EXTEND_SSC1_MATCH_INT_MK  : 1;         ///<BIT [1] Extend SSC1 Match.
        uint16_t EXTEND_SSC2_MATCH_INT_MK  : 1;         ///<BIT [2] Extend SSC2 Match.
        uint16_t EXTEND_SSC3_MATCH_INT_MK  : 1;         ///<BIT [3] Extend SSC3 Match.
        uint16_t EXTEND_SSC4_MATCH_INT_MK  : 1;         ///<BIT [4] Extend SSC4 Match.
        uint16_t EXTEND_SSC5_MATCH_INT_MK  : 1;         ///<BIT [5] Extend SSC5 Match.
        uint16_t EXTEND_SSC6_MATCH_INT_MK  : 1;         ///<BIT [6] Extend SSC6 Match.
        uint16_t EXTEND_SSC7_MATCH_INT_MK  : 1;         ///<BIT [7] Extend SSC7 Match.
        uint16_t EXTEND_SSC8_MATCH_INT_MK  : 1;         ///<BIT [8] Extend SSC8 Match.
        uint16_t EXTEND_SSC9_MATCH_INT_MK  : 1;         ///<BIT [9] Extend SSC9 Match.
        uint16_t EXTEND_SSC10_MATCH_INT_MK : 1;         ///<BIT [10] Extend SSC10 Match.
        uint16_t EXTEND_SSC11_MATCH_INT_MK : 1;         ///<BIT [11] Extend SSC11 Match.
        uint16_t EXTEND_SSC12_MATCH_INT_MK : 1;         ///<BIT [12] Extend SSC12 Match.
        uint16_t EXTEND_SSC13_MATCH_INT_MK : 1;         ///<BIT [13] Extend SSC13 Match.
        uint16_t EXTEND_SSC14_MATCH_INT_MK : 1;         ///<BIT [14] Extend SSC14 Match.
        uint16_t EXTEND_SSC15_MATCH_INT_MK : 1;         ///<BIT [15] Extend SSC15 Match.
    } b;    
} ExtendSscCompIntrMsk0_t;

///  @brief 0xF01148BE : Extend SSC Comp Interrupt Mask 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXTEND_SSC16_MATCH_INT_MK : 1;         ///<BIT [0] Extend SSC16 Match.
        uint16_t EXTEND_SSC17_MATCH_INT_MK : 1;         ///<BIT [1] Extend SSC17 Match.
        uint16_t EXTEND_SSC18_MATCH_INT_MK : 1;         ///<BIT [2] Extend SSC18 Match.
        uint16_t EXTEND_SSC19_MATCH_INT_MK : 1;         ///<BIT [3] Extend SSC19 Match.
        uint16_t EXTEND_SSC20_MATCH_INT_MK : 1;         ///<BIT [4] Extend SSC20 Match.
        uint16_t EXTEND_SSC21_MATCH_INT_MK : 1;         ///<BIT [5] Extend SSC21 Match.
        uint16_t EXTEND_SSC22_MATCH_INT_MK : 1;         ///<BIT [6] Extend SSC22 Match.
        uint16_t EXTEND_SSC23_MATCH_INT_MK : 1;         ///<BIT [7] Extend SSC23 Match.
        uint16_t EXTEND_SSC24_MATCH_INT_MK : 1;         ///<BIT [8] Extend SSC24 Match.
        uint16_t EXTEND_SSC25_MATCH_INT_MK : 1;         ///<BIT [9] Extend SSC25 Match.
        uint16_t EXTEND_SSC26_MATCH_INT_MK : 1;         ///<BIT [10] Extend SSC26 Match.
        uint16_t EXTEND_SSC27_MATCH_INT_MK : 1;         ///<BIT [11] Extend SSC27 Match.
        uint16_t EXTEND_SSC28_MATCH_INT_MK : 1;         ///<BIT [12] Extend SSC28 Match.
        uint16_t EXTEND_SSC29_MATCH_INT_MK : 1;         ///<BIT [13] Extend SSC29 Match.
        uint16_t EXTEND_SSC30_MATCH_INT_MK : 1;         ///<BIT [14] Extend SSC30 Match.
        uint16_t EXTEND_SSC31_MATCH_INT_MK : 1;         ///<BIT [15] Extend SSC31 Match.
    } b;    
} ExtendSscCompIntrMsk1_t;

///  @brief 0xF01148C0 : Host MQ RAM Approved Pointer
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_MQ_RAM_APP_PTR  :10;         ///<BIT [9:0] Host MQ RAM Approved Pointer.
        uint16_t RF01148C0_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} HostMqRamApprovedPtr_t;

///  @brief 0xF01148C4 : Flash MQ RAM Approved Pointer
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FLSH_MQ_RAM_APP_PTR  :10;         ///<BIT [9:0] Flash MQ RAM Approved Pointer.
        uint16_t RF01148C4_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} FlashMqRamApprovedPtr_t;

///  @brief 0xF01148C6 : Extend MQ RAM Approved Pointer
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXTD_MQ_RAM_APP_PTR  :10;         ///<BIT [9:0] Extend MQ RAM Approved Pointer.
        uint16_t RF01148C6_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} ExtendMqRamApprovedPtr_t;

///  @brief 0xF01148C8 : Host MQ RAM Search Pointer
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_MQ_RAM_SER_PTR  :10;         ///<BIT [9:0] Host MQ RAM Search Pointer.
        uint16_t RF01148C8_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} HostMqRamSearchPtr_t;

///  @brief 0xF01148CC : Flash MQ RAM Search Pointer
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FLSH_MQ_RAM_SER_PTR  :10;         ///<BIT [9:0] Flash MQ RAM Search Pointer.
        uint16_t RF01148CC_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} FlashMqRamSearchPtr_t;

///  @brief 0xF01148CE : Extend MQ RAM Search Pointer
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXTD_MQ_RAM_SER_PTR  :10;         ///<BIT [9:0] Extend MQ RAM Search Pointer.
        uint16_t RF01148CE_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} ExtendMqRamSearchPtr_t;

///  @brief 0xF01148D0 : Host MQ RAM Queue Count
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_MQ_RAM_QUE_CNT  :11;         ///<BIT [10:0] Host MQ RAM Queue Count.
        uint16_t RF01148D0_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
    } b;    
} HostMqRamQueueCnt_t;

///  @brief 0xF01148D4 : Flash MQ RAM Queue Count
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FLSH_MQ_RAM_QUE_CNT  :11;         ///<BIT [10:0] Flash MQ RAM Queue Count.
        uint16_t RF01148D4_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
    } b;    
} FlashMqRamQueueCnt_t;

///  @brief 0xF01148D6 : Extend MQ RAM Queue Count
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXTD_MQ_RAM_QUE_CNT  :11;         ///<BIT [10:0] Extend MQ RAM Queue Count.
        uint16_t RF01148D6_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
    } b;    
} ExtendMqRamQueueCnt_t;

///  @brief 0xF01148F8 : BIST Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BIST_RESET           : 1;         ///<BIT [0] BIST Reset.
        uint16_t ROWCOLSWITCH         : 1;         ///<BIT [1] Row Column Switch.
        uint16_t RUNSTOP              : 1;         ///<BIT [2] BIST Run Stop.
        uint16_t BSTATUS_EN           : 1;         ///<BIT [3] BIST Status Enable.
        uint16_t BSTATUS_CLK          : 1;         ///<BIT [4] BIST Status Clock.
        uint16_t RF01148F8_11_5_RSVD  : 7;         ///<BIT [11:5] Reserved.
        uint16_t RF01148F8_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} BistCtrl_t;

///  @brief 0xF01148FA : BIST Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF01148FA_1_0_RSVD   : 2;         ///<BIT [1:0] Reserved.
        uint16_t FLASH_BFINISH        : 1;         ///<BIT [2] BIST Test Finished for Flash QRAM.
        uint16_t EXTEND_BFINISH       : 1;         ///<BIT [3] BIST Test Finished for Extend QRAM.
        uint16_t RF01148FA_5_4_RSVD   : 2;         ///<BIT [5:4] Reserved.
        uint16_t FLASH_BERROR         : 1;         ///<BIT [6] BIST Test Error during Diagnostic Mode for Flash QRAM.
        uint16_t EXTEND_BERROR        : 1;         ///<BIT [7] BIST Test Error during Diagnostic Mode for Extend QRAM.
        uint16_t RF01148FA_9_8_RSVD   : 2;         ///<BIT [9:8] Reserved.
        uint16_t FLASH_ANY_BISTFAIL   : 1;         ///<BIT [10] BIST Failure for Flash QRAM.
        uint16_t EXTEND_ANY_BISTFAIL  : 1;         ///<BIT [11] BIST Failure for Extend QRAM.
        uint16_t RF01148FA_13_12_RSVD : 2;         ///<BIT [13:12] Reserved.
        uint16_t FLASH_BSTAUTS        : 1;         ///<BIT [14] BIST Status Output for Flash QRAM.
        uint16_t EXTEND_BSTAUTS       : 1;         ///<BIT [15] BIST Status Output for Extend QRAM.
    } b;    
} BistStat_t;

///  @brief 0xF01148FC : MQ RAM Timing Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF01148FC_1_0_RSVD : 2;         ///<BIT [1:0] Reserved.
        uint16_t RF01148FC_3_2_RSVD : 2;         ///<BIT [3:2] Reserved.
        uint16_t RF01148FC_5_4_RSVD : 2;         ///<BIT [5:4] Reserved.
        uint16_t RF01148FC_7_6_RSVD : 2;         ///<BIT [7:6] Reserved.
        uint16_t FLASH_MQRAM_RTC    : 2;         ///<BIT [9:8] RTC for Flash MQRAM.
        uint16_t FLASH_MQRAM_WTC    : 2;         ///<BIT [11:10] WTC for Flash MQRAM.
        uint16_t EXTEND_MQRAM_RTC   : 2;         ///<BIT [13:12] RTC for Extend MQRAM.
        uint16_t EXTEND_MQRAM_WTC   : 2;         ///<BIT [15:14] WTC for Extend MQRAM.
    } b;    
} MqRamTmCtrl_t;

///  @brief 0xF01148FE : MQ RAM Power Down Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF01148FE_3_0_RSVD   : 4;         ///<BIT [3:0] Reserved.
        uint16_t RF01148FE_7_4_RSVD   : 4;         ///<BIT [7:4] Reserved.
        uint16_t RF01148FE_8_RSVD     : 1;         ///<BIT [8] Reserved.
        uint16_t FLSH_MQRAM_PDFVSSM   : 1;         ///<BIT [9] Flash MQ RAM Power Down Leakage using PDFVSSM.
        uint16_t FLSH_MQRAM_PDLVMC    : 1;         ///<BIT [10] Flash MQ RAM Power Down Leakage using PDLVMC.
        uint16_t FLSH_MQRAM_PDWN      : 1;         ///<BIT [11] Flash MQ RAM Power Down.
        uint16_t RF01148FE_12_RSVD    : 1;         ///<BIT [12] Reserved.
        uint16_t EXTEND_MQRAM_PDFVSSM : 1;         ///<BIT [13] Extend MQ RAM Power Down Leakage using PDFVSSM.
        uint16_t EXTEND_MQRAM_PDLVMC  : 1;         ///<BIT [14] Extend MQ RAM Power Down Leakage using PDLVMC.
        uint16_t EXTEND_MQRAM_PDWN    : 1;         ///<BIT [15] Extend MQ RAM Power Down.
    } b;    
} MqRamPowerDnCtrl_t;


/// @brief 0xF0114800-0xF0114B00 : SegStr_t MODULE
typedef struct
{
    StcCtrl_t stcCtrl;                                       ///< 0xF0114800 : STC Control
    StreaminPortInitialiCtrl_t streaminPortInitialiCtrl;     ///< 0xF0114802 : Streaming Port Initialization Control
    uint8_t rsvd_RF0114804[4];                               ///< 0xF0114804 - 0xF0114807 : Reserved.
    HostRatio_t hostRatio;                                   ///< 0xF0114808 : Host Ratio
    SscMatchDetectCtrl_t sscMatchDetectCtrl;                 ///< 0xF011480A : SSC Match Detect Control
    uint8_t rsvd_RF011480C[4];                               ///< 0xF011480C - 0xF011480F : Reserved.
    MqRamAddr_t mqRamAddr;                                   ///< 0xF0114810 : MQ RAM Address
    uint16_t MQ_RAM_DATA;                                    ///< 0xF0114812 : MQ RAM Data
    MqRamDepthCtrl_t mqRamDepthCtrl;                         ///< 0xF0114814 : MQ RAM Depth Control
    uint8_t rsvd_RF0114816[6];                               ///< 0xF0114816 - 0xF011481B : Reserved.
    FlashMqRamRsvdSize_t flashMqRamRsvdSize;                 ///< 0xF011481C : Flash MQ RAM Reserved Size
    ExtendMqRamRsvdSize_t extendMqRamRsvdSize;               ///< 0xF011481E : Extend MQ RAM Reserved Size
    HostSgmStat0_t hostSgmStat0;                             ///< 0xF0114820 : Host Segment Status 0
    HostSgmStat1_t hostSgmStat1;                             ///< 0xF0114822 : Host Segment Status 1
    HostSgmStat2_t hostSgmStat2;                             ///< 0xF0114824 : Host Segment Status 2
    HostSgmStat3_t hostSgmStat3;                             ///< 0xF0114826 : Host Segment Status 3
    uint8_t rsvd_RF0114828[8];                               ///< 0xF0114828 - 0xF011482F : Reserved.
    FlashSgmStat0_t flashSgmStat0;                           ///< 0xF0114830 : Flash Segment Status 0
    FlashSgmStat1_t flashSgmStat1;                           ///< 0xF0114832 : Flash Segment Status 1
    FlashSgmStat2_t flashSgmStat2;                           ///< 0xF0114834 : Flash Segment Status 2
    FlashSgmStat3_t flashSgmStat3;                           ///< 0xF0114836 : Flash Segment Status 3
    ExtendSgmStat0_t extendSgmStat0;                         ///< 0xF0114838 : Extend Segment Status 0
    ExtendSgmStat1_t extendSgmStat1;                         ///< 0xF011483A : Extend Segment Status 1
    ExtendSgmStat2_t extendSgmStat2;                         ///< 0xF011483C : Extend Segment Status 2
    ExtendSgmStat3_t extendSgmStat3;                         ///< 0xF011483E : Extend Segment Status 3
    uint16_t HOST_SEG_INT_MK0;                               ///< 0xF0114840 : Host Segment Interrupt Mask 0
    uint16_t HOST_SEG_INT_MK1;                               ///< 0xF0114842 : Host Segment Interrupt Mask 1
    uint16_t HOST_SEG_INT_MK2;                               ///< 0xF0114844 : Host Segment Interrupt Mask 2
    uint16_t HOST_SEG_INT_MK3;                               ///< 0xF0114846 : Host Segment Interrupt Mask 3
    uint8_t rsvd_RF0114848[8];                               ///< 0xF0114848 - 0xF011484F : Reserved.
    uint16_t FLASH_SEG_INT_MK0;                              ///< 0xF0114850 : Flash Segment Interrupt Mask 0
    uint16_t FLASH_SEG_INT_MK1;                              ///< 0xF0114852 : Flash Segment Interrupt Mask 1
    uint16_t FLASH_SEG_INT_MK2;                              ///< 0xF0114854 : Flash Segment Interrupt Mask 2
    uint16_t FLASH_SEG_INT_MK3;                              ///< 0xF0114856 : Flash Segment Interrupt Mask 3
    uint16_t EXTEND_SEG_INT_MK0;                             ///< 0xF0114858 : Extend Segment Interrupt Mask 0
    uint16_t EXTEND_SEG_INT_MK1;                             ///< 0xF011485A : Extend Segment Interrupt Mask 1
    uint16_t EXTEND_SEG_INT_MK2;                             ///< 0xF011485C : Extend Segment Interrupt Mask 2
    uint16_t EXTEND_SEG_INT_MK3;                             ///< 0xF011485E : Extend Segment Interrupt Mask 3
    HostSgmSel_t hostSgmSel;                                 ///< 0xF0114860 : Host Segment Select
    uint8_t rsvd_RF0114862[2];                               ///< 0xF0114862 - 0xF0114863 : Reserved.
    FlashSgmSel_t flashSgmSel;                               ///< 0xF0114864 : Flash Segment Select
    ExtendSgmSel_t extendSgmSel;                             ///< 0xF0114866 : Extend Segment Select
    HostSgmStreaminCtrl_t hostSgmStreaminCtrl;               ///< 0xF0114868 : Host Segment Streaming Control
    uint8_t rsvd_RF011486A[2];                               ///< 0xF011486A - 0xF011486B : Reserved.
    FlashSgmStreaminCtrl_t flashSgmStreaminCtrl;             ///< 0xF011486C : Flash Segment Streaming Control
    ExtendSgmStreaminCtrl_t extendSgmStreaminCtrl;           ///< 0xF011486E : Extend Segment Streaming Control
    HostAdjSgmSel_t hostAdjSgmSel;                           ///< 0xF0114870 : Host Adjust Segment Select
    Host1AdjSgmSel_t host1AdjSgmSel;                         ///< 0xF0114872 : Host1 Adjust Segment Select
    FlashAdjSgmSel_t flashAdjSgmSel;                         ///< 0xF0114874 : Flash Adjust Segment Select
    ExtendAdjSgmSel_t extendAdjSgmSel;                       ///< 0xF0114876 : Extend Adjust Segment Select
    uint16_t HOST_SEG_SEC_CNT_ADJ;                           ///< 0xF0114878 : Host Segment Sector Count Adjust
    uint8_t rsvd_RF011487A[2];                               ///< 0xF011487A - 0xF011487B : Reserved.
    uint16_t FLASH_SEG_SEC_CNT_ADJ;                          ///< 0xF011487C : Flash Segment Sector Count Adjust
    uint16_t EXTEND_SEG_SEC_CNT_ADJ;                         ///< 0xF011487E : Extend Segment Sector Count Adjust
    uint8_t rsvd_RF0114880[32];                              ///< 0xF0114880 - 0xF011489F : Reserved.
    HostSscCompStat0_t hostSscCompStat0;                     ///< 0xF01148A0 : Host SSC Comp Status 0
    HostSscCompStat1_t hostSscCompStat1;                     ///< 0xF01148A2 : Host SSC Comp Status 1
    uint8_t rsvd_RF01148A4[4];                               ///< 0xF01148A4 - 0xF01148A7 : Reserved.
    FlashSscCompStat0_t flashSscCompStat0;                   ///< 0xF01148A8 : Flash SSC Comp Status 0
    FlashSscCompStat1_t flashSscCompStat1;                   ///< 0xF01148AA : Flash SSC Comp Status 1
    ExtendSscCompStat0_t extendSscCompStat0;                 ///< 0xF01148AC : Extend SSC Comp Status 0
    ExtendSscCompStat1_t extendSscCompStat1;                 ///< 0xF01148AE : Extend SSC Comp Status 1
    HostSscCompIntrMsk0_t hostSscCompIntrMsk0;               ///< 0xF01148B0 : Host SSC Comp Interrupt Mask 0
    HostSscCompIntrMsk1_t hostSscCompIntrMsk1;               ///< 0xF01148B2 : Host SSC Comp Interrupt Mask 1
    uint8_t rsvd_RF01148B4[4];                               ///< 0xF01148B4 - 0xF01148B7 : Reserved.
    FlashSscCompIntrMsk0_t flashSscCompIntrMsk0;             ///< 0xF01148B8 : Flash SSC Comp Interrupt Mask 0
    FlashSscCompIntrMsk1_t flashSscCompIntrMsk1;             ///< 0xF01148BA : Flash SSC Comp Interrupt Mask 1
    ExtendSscCompIntrMsk0_t extendSscCompIntrMsk0;           ///< 0xF01148BC : Extend SSC Comp Interrupt Mask 0
    ExtendSscCompIntrMsk1_t extendSscCompIntrMsk1;           ///< 0xF01148BE : Extend SSC Comp Interrupt Mask 1
    HostMqRamApprovedPtr_t hostMqRamApprovedPtr;             ///< 0xF01148C0 : Host MQ RAM Approved Pointer
    uint8_t rsvd_RF01148C2[2];                               ///< 0xF01148C2 - 0xF01148C3 : Reserved.
    FlashMqRamApprovedPtr_t flashMqRamApprovedPtr;           ///< 0xF01148C4 : Flash MQ RAM Approved Pointer
    ExtendMqRamApprovedPtr_t extendMqRamApprovedPtr;         ///< 0xF01148C6 : Extend MQ RAM Approved Pointer
    HostMqRamSearchPtr_t hostMqRamSearchPtr;                 ///< 0xF01148C8 : Host MQ RAM Search Pointer
    uint8_t rsvd_RF01148CA[2];                               ///< 0xF01148CA - 0xF01148CB : Reserved.
    FlashMqRamSearchPtr_t flashMqRamSearchPtr;               ///< 0xF01148CC : Flash MQ RAM Search Pointer
    ExtendMqRamSearchPtr_t extendMqRamSearchPtr;             ///< 0xF01148CE : Extend MQ RAM Search Pointer
    HostMqRamQueueCnt_t hostMqRamQueueCnt;                   ///< 0xF01148D0 : Host MQ RAM Queue Count
    uint8_t rsvd_RF01148D2[2];                               ///< 0xF01148D2 - 0xF01148D3 : Reserved.
    FlashMqRamQueueCnt_t flashMqRamQueueCnt;                 ///< 0xF01148D4 : Flash MQ RAM Queue Count
    ExtendMqRamQueueCnt_t extendMqRamQueueCnt;               ///< 0xF01148D6 : Extend MQ RAM Queue Count
    uint8_t rsvd_RF01148D8[32];                              ///< 0xF01148D8 - 0xF01148F7 : Reserved.
    BistCtrl_t bistCtrl;                                     ///< 0xF01148F8 : BIST Control
    BistStat_t bistStat;                                     ///< 0xF01148FA : BIST Status
    MqRamTmCtrl_t mqRamTmCtrl;                               ///< 0xF01148FC : MQ RAM Timing Control
    MqRamPowerDnCtrl_t mqRamPowerDnCtrl;                     ///< 0xF01148FE : MQ RAM Power Down Control

    //    ARRAY DECLARATION
    uint16_t HOST_SEG_SIZE[32];
    //    uint16_t HOST_SEG0_SIZE;                                 ///< 0xF0114900 : Host Segment 0 Sector Size
    //    uint16_t HOST_SEG1_SIZE;                                 ///< 0xF0114902 : Host Segment 1 Sector Size
    //    uint16_t HOST_SEG2_SIZE;                                 ///< 0xF0114904 : Host Segment 2 Sector Size
    //    uint16_t HOST_SEG3_SIZE;                                 ///< 0xF0114906 : Host Segment 3 Sector Size
    //    uint16_t HOST_SEG4_SIZE;                                 ///< 0xF0114908 : Host Segment 4 Sector Size
    //    uint16_t HOST_SEG5_SIZE;                                 ///< 0xF011490A : Host Segment 5 Sector Size
    //    uint16_t HOST_SEG6_SIZE;                                 ///< 0xF011490C : Host Segment 6 Sector Size
    //    uint16_t HOST_SEG7_SIZE;                                 ///< 0xF011490E : Host Segment 7 Sector Size
    //    uint16_t HOST_SEG8_SIZE;                                 ///< 0xF0114910 : Host Segment 8 Sector Size
    //    uint16_t HOST_SEG9_SIZE;                                 ///< 0xF0114912 : Host Segment 9 Sector Size
    //    uint16_t HOST_SEG10_SIZE;                                ///< 0xF0114914 : Host Segment 10 Sector Size
    //    uint16_t HOST_SEG11_SIZE;                                ///< 0xF0114916 : Host Segment 11 Sector Size
    //    uint16_t HOST_SEG12_SIZE;                                ///< 0xF0114918 : Host Segment 12 Sector Size
    //    uint16_t HOST_SEG13_SIZE;                                ///< 0xF011491A : Host Segment 13 Sector Size
    //    uint16_t HOST_SEG14_SIZE;                                ///< 0xF011491C : Host Segment 14 Sector Size
    //    uint16_t HOST_SEG15_SIZE;                                ///< 0xF011491E : Host Segment 15 Sector Size
    //    uint16_t HOST_SEG16_SIZE;                                ///< 0xF0114920 : Host Segment 16 Sector Size
    //    uint16_t HOST_SEG17_SIZE;                                ///< 0xF0114922 : Host Segment 17 Sector Size
    //    uint16_t HOST_SEG18_SIZE;                                ///< 0xF0114924 : Host Segment 18 Sector Size
    //    uint16_t HOST_SEG19_SIZE;                                ///< 0xF0114926 : Host Segment 19 Sector Size
    //    uint16_t HOST_SEG20_SIZE;                                ///< 0xF0114928 : Host Segment 20 Sector Size
    //    uint16_t HOST_SEG21_SIZE;                                ///< 0xF011492A : Host Segment 21 Sector Size
    //    uint16_t HOST_SEG22_SIZE;                                ///< 0xF011492C : Host Segment 22 Sector Size
    //    uint16_t HOST_SEG23_SIZE;                                ///< 0xF011492E : Host Segment 23 Sector Size
    //    uint16_t HOST_SEG24_SIZE;                                ///< 0xF0114930 : Host Segment 24 Sector Size
    //    uint16_t HOST_SEG25_SIZE;                                ///< 0xF0114932 : Host Segment 25 Sector Size
    //    uint16_t HOST_SEG26_SIZE;                                ///< 0xF0114934 : Host Segment 26 Sector Size
    //    uint16_t HOST_SEG27_SIZE;                                ///< 0xF0114936 : Host Segment 27 Sector Size
    //    uint16_t HOST_SEG28_SIZE;                                ///< 0xF0114938 : Host Segment 28 Sector Size
    //    uint16_t HOST_SEG29_SIZE;                                ///< 0xF011493A : Host Segment 29 Sector Size
    //    uint16_t HOST_SEG30_SIZE;                                ///< 0xF011493C : Host Segment 30 Sector Size
    //    uint16_t HOST_SEG31_SIZE;                                ///< 0xF011493E : Host Segment 31 Sector Size
    uint8_t rsvd_RF0114940[64];                              ///< 0xF0114940 - 0xF011497F : Reserved.

    //    ARRAY DECLARATION
    uint16_t FLASH_SEG_SIZE[32];
    //    uint16_t FLASH_SEG0_SIZE;                                ///< 0xF0114980 : Flash Segment 0 Sector Size
    //    uint16_t FLASH_SEG1_SIZE;                                ///< 0xF0114982 : Flash Segment 1 Sector Size
    //    uint16_t FLASH_SEG2_SIZE;                                ///< 0xF0114984 : Flash Segment 2 Sector Size
    //    uint16_t FLASH_SEG3_SIZE;                                ///< 0xF0114986 : Flash Segment 3 Sector Size
    //    uint16_t FLASH_SEG4_SIZE;                                ///< 0xF0114988 : Flash Segment 4 Sector Size
    //    uint16_t FLASH_SEG5_SIZE;                                ///< 0xF011498A : Flash Segment 5 Sector Size
    //    uint16_t FLASH_SEG6_SIZE;                                ///< 0xF011498C : Flash Segment 6 Sector Size
    //    uint16_t FLASH_SEG7_SIZE;                                ///< 0xF011498E : Flash Segment 7 Sector Size
    //    uint16_t FLASH_SEG8_SIZE;                                ///< 0xF0114990 : Flash Segment 8 Sector Size
    //    uint16_t FLASH_SEG9_SIZE;                                ///< 0xF0114992 : Flash Segment 9 Sector Size
    //    uint16_t FLASH_SEG10_SIZE;                               ///< 0xF0114994 : Flash Segment 10 Sector Size
    //    uint16_t FLASH_SEG11_SIZE;                               ///< 0xF0114996 : Flash Segment 11 Sector Size
    //    uint16_t FLASH_SEG12_SIZE;                               ///< 0xF0114998 : Flash Segment 12 Sector Size
    //    uint16_t FLASH_SEG13_SIZE;                               ///< 0xF011499A : Flash Segment 13 Sector Size
    //    uint16_t FLASH_SEG14_SIZE;                               ///< 0xF011499C : Flash Segment 14 Sector Size
    //    uint16_t FLASH_SEG15_SIZE;                               ///< 0xF011499E : Flash Segment 15 Sector Size
    //    uint16_t FLASH_SEG16_SIZE;                               ///< 0xF01149A0 : Flash Segment 16 Sector Size
    //    uint16_t FLASH_SEG17_SIZE;                               ///< 0xF01149A2 : Flash Segment 17 Sector Size
    //    uint16_t FLASH_SEG18_SIZE;                               ///< 0xF01149A4 : Flash Segment 18 Sector Size
    //    uint16_t FLASH_SEG19_SIZE;                               ///< 0xF01149A6 : Flash Segment 19 Sector Size
    //    uint16_t FLASH_SEG20_SIZE;                               ///< 0xF01149A8 : Flash Segment 20 Sector Size
    //    uint16_t FLASH_SEG21_SIZE;                               ///< 0xF01149AA : Flash Segment 21 Sector Size
    //    uint16_t FLASH_SEG22_SIZE;                               ///< 0xF01149AC : Flash Segment 22 Sector Size
    //    uint16_t FLASH_SEG23_SIZE;                               ///< 0xF01149AE : Flash Segment 23 Sector Size
    //    uint16_t FLASH_SEG24_SIZE;                               ///< 0xF01149B0 : Flash Segment 24 Sector Size
    //    uint16_t FLASH_SEG25_SIZE;                               ///< 0xF01149B2 : Flash Segment 25 Sector Size
    //    uint16_t FLASH_SEG26_SIZE;                               ///< 0xF01149B4 : Flash Segment 26 Sector Size
    //    uint16_t FLASH_SEG27_SIZE;                               ///< 0xF01149B6 : Flash Segment 27 Sector Size
    //    uint16_t FLASH_SEG28_SIZE;                               ///< 0xF01149B8 : Flash Segment 28 Sector Size
    //    uint16_t FLASH_SEG29_SIZE;                               ///< 0xF01149BA : Flash Segment 29 Sector Size
    //    uint16_t FLASH_SEG30_SIZE;                               ///< 0xF01149BC : Flash Segment 30 Sector Size
    //    uint16_t FLASH_SEG31_SIZE;                               ///< 0xF01149BE : Flash Segment 31 Sector Size

    //    ARRAY DECLARATION
    uint16_t EXTEND_SEG_SIZE[32];
    //    uint16_t EXTEND_SEG0_SIZE;                               ///< 0xF01149C0 : Extend Segment 0 Sector Size
    //    uint16_t EXTEND_SEG1_SIZE;                               ///< 0xF01149C2 : Extend Segment 1 Sector Size
    //    uint16_t EXTEND_SEG2_SIZE;                               ///< 0xF01149C4 : Extend Segment 2 Sector Size
    //    uint16_t EXTEND_SEG3_SIZE;                               ///< 0xF01149C6 : Extend Segment 3 Sector Size
    //    uint16_t EXTEND_SEG4_SIZE;                               ///< 0xF01149C8 : Extend Segment 4 Sector Size
    //    uint16_t EXTEND_SEG5_SIZE;                               ///< 0xF01149CA : Extend Segment 5 Sector Size
    //    uint16_t EXTEND_SEG6_SIZE;                               ///< 0xF01149CC : Extend Segment 6 Sector Size
    //    uint16_t EXTEND_SEG7_SIZE;                               ///< 0xF01149CE : Extend Segment 7 Sector Size
    //    uint16_t EXTEND_SEG8_SIZE;                               ///< 0xF01149D0 : Extend Segment 8 Sector Size
    //    uint16_t EXTEND_SEG9_SIZE;                               ///< 0xF01149D2 : Extend Segment 9 Sector Size
    //    uint16_t EXTEND_SEG10_SIZE;                              ///< 0xF01149D4 : Extend Segment 10 Sector Size
    //    uint16_t EXTEND_SEG11_SIZE;                              ///< 0xF01149D6 : Extend Segment 11 Sector Size
    //    uint16_t EXTEND_SEG12_SIZE;                              ///< 0xF01149D8 : Extend Segment 12 Sector Size
    //    uint16_t EXTEND_SEG13_SIZE;                              ///< 0xF01149DA : Extend Segment 13 Sector Size
    //    uint16_t EXTEND_SEG14_SIZE;                              ///< 0xF01149DC : Extend Segment 14 Sector Size
    //    uint16_t EXTEND_SEG15_SIZE;                              ///< 0xF01149DE : Extend Segment 15 Sector Size
    //    uint16_t EXTEND_SEG16_SIZE;                              ///< 0xF01149E0 : Extend Segment 16 Sector Size
    //    uint16_t EXTEND_SEG17_SIZE;                              ///< 0xF01149E2 : Extend Segment 17 Sector Size
    //    uint16_t EXTEND_SEG18_SIZE;                              ///< 0xF01149E4 : Extend Segment 18 Sector Size
    //    uint16_t EXTEND_SEG19_SIZE;                              ///< 0xF01149E6 : Extend Segment 19 Sector Size
    //    uint16_t EXTEND_SEG20_SIZE;                              ///< 0xF01149E8 : Extend Segment 20 Sector Size
    //    uint16_t EXTEND_SEG21_SIZE;                              ///< 0xF01149EA : Extend Segment 21 Sector Size
    //    uint16_t EXTEND_SEG22_SIZE;                              ///< 0xF01149EC : Extend Segment 22 Sector Size
    //    uint16_t EXTEND_SEG23_SIZE;                              ///< 0xF01149EE : Extend Segment 23 Sector Size
    //    uint16_t EXTEND_SEG24_SIZE;                              ///< 0xF01149F0 : Extend Segment 24 Sector Size
    //    uint16_t EXTEND_SEG25_SIZE;                              ///< 0xF01149F2 : Extend Segment 25 Sector Size
    //    uint16_t EXTEND_SEG26_SIZE;                              ///< 0xF01149F4 : Extend Segment 26 Sector Size
    //    uint16_t EXTEND_SEG27_SIZE;                              ///< 0xF01149F6 : Extend Segment 27 Sector Size
    //    uint16_t EXTEND_SEG28_SIZE;                              ///< 0xF01149F8 : Extend Segment 28 Sector Size
    //    uint16_t EXTEND_SEG29_SIZE;                              ///< 0xF01149FA : Extend Segment 29 Sector Size
    //    uint16_t EXTEND_SEG30_SIZE;                              ///< 0xF01149FC : Extend Segment 30 Sector Size
    //    uint16_t EXTEND_SEG31_SIZE;                              ///< 0xF01149FE : Extend Segment 31 Sector Size

    //    ARRAY DECLARATION
    uint16_t HOST_SEG_SSC[32];
    //    uint16_t HOST_SEG0_SSC;                                  ///< 0xF0114A00 : Host Segment 0 Sector Count
    //    uint16_t HOST_SEG1_SSC;                                  ///< 0xF0114A02 : Host Segment 1 Sector Count
    //    uint16_t HOST_SEG2_SSC;                                  ///< 0xF0114A04 : Host Segment 2 Sector Count
    //    uint16_t HOST_SEG3_SSC;                                  ///< 0xF0114A06 : Host Segment 3 Sector Count
    //    uint16_t HOST_SEG4_SSC;                                  ///< 0xF0114A08 : Host Segment 4 Sector Count
    //    uint16_t HOST_SEG5_SSC;                                  ///< 0xF0114A0A : Host Segment 5 Sector Count
    //    uint16_t HOST_SEG6_SSC;                                  ///< 0xF0114A0C : Host Segment 6 Sector Count
    //    uint16_t HOST_SEG7_SSC;                                  ///< 0xF0114A0E : Host Segment 7 Sector Count
    //    uint16_t HOST_SEG8_SSC;                                  ///< 0xF0114A10 : Host Segment 8 Sector Count
    //    uint16_t HOST_SEG9_SSC;                                  ///< 0xF0114A12 : Host Segment 9 Sector Count
    //    uint16_t HOST_SEG10_SSC;                                 ///< 0xF0114A14 : Host Segment 10 Sector Count
    //    uint16_t HOST_SEG11_SSC;                                 ///< 0xF0114A16 : Host Segment 11 Sector Count
    //    uint16_t HOST_SEG12_SSC;                                 ///< 0xF0114A18 : Host Segment 12 Sector Count
    //    uint16_t HOST_SEG13_SSC;                                 ///< 0xF0114A1A : Host Segment 13 Sector Count
    //    uint16_t HOST_SEG14_SSC;                                 ///< 0xF0114A1C : Host Segment 14 Sector Count
    //    uint16_t HOST_SEG15_SSC;                                 ///< 0xF0114A1E : Host Segment 15 Sector Count
    //    uint16_t HOST_SEG16_SSC;                                 ///< 0xF0114A20 : Host Segment 16 Sector Count
    //    uint16_t HOST_SEG17_SSC;                                 ///< 0xF0114A22 : Host Segment 17 Sector Count
    //    uint16_t HOST_SEG18_SSC;                                 ///< 0xF0114A24 : Host Segment 18 Sector Count
    //    uint16_t HOST_SEG19_SSC;                                 ///< 0xF0114A26 : Host Segment 19 Sector Count
    //    uint16_t HOST_SEG20_SSC;                                 ///< 0xF0114A28 : Host Segment 20 Sector Count
    //    uint16_t HOST_SEG21_SSC;                                 ///< 0xF0114A2A : Host Segment 21 Sector Count
    //    uint16_t HOST_SEG22_SSC;                                 ///< 0xF0114A2C : Host Segment 22 Sector Count
    //    uint16_t HOST_SEG23_SSC;                                 ///< 0xF0114A2E : Host Segment 23 Sector Count
    //    uint16_t HOST_SEG24_SSC;                                 ///< 0xF0114A30 : Host Segment 24 Sector Count
    //    uint16_t HOST_SEG25_SSC;                                 ///< 0xF0114A32 : Host Segment 25 Sector Count
    //    uint16_t HOST_SEG26_SSC;                                 ///< 0xF0114A34 : Host Segment 26 Sector Count
    //    uint16_t HOST_SEG27_SSC;                                 ///< 0xF0114A36 : Host Segment 27 Sector Count
    //    uint16_t HOST_SEG28_SSC;                                 ///< 0xF0114A38 : Host Segment 28 Sector Count
    //    uint16_t HOST_SEG29_SSC;                                 ///< 0xF0114A3A : Host Segment 29 Sector Count
    //    uint16_t HOST_SEG30_SSC;                                 ///< 0xF0114A3C : Host Segment 30 Sector Count
    //    uint16_t HOST_SEG31_SSC;                                 ///< 0xF0114A3E : Host Segment 31 Sector Count
    uint8_t rsvd_RF0114A40[64];                              ///< 0xF0114A40 - 0xF0114A7F : Reserved.

    //    ARRAY DECLARATION
    uint16_t FLASH_SEG_SSC[32];
    //    uint16_t FLASH_SEG0_SSC;                                 ///< 0xF0114A80 : Flash Segment 0 Sector Count
    //    uint16_t FLASH_SEG1_SSC;                                 ///< 0xF0114A82 : Flash Segment 1 Sector Count
    //    uint16_t FLASH_SEG2_SSC;                                 ///< 0xF0114A84 : Flash Segment 2 Sector Count
    //    uint16_t FLASH_SEG3_SSC;                                 ///< 0xF0114A86 : Flash Segment 3 Sector Count
    //    uint16_t FLASH_SEG4_SSC;                                 ///< 0xF0114A88 : Flash Segment 4 Sector Count
    //    uint16_t FLASH_SEG5_SSC;                                 ///< 0xF0114A8A : Flash Segment 5 Sector Count
    //    uint16_t FLASH_SEG6_SSC;                                 ///< 0xF0114A8C : Flash Segment 6 Sector Count
    //    uint16_t FLASH_SEG7_SSC;                                 ///< 0xF0114A8E : Flash Segment 7 Sector Count
    //    uint16_t FLASH_SEG8_SSC;                                 ///< 0xF0114A90 : Flash Segment 8 Sector Count
    //    uint16_t FLASH_SEG9_SSC;                                 ///< 0xF0114A92 : Flash Segment 9 Sector Count
    //    uint16_t FLASH_SEG10_SSC;                                ///< 0xF0114A94 : Flash Segment 10 Sector Count
    //    uint16_t FLASH_SEG11_SSC;                                ///< 0xF0114A96 : Flash Segment 11 Sector Count
    //    uint16_t FLASH_SEG12_SSC;                                ///< 0xF0114A98 : Flash Segment 12 Sector Count
    //    uint16_t FLASH_SEG13_SSC;                                ///< 0xF0114A9A : Flash Segment 13 Sector Count
    //    uint16_t FLASH_SEG14_SSC;                                ///< 0xF0114A9C : Flash Segment 14 Sector Count
    //    uint16_t FLASH_SEG15_SSC;                                ///< 0xF0114A9E : Flash Segment 15 Sector Count
    //    uint16_t FLASH_SEG16_SSC;                                ///< 0xF0114AA0 : Flash Segment 16 Sector Count
    //    uint16_t FLASH_SEG17_SSC;                                ///< 0xF0114AA2 : Flash Segment 17 Sector Count
    //    uint16_t FLASH_SEG18_SSC;                                ///< 0xF0114AA4 : Flash Segment 18 Sector Count
    //    uint16_t FLASH_SEG19_SSC;                                ///< 0xF0114AA6 : Flash Segment 19 Sector Count
    //    uint16_t FLASH_SEG20_SSC;                                ///< 0xF0114AA8 : Flash Segment 20 Sector Count
    //    uint16_t FLASH_SEG21_SSC;                                ///< 0xF0114AAA : Flash Segment 21 Sector Count
    //    uint16_t FLASH_SEG22_SSC;                                ///< 0xF0114AAC : Flash Segment 22 Sector Count
    //    uint16_t FLASH_SEG23_SSC;                                ///< 0xF0114AAE : Flash Segment 23 Sector Count
    //    uint16_t FLASH_SEG24_SSC;                                ///< 0xF0114AB0 : Flash Segment 24 Sector Count
    //    uint16_t FLASH_SEG25_SSC;                                ///< 0xF0114AB2 : Flash Segment 25 Sector Count
    //    uint16_t FLASH_SEG26_SSC;                                ///< 0xF0114AB4 : Flash Segment 26 Sector Count
    //    uint16_t FLASH_SEG27_SSC;                                ///< 0xF0114AB6 : Flash Segment 27 Sector Count
    //    uint16_t FLASH_SEG28_SSC;                                ///< 0xF0114AB8 : Flash Segment 28 Sector Count
    //    uint16_t FLASH_SEG29_SSC;                                ///< 0xF0114ABA : Flash Segment 29 Sector Count
    //    uint16_t FLASH_SEG30_SSC;                                ///< 0xF0114ABC : Flash Segment 30 Sector Count
    //    uint16_t FLASH_SEG31_SSC;                                ///< 0xF0114ABE : Flash Segment 31 Sector Count

    //    ARRAY DECLARATION
    uint16_t EXTEND_SEG_SSC[32];
    //    uint16_t EXTEND_SEG0_SSC;                                ///< 0xF0114AC0 : Extend Segment 0 Sector Count
    //    uint16_t EXTEND_SEG1_SSC;                                ///< 0xF0114AC2 : Extend Segment 1 Sector Count
    //    uint16_t EXTEND_SEG2_SSC;                                ///< 0xF0114AC4 : Extend Segment 2 Sector Count
    //    uint16_t EXTEND_SEG3_SSC;                                ///< 0xF0114AC6 : Extend Segment 3 Sector Count
    //    uint16_t EXTEND_SEG4_SSC;                                ///< 0xF0114AC8 : Extend Segment 4 Sector Count
    //    uint16_t EXTEND_SEG5_SSC;                                ///< 0xF0114ACA : Extend Segment 5 Sector Count
    //    uint16_t EXTEND_SEG6_SSC;                                ///< 0xF0114ACC : Extend Segment 6 Sector Count
    //    uint16_t EXTEND_SEG7_SSC;                                ///< 0xF0114ACE : Extend Segment 7 Sector Count
    //    uint16_t EXTEND_SEG8_SSC;                                ///< 0xF0114AD0 : Extend Segment 8 Sector Count
    //    uint16_t EXTEND_SEG9_SSC;                                ///< 0xF0114AD2 : Extend Segment 9 Sector Count
    //    uint16_t EXTEND_SEG10_SSC;                               ///< 0xF0114AD4 : Extend Segment 10 Sector Count
    //    uint16_t EXTEND_SEG11_SSC;                               ///< 0xF0114AD6 : Extend Segment 11 Sector Count
    //    uint16_t EXTEND_SEG12_SSC;                               ///< 0xF0114AD8 : Extend Segment 12 Sector Count
    //    uint16_t EXTEND_SEG13_SSC;                               ///< 0xF0114ADA : Extend Segment 13 Sector Count
    //    uint16_t EXTEND_SEG14_SSC;                               ///< 0xF0114ADC : Extend Segment 14 Sector Count
    //    uint16_t EXTEND_SEG15_SSC;                               ///< 0xF0114ADE : Extend Segment 15 Sector Count
    //    uint16_t EXTEND_SEG16_SSC;                               ///< 0xF0114AE0 : Extend Segment 16 Sector Count
    //    uint16_t EXTEND_SEG17_SSC;                               ///< 0xF0114AE2 : Extend Segment 17 Sector Count
    //    uint16_t EXTEND_SEG18_SSC;                               ///< 0xF0114AE4 : Extend Segment 18 Sector Count
    //    uint16_t EXTEND_SEG19_SSC;                               ///< 0xF0114AE6 : Extend Segment 19 Sector Count
    //    uint16_t EXTEND_SEG20_SSC;                               ///< 0xF0114AE8 : Extend Segment 20 Sector Count
    //    uint16_t EXTEND_SEG21_SSC;                               ///< 0xF0114AEA : Extend Segment 21 Sector Count
    //    uint16_t EXTEND_SEG22_SSC;                               ///< 0xF0114AEC : Extend Segment 22 Sector Count
    //    uint16_t EXTEND_SEG23_SSC;                               ///< 0xF0114AEE : Extend Segment 23 Sector Count
    //    uint16_t EXTEND_SEG24_SSC;                               ///< 0xF0114AF0 : Extend Segment 24 Sector Count
    //    uint16_t EXTEND_SEG25_SSC;                               ///< 0xF0114AF2 : Extend Segment 25 Sector Count
    //    uint16_t EXTEND_SEG26_SSC;                               ///< 0xF0114AF4 : Extend Segment 26 Sector Count
    //    uint16_t EXTEND_SEG27_SSC;                               ///< 0xF0114AF6 : Extend Segment 27 Sector Count
    //    uint16_t EXTEND_SEG28_SSC;                               ///< 0xF0114AF8 : Extend Segment 28 Sector Count
    //    uint16_t EXTEND_SEG29_SSC;                               ///< 0xF0114AFA : Extend Segment 29 Sector Count
    //    uint16_t EXTEND_SEG30_SSC;                               ///< 0xF0114AFC : Extend Segment 30 Sector Count
    //    uint16_t EXTEND_SEG31_SSC;                               ///< 0xF0114AFE : Extend Segment 31 Sector Count
    uint16_t HOST_EXP_SEG_SSC[32];                                 ///< 0xF0114B00 : Host Expected Segment 0 Sector Count
    uint8_t rsvd_RF0114B40[64];                                    ///< 0xF0114B40 - 0xF0114B7F : Reserved.
    uint16_t FLASH_EXP_SEG_SSC[32];                                ///< 0xF0114B80 : Flash Expected Segment 0 Sector Count
    uint8_t rsvd_RF0114BBE[2];                                     ///< 0xF0114BBE - 0xF0114BBF : Reserved.
    uint16_t EXTEND_EXP_SEG_SSC[32];                               ///< 0xF0114BC0 : Extend Expected Segment 0 Sector Count
} SegStr_t;
COMPILE_ASSERT(offsetof(SegStr_t,EXTEND_SEG_SSC[31])==0xF0114AFE-0xF0114800,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile SegStr_t rSegStr;                    ///< 0xF0114800-0xF0114B00 : SegStr_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------

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
//! @brief FlashLink List Controller Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 2/19/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  FlashLink List Controller Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF200B800 : SSD Match or Stop Interrupt Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PTR_MATCH_INT_MSK      :16;         ///<BIT [15:0] Subsegment Descriptor Index Match Interrupt Mask.
        uint32_t PTR_MATCH_STOP_INT_MSK : 1;         ///<BIT [16] Subsegment Descriptor Index Match Stop Interrupt Mask.
        uint32_t RF200B800_31_17_RSVD   :15;         ///<BIT [31:17] Reserved.
    } b;    
} SsdMatchOrStopIntrMsk_t;

///  @brief 0xF200B804 : SSD Match or Stop Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PTR_MATCH_STAT      :16;         ///<BIT [15:0] Subsegment Descriptor Match Status of Segment.
        uint32_t PTR_MATCH_STOP_STAT :16;         ///<BIT [31:16] Subsegment Descriptor Index Match Stop Status of Segments.
    } b;    
} SsdMatchOrStopStat_t;

///  @brief 0xF200B808 : Block Reset or Test Monitor Selection
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B808_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RF200B808_30_16_RSVD :15;         ///<BIT [30:16] Reserved.
        uint32_t FLC_RST              : 1;         ///<BIT [31] Flash Link Controller Block Software Reset.
    } b;    
} BlkRstOrTstMonitorSelectio_t;

///  @brief 0xF200B80C : Stop Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B80C_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t STOP_LLIST_PNT      :16;         ///<BIT [31:16] Stop Linked List Pointer.
    } b;    
} StopLnkListPtr_t;

///  @brief 0xF200B810 : Current Read Subsegment Descriptor [31:0] or Next Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B810_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t RD_NXT_LINK_PTR     :16;         ///<BIT [31:16] Next Linked List Pointer for Current Read Subsegment.
    } b;    
} CurRdSubsegmeDescript310OrNxt_t;

///  @brief 0xF200B820 : Current Write Subsegment Descriptor [31:0] or Next Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B820_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t WR_NXT_LLIST_PTR    :16;         ///<BIT [31:16] Write Next Linked List Pointer.
    } b;    
} CurWrSubsegmeDescript310OrNxt_t;

///  @brief Segment Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RESERVED           :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG_LLIST_PTR      :16;         ///<BIT [31:16] Segment 0 Linked List Pointer.
    } b;
} SgmLnkListPtr_t;

///  @brief 0xF200B870 : Segment 0 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B870_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG0_LLIST_PTR      :16;         ///<BIT [31:16] Segment 0 Linked List Pointer.
    } b;    
} Sgm0LnkListPtr_t;

///  @brief 0xF200B874 : Segment 1 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B874_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG1_LLIST_PTR      :16;         ///<BIT [31:16] Segment 1 Linked List Pointer.
    } b;    
} Sgm1LnkListPtr_t;

///  @brief 0xF200B878 : Segment 2 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B878_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG2_LLIST_PTR      :16;         ///<BIT [31:16] Segment 2 Linked List Pointer.
    } b;    
} Sgm2LnkListPtr_t;

///  @brief 0xF200B87C : Segment 3 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B87C_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG3_LLIST_PTR      :16;         ///<BIT [31:16] Segment 3 Linked List Pointer.
    } b;    
} Sgm3LnkListPtr_t;

///  @brief 0xF200B880 : Segment 4 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B880_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG4_LLIST_PTR      :16;         ///<BIT [31:16] Segment 4 Linked List Pointer.
    } b;    
} Sgm4LnkListPtr_t;

///  @brief 0xF200B884 : Segment 5 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B884_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG5_LLIST_PTR      :16;         ///<BIT [31:16] Segment 5 Linked List Pointer.
    } b;    
} Sgm5LnkListPtr_t;

///  @brief 0xF200B888 : Segment 6 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B888_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG6_LLIST_PTR      :16;         ///<BIT [31:16] Segment 6 Linked List Pointer.
    } b;    
} Sgm6LnkListPtr_t;

///  @brief 0xF200B88C : Segment 7 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B88C_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG7_LLIST_PTR      :16;         ///<BIT [31:16] Segment 7 Linked List Pointer.
    } b;    
} Sgm7LnkListPtr_t;

///  @brief 0xF200B890 : Segment 8 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B890_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG8_LLIST_PTR      :16;         ///<BIT [31:16] Segment 8 Linked List Pointer.
    } b;    
} Sgm8LnkListPtr_t;

///  @brief 0xF200B894 : Segment 9 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B894_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG9_LLIST_PTR      :16;         ///<BIT [31:16] Segment 9 Linked List Pointer.
    } b;    
} Sgm9LnkListPtr_t;

///  @brief 0xF200B898 : Segment 10 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B898_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG10_LLIST_PTR     :16;         ///<BIT [31:16] Segment 10 Linked List Pointer.
    } b;    
} Sgm10LnkListPtr_t;

///  @brief 0xF200B89C : Segment 11 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B89C_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG11_LLIST_PTR     :16;         ///<BIT [31:16] Segment 11 Linked List Pointer.
    } b;    
} Sgm11LnkListPtr_t;

///  @brief 0xF200B8A0 : Segment 12 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B8A0_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG12_LLIST_PTR     :16;         ///<BIT [31:16] Segment 12 Linked List Pointer.
    } b;    
} Sgm12LnkListPtr_t;

///  @brief 0xF200B8A4 : Segment 13 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B8A4_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG13_LLIST_PTR     :16;         ///<BIT [31:16] Segment 13 Linked List Pointer.
    } b;    
} Sgm13LnkListPtr_t;

///  @brief 0xF200B8A8 : Segment 14 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B8A8_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG14_LLIST_PTR     :16;         ///<BIT [31:16] Segment 14 Linked List Pointer.
    } b;    
} Sgm14LnkListPtr_t;

///  @brief 0xF200B8AC : Segment 15 Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B8AC_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG15_LLIST_PTR     :16;         ///<BIT [31:16] Segment 15 Linked List Pointer.
    } b;    
} Sgm15LnkListPtr_t;

///  @brief 0xF200B8B0 : Segment Valid Transfer Pointer Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B8B0_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG_VALID_TP_STS    :16;         ///<BIT [31:16] Valid Transfer Pointer Status per Segment.
    } b;    
} SgmValidXfrPtrStat_t;

///  @brief 0xF200B8BC : Subsegment Descriptor Mode
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B8BC_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t SSD_SIZE_MODE        : 1;         ///<BIT [16] SSD Entry Size Mode.
        uint32_t RF200B8BC_17_RSVD    : 1;         ///<BIT [17] Reserved.
        uint32_t SSD_ADDR_MODE        : 1;         ///<BIT [18] Subsegment Descriptor Address Mode.
        uint32_t SSD_END_ADDR_MODE    : 1;         ///<BIT [19] Subsegment Descriptor End Address Mode.
        uint32_t SSD_LEN_MODE         : 1;         ///<BIT [20] Subsegment Descriptor Length Mode.
        uint32_t SSD_CNT_RD_EN        : 1;         ///<BIT [21] Subsegment Descriptor Counter Enable for Read.
        uint32_t SSD_CNT_WR_EN        : 1;         ///<BIT [22] Subsegment Descriptor Counter Enable for Write.
        uint32_t RF200B8BC_23_RSVD    : 1;         ///<BIT [23] Reserved.
        uint32_t PUSH_ACK_DIS         : 1;         ///<BIT [24] ECC Push Ack Disable.
        uint32_t RF200B8BC_31_25_RSVD : 7;         ///<BIT [31:25] Reserved.
    } b;    
} SubsegmeDescriptMode_t;

///  @brief 0xF200B8C0 : Segment 0 Expected Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B8C0_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG0_EXP_LLIST_PTR  :16;         ///<BIT [31:16] Segment 0 Expected Linked List Pointer.
    } b;    
} Sgm0ExpectedLnkListPtr_t;

///  @brief 0xF200B8C4 : Segment 1 Expected Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B8C4_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG1_EXP_LLIST_PTR  :16;         ///<BIT [31:16] Segment 1 Expected Linked List Pointer.
    } b;    
} Sgm1ExpectedLnkListPtr_t;

///  @brief 0xF200B8FC : Segment 15 Expected Link List Pointer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B8FC_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t SEG15_EXP_LLIST_PTR :16;         ///<BIT [31:16] Segment 15 Expected Linked List Pointer.
    } b;    
} Sgm15ExpectedLnkListPtr_t;

///  @brief 0xF200B920 : Subsegment Descriptor Counter Delta for Read Path
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B920_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t RD_SSD_CNT_DLT       : 8;         ///<BIT [23:16] Subsegment Descriptor Counter Delta for Read Path.
        uint32_t RF200B920_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} SubsegmeDescriptCntrDeltaForRdPath_t;

///  @brief 0xF200B924 : Subsegment Descriptor Counter Delta for Write Path
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B924_15_0_RSVD  :16;         ///<BIT [15:0] Reserved.
        uint32_t WR_SSD_CNT_DLT       : 8;         ///<BIT [23:16] Subsegment Descriptor Counter Delta for Write Path.
        uint32_t RF200B924_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} SubsegmeDescriptCntrDeltaForWrPath_t;

///  @brief 0xF200B928 : Subsegment Descriptor Counter for Read Path
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B928_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t RD_SSD_CNT          :16;         ///<BIT [31:16] Subsegment Descriptor Counter for Read.
    } b;    
} SubsegmeDescriptCntrForRdPath_t;

///  @brief 0xF200B92C : Subsegment Descriptor Counter for Write Path
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF200B92C_15_0_RSVD :16;         ///<BIT [15:0] Reserved.
        uint32_t WR_SSD_CNT          :16;         ///<BIT [31:16] Subsegment Descriptor Counter for Write.
    } b;    
} SubsegmeDescriptCntrForWrPath_t;


/// @brief 0xF200B800-0xF200B930 : FlashLlc_t MODULE
typedef struct
{
    SsdMatchOrStopIntrMsk_t ssdMatchOrStopIntrMsk;                               ///< 0xF200B800 : SSD Match or Stop Interrupt Mask
    SsdMatchOrStopStat_t ssdMatchOrStopStat;                                     ///< 0xF200B804 : SSD Match or Stop Status
    BlkRstOrTstMonitorSelectio_t blkRstOrTstMonitorSelectio;                     ///< 0xF200B808 : Block Reset or Test Monitor Selection
    StopLnkListPtr_t stopLnkListPtr;                                             ///< 0xF200B80C : Stop Link List Pointer
    CurRdSubsegmeDescript310OrNxt_t curRdSubsegmeDescript310OrNxt;               ///< 0xF200B810 : Current Read Subsegment Descriptor [31:0] or Next Pointer
    uint32_t CUR_RD_START_ADDR;                                                  ///< 0xF200B814 : Current Read Subsegment Descriptor [63:32] or Start Address
    uint32_t CUR_RD_END_ADDR;                                                    ///< 0xF200B818 : Current Read Subsegment Descriptor [95:64] or End Address
    uint8_t rsvd_RF200B81C[4];                                                   ///< 0xF200B81C - 0xF200B81F : Reserved.
    CurWrSubsegmeDescript310OrNxt_t curWrSubsegmeDescript310OrNxt;               ///< 0xF200B820 : Current Write Subsegment Descriptor [31:0] or Next Pointer
    uint32_t CUR_WR_START_ADDR;                                                  ///< 0xF200B824 : Current Write Subsegment Descriptor [63:32] or Start Address
    uint32_t CUR_WR_END_ADDR;                                                    ///< 0xF200B828 : Current Write Subsegment Descriptor [95:64] or End Address
    uint32_t RF200B82C_31_0_RSVD;                                                ///< 0xF200B82C : Current Write Subsegment Descriptor [127:96]

    //    ARRAY DECLARATION
    uint32_t sgmXfr[16];
    //    uint32_t SEG0_XFR_PTR;                                                       ///< 0xF200B830 : Segment 0 Transfer Pointer
    //    uint32_t SEG1_XFR_PTR;                                                       ///< 0xF200B834 : Segment 1 Transfer Pointer
    //    uint32_t SEG2_XFR_PTR;                                                       ///< 0xF200B838 : Segment 2 Transfer Pointer
    //    uint32_t SEG3_XFR_PTR;                                                       ///< 0xF200B83C : Segment 3 Transfer Pointer
    //    uint32_t SEG4_XFR_PTR;                                                       ///< 0xF200B840 : Segment 4 Transfer Pointer
    //    uint32_t SEG5_XFR_PTR;                                                       ///< 0xF200B844 : Segment 5 Transfer Pointer
    //    uint32_t SEG6_XFR_PTR;                                                       ///< 0xF200B848 : Segment 6 Transfer Pointer
    //    uint32_t SEG7_XFR_PTR;                                                       ///< 0xF200B84C : Segment 7 Transfer Pointer
    //    uint32_t SEG8_XFR_PTR;                                                       ///< 0xF200B850 : Segment 8 Transfer Pointer
    //    uint32_t SEG9_XFR_PTR;                                                       ///< 0xF200B854 : Segment 9 Transfer Pointer
    //    uint32_t SEG10_XFR_PTR;                                                      ///< 0xF200B858 : Segment 10 Transfer Pointer
    //    uint32_t SEG11_XFR_PTR;                                                      ///< 0xF200B85C : Segment 11 Transfer Pointer
    //    uint32_t SEG12_XFR_PTR;                                                      ///< 0xF200B860 : Segment 12 Transfer Pointer
    //    uint32_t SEG13_XFR_PTR;                                                      ///< 0xF200B864 : Segment 13 Transfer Pointer
    //    uint32_t SEG14_XFR_PTR;                                                      ///< 0xF200B868 : Segment 14 Transfer Pointer
    //    uint32_t SEG15_XFR_PTR;                                                      ///< 0xF200B86C : Segment 15 Transfer Pointer

    //    ARRAY DECLARATION
    SgmLnkListPtr_t sgmLnkListPtr[16];
    //    Sgm0LnkListPtr_t sgm0LnkListPtr;                                             ///< 0xF200B870 : Segment 0 Link List Pointer
    //    Sgm1LnkListPtr_t sgm1LnkListPtr;                                             ///< 0xF200B874 : Segment 1 Link List Pointer
    //    Sgm2LnkListPtr_t sgm2LnkListPtr;                                             ///< 0xF200B878 : Segment 2 Link List Pointer
    //    Sgm3LnkListPtr_t sgm3LnkListPtr;                                             ///< 0xF200B87C : Segment 3 Link List Pointer
    //    Sgm4LnkListPtr_t sgm4LnkListPtr;                                             ///< 0xF200B880 : Segment 4 Link List Pointer
    //    Sgm5LnkListPtr_t sgm5LnkListPtr;                                             ///< 0xF200B884 : Segment 5 Link List Pointer
    //    Sgm6LnkListPtr_t sgm6LnkListPtr;                                             ///< 0xF200B888 : Segment 6 Link List Pointer
    //    Sgm7LnkListPtr_t sgm7LnkListPtr;                                             ///< 0xF200B88C : Segment 7 Link List Pointer
    //    Sgm8LnkListPtr_t sgm8LnkListPtr;                                             ///< 0xF200B890 : Segment 8 Link List Pointer
    //    Sgm9LnkListPtr_t sgm9LnkListPtr;                                             ///< 0xF200B894 : Segment 9 Link List Pointer
    //    Sgm10LnkListPtr_t sgm10LnkListPtr;                                           ///< 0xF200B898 : Segment 10 Link List Pointer
    //    Sgm11LnkListPtr_t sgm11LnkListPtr;                                           ///< 0xF200B89C : Segment 11 Link List Pointer
    //    Sgm12LnkListPtr_t sgm12LnkListPtr;                                           ///< 0xF200B8A0 : Segment 12 Link List Pointer
    //    Sgm13LnkListPtr_t sgm13LnkListPtr;                                           ///< 0xF200B8A4 : Segment 13 Link List Pointer
    //    Sgm14LnkListPtr_t sgm14LnkListPtr;                                           ///< 0xF200B8A8 : Segment 14 Link List Pointer
    //    Sgm15LnkListPtr_t sgm15LnkListPtr;                                           ///< 0xF200B8AC : Segment 15 Link List Pointer
    SgmValidXfrPtrStat_t sgmValidXfrPtrStat;                                     ///< 0xF200B8B0 : Segment Valid Transfer Pointer Status
    uint32_t RD_SSD_START_ADDR;                                                  ///< 0xF200B8B4 : Read Subsegment Descriptor Table Start Address
    uint32_t WR_SSD_START_ADDR;                                                  ///< 0xF200B8B8 : Write SSD Table Start Address
    SubsegmeDescriptMode_t subsegmeDescriptMode;                                 ///< 0xF200B8BC : Subsegment Descriptor Mode
    Sgm0ExpectedLnkListPtr_t sgm0ExpectedLnkListPtr;                             ///< 0xF200B8C0 : Segment 0 Expected Link List Pointer
    Sgm1ExpectedLnkListPtr_t sgm1ExpectedLnkListPtr;                             ///< 0xF200B8C4 : Segment 1 Expected Link List Pointer
    uint8_t rsvd_RF200B8C8[52];                                                  ///< 0xF200B8C8 - 0xF200B8FB : Reserved.
    Sgm15ExpectedLnkListPtr_t sgm15ExpectedLnkListPtr;                           ///< 0xF200B8FC : Segment 15 Expected Link List Pointer
    uint8_t rsvd_RF200B900[32];                                                  ///< 0xF200B900 - 0xF200B91F : Reserved.
    SubsegmeDescriptCntrDeltaForRdPath_t subsegmeDescriptCntrDeltaForRdPath;     ///< 0xF200B920 : Subsegment Descriptor Counter Delta for Read Path
    SubsegmeDescriptCntrDeltaForWrPath_t subsegmeDescriptCntrDeltaForWrPath;     ///< 0xF200B924 : Subsegment Descriptor Counter Delta for Write Path
    SubsegmeDescriptCntrForRdPath_t subsegmeDescriptCntrForRdPath;               ///< 0xF200B928 : Subsegment Descriptor Counter for Read Path
    SubsegmeDescriptCntrForWrPath_t subsegmeDescriptCntrForWrPath;               ///< 0xF200B92C : Subsegment Descriptor Counter for Write Path
} FlashLlc_t;
COMPILE_ASSERT(offsetof(FlashLlc_t,subsegmeDescriptCntrForWrPath)==0xF200B92C-0xF200B800,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile FlashLlc_t rFlashLlc;                ///< 0xF200B800-0xF200B930 : FlashLlc_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
extern FlashLlc_t mFlashLlc;                ///< 0xF200B800-0xF200B930 : FlashLlc_t MODULE

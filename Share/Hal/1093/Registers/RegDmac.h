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
//! @brief DMA Controller Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  DMA Controller Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF010A000 : Indirect Access Index
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IND_ACC_IDX          :16;         ///<BIT [15:0] Indirect Access Index.
        uint32_t RF010A000_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} IndAccessIndex_t;

///  @brief 0xF010A008 : Command 01
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DMAC_SDRAM_TST_STRT_CMD : 1;         ///<BIT [0] DMAC/SDRAM Test Start Command.
        uint32_t DMAC_SDRAM_TST_DMA_EN   : 1;         ///<BIT [1] DMAC/SDRAM Test DMA Enable.
        uint32_t FUNC_SEL                : 3;         ///<BIT [4:2] Function Select.
        uint32_t RF010A008_5_RSVD        : 1;         ///<BIT [5] Reserved.
        uint32_t IGNR_MISCMP             : 1;         ///<BIT [6] Ignore Miscompare.
        uint32_t DIS_NEXT_ADDR_CMP       : 1;         ///<BIT [7] Disable Next Address Compare.
        uint32_t SDRAM_TST_EN            : 1;         ///<BIT [8] SDRAM Test Enable.
        uint32_t SDRAM_TST_LBA_CRC_FMT   : 2;         ///<BIT [10:9] SDRAM Test LBA/CRC Format.
        uint32_t SDRAM_TST_PTRN          : 2;         ///<BIT [12:11] SDRAM Test Pattern.
        uint32_t SDRAM_TST_LBA_INC_DIS   : 1;         ///<BIT [13] SDRAM Test LBA Increment Disable.
        uint32_t SDRAM_TST_WR_EN         : 1;         ///<BIT [14] SDRAM Test Write Enable.
        uint32_t RF010A008_15_RSVD       : 1;         ///<BIT [15] Reserved.
        uint32_t RF010A00A_5_0_RSVD      : 6;         ///<BIT [21:16] Reserved.
        uint32_t IGNR_CRC_ERR            : 1;         ///<BIT [22] Ignore CRC Error.
        uint32_t IGNR_MPCC_ERR           : 1;         ///<BIT [23] Ignore MPECC Error.
        uint32_t IGNR_LBA_ERR            : 1;         ///<BIT [24] Ignore LBA Error.
        uint32_t RF010A00A_15_9_RSVD     : 7;         ///<BIT [31:25] Reserved.
    } b;    
} Cmd01_t;

///  @brief 0xF010A00C : Command 23
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t REF_TAG_CHK                                 : 1;         ///<BIT [0] Reference Tag Check.
        uint32_t APP_TAG_CHK                                 : 1;         ///<BIT [1] Application Tag Check.
        uint32_t GRD_CHK                                     : 1;         ///<BIT [2] Guard Check.
        uint32_t RF010A00C_3_RSVD                            : 1;         ///<BIT [3] Reserved.
        uint32_t DIS_DATA_PROT_CHK_IF_APP_TAGS_ALL_F         : 1;         ///<BIT [4] Disable Data Protection Check If Application Tags All Fs.
        uint32_t DIS_DATA_PROT_CHK_IF_REF_TAGS_ALL_F         : 1;         ///<BIT [5] Disable Data Protection Check If Reference Tags All Fs.
        uint32_t DIS_DATA_PROT_CHK_IF_APP_AND_REF_TAGS_ALL_F : 1;         ///<BIT [6] Disable Data Protection Check If Application And Reference Tags.
        uint32_t RF010A00C_7_RSVD                            : 1;         ///<BIT [7] Reserved.
        uint32_t IGNR_REF_TAG_MISCMP                         : 1;         ///<BIT [8] Ignore Reference Tag Miscompare.
        uint32_t IGNR_APP_TAG_MISCMP                         : 1;         ///<BIT [9] Ignore Application Tag Miscompare.
        uint32_t IGNR_GRD_MISCMP                             : 1;         ///<BIT [10] Ignore Guard Miscompare.
        uint32_t RF010A00C_15_11_RSVD                        : 5;         ///<BIT [15:11] Reserved.
        uint32_t MSK_AXI_BRDG_WR_TMT_ERR                     : 1;         ///<BIT [16] Mask AXI Bridge Write Timeout Error.
        uint32_t MSK_AXI_BRDG_WR_FIFO_ERR                    : 1;         ///<BIT [17] Mask AXI Bridge Write FIFO Error.
        uint32_t MSK_AXI_BRDG_ABNRML_RESP_RCVD               : 1;         ///<BIT [18] Mask AXI Bridge Abnormal Response Received.
        uint32_t RF010A00E_3_RSVD                            : 1;         ///<BIT [19] Reserved.
        uint32_t MSK_AXI_BRDG_RD_TMT_ERR                     : 1;         ///<BIT [20] Mask AXI Bridge Read Timeout Error.
        uint32_t MSK_AXI_BRDG_RD_ABNRML_RESP_RCVD            : 1;         ///<BIT [21] Mask AXI Bridge Read Abnormal Response Received.
        uint32_t MSK_AXI_BRDG_RD_BRST_LEN_ERR                : 1;         ///<BIT [22] Mask AXI Bridge Read Burst Length Error.
        uint32_t RF010A00E_7_RSVD                            : 1;         ///<BIT [23] Reserved.
        uint32_t MSK_WR_FIFO_CRC_ERR                         : 1;         ///<BIT [24] Mask Write FIFO CRC Error.
        uint32_t MSK_BFFR1_RD_FIFO_CRC_ERR                   : 1;         ///<BIT [25] Mask Buffer1 Read FIFO CRC Error.
        uint32_t MSK_BFFR2_RD_FIFO_CRC_ERR                   : 1;         ///<BIT [26] Mask Buffer2 Read FIFO CRC Error.
        uint32_t MSK_SSD_PAR_ERR                             : 1;         ///<BIT [27] Mask Sub-segment Descriptor Parity Error.
        uint32_t ERR_STOP_IMMDT                              : 1;         ///<BIT [28] Error Stop Immediate.
        uint32_t RD_BUF_CH_2_IGNR_ERR                        : 1;         ///<BIT [29] Read Channel 2 Ignore Error.
        uint32_t RD_BUF_CH_1_IGNR_ERR                        : 1;         ///<BIT [30] Read Channel 1 Ignore Error.
        uint32_t WR_BUF_CH_IGNR_ERR                          : 1;         ///<BIT [31] Write Channel Ignore Error.
    } b;    
} Cmd23_t;

///  @brief 0xF010A010 : Debug Command
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RF010A010_1_0_RSVD    : 2;         ///<BIT [1:0] Reserved.
        uint32_t CLR_LNKD_LIST_PAR_ERR : 1;         ///<BIT [2] Clear Linked List Parity Error.
        uint32_t RST_RD_CH_1           : 1;         ///<BIT [3] Reset Read Channel 1.
        uint32_t RST_RD_CH_2           : 1;         ///<BIT [4] Reset Read Channel 2.
        uint32_t RST_WR_CH             : 1;         ///<BIT [5] Reset Write Channel.
        uint32_t RF010A010_15_6_RSVD   :10;         ///<BIT [15:6] Reserved.
        uint32_t PAUSE_BUF_RD_CH_1     : 1;         ///<BIT [16] Pause Buffer Read Channel 1.
        uint32_t PAUSE_BUF_RD_CH_2     : 1;         ///<BIT [17] Pause Buffer Read Channel 2.
        uint32_t PAUSE_BUF_WR_CH       : 1;         ///<BIT [18] Pause Buffer Write Channel.
        uint32_t RF010A010_23_19_RSVD  : 5;         ///<BIT [23:19] Reserved.
        uint32_t PLS_BUS               : 8;         ///<BIT [31:24] Pulse BUS.
    } b;    
} DbgCmd_t;

///  @brief 0xF010A014 : Configuration 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RD_CH_1_FXD_ADDR_MD         : 1;         ///<BIT [0] Read Channel 1 Fixed Address Mode.
        uint32_t RD_CH_2_FXD_ADDR_MD         : 1;         ///<BIT [1] Read Channel 2 Fixed Address Mode.
        uint32_t WR_CH_FXD_ADDR_MD           : 1;         ///<BIT [2] Write Channel Fixed Address Mode.
        uint32_t SSD_ADDR_INDXNG_MD          : 1;         ///<BIT [3] Sub-segment Descriptor Address Indexing Mode.
        uint32_t LNKD_LIST_1_LBA_SRCH_EN     : 1;         ///<BIT [4] Linked List 1 LBA Search Enable.
        uint32_t LNKD_LIST_2_LBA_SRCH_EN     : 1;         ///<BIT [5] Linked List 2 LBA Search Enable.
        uint32_t LNKD_LIST_3_LBA_SRCH_EN     : 1;         ///<BIT [6] Linked List 3 LBA Search Enable.
        uint32_t LNKD_LIST_PAR_DIS           : 1;         ///<BIT [7] Linked List Parity Disable.
        uint32_t RF010A014_11_8_RSVD         : 4;         ///<BIT [11:8] Reserved.
        uint32_t SBSGMNT_DSCRPTR_SIZE        : 4;         ///<BIT [15:12] Subsegment Descriptor Size.
        uint32_t RD_CH_1_AND_2_MPCC_EN       : 1;         ///<BIT [16] Read Channel 1 and 2 MPECC Enable.
        uint32_t RD_CH_1_AND_2_LBA_EN        : 1;         ///<BIT [17] Read Channel 1 and 2 LBA Enable.
        uint32_t RD_CH_1_AND_2_DATA_PDDNG_EN : 1;         ///<BIT [18] Read Channel 1 and 2 Data Padding Enable.
        uint32_t RD_CH_DMMY_HLBA_EN          : 1;         ///<BIT [19] Read Channel Dummy HLBA Enable.
        uint32_t WR_CH_MPCC_EN               : 1;         ///<BIT [20] Write Channel MPECC Enable.
        uint32_t WR_CH_LBA_EN                : 1;         ///<BIT [21] Write Channel LBA Enable.
        uint32_t WR_CH_DATA_PDDNG_EN         : 1;         ///<BIT [22] Write Channel Data Padding Enable.
        uint32_t WR_CH_DMMY_HLBA_EN          : 1;         ///<BIT [23] Write Channel Dummy HLBA Enable.
        uint32_t WR_CH_HOST_GRD_EN           : 1;         ///<BIT [24] Write Channel Host Guard Enable.
        uint32_t WR_CH_HOST_APP_TAG_EN       : 1;         ///<BIT [25] Write Channel Host Application Tag Enable.
        uint32_t WR_CH_HOST_RFRNC_TAG_EN_MD  : 2;         ///<BIT [27:26] Write Channel Host Reference Tag Enable Mode.
        uint32_t RD_CH_32_LBA_EN             : 1;         ///<BIT [28] Read Channel 32-Bit LBA Enable.
        uint32_t LNKD_LIST_WORD_ADDR_MD      : 1;         ///<BIT [29] Linked List Word Address Mode.
        uint32_t DATA_FMT                    : 2;         ///<BIT [31:30] Data Format.
    } b;    
} Cfg0_t;

///  @brief 0xF010A018 : DAB Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AXI_ID                             : 4;         ///<BIT [3:0] AXI ID.
        uint32_t AXI_RD_BRST_LEN_FOR_DMAC_RD_REQ_CH : 4;         ///<BIT [7:4] AXI Read Burst Length for DMAC READ Request Channel.
        uint32_t AXI_RD_ACC_LOCK_TYPE               : 2;         ///<BIT [9:8] AXI Read Access Lock Type.
        uint32_t AXI_WR_ACC_LOCK_TYPE               : 2;         ///<BIT [11:10] AXI WRITE Access Lock Type.
        uint32_t AXI_RD_ACC_PROT_TYPE               : 3;         ///<BIT [14:12] AXI READ Access Protection Type.
        uint32_t RF010A018_15_RSVD                  : 1;         ///<BIT [15] Reserved.
        uint32_t AXI_WR_ACC_PROT_TYPE               : 3;         ///<BIT [18:16] AXI WRITE Access Protection Type.
        uint32_t WR_BFFRBL                          : 1;         ///<BIT [19] Write Bufferable.
        uint32_t MAX_OTSTNDNG_RD_REQ                : 4;         ///<BIT [23:20] Maximum Outstanding Read Request.
        uint32_t STRCT_ARB                          : 1;         ///<BIT [24] Strict Arbitration.
        uint32_t ARB_BEAT_LMT                       : 6;         ///<BIT [30:25] Arbitration Beat Limit.
        uint32_t RF010A018_31_RSVD                  : 1;         ///<BIT [31] Reserved.
    } b;    
} DabCfg_t;

///  @brief 0xF010A01C : Additional Configuration for DAB
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t AXI_RD_BRST_LEN_FOR_DMAC_WR_REQ_CH : 4;         ///<BIT [3:0] AXI Read Burst Length for DMAC Write Request Channel.
        uint16_t MAX_OTSTNDNG_WR_REQ                : 4;         ///<BIT [7:4] Maximum Outstanding Write Requests.
        uint16_t TIME_OUT_CNT                       : 8;         ///<BIT [15:8] Time Out Count.
    } b;    
} AdditionCfgForDab_t;

///  @brief 0xF010A01E : Auxiliary Operation Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF010A01E_4_0_RSVD          : 5;         ///<BIT [4:0] Reserved.
        uint16_t RD_CH_1_LNKD_LIST_PAR_ERR   : 1;         ///<BIT [5] Read Channel 1 Linked List Parity Error.
        uint16_t RD_CH_2_LNKD_LIST_PAR_ERR   : 1;         ///<BIT [6] Read Channel 2 Linked List Parity Error.
        uint16_t WR_CH_LNKD_LIST_PAR_ERR     : 1;         ///<BIT [7] Write Channel Linked List Parity Error.
        uint16_t RD_CH_1_LGCL_BLK_GRD_MISCMP : 1;         ///<BIT [8] Read Channel 1 Logical Block Guard Miscompare.
        uint16_t RD_CH_2_LGCL_BLK_GRD_MISCMP : 1;         ///<BIT [9] Read Channel 2 Logical Block Guard Miscompare.
        uint16_t RF010A01E_15_10_RSVD        : 6;         ///<BIT [15:10] Reserved.
    } b;    
} AuxOpStat_t;

///  @brief 0xF010A020 : DMAC Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t OP_CMPLT                     : 1;         ///<BIT [0] Operation Complete.
        uint32_t DMAC_CMP_FAIL                : 1;         ///<BIT [1] DMAC Compare Fail.
        uint32_t SDRAM_TST_FAIL               : 1;         ///<BIT [2] SDRAM Test Fail.
        uint32_t RF010A020_3_RSVD             : 1;         ///<BIT [3] Reserved.
        uint32_t CRC_ERR                      : 2;         ///<BIT [5:4] CRC Error.
        uint32_t RF010A020_7_6_RSVD           : 2;         ///<BIT [7:6] Reserved.
        uint32_t REF_TAG_MISCMP               : 1;         ///<BIT [8] Reference Tag Miscompare.
        uint32_t APP_TAG_MISCMP               : 1;         ///<BIT [9] Application Tag Miscompare.
        uint32_t GRD_MISCMP                   : 1;         ///<BIT [10] Guard Miscompare.
        uint32_t RD_CH_1_MPCC_ERR             : 1;         ///<BIT [11] Read Channel 1 MPECC Error.
        uint32_t RD_CH_1_LBA_ERR              : 1;         ///<BIT [12] Read Channel 1 LBA Error.
        uint32_t RD_CH_2_MPCC_ERR             : 1;         ///<BIT [13] Read Channel 2 MPECC Error.
        uint32_t RD_CH_2_LBA_ERR              : 1;         ///<BIT [14] Read Channel 2 LBA Error.
        uint32_t END_TO_END_FAIL              : 1;         ///<BIT [15] End-to-End Fail.
        uint32_t AXI_BRDG_WR_TMT_ERR          : 1;         ///<BIT [16] AXI Bridge Write Timeout Error.
        uint32_t AXI_BRDG_WR_FIFO_ERR         : 1;         ///<BIT [17] AXI Bridge Write FIFO Error.
        uint32_t AXI_BRDG_ABNRML_RESP_RCVD    : 1;         ///<BIT [18] AXI Bridge Abnormal Response Received.
        uint32_t RF010A020_19_RSVD            : 1;         ///<BIT [19] Reserved.
        uint32_t AXI_BRDG_RD_TMT_ERR          : 1;         ///<BIT [20] AXI Bridge Read Timeout Error.
        uint32_t AXI_BRDG_RD_ABNRML_RESP_RCVD : 1;         ///<BIT [21] AXI Bridge Read Abnormal Response Received.
        uint32_t AXI_BRDG_RD_BRST_LEN_ERR     : 1;         ///<BIT [22] AXI Bridge Read Burst Length Error.
        uint32_t RF010A020_23_RSVD            : 1;         ///<BIT [23] Reserved.
        uint32_t WR_CH_FIFO_CRC_ERR           : 1;         ///<BIT [24] Write Channel FIFO CRC Error.
        uint32_t RD_CH_1_FIFO_CRC_ERR         : 1;         ///<BIT [25] Read Channel 1 FIFO CRC Error.
        uint32_t RD_CH_2_FIFO_CRC_ERR         : 1;         ///<BIT [26] Read Channel 2 FIFO CRC Error.
        uint32_t SSD_PAR_ERR_DET              : 1;         ///<BIT [27] Sub-segment Descriptor Parity Error Detected.
        uint32_t RD_BUF_1_OP_CMPLT            : 1;         ///<BIT [28] Read Buffer 1 Operation Complete.
        uint32_t RD_BUF_2_OP_CMPLT            : 1;         ///<BIT [29] Read Buffer 2 Operation Complete.
        uint32_t WR_BUF_OP_CMPLT              : 1;         ///<BIT [30] Write Buffer Operation Complete.
        uint32_t ENGN_OP_CMPLT                : 1;         ///<BIT [31] Engine Operation Complete.
    } b;    
} DmacStat_t;

///  @brief 0xF010A024 : Masked DMAC Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DMAC_SDRAM_TST_CMPLT               : 1;         ///<BIT [0] DMAC/SDRAM Test Complete.
        uint32_t DMAC_CMP_FAIL_MSTAT                : 1;         ///<BIT [1] DMAC Compare Failure for Masked DMAC Status.
        uint32_t SDRAM_TST_FAIL_MSTAT               : 1;         ///<BIT [2] SDRAM Test Failure for Masked DMAC Status.
        uint32_t ANY_UNMSKD_ERR_OCCRD               : 1;         ///<BIT [3] Any Unmasked Error Occurred.
        uint32_t CRC_ERR_MSTAT                      : 1;         ///<BIT [4] CRC Error for Masked DMAC Status.
        uint32_t RF010A024_7_5_RSVD                 : 3;         ///<BIT [7:5] Reserved.
        uint32_t REF_TAG_MISCMP_MSTAT               : 1;         ///<BIT [8] Reference Tag Miscompare for Masked DMAC Status.
        uint32_t APP_TAG_MISCMP_MSTAT               : 1;         ///<BIT [9] Application Tag Miscompare for Masked DMAC Status.
        uint32_t GRD_MISCMP_MSTAT                   : 1;         ///<BIT [10] Guard Miscompare for Masked DMAC Status.
        uint32_t BFFR1_MPECC_ERR                    : 1;         ///<BIT [11] Buffer1 MPECC Error.
        uint32_t BFFR1_LBA_ERR                      : 1;         ///<BIT [12] Buffer1 LBA Error.
        uint32_t BUF_2_MPECC_ERR                    : 1;         ///<BIT [13] Buffer 2 MPECC Error.
        uint32_t BUF_2_LBA_ERR                      : 1;         ///<BIT [14] Buffer 2 LBA Error.
        uint32_t END_TO_END_FAIL_MSTAT              : 1;         ///<BIT [15] End-to-End Fail for Masked DMAC Status.
        uint32_t AXI_BRDG_WR_TMT_ERR_MSTAT          : 1;         ///<BIT [16] AXI Bridge Write Timeout Error for Masked DMAC Status.
        uint32_t AXI_BRDG_WR_FIFO_ERR_MSTAT         : 1;         ///<BIT [17] AXI Bridge Write FIFO Error for Masked DMAC Status.
        uint32_t AXI_BRDG_ABNRML_RESP_RCVD_MSTAT    : 1;         ///<BIT [18] AXI Bridge Abnormal Response Received for Masked DMAC Status.
        uint32_t RF010A024_19_RSVD                  : 1;         ///<BIT [19] Reserved.
        uint32_t AXI_BRDG_RD_TMT_ERR_MSTAT          : 1;         ///<BIT [20] AXI Bridge Read Timeout Error for Masked DMAC Status.
        uint32_t AXI_BRDG_RD_ABNRML_RESP_RCVD_MSTAT : 1;         ///<BIT [21] AXI Bridge Read Abnormal Response Received for Masked DMAC Stat.
        uint32_t AXI_BRDG_RD_BRST_LEN_ERR_MSTAT     : 1;         ///<BIT [22] AXI Bridge Read Burst Length Error for Masked DMAC Status.
        uint32_t RF010A024_23_RSVD                  : 1;         ///<BIT [23] Reserved.
        uint32_t WR_FIFO_CRC_ERR                    : 1;         ///<BIT [24] Write FIFO CRC Error.
        uint32_t BFFR1_RD_FIFO_CRC_ERR              : 1;         ///<BIT [25] Buffer1 Read FIFO CRC Error.
        uint32_t BFFR2_RD_FIFO_CRC_ERR              : 1;         ///<BIT [26] Buffer2 Read FIFO CRC Error.
        uint32_t SSD_PAR_ERR_DET_MSTAT              : 1;         ///<BIT [27] Sub-segment Descriptor Parity Error Detected for Masked DMAC St.
        uint32_t RD_BUF_1_OP_CMPLT_MSTAT            : 1;         ///<BIT [28] Read Buffer 1 Operation Complete for Masked DMAC Status.
        uint32_t RD_BUF_2_OP_CMPLT_MSTAT            : 1;         ///<BIT [29] Read Buffer 2 Operation Complete for Masked DMAC Status.
        uint32_t WR_BUF_OP_CMPLT_MSTAT              : 1;         ///<BIT [30] Write Buffer Operation Complete for Masked DMAC Status.
        uint32_t ENGN_OP_CMPLT_MSTAT                : 1;         ///<BIT [31] Engine Operation Complete for Masked DMAC Status.
    } b;    
} MskdDmacStat_t;

///  @brief 0xF010A02C : Block Size
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BLK_SIZE             :30;         ///<BIT [29:0] Block Size.
        uint32_t RF010A02C_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;    
} BlkSize_t;

///  @brief 0xF010A030 : Read Buffer Block Size
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RD_BUF_BLK_SIZE      :30;         ///<BIT [29:0] Read Buffer Block Size.
        uint32_t RF010A030_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;    
} RdBufBlkSize_t;

///  @brief 0xF010A034 : Write Buffer Block Size
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t WR_BUF_BLK_SIZE      :30;         ///<BIT [29:0] Write Buffer Block Size.
        uint32_t RF010A034_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;    
} WrBufBlkSize_t;

///  @brief 0xF010A050 : Write Read Buffer FIFO Threshold
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t WR_BFFRS_FIFO_THRES : 8;         ///<BIT [7:0] Write Buffers FIFO Threshold.
        uint32_t RD_BFFRS_FIFO_THRES : 8;         ///<BIT [15:8] Read Buffers FIFO Threshold.
        uint32_t rsvd_RF010A052:16;               ///<BIT [31:16] Reserved
    } b;    
} WrRdBufFifoThr_t;

///  @brief 0xF010A054 : System Address Zone
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SYSTM_ADDR_ZONE_FOR_LNKD_LIST_1 : 4;         ///<BIT [3:0] System Address Zone for Linked List 1.
        uint32_t SYSTM_ADDR_ZONE_FOR_LNKD_LIST_2 : 4;         ///<BIT [7:4] System Address Zone for Linked List 2.
        uint32_t RF010A054_11_8_RSVD             : 4;         ///<BIT [11:8] Reserved.
        uint32_t SYSTM_ADDR_ZONE_FOR_LNKD_LIST_3 : 4;         ///<BIT [15:12] System Address Zone for Linked List 3.
        uint32_t RF010A054_31_16_RSVD            :16;         ///<BIT [31:16] Reserved.
    } b;    
} SystemAddrZone_t;

///  @brief 0xF010A07C : E2E Index Buffer Block/Host Block Ratio
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t E2E_IDX                : 8;         ///<BIT [7:0] E2E Index.
        uint16_t BUF_BLK_HOST_BLK_RATIO : 8;         ///<BIT [15:8] Buffer Block / Host Block Ratio.
    } b;    
} E2eIndexBufBlkHostBlkRatio_t;

///  @brief 0xF010A07E : First Buffer Block/Host Block Ratio
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FRST_HOST_BLK_RATIO : 8;         ///<BIT [7:0] First Host Block Ratio.
        uint16_t FRST_BLK_RATIO_EN   : 1;         ///<BIT [8] First Block Ratio Enable.
        uint16_t RF010A07E_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
    } b;    
} FirstBufBlkHostBlkRatio_t;

///  @brief 0xF010A090 : Memory BIST Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MEM_BIST_CTL_RD_CH_1_FIFO_RAM : 8;         ///<BIT [7:0] Memory BIST Control for Read Channel 1 FIFO RAM.
        uint32_t MEM_BIST_CTL_RD_CH_2_FIFO_RAM : 8;         ///<BIT [15:8] Memory BIST Control for Read Channel 2 FIFO RAM.
        uint32_t MEM_BIST_CTL_WR_CH_FIFO_RAM   : 8;         ///<BIT [23:16] Memory BIST Control for Write Channel FIFO RAM.
        uint32_t RF010A090_31_24_RSVD          : 8;         ///<BIT [31:24] Reserved.
    } b;    
} MemBistCtrl_t;

///  @brief 0xF010A094 : Memory BIST XOR Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MEM_BIST_STS_RD_CH_1_FIFO_RAM : 8;         ///<BIT [7:0] Memory BIST Status for Read Channel 1 FIFO RAM.
        uint32_t MEM_BIST_STS_RD_CH_2_FIFO_RAM : 8;         ///<BIT [15:8] Memory BIST Status for Read Channel 2 FIFO RAM.
        uint32_t MEM_BIST_STS_WR_CH_FIFO_RAM   : 8;         ///<BIT [23:16] Memory BIST Status for Write Channel FIFO RAM.
        uint32_t RF010A094_31_24_RSVD          : 8;         ///<BIT [31:24] Reserved.
    } b;    
} MemBistXorStat_t;

///  @brief 0xF010A098 : Configuration 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t USE_RD_CH_2_LBA_EN      : 1;         ///<BIT [0] Use Read Channel 2 LBA Enable.
        uint32_t USE_WR_DMAC_DATA_FMT_EN : 1;         ///<BIT [1] Use Write DMAC Data Format Enable.
        uint32_t RF010A098_20_2_RSVD     :19;         ///<BIT [20:2] Reserved.
        uint32_t RD_MPECC_PAD_EN         : 1;         ///<BIT [21] Read Channel MPECC Padding Enable.
        uint32_t WR_MPECC_PAD_EN         : 1;         ///<BIT [22] Write Channel MPECC Padding Enable.
        uint32_t HLBA_4_8_BYTE_SEL       : 1;         ///<BIT [23] HLBA 4-Byte Or 8-Byte Selection.
        uint32_t WR_CH_DATA_FMT          : 2;         ///<BIT [25:24] Write Channel Data Format.
        uint32_t RF010A098_30_26_RSVD    : 5;         ///<BIT [30:26] Reserved.
        uint32_t WR_CH_REQ_BRK_EN        : 1;         ///<BIT [31] Write Channel Request Break Enable.
    } b;    
} Cfg1_t;

///  @brief 0xF010A0C0 : Pattern Search and Zero Count Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PATTERN_SEARCH_EN    : 1;         ///<BIT [0] Pattern Search Enable.
        uint32_t PATTERN_SEARCH_WIDTH : 1;         ///<BIT [1] Pattern Search Width.
        uint32_t PATTERN_SEARCH_MODE  : 1;         ///<BIT [2] Pattern Search Mode.
        uint32_t ZERO_COUNT_EN        : 1;         ///<BIT [3] Zero Count Enable.
        uint32_t ZERO_COUNT_SEL       : 1;         ///<BIT [4] Zero Count Select.
        uint32_t SEARCH_WR_EN         : 1;         ///<BIT [5] Pattern Search and Zero Count Write Enable.
        uint32_t RF010A0C0_31_6_RSVD  :26;         ///<BIT [31:6] Reserved.
    } b;    
} PatSearchAndZeroCntCtrl_t;

///  @brief 0xF010A0C4 : Scramble Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SCR_DSCR_EN         : 1;         ///<BIT [0] Scramble Enable.
        uint32_t SCRAMBLE_SEL        : 1;         ///<BIT [1] Scramble Select.
        uint32_t RF010A0C4_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;    
} ScrambleCtrl_t;

///  @brief 0xF010B014 : Read and Write Channel Protection
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DPP_BYPASS_MODE_SET  : 1;         ///<BIT [0] DPP Bypass Mode Set.
        uint32_t CPU_INTERRUPT_MASK   : 1;         ///<BIT [1] CPU Interrupt Mask.
        uint32_t CPU_INTERRUPT_CLR    : 1;         ///<BIT [2] CPU Interrupt Clear.
        uint32_t RF010B014_3_RSVD     : 1;         ///<BIT [3] Reserved.
        uint32_t RD_SOFT_RST_N        : 1;         ///<BIT [4] RD_SOFT_RST_N Read Channel Soft Reset.
        uint32_t WD_SOFT_RST_N        : 1;         ///<BIT [5] WR_SOFT_RST_N Read Channel Soft Reset.
        uint32_t RF010B014_6_RSVD     : 1;         ///<BIT [6] Reserved.
        uint32_t RF010B014_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint32_t RD_PROT_EN           : 1;         ///<BIT [8] Read Channel Protection Enable.
        uint32_t RD_PROT_ADDR_ALL     : 1;         ///<BIT [9] Read Channel Protection Enable for All Addresses.
        uint32_t RF010B014_10_RSVD    : 1;         ///<BIT [10] Reserved.
        uint32_t WR_PROT_EN           : 1;         ///<BIT [11] Write Channel Protection Enable.
        uint32_t WR_PROT_ADDR_ALL     : 1;         ///<BIT [12] Write Channel Protection Enable for All Addresses.
        uint32_t RF010B014_13_RSVD    : 1;         ///<BIT [13] Reserved.
        uint32_t RF010B014_31_14_RSVD :18;         ///<BIT [31:14] Reserved.
    } b;    
} RdAndWrChanProtecti_t;

///  @brief 0xF010B01C : Register Slice Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CPU_ACC             : 1;         ///<BIT [0] Set to Handle CPU Type Access Only.
        uint32_t DWNSTR_REG_SLICE    : 1;         ///<BIT [1] Enable Register Slice for Downstream.
        uint32_t UPSTR_REG_SLICE     : 1;         ///<BIT [2] Enable Register Slice for Upstream.
        uint32_t RF010B01C_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} RegisterSliceCfg_t;

///  @brief 0xF010B04C : SDP Error
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CRC_SYNDROME         :16;         ///<BIT [15:0] CRC Syndrome.
        uint32_t RD_ACCESS_ERR        : 1;         ///<BIT [16] Read Access Error.
        uint32_t WR_ACCESS_ERR        : 1;         ///<BIT [17] Write Access Error.
        uint32_t RD_DEC_ERR           : 1;         ///<BIT [18] Read Decode Error.
        uint32_t RD_SLV_ERR           : 1;         ///<BIT [19] Read Slave Error.
        uint32_t DPP_ERR              : 1;         ///<BIT [20] Data Path Protection Error.
        uint32_t WR_DEC_ERR           : 1;         ///<BIT [21] Write Decode Error.
        uint32_t WR_SLV_ERR           : 1;         ///<BIT [22] Write Slave Error.
        uint32_t RF010B04C_31_23_RSVD : 9;         ///<BIT [31:23] Reserved.
    } b;    
} SdpErr_t;

///  @brief 0xF010B050 : SDP Debug
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t STATUS_DPP_BYPASS_MODE : 1;         ///<BIT [0] Status DPP Bypass Mode.
        uint32_t RF010B050_15_1_RSVD    :15;         ///<BIT [15:1] Reserved.
        uint32_t DEBUG_AXI_SLAVE_WR_ST  : 2;         ///<BIT [17:16] Debug AXI Slave Write State.
        uint32_t DEBUG_AXI_MASTER_RD_ST : 2;         ///<BIT [19:18] Debug AXI Master Read State.
        uint32_t DBUG_AXI_MASTER_WR_ST  : 2;         ///<BIT [21:20] Debug AXI Master Write State.
        uint32_t DBUG_RD_CTRL_ST        : 4;         ///<BIT [25:22] Debug Read Control State.
        uint32_t DBUG_WR_CTRL_ST        : 3;         ///<BIT [28:26] Debug Write Control State.
        uint32_t RF010B050_31_29_RSVD   : 3;         ///<BIT [31:29] Reserved.
    } b;    
} SdpDbg_t;


/// @brief 0xF010A000-0xF010B054 : Dmac_t MODULE
typedef struct
{
    IndAccessIndex_t indAccessIndex;                                    ///< 0xF010A000 : Indirect Access Index
    uint32_t IND_ACC_PRT;                                               ///< 0xF010A004 : Indirect Access Port
    Cmd01_t cmd01;                                                      ///< 0xF010A008 : Command 0
    Cmd23_t cmd23;                                                      ///< 0xF010A00C : Command 2
    DbgCmd_t dbgCmd;                                                    ///< 0xF010A010 : Debug Command
    Cfg0_t cfg0;                                                        ///< 0xF010A014 : Configuration 0
    DabCfg_t dabCfg;                                                    ///< 0xF010A018 : DAB Configuration
    AdditionCfgForDab_t additionCfgForDab;                              ///< 0xF010A01C : Additional Configuration for DAB
    AuxOpStat_t auxOpStat;                                              ///< 0xF010A01E : Auxiliary Operation Status
    DmacStat_t dmacStat;                                                ///< 0xF010A020 : DMAC Status
    MskdDmacStat_t mskdDmacStat;                                        ///< 0xF010A024 : Masked DMAC Status
    uint32_t BLK_CNT:16;                                                ///< 0xF010A028 : Block Count
    uint32_t rsvd_RF010A02A:16;                                         ///< 0xF010A02A - 0xF010A02B : Reserved.
    BlkSize_t blkSize;                                                  ///< 0xF010A02C : Block Size
    RdBufBlkSize_t rdBufBlkSize;                                        ///< 0xF010A030 : Read Buffer Block Size
    WrBufBlkSize_t wrBufBlkSize;                                        ///< 0xF010A034 : Write Buffer Block Size
    uint32_t RD_BUF_LGCL_BLK_ADDR_0;                                    ///< 0xF010A038 : Read Buffer Logical Block Address 0
    uint32_t RD_BUF_LGCL_BLK_ADDR_1;                                    ///< 0xF010A03C : Read Buffer Logical Block Address 1
    uint32_t WR_BUF_LGCL_BLK_ADDR_0;                                    ///< 0xF010A040 : Write Buffer Logical Block Address 0
    uint32_t WR_BUF_LGCL_BLK_ADDR_1;                                    ///< 0xF010A044 : Write Buffer Logical Block Address 1
    uint32_t WR_BUF_LGCL_BLK_ADDR_REF_TAG;                              ///< 0xF010A048 : Write Buffer Logical Block Address Reference Tag
    uint32_t WR_BUF_APP_TAG;                                            ///< 0xF010A04C : Write Buffer Application Tag
    WrRdBufFifoThr_t wrRdBufFifoThr;                                    ///< 0xF010A050 : Write Read Buffer FIFO Threshold
    SystemAddrZone_t systemAddrZone;                                    ///< 0xF010A054 : System Address Zone
    uint32_t LNKD_LIST_1_SUB_SEG_DSCRPTR_TBL_BASE_ADDR;                 ///< 0xF010A058 : Linked List 1 SSD Table Base Address
    uint32_t LNKD_LIST_1_STRT_IDX_OR_ADDR;                              ///< 0xF010A05C : Linked List 1 SSD Table Start Index
    uint32_t LNKD_LIST_1_SSD_STRT_LBA;                                  ///< 0xF010A060 : Linked List 1 SSD Table Start LBA
    uint32_t LNKD_LIST_2_SUB_SEG_DSCRPTR_TBL_BASE_ADDR;                 ///< 0xF010A064 : Linked List 2 SSD Table Base Address
    uint32_t LNKD_LIST_2_SUB_SEG_DSCRPTR_START_IX_ADDR_OR_RLD_ADDR;     ///< 0xF010A068 : Linked List 2 SSD Table Start Index
    uint32_t LNKD_LIST_2_SSD_STRT_LBA;                                  ///< 0xF010A06C : Linked List 2 SSD Table Start LBA
    uint32_t LNKD_LIST_3_SUB_SEG_DSCRPTR_TBL_BASE_ADDR;                 ///< 0xF010A070 : Linked List 3 SSD Table Base Address
    uint32_t SUB_SEG_DSCRPTR_3_STRT_ADDR_OR_IDX;                        ///< 0xF010A074 : Linked List 3 SSD Table Start Index
    uint32_t LNKD_LIST_3_SSD_STRT_LBA;                                  ///< 0xF010A078 : Linked List 3 SSD Table Start LBA
    E2eIndexBufBlkHostBlkRatio_t e2eIndexBufBlkHostBlkRatio;            ///< 0xF010A07C : E2E Index Buffer Block/Host Block Ratio
    FirstBufBlkHostBlkRatio_t firstBufBlkHostBlkRatio;                  ///< 0xF010A07E : First Buffer Block/Host Block Ratio
    uint32_t APP_TAG_MSK:16;                                            ///< 0xF010A080 : Application Tag Mask
    uint32_t E2E_IDX_EXTNTN:16;                                         ///< 0xF010A082 : E2E Index Extension
    uint32_t CH_1_LGCL_BLK_GRD_CRC_SEED:16;                             ///< 0xF010A084 : Read Channel 1 Logical Block Guard CRC Seed
    uint32_t CH_2_LGCL_BLK_GRD_CRC_SEED:16;                             ///< 0xF010A086 : Read Channel 2 Logical Block Guard CRC Seed
    uint32_t WR_CH_LGCL_BLK_GRD_CRC_SEED:16;                            ///< 0xF010A088 : Write Channel Logical Block Guard CRC Seed
    uint32_t rsvd_RF010A08A:16;                                         ///< 0xF010A08A - 0xF010A08B : Reserved.
    uint32_t RAMS_TMG_CFGA;                                             ///< 0xF010A08C : RAMs Timing Configuration
    MemBistCtrl_t memBistCtrl;                                          ///< 0xF010A090 : Memory BIST Control
    MemBistXorStat_t memBistXorStat;                                    ///< 0xF010A094 : Memory BIST XOR Status
    Cfg1_t cfg1;                                                        ///< 0xF010A098 : Configuration 1
    uint32_t RD_BUF_CH_2_LGCL_BLK_ADDR_0;                               ///< 0xF010A09C : Read Buffer Channel 2 Logical Block Address 0
    uint16_t READ_CH2_LBA1;                                             ///< 0xF010A0A0 : Read Buffer Channel 2 Logical Block Address 1
    uint8_t rsvd_RF010A0A2[30];                                         ///< 0xF010A0A2 - 0xF010A0BF : Reserved.
    PatSearchAndZeroCntCtrl_t patSearchAndZeroCntCtrl;                  ///< 0xF010A0C0 : Pattern Search and Zero Count Control
    ScrambleCtrl_t scrambleCtrl;                                        ///< 0xF010A0C4 : Scramble Control
    uint32_t SEARCH_PATTERN;                                            ///< 0xF010A0C8 : Search Pattern
    uint32_t SEARCH_PATTERN_MASK;                                       ///< 0xF010A0CC : Search Pattern Mask
    uint32_t SCRAMBLE_SEED;                                             ///< 0xF010A0D0 : Scramble Seed
    uint32_t PATTERN_MATCH_COUNT;                                       ///< 0xF010A0D4 : Pattern Match Count
    uint32_t PATTERN_MATCH_ADDR0;                                       ///< 0xF010A0D8 : Pattern Match Address 0
    uint32_t PATTERN_MATCH_ADDR1;                                       ///< 0xF010A0DC : Pattern Match Address 1
    uint32_t PATTERN_MATCH_ADDR2;                                       ///< 0xF010A0E0 : Pattern Match Address 2
    uint32_t PATTERN_MATCH_ADDR3;                                       ///< 0xF010A0E4 : Pattern Match Address 3
    uint32_t PATTERN_MATCH_ADDR4;                                       ///< 0xF010A0E8 : Pattern Match Address 4
    uint32_t PATTERN_MATCH_ADDR5;                                       ///< 0xF010A0EC : Pattern Match Address 5
    uint32_t PATTERN_MATCH_ADDR6;                                       ///< 0xF010A0F0 : Pattern Match Address 6
    uint32_t PATTERN_MATCH_ADDR7;                                       ///< 0xF010A0F4 : Pattern Match Address 7
    uint32_t PATTERN_MATCH_ADDR8;                                       ///< 0xF010A0F8 : Pattern Match Address 8
    uint32_t PATTERN_MATCH_ADDR9;                                       ///< 0xF010A0FC : Pattern Match Address 9
    uint32_t PATTERN_MATCH_ADDR10;                                      ///< 0xF010A100 : Pattern Match Address 10
    uint32_t PATTERN_MATCH_ADDR11;                                      ///< 0xF010A104 : Pattern Match Address 11
    uint32_t PATTERN_MATCH_ADDR12;                                      ///< 0xF010A108 : Pattern Match Address 12
    uint32_t PATTERN_MATCH_ADDR13;                                      ///< 0xF010A10C : Pattern Match Address 13
    uint32_t PATTERN_MATCH_ADDR14;                                      ///< 0xF010A110 : Pattern Match Address 14
    uint32_t PATTERN_MATCH_ADDR15;                                      ///< 0xF010A114 : Pattern Match Address 15
    uint32_t ZERO_COUNT;                                                ///< 0xF010A118 : Zero Count
    uint8_t rsvd_RF010A11C[3812];                                       ///< 0xF010A11C - 0xF010AFFF : Reserved.
    uint32_t PARITY_BASE_ADDR;                                          ///< 0xF010B000 : Parity Base Address
    uint32_t PROT_BASE_ADDR;                                            ///< 0xF010B004 : Protection Region Base Address
    uint32_t PROT_END_ADDR;                                             ///< 0xF010B008 : Protection Region End Address
    uint8_t rsvd_RF010B00C[8];                                          ///< 0xF010B00C - 0xF010B013 : Reserved.
    RdAndWrChanProtecti_t rdAndWrChanProtecti;                          ///< 0xF010B014 : Read and Write Channel Protection
    uint32_t DATA_BASE_ADDR;                                            ///< 0xF010B018 : Base Address for Data
    RegisterSliceCfg_t regSliceCfg;                                     ///< 0xF010B01C : Register Slice Configuration
    uint8_t rsvd_RF010B020[8];                                          ///< 0xF010B020 - 0xF010B027 : Reserved.
    uint32_t RD_ERR_DATA_WORD_6;                                        ///< 0xF010B028 : Read Error Data Word 6
    uint32_t RD_ERR_DATA_WORD_7;                                        ///< 0xF010B02C : Read Error Data Word 7
    uint32_t RD_ERR_DATA_WORD_4;                                        ///< 0xF010B030 : Read Error Data Word 4
    uint32_t RD_ERR_DATA_WORD_5;                                        ///< 0xF010B034 : Read Error Data Word 5
    uint32_t RD_ERR_DATA_WORD_2;                                        ///< 0xF010B038 : Read Error Data Word 2
    uint32_t RD_ERR_DATA_WORD_3;                                        ///< 0xF010B03C : Read Error Data Word 3
    uint32_t RD_ERR_DATA_WORD_0;                                        ///< 0xF010B040 : Read Error Data Word 0
    uint32_t RD_ERR_DATA_WORD_1;                                        ///< 0xF010B044 : Read Error Data Word 1
    uint32_t RD_ERR_DATA_ADDR_LSB;                                      ///< 0xF010B048 : Read Error Data Address LSB
    SdpErr_t sdpErr;                                                    ///< 0xF010B04C : SDP Error
    SdpDbg_t sdpDbg;                                                    ///< 0xF010B050 : SDP Debug
} Dmac_t;
COMPILE_ASSERT(offsetof(Dmac_t,sdpDbg)==0xF010B050-0xF010A000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile Dmac_t rDmac;                        ///< 0xF010A000-0xF010B054 : Dmac_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
extern Dmac_t mDmac;                        ///< 0xF010A000-0xF010B054 : Dmac_t MODULE

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
//! @brief ECU Top Registers
//!
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  ECU Top Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF200C000 : ECU Control and Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECU_SOFT_RESET      : 1;         ///<BIT [0] ECU Soft Reset.
#if (_SOC_REV_ >= 0x0210)
        uint16_t RF200C000_5_1_RSVD  : 5;         ///<BIT [5:1] Reserved.
        uint16_t ECC_REG_PARA_WR     : 1;         ///<BIT [6] EC_CORE Register Parallel Writing Enable.
        uint16_t RF200C000_15_7_RSVD : 9;         ///<BIT [15:7] Reserved.
#else
        uint16_t RF200C000_15_1_RSVD :15;         ///<BIT [15:1] Reserved.
#endif
    } b;
} EcuCtrlAndCfg1_t;

///  @brief 0xF200C002 : ECU Control and Configuration 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
#if (_SOC_REV_ >= 0x0210)
        uint16_t RF200C002_0_RSVD         : 1;         ///<BIT [0] Reserved.
#else
        uint16_t STOP_ON_AXI_PAR_ERR      : 1;         ///<BIT [0] Stop on AXI Parity Error.
#endif
        uint16_t STOP_ON_MPECC_ERR        : 1;         ///<BIT [1] Stop on MPECC Error.
        uint16_t STOP_ON_HLBA_MMATCH      : 1;         ///<BIT [2] Stop on HLBA Mismatch on Read.
        uint16_t STOP_ON_UNCORR           : 1;         ///<BIT [3] Stop on Uncorrectable AU on Read.
        uint16_t STOP_ON_CW_OVRLMT        : 1;         ///<BIT [4] Stop on Codeword Over-Limit.
        uint16_t STOP_ON_MCRC_ERR         : 1;         ///<BIT [5] Stop on Media-CRC Error on Read.
        uint16_t STOP_ON_AUX_MMATCH       : 1;         ///<BIT [6] Stop on Auxiliary Data Mismatch on Read.
        uint16_t STOP_ON_ERASE            : 1;         ///<BIT [7] Stop on Erase Page on Read.
        uint16_t STOP_ON_DPP_ERR          : 1;         ///<BIT [8] Stop on Memory Data Path Protection Error.
        uint16_t STOP_ON_16_AU            : 1;         ///<BIT [9] Stop on 16th AU.
        uint16_t STOP_ON_RAID_DPP_ERR     : 1;         ///<BIT [10] Stop on RAID DPP Error.
        uint16_t STOP_ON_AU_NUM           : 1;         ///<BIT [11] Stop on AU Number.
        uint16_t STOP_ON_AU_OVRLMT        : 1;         ///<BIT [12] Stop on AU Over-Limit on Read.
        uint16_t STOP_ON_RAID_BUF0CHK_ERR : 1;         ///<BIT [13] Stop on RAID Buffer Zero Check Error on Read.
        uint16_t STOP_ON_AXI_ERR          : 1;         ///<BIT [14] Stop on AXI Error.
        uint16_t RF200C002_15_RSVD        : 1;         ///<BIT [15] Reserved.
    } b;
} EcuCtrlAndCfg2_t;

///  @brief 0xF200C004 : ECU Control and Configuration 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t MAX_AU_ERR_THR       :12;         ///<BIT [11:0] Maximum AU Error Threshold.
        uint16_t RF200C004_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcuCtrlAndCfg3_t;

///  @brief 0xF200C006 : ECU Control and Configuration 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t XOR_DPP_POLARITY       : 1;         ///<BIT [0] XOR Data Path Protection Polarity.
        uint16_t ERASE_PAGE_DET_ONLY    : 1;         ///<BIT [1] Erase Page Detection Only.
        uint16_t META_AUX_RCVR          : 1;         ///<BIT [2] Metadata Auxiliary Recovery.
        uint16_t HWLL_META_READ_SKIP_EN : 1;         ///<BIT [3] HWLL Metadata Read Skip.
        uint16_t FWLL_META_READ_SKIP_EN : 1;         ///<BIT [4] FWLL Metadata Read Skip.
        uint16_t SQCR_AUX_PARITY_CHK_EN : 1;         ///<BIT [5] Sequencer Auxiliary Parity Check Enable.
#if (_SOC_REV_ >= 0x0210)
        uint16_t RF200C006_6_RSVD       : 1;         ///<BIT [6] Reserved.
#else
        uint16_t FLC_LL_PARITY_CHK      : 1;         ///<BIT [6] Flash Link Controller Link List Parity Check Enable.
#endif
        uint16_t SEND_UNCORR_TODDR      : 1;         ///<BIT [7] Send Uncorrectable AU to DDR.
        uint16_t RF200C006_8_RSVD       : 1;         ///<BIT [8] Reserved.
        uint16_t SEND_UNCORR_HISTO_ECNT : 1;         ///<BIT [9] Send Uncorrectable AU Error Count.
        uint16_t SOFT_INFO_IF_TYPE      : 2;         ///<BIT [11:10] Soft Information Interface Type.
#if (_SOC_REV_ >= 0x0210)
        uint16_t DMA_DISABLE            : 1;         ///<BIT [12] 0h:Don't disable DMA.
        uint16_t RF200C006_15_13_RSVD   : 3;         ///<BIT [15:13] Reserved.
#else
        uint16_t RF200C006_15_12_RSVD   : 4;         ///<BIT [15:12] Reserved.
#endif
    } b;
} EcuCtrlAndCfg4_t;

///  @brief 0xF200C008 : ECU Control and Configuration 5 and 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FIT_2_PHY_PAGE         : 1;         ///<BIT [0] Fit to Physical Page.
        uint16_t RF200C008_1_RSVD       : 1;         ///<BIT [1] Reserved.
        uint16_t ECC_INV_MODE           : 1;         ///<BIT [2] ECC Invert Mode.
        uint16_t MCRC_INV_MODE          : 1;         ///<BIT [3] Media-CRC Invert Mode.
        uint16_t META_ECC_EN            : 1;         ///<BIT [4] Metadata ECC Enable.
        uint16_t ECC_ITR_BYPASS_EN      : 1;         ///<BIT [5] ECC Iterative Bypass Enable.
        uint16_t META_BCH_BYPASS_EN     : 1;         ///<BIT [6] Metadata BCH Bypass Enable.
        uint16_t RF200C008_7_RSVD       : 1;         ///<BIT [7] Reserved.
        uint16_t MANUAL_ERASE_THRSHD_EN : 1;         ///<BIT [8] Manual Erase Threshold Enable.
        uint16_t RF200C008_10_9_RSVD    : 2;         ///<BIT [10:9] Reserved.
        uint16_t ECC_LDPC_DIS           : 1;         ///<BIT [11] ECC LDPC Disable.
        uint16_t ECC_ITR_CODE_SEL       : 2;         ///<BIT [13:12] ECC Iterative Code Selection.
        uint16_t RF200C008_15_14_RSVD   : 2;         ///<BIT [15:14] Reserved.
    } b;
} EcuCtrlAndCfg5And6_t;

///  @brief 0xF200C00A : ECU Control and Configuration 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t MAX_ACT_AU_DEC_CORE : 6;         ///<BIT [5:0] Maximum Active AU Decoding Core.
        uint16_t RF200C00A_8_6_RSVD  : 3;         ///<BIT [8:6] Reserved.
        uint16_t MAX_ACT_AU_ENC_CORE : 6;         ///<BIT [14:9] Maximum Active AU Encoding Core.
        uint16_t RF200C00A_15_RSVD   : 1;         ///<BIT [15] Reserved.
    } b;
} EcuCtrlAndCfg6_t;

///  @brief 0xF200C010 : ECU Control and Configuration 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECU_HOST_MPECC_EN      : 1;         ///<BIT [0] ECU Host MPECC Enable.
        uint16_t ECU_META_MPECC_EN      : 1;         ///<BIT [1] ECU Meta MPECC Enable.
        uint16_t ECU_HLBA_MODE          : 2;         ///<BIT [3:2] ECU HLBA Mode.
        uint16_t ECU_HLBA_STRIP_MODE    : 2;         ///<BIT [5:4] ECU HLBA Strip Mode.
        uint16_t RF200C010_6_RSVD       : 1;         ///<BIT [6] Reserved.
        uint16_t ECU_HLBA_INSERT_OPTION : 1;         ///<BIT [7] ECU HLBA Insert Option.
        uint16_t ECU_HOST_MPECC_CHK     : 1;         ///<BIT [8] ECU Host MPECC Check.
        uint16_t ECU_META_MPECC_CHK     : 1;         ///<BIT [9] ECU Meta MPECC Check.
        uint16_t RF200C010_14_10_RSVD   : 5;         ///<BIT [14:10] Reserved.
        uint16_t BCRC_EN                : 1;         ///<BIT [15] Block CRC Enable.
    } b;
} EcuCtrlAndCfg7_t;

///  @brief 0xF200C012 : ECU Control and Configuration 8
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HOST_SECTOR_SIZE     :14;         ///<BIT [13:0] Host Sector Size.
        uint16_t RF200C012_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;
} EcuCtrlAndCfg8_t;

///  @brief 0xF200C014 : ECU Control and Configuration 9
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECU2HOST_RATIO       :10;         ///<BIT [9:0] ECU to Host Ratio.
        uint16_t RF200C014_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;
} EcuCtrlAndCfg9_t;

///  @brief 0xF200C016 : ECU Control and Configuration 10
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t META_SIZE            :10;         ///<BIT [9:0] Metadata Size.
        uint16_t RF200C016_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;
} EcuCtrlAndCfg10_t;

///  @brief 0xF200C018 : ECU Control and Configuration 11
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t LDPC_SYND_WEIGHT_THRSHD_PTR : 2;         ///<BIT [1:0] LDPC Syndrome Weight Threshold Pointer.
        uint16_t RF200C018_15_2_RSVD         :14;         ///<BIT [15:2] Reserved.
    } b;
} EcuCtrlAndCfg11_t;

///  @brief 0xF200C01A : ECU Control and Configuration 12
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t LDPC_SYND_WEIGHT_THRSHD :12;         ///<BIT [11:0] LDPC Syndrome Weight Threshold.
        uint16_t RF200C01A_15_12_RSVD    : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcuCtrlAndCfg12_t;

///  @brief 0xF200C01C : ECU Control and Configuration 13
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C01C_2_0_RSVD   : 3;         ///<BIT [2:0] Reserved.
        uint16_t SCR_META_INV_EN      : 1;         ///<BIT [3] Scrambler Metadata Invert Enable.
        uint16_t SCR_EN               : 1;         ///<BIT [4] Scrambler Enable.
        uint16_t SCR_SEED_SRC         : 3;         ///<BIT [7:5] Scrambler Seed Source.
        uint16_t SCR_SEED_MAP_SEL     : 1;         ///<BIT [8] Scrambler Seed Mapping Select.
        uint16_t RF200C01C_10_9_RSVD  : 2;         ///<BIT [10:9] Reserved.
        uint16_t SCR_SEED_MAP_BYPASS  : 1;         ///<BIT [11] Scrambler Seed Map Bypass.
        uint16_t RF200C01C_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcuCtrlAndCfg13_t;

///  @brief 0xF200C01E : ECU AU Format Configuration Pointer
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t AU_FORMAT_CONF_PTR       : 5;         ///<BIT [4:0] Allocation Unit Format Configuration Pointer.
        uint16_t RF200C01E_7_5_RSVD       : 3;         ///<BIT [7:5] Reserved.
        uint16_t AU_FORMAT_CHECK_ENABLE   : 1;         ///<BIT [8] Allocation Unit Format Check Enable.
        uint16_t RF200C01E_9_RSVD         : 1;         ///<BIT [9] Reserved.
        uint16_t AU_FORMAT_VLD            : 1;         ///<BIT [10] Allocation Unit Format Valid.
        uint16_t AU_FORMAT_CW_SIZE_SELECT : 3;         ///<BIT [13:11] Allocation Unit Format Codeword Size Select.
        uint16_t RF200C01E_15_14_RSVD     : 2;         ///<BIT [15:14] Reserved.
    } b;
} EcuAuFmtCfgPtr_t;

///  @brief 0xF200C020 : ECU Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CW_PER_AU           : 3;         ///<BIT [2:0] Codeword Per Allocation Unit.
        uint16_t RF200C020_4_3_RSVD  : 2;         ///<BIT [4:3] Reserved.
        uint16_t AU_MCRC_EN          : 1;         ///<BIT [5] AU MCRC Enable.
        uint16_t RF200C020_6_RSVD    : 1;         ///<BIT [6] Reserved.
        uint16_t AU_MCRC_SIZE_OPT    : 1;         ///<BIT [7] Allocation Unit MCRC Size Option.
        uint16_t RF200C020_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;
} EcuCfg1_t;

///  @brief 0xF200C022 : ECU Configuration 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PHY_PAGE_SIZE     :15;         ///<BIT [14:0] Physical Page Size.
        uint16_t RF200C022_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;
} EcuCfg2_t;

///  @brief 0xF200C024 : ECU Configuration 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t META_T             : 6;         ///<BIT [5:0] Correction Power for Meta-ECC.
        uint16_t RF200C024_7_6_RSVD : 2;         ///<BIT [7:6] Reserved.
        uint16_t META_MCRC_EN       : 1;         ///<BIT [8] Metadata Media CRC Enable.
        uint16_t META_THRSHD        : 6;         ///<BIT [14:9] Meta-ECC Threshold.
        uint16_t RF200C024_15_RSVD  : 1;         ///<BIT [15] Reserved.
    } b;
} EcuCfg3_t;

///  @brief 0xF200C028 : ECU Configuration 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t THRSHD               :10;         ///<BIT [9:0] Threshold.
        uint16_t RF200C028_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;
} EcuCfg5_t;

///  @brief 0xF200C02A : ECU AU Size
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t AU_FORMAT_CW_SIZE    :14;         ///<BIT [13:0] Allocation Unit Format Codeword Size.
        uint16_t RF200C02A_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;
} EcuAuSize_t;

///  @brief 0xF200C02E : ECU Configuration 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ERASE_THRSHD        : 7;         ///<BIT [6:0] Erase Threshold.
        uint16_t RF200C02E_15_7_RSVD : 9;         ///<BIT [15:7] Reserved.
    } b;
} EcuCfg6_t;

///  @brief 0xF200C040 : ECU Encode Status 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECU_ENC_RESUME         : 1;         ///<BIT [0] ECU Encode Resume.
        uint16_t MPECC_ERR              : 1;         ///<BIT [1] MPECC Error.
        uint16_t ENC_DPP_ERR            : 1;         ///<BIT [2] Encode Data Path Protection Error.
#if (_SOC_REV_ >= 0x0210)
        uint16_t MPECC_ERR_STATUS_INVLD : 1;         ///<BIT [3] MPECC Error Status Invalid.
#else
        uint16_t MPECC_ERR_STATUS_VLD   : 1;         ///<BIT [3] MPECC Error Status Valid.
#endif
        uint16_t RAID_ENC_0CHK_ERR      : 1;         ///<BIT [4] RAID Encoder Zero Check Error.
        uint16_t ENC_HLBA_MMATCH        : 1;         ///<BIT [5] Encode HLBA Mismatch.
#if (_SOC_REV_ >= 0x0210)
        uint16_t RF200C040_15_6_RSVD    :10;         ///<BIT [15:6] Reserved.
#else
        uint16_t AXI_PAR_ERR            : 1;         ///<BIT [6] AXI Parity Error.
        uint16_t RF200C040_15_7_RSVD    : 9;         ///<BIT [15:7] Reserved.
#endif
    } b;
} EcuEncodeStat0_t;

///  @brief 0xF200C044 : ECU Encode Status 1b
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ENC_AU_DONE_ID_17_16 : 2;         ///<BIT [1:0] Encode Allocation Unit Done ID Bits [17:16].
        uint16_t RF200C044_15_2_RSVD  :14;         ///<BIT [15:2] Reserved.
    } b;
} EcuEncodeStat1b_t;

///  @brief 0xF200C048 : ECU Encoder DPP Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C048_0_RSVD      : 1;         ///<BIT [0] Reserved.
        uint16_t ENC_DS_DPP_ERR        : 1;         ///<BIT [1] Encoder Data Scrambler Data Path Protection Error.
        uint16_t RAID_ENC_DPP_ERR      : 1;         ///<BIT [2] RAID Encoder Data Path Protection Error.
        uint16_t SEQ_AUX_DPP_ERR       : 1;         ///<BIT [3] Sequencer Auxiliary Data Path Protection Error.
        uint16_t AUX_DPP_ERR           : 1;         ///<BIT [4] Auxiliary Data Path Protection Error.
        uint16_t HLBA_REF_DPP_ERR      : 1;         ///<BIT [5] HLBA Reference Data Path Protection Error.
        uint16_t ENC_AEFIFO_DPP_ERR    : 1;         ///<BIT [6] Encoder AXI-ECU FIFO Data Path Protection Error.
        uint16_t ENC_SCR_SEED_DPP_ERR  : 1;         ///<BIT [7] Encoder Scrambler Seed Data Path Protection Error.
        uint16_t ENC_MCRC_DPP_ERR      : 1;         ///<BIT [8] Encoder MCRC Data Path Protection Error.
        uint16_t ENC_MPECC_MEM_DPP_ERR : 1;         ///<BIT [9] Encoder MPECC Memory Data Path Protection Error.
        uint16_t SCR_SEED_MAP_DPP_ERR  : 1;         ///<BIT [10] Scrambler Seed Map Data Path Protection Error.
        uint16_t RF200C048_13_11_RSVD  : 3;         ///<BIT [13:11] Reserved.
        uint16_t ENC_CW_BUF_DPP_ERR    : 1;         ///<BIT [14] Encoder Codeword Buffer Data Path Protection Error.
        uint16_t LDPC_ENC_CMEM_DPP_ERR : 1;         ///<BIT [15] LDPC Encoder Code Memory Data Path Protection Error.
    } b;
} EcuEncDppStat_t;

///  @brief 0xF200C050 : ECU Decoder Status 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECU_DEC_RESUME        : 1;         ///<BIT [0] ECU Decode Resume.
        uint16_t CUR_AU_UNCORR         : 1;         ///<BIT [1] Current Allocation Unit Uncorrectable.
        uint16_t CUR_AU_OVRLMT         : 1;         ///<BIT [2] Current Allocation Unit Over Limit.
        uint16_t CUR_AU_META_MCRC_ERR  : 1;         ///<BIT [3] Current Allocation Unit Metadata MCRC Error.
        uint16_t CUR_AU_MCRC_ERR       : 1;         ///<BIT [4] Current Allocation Unit MCRC Error.
        uint16_t CUR_AU_AUX_MMATCH     : 1;         ///<BIT [5] Current Allocation Unit Auxiliary Mismatch.
        uint16_t CUR_AU_ERASE          : 1;         ///<BIT [6] Current Allocation Unit Erase.
        uint16_t CUR_AU_16             : 1;         ///<BIT [7] Current Allocation Unit 16.
        uint16_t CUR_DEC_DPP_ERR       : 1;         ///<BIT [8] Current Decoder Data Path Protection Error.
        uint16_t RF200C050_9_RSVD      : 1;         ///<BIT [9] Reserved.
        uint16_t CUR_RAID_DEC_DPP_ERR  : 1;         ///<BIT [10] Current RAID Decoder Data Path Protection Error.
        uint16_t CUR_RAID_DEC_0CHK_ERR : 1;         ///<BIT [11] Current RAID Decoder Zero Check Error.
        uint16_t CUR_AXI_DEC_ERR       : 1;         ///<BIT [12] Current AXI Decoder Error.
        uint16_t CUR_AU_AU_OVRLMT      : 1;         ///<BIT [13] Current AU Allocation Unit Over Limit.
        uint16_t MAX_AU_NUM            : 1;         ///<BIT [14] Maximum Allocation Unit Number.
        uint16_t CUR_HWLL_ERR          : 1;         ///<BIT [15] Current Allocation Unit on Read Path with HWLL Error.
    } b;
} EcuDecStat0_t;

///  @brief 0xF200C052 : ECU Decoder Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CWX_UNCLEAN          : 1;         ///<BIT [0] CWX Unclean.
        uint16_t CWX_UNCORR           : 1;         ///<BIT [1] CWX Uncorrectable.
        uint16_t CWX_OVRLMT           : 1;         ///<BIT [2] CWX Over Limit.
        uint16_t RF200C052_8_3_RSVD   : 6;         ///<BIT [8:3] Reserved.
        uint16_t CWX_IDEC_ABORTED     : 1;         ///<BIT [9] CWX Inline Decoder Aborted.
        uint16_t RF200C052_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;
} EcuDecStat1_t;

///  @brief 0xF200C054 : ECU Decoder Status 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t AU_DEC_STATUS_PTR    : 4;         ///<BIT [3:0] Allocation Unit Decoder Status Pointer.
        uint16_t RF200C054_7_4_RSVD   : 4;         ///<BIT [7:4] Reserved.
#if (_SOC_REV_ >= 0x0210)
        uint16_t CW_DEC_STAT_PTR      : 2;         ///<BIT [9:8] Codeword Decoder Status Pointer.
        uint16_t RF200C054_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
#else
        uint16_t CW_DEC_STAT_PTR      : 4;         ///<BIT [11:8] Codeword Decoder Status Pointer.
        uint16_t RF200C054_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
#endif
    } b;
} EcuDecStat2_t;

///  @brief 0xF200C056 : ECU Decoder Status 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C056_0_RSVD      : 1;         ///<BIT [0] Reserved.
        uint16_t PTR_AU_UNCORR         : 1;         ///<BIT [1] Pointer Allocation Unit Uncorrectable.
        uint16_t PTR_AU_CW_OVRLMT      : 1;         ///<BIT [2] Pointer Allocation Unit Codeword Over Limit.
        uint16_t PTR_AU_META_MCRC_ERR  : 1;         ///<BIT [3] Pointer Allocation Unit Metadata MCRC Error.
        uint16_t PTR_AU_MCRC_ERR       : 1;         ///<BIT [4] Pointer Allocation Unit MCRC Error.
        uint16_t PTR_AU_AUX_MMATCH     : 1;         ///<BIT [5] Pointer Allocation Unit Auxiliary Mismatch.
        uint16_t PTR_AU_ERASE          : 1;         ///<BIT [6] Pointer Allocation Unit Erase.
        uint16_t PTR_AU_16             : 1;         ///<BIT [7] Pointer Allocation Unit 16.
        uint16_t PTR_DEC_DPP_ERR       : 1;         ///<BIT [8] Pointer Decoder Data Path Protection Error.
        uint16_t RF200C056_9_RSVD      : 1;         ///<BIT [9] Reserved.
        uint16_t PTR_RAID_DEC_DPP_ERR  : 1;         ///<BIT [10] The AU pointed by AU_DEC_STAT_PTR (RF200C054h [3:0]) has a RAID.
        uint16_t PTR_RAID_DEC_0CHK_ERR : 1;         ///<BIT [11] Pointer RAID Decoder Zero Check Error.
        uint16_t PTR_AXI_DEC_ERR       : 1;         ///<BIT [12] Pointer AXI Decoder Error.
        uint16_t PTR_AU_OVRLMT         : 1;         ///<BIT [13] Pointer Allocation Unit Over Limit.
        uint16_t RF200C056_14_RSVD     : 1;         ///<BIT [14] Reserved.
        uint16_t PTR_HWLL_ERR          : 1;         ///<BIT [15] Pointer Hardware Link List Error.
    } b;
} EcuDecStat3_t;

///  @brief 0xF200C058 : ECU Decoder Status 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CWX_CORR_CNT         :10;         ///<BIT [9:0] CWX Correction Count.
        uint16_t RF200C058_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;
} EcuDecStat4_t;

///  @brief 0xF200C05A : ECU Decoder Status 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t AU_CORR_CNT          :12;         ///<BIT [11:0] Allocation Unit Correction Count.
        uint16_t RF200C05A_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcuDecStat5_t;

///  @brief 0xF200C06C : ECU Decoder Status 14
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CWX_CORR_CNT_0TO1    :10;         ///<BIT [9:0] CWX Correction Count Zero to One.
        uint16_t RF200C06C_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;
} EcuDecStat14_t;

///  @brief 0xF200C06E : ECU Decoder Status 15
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CUR_AU_TID          : 7;         ///<BIT [6:0] Current Allocation Unit Tag ID.
        uint16_t RF200C06E_15_7_RSVD : 9;         ///<BIT [15:7] Reserved.
    } b;
} EcuDecStat15_t;

///  @brief 0xF200C070 : ECU AXI WRITE Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C070_3_0_RSVD    : 4;         ///<BIT [3:0] Reserved.
        uint16_t ECU_AXI_CW_LEN        : 4;         ///<BIT [7:4] ECU AXI Codeword Write Burst Length.
        uint16_t RF200C070_11_8_RSVD   : 4;         ///<BIT [11:8] Reserved.
        uint16_t ECU_AXI_WR_BUFFERABLE : 1;         ///<BIT [12] ECU AXI Write Bufferable.
        uint16_t RF200C070_15_13_RSVD  : 3;         ///<BIT [15:13] Reserved.
    } b;
} EcuAxiWriteCfg1_t;

///  @brief 0xF200C072 : ECU AXI WRITE Configuration 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C072_2_0_RSVD  : 3;         ///<BIT [2:0] Reserved.
        uint16_t ECU_AXI_CW_MAXRSP   : 5;         ///<BIT [7:3] ECU AXI Codeword Maximum Response.
        uint16_t RF200C072_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;
} EcuAxiWriteCfg2_t;

///  @brief 0xF200C074 : ECU AXI Write Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C074_2_0_RSVD   : 3;         ///<BIT [2:0] Reserved.
        uint16_t ECU_AXI_WR_XFER_DONE : 1;         ///<BIT [3] ECU AXI Write Transfer Done.
        uint16_t RF200C074_15_4_RSVD  :12;         ///<BIT [15:4] Reserved.
    } b;
} EcuAxiWrStat_t;

///  @brief 0xF200C078 : ECU AXI Read Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C078_3_0_RSVD    : 4;         ///<BIT [3:0] Reserved.
        uint16_t ECU_AXI_CR_LEN        : 4;         ///<BIT [7:4] ECU AXI Read Burst Length.
        uint16_t ECU_AXI_CR_LOCK       : 2;         ///<BIT [9:8] ECU AXI Read Lock Type.
        uint16_t RF200C078_11_10_RSVD  : 2;         ///<BIT [11:10] Reserved.
        uint16_t ECU_AXI_RD_BUFFERABLE : 1;         ///<BIT [12] ECU AXI Read Bufferable.
        uint16_t RF200C078_15_13_RSVD  : 3;         ///<BIT [15:13] Reserved.
    } b;
} EcuAxiRdCfg1_t;

///  @brief 0xF200C07A : ECU AXI Read Configuration 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECU_AXI_CR_PROT     : 3;         ///<BIT [2:0] ECU AXI Protection Type.
        uint16_t ECU_AXI_CR_MAXREQ   : 5;         ///<BIT [7:3] ECU AXI Maximum Outstanding Read Response.
        uint16_t RF200C07A_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;
} EcuAxiRdCfg2_t;

///  @brief 0xF200C07C : ECU AXI Read Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C07C_2_0_RSVD   : 3;         ///<BIT [2:0] Reserved.
        uint16_t ECU_AXI_RD_XFER_DONE : 1;         ///<BIT [3] ECU AXI Read Transfer Done.
        uint16_t RF200C07C_15_4_RSVD  :12;         ///<BIT [15:4] Reserved.
    } b;
} EcuAxiRdStat_t;

///  @brief 0xF200C080 : AXI ID Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t AXI_RWID_0          : 4;         ///<BIT [3:0] AXI ID Choice 0.
        uint16_t RF200C080_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;
} AxiIdCfg1_t;

///  @brief 0xF200C086 : ECU Abort Control Signal
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECU_ABT_DIS_WR_REQ  : 1;         ///<BIT [0] ECU Abort Disable Write Request.
        uint16_t ECU_ABT_DIS_RD_REQ  : 1;         ///<BIT [1] ECU Abort Disable Read Request.
        uint16_t RF200C086_15_2_RSVD :14;         ///<BIT [15:2] Reserved.
    } b;
} EcuAbortCtrlSgn_t;

///  @brief 0xF200C0D0 : ECU RAID Control and Status 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC_RAID_EN              : 1;         ///<BIT [0] ECC RAID Enable.
        uint16_t ECC_RAID_SRST            : 1;         ///<BIT [1] ECC RAID Soft Reset.
#if (_SOC_REV_ >= 0x0210)
        uint16_t RF200C0D0_15_2_RSVD :14;         ///<BIT [15:2] Reserved.
#else
        uint16_t RF200C0D0_2_RSVD         : 1;         ///<BIT [2] Reserved.
        uint16_t ECC_RAID_SEQSRST_DISABLE : 1;         ///<BIT [3] ECC RAID Sequencer Soft Reset Disable.
        uint16_t RF200C0D0_15_4_RSVD      :12;         ///<BIT [15:4] Reserved.
#endif
    } b;
} EcuRaidCtrlAndStat0_t;

///  @brief 0xF200C0D6 : ECU RAID Control and Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RAID_CUR_DPP_ERR_ENC : 1;         ///<BIT [0] RAID Current Data Path Protection Error Encoder.
        uint16_t RF200C0D6_7_1_RSVD   : 7;         ///<BIT [7:1] Reserved.
        uint16_t RAID_CUR_DPP_ERR_DEC : 1;         ///<BIT [8] RAID Current Data Path Protection Error Decoder.
        uint16_t RF200C0D6_11_9_RSVD  : 3;         ///<BIT [11:9] Reserved.
        uint16_t RAID_PTR_DPP_ERR_DEC : 1;         ///<BIT [12] RAID Pointer Data Path Protection Error Decoder.
        uint16_t RF200C0D6_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
    } b;
} EcuRaidCtrlAndStat1_t;

///  @brief 0xF200C0D8 : ECU RAID Control and Status 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C0D8_7_0_RSVD  : 8;         ///<BIT [7:0] Reserved.
        uint16_t RAID_ADDR_ID_PTR    : 4;         ///<BIT [11:8] RAID Address ID Pointer.
        uint16_t RAID_ADDR_SUBID_PTR : 3;         ///<BIT [14:12] RAID Address Sub ID Pointer.
        uint16_t RF200C0D8_15_RSVD   : 1;         ///<BIT [15] Reserved.
    } b;
} EcuRaidCtrlAndStat4_t;

///  @brief 0xF200C0DA : ECU RAID Control and Status 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RAID_SUBID_START_ADDR :13;         ///<BIT [12:0] RAID Sub ID Start Address.
        uint16_t RF200C0DA_15_13_RSVD  : 3;         ///<BIT [15:13] Reserved.
    } b;
} EcuRaidCtrlAndStat5_t;

///  @brief 0xF200C0DC : RAID Buffer Power Down Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
#if (_SOC_REV_ >= 0x0210)
        uint16_t RF200C0DC_7_0_RSVD   : 8;         ///<BIT [7:0] Reserved.
        uint16_t RAID_AS_SPM_EN       : 1;         ///<BIT [8] Enable RAID as Scratchpad Memory.
        uint16_t RF200C0DC_11_9_RSVD  : 3;         ///<BIT [11:9] Reserved.
#else
        uint16_t RF200C0DC_11_0_RSVD  :12;         ///<BIT [11:0] Reserved.
#endif
        uint16_t RAID_BUF_DPP_ERR_INJ : 1;         ///<BIT [12] RAID Buffer Data Path Protection Error Injection.
        uint16_t RF200C0DC_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
    } b;
} RaidBufPowerDnCtrl_t;

///  @brief 0xF200C120 : ECU Deep Retry Mode Control and Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DRTM_ON_UNCORR          : 1;         ///<BIT [0] Deep Retry Mode On Uncorrectable.
        uint16_t DRTM_DEC_START          : 1;         ///<BIT [1] Deep Retry Mode Decoder Start.
        uint16_t DRTM_ABORT_AU           : 1;         ///<BIT [2] Deep Retry Mode Abort Allocation Unit.
        uint16_t DRTM_HED_MEM_INIT       : 1;         ///<BIT [3] Deep Retry Mode Hard Error Detection Memory Initialization.
        uint16_t DRTM_HERR_COLLECTION_EN : 2;         ///<BIT [5:4] Deep Retry Mode Hard Error Detection Function Enable.
        uint16_t RF200C120_15_6_RSVD     :10;         ///<BIT [15:6] Reserved.
    } b;
} EcuDeepRetryModeCtrlAndCfg1_t;

///  @brief 0xF200C122 : ECU Deep Retry Mode Control and Configuration 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DRTM_ABORT_AU_TID   : 7;         ///<BIT [6:0] Deep Retry Mode Aborted Allocation Unit Tag ID.
        uint16_t RF200C122_15_7_RSVD : 9;         ///<BIT [15:7] Reserved.
    } b;
} EcuDeepRetryModeCtrlAndCfg2_t;

#if (_SOC_REV_ < 0x0210)
///  @brief 0xF200C12C : ECU Deep Retry Status 1a
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DRTM_CW_ERR_CNT_DIFF    : 8;         ///<BIT [7:0] Deep Retry Mode Codeword Error Count Difference.
        uint16_t RF200C12C_9_8_RSVD      : 2;         ///<BIT [9:8] Reserved.
        uint16_t DRTM_CW_ERR_CNT_SUM_5_0 : 6;         ///<BIT [15:10] Deep Retry Mode Codeword Error Count Sum Bits [5:0].
    } b;
} EcuDeepRetryStat1a_t;

///  @brief 0xF200C12E : ECU Deep Retry Status 1b
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DRTM_CW_ERR_CNT_SUM_7_6 : 2;         ///<BIT [1:0] Deep Retry Mode Codeword Error Count Sum Bits [7:6].
        uint16_t RF200C12E_15_2_RSVD     :14;         ///<BIT [15:2] Reserved.
    } b;
} EcuDeepRetryStat1b_t;
#endif

///  @brief 0xF200C130 : ECU Deep Retry Status 2a
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
#if (_SOC_REV_ >= 0x0210)
        uint16_t DRTM_ITR_CW_ERR_CNT_DIFF    :11;         ///<BIT [10:0] Deep Retry Mode LDPC Codeword Error Count Difference.
        uint16_t RF200C130_15_11_RSVD        : 5;         ///<BIT [15:11] Reserved.
#else
        uint16_t DRTM_ITR_CW_ERR_CNT_DIFF    : 8;         ///<BIT [7:0] Deep Retry Mode Iterative Codeword Error Count Difference.
        uint16_t RF200C130_9_8_RSVD          : 2;         ///<BIT [9:8] Reserved.
        uint16_t DRTM_ITR_CW_ERR_CNT_SUM_5_0 : 6;         ///<BIT [15:10] Deep Retry Mode Iterative Codeword Error Count Sum Bits [5:0].
#endif
    } b;
} EcuDeepRetryStat2a_t;

///  @brief 0xF200C132 : ECU Deep Retry Status 2b
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
#if (_SOC_REV_ >= 0x0210)
        uint16_t DRTM_ITR_CW_ERR_CNT_SUM     :11;         ///<BIT [10:0] Deep Retry Mode LDPC Codeword Error Count Sum Bits.
        uint16_t RF200C132_15_11_RSVD        : 5;         ///<BIT [15:11] Reserved.
#else
        uint16_t DRTM_ITR_CW_ERR_CNT_SUM_7_6 : 2;         ///<BIT [1:0] Deep Retry Mode Iterative Codeword Error Count Sum Bits [7:6].
        uint16_t RF200C132_15_2_RSVD         :14;         ///<BIT [15:2] Reserved.
#endif
    } b;
} EcuDeepRetryStat2b_t;

///  @brief 0xF200C140 : HW RRD Control Reg
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t MAX_HW_RRD_AU_NUM     : 6;         ///<BIT [5:0] Maximum Hardware Re-Read Allocation Unit Number.
        uint16_t FW_RRD_EN             : 1;         ///<BIT [6] Firmware Re-Read Enable.
        uint16_t HWLL_ABORT_ON_DEC_ERR : 1;         ///<BIT [7] Hardware Link List Abort on AU Error.
        uint16_t RF200C140_10_8_RSVD   : 3;         ///<BIT [10:8] Reserved.
        uint16_t HW_RRD_ON_UNCORR      : 1;         ///<BIT [11] Hardware Re-Read on Uncorrectable Error.
        uint16_t HW_RRD_ON_MCRC_ERR    : 1;         ///<BIT [12] Hardware Re-Read on MCRC Error.
        uint16_t HW_RRD_ON_AUX_MMATCH  : 1;         ///<BIT [13] Hardware Re-Read on Auxiliary Mismatch.
        uint16_t HW_RRD_ON_DPP_ERR     : 1;         ///<BIT [14] Hardware Re-Read on Data Path Protection Error.
        uint16_t HW_RRD_ON_AXI_ERR     : 1;         ///<BIT [15] Hardware Re-Read on AXI Error.
    } b;
} HwRrdCtrlRegister_t;

///  @brief 0xF200C142 : HWLL Error Status 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
#if (_SOC_REV_ >= 0x0210)
        uint16_t HWLL_ERROR_CODE      : 4;         ///<BIT [3:0] Hardware Linked List Error Code from HWLL Builder.
        uint16_t RF200C142_5_4_RSVD   : 2;         ///<BIT [5:4] Reserved.
        uint16_t HWLL_ERROR_ATV_PTR   : 7;         ///<BIT [12:6] Hardware Linked List Error ATV Pointer.
        uint16_t RF200C142_14_13_RSVD : 2;         ///<BIT [14:13] Reserved.
        uint16_t HWLL_ABORT_FINISHED  : 1;         ///<BIT [15] Hardware Linked List Abort Finished.
#else
        uint16_t HWLL_ERROR_CODE     : 2;         ///<BIT [1:0] Hardware Link List Error Code from HWLL Builder.
        uint16_t HWLL_ERROR_ATV_PTR  : 6;         ///<BIT [7:2] Active Pointer Value for HWLL EC1 Error.
        uint16_t HWLL_ABORT_FINISHED : 1;         ///<BIT [8] Hardware Link List Abort Finished.
        uint16_t RF200C142_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
#endif
    } b;
} HwllErrStat0_t;

///  @brief 0xF200C144 : HWLL Error Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HWLL_ERROR_CMD_ID_SAVED : 1;         ///<BIT [0] HWLL Command ID for HWLL EC2 and EC3 Errors Save.
        uint16_t HWLL_ERROR_CMD_ID       : 9;         ///<BIT [9:1] HWLL Command ID for HWLL EC2 and EC3 Errors.
        uint16_t RF200C144_15_10_RSVD    : 6;         ///<BIT [15:10] Reserved.
    } b;
} HwllErrStat1_t;

///  @brief 0xF200C146 : HWLL Error Status 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HWLL_CLEAN_CMD_ID   : 9;         ///<BIT [8:0] HWLL Clean Command ID.
        uint16_t RF200C146_15_9_RSVD : 7;         ///<BIT [15:9] Reserved.
    } b;
} HwllErrStat2_t;

///  @brief 0xF200C148 : HWLL Error Status 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HWLL_ERROR_CODE_MASK : 1;         ///<BIT [0] HWLL Builder Error Code Disable.
        uint16_t RF200C148_15_1_RSVD  :15;         ///<BIT [15:1] Reserved.
    } b;
} HwllErrStat3_t;

///  @brief 0xF200C180 : ECU Decoder Status 16
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DEC_AEDF_DPP_ERR       : 1;         ///<BIT [0] Decoder AXI-ECU Data Path Protection Error.
        uint16_t SEQ_AUX_REF_PARITY_ERR : 1;         ///<BIT [1] Sequencer Auxiliary Reference Parity Error.
        uint16_t RAID_DEC_DPP_ERR       : 1;         ///<BIT [2] RAID Decoder Data Path Protection Error.
        uint16_t BCH_DECBUF_DPP_ERR     : 1;         ///<BIT [3] BCH Decoder Buffer Data Path Protection Error.
        uint16_t BCH_ERRBUF_DPP_ERR     : 1;         ///<BIT [4] BCH Error Buffer Data Path Protection Error.
        uint16_t ERRCNT_DPP_ERR         : 1;         ///<BIT [5] Error Count Data Path Protection Error.
        uint16_t XTRCT_AUX_DPP_ERR      : 1;         ///<BIT [6] Extracted Auxiliary Data Path Protection Error.
        uint16_t AUX_MSK_DPP_ERR        : 1;         ///<BIT [7] Auxiliary Mask Data Path Protection Error.
        uint16_t DEC_SCR_SEED_DPP_ERR   : 1;         ///<BIT [8] Decoder Scrambler Seed Data Path Protection Error.
        uint16_t AUX_REF_DPP_ERR        : 1;         ///<BIT [9] Reserved.
        uint16_t DEC_AEFIFO_DPP_ERR     : 1;         ///<BIT [10] Decoder AXI-ECU FIFO Data Path Protection Error.
        uint16_t DEC_THR_BUF_DPP_ERR    : 1;         ///<BIT [11] Decoder Threshold Buffer Data Path Protection Error.
        uint16_t DEC_INPUT_BUF_DPP_ERR  : 1;         ///<BIT [12] Decoder Input Buffer Data Path Protection Error.
        uint16_t MCRC_MEM_DPP_ERR       : 1;         ///<BIT [13] MCRC Memory Data Path Protection Error.
        uint16_t DS_SEED_MEM_DPP_ERR    : 1;         ///<BIT [14] Descrambler Seed Memory Data Path Protection Error.
        uint16_t DS_DPP_ERR             : 1;         ///<BIT [15] Descrambler Data Path Protection Error.
    } b;
} EcuDecStat16_t;

///  @brief 0xF200C184 : ECU Decoder Status 18
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CWX_SIZE             :12;         ///<BIT [11:0] CWX Size.
        uint16_t RF200C184_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcuDecStat18_t;

///  @brief 0xF200C188 : ECU Decoder Status 19b
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CUR_DEC_AU_DONE_ID_17_16 : 2;         ///<BIT [1:0] Current Decoder Allocation Unit Done ID Bits [17:16].
        uint16_t RF200C188_15_2_RSVD      :14;         ///<BIT [15:2] Reserved.
    } b;
} EcuDecStat19b_t;

///  @brief 0xF200C18E : ECU Decoder Status 20b
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CUR_DEC_AU_DONE_17_16 : 2;         ///<BIT [1:0] Current Decoder Allocation Unit Done ID Pointer Bits [17:16].
        uint16_t RF200C18E_15_2_RSVD   :14;         ///<BIT [15:2] Reserved.
    } b;
} EcuDecStat20b_t;

///  @brief 0xF200C192 : ECU Decoder Status 21
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CUR_16HISTORY_READ_PTR : 4;         ///<BIT [3:0] Current 16 Allocation Unit History Read Pointer.
        uint16_t RF200C192_15_4_RSVD    :12;         ///<BIT [15:4] Reserved.
    } b;
} EcuDecStat21_t;

///  @brief 0xF200C198 : ECU Decoder Status 24
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C198_7_0_RSVD   : 8;         ///<BIT [7:0] Reserved.
        uint16_t PTR_IDEC_ABORT       : 4;         ///<BIT [11:8] Pointer Inline Decoder Abort.
        uint16_t RF200C198_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcuDecStat24_t;

///  @brief 0xF200C19A : ECU Decoder Status 25
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DEC_DMA_BUF_DPP_ERR : 1;         ///<BIT [0] Decoder Direct Memory Access Buffer Data Path Protection Error.
        uint16_t RF200C19A_15_1_RSVD :15;         ///<BIT [15:1] Reserved.
    } b;
} EcuDecStat25_t;

///  @brief 0xF200C1AA : ACCM ECU Decoder Status 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ACCM_VREF_CNT_CH_EN : 4;         ///<BIT [3:0] Accumulate VREF Counter Channel Enable.
        uint16_t RF200C1AA_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;
} AccmEcuDecStat5_t;

///  @brief 0xF200C200 : ECU AXI Interrupt
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ENC_AXI0_ID_ERR_INT       : 1;         ///<BIT [0] Encoder AXI Port 0 ID Error Interrupt.
#if (_SOC_REV_ >= 0x0210)
        uint16_t RF200C200_1_RSVD          : 1;         ///<BIT [1] Reserved.
#else
        uint16_t ENC_AXI0_PAR_ERR_INT      : 1;         ///<BIT [1] Encoder AXI Port 0 Parity Error Interrupt.
#endif
        uint16_t ENC_AXI0_RD_BRESP_ERR_INT : 1;         ///<BIT [2] Encoder AXI Port 0 Read BRESP Error Interrupt.
        uint16_t ENC_AXI0_RD_BLEN_ERR_INT  : 1;         ///<BIT [3] Encoder AXI Port 0 Read BLEN Error Interrupt.
        uint16_t DEC_AXI0_ID_ERR_INT       : 1;         ///<BIT [4] Decoder AXI Port 0 ID Error Interrupt.
        uint16_t RF200C200_15_5_RSVD       :11;         ///<BIT [15:5] Reserved.
    } b;
} EcuAxiIntr_t;

///  @brief 0xF200C20A : ECU AXI Interrupt CPU Mask
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ENC_AXI_ID_ERR_INT_MSK       : 1;         ///<BIT [0] Encoder AXI ID Error Interrupt Mask.
        uint16_t RF200C20A_1_RSVD             : 1;         ///<BIT [1] Reserved.
        uint16_t ENC_AXI_RD_BRESP_ERR_INT_MSK : 1;         ///<BIT [2] Encoder AXI Read BRESP Error Interrupt Mask.
        uint16_t ENC_AXI_RD_BLEN_ERR_INT_MSK  : 1;         ///<BIT [3] Encoder AXI Read BLEN Error Interrupt Mask.
        uint16_t DEC_AXI_ID_ERR_INT_MSK       : 1;         ///<BIT [4] Decoder AXI ID Error Interrupt Mask.
        uint16_t RF200C20A_15_5_RSVD          :11;         ///<BIT [15:5] Reserved.
    } b;
} EcuAxiIntrCpuMsk_t;

///  @brief 0xF200C20E : ECU Link List and AU ID Interrupt
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C20E_0_RSVD       : 1;         ///<BIT [0] Reserved.
        uint16_t ENC_ECU_LL_DPP_ERR_INT : 1;         ///<BIT [1] Encoder ECU Link List Data Path Protection Error Interrupt.
        uint16_t ENC_LL_ERR_INT         : 1;         ///<BIT [2] Encoder Link List Error Interrupt.
        uint16_t RF200C20E_3_RSVD       : 1;         ///<BIT [3] Reserved.
        uint16_t ENC_AU_ID_DPP_ERR_INT  : 1;         ///<BIT [4] Encoder Allocation Unit ID Data Path Protection Error Interrupt.
        uint16_t RF200C20E_8_5_RSVD     : 4;         ///<BIT [8:5] Reserved.
        uint16_t DEC_ECU_LL_DPP_ERR_INT : 1;         ///<BIT [9] Decoder ECU Link List Data Path Protection Error Interrupt.
        uint16_t DEC_LL_ERR_INT         : 1;         ///<BIT [10] Decoder Link List Error Interrupt.
        uint16_t RF200C20E_11_RSVD      : 1;         ///<BIT [11] Reserved.
        uint16_t DEC_AU_ID_DPP_ERR_INT  : 1;         ///<BIT [12] Decoder Allocation Unit ID Data Path Protection Error Interrupt.
        uint16_t RF200C20E_15_13_RSVD   : 3;         ///<BIT [15:13] Reserved.
    } b;
} EcuLnkListAndAuIdIntr_t;

///  @brief 0xF200C210 : ECU Link List and AU ID Interrupt CPU Mask
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C210_0_RSVD           : 1;         ///<BIT [0] Reserved.
        uint16_t ENC_ECU_LL_DPP_ERR_INT_MSK : 1;         ///<BIT [1] Encoder Link List Data Path Protection Error Interrupt Mask.
        uint16_t ENC_LL_ERR_INT_MSK         : 1;         ///<BIT [2] Encoder Link List Error Interrupt Mask.
        uint16_t RF200C210_3_RSVD           : 1;         ///<BIT [3] Reserved.
        uint16_t ENC_AU_ID_DPP_ERR_INT_MSK  : 1;         ///<BIT [4] Encoder Allocation Unit ID Data Path Protection Error Interrupt.
        uint16_t RF200C210_8_5_RSVD         : 4;         ///<BIT [8:5] Reserved.
        uint16_t DEC_ECU_LL_DPP_ERR_INT_MSK : 1;         ///<BIT [9] Decoder ECU Link List Data Path Protection Error Interrupt Mask.
        uint16_t DEC_LL_ERR_INT_MSK         : 1;         ///<BIT [10] Decoder Link List Error Interrupt Mask.
        uint16_t RF200C210_11_RSVD          : 1;         ///<BIT [11] Reserved.
        uint16_t DEC_AU_ID_DPP_ERR_INT_MSK  : 1;         ///<BIT [12] Decoder Allocation Unit ID Data Path Protection Error Interrupt.
        uint16_t RF200C210_15_13_RSVD       : 3;         ///<BIT [15:13] Reserved.
    } b;
} EcuLnkListAndAuIdIntrCpu_t;

///  @brief 0xF200C21A : ECU DRTM Interrupt
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DRTM_DEC_DONE_INT             : 1;         ///<BIT [0] Deep Retry Mode Decoder Done Interrupt.
        uint16_t DRTM_DEC_FAIL_INT             : 1;         ///<BIT [1] Deep Retry Mode Decoder Fail Interrupt.
        uint16_t DRTM_BIN_IDX_GEN_DONE_INT     : 1;         ///<BIT [2] Deep Retry Mode Binary Index Generation Done Interrupt.
        uint16_t RF200C21A_4_3_RSVD            : 2;         ///<BIT [4:3] Reserved.
        uint16_t DRTM_HERR_COLLECTION_DONE_INT : 1;         ///<BIT [5] Deep Retry Mode Hard Error Collection Done Interrupt.
        uint16_t RF200C21A_15_6_RSVD           :10;         ///<BIT [15:6] Reserved.
    } b;
} EcuDrtmIntr_t;

///  @brief 0xF200C21C : ECU DRTM Interrupt CPU Mask
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DRTM_DEC_DONE_INT_MSK              : 1;         ///<BIT [0] Deep Retry Mode Decoder Done Interrupt Mask.
        uint16_t DRTM_DEC_FAIL_INT_MSK              : 1;         ///<BIT [1] Deep Retry Mode Decoder Fail Interrupt Mask.
        uint16_t DRTM_BIN_IDX_GEN_DONE_INT_MSK      : 1;         ///<BIT [2] Deep Retry Mode Binary Index Generation Done Interrupt Mask.
#if (_SOC_REV_ >= 0x0210)
        uint16_t RF200C21C_4_3_RSVD                 : 2;         ///<BIT [4:3] Reserved.
#else
        uint16_t DRTM_HISTO_COLLECTION_DONE_INT_MSK : 1;         ///<BIT [3] Deep Retry Mode Histogram Collection Done Interrupt Mask.
        uint16_t RF200C21C_4_RSVD                   : 1;         ///<BIT [4] Reserved.
#endif
        uint16_t DRTM_HERR_COLLECTION_DONE_INT_MSK  : 1;         ///<BIT [5] Deep Retry Mode Hard Error Collection (for BBLD or BCD) Done In.
        uint16_t RF200C21C_15_6_RSVD                :10;         ///<BIT [15:6] Reserved.
    } b;
} EcuDrtmIntrCpuMsk_t;

///  @brief 0xF200C220 : MPECC Status 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ENC_MPECC_ERR_CW_CNT :10;         ///<BIT [9:0] Encoder MPECC Error Codeword Count.
        uint16_t RF200C220_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;
} MpeccStat0_t;

///  @brief 0xF200C222 : MPECC Status 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ENC_MPECC_ERR_CW_NUM1 :10;         ///<BIT [9:0] Encoder MPECC First Error Codeword Number.
        uint16_t RF200C222_15_10_RSVD  : 6;         ///<BIT [15:10] Reserved.
    } b;
} MpeccStat2_t;

///  @brief 0xF200C224 : MPECC Status 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ENC_MPECC_ERR_CW_NUM2 :10;         ///<BIT [9:0] Encoder MPECC Second Error Codeword Number.
        uint16_t RF200C224_15_10_RSVD  : 6;         ///<BIT [15:10] Reserved.
    } b;
} MpeccStat3_t;

///  @brief 0xF200C542 : DMA Power Down Control1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DMA_WAKEUP_CYC      : 6;         ///<BIT [5:0] Direct Memory Access Wake Up Cycle.
        uint16_t RF200C542_13_6_RSVD : 8;         ///<BIT [13:6] Reserved.
        uint16_t DMA_DYNAMIC_PD_EN   : 1;         ///<BIT [14] Direct Memory Access Dynamic Power-Down Enable.
        uint16_t DMA_PD              : 1;         ///<BIT [15] Direct Memory Access Power-Down.
    } b;
} DmaPowerDnControl1_t;

///  @brief 0xF200C564 : ECU Retry External Memory0 Configuration 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXT_MEM0_ENTRY_NUM   : 8;         ///<BIT [7:0] External Memory 0 Entry Number.
        uint16_t RF200C564_9_8_RSVD   : 2;         ///<BIT [9:8] Reserved.
        uint16_t EXT_MEM0_AXI_ID_SEL  : 2;         ///<BIT [11:10] External Memory 0 AXI ID Select.
        uint16_t RF200C564_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcuRetryExtMemory0Cfg2_t;

///  @brief 0xF200C56C : ECU Retry External Memory1 Configuration 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EXT_MEM1_ENTRY_NUM   : 8;         ///<BIT [7:0] External Memory 1 Entry Number.
        uint16_t RF200C56C_9_8_RSVD   : 2;         ///<BIT [9:8] Reserved.
        uint16_t EXT_MEM1_AXI_ID_SEL  : 2;         ///<BIT [11:10] External Memory 1 AXI ID Select.
        uint16_t RF200C56C_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcuRetryExtMemory1Cfg2_t;

///  @brief 0xF200C600 : LLR Generation 0 and 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RTM_LLR_TABLE_RST      : 1;         ///<BIT [0] Retry Mode LLR Table Reset.
        uint16_t RTM_LLR_TABLE_RD_WRB   : 1;         ///<BIT [1] Retry Mode LLR Table Read/Write Bar.
        uint16_t RTM_LLR_TABLE_PRGRM_EN : 1;         ///<BIT [2] Retry Mode LLR Table Program Enable.
        uint16_t RTM_LLR_TABLE_BYPASS   : 1;         ///<BIT [3] Retry Mode LLR Table Bypass.
        uint16_t RTM_DEC_START          : 1;         ///<BIT [4] Retry Mode DEC Start.
        uint16_t RTM_RST_LLRGEN         : 1;         ///<BIT [5] Retry Mode Reset LLR Generation.
        uint16_t RF200C600_6_RSVD       : 1;         ///<BIT [6] Reserved.
        uint16_t RF200C600_7_RSVD       : 1;         ///<BIT [7] Reserved.
        uint16_t RTM_BBLD_ACCUM_CW_NUM  : 8;         ///<BIT [15:8] Retry Mode BBLD Accumulating Codeword Number.
    } b;
} LlrGen0And1_t;

///  @brief 0xF200C604 : LLR Generation 3 and 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C604_6_0_RSVD     : 7;         ///<BIT [6:0] Reserved.
        uint16_t RF200C604_7_RSVD       : 1;         ///<BIT [7] Reserved.
        uint16_t RTM_BBLD_CMP_EN        : 1;         ///<BIT [8] RTM BBLD CMP Enable.
        uint16_t RTM_BBLD_ACCUM_EN      : 1;         ///<BIT [9] Retry Mode BBLD Accumulation Enable.
        uint16_t RTM_BCD_EN             : 1;         ///<BIT [10] Retry Mode BCD Enable.
        uint16_t RTM_LLRGEN_COMPRESS_EN : 1;         ///<BIT [11] Retry Mode LLR Generation Compression Enable.
        uint16_t RF200C604_13_12_RSVD   : 2;         ///<BIT [13:12] Reserved.
        uint16_t RF200C604_14_RSVD      : 1;         ///<BIT [14] Reserved.
        uint16_t RTM_HISTO_COLLECT_EN   : 1;         ///<BIT [15] Retry Mode Histogram Collection Mode.
    } b;
} LlrGen3And4_t;

///  @brief 0xF200C606 : LLR Generation 5 and 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RTM_HISTO_DATA :15;         ///<BIT [14:0] Retry Mode Histogram Data.
        uint16_t RTM_HISTO_INIT : 1;         ///<BIT [15] Retry Mode Histogram Initialization.
    } b;
} LlrGen5And6_t;

///  @brief 0xF200C608 : LLR Generation 7 and 8
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RTM_BBLD_THR              : 8;         ///<BIT [7:0] Retry Mode BBLD Threshold.
        uint16_t LLRGEN_MEM_PDLVMC         : 1;         ///<BIT [8] LLR Generation Memory Power-Down LVMC.
        uint16_t LLRGEN_MEM_PDFVSSM        : 1;         ///<BIT [9] LLR Generation Memory Power-Down FVSSM.
        uint16_t RF200C608_10_RSVD         : 1;         ///<BIT [10] Reserved.
        uint16_t RTM_ERASURE_EN            : 1;         ///<BIT [11] Retry Mode Erasure Enable.
        uint16_t RD_ERASURE_LOC_RDY        : 1;         ///<BIT [12] Retry Mode Erasure Locations Ready.
        uint16_t RTM_ERASURE_LOC_RW_SEL    : 1;         ///<BIT [13] Retry Mode Erasure Locations Read/Write Select.
        uint16_t RTM_ERASURE_LOC_UNMARK_EN : 1;         ///<BIT [14] Retry Mode Erasure Locations Unmark Enable.
        uint16_t RTM_ERASURE_LOC_CLR       : 1;         ///<BIT [15] Retry Mode Erasure Locations Clear.
    } b;
} LlrGen7And8_t;

///  @brief 0xF200C60A : LLR Generation 9 and 10
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RTM_ERASURE_LOC   :15;         ///<BIT [14:0] Retry Mode Erasure Location.
        uint16_t RF200C60A_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;
} LlrGen9And10_t;

///  @brief 0xF200C60C : LLR Generation 11 and 12
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RTM_BCD_LUT_RST      : 1;         ///<BIT [0] Retry Mode BCD Look-Up Table Reset.
        uint16_t RTM_BCD_LUT_RD_WRB   : 1;         ///<BIT [1] Retry Mode BCD Look-Up Table Read/Write Bar.
        uint16_t RF200C60C_2_RSVD     : 1;         ///<BIT [2] Reserved.
        uint16_t RF200C60C_3_RSVD     : 1;         ///<BIT [3] Reserved.
#if (_SOC_REV_ >= 0x0210)
        uint16_t RF200C60C_5_4_RSVD   : 2;         ///<BIT [5:4] Reserved.
        uint16_t RF200C60C_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
#else
        uint16_t LLRGEN_MEM_WTC       : 2;         ///<BIT [5:4] LLR Generation Memory Write Timing Control.
        uint16_t LLRGEN_MEM_RTC       : 2;         ///<BIT [7:6] LLR Generation Memory Read Timing Control.
#endif
        uint16_t RF200C60C_11_8_RSVD  : 4;         ///<BIT [11:8] Reserved.
        uint16_t RF200C60C_14_12_RSVD : 3;         ///<BIT [14:12] Reserved.
        uint16_t RF200C60C_15_RSVD    : 1;         ///<BIT [15] Reserved.
    } b;
} LlrGen11And12_t;

///  @brief 0xF200C60E : LLR Generation 13 and 14
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RTM_LLRGEN_COMPRESS_WR_WIN_LEN   : 4;         ///<BIT [3:0] Retry Mode LLR Generation Compression Write Window Length.
        uint16_t RTM_LLRGEN_COMPRESS_WR_WIN_START : 4;         ///<BIT [7:4] Retry Mode LLR Generation Compression Write Window Start.
        uint16_t RTM_LLRGEN_COMPRESS_RD_WIN_LEN   : 4;         ///<BIT [11:8] Retry Mode LLR Generation Compression Read Window Length.
        uint16_t RTM_LLRGEN_COMPRESS_RD_WIN_START : 4;         ///<BIT [15:12] Retry Mode LLR Generation Compression Read Window Start.
    } b;
} LlrGen13And14_t;

///  @brief 0xF200C610 : LLR Generation 15 and 16
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RTM_NUM_ERASURE_14_0 :15;         ///<BIT [14:0] Retry Mode Number of Erasures Bits [14:0].
        uint16_t RTM_NUM_ERASURE_CLR  : 1;         ///<BIT [15] Retry Mode Number of Erasures Clear.
    } b;
} LlrGen15And16_t;

///  @brief 0xF200C614 : LLR Generation 19 and 20
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C614_8_0_RSVD    : 9;         ///<BIT [8:0] Reserved.
        uint16_t RTM_NUM_ERASURE_19_15 : 5;         ///<BIT [13:9] RTM Number Erasure Bits [19:15].
        uint16_t RF200C614_15_14_RSVD  : 2;         ///<BIT [15:14] Reserved.
    } b;
} LlrGen19And20_t;

///  @brief 0xF200C67A : LLR Power Down Control1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t LLR_WAKEUP_CYC      : 6;         ///<BIT [5:0] LLR Wake Up Cycle.
        uint16_t RF200C67A_13_6_RSVD : 8;         ///<BIT [13:6] Reserved.
        uint16_t LLR_DYNAMIC_PD_EN   : 1;         ///<BIT [14] LLR Dynamic Power-Down Enable.
        uint16_t LLR_PD              : 1;         ///<BIT [15] LLR Power-Down.
    } b;
} LlrPowerDnControl1_t;

///  @brief 0xF200C67C : Two-Bit Information Generator Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C67C_1_0_RSVD  : 2;         ///<BIT [1:0] Reserved.
        uint16_t TWOBIT_2AUMODE      : 1;         ///<BIT [2] Two-Bit Two Allocation Unit Mode.
        uint16_t RF200C67C_15_3_RSVD :13;         ///<BIT [15:3] Reserved.
    } b;
} TwoBitInformatGeneratoCtrl_t;

///  @brief 0xF200C80C : EC Core 0 Iterative Decoder Control and Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC0_ITR_DEC_CONFIG_RD_SEL   : 3;         ///<BIT [2:0] Iterative Decoder Configuration Read Select.
        uint16_t RF200C80C_7_3_RSVD           : 5;         ///<BIT [7:3] Reserved.
        uint16_t ECC0_ITR_DEC_CONFIG_CODESEL0 : 1;         ///<BIT [8] Iterative Decoder Configuration Code Select 0.
        uint16_t ECC0_ITR_DEC_CONFIG_RD_WRB   : 1;         ///<BIT [9] Iterative Decoder Configuration Flash Read/Write Bar.
        uint16_t ECC0_ITR_DEC_CONFIG_MODE_ON  : 1;         ///<BIT [10] Iterative Decoder Configuration Mode On.
        uint16_t ECC0_ITR_DEC_CONFIG_RDY      : 1;         ///<BIT [11] Iterative Decoder Configured and Ready.
        uint16_t ECC0_ITR_DEC_CONFIG_CODESEL1 : 1;         ///<BIT [12] Iterative Decoder Configuration Code Select 1.
        uint16_t RF200C80C_13_RSVD            : 1;         ///<BIT [13] Reserved.
        uint16_t RF200C80C_14_RSVD            : 1;         ///<BIT [14] Reserved.
        uint16_t ECC0_ITR_DEC_CONFIG_INIT     : 1;         ///<BIT [15] Iterative Decoder Configuration Initialization.
    } b;
} EcCore0ItrDecCtrlAndStat_t;

///  @brief 0xF200C810 : EC Core 0 Iterative Core Iterative Encoder Control and Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC0_ITR_ENC_CONFIG_RD_SEL   : 3;         ///<BIT [2:0] Iterative Encoder Configuration Read Select.
        uint16_t RF200C810_7_3_RSVD           : 5;         ///<BIT [7:3] Reserved.
        uint16_t ECC0_ITR_ENC_CONFIG_CODESEL0 : 1;         ///<BIT [8] Iterative Encoder Configuration Code Select 0.
        uint16_t ECC0_ITR_ENC_CONFIG_RD_WRB   : 1;         ///<BIT [9] Iterative Encoder Configuration Read/Write Bar.
        uint16_t ECC0_ITR_ENC_CONFIG_MODE_ON  : 1;         ///<BIT [10] Iterative Encoder Configuration Mode On.
        uint16_t ECC0_ITR_ENC_CONFIG_RDY      : 1;         ///<BIT [11] Iterative Encoder Configured and Ready.
        uint16_t ECC0_ITR_ENC_CONFIG_CODESEL1 : 1;         ///<BIT [12] Iterative Encoder Configuration Code Select 1.
        uint16_t RF200C810_13_RSVD            : 1;         ///<BIT [13] Reserved.
        uint16_t RF200C810_14_RSVD            : 1;         ///<BIT [14] Reserved.
        uint16_t ECC0_ITR_ENC_CONFIG_INIT     : 1;         ///<BIT [15] Iterative Encoder Configuration Initialization.
    } b;
} EcCore0ItrCoreItrEncCtrl_t;

///  @brief 0xF200C818 : EC Core 0 Iterative Code 0 Parity Length
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC0_ITR_PARITY_LEN0 : 9;         ///<BIT [8:0] Iterative Code Parity 0 Length.
        uint16_t RF200C818_15_9_RSVD  : 7;         ///<BIT [15:9] Reserved.
    } b;
} EcCore0ItrCmem0ParLen_t;

///  @brief 0xF200C81A : EC Core 0 Iterative Code 1 Parity Length
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC0_ITR_PARITY_LEN1 : 9;         ///<BIT [8:0] Iterative Code Parity 1 Length.
        uint16_t RF200C81A_15_9_RSVD  : 7;         ///<BIT [15:9] Reserved.
    } b;
} EcCore0ItrCmem1ParLen_t;

///  @brief 0xF200C81C : EC Core 0 Iterative Code 0 Codeword Length
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ITR_CW_LEN0          :12;         ///<BIT [11:0] Iterative Code Codeword 0 Length.
        uint16_t RF200C81C_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcCore0ItrCmem0CwLen_t;

///  @brief 0xF200C81E : EC Core 0 Iterative Code 1 Codeword Length
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC0_ITR_CW_LEN1     :12;         ///<BIT [11:0] Iterative Code Codeword 1 Length.
        uint16_t RF200C81E_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcCore0ItrCmem1CwLen_t;

///  @brief 0xF200C820 : EC Core 0 Iterative Schedule 0 and 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC0_SCHED_VER0   : 4;         ///<BIT [3:0] Schedule Version 0.
        uint16_t ECC0_TAPEOUT_VER0 : 4;         ///<BIT [7:4] Tapeout Version 0.
        uint16_t ECC0_SCHED_VER1   : 4;         ///<BIT [11:8] Schedule Version 1.
        uint16_t ECC0_TAPEOUT_VER1 : 4;         ///<BIT [15:12] Tapeout Version 1.
    } b;
} EcCore0ItrSchedule0And1_t;

///  @brief 0xF200C822 : EC Core 0 Iterative Schedule 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC0_SCHED_VER2      : 4;         ///<BIT [3:0] Schedule Version 2.
        uint16_t ECC0_TAPEOUT_VER2    : 4;         ///<BIT [7:4] Tapeout Version 2.
        uint16_t RF200C822_11_8_RSVD  : 4;         ///<BIT [11:8] Reserved.
        uint16_t RF200C822_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcCore0ItrSchedule2_t;

///  @brief 0xF200C826 : EC Core 0 Iterative Schedule 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C826_7_0_RSVD : 8;         ///<BIT [7:0] Reserved.
        uint16_t ECC0_SCHED_VER3    : 4;         ///<BIT [11:8] Schedule Version 3.
        uint16_t ECC0_TAPEOUT_VER3  : 4;         ///<BIT [15:12] Tapeout Version 3.
    } b;
} EcCore0ItrSchedule3_t;

///  @brief 0xF200C840 : EC Core 0 Iterative Inline Decoder 0 Control and Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C840_3_0_RSVD            : 4;         ///<BIT [3:0] Reserved.
        uint16_t RF200C840_7_4_RSVD            : 4;         ///<BIT [7:4] Reserved.
        uint16_t ECC0_ITR_IDEC0_FINAL_ITER_CNT : 6;         ///<BIT [13:8] Iterative Inline Decoder Final Iteration Count.
        uint16_t ECC0_ITR_IDEC0_RESET          : 1;         ///<BIT [14] Iterative Inline Decoder Reset.
        uint16_t ECC0_ITR_IDEC0_DISABLE        : 1;         ///<BIT [15] Iterative Inline Decoder Disable.
    } b;
} EcCore0ItrInlineDec0Ctrl_t;

///  @brief 0xF200C844 : EC Core 0 Iterative Inline Decoder0 Control and Status (ECC0_ITR_IDEC0_2B_IDX = 0)
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC0_ITR_IDEC0_MAX_NUM_ITER_0  : 6;         ///<BIT [5:0] Iterative Inline Decoder Maximum Number of Iterations 0.
        uint16_t ECC0_ITR_IDEC0_EARLY_TERM_EN_0 : 1;         ///<BIT [6] Iterative Inline Decoder Early Termination Enable 0.
        uint16_t RF200C844_7_RSVD               : 1;         ///<BIT [7] Reserved.
        uint16_t RF200C844_15_8_RSVD            : 8;         ///<BIT [15:8] Reserved.
    } b;
} EcCore0ItrInlineDecoder0CtrlAnd_t;

///  @brief 0xF200C848 : EC Core 0Iterative Inline Decoder 0 Error Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C848_7_0_RSVD        : 8;         ///<BIT [7:0] Reserved.
        uint16_t ECC0_ITR_IDEC0_ERR_CNT_EN : 1;         ///<BIT [8] Iterative Inline Decoder Error Count Enable.
        uint16_t RF200C848_9_RSVD          : 1;         ///<BIT [9] Reserved.
        uint16_t RF200C848_10_RSVD         : 1;         ///<BIT [10] Reserved.
        uint16_t RF200C848_11_RSVD         : 1;         ///<BIT [11] Reserved.
        uint16_t ECC0_ITR_IDEC0_METHOD     : 1;         ///<BIT [12] Iterative Inline Decoder Method.
        uint16_t RF200C848_14_13_RSVD      : 2;         ///<BIT [14:13] Reserved.
        uint16_t ECC0_ITR_IDEC0_2B_IDX     : 1;         ///<BIT [15] Iterative Inline Decoder 2-Bit Index.
    } b;
} EcCore0iteratiInlineDec0ErrCtrl_t;

// **==================Frank 0428 Add EC0 IDEC 1 Start================**
///  @brief 0xF200C860 : EC Core 0 Iterative Retry Decoder Control and Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C860_3_0_RSVD            : 4;         ///<BIT [3:0] Reserved.
        uint16_t RF200C860_7_4_RSVD            : 4;         ///<BIT [7:4] Reserved.
        uint16_t ECC0_ITR_IDEC1_FINAL_ITER_CNT : 6;         ///<BIT [13:8] Iterative Inline Decoder Final Iteration Count.
        uint16_t ECC0_ITR_IDEC1_RESET          : 1;         ///<BIT [14] Iterative Inline Decoder Reset.
        uint16_t ECC0_ITR_IDEC1_DISABLE        : 1;         ///<BIT [15] Iterative Inline Decoder Disable.
    } b;
} EcCore0ItrInlineDec1Ctrl_t;

///  @brief 0xF200C864 : EC Core 0 Iterative Inline Decoder1 Control and Status (ECC0_ITR_IDEC0_2B_IDX = 0)
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC0_ITR_IDEC1_MAX_NUM_ITER_0  : 6;         ///<BIT [5:0] Iterative Inline Decoder Maximum Number of Iterations 0.
        uint16_t ECC0_ITR_IDEC1_EARLY_TERM_EN_0 : 1;         ///<BIT [6] Iterative Inline Decoder Early Termination Enable 0.
        uint16_t RF200C864_7_RSVD               : 1;         ///<BIT [7] Reserved.
        uint16_t RF200C864_15_8_RSVD            : 8;         ///<BIT [15:8] Reserved.
    } b;
} EcCore0ItrInlineDecoder1CtrlAnd_t;

///  @brief 0xF200C868 : EC Core 0Iterative Inline Decoder 1 Error Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C868_7_0_RSVD        : 8;         ///<BIT [7:0] Reserved.
        uint16_t ECC0_ITR_IDEC1_ERR_CNT_EN : 1;         ///<BIT [8] Iterative Inline Decoder Error Count Enable.
        uint16_t RF200C868_9_RSVD          : 1;         ///<BIT [9] Reserved.
        uint16_t RF200C868_10_RSVD         : 1;         ///<BIT [10] Reserved.
        uint16_t RF200C868_11_RSVD         : 1;         ///<BIT [11] Reserved.
        uint16_t ECC0_ITR_IDEC1_METHOD     : 1;         ///<BIT [12] Iterative Inline Decoder Method.
        uint16_t RF200C868_14_13_RSVD      : 2;         ///<BIT [14:13] Reserved.
        uint16_t ECC0_ITR_IDEC1_2B_IDX     : 1;         ///<BIT [15] Iterative Inline Decoder 2-Bit Index.
    } b;
} EcCore0iteratiInlineDec1ErrCtrl_t;
// **==================Frank 0428 Add EC0 IDEC 1 End================**

///  @brief 0xF200C8C0 : EC Core 0 Iterative Retry Decoder Control and Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C8C0_6_0_RSVD                : 7;         ///<BIT [6:0] Reserved.
        uint16_t RF200C8C0_7_RSVD                  : 1;         ///<BIT [7] Reserved.
        uint16_t ECC0_ITR_RDEC_ERASURE_INJ_WIN_LEN : 8;         ///<BIT [15:8] Iterative Retry Decoder Erasure Injection Window Length.
    } b;
} EcCore0ItrRetryDecCtrlAnd01_t;

///  @brief 0xF200C8C2 : EC Core 0 Iterative Retry Decoder Start Location and Error Length
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC0_ITR_RDEC_ERASURE_INJ_WIN_START_LOC :12;         ///<BIT [11:0] Iterative Retry Decoder Erasure Injection Window Start Location.
        uint16_t ECC0_ITR_RDEC_ERASURE_INJ_IO_SEL        : 1;         ///<BIT [12] Iterative Retry Decoder Erasure Injection I/O Selection.
        uint16_t ECC0_ITR_RDEC_ERASURE_INJ_EN            : 1;         ///<BIT [13] Iterative Retry Decoder Erasure Injection Enable.
        uint16_t ECC0_ITR_RDEC_RESET                     : 1;         ///<BIT [14] Iterative Retry Decoder Reset.
        uint16_t RF200C8C2_15_RSVD                       : 1;         ///<BIT [15] Reserved.
    } b;
} EcCore0ItrRetryDecStartLoc_t;

///  @brief 0xF200C8C6 : EC Core 0 Iterative Retry Decoder Control and Status 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C8C6_7_0_RSVD           : 8;         ///<BIT [7:0] Reserved.
        uint16_t ECC0_ITR_RDEC_FINAL_ITER_CNT : 7;         ///<BIT [14:8] Iterative Retry Decoder Final Iteration Count.
        uint16_t FAST_RTMB_MODE               : 1;         ///<BIT [15] Fast Retry Mode.
    } b;
} EcCore0ItrRetryDecCtrlAnd12_t;

///  @brief 0xF200C8D4 : EC Core Iterative Retry Decoder 20b (FAST_RTMB_MODE = 0)
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RTM_ITR_MAX_NUM_ITER  : 7;         ///<BIT [6:0] Retry Mode Iterative Retry Decoder Maximum Number of Iterations.
        uint16_t RTM_ITR_EARLY_TERM_EN : 1;         ///<BIT [7] Retry Mode Iterative Retry Decoder Early Termination Enable.
        uint16_t RF200C8D4_15_8_RSVD   : 8;         ///<BIT [15:8] Reserved.
    } b;
} EcCoreItrRetryDec20bFastRtmb_t;

///  @brief 0xF200C900 : EC Core 0 Iterative Encoder Control and Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200C900_3_0_RSVD               : 4;         ///<BIT [3:0] Reserved.
        uint16_t ECC0_ITR_ENC_RESET               : 1;         ///<BIT [4] Iterative Encoder Reset.
        uint16_t ECC0_ITR_ENC_BYPASS              : 1;         ///<BIT [5] Iterative Encoder Bypass.
        uint16_t ECC0_ITR_ENC_D_CRC_UPDATE_FREEZE : 1;         ///<BIT [6] CRC Update Freeze.
        uint16_t RF200C900_7_RSVD                 : 1;         ///<BIT [7] Reserved.
        uint16_t RF200C900_13_8_RSVD              : 6;         ///<BIT [13:8] Reserved.
        uint16_t ECC0_ITR_ENC_CFG_CRC_ERR         : 2;         ///<BIT [15:14] Iterative Encoder Configuration CRC Error.
    } b;
} EcCore0ItrEncCtrlAndStat_t;

#if (_SOC_REV_ >= 0x0210)
///  @brief 0xF200C972 : EC Core 0 EC Core RDEC Power Down Control 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ITR_RDEC_PD_WAKEUP_CYC : 6;         ///<BIT [5:0] LDPC Retry Decoder Power-Down Wake Up Cycle.
        uint16_t RF200C972_14_6_RSVD    : 9;         ///<BIT [14:6] Reserved.
        uint16_t ITR_RDEC_PD_EN         : 1;         ///<BIT [15] LDPC Retry Decoder Power-Down Enable.
    } b;
} EcCore0EcCoreRdecPowerDn11_t;

///  @brief 0xF200C974 : EC Core 0 EC Core Encoder Power Down Control0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PD_ITR_ENC          : 1;         ///<BIT [0] Power-Down Control for LDPC Encoder on Channels 4-7.
        uint16_t RF200C974_15_1_RSVD :15;         ///<BIT [15:1] Reserved.
    } b;
} EcCore0EcCoreEncPowerDn00_t;

///  @brief 0xF200C976 : EC Core 0 EC Core Encoder Power Down Control1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PD_ECU_DP_ENC       : 4;         ///<BIT [3:0] Power-Down ECU Data Path Encoder.
        uint16_t RF200C976_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;
} EcCore0EcCoreEncPowerDn11_t;

///  @brief 0xF200C978 : EC Core 0 EC Core Decoder Power Down Control0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PD_ITR_IDEC         : 2;         ///<BIT [1:0] Power Down Control for LDPC decoder.
        uint16_t RF200C978_15_2_RSVD :14;         ///<BIT [15:2] Reserved.
    } b;
} EcCore0EcCoreDecPowerDn00_t;

///  @brief 0xF200C97A : EC Core 0 EC Core Decoder Power Down Control1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PD_ITR_RDEC         : 1;         ///<BIT [0] Power-Down LDPC Retry Decoder.
        uint16_t RF200C97A_3_1_RSVD  : 3;         ///<BIT [3:1] Reserved.
        uint16_t PD_ECU_DP_DEC       : 1;         ///<BIT [4] Power-Down ECU Data Path Decoder.
        uint16_t RF200C97A_15_5_RSVD :11;         ///<BIT [15:5] Reserved.
    } b;
} EcCore0EcCoreDecPowerDn11_t;

///  @brief 0xF200C97C : EC Core 0 EC Core IOD Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t IOD_BUF_THRSHD      : 3;         ///<BIT [2:0] Iteration On Demand Buffer Threshold.
        uint16_t RF200C97C_15_3_RSVD :13;         ///<BIT [15:3] Reserved.
    } b;
} EcCore0EcCoreIodCtrl_t;
#else
///  @brief 0xF200C942 : EC Core 0 EC Core RDEC Power Down Control 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ITR_RDEC_PD_WAKEUP_CYC : 6;         ///<BIT [5:0] Iterative Retry Decoder Power-Down Wake Up Cycle.
        uint16_t RF200C942_14_6_RSVD    : 9;         ///<BIT [14:6] Reserved.
        uint16_t ITR_RDEC_PD_EN         : 1;         ///<BIT [15] Iterative Retry Decoder Power-Down Enable.
    } b;
} EcCore0EcCoreRdecPowerDn11_t;

///  @brief 0xF200C944 : EC Core 0 EC Core Encoder Power Down Control0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PD_ITR_ENC          : 4;         ///<BIT [3:0] Power-Down Iterative Encoder.
        uint16_t RF200C944_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;
} EcCore0EcCoreEncPowerDn00_t;

///  @brief 0xF200C946 : EC Core 0 EC Core Encoder Power Down Control1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PD_ECU_DP_ENC       : 4;         ///<BIT [3:0] Power-Down ECU Data Path Encoder.
        uint16_t RF200C946_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;
} EcCore0EcCoreEncPowerDn11_t;

///  @brief 0xF200C948 : EC Core 0 EC Core Decoder Power Down Control0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PD_ITR_IDEC         : 4;         ///<BIT [3:0] Power-Down Iterative Inline Decoder.
        uint16_t RF200C948_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;
} EcCore0EcCoreDecPowerDn00_t;

///  @brief 0xF200C94A : EC Core 0 EC Core Decoder Power Down Control1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PD_ITR_RDEC         : 1;         ///<BIT [0] Power-Down Iterative Retry Decoder.
        uint16_t RF200C94A_3_1_RSVD  : 3;         ///<BIT [3:1] Reserved.
        uint16_t PD_ECU_DP_DEC       : 1;         ///<BIT [4] Power-Down ECU Data Path Decoder.
        uint16_t RF200C94A_15_5_RSVD :11;         ///<BIT [15:5] Reserved.
    } b;
} EcCore0EcCoreDecPowerDn11_t;

///  @brief 0xF200C94C : EC Core 0 EC Core IOD Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t IOD_BUF_THRSHD      : 3;         ///<BIT [2:0] Iteration On Demand Buffer Threshold.
        uint16_t RF200C94C_15_3_RSVD :13;         ///<BIT [15:3] Reserved.
    } b;
} EcCore0EcCoreIodCtrl_t;
#endif

////(START _SOC_REV_ < 0x0210)
///  @brief 0xF200C9A2 : EC Core 1 EC Core RDEC Power Down Control 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_ITR_RDEC_PD_WAKEUP_CYC : 6;         ///<BIT [5:0] Iterative Retry Decoder Power-Down Wake Up Cycle.
        uint16_t RF200C9A2_14_6_RSVD         : 9;         ///<BIT [14:6] Reserved.
        uint16_t ECC1_ITR_RDEC_PD_EN         : 1;         ///<BIT [15] Iterative Retry Decoder Power-Down Enable.
    } b;
} EcCore1EcCoreRdecPowerDn11_t;

///  @brief 0xF200C9A4 : EC Core 1 EC Core Encoder Power Down Control0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_PD_ITR_ENC     : 4;         ///<BIT [3:0] Power-Down Iterative Encoder.
        uint16_t RF200C9A4_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;
} EcCore1EcCoreEncPowerDn00_t;

///  @brief 0xF200C9A6 : EC Core 1 EC Core Encoder Power Down Control1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_PD_ECU_DP_ENC  : 4;         ///<BIT [3:0] Power-Down ECU Data Path Encoder.
        uint16_t RF200C9A6_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;
} EcCore1EcCoreEncPowerDn11_t;

///  @brief 0xF200C9A8 : EC Core 1 EC Core Decoder Power Down Control0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_PD_ITR_IDEC    : 4;         ///<BIT [3:0] Power-Down Iterative Inline Decoder.
        uint16_t RF200C9A8_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;
} EcCore1EcCoreDecPowerDn00_t;

///  @brief 0xF200C9AA : EC Core 1 EC Core Decoder Power Down Control1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_PD_ITR_RDEC    : 1;         ///<BIT [0] Power-Down Iterative Retry Decoder.
        uint16_t RF200C9AA_3_1_RSVD  : 3;         ///<BIT [3:1] Reserved.
        uint16_t ECC1_PD_ECU_DP_DEC  : 1;         ///<BIT [4] Power-Down ECU Data Path Decoder.
        uint16_t RF200C9AA_15_5_RSVD :11;         ///<BIT [15:5] Reserved.
    } b;
} EcCore1EcCoreDecPowerDn11_t;

///  @brief 0xF200C9AC : EC Core 1 EC Core IOD Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_IOD_BUF_THRSHD : 3;         ///<BIT [2:0] Iteration On Demand Buffer Threshold.
        uint16_t RF200C9AC_15_3_RSVD :13;         ///<BIT [15:3] Reserved.
    } b;
} EcCore1EcCoreIodCtrl_t;

///  @brief 0xF200CA0C : EC Core 1 Iterative Decoder Control and Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_ITR_DEC_CONFIG_RD_SEL   : 3;         ///<BIT [2:0] Iterative Decoder Configuration Read Select.
        uint16_t RF200CA0C_7_3_RSVD           : 5;         ///<BIT [7:3] Reserved.
        uint16_t ECC1_ITR_DEC_CONFIG_CODESEL0 : 1;         ///<BIT [8] Iterative Decoder Configuration Code Select 0.
        uint16_t ECC1_ITR_DEC_CONFIG_RD_WRB   : 1;         ///<BIT [9] Iterative Decoder Configuration Flash Read/Write Bar.
        uint16_t ECC1_ITR_DEC_CONFIG_MODE_ON  : 1;         ///<BIT [10] Iterative Decoder Configuration Mode On.
        uint16_t ECC1_ITR_DEC_CONFIG_RDY      : 1;         ///<BIT [11] Iterative Decoder Configured and Ready.
        uint16_t ECC1_ITR_DEC_CONFIG_CODESEL1 : 1;         ///<BIT [12] Iterative Decoder Configuration Code Select 1.
        uint16_t RF200CA0C_13_RSVD            : 1;         ///<BIT [13] Reserved.
        uint16_t RF200CA0C_14_RSVD            : 1;         ///<BIT [14] Reserved.
        uint16_t ECC1_ITR_DEC_CONFIG_INIT     : 1;         ///<BIT [15] Iterative Decoder Configuration Initialization.
    } b;
} EcCore1ItrDecCtrlAndStat_t;

///  @brief 0xF200CA10 : EC Core 1 Iterative Core Iterative Encoder Control and Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_ITR_ENC_CONFIG_RD_SEL   : 3;         ///<BIT [2:0] Iterative Encoder Configuration Read Select.
        uint16_t RF200CA10_7_3_RSVD           : 5;         ///<BIT [7:3] Reserved.
        uint16_t ECC1_ITR_ENC_CONFIG_CODESEL0 : 1;         ///<BIT [8] Iterative Encoder Configuration Code Select 0.
        uint16_t ECC1_ITR_ENC_CONFIG_RD_WRB   : 1;         ///<BIT [9] Iterative Encoder Configuration Read/Write Bar.
        uint16_t ECC1_ITR_ENC_CONFIG_MODE_ON  : 1;         ///<BIT [10] Iterative Encoder Configuration Mode On.
        uint16_t ECC1_ITR_ENC_CONFIG_RDY      : 1;         ///<BIT [11] Iterative Encoder Configured and Ready.
        uint16_t ECC1_ITR_ENC_CONFIG_CODESEL1 : 1;         ///<BIT [12] Iterative Encoder Configuration Code Select 1.
        uint16_t RF200CA10_13_RSVD            : 1;         ///<BIT [13] Reserved.
        uint16_t RF200CA10_14_RSVD            : 1;         ///<BIT [14] Reserved.
        uint16_t ECC1_ITR_ENC_CONFIG_INIT     : 1;         ///<BIT [15] Iterative Encoder Configuration Initialization.
    } b;
} EcCore1ItrCoreItrEncCtrl_t;

///  @brief 0xF200CA18 : EC Core 1 Iterative Code 0 Parity Length
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_ITR_PARITY_LEN0 : 9;         ///<BIT [8:0] Iterative Code Parity 0 Length.
        uint16_t RF200CA18_15_9_RSVD  : 7;         ///<BIT [15:9] Reserved.
    } b;
} EcCore1ItrCmem0ParLen_t;

///  @brief 0xF200CA1A : EC Core 1 Iterative Code 1 Parity Length
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_ITR_PARITY_LEN1 : 9;         ///<BIT [8:0] Iterative Code Parity 1 Length.
        uint16_t RF200CA1A_15_9_RSVD  : 7;         ///<BIT [15:9] Reserved.
    } b;
} EcCore1ItrCmem1ParLen_t;

///  @brief 0xF200CA1C : EC Core 1 Iterative Code 0 Codeword Length
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ITR_CW_LEN0          :12;         ///<BIT [11:0] Iterative Code Codeword 0 Length.
        uint16_t RF200CA1C_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcCore1ItrCmem0CwLen_t;

///  @brief 0xF200CA1E : EC Core 1 Iterative Code 1 Codeword Length
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_ITR_CW_LEN1     :12;         ///<BIT [11:0] Iterative Code Codeword 1 Length.
        uint16_t RF200CA1E_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcCore1ItrCmem1CwLen_t;

///  @brief 0xF200CA20 : EC Core 1 Iterative Schedule 0 and 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_SCHED_VER0   : 4;         ///<BIT [3:0] Schedule Version 0.
        uint16_t ECC1_TAPEOUT_VER0 : 4;         ///<BIT [7:4] Tapeout Version 0.
        uint16_t ECC1_SCHED_VER1   : 4;         ///<BIT [11:8] Schedule Version 1.
        uint16_t ECC1_TAPEOUT_VER1 : 4;         ///<BIT [15:12] Tapeout Version 1.
    } b;
} EcCore1ItrSchedule0And1_t;

///  @brief 0xF200CA22 : EC Core 1 Iterative Schedule 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_SCHED_VER2      : 4;         ///<BIT [3:0] Schedule Version 2.
        uint16_t ECC1_TAPEOUT_VER2    : 4;         ///<BIT [7:4] Tapeout Version 2.
        uint16_t RF200CA22_11_8_RSVD  : 4;         ///<BIT [11:8] Reserved.
        uint16_t RF200CA22_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;
} EcCore1ItrSchedule2_t;

///  @brief 0xF200CA26 : EC Core 1 Iterative Schedule 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200CA26_7_0_RSVD : 8;         ///<BIT [7:0] Reserved.
        uint16_t ECC1_SCHED_VER3    : 4;         ///<BIT [11:8] Schedule Version 3.
        uint16_t ECC1_TAPEOUT_VER3  : 4;         ///<BIT [15:12] Tapeout Version 3.
    } b;
} EcCore1ItrSchedule3_t;

///  @brief 0xF200CA40 : EC Core 1 Iterative Inline Decoder 0 Control and Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200CA40_7_0_RSVD            : 8;         ///<BIT [7:0] Reserved.
        uint16_t ECC1_ITR_IDEC0_FINAL_ITER_CNT : 6;         ///<BIT [13:8] Iterative Inline Decoder Final Iteration Count.
        uint16_t ECC1_ITR_IDEC0_RESET          : 1;         ///<BIT [14] Iterative Inline Decoder Reset.
        uint16_t ECC1_ITR_IDEC0_DISABLE        : 1;         ///<BIT [15] Iterative Inline Decoder Disable.
    } b;
} EcCore1ItrInlineDec0Ctrl_t;

///  @brief 0xF200CA44 : EC Core 1 Iterative Inline Decoder0 Control and Status (ECC1_ITR_IDEC0_2B_IDX = 0)
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_ITR_IDEC0_MAX_NUM_ITER_0  : 6;         ///<BIT [5:0] Iterative Inline Decoder Maximum Number of Iterations 0.
        uint16_t ECC1_ITR_IDEC0_EARLY_TERM_EN_0 : 1;         ///<BIT [6] Iterative Inline Decoder Early Termination Enable 0.
        uint16_t RF200CA44_7_RSVD               : 1;         ///<BIT [7] Reserved.
        uint16_t RF200CA44_15_8_RSVD            : 8;         ///<BIT [15:8] Reserved.
    } b;
} EcCore1ItrInlineDecoder0CtrlAnd_t;

///  @brief 0xF200CA48 : EC Core 1Iterative Inline Decoder 0 Error Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200CA48_7_0_RSVD        : 8;         ///<BIT [7:0] Reserved.
        uint16_t ECC1_ITR_IDEC0_ERR_CNT_EN : 1;         ///<BIT [8] Iterative Inline Decoder Error Count Enable.
        uint16_t RF200CA48_9_RSVD          : 1;         ///<BIT [9] Reserved.
        uint16_t RF200CA48_10_RSVD         : 1;         ///<BIT [10] Reserved.
        uint16_t RF200CA48_11_RSVD         : 1;         ///<BIT [11] Reserved.
        uint16_t ECC1_ITR_IDEC0_METHOD     : 1;         ///<BIT [12] Iterative Inline Decoder Method.
        uint16_t RF200CA48_14_13_RSVD      : 2;         ///<BIT [14:13] Reserved.
        uint16_t ECC1_ITR_IDEC0_2B_IDX     : 1;         ///<BIT [15] Iterative Inline Decoder 2-Bit Index.
    } b;
} EcCore1iteratiInlineDec0ErrCtrl_t;

///  @brief 0xF200CAC0 : EC Core 1 Iterative Retry Decoder Control and Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200CAC0_6_0_RSVD                : 7;         ///<BIT [6:0] Reserved.
        uint16_t RF200CAC0_7_RSVD                  : 1;         ///<BIT [7] Reserved.
        uint16_t ECC1_ITR_RDEC_ERASURE_INJ_WIN_LEN : 8;         ///<BIT [15:8] Iterative Retry Decoder Erasure Injection Window Length.
    } b;
} EcCore1ItrRetryDecCtrlAnd01_t;

///  @brief 0xF200CAC2 : EC Core 1 Iterative Retry Decoder Start Location and Error Length
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_ITR_RDEC_ERASURE_INJ_WIN_START_LOC :12;         ///<BIT [11:0] Iterative Retry Decoder Erasure Injection Window Start Location.
        uint16_t ECC1_ITR_RDEC_ERASURE_INJ_IO_SEL        : 1;         ///<BIT [12] Iterative Retry Decoder Erasure Injection I/O Selection.
        uint16_t ECC1_ITR_RDEC_ERASURE_INJ_EN            : 1;         ///<BIT [13] Iterative Retry Decoder Erasure Injection Enable.
        uint16_t ECC1_ITR_RDEC_RESET                     : 1;         ///<BIT [14] Iterative Retry Decoder Reset.
        uint16_t RF200CAC2_15_RSVD                       : 1;         ///<BIT [15] Reserved.
    } b;
} EcCore1ItrRetryDecStartLoc_t;

///  @brief 0xF200CAC6 : EC Core 1 Iterative Retry Decoder Control and Status 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200CAC6_7_0_RSVD           : 8;         ///<BIT [7:0] Reserved.
        uint16_t ECC1_ITR_RDEC_FINAL_ITER_CNT : 7;         ///<BIT [14:8] Iterative Retry Decoder Final Iteration Count.
        uint16_t FAST_RTMB_MODE               : 1;         ///<BIT [15] Fast Retry Mode.
    } b;
} EcCore1ItrRetryDecCtrlAnd12_t;

///  @brief 0xF200CAD4 : EC Core 1 Iterative Retry Decoder 20b (FAST_RTMB_MODE = 0)
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RTM_ITR_MAX_NUM_ITER  : 7;         ///<BIT [6:0] Retry Mode Iterative Retry Decoder Maximum Number of Iterations.
        uint16_t RTM_ITR_EARLY_TERM_EN : 1;         ///<BIT [7] Retry Mode Iterative Retry Decoder Early Termination Enable.
        uint16_t RF200CAD4_15_8_RSVD   : 8;         ///<BIT [15:8] Reserved.
    } b;
} EcCore1ItrRetryDec20bFast_t;

///  @brief 0xF200CB00 : EC Core 1 Iterative Encoder Control and Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200CB00_3_0_RSVD               : 4;         ///<BIT [3:0] Reserved.
        uint16_t ECC1_ITR_ENC_RESET               : 1;         ///<BIT [4] Iterative Encoder Reset.
        uint16_t ECC1_ITR_ENC_BYPASS              : 1;         ///<BIT [5] Iterative Encoder Bypass.
        uint16_t ECC1_ITR_ENC_D_CRC_UPDATE_FREEZE : 1;         ///<BIT [6] CRC Update Freeze.
        uint16_t RF200CB00_7_RSVD                 : 1;         ///<BIT [7] Reserved.
        uint16_t RF200CB00_13_8_RSVD              : 6;         ///<BIT [13:8] Reserved.
        uint16_t ECC1_ITR_ENC_CFG_CRC_ERR         : 2;         ///<BIT [15:14] Iterative Encoder Configuration CRC Error.
    } b;
} EcCore1ItrEncCtrlAndStat_t;
///(END _SOC_REV_ < 0x0210)


// **====================Frank 0428 Add EC1 IDEC 1 Start===========================**

///  @brief 0xF200CA60 : EC Core 0 Iterative Inline Decoder1 Control and Status 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200CA60_3_0_RSVD            : 4;         ///<BIT [3:0] Reserved.
        uint16_t RF200CA60_7_4_RSVD            : 4;         ///<BIT [7:4] Reserved.
        uint16_t ECC1_ITR_IDEC1_FINAL_ITER_CNT : 6;         ///<BIT [13:8] Iterative Inline Decoder Final Iteration Count.
        uint16_t ECC1_ITR_IDEC1_RESET          : 1;         ///<BIT [14] Iterative Inline Decoder Reset.
        uint16_t ECC1_ITR_IDEC1_DISABLE        : 1;         ///<BIT [15] Iterative Inline Decoder Disable.
    } b;
} EcCore1ItrInlineDec1Ctrl_t;

///  @brief 0xF200CA64 : EC Core 0 Iterative Inline Decoder1 Control and Status (ECC0_ITR_IDEC0_2B_IDX = 0)
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECC1_ITR_IDEC1_MAX_NUM_ITER_0  : 6;         ///<BIT [5:0] Iterative Inline Decoder Maximum Number of Iterations 0.
        uint16_t ECC1_ITR_IDEC1_EARLY_TERM_EN_0 : 1;         ///<BIT [6] Iterative Inline Decoder Early Termination Enable 0.
        uint16_t RF200CA64_7_RSVD               : 1;         ///<BIT [7] Reserved.
        uint16_t RF200Ca64_15_8_RSVD            : 8;         ///<BIT [15:8] Reserved.
    } b;
} EcCore1ItrInlineDecoder1CtrlAnd_t;

///  @brief 0xF200CA68 : EC Core 0Iterative Inline Decoder 1 Error Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200CA68_7_0_RSVD        : 8;         ///<BIT [7:0] Reserved.
        uint16_t ECC1_ITR_IDEC1_ERR_CNT_EN : 1;         ///<BIT [8] Iterative Inline Decoder Error Count Enable.
        uint16_t RF200CA68_9_RSVD          : 1;         ///<BIT [9] Reserved.
        uint16_t RF200CA68_10_RSVD         : 1;         ///<BIT [10] Reserved.
        uint16_t RF200CA68_11_RSVD         : 1;         ///<BIT [11] Reserved.
        uint16_t ECC1_ITR_IDEC1_METHOD     : 1;         ///<BIT [12] Iterative Inline Decoder Method.
        uint16_t RF200CA68_14_13_RSVD      : 2;         ///<BIT [14:13] Reserved.
        uint16_t ECC1_ITR_IDEC1_2B_IDX     : 1;         ///<BIT [15] Iterative Inline Decoder 2-Bit Index.
    } b;
} EcCore1iteratiInlineDec1ErrCtrl_t;

// **==================Frank 0428 Add EC1 IDEC 1 End================**

/// @brief 0xF200C000-0xF200CBE0 : Ecu_t MODULE
typedef struct
{
    EcuCtrlAndCfg1_t ecuCtrlAndCfg1;                                       ///< 0xF200C000 : ECU Control and Configuration 1
    EcuCtrlAndCfg2_t ecuCtrlAndCfg2;                                       ///< 0xF200C002 : ECU Control and Configuration 2
    EcuCtrlAndCfg3_t ecuCtrlAndCfg3;                                       ///< 0xF200C004 : ECU Control and Configuration 3
    EcuCtrlAndCfg4_t ecuCtrlAndCfg4;                                       ///< 0xF200C006 : ECU Control and Configuration 4
    EcuCtrlAndCfg5And6_t ecuCtrlAndCfg5And6;                               ///< 0xF200C008 : ECU Control and Configuration 5 and 6
    EcuCtrlAndCfg6_t ecuCtrlAndCfg6;                                       ///< 0xF200C00A : ECU Control and Configuration 6
    uint16_t MAX_AU_NUM_THR_L;                                             ///< 0xF200C00C : ECU Control and Configuration 14
    uint16_t MAX_AU_NUM_THR_H;                                             ///< 0xF200C00E : ECU Control and Configuration 15
    EcuCtrlAndCfg7_t ecuCtrlAndCfg7;                                       ///< 0xF200C010 : ECU Control and Configuration 7
    EcuCtrlAndCfg8_t ecuCtrlAndCfg8;                                       ///< 0xF200C012 : ECU Control and Configuration 8
    EcuCtrlAndCfg9_t ecuCtrlAndCfg9;                                       ///< 0xF200C014 : ECU Control and Configuration 9
    EcuCtrlAndCfg10_t ecuCtrlAndCfg10;                                     ///< 0xF200C016 : ECU Control and Configuration 10
    EcuCtrlAndCfg11_t ecuCtrlAndCfg11;                                     ///< 0xF200C018 : ECU Control and Configuration 11
    EcuCtrlAndCfg12_t ecuCtrlAndCfg12;                                     ///< 0xF200C01A : ECU Control and Configuration 12
    EcuCtrlAndCfg13_t ecuCtrlAndCfg13;                                     ///< 0xF200C01C : ECU Control and Configuration 13
    EcuAuFmtCfgPtr_t ecuAuFmtCfgPtr;                                       ///< 0xF200C01E : ECU AU Format Configuration Pointer
    EcuCfg1_t ecuCfg1;                                                     ///< 0xF200C020 : ECU Configuration 1
    EcuCfg2_t ecuCfg2;                                                     ///< 0xF200C022 : ECU Configuration 2
    EcuCfg3_t ecuCfg3;                                                     ///< 0xF200C024 : ECU Configuration 3
    uint8_t rsvd_RF200C026[2];                                             ///< 0xF200C026 - 0xF200C027 : Reserved.
    EcuCfg5_t ecuCfg5;                                                     ///< 0xF200C028 : ECU Configuration 5
    EcuAuSize_t ecuAuSize;                                                 ///< 0xF200C02A : ECU AU Size
    uint8_t rsvd_RF200C02C[2];                                             ///< 0xF200C02C - 0xF200C02D : Reserved.
    EcuCfg6_t ecuCfg6;                                                     ///< 0xF200C02E : ECU Configuration 6
    uint8_t rsvd_RF200C030[8];                                             ///< 0xF200C030 - 0xF200C037 : Reserved.
    uint16_t SCR_SEED_VAL_15_0;                                            ///< 0xF200C038 : Scrambler Seed Value a
    uint16_t SCR_SEED_VAL_31_16;                                           ///< 0xF200C03A : Scrambler Seed Value b
    uint16_t SCR_SEED_MASK_15_0;                                           ///< 0xF200C03C : Scrambler Seed Mask a
    uint16_t SCR_SEED_MASK_31_16;                                          ///< 0xF200C03E : Scrambler Seed Mask b
    EcuEncodeStat0_t ecuEncodeStat0;                                       ///< 0xF200C040 : ECU Encode Status 0
    uint16_t ENC_AU_DONE_ID_15_0;                                          ///< 0xF200C042 : ECU Encode Status 1a
    EcuEncodeStat1b_t ecuEncodeStat1b;                                     ///< 0xF200C044 : ECU Encode Status 1b
    uint8_t rsvd_RF200C046[2];                                             ///< 0xF200C046 - 0xF200C047 : Reserved.
    EcuEncDppStat_t ecuEncDppStat;                                         ///< 0xF200C048 : ECU Encoder DPP Status
    uint8_t rsvd_RF200C04A[6];                                             ///< 0xF200C04A - 0xF200C04F : Reserved.
    EcuDecStat0_t ecuDecStat0;                                             ///< 0xF200C050 : ECU Decoder Status 0
    EcuDecStat1_t ecuDecStat1;                                             ///< 0xF200C052 : ECU Decoder Status 1
    EcuDecStat2_t ecuDecStat2;                                             ///< 0xF200C054 : ECU Decoder Status 2
    EcuDecStat3_t ecuDecStat3;                                             ///< 0xF200C056 : ECU Decoder Status 3
    EcuDecStat4_t ecuDecStat4;                                             ///< 0xF200C058 : ECU Decoder Status 4
    EcuDecStat5_t ecuDecStat5;                                             ///< 0xF200C05A : ECU Decoder Status 5
    uint8_t rsvd_RF200C05C[16];                                            ///< 0xF200C05C - 0xF200C06B : Reserved.
    EcuDecStat14_t ecuDecStat14;                                           ///< 0xF200C06C : ECU Decoder Status 14
    EcuDecStat15_t ecuDecStat15;                                           ///< 0xF200C06E : ECU Decoder Status 15
    EcuAxiWriteCfg1_t ecuAxiWriteCfg1;                                     ///< 0xF200C070 : ECU AXI WRITE Configuration 1
    EcuAxiWriteCfg2_t ecuAxiWriteCfg2;                                     ///< 0xF200C072 : ECU AXI WRITE Configuration 2
    EcuAxiWrStat_t ecuAxiWrStat;                                           ///< 0xF200C074 : ECU AXI Write Status
    uint8_t rsvd_RF200C076[2];                                             ///< 0xF200C076 - 0xF200C077 : Reserved.
    EcuAxiRdCfg1_t ecuAxiRdCfg1;                                           ///< 0xF200C078 : ECU AXI Read Configuration 1
    EcuAxiRdCfg2_t ecuAxiRdCfg2;                                           ///< 0xF200C07A : ECU AXI Read Configuration 2
    EcuAxiRdStat_t ecuAxiRdStat;                                           ///< 0xF200C07C : ECU AXI Read Status
    uint8_t rsvd_RF200C07E[2];                                             ///< 0xF200C07E - 0xF200C07F : Reserved.
    AxiIdCfg1_t axiIdCfg1;                                                 ///< 0xF200C080 : AXI ID Configuration 1
    uint8_t rsvd_RF200C082[4];                                             ///< 0xF200C082 - 0xF200C085 : Reserved.
    EcuAbortCtrlSgn_t ecuAbortCtrlSgn;                                     ///< 0xF200C086 : ECU Abort Control Signal
    uint8_t rsvd_RF200C088[72];                                            ///< 0xF200C088 - 0xF200C0CF : Reserved.
    EcuRaidCtrlAndStat0_t ecuRaidCtrlAndStat0;                             ///< 0xF200C0D0 : ECU RAID Control and Status 0
    uint8_t rsvd_RF200C0D2[4];                                             ///< 0xF200C0D2 - 0xF200C0D5 : Reserved.
    EcuRaidCtrlAndStat1_t ecuRaidCtrlAndStat1;                             ///< 0xF200C0D6 : ECU RAID Control and Status 1
    EcuRaidCtrlAndStat4_t ecuRaidCtrlAndStat4;                             ///< 0xF200C0D8 : ECU RAID Control and Status 4
    EcuRaidCtrlAndStat5_t ecuRaidCtrlAndStat5;                             ///< 0xF200C0DA : ECU RAID Control and Status 5
    RaidBufPowerDnCtrl_t raidBufPowerDnCtrl;                               ///< 0xF200C0DC : RAID Buffer Power Down Control
    uint8_t rsvd_RF200C0DE[66];                                            ///< 0xF200C0DE - 0xF200C11F : Reserved.
    EcuDeepRetryModeCtrlAndCfg1_t ecuDeepRetryModeCtrlAndCfg1;             ///< 0xF200C120 : ECU Deep Retry Mode Control and Configuration 1
    EcuDeepRetryModeCtrlAndCfg2_t ecuDeepRetryModeCtrlAndCfg2;             ///< 0xF200C122 : ECU Deep Retry Mode Control and Configuration 2
#if (_SOC_REV_ >= 0x0210)
    uint8_t rsvd_RF200C124[12];                                            ///< 0xF200C124 - 0xF200C12F : Reserved.
#else
    uint8_t rsvd_RF200C124[8];                                             ///< 0xF200C124 - 0xF200C12B : Reserved.
    EcuDeepRetryStat1a_t ecuDeepRetryStat1a;                               ///< 0xF200C12C : ECU Deep Retry Status 1a
    EcuDeepRetryStat1b_t ecuDeepRetryStat1b;                               ///< 0xF200C12E : ECU Deep Retry Status 1b
#endif
    EcuDeepRetryStat2a_t ecuDeepRetryStat2a;                               ///< 0xF200C130 : ECU Deep Retry Status 2a
    EcuDeepRetryStat2b_t ecuDeepRetryStat2b;                               ///< 0xF200C132 : ECU Deep Retry Status 2b
    uint8_t rsvd_RF200C134[12];                                            ///< 0xF200C134 - 0xF200C13F : Reserved.
    HwRrdCtrlRegister_t hwRrdCtrlRegister;                                 ///< 0xF200C140 : HW RRD Control Reg
    HwllErrStat0_t hwllErrStat0;                                           ///< 0xF200C142 : HWLL Error Status 0
    HwllErrStat1_t hwllErrStat1;                                           ///< 0xF200C144 : HWLL Error Status 1
    HwllErrStat2_t hwllErrStat2;                                           ///< 0xF200C146 : HWLL Error Status 2
    HwllErrStat3_t hwllErrStat3;                                           ///< 0xF200C148 : HWLL Error Status 3
    uint8_t rsvd_RF200C14A[54];                                            ///< 0xF200C14A - 0xF200C17F : Reserved.
    EcuDecStat16_t ecuDecStat16;                                           ///< 0xF200C180 : ECU Decoder Status 16
    uint8_t rsvd_RF200C182[2];                                             ///< 0xF200C182 - 0xF200C183 : Reserved.
    EcuDecStat18_t ecuDecStat18;                                           ///< 0xF200C184 : ECU Decoder Status 18
    uint16_t CUR_DEC_AU_DONE_15_0;                                         ///< 0xF200C186 : ECU Decoder Status 19a
    EcuDecStat19b_t ecuDecStat19b;                                         ///< 0xF200C188 : ECU Decoder Status 19b
    uint8_t rsvd_RF200C18A[2];                                             ///< 0xF200C18A - 0xF200C18B : Reserved.
    uint16_t DEC_AU_DONE_ID_15_0;                                          ///< 0xF200C18C : ECU Decoder Status 20a
    EcuDecStat20b_t ecuDecStat20b;                                         ///< 0xF200C18E : ECU Decoder Status 20b
    uint8_t rsvd_RF200C190[2];                                             ///< 0xF200C190 - 0xF200C191 : Reserved.
    EcuDecStat21_t ecuDecStat21;                                           ///< 0xF200C192 : ECU Decoder Status 21
    uint8_t rsvd_RF200C194[4];                                             ///< 0xF200C194 - 0xF200C197 : Reserved.
    EcuDecStat24_t ecuDecStat24;                                           ///< 0xF200C198 : ECU Decoder Status 24
    EcuDecStat25_t ecuDecStat25;                                           ///< 0xF200C19A : ECU Decoder Status 25
    uint8_t rsvd_RF200C19C[4];                                             ///< 0xF200C19C - 0xF200C19F : Reserved.
    uint16_t ACCM_CORR_CNT_LSB;                                            ///< 0xF200C1A0 : ACCM ECU Decoder Status 0
    uint16_t ACCM_CORR_CNT_MSB;                                            ///< 0xF200C1A2 : ACCM ECU Decoder Status 1
    uint16_t ACCM_UNCLEAN_AU;                                              ///< 0xF200C1A4 : ACCM ECU Decoder Status 2
    uint16_t ACCM_OVRLMT_AU;                                               ///< 0xF200C1A6 : ACCM ECU Decoder Status 3
    uint16_t ACCM_UNCORR_AU;                                               ///< 0xF200C1A8 : ACCM ECU Decoder Status 4
    AccmEcuDecStat5_t accmEcuDecStat5;                                     ///< 0xF200C1AA : ACCM ECU Decoder Status 5
    uint16_t ACCM_VREF_CNT_LSB;                                            ///< 0xF200C1AC : ACCM ECU Decoder Status 6
    uint16_t ACCM_VREF_CNT_MSB;                                            ///< 0xF200C1AE : ACCM ECU Decoder Status 7
    uint8_t rsvd_RF200C1B0[80];                                            ///< 0xF200C1B0 - 0xF200C1FF : Reserved.
    EcuAxiIntr_t ecuAxiIntr;                                               ///< 0xF200C200 : ECU AXI Interrupt
    uint8_t rsvd_RF200C202[8];                                               ///< 0xF200C202 - 0xF200C209 : Reserved.
    EcuAxiIntrCpuMsk_t ecuAxiIntrCpuMsk;                                   ///< 0xF200C20A : ECU AXI Interrupt CPU Mask
    uint8_t rsvd_RF200C20C[2];                                             ///< 0xF200C20C - 0xF200C20D : Reserved.
    EcuLnkListAndAuIdIntr_t ecuLnkListAndAuIdIntr;                         ///< 0xF200C20E : ECU Link List and AU ID Interrupt
    EcuLnkListAndAuIdIntrCpu_t ecuLnkListAndAuIdIntrCpu;                   ///< 0xF200C210 : ECU Link List and AU ID Interrupt CPU Mask
    uint8_t rsvd_RF200C212[8];                                             ///< 0xF200C212 - 0xF200C219 : Reserved.
    EcuDrtmIntr_t ecuDrtmIntr;                                             ///< 0xF200C21A : ECU DRTM Interrupt
    EcuDrtmIntrCpuMsk_t ecuDrtmIntrCpuMsk;                                 ///< 0xF200C21C : ECU DRTM Interrupt CPU Mask
    uint8_t rsvd_RF200C21E[2];                                             ///< 0xF200C21E - 0xF200C21F : Reserved.
    MpeccStat0_t mpeccStat0;                                               ///< 0xF200C220 : MPECC Status 0
    MpeccStat2_t mpeccStat2;                                               ///< 0xF200C222 : MPECC Status 2
    MpeccStat3_t mpeccStat3;                                               ///< 0xF200C224 : MPECC Status 3
    //    ARRAY DECLARATION
    uint16_t ENC_MPECC_ERR_CW_SYND1[6];
    //    uint16_t ENC_MPECC_ERR_CW_SYND1_0;                                     ///< 0xF200C226 : MPECC Error Syndrome1 0
    //    uint16_t ENC_MPECC_ERR_CW_SYND1_1;                                     ///< 0xF200C228 : MPECC Error Syndrome1 1
    //    uint16_t ENC_MPECC_ERR_CW_SYND1_2;                                     ///< 0xF200C22A : MPECC Error Syndrome1 2
    //    uint16_t ENC_MPECC_ERR_CW_SYND1_3;                                     ///< 0xF200C22C : MPECC Error Syndrome1 3
    //    uint16_t ENC_MPECC_ERR_CW_SYND1_4;                                     ///< 0xF200C22E : MPECC Error Syndrome1 4
    //    uint16_t ENC_MPECC_ERR_CW_SYND1_5;                                     ///< 0xF200C230 : MPECC Error Syndrome1 5

    //    ARRAY DECLARATION
    uint16_t ENC_MPECC_ERR_CW_SYND2[6];
    //    uint16_t ENC_MPECC_ERR_CW_SYND2_0;                                     ///< 0xF200C232 : MPECC Error Syndrome2 0
    //    uint16_t ENC_MPECC_ERR_CW_SYND2_1;                                     ///< 0xF200C234 : MPECC Error Syndrome2 1
    //    uint16_t ENC_MPECC_ERR_CW_SYND2_2;                                     ///< 0xF200C236 : MPECC Error Syndrome2 2
    //    uint16_t ENC_MPECC_ERR_CW_SYND2_3;                                     ///< 0xF200C238 : MPECC Error Syndrome2 3
    //    uint16_t ENC_MPECC_ERR_CW_SYND2_4;                                     ///< 0xF200C23A : MPECC Error Syndrome2 4
    //    uint16_t ENC_MPECC_ERR_CW_SYND2_5;                                     ///< 0xF200C23C : MPECC Error Syndrome2 5
    uint8_t rsvd_RF200C23E[770];                                           ///< 0xF200C23E - 0xF200C53F : Reserved.
    uint16_t DMA_PD_MAX_IDLE_CYC;                                          ///< 0xF200C540 : DMA Power Down Control0
    DmaPowerDnControl1_t dmaPowerDnControl1;                               ///< 0xF200C542 : DMA Power Down Control1
    uint8_t rsvd_RF200C544[28];                                            ///< 0xF200C544 - 0xF200C55F : Reserved.
    uint16_t EXT_MEM0_BASE_ADDR_L;                                         ///< 0xF200C560 : ECU Retry External Memory0 Configuration 1 Low
    uint16_t EXT_MEM0_BASE_ADDR_H;                                         ///< 0xF200C562 : ECU Retry External Memory0 Configuration 1 High
    EcuRetryExtMemory0Cfg2_t ecuRetryExtMemory0Cfg2;                       ///< 0xF200C564 : ECU Retry External Memory0 Configuration 2
    uint8_t rsvd_RF200C566[2];                                             ///< 0xF200C566 - 0xF200C567 : Reserved.
    uint16_t EXT_MEM1_BASE_ADDR_L;                                         ///< 0xF200C568 : ECU Retry External Memory1 Configuration 1 Low
    uint16_t EXT_MEM1_BASE_ADDR_H;                                         ///< 0xF200C56A : ECU Retry External Memory1 Configuration 1 High
    EcuRetryExtMemory1Cfg2_t ecuRetryExtMemory1Cfg2;                       ///< 0xF200C56C : ECU Retry External Memory1 Configuration 2
    uint8_t rsvd_RF200C56E[2];                                             ///< 0xF200C56E - 0xF200C56F : Reserved.
    uint16_t EXT_MEM_ENTRY_SIZE;                                           ///< 0xF200C570 : External Memory Entry Size
    uint8_t rsvd_RF200C572[142];                                           ///< 0xF200C572 - 0xF200C5FF : Reserved.
    LlrGen0And1_t llrGen0And1;                                             ///< 0xF200C600 : LLR Generation 0 and 1
    uint16_t RTM_LLR_TABLE_RWDATA_IO;                                      ///< 0xF200C602 : LLR Generation Table 1 and 2
    LlrGen3And4_t llrGen3And4;                                             ///< 0xF200C604 : LLR Generation 3 and 4
    LlrGen5And6_t llrGen5And6;                                             ///< 0xF200C606 : LLR Generation 5 and 6
    LlrGen7And8_t llrGen7And8;                                             ///< 0xF200C608 : LLR Generation 7 and 8
    LlrGen9And10_t llrGen9And10;                                           ///< 0xF200C60A : LLR Generation 9 and 10
    LlrGen11And12_t llrGen11And12;                                         ///< 0xF200C60C : LLR Generation 11 and 12
    LlrGen13And14_t llrGen13And14;                                         ///< 0xF200C60E : LLR Generation 13 and 14
    LlrGen15And16_t llrGen15And16;                                         ///< 0xF200C610 : LLR Generation 15 and 16
    uint16_t RTM_BCD_LUT_DATA;                                             ///< 0xF200C612 : LLR Generation 17 and 18
    LlrGen19And20_t llrGen19And20;                                         ///< 0xF200C614 : LLR Generation 19 and 20
    uint8_t rsvd_RF200C616[98];                                            ///< 0xF200C616 - 0xF200C677 : Reserved.
    uint16_t LLR_PD_MAX_IDLE_CYC;                                          ///< 0xF200C678 : LLR Power Down Control0
    LlrPowerDnControl1_t llrPowerDnControl1;                               ///< 0xF200C67A : LLR Power Down Control1
    TwoBitInformatGeneratoCtrl_t twoBitInformatGeneratoCtrl;               ///< 0xF200C67C : Two-Bit Information Generator Control
    uint16_t TWOBIT_DEC_LUT;                                               ///< 0xF200C67E : Two-Bit Decoding LUT
    uint8_t rsvd_RF200C680[396];                                           ///< 0xF200C680 - 0xF200C80B : Reserved.
    EcCore0ItrDecCtrlAndStat_t ecCore0ItrDecCtrlAndStat;                   ///< 0xF200C80C : EC Core 0 Iterative Decoder Control and Status
    uint16_t ECC0_ITR_DEC_CONFIG_RWDATA_IO;                                ///< 0xF200C80E : EC Core 0 Iterative Decoder Configuration Data
    EcCore0ItrCoreItrEncCtrl_t ecCore0ItrCoreItrEncCtrl;                   ///< 0xF200C810 : EC Core 0 Iterative Core Iterative Encoder Control and Status
    uint16_t ECC0_ITR_ENC_CONFIG_RWDATA_IO;                                ///< 0xF200C812 : EC Core 0 Iterative Encoder Configuration Data
    uint8_t rsvd_RF200C814[4];                                             ///< 0xF200C814 - 0xF200C817 : Reserved.
    EcCore0ItrCmem0ParLen_t ecCore0ItrCmem0ParLen;                         ///< 0xF200C818 : EC Core 0 Iterative Code 0 Parity Length
    EcCore0ItrCmem1ParLen_t ecCore0ItrCmem1ParLen;                         ///< 0xF200C81A : EC Core 0 Iterative Code 1 Parity Length
    EcCore0ItrCmem0CwLen_t ecCore0ItrCmem0CwLen;                           ///< 0xF200C81C : EC Core 0 Iterative Code 0 Codeword Length
    EcCore0ItrCmem1CwLen_t ecCore0ItrCmem1CwLen;                           ///< 0xF200C81E : EC Core 0 Iterative Code 1 Codeword Length
    EcCore0ItrSchedule0And1_t ecCore0ItrSchedule0And1;                     ///< 0xF200C820 : EC Core 0 Iterative Schedule 0 and 1
    EcCore0ItrSchedule2_t ecCore0ItrSchedule2;                             ///< 0xF200C822 : EC Core 0 Iterative Schedule 2
    uint8_t rsvd_RF200C824[2];                                             ///< 0xF200C824 - 0xF200C825 : Reserved.
    EcCore0ItrSchedule3_t ecCore0ItrSchedule3;                             ///< 0xF200C826 : EC Core 0 Iterative Schedule 3
    uint8_t rsvd_RF200C828[24];                                            ///< 0xF200C828 - 0xF200C83F : Reserved.
    EcCore0ItrInlineDec0Ctrl_t ecCore0ItrInlineDec0Ctrl;                   ///< 0xF200C840 : EC Core 0 Iterative Inline Decoder 0 Control and Status 1
    uint8_t rsvd_RF200C842[2];                                             ///< 0xF200C842 - 0xF200C843 : Reserved.
    EcCore0ItrInlineDecoder0CtrlAnd_t ecCore0ItrInlineDecoder0CtrlAnd;     ///< 0xF200C844 : EC Core 0 Iterative Inline Decoder0 Control and Status (ECC0_ITR_IDEC0_2B_IDX = 0)
    uint8_t rsvd_RF200C846[2];                                             ///< 0xF200C846 - 0xF200C847 : Reserved.
    EcCore0iteratiInlineDec0ErrCtrl_t ecCore0iteratiInlineDec0ErrCtrl;     ///< 0xF200C848 : EC Core 0Iterative Inline Decoder 0 Error Control
    uint8_t rsvd_RF200C84A[22];                                           ///< 0xF200C84A - 0xF200C85F : Reserved.
    EcCore0ItrInlineDec1Ctrl_t   ecCore0ItrInlineDec1Ctrl;                ///< 0xF200C860: : EC Core 0 Iterative Inline Decoder 1 Control and Status 1
    uint8_t rsvd_RF200C862[2];                                             ///< 0xF200C862 - 0xF200C863 : Reserved.
    EcCore0ItrInlineDecoder1CtrlAnd_t ecCore0ItrInlineDecoder1CtrlAnd;     ///< 0xF200C864 : EC Core 0 Iterative Inline Decoder1 Control and Status (ECC0_ITR_IDEC1_2B_IDX = 0)
    uint8_t rsvd_RF200C866[2];                                             ///< 0xF200C866 - 0xF200C867 : Reserved.
    EcCore0iteratiInlineDec1ErrCtrl_t ecCore0iteratiInlineDec1ErrCtrl;     ///< 0xF200C868 : EC Core 0 Iterative Inline Decoder 1 Error Control
    uint8_t rsvd_RF200C86A[86];                                            ///< 0xF200C86A - 0XF200C8BF
    EcCore0ItrRetryDecCtrlAnd01_t ecCore0ItrRetryDecCtrlAnd0_1;            ///< 0xF200C8C0 : EC Core 0 Iterative Retry Decoder Control and Status 1
    EcCore0ItrRetryDecStartLoc_t ecCore0ItrRetryDecStartLoc;               ///< 0xF200C8C2 : EC Core 0 Iterative Retry Decoder Start Location and Error Length
    uint8_t rsvd_RF200C8C4[2];                                             ///< 0xF200C8C4 - 0xF200C8C5 : Reserved.
    EcCore0ItrRetryDecCtrlAnd12_t ecCore0ItrRetryDecCtrlAnd1_2;            ///< 0xF200C8C6 : EC Core 0 Iterative Retry Decoder Control and Status 2
    uint8_t rsvd_RF200C8C8[12];                                            ///< 0xF200C8C8 - 0xF200C8D3 : Reserved.
    EcCoreItrRetryDec20bFastRtmb_t ecCoreItrRetryDec20bFastRtmb;           ///< 0xF200C8D4 : EC Core Iterative Retry Decoder 20b (FAST_RTMB_MODE = 0)
    uint8_t rsvd_RF200C8D6[42];                                            ///< 0xF200C8D6 - 0xF200C8FF : Reserved.
    EcCore0ItrEncCtrlAndStat_t ecCore0ItrEncCtrlAndStat;                   ///< 0xF200C900 : EC Core 0 Iterative Encoder Control and Status
#if (_SOC_REV_ >= 0x0210)
    uint8_t rsvd_RF200C902[110];                                           ///< 0xF200C902 - 0xF200C96F : Reserved.
    uint16_t ITR_RDEC_PD_MAX_IDLE_CYC;                                     ///< 0xF200C970 : EC Core 0 EC Core RDEC Power Down Control 0
    EcCore0EcCoreRdecPowerDn11_t ecCore0EcCoreRdecPowerDn1_1;              ///< 0xF200C972 : EC Core 0 EC Core RDEC Power Down Control 1
    EcCore0EcCoreEncPowerDn00_t ecCore0EcCoreEncPowerDn0_0;                ///< 0xF200C974 : EC Core 0 EC Core Encoder Power Down Control0
    EcCore0EcCoreEncPowerDn11_t ecCore0EcCoreEncPowerDn1_1;                ///< 0xF200C976 : EC Core 0 EC Core Encoder Power Down Control1
    EcCore0EcCoreDecPowerDn00_t ecCore0EcCoreDecPowerDn0_0;                ///< 0xF200C978 : EC Core 0 EC Core Decoder Power Down Control0
    EcCore0EcCoreDecPowerDn11_t ecCore0EcCoreDecPowerDn1_1;                ///< 0xF200C97A : EC Core 0 EC Core Decoder Power Down Control1
    EcCore0EcCoreIodCtrl_t ecCore0EcCoreIodCtrl;                           ///< 0xF200C97C : EC Core 0 EC Core IOD Control
    uint8_t rsvd_RF200C97E[34];                                            ///< 0xF200C97E - 0xF200C99F : Reserved.
#else
    uint8_t rsvd_RF200C902[62];                                            ///< 0xF200C902 - 0xF200C93F : Reserved.
    uint16_t ITR_RDEC_PD_MAX_IDLE_CYC;                                     ///< 0xF200C940 : EC Core 0 EC Core RDEC Power Down Control 0
    EcCore0EcCoreRdecPowerDn11_t ecCore0EcCoreRdecPowerDn1_1;              ///< 0xF200C942 : EC Core 0 EC Core RDEC Power Down Control 1
    EcCore0EcCoreEncPowerDn00_t ecCore0EcCoreEncPowerDn0_0;                ///< 0xF200C944 : EC Core 0 EC Core Encoder Power Down Control0
    EcCore0EcCoreEncPowerDn11_t ecCore0EcCoreEncPowerDn1_1;                ///< 0xF200C946 : EC Core 0 EC Core Encoder Power Down Control1
    EcCore0EcCoreDecPowerDn00_t ecCore0EcCoreDecPowerDn0_0;                ///< 0xF200C948 : EC Core 0 EC Core Decoder Power Down Control0
    EcCore0EcCoreDecPowerDn11_t ecCore0EcCoreDecPowerDn1_1;                ///< 0xF200C94A : EC Core 0 EC Core Decoder Power Down Control1
    EcCore0EcCoreIodCtrl_t ecCore0EcCoreIodCtrl;                           ///< 0xF200C94C : EC Core 0 EC Core IOD Control
    uint8_t rsvd_RF200C94E[82];                                            ///< 0xF200C94E - 0xF200C99F : Reserved.
#endif
    uint16_t ECC1_ITR_RDEC_PD_MAX_IDLE_CYC;                                ///< 0xF200C9A0 : EC Core 1 EC Core RDEC Power Down Control 0
    EcCore1EcCoreRdecPowerDn11_t ecCore1EcCoreRdecPowerDn1_1;              ///< 0xF200C9A2 : EC Core 1 EC Core RDEC Power Down Control 1
    EcCore1EcCoreEncPowerDn00_t ecCore1EcCoreEncPowerDn0_0;                ///< 0xF200C9A4 : EC Core 1 EC Core Encoder Power Down Control0
    EcCore1EcCoreEncPowerDn11_t ecCore1EcCoreEncPowerDn1_1;                ///< 0xF200C9A6 : EC Core 1 EC Core Encoder Power Down Control1
    EcCore1EcCoreDecPowerDn00_t ecCore1EcCoreDecPowerDn0_0;                ///< 0xF200C9A8 : EC Core 1 EC Core Decoder Power Down Control0
    EcCore1EcCoreDecPowerDn11_t ecCore1EcCoreDecPowerDn1_1;                ///< 0xF200C9AA : EC Core 1 EC Core Decoder Power Down Control1
    EcCore1EcCoreIodCtrl_t ecCore1EcCoreIodCtrl;                           ///< 0xF200C9AC : EC Core 1 EC Core IOD Control
    uint8_t rsvd_RF200C9AE[94];                                            ///< 0xF200C9AE - 0xF200CA0B : Reserved.
    EcCore1ItrDecCtrlAndStat_t ecCore1ItrDecCtrlAndStat;                   ///< 0xF200CA0C : EC Core 1 Iterative Decoder Control and Status
    uint16_t ECC1_ITR_DEC_CONFIG_RWDATA_IO;                                ///< 0xF200CA0E : EC Core 1 Iterative Decoder Configuration Data
    EcCore1ItrCoreItrEncCtrl_t ecCore1ItrCoreItrEncCtrl;                   ///< 0xF200CA10 : EC Core 1 Iterative Core Iterative Encoder Control and Status
    uint16_t ECC1_ITR_ENC_CONFIG_RWDATA_IO;                                ///< 0xF200CA12 : EC Core 1 Iterative Encoder Configuration Data
    uint8_t rsvd_RF200CA14[4];                                             ///< 0xF200CA14 - 0xF200CA17 : Reserved.
    EcCore1ItrCmem0ParLen_t ecCore1ItrCmem0ParLen;                         ///< 0xF200CA18 : EC Core 1 Iterative Code 0 Parity Length
    EcCore1ItrCmem1ParLen_t ecCore1ItrCmem1ParLen;                         ///< 0xF200CA1A : EC Core 1 Iterative Code 1 Parity Length
    EcCore1ItrCmem0CwLen_t ecCore1ItrCmem0CwLen;                           ///< 0xF200CA1C : EC Core 1 Iterative Code 0 Codeword Length
    EcCore1ItrCmem1CwLen_t ecCore1ItrCmem1CwLen;                           ///< 0xF200CA1E : EC Core 1 Iterative Code 1 Codeword Length
    EcCore1ItrSchedule0And1_t ecCore1ItrSchedule0And1;                     ///< 0xF200CA20 : EC Core 1 Iterative Schedule 0 and 1
    EcCore1ItrSchedule2_t ecCore1ItrSchedule2;                             ///< 0xF200CA22 : EC Core 1 Iterative Schedule 2
    uint8_t rsvd_RF200CA24[2];                                             ///< 0xF200CA24 - 0xF200CA25 : Reserved.
    EcCore1ItrSchedule3_t ecCore1ItrSchedule3;                             ///< 0xF200CA26 : EC Core 1 Iterative Schedule 3
    uint8_t rsvd_RF200CA28[24];                                            ///< 0xF200CA28 - 0xF200CA3F : Reserved.
    EcCore1ItrInlineDec0Ctrl_t ecCore1ItrInlineDec0Ctrl;                   ///< 0xF200CA40 : EC Core 1 Iterative Inline Decoder 0 Control and Status 1
    uint8_t rsvd_RF200CA42[2];                                             ///< 0xF200CA42 - 0xF200CA43 : Reserved.
    EcCore1ItrInlineDecoder0CtrlAnd_t ecCore1ItrInlineDecoder0CtrlAnd;     ///< 0xF200CA44 : EC Core 1 Iterative Inline Decoder0 Control and Status (ECC1_ITR_IDEC0_2B_IDX = 0)
    uint8_t rsvd_RF200CA46[2];                                             ///< 0xF200CA46 - 0xF200CA47 : Reserved.
    EcCore1iteratiInlineDec0ErrCtrl_t ecCore1iteratiInlineDec0ErrCtrl;     ///< 0xF200CA48 : EC Core 1Iterative Inline Decoder 0 Error Control
    uint8_t rsvd_RF200CA4A[22];                                           ///< 0xF200CA4A - 0xF200CA5F : Reserved.
    EcCore1ItrInlineDec1Ctrl_t   ecCore1ItrInlineDec1Ctrl;                ///< 0xF200CA60: : EC Core 1 Iterative Inline Decoder 1 Control and Status 1
    uint8_t rsvd_RF200CA62[2];                                             ///< 0xF200CA62 - 0xF200CA63 : Reserved.
    EcCore1ItrInlineDecoder1CtrlAnd_t ecCore1ItrInlineDecoder1CtrlAnd;     ///< 0xF200CA64 : EC Core 1 Iterative Inline Decoder1 Control and Status (ECC1_ITR_IDEC1_2B_IDX = 0)
    uint8_t rsvd_RF200CA66[2];                                             ///< 0xF200CA66 - 0xF200CA67 : Reserved.
    EcCore1iteratiInlineDec1ErrCtrl_t ecCore1iteratiInlineDec1ErrCtrl;     ///< 0xF200CA68 : EC Core 1 Iterative Inline Decoder 1 Error Control
    uint8_t rsvd_RF200CA6A[86];                                            ///< 0xF200CA6A - 0XF200CABF                                         ///< 0xF200CA4A - 0xF200CABF : Reserved.
    EcCore1ItrRetryDecCtrlAnd01_t ecCore1ItrRetryDecCtrlAnd0_1;            ///< 0xF200CAC0 : EC Core 1 Iterative Retry Decoder Control and Status 1
    EcCore1ItrRetryDecStartLoc_t ecCore1ItrRetryDecStartLoc;               ///< 0xF200CAC2 : EC Core 1 Iterative Retry Decoder Start Location and Error Length
    uint8_t rsvd_RF200CAC4[2];                                             ///< 0xF200CAC4 - 0xF200CAC5 : Reserved.
    EcCore1ItrRetryDecCtrlAnd12_t ecCore1ItrRetryDecCtrlAnd1_2;            ///< 0xF200CAC6 : EC Core 1 Iterative Retry Decoder Control and Status 2
    uint8_t rsvd_RF200CAC8[12];                                            ///< 0xF200CAC8 - 0xF200CAD3 : Reserved.
    EcCore1ItrRetryDec20bFast_t ecCore1ItrRetryDec20bFast;                 ///< 0xF200CAD4 : EC Core 1 Iterative Retry Decoder 20b (FAST_RTMB_MODE = 0)
    uint8_t rsvd_RF200CAD6[42];                                            ///< 0xF200CAD6 - 0xF200CAFF : Reserved.
    EcCore1ItrEncCtrlAndStat_t ecCore1ItrEncCtrlAndStat;                   ///< 0xF200CB00 : EC Core 1 Iterative Encoder Control and Status
} Ecu_t;
COMPILE_ASSERT(offsetof(Ecu_t,ecCore1ItrEncCtrlAndStat)==0xF200CB00-0xF200C000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile Ecu_t rEcu;                          ///< 0xF200C000-0xF200CBE0 : Ecu_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
extern Ecu_t mEcu;                          ///< 0xF200C000-0xF200CBE0 : Ecu_t MODULE

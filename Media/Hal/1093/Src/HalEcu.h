#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2012  -  All rights reserved
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
//! @brief Pure HAL API definition for ECU(Error Correction Unit) HW control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define MCRC_SIZE                   4   ///< 4-bytes
#define MPECC_SIZE                  12  ///< 12-bytes
#define MAX_AU_FORMAT               32  ///< MAX # of AU format

//AU FORMAT types
#define AU_FORMAT_META_USER         0   ///< AU_FORMAT#0 : META + USER for META data mode
#define AU_FORMAT_LONG_MODE         1   ///< AU_FORMAT#1 : LONG(RAW)   for META data mode
#define AU_FORMAT_USER_ONLY         2   ///< AU_FORAMT#2 : USER only   for META data mode
#define AU_FORMAT_META_ONLY         3   ///< AU_FORMAT#3 : META only   for META data mode
#define AU_FORMAT_DISSCAMBLE        4   ///< AU_FORMAT#4 : DISABLE SCARMBLE && ECCINVD
#define AU_FORMAT_READ_TOTAL_AU     5   ///< AU_FORMAT#5 : Actually Long Mode

#define EC_DEC_CUR_RAID_DEC_0CHK_ERR         0x8000  //RAID decode RAID Buffer Zero Check Error
#define EC_DEC_CUR_ERRBUF_DPP_ERR            0x2000  //Error Buffer DPP Error Status for Current AU READ
#define EC_DEC_CUR_DECBUF_DPP_ERR            0x1000  //Decode Buffer DPP Error Status for Current AU READ
#define EC_DEC_CUR_RAID_DEC_INVALID_ID_ERR   0x0800  //RAID Invalid ID Status for Current AU READ
#define EC_DEC_CUR_RAID_DEC_DPP_ERR          0x0400  //RAID Buffer DPP Error Status for Current AU READ
#define EC_DEC_CUR_DEC_DS_DPP_ERR            0x0200
#define EC_DEC_CUR_ECU_STRG_SYND_ERR         0x0100   //Strong Syndrome Erro Status for Current AU READ.
#define EC_DEC_CUR_AU_AU16                   0x0080   //Current AU at 16th AU.
#define EC_DEC_CUR_AU_ERASE                  0x0040   //Erase Pager Status for Current AU READ.
#define EC_DEC_CUR_AU_AUX_MMATCH             0x0020   //Auxiliary Data Mismatch Error Status for Current AU
#define EC_DEC_CUR_AU_MCRC                   0x0010   //AU MCRC Error Status for Current AU READ.
#define EC_DEC_CUR_AU_META_MCRC              0x0008   //META_MCRC Error Status for Current AU.
#define EC_DEC_CUR_AU_OVRLMT                 0x0004   //Over-Limit Status for Current AU READ.
#define EC_DEC_CUR_AU_UNCORR                 0x0002   //Uncorrectable Status for Current AU READ.
#define EC_DEC_STOP_RESUME                   0x0001   //EC Decoder resume

///@brief AU FORMAT types
typedef enum
{
    cAuFormatMetaUser = 0,          ///< AU_FORMAT#0 : META + USER for META data mode
    cAuFormatLongMode,              ///< AU_FORMAT#1 : LONG(RAW)   for META data mode
    cAuFormatUserOnly,              ///< AU_FORAMT#2 : USER only   for META data mode
    cAuFormatMetaOnly,              ///< AU_FORMAT#3 : META only   for META data mode
    cAuFormatPseudoRawMode,         ///< AU_FORMAT#4 : DISABLE SCRAMBLE & EccInvMode
#if (META_DATA_MODE==1)
    cAuFormatLongMode2,             ///< AU_FORMAT#5 : LONG(RAW)   for 2nd data transfer
#endif
    cNumberOfAuFormats,             ///< Total number of tasks. Has to be the last item!
    cAuFormat_NULL=0x7FFFFFFF
} EcuAuFormats_t;
/// @brief ECU AU to Host sector size ratio
typedef enum
{
    ECU_AU2HOST_RATIO_1_TO_1=1, ///< Host sector size to AU size ratio = 1 : 1
    ECU_AU2HOST_RATIO_2_TO_1,   ///< Host sector size to AU size ratio = 2 : 1
    ECU_AU2HOST_RATIO_3_TO_1,   ///< Host sector size to AU size ratio = 3 : 1
    ECU_AU2HOST_RATIO_4_TO_1,   ///< Host sector size to AU size ratio = 4 : 1
    ECU_AU2HOST_RATIO_5_TO_1,   ///< Host sector size to AU size ratio = 5 : 1
    ECU_AU2HOST_RATIO_6_TO_1,   ///< Host sector size to AU size ratio = 6 : 1
    ECU_AU2HOST_RATIO_7_TO_1,   ///< Host sector size to AU size ratio = 7 : 1
    ECU_AU2HOST_RATIO_8_TO_1,   ///< Host sector size to AU size ratio = 8 : 1
    ECU_AU2HOST_RATIO_9_TO_1,   ///< Host sector size to AU size ratio = 9 : 1
    ECU_AU2HOST_RATIO_10_TO_1,  ///< Host sector size to AU size ratio = 10 : 1
    ECU_AU2HOST_RATIO_11_TO_1,  ///< Host sector size to AU size ratio = 11 : 1
    ECU_AU2HOST_RATIO_12_TO_1,  ///< Host sector size to AU size ratio = 12 : 1
    ECU_AU2HOST_RATIO_13_TO_1,  ///< Host sector size to AU size ratio = 13 : 1
    ECU_AU2HOST_RATIO_14_TO_1,  ///< Host sector size to AU size ratio = 14 : 1
    ECU_AU2HOST_RATIO_15_TO_1,  ///< Host sector size to AU size ratio = 15 : 1
    ECU_AU2HOST_RATIO_16_TO_1,  ///< Host sector size to AU size ratio = 16 : 1
    ECU_AU2HOST_RATIO_NULL=0x7FFFFFFF
} EcuAu2hostRatio_t;

/// @brief ECU Cordwords per AU
typedef enum
{
    ECU_ECC_CORE0=0,            ///< ECC Core0
    ECU_ECC_CORE1,              ///< ECC Core1
    ECU_ECC_CORE_END,           ///< ECC Core End
    ECU_ECC_CORE_NULL=0x7FFFFFFF
} EcuEccCore_t;

/// @brief ECU DEC Type
typedef enum
{
    ECU_IDEC0=0,                 ///< ECC ITR Inline decoder0 (IDEC0)
    ECU_IDEC1,                   ///< ECC ITR Inline decoder1 (IDEC1)
    ECU_RDEC,                    ///< ECC ITR Retry decoder (RDEC)
    ECU_EC_NULL=0x7FFFFFFF
} EcuDecType_t;

/// @brief ECU Cordwords per AU
typedef enum
{
    ECU_CW_PER_AU_1=1,          ///< # of ECC cordwords per AU = 1
    ECU_CW_PER_AU_2,            ///< # of ECC cordwords per AU = 2
    ECU_CW_PER_AU_3,            ///< # of ECC cordwords per AU = 3
    ECU_CW_PER_AU_4,            ///< # of ECC cordwords per AU = 4
    ECU_CW_PER_AU_NULL=0x7FFFFFFF
} EcuCwPerAu_t;

/// @brief ECU ECC Invert
typedef enum
{
    ECU_ECC_INV_ENABLED=0,      ///< Invert ECC output
    ECU_ECC_INV_DISABLED,       ///< Do not invert ECC output
    ECU_ECC_INV_NULL=0x7FFFFFFF
} EcuEccInv_t;

/// @brief ECU AU MCRC
typedef enum
{
    ECU_AU_MCRC_DISABLED=0,      ///< Disable AU MCRC
    ECU_AU_MCRC_ENABLED,        ///< Enable AU MCRC
    ECU_AU_MCRC_NULL=0x7FFFFFFF
} EcuAuMcrc_t;

/// @brief ECU AU MCRC Invert
typedef enum
{
    ECU_AU_MCRC_INV_ENABLED=0,  ///< Invert AU MCRC
    ECU_AU_MCRC_INV_DISABLED,   ///< Do not invert AU MCRC
    ECU_AU_MCRC_INV_NULL=0x7FFFFFFF
} EcuAuMcrcInv_t;

/// @brief ECU META ECC
typedef enum
{
    ECU_META_ECC_DISABLED=0,     ///< Disable META ECC
    ECU_META_ECC_ENABLED,       ///< Enable META ECC
    ECU_META_ECC_NULL=0x7FFFFFFF
} EcuMetaEcc_t;

/// @brief ECU META MCRC
typedef enum
{
    ECU_META_MCRC_DISABLED=0,    ///< Disable META MCRC
    ECU_META_MCRC_ENABLED,      ///< Enable META MCRC
    ECU_META_MCRC_NULL=0x7FFFFFFF
} EcuMetaMcrc_t;

/// @brief ECU ITR Code
typedef enum
{
    ECU_ITR_CODE_0=0,           ///< ITR memory 0
    ECU_ITR_CODE_1,             ///< ITR memory 1
    ECU_ITR_CORE_NULL=0x7FFFFFFF
} EcuItrCode_t;

/// @brief ECU MPECC
typedef enum
{
    ECU_MPECC_DISABLED=0,       ///< Disable MPECC
    ECU_MPECC_ENABLED,          ///< Enable MPECC
    ECU_MPECC_NULL=0x7FFFFFFF
} EcuMpecc_t;

/// @brief ECU MPECC DDR Combined/Separate
typedef enum
{
    ECU_MPECC_LOC_SEP=0,        ///< MPECC is stored in separate location from data field
    ECU_MPECC_LOC_COMB,         ///< MPECC is stored continuously and folloing data field
    ECU_MPECC_LOC_NULL=0x7FFFFFFF
} EcuMpeccLoc_t;

/// @brief ECU MPECC DDR Combined/Separate
typedef enum
{
    ECU_BCRC_DISABLED=0,        ///< Disable 4bytes of BCRC
    ECU_BCRC_ENABLED,           ///< Enable 4bytes of BCRC
    ECU_BCRC_NULL=0x7FFFFFFF
} EcuBcrc_t;

/// @brief ECU HLBA size
typedef enum
{
    ECU_HLBA_SIZE_NO=0,         ///< No HLBA
    ECU_HLBA_SIZE_4B,           ///< 4bytes of HLBA
    ECU_HLBA_SIZE_8B,           ///< 8bytes of HLBA
    ECU_HLBA_SIZE_NULL=0x7FFFFFFF
} EcuHlbaSize_t;

/// @brief ECU HLBA Stripe
typedef enum
{
    ECU_HLBA_STRIP_NO=0,        ///< No HLBA stripping
    ECU_HLBA_STRIP_ALL,         ///< Strip all HLBA
    ECU_HLBA_STRIP_ONE,         ///< Keep one HLBA
    ECU_HLBA_STRIP_NULL=0x7FFFFFFF
} EcuHlbaStrip_t;

/// @brief ECU Scrambler
typedef enum
{
    ECU_SCR_DISABLED=0,         ///< Disable scrambler
    ECU_SCR_ENABLED,            ///< Enable scrambler
    ECU_SCR_NULL=0x7FFFFFFF
} EcuScr_t;

/// @brief ECU Scrambler Invert
typedef enum
{
    ECU_SCR_INV_DISABLED=0,     ///< Invert scrambler output
    ECU_SCR_INV_ENABLED,        ///< Do not scrambler output
    ECU_SCR_INV_NULL=0x7FFFFFFF
} EcuScrInv_t;

/// @brief ECU Scrambler Seed Source
typedef enum
{
    ECU_SCR_SEED_SRC_0=0,       ///< Seed value is zero
    ECU_SCR_SEED_SRC_1,         ///< Seed value is register value
    ECU_SCR_SEED_SRC_2,         ///< PBA
    ECU_SCR_SEED_SRC_3,         ///< PBA ^ MASK
    ECU_SCR_SEED_SRC_4,         ///< PBA ^ ~MASK
    ECU_SCR_SEED_SRC_5,         ///< The first 4 bytes of AUX or META
    ECU_SCR_SEED_SRC_NULL=0x7FFFFFFF
} EcuScrSeedSrc_t;

/// @brief ECU Scrambler Seed Map Select
typedef enum
{
    ECU_SCR_SEED_MAP_ROM=0,     ///< Scrambler seed ROM mapping
    ECU_SCR_SEED_MAP_SRAM,      ///< Scrambler seed SRAM mapping
    ECU_SCR_SEED_MAP_NULL=0x7FFFFFFF
} tECU_SCR_SEED_MAP_SEL;

/// @brief ECU Scrambler Seed Map Bypass
typedef enum
{
    ECU_SCR_SEED_MAP_BYPASS_DISABLED=0,    ///< Disable scrambler seed map bypass
    ECU_SCR_SEED_MAP_BYPASS_ENABLED,       ///< Enable scramber seed map bypass
    ECU_SCR_SEED_MAP_BYPASS_NULL=0x7FFFFFFF
} tECU_SCR_SEED_MAP_BYPASS;

/// @brief ECU RAID
typedef enum
{
    ECU_RAID_DISABLED=0,         ///< Disable RAID
    ECU_RAID_ENABLED,            ///< Enable RAID
    ECU_RAID_NULL=0x7FFFFFFF
} EcuRaid_t;

/// @brief ECU Configuration function number
typedef enum
{
    ECU_CFN_MODE=0,             ///< ECU mode
    ECU_CFN_PHY_PAGE_SIZE,      ///< Physical Page Size
    ECU_CFN_META_SIZE,          ///< META size
    ECU_CFN_ECC_INV,            ///< ECC Invert
    ECU_CFN_AU_MCRC,            ///< AU MCRC
    ECU_CFN_AU_MCRC_INV,        ///< AU MCRC invert
    ECU_CFN_META_ECC,           ///< META ECC
    ECU_CFN_META_MCRC,          ///< META MCRC
    ECU_CFN_BCH_LEVEL,          ///< BCH Level
    ECU_CFN_WEAK_POWER,         ///< Weak Power
    ECU_CFN_WEAK_THRESHOLD,     ///< Weak Threshold
    ECU_CFN_STRONG_POWER,       ///< Strong Power
    ECU_CFN_STRONG_THRESHOLD,   ///< Strong Threshold
    ECU_CFN_ITR_CODE,           ///< ITR Code
    ECU_CFN_ITR0_PARITY_SIZE,   ///< ITR0 Parity Size
    ECU_CFN_ITR1_PARITY_SIZE,   ///< ITR1 Parity Size
    ECU_CFN_MPECC,              ///< MPECC
    ECU_CFN_HLBA_SIZE,          ///< HLBA Size
    ECU_CFN_HLBA_STRIP,         ///< HLBA Strip
    ECU_CFN_META_SCR,           ///< META Scrambler
    ECU_CFN_META_SCR_INV,       ///< META Scrambler Invert
    ECU_CFN_META_SCR_SEED_SRC,  ///< META Scrambler Seed Source
    ECU_CFN_USER_SCR,           ///< USER Scrambler
    ECU_CFN_USER_SCR_INV,       ///< USER Scrambler Invert
    ECU_CFN_USER_SCR_SEED_SRC,  ///< USER Scrambler Seed Source
    ECU_CFN_RAID,               ///< RAID
    ECU_CFN_CALCULATE_AU_BYTES, ///< Calculate Encoded AU bytes
    ECU_CFN_MAX,
    ECU_CFN_NULL=0x7FFFFFFF
} EcuCfn_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief ECU AU_FORMAT Configuration
typedef struct
{
    uint16_t index;                      ///< [ 0] table index (additional information)
    uint16_t auxMetaDataMode;            ///< [ 1] META data mode(0), AUX data mode(1) (additional information)
    uint16_t xferSizePerAu;              ///< [ 2] Transfer size to program or read one AU (additional information)
    uint16_t auxSize;                    ///< [ 3] AUX size (additional information)
    uint16_t fit2PhyPage;                ///< [ 4] FIT_2_PHY_PAGE
    uint16_t eccInvMode;                 ///< [ 5] ECC_INV_MODE
    uint16_t mcrcInvMode;                ///< [ 6] MCRC_INV_MODE
    uint16_t metaEccEn;                  ///< [ 7] META_ECC_EN
    uint16_t eccLdpcDis;                 ///< [ 8] ECC_LDPC_DIS
    uint16_t eccDecBypassEn;             ///< [ 9] ECC_DEC_BYPASS_EN/ECC_ITR_BYPSS_EN
    uint16_t metaBchBypassEn;            ///< [10] META_BCH_BYPASS_EN
    uint16_t eccItrCodeSel;              ///< [11] ECC_ITR_CODE_SEL
    uint16_t ecuMpeccEn;                 ///< [12] ECU_MPECC_ENABLED
    uint16_t ecuHlbaMode;                ///< [13] ECU_HLBA_MODE
    uint16_t ecuHlbaStripMode;           ///< [14] ECU_HLBA_STRIP_MODE
    uint16_t ecuHlbaInsertOption;        ///< [15] ECU_HLBA_INSERT_OPTION
    uint16_t ecuMetaRdSkipDecEn;         ///< [16] ECU_META_RD_SKIP_DEC_EN
    uint16_t bcrcEn;                     ///< [17] BCRC_EN
    uint16_t hostSectorSize;             ///< [18] HOST_SECTOR_SIZE
    uint16_t ecu2HostRatio;              ///< [19] ECU2HOST_RATIO
    uint16_t metaSize;                   ///< [20] META_SIZE
    uint16_t scrMetaEn;                  ///< [21] SCR_META_EN
    uint16_t scrMetaInvEn;               ///< [22] SCR_META_INV_EN
    uint16_t scrUserEn;                  ///< [23] SCR_USER_EN
    uint16_t scrUserSeedSrc;             ///< [24] SCR_USER_SEED_SRC
    uint16_t cwPerAu;                    ///< [25] CW_PER_AU
    uint16_t auMcrcEn;                   ///< [26] AU_MCRC_EN
    uint16_t phyPageSize;                ///< [27] PHY_PAGE_SIZE
    uint16_t metaPower;                  ///< [28] META_T
    uint16_t metaThreshold;              ///< [29] META_THRSHD
    uint16_t metaMcrcEn;                 ///< [30] META_MCRC_EN
    uint16_t threshold;                  ///< [31] THRSHD -=------
    uint16_t eraseThreshold;             ///< [32] ERASE_THRSHD
} EcuAuFormatInfoTbl_t;

/// @brief ECU Configuration
typedef struct
{
    EcuCfn_t cfn;
    uint32_t param[4];
}
tECU_CFG;
COMPILE_ASSERT((sizeof(tECU_CFG) & 0x3) == 0, "please align to 4 bytes!");
/// @brief ECU information
typedef struct
{
    uint32_t auFormat;
    uint32_t auSize;
    uint32_t physicalPageSize;
    uint32_t physicalPageSizePerAu;
    uint32_t auMetaSize;
    uint32_t auPerPage;
    uint32_t cwPerAu;
    uint32_t xferSizePerAu;
    EcuRaid_t raid;
} EcuInfo_t;
COMPILE_ASSERT((sizeof(EcuInfo_t) & 0x3) == 0, "please align to 4 bytes!");

typedef struct
{
    U16 cw1size;
    U16 cw2size;
} CwInfo_t;
COMPILE_ASSERT((sizeof(CwInfo_t) & 0x3) == 0, "please align to 4 bytes!");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern const EcuAuFormatInfoTbl_t gEcuAuFormatList[];
extern       EcuInfo_t gEcuInfo;
extern       CwInfo_t gCwInfo;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t  HalEcu_OneTimeInit(InitBootMode_t initMode);
Error_t  HalEcu_Resume(void);
void     HalEcu_DumpInfo(void);
uint32_t HalEcu_GetEncStatus(void);
uint32_t HalEcu_GetDecStatus(void);

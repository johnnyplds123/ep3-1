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
//! \file
//! @brief ECU(Error Correction Unit) control API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "RegClockPLL.h"
#include "RegEcu.h"
#include "RegSequencer.h"
#include "HalLdpcSch.h"
#include "HalSeq.h"

#include "HalEcu.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#if (AU_SIZE == 0x800)
#define ECU_CW_PER_AU               1       ///< Cordword number per AU
#elif (AU_SIZE == 0x2000)
#define ECU_CW_PER_AU               4       ///< Cordword number per AU
#else
#define ECU_CW_PER_AU               2       ///< Cordword number per AU
#endif
#define ECU_META_POWER              10      ///< META power
#define ECU_META_THRESHOLD          0       ///< MEAT threshold
#define COMPR_EN                    0       ///< Compress Enable.



#define cMaxAuErrorThreshold        0xFFF   ///< Max error threshold for AU over limit

#if _NAND_ == NAND_3DTLCG2
#define cMaxCwErrorThreshold        50      ///< Max error threshold for CW over limit
#define MAX_ITR_NUM_EC0_IDEC0       63      ///< Max number of iterations
#define MAX_ITR_NUM_EC0_IDEC1       63      ///< Max number of iterations
#define MAX_ITR_NUM_EC1_IDEC0       63      ///< Max number of iterations
#define MAX_ITR_NUM_EC1_IDEC1       63      ///< Max number of iterations
#elif _NAND_ == NAND_MLC
#define cMaxCwErrorThreshold        60      ///< Max error threshold for CW over limit
#define MAX_ITR_NUM_EC0_IDEC0       20      ///< Max number of iterations
#define MAX_ITR_NUM_EC0_IDEC1       20      ///< Max number of iterations
#define MAX_ITR_NUM_EC1_IDEC0       20      ///< Max number of iterations
#define MAX_ITR_NUM_EC1_IDEC1       20      ///< Max number of iterations
#endif

#define IS_1BIT_DEC                 0       ///< 0:1-bit decoder, 1:2-bit decoder
#define IS_2BIT_DEC                 1       ///< 0:1-bit decoder, 1:2-bit decoder

#define METHOD                      1       ///< Method1: Reliability info(bits) won't be updated.
                                            ///< Method2: Reliability info(bits) can be updated
#define TYPE                        1       ///< NAND I/F supporting soft information
                                            ///< (1: Type0, 4: Type1, 6: Type2)
#define OPTION                      0       ///< 2-bit 2 AU mode option
                                            ///< Option 1: process one 2-bit AU at a time.
                                            ///< Option 2: process two 2-bit AUs at the same time.

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define CEIL(X)             (((X)-(uint32_t)(X))==0?(uint32_t)(X):(uint32_t)(X)+1)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void Ecu_ConfigureIdec(uint8_t nMaxItrNumber_ec0_idec0, uint8_t nMaxItrNumber_ec0_idec1, uint8_t nMaxItrNumber_ec1_idec0,uint8_t nMaxItrNumber_ec1_idec1, uint8_t nIs2bitDec, uint8_t nMethod, uint8_t nType, uint8_t nOption);
static void Ecu_EnDisableSendUncorrToDdr(bool enDisable);
static void Ecu_EnDisableDecoderErrorCount(bool enDisable);
static uint32_t Ecu_CalEncodedAuBytes(EcuAuFormatInfoTbl_t *pAuFormat);
static Error_t  Ecu_SetAuFormat(void);
static Error_t  Ecu_Configure(tECU_CFG *pEcuCfg);
//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
__align(16) EcuInfo_t gEcuInfo;
__align(16) CwInfo_t  gCwInfo;

#if (META_DATA_MODE == 0)
// AU_FORMAT#0 : META + USER for META data mode
// AU_FORMAT#1 : LONG(RAW)   for META data mode
// AU_FORAMT#2 : USER only   for META data mode
// AU_FORMAT#3 : META only   for META data mode
const __align(16) EcuAuFormatInfoTbl_t gEcuAuFormatList[cNumberOfAuFormats] =
{
///         0, 1,       2,              3, 4, 5, 6, 7, 8, 9 10,11,12,13,14,15,16,17,       18,                 19,             20,                 21,22,23,24,            25,26,     27,             28,29,30, 31, 32
/*1st*/    {0, 0,       0,              0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, LBU_SIZE, (AU_SIZE/LBU_SIZE), META_DATA_SIZE, ECU_META_THRESHOLD, 0, 0, 0, ECU_CW_PER_AU, 1, 0xFFFF, ECU_META_POWER, 0, 1,  0,  0},
/*2nd*/    {1, 0, AU_SIZE,              0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, LBU_SIZE, (AU_SIZE/LBU_SIZE),              0,                  0, 0, 0, 0, ECU_CW_PER_AU, 0, 0xFFFF,              0, 0, 0,  0,  0},
/*3rd*/    {2, 0,       0,              0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, LBU_SIZE, (AU_SIZE/LBU_SIZE), META_DATA_SIZE, ECU_META_THRESHOLD, 0, 0, 0, ECU_CW_PER_AU, 1, 0xFFFF, ECU_META_POWER, 0, 1,  0,  0},
/*4th*/    {3, 0,       0,              0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,        0,                  0, META_DATA_SIZE, ECU_META_THRESHOLD, 0, 0, 0, ECU_CW_PER_AU, 0, 0xFFFF, ECU_META_POWER, 0, 1,  0,  0},
};
#else
// AU_FORMAT#0 : AUX + USER  for AUX data mode
// AU_FORMAT#1 : LONG(RAW)   for AUX data mode
// (AUX_SIZE register should be set to zero)
const __align(16) EcuAuFormatInfoTbl_t gEcuAuFormatList[cNumberOfAuFormats] =
{
///                  0, 1,       2,       3,              4, 5, 6, 7, 8, 9 10,11,12,13,14,15,16,17, 18,            19,                    20, 21,22,23,24,            25,26,     27,  28,29,30, 31, 32
/*1st:aux+user*/    {0, 1,       AU_SIZE, META_DATA_SIZE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, LBU_SIZE     , (AU_SIZE/LBU_SIZE),     0,  1, 0, 1, 2, ECU_CW_PER_AU, 1, 0xFFFF,   0, 0, 0, cMaxCwErrorThreshold, 40},
/*2nd:long*/        {1, 1,       AU_SIZE,              0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, LBU_SIZE     , (AU_SIZE/LBU_SIZE),     0,  0, 0, 0, 0, ECU_CW_PER_AU, 0, 0xFFFF,   0, 0, 0, cMaxCwErrorThreshold,  0},
/*3rd:val5*/        {2, 1,          4396,              0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, LBU_SIZE     , (AU_SIZE/LBU_SIZE),     0,  0, 0, 0, 0, ECU_CW_PER_AU, 0, 0xFFFF,   0, 0, 0, cMaxCwErrorThreshold,  0},
/*4th:val1*/        {3, 1,          2198,              0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, LBU_SIZE     , (AU_SIZE/LBU_SIZE/2),   0,  0, 0, 0, 0,             1, 0, 0xFFFF,   0, 0, 0, cMaxCwErrorThreshold,  0},
/*5th:dis scramble*/{4, 1,       AU_SIZE, META_DATA_SIZE, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, LBU_SIZE     , (AU_SIZE/LBU_SIZE),     0,  1, 0, 0, 2, ECU_CW_PER_AU, 1, 0xFFFF,   0, 0, 0, cMaxCwErrorThreshold, 40},
/*6th:read totalAU*/{5, 1, TOTAL_AU_SIZE,              0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, TOTAL_AU_SIZE,                    1,   0,  0, 0, 0, 2, ECU_CW_PER_AU, 0, 0xFFFF,   0, 0, 0, cMaxCwErrorThreshold,  0},
};
#endif

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    One-timie initialization of ECU HW.

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalEcu_OneTimeInit(InitBootMode_t initMode)
{
    tECU_CFG ecuCfg;

    smEcuInfoCpu1Addr = (U32)&gEcuInfo;
    MEM_CLR(&gEcuInfo,sizeof(EcuInfo_t));

    gEcuInfo.auSize                                 = AU_SIZE;
    gEcuInfo.raid                                   = ECU_RAID_ENABLED;
    //rClockPll.clkSelectioCtrl.b.CLK_SEL_ECC_OSC     = 1;    // OSC
    rClockPll.clkSelectioCtrl.b.ECC_CLK_SEL         = 2;    // 1GHz / ??
    //rClockPll.clkEnCtrl.b.ECC_CLK_EN                = 1;    // ECC enable
    //rClockPll.clkSelectioCtrl.b.CLK_SEL_ECC_OSC     = 0;    // PLL

    // Set ECU mask bits.
    rEcu.ecuCtrlAndCfg2.all                         = 0; ///< Clear all
#if (_SOC_REV_ < 0x0210)
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_AXI_PAR_ERR       = 0; ///< Do not set this bit
#endif

    mEcu.ecuCtrlAndCfg2.b.STOP_ON_MPECC_ERR         = 1;
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_HLBA_MMATCH       = 1;

    mEcu.ecuCtrlAndCfg2.b.STOP_ON_UNCORR            = 1;
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_MCRC_ERR          = 1;
    #if CHK_FW_TYPE(FW_AGING)
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_CW_OVRLMT         = 0;
    #else
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_CW_OVRLMT         = 0;
    #endif
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_AUX_MMATCH        = 1;
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_DPP_ERR           = 1;
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_16_AU             = 0;
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_RAID_DPP_ERR      = 1;
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_AU_NUM            = 0;
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_AU_OVRLMT         = 0;
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_RAID_BUF0CHK_ERR  = 1;
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_AXI_ERR           = 1;
    mEcu.ecuCtrlAndCfg2.b.STOP_ON_ERASE             = 1;

    rEcu.ecuCtrlAndCfg3.all                         = 0;
    mEcu.ecuCtrlAndCfg4.b.XOR_DPP_POLARITY          = 1; ///< Odd parity
    mEcu.ecuCtrlAndCfg4.b.ERASE_PAGE_DET_ONLY       = 0;
    mEcu.ecuCtrlAndCfg4.b.META_AUX_RCVR             = 0;
    mEcu.ecuCtrlAndCfg4.b.FWLL_META_READ_SKIP_EN    = 0;
    mEcu.ecuCtrlAndCfg4.b.SQCR_AUX_PARITY_CHK_EN    = 0;
#if (_SOC_REV_ < 0x0210)
    mEcu.ecuCtrlAndCfg4.b.FLC_LL_PARITY_CHK         = 0;
#endif

    mEcu.ecuCtrlAndCfg4.b.SEND_UNCORR_TODDR         = 0;
    mEcu.hwRrdCtrlRegister.b.HWLL_ABORT_ON_DEC_ERR  = 1;

    mEcu.ecuCtrlAndCfg4.b.RF200C006_8_RSVD          = 0;
    mEcu.ecuCtrlAndCfg4.b.SEND_UNCORR_HISTO_ECNT    = 1;
    mEcu.ecuCtrlAndCfg4.b.SOFT_INFO_IF_TYPE         = 0;   // fix

    mEcu.ecuCtrlAndCfg5And6.b.FIT_2_PHY_PAGE        = 0;
    mEcu.ecuCtrlAndCfg5And6.b.ECC_INV_MODE          = ECU_ECC_INV_ENABLED;
    mEcu.ecuCtrlAndCfg5And6.b.MCRC_INV_MODE         = ECU_AU_MCRC_INV_ENABLED;
    mEcu.ecuCtrlAndCfg5And6.b.META_ECC_EN           = ((META_DATA_MODE == 0) && (META_DATA_SIZE != 0)) ? ECU_META_ECC_ENABLED : ECU_META_ECC_DISABLED;
    mEcu.ecuCtrlAndCfg5And6.b.ECC_ITR_BYPASS_EN     = 0;
    mEcu.ecuCtrlAndCfg5And6.b.META_BCH_BYPASS_EN    = 0;
    mEcu.ecuCtrlAndCfg5And6.b.MANUAL_ERASE_THRSHD_EN= 0;
    mEcu.ecuCtrlAndCfg5And6.b.ECC_LDPC_DIS          = 0; ///< Enable LDPC
    mEcu.ecuCtrlAndCfg5And6.b.ECC_ITR_CODE_SEL      = 0; ///< Default ITR Code#0

    mEcu.ecuCtrlAndCfg7.b.ECU_HOST_MPECC_EN         = (META_MPECC_SIZE == 0) ? ECU_MPECC_DISABLED : ECU_MPECC_ENABLED;
    mEcu.ecuCtrlAndCfg7.b.ECU_META_MPECC_EN         = (META_MPECC_SIZE == 0) ? ECU_MPECC_DISABLED : ECU_MPECC_ENABLED;
    mEcu.ecuCtrlAndCfg7.b.ECU_HLBA_MODE             = (META_MPECC_SIZE == 0) ? ECU_HLBA_SIZE_NO : ECU_HLBA_SIZE_4B;
    mEcu.ecuCtrlAndCfg7.b.ECU_HLBA_STRIP_MODE       = ECU_HLBA_STRIP_NO;
    mEcu.ecuCtrlAndCfg7.b.RF200C010_6_RSVD          = 0;
    mEcu.ecuCtrlAndCfg7.b.ECU_HLBA_INSERT_OPTION    = 0;
    mEcu.ecuCtrlAndCfg7.b.ECU_HOST_MPECC_CHK        = 0; ///< Disable
    mEcu.ecuCtrlAndCfg7.b.ECU_META_MPECC_CHK        = 0; ///< Disable

    mEcu.ecuCtrlAndCfg8.b.HOST_SECTOR_SIZE          = LBU_SIZE;

    mEcu.ecuCtrlAndCfg9.b.ECU2HOST_RATIO            = (AU_SIZE/LBU_SIZE);

    mEcu.ecuCtrlAndCfg10.b.META_SIZE                = ((META_DATA_MODE == 0) && (META_DATA_SIZE != 0)) ? META_DATA_SIZE : 0;

    mEcu.ecuCtrlAndCfg13.b.SCR_META_INV_EN          = ECU_SCR_INV_DISABLED;
    mEcu.ecuCtrlAndCfg13.b.SCR_EN                   = ECU_SCR_ENABLED;
    mEcu.ecuCtrlAndCfg13.b.SCR_SEED_SRC             = ECU_SCR_SEED_SRC_0;
    mEcu.ecuCtrlAndCfg13.b.SCR_SEED_MAP_SEL         = ECU_SCR_SEED_MAP_ROM;
    mEcu.ecuCtrlAndCfg13.b.SCR_SEED_MAP_BYPASS      = ECU_SCR_SEED_MAP_BYPASS_DISABLED;

    mEcu.MAX_AU_NUM_THR_L                           = 0xFFFF;
    mEcu.MAX_AU_NUM_THR_H                           = 0;

    mEcu.ecuCfg1.b.CW_PER_AU                        = ECU_CW_PER_AU;
    mEcu.ecuCfg1.b.AU_MCRC_EN                       = ECU_AU_MCRC_ENABLED;
    mEcu.ecuCfg1.b.AU_MCRC_SIZE_OPT                 = 0; ///< 4B

    mEcu.ecuCfg3.b.META_T                           = ((META_DATA_MODE == 0) && (META_DATA_SIZE != 0)) ? ECU_META_POWER : 0;
    mEcu.ecuCfg3.b.META_THRSHD                      = ((META_DATA_MODE == 0) && (META_DATA_SIZE != 0)) ? ECU_META_THRESHOLD : 0;
    mEcu.ecuCfg3.b.META_MCRC_EN                     = ((META_DATA_MODE == 0) && (META_DATA_SIZE != 0)) ? ECU_META_MCRC_ENABLED : ECU_META_MCRC_DISABLED;

    mEcu.ecuCfg6.b.ERASE_THRSHD                     = 0;

    mEcu.ecuRaidCtrlAndStat0.b.ECC_RAID_EN          = gEcuInfo.raid;

    HalEcu_DownloadLdpc();
    Ecu_EnDisableDecoderErrorCount(ENABLE);

    ecuCfg.cfn      = ECU_CFN_PHY_PAGE_SIZE;
    ecuCfg.param[0] = gNfInfo.dataBytesPerPage + gNfInfo.spareBytesPerPage;
    Ecu_Configure(&ecuCfg);     // configure ECU

    return cEcNoError;
}
#include "NormSection.h"
//-----------------------------------------------------------------------------
/**
    Dump ECU information\n
**/
//-----------------------------------------------------------------------------
void HalEcu_DumpInfo(void)
{
    uint32_t index;
    EcuAuFormatInfoTbl_t *pAuFormat = NULL;

    M_PRINTF("\n[ECU]\n");
    M_PRINTF("EcuSize;           %x\n",sizeof(EcuInfo_t));
    M_PRINTF("AuSize:            %9u bytes\n", gEcuInfo.auSize);
    M_PRINTF("AuMetaSize:        %9u bytes\n", gEcuInfo.auMetaSize);
    M_PRINTF("physicalPageSize:  %9u bytes\n", gEcuInfo.physicalPageSize);
    M_PRINTF("phyPageSizePerAu:  %9u bytes\n", gEcuInfo.physicalPageSizePerAu);
    M_PRINTF("xferSizePerAu:     %9u bytes\n", gEcuInfo.xferSizePerAu);
    M_PRINTF("AuPerPage:         %9u\n", gEcuInfo.auPerPage);
    M_PRINTF("Raid:              %s\n", (gEcuInfo.raid== ECU_RAID_DISABLED) ? "DISABLED" : "ENABLED");

    index = rEcu.ecuAuFmtCfgPtr.b.AU_FORMAT_CONF_PTR;
    pAuFormat = (EcuAuFormatInfoTbl_t *)&gEcuAuFormatList[index];

    M_PRINTF("AU_FORMAT#%d [%s]________________________\n", index, (pAuFormat->auxMetaDataMode == SEQ_DATA_MODE_META) ? "META" : "AUX ");
    M_PRINTF("XFER_SIZE/AU:            %9u bytes\n", pAuFormat->xferSizePerAu);
    if(pAuFormat->auxMetaDataMode == SEQ_DATA_MODE_AUX)
    {
        M_PRINTF("AUX_SIZE:                %9u bytes\n", pAuFormat->auxSize);
    }

    M_PRINTF("ECC_ITR_CODE_SEL:        %9u\n", rEcu.ecuCtrlAndCfg5And6.b.ECC_ITR_CODE_SEL);
    M_PRINTF("ECC_LDPC_DIS:            %9u\n", rEcu.ecuCtrlAndCfg5And6.b.ECC_LDPC_DIS);
    M_PRINTF("META_BCH_BYPASS_EN:      %9u\n", rEcu.ecuCtrlAndCfg5And6.b.META_BCH_BYPASS_EN);
    M_PRINTF("ECC_ITR_BYPASS_EN:       %9u\n", rEcu.ecuCtrlAndCfg5And6.b.ECC_ITR_BYPASS_EN);
    M_PRINTF("META_ECC_EN:             %9u\n", rEcu.ecuCtrlAndCfg5And6.b.META_ECC_EN);
    M_PRINTF("MCRC_INV_MODE:           %9u\n", rEcu.ecuCtrlAndCfg5And6.b.MCRC_INV_MODE);
    M_PRINTF("ECC_INV_MODE:            %9u\n", rEcu.ecuCtrlAndCfg5And6.b.ECC_INV_MODE);
    M_PRINTF("FIT_2_PHY_PAGE:          %9u\n", rEcu.ecuCtrlAndCfg5And6.b.FIT_2_PHY_PAGE);
    M_PRINTF("BCRC_EN:                 %9u\n", rEcu.ecuCtrlAndCfg7.b.BCRC_EN);
    M_PRINTF("ECU_META_MPECC_CHK:      %9u\n", rEcu.ecuCtrlAndCfg7.b.ECU_META_MPECC_CHK);
    M_PRINTF("ECU_HOST_MPECC_CHK:      %9u\n", rEcu.ecuCtrlAndCfg7.b.ECU_HOST_MPECC_CHK);
    M_PRINTF("ECU_HLBA_INSERT_OPTION:  %9u\n", rEcu.ecuCtrlAndCfg7.b.ECU_HLBA_INSERT_OPTION);
    M_PRINTF("ECU_HLBA_STRIP_MODE:     %9u\n", rEcu.ecuCtrlAndCfg7.b.ECU_HLBA_STRIP_MODE);
    M_PRINTF("ECU_HLBA_MODE:           %9u\n", rEcu.ecuCtrlAndCfg7.b.ECU_HLBA_MODE);
    M_PRINTF("ECU_META_MPECC_EN:       %9u\n", rEcu.ecuCtrlAndCfg7.b.ECU_META_MPECC_EN);
    M_PRINTF("ECU_HOST_MPECC_EN:       %9u\n", rEcu.ecuCtrlAndCfg7.b.ECU_HOST_MPECC_EN);
    M_PRINTF("HOST_SECTOR_SIZE:        %9u bytes\n", rEcu.ecuCtrlAndCfg8.b.HOST_SECTOR_SIZE);
    M_PRINTF("ECU2HOST_RATIO:          %9u\n", rEcu.ecuCtrlAndCfg9.b.ECU2HOST_RATIO);
    M_PRINTF("META_SIZE:               %9u\n", rEcu.ecuCtrlAndCfg10.b.META_SIZE);
    M_PRINTF("SCR_SEED_SRC:            %9u\n", rEcu.ecuCtrlAndCfg13.b.SCR_SEED_SRC);
    M_PRINTF("SCR_EN:                  %9u\n", rEcu.ecuCtrlAndCfg13.b.SCR_EN);
    M_PRINTF("SCR_META_INV_EN:         %9u\n", rEcu.ecuCtrlAndCfg13.b.SCR_META_INV_EN);
    M_PRINTF("AU_META_SIZE_OPT:        %9u\n", rEcu.ecuCfg1.b.AU_MCRC_SIZE_OPT);
    M_PRINTF("AU_MCRC_EN:              %9u\n", rEcu.ecuCfg1.b.AU_MCRC_EN);
    M_PRINTF("CW_PER_AU:               %9u\n", rEcu.ecuCfg1.b.CW_PER_AU);
    M_PRINTF("PHY_PAGE_SIZE:           %9u bytes\n", rEcu.ecuCfg2.b.PHY_PAGE_SIZE);
    M_PRINTF("META_THRSHD:             %9u\n", rEcu.ecuCfg3.b.META_THRSHD);
    M_PRINTF("META_MCRC_EN:            %9u\n", rEcu.ecuCfg3.b.META_MCRC_EN);
    M_PRINTF("META_T:                  %9u\n", rEcu.ecuCfg3.b.META_T);
    M_PRINTF("THRSHD:                  %9u\n", rEcu.ecuCfg5.b.THRSHD);
    M_PRINTF("ERASE_THRSHD:            %9u\n", rEcu.ecuCfg6.b.ERASE_THRSHD);
}

#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    Enable/Disable ECU power save mode

    @param[in]  enable    TRUE: Power Down, FALSE: Normal

**/
//-----------------------------------------------------------------------------
void HalEcu_PowerManage(bool enable)
{
    if (enable == TRUE)
    {
        mEcu.ecCore0EcCoreEncPowerDn1_1.b.PD_ECU_DP_ENC = 0xF;
        mEcu.llrPowerDnControl1.b.LLR_PD                = 1;
        mEcu.dmaPowerDnControl1.b.DMA_PD                = 1;
        mEcu.ecCore0EcCoreDecPowerDn1_1.b.PD_ITR_RDEC   = 1;
        mEcu.ecCore0EcCoreDecPowerDn1_1.b.PD_ECU_DP_DEC = 1;
        mEcu.ecCore0EcCoreDecPowerDn0_0.b.PD_ITR_IDEC   = 0x3;
    }
    else
    {
        mEcu.ecCore0EcCoreEncPowerDn1_1.b.PD_ECU_DP_ENC = 0;
        mEcu.llrPowerDnControl1.b.LLR_PD                = 0;
        mEcu.dmaPowerDnControl1.b.DMA_PD                = 0;
        mEcu.ecCore0EcCoreDecPowerDn1_1.b.PD_ITR_RDEC   = 0;
        mEcu.ecCore0EcCoreDecPowerDn1_1.b.PD_ECU_DP_DEC = 0;
        mEcu.ecCore0EcCoreDecPowerDn0_0.b.PD_ITR_IDEC   = 0;
    }
}
//-----------------------------------------------------------------------------
/**
    Get ECU Encoder Status

    @return error code
**/
//-----------------------------------------------------------------------------
uint32_t HalEcu_GetEncStatus(void)
{
    return rEcu.ecuEncodeStat0.all;
}

//-----------------------------------------------------------------------------
/**
    Get ECU Decoder Status

    @return error code
**/
//-----------------------------------------------------------------------------
uint32_t HalEcu_GetDecStatus(void)
{
    return rEcu.ecuDecStat0.all;
}

//-----------------------------------------------------------------------------
/**
    Resume ECU Encoder/Decoder

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalEcu_Resume(void)
{
    rEcu.ecuEncodeStat0.b.ECU_ENC_RESUME = 0;
    rEcu.ecuDecStat0.b.ECU_DEC_RESUME    = 0;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    ECU HW configuration

    @param[in]  pEcuCfg     ECU configuration pointer

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Ecu_Configure(tECU_CFG *pEcuCfg)
{
    switch (pEcuCfg->cfn)
    {
        case ECU_CFN_PHY_PAGE_SIZE:
            mEcu.ecuCfg2.b.PHY_PAGE_SIZE = pEcuCfg->param[0];
            gEcuInfo.physicalPageSize = pEcuCfg->param[0];
            gEcuInfo.physicalPageSizePerAu = gEcuInfo.physicalPageSize/(gEcuInfo.physicalPageSize/AU_SIZE);
            gEcuInfo.auPerPage = gEcuInfo.physicalPageSize / gEcuInfo.physicalPageSizePerAu;
            gEcuInfo.auMetaSize = gEcuInfo.auSize + ((gEcuAuFormatList[gEcuInfo.auFormat].auxMetaDataMode == SEQ_DATA_MODE_META)? gEcuAuFormatList[gEcuInfo.auFormat].metaSize : 0);
            gEcuInfo.cwPerAu    = ECU_CW_PER_AU;
            break;
        case ECU_CFN_RAID:
            mEcu.ecuRaidCtrlAndStat0.b.ECC_RAID_EN = pEcuCfg->param[0];
            gEcuInfo.raid = (EcuRaid_t)pEcuCfg->param[0];
            break;
        default:
            E_PRINTF(cEcEcuInvalidCfn, "Couldn't find ECU configuration function number (%d)\n", pEcuCfg->cfn);
            return cEcEcuInvalidCfn;
    }

    if (Ecu_SetAuFormat() == cEcEcuInvalidCfn)
    {
        return cEcEcuInvalidCfn;
    }

    Ecu_ConfigureIdec(MAX_ITR_NUM_EC0_IDEC0,MAX_ITR_NUM_EC0_IDEC1,MAX_ITR_NUM_EC1_IDEC0,MAX_ITR_NUM_EC1_IDEC1, IS_1BIT_DEC, METHOD, TYPE, OPTION);
    Ecu_ConfigureIdec(MAX_ITR_NUM_EC0_IDEC0,MAX_ITR_NUM_EC0_IDEC1,MAX_ITR_NUM_EC1_IDEC0,MAX_ITR_NUM_EC1_IDEC1, IS_2BIT_DEC, METHOD, TYPE, OPTION);

    return cEcNoError;
}
//-----------------------------------------------------------------------------
/**
    Reset ECU.

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Ecu_Reset(void)
{
    rEcu.ecuCtrlAndCfg1.b.ECU_SOFT_RESET = 1;
    rEcu.ecuCtrlAndCfg1.b.ECU_SOFT_RESET = 0;

    return cEcNoError;
}
//-----------------------------------------------------------------------------
/**
    Get AU format pointer.\n

    @return AU format

**/
//-----------------------------------------------------------------------------
static uint8_t Ecu_GetAuFormatPointer(void)
{
    return (uint8_t)(rEcu.ecuAuFmtCfgPtr.b.AU_FORMAT_CONF_PTR);
}

//-----------------------------------------------------------------------------
/**
    Set (Switch) AU format pointer

    @input[in] auFormat                  a pointer to AU_FORMAT

    @return encoded AU in bytes

**/
//-----------------------------------------------------------------------------
static void Ecu_SetAuFormatPointer(uint8_t auFormat)
{
    tSEQ_CFG seqCfg;
    EcuAuFormatInfoTbl_t *pAuFormat = (EcuAuFormatInfoTbl_t *)&gEcuAuFormatList[auFormat];

    gEcuInfo.auFormat = auFormat;
    mEcu.ecuAuFmtCfgPtr.b.AU_FORMAT_CONF_PTR = auFormat;

    if(pAuFormat->auxMetaDataMode == SEQ_DATA_MODE_AUX)
    {
        seqCfg.cfn = SEQ_CFN_AUX_SIZE;
        seqCfg.param[0] = pAuFormat->auxSize / 4;
        HalSeq_Configure(&seqCfg);
    }
}



//-----------------------------------------------------------------------------
/**
    Enable/Disable Decoder Error Count feature.\n

    @param[in] enDisable    enable or disable
**/
//-----------------------------------------------------------------------------
static void Ecu_EnDisableDecoderErrorCount(bool enDisable)
{
    volatile EcCore0iteratiInlineDec0ErrCtrl_t *pDecErrCtrl;
    EcuEccCore_t core;
    EcuDecType_t type;

    for (core = ECU_ECC_CORE0; core < ECU_ECC_CORE_END; core++)
    {
        for (type = ECU_IDEC0; type < ECU_RDEC; type++)
        {
            pDecErrCtrl = (EcCore0iteratiInlineDec0ErrCtrl_t *)((uint32_t)&rEcu.ecCore0iteratiInlineDec0ErrCtrl.all + (0x200 * core) + (0x20 * type));

            pDecErrCtrl->b.ECC0_ITR_IDEC0_ERR_CNT_EN = enDisable;
        }
    }
}

//=============================================================================
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
    Calculate encoded AU bytes

    @input[in]                      AU_FORMAT pointer

    @return encoded AU in bytes

**/
//-----------------------------------------------------------------------------
static uint32_t Ecu_CalEncodedAuBytes(EcuAuFormatInfoTbl_t *pAuFormat)
{
    uint32_t hlbaSizeLeftInAu = 0;
    uint32_t bcrcBytes = 0;
    uint32_t userDataSize = 0;
    uint32_t auxBytes = 0;
    uint32_t metaBytes = 0;
    uint32_t metaEccBytes = 0;
    uint32_t metaMcrcBytes = 0;
    uint32_t itrParityBytes = 0;
    uint32_t auMcrcBytes = 0;
    uint32_t auEccBytes = 0;
    uint32_t totalParityBytes = 0;
    uint32_t xferCountByte = 0;
    uint32_t roundedXferCount = 0;
    uint32_t comprAuHeaderBytes = COMPR_EN;          ///< REVISIT!!

    switch (pAuFormat->ecuHlbaStripMode)
    {
        case ECU_HLBA_STRIP_NO:
            hlbaSizeLeftInAu = (pAuFormat->ecuHlbaMode * pAuFormat->ecu2HostRatio);
            break;
        case ECU_HLBA_STRIP_ONE:
            hlbaSizeLeftInAu = (rSeq.seqCfg3.b.TWO_LBA_EN + 1) * pAuFormat->ecuHlbaMode;
            break;
        case ECU_HLBA_STRIP_ALL:
            hlbaSizeLeftInAu = 0;
            break;
        default:
            D_PRINTF("No supported HLBA strip mode!\n");
            break;
    }

    userDataSize = (pAuFormat->hostSectorSize * pAuFormat->ecu2HostRatio);
    bcrcBytes    = (pAuFormat->bcrcEn * pAuFormat->ecu2HostRatio);
    if (pAuFormat->auxMetaDataMode == SEQ_DATA_MODE_META)
    {
        metaBytes = pAuFormat->metaSize;
    }
    else
    {
        auxBytes  = pAuFormat->auxSize;
    }

    metaEccBytes = (pAuFormat->metaEccEn * CEIL(rEcu.ecuCfg3.b.META_T * 12.0/8.0));
    metaMcrcBytes    = (4 * pAuFormat->metaMcrcEn);
    //itrParityBytes   = rEcu.ecuCtrlAndCfg5And6.b.ECC_LDPC_DIS ? 0: gLdpcSchManager[pAuFormat->eccItrCodeSel].nParitySize;
    itrParityBytes   = pAuFormat->eccLdpcDis ? 0: gLdpcSchManager[pAuFormat->eccItrCodeSel].nParitySize;
    auMcrcBytes      = pAuFormat->auMcrcEn * ((2 * rEcu.ecuCfg1.b.AU_MCRC_SIZE_OPT) + 4);

    auEccBytes       = pAuFormat->cwPerAu * itrParityBytes;
    totalParityBytes = metaEccBytes + auEccBytes + metaMcrcBytes + auMcrcBytes;
    xferCountByte    = userDataSize + bcrcBytes + hlbaSizeLeftInAu + auxBytes + metaBytes + comprAuHeaderBytes + totalParityBytes;
    roundedXferCount = xferCountByte + (xferCountByte % 2);

    gCwInfo.cw1size = (((META_DATA_SIZE + AU_SIZE + MCRC_SIZE) / 0x8) % ECU_CW_PER_AU) ? ((META_DATA_SIZE + AU_SIZE + MCRC_SIZE) / 0x8 / ECU_CW_PER_AU) + 1 : ((META_DATA_SIZE + AU_SIZE + MCRC_SIZE) / 0x8 / ECU_CW_PER_AU) ;
    gCwInfo.cw1size = 8 * gCwInfo.cw1size+itrParityBytes;
    gCwInfo.cw2size = ((META_DATA_SIZE + AU_SIZE + MCRC_SIZE) / 0x8 / ECU_CW_PER_AU) ;
    gCwInfo.cw2size = 8 * gCwInfo.cw2size+itrParityBytes + ((META_DATA_SIZE + AU_SIZE + MCRC_SIZE) % 0x8);

    if (gEcuInfo.physicalPageSizePerAu < roundedXferCount)
    {
        E_PRINTF(cEcEcuXferError, "Couldn't support transfer size %d bytes(>%d)", roundedXferCount, gEcuInfo.physicalPageSizePerAu);
        return cEcEcuXferError;
    }

    pAuFormat->xferSizePerAu = roundedXferCount;
    gEcuInfo.xferSizePerAu   = roundedXferCount;

    return roundedXferCount;
}

///-----------------------------------------------------------------------------
/**
    Set ECU AU Format

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Ecu_SetAuFormat(void)
{
    uint32_t index;
    uint32_t powerIndex;

    EcuAuFormatInfoTbl_t *pAuFormat = NULL;

    for (index = 0; index < cNumberOfAuFormats; index++)
    {
        rEcu.ecuAuFmtCfgPtr.b.AU_FORMAT_CONF_PTR = index;     ///< select AU Format index: this should be placed first

        pAuFormat = (EcuAuFormatInfoTbl_t *)&gEcuAuFormatList[index];

        if (pAuFormat->phyPageSize == 0)
        {
            break;
        }
        pAuFormat->phyPageSize = gEcuInfo.physicalPageSizePerAu;

        switch(index)
        {
            case AU_FORMAT_LONG_MODE:
            case AU_FORMAT_META_ONLY:
            {
                break;
            }
            default:
            {
                pAuFormat->xferSizePerAu = Ecu_CalEncodedAuBytes(pAuFormat);
                break;
            }
        }
        rEcu.ecuAuFmtCfgPtr.b.AU_FORMAT_CONF_PTR        = index;     ///< select AU Format index: this should be placed first
        rEcu.ecuCtrlAndCfg5And6.b.FIT_2_PHY_PAGE        = pAuFormat->fit2PhyPage;               ///< [ 4]
        rEcu.ecuCtrlAndCfg5And6.b.ECC_INV_MODE          = pAuFormat->eccInvMode;                ///< [ 5]
        rEcu.ecuCtrlAndCfg5And6.b.MCRC_INV_MODE         = pAuFormat->mcrcInvMode;               ///< [ 6]
        rEcu.ecuCtrlAndCfg5And6.b.META_ECC_EN           = pAuFormat->metaEccEn;                 ///< [ 7]
        rEcu.ecuCtrlAndCfg5And6.b.ECC_LDPC_DIS          = pAuFormat->eccLdpcDis;                ///< [ 8]
        rEcu.ecuCtrlAndCfg5And6.b.ECC_ITR_BYPASS_EN     = pAuFormat->eccDecBypassEn;            ///< [ 9]
        rEcu.ecuCtrlAndCfg5And6.b.META_BCH_BYPASS_EN    = pAuFormat->metaBchBypassEn;           ///< [10]
        rEcu.ecuCtrlAndCfg5And6.b.ECC_ITR_CODE_SEL      = pAuFormat->eccItrCodeSel;             ///< [11]
        rEcu.ecuCtrlAndCfg7.b.ECU_HLBA_MODE             = pAuFormat->ecuHlbaMode;               ///< [13]
        rEcu.ecuCtrlAndCfg7.b.ECU_HLBA_STRIP_MODE       = pAuFormat->ecuHlbaStripMode;          ///< [14]
        rEcu.ecuCtrlAndCfg7.b.ECU_HLBA_INSERT_OPTION    = pAuFormat->ecuHlbaInsertOption;       ///< [15]
        rEcu.ecuCtrlAndCfg7.b.BCRC_EN                   = pAuFormat->bcrcEn;                    ///< [17]
        rEcu.ecuCtrlAndCfg8.b.HOST_SECTOR_SIZE          = pAuFormat->hostSectorSize;            ///< [18]
        rEcu.ecuCtrlAndCfg9.b.ECU2HOST_RATIO            = pAuFormat->ecu2HostRatio;             ///< [19]
        rEcu.ecuCtrlAndCfg10.b.META_SIZE                = pAuFormat->metaSize;                  ///< [20]
        rEcu.ecuCtrlAndCfg13.b.SCR_META_INV_EN          = pAuFormat->scrMetaInvEn;              ///< [22]
        rEcu.ecuCtrlAndCfg13.b.SCR_EN                   = pAuFormat->scrUserEn;                 ///< [23]
        rEcu.ecuCtrlAndCfg13.b.SCR_SEED_SRC             = pAuFormat->scrUserSeedSrc;            ///< [24]
        rEcu.ecuCfg1.b.CW_PER_AU                        = pAuFormat->cwPerAu;                   ///< [25]
        rEcu.ecuCfg1.b.AU_MCRC_EN                       = pAuFormat->auMcrcEn;                  ///< [26]
        rEcu.ecuCfg2.b.PHY_PAGE_SIZE                    = pAuFormat->phyPageSize;               ///< [27]
        rEcu.ecuCfg3.b.META_T                           = pAuFormat->metaPower;                 ///< [28]
        rEcu.ecuCfg3.b.META_THRSHD                      = pAuFormat->metaThreshold;             ///< [29]
        rEcu.ecuCfg3.b.META_MCRC_EN                     = pAuFormat->metaMcrcEn;                ///< [30]
        rEcu.ecuCfg5.b.THRSHD                           = pAuFormat->threshold;                 ///< [31]
        rEcu.ecuCfg6.b.ERASE_THRSHD                     = pAuFormat->eraseThreshold;            ///< [32]
    }

    rEcu.ecuAuFmtCfgPtr.b.AU_FORMAT_CONF_PTR = 0;       ///< set to 0 as default

    return cEcNoError;
}

///-----------------------------------------------------------------------------
/**
    Set EnDisableSendUncorrToDdr

    @return error code
**/
//-----------------------------------------------------------------------------
static void Ecu_EnDisableSendUncorrToDdr(bool enDisable)
{
    rEcu.ecuCtrlAndCfg4.b.SEND_UNCORR_TODDR = (enDisable) ? 1 : 0;
}

//-----------------------------------------------------------------------------
/**
    Configure IDEC.\n
    Decide decoding mode among 1bit and 2bit decoding mode.\n

    @param[in]  nMaxItrNumber  Set Maximum iteration number.(0,...,32)

    @param[in]  nIs2bitDec     selection bit for 1-bit or 2-bit IDEC (0h-1bit, 1-2bit)

    @param[in]  nMethod        update reliability information or not
                               (0 - Method 2,  1 - Method 1)
                               LDPC_INLINE_METHOD1 or LDPC_INLINE_METHOD2
                               Method1: Reliability info(bits) won't be updated.
                               Method2: Reliability info(bits) can be updated
                                        to achieve a better decoding performance.

    @param[in]  nType          NAND I/F supporting soft information
                               (1: Type0, 4: Type1, 6: Type2)

                               Type 0: NAND device can provide 2-bit soft
                                       information simultaneously.
                               Type 1: NAND device can only provide 2-bit soft
                                       information one by one.
                               Type 2: NAND device can't provide 2-bit soft information.

    @param[in]  nOption        2-bit 2 AU mode option
                               Option 1: process one 2-bit AU at a time.
                               Option 2: process two 2-bit AUs at the same time.

**/
//-----------------------------------------------------------------------------
static void Ecu_ConfigureIdec(uint8_t nMaxItrNumber_ec0_idec0, uint8_t nMaxItrNumber_ec0_idec1, uint8_t nMaxItrNumber_ec1_idec0,uint8_t nMaxItrNumber_ec1_idec1, uint8_t nIs2bitDec, uint8_t nMethod, uint8_t nType, uint8_t nOption)
{
    rEcu.ecuCtrlAndCfg1.b.ECC_REG_PARA_WR = 0 ; // When set 0, Do Not syn core0 core 1
    //2B-IDX
    rEcu.ecCore0iteratiInlineDec0ErrCtrl.b.ECC0_ITR_IDEC0_2B_IDX = nIs2bitDec;
    rEcu.ecCore0iteratiInlineDec1ErrCtrl.b.ECC0_ITR_IDEC1_2B_IDX = nIs2bitDec;
    //Early Termination
    rEcu.ecCore0ItrInlineDecoder0CtrlAnd.b.ECC0_ITR_IDEC0_EARLY_TERM_EN_0=1;
    rEcu.ecCore0ItrInlineDecoder1CtrlAnd.b.ECC0_ITR_IDEC1_EARLY_TERM_EN_0=1;
    //Max Iter Num
    rEcu.ecCore0ItrInlineDecoder0CtrlAnd.b.ECC0_ITR_IDEC0_MAX_NUM_ITER_0 = nMaxItrNumber_ec0_idec0; //default = 0x14;(R1C00C844_0[5:0])
    rEcu.ecCore0ItrInlineDecoder1CtrlAnd.b.ECC0_ITR_IDEC1_MAX_NUM_ITER_0 = nMaxItrNumber_ec0_idec1; //default = 0x14;(R1C00C844_0[5:0])
    //***Error Cnt Enable
    rEcu.ecCore0iteratiInlineDec0ErrCtrl.b.ECC0_ITR_IDEC0_ERR_CNT_EN=1;
    rEcu.ecCore0iteratiInlineDec1ErrCtrl.b.ECC0_ITR_IDEC1_ERR_CNT_EN=1;

     //2B-IDX
    rEcu.ecCore1iteratiInlineDec0ErrCtrl.b.ECC1_ITR_IDEC0_2B_IDX = nIs2bitDec;
    rEcu.ecCore1iteratiInlineDec1ErrCtrl.b.ECC1_ITR_IDEC1_2B_IDX = nIs2bitDec;
     //Early Termination
    rEcu.ecCore1ItrInlineDecoder0CtrlAnd.b.ECC1_ITR_IDEC0_EARLY_TERM_EN_0=1;
    rEcu.ecCore1ItrInlineDecoder1CtrlAnd.b.ECC1_ITR_IDEC1_EARLY_TERM_EN_0=1;
     //Max Iter Num
    rEcu.ecCore1ItrInlineDecoder0CtrlAnd.b.ECC1_ITR_IDEC0_MAX_NUM_ITER_0 = nMaxItrNumber_ec1_idec0; //default = 0x14;(R1C00C844_0[5:0])
    rEcu.ecCore1ItrInlineDecoder1CtrlAnd.b.ECC1_ITR_IDEC1_MAX_NUM_ITER_0 = nMaxItrNumber_ec1_idec1; //default = 0x14;(R1C00C844_0[5:0])
     //***Error Cnt Enable
    rEcu.ecCore1iteratiInlineDec0ErrCtrl.b.ECC1_ITR_IDEC0_ERR_CNT_EN=1;
    rEcu.ecCore1iteratiInlineDec1ErrCtrl.b.ECC1_ITR_IDEC1_ERR_CNT_EN=1;

    #if 0
    D_PRINTF("after setting Direct Read Early Term Register CH01 %d \n", rEcu.ecCore0ItrInlineDecoder0CtrlAnd.b.ECC0_ITR_IDEC0_EARLY_TERM_EN_0);
    D_PRINTF("after setting Direct Read Early Term Register CH23 %d \n", rEcu.ecCore0ItrInlineDecoder1CtrlAnd.b.ECC0_ITR_IDEC1_EARLY_TERM_EN_0);
    D_PRINTF("after setting Direct Read Early Term Register CH45 %d \n", rEcu.ecCore1ItrInlineDecoder0CtrlAnd.b.ECC1_ITR_IDEC0_EARLY_TERM_EN_0);
    D_PRINTF("after setting Direct Read Early Term Register CH67 %d \n", rEcu.ecCore1ItrInlineDecoder1CtrlAnd.b.ECC1_ITR_IDEC1_EARLY_TERM_EN_0);

    D_PRINTF("Error Cnt Enable CH01 %d \n", rEcu.ecCore0iteratiInlineDec0ErrCtrl.b.ECC0_ITR_IDEC0_ERR_CNT_EN);
    D_PRINTF("Error Cnt Enable CH23 %d \n",   rEcu.ecCore0iteratiInlineDec1ErrCtrl.b.ECC0_ITR_IDEC1_ERR_CNT_EN);
    D_PRINTF("Error Cnt Enable CH45 %d \n",   rEcu.ecCore1iteratiInlineDec0ErrCtrl.b.ECC1_ITR_IDEC0_ERR_CNT_EN);
    D_PRINTF("Error Cnt Enable CH67 %d \n",   rEcu.ecCore1iteratiInlineDec1ErrCtrl.b.ECC1_ITR_IDEC1_ERR_CNT_EN);

    // HalEcu_Set1BitOr2BitDecoder(nIs2bitDec);

    D_PRINTF("after2 setting Direct Read MAX_NUM_ITER Register CH01 %d \n", rEcu.ecCore0ItrInlineDecoder0CtrlAnd.b.ECC0_ITR_IDEC0_MAX_NUM_ITER_0);
    D_PRINTF("after2 setting Direct Read MAX_NUM_ITER Register CH23 %d \n", rEcu.ecCore0ItrInlineDecoder1CtrlAnd.b.ECC0_ITR_IDEC1_MAX_NUM_ITER_0);
    D_PRINTF("after2 setting Direct Read MAX_NUM_ITER Register CH45 %d \n", rEcu.ecCore1ItrInlineDecoder0CtrlAnd.b.ECC1_ITR_IDEC0_MAX_NUM_ITER_0);
    D_PRINTF("after2 setting Direct Read MAX_NUM_ITER Register CH67 %d \n", rEcu.ecCore1ItrInlineDecoder1CtrlAnd.b.ECC1_ITR_IDEC1_MAX_NUM_ITER_0);
    #endif
}

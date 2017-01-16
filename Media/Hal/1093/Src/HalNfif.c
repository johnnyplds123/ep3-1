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
//! @brief NAND flash interface Highlevel driver codes.
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"

#include "MemIo.h"
#include "ClockTree.h"
#include "HalTimer.h"
#include "HalClock.h"
#include "HalEcu.h"
#include "HalRegConfig.h"
#include "RegNandFlashIf.h"
#include "HalGpio.h"
#include "SysInfo.h"
#include "HalNfif.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Temporary definitions for code merging - will be replaced all
//-----------------------------------------------------------------------------
#define NFIF_DEBUG_PRINT 0//1
#define OPEN_DATA_ODT    FALSE

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define cMaxDivider              18

enum{
    TM_CTRL1 = 0,
    TM_CTRL2,
    TM_CTRL3,
    TM_CTRL4,
    TM_CTRL6,
    TM_CTRL7,
    TM_CTRL_COUNT
};

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void     NFIF_PowerManage(bool enable);
static void     NFIF_EnablePdCh(bool enable);
static void     NFIF_EnablePwrDownCh(bool enable);
static void     NFIF_ResetNandPhy(void);
static void     NFIF_SetlDriveSelect(uint8_t value);
static void     NFIF_SetNandPhyOdt(NfPhyInfo_t *pPhyInfo);
static void     NFIF_SetFlashClock(NfIfData_t ifData, uint8_t xferMode);
static void     NFIF_SetExtCeEn(uint8_t value);
static void     NFIF_SetCeMode(tCE_MODE ceMode);
static Error_t  NFIF_UpdateFlashDll(NfPhyInfo_t *pPhyInfo);
static uint32_t NFIF_SetNfifClockFrequency(uint32_t freqMhz);

//Direct Mode
static void     NFIF_DmSetAddrDataPort(uint16_t word);
static uint16_t NFIF_DmGetAddrDataPort(void);

// Indirect Mode
static void     NFIF_ImSetDeviceChannel(Pna_t* pAddr);
static void     NFIF_ImResetFifo(void);
static void     NFIF_ImWriteDataU32(uint32_t dword);
static void     NFIF_SetClkSel(uint8_t value);

static void     NFIF_SetNandPhyZpZn(uint16_t zp, uint16_t zn);
static void     NFIF_ResetNandDll(void);
static void     NFIF_InitialPadPullUpDown(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
#if OPEN_DATA_ODT
const NfPhyInfo_t gNfifPhyInfoList = {0, 0, 0, {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1}, {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}, {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}, {0, 0, 0, 0, 0} };
#else
const NfPhyInfo_t gNfifPhyInfoList = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}, {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}, {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}, {0, 0, 0, 0, 0} };
#endif

const TMCtrlInfo_t gTMCtrlTbl[TM_CTRL_COUNT] =
{
    {0x0D,  0x2, (0x3F - 0x0D) / TT_ADAPTIVE_LEVEL, 0}, // Timing Control 1
    {0x10,  0x1, (0x3F - 0x10) / TT_ADAPTIVE_LEVEL, 0}, // Timing Control 2
    {0x17,  0x2, (0x3F - 0x17) / TT_ADAPTIVE_LEVEL, 0}, // Timing Control 3
    {0x0D,  0x2, (0x3F - 0x0D) / TT_ADAPTIVE_LEVEL, 0}, // Timing Control 4
    {0x0D,  0x1, (0x3F - 0x0D) / TT_ADAPTIVE_LEVEL, 0}, // Timing Control 6
    {0x02,  0x2, (0x3F - 0x02) / TT_ADAPTIVE_LEVEL, 0}, // Timing Control 7
};

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern Error_t HalNfCtrl_OneTimeInit(void);
extern Error_t HalNfCtrl_UpdateTimingInfo(NfCfgInfo_t* pCfgInfo);
extern Error_t HalNfCtrl_EraseBlockIm(Pna_t* pAddr);
extern Error_t HalNfCtrl_WritePageIm(Pna_t* pAddr, void* pBuffer, uint32_t count);
extern Error_t HalNfCtrl_WritePageDm(Pna_t* pAddr, void* pBuffer, uint32_t count, uint8_t prefix);
extern Error_t HalNfCtrl_ReadPageIm(Pna_t* pAddr, void* pBuffer, uint32_t count);
extern Error_t HalNfCtrl_ReadPageDm(Pna_t* pAddr, void* pBuffer, uint32_t count, uint8_t prefix);
extern Error_t HalNfCtrl_ResetDm(Pna_t* pAddr, uint8_t resetCmd);
extern Error_t HalNfCtrl_ResetIm(Pna_t* pAddr, uint8_t resetCmd);
extern uint8_t HalNfCtrl_ReadStatusIm(Pna_t* pAddr);
extern uint8_t HalNfCtrl_ReadStatusDm(Pna_t* pAddr);
extern Error_t HalNfCtrl_SetFeatureIm(Pna_t* pAddr, uint8_t param[]);
extern Error_t HalNfCtrl_SetFeatureDm(Pna_t* pAddr, uint8_t param[]);
extern Error_t HalNfCtrl_GetFeatureIm(Pna_t* pAddr, uint8_t param[]);
extern Error_t HalNfCtrl_GetFeatureDm(Pna_t* pAddr, uint8_t param[]);
extern Error_t HalNfCtrl_ReadParameterPageIm(Pna_t* pAddr, void* pBuffer, uint32_t count);
extern Error_t HalNfCtrl_ReadParameterPageDm(Pna_t* pAddr, void* pBuffer, uint32_t count);
extern Error_t HalNfCtrl_ReadIdIm(Pna_t* pAddr, void* pBuffer, uint32_t count);
extern Error_t HalNfCtrl_ReadIdDm(Pna_t* pAddr, void* pBuffer, uint32_t count);
extern Error_t HalNfCtrl_ReadIdTm(Pna_t* pAddr, void* pBuffer, uint32_t count);
extern Error_t HalNfCtrl_WaitForStatusSetIm(Pna_t* pAddr, uint32_t value, uint32_t timeOutUsec);
extern Error_t HalNfCtrl_WaitForStatusSetDm(Pna_t* pAddr, uint32_t value, uint32_t timeOutUsec);
extern Error_t HalNfCtrl_GetVoltageOffsetDm(Pna_t *pAddr, uint8_t featureAddr, uint8_t param[]);
extern Error_t HalNfCtrl_SetVoltageOffsetDm(Pna_t *pAddr, uint8_t featureAddr, uint8_t param[]);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    One time Initialization of NFIF HW.\n

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_OneTimeInit(InitBootMode_t initMode)
{
    NfPhyInfo_t *pPhyInfo = (NfPhyInfo_t *)&gNfifPhyInfoList;

    gCfgInfo.socIfData                             = NF_JEDEC_TOGGLE2;

    // Initialize NAND PHY
    NFIF_PowerManage(FALSE);     //< All channel power on
    NFIF_ResetNandPhy();
    NFIF_ResetNandDll();                 ///< Reset the DLL and wait for lock
    NFIF_InitialPadPullUpDown();         ///< Initialize NAND PHY Pad pull-up/down setting

    NFIF_SetlDriveSelect(cDriverValue);
    NFIF_SetNandPhyZpZn(cInitNandPhyPadSelect, cInitNandPhyPadSelect);
    NFIF_SetNandPhyOdt(pPhyInfo);

    // Reset Flash Interface logic
    rNandFlashIf.flashInterfacCfg.b.IF_RESET        = 1;
    rNandFlashIf.flashInterfacCfg.b.IF_RESET        = 0;

    mNandFlashIf.flashInterfacCfg.b.CLK_PD_EN       = 1;    ///< Enable Clock Signal Power Down mode
    mNandFlashIf.flashInterfacCfg.b.PD_EN           = 1;    ///< Enable Power Down mode
    mNandFlashIf.flashSeqChanEngineClkCtrl1.all     = 0;    ///< Disable use of eight CEs per channel

    // Enable DQS Delay circuit (low speed)
    mNandFlashIf.dllCtrl1ForChan0.b.DELAY_TEST_CH0  = 50;
    mNandFlashIf.dllCtrl1ForChan1.b.DELAY_TEST_CH1  = 50;
    mNandFlashIf.dllCtrl1ForChan2.b.DELAY_TEST_CH2  = 50;
    mNandFlashIf.dllCtrl1ForChan3.b.DELAY_TEST_CH3  = 50;
    mNandFlashIf.dllCtrl1ForChan4.b.DELAY_TEST_CH4  = 50;
    mNandFlashIf.dllCtrl1ForChan5.b.DELAY_TEST_CH5  = 50;
    mNandFlashIf.dllCtrl1ForChan6.b.DELAY_TEST_CH6  = 50;
    mNandFlashIf.dllCtrl1ForChan7.b.DELAY_TEST_CH7  = 50;
    mNandFlashIf.dllCtrl1ForChan0.b.GAIN2X_CH0      = 1;
    mNandFlashIf.dllCtrl1ForChan1.b.GAIN2X_CH1      = 1;
    mNandFlashIf.dllCtrl1ForChan2.b.GAIN2X_CH2      = 1;
    mNandFlashIf.dllCtrl1ForChan3.b.GAIN2X_CH3      = 1;
    mNandFlashIf.dllCtrl1ForChan4.b.GAIN2X_CH4      = 1;
    mNandFlashIf.dllCtrl1ForChan5.b.GAIN2X_CH5      = 1;
    mNandFlashIf.dllCtrl1ForChan6.b.GAIN2X_CH6      = 1;
    mNandFlashIf.dllCtrl1ForChan7.b.GAIN2X_CH7      = 1;
    mNandFlashIf.dllCtrl1ForChan0.b.BYPASS_EN_CH0   = 1;
    mNandFlashIf.dllCtrl1ForChan1.b.BYPASS_EN_CH1   = 1;
    mNandFlashIf.dllCtrl1ForChan2.b.BYPASS_EN_CH2   = 1;
    mNandFlashIf.dllCtrl1ForChan3.b.BYPASS_EN_CH3   = 1;
    mNandFlashIf.dllCtrl1ForChan4.b.BYPASS_EN_CH4   = 1;
    mNandFlashIf.dllCtrl1ForChan5.b.BYPASS_EN_CH5   = 1;
    mNandFlashIf.dllCtrl1ForChan6.b.BYPASS_EN_CH6   = 1;
    mNandFlashIf.dllCtrl1ForChan7.b.BYPASS_EN_CH7   = 1;

    mNandFlashIf.vrefRcvTypeSelectio.b.DQS_VREF_SEL = 0;    // default - don't care
    mNandFlashIf.vrefRcvTypeSelectio.b.REB_VREF_SEL = 0;    // default - don't care
    mNandFlashIf.vrefRcvTypeSelectio.b.WEB_VREF_SEL = 0;    // default - don't care
    mNandFlashIf.vrefRcvTypeSelectio.b.DQ_RCVTYPE   = 1;    // Single end enable
    mNandFlashIf.vrefRcvTypeSelectio.b.DQS_RCVTYPE  = 1;    // Single end enable
    mNandFlashIf.vrefRcvTypeSelectio.b.CMD_RCVTYPE  = 1;    // Single end enable
    mNandFlashIf.vrefRcvTypeSelectio.b.REB_RCVTYPE  = 1;    // Single end enable
    mNandFlashIf.vrefRcvTypeSelectio.b.WEB_RCVTYPE  = 1;    // Single end enable

    mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.all           = 0x0FFF;

    mNandFlashIf.flashSeqCtrl2.all                  = 0x80A1;

    mNandFlashIf.hprjCfg.b.HPRJ_JOB_PROC_NUM        = 4;

    //mNandFlashIf.efFifoClkCtrlPort.all              = 0x0348;

    mNandFlashIf.hiddenCfg1.all                     = 0x0045;

    *(uint16_t *)(0xF200E0EE) = (1 << 5);   ///< 0xF200E0EE[5]: G_ABORT_ER_SEL (Graceful Abort Erase Handling Select)

    HalNfif_SetDataIf(NF_ASYNC);
    HalNfif_DmBackToDefault();

    NFIF_SetCeMode(CE_MODE0);

    //HalFlexible_OneTimeInit();

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Configure HW Clock and Timing

    @param[in] pNfInfo   NF information.

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_Configure(NfCfgInfo_t* pCfgInfo)
{
    uint8_t lunBit;

    // READ
    rNandFlashIf.flashSeqCmdPort1.b.RBYTE0          = FC_READ_BYTE0;
    rNandFlashIf.flashSeqCmdPort1.b.RBYTE1          = FC_READ_BYTE1;
    rNandFlashIf.flashSeqCmdPort2.b.RBYTE2          = FC_READ_BYTE2;
    rNandFlashIf.flashSeqFlashSeqCmdPort7.b.RBYTE3  = (pCfgInfo->ifType == NF_ONFI) ? FC_READ_BYTE3_ONFI : FC_READ_BYTE3_JEDEC;
    rNandFlashIf.flashSeqFlashSeqCmdPort7.b.RBYTE4  = (pCfgInfo->ifType == NF_ONFI) ? FC_READ_BYTE4_ONFI : FC_READ_BYTE4_JEDEC;
    rNandFlashIf.flashSeqCmdPort8.b.RBYTE5          = FC_READ_BYTE5;
    rNandFlashIf.flashSeqCmdPort8.b.RBYTE6          = FC_READ_BYTE6;
    rNandFlashIf.flashSeqCmdPort9.b.RBYTE7          = FC_READ_BYTE7_ONFI;
    rNandFlashIf.flashSeqCmdPort9.b.RBYTE8          = FC_READ_BYTE8;
    // PROG
    rNandFlashIf.flashSeqCmdPort3.b.PBYTE0          = FC_PROG_BYTE0;
    rNandFlashIf.flashSeqCmdPort3.b.PBYTE1          = FC_PROG_BYTE1;
    rNandFlashIf.flashSeqCmdPort10.b.PBYTE2         = FC_PROG_BYTE2;
    rNandFlashIf.flashSeqCmdPort10.b.PBYTE3         = (pCfgInfo->ifType == NF_ONFI) ? FC_PROG_BYTE3_ONFI : FC_PROG_BYTE3_JEDEC;
    rNandFlashIf.flashSeqCmdPort11.b.PBYTE4         = FC_PROG_BYTE4;
    // ERASE
    rNandFlashIf.flashSeqCmdPort4.b.EBYTE0          = FC_ERASE_BYTE0;
    rNandFlashIf.flashSeqCmdPort4.b.EBYTE1          = FC_ERASE_BYTE1;
    rNandFlashIf.flashSeqCmdPort11.b.EBYTE2         = FC_ERASE_BYTE2_ONFI;
    // READ STATUS
    rNandFlashIf.flashSeqCtrl1.b.FAIL_BIT0          = FC_STATUS_FAIL_BIT0;
    rNandFlashIf.flashSeqCtrl1.b.READY_BIT0         = FC_STATUS_READY_BIT0;
    rNandFlashIf.flashSeqCmdPort5.b.SBYTE0          = FC_READ_STATUS_BYTE0;
    // Regular Polling
    rNandFlashIf.flashSeqStatByteCtrl1.b.SBYTE1     = FC_READ_STATUS_BYTE1;
    rNandFlashIf.flashSeqStatByteCtrl1.b.FAIL_BIT1  = FC_STATUS_FAIL_BIT1;
    rNandFlashIf.flashSeqStatByteCtrl1.b.READY_BIT1 = FC_STATUS_READY_BIT1;

    rNandFlashIf.flashSeqStatByteCtrl2.b.SBYTE2     = FC_READ_STATUS_BYTE2;
    rNandFlashIf.flashSeqStatByteCtrl2.b.SBYTE3     = FC_READ_STATUS_BYTE3;
    rNandFlashIf.flashSeqStatByteCtrl3.b.FAIL_BIT2  = FC_STATUS_FAIL_BIT2;
    rNandFlashIf.flashSeqStatByteCtrl3.b.READY_BIT2 = FC_STATUS_READY_BIT2;
    rNandFlashIf.flashSeqStatByteCtrl3.b.FAIL_BIT3  = FC_STATUS_FAIL_BIT3;
    rNandFlashIf.flashSeqStatByteCtrl3.b.FAIL_BIT4  = FC_STATUS_FAIL_BIT4;
    rNandFlashIf.flashSeqStatByteCtrl4.b.FAIL_BIT5  = FC_STATUS_FAIL_BIT5;
    rNandFlashIf.flashSeqStatByteCtrl4.b.FAIL_BIT6  = FC_STATUS_FAIL_BIT6;

    // LUN information
    lunBit = (gNfInfo.lunsPerDevice == 1) ? 0x0F : (gNfInfo.lunLsbPosition - 16);
    mNandFlashIf.flashSeqChan0ToChan3Lun.b.LUN_CH0  = lunBit;
    mNandFlashIf.flashSeqChan0ToChan3Lun.b.LUN_CH1  = lunBit;
    mNandFlashIf.flashSeqChan0ToChan3Lun.b.LUN_CH2  = lunBit;
    mNandFlashIf.flashSeqChan0ToChan3Lun.b.LUN_CH3  = lunBit;

    mNandFlashIf.flashSeqChan4ToChan7Lun.b.LUN_CH4  = lunBit;
    mNandFlashIf.flashSeqChan4ToChan7Lun.b.LUN_CH5  = lunBit;
    mNandFlashIf.flashSeqChan4ToChan7Lun.b.LUN_CH6  = lunBit;
    mNandFlashIf.flashSeqChan4ToChan7Lun.b.LUN_CH7  = lunBit;

    NFIF_SetFlashClock(pCfgInfo->reqIfData, pCfgInfo->reqXferMode);

    if (pCfgInfo->updatePhyInfo == FALSE)
    {
        MEM_COPY(&pCfgInfo->phyInfo, &gNfifPhyInfoList, sizeof(pCfgInfo->phyInfo));
    }

    NFIF_ResetNandPhy();
    NFIF_SetNandPhyOdt(&pCfgInfo->phyInfo);

    if (pCfgInfo->updateTimingInfo == FALSE)
    {
        HalNfCtrl_UpdateTimingInfo(pCfgInfo);
    }

    if (pCfgInfo->updateTimingInfo & 0x2)
    {
        NFIF_SetNfifClockFrequency((pCfgInfo->updateTimingInfo>>4));
    }

    switch (pCfgInfo->reqIfData)
    {
        case NF_ASYNC:
        case NF_ONFI_SDR:
        {
            mNandFlashIf.flashInterfacTmCtrl1.b.T1_SEQ_ASYN1            = pCfgInfo->timingInfo.T1;
            mNandFlashIf.flashInterfacTmCtrl1.b.T2_SEQ_ASYN1            = pCfgInfo->timingInfo.T2;
            mNandFlashIf.flashInterfacTmCtrl1.b.T3_SEQ_ASYN1            = pCfgInfo->timingInfo.T3;
            mNandFlashIf.flashInterfacTmCtrl1.b.T4_SEQ_ASYN1            = pCfgInfo->timingInfo.T4;
            mNandFlashIf.flashInterfacTmCtrl1.b.T5_SEQ_ASYN1            = pCfgInfo->timingInfo.T5;
            mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.b.T1_IND_ASYN             = pCfgInfo->timingInfo.T1;
            mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.b.T2_IND_ASYN             = pCfgInfo->timingInfo.T2;
            mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.b.T3_IND_ASYN             = pCfgInfo->timingInfo.T3;
            mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.b.T4_IND_ASYN             = pCfgInfo->timingInfo.T4;
            mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.b.T5_IND_ASYN             = pCfgInfo->timingInfo.T5;
            mNandFlashIf.flashSeqInterfacTmChan0Cfg.b.TIMING_SEL_CH0    = 0;
            mNandFlashIf.flashSeqInterfacTmChan1Cfg.b.TIMING_SEL_CH1    = 0;
            mNandFlashIf.flashSeqInterfacTmChan2Cfg.b.TIMING_SEL_CH2    = 0;
            mNandFlashIf.flashSeqInterfacTmChan3Cfg.b.TIMING_SEL_CH3    = 0;
            mNandFlashIf.flashSeqInterfacTmChan4Cfg.b.TIMING_SEL_CH4    = 0;
            mNandFlashIf.flashSeqInterfacTmChan5Cfg.b.TIMING_SEL_CH5    = 0;
            mNandFlashIf.flashSeqInterfacTmChan6Cfg.b.TIMING_SEL_CH6    = 0;
            mNandFlashIf.flashSeqInterfacTmChan7Cfg.b.TIMING_SEL_CH7    = 0;

            mNandFlashIf.flashInterfacTmCtrl6.b.T8_SEQ                  = pCfgInfo->timingInfo.T8;
            mNandFlashIf.flashInterfacTmCtrl6.b.T9_SEQ                  = pCfgInfo->timingInfo.T9;
            mNandFlashIf.flashInterfacTmCtrl7.b.TCS                     = pCfgInfo->timingInfo.tCS;
            mNandFlashIf.flashInterfacTmCtrl9.b.TRHW                    = pCfgInfo->timingInfo.tRHW;
            mNandFlashIf.flashInterfacTmCtrl9.b.TWB                     = pCfgInfo->timingInfo.tWB;
            mNandFlashIf.vrefRcvTypeSelectio.b.DQS_VREF_SEL             = 0; // default - don't care
            mNandFlashIf.vrefRcvTypeSelectio.b.DQS_RCVTYPE              = 1; // Single end enable
            break;
        }
        case NF_ONFI_NVDDR:
        {
            mNandFlashIf.flashInterfacTmCtrl4Toggle1Toggle2.b.T1_SEQ_JDC              = pCfgInfo->timingInfo.T1;
            mNandFlashIf.flashInterfacTmCtrl4Toggle1Toggle2.b.T2_SEQ_JDC              = pCfgInfo->timingInfo.T2;
            mNandFlashIf.flashInterfacTmCtrl4Toggle1Toggle2.b.T3_SEQ_JDC              = pCfgInfo->timingInfo.T3;
            mNandFlashIf.flashInterfacTmCtrl4Toggle1Toggle2.b.T4_SEQ_JDC              = pCfgInfo->timingInfo.T4;
            mNandFlashIf.flashInterfacTmCtrl4Toggle1Toggle2.b.RF200E00A_15_12_RSVD    = pCfgInfo->timingInfo.T5;
            mNandFlashIf.dllCtrl1ForChan0.b.BYPASS_EN_CH0               = 1;
            mNandFlashIf.dllCtrl1ForChan1.b.BYPASS_EN_CH1               = 1;
            mNandFlashIf.dllCtrl1ForChan2.b.BYPASS_EN_CH2               = 1;
            mNandFlashIf.dllCtrl1ForChan3.b.BYPASS_EN_CH3               = 1;
            mNandFlashIf.dllCtrl1ForChan4.b.BYPASS_EN_CH4               = 1;
            mNandFlashIf.dllCtrl1ForChan5.b.BYPASS_EN_CH5               = 1;
            mNandFlashIf.dllCtrl1ForChan6.b.BYPASS_EN_CH6               = 1;
            mNandFlashIf.dllCtrl1ForChan7.b.BYPASS_EN_CH7               = 1;

            mNandFlashIf.dllCtrl1ForChan0.b.DELAY_TEST_CH0              = pCfgInfo->timingInfo.T7[0];
            mNandFlashIf.dllCtrl1ForChan1.b.DELAY_TEST_CH1              = pCfgInfo->timingInfo.T7[1];
            mNandFlashIf.dllCtrl1ForChan2.b.DELAY_TEST_CH2              = pCfgInfo->timingInfo.T7[2];
            mNandFlashIf.dllCtrl1ForChan3.b.DELAY_TEST_CH3              = pCfgInfo->timingInfo.T7[3];
            mNandFlashIf.dllCtrl1ForChan4.b.DELAY_TEST_CH4              = pCfgInfo->timingInfo.T7[4];
            mNandFlashIf.dllCtrl1ForChan5.b.DELAY_TEST_CH5              = pCfgInfo->timingInfo.T7[5];
            mNandFlashIf.dllCtrl1ForChan6.b.DELAY_TEST_CH6              = pCfgInfo->timingInfo.T7[6];
            mNandFlashIf.dllCtrl1ForChan7.b.DELAY_TEST_CH7              = pCfgInfo->timingInfo.T7[7];

            mNandFlashIf.dllCtrl1ForChan0.b.GAIN2X_CH0                  = 1;
            mNandFlashIf.dllCtrl1ForChan1.b.GAIN2X_CH1                  = 1;
            mNandFlashIf.dllCtrl1ForChan2.b.GAIN2X_CH2                  = 1;
            mNandFlashIf.dllCtrl1ForChan3.b.GAIN2X_CH3                  = 1;
            mNandFlashIf.dllCtrl1ForChan4.b.GAIN2X_CH4                  = 1;
            mNandFlashIf.dllCtrl1ForChan5.b.GAIN2X_CH5                  = 1;
            mNandFlashIf.dllCtrl1ForChan6.b.GAIN2X_CH6                  = 1;
            mNandFlashIf.dllCtrl1ForChan7.b.GAIN2X_CH7                  = 1;

            mNandFlashIf.flashInterfacTmCtrl6.b.T8_SEQ                  = pCfgInfo->timingInfo.T8;
            mNandFlashIf.flashInterfacTmCtrl7.b.TCS                     = pCfgInfo->timingInfo.tCS;
            mNandFlashIf.flashInterfacTmCtrl8.b.TWPST                   = pCfgInfo->timingInfo.tWPST;
            mNandFlashIf.flashInterfacTmCtrl9.b.TWPRE                   = pCfgInfo->timingInfo.tWPRE;
            mNandFlashIf.vrefRcvTypeSelectio.b.DQS_VREF_SEL             = 1; // Single ended
            mNandFlashIf.vrefRcvTypeSelectio.b.REB_VREF_SEL             = 1; // Single ended
            mNandFlashIf.vrefRcvTypeSelectio.b.DQS_RCVTYPE              = 1; // CMOS Receiver
            mNandFlashIf.vrefRcvTypeSelectio.b.REB_RCVTYPE              = 1; // COMS Receiver
            break;
        }
        case NF_JEDEC_TOGGLE1:
        {
            mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.b.T1_IND_ASYN  = pCfgInfo->timingInfo.T1;
            mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.b.T2_IND_ASYN  = pCfgInfo->timingInfo.T2;
            mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.b.T3_IND_ASYN  = pCfgInfo->timingInfo.T3;
            mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.b.T4_IND_ASYN  = pCfgInfo->timingInfo.T4;
            mNandFlashIf.flashInterfacTmCtrl4Toggle1Toggle2.b.T1_SEQ_JDC      = pCfgInfo->timingInfo.T1;
            mNandFlashIf.flashInterfacTmCtrl4Toggle1Toggle2.b.T2_SEQ_JDC      = pCfgInfo->timingInfo.T2;
            mNandFlashIf.flashInterfacTmCtrl4Toggle1Toggle2.b.T3_SEQ_JDC      = pCfgInfo->timingInfo.T3;
            mNandFlashIf.flashInterfacTmCtrl4Toggle1Toggle2.b.T4_SEQ_JDC      = pCfgInfo->timingInfo.T4;
            mNandFlashIf.dllCtrl1ForChan0.b.BYPASS_EN_CH0               = 1;
            mNandFlashIf.dllCtrl1ForChan1.b.BYPASS_EN_CH1               = 1;
            mNandFlashIf.dllCtrl1ForChan2.b.BYPASS_EN_CH2               = 1;
            mNandFlashIf.dllCtrl1ForChan3.b.BYPASS_EN_CH3               = 1;
            mNandFlashIf.dllCtrl1ForChan4.b.BYPASS_EN_CH4               = 1;
            mNandFlashIf.dllCtrl1ForChan5.b.BYPASS_EN_CH5               = 1;
            mNandFlashIf.dllCtrl1ForChan6.b.BYPASS_EN_CH6               = 1;
            mNandFlashIf.dllCtrl1ForChan7.b.BYPASS_EN_CH7               = 1;
            mNandFlashIf.dllCtrl1ForChan0.b.DELAY_TEST_CH0              = pCfgInfo->timingInfo.T7[0];
            mNandFlashIf.dllCtrl1ForChan1.b.DELAY_TEST_CH1              = pCfgInfo->timingInfo.T7[1];
            mNandFlashIf.dllCtrl1ForChan2.b.DELAY_TEST_CH2              = pCfgInfo->timingInfo.T7[2];
            mNandFlashIf.dllCtrl1ForChan3.b.DELAY_TEST_CH3              = pCfgInfo->timingInfo.T7[3];
            mNandFlashIf.dllCtrl1ForChan4.b.DELAY_TEST_CH4              = pCfgInfo->timingInfo.T7[4];
            mNandFlashIf.dllCtrl1ForChan5.b.DELAY_TEST_CH5              = pCfgInfo->timingInfo.T7[5];
            mNandFlashIf.dllCtrl1ForChan6.b.DELAY_TEST_CH6              = pCfgInfo->timingInfo.T7[6];
            mNandFlashIf.dllCtrl1ForChan7.b.DELAY_TEST_CH7              = pCfgInfo->timingInfo.T7[7];

            mNandFlashIf.flashInterfacTmCtrl6.b.T8_IND                  = pCfgInfo->timingInfo.T8;
            mNandFlashIf.flashInterfacTmCtrl6.b.T9_IND                  = pCfgInfo->timingInfo.T9;
            mNandFlashIf.flashInterfacTmCtrl6.b.T8_SEQ                  = pCfgInfo->timingInfo.T8;
            mNandFlashIf.flashInterfacTmCtrl6.b.T9_SEQ                  = pCfgInfo->timingInfo.T9;
            mNandFlashIf.flashInterfacTmCtrl7.b.TCS                     = pCfgInfo->timingInfo.tCS;
            mNandFlashIf.flashInterfacTmCtrl7.b.TCEH                    = pCfgInfo->timingInfo.tCEH;
            mNandFlashIf.flashInterfacTmCtrl7.b.TCWAW                   = pCfgInfo->timingInfo.tCWAW;
            mNandFlashIf.flashInterfacTmCtrl8.b.TWPST                   = pCfgInfo->timingInfo.tWPST;
            mNandFlashIf.flashInterfacTmCtrl8.b.TWPSTH                  = pCfgInfo->timingInfo.tWPSTH;
            mNandFlashIf.flashInterfacTmCtrl8.b.TRPST                   = pCfgInfo->timingInfo.tRPST;
            mNandFlashIf.flashInterfacTmCtrl8.b.TRPSTH                  = pCfgInfo->timingInfo.tRPSTH;
            mNandFlashIf.flashInterfacTmCtrl9.b.TRHW                    = pCfgInfo->timingInfo.tRHW;
            mNandFlashIf.flashInterfacTmCtrl9.b.TWB                     = pCfgInfo->timingInfo.tWB;
            mNandFlashIf.flashInterfacTmCtrl9.b.TWPRE                   = pCfgInfo->timingInfo.tWPRE;
            mNandFlashIf.flashInterfacTmCtrl9.b.TRPRE                   = pCfgInfo->timingInfo.tRPRE;
            mNandFlashIf.vrefRcvTypeSelectio.b.DQS_VREF_SEL             = 1; // default - don't care -> Cathy's recommendation for CMOS receiver
            mNandFlashIf.vrefRcvTypeSelectio.b.DQS_RCVTYPE              = 1; // Single end enable
            mNandFlashIf.vrefRcvTypeSelectio.b.REB_VREF_SEL             = 1; // default - don't care -> I think this should be 1 as the same reason of DQS_VREF_SEL
            mNandFlashIf.vrefRcvTypeSelectio.b.REB_RCVTYPE              = 1; // Single end enable
            break;
        }
        case NF_ONFI_NVDDR2:
        case NF_JEDEC_TOGGLE2:
        {
            mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.b.T1_IND_ASYN  = pCfgInfo->timingInfo.T1;
            mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.b.T2_IND_ASYN  = pCfgInfo->timingInfo.T2;
            mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.b.T3_IND_ASYN  = pCfgInfo->timingInfo.T3;
            mNandFlashIf.flashInterfacTmCtrl3AsynchroDeviceAnd.b.T4_IND_ASYN  = pCfgInfo->timingInfo.T4;
            mNandFlashIf.flashInterfacTmCtrl4Toggle1Toggle2.b.T1_SEQ_JDC      = pCfgInfo->timingInfo.T1;
            mNandFlashIf.flashInterfacTmCtrl4Toggle1Toggle2.b.T2_SEQ_JDC      = pCfgInfo->timingInfo.T2;
            mNandFlashIf.flashInterfacTmCtrl4Toggle1Toggle2.b.T3_SEQ_JDC      = pCfgInfo->timingInfo.T3;
            mNandFlashIf.flashInterfacTmCtrl4Toggle1Toggle2.b.T4_SEQ_JDC      = pCfgInfo->timingInfo.T4;
            mNandFlashIf.dllCtrl1ForChan0.b.BYPASS_EN_CH0               = 0;
            mNandFlashIf.dllCtrl1ForChan1.b.BYPASS_EN_CH1               = 0;
            mNandFlashIf.dllCtrl1ForChan2.b.BYPASS_EN_CH2               = 0;
            mNandFlashIf.dllCtrl1ForChan3.b.BYPASS_EN_CH3               = 0;
            mNandFlashIf.dllCtrl1ForChan4.b.BYPASS_EN_CH4               = 0;
            mNandFlashIf.dllCtrl1ForChan5.b.BYPASS_EN_CH5               = 0;
            mNandFlashIf.dllCtrl1ForChan6.b.BYPASS_EN_CH6               = 0;
            mNandFlashIf.dllCtrl1ForChan7.b.BYPASS_EN_CH7               = 0;
            mNandFlashIf.dllCtrl1ForChan0.b.GAIN2X_CH0                  = 0;
            mNandFlashIf.dllCtrl1ForChan1.b.GAIN2X_CH1                  = 0;
            mNandFlashIf.dllCtrl1ForChan2.b.GAIN2X_CH2                  = 0;
            mNandFlashIf.dllCtrl1ForChan3.b.GAIN2X_CH3                  = 0;
            mNandFlashIf.dllCtrl1ForChan4.b.GAIN2X_CH4                  = 0;
            mNandFlashIf.dllCtrl1ForChan5.b.GAIN2X_CH5                  = 0;
            mNandFlashIf.dllCtrl1ForChan6.b.GAIN2X_CH6                  = 0;
            mNandFlashIf.dllCtrl1ForChan7.b.GAIN2X_CH7                  = 0;
            mNandFlashIf.dllCtrl1ForChan0.b.DELAY_TEST_CH0              = pCfgInfo->timingInfo.T7[0];
            mNandFlashIf.dllCtrl1ForChan1.b.DELAY_TEST_CH1              = pCfgInfo->timingInfo.T7[1];
            mNandFlashIf.dllCtrl1ForChan2.b.DELAY_TEST_CH2              = pCfgInfo->timingInfo.T7[2];
            mNandFlashIf.dllCtrl1ForChan3.b.DELAY_TEST_CH3              = pCfgInfo->timingInfo.T7[3];
            mNandFlashIf.dllCtrl1ForChan4.b.DELAY_TEST_CH4              = pCfgInfo->timingInfo.T7[4];
            mNandFlashIf.dllCtrl1ForChan5.b.DELAY_TEST_CH5              = pCfgInfo->timingInfo.T7[5];
            mNandFlashIf.dllCtrl1ForChan6.b.DELAY_TEST_CH6              = pCfgInfo->timingInfo.T7[6];
            mNandFlashIf.dllCtrl1ForChan7.b.DELAY_TEST_CH7              = pCfgInfo->timingInfo.T7[7];

            mNandFlashIf.flashInterfacTmCtrl6.b.T8_SEQ                  = pCfgInfo->timingInfo.T8;
            mNandFlashIf.flashInterfacTmCtrl6.b.T9_SEQ                  = pCfgInfo->timingInfo.T9;
            mNandFlashIf.flashInterfacTmCtrl7.b.TCS                     = pCfgInfo->timingInfo.tCS;
            mNandFlashIf.flashInterfacTmCtrl7.b.TCEH                    = pCfgInfo->timingInfo.tCEH;
            mNandFlashIf.flashInterfacTmCtrl7.b.TCWAW                   = pCfgInfo->timingInfo.tCWAW;
            mNandFlashIf.flashInterfacTmCtrl8.b.TWPST                   = pCfgInfo->timingInfo.tWPST;
            mNandFlashIf.flashInterfacTmCtrl8.b.TWPSTH                  = pCfgInfo->timingInfo.tWPSTH;
            mNandFlashIf.flashInterfacTmCtrl8.b.TRPST                   = pCfgInfo->timingInfo.tRPST;
            mNandFlashIf.flashInterfacTmCtrl8.b.TRPSTH                  = pCfgInfo->timingInfo.tRPSTH;
            mNandFlashIf.flashInterfacTmCtrl9.b.TRHW                    = pCfgInfo->timingInfo.tRHW;
            mNandFlashIf.flashInterfacTmCtrl9.b.TWB                     = pCfgInfo->timingInfo.tWB;
            mNandFlashIf.flashInterfacTmCtrl9.b.TWPRE                   = pCfgInfo->timingInfo.tWPRE;
            mNandFlashIf.flashInterfacTmCtrl9.b.TRPRE                   = pCfgInfo->timingInfo.tRPRE;
            mNandFlashIf.vrefRcvTypeSelectio.b.DQS_VREF_SEL             = 1; // Differential reference voltage
            mNandFlashIf.vrefRcvTypeSelectio.b.DQS_RCVTYPE              = 1; // Differential receiver
            mNandFlashIf.vrefRcvTypeSelectio.b.REB_VREF_SEL             = 1; // Differential reference voltage
            mNandFlashIf.vrefRcvTypeSelectio.b.REB_RCVTYPE              = 1; // Differential receiver

            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_DQ          = 0;    // default - don't care
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_DQ            = 0;    // variable
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_DQSC        = 0;    // default - don't care
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_DQSC          = 1;    // default - don't care
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_DQS         = 1;    // default - don't care
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_DQS           = 0;    // Fixed
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_AC          = 0;    // variable
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_AC            = 1;    // Fixed
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_REC         = 1;    // Fixed
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_REC           = 0;    // Fixed

            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_REN         = 0;    // default - don't care
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_REN           = 1;    // variable
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_WP          = 0;    // default - don't care
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_WP            = 1;    // default - don't care
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_WEN         = 0;    // default - don't care
            mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_WEN           = 1;    // Fixed

            NFIF_UpdateFlashDll(&pCfgInfo->phyInfo);

            HalNfCtrl_OneTimeInit();

            //---Nand Busy Time
            mNandFlashIf.flashInterfacCfg3.b.BUSY_LOG_EN                = 1;
            mNandFlashIf.nandPhyCfg1.b.BUSY_INFO_SEL                    = 1;
            mNandFlashIf.flashInterfacCfg3.b.BUSY_DIV_SEL               = 0x01;

            break;
        }
        default:
        {
            HALT();
        }
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Change NFIF WCNT timming

    @param[in] level   Target temp adaptive level.
**/
//-----------------------------------------------------------------------------
void HalNfif_ChangeTiming(U32 level)
{
    // Read
    mNandFlashIf.flashSeqInterfacTmCtrl6.b.WCNT61 = gTMCtrlTbl[TM_CTRL6].ivWCNT + gTMCtrlTbl[TM_CTRL6].ofstWCNT * level;

    // Write
    mNandFlashIf.flashSeqInterfacTmCtrl2.b.WCNT21 = gTMCtrlTbl[TM_CTRL2].ivWCNT + gTMCtrlTbl[TM_CTRL2].ofstWCNT * level;
}

#include "NormSection.h"
//-----------------------------------------------------------------------------
/**
    Dump NFIF information.\n
**/
//-----------------------------------------------------------------------------
void HalNfif_DumpInfo(void)
{
    uint32_t ch;
    NfPhyInfo_t *pPhyInfo = &gCfgInfo.phyInfo;

    M_PRINTF("\n[NFIF]\n");
    M_PRINTF("NPHY CMD/REB/WEB_ODT[0x%02X,0x%02X,0x%02X]\n", pPhyInfo->CMD_ODT, pPhyInfo->REB_ODT, pPhyInfo->WEB_ODT);
    for (ch = 0; ch < gNfInfo.numChannels; ch++)
    {
        M_PRINTF("NPHY CH#%d DQ/DQS_ODT[0x%02X] PHSEL[0x%02X,0x%02X]\n", ch, pPhyInfo->DATA_ODT[ch], pPhyInfo->PHSEL0[ch], pPhyInfo->PHSEL1[ch]);
    }
}

#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    Set up NAND flash data interface type

    @param[in]  ifData     data interface type
**/
//-----------------------------------------------------------------------------
void HalNfif_SetDataIf(NfIfData_t ifData)
{
    switch (ifData)
    {
        case NF_ASYNC:
            mNandFlashIf.flashInterfacCfg.b.FLASH_TYPE_L = 0;
            mNandFlashIf.flashInterfacCfg.b.FLASH_TYPE_H = 0;
            break;
        case NF_ONFI_SDR:
            mNandFlashIf.flashInterfacCfg.b.FLASH_TYPE_L = 2;
            mNandFlashIf.flashInterfacCfg.b.FLASH_TYPE_H = 0;
            break;
        case NF_ONFI_NVDDR:
            mNandFlashIf.flashInterfacCfg.b.FLASH_TYPE_L = 2;
            mNandFlashIf.flashInterfacCfg.b.FLASH_TYPE_H = 1;
            break;
        case NF_JEDEC_TOGGLE1:
            mNandFlashIf.flashInterfacCfg.b.FLASH_TYPE_L = 1;
            mNandFlashIf.flashInterfacCfg.b.FLASH_TYPE_H = 1;
            break;
        case NF_ONFI_NVDDR2:
            mNandFlashIf.flashInterfacCfg.b.FLASH_TYPE_L = 0;
            mNandFlashIf.flashInterfacCfg.b.FLASH_TYPE_H = 1;
            break;
        case NF_JEDEC_TOGGLE2:
            mNandFlashIf.flashInterfacCfg.b.FLASH_TYPE_L = 3;
            mNandFlashIf.flashInterfacCfg.b.FLASH_TYPE_H = 1;
            break;
        default:
            HALT();
    }
}

//-----------------------------------------------------------------------------
/**
    Reset a NAND flash.

    @param[in]  pAddr    Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
    @param[in]  resetCmd   reset command (FFh=reset, FAh=reset LUN, FC=sync reset)

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_Reset(Pna_t* pAddr, uint8_t resetCmd, uint8_t mode)
{
    Error_t error = cEcNoError;

    switch (mode)
    {
        case DIRECT:
            HalNfCtrl_ResetDm(pAddr, resetCmd);
            break;
        case INDIRECT:
            HalNfCtrl_ResetIm(pAddr, resetCmd);
            break;
        default:
            error = cEcNfifNotSupport;
            break;
    }

    return error;
}

//-----------------------------------------------------------------------------
/**
    Read a status

    @param[in]  pAddr    Phyiscal NAND Flash address pointer
                pAddr->ch    channel number
                pAddr->dev  device number

    @return value of NAND flash status register
**/
//-----------------------------------------------------------------------------
uint8_t HalNfif_ReadStatus(Pna_t* pAddr, uint8_t mode)
{
    uint8_t status = 0;

    switch(mode)
    {
        case DIRECT:
            status = HalNfCtrl_ReadStatusDm(pAddr);
            break;
        case INDIRECT:
            status = HalNfCtrl_ReadStatusIm(pAddr);
            break;
        default:
            break;
    }

    return status;
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - Read flash id.\n

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
                pAddr->row      id address (00h=manufacturer id, 20h=ONFI, 40h=JEDEC signature)
    @param[out] pBuffer    Id data pointer
    @param[in]  count      id byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_ReadId(Pna_t* pAddr, void* pBuffer, uint32_t count, uint8_t mode)
{
    Error_t error = cEcNoError;

    switch (mode)
    {
        case DIRECT:
            HalNfCtrl_ReadIdDm(pAddr, pBuffer, count);
            break;
        case INDIRECT:
            HalNfCtrl_ReadIdIm(pAddr, pBuffer, count);
            break;
        case TEST:
            HalNfCtrl_ReadIdTm(pAddr, pBuffer, count);
            break;
        default:
            error = cEcNfifNotSupport;
            break;
    }

    return error;
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - Read parameter page.\n

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch       channel number
                pAddr->dev      device number
                pAddr->row      parameter page address (00h=ONFI, 40h=JEDEC)
    @param[out] pBuffer    Parameter buffer pointer
    @param[in]  count      byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_ReadParameterPage(Pna_t* pAddr, void* pBuffer, uint32_t count, uint8_t mode)
{
    Error_t error = cEcNoError;

    switch (mode)
    {
        case DIRECT:
            HalNfCtrl_ReadParameterPageDm(pAddr, pBuffer, count);
            break;
        case INDIRECT:
            HalNfCtrl_ReadParameterPageIm(pAddr, pBuffer, count);
            break;
        default:
            error = cEcNfifNotSupport;
            break;
    }

    return error;
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - Get 4bytes feature parameter.\n

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
                pAddr->row      feature address
    @param[out] param      4byte feature parameters

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_GetFeature(Pna_t* pAddr, uint8_t param[], uint8_t mode)
{
    Error_t error = cEcNoError;

    switch(mode)
    {
        case DIRECT:
            HalNfCtrl_GetFeatureDm(pAddr, param);
            break;
        case INDIRECT:
            HalNfCtrl_GetFeatureIm(pAddr, param);
            break;
        default:
            error = cEcNfifNotSupport;
            break;
    }

    return error;
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - Set 4bytes feature parameter.\n

    @param[in]  pAddr    Phyiscal NAND Flash address pointer
                pAddr->ch       channel number
                pAddr->dev      device number
                pAddr->row      feature address
    @param[in]  param    4byte feature parameters

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_SetFeature(Pna_t* pAddr, uint8_t param[], uint8_t mode)
{
    Error_t error = cEcNoError;

    switch (mode)
    {
        case DIRECT:
            HalNfCtrl_SetFeatureDm(pAddr, param);
            break;
        case INDIRECT:
            HalNfCtrl_SetFeatureIm(pAddr, param);
            break;
        default:
            error = cEcNfifNotSupport;
            break;
    }

    return error;
}

//-----------------------------------------------------------------------------
/**
    NAND flash Read Page operation.

    @param[in]  pAddr      Logical NAND Flash address pointer

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t  HalNfif_ReadPage(Pna_t* pAddr, void* pBuffer, uint32_t count, uint8_t mode, uint8_t prefix)
{
    Error_t error = cEcNoError;

    switch (mode)
    {
        case DIRECT:
            HalNfCtrl_ReadPageDm(pAddr, pBuffer, count, prefix);
            break;
        case INDIRECT:
            HalNfCtrl_ReadPageIm(pAddr, pBuffer, count);
            break;
        default:
            error = cEcNfifNotSupport;
            break;
    }

    return error;
}
//-----------------------------------------------------------------------------
/**
    NAND flash Write Page operation.

    @param[in]  pAddr      Logical NAND Flash address pointer

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t  HalNfif_WritePage(Pna_t* pAddr, void* pBuffer, uint32_t count, uint8_t mode, uint8_t prefix)
{
    Error_t error = cEcNoError;

    switch (mode)
    {
        case DIRECT:
            HalNfCtrl_WritePageDm(pAddr, pBuffer, count, prefix);
            break;
        case INDIRECT:
            HalNfCtrl_WritePageIm(pAddr, pBuffer, count);
            break;
        default:
            error = cEcNfifNotSupport;
            break;
    }

    return error;
}
//-----------------------------------------------------------------------------
/**
    NAND flash erase block operation.

    @param[in]  pAddr      Logical NAND Flash address pointer

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t  HalNfif_EraseBlock(Pna_t* pAddr, uint8_t mode, uint8_t slc)
{
    Error_t error = cEcNoError;

    switch (mode)
    {
        case DIRECT:
        case INDIRECT:
            HalNfCtrl_EraseBlockIm(pAddr);
            break;
        default:
            error = cEcNfifNotSupport;
            break;
    }

    return error;
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - Wait for ready status.

    @param[in]  pAddr       Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
    @param[in]  value       bit value
    @param[in]  timeOutUsec time out usec

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_WaitForStatusSet(Pna_t* pAddr, uint32_t value, uint32_t timeOutUsec, uint8_t mode)
{
    Error_t error = cEcNoError;

    switch (mode)
    {
        case DIRECT:
            HalNfCtrl_WaitForStatusSetDm(pAddr, value, timeOutUsec);
            break;
        case INDIRECT:
            HalNfCtrl_WaitForStatusSetIm(pAddr, value, timeOutUsec);
            break;
        default:
            error = cEcNfifNotSupport;
            break;
    }

    return error;
}
//-----------------------------------------------------------------------------
/**
    Indirect mode - Wait for ready status.

    @param[in]  pAddr       Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
    @param[in]  value       bit value
    @param[in]  timeOutUsec time out usec

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_SetVoltageOffset(Pna_t* pAddr, uint8_t featureAddr, uint8_t param[], uint8_t mode)
{
    Error_t error = cEcNoError;

    switch (mode)
    {
        case DIRECT:
        case INDIRECT:
            HalNfCtrl_SetVoltageOffsetDm(pAddr, featureAddr, param);
            break;
        default:
            error = cEcNfifNotSupport;
            break;
    }

    return error;
}
//-----------------------------------------------------------------------------
/**
    Indirect mode - Wait for ready status.

    @param[in]  pAddr       Phyiscal NAND Flash address pointer
                pAddr->ch        channel number
                pAddr->dev      device number
    @param[in]  value       bit value
    @param[in]  timeOutUsec time out usec

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_GetVoltageOffset(Pna_t* pAddr, uint8_t featureAddr, uint8_t param[], uint8_t mode)
{
    Error_t error = cEcNoError;

    switch (mode)
    {
        case DIRECT:
        case INDIRECT:
            HalNfCtrl_GetVoltageOffsetDm(pAddr, featureAddr, param);
            break;
        default:
            error = cEcNfifNotSupport;
            break;
    }

    return error;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    Set NAND Flash Operation Mode.

    @param[in]  mode         device  number

    @return previous mode
**/
//-----------------------------------------------------------------------------
uint32_t HalNfif_SetFlashOperationMode(FcAccessMode_t mode)
{
    uint32_t previousMode = rNandFlashIf.flashInterfacCfg.b.OP_MODE;

    rNandFlashIf.flashInterfacCfg.b.OP_MODE = mode;

    return previousMode;
}
//-----------------------------------------------------------------------------
/**
    Indirect/Direct mode - enable or disable write protection

    @param[in] enDisable    enable or disable
**/
//-----------------------------------------------------------------------------
void HalNfif_EnDisableWriteProtect(bool enDisable)
{
    rNandFlashIf.flashDirectAccessCtrlPort1.b.WP_N_CTRL = (enDisable == ENABLE) ? 0 : 1;
}

//=============================================================================
//=============================================================================
//=============================================================================
//Direct mode
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    Direct mode - write a command (1byte)

    @param[in]  cmd        commands
**/
//-----------------------------------------------------------------------------
void HalNfif_DmWriteCommandU8(uint16_t cmd)
{
    FlashDirectAccessCtrlPort1_t dmCtrl;

    NFIF_DmSetAddrDataPort(cmd);

    dmCtrl.all = rNandFlashIf.flashDirectAccessCtrlPort1.all;
    dmCtrl.b.CE_N_CTRL = 0; // assert chip enable
    dmCtrl.b.CLE_CTRL  = 1; // assert command latch enable
    dmCtrl.b.ALE_CTRL  = 0; // deassert address latch enable
    dmCtrl.b.WE_N_CTRL = 0; // assert write enable
    dmCtrl.b.RE_N_CTRL = 1; // deassert read enable
    dmCtrl.b.OE_EN     = 1; // output mode
    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;

    HalTimer_DelayUsec(NF_tWP_USEC);

    dmCtrl.b.WE_N_CTRL = 1; // deassert write enable
    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;
}

//-----------------------------------------------------------------------------
/**
    Direct mode - write a address byte (CLE=0, ALE=1)

    @param[in] value  address value
**/
//-----------------------------------------------------------------------------
void HalNfif_DmWriteAddressU8(uint8_t value)
{
    FlashDirectAccessCtrlPort1_t dmCtrl;

    NFIF_DmSetAddrDataPort(value);

    dmCtrl.all = rNandFlashIf.flashDirectAccessCtrlPort1.all;
    dmCtrl.b.CE_N_CTRL = 0; // assert chip enable
    dmCtrl.b.CLE_CTRL  = 0; // deassert command latch enable
    dmCtrl.b.ALE_CTRL  = 1; // assert address latch enable
    dmCtrl.b.WE_N_CTRL = 0; // assert write enable
    dmCtrl.b.RE_N_CTRL = 1; // deassert read enable
    dmCtrl.b.OE_EN     = 1; // output mode
    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;

    HalTimer_DelayUsec(NF_tWP_USEC);

    dmCtrl.b.WE_N_CTRL = 1; // deassert write enable
    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;
}

//-----------------------------------------------------------------------------
/**
    Direct mode - write a data (actual write data depends on flash data bus width)

    @param[in]  word  word data to be written
**/
//-----------------------------------------------------------------------------
void HalNfif_DmWriteDataU16(uint16_t word)
{
    FlashDirectAccessCtrlPort1_t dmCtrl;

    NFIF_DmSetAddrDataPort(word);    // set data byte

    dmCtrl.all = rNandFlashIf.flashDirectAccessCtrlPort1.all;
    dmCtrl.b.CE_N_CTRL = 0; // assert chip enable
    dmCtrl.b.CLE_CTRL  = 0; // deassert command latch enable
    dmCtrl.b.ALE_CTRL  = 0; // deassert address latch enable
    dmCtrl.b.WE_N_CTRL = 0; // assert write enable
    dmCtrl.b.RE_N_CTRL = 1; // deassert read enable
    dmCtrl.b.OE_EN     = 1; // output mode
    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;

    HalTimer_DelayUsec(NF_tWP_USEC);

    dmCtrl.b.WE_N_CTRL = 1; // deassert write enable
    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;
}

//-----------------------------------------------------------------------------
/**
    Direct mode - write a data with Toggle

    @param[in]  word  word data to be written
**/
//-----------------------------------------------------------------------------
void HalNfif_DmWriteToggleDataU16(uint16_t word)
{
    FlashDirectAccessCtrlPort1_t dmCtrl;

    NFIF_DmSetAddrDataPort(word);    // set data byte

    dmCtrl.all = rNandFlashIf.flashDirectAccessCtrlPort1.all;
    dmCtrl.b.CE_N_CTRL = 0; // assert chip enable
    dmCtrl.b.CLE_CTRL  = 0; // deassert command latch enable
    dmCtrl.b.ALE_CTRL  = 0; // deassert address latch enable
    dmCtrl.b.OE_EN     = 1; // output mode
    dmCtrl.b.DQS       = 0; // deassert write enable
    dmCtrl.b.DQS_OE    = 1; // output mode
    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;

    HalTimer_DelayUsec(NF_tWP_USEC);

    dmCtrl.b.DQS       = 1; // assert write enable
    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;
}

//-----------------------------------------------------------------------------
/**
    Direct mode - write a data with Toggle

    @param[in]  word  word data to be written
**/
//-----------------------------------------------------------------------------
void HalNfif_DmWriteToggleData(uint16_t word, uint16_t mode)
{
    FlashDirectAccessCtrlPort1_t dmCtrl;

    NFIF_DmSetAddrDataPort(word);    // set data byte

    dmCtrl.all = rNandFlashIf.flashDirectAccessCtrlPort1.all;
    dmCtrl.b.CE_N_CTRL = 0; // assert chip enable
    dmCtrl.b.CLE_CTRL  = 0; // deassert command latch enable
    dmCtrl.b.ALE_CTRL  = 0; // deassert address latch enable
    dmCtrl.b.OE_EN     = 1; // output mode
    dmCtrl.b.DQS       = mode; // deassert write enable
    dmCtrl.b.DQS_OE    = 1; // output mode
    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;

    HalTimer_DelayUsec(NF_tWP_USEC);

    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;
}
//-----------------------------------------------------------------------------
/**
    Direct mode - Read a data with Toggle (actual read data depends on flash data bus width)

    @return data
**/
//-----------------------------------------------------------------------------
uint16_t HalNfif_DmReadToggleDataU16(void)
{
    uint16_t word;
    FlashDirectAccessCtrlPort1_t dmCtrl;

    dmCtrl.all = rNandFlashIf.flashDirectAccessCtrlPort1.all;
    dmCtrl.b.RE_N_CTRL = ~(dmCtrl.b.RE_N_CTRL); // toggle read enable
    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;

    HalTimer_DelayUsec(NF_tRP_USEC);
    word = NFIF_DmGetAddrDataPort();

    return word;
}

//-----------------------------------------------------------------------------
/**
    Direct mode - Read a data (actual read data depends on flash data bus width)

    @return data
**/
//-----------------------------------------------------------------------------
uint16_t HalNfif_DmReadDataU16(void)
{
    uint16_t word;
    FlashDirectAccessCtrlPort1_t dmCtrl;

    dmCtrl.all = rNandFlashIf.flashDirectAccessCtrlPort1.all;
    dmCtrl.b.CE_N_CTRL = 0; // assert chip enable
    dmCtrl.b.CLE_CTRL  = 0; // deassert command latch enable
    dmCtrl.b.ALE_CTRL  = 0; // deassert address latch enable
    dmCtrl.b.WE_N_CTRL = 1; // deassert write enable
    dmCtrl.b.RE_N_CTRL = 0; // assert read enable
    dmCtrl.b.OE_EN     = 0; // input mode
    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;

    HalTimer_DelayUsec(NF_tRP_USEC);
    word = NFIF_DmGetAddrDataPort();

    dmCtrl.b.RE_N_CTRL = 1; // deassert read enable
    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;

    return word;
}

//-----------------------------------------------------------------------------
/**
    Direct mode - back to default mode
**/
//-----------------------------------------------------------------------------
void HalNfif_DmBackToDefault(void)
{
    FlashDirectAccessCtrlPort1_t dmCtrl;

    dmCtrl.all = rNandFlashIf.flashDirectAccessCtrlPort1.all;

    dmCtrl.b.CE_N_CTRL = 1; // deassert chip enable
    dmCtrl.b.CLE_CTRL  = 0; // deassert command latch enable
    dmCtrl.b.ALE_CTRL  = 0; // deassert address latch enable
    dmCtrl.b.WE_N_CTRL = 1; // deassert write enable
    dmCtrl.b.RE_N_CTRL = 1; // deassert read enable
    dmCtrl.b.WP_N_CTRL = 1; // assert write protect enable

    dmCtrl.b.DQS_OE    = 0; // input mode
    dmCtrl.b.DQS       = 1; // deassert write enable

    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;

    HalNfif_SetFlashOperationMode(SEQUENCER);

    rNandFlashIf.flashSeqCtrl1.b.FSEQ_RESET = 1;
    rNandFlashIf.flashSeqCtrl1.b.FSEQ_RESET = 0;
}

//-----------------------------------------------------------------------------
/**
    Direct mode - Wait RE to DQS delay with Toggle mode

    @return data
**/
//-----------------------------------------------------------------------------
void HalNfif_DmWaitReToDqsDelay(void)
{
    FlashDirectAccessCtrlPort1_t dmCtrl;

    dmCtrl.all = rNandFlashIf.flashDirectAccessCtrlPort1.all;
    dmCtrl.b.CE_N_CTRL = 0; // assert chip enable
    dmCtrl.b.CLE_CTRL  = 0; // deassert command latch enable
    dmCtrl.b.ALE_CTRL  = 0; // deassert address latch enable
    dmCtrl.b.WE_N_CTRL = 1; // deassert write enable
    dmCtrl.b.RE_N_CTRL = ~(dmCtrl.b.RE_N_CTRL); // toggle read enable
    dmCtrl.b.OE_EN     = 0; // input mode
    rNandFlashIf.flashDirectAccessCtrlPort1.all = dmCtrl.all;

    HalTimer_DelayUsec(NF_tDQSRE_USEC);
}
//-----------------------------------------------------------------------------
/**
    Direct mode - set device and channel number.

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch       channel number
                pAddr->dev     device number
**/
//-----------------------------------------------------------------------------
void HalNfif_DmSetDeviceChannel(Pna_t* pAddr)
{
    HalNfif_SetFlashOperationMode(DIRECT);

    rNandFlashIf.flashDirectIndAccessCfg.b.DEVICE_DIR_IND = pAddr->dev;
    rNandFlashIf.flashDirectIndAccessCfg.b.CHANNEL_DIR_IND = pAddr->ch;
}

//=============================================================================
//=============================================================================
//=============================================================================
//Indirect Mode
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    Indirect mode - Wait for FIFO ready.

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_ImWaitForFifoReady(void)
{
    return HalRci_WaitValueU16((uint32_t)&rNandFlashIf.flashIndAccessCfg, B_14, B_14, 1000);
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - Wait for command start bit clear.

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_ImWaitForCommandStart(void)
{
    return HalRci_WaitValueU16((uint32_t)&rNandFlashIf.flashIndAccessCfg, 0, B_7, 1000);
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - force ready by firmware
 **/
//-----------------------------------------------------------------------------
void HalNfif_ImSetDummyReady(void)
{
    rNandFlashIf.flashIndAccessCfg.b.DUMMY_RBB = 1;
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - Setup command, nand address\n

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch       channel number
                pAddr->dev     device number
                pAddr->col      column address (2byte)
                pAddr->row     row address (3byte)
    @param[in]  command1   1st command op code
    @param[in]  command2   2nd command op code
    @param[in]  count      byte count to transfer
**/
//-----------------------------------------------------------------------------
void HalNfif_ImSetupCmdAddrCount(Pna_t* pAddr, uint8_t command1, uint8_t command2, uint32_t count)
{
    NFIF_ImSetDeviceChannel(pAddr);

    rNandFlashIf.flashIndAccessCmdPort1.b.BYTE0_IND = command1;
    rNandFlashIf.flashIndAccessCmdPort1.b.BYTE1_IND = (uint8_t)pAddr->col;
    rNandFlashIf.flashIndAccessCmdPort2.b.BYTE2_IND = (uint8_t)(pAddr->col>>8);
    rNandFlashIf.flashIndAccessCmdPort2.b.BYTE3_IND = (uint8_t)pAddr->row;
    rNandFlashIf.flashIndAccessCmdPort3.b.BYTE4_IND = (uint8_t)(pAddr->row>>8);
    rNandFlashIf.flashIndAccessCmdPort3.b.BYTE5_IND = (uint8_t)(pAddr->row>>16);
    rNandFlashIf.flashIndAccessCmdPort4.b.BYTE6_IND = command2;
    rNandFlashIf.flashIndAccessCmdPort4.b.BYTE7_IND = 0;

    rNandFlashIf.XFER_CNT_IND = count;

    NFIF_ImResetFifo();
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - Setup command, nand row address\n

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch       channel number
                pAddr->dev     device number
                pAddr->row     row address (3byte)
    @param[in]  command1   1st command op code
    @param[in]  command2   2nd command op code
    @param[in]  count      byte count to transfer
**/
//-----------------------------------------------------------------------------
void HalNfif_ImSetupCmdRowAddrCount(Pna_t* pAddr, uint8_t command1, uint8_t command2, uint32_t count)
{
    NFIF_ImSetDeviceChannel(pAddr);

    rNandFlashIf.flashIndAccessCmdPort1.b.BYTE0_IND = command1;
    rNandFlashIf.flashIndAccessCmdPort1.b.BYTE1_IND = (uint8_t)pAddr->row;
    rNandFlashIf.flashIndAccessCmdPort2.b.BYTE2_IND = (uint8_t)(pAddr->row>>8);
    rNandFlashIf.flashIndAccessCmdPort2.b.BYTE3_IND = (uint8_t)(pAddr->row>>16);
    rNandFlashIf.flashIndAccessCmdPort3.b.BYTE4_IND = command2;
    rNandFlashIf.flashIndAccessCmdPort3.b.BYTE5_IND = 0;

    rNandFlashIf.XFER_CNT_IND = count;

    NFIF_ImResetFifo();
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - execute a NAND flash command.

    @param[in]  cmd       command code to excute
**/
//-----------------------------------------------------------------------------
void HalNfif_ImExecuteCommand(uint16_t cmd)
{
    uint16_t tmpCmd;

    tmpCmd = rNandFlashIf.flashIndAccessCfg.all;
    tmpCmd &= 0xFE00;
    tmpCmd |= cmd;

    rNandFlashIf.flashIndAccessCfg.all = cmd;
}
//-----------------------------------------------------------------------------
/**
    Indirect mode - read dword data

    @return data from port
**/
//-----------------------------------------------------------------------------
uint32_t HalNfif_ImReadDataU32(void)
{
    uint32_t data;

    data =  ((uint32_t)(((rNandFlashIf.FLASH_DATA_IND_31_16) << 16) & DWORD_MASK)) | (uint32_t)(rNandFlashIf.FLASH_DATA_IND_15_0 & DWORD_MASK);

    return data;
}
//-----------------------------------------------------------------------------
/**
    Indirect mode - read data from NAND flash.

    @param[out] pBuffer    Buffer pointer
    @param[in]  count      byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_ImReadData(uint32_t* pBuffer, uint32_t count)
{
    count = count / 4;                   ///< 4 bytes at a time

    while(count--)
    {
        if (HalNfif_ImWaitForFifoReady()!=cEcNoError) ///< wait FIFO ready (1msec)
        {
            return(cEcNfimFifoRdTimeOut);
        }
            *pBuffer = HalNfif_ImReadDataU32();


        BEGIN_MEDIA_DBG_DUMP(DBG_DUMP_MEDIA_SEQ)
            Debug_DumpMemU32((uint32_t)pBuffer, 4);
        END_MEDIA_DBG_DUMP
            //Debug_DumpMemU32((uint32_t)pBuffer, 4);
        *pBuffer++;
    }

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - write data from NAND flash.

    @param[in]  pBuffer    Buffer pointer
    @param[in]  count      byte count to write

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_ImWriteData(uint32_t* pBuffer, uint32_t count)
{
    count = count/4;                   ///< 4 bytes at a time

    while(count--)
    {
        if (HalNfif_ImWaitForFifoReady()!=cEcNoError) ///< wait FIFO ready (1msec)
        {
            return(cEcNfimFifoWrTimeOut);
        }
        NFIF_ImWriteDataU32(*pBuffer++);
    }

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - read data from NAND flash.

    @param[out] pBuffer    Buffer pointer
    @param[in]  count      byte count to read

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNfif_ImReadDataDdr(uint8_t *pBuffer, uint32_t count)
{
    uint32_t value;
    count = count / 4;                   ///< 4 bytes at a time

    while(count--)
    {
        if (HalNfif_ImWaitForFifoReady()!=cEcNoError) ///< wait FIFO ready (1msec)
        {
            return(cEcNfimFifoRdTimeOut);
        }
        value = HalNfif_ImReadDataU32();
        *pBuffer++ = value & 0xFF;
        *pBuffer++ = (value >> 16) & 0xFF;

        BEGIN_MEDIA_DBG_DUMP(DBG_DUMP_MEDIA_SEQ)
            Debug_DumpMemU32((uint32_t)&value, 4);
        END_MEDIA_DBG_DUMP
    }

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
//=============================================================================
//=============================================================================
//=============================================================================
//-----------------------------------------------------------------------------
/**
    Enable/Disable Nfif power save mode

    @param[in]  enable    TRUE: Power Down, FALSE: Normal

**/
//-----------------------------------------------------------------------------
static void NFIF_PowerManage(bool enable)
{
    NFIF_EnablePwrDownCh(enable);
    NFIF_EnablePdCh(enable);
}
//-----------------------------------------------------------------------------
/**
    Enable Power Down Phy for Channels.

    @param[in]  enable    TRUE: Power Down, FALSE: Normal

    @return error code
**/
//-----------------------------------------------------------------------------
static void NFIF_EnablePwrDownCh(bool enable)
{
    if (enable == TRUE)
    {
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH0 = 1;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH1 = 1;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH2 = 1;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH3 = 1;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH4 = 1;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH5 = 1;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH6 = 1;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH7 = 1;
    }
    else
    {
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH0 = 0;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH1 = 0;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH2 = 0;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH3 = 0;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH4 = 0;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH5 = 0;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH6 = 0;
        mNandFlashIf.padPowerDnCtrl.b.PWR_DOWN_CH7 = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Enable Power Down Channels.

    @param[in]  enable    TRUE: Power Down, FALSE: Normal

    @return error code
**/
//-----------------------------------------------------------------------------
static void NFIF_EnablePdCh(bool enable)
{
    if(enable == TRUE)
    {
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH0 = 1;
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH1 = 1;
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH2 = 1;
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH3 = 1;
#if (_SOC_REV_>=0x0200)
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH4 = 1;
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH5 = 1;
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH6 = 1;
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH7 = 1;
#else
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.RF200E080_7_4_RSVD = 0x0F;
#endif
    }
    else
    {
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH0 = 0;
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH1 = 0;
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH2 = 0;
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH3 = 0;
#if (_SOC_REV_>=0x0200)
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH4 = 0;
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH5 = 0;
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH6 = 0;
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.PD_CH7 = 0;
#else
        mNandFlashIf.flashSeqChanEngineClkCtrl1.b.RF200E080_7_4_RSVD = 0x00;
#endif
    }
}

//=============================================================================
//=============================================================================
//=============================================================================
//-----------------------------------------------------------------------------
/**
    Reset NAND PHY.
    The NAND PHY requires a correct initalization sequece after power-on-reset(POR)
    before it enters operation mode. During the power-up and POR period, all the
    NAND PHY output driver is disabled. After the POR reset, the firmware must follow
    this sequence to enable and reset the NAND PHY before normal operation starts.

**/
//-----------------------------------------------------------------------------
static void NFIF_ResetNandPhy(void)
{
    rNandFlashIf.nandPhyInitCtrl.b.NP_SYNC_RSTB     = 0;
    rNandFlashIf.nandPhyInitCtrl.b.NP_SYNC_RSTB     = 1;    // Deassert PHY reset B
    HalTimer_DelayUsec(10);                                // Delay is needed between deasserting PHY reset B and disabling PHY reset
    rNandFlashIf.nandPhyInitCtrl.b.NP_RST_EN        = 1;
    rNandFlashIf.nandPhyInitCtrl.b.NP_RST_EN        = 0;    // Disable PHY reset
    HalTimer_DelayUsec(20);
    rNandFlashIf.nandPhyInitCtrl.b.NP_ZD_EN         = 0;    // Enable all output drivers of NAND PHY
    rNandFlashIf.nandPhyInitCtrl.b.NP_ZD_EN         = 1;
}

//-----------------------------------------------------------------------------
/**
    Reset NAND Dll and wait for lock.
**/
//-----------------------------------------------------------------------------
static void NFIF_ResetNandDll(void)
{
    uint8_t ch;

    volatile DllCtrl1ForChan0_t *pDllCtrl1;

    ///< Set register address
    pDllCtrl1 = (DllCtrl1ForChan0_t *)&rNandFlashIf.dllCtrl1ForChan0.all;

    for (ch = 0; ch < MAX_SUPPORTED_CHANNELS; ch++)
    {
        ///< Reset channel phy
        pDllCtrl1->b.RST_CH0 = 0;
        pDllCtrl1->b.RST_CH0 = 1;
        pDllCtrl1->b.RST_CH0 = 0;

        pDllCtrl1 += 2;
    }

    // Polling LOCK status
    // Note: WA for Eldora
    // PLL Lock status doesn't update correctly for CH4/5/6/7
    // So doesn't wait Lock Done, then add more delay
    HalTimer_DelayUsec(100);
}
//-----------------------------------------------------------------------------
/**
    Set Coarse value of DQS Gate Control\n

    @param[in]  CH      Ch
                value   Setting value
    @param[out] Error_t

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t NFIF_SetCoarseOffsetDqsGateControl(uint32_t ch, uint32_t value)
{
    uint32_t adjustedValue;

    // change bit order into b[2:0, 3] (bit 3 is least significate bit)
    adjustedValue  = (value & 0xE) >> 1;
    adjustedValue |= (value & 0x1) ? 8:0;

    //M_PRINTF("Idx=%1X [Reg=%1X] | ", value, adjustedValue);

    switch (ch)
    {
        case 0: rNandFlashIf.dqsGateCtrlForChan0To3.b.DQS_GATE_CTRL_CH0 = adjustedValue; break;
        case 1: rNandFlashIf.dqsGateCtrlForChan0To3.b.DQS_GATE_CTRL_CH1 = adjustedValue; break;
        case 2: rNandFlashIf.dqsGateCtrlForChan0To3.b.DQS_GATE_CTRL_CH2 = adjustedValue; break;
        case 3: rNandFlashIf.dqsGateCtrlForChan0To3.b.DQS_GATE_CTRL_CH3 = adjustedValue; break;
        case 4: rNandFlashIf.dqsGateCtrlForChan4To7.b.DQS_GATE_CTRL_CH4 = adjustedValue; break;
        case 5: rNandFlashIf.dqsGateCtrlForChan4To7.b.DQS_GATE_CTRL_CH5 = adjustedValue; break;
        case 6: rNandFlashIf.dqsGateCtrlForChan4To7.b.DQS_GATE_CTRL_CH6 = adjustedValue; break;
        case 7: rNandFlashIf.dqsGateCtrlForChan4To7.b.DQS_GATE_CTRL_CH7 = adjustedValue; break;
        default: break;
    }

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Set Fine value of DQS Gate Control\n

    @param[in]  CH      Ch
                value   Setting value
    @param[out] Error_t

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t NFIF_SetFineOffsetDqsGateControl(uint32_t ch, uint32_t value)
{
    switch (ch)
    {
        case 0: rNandFlashIf.qsGateCtrl1ForChan0And.b.QS_GATE_DLY_CH0 = value; break;
        case 1: rNandFlashIf.qsGateCtrl1ForChan0And.b.QS_GATE_DLY_CH1 = value; break;
        case 2: rNandFlashIf.qsGateCtrl2ForChan2And.b.QS_GATE_DLY_CH2 = value; break;
        case 3: rNandFlashIf.qsGateCtrl2ForChan2And.b.QS_GATE_DLY_CH3 = value; break;
        case 4: rNandFlashIf.qsGateCtrl3ForChan4And.b.QS_GATE_DLY_CH4 = value; break;
        case 5: rNandFlashIf.qsGateCtrl3ForChan4And.b.QS_GATE_DLY_CH5 = value; break;
        case 6: rNandFlashIf.qsGateCtrl4ForChan6And.b.QS_GATE_DLY_CH6 = value; break;
        case 7: rNandFlashIf.qsGateCtrl4ForChan6And.b.QS_GATE_DLY_CH7 = value; break;
        default: break;
    }

    return (cEcNoError);
}


//=============================================================================
//=============================================================================
//=============================================================================
//-----------------------------------------------------------------------------
/**
    Set NAND Channel Drive Select.
**/
//-----------------------------------------------------------------------------
static void NFIF_SetlDriveSelect(uint8_t value)
{
    uint32_t ch;
    volatile DriveSelForChan0_t *drDriveSelForChan;

    drDriveSelForChan = (DriveSelForChan0_t *)&rNandFlashIf.driveSelForChan0.all;
    // For initial values, use the recommended values out of the spec.
    for (ch = 0; ch < MAX_SUPPORTED_CHANNELS; ch++)
    {
        drDriveSelForChan->b.D_DRV_SEL_CH0 = value;
        drDriveSelForChan->b.C_DRV_SEL_CH0 = value;
        drDriveSelForChan->b.RE_DRV_SEL_CH0 = value;
        drDriveSelForChan->b.WE_DRV_SEL_CH0 = value;

        drDriveSelForChan++;
    }
}

//-----------------------------------------------------------------------------
/**
    Set NAND PHY ODT.

    @param[in]  pPhyInfo             NAND PHY information pointer
**/
//-----------------------------------------------------------------------------
static void NFIF_SetNandPhyOdt(NfPhyInfo_t *pPhyInfo)
{
    rNandFlashIf.cmdSubPhyOdtSetting.b.CMD_ODT_SEL      = pPhyInfo->CMD_ODT;
    rNandFlashIf.cmdSubPhyOdtSetting.b.REB_ODT_SEL      = pPhyInfo->REB_ODT;
    rNandFlashIf.cmdSubPhyOdtSetting.b.WEB_ODT_SEL      = pPhyInfo->WEB_ODT;
    rNandFlashIf.dataSubPhyOdtSetting1.b.ODT_SEL_CH0    = pPhyInfo->DATA_ODT[0];
    rNandFlashIf.dataSubPhyOdtSetting1.b.ODT_SEL_CH1    = pPhyInfo->DATA_ODT[1];
    rNandFlashIf.dataSubPhyOdtSetting1.b.ODT_SEL_CH2    = pPhyInfo->DATA_ODT[2];
    rNandFlashIf.dataSubPhyOdtSetting1.b.ODT_SEL_CH3    = pPhyInfo->DATA_ODT[3];
    rNandFlashIf.dataSubPhyOdtSetting1.b.ODT_SEL_CH4    = pPhyInfo->DATA_ODT[4];
    rNandFlashIf.dataSubPhyOdtSetting1.b.ODT_SEL_CH5    = pPhyInfo->DATA_ODT[5];
    rNandFlashIf.dataSubPhyOdtSetting1.b.ODT_SEL_CH6    = pPhyInfo->DATA_ODT[6];
    rNandFlashIf.dataSubPhyOdtSetting1.b.ODT_SEL_CH7    = pPhyInfo->DATA_ODT[7];
}

//-----------------------------------------------------------------------------
/**
    Set EXT_CE_EN_CHx - enable or disable for all channels.

    @param[in]  value      1 = enable, 0 = disable

**/
//-----------------------------------------------------------------------------
static void NFIF_SetExtCeEn(uint8_t value)
{
    mNandFlashIf.flashSeqChanEngineClkCtrl1.b.EXT_CE_EN_CH0 = value;
    mNandFlashIf.flashSeqChanEngineClkCtrl1.b.EXT_CE_EN_CH1 = value;
    mNandFlashIf.flashSeqChanEngineClkCtrl1.b.EXT_CE_EN_CH2 = value;
    mNandFlashIf.flashSeqChanEngineClkCtrl1.b.EXT_CE_EN_CH3 = value;
    mNandFlashIf.flashSeqChanEngineClkCtrl1.b.EXT_CE_EN_CH4 = value;
    mNandFlashIf.flashSeqChanEngineClkCtrl1.b.EXT_CE_EN_CH5 = value;
    mNandFlashIf.flashSeqChanEngineClkCtrl1.b.EXT_CE_EN_CH6 = value;
    mNandFlashIf.flashSeqChanEngineClkCtrl1.b.EXT_CE_EN_CH7 = value;
}

//-----------------------------------------------------------------------------
/**
    Set CE_MODE - set ce mode.

    @param[in]  ceMode      to set CE mode

**/
//-----------------------------------------------------------------------------
static void NFIF_SetCeMode(tCE_MODE ceMode)
{
    uint8_t i = 0;

    switch (ceMode)
    {
        case CE_MODE1:
        {
            mNandFlashIf.flashInterfacCfg.b.ECE_EN           = 0;
            mNandFlashIf.flashInterfacCfg.b.A15CE_EN         = 0;
            NFIF_SetExtCeEn(0);
            break;
        }
        case CE_MODE2:
        {
            mNandFlashIf.flashInterfacCfg.b.ECE_EN           = 0;
            mNandFlashIf.flashInterfacCfg.b.A15CE_EN         = 0;
            NFIF_SetExtCeEn(1);
            break;
        }
        case CE_MODE3:
        {
            mNandFlashIf.flashInterfacCfg.b.ECE_EN           = 1;
            mNandFlashIf.flashInterfacCfg.b.A15CE_EN         = 0;
            NFIF_SetExtCeEn(0);
            break;
        }
        case CE_MODE4:
        {
            mNandFlashIf.flashInterfacCfg.b.ECE_EN           = 1;
            mNandFlashIf.flashInterfacCfg.b.A15CE_EN         = 1;
            NFIF_SetExtCeEn(0);
            break;
        }
        case CE_MODE5:
        {
            // This mode forces to 8-CE mode.
            // GPIO pins are forced to output and controlled by NFIF
            mNandFlashIf.flashInterfacCfg.b.ECE_EN           = 0;
            mNandFlashIf.flashInterfacCfg.b.A15CE_EN         = 1;
            NFIF_SetExtCeEn(1);
            HalGpio_OutputEnable(0xFFFF);
            break;
        }
        case CE_MODE0:
        default:
        {
            break;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Set NAND PHY ZP/ZN.\n

    @param[in]  zp                   NAND PHY ZP value
    @param[in]  zn                   NAND PHY ZN value

    @return     None
**/
//-----------------------------------------------------------------------------
static void NFIF_SetNandPhyZpZn(uint16_t zp, uint16_t zn)
{
    uint32_t ch;
    volatile ZpZnCtrl1ForChan0_t *zpZnControl1;
    volatile ZpZnCtrl2ForChan0_t *zpZnControl2;

    zpZnControl1 = (ZpZnCtrl1ForChan0_t *)&rNandFlashIf.zpZnCtrl1ForChan0.all;
    zpZnControl2 = (ZpZnCtrl2ForChan0_t *)&rNandFlashIf.zpZnCtrl2ForChan0.all;
    // For initial values, use the recommended values out of the spec.
    for (ch = 0; ch < MAX_SUPPORTED_CHANNELS; ch++)
    {
        zpZnControl1->b.DATA_ZPR_CH0 = zp;
        zpZnControl1->b.DATA_ZNR_CH0 = zn;
        zpZnControl1->b.CMD_ZPR_CH0  = zp;
        zpZnControl1->b.CMD_ZNR_CH0  = zn;
        zpZnControl2->b.WEB_ZPR_CH0  = zp;
        zpZnControl2->b.WEB_ZNR_CH0  = zn;
        zpZnControl2->b.REB_ZPR_CH0  = zp;
        zpZnControl2->b.REB_ZNR_CH0  = zn;

        zpZnControl1 += 2;   // Since zpZnControl1 & zpZnControl2 are a pair.
        zpZnControl2 += 2;   // Since zpZnControl1 & zpZnControl2 are a pair.
    }
}
//-----------------------------------------------------------------------------
/**
    Set Nand Flash Interface clock frequency.

    @param[in]  freqMhz    frequency in Mhz unit

    @return actual set frequency in Mhz unit. (always less or equal)

**/
//-----------------------------------------------------------------------------
static uint32_t NFIF_SetNfifClockFrequency(uint32_t freqMhz)
{
    HalClock_SetFrequency(cClockTbgC, freqMhz); // set FLASH
    D_PRINTF("[FCLK] %dMHz\n", freqMhz);
    return freqMhz;
}

//-----------------------------------------------------------------------------
/**
    Set CLK_SEL and FCLK_SEL.

    @param[in]  value     value for CLK_SEL and FCLK_SEL

**/
//-----------------------------------------------------------------------------
static void NFIF_SetClkSel(uint8_t value)
{
    mNandFlashIf.nandPhyCfg1.b.FCLK_SEL = value;
    mNandFlashIf.nandPhyCfg1.b.CLK_SEL  = value;
}

//-----------------------------------------------------------------------------
/**
    Set NAND Flash Clock.

    @param[in]  ifData     NAND flash interface data type
    @param[in]  xferMode   Transfer mode

**/
//-----------------------------------------------------------------------------
static void NFIF_SetFlashClock(NfIfData_t ifData, uint8_t xferMode)
{
    switch (ifData)
    {
        case NF_ASYNC:
        case NF_ONFI_SDR:
            if(xferMode == NF_XFER_MODE_0)
            {
                // FLASH_CLK=FC_CLK = 100MHz
                NFIF_SetNfifClockFrequency(100);
            }
            else
            {
                // FLASH_CLK=FC_CLK = 200MHz
                NFIF_SetNfifClockFrequency(200);
            }
            NFIF_SetClkSel( 0 );    // Normal setting
            break;
        case NF_ONFI_NVDDR:
        case NF_JEDEC_TOGGLE1:
            if((xferMode == NF_XFER_MODE_0) || (xferMode == NF_XFER_MODE_4))
            {
                // FLASH_CLK=FC_CLK = (166MHz)
                NFIF_SetNfifClockFrequency(166);
            }
            else if((xferMode == NF_XFER_MODE_1) || (xferMode == NF_XFER_MODE_3))
            {
                // FLASH_CLK=FC_CLK = 133MHz
                NFIF_SetNfifClockFrequency(133);
            }
            else
            {
                // FLASH_CLK=FC_CLK = 200MHz
                NFIF_SetNfifClockFrequency(200);
            }
            NFIF_SetClkSel( 0 );    // Normal setting
            break;
        case NF_ONFI_NVDDR2:
        case NF_JEDEC_TOGGLE2:
            if((xferMode == NF_XFER_MODE_0) || (xferMode == NF_XFER_MODE_2))
            {
                // FC_CLK = 133MHz
                NFIF_SetNfifClockFrequency(133);
                NFIF_SetClkSel( 0 );    // Normal setting
            }
            else if(xferMode == NF_XFER_MODE_1)
            {
                // FLASH_CLK=FC_CLK = 200MHz
                NFIF_SetNfifClockFrequency(200);
                NFIF_SetClkSel( 0 );    // Normal setting
            }
            else if(xferMode == NF_XFER_MODE_3)
            {
                // FLASH_CLK=FC_CLK = 160MHz
                NFIF_SetNfifClockFrequency(160);
                NFIF_SetClkSel( 0 );    // Normal setting
            }
            else if(xferMode == NF_XFER_MODE_4)
            {
                // FLASH_CLK=200MHz, FC_CLK = 100MHz
                NFIF_SetNfifClockFrequency(200);
                NFIF_SetClkSel( 1 );    // Divide setting
            }
            else if(xferMode == NF_XFER_MODE_5)
            {
                // FLASH_CLK=266MHz, FC_CLK = 133MHz
                NFIF_SetNfifClockFrequency(266);
                NFIF_SetClkSel( 1 );    // Divide setting
            }
            else if(xferMode == NF_XFER_MODE_6)
            {
                // FLASH_CLK=333MHz, FC_CLK = 166MHz
                NFIF_SetNfifClockFrequency(333);
                NFIF_SetClkSel( 1 );    // Divide setting
            }
            else
            {
                // FLASH_CLK=400MHz, FC_CLK = 200MHz
                #if (TOGGLE_533 == TRUE)
                NFIF_SetNfifClockFrequency(533);
                #else
                NFIF_SetNfifClockFrequency(400);
                #endif
                NFIF_SetClkSel( 1 );    // Divide setting
            }
            break;
        default:
            E_PRINTF(cEcNfifGeneral, "Couldn't find interface speed, use 100Mhz.\n");
            NFIF_SetNfifClockFrequency(100);
            NFIF_SetClkSel( 0 );    // Normal setting
            break;
    }
}
//-----------------------------------------------------------------------------
/**
    Updata Flash DLL to support ONFI_NVDDR2/JEDEC_TOGGLE2.

    @param[in]  pPhyInfo             NAND PHY information pointer

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t NFIF_UpdateFlashDll(NfPhyInfo_t *pPhyInfo)
{
    uint32_t ch;
    volatile DllCtrl1ForChan0_t *pdllCtrl1ForChan0;
    volatile DllCtrl2ForChan0_t *pdllCtrl2ForChan0;
    //volatile DllMonitorCh0_t *pdllMonitorCh0;

    pdllCtrl1ForChan0 = (DllCtrl1ForChan0_t *)&rNandFlashIf.dllCtrl1ForChan0.all;
    pdllCtrl2ForChan0 = (DllCtrl2ForChan0_t *)&rNandFlashIf.dllCtrl2ForChan0.all;
    //pdllMonitorCh0 = (DllMonitorCh0_t *)&rNandFlashIf.dllMonitorCh0.all;

    for (ch = 0; ch < gNfInfo.numChannels; ch++)
    {
        pdllCtrl1ForChan0->b.RST_CH0 = 0;
        pdllCtrl1ForChan0->b.RST_CH0 = 1;
        pdllCtrl1ForChan0->b.RST_CH0 = 0;


        // Polling LOCK status
        // Note: WA for Eldora
        // PLL Lock status doesn't update correctly for CH4/5/6/7
        // So doesn't wait Lock Done, then add more delay
      #if 0
        error = HalRci_WaitValueU16((uint32_t)pdllMonitorCh0, B_8, B_8, 100);
        if (error != cEcNoError)
        {
            D_PRINTF("NFIF_DLL_TIME_OUT = CH%1X\n", ch);
            return cEcTimeOut;
        }
      #endif
        HalTimer_DelayUsec(100);

        pdllCtrl2ForChan0->b.PHSEL0_CH0 = pPhyInfo->PHSEL0[ch];
        pdllCtrl2ForChan0->b.PHSEL1_CH0 = pPhyInfo->PHSEL1[ch];

        pdllCtrl1ForChan0->b.UPDATE_EN_CH0 = 0;
        HalTimer_DelayUsec(2);    // Delay need larger than 64 FCLK clock cycles
        pdllCtrl1ForChan0->b.UPDATE_EN_CH0 = 1;
        HalTimer_DelayUsec(2);    // Delay need larger than 17 FCLK clock cycles
        pdllCtrl1ForChan0->b.UPDATE_EN_CH0 = 0;

        pdllCtrl1ForChan0+=2;
        pdllCtrl2ForChan0+=2;
        //pdllMonitorCh0++;
    }

    return cEcNoError;
}
//-----------------------------------------------------------------------------
/**
    Direct mode - Set word data to data port.
**/
//-----------------------------------------------------------------------------
static void NFIF_DmSetAddrDataPort(uint16_t word)
{
    rNandFlashIf.FLASH_ADDR_DATA_DIR = word;
}

//-----------------------------------------------------------------------------
/**
     Direct mode - Get flash direct access Address/Data Port.
**/
//-----------------------------------------------------------------------------
static uint16_t NFIF_DmGetAddrDataPort(void)
{
    return rNandFlashIf.FLASH_ADDR_DATA_DIR;
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - set device and channel number.

    @param[in]  pAddr      Phyiscal NAND Flash address pointer
                pAddr->ch       channel number
                pAddr->dev     device number
**/
//-----------------------------------------------------------------------------
static void NFIF_ImSetDeviceChannel(Pna_t* pAddr)
{
    HalNfif_SetFlashOperationMode(INDIRECT);

    rNandFlashIf.flashDirectIndAccessCfg.b.DEVICE_DIR_IND = pAddr->dev;
    rNandFlashIf.flashDirectIndAccessCfg.b.CHANNEL_DIR_IND = pAddr->ch;
}
//-----------------------------------------------------------------------------
/**
    Indirect mode - reset FIFO pointer.
**/
//-----------------------------------------------------------------------------
static void NFIF_ImResetFifo(void)
{
    rNandFlashIf.flashIndAccessCfg.b.FIFO_RESET = 1;
    rNandFlashIf.flashIndAccessCfg.b.FIFO_RESET = 0;
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - write dword data

    @return data from port
**/
//-----------------------------------------------------------------------------
static void NFIF_ImWriteDataU32(uint32_t data)
{
    *(uint32_t *)(&rNandFlashIf.FLASH_DATA_IND_15_0) = data;
}

//-----------------------------------------------------------------------------
/**
    Initialize NAND PHY Pad Pull-up/down setting.
**/
//-----------------------------------------------------------------------------
static void NFIF_InitialPadPullUpDown(void)
{
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_DQ   = 0;   // Pull Down for DQ Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_DQ     = 0;   // Pull Up for DQ Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_DQSC = 0;   // Pull Down for DQS_C Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_DQSC   = 1;   // Pull Up for DQS_C Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_DQS  = 1;   // Pull Down for DQS Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_DQS    = 0;   // Pull Up for DQS Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_AC   = 1;   // Pull Down for CLE/ALE/CE Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_AC     = 0;   // Pull Up for CLE/ALE/CE Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_REC  = 1;   // Pull Down for RE_C Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_REC    = 0;   // Pull Up for RE_C Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_REN  = 0;   // Pull Down for RE_N Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_REN    = 1;   // Pull Up for RE_N Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_WP   = 0;   // Pull Down for WP Pad of All channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_WP     = 1;   // Pull Up for WP Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_DOWN_WEN  = 0;   // Pull Down for WE_N Pad of All Channels.
    mNandFlashIf.padPullUpAndPullDnCtrl.b.PULL_UP_WEN    = 1;   // Pull Up for WE_N Pad of All Channels.
}

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
//! @brief Host I/F Link layer API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"

// HAL
#include "HalCpu.h"
#include "HalRegConfig.h"
#include "HalPcieLink.h"
#include "RegPciePhy.h"
#include "RegPcie.h"
#include "RegPcieShadow.h"
#include "RegPcieCore.h"
#include "HalTimer.h"

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Compile options
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
static const uint32_t cPcieClockTimeOutUs = 4*1000*1000;   ///< PCIe clock timeout
static const uint32_t cPcieLinkTrainTimeOutUs = 2*1000*1000;
static const uint32_t cMaxLatencyScale = 4;            ///< 100B=4:1048576 nsec
static const uint32_t cMaxLatencyValue = 3;            ///< 3 :Scale(1048576nsec) * 3
static const uint32_t cPcieFlrTimeoutUs = 500 * 1000;
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void SetupPcieConfigForPm(void);
static void SetupPcieConfigForExpansionRom(void);
static void HalPcieLink_SendLtrMessage(uint16_t ms, uint16_t scale);
static void HalLink_StartTraining(void);
static void HalLink_Initialize(void);

void HalLink_SetupPcieConfig(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

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
    One-time initialization for Host I/F Link Layer H/W\n

    @param  initMode    initialization mode

    @return
**/
//-----------------------------------------------------------------------------
Error_t HalLink_OneTimeInit(InitBootMode_t initMode)
{
    if (HalLink_Reset(cResetHard)==cEcNoError)
    {
        //HalIrq_InitPcieInterrupt(); // setup PCIe interrupt to handle PREST ASAP
        HalCpu_EnableIrq();         // enable CPU interrupt to service PREST
    }

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Reset Host I/F Link Layer H/W\n

    @param  resetMode    reset mode
**/
//-----------------------------------------------------------------------------
Error_t HalLink_Reset(ResetMode_t resetMode)
{
    CpuCs_t  orgState;

    // Wait for pipe clock ready
    if (HalRci_WaitValueU32((uint32_t)&rPciePhy.laneStat1, BIT(0), BIT(0), cPcieClockTimeOutUs) != cEcNoError)
    {
        E_PRINTF(cEcTimeOut, "PCIe LINK FAILED - NO PCIe clock from host.\n");
        return(cEcTimeOut);
    }

    // Make Link_Reset a crtical section to prevent a re-entrance issue due to PCIe PERST while Link_Reset.
    BEGIN_CS(orgState);

    HalLink_Initialize();

    HalLink_ReadyToSendPmEnterL23(OFF);                         // 0xF0080000[21] APP_READY_ENTR_L23 enable.

    HalLink_SetupPcieConfig();

#if (CHK_FW_TYPE(FW_DNLD) == FALSE)
    HalLink_StartTraining();
#endif
    // Wait for training done
    //HalRci_WaitValueU32(0xF0080000, BIT(6)| BIT(7), BIT(6)| BIT(7), cPcieLinkTrainTimeOutUs);

    END_CS(orgState);

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Setup Pcie configuration for NVMe I/F
**/
//-----------------------------------------------------------------------------
static void SetupPcieConfigForNvme(void)
{
    rPciePf.revisionIdAndClassCmem.all = NVME_CLASS_CODE;
    rPciePf.revisionIdAndClassCmem.b.REV_ID = 1;

    rPcieShadow.bar1AndBarMsk.all = 0;              // BAR1 disabled

    rPciePf.baseAddr0.b.BAR0_TYPE = 2;              // setting 64 bit BAR 0 should be after BAR1 dsiabled

    rPcieShadow.bar0AndBarMsk.all = 0x3FFF;         // BAR 0 mask setup
    rPcieShadow.bar2AndBarMsk.all = 0;              // BAR 2,3,4,5 disabled
    rPcieShadow.bar3AndBarMsk.all = 0;
    rPcieShadow.bar4AndBarMsk.all = 0;
    rPcieShadow.bar5AndBarMsk.all = 0;

    // Inbound address translaton, region 0
    rPciePf.iatuIndex.all               = 0x80000000;   // region index 0
    rPciePf.iatuRegisterionNOsAddr.all  = 0xF5800000;   // NVMe register AHB base
    rPciePf.iatuRegisterionCtrl2.all    = 0xC0000000;   // BAR0 match mode

    // power management capability (use value in NVMe spec)
    {
        PowerMngCapabili_t pmCap;

        pmCap.all =  rPciePf.powerMngCapabili.all;
        pmCap.b.PME_SUPPORT = 0;
        pmCap.b.D2_SUPPORT = 0;
        pmCap.b.D1_SUPPORT = 0;
        pmCap.b.AUX_CURR = 0;
        pmCap.b.DSI = 0;

        rPciePf.powerMngCapabili.all = pmCap.all;
        rPciePf.powerMngCtrlAndStat.b.NSFRST= 1; // No Soft Reset

        //D_PRINTF("%X %X\n", rPciePf.powerMngCapabili.all, rPciePf.powerMngCtrlAndStat.all);
    }


    // MSI-X mode
    rPciePf.msiXCfg.b.MSI_X_TABLE_SIZE = HW_MAX_INT_VECTORS - 1;
    rPciePf.msiXTblAddr.all = 0x00002000;
    rPciePf.msiXPendingBitArray.all = 0x00003000;
    rPciePf.msiXCfg.b.CAP_POINTR = 0;

    // Enable AXI report error (stop xfer)
    rPciePf.axiBdgSlaveErrResponse.b.GLOBAL = 1;
    rPciePf.axiBdgSlaveErrResponse.b.Report_Axi_Err = 1;
}

//-----------------------------------------------------------------------------
/**
    Setup Pcie configuration for AHCI I/F
**/
//-----------------------------------------------------------------------------
static void SetupPcieConfigForAhci(void)
{
    // set class code
    rPciePf.revisionIdAndClassCmem.all = AHCI_CLASS_CODE;

    rPcieShadow.bar0AndBarMsk.all = 0;
    rPcieShadow.bar1AndBarMsk.all = 0;
    rPcieShadow.bar2AndBarMsk.all = 0;
    rPcieShadow.bar3AndBarMsk.all = 0;
    rPcieShadow.bar4AndBarMsk.all = 0;

    // Inbound address translation, region 0
    rPciePf.iatuIndex.all               = 0x80000000; // Inbound region 0
    rPciePf.iatuRegisterionNOsAddr.all  = 0x25800000; // AHCI register base address
    rPciePf.iatuRegisterionCtrl2.all    = 0xC0000500; // BAR5 match mode

    rPciePf.msiCapabiliHdr.b.A64B_CAP=0;    // MSI capability (64 bit address enable)
}

//-----------------------------------------------------------------------------
/**
    Setup Pcie configuration space for expansion ROM
**/
//-----------------------------------------------------------------------------
static void SetupPcieConfigForExpansionRom(void)
{

    D_PRINTF("[PCIe] OpRomState|%d\n", _OPROM_);
#if (_OPROM_ != OPROM_NONE)
//Norman_Mark    rPcieShadow.expansioRomBarAndBarMsk.all = 0xFFFF; // Enable Expansion ROM, size 64KB
    rPcieShadow.expansioRomBarAndBarMsk.all = 0x1FFFF; // Enable Expansion ROM, size 128KB //Norman update for 128K OPROM

    // region 1 - Expansion ROM BAR to SPI address
    rPciePf.iatuIndex.all               = 0x80000001;   // Inbound region 1
    rPciePf.iatuRegisterionNOsAddr.all  = HW_SPI_OPROM_ADDR;  // should be in SPI region (set proper SPI address for ROM image)
    rPciePf.iatuRegisterionCtrl2.all    = 0xC0000600;   // ROM(BAR6) match mode
#else
    rPcieShadow.expansioRomBarAndBarMsk.all = 0;        // disable expansion ROM
#endif
}

//-----------------------------------------------------------------------------
/**
    Setup Pcie configuration space
**/
//-----------------------------------------------------------------------------
void HalLink_SetupPcieConfig(void)
{

    rPciePf.gen3EqualizaCtrl.b.PHASE23_EXIT_MODE = 0;
    rPciePf.gen3EqualizaCtrl.b.EVAL_2MS_DIS = 1;

    rPciePf.lnkWidthAndSpeedChangeCtrl.b.N_LANES = 1;
    rPciePf.lnkWidthAndSpeedChangeCtrl.b.DIRECT_SC = 0; // disable direct speed change from device for better compability

    rPciePf.gen3Ctrl.b.ZRXDC_NCPL = 0;          // Disable timer for non-compliance

    rPciePf.gen3Ctrl.b.EIEOS_CNT = 1;           // disable EIEOS count reset
    rPciePf.lnkWidthAndSpeedChangeCtrl.b.DIRECT_SC = 0;
    //rPciePf.powerMngCapabili.b.NEXT_PTR = 0x70;


#if (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED)
    rPciePf.secondaryPciExpressExtCapabiliHdr.b.NEXT_OFFSET = 0x2B8;        // LTR_Cap
    rPciePf.latencyTolReportinExtCapabiliHdr.b.NEXT_OFFSET  = 0x2C0;         // L1substates_Cap
    rPciePf.l1SubstateExtCapabiliHdr.b.NEXT_OFFSET = 0;
#else
    rPciePf.latencyTolReportinExtCapabiliHdr.b.NEXT_OFFSET  = 0;
    rPciePf.secondaryPciExpressExtCapabiliHdr.b.NEXT_OFFSET = 0;            // LTR_Cap
#endif

#if (PCIE_LIMIT_SPEED == PCIE_GEN1)
    rPciePf.lnkCapabili.b.MAX_LINK_SPD = 1;
    rPciePf.lnkCtrl2AndLnkStat2.b.TRGT_LINK_SPD = 1;
#elif (PCIE_LIMIT_SPEED == PCIE_GEN2)
    rPciePf.lnkCapabili.b.MAX_LINK_SPD = 2;
    rPciePf.lnkCtrl2AndLnkStat2.b.TRGT_LINK_SPD = 2;
#endif

#if (PCIE_LIMIT_WIDTH == PCIE_X1)
    rPciePf.portLnkCtrl.b.LINK_CAPABLE = 1;           // X1 port config
    rPciePf.lnkWidthAndSpeedChangeCtrl.b.N_LANES = 1;
#elif (PCIE_LIMIT_WIDTH == PCIE_X2)
    rPciePf.portLnkCtrl.b.LINK_CAPABLE = 3;           // X2 port config
    rPciePf.lnkWidthAndSpeedChangeCtrl.b.N_LANES = 2;
#endif
    // set up payload size
    rPciePf.deviceCapabili.b.MAX_PAYLOAD = 0;         // 128B

    // PCI Header 0 : set device ID and vendor ID
    rPciePf.vendorIdAndDeviceId.all  = MAKE_U32(PCI_DEVICE_ID, PCI_VENDOR_ID);

    // Advanced Error capability and control
    {
        AdvancedErrCapabiliAndCtrl_t advErrCapCtrl;

        advErrCapCtrl.all = rPciePf.advancedErrCapabiliAndCtrl.all;
        advErrCapCtrl.b.ECRC_CHK_TX = 1;
        advErrCapCtrl.b.ECRC_GEN_TX_EN = 1;
        advErrCapCtrl.b.ECRC_CHCK = 1;
        advErrCapCtrl.b.ECRC_CHCK_RCV = 1;
        rPciePf.advancedErrCapabiliAndCtrl.all = advErrCapCtrl.all;
    }

    rPciePf.subsysteVendorIdAndSubsysteId.all = MAKE_U32(PCI_SUBSYS_ID, PCI_SUBSYS_VENDOR_ID);

    SetupPcieConfigForPm();

#if (_HOST_==HOST_NVME)
    SetupPcieConfigForNvme();
#else
    SetupPcieConfigForAhci();
#endif

    SetupPcieConfigForExpansionRom();
}

//-----------------------------------------------------------------------------
/**
    Setup Pcie configuration space for power management
**/
//-----------------------------------------------------------------------------
static void SetupPcieConfigForPm(void)
{
    // Device Capabilities
    // Broadcast to host Acceptable Latency for Transition from L1 to L0.
    rPciePf.deviceCapabili.b.L1_ACC_LATENCY = 7;    //No limit

    // Link Capabilities
    {
        LnkCapabili_t lnkCapabili;

        lnkCapabili.all = rPciePf.lnkCapabili.all;

        // Broadcast to host Clock Power Management capability
        lnkCapabili.b.CLK_PM = (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED) ? 1 : 0;                   //Clock Power Management support
        // Broadcast to host L1 capability
        lnkCapabili.b.ASPM_SUPT = (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED) ? 2 : 0;
        // Broadcast to host Latency from L0S to L0.
        lnkCapabili.b.L0S_EXIT_LTC = 7;             // More than 4us
        // Broadcast to host Exit Latency From L1.
        lnkCapabili.b.L1_EXIT_LTC = 6;              // 32us - 64us

        rPciePf.lnkCapabili.all = lnkCapabili.all;
    }

    // Device Capabilities 2
    // Set Latency Tolerance Reporting Mechanism Supported.
    rPciePf.deviceCapabili2.b.LTR_SUPT = (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED) ? 1 : 0;

    // Device Control 2 And Status 2
    // Disable Latency Tolerance Reporting. It shall be set by RC.
    rPciePf.deviceCtrl2AndStat2.b.LTR_EN = 0;

    // L1 Substate Capability
    {
        L1SubstateCapabili_t l1SubstateCapabili;

        l1SubstateCapabili.all = rPciePf.l1SubstateCapabili.all;

        // Broadcast to host L1 Sub State capability
        l1SubstateCapabili.b.L1PM_SPUT     = (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED) ? 1 : 0;    //L1 sub state support
        l1SubstateCapabili.b.PCPM_L11_SUPT = (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED) ? 1 : 0;    //PCI-PM L1.1 support
        l1SubstateCapabili.b.PCPM_L12_SUPT = (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED) ? 1 : 0;    //PCI-PM L1.2 support
        l1SubstateCapabili.b.ASPM_L11_SUPT = (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED) ? 1 : 0;    //ASPM L1.1 support
        l1SubstateCapabili.b.ASPM_L12_SUPT = (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED) ? 1 : 0;    //ASPM L1.2 support

        rPciePf.l1SubstateCapabili.all = l1SubstateCapabili.all;
        rPciePf.ackFrqAndL0sL1AspmCtrl.b.L1_ENTER_LTC = 3;
    }

    // L1 Substates Control 1	//Host will set following value, so disable
   /* {
        L1SubstateCtrl1_t l1SubstateCtrl1;

        l1SubstateCtrl1.all = rPciePf.l1SubstateCtrl1.all;

        // Broadcast to host LTR L1.2 Threshold
        l1SubstateCtrl1.b.L12_TH_SCALE = 2;  // 1024ns
        l1SubstateCtrl1.b.L12_TH_VAL   = 10; // 1024ns * 10

        rPciePf.l1SubstateCtrl1.all = l1SubstateCtrl1.all;
    }*/
}

#if (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED)
//-----------------------------------------------------------------------------
/**
    Send Ltr message
    @param[in]  value - Latency value (10bits)
                scale - 000B:1ns 001B:32ns 010B:1024ns 011B:32768ns 100B:1048576ns

    @return void
**/
//-----------------------------------------------------------------------------
static void HalPcieLink_SendLtrMessage(uint16_t value, uint16_t scale)
{
    uint32_t wdata = 0;

    rPcieCore.pciecoreGlobal0.b.RF0080000_31_RSVD = 1;      //Clear LTR request
    rPcieCore.pciecoreGlobal0.b.APP_LTR_MSG_REQ   = 0;
    // D_PRINTF("value=%d, scale=%d\n", value, scale);
    while (rPcieCore.pciecoreGlobal0.b.APP_LTR_MSG_REQ)
    {
        // Wait for Bit 23 is 0
    }

    if(scale < 6)
    {
        wdata = scale << 10;
        wdata |= scale << 26;
    }

    if(value < 0x400)
    {
        wdata |= value;
        wdata |= value << 16;
    }

    wdata |= 0x80008000; //BIT15 and BIT31 to Requirement=1
    D_PRINTF("send ltr message %x\n", wdata);

    rPcieCore.APP_LTR_MSG_LATENCY_31_0 = wdata;
    rPcieCore.pciecoreGlobal0.b.APP_LTR_MSG_REQ   = 1; //Request LTR message
}
//-----------------------------------------------------------------------------
/**
    LTR FW message request

    @param[in]  Firmware Vector Selection
    @return void
**/
//-----------------------------------------------------------------------------
void HalPcieLink_RequestToSendLtrMessage(LtrVectorSel_t ltrVectorId)
{
    HwTimerTick_t tick;
    uint32_t elapseTimeUsec;
    uint32_t timeOutUsec=3000;

    if(!rPcieCore.pciecoreCfg.b.CFG_LTR_M_EN) return;   // return here if LTR is NOT enabled.

    tick = HalTimer_ReadTick();
    do
    {
        // check electrical Idle and APP_REQ_EXIT_L1 conditions
        if ((rPciePhy.laneStat2.b.PHY_MAC_RXELECIDLE) && (!rPcieCore.pciecoreGlobal0.b.APP_REQ_EXIT_L1))
        {
            rPcieCore.pciecoreGlobal0.b.APP_REQ_EXIT_L1 = 1;
        }

        if (rPcieCore.pciecoreGlobal0.b.SMLH_LTSSM_STATE_5_0 == 0x11)
        {
            rPcieCore.pciecoreGlobal0.b.APP_REQ_EXIT_L1 = 1;
            break;
        }

        elapseTimeUsec = HalTimer_GetElapsedTimeUsec(tick);

    } while(elapseTimeUsec < timeOutUsec);

    rPcieCore.pciecoreLtrMsgCtrl.b.LTR_FW_VECTOR_SEL = ltrVectorId;     // 0xF00800DC[2:1]
    rPcieCore.pciecoreLtrMsgCtrl.b.LTR_FW_REQ        = 1;               // 0xF00800DC[0]

    ///< wait for LTR sent out
    HalRci_WaitValueU32((uint32_t)&rPcieCore.pciecoreLtrMsgCtrl.all, 0x0, 0x1, 1000);

    // Clear APP_REQ_EXIT_L1 - 0xF0080000[19]
    if (rPcieCore.pciecoreGlobal0.b.APP_REQ_EXIT_L1 == 1)
    {
        rPcieCore.pciecoreGlobal0.b.APP_REQ_EXIT_L1 = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Request to exit L1 mode

    @param[in]  onOff       turn On or Off
    @return void
**/
//-----------------------------------------------------------------------------
void HalPcieLink_RequestToExitL1(uint32_t onOff)
{
    HwTimerTick_t tick;
    uint32_t elapseTimeUsec;
    uint32_t timeOutUsec=3000;

    tick = HalTimer_ReadTick();

    do
    {
        // check electrical Idle and APP_REQ_EXIT_L1 conditions
        if ((rPciePhy.laneStat2.b.PHY_MAC_RXELECIDLE) && (!rPcieCore.pciecoreGlobal0.b.APP_REQ_EXIT_L1))
        {
            rPcieCore.pciecoreGlobal0.b.APP_REQ_EXIT_L1 = 1;
        }

        if (rPcieCore.pciecoreGlobal0.b.SMLH_LTSSM_STATE_5_0 == 0x11)
        {
            rPcieCore.pciecoreGlobal0.b.APP_REQ_EXIT_L1 = onOff;
            break;
        }

        elapseTimeUsec = HalTimer_GetElapsedTimeUsec(tick);

     }while(elapseTimeUsec < timeOutUsec);

}
#endif

//-----------------------------------------------------------------------------
/**
    Display link state
**/
//-----------------------------------------------------------------------------
void HalPcieLink_DisplayLinkState(void)
{
    PcieLinkState_t pcieLinkState;

    LaneStat0_t laneStat0;
    LaneStat1_t laneStat1;
    LaneStat3_t laneStat3;

    laneStat0.all = rPciePhy.laneStat0.all;
    laneStat1.all = rPciePhy.laneStat1.all;
    laneStat3.all = rPciePhy.laneStat3.all;

    D_PRINTF("PM_PU_PLL        : 0x%x\n", laneStat0.b.PM_PU_PLL);
    D_PRINTF("PM_PU_TX         : 0x%x\n", laneStat0.b.PM_PU_TX);
    D_PRINTF("PM_PU_RX         : 0x%x\n", laneStat0.b.PM_PU_RX);
    D_PRINTF("PM_PU_IVREF      : 0x%x\n", laneStat0.b.PM_PU_IVREF);
    D_PRINTF("PM_PU_SQ         : 0x%x\n", laneStat3.b.PM_PU_SQ);
    D_PRINTF("PM_TX_VCMHOLD_EN : 0x%x\n", laneStat0.b.PM_TX_VCMHOLD_EN);
    D_PRINTF("PM_STATE         : 0x%x\n", laneStat1.b.PM_STATE);
    D_PRINTF("LTSSM_STATE(0xF0080000 BIT 5-0)    : 0x%x\n", rPcieCore.pciecoreGlobal0.b.SMLH_LTSSM_STATE_5_0);

    pcieLinkState = HalLink_GetPcieLinkSts();

    //Link State
    switch (pcieLinkState)
    {
    case cPcieLinkL0:
        D_PRINTF("PCIE_LINK_ST : L0\n");
        break;

    case cPcieLinkL0s:
        D_PRINTF("PCIE_LINK_ST : L0s\n");
        break;

    case cPcieLinkL10:
        D_PRINTF("PCIE_LINK_ST : L1.0\n");
        break;

    case cPcieLinkL1WCLKREQ:
        D_PRINTF("PCIE_LINK_ST : L1.0 + ClkReq\n");
        break;

    case cPcieLinkL11:
        D_PRINTF("PCIE_LINK_ST : L1.1\n");
        break;

    case cPcieLinkL12:
        D_PRINTF("PCIE_LINK_ST : L1.2\n");
        break;

    case cPcieLinkL2:
        D_PRINTF("PCIE_LINK_ST : L2\n");
        break;

    default:
        D_PRINTF("PCIE_LINK_ST : ?\n");
        break;
    }

    if ((pcieLinkState == cPcieLinkL1WCLKREQ)||(pcieLinkState == cPcieLinkL11)||(pcieLinkState == cPcieLinkL12))
    {
        switch (laneStat1.b.PM_STATE)
        {
        case 0xD:
            D_PRINTF("PCIe Gen1\n");
            break;

        case 0x11:
            D_PRINTF("PCIe Gen2\n");
            break;

        case 0x25:
            D_PRINTF("PCIe Gen3\n");
            break;

        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Check whether the PCIe Link state is L1 w/CLK or L1.1 or L1.2 .\n

    @return the current PCIe Link state
**/
//-----------------------------------------------------------------------------
PcieLinkState_t HalLink_GetPcieLinkSts(void)
{
    PcieLinkState_t pcieLinkState = cPcieLinkOther;

    LaneStat0_t laneStat0;
    LaneStat3_t laneStat3;

    laneStat0.all = rPciePhy.laneStat0.all;
    laneStat3.all = rPciePhy.laneStat3.all;

    //Link State
    switch (rPcieCore.pciecoreGlobal0.b.SMLH_LTSSM_STATE_5_0)
    {
    case 0x11:
        pcieLinkState = cPcieLinkL0;
        break;

    case 0x12:
        pcieLinkState = cPcieLinkL0s;
        break;

    case 0x14://L1.?
        if ((laneStat0.b.PM_PU_PLL        == 0) &&
            (laneStat0.b.PM_PU_TX         == 0) &&
            (laneStat0.b.PM_PU_RX         == 0) &&
            (laneStat0.b.PM_PU_IVREF      == 1) &&
            (laneStat3.b.PM_PU_SQ         == 1) &&
            (laneStat0.b.PM_TX_VCMHOLD_EN == 1))
        {
            pcieLinkState = cPcieLinkL1WCLKREQ;
        }

        else if ((laneStat0.b.PM_PU_PLL        == 0) &&
                 (laneStat0.b.PM_PU_TX         == 0) &&
                 (laneStat0.b.PM_PU_RX         == 0) &&
                 (laneStat0.b.PM_PU_IVREF      == 0) &&
                 (laneStat3.b.PM_PU_SQ         == 0) &&
                 (laneStat0.b.PM_TX_VCMHOLD_EN == 1))
        {
            pcieLinkState = cPcieLinkL11;
        }

        else if ((laneStat0.b.PM_PU_PLL        == 0) &&
                 (laneStat0.b.PM_PU_TX         == 0) &&
                 (laneStat0.b.PM_PU_RX         == 0) &&
                 (laneStat0.b.PM_PU_IVREF      == 0) &&
                 (laneStat3.b.PM_PU_SQ         == 0) &&
                 (laneStat0.b.PM_TX_VCMHOLD_EN == 0))
        {
            pcieLinkState = cPcieLinkL12;
        }

        else
        {
            pcieLinkState = cPcieLinkL10;
        }
        break;

    case 0x15:
        pcieLinkState = cPcieLinkL2;
        break;

    default:
        break;
    }
    return pcieLinkState;
}

//-----------------------------------------------------------------------------
/**
    Initialize phy/pipe error counters for monitoring
    @param[in]  counterType - disparity, 8/10b, etc, refer to regPciePipe.h
                pulseWidth - sampling control pulse width 1-32

    @return void
**/
//-----------------------------------------------------------------------------
Error_t HalLink_SetErrorCounter(uint32_t counterType, uint32_t pulseLen)
{
    // TODO:

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Read out phy/pipe error counters for monitoring
    @param[in]  void

    @return counterValue that has been sampled
**/
//-----------------------------------------------------------------------------
uint32_t HalLink_ReadErrorCounter(void)
{
    // TODO:

    return 0;
}

#if CHK_CODE_TYPE(CODE_MAIN)
//-----------------------------------------------------------------------------
/**
    Dump PCIe link information
**/
//-----------------------------------------------------------------------------
void HalLink_DumpInfo(void)
{
    uint16_t PM_STATE = rPciePhy.laneStat1.b.PM_STATE;


    if (rPciePhy.laneStat1.b.PM_TXDCLK_PCLK_EN)
    {
        M_PRINTF("GEN:%u LANES:%u\n", rPciePf.lnkCtrlAndLnkStat.b.LINK_SPEED, rPciePf.lnkCtrlAndLnkStat.b.NEGO_LW);
    }
    else
    {
        M_PRINTF("No Link\n");
    }

    if( rPciePhy.laneStat1.b.PM_TXDCLK_PCLK_EN || ((PM_STATE!=0) && (PM_STATE!=2) && (PM_STATE!=0x15)) )
    {
        HalPcieLink_DisplayLinkState();
    }

    if (rPciePhy.laneStat1.b.PM_TXDCLK_PCLK_EN)
    {
        M_PRINTF("l1SubstateCapabili : %X\n", rPciePf.l1SubstateCapabili.all);
        M_PRINTF("l1SubstateCtrl1    : %X\n", rPciePf.l1SubstateCtrl1.all);
        M_PRINTF("lnkCapabili        : %X\n", rPciePf.lnkCapabili.all);
        M_PRINTF("ASPM_SUPT          : %2X\n", rPciePf.lnkCapabili.b.ASPM_SUPT);
        M_PRINTF("ASPM_CTRL          : %2X\n", rPciePf.lnkCtrlAndLnkStat.b.ASPM_CTRL);
    }
    else
    {
        M_PRINTF("CLK PM reguesting...\n");
    }

}
#endif

//-----------------------------------------------------------------------------
/**
    Handle NVMe Subsystem Reset at PCIe.

    @param[in]  void
    @return void
**/
//-----------------------------------------------------------------------------
void HalPcieLink_HandleNssr(void)
{
    D_PRINTF("NSSR\n");
    HalLink_Reset(cResetHard);
}

#if CHK_CODE_TYPE(CODE_MAIN)
//-----------------------------------------------------------------------------
/**
    Handle PCIe function level reset.
        To check which type of function received the FLR bit from host
            RF00800A8[24], CFG_FLR_PF_ACTIVE_INT
        Handle FLR -FW init the pcie function registers that has reset
        Assert APP_FLR_PF_DONE, RF0080000[29] = 1
        Wait until ACTIVE_STATUS = 0
            RF00800A8[8] = 0, eventually when reset is complete
        Clear done bit
            RF0080000[29] = 0, APP_FLR_PF_DONE
        Clear FLR_ACTIVE bit
            RF00800A8[24] = 1, CFG_FLR_PF_ACTIVE_INT

    @param[in]  void
    @return void
**/
//-----------------------------------------------------------------------------
void HalPcieLink_HandleFLR(void)
{
    PciecoreIs_t pcieCoreIs;

    // Assert PF FLR done
    rPcieCore.pciecoreGlobal0.b.APP_FLR_PF_DONE = 1;

    // wait until PF active status reflects reset done
    if (HalRci_WaitValueU32((uint32_t)&rPcieCore.pciecoreGlobal2, 0, BIT(8), cPcieFlrTimeoutUs) != cEcNoError)
    {
        E_PRINTF(cEcTimeOut, "Clear CFG_FLR_PF_ACTIVE_STATUS timeout\n");
        return;
    }

    HalLink_SetupPcieConfig(); // set default value

    // clear FLR done
    rPcieCore.pciecoreGlobal0.b.APP_FLR_PF_DONE = 0;

    pcieCoreIs.all = 0;
    pcieCoreIs.b.FLR_PF_ACTIVE = 1;
    rPcieCore.pciecoreIs.all = pcieCoreIs.all; // clear interrupt pending
}
#endif

//-----------------------------------------------------------------------------
/**
    Control the L2/L3 Ready status for sending a PM_Enter_L23 DLLP
    @param[in]  OnOff
    @return void
**/
//-----------------------------------------------------------------------------
void HalLink_ReadyToSendPmEnterL23(uint32_t onOff)
{
    rPcieCore.pciecoreGlobal0.b.APP_READY_ENTR_L23 = onOff;
}

//-----------------------------------------------------------------------------
/**
    Start link training
    @param[in]  void
    @return void
**/
//-----------------------------------------------------------------------------
static void HalLink_StartTraining(void)
{
    rPcieCore.pciecoreGlobal0.b.APP_LTSSM_ENABLE = 0;
    rPcieCore.pciecoreGlobal0.b.APP_LTSSM_ENABLE = 1;
}

//-----------------------------------------------------------------------------
/**
    initialize the PCIe Link
    @param[in]  void
    @return void
**/
//-----------------------------------------------------------------------------
static void HalLink_Initialize(void)
{
#if (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED)
    rPcieCore.pciecoreGlobal0.b.RF0080000_31_RSVD = 1;          // 0xF0080000[31] LTR Mechanism enable

    rPcieCore.pciecoreGlobal1.b.AUTO_L1_EXIT_EN = 1;            // B[31]  wake up link when AXI transactions are issued.
    rPcieCore.pciecoreGlobal1.b.AUTO_L1_EXIT_MSI_EN = 1;        // B[30]  for MSI interrupt (msi_req signal)

    // Enable Autonomous LTR Message for LTR Enable.
    rPcieCore.pciecoreLtrMsgCtrl.b.LTR_EN_EN = 1;               // 0xF00800DC[31]
    rPcieCore.pciecoreLtrMsgCtrl.b.LTR_L1_2_EN = 0;             // 0xF00800DC[30]
#endif

    /*** REFCLK auto detect mode enable***/
    rPciePhy.miscCtrl.b.CFG_REFCLK_DET_TYPE = (PCIE_REF_CLOCK_AUTO_DETECTION == ENABLE) ? 1:0;

#if (PCIE_SRIS == ENABLE)
    REG_U32(0xF0080000) |= BIT(17);    //APP_SRIS_MODE = 1. SRIS mode
#endif
}

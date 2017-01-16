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
//! @brief PCIe Interrupt request API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"
#include "SharePkg.h"
#include "HostPkg.h"

// REG
#include "RegPcie.h"
#include "RegPcieCore.h"
#include "RegArmVic.h"
#include "RegIntController.h"
#include "RegApb.h"
#include "RegPciePhy.h"

// HAL
#include "HalIrq.h"
#include "HalIrqSrc.h"
#include "HalPciePhy.h"
#include "HalPcieLink.h"
#include "HalPciePm.h"
#include "HalPcieErr.h"

// APL
#include "HldNvme.h"

#if CHK_CODE_TYPE(CODE_MAIN)
    #include "Cache.h"
#endif

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void HandleOtherBlockResetsAfterPerst(void);
static void HandlePcieIrq(IrqHandler_t irqHandler[]);
       void HandlePcieResetNonIrq(IrqHandler_t irqHandler[]);
       void HandlePCIeResetIrq(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
extern uint32_t gPcieRstRedirected;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern void HalLink_SetupPcieConfig(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    One-time initialization for PCIe Interrupt\n

    @return
**/
//-----------------------------------------------------------------------------
Error_t HalIrq_InitPcieInterrupt(void)
{
    PciecoreIe_t pcieCoreIe;

    if (gPcieRstRedirected)
    {
        VIC_SET_HANDLER(0, cIrqSourcePcieVRstN, HandlePcieResetNonIrq);    // Register PCIE Reset irq handler
    }
    else
    {
        VIC_SET_HANDLER(0, cIrqSourcePcieVRstN, HandlePCIeResetIrq);       // Register PCIE Reset irq handler
    }

    VIC_SET_HANDLER(0, cIrqSourcePcie, HandlePcieIrq);              // Register PCIE irq handler

    VIC_ENABLE(0, BIT(cIrqSourcePcieVRstN)|BIT(cIrqSourcePcie));    // Enable IRQ for PCIe Reset and PCIe

    // Mask an obsolete PCIe Interrupt mask register, it is only for an Eldora 1.0.
    rPcieCore.pciecoreGlobal3.all = 0xFFFFFFFF;

    // PCIe Interrupt enable
    pcieCoreIe.all = 0;
    pcieCoreIe.b.LINK_REQ_RST_NOT = 1;
    pcieCoreIe.b.FLR_PF_ACTIVE    = 1;
    pcieCoreIe.b.RADM_PM_TURNOFF  = 1;

  #if (PCIE_REF_CLOCK_AUTO_DETECTION == ENABLE)
    pcieCoreIe.b.LTSSM_L1_ENTER   = 1;
    pcieCoreIe.b.LTSSM_L1_EXIT    = 1;
  #endif

    rPcieCore.pciecoreIe.all = pcieCoreIe.all;

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Interrupt Request handler to emulate vectored interrupt.\n
    This code decodes PCIE DPMU interrupt and calls registered handler.\n

    @param[in]  irqHandler  interrupt request handler table
**/
//-----------------------------------------------------------------------------
static void HandlePcieDpmuIrq(IrqHandler_t irqHandler[])
{
    D_PRINTF("DP ");
}

//-----------------------------------------------------------------------------
/**
    Interrupt Request handler for PCIe events.\n
    This code decodes PCIE interrupt and calls registered handler.\n

    @param[in]  irqHandler  interrupt request handler table
**/
//-----------------------------------------------------------------------------
static void HandlePcieIrq(IrqHandler_t irqHandler[])
{
    PciecoreIs_t pcieCoreIs;

    while ((pcieCoreIs.all = rPcieCore.pciecoreIs.all & rPcieCore.pciecoreIe.all) != 0)
    {
        rPcieCore.pciecoreIs.all = pcieCoreIs.all; // clear interrupt

        //D_PRINTF("PCIE_INT STATUS: %8X\n", pcieCoreIs.all);

        if (pcieCoreIs.b.LINK_REQ_RST_NOT) // The Falling Edge Detection of Link Requester Edge Detector
        {
            M_PRINTF("\n[PCIE_LINK_REQ_RST_NOT]\n");
            Debug_DumpMemU32((uint32_t)0xF0000100, 0x4);
            Debug_DumpMemU32((uint32_t)0xF0000000, 0x1);
            Debug_DumpMemU32((uint32_t)0xF00800D4, 0x1);
            Debug_DumpMemU32((uint32_t)0xF00800F8, 0x2);
            HalLink_SetupPcieConfig();
        }

        if (pcieCoreIs.b.FLR_PF_ACTIVE)      // FLR is actuated by the host on PF
        {
            M_PRINTF("\n[FLR]\n");
            // FLR is activated at PF, handle it
            HalPcieLink_HandleFLR();
            irqHandler[cIrqHostReset](cNvmeControllerResetClr);
        }

        if ((pcieCoreIs.b.LTSSM_L1_ENTER) || (pcieCoreIs.b.LTSSM_L1_EXIT))
        {
          #if (PCIE_REF_CLOCK_AUTO_DETECTION == ENABLE)
            rPcieCore.pciecoreRefclkRx.b.REFCLK_RX_OFFSET_EN = (rPcieCore.pciecoreGlobal0.b.SMLH_LTSSM_STATE_5_0 == 0x14) ? 1 : 0;
          #endif
        }

        if (pcieCoreIs.b.RADM_PM_TURNOFF)
        {
            HalLink_ReadyToSendPmEnterL23(ON);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    PERST Handle function
**/
//-----------------------------------------------------------------------------
void HandlePCIeResetIrq(void)
{
    extern void HalNvme_Reset(ResetMode_t resetMode);

#if CO_REBOOT_DEVICE_ON_PERST
    HalCpu_Reboot(0);
#endif

    M_PRINTF("\n[RSET] PCIE_RESET\n");

#if (CO_RESET_PCIE_PHY_ON_PERST == FALSE)
    REG_U32(0xF0112090) |= BIT(4);      // botton reset
#endif

    AplHost_ReleaseDataCache();

    // wait for PCIe RESET release
    HalPhy_WaitForPerstRelease(10*1000*1000);

#if (CO_RESET_PCIE_PHY_ON_PERST == FALSE)
    REG_U32(0xF0112090) &= ~BIT(4);     // botton reset
#endif

#if CO_RESET_PCIE_PHY_ON_PERST
    HalPhy_Reset(cResetHard);
#endif

#if _HOST_ == HOST_NVME  // reset NVMe before PCIe link reset
    HalNvme_Reset(cResetHard);
#endif

    HalLink_Reset(cResetHard);

    HalIrq_InitPcieInterrupt(); // Set PCIe Interrupt Enable bits again after PCIe Reset

    VIC_ENABLE(0, BIT(cIrqSourcePcieVRstN));    // Enable IRQ for PCIe Reset and PCIe
}

//-----------------------------------------------------------------------------
/**
    Interrupt handler for PERST.\n

    @param[in]  irqHandler  interrupt request handler table
**/
//-----------------------------------------------------------------------------
void HandlePcieResetNonIrq(IrqHandler_t irqHandler[])
{
    VIC_DISABLE(0, BIT(cIrqSourcePcieVRstN));    // Enable IRQ for PCIe Reset and PCIe

#if _HOST_ == HOST_NVME
    irqHandler[cIrqHostReset](cNvmePCIeReset);
#endif
}

//-----------------------------------------------------------------------------
/**
    Reset other blocks post PERST#.\n

    @param[in]  none
**/
//-----------------------------------------------------------------------------
static void HandleOtherBlockResetsAfterPerst(void)
{
    // now that PERST# has reset the PCIe core and phy by HW,
    // reset AHCI, NVMe, PCIE_DMA, DMAC, SEQUENCER by toggling 0xF0115090[11]
    rApb.rstCtrl.b.RST_SYSAXI = 1;
    rApb.rstCtrl.b.RST_SYSAXI = 0;
}

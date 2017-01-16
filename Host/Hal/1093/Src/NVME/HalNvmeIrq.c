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
//! @brief NVMe controller handling code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"
#include "SharePkg.h"
#include "RegNvmePkg.h"

// REG
#include "RegCp15.h"

// HAL
#include "HalCpu.h"
#include "HalIrq.h"
#include "HalNvmeXfer.h"
#include "HalTimer.h"

// APL
#include "HostBase.h"
#include "HldHost.h"
#include "HldNvme.h"
#include "Kernel.h"
#include "Cache.h"
#include "NvmeCmds.h"

// Code Section
#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define NVME_RST_GROUP      (BIT(0) | BIT(1) | BIT(2) | BIT(4))      // CC CLR, CC SET, Shutdown, Sub-Sys RST
#define NVME_ERR_GROUP      (BIT(24) | BIT(26) | BIT(27) | BIT(28) | BIT(30))
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static uint16_t GetXferDoneAdminCommand(void);
void HalNvme_InfoDump(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
uint32_t CounterInCpuCycle = 0;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern void Host_DumpInfo(U32 allFlag);
extern void HalNvme_HandlePktmError(IrqHandler_t irqHandler[]);
extern void HalNvme_HandleStsmError(IrqHandler_t irqHandler[]);
extern void HalNvme_HandleCmdfError(IrqHandler_t irqHandler[]);
extern void HalNvme_HandleDatmError(IrqHandler_t irqHandler[]);
extern void HalNvme_HandleCregError(IrqHandler_t irqHandler[]);
//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    NVMe Interrupt service routine

    @param[in]  param       software interrupt handler table.
**/
//-----------------------------------------------------------------------------
void HalNvme_IrqHandler(IrqHandler_t irqHandler[])
{
    CentralCtrlModuleIntr_t centralCtrlModuleIntr;
    uint32_t xferDoneParam = 0;
    uint16_t cmdIndex = 0xFFFF; // Invalid Command Id

    while ((centralCtrlModuleIntr.all = rNvmeCentralCtrl.centralCtrlModuleIntr.all & rNvmeCentralCtrl.centralCtrlModuleIntrEn.all) != 0)
    {
        rNvmeCentralCtrl.centralCtrlModuleIntr.all = centralCtrlModuleIntr.all; // clear interrupt

        //D_PRINTF("INT STATUS:%X\n", centralCtrlModuleIntr.all);

        if (centralCtrlModuleIntr.all & NVME_RST_GROUP)
        {
            D_PRINTF("[RSET] INT STATUS:%X\n", centralCtrlModuleIntr.all);
            if(centralCtrlModuleIntr.b.CC_EN_CLR) // CC.EN = 0 (from 1)
            {
                irqHandler[cIrqHostReset](cNvmeControllerResetClr);

                D_PRINTF("\n[RSET] CC_EN_CLR\n");
                D_PRINTF("[RSET] CCBM GP_CNT[%08d]\n", COMPLETED_GROUP_BM());

                D_PRINTF("[RSET] ReqJob:%d ReqDone:%d\n", gHostIfMgr.info.mediaReqJobCnt, gHostIfMgr.info.mediaReqDoneCnt);

                //HalNvme_InfoDump();
            }

            if (centralCtrlModuleIntr.b.CC_EN_SET) // CC.EN = 1 (from 0)
            {
                //M_PRINTF("CC_EN\n");
                irqHandler[cIrqHostReset](cNvmeControllerResetSet);

                //HalNvme_InfoDump();
            }

            if(centralCtrlModuleIntr.b.CC_SHDN_SET) // shutdown notification
            {
                if (!(gResetFlag & BIT(cNvmeControllerResetClr)))
                {
                    if (rNvmeController.controller[0].CC.b.SHN!=0)
                    {
                        rNvmeController.controller[0].CSTS.b.SHST = 1;   // Shutdown occurred.
                        irqHandler[cIrqHostShutdown](rNvmeController.controller[0].CC.b.SHN);
                    }
                }
            }

            if (centralCtrlModuleIntr.b.SUB_SYS) // sub-system reset
            {
                irqHandler[cIrqHostReset](cNvmeSubsystemReset);
            }
        }

        if (centralCtrlModuleIntr.b.CSRM_G_STS) // command in (Admin, Io)
        {
            rNvmeCentralCtrl.centralCtrlModuleIntrEn.b.CSRM_G_STS_EN = 0; // disable interrupt, enabled when FIFO is empty
            //irqHandler[cIrqHostCommandIn](centralCtrlModuleIntr.b.CSRM_G_STS);
        }

        if (centralCtrlModuleIntr.b.DATM_G_STS & 1)  // Admin Transfer Done Bitmap (ATDBM)
        {
            while ((cmdIndex = GetXferDoneAdminCommand()) != 0xFFFF)
            {
                // Clear admin transfer done bitmap in advance because it is level-triggered.
                rNvmeDataMngr.dataManagerStat0.b.DATM_ACMD_ATDBM = BIT(cmdIndex);  // W1C
                irqHandler[cIrqHostXferDone](GET_ADM_XFER_PARAM_FROM_INDEX(cmdIndex));

            #if FIX_NVME_IOCMD_DWORD_SIZE_TRANSFER == ENABLE
                if (cmdIndex==cReservedAdminIndexForIoCmd) // release admin command index once transfer done
                {
                    HalNvme_ReleaseAdminCommand(cmdIndex);
                }
            #endif
            }
        }

        // A Status pending signal can be asserted like an interrupt using BSRM_CCBM_CNT
        if (centralCtrlModuleIntr.b.BSRM_G_STS)
        {
            HalNvme_ProcessCompletedIoCommands();
        }

        if (centralCtrlModuleIntr.all & NVME_ERR_GROUP)
        {
            if(centralCtrlModuleIntr.b.STSM_ERR_STS)
            {
                HalNvme_HandleStsmError(irqHandler);
            }

            if(centralCtrlModuleIntr.b.PKTM_ERR_STS)
            {
                HalNvme_HandlePktmError(irqHandler);
            }

            // command fetch error
            if(centralCtrlModuleIntr.b.CMDF_ERR_STS)
            {
                HalNvme_HandleCmdfError(irqHandler);
            }

            if(centralCtrlModuleIntr.b.DATM_ERR_STS)
            {
                HalNvme_HandleDatmError(irqHandler);
            }

            if(centralCtrlModuleIntr.b.CREG_ERR_STS)
            {
                HalNvme_HandleCregError(irqHandler);
            }
        }
    }
}
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
/**
    Enable or disable NVMe interrupt.

    @param[in]  sIrqId       software IRQ ID.
    @param[in]  state        ENABLE or DISABLE IRQ

    @return previous IRQ state
**/
//-----------------------------------------------------------------------------
uint32_t HalNvme_IrqEnDisable(IrqId_t irqId, uint32_t state)
{
    bool orgState = DISABLE;

    switch(irqId)
    {
    case cIrqHostCommandIn: // enable both Admin and I/O
        orgState = rNvmeCentralCtrl.centralCtrlModuleIntrEn.b.CSRM_G_STS_EN ? ENABLE : DISABLE;
        if(state == DISABLE)
        {
            rNvmeCentralCtrl.centralCtrlModuleIntrEn.b.CSRM_G_STS_EN = 0;
        }
        else
        {
            rNvmeCentralCtrl.centralCtrlModuleIntrEn.b.CSRM_G_STS_EN = 0x3;
        }
        break;

    case cIrqHostXferDone:
#if (CO_AUTO_CABM_CLEAR!=ENABLE) // do not support xfer done call back if AUTO_CABM is enabled, this will be revisted later
        orgState = rNvmeCentralCtrl.centralCtrlModuleIntrEn.b.BSRM_G_STS_EN;// When AutoCABMClearEn=1: CD=0(Io Command Completion Done), CD=1(IO Transfer Done)
        rNvmeCentralCtrl.centralCtrlModuleIntrEn.b.BSRM_G_STS_EN = state;   // When AutoCABMClearEn=0: Use CCBM Counter
#endif
        break;

    case cIrqHostReset:
        orgState = rNvmeCentralCtrl.centralCtrlModuleIntrEn.b.CCEN_CLR_EN;
        rNvmeCentralCtrl.centralCtrlModuleIntrEn.b.CCEN_CLR_EN = state;
        break;

    case cIrqHostShutdown:
        orgState = rNvmeCentralCtrl.centralCtrlModuleIntrEn.b.CCSHN_SET_EN;
        rNvmeCentralCtrl.centralCtrlModuleIntrEn.b.CCSHN_SET_EN = state;
        break;

    case cIrqHostIfPm:
        break;

    case cIrqHostIfError:
        break;
    }

    return orgState;
}

//-----------------------------------------------------------------------------
/**
    NVMe Central Control Module interrupt Enable.

    @param[in]  none
    @return     none
**/
//-----------------------------------------------------------------------------
void HalNvme_CentralModuleInterruptEnable(void)
{
    CentralCtrlModuleIntrEn_t intpEn;

    // NVMe Interrupt enable
    intpEn.all = 0;
    intpEn.b.CCEN_CLR_EN = 1;
    intpEn.b.CCEN_SET_EN = 1;
    intpEn.b.CCSHN_SET_EN = 1;
    intpEn.b.SUB_SYS_EN = 1;
    intpEn.b.DATM_G_STS_EN = 1;   // Admin Transfer Done Bitmap (ATDBM)
    intpEn.b.STSM_ERR_STS_EN = 1;
    intpEn.b.CMDF_ERR_STS_EN = 1;
    intpEn.b.DATM_ERR_STS_EN = 1;
    intpEn.b.CREG_ERR_STS_EN = 1;
    intpEn.b.PKTM_ERR_STS_EN = 1;

    rNvmeCentralCtrl.centralCtrlModuleIntrEn.all = intpEn.all;
}

//-----------------------------------------------------------------------------
/**
    Get 1st transfer done bitmap for an admin command

    @return command Index   0 to 7 is valid.
                            0xFFFF (-1) is invalid (In case of bitmap is 0x00).
**/
//-----------------------------------------------------------------------------
static uint16_t GetXferDoneAdminCommand(void)
{
    return (31 - __clz(rNvmeDataMngr.dataManagerStat0.b.DATM_ACMD_ATDBM & NvmeHba.acmdInUseBitmap));
}


//-----------------------------------------------------------------------------
/**
    Information dump

**/
//-----------------------------------------------------------------------------
void HalNvme_InfoDump(void)
{
    if (gHostIfMgr.info.mediaReqJobCnt != gHostIfMgr.info.mediaReqDoneCnt)
    {
        BEGIN_HOST_DBG_DUMP(DBG_DUMP_ALL_INFO_RST);
            Host_DumpInfo(TRUE);
        END_HOST_DBG_DUMP;
    }
}

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
#include "NvmeCmdPkg.h"

// REG
#include "RegClockPLL.h"
#include "RegPcie.h"
#include "RegPcieCore.h"
#include "RegArmVic.h"
#include "RegIntController.h"

// HAL
#include "HalCpu.h"
#include "HalIrqSrc.h"
#include "HalIrq.h"
#include "HalRegConfig.h"
#include "HalNvmeXfer.h"
#include "HalPcieLink.h"
#include "HalGpio.h"

// APL
#include "HldNvme.h"
#include "HldHost.h"
#include "IDS.h"
#include "Cache.h"
#include "NvmeDirective.h"
// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
static const uint32_t cHwCommandSramInit=0x01;       // HW command SRAM initialize
static const uint32_t cFwCommandSramInit=0x02;       // FW command SRAM initialize
static const uint32_t cHwControlRegInit=0x04;        // HW NVMe Controller register initialize
static const uint32_t cBitmapSramInit=0x08;          // HW SRAM Bitmap initialize
static const uint32_t cHwPartialControlRegInit=0x10; // HW NVMe controller register initialize partially (preserve value for Reset)
static const uint16_t cAdminSsdIndexBase = 16;      // SSD base index for admin command (one per command)
static const uint16_t cIoSsdIndexBase    = 32;      // SSD base index for I/O commnad (one per command)
static const uint32_t cSubSegBlockCount  = 32768;   // Sub-segment block size,

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define DECREASE_CCBM_COUNTER(N)   (rNvmeBitmapSram.bitmapSramCmdCompletiBitmapCntrDecrease.all = N) // FW decrease CCBM counter

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief Save NVMe registers before deep power down
typedef struct
{
    QueueArbFuncArbCfg_t                        queueArbFuncArbCfg[3];                      // RF5014030h-4038h
    StatManagerIntrVectorValidGroup_t           statManagerIntrVectorValidGroup0;           // RF5015020h
    StatManagerIntrVectorCoalesciDisGroup_t     statManagerIntrVectorCoalesciDisGroup0;     // RF5015080h
    StatManagerIntrAggregatThr_t                statManagerIntrAggregatThr[3];              // RF5015100h-5108h
    NvmeDataMngr_t                              nvmeDataManger;
} SaveNvmeReg_t;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void     HalNvme_InitControllerRegisters(uint16_t functionId);
static void     HalNvme_ShutdownInternalInterfaces(void);
static void     HalNvme_InitMemories(uint32_t flags);
static void     HalNvme_InitPerFunc(void);
void            HalNvme_InitNvmeIrq(void);
static void     HalNvme_DeleteAllQueues(void);
static void     HalNvme_InitIds(bool initFlag);
static Error_t  HalNvme_SaveContext(void);
static Error_t  HalNvme_RestoreContext(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
#if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
static NvmePowerMode_t  currentNvmePowerMode;
static SaveNvmeReg_t    saveNvmeRegister;
#endif

NvmeHba_t               NvmeHba;                                    ///< NVMe host bus adapter
NvmeAdminXfer_t         sNvmeAdminXfer[HW_MAX_ADMIN_CMD_INDEX];     ///< NVMe Hal Admin Cdb format
NvmeIoXfer_t            sNvmeIoXfer[HW_MAX_IO_CMD_INDEX];           ///< NVMe Hal Io Cdb format
NvmeAbortInfo_t         gNvmeAbortCmdInfo;
CirBuf_t                sCqRepostFifo;                              ///< Manual CQ Repost Fifo using a circular buffer
//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
extern IrqHandler_t     sIrqHandler[cIrqNumOfId];
extern NvmeHwIoCmdDw0_t ghwIocmdDw0_ids[NVME_NUM_OF_NAMESPACES];
extern NvmeHwIoCmdDw1_t ghwIocmdDw1_ids[NVME_NUM_OF_NAMESPACES];
extern NvmeHwIoCmdDw0_t ghwIocmdDw0_ssd[NVME_NUM_OF_NAMESPACES];
extern NvmeHwIoCmdDw1_t ghwIocmdDw1_ssd[NVME_NUM_OF_NAMESPACES];

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
#if _CPU_==CPU_ID0
extern Error_t HalIrq_InitPcieInterrupt(void);
extern void HalNvme_IrqHandler(IrqHandler_t irqHandler[]);
extern void HalNvme_HandleStsmError(IrqHandler_t irqHandler[]);
extern void HalNvme_CentralModuleInterruptEnable(void);
#endif

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    One-time initialization for NVMe contoller.

    @param[in]  initMode    initialization mode
**/
//-----------------------------------------------------------------------------
Error_t HalNvme_OneTimeInit(InitBootMode_t initMode)
{
    // Set NVMe mode
    rClockPll.powerCtrl2.b.FORCE_AHCI_NVME_SEL = 1;
    rClockPll.powerCtrl2.b.AHCI_NVME_SEL = 1;

    // Only Cold Boot mode can clear All NVMe Information
    if (initMode == cInitBootCold)
    {
        // clear all memories and registers
        HalNvme_InitMemories(cHwCommandSramInit|cFwCommandSramInit|cHwControlRegInit|cBitmapSramInit);
    }

    IDS_OneTimeInit(initMode);  // Use HWLL (Hardware Linked List) Memory

    if (initMode != cInitBootDeepPowerDown)
    {
        uint32_t nsid0;

        HalNvme_InitNvmeIrq();      // set up IRQ

        if (initMode == cInitBootFwUpdated)
        {
            HalNvme_Reset(cResetFwUpdate);
        }
        else
        {
            HalNvme_Reset(cResetHard);
        }

        for (nsid0 = 0; nsid0 < NVME_NUM_OF_NAMESPACES; nsid0++)
        {
            // IDS mode template init
            ghwIocmdDw0_ids[nsid0].all      = 0;
            ghwIocmdDw0_ids[nsid0].b.RWMODE = 0;
            ghwIocmdDw0_ids[nsid0].b.CD     = 0;
            ghwIocmdDw0_ids[nsid0].b.JU     = 1;   // BIT(11)
            ghwIocmdDw0_ids[nsid0].b.RM     = 0;   // BIT(12)
            ghwIocmdDw0_ids[nsid0].b.WM     = 1;   // BIT(13)
            ghwIocmdDw1_ids[nsid0].all      = 0;

            // SSD mode template init
            ghwIocmdDw0_ssd[nsid0].all      = 0;
            ghwIocmdDw0_ssd[nsid0].b.JU     = 1;
            ghwIocmdDw0_ssd[nsid0].b.RM     = 1;   // BIT(11)
            ghwIocmdDw0_ssd[nsid0].b.WM     = 1;   // BIT(12)
            ghwIocmdDw0_ssd[nsid0].b.WC     = 1;   // BIT(13)
            ghwIocmdDw1_ssd[nsid0].all      = 0;
        }
    }
    else
    {
        // Initialize PCIe Interrupt
        HalIrq_InitPcieInterrupt();

#if !CO_AUTO_CABM_CLEAR
        // A prerequisite of GPSBSRM interrupt (CCBM Counter Interrupt)
        rNvmeBitmapSram.bitmapSramCmdCompletiBitmapTmr.b.CCBM_TO_THR = 1;
        rNvmeBitmapSram.bitmapSramCmdCompletiBitmapThr.b.CCBM_INTR_THR = XCC_CNT;
#endif

        HalNvme_CentralModuleInterruptEnable();

        // Since MAN_CQ_REQ_DONE_BM is cleared after S3 mode, clear this variable.
        NvmeHba.mcqInUseBitmap = 0;
    }

    // The time-out threshold for triggering PCIe request time-out event.
    // If the counter reaches the threshold specified in this field
    // before PCIe Packet Manager transfers data,
    // the Command Fetcher starts the abort/retry operation for the time-out PCIe request.
    // In default value 4, command was missed sometimes.
    rNvmeCmdFetch.cmdFetcherPcieTmr.b.CMDF_TO_THR = 0xFFFF;


    // Configure cache enable and auto cabm clear enable
    rNvmeStatMngr.statManagerCtrl.b.STSM_AUTO_ABM_CLR_EN = cAuoCabmClrEn;
    rNvmeStatMngr.statManagerCtrl.b.STSM_CACHE_EN = 1;

#if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
    currentNvmePowerMode = cNvmePmActive;
#endif

    return(cEcNoError);
}

#if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
//-----------------------------------------------------------------------------
/**
    H/W block level power management API

    @param[in]  powerMode        targeting pm mode to enter
    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNvme_SetPowerMode(NvmePowerMode_t pmMode)
{
    // no set up if power mode is not changed.
    if (currentNvmePowerMode == pmMode) return cEcNoError;

    switch(pmMode)
    {
    case cNvmePmDeepSleep:
        //HalNvme_ShutdownInternalInterfaces();
        HalNvme_SaveContext();
        break;
    case cNvmePmActive:
        HalNvme_RestoreContext();
        HalNvme_OneTimeInit(cInitBootDeepPowerDown);
        break;
    default:
        break;
    }

    currentNvmePowerMode = pmMode;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Save the Nvme setting register valaue

    @param[in]  none
    @return     Error_t
**/
//-----------------------------------------------------------------------------
static Error_t HalNvme_SaveContext(void)
{
    uint32_t cxtStat;

    rNvme.ctrlRegisterModuleCmd.b.CXT_SAVE_START = 1;                       // 0xF5000008[2]: Context Save Start

    // NVMe register backup
    saveNvmeRegister.queueArbFuncArbCfg[0] = rNvmeQueueArbiter.queueArbFuncArbCfg[0];            // RF5014030h
    saveNvmeRegister.queueArbFuncArbCfg[1] = rNvmeQueueArbiter.queueArbFuncArbCfg[1];            // RF5014034h
    saveNvmeRegister.queueArbFuncArbCfg[2] = rNvmeQueueArbiter.queueArbFuncArbCfg[2];            // RF5014038h

    saveNvmeRegister.statManagerIntrVectorValidGroup0       = rNvmeStatMngr.statManagerIntrVectorValidGroup[0];           // RF5015020h
    saveNvmeRegister.statManagerIntrVectorCoalesciDisGroup0 = rNvmeStatMngr.statManagerIntrVectorCoalesciDisGroup[0];     // RF5015080h

    saveNvmeRegister.statManagerIntrAggregatThr[0] = rNvmeStatMngr.statManagerIntrAggregatThr[0];            // RF5015100h
    saveNvmeRegister.statManagerIntrAggregatThr[1] = rNvmeStatMngr.statManagerIntrAggregatThr[1];            // RF5015104h
    saveNvmeRegister.statManagerIntrAggregatThr[2] = rNvmeStatMngr.statManagerIntrAggregatThr[2];            // RF5015108h

    // Setup SSD List Maximum Entry Count and SSD Command Base Addresses for each segments
    saveNvmeRegister.nvmeDataManger.DATM_SSD_RD_CMD_BASE_ADDR = rNvmeDataMngr.DATM_SSD_RD_CMD_BASE_ADDR;
    saveNvmeRegister.nvmeDataManger.DATM_SSD_WR_CMD_BASE_ADDR = rNvmeDataMngr.DATM_SSD_WR_CMD_BASE_ADDR;
    saveNvmeRegister.nvmeDataManger.ssdListMaxEntryCnt.all = rNvmeDataMngr.ssdListMaxEntryCnt.all;

    // Configure subsegment block count
    saveNvmeRegister.nvmeDataManger.dataManagerSsdFetcherCtrl.b.SUBSEG_BLK_CNT = rNvmeDataMngr.dataManagerSsdFetcherCtrl.b.SUBSEG_BLK_CNT;

    // save LbaFmt
    saveNvmeRegister.nvmeDataManger.lbaFmt[0].all = rNvmeDataMngr.lbaFmt[0].all;
    saveNvmeRegister.nvmeDataManger.lbaFmt[1].all = rNvmeDataMngr.lbaFmt[1].all;
    saveNvmeRegister.nvmeDataManger.lbaFmt[2].all = rNvmeDataMngr.lbaFmt[2].all;
    saveNvmeRegister.nvmeDataManger.lbaFmt[3].all = rNvmeDataMngr.lbaFmt[3].all;

    do
    {
        cxtStat = rNvme.ctrlRegisterModuleEngineStat.b.CXT_STAT;            // 0xF500000C[3:1]: Context Save and Restore Status
    } while ((cxtStat & 0x1) == 0);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Restore the Nvme setting register valaue

    @param[in]  none
    @return     Error_t
**/
//-----------------------------------------------------------------------------
static Error_t HalNvme_RestoreContext(void)
{
    uint32_t                cxtStat;
    CmdSramCmd_t            cmdSramCmd;
    // --- No need to check CMD SRAM zerorizing done.
    //CmdSramEngineStat_t     cmdSramEngineStat;

    // Bitmap SRAM memory zerorize
    rNvmeBitmapSram.bitmapSramCmd.b.BSRM_ZERO_START = 1;                    // 0xF5016008[0]: Bitmap SRAM Zeroize Start

    // Command SRAM memory zerorize
    cmdSramCmd = rNvmeCmdSram.cmdSramCmd;
    cmdSramCmd.b.HM_ZERO_START = 1;                            // 0xF5017008[0]: Hardware Command SRAM Zeroize Start
    cmdSramCmd.b.FM_ZERO_START = 1;                            // 0xF5017008[1]: Firmware Command SRAM Zeroize Start
    rNvmeCmdSram.cmdSramCmd = cmdSramCmd;
    // NVMe context Restore Start
    rNvme.ctrlRegisterModuleCmd.b.CXT_RSTR_START = 1;                       // 0xF5000008[1]: Context Restore Start

    rNvmeQueueArbiter.queueArbFuncArbCfg[0] = saveNvmeRegister.queueArbFuncArbCfg[0];            // RF5014030h
    rNvmeQueueArbiter.queueArbFuncArbCfg[1] = saveNvmeRegister.queueArbFuncArbCfg[1];            // RF5014034h
    rNvmeQueueArbiter.queueArbFuncArbCfg[2] = saveNvmeRegister.queueArbFuncArbCfg[2];            // RF5014038h

    do
    {
        cxtStat = rNvme.ctrlRegisterModuleEngineStat.b.CXT_STAT;            // 0xF500000C[3:1]: Context Save and Restore Status
    } while ((cxtStat & 0x1) == 0);

    while(rNvmeBitmapSram.bitmapSramEngineStat.b.BSRM_ZERO_DONE == 0);              // 0xF501600C[0]: Bitmap SRAM Zeroize Done

    // --- No need to check CMD SRAM zerorizing done.
    // --- CDM SRAM zerorizing will block CMD SRAM access during command fetching and when it is done, CMDF will be able to access CMD SRAM.
    //do
    //{
    //    cmdSramEngineStat = rNvmeCmdSram.cmdSramEngineStat;                                             // 0xF501700C[0]: Hardware Command SRAM Zeroize Done
    //} while ((cmdSramEngineStat.b.HM_ZERO_DONE == 0) || (cmdSramEngineStat.b.FM_ZERO_DONE == 0));       // 0xF501700C[1]: Firmware Command SRAM Zeroize Done

    rNvme.ctrlRegisterModuleAccessCtrl.b.CREG_MEM_LCK = 0;      // RF5000150h[0]
    rClockPll.clkEnCtrl.b.PCIAXI_CLK_EN = 1;

    rNvmeStatMngr.statManagerIntrVectorValidGroup[0]       = saveNvmeRegister.statManagerIntrVectorValidGroup0;           // RF5015020h
    rNvmeStatMngr.statManagerIntrVectorCoalesciDisGroup[0] = saveNvmeRegister.statManagerIntrVectorCoalesciDisGroup0;     // RF5015080h
    rNvmeStatMngr.statManagerIntrAggregatThr[0] = saveNvmeRegister.statManagerIntrAggregatThr[0];            // RF5015100h
    rNvmeStatMngr.statManagerIntrAggregatThr[1] = saveNvmeRegister.statManagerIntrAggregatThr[1];            // RF5015104h
    rNvmeStatMngr.statManagerIntrAggregatThr[2] = saveNvmeRegister.statManagerIntrAggregatThr[2];            // RF5015108h

    // Setup SSD List Maximum Entry Count and SSD Command Base Addresses for each segments
    rNvmeDataMngr.DATM_SSD_RD_CMD_BASE_ADDR = saveNvmeRegister.nvmeDataManger.DATM_SSD_RD_CMD_BASE_ADDR;
    rNvmeDataMngr.DATM_SSD_WR_CMD_BASE_ADDR = saveNvmeRegister.nvmeDataManger.DATM_SSD_WR_CMD_BASE_ADDR;
    rNvmeDataMngr.ssdListMaxEntryCnt.all    = saveNvmeRegister.nvmeDataManger.ssdListMaxEntryCnt.all;

    // Configure subsegment block count
    rNvmeDataMngr.dataManagerSsdFetcherCtrl.b.SUBSEG_BLK_CNT = saveNvmeRegister.nvmeDataManger.dataManagerSsdFetcherCtrl.b.SUBSEG_BLK_CNT;

    // LBA format info
    rNvmeDataMngr.lbaFmt[0].all = saveNvmeRegister.nvmeDataManger.lbaFmt[0].all;
    rNvmeDataMngr.lbaFmt[1].all = saveNvmeRegister.nvmeDataManger.lbaFmt[1].all;
    rNvmeDataMngr.lbaFmt[2].all = saveNvmeRegister.nvmeDataManger.lbaFmt[2].all;
    rNvmeDataMngr.lbaFmt[3].all = saveNvmeRegister.nvmeDataManger.lbaFmt[3].all;

    ///< The rNvmeStatMngr.MAN_CQ_REQ_DONE_BM(F501500Ch) is a status register to notify FW that the manual request is completed.
    ///< This register will be cleared during enter/exit S3 mode. Then it should be restored after wake up. But there's no way to set the rNvmeStatMngr.MAN_CQ_REQ_DONE_BM(F501500Ch) by FW.
    ///< So rather than restore it, the NvmeHba.mcqInUseBitmap should be cleared to keep the coherence between rNvmeStatMngr.MAN_CQ_REQ_DONE_BM(HW Register) and NvmeHba.mcqInUseBitmap(FW Variable)
    ///< In order to guaranty this code working, S3 never happen with any outstanding command.
    NvmeHba.mcqInUseBitmap = 0;

    return cEcNoError;
}
#endif

//-----------------------------------------------------------------------------
/**
    Initialize(clear) NVMe controller SRAM.\n
    The order of initializing the memory does not matter.\n
    Recommend to start the zerorizing from the largest SRAM to smallest SRAM.\n
    The largest SRMA is the Command SRAM, then Control Register.\n
    The smallest SRAM is the Bitmap SRAM.\n
**/
//-----------------------------------------------------------------------------
void HalNvme_InitMemories(uint32_t flag)
{
    // Zeroize mode to skip some NVMe controller register during zeroizing
    if (flag & cHwPartialControlRegInit) rNvme.ctrlRegisterModuleCfg.b.ZEROIZE_MD = 1;

    // Command SRAM memory zerorize
    if (flag & cHwCommandSramInit) rNvmeCmdSram.cmdSramCmd.b.HM_ZERO_START = 1;
    if (flag & cFwCommandSramInit) rNvmeCmdSram.cmdSramCmd.b.FM_ZERO_START = 1;

    // Control register memory zerorize
    if (flag & (cHwControlRegInit|cHwPartialControlRegInit)) rNvme.ctrlRegisterModuleCmd.b.MEM_ZERO_START = 1;

    // Bitmap SRAM memory zerorize
    if (flag & cBitmapSramInit) rNvmeBitmapSram.bitmapSramCmd.b.BSRM_ZERO_START = 1;

    while(rNvmeBitmapSram.bitmapSramEngineStat.b.BSRM_ZERO_DONE == 0 && (flag & cBitmapSramInit));      // Wait done (Bitmap  SRAM)
    while(rNvme.ctrlRegisterModuleEngineStat.b.ENG_STAT_ZERO_DONE == 0 && (flag & cHwControlRegInit));  // Wait done (Control SRAM)
    while(rNvmeCmdSram.cmdSramEngineStat.b.HM_ZERO_DONE == 0 && (flag & cHwCommandSramInit));           // Wait done (Command SRAM)
    while(rNvmeCmdSram.cmdSramEngineStat.b.FM_ZERO_DONE == 0 && (flag & cFwCommandSramInit));           // Wait done (Command SRAM)

    // Revert zeroize mode to default.
    if (flag & cHwPartialControlRegInit) rNvme.ctrlRegisterModuleCfg.b.ZEROIZE_MD = 0;
}

//-----------------------------------------------------------------------------
/**
    Abort and shutdown all lnternal Interfaces.\n
**/
//-----------------------------------------------------------------------------
void HalNvme_ShutdownInternalInterfaces(void)
{
    // Abort all the AXI pending requests in the Command Fetcher
    rNvmeCmdFetch.cmdFetcherAxiCtrl.b.AXI_REQ_ABT = 1;

    // Shut down the AHB slave module that handles the host access to the NVMe controller register space
    rNvme.ctrlRegisterModuleCmd.b.AHB_SHUT_START = 1;

    // Abort all the AXI pending in the Packet Manager
    rNvmePktMngr.packetManagerAxiAbortCtrl.b.ABT = 1;

    // Abort all the AXI pending in the Data Manager
    rNvmeDataMngr.dataManagerAxiAbortCtrl.b.ABT = 1;

    //while(rNvmeCmdFetch.cmdFetcherAxiStat.b.AXI_REQ_ABT_DONE == 0);
    HalRci_WaitValueU32((uint32_t)&rNvmeCmdFetch.cmdFetcherAxiStat, BIT(0), BIT(0), 1000);

    //while(rNvme.ctrlRegisterModuleEngineStat.b.AHB_SHUT_DONE == 0);
    HalRci_WaitValueU32((uint32_t)&rNvme.ctrlRegisterModuleEngineStat, BIT(4), BIT(4), 1000);

    //while((rNvmePktMngr.packetManagerAxiAbortStatus.b.RD_DONE==0) || (rNvmePktMngr.packetManagerAxiAbortStatus.b.WR_DONE==0));
    HalRci_WaitValueU32((uint32_t)&rNvmePktMngr.packetManagerAxiAbortStatus, BIT(0)|BIT(1), 0x3, 1000);

    //while((rNvmeDataMngr.dataManagerAxiAbortStat.b.DDR_RD_DONE==0) || (rNvmeDataMngr.dataManagerAxiAbortStat.b.DDR_WR_DONE==0) ||
    //      (rNvmeDataMngr.dataManagerAxiAbortStat.b.SSDF_RD_DONE==0) || (rNvmeDataMngr.dataManagerAxiAbortStat.b.SRAM_RD_DONE==0) );

    HalRci_WaitValueU32((uint32_t)&rNvmeDataMngr.dataManagerAxiAbortStat, BIT(0)|BIT(1)|BIT(2)|BIT(3), 0xF, 1000);
}

//-----------------------------------------------------------------------------
/**
    Initialize an NVMe controller registers. (controller registers in NVMe spec)

    @param[in]  functionId    function Id
**/
//-----------------------------------------------------------------------------
void HalNvme_InitControllerRegisters(uint16_t functionId)
{
    NvmeConfiguration_t nvmeCC;
    NvmeVersion_t nvmeVS;
    NvmeCap_t nvmeCap;

    // NVMe CAP
    nvmeCap.all = 0;
    nvmeCap.b.TO =      cNvmeCapTo;
    nvmeCap.b.AMS =     cNvmeCapAms;
    nvmeCap.b.CQR =     cNvmeCapCqr;
    nvmeCap.b.MQES =    HW_MAX_QUEUE_SIZE - 1;
    nvmeCap.b.MPSMAX =  cNvmeCapMpsmax;
    nvmeCap.b.MPSMIN =  cNvmeCapMpsmin;
    nvmeCap.b.CSS =     cNvmeCapCss;
    nvmeCap.b.NSSRS =   cNvmeCapNssrs;
    nvmeCap.b.DSTRD =   cNvmeCapDstrd;
    rNvmeController.controller[functionId].CAP.all = nvmeCap.all;

    // Controler RID changed as MJR=0001, MNR=0000
    nvmeVS.all = 0;
    nvmeVS.b.MJR = cNvmeMajorVersionNumber;
    nvmeVS.b.MNR = cNvmeMinorVersionNumber;
    rNvmeController.controller[functionId].VS.all = nvmeVS.all;

    // CSTS CQES/SQES
    nvmeCC.all = 0;
    rNvmeController.controller[functionId].CC.all = nvmeCC.all;

    // Clear interrupt CSTS CQES/SQES
    rNvmeController.controller[functionId].INTMC.all = U32_ALL_BIT_SET; // clear all interrupt mask
}


//-----------------------------------------------------------------------------
/**
    Initialize Interrupt Request.
     - Register VIC handler and enable interrupt.
**/
//-----------------------------------------------------------------------------
void HalNvme_InitNvmeIrq(void)
{
    CentralCtrlModuleIntrEn_t intpEn;

    // Register  and enable an NVMe irq and a handler for an ARM VIC vector.
    VIC_SET_HANDLER(0, cIrqSourceNvme, HalNvme_IrqHandler);

    // Initialize PCIe Interrupt
    HalIrq_InitPcieInterrupt();

#if !CO_AUTO_CABM_CLEAR
    // A prerequisite of GPSBSRM interrupt (CCBM Counter Interrupt)
    rNvmeBitmapSram.bitmapSramCmdCompletiBitmapTmr.b.CCBM_TO_THR = 1;
    rNvmeBitmapSram.bitmapSramCmdCompletiBitmapThr.b.CCBM_INTR_THR = XCC_CNT;
#endif

    // NVMe Interrupt enable
    HalNvme_CentralModuleInterruptEnable();
}

//-----------------------------------------------------------------------------
/**
    Enable NVMe Interrupt Request.
     - Register VIC handler and enable interrupt.
**/
//-----------------------------------------------------------------------------
void HalNvme_EnableNvmeIrq(void)
{
    VIC_ENABLE(0, BIT(cIrqSourceNvme));
}

//-----------------------------------------------------------------------------
/**
    Initialize NVMe Queues Per Function.\n
**/
//-----------------------------------------------------------------------------
void HalNvme_InitPerFunc(void)
{
    CtrlRegisterModuleQueueAllocatiFunc_t queueAllocFunc[3] = { 0, 0, 0 };
    QueueArbFuncArbCfg_t queueArbCfg;

    // Assign number of queues and the base index of Queue Status Table
    // CREG QAL FUN0/1/2 (Fucntion Queue Allocation Information)
    queueAllocFunc[0].b.FTBI = cNvmeCregFun0Ftbi;
    queueAllocFunc[0].b.FTS  = cNvmeCregFun0Fts;
    queueAllocFunc[1].b.FTBI = cNvmeCregFun1Ftbi;
    queueAllocFunc[1].b.FTS  = cNvmeCregFun1Fts;
    queueAllocFunc[2].b.FTBI = cNvmeCregFun2Ftbi;
    queueAllocFunc[2].b.FTS  = cNvmeCregFun2Fts;
    rNvme.ctrlRegisterModuleQueueAllocatiFunc[0].all = queueAllocFunc[0].all;
    rNvme.ctrlRegisterModuleQueueAllocatiFunc[1].all = queueAllocFunc[1].all;
    rNvme.ctrlRegisterModuleQueueAllocatiFunc[2].all = queueAllocFunc[2].all;

    // QARB_FARB0 (Function 0 Queue Arbitration Information)
    queueArbCfg.all = 0;
    queueArbCfg.b.QARB_ARB_BST = cNvmeQarbFun0ArbBurst; // 8 burst commands (2**3)
    queueArbCfg.b.QARB_HP_WT   = cNvmeQarbFun0HpWeight; // High Priority Weight in each arbitration)
    queueArbCfg.b.QARB_MP_WT   = cNvmeQarbFun0MpWeight; // Zero-based number
    queueArbCfg.b.QARB_LP_WT   = cNvmeQarbFun0LpWeight;
    rNvmeQueueArbiter.queueArbFuncArbCfg[0].all = queueArbCfg.all;
}

//-------------------------------------------------------------------------
/**
    Initialize NVMe Controller Per Queue.\n
     - Initialize NVMe Controller registers after PCIe setup.\n
     - Setup NVMe Queue registers.\n
**/
//-----------------------------------------------------------------------------
void HalNvme_InitAdminQueue(void)
{
    NvmeQueueStatusEntrySq_t sramSq;
    NvmeQueueStatusEntryCq_t sramCq;
    NvmeQueueStatusRegSqEntry_t regSq;
    NvmeQueueStatusRegCqEntry_t regCq;
    uint64_t cmdCqPrp;

    ////////////////////////////////////
    // Init-2 Sequence start (Per-Queue)
    ////////////////////////////////////

    // Queue Status Table (SRAM portion)
    sramSq.all = 0;
    sramSq.b.size = rNvmeController.controller[0].AQA.b.ASQS; // Admin Submission Queue Size
    rNvmeController.queueStatus[0].sq.all = sramSq.all;
    CREATE_SQ(0);

    sramCq.all = 0;
    sramCq.b.size = rNvmeController.controller[0].AQA.b.ACQS; // Admin Completion Queue Size
    rNvmeController.queueStatus[0].cq.all = sramCq.all;
    CREATE_CQ(0);

    // Queue Status Table (Register portion)
    regSq.all = 0;
    regSq.b.QC = 1;     // Queue Created
    regSq.b.CT = 1;     // Contiguous, whether the SQ is physically contiguous
    regSq.b.Init = 1;   // Indicates whether the SQ is just created
    rNvmeController.queueReg[0].sq.all = regSq.all;

    regCq.all = 0;
    regCq.b.QC = 1;     // Queue Created, cleared when the CQ is deleted
    regCq.b.CT = 1;     // Contiguous, whether the CQ is physically contiguous
    regCq.b.IE = 1;     // Interrupt Enable, the CQ's interrupt is enabled
    regCq.b.Init = 1;   // Indicates whether the CQ is just created or deleted
    regCq.b.PH = 1;     // Phase, Keeps the posting status of the CQ.
    rNvmeController.queueReg[0].cq.all = regCq.all;

    // Queue PRP Table 1 (CMD PRP Table)
    rNvmeController.cmdPrp[0].sqPrp = rNvmeController.controller[0].ASQB;
    cmdCqPrp = rNvmeController.controller[0].ACQB;
    rNvmeController.cmdPrp[0].cqPrp = cmdCqPrp;

    // Queue PRP Table 2 (Current PRP Table)
    rNvmeController.currentPrp[0].cqPrp = cmdCqPrp;

    // Ready to process commands
    rNvmeStatMngr.statManagerIntrVectorValidGroup[0].b.IV_VALID = BIT(cNvmeCregFun0Ftbi) | BIT(cNvmeCregFun1Ftbi) | BIT(cNvmeCregFun2Ftbi);
}

//-----------------------------------------------------------------------------
/**
    Set a ready state of an NVMe controller to process commands.
**/
//-----------------------------------------------------------------------------
void HalNvme_SetReadyState(void)
{
    while(rNvme.ctrlRegisterModuleAccessStat1.b.CREG_ERR_STS);    // Check no parity error
    while(rNvmeBitmapSram.bitmapSramAccessStat1.b.BSRAM_ERR_STS);    // Check no parity error
    while(rNvmeCmdSram.cmdSramAccessStat1.b.HM_ERR_STAT | rNvmeCmdSram.cmdSramAccessStat1.b.FM_ERR_STAT);    // Check no parity error

    if (NvmeHba.ready==true && rNvmeController.controller[cDefaultFunctionId].CC.b.EN) // Set ready bit
    {
        rNvmeController.controller[cDefaultFunctionId].CSTS.b.RDY = 1;
        M_PRINTF("[RSET] CTST.RDY\n");
    }
}

//-----------------------------------------------------------------------------
/**
    Reset NVMe controller.

    @param[in]  resetMode    reset mode
**/
//-----------------------------------------------------------------------------
void HalNvme_Reset(ResetMode_t resetMode)
{
    CentralCtrlModuleRstCtrl_t ctrlReset;

    // Abort and shutdown all internal interfaces (CMDF, AHB, PKTM, DATM, ...)
    HalNvme_ShutdownInternalInterfaces();

    // Soft reset all modules (CREG, CSRM, BSRM, ..., PCIE),
    ctrlReset.all = 0;
    ctrlReset.b.CREG_RST = 1;
    ctrlReset.b.CSRAM_RST = 1;
    ctrlReset.b.BSRM_RST = 1;
    ctrlReset.b.DATM_RST = 1;
    ctrlReset.b.CMDF_RST = 1;
    ctrlReset.b.STSM_RST = 1;

    ctrlReset.b.QARB_RST = 1;
    ctrlReset.b.PCIE_PKTM_RST_NVME = 1;
    ctrlReset.b.HWLL_RST = 1;
    ctrlReset.b.CENT_RST = 1;
    ctrlReset.b.PCIE_PKTM_RST_PCIE = 1;
    ctrlReset.b.PCIE_CORE_RST = 1;
    rNvmeCentralCtrl.centralCtrlModuleRstCtrl.all = ctrlReset.all;  //0x80003737; // Soft reset all modules

    // Clear Abort bits
    rNvmePktMngr.packetManagerAxiAbortCtrl.b.ABT = 0;
    rNvmeDataMngr.dataManagerAxiAbortCtrl.b.ABT = 0;

    MEM_CLR(&NvmeHba, sizeof(NvmeHba_t)); // Initialize NvmeHba data structure and also delay before reset release

    rNvmeCentralCtrl.centralCtrlModuleRstCtrl.all = 0x00000000;     // Reset Release

    if (resetMode==cResetHard)
    {
        HalNvme_InitMemories(cHwCommandSramInit|cFwCommandSramInit|cBitmapSramInit|cHwControlRegInit);
    }
    else if (resetMode==cResetSoft)
    {
        HalNvme_InitMemories(cHwCommandSramInit|cFwCommandSramInit|cBitmapSramInit);
    }

    #if (SUP_DIRE_STREAMS == ENABLE)
    gCmdfPtr = 0;
    #endif

    HalNvme_InitPerFunc();
    HalNvme_InitControllerRegisters(cDefaultFunctionId);
    HalNvme_DeleteAllQueues();

    // Clear VLD bit in INTP
    rNvmeStatMngr.statManagerIntrVectorValidGroup[cDefaultFunctionId].b.IV_VALID = 0;

    // Host Interrupt All Clear
    rNvmeCentralCtrl.centralCtrlModuleIntr.all = U32_ALL_BIT_SET;

    //  The defualt value of CREG FW lock bit is changed to 1h.
    //  NVMe register interface is changed to prevent early access from host before NVMe register recovery is not yet finished.
    //  So, after all things are ready, FW need to clear this bit (R10000150h[0]).
    rNvme.ctrlRegisterModuleAccessCtrl.b.CREG_MEM_LCK = 0;

#if (FIX_NVME_FETCH_ONE_COMMAND_AT_A_TIME == ENABLE)
    rNvmeQueueArbiter.queueArbFuncArbCfg[cDefaultFunctionId].b.QARB_ARB_BST = 0; // set burst size to 1
#endif

    rNvmeController.controller[cDefaultFunctionId].CSTS.all = 0;                 // clear all status
}

//-----------------------------------------------------------------------------
/**
    Reset NVMe controller.

    @param[in]  resetMode    reset mode
**/
//-----------------------------------------------------------------------------
void HalNvme_ResetController(void)
{
    HalNvme_Reset(cResetSoft);
}

//-----------------------------------------------------------------------------
/**
    Reset the subsystems of an NVMe controller.
**/
//-----------------------------------------------------------------------------
void HalNvme_ResetSubsystem(void)
{
    HalPcieLink_HandleNssr();

    HalNvme_Reset(cResetHard);

    rNvmeController.controller[0].CSTS.b.NSSRO = 1;
}

//-----------------------------------------------------------------------------
/**
    Reset HW link list
**/
//-----------------------------------------------------------------------------
void HalNvme_ResetHwLinkList(void)
{
    HardwareLnkListEcuAccessCtrl010_t hwllEcuAccessCtrl;

    rNvmeHwll.hardwareLnkListEcuAccessCtrl01_0.b.FW_INIT_HT_REQ = 1;
    while(rNvmeHwll.hardwareLnkListEcuAccessCtrl01_0.b.FW_INIT_HT_DONE == 0);   // Wait done (Firmware Head/Tail Initialization)
    hwllEcuAccessCtrl.all = rNvmeHwll.hardwareLnkListEcuAccessCtrl01_0.all;
    hwllEcuAccessCtrl.b.FW_INIT_HT_REQ = 0;
    hwllEcuAccessCtrl.b.FW_INIT_HT_DONE = 0;
    rNvmeHwll.hardwareLnkListEcuAccessCtrl01_0.all = hwllEcuAccessCtrl.all;
}

//-----------------------------------------------------------------------------
/**
    Delete all NVMe Queues.
**/
//-----------------------------------------------------------------------------
void HalNvme_DeleteAllQueues(void)
{
    NvmeQueueStatusRegCqEntry_t cq;
    uint32_t qIndex;

    // Delete IOQs, Reset ADMQs and Clear MSI-X pending bits
    for(qIndex = 0; qIndex <= HW_MAX_IO_QUEUES; qIndex++)
    {
        rNvmeController.queueReg[qIndex].sq.b.QC = 0;
        cq.all = rNvmeController.queueReg[qIndex].cq.all;
        cq.b.QC = 0;
        cq.b.Init = 1;
        rNvmeController.queueReg[qIndex].cq.all = cq.all;
        rNvmeController.msixPba[qIndex] = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Shutdown NVMe controller.
**/
//-----------------------------------------------------------------------------
void HalNvme_ShutDown(void)
{
    HalNvme_Reset(cResetSoft);
}

#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
/**
    Check command PRP fetch error or not

    @param[in] cmdIndex     command Index

    @return error code
**/
//-----------------------------------------------------------------------------
__inline Error_t HalNvme_PrpReady(uint16_t idx)
{
    MsgHostIO_t* pHcmdMsg;
    Error_t status = cEcNoError;
    uint32_t    counter = 0;

    pHcmdMsg = (MsgHostIO_t*)&smHcmdMsg[idx];

    //D_PRINTF("[%d] Xfer, NB[%x]\n", idx, gHcmdQ[idx].addrInfo.nb);

#if 0
    if (pHcmdMsg->attrInfo.b.direction == cXferD2h)
    {
        if (gHcmdQ[idx].addrInfo.nb > 16)
        {
            if (gHcmdQ[idx].addrInfo.nb <= 0x200)
            {
                while ((!rNvmeHwIoCmd[idx].PF1) && (gNvmeIfMgr.state == cNvmeIfReady))
                {
                    counter ++;
                    if (counter > 0x500000)
                    {
                        D_PRINTF("PF1_TO[%d], LBA[%x], Sec[%08x], FC[%d]!!\n",
                            idx, gHcmdQ[idx].addrInfo.lba, gHcmdQ[idx].addrInfo.nb, pHcmdMsg->bufDesc.flowCtrl);
                        status = cEcError;
                        break;
                    }
                }
            }
            else
            {
                while ((!rNvmeHwIoCmd[idx].PF2) && (gNvmeIfMgr.state == cNvmeIfReady))
                {
                    counter ++;
                    if (counter > 0x500000)
                    {
                        D_PRINTF("PF2_TO[%d], LBA[%x], Sec[%08x], FC[%d]!!\n",
                            idx, gHcmdQ[idx].addrInfo.lba, gHcmdQ[idx].addrInfo.nb, pHcmdMsg->bufDesc.flowCtrl);
                        status = cEcError;
                        break;
                    }
                }
            }
        }
    }
#endif
    pHcmdMsg->bufDesc.prpRdy = cMcPrpReady;
    return  status;
}

//-----------------------------------------------------------------------------
/**
    Reqeust Data transfer for an Admin command

    @param[in] pRequest     data transfer request structure

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNvme_RequestDataTransferAdmin(XferRequest_t* pRequest)
{
    volatile NvmeHwAdmCmd_t*     pHwAcmd;
    uint16_t cmdIndex;

    cmdIndex = GET_COMMAND_INDEX(pRequest->attr.b.cmdId);

    // Fill the HW ADM CMD SRAM before activate it.
    // PRP1/2 were already filled while command was fetched.
    pHwAcmd = (NvmeHwAdmCmd_t*) &rNvmeHwAdmCmd[cmdIndex];
    pHwAcmd->RWMODE = pRequest->attr.b.direction;   // Read/Write operation, 0 = Read, 1 = Write.
    pHwAcmd->RM = 1;
    pHwAcmd->WM = 1;                            // Use SSD
#if (_SOC_REV_>=0x0200)
    pHwAcmd->JU = 1;                            // Use R1.0 type SSD
#endif

    if (pRequest->mode == ADMIN_TRIM)
    {
        pHwAcmd->PRP1_LO = pRequest->PRP1_LO;
        pHwAcmd->PRP1_HI = pRequest->PRP1_HI;
        pHwAcmd->PRP2_LO = pRequest->PRP2_LO;
        pHwAcmd->PRP2_HI = pRequest->PRP2_HI;
    }

    pHwAcmd->POINTER = pRequest->ssdIndex;
    pHwAcmd->NDW = pRequest->xferByteCount / 4; // Transfer DW Count
    ASSERT(GET_BIT(rNvmeDataMngr.dataManagerCtrl0.b.DATM_ACMD_CABM, cmdIndex) == 0);
    BIT_SET(rNvmeDataMngr.dataManagerCtrl0.b.DATM_ACMD_CABM, BIT(cmdIndex));

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Reqeust Data transfer thru I/O command index

    @param[in] pRequest     data transfer request structure

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNvme_RequestDataTransferIoQuick(MsgHostIO_t* pHcmdMsg)
{
    volatile NvmeHwIoCmd_t* pHwIocmd;
    NvmeHwIoCmdDw0_t hwIocmdDw0;
    NvmeHwIoCmdDw1_t hwIocmdDw1;
    uint16_t cmdIndex;
    uint32_t nsId0;
    uint32_t GroupId;
    uint32_t Bitmap;

    cmdIndex = GET_COMMAND_INDEX(pHcmdMsg->attrInfo.b.cmdId);
    pHwIocmd = (NvmeHwIoCmd_t*) &rNvmeHwIoCmd[cmdIndex];

    GroupId = cmdIndex >> 5;
    Bitmap  = (1 << (cmdIndex & 0x1F));
    nsId0   = rNvmeHwIoCmd[cmdIndex].NSID - 1;

    switch (pHcmdMsg->bufDesc.flowCtrl)
    {
        case cXferIds:
            //D_PRINTF("IDS[%d\n", cmdIndex);
            pHwIocmd->dw0.all = ghwIocmdDw0_ids[nsId0].all;
            pHwIocmd->dw1.all = ghwIocmdDw1_ids[nsId0].all;
            rNvmeBitmapW1s.CABM0[GroupId] = Bitmap;
            if (HalNvme_PrpReady(cmdIndex) == cEcError)
            {
                return cEcError;
            }
            break;

        case cXferNonStreaming:
            hwIocmdDw0.all = ghwIocmdDw0_ssd[nsId0].all;
            hwIocmdDw1.all = ghwIocmdDw1_ssd[nsId0].all;
            hwIocmdDw0.b.RWMODE = pHcmdMsg->attrInfo.b.direction;
            hwIocmdDw0.b.CD     = !pHcmdMsg->attrInfo.b.autoEoC;
            hwIocmdDw1.POINTER  = pHcmdMsg->bufDesc.ssdIndex;

            //D_PRINTF("SSD[%d] HW0[%x], HW1[%x]\n", cmdIndex, hwIocmdDw0.all, hwIocmdDw1.all);
            pHwIocmd->dw0.all = hwIocmdDw0.all;
            pHwIocmd->dw1.all = hwIocmdDw1.all;

            if (pHcmdMsg->attrInfo.b.direction == cXferH2d) // Write command
            {
                while(rNvmeDataMngr.wrCmdInOrderFifoStat.b.FIFO_FULL == 1);
                rNvmeDataMngr.wrCmdInOrderFifoWrPort.all = cmdIndex;
            }
            else
            {
                #if 0    // Req Xfer by FIFO
                while(rNvmeDataMngr.rdCmdInOrderFifoStat.b.FIFO_FULL == 1);
                rNvmeDataMngr.rdCmdInOrderFifoWrPort.all = cmdIndex;
                #else    // Req Xfer by CABM
                rNvmeBitmapW1s.CABM0[GroupId] = Bitmap;
                #endif
            }

            if (pHcmdMsg->attrInfo.b.direction == cXferD2h)
            {
                if (HalNvme_PrpReady(cmdIndex) == cEcError)
                {
                    return cEcError;
                }
                rNvmeBitmapW1s.RDRBM0[GroupId] = Bitmap;
            }
            break;

        default :
            break;
    }

    pHcmdMsg->hdr.b.status = cMsgXfered;

    return(cEcNoError);
}
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

#if _CPU_==CPU_ID0
//-----------------------------------------------------------------------------
/**
    Get a available command index for an admin command FIFO.\n
    There are 8 HW_ACMD entries in HW_CMD_SRAM.\n
      - one index is reserved for Dword size I/O command data transfer
    So it should be pick up any available one within 7 slots.\n
    It searchs an available one from LSB to MSB based on a gcStartAcmdIndex.\n

    @return a command index.\n
                0 to 6 is valid.\n
                0xFFFF (-1) is invalid.\n
**/
//-----------------------------------------------------------------------------
uint16_t HalNvme_GetAvailableAcmdIndex(void)
{
    static const uint16_t cValidAdminIndexCount = HW_MAX_ADMIN_CMD_INDEX - 1;
    static uint16_t gcStartAcmdIndex = 0; // It should be between 0 and 6.
    uint16_t cmdIndex;

    cmdIndex = GetFirstMsbSetOfRange(~NvmeHba.acmdInUseBitmap,  cValidAdminIndexCount - 1, gcStartAcmdIndex, 0);
    gcStartAcmdIndex = (cmdIndex + 1) % cValidAdminIndexCount;

    return  cmdIndex;
}

//-----------------------------------------------------------------------------
/**
    Save a transfer parameters to the HAL layer for Admin Commands.

    @param[in] cmdIndex     commnad Index to index HW entries
    @param[in] pvAcmdCdb    Pointer to command data block for a popped admin command (CDB)

    return function id
**/
//-----------------------------------------------------------------------------
uint32_t HalNvme_SaveAdminCommandId(uint16_t cmdIndex, void* pvAcmdDw0, void* pvAcmdCdb)
{
    NvmeAdminXfer_t* pAdminXfer = &sNvmeAdminXfer[cmdIndex];
    uint32_t* pdwCdb = (uint32_t*) pvAcmdCdb;

    MEM_CLR((void*) pAdminXfer, sizeof(NvmeAdminXfer_t));

    pAdminXfer->id.all   = *((uint32_t*) pvAcmdDw0);
    pAdminXfer->id.b.CID = ((NvmeCommandDw0_t*)pvAcmdCdb)->b.CID;

    return pAdminXfer->id.b.FID;
}

//-----------------------------------------------------------------------------
/**
    Release resource associated with Admin Command index

    @param[in] cmdIndex     Admin commnad Index to release

**/
//-----------------------------------------------------------------------------
void HalNvme_ReleaseAdminCommand(uint32_t cmdIndex)
{
    BIT_CLR(NvmeHba.acmdInUseBitmap, BIT(cmdIndex));
    BIT_CLR(rNvmeDataMngr.dataManagerCtrl0.b.DATM_ACMD_CABM, BIT(cmdIndex));   // CLEAR
}

//-----------------------------------------------------------------------------
/**
    Release resource associated with I/O Command index

    @param[in] cmdIndex     I/O commnad Index to release
**/
//-----------------------------------------------------------------------------
void HalNvme_ReleaseIoCommand(uint32_t cmdIndex)
{
    if(!IS_IOCMD_XFERABLE(cmdIndex))
    {
        BA_WR1_U32_L2M(rNvmeBitmapW1c.CIABM, cmdIndex);             // Clear CIABM
        rNvmeBitmapSram.bitmapSramCmdIndexAvailablBitmapCntrIncrease.all = 1;
    }
#if !CO_AUTO_CABM_CLEAR
    else
    {
        BA_WR1_U32_L2M(rNvmeBitmapW1c.CCBM, cmdIndex);              // Clear CCBM
        BA_WR1_U32_L2M(rNvmeBitmapW1c.CABM0, cmdIndex);             // Clear CABM0
    }
#endif
}

//-----------------------------------------------------------------------------
/**
    FW uses less or max FIFO count, so there is no FIFO full condition.
    Don't need to check FIFO full

    @param[in] mcqBitMask   Manual completion queue bitmask not to use
**/
//-----------------------------------------------------------------------------
static uint32_t HalNvme_GetFreeMcqIndex(uint32_t mcqBitMask)
{
    uint32_t mcqIndex;
    uint32_t mcqDoneBitmap;

    // Check whether FW mcq bits are available first.
    while(!(~(NvmeHba.mcqInUseBitmap | mcqBitMask)))
    {
        mcqDoneBitmap = rNvmeStatMngr.MAN_CQ_REQ_DONE_BM & ~mcqBitMask;
        rNvmeStatMngr.MAN_CQ_REQ_DONE_BM = mcqDoneBitmap;   // Release and clean up MCQ Done Bitmap Register
        NvmeHba.mcqInUseBitmap &= ~mcqDoneBitmap;           // Release avaiable variable bitmaps

        // handle status manager error to avoid dead-lock in case of interrupt disabled
        if (rNvmeStatMngr.statManagerErrFifoStat.b.ERR_RPT_VAL_ENT_CNT!=0)
        {
            HalNvme_HandleStsmError(sIrqHandler);
        }
    }

    // find manual CQ index from bitmap variable
    mcqIndex = 31-__clz(~(NvmeHba.mcqInUseBitmap|mcqBitMask));
    BIT_SET(NvmeHba.mcqInUseBitmap, BIT(mcqIndex));

    return mcqIndex;
}

//-----------------------------------------------------------------------------
/**
    Push one entry to Manual CQ FIFO.

    @param[in] fifo         HW FIFO data
    @param[in] mcqBitMask   Manual completion queue bitmask not to use
**/
//-----------------------------------------------------------------------------
void HalNvme_PushToMcqFifo(StatManagerFifoWritePort_t fifo[], uint32_t mcqBitMask)
{
    uint32_t mcqIndex;

    if (!IS_SQ_CREATED(fifo[2].dw2.MCQ_SQ_ID)) // do not send status if submission queue is not created or deleted.
    {
        return;
    }

    mcqIndex = HalNvme_GetFreeMcqIndex(mcqBitMask);

    fifo[0].dw0.MCQ_REQ_ID = mcqIndex;
#if (_SOC_REV_>=0x0200)
    fifo[0].dw0.WT = 1;                   // Wait for CQ available
#endif

    //D_PRINTF("MCQ:%X %X\n", mcqIndex, fifo[2]);

    BEGIN_CS1
        rNvmeStatMngr.statManagerFifoWritePort = fifo[0].all;
        rNvmeStatMngr.statManagerFifoWritePort = fifo[1].all;
        rNvmeStatMngr.statManagerFifoWritePort = fifo[2].all;
        rNvmeStatMngr.statManagerFifoWritePort = fifo[3].all;
        // copy mcq FIFO data for retry (max 8)
        MEM_COPY(NvmeHba.mcqFifoBackup[mcqIndex], fifo, sizeof(NvmeHba.mcqFifoBackup[mcqIndex]));
    END_CS1

}

//-----------------------------------------------------------------------------
/**
    HAL sends good status to host for EoC if HW does not support Auto EOC.

    @param[in] cmdIndex     I/O commnad Index
**/
//-----------------------------------------------------------------------------
static void HalNvme_SendIoCmdGoodStatus(uint32_t cmdIndex)
{
    NvmeCqeModeId_t cqeMode;
    static StatManagerFifoWritePort_t fifo[4] = { 0, 0, 0, 0 };

#if (_SOC_REV_>=0x0200)
    cqeMode = (NvmeCqeModeId_t) CO_AUTO_CABM_CLEAR;
    fifo[0].dw0.CQE_MODE = (uint32_t) cqeMode; // CQE Transfer mode
    if(cqeMode == cCqeHwIo)    // HW_I/O CQE mode
    {
        fifo[0].dw0.MCQ_FUNC_ID = cmdIndex;
    }
    else    // Default CQE mode
#endif
    {
        fifo[2].dw2.MCQ_SQ_ID = GET_IO_SQID_FROM_INDEX(cmdIndex);
        fifo[2].dw2.MCQ_CMD_ID = GET_IO_CID_FROM_INDEX(cmdIndex);// NVMe Host Command Id
    }

    HalNvme_PushToMcqFifo(fifo, cHalMcqIndexMask);

#if !CO_RELEASE_CMDS_PER_GROUP
    HalNvme_ReleaseIoCommand(cmdIndex);
#endif
}

//-----------------------------------------------------------------------------
/**
    Completion of one I/O command group

    @param[in] groupIndex       I/O commnad group Index
    @param[in] completedBitmap  data transfer or command completion bitmap
**/
//-----------------------------------------------------------------------------
uint32_t HalNvme_OneIoCmdGroupCompleted(uint32_t groupIndex, uint32_t completedBitmap)
{
    uint32_t cmdIndex;
    uint32_t cmdOffset;
    uint32_t bitmap;
    uint32_t releaseBitmap;
    uint32_t completedCount;

    bitmap = completedBitmap & ACTIVATED_DW_FW(groupIndex);
    releaseBitmap = 0;
    completedCount = 0;

    while (bitmap!=0)
    {
        cmdOffset = FFS(bitmap);
        cmdIndex = groupIndex*32 + cmdOffset;
        BIT_CLR(bitmap, BIT(cmdOffset));

        sIrqHandler[cIrqHostIoXferDone](GET_IO_XFER_PARAM_FROM_INDEX(cmdIndex)); // transfer done call back
        completedCount +=1;

        if(IS_AUTO_EOC(cmdIndex)) // HAL firmware sends good status if HW AUTO_CQ is disabled but autoEoC flag is on.
        {
#if !CO_AUTO_CQ
            HalNvme_SendIoCmdGoodStatus(cmdIndex);
#endif
            BIT_SET(releaseBitmap, BIT(cmdOffset));
        }
    }

    BIT_CLR(ACTIVATED_DW_FW(groupIndex), completedBitmap);

#if CO_RELEASE_CMDS_PER_GROUP
    if (releaseBitmap!=0)
    {
        // clear one group at a time
        rNvmeBitmapW1c.CCBM[groupIndex] = releaseBitmap;
        rNvmeBitmapW1c.CABM0[groupIndex] = releaseBitmap;
    }
#endif
    return completedCount;
}

//-----------------------------------------------------------------------------
/**
    Process completed I/O commands
**/
//-----------------------------------------------------------------------------
void HalNvme_ProcessCompletedIoCommands(void)
{
    uint32_t groupIndex;
    uint32_t completedCount;
    uint32_t completedGroup;

    completedGroup = COMPLETED_GROUP_BM();
    completedCount = 0;

    while(completedGroup != 0)
    {
        groupIndex = FFS(completedGroup);
        BIT_CLR(completedGroup, BIT(groupIndex));

        completedCount += HalNvme_OneIoCmdGroupCompleted(groupIndex, COMPLETED_DW(groupIndex));
    }

    // It could reset the CCBM counter when the mismatch between the CCBM COunter and the Bitmaps happened.
    DECREASE_COMPLETED_COUNT(completedCount);
}

//-----------------------------------------------------------------------------
/**
    Add MCQ data into a Repost FIFO.

**/
//-----------------------------------------------------------------------------
void HalNvme_AddMcqIntoRepostFifo(uint16_t mcqIndex)
{
    if (NvmeHba.mcqInUseBitmap & BIT(mcqIndex))
    {
        // Add MCQ Fifo data into deferred queue
        CbWrite(&sCqRepostFifo, (void*)&NvmeHba.mcqFifoBackup[mcqIndex]);
        BIT_CLR(NvmeHba.mcqInUseBitmap, BIT(mcqIndex)); // release mcq index
        D_PRINTF(" QUEUED MCQ(%2X), Q_CID(%08X)", mcqIndex, NvmeHba.mcqFifoBackup[mcqIndex][2]);
    }
}

//-----------------------------------------------------------------------------
/**
    NVMe CMD abort

    @param[in] CmdIdx        I/O commnad Index
    @param[in] GroupId       I/O commnad group Index
    @param[in] Bitmap  data transfer or command completion bitmap

**/
//-----------------------------------------------------------------------------
void HalNvme_IoCmdAbort(uint16_t CmdIdx, uint32_t GroupId, uint32_t Bitmap)
{
    uint32_t cpuCycle = HalCpu_ReadPerformanceCounter();
    uint32_t idx = 0;

    D_PRINTF("Abort[%3d]\n",CmdIdx);

    // Step 1. Prepare Abort CMD info
    BEGIN_CS1
    gNvmeAbortCmdInfo.active = TRUE;
    gNvmeAbortCmdInfo.Id = CmdIdx;
    END_CS1

    // Step 2. Clear RDRBM
    rNvmeBitmapW1c.RDRBM0[GroupId] = Bitmap;

    // Step 3. Set to SSD mode
    rNvmeHwIoCmd[CmdIdx].dw0.b.RM = 1;   // Change to SSD mode

    // Step 4. Abort HW_IO transfer
    rNvmeHwIoCmd[CmdIdx].dw0.b.AB = 1;   // AB bit set

    // Step 5. Set RDRBM
    rNvmeBitmapW1s.RDRBM0[GroupId] = Bitmap;

    // Step 6. Wait Status Manager report error
    while (gNvmeAbortCmdInfo.active == TRUE)
    {
        // timmer
        if ((HalCpu_ReadPerformanceCounter() - cpuCycle)*2 > 10000*1000*100)
        {
            D_PRINTF("Abort time[%d]\n",HalCpu_ReadPerformanceCounter() - cpuCycle);
            //D_PRINTF(" Lock[%d]\n",rNvmeBitmapSram.bitmapSramAccessCtrl.b.BSRAM_MEM_LOCK);
            cpuCycle = HalCpu_ReadPerformanceCounter();

            if (idx == 5)
            {
                ASSERT(0);
            }
            else
            {
                rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all = 0x08010800;

                rNvmeHwll.hardwareLnkListHostActiveReqTblAddr1_1.b.HART_ADDR = 0;
                D_PRINTF("Wrt->R0[%X]\n",rNvmeCentralCtrl.centralCtrlModuleMonitorData.all);
                rNvmeHwll.hardwareLnkListHostActiveReqTblAddr1_1.b.HART_ADDR = 1;
                D_PRINTF("Wrt->R1[%X]\n",rNvmeCentralCtrl.centralCtrlModuleMonitorData.all);
                rNvmeHwll.hardwareLnkListHostActiveReqTblAddr1_1.b.HART_ADDR = 2;
                D_PRINTF("Wrt->R2[%X]\n",rNvmeCentralCtrl.centralCtrlModuleMonitorData.all);
            }

            idx++;
        }
    }

    if (rNvmeStatMngr.statManagerErrFifoStat.b.ERR_RPT_VAL_ENT_CNT > 0)
    {
        D_PRINTF("No matching current algorithms[%x]\n",rNvmeStatMngr.statManagerErrFifoStat.all);
        ASSERT(0);
        DECREASE_COMPLETED_COUNT(1);
    }
    else
    {
        DECREASE_COMPLETED_COUNT(1);
    }
}

//-----------------------------------------------------------------------------
/**
    NVMe create completion CMD queue

    @param[in]  prp                 Physical Region Page
    @param[in]  cqId                completion queue Id
    @param[in]  qSize0              queue size (number of entries), zero based value
    @param[in]  contiguous          physically contiguous memory
    @param[in]  interruptEnable     interrupt enable
    @param[in]  interruptVector     interrupr vector

**/
//-----------------------------------------------------------------------------
void HalNvme_CreateIoCq(uint64_t prp, uint32_t cqId, uint32_t qSize0, uint32_t contiguous, uint32_t interruptEnable, uint32_t interruptVector)
{
    NvmeQueueStatusEntryCq_t sramCq;
    NvmeQueueStatusRegCqEntry_t regCq;
    // Queue Status Table (SRAM portion)
    sramCq.all = 0;                     // headDbPtr = 0, tailPtr = 0
    sramCq.b.size = qSize0;
    sramCq.b.vector = interruptVector;
    rNvmeController.queueStatus[cqId].cq.all = sramCq.all;

    // Queue PRP Table 1 (CMD PRP Table)
    rNvmeController.cmdPrp[cqId].cqPrp = prp;

    // If physically contiguous, set Queue PRP Table 2 (Current PRP Table)
    if(contiguous)
    {
        rNvmeController.currentPrp[cqId].cqPrp = prp;
    }

    // Set Interrupt Valid bit ? [0] ???
    BIT_SET(rNvmeStatMngr.statManagerIntrVectorValidGroup[0].b.IV_VALID, BIT(interruptVector));

    // Queue Status Table (Register portion)
    regCq.all = 0;
    regCq.b.QC = 1;                 // Queue Created, cleared when the CQ is deleted
    regCq.b.CT = contiguous;        // Contiguous, whether the CQ is physically contiguous
    regCq.b.IE = interruptEnable;   // Interrupt Enable, the CQ's interrupt is enabled
    regCq.b.Init = 1;               // Indicates whether the CQ is just created or deleted
    regCq.b.PH = 1;                 // Phase, Keeps the posting status of the CQ.
    rNvmeController.queueReg[cqId].cq.all = regCq.all;
}

//-----------------------------------------------------------------------------
/**
    NVMe create submission CMD queue

    @param[in]  prp                 Physical Region Page
    @param[in]  sqId                submission queue Id
    @param[in]  qSize0              queue size (number of entries), zero based value
    @param[in]  contiguous          physically contiguous memory
    @param[in]  priority            priority
    @param[in]  cqId                completion queue Id
**/
//-----------------------------------------------------------------------------
void HalNvme_CreateIoSq(uint64_t prp, uint32_t sqId, uint32_t qSize0, uint32_t contiguous, uint32_t priority, uint32_t cqId)
{
    NvmeQueueStatusEntrySq_t sramSq;
    NvmeQueueStatusRegSqEntry_t regSq;

    // Queue Status Table (SRAM portion)
    sramSq.all = 0;             // headDbPtr = 0, tailPtr = 0
    sramSq.b.size = qSize0;     // SQ Size
    sramSq.b.cqId = cqId;       // Associative CQ ID
    rNvmeController.queueStatus[sqId].sq.all = sramSq.all;

    // Queue PRP Table 1 (CMD PRP Table)
    rNvmeController.cmdPrp[sqId].sqPrp = prp;

    // Queue PRP Table 2 (Current PRP Table)
    rNvmeController.currentPrp[sqId].sqPrp = prp;

    // Queue Status Table (Register portion)
    regSq.all = 0;
    regSq.b.QC = 1;              // Queue Created
    regSq.b.CT = contiguous;     // Contiguous, whether the SQ is physically contiguous
    regSq.b.Priority = priority; // Priority, Indicates the priority of the SQ
    regSq.b.Init = 1;            // Indicates whether the SQ is just created
    rNvmeController.queueReg[sqId].sq.all = regSq.all;

}

//-----------------------------------------------------------------------------
/**
    NVMe delete CMD queue

    @param[in] Cq           Complete Queue or not

**/
//-----------------------------------------------------------------------------
void HalNvme_DeleteIoQueue(bool Cq, uint16_t QId)
{
    NvmeQueueStatusRegCqEntry_t regCq;

    if (Cq == TRUE)          // Complete Queue
    {
        regCq.all = 0;
        regCq.b.Init = 1;
        rNvmeController.queueReg[QId].cq.all= regCq.all;
    }
    else
    {
        rNvmeController.queueReg[QId].sq.all = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Set Interrupt Coalescing

    @param[in]  timeUsec     aggregation time (usec)
    @param[in]  threshold    aggregation thresold (0 based value)

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNvme_SetInterruptCoalescing(uint32_t timeUsec, uint32_t threshold)
{
    Error_t error;

    error = cEcNoError;

    if( (timeUsec == 0) && (threshold == 0))
    {
        rNvmeStatMngr.statManagerIntrAggregatThr[0].b.INT_AGG_TMR_THR = 1;
        rNvmeStatMngr.statManagerIntrAggregatThr[0].b.INTR_AGG_CNTR_THR = 0;
        rNvmeStatMngr.statManagerIntrVectorCoalesciDisGroup[0].b.IV_COAL_DIS = 0x7FFFE; // Enable the coalescing of CQ1 of IV0
    }
    else
    {
        rNvmeStatMngr.statManagerIntrAggregatThr[0].b.INT_AGG_TMR_THR = timeUsec/100;   // timer is 100usec unit
        rNvmeStatMngr.statManagerIntrAggregatThr[0].b.INTR_AGG_CNTR_THR = threshold;
        rNvmeStatMngr.statManagerIntrVectorCoalesciDisGroup[0].b.IV_COAL_DIS = 0;       // Enable the coalescing of all CQ of IV0
    }
    return(error);
}

//-----------------------------------------------------------------------------
/**
    Interrupt Vector configuration

    @param[in]  vector        interrupt vector
    @param[in]  disable       disable coalescing

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalNvme_SetInterruptConfiguration(uint32_t vector, uint32_t disable)
{
    Error_t error;

    error = cEcNoError;


    if(vector < HW_MAX_INT_VECTORS)
    {
        if(disable)
        {
            BIT_SET( rNvmeStatMngr.statManagerIntrVectorCoalesciDisGroup[0].b.IV_COAL_DIS, BIT(vector));
        }
        else
        {
            BIT_CLR( rNvmeStatMngr.statManagerIntrVectorCoalesciDisGroup[0].b.IV_COAL_DIS, BIT(vector));
        }
    }
    else
    {
        error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
    return(error);
}

//-----------------------------------------------------------------------------
/**
    Command processing arbitration

    @param[in]  burst        arbitraion burst
    @param[in]  lpw          low priority weight
    @param[in]  mpw          medium priority weight
    @param[in]  hpw          high priority weight

    @return error code
**/
//-----------------------------------------------------------------------------
// TODO: Put an argument for a function id
Error_t HalNvme_SetCommandArbitration(uint32_t burst, uint32_t lpw, uint32_t mpw, uint32_t hpw)
{
    Error_t error;

    error = cEcNoError;

    rNvmeQueueArbiter.queueArbFuncArbCfg[0].b.QARB_ARB_BST = burst;

    if(rNvmeController.controller[0].CAP.b.AMS == 1)
    {
        rNvmeQueueArbiter.queueArbFuncArbCfg[0].b.QARB_LP_WT = lpw;
        rNvmeQueueArbiter.queueArbFuncArbCfg[0].b.QARB_MP_WT = mpw;
        rNvmeQueueArbiter.queueArbFuncArbCfg[0].b.QARB_HP_WT = hpw;
    }
    else
    {
        error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
    return(error);
}

//-----------------------------------------------------------------------------
/**
    Get NVMe LBA format

    @param[in] idx        commnad Index

**/
//-----------------------------------------------------------------------------
uint32_t HalNvme_GetLbaFormat(uint32_t idx)
{
    return rNvmeDataMngr.lbaFmt[idx].all;
}

//-----------------------------------------------------------------------------
/**
    Set NVMe LBA format

    @param[in] lbaFmt     LBA format
    @param[in] idx        commnad Index

**/
//-----------------------------------------------------------------------------
void HalNvme_SetLbaFormat(uint32_t lbaFmt, uint32_t idx)
{
    rNvmeDataMngr.lbaFmt[idx].all = lbaFmt;
}

//-----------------------------------------------------------------------------
/**
    Setup NVMe SSD

**/
//-----------------------------------------------------------------------------
void HalNvme_SsdSetup(void)
{
    // Setup SSD List Maximum Entry Count and SSD Command Base Addresses for each segments
    rNvmeDataMngr.DATM_SSD_RD_CMD_BASE_ADDR = (uint32_t) smSSD;
    rNvmeDataMngr.DATM_SSD_WR_CMD_BASE_ADDR = (uint32_t) smSSD;
    rNvmeDataMngr.ssdListMaxEntryCnt.b.RD_SSD_MAX_CNT = SSD_RW_COUNT + 1;
    rNvmeDataMngr.ssdListMaxEntryCnt.b.WR_SSD_MAX_CNT = SSD_RW_COUNT + 1;

    // Configure subsegment block count
    rNvmeDataMngr.dataManagerSsdFetcherCtrl.b.SUBSEG_BLK_CNT = cSubSegBlockCount;

    // set up generic data transfer buffer region, entire memory space
    // SSD index 0 are reserved for this
}


//-----------------------------------------------------------------------------
/**
    Shutdown completed.

    @param  error    error code
**/
//-----------------------------------------------------------------------------
void HalNvme_ShutdownComplete(void)
{
    //HalNvme_ShutDown(); Do not need it, it will cause IOL test 4.16 fail
    rNvmeController.controller[0].CSTS.b.SHST = 2;   // Shutdown completed.
}
#endif

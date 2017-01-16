// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2014  -  All rights reserved
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
//! @brief Power Management control API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "RegClockPLL.h"

#include "RegNvmePkg.h"

#include "RegApb.h"

#include "HalUart.h"
#include "HalSetm.h"
#include "HalPciePhy.h"
#include "HalPcieLink.h"

#include "IDS.h"

#if _CPU_==CPU_ID0
    #include "HalNvmeXfer.h"
    #include "NvmeFeatures.h"
#elif _CPU_==CPU_ID1
    #include "MediaPkg.h"
    #include "AplNfif.h"
#elif _CPU_==CPU_ID2
    #include "Thermal.h"
#endif

#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define NUM_OF_CONTEXT_REGION   sizeof(memContextRegion) / sizeof(MemRegion_t)
#define NUM_OF_PROTECT_REGION   sizeof(memProtectRegion) / sizeof(MemRegion_t)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct
{
    uint32_t        address;    // memory address
    uint32_t        size;       // byte size
    uint32_t        dma;        // support dma copy
    InitBootMode_t  bootMode;   // mode for save context
} MemRegion_t;

const MemRegion_t memContextRegion[] =
{
#if (_CPU_==CPU_ID0)
    { AXI_ATCM_CPU1_ADDR,   CPU1_ITCM_SIZE,                             TRUE,  cInitBootCold },
    { AXI_ATCM_CPU2_ADDR,   CPU2_ITCM_SIZE,                             TRUE,  cInitBootCold },
    { AXI_B1TCM_CPU0_ADDR,  SHARED_TCM_SIZE,                            TRUE,  cInitBootDeepPowerDown },
    { SHARED_SPM_ADDR,      SHARED_SPM_SIZE,                            TRUE,  cInitBootDeepPowerDown },
#elif (_CPU_==CPU_ID1)
    { AXI_B0TCM_CPU1_ADDR,  (CPU1_DTCM_SIZE - CPU1_DTCM_WAKEUP_SIZE),   TRUE,  cInitBootDeepPowerDown },
#elif (_CPU_==CPU_ID2)
    { AXI_B0TCM_CPU2_ADDR,  (CPU2_DTCM_SIZE - CPU2_DTCM_WAKEUP_SIZE),   TRUE,  cInitBootDeepPowerDown },
#endif
};

const MemRegion_t memProtectRegion[] =
{
#if (_CPU_==CPU_ID0)
    { (AXI_B0TCM_CPU0_ADDR + CPU0_DTCM_SIZE - CPU0_DTCM_WAKEUP_SIZE), CPU0_DTCM_WAKEUP_SIZE, TRUE, cInitBootDeepPowerDown },
#elif (_CPU_==CPU_ID1)
    { (AXI_B0TCM_CPU1_ADDR + CPU1_DTCM_SIZE - CPU1_DTCM_WAKEUP_SIZE), CPU1_DTCM_WAKEUP_SIZE, TRUE, cInitBootDeepPowerDown },
#elif (_CPU_==CPU_ID2)
    { (AXI_B0TCM_CPU2_ADDR + CPU2_DTCM_SIZE - CPU2_DTCM_WAKEUP_SIZE), CPU2_DTCM_WAKEUP_SIZE, TRUE, cInitBootDeepPowerDown },
#endif
};

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
__inline void HalPm_WakeupInitialization(InitBootMode_t initMode);
         void HalPm_BackupContext(InitBootMode_t initMode, uint32_t backupBufAddr, const MemRegion_t region[], uint32_t count);
         void HalPm_RestoreContext(InitBootMode_t initMode, uint32_t backupBufAddr, const MemRegion_t region[], uint32_t count);
__asm    void WakeupDeepPowerDownMode(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
#if (_CPU_==CPU_ID0)
    static PmPowerMode_t    currentPmFctPowerMode;
    static PmPowerMode_t    currentPmApbPowerMode;
    static uint32_t         gBackupCpu0ContextBuf;
           tPmInfo          gPmInfo;
#endif

jmp_buf env_buffer;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
extern uint32_t gIdleEventTimer;

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

#if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
//-----------------------------------------------------------------------------
/**
    One-time initialization of Power Management Controller.\n

    @param[in]  initMode            initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalPm_OneTimeInit(InitBootMode_t initMode)
{
    uint32_t index;
    uint32_t totalBackupSize;

    MEM_SET((void*)&smPmCtrl[_CPU_], 0x00, sizeof(PowerControl_t));

    totalBackupSize = 0;
    for (index = 0; index < NUM_OF_CONTEXT_REGION; index++)
    {
        totalBackupSize += memContextRegion[index].size;
    }

    if (totalBackupSize)
    {
        #if   (_CPU_ == CPU_ID0)
            gBackupCpu0ContextBuf       = (uint32_t)MEM_AllocBuffer(totalBackupSize, 0x200);
        #else
            smPmCtrl[_CPU_].ContextAddr = (uint32_t)MEM_AllocBuffer(totalBackupSize, 0x200);
        #endif
    }

    totalBackupSize = 0;
    for (index = 0; index < NUM_OF_PROTECT_REGION; index++)
    {
        totalBackupSize += memProtectRegion[index].size;
    }

    if (totalBackupSize)
    {
        #if   (_CPU_ != CPU_ID0)
            smPmCtrl[_CPU_].ProtectAddr = (uint32_t)MEM_AllocBuffer(totalBackupSize, 0x200);
        #endif
    }

    #if (_CPU_ == CPU_ID0)
        HalPm_BackupContext(cInitBootCold, gBackupCpu0ContextBuf, memContextRegion, NUM_OF_CONTEXT_REGION);
        MEM_CLR((void*)&gPmInfo, sizeof(gPmInfo));
        gPmInfo.bypassTimerValue = DWORD_MASK;
        gPmInfo.maxPowerState    = POWER_STATE_4;
    #endif

    smPmCtrl[_CPU_].WakeupAddr = HalPm_WakeupDeepPowerDownMode();

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Save memory context to DRAM buffer
**/
//-----------------------------------------------------------------------------
void HalPm_BackupContext(InitBootMode_t bootMode, uint32_t backupBufAddr, const MemRegion_t region[], uint32_t count)
{
    uint32_t index;

    for (index = 0; index < count; index++)
    {
        if (region[index].bootMode == bootMode && region[index].size)
        {
            if (region[index].dma)
            {
                HalDmac_StartCopy(backupBufAddr, region[index].address, region[index].size);
                HalDmac_WaitDone();
            }
            else
            {
                MEM_COPY((void*)backupBufAddr, (void*)region[index].address, region[index].size);
            }
        }

        backupBufAddr += region[index].size;
    }
}

//-----------------------------------------------------------------------------
/**
    Restore memory context from DRAM buffer
**/
//-----------------------------------------------------------------------------
void HalPm_RestoreContext(InitBootMode_t bootMode, uint32_t backupBufAddr, const MemRegion_t region[], uint32_t count)
{
    uint32_t index;

    for (index = 0; index < count; index++)
    {
        if (region[index].size)
        {
            if (region[index].dma)
            {
                HalDmac_StartCopy(region[index].address, backupBufAddr, region[index].size);
                HalDmac_WaitDone();
            }
            else
            {
                MEM_COPY((void*)region[index].address, (void*)backupBufAddr, region[index].size);
            }
        }

        backupBufAddr += region[index].size;
    }
}

#if (_CPU_ == CPU_ID0)
//-----------------------------------------------------------------------------
/**
    Check if host link state is ready to enter deep power down mode or not
**/
//-----------------------------------------------------------------------------
bool HalPm_ReadyToEnterDeepPowerDownMode(void)
{
    if (HalLink_GetPcieLinkSts() == cPcieLinkL12)
    {
        return TRUE;
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
/**
    set media and core power mode
**/
//-----------------------------------------------------------------------------
PmDeviceLock_t HalPm_MediaCoreSetPowerMode(DevicePowerMode_t powerMode)
{
    switch (powerMode)
    {
        case cDevicePmSleep:
        case cDevicePmDeepSleep:
            smPmCtrl[CPU_ID2].PowerMode = powerMode;
            smPmCtrl[CPU_ID2].LockStatus= cPmDeviceNone;
            smPmCtrl[CPU_ID2].ReqLock   = TRUE;
            while (smPmCtrl[CPU_ID2].LockStatus == cPmDeviceNone);

            if (smPmCtrl[CPU_ID2].LockStatus != cPmDeviceLock)
            {
                return smPmCtrl[CPU_ID2].LockStatus;
            }

            smPmCtrl[CPU_ID1].PowerMode = powerMode;
            smPmCtrl[CPU_ID1].LockStatus= cPmDeviceNone;
            smPmCtrl[CPU_ID1].ReqLock   = TRUE;
            while (smPmCtrl[CPU_ID1].LockStatus == cPmDeviceNone);

            return cPmDeviceLock;
        case cDevicePmActive:
            smPmCtrl[CPU_ID1].ReqLock = FALSE;
            smPmCtrl[CPU_ID2].ReqLock = FALSE;

            while ((smPmCtrl[CPU_ID1].PowerMode != powerMode) ||
                   (smPmCtrl[CPU_ID2].PowerMode != powerMode));

            break;
        default:
            HALT();
            break;
    }

    return cPmDeviceNone;
}

//-----------------------------------------------------------------------------
/**
    Send IPC cmd to wakeup media
**/
//-----------------------------------------------------------------------------
void HalPm_IpcSendWakeUpMsg(U32 cpuid)
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnWakeup);

    if      (cpuid == CPU_ID1)  Ipc_SendWaitMsg(cH2mReqQueue,  (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);
    else if (cpuid == CPU_ID2)  Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);
}

//-----------------------------------------------------------------------------
/**
    Send IPC cmd to wakeup core
**/
//-----------------------------------------------------------------------------
Error_t HalPm_IpcSendPowerDownMsg(U32 cpuid, DevicePowerMode_t powerMode)
{
    MsgPM_t* pPmMsg = (MsgPM_t*)&smHcmdMsg[HMSG_EXT_IDX_PM];

    #if 0
    pPmMsg->error        = cEcNoError;
    pPmMsg->hdr.b.opCode = cMcHostPowerDown;
    pPmMsg->powerMode    = powerMode;

    IPC_SendMsgQ(cH2cUartQueue, (Msg_t*) pPmMsg);

    while (pPmMsg->hdr.b.status != cMsgCompleted);
    #endif

    return pPmMsg->error;
}

//-----------------------------------------------------------------------------
/**
    Soc Advanced Peripheral Bus power-up and power-down process .\n

    @param[in]  PowDownMode            Power down mode

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t HalPm_ApbSetPowerMode(PmPowerMode_t powerMode)
{
    SramCtrl1_t     sramCtrl1;
    SramCtrl2_t     sramCtrl2;
    CpuSramCtrl_t   cpuSramCtrl;

    // no set up if power mode is not changed.
    if (currentPmApbPowerMode == powerMode) return cEcNoError;

    switch (powerMode)
    {
        case cPmDeepSleep:
            rApb.rom0Ctrl.b.CPU0_ROM_PDWN_EN  = 1;                          // 0xF0115010[31]: CPU0 ROM Power Down Enable.
            rApb.rom1Ctrl.b.CPU1_ROM_PDWN_EN  = 1;                          // 0xF0115020[31]: CPU1 ROM Power Down Enable.
            rApb.rom2Ctrl.b.CPU2_ROM_PDWN_EN  = 1;                          // 0xF0115030[31]: CPU2 ROM Power Down Enable.

            sramCtrl1 = rApb.sramCtrl1;
            sramCtrl1.b.SPI_SRAM_PWDN          = 1;                         // 0xF01150A0[24]: SPI SRAM Power Down Mode Control.
            sramCtrl1.b.IDS_SRAM_PDWN          = 1;                         // 0xF01150A0[18]: IDS SRAM Power Down Mode Control.
            sramCtrl1.b.AHCI_CMDFIFO_SRAM_PDWN = 1;                         // 0xF01150A0[14]: AHCI Command FIFO SRAM Power Down Mode Control.
            sramCtrl1.b.AHCI_SRAM_PDWN         = 1;                         // 0xF01150A0[6]:  AHCI SRAM Power Down Mode Control.
            rApb.sramCtrl1 = sramCtrl1;

            sramCtrl2 = rApb.sramCtrl2;
            sramCtrl2.b.SPM_SRAM_PDWN          = 1;                         // 0xF01150A4[24]: SPM SRAM Power Down Mode Control.
            sramCtrl2.b.SPM_SRAM_PD_EN         = 1;                         // 0xF01150A4[21]: SPM Power Down Enable
            sramCtrl2.b.SPM_SRAM_PD_TH         = 4;                         // 0xF01150A4[11:0]: SPM Power Down Threshold
            rApb.sramCtrl2 = sramCtrl2;

            cpuSramCtrl = rApb.cpuSramCtrl;
            //cpuSramCtrl.b.ATCM_PDWN           = 1;                          // 0xF01150B0[15]: ATCM SRAM Power Down Mode Control.
            //cpuSramCtrl.b.BTCM_PDWN           = 1;                          // 0xF01150B0[11]: BTCM SRAM Power Down Mode Control.
            cpuSramCtrl.b.CACHE_PDWN          = 1;                          // 0xF01150B0[7]: Cache SRAM Power Down Mode Control.
            cpuSramCtrl.b.ETB_PDWN            = 1;                          // 0xF01150B0[3]: CoreSight ETB SRAM Power Down Mode Cont.
            rApb.cpuSramCtrl = cpuSramCtrl;
            break;

        case cPmActive:
            rApb.rom0Ctrl.b.CPU0_ROM_PDWN_EN  = 0;                          // 0xF0115010[31]: CPU0 ROM Power Down Enable.
            rApb.rom1Ctrl.b.CPU1_ROM_PDWN_EN  = 0;                          // 0xF0115020[31]: CPU1 ROM Power Down Enable.
            rApb.rom2Ctrl.b.CPU2_ROM_PDWN_EN  = 0;                          // 0xF0115030[31]: CPU2 ROM Power Down Enable.

            sramCtrl1 = rApb.sramCtrl1;
            sramCtrl1.b.SPI_SRAM_PWDN          = 0;                         // 0xF01150A0[24]: SPI SRAM Power Down Mode Control.
            sramCtrl1.b.IDS_SRAM_PDWN          = 0;                         // 0xF01150A0[18]: IDS SRAM Power Down Mode Control.
            sramCtrl1.b.AHCI_CMDFIFO_SRAM_PDWN = 0;                         // 0xF01150A0[14]: AHCI Command FIFO SRAM Power Down Mode Control.
            sramCtrl1.b.AHCI_SRAM_PDWN         = 0;                         // 0xF01150A0[6]:  AHCI SRAM Power Down Mode Control.
            rApb.sramCtrl1 = sramCtrl1;

            sramCtrl2 = rApb.sramCtrl2;
            sramCtrl2.b.SPM_SRAM_PDWN          = 0;                         // 0xF01150A4[24]: SPM SRAM Power Down Mode Control.
            sramCtrl2.b.SPM_SRAM_PD_EN         = 0;                         // 0xF01150A4[21]: SPM Power Down Enable
            sramCtrl2.b.SPM_SRAM_PD_TH         = 0;                         // 0xF01150A4[11:0]: SPM Power Down Threshold
            rApb.sramCtrl2 = sramCtrl2;

            cpuSramCtrl = rApb.cpuSramCtrl;
            //cpuSramCtrl.b.ATCM_PDWN           = 0;                          // 0xF01150B0[15]: ATCM SRAM Power Down Mode Control.
            //cpuSramCtrl.b.BTCM_PDWN           = 0;                          // 0xF01150B0[11]: BTCM SRAM Power Down Mode Control.
            cpuSramCtrl.b.CACHE_PDWN          = 0;                          // 0xF01150B0[7]: Cache SRAM Power Down Mode Control.
            cpuSramCtrl.b.ETB_PDWN            = 0;                          // 0xF01150B0[3]: CoreSight ETB SRAM Power Down Mode Cont.
            rApb.cpuSramCtrl = cpuSramCtrl;
            break;

        default:
            break;
    }

    currentPmApbPowerMode = powerMode;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Soc Fct Block power-up and power-down process .\n

    @param[in]  PowDownMode            Power down mode

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t HalPm_FctSetPowerMode(PmPowerMode_t powerMode)
{
    if (currentPmFctPowerMode == powerMode) return cEcNoError;

    switch (powerMode)
    {
        case cPmActive:
            rClockPll.powerCtrl2.b.FCT_PD_REQ  = 0;                          // 0xF011307C[23]: Clear FCT power down request
            rClockPll.powerCtrl2.b.FCT_PU_REQ  = 1;                          // 0xF011307C[22]: Initial FCT power up request
            while (rClockPll.powerCtrl2.b.FCT_PU_ACK == 0);
            break;

        case cPmDeepSleep:
            rClockPll.powerCtrl2.b.FCT_PU_REQ  = 0;                          // 0xF011307C[22]: Clear FCT power up request
            rClockPll.powerCtrl2.b.SEL_NOA_FCT = 1;                          // 0xF011307C[28]: Set to FCT power down mode
            rClockPll.powerCtrl2.b.FCT_PD_REQ  = 1;                          // 0xF011307C[23]: Initial FCT power down request
            while (rClockPll.powerCtrl2.b.FCT_PD_ACK == 0);
            break;

        default:
            break;
    }

    currentPmFctPowerMode = powerMode;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Set one shot timer for swtich power state
**/
//-----------------------------------------------------------------------------
void HalPm_SetOneShotTimer(uint32_t powerState)
{
    //uint32_t elapsedTime = 500 * KERNEL_TICK_PER_MSEC;
    uint32_t elapsedTime = 0;

    if (currentFeatureValue[cAutonomousPowerStateTransition] && powerState < gPmInfo.maxPowerState)
    {
        if  ((gPmInfo.apstTimerTbl[powerState + 1] > gPmInfo.apstTimerTbl[powerState]) &&
            ((gPmInfo.apstTimerTbl[powerState + 1] - gPmInfo.apstTimerTbl[powerState]) > 5))
        {
            elapsedTime = gPmInfo.apstTimerTbl[powerState + 1] - gPmInfo.apstTimerTbl[powerState];
        }
    }

    gPmInfo.lastEsplasedTime = elapsedTime * 1000 / 12;

    if (gPmInfo.lastEsplasedTime)
    {
        HalTimer_SetOneShotTimer(gPmInfo.lastEsplasedTime);
    }
}
#endif


//-----------------------------------------------------------------------------
/**
    OneTimeInit function for S2/S3 wakeup

**/
//-----------------------------------------------------------------------------
__inline void HalPm_WakeupInitialization(InitBootMode_t initMode)
{
    switch (initMode)
    {
        case cInitBootPowerDown:
            #if _CPU_==CPU_ID0

            #elif _CPU_==CPU_ID1
                AplNfif_OneTimeInit(initMode);

            #elif _CPU_==CPU_ID2

            #endif

            break;
        case cInitBootDeepPowerDown:
            #if _CPU_==CPU_ID0

            #elif _CPU_==CPU_ID1
                AplNfif_OneTimeInit(initMode);

            #elif _CPU_==CPU_ID2
                AplThermal_OneTimeInit(initMode);

            #endif

            break;
        default:
            HALT();
            break;
    }
}

//-----------------------------------------------------------------------------
/**
    Enter power down process (S2/PS3).\n

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalPm_EnterExitPowerDownMode(void)
{
    CpuCs_t     orgState;

#if _CPU_==CPU_ID0
    if (HalPm_MediaCoreSetPowerMode(cDevicePmSleep) == cPmDeviceLock)
    {
        HalUart_ResetFIFO(0);

        HalPm_SetOneShotTimer(POWER_STATE_3);

        HalPm_FctSetPowerMode(cPmDeepSleep);
        HalPm_ApbSetPowerMode(cPmDeepSleep);
        HalDram_SetPowerMode(cDramPmSelfRefresh);
        HalClock_SetPowerMode(cClockPmSleep);

        HalIrq_SetPowerMode(cIrqPmSleep);

        HalCpu_WaitForInterrupt();

        HalClock_SetPowerMode(cClockPmActive);
        HalDram_SetPowerMode(cDramPmActive);
        HalPm_ApbSetPowerMode(cPmActive);
        HalPm_FctSetPowerMode(cPmActive);

        HalPm_IpcSendWakeUpMsg(CPU_ID1);
        HalPm_IpcSendWakeUpMsg(CPU_ID2);

        HalIrq_SetPowerMode(cIrqPmActive);

        M_PRINTF("@");
    }
    else
    {
        gIdleEventTimer = 0;
    }

    HalPm_MediaCoreSetPowerMode(cDevicePmActive);

#else
    BEGIN_CS(orgState);

    HalIrq_SetPowerMode(cIrqPmDeepSleep);

    smPmCtrl[_CPU_].LockStatus = cPmDeviceLock;

    HalCpu_WaitForInterrupt();

    HalIrq_SetPowerMode(cIrqPmActive);

    HalPm_WakeupInitialization(cInitBootPowerDown);

    smPmCtrl[_CPU_].PowerMode = cDevicePmActive;

    END_CS(orgState);

#endif

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Enter power down process (S2/PS3).\n

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalPm_EnterDeepPowerDownMode(void)
{
    JumpParam_t jmpValue;
    CpuCs_t     orgState;

#if _CPU_==CPU_ID0

    jmpValue = (JumpParam_t)HalCpu_SetJump(env_buffer);
    if (jmpValue == cFromSetJump)
    {
        HalIrq_SetPowerMode(cIrqPmDeepSleep);
        HalIpc_SetPowerMode(cIpcPmDeepSleep);

        HalPm_BackupContext(cInitBootDeepPowerDown, gBackupCpu0ContextBuf, memContextRegion, NUM_OF_CONTEXT_REGION);

        HalSetm_SetPowerMode(cSetmPmDeepSleep);
        HalSflash_SetPowerMode(cSflashPmDeepSleep);
        HalTimer_SetPowerMode(cTimerPmDeepSleep);
        HalCpu_SetPowerMode(cCpuPmDeepSleep);
        HalUart_SetPowerMode(cUartPmDeepSleep);
        HalGpio_SetPowerMode(cGpioPmDeepSleep);

        if ((HalPm_ReadyToEnterDeepPowerDownMode() == FALSE) || (rClockPll.powerCtrl1.b.CLKREQ_N_RAW == 0))
        {
            M_PRINTF("*");
            return cEcError;
        }

        rClockPll.clkEnCtrl.b.PCIAXI_CLK_EN = 0;

        if ((ADMIN_CMD_FIFO_CNT() != 0) || (IO_CMD_FIFO_CNT() != 0))
        {
            M_PRINTF("+");
            rClockPll.clkEnCtrl.b.PCIAXI_CLK_EN = 1;
            return cEcError;
        }

        HalNvme_SetPowerMode(cNvmePmDeepSleep);
        HalPhy_SetPowerMode(cPciePhyPmDeepSleep);
        HalDram_SetPowerMode(cDramPmDeepSleep);
        HalClock_SetPowerMode(cClockPmDeepSleep);
    }
    else
    {
        HalPm_WakeupInitialization(cInitBootDeepPowerDown);
    }

#else

    BEGIN_CS(orgState);

    jmpValue = (JumpParam_t)HalCpu_SetJump(env_buffer);
    if (jmpValue != cFromSetJump)
    {
        HalPm_RestoreContext(cInitBootDeepPowerDown, smPmCtrl[_CPU_].ProtectAddr, memProtectRegion, NUM_OF_PROTECT_REGION);
        smPmCtrl[_CPU_].WakeUpBusy = FALSE;

        HalPm_WakeupInitialization(cInitBootDeepPowerDown);

        smPmCtrl[_CPU_].PowerMode  = cDevicePmActive;
        END_CS(orgState);

        return cEcNoError;
    }

    HalIrq_SetPowerMode(cIrqPmDeepSleep);
    HalCpu_SetPowerMode(cCpuPmDeepSleep);

    HalPm_BackupContext(cInitBootDeepPowerDown, smPmCtrl[_CPU_].ContextAddr, memContextRegion, NUM_OF_CONTEXT_REGION);
    HalPm_BackupContext(cInitBootDeepPowerDown, smPmCtrl[_CPU_].ProtectAddr, memProtectRegion, NUM_OF_PROTECT_REGION);

    smPmCtrl[_CPU_].LockStatus = cPmDeviceLock;

    while (smPmCtrl[_CPU_].ReqLock);

    HalCpu_SetPowerMode(cCpuPmActive);
    HalIrq_SetPowerMode(cIrqPmActive);

    smPmCtrl[_CPU_].PowerMode = cDevicePmActive;
    END_CS(orgState);

#endif

    return cEcNoError;
}

#if _CPU_==CPU_ID0
//-----------------------------------------------------------------------------
/**
    Exit Deep Power Down mode (S3/PS4).\n

**/
//-----------------------------------------------------------------------------
void HalPm_ExitDeepPowerDownSequence(void)
{
    HalPcieLink_RequestToExitL1(ON);
    HalNvme_SetPowerMode(cNvmePmActive);
    HalClock_SetPowerMode(cClockPmActive);

    HalCpu_DefaultMpuRegionSetting();

    HalPhy_SetPowerMode(cPciePhyPmActive);
    HalPcieLink_RequestToExitL1(OFF);

    HalDram_SetPowerMode(cDramPmActive);
    HalGpio_SetPowerMode(cGpioPmActive);
    HalUart_SetPowerMode(cUartPmActive);
    HalCpu_SetPowerMode(cCpuPmActive);
    HalTimer_SetPowerMode(cTimerPmActive);
    HalSflash_SetPowerMode(cSflashPmActive);
    HalSetm_SetPowerMode(cSetmPmActive);
}
#endif

//-----------------------------------------------------------------------------
/**
    Exit Deep Power Down mode (S3/PS4).\n

**/
//-----------------------------------------------------------------------------
void HalPm_ExitDeepPowerDownMode(void)
{
#if _CPU_==CPU_ID0

    HalPm_ExitDeepPowerDownSequence();

    HalPm_RestoreContext(cInitBootDeepPowerDown, gBackupCpu0ContextBuf, memContextRegion, NUM_OF_CONTEXT_REGION);

    HalIpc_SetPowerMode(cIpcPmActive);

    HalTimer_DelayUsec(1);

    smPmCtrl[CPU_ID1].WakeUpBusy = TRUE;
    HalIpc_JumpToAddress(CPU_ID1, smPmCtrl[CPU_ID1].WakeupAddr);
    while (smPmCtrl[CPU_ID1].WakeUpBusy);

    smPmCtrl[CPU_ID2].WakeUpBusy = TRUE;
    HalIpc_JumpToAddress(CPU_ID2, smPmCtrl[CPU_ID2].WakeupAddr);
    while (smPmCtrl[CPU_ID2].WakeUpBusy);

    HalIrq_SetPowerMode(cIrqPmActive);

    M_PRINTF("~");

#else

    HalPm_RestoreContext(cInitBootDeepPowerDown, smPmCtrl[_CPU_].ContextAddr, memContextRegion, NUM_OF_CONTEXT_REGION);

    HalCpu_SetPowerMode(cCpuPmActive);
    HalIrq_SetPowerMode(cIrqPmActive);

#endif

    HalCpu_LongJump(env_buffer, cFromLongJmp);
}

//-----------------------------------------------------------------------------
/**
    Wake Up Entrance Point (S3/PS4).\n

**/
//-----------------------------------------------------------------------------
uint32_t HalPm_WakeupDeepPowerDownMode(void)
{
    return (uint32_t)WakeupDeepPowerDownMode;
}

//-----------------------------------------------------------------------------
/**
    Wake Up Entrance Point Instance (S3/PS4).\n

**/
//-----------------------------------------------------------------------------
__asm void WakeupDeepPowerDownMode(void)
{
    PRESERVE8

  #if _CPU_==CPU_ID0
    IMPORT IRQ_Stack0
    LDR sp, =IRQ_Stack0-4       ; end of FIQ_Stack0 region
  #elif _CPU_==CPU_ID1
    LDR sp, = CPU1_DTCM_ADDR + CPU1_DTCM_SIZE - 4   ; Since FIQ stack is small, use the last part of DTCM
  #elif _CPU_==CPU_ID2
    LDR sp, = CPU2_DTCM_ADDR + CPU2_DTCM_SIZE - 4   ; Since FIQ stack is small, use the last part of DTCM
  #endif

    IMPORT HalPm_ExitDeepPowerDownMode
    BL  HalPm_ExitDeepPowerDownMode
}
#endif

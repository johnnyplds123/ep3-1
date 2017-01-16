#pragma once
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
//! @file
//! @brief Pure HAL API definition for Power Management control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
enum {
    POWER_STATE_0 = 0,
    POWER_STATE_1,
    POWER_STATE_2,
    POWER_STATE_3,
    POWER_STATE_4,
};

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
    #define PM_MEDIA_LOCK_REQUEST()     smPmCtrl[CPU_ID1].ReqLock
    #define PM_CORE_LOCK_REQUEST()      smPmCtrl[CPU_ID2].ReqLock
#else
    #define PM_MEDIA_LOCK_REQUEST()     FALSE
    #define PM_CORE_LOCK_REQUEST()      FALSE
#endif

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief Power down mode
typedef enum
{
    cPmActive=1,            ///< Enter power down
    cPmDeepSleep=15         ///< Exit Power down
} PmPowerMode_t;

/// @brief Set/Long Jump Parameter;
typedef enum
{
    cFromSetJump=0,        ///< Set Jump return value
    cFromLongJmp           ///< Long Jump return value
} JumpParam_t;

typedef struct
{
    uint32_t    lastEsplasedTime;
    uint32_t    maxPowerState;
    uint32_t    bypassTimerValue;
    uint32_t    apstTimerTbl[NVME_NUM_OF_POWER_STATES];
} tPmInfo;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
#if (_CPU_==CPU_ID0)
    extern tPmInfo gPmInfo;
#endif

extern jmp_buf env_buffer;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t         HalPm_OneTimeInit(InitBootMode_t initMode);
bool            HalPm_ReadyToEnterDeepPowerDownMode(void);
void            HalPm_SetOneShotTimer(uint32_t powerState);
PmDeviceLock_t  HalPm_MediaCoreSetPowerMode(DevicePowerMode_t powerMode);
Error_t         HalPm_EnterExitPowerDownMode(void);
Error_t         HalPm_EnterDeepPowerDownMode(void);
void            HalPm_ExitDeepPowerDownSequence(void);
void            HalPm_ExitDeepPowerDownMode(void);
uint32_t        HalPm_WakeupDeepPowerDownMode(void);
void            HalPm_IpcSendWakeUpMsg(U32 cpuid);
Error_t         HalPm_IpcSendPowerDownMsg(U32 cpuid, DevicePowerMode_t powerMode);

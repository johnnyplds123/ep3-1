// =================================================================================
//
// Copyright (C) 2015 Marvell Semiconductor, Inc.
// All rights reserved.
//
// This file contains information that is confidential and proprietary to Marvell
// Semiconductor, Inc. Commercial use of this code is subject to licensing terms
// and is permitted only on semiconductor devices manufactured by or on behalf of
// Marvell Semiconductor, Inc or its affiliates. Distribution subject to written
// consent from Marvell Semiconductor, Inc.
//
// =================================================================================
// = C O M P A N Y P R O P R I E T A R Y M A T E R I A L =
// =================================================================================
/*******************************************************************************//**
*
* @file AdaptiveVoltageScaling.h
*
* Adjust voltage regulator according to VDD setpoint.
*
***********************************************************************************/


#pragma once

#include "BuildPkg.h"
#include "HalSflash.h"
#if (_CPU_==CPU_ID0) && (_CODE_TYPE_ == CODE_MAIN)
#include "bcmClientSection.h"
#include "BcmClientUtilities.h"
#include "BcmClient.h"
#include "BcmClientTypedef.h"
#endif
//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define SS_VOLTAGE                              950
#define TT_VOLTAGE                              900
#define FF_VOLTAGE                              875
#define AVStag                                  0x00535641      //'AVS '
//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

/// @ Get AVS VddPoint path
typedef enum
{
    cAVSSFLASH,         /// Get AVS from SFlash
  #if (_CODE_TYPE_ == CODE_MAIN)
    cAVSOTP             /// Get AVS from OTP (link by BCM)
  #endif
} AVSReadMode_t;

/// @ AVS variable
typedef struct
{
    uint8_t     isSFlashExist;      // AVS should be store in SFlash at aging stage
    uint8_t     isBCMInitial;
    uint16_t    voltage;            // vddSetPoint voltage
} AVSData_t;
//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern AVSData_t AVSData;
//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
#if (CO_SUPPORT_READSOC == TRUE)
extern uint32_t GetVddSetPointVoltage(uint8_t vddSetPoint);
/**
 *  Initializes the adaptive voltage scaling
 *
 *  @return   Voltage
 */
Error_t AplAVS_OneTimeInit(InitBootMode_t initMode);
Error_t AdaptiveVoltageScaling_GetVddSetPoint(InitBootMode_t initMode, AVSReadMode_t mode);
void AdaptiveVoltageScaling_SetVdd(void);
#endif

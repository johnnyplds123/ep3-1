// =================================================================================
//
//  Copyright (C) 2014-2015 Marvell Semiconductor, Inc.
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
* @file VddSetPoint.c
*
* BCM reads VDD setpoint from OTP and program the voltage regulator
*
*
***********************************************************************************/

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"
// REG
#include "RegAvs.h"
// HAL
#include "HalCpu.h"
// APL
#include "AdaptiveVoltageScaling.h"
#include "SharedVars.h"
// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

#if (_CPU_ == CPU_ID0)
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
// VddSetPoint
#define cVddSetPointMinValidIndex               0x9
#define cVddSetPointMaxValidIndex               0x17
#define cVddSetPointDefaultVoltage              900
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @ AVS struct in SFlash
typedef struct
{
    uint32_t    sAVStag;
    uint8_t     vddSetPointIndex;
}AVS_SF_t;
//-----------------------------------------------------------------------------
//  Private function proto-type definitions:
//-----------------------------------------------------------------------------
void SetVCS(uint32_t voltage);



//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
/// @brief VDD voltage table in mV
const uint32_t cVddSetPointVoltageTable[] = {813,825,838,850,863,875,888,900,913,925,938,950,963,975,988};

__DATA_FAST_UI AVSData_t    AVSData;    // use for boot & normal code, so uninit is needed.
AVS_SF_t    AVS_SF;
//-----------------------------------------------------------------------------
//  Public functions
//-----------------------------------------------------------------------------
#if (CO_SUPPORT_READSOC == TRUE)
Error_t AplAVS_OneTimeInit(InitBootMode_t initMode)
{
    // Initialize AVSData
    AVSData.isSFlashExist = FALSE;
    if ( gBcmInitStat == TRUE )
    {
        AVSData.isBCMInitial = TRUE;
    }
    else
    {
        AVSData.isBCMInitial = FALSE;
    }
    AVSData.voltage = cVddSetPointDefaultVoltage;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Get VddSetPoint from SFlash or BCM by mode
    And write into HW Sysinfo

    @param  mode    AVSReadMode_t
            if mode == cAVSSFLASH   => do this function after HalSflash_OneTimeInit.
            if mode == cAVSOTP      => do this function after BCM_OneTimeInit.
    @return error code
**/
//-----------------------------------------------------------------------------
Error_t AdaptiveVoltageScaling_GetVddSetPoint(InitBootMode_t initMode, AVSReadMode_t mode)
{
    Error_t result = cEcNoError;

    switch(mode)
    {
        case cAVSSFLASH:            // Read SoC Type From SPI
            break;
        case cAVSOTP:               // Read SoC Type From BCM
        {
            if (AVSData.isBCMInitial == FALSE)
            {
                return cEcError;
            }
            uint32_t vddSetPoint = 0;
            STATUS bcmStatus = BCM_otp_read_vdd_setpoint(&vddSetPoint);
            D_PRINTF("[AVS ]OTPStat|%x|VddPt|%x|", bcmStatus, vddSetPoint);
            if ((bcmStatus == STATUS_SUCCESS) && (vddSetPoint >= cVddSetPointMinValidIndex) && (vddSetPoint <= cVddSetPointMaxValidIndex))
            {
                uint8_t i;
                AVS_SF.vddSetPointIndex = vddSetPoint;

                AVSData.isSFlashExist = TRUE;
                AVSData.voltage = GetVddSetPointVoltage(AVS_SF.vddSetPointIndex);
                D_PRINTF("Vol|%dmV\n", AVSData.voltage);

                AVSData.voltage = GetVddSetPointVoltage(vddSetPoint);

                smHwInfo.d.socType.avs = AVSData.voltage;
                smHwInfo.d.socType.tag = AVStag;
                HalSflash_MiscHwInfo_Flush();
            }
            else
            {
                AVSData.voltage = cVddSetPointDefaultVoltage;
                D_PRINTF("[AVS ] Invailid Vdd Point");
            }
        }
    }
    return result;
}

//-----------------------------------------------------------------------------
//  Private functions
//-----------------------------------------------------------------------------

/**
    Get VDD Setpoint voltage \n
    @return voltage
**/
uint32_t GetVddSetPointVoltage(uint8_t vddSetPoint)
{
    return cVddSetPointVoltageTable[vddSetPoint - cVddSetPointMinValidIndex];
}

//-----------------------------------------------------------------------------
/**
    Set VddSetPoint by AVSData.voltage
    This function maybe enter recovery mode, so no log, timer...
    AVS voltage dac reference to Eldora SPEC E version
**/
//-----------------------------------------------------------------------------
void AdaptiveVoltageScaling_SetVdd()
{
    uint8_t dac;
    if((AVSData.voltage >= 640) && (AVSData.voltage <= 1150))   // dac is valid only between 0 and 51. => 51*10 + 640 = 1150
    {
        VddLimitCtrl_t vddLimitCtrlBak;
        dac = (AVSData.voltage-640)/10;
        vddLimitCtrlBak = rAvs.vddLimitCtrl;
        vddLimitCtrlBak.b.LOW_VDD_LIM = dac;
        vddLimitCtrlBak.b.HIGH_VDD_LMT = dac;
        rAvs.vddLimitCtrl = vddLimitCtrlBak;
        HalCpu_DelayUsec(100);  // use cpu delay while entering recovery mode
    }
    //else
    //{
    //  D_PRINTF("AVS: Wrong voltage=%dmV\n",AVSData.voltage);
    //}
}
#endif
#endif

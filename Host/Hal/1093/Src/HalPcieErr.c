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
//! @brief PCIe error management handler
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"

// REG
#include "RegPcie.h"

// HAL
#include "HalPcieErr.h"

// APL

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
    Request handler to manage PCIe FATAL error.\n
    PCIe HW block sends out fatal errors under certain conditions.\n
    In addition FW may want to do any necessary cleanup here.
**/
//-----------------------------------------------------------------------------
void HalPcieErr_HandlePcieFatal(void)
{
    D_PRINTF("[FATAL ERR] = %x\n", rPciePf.deviceCtrlAndStat.all);
}

//-----------------------------------------------------------------------------
/**
    Request handler to manage PCIe NONFATAL error.\n
    PCIe HW block sends out fatal errors under certain conditions.\n
    In addition FW may want to do any necessary cleanup here.
**/
//-----------------------------------------------------------------------------
void HalPcieErr_HandlePcieNonFatal(void)
{
    D_PRINTF("[NFATAL ERR] = %x\n", rPciePf.deviceCtrlAndStat.all);
}

//-----------------------------------------------------------------------------
/**
    Request handler to manage PCIe CORR error.\n
    PCIe HW block sends out fatal errors under certain conditions.\n
    In addition FW may want to do any necessary cleanup here.
**/
//-----------------------------------------------------------------------------
void HalPcieErr_HandlePcieCorrectable(void)
{
    D_PRINTF("[CORR ERR] = %x\n", rPciePf.deviceCtrlAndStat.all);
}

//-----------------------------------------------------------------------------
/**
    Request handler to manage PCIe LMI(Local Management Interface) error.\n
    This is an internal PCIe core error, and may need FW intervention.
**/
//-----------------------------------------------------------------------------
void HalPcieErr_HandlePcieLMI(void)
{
    D_PRINTF("[LMI ERR] = %x\n", rPciePf.deviceCtrlAndStat.all);
}

//-----------------------------------------------------------------------------
/**
    Enable PCIe error reporting to host.\n
**/
//-----------------------------------------------------------------------------
void HalPcieErr_EnableReporting(void)
{
    // enable sending of various error messages for this function
}

//-----------------------------------------------------------------------------
/**
    Enable detection of PCIe error UNCORRECTABLKE.\n
**/
//-----------------------------------------------------------------------------
void HalPcieErr_EnableDetectUncorrectable(void)
{
    // uncorrectable error mask
}

//-----------------------------------------------------------------------------
/**
    Enable detection of PCIe error CORRECTABLKE.\n
**/
//-----------------------------------------------------------------------------
void HalPcieErr_EnableDetectCorrectable(void)
{
    // correctable error mask
}


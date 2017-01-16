#pragma once
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
//! @file
//! @brief Pure HAL API definition for Host Interface Data Link layer HW control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define PCIE_GEN1                   1
#define PCIE_GEN2                   2
#define PCIE_GEN3                   3

#define PCIE_X1                     1
#define PCIE_X2                     2
#define PCIE_X4                     4

#define PCIE_LIMIT_SPEED            PCIE_GEN3
#define PCIE_LIMIT_WIDTH            PCIE_X4

#define ASPM_L1_SLEF_EXIT_TIMEOUT       50000
#define PCIE_REF_CLOCK_AUTO_DETECTION   (DISABLE && (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED))
#define PCIE_SRIS                       DISABLE

#define NVME_CLASS_CODE             0x01080200
#define AHCI_CLASS_CODE             0x01060100
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief PCIe link state
typedef enum
{
    cPcieLinkOther = 0,       ///< Ohter
    cPcieLinkL0,              ///< L0
    cPcieLinkL0s,             ///< L0s
    cPcieLinkL10,             ///< L1.0
    cPcieLinkL1WCLKREQ,       ///< L1 w/CLKREQ
    cPcieLinkL11,             ///< L1.1
    cPcieLinkL12,             ///< L1.2
    cPcieLinkL2               ///< L2
} PcieLinkState_t;

/// @brief LTR Vector Selection
typedef enum
{
    cLtrVector0 =0,         ///< LtrFW0: Send LTR value in RF00800E8h
    cLtrVector1,            ///< LtrFW1: Send LTR value in RF00800ECh
    cLtrVector2,            ///< LtrFW2: Send LTR value in RF00800F0h
    cLtrVector3,            ///< LtrFW3: Send LTR value in RF00800F4h
    cLtrVectorEnd           ///< Invalid
} LtrVectorSel_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t HalLink_OneTimeInit(InitBootMode_t initMode);
Error_t HalLink_Reset(ResetMode_t resetMode);

Error_t HalLink_SetErrorCounter(uint32_t counterType, uint32_t pulseLen);
uint32_t HalLink_ReadErrorCounter(void);
void HalLink_DumpInfo(void);
void HalLink_ReadyToSendPmEnterL23(uint32_t onOff);

// --- For PCIe PM
void HalPcieLink_RequestToSendLtrMessage(LtrVectorSel_t ltrVectorId);
void HalPcieLink_DisplayLinkState(void);
PcieLinkState_t HalLink_GetPcieLinkSts(void);
void HalPcieLink_RequestToExitL1(uint32_t onOff);

void HalPcieLink_HandleNssr(void);
void HalPcieLink_HandleFLR(void);

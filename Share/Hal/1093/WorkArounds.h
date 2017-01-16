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
//! @brief SoC Work around
//!
//=============================================================================

//-----------------------------------------------------------------------------
// SoC Work around options - Host
//-----------------------------------------------------------------------------
// PCIe
#define FIX_NVME_FETCH_ONE_COMMAND_AT_A_TIME            ENABLE                  ///< Limit an Arbitration Burst size to prevent Command Fetch Stop issue.
#define FIX_NVME_IOCMD_DWORD_SIZE_TRANSFER              ENABLE                  ///< Use HW Admin cmd to transfer non-block sized data transfer for I/O command (ex. Data set managment command)
#define FIX_NVME_FETCH_CMD_DEADLOCK_CQ_FULL             ENABLE                  ///< workaround for unable to fetch admin cmd when CQ is full
#define FIX_SAC_RW_ERR_INT_CLEAR_ISSUE                  ENABLE                  ///< Need to clear those interrupts by using RD014h[13:12] instread of RD014h[15:14].

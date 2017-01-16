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
//-----------------------------------------------------------------------------
// Title         : BCM Utilities
//-----------------------------------------------------------------------------
// File          : BcmClientUtilities.h
//-----------------------------------------------------------------------------
// Description :
//   - Include all basic utilities for accessing BCM & tools
//-----------------------------------------------------------------------------
// Copyright (c) 2014 by Marvell Semiconductor This model is the confidential and
// proprietary property of Marvell Semiconductor and the possession or use of this
// file requires a written license from Marvell Semiconductor.
//------------------------------------------------------------------------------
// Modification history :
// 
//-----------------------------------------------------------------------------

#include "BcmClientPrintf.h"
#include "BcmClientTypedef.h"       // Contains BCM internal data type & Register Offset


//---------------------------------------------------------------------------------------+
//                                                                         Configuration |
//---------------------------------------------------------------------------------------+
typedef struct BcmClientVars
{
    uint8_t  eromLoaded;
    uint8_t  eromLoadStarted;
    uint16_t rd_err_cnt;    
    uint32_t ipcCmdTimeout; 
    uint32_t testLoops;     
    volatile STATUS   originStatus;
    volatile STATUS   firstStatus;
    volatile STATUS   delayedStatus;
    volatile uint32_t stSave;        
    uint32_t stReads;       
    uint32_t stReadsMax;    
} BcmClientVars;


extern BcmClientVars bcmClientVars;
//extern volatile MAILBOX *pMbox;


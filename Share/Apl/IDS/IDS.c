// ============================================================================
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
/*
    This computer program contains confidential and proprietary information,
    and may NOT be reproduced or transmitted, in whole or in part, in any form,
    or by any means electronic, mechanical, photo-optical, or otherwise, and
    may NOT be translated into another language without the express written
    permission from LITEON Corporation.
*/
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
//=============================================================================
//
//! @file
//! @brief description for example code
//!
//=============================================================================


//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#if _CPU_==CPU_ID0    // CPU0 only
    #include "RegNvmePkg.h"
#endif

#include "IDS.h"

#include "FastSection.h"
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
//  Imported external variable
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
    Shared Function
**/
//-----------------------------------------------------------------------------
#if _CPU_==CPU_ID0
//-----------------------------------------------------------------------------
/*
  Description : IDS one time initialization code
  Input       : initMode    : initial mode
  return      : Error_t     : error code
*/
//-----------------------------------------------------------------------------
Error_t IDS_OneTimeInit(InitBootMode_t initMode)
{
    HardwareLnkListMemAllocation_t hardwareLnkListMemAllocation;
    HardwareLnkListCfg11_t hwllCfg;
    HardwareLnkListEcuAccessCtrl010_t hwllEcuAccessCtrl;
    uint8_t* pBufferIds = NULL;

    #if (ENABLE_READ_IDS_USE_DRAM == TRUE)
    if (initMode != cInitBootDeepPowerDown)
    {
        // set up external buffer region
        // IDS can use 128 AU buffer including internal and external memory.
        pBufferIds = MEM_AllocBuffer((MAX_IDS_AU_COUNT - (HW_IDS_SIZE / AU_SIZE)) * AU_SIZE, 512);      // after IRQ??
    }
    #endif

    // set up external buffer region
    // IDS can use 128 AU buffer including internal and external memory.

    // Base address for IDS and DDR
    rNvmeHwll.HWLL_INT_MEM_BASE_ADDR = HW_IDS_ADDR;
    rNvmeHwll.HWLL_EXT_MEM_BASE_ADDR = (uint32_t) pBufferIds;

    hardwareLnkListMemAllocation.all = 0;
    hardwareLnkListMemAllocation.b.HWLL_PRI_MEM_SIZE = HW_IDS_SIZE / AU_SIZE;

    #if (ENABLE_READ_IDS_USE_DRAM == TRUE)
    hardwareLnkListMemAllocation.b.HWLL_SEC_MEM_SIZE = (MAX_IDS_AU_COUNT - (HW_IDS_SIZE / AU_SIZE));
    #else
    hardwareLnkListMemAllocation.b.HWLL_SEC_MEM_SIZE = 0;
    #endif

    rNvmeHwll.hardwareLnkListMemAllocation.all = hardwareLnkListMemAllocation.all;

    // Setup Hardware Link List
    hwllCfg.all = rNvmeHwll.hardwareLnkListCfg1_1.all;
    hwllCfg.b.HWLL_LNK_SRCH_MD = 0;                     // Link Search Mode (mode 0 by cmdIndex, mode 1 by cmdIndex and auIndex)
    hwllCfg.b.HWLL_RDDAT_RDY_BM_UPDT = 1;
    hwllCfg.b.HWLL_AU_MEM_BLK_SIZE = (AU_SIZE>>2);      // AU Memory Block Size (Dwords)
    rNvmeHwll.hardwareLnkListCfg1_1.all = hwllCfg.all;

    // Enable the PRP starvation prevention feature
    rNvmeDataMngr.dataManagerCtrl0.b.TD64_BM_MODE = 0;

    // Zerorize HT table
    rNvmeHwll.hardwareLnkListEcuAccessCtrl01_0.b.FW_INIT_HT_REQ = 1;
    while(rNvmeHwll.hardwareLnkListEcuAccessCtrl01_0.b.FW_INIT_HT_DONE == 0);   // Wait done (Firmware Head/Tail Initialization)
    hwllEcuAccessCtrl.all = rNvmeHwll.hardwareLnkListEcuAccessCtrl01_0.all;
    hwllEcuAccessCtrl.b.FW_INIT_HT_REQ = 0;
    hwllEcuAccessCtrl.b.FW_INIT_HT_DONE = 0;
    rNvmeHwll.hardwareLnkListEcuAccessCtrl01_0.all = hwllEcuAccessCtrl.all;

    return cEcNoError;
}
#endif

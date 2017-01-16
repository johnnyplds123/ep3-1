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

#if (_CPU_ == CPU_ID2)
#include "Gc.h"
#include "Cache.h"
#include "Ftl.h"
#include "CoreDbg.h"
#endif

#include "SysInfo.h"
#include "HalSflash.h"
#include "UncTable.h"
#include "Purge.h"


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

static void Purge_SysInfo(U64 PurgeBitMap);
static void Purge_NOR(U64 PurgeBitMap);
static void Purge_Core(U64 PurgeBitMap);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

tPG_NORTABLE gPurgeNORTable[] =
{
    { SFH_FW_START,         SFH_FW_COUNT,          },
    { SFH_HWINFO_START,     SFH_HWINFO_COUNT,      },
    { SFH_QUICK_IDX_START,  SFH_QUICK_IDX_COUNT,   },
    { SFH_CRC_START,        SFH_CRC_COUNT,         },
    { SFH_TCG_START,        SFH_TCG_COUNT,         },
    {SFH_OVERRIDRFTL_START, SFH_OVERRIDRFTL_COUNT, },
};
//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------


#if (_CPU_ == CPU_ID2)
//-----------------------------------------------------------------------------
/*
  Description : CPU2 AP Call
*/
//-----------------------------------------------------------------------------
static void Purge_Core(U64 PurgeBitMap)
{
    if (PurgeBitMap & PURGE_CORE_FTL)    FTL_Preformat();
}

//-----------------------------------------------------------------------------
/*
  Description : Purge_SysInfo
  Input       : detailBM
  return      : tPG_ERR
*/
//-----------------------------------------------------------------------------
static void Purge_SysInfo(U64 PurgeBitMap)
{
    SyIn_Purge(PurgeBitMap, SYSINFO_WRITE);
}

//-----------------------------------------------------------------------------
/*
  Description : NAND Flash Clear
*/
//-----------------------------------------------------------------------------
static void Purge_NAND(U64 PurgeBitMap)
{
    tPAA paa;
    paa.all32 = 0;

    if (PurgeBitMap & PURGE_NAND_SYSTEM_WO) paa.b.block = SYSINFO_NAND_SBLK_FINAL;
    if (PurgeBitMap & PURGE_MASK_NAND) CoreDbg_Center(COREDBG_CMD_ERASEALL, NULL, paa, NULL, NULL, NULL);
}
#endif

#if (_CPU_ == CPU_ID0 || _CPU_ == CPU_ID2)
//-----------------------------------------------------------------------------
/*
  Description : NOR Flash Clear
*/
//-----------------------------------------------------------------------------
static void Purge_NOR(U64 PurgeBitMap)
{
    U32      NORArea;

    for (NORArea = PURGE_NOR_START; NORArea < (PURGE_NOR_SPLITCNT + PURGE_NOR_START); NORArea++)
    {
        if (PurgeBitMap & BIT(NORArea))
        {
            if (BIT(NORArea) & PURGE_NOR_FTLOVERRIDE)
            {
                HalSflash_OverRideFTL_Sync();
                continue;
            }
            HalSflash_EraseSector((U32)gPurgeNORTable[(NORArea - PURGE_NOR_START)].dwStartSector, gPurgeNORTable[(NORArea - PURGE_NOR_START)].dwSectorCnt, 3);
        }
    }
}
#endif
#if (_CPU_ == CPU_ID0)
//-----------------------------------------------------------------------------
/*
  Description : cpu0 can handle function
*/
//-----------------------------------------------------------------------------
static void Purge_CPU0(U64 PurgeBitMap)
{
    if (PurgeBitMap & PURGE_CORE_UNCT)       UNCT_CHECK_DO_RESET();
    if (PurgeBitMap & PURGE_MASK_NOR)        Purge_NOR(PurgeBitMap);
}
#endif
//-----------------------------------------------------------------------------
/*
  Description : AP For Others
*/
//-----------------------------------------------------------------------------
tPG_ERR Purge_Center(U64 PurgeBitMap)
{
    tPG_ERR error = PURGE_ERR_NON;
#if _CPU_ == CPU_ID0
    if (PurgeBitMap & PURGE_MASK_CPU0)
    {
        Purge_CPU0(PurgeBitMap);
        BIT_CLR(PurgeBitMap, PURGE_MASK_CPU0);
    }
    if (PurgeBitMap)
    {
        MsgPG_t* pHcmdMsg;
        ASM_DMB();

        pHcmdMsg = (MsgPG_t*)Ipc_GetExtendMsg(cMcPurge);
        pHcmdMsg->pgBmp= PurgeBitMap;

        IPC_SendMsgQ(cH2cReqQueue, (Msg_t*)pHcmdMsg);
        while (pHcmdMsg->hdr.b.status != cMsgCompleted);
    }
#elif _CPU_ == CPU_ID2
    Purge_Core(PurgeBitMap);
    Purge_NAND(PurgeBitMap);
    Purge_NOR(PurgeBitMap);
    Purge_SysInfo(PurgeBitMap);
#endif
    return error;
}


/* End of File */

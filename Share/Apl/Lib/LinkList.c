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
//! @brief  Circular buffer management code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "LinkList.h"

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
    Doublly linked list iteratation call back.

    @param[in]  pList           List start pointer
    @param[in]  callBack        call back function while iteration
    @param[in]  offset          structure offset of link entry

    @return  none
**/
//-----------------------------------------------------------------------------
void DlForEachCallBack(LinkHeadTail_t* pList, LlCallBack_t callBack, uint32_t offset)
{
    DlinkEntry_t* pEntry;

    DL_FOR_EACH(pEntry, pList)
    {
        callBack((void*)((uint8_t *)pEntry-offset));
    }
}



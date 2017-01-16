#pragma once
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
//! @brief Error Handle
//=============================================================================
//-----------------------------------------------------------------------------
//  Include
//-----------------------------------------------------------------------------
#include "Fio.h"

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define DBG_ERH                                TRUE

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#if (DBG_ERH == TRUE)
  #define D_PRINTF_ERH(F, args...)             D_PRINTF("[ERH ] "F, ##args)
#else
  #define D_PRINTF_ERH(F, args...)             {}
#endif
//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void ErrHandle_OneTimeInit(InitBootMode_t initMode);
void ErrHandle_MarkError(tFIO_JOB* pJob);
void ErrHandle_MarkDefect(tFIO_JOB* pJob);


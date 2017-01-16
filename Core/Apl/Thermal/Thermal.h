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

//-----------------------------------------------------------------------------
//  Constants definitions: THERMAL_THROTTLE (TT)
//-----------------------------------------------------------------------------
#define TT_TICK_PER_SEC     1

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define TT_PROTECT_START    BEGIN_CS1
#define TT_PROTECT_END      END_CS1

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef enum
{
    TT_MT_0 = 0,    // FULL ACTIVE
    TT_MT_1,
    TT_MT_2,
    TT_MT_COUNT
} tTT_MT;

typedef enum
{
    TT_PS_0 = 0,
    TT_PS_1,
    TT_PS_2,
    TT_PS_COUNT
} tTT_PS;

typedef struct
{
    U32 mt2_th;     ///< Heavy Throttling temperature threshold in degrees K[15:00]
    U32 mt1_th;     ///< Light Throttling temperature threshold in degrees K[31:16]
} tTEMP_MT_TH;
COMPILE_ASSERT(sizeof(tTEMP_MT_TH) == 8, "tTEMP_MT_TH size shall be 8 bytes");

//-----------------------------------------------------------------------------
#ifdef DBG_UART_DYNAMIC_BAUDRATE
typedef struct _tTT_UART_SET
{
    U8  ClkDividerSource;
    U8  ClkDividerBaudrate;
    U8  OverSampleSet[4];
} tTT_UART_SET;
#endif

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern tTEMP_MT_TH gCurrMtTh;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void AplThermal_OneTimeInit(InitBootMode_t initMode);
void AplThermal_ThrottleEntry(void);
void AplThermal_ThrottleException(void);

S8   AplThermal_GetI2cTemperature(void);

/* End of File */

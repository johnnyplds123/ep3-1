// ============================================================================
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
//  This computer program contains confidential and proprietary information,
//  and may NOT be reproduced or transmitted, in whole or in part, in any form,
//  or by any means electronic, mechanical, photo-optical, or otherwise, and
//  may NOT be translated into another language without the express written
//  permission from LITEON Corporation.
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define TCG_SMBR_LAA_START          0x0000  // size = 0x2000  pages  = 0x8000000 bytes(128MB)
#define TCG_SMBR_LAA_END            0x1FFF  // size = 0x2000  pages  = 0x8000000 bytes(128MB)

#define TCG_G1_LAA0                 0x2000  // 1 laa = 16K for G1
#define TCG_G2_LAA0                 0x2001  // 2 laa = 32K for G2
#define TCG_G2_LAA1                 0x2002  //
#define TCG_G3_LAA0                 0x2003  // 2 laa = 32K for G3
#define TCG_G3_LAA1                 0x2004  //

#define TCG_DS_LAA_START            0x2005  // size = 0x280  pages  = 0xA00000 bytes(10MB)
#define TCG_DS_LAA_END              0x228C
#define TCG_DUMMY_LAA               0x228D  // this is for  TcgTempMbrClean()
#define TCG_DS_DUMMY_LAA            0x228E  // this is for  TcgTempDSClean()

#define TCG_G1_DEFAULT_LAA0         0x228F
#define TCG_G2_DEFAULT_LAA0         0x2290
#define TCG_G2_DEFAULT_LAA1         0x2291
#define TCG_G3_DEFAULT_LAA0         0x2292
#define TCG_G3_DEFAULT_LAA1         0x2293

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define _cf(a)                  TCGNFIF##_##a
#define cf(a, b)                _cf(a##b)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef enum{
    ZONE_GRP1,
    ZONE_GRP2,
    ZONE_GRP3,
    ZONE_GRP4,
    ZONE_SMBR,
    ZONE_DS,
    ZONE_TGRP1,
    ZONE_TGRP2,
    ZONE_TGRP3,
    ZONE_GRP5,
    ZONE_TDS,
    ZONE_TSMBR,
    ZONE_LAST
}tTcgGrpDef;

typedef enum
{
    MSG_TCG_G1RD = 0,
    MSG_TCG_G1WR,
    MSG_TCG_TG1RD,
    MSG_TCG_TG1WR,
    MSG_TCG_G2RD,
    MSG_TCG_G2WR,
    MSG_TCG_TG2RD,
    MSG_TCG_TG2WR,
    MSG_TCG_G3RD,
    MSG_TCG_G3WR,
    MSG_TCG_TG3RD,
    MSG_TCG_TG3WR,
    MSG_TCG_SMBRRD,
    MSG_TCG_SMBRWR,
    MSG_TCG_SMBRCOMMIT,
    MSG_TCG_SMBRCLEAR,
    MSG_TCG_TSMBRCLEAR,
    MSG_TCG_DSRD,
    MSG_TCG_DSWR,
    MSG_TCG_DSCOMMIT,
    MSG_TCG_DSCLEAR,
    MSG_TCG_TDSCLEAR,
    MSG_TCG_G5FTL,
    MSG_TCG_G4FTL,
    MSG_TCG_G1RDDEFAULT,
    MSG_TCG_G2RDDEFAULT,
    MSG_TCG_G3RDDEFAULT,
    MSG_TCG_G5RDDEFAULT,
    MSG_TCG_G4RDDEFAULT,
    MSG_TCG_G5WRDEFAULT,
    MSG_TCG_G4WRDEFAULT,
    MSG_TCG_G5BUILDDEFECT,
    MSG_TCG_G4BUILDDEFECT,
    MSG_TCG_G5DMYRD,
    MSG_TCG_G4DMYRD,
    MSG_TCG_G5DMYWR,
    MSG_TCG_G4DMYWR,
    MSG_TCG_SETGRPADDR,
    MSG_TCG_SETBUFADDR,
    MSG_TCG_GXRD,
    MSG_TCG_INIT_ZEROCACHE,
    MSG_TCG_REGEN_ZEROCACHE,
    MSG_TCG_NOREEP_INIT,
    MSG_TCG_NOREEP_RD,
    MSG_TCG_NOREEP_WR,
    MSG_TCG_NFIF_LAST,
} tMSG_TCG_SUBOP;

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
extern tTcgLogAddr  *pMBR_TEMPL2PTBL;
extern tTcgLogAddr  *pDS_TEMPL2PTBL;
extern U16          TCG_FIO_F_MUTE;
extern U8           *pTcgTmpBuf;

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Public function prototype definitions:
//-----------------------------------------------------------------------------
extern int TCGNFIF_Start(MsgTcg_t* pHcmdMsg);



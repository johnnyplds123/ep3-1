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
//! @brief FLC(Flash Link Controller) control API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "RegNandFlashIf.h"

#include "HalFlexible.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
const U32 NandCmdEntry[256] = {
/*000*/(CEB_DES | CLE_H | ALE_L | WEB_L | REB_H | DQS_H | DQ(0)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//CLE high & WE low & prepare for 5Ch
/*001*/(CEB_DES | CLE_H | ALE_L | WEB_H | REB_H | DQS_H | DQ(0)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching CMD 5Ch
/*002*/(CEB_DES | CLE_H | ALE_L | WEB_L | REB_H | DQS_H | DQ(1)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & prepare for C5h
/*003*/(CEB_DES | CLE_H | ALE_L | WEB_H | REB_H | DQS_H | DQ(1)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching CMD C5h
/*004*/(CEB_DES | CLE_H | ALE_L | WEB_L | REB_H | DQS_H | DQ(2)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & prepare for 55h
/*005*/(CEB_DES | CLE_H | ALE_L | WEB_H | REB_H | DQS_H | DQ(2)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching CMD 55h
/*006*/(CEB_DES | CLE_L | ALE_H | WEB_L | REB_H | DQS_H | DQ(3)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & ALE high & prepare for Addr 04h
/*007*/(CEB_DES | CLE_L | ALE_H | WEB_H | REB_H | DQS_H | DQ(3)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching Addr 04h
/*008*/(CEB_DES | CLE_L | ALE_L | WEB_L | REB_H | DQS_H | DQ(0)  | DQ_DES | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & prepare for Data 00h
/*009*/(CEB_DES | CLE_L | ALE_L | WEB_H | REB_H | DQS_H | DQ(0)  | DQ_DES | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching Data 00h

/*010*/(CEB_DES | CLE_H | ALE_L | WEB_L | REB_H | DQS_H | DQ(2)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & prepare for 55h
/*011*/(CEB_DES | CLE_H | ALE_L | WEB_H | REB_H | DQS_H | DQ(2)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching CMD 55h
/*012*/(CEB_DES | CLE_L | ALE_H | WEB_L | REB_H | DQS_H | DQ(4)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & ALE high & prepare for Addr 08h
/*013*/(CEB_DES | CLE_L | ALE_H | WEB_H | REB_H | DQS_H | DQ(4)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching Addr 08h
/*014*/(CEB_DES | CLE_L | ALE_L | WEB_L | REB_H | DQS_H | DQ(1)  | DQ_DES | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & prepare for Data 00h
/*015*/(CEB_DES | CLE_L | ALE_L | WEB_H | REB_H | DQS_H | DQ(1)  | DQ_DES | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching Data 00h

/*016*/(CEB_DES | CLE_H | ALE_L | WEB_L | REB_H | DQS_H | DQ(2)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & prepare for 55h
/*017*/(CEB_DES | CLE_H | ALE_L | WEB_H | REB_H | DQS_H | DQ(2)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching CMD 55h
/*018*/(CEB_DES | CLE_L | ALE_H | WEB_L | REB_H | DQS_H | DQ(5)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & ALE high & prepare for Addr 09h
/*019*/(CEB_DES | CLE_L | ALE_H | WEB_H | REB_H | DQS_H | DQ(5)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching Addr 09h
/*020*/(CEB_DES | CLE_L | ALE_L | WEB_L | REB_H | DQS_H | DQ(2)  | DQ_DES | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & prepare for Data 70h
/*021*/(CEB_DES | CLE_L | ALE_L | WEB_H | REB_H | DQS_H | DQ(2)  | DQ_DES | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching Data 70h

/*022*/(CEB_DES | CLE_H | ALE_L | WEB_L | REB_H | DQS_H | DQ(2)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & prepare for 55h
/*023*/(CEB_DES | CLE_H | ALE_L | WEB_H | REB_H | DQS_H | DQ(2)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching CMD 55h
/*024*/(CEB_DES | CLE_L | ALE_H | WEB_L | REB_H | DQS_H | DQ(6)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & ALE high & prepare for Addr 0Ah
/*025*/(CEB_DES | CLE_L | ALE_H | WEB_H | REB_H | DQS_H | DQ(6)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching Addr 0Ah
/*026*/(CEB_DES | CLE_L | ALE_L | WEB_L | REB_H | DQS_H | DQ(3)  | DQ_DES | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & prepare for Data 00h
/*027*/(CEB_DES | CLE_L | ALE_L | WEB_H | REB_H | DQS_H | DQ(3)  | DQ_DES | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching Data 00h

/*028*/(CEB_DES | CLE_H | ALE_L | WEB_L | REB_H | DQS_H | DQ(2)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & prepare for 55h
/*029*/(CEB_DES | CLE_H | ALE_L | WEB_H | REB_H | DQS_H | DQ(2)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching CMD 55h
/*030*/(CEB_DES | CLE_L | ALE_H | WEB_L | REB_H | DQS_H | DQ(7)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & ALE high & prepare for Addr 0Bh
/*031*/(CEB_DES | CLE_L | ALE_H | WEB_H | REB_H | DQS_H | DQ(7)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching Addr 0Bh
/*032*/(CEB_DES | CLE_L | ALE_L | WEB_L | REB_H | DQS_H | DQ(4)  | DQ_DES | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & prepare for Data 00h
/*033*/(CEB_DES | CLE_L | ALE_L | WEB_H | REB_H | DQS_H | DQ(4)  | DQ_DES | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching Data 00h

/*034*/(CEB_DES | CLE_H | ALE_L | WEB_L | REB_H | DQS_H | DQ(2)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & prepare for 55h
/*035*/(CEB_DES | CLE_H | ALE_L | WEB_H | REB_H | DQS_H | DQ(2)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching CMD 55h
/*036*/(CEB_DES | CLE_L | ALE_H | WEB_L | REB_H | DQS_H | DQ(8)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & ALE high & prepare for Addr 0Ch
/*037*/(CEB_DES | CLE_L | ALE_H | WEB_H | REB_H | DQS_H | DQ(8)  | DQ_TBL | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching Addr 0Ch
/*038*/(CEB_DES | CLE_L | ALE_L | WEB_L | REB_H | DQS_H | DQ(5)  | DQ_DES | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE low & prepare for Data 00h
/*039*/(CEB_DES | CLE_L | ALE_L | WEB_H | REB_H | DQS_H | DQ(5)  | DQ_DES | DQ_OUT | NOLATCH | EXECYC(MIN_EXECYC_RRT) | END_L),//WE high latching Data 00h
/*040*/(END_H),//RSV
/*041*/(END_H),//RSV
/*042*/(END_H),//RSV
/*043*/(END_H),//RSV
/*044*/(END_H),//RSV
/*045*/(END_H),//RSV
/*046*/(END_H),//RSV
/*047*/(END_H),//RSV
/*048*/(END_H),//RSV
/*049*/(END_H),//RSV
/*050*/(END_H),//RSV
/*051*/(END_H),//RSV
/*052*/(END_H),//RSV
/*053*/(END_H),//RSV
/*054*/(END_H),//RSV
/*055*/(END_H),//RSV
/*056*/(END_H),//RSV
/*057*/(END_H),//RSV
/*058*/(END_H),//RSV
/*059*/(END_H),//RSV
/*060*/(END_H),//RSV
/*061*/(END_H),//RSV
/*062*/(END_H),//RSV
/*063*/(END_H),//RSV
/*064*/(END_H),//RSV
/*065*/(END_H),//RSV
/*066*/(END_H),//RSV
/*067*/(END_H),//RSV
/*068*/(END_H),//RSV
/*069*/(END_H),//RSV
/*070*/(END_H),//RSV
/*071*/(END_H),//RSV
/*072*/(END_H),//RSV
/*073*/(END_H),//RSV
/*074*/(END_H),//RSV
/*075*/(END_H),//RSV
/*076*/(END_H),//RSV
/*077*/(END_H),//RSV
/*078*/(END_H),//RSV
/*079*/(END_H),//RSV
/*080*/(END_H),//RSV
/*081*/(END_H),//RSV
/*082*/(END_H),//RSV
/*083*/(END_H),//RSV
/*084*/(END_H),//RSV
/*085*/(END_H),//RSV
/*086*/(END_H),//RSV
/*087*/(END_H),//RSV
/*088*/(END_H),//RSV
/*089*/(END_H),//RSV
/*090*/(END_H),//RSV
/*091*/(END_H),//RSV
/*092*/(END_H),//RSV
/*093*/(END_H),//RSV
/*094*/(END_H),//RSV
/*095*/(END_H),//RSV
/*096*/(END_H),//RSV
/*097*/(END_H),//RSV
/*098*/(END_H),//RSV
/*099*/(END_H),//RSV
/*100*/(END_H),//RSV
/*101*/(END_H),//RSV
/*102*/(END_H),//RSV
/*103*/(END_H),//RSV
/*104*/(END_H),//RSV
/*105*/(END_H),//RSV
/*106*/(END_H),//RSV
/*107*/(END_H),//RSV
/*108*/(END_H),//RSV
/*109*/(END_H),//RSV
/*110*/(END_H),//RSV
/*111*/(END_H),//RSV
/*112*/(END_H),//RSV
/*113*/(END_H),//RSV
/*114*/(END_H),//RSV
/*115*/(END_H),//RSV
/*116*/(END_H),//RSV
/*117*/(END_H),//RSV
/*118*/(END_H),//RSV
/*119*/(END_H),//RSV
/*120*/(END_H),//RSV
/*121*/(END_H),//RSV
/*122*/(END_H),//RSV
/*123*/(END_H),//RSV
/*124*/(END_H),//RSV
/*125*/(END_H),//RSV
/*126*/(END_H),//RSV
/*127*/(END_H),//RSV
/*128*/(END_H),//RSV
/*129*/(END_H),//RSV
/*130*/(END_H),//RSV
/*131*/(END_H),//RSV
/*132*/(END_H),//RSV
/*133*/(END_H),//RSV
/*134*/(END_H),//RSV
/*135*/(END_H),//RSV
/*136*/(END_H),//RSV
/*137*/(END_H),//RSV
/*138*/(END_H),//RSV
/*139*/(END_H),//RSV
/*140*/(END_H),//RSV
/*141*/(END_H),//RSV
/*142*/(END_H),//RSV
/*143*/(END_H),//RSV
/*144*/(END_H),//RSV
/*145*/(END_H),//RSV
/*146*/(END_H),//RSV
/*147*/(END_H),//RSV
/*148*/(END_H),//RSV
/*149*/(END_H),//RSV
/*150*/(END_H),//RSV
/*151*/(END_H),//RSV
/*152*/(END_H),//RSV
/*153*/(END_H),//RSV
/*154*/(END_H),//RSV
/*155*/(END_H),//RSV
/*156*/(END_H),//RSV
/*157*/(END_H),//RSV
/*158*/(END_H),//RSV
/*159*/(END_H),//RSV
/*160*/(END_H),//RSV
/*161*/(END_H),//RSV
/*162*/(END_H),//RSV
/*163*/(END_H),//RSV
/*164*/(END_H),//RSV
/*165*/(END_H),//RSV
/*166*/(END_H),//RSV
/*167*/(END_H),//RSV
/*168*/(END_H),//RSV
/*169*/(END_H),//RSV
/*170*/(END_H),//RSV
/*171*/(END_H),//RSV
/*172*/(END_H),//RSV
/*173*/(END_H),//RSV
/*174*/(END_H),//RSV
/*175*/(END_H),//RSV
/*176*/(END_H),//RSV
/*177*/(END_H),//RSV
/*178*/(END_H),//RSV
/*179*/(END_H),//RSV
/*180*/(END_H),//RSV
/*181*/(END_H),//RSV
/*182*/(END_H),//RSV
/*183*/(END_H),//RSV
/*184*/(END_H),//RSV
/*185*/(END_H),//RSV
/*186*/(END_H),//RSV
/*187*/(END_H),//RSV
/*188*/(END_H),//RSV
/*189*/(END_H),//RSV
/*190*/(END_H),//RSV
/*191*/(END_H),//RSV
/*192*/(END_H),//RSV
/*193*/(END_H),//RSV
/*194*/(END_H),//RSV
/*195*/(END_H),//RSV
/*196*/(END_H),//RSV
/*197*/(END_H),//RSV
/*198*/(END_H),//RSV
/*199*/(END_H),//RSV
/*200*/(END_H),//RSV
/*201*/(END_H),//RSV
/*202*/(END_H),//RSV
/*203*/(END_H),//RSV
/*204*/(END_H),//RSV
/*205*/(END_H),//RSV
/*206*/(END_H),//RSV
/*207*/(END_H),//RSV
/*208*/(END_H),//RSV
/*209*/(END_H),//RSV
/*210*/(END_H),//RSV
/*211*/(END_H),//RSV
/*212*/(END_H),//RSV
/*213*/(END_H),//RSV
/*214*/(END_H),//RSV
/*215*/(END_H),//RSV
/*216*/(END_H),//RSV
/*217*/(END_H),//RSV
/*218*/(END_H),//RSV
/*219*/(END_H),//RSV
/*220*/(END_H),//RSV
/*221*/(END_H),//RSV
/*222*/(END_H),//RSV
/*223*/(END_H),//RSV
/*224*/(END_H),//RSV
/*225*/(END_H),//RSV
/*226*/(END_H),//RSV
/*227*/(END_H),//RSV
/*228*/(END_H),//RSV
/*229*/(END_H),//RSV
/*230*/(END_H),//RSV
/*231*/(END_H),//RSV
/*232*/(END_H),//RSV
/*233*/(END_H),//RSV
/*234*/(END_H),//RSV
/*235*/(END_H),//RSV
/*236*/(END_H),//RSV
/*237*/(END_H),//RSV
/*238*/(END_H),//RSV
/*239*/(END_H),//RSV
/*240*/(END_H),//RSV
/*241*/(END_H),//RSV
/*242*/(END_H),//RSV
/*243*/(END_H),//RSV
/*244*/(END_H),//RSV
/*245*/(END_H),//RSV
/*246*/(END_H),//RSV
/*247*/(END_H),//RSV
/*248*/(END_H),//RSV
/*249*/(END_H),//RSV
/*250*/(END_H),//RSV
/*251*/(END_H),//RSV
/*252*/(END_H),//RSV
/*253*/(END_H),//RSV
/*254*/(END_H),//RSV
/*255*/(END_H) //RSV
    };


//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef union{
    U32 all;
    struct{
        U32  CMD_DATA1: 16;
        U32  CMD_DATA2:  4;
        U32  RSV:       12;
    }b;
} tCMD_MEMORY_ENTRY;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static __inline void Flex_InitFlexDQTable(void);
static __inline void Flex_InitCMDMemory(void);
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
//-------------------------------------------------------------------
// Function     : void NFIF_InitFlexible(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void HalFlexible_OneTimeInit(void)
{
    Flex_InitCMDMemory();
    Flex_InitFlexDQTable();
}
//-------------------------------------------------------------------
// Function     : void NFIF_InitFlexDQTable(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static __inline void Flex_InitFlexDQTable(void)
{
    mNandFlashIf.flexibleModeDqTbl1.b.F_DQ_0  = 0x5C;//RRT & 2-bit Primary
    mNandFlashIf.flexibleModeDqTbl1.b.F_DQ_1  = 0xC5;//RRT & 2-bit Primary
    mNandFlashIf.flexibleModeDqTbl2.b.F_DQ_2  = 0x55;//RRT & 2-bit Primary
    mNandFlashIf.flexibleModeDqTbl2.b.F_DQ_3  = 0x04;//RRT & 2-bit Primary
    mNandFlashIf.flexibleModeDqTbl3.b.F_DQ_4  = 0x08;//RRT & 2-bit Primary
    mNandFlashIf.flexibleModeDqTbl3.b.F_DQ_5  = 0x09;//RRT & 2-bit Primary
    mNandFlashIf.flexibleModeDqTbl4.b.F_DQ_6  = 0x0A;//SLC mode RR (1- & 2-bit) address
    mNandFlashIf.flexibleModeDqTbl4.b.F_DQ_7  = 0x0B;//RSV
    mNandFlashIf.flexibleModeDqTbl5.b.F_DQ_8  = 0x0C;//RSV
    mNandFlashIf.flexibleModeDqTbl5.b.F_DQ_9  = 0x00;//RSV
    mNandFlashIf.flexibleModeDqTbl6.b.F_DQ_10 = 0x40;//RSV
    mNandFlashIf.flexibleModeDqTbl6.b.F_DQ_11 = 0x00;//RSV
    mNandFlashIf.flexibleModeDqTbl7.b.F_DQ_12 = 0x00;//RSV
    mNandFlashIf.flexibleModeDqTbl7.b.F_DQ_13 = 0x00;//RSV
    mNandFlashIf.flexibleModeDqTbl8.b.F_DQ_14 = 0x00;//RSV
    mNandFlashIf.flexibleModeDqTbl8.b.F_DQ_15 = 0x00;//RSV
}

//-------------------------------------------------------------------
// Function     : void NFIF_InitFlexDQTable(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static __inline void Flex_InitCMDMemory(void)
{
    tCMD_MEMORY_ENTRY cmdMemory;
    U16 i;

    mNandFlashIf.flashInterfacCfg.b.PD_EN       = 0;
    mNandFlashIf.cmdMemAccess.b.CM_ACC_EN       = 1;
    mNandFlashIf.cmdMemAccess.b.CM_AUTO_WADDR   = 1;
    mNandFlashIf.cmdMemAddr.b.CM_ADDR           = 0;

    for (i = 0; i < NUMBER_OF_TBL_ELEMENTS(NandCmdEntry); i++)
    {
        cmdMemory.all                           = NandCmdEntry[i];
        mNandFlashIf.cmdMemDataPort2.b.CM_DATA2 = cmdMemory.b.CMD_DATA2;
        mNandFlashIf.CM_DATA1                   = cmdMemory.b.CMD_DATA1;
    }

    mNandFlashIf.cmdMemAccess.b.CM_ACC_EN       = 0;
}

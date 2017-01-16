// ============================================================================
//
//     Copyright (c) LITEON Corporation  -  All rights reserved
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
//! @brief Inter Processor Communication code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "TcgSection.h"    // CODE AND DATA MEMORY MAPPING
#include "TcgCommon.h"

#include "TcgTbl.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
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
//TCG Construct the default tables here ....

/// ********************************************************///
///         Admin Table Init                                                                                ///
/// ********************************************************///
__align(16) tG1 G1= { {

//__align(4) const sTcgTblInfo cTcgTblInfo=
{    TCG_TBL_ID, TCG_TBL_VER    },

/* AdminSP Tables */

//__align(4) const sSPInfo_Tbl cAdmSPInfo_Tbl=
{
    { //ColPty
        {UID_SPInfo, UID_SP_Admin, "Admin", TRUE}
    },
    { //ColPty
        {0x00,8,UID_TYPE},     //UID
        {0x01,8,UID_TYPE},     //SPID
        {0x02,sizeof(G1.b.mAdmSPInfo_Tbl.val[0].name),STRING_TYPE}, //Name, cannot use "sizeof(sSPInfo_TblObj.name)"
        //{0x05,sizeof(cAdmSPInfo_Tbl.val[0].spSessionTimeout),VALUE_TYPE},
        {0x06,sizeof(G1.b.mAdmSPInfo_Tbl.val[0].enabled),VALUE_TYPE}    //Enabled (bool)
    },
    sizeof(G1.b.mAdmSPInfo_Tbl.pty)/sizeof(sColPrty),        //ColCnt
    sizeof(G1.b.mAdmSPInfo_Tbl.val)/sizeof(sSPInfo_TblObj),  //RowCnt
    sizeof(sSPInfo_TblObj),                              //TblObj size
    6
},



//__align(4) const sSPTemplates_Tbl cAdmSPTemplates_Tbl=
{
    {
        {UID_SPTemplate_1, UID_Template_Base,  "Base",  0x00,0x00,0x00,0x02},
        {UID_SPTemplate_2, UID_Template_Admin, "Admin", 0x00,0x00,0x00,0x02}
    },
    { //ColPty
        {0x00,8,UID_TYPE},     //UID
        {0x01,8,UID_TYPE},     //TemplateID
        {0x02,sizeof(G1.b.mAdmSPTemplates_Tbl.val[0].name),STRING_TYPE}, //Name
        {0x03,sizeof(G1.b.mAdmSPTemplates_Tbl.val[0].version),FBYTE_TYPE}     //Enabled
    },
    sizeof(G1.b.mAdmSPTemplates_Tbl.pty)/sizeof(sColPrty),        //ColCnt
    sizeof(G1.b.mAdmSPTemplates_Tbl.val)/sizeof(sSPTemplates_TblObj),  //RowCnt
    sizeof(sSPTemplates_TblObj),                             //TblObj size
    3
},


//__align(4) const sTbl_Tbl cAdmTbl_Tbl=
{
    {
        {UID_Table_Table,       "Table",        TBL_K_OBJECT, 0x00, 0x00},  //R1: Table
        {UID_Table_SPInfo,      "SPInfo",       TBL_K_OBJECT, 0x00, 0x00},  //R2: SPInfo
        {UID_Table_SPTemplates, "SPTemplates",  TBL_K_OBJECT, 0x00, 0x00},  //R3:
        {UID_Table_MethodID,    "MethodID",     TBL_K_OBJECT, 0x00, 0x00},
        {UID_Table_AccessControl,"AccessControl",TBL_K_OBJECT,0x00, 0x00},
        {UID_Table_ACE,         "ACE",          TBL_K_OBJECT, 0x00, 0x00},
        {UID_Table_Authority,   "Authority",    TBL_K_OBJECT, 0x00, 0x00},
        {UID_Table_CPIN,        "C_Pin",        TBL_K_OBJECT, 0x00, 0x00},
        {UID_Table_TPerInfo,    "TPerInf",      TBL_K_OBJECT, 0x00, 0x00},
        {UID_Table_Template,    "Template",     TBL_K_OBJECT, 0x00, 0x00},
        {UID_Table_SP,          "SP",           TBL_K_OBJECT, 0x00, 0x00}
    },
    { //ColPty
        {0x00,8,UID_TYPE},                                      //UID
        {0x01,sizeof(G1.b.mAdmTbl_Tbl.val[0].name),STRING_TYPE},     //Name
        {0x04,sizeof(G1.b.mAdmTbl_Tbl.val[0].kind),VALUE_TYPE},      //Kind (Object or Byte)
        {0x0D,sizeof(G1.b.mAdmTbl_Tbl.val[0].mGranularity),VALUE_TYPE}, //MaxSize
        {0x0E,sizeof(G1.b.mAdmTbl_Tbl.val[0].rGranularity),VALUE_TYPE}  //MaxSize
    },
    sizeof(G1.b.mAdmTbl_Tbl.pty)/sizeof(sColPrty),               //ColCnt
    sizeof(G1.b.mAdmTbl_Tbl.val)/sizeof(sTbl_TblObj),     //RowCnt
    sizeof(sTbl_TblObj),
    0x0e
},


//__align(4) const sMethod_Tbl cAdmMethod_Tbl=
{
    {
        {UID_MethodID_Next,     "Next"},        //R1:
        {UID_MethodID_GetACL,   "GetACL"},
        {UID_MethodID_Get,      "Get"},
        {UID_MethodID_Set,      "Set"},
        {UID_MethodID_Authenticate,"Authenticate"},
        {UID_MethodID_Revert,   "Revert"},
        {UID_MethodID_Activate, "Activate"},
        {UID_MethodID_Random,   "Random"}
    },
    { //ColPty
        {0x00,8,UID_TYPE},      //UID
        {0x01,sizeof(G1.b.mAdmMethod_Tbl.val[0].name),STRING_TYPE}     //Kind (Object or Byte)
    },
    sizeof(G1.b.mAdmMethod_Tbl.pty)/sizeof(sColPrty),        //ColCnt
    sizeof(G1.b.mAdmMethod_Tbl.val)/sizeof(sMethod_TblObj),  //RowCnt
    sizeof(sMethod_TblObj),
    0x03
},

//__align(4) const sAccessCtrl_Tbl cAdmAccessCtrl_Tbl=
{
    {
        //ThisSP: +2 rows
        {UID_ThisSP,    UID_MethodID_Authenticate,  {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_ThisSP,    UID_MethodID_Random,        {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        //Table: 12 rows
        {UID_Table,     UID_MethodID_Next,          {UID_ACE_Anybody,0,0}, UID_ACE_Anybody}, //R1: Table.Next
        {UID_Table_Table, UID_MethodID_Get,         {UID_ACE_Anybody,0,0}, UID_ACE_Anybody}, //R2: TableObj.Get
        {UID_Table_SPInfo, UID_MethodID_Get,        {UID_ACE_Anybody,0,0}, UID_ACE_Anybody}, //R3: TableObj.Get
        {UID_Table_SPTemplates, UID_MethodID_Get,   {UID_ACE_Anybody,0,0}, UID_ACE_Anybody}, //R4: TableObj.Get
        {UID_Table_MethodID, UID_MethodID_Get,      {UID_ACE_Anybody,0,0}, UID_ACE_Anybody}, //R5: TableObj.Get
        {UID_Table_AccessControl, UID_MethodID_Get, {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},  //R6: TableObj.Get
        {UID_Table_ACE, UID_MethodID_Get,           {UID_ACE_Anybody,0,0}, UID_ACE_Anybody}, //R7: TableObj.Get
        {UID_Table_Authority, UID_MethodID_Get,     {UID_ACE_Anybody,0,0}, UID_ACE_Anybody}, //R8: TableObj.Get
        {UID_Table_CPIN, UID_MethodID_Get,          {UID_ACE_Anybody,0,0}, UID_ACE_Anybody}, //R9: TableObj.Get
        {UID_Table_TPerInfo, UID_MethodID_Get,      {UID_ACE_Anybody,0,0}, UID_ACE_Anybody}, //R10: TableObj.Get
        {UID_Table_Template, UID_MethodID_Get,      {UID_ACE_Anybody,0,0}, UID_ACE_Anybody}, //R11: TableObj.Get
        {UID_Table_SP, UID_MethodID_Get,            {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},         //R12: TableObj.Get

        //SPInfo: 1 row
        {UID_SPInfo,    UID_MethodID_Get,           {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},

        //SPTemplates: 3 row
        {UID_SPTemplate,  UID_MethodID_Next,        {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_SPTemplate_1, UID_MethodID_Get,        {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_SPTemplate_2, UID_MethodID_Get,        {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},

        //MethodID: 7 rows + 2 rows
        {UID_MethodID, UID_MethodID_Next,           {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},   //R1: Table.Next
        {UID_MethodID_Next, UID_MethodID_Get,       {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_MethodID_GetACL,UID_MethodID_Get,      {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_MethodID_Get, UID_MethodID_Get,        {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_MethodID_Set, UID_MethodID_Get,        {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_MethodID_Authenticate,UID_MethodID_Get,{UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_MethodID_Revert, UID_MethodID_Get,     {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_MethodID_Activate,UID_MethodID_Get,    {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_MethodID_Random,   UID_MethodID_Get,   {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},

        //ACE: 8 rows + 2 rows
        {UID_ACE, UID_MethodID_Next,                {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_ACE_Anybody, UID_MethodID_Get,         {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_Admin, UID_MethodID_Get,           {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_ACE_Makers_Set_Enabled, UID_MethodID_Get,{UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
    #endif
        {UID_ACE_CPIN_SID_Get_NOPIN, UID_MethodID_Get,{UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_ACE_CPIN_SID_Set_PIN, UID_MethodID_Get,  {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_ACE_CPIN_MSID_Get_PIN, UID_MethodID_Get, {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_CPIN_Admins_Set_PIN,UID_MethodID_Get,{UID_ACE_Anybody,0,0},UID_ACE_Anybody},
    #endif
        {UID_ACE_TPerInfo_Set_PReset,UID_MethodID_Get,{UID_ACE_Anybody,0,0},UID_ACE_Anybody},
        {UID_ACE_SP_SID, UID_MethodID_Get,          {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},

        //Authority: 6 rows + 2 rows
        {UID_Authority, UID_MethodID_Next,          {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_Authority_Anybody,UID_MethodID_Get,    {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_Authority_Admins, UID_MethodID_Get,    {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_Authority_Makers, UID_MethodID_Get,    {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
    #endif
        {UID_Authority_SID,     UID_MethodID_Get,   {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_Authority_Makers, UID_MethodID_Set,    {UID_ACE_Makers_Set_Enabled,0,0}, UID_ACE_Anybody},
    #endif
        {UID_Authority_SID,     UID_MethodID_Set,   {UID_ACE_Makers_Set_Enabled,0,0}, UID_ACE_Anybody},   //for eDrive
    #if _TCG_ != TCG_PYRITE
        {UID_Authority_AdmAdmin1,UID_MethodID_Get,  {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_Authority_AdmAdmin1,UID_MethodID_Set,  {UID_ACE_Makers_Set_Enabled,0,0}, UID_ACE_Anybody},
    #endif
    // PSID [
        {UID_Authority_PSID,    UID_MethodID_Get,   {UID_ACE_Anybody,0,0},            UID_ACE_Anybody},
    // ]

        //CPIN: 4 rows + 2 rows
        {UID_CPIN,              UID_MethodID_Next,  {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},             //R1: CPIN.Next
        {UID_CPIN_SID,          UID_MethodID_Get,   {UID_ACE_CPIN_SID_Get_NOPIN,0,0}, UID_ACE_Anybody},  //R2: CPIN_SID.Get
        {UID_CPIN_SID,          UID_MethodID_Set,   {UID_ACE_CPIN_SID_Set_PIN,0,0}, UID_ACE_Anybody},    //R3: CPIN_SID.Set
        {UID_CPIN_MSID,         UID_MethodID_Get,   {UID_ACE_CPIN_MSID_Get_PIN,0,0}, UID_ACE_Anybody},   //R4: CPIN_MSID.Get
#if _TCG_ != TCG_PYRITE
        {UID_CPIN_AdmAdmin1,    UID_MethodID_Get,   {UID_ACE_CPIN_SID_Get_NOPIN,0,0}, UID_ACE_Anybody},
        {UID_CPIN_AdmAdmin1,    UID_MethodID_Set,   {UID_ACE_CPIN_Admins_Set_PIN,0,0},UID_ACE_Anybody},
#endif
    // PSID [
        //{UID_CPIN_PSID,         UID_MethodID_Get,   {UID_ACE_CPIN_SID_Get_NOPIN,0}, UID_ACE_Anybody},
        {UID_CPIN_PSID,         UID_MethodID_Get,   {UID_ACE_CPIN_Get_PSID_NoPIN,0,0}, UID_ACE_Anybody},
        {UID_CPIN_PSID,         UID_MethodID_Set,   {UID_ACE_CPIN_SID_Set_PIN,0,0},   UID_ACE_Anybody},
    // ]

        //TPerInfo: 1 row + 1 row
        {UID_TPerInfo,      UID_MethodID_Get,       {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_TPerInfo,      UID_MethodID_Set,       {UID_ACE_TPerInfo_Set_PReset,0,0}, UID_ACE_Anybody},

        //Template: 4 rows
        {UID_Template,      UID_MethodID_Next,      {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_Template_Base, UID_MethodID_Get,       {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_Template_Admin, UID_MethodID_Get,      {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_Template_Locking,UID_MethodID_Get,     {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},

        //SP: 7 rows
        {UID_SP,            UID_MethodID_Next,      {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_SP_Admin,      UID_MethodID_Get,       {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_SP_Locking,    UID_MethodID_Get,       {UID_ACE_Anybody,0,0}, UID_ACE_Anybody},
        {UID_SP_Admin,      UID_MethodID_Revert,    {UID_ACE_SP_SID,UID_ACE_Admin,UID_ACE_SP_PSID},  UID_ACE_Anybody},
        {UID_SP_Locking,    UID_MethodID_Revert,    {UID_ACE_SP_SID,UID_ACE_Admin,0},  UID_ACE_Anybody},
        {UID_SP_Admin,      UID_MethodID_Activate,  {UID_ACE_SP_SID,0,0},  UID_ACE_Anybody},
        {UID_SP_Locking,    UID_MethodID_Activate,  {UID_ACE_SP_SID,0,0}, UID_ACE_Anybody}
    },
    { //ColPty
        {0x01,8,UID_TYPE},      //InvokingID
        {0x02,8,UID_TYPE},      //MethodID
        {0x04,sizeof(G1.b.mAdmAccessCtrl_Tbl.val[0].acl),UIDLIST_TYPE},     //ACL
        {0x08,8,UID_TYPE}      //GetACLACL
    },
    sizeof(G1.b.mAdmAccessCtrl_Tbl.pty)/sizeof(sColPrty),            //ColCnt
    sizeof(G1.b.mAdmAccessCtrl_Tbl.val)/sizeof(sAccessCtrl_TblObj),  //RowCnt
    sizeof(sAccessCtrl_TblObj),
    0x0e
},

//__align(4) const sACE_Tbl cAdmACE_Tbl=
{
    {
        {UID_ACE_Anybody,           {UID_Authority_Anybody,0},   {0,0,0,0,0,0,0,0,0,0,0,0}},   //ACE_Anybody, col(All)
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_Admin,             {UID_Authority_Admins,0},    {0,0,0,0,0,0,0,0,0,0,0,0}},   //ACE_Admin, col(All)
        {UID_ACE_Makers_Set_Enabled,{UID_Authority_SID,0},       {1,5,0,0,0,0,0,0,0,0,0,0}},   //ACE_Makers..., b5->col5 (col(Enabled))
    #endif
        {UID_ACE_CPIN_SID_Get_NOPIN,{UID_Authority_Admins,UID_Authority_SID}, {5,0,4,5,6,7,0,0,0,0,0,0}},   //ACE_CPIN_SID_Get..., col(UID, CharSet, TryLimit, Tries, Persistence)
        {UID_ACE_CPIN_SID_Set_PIN,  {UID_Authority_SID,0},       {1,3,0,0,0,0,0,0,0,0,0,0}},   //ACE_CPIN_SID_Set..., col(PIN)
        {UID_ACE_CPIN_MSID_Get_PIN, {UID_Authority_Anybody,0},   {2,0,3,0,0,0,0,0,0,0,0,0}},   //ACE_CPIN_MSID_Get..., col(UID,PIN)
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_CPIN_Admins_Set_PIN,{UID_Authority_Admins,UID_Authority_SID},{1,3,0,0,0,0,0,0,0,0,0,0}},
    #endif
        {UID_ACE_TPerInfo_Set_PReset,{UID_Authority_SID,0},      {1,8,0,0,0,0,0,0,0,0,0,0}},
    // PSID [
        {UID_ACE_CPIN_Get_PSID_NoPIN,{UID_Authority_Anybody,0},  {5,0,4,5,6,7,0,0,0,0,0,0}},
        {UID_ACE_SP_SID,            {UID_Authority_SID,0}, {0,0,0,0,0,0,0,0,0,0,0,0}},  //ACE_SP_SID, col(All)
        {UID_ACE_SP_PSID,           {UID_Authority_PSID,0}, {0,0,0,0,0,0,0,0,0,0,0,0}},
    // ]
    },
    { //ColPty
        {0x00,8,UID_TYPE},      //UID
        {0x03,sizeof(G1.b.mAdmACE_Tbl.val[0].booleanExpr),UIDLIST_TYPE}, //BooleanExpr
        {0x04,sizeof(G1.b.mAdmACE_Tbl.val[0].col),LIST_TYPE}             //Columns
    },
    sizeof(G1.b.mAdmACE_Tbl.pty)/sizeof(sColPrty),       //ColCnt
    sizeof(G1.b.mAdmACE_Tbl.val)/sizeof(sACE_TblObj),    //RowCnt
    sizeof(sACE_TblObj),
    0x04
},

//__align(4) const sAuthority_Tbl cAdmAuthority_Tbl=
{
    {
        {UID_Authority_Anybody, "Anybody",FALSE,UID_Null, TRUE, SECURE_None, HASH_None, FALSE, AUTH_None,    UID_Null, UID_Null, UID_Null},
        {UID_Authority_Admins,  "Admins", TRUE, UID_Null, TRUE, SECURE_None, HASH_None, FALSE, AUTH_None,    UID_Null, UID_Null, UID_Null},
     #if _TCG_ != TCG_PYRITE
        {UID_Authority_Makers,  "Makers", TRUE, UID_Null, TRUE, SECURE_None, HASH_None, FALSE, AUTH_None,    UID_Null, UID_Null, UID_Null},
     #endif
        {UID_Authority_SID,     "SID",    FALSE,UID_Null, TRUE, SECURE_None, HASH_None, FALSE, AUTH_Password,UID_CPIN_SID, UID_Null, UID_Null},
     #if _TCG_ != TCG_PYRITE
        {UID_Authority_AdmAdmin1,"Admin1",FALSE,UID_Authority_Admins,FALSE,SECURE_None,HASH_None,FALSE,AUTH_Password,UID_CPIN_AdmAdmin1,UID_Null,UID_Null},
     #endif
    // PSID [
        {UID_Authority_PSID,    "PSID",   FALSE,UID_Null, TRUE, SECURE_None, HASH_None, FALSE, AUTH_Password,UID_CPIN_PSID, UID_Null, UID_Null}
    // ]
    },
    { //ColPty
        {0x00,8,UID_TYPE},      //UID
        {0x01,sizeof(G1.b.mAdmAuthority_Tbl.val[0].name),STRING_TYPE},
        {0x03,sizeof(G1.b.mAdmAuthority_Tbl.val[0].isClass),VALUE_TYPE},    //IsClass
        {0x04,8,UID_TYPE},      //Class
        {0x05,sizeof(G1.b.mAdmAuthority_Tbl.val[0].enabled),VALUE_TYPE},    //Enabled
        {0x06,sizeof(secure_message),VALUE_TYPE},    //Secure
        {0x07,sizeof(hash_protocol),VALUE_TYPE},    //HashAndSign
        {0x08,sizeof(G1.b.mAdmAuthority_Tbl.val[0].presentCertificate),VALUE_TYPE},    //PresentCertificate  (bool)
        {0x09,sizeof(auth_method),VALUE_TYPE},    //Operation
        {0x0A,8,UID_TYPE},      //Credential (UID)
        {0x0B,8,UID_TYPE},      //ResponseSign
        {0x0C,8,UID_TYPE}       //ResponseExch
    },
    sizeof(G1.b.mAdmAuthority_Tbl.pty)/sizeof(sColPrty),             //ColCnt
    sizeof(G1.b.mAdmAuthority_Tbl.val)/sizeof(sAuthority_TblObj),    //RowCnt
    sizeof(sAuthority_TblObj),
    0x12
},

//__align(4) const sCPin_Tbl cAdmCPin_Tbl=
{
    {
        {   UID_CPIN_SID, "C_PIN_SID",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 7, 0, FALSE, 0, 0, 0},
        {   UID_CPIN_MSID, "C_PIN_MSID",
            {16,'m','y','_','M','S','I','D','_','p','a','s','s','w','o','r','d', \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 0, 0, FALSE, 0, 0, 0},
        //{   UID_CPIN_MSID, "C_PIN_MSID",
        //    {15,'<','M','S','I','D','_','p','a','s','s','w','o','r','d','>', \
        //      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
        //    UID_Null, 0, 0, FALSE, 0, 0, 0}
    #if _TCG_ != TCG_PYRITE
        {   UID_CPIN_AdmAdmin1, "C_PIN_Admin1",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 0, 0, FALSE, 0, 0, 0},
    #endif
        {   UID_CPIN_PSID, "C_PIN_PSID",
            // "my_MSID_password":
            {16, 0xD6,0xA5,0x11,0x35,0x7A,0xED,0x9B,0x86,0x8E,0xFA,0x13,0x2A,0xE7,0x35,0xF6,0x7F,0x28,0xED,0xC4,0x2B,
                 0xC6,0xE8,0x97,0x53,0xAC,0x50,0x49,0x2B,0x2B,0x8F,0xE0,0xE6,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0}
            // "<MSID_password>":
            //{15, 0x8F,0x29,0xD2,0x5B,0x6F,0xD1,0x84,0xDE,0x7E,0x05,0x06,0x07,0xCC,0x40,0xC4,0x4A,0x3D,0x30,0xC8,0xDC,
            //     0xE1,0x32,0xB2,0x77,0x3B,0xFD,0xA5,0x9F,0xE0,0x8C,0x01,0x8B,   0,   0,   0},
            // UID_Null, 5, 0, FALSE, 0, 0, 0}
    },
    { //ColPty
        {0x00,sizeof(G1.b.mAdmCPin_Tbl.val[0].uid),UID_TYPE},       //UID
        {0x01,sizeof(G1.b.mAdmCPin_Tbl.val[0].name),STRING_TYPE},
        {0x03,sizeof(G1.b.mAdmCPin_Tbl.val[0].pwd),VBYTE_TYPE},  //PIN
        {0x04,sizeof(G1.b.mAdmCPin_Tbl.val[0].charSet),UID_TYPE},      //CharSet
        {0x05,sizeof(G1.b.mAdmCPin_Tbl.val[0].tryLimit),VALUE_TYPE},    //TryLimit
        {0x06,sizeof(G1.b.mAdmCPin_Tbl.val[0].tries),VALUE_TYPE},    //Tries
        {0x07,sizeof(G1.b.mAdmCPin_Tbl.val[0].presistence),VALUE_TYPE}     //Persistence
    },
    sizeof(G1.b.mAdmCPin_Tbl.pty)/sizeof(sColPrty),      //ColCnt
    sizeof(G1.b.mAdmCPin_Tbl.val)/sizeof(sCPin_TblObj),   //RowCnt
    sizeof(sCPin_TblObj),
    0x07
},

//__align(4) const sTPerInfo_Tbl cAdmTPerInfo_Tbl=
{
    {
        {UID_TPerInfo, TPER_FW_VERSION, 0x01, SSC_STRING, FALSE}   //double '\0' for String List ends.
    },
    { //ColPty
        {0x00,8,UID_TYPE},     //UID
        {0x04,sizeof(G1.b.mAdmTPerInfo_Tbl.val[0].firmwareVersion),VALUE_TYPE},   //UID
        {0x05,sizeof(G1.b.mAdmTPerInfo_Tbl.val[0].protocolVersion),VALUE_TYPE},   //ProtocolVersion
        {0x07,sizeof(G1.b.mAdmTPerInfo_Tbl.val[0].ssc),       STRINGLIST_TYPE},   //SSC
        {0x08,sizeof(G1.b.mAdmTPerInfo_Tbl.val[0].preset),         VALUE_TYPE},
    },
    sizeof(G1.b.mAdmTPerInfo_Tbl.pty)/sizeof(sColPrty),        //ColCnt
    sizeof(G1.b.mAdmTPerInfo_Tbl.val)/sizeof(sTPerInfo_TblObj),  //RowCnt
    sizeof(sTPerInfo_TblObj),                              //TblObj size
    0x08
},

//__align(4) const sTemplate_Tbl cAdmTemplate_Tbl=
{
    {
        {UID_Template_Base,   "Base", 1, 2, 2},
        {UID_Template_Admin,  "Admin", 1, 1, 1},
        {UID_Template_Locking,"Locking", 1, 1, 1}
    },
    { //ColPty
        {0x00,8,UID_TYPE},      //UID
        {0x01,sizeof(G1.b.mAdmTemplate_Tbl.val[0].name),STRING_TYPE},    //Name
        {0x02,sizeof(G1.b.mAdmTemplate_Tbl.val[0].revision),VALUE_TYPE},    //Revision Number
        {0x03,sizeof(G1.b.mAdmTemplate_Tbl.val[0].instances),VALUE_TYPE},    //Instances
        {0x04,sizeof(G1.b.mAdmTemplate_Tbl.val[0].maxInstances),VALUE_TYPE}     //Max Instances
    },
    sizeof(G1.b.mAdmTemplate_Tbl.pty)/sizeof(sColPrty),      //ColCnt
    sizeof(G1.b.mAdmTemplate_Tbl.val)/sizeof(sTemplate_TblObj),   //RowCnt
    sizeof(sTemplate_TblObj),
    0x04
},


//__align(4) const sSP_Tbl cAdmSP_Tbl=
{
    {
        {UID_SP_Admin,  "Admin",  manufactured,         FALSE},
        {UID_SP_Locking,"Locking",manufactured_inactive,FALSE}
    },
    { //ColPty
        {0x00,8,UID_TYPE},      //UID
        {0x01,sizeof(G1.b.mAdmSP_Tbl.val[0].name),STRING_TYPE},
        {0x06,sizeof(life_cycle_state),VALUE_TYPE},    //LifeCycle
        {0x07,sizeof(G1.b.mAdmSP_Tbl.val[0].frozen),VALUE_TYPE}     //Frozen (bool)
    },
    sizeof(G1.b.mAdmSP_Tbl.pty)/sizeof(sColPrty),      //ColCnt
    sizeof(G1.b.mAdmSP_Tbl.val)/sizeof(sSP_TblObj),   //RowCnt
    sizeof(sSP_TblObj),
    0x07
}

}
};

/// ********************************************************///
///         Locking Table Init                                                                               ///
/// ********************************************************///
/* LockingSP Tables */
__align(16) tG2 G2={ {
//__align(4) const sSPInfo_Tbl cLckSPInfo_Tbl=
{
    {
        {UID_SPInfo, UID_SP_Locking, "Locking", TRUE}
    },
    { //ColPty
        {0x00,8,UID_TYPE},     //UID
        {0x01,8,UID_TYPE},     //SPID
        {0x02,sizeof(G2.b.mLckSPInfo_Tbl.val[0].name),STRING_TYPE}, //Name, cannot use "sizeof(sSPInfo_TblObj.name)"
        {0x06,sizeof(G2.b.mLckSPInfo_Tbl.val[0].enabled),VALUE_TYPE}    //Enabled
    },
    sizeof(G2.b.mLckSPInfo_Tbl.pty)/sizeof(sColPrty),        //ColCnt
    sizeof(G2.b.mLckSPInfo_Tbl.val)/sizeof(sSPInfo_TblObj),  //RowCnt
    sizeof(sSPInfo_TblObj),                             //TblObj size
    0x06
},

//__align(4) const sSPTemplates_Tbl cLckSPTemplates_Tbl=
{
    {
        {UID_SPTemplate_1, UID_Template_Base,   "Base",   0x00,0x00,0x00,0x02},
        {UID_SPTemplate_2, UID_Template_Locking,"Locking",0x00,0x00,0x00,0x02}
    },
    { //ColPty
        {0x00,8,UID_TYPE},     //UID
        {0x01,8,UID_TYPE},     //TemplateID
        {0x02,sizeof(G2.b.mLckSPTemplates_Tbl.val[0].name),STRING_TYPE},     //Name
        {0x03,sizeof(G2.b.mLckSPTemplates_Tbl.val[0].version),FBYTE_TYPE}    //Version
    },
    sizeof(G2.b.mLckSPTemplates_Tbl.pty)/sizeof(sColPrty),        //ColCnt
    sizeof(G2.b.mLckSPTemplates_Tbl.val)/sizeof(sSPTemplates_TblObj),  //RowCnt
    sizeof(sSPTemplates_TblObj),                              //TblObj size
    0x03
},


//__align(4) const sLckTbl_Tbl cLckTbl_Tbl=
{
    {
    #if _TCG_ != TCG_PYRITE
        {UID_Table_Table,       "Table",        TBL_K_OBJECT, 15+DSTBL_MAX_NUM-1,     0x00,   0x00},  //R1: Table
        {UID_Table_SPInfo,      "SPInfo",       TBL_K_OBJECT, 1,                      0x00,   0x00},  //R2:
        {UID_Table_SPTemplates, "SPTemplates",  TBL_K_OBJECT, 2,                      0x00,   0x00},  //R3:
        {UID_Table_MethodID,    "MethodID",     TBL_K_OBJECT, LCK_METHOD_TBLOBJ_CNT,  0x00,   0x00},
        {UID_Table_AccessControl,"AccessControl",TBL_K_OBJECT,LCK_ACCESSCTRL_TBLOBJ_CNT,0x00,0x00},
        {UID_Table_ACE,         "ACE",          TBL_K_OBJECT, LCK_ACE_TBLOBJ_CNT,     0x00,   0x00},
        {UID_Table_Authority,   "Authority",    TBL_K_OBJECT, LCK_AUTHORITY_TBLOBJ_CNT,0x00,   0x00},
        {UID_Table_CPIN,        "C_Pin",        TBL_K_OBJECT, LCK_CPIN_TBLOBJ_CNT,    0x00,   0x00},  //row
        {UID_Table_SecretProtect,"SecretProtect",TBL_K_OBJECT,1,                      0x00,   0x00},
        {UID_Table_LockingInfo, "LockingInfo",  TBL_K_OBJECT, 1,                      0x00,   0x00},
        {UID_Table_Locking,     "Locking",      TBL_K_OBJECT, LOCKING_RANGE_CNT+1,    0x00,   0x00},  //row
        {UID_Table_MBRControl,  "MBRControl",   TBL_K_OBJECT, 1,                      0x00,   0x00},
        {UID_Table_MBR,         "MBR",          TBL_K_BYTE,   MBR_LEN,                0x01,   0x01},  //VU/VU
        {UID_Table_K_AES_256,   "K_AES_256",    TBL_K_OBJECT, LOCKING_RANGE_CNT+1,    0x00,   0x00},  //row
        {UID_Table_DataStore,   "DataStore",    TBL_K_BYTE,   DATASTORE_LEN,          0x01,   0x01},   //VU/VU
        {UID_Table_DataStore2|UID_FF,  "DataStore2",   TBL_K_BYTE,   0x00,            0x01,   0x01},  //VU/VU
        {UID_Table_DataStore3|UID_FF,  "DataStore3",   TBL_K_BYTE,   0x00,            0x01,   0x01},  //VU/VU
        {UID_Table_DataStore4|UID_FF,  "DataStore4",   TBL_K_BYTE,   0x00,            0x01,   0x01},  //VU/VU
        {UID_Table_DataStore5|UID_FF,  "DataStore5",   TBL_K_BYTE,   0x00,            0x01,   0x01},  //VU/VU
        {UID_Table_DataStore6|UID_FF,  "DataStore6",   TBL_K_BYTE,   0x00,            0x01,   0x01},  //VU/VU
        {UID_Table_DataStore7|UID_FF,  "DataStore7",   TBL_K_BYTE,   0x00,            0x01,   0x01},  //VU/VU
        {UID_Table_DataStore8|UID_FF,  "DataStore8",   TBL_K_BYTE,   0x00,            0x01,   0x01},  //VU/VU
        {UID_Table_DataStore9|UID_FF,  "DataStore9",   TBL_K_BYTE,   0x00,            0x01,   0x01}   //VU/VU
    #else
        {UID_Table_Table,       "Table",        TBL_K_OBJECT, 13,                     0x00,   0x00},  //R1: Table
        {UID_Table_SPInfo,      "SPInfo",       TBL_K_OBJECT, 1,                      0x00,   0x00},  //R2:
        {UID_Table_SPTemplates, "SPTemplates",  TBL_K_OBJECT, 2,                      0x00,   0x00},  //R3:
        {UID_Table_MethodID,    "MethodID",     TBL_K_OBJECT, LCK_METHOD_TBLOBJ_CNT,  0x00,   0x00},
        {UID_Table_AccessControl,"AccessControl",TBL_K_OBJECT,LCK_ACCESSCTRL_TBLOBJ_CNT,0x00, 0x00},
        {UID_Table_ACE,         "ACE",          TBL_K_OBJECT, LCK_ACE_TBLOBJ_CNT,     0x00,   0x00},
        {UID_Table_Authority,   "Authority",    TBL_K_OBJECT, LCK_AUTHORITY_TBLOBJ_CNT,0x00,  0x00},
        {UID_Table_CPIN,        "C_Pin",        TBL_K_OBJECT, LCK_CPIN_TBLOBJ_CNT,    0x00,   0x00},  //row
        //{UID_Table_SecretProtect,"SecretProtect",TBL_K_OBJECT,1,                      0x00,   0x00},
        {UID_Table_LockingInfo, "LockingInfo",  TBL_K_OBJECT, 1,                      0x00,   0x00},
        {UID_Table_Locking,     "Locking",      TBL_K_OBJECT, LOCKING_RANGE_CNT+1,    0x00,   0x00},  //row
        {UID_Table_MBRControl,  "MBRControl",   TBL_K_OBJECT, 1,                      0x00,   0x00},
        {UID_Table_MBR,         "MBR",          TBL_K_BYTE,   MBR_LEN,                0x01,   0x01},  //VU/VU
        //{UID_Table_K_AES_256,   "K_AES_256",    TBL_K_OBJECT, LOCKING_RANGE_CNT+1,    0x00,   0x00},  //row
        {UID_Table_DataStore,   "DataStore",    TBL_K_BYTE,   DATASTORE_LEN,          0x01,   0x01},   //VU/VU
    #endif
    },
    { //ColPty
        {0x00,8,UID_TYPE},                                              //UID
        {0x01,sizeof(G2.b.mLckTbl_Tbl.val[0].name),STRING_TYPE},             //Name
        {0x04,sizeof(G2.b.mLckTbl_Tbl.val[0].kind),VALUE_TYPE},                           //Kind (Object or Byte)
        {0x07,sizeof(G2.b.mLckTbl_Tbl.val[0].rows),VALUE_TYPE},              //Rows
        {0x0D,sizeof(G2.b.mLckTbl_Tbl.val[0].mGranularity),VALUE_TYPE},
        {0x0E,sizeof(G2.b.mLckTbl_Tbl.val[0].rGranularity),VALUE_TYPE}       //MaxSize
    },
    sizeof(G2.b.mLckTbl_Tbl.pty)/sizeof(sColPrty),                           //ColCnt
    sizeof(G2.b.mLckTbl_Tbl.val)/sizeof(sLckTbl_TblObj),                     //RowCnt
    sizeof(sLckTbl_TblObj),
    0x0E
},



//__align(4) const sLckMethod_Tbl cLckMethod_Tbl=
{
    {
        {UID_MethodID_Next,     "Next"},        //R1:
        {UID_MethodID_GetACL,   "GetACL"},
    #if _TCG_ != TCG_PYRITE
        {UID_MethodID_GenKey,   "GenKey"},
    #endif
        {UID_MethodID_RevertSP, "RevertSP"},
        {UID_MethodID_Get,      "Get"},
        {UID_MethodID_Set,      "Set"},
        {UID_MethodID_Authenticate, "Authenticate"},
        {UID_MethodID_Random,       "Random"},
    #if _TCG_ != TCG_PYRITE
        {UID_MethodID_Reactivate,   "Reactivate"},
        {UID_MethodID_Erase,        "Erase"}
    #endif
    },
    { //ColPty
        {0x00,8,UID_TYPE},      //UID
        {0x01,sizeof(G2.b.mLckMethod_Tbl.val[0].name),STRING_TYPE}     //Kind (Object or Byte)
    },
    sizeof(G2.b.mLckMethod_Tbl.pty)/sizeof(sColPrty),        //ColCnt
    sizeof(G2.b.mLckMethod_Tbl.val)/sizeof(sMethod_TblObj),  //RowCnt
    sizeof(sMethod_TblObj),
    0x03
},

//__align(4) const sLckAccessCtrl_Tbl cLckAccessCtrl_Tbl=
{
    {
        //SP: 1 row + 2 rows
        {UID_ThisSP,    UID_MethodID_RevertSP,      {UID_ACE_Admin,  0,0,0},UID_ACE_Anybody},
        {UID_ThisSP,    UID_MethodID_Authenticate,  {UID_ACE_Anybody,0,0,0},UID_ACE_Anybody},
        {UID_ThisSP,    UID_MethodID_Random,        {UID_ACE_Anybody,0,0,0},UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_ThisSP,    UID_MethodID_Reactivate,    {UID_ACE_SP_Reactivate_Admin,0,0,0},UID_ACE_Anybody},
    #endif
        //Table: 15 rows + 1 row
        {UID_Table,             UID_MethodID_Next,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},  //R1: Table.Next
        {UID_Table_Table,       UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},  //R2: TableObj.Get
        {UID_Table_SPInfo,      UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},  //R3: TableObj.Get
        {UID_Table_SPTemplates, UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_MethodID,    UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_AccessControl,UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_ACE,         UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_Authority,   UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_CPIN,        UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_Table_SecretProtect,UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
    #endif
        {UID_Table_LockingInfo, UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_Locking,     UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_MBRControl,  UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_MBR,         UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_Table_K_AES_256,   UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
    #endif
        {UID_Table_DataStore,   UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_Table_DataStore2,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_DataStore3,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_DataStore4,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_DataStore5,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_DataStore6,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_DataStore7,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_DataStore8,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_Table_DataStore9,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
    #endif
        //SPInfo: 1 row
        {UID_SPInfo,            UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},

        //SPTemplates: 3 row
        {UID_SPTemplate,        UID_MethodID_Next,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_SPTemplate_1,      UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_SPTemplate_2,      UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},

        //MethodID: 7 rows + 2 rows
        {UID_MethodID,          UID_MethodID_Next,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},   //R1: Method.Next
        {UID_MethodID_Next,     UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_MethodID_GetACL,   UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_MethodID_GenKey,   UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
    #endif
        {UID_MethodID_RevertSP, UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_MethodID_Get,      UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_MethodID_Set,      UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_MethodID_Authenticate,UID_MethodID_Get,{UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_MethodID_Random,   UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_MethodID_Reactivate,UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_MethodID_Erase,    UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
    #endif
        //ACE: 65 rows + 58 rows
        {UID_ACE,               UID_MethodID_Next,          {UID_ACE_Anybody,    0,0,0},    UID_ACE_Anybody},
        {UID_ACE_Anybody,       UID_MethodID_Get,           {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_Admin,         UID_MethodID_Get,           {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_Anybody_Get_CommonName,UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_Admins_Set_CommonName, UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
    #endif
        {UID_ACE_ACE_Get_All, UID_MethodID_Get,             {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_ACE_Get_All, UID_MethodID_Set,             {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_ACE_Set_BooleanExpression,UID_MethodID_Get,{UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_Authority_Get_All, UID_MethodID_Get,       {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_Authority_Get_All, UID_MethodID_Set,       {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Authority_Set_Enabled,UID_MethodID_Get,    {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_User1_Set_CommonName,  UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User2_Set_CommonName,  UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User3_Set_CommonName,  UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User4_Set_CommonName,  UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User5_Set_CommonName,  UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User6_Set_CommonName,  UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User7_Set_CommonName,  UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User8_Set_CommonName,  UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User9_Set_CommonName,  UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User1_Set_CommonName,  UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User2_Set_CommonName,  UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User3_Set_CommonName,  UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User4_Set_CommonName,  UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User5_Set_CommonName,  UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User6_Set_CommonName,  UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User7_Set_CommonName,  UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User8_Set_CommonName,  UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_User9_Set_CommonName,  UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0},    UID_ACE_Anybody},
    #endif
        {UID_ACE_C_PIN_Admins_Get_All_NOPIN,UID_MethodID_Get,{UID_ACE_ACE_Get_All,0,0,0},   UID_ACE_Anybody},
        {UID_ACE_C_PIN_Admins_Set_PIN,  UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_C_PIN_User1_Set_PIN,   UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_C_PIN_User2_Set_PIN,   UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_C_PIN_User3_Set_PIN,   UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_C_PIN_User4_Set_PIN,   UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_C_PIN_User5_Set_PIN,   UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_C_PIN_User6_Set_PIN,   UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_C_PIN_User7_Set_PIN,   UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_C_PIN_User8_Set_PIN,   UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_C_PIN_User9_Set_PIN,   UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
    #endif
        {UID_ACE_C_PIN_User1_Set_PIN,   UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_C_PIN_User2_Set_PIN,   UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_C_PIN_User3_Set_PIN,   UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_C_PIN_User4_Set_PIN,   UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},      //to User8 for Opal2
        {UID_ACE_C_PIN_User5_Set_PIN,   UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},      //to User8 for Opal2
        {UID_ACE_C_PIN_User6_Set_PIN,   UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},      //to User8 for Opal2
        {UID_ACE_C_PIN_User7_Set_PIN,   UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},      //to User8 for Opal2
        {UID_ACE_C_PIN_User8_Set_PIN,   UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},      //to User8 for Opal2
        {UID_ACE_C_PIN_User9_Set_PIN,   UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},      //to User8 for Opal2

        {UID_ACE_K_AES_256_GlobalRange_GenKey,UID_MethodID_Get,{UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_256_GlobalRange_GenKey,UID_MethodID_Set,{UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range1_GenKey, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range1_GenKey, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range2_GenKey, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range2_GenKey, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range3_GenKey, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range3_GenKey, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range4_GenKey, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range4_GenKey, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range5_GenKey, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range5_GenKey, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range6_GenKey, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range6_GenKey, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range7_GenKey, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range7_GenKey, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_256_Range8_GenKey, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0},    UID_ACE_Anybody},  //to Range8 for opal2
        {UID_ACE_K_AES_256_Range8_GenKey, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_K_AES_Mode,            UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
    #endif
        {UID_ACE_Locking_GRange_Get_RangeStartToActiveKey,UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_GRange_Get_RangeStartToActiveKey,UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_Locking_Range1_Get_RangeStartToActiveKey,UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range1_Get_RangeStartToActiveKey,UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range2_Get_RangeStartToActiveKey,UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range2_Get_RangeStartToActiveKey,UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range3_Get_RangeStartToActiveKey,UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range3_Get_RangeStartToActiveKey,UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range4_Get_RangeStartToActiveKey,UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range4_Get_RangeStartToActiveKey,UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range5_Get_RangeStartToActiveKey,UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range5_Get_RangeStartToActiveKey,UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range6_Get_RangeStartToActiveKey,UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range6_Get_RangeStartToActiveKey,UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range7_Get_RangeStartToActiveKey,UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range7_Get_RangeStartToActiveKey,UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range8_Get_RangeStartToActiveKey,UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range8_Get_RangeStartToActiveKey,UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
    #endif
        {UID_ACE_Locking_GRange_Set_RdLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_GRange_Set_RdLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_Locking_Range1_Set_RdLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range1_Set_RdLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range2_Set_RdLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range2_Set_RdLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range3_Set_RdLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range3_Set_RdLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range4_Set_RdLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range4_Set_RdLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range5_Set_RdLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range5_Set_RdLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range6_Set_RdLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range6_Set_RdLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range7_Set_RdLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range7_Set_RdLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range8_Set_RdLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range8_Set_RdLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
    #endif
        {UID_ACE_Locking_GRange_Set_WrLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_GRange_Set_WrLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_Locking_Range1_Set_WrLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range1_Set_WrLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range2_Set_WrLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range2_Set_WrLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range3_Set_WrLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range3_Set_WrLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range4_Set_WrLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range4_Set_WrLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range5_Set_WrLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range5_Set_WrLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range6_Set_WrLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range6_Set_WrLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range7_Set_WrLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range7_Set_WrLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range8_Set_WrLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range8_Set_WrLocked, UID_MethodID_Set, {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
    #endif
        {UID_ACE_Locking_GlblRng_Admins_Set, UID_MethodID_Get,  {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_Locking_Admins_RangeStartToLocked, UID_MethodID_Get, {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
    #endif
        {UID_ACE_MBRControl_Admins_Set, UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_MBRControl_Set_Done,   UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_MBRControl_Set_Done,   UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore_Get_All,     UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore_Get_All,     UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore_Set_All,     UID_MethodID_Get,   {UID_ACE_ACE_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore_Set_All,     UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE // *** 32 rows
        {UID_ACE_DataStore2_Get_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore2_Get_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore2_Set_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore2_Set_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore3_Get_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore3_Get_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore3_Set_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore3_Set_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore4_Get_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore4_Get_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore4_Set_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore4_Set_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore5_Get_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore5_Get_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore5_Set_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore5_Set_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore6_Get_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore6_Get_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore6_Set_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore6_Set_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore7_Get_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore7_Get_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore7_Set_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore7_Set_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore8_Get_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore8_Get_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore8_Set_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore8_Set_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore9_Get_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore9_Get_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore9_Set_All,    ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_DataStore9_Set_All,    ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_SP_Reactivate_Admin,   UID_MethodID_Get,   {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_SP_Reactivate_Admin,   UID_MethodID_Set,   {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},

        {UID_ACE_Locking_GRange_Set_ReadToLOR,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range1_Set_ReadToLOR,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range2_Set_ReadToLOR,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range3_Set_ReadToLOR,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range4_Set_ReadToLOR,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range5_Set_ReadToLOR,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range6_Set_ReadToLOR,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range7_Set_ReadToLOR,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range8_Set_ReadToLOR,  ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range1_Set_Range,      ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range2_Set_Range,      ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range3_Set_Range,      ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range4_Set_Range,      ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range5_Set_Range,      ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range6_Set_Range,      ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range7_Set_Range,      ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range8_Set_Range,      ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},

        {UID_ACE_CPIN_Anybody_Get_NoPIN,        ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_GRange_Erase,          ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_GRange_Erase,          ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range1_Erase,          ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range1_Erase,          ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range2_Erase,          ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range2_Erase,          ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range3_Erase,          ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range3_Erase,          ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range4_Erase,          ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range4_Erase,          ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range5_Erase,          ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range5_Erase,          ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range6_Erase,          ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range6_Erase,          ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range7_Erase,          ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range7_Erase,          ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range8_Erase,          ~UID_MethodID_Get,  {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_ACE_Locking_Range8_Erase,          ~UID_MethodID_Set,  {UID_ACE_ACE_Set_BooleanExpression,0,0,0}, UID_ACE_Anybody},
    #endif
        //Authority: 28 rows
        {UID_Authority,         UID_MethodID_Next,      {UID_ACE_Anybody,0,0,0},               UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_Authority_Anybody, UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_Admins,  UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_Admin1,  UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_Admin2,  UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_Admin3,  UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_Admin4,  UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_Users,   UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_User1,   UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_User2,   UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_User3,   UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_User4,   UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_User5,   UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_User6,   UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_User7,   UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_User8,   UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_User9,   UID_MethodID_Get,       {UID_ACE_Authority_Get_All,UID_ACE_Anybody_Get_CommonName,0,0},     UID_ACE_Anybody},
        {UID_Authority_Admin1,  UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,UID_ACE_Admins_Set_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Authority_Admin2,  UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,UID_ACE_Admins_Set_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Authority_Admin3,  UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,UID_ACE_Admins_Set_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Authority_Admin4,  UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,UID_ACE_Admins_Set_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Authority_User1,   UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,UID_ACE_User1_Set_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Authority_User2,   UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,UID_ACE_User2_Set_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Authority_User3,   UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,UID_ACE_User3_Set_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Authority_User4,   UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,UID_ACE_User4_Set_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Authority_User5,   UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,UID_ACE_User5_Set_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Authority_User6,   UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,UID_ACE_User6_Set_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Authority_User7,   UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,UID_ACE_User7_Set_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Authority_User8,   UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,UID_ACE_User8_Set_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Authority_User9,   UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,UID_ACE_User9_Set_CommonName,0,0}, UID_ACE_Anybody},
    #else
        {UID_Authority_Anybody, UID_MethodID_Get,       {UID_ACE_Authority_Get_All,0,0,0},     UID_ACE_Anybody},
        {UID_Authority_Admins,  UID_MethodID_Get,       {UID_ACE_Authority_Get_All,0,0,0},     UID_ACE_Anybody},
        {UID_Authority_Admin1,  UID_MethodID_Get,       {UID_ACE_Authority_Get_All,0,0,0},     UID_ACE_Anybody},
        {UID_Authority_Users,   UID_MethodID_Get,       {UID_ACE_Authority_Get_All,0,0,0},     UID_ACE_Anybody},
        {UID_Authority_User1,   UID_MethodID_Get,       {UID_ACE_Authority_Get_All,0,0,0},     UID_ACE_Anybody},
        {UID_Authority_User2,   UID_MethodID_Get,       {UID_ACE_Authority_Get_All,0,0,0},     UID_ACE_Anybody},
        {UID_Authority_User1,   UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,0,0,0}, UID_ACE_Anybody},
        {UID_Authority_User2,   UID_MethodID_Set,       {UID_ACE_Authority_Set_Enabled,0,0,0}, UID_ACE_Anybody},
    #endif

        //CPIN: 25 rows
        {UID_CPIN,       UID_MethodID_Next, {UID_ACE_Anybody,0,0,0},            UID_ACE_Anybody}, //R1: CPIN.Next
    #if _TCG_ != TCG_PYRITE
        {UID_CPIN_Admin1, UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},  //R2:
        {UID_CPIN_Admin1, UID_MethodID_Set, {UID_ACE_C_PIN_Admins_Set_PIN,0,0,0},       UID_ACE_Anybody},
        {UID_CPIN_Admin2, UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_Admin2, UID_MethodID_Set, {UID_ACE_C_PIN_Admins_Set_PIN,0,0,0},       UID_ACE_Anybody},
        {UID_CPIN_Admin3, UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_Admin3, UID_MethodID_Set, {UID_ACE_C_PIN_Admins_Set_PIN,0,0,0},       UID_ACE_Anybody},
        {UID_CPIN_Admin4, UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_Admin4, UID_MethodID_Set, {UID_ACE_C_PIN_Admins_Set_PIN,0,0,0},       UID_ACE_Anybody},
        {UID_CPIN_User1,  UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_User1,  UID_MethodID_Set, {UID_ACE_C_PIN_User1_Set_PIN,0,0,0},        UID_ACE_Anybody},
        {UID_CPIN_User2,  UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_User2,  UID_MethodID_Set, {UID_ACE_C_PIN_User2_Set_PIN,0,0,0},        UID_ACE_Anybody},
        {UID_CPIN_User3,  UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_User3,  UID_MethodID_Set, {UID_ACE_C_PIN_User3_Set_PIN,0,0,0},        UID_ACE_Anybody},
        {UID_CPIN_User4,  UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_User4,  UID_MethodID_Set, {UID_ACE_C_PIN_User4_Set_PIN,0,0,0},        UID_ACE_Anybody},
        {UID_CPIN_User5,  UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_User5,  UID_MethodID_Set, {UID_ACE_C_PIN_User5_Set_PIN,0,0,0},        UID_ACE_Anybody},
        {UID_CPIN_User6,  UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_User6,  UID_MethodID_Set, {UID_ACE_C_PIN_User6_Set_PIN,0,0,0},        UID_ACE_Anybody},
        {UID_CPIN_User7,  UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_User7,  UID_MethodID_Set, {UID_ACE_C_PIN_User7_Set_PIN,0,0,0},        UID_ACE_Anybody},
        {UID_CPIN_User8,  UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_User8,  UID_MethodID_Set, {UID_ACE_C_PIN_User8_Set_PIN,0,0,0},        UID_ACE_Anybody},
        {UID_CPIN_User9,  UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_User9,  UID_MethodID_Set, {UID_ACE_C_PIN_User9_Set_PIN,0,0,0},        UID_ACE_Anybody},
    #else
        {UID_CPIN_Admin1, UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},  //R2:
        {UID_CPIN_Admin1, UID_MethodID_Set, {UID_ACE_C_PIN_Admins_Set_PIN,0,0,0},       UID_ACE_Anybody},
        {UID_CPIN_User1,  UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_User1,  UID_MethodID_Set, {UID_ACE_C_PIN_User1_Set_PIN,0,0,0},        UID_ACE_Anybody},
        {UID_CPIN_User2,  UID_MethodID_Get, {UID_ACE_C_PIN_Admins_Get_All_NOPIN,0,0,0}, UID_ACE_Anybody},
        {UID_CPIN_User2,  UID_MethodID_Set, {UID_ACE_C_PIN_User2_Set_PIN,0,0,0},        UID_ACE_Anybody},
    #endif
    #if _TCG_ != TCG_PYRITE    //SecretPortect: +2 rows
        {UID_SecretProtect,     UID_MethodID_Next,      {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_SecretProtect_256, UID_MethodID_Get,       {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
    #endif
        //LockingInfo: 1 row
        {UID_LockingInfo,       UID_MethodID_Get, {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},

        //Locking: 11 rows + 8 rows
        {UID_Locking,           UID_MethodID_Next,{UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_Locking_GRange,    UID_MethodID_Get, {UID_ACE_Locking_GRange_Get_RangeStartToActiveKey,UID_ACE_Anybody_Get_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range1,    UID_MethodID_Get, {UID_ACE_Locking_Range1_Get_RangeStartToActiveKey,UID_ACE_Anybody_Get_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range2,    UID_MethodID_Get, {UID_ACE_Locking_Range2_Get_RangeStartToActiveKey,UID_ACE_Anybody_Get_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range3,    UID_MethodID_Get, {UID_ACE_Locking_Range3_Get_RangeStartToActiveKey,UID_ACE_Anybody_Get_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range4,    UID_MethodID_Get, {UID_ACE_Locking_Range4_Get_RangeStartToActiveKey,UID_ACE_Anybody_Get_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range5,    UID_MethodID_Get, {UID_ACE_Locking_Range5_Get_RangeStartToActiveKey,UID_ACE_Anybody_Get_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range6,    UID_MethodID_Get, {UID_ACE_Locking_Range6_Get_RangeStartToActiveKey,UID_ACE_Anybody_Get_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range7,    UID_MethodID_Get, {UID_ACE_Locking_Range7_Get_RangeStartToActiveKey,UID_ACE_Anybody_Get_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range8,    UID_MethodID_Get, {UID_ACE_Locking_Range8_Get_RangeStartToActiveKey,UID_ACE_Anybody_Get_CommonName,0,0}, UID_ACE_Anybody},
        {UID_Locking_GRange,    UID_MethodID_Set, {UID_ACE_Locking_GlblRng_Admins_Set,UID_ACE_Locking_GRange_Set_RdLocked,UID_ACE_Locking_GRange_Set_WrLocked,UID_ACE_Admins_Set_CommonName}, UID_ACE_Anybody}, //ACL: >1
        {UID_Locking_Range1,    UID_MethodID_Set, {UID_ACE_Locking_Admins_RangeStartToLocked,UID_ACE_Locking_Range1_Set_RdLocked,UID_ACE_Locking_Range1_Set_WrLocked,UID_ACE_Admins_Set_CommonName},    UID_ACE_Anybody}, //ACL: >1
        {UID_Locking_Range2,    UID_MethodID_Set, {UID_ACE_Locking_Admins_RangeStartToLocked,UID_ACE_Locking_Range2_Set_RdLocked,UID_ACE_Locking_Range2_Set_WrLocked,UID_ACE_Admins_Set_CommonName},    UID_ACE_Anybody}, //ACL: >1
        {UID_Locking_Range3,    UID_MethodID_Set, {UID_ACE_Locking_Admins_RangeStartToLocked,UID_ACE_Locking_Range3_Set_RdLocked,UID_ACE_Locking_Range3_Set_WrLocked,UID_ACE_Admins_Set_CommonName},    UID_ACE_Anybody}, //ACL: >1
        {UID_Locking_Range4,    UID_MethodID_Set, {UID_ACE_Locking_Admins_RangeStartToLocked,UID_ACE_Locking_Range4_Set_RdLocked,UID_ACE_Locking_Range4_Set_WrLocked,UID_ACE_Admins_Set_CommonName},    UID_ACE_Anybody}, //ACL: >1
        {UID_Locking_Range5,    UID_MethodID_Set, {UID_ACE_Locking_Admins_RangeStartToLocked,UID_ACE_Locking_Range5_Set_RdLocked,UID_ACE_Locking_Range5_Set_WrLocked,UID_ACE_Admins_Set_CommonName},    UID_ACE_Anybody}, //ACL: >1
        {UID_Locking_Range6,    UID_MethodID_Set, {UID_ACE_Locking_Admins_RangeStartToLocked,UID_ACE_Locking_Range6_Set_RdLocked,UID_ACE_Locking_Range6_Set_WrLocked,UID_ACE_Admins_Set_CommonName},    UID_ACE_Anybody}, //ACL: >1
        {UID_Locking_Range7,    UID_MethodID_Set, {UID_ACE_Locking_Admins_RangeStartToLocked,UID_ACE_Locking_Range7_Set_RdLocked,UID_ACE_Locking_Range7_Set_WrLocked,UID_ACE_Admins_Set_CommonName},    UID_ACE_Anybody}, //ACL: >1
        {UID_Locking_Range8,    UID_MethodID_Set, {UID_ACE_Locking_Admins_RangeStartToLocked,UID_ACE_Locking_Range8_Set_RdLocked,UID_ACE_Locking_Range8_Set_WrLocked,UID_ACE_Admins_Set_CommonName},    UID_ACE_Anybody}, //ACL: >1
        {UID_Locking_GRange,    ~UID_MethodID_Erase, {UID_ACE_Locking_GRange_Erase,0,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range1,    ~UID_MethodID_Erase, {UID_ACE_Locking_Range1_Erase,0,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range2,    ~UID_MethodID_Erase, {UID_ACE_Locking_Range2_Erase,0,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range3,    ~UID_MethodID_Erase, {UID_ACE_Locking_Range3_Erase,0,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range4,    ~UID_MethodID_Erase, {UID_ACE_Locking_Range4_Erase,0,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range5,    ~UID_MethodID_Erase, {UID_ACE_Locking_Range5_Erase,0,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range6,    ~UID_MethodID_Erase, {UID_ACE_Locking_Range6_Erase,0,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range7,    ~UID_MethodID_Erase, {UID_ACE_Locking_Range7_Erase,0,0,0}, UID_ACE_Anybody},
        {UID_Locking_Range8,    ~UID_MethodID_Erase, {UID_ACE_Locking_Range8_Erase,0,0,0}, UID_ACE_Anybody},
    #else
        {UID_Locking_GRange,    UID_MethodID_Get, {UID_ACE_Locking_GRange_Get_RangeStartToActiveKey,0,0,0}, UID_ACE_Anybody},
        {UID_Locking_GRange,    UID_MethodID_Set, {UID_ACE_Locking_GlblRng_Admins_Set,UID_ACE_Locking_GRange_Set_RdLocked,UID_ACE_Locking_GRange_Set_WrLocked,0}, UID_ACE_Anybody}, //ACL: >1
    #endif
        //MBRControl: 2 rows
        {UID_MBRControl,        UID_MethodID_Get, {UID_ACE_Anybody,0,0,0},               UID_ACE_Anybody},
        {UID_MBRControl,        UID_MethodID_Set, {UID_ACE_MBRControl_Admins_Set,UID_ACE_MBRControl_Set_Done,0,0}, UID_ACE_Anybody}, //ACL>1

        //MBR: 2 rows
        {UID_MBR,               UID_MethodID_Get, {UID_ACE_Anybody,0,0,0}, UID_ACE_Anybody},
        {UID_MBR,               UID_MethodID_Set, {UID_ACE_Admin,0,0,0},   UID_ACE_Anybody},

    #if _TCG_ != TCG_PYRITE //K_AES: 10 rows + 8 rows
        {UID_K_AES_256_GRange_Key, UID_MethodID_GenKey,{UID_ACE_K_AES_256_GlobalRange_GenKey,0,0,0}, UID_ACE_Anybody},
        {UID_K_AES_256_GRange_Key, UID_MethodID_Get,   {UID_ACE_K_AES_Mode,0,0,0},              UID_ACE_Anybody},
        {UID_K_AES_256_Range1_Key, UID_MethodID_GenKey,     {UID_ACE_K_AES_256_Range1_GenKey,0,0,0}, UID_ACE_Anybody},
        {UID_K_AES_256_Range1_Key, UID_MethodID_Get,        {UID_ACE_K_AES_Mode,0,0,0},              UID_ACE_Anybody},
        {UID_K_AES_256_Range2_Key, UID_MethodID_GenKey,     {UID_ACE_K_AES_256_Range2_GenKey,0,0,0}, UID_ACE_Anybody},
        {UID_K_AES_256_Range2_Key, UID_MethodID_Get,        {UID_ACE_K_AES_Mode,0,0,0},              UID_ACE_Anybody},
        {UID_K_AES_256_Range3_Key, UID_MethodID_GenKey,     {UID_ACE_K_AES_256_Range3_GenKey,0,0,0}, UID_ACE_Anybody},
        {UID_K_AES_256_Range3_Key, UID_MethodID_Get,        {UID_ACE_K_AES_Mode,0,0,0},              UID_ACE_Anybody},
        {UID_K_AES_256_Range4_Key, UID_MethodID_GenKey,     {UID_ACE_K_AES_256_Range4_GenKey,0,0,0}, UID_ACE_Anybody},
        {UID_K_AES_256_Range4_Key, UID_MethodID_Get,        {UID_ACE_K_AES_Mode,0,0,0},              UID_ACE_Anybody},
        {UID_K_AES_256_Range5_Key, UID_MethodID_GenKey,     {UID_ACE_K_AES_256_Range5_GenKey,0,0,0}, UID_ACE_Anybody},
        {UID_K_AES_256_Range5_Key, UID_MethodID_Get,        {UID_ACE_K_AES_Mode,0,0,0},              UID_ACE_Anybody},
        {UID_K_AES_256_Range6_Key, UID_MethodID_GenKey,     {UID_ACE_K_AES_256_Range6_GenKey,0,0,0}, UID_ACE_Anybody},
        {UID_K_AES_256_Range6_Key, UID_MethodID_Get,        {UID_ACE_K_AES_Mode,0,0,0},              UID_ACE_Anybody},
        {UID_K_AES_256_Range7_Key, UID_MethodID_GenKey,     {UID_ACE_K_AES_256_Range7_GenKey,0,0,0}, UID_ACE_Anybody},
        {UID_K_AES_256_Range7_Key, UID_MethodID_Get,        {UID_ACE_K_AES_Mode,0,0,0},              UID_ACE_Anybody},
        {UID_K_AES_256_Range8_Key, UID_MethodID_GenKey,     {UID_ACE_K_AES_256_Range8_GenKey,0,0,0}, UID_ACE_Anybody},
        {UID_K_AES_256_Range8_Key, UID_MethodID_Get,        {UID_ACE_K_AES_Mode,0,0,0},              UID_ACE_Anybody},
    #endif

    //DataStore: 2 rows
        {UID_DataStore,     UID_MethodID_Get,   {UID_ACE_DataStore_Get_All,0,0,0},  UID_ACE_Anybody},
        {UID_DataStore,     UID_MethodID_Set,   {UID_ACE_DataStore_Set_All,0,0,0},  UID_ACE_Anybody},
    #if _TCG_ != TCG_PYRITE
        {UID_DataStore2,    ~UID_MethodID_Get,  {UID_ACE_DataStore2_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore2,    ~UID_MethodID_Set,  {UID_ACE_DataStore2_Set_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore3,    ~UID_MethodID_Get,  {UID_ACE_DataStore3_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore3,    ~UID_MethodID_Set,  {UID_ACE_DataStore3_Set_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore4,    ~UID_MethodID_Get,  {UID_ACE_DataStore4_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore4,    ~UID_MethodID_Set,  {UID_ACE_DataStore4_Set_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore5,    ~UID_MethodID_Get,  {UID_ACE_DataStore5_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore5,    ~UID_MethodID_Set,  {UID_ACE_DataStore5_Set_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore6,    ~UID_MethodID_Get,  {UID_ACE_DataStore6_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore6,    ~UID_MethodID_Set,  {UID_ACE_DataStore6_Set_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore7,    ~UID_MethodID_Get,  {UID_ACE_DataStore7_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore7,    ~UID_MethodID_Set,  {UID_ACE_DataStore7_Set_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore8,    ~UID_MethodID_Get,  {UID_ACE_DataStore8_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore8,    ~UID_MethodID_Set,  {UID_ACE_DataStore8_Set_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore9,    ~UID_MethodID_Get,  {UID_ACE_DataStore9_Get_All,0,0,0}, UID_ACE_Anybody},
        {UID_DataStore9,    ~UID_MethodID_Set,  {UID_ACE_DataStore9_Set_All,0,0,0}, UID_ACE_Anybody},
    #endif
    },
    { //ColPty
        {0x01,8,UID_TYPE},          //InvokingID
        {0x02,8,UID_TYPE},          //MethodID
        {0x04,sizeof(G2.b.mLckAccessCtrl_Tbl.val[0].acl),UIDLIST_TYPE},      //ACL
        {0x08,8,UID_TYPE}           //GetACLACL
    },
    sizeof(G2.b.mLckAccessCtrl_Tbl.pty)/sizeof(sColPrty),                    //ColCnt
    sizeof(G2.b.mLckAccessCtrl_Tbl.val)/sizeof(sLckAccessCtrl_TblObj),       //RowCnt, count the total rows, no matter Additional DataStoreTbls are added or not
    sizeof(sLckAccessCtrl_TblObj),
    0x0e
},

//__align(4) const sSecretProtect_Tbl cLckSecretProtect_Tbl=
{
    {
        {UID_SecretProtect_256, UID_K_AES_256, 0x03, {0x01,0x01,0x00}}  //ProtectMechanisms=1 for eDrive
    },
    { //ColPty
        {0x00,8,UID_TYPE},      //UID
        {0x01,8,UID_TYPE},      //table
        {0x02,sizeof(G2.b.mLckSecretProtect_Tbl.val[0].colNumber),VALUE_TYPE},    //colNumber
        {0x03,sizeof(G2.b.mLckSecretProtect_Tbl.val[0].protectMechanism),LIST_TYPE},    //protectMechanisms
    },
    sizeof(G2.b.mLckSecretProtect_Tbl.pty)/sizeof(sColPrty),                 //ColCnt
    sizeof(G2.b.mLckSecretProtect_Tbl.val)/sizeof(sSecretProtect_TblObj),    //RowCnt
    sizeof(sSecretProtect_Tbl),
    0x03
},

//__align(4) const sLockingInfo_Tbl cLckLockingInfo_Tbl=
{
    {
    #if _TCG_ != TCG_PYRITE
        {UID_LockingInfo, "", 0x01, 0x01, LOCKING_RANGE_CNT, TRUE, LBU_SIZE, TCG_AlignmentGranularity, 0, {0,0,0,0,0,0,0,0,0}, 1}  //512B, 8KB/page
    #else
        {UID_LockingInfo, "", 0x01, 0x00, LOCKING_RANGE_CNT, TRUE, LBU_SIZE, TCG_AlignmentGranularity, 0, {0}, 1 }
    #endif
    },
    { //ColPty
        {0x00,8,UID_TYPE},      //UID
        {0x01,sizeof(G2.b.mLckLockingInfo_Tbl.val[0].name),STRING_TYPE},     //name
        {0x02,sizeof(G2.b.mLckLockingInfo_Tbl.val[0].version),VALUE_TYPE},   //version
        {0x03,sizeof(G2.b.mLckLockingInfo_Tbl.val[0].encryptSupport),VALUE_TYPE},    //encryptSupport
        {0x04,sizeof(G2.b.mLckLockingInfo_Tbl.val[0].maxRanges),VALUE_TYPE},    //maxRange
        {0x07,sizeof(G2.b.mLckLockingInfo_Tbl.val[0].alignentReuired),VALUE_TYPE},    //AlignmentRequired
        {0x08,sizeof(G2.b.mLckLockingInfo_Tbl.val[0].logicalBlockSize),VALUE_TYPE},    //LogicalBlcokSize
        {0x09,sizeof(G2.b.mLckLockingInfo_Tbl.val[0].alignmentGranularity),VALUE_TYPE},    //AlignmentGranularity
        {0x0A,sizeof(G2.b.mLckLockingInfo_Tbl.val[0].lowestAlignedLBA),VALUE_TYPE},     //LowestAlignedLBA
        {0x60,sizeof(G2.b.mLckLockingInfo_Tbl.val[0].singleUserModeRange),UID2_TYPE},    //0x60 -> 0x600000
        {0x61,sizeof(G2.b.mLckLockingInfo_Tbl.val[0].rangeStartLengthPolicy),VALUE_TYPE}     //RangeStartLengthPolicy
    },
    sizeof(G2.b.mLckLockingInfo_Tbl.pty)/sizeof(sColPrty),               //ColCnt
    sizeof(G2.b.mLckLockingInfo_Tbl.val)/sizeof(sLockingInfo_TblObj),    //RowCnt
    sizeof(sLockingInfo_TblObj),
    0x61
}
}
};  //G2 init end

__align(16) tG3 G3 ={ {
//__align(4) const sLckACE_Tbl cLckACE_Tbl=
{
    {
        {UID_ACE_Anybody,               {UID_Authority_Anybody,0,0,0,0,0,0,0,0,0,0,0,0,0},  {0,0,0,0,0,0,0,0,0,0,0,0}},   //ACE_Anybody, col(All)
        {UID_ACE_Admin,                 {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {0,0,0,0,0,0,0,0,0,0,0,0}},   //ACE_Admin, col(All)
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_Anybody_Get_CommonName,{UID_Authority_Anybody, 0,0,0,0,0,0,0,0,0,0,0,0,0}, {2,0,2,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Admins_Set_CommonName, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {1,2,0,0,0,0,0,0,0,0,0,0}}, //CommonName
    #endif
        {UID_ACE_ACE_Get_All,           {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_ACE_Set_BooleanExpression,{UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,3,0,0,0,0,0,0,0,0,0,0}}, //BooleanExpr
        {UID_ACE_Authority_Get_All,     {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Authority_Set_Enabled, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {1,5,0,0,0,0,0,0,0,0,0,0}},   //Enabled
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_User1_Set_CommonName,  {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {1,2,0,0,0,0,0,0,0,0,0,0}}, //CommonName
        {UID_ACE_User2_Set_CommonName,  {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {1,2,0,0,0,0,0,0,0,0,0,0}}, //CommonName
        {UID_ACE_User3_Set_CommonName,  {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {1,2,0,0,0,0,0,0,0,0,0,0}}, //CommonName
        {UID_ACE_User4_Set_CommonName,  {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {1,2,0,0,0,0,0,0,0,0,0,0}}, //CommonName
        {UID_ACE_User5_Set_CommonName,  {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {1,2,0,0,0,0,0,0,0,0,0,0}}, //CommonName
        {UID_ACE_User6_Set_CommonName,  {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {1,2,0,0,0,0,0,0,0,0,0,0}}, //CommonName
        {UID_ACE_User7_Set_CommonName,  {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {1,2,0,0,0,0,0,0,0,0,0,0}}, //CommonName
        {UID_ACE_User8_Set_CommonName,  {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {1,2,0,0,0,0,0,0,0,0,0,0}}, //CommonName
        {UID_ACE_User9_Set_CommonName,  {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {1,2,0,0,0,0,0,0,0,0,0,0}}, //CommonName
    #endif

        {UID_ACE_C_PIN_Admins_Get_All_NOPIN,{UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0},{5,0,4,5,6,7,0,0,0,0,0,0}}, //ACE_CPIN_SID_Get..., col(UID, CharSet, TryLimit, Tries, Persistence)
        {UID_ACE_C_PIN_Admins_Set_PIN,  {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},  {1,3,0,0,0,0,0,0,0,0,0,0}},      //ACE_CPIN_SID_Set..., col(PIN)
        {UID_ACE_C_PIN_User1_Set_PIN,   {UID_Authority_Admins,UID_Authority_User1,0,0,0,0,0,0,0,0,0,0,0,0},  {1,3,0,0,0,0,0,0,0,0,0,0}},   //Boolean>1
        {UID_ACE_C_PIN_User2_Set_PIN,   {UID_Authority_Admins,UID_Authority_User2,0,0,0,0,0,0,0,0,0,0,0,0},  {1,3,0,0,0,0,0,0,0,0,0,0}},   //Boolean>1
    #if _TCG_ != TCG_PYRITE
        {UID_ACE_C_PIN_User3_Set_PIN,   {UID_Authority_Admins,UID_Authority_User3,0,0,0,0,0,0,0,0,0,0,0,0},  {1,3,0,0,0,0,0,0,0,0,0,0}},   //Boolean>1
        {UID_ACE_C_PIN_User4_Set_PIN,   {UID_Authority_Admins,UID_Authority_User4,0,0,0,0,0,0,0,0,0,0,0,0},  {1,3,0,0,0,0,0,0,0,0,0,0}},   //Boolean>1
        {UID_ACE_C_PIN_User5_Set_PIN,   {UID_Authority_Admins,UID_Authority_User5,0,0,0,0,0,0,0,0,0,0,0,0},  {1,3,0,0,0,0,0,0,0,0,0,0}},   //Boolean>1
        {UID_ACE_C_PIN_User6_Set_PIN,   {UID_Authority_Admins,UID_Authority_User6,0,0,0,0,0,0,0,0,0,0,0,0},  {1,3,0,0,0,0,0,0,0,0,0,0}},   //Boolean>1
        {UID_ACE_C_PIN_User7_Set_PIN,   {UID_Authority_Admins,UID_Authority_User7,0,0,0,0,0,0,0,0,0,0,0,0},  {1,3,0,0,0,0,0,0,0,0,0,0}},   //Boolean>1
        {UID_ACE_C_PIN_User8_Set_PIN,   {UID_Authority_Admins,UID_Authority_User8,0,0,0,0,0,0,0,0,0,0,0,0},  {1,3,0,0,0,0,0,0,0,0,0,0}},   //Boolean>1
        {UID_ACE_C_PIN_User9_Set_PIN,   {UID_Authority_Admins,UID_Authority_User9,0,0,0,0,0,0,0,0,0,0,0,0},  {1,3,0,0,0,0,0,0,0,0,0,0}},   //Boolean>1
    #endif

    #if _TCG_ != TCG_PYRITE
        {UID_ACE_K_AES_256_GlobalRange_GenKey, {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_K_AES_256_Range1_GenKey, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_K_AES_256_Range2_GenKey, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_K_AES_256_Range3_GenKey, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_K_AES_256_Range4_GenKey, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_K_AES_256_Range5_GenKey, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_K_AES_256_Range6_GenKey, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_K_AES_256_Range7_GenKey, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_K_AES_256_Range8_GenKey, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_K_AES_Mode,            {UID_Authority_Anybody,0,0,0,0,0,0,0,0,0,0,0,0,0},  {1,4,0,0,0,0,0,0,0,0,0,0}},
    #endif

    #if _TCG_ != TCG_PYRITE
        {UID_ACE_Locking_GRange_Get_RangeStartToActiveKey, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0}, {8,3,4,5,6,7,8,9,10,0,0,0}},
        {UID_ACE_Locking_Range1_Get_RangeStartToActiveKey, {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0}, {8,3,4,5,6,7,8,9,10,0,0,0}},
        {UID_ACE_Locking_Range2_Get_RangeStartToActiveKey, {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0}, {8,3,4,5,6,7,8,9,10,0,0,0}},
        {UID_ACE_Locking_Range3_Get_RangeStartToActiveKey, {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0}, {8,3,4,5,6,7,8,9,10,0,0,0}},
        {UID_ACE_Locking_Range4_Get_RangeStartToActiveKey, {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0}, {8,3,4,5,6,7,8,9,10,0,0,0}},
        {UID_ACE_Locking_Range5_Get_RangeStartToActiveKey, {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0}, {8,3,4,5,6,7,8,9,10,0,0,0}},
        {UID_ACE_Locking_Range6_Get_RangeStartToActiveKey, {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0}, {8,3,4,5,6,7,8,9,10,0,0,0}},
        {UID_ACE_Locking_Range7_Get_RangeStartToActiveKey, {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0}, {8,3,4,5,6,7,8,9,10,0,0,0}},
        {UID_ACE_Locking_Range8_Get_RangeStartToActiveKey, {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0}, {8,3,4,5,6,7,8,9,10,0,0,0}},
        {UID_ACE_Locking_GRange_Set_RdLocked, {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0}, {1,7,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range1_Set_RdLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,7,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range2_Set_RdLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,7,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range3_Set_RdLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,7,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range4_Set_RdLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,7,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range5_Set_RdLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,7,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range6_Set_RdLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,7,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range7_Set_RdLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,7,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range8_Set_RdLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,7,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_GRange_Set_WrLocked, {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,8,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range1_Set_WrLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,8,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range2_Set_WrLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,8,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range3_Set_WrLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,8,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range4_Set_WrLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,8,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range5_Set_WrLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,8,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range6_Set_WrLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,8,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range7_Set_WrLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,8,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range8_Set_WrLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},    {1,8,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_GlblRng_Admins_Set,        {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0}, {5,5,6,7,8,9,0,0,0,0,0,0}},
        {UID_ACE_Locking_Admins_RangeStartToLocked, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{7,3,4,5,6,7,8,9,0,0,0,0}},
    #else
        {UID_ACE_Locking_GRange_Get_RangeStartToActiveKey, {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0}, {8,3,4,5,6,7,8,9,10,0,0,0}},
        {UID_ACE_Locking_GRange_Set_RdLocked,       {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0}, {1,7,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_GRange_Set_WrLocked,       {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,8,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_GlblRng_Admins_Set,        {UID_Authority_Admins,0,0,0,0,0,0,0,0,0,0,0,0,0}, {5,5,6,7,8,9,0,0,0,0,0,0}},
    #endif

        {UID_ACE_MBRControl_Admins_Set,             {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{3,1,2,3,0,0,0,0,0,0,0,0}},
        {UID_ACE_MBRControl_Set_Done,               {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{2,2,3,0,0,0,0,0,0,0,0,0}},

        {UID_ACE_DataStore_Get_All,                 {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore_Set_All,                 {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}}
    #if _TCG_ != TCG_PYRITE
       ,{UID_ACE_DataStore2_Get_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore2_Set_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore3_Get_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore3_Set_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore4_Get_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore4_Set_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore5_Get_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore5_Set_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore6_Get_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore6_Set_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore7_Get_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore7_Set_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore8_Get_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore8_Set_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore9_Get_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_DataStore9_Set_All|UID_FF,        {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}}
       ,{UID_ACE_SP_Reactivate_Admin,               {UID_Authority_Admins, 0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_GRange_Set_ReadToLOR|UID_FF,{UID_Authority_User1,0,0,0,0,0,0,0,0,0,0,0,0,0},{5,5,6,7,8,9,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range1_Set_ReadToLOR|UID_FF,{UID_Authority_User2,0,0,0,0,0,0,0,0,0,0,0,0,0},{5,5,6,7,8,9,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range2_Set_ReadToLOR|UID_FF,{UID_Authority_User3,0,0,0,0,0,0,0,0,0,0,0,0,0},{5,5,6,7,8,9,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range3_Set_ReadToLOR|UID_FF,{UID_Authority_User4,0,0,0,0,0,0,0,0,0,0,0,0,0},{5,5,6,7,8,9,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range4_Set_ReadToLOR|UID_FF,{UID_Authority_User5,0,0,0,0,0,0,0,0,0,0,0,0,0},{5,5,6,7,8,9,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range5_Set_ReadToLOR|UID_FF,{UID_Authority_User6,0,0,0,0,0,0,0,0,0,0,0,0,0},{5,5,6,7,8,9,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range6_Set_ReadToLOR|UID_FF,{UID_Authority_User7,0,0,0,0,0,0,0,0,0,0,0,0,0},{5,5,6,7,8,9,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range7_Set_ReadToLOR|UID_FF,{UID_Authority_User8,0,0,0,0,0,0,0,0,0,0,0,0,0},{5,5,6,7,8,9,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range8_Set_ReadToLOR|UID_FF,{UID_Authority_User9,0,0,0,0,0,0,0,0,0,0,0,0,0},{5,5,6,7,8,9,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range1_Set_Range|UID_FF,    {UID_Authority_User2,0,0,0,0,0,0,0,0,0,0,0,0,0},{2,3,4,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range2_Set_Range|UID_FF,    {UID_Authority_User3,0,0,0,0,0,0,0,0,0,0,0,0,0},{2,3,4,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range3_Set_Range|UID_FF,    {UID_Authority_User4,0,0,0,0,0,0,0,0,0,0,0,0,0},{2,3,4,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range4_Set_Range|UID_FF,    {UID_Authority_User5,0,0,0,0,0,0,0,0,0,0,0,0,0},{2,3,4,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range5_Set_Range|UID_FF,    {UID_Authority_User6,0,0,0,0,0,0,0,0,0,0,0,0,0},{2,3,4,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range6_Set_Range|UID_FF,    {UID_Authority_User7,0,0,0,0,0,0,0,0,0,0,0,0,0},{2,3,4,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range7_Set_Range|UID_FF,    {UID_Authority_User8,0,0,0,0,0,0,0,0,0,0,0,0,0},{2,3,4,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range8_Set_Range|UID_FF,    {UID_Authority_User9,0,0,0,0,0,0,0,0,0,0,0,0,0},{2,3,4,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_CPIN_Anybody_Get_NoPIN|UID_FF,     {UID_Authority_Anybody,0,0,0,0,0,0,0,0,0,0,0,0,0},{5,0,4,5,6,7,0,0,0,0,0,0}},
        {UID_ACE_Locking_GRange_Erase|UID_FF,       {UID_Authority_Admins, UID_Authority_User1,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range1_Erase|UID_FF,       {UID_Authority_Admins, UID_Authority_User2,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range2_Erase|UID_FF,       {UID_Authority_Admins, UID_Authority_User3,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range3_Erase|UID_FF,       {UID_Authority_Admins, UID_Authority_User4,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range4_Erase|UID_FF,       {UID_Authority_Admins, UID_Authority_User5,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range5_Erase|UID_FF,       {UID_Authority_Admins, UID_Authority_User6,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range6_Erase|UID_FF,       {UID_Authority_Admins, UID_Authority_User7,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range7_Erase|UID_FF,       {UID_Authority_Admins, UID_Authority_User8,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
        {UID_ACE_Locking_Range8_Erase|UID_FF,       {UID_Authority_Admins, UID_Authority_User9,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}}
    #endif
    },
    { //ColPty
        {0x00,8,UID_TYPE},      //UID
        {0x03,sizeof(G3.b.mLckACE_Tbl.val[0].booleanExpr),UIDLIST_TYPE}, //BooleanExpr
        {0x04,sizeof(G3.b.mLckACE_Tbl.val[0].col),LIST_TYPE}             //Columns
    },
    sizeof(G3.b.mLckACE_Tbl.pty)/sizeof(sColPrty),                       //ColCnt
    sizeof(G3.b.mLckACE_Tbl.val)/sizeof(sLckACE_TblObj),                 //RowCnt
    sizeof(sLckACE_TblObj),
    0x04
},

//__align(4) const sLckAuthority_Tbl cLckAuthority_Tbl=
{
    {
        {UID_Authority_Anybody,"Anybody",   "", FALSE,UID_Null,            TRUE, SECURE_None, HASH_None, FALSE, AUTH_None,     UID_Null,        UID_Null, UID_Null},
        {UID_Authority_Admins, "Admins",    "", TRUE, UID_Null,            TRUE, SECURE_None, HASH_None, FALSE, AUTH_None,     UID_Null,        UID_Null, UID_Null},
        {UID_Authority_Admin1, "Admin1",    "", FALSE,UID_Authority_Admins,TRUE, SECURE_None, HASH_None, FALSE, AUTH_Password, UID_CPIN_Admin1, UID_Null, UID_Null},
    #if _TCG_ != TCG_PYRITE
        {UID_Authority_Admin2, "Admin2",    "", FALSE,UID_Authority_Admins,FALSE,SECURE_None, HASH_None, FALSE, AUTH_Password, UID_CPIN_Admin2, UID_Null, UID_Null},
        {UID_Authority_Admin3, "Admin3",    "", FALSE,UID_Authority_Admins,FALSE,SECURE_None, HASH_None, FALSE, AUTH_Password, UID_CPIN_Admin3, UID_Null, UID_Null},
        {UID_Authority_Admin4, "Admin4",    "", FALSE,UID_Authority_Admins,FALSE,SECURE_None, HASH_None, FALSE, AUTH_Password, UID_CPIN_Admin4, UID_Null, UID_Null},
    #endif
        {UID_Authority_Users,  "Users ",    "", TRUE, UID_Null,            TRUE, SECURE_None, HASH_None, FALSE, AUTH_None,     UID_Null,        UID_Null, UID_Null},
        {UID_Authority_User1,  "User1",     "", FALSE,UID_Authority_Users, FALSE,SECURE_None, HASH_None, FALSE, AUTH_Password, UID_CPIN_User1,  UID_Null, UID_Null},
        {UID_Authority_User2,  "User2",     "", FALSE,UID_Authority_Users, FALSE,SECURE_None, HASH_None, FALSE, AUTH_Password, UID_CPIN_User2,  UID_Null, UID_Null},
    #if _TCG_ != TCG_PYRITE
        {UID_Authority_User3,  "User3",     "", FALSE,UID_Authority_Users, FALSE,SECURE_None, HASH_None, FALSE, AUTH_Password, UID_CPIN_User3,  UID_Null, UID_Null},
        {UID_Authority_User4,  "User4",     "", FALSE,UID_Authority_Users, FALSE,SECURE_None, HASH_None, FALSE, AUTH_Password, UID_CPIN_User4,  UID_Null, UID_Null},
        {UID_Authority_User5,  "User5",     "", FALSE,UID_Authority_Users, FALSE,SECURE_None, HASH_None, FALSE, AUTH_Password, UID_CPIN_User5,  UID_Null, UID_Null},
        {UID_Authority_User6,  "User6",     "", FALSE,UID_Authority_Users, FALSE,SECURE_None, HASH_None, FALSE, AUTH_Password, UID_CPIN_User6,  UID_Null, UID_Null},
        {UID_Authority_User7,  "User7",     "", FALSE,UID_Authority_Users, FALSE,SECURE_None, HASH_None, FALSE, AUTH_Password, UID_CPIN_User7,  UID_Null, UID_Null},
        {UID_Authority_User8,  "User8",     "", FALSE,UID_Authority_Users, FALSE,SECURE_None, HASH_None, FALSE, AUTH_Password, UID_CPIN_User8,  UID_Null, UID_Null},
        {UID_Authority_User9,  "User9",     "", FALSE,UID_Authority_Users, FALSE,SECURE_None, HASH_None, FALSE, AUTH_Password, UID_CPIN_User9,  UID_Null, UID_Null}
    #endif
    },
    { //ColPty
        {0x00,8,UID_TYPE},      //UID
        {0x01,sizeof(G3.b.mLckAuthority_Tbl.val[0].name),STRING_TYPE},
        {0x02,sizeof(G3.b.mLckAuthority_Tbl.val[0].commonName),STRING_TYPE},
        {0x03,sizeof(G3.b.mLckAuthority_Tbl.val[0].isClass),VALUE_TYPE},    //IsClass (bool)
        {0x04,8,UID_TYPE},      //Class
        {0x05,sizeof(G3.b.mLckAuthority_Tbl.val[0].enabled),VALUE_TYPE},    //Enabled (bool)
        {0x06,sizeof(secure_message),VALUE_TYPE},    //Secure
        {0x07,sizeof(hash_protocol),VALUE_TYPE},    //HashAndSign
        {0x08,sizeof(G3.b.mLckAuthority_Tbl.val[0].presentCertificate),VALUE_TYPE},    //PresentCertificate (bool)
        {0x09,sizeof(auth_method),VALUE_TYPE},    //Operation
        {0x0A,8,UID_TYPE},      //Credential (UID)
        {0x0B,8,UID_TYPE},      //ResponseSign
        {0x0C,8,UID_TYPE}       //ResponseExch
    },
    sizeof(G3.b.mLckAuthority_Tbl.pty)/sizeof(sColPrty),             //ColCnt
    sizeof(G3.b.mLckAuthority_Tbl.val)/sizeof(sLckAuthority_TblObj),    //RowCnt
    sizeof(sLckAuthority_TblObj),
    0x12
},

//__align(4) const sLckCPin_Tbl cLckCPin_Tbl=
{
    {
        {   UID_CPIN_Admin1, "C_PIN_Admin1",
            //{15,'<','M','S','I','D','_','p','a','s','s','w','o','r','d','>', \
            //0,   0,   0,   0,    0,   0,   0,   0,    0,   0,   0,   0},
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0},
    #if _TCG_ != TCG_PYRITE
        {   UID_CPIN_Admin2, "C_PIN_Admin2",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0},
        {   UID_CPIN_Admin3, "C_PIN_Admin3",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0},
        {   UID_CPIN_Admin4, "C_PIN_Admin4",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0},
    #endif
        {   UID_CPIN_User1, "C_PIN_User1",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0},
        {   UID_CPIN_User2, "C_PIN_User2",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0},
    #if _TCG_ != TCG_PYRITE
        {   UID_CPIN_User3, "C_PIN_User3",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0},
        {   UID_CPIN_User4, "C_PIN_User4",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0},
        {   UID_CPIN_User5, "C_PIN_User5",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0},
        {   UID_CPIN_User6, "C_PIN_User6",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0},
        {   UID_CPIN_User7, "C_PIN_User7",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0},
        {   UID_CPIN_User8, "C_PIN_User8",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0},
        {   UID_CPIN_User9, "C_PIN_User9",
            { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, \
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
            UID_Null, 5, 0, FALSE, 0, 0, 0},
    #endif
    },
    { //ColPty
        {0x00,sizeof(G3.b.mLckCPin_Tbl.val[0].uid),UID_TYPE},       //UID
        {0x01,sizeof(G3.b.mLckCPin_Tbl.val[0].name),STRING_TYPE},
        //{0x03,sizeof(U8*),VALUE_TYPE},  //PIN, check later ...
        {0x03,sizeof(G3.b.mLckCPin_Tbl.val[0].pwd),VBYTE_TYPE},  //PIN
        {0x04,sizeof(G3.b.mLckCPin_Tbl.val[0].charSet),UID_TYPE},      //CharSet
        {0x05,sizeof(G3.b.mLckCPin_Tbl.val[0].tryLimit),VALUE_TYPE},    //TryLimit
        {0x06,sizeof(G3.b.mLckCPin_Tbl.val[0].tries),VALUE_TYPE},    //Tries
        {0x07,sizeof(G3.b.mLckCPin_Tbl.val[0].presistence),VALUE_TYPE}     //Persistence (bool)
    },
    sizeof(G3.b.mLckCPin_Tbl.pty)/sizeof(sColPrty),      //ColCnt
    sizeof(G3.b.mLckCPin_Tbl.val)/sizeof(sCPin_TblObj),   //RowCnt
    sizeof(sCPin_TblObj),
    0x07
},


//__align(4) const sLocking_Tbl cLckLocking_Tbl=
{
    {
    #if _TCG_ != TCG_PYRITE
        {UID_Locking_GRange,      "Locking_GlobalRange", "", 0x00,0x00, 0,0,0,0, {1,PowerCycle,0,0},UID_K_AES_256_GRange_Key},
        {UID_Locking_Range1,      "Locking_Range1",      "", 0x00,0x00, 0,0,0,0, {1,PowerCycle,0,0},UID_K_AES_256_Range1_Key},
        {UID_Locking_Range2,      "Locking_Range2",      "", 0x00,0x00, 0,0,0,0, {1,PowerCycle,0,0},UID_K_AES_256_Range2_Key},
        {UID_Locking_Range3,      "Locking_Range3",      "", 0x00,0x00, 0,0,0,0, {1,PowerCycle,0,0},UID_K_AES_256_Range3_Key},
        {UID_Locking_Range4,      "Locking_Range4",      "", 0x00,0x00, 0,0,0,0, {1,PowerCycle,0,0},UID_K_AES_256_Range4_Key},
        {UID_Locking_Range5,      "Locking_Range5",      "", 0x00,0x00, 0,0,0,0, {1,PowerCycle,0,0},UID_K_AES_256_Range5_Key},
        {UID_Locking_Range6,      "Locking_Range6",      "", 0x00,0x00, 0,0,0,0, {1,PowerCycle,0,0},UID_K_AES_256_Range6_Key},
        {UID_Locking_Range7,      "Locking_Range7",      "", 0x00,0x00, 0,0,0,0, {1,PowerCycle,0,0},UID_K_AES_256_Range7_Key},
        {UID_Locking_Range8,      "Locking_Range8",      "", 0x00,0x00, 0,0,0,0, {1,PowerCycle,0,0},UID_K_AES_256_Range8_Key}
    #else
        {UID_Locking_GRange,      "Locking_GlobalRange", "", 0x00,0x00, 0,0,0,0, {1,PowerCycle,0,0},UID_Null},
    #endif
    },
    { //ColPty
        {0x00,8,UID_TYPE},      //UID
        {0x01,sizeof(G3.b.mLckLocking_Tbl.val[0].name),STRING_TYPE},         //name
        {0x02,sizeof(G3.b.mLckLocking_Tbl.val[0].commonName),STRING_TYPE},
        {0x03,sizeof(G3.b.mLckLocking_Tbl.val[0].rangeStart),VALUE_TYPE},    //rangeStart
        {0x04,sizeof(G3.b.mLckLocking_Tbl.val[0].rangeLength),VALUE_TYPE},
        {0x05,1,VALUE_TYPE},    //readLockEnabled
        {0x06,1,VALUE_TYPE},    //writeLockEnabled
        {0x07,1,VALUE_TYPE},    //readLocked
        {0x08,1,VALUE_TYPE},    //writeLocked
        {0x09,sizeof(G3.b.mLckLocking_Tbl.val[0].lockOnReset),LIST_TYPE},    //LockOnReset
        {0x0A,8,UID_TYPE}       //ActiveKey
    },
    sizeof(G3.b.mLckLocking_Tbl.pty)/sizeof(sColPrty),        //ColCnt
    sizeof(G3.b.mLckLocking_Tbl.val)/sizeof(sLocking_TblObj),  //RowCnt jg
    sizeof(sLocking_TblObj),
    0x13
},


//__align(4) const sMbrCtrl_Tbl cLckMbrCtrl_Tbl=
{
    {
        {UID_MBRControl, 0, 0, {1,PowerCycle,0,0}}
    },
    { //ColPty
        {0x00,8,UID_TYPE},      //UID
        {0x01,sizeof(G3.b.mLckMbrCtrl_Tbl.val[0].enable),VALUE_TYPE},    //Enable
        {0x02,sizeof(G3.b.mLckMbrCtrl_Tbl.val[0].done),VALUE_TYPE},    //Done
        {0x03,sizeof(G3.b.mLckMbrCtrl_Tbl.val[0].doneOnReset),LIST_TYPE}       //DoneOnReset
    },
    sizeof(G3.b.mLckMbrCtrl_Tbl.pty)/sizeof(sColPrty),        //ColCnt
    sizeof(G3.b.mLckMbrCtrl_Tbl.val)/sizeof(sMbrCtrl_TblObj),  //RowCnt jg
    sizeof(sMbrCtrl_TblObj),
    0x03
},

//__align(4) const sKAES_Tbl cLckKAES_256_Tbl=
{
    {
        {UID_K_AES_256_GRange_Key,  {0x11111111,0x22222222,0x33333333,0x44444444,0x55555555,0x66666666,0x77777777,0x88888888}, AES_XTS},    //AES_CTR
    #if _TCG_ != TCG_PYRITE
        {UID_K_AES_256_Range1_Key,  {0,0,0,0,0,0,0,0}, AES_XTS},    //eDrive, AES_CTR
        {UID_K_AES_256_Range2_Key,  {0,0,0,0,0,0,0,0}, AES_XTS},
        {UID_K_AES_256_Range3_Key,  {0,0,0,0,0,0,0,0}, AES_XTS},
        {UID_K_AES_256_Range4_Key,  {0,0,0,0,0,0,0,0}, AES_XTS},
        {UID_K_AES_256_Range5_Key,  {0,0,0,0,0,0,0,0}, AES_XTS},
        {UID_K_AES_256_Range6_Key,  {0,0,0,0,0,0,0,0}, AES_XTS},
        {UID_K_AES_256_Range7_Key,  {0,0,0,0,0,0,0,0}, AES_XTS},
        {UID_K_AES_256_Range8_Key,  {0,0,0,0,0,0,0,0}, AES_XTS}
    #endif
    },
    { //ColPty
        {0x00,8,UID_TYPE},      //UID
        {0x03,sizeof(G3.b.mLckKAES_256_Tbl.val[0].key),FBYTE_TYPE},
        {0x04,sizeof(G3.b.mLckKAES_256_Tbl.val[0].mode),VALUE_TYPE}     //mode
    },
    sizeof(G3.b.mLckKAES_256_Tbl.pty)/sizeof(sColPrty),        //ColCnt
    sizeof(G3.b.mLckKAES_256_Tbl.val)/sizeof(sKAES_TblObj),  //RowCnt jg
    sizeof(sKAES_TblObj),
    0x04
}
}
};      //G3 init end








#pragma once
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
//#define _ALEXDEBUG_PAA
#define TCG_COLORFUL
#define zBLACK                      "\033[7;30m"
#define zRED                        "\033[7;31m"
#define zGREEN                      "\033[7;32m"
#define zYELLOW                     "\033[7;33m"
#define zBLUE                       "\033[7;34m"
#define zMAGENTA                    "\033[7;35m"
#define zCYAN                       "\033[7;36m"
#define zWHITE                      "\033[7;37m"
#define zNONE                       "\033[m"

#ifdef  TCG_COLORFUL
#define TCG_PRINTF(F,args...)       Debug_DbgPrintf(zGREEN##F##zNONE, ##args)
#define HERE(a)                     a == NULL ? D_PRINTF(zYELLOW"[A]%s\n"zNONE,__FUNCTION__) : D_PRINTF(zYELLOW"[A]%s() %x\n"zNONE,__FUNCTION__, a)
#define HERE2(a,b)                  D_PRINTF(zYELLOW"[A]%s() %4X %4X\n"zNONE,__FUNCTION__, a, b)
#else
#define TCG_PRINTF(F,args...)       Debug_DbgPrintf(F, ##args)    ///< Debug message print out
#define HERE(a)                     a == NULL ? D_PRINTF("[A]%s\n",__FUNCTION__) : D_PRINTF("[A]%s() %x\n",__FUNCTION__, a)
#define HERE2(a,b)                  D_PRINTF("[A]%s() %4X %4X\n",__FUNCTION__, a, b)
#endif

#define zOK     0
#define zNG     -1
#define TCG_EEP_NOR

#define alexcheck

#define TCG_GENERAL_BUF_SIZE        (1 * MBYTE)

#define TPER_FW_VERSION             0x00        //uint32

//------------------------------------------------------
#define TCG_CFG_BLK_CNT_PER_GRP     128   //192 bolcks for L9 , 128 blocks is for M6/M3

#define TCG_MBR_CELLS               TCG_CFG_BLK_CNT_PER_GRP     // L9=192 blocks , M3/M6=128 blocks

// [[[ NAND config parameter ]]]
#define CFG_UDATA_PER_PAGE          16384  //1page = 16K
#define mNUM_AU_PER_PAGE            CFG_UDATA_PER_PAGE/2048
#define CFG_PAGE_ADDR_BIT_CNT       8
#define CFG_CH_BIT_CNT              3  //CH
#define CFG_CE_BIT_CNT              2  //CE
#define CFG_CHANNEL_CNT             (1 << CFG_CH_BIT_CNT)
#define CFG_CE_CNT                  (1 << CFG_CE_BIT_CNT)
#define CFG_LBA_PER_LAA             (4096 / 512)

#if _NAND_ == NAND_3DTLCG2
    #define L2P_PAGE_CNT                5
#elif _NAND_ == NAND_MLC
    #define L2P_PAGE_CNT                3
#endif

#define TCG_SSC_EDRIVE   0xEE
#define TCG_SSC_OPAL     0xDD
#define TCG_SSC_PYRITE   0xCC

#if _TCG_ != TCG_PYRITE
  #if _TCG_ == TCG_EDRV
    #define TCG_DEVICE_TYPE         TCG_SSC_EDRIVE
  #else
    #define TCG_DEVICE_TYPE         TCG_SSC_OPAL
  #endif

  #define TCG_TBL_ID                0x12345678
  #define TCG_TBL_VER               0x0300

  #define LOCKING_RANGE_CNT         8   //total locking range: 9 (=1+8, including global range)
  #define TCG_AdminCnt              4
  #define TCG_UserCnt               (LOCKING_RANGE_CNT+1)
#else
  #define TCG_DEVICE_TYPE           TCG_SSC_PYRITE

  #define TCG_TBL_ID                0x50797269  //Pyri
  #define TCG_TBL_VER               0x0300

  #define LOCKING_RANGE_CNT         0   //GlobalRange only
  #define TCG_AdminCnt              1
  #define TCG_UserCnt               2
#endif

#define DATASTORE_LEN               0x00A20000      //10.28MB
#define MBR_LEN                     0x08000000      //128MB

#define TCG_LogicalBlockSize        512
//#define TCG_AlignmentGranularity    (CFG_UDATA_PER_PAGE/LBA_SIZE_IN_BYTE)  //8KB=16LBA, 16KB=32LBA
#define TCG_AlignmentGranularity    8   //4KB=8LBA

#define TCG_LowestAlignedLBA        0
#define G4G5_LAA_AMOUNT_LIMIT       0x4800  //18432

#define DSTBL_ALIGNMENT             1    //byte
#define DSTBL_MAX_NUM               (LOCKING_RANGE_CNT + 1)

//------------------------------------------------------
#define UID_Null                    (0x0000000000000000)

//UID for SessionManager
#define UID_ThisSP                  0x00000000000001
#define SMUID                       0x000000000000FF
//SessionManager Method UID
#define SM_MTD_Properties           0x000000000000FF01  // Properties
#define SM_MTD_StartSession         0x000000000000FF02  // StartSession
#define SM_MTD_SyncSession          0x000000000000FF03  // SyncSession
//#define SM_MTD_StartTrustedSession    0x000000000000FF04    // StartTrustedSession
//#define SM_MTD_SyncTrustedSession     0x000000000000FF05    // SyncTrustedSession
#define SM_MTD_CloseSession         0x000000000000FF06  // CloseSession

//UID for Column Type
#define UID_CT_Authority_object_ref (0x0000000500000C05)
#define UID_CT_boolean_ACE          (0x000000050000040E)

//UID for AdminSP:
//SPInfo Table
//#define UID_Admin_SPInfo_Admin   (0x0000000200000001)
#define UID_SPInfo                  (0x0000000200000001)

//SPTemplate Table
#define UID_SPTemplate              (0x0000000300000000)
#define UID_SPTemplate_1            (0x0000000300000001)
#define UID_SPTemplate_2            (0x0000000300000002)

//Table Table
#define UID_Table                   (0x0000000100000000)
#define UID_Table_Table             (0x0000000100000001)
#define UID_Table_SPInfo            (0x0000000100000002)
#define UID_Table_SPTemplates       (0x0000000100000003)
#define UID_Table_MethodID          (0x0000000100000006)
#define UID_Table_AccessControl     (0x0000000100000007)
#define UID_Table_ACE               (0x0000000100000008)
#define UID_Table_Authority         (0x0000000100000009)
#define UID_Table_CPIN              (0x000000010000000B)
#define UID_Table_TPerInfo          (0x0000000100000201)
#define UID_Table_Template          (0x0000000100000204)
#define UID_Table_SP                (0x0000000100000205)

//MethodID Table
#define UID_MethodID                (0x0000000600000000)
#define UID_MethodID_Next           (0x0000000600000008)
#define UID_MethodID_GetACL         (0x000000060000000D)
#define UID_MethodID_Get            (0x0000000600000016)
#define UID_MethodID_Set            (0x0000000600000017)
#define UID_MethodID_Authenticate   (0x000000060000001C)    //Opal2
#define UID_MethodID_Revert         (0x0000000600000202)
#define UID_MethodID_Activate       (0x0000000600000203)
#define UID_MethodID_Random         (0x0000000600000601)    //Opal2

//AccessControl Table: x
#define UID_AccessControl           (0x0000000700000000)

//ACE Table
#define UID_ACE                     (0x0000000800000000)
#define UID_ACE_Anybody             (0x0000000800000001)
#define UID_ACE_Admin               (0x0000000800000002)
#define UID_ACE_Makers_Set_Enabled  (0x0000000800030001)
#define UID_ACE_CPIN_SID_Get_NOPIN  (0x0000000800008C02)
#define UID_ACE_CPIN_SID_Set_PIN    (0x0000000800008C03)
#define UID_ACE_CPIN_MSID_Get_PIN   (0x0000000800008C04)
#define UID_ACE_CPIN_Get_PSID_NoPIN (0x00000008000100e1)    //PSID
#define UID_ACE_CPIN_Admins_Set_PIN (0x000000080003A001)    //Opal2
#define UID_ACE_TPerInfo_Set_PReset (0x0000000800030003)    //Opal2
#define UID_ACE_SP_SID              (0x0000000800030002)
#define UID_ACE_SP_PSID             (0x00000008000100e0)

//Authority Table
#define UID_Authority               (0x0000000900000000)
#define UID_Authority_Anybody       (0x0000000900000001)
#define UID_Authority_Admins        (0x0000000900000002)
#define UID_Authority_Makers        (0x0000000900000003)
#define UID_Authority_SID           (0x0000000900000006)
#define UID_Authority_PSID          (0x000000090001FF01)    //eDrive

#define UID_Authority_AdmAdmin1     (0x0000000900000201)
#define UID_Authority_AdmAdmin2     (0x0000000900000202)
#define UID_Authority_AdmAdmin3     (0x0000000900000203)
#define UID_Authority_AdmAdmin4     (0x0000000900000204)

//C_PIN Table
#define UID_CPIN                    (0x0000000B00000000) //CPIN Table
#define UID_CPIN_SID                (0x0000000B00000001)
#define UID_CPIN_MSID               (0x0000000B00008402)

#define UID_CPIN_AdmAdmin1          (0x0000000B00000201)
#define UID_CPIN_AdmAdmin2          (0x0000000B00000202)
#define UID_CPIN_AdmAdmin3          (0x0000000B00000203)
#define UID_CPIN_AdmAdmin4          (0x0000000B00000204)

#define UID_CPIN_PSID               (0x0000000B0001FF01) //eDriver

//TPerInfo Table
#define UID_TPerInfo                (0x0000020100030001)

//Template Table
#define UID_Template                (0x0000020400000000)
#define UID_Template_Base           (0x0000020400000001)
#define UID_Template_Admin          (0x0000020400000002)
#define UID_Template_Locking        (0x0000020400000006)
#define UID_Template_InterfaceControl   (0x0000020400000007)

//SP Table
#define UID_SP                      (0x0000020500000000)
#define UID_SP_Admin                (0x0000020500000001)
#define UID_SP_Locking              (0x0000020500000002)

//UID for LockingSP:
//SPInfo Table
//#define UID_Locking_SPInfo_Locking      (0x0000000200000001)

//SPTemplate Table, ?? check Table 245 and Table 21 ??
//#define UID_Locking_SPTemplate_Base     (0x0000000300000001)
//#define UID_Locking_SPTemplate_Locking  (0x0000000300000002)
//#define UID_Locking_SPTemplate_InterfaceControl (0x0000000300000003)

//Table Table
//#define UID_Table_Table       (0x0000000100000001)
//#define UID_Table_SPInfo      (0x0000000100000002)
//#define UID_Table_SPTemplates (0x0000000100000003)
//#define UID_Table_MethodID    (0x0000000100000006)
//#define UID_Table_AccessControl   (0x0000000100000007)
//#define UID_Table_ACE            (0x0000000100000008)
//#define UID_Table_Authority   (0x0000000100000009)
//#define UID_Table_CPIN        (0x000000010000000B)
#define UID_Table_SecretProtect         (0x000000010000001D)    //Opal2
#define UID_Table_LockingInfo           (0x0000000100000801)
#define UID_Table_Locking               (0x0000000100000802)
#define UID_Table_MBRControl            (0x0000000100000803)
#define UID_Table_MBR                   (0x0000000100000804)
#define UID_Table_K_AES_128             (0x0000000100000805)
#define UID_Table_K_AES_256             (0x0000000100000806)
#define UID_Table_RestrictedCommands    (0x0000000100000C01)

#define UID_Table_DataStore     (0x0000000100001001)
#define UID_Table_DataStore2    (0x0000000100001002)    //FS: DataStore
#define UID_Table_DataStore3    (0x0000000100001003)
#define UID_Table_DataStore4    (0x0000000100001004)
#define UID_Table_DataStore5    (0x0000000100001005)
#define UID_Table_DataStore6    (0x0000000100001006)
#define UID_Table_DataStore7    (0x0000000100001007)
#define UID_Table_DataStore8    (0x0000000100001008)
#define UID_Table_DataStore9    (0x0000000100001009)


//MethodID Table
//#define UID_MethodID_Next     (0x0000000600000008)
//#define UID_MethodID_GetACL   (0x000000060000000D)
#define UID_MethodID_GenKey     (0x0000000600000010)
#define UID_MethodID_RevertSP   (0x0000000600000011)
//#define UID_MethodID_Get      (0x0000000600000016)
//#define UID_MethodID_Set      (0x0000000600000017)
#define UID_MethodID_Reactivate (0x0000000600000801)    //SingleUserMode FS
#define UID_MethodID_Erase      (0x0000000600000803)    //SingleUserMode FS

//AccessControl Table: x

//ACE Table
//#define UID_ACE_Anybody         (0x0000000800000001)
//#define UID_ACE_Admin           (0x0000000800000002)
#define UID_ACE_Anybody_Get_CommonName      (0x0000000800000003)    //Opal2
#define UID_ACE_Admins_Set_CommonName       (0x0000000800000004)    //Opal2
#define UID_ACE_ACE_Get_All                 (0x0000000800038000)
#define UID_ACE_ACE_Set_BooleanExpression   (0x0000000800038001)
#define UID_ACE_Authority_Get_All           (0x0000000800039000)
#define UID_ACE_Authority_Set_Enabled       (0x0000000800039001)
#define UID_ACE_User1_Set_CommonName        (0x0000000800044001)    //Opal2
#define UID_ACE_User2_Set_CommonName        (0x0000000800044002)    //Opal2
#define UID_ACE_User3_Set_CommonName        (0x0000000800044003)    //Opal2
#define UID_ACE_User4_Set_CommonName        (0x0000000800044004)    //Opal2
#define UID_ACE_User5_Set_CommonName        (0x0000000800044005)    //Opal2
#define UID_ACE_User6_Set_CommonName        (0x0000000800044006)    //Opal2
#define UID_ACE_User7_Set_CommonName        (0x0000000800044007)    //Opal2
#define UID_ACE_User8_Set_CommonName        (0x0000000800044008)    //Opal2
#define UID_ACE_User9_Set_CommonName        (0x0000000800044009)    //Opal2

#define UID_ACE_C_PIN_Admins_Get_All_NOPIN  (0x000000080003A000)
#define UID_ACE_C_PIN_Admins_Set_PIN        (0x000000080003A001)
#define UID_ACE_C_PIN_User_Set_PIN          (0x000000080003A800)
#define UID_ACE_C_PIN_User1_Set_PIN         (0x000000080003A801)
#define UID_ACE_C_PIN_User2_Set_PIN         (0x000000080003A802)
#define UID_ACE_C_PIN_User3_Set_PIN         (0x000000080003A803)
#define UID_ACE_C_PIN_User4_Set_PIN         (0x000000080003A804)
#define UID_ACE_C_PIN_User5_Set_PIN         (0x000000080003A805)
#define UID_ACE_C_PIN_User6_Set_PIN         (0x000000080003A806)
#define UID_ACE_C_PIN_User7_Set_PIN         (0x000000080003A807)
#define UID_ACE_C_PIN_User8_Set_PIN         (0x000000080003A808)
#define UID_ACE_C_PIN_User9_Set_PIN         (0x000000080003A809)

#define UID_ACE_K_AES_Mode                      (0x000000080003BFFF)
#define UID_ACE_K_AES_256_GlobalRange_GenKey    (0x000000080003B800)
#define UID_ACE_K_AES_256_Range1_GenKey         (0x000000080003B801)
#define UID_ACE_K_AES_256_Range2_GenKey         (0x000000080003B802)
#define UID_ACE_K_AES_256_Range3_GenKey         (0x000000080003B803)
#define UID_ACE_K_AES_256_Range4_GenKey         (0x000000080003B804)
#define UID_ACE_K_AES_256_Range5_GenKey         (0x000000080003B805)
#define UID_ACE_K_AES_256_Range6_GenKey         (0x000000080003B806)
#define UID_ACE_K_AES_256_Range7_GenKey         (0x000000080003B807)
#define UID_ACE_K_AES_256_Range8_GenKey         (0x000000080003B808)
#define UID_ACE_Locking_GRange_Get_RangeStartToActiveKey   (0x000000080003D000)
#define UID_ACE_Locking_Range1_Get_RangeStartToActiveKey   (0x000000080003D001)
#define UID_ACE_Locking_Range2_Get_RangeStartToActiveKey   (0x000000080003D002)
#define UID_ACE_Locking_Range3_Get_RangeStartToActiveKey   (0x000000080003D003)
#define UID_ACE_Locking_Range4_Get_RangeStartToActiveKey   (0x000000080003D004)
#define UID_ACE_Locking_Range5_Get_RangeStartToActiveKey   (0x000000080003D005)
#define UID_ACE_Locking_Range6_Get_RangeStartToActiveKey   (0x000000080003D006)
#define UID_ACE_Locking_Range7_Get_RangeStartToActiveKey   (0x000000080003D007)
#define UID_ACE_Locking_Range8_Get_RangeStartToActiveKey   (0x000000080003D008)
#define UID_ACE_Locking_GRange_Set_RdLocked         (0x000000080003E000)
#define UID_ACE_Locking_Range1_Set_RdLocked         (0x000000080003E001)
#define UID_ACE_Locking_Range2_Set_RdLocked         (0x000000080003E002)
#define UID_ACE_Locking_Range3_Set_RdLocked         (0x000000080003E003)
#define UID_ACE_Locking_Range4_Set_RdLocked         (0x000000080003E004)
#define UID_ACE_Locking_Range5_Set_RdLocked         (0x000000080003E005)
#define UID_ACE_Locking_Range6_Set_RdLocked         (0x000000080003E006)
#define UID_ACE_Locking_Range7_Set_RdLocked         (0x000000080003E007)
#define UID_ACE_Locking_Range8_Set_RdLocked         (0x000000080003E008)
#define UID_ACE_Locking_GRange_Set_WrLocked         (0x000000080003E800)
#define UID_ACE_Locking_Range1_Set_WrLocked         (0x000000080003E801)
#define UID_ACE_Locking_Range2_Set_WrLocked         (0x000000080003E802)
#define UID_ACE_Locking_Range3_Set_WrLocked         (0x000000080003E803)
#define UID_ACE_Locking_Range4_Set_WrLocked         (0x000000080003E804)
#define UID_ACE_Locking_Range5_Set_WrLocked         (0x000000080003E805)
#define UID_ACE_Locking_Range6_Set_WrLocked         (0x000000080003E806)
#define UID_ACE_Locking_Range7_Set_WrLocked         (0x000000080003E807)
#define UID_ACE_Locking_Range8_Set_WrLocked         (0x000000080003E808)
#define UID_ACE_Locking_GlblRng_Admins_Set          (0x000000080003F000)
#define UID_ACE_Locking_Admins_RangeStartToLocked   (0x000000080003F001)
#define UID_ACE_MBRControl_Admins_Set               (0x000000080003F800)
#define UID_ACE_MBRControl_Set_Done                 (0x000000080003F801)
#define UID_ACE_DataStore_Get_All                   (0x000000080003FC00)
#define UID_ACE_DataStore_Set_All                   (0x000000080003FC01)
#define UID_ACE_DataStore2_Get_All                  (0x000000080003FC02)    //FS_DataStore
#define UID_ACE_DataStore2_Set_All                  (0x000000080003FC03)    //FS_DataStore
#define UID_ACE_DataStore3_Get_All                  (0x000000080003FC04)    //FS_DataStore
#define UID_ACE_DataStore3_Set_All                  (0x000000080003FC05)    //FS_DataStore
#define UID_ACE_DataStore4_Get_All                  (0x000000080003FC06)    //FS_DataStore
#define UID_ACE_DataStore4_Set_All                  (0x000000080003FC07)    //FS_DataStore
#define UID_ACE_DataStore5_Get_All                  (0x000000080003FC08)    //FS_DataStore
#define UID_ACE_DataStore5_Set_All                  (0x000000080003FC09)    //FS_DataStore
#define UID_ACE_DataStore6_Get_All                  (0x000000080003FC0A)    //FS_DataStore
#define UID_ACE_DataStore6_Set_All                  (0x000000080003FC0B)    //FS_DataStore
#define UID_ACE_DataStore7_Get_All                  (0x000000080003FC0C)    //FS_DataStore
#define UID_ACE_DataStore7_Set_All                  (0x000000080003FC0D)    //FS_DataStore
#define UID_ACE_DataStore8_Get_All                  (0x000000080003FC0E)    //FS_DataStore
#define UID_ACE_DataStore8_Set_All                  (0x000000080003FC0F)    //FS_DataStore
#define UID_ACE_DataStore9_Get_All                  (0x000000080003FC10)    //FS_DataStore
#define UID_ACE_DataStore9_Set_All                  (0x000000080003FC11)    //FS_DataStore

#define UID_ACE_Locking_GRange_Set_ReadToLOR        (0x0000000800040000)    //FS: SingleUserMode
#define UID_ACE_Locking_Range1_Set_ReadToLOR        (0x0000000800040001)    //FS: SingleUserMode
#define UID_ACE_Locking_Range2_Set_ReadToLOR        (0x0000000800040002)    //FS: SingleUserMode
#define UID_ACE_Locking_Range3_Set_ReadToLOR        (0x0000000800040003)    //FS: SingleUserMode
#define UID_ACE_Locking_Range4_Set_ReadToLOR        (0x0000000800040004)    //FS: SingleUserMode
#define UID_ACE_Locking_Range5_Set_ReadToLOR        (0x0000000800040005)    //FS: SingleUserMode
#define UID_ACE_Locking_Range6_Set_ReadToLOR        (0x0000000800040006)    //FS: SingleUserMode
#define UID_ACE_Locking_Range7_Set_ReadToLOR        (0x0000000800040007)    //FS: SingleUserMode
#define UID_ACE_Locking_Range8_Set_ReadToLOR        (0x0000000800040008)    //FS: SingleUserMode

#define UID_ACE_Locking_Range1_Set_Range            (0x0000000800041001)    //FS: SingleUserMode
#define UID_ACE_Locking_Range2_Set_Range            (0x0000000800041002)    //FS: SingleUserMode
#define UID_ACE_Locking_Range3_Set_Range            (0x0000000800041003)    //FS: SingleUserMode
#define UID_ACE_Locking_Range4_Set_Range            (0x0000000800041004)    //FS: SingleUserMode
#define UID_ACE_Locking_Range5_Set_Range            (0x0000000800041005)    //FS: SingleUserMode
#define UID_ACE_Locking_Range6_Set_Range            (0x0000000800041006)    //FS: SingleUserMode
#define UID_ACE_Locking_Range7_Set_Range            (0x0000000800041007)    //FS: SingleUserMode
#define UID_ACE_Locking_Range8_Set_Range            (0x0000000800041008)    //FS: SingleUserMode

#define UID_ACE_CPIN_Anybody_Get_NoPIN              (0x0000000800042000)    //FS: SingleUserMode
#define UID_ACE_SP_Reactivate_Admin                 (0x0000000800042001)    //FS: SingleUserMode

#define UID_ACE_Locking_GRange_Erase                (0x0000000800043000)    //FS: SingleUserMode
#define UID_ACE_Locking_Range1_Erase                (0x0000000800043001)    //FS: SingleUserMode
#define UID_ACE_Locking_Range2_Erase                (0x0000000800043002)    //FS: SingleUserMode
#define UID_ACE_Locking_Range3_Erase                (0x0000000800043003)    //FS: SingleUserMode
#define UID_ACE_Locking_Range4_Erase                (0x0000000800043004)    //FS: SingleUserMode
#define UID_ACE_Locking_Range5_Erase                (0x0000000800043005)    //FS: SingleUserMode
#define UID_ACE_Locking_Range6_Erase                (0x0000000800043006)    //FS: SingleUserMode
#define UID_ACE_Locking_Range7_Erase                (0x0000000800043007)    //FS: SingleUserMode
#define UID_ACE_Locking_Range8_Erase                (0x0000000800043008)    //FS: SingleUserMode


//Authority Table
//#define UID_Authority_Anybody (0x0000000900000001)
//#define UID_Authority_Admins  (0x0000000900000002)
#define UID_Authority_AdminX    (0x0000000900010000)
#define UID_Authority_Admin1    (0x0000000900010001)
#define UID_Authority_Admin2    (0x0000000900010002)
#define UID_Authority_Admin3    (0x0000000900010003)
#define UID_Authority_Admin4    (0x0000000900010004)
#define UID_Authority_Users     (0x0000000900030000)
#define UID_Authority_User1     (0x0000000900030001)
#define UID_Authority_User2     (0x0000000900030002)
#define UID_Authority_User3     (0x0000000900030003)
#define UID_Authority_User4     (0x0000000900030004)
#define UID_Authority_User5     (0x0000000900030005)
#define UID_Authority_User6     (0x0000000900030006)
#define UID_Authority_User7     (0x0000000900030007)
#define UID_Authority_User8     (0x0000000900030008)
#define UID_Authority_User9     (0x0000000900030009)

//C_PIN Table
#define UID_CPIN_Admin1         (0x0000000B00010001)
#define UID_CPIN_Admin2         (0x0000000B00010002)
#define UID_CPIN_Admin3         (0x0000000B00010003)
#define UID_CPIN_Admin4         (0x0000000B00010004)
#define UID_CPIN_User1          (0x0000000B00030001)
#define UID_CPIN_User2          (0x0000000B00030002)
#define UID_CPIN_User3          (0x0000000B00030003)
#define UID_CPIN_User4          (0x0000000B00030004)
#define UID_CPIN_User5          (0x0000000B00030005)
#define UID_CPIN_User6          (0x0000000B00030006)
#define UID_CPIN_User7          (0x0000000B00030007)
#define UID_CPIN_User8          (0x0000000B00030008)
#define UID_CPIN_User9          (0x0000000B00030009)

//SecretProtect
#define UID_SecretProtect       (0x0000001D00000000)
#define UID_SecretProtect_128   (0x0000001D0000001D)    //Opal2
#define UID_SecretProtect_256   (0x0000001D0000001E)    //Opal2

//LockingInfo Table
#define UID_LockingInfo         (0x0000080100000001)

//Locking Table
#define UID_Locking             (0x0000080200000000)
#define UID_Locking_GRange      (0x0000080200000001)
#define UID_Locking_Range1      (0x0000080200030001)
#define UID_Locking_Range2      (0x0000080200030002)
#define UID_Locking_Range3      (0x0000080200030003)
#define UID_Locking_Range4      (0x0000080200030004)
#define UID_Locking_Range5      (0x0000080200030005)
#define UID_Locking_Range6      (0x0000080200030006)
#define UID_Locking_Range7      (0x0000080200030007)
#define UID_Locking_Range8      (0x0000080200030008)
#define UID_Locking_Range       (0x0000080200030000)

//MBRControl Table
#define UID_MBRControl          (0x0000080300000001)

#define UID_MBR                 (0x0000080400000000)

//K_AES_256 Table
#define UID_K_AES_256               (0x0000000100000806)    //(0x0000080600000000)
#define UID_K_AES_256_GRange_Key    (0x0000080600000001)
#define UID_K_AES_256_Range1_Key    (0x0000080600030001)
#define UID_K_AES_256_Range2_Key    (0x0000080600030002)
#define UID_K_AES_256_Range3_Key    (0x0000080600030003)
#define UID_K_AES_256_Range4_Key    (0x0000080600030004)
#define UID_K_AES_256_Range5_Key    (0x0000080600030005)
#define UID_K_AES_256_Range6_Key    (0x0000080600030006)
#define UID_K_AES_256_Range7_Key    (0x0000080600030007)
#define UID_K_AES_256_Range8_Key    (0x0000080600030008)

#define UID_DataStoreType       (0x0000100000000000)
#define UID_DataStore           (0x0000100100000000)
#define UID_DataStore2          (0x0000100200000000)    //FS: DataStore
#define UID_DataStore3          (0x0000100300000000)    //FS: DataStore
#define UID_DataStore4          (0x0000100400000000)    //FS: DataStore
#define UID_DataStore5          (0x0000100500000000)    //FS: DataStore
#define UID_DataStore6          (0x0000100600000000)    //FS: DataStore
#define UID_DataStore7          (0x0000100700000000)    //FS: DataStore
#define UID_DataStore8          (0x0000100800000000)    //FS: DataStore
#define UID_DataStore9          (0x0000100900000000)    //FS: DataStore
//#define UID_DSx                 (0x00FF000000000000)
//#define UID_DS2                 (0x00F1000000000000)
//#define UID_DS3                 (0x00F2000000000000)
//#define UID_DS4                 (0x00F3000000000000)
//#define UID_DS5                 (0x00F4000000000000)
//#define UID_DS6                 (0x00F5000000000000)
//#define UID_DS7                 (0x00F6000000000000)
//#define UID_DS8                 (0x00F7000000000000)
//#define UID_DS9                 (0x00F8000000000000)

//#define UID_SUR0                (0xF000000000000000)    //FS: SingleUserMode
//#define UID_SUR1                (0xF100000000000000)    //FS: SingleUserMode
//#define UID_SUR2                (0xF200000000000000)    //FS: SingleUserMode
//#define UID_SUR3                (0xF100000000000000)    //FS: SingleUserMode
//#define UID_SUR4                (0xF100000000000000)    //FS: SingleUserMode
//#define UID_SUR5                (0xF100000000000000)    //FS: SingleUserMode
//#define UID_SUR6                (0xF100000000000000)    //FS: SingleUserMode
//#define UID_SUR7                (0xF100000000000000)    //FS: SingleUserMode
//#define UID_SUR8                (0xF100000000000000)    //FS: SingleUserMode
#define UID_FF                  (0xFFFFFFFF00000000)

//Token Definition
#define TOK_StartList           0xF0
#define TOK_EndList             0xF1
#define TOK_StartName           0xF2
#define TOK_EndName             0xF3
#define TOK_Call                0xF8
#define TOK_EndOfData           0xF9
#define TOK_EndOfSession        0xFA
#define TOK_StartTransaction    0xFB
#define TOK_EndTransaction      0xFC
#define TOK_Empty               0xFF

//Method Status Code Definition
#define STS_SUCCESS                     0x00
#define STS_NOT_AUTHORIZED              0x01
//#define STS_OBSOLETE                       0x02
#define STS_SP_BUSY                     0x03
#define STS_SP_FAILED                   0x04
#define STS_SP_DISABLED                 0x05
#define STS_SP_FROZEN                   0x06
#define STS_NO_SESSIONS_AVAILABLE       0x07
#define STS_UNIQUENESS_CONFLICT         0x08
#define STS_INSUFFICIENT_SPACE          0x09
#define STS_INSUFFICIENT_ROWS           0x0A
#define STS_INVALID_METHOD              0x0B
#define STS_INVALID_PARAMETER           0x0C
//0x0D
//0x0E
#define STS_TPER_MALFUNCTION            0x0F
#define STS_TRANSACTION_FAILURE         0x10
#define STS_RESPONSE_OVERFLOW           0x11
#define STS_AUTHORITY_LOCKED_OUT        0x12
#define STS_FAIL                        0x3F

#define STS_SUCCESS_THEN_ABORT          0xE0FF    //internal use only, status is success but need to close session (RevertSP)
#define STS_STAY_IN_IF_SEND             0xF0FF    //internal use only, it should keep gTcgCmdState at "ST_AWAIT_IF_SEND"
#define STS_SESSION_ABORT               0xFFFF    //internal use only
#define STS_1667_NO_DATA_RETURN         0xF8FF

//#ifdef TCG_EDRIVE
#define TCG_1667_ProbeCmd               0x0001  //Index (HiByte) + Cmd (LoByte)
#define TCG_1667_GetSiloCap             0x0101
#define TCG_1667_Transfer               0x0102
#define TCG_1667_Reset                  0x0103
#define TCG_1667_GetResult              0x0104
#define TCG_1667_TPerReset              0x0105

#define STS_1667_SUCCESS                0x00
#define STS_1667_FAILURE                0x80
#define STS_1667_INV_PARAMETER_Combi    0xF7
#define STS_1667_INV_PARAMETER_LENGTH   0xF8
#define STS_1667_INCONSISTENT_PCLENGTH  0xF9
#define STS_1667_INV_SILO               0xFA
#define STS_1667_INCOMPLETE_COMMAND     0xFB
#define STS_1667_INV_PARAMETER          0xFC
#define STS_1667_SP_SEQUENCE_REJECTION  0xFD
#define STS_1667_NO_PROBE               0xFE
#define STS_1667_RESERVED_FUNCTION      0xFF

#define STS_1667_DEFAULT_BEHAVIOR       0x01    //Probe Silo
#define STS_1667_UNSUPPORTED_HOST_VER   0x81

#define STS_1667_INV_TX_LEN_ON_POUT     0x81    //TCG Silo
#define STS_1667_INV_TCG_COMID          0x82
#define STS_1667_TCG_SYNC_VIOLATION     0x83

//error code for TCG SIIS
#define STS_TPER_SUCCESS                    STS_1667_SUCCESS
#define STS_TPER_INVALID_SEC_PID_PARAM      STS_1667_INV_TCG_COMID
#define STS_TPER_INVALID_TX_PARAM_SEND      STS_1667_INV_TX_LEN_ON_POUT
#define STS_TPER_OTHER_INVALID_CMD_PARAM    STS_1667_INV_TCG_COMID
#define STS_TPER_SYNC_PROTOCOL_VIOLATION    STS_1667_TCG_SYNC_VIOLATION
#define STS_TPER_DATA_PROTECTION_ERROR      //
#define STS_TPER_INVALID_SEC_STATE          //
#define STS_TPER_ACCESS_DENIED              //

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define LoByte(i) *((U8*)&i)
#define HiByte(i) *(((U8*)&i)+1)
#define LoWord(i) *((U16*)&i)
#define HiWord(i) *(((U16*)&i)+1)
#define LoLong(i) *((U32*)&i)
#define HiLong(i) *(((U32*)&i)+1)
#define mBIT(n)   (1 << (n))

#define tcgEepProgIdx       (smSysInfo->d.TcgData.d.TCGUsed.TCG_EEPROM_PROG_IDX)
#define tcgDevTyp           (smSysInfo->d.TcgData.d.TCGUsed.TCG_DEV_TYPE)

#define tcgG4Dft            (smSysInfo->d.TcgData.d.TCGUsed.TcgG4NorCellDft)
#define tcgG5Dft            (smSysInfo->d.TcgData.d.TCGUsed.TcgG5NorCellDft)
#define tcgG4EraCnt         (smSysInfo->d.TcgData.d.TCGUsed.TcgG4NorBlkErasedCnt)
#define tcgG5EraCnt         (smSysInfo->d.TcgData.d.TCGUsed.TcgG5NorBlkErasedCnt)
#define tcgDefectID         (smSysInfo->d.TcgData.d.TCGUsed.TcgDefect_ID)
#define tcgErasedCntID      (smSysInfo->d.TcgData.d.TCGUsed.TcgErasedCnt_ID)

#define tcg_mTbl_idStr      (smSysInfo->d.TcgData.d.TCGUsed.Tcg_m_ID_Text)
#define tcg_mTbl_id         (smSysInfo->d.TcgData.d.TCGUsed.m_ID)
#define tcg_mTbl_verStr     (smSysInfo->d.TcgData.d.TCGUsed.Tcg_m_Ver_Text)
#define tcg_mTbl_ver        (smSysInfo->d.TcgData.d.TCGUsed.m_Ver)
#define tcg_cTbl_idStr      (smSysInfo->d.TcgData.d.TCGUsed.Tcg_c_ID_Text)
#define tcg_cTbl_id         (smSysInfo->d.TcgData.d.TCGUsed.c_ID)
#define tcg_cTbl_verStr     (smSysInfo->d.TcgData.d.TCGUsed.Tcg_c_Ver_Text)
#define tcg_cTbl_ver        (smSysInfo->d.TcgData.d.TCGUsed.c_Ver)
#define tcg_prefmt_tag      (smSysInfo->d.TcgData.d.TCGUsed.Preformat_ID)

#define PREFORMAT_STRING    "preformat"
#define ERASE_ALL_TAG       "erase all"
#define PREFORMAT_START_TAG "preformat start"
#define PREFORMAT_END_TAG   "preformat end  "
#define DEFECT_STRING       "DftTbl_03"
#define ERASED_CNT_STRING   "EraCntTbl_03"

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
#if _NAND_ == NAND_3DTLCG2

    #define TCG_LAA_INIT_VAL U32_MAX
    
    typedef union
    {
        U32    all;
        struct
        {
            U32 page   : 10;
            U32 cell   : 8;
            U32 rsvd   : 14;
        }pc;
    }tTcgLogAddr;
    
#elif _NAND_ == NAND_MLC

    #define TCG_LAA_INIT_VAL U16_MAX

    typedef union
    {
        U16    all;
        struct
        {
            U16 page   : 8;
            U16 cell   : 8;
        }pc;
    }tTcgLogAddr;

#endif


typedef struct
{
    U8      dev;
    U8      ch;
    U8      il;
    U8      page;
    U8      row;
}tTcgAdrForm;

typedef enum{
    tIDLE,
    tREAD,
    tWRITE,
    tERASE
}sTcgNvOpType;

typedef enum{
    zGRP1,
    zGRP2,
    zGRP3,
    zGRP4,
    zDATASTORE,
    zMBR,
    zGRP5,
    zMBRTMP,
    zDATASTORETMP
}sTcgGroupZone;

typedef struct
{
    sTcgNvOpType    NvOpType;
    sTcgGroupZone   NvOpGrp;
    U32             NvOpLaaStartIdx;
    U16             NvOpPageCnt;
    U8*             NvOpBufAddr;
    U32             NvOpSeqRdIndex;
    U32             NvOpBaseBlkNo;
    U8              NvOpMultiPageRead;
    U32             NvOpSeqWrIndex;
}sTcg2RWtaskInfo;

//Session Manager param
typedef enum
{
    SESSION_CLOSE,
    SESSION_START,
    SESSION_NG,
    SESSION_NAN = 0x7FFFFFFF
}tSESSION_STATE;

typedef enum
{
    TRNSCTN_ACTIVE,
    TRNSCTN_IDLE,
    TRNSCTN_NAN = 0x7FFFFFFF
}tTRNSCTN_STATE;

typedef enum
{
    ATOM_HALFUID,
    ATOM_UID,
    ATOM_BYTES,
    ATOM_INT,
    ATOM_UNKNOW,
    ATOM_NAN = 0x7FFFFFFF
}tATOM_DEC_TYPE;

//========================================================
// TCG Table Structure Definition and Declaration
//========================================================
#if _TCG_ != TCG_PYRITE
  #define ADM_ACCESSCTRL_TBLOBJ_CNT     (63 + 5)  //include PSID
  #define ADM_ACE_TBLOBJ_CNT            (11)
  #define ADM_AUTHORITY_TBLOBJ_CNT      (5 + 1)
  #define ADM_CPIN_TBLOBJ_CNT           (3 + 1)
  #define SSC_STRING                    "Opal\0"    //For TPerInfoTbl.SSC
#else
  #define ADM_ACCESSCTRL_TBLOBJ_CNT     (63 + 5 - 9)  //include PSID
  #define ADM_ACE_TBLOBJ_CNT            (11 - 3)
  #define ADM_AUTHORITY_TBLOBJ_CNT      (3 + 1)
  #define ADM_CPIN_TBLOBJ_CNT           (2 + 1)
  #define SSC_STRING                    "Pyrite\0"
#endif

//typedef enum  {READ_ONLY, WRITABLE, NO_DEF} eTblRW;      //Cell Property

//Column Type:
//      VALUE_TYPE: uinteger (get: value)
//      FBYTE_TYPE: byte sequence with fixed length (column size)  (get: Ax b1 ...)
//      VBYTE_TYPE: byte sequence with variable length, first byte is the length (Ax b1...)
//      LIST_TYPE: first byte is the length... (get: F0 b1 ... F1)
//      STRING_TYPE: string in char array (get: Ax ...)
//      STRINGLIST_TYPE: string list in char array, ending with double '/0' (get: F0 Ax ... F1)
typedef enum  {
    VALUE_TYPE,
    FBYTE_TYPE,
    VBYTE_TYPE,
    UID_TYPE,
    LIST_TYPE,
    STRING_TYPE,
    STRINGLIST_TYPE,
    UIDLIST_TYPE,
    UID2_TYPE
}eColType;

#pragma pack(1)
//Column Property
typedef struct
{
    U8    colNo;    //column number
    U8    size;     //column byte count
    U8    colType;  //colType
    U8    dummy;
}sColPrty;
#pragma pack()

/* AdminSP Table Declaration */
#pragma pack(1)
typedef struct
{
    U32 ID;
    U32 ver;
}sTcgTblInfo;
#pragma pack()

//--------------------------------------------------------
// SPInfo Table prototype:
//--------------------------------------------------------
#pragma pack(1)
typedef struct
{
    U64 uid;       //col#0, UID
    U64 spid;
    U8  name[8];
    //U8 size;
    //U8 sizeInUse;
    //U32 spSessionTimeout;
    U32 enabled;   //bool
}sSPInfo_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct
{
    sSPInfo_TblObj val[1];     //row obj
    sColPrty    pty[4];     //column property (# and bytes)
    U8 colCnt;
    U8 rowCnt;
    U8 objSize;
    U8 maxCol;          //maximum column number in TCG spec
}sSPInfo_Tbl;
#pragma pack()

//--------------------------------------------------------
// SPTemplates Table prototype:
//--------------------------------------------------------
#pragma pack(1)
typedef struct
{
    U64 uid;           //col#0, UID
    U64 templateId;
    U8  name[8];
    U8  version[4];
}sSPTemplates_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct
{
    sSPTemplates_TblObj val[2];     //row obj
    sColPrty    pty[4];             //column property (# and bytes)
    U8 colCnt;
    U8 rowCnt;
    U8 objSize;
    U8 maxCol;                      //maximum column number in TCG spec
}sSPTemplates_Tbl;
#pragma pack()

//--------------------------------------------------------
// Table Table prototype:
//--------------------------------------------------------
#define TBL_K_OBJECT 1
#define TBL_K_BYTE   2

#pragma pack(1)
    typedef struct
    {
        U64         uid;            //col#0, UID
        U8          name[16];
        U16         kind;           //col#4, Kind
        U8          mGranularity;   //col#13, manGranularity    //maxSize;
        U8          rGranularity;   //col#14
    }sTbl_TblObj;
#pragma pack()

#pragma pack(1)
    typedef struct
    {
        sTbl_TblObj val[11];  //row obj
        sColPrty    pty[5];   //column property (# and bytes)
        U8          colCnt;   //=3;
        U8          rowCnt;   //=11;
        U8          objSize;  //table object size
        U8          maxCol;   //maximum column number in TCG spec
    }sTbl_Tbl;
#pragma pack()

//--------------------------------------------------------
// MethodID Table prototype, Read-Only
//--------------------------------------------------------
#pragma pack(1)
typedef struct
{
    U64 uid;       //col#1, InvokingID
    U8 name[12];   //col#2, MethodID
}sMethod_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct
{
    sMethod_TblObj val[8];  //row obj
    sColPrty       pty[2];  //column property (# and bytes)
    U8 colCnt;              //=2;
    U8 rowCnt;              //=6;
    U8 objSize;             //table object size
    U8 maxCol;              //maximum column number in TCG spec
}sMethod_Tbl;
#pragma pack()

//--------------------------------------------------------
// AccessControl Table prototype, Read-Only
//--------------------------------------------------------
#define ADM_ACCESSCTRL_ACL_CNT    3
#pragma pack(1)
typedef struct
{
    U64 invID;                          //col#1, InvokingID
    U64 mtdID;                          //col#2, MethodID
    U64 acl[ADM_ACCESSCTRL_ACL_CNT];    //col#4, it should be a ACL "UIDList_Type"
    U64 getAclAcl;                      //col#8, GetACLACL
}sAccessCtrl_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct
{
    sAccessCtrl_TblObj val[ADM_ACCESSCTRL_TBLOBJ_CNT];
    sColPrty    pty[4];             //column property (# and bytes)
    U8 colCnt;                      //=4;
    U8 rowCnt;                      //=4;
    U8 objSize;                     //table object size
    U8 maxCol;                      //maximum column number in TCG spec
}sAccessCtrl_Tbl;
#pragma pack()

//--------------------------------------------------------
// ACE Table prototype:
//--------------------------------------------------------
#define ADM_ACE_BOOLEXPR_CNT    2
#pragma pack(1)
typedef struct {
    U64 uid;
    U64 booleanExpr[ADM_ACE_BOOLEXPR_CNT];  //5.1.3.3 ACE_expression,  uidref to an Authority object + boolean_ACE (AND = 0 and OR = 1)
                                            //for Opal drive, boolean_ACE should be "OR" only, so we only need to store lower 4B uid of an Authority object
                                            //UID List, Admins OR SID
    U8 col[12];                             //VBYTE_TYPE, ?? how to deal with?  bit# stands for col#?
}sACE_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct {
    sACE_TblObj val[ADM_ACE_TBLOBJ_CNT]; //row cnt
    sColPrty pty[3];        //column cnt
    U8 colCnt;
    U8 rowCnt;
    U8 objSize;             //table object size
    U8 maxCol;              //maximum column number in TCG spec
}sACE_Tbl;
#pragma pack()

//--------------------------------------------------------
// Authority Table prototype:
//--------------------------------------------------------
typedef enum  {AUTH_None=0, AUTH_Password, AUTH_Exchange, AUTH_Sign, AUTH_SymK, AUTh_NAN=0x7fffffff} auth_method;
typedef enum  {SECURE_None=0, HMAC_SHA_256, SECURE_NAN=0x7fffffff} secure_message ;
typedef enum  {HASH_None=0, SHA_1, SHA_256, SHA_384, SHA_512, HASH_NAN=0x7fffffff} hash_protocol;
typedef enum  {GRP_1=1, GRP_2=2, GRP_3=3, GRP_ALL=0xff} grp_define;

#pragma pack(1)
typedef struct {
    U64     uid;
    U8      name[8];
    U32     isClass;     //bool
    U64     Class;
    U32     enabled;   //bool
    secure_message  secure;
    hash_protocol   HashAndSign;
    U32     presentCertificate;    //bool
    auth_method operation;
    U64     credential;
    U64     responseSign;
    U64     responseExch;
}sAuthority_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct {
    sAuthority_TblObj val[ADM_AUTHORITY_TBLOBJ_CNT];     //row cnt  , include PSID
    sColPrty pty[12];       //column cnt
    U8 colCnt;
    U8 rowCnt;
    U8 objSize;       //table object size
    U8 maxCol;          //maximum column number in TCG spec
}sAuthority_Tbl;
#pragma pack()

//--------------------------------------------------------
// CPIN Table prototype:
//--------------------------------------------------------
#define PWD_LENGTH  36 //33
#define CPIN_LENGTH 32
#pragma pack(1)
typedef struct {
    U64 uid;
    U8  name[12];
    U8  pwd[PWD_LENGTH];  //33];
    U64 charSet;
    U32 tryLimit;
    U32 tries;
    U8  presistence; //bool
    U8  dummy0;
    U8  dummy1;
    U8  dummy2;
}sCPin_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct {
    sCPin_TblObj val[ADM_CPIN_TBLOBJ_CNT];  //include PSID
    sColPrty pty[7];        //column cnt
    U8 colCnt;
    U8 rowCnt;
    U8 objSize;             //table object size
    U8 maxCol;              //maximum column number in TCG spec
}sCPin_Tbl;
#pragma pack()

//--------------------------------------------------------
// TPerInfo Table prototype:
//--------------------------------------------------------
#pragma pack(1)
typedef struct {
    U64 uid;
    U32 firmwareVersion;
    U32 protocolVersion;
    U8  ssc[8];
    U32 preset;
}sTPerInfo_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct {
    sTPerInfo_TblObj val[1];
    sColPrty    pty[5];    //column cnt
    U8      colCnt;
    U8      rowCnt;
    U8      objSize;       //table object size
    U8      maxCol;        //maximum column number in TCG spec
}sTPerInfo_Tbl;
#pragma pack()

//--------------------------------------------------------
// Template Table prototype:
//--------------------------------------------------------
#pragma pack(1)
typedef struct {
    U64  uid;
    U8    name[12];
    U32    revision;
    U32    instances;
    U32    maxInstances;
}sTemplate_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct {
    sTemplate_TblObj val[3];
    sColPrty pty[5];    //column cnt
    U8 colCnt;
    U8 rowCnt;
    U8 objSize;         //table object size
    U8 maxCol;          //maximum column number in TCG spec
}sTemplate_Tbl;
#pragma pack()

//--------------------------------------------------------
// SP Table prototype:
//--------------------------------------------------------
typedef enum
{
    issued=0,
    manufactured_inactive=8,
    manufactured=9,
    //manufactured_disabled=10
    LIFE_CYCLE_NAN = 0x7FFFFFFF
}life_cycle_state;

#pragma pack(1)
typedef struct {
    U64   uid;
    U8    name[8];
    life_cycle_state lifeCycle;
    U32  frozen;    //bool
}sSP_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct {
    sSP_TblObj val[2];
    sColPrty pty[4];    //column cnt
    U8 colCnt;
    U8 rowCnt;
    U8 objSize;         //table object size
    U8 maxCol;          //maximum column number in TCG spec
}sSP_Tbl;
#pragma pack()

/* LockingSP Table Declaration */
#if _TCG_ != TCG_PYRITE
  #define LCK_TABLE_TBLOBJ_CNT          (15+DSTBL_MAX_NUM-1)
  #define LCK_METHOD_TBLOBJ_CNT         10
  #define LCK_ACCESSCTRL_TBLOBJ_CNT     (258+((DSTBL_MAX_NUM-1)*7)+65)
  #define LCK_ACE_TBLOBJ_CNT            (69+((DSTBL_MAX_NUM-1)*2)+31)
#else
  #define LCK_TABLE_TBLOBJ_CNT          13
  #define LCK_METHOD_TBLOBJ_CNT         (10-3)
  #define LCK_ACCESSCTRL_TBLOBJ_CNT     84
  #define LCK_ACE_TBLOBJ_CNT            18
#endif
#define LCK_AUTHORITY_TBLOBJ_CNT        (TCG_AdminCnt+TCG_UserCnt+3)  // 16 or 6
#define LCK_CPIN_TBLOBJ_CNT             (TCG_AdminCnt+TCG_UserCnt)    // 13 or 3

//--------------------------------------------------------
// SPInfo Table prototype:
//--------------------------------------------------------
//const sSPInfo_Tbl cLckSPInfo_Tbl;

//--------------------------------------------------------
// SPTemplates Table prototype:
//--------------------------------------------------------
//const sSPTemplates_Tbl cLckSPTemplates_Tbl;

//--------------------------------------------------------
// Table Table prototype, Read-Only
//--------------------------------------------------------
#pragma pack(1)
    typedef struct
    {
        U64    uid;            //col#0, UID
        U8      name[16];
        U32    kind;           //col#4, Kind
        U32    rows;           //col#7, Rows
        U16    mGranularity;   //col#13, manGranularity    //maxSize;
        U16    rGranularity;   //col#14
    } sLckTbl_TblObj;
#pragma pack()

#pragma pack(1)
    typedef struct
    {
        sLckTbl_TblObj val[LCK_TABLE_TBLOBJ_CNT];     //row obj
        sColPrty    pty[6];     //column property (# and bytes)
        U8      colCnt;         //=3;
        U8      rowCnt;         //=11;
        U8      objSize;        //table object size
        U8      maxCol;         //maximum column number in TCG spec
    } sLckTbl_Tbl;
#pragma pack()

//--------------------------------------------------------
// MethodID Table prototype, Read-Only
//--------------------------------------------------------
#pragma pack(1)
typedef struct
{
    sMethod_TblObj val[LCK_METHOD_TBLOBJ_CNT];          //row obj
    sColPrty    pty[2];         //column property (# and bytes)
    U8 colCnt;                  //=2;
    U8 rowCnt;                  //=6;
    U8 objSize;                 //table object size
    U8 maxCol;                  //maximum column number in TCG spec
}sLckMethod_Tbl;
#pragma pack()

//--------------------------------------------------------
// AccessControl Table prototype, Read-Only
//--------------------------------------------------------
#define LCK_ACCESSCTRL_ACL_CNT    4
#pragma pack(1)
typedef struct
{
    U64 invID;                 //col#1, InvokingID
    U64 mtdID;                 //col#2, MethodID
    U64 acl[LCK_ACCESSCTRL_ACL_CNT];      //col#4, it should be a ACL "UIDList_Type"
    U64 getAclAcl;             //col#8, GetACLACL
}sLckAccessCtrl_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct
{
    sLckAccessCtrl_TblObj val[LCK_ACCESSCTRL_TBLOBJ_CNT]; //row obj
    sColPrty    pty[4];             //column property (# and bytes)
    U16    colCnt;
    U16    rowCnt;
    U16    objSize;            //table object size
    U16    maxCol;             //maximum column number in TCG spec
}sLckAccessCtrl_Tbl;
#pragma pack()

//--------------------------------------------------------
// ACE Table prototype:
//--------------------------------------------------------
#define LCK_ACE_BOOLEXPR_CNT    14  //(TCG_AdminCnt+TCG_UserCnt)  //
#pragma pack(1)
typedef struct {
    U64 uid;
    U64 booleanExpr[LCK_ACE_BOOLEXPR_CNT]; //UID List,  AdminX (4) + UserY (8) + Admins + Users
    U8 col[12];          //VBYTE_TYPE, ?? how to deal with?  bit# stands for col#?  //size 9->12
}sLckACE_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct {
    sLckACE_TblObj val[LCK_ACE_TBLOBJ_CNT];  //row cnt  +25
    sColPrty pty[3];            //column cnt
    U8  colCnt;
    U8 rowCnt;
    U8 objSize;             //table object size
    U8 maxCol;              //maximum column number in TCG spec
}sLckACE_Tbl;
#pragma pack()

//--------------------------------------------------------
// Authority Table prototype:
//--------------------------------------------------------
#pragma pack(1)
    typedef struct {
        U64     uid;
        U8      name[8];
        U8      commonName[32];         //Opal2
        U32     isClass;                //bool
        U64     Class;
        U32     enabled;                //bool
        secure_message  secure;
        hash_protocol   HashAndSign;
        U32     presentCertificate;     //bool
        auth_method operation;
        U64     credential;
        U64     responseSign;
        U64     responseExch;
    } sLckAuthority_TblObj;
#pragma pack()

#pragma pack(1)
    typedef struct {
        sLckAuthority_TblObj val[LCK_AUTHORITY_TBLOBJ_CNT];       //row cnt
        sColPrty    pty[13];            //column cnt
        U8      colCnt;
        U8      rowCnt;
        U8      objSize;                //table object size
        U8      maxCol;                 //maximum column number in TCG spec
    } sLckAuthority_Tbl;
#pragma pack()

//--------------------------------------------------------
// CPIN Table prototype:
//--------------------------------------------------------
#pragma pack(1)
typedef struct {
    sCPin_TblObj val[LCK_CPIN_TBLOBJ_CNT];
    sColPrty    pty[7];             //column cnt
    U8      colCnt;
    U8      rowCnt;
    U8      objSize;            //table object size
    U8      maxCol;             //maximum column number in TCG spec
}sLckCPin_Tbl;
#pragma pack()

//--------------------------------------------------------
// SecretProtect Table prototype:
//--------------------------------------------------------
#pragma pack(1)
typedef struct {
    U64     uid;
    U64     table;
    U8      colNumber;
    U8      protectMechanism[3];
}sSecretProtect_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct {
    sSecretProtect_TblObj    val[1];
    sColPrty        pty[4];                 //column cnt
    U8          colCnt;
    U8          rowCnt;
    U8          objSize;                //table object size
    U8          maxCol;                 //maximum column number in TCG spec
}sSecretProtect_Tbl;
#pragma pack()

//--------------------------------------------------------
// LockingInfo Table prototype:
//--------------------------------------------------------
#pragma pack(1)
typedef struct {
    U64     uid;
    U8      name[8];
    U8      version;
    U8      encryptSupport;
    U8      maxRanges;
    U8      alignentReuired;
    U16     logicalBlockSize;
    U8      alignmentGranularity;
    U8      lowestAlignedLBA;
    U64     singleUserModeRange[LOCKING_RANGE_CNT+1];   //Range N <-> User N+1
    U32     rangeStartLengthPolicy;
}sLockingInfo_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct {
    sLockingInfo_TblObj val[1];
    sColPrty    pty[11];            //column cnt
    U8      colCnt;
    U8      rowCnt;
    U8      objSize;            //table object size
    U8      maxCol;             //maximum column number in TCG spec
}sLockingInfo_Tbl;
#pragma pack()

//--------------------------------------------------------
// Locking Table prototype:
//--------------------------------------------------------
typedef enum
{
    PowerCycle=0,
    Hardware=1,
    HotPlug=2,
    Programmatic=3
}reset_types;

#pragma pack(1)
typedef struct {
    U64    uid;
    U8      name[20];
    U8      commonName[32];
    U64    rangeStart;
    U64    rangeLength;
    U8      readLockEnabled;
    U8      writeLockEnabled;
    U8      readLocked;
    U8      writeLocked;
    U8      lockOnReset[4];
    U64  activeKey;
}sLocking_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct {
    sLocking_TblObj val[LOCKING_RANGE_CNT+1];
    sColPrty    pty[11];            //column cnt
    U8      colCnt;
    U8      rowCnt;
    U8      objSize;            //table object size
    U8      maxCol;             //maximum column number in TCG spec
}sLocking_Tbl;
#pragma pack()

//--------------------------------------------------------
// MBRControl Table prototype:
//--------------------------------------------------------
#pragma pack(1)
typedef struct {
    U64    uid;
    U32    enable;
    U32    done;
    U8      doneOnReset[4];     //List, first byte is the element count
}sMbrCtrl_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct {
    sMbrCtrl_TblObj val[1];
    sColPrty    pty[4];             //column cnt
    U8      colCnt;
    U8      rowCnt;
    U8      objSize;            //table object size
    U8      maxCol;             //maximum column number in TCG spec
}sMbrCtrl_Tbl;
#pragma pack()

//--------------------------------------------------------
// K_AES Table prototype:
//--------------------------------------------------------
typedef enum
{
    AES_ECB=0,
    AES_CBC=1,
    AES_CTR=5,
    AES_XTS=7
}symmetric_mode_media;

#pragma pack(1)
typedef struct {
    U64  uid;
    //U8    name[32];
    U32 key[8];
    //symmetric_mode_media  mode;
    U32  mode;
}sKAES_TblObj;
#pragma pack()

#pragma pack(1)
typedef struct {
    sKAES_TblObj val[LOCKING_RANGE_CNT+1];
    sColPrty    pty[3];             //column cnt
    U8      colCnt;
    U8      rowCnt;
    U8      objSize;            //table object size
    U8      maxCol;             //maximum column number in TCG spec
}sKAES_Tbl;
#pragma pack()

#pragma pack(4)
typedef union
{
    struct
    {
        sTcgTblInfo       mTcgTblInfo;
        sSPInfo_Tbl       mAdmSPInfo_Tbl;
        sSPTemplates_Tbl  mAdmSPTemplates_Tbl;
        sTbl_Tbl          mAdmTbl_Tbl;
        sMethod_Tbl       mAdmMethod_Tbl;
        sAccessCtrl_Tbl   mAdmAccessCtrl_Tbl;
        sACE_Tbl          mAdmACE_Tbl;
        sAuthority_Tbl    mAdmAuthority_Tbl;
        sCPin_Tbl         mAdmCPin_Tbl;
        sTPerInfo_Tbl     mAdmTPerInfo_Tbl;
        sTemplate_Tbl     mAdmTemplate_Tbl;
        sSP_Tbl           mAdmSP_Tbl;
    } b;
    U8 all[CFG_UDATA_PER_PAGE * 1];     // buf size = 1 page = 16K                                                                                                                                                                                                                                                                 ///< All bits.
}tG1;
#pragma pack()

#pragma pack(4)
typedef union
{
    struct
    {
        sSPInfo_Tbl             mLckSPInfo_Tbl;
        sSPTemplates_Tbl        mLckSPTemplates_Tbl;
        sLckTbl_Tbl             mLckTbl_Tbl;
        sLckMethod_Tbl          mLckMethod_Tbl;
        sLckAccessCtrl_Tbl      mLckAccessCtrl_Tbl;
        sSecretProtect_Tbl      mLckSecretProtect_Tbl;
        sLockingInfo_Tbl        mLckLockingInfo_Tbl;
    } b;
    U8 all[CFG_UDATA_PER_PAGE * 2];     // buf size = 2 page = 32K                                                                                                                                                                                                                                                                 ///< All bits.
}tG2;
#pragma pack()

#pragma pack(4)
typedef union
{
    struct
    {
        sLckACE_Tbl         mLckACE_Tbl;
        sLckAuthority_Tbl   mLckAuthority_Tbl;
        sLckCPin_Tbl        mLckCPin_Tbl;
        sLocking_Tbl        mLckLocking_Tbl;
        sMbrCtrl_Tbl        mLckMbrCtrl_Tbl;
        sKAES_Tbl           mLckKAES_256_Tbl;
    } b;
    U8 all[CFG_UDATA_PER_PAGE * 2];     // buf size = 2 page = 32K                                                                                                                                                                                                                                                                 ///< All bits.
}tG3;
#pragma pack()

#pragma pack(4)
typedef union
{
    U8 all[CFG_UDATA_PER_PAGE * L2P_PAGE_CNT];          // ep2 = 48K, ep3 = 80K
    struct
    {
        U16 TcgG4Header;
        U16 Tcg4Gap[0x400/sizeof(U16)-1];               // 1K
        tTcgLogAddr TcgMbrL2P[G4G5_LAA_AMOUNT_LIMIT];   // ep2 = 37K, ep3 = 74K bytes
    } b;
}tG4;
#pragma pack()

#pragma pack(4)
typedef union
{
    U8 all[CFG_UDATA_PER_PAGE * L2P_PAGE_CNT];              // ep2 = 48K, ep3 = 80K
    struct
    {
        U16 TcgG5Header;
        U16 TcgG5Gap[0x400/sizeof(U16)-1];                  // 1K
        tTcgLogAddr TcgTempMbrL2P[G4G5_LAA_AMOUNT_LIMIT];   // ep2 = 37K, ep3 = 74K bytes
    } b;
}tG5;
#pragma pack()

typedef struct {
    U64 rangeStart;
    U64 rangeEnd;
    U32 rangeNo;
    U32 blkcnt;
    U32 readLocked;
    U32 writeLocked;
}sEnabledLockingTable;
COMPILE_ASSERT(sizeof(sEnabledLockingTable) == 32, "s32 bytes!");

//TCG Status Definition
#define TCG_ACTIVATED   0x01
#define MBR_SHADOW_MODE 0x02

// Block SID Auth status
#define SID_VALUE_CHANGED   0x10
#define SID_BLOCKED         0x20
#define SID_HW_RESET        0x40

#define TCG_DOMAIN_NORMAL    0
#define TCG_DOMAIN_SHADOW    1
#define TCG_DOMAIN_DUMMY     2
#define TCG_DOMAIN_ERROR     3

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
extern U32 mTcgStatus;        //TCG status variable for others
extern U16 mReadLockedStatus;   // b0=1: GlobalRange is Read-Locked, b1~b8=1: RangeN is Read-Locked.
extern U16 mWriteLockedStatus;  // b0=1: GlobalRange is Write-Locked, b1~b8=1: RangeN is Write-Locked.

extern sEnabledLockingTable mLockingRangeTable[LOCKING_RANGE_CNT+1];
extern tG1 G1;
extern tG2 G2;
extern tG3 G3;
extern tG4 G4;
extern tG5 G5;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Public function prototype definitions:
//-----------------------------------------------------------------------------
U16  TcgRangeCheck(U32 lba, U32 sc, BOOL writemode);
void TcgUpdateGlobalKey(void);



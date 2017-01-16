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
//#define _TCG_DEBUG_       //for temporary debug code only
//#define _TCG_LAB_RESCUE_ONLY

#define TCG_BUF_LEN (16*KBYTE)*3             // size=16K
#define DECBUF_LEN  (16*KBYTE)*3             //16*1024    // size=16K
#define LAA_LEN     CFG_UDATA_PER_PAGE

#define TCG_OVLIM_DISABLED

//#define FORCE_TO_CLEAR_ERASED_COUNT

// KEY_NO 0 is for FTL etc...
// KEY_NO 1 is for user data non-specific region (global)
// KEY_NO 2~9 for user data specific regions
#define GLOBAL_AES_KEY_NO           1
//#define TCG_REGION_TEST

#define TCG_RD_ERR_DESC_NO_MAX_SIZE  CFG_CHANNEL_CNT * CFG_CE_CNT * TCG_DISC_CNT_PER_PAGE

#define TCG_SILO_TXH_LEN            0x20    //TCG Silo transfer command header length

#define HeadOfEachBlk_Cnt           3
#define TCG_MBR_CELLS               TCG_CFG_BLK_CNT_PER_GRP     // L9=192 blocks , M3/M6=128 blocks
#define TCG_GROUP4_MARK             0x00434734

#define START_TCG_DS_IDX           START_TCG_MBR_IDX

#define TCG_GROUP5_MARK             0x00434735

#define TCG_GROUP1_MARK             0x54434731
#define TCG_GROUP1_START_CELL_NO   0
#define TCG_GROUP1_CELLS           1
#define TCG_GROUP1_END_CELL_NO     (TCG_GROUP1_START_CELL_NO+TCG_GROUP1_CELLS)
#define TCG_GROUP2_MARK             0x54434732
#define TCG_GROUP2_START_CELL_NO   TCG_GROUP1_END_CELL_NO
#define TCG_GROUP2_CELLS           2
#define TCG_GROUP2_END_CELL_NO     (TCG_GROUP2_START_CELL_NO+TCG_GROUP2_CELLS)
#define TCG_GROUP3_MARK             0x54434733
#define TCG_GROUP3_START_CELL_NO   TCG_GROUP2_END_CELL_NO
#define TCG_GROUP3_CELLS           2
#define TCG_GROUP3_END_CELL_NO     (TCG_GROUP3_START_CELL_NO+TCG_GROUP3_CELLS)

#define TCG_DISC_CNT_PER_PAGE       2
#define TCG_LAA_CNT_PER_DESC        2

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#ifdef alexcheck
#define soutb(a) {}
#define soutb2(a,b) {}
#define soutb3(a, b, c) {}
#define soutb4(a, b, c, d) {}
#define soutw(a) {}
#define soutd(a) {}
#define souts(a) {}
#define sout64(a) {}
#endif

#define ResetAllCache() {}

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef union
{
    U64  all;
    U8   bytes[8];
} UID64;

typedef struct
{
    U8   rsv[6];
    U16  kind;
    U32  length;
} sSubPktFmt;

typedef struct
{
    U32  TSN;
    U32  HSN;
    U32  SeqNo;
    U8   rsv[2];
    U16  AckType;
    U32  ack;
    U32  length;
} sPktFmt;

typedef struct
{
    U8     rsv[4];
    U16    ComID;
    U16    ComIDExt;
    U32    Outstanding;
    U32    MinTx;
    U32    length;
    sPktFmt     mPktFmt;
    sSubPktFmt  mSubPktFmt;
    //U8      payload[456];   //payload, b56~b511
    U8      payload[TCG_BUF_LEN \
                        -sizeof(U8)*4 \
                        -sizeof(U16) \
                        -sizeof(U16) \
                        -sizeof(U32) \
                        -sizeof(U32) \
                        -sizeof(U32) \
                        -sizeof(sPktFmt)  \
                        -sizeof(sSubPktFmt)];
    //U16        idx;
} sCmdPkt;

typedef struct
{
    U8  cmd;
    U8  ProtocolID;
    U8  Len;
    U16 ComID;
} sTcgCmd;

/* typedef struct
{
    U8 InvokingUID[8];
    U8 MethodUID[8];
    ...
} sTcgCmdParam; */

typedef struct
{
    U64     HtAuthorityClass;   //added for Authority Class: Null / Admins / Users
    UID64   SPID;               //AdminSP or LockingSP
    UID64   HtSgnAuthority;     //3, Authority obj
    U32     HostSessionID;
    U32     SPSessionID;        //assigned by TPer
    tSESSION_STATE state;
    tTRNSCTN_STATE TransactionState;
    U8      HtChallenge[36];    //0
    U8      Write;
    U8      status[3];
} sSessionManager;
COMPILE_ASSERT(sizeof(sSessionManager) == 80, "please align to 4 bytes!");

typedef struct
{
    U32    invokingUID;
    tATOM_DEC_TYPE type;
} sWrTypeAdjustment;

typedef struct
{
  U32    offset;
  U32    length;
} sDataStoreAddr;

typedef struct
{
  U8     cnt;
  U8     policy;
  U16    range;    //b0=1 for GRange is SingleUser, bN=1 for RangeN is SingleUser
} sSgUser;
sSgUser mSgUser;

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
typedef enum
{
    ST_POWER_OFF,
    ST_AWAIT_IF_SEND,
    ST_PROCESSING,
    ST_AWAIT_IF_RECV,
    ST_NAN = 0x7FFFFFFF
} tTCG_CMD_STATE;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
extern tG1  G1;
extern tG2  G2;
extern tG3  G3;
extern tG4  G4;
extern tG5  G5;
extern U16  MBR_TEMPL2PTBL[MBR_LEN / CFG_UDATA_PER_PAGE];
extern U16  DS_TEMPL2PTBL[DATASTORE_LEN / CFG_UDATA_PER_PAGE];

extern U16  gTcgMbrCellVac[TCG_MBR_CELLS];               // 256
extern U16  gTcgTempMbrCellVac[TCG_MBR_CELLS];           // 256
#if _NAND_ == NAND_3DTLCG2
    extern U32 gTcgMbrCellValidMap[TCG_MBR_CELLS][18];       // 8K
    extern U32 gTcgTempMbrCellValidMap[TCG_MBR_CELLS][18];   // 8K
#elif _NAND_ == NAND_MLC
    extern U32 gTcgMbrCellValidMap[TCG_MBR_CELLS][8];       // 4K
    extern U32 gTcgTempMbrCellValidMap[TCG_MBR_CELLS][8];   // 4K
#endif
extern BOOL RdTcgMBRbyNormalRdCmd;                      // Non-TrustCmd read TcgMBR area, 1-> Non-TrustCmd,  0->TrustCmd

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
//extern void sha256(unsigned char *message, unsigned int len, unsigned char *digest);

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
//TCG Function Declaration:
void TcgInit(BOOL, BOOL);
void TcgComReset(void);
void CopyHashed(U8*, U8, U8*); //CopyMSID2SID(void);

void TcgCmdProcessing(HcmdQ_t* pHcmdQ);
U16  TperProcessing(HcmdQ_t* pHcmdQ);
U16  TcgCmdPktParser(U8 *);
U16  TcgStreamDecode(void);    //sCmdPkt *);
U16  TcgCmdPkt4Response(void);
U16  TcgRespPktUpdate(U8);
void TcgCmdPkt4NoData(void);
void TcgCmdPkt4CloseSession(void);

U16  TcgAbortSession(void);
U16  H2TP_Properties(void);
void InitTbl(void);

U16  Level0_Discovery(U8 *);
void Supported_Security_Protocol(U8 *);

U16  InvokingSessionManager(void);
U16  InvokingAdminTable(void);
U16  InvokingLockingTable(void);
int  AtomDecoding(tATOM_DEC_TYPE AtomDecodeType, U32* length);
int  AtomDecoding_HalfUid(U32* length);
int  AtomDecoding_Uid(U32* length);
int  AtomDecoding_Bytes(U32* length);
int  AtomDecoding_Int(U32* length);
U16  AtomDecoding_Uid2(U8 *);
U16  AtomDecoding_Uint8(U8 *);
U16  AtomDecoding_Uint32(U32 *);
int  SkipTokenEmpty(void);
U8   ChkToken(void);

U16  HostSigningAuthorityCheck(void);
U16  ChkMethodStatus(void);
void ResetSessionManager(void);

int  AdminAccessControlCheck(void);
int  AdminAceBooleanExprChk(void);
int  LockingAccessControlCheck(void);
int  LockingAceBooleanExprChk(void);

U16  Method_Next(void);
U16  Method_Get(void);
U16  Method_Set(void);
int  AceColumnsChk(U8);
int  AtomEncoding_Byte(U32);
int  AtomEncoding_Integer(U8*, U8);
U16  Method_GetACL(void);
void WriteMtable2NAND(void);
void ReadNAND2Mtable(void);
int  InvertByteSequence(U8, U8 *);
int  Write2Mtable(U8 *tBuf, U32 tLen, U32 setColNo, U8 MultiAuthority_idx);
int  IsSettingInSameColumn(U32 *colbuf, U32 curIndex);
void ClearMtableChangedFlag(void);
void CalDataStoreLaaLoaction(U32 saddr, U32 eaddr);
int  IsPSIDWritable(U8 psid_row);

U16  Method_Activate(void);
U16  Method_Revert(void);
U16  Method_GenKey(void);
U16  Method_RevertSP(void);
void ChangeKey(U8);

void LockingTbl_Reset(U8 type);
int  LockingTbl_RangeChk(U64, U64, U64);
void LockingRangeTable_Update(void);

void TcgPreformatAndInit(U8 IsTcgInit);
void TcgKeepGroupDataBridge(U32 GrpNum);

void TcgAllClean(void);
void MbrCtrlTbl_Reset(U8 type);

U16  HandleComIDRequest(HcmdQ_t* pHcmdQ, U8* buf);
void HandleComIDResponse(U8 * buf);
U16  TcgTperReset(void);
void HostPropertiesReset(void);
U16  Method_Authenticate(void);
U16  Method_Random(void);
void SingleUser_Setting(void);
void DataStore_Setting(U8 cnt, U32 * DSTblSize);
void DataStoreAddr_Update(void);
U16  Method_Reactivate(void);
U16  Method_Erase(void);
void HostTx2TcgBuf (HcmdQ_t* pHcmdQ, U8* buf);

#if _TCG_ == TCG_EDRV
void SiloCmdProcessing(HcmdQ_t* pHcmdQ);
void ProbeSilo_Probe(U8* buf, U32 len);
void TcgSilo_GetSiloCap(U8* buf, U32 len);
void TcgSilo_Transfer(HcmdQ_t* pHcmdQ, U8* buf, U32 len);
void TcgSilo_Reset(HcmdQ_t* pHcmdQ, U8* buf, U32 len);
void TcgSilo_TPerReset(U8* buf, U32 len);
void TcgSilo_GetResult(HcmdQ_t* pHcmdQ, U8* buf, U32 len);
#endif

#ifdef _TCG_LAB_RESCUE_ONLY
U16  Method_Debug_Revert(void);
#endif

U32 IsG1Blank(void);
U32 IsG2Blank(void);
U32 IsG3Blank(void);
U32 IsDSBlank(void);
U32 IsG1G2G3DSAllBlank(void);
U32 IsG1G2G3OneOfBlank(void);
int TCG_NewTable(BOOL mRst, U8* tmp);
int TCG_BuildTable(void);

int ChkDefaultTblPattern(void);

void DumpG4ErasedCount(void);
void DumpG5ErasedCount(void);
void DumpG4DftAmount(void);
void DumpG5DftAmount(void);
void DumpRangeInfo(void);
void DumpTcgEepInfo(void);

//-----------------------------------------------------------------------------
//  Public function prototype definitions:
//-----------------------------------------------------------------------------
void TcgInit8PwrOn(void);
void SATA_CMD_TrustedNonData(HcmdQ_t* pHcmdQ);

void Nvme2Sata_TrustedReceive(HcmdQ_t* pHcmdQ);
void Nvme2Sata_TrustedSend (HcmdQ_t* pHcmdQ);

void TCG_ReadNormal(HcmdQ_t* pHcmdQ, U16 RngSts);

int  BufAddrShare(void);
int  GroupAddrShare(void);
int  SMBR_Read(U16 laas, U16 laae, PVOID pBuffer, MsgTcg_t** myTcgMsg);
int  SMBR_Write(U16 laas, U16 laae, PVOID pBuffer, U16 DesOffset, U16 SrcLen, U8* SrcBuf);
int  DS_Read(U16 laas, U16 laae, PVOID pBuffer);
int  DS_Write(U16 laas, U16 laae, PVOID pBuffer, U16 DesOffset, U16 SrcLen, U8* SrcBuf);

/*----------------------------------------------------------------------------
//Personal debug message define & coding
 ---------------------------------------------------------------------------*/
//#define _ALEXDEBUG
//#define _ALEXDBG_NAND
#ifdef _ALEXDEBUG
    #define s1 ""
    #define s2 ""
    #define alexdebug1s(d0,s) alexdebug1_s(d0,#s)
    #define alexdebug2s(d0,d1,s) alexdebug2_s(d0,d1,#s)
    #define alexdebug3s(d0,d1,d2,s) alexdebug3_s(d0,d1,d2,#s)
    #define alexdebug4s(d0,d1,d2,d3,s) alexdebug4_s(d0,d1,d2,d3,#s)
    #define alexdebugws(d0,s) alexdebugw_s(d0,#s)
    #define alexdebug2ws(d0,d1,s) alexdebug2w_s(d0,d1,#s)
    #define alexdebugds(d0,s) alexdebugd_s(d0,#s)
    #define alexdebug2ds(d0,d1,s) alexdebug2d_s(d0,d1,#s)
    #define alexdebugls(d0,s) alexdebugl_s(d0,#s)
    #define alexdebug2ls(d0,d1,s) alexdebug2l_s(d0,d1,#s)
    #define alexdebug3ws(d0,d1,d2,s) alexdebug3w_s(d0,d1,d2,#s)
    #define alexdebug3ds(d0,d1,d2,s) alexdebug3d_s(d0,d1,d2,#s)
    #define alexdebug3ls(d0,d1,d2,s) alexdebug3l_s(d0,d1,d2,#s)
    #define alexdebug4ws(d0,d1,d2,d3,s) alexdebug4w_s(d0,d1,d2,d3,#s)
    #define alexdebug4ds(d0,d1,d2,d3,s) alexdebug4d_s(d0,d1,d2,d3,#s)
    #define alexdebug4ls(d0,d1,d2,d3,s) alexdebug4l_s(d0,d1,d2,d3,#s)
    #define alexdebugs(s) alexdebug_s(#s)

    #define alexdebug1b(d0) alexdebug1_s(d0)
    #define alexdebug2b(d0,d1) alexdebug2_s(d0,d1)
    #define alexdebug3b(d0,d1,d2) alexdebug3_s(d0,d1,d2)
    #define alexdebug4b(d0,d1,d2,d3) alexdebug4_s(d0,d1,d2,d3)
    #define alexdebugwb(d0) alexdebugw_s(d0)
    #define alexdebugdb(d0) alexdebugd_s(d0)
    #define alexdebuglb(d0) alexdebugl_s(d0)

    EXTERN void alexdebug1_s(U8 dat0,char *str);
    EXTERN void alexdebug2_s(U8 dat0,U8 dat1,char *str);
    EXTERN void alexdebug3_s(U8 dat0,U8 dat1,U8 dat2,char *str);
    EXTERN void alexdebug4_s(U8 dat0,U8 dat1,U8 dat2,U8 dat3,char *str);
    EXTERN void alexdebugw_s(U16 dat0,char *str);
    EXTERN void alexdebug2w_s(U16 dat0,U16 dat1,char *str);
    EXTERN void alexdebugd_s(U32 dat0,char *str);
    EXTERN void alexdebug2d_s(U32 dat0,U32 dat1,char *str);
    EXTERN void alexdebugl_s(U64 dat0,char *str);
    EXTERN void alexdebug2l_s(U64 dat0,U64 dat1,char *str);

    EXTERN void alexdebug3w_s(U16 dat0,U16 dat1,U16 dat2,char *str);
    EXTERN void alexdebug3d_s(U32 dat0,U32 dat1,U32 dat2,char *str);
    EXTERN void alexdebug3l_s(U64 dat0,U64 dat1,U64 dat2,char *str);
    EXTERN void alexdebug4w_s(U16 dat0,U16 dat1,U16 dat2,U16 dat3,char *str);
    EXTERN void alexdebug4d_s(U32 dat0,U32 dat1,U32 dat2,U32 dat3,char *str);
    EXTERN void alexdebug4l_s(U64 dat0,U64 dat1,U64 dat2,U64 dat3,char *str);
    EXTERN void alexdebug_s(char *str);

    EXTERN void alexdebug1(U8 dat0);
    EXTERN void alexdebug2(U8 dat0,U8 dat1);
    EXTERN void alexdebug3(U8 dat0,U8 dat1,U8 dat2);
    EXTERN void alexdebug4(U8 dat0,U8 dat1,U8 dat2,U8 dat3);
    EXTERN void alexdebugw(U16 dat0);
    EXTERN void alexdebugd(U32 dat0);
    EXTERN void alexdebugl(U64 dat0);
    EXTERN void alexdebugdns(U32 dat0);
#else
    #define s1 ""
    #define s2 ""
    #define alexdebug1s(d0,s) {}
    #define alexdebug2s(d0,d1,s) {}
    #define alexdebug3s(d0,d1,d2,s) {}
    #define alexdebug4s(d0,d1,d2,d3,s) {}
    #define alexdebugws(d0,s) {}
    #define alexdebug2ws(d0,d1,s) {}
    #define alexdebugds(d0,s) {}
    #define alexdebug2ds(d0,d1,s) {}
    #define alexdebugls(d0,s) {}
    #define alexdebug2ls(d0,d1,s) {}
    #define alexdebug3ws(d0,d1,d2,s) {}
    #define alexdebug3ds(d0,d1,d2,s) {}
    #define alexdebug3ls(d0,d,d2,s) {}
    #define alexdebug4ws(d0,d1,d2,d3,s) {}
    #define alexdebug4ds(d0,d1,d2,d3,s) {}
    #define alexdebug4ls(d0,d,d2,d3,s) {}
    #define alexdebugs(s) {}

    #define alexdebug1b(d0) {}
    #define alexdebug2b(d0,d1) {}
    #define alexdebug3b(d0,d1,d2) {}
    #define alexdebug4b(d0,d1,d2,d3) {}
    #define alexdebugwb(d0) {}
    #define alexdebugdb(d0) {}
    #define alexdebuglb(d0) {}
#endif  //_ALEXDEBUG

#if defined(_ALEXDEBUG) && defined(_ALEXDBG_NAND)
    #define alexdbg_nand2ws(w0,w1,s0)           alexdebug2ws(w0,w1,s0)
    #define alexdbg_nand3ws(w0,w1,w2,s0)        alexdebug3ws(w0,w1,w2,s0)
    #define alexdbg_nand4ws(w0,w1,w2,w3,s0)     alexdebug4ws(w0,w1,w2,w3,s0)
    #define alexdbg_nand2ds(d0,d1,s0)           alexdebug2ds(d0,d1,s0)
    #define alexdbg_nand3ds(d0,d1,d2,s0)        alexdebug3ds(d0,d1,d2,s0)
    #define alexdbg_nand4ds(d0,d1,d2,d3,s0)     alexdebug4ds(d0,d1,d2,d3,s0)
#else
    #define alexdbg_nand2ws(w0,w1,s0){}
    #define alexdbg_nand3ws(w0,w1,w2,s0){}
    #define alexdbg_nand4ws(w0,w1,w2,w3,s0){}
    #define alexdbg_nand2ds(d0,d1,s0){}
    #define alexdbg_nand3ds(d0,d1,d2,s0){}
    #define alexdbg_nand4ds(d0,d1,d2,d3,s0){}
#endif


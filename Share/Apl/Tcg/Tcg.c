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
#include "BuildPkg.h"
#include "SharePkg.h"
#include "NvmeCmdPkg.h"
#include "HostPkg.h"

#include "SysInfo.h"
#include "HldNvme.h"
#include "NvmeDirective.h"
#include "NvmeStatusCodes.h"
#include "HalNvmeXfer.h"
#include "NvmeSecurityCmd.h"
#include "TcgSection.h"    // CODE AND DATA MEMORY MAPPING
#include "TcgCommon.h"
#include "Sha1.h"
#include "Tcg.h"
#include "TcgNfIf.h"
#include "BcmClient.h"
#include "Cache.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
//ATA definition
#define ATA_TRUSTED_RECEIVE         0x5C
#define ATA_TRUSTED_RECEIVE_DMA     0x5D
#define ATA_TRUSTED_SEND            0x5E
#define ATA_TRUSTED_SEND_DMA        0x5F

//TPer Definition
#define BASE_COMID                  0x07FE   //test only
#define BASE_SPSESSION_ID           0x1001


#define PropertiesAmount            25
#define HostPropertiesCnt           6
#define TperPropertiesCnt           15

//H2TP properties table
#define HOST_MAX_COMPKT_SZ          0x800
#define HOST_MAX_PKT_SZ             0x7EC
#define HOST_MAX_INDTKN_SZ          0x7C8

// TP2H properties table
#define MAX_COMPKT_SZ               (0xC000)  //change from 0x2000 to 0x2494 for WareSystem AP
#define MAX_PKT_SZ                  (MAX_COMPKT_SZ-20)
#define MAX_INDTKN_SZ               (MAX_COMPKT_SZ-56)
#define MAX_RESP_COMPKT_SZ          0xC000  //change from 0x2000 to 0x2494 for WareSystem AP

#define eMaxSetParamCnt             32

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct
{
    U8     cmd;
    U8     ProtocolID; //0x01, 0x02, or 0x06
    U16    length;      //ByteCnt for NVMe
    U16    ComID;
} sAtaCmd;
sAtaCmd mAtaCmd;        //stored trusted command parameter

typedef struct
{
    char *name;
    U32 val;
} sProperties;

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
//#include "NormSection.h"
#include "FastSection.h"      // from DRAM change to CPU0 B0TCM0, it need to take CPU2 whether can access this. (pLockingRangeTable)
__align(16) sEnabledLockingTable mLockingRangeTable[LOCKING_RANGE_CNT+1];

#include "FastSection.h"
/****** for speed up ******/
U32     mTcgStatus;             //TCG status variable for others
U16     mReadLockedStatus;      // b0=1: GlobalRange is Read-Locked, b1~b8=1: RangeN is Read-Locked.
U16     mWriteLockedStatus;     // b0=1: GlobalRange is Write-Locked, b1~b8=1: RangeN is Write-Locked.
U32     RdTcgMBRbyNormalRdCmd = FALSE; //Non-TrustCmd read TcgMBR area, 1-> Non-TrustCmd,  0->TrustCmd
U32     TcgMsgOpCode;

#include "TcgSection.h"    // CODE AND DATA MEMORY MAPPING

__align(16) U8 TcgHstTxRxBuf[TCG_BUF_LEN+512+4096]; // tcg transfer buffer for host
__align(16) U8 dataBuf[TCG_BUF_LEN];                //buffer for IF-RECV data block (packet), temp only...
__align(16) U8 DecBuf[DECBUF_LEN];
__align(16) sCmdPkt mCmdPkt;

sSessionManager mSessionManager;

tG4     G4;
tG5     G5;
U16     MBR_TEMPL2PTBL[MBR_LEN/CFG_UDATA_PER_PAGE];
U16     DS_TEMPL2PTBL[DATASTORE_LEN/CFG_UDATA_PER_PAGE];

U32     SendHostSectorCnt;   //bytecnt for NVMe
U8      statusCode[3];       //status code list for method encoding (core 3.2.4.2)

U32     ploadLen;            //payload length in the subpacket
U16     iPload;              //index for payload access
U16     iDataBuf;            //index (pointer) for dataBuf
U16     rcvCmdPktLen;        //cmdPktLen for IF-RECV

U16     tgtComID = 0x0000;   //tgtComID from IF-Send, for MS eDrive requirement...

BOOL    bControlSession;     //InvokingUID should be Session Manager only...
BOOL    bKeyChanged = FALSE;

// TCG global variables for Access Control [
UID64   invokingUID, methodUID;                        //InvokingID and MethodID from payload
U64     aclUid[LCK_ACCESSCTRL_ACL_CNT], getAclAclUid;  //from AccessControl
U8      iAceRow;                                       //row index for ACE check (for Access Control)
U8      authorityChkIsDone;

U32     mCPinPatch;

#if _TCG_ == TCG_EDRV
BOOL    bEHddLogoTest;
U16     mPsidRevertCnt;
#endif

__align(4) sDataStoreAddr    mDataStoreAddr[DSTBL_MAX_NUM];
__align(4) tTCG_CMD_STATE    gTcgCmdState;
__align(4) sTcg2RWtaskInfo   _mTcg2RWtaskInfo_bak;
__align(4) U32               TcgAuxData[mNUM_AU_PER_PAGE];  // for 8K page // [2] [3] are not used

BOOL    G1MtableChanged;        // group1  Mtable
BOOL    G2MtableChanged;        // group2  Mtable
BOOL    G3MtableChanged;        // group3  Mtable
BOOL    G4MBRChanged;           // group4  MBR
BOOL    G6DataStoreChanged;     // group6  Data Store
BOOL    LockingTabChanged;      // range start or range length of locking table was changed

BOOL    Tcg_RdWrProcessing;     //TCG RD/WR NAND processing
U16     Start_DataStoreLaa;
U16     End_DataStoreLaa;

U8      mHandleComIDRequest;    //for ProtocolID=2
U8      mHandleComIDResponse;

U32     mPOutLength;            //in bytes
U32     mPCLength;              //P_OUT Payload Content Length (B0~B3)
U16     siloIdxCmd;             //HiByte: Index, LoByte: Cmd, this parameter is also used to identify whether the Silo command is processing or not!!
U16     SiloComID;
U8      b1667Probed;
U8      m1667Status;

char    preformat_id_string[16];
U32     TableError;

U16     TcgPageEccCount;
U8      TcgG4Defects,TcgG5Defects;
PVOID   TcgTmpBuf = NULL;

#ifdef FTL_FORWARD_CLOSE
U16 TCG_FTLReqFirstCloseBlk;
#endif

// following is for Properties
// H2TP properties data struct configuration
char strMaxComPktSz[]=      "MaxComPacketSize";
char strMaxRespComPktSz[]=  "MaxResponseComPacketSize";
char strMaxPktSz[]=         "MaxPacketSize";
char strMaxIndTknSz[]=      "MaxIndTokenSize";
char strMaxPkt[]=           "MaxPackets";
char strMaxSubPkt[]=        "MaxSubpackets";
char strMaxMtd[]=           "MaxMethods";
char strMaxSess[]=          "MaxSessions";
char strAuth[]=             "MaxAuthentications";
char strTranLmt[]=          "MaxTransactionLimit";
char strDefSessTmOut[]=     "DefSessionTimeout";
//char str11[]=               "MaxReadSessions";
//char str12[]=               "MaxAggTokenSize";

//char str13[]=               "MaxSessionTimeout";
//char str14[]=               "MinSessionTimeout";
//char str15[]=               "DefTransTimeout";
//char str16[]=               "MaxTransTimeout";
//char str17[]=               "MinTransTimeout";
//char str18[]=               "MaxComIDTime";
char strContTkn[]=          "ContinuedTokens";
char strSeqNum[]=           "SequenceNumbers";
char strAckNak[]=           "AckNak";
char strAsync[]=            "Asynchronous";
//char str23[]=               "MaxSessionTimeout";
//char str24[]=               "MinSessionTimeout";

sProperties mHostProperties[HostPropertiesCnt]=
{
    {strMaxComPktSz,    HOST_MAX_COMPKT_SZ},    //MaxComPacketSize
    {strMaxPktSz,       HOST_MAX_PKT_SZ},       //MaxPacketSize
    {strMaxIndTknSz,    HOST_MAX_INDTKN_SZ},    //MaxIndTokenSize
    {strMaxPkt,         1},                     //MaxPackets
    {strMaxSubPkt,      1},                     //MaxSubpackets
    {strMaxMtd,         1},                     //MaxMethods
};

sProperties mTperProperties[TperPropertiesCnt]=
{
    {strMaxComPktSz,    MAX_COMPKT_SZ},         //MaxComPacketSize
    {strMaxRespComPktSz, MAX_COMPKT_SZ},
    {strMaxPktSz,       MAX_PKT_SZ},            //MaxPacketSize
    {strMaxIndTknSz,    MAX_INDTKN_SZ},         //MaxIndTokenSize
    {strMaxPkt,         1},                     //MaxPackets
    {strMaxSubPkt,      1},                     //MaxSubpackets
    {strMaxMtd,         1},                     //MaxMethods
    {strContTkn,        0},
    {strSeqNum,         0},
    {strAckNak,         0},
    {strAsync,          0},
    {strMaxSess,        1},
    {strAuth,           2},
    {strTranLmt,        1},
    {strDefSessTmOut,   0},
};

sWrTypeAdjustment cWrTypeAdjustment[]= \
{ \
    {0x00000008,ATOM_UID},      //ACE
    {0x00000009,ATOM_INT},      //Authority
    {0x0000000B,ATOM_BYTES},    //C_PIN
    {0x00000802,ATOM_INT},      //Locking
    {0x00000803,ATOM_INT},    //MBRControl
    {0x00000804,ATOM_BYTES},    //MBR
    {0x00001001,ATOM_BYTES}    //Data Store
    ,{0x00000201,ATOM_INT}      //TPerInfo
};

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
int TcgH2cJobIssue(tMSG_TCG_SUBOP subOP, MsgTcg_t* pMyTcgMsg);
int TcgResetCache(tMSG_TCG_SUBOP subOP);

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern Error_t HalNvme_RequestDataTransferIoQuick(MsgHostIO_t* pHcmdMsg);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
void TcgInit8PwrOn(void)
{
    HERE(NULL);

    TCG_PRINTF("FTL provide TCGBLK0[%X] TCGBLK1[%X] TCGBLK2[%X] TCGBLK3[%X]\n", \
                smSysInfo->d.TcgData.d.TCGBlockNo[0], smSysInfo->d.TcgData.d.TCGBlockNo[1],  \
                smSysInfo->d.TcgData.d.TCGBlockNo[2], smSysInfo->d.TcgData.d.TCGBlockNo[3]   \
                );

#ifdef TCG_EEP_NOR
    TcgH2cJobIssue(MSG_TCG_NOREEP_INIT, NULL);
    TCG_PRINTF("TcgNorEepIdx = %X\n", tcgEepProgIdx);
#endif

    if(strcmp((char *)tcg_prefmt_tag, PREFORMAT_END_TAG) != 0){
        soutb3(0x5C, 0xFE, 0x9B); souts((char *)tcg_prefmt_tag);
        TCG_PRINTF("!!! Error !!! PreFormat is Required.\n");              //soutb3(0x5C, 0xFE, 0x98);  //5C FE 98,    "!!! table NG !!! PreFormat is Required."
    }
    if( (strcmp((char *)tcgDefectID, DEFECT_STRING) != 0)                 /*   // first time ? , if so, then init defect table  */
         || (strcmp((char *)tcgErasedCntID, ERASED_CNT_STRING) != 0)       /*   // first time ? , if so, then init erased count table */
         #if 0   // now, this function is closed auto tcg preformat when mtab is not equal ctab . it need to disscuss  if release wrong version fw
         || (tcg_mTbl_id != cTcgTblInfo.ID)
         || (tcg_mTbl_ver != cTcgTblInfo.ver)
         #endif
    ){
        TableError = TRUE;   // NOR or NAND table error
        // TcgPreformatAndInit(FALSE);        // Don't auto do this to protect user's data, It just show message
        TCG_PRINTF("FW DftTbl Tag= %s, EEP DftTbl Tag= %s \n", DEFECT_STRING, (char *)tcgDefectID);    //soutb3(0x5C, 0xFE, 0x94); souts(DEFECT_STRING);             //5C FE 94,    "Bin defect tbl ID = %s"
                                                                                                       //soutb3(0x5C, 0xFE, 0x95); souts((char *)tcgDefectID);       //5C FE 95,    "NOR defect tbl ID = %s"
        TCG_PRINTF("FW EraCntTbl Tag= %s, EEP EraCntTbl Tag= %s\n", ERASED_CNT_STRING, (char *)tcgErasedCntID);   //soutb3(0x5C, 0xFE, 0x96); souts(ERASED_CNT_STRING);         //5C FE 96,    "Bin Erase cnt tbl ID = %s"
                                                                                                                  //soutb3(0x5C, 0xFE, 0x97); souts((char *)tcgErasedCntID);    //5C FE 97,    "NOR Erase cnt tbl ID = %s"
        TCG_PRINTF("!!! table NG !!! PreFormat is Required.\n");              //soutb3(0x5C, 0xFE, 0x98);  //5C FE 98,    "!!! table NG !!! PreFormat is Required."
    }else{
        TableError = FALSE;
    }
    TcgInit(FALSE,FALSE);
}

void TcgInit(BOOL mRst, BOOL bClearCache)
{
    U32 cTblId, cTblVer;
    U32 *pt;
    tG1 *pMyG1;
    U8 tmp[PWD_LENGTH];
    U8 result;
    U8 j=0;
    U8 y=0;

    HERE(NULL);
    if((tcgDevTyp != TCG_SSC_EDRIVE) && (tcgDevTyp != TCG_SSC_OPAL) && (tcgDevTyp != TCG_SSC_PYRITE))
    {
        tcgDevTyp = TCG_DEVICE_TYPE;  //Initial=edriver if there is no identify tcg driver

    #ifdef TCG_EEP_NOR
        TcgH2cJobIssue(MSG_TCG_NOREEP_WR, NULL);
    #else
        SyIn_Synchronize(SI_AREA_MISC, SYSINFO_WRITE, SI_SYNC_BY_TCG);
    #endif
    }

  //---------- Take this off when decide model by EEPROM --------
    tcgDevTyp = TCG_DEVICE_TYPE;
  //--------------------------------------------------------------------------------------------
    TCG_PRINTF("TCG_TYPE = %2X\n", tcgDevTyp);

    cTblId = G1.b.mTcgTblInfo.ID;
    cTblVer = G1.b.mTcgTblInfo.ver;
    TCG_PRINTF("cTcgTblID=%x, cTcgTblver=%x\n", cTblId, cTblVer);

    // following 4 function should be performed first.
    RdTcgMBRbyNormalRdCmd = FALSE;
    if(TcgTmpBuf == NULL){
        TcgTmpBuf = MEM_AllocBuffer(TCG_GENERAL_BUF_SIZE, 512);     // allocate 1MB for TCG using
    }
    if(BufAddrShare() == zNG){         //share the TcgTmpBuf  address
        return;
    }
    GroupAddrShare();       // share G1~G5 address
    //TableError = FALSE;

    mTcgStatus  = 0;
    gTcgCmdState = ST_AWAIT_IF_SEND;
    mSessionManager.SPSessionID = BASE_SPSESSION_ID-1;    //0x1001
    ResetSessionManager();

    Tcg_RdWrProcessing = FALSE;
    TcgG4Defects       = TcgG5Defects = 0;
#if 0
    //get SN# for CPin Hash calculation
    pt = (U32 *)&smSysInfo->d.MPInfo.d.SerialNumberDrive[0];
    if(smSysInfo->d.MPInfo.d.SerialNumberDrive[0]!=0) {
        MemSwapCopy(&mCPinPatch, smSysInfo->d.MPInfo.d.SerialNumberDrive+8, 4); // Current LITEON use 12 Bytes SN
        TCG_PRINTF("cp: %08X\n",mCPinPatch);
    }
    else
#endif

    mCPinPatch = 0x00;  //FFFFFFFF;

    // backup PSID if warm boot
    if(mRst){
        for(j=0; j<PWD_LENGTH; j++)
        tmp[j] = G1.b.mAdmCPin_Tbl.val[CPIN_PSID_IDX].pwd[j];
    }

    // G5, build FTL ---------------
    TcgH2cJobIssue(MSG_TCG_G5FTL, NULL);

    // G4, build FTL ---------------
    TcgH2cJobIssue(MSG_TCG_G4FTL, NULL);

    Start_DataStoreLaa = 0;
    End_DataStoreLaa   = DATASTORE_LEN/LAA_LEN;

    // build TCG table -----------------
    if(IsG1G2G3DSAllBlank()){
        TCG_NewTable(mRst,tmp);                 // new TCG table
    }else{
        if(IsG1G2G3OneOfBlank()){
            TableError = TRUE;
        }else{
            if(TCG_BuildTable() == zNG){         // build TCG table
                TableError = TRUE;
            }
        }
    }

    // G4, TCG MBR Group -end-------------------------------------------------------------------------------

    strcpy(preformat_id_string," ");  //clear preformat_id_string

    //check TcgTbl version first, if the version doesnot match, skip AesKey Initialization.
    //TCG_PRINTF("mTcgTblID=%x, mTcgTblver=%x\n", G1.b.mTcgTblInfo.ID, G1.b.mTcgTblInfo.ver);

    if((G1.b.mTcgTblInfo.ID != tcg_mTbl_id) || (G1.b.mTcgTblInfo.ver != tcg_mTbl_ver)){
        strcpy((char *)tcg_mTbl_idStr , "mID");
        tcg_mTbl_id = G1.b.mTcgTblInfo.ID;
        strcpy((char *)tcg_mTbl_verStr , "Ver");
        tcg_mTbl_ver = G1.b.mTcgTblInfo.ver;
    #ifdef TCG_EEP_NOR
        TcgH2cJobIssue(MSG_TCG_NOREEP_WR ,NULL);
    #else
        SyIn_Synchronize(SI_AREA_MISC, SYSINFO_WRITE, SI_SYNC_BY_TCG);
        #endif
    }

    if((cTblId != tcg_cTbl_id) || (cTblVer != tcg_cTbl_ver)) {
        strcpy((char *)tcg_cTbl_idStr , "cID");
        tcg_cTbl_id = cTblId;
        strcpy((char *)tcg_cTbl_verStr , "Ver");
        tcg_cTbl_ver = cTblVer;
    #ifdef TCG_EEP_NOR
        TcgH2cJobIssue(MSG_TCG_NOREEP_WR, NULL);
    #else
        SyIn_Synchronize(SI_AREA_MISC, SYSINFO_WRITE, SI_SYNC_BY_TCG);
        #endif
    }

    if(TableError){
        TCG_PRINTF("!!! NG TCG Table Err, G1bnk=%X, G2bnk=%X, G3bnk=%X\n", IsG1Blank(), IsG2Blank(), IsG3Blank());
    }else{
        if((cTblId != G1.b.mTcgTblInfo.ID) || (cTblVer != G1.b.mTcgTblInfo.ver))
        { //not matched, NOR or NAND table error, and all of AES ranges are disabled!!
            TableError = TRUE;
            TCG_PRINTF("!!! NG mTcgTblID=%x, mTcgTblver=%x\n", G1.b.mTcgTblInfo.ID, G1.b.mTcgTblInfo.ver);
        }
        else
        {
            //Locking Table Initialization
            MbrCtrlTbl_Reset(PowerCycle);       // G3.b.mLckMbrCtrl_Tbl.val[0].done should be zero after power on reset. (POWER CYCLE)
            LockingTbl_Reset(PowerCycle);       // LckLocking_Tbl "PowerOnReset"
            LockingRangeTable_Update();         // Update ranges, keys, and Read/Write LockedTable for Media Read/Write control
            DataStoreAddr_Update();
        }
    }

    //reset CPin tries count
    for(j=0;j<G1.b.mAdmCPin_Tbl.rowCnt;j++)
        G1.b.mAdmCPin_Tbl.val[j].tries=0;

    for(j=0;j<G3.b.mLckCPin_Tbl.rowCnt;j++)
        G3.b.mLckCPin_Tbl.val[j].tries=0;

#if 0 //(OEM == LENOVO) || (OEM == Lenovo) || (OEM == lenovo)
    G1.b.mAdmCPin_Tbl.val[0].tryLimit = 7;  // CPIN  ->  SID  ->  TRY LIMIT change from 5 to 7 for Lenovo [BID22959]
#endif

    if(G1.b.mAdmSP_Tbl.val[SP_LOCKING_IDX].lifeCycle==manufactured)
        mTcgStatus|= TCG_ACTIVATED;

#if CO_SUPPORT_BLOCK_SID_AUTH
    CopyHashed(&G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[1], G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[0], tmp);
    for(j=0; j<=CPIN_LENGTH; j++) {
        if(G1.b.mAdmCPin_Tbl.val[CPIN_SID_IDX].pwd[j] != tmp[j]){
            mTcgStatus |= SID_VALUE_CHANGED;
            break;
        }
    }
#endif

    TCG_PRINTF("mTcgStatus=%4x %s\n",mTcgStatus, mTcgStatus & TCG_ACTIVATED ? "Activated" : "unActivated");

    ClearMtableChangedFlag();
    LockingTabChanged = FALSE;

    mHandleComIDRequest = 0x00;

  #if _TCG_ == TCG_EDRV
    //b1667Probed = 0x00;
    siloIdxCmd = 0x00;
    SiloComID = BASE_COMID; //0x00;     //eDrive won't call GetSiloCap to get ComID @ Power-On

    bEHddLogoTest = FALSE; //for WHQL test
    mPsidRevertCnt = 0;
  #endif

    if((mTcgStatus&TCG_ACTIVATED)==0)
        G2.b.mLckLockingInfo_Tbl.val[0].rangeStartLengthPolicy=1; //set default as 1

    mSgUser.policy = G2.b.mLckLockingInfo_Tbl.val[0].rangeStartLengthPolicy;
    if(G2.b.mLckLockingInfo_Tbl.val[0].singleUserModeRange[0] == UID_Locking)
    { //EntireLocking
        mSgUser.range = 0xffff;
        mSgUser.cnt = LOCKING_RANGE_CNT+1;
    }
    else
    {
        U64 tmp64;
        U8 i;

        mSgUser.range = 0;
        mSgUser.cnt = 0;
        for(i=0;i<(LOCKING_RANGE_CNT+1);i++)
        {
            tmp64 = G2.b.mLckLockingInfo_Tbl.val[0].singleUserModeRange[i];
            if(tmp64)
            {
                for(j=0;j<(LOCKING_RANGE_CNT+1);j++)
                {
                    if(tmp64==G3.b.mLckLocking_Tbl.val[j].uid)
                    {
                        mSgUser.range |= (0x01<<j);
                        mSgUser.cnt++;
                        break;
                    }
                }
            }
            else
                break;
        }
    }

#ifdef _TCG_RESET_PSID
    {   //Copy Hash(MSID) 2 PSID
        CopyHashed(&G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[1], G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[0], G1.b.mAdmCPin_Tbl.val[CPIN_PSID_IDX].pwd);

        //if(RW_WaitG1Wr()==zOK)
        if(TcgH2cJobIssue(MSG_TCG_G1WR, NULL) == zOK)
            TCG_PRINTF("rst PSID OK\n");
        else
            TCG_PRINTF("rst PSID NG\n");
    }
#endif

    //Method_Debug_Revert();
#ifdef _TCG_LAB_RESCUE_ONLY
    {
        // to Lock any rd & wr in rescue mode
        U8 i;
        for(i=0; i<=LOCKING_RANGE_CNT; i++){
            mLockingRangeTable[i].writeLocked=1;
            mLockingRangeTable[i].readLocked=1;
        }
        soutb3(0x5C, 0xFE, 0x46); //!!Watch out, Lock any write and read in _TCG_LAB_RESCUE_ONLY
        TCG_PRINTF("!!Watch out, Locked all range in _TCG_LAB_RESCUE_ONLY\n");
    }
#endif

//#ifdef _ALEXDEBUG
    DumpG4ErasedCount();
    DumpG5ErasedCount();
//#endif
    if((TcgG4Defects > TCG_MBR_CELLS/2) || (TcgG5Defects > TCG_MBR_CELLS/2)){
        TCG_PRINTF("Warnning!! ,There are a lot of defect blocks.\n");
    }
    MEM_SET(MBR_TEMPL2PTBL, 0xFF, sizeof(MBR_TEMPL2PTBL));
    MEM_SET(DS_TEMPL2PTBL, 0xFF, sizeof(DS_TEMPL2PTBL));

    TcgResetCache(MSG_TCG_INIT_ZEROCACHE);
}

void TcgStackReset(void)
{
    //STACT_RESET processing
    if(mSessionManager.state==SESSION_START)
        ResetSessionManager();

    HostPropertiesReset();

    mHandleComIDRequest=0x02;
    mHandleComIDResponse=0x00;

    gTcgCmdState=ST_AWAIT_IF_SEND;       //need to reply status...
}

//deal with STACK_RESET only, Core 3.3.4.7.5
U16 HandleComIDRequest(HcmdQ_t* pHcmdQ, U8* buf)
{
    U8  errCode = 0;
    U16 tmp16;
    U16 i = 0;
    U32 tmp32 = 0;
    //mtBUFFER_ADDRESS bufAddr;

    // Get data from host and store in ATACmdBuf
    if(siloIdxCmd==0)   //normal IF-SEND
        HostTx2TcgBuf(pHcmdQ, buf);

    soutb3(0x5C, 0xFE, 0x52);
    TCG_PRINTF("Stack_Reset Request: ");

    HiByte(tmp16) = buf[i++];       //should be the same as the ComID @ ATA register
    LoByte(tmp16) = buf[i++];
    if(tmp16!=BASE_COMID)   //mAtaCmd.ComID)
    {   errCode=0xF0;   goto REQUEST_NG;    }

    HiByte(tmp16) = buf[i++];       //0x0000 for static ComID
    LoByte(tmp16) = buf[i++];
    if(tmp16!=0x00)
    {   errCode=0xF1;   goto REQUEST_NG;    }

    HiByte(HiWord(tmp32))=buf[i++];
    LoByte(HiWord(tmp32))=buf[i++];
    HiByte(LoWord(tmp32))=buf[i++];
    LoByte(LoWord(tmp32))=buf[i++];

    if(tmp32!=0x02)                 //TODO: how to reponse if not STACK_RESET request?
    {   errCode=0xF2;   goto REQUEST_NG;    }
#if 0  //reserved bytes, ignore for DM test.
    for(;i<(mAtaCmd.length*512);i++)
    {
        if(buf[i]!=0)
        {   errCode=0xF3;   goto REQUEST_NG;    }
    }
#endif
    TcgStackReset();


    //MEM_SET(dataBuf, 0x00, TCG_BUF_LEN);    //clear Tcg Recv Buffer
    //for(i=0;i<7;i++)
    //    dataBuf[i]=buf[i];
    //dataBuf[11]=0x04;                   //Available Data Length
    soutb(0x00);
    TCG_PRINTF("OK\n");
    return TRUE;


REQUEST_NG:
    mHandleComIDRequest=0x02;       //TODO: need to check...
    mHandleComIDResponse=0x01;
    //dataBuf[15]=0x01;                   //00 00 00 01 for Failure
    soutb(errCode);
    TCG_PRINTF("NG %2X\n", errCode);
    return TRUE;
}

void HandleComIDResponse(U8* buf)
{
soutb3(0x5C, 0xFE, 0x53);  soutb2(mHandleComIDRequest,mHandleComIDResponse);
    TCG_PRINTF("Stack_Reset Response: %2X %2X\n",mHandleComIDRequest, mHandleComIDResponse);

    //cj ToDO: return NG if mHandleComIDRequest==0? (no request, no response?) 2016/05/17

    buf[0]=HiByte(mAtaCmd.ComID);
    buf[1]=LoByte(mAtaCmd.ComID);
    //buf[2]=0;
    //buf[3]=0;

    if(mHandleComIDRequest==2)
    {
        //buf[4]=0;                     //Request Code
        //buf[5]=0;
        //buf[6]=0;
        buf[7]=2;

        //buf[8]=0;                     //Reserved
        //buf[9]=0;

        //buf[10]=0;                     //Available Data Length
        buf[11]=4;

        if(mHandleComIDResponse!=0x00)
            buf[15]=1;          //Failure
    }
    else
    { //No Response Available
        buf[0]=HiByte(mAtaCmd.ComID);
        buf[1]=LoByte(mAtaCmd.ComID);
    }

    mHandleComIDRequest = 0;
}

U16 TcgTperReset(void)
{
soutb3(0x5C, 0xFE, 0x54);
    TCG_PRINTF("TPer_Reset: ");

    if(G1.b.mAdmTPerInfo_Tbl.val[0].preset==TRUE)
    {
soutb(0x00);
        TCG_PRINTF("OK\n");
        if(mSessionManager.state==SESSION_START)
            ResetSessionManager();

        HostPropertiesReset();

        LockingTbl_Reset(Programmatic);     // LckLocking_Tbl "ProgrammaReset"
        LockingRangeTable_Update();         //Update Read/Write LockedTable for Media Read/Write control

        MbrCtrlTbl_Reset(Programmatic);

        gTcgCmdState=ST_AWAIT_IF_SEND;

        return TRUE;
    }
    else
    {
soutb(0xff);
        TCG_PRINTF("NG\n");
        return FALSE;
    }
}

#if CO_SUPPORT_BLOCK_SID_AUTH
U16 TcgBlkSIDAuthentication(HcmdQ_t* pHcmdQ, U8 *buf)
{
    U16 status;
    TCG_PRINTF("BlkSIDAuth: ");

    if(mTcgStatus&SID_VALUE_CHANGED){
        status = STS_SUCCESS;
    }
    else if(mTcgStatus&SID_BLOCKED) {
        status = STS_INVALID_PARAMETER;
    }
    else {
        mTcgStatus |= SID_BLOCKED;

        if(siloIdxCmd==0)   //normal IF-SEND
            HostTx2TcgBuf(pHcmdQ, buf);

        if(buf[0]&0x01)
            mTcgStatus |= SID_HW_RESET;

        status = STS_SUCCESS;
        //gTcgCmdState=ST_AWAIT_IF_SEND;
    }

    return status;
}
#endif
//reset TPer's knowledge of Host Properties...
void HostPropertiesReset(void)
{
    //reset TPer's knowledge of Host Properties...
    mHostProperties[0].val = HOST_MAX_COMPKT_SZ;
    mHostProperties[1].val = HOST_MAX_PKT_SZ;
    mHostProperties[2].val = HOST_MAX_INDTKN_SZ;
    mHostProperties[3].val = 1;
    mHostProperties[4].val = 1;
    mHostProperties[5].val = 1;
}

void TcgComReset(void)
{
    if(mSessionManager.state==SESSION_START)
        ResetSessionManager();

    gTcgCmdState=ST_AWAIT_IF_SEND;
}

// Do Hash of "src" and copy the result to "dest", B0 of "dest" is the length.
void CopyHashed(U8 *src, U8 len, U8 *dest)
{
#if 1
    U32 tmp32;

    //len = *src;
    *dest = len;   //length

    //++src;
    ++dest;

    tmp32 = *(U32*)src;

//sha256(src, len, dest);
//TCG_PRINTF("H0: %08X\n", *(U32*)dest);

    tmp32 ^= mCPinPatch;
    *(U32*)src = tmp32;

    //sha256(src, len, dest);
    HAL_Sha256(src, len, dest);
TCG_PRINTF("H: %08X\n", *(U32*)dest);

#else
    U8 k;

    sha256(&G1.b.mAdmCPin_Tbl.val[1].pwd[1], G1.b.mAdmCPin_Tbl.val[1].pwd[0], digest);    //MSID
    soutb3(0x5C,0xFE,0xF2);
    soutb(digest[0]); soutb(digest[1]); soutb(digest[2]); soutb(digest[3]);

    G1.b.mAdmCPin_Tbl.val[0].pwd[0] = G1.b.mAdmCPin_Tbl.val[1].pwd[0];    //length

    for(k=0;k<SHA256_DIGEST_SIZE;k++)
        G1.b.mAdmCPin_Tbl.val[0].pwd[k+1]=digest[k];  //SID
#endif
}

/*******************************************************************
*   TrustedReceive
********************************************************************/
void Nvme2Sata_TrustedReceive(HcmdQ_t* pHcmdQ)
{
    NvmeAdminSecurityReceiveCommand_t* pCdb;

    pHcmdQ->attr.b.autoEoC = FALSE;    // Don't auto command complete when end of trandfer
    if(TableError == TRUE){
        soutb3(0x5C, 0xFE, 0x98);  //5C FE 98,    "!!! table NG !!! PreFormat is Required."
        TCG_PRINTF("!!!table NG!!! PreFormat is Required.\n");
        pHcmdQ->error = cEcInvalidCommand;
        return;
    }

    pCdb = (NvmeAdminSecurityReceiveCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);

    mAtaCmd.cmd        = ATA_TRUSTED_RECEIVE;
    mAtaCmd.ProtocolID = pCdb->SECP;
    mAtaCmd.ComID      = pCdb->SPSP;
    mAtaCmd.length     = pCdb->AL;
    pHcmdQ->error      = cEcNoError;

    TcgCmdProcessing(pHcmdQ);
    if(pHcmdQ->error == cEcNoError){
#if _TCG_ == TCG_EDRV
        if(siloIdxCmd)
        { // 1667 cmd (P_IN) processing...
            //if status is SPOUT/SPIN Sequence Rejection, 1667 stays in "Wait for SPIN" => don't clear siloIdxCmd
            if(m1667Status!=STS_1667_SP_SEQUENCE_REJECTION)
                siloIdxCmd = 0;
        }
#endif
        SendHostSectorCnt=mAtaCmd.length;    //(tmp+511)/512;

        pHcmdQ->attr.b.xferDir         = cXferD2h;                  // Receive (D -> H)
        pHcmdQ->bufDesc.ssdIndex       = SSD_TCG_START;
        smSSD[SSD_TCG_START].startAddr =  ((uint32_t)TcgHstTxRxBuf                      >> SSD_ADDR_SHIFT);
        smSSD[SSD_TCG_START].endAddr   = (((uint32_t)TcgHstTxRxBuf + SendHostSectorCnt) >> SSD_ADDR_SHIFT) - 1;
        smSSD[SSD_TCG_START].prevIndex = SSD_TCG_START;
        smSSD[SSD_TCG_START].nextIndex = SSD_TCG_START;

        AplHost_TransferWholeData_WithPRPList(pHcmdQ, TcgHstTxRxBuf, SendHostSectorCnt, TRUE);
    }

    Host_CommandComplete(pHcmdQ);
}

/*******************************************************************
*  TrustedSend
********************************************************************/
void Nvme2Sata_TrustedSend(HcmdQ_t* pHcmdQ)
{
    NvmeAdminSecuritySendCommand_t* pCdb;

    pHcmdQ->attr.b.autoEoC = FALSE;     // Don't auto command complete when end of trandfer
    if(TableError == TRUE){
        soutb3(0x5C, 0xFE, 0x98);  //5C FE 98,    "!!! table NG !!! PreFormat is Required."
        TCG_PRINTF("!!!table NG!!!PreFormat is Req.\n");
        pHcmdQ->error = cEcInvalidCommand;
        return;
    }

    pCdb = (NvmeAdminSecuritySendCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);

    mAtaCmd.cmd        = ATA_TRUSTED_SEND;
    mAtaCmd.ProtocolID = pCdb->SECP;
    mAtaCmd.ComID      = pCdb->SPSP;
    mAtaCmd.length     = pCdb->TL;
    pHcmdQ->error      = cEcNoError;

    TcgCmdProcessing(pHcmdQ);
    Host_CommandComplete(pHcmdQ);
}

/***********************************************************
 * TcgCmdProcessing:
 *   Trusted Send/Receive command blocks are handled here.
 *   They are processed in 3 + 1 (power off) State.
 *   Check Core 3.3.10.4 for Command State Transition.
 *
 * return 0 if success
 *
 * TODO: error handling, reset handling, process for different ProtocolID ...
 ***********************************************************/
void TcgCmdProcessing(HcmdQ_t* pHcmdQ)
{
    //TCG_PRINTF("Cmd: %2X PID: %2X ComID: %4X Len: %X\n", mAtaCmd.cmd, mAtaCmd.ProtocolID, mAtaCmd.ComID, mAtaCmd.length);

#if 1  //_TCG_ == TCG_EDRV
    if(mAtaCmd.length == 0){
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry); //cEcRxFisSize;
        return;
    }
#endif

#if _TCG_ == TCG_EDRV
    if (mAtaCmd.ProtocolID == TCG_SSC_EDRIVE) {
        //if(EEPROM_data.nor.element.ATA_data.TCG_DEV_TYPE == _eDRIVER)
            SiloCmdProcessing(pHcmdQ);
        //else  pHcmdQ->error = cEcInvalidCommand;
    }
    else
#endif
    {
        U16 result;
        result = TperProcessing(pHcmdQ);
        if(result!=STS_TPER_SUCCESS) {
            switch(result) {
               case STS_TPER_SYNC_PROTOCOL_VIOLATION:
                  pHcmdQ->error = MK_NVME_ERROR_CODE(cCommandSequenceError, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
                  break;
               default:
                  pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
                  break;
            }
        }
    }

    return;

}

// transfer TCG payload from host to buf[]
void HostTx2TcgBuf (HcmdQ_t* pHcmdQ, U8* buf)
{
    U32 XferErr = FALSE;

    HERE(mAtaCmd.length);
    pHcmdQ->attr.b.xferDir         = cXferH2d;                  // Receive (H -> D)
    pHcmdQ->bufDesc.ssdIndex       = SSD_TCG_START;
    smSSD[SSD_TCG_START].startAddr =   (uint32_t)buf                   >> SSD_ADDR_SHIFT;
    smSSD[SSD_TCG_START].endAddr   = (((uint32_t)buf + mAtaCmd.length) >> SSD_ADDR_SHIFT) - 1;

    XferErr = AplHost_TransferWholeData_WithPRPList(pHcmdQ, buf, mAtaCmd.length, TRUE);

    if(XferErr){
        pHcmdQ->error = cEcTxDataFis;
    }
}

#if _TCG_ == TCG_EDRV
//
// SiloCmdProcessing() will deal with 1667 commands (protocol 0xEE)
//
// It supports:
// (1) Probe Silo with ProbeCmd, and
// (2) TCG Silo with (a) GetSiloCap cmd, (b) Transfer cmd, (c) Reset cmd, and (4) GetTransferResult cmd.
void SiloCmdProcessing(HcmdQ_t* pHcmdQ)
{
    U8 index;

    //P-OUT (IF-SEND) : ATA_TRUSTED_SEND / ATA_TRUSTED_SEND_DMA
    if ((mAtaCmd.cmd == ATA_TRUSTED_SEND) || (mAtaCmd.cmd == ATA_TRUSTED_SEND_DMA))
    {

        //if(mAtaCmd.length>(MAX_COMPKT_SZ/512))
            //P_LENGTH > max buffer size
            // 1. receive and discard, 2. status code
            //m1667Status=STS_1667_INV_TX_LEN_ON_POUT;
        //else
        {
            HostTx2TcgBuf(pHcmdQ, TcgHstTxRxBuf);

            siloIdxCmd = mAtaCmd.ComID;
            mPOutLength = mAtaCmd.length;
        }
soutb2(0x5c, 0x03); soutb2(mAtaCmd.cmd, mAtaCmd.length); soutw(siloIdxCmd);
        TCG_PRINTF(">> P_OUT Cmd=%02X BC=%04X SiloIdxCmd=%04X\n",mAtaCmd.cmd, mAtaCmd.length, siloIdxCmd);
        return;
    }

    //processing IF-RECV (P_IN) ...
    index = HiByte(mAtaCmd.ComID);
    m1667Status = STS_1667_SUCCESS;

soutb2(0x5c, 0x04); soutb2(mAtaCmd.cmd, mAtaCmd.length); soutw(siloIdxCmd);
    TCG_PRINTF("<< P_IN  Cmd=%02X BC=%04X SiloIdxCmd=%04X\n",mAtaCmd.cmd, mAtaCmd.length, siloIdxCmd);
    if (index==0x00)
    { // Probe Silo

        if(siloIdxCmd!=mAtaCmd.ComID)
            m1667Status = STS_1667_SP_SEQUENCE_REJECTION;
        else if(siloIdxCmd!=TCG_1667_ProbeCmd)
            m1667Status = STS_1667_RESERVED_FUNCTION;
        else if(mPOutLength>(MAX_COMPKT_SZ+512))
            m1667Status = STS_1667_INV_TX_LEN_ON_POUT;
        else
        {
            mPCLength = ((U32)TcgHstTxRxBuf[0]<<24) | ((U32)TcgHstTxRxBuf[1]<<16) | ((U32)TcgHstTxRxBuf[2]<<8) | (U32)TcgHstTxRxBuf[3];
            if(mPOutLength<mPCLength)
                m1667Status = STS_1667_INCOMPLETE_COMMAND;
        }

        if(m1667Status==STS_1667_SUCCESS)
        {
            ProbeSilo_Probe(TcgHstTxRxBuf, mAtaCmd.length);    //Probe cmd processing
            return;
        }
    }
    else if(index==0x01)
    { // TCG Silo

        //check if ProbeCmd is run first...

        //mPLength = mAtaCmd.length;

        if(b1667Probed==0)
            m1667Status = STS_1667_NO_PROBE;
        else if(siloIdxCmd!=mAtaCmd.ComID)  // 2
            m1667Status = STS_1667_SP_SEQUENCE_REJECTION;
        else if((siloIdxCmd<TCG_1667_GetSiloCap) || (siloIdxCmd>TCG_1667_TPerReset /*TCG_1667_GetResult */))
            m1667Status = STS_1667_RESERVED_FUNCTION;   // 3
        else if(mPOutLength>(MAX_COMPKT_SZ+512))    //cj: MAX_COMPKT_SZ + 0x20 for NVMe, 4
            m1667Status = STS_1667_INV_TX_LEN_ON_POUT;
        else // 5
        {
            mPCLength = ((U32)TcgHstTxRxBuf[0]<<24) | ((U32)TcgHstTxRxBuf[1]<<16) | ((U32)TcgHstTxRxBuf[2]<<8) | TcgHstTxRxBuf[3];

            if(mPOutLength<mPCLength)
                m1667Status = STS_1667_INCOMPLETE_COMMAND;
        }

        if(m1667Status==STS_1667_SUCCESS)
        {
            switch(siloIdxCmd)
            {
                case TCG_1667_GetSiloCap:
                    TcgSilo_GetSiloCap(TcgHstTxRxBuf, TCG_BUF_LEN);
                    return;

                case TCG_1667_Transfer:
                    TcgSilo_Transfer(pHcmdQ, TcgHstTxRxBuf, TCG_BUF_LEN);
                    return;

                case TCG_1667_Reset:
                    TcgSilo_Reset(pHcmdQ, TcgHstTxRxBuf, TCG_BUF_LEN+512);
                    return;

                case TCG_1667_GetResult:
                    TcgSilo_GetResult(pHcmdQ, TcgHstTxRxBuf, TCG_BUF_LEN);
                    return;

                case TCG_1667_TPerReset:
                    TcgSilo_TPerReset(TcgHstTxRxBuf, TCG_BUF_LEN);
                    return;
            }
        }
    }
    else
    { // Error Silo
        m1667Status = STS_1667_INV_SILO;
    }

    //return error status
    if(m1667Status!=STS_1667_SUCCESS)
    {
soutb3(0x5C, 0x05, m1667Status);
        TCG_PRINTF("!!P_IN NG: %02X\n",m1667Status);
        MEM_SET(TcgHstTxRxBuf, 0x00, TCG_BUF_LEN);   //clear send buffer

        // set Payload Content Length
        if(mAtaCmd.length<8)
            TcgHstTxRxBuf[3]=mAtaCmd.length;
        else
        TcgHstTxRxBuf[3]=8;

        TcgHstTxRxBuf[7]=m1667Status;
    }

    //else         siloIdxCmd = 0;

    return;
}

void ProbeSilo_Probe(U8* buf, U32 len)
{
    U8 i, specLen;
    U32 siloList=0;

    //TODO: check Payload Content Length?

soutb3(0x5C, 0xFE, 0x60);
    TCG_PRINTF("ProbeSilo_Probe...\n");
    for(i=0; i<16; i++)
        TCG_PRINTF("%02X ", buf[i]);
    TCG_PRINTF("\n");

    alexdebug1s(buf[8],1667_Major_Ver);
    alexdebug1s(buf[9],1667_Minor_Ver);
    alexdebug1s(buf[12],Host_OS);
    alexdebug1s(buf[13],Host_Spec_Len);
    alexdebug1s(buf[14],Host_Major_Ver);
    alexdebug1s(buf[15],Host_Minor_Ver);

    specLen=buf[13];
    for(i=0;i<specLen;i++){
        soutb(buf[i+16]);
        TCG_PRINTF("%2X ", buf[i+16]);
    }
    TCG_PRINTF("\n");

    //TODO: Success (0x00) or Default Behavior (0x01) or Failure (0x80) or Unsupported Host Version (0x81) ...
    MEM_SET(buf, 0x00, len);   //clear send buffer
    buf[7]=STS_1667_SUCCESS;

    //Silo List: Probe Silo
    // A.6.2.2.1,  0~27: reserved
    //                 28-31: STID
    //                       32: SiloType Spec Major Version
    //                       33:                       Minor Version
    //                       34: SiloType Impl  Major Version
    //                       35:                       Minor Version
    siloList++;
    i=14+28+2;
    buf[i++]=0x01;  //B28~31: STID=0x0100
    buf[i++]=0x00;

    buf[i++]=0x01;  //B32: Silo Type Spec Major Ver
    buf[i++]=0x00;  //B33: Silo Type Spec Minor Ver

    buf[i++]=0x01;  //B34: Silo Type Implement Major Ver
    buf[i++]=0x00;  //B35: Silo Type Implement Minor Ver

    //Silo List: TCG Silo
    siloList++;
    i+=(28+2);
    buf[i++]=0x01;  //B28~31: STID=0x0104
    buf[i++]=0x04;

    buf[i++]=0x01;  //B32: Silo Type Spec Major Ver
    buf[i++]=0x00;  //B33: Silo Type Spec Minor Ver

    buf[i++]=0x01;  //B34: Silo Type Implement Major Ver
    buf[i++]=0x00;  //B35: Silo Type Implement Minor Ver

    //update length
    buf[13]=36*siloList;   //B12~13: Silo List Length

    buf[3]=i;       //B0~3: Payload Content Length

    if(len < (36*siloList))
        buf[11] = 36*siloList + 14;     //B8~B11: Available Payload Length

    b1667Probed=1;

    return;
}

// instead of call IF-SEND/IF_RECV commands (TperProcessing()), I called "Level0_Discovery()" directly here.
void TcgSilo_GetSiloCap(U8* buf, U32 len)
{
    U16 len16;

soutb3(0x5C, 0xFE, 0x61);
    TCG_PRINTF("TcgSilo_GetSiloCap...\n");
    MEM_SET(buf, 0x00, len);   //clear send buffer
    if(mPCLength!=8)
    {
        buf[3]=8;
        buf[7]=STS_1667_INCONSISTENT_PCLENGTH;

        return;
    }

    SiloComID=BASE_COMID;
    buf[7]=STS_1667_SUCCESS;

    buf[12]=(U8)(BASE_COMID/256);
    buf[13]=(U8)BASE_COMID;

    buf[18]=(U8)((MAX_COMPKT_SZ)/256);   //B16~19: Max P_OUT transfer size
    buf[19]=(U8)(MAX_COMPKT_SZ);

    len16=Level0_Discovery((U8 *)&buf[32]);

    buf[30]=(U8)(len16/256);           //B28~31: Level0 length
    buf[31]=(U8)len16;

    len16 += 32;

    if(len16>mAtaCmd.length){
      buf[10]=(U8)(len16/256);         //B8~11: Available Payload Length
      buf[11]=(U8)len16;
    }

    buf[2]=(U8)(len16/256);            //B0~3: Payload Content Length
    buf[3]=(U8)len16;

    return ;
}

void TcgSilo_Transfer(HcmdQ_t* pHcmdQ, U8* buf, U32 len)
{
    U8  errorCode = 0;
    U16 result=0;
    U32 tmp1, tmp2=0;

soutb3(0x5C, 0xFE, 0x62);
    TCG_PRINTF("TcgSilo_Transfer...");
    //TODO: check A.X.5.4 item 1,2,3,4 first...
    //1. contain a Complet ComPacket... check outstanding data??
    //    buf[7]= STS_1667_INCOMPLETE_COMMAND;

    //2. ComID check, should be the same as the ComID @ ATA register
    result = ((U16)buf[4+0x20]<<8) | (U16)buf[5+0x20];
    if((result!=SiloComID))
    {
        errorCode=STS_1667_INV_TCG_COMID;
        TCG_PRINTF("!!NG Invalid ComID: %4X\n", result);
        goto ERROR_STATUS;
    }

    // ComID Ext = 0
    result = ((U16)buf[6+0x20]<<8) | (U16)buf[7+0x20];
    if(result) //!=0
    {
        errorCode=STS_1667_INV_TCG_COMID;
        TCG_PRINTF("!!NG ComIDExt!=0 %4X\n", result);
        goto ERROR_STATUS;
    }

    //3. Length check
    //Length of TCGComPacket:
    tmp1 = ((U32)buf[28]<<24) | ((U32)buf[29]<<16) + ((U32)buf[30]<<8) + (U32)buf[31];

    //ComPkt.length:
    tmp2 = ((U32)buf[0x10+0x20]<<24) | ((U32)buf[0x11+0x20]<<16) + ((U32)buf[0x12+0x20]<<8) + (U32)buf[0x13+0x20];

    if((tmp2+0x20)>mPOutLength){ //ComPacket Length > Xfer data length
        errorCode=STS_1667_INCOMPLETE_COMMAND;
        TCG_PRINTF("!!NG ComPkt.len (%X) > mPOutLen (%X)\n", tmp2, tmp1);
        goto ERROR_STATUS;
    }
    else if(tmp2>MAX_COMPKT_SZ) {
        errorCode = STS_1667_INV_TX_LEN_ON_POUT;
        TCG_PRINTF("!!NG ComPkt.len (%X)\n", tmp2);
        goto ERROR_STATUS;
    }

    if((mPCLength!=(tmp1+0x20)) || (tmp1!=(tmp2+0x14))){ // if(mPCLengh>mPOutLength) ??
        //errorCode=STS_1667_INV_PARAMETER;
        //goto ERROR_STATUS;
    }

    TCG_PRINTF("PCLen: %4X LenOfTcgComPkt: %4X ComPkt.len: %4x\n", mPCLength, tmp1, tmp2);

    //4. P_IN Transfer Length check
    if((mAtaCmd.length <(0x20+0x14))
     //(mAtaCmd.length > (MAX_COMPKT_SZ+0x20))
     )
    {
        errorCode=STS_1667_INV_PARAMETER_LENGTH;
        TCG_PRINTF("!!NG P_IN TxLen < 52\n");
        goto ERROR_STATUS;
    }

    //5. IF-SEND
    mAtaCmd.cmd=ATA_TRUSTED_SEND_DMA;
    mAtaCmd.ProtocolID=0x01;
    mAtaCmd.ComID=BASE_COMID;

    errorCode = TperProcessing(pHcmdQ);
    if(errorCode!=STS_1667_SUCCESS)
    {
        //6. TPer Error
        if((errorCode!=STS_1667_TCG_SYNC_VIOLATION)&&(errorCode!=STS_1667_INV_TX_LEN_ON_POUT))
            errorCode=STS_1667_INV_TCG_COMID;

TCG_PRINTF("!!NG Send: %02X\n", errorCode);
        goto ERROR_STATUS;
    }

    //7. IF-RECV
    mAtaCmd.cmd=ATA_TRUSTED_RECEIVE_DMA;
    mAtaCmd.ProtocolID=0x01;
    mAtaCmd.ComID=BASE_COMID;

    errorCode = TperProcessing(pHcmdQ);     //cj:  "mAtaCmd.length -= 32; " ??
    if(errorCode!=STS_1667_SUCCESS)
    {
        //8. TPer Error
        if((errorCode!=STS_1667_TCG_SYNC_VIOLATION)&&(errorCode!=STS_1667_INV_TX_LEN_ON_POUT))
            errorCode=STS_1667_INV_TCG_COMID;
TCG_PRINTF("!!NG Recv: %02X\n", errorCode);
        goto ERROR_STATUS;
    }
    else
    { //9. successfully return RespComPkt
        U32 pcLen;

        pcLen = rcvCmdPktLen+0x20;
        if(pcLen > mAtaCmd.length)
        { //c. packet size is larger than host request...
            buf[0]=0;       //Payload Content Length
            buf[1]=0;
            buf[2]=(U8)(mAtaCmd.length>>8);
            buf[3]=(U8)mAtaCmd.length;

            // set Available Payload Length to data bytes available
            buf[8]=HiByte(HiWord(pcLen));
            buf[9]=LoByte(HiWord(pcLen));
            buf[10]=HiByte(LoWord(pcLen));
            buf[11]=LoByte(LoWord(pcLen));
        }
        else
        { //d.
            buf[0]=HiByte(HiWord(pcLen));
            buf[1]=LoByte(HiWord(pcLen));
            buf[2]=HiByte(LoWord(pcLen));
            buf[3]=LoByte(LoWord(pcLen));

            buf[8]=0;
            buf[9]=0;
            buf[10]=0;
            buf[11]=0;
        }

        buf[28]=0;
        buf[29]=0;
        buf[30]=HiByte(rcvCmdPktLen);
        buf[31]=LoByte(rcvCmdPktLen);

        buf[7]=STS_1667_SUCCESS;
    }

    return;

ERROR_STATUS:
    TCG_PRINTF("\n");
    MEM_SET(buf, 0x00, len);   //clear send buffer

    // set Payload Content Length
    if(mAtaCmd.length<8)
        buf[3]=mAtaCmd.length;
    else
        buf[3]=8;

    buf[7] = errorCode;

    return;
}

// instead of call IF-SEND/IF_RECV commands (TperProcessing()), I called "HandlecomIDRequest()" directly here.
// Modified: call stack reset function directly.
void TcgSilo_Reset(HcmdQ_t* pHcmdQ, U8* buf, U32 len)
{
    soutb3(0x5C, 0xFE, 0x63);
    TCG_PRINTF("TcgSilo_Reset...\n");
    MEM_SET(buf, 0x00, len);   //clear send buffer
#if 0   //not contain a valid ComID???
    if(SiloComID==0x00)
    {
        buf[3]=8;
        buf[7]=STS_1667_SUCCESS;
    }
    else
#endif
    if(mPCLength!=8)
    {
        buf[3]=8;
        buf[7]=STS_1667_INCONSISTENT_PCLENGTH;
    }
    else
    {
    #if 0
        //mAtaCmd.cmd=ATA_TRUSTED_SEND_DMA;
        //mAtaCmd.ProtocolID=0x02;
        //mAtaCmd.ComID=BASE_COMID;

        buf[0]=(U8)(BASE_COMID/256);
        buf[1]=(U8)(BASE_COMID);
        buf[7]=0x02;
        //result = TperProcessing();
        HandleComIDRequest(buf);

        //mAtaCmd.cmd=ATA_TRUSTED_RECEIVE_DMA;
        //mAtaCmd.ProtocolID=0x02;
        //mAtaCmd.ComID=BASE_COMID;
        //result = TperProcessing();
    #endif
        TcgStackReset();
        mHandleComIDRequest=0;

        buf[0]=0;
        buf[1]=0;
        buf[2]=0;
        buf[3]=8;
        buf[7]=STS_1667_SUCCESS;
    }

    return ;
}

void TcgSilo_TPerReset(U8* buf, U32 len)
{
    soutb3(0x5C, 0xFE, 0x63);
    TCG_PRINTF("TcgSilo_TPerReset...\n");
    MEM_SET(buf, 0x00, len);   //clear send buffer
    alexdebugws(mPCLength,mPCLength);
    if(mPCLength != 8)
    {
        buf[3]=8;
        buf[7]=STS_1667_INCONSISTENT_PCLENGTH;
    }
    else
    {
        buf[0]=(U8)(BASE_COMID/256);
        buf[1]=(U8)(BASE_COMID);
        buf[2]=0;
        buf[3]=8;
        if(TcgTperReset() == FALSE){
            buf[7] = STS_1667_FAILURE;
            alexdebugs(STS_1667_FAILURE);
        }else{
            buf[7] = STS_1667_SUCCESS;
            alexdebugs(STS_1667_SUCCESS);
        }
    }
    return ;
}

void TcgSilo_GetResult(HcmdQ_t* pHcmdQ, U8* buf, U32 len)
{
    U16 result=0;
    soutb3(0x5C, 0xFE, 0x64);
    TCG_PRINTF("TcgSilo_GetResult...\n");
    if(mPCLength!=8)
    {
        MEM_SET(buf, 0x00, len);   //clear send buffer
        buf[3]=8;
        buf[7]=STS_1667_INCONSISTENT_PCLENGTH;
        return;
    }

    if(mAtaCmd.length <= (0x20+0x14))  //i.e., length=0
    {
        MEM_SET(buf, 0x00, len);   //clear send buffer
        buf[3]=8;
        buf[7]=STS_1667_INV_PARAMETER_LENGTH;
        return;
    }

    mAtaCmd.cmd=ATA_TRUSTED_RECEIVE_DMA;
    mAtaCmd.ProtocolID=0x01;
    mAtaCmd.ComID=SiloComID;

    result = TperProcessing(pHcmdQ);
    if(result!=STS_1667_SUCCESS)
    { // return 8 byte status header
        if((result!=STS_1667_TCG_SYNC_VIOLATION)&&(result!=STS_1667_INV_TX_LEN_ON_POUT))
            result=STS_1667_INV_TCG_COMID;
        MEM_SET(buf, 0x00, len);   //clear send buffer
        buf[3]=8;
        buf[7]=result;
    }
    else
    { // successfully return RespComPkt
        U32 pcLen;

        pcLen = rcvCmdPktLen+0x20;
        if(pcLen > mAtaCmd.length)
        { //packet size is larger than host request...
            buf[0]=0;       //Payload Content Length
            buf[1]=0;
            buf[2]=(U8)(mAtaCmd.length>>8);
            buf[3]=(U8)(mAtaCmd.length);

            buf[8]=HiByte(HiWord(pcLen));
            buf[9]=LoByte(HiWord(pcLen));
            buf[10]=HiByte(LoWord(pcLen));
            buf[11]=LoByte(LoWord(pcLen));
        }
        else
        {
            buf[0]=HiByte(HiWord(pcLen));
            buf[1]=LoByte(HiWord(pcLen));
            buf[2]=HiByte(LoWord(pcLen));
            buf[3]=LoByte(LoWord(pcLen));

            buf[8]=0;
            buf[9]=0;
            buf[10]=0;
            buf[11]=0;
        }

        buf[28]=0;
        buf[29]=0;
        buf[30]=HiByte(rcvCmdPktLen);
        buf[31]=LoByte(rcvCmdPktLen);

        buf[7]=STS_1667_SUCCESS;
    }

    return ;
}

#endif

//
// TperProcessing() is splitted from original TcgCmdProcessing() due to 1667 requirement.
//
// It will handle TCG defined IF-SEND/IF-RECV commands, and return TCG command processing
// result according to the 1667 definition.
//
// For host payload from IF-SEND, if it is 1667 (0xEE) protocol, then it is transferred in
// SiloCmdProcessing() and the TCG ComPkt is decoded here with 0x20 byte offset (1667 header size).
//
// For TPer response data (for IF-RECV), if it is 1667 (0xEE) protocol, then it is encoded with
// 0x20 byte offset.
U16 TperProcessing(HcmdQ_t* pHcmdQ)   //mtCMDQ_ENTRY *psNewCmdQentry)
{
    U16 result=STS_TPER_SUCCESS;

    //check ATA registers first
    if((mAtaCmd.cmd==ATA_TRUSTED_RECEIVE)||(mAtaCmd.cmd==ATA_TRUSTED_RECEIVE_DMA))
    {
        soutb3(0x5C, 0x01, mAtaCmd.cmd);
        soutb2(mAtaCmd.ProtocolID, mAtaCmd.length); soutw(mAtaCmd.ComID);
        TCG_PRINTF("<= TCG CMD=%02X PID=%02X ByteCnt=%04X ComID=%04X\n",mAtaCmd.cmd, mAtaCmd.ProtocolID, mAtaCmd.length, mAtaCmd.ComID);

        if(mAtaCmd.length==0)
            return STS_TPER_OTHER_INVALID_CMD_PARAM; //STS_1667_INV_TCG_COMID;

        MEM_SET(TcgHstTxRxBuf, 0x00, sizeof(TcgHstTxRxBuf));   //clear send buffer

        if(mAtaCmd.ProtocolID==0x01)
        {
            if(mAtaCmd.ComID==0x0001)
            {   // Level0 discovery
                //put response data to ATA buffer directly...
                Level0_Discovery(TcgHstTxRxBuf);

                if(tgtComID==0x0001) // SEND-RECV pair
                gTcgCmdState = ST_AWAIT_IF_SEND;

                return result;
            }
            //else if(mAtaCmd.ComID!=BASE_COMID)
            else if(mAtaCmd.ComID!=tgtComID)
            {
                //psNewCmdQentry->mErrorCode=mEC_INV_COMMAND;
                return STS_TPER_OTHER_INVALID_CMD_PARAM; //STS_1667_INV_TCG_COMID;
            }

            //TODO: if gTcgCmdState=ST_AWAIT_IF_SEND, return "no further data"
        }
        else if(mAtaCmd.ProtocolID==0x00)
        { // check ATA8
            if(mAtaCmd.ComID==0x0000)
            {   // Supported Protocols...
                //put response data to ATA buffer directly...
                Supported_Security_Protocol(TcgHstTxRxBuf);

                //gTcgCmdState = ST_AWAIT_IF_RECV;
                return result;
            }
            else if(mAtaCmd.ComID==0x0001)
            {   // Certificate Data... no data

                //gTcgCmdState = ST_AWAIT_IF_RECV;
                return result;  //STS_1667_INV_TCG_COMID;
            }
            else
                return STS_TPER_OTHER_INVALID_CMD_PARAM;    //STS_1667_INV_TCG_COMID;

        }
        else if(mAtaCmd.ProtocolID==0x02)
        { //TODO?
            if((mAtaCmd.ComID==BASE_COMID) && (mAtaCmd.length))  //(mAtaCmd.length==1)??
            {
                HandleComIDResponse(TcgHstTxRxBuf);     //deal with STACK_RESET only, Core 3.3.4.7.5
                return result;
            }
            else
            { // GetComID, not supported
                soutb3(0x5C, 0x03, 0x02);  soutw(mAtaCmd.ComID);
                TCG_PRINTF("ComID not supported %4X\n", mAtaCmd.ComID);
                return STS_TPER_OTHER_INVALID_CMD_PARAM;    //STS_1667_INV_TCG_COMID;
            }
        }
        else
        { //ProtocolID!=0,1,2
            soutb3(0x5C, 0x03, mAtaCmd.ProtocolID);  soutw(mAtaCmd.ComID);
            TCG_PRINTF("PID not supported %2X %4X\n",mAtaCmd.ProtocolID, mAtaCmd.ComID);

            return STS_TPER_INVALID_SEC_PID_PARAM;    //STS_1667_INV_TCG_COMID;
        }
    }
    else
    { // mAtaCmd.cmd==ATA_TRUSTED_SEND || ATA_TRUSTED_SEND_DMA

        soutb3(0x5C, 0x00, mAtaCmd.cmd);
        soutb2(mAtaCmd.ProtocolID, mAtaCmd.length); soutw(mAtaCmd.ComID);
        TCG_PRINTF("=> TCG CMD=%02X PID=%02X ByteCnt=%04X ComID=%04X\n",mAtaCmd.cmd, mAtaCmd.ProtocolID, mAtaCmd.length, mAtaCmd.ComID);

        if(mAtaCmd.ProtocolID==0x02)
        {
            if((mAtaCmd.ComID==BASE_COMID) && (mAtaCmd.length))  //(mAtaCmd.length==1)??
            { //deal with STACK_RESET only, Core 3.3.4.7.5
                HandleComIDRequest(pHcmdQ, TcgHstTxRxBuf);
            }
            else if((mAtaCmd.ComID==0x0004) && mAtaCmd.length)
            { //TPer reset, Opal2 3.2.3
                if(TcgTperReset()==FALSE)
                    result = STS_TPER_OTHER_INVALID_CMD_PARAM;  //STS_1667_INV_TCG_COMID;
            }
#if CO_SUPPORT_BLOCK_SID_AUTH
            else if((mAtaCmd.ComID==0x0005) && mAtaCmd.length)
            {
                if(TcgBlkSIDAuthentication(pHcmdQ, TcgHstTxRxBuf)!=STS_SUCCESS)
                    result = STS_TPER_OTHER_INVALID_CMD_PARAM;  //STS_1667_INV_PARAMETER;
            }
#endif
            else
                result = STS_TPER_OTHER_INVALID_CMD_PARAM;      //STS_1667_INV_TCG_COMID;

            return result;

        }
        else if(gTcgCmdState==ST_AWAIT_IF_RECV)
        {
            if(mAtaCmd.length)
                return STS_TPER_SYNC_PROTOCOL_VIOLATION;        //STS_1667_TCG_SYNC_VIOLATION
        }
        else if((mAtaCmd.length==0) || (mAtaCmd.length > MAX_COMPKT_SZ))   //add one sector for eDrive  //(MAX_COMPKT_SZ/512)))
        {
            if(siloIdxCmd)  //1667 protocol
            {
                if(mAtaCmd.length > (MAX_COMPKT_SZ+512)) //cj test only
                {
                    souts("!!NG SC");
                    TCG_PRINTF("!!NG SC\n");
                    return STS_TPER_INVALID_TX_PARAM_SEND;
                }
            }
            else   //TCG2.0 non-eDriver
            {
                souts("!!NG SC");
                TCG_PRINTF("!!NG SC\n");
                return  STS_TPER_INVALID_TX_PARAM_SEND; //STS_1667_INV_TCG_COMID;
            }
        }
        else if(mAtaCmd.ProtocolID!=0x01)
        {
            souts("!!NG PID");
            TCG_PRINTF("!!NG PID\n");
            return STS_TPER_INVALID_SEC_PID_PARAM;      //STS_1667_INV_TCG_COMID;
        }
        else if(mAtaCmd.ComID==0x0001)
        { //L0_Discovery for IF-Send: do nothing
            HostTx2TcgBuf(pHcmdQ, TcgHstTxRxBuf);
            tgtComID = 0x0001;
            gTcgCmdState = ST_AWAIT_IF_RECV;
            return STS_TPER_SUCCESS;
        }
        else if(mAtaCmd.ComID!=BASE_COMID)
        {
souts("!!NG ComID");
            TCG_PRINTF("!!NG ComID\n");
            return STS_TPER_OTHER_INVALID_CMD_PARAM;    //STS_1667_INV_TCG_COMID;
        }

        tgtComID = BASE_COMID;
    }

    //ProtocolID==1 for the following state processing :
    switch(gTcgCmdState){
    case ST_AWAIT_IF_SEND:
        if((mAtaCmd.cmd==ATA_TRUSTED_SEND)||(mAtaCmd.cmd==ATA_TRUSTED_SEND_DMA))
        {
            // Get data from host and store in ATACmdBuf

          #if _TCG_ == TCG_EDRV
            if(siloIdxCmd==0)   //normal IF-SEND (non-1667)
          #endif
                HostTx2TcgBuf(pHcmdQ, TcgHstTxRxBuf);

            gTcgCmdState = ST_PROCESSING;
        }
        else
        { //ATA_TRUSTED_RECEIVE
            TcgCmdPkt4NoData();
#if _TCG_ == TCG_EDRV
            if(siloIdxCmd) { //1667 format
                rcvCmdPktLen = 56;
                MEM_COPY(TcgHstTxRxBuf+0x20, (void*)&dataBuf, TCG_BUF_LEN);    //sector count?
            }
            else
#endif
            MEM_COPY(TcgHstTxRxBuf, (void*)&dataBuf, TCG_BUF_LEN);    //sector count?
            break;
        }

    case ST_PROCESSING:
#if _TCG_ == TCG_EDRV
        if(siloIdxCmd){   //1667 format
            //ErrStatus1667 = STS_1667_SUCCESS;
            result=TcgCmdPktParser(TcgHstTxRxBuf+0x20);
        }
        else
#endif
        result=TcgCmdPktParser(TcgHstTxRxBuf);


        if(result==STS_SESSION_ABORT)
        {
            //soutb3(0x5C, 0xFE, 0x32);   //!!NG:...

            ResetSessionManager();
            TcgAbortSession();
#if _TCG_ == TCG_EDRV
            if(siloIdxCmd) //&&(ErrStatus1667!=STS_1667_SUCCESS))
                TcgCmdPkt4CloseSession();
#endif
            gTcgCmdState = ST_AWAIT_IF_RECV;
            break;
        }
        else if(result==STS_STAY_IN_IF_SEND)
        {
            //soutb3(0x5C, 0xFE, 0x33);   //!!NG:...
            gTcgCmdState = ST_AWAIT_IF_SEND;
            break;
        }

        //prepare CmdPacket in dataBuffer[] (for IF-RECV)
        TcgCmdPkt4Response();

        if(result!=STS_SUCCESS)
        {
            dataBuf[iDataBuf++]=TOK_StartList;
            dataBuf[iDataBuf++]=TOK_EndList;
            dataBuf[iDataBuf++]=TOK_EndOfData;

            //add status to reponse buffer and update length
            statusCode[0]=result;
            statusCode[1]=0;
            statusCode[2]=0;
            TcgRespPktUpdate(TRUE);

            gTcgCmdState = ST_AWAIT_IF_RECV;
            break;
        }

        //Decode the payload, process the data, and prepare the response payload ...
        //  ex: SessionManager, SP, Table processing ...
        result=TcgStreamDecode();
        //soutw(result);
        if(result==STS_STAY_IN_IF_SEND)
        {
            soutb3(0x5C, 0xFE, 0x35);   //!!NG:...
            TCG_PRINTF("!!NG: StreamDecode NG -> Stay in IF-SEND\n");
            gTcgCmdState = ST_AWAIT_IF_SEND;
            break;
        }

        if(result==STS_SESSION_ABORT)
        { //prepare payload for "Close Session"
            soutb3(0x5C, 0xFE, 0x34);   //!!NG:...
            TCG_PRINTF("!!NG: StreamDecode NG -> Abort Session\n");
            ResetSessionManager();
            TcgAbortSession();
        }
        else if(result==STS_RESPONSE_OVERFLOW)
        {
            TcgCmdPkt4Response();
            dataBuf[iDataBuf++]=TOK_StartList;
            dataBuf[iDataBuf++]=TOK_EndList;
            dataBuf[iDataBuf++]=TOK_EndOfData;

        //add status to reponse buffer and update length
        statusCode[0]=result;
        statusCode[1]=0;
        statusCode[2]=0;
            TcgRespPktUpdate(TRUE);
        }

        gTcgCmdState = ST_AWAIT_IF_RECV;
        break;

    case ST_AWAIT_IF_RECV:
        if((mAtaCmd.cmd==ATA_TRUSTED_RECEIVE)||(mAtaCmd.cmd==ATA_TRUSTED_RECEIVE_DMA))
        { //if(IF-RECV is able to retrieve the entire response resulting from the IF-SEND ...

            //if xfer length is not enough...
            if(mAtaCmd.length < rcvCmdPktLen)
            {
#if _TCG_ == TCG_EDRV
              if(siloIdxCmd){  //1667 format
                MEM_COPY(TcgHstTxRxBuf+0x20, (void*)&dataBuf, 8);
                TcgHstTxRxBuf[10+0x20]=HiByte(rcvCmdPktLen);    //OutstandingData
                TcgHstTxRxBuf[11+0x20]=LoByte(rcvCmdPktLen);

                TcgHstTxRxBuf[14+0x20]=HiByte(rcvCmdPktLen);    //MinTransfer
                TcgHstTxRxBuf[15+0x20]=LoByte(rcvCmdPktLen);
                rcvCmdPktLen= 0x38;
              }
              else {
#endif
                MEM_COPY(TcgHstTxRxBuf, (void*)&dataBuf, 8);
                TcgHstTxRxBuf[10]=HiByte(rcvCmdPktLen);    //OutstandingData
                TcgHstTxRxBuf[11]=LoByte(rcvCmdPktLen);

                TcgHstTxRxBuf[14]=HiByte(rcvCmdPktLen);    //MinTransfer
                TcgHstTxRxBuf[15]=LoByte(rcvCmdPktLen);
#if _TCG_ == TCG_EDRV
              }
#endif
                gTcgCmdState = ST_AWAIT_IF_RECV;
                break;

            }

            //put response data to ATA buffer...
#if _TCG_ == TCG_EDRV
            if(siloIdxCmd)  //1667 format
                MEM_COPY(TcgHstTxRxBuf+0x20, (void*)&dataBuf, TCG_BUF_LEN);     //sector count?
            else
#endif
            MEM_COPY(TcgHstTxRxBuf, (void*)&dataBuf, TCG_BUF_LEN);    //sector count?

            gTcgCmdState = ST_AWAIT_IF_SEND;
        }
        else
        { //ATA_TRUSTED_SEND
            //error, still need to get data from host and store in ATACmdBuf!!
            /* mHostClrIntrStatus(m_tHBI_INT_bits_fwXferDone);
                        mHostSetBitAtMode (m_tHOST_BUS_INTERFACE_ATmode0_bits_writeDir);

                        // get the packet from host
                        mHostSetTaskFileReg(mTF_CYL_LOW_REG, 1);    // program sector counts
                        mHostSetTaskFileReg(mTF_CYL_HIGH_REG, 0x0); // program rd or wr direction, in this case, it is write
                        xferdone = 0;

                        // set up the buffer link list
                        bufAddr = mBfrGetBufferAdrsFromCPUdirectAccessAdrs((mtUP_ADDRESS)&ATACmdBuf);
                        mBfrForHost(bufAddr, 1, 0, mWRITE_OP);
                        mHBIsetCxsbFwReqNonQDataXfer(1, 0, mWRITE_OP, FALSE);
                        while (xferdone != 1)
                        {
                            if (mCurrentSystemState() != mPOWER_UP_STATE)
                            {
                                return -1;
                            }
                            mKrnlWaitForEvents(0);              // let others have the CPU while I am waiting
                        }     */
        }
        break;
    }

    return STS_TPER_SUCCESS; // zOK;
}

/***********************************************************
 *TcgCmdPktParser(BYTE* buf):
 *  IF-SEND data block is placed into structure "mCmdPkt"
 *  Packet format should be checked here.
 *
 *  If session has not started yet, HSN and TSN should be zero. HSN is initially
 *  transmitted in the StartSession method, and TSN is initially transmitted in
 *  the SyncSession method.
 *
 *  return STS_SUCCESS if no error. Check Test Cases 3.1.4
 ***********************************************************/
U16 TcgCmdPktParser(U8* buf)
{
    U16 result=STS_SUCCESS;
    U8  errcode=0x00;

    //buf to CmdPkt ...
    register U16 i=0;
    register U16 j=0;

    for(j=0;j<4;j++)
        mCmdPkt.rsv[j]=buf[i++];

    HiByte(mCmdPkt.ComID) = buf[i++];       //should be the same as the ComID @ ATA register
    LoByte(mCmdPkt.ComID) = buf[i++];
    if(mCmdPkt.ComID!=mAtaCmd.ComID)
    {   result=STS_STAY_IN_IF_SEND;     errcode=0x10;   goto EXIT0; }

    HiByte(mCmdPkt.ComIDExt) = buf[i++];    //0x0000 for static ComID
    LoByte(mCmdPkt.ComIDExt) = buf[i++];
    if(mCmdPkt.ComIDExt!=0x00)
    {   result=STS_STAY_IN_IF_SEND;     errcode=0x20;   goto EXIT0; }

    HiByte(HiWord(mCmdPkt.Outstanding)) = buf[i++];
    LoByte(HiWord(mCmdPkt.Outstanding)) = buf[i++];
    HiByte(LoWord(mCmdPkt.Outstanding)) = buf[i++];
    LoByte(LoWord(mCmdPkt.Outstanding)) = buf[i++];

    HiByte(HiWord(mCmdPkt.MinTx)) = buf[i++];
    LoByte(HiWord(mCmdPkt.MinTx)) = buf[i++];
    HiByte(LoWord(mCmdPkt.MinTx)) = buf[i++];
    LoByte(LoWord(mCmdPkt.MinTx)) = buf[i++];

    HiByte(HiWord(mCmdPkt.length)) = buf[i++];
    LoByte(HiWord(mCmdPkt.length)) = buf[i++];
    HiByte(LoWord(mCmdPkt.length)) = buf[i++];
    LoByte(LoWord(mCmdPkt.length)) = buf[i++];
    if(mCmdPkt.length > (U32)mAtaCmd.length)
    {
        result=STS_STAY_IN_IF_SEND;     errcode=0x30;
#if _TCG_ == TCG_EDRV
//        ErrStatus1667=STS_1667_INCOMPLETE_COMMAND;
#endif
        goto EXIT0;
    }

    if(mCmdPkt.length<24)
    {
        result=STS_STAY_IN_IF_SEND;     errcode=0x40;   goto EXIT0;
    }

    //PktFmt
    HiByte(HiWord(mCmdPkt.mPktFmt.TSN)) = buf[i++];     //should be zero if session not started yet
    LoByte(HiWord(mCmdPkt.mPktFmt.TSN)) = buf[i++];
    HiByte(LoWord(mCmdPkt.mPktFmt.TSN)) = buf[i++];
    LoByte(LoWord(mCmdPkt.mPktFmt.TSN)) = buf[i++];

    HiByte(HiWord(mCmdPkt.mPktFmt.HSN)) = buf[i++];     //should be zero if session not started yet
    LoByte(HiWord(mCmdPkt.mPktFmt.HSN)) = buf[i++];
    HiByte(LoWord(mCmdPkt.mPktFmt.HSN)) = buf[i++];
    LoByte(LoWord(mCmdPkt.mPktFmt.HSN)) = buf[i++];
    //discarded if TSN/HSN are wrong...

    HiByte(HiWord(mCmdPkt.mPktFmt.SeqNo)) = buf[i++];   //incrementing counter, need to check?
    LoByte(HiWord(mCmdPkt.mPktFmt.SeqNo)) = buf[i++];
    HiByte(LoWord(mCmdPkt.mPktFmt.SeqNo)) = buf[i++];
    LoByte(LoWord(mCmdPkt.mPktFmt.SeqNo)) = buf[i++];

    mCmdPkt.mPktFmt.rsv[0] = buf[i++];
    mCmdPkt.mPktFmt.rsv[1] = buf[i++];

    HiByte(mCmdPkt.mPktFmt.AckType) = buf[i++];         //?
    LoByte(mCmdPkt.mPktFmt.AckType) = buf[i++];

    HiByte(HiWord(mCmdPkt.mPktFmt.ack)) = buf[i++];
    LoByte(HiWord(mCmdPkt.mPktFmt.ack)) = buf[i++];
    HiByte(LoWord(mCmdPkt.mPktFmt.ack)) = buf[i++];
    LoByte(LoWord(mCmdPkt.mPktFmt.ack)) = buf[i++];

    HiByte(HiWord(mCmdPkt.mPktFmt.length)) = buf[i++];
    LoByte(HiWord(mCmdPkt.mPktFmt.length)) = buf[i++];
    HiByte(LoWord(mCmdPkt.mPktFmt.length)) = buf[i++];
    LoByte(LoWord(mCmdPkt.mPktFmt.length)) = buf[i++];

    //if length not fit or length<12, Regular Session aborted or packet discarded for Control Session...
    if ((mCmdPkt.mPktFmt.length>(mCmdPkt.length-24)) ||
        (mCmdPkt.mPktFmt.length<12))
    {
#if _TCG_ == TCG_EDRV
//        ErrStatus1667 = STS_1667_NO_DATA_RETURN;
#endif
        if(mSessionManager.state==SESSION_START)
        {   result=STS_SESSION_ABORT;     errcode=0x50;   goto EXIT0; }
        else
        {   result=STS_STAY_IN_IF_SEND;     errcode=0x60;   goto EXIT0; }
    }

    //SubPktFmt
    for (j=0; j<6; j++)
        mCmdPkt.mSubPktFmt.rsv[j]=buf[i++];

    HiByte(mCmdPkt.mSubPktFmt.kind) = buf[i++];     //zeros for data subpackets.
    LoByte(mCmdPkt.mSubPktFmt.kind) = buf[i++];

    HiByte(HiWord(mCmdPkt.mSubPktFmt.length)) = buf[i++];
    LoByte(HiWord(mCmdPkt.mSubPktFmt.length)) = buf[i++];
    HiByte(LoWord(mCmdPkt.mSubPktFmt.length)) = buf[i++];
    LoByte(LoWord(mCmdPkt.mSubPktFmt.length)) = buf[i++];

    //if length exceeds the packet, Regular Session aborted or packet discarded for Control Session...
    if (mCmdPkt.mSubPktFmt.length>(mCmdPkt.mPktFmt.length-12))
    {
        if(mSessionManager.state==SESSION_START)
        {   result=STS_SESSION_ABORT;     errcode=0x70;   goto EXIT0; }
        else
        {   result=STS_STAY_IN_IF_SEND;     errcode=0x80;   goto EXIT0; }
    }

    ploadLen=mCmdPkt.mSubPktFmt.length;

#if 0
    for (j=0; i<TCG_BUF_LEN; j++)   //length
        mCmdPkt.payload[j]=buf[i++];
#else

    MEM_COPY((void *)(&mCmdPkt.payload[0]), (void *)(&buf[i]), sizeof(mCmdPkt.payload));   //copy trusted send cmd buffer to mCmdPkt.payload buffer
#endif

    //CmdPkt format checking...
    if((mCmdPkt.mPktFmt.TSN==0)&&(mCmdPkt.mPktFmt.HSN==0))
    { //Session Manager call only, for Test Cases A11-3-5-6-1-1
        bControlSession = TRUE;
        return STS_SUCCESS;
    }

    else if(mSessionManager.state==SESSION_START)
    {
        if((mCmdPkt.mPktFmt.TSN!=mSessionManager.SPSessionID) ||
           (mCmdPkt.mPktFmt.HSN!=mSessionManager.HostSessionID))
        {   result=STS_STAY_IN_IF_SEND;     errcode=0x90;   goto EXIT0; }

        bControlSession = FALSE;   //Regular Session
    }
    else //if(mSessionManager.state==SESSION_CLOSE)
    {
        if((mCmdPkt.mPktFmt.TSN!=0) ||
           (mCmdPkt.mPktFmt.HSN!=0))
        {   result=STS_STAY_IN_IF_SEND;     errcode=0xA0;   goto EXIT0; } //A8-1-1-1-1(3)

        bControlSession = TRUE;
    }

    //Token checking...
EXIT0:
    if(result!=STS_SUCCESS)
    {
        if(result==STS_STAY_IN_IF_SEND){
            soutb3(0x5C, 0xFE, 0x33);   //!!NG:...
            TCG_PRINTF("!!NG: CmdPktParser NG -> Stay in IF-SEND ");
        }else{
            soutb3(0x5C, 0xFE, 0x32);
            TCG_PRINTF("!!NG: CmdPktParser NG -> Abort Session ");
        }
        soutb(errcode);
        TCG_PRINTF("%2X\n", errcode);
    }

    return result;

}

/*U16 TcgTransactionTokenCheck(void)
{
    //UID64 invokingUID, methodUID;
    U8 byte;

    iPload=0;   //reset payload index

    // 0. check Control Tokens first
    byte=ChkToken();    //mCmdPkt.payload[iPload];

    if(byte==TOK_StartTransaction)
    {
        dataBuf[iDataBuf++] = byte;  //Mirro TOK_StartTransaction for response
        if(mSessionManager.state!=SESSION_START)
        {   // error!
            dataBuf[iDataBuf++]=STS_NOT_AUTHORIZED;  //error, for response Transaction status
            mSessionManager.TransactionState=TRNSCTN_NG;
            return STS_SUCCESS;  // found Transaction token
        }
        if(mSessionManager.TransactionState != TRNSCTN_IDLE)
        {   // error!
            dataBuf[iDataBuf++]=STS_UNIQUENESS_CONFLICT;  //error, for response Transaction status
            mSessionManager.TransactionState=TRNSCTN_NG;
            return STS_SUCCESS;  // found Transaction token
        }
        // Enable Transaction
        dataBuf[iDataBuf++]=0;   //ok, for response Transaction status
        mSessionManager.TransactionState=TRNSCTN_ACTIVE;
        return STS_SUCCESS;  // found Transaction token
    }
    else if(byte==TOK_EndTransaction)
    {
        dataBuf[iDataBuf++] = byte;  //Mirro TOK_StartTransaction for response
        if(mSessionManager.state!=SESSION_START)
        {   // error!
            dataBuf[iDataBuf++]=STS_NOT_AUTHORIZED;  //error, for response Transaction status
            mSessionManager.TransactionState=TRNSCTN_NG;
            return STS_SUCCESS;  // found Transaction token
        }
        if(mSessionManager.TransactionState != TRNSCTN_ACTIVE)
        {   // error!
            dataBuf[iDataBuf++]=STS_UNIQUENESS_CONFLICT;  //error, for response Transaction status
            mSessionManager.TransactionState=TRNSCTN_NG;
            return STS_SUCCESS;  // found Transaction token
        }
        // Enable Transaction
        dataBuf[iDataBuf++]=0;   //ok, for response Transaction status
        mSessionManager.TransactionState=TRNSCTN_OK;
        return STS_SUCCESS;  // found Transaction token
    }
    else
        --iPload;

    return STS_INVALID_PARAMETER;  // no found Transaction
} */

/***********************************************************
 *TcgStreamDecode(void):
 *  Decode the subpacket payload here. See Core 3.2.4.2
 *  =>  Method Header + Parameters + Status
 *
 *  Payload is decoded from TOK_Call (0xF8) to MethodUID only in this subrourtine.
 *  The remaining payload decoding (TOK_StartList, ...) should be followed by the
 *  corresponding function call.
 *
 *  Response Data (dataBuf[]) should also be prepared here.
 *
 *  return 0 if no error.
 ***********************************************************/
U16 TcgStreamDecode(void)  //sCmdPkt*  mPkt)
{
    //UID64 invokingUID, methodUID;
    U8 byte;
    U16 result = STS_SUCCESS;
    U32 tmp32;

    iPload=0;   //reset payload index
    //soutb(0xff);    //wierd!!!, need to add some debug codes, otherwise "InvokingAdminTable" NG... cj 03/22/2012

    if(bControlSession)
    { //Session is not started => Control Session, only accept SMUID
//Control Session, TSN/HSN=0x00, A6-3-1...
        byte=ChkToken();
        if(byte==TOK_Call)
        {   //1. Call token, start of a method invocation...

            //2. check method header:
             // get Invoking UID
            if(AtomDecoding_Uid2(invokingUID.bytes)!=STS_SUCCESS)
                return STS_STAY_IN_IF_SEND;

            if(invokingUID.all!=SMUID)
                return STS_STAY_IN_IF_SEND;

            // get Method UID
            if(AtomDecoding_Uid2(methodUID.bytes)!=STS_SUCCESS)
                return STS_STAY_IN_IF_SEND;

            result = InvokingSessionManager();
            if(result!=STS_SUCCESS)
            {
                if(result&0xff00)   //STS_SESSION_ABORT or STS_STAY_IN_IF_SEND
                    return result;

                //dataBuf[iDataBuf++]=TOK_StartList;   // remark for Wave's response on 1/18 2013 mail.
                //dataBuf[iDataBuf++]=TOK_EndList;
                //dataBuf[iDataBuf++]=TOK_EndOfData;
            }

            //add status to reponse buffer and update length
            statusCode[0]=result;
            statusCode[1]=0;
            statusCode[2]=0;
            TcgRespPktUpdate(TRUE);

            return result;
        }
        else
        { //Upexpected!
            return STS_STAY_IN_IF_SEND;
        }
    }
    else
    { //TSN/HSN!=0x00, Session is started => Regular Session, A6-0-1...
//soutb(0xff);
        // 0. check Control Tokens first
        byte=ChkToken();
        if(byte==TOK_Call)
        {   //1. Call token, start of a method invocation...

            //2. check method header:
            // get Invoking UID
            if(AtomDecoding_Uid2(invokingUID.bytes)!=STS_SUCCESS)
            {
                result = STS_NOT_AUTHORIZED;
                goto UPDATE_STATUS;
            }

            // get Method UID
            if(AtomDecoding_Uid2(methodUID.bytes)!=STS_SUCCESS)
            {
                result = STS_NOT_AUTHORIZED;
                goto UPDATE_STATUS;
            }

            //3. parameters...
            //4. End of data
            //5. Status code list
            if(invokingUID.all==SMUID)
            { // Session Manager
                result = InvokingSessionManager();
            }
            else
            {
                if(mSessionManager.SPID.all==UID_SP_Admin)
                    result=InvokingAdminTable();
                else //if(mSessionManager.SPID.all==UID_SP_Locking)
                    result=InvokingLockingTable();
            }

UPDATE_STATUS:
            if(result!=STS_SUCCESS)
            {
                if((result==STS_SESSION_ABORT)||(result==STS_STAY_IN_IF_SEND))
                    return result;

                if(result==STS_SUCCESS_THEN_ABORT)
                { //??? D10-3-3-1-1
                    ResetSessionManager();
                    result = STS_SUCCESS;
                }

                dataBuf[iDataBuf++]=TOK_StartList;   //cj: need these lines for RevertSP, 02/15/2013 // remark for Wave's response on 1/18 2013 mail.
                dataBuf[iDataBuf++]=TOK_EndList;
                dataBuf[iDataBuf++]=TOK_EndOfData;
            }

            //add status to reponse buffer and update length
            statusCode[0]=result;
            statusCode[1]=0;
            statusCode[2]=0;
            result = TcgRespPktUpdate(TRUE);

            return result;

        }
        else if(byte==TOK_EndOfSession)
        {
            //EOD, no status ...
            //for(j=0; j<3; j++)     mSessionManager.status[j] = mCmdPkt.payload[iPload++];

            {
                soutb3(0x5C, 0xFE, 0x03);  // show "Close Session"
                TCG_PRINTF("Close Session >>>\n");
                invokingUID.all = SMUID;
                methodUID.all = SM_MTD_CloseSession;

                ResetSessionManager();

                //prepare payload
                dataBuf[iDataBuf++] = TOK_EndOfSession;
                TcgRespPktUpdate(FALSE);

                return STS_SUCCESS;
            }
        }
        else if(byte==TOK_StartTransaction)
        {
            result = AtomDecoding_Uint32(&tmp32);
soutb3(0x5C, 0xFE, 0x13); soutw(result); soutb(mSessionManager.TransactionState);
            TCG_PRINTF("TransactionStart Status=%04X, State=%02X\n",result ,mSessionManager.TransactionState);
            if(result!=STS_SUCCESS) // no status code
                return STS_SESSION_ABORT;

            if(mSessionManager.TransactionState != TRNSCTN_IDLE)
                return STS_SESSION_ABORT;  // found Transaction token

            // Enable Transaction
            dataBuf[iDataBuf++]=TOK_StartTransaction;
            dataBuf[iDataBuf++]=0;   //ok, for response Transaction status
            TcgRespPktUpdate(FALSE);
            mSessionManager.TransactionState=TRNSCTN_ACTIVE;
            bKeyChanged=FALSE;

soutb3(0x5C, 0xFE, 0x18);
            TCG_PRINTF("TransactionStart OK\n");
            return STS_SUCCESS;

        }
        else if(byte==TOK_EndTransaction)
        {
            result = AtomDecoding_Uint32(&tmp32);
soutb3(0x5C, 0xFE, 0x19); soutw(result); soutb(mSessionManager.TransactionState);
            TCG_PRINTF("TransactionEnd Status=%04X, State=%02X\n", result, mSessionManager.TransactionState);
            if(result!=STS_SUCCESS) //no status code
                return STS_SESSION_ABORT;

            if(mSessionManager.TransactionState != TRNSCTN_ACTIVE)
                return STS_SESSION_ABORT;

            dataBuf[iDataBuf++] = TOK_EndTransaction;

            if(tmp32==0)
            { //transaction commit
soutb3(0x5C, 0xFE, 0x1A);
                TCG_PRINTF("TransactionEnd OK: Commit!!\n");
                dataBuf[iDataBuf++] = 0;

                WriteMtable2NAND();

                if(G3MtableChanged){
                    //D4-1-4-1-1: only update LockingRange after Transaction is done!
                    LockingRangeTable_Update(); // update AES key/range setting if changed.
                    if(bKeyChanged)
                    {
                    //Altaplus,    InitAesKey4Tcg(TRUE);  //key 0~8 for user data
                        TcgResetCache(MSG_TCG_REGEN_ZEROCACHE);
                        bKeyChanged=FALSE;
                    }
                }

                ClearMtableChangedFlag();
                //D5-1-3-1-1
                if((G3.b.mLckMbrCtrl_Tbl.val[0].enable==TRUE)&&(G3.b.mLckMbrCtrl_Tbl.val[0].done==FALSE))
                    mTcgStatus |= MBR_SHADOW_MODE;
                else
                    mTcgStatus &= (~MBR_SHADOW_MODE);

                //D8-1-3-1-1
            }
            else //if(tmp32)
            { //transaction abort
soutb3(0x5C, 0xFE, 0x1B);
                TCG_PRINTF("TransactionEnd OK: Abort!!\n");
                dataBuf[iDataBuf++] = 1;

                ReadNAND2Mtable();
                ClearMtableChangedFlag();

                //mTcgStatus = 0;
                //if(G1.b.mAdmSP_Tbl.val[SP_LOCKING_IDX].lifeCycle==manufactured_inactive)
                //    mTcgStatus |= TCG_ACTIVATED;

                //if((G3.b.mLckMbrCtrl_Tbl.val[0].enable==TRUE)&&(G3.b.mLckMbrCtrl_Tbl.val[0].done==FALSE))
                //    mTcgStatus |= MBR_SHADOW_MODE;

                //LockingRangeTable_Update();

            }
            TcgRespPktUpdate(FALSE);
            mSessionManager.TransactionState=TRNSCTN_IDLE;

            return STS_SUCCESS;
        }
        else
        {   //Invalid token, session abort! (Test Cases 3.1.5)
            return STS_SESSION_ABORT;
        }
        //return result;
    }
}

/***********************************************************
 *TcgCmdRespPkt(void):
 *
 *  Only prepare the Command Response Packet in Buffer, Payload and Status
 *  should be prepared in TcgStreamDecode
 *
 *  return 0 if no error.
 ***********************************************************/
U16 TcgCmdPkt4Response(void)
{
    U16 i=0;
    U16 j;

    MEM_SET(dataBuf, 0x00, TCG_BUF_LEN);   //clear Tcg Recv Buffer

    for(j=0;j<4;j++)
        dataBuf[i++]=0x00;

    dataBuf[i++] = (U8)(BASE_COMID/256);    //ComID
    dataBuf[i++] = (U8)(BASE_COMID);
    dataBuf[i++] = 0x00;        //Ext ComID, 0x0000 for static ComID
    dataBuf[i++] = 0x00;
    for(j=0;j<4;j++)            //OutstandingData
        dataBuf[i++]=0x00;
    for(j=0;j<4;j++)            //MinTransfer
        dataBuf[i++]=0x00;
    for(j=0;j<4;j++)            //Length, should be updated later
        dataBuf[i++]=0x00;

    //PktFmt
    dataBuf[i++] = HiByte(HiWord(mCmdPkt.mPktFmt.TSN));     //TSN, should be zero if session not started yet
    dataBuf[i++] = LoByte(HiWord(mCmdPkt.mPktFmt.TSN));
    dataBuf[i++] = HiByte(LoWord(mCmdPkt.mPktFmt.TSN));
    dataBuf[i++] = LoByte(LoWord(mCmdPkt.mPktFmt.TSN));
    dataBuf[i++] = HiByte(HiWord(mCmdPkt.mPktFmt.HSN));     //HSN, should be zero if session not started yet
    dataBuf[i++] = LoByte(HiWord(mCmdPkt.mPktFmt.HSN));
    dataBuf[i++] = HiByte(LoWord(mCmdPkt.mPktFmt.HSN));
    dataBuf[i++] = LoByte(LoWord(mCmdPkt.mPktFmt.HSN));
    dataBuf[i++] = HiByte(HiWord(mCmdPkt.mPktFmt.SeqNo));   //incrementing counter, need to check?
    dataBuf[i++] = LoByte(HiWord(mCmdPkt.mPktFmt.SeqNo));
    dataBuf[i++] = HiByte(LoWord(mCmdPkt.mPktFmt.SeqNo));
    dataBuf[i++] = LoByte(LoWord(mCmdPkt.mPktFmt.SeqNo));

    // all '0' for the rest ...
    /*dataBuf[i++] = 0x00;    //rsv
    dataBuf[i++] = 0x00;
    dataBuf[i++] = 0x00;    //AckType
    dataBuf[i++] = 0x00;
    for(j=0;j<4;j++)        //ack
        dataBuf[i++]=0x00;
    for(j=0;j<4;j++)    //Length, should be updated later
        dataBuf[i++]=0x00;

    //SubPktFmt
    for (j=0; j<6; j++)
        dataBuf[i++]=0x00;

    dataBuf[i++]=0x00;      //kind, zeros for data subpackets.
    dataBuf[i++]=0x00;
    for(j=0;j<4;j++)        //Length, should be updated later
        dataBuf[i++]=0x00; */

    iDataBuf=56;

    return 0;
}

void TcgCmdPkt4NoData(void)
{
    U16 i=0;
    U16 j;

    MEM_SET(dataBuf, 0x00, TCG_BUF_LEN);   //clear Tcg Recv Buffer

    for(j=0;j<4;j++)
        dataBuf[i++]=0x00;

    dataBuf[i++] = (U8)(BASE_COMID/256);    //ComID
    dataBuf[i++] = (U8)(BASE_COMID);
    dataBuf[i++] = 0x00;        //Ext ComID, 0x0000 for static ComID
    dataBuf[i++] = 0x00;
    for(j=0;j<4;j++)            //OutstandingData
        dataBuf[i++]=0x00;
    for(j=0;j<4;j++)            //MinTransfer
        dataBuf[i++]=0x00;
    for(j=0;j<4;j++)            //Length, should be updated later
        dataBuf[i++]=0x00;

}

#if _TCG_ == TCG_EDRV
void TcgCmdPkt4CloseSession(void)
{
    MEM_SET(dataBuf, 0x00, TCG_BUF_LEN);   //clear Tcg Recv Buffer

    dataBuf[0x05] = (U8)(BASE_COMID/256);    //ComID
    dataBuf[0x06] = (U8)(BASE_COMID);

    dataBuf[0x13]=0x48;     //ComPkt.Length
    dataBuf[0x2B]=0x30;     //Pkt.Length
    dataBuf[0x37]=0x21;     //SubPkt.Length

    dataBuf[0x38]=TOK_Call;
    dataBuf[0x39]=0xA8;
    dataBuf[0x41]=0xff;
    dataBuf[0x42]=0xA8;
    dataBuf[0x49]=0xFF;
    dataBuf[0x4A]=0x06;

    dataBuf[0x4B]=TOK_StartList;
    dataBuf[0x4C]=0x01;
    dataBuf[0x4D]=0x84;
    dataBuf[0x4E]=HiByte(HiWord(mCmdPkt.mPktFmt.TSN));
    dataBuf[0x4F]=LoByte(HiWord(mCmdPkt.mPktFmt.TSN));
    dataBuf[0x50]=HiByte(LoWord(mCmdPkt.mPktFmt.TSN));
    dataBuf[0x51]=LoByte(LoWord(mCmdPkt.mPktFmt.TSN));
    dataBuf[0x52]=TOK_EndList;

    dataBuf[0x53]=TOK_EndOfData;

    //add status to reponse buffer and update length
    dataBuf[0x54]=TOK_StartList;
    //statusCode[0x53]=0x00;
    dataBuf[0x58]=TOK_EndList;

    rcvCmdPktLen= 0x59;
}
#endif

/***********************************************************
 *TcgMethodStatus4Response(void):
 *
 *  dataBuffer for IF-RECV
 *  1. Add the Method Status List
 *  2. Update the payload length
 *
 *  return 0 if no error.
 ***********************************************************/
U16 TcgRespPktUpdate(U8 addStatus)
{
    U16 j;

    //add Method Status List
    if(addStatus==TRUE)
    { //No method status fo CloseSession
        //Status list
        dataBuf[iDataBuf++] = TOK_StartList;    //0xF0
        dataBuf[iDataBuf++] = statusCode[0];    //Status: OK, TODO: update error code here...
        dataBuf[iDataBuf++] = statusCode[1];
        dataBuf[iDataBuf++] = statusCode[2];
        dataBuf [iDataBuf++] = TOK_EndList;     //0xF1
    }

    iDataBuf -= 56;
    dataBuf[52] = 0x00;     //SubPacket length, padding is not included
    dataBuf[53] = 0x00;
    dataBuf[54] = (U8)(iDataBuf/256);
    dataBuf[55] = (U8)(iDataBuf);

    j = iDataBuf % 4;
    if(j)
        j = 4-j;

    iDataBuf += j + 12;         //Packet length, padding is included
    dataBuf[40] = 0x00;
    dataBuf[41] = 0x00;
    dataBuf[42] = (U8)(iDataBuf/256);
    dataBuf[43] = (U8)(iDataBuf);

    iDataBuf += 24;               //ComPacket length, padding is included
    dataBuf[16] = 0x00;
    dataBuf[17] = 0x00;
    dataBuf[18] = (U8)(iDataBuf/256);
    dataBuf[19] = (U8)(iDataBuf);

    rcvCmdPktLen = iDataBuf + 20;

    if(iDataBuf>mHostProperties[1].val)   //hostMaxPacketSize)
    {
souts("pktSz>MaxPktSz"); soutw(rcvCmdPktLen); soutw(mHostProperties[1].val);//ComPacket.length
        TCG_PRINTF("pktSz>MaxPktSz %4X %X\n", rcvCmdPktLen, mHostProperties[1].val);
        return  STS_RESPONSE_OVERFLOW;
    }

    if(rcvCmdPktLen>mHostProperties[0].val)   //hostMaxComPacketSize)
    {
souts("pktSz>MaxComPktSz"); soutw(rcvCmdPktLen); soutw(mHostProperties[0].val);
        TCG_PRINTF("pktSz>MaxComPktSz %4X %X\n", rcvCmdPktLen, mHostProperties[0].val);
        return  STS_RESPONSE_OVERFLOW;
    }

    if((iDataBuf-36)>mHostProperties[2].val)   //hostIndTokenSize)
    {
souts("indTokenSz>MaxPktSz"); soutw(rcvCmdPktLen); soutw(mHostProperties[2].val);   //subPkt.length
        TCG_PRINTF("indTokenSz>MaxPktSz %4X %X\n", rcvCmdPktLen, mHostProperties[2].val);
        return  STS_SESSION_ABORT;  //STS_RESPONSE_OVERFLOW;
    }
    return STS_SUCCESS;
}

/***********************************************************
 *TcgAbortSession(void):
 *
 *  dataBuffer for IF-RECV
 *  1. Add the Method Status List
 *  2. Update the payload length
 *
 *  return STS_SUCCESS if no error.
 ***********************************************************/
U16 TcgAbortSession(void){

    U16 j;

    MEM_SET(dataBuf, 0x00, TCG_BUF_LEN);   //clear Tcg Recv Buffer


    //ComPkt
    dataBuf[4] = (U8)(BASE_COMID/256);    //ComID
    dataBuf[5] = (U8)(BASE_COMID);

    //Construct payload  for "Close Session"
    iDataBuf=56;

    dataBuf[iDataBuf++] = TOK_Call;
    // Session Manager
    dataBuf [iDataBuf++] = 0xA8;
    for(j=0; j<7; j++)
        dataBuf[iDataBuf++] = 0x00;
    dataBuf[iDataBuf++] = 0xFF;

    // Close Session
    dataBuf [iDataBuf++] = 0xA8;
    for(j=0; j<6; j++)
        dataBuf[iDataBuf++] = 0x00;
    dataBuf[iDataBuf++] = 0xFF;
    dataBuf[iDataBuf++] = 0x06;

    dataBuf[iDataBuf++] = TOK_StartList;

    dataBuf[iDataBuf++] = 0x84; //short atom
    dataBuf[iDataBuf++] = HiByte(HiWord(mSessionManager.HostSessionID));
    dataBuf[iDataBuf++] = LoByte(HiWord(mSessionManager.HostSessionID));
    dataBuf[iDataBuf++] = HiByte(LoWord(mSessionManager.HostSessionID));
    dataBuf[iDataBuf++] = LoByte(LoWord(mSessionManager.HostSessionID));

    dataBuf[iDataBuf++] = 0x84; //short atom
    dataBuf[iDataBuf++] = HiByte(HiWord(mSessionManager.SPSessionID));
    dataBuf[iDataBuf++] = LoByte(HiWord(mSessionManager.SPSessionID));
    dataBuf[iDataBuf++] = HiByte(LoWord(mSessionManager.SPSessionID));
    dataBuf[iDataBuf++] = LoByte(LoWord(mSessionManager.SPSessionID));

    dataBuf[iDataBuf++] = TOK_EndList;
    dataBuf[iDataBuf++] = TOK_EndOfData;

    //add status to reponse buffer and update length
    statusCode[0]=0;
    statusCode[1]=0;
    statusCode[2]=0;
    TcgRespPktUpdate(TRUE);

    return STS_SUCCESS;
}

/***********************************************************
* Decode and process Method invoked at SessionManager layer
* return 0 if success
***********************************************************/
U16 InvokingSessionManager(void)
{
    U8 byte, errCode=0;
    U32 j;
    U16 result = STS_SUCCESS;

    // Invoking UID: Session Manager
    switch (methodUID.all)
    {
    case SM_MTD_Properties:
        result = H2TP_Properties();
        //TP2H_PropertiesResponsePreparing();
        return result;

    case SM_MTD_StartSession:
        soutb3(0x5C, 0xFE, 0x17);  // show "StartSession"
        TCG_PRINTF("<<< Start Session...\n");

        if(mSessionManager.state==SESSION_START)
        {   /* session already started, error */
            if(bControlSession)
            {   errCode=0x02;   result= STS_NO_SESSIONS_AVAILABLE;  }
            else
            {   errCode=0x04;   result= STS_NOT_AUTHORIZED; }     //;

            goto SYNC_REPLY;
        }

        // TODO: clear session manager first
        //soutb3(0x5C, 0xFE, 0x02);  // show "StartSession"

        // ref: Core3.2.4.1, Parameters
        //1. Start List token
        if ((byte=ChkToken())!=TOK_StartList)
        {   errCode=0x10; result=STS_STAY_IN_IF_SEND; goto MNG_EXIT;    }

        //2. Required parameters: HostSessionID:uint, SPID:uid, write:bool
        if(AtomDecoding_Uint32(&mSessionManager.HostSessionID)!=STS_SUCCESS)
        {   errCode=0x12; result=STS_STAY_IN_IF_SEND; goto MNG_EXIT;    }

        if(AtomDecoding_Uid2(mSessionManager.SPID.bytes)!=STS_SUCCESS)
        {   errCode=0x14; result=STS_STAY_IN_IF_SEND; goto MNG_EXIT;    }

        //for(j=0;j<G1.b.mAdmSP_Tbl.rowCnt;j++)
        if((mSessionManager.SPID.all!=G1.b.mAdmSP_Tbl.val[0].uid)&&
           (mSessionManager.SPID.all!=G1.b.mAdmSP_Tbl.val[1].uid))
        {   errCode=0x18;   result=STS_INVALID_PARAMETER;   goto SYNC_REPLY;    }

        if(mSessionManager.SPID.all==UID_SP_Locking)
        {
            if(G1.b.mAdmSP_Tbl.val[SP_LOCKING_IDX].lifeCycle!=manufactured)
            { //inactive
soutb3(0x5C, 0xFE, 0x30);
                TCG_PRINTF("!!NG: Not Activate Yet\n");
                errCode=0x1A;   result=STS_INVALID_PARAMETER;
                goto SYNC_REPLY;
            }
        }

        //Write-enabled bit
        if(AtomDecoding_Uint8(&mSessionManager.Write)!=STS_SUCCESS)
        {   errCode=0x20; result=STS_STAY_IN_IF_SEND;   goto MNG_EXIT;  }

        if((mSessionManager.Write!=0x00)&&(mSessionManager.Write!=0x01))
        {   errCode=0x22;   result = STS_INVALID_PARAMETER; goto SYNC_REPLY;    }

        //3. Optional parameters, must be in order: [0]HostSessionID=byte, [3]HostSigningAuthority=uid
        mSessionManager.HtSgnAuthority.all=UID_Authority_Anybody;

        if((byte=ChkToken())==TOK_StartName) //3.1 Start Name token
        {
            //while(iPload<ploadLen)
            //3.2 encoded name, 3.3 encoded value, 3.4 End Name token

            //check Tiny Token
            if(AtomDecoding_Uint8(&byte)!=STS_SUCCESS)
            {
                errCode=0x24;
                result = STS_INVALID_PARAMETER;       //need to check ...
                goto SYNC_REPLY;
            }

            if(byte==0x00)
            { //HostChallenge
                U32 len;

                result=AtomDecoding(ATOM_BYTES,&len);
                if(result==STS_SUCCESS)
                {
                    //if(len>CPIN_LENGTH)
                    //{
                    //    result = STS_INVALID_PARAMETER;       //need to check ...
                    //    goto SYNC_REPLY;
                    //}

                    mSessionManager.HtChallenge[0]=LoByte(LoWord(len));
                    for (j=0;j<len;j++)
                        mSessionManager.HtChallenge[j+1]=DecBuf[j];

                    if(ChkToken()!=TOK_EndName)
                    {   errCode=0x30; result=STS_STAY_IN_IF_SEND; goto MNG_EXIT;    }


                    if((byte=ChkToken())!=TOK_StartName)
                        goto END_LIST;

                    if(AtomDecoding_Uint8(&byte)!=STS_SUCCESS)
                    {
                        errCode=0x32;
                        result = STS_INVALID_PARAMETER;       //need to check ...
                        goto SYNC_REPLY;
                    }
                }
            }

            if (byte==0x03)
            {//HostSigningAuthority
                if(AtomDecoding_Uid2(mSessionManager.HtSgnAuthority.bytes)!=STS_SUCCESS)

                {   errCode=0x34; result=STS_STAY_IN_IF_SEND; goto MNG_EXIT;    }


                if(ChkToken()!=TOK_EndName)

                {   errCode=0x36; result=STS_STAY_IN_IF_SEND; goto MNG_EXIT;    }


                //get EndList
                byte=ChkToken();
                if(byte==TOK_StartName)
                { //more optional parameters..., A6-3-4-2-1(3)
                    errCode=0x38;
                    result = STS_INVALID_PARAMETER;
                    goto SYNC_REPLY;
                }
            }
            else
            {
                //more optional parameters...
                errCode=0x3A;
                result = STS_INVALID_PARAMETER;
                goto END_LIST;
            }
        }

END_LIST:
        if(byte!=TOK_EndList)
        {   errCode=0x40; result=STS_STAY_IN_IF_SEND; goto MNG_EXIT;    }

        //4. End of Data token
        if(ChkToken()!=TOK_EndOfData)
        {   errCode=0x42; result=STS_STAY_IN_IF_SEND; goto MNG_EXIT;    }

        //5. Status Code list
        result = ChkMethodStatus();
        if(result==STS_SESSION_ABORT)
        {   result = STS_STAY_IN_IF_SEND;   //Control Session, A6-3-8-6-1
            errCode=0x44;  goto MNG_EXIT;    }

        soutb3(0x5C, 0xFE, 0x02);  // show "StartSession"
        soutb(mSessionManager.SPID.all);
        soutd(mSessionManager.HtSgnAuthority.all);
        TCG_PRINTF("... SP:%02X AUTH:%08X\n",(U8)mSessionManager.SPID.all, (U32)mSessionManager.HtSgnAuthority.all);

        if(result==STS_SUCCESS)
        { //papyload check ok, check authority...  prepare SyncSession data
#if (CO_SUPPORT_PSID==FALSE)  // PSID is not supported!!
            if(mSessionManager.HtSgnAuthority.all==UID_Authority_PSID)
                result = STS_NOT_AUTHORIZED;
            else
#endif
#if CO_SUPPORT_BLOCK_SID_AUTH
            if((mTcgStatus&SID_BLOCKED)&&(mSessionManager.HtSgnAuthority.all==UID_Authority_SID))
                result = STS_NOT_AUTHORIZED;
            else
#endif
            result = HostSigningAuthorityCheck();
            if(result==STS_SUCCESS)
            { // SessionStart OK!
                mSessionManager.state=SESSION_START;
                mSessionManager.SPSessionID += 1;   //start from 0x1001;   //assigned by device, add 1 per session start-up ok

                soutb3(0x5C, 0xFE, 0x0F);  // show "SyncSession"
                soutw(mSessionManager.SPSessionID);
                TCG_PRINTF("Sync Session %04X\n", mSessionManager.SPSessionID);
                //result =  STS_SUCCESS;
            }
            else
            { // authority check NG!
                soutb3(0x5C, 0xFE, 0x31); soutw(result); //"!!NG AuthorityCheck"
                TCG_PRINTF("!!NG: AuthorityCheck %04X\n", result);
                //errCode=0x10;
                //result = STS_INVALID_PARAMETER;
            }
        }

SYNC_REPLY:
        //prepare payload: SyncSession data
        dataBuf [iDataBuf++] = TOK_Call;
        dataBuf [iDataBuf++] = 0xA8;
        for(j=8; j!=0; )
            dataBuf[iDataBuf++] = invokingUID.bytes[--j];

        dataBuf [iDataBuf++] = 0xA8;
        for(j=0; j<6; j++)
            dataBuf[iDataBuf++] = 0x00;
        dataBuf[iDataBuf++] = 0xFF;
        dataBuf[iDataBuf++] = 0x03;

        dataBuf[iDataBuf++] = TOK_StartList;

        dataBuf[iDataBuf++] = 0x84; //short atom
        dataBuf[iDataBuf++] = HiByte(HiWord(mSessionManager.HostSessionID));
        dataBuf[iDataBuf++] = LoByte(HiWord(mSessionManager.HostSessionID));
        dataBuf[iDataBuf++] = HiByte(LoWord(mSessionManager.HostSessionID));
        dataBuf[iDataBuf++] = LoByte(LoWord(mSessionManager.HostSessionID));

        if(result==STS_SUCCESS)
        {
            dataBuf[iDataBuf++] = 0x84; //short atom
            dataBuf[iDataBuf++] = HiByte(HiWord(mSessionManager.SPSessionID));
            dataBuf[iDataBuf++] = LoByte(HiWord(mSessionManager.SPSessionID));
            dataBuf[iDataBuf++] = HiByte(LoWord(mSessionManager.SPSessionID));
            dataBuf[iDataBuf++] = LoByte(LoWord(mSessionManager.SPSessionID));
        }
        else
            dataBuf[iDataBuf++] = 0x00;

        dataBuf[iDataBuf++] = TOK_EndList;
        dataBuf[iDataBuf++] = TOK_EndOfData;

MNG_EXIT:
        if(errCode){
            //alexdebug1s(errCode,SSNG!!);
            soutb3(0x5C, 0xFE, 0x38); soutb(errCode);
            TCG_PRINTF("!!NG: SessionManager %02X\n", errCode);
        }
        return result;

    //case SM_MTD_CloseSession:
    //    EndSession();
    //    break;

    default:
        return  STS_STAY_IN_IF_SEND;
    }
}

U16 HostSigningAuthorityCheck(void)
{
    U64 tgtCPinUid;
    register U16 j,i,k;
    U8 digest[SHA256_DIGEST_SIZE];

    if(mSessionManager.SPID.all==UID_SP_Admin)
    {
        for(j=0;j<G1.b.mAdmAuthority_Tbl.rowCnt;j++)
        { // row# (4) can be acquired from Table tbl;
            if(mSessionManager.HtSgnAuthority.all==G1.b.mAdmAuthority_Tbl.val[j].uid)
            {
                mSessionManager.HtAuthorityClass=G1.b.mAdmAuthority_Tbl.val[j].Class;
                if(G1.b.mAdmAuthority_Tbl.val[j].isClass==TRUE)
                    return STS_INVALID_PARAMETER;   //UID is a class

                if(G1.b.mAdmAuthority_Tbl.val[j].enabled==FALSE) //UID disabled!
                    return STS_NOT_AUTHORIZED;      //core v2.0 r2.0 5.3.4.1.4
                    //STS_INVALID_PARAMETER;            //test case v1.0

                if(G1.b.mAdmAuthority_Tbl.val[j].operation==AUTH_Password)
                {
                    //if(mSessionManager.HtChallenge[0]==0) //lengh=0: no password
                    //    return STS_INVALID_PARAMETER;

                    tgtCPinUid=G1.b.mAdmAuthority_Tbl.val[j].credential;

                    for(i=0;i<G1.b.mAdmCPin_Tbl.rowCnt;i++)
                    {
                        if(tgtCPinUid==G1.b.mAdmCPin_Tbl.val[i].uid)
                        {
                            if(G1.b.mAdmCPin_Tbl.val[i].tryLimit!=0)
                            { // check tries count
                                if(G1.b.mAdmCPin_Tbl.val[i].tries>=G1.b.mAdmCPin_Tbl.val[i].tryLimit)
                                {
soutb3(i, G1.b.mAdmCPin_Tbl.val[i].tryLimit, G1.b.mAdmCPin_Tbl.val[i].tries);
                                    TCG_PRINTF("tryLimit %2X %2X %2X\n", i, G1.b.mAdmCPin_Tbl.val[i].tryLimit, G1.b.mAdmCPin_Tbl.val[i].tries);
                                    return STS_AUTHORITY_LOCKED_OUT;
                                }
                            }

                            //password checking...  password is Hashed!
                            if(mSessionManager.HtChallenge[0]==G1.b.mAdmCPin_Tbl.val[i].pwd[0]) //password length
                            {
                                if(G1.b.mAdmCPin_Tbl.val[i].pwd[0]==0)   //password==""
                                {
                                    G1.b.mAdmCPin_Tbl.val[i].tries = 0;
                                    return STS_SUCCESS;
                                }
#ifdef _ALEXDEBUG  // for debug
    soutb3(0x5C,0xFE,0xF4);  // here ASCII code , ex 'A'=0x41, 'B'=0x42, etc....
    for(k=1;k<=4;k++)
        soutb(mSessionManager.HtChallenge[k]);
#endif
                                //sha256(&mSessionManager.HtChallenge[1], mSessionManager.HtChallenge[0], digest);
                                HAL_Sha256(&mSessionManager.HtChallenge[1], mSessionManager.HtChallenge[0], digest);

                                soutb3(0x5C,0xFE,0xF2);
                                soutb(digest[0]); soutb(digest[1]); soutb(digest[2]); soutb(digest[3]);
                                TCG_PRINTF("SHA2: %2X%2X%2X%2X ...\n",digest[0], digest[1], digest[2], digest[3]);

                                for(k=0;k<SHA256_DIGEST_SIZE;k++)
                                {
                                    if(G1.b.mAdmCPin_Tbl.val[i].pwd[k+1]!=digest[k]) //password compare NG
                                    {
                                        TCG_PRINTF("NG@: ");
                                        for(j=1;j<=4;j++)
                                            TCG_PRINTF("%2X",G1.b.mAdmCPin_Tbl.val[i].pwd[j]);
                                        TCG_PRINTF("...\n");

                                        //TCG_PRINTF("digest@: ");
                                        //for(j=0;j<32;j++) TCG_PRINTF("%2X",digest[j]);  TCG_PRINTF("...\n");
                                        goto AdmCPin_NG;
                                    }
                                }

                                G1.b.mAdmCPin_Tbl.val[i].tries = 0;
                                return STS_SUCCESS;
                            }
                            else if(mSessionManager.HtChallenge[0]==0)
                            {
                                return STS_INVALID_PARAMETER;
                            }
AdmCPin_NG:
                                    if(G1.b.mAdmCPin_Tbl.val[i].tries<G1.b.mAdmCPin_Tbl.val[i].tryLimit)
                                        G1.b.mAdmCPin_Tbl.val[i].tries++;

soutb3(i,G1.b.mAdmCPin_Tbl.val[i].tryLimit,G1.b.mAdmCPin_Tbl.val[i].tries);
                                    TCG_PRINTF("Admin tryLimit: %2X %2X %2X\n", i, G1.b.mAdmCPin_Tbl.val[i].tryLimit, G1.b.mAdmCPin_Tbl.val[i].tries);
                                    return STS_NOT_AUTHORIZED;
                        }
                    }
                }
                else if(mSessionManager.HtSgnAuthority.all==UID_Authority_Anybody)
                { //no password (ex. Anybody)
                     if(mSessionManager.HtChallenge[0]<=32)   //password length<=32
                        return STS_SUCCESS ;
                }

                return STS_INVALID_PARAMETER;
            }
        }

        return STS_INVALID_PARAMETER;  //HostSigningAuthority UID nonexistent
    }
    else //if(mSessionManager.SPID.all==UID_SP_Locking)
    {
        for(j=0;j<G3.b.mLckAuthority_Tbl.rowCnt;j++)
        { // row# (4) can be acquired from Table tbl;
            if(mSessionManager.HtSgnAuthority.all==G3.b.mLckAuthority_Tbl.val[j].uid)
            {
                mSessionManager.HtAuthorityClass=G3.b.mLckAuthority_Tbl.val[j].Class;
                if(G3.b.mLckAuthority_Tbl.val[j].isClass==TRUE) //"Class Authority", return NG!
                    return STS_INVALID_PARAMETER;   //UID is a class

                if(G3.b.mLckAuthority_Tbl.val[j].enabled==FALSE) //not enabled yet!
                    return STS_NOT_AUTHORIZED;      //core v2.0 r2.0 5.3.4.1.4
                    //STS_INVALID_PARAMETER;            //test case v1.0

                if(G3.b.mLckAuthority_Tbl.val[j].operation==AUTH_Password)
                {
                    //if(mSessionManager.HtChallenge[0]==0) //lengh=0: no password
                    //{    errorCode = 0x22; result = STS_INVALID_PARAMETER; goto ERR_EXIT;  }

                    tgtCPinUid=G3.b.mLckAuthority_Tbl.val[j].credential;
                    for(i=0;i<G3.b.mLckCPin_Tbl.rowCnt;i++)
                    {
                        if(tgtCPinUid==G3.b.mLckCPin_Tbl.val[i].uid)
                        {
                            if(G3.b.mLckCPin_Tbl.val[i].tryLimit!=0)
                            {
                                if(G3.b.mLckCPin_Tbl.val[i].tries>=G3.b.mLckCPin_Tbl.val[i].tryLimit)
                                    return STS_AUTHORITY_LOCKED_OUT;
                            }

                            D_PRINTF("L=%02X, %02X\n",  mSessionManager.HtChallenge[0], G3.b.mLckCPin_Tbl.val[i].pwd[0]);

                            //password checking...  password is Hashed!
                            if(mSessionManager.HtChallenge[0]==G3.b.mLckCPin_Tbl.val[i].pwd[0]) //password length
                            {
                                if(G3.b.mLckCPin_Tbl.val[i].pwd[0]==0)   //password==""
                                {
                                    G3.b.mLckCPin_Tbl.val[i].tries = 0;
                                    return STS_SUCCESS;
                                }
#ifdef _ALEXDEBUG  // for debug
    soutb3(0x5C,0xFE,0xF7);  // here ASCII code , ex 'A'=0x41, 'B'=0x42, etc....
    for(k=1;k<=4;k++)
        soutb(mSessionManager.HtChallenge[k]);
#endif

D_PRINTF("pwd: L%02X ",mSessionManager.HtChallenge[0]);
for(k=1;k<=8;k++)
    D_PRINTF("%02X ", mSessionManager.HtChallenge[k]);
D_PRINTF("\n");
                                //sha256(&mSessionManager.HtChallenge[1], mSessionManager.HtChallenge[0], digest);
                                HAL_Sha256(&mSessionManager.HtChallenge[1], mSessionManager.HtChallenge[0], digest);

                                soutb3(0x5C,0xFE,0xF2);
                                soutb(digest[0]); soutb(digest[1]); soutb(digest[2]); soutb(digest[3]);
                                TCG_PRINTF("SHA2: %2X%2X%2X%2X ...\n",digest[0], digest[1], digest[2], digest[3]);

                                for(k=0;k<SHA256_DIGEST_SIZE;k++)
                                {
                                    if(G3.b.mLckCPin_Tbl.val[i].pwd[k+1]!=digest[k]) //password compare NG
                                    {
                                        TCG_PRINTF("NG@: ");
                                        for(j=1;j<=4;j++)
                                            TCG_PRINTF("%2X",G3.b.mLckCPin_Tbl.val[i].pwd[j]);
                                        TCG_PRINTF("...\n");

                                        goto LckCPin_NG;
                                    }
                                }

                                G3.b.mLckCPin_Tbl.val[i].tries = 0;
                                return STS_SUCCESS;
                            }
                            else if(mSessionManager.HtChallenge[0]==0)
                            {
                                return STS_INVALID_PARAMETER;
                            }
LckCPin_NG:
                            if(G3.b.mLckCPin_Tbl.val[i].tries<G3.b.mLckCPin_Tbl.val[i].tryLimit)
                                G3.b.mLckCPin_Tbl.val[i].tries++;

soutb3(0x5C, 0xFE, 0x37);
soutb3(i,G3.b.mLckCPin_Tbl.val[i].tryLimit,G3.b.mLckCPin_Tbl.val[i].tries);
                            TCG_PRINTF("Locking tryLimit: %2X %2X %2X\n", i, G3.b.mLckCPin_Tbl.val[i].tryLimit, G3.b.mLckCPin_Tbl.val[i].tries);
                            return STS_NOT_AUTHORIZED;

                        }
                    }
                }
                else if(mSessionManager.HtSgnAuthority.all==UID_Authority_Anybody)
                { //no password (ex. Anybody)
                     if(mSessionManager.HtChallenge[0]<=32)   //password length<=32
                        return STS_SUCCESS ;
                }

            }
        }

        return STS_INVALID_PARAMETER;  //HostSigningAuthority UID nonexistent
    }
}

/***********************************************************
* AtomDecoding
***********************************************************/
int AtomDecoding(tATOM_DEC_TYPE AtomDecodeType, U32* length)
{
    int zSTS;

    *length = 0;
    MEM_SET(DecBuf, 0x00, sizeof(DecBuf));   //clear Buffer

    //for test case A13-2-1-2-6
    if(ChkToken()==TOK_EndName){
        zSTS = STS_SUCCESS;
        iPload--;
        goto AtomDecoding_exit;
    }
    iPload--;

    switch(AtomDecodeType){
        case ATOM_HALFUID:
            zSTS=AtomDecoding_HalfUid(length);
            break;
        case ATOM_UID:
            zSTS=AtomDecoding_Uid(length);
            break;
        case ATOM_BYTES:
            zSTS=AtomDecoding_Bytes(length);
            break;
        case ATOM_INT:
            zSTS=AtomDecoding_Int(length);
            break;
        default:
            zSTS=STS_INVALID_PARAMETER;
            break;
    }
AtomDecoding_exit:
    return zSTS;
}

int AtomDecoding_HalfUid(U32* length)
{
    int zSTS;

    zSTS=AtomDecoding_Bytes(length);
    if(*length != 4){
        zSTS=STS_INVALID_PARAMETER;
    }
    return zSTS;
}

int AtomDecoding_Uid(U32* length)
{
    int zSTS;

    zSTS=AtomDecoding_Bytes(length);
    if(*length != 8){
        zSTS=STS_INVALID_PARAMETER;
    }
    return zSTS;
}

int AtomDecoding_Bytes(U32* length)
{
    U8 byte;
    U32 j;
    U8 ErrCode=0x00;  //no error

    byte=mCmdPkt.payload[iPload++];
    *length = 0;

    if((byte&0x80)==0x00)
    {  //Tiny atom, 0~63 or -32~31
        ErrCode=0x20;
        goto AtomDecoding_Bytes_err;
    }
    else if((byte&0xc0)==0x80)
    { //Short atom
        //TODO
        *length=byte&0x0f;
        if(*length > sizeof(DecBuf)){ // over ary size ?
            ErrCode=0x30;
            goto AtomDecoding_Bytes_err;
        }
        if((byte&0xf0)==0xA0){ //byte sequence
            for(j=0;j<*length;j++){
                DecBuf[j] = mCmdPkt.payload[iPload++];
            }
        }else if((byte&0xf0)==0xb0){
            // TODO: Continued into another atom

            //return error temporarily
            ErrCode=0x40;
            goto AtomDecoding_Bytes_err;
        }
        else
        {
            ErrCode=0x50;
            goto AtomDecoding_Bytes_err;
        }
    }
    else if((byte&0xe0)==0xc0)
    { //Medium atom
        //TODO
        *((U8 *)length+1)=byte&0x07;
        *((U8 *)length+0)=mCmdPkt.payload[iPload++];
        if(*length > sizeof(DecBuf)){ // over ary size ?
            ErrCode=0x60;
            goto AtomDecoding_Bytes_err;
        }
        if((byte&0xf8)==0xd0){ //byte sequence
            for(j=0;j<*length;j++){
                DecBuf[j] = mCmdPkt.payload[iPload++];
            }
        }else if((byte&0xf0)==0xd8){
            // TODO: Continued into another atom

            //return error temporarily
            ErrCode=0x70;
            goto AtomDecoding_Bytes_err;
        }
        else
        {
            ErrCode=0x80;
            goto AtomDecoding_Bytes_err;
        }
    }
    else if((byte&0xfc)==0xe0)
    {  //long atom
        //TODO
        *((U8 *)length+2)=mCmdPkt.payload[iPload++];
        *((U8 *)length+1)=mCmdPkt.payload[iPload++];
        *((U8 *)length+0)=mCmdPkt.payload[iPload++];
        if(*length > sizeof(DecBuf)){ // over ary size ?
            ErrCode=0x90;
            goto AtomDecoding_Bytes_err;
        }
        if((byte&0xfe)==0xe2){ //byte sequence
            for(j=0;j<*length;j++){
                DecBuf[j] = mCmdPkt.payload[iPload++];
            }
        }else if((byte&0xf0)==0xe3){
            // TODO: Continued into another atom

            //return error temporarily
            ErrCode=0xa0;
            goto AtomDecoding_Bytes_err;
        }
        else
        {
            ErrCode=0xb0;
            goto AtomDecoding_Bytes_err;
        }
    }
    else
    {
        ErrCode=0xc0;    // unknow Token
        goto AtomDecoding_Bytes_err;
    }
    return STS_SUCCESS;

AtomDecoding_Bytes_err:
    soutb3(0x5C, 0xFE, 0x08); soutb(ErrCode);
    TCG_PRINTF("TokenDecoding err %2X\n",ErrCode);
    if(ErrCode!=0x00){
        return STS_INVALID_PARAMETER;
    }
    return STS_SUCCESS;
}

int AtomDecoding_Int(U32* length)
{
    U8 byte;
    U32 j,n;
    U8 ErrCode=0x00;  //no error

    byte=mCmdPkt.payload[iPload++];

    if((byte&0x80)==0x00){  //Tiny atom, 0~63 or -32~31
        if(byte&0x40){ //sign bit indicator
            if((byte & 0x20)){     //chk Sign bit
                DecBuf[0] = (byte & 0x1f) | 0xe0;   //negative
            }else{
                DecBuf[0] = (byte & 0x1f);          //positive
            }
        }else{
            DecBuf[0] = byte&0x3F;
        }
        *length=1;
    }
    else if((byte&0xc0)==0x80)
    { //Short atom
        //TODO
        *length=byte&0x0f;
        if(*length > sizeof(DecBuf)){ // over ary size ?
            ErrCode=0x20;
            goto AtomDecoding_Int_err;
            }
        if((byte&0xe0)==0xA0){ //byte sequence
            ErrCode=0x30;
            goto AtomDecoding_Int_err;
        }
        else
        {
            if(*length <= 8){
                for(j=*length; j>0; j--){
                    DecBuf[j-1] = mCmdPkt.payload[iPload++];
                }
            }else{
                for(j=0; j<*length; j++){
                    if(mCmdPkt.payload[iPload]==0){
                        iPload++;
                    }else{
                        break;
                    }
                }
                n=*length-j;
                if(n<=8){
                for(j=*length; j>0; j--){
                        DecBuf[j-1] = mCmdPkt.payload[iPload++];
                }
                    *length = n; // adjust *length for return
            }else{
                    ErrCode=0x40;    // no support data format
                    goto AtomDecoding_Int_err;
                }
            }
        }
    }
    else if((byte&0xe0)==0xc0)
    { //Medium atom
        //TODO
        *((U8 *)length+1)=byte&0x07;
        *((U8 *)length+0)=mCmdPkt.payload[iPload++];
        if(*length > sizeof(DecBuf)){ // over ary size ?
            ErrCode=0x50;
            goto AtomDecoding_Int_err;
            }
        if((byte&0xf0)==0xd0){ //byte sequence
            ErrCode=0x60;
            goto AtomDecoding_Int_err;
        }
        else
        {
            if(*length <= 8){
                for(j=*length; j>0; j--){
                    DecBuf[j-1] = mCmdPkt.payload[iPload++];
                }
            }else{
                for(j=0; j<*length; j++){
                    if(mCmdPkt.payload[iPload]==0){
                        iPload++;
                    }else{
                        break;
                    }
                }
                n=*length-j;
                if(n<=8){
                    for(j=*length; j>0; j--){
                        DecBuf[j-1] = mCmdPkt.payload[iPload++];
                }
                    *length = n; // adjust *length for return
            }else{
                    ErrCode=0x70;    // no support data format
                    goto AtomDecoding_Int_err;
                }
            }
        }
    }
    else if((byte&0xfc)==0xe0)
    {  //long atom
        //TODO
        *((U8 *)length+2)=mCmdPkt.payload[iPload++];
        *((U8 *)length+1)=mCmdPkt.payload[iPload++];
        *((U8 *)length+0)=mCmdPkt.payload[iPload++];
        if(*length > sizeof(DecBuf)){ // over ary size ?
            ErrCode=0x80;
            goto AtomDecoding_Int_err;
        }
        if((byte&0xfe)==0xe2){ //byte sequence
            ErrCode=0x90;
            goto AtomDecoding_Int_err;
        }
        else
        {
            if(*length <= 8){
                for(j=*length; j>0; j--){
                    DecBuf[j-1] = mCmdPkt.payload[iPload++];
                }
            }else{
                for(j=0; j<*length; j++){
                    if(mCmdPkt.payload[iPload]==0){
                        iPload++;
                    }else{
                        break;
                    }
                }
                n=*length-j;
                if(n<=8){
                for(j=*length; j>0; j--){
                        DecBuf[j-1] = mCmdPkt.payload[iPload++];
                }
                    *length = n; // adjust *length for return
            }else{
                    ErrCode=0xA0;    // no support data format
                    goto AtomDecoding_Int_err;
                }
            }
        }
    }
    else
    {
        ErrCode=0xB0;    // unknow Token
        goto AtomDecoding_Int_err;
    }
    return STS_SUCCESS;

AtomDecoding_Int_err:
    soutb3(0x5C, 0xFE, 0x08); soutb(ErrCode);
    TCG_PRINTF("TokenDecoding err:%2X\n",ErrCode);
    if(ErrCode!=0x00){
        return STS_INVALID_PARAMETER;
    }
    return STS_SUCCESS;
}

int SkipTokenEmpty(void)
{
    while(mCmdPkt.payload[iPload]==TOK_Empty){
        iPload++;
        if(iPload >= mCmdPkt.mSubPktFmt.length){
            return zNG;
        }
    }
    return zOK;
}

//Atom Decoding, only accept "Byte" and the byte length must be "8"
U16 AtomDecoding_Uid2(U8* data)
{
    U8 byte;
    U8 j;
    //U16 result;
    U32 len=0;

    for(;;)
    {
        if(iPload >= mCmdPkt.mSubPktFmt.length)
            return STS_SESSION_ABORT;

        byte = mCmdPkt.payload[iPload++];
        if(byte!=0xff)
            break;
    }

    if((byte&0xF0)==0xA0)
    { // short byte
        len = byte&0x0F;
    }
    else if((byte&0xF8)==0xD0)
    { //medium byte
        HiByte(LoWord(len))=byte&0x07;
        LoByte(LoWord(len))=mCmdPkt.payload[iPload++];
    }
    else if(byte==0xE2)
    { //long byte
        LoByte(HiWord(len))=mCmdPkt.payload[iPload++];
        HiByte(LoWord(len))=mCmdPkt.payload[iPload++];
        LoByte(LoWord(len))=mCmdPkt.payload[iPload++];
    }
    else
        return STS_SESSION_ABORT;

    if(len!=0x08)
        return STS_SESSION_ABORT;

    for(j=8; j>0; j--)
        *(data+j-1)=mCmdPkt.payload[iPload++];

    return STS_SUCCESS;
}

//Atom Decoding, only accept "UINT8"  (the data length must be "1")
U16 AtomDecoding_Uint8(U8* data)
{
    U8 byte;
    U32 j, len = 0;

    for(;;)
    {
        if(iPload >= mCmdPkt.mSubPktFmt.length)
        {
            byte = 0x10;
            goto AtomDecoding_Uint_Abort;
        }

        byte = mCmdPkt.payload[iPload++];
        if(byte!=0xff)
            break;
    }

    if(byte<64)
    {  //Tiny atom, 0~63
        *data = byte;
        return STS_SUCCESS;
    }
    else if((byte&0xF0)==0x80)
    { //Short Atom, unsigned integer
        len=byte&0x0f;
    }
    else if((byte&0xF8)==0xc0)
    { //Medium Atom, unsigned integer

        HiByte(LoWord(len))=byte&0x07;
        LoByte(LoWord(len))=mCmdPkt.payload[iPload++];
    }
    else if(byte==0xe0)
    {  //Long Atom, unsigned integer
        LoByte(HiWord(len))=mCmdPkt.payload[iPload++];
        HiByte(LoWord(len))=mCmdPkt.payload[iPload++];
        LoByte(LoWord(len))=mCmdPkt.payload[iPload++];
    }
    else
    { // unknow Token
        byte = 0x20;
        goto AtomDecoding_Uint_Abort;
    }

    if(len==0)
    {
        byte = 0x60;
        goto AtomDecoding_Uint_Err2;
    }

    //check if data length is larger than "1"
    for(j=0; j<(len-1); j++)
    {
        if(mCmdPkt.payload[iPload++]!=0)
        { //the data length is larger than "1"
            byte = 0x70;
            goto AtomDecoding_Uint_Err2;
        }
    }

    *data = mCmdPkt.payload[iPload++];

    return STS_SUCCESS;

AtomDecoding_Uint_Abort:
    soutb3(0x5C, 0xFE, 0x08); soutb(0xA1);
    TCG_PRINTF("TokenDecoding A1 err\n");
    return STS_SESSION_ABORT;

AtomDecoding_Uint_Err2:
    soutb3(0x5C, 0xFE, 0x08); soutb(0xB1);
    TCG_PRINTF("TokenDecoding B1 err\n");
    return STS_INVALID_PARAMETER;
}

//Atom Decoding, only accept "UINT32"  (the data length must be "4")
U16 AtomDecoding_Uint32(U32* data)
{
    U8 byte;
    U32 j, len=0;
//souts("spkt:"); soutb(mCmdPkt.mSubPktFmt.length);
    for(;;)
    {
        if(iPload >= mCmdPkt.mSubPktFmt.length)
        {
            byte = 0x10;
            goto AtomDecoding_Uint32_Abort;
        }

        byte = mCmdPkt.payload[iPload++];
        if(byte!=0xff)
            break;
    }

    if(byte<64)
    {  //Tiny atom, 0~63
        *data = byte;
        return STS_SUCCESS;
    }
    else if((byte&0xF0)==0x80)
    { //Short Atom, unsigned integer
        len=byte&0x0f;
    }
    else if((byte&0xF8)==0xc0)
    { //Medium Atom, unsigned integer
        HiByte(LoWord(len))=byte&0x07;
        LoByte(LoWord(len))=mCmdPkt.payload[iPload++];
    }
    else if(byte==0xe0)
    {  //Long Atom, unsigned integer
        LoByte(HiWord(len))=mCmdPkt.payload[iPload++];
        HiByte(LoWord(len))=mCmdPkt.payload[iPload++];
        LoByte(LoWord(len))=mCmdPkt.payload[iPload++];
    }
    else
    { // unknow Token
        byte = 0x20;
        goto AtomDecoding_Uint32_Abort;
    }

    if(len==0)
    {
        byte = 0x60;
        goto AtomDecoding_Uint32_Err2;
    }

    //check if data length is larger than "4"
    if(len>4)
    {
        for(j=0; j<(len-4); j++)
        {
            if(mCmdPkt.payload[iPload++]!=0)
            { //the data length is larger than "4"
                byte = 0x70;
                goto AtomDecoding_Uint32_Err2;
            }
        }
        len = 4;
    }

    *data = 0;

    for(j=len;j>0;j--)
        *((U8 *)data+j-1) = mCmdPkt.payload[iPload++];

    return STS_SUCCESS;

AtomDecoding_Uint32_Abort:
    soutb3(0x5C, 0xFE, 0x08); soutb(byte);
    TCG_PRINTF("TokenDecoding err=%2X\n", byte);
    return STS_SESSION_ABORT;

AtomDecoding_Uint32_Err2:
    soutb3(0x5C, 0xFE, 0x08); soutb(byte);
    TCG_PRINTF("TokenDecoding error %2X\n", byte);
    return STS_INVALID_PARAMETER;
}

// skip Empty Token (0xff), and return the next non-empty payload
// (iPload will be updated to the non-empty payload)
U8 ChkToken(void)
{
    U8 data;
    for(;;)
    {
        data = mCmdPkt.payload[iPload++];
        if(data!=0xff)
            return data;

        if(iPload > mCmdPkt.mSubPktFmt.length)
            return 0xff;    //NG
    }
}

/***********************************************************
* AdminSP Table Method decodes and processes
* return 0 if success
***********************************************************/
U16 InvokingAdminTable(void)
{
    U16 result=STS_SUCCESS;
    int status;

    if(methodUID.all!=UID_MethodID_GetACL)
    {
        //1. AccessControl checking, get ACL here
        status=AdminAccessControlCheck();
        soutb3(0x5C, 0xFE, 0xD0); soutb(status);
        soutb3(0x5C, 0xFE, 0xEA);  sout64(invokingUID.all); sout64(methodUID.all);
        TCG_PRINTF("Admin AccessControl Check: %2X\n", status);
        TCG_PRINTF("inv[%X%X] mtd[%X%X]\n", (U32)(invokingUID.all>>32), (U32)(invokingUID.all), (U32)(methodUID.all>>32), (U32)(methodUID.all));
        if(status == zNG)
        {
            if(methodUID.all!=UID_MethodID_Get)     //test cases 3.1.5
                 return STS_NOT_AUTHORIZED;   //0x01
            else
            {
                dataBuf[iDataBuf++]=TOK_StartList;
                dataBuf[iDataBuf++]=TOK_EndList;
                dataBuf[iDataBuf++]=TOK_EndOfData;
                return STS_SUCCESS;
            }
        }

        //2. ACE BooleanExpr checking
        status=AdminAceBooleanExprChk();
        soutb3(0x5C, 0xFE, 0xD1);  soutb(status);
        TCG_PRINTF("Admin ACE BooleanExp Check: %2X\n", status);
        //sout64(aclUid[0]); sout64(aclUid[1]);
        if(status == zNG)
        {
            if(methodUID.all!=UID_MethodID_Get)     //test cases 3.1.5
                 return STS_NOT_AUTHORIZED;   //0x01
            else
            {
                dataBuf[iDataBuf++]=TOK_StartList;
                dataBuf[iDataBuf++]=TOK_EndList;
                dataBuf[iDataBuf++]=TOK_EndOfData;
                return STS_SUCCESS;
            }
        }
    }

    //3. Authority checking
    //result=AdminAuthorityCheck();
    //if(result == zNG) return zNG;

    //4. Method Operation (decode the parameters, then do the required action!)
    switch (methodUID.all)
    {
    case UID_MethodID_Next:
        result=Method_Next();
        break;

    case UID_MethodID_GetACL:
        result=Method_GetACL();
        break;

    case UID_MethodID_Get:
        result=Method_Get();
        break;

    case UID_MethodID_Set:
        if(mSessionManager.Write==0x00){   // check write bit in start session payload
            return STS_INVALID_METHOD;
        }
        result=Method_Set();
        if(result==STS_SUCCESS){
            if(mSessionManager.TransactionState==TRNSCTN_IDLE){
                WriteMtable2NAND();
                ClearMtableChangedFlag();
            }
            //else{ }  //transcation active
        }else{
            ReadNAND2Mtable();
            ClearMtableChangedFlag();
        }
        break;

    case UID_MethodID_Revert:
        result=Method_Revert();
        break;

    case UID_MethodID_Activate:
        result=Method_Activate();
        break;

    case UID_MethodID_Authenticate:
        result=Method_Authenticate();
        break;

    case UID_MethodID_Random:
        result=Method_Random();
        break;
    }
    return result;
}

/***********************************************************
* InvokingLockingTable
***********************************************************/
U16 InvokingLockingTable(void)
{
    U16 result=STS_SUCCESS;
    int status;

    if(methodUID.all!=UID_MethodID_GetACL)
    {
        //1. AccessControl checking, get ACL here
        status=LockingAccessControlCheck();
        soutb3(0x5C, 0xFE, 0xE0);  soutb(status);
        soutb3(0x5C, 0xFE, 0xEA);  sout64(invokingUID.all); sout64(methodUID.all);
        TCG_PRINTF("Locking AccessControl Check: %2X\n", status);
        TCG_PRINTF("inv[%X%X] mtd[%X%X]\n", (U32)(invokingUID.all>>32), (U32)(invokingUID.all), (U32)(methodUID.all>>32), (U32)(methodUID.all));

        if(status == zNG)
        {
            if(methodUID.all!=UID_MethodID_Get)     //test cases 3.1.5
                 return STS_NOT_AUTHORIZED;   //0x01
            else
            {
                dataBuf[iDataBuf++]=TOK_StartList;
                dataBuf[iDataBuf++]=TOK_EndList;
                dataBuf[iDataBuf++]=TOK_EndOfData;
                return STS_SUCCESS;
            }
        }

        //2. ACE BooleanExpr checking
        status=LockingAceBooleanExprChk();
        soutb3(0x5C, 0xFE, 0xE1);  soutb(status);
        TCG_PRINTF("Locking ACE BooleanExp Check: %2X\n", status);
        //sout64(aclUid[0]); sout64(aclUid[1]);
        if(status == zNG)
        {
            if(methodUID.all!=UID_MethodID_Get)     //test cases 3.1.5
                 return STS_NOT_AUTHORIZED;   //0x01
            else
            {
                dataBuf[iDataBuf++]=TOK_StartList;
                dataBuf[iDataBuf++]=TOK_EndList;
                dataBuf[iDataBuf++]=TOK_EndOfData;
                return STS_SUCCESS;
            }
        }
    }

    //3. Authority checking
    //result=AdminAuthorityCheck();
    //if(result == zNG) return zNG;

    //4. Method Operation (decode the parameters, then do the required action!)
    switch (methodUID.all)
    {
    case UID_MethodID_Next:
        result=Method_Next();
        break;

    case UID_MethodID_GetACL:
        result=Method_GetACL();
        break;

    case UID_MethodID_GenKey:
        result=Method_GenKey();
        break;

    case UID_MethodID_RevertSP:
        result=Method_RevertSP();
        break;

    case UID_MethodID_Get:
        result=Method_Get();
        break;

    case UID_MethodID_Set:
        if(mSessionManager.Write==0x00){   // check write bit in start session payload
            return STS_INVALID_METHOD;
        }
        result=Method_Set();
        if(result==STS_SUCCESS){
            if(mSessionManager.TransactionState==TRNSCTN_IDLE){
                WriteMtable2NAND();
                ClearMtableChangedFlag();

            //cj tested [
                if((invokingUID.all>>32)==(UID_Locking>>32)) {
                    LockingRangeTable_Update();
                }

                if((invokingUID.all>>32)==(UID_MBRControl>>32))
                {
                    if((G3.b.mLckMbrCtrl_Tbl.val[0].enable==TRUE)&&(G3.b.mLckMbrCtrl_Tbl.val[0].done==FALSE))
                        mTcgStatus |= MBR_SHADOW_MODE;
                    else
                        mTcgStatus &= (~MBR_SHADOW_MODE);
                }
            //]
            }
            //else {
                //Core 3.3.7.3:
                //    Changes made within a transaction SHALL be visible within that transaction.
                // If the transaction is aborted, those changes SHALL be rolled back.
                //    Changes that affect other aspects of the TPer (i.e. hardware settings) SHALL occur when associated
                // changes are successfully committed.
            //} //transcation active


        }else{
            ReadNAND2Mtable();
            ClearMtableChangedFlag();
        }
        break;

    case UID_MethodID_Authenticate:
        result=Method_Authenticate();
        break;

    case UID_MethodID_Random:
        result=Method_Random();
        break;

    case UID_MethodID_Reactivate:
        result=Method_Reactivate();
        break;

    case UID_MethodID_Erase:
        result=Method_Erase();
        break;

    }
    return result;
}

int AdminAccessControlCheck(void)
{
    //BOOL invIdIsFound=FALSE;
    register U16 i;
    U8 j;

    for(i=0;i<G1.b.mAdmAccessCtrl_Tbl.rowCnt;i++)
    { // search for InvokingID
        if(G1.b.mAdmAccessCtrl_Tbl.val[i].invID == invokingUID.all)
        {
            //invIdIsFound=TRUE;
            if(G1.b.mAdmAccessCtrl_Tbl.val[i].mtdID==methodUID.all)
            {    // InvokingID/MethodID ok, store ACL list and getAclAcl uid for ACE checking
                for(j=0;j<ADM_ACCESSCTRL_ACL_CNT;j++)
                    aclUid[j]=G1.b.mAdmAccessCtrl_Tbl.val[i].acl[j];

                getAclAclUid=G1.b.mAdmAccessCtrl_Tbl.val[i].getAclAcl;

                return zOK;
            }
        }
        else if((G1.b.mAdmAccessCtrl_Tbl.val[i].invID>>32) > (invokingUID.all>>32))
        {
            //if(invIdIsFound) //out of InvokingUID search, NG
                break;
        }
    }
    return zNG;  //NG
}

int LockingAccessControlCheck(void)
{
    //BOOL invIdIsFound=FALSE;
    register U16 i;
    U8 j;

    for(i=0;i<G2.b.mLckAccessCtrl_Tbl.rowCnt;i++)
    { // search for InvokingID
        if(G2.b.mLckAccessCtrl_Tbl.val[i].invID == invokingUID.all)
        {
            //invIdIsFound=TRUE;
            if(G2.b.mLckAccessCtrl_Tbl.val[i].mtdID==methodUID.all)
            {    //InvokingID/MethodID ok, store ACL list and getAclAcl uid
                for(j=0;j<LCK_ACCESSCTRL_ACL_CNT;j++)
                    aclUid[j]=G2.b.mLckAccessCtrl_Tbl.val[i].acl[j];

                getAclAclUid=G2.b.mLckAccessCtrl_Tbl.val[i].getAclAcl;

                return zOK;
            }
        }
        else if((G2.b.mLckAccessCtrl_Tbl.val[i].invID>>32) > (invokingUID.all>>32))
        {
            //if(invIdIsFound) //out of InvokingUID search, NG
                break;
        }
    }
    return zNG;  //NG
}

//
// check BooleanExpr column in ACE table with the HostSigningAuthority
// Return zOK if the authority is passed, else return zNG.
//
//
// 1. look up ACE table object from aclUid
// 2. get the booleanExpr of that ACE table object for authority check
// 3. return OK if requested authority is "Anybody", or requested authority is the same as host signed in,
//
int AdminAceBooleanExprChk(void)
{
    register U16 i;
    U8 j,iAcl;
    U64 tmp64;     //wierd!! "if-adjustment" NG if compared directly

    if(methodUID.all!=UID_MethodID_GetACL)
    { //not GetACL method, check ACL list
        if(aclUid[1]==0) //only one uid in ACL list
            authorityChkIsDone=TRUE;
        else
            authorityChkIsDone=FALSE;

        for(iAcl=0;iAcl<ADM_ACCESSCTRL_ACL_CNT;iAcl++)
        {
            if(aclUid[iAcl]==UID_Null)
                return zNG;

            for(i=0;i<G1.b.mAdmACE_Tbl.rowCnt;i++)
            {
                if(G1.b.mAdmACE_Tbl.val[i].uid==aclUid[iAcl])
                {
                    iAceRow = i;    //store the index for ACE "Column" check

                    //check BooleanExpr
                    for(j=0;j<ADM_ACE_BOOLEXPR_CNT;j++)
                    {
                        //if(G1.b.mAdmACE_Tbl.val[i].booleanExpr[j]==UID_Null)
                        //  break;      //for j-loop
                        //else if((G1.b.mAdmACE_Tbl.val[i].booleanExpr[j]==UID_Authority_Anybody)||
                        //   (G1.b.mAdmACE_Tbl.val[i].booleanExpr[j]==mSessionManager.HtSgnAuthority.all))
                        //  return zOK; //at least one authority from ACL list is PASS!

                        tmp64 = G1.b.mAdmACE_Tbl.val[i].booleanExpr[j];

                        if(tmp64==UID_Null)
                            break;      //for j-loop
                        else if((tmp64==UID_Authority_Anybody)||
                                (tmp64==mSessionManager.HtSgnAuthority.all))
                            return zOK; //at least one authority from ACL list is PASS!
                    }

                    break;  //for i-loop
                }
            }
        }
    }
    else
    { //GetACLACL
        for(i=0;i<G1.b.mAdmACE_Tbl.rowCnt;i++)
        {
            if(G1.b.mAdmACE_Tbl.val[i].uid==getAclAclUid)
            {
                //check BooleanExpr
                for(j=0;j<ADM_ACE_BOOLEXPR_CNT;j++)
                {
                    /*if(G1.b.mAdmACE_Tbl.val[i].booleanExpr[j]==UID_Null)
                        break;      //for j-loop
                    else if((G1.b.mAdmACE_Tbl.val[i].booleanExpr[j]==UID_Authority_Anybody)||
                       (G1.b.mAdmACE_Tbl.val[i].booleanExpr[j]==mSessionManager.HtSgnAuthority.all))
                        return zOK; //at least one authority from ACL list is PASS!       */

                    tmp64 = G1.b.mAdmACE_Tbl.val[i].booleanExpr[j];
                    if(tmp64==UID_Null)
                        break;      //for j-loop
                    else if((tmp64==UID_Authority_Anybody)||
                            (tmp64==mSessionManager.HtSgnAuthority.all))
                        return zOK; //at least one authority from ACL list is PASS!
                }

                break;  //for i-loop
            }
        }
    }

    return zNG;
}

// 1. look up ACE table object from aclUid
// 2. get the booleanExpr of that ACE table object for authority check
// 3. return OK if requested authority is "Anybody", or requested authority is the same as host signed in,
//    or requrested authority by the same class (Admins or Users).
int LockingAceBooleanExprChk(void)
{
    register U16 i;
    U8 j,iAcl;
    U64 tmp64;     //wierd!! "if-adjustment" NG if compared directly

    if(methodUID.all!=UID_MethodID_GetACL)
    { //not GetACL method, check ACL list
        if(aclUid[1]==0) //only one uid in ACL list
            authorityChkIsDone=TRUE;
        else
            authorityChkIsDone=FALSE;

        for(iAcl=0;iAcl<LCK_ACCESSCTRL_ACL_CNT;iAcl++)
        {
            if(aclUid[iAcl]==UID_Null)
                return zNG;

            for(i=0;i<G3.b.mLckACE_Tbl.rowCnt;i++)
            {
                if(G3.b.mLckACE_Tbl.val[i].uid==aclUid[iAcl])
                {
                    iAceRow = i;    //store the index for ACE "Column" check

                    for(j=0;j<LCK_ACE_BOOLEXPR_CNT;j++)
                    {
                      /*if(G3.b.mLckACE_Tbl.val[i].booleanExpr[j]==0) //no more BooleanExpr list
                            break;          //for j-loop
                        else if((G3.b.mLckACE_Tbl.val[i].booleanExpr[j]==UID_Authority_Anybody)||
                           (G3.b.mLckACE_Tbl.val[i].booleanExpr[j]==mSessionManager.HtSgnAuthority.all)||
                                (G3.b.mLckACE_Tbl.val[i].booleanExpr[j]==mSessionManager.HtAuthorityClass)) */

                        tmp64=G3.b.mLckACE_Tbl.val[i].booleanExpr[j];
                        if(tmp64==0) //no more BooleanExpr list
                            break;          //for j-loop
                        else if((tmp64==UID_Authority_Anybody)||
                                (tmp64==mSessionManager.HtSgnAuthority.all)||
                                (tmp64==mSessionManager.HtAuthorityClass))
                            return zOK;     //at least one authority from ACL list is PASS!
                    }

                    break;  //for i-loop
                }
            }
        }
    }
    else
    { //GetACLACL
        for(i=0;i<G3.b.mLckACE_Tbl.rowCnt;i++)
        {
            if(G3.b.mLckACE_Tbl.val[i].uid==getAclAclUid)
            {
                //check BooleanExpr
                for(j=0;j<LCK_ACE_BOOLEXPR_CNT;j++)
                {
                    /* if(G3.b.mLckACE_Tbl.val[i].booleanExpr[j]==0) //no more BooleanExpr list
                        break;          //for j-loop
                    else if((G3.b.mLckACE_Tbl.val[i].booleanExpr[j]==UID_Authority_Anybody)||
                       (G3.b.mLckACE_Tbl.val[i].booleanExpr[j]==mSessionManager.HtSgnAuthority.all)||
                       (G3.b.mLckACE_Tbl.val[i].booleanExpr[j]==mSessionManager.HtAuthorityClass))
                        return zOK;     //at least one authority from ACL list is PASS!     */

                    tmp64=G3.b.mLckACE_Tbl.val[i].booleanExpr[j];
                    if(tmp64==0) //no more BooleanExpr list
                        break;          //for j-loop
                    else if((tmp64==UID_Authority_Anybody)||
                            (tmp64==mSessionManager.HtSgnAuthority.all)||
                            (tmp64==mSessionManager.HtAuthorityClass))
                        return zOK;     //at least one authority from ACL list is PASS!
                }

                break;  //for i-loop
            }
        }
    }

    return zNG;
}

//
// update statusCode[] from payload and check if status = "F0 00 00 00 F1"
// return STS_SUCCESS if ok.
//
U16 ChkMethodStatus(void)
{
    if(ChkToken()!=TOK_StartList)
        return STS_SESSION_ABORT;

    //statusCode[0]=mCmdPkt.payload[iPload++];    //TODO:
    //statusCode[1]=mCmdPkt.payload[iPload++];
    //statusCode[2]=mCmdPkt.payload[iPload++];
    if(AtomDecoding_Uint8(&statusCode[0])!=STS_SUCCESS)
        return STS_SESSION_ABORT;

    if(AtomDecoding_Uint8(&statusCode[1])!=STS_SUCCESS)
        return STS_SESSION_ABORT;

    if(AtomDecoding_Uint8(&statusCode[2])!=STS_SUCCESS)
        return STS_SESSION_ABORT;

    if(statusCode[0]!=0x00) //status NG, method call fail!
        return statusCode[0];

    if(ChkToken()!=TOK_EndList)
        return STS_SESSION_ABORT;
    else
        return STS_SUCCESS;
}

U16 Method_Next(void)
{
    U16 result;
    U8 *ptTblObj=NULL, byte;
    U16  rowCnt=0, objSize=0;
    U8 iRow, i;
    UID64   where;
    U64 tmp64;
    U8 count=0, nextCnt=0;
    BOOL p1Found=FALSE, p2Found=FALSE;
    U32    tmp32;

    //parameter check
    if(ChkToken()!=TOK_StartList)    //test cases 3.1.5
        return STS_SESSION_ABORT;

    where.all=0x00;

    byte = ChkToken();
    if(byte==TOK_EndList)
        goto END_LIST;

    else if(byte==TOK_StartName)
    {//optional parameters: where / count
        if (AtomDecoding_Uint8(&byte)!=STS_SUCCESS)
            return STS_INVALID_PARAMETER;

        if(byte==0x00)
        { // where
            if(AtomDecoding_Uid2(where.bytes)!=STS_SUCCESS)
                return STS_INVALID_PARAMETER;

            if(ChkToken()!=TOK_EndName)
                return STS_INVALID_PARAMETER;

            p1Found=TRUE;

            byte = ChkToken();
            if(byte==TOK_StartName) //next option parameter
            {
                if (AtomDecoding_Uint8(&byte)!=STS_SUCCESS)
                    return STS_INVALID_PARAMETER;
            }
        }

        if(byte==0x01)
        { // count
            if (AtomDecoding_Uint8(&count)!=STS_SUCCESS)
                return STS_INVALID_PARAMETER;

            if(ChkToken()!=TOK_EndName)
                return STS_INVALID_PARAMETER;

            p2Found=TRUE;

            byte = ChkToken();
        }

        if(byte!=TOK_EndList)
            return STS_SESSION_ABORT;

    }
    else //NG within parameters
        return STS_SESSION_ABORT;   //A6-1-5-2-1, STS_INVALID_PARAMETER;

END_LIST:

    if(ChkToken()!=TOK_EndOfData)
        return STS_SESSION_ABORT;

    //status list check
    result = ChkMethodStatus();
    if(result!=STS_SUCCESS)
        return result;


    //method execution   :
soutb3(0x5C, 0xFE, 0x0E);  //soutb2(startCol, endCol);
    TCG_PRINTF("Method->Next\n");

    //prepare payload for response
    dataBuf[iDataBuf++]=TOK_StartList;
    dataBuf[iDataBuf++]=TOK_StartList;

    tmp32 = invokingUID.all >>32;
    if(mSessionManager.SPID.all==UID_SP_Admin)
    {
        switch(tmp32)
        {
        case 0x0001:
            ptTblObj = (U8 *)G1.b.mAdmTbl_Tbl.val;
            rowCnt = G1.b.mAdmTbl_Tbl.rowCnt;
            objSize = G1.b.mAdmTbl_Tbl.objSize;

            if(p1Found==FALSE)
                where.all = G1.b.mAdmTbl_Tbl.val[0].uid; //1st row

            if(p2Found==FALSE)
                count = G1.b.mAdmTbl_Tbl.rowCnt;
            break;

        //case 0x0002:  //SPInfo, no support
        //    break;

        case 0x0003:
            ptTblObj = (U8 *)G1.b.mAdmSPTemplates_Tbl.val;
            rowCnt = G1.b.mAdmSPTemplates_Tbl.rowCnt;
            objSize = G1.b.mAdmSPTemplates_Tbl.objSize;

            if(p1Found==FALSE)
                where.all = G1.b.mAdmSPTemplates_Tbl.val[0].uid; //1st row

            if(p2Found==FALSE)
                count = G1.b.mAdmSPTemplates_Tbl.rowCnt;
        break;

    case 0x0006:
        ptTblObj = (U8 *)G1.b.mAdmMethod_Tbl.val;
        rowCnt = G1.b.mAdmMethod_Tbl.rowCnt;
        objSize = G1.b.mAdmMethod_Tbl.objSize;

        if(p1Found==FALSE)
            where.all = G1.b.mAdmMethod_Tbl.val[0].uid; //1st row

        if(p2Found==FALSE)
            count = G1.b.mAdmMethod_Tbl.rowCnt;
        break;

    case 0x0008:
        ptTblObj = (U8 *)G1.b.mAdmACE_Tbl.val;
        rowCnt = G1.b.mAdmACE_Tbl.rowCnt;
        objSize = G1.b.mAdmACE_Tbl.objSize;

        if(p1Found==FALSE)
            where.all = G1.b.mAdmACE_Tbl.val[0].uid; //1st row

        if(p2Found==FALSE)
            count = G1.b.mAdmACE_Tbl.rowCnt;
        break;

    case 0x0009:
        ptTblObj = (U8 *)G1.b.mAdmAuthority_Tbl.val;
        rowCnt = G1.b.mAdmAuthority_Tbl.rowCnt;
        objSize = G1.b.mAdmAuthority_Tbl.objSize;

        if(p1Found==FALSE)
            where.all = G1.b.mAdmAuthority_Tbl.val[0].uid; //1st row

        if(p2Found==FALSE)
            count = G1.b.mAdmAuthority_Tbl.rowCnt;
        break;

    case 0x000B:
        ptTblObj = (U8 *)G1.b.mAdmCPin_Tbl.val;
        rowCnt = G1.b.mAdmCPin_Tbl.rowCnt;
        objSize = G1.b.mAdmCPin_Tbl.objSize;

        if(p1Found==FALSE)
            where.all = G1.b.mAdmCPin_Tbl.val[0].uid; //1st row

        if(p2Found==FALSE)
            count = G1.b.mAdmCPin_Tbl.rowCnt;
        break;

    //case 0x0201:  //no support
    //    break;

    case 0x0204:
            ptTblObj = (U8 *)G1.b.mAdmTemplate_Tbl.val;
            rowCnt = G1.b.mAdmTemplate_Tbl.rowCnt;
            objSize = G1.b.mAdmTemplate_Tbl.objSize;

            if(p1Found==FALSE)
                where.all = G1.b.mAdmTemplate_Tbl.val[0].uid; //1st row

            if(p2Found==FALSE)
                count = G1.b.mAdmTemplate_Tbl.rowCnt;
            break;

        case 0x0205:
            ptTblObj = (U8 *)G1.b.mAdmSP_Tbl.val;
            rowCnt = G1.b.mAdmSP_Tbl.rowCnt;
            objSize = G1.b.mAdmSP_Tbl.objSize;

            if(p1Found==FALSE)
                where.all = G1.b.mAdmSP_Tbl.val[0].uid; //1st row

            if(p2Found==FALSE)
                count = G1.b.mAdmCPin_Tbl.rowCnt;
            break;
        }
    }
    else //if(mSessionManager.SPID.all==UID_SP_Locking)
    {
        switch(tmp32)
        {
        case 0x0001:
            ptTblObj = (U8 *)G2.b.mLckTbl_Tbl.val;
            rowCnt = G2.b.mLckTbl_Tbl.rowCnt;
            objSize = G2.b.mLckTbl_Tbl.objSize;

            if(p1Found==FALSE)
                where.all = G2.b.mLckTbl_Tbl.val[0].uid; //1st row

            if(p2Found==FALSE)
                count = G2.b.mLckTbl_Tbl.rowCnt;
            break;

        //case 0x0002:  //SPInfo, no support
        //    break;

        case 0x0003:
            ptTblObj = (U8 *)G2.b.mLckSPTemplates_Tbl.val;
            rowCnt = G2.b.mLckSPTemplates_Tbl.rowCnt;
            objSize = G2.b.mLckSPTemplates_Tbl.objSize;

            if(p1Found==FALSE)
                where.all = G2.b.mLckSPTemplates_Tbl.val[0].uid; //1st row

            if(p2Found==FALSE)
                count = G2.b.mLckSPTemplates_Tbl.rowCnt;
            break;

        case 0x0006:
            ptTblObj = (U8 *)G2.b.mLckMethod_Tbl.val;
            rowCnt = G2.b.mLckMethod_Tbl.rowCnt;
            objSize = G2.b.mLckMethod_Tbl.objSize;

            if(p1Found==FALSE)
                where.all = G2.b.mLckMethod_Tbl.val[0].uid; //1st row

            if(p2Found==FALSE)
                count = G2.b.mLckMethod_Tbl.rowCnt;
            break;

        case 0x0008:
            ptTblObj = (U8 *)G3.b.mLckACE_Tbl.val;
            rowCnt = G3.b.mLckACE_Tbl.rowCnt;
            objSize = G3.b.mLckACE_Tbl.objSize;

            if(p1Found==FALSE)
                where.all = G3.b.mLckACE_Tbl.val[0].uid; //1st row

            if(p2Found==FALSE)
                count = G3.b.mLckACE_Tbl.rowCnt;
            break;

        case 0x0009:
            ptTblObj = (U8 *)G3.b.mLckAuthority_Tbl.val;
            rowCnt = G3.b.mLckAuthority_Tbl.rowCnt;
            objSize = G3.b.mLckAuthority_Tbl.objSize;

            if(p1Found==FALSE)
                where.all = G3.b.mLckAuthority_Tbl.val[0].uid; //1st row

            if(p2Found==FALSE)
                count = G3.b.mLckAuthority_Tbl.rowCnt;
            break;

        case 0x000B:
            ptTblObj = (U8 *)G3.b.mLckCPin_Tbl.val;
            rowCnt = G3.b.mLckCPin_Tbl.rowCnt;
            objSize = G3.b.mLckCPin_Tbl.objSize;

            if(p1Found==FALSE)
                where.all = G3.b.mLckCPin_Tbl.val[0].uid; //1st row

            if(p2Found==FALSE)
                count = G3.b.mLckCPin_Tbl.rowCnt;
            break;

        case 0x001D:
            ptTblObj = (U8 *)G2.b.mLckSecretProtect_Tbl.val;
            rowCnt = G2.b.mLckSecretProtect_Tbl.rowCnt;
            objSize = G2.b.mLckSecretProtect_Tbl.objSize;

            if(p1Found==FALSE)
                where.all = G2.b.mLckSecretProtect_Tbl.val[0].uid; //1st row

            if(p2Found==FALSE)
                count = G2.b.mLckSecretProtect_Tbl.rowCnt;
            break;

        //case 0x0801:  //LockingInfo, no support
        //    break;

        case 0x0802:
             ptTblObj = (U8 *)G3.b.mLckLocking_Tbl.val;
             rowCnt = G3.b.mLckLocking_Tbl.rowCnt;
             objSize = G3.b.mLckLocking_Tbl.objSize;

             if(p1Found==FALSE)
                 where.all = G3.b.mLckLocking_Tbl.val[0].uid; //1st row

             if(p2Found==FALSE)
                 count = G3.b.mLckLocking_Tbl.rowCnt;
             break;

        //case 0x0803:  //MBRControl, no support
        //    break;
        }
    }
//souts("rowCnt="); soutb(rowCnt);
    p1Found = !p1Found; //true: uid is found!
#if 0
    for(iRow=0;iRow<rowCnt;iRow++)
    {
        if((p1Found==TRUE)&&(nextCnt<count))
        {
            dataBuf[iDataBuf++]= 0xA8;

            for(i=1;i<=8;i++)
                dataBuf[iDataBuf++]= *(ptTblObj+8-i);

            nextCnt++;
        }

        for(i=0;i<8;i++)
            *(((U8 *)&tmp64)+i)=*(ptTblObj+i);

        if(tmp64==where.all) //the table object is found!
            p1Found = TRUE;

        ptTblObj += objSize;
    }
#else
    for(iRow=0;iRow<rowCnt;iRow++)
    {
        //tmp64 = *(U64 *)ptTblObj;
        for(i=0;i<8;i++)
            *(((U8 *)&tmp64)+i)=*(ptTblObj+i);

        if(p1Found==FALSE)
        {
            if(tmp64==where.all) //the table object is found!
                p1Found = TRUE;
        }
        else
        {
//souts("iRow"); soutb(iRow);
            if(nextCnt<count)
            {
                if(((U32)(tmp64>>32))==tmp32)
                {
                    dataBuf[iDataBuf++]= 0xA8;

                    for(i=1;i<=8;i++)
                        dataBuf[iDataBuf++]= *(ptTblObj+8-i);

                    nextCnt++;
                }
//                else { soutb(0xff);    sout64(tmp64);}
            }
            else
                break;
        }

        ptTblObj += objSize;
    }

#endif
    dataBuf[iDataBuf++]=TOK_EndList;
    dataBuf[iDataBuf++]=TOK_EndList;
    dataBuf[iDataBuf++]=TOK_EndOfData;

    if(p1Found==FALSE)
        return STS_INVALID_PARAMETER;
    else
    return STS_SUCCESS;
}

U16 Method_Get(void)
{
    U64 tmp64;
    U32 startCol=0, endCol=0xffffffff;
    U32 startRow=0, endRow=0xffffffff, i32=0, maxRow=0;
    U32 byteSize, tblKind;
    U8 *ptColPty=NULL, *ptTblObj=NULL;

    U16 result;
    U8  rowCnt=0, colCnt=0, objSize=0, namedValue=0, maxCol=0;
    U8  byte, boolExprCnt, dataType;


    //result=zNG;

    //retrieve the start/end column from the payload first...
    if(ChkToken()!=TOK_StartList)
        return STS_SESSION_ABORT;
    if(ChkToken()!=TOK_StartList)
       return STS_SESSION_ABORT;

    if((invokingUID.all&0xfffffff0ffffffff)==UID_DataStoreType || invokingUID.all==UID_MBR)
        tblKind=TBL_K_BYTE;
    else
        tblKind=TBL_K_OBJECT;

    //Named Value: Table (0x00), startRow (0x01), endRow (0x02), startCol (0x03), endCol (0x04)

    byte = ChkToken();
    if(byte==TOK_StartName)
    { //retrieve start/end column parameters

        if(tblKind==TBL_K_OBJECT)
        { //Object Table, retrieve startCol or endCol only
            if (AtomDecoding_Uint8(&namedValue)!=STS_SUCCESS)
                return STS_INVALID_PARAMETER;

            if(namedValue==0x03) //startCol
            {
                if (AtomDecoding_Uint32(&startCol)!=STS_SUCCESS)
                    return STS_INVALID_PARAMETER;

                if(ChkToken()!=TOK_EndName)
                    return STS_INVALID_PARAMETER;

                byte = ChkToken();
                if(byte==TOK_EndList) //no more parameters
                    goto END_LIST;
                else if(byte!=TOK_StartName)
                    return STS_SESSION_ABORT;   //A6-1-5-2-1    //STS_INVALID_PARAMETER;

                if (AtomDecoding_Uint8(&namedValue)!=STS_SUCCESS)
                    return STS_INVALID_PARAMETER;
            }

            if(namedValue==0x04) //endCol
            {
                if (AtomDecoding_Uint32(&endCol)!=STS_SUCCESS)
                    return STS_INVALID_PARAMETER;

                if(ChkToken()!=TOK_EndName)
                    return STS_INVALID_PARAMETER;

                byte = ChkToken();
                //if(byte==TOK_EndList) //no more parameters
                    goto END_LIST;
                //else
                //    return STS_INVALID_PARAMETER;
            }
            else
                return STS_INVALID_PARAMETER;
        }
        else
        { //retrieve startRow or endRow only
            if (AtomDecoding_Uint8(&namedValue)!=STS_SUCCESS)
                return STS_INVALID_PARAMETER;

            if(namedValue==0x01) //startRow
            {
                if (AtomDecoding_Uint32(&startRow)!=STS_SUCCESS)
                    return STS_INVALID_PARAMETER;

                if(ChkToken()!=TOK_EndName)
                    return STS_INVALID_PARAMETER;

                byte = ChkToken();
                if(byte==TOK_EndList) //no more parameters
                    goto END_LIST;
                else if(byte!=TOK_StartName)
                    return STS_SESSION_ABORT;   //A6-1-5-2-1, STS_INVALID_PARAMETER;

                if (AtomDecoding_Uint8(&namedValue)!=STS_SUCCESS)
                    return STS_INVALID_PARAMETER;
            }

            if(namedValue==0x02) //endRow
            {
                if (AtomDecoding_Uint32(&endRow)!=STS_SUCCESS)
                    return STS_INVALID_PARAMETER;

                //TODO: check max row value ...

                if(ChkToken()!=TOK_EndName)
                    return STS_INVALID_PARAMETER;

                byte = ChkToken();
                //if(byte==TOK_EndList) //no more parameters
                    goto END_LIST;
                //else
                //    return STS_INVALID_PARAMETER;
            }
            else
                return STS_INVALID_PARAMETER;

        //if(endRow>rowSize) return zNG;
        }
    }

END_LIST:
    if((byte!=TOK_EndList)||
       (ChkToken()!=TOK_EndList)||
       (ChkToken()!=TOK_EndOfData))
        return STS_SESSION_ABORT;

    if(mSessionManager.SPID.all==UID_SP_Admin)
    {
        switch((invokingUID.all)>>32)
        {
        case 0x0001:
            ptColPty = (U8 *)G1.b.mAdmTbl_Tbl.pty;
            ptTblObj = (U8 *)G1.b.mAdmTbl_Tbl.val;
            rowCnt = G1.b.mAdmTbl_Tbl.rowCnt;
            colCnt = G1.b.mAdmTbl_Tbl.colCnt;
            objSize = G1.b.mAdmTbl_Tbl.objSize;
            maxCol = G1.b.mAdmTbl_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G1.b.mAdmTbl_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0002:
            ptColPty = (U8 *)G1.b.mAdmSPInfo_Tbl.pty;
            ptTblObj = (U8 *)G1.b.mAdmSPInfo_Tbl.val;
            rowCnt = G1.b.mAdmSPInfo_Tbl.rowCnt;
            colCnt = G1.b.mAdmSPInfo_Tbl.colCnt;
            objSize = G1.b.mAdmSPInfo_Tbl.objSize;
            maxCol = G1.b.mAdmSPInfo_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G1.b.mAdmSPInfo_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0003:
            ptColPty = (U8 *)G1.b.mAdmSPTemplates_Tbl.pty;
            ptTblObj = (U8 *)G1.b.mAdmSPTemplates_Tbl.val;
            rowCnt = G1.b.mAdmSPTemplates_Tbl.rowCnt;
            colCnt = G1.b.mAdmSPTemplates_Tbl.colCnt;
            objSize = G1.b.mAdmSPTemplates_Tbl.objSize;
            maxCol = G1.b.mAdmSPTemplates_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G1.b.mAdmSPTemplates_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0006:
            ptColPty = (U8 *)G1.b.mAdmMethod_Tbl.pty;
            ptTblObj = (U8 *)G1.b.mAdmMethod_Tbl.val;
            rowCnt = G1.b.mAdmMethod_Tbl.rowCnt;
            colCnt = G1.b.mAdmMethod_Tbl.colCnt;
            objSize = G1.b.mAdmMethod_Tbl.objSize;
            maxCol = G1.b.mAdmMethod_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G1.b.mAdmMethod_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0008:
            ptColPty = (U8 *)G1.b.mAdmACE_Tbl.pty;
            ptTblObj = (U8 *)G1.b.mAdmACE_Tbl.val;
            rowCnt = G1.b.mAdmACE_Tbl.rowCnt;
            colCnt = G1.b.mAdmACE_Tbl.colCnt;
            objSize = G1.b.mAdmACE_Tbl.objSize;
            maxCol = G1.b.mAdmACE_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G1.b.mAdmACE_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0009:
            ptColPty = (U8 *)G1.b.mAdmAuthority_Tbl.pty;
            ptTblObj = (U8 *)G1.b.mAdmAuthority_Tbl.val;
            rowCnt = G1.b.mAdmAuthority_Tbl.rowCnt;
            colCnt = G1.b.mAdmAuthority_Tbl.colCnt;
            objSize = G1.b.mAdmAuthority_Tbl.objSize;
            maxCol = G1.b.mAdmAuthority_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G1.b.mAdmAuthority_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x000B:
            ptColPty = (U8 *)G1.b.mAdmCPin_Tbl.pty;
            ptTblObj = (U8 *)G1.b.mAdmCPin_Tbl.val;
            rowCnt = G1.b.mAdmCPin_Tbl.rowCnt;
            colCnt = G1.b.mAdmCPin_Tbl.colCnt;
            objSize = G1.b.mAdmCPin_Tbl.objSize;
            maxCol = G1.b.mAdmCPin_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G1.b.mAdmCPin_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0201:
            ptColPty = (U8 *)G1.b.mAdmTPerInfo_Tbl.pty;
            ptTblObj = (U8 *)G1.b.mAdmTPerInfo_Tbl.val;
            rowCnt = G1.b.mAdmTPerInfo_Tbl.rowCnt;
            colCnt = G1.b.mAdmTPerInfo_Tbl.colCnt;
            objSize = G1.b.mAdmTPerInfo_Tbl.objSize;
            maxCol = G1.b.mAdmTPerInfo_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G1.b.mAdmTPerInfo_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0204:
            ptColPty = (U8 *)G1.b.mAdmTemplate_Tbl.pty;
            ptTblObj = (U8 *)G1.b.mAdmTemplate_Tbl.val;
            rowCnt = G1.b.mAdmTemplate_Tbl.rowCnt;
            colCnt = G1.b.mAdmTemplate_Tbl.colCnt;
            objSize = G1.b.mAdmTemplate_Tbl.objSize;
            maxCol = G1.b.mAdmTemplate_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G1.b.mAdmTemplate_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0205:
            ptColPty = (U8 *)G1.b.mAdmSP_Tbl.pty;
            ptTblObj = (U8 *)G1.b.mAdmSP_Tbl.val;
            rowCnt = G1.b.mAdmSP_Tbl.rowCnt;
            colCnt = G1.b.mAdmSP_Tbl.colCnt;
            objSize = G1.b.mAdmSP_Tbl.objSize;
            maxCol = G1.b.mAdmSP_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G1.b.mAdmSP_Tbl.pty[colCnt-1].colNo;
            break;

        default:
            return STS_INVALID_PARAMETER;
        }
    }
    else //if(mSessionManager.SPID.all==UID_SP_Locking)
    {
        switch((invokingUID.all)>>32)
        {
        case 0x0001:
            ptColPty = (U8 *)G2.b.mLckTbl_Tbl.pty;
            ptTblObj = (U8 *)G2.b.mLckTbl_Tbl.val;
            rowCnt = G2.b.mLckTbl_Tbl.rowCnt;
            colCnt = G2.b.mLckTbl_Tbl.colCnt;
            objSize = G2.b.mLckTbl_Tbl.objSize;
            maxCol = G2.b.mLckTbl_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G2.b.mLckTbl_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0002:
            ptColPty = (U8 *)G2.b.mLckSPInfo_Tbl.pty;
            ptTblObj = (U8 *)G2.b.mLckSPInfo_Tbl.val;
            rowCnt = G2.b.mLckSPInfo_Tbl.rowCnt;
            colCnt = G2.b.mLckSPInfo_Tbl.colCnt;
            objSize = G2.b.mLckSPInfo_Tbl.objSize;
            maxCol = G2.b.mLckSPInfo_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G2.b.mLckSPInfo_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0003:
            ptColPty = (U8 *)G2.b.mLckSPTemplates_Tbl.pty;
            ptTblObj = (U8 *)G2.b.mLckSPTemplates_Tbl.val;
            rowCnt = G2.b.mLckSPTemplates_Tbl.rowCnt;
            colCnt = G2.b.mLckSPTemplates_Tbl.colCnt;
            objSize = G2.b.mLckSPTemplates_Tbl.objSize;
            maxCol = G2.b.mLckSPTemplates_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G2.b.mLckSPTemplates_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0006:
            ptColPty = (U8 *)G2.b.mLckMethod_Tbl.pty;
            ptTblObj = (U8 *)G2.b.mLckMethod_Tbl.val;
            rowCnt = G2.b.mLckMethod_Tbl.rowCnt;
            colCnt = G2.b.mLckMethod_Tbl.colCnt;
            objSize = G2.b.mLckMethod_Tbl.objSize;
            maxCol = G2.b.mLckMethod_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G2.b.mLckMethod_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0008:
            ptColPty = (U8 *)G3.b.mLckACE_Tbl.pty;
            ptTblObj = (U8 *)G3.b.mLckACE_Tbl.val;
            rowCnt = G3.b.mLckACE_Tbl.rowCnt;
            colCnt = G3.b.mLckACE_Tbl.colCnt;
            objSize = G3.b.mLckACE_Tbl.objSize;
            maxCol = G3.b.mLckACE_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G3.b.mLckACE_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0009:
            ptColPty = (U8 *)G3.b.mLckAuthority_Tbl.pty;
            ptTblObj = (U8 *)G3.b.mLckAuthority_Tbl.val;
            rowCnt = G3.b.mLckAuthority_Tbl.rowCnt;
            colCnt = G3.b.mLckAuthority_Tbl.colCnt;
            objSize = G3.b.mLckAuthority_Tbl.objSize;
            maxCol = G3.b.mLckAuthority_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G3.b.mLckAuthority_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x000B:
            ptColPty = (U8 *)G3.b.mLckCPin_Tbl.pty;
            ptTblObj = (U8 *)G3.b.mLckCPin_Tbl.val;
            rowCnt = G3.b.mLckCPin_Tbl.rowCnt;
            colCnt = G3.b.mLckCPin_Tbl.colCnt;
            objSize = G3.b.mLckCPin_Tbl.objSize;
            maxCol = G3.b.mLckCPin_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G3.b.mLckCPin_Tbl.pty[colCnt-1].colNo;
            break;
        case 0x001D:
            ptColPty = (U8 *)G2.b.mLckSecretProtect_Tbl.pty;
            ptTblObj = (U8 *)G2.b.mLckSecretProtect_Tbl.val;
            rowCnt = G2.b.mLckSecretProtect_Tbl.rowCnt;
            colCnt = G2.b.mLckSecretProtect_Tbl.colCnt;
            objSize = G2.b.mLckSecretProtect_Tbl.objSize;
            maxCol = G2.b.mLckSecretProtect_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G2.b.mLckSecretProtect_Tbl.pty[colCnt-1].colNo;
            break;
        case 0x0801:
            ptColPty = (U8 *)G2.b.mLckLockingInfo_Tbl.pty;
            ptTblObj = (U8 *)G2.b.mLckLockingInfo_Tbl.val;
            rowCnt = G2.b.mLckLockingInfo_Tbl.rowCnt;
            colCnt = G2.b.mLckLockingInfo_Tbl.colCnt;
            objSize = G2.b.mLckLockingInfo_Tbl.objSize;
            maxCol = G2.b.mLckLockingInfo_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G2.b.mLckLockingInfo_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0802:
            ptColPty = (U8 *)G3.b.mLckLocking_Tbl.pty;
            ptTblObj = (U8 *)G3.b.mLckLocking_Tbl.val;
            rowCnt = G3.b.mLckLocking_Tbl.rowCnt;
            colCnt = G3.b.mLckLocking_Tbl.colCnt;
            objSize = G3.b.mLckLocking_Tbl.objSize;
            maxCol = G3.b.mLckLocking_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G3.b.mLckLocking_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0803:
            ptColPty = (U8 *)G3.b.mLckMbrCtrl_Tbl.pty;
            ptTblObj = (U8 *)G3.b.mLckMbrCtrl_Tbl.val;
            rowCnt = G3.b.mLckMbrCtrl_Tbl.rowCnt;
            colCnt = G3.b.mLckMbrCtrl_Tbl.colCnt;
            objSize = G3.b.mLckMbrCtrl_Tbl.objSize;
            maxCol = G3.b.mLckMbrCtrl_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G3.b.mLckMbrCtrl_Tbl.pty[colCnt-1].colNo;
            break;

        case 0x0804:    //MBR table
            maxRow = MBR_LEN-1;
            break;
#ifdef TCG_KAES_128
        case 0x0805:
            ptColPty = (U8 *)mLckKAES_128_Tbl.pty;
            ptTblObj = (U8 *)mLckKAES_128_Tbl.val;
            rowCnt = mLckKAES_128_Tbl.rowCnt;
            colCnt = mLckKAES_128_Tbl.colCnt;
            objSize = mLckKAES_128_Tbl.objSize;
            maxCol = mLckKAES_128_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //mLckKAES_128_Tbl.pty[colCnt-1].colNo;
            break;
#else
        case 0x0806:
            ptColPty = (U8 *)G3.b.mLckKAES_256_Tbl.pty;
            ptTblObj = (U8 *)G3.b.mLckKAES_256_Tbl.val;
            rowCnt = G3.b.mLckKAES_256_Tbl.rowCnt;
            colCnt = G3.b.mLckKAES_256_Tbl.colCnt;
            objSize = G3.b.mLckKAES_256_Tbl.objSize;
            maxCol = G3.b.mLckKAES_256_Tbl.maxCol;

            //if(endCol==0xff) //no start/end parameters
            //    endCol=maxCol;  //G3.b.mLckKAES_256_Tbl.pty[colCnt-1].colNo;
            break;
#endif

        case 0x1001:    //DataStore table
        case 0x1002:
        case 0x1003:
        case 0x1004:
        case 0x1005:
        case 0x1006:
        case 0x1007:
        case 0x1008:
        case 0x1009:
            startCol = ((invokingUID.all>>32)&0xff) - 1;
            maxRow = mDataStoreAddr[startCol].length - 1;
            break;

        default:
            return STS_INVALID_PARAMETER;
        }
    }

    if(tblKind==TBL_K_OBJECT)
    {
        if(invokingUID.all==UID_LockingInfo)
        {
            if(endCol<startCol)
                return STS_INVALID_PARAMETER;

            if(startCol>0x060001)
                return STS_INVALID_PARAMETER;
            if(startCol==0x60001)
                startCol= 0x61;
            else if(startCol==0x60000)
                startCol=0x60;
            else if(startCol>0x0A)
                startCol=0x0A;

            if(endCol==0xffffffff)
                endCol=maxCol;
            else if(endCol>0x60001)
                return STS_INVALID_PARAMETER;
            else if(endCol==0x60001)
                endCol=0x61;
            else if(endCol==0x60000)
                endCol=0x60;
            else if(endCol>0x0A)
                endCol=0x0A;
        }
        else
        {
        if(endCol==0xffffffff) //no end parameters
            endCol=maxCol;

        if(endCol<startCol)
            return STS_INVALID_PARAMETER;

        if(endCol>maxCol)
            return STS_INVALID_PARAMETER;
        }
    }
    else
    {
        if(endRow==0xffffffff)
            endRow = maxRow;

        if(endRow<startRow)
            return STS_INVALID_PARAMETER;

        if(endRow>maxRow)
            return STS_INVALID_PARAMETER;

        //check if row size is over the buffer length,  F0 82 xx yy F1 F9 F0 00 00 00 F1, , iDataBuf=0x38
        if((endRow-startRow+1)>(TCG_BUF_LEN-iDataBuf-11))
             return STS_RESPONSE_OVERFLOW;
    }

    //status list check
    result = ChkMethodStatus();
    if(result!=STS_SUCCESS)
        return result;

    //method execution

    //prepare payload for response
    dataBuf[iDataBuf++]=TOK_StartList;

    if(tblKind==TBL_K_OBJECT)
    {

    U32 iRow, iCol, iGet, iList;

soutb3(0x5C, 0xFE, 0x05);  soutb2(startCol, endCol);
    TCG_PRINTF("Get UID=%X sCol=%2X eCol=%2X\n", (U32)(invokingUID.all>>32), startCol , endCol);
    dataBuf[iDataBuf++]=TOK_StartList;

    for(iRow=0;iRow<rowCnt;iRow++)
    {
        tmp64=*((U64 *)ptTblObj);
        //for(i32=0;i32<8;i32++)  *(((U8 *)&tmp64)+i32)=*(ptTblObj+i32);

        if(tmp64==invokingUID.all)
        //if(*(U64 *)ptTblObj==invokingUID.all)
        { //the table object is found!
            iCol=0;
//souts("iRow=");  soutb(iRow);

            for(iGet=startCol;iGet<=endCol;iGet++)
            {
//souts("  iGet=");  soutb(iGet);

                //if ptColPty[iCol].colNo is smaller than iGet, jump to next ptColPty[iCol].colNo
                while ((iCol<(colCnt-1))&&(*ptColPty<iGet)) //*ptColPty=Col#, *(ptColPty+1)=Col Size, *(ptColPty+2)=Col Type
                {
                    iCol++;
                    ptTblObj += *(ptColPty+1);      //pointer to next table cell
                    ptColPty += sizeof(sColPrty);   //pointer to next colPty cell
                }

                if (*ptColPty==iGet)
                { //need to modify later for atom processing ...

                    if(AceColumnsChk(iGet)==zOK) //check ACE columns first
                    {
                        dataBuf[iDataBuf++]=TOK_StartName;
                        if(iGet>=0x60)
                        {
                            dataBuf[iDataBuf++]=0x83;
                            dataBuf[iDataBuf++]=0x06;
                            dataBuf[iDataBuf++]=0x00;
                            dataBuf[iDataBuf++]= iGet&0x0f;
                        } else
                        dataBuf[iDataBuf++]=iGet;   //name

                        //value...
                        dataType = *(ptColPty+2);
                        if(dataType==UID_TYPE)
                        {
                            byteSize= *(ptColPty+1);    //fixed byte sequence, check ColPty[1]
                            AtomEncoding_Byte(byteSize);
                            for(i32=1;i32<=byteSize;i32++)
                                dataBuf[iDataBuf++]= *(ptTblObj+byteSize-i32);
                        }
                        else if(dataType==FBYTE_TYPE)
                        {
                            byteSize= *(ptColPty+1);    //fixed byte sequence, check ColPty[1]
                            AtomEncoding_Byte(byteSize);
                            for(i32=0;i32<byteSize;i32++)
                                dataBuf[iDataBuf++]= *(ptTblObj+i32);
                        }
                        else if(dataType==VBYTE_TYPE)
                        {
                            byteSize= *ptTblObj;     //variable byte sequence, check table cell[0]
                            AtomEncoding_Byte(byteSize);
                            for(i32=1;i32<=byteSize;i32++)
                                dataBuf[iDataBuf++]= *(ptTblObj+i32);
                        }
                        else if(dataType==VALUE_TYPE)
                        {
                            AtomEncoding_Integer(ptTblObj,*(ptColPty+1));
                            //dataBuf[iDataBuf++]= *ptTblObj;
                        }
                        else if(dataType==LIST_TYPE)
                        {   //TODO: how to deal with "BooleanExpr?
                            byteSize= *ptTblObj;     //List sequence, check table cell[0]
                            dataBuf[iDataBuf++]= TOK_StartList;
                            for(i32=1;i32<=byteSize;i32++)
                                dataBuf[iDataBuf++]= *(ptTblObj+i32);
                            dataBuf[iDataBuf++]= TOK_EndList;
                        }
                        else if(dataType==STRING_TYPE)
                        {
                            byteSize= *(ptColPty+1);    //ColPty[1] for ColSize

                            for(i32=0;i32<byteSize;i32++)
                            {
                                if(*(ptTblObj+i32)==0x00)
                                {//string end '\0'
                                    byteSize = i32;
                                    break;
                                }
                            }

                            AtomEncoding_Byte(byteSize);
                            for(i32=0;i32<byteSize;i32++)
                                dataBuf[iDataBuf++]= *(ptTblObj+i32);
                        }
                        else if(dataType==STRINGLIST_TYPE)
                        {
                            byteSize= *(ptColPty+1);    //ColPty[1] for ColSize

                            //TODO: need to deal with more than one list ...
                            dataBuf[iDataBuf++]= TOK_StartList;
                            for(i32=0;i32<byteSize;i32++)
                            {
                                if(*(ptTblObj+i32)==0x00)
                                {//string end '\0'
                                    byteSize = i32;
                                    break;
                                }
                            }

                            AtomEncoding_Byte(byteSize);
                            for(i32=0;i32<byteSize;i32++)
                                dataBuf[iDataBuf++]= *(ptTblObj+i32);

                            dataBuf[iDataBuf++]= TOK_EndList;
                        }
                        else if(dataType==UIDLIST_TYPE)
                        { //mainly for "BooleanExpr"
                            dataBuf[iDataBuf++]= TOK_StartList;

                            //get 1st UID
                            dataBuf[iDataBuf++]= TOK_StartName;
                                //place half-UID first (Authority_object_ref)
                                dataBuf[iDataBuf++] = 0xA4;
                                dataBuf[iDataBuf++] = (U8)(UID_CT_Authority_object_ref>>24);  //0x00
                                dataBuf[iDataBuf++] = (U8)(UID_CT_Authority_object_ref>>16);  //0x00
                                dataBuf[iDataBuf++] = (U8)(UID_CT_Authority_object_ref>>8);   //0x0C
                                dataBuf[iDataBuf++] = (U8)(UID_CT_Authority_object_ref);      //0x05

                                byteSize=8;
                                dataBuf[iDataBuf++]= 0xA0 | byteSize;
                                for(i32=1;i32<=byteSize;i32++)
                                    dataBuf[iDataBuf++]= *(ptTblObj+byteSize-i32);        //UID

                            dataBuf[iDataBuf++]= TOK_EndName;

                            //get next UID
                            if(mSessionManager.SPID.all==UID_SP_Admin)
                                boolExprCnt=ADM_ACE_BOOLEXPR_CNT;
                            else
                                boolExprCnt=LCK_ACE_BOOLEXPR_CNT;

                            for(iList=1;iList<boolExprCnt;iList++)
                            {
                                //check if next UID exists or not
                                tmp64=*((U64 *)(ptTblObj + byteSize*iList));
                                //sout64(tmp64); souts(" ");
                                if(tmp64)
                                {
                                    dataBuf[iDataBuf++]= TOK_StartName;

                                        dataBuf[iDataBuf++] = 0xA4;
                                        dataBuf[iDataBuf++] = (U8)(UID_CT_Authority_object_ref>>24);  //0x00
                                        dataBuf[iDataBuf++] = (U8)(UID_CT_Authority_object_ref>>16);  //0x00
                                        dataBuf[iDataBuf++] = (U8)(UID_CT_Authority_object_ref>>8);   //0x0C
                                        dataBuf[iDataBuf++] = (U8)(UID_CT_Authority_object_ref);      //0x05

                                        dataBuf[iDataBuf++]= 0xA0 | byteSize;
                                        for(i32=1;i32<=byteSize;i32++) //UID
                                            dataBuf[iDataBuf++]= *(ptTblObj + (byteSize*(iList+1)) - i32);

                                    dataBuf[iDataBuf++]= TOK_EndName;

                                    dataBuf[iDataBuf++]= TOK_StartName;

                                        dataBuf[iDataBuf++] = 0xA4;
                                        dataBuf[iDataBuf++] = (U8)(UID_CT_boolean_ACE>>24);  //0x00
                                        dataBuf[iDataBuf++] = (U8)(UID_CT_boolean_ACE>>16);  //0x00
                                        dataBuf[iDataBuf++] = (U8)(UID_CT_boolean_ACE>>8);   //0x04
                                        dataBuf[iDataBuf++] = (U8)(UID_CT_boolean_ACE);      //0x0E
                                        dataBuf[iDataBuf++] = 0x01; //"OR"

                                    dataBuf[iDataBuf++]= TOK_EndName;
                                }else
                                    break;
                            }
                            dataBuf[iDataBuf++]= TOK_EndList;

                        }
                        else if(dataType==UID2_TYPE)
                        { // UID list, for LockingInfo_SingleUserRange
                            alexdebugds(G2.b.mLckLockingInfo_Tbl.val[0].singleUserModeRange[0],SUser);

                            byteSize=8;
                            tmp64=*((U64 *)ptTblObj);
                            if(tmp64==UID_Locking)
                            { //EntireLocking
                                AtomEncoding_Byte(byteSize);

                                for(i32=0;i32<byteSize;i32++) //UID
                                    dataBuf[iDataBuf++]= *((U8 *)&tmp64 + 7 - i32);
                            }
                            else
                            { //UID list or Empty list, get next UID

                                dataBuf[iDataBuf++]= TOK_StartList;

                            for(iList=0;iList<=LOCKING_RANGE_CNT;iList++)
                            {
                                //check if next UID exists or not
                                tmp64=*((U64 *)(ptTblObj + byteSize*iList));
                                //sout64(tmp64); souts(" ");
                                if(tmp64)
                                {
                                    AtomEncoding_Byte(byteSize);
                                        for(i32=1;i32<=byteSize;i32++) //UID
                                            dataBuf[iDataBuf++]= *(ptTblObj + (byteSize*(iList+1)) - i32);
                                }else
                                    break;
                            }

                            dataBuf[iDataBuf++]= TOK_EndList;
                        }

                        }
                        dataBuf[iDataBuf++]=TOK_EndName;
                    }
                }
                else if(*ptColPty<iGet)
                {//table column size is smaller than iGet
                    break;
                }
                else if (*ptColPty>endCol)
                    break;

            }
            break;
        }
        else
            ptTblObj += objSize;
    }

    dataBuf[iDataBuf++]=TOK_EndList;

    }
    else //if(tblKind==Byte)
    {
soutb3(0x5C, 0xFE, 0x1C);  soutd(startRow); soutd(endRow);
        TCG_PRINTF("Get UID=%X sRow=%X eRow=%X\n", (U32)(invokingUID.all>>32), startRow, endRow);
        if(invokingUID.all!=UID_MBR)
        {
            alexdebug2ds(startCol,mDataStoreAddr[startCol].offset,idx__offset);

            //encode Atom Token...
            AtomEncoding_Byte(endRow-startRow+1);    //dataBuf[iDataBuf]

            {
                int rlen;
                U32 LaaAddr,LaaOffAddr,rdptr;
                U32 LaaCnt;
                U16 ii;
                U16 HasBlank;
                U8  *Desptr=(U8 *)TcgTmpBuf;


                rlen=endRow-startRow+1;              //rlen = wr remain length
                if(rlen > TCG_BUF_LEN){
                    return STS_INVALID_PARAMETER;
                }
                rdptr=startRow+mDataStoreAddr[startCol].offset;         //rd point
                LaaAddr = rdptr / LAA_LEN;
                LaaOffAddr = rdptr % LAA_LEN;
                alexdebug2ds(LaaAddr,LaaOffAddr,LaaAddr__LaaOffAddr);

                LaaCnt = (LaaOffAddr+rlen)/LAA_LEN;
                if((LaaOffAddr+rlen)%LAA_LEN) LaaCnt+=1;
                alexdebug2ds(rlen,LaaCnt,rlen__LaaCnt);
                MEM_SET((U8 *)TcgTmpBuf, 0x00, LaaCnt*LAA_LEN);  //clr buffer

                HasBlank = FALSE;
                for(ii=LaaAddr; ii<LaaAddr+LaaCnt; ii++){
                    if(G4.b.TcgMbrL2P[TCG_DS_LAA_START + ii].pc.cell >= TCG_MBR_CELLS){
                        HasBlank = TRUE;
                        break;
                    }
                }
                if(HasBlank){
                    for(ii=LaaAddr; ii<LaaAddr+LaaCnt; ii++){
                        if(G4.b.TcgMbrL2P[TCG_DS_LAA_START + ii].pc.cell < TCG_MBR_CELLS){  //blank ?
                            DS_Read(ii, ii+1, (U8 *)TcgTmpBuf + (ii-LaaAddr)*CFG_UDATA_PER_PAGE);   //read 1 page
                        }
                    }
                }else{
                    DS_Read(LaaAddr, LaaAddr+LaaCnt, (U8 *)TcgTmpBuf);  // read all
                }

                for(i32=0;i32<rlen;i32++){
                    dataBuf[iDataBuf++]=Desptr[LaaOffAddr+i32];
                }
            }
        }
        else
        {
            U32 LaaAddr,LaaOffAddr,rdptr;
            int      rlen;

            rlen=endRow-startRow+1;              //rlen = wr remain length
            if(rlen > TCG_BUF_LEN){
                return STS_INVALID_PARAMETER;
            }

            if(rlen<=15) //Short Atom, length=0~15
                dataBuf[iDataBuf++] = 0xA0 + (U8)rlen;
            else if(rlen<=2047)
            { //Medium Atom
                dataBuf[iDataBuf++] = 0xD0 + HiByte(LoWord(rlen));
                dataBuf[iDataBuf++] = LoByte(LoWord(rlen));
            }
            else
            { //Long Atom
                dataBuf[iDataBuf++] = 0xE2;
                dataBuf[iDataBuf++] = LoByte(HiWord(rlen));
                dataBuf[iDataBuf++] = HiByte(LoWord(rlen));
                dataBuf[iDataBuf++] = LoByte(LoWord(rlen));
            }

            rdptr=startRow;         //rd point
            if(rlen>0){
                U32 LaaCnt;
                U8 *Desptr=(U8 *)TcgTmpBuf;

                LaaAddr = rdptr / LAA_LEN;
                LaaOffAddr = rdptr % LAA_LEN;
                alexdebug2ds(LaaAddr,LaaOffAddr,LaaAddr__LaaOffAddr);
                RdTcgMBRbyNormalRdCmd=FALSE;

                LaaCnt = (LaaOffAddr+rlen)/LAA_LEN;
                if((LaaOffAddr+rlen)%LAA_LEN) LaaCnt+=1;
                alexdebug2ds(rlen,LaaCnt,rlen__LaaCnt);
                MEM_SET((U8 *)TcgTmpBuf, 0x00, LaaCnt*LAA_LEN);  //clr buffer
            #if 1
                {
                    U16 ii;
                    U16 HasBlank;

                    HasBlank = FALSE;

                    for(ii=LaaAddr; ii<LaaAddr+LaaCnt; ii++){
                        if(G4.b.TcgMbrL2P[TCG_SMBR_LAA_START+ii].pc.cell >= TCG_MBR_CELLS){
                            HasBlank = TRUE;
                            break;
                        }
                    }
                    if(HasBlank){
                        for(ii=LaaAddr; ii<LaaAddr+LaaCnt; ii++){
                            if(G4.b.TcgMbrL2P[TCG_SMBR_LAA_START + ii].pc.cell < TCG_MBR_CELLS){  //blank ?
                                SMBR_Read(ii, ii+1, (U8 *)TcgTmpBuf + (ii-LaaAddr)*CFG_UDATA_PER_PAGE, NULL);   //read 1 page
                            }
                        }
                    }else{
                        SMBR_Read(LaaAddr, LaaAddr+LaaCnt, (U8 *)TcgTmpBuf, NULL);   //read all
                    }
                }
            #else
                SMBR_Read(LaaAddr, LaaAddr+LaaCnt, (U8 *)TcgTmpBuf, NULL);    //WaitMbrRd(LaaAddr, LaaAddr+LaaCnt, (U8 *)TcgTmpBuf);  //read 6 page for max transfer buffer
            #endif
                for(i32=0;i32<rlen;i32++){
                    dataBuf[iDataBuf++]=Desptr[LaaOffAddr+i32];
                }
            }
        }
    }

    dataBuf[iDataBuf++]=TOK_EndList;
    dataBuf[iDataBuf++]=TOK_EndOfData;

    return STS_SUCCESS;

}

//check if column index "iCol" is in the ACE "Columns" column or not
int AceColumnsChk(U8 iCol)
{
    U8 i,j,k;
    U8 AclCnt;

//    if((iCol==0x01) && ((invokingUID.all>>32)!=(UID_CPIN>>32)))//Col#1: Name or MethodID ...
//        return zOK;

    if(mSessionManager.SPID.all==UID_SP_Admin)
    {
        if(G1.b.mAdmACE_Tbl.val[iAceRow].col[0]==0x00)   //All
            return zOK;

        for(i=1;i<=G1.b.mAdmACE_Tbl.val[iAceRow].col[0];i++)
        {
            if(G1.b.mAdmACE_Tbl.val[iAceRow].col[i]==iCol)
                return zOK;
        }
    }
    else //if(mSessionManager.SPID.all==UID_SP_Locking)
    {
        AclCnt=0;
        while((aclUid[AclCnt]!=UID_Null) && (AclCnt<LCK_ACCESSCTRL_ACL_CNT))
        {
            for(i=0;i<G3.b.mLckACE_Tbl.rowCnt;i++)
            {
                if(G3.b.mLckACE_Tbl.val[i].uid==aclUid[AclCnt])
                {
                    for(j=0;j<LCK_ACE_BOOLEXPR_CNT;j++)
                    {
                        if((G3.b.mLckACE_Tbl.val[i].booleanExpr[j]==mSessionManager.HtSgnAuthority.all)||
                           (G3.b.mLckACE_Tbl.val[i].booleanExpr[j]==mSessionManager.HtAuthorityClass)||
                           (G3.b.mLckACE_Tbl.val[i].booleanExpr[j]==UID_Authority_Anybody))
                        {
                            if(G3.b.mLckACE_Tbl.val[i].col[0]==0x00){   //All
                                return zOK;
                            }
                            for(k=1;k<=G3.b.mLckACE_Tbl.val[i].col[0];k++){
                                if(G3.b.mLckACE_Tbl.val[i].col[k]==iCol){
                                    return zOK;
                                }
                            }
                        }
                        else if(G3.b.mLckACE_Tbl.val[i].booleanExpr[j]==0) //no more BooleanExpr list
                        {
                            break;          //for j-loop
                        }
                    }

                    break;  //for i-loop
                }
            }
            AclCnt++;
        }
        return zNG;
    }
    return zNG;
}

//
// Encode the Byte Sequence into Atom (Core 3.2.2.3.1)
// This function will only encode the Atom Header and write it directly to "dataBuf[iDataBuf++]".
//
// Return the Byte Count written to dataBuf[].
int AtomEncoding_Byte(U32 length)
{
    //Byte Sequence,  write Header only
    if(length<=15)
    { //Short Atom, length=0~15
        dataBuf[iDataBuf++] = 0xA0 + (U8)length;
        return 1;
    }
    else if(length<=2047)
    { //Medium Atom
        dataBuf[iDataBuf++] = 0xD0 + HiByte(LoWord(length));
        dataBuf[iDataBuf++] = LoByte(LoWord(length));
        return 2;
    }
    else
    { //Long Atom
        dataBuf[iDataBuf++] = 0xE2;
        dataBuf[iDataBuf++] = LoByte(HiWord(length));
        dataBuf[iDataBuf++] = HiByte(LoWord(length));
        dataBuf[iDataBuf++] = LoByte(LoWord(length));
        return 4;
    }

}

//
// Encode the Integer Value into Atom
//
// This function will encode the Atom Header + Data, and write them directly to "dataBuf[iDataBuf++]".
// It only deals with the unsigned value.
//
// Return the Byte Count written to dataBuf[].
int AtomEncoding_Integer(U8 *data, U8 size)
{
    //Integer value,  write Header + data
    U8 i;

    for(i=size;i>0;i--)
    {
        if(*(data+i-1)!=0)
            break;
        }

    size = i;

    if(size<=1)
    {
        if(*data<=63) //Tiny Atom 0~63
            dataBuf[iDataBuf++] = *data;
            else
        {
            dataBuf[iDataBuf++] = 0x81;
            dataBuf[iDataBuf++] = *data;
        }
    }
    else if(size<=15)
    { //size>=2, <=15
        //Short Atom, length=0~15
        dataBuf[iDataBuf++] = 0x80 + size;

        for(i=size;i>0;i--)
            dataBuf[iDataBuf++] = *(data+i-1);
    }
    else
        return zNG;

    return zOK;
}

/***********************************************************
* Admin table set method.
* ref. core spec 5.3.3.7
***********************************************************/
U16 Method_Set(void)
{
    U32 DecLen;
    U8 Where_or_Value;  //0->Where, 1->Value, 0xff->illegal
    U8 AceColResult,err_code=0x00;  //no error
    U32 SetParm_ColNo[eMaxSetParamCnt];
    U8 SetParm_SameColNo_Cnt[eMaxSetParamCnt];
    U8 SetParm_Cnt=0;
    U32 halfUID;
    U8 MultiFunc_TarCol;
    U8 MultiFunc_ColCnt=0;
    U8 tmp[sizeof(U32)];
    U8 i,y;

    int zSTS=STS_SUCCESS;
    tATOM_DEC_TYPE atom_type;

#ifdef _TCG_LAB_RESCUE_ONLY
    if(zSTS==STS_SUCCESS){
        soutb3(0x5C, 0xFE, 0x45);  //!!Watch out, Lock setting in _TCG_LAB_RESCUE_ONLY
        zSTS=STS_SESSION_ABORT;
        return zSTS;
    }
#endif

    soutb3(0x5C, 0xFE, 0x16);
    MEM_SET(SetParm_ColNo, 0x00, sizeof(SetParm_ColNo));  //clear buffer
    MEM_SET(SetParm_SameColNo_Cnt, 0x00, sizeof(SetParm_SameColNo_Cnt));  //clear buffer
    MEM_SET(tmp, 0x00, sizeof(U32));  //clear buffer
    if(ChkToken()!=TOK_StartList){  //chk 0xF0
        zSTS=STS_SESSION_ABORT; err_code=0x01; goto exit_Method_Set;
    }
    if(ChkToken()!=TOK_StartName){  //chk 0xF2
        zSTS=STS_SESSION_ABORT; err_code=0x02; goto exit_Method_Set;
        }
    if((zSTS=AtomDecoding(ATOM_INT,&DecLen)) != STS_SUCCESS){
        err_code=0x03; goto exit_Method_Set;
        }
    Where_or_Value=*((U8 *)DecBuf);

    if((invokingUID.all&0xfffffff0ffffffff)==UID_DataStoreType || invokingUID.all==UID_MBR)
    { //byte table
        U32 tblLen;

        if(invokingUID.all==UID_MBR)
            tblLen = MBR_LEN;
        else{
            i = ((invokingUID.all >> 32) & 0xff) - 1;
            tblLen = mDataStoreAddr[i].length;
        }

        SetParm_ColNo[SetParm_Cnt]=0;  // the writing address (where) is store in *((U32 *)SetParm_ColNo), it is 0x00 if no "where" parameter
        if(Where_or_Value==0x00){ //Where

            //get target row
            if((zSTS=AtomDecoding(ATOM_INT,&DecLen)) != STS_SUCCESS){
                err_code=0x06; goto exit_Method_Set;
            }
            SetParm_ColNo[SetParm_Cnt]=*((U32 *)DecBuf);  // the writing address (where) is store in *((U32 *)SetParm_ColNo)
            if(SetParm_ColNo[SetParm_Cnt]>(tblLen-1)){
                zSTS=STS_INVALID_PARAMETER; err_code=0x07; goto exit_Method_Set;
            }

            if(ChkToken()!=TOK_EndName){  //check 0xF3
                zSTS=STS_SESSION_ABORT; err_code=0x08; goto exit_Method_Set;
            }

            if(ChkToken()!=TOK_StartName){  //check F2 for "value"
                zSTS=STS_SESSION_ABORT; err_code=0x0A; goto exit_Method_Set;
            }
            if((zSTS=AtomDecoding(ATOM_INT,&DecLen)) != STS_SUCCESS){
                err_code=0x0B; goto exit_Method_Set;
            }
            Where_or_Value=*((U8 *)DecBuf);
        }

        if(Where_or_Value!=0x01){  //Values ?
            zSTS=STS_INVALID_PARAMETER; err_code=0x0C; goto exit_Method_Set;
        }
        //get setting value which could be byte seqence  type.
        if((zSTS=AtomDecoding(ATOM_BYTES,&DecLen)) != STS_SUCCESS){
            err_code=0x10; goto exit_Method_Set;
        }

        if((SetParm_ColNo[SetParm_Cnt]+DecLen)>tblLen){
            zSTS=STS_INVALID_PARAMETER; err_code=0x12; goto exit_Method_Set;
        }

        //MEM_COPY(&SetParm_Buf[SetParm_ColNo[SetParm_Cnt]], tTkBuf, tTkDatLen);
        if(Write2Mtable(DecBuf,DecLen,SetParm_ColNo[SetParm_Cnt],0) == zNG){
            zSTS=STS_INVALID_PARAMETER; err_code=0x14; goto exit_Method_Set;
        }
        SetParm_Cnt++;
    }
    else
    { // object table, should omit "Where" for object table
        if(Where_or_Value!=0x01){  //Values ?
            zSTS=STS_INVALID_PARAMETER; err_code=0x16; goto exit_Method_Set;
        }

        if(ChkToken()!=TOK_StartList){
            zSTS=STS_SESSION_ABORT; err_code=0x18; goto exit_Method_Set;
        }
        while(ChkToken()==TOK_StartName)
        {
            if(iPload < mCmdPkt.mSubPktFmt.length)
            {  //check payload size
                //get target column
                if((zSTS=AtomDecoding(ATOM_INT,&DecLen)) != STS_SUCCESS){
                    err_code=0x1c; goto exit_Method_Set;
                }
                SetParm_ColNo[SetParm_Cnt]=*((U32 *)DecBuf);  //temp. save target column
                //test case A13-4-1-4-15
                if(SetParm_Cnt>0){
                    for(i=0; i<SetParm_Cnt; i++){
                        if(*((U32 *)DecBuf)==SetParm_ColNo[i]){  //set same column ?
                            zSTS=STS_INVALID_PARAMETER; err_code=0x20; goto exit_Method_Set;
                }
                    }
                }
                if(ChkToken() == TOK_StartList)
                {
                  if(((invokingUID.all==UID_MBRControl) && (SetParm_ColNo[SetParm_Cnt]==0x03)) ||
                      (((invokingUID.all&0xffffffff00000000)==UID_Locking) && (SetParm_ColNo[SetParm_Cnt]==0x09)))
                  { //"DoneOnReset" or "LockOnReset", {0} or {0,3} only.  ListType: F0, n1, n2, ... F1
souts("DOR/LOR");
                    TCG_PRINTF("DOR/LOR\n");
                    for(i=0; i<4; i++)
                        tmp[i]=0;

                    while(ChkToken()!=TOK_EndList)
                    {
                        iPload--;
                        if(AtomDecoding_Uint8(&y)== STS_SUCCESS)
                        {
                            tmp[0]++;
                            if(tmp[0]==1)
                            { // element 1
                                if(y!=0)
                                {   zSTS=STS_INVALID_PARAMETER; err_code=0x25; goto exit_Method_Set;  }
                            }
                            else if(tmp[0]==2)
                            { // element 2
                                 if(y!=3)
                                 {   zSTS=STS_INVALID_PARAMETER; err_code=0x26; goto exit_Method_Set;  }
                            }
                            else
                            {   zSTS=STS_INVALID_PARAMETER; err_code=0x27; goto exit_Method_Set;  }

                            tmp[tmp[0]]=y;
                        }
                        else
                        {   zSTS=STS_INVALID_PARAMETER; err_code=0x28; goto exit_Method_Set;  }
                    }

                    if(ChkToken()!=TOK_EndName)
                    {    zSTS=STS_SESSION_ABORT; err_code=0x29; goto exit_Method_Set;   }

                    SetParm_Cnt = 1;

                    if(invokingUID.all==UID_MBRControl)
                    {
                        for(i=0;i<=tmp[0];i++)
                            G3.b.mLckMbrCtrl_Tbl.val[0].doneOnReset[i]=tmp[i];
                        G3MtableChanged=TRUE;
                    }
                    else
                    {
                        for(y=0;y<G3.b.mLckLocking_Tbl.rowCnt;y++)
                        {
                            if(G3.b.mLckLocking_Tbl.val[y].uid==invokingUID.all)
                            {
                                for(i=0;i<=tmp[0];i++)
                                    G3.b.mLckLocking_Tbl.val[y].lockOnReset[i]=tmp[i];
                                G3MtableChanged=TRUE;
                                break;
                            }
                        }
                    }
                  }
                  else
                  {
                    // more than one setting value in one column  ( ex. ACE_Locking_Range1_Set_RdLocked_UID.Set[Values = [BooleanExpr = [User1_UID | User2_UID]]])
                    MultiFunc_TarCol=DecBuf[0];  //backup target column
                    MultiFunc_ColCnt=0;
                    while(ChkToken()==TOK_StartName)
                    {
                        if((zSTS=AtomDecoding(ATOM_HALFUID,&DecLen)) != STS_SUCCESS){
                            err_code=0x24; goto exit_Method_Set;
                        }
                        InvertByteSequence(4, DecBuf);
                        halfUID=*((U32 *)DecBuf);
                        switch(halfUID)
                        {
                            case 0x00000C05:    //Authority_object
                                SetParm_ColNo[SetParm_Cnt]=MultiFunc_TarCol;  // write cloumn #no is same as last column
                                if((zSTS=AtomDecoding(ATOM_UID,&DecLen)) != STS_SUCCESS){  //Authority_object
                                    zSTS=STS_SESSION_ABORT; err_code=0x2c; goto exit_Method_Set;
                                }
                                SetParm_SameColNo_Cnt[SetParm_Cnt]=MultiFunc_ColCnt;
                                if(ChkToken()!=TOK_EndName){
                                    zSTS=STS_SESSION_ABORT; err_code=0x30; goto exit_Method_Set;
                                }

                                AceColResult=AceColumnsChk(SetParm_ColNo[SetParm_Cnt]);
                                if(AceColResult == zOK){  // check target column whether allow to set in ACE table
                                    if(Write2Mtable(DecBuf,DecLen,SetParm_ColNo[SetParm_Cnt],SetParm_SameColNo_Cnt[SetParm_Cnt]) == zNG){
                                        zSTS=STS_INVALID_PARAMETER; err_code=0x34; goto exit_Method_Set;
                                    }
                                    MultiFunc_ColCnt++;
                                    SetParm_Cnt++;
                                }

                                break;
                            case 0x0000040E:    //boolean_ACE
                                if((zSTS=AtomDecoding(ATOM_INT,&DecLen)) != STS_SUCCESS){
                                    zSTS=STS_INVALID_PARAMETER; err_code=0x38; goto exit_Method_Set;
                                }
                                if(DecBuf[0]!=1){  // ACE_boolean != OR
                                    err_code=0x3c; goto exit_Method_Set;
                                }
                                if(ChkToken()!=TOK_EndName){
                                    zSTS=STS_SESSION_ABORT; err_code=0x40; goto exit_Method_Set;
                                }
                                break;
                            default:
                                zSTS=STS_INVALID_PARAMETER; err_code=0x44; goto exit_Method_Set;
                        }
                    }
                    iPload--;
                    if(ChkToken()!=TOK_EndList){
                        zSTS=STS_SESSION_ABORT; err_code=0x45; goto exit_Method_Set;
                    }
                    if(ChkToken()!=TOK_EndName){
                        zSTS=STS_SESSION_ABORT; err_code=0x46; goto exit_Method_Set;
                    }
                  }
                }
                else
                {   // one setting value in one column
                    //get setting value which could be any type. (ex. integer, string or boolean ...)
                    iPload-=1;
                    atom_type = ATOM_UNKNOW;
                  if( (((invokingUID.all&0xffffffff00000000)==UID_Authority)||((invokingUID.all&0xffffffff00000000)==UID_Locking))
                     &&(SetParm_ColNo[SetParm_Cnt]==0x02)) //"CommonName"
                  {
                       // souts("CommonName");
                       atom_type = ATOM_BYTES;
                  }
                  else
                    for(i=0; i<sizeof(cWrTypeAdjustment)/sizeof(sWrTypeAdjustment); i++){
                        if((invokingUID.all >>32) == cWrTypeAdjustment[i].invokingUID){
                            atom_type=cWrTypeAdjustment[i].type;
                        }
                    }

                    if(atom_type == ATOM_UNKNOW){
                        zSTS=STS_INVALID_PARAMETER; err_code=0x47; goto exit_Method_Set;
                    }
                    if((zSTS=AtomDecoding(atom_type,&DecLen)) != STS_SUCCESS){
                        err_code=0x48; goto exit_Method_Set;
                    }
                    MultiFunc_ColCnt=0;
                    SetParm_SameColNo_Cnt[SetParm_Cnt]=MultiFunc_ColCnt;  // always fill 0, first value in Write Column
                    if(ChkToken()!=TOK_EndName){
                        zSTS=STS_SESSION_ABORT; err_code=0x4c; goto exit_Method_Set;
                    }
                    //check 0xFF
                    SkipTokenEmpty();

                    AceColResult=AceColumnsChk(SetParm_ColNo[SetParm_Cnt]);
                    if(AceColResult == zOK){  // check target column whether allow to set in ACE table
                        if(IsSettingInSameColumn(SetParm_ColNo, SetParm_Cnt)==zOK){
                            zSTS=STS_INVALID_PARAMETER; err_code=0x4E; goto exit_Method_Set;
                        }
                        if(Write2Mtable(DecBuf,DecLen,SetParm_ColNo[SetParm_Cnt],SetParm_SameColNo_Cnt[SetParm_Cnt]) == zNG){
                            zSTS=STS_INVALID_PARAMETER; err_code=0x50; goto exit_Method_Set;
                        }
                        SetParm_Cnt++;
                    }
                }
            }
            else
            {
                zSTS=STS_SESSION_ABORT; err_code=0x54; goto exit_Method_Set;
            }
        }
        iPload--;
        if(ChkToken()!=TOK_EndList){
            zSTS=STS_SESSION_ABORT; err_code=0x58; goto exit_Method_Set;
        }

        if(SetParm_Cnt==0){  // no match column
            zSTS=STS_INVALID_PARAMETER; err_code=0x5c; goto exit_Method_Set;
        }
    }
//    else {   zSTS=STS_INVALID_PARAMETER; err_code=0x60; goto exit_Method_Set; }

    if(ChkToken()!=TOK_EndName){
        zSTS=STS_SESSION_ABORT; err_code=0x64; goto exit_Method_Set;
    }
    if(ChkToken()!=TOK_EndList){
        zSTS=STS_SESSION_ABORT; err_code=0x65; goto exit_Method_Set;
    }
    if(ChkToken()!=TOK_EndOfData){
        zSTS=STS_SESSION_ABORT; err_code=0x66; goto exit_Method_Set;
    }


    //status list check
    zSTS = ChkMethodStatus();
    if(zSTS!=STS_SUCCESS)
    {
        err_code=0x70;
        goto exit_Method_Set;
    }

    //check locking range start or range length whether has been changed
    if(LockingTabChanged){
        LockingTabChanged=FALSE;
        for(y=0; y<G3.b.mLckLocking_Tbl.rowCnt; y++){  //search row
            if(invokingUID.all==G3.b.mLckLocking_Tbl.val[y].uid){
                if(LockingTbl_RangeChk(G3.b.mLckLocking_Tbl.val[y].uid, G3.b.mLckLocking_Tbl.val[y].rangeStart, G3.b.mLckLocking_Tbl.val[y].rangeLength)==zNG){
                     zSTS=STS_INVALID_PARAMETER; err_code=0x72; goto exit_Method_Set;
                }
            }
        }

    }

exit_Method_Set:
    //prepare payload for response
    //dataBuf[iDataBuf++]=TOK_StartList;
    //here nothing to response, if method writing is OK.
    //dataBuf[iDataBuf++]=TOK_EndList;
    //dataBuf[iDataBuf++]=TOK_EndOfData;
    //if result is OK.
    //statusCode[0]=0;
    //statusCode[1]=0;
    //statusCode[2]=0;
    if(!err_code){
        dataBuf[iDataBuf++]=TOK_StartList;
        dataBuf[iDataBuf++]=TOK_EndList;
        dataBuf[iDataBuf++]=TOK_EndOfData;

        return STS_SUCCESS;
    }else{
        //statusCode[0]=STS_INVALID_PARAMETER;   //temp.  ,   need to check in feature.
        //statusCode[1]=0;
        //statusCode[2]=0;
        soutb3(0x5C, 0xFE, 0x09); soutb(err_code);
        TCG_PRINTF("Admin_Set err, ErrCode=%02X\n", err_code);
        return zSTS;   //or STS_SESSION_ABORT
    }
}

int IsSettingInSameColumn(U32 *colbuf, U32 curIndex)
{
    U32 i;
    U32 curVal;
    curVal=colbuf[curIndex];
    for(i=0; i<curIndex; i++){
        if(curVal==colbuf[i]){
            return zOK;
        }
    }
    return zNG;
}

int Write2Mtable(U8 *tBuf, U32 tLen, U32 setColNo, U8 MultiFunc_idx)
{
    U8  x,y;
    U8  GotFg=0;
    U8  ErrCode=0;
    U8  k;
    U8  i = 0;
    U64 uidAuthClass;

    soutb3(0x5c, 0xfe, 0x14); soutd(invokingUID.all >> 32); soutd(setColNo); soutd(tLen);
    TCG_PRINTF("M_Set: Col<%X> Len<%4X> ", (U32)setColNo, (U32)tLen);

    y=0;
    switch(invokingUID.all >> 32)
    {
        case 0x00000008:  // ACE table
            if(mSessionManager.SPID.all==UID_SP_Admin)
            {
                ErrCode=0x78; goto Exit_Write2Mtable;
            }
            else if(mSessionManager.SPID.all==UID_SP_Locking)
            {
                for(y=0; y<G3.b.mLckACE_Tbl.rowCnt; y++){  //search row
                    if(invokingUID.all==G3.b.mLckACE_Tbl.val[y].uid){
                        GotFg=1;  // y=row
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0x84; goto Exit_Write2Mtable;
                }
                GotFg=0;
                for(x=0; x<G3.b.mLckACE_Tbl.colCnt; x++){ //search column
                    if(setColNo==G3.b.mLckACE_Tbl.pty[x].colNo){
                        GotFg=1;  //x=column
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0x88; goto Exit_Write2Mtable;
                }
                if(tLen>G3.b.mLckACE_Tbl.pty[x].size){  // check length
                    ErrCode=0x8c; goto Exit_Write2Mtable;
                }
                switch(x)
                {
                    case 1:  //BooleanExpr
                        InvertByteSequence((U8)tLen, tBuf);
                        alexdebugls(*(U64 *)tBuf,ACE_Wr_BooleanExpr);
                        //ACE_C_PIN_UserX_Set_PIN shall only accept "Admins" and "Admins OR UserX"
                        if((invokingUID.all&0xFFFFFFFFFFFFFF00)==UID_ACE_C_PIN_User_Set_PIN)
                        {
                            uidAuthClass = *(U64 *)tBuf & 0xFFFFFFFFFFFFFF00;
                            if(MultiFunc_idx==0)
                            { //1st Auth must be Admins
                                if(uidAuthClass!=UID_Authority_AdminX)
                                {   ErrCode=0x8d; goto Exit_Write2Mtable;   }
                            }

                            if(uidAuthClass==UID_Authority_AdminX)
                            {
                                if(i> TCG_AdminCnt)
                                {   ErrCode=0x8f; goto Exit_Write2Mtable;   }
                            }
                            else if(uidAuthClass==UID_Authority_Users)
                            { //User UID can only be UserX
                                i=*(U64 *)tBuf & 0xFF;
                                if(i!=(invokingUID.all&0xff))
                                {   ErrCode=0x8f; goto Exit_Write2Mtable;   }
                            }
                            else //not Admins or Users UID
                            {   ErrCode=0x8f; goto Exit_Write2Mtable;   }
                        }

                        {
                            U64 UidAnyAdmin;
                            U64 UidAdminGeneral;
                            //test case D1-1-1-1-13  , (n user_x, 1 admin_x)
                            if(MultiFunc_idx>1){
                                UidAnyAdmin=UID_Authority_Admin1 & 0xFFFFFFFFFFFFFFF0;
                                UidAdminGeneral=(*(U64 *)tBuf) & 0xFFFFFFFFFFFFFFF0;
                                if(UidAdminGeneral==UidAnyAdmin){
                                    for(i=0; i<MultiFunc_idx; i++){
                                        if(UidAdminGeneral==(G3.b.mLckACE_Tbl.val[y].booleanExpr[i] & 0xFFFFFFFFFFFFFFF0)){  // same Adminx ?
                                            ErrCode=0x8e; goto Exit_Write2Mtable;
                                        }
                                    }
                                }
                            }
                        }
                        G3.b.mLckACE_Tbl.val[y].booleanExpr[MultiFunc_idx]=*(U64 *)tBuf;
                        G3MtableChanged=TRUE;
                        break;
                    default:
                        ErrCode=0x90;
                        goto Exit_Write2Mtable;
                }
            }
            break;

        case 0x00000009:  //Authority table
            if(mSessionManager.SPID.all==UID_SP_Admin)
            {   //admin
                for(y=0; y<G1.b.mAdmAuthority_Tbl.rowCnt; y++){  //search row
                    if((invokingUID.all)==(G1.b.mAdmAuthority_Tbl.val[y].uid)){
                        GotFg=1;  // y=row
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0x94; goto Exit_Write2Mtable;
                }
                GotFg=0;
                for(x=0; x<G1.b.mAdmAuthority_Tbl.colCnt; x++){ //search column
                    if(setColNo==G1.b.mAdmAuthority_Tbl.pty[x].colNo){
                        GotFg=1;  //x=column
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0x98; goto Exit_Write2Mtable;
                }
                if(tLen>G1.b.mAdmAuthority_Tbl.pty[x].size){  // check length
                    ErrCode=0x9c; goto Exit_Write2Mtable;
                }
                switch(x)
                {
                    case 4:
                        alexdebugds(*(BOOL *)tBuf,AdmAuthority_Wr_Enable);
                        G1.b.mAdmAuthority_Tbl.val[y].enabled=*(BOOL *)tBuf;
                        G1MtableChanged=TRUE;
                        break;
                    default:
                        ErrCode=0xa0;
                        goto Exit_Write2Mtable;
                }
            }
            else if(mSessionManager.SPID.all==UID_SP_Locking)
            {   //locking
                for(y=0; y<G3.b.mLckAuthority_Tbl.rowCnt; y++){  //search row
                    if((invokingUID.all)==(G3.b.mLckAuthority_Tbl.val[y].uid)){
                        GotFg=1;  // y=row
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0xa8; goto Exit_Write2Mtable;
                }
                GotFg=0;
                for(x=0; x<G3.b.mLckAuthority_Tbl.colCnt; x++){ //search column
                    if(setColNo==G3.b.mLckAuthority_Tbl.pty[x].colNo){
                        GotFg=1;  //x=column
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0xac; goto Exit_Write2Mtable;
                }
                if(tLen>G3.b.mLckAuthority_Tbl.pty[x].size){  // check length
                    ErrCode=0xb0; goto Exit_Write2Mtable;
                }
                switch(x)
                {
                    case 2: //CommonName
                        //alexdebugds(*(BOOL *)tBuf,AdmAuthority_Wr_Enable);
                        for(i=0;i<tLen;i++)
                            G3.b.mLckAuthority_Tbl.val[y].commonName[i]= *tBuf++;
                        if(i<sizeof(G3.b.mLckAuthority_Tbl.val[0].commonName))
                        G3.b.mLckAuthority_Tbl.val[y].commonName[i] = '\0';
                        G3MtableChanged=TRUE;
                        break;

                    case 5: //Enabled
                        alexdebugds(*(BOOL *)tBuf,AdmAuthority_Wr_Enable);
                        G3.b.mLckAuthority_Tbl.val[y].enabled=*(BOOL *)tBuf;
                        G3MtableChanged=TRUE;
                        break;
                    default:
                        ErrCode=0xb4;
                        goto Exit_Write2Mtable;
                }
            }
            break;

        case 0x0000000B:  //C_PIN table
            if(mSessionManager.SPID.all==UID_SP_Admin)
            {
                for(y=0; y<G1.b.mAdmCPin_Tbl.rowCnt; y++){  //search row
                    if((invokingUID.all)==(G1.b.mAdmCPin_Tbl.val[y].uid)){
                        GotFg=1;  // y=row
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0xbc; goto Exit_Write2Mtable;
                }
                GotFg=0;
                for(x=0; x<G1.b.mAdmCPin_Tbl.colCnt; x++){ //search column
                    if(setColNo==G1.b.mAdmCPin_Tbl.pty[x].colNo){
                        GotFg=1;  //x=column
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0xc0; goto Exit_Write2Mtable;
                }
                if(tLen>G1.b.mAdmCPin_Tbl.pty[x].size){  // check length
                    ErrCode=0xc4; goto Exit_Write2Mtable;
                }
                switch(x)
                {
                    case 2:
                        if(tLen>CPIN_LENGTH){ // password length
                            ErrCode=0xc4; goto Exit_Write2Mtable;
                        }
                        if(G1.b.mAdmCPin_Tbl.val[y].uid==UID_CPIN_PSID){    //check PSID PW whether could be written ?
                            if(IsPSIDWritable(y)==zNG){
                                ErrCode=0xc5; goto Exit_Write2Mtable; //It can not.
                            }
                        }
                        //if((invokingUID.all)==UID_CPIN_MSID){
                        //    G1.b.mAdmCPin_Tbl.val[y].pwd[0]=tLen;  //save length in byte0 of Array
                        //    MEM_COPY(&G1.b.mAdmCPin_Tbl.val[y].pwd[1], tBuf, tLen);  // array byte0 indicate length
                        //}else
                        {
                            #ifdef _ALEXDEBUG  // for debug
                            soutb3(0x5C,0xFE,0xF4);  // here ASCII code , ex 'A'=0x41, 'B'=0x42, etc....
                            soutb(tBuf[0]); soutb(tBuf[1]); soutb(tBuf[2]); soutb(tBuf[3]);
                            #endif

                            if(tLen)
                            {
                                soutb3(0x5C,0xFE,0xF5);
                                soutb(G1.b.mAdmCPin_Tbl.val[y].pwd[0]); soutb(G1.b.mAdmCPin_Tbl.val[y].pwd[1]); soutb(G1.b.mAdmCPin_Tbl.val[y].pwd[2]); soutb(G1.b.mAdmCPin_Tbl.val[y].pwd[3]); soutb(G1.b.mAdmCPin_Tbl.val[y].pwd[4]);

                                //sha256(tBuf, tLen, digest);
                                //MEM_COPY(&G1.b.mAdmCPin_Tbl.val[y].pwd[0], tBuf, tLen);  // array byte0 is not length
                                //G1.b.mAdmCPin_Tbl.val[y].pwd[0]=tLen;
                                //for(k=0; k<SHA256_DIGEST_SIZE; k++)
                                //    G1.b.mAdmCPin_Tbl.val[y].pwd[k+1]=digest[k];
                                CopyHashed(tBuf, tLen, G1.b.mAdmCPin_Tbl.val[y].pwd);

                                #ifdef _ALEXDEBUG  // for debug, after update G1.b.mAdmCPin_Tbl
                                soutb3(0x5C,0xFE,0xF6);
                                soutb(G1.b.mAdmCPin_Tbl.val[y].pwd[0]); soutb(G1.b.mAdmCPin_Tbl.val[y].pwd[1]); soutb(G1.b.mAdmCPin_Tbl.val[y].pwd[2]); soutb(G1.b.mAdmCPin_Tbl.val[y].pwd[3]); soutb(G1.b.mAdmCPin_Tbl.val[y].pwd[4]);
                                #endif
                            }
                            else
                            {
                                G1.b.mAdmCPin_Tbl.val[y].pwd[0]=tLen;
                                for(x=1;x<PWD_LENGTH;x++)
                                    G1.b.mAdmCPin_Tbl.val[y].pwd[x]=0;
                            }
                        }
                        G1MtableChanged=TRUE;
                        break;
                    default:
                        ErrCode=0xc8;
                        goto Exit_Write2Mtable;
                }
            }
            else if(mSessionManager.SPID.all==UID_SP_Locking)
            {
                for(y=0; y<G3.b.mLckCPin_Tbl.rowCnt; y++){  //search row
                    if((invokingUID.all)==(G3.b.mLckCPin_Tbl.val[y].uid)){
                        GotFg=1;  // y=row
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0xd0; goto Exit_Write2Mtable;
                }
                GotFg=0;
                for(x=0; x<G3.b.mLckCPin_Tbl.colCnt; x++){ //search column
                    if(setColNo==G3.b.mLckCPin_Tbl.pty[x].colNo){
                        GotFg=1;  //x=column
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0xd4; goto Exit_Write2Mtable;
                }
                if(tLen>G3.b.mLckCPin_Tbl.pty[x].size){  // check length
                    ErrCode=0xd8; goto Exit_Write2Mtable;
                }
                switch(x)
                {
                    case 2:
                        if(tLen>CPIN_LENGTH){ // password length
                            ErrCode=0xd8; goto Exit_Write2Mtable;
                        }
                        //MEM_COPY(&G3.b.mLckCPin_Tbl.val[y].pwd[0], tBuf, tLen);  // array byte0 is not length (password is Hash)
                        #ifdef _ALEXDEBUG  // for debug
                            soutb3(0x5C,0xFE,0xF7);  // here ASCII code , ex 'A'=0x41, 'B'=0x42, etc....
                            for(k=0;k<4;k++)
                            //for(k=0;k<tLen;k++)
                                soutb(tBuf[k]);
                        #endif

                        if(tLen)
                        {
                            soutb3(0x5C,0xFE,0xF8);
                            soutb(G3.b.mLckCPin_Tbl.val[y].pwd[0]); soutb(G3.b.mLckCPin_Tbl.val[y].pwd[1]); soutb(G3.b.mLckCPin_Tbl.val[y].pwd[2]); soutb(G3.b.mLckCPin_Tbl.val[y].pwd[3]); soutb(G3.b.mLckCPin_Tbl.val[y].pwd[4]);

                            //sha256(tBuf, tLen, digest);
                            //G3.b.mLckCPin_Tbl.val[y].pwd[0]=tLen;
                            //for(k=0; k<SHA256_DIGEST_SIZE; k++)
                            //    G3.b.mLckCPin_Tbl.val[y].pwd[k+1]=digest[k];
                            CopyHashed(tBuf, tLen, G3.b.mLckCPin_Tbl.val[y].pwd);
                            TCG_PRINTF("* LckCPIN: L%02X ",G3.b.mLckCPin_Tbl.val[y].pwd[0]);
                            for(i=1; i<9; i++)
                                TCG_PRINTF(" %02X",G3.b.mLckCPin_Tbl.val[y].pwd[i]);

                            #ifdef _ALEXDEBUG  // for debug, after update G1.b.mAdmCPin_Tbl
                            soutb3(0x5C,0xFE,0xF9);
                            soutb(G3.b.mLckCPin_Tbl.val[y].pwd[0]); soutb(G3.b.mLckCPin_Tbl.val[y].pwd[1]); soutb(G3.b.mLckCPin_Tbl.val[y].pwd[2]); soutb(G3.b.mLckCPin_Tbl.val[y].pwd[3]); soutb(G3.b.mLckCPin_Tbl.val[y].pwd[4]);
                            #endif
                        }
                        else
                        {
                            G3.b.mLckCPin_Tbl.val[y].pwd[0]=tLen;
                            for(x=1;x<PWD_LENGTH;x++)
                                G3.b.mLckCPin_Tbl.val[y].pwd[x]=0;
                        }
                        G3MtableChanged=TRUE;
                        break;
                    default:
                        ErrCode=0xdc;
                        goto Exit_Write2Mtable;
                }
            }
            break;

        case 0x00000802:  // Locking
            if(mSessionManager.SPID.all==UID_SP_Admin)
            {   //admin
                ErrCode=0xe0; goto Exit_Write2Mtable;
            }
            else if(mSessionManager.SPID.all==UID_SP_Locking)
            {   //locking
                for(y=0; y<G3.b.mLckLocking_Tbl.rowCnt; y++){  //search row
                    if((invokingUID.all)==(G3.b.mLckLocking_Tbl.val[y].uid)){
                        GotFg=1;  // y=row
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0xe8; goto Exit_Write2Mtable;
                }
                GotFg=0;
                for(x=0; x<G3.b.mLckLocking_Tbl.colCnt; x++){ //search column
                    if(setColNo==G3.b.mLckLocking_Tbl.pty[x].colNo){
                        GotFg=1;  //x=column
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0xec; goto Exit_Write2Mtable;
                }
                if(tLen>G3.b.mLckLocking_Tbl.pty[x].size){  // check length
                    ErrCode=0xf0; goto Exit_Write2Mtable;
                }

                switch(x)
                {
                    case 2:  //CommonName
                        //alexdebugds(*((U64 *)tBuf),Locking_Wr_RangeStart);

                        for(i=0;i<tLen;i++)
                            G3.b.mLckLocking_Tbl.val[y].commonName[i]=*tBuf++;
                        if(i<sizeof(G3.b.mLckAuthority_Tbl.val[0].commonName))
                        G3.b.mLckLocking_Tbl.val[y].commonName[i]='\0';

                        LockingTabChanged=TRUE;
                        G3MtableChanged=TRUE;
                        break;

                    case 3:  //RangeStart
                        TCG_PRINTF("* %08X", *((U32 *)tBuf));    //,Locking_Wr_RangeStart);
                        #if 0  // boundary need check ULink
                        if(*((U64 *)tBuf) & 0x000000000000000F){
                            ErrCode=0xf1; goto Exit_Write2Mtable;
                        }
                        #endif
                        G3.b.mLckLocking_Tbl.val[y].rangeStart=*((U64 *)tBuf);
                        LockingTabChanged=TRUE;
                        G3MtableChanged=TRUE;
                        break;
                    case 4:  //RangeLength
                        TCG_PRINTF("* %08X",*((U32 *)tBuf));    //,Locking_Wr_RangeLength);
                        #if 0  // boundary need check ULink
                        if(*((U64 *)tBuf) & 0x000000000000000F){
                            ErrCode=0xf2; goto Exit_Write2Mtable;
                        }
                        #endif
                        G3.b.mLckLocking_Tbl.val[y].rangeLength=*((U64 *)tBuf);
                        LockingTabChanged=TRUE;
                        G3MtableChanged=TRUE;
                        break;
                    case 5:  //ReadLockEnable
                        TCG_PRINTF("* %02X", tBuf[0]);  //,Locking_Wr_ReadLockEnable);
                        G3.b.mLckLocking_Tbl.val[y].readLockEnabled=tBuf[0];
                        G3MtableChanged=TRUE;
                        break;
                    case 6:  //WriteLockEnable
                        TCG_PRINTF("* %02X", tBuf[0]);  //,Locking_Wr_WriteLockEnable);
                        G3.b.mLckLocking_Tbl.val[y].writeLockEnabled=tBuf[0];
                        G3MtableChanged=TRUE;
                        break;
                    case 7:  //ReadLock
                        TCG_PRINTF("* %02X",tBuf[0]);   //,Locking_Wr_ReadLock);
                        G3.b.mLckLocking_Tbl.val[y].readLocked=tBuf[0];

                        //no need to write ReadLock to NAND if lockOnReset is PowerCycle and readLockEnabled=1
                        //(since fw will auto set ReadLock=true)
                        //if((G3.b.mLckLocking_Tbl.val[y].readLockEnabled==0)
                        // ||(G3.b.mLckLocking_Tbl.val[y].lockOnReset[1]!=PowerCycle))
                        G3MtableChanged=TRUE;

                        break;
                    case 8:  //WriteLock
                        TCG_PRINTF("* %02X",tBuf[0]);   //,Locking_Wr_WriteLock);
                        G3.b.mLckLocking_Tbl.val[y].writeLocked=tBuf[0];

                        //if((G3.b.mLckLocking_Tbl.val[y].writeLockEnabled==0)
                        // ||(G3.b.mLckLocking_Tbl.val[y].lockOnReset[1]!=PowerCycle))
                        G3MtableChanged=TRUE;

                        break;
                    //case 9:  //LockOnReset, { PowerCycle } or { PowerCycle, Programmatic }
                    //    alexdebug1s(tBuf[0],Locking_LockOnReset);
                    //    //TODO: G3.b.mLckLocking_Tbl.val[y].lockOnReset=tBuf[0];
                    //    G3MtableChanged=TRUE;
                    //    break;
                    default:
                        ErrCode=0xf4;
                        goto Exit_Write2Mtable;
                }
            }
            break;

        case 0x00000803:   // MBRControl
            if(mSessionManager.SPID.all==UID_SP_Admin)
            {   //admin
                ErrCode=0xf8; goto Exit_Write2Mtable;
            }
            else if(mSessionManager.SPID.all==UID_SP_Locking)
            {   //locking
                for(y=0; y<G3.b.mLckMbrCtrl_Tbl.rowCnt; y++){  //search row
                    if(invokingUID.all==G3.b.mLckMbrCtrl_Tbl.val[y].uid){
                        GotFg=1;  // y=row
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0x04; goto Exit_Write2Mtable;
                }
                GotFg=0;
                for(x=0; x<G3.b.mLckMbrCtrl_Tbl.colCnt; x++){ //search column
                    if(setColNo==G3.b.mLckMbrCtrl_Tbl.pty[x].colNo){
                        GotFg=1;  //x=column
                        break;
                    }
                }
                if(!GotFg){
                    ErrCode=0x08; goto Exit_Write2Mtable;
                }
                if(tLen>G3.b.mLckMbrCtrl_Tbl.pty[x].size){  // check length
                    ErrCode=0x0c; goto Exit_Write2Mtable;
                }
                switch(x)
                {
                    case 1:  //enable
                        alexdebug1s(tBuf[0],MBRctl_Wr_Enable);
                        G3.b.mLckMbrCtrl_Tbl.val[y].enable=tBuf[0];
                        G3MtableChanged=TRUE;
                        ResetAllCache();       //note : ResetAllCache() = {}
                        break;
                    case 2:  //done
                        alexdebug1s(tBuf[0],MBRctl_Wr_Done);
                        G3.b.mLckMbrCtrl_Tbl.val[y].done=tBuf[0];

                        //no need to write "Done' to NAND if doneOnReset is PowerCycle and Enable=1
                        //(since fw will auto set Done=false)
                        if((G3.b.mLckMbrCtrl_Tbl.val[y].enable==0)
                         ||(G3.b.mLckMbrCtrl_Tbl.val[y].doneOnReset!=PowerCycle))
                        G3MtableChanged=TRUE;
                        ResetAllCache();       //note : ResetAllCache() = {}
                        break;

                    default:
                        ErrCode=0x10;
                        goto Exit_Write2Mtable;
                }
            }
            break;

        case 0x00000804:   // MBR
            if(mSessionManager.SPID.all==UID_SP_Admin)
            {   // admin
                ErrCode=0x14; goto Exit_Write2Mtable;
            }
            else if(mSessionManager.SPID.all==UID_SP_Locking)
            {   //locking
                U32 LaaAddr,LaaOffAddr,wrptr;
                int rlen;

                alexdebugds(setColNo,MBR_Wr_StrtAddr);
                alexdebugds(tLen,MBR_Wr_Len);
                if((setColNo+tLen)>MBR_LEN || tLen > DECBUF_LEN){
                    ErrCode=0x15; goto Exit_Write2Mtable;
                }

                rlen=tLen;              //rlen = wr remain length
                wrptr=setColNo;         //wr point
                if(rlen>0){
                    U32 LaaCnt;
                    U8 *Desptr=(U8 *)TcgTmpBuf;

                    LaaAddr = wrptr / LAA_LEN;
                    LaaOffAddr = wrptr % LAA_LEN;
                    alexdebug2ds(LaaAddr,LaaOffAddr,LaaAddr__LaaOffAddr);
                    RdTcgMBRbyNormalRdCmd=FALSE;

                    LaaCnt = (LaaOffAddr+rlen)/LAA_LEN;
                    if((LaaOffAddr+rlen)%LAA_LEN) LaaCnt+=1;
                    alexdebug2ds(rlen,LaaCnt,rlen__LaaCnt);

                    //WaitMbrRd(LaaAddr, LaaAddr+LaaCnt, (U8 *)TcgTmpBuf);  //read 6 page for max transfer buffer
                    MEM_SET((U8 *)TcgTmpBuf,0x00,LaaCnt*LAA_LEN /*sizeof(TcgTmpBuf)*/);
                    if(LaaOffAddr || ((setColNo+tLen)%LAA_LEN))
                    {
                #if 1
                        U16 ii;
                        U16 HasBlank;

                        HasBlank = FALSE;

                        for(ii=LaaAddr; ii<LaaAddr+LaaCnt; ii++){
                            if(G5.b.TcgTempMbrL2P[TCG_SMBR_LAA_START + ii].pc.cell >= TCG_MBR_CELLS){
                                HasBlank = TRUE;
                                break;
                            }
                        }
                        if(HasBlank){
                            for(ii=LaaAddr; ii<LaaAddr+LaaCnt; ii++){
                                if((G5.b.TcgTempMbrL2P[TCG_SMBR_LAA_START + ii].pc.cell < TCG_MBR_CELLS)
                                    || (G4.b.TcgMbrL2P[TCG_SMBR_LAA_START + ii].pc.cell < TCG_MBR_CELLS))   //blank ?  //CA1+
                                {
                                    SMBR_Read(ii, ii+1, (U8 *)TcgTmpBuf + (ii-LaaAddr)*CFG_UDATA_PER_PAGE, NULL);   //read 1 page
                                }
                            }
                        }else{
                            SMBR_Read(LaaAddr, LaaAddr+LaaCnt, (U8 *)TcgTmpBuf, NULL);   //read all
                        }
                #else
                        SMBR_Read(LaaAddr, LaaAddr+LaaCnt, (U8 *)TcgTmpBuf, NULL);   //WaitMbrRd(LaaAddr, LaaAddr+LaaCnt, (U8 *)TcgTmpBuf);  //read 1 page
                #endif
                    }
                    //MEM_COPY((U8 *)&Desptr[LaaOffAddr], &tBuf[srcptr], rlen);
                    //The DRAM Data will happen data misscompare when copy big size pattern.
                    //Therefore move this feature to CPU1.
                    SMBR_Write(LaaAddr, LaaAddr+LaaCnt, TcgTmpBuf, LaaOffAddr, tLen, tBuf);
                    G4MBRChanged = TRUE;
                }
            }
            break;
        case 0x00001001:   // Data Store
        case 0x00001002:
        case 0x00001003:
        case 0x00001004:
        case 0x00001005:
        case 0x00001006:
        case 0x00001007:
        case 0x00001008:
        case 0x00001009:
            if(mSessionManager.SPID.all==UID_SP_Admin)
            {   //admin
                ErrCode=0x18; goto Exit_Write2Mtable;
            }
            else if(mSessionManager.SPID.all==UID_SP_Locking)
            {   //locking

                y = ((invokingUID.all >> 32) & 0xff) - 1;

                alexdebug2ds(setColNo,tLen,DSiWrAddr__DSiWrLen);
                alexdebug1s(y,tgtUser);
                alexdebug2ds(mDataStoreAddr[y].offset,mDataStoreAddr[y].length,userOffset__userLen);

                //if((setColNo+tLen)>DATASTORE_LEN || tLen>DECBUF_LEN ){
                if((setColNo+tLen)>mDataStoreAddr[y].length || tLen>DECBUF_LEN) {
                    ErrCode=0x1c; goto Exit_Write2Mtable;
                }

                // for test case A13-2-1-3-9
                if(tLen>0){
                    U32 LaaAddr,LaaOffAddr;
                    U32 LaaCnt;
                    U8 *Desptr=(U8 *)TcgTmpBuf;

                    setColNo += mDataStoreAddr[y].offset;
                    LaaAddr = setColNo / LAA_LEN;
                    LaaOffAddr = setColNo % LAA_LEN;  //offset
                    alexdebug2ds(LaaAddr,LaaOffAddr,LaaAddr__LaaOffAddr);

                    LaaCnt = (LaaOffAddr+tLen)/LAA_LEN;
                    if((LaaOffAddr+tLen)%LAA_LEN) LaaCnt+=1;
                    alexdebug2ds(LaaAddr, LaaAddr+LaaCnt-1,DSiLaaS__DSiLaaE);

                    //WaitDSRd(LaaAddr, LaaAddr+LaaCnt, (U8 *)TcgTmpBuf);  //read 6 page for max transfer buffer
                    MEM_SET((U8 *)TcgTmpBuf,0x00,LaaCnt*LAA_LEN /*sizeof(TcgTmpBuf)*/);
                    if(LaaOffAddr || ((setColNo+tLen)%LAA_LEN))
                    {
                        U16 ii;
                        U16 HasBlank;

                        HasBlank = FALSE;

                        for(ii=LaaAddr; ii<LaaAddr+LaaCnt; ii++){
                            if(G5.b.TcgTempMbrL2P[TCG_DS_LAA_START + ii].pc.cell >= TCG_MBR_CELLS)
                            {
                                HasBlank = TRUE;
                                break;
                            }
                        }
                        if(HasBlank){
                            for(ii=LaaAddr; ii<LaaAddr+LaaCnt; ii++){
                                if((G5.b.TcgTempMbrL2P[TCG_DS_LAA_START + ii].pc.cell < TCG_MBR_CELLS)
                                    || (G4.b.TcgMbrL2P[TCG_DS_LAA_START + ii].pc.cell < TCG_MBR_CELLS))
                                {   //blank ?
                                    DS_Read(ii, ii+1, (U8 *)TcgTmpBuf + (ii-LaaAddr)*CFG_UDATA_PER_PAGE);
                                }
                            }
                        }else{
                            DS_Read(LaaAddr, LaaAddr+LaaCnt, (U8 *)TcgTmpBuf);  // read all
                        }
                    }
                    //MEM_COPY((U8 *)&Desptr[LaaOffAddr], &tBuf[0], tLen);
                    //The DRAM Data will happen data misscompare when copy big size pattern.
                    //Therefore move this feature to CPU1.
                    DS_Write(LaaAddr, LaaAddr+LaaCnt, TcgTmpBuf, LaaOffAddr, tLen, tBuf);
                    G6DataStoreChanged = TRUE;
                }
            }
            break;

        case 0x00000201:   // TPerInfo
            if(invokingUID.all!=0x0000020100030001)
            {   ErrCode=0x20; goto Exit_Write2Mtable;  }

            switch(setColNo)
            {
                case 0x08:  //ProgrammaticResetEnable
                    alexdebug1s(tBuf[0],TPerInfo_Wr_PReset);
                    G1.b.mAdmTPerInfo_Tbl.val[0].preset=tBuf[0];
                    G1MtableChanged=TRUE;
                    break;
                default:
                    ErrCode=0x21;
                    goto Exit_Write2Mtable;
            }
            break;
        default:
            ErrCode=0x24; goto Exit_Write2Mtable;
    }
    TCG_PRINTF("\n");

    return zOK;

Exit_Write2Mtable:
    soutb3(0x5C, 0xFE, 0x15); soutb(ErrCode);
    TCG_PRINTF("Write2Mtable() ErrCode=%02X\n", ErrCode);
    return zNG;

}

void WriteMtable2NAND(void)
{
    HERE(NULL);
    if(mSessionManager.Write!=0x00){   // check write bit in start session payload
        if(G1MtableChanged){
            TcgH2cJobIssue(MSG_TCG_G1WR, NULL);   //WaitG1Wr();
        }
        if(G2MtableChanged){
            TcgH2cJobIssue(MSG_TCG_G2WR, NULL);   //WaitG2Wr();
        }

        if(G3MtableChanged){
            TcgH2cJobIssue(MSG_TCG_G3WR, NULL);   //WaitG3Wr();
            //LockingRangeTable_Update();  // move to other place
        }
        if(G4MBRChanged){
            TcgH2cJobIssue(MSG_TCG_SMBRCOMMIT, NULL);   //WaitMbrCommit();
        }
        if(G6DataStoreChanged){
            TcgH2cJobIssue(MSG_TCG_DSCOMMIT, NULL);   //WaitDSCommit();
        }
    }
}

void ReadNAND2Mtable(void)
{
    if(G1MtableChanged){
        TcgH2cJobIssue(MSG_TCG_G1RD, NULL);   //WaitG1Rd();
    }
    if(G2MtableChanged){
        TcgH2cJobIssue(MSG_TCG_G2RD, NULL);   //WaitG2Rd();
    }
    if(G3MtableChanged){
        TcgH2cJobIssue(MSG_TCG_G3RD, NULL);   //WaitG3Rd();
    }
    if(G4MBRChanged){
        TcgH2cJobIssue(MSG_TCG_TSMBRCLEAR, NULL);   //WaitTmpMbrClr();
    }
    if(G6DataStoreChanged){
        Start_DataStoreLaa=0;
        End_DataStoreLaa=DATASTORE_LEN/LAA_LEN;

        TcgH2cJobIssue(MSG_TCG_TDSCLEAR, NULL);   //WaitTmpDSClr();
    }
}

void ClearMtableChangedFlag(void)
{
    G1MtableChanged=FALSE;
    G2MtableChanged=FALSE;
    G3MtableChanged=FALSE;
    G4MBRChanged=FALSE;
    G6DataStoreChanged=FALSE;
}

void CalDataStoreLaaLoaction(U32 saddr, U32 eaddr)
{
    U16 sLaa,eLaa;

    sLaa=saddr/LAA_LEN;
    eLaa=eaddr/LAA_LEN;
    eLaa+=1;

    if(sLaa < Start_DataStoreLaa){
        Start_DataStoreLaa=sLaa;
    }
    if(eLaa > End_DataStoreLaa){
        End_DataStoreLaa=eLaa;
    }
}

int IsPSIDWritable(U8 psid_row)
{
#if 1
    U16 i;
    U8 pwd[PWD_LENGTH];

    CopyHashed(&G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[1], G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[0], pwd);

    for(i=0; i<=CPIN_LENGTH; i++){
        if(G1.b.mAdmCPin_Tbl.val[CPIN_PSID_IDX].pwd[i] != pwd[i])
            return zNG;
    }
#else
    U8 sid_row;
    U8 GotFg=0;
    U16 i;
    for(sid_row=0; sid_row<G1.b.mAdmCPin_Tbl.rowCnt; sid_row++){  //search row
        if(UID_CPIN_SID==(G1.b.mAdmCPin_Tbl.val[sid_row].uid)){
            GotFg=1;  // y=row
            break;
        }
    }
    if(!GotFg) return zNG;

    for(i=0; i<CPIN_LENGTH; i++){
        if((G1.b.mAdmCPin_Tbl.val[psid_row].pwd[i+1] != 0) && (G1.b.mAdmCPin_Tbl.val[psid_row].pwd[i+1] != G1.b.mAdmCPin_Tbl.val[sid_row].pwd[i+1])){
            return zNG;
        }
    }
#endif
    return zOK;
}


/***********************************************************
* Admin table getACL method
* ref. core spec 5.3.3.13
***********************************************************/
U16 Method_GetACL(void)
{
    U32 tTkDatLen;
    U16 i;
    U8 j;
    U8 err_code=0x00;  //no error
    U64 GetACL_InvUID;
    U64 GetACL_MtdUID;
    U64 GetACL_GetAclUID;
    U8 GotFg;
    int zSTS=STS_SUCCESS;


    if(invokingUID.all!=UID_AccessControl){
        zSTS=STS_NOT_AUTHORIZED; err_code=0x01; goto exit_Method_GetACL;
    }
    if(ChkToken()!=TOK_StartList){
        zSTS=STS_SESSION_ABORT; err_code=0x08; goto exit_Method_GetACL;
    }
    if((zSTS=AtomDecoding(ATOM_UID,&tTkDatLen)) != STS_SUCCESS){
        err_code=0x11; goto exit_Method_GetACL;
    }
    InvertByteSequence((U8)tTkDatLen, DecBuf);
    MEM_COPY(&GetACL_InvUID,DecBuf,sizeof(GetACL_InvUID));
    if((zSTS=AtomDecoding(ATOM_UID,&tTkDatLen)) != STS_SUCCESS){
        err_code=0x21; goto exit_Method_GetACL;
    }
    InvertByteSequence((U8)tTkDatLen, DecBuf);
    MEM_COPY(&GetACL_MtdUID,DecBuf,sizeof(GetACL_MtdUID));
    if(ChkToken()!=TOK_EndList){
        zSTS=STS_SESSION_ABORT; err_code=0x23; goto exit_Method_GetACL;
    }
    if(ChkToken()!=TOK_EndOfData){
        zSTS=STS_SESSION_ABORT; err_code=0x24; goto exit_Method_GetACL;
    }

    //status list check
    zSTS = ChkMethodStatus();
    if(zSTS!=STS_SUCCESS)
    {
        err_code=0x70;
        goto exit_Method_GetACL;
    }

    soutb3(0x5C, 0xFE, 0xD8); soutd((GetACL_InvUID)>>32); soutd(GetACL_InvUID);  soutd((GetACL_MtdUID)>>32); soutd(GetACL_MtdUID);
    TCG_PRINTF("Method getACL => acl_Inv:acl_Mtd %08X-%08X : %08X-%08X\n", (U32)(GetACL_InvUID>>32), (U32)GetACL_InvUID,
                                                                           (U32)(GetACL_MtdUID>>32), (U32)GetACL_MtdUID);

    GotFg=0;
    if(mSessionManager.SPID.all==UID_SP_Admin)
    {    //admin table
        for(i=0;i<G1.b.mAdmAccessCtrl_Tbl.rowCnt;i++)
        { // search for InvokingID
            if(G1.b.mAdmAccessCtrl_Tbl.val[i].invID==GetACL_InvUID)
            {
                if(G1.b.mAdmAccessCtrl_Tbl.val[i].mtdID==GetACL_MtdUID)
                {    //target found!
                    getAclAclUid=G1.b.mAdmAccessCtrl_Tbl.val[i].getAclAcl;
                    GotFg=1;
                    break;
                }
            }
        }
        if(!GotFg){
            zSTS=STS_NOT_AUTHORIZED; err_code=0x31; goto exit_Method_GetACL;
        }
        if(AdminAceBooleanExprChk()==zNG){
            err_code=0x39; goto exit_Method_GetACL;
        }
        dataBuf[iDataBuf++]=TOK_StartList;
        dataBuf[iDataBuf++]=TOK_StartList;
        for(j=0; j<ADM_ACCESSCTRL_ACL_CNT ;j++){
            GetACL_GetAclUID=G1.b.mAdmAccessCtrl_Tbl.val[i].acl[j];
            if(GetACL_GetAclUID==UID_Null)  break;
            InvertByteSequence(sizeof(GetACL_GetAclUID), (U8 *)&GetACL_GetAclUID);
            dataBuf[iDataBuf++]=0xA8;
            MEM_COPY(&dataBuf[iDataBuf],&GetACL_GetAclUID,sizeof(GetACL_GetAclUID));
            iDataBuf += sizeof(GetACL_GetAclUID);
        }
    }
    else
    {   // locking table
        for(i=0;i<G2.b.mLckAccessCtrl_Tbl.rowCnt;i++)
        { // search for InvokingID
            if(G2.b.mLckAccessCtrl_Tbl.val[i].invID==GetACL_InvUID)
            {
                if(G2.b.mLckAccessCtrl_Tbl.val[i].mtdID==GetACL_MtdUID)
                {    //target found!
                    getAclAclUid=G2.b.mLckAccessCtrl_Tbl.val[i].getAclAcl;
                    GotFg=1;
                    break;
                }
            }
        }
        if(!GotFg){
            err_code=0x41; goto exit_Method_GetACL;
        }
        if(LockingAceBooleanExprChk()==zNG){
            err_code=0x49; goto exit_Method_GetACL;
        }
        dataBuf[iDataBuf++]=TOK_StartList;
        dataBuf[iDataBuf++]=TOK_StartList;
        for(j=0; j<LCK_ACCESSCTRL_ACL_CNT ;j++){
            GetACL_GetAclUID=G2.b.mLckAccessCtrl_Tbl.val[i].acl[j];
            if(GetACL_GetAclUID==UID_Null)  break;
            InvertByteSequence(sizeof(GetACL_GetAclUID), (U8 *)&GetACL_GetAclUID);
            dataBuf[iDataBuf++]=0xA8;
            MEM_COPY(&dataBuf[iDataBuf],&GetACL_GetAclUID,sizeof(GetACL_GetAclUID));
            iDataBuf += sizeof(GetACL_GetAclUID);
        }

    }
    dataBuf[iDataBuf++]=TOK_EndList;
    dataBuf[iDataBuf++]=TOK_EndList;
    dataBuf[iDataBuf++]=TOK_EndOfData;
    statusCode[0]=0;
    statusCode[1]=0;
    statusCode[2]=0;
    //return zOK;
    return STS_SUCCESS;

exit_Method_GetACL:
    //prepare payload for response
    //dataBuf[iDataBuf++]=TOK_StartList;
    //here nothing to response,
    //dataBuf[iDataBuf++]=TOK_EndList;
    //dataBuf[iDataBuf++]=TOK_EndOfData;

    //statusCode[0]=STS_INVALID_PARAMETER;   //temp.  ,   need to check in feature.
    //statusCode[1]=0;
    //statusCode[2]=0;
    soutb3(0x5C, 0xFE, 0x0A); soutb(err_code);
    //return STS_INVALID_PARAMETER;   //or STS_SESSION_ABORT
    return zSTS;
}

int InvertByteSequence(U8 ByteAmount, U8 *SeqBuf)
{
    U8 i,lmt;
    U8 InvSeqBuf[16];  //max. 16 bytes

    lmt=sizeof(InvSeqBuf);
    if(ByteAmount > lmt) return zNG;
    for(i=0;i<ByteAmount;i++){
        InvSeqBuf[ByteAmount-i-1]=SeqBuf[i];
    }
    MEM_COPY(SeqBuf,InvSeqBuf,ByteAmount);
    return zOK;
}

U16 Method_Activate(void)
{
    U8 i;
    U16 result=0;
    U8 errCode=0;
    U64 tmp64;
    U8  tmp8, cnt=0;
    U32 nameValue, tmp32, DSTblSize[DSTBL_MAX_NUM], totalsize=0;

    U8  sgUserCnt=0;
    U8  sgUserPolicy=1;    //admin has ownership
    U16 sgUserRange=0;

    if(mSessionManager.HtSgnAuthority.all==UID_Authority_PSID)
        return STS_NOT_AUTHORIZED;

    //parameter check
    if(ChkToken()!=TOK_StartList)    //test cases 3.1.5
    {   errCode=0x10;  result=STS_SESSION_ABORT;  goto MTD_EXIT;    }

    //retrieve parameter 'DataStoreTblSizes'
    tmp8 = ChkToken();
#if _TCG_ != TCG_PYRITE    //SingleUserMode and Additional DataStore
    if(tmp8==TOK_StartName)
    {
        if(invokingUID.all!=UID_SP_Locking)   //cj added for DM test
        {    errCode=0x50;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

        if(AtomDecoding_Uint32(&nameValue)!=STS_SUCCESS)
        {   errCode=0x20;  result=STS_INVALID_PARAMETER;  goto MTD_EXIT;    }

        if(nameValue==0x060000) //'SingleUserSelectionList'
        {
            //mSgUser.cnt=0;
            //mSgUser.policy=0;
            //mSgUser.range=0;

            if(ChkToken()==TOK_StartList)
            { // Locking table objects list
                tmp8=0;
                while(ChkToken()!=TOK_EndList)
                {
                    iPload--;

                    if(AtomDecoding_Uid2((U8 *)&tmp64)!=STS_SUCCESS)
                    {   errCode=0x30;  result=STS_INVALID_PARAMETER;  goto MTD_EXIT;    }

                    //check if it is in the locking table
                    for(i=tmp8;i<G3.b.mLckLocking_Tbl.rowCnt;i++)
                    {
                        if(G3.b.mLckLocking_Tbl.val[i].uid==tmp64)
                        {
                            //mSgUser.range[mSgUser.cnt]=i;
                            sgUserRange |= (0x01<<i);
                            sgUserCnt++;
                            tmp8 = i;
alexdebug2s(sgUserCnt, i, LckTblObj);
                            break;
                        }
                    }

                    if(i>=G3.b.mLckLocking_Tbl.rowCnt)
                    {   errCode=0x31;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }
                }
            }
            else
            { // check if entire Locking table
                iPload--;
                if(AtomDecoding_Uid2((U8 *)&tmp64)!=STS_SUCCESS)
                {    errCode=0x32;  result=STS_STAY_IN_IF_SEND;    goto MTD_EXIT; }

                if(tmp64!=UID_Locking)
                {    errCode=0x33;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

                sgUserCnt=LOCKING_RANGE_CNT+1;
                sgUserRange=0xffff;   //EntireLocking
alexdebug1s(sgUserCnt,EntireLocking);
            }

            if(ChkToken()!=TOK_EndName)
            {    errCode=0x34;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

            if((tmp8=ChkToken())!=TOK_StartName)
                goto CHK_ENDLIST;

            if(AtomDecoding_Uint32(&nameValue)!=STS_SUCCESS)
            {    errCode=0x35;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }
        }

        if(sgUserCnt)
            sgUserPolicy = 0;   //User has ownership

        if(nameValue==0x060001) //'RangePolicy'
        {
            if(AtomDecoding_Uint8(&tmp8)!=STS_SUCCESS)
            {    errCode=0x38;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

            if(tmp8==0)
            {
                if(sgUserCnt!=0) sgUserPolicy=0;
            }
            else if(tmp8==1)
                sgUserPolicy=1;
            else
            {    errCode=0x39;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }
alexdebug1s(sgUserPolicy,policy);

            if(ChkToken()!=TOK_EndName)
            {    errCode=0x3A;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

            if((tmp8=ChkToken())!=TOK_StartName)
                goto CHK_ENDLIST;

            if(AtomDecoding_Uint32(&nameValue)!=STS_SUCCESS)
            {    errCode=0x3B;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

        }

        //check Additional DataStore Parameter
        if(nameValue==0x060002) //'DataStoreTblSizes'
        {
            if(ChkToken()!=TOK_StartList)
            {    errCode=0x21;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

            while(ChkToken()!=TOK_EndList)
            {
                iPload--;

                if(AtomDecoding_Uint32(&tmp32)!=STS_SUCCESS)
                {    errCode=0x22;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

                if(cnt>=DSTBL_MAX_NUM)      //too many tables
                {    errCode=0x23;  result=STS_INSUFFICIENT_SPACE;    goto MTD_EXIT; }

                totalsize += tmp32;
                if(totalsize>DATASTORE_LEN) //size is too large
                {    errCode=0x24;  result=STS_INSUFFICIENT_SPACE;    goto MTD_EXIT; }

                if(tmp32%DSTBL_ALIGNMENT)   //not aligned
                {
                    souts("not aligned:"); soutd(tmp32);
                    errCode=0x25;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

                DSTblSize[cnt]=tmp32;
alexdebugds(tmp32,DSTblSize);
                cnt++;
            }

            if(ChkToken()!=TOK_EndName)
            {    errCode=0x26;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

            tmp8 = ChkToken();
        }
        else
        {    errCode=0x27;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }
    }
#endif
CHK_ENDLIST:
    if(tmp8!=TOK_EndList)      //test cases 3.1.5
    {    errCode=0x15;  result=STS_SESSION_ABORT;    goto MTD_EXIT; }

    if(ChkToken()!=TOK_EndOfData)    //test cases 3.1.5
    {    errCode=0x16;  result=STS_SESSION_ABORT;    goto MTD_EXIT; }

    //status list check
    result = ChkMethodStatus();
    if(result!=STS_SUCCESS)
    {    errCode=0x17;  goto MTD_EXIT; }


    //TODO: Opal 5.2.1: return fail with a status FAIL if ATA Security is enabled
    if ((gNvmeIfMgr.SecurityMgr.SecurityState != SCU_SEC1) &&
        (gNvmeIfMgr.SecurityMgr.SecurityState != SCU_SEC2))
    {
        dataBuf[iDataBuf++]=TOK_StartList;
        dataBuf[iDataBuf++]=TOK_EndList;
        dataBuf[iDataBuf++]=TOK_EndOfData;

        //return STS_FAIL;
        {    errCode=0x18;  result=STS_FAIL;    goto MTD_EXIT; }
    }
    else
    {
//        if((invokingUID.all==UID_SP_Locking) &&
        if(G1.b.mAdmSP_Tbl.val[SP_LOCKING_IDX].lifeCycle==manufactured_inactive)
        {
            mSgUser.cnt=sgUserCnt;
            mSgUser.policy=sgUserPolicy;
            mSgUser.range=sgUserRange;

            //if(cnt)
                DataStore_Setting(cnt, DSTblSize);

            //if(mSgUser.cnt)
                SingleUser_Setting();
            //if(cnt||mSgUser.cnt)
            {
                TcgH2cJobIssue(MSG_TCG_G2WR, NULL);   //WaitG2Wr();  //AccessCtrlTbl, cj: add LockingInfoTbl
                //WaitG3Wr();  //ACE, Authority
            }

            //TODO:
            //  1. bit 1 of word 82, bit 1 of word 85 and all bits of word 89, 90, 92, 128 in the IDENTIFY DEVICE
            //     data SHALL be set to all-0.
            //  2. LockingEnabled bit in Locking Feature Descriptor in the Level 0 SHALL be set to 1. (v)
            //  3. LifeCycleState of Locking SP object in the SP table SHALL be set to 0x09. (v)
            //  4. A startup of a session to the Locking SP can succeed. (v)
            //  5. PIN for Admin1 in LockingSP should be set to SID PIN (Application Note)
            //  6. update table to NAND

            for (i = 0; i < PWD_LENGTH; i++){
                G3.b.mLckCPin_Tbl.val[0].pwd[i] = G1.b.mAdmCPin_Tbl.val[0].pwd[i];  //copy SID pin to Admin pin
            }

            TcgH2cJobIssue(MSG_TCG_G3WR, NULL);   //WaitG3Wr();

            G1.b.mAdmSP_Tbl.val[SP_LOCKING_IDX].lifeCycle = manufactured;     // (3)
            TcgH2cJobIssue(MSG_TCG_G1WR, NULL);   //WaitG1Wr();

            mTcgStatus |= TCG_ACTIVATED;
        }
        else
        {
            errCode=0xff;
        }

    dataBuf[iDataBuf++]=TOK_StartList;
    dataBuf[iDataBuf++]=TOK_EndList;
    dataBuf[iDataBuf++]=TOK_EndOfData;

        result = STS_SUCCESS;
    }

MTD_EXIT:
    soutb3(0x5C, 0xFE, 0x0D);  soutb2(errCode, cnt); soutw(mSgUser.range); //Method->Activate
    TCG_PRINTF("M_Activate: Err=%02X, DSCnt=%02X, SURx=%04X\n",errCode, cnt, mSgUser.range);
    return result;

}

void SingleUser_Setting(void)
{
    U8 i;
    U16 j,k=0;
    U64 tmp64;

//alexdebug2s(mSgUser.cnt, mSgUser.range, SingleUser);
soutb3(0x5C, 0xFE, 0x58); soutb2(mSgUser.cnt, mSgUser.policy); soutw(mSgUser.range);

    if(mSgUser.cnt!=0)
    {
        if(mSgUser.range&0x01)
        { //GlobalRange -> SingerUser
            //1. update AccessCtrl table
            k=0;
            tmp64 = ~UID_MethodID_Set;
            for(j=0;j<G2.b.mLckAccessCtrl_Tbl.rowCnt;j++)
            { // add ACE_Locking_GRange_Erase.Set
                if((G2.b.mLckAccessCtrl_Tbl.val[j].invID==UID_ACE_Locking_GRange_Erase)&&(G2.b.mLckAccessCtrl_Tbl.val[j].mtdID==tmp64))
                {
//soutb(0x10);
                    G2.b.mLckAccessCtrl_Tbl.val[j].mtdID=UID_MethodID_Set;
                    k=j;
                    break;
                }
            }
            for(j=k;j<G2.b.mLckAccessCtrl_Tbl.rowCnt;j++)
            { //modify User1.Set ACL
                if((G2.b.mLckAccessCtrl_Tbl.val[j].invID==UID_Authority_User1)&&(G2.b.mLckAccessCtrl_Tbl.val[j].mtdID==UID_MethodID_Set))
                {
//soutb(0x20);
                    G2.b.mLckAccessCtrl_Tbl.val[j].acl[0]=UID_ACE_User1_Set_CommonName;
                    G2.b.mLckAccessCtrl_Tbl.val[j].acl[1]=0;
                    G2.b.mLckAccessCtrl_Tbl.val[j].acl[2]=0;
                    G2.b.mLckAccessCtrl_Tbl.val[j].acl[3]=0;
                    k=j;
                    break;
                }
            }
            for(j=k;j<G2.b.mLckAccessCtrl_Tbl.rowCnt;j++)
            { // modify C_PIN_User1.Get
                if((G2.b.mLckAccessCtrl_Tbl.val[j].invID==UID_CPIN_User1)&&(G2.b.mLckAccessCtrl_Tbl.val[j].mtdID==UID_MethodID_Get))
                {
//soutb(0x30);
                    G2.b.mLckAccessCtrl_Tbl.val[j].acl[0]=UID_ACE_CPIN_Anybody_Get_NoPIN;
                    G2.b.mLckAccessCtrl_Tbl.val[j].acl[1]=0;
                    G2.b.mLckAccessCtrl_Tbl.val[j].acl[2]=0;
                    G2.b.mLckAccessCtrl_Tbl.val[j].acl[3]=0;
                    k=j;
                    break;
                }
            }
            for(j=k;j<G2.b.mLckAccessCtrl_Tbl.rowCnt;j++)
            { // modify GRange.Set ACL
                if((G2.b.mLckAccessCtrl_Tbl.val[j].invID==UID_Locking_GRange)&&(G2.b.mLckAccessCtrl_Tbl.val[j].mtdID==UID_MethodID_Set))
                {
//soutb(0x40);
                    if(mSgUser.policy)
                    {
                        G2.b.mLckAccessCtrl_Tbl.val[j].acl[0]=UID_ACE_Locking_GRange_Set_ReadToLOR;
                        G2.b.mLckAccessCtrl_Tbl.val[j].acl[1]=UID_ACE_Admins_Set_CommonName;
                        G2.b.mLckAccessCtrl_Tbl.val[j].acl[2]=0;
                        G2.b.mLckAccessCtrl_Tbl.val[j].acl[3]=0;
                    }
                    else
                    {
                        G2.b.mLckAccessCtrl_Tbl.val[j].acl[0]=UID_ACE_Locking_GRange_Set_ReadToLOR;
                        G2.b.mLckAccessCtrl_Tbl.val[j].acl[1]=UID_ACE_User1_Set_CommonName;
                        G2.b.mLckAccessCtrl_Tbl.val[j].acl[2]=0;
                        G2.b.mLckAccessCtrl_Tbl.val[j].acl[3]=0;
                    }
                    k=j;
                    break;
                }
            }
            tmp64 = ~UID_MethodID_Erase;
            for(j=k;j<G2.b.mLckAccessCtrl_Tbl.rowCnt;j++)
            { // add GRange.Erase
                if((G2.b.mLckAccessCtrl_Tbl.val[j].invID==UID_Locking_GRange)&&(G2.b.mLckAccessCtrl_Tbl.val[j].mtdID==tmp64))
                {
//soutb(0x50);
                    G2.b.mLckAccessCtrl_Tbl.val[j].mtdID=UID_MethodID_Erase;
                    //k=j;
                    break;
                }
}

            //2. update ACE table, need to follow the row sequence...
            for(j=0;j<G3.b.mLckACE_Tbl.rowCnt;j++)
            { //modify
                if(G3.b.mLckACE_Tbl.val[j].uid==UID_ACE_User1_Set_CommonName)
                {
//soutb(0x60);
                    G3.b.mLckACE_Tbl.val[j].booleanExpr[0]= UID_Authority_User1;
                    for(k=1;k<LCK_ACE_BOOLEXPR_CNT; k++)
                        G3.b.mLckACE_Tbl.val[j].booleanExpr[k]=0;
                    k=j;
                    break;
                }
            }
            for(j=k;j<G3.b.mLckACE_Tbl.rowCnt;j++)
            { //modify
                if(G3.b.mLckACE_Tbl.val[j].uid==UID_ACE_C_PIN_User1_Set_PIN)
                {
//soutb(0x70);
                    G3.b.mLckACE_Tbl.val[j].booleanExpr[0]= UID_Authority_User1;
                    for(k=1;k<LCK_ACE_BOOLEXPR_CNT; k++)
                        G3.b.mLckACE_Tbl.val[j].booleanExpr[k]=0;
                    k=j;
                    break;
                }
            }
            for(j=k;j<G3.b.mLckACE_Tbl.rowCnt;j++)
            { //modify

                if(G3.b.mLckACE_Tbl.val[j].uid==UID_ACE_K_AES_256_GlobalRange_GenKey)
                {
//soutb(0x80);
                    G3.b.mLckACE_Tbl.val[j].booleanExpr[0]= UID_Authority_User1;
                    for(k=1;k<LCK_ACE_BOOLEXPR_CNT; k++)
                        G3.b.mLckACE_Tbl.val[j].booleanExpr[k]=0;
                    k=j;
                    break;
                }
            }
            for(j=k;j<G3.b.mLckACE_Tbl.rowCnt;j++)
            { //modify
                if(G3.b.mLckACE_Tbl.val[j].uid==UID_ACE_Locking_GRange_Get_RangeStartToActiveKey)
                {
//soutb(0x90);
                    G3.b.mLckACE_Tbl.val[j].booleanExpr[0]= UID_Authority_Anybody;
                    for(k=1;k<LCK_ACE_BOOLEXPR_CNT; k++)
                        G3.b.mLckACE_Tbl.val[j].booleanExpr[k]=0;
                    k=j;
                    break;
                }
            }
            for(j=k;j<G3.b.mLckACE_Tbl.rowCnt;j++)
            { //add
                if((U32)G3.b.mLckACE_Tbl.val[j].uid==(U32)UID_ACE_Locking_GRange_Set_ReadToLOR)
                {
//soutb(0xA0);
                    G3.b.mLckACE_Tbl.val[j].uid = UID_ACE_Locking_GRange_Set_ReadToLOR;
                    k=j;
                    break;
                }
            }
            for(j=k;j<G3.b.mLckACE_Tbl.rowCnt;j++)
            { //add
                if((U32)G3.b.mLckACE_Tbl.val[j].uid==(U32)UID_ACE_CPIN_Anybody_Get_NoPIN)
                {
//soutb(0xB0);
                    G3.b.mLckACE_Tbl.val[j].uid = UID_ACE_CPIN_Anybody_Get_NoPIN;
                    k=j;
                    break;
                }
            }
            for(j=k;j<G3.b.mLckACE_Tbl.rowCnt;j++)
            { //add
                if((U32)G3.b.mLckACE_Tbl.val[j].uid==(U32)UID_ACE_Locking_GRange_Erase)
                {
//soutb(0xC0);
                    G3.b.mLckACE_Tbl.val[j].uid = UID_ACE_Locking_GRange_Erase;
                    //k=j;
                    break;
                }
            }

            //3. update Authority table
            for(j=0;j<G3.b.mLckAuthority_Tbl.rowCnt;j++)
            {
                if(G3.b.mLckAuthority_Tbl.val[j].uid==UID_Authority_User1)
                {
//soutb(0xD0);
                    G3.b.mLckAuthority_Tbl.val[j].enabled= 1;
                    break;
                }
            }
        }

        //for Range 1~7
        for(i=1; i<=LOCKING_RANGE_CNT; i++)
        {
            if(mSgUser.range&(0x01<<i))
            { //RangeN -> SingerUser
                //1. update AccessCtrl table
                k=0;
                tmp64 = ~UID_MethodID_Set;
                for(j=0;j<G2.b.mLckAccessCtrl_Tbl.rowCnt;j++)
                { // add ACE_Locking_GRange_Erase Set method
                    if(G2.b.mLckAccessCtrl_Tbl.val[j].invID==(UID_ACE_Locking_Range1_Erase+i-1))
                    {
                        if(G2.b.mLckAccessCtrl_Tbl.val[j].mtdID==tmp64)
                        {
//soutb(0x10+i);
                            G2.b.mLckAccessCtrl_Tbl.val[j].mtdID=UID_MethodID_Set;
                            k=j;
                            break;
                        }
                    }
                }
                for(j=k;j<G2.b.mLckAccessCtrl_Tbl.rowCnt;j++)
                { //modify UserN+1.Set ACL
                    if(G2.b.mLckAccessCtrl_Tbl.val[j].invID==(UID_Authority_User1+i))
                    {
                        if(G2.b.mLckAccessCtrl_Tbl.val[j].mtdID==UID_MethodID_Set)
                        {
//soutb(0x20+i);
                            G2.b.mLckAccessCtrl_Tbl.val[j].acl[0]=UID_ACE_User1_Set_CommonName+i;
                            G2.b.mLckAccessCtrl_Tbl.val[j].acl[1]=0;
                            G2.b.mLckAccessCtrl_Tbl.val[j].acl[2]=0;
                            G2.b.mLckAccessCtrl_Tbl.val[j].acl[3]=0;
                            k=j;
                            break;
                        }
                    }
                }
                for(j=k;j<G2.b.mLckAccessCtrl_Tbl.rowCnt;j++)
                { // modify C_PIN_UserN+1 Get method
                    if(G2.b.mLckAccessCtrl_Tbl.val[j].invID==(UID_CPIN_User1+i))
                    {
                        if(G2.b.mLckAccessCtrl_Tbl.val[j].mtdID==UID_MethodID_Get)
                        {
//soutb(0x30+i);
                            G2.b.mLckAccessCtrl_Tbl.val[j].acl[0]=UID_ACE_CPIN_Anybody_Get_NoPIN;
                            G2.b.mLckAccessCtrl_Tbl.val[j].acl[1]=0;
                            G2.b.mLckAccessCtrl_Tbl.val[j].acl[2]=0;
                            G2.b.mLckAccessCtrl_Tbl.val[j].acl[3]=0;
                            k=j;
                            break;
                        }
                    }
                }
                for(j=k;j<G2.b.mLckAccessCtrl_Tbl.rowCnt;j++)
                { // modify RangeN.Set ACL
                    if(G2.b.mLckAccessCtrl_Tbl.val[j].invID==(UID_Locking_Range+i))
                    {
                        if(G2.b.mLckAccessCtrl_Tbl.val[j].mtdID==UID_MethodID_Set)
                        {
//soutb(0x40+i);
                            if(mSgUser.policy)
                            {
                                G2.b.mLckAccessCtrl_Tbl.val[j].acl[0]=UID_ACE_Locking_Range1_Set_ReadToLOR + i-1;
                                G2.b.mLckAccessCtrl_Tbl.val[j].acl[1]=UID_ACE_Locking_Range1_Set_Range + i-1;
                                G2.b.mLckAccessCtrl_Tbl.val[j].acl[2]=UID_ACE_Admins_Set_CommonName;
                                G2.b.mLckAccessCtrl_Tbl.val[j].acl[3]=0;
                            }
                            else
                            {
                                G2.b.mLckAccessCtrl_Tbl.val[j].acl[0]=UID_ACE_Locking_Range1_Set_ReadToLOR + i-1;
                                G2.b.mLckAccessCtrl_Tbl.val[j].acl[1]=UID_ACE_Locking_Range1_Set_Range + i-1;
                                G2.b.mLckAccessCtrl_Tbl.val[j].acl[2]=UID_ACE_User1_Set_CommonName+i;
                                G2.b.mLckAccessCtrl_Tbl.val[j].acl[3]=0;
                            }
                            k=j;
                            break;
                        }
                    }
                }
                tmp64 = ~UID_MethodID_Erase;
                for(j=k;j<G2.b.mLckAccessCtrl_Tbl.rowCnt;j++)
                { // add RangeN Erase method
                    if(G2.b.mLckAccessCtrl_Tbl.val[j].invID==(UID_Locking_Range+i))
                    {
                        if(G2.b.mLckAccessCtrl_Tbl.val[j].mtdID==tmp64)
                        {
//soutb(0x50+i);
                            G2.b.mLckAccessCtrl_Tbl.val[j].mtdID=UID_MethodID_Erase;
                            k=j;
                            break;
                        }
                    }
                }

                //2. update ACE table
                for(j=0;j<G3.b.mLckACE_Tbl.rowCnt;j++)
                { //modify
                    if(G3.b.mLckACE_Tbl.val[j].uid==(UID_ACE_User1_Set_CommonName+i))
                    {
//soutb(0x60+i);
                        G3.b.mLckACE_Tbl.val[j].booleanExpr[0]= UID_Authority_User1+i;
                        for(k=1;k<LCK_ACE_BOOLEXPR_CNT; k++)
                            G3.b.mLckACE_Tbl.val[j].booleanExpr[k]=0;
                        k=j;
                        break;
                    }
                }
                for(j=k;j<G3.b.mLckACE_Tbl.rowCnt;j++)
                { //modify
                    if(G3.b.mLckACE_Tbl.val[j].uid==(UID_ACE_C_PIN_User1_Set_PIN+i))
                    {
//soutb(0x70+i);
                        G3.b.mLckACE_Tbl.val[j].booleanExpr[0]= UID_Authority_User1+i;
                        for(k=1;k<LCK_ACE_BOOLEXPR_CNT; k++)
                            G3.b.mLckACE_Tbl.val[j].booleanExpr[k]=0;
                        k=j;
                        break;
                    }
                }
                for(j=k;j<G3.b.mLckACE_Tbl.rowCnt;j++)
                { //modify
                    if(G3.b.mLckACE_Tbl.val[j].uid==(UID_ACE_K_AES_256_Range1_GenKey+i-1))
                    {
//soutb(0x80+i);
                        G3.b.mLckACE_Tbl.val[j].booleanExpr[0]= UID_Authority_User1+i;
                        for(k=1;k<LCK_ACE_BOOLEXPR_CNT; k++)
                            G3.b.mLckACE_Tbl.val[j].booleanExpr[k]=0;
                        k=j;
                        break;
                    }
                }
                for(j=k;j<G3.b.mLckACE_Tbl.rowCnt;j++)
                { //modify
                    if(G3.b.mLckACE_Tbl.val[j].uid==(UID_ACE_Locking_Range1_Get_RangeStartToActiveKey+i-1))
                    {
//soutb(0x90+i);
                        G3.b.mLckACE_Tbl.val[j].booleanExpr[0]= UID_Authority_Anybody;
                        for(k=1;k<LCK_ACE_BOOLEXPR_CNT; k++)
                            G3.b.mLckACE_Tbl.val[j].booleanExpr[k]=0;
                        k=j;
                        break;
                    }
                }
                for(j=k;j<G3.b.mLckACE_Tbl.rowCnt;j++)
                { //add
                    if((U32)G3.b.mLckACE_Tbl.val[j].uid==(U32)(UID_ACE_Locking_Range1_Set_ReadToLOR+i-1))
                    {
//soutb(0xA0+i);
                        G3.b.mLckACE_Tbl.val[j].uid = UID_ACE_Locking_Range1_Set_ReadToLOR+i-1;
                        k=j;
                        break;
                    }
                }
                for(j=0;j<G3.b.mLckACE_Tbl.rowCnt;j++)
                { //add
                    if((U32)G3.b.mLckACE_Tbl.val[j].uid==(U32)(UID_ACE_Locking_Range1_Set_Range+i-1))
                    {
//soutb(0xB0+i);
                        G3.b.mLckACE_Tbl.val[j].uid = UID_ACE_Locking_Range1_Set_Range+i-1;
                        if(mSgUser.policy)
                            G3.b.mLckACE_Tbl.val[j].booleanExpr[0]= UID_Authority_Admins;
                        else
                            G3.b.mLckACE_Tbl.val[j].booleanExpr[0]= UID_Authority_User1+i;
                        for(k=1;k<LCK_ACE_BOOLEXPR_CNT; k++)
                            G3.b.mLckACE_Tbl.val[j].booleanExpr[k]=0;
                        k=j;
                        break;
                    }
                }
                for(j=0;j<G3.b.mLckACE_Tbl.rowCnt;j++)
                { //add
                    if((U32)G3.b.mLckACE_Tbl.val[j].uid==(U32)UID_ACE_CPIN_Anybody_Get_NoPIN)
                    {
//soutb(0xC0+i);
                        G3.b.mLckACE_Tbl.val[j].uid = UID_ACE_CPIN_Anybody_Get_NoPIN;
                        k=j;
                        break;
                    }
                }

                for(j=0;j<G3.b.mLckACE_Tbl.rowCnt;j++)
                { //add
                    if((U32)G3.b.mLckACE_Tbl.val[j].uid==(U32)(UID_ACE_Locking_Range1_Erase+i-1))
                    {
//soutb(0xD0+i);
                        G3.b.mLckACE_Tbl.val[j].uid = UID_ACE_Locking_Range1_Erase+i-1;
                        k=j;
                        break;
                    }
                }

                //3. update Authority table
                for(j=0;j<G3.b.mLckAuthority_Tbl.rowCnt;j++)
                {
                    if(G3.b.mLckAuthority_Tbl.val[j].uid==(UID_Authority_User1+i))
                    {
//soutb(0xE0+i);
                        G3.b.mLckAuthority_Tbl.val[j].enabled= 1;
                        break;
                    }
                }
            }
        }


        //update LockingInfo table
        for(j=0;j<=LOCKING_RANGE_CNT;j++)
            G2.b.mLckLockingInfo_Tbl.val[0].singleUserModeRange[j]=0;

        if(mSgUser.range==0xffff) // EntireLocking
            G2.b.mLckLockingInfo_Tbl.val[0].singleUserModeRange[0]=UID_Locking;
        else
        {
            j=0;
            if(mSgUser.range&0x01)
                G2.b.mLckLockingInfo_Tbl.val[0].singleUserModeRange[j++]=UID_Locking_GRange;

            for(i=1; i<8; i++)
            {
                if(mSgUser.range&(0x01<<i))
                   G2.b.mLckLockingInfo_Tbl.val[0].singleUserModeRange[j++]=UID_Locking_Range+i;
            }
        }

        if(mSgUser.policy)
            G2.b.mLckLockingInfo_Tbl.val[0].rangeStartLengthPolicy=1;
        else
            G2.b.mLckLockingInfo_Tbl.val[0].rangeStartLengthPolicy=0;
    }

}

void DataStore_Setting(U8 cnt, U32 * DSTblSize)
{
    U8 i,tmp8;
    U16 j,k=0;
    U32 tmp32;
    U64 tmp64;

    if(cnt)
    { //with additional DataStore
souts("DataStore_Setting");
        //1. update TableTbl, some table objects are enabled
        for(i=0;i<G2.b.mLckTbl_Tbl.rowCnt;i++)
        {
            tmp64 = G2.b.mLckTbl_Tbl.val[i].uid;
            tmp32 = ((U32)tmp64) & 0xfffffff0;
//alexdebugds(tmp32,LckTbl&f0);
            if(tmp32==0x1000)
            { //DataStore
                tmp8 = ((U8)G2.b.mLckTbl_Tbl.val[i].uid)-1;

                if(tmp8<cnt)
                {
//soutb(i);
                     G2.b.mLckTbl_Tbl.val[i].uid = UID_Table_DataStore+tmp8;
                     G2.b.mLckTbl_Tbl.val[i].rows = DSTblSize[tmp8];
                }
            }
        }

        //souts("mLckTbl.rowCnt="); soutb(G2.b.mLckTbl_Tbl.rowCnt);

        if(cnt>1)
        {
            //2. update AccessCtrlTbl,  need to enable some rows, no need to update row count.
            for(j=0;j<G2.b.mLckAccessCtrl_Tbl.rowCnt;j++)
            { // Table: DataStoreObj.Get
                if(G2.b.mLckAccessCtrl_Tbl.val[j].invID==UID_Table_DataStore2)
                {
//soutb(0x10);
                    for(k=j;k<(j+cnt-1);k++)
                    {
                        if((G2.b.mLckAccessCtrl_Tbl.val[k].mtdID>>32)!=(UID_MethodID>>32))
                            G2.b.mLckAccessCtrl_Tbl.val[k].mtdID= ~G2.b.mLckAccessCtrl_Tbl.val[k].mtdID;
                    }
                    break;
                }
            }
            for(j=k;j<G2.b.mLckAccessCtrl_Tbl.rowCnt;j++)
            { //ACE_DataStore2_Get_All.Get / ACE_DataStore2_Get_All.Set / ACE_DataStore2_Set_All.Get / ACE_DataStore2_Set_All.Set
                    if(G2.b.mLckAccessCtrl_Tbl.val[j].invID==UID_ACE_DataStore2_Get_All)
                    {
//soutb(0x20);
                        for(k=j;k<(j+(cnt-1)*4);k++)
                        {
                            if((G2.b.mLckAccessCtrl_Tbl.val[k].mtdID>>32)!=(UID_MethodID>>32))
                                G2.b.mLckAccessCtrl_Tbl.val[k].mtdID= ~G2.b.mLckAccessCtrl_Tbl.val[k].mtdID;
                        }
                        break;
                    }
            }
            for(j=k;j<G2.b.mLckAccessCtrl_Tbl.rowCnt;j++)
            { // DataStore2.Get / DataStore2.Set
                if(G2.b.mLckAccessCtrl_Tbl.val[j].invID==UID_DataStore2)
                {
//soutb(0x30);
                    for(k=j;k<(j+(cnt-1)*2);k++)
                    {
                        if((G2.b.mLckAccessCtrl_Tbl.val[k].mtdID>>32)!=(UID_MethodID>>32))
                            G2.b.mLckAccessCtrl_Tbl.val[k].mtdID= ~G2.b.mLckAccessCtrl_Tbl.val[k].mtdID;
                    }
                    break;
                }
            }

            //3. update AceTbl, some ACE objects are enabled
            tmp8 = cnt*2;
            for(i=0;i<G3.b.mLckACE_Tbl.rowCnt;i++)
            {
                tmp64 = G3.b.mLckACE_Tbl.val[i].uid & (~UID_FF);
                tmp32 = (U32)tmp64;
                if((tmp32>0x03FC01)&&tmp32<(0x03FC00+cnt*2))
                { //ACE_DataStoreX_Get_All / ACE_DataStoreX_Set_All
//soutb(i);
                    G3.b.mLckACE_Tbl.val[i].uid = tmp64|UID_ACE;
                }
            }
        }

        DataStoreAddr_Update();
    }
}

void DataStoreAddr_Update(void)
{
    U8  i=0, tblRow;
    U32 offset=0;

    for(tblRow=0; tblRow<G2.b.mLckTbl_Tbl.rowCnt; tblRow++)
    {
        if(G2.b.mLckTbl_Tbl.val[tblRow].uid==UID_Table_DataStore)
            break;
    }

    mDataStoreAddr[0].offset= 0;
    mDataStoreAddr[0].length= G2.b.mLckTbl_Tbl.val[tblRow].rows;
soutb3(0x5C, 0xFE, 0x55); soutb(0); soutd(mDataStoreAddr[0].offset); soutd(mDataStoreAddr[0].length);
    TCG_PRINTF("DS00: %08X %08X\n",mDataStoreAddr[0].offset,mDataStoreAddr[0].length);
    for(i=1;i<DSTBL_MAX_NUM;i++)
    {
        offset += G2.b.mLckTbl_Tbl.val[tblRow+i-1].rows;

        mDataStoreAddr[i].offset=offset;
        mDataStoreAddr[i].length=G2.b.mLckTbl_Tbl.val[tblRow+i].rows;

        if(mDataStoreAddr[i].length)
        {
            soutb3(0x5C, 0xFE, 0x55);
            soutb(i); soutd(mDataStoreAddr[i].offset); soutd(mDataStoreAddr[i].length);
            TCG_PRINTF("DS%02X: %08X %08X\n",i,mDataStoreAddr[i].offset,mDataStoreAddr[i].length);
        }
    }
}

//Trim all blocks and do Block Erase at background
void TrimAndBGC(void){
#ifndef alexcheck
    if (CurrentTrack.all){
        ReqCloseBlock = TRUE;
        WaitSeqencerReady();
    } //Yenheng

    ForceCloseEngSeqencer();
    TrimAllBlk();

    L2PBlockNeedToWrite.bits.Immd = TRUE;//,trig update L2P,Yenheng
    mPmData.CheckDriveState |= PM_L2P_BUSY;
    mPmData.CheckDriveState |= PM_GC_BUSY;
#endif
    return;
}

U16 Method_Revert(void)
{
    U16 result;
    U8 j, tmp[PWD_LENGTH];
    BOOL doBgTrim;

    doBgTrim = FALSE;
    //parameter check
    if(ChkToken()!=TOK_StartList)    //test cases 3.1.5
        return STS_SESSION_ABORT;

    if(ChkToken()!=TOK_EndList)      //test cases 3.1.5
        return STS_SESSION_ABORT;

    if(ChkToken()!=TOK_EndOfData)    //test cases 3.1.5
        return STS_SESSION_ABORT;

    //status list check
    result = ChkMethodStatus();
    if(result!=STS_SUCCESS)
        return result;

    if(mSessionManager.TransactionState==TRNSCTN_ACTIVE)
        return STS_SESSION_ABORT;   //no definition in Test Case!!

soutb3(0x5C, 0xFE, 0x11);
    if(invokingUID.all==UID_SP_Admin)
    {
soutb(0x01);
        TCG_PRINTF("M_Revert: AdminSP\n");

#if _TCG_ == TCG_EDRV
        if(mSessionManager.HtSgnAuthority.all==UID_Authority_PSID)
            mPsidRevertCnt++;   //for WHQL test
#endif

        //TODO:
        //1. This session SHALL be aborted.
        //2. if ATA security feature set is not diabled, then bit 1 in word 82 in IDENTIFY DEVICE SHALL be set to 1,
        //   and bit 1 in word 85 and all bits in word 89, 90, 92 and 128 SHALL be set to appropriate values.
        //3. LockingEnbabled bit in Locking Feature Descriptor in Level 0 SHALL be set to 0. (v)
        //4. LifeCycleState column of the Locking SP object in the SP table, SHALL be set to 0x08. (v)
        //5. a startup of a session to the Locking SP SHALL fail. (v)
        //6. The PIN value of SID object in the C_PIN table is set to the value of MSID credential.
        //7. If the Locking SP is in Manufactured state:
        //      a. All the data in the User LBA Range SHALL be cryptographically erased.
        //      b. All the values in DataStore table SHALL be the value in its OFS.
        //      c. All the values in MBR table SHALL be the value in its OFS.
        //8. If the Locking SP is in Manufactured-Inactive state:
        //      All the data in user LBA range SHALL NOT change.
        //?  The entire TPer SHALL revert to its OFS.

        //copy MSID to SID ... (6)

        if(G1.b.mAdmSP_Tbl.val[SP_LOCKING_IDX].lifeCycle==manufactured)
        {
            //G1.b.mAdmSP_Tbl.val[SP_LOCKING_IDX].lifeCycle = manufactured_inactive;    // (3) (4)

            //clear MBR table  (7c)
            TcgH2cJobIssue(MSG_TCG_SMBRCLEAR, NULL);   //WaitMbrClr();

#if 0       //Revert Admin SP tables:
//PSID[
            //1. save PSID password
            for(y=0;y<G1.b.mAdmCPin_Tbl.rowCnt;y++){
                if(G1.b.mAdmCPin_Tbl.val[y].uid==UID_CPIN_PSID){
                    for(j=0; j<PWD_LENGTH; j++)
                        tmp[j] = G1.b.mAdmCPin_Tbl.val[y].pwd[j];
                    break;
                }
            }
//]
            //2. copy cTbl to mTbl
            cTbl_2_mTbl_byGrp(GRP_1);
//PSID[
            //3. restore PSID password
            for(j=0; j<PWD_LENGTH; j++)
               G1.b.mAdmCPin_Tbl.val[y].pwd[j] = tmp[j];
//]
#endif
            //Revert Locking SP tables (?)
            TcgH2cJobIssue(MSG_TCG_G2RDDEFAULT, NULL);   //cTbl_2_mTbl_byGrp(GRP_2);
            TcgH2cJobIssue(MSG_TCG_G2WR, NULL);          //WaitG2Wr();

            TcgH2cJobIssue(MSG_TCG_G3RDDEFAULT, NULL);   //cTbl_2_mTbl_byGrp(GRP_3);

            Start_DataStoreLaa=0;
            End_DataStoreLaa=DATASTORE_LEN/LAA_LEN;

            TcgH2cJobIssue(MSG_TCG_DSCLEAR, NULL);   //WaitDSClr();    //clear DataStore table (8)

            for(j=0;j<=LOCKING_RANGE_CNT;j++)
                ChangeKey(j); //Erase Key (7a)

            TcgH2cJobIssue(MSG_TCG_G3WR, NULL);   //WaitG3Wr();

            mTcgStatus = 0;
            LockingRangeTable_Update();

            //update keys to Key RAM and NAND, no need to check the Transaction State for Revert
            //InitAesKey4Tcg(TRUE);
            TcgResetCache(MSG_TCG_REGEN_ZEROCACHE);

            //TrimAndBGC();   //cj added
            doBgTrim = TRUE;
        }

#if 1   //Revert Admin SP tables:
//PSID[
        //1. save PSID password
        for(j=0; j<PWD_LENGTH; j++)
            tmp[j] = G1.b.mAdmCPin_Tbl.val[CPIN_PSID_IDX].pwd[j];
//]
        //2. copy cTbl to mTbl
        TcgH2cJobIssue(MSG_TCG_G1RDDEFAULT, NULL);       //cTbl_2_mTbl_byGrp(GRP_1);
//PSID[
        //3. restore PSID password
        for(j=0; j<PWD_LENGTH; j++)
           G1.b.mAdmCPin_Tbl.val[CPIN_PSID_IDX].pwd[j] = tmp[j];
//]
#endif
        CopyHashed(&G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[1], G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[0], G1.b.mAdmCPin_Tbl.val[CPIN_SID_IDX].pwd);     // MSID 2 SID
        TcgH2cJobIssue(MSG_TCG_G1WR, NULL);   //WaitG1Wr();

        ResetSessionManager();  //D10-1-1-1-1

#if 0 //(OEM == LENOVO) || (OEM == Lenovo) || (OEM == lenovo)
        G1.b.mAdmCPin_Tbl.val[0].tryLimit = 7;  // CPIN  ->  SID  ->  TRY LIMIT change from 5 to 7 for Lenovo [BID22959]
#endif
        //WaitDmyWr(6);  // write 6 dummy pages for open block

#if CO_SUPPORT_BLOCK_SID_AUTH
        mTcgStatus &= ~(SID_BLOCKED + SID_HW_RESET);
#endif
        if(doBgTrim){
            TrimAndBGC();   //cj added
            doBgTrim = FALSE;
        }
        result=STS_SUCCESS;     // STS_SESSION_ABORT;
    }
    else if(invokingUID.all==UID_SP_Locking)
    {
soutb(0x02);
        TCG_PRINTF("M_Revert: LockingSP\n");
        //TODO:
        //1. This session remains open.
        //2. if ATA security feature set is not diabled, then bit 1 in word 82 in IDENTIFY DEVICE SHALL be set to 1,
        //   and bit 1 in word 85 and all bits in word 89, 90, 92 and 128 SHALL be set to appropriate values.
        //3. LockingEnbabled bit in Locking Feature Descriptor in Level 0 SHALL be set to 0. (v)
        //4. LifeCycleState column of the Locking SP object in the SP table, SHALL be set to 0x08. (v)
        //5. a startup of a session to the Locking SP SHALL fail. (v)
        //6. x
        //7. If the Locking SP is in Manufactured state:
        //      a. All the data in the User LBA Range SHALL be cryptographically erased.
        //      b. All the values in DataStore table SHALL be the value in its OFS.
        //      c. All the values in MBR table SHALL be the value in its OFS.
        //8. If the Locking SP is in Manufactured-Inactive state:
        //      a. All the data in user LBA range SHALL NOT change.
        //      b. All the values in DataStore table SHALL be the value in its OFS.
        //      c. All the values in the MBR table SHALL be the value in its OFS.
        //?  The SP itself SHALL revert to its OFS.

        if(G1.b.mAdmSP_Tbl.val[SP_LOCKING_IDX].lifeCycle==manufactured)
        {
            G1.b.mAdmSP_Tbl.val[SP_LOCKING_IDX].lifeCycle = manufactured_inactive;    // (3) (4)
            TcgH2cJobIssue(MSG_TCG_G1WR, NULL);   //WaitG1Wr();

            //clear MBR table  (7c)
            TcgH2cJobIssue(MSG_TCG_SMBRCLEAR, NULL);   //WaitMbrClr();

            //Revert Locking SP tables (?)
            souts("w2/w3");
            TcgH2cJobIssue(MSG_TCG_G2RDDEFAULT, NULL);   //cTbl_2_mTbl_byGrp(GRP_2);
            TcgH2cJobIssue(MSG_TCG_G2WR, NULL);          //WaitG2Wr();

            TcgH2cJobIssue(MSG_TCG_G3RDDEFAULT, NULL);   //cTbl_2_mTbl_byGrp(GRP_3);
            Start_DataStoreLaa=0;
            End_DataStoreLaa=DATASTORE_LEN/LAA_LEN;

            TcgH2cJobIssue(MSG_TCG_DSCLEAR, NULL);   //WaitDSClr();     //clear DataStore table (7b)

            for(j=0;j<=LOCKING_RANGE_CNT;j++)
                ChangeKey(j); //Erase Keys (7a)

            TcgH2cJobIssue(MSG_TCG_G3WR, NULL);   //WaitG3Wr();

            mTcgStatus = 0;
            LockingRangeTable_Update();
            //update keys to Key RAM and NAND, no need to check the Transaction State for Revert
            //InitAesKey4Tcg(TRUE);
            TcgResetCache(MSG_TCG_REGEN_ZEROCACHE);

            //WaitDmyWr(6);  // write 6 dummy pages for open block

            TrimAndBGC();   //cj added

        }

#ifdef _TCG_LAB_RESCUE_ONLY
        {
            // to Lock any rd & wr in rescue mode
            U8 i;
            for(i=0; i<=LOCKING_RANGE_CNT; i++){
                mLockingRangeTable[i].writeLocked=1;
                mLockingRangeTable[i].readLocked=1;
            }
            soutb3(0x5C, 0xFE, 0x46); //!!Watch out, Lock any write and read in _TCG_LAB_RESCUE_ONLY
        }
#endif

        result=STS_SUCCESS;
    }
    else
        result=STS_INVALID_PARAMETER;

    dataBuf[iDataBuf++]=TOK_StartList;
    dataBuf[iDataBuf++]=TOK_EndList;
    dataBuf[iDataBuf++]=TOK_EndOfData;

    return result;

}

//Test Cases D8: how to deal with "Transaction"?
U16 Method_GenKey(void)
{
    U16 result;
    U8 keyNo=0xff;

    //parameter check
    if(ChkToken()!=TOK_StartList)    //test cases 3.1.5
        return STS_SESSION_ABORT;

    if(ChkToken()!=TOK_EndList)      //test cases 3.1.5
        return STS_SESSION_ABORT;

    if(ChkToken()!=TOK_EndOfData)    //test cases 3.1.5
        return STS_SESSION_ABORT;

    //status list check
    result = ChkMethodStatus();
    if(result!=STS_SUCCESS)
        return result;


    //check the key range (invoking UID) and update key
    switch (invokingUID.all)
    {
#ifdef TCG_KAES_128
        case UID_K_AES_128_GRange_Key:
            keyNo = 0;
            break;
        case UID_K_AES_128_Range1_Key:
            keyNo = 1;
            break;
        case UID_K_AES_128_Range2_Key:
            keyNo = 2;
            break;
        case UID_K_AES_128_Range3_Key:
            keyNo = 3;
            break;
        case UID_K_AES_128_Range4_Key:
            keyNo = 4;
            break;
#else
        case UID_K_AES_256_GRange_Key:
            keyNo = 0;
            break;
        case UID_K_AES_256_Range1_Key:
            keyNo = 1;
            break;
        case UID_K_AES_256_Range2_Key:
            keyNo = 2;
            break;
        case UID_K_AES_256_Range3_Key:
            keyNo = 3;
            break;
        case UID_K_AES_256_Range4_Key:
            keyNo = 4;
            break;
        case UID_K_AES_256_Range5_Key:
            keyNo = 5;
            break;
        case UID_K_AES_256_Range6_Key:
            keyNo = 6;
            break;
        case UID_K_AES_256_Range7_Key:
            keyNo = 7;
            break;
        case UID_K_AES_256_Range8_Key:
            keyNo = 8;
            break;
#endif
    }

    if(keyNo<=LOCKING_RANGE_CNT){
TCG_PRINTF("M_GKey: %X\n", keyNo);
        ChangeKey(keyNo);
        //update keys to Key RAM and NAND
        if(mSessionManager.TransactionState==TRNSCTN_IDLE)
        {
            U32 ii;
            TcgH2cJobIssue(MSG_TCG_G3WR, NULL);   //WaitG3Wr();
            //UpdateAesKey4Tcg(keyNo);
            #if 0
            for(ii=0; ii<=LOCKING_RANGE_CNT; ii++){
                if(mLockingRangeTable[ii].rangeNo==keyNo) {
                    HAL_SEC_SetAesKey(ii, G3.b.mLckKAES_256_Tbl.val[keyNo].key);
                    //HAL_SEC_SetAesRange();
                    break;
                }
            }
            #else
            HAL_SEC_InitAesKeyRng();
            //HAL_SEC_SetAesKey(0, G3.b.mLckKAES_256_Tbl.val[1].key);
            //HAL_SEC_SetAesKey(1, G3.b.mLckKAES_256_Tbl.val[0].key);
            #endif

            TcgResetCache(MSG_TCG_REGEN_ZEROCACHE);
        }
        else
        {
            G3MtableChanged = TRUE;
            bKeyChanged = TRUE;
        }
        result = STS_SUCCESS;
    }
    else
        result = STS_INVALID_PARAMETER;

    dataBuf[iDataBuf++]=TOK_StartList;
    dataBuf[iDataBuf++]=TOK_EndList;
    dataBuf[iDataBuf++]=TOK_EndOfData;

    //TODO: inform the kernel to update AES key setting if not in Transaction state
    return result;
}

//LockingSP.RevertSP()
//Check Test Cases D10-3-1-1-1
extern void Nvme_Security_FlushAll(void);
U16 Method_RevertSP(void)
{
    U16 result;
    U32 nameValue=0x00;
    U8  data, keepGlobalRangeKey=0x00, j;
    U32 key[8];
    //U32 len;

    //TODO:
    //0. If 'KeepGlobalRangeKey' is 1, and Locking GlobalRange is both read-locked and write-locked, fail with FAIL.
    //1. This session SHALL be aborted.
    //2. if ATA security feature set is not diabled, then bit 1 in word 82 in IDENTIFY DEVICE SHALL be set to 1,
    //   and bit 1 in word 85 and all bits in word 89, 90, 92 and 128 SHALL be set to appropriate values.
    //3. LockingEnbabled bit in Locking Feature Descriptor in Level 0 SHALL be set to 0. (v)
    //4. LifeCycleState column of the Locking SP object in the SP table, SHALL be set to 0x08. (v)
    //5. a startup of a session to the Locking SP SHALL fail. (v)
    //6. x
    //7. except GlobalRange (if 'KeepGlobalRangeKey'=1), all of the user ranges SHALL be cryptographically erased.
    //8. Whichever 'KeepGlobalRangeKey' is, all the values in DataStore table SHALL be the value in its OFS.
    //9. Whichever 'KeepGlobalRangeKey' is, all the values in MBR table SHALL be the value in its OFS.
    //?  The SP itself SHALL revert to its OFS.


    //parameter check
    if(ChkToken()!=TOK_StartList)    //test cases 3.1.5
        return STS_SESSION_ABORT;


    //retrieve parameter 'KeepGlobalRangeKey'
    data = ChkToken();

    if(data==TOK_StartName)
    {
        if(AtomDecoding_Uint32(&nameValue)!=STS_SUCCESS)
            return STS_INVALID_PARAMETER;

        if(nameValue==0x060000) //
        {
            if(AtomDecoding_Uint8(&keepGlobalRangeKey)!=STS_SUCCESS)
                return STS_INVALID_PARAMETER;

            if((keepGlobalRangeKey!=1)&&(keepGlobalRangeKey!=0))
                return STS_INVALID_PARAMETER;

            if(keepGlobalRangeKey==1)
            {
                if(G3.b.mLckLocking_Tbl.val[0].readLocked && G3.b.mLckLocking_Tbl.val[0].writeLocked)
                    return STS_FAIL;    //(0)

                Nvme_Security_FlushAll();
            }
        }

        if(ChkToken()!=TOK_EndName)
            return STS_INVALID_PARAMETER;

        data = ChkToken();
    }

    if(data!=TOK_EndList)      //test cases 3.1.5
        return STS_SESSION_ABORT;

    if(ChkToken()!=TOK_EndOfData)    //test cases 3.1.5
        return STS_SESSION_ABORT;

    //status list check
    result = ChkMethodStatus();
    if(result!=STS_SUCCESS)
        return result;

    if(mSessionManager.TransactionState==TRNSCTN_ACTIVE)
        return STS_SESSION_ABORT;   //no definition in Test Case!!

soutb3(0x5C, 0xFE, 0x12); soutb(keepGlobalRangeKey);
    TCG_PRINTF("M_RevertSP: %X\n", keepGlobalRangeKey);

    G1.b.mAdmSP_Tbl.val[SP_LOCKING_IDX].lifeCycle = manufactured_inactive;    // (3) (4)
    TcgH2cJobIssue(MSG_TCG_G1WR, NULL);   //WaitG1Wr();

    //clear MBR table  (9)
    TcgH2cJobIssue(MSG_TCG_SMBRCLEAR, NULL);   //WaitMbrClr();

    //Revert Locking SP tables (?)
souts("w2/w3");
    TcgH2cJobIssue(MSG_TCG_G2RDDEFAULT, NULL);   //cTbl_2_mTbl_byGrp(GRP_2);
    TcgH2cJobIssue(MSG_TCG_G2WR, NULL);          //WaitG2Wr();

    if(keepGlobalRangeKey==1)
    { //backup key 0
        for(j=0;j<8;j++)
#ifdef TCG_KAES_128
            key[j] = mLckKAES_128_Tbl.val[0].key[j];
#else
            key[j] = G3.b.mLckKAES_256_Tbl.val[0].key[j];
#endif
    }
    TcgH2cJobIssue(MSG_TCG_G3RDDEFAULT, NULL);   //cTbl_2_mTbl_byGrp(GRP_3);
    Start_DataStoreLaa=0;
    End_DataStoreLaa=DATASTORE_LEN/LAA_LEN;

    TcgH2cJobIssue(MSG_TCG_DSCLEAR, NULL);   //WaitDSClr();    //clear DataStore table (8)

    //Erase Keys (7)
    if(keepGlobalRangeKey!=1)
        ChangeKey(0);
    else
    { //restore key 0
        for(j=0;j<8;j++)
#ifdef TCG_KAES_128
            mLckKAES_128_Tbl.val[0].key[j] = key[j];
#else
            G3.b.mLckKAES_256_Tbl.val[0].key[j] = key[j];
#endif
    }

    for(j=1;j<=LOCKING_RANGE_CNT;j++)
        ChangeKey(j);

    TcgH2cJobIssue(MSG_TCG_G3WR, NULL);   //WaitG3Wr();
    //WaitDmyWr(6);  // write 6 dummy pages for open block

    mTcgStatus = 0;
    LockingRangeTable_Update();
    TcgResetCache(MSG_TCG_REGEN_ZEROCACHE);

    if(keepGlobalRangeKey!=1) //all keys are reset, do Block Erase in background
        TrimAndBGC();

    //dataBuf[iDataBuf++]=TOK_StartList;
    //dataBuf[iDataBuf++]=TOK_EndList;
    //dataBuf[iDataBuf++]=TOK_EndOfData;

#ifdef _TCG_LAB_RESCUE_ONLY
    {
        // to Lock any rd & wr in rescue mode
        U8 i;
        for(i=0; i<=LOCKING_RANGE_CNT; i++){
            mLockingRangeTable[i].writeLocked=1;
            mLockingRangeTable[i].readLocked=1;
        }
        soutb3(0x5C, 0xFE, 0x46); //!!Watch out, Lock any write and read in _TCG_LAB_RESCUE_ONLY
    }
#endif

    return STS_SUCCESS_THEN_ABORT; //??? STS_SESSION_ABORT;   //D10-3-3-1-1
}

//Generate a new key, and update to G3.b.mLckKAES_256_Tbl[]
void ChangeKey(U8 rangeNo)
{
#ifdef TCG_KAES_128
    HAL_SEC_CollectEntropySamples(G3.b.mLckKAES_128_Tbl.val[rangeNo].key,8);
#else
    HAL_SEC_CollectEntropySamples(G3.b.mLckKAES_256_Tbl.val[rangeNo].key,8);
#endif
}

//Generate and update Global AES key.
//This function is called by normal code only (ATA_Security or Sanitize command)
void TcgUpdateGlobalKey(void)
{
    U32 i;

#ifdef TCG_KAES_128
    HAL_SEC_CollectEntropySamples(G3.b.mLckKAES_128_Tbl.val[0].key,8);
#else
    HAL_SEC_CollectEntropySamples(G3.b.mLckKAES_256_Tbl.val[0].key,8);  //Global Key
#endif

    TcgH2cJobIssue(MSG_TCG_G3WR, NULL);
    for(i=0; i<=LOCKING_RANGE_CNT; i++)
    {
        if(mLockingRangeTable[i].rangeNo==0) {
            HAL_SEC_SetAesKey(i, G3.b.mLckKAES_256_Tbl.val[0].key);
            break;
        }
    }

    TcgResetCache(MSG_TCG_REGEN_ZEROCACHE);
}


//ThisSP.Authenticate()
U16 Method_Authenticate(void)
{
    U16 result;
    U8  byte,j;
    UID64 authority;
    U64 auth0=UID_Null;
    U32 len=0;

    //parameter check
    if(ChkToken()!=TOK_StartList)    //test cases 3.1.5
        return STS_SESSION_ABORT;

    //retrieve parameter 'Authority'
    if(AtomDecoding_Uid2(authority.bytes)!=STS_SUCCESS)
        return STS_INVALID_PARAMETER;

    byte = ChkToken();
    if(byte==TOK_StartName)
    {
        if(AtomDecoding_Uint8(&byte)!=STS_SUCCESS)
            return STS_INVALID_PARAMETER;

        if(byte==0x00)
        {// param: Proof (HostChallenge)
            result=AtomDecoding(ATOM_BYTES,&len);
            if(result==STS_SUCCESS)
            {
                //mSessionManager.HtChallenge[0]=LoByte(LoWord(len));
                //for (j=0;j<len;j++)
                //    mSessionManager.HtChallenge[j+1]=DecBuf[j];
                if(len>CPIN_LENGTH)
                    return STS_INVALID_PARAMETER;

                byte =ChkToken();
            }
        }

        if(byte!=TOK_EndName)
            return STS_INVALID_PARAMETER;

        byte =ChkToken();
    }

    if(byte==TOK_EndList)
        goto END_LIST;
    else  //NG within parameters
        return STS_SESSION_ABORT;   //A6-1-5-2-1, STS_INVALID_PARAMETER;

END_LIST:
    if(ChkToken()!=TOK_EndOfData)
        return STS_STAY_IN_IF_SEND;

    //status list check
    result = ChkMethodStatus();
    if(result!=STS_SUCCESS)
        return result;

//soutb3(0x5C, 0xFE, 0x51); soutd(authority.all);

    //method execution:
    // only suuport "Anybody + one authority" in a session
    if (authority.all==UID_Authority_Anybody)
        result = STS_SUCCESS;
    else
    { //authority!=anybody
    #if 1 //check if authority is found and not "IsClass"
        if(mSessionManager.SPID.all==UID_SP_Admin)
        {
            for(j=0;j<G1.b.mAdmAuthority_Tbl.rowCnt;j++)
            { // row# (4) can be acquired from Table tbl;
                if(authority.all==G1.b.mAdmAuthority_Tbl.val[j].uid)
                {
                    if(G1.b.mAdmAuthority_Tbl.val[j].isClass==TRUE)
                        return STS_INVALID_PARAMETER;   //UID is a class
                    else
                        goto CHECK_SIGN;
                }
            }
            return STS_INVALID_PARAMETER;   //no authority found!
        }
        else
        {
            for(j=0;j<G3.b.mLckAuthority_Tbl.rowCnt;j++)
            { // row# (4) can be acquired from Table tbl;
                if(authority.all==G3.b.mLckAuthority_Tbl.val[j].uid)
                {
                    if(G3.b.mLckAuthority_Tbl.val[j].isClass==TRUE)
                        return STS_INVALID_PARAMETER;   //UID is a class
                    else
                        goto CHECK_SIGN;
                }
            }
            return STS_INVALID_PARAMETER;   //no authority found!
        }
    CHECK_SIGN:
    #endif
        auth0 = mSessionManager.HtSgnAuthority.all;     //backup original authority
#if 1 //temperarily marked off for eDrive test ...
        if((auth0 != UID_Authority_Anybody) && (auth0 != authority.all))
            result = STS_NOT_AUTHORIZED;    //return STS_SUCCESS with result=FALSE
        else
#endif
#if (CO_SUPPORT_PSID==FALSE)  // PSID is not supported!!
        if(authority.all==UID_Authority_PSID)
            result = STS_NOT_AUTHORIZED;
        else
#endif
#if CO_SUPPORT_BLOCK_SID_AUTH
        if((mTcgStatus&SID_BLOCKED)&&(authority.all==UID_Authority_SID))
            result = STS_NOT_AUTHORIZED;
        else
#endif
        {
            mSessionManager.HtSgnAuthority.all = authority.all;
            mSessionManager.HtChallenge[0]=LoByte(LoWord(len));
            for (j=0;j<len;j++)
                mSessionManager.HtChallenge[j+1]=DecBuf[j];
            result = HostSigningAuthorityCheck();
        }
    }

    dataBuf[iDataBuf++]=TOK_StartList;

soutb3(0x5C, 0xFE, 0x51); soutd(authority.all); soutw(result);
    TCG_PRINTF("M_Authenticate: %08X %04X\n",(U32)authority.all,result);

    if(result==STS_SUCCESS)
        dataBuf[iDataBuf++]=0x01;   //TRUE;
    else
    { // failed, restore authority
        mSessionManager.HtSgnAuthority.all = auth0;
        dataBuf[iDataBuf++]=0x00;   //FALSE;
    }
    dataBuf[iDataBuf++]=TOK_EndList;
    dataBuf[iDataBuf++]=TOK_EndOfData;

    return STS_SUCCESS;
}

U16 Method_Random(void)
{
    U16 result;
    U32 count, i;

    //parameter check
    if(ChkToken()!=TOK_StartList)    //test cases 3.1.5
        return STS_SESSION_ABORT;

    //retrieve parameter 'Count'
    if(AtomDecoding_Uint32(&count)!=STS_SUCCESS)
        return STS_INVALID_PARAMETER;

    if(count > 0x20){                   // OPAL spec 4.2.6.1 , The TPer SHALL support Count parameter values less than or equal to 32.
        return STS_INVALID_PARAMETER;
    }

    if(count>(TCG_BUF_LEN-iDataBuf-11))     //buffersize limitation: F0 82 xx yy F1 F9 F0 00 00 00 F1, iDataBuf=0x38
        return STS_RESPONSE_OVERFLOW;

    if(ChkToken()!=TOK_EndList)      //test cases 3.1.5
        return STS_SESSION_ABORT;

    if(ChkToken()!=TOK_EndOfData)  //test cases 3.1.5
        return STS_SESSION_ABORT;

    //status list check
    result = ChkMethodStatus();
    if(result!=STS_SUCCESS)
        return result;

    if(mSessionManager.TransactionState==TRNSCTN_ACTIVE)
        return STS_SESSION_ABORT;   //no definition in Test Case!!

soutb3(0x5C, 0xFE, 0x50); soutb(count);
    TCG_PRINTF("M_RND: %X\n",count);

    dataBuf[iDataBuf++]=TOK_StartList;

    //TODO: RNG
    HAL_SEC_CollectEntropySamples((U32 *)DecBuf,(count+3)>>2);

    AtomEncoding_Byte(count);

    for(i=0;i<count;i++)
        dataBuf[iDataBuf++]=DecBuf[i];

    dataBuf[iDataBuf++]=TOK_EndList;
    dataBuf[iDataBuf++]=TOK_EndOfData;

    return STS_SUCCESS;
}


U16 Method_Reactivate(void)
{
    U16 result=0, idx;
    U8 i,j;
    U64 tmp64;
    U8  tmp8, dsCnt=0, bAdmin1PIN=0, errCode=0;
    U32 nameValue, tmp32, DSTblSize[DSTBL_MAX_NUM], totalsize=0;

    U8  sgUserCnt=0;
    U8  sgUserPolicy=1;    //admin has ownership
    U16 sgUserRange=0;
    //U8  admin1Pin[33];

    //parameter check
    if(ChkToken()!=TOK_StartList)    //test cases 3.1.5
    {   errCode=0x10;  result=STS_SESSION_ABORT;  goto MTD_EXIT;    }

    //retrieve parameter 'DataStoreTblSizes'
    tmp8 = ChkToken();
    if(tmp8==TOK_StartName)
    {
        if(AtomDecoding_Uint32(&nameValue)!=STS_SUCCESS)
        {   errCode=0x20;  result=STS_INVALID_PARAMETER;  goto MTD_EXIT;    }

        if(nameValue==0x060000) //'SingleUserSelectionList'
        {
            //mSgUser.cnt=0;
            //mSgUser.policy=0;
            //mSgUser.range=0;

            if(ChkToken()==TOK_StartList)
            { // Locking table objects list
                tmp8=0;
                while(ChkToken()!=TOK_EndList)
                {
                    iPload--;

                    if(AtomDecoding_Uid2((U8 *)&tmp64)!=STS_SUCCESS)
                    {   errCode=0x30;  result=STS_INVALID_PARAMETER;  goto MTD_EXIT;    }

                    //check if it is in the locking table
                    for(i=tmp8;i<G3.b.mLckLocking_Tbl.rowCnt;i++)
                    {
                        if(G3.b.mLckLocking_Tbl.val[i].uid==tmp64)
                        {
                            sgUserRange |= (0x01<<i);
                            sgUserCnt++;
                            tmp8 = i;
                            alexdebug2s(sgUserCnt, i, LckTblObj);
                            break;
                        }
                    }

                    if(i>=G3.b.mLckLocking_Tbl.rowCnt)
                    {   errCode=0x31;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }
                }
            }
            else
            { // check if entire Locking table
                iPload--;
                if(AtomDecoding_Uid2((U8 *)&tmp64)!=STS_SUCCESS)
                {    errCode=0x32;  result=STS_STAY_IN_IF_SEND;    goto MTD_EXIT; }

                if(tmp64!=UID_Locking)
                {    errCode=0x33;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

                sgUserCnt=LOCKING_RANGE_CNT+1;
                sgUserRange=0xffff;   //EntireLocking
                alexdebug1s(sgUserCnt,EntireLocking);
            }

            if(ChkToken()!=TOK_EndName)
            {   errCode=0x34;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

            if((tmp8=ChkToken())!=TOK_StartName)
                goto CHK_ENDLIST;

            if(AtomDecoding_Uint32(&nameValue)!=STS_SUCCESS)
            {    errCode=0x35;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }
        }

        if(sgUserCnt)
            sgUserPolicy = 0;   //User has ownership

        if(nameValue==0x060001) //'RangePolicy'
        {
            if(AtomDecoding_Uint8(&tmp8)!=STS_SUCCESS)
            {   errCode=0x38;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

            if(tmp8==0)
            {
                if(sgUserCnt!=0) sgUserPolicy=0;
            }
            else if(tmp8==1)
                sgUserPolicy=1;
            else
            {    errCode=0x39;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }
                alexdebug1s(sgUserPolicy,policy);

            if(ChkToken()!=TOK_EndName)
            {   errCode=0x3A;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

            if((tmp8=ChkToken())!=TOK_StartName)
                goto CHK_ENDLIST;

            if(AtomDecoding_Uint32(&nameValue)!=STS_SUCCESS)
            {    errCode=0x3B;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

        }

        if(nameValue==0x060002) //'Admin1PIN'
        {
            //if(AtomDecoding_Uint8(&tmp8)!=STS_SUCCESS)
            //{   errCode=0x3C;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

            result=AtomDecoding(ATOM_BYTES,&tmp32);
            if(result==STS_SUCCESS)
            {
                //if(len>CPIN_LENGTH)
                //{
                //    result = STS_INVALID_PARAMETER;       //need to check ...
                //    goto SYNC_REPLY;
                //}
                bAdmin1PIN=1;
                mSessionManager.HtChallenge[0]=LoByte(LoWord(tmp32));
                for (i=0;i<tmp32;i++)
                    mSessionManager.HtChallenge[i+1]=DecBuf[i];
            }
            else
            {   errCode=0x3D;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

            if(ChkToken()!=TOK_EndName)
            {   errCode=0x3E;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

            if((tmp8=ChkToken())!=TOK_StartName)
                goto CHK_ENDLIST;

            if(AtomDecoding_Uint32(&nameValue)!=STS_SUCCESS)
            {   errCode=0x3F;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }
        }


        //check Additional DataStore Parameter
        if(nameValue==0x060003) //'DataStoreTblSizes'
        {
            if(ChkToken()!=TOK_StartList)
            {   errCode=0x21;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

            while(ChkToken()!=TOK_EndList)
            {
                iPload--;

                if(AtomDecoding_Uint32(&tmp32)!=STS_SUCCESS)
                {   errCode=0x22;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

                if(dsCnt>=DSTBL_MAX_NUM)      //too many tables
                {   errCode=0x23;  result=STS_INSUFFICIENT_SPACE;    goto MTD_EXIT; }

                totalsize += tmp32;
                if(totalsize>DATASTORE_LEN) //size is too large
                {   errCode=0x24;  result=STS_INSUFFICIENT_SPACE;    goto MTD_EXIT; }

                if(tmp32%DSTBL_ALIGNMENT)   //not aligned
                {
                    souts("not aligned!");
                    errCode=0x25;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

                DSTblSize[dsCnt]=tmp32;
                alexdebugds(tmp32,DSTblSize);
                dsCnt++;
            }

            if(ChkToken()!=TOK_EndName)
            {    errCode=0x26;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }

            tmp8 = ChkToken();
        }
        else
        {   errCode=0x27;  result=STS_INVALID_PARAMETER;    goto MTD_EXIT; }
    }

CHK_ENDLIST:
    if(tmp8!=TOK_EndList)      //test cases 3.1.5
    {    errCode=0x15;  result=STS_SESSION_ABORT;    goto MTD_EXIT; }

    if(ChkToken()!=TOK_EndOfData)    //test cases 3.1.5
    {    errCode=0x16;  result=STS_SESSION_ABORT;    goto MTD_EXIT; }

    //status list check
    result = ChkMethodStatus();
    if(result!=STS_SUCCESS)
    {   errCode=0x17;  goto MTD_EXIT; }

    if(mSessionManager.TransactionState==TRNSCTN_ACTIVE)
    {   errCode=0x18;   result=STS_SESSION_ABORT;   goto MTD_EXIT;  } //no definition in Test Case!!


    for(i=0;i<=LOCKING_RANGE_CNT;i++)
    { // return FAIL if any of the locking ranges is enabled
        if(G3.b.mLckLocking_Tbl.val[i].readLockEnabled || G3.b.mLckLocking_Tbl.val[i].writeLockEnabled)
        {   errCode=0x19;   result=STS_FAIL;    goto MTD_EXIT;  }
    }

    //Reactivate processing...

    //restore to OFS with some exceptions... C_PIN_Admin1.PIN, RangeStart/RangeLength, K_AES
    //save those data to DecBuf temporarily
    idx=0;

    if(bAdmin1PIN)
    {
        souts("update new CPIN_Admin1");

        //sha256(&mSessionManager.HtChallenge[1], mSessionManager.HtChallenge[0], digest);
        //G3.b.mLckCPin_Tbl.val[0].pwd[0]=mSessionManager.HtChallenge[0];
        //for(i=0; i<SHA256_DIGEST_SIZE; i++)
        //    G3.b.mLckCPin_Tbl.val[0].pwd[i+1]=digest[i];

        CopyHashed(&mSessionManager.HtChallenge[1], mSessionManager.HtChallenge[0], G3.b.mLckCPin_Tbl.val[LCK_CPIN_ADMIN1_IDX].pwd);
    }

//souts("save CPIN_Admin1");
    for(j=0;j<PWD_LENGTH;j++)        //
        DecBuf[idx++]= *((U8 *)&G3.b.mLckCPin_Tbl.val[0].pwd + j);

//souts("save KAES");
     for(i=0;i<=LOCKING_RANGE_CNT;i++)
    {
        alexdebug1s(i, KAES);
        for(j=0;j<(sizeof(G3.b.mLckKAES_256_Tbl.val[0].key[0])*8);j++)
        {
            //DecBuf[idx++]= *((U8 *)&mLckKAES_128_Tbl.val[i].key[0] + j);
            DecBuf[idx]= *((U8 *)&G3.b.mLckKAES_256_Tbl.val[i].key[0] + j);
//soutb(DecBuf[idx]);
            idx++;
        }
    }
//souts("save Range");
    for(i=0;i<=LOCKING_RANGE_CNT;i++)
    {
        for(j=0;j<sizeof(G3.b.mLckLocking_Tbl.val[0].rangeStart);j++)
            DecBuf[idx++]= *((U8 *)&G3.b.mLckLocking_Tbl.val[i].rangeStart + j);

        for(j=0;j<sizeof(G3.b.mLckLocking_Tbl.val[0].rangeLength);j++)
            DecBuf[idx++]= *((U8 *)&G3.b.mLckLocking_Tbl.val[i].rangeLength + j);
    }

    //restore mTbl to OFS
    TcgH2cJobIssue(MSG_TCG_G2RDDEFAULT, NULL);   //cTbl_2_mTbl_byGrp(GRP_2);
    TcgH2cJobIssue(MSG_TCG_G3RDDEFAULT, NULL);   //cTbl_2_mTbl_byGrp(GRP_3);

    idx=0;
    for(i=0;i<PWD_LENGTH;i++)
        G3.b.mLckCPin_Tbl.val[0].pwd[i] = DecBuf[idx++];  //copy Admin1PIN to Admin1 pin

    for(i=0;i<=LOCKING_RANGE_CNT;i++)
        for(j=0;j<(sizeof(G3.b.mLckKAES_256_Tbl.val[0].key[0])*8);j++)
            *((U8 *)&G3.b.mLckKAES_256_Tbl.val[i].key[0] + j) = DecBuf[idx++];
    for(i=0;i<=LOCKING_RANGE_CNT;i++)
    {
        for(j=0;j<sizeof(G3.b.mLckLocking_Tbl.val[0].rangeStart);j++)
            *((U8 *)&G3.b.mLckLocking_Tbl.val[i].rangeStart + j) = DecBuf[idx++];

        for(j=0;j<sizeof(G3.b.mLckLocking_Tbl.val[0].rangeLength);j++)
            *((U8 *)&G3.b.mLckLocking_Tbl.val[i].rangeLength + j) = DecBuf[idx++];
    }

    mSgUser.cnt=sgUserCnt;
    mSgUser.policy=sgUserPolicy;
    mSgUser.range=sgUserRange;

    TCG_PRINTF("M_ReAct: DSCnt=%02X, SURx=%04X\n", dsCnt, mSgUser.range);

    DataStore_Setting(dsCnt, DSTblSize);    //update group2
    SingleUser_Setting();                   //update group2/group3

//souts("w2/w3");
    TcgH2cJobIssue(MSG_TCG_G2WR, NULL);   //WaitG2Wr();
    TcgH2cJobIssue(MSG_TCG_G3WR, NULL);   //WaitG3Wr();

    //cj: TcgStatus should keep the same, mTcgStatus = 0;
    LockingRangeTable_Update();

    //dataBuf[iDataBuf++]=TOK_StartList;
    //dataBuf[iDataBuf++]=TOK_EndList;
    //dataBuf[iDataBuf++]=TOK_EndOfData;

    result = STS_SUCCESS_THEN_ABORT;

MTD_EXIT:
    soutb3(0x5C, 0xFE, 0x56);  soutb3(errCode, mSgUser.range, dsCnt); //Method->Reactivate

    return result;

}

U16 Method_Erase(void)
{
    U64 tmp64;
    U16 result;
    U8 range=0xff,i,j;

soutb3(0x5C, 0xFE, 0x57);

    //parameter check
    if(ChkToken()!=TOK_StartList)    //test cases 3.1.5
        return STS_SESSION_ABORT;

    if(ChkToken()!=TOK_EndList)      //test cases 3.1.5
        return STS_SESSION_ABORT;

    if(ChkToken()!=TOK_EndOfData)    //test cases 3.1.5
        return STS_SESSION_ABORT;

    //status list check
    result = ChkMethodStatus();
    if(result!=STS_SUCCESS)
        return result;

    //get the locking range (invoking UID)
    if(invokingUID.all==UID_Locking_GRange)
        range=0;
    else
        range=(U8)invokingUID.all;
soutb(range);
    if(range<=LOCKING_RANGE_CNT)
    {
TCG_PRINTF("M_Erase: rng=%X\n",range);
        ChangeKey(range);
        G3.b.mLckLocking_Tbl.val[range].readLockEnabled=0x00;
        G3.b.mLckLocking_Tbl.val[range].writeLockEnabled=0x00;
        G3.b.mLckLocking_Tbl.val[range].readLocked=0x00;
        G3.b.mLckLocking_Tbl.val[range].writeLocked=0x00;

        //search for corresponding CPIN
        tmp64=UID_CPIN_User1+range;
        for(i=0;i<G3.b.mLckCPin_Tbl.rowCnt;i++)
        {
            if(tmp64==G3.b.mLckCPin_Tbl.val[i].uid)
            {
                for(j=0;j<PWD_LENGTH;j++)
                    G3.b.mLckCPin_Tbl.val[i].pwd[j]=0;

                G3.b.mLckCPin_Tbl.val[i].tries=0;
                break;
            }
        }
    }
    else
        result = STS_INVALID_PARAMETER;

    //update keys to Key RAM and NAND
    if(mSessionManager.TransactionState==TRNSCTN_IDLE)
    {
        TcgH2cJobIssue(MSG_TCG_G3WR, NULL);   //WaitG3Wr();

        //update key and range
        LockingRangeTable_Update();
        //for(i=0; i<=LOCKING_RANGE_CNT; i++){
        //    if(mLockingRangeTable[i].rangeNo==range) {
        //        HAL_SEC_SetAesKey(i, G3.b.mLckKAES_256_Tbl.val[range].key);
        //        break;
        //    }
        //}

        TcgResetCache(MSG_TCG_REGEN_ZEROCACHE);
    }
    else
    {
        G3MtableChanged = TRUE;
        bKeyChanged = TRUE;
    }

    dataBuf[iDataBuf++]=TOK_StartList;
    dataBuf[iDataBuf++]=TOK_EndList;
    dataBuf[iDataBuf++]=TOK_EndOfData;

    return result;
}



#ifdef _TCG_LAB_RESCUE_ONLY
U16 Method_Debug_Revert(void)
{
    U8  i,j;
    U32 key[LOCKING_RANGE_CNT+1][8];

    soutb3(0x5C, 0xFE, 0x44);  //Method_Debug_Revert()
    //G1.b.mAdmSP_Tbl.val[SP_LOCKING_IDX].lifeCycle = manufactured_inactive;    // (3) (4)

    //Revert Admin SP tables
    TcgH2cJobIssue(MSG_TCG_G1RDDEFAULT, NULL);   //cTbl_2_mTbl_byGrp(GRP_1);

    CopyHashed(&G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[1], G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[0], G1.b.mAdmCPin_Tbl.val[CPIN_SID_IDX].pwd);     // MSID 2 SID
    TcgH2cJobIssue(MSG_TCG_G1WR, NULL);   //WaitG1Wr();
    //clear MBR table  (7c)
    TcgH2cJobIssue(MSG_TCG_SMBRCLEAR, NULL);   //WaitMbrClr();

    //Revert Locking SP tables (?)
    TcgH2cJobIssue(MSG_TCG_G2RDDEFAULT, NULL);   //cTbl_2_mTbl_byGrp(GRP_2);
    TcgH2cJobIssue(MSG_TCG_G2WR, NULL);   //WaitG2Wr();

    for(i=0;i<=LOCKING_RANGE_CNT;i++)
    { //backup keys
soutb3(0x5C, 0xFE, 0xE5); soutb(i); soutd(mLckKAES_128_Tbl.val[i].key[0]);

        for(j=0;j<8;j++)
            key[i][j] = mLckKAES_128_Tbl.val[i].key[j];
    }

    TcgH2cJobIssue(MSG_TCG_G3RDDEFAULT, NULL);   //cTbl_2_mTbl_byGrp(GRP_3);

    for(i=0;i<=LOCKING_RANGE_CNT;i++)
    { //restore keys
        for(j=0;j<8;j++)
            mLckKAES_128_Tbl.val[i].key[j] = key[i][j];
    }
    TcgH2cJobIssue(MSG_TCG_G3WR, NULL);   //WaitG3Wr();

    Start_DataStoreLaa=0;
    End_DataStoreLaa=DATASTORE_LEN/LAA_LEN;

    TcgH2cJobIssue(MSG_TCG_SMBRCLEAR, NULL);   //WaitMbrClr();   //clear DataStore table (8)

    mTcgStatus = 0;
    LockingRangeTable_Update();

    {
        // to Lock any rd & wr in rescue mode
        U8 i;
        for(i=0; i<=LOCKING_RANGE_CNT; i++){
            mLockingRangeTable[i].writeLocked=1;
            mLockingRangeTable[i].readLocked=1;
        }
        soutb3(0x5C, 0xFE, 0x46); //!!Watch out, Lock any write and read in _TCG_LAB_RESCUE_ONLY
    }

    return STS_SUCCESS; //??? STS_SESSION_ABORT;   //D10-3-3-1-1
}
#endif

/***********************************************************
* Level0_Discovery
* Prepare Level 0 Discovery response data in *buf
* ref. core spec 3.3.6
***********************************************************/
U16 Level0_Discovery(U8* buf)
{
    U16 pt;
    U8 tmp=0;
    soutb3(0x5C, 0xFE, 0x00);  // show "Level0 Discovery"

    //pt=0;
    //construct L0 Discovery Header
                        //B0~B3: length
    pt=7;               //B4~7: Data structure revision
    buf[pt++]=0x01;
                        //B8~15: Reserved
    pt=48;              //B16~47: Vendor Specific

    //construct TPer Feature Descriptor
    buf[pt++]=0x00;     //B0~1: Feature Code (0x0001)
    buf[pt++]=0x01;
    buf[pt++]=0x10;     //B2: Version
    buf[pt++]=0x0C;     //B3: Length
    buf[pt++]=0x11;     //B4: Sync Supported, Streaming Supported
    pt += 11;           //B5~15: Reserved

    //construct Locking Feature Descriptor
    buf[pt++]=0x00;     //B0~1: Feature Code (0x0002)
    buf[pt++]=0x02;
    buf[pt++]=0x10;     //B2: Version
    buf[pt++]=0x0C;     //B3: Length

#if _TCG_ != TCG_PYRITE
    tmp=0x09;
#else
    tmp = 0x01;         //Media Encryption (b3) = 0, MBR Shadowing Absent (b6) = 0.
#endif
    //MBRControlSet case11(D5-1-2-2-4)check MBR_Done(b5), MBR_Enabled(b4), and Locked(b2) for B4
    if(G3.b.mLckMbrCtrl_Tbl.val[0].enable)
    { //check MBR_Enabled(b4)
        tmp |= 0x10;
        if(G3.b.mLckMbrCtrl_Tbl.val[0].done)  //check MRB_Done(b5) only when "enable" is true
            tmp |= 0x20;
    }

    if(G1.b.mAdmSP_Tbl.val[SP_LOCKING_IDX].lifeCycle==manufactured)
        tmp |= 0x02;    //Locking Enabled
    if(mReadLockedStatus||mWriteLockedStatus)
        tmp |= 0x04;    //'Locked'

    buf[pt++] = tmp;
    soutb(tmp);
    TCG_PRINTF("Level0 %02X\n",tmp);
    pt += 11;           //B5~15: Reserved

    //construct Geometry Reporting Feature Descriptor
    buf[pt++]=0x00;     //B0~1: Feature Code (0x0003)
    buf[pt++]=0x03;
    buf[pt++]=0x10;     //B2: Version
    buf[pt++]=0x1C;     //B3: Length
    buf[pt++]=0x01;     //B4: ALIGN
    pt += 7;            //B5~11: Reserved
    pt += 2;            //B12~15: LogicalBlockSize 512
    buf[pt++]=(U8)(TCG_LogicalBlockSize/256);
    buf[pt++]=(U8)TCG_LogicalBlockSize;
    pt += 7;            //B16~23: AlignmentGranularity
    buf[pt++]=TCG_AlignmentGranularity;
    pt += 8;            //B24~31: LowestAlignedLBA

#if _TCG_ != TCG_PYRITE
    //construct Single User Mode Feature Descriptor
    buf[pt++]=0x02;     //B0~1: Feature Code (0x0201)
    buf[pt++]=0x01;
    buf[pt++]=0x10;     //B2: Version
    buf[pt++]=0x0C;     //B3: Length
    pt += 3;            //B4~7: # of Locking Objects Supported:
    buf[pt++]= LOCKING_RANGE_CNT+1;

    if(mSgUser.range)   //B8: b2~Policy, b1~All, b0~Any
        tmp = 1;
    else
        tmp = 0;

    if(mSgUser.range==0xffff)   //EntireLocking
        tmp |= 0x02;

    if(mSgUser.policy)
        tmp |= 0x04;
    buf[pt++]=tmp;
    pt += 7;            //B9~15: Reserved

    //construct DataStore Table Feature Descriptor
    buf[pt++]=0x02;     //B0~1: Feature Code (0x0202)
    buf[pt++]=0x02;
    buf[pt++]=0x10;     //B2: Version
    buf[pt++]=0x0C;     //B3: Length
    pt += 2;            //B4~5 Reserved
    buf[pt++]=0x00;     //B6~7: Max. # of DataStore tables
    buf[pt++]=DSTBL_MAX_NUM;
    buf[pt++]=(U8)(DATASTORE_LEN>>24);    //B8~11: Max total size of DataStore tables
    buf[pt++]=(U8)(DATASTORE_LEN>>16);
    buf[pt++]=(U8)(DATASTORE_LEN>>8);
    buf[pt++]=(U8)DATASTORE_LEN;
    buf[pt++]=(U8)(DSTBL_ALIGNMENT>>24);  //B12~15: table size alignment
    buf[pt++]=(U8)(DSTBL_ALIGNMENT>>16);
    buf[pt++]=(U8)(DSTBL_ALIGNMENT>>8);
    buf[pt++]=(U8)DSTBL_ALIGNMENT;

    //construct Opal SSC Feature Descriptor
    buf[pt++]=0x02;     //B0~1: Feature Code (0x0203)
    buf[pt++]=0x03;
    buf[pt++]=0x10;     //B2: Version
    buf[pt++]=0x10;     //B3: Length

    buf[pt++]=(U8)(BASE_COMID/256); //B4~5: Base ComID
    buf[pt++]=(U8)BASE_COMID;
    buf[pt++]=0x00;     //B6~B7: Number of ComIDs
    buf[pt++]=0x01;
  #if 1 //def 1  //TCG_EDRIVE
    buf[pt++]=0x00;     //B8: x Range Crossing enabled
  #else
    buf[pt++]=0x01;     //B8: x Range Crossing  TODO: 0x00 to support Range Crossing for eDrive?
  #endif
    buf[pt++]=0;        //B9-10: Admin Authorities
    buf[pt++]=TCG_AdminCnt;
    buf[pt++]=0;        //B11-12: User Authorities
    buf[pt++]=TCG_UserCnt;
    buf[pt++]=0x00;     //B13: Initial C_PIN_SID
    buf[pt++]=0x00;     //B14: Behavior of C_PIN_SID PIN upon Revert
    pt += 5;           //B15-19, Reserved
#else
    //construct Pyrite SSC Feature Descriptor
    buf[pt++]=0x03;     //B0~1: Feature Code (0x0302)
    buf[pt++]=0x02;
    buf[pt++]=0x10;     //B2: Version
    buf[pt++]=0x10;     //B3: Length

    buf[pt++]=(U8)(BASE_COMID/256); //B4~5: Base ComID
    buf[pt++]=(U8)BASE_COMID;
    buf[pt++]=0x00;     //B6~B7: Number of ComIDs
    buf[pt++]=0x01;
    pt += (5+2+5);      //B8-12: Reserved,
                        //B13: Initial C_PIN_SID, B14: Behavior of C_PIN_SID PIN upon Revert
                        //B15-19, Reserved
#endif

#if CO_SUPPORT_BLOCK_SID_AUTH
    //construct Block SID Authentication Feature Descriptor
    buf[pt++]=0x04;     //B0~1: Feature Code (0x0302)
    buf[pt++]=0x02;
    buf[pt++]=0x10;     //B2: Version
    buf[pt++]=0x0C;     //B3: Length

    tmp=0x00;
    if(mTcgStatus&SID_VALUE_CHANGED)
        tmp |= 0x01;
    if(mTcgStatus&SID_BLOCKED)
        tmp |= 0x02;
    buf[pt++]=tmp;     //B4: SID Blocked State / SID Value State

    tmp=0x00;
    if(mTcgStatus&SID_HW_RESET)
        tmp |= 0x01;
    buf[pt++]=tmp;     //B5: Hardware Reset

    pt += 10;          //B6-10: Reserved
  #endif

    //update Length of Parameter Data
    pt -= 4;
    buf[2]=HiByte(pt);
    buf[3]=LoByte(pt);

    return (pt+4+1);
}

U16 H2TP_Properties(void)
{
    U8 byte, slen, hitHostPty[HostPropertiesCnt];
    U32 len;
    U16 kk, result,j;
    U32 data, tmpHostPty[HostPropertiesCnt];
    U8  hostPtyCnt=0;

soutb3(0x5C, 0xFE, 0x01);  // show "H2TP Properties"
    TCG_PRINTF("H2TP Properties\n");
    for(kk=0;kk<HostPropertiesCnt;kk++)
    {
        tmpHostPty[kk] = mHostProperties[kk].val;
        hitHostPty[kk] = 0;
    }

    //parameter check
    if(ChkToken()!=TOK_StartList)    //List1, test cases 3.1.5
        goto TOKEN_ERROR;    //return STS_SESSION_ABORT;

    byte = ChkToken();
    //if(byte==TOK_EndList)   //no  Host Properties
    //    goto END_LIST;

    if(byte==TOK_StartName) //Name1
    { //check host properties
        if (AtomDecoding_Uint8(&byte)!=STS_SUCCESS)
        {    result=STS_INVALID_PARAMETER; goto STATUS_ERROR;   }

        if(byte==0x00)
        { // Host Properties
            if(ChkToken()==TOK_StartList)
            { //L2
                while((byte=ChkToken())!=0xff)
                {
                    if(byte==TOK_StartName)
                    {
                        if(AtomDecoding_Bytes(&len)!=STS_SUCCESS)   //name
                        {    result=STS_INVALID_PARAMETER;  goto STATUS_ERROR; }

                        if(AtomDecoding_Uint32(&data)!=STS_SUCCESS) //value
                        {    result=STS_INVALID_PARAMETER;  goto STATUS_ERROR; }

                        hostPtyCnt++;
//soutb(hostPtyCnt);
                        DecBuf[len]= '\0';

                        for(kk=0;kk<HostPropertiesCnt;kk++)
                        {
                            if(strcmp((char *)DecBuf,mHostProperties[kk].name)==0)
                            //if(StrCmp2((char *)DecBuf,mHostProperties[kk].name)==TRUE)
                            {
soutb3(0x5C, 0xFE, 0x1D); soutb(kk); soutd(data);
                                if(hitHostPty[kk]==0)
                                    hitHostPty[kk]++;
                                else
                                {    result=STS_INVALID_PARAMETER;  goto STATUS_ERROR; }   //A6-3-4-2-1(2)

                                if(kk==0)
                                {
                                    if(data<HOST_MAX_COMPKT_SZ)
                                        data = HOST_MAX_COMPKT_SZ;
//souts("ComPktSz");
                                    //mHostProperties[0].val=data;
                                    tmpHostPty[0]=data;
                                }
                                else if(kk==1)
                                {
                                    if(data<HOST_MAX_PKT_SZ)
                                        data = HOST_MAX_PKT_SZ;
  //souts("PktSz");
                                    //mHostProperties[1].val=data;
                                    tmpHostPty[1]=data;
                                }
                                else if(kk==2)
                                {
                                    if(data<HOST_MAX_INDTKN_SZ)
                                        data = HOST_MAX_INDTKN_SZ;
 //souts("IndTokSz:");
                                    //mHostProperties[2].val=data;
                                    tmpHostPty[2]=data;
                                }
                                break;
                            }
                        }

                        if(ChkToken()!=TOK_EndName) //EndName2
                        {    result=STS_INVALID_PARAMETER;  goto STATUS_ERROR; }
                    }
                    else if(byte==TOK_EndList) //EndList2
                        break;
                    else
                    {    result=STS_INVALID_PARAMETER;  goto STATUS_ERROR; }
                }
            }
        }
        else
        {    result=STS_INVALID_PARAMETER;  goto STATUS_ERROR; }

        if((byte=ChkToken())!=TOK_EndName)   //EndName1
        {    result=STS_INVALID_PARAMETER;  goto STATUS_ERROR; }

        byte=ChkToken();
        if(byte==TOK_StartName)
        {    result=STS_INVALID_PARAMETER;  goto STATUS_ERROR; }   //A6-3-4-2-1(2), encoded twice!
    }
    //else //NG within parameters
    //    goto TOKEN_ERROR;    //return STS_INVALID_PARAMETER;

//END_LIST:
    if(byte!=TOK_EndList)   //EndList1
        goto TOKEN_ERROR;

    if(ChkToken()!=TOK_EndOfData)
        goto TOKEN_ERROR;

    //status list check
    result = ChkMethodStatus();
    if(result==STS_SESSION_ABORT)
        goto TOKEN_ERROR;
    if(result!=STS_SUCCESS)
        goto STATUS_ERROR;  //return result;

//cj test: Subpackets > 1 [
    kk=iPload%4;
    if(kk) iPload += (4-kk);

    if((iPload+12+12)<=mCmdPkt.mPktFmt.length)
    {
        iPload += 8; //reserved  + kind
        HiByte(HiWord(data)) = mCmdPkt.payload[iPload++];
        LoByte(HiWord(data)) = mCmdPkt.payload[iPload++];
        HiByte(LoWord(data)) = mCmdPkt.payload[iPload++];
        LoByte(LoWord(data)) = mCmdPkt.payload[iPload++];

        if(data) goto TOKEN_ERROR;
    }
// ]

    //Success!!
    for(kk=0;kk<HostPropertiesCnt;kk++)
        mHostProperties[kk].val = tmpHostPty[kk];

    //prepare payload for reply: TperPerperties data

    dataBuf [iDataBuf++] = TOK_Call;
    dataBuf [iDataBuf++] = 0xA8;
    for(j=8; j!=0; )
        dataBuf[iDataBuf++] = invokingUID.bytes[--j];   //SessionManagerUID

    dataBuf [iDataBuf++] = 0xA8;
    for(j=8; j!=0; )
        dataBuf[iDataBuf++] =methodUID.bytes[--j];      //PropertiesUID

    dataBuf[iDataBuf++] = TOK_StartList;
    dataBuf[iDataBuf++] = TOK_StartList;

    for(kk=0;kk<TperPropertiesCnt;kk++) {
        dataBuf[iDataBuf++] = TOK_StartName;

        slen = strlen(mTperProperties[kk].name);

        if(slen<0x10)
            dataBuf[iDataBuf++]=0xA0 + slen;
        else
        {
            dataBuf[iDataBuf++]=0xD0;
            dataBuf[iDataBuf++]=slen;
        }

        for(j=0;j<slen;j++)
        {
            byte=*(mTperProperties[kk].name+j);
            dataBuf[iDataBuf++]=byte;
        }
        AtomEncoding_Integer((U8 *)&mTperProperties[kk].val,sizeof(mTperProperties[0].val));
    //D_PRINTF("%02X %08X\n", kk, mTperProperties[kk].val);

        dataBuf[iDataBuf++] = TOK_EndName;
    }

    dataBuf[iDataBuf++] = TOK_EndList;

    if(hostPtyCnt)
    {
        dataBuf[iDataBuf++] = TOK_StartName;
        dataBuf[iDataBuf++] = 0x00;
        dataBuf[iDataBuf++] = TOK_StartList;

        for(kk=0;kk<HostPropertiesCnt;kk++)
        {
            dataBuf[iDataBuf++] = TOK_StartName;

            slen = strlen(mHostProperties[kk].name);

            if(slen<0x10)
                dataBuf[iDataBuf++]=0xA0 + slen;
            else
            {
                dataBuf[iDataBuf++]=0xD0;
                dataBuf[iDataBuf++]=slen;
            }

            for(j=0;j<slen;j++)
            {
                byte=*(mHostProperties[kk].name+j);
                dataBuf[iDataBuf++]=byte;
            }
            AtomEncoding_Integer((U8 *)&mHostProperties[kk].val,sizeof(mHostProperties[0].val));
//souts("HostPty:");
//soutd(mHostProperties[kk].val);
            dataBuf[iDataBuf++] = TOK_EndName;
        }

        dataBuf[iDataBuf++] = TOK_EndList;
        dataBuf[iDataBuf++] = TOK_EndName;
    }

    dataBuf[iDataBuf++] = TOK_EndList;
    dataBuf[iDataBuf++] = TOK_EndOfData;

    return STS_SUCCESS;

STATUS_ERROR:
    dataBuf[iDataBuf++] = TOK_StartList;
    dataBuf[iDataBuf++] = TOK_EndList;
    dataBuf[iDataBuf++] = TOK_EndOfData;

    return result;

TOKEN_ERROR:
    if(bControlSession)
        return STS_STAY_IN_IF_SEND;
    else
        return STS_SESSION_ABORT;

        }


/***********************************************************
* Level0_Discovery
* Prepare Level 0 Discovery response data in *buf
* ref. core spec 3.3.6
***********************************************************/
void Supported_Security_Protocol(U8* buf)
{
    U16 i, pt;
soutb3(0x5C, 0xFE, 0x10);

    pt=0;
    //Reserved
    for(i=0;i<6;i++)
        buf[pt++]=0;

    //List Length
    buf[pt++] = 0x00;
#if _TCG_ == TCG_EDRV
        buf[pt++] = 0x04;

        //List
        buf[pt++] = 0x00;
        buf[pt++] = 0x01;
        buf[pt++] = 0x02;
        buf[pt++] = 0xEE;   //IEEE1667
#else
    buf[pt++] = 0x03;

    //List
    buf[pt++] = 0x00;
    buf[pt++] = 0x01;
    buf[pt++] = 0x02;
#endif

    return;
}


void ResetSessionManager(void)
{
    U32 i;
    mSessionManager.HostSessionID = 0;

    //for(i=0;i<mSessionManager.HtChallenge[0]; i++)
    for(i=0;i<=CPIN_LENGTH; i++)
        mSessionManager.HtChallenge[i]=0;

    mSessionManager.HtSgnAuthority.all = UID_Null;
    mSessionManager.SPID.all = UID_Null;

    //mSessionManager.SPSessionID = 0x1001;     //assigned by TPer

    mSessionManager.state = SESSION_CLOSE;

    if(mSessionManager.TransactionState==TRNSCTN_ACTIVE)
    { //Transaction abort!!
        ReadNAND2Mtable();
        ClearMtableChangedFlag();
    }
        mSessionManager.TransactionState=TRNSCTN_IDLE;

    //TODO: construct payload for response
    //dataBuf[iDataBuf++] = TOK_EndOfSession;
}

//
//LockingTbl Power-On Reset
//      It will check in each range and
// i) set 'ReadLocked' bit if 'ReadLockEnabled' is set,
// ii) set 'WriteLocked' bit if 'WriteLockEnabled' is set
//
void LockingTbl_Reset(U8 type)
{
   U8 i,j;
soutb3(0x5C, 0xFE, 0xE2);

   for(i=0;i<=LOCKING_RANGE_CNT;i++)
   {
       for(j=1;j<=G3.b.mLckLocking_Tbl.val[i].lockOnReset[0];j++)
       {
           if(G3.b.mLckLocking_Tbl.val[i].lockOnReset[j]==type)
           {
               if(G3.b.mLckLocking_Tbl.val[i].readLockEnabled)
                   G3.b.mLckLocking_Tbl.val[i].readLocked=TRUE;

               if(G3.b.mLckLocking_Tbl.val[i].writeLockEnabled)
                   G3.b.mLckLocking_Tbl.val[i].writeLocked=TRUE;

               break;
           }
       }
   }

}

//
// Compared the intended Locking Table uid/rangeStart/rangeLength with the other ranges.
// return zOK if there is no overlap with the other effective ranges (i.e., its rangeLength should not be zero),
// otherwise return zNG.
//
int LockingTbl_RangeChk(U64 uid, U64 rangeStart, U64 rangeLength)
{
    U8 i;
    U64 rangeEnd = rangeStart;
    U64 tmpRangeStart,tmpRangeLength,tmpRangeEnd;

soutb3(0x5C, 0xFE, 0xE3);

    //if(rangeStart%(CFG_UDATA_PER_PAGE/LBA_SIZE_IN_BYTE))
    if(rangeStart%TCG_AlignmentGranularity)
    {
        soutb(0xF0);    sout64(rangeStart); //souts("!!NG: StartAlignment");
#if _TCG_ == TCG_EDRV
        if(bEHddLogoTest==FALSE)
        {
            if((mPsidRevertCnt>5) && (rangeStart==0x14) && (rangeLength==0x14))
                bEHddLogoTest = TRUE;  //LogoTest, skip alignment check
        }

        if(bEHddLogoTest==FALSE)
#endif
        return zNG; //not align
    }

    //if(rangeLength%(CFG_UDATA_PER_PAGE/LBA_SIZE_IN_BYTE))
    if(rangeLength%TCG_AlignmentGranularity)
    {
        soutb(0xF1);    sout64(rangeLength); //souts("!!NG: LengthAlignment");
#if _TCG_ == TCG_EDRV
        if(bEHddLogoTest==FALSE)
#endif
        return zNG; //not align
    }

    if(rangeLength) //!=0
        rangeEnd += (rangeLength-1);

    else
    {
        soutb(0x01);
        return zOK; //TODO: need to check... No LBAs are covered by this range (please check Test Case 3.3.4, D4-1-3-3-1)
    }

    for(i=1;i<=LOCKING_RANGE_CNT;i++)   //skip GloblaRange here
    {
        if(uid==G3.b.mLckLocking_Tbl.val[i].uid)
            continue;

        tmpRangeStart = G3.b.mLckLocking_Tbl.val[i].rangeStart;
        tmpRangeLength = G3.b.mLckLocking_Tbl.val[i].rangeLength;

        if(tmpRangeLength==0) //No LBAs are covered by this range (please check Test Case 3.3.4, D4-1-3-3-1)
            continue;

        tmpRangeEnd = tmpRangeStart + (tmpRangeLength-1);

        //check if there is any overlap between these two ranges:
        //  a. rangeStart is in this range
        //  b. rangeEnd is in this range
        //  c. rangeStart is smaller than tmpRangeStart and rangeEnd is larger than tmpRangeEnd
        if((rangeStart>=tmpRangeStart)&&(rangeStart<=tmpRangeEnd))
        {
            soutb(0xFF);
            return zNG;
        }
        if((rangeEnd>=tmpRangeStart)&&(rangeEnd<=tmpRangeEnd))
        {
            soutb(0xFE);
            return zNG;
        }
        if((rangeStart<=tmpRangeStart)&&(rangeEnd>=tmpRangeEnd))
        {
            soutb(0xFD);
            return zNG;
        }
    }

soutb(0x00);
    return zOK;
}

//
// Update "mReadLockedTable[]" and "mWriteLockedTable[]" for easier/faster ATA Kernel access.
//
//      These two tables are extracted from "G3.b.mLckLocking_Tbl" for Media Read/Write control.
//      The object sequence in these tables are sorted according to the "ragneStart" in "G3.b.mLckLocking_Tbl".
//      LockingTbl_RangeChk() should be run to avoid range overlapping.
//
//      Table Object:     [rangeNo, rangeStart, rangeEnd]
//      rangeNo=0 for GlobalRange, and it should be located at the last effective row in the tables if it is locked.
//
void LockingRangeTable_Update(void)
{
    U8 i,k,iSort,rangeCnt=0;
    U64 tmpRangeStart,tmpRangeLength,tmpRangeEnd;

    mReadLockedStatus=0;
    mWriteLockedStatus=0;

    //establish the table ...
    for(i=1;i<=LOCKING_RANGE_CNT;i++)   //skip GloblaRange here
    {
        //check if this is an effective range
        tmpRangeLength = G3.b.mLckLocking_Tbl.val[i].rangeLength;
        //D_PRINTF("** %02X %08X\n", i, tmpRangeLength);
        if(tmpRangeLength==0)    //not an effective range, skip
            continue;

        tmpRangeStart = G3.b.mLckLocking_Tbl.val[i].rangeStart;
        tmpRangeEnd =  tmpRangeStart + (tmpRangeLength-1);

        //sorting in "mLockingRangeTable[]"
        for(iSort=0;iSort<rangeCnt;iSort++)
        {
            if(tmpRangeStart<mLockingRangeTable[iSort].rangeStart)
                break;
        }

        if(rangeCnt) // previous range data existed in the table, update them first
        {
            for(k=rangeCnt;k>iSort;k--)
            {
                mLockingRangeTable[k].rangeNo = mLockingRangeTable[k-1].rangeNo;
                mLockingRangeTable[k].rangeStart = mLockingRangeTable[k-1].rangeStart;
                mLockingRangeTable[k].rangeEnd = mLockingRangeTable[k-1].rangeEnd;
                mLockingRangeTable[k].blkcnt = mLockingRangeTable[k-1].blkcnt;
                mLockingRangeTable[k].readLocked = mLockingRangeTable[k-1].readLocked;
                mLockingRangeTable[k].writeLocked = mLockingRangeTable[k-1].writeLocked;
            }
        }

        //add the new object
        mLockingRangeTable[iSort].rangeNo = i;
        mLockingRangeTable[iSort].rangeStart = tmpRangeStart;
        mLockingRangeTable[iSort].rangeEnd = tmpRangeEnd;
        mLockingRangeTable[iSort].blkcnt = (U32)tmpRangeLength;  //added for AltaPlus
        //D_PRINTF("bct: %02X %08X\n", i, mLockingRangeTable[iSort].blkcnt);


        if(G3.b.mLckLocking_Tbl.val[i].readLockEnabled && G3.b.mLckLocking_Tbl.val[i].readLocked)
        { //this range is Read-Locked
            mLockingRangeTable[iSort].readLocked = 0x01;
            mReadLockedStatus |= (0x01<<i);
        }
        else
            mLockingRangeTable[iSort].readLocked = 0x00;

        if(G3.b.mLckLocking_Tbl.val[i].writeLockEnabled && G3.b.mLckLocking_Tbl.val[i].writeLocked)
        { //this range is Write-Locked
            mLockingRangeTable[iSort].writeLocked = 0x01;
            mWriteLockedStatus |= (0x01<<i);
        }
        else
            mLockingRangeTable[iSort].writeLocked = 0x00;

        rangeCnt++;
    }

    //add GlobalRange as the last object
    mLockingRangeTable[rangeCnt].rangeNo = 0;
    mLockingRangeTable[rangeCnt].rangeStart = 0;
    mLockingRangeTable[rangeCnt].rangeEnd = 0;
    mLockingRangeTable[rangeCnt].blkcnt= 0xffffffff;

    if(G3.b.mLckLocking_Tbl.val[0].readLockEnabled && G3.b.mLckLocking_Tbl.val[0].readLocked)
    { //global range is Read-Locked
        mLockingRangeTable[rangeCnt].readLocked = 0x01;
        mReadLockedStatus |= 0x01;
    }
    else
        mLockingRangeTable[rangeCnt].readLocked = 0x00;

    if(G3.b.mLckLocking_Tbl.val[0].writeLockEnabled && G3.b.mLckLocking_Tbl.val[0].writeLocked)
    { //global range is Write-Locked
        mLockingRangeTable[rangeCnt].writeLocked = 0x01;
        mWriteLockedStatus |= 0x01;
    }
    else
        mLockingRangeTable[rangeCnt].writeLocked = 0x00;

    if(rangeCnt!=LOCKING_RANGE_CNT)
    {
        // update the last row for TcgRangeCheck()
        mLockingRangeTable[LOCKING_RANGE_CNT].rangeNo = 0;
        mLockingRangeTable[LOCKING_RANGE_CNT].rangeStart = 0;
        mLockingRangeTable[LOCKING_RANGE_CNT].rangeEnd = 0;
        mLockingRangeTable[LOCKING_RANGE_CNT].readLocked = mLockingRangeTable[rangeCnt].readLocked;
        mLockingRangeTable[LOCKING_RANGE_CNT].writeLocked = mLockingRangeTable[rangeCnt].writeLocked;
    }

    //++rangeCnt;

    HAL_SEC_InitAesKeyRng();

#if 1
    DumpRangeInfo();
#else
    soutb3(0x5C, 0xFE, 0xE4); soutw(mReadLockedStatus); soutw(mWriteLockedStatus);
    for(i=0;i<=rangeCnt;i++)
    {
        soutb3(0x5C, 0xFE, 0xE6);

        soutb3(mLockingRangeTable[i].rangeNo, mLockingRangeTable[i].readLocked, mLockingRangeTable[i].writeLocked);
        sout64(mLockingRangeTable[i].rangeStart);
        sout64(mLockingRangeTable[i].rangeEnd);
    }
#endif

    //          Need to take care of MBR-S condition ...

}



//
//MbrCtrlTbl Power-On or Porgrammatic Reset
//
//  set 'Done' bit according to reset type and DoneOnReset setting
//
void MbrCtrlTbl_Reset(U8 type)
{
    U8 i;

    for(i=1;i<=G3.b.mLckMbrCtrl_Tbl.val[0].doneOnReset[0];i++)
    {
        if(G3.b.mLckMbrCtrl_Tbl.val[0].doneOnReset[i]==type)
        {
//souts("MBR DOR!");
            G3.b.mLckMbrCtrl_Tbl.val[0].done=FALSE;
            break;
        }
    }

    if((G3.b.mLckMbrCtrl_Tbl.val[0].enable==TRUE)&&(G3.b.mLckMbrCtrl_Tbl.val[0].done==FALSE))
        mTcgStatus |= MBR_SHADOW_MODE;
    else
        mTcgStatus &= (~MBR_SHADOW_MODE);

soutb3(0x5C, 0xFE, 0xD9);  soutb(mTcgStatus);

    return;
}


void TcgPreformatAndInit(U8 IsTcgInit)
{
    U32 x;
    U32 DefChanMap;

    HERE(NULL);
    strcpy((char *)tcg_prefmt_tag, PREFORMAT_START_TAG);

#ifdef FORCE_TO_CLEAR_ERASED_COUNT
    MEM_SET((void *)tcgG4EraCnt, 0, sizeof(tcgG4EraCnt));   // force clear G4 erased count
    MEM_SET((void *)tcgG5EraCnt, 0, sizeof(tcgG5EraCnt));   // force clear G5 erased count
#endif

    if(strcmp((char *)tcgDefectID, DEFECT_STRING) != 0){   // first time ? , if so, then init defect table
        MEM_SET((void *)tcgG4Dft, 0, sizeof(tcgG4Dft));   // force clear G4 defect table
        MEM_SET((void *)tcgG5Dft, 0, sizeof(tcgG5Dft));   // force clear G5 defect table
        TCG_PRINTF(">>> Defect table is cleared.\n");  //soutb3(0x5C, 0xFE, 0x99);  //5C FE 99,    ">>> Defect table is cleared."
    }

    if(strcmp((char *)tcgErasedCntID, ERASED_CNT_STRING) != 0){   // first time ? , if so, then init erased count table
        MEM_SET((void *)tcgG4EraCnt, 0, sizeof(tcgG4EraCnt));   // force clear G4 erased count table
        MEM_SET((void *)tcgG5EraCnt, 0, sizeof(tcgG5EraCnt));   // force clear G5 erased count table
        TCG_PRINTF(">>> Erase count table is cleared.\n");  //soutb3(0x5C, 0xFE, 0x9A);  //5C FE 9A,    ">>> Erase count table is cleared."
    }

    TcgH2cJobIssue(MSG_TCG_G4BUILDDEFECT, NULL);
    DumpG4DftAmount();

    TcgH2cJobIssue(MSG_TCG_G5BUILDDEFECT, NULL);
    DumpG5DftAmount();

    // ID : tcg pre-format ID for tcgInit()
    strcpy(preformat_id_string,PREFORMAT_STRING);
    // ID : record defected ID in NOR
    if(strcmp((char *)tcgDefectID, DEFECT_STRING) != 0){   // first time ? , if so, then init defect table
        strcpy((char *)tcgDefectID,DEFECT_STRING);          // ID
    }
    // ID : record erased count ID in NOR
    if(strcmp((char *)tcgErasedCntID, ERASED_CNT_STRING) != 0){   // first time ? , if so, then init erased count table
        strcpy((char *)tcgErasedCntID,ERASED_CNT_STRING);   // ID
    }

    if((TcgG4Defects > TCG_MBR_CELLS/2) || (TcgG5Defects > TCG_MBR_CELLS/2)){
        //souts("Error!!, There are a lot of defect blocks. TCG function off.");
        TCG_PRINTF("Error!!, There are a lot of defect blocks. TCG function off.");
        TableError = TRUE;
        return;
    }

    strcpy((char *)tcg_prefmt_tag, PREFORMAT_END_TAG);
#ifdef TCG_EEP_NOR
    TcgH2cJobIssue(MSG_TCG_NOREEP_WR, NULL);
#else
    SyIn_Synchronize(SI_AREA_MISC, SYSINFO_WRITE, SI_SYNC_BY_TCG);
#endif

    if(IsTcgInit == TRUE){
        if(ChkDefaultTblPattern() == zOK)
            TcgInit(TRUE, TRUE); //reset and clear cache
        else
            TCG_PRINTF("Error!!, Default table error, power off.");
    }
}



#ifdef _ALEXDEBUG
void souts2(char *str)
{
    while(1){
        soutb(*str);
        if(*str==0) break;
        str++;
    }
}

void alexdebug1(U8 dat0)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xE0);
    soutb(dat0);
    MUTEX_UNLOCK_INT;
}
void alexdebug2(U8 dat0,U8 dat1)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xE1);
    soutb(dat0);
    soutb(dat1);
    MUTEX_UNLOCK_INT;
}
void alexdebug3(U8 dat0,U8 dat1,U8 dat2)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xE2);
    soutb(dat0);
    soutb(dat1);
    soutb(dat2);
    MUTEX_UNLOCK_INT;
}
void alexdebug4(U8 dat0,U8 dat1,U8 dat2,U8 dat3)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xE3);
    soutb(dat0);
    soutb(dat1);
    soutb(dat2);
    soutb(dat3);
    MUTEX_UNLOCK_INT;
}
void alexdebugw(U16 dat0)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xE4);
    soutb(dat0>>8);
    soutb(dat0);
    MUTEX_UNLOCK_INT;
}
void alexdebugd(U32 dat0)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xE5);
    soutb(dat0>>24);
    soutb(dat0>>16);
    soutb(dat0>>8);
    soutb(dat0);
    MUTEX_UNLOCK_INT;
}
void alexdebugl(U64 dat0)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xE6);
    alexdebugdns(dat0>>32);
    alexdebugdns(dat0);
    MUTEX_UNLOCK_INT;
}

void alexdebugdns(U32 dat0)
{
    //MUTEX_LOCK_INT;
    soutb(dat0>>24);
    soutb(dat0>>16);
    soutb(dat0>>8);
    soutb(dat0);
    //MUTEX_UNLOCK;
}
//--------------------------------
void alexdebug1_s(U8 dat0,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xF0);
    soutb(dat0);
    souts2(str);
    MUTEX_UNLOCK_INT;
}
void alexdebug2_s(U8 dat0,U8 dat1,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xF1);
    soutb(dat0);
    soutb(dat1);
    souts2(str);
    MUTEX_UNLOCK_INT;
}
void alexdebug3_s(U8 dat0,U8 dat1,U8 dat2,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xF2);
    soutb(dat0);
    soutb(dat1);
    soutb(dat2);
    souts2(str);
    MUTEX_UNLOCK_INT;
}
void alexdebug4_s(U8 dat0,U8 dat1,U8 dat2,U8 dat3,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xF3);
    soutb(dat0);
    soutb(dat1);
    soutb(dat2);
    soutb(dat3);
    souts2(str);
    MUTEX_UNLOCK_INT;
}
void alexdebugw_s(U16 dat0,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xF4);
    soutb(dat0>>8);
    soutb(dat0);
    souts2(str);
    MUTEX_UNLOCK_INT;
}
void alexdebugd_s(U32 dat0,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xF5);
    soutb(dat0>>24);
    soutb(dat0>>16);
    soutb(dat0>>8);
    soutb(dat0);
    souts2(str);
    MUTEX_UNLOCK_INT;
}
void alexdebugl_s(U64 dat0,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xF6);
    alexdebugdns(dat0>>32);
    alexdebugdns(dat0);
    souts2(str);
    MUTEX_UNLOCK_INT;
}
void alexdebug2w_s(U16 dat0,U16 dat1,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xF7);
    soutb(dat0>>8);
    soutb(dat0);

    soutb(dat1>>8);
    soutb(dat1);

    souts2(str);
    MUTEX_UNLOCK_INT;
}
void alexdebug2d_s(U32 dat0,U32 dat1,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xF8);
    soutb(dat0>>24);
    soutb(dat0>>16);
    soutb(dat0>>8);
    soutb(dat0);

    soutb(dat1>>24);
    soutb(dat1>>16);
    soutb(dat1>>8);
    soutb(dat1);

    souts2(str);
    MUTEX_UNLOCK_INT;
}
void alexdebug2l_s(U64 dat0,U64 dat1,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xF9);
    alexdebugdns(dat0>>32);
    alexdebugdns(dat0);

    alexdebugdns(dat1>>32);
    alexdebugdns(dat1);

    souts2(str);
    MUTEX_UNLOCK_INT;
}

void alexdebug3w_s(U16 dat0,U16 dat1,U16 dat2,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xFA);
    soutb(dat0>>8);
    soutb(dat0);

    soutb(dat1>>8);
    soutb(dat1);

    soutb(dat2>>8);
    soutb(dat2);

    souts2(str);
    MUTEX_UNLOCK_INT;
}
void alexdebug3d_s(U32 dat0,U32 dat1,U32 dat2,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xFB);
    soutb(dat0>>24);
    soutb(dat0>>16);
    soutb(dat0>>8);
    soutb(dat0);

    soutb(dat1>>24);
    soutb(dat1>>16);
    soutb(dat1>>8);
    soutb(dat1);

    soutb(dat2>>24);
    soutb(dat2>>16);
    soutb(dat2>>8);
    soutb(dat2);

    souts2(str);
    MUTEX_UNLOCK_INT;
}
void alexdebug3l_s(U64 dat0,U64 dat1,U64 dat2,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xFC);
    alexdebugdns(dat0>>32);
    alexdebugdns(dat0);

    alexdebugdns(dat1>>32);
    alexdebugdns(dat1);

    alexdebugdns(dat2>>32);
    alexdebugdns(dat2);

    souts2(str);
    MUTEX_UNLOCK_INT;
}

void alexdebug4w_s(U16 dat0,U16 dat1,U16 dat2,U16 dat3,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xFD);
    soutb(dat0>>8);
    soutb(dat0);

    soutb(dat1>>8);
    soutb(dat1);

    soutb(dat2>>8);
    soutb(dat2);

    soutb(dat3>>8);
    soutb(dat3);

    souts2(str);
    MUTEX_UNLOCK_INT;
}
void alexdebug4d_s(U32 dat0,U32 dat1,U32 dat2,U32 dat3,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xFE);
    soutb(dat0>>24);
    soutb(dat0>>16);
    soutb(dat0>>8);
    soutb(dat0);

    soutb(dat1>>24);
    soutb(dat1>>16);
    soutb(dat1>>8);
    soutb(dat1);

    soutb(dat2>>24);
    soutb(dat2>>16);
    soutb(dat2>>8);
    soutb(dat2);

    soutb(dat3>>24);
    soutb(dat3>>16);
    soutb(dat3>>8);
    soutb(dat3);

    souts2(str);
    MUTEX_UNLOCK_INT;
}
void alexdebug4l_s(U64 dat0,U64 dat1,U64 dat2,U64 dat3,char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xFF);
    alexdebugdns(dat0>>32);
    alexdebugdns(dat0);

    alexdebugdns(dat1>>32);
    alexdebugdns(dat1);

    alexdebugdns(dat2>>32);
    alexdebugdns(dat2);

    alexdebugdns(dat3>>32);
    alexdebugdns(dat3);

    souts2(str);
    MUTEX_UNLOCK_INT;
}

void alexdebug_s(char *str)
{
    MUTEX_LOCK_INT;
    soutb3(0x5C, 0xFF, 0xEF);
    souts2(str);
    MUTEX_UNLOCK_INT;
}
#endif

int GroupAddrShare(void)
{
    MsgTcgGroup_t *TcgMsg;

    TcgMsg = (MsgTcgGroup_t*)Ipc_GetExtendMsg(cMcTcg);

    TcgMsg->pG1Tbl = &G1.all;
    TcgMsg->pG2Tbl = &G2.all;
    TcgMsg->pG3Tbl = &G3.all;
    TcgMsg->pG4Tbl = &G4.all;
    TcgMsg->pG5Tbl = &G5.all;

    return(TcgH2cJobIssue(MSG_TCG_SETGRPADDR, (MsgTcg_t*)TcgMsg));
}

int BufAddrShare(void)
{
    MsgTcgBuffer_t *TcgMsg;

    TcgMsg = (MsgTcgBuffer_t*)Ipc_GetExtendMsg(cMcTcg);

    TcgMsg->pTmpBuf          = TcgTmpBuf;
    TcgMsg->pTmpMBRL2PTbl    = MBR_TEMPL2PTBL;
    TcgMsg->pTmpDSL2PTbl     = DS_TEMPL2PTBL;
    TcgMsg->pLockingRangeTbl = mLockingRangeTable;
    return(TcgH2cJobIssue(MSG_TCG_SETBUFADDR, (MsgTcg_t*)TcgMsg));
}

int SMBR_Read(U16 laas, U16 laae, PVOID pBuffer, MsgTcg_t** myTcgMsg)
{
    MsgTcg_t* pTcgMsg;

    pTcgMsg = (MsgTcg_t*)Ipc_GetExtendMsg(cMcTcg);
    if(myTcgMsg != NULL) *myTcgMsg = pTcgMsg;

    pTcgMsg->laas            = laas;
    pTcgMsg->laae            = laae;
    pTcgMsg->pBuffer         = pBuffer;
    pTcgMsg->param[0]        = RdTcgMBRbyNormalRdCmd;
    pTcgMsg->param[1]        = (U32)mSessionManager.TransactionState;

    return(TcgH2cJobIssue(MSG_TCG_SMBRRD, pTcgMsg));
}

int SMBR_Write(U16 laas, U16 laae, PVOID pBuffer, U16 DesOffset, U16 SrcLen, U8* SrcBuf)
{
    MsgTcg_t *TcgMsg;

    TcgMsg = (MsgTcg_t*)Ipc_GetExtendMsg(cMcTcg);

    TcgMsg->laas      =   laas;
    TcgMsg->laae      =   laae;
    TcgMsg->pBuffer   =   pBuffer;
    TcgMsg->param[0]  =   (DesOffset << 16) | (0x0000FFFF & SrcLen);
    TcgMsg->param[1]  =   (U32)SrcBuf;
    return(TcgH2cJobIssue(MSG_TCG_SMBRWR, TcgMsg));
}

int DS_Read(U16 laas, U16 laae, PVOID pBuffer)
{
    MsgTcg_t* pTcgMsg;

    pTcgMsg = (MsgTcg_t*)Ipc_GetExtendMsg(cMcTcg);

    pTcgMsg->laas = laas;
    pTcgMsg->laae = laae;
    pTcgMsg->pBuffer = pBuffer;

    return(TcgH2cJobIssue(MSG_TCG_DSRD, pTcgMsg));
}

int DS_Write(U16 laas, U16 laae, PVOID pBuffer, U16 DesOffset, U16 SrcLen, U8* SrcBuf)
{
    MsgTcg_t *pTcgMsg;

    pTcgMsg = (MsgTcg_t*)Ipc_GetExtendMsg(cMcTcg);

    pTcgMsg->laas      =   laas;
    pTcgMsg->laae      =   laae;
    pTcgMsg->pBuffer   =   pBuffer;
    pTcgMsg->param[0]  =   (DesOffset << 16) | (0x0000FFFF & SrcLen);
    pTcgMsg->param[1]  =   (U32)SrcBuf;

    return(TcgH2cJobIssue(MSG_TCG_DSWR, pTcgMsg));
}

int TcgH2cJobIssue(tMSG_TCG_SUBOP subOP, MsgTcg_t* pTcgMsg)
{
    if (pTcgMsg == NULL){
        pTcgMsg = (MsgTcg_t*)Ipc_GetExtendMsg(cMcTcg);
    }

    pTcgMsg->hdr.b.subOpCode = subOP;

    IPC_SendMsgQ(cH2cReqQueue, (Msg_t*) pTcgMsg);  //send MSG to CPU2(Core)

    while (pTcgMsg->hdr.b.status != cMsgCompleted);
    if(pTcgMsg->error == cEcNoError){
        return zOK;
    }else{
        return zNG;
    }
}

int TcgResetCache(tMSG_TCG_SUBOP subOP)
{
    MsgTcg_t*   pTcgMsg;

    pTcgMsg = (MsgTcg_t*)Ipc_GetExtendMsg(cMcTcg);

    pTcgMsg->hdr.b.opCode = cMcResetCache;
    pTcgMsg->param[0]  = RST_CACHE_INIT;

    return TcgH2cJobIssue(subOP, pTcgMsg);
}

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
U16 TcgRangeCheck(U32 lbaStart, U32 sc, BOOL writemode)
{
    if (mTcgStatus & TCG_ACTIVATED) //TCG is activated
    {
        BOOL   startChecked=FALSE, globalChecked=FALSE;
        U32    i;
        U32    lbaEnd;

        lbaEnd = lbaStart + sc - 1;
        if (mTcgStatus & MBR_SHADOW_MODE)
        {
            if (lbaEnd >= 0x40000) //TODO: data '0' for read
            {
                if(lbaStart < 0x40000)
                {
                    soutb3(0x5C, 0xFE, 0x2B);
                    TCG_PRINTF("!!MS X\n");
                    return TCG_DOMAIN_ERROR;
                }
                //else
                //{
                //    if(!writemode)
                //    {     return TCG_DOMAIN_DUMMY;    }
                //}
            }
            else //<128M
            {
                if(writemode)
                {
                    soutb3(0x5C, 0xFE, 0x28);   //5C FE 28,    "!!NG: MBR-S Write!!!"
                    TCG_PRINTF("!!MS W\n");
                    return TCG_DOMAIN_ERROR;
                }
                else
                {
                    soutb3(0x5C, 0xFE, 0x29);   //5C FE 29,    "*** MBR-S Read!!!"
                    TCG_PRINTF("**MS\n");
                    return TCG_DOMAIN_SHADOW;
                }
            }
        }


        for (i=0; i <=LOCKING_RANGE_CNT; i++)
        {
            if(mLockingRangeTable[i].rangeNo == 0)
            { //Global Range: last row
                if(globalChecked==FALSE)
                {
                    if(((!writemode)&&mLockingRangeTable[LOCKING_RANGE_CNT].readLocked) || (writemode&&mLockingRangeTable[LOCKING_RANGE_CNT].writeLocked))
                    { //range is read / write locked!
                        soutb3(0x5C, 0xFE, 0xE8);  //5C FE E8,    "*** R%02X Locked!!!!!" 1
                        soutb(mLockingRangeTable[LOCKING_RANGE_CNT].rangeNo);   //Locked!
                        TCG_PRINTF("!! R0 Lck\n");
                        if ((!writemode) && (mTcgStatus & MBR_SHADOW_MODE)){
                            return TCG_DOMAIN_DUMMY;
                        }
                        return TCG_DOMAIN_ERROR;
                    }
                    else
                        return TCG_DOMAIN_NORMAL;
                }
                else
                    return TCG_DOMAIN_NORMAL;
            }


            if(startChecked==FALSE)
            { // find startLBA range first
                if(lbaStart<=mLockingRangeTable[i].rangeEnd)
                { //startLBA range is found!
                    startChecked=TRUE;

                    if(lbaStart>=mLockingRangeTable[i].rangeStart)
                    { //startLBA at Tbl[i]
                        if(((!writemode)&&mLockingRangeTable[i].readLocked)|| (writemode&&mLockingRangeTable[i].writeLocked))
                        { //range is read / write locked!
                            soutb3(0x5C, 0xFE, 0xE8);  //5C FE E8,    "*** R%02X Locked!!!!!" 1
                            soutb(mLockingRangeTable[i].rangeNo);   //Locked!
                            TCG_PRINTF("!! R%02X Lck\n", mLockingRangeTable[i].rangeNo);
                            if ((!writemode) && (mTcgStatus & MBR_SHADOW_MODE)){
                                return TCG_DOMAIN_DUMMY;
                            }
                            return TCG_DOMAIN_ERROR;
                        }
                    }

                    else
                    { //startLBA @ global range
                        globalChecked=TRUE;
                        if(((!writemode)&&mLockingRangeTable[LOCKING_RANGE_CNT].readLocked)|| (writemode&&mLockingRangeTable[LOCKING_RANGE_CNT].writeLocked))
                        { //range is read / write locked!
                            soutb3(0x5C, 0xFE, 0xE8);  //5C FE E8,    "*** R%02X Locked!!!!!" 1
                            soutb(mLockingRangeTable[LOCKING_RANGE_CNT].rangeNo);   //Locked!
                            TCG_PRINTF("!! R00 Lck\n");
                            return TCG_DOMAIN_ERROR;
                        }
                    }

                    //check if lbaEnd is at this range or not
                    if(lbaEnd<=mLockingRangeTable[i].rangeEnd)
                    { // endLBA range is found!
                        //endChecked = TRUE;

                        if(lbaEnd>=mLockingRangeTable[i].rangeStart)
                        { //endLBA at Tbl[i]
                            if(globalChecked==TRUE)
                            { // startLBA is at global range
                                if(((!writemode)&&mLockingRangeTable[i].readLocked)|| (writemode&&mLockingRangeTable[i].writeLocked))
                                { //range is read / write locked!
                                    soutb3(0x5C, 0xFE, 0xE8);  //5C FE E8,    "*** R%02X Locked!!!!!" 1
                                    soutb(mLockingRangeTable[i].rangeNo);   //Locked!

                                    TCG_PRINTF("!! R%02X Lck\n", mLockingRangeTable[i].rangeNo);
                                    return TCG_DOMAIN_ERROR;
                                }
                                else
                                    return TCG_DOMAIN_NORMAL;

                            }
                            //else startLBA is at Tbl[i] (already checked)
                        }
                        //else    endLBA at global range (already checked)

                        return TCG_DOMAIN_NORMAL;
                    }
                }
            }

#if 1 //def TCG_EDRIVE
            else //if(endChecked==FALSE)
            { // find endLBA range
                if(globalChecked==FALSE)
                { // check if there is gap between ranges...
                    if(mLockingRangeTable[i].rangeStart!=(mLockingRangeTable[i-1].rangeEnd+1))
                    {
                        globalChecked=TRUE;
                        if(((!writemode)&&mLockingRangeTable[LOCKING_RANGE_CNT].readLocked)|| (writemode&&mLockingRangeTable[LOCKING_RANGE_CNT].writeLocked))
                        { //range is read / write locked!
                            soutb3(0x5C, 0xFE, 0xE8);  //5C FE E8,    "*** R%02X Locked!!!!!" 1
                            soutb(mLockingRangeTable[LOCKING_RANGE_CNT].rangeNo);   //Locked!
                            TCG_PRINTF("!! R00 Lck\n");
                            return TCG_DOMAIN_ERROR;
                        }
                    }
                }

                if(lbaEnd>=mLockingRangeTable[i].rangeStart)
                { // endLBA  passed Tbl[i]
                    if(((!writemode)&&mLockingRangeTable[i].readLocked)|| (writemode&&mLockingRangeTable[i].writeLocked))
                    { //range is read / write locked!
                        soutb3(0x5C, 0xFE, 0xE8);  //5C FE E8,    "*** R%02X Locked!!!!!" 1
                        soutb(mLockingRangeTable[i].rangeNo);   //Locked!
                        TCG_PRINTF("!! R%02X Lck\n", mLockingRangeTable[i].rangeNo);
                        return TCG_DOMAIN_ERROR;
                    }

                    if(lbaEnd<=mLockingRangeTable[i].rangeEnd)
                    {
                        //endChecked=TRUE;
                        return TCG_DOMAIN_NORMAL;
                    }

                }
            }
#endif
        }
    }

    return TCG_DOMAIN_NORMAL;

}

//
//
//
#include "TcgSection.h"    // CODE AND DATA MEMORY MAPPING
void TcgReadNormalTransferDone(HcmdQ_t* pHcmdQ)
{
    MsgHostIO_t* pHcmdMsg;

    A_HERE;
    pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);

    pHcmdMsg->hdr.b.status = cMsgCompleted;

    BcmMaesEnable();    //Enable AES again
}

void TCG_PrepareHcmdMsg(HcmdQ_t* pHcmdQ, MsgHostIO_t* pHcmdMsg)
{
    uint32_t   groupId;
    uint32_t   bitmap;
    XferAttr_t attrInfo;
    uint16_t   cmdIndex;

    // HLD_HOST Code
    cmdIndex = GET_COMMAND_INDEX(GetHcmdId(pHcmdQ));
    groupId  = cmdIndex >> 5;
    bitmap   = (1 << (cmdIndex & 0x1F));

    attrInfo.all         = 0;
    attrInfo.b.cmdId     = GetHcmdId(pHcmdQ);
    attrInfo.b.direction = pHcmdQ->attr.b.xferDir;
    attrInfo.b.autoEoC   = pHcmdQ->attr.b.autoEoC;

    SET_IO_XFER_PARAM_FROM_INDEX(cmdIndex, (uint32_t) pHcmdQ);
    SET_IO_ATTR_FROM_INDEX(cmdIndex, attrInfo.all);

    MSG_SET_HOST_INIT(pHcmdMsg, pHcmdQ->attr.b.xferDir);

    if (pHcmdQ->attr.b.xferDir == cXferH2d)
    {
        #if (SUP_DIRE_STREAMS == ENABLE)
        Directive_WriteHandling(pHcmdMsg, pHcmdQ->addrInfo.nsId0);
        #endif
    }

    if (gNvmeIfMgr.ns[pHcmdQ->addrInfo.nsId0].hbu_shift == 0)
    {
        pHcmdMsg->bufDesc.laaStartBmp = gSLaaBitMask[pHcmdQ->addrInfo.lba & HBU_PER_MBU_MASK];
        pHcmdMsg->bufDesc.laaEndBmp   = gELaaBitMask[(pHcmdQ->addrInfo.lba + pHcmdQ->addrInfo.nb - 1) & HBU_PER_MBU_MASK];
        pHcmdMsg->laa                 =   pHcmdQ->addrInfo.lba                                           >> HBU_PER_MBU_SFT;
        pHcmdMsg->bufDesc.ssdCnt      = ((pHcmdQ->addrInfo.lba + pHcmdQ->addrInfo.nb + HBU_PER_MBU_MASK) >> HBU_PER_MBU_SFT) - pHcmdMsg->laa;

        if (pHcmdMsg->bufDesc.ssdCnt == 1) pHcmdMsg->bufDesc.laaStartBmp &= pHcmdMsg->bufDesc.laaEndBmp;
    }
    else
    {
        pHcmdMsg->bufDesc.laaStartBmp = pHcmdMsg->bufDesc.laaEndBmp = CACHE_BMP_ALL_VALID;
        pHcmdMsg->laa                 = pHcmdQ->addrInfo.lba;
        pHcmdMsg->bufDesc.ssdCnt      = pHcmdQ->addrInfo.nb;
    }

    pHcmdMsg->bufDesc.allFlags = 0;
    pHcmdMsg->attrInfo.all  = attrInfo.all;

    // Set a bitmap whether a command is transferable.
    NvmeHba.bitmapXferable[groupId] |= bitmap;
    // Set a Cabm fw bitmaps to check transfer done or command completion
    NvmeHba.bitmapFwCabm[groupId]   |= bitmap;
    NvmeHba.bitmapFwCabmGroup       |= BIT(groupId);
}

///*********************************************
/// ATA read command read SMBR
///*********************************************
void TCG_ReadNormal(HcmdQ_t* pHcmdQ, U16 RngSts)
{
    MsgHostIO_t* pHcmdMsg;     // IO cmd Msg, because follow IO read cmd.
    MsgTcg_t*    pTcgMsg;      // TCG Msg
    uint32_t     slaa, elaa;
    uint16_t     offset_16K_2_4K;
    uint16_t     ssdIndex;
    uint16_t     i;

    pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);
    SetHcmdHandler(pHcmdQ, TcgReadNormalTransferDone);                // register transfer done subroutinue.
    TCG_PrepareHcmdMsg(pHcmdQ, pHcmdMsg);

    RdTcgMBRbyNormalRdCmd = TRUE;
    slaa = pHcmdQ->addrInfo.lba >> 5;                                 // slaa = lba/32
    elaa = (pHcmdQ->addrInfo.lba + pHcmdQ->addrInfo.nb + 31) >> 5;    // elaa = (lba+lbaCnt+31)/32
    alexprn2(slaa, elaa);
    A_LANDMARK(RngSts);
    if(RngSts == TCG_DOMAIN_DUMMY){   // TCG_DOMAIN_DUMMY
        if(TcgH2cJobIssue(MSG_TCG_G4DMYRD, NULL) == zNG){
            pHcmdQ->error = cEcNfifGeneral;
            return;
        }
    }else{
        if(SMBR_Read(slaa, elaa, TcgTmpBuf, &pTcgMsg) == zNG){
            pHcmdQ->error = cEcNfifGeneral;
            return;
        }
    }

    offset_16K_2_4K = (pHcmdQ->addrInfo.lba >> 3) & 0x00000003;
    ssdIndex        = pTcgMsg->ssdIndex;
    for(i = 0; i < offset_16K_2_4K; i++){
        ssdIndex = GET_SSD_NEXT_INDEX_FROM_INDEX(ssdIndex);
    }

    pHcmdMsg->bufDesc.ssdIndex = ssdIndex;

    BcmMaesDisable();   //Disable AES for MBR-S transfering
    HalNvme_RequestDataTransferIoQuick(pHcmdMsg);
}

void TcgEepBackupBridge(void)
{
    MEM_COPY(TcgTmpBuf, &tcgEepProgIdx, sizeof(smSysInfo->d.TcgData.d.TCGUsed));
}

void TcgEepRestoreBridge(void)
{
    MEM_COPY(&tcgEepProgIdx, TcgTmpBuf, sizeof(smSysInfo->d.TcgData.d.TCGUsed));
}

void TcgPsidBackupBridge(void)
{
    //if(mRst){
    //    for(j=0; j<PWD_LENGTH; j++)
    //    tmp[j] = G1.b.mAdmCPin_Tbl.val[CPIN_PSID_IDX].pwd[j];
    //}
    MEM_COPY((U8 *)TcgTmpBuf+sizeof(smSysInfo->d.TcgData.d.TCGUsed), (U8 *)&G1.b.mAdmCPin_Tbl.val[CPIN_PSID_IDX].pwd[0], PWD_LENGTH);
}

void TcgPsidRestoreBridge(void)
{
    MEM_COPY((U8 *)&G1.b.mAdmCPin_Tbl.val[CPIN_PSID_IDX].pwd[0], (U8 *)TcgTmpBuf+sizeof(smSysInfo->d.TcgData.d.TCGUsed), PWD_LENGTH);
}


void TcgEepOperatedBridge(U32 Metholdtype)
{
#ifdef TCG_EEP_NOR
    switch(Metholdtype){
        case 0:
            TcgH2cJobIssue(MSG_TCG_NOREEP_INIT, NULL);
        break;
        case 1:
            TcgH2cJobIssue(MSG_TCG_NOREEP_RD, NULL);
        break;
        case 2:
            TcgH2cJobIssue(MSG_TCG_NOREEP_WR, NULL);
        break;
    }

#endif
}

void TcgKeepGroupDataBridge(U32 GrpNum)
{
    switch(GrpNum){
        case 1:
            TcgH2cJobIssue(MSG_TCG_G1RDDEFAULT, NULL);
        break;
        case 2:
            TcgH2cJobIssue(MSG_TCG_G2RDDEFAULT, NULL);
        break;
        case 3:
            TcgH2cJobIssue(MSG_TCG_G3RDDEFAULT, NULL);
        break;
        case 4:
            TcgH2cJobIssue(MSG_TCG_G4RDDEFAULT, NULL);
        break;
        default:
        break;
    }
}

U32 IsG1Blank(void)
{
    return ((G4.b.TcgMbrL2P[TCG_G1_LAA0].pc.cell >= TCG_MBR_CELLS)
     && (G5.b.TcgTempMbrL2P[TCG_G1_LAA0].pc.cell >= TCG_MBR_CELLS));
}


U32 IsG2Blank(void)
{
    return ((G4.b.TcgMbrL2P[TCG_G2_LAA0].pc.cell >= TCG_MBR_CELLS)
         && (G4.b.TcgMbrL2P[TCG_G2_LAA1].pc.cell >= TCG_MBR_CELLS)
     && (G5.b.TcgTempMbrL2P[TCG_G2_LAA0].pc.cell >= TCG_MBR_CELLS)
     && (G5.b.TcgTempMbrL2P[TCG_G2_LAA1].pc.cell >= TCG_MBR_CELLS));
}


U32 IsG3Blank(void)
{
    return((G4.b.TcgMbrL2P[TCG_G3_LAA0].pc.cell >= TCG_MBR_CELLS)
        && (G4.b.TcgMbrL2P[TCG_G3_LAA1].pc.cell >= TCG_MBR_CELLS)
    && (G5.b.TcgTempMbrL2P[TCG_G3_LAA0].pc.cell >= TCG_MBR_CELLS)
    && (G5.b.TcgTempMbrL2P[TCG_G3_LAA1].pc.cell >= TCG_MBR_CELLS));
}

U32 IsDSBlank(void)
{
    return((G4.b.TcgMbrL2P[TCG_DS_LAA_START].pc.cell >= TCG_MBR_CELLS));
}


U32 IsG1G2G3DSAllBlank(void)
{
    return(IsG1Blank() && IsG2Blank() && IsG3Blank() && IsDSBlank());
}

U32 IsG1G2G3OneOfBlank(void)
{
    return(IsG1Blank() || IsG2Blank() || IsG3Blank());
}

int TCG_NewTable(BOOL mRst, U8* tmp)
{
    int j;

    TCG_PRINTF("w1\n");
    TcgH2cJobIssue(MSG_TCG_G1RDDEFAULT, NULL);       //cTbl_2_mTbl_byGrp(GRP_1);

    // copy Hash(MSID) to SID
    CopyHashed(&G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[1], G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[0], G1.b.mAdmCPin_Tbl.val[CPIN_SID_IDX].pwd);

    if(mRst){ //restore PSID if warm boot
        for(j=0; j<PWD_LENGTH; j++){
            G1.b.mAdmCPin_Tbl.val[CPIN_PSID_IDX].pwd[j] = tmp[j];
        }
        for(j=0; j<9; j++){
            D_PRINTF("%02X",G1.b.mAdmCPin_Tbl.val[CPIN_PSID_IDX].pwd[j]);
        }
        D_PRINTF("<- restore PSID\n");
    }
#if 0
    else
    {   // copy Hash(MSID) to PSID
        CopyHashed(&G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[1], G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[0], G1.b.mAdmCPin_Tbl.val[CPIN_PSID_IDX].pwd);
        for(j=0; j<PWD_LENGTH; j++)
            D_PRINTF("%02X",G1.b.mAdmCPin_Tbl.val[CPIN_PSID_IDX].pwd[j]);
        D_PRINTF("<- MSID 2 PSID\n");
    }
#endif
    TcgH2cJobIssue(MSG_TCG_G1WR, NULL);   //RW_WaitG1Wr();

    //----------------------------------------------------------------
    TCG_PRINTF("w2\n");
    TcgH2cJobIssue(MSG_TCG_G2RDDEFAULT, NULL);   //cTbl_2_mTbl_byGrp(GRP_2);
    TcgH2cJobIssue(MSG_TCG_G2WR, NULL);          //RW_WaitG2Wr();

    //----------------------------------------------------------------
    TCG_PRINTF("w3\n");
    TcgH2cJobIssue(MSG_TCG_G3RDDEFAULT, NULL);   //cTbl_2_mTbl_byGrp(GRP_3);

    //generate AES keys
    for(j=0;j<=LOCKING_RANGE_CNT;j++)
        ChangeKey(j);
    TcgH2cJobIssue(MSG_TCG_G3WR, NULL);          //RW_WaitG3Wr();
    TcgH2cJobIssue(MSG_TCG_SMBRCLEAR, NULL);     //RW_WaitMbrClr();

    //----------------------------------------------------------------
    TCG_PRINTF("w6\n");
    TcgH2cJobIssue(MSG_TCG_DSCLEAR, NULL);       //RW_WaitDSClr();
    TcgH2cJobIssue(MSG_TCG_G4DMYWR, NULL);

    return zOK;
}

int TCG_BuildTable(void)
{
    int j;

    TCG_PRINTF("TCG_BuildTable()\n");
    //read G1
    if(TcgH2cJobIssue(MSG_TCG_G1RD, NULL) == zNG) return zNG;   //RW_WaitG1Rd();

    for(j=0; j<5; j++)
        D_PRINTF("%02X",G1.b.mAdmCPin_Tbl.val[CPIN_SID_IDX].pwd[j]);
    D_PRINTF(" <- SID\n");

    for(j=0; j<5; j++)
        D_PRINTF("%02X",G1.b.mAdmCPin_Tbl.val[CPIN_PSID_IDX].pwd[j]);
    D_PRINTF(" <- PSID\n");

    //read G2
    if(TcgH2cJobIssue(MSG_TCG_G2RD, NULL) == zNG) return zNG;    //RW_WaitG2Rd();

    //read G3
    if(TcgH2cJobIssue(MSG_TCG_G3RD, NULL) == zNG) return zNG;    //RW_WaitG3Rd();

    return zOK;
}

int ChkDefaultTblPattern(void)
{
    if(G1.b.mTcgTblInfo.ID  != TCG_TBL_ID)                 return zNG;
    if(G1.b.mTcgTblInfo.ver != TCG_TBL_VER)                return zNG;
    if(G2.b.mLckSPInfo_Tbl.val[0].uid  != UID_SPInfo)      return zNG;
    if(G2.b.mLckSPInfo_Tbl.val[0].spid != UID_SP_Locking)  return zNG;
    if(G3.b.mLckACE_Tbl.val[0].uid     != UID_ACE_Anybody) return zNG;
    if(G3.b.mLckACE_Tbl.val[1].uid     != UID_ACE_Admin)   return zNG;
    TCG_PRINTF("Default Tbl OK\n");
    return zOK;
}


void DumpG4ErasedCount(void)
{
#if 0
    U8 i,j;

    for(i = 0; i < TCG_MBR_CELLS / 16; i++){
        soutb3(0x5C, 0xFF, 0x37); soutb(i * 16);
        for(j = 0; j < 16; j++){
            if(tcgG4Dft[i * 16 + j] != 0){      // is defect block ?
                soutw(0xFFFF);                          // show 0xFFFF if it is defect block
            }else{
                soutw(tcgG4EraCnt[i * 16 + j]);   // show erased count
            }
        }
    }
    DumpG4DftAmount();
    soutb3(0xCA, 0xA1, 0x23);  // CA A1 23,       "----------------------"
#else
    U8 i,j;

    for(i = 0; i < TCG_MBR_CELLS / 16; i++){
        TCG_PRINTF("G4 EraCnt=%2X :", i*16);
        for(j = 0; j < 16; j++){
            if(j == 8) TCG_PRINTF("- ");          //
            if(tcgG4Dft[i * 16 + j] != 0){      // is defect block ?
                TCG_PRINTF("FFFF ");                          // show 0xFFFF if it is defect block
            }else{
                TCG_PRINTF("%4X ", tcgG4EraCnt[i * 16 + j]);   // show erased count
            }
        }
        TCG_PRINTF("\n");
    }
    DumpG4DftAmount();
    TCG_PRINTF("----------------------\n");  // CA A1 23,       "----------------------"
#endif
}

void DumpG5ErasedCount(void)
{
#if 0

    U8 i,j;

    for(i = 0; i < TCG_MBR_CELLS / 16; i++){
        soutb3(0x5C, 0xFF, 0x39); soutb(i * 16);
        for(j = 0; j < 16; j++){
            if(tcgG5Dft[i * 16 + j] != 0){  // is defect block ?
                soutw(0xFFFF);                          // show 0xFFFF if it is defect block
            }else{
                soutw(tcgG5EraCnt[i * 16 + j]);   // show erased count
            }
        }
    }
    DumpG5DftAmount();
    soutb3(0xCA, 0xA1, 0x23);  // CA A1 23,       "----------------------"
#else
    U8 i,j;

    for(i = 0; i < TCG_MBR_CELLS / 16; i++){
        TCG_PRINTF("G5 EraCnt=%2X :", i*16);
        for(j = 0; j < 16; j++){
            if(j == 8) TCG_PRINTF("- ");          //
            if(tcgG5Dft[i * 16 + j] != 0){      // is defect block ?
                TCG_PRINTF("FFFF ");                          // show 0xFFFF if it is defect block
            }else{
                TCG_PRINTF("%4X ", tcgG5EraCnt[i * 16 + j]);   // show erased count
            }
        }
        TCG_PRINTF("\n");
    }
    DumpG5DftAmount();
    TCG_PRINTF("----------------------\n");  // CA A1 23,       "----------------------"
#endif
}

void DumpG4DftAmount(void)
{
    U8 i;
    U8 amount = 0;

    for(i = 0; i < TCG_MBR_CELLS; i++){
        if(tcgG4Dft[i] != 0) amount++;
    }
    TcgG4Defects = amount;
    TCG_PRINTF(">> G4 defect amount= %2X\n", amount);  //soutb3(0x5C, 0xFF, 0x3C); soutb(amount);  //5C FF 3C,    "==> G4 defect amount = %02X" 1
}

void DumpG5DftAmount(void)
{
    U8 i;
    U8 amount = 0;

    for(i = 0; i < TCG_MBR_CELLS; i++){
        if(tcgG5Dft[i] != 0) amount++;
    }
    TcgG5Defects = amount;
    TCG_PRINTF(">> G5 defect amount= %2X\n", amount);  //soutb3(0x5C, 0xFF, 0x3E); soutb(amount);  //5C FF 3E,    "==> G4 defect amount = %02X" 1
}


void DumpRangeInfo(void)
{
    U16 i;
    soutb3(0x5C, 0xFE, 0xE4); soutw(mReadLockedStatus); soutw(mWriteLockedStatus);
    TCG_PRINTF("RdLck: %4X, WrLck: %4X, tcgStatus: %2X\n", mReadLockedStatus, mWriteLockedStatus, mTcgStatus);
    for(i=0; i<LOCKING_RANGE_CNT+1; i++)
{
        soutb3(0x5C, 0xFE, 0xE6);
        soutb3(mLockingRangeTable[i].rangeNo, mLockingRangeTable[i].readLocked, mLockingRangeTable[i].writeLocked);
        sout64(mLockingRangeTable[i].rangeStart);
        sout64(mLockingRangeTable[i].rangeEnd);

        TCG_PRINTF("#%2X R:%2X W:%2X *%X ~ &%X %X\n", mLockingRangeTable[i].rangeNo, mLockingRangeTable[i].readLocked,
                                                   mLockingRangeTable[i].writeLocked, (U32)mLockingRangeTable[i].rangeStart,
                                                   (U32)mLockingRangeTable[i].rangeEnd, mLockingRangeTable[i].blkcnt);

        if(mLockingRangeTable[i].rangeNo==0x00)
            break;
        }
    }

void DumpTcgEepInfo(void)
{

    TCG_PRINTF("M table IdText[%s] Id[%X], VerText[%s] Ver[%X]\n", tcg_mTbl_idStr, tcg_mTbl_id, tcg_mTbl_verStr, tcg_mTbl_ver);
    TCG_PRINTF("C table IdText[%s] Id[%X], VerText[%s] Ver[%X]\n", tcg_cTbl_idStr, tcg_cTbl_id, tcg_cTbl_verStr, tcg_cTbl_ver);

    TCG_PRINTF("FW Dft Tag[%s], EEP Def Tag[%s]\n", DEFECT_STRING, (char *)tcgDefectID);
    TCG_PRINTF("FW EraCnt Tag[%s], EEP EraCnt Tag[%s]\n", ERASED_CNT_STRING, (char *)tcgErasedCntID);
    TCG_PRINTF("EEP Prefmt Tag[%s]\n", tcg_prefmt_tag);
}

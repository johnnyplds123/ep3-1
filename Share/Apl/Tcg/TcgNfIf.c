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

#include "Ftl.h"
#include "Cache.h"
#include "HalNfif.h"
#include "SysInfo.h"
#include "Fio.h"

#include "TcgSection.h"    // CODE AND DATA MEMORY MAPPING
#include "TcgCommon.h"
#include "TcgNfIf.h"
#include "TcgNfRdWr.h"

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

typedef int (*TCGMsgHandler_t)(MsgTcg_t*);

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

static int TCGNFIF_G1Rd(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G1Wr(MsgTcg_t* pTcgMsg);
static int TCGNFIF_TG1Rd(MsgTcg_t* pTcgMsg);
static int TCGNFIF_TG1Wr(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G2Rd(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G2Wr(MsgTcg_t* pTcgMsg);
static int TCGNFIF_TG2Rd(MsgTcg_t* pTcgMsg);
static int TCGNFIF_TG2Wr(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G3Rd(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G3Wr(MsgTcg_t* pTcgMsg);
static int TCGNFIF_TG3Rd(MsgTcg_t* pTcgMsg);
static int TCGNFIF_TG3Wr(MsgTcg_t* pTcgMsg);
static int TCGNFIF_SMBRRd(MsgTcg_t* pTcgMsg);
static int TCGNFIF_SMBRWr(MsgTcg_t* pTcgMsg);
static int TCGNFIF_SMBRCommit(MsgTcg_t* pTcgMsg);
static int TCGNFIF_SMBRClear(MsgTcg_t* pTcgMsg);
static int TCGNFIF_TSMBRClear(MsgTcg_t* pTcgMsg);
static int TCGNFIF_DSRd(MsgTcg_t* pTcgMsg);
static int TCGNFIF_DSWr(MsgTcg_t* pTcgMsg);
static int TCGNFIF_DSCommit(MsgTcg_t* pTcgMsg);
static int TCGNFIF_DSClear(MsgTcg_t* pTcgMsg);
static int TCGNFIF_TDSClear(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G5FTL(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G4FTL(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G1RdDefault(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G2RdDefault(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G3RdDefault(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G5RdDefault(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G4RdDefault(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G5WrDefault(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G4WrDefault(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G5BuildDefect(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G4BuildDefect(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G5DmyRd(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G4DmyRd(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G5DmyWr(MsgTcg_t* pTcgMsg);
static int TCGNFIF_G4DmyWr(MsgTcg_t* pTcgMsg);
static int TCGNFIF_SETGrpAddr(MsgTcgGroup_t* pTcgMsg);
static int TCGNFIF_SETBufAddr(MsgTcgBuffer_t* pTcgMsg);
static int TCGNFIF_GxRd(MsgTcg_t* pTcgMsg);
static int TCGNFIF_CLRCache(MsgTcg_t* pTcgMsg);
static int TCGNFIF_NorEepInit(MsgTcg_t* pTcgMsg);
static int TCGNFIF_NorEepRd(MsgTcg_t* pTcgMsg);
static int TCGNFIF_NorEepWr(MsgTcg_t* pTcgMsg);
static int G4RdRetry(void);
static int G5RdRetry(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
#include "FastSection.h"
U16 TCG_FIO_F_MUTE = 0;

#include "TcgSection.h"    // CODE AND DATA MEMORY MAPPING

static TCGMsgHandler_t gTCGProcNfIfMsgTable[MSG_TCG_NFIF_LAST] =
{
    (TCGMsgHandler_t)TCGNFIF_G1Rd,
    (TCGMsgHandler_t)TCGNFIF_G1Wr,
    (TCGMsgHandler_t)TCGNFIF_TG1Rd,
    (TCGMsgHandler_t)TCGNFIF_TG1Wr,
    (TCGMsgHandler_t)TCGNFIF_G2Rd,
    (TCGMsgHandler_t)TCGNFIF_G2Wr,
    (TCGMsgHandler_t)TCGNFIF_TG2Rd,
    (TCGMsgHandler_t)TCGNFIF_TG2Wr,
    (TCGMsgHandler_t)TCGNFIF_G3Rd,
    (TCGMsgHandler_t)TCGNFIF_G3Wr,
    (TCGMsgHandler_t)TCGNFIF_TG3Rd,
    (TCGMsgHandler_t)TCGNFIF_TG3Wr,
    (TCGMsgHandler_t)TCGNFIF_SMBRRd,
    (TCGMsgHandler_t)TCGNFIF_SMBRWr,
    (TCGMsgHandler_t)TCGNFIF_SMBRCommit,
    (TCGMsgHandler_t)TCGNFIF_SMBRClear,
    (TCGMsgHandler_t)TCGNFIF_TSMBRClear,
    (TCGMsgHandler_t)TCGNFIF_DSRd,
    (TCGMsgHandler_t)TCGNFIF_DSWr,
    (TCGMsgHandler_t)TCGNFIF_DSCommit,
    (TCGMsgHandler_t)TCGNFIF_DSClear,
    (TCGMsgHandler_t)TCGNFIF_TDSClear,
    (TCGMsgHandler_t)TCGNFIF_G5FTL,
    (TCGMsgHandler_t)TCGNFIF_G4FTL,
    (TCGMsgHandler_t)TCGNFIF_G1RdDefault,
    (TCGMsgHandler_t)TCGNFIF_G2RdDefault,
    (TCGMsgHandler_t)TCGNFIF_G3RdDefault,
    (TCGMsgHandler_t)TCGNFIF_G5RdDefault,
    (TCGMsgHandler_t)TCGNFIF_G4RdDefault,
    (TCGMsgHandler_t)TCGNFIF_G5WrDefault,
    (TCGMsgHandler_t)TCGNFIF_G4WrDefault,
    (TCGMsgHandler_t)TCGNFIF_G5BuildDefect,
    (TCGMsgHandler_t)TCGNFIF_G4BuildDefect,
    (TCGMsgHandler_t)TCGNFIF_G5DmyRd,
    (TCGMsgHandler_t)TCGNFIF_G4DmyRd,
    (TCGMsgHandler_t)TCGNFIF_G5DmyWr,
    (TCGMsgHandler_t)TCGNFIF_G4DmyWr,
    (TCGMsgHandler_t)TCGNFIF_SETGrpAddr,
    (TCGMsgHandler_t)TCGNFIF_SETBufAddr,
    (TCGMsgHandler_t)TCGNFIF_GxRd,
    (TCGMsgHandler_t)TCGNFIF_CLRCache,
    (TCGMsgHandler_t)TCGNFIF_CLRCache,
    (TCGMsgHandler_t)TCGNFIF_NorEepInit,
    (TCGMsgHandler_t)TCGNFIF_NorEepRd,
    (TCGMsgHandler_t)TCGNFIF_NorEepWr,
};

tG1                 *pG1;
tG2                 *pG2;
tG3                 *pG3;
tG4                 *pG4;
tG5                 *pG5;
U8                  *pTcgTmpBuf;
tTcgLogAddr         *pMBR_TEMPL2PTBL;
tTcgLogAddr         *pDS_TEMPL2PTBL;
sEnabledLockingTable *pLockingRangeTable;

U32                 FirstTx_ssdStartIdx_Record_flag = 0x0;      // get first write ssdStartIdx flag;


//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
extern tFIO_JOB* pTcgJob;

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
int TCGNFIF_Start(MsgTcg_t* pTcgMsg)
{
    A_HERE;
    if( gTCGProcNfIfMsgTable[pTcgMsg->hdr.b.subOpCode](pTcgMsg) == zNG ){
        pTcgMsg->error = cEcMioGeneral;
        return zNG;
    }else{
        pTcgMsg->error = cEcNoError;
        return zOK;
    }
}

//-------------------------------------------------
//  Group1 read
//-------------------------------------------------
static int TCGNFIF_G1Rd(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G1_LAA0;
    pTcgMsg->laae = TCG_G1_LAA0 + 1;
    pTcgMsg->pBuffer = &pG1->all[0];

    if(TCG_G4Rd(pTcgMsg) == zNG){
        if(TCG_G5Rd(pTcgMsg) == zNG){
            return zNG;   // if read retry failure
        }else{
            TCG_G4Wr(pTcgMsg);
        }
    }
    return zOK;
}

//-------------------------------------------------
//  Group1 write
//-------------------------------------------------
static int TCGNFIF_G1Wr(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);

    pTcgMsg->laas = TCG_G1_LAA0;
    pTcgMsg->laae = TCG_G1_LAA0 + 1;
    pTcgMsg->pBuffer = &pG1->all[0];
    if(TCG_G5Wr(pTcgMsg) == zNG) return zNG;
    return TCG_G4Wr(pTcgMsg);
}

//-------------------------------------------------
//  Temp group1 read
//-------------------------------------------------
static int TCGNFIF_TG1Rd(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G1_LAA0;
    pTcgMsg->laae = TCG_G1_LAA0 + 1;
    pTcgMsg->pBuffer = &pG1->all[0];

    return TCG_G5Rd(pTcgMsg);
}

//-------------------------------------------------
//  Temp group1 write
//-------------------------------------------------
static int TCGNFIF_TG1Wr(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G1_LAA0;
    pTcgMsg->laae = TCG_G1_LAA0 + 1;
    pTcgMsg->pBuffer = &pG1->all[0];

    return TCG_G5Wr(pTcgMsg);
}

//-------------------------------------------------
//  Group2 read
//-------------------------------------------------
static int TCGNFIF_G2Rd(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G2_LAA0;
    pTcgMsg->laae = TCG_G2_LAA1 + 1;
    pTcgMsg->pBuffer = &pG2->all[0];
    if(TCG_G4Rd(pTcgMsg) == zNG){
        if(TCG_G5Rd(pTcgMsg) == zNG){
            return zNG;   // if read retry failure
        }else{
            TCG_G4Wr(pTcgMsg);
        }
    }
    return zOK;
}

//-------------------------------------------------
//  Group2 write
//-------------------------------------------------
static int TCGNFIF_G2Wr(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G2_LAA0;
    pTcgMsg->laae = TCG_G2_LAA1 + 1;
    pTcgMsg->pBuffer = &pG2->all[0];
    if(TCG_G5Wr(pTcgMsg) == zNG) return zNG;
    return TCG_G4Wr(pTcgMsg);
}

//-------------------------------------------------
//  Temp group2 read
//-------------------------------------------------
static int TCGNFIF_TG2Rd(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G2_LAA0;
    pTcgMsg->laae = TCG_G2_LAA1 + 1;
    pTcgMsg->pBuffer = &pG2->all[0];
    return TCG_G5Rd(pTcgMsg);
}

//-------------------------------------------------
//  Temp group2 write
//-------------------------------------------------
static int TCGNFIF_TG2Wr(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G2_LAA0;
    pTcgMsg->laae = TCG_G2_LAA1 + 1;
    pTcgMsg->pBuffer = &pG2->all[0];

    return TCG_G5Wr(pTcgMsg);
}

//-------------------------------------------------
//  Group3 read
//-------------------------------------------------
static int TCGNFIF_G3Rd(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G3_LAA0;
    pTcgMsg->laae = TCG_G3_LAA1 + 1;
    pTcgMsg->pBuffer = &pG3->all[0];
    if(TCG_G4Rd(pTcgMsg) == zNG){
        if(TCG_G5Rd(pTcgMsg) == zNG){
            return zNG;   // if read retry failure
        }else{
            TCG_G4Wr(pTcgMsg);
        }
    }
    return zOK;
}

//-------------------------------------------------
//  Group3 write
//-------------------------------------------------
static int TCGNFIF_G3Wr(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G3_LAA0;
    pTcgMsg->laae = TCG_G3_LAA1 + 1;
    pTcgMsg->pBuffer = &pG3->all[0];
    if(TCG_G5Wr(pTcgMsg) == zNG) return zNG;
    return TCG_G4Wr(pTcgMsg);
}

//-------------------------------------------------
//  Temp group3 read
//-------------------------------------------------
static int TCGNFIF_TG3Rd(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G3_LAA0;
    pTcgMsg->laae = TCG_G3_LAA1 + 1;
    pTcgMsg->pBuffer = &pG3->all[0];
    return TCG_G5Rd(pTcgMsg);
}

//-------------------------------------------------
//  Temp Group3 write
//-------------------------------------------------
static int TCGNFIF_TG3Wr(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G3_LAA0;
    pTcgMsg->laae = TCG_G3_LAA1 + 1;
    pTcgMsg->pBuffer = &pG3->all[0];

    return TCG_G5Wr(pTcgMsg);
}

//-------------------------------------------------
//  SMBR read
//-------------------------------------------------
static int TCGNFIF_SMBRRd(MsgTcg_t* pTcgMsg)
{
    //pTcgMsg->param[0] : RdTcgMBRbyNormalRdCmd
    //pTcgMsg->param[1] : mSessionManager.TransactionState
    tTRNSCTN_STATE  trnsctnSt = (tTRNSCTN_STATE)pTcgMsg->param[1];   //pTcgMsg->param[1] : mSessionManager.TransactionState
    BOOL            readCmdReq = pTcgMsg->param[0];                //pTcgMsg->param[0] : RdTcgMBRbyNormalRdCmd
    S32             st;
    U16             ilaa, totalRdCnt, rdCnt;
    U16             laas = pTcgMsg->laas;
    U16             laae = pTcgMsg->laae;
    U16             nextLaas = 0;
    U8*             bufptr = pTcgMsg->pBuffer;
    tTcgGrpDef      currGroup, nextGroup;

    HERE2(laas, laae);
    FirstTx_ssdStartIdx_Record_flag = 0x53415645;        // get first write ssdStartIdx flag;
    if((laae - laas) > (TCG_GENERAL_BUF_SIZE / CFG_UDATA_PER_PAGE)){   //read length could be over buffer size ( TcgTmpBuf )
        TCG_PRINTF("!!Read size is over TcgTmpBuf, Rd BlkCnt=%x", laae - laas);
        return zNG;
    }

    if(((readCmdReq == TRUE) && (trnsctnSt == TRNSCTN_ACTIVE)) || (pMBR_TEMPL2PTBL[laas].pc.cell >= TCG_MBR_CELLS)){
        currGroup = ZONE_SMBR;     //read from group4 (MBR)
    }else{
        currGroup = ZONE_TSMBR;  //read from group5 (tempMBR)
    }
    totalRdCnt = laae - laas;
    rdCnt = 0;

    for(ilaa = 0; ilaa < totalRdCnt; ilaa++){
        if(rdCnt == 0) nextLaas = TCG_SMBR_LAA_START + laas + ilaa;
        rdCnt ++;
        if(ilaa != totalRdCnt - 1){
            //Peek Next Laa's Group
            if(((readCmdReq == TRUE) && (trnsctnSt == TRNSCTN_ACTIVE)) || (pMBR_TEMPL2PTBL[laas + ilaa + 1].pc.cell >= TCG_MBR_CELLS)){
                nextGroup = ZONE_SMBR;     //read from group4 (MBR)
            }else{
                nextGroup = ZONE_TSMBR;  //read from group5 (tempMBR)
            }
            if((currGroup == nextGroup) && (rdCnt < MAX_PAGE_PER_TRK)){
                continue;
            }
        }

        pTcgMsg->laas = nextLaas;
        pTcgMsg->laae = nextLaas + rdCnt;
        pTcgMsg->pBuffer = bufptr + (nextLaas - laas - TCG_SMBR_LAA_START) * CFG_UDATA_PER_PAGE;
        st = (currGroup == ZONE_SMBR) ? TCG_G4Rd(pTcgMsg) : TCG_G5Rd(pTcgMsg);
        if(st == zNG){          // read NG
            if(currGroup == ZONE_SMBR){ //if G4 read fail then Read G5 else return fail
                if(TCG_G5Rd(pTcgMsg) == zOK){
                    if(TCGNFIF_SMBRCommit(NULL) == zNG) return zNG;
                }else{
                    return zNG;
                }
            }else{
                return zNG;
            }
        }
        rdCnt = 0;
        currGroup = nextGroup;
    }

    return zOK;
}

//-------------------------------------------------
//  SMBR write
//-------------------------------------------------
static int TCGNFIF_SMBRWr(MsgTcg_t* pTcgMsg)
{
    U16 SrcLen = pTcgMsg->param[0];
    U16 DesOffset = pTcgMsg->param[0] >> 16;
    U8* SrcBuf = (U8 *)pTcgMsg->param[1];

#if 1
    U16 ilaa;
    HERE2(pTcgMsg->laas, pTcgMsg->laae);

    MEM_COPY((U8 *)pTcgMsg->pBuffer + DesOffset, SrcBuf, SrcLen);
    //TCGNFIF_ParamsSet(pTcgMsg->laas, pTcgMsg->laae, pTcgMsg->pBuffer);
    if(TCG_G5Wr(pTcgMsg) == zNG){
        return zNG;
    }

    for(ilaa = pTcgMsg->laas; ilaa < pTcgMsg->laae; ilaa++){
        pMBR_TEMPL2PTBL[ilaa] = pG5->b.TcgTempMbrL2P[ilaa];
    }
    return zOK;
#else
    U16 laa;
    U16 i;
    U8 *pbuf;

    HERE(NULL);
    MEM_COPY((U8 *)pTcgMsg->pBuffer + DesOffset, SrcBuf, SrcLen);
    pbuf = pTcgMsg->pBuffer;
    for(i = pTcgMsg->laas; i < pTcgMsg->laae; i++){
        TCGNFIF_ParamsSet(i, i + 1, (pbuf + ((i - (pTcgMsg->laas)) * CFG_UDATA_PER_PAGE)));
        if(TCG_G5Wr() == zNG){
            return zNG;
        }

        pMBR_TEMPL2PTBL[i]   =   pG5->b.TcgTempMbrL2P[i];
    }
    return zOK;

#endif
}

//-------------------------------------------------
//  SMBR cimmit
//-------------------------------------------------
static int TCGNFIF_SMBRCommit(MsgTcg_t* pTcgMsg)
{
    U16     ilaa, FirstLaa = 0xFFFF;
    U16     ValidLaaCnt;

    HERE(NULL);
    ValidLaaCnt = 0;
    for(ilaa=0; ilaa<((U32)MBR_LEN/CFG_UDATA_PER_PAGE); ilaa++){
        if(pMBR_TEMPL2PTBL[ilaa].pc.cell < TCG_MBR_CELLS){

            if(ilaa < FirstLaa) FirstLaa = ilaa;
            ValidLaaCnt++;

            if(ValidLaaCnt < MAX_PAGE_PER_TRK){
                continue;
            }else{
                pTcgMsg->laas = FirstLaa;
                pTcgMsg->laae = FirstLaa + ValidLaaCnt;
                pTcgMsg->pBuffer = pTcgTmpBuf;
                if(TCG_G5Rd(pTcgMsg)== zNG) return zNG;
                if(TCG_G4Wr(pTcgMsg)== zNG) return zNG;
                ValidLaaCnt = 0;
                FirstLaa = 0xFFFF;
            }
        }else{
            if(ValidLaaCnt){
                pTcgMsg->laas = FirstLaa;
                pTcgMsg->laae = FirstLaa + ValidLaaCnt;
                pTcgMsg->pBuffer = pTcgTmpBuf;
                if(TCG_G5Rd(pTcgMsg)== zNG) return zNG;
                if(TCG_G4Wr(pTcgMsg)== zNG) return zNG;
                ValidLaaCnt = 0;
                FirstLaa = 0xFFFF;
            }
        }
    }
    if(ValidLaaCnt){
        pTcgMsg->laas = FirstLaa;
        pTcgMsg->laae = FirstLaa + ValidLaaCnt;
        pTcgMsg->pBuffer = pTcgTmpBuf;
        if(TCG_G5Rd(pTcgMsg)== zNG) return zNG;
        if(TCG_G4Wr(pTcgMsg)== zNG) return zNG;
    }
    MEM_SET(pMBR_TEMPL2PTBL, 0xFF, ((U32)MBR_LEN/(U32)CFG_UDATA_PER_PAGE) * sizeof(tTcgLogAddr));
    return zOK;
}

//-------------------------------------------------
//  SMBR clear
//-------------------------------------------------
static int TCGNFIF_SMBRClear(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);

    MEM_SET(pMBR_TEMPL2PTBL, 0xFF, ((U32)MBR_LEN/(U32)CFG_UDATA_PER_PAGE) * sizeof(tTcgLogAddr));
    MEM_SET(&pG5->b.TcgTempMbrL2P[TCG_SMBR_LAA_START], 0xFF, ((U32)MBR_LEN/(U32)CFG_UDATA_PER_PAGE) * sizeof(tTcgLogAddr));

    MEM_SET(&pG4->b.TcgMbrL2P[TCG_SMBR_LAA_START], 0xFF, ((U32)MBR_LEN/(U32)CFG_UDATA_PER_PAGE) * sizeof(tTcgLogAddr));
    MEM_CLR(pTcgTmpBuf, CFG_UDATA_PER_PAGE);
    pTcgMsg->laas = TCG_SMBR_LAA_START;
    pTcgMsg->laae = TCG_SMBR_LAA_START + 1;
    pTcgMsg->pBuffer = pTcgTmpBuf;
    return TCG_G4Wr(pTcgMsg);
}

//-------------------------------------------------
//  SMBR clear
//-------------------------------------------------
static int TCGNFIF_TSMBRClear(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);

    MEM_SET(pMBR_TEMPL2PTBL, 0xFF, ((U32)MBR_LEN/(U32)CFG_UDATA_PER_PAGE) * sizeof(tTcgLogAddr));
    MEM_SET(&pG5->b.TcgTempMbrL2P[TCG_SMBR_LAA_START], 0xFF, ((U32)MBR_LEN/(U32)CFG_UDATA_PER_PAGE) * sizeof(tTcgLogAddr));
    TcgG5NxtPnt.all = TCG_LAA_INIT_VAL;
    MEM_CLR(pTcgTmpBuf, CFG_UDATA_PER_PAGE);
    pTcgMsg->laas = TCG_DUMMY_LAA;
    pTcgMsg->laae = TCG_DUMMY_LAA + 1;
    pTcgMsg->pBuffer = pTcgTmpBuf;
    return TCG_G5Wr(pTcgMsg);
}

//-------------------------------------------------
//  Data Store read
//-------------------------------------------------
static int TCGNFIF_DSRd(MsgTcg_t* pTcgMsg)
{
    U16             ilaa, totalRdCnt, rdCnt;
    U16             laas = pTcgMsg->laas;
    U16             laae = pTcgMsg->laae;
    U8*             bufptr = pTcgMsg->pBuffer;
    U16             nextLaas = 0;
    tTcgGrpDef      currGroup, nextGroup;
    int             st;

    HERE2(laas, laae);
    if((laae - laas) > (TCG_GENERAL_BUF_SIZE / CFG_UDATA_PER_PAGE)){   //read length could be over buffer size ( TcgTmpBuf )
        TCG_PRINTF("!!Read size is over TcgTmpBuf, Rd BlkCnt=%x", laae - laas);
        return zNG;
    }

    if(pDS_TEMPL2PTBL[laas].pc.cell >= TCG_MBR_CELLS){
        currGroup = ZONE_DS;     //read from group4 (MBR)
    }else{
        currGroup = ZONE_TDS;  //read from group5 (tempMBR)
    }
    totalRdCnt = laae - laas;
    rdCnt = 0;

    for(ilaa = 0; ilaa < totalRdCnt; ilaa++){
        if(rdCnt == 0) nextLaas = TCG_DS_LAA_START + laas + ilaa;
        rdCnt ++;
        if(ilaa != totalRdCnt - 1){
            //Peek Next Laa's Group
            if(pDS_TEMPL2PTBL[laas + ilaa + 1].pc.cell >= TCG_MBR_CELLS){
                nextGroup = ZONE_DS;//read from group4 (MBR)
            }else{
                nextGroup = ZONE_TDS;//read from group5 (tempMBR)
            }

            if((currGroup == nextGroup) && (rdCnt < MAX_PAGE_PER_TRK)){
                continue;
            }
        }

        pTcgMsg->laas = nextLaas;
        pTcgMsg->laae = nextLaas + rdCnt;
        pTcgMsg->pBuffer = bufptr + (nextLaas - laas - TCG_DS_LAA_START) * CFG_UDATA_PER_PAGE;
        st = (currGroup == ZONE_DS) ? TCG_G4Rd(pTcgMsg) : TCG_G5Rd(pTcgMsg);
        if(st == zNG){          // read NG
            if(currGroup == ZONE_DS){ //if G4 read fail then Read G5 else return fail
                if(TCG_G5Rd(pTcgMsg) == zOK){
                    if(TCGNFIF_DSCommit(NULL) == zNG) return zNG;
                }else{
                    return zNG;
                }
            }else{
                return zNG;
            }
        }
        rdCnt = 0;
        currGroup = nextGroup;
    }

    return zOK;
}

//-------------------------------------------------
//  Data Store write
//-------------------------------------------------
static int TCGNFIF_DSWr(MsgTcg_t* pTcgMsg)
{
    U16 ilaa;
    U16 laas = pTcgMsg->laas;
    U16 laae = pTcgMsg->laae;
    U16 SrcLen = pTcgMsg->param[0];
    U16 DesOffset = pTcgMsg->param[0] >> 16;
    U8* SrcBuf = (U8 *)pTcgMsg->param[1];

    HERE2(laas, laae);

    MEM_COPY((U8 *)pTcgMsg->pBuffer + DesOffset, SrcBuf, SrcLen);

    pTcgMsg->laas = laas + TCG_DS_LAA_START;
    pTcgMsg->laae = laae + TCG_DS_LAA_START;
    if(TCG_G5Wr(pTcgMsg) == zNG){
        return zNG;
    }

    for(ilaa = laas; ilaa < laae; ilaa++){
        pDS_TEMPL2PTBL[ilaa].all = pG5->b.TcgTempMbrL2P[TCG_DS_LAA_START + ilaa].all;
    }
    return zOK;
}

//-------------------------------------------------
//  Data Store commit
//-------------------------------------------------
static int TCGNFIF_DSCommit(MsgTcg_t* pTcgMsg)
{
    U16     laa, FirstLaa = 0xFFFF;
    U16     ValidLaaCnt;

    HERE(NULL);
    ValidLaaCnt = 0;
    for(laa = TCG_DS_LAA_START; laa < ((U32)DATASTORE_LEN/CFG_UDATA_PER_PAGE) + TCG_DS_LAA_START; laa++){
        if(pDS_TEMPL2PTBL[laa - TCG_DS_LAA_START].pc.cell < TCG_MBR_CELLS){
            if(laa < FirstLaa) FirstLaa = laa;
            ValidLaaCnt++;
            if(ValidLaaCnt < MAX_PAGE_PER_TRK){
                continue;
            }else{
                pTcgMsg->laas = FirstLaa;
                pTcgMsg->laae = FirstLaa + ValidLaaCnt;
                pTcgMsg->pBuffer = pTcgTmpBuf;
                if(TCG_G5Rd(pTcgMsg)== zNG) return zNG;
                if(TCG_G4Wr(pTcgMsg)== zNG) return zNG;
                ValidLaaCnt = 0;
                FirstLaa = 0xFFFF;
            }
        }else{
            if(ValidLaaCnt){
                pTcgMsg->laas = FirstLaa;
                pTcgMsg->laae = FirstLaa + ValidLaaCnt;
                pTcgMsg->pBuffer = pTcgTmpBuf;
                if(TCG_G5Rd(pTcgMsg)== zNG) return zNG;
                if(TCG_G4Wr(pTcgMsg)== zNG) return zNG;
                ValidLaaCnt = 0;
                FirstLaa = 0xFFFF;
            }
        }
    }
    if(ValidLaaCnt){
        pTcgMsg->laas = FirstLaa;
        pTcgMsg->laae = FirstLaa + ValidLaaCnt;
        pTcgMsg->pBuffer = pTcgTmpBuf;
        if(TCG_G5Rd(pTcgMsg)== zNG) return zNG;
        if(TCG_G4Wr(pTcgMsg)== zNG) return zNG;
    }
    MEM_SET(pDS_TEMPL2PTBL, 0xFF, ((U32)DATASTORE_LEN/(U32)CFG_UDATA_PER_PAGE) * sizeof(tTcgLogAddr));
    return zOK;
}

//-------------------------------------------------
//  Data Store clear
//-------------------------------------------------
static int TCGNFIF_DSClear(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);

    MEM_SET(pDS_TEMPL2PTBL, 0xFF, ((U32)DATASTORE_LEN/(U32)CFG_UDATA_PER_PAGE) * sizeof(tTcgLogAddr));
    MEM_SET(&pG5->b.TcgTempMbrL2P[TCG_DS_LAA_START].all, 0xFF, ((U32)DATASTORE_LEN/(U32)CFG_UDATA_PER_PAGE) * sizeof(tTcgLogAddr));

    MEM_SET(&pG4->b.TcgMbrL2P[TCG_DS_LAA_START].all, 0xFF,((U32)DATASTORE_LEN/(U32)CFG_UDATA_PER_PAGE) * sizeof(tTcgLogAddr));
    MEM_CLR(pTcgTmpBuf, CFG_UDATA_PER_PAGE);

    pTcgMsg->laas = TCG_DS_LAA_START;
    pTcgMsg->laae = TCG_DS_LAA_START + 1;
    pTcgMsg->pBuffer = pTcgTmpBuf;
    return TCG_G4Wr(pTcgMsg);
}

//-------------------------------------------------
//  Data Store clear
//-------------------------------------------------
static int TCGNFIF_TDSClear(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);

    MEM_SET(pDS_TEMPL2PTBL, 0xFF, ((U32)DATASTORE_LEN/(U32)CFG_UDATA_PER_PAGE) * sizeof(tTcgLogAddr));
    MEM_SET(&pG5->b.TcgTempMbrL2P[TCG_DS_LAA_START].all, 0xFF, ((U32)DATASTORE_LEN/(U32)CFG_UDATA_PER_PAGE) * sizeof(tTcgLogAddr));
    TcgG5NxtPnt.all = TCG_LAA_INIT_VAL;
    MEM_CLR(pTcgTmpBuf, CFG_UDATA_PER_PAGE);
    pTcgMsg->laas = TCG_DS_DUMMY_LAA;
    pTcgMsg->laae = TCG_DS_DUMMY_LAA + 1;
    pTcgMsg->pBuffer = pTcgTmpBuf;
    return TCG_G5Wr(pTcgMsg);
}

//-------------------------------------------------
//  get G5 FTL
//-------------------------------------------------
static int TCGNFIF_G5FTL(MsgTcg_t* pTcgMsg)
{
    // G5, TCG Temp MBR Group -start-------------------------------------------------------------------------
    U32 x;
    U32 DefChanMap;
    U8 DefsPerRow;
    U8 RdOKInx;
    int i;
    HERE(NULL);

    TcgG5CurHistNo = 0;
    TcgG5NxtHistNo = 1;
    TcgG5CurPnt.all = TCG_LAA_INIT_VAL;
    TcgG5NxtPnt.all = 0;

    TCG_FIO_F_MUTE = FIO_F_MUTE;
    for(TcgG5Pnt.all = 0; TcgG5Pnt.pc.cell < TCG_MBR_CELLS; TcgG5Pnt.pc.cell += DEVICES_NUM_PER_ROW)
    {
        DefChanMap = 0;
        DefsPerRow = 0;
        for(x = 0; x < DEVICES_NUM_PER_ROW; x++)
        {
            if(tcgG5Dft[TcgG5Pnt.pc.cell + x] != 0)
            {
                DefChanMap |= mBIT(x);
                DefsPerRow++;
            }
        }
        // read first page of each block
        TcgG5Pnt.pc.page = 0;
        TCG_RdMulChan(ZONE_GRP5, TcgG5Pnt, DefChanMap);

        for(i = 0; i < (PAA_NUM_PER_ROW - DefsPerRow * PAA_NUM_PER_PAGE); i += PAA_NUM_PER_PAGE)
        {
            if((FIO_CHK_ERR_BMP(pTcgJob->fid, i) == 0)
            && (FIO_CHK_ERR_BMP(pTcgJob->fid, i+1) == 0)
            && (FIO_CHK_ERR_BMP(pTcgJob->fid, i+2) == 0)
            && (FIO_CHK_ERR_BMP(pTcgJob->fid, i+3) == 0))
            {
                if(gTcgAux[i].aux0 == TCG_G5_TAG)
                {
                    if(gTcgAux[i].aux1 >= TcgG5CurHistNo)
                    {
                        TcgG5CurHistNo = gTcgAux[i].aux1;
                        RdOKInx = 0;
                        for(x = 0; x < DEVICES_NUM_PER_ROW; x++)
                        {
                            if(DefChanMap & mBIT(x))
                            {    // defect
                                 continue;
                            }
                            else
                            {
                                if((i / PAA_NUM_PER_PAGE) == RdOKInx)
                                {
                                    TcgG5CurPnt.pc.cell = TcgG5Pnt.pc.cell + x;
                                    break;
                                }
                                else
                                {
                                    RdOKInx++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    TcgG5NxtHistNo = TcgG5CurHistNo + 1;
    TCG_PRINTF("|| TCG G5  Pnt |  Cell  |  Page  ||\n");
    TCG_PRINTF("|| Current Pnt |%8X|%8X||\n", TcgG5CurPnt.pc.cell, TcgG5CurPnt.pc.page);
    TCG_PRINTF("|| Next    Pnt |%8X|%8X||\n", TcgG5NxtPnt.pc.cell, TcgG5NxtPnt.pc.page);

    if(TcgG5CurPnt.all == TCG_LAA_INIT_VAL)
    {   // tbl not in nand, write the very first one
        //soutb3(0x38, 0x01, 0x05);       //38 01,       "  TCG group %02X write" 1
        pG5->b.TcgG5Header = 0x5631;         // "V1"
        MEM_SET(pG5->b.TcgTempMbrL2P, 0xFF, sizeof(pG5->b.TcgTempMbrL2P));
        TcgG5NxtPnt.all = 0;
        while(tcgG5Dft[TcgG5NxtPnt.pc.cell]) TcgG5NxtPnt.pc.cell++;
        TCG_G5WrL2p();
        TCGNFIF_G5WrDefault(pTcgMsg);
    }
    else
    {   //tbl exists in nand, read it
        //soutb3(0x38, 0x02, 0x05);               //38 02,       "  TCG group %02X read" 1
        TcgG5CurPnt.pc.page = 0;
        TcgG5Pnt.all = TcgG5CurPnt.all;
        TcgG5NxtPnt.all = TcgG5CurPnt.all + SLC_OFF(L2P_PAGE_CNT);
        // 1. read l2p on page0~4
        if(TCG_G5RdL2p() == zNG)
        {
            // TODO: L2P readretry
        }
        // 2. walk thru page5~255, decide NextPnt and update l2p
        TcgG5Pnt.pc.page = SLC_OFF(L2P_PAGE_CNT);
        while(1)
        {
            if(TCG_Rd1Pg(TcgG5Pnt, ZONE_GRP5) == zNG)
            {
                // TODO: readretry
            }
            if((pTcgJob->status & FIO_S_ERR_MASK)== FIO_S_NO_ERR)
            {   // 0:ecc ok
                pG5->b.TcgTempMbrL2P[gTcgAux[0].aux0].all = TcgG5Pnt.all;
                TcgG5CurPnt.all = TcgG5NxtPnt.all;
                TcgG5NxtPnt.all += SLC_OFF(1);
            }
            else if(pTcgJob->status & FIO_S_BLANK_ERR)
            {   // 1:blank
                break;
            }
            else
            {   // other:ecc error
                TcgG5NxtPnt.all += SLC_OFF(1);
            }
            TcgG5Pnt.pc.page += SLC_OFF(1);
            if(TcgG5Pnt.pc.page == PAGE_NUM_PER_BLOCK)
            {
                TcgG5NxtPnt.all = TCG_LAA_INIT_VAL;
                break;
            }
        }
        //memset(TcgTempMbrL2P,0xFF,sizeof(TcgTempMbrL2P));
        //memset((mUINT_8 *)TcgTempMbrL2P,0xFF,((mUINT_32)MBR_LEN/(mUINT_32)CFG_UDATA_PER_PAGE)*2);  //clr G5 SMBR
        //memset((mUINT_8 *)&TcgTempMbrL2P[TCG_DATASTORE_LAA_START],0xFF,((mUINT_32)DATASTORE_LEN/CFG_UDATA_PER_PAGE)*2); //clr G5 DataStore
    }
    TcgG5Defects = 0;
    TCG_FIO_F_MUTE = 0;
    for(i=0;i<TCG_MBR_CELLS;i++)
    {   //calculate G5 defect blocks
        if(tcgG5Dft[i]) TcgG5Defects++;
    }
    // G5, TCG Temp MBR Group -end-------------------------------------------------------------------------------
    TCG_PRINTF("|| TCG G5  Pnt |  Cell  |  Page  ||\n");
    TCG_PRINTF("|| Current Pnt |%8X|%8X||\n", TcgG5CurPnt.pc.cell, TcgG5CurPnt.pc.page);
    TCG_PRINTF("|| Next    Pnt |%8X|%8X||\n", TcgG5NxtPnt.pc.cell, TcgG5NxtPnt.pc.page);

    return zOK;
}

//-------------------------------------------------
//  get G4 FTL
//-------------------------------------------------
static int TCGNFIF_G4FTL(MsgTcg_t* pTcgMsg)
{
    // G4, TCG MBR Group -start------------------------------------------------------------------
    U32 x;
    U32 DefChanMap;
    U8 DefsPerRow;
    U8 RdOKInx;
    int i;
    HERE(NULL);

    TcgG4CurHistNo = 0;
    TcgG4NxtHistNo = 1;
    TcgG4CurPnt.all = TCG_LAA_INIT_VAL;
    TcgG4NxtPnt.all = 0;

    TCG_FIO_F_MUTE = FIO_F_MUTE;
    for(TcgG4Pnt.all = 0; TcgG4Pnt.pc.cell < TCG_MBR_CELLS; TcgG4Pnt.pc.cell += DEVICES_NUM_PER_ROW)
    {
        DefChanMap = 0;
        DefsPerRow = 0;
        for(x = 0; x < DEVICES_NUM_PER_ROW; x++)
        {
            if(tcgG4Dft[TcgG4Pnt.pc.cell + x] != 0)
            {
                DefChanMap |= mBIT(x);
                DefsPerRow++;
            }
        }
        // read first page of each block
        TcgG4Pnt.pc.page = 0;
        TCG_RdMulChan(ZONE_GRP4, TcgG4Pnt, DefChanMap);

        for(i = 0; i < (PAA_NUM_PER_ROW - DefsPerRow * PAA_NUM_PER_PAGE); i += PAA_NUM_PER_PAGE)
        {
            if((FIO_CHK_ERR_BMP(pTcgJob->fid, i)==0)
            && (FIO_CHK_ERR_BMP(pTcgJob->fid, i+1)==0) // i & i+1 is same page for 16K page size
            && (FIO_CHK_ERR_BMP(pTcgJob->fid, i+2)==0)
            && (FIO_CHK_ERR_BMP(pTcgJob->fid, i+3)==0))
            {
                if(gTcgAux[i].aux0 == TCG_G4_TAG)
                {
                    if(gTcgAux[i].aux1 >= TcgG4CurHistNo)
                    {
                        TcgG4CurHistNo = gTcgAux[i].aux1;
                        RdOKInx = 0;
                        for(x = 0; x < DEVICES_NUM_PER_ROW; x++)
                        {
                            if(DefChanMap & mBIT(x))
                            { // defect
                                 continue;
                            }
                            else
                            {
                                if((i / PAA_NUM_PER_PAGE) == RdOKInx)
                                {
                                    TcgG4CurPnt.pc.cell = TcgG4Pnt.pc.cell + x;
                                    break;
                                }
                                else
                                {
                                    RdOKInx++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //alexdebugws(TcgGroup4CurrPnt.all,TcgGroup4CurrPnt);
    TcgG4NxtHistNo = TcgG4CurHistNo + 1;
    TCG_PRINTF("|| TCG G4  Pnt |  Cell  |  Page  ||\n");
    TCG_PRINTF("|| Current Pnt |%8X|%8X||\n", TcgG4CurPnt.pc.cell, TcgG4CurPnt.pc.page);
    TCG_PRINTF("|| Next    Pnt |%8X|%8X||\n", TcgG4NxtPnt.pc.cell, TcgG4NxtPnt.pc.page);

    if(TcgG4CurPnt.all == TCG_LAA_INIT_VAL)
    {   // tbl not in nand, write the very first one
        //soutb3(0x38, 0x01, 0x04);//38 01, "  TCG group %02X write" 1
        pG4->b.TcgG4Header = 0x5631;     // "V1"
        MEM_SET(pG4->b.TcgMbrL2P, 0xFF, sizeof(pG4->b.TcgMbrL2P));
        TcgG4NxtPnt.all = 0;
        while(tcgG4Dft[TcgG4NxtPnt.pc.cell]) TcgG4NxtPnt.pc.cell++;
        TCG_G4WrL2p();
        TCGNFIF_G4WrDefault(pTcgMsg);
    }
    else
    {   // tbl exists in nand, read it
        //soutb3(0x38, 0x02, 0x04);       //38 02,       "  TCG group %02X read" 1
        TcgG4CurPnt.pc.page = 0;
        TcgG4Pnt.all = TcgG4CurPnt.all;
        TcgG4NxtPnt.all = TcgG4CurPnt.all + SLC_OFF(L2P_PAGE_CNT);
        // 1. read l2p on page0~4
        if(TCG_G4RdL2p() == zNG)
        {
            // TODO: L2P readretry
        }
        // 2. walk thru page5~255, decide NextPnt and update l2p
        TcgG4Pnt.pc.page = SLC_OFF(L2P_PAGE_CNT);
        while(1)
        {
            if(TCG_Rd1Pg(TcgG4Pnt, ZONE_GRP4) == zNG)
            {
                // TODO: readretry
            }
            if((pTcgJob->status & FIO_S_ERR_MASK) == FIO_S_NO_ERR)
            {   // 0:ecc ok
                pG4->b.TcgMbrL2P[gTcgAux[0].aux0].all = TcgG4Pnt.all;
                TcgG4CurPnt.all = TcgG4NxtPnt.all;
                TcgG4NxtPnt.all += SLC_OFF(1);
            }
            else if(pTcgJob->status & FIO_S_BLANK_ERR)
            {   // 1:blank
                break;
            }
            else
            {   // other:ecc error
                TcgG4NxtPnt.all += SLC_OFF(1);
            }
            TcgG4Pnt.pc.page += SLC_OFF(1);
            if(TcgG4Pnt.pc.page == PAGE_NUM_PER_BLOCK)
            {
                TcgG4NxtPnt.all = TCG_LAA_INIT_VAL;
                break;
            }
        }
    }
    TcgG4Defects = 0;
    TCG_FIO_F_MUTE = 0;
    for(i=0;i<TCG_MBR_CELLS;i++)
    {   // calculate G4 defect blocks
        if(tcgG4Dft[i]) TcgG4Defects++;
    }

    TCG_PRINTF("|| TCG G4  Pnt |  Cell  |  Page  ||\n");
    TCG_PRINTF("|| Current Pnt |%8X|%8X||\n", TcgG4CurPnt.pc.cell, TcgG4CurPnt.pc.page);
    TCG_PRINTF("|| Next    Pnt |%8X|%8X||\n", TcgG4NxtPnt.pc.cell, TcgG4NxtPnt.pc.page);

    // G4, TCG MBR Group -end-------------------------------------------------------------------------------
    return zOK;
}

//-------------------------------------------------
//  read G1 Default table
//-------------------------------------------------
static int TCGNFIF_G1RdDefault(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G1_DEFAULT_LAA0;
    pTcgMsg->laae = TCG_G1_DEFAULT_LAA0 + 1;
    pTcgMsg->pBuffer = &pG1->all[0];
    if(TCG_G4Rd(pTcgMsg) == zNG){
        if(TCG_G5Rd(pTcgMsg) == zNG) return zNG;
    }
    return zOK;
}

//-------------------------------------------------
//  read G2 Default table
//-------------------------------------------------
static int TCGNFIF_G2RdDefault(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G2_DEFAULT_LAA0;
    pTcgMsg->laae = TCG_G2_DEFAULT_LAA1 + 1;
    pTcgMsg->pBuffer = &pG2->all[0];
    if(TCG_G4Rd(pTcgMsg) == zNG){
        if(TCG_G5Rd(pTcgMsg) == zNG) return zNG;
    }
    return zOK;
}

//-------------------------------------------------
//  read G3 Default table
//-------------------------------------------------
static int TCGNFIF_G3RdDefault(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G3_DEFAULT_LAA0;
    pTcgMsg->laae = TCG_G3_DEFAULT_LAA1 + 1;
    pTcgMsg->pBuffer = &pG3->all[0];
    if(TCG_G4Rd(pTcgMsg) == zNG){
        if(TCG_G5Rd(pTcgMsg) == zNG) return zNG;
    }
    return zOK;
}

//-------------------------------------------------
//  read G5 (TG1, TG2 & TG3) Default table
//-------------------------------------------------
static int TCGNFIF_G5RdDefault(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    pTcgMsg->laas = TCG_G1_DEFAULT_LAA0;
    pTcgMsg->laae = TCG_G1_DEFAULT_LAA0 + 1;
    pTcgMsg->pBuffer = &pG1->all[0];
    if(TCG_G5Rd(pTcgMsg) == zNG) return zNG;

    pTcgMsg->laas = TCG_G2_DEFAULT_LAA0;
    pTcgMsg->laae = TCG_G2_DEFAULT_LAA1 + 1;
    pTcgMsg->pBuffer = &pG2->all[0];
    if(TCG_G5Rd(pTcgMsg) == zNG) return zNG;

    pTcgMsg->laas = TCG_G3_DEFAULT_LAA0;
    pTcgMsg->laae = TCG_G3_DEFAULT_LAA1 + 1;
    pTcgMsg->pBuffer = &pG3->all[0];
    return TCG_G5Rd(pTcgMsg);
}

//-------------------------------------------------
//  read G4 (G1, G2 & G3) Default table
//-------------------------------------------------
static int TCGNFIF_G4RdDefault(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    if(TCGNFIF_G1RdDefault(pTcgMsg) ||
       TCGNFIF_G2RdDefault(pTcgMsg) ||
       TCGNFIF_G3RdDefault(pTcgMsg)) return zNG;
    return zOK;
}

//-------------------------------------------------
//  write G5 (TG1, TG2 & TG3) Default table
//-------------------------------------------------
static int TCGNFIF_G5WrDefault(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);

    pTcgMsg->laas = TCG_G1_DEFAULT_LAA0;
    pTcgMsg->laae = TCG_G1_DEFAULT_LAA0 + 1;
    pTcgMsg->pBuffer = &pG1->all[0];
    if(TCG_G5Wr(pTcgMsg) == zNG) return zNG;

    pTcgMsg->laas = TCG_G2_DEFAULT_LAA0;
    pTcgMsg->laae = TCG_G2_DEFAULT_LAA1 + 1;
    pTcgMsg->pBuffer = &pG2->all[0];
    if(TCG_G5Wr(pTcgMsg) == zNG) return zNG;

    pTcgMsg->laas = TCG_G3_DEFAULT_LAA0;
    pTcgMsg->laae = TCG_G3_DEFAULT_LAA1 + 1;
    pTcgMsg->pBuffer = &pG3->all[0];

    return TCG_G5Wr(pTcgMsg);
}

//-------------------------------------------------
//  write G4 (G1, G2 & G3) Default table
//-------------------------------------------------
static int TCGNFIF_G4WrDefault(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);

    pTcgMsg->laas = TCG_G1_DEFAULT_LAA0;
    pTcgMsg->laae = TCG_G1_DEFAULT_LAA0 + 1;
    pTcgMsg->pBuffer = &pG1->all[0];
    if(TCG_G4Wr(pTcgMsg) == zNG) return zNG;

    pTcgMsg->laas = TCG_G2_DEFAULT_LAA0;
    pTcgMsg->laae = TCG_G2_DEFAULT_LAA1 + 1;
    pTcgMsg->pBuffer = &pG2->all[0];
    if(TCG_G4Wr(pTcgMsg) == zNG) return zNG;

    pTcgMsg->laas = TCG_G3_DEFAULT_LAA0;
    pTcgMsg->laae = TCG_G3_DEFAULT_LAA1 + 1;
    pTcgMsg->pBuffer = &pG3->all[0];

    return TCG_G4Wr(pTcgMsg);
}

//-------------------------------------------------
//   G5  build defect
//-------------------------------------------------
static int TCGNFIF_G5BuildDefect(MsgTcg_t* pTcgMsg)
{
    U32 x;
    U32 DefChanMap;

    HERE(NULL);
    TCG_PRINTF("FTL offer super block G4=%4X, G5=%4X\n",smSysInfo->d.TcgData.d.TCGBlockNo[0], smSysInfo->d.TcgData.d.TCGBlockNo[1]);
    if(smSysInfo->d.TcgData.d.TCGBlockNo[0] == smSysInfo->d.TcgData.d.TCGBlockNo[1]){
        TCG_PRINTF("!! Error, FTL doesn't offer blocks for TCG. check out!\n");
    }
    //------------------------------ Group5 -----------------------------------
    TCG_FIO_F_MUTE = FIO_F_MUTE;
    MEM_CLR(pTcgTmpBuf, TCG_GENERAL_BUF_SIZE);

    // GROUP 5 safe erase all and write pattern (multi chan)
    for(TcgG5Pnt.all = 0; TcgG5Pnt.pc.cell < TCG_MBR_CELLS; TcgG5Pnt.pc.cell += DEVICES_NUM_PER_ROW)
    {
        DefChanMap = 0;
        for(x = 0; x < DEVICES_NUM_PER_ROW; x++)
        {
            if(tcgG5Dft[TcgG5Pnt.pc.cell+x] != 0)
            {
                TCG_PRINTF("ori DF %4X\n",  TcgG5Pnt.pc.cell + x);  //soutb3(0x5C, 0xFF, 0x73); soutw(TcgG5Pnt.pc.cellNo + x);  //"..%04X" 2
                DefChanMap |= mBIT(x);
            }
        }

        // ___ERASE___,  check any error happened after erase?
        if(TCG_ErMulChan(ZONE_GRP5, TcgG5Pnt, DefChanMap) == zNG)
        {
            int i;
            U16 DftBlk;

            for(i = 0; i < ValidCnt * PAA_NUM_PER_PAGE; i += PAA_NUM_PER_PAGE)
            {
                if((FIO_CHK_ERR_BMP(pTcgJob->fid, i) != 0)
                || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 1) != 0)
                || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 2) != 0)
                || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 3) != 0))
                {
                    DftBlk = ValidBlks[i / PAA_NUM_PER_PAGE];
                    TCG_PRINTF("era DF %4X\n", DftBlk);  //soutb3(0x5C, 0xFF, 0x79); soutw(DftBlk);   //5C FF 79,    "Got dft blk = %04X in erasing" 2
                    tcgG5Dft[DftBlk] = 0xDF;
                    DefChanMap |= mBIT(DftBlk & (DEVICES_NUM_PER_ROW - 1));
                }
            }
        }

        // ___WRITE___,  multi write
        for(x = 0; x < gNfInfo.pagesPerBlock; x += SLC_OFF(1))
        {
            TcgG5Pnt.pc.page = x;

            // check any writing error happened ?
            if(TCG_WrMulChan(ZONE_GRP5, TcgG5Pnt, DefChanMap) == zNG)
            {
                int i;
                U16 DftBlk;

                for(i = 0; i < ValidCnt * PAA_NUM_PER_PAGE; i += PAA_NUM_PER_PAGE)
                {
                    if((FIO_CHK_ERR_BMP(pTcgJob->fid, i) != 0)
                    || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 1) != 0)
                    || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 2) != 0)
                    || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 3) != 0))
                    {
                        DftBlk = ValidBlks[i / PAA_NUM_PER_PAGE];
                        TCG_PRINTF("wr DF %4X\n", DftBlk);    //soutb3(0x5C, 0xFF, 0x79); soutw(DftBlk);     //5C FF 79,    "Got dft blk = %04X in erasing" 2
                        tcgG5Dft[DftBlk] = 0xDF;
                        DefChanMap |= mBIT(DftBlk & (DEVICES_NUM_PER_ROW - 1));
                    }
                }
            }
        }
        TcgG5Pnt.pc.page = 0;
    }

    // ___READ___,  multi read, GROUP5 read check, mark defect if found any (single chan)
    for(TcgG5Pnt.all = 0; TcgG5Pnt.pc.cell < TCG_MBR_CELLS; TcgG5Pnt.pc.cell += DEVICES_NUM_PER_ROW){
        DefChanMap = 0;
        for(x = 0; x < DEVICES_NUM_PER_ROW; x++){
            if(tcgG5Dft[TcgG5Pnt.pc.cell + x] != 0){
                DefChanMap |= mBIT(x);
            }
        }
        //multi read
        for(x = 0; x < gNfInfo.pagesPerBlock; x += SLC_OFF(1))
        {
            TcgG5Pnt.pc.page = x;
            if(TCG_RdMulChan(ZONE_GRP5, TcgG5Pnt, DefChanMap) == zNG)
            {
                int i;
                U16 DftBlk;

                for(i = 0; i < ValidCnt*PAA_NUM_PER_PAGE; i += PAA_NUM_PER_PAGE)
                {
                    if((FIO_CHK_ERR_BMP(pTcgJob->fid, i) != 0)
                    || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 1) != 0)
                    || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 2) != 0)
                    || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 3) != 0))
                    {
                        DftBlk = ValidBlks[i / PAA_NUM_PER_PAGE];
                        TCG_PRINTF("rd DF %4X\n", DftBlk);    //soutb3(0x5C, 0xFF, 0x79); soutw(DftBlk);     //5C FF 79,    "Got dft blk = %04X in erasing" 2
                        tcgG5Dft[DftBlk] = 0xDF;
                        DefChanMap |= mBIT(DftBlk & (DEVICES_NUM_PER_ROW - 1));
                    }
                }
            }
        }
        TcgG5Pnt.pc.page = 0;
    }

    // ___ERASE___,  GROUP5 safe erase all (multi chan)
    for(TcgG5Pnt.all = 0; TcgG5Pnt.pc.cell < TCG_MBR_CELLS; TcgG5Pnt.pc.cell += DEVICES_NUM_PER_ROW)
    {
        DefChanMap = 0;
        for(x = 0; x < DEVICES_NUM_PER_ROW; x++)
        {
            if(tcgG5Dft[TcgG5Pnt.pc.cell + x]!=0)
            {
                TCG_PRINTF("fnl DF %4X\n",  TcgG5Pnt.pc.cell + x);  //soutb3(0x5C, 0xFF, 0x73); soutw(TcgGroup5Pnt.pc.cellNo + x);  //"..%04X" 2
                DefChanMap |= mBIT(x);
            }
        }
        TCG_ErMulChan(ZONE_GRP5, TcgG5Pnt, DefChanMap);
    }
    TCG_FIO_F_MUTE = 0;
    MEM_SET(pG5->b.TcgTempMbrL2P, 0xFF, sizeof(G5.b.TcgTempMbrL2P));
    return zOK;
}

//-------------------------------------------------
//   G4  build defect
//-------------------------------------------------
static int TCGNFIF_G4BuildDefect(MsgTcg_t* pTcgMsg)
{
    U32 x;
    U32 DefChanMap;

    HERE(NULL);
    TCG_PRINTF("FTL offer super block G4=%4X, G5=%4X\n",smSysInfo->d.TcgData.d.TCGBlockNo[0], smSysInfo->d.TcgData.d.TCGBlockNo[1]);
    if(smSysInfo->d.TcgData.d.TCGBlockNo[0] == smSysInfo->d.TcgData.d.TCGBlockNo[1]){
        TCG_PRINTF("!! Error, FTL doesn't offer blocks for TCG. check out!\n");
    }
    //------------------------------ Group4 -----------------------------------
    TCG_FIO_F_MUTE = FIO_F_MUTE;
    MEM_CLR(pTcgTmpBuf, TCG_GENERAL_BUF_SIZE);

    // GROUP 4 safe erase all and write pattern (multi chan)
    for(TcgG4Pnt.all = 0; TcgG4Pnt.pc.cell < TCG_MBR_CELLS; TcgG4Pnt.pc.cell += DEVICES_NUM_PER_ROW)
    {
        DefChanMap=0;
        for(x = 0; x < DEVICES_NUM_PER_ROW; x++)
        {
            if(tcgG4Dft[TcgG4Pnt.pc.cell + x] != 0)
            {
                TCG_PRINTF("ori DF %4X\n",  TcgG4Pnt.pc.cell + x);  //soutb3(0x5C, 0xFF, 0x73); soutw(TcgG4Pnt.pc.cellNo + x);  //"..%04X" 2
                DefChanMap|=mBIT(x);
            }
        }

        // ___ERASE___,  check any erasing error happened ?
        if(TCG_ErMulChan(ZONE_GRP4, TcgG4Pnt, DefChanMap) == zNG)
        {
            int i;
            U16 DftBlk;

            for(i = 0; i < ValidCnt * PAA_NUM_PER_PAGE; i += PAA_NUM_PER_PAGE)
            {
                if((FIO_CHK_ERR_BMP(pTcgJob->fid, i) != 0)
                || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 1) != 0)
                || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 2) != 0)
                || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 3) != 0))
                {
                    DftBlk = ValidBlks[i / PAA_NUM_PER_PAGE];
                    TCG_PRINTF("era DF %4X\n", DftBlk);  //soutb3(0x5C, 0xFF, 0x79); soutw(DftBlk);   //5C FF 79,    "Got dft blk = %04X in erasing" 2
                    tcgG4Dft[DftBlk] = 0xDF;
                    DefChanMap |= mBIT(DftBlk & (DEVICES_NUM_PER_ROW - 1));
                }
            }
        }

        // ___WRITE___,   multi write
        for(x = 0; x < gNfInfo.pagesPerBlock; x += SLC_OFF(1))
        {
            TcgG4Pnt.pc.page = x;

            // check any writing error happened ?
            if(TCG_WrMulChan(ZONE_GRP4, TcgG4Pnt, DefChanMap) == zNG)
            {
                int i;
                U16 DftBlk;

                for(i = 0; i < ValidCnt * PAA_NUM_PER_PAGE; i += PAA_NUM_PER_PAGE)
                {
                    if((FIO_CHK_ERR_BMP(pTcgJob->fid, i) != 0)
                    || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 1) != 0)
                    || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 2) != 0)
                    || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 3) != 0))
                    {
                        DftBlk = ValidBlks[i / PAA_NUM_PER_PAGE];
                        TCG_PRINTF("wr DF %4X\n", DftBlk);  //soutb3(0x5C, 0xFF, 0x78); soutw(DftBlk);   //5C FF 78,    "Got dft blk = %04X in writing" 2
                        tcgG4Dft[DftBlk] = 0xDF;
                        DefChanMap |= mBIT(DftBlk & (DEVICES_NUM_PER_ROW - 1));
                    }
                }
            }
        }
        TcgG4Pnt.pc.page = 0;
    }

    // ___READ___,   GROUP4 read check, mark defect if found any (single chan)
    for(TcgG4Pnt.all = 0; TcgG4Pnt.pc.cell < TCG_MBR_CELLS; TcgG4Pnt.pc.cell += DEVICES_NUM_PER_ROW){
        DefChanMap = 0;
        for(x = 0; x < DEVICES_NUM_PER_ROW; x++){
            if(tcgG4Dft[TcgG4Pnt.pc.cell + x] != 0){
                DefChanMap |= mBIT(x);
            }
        }
        // multi read
        for(x = 0; x < gNfInfo.pagesPerBlock; x += SLC_OFF(1))
        {
            TcgG4Pnt.pc.page = x;
            if(TCG_RdMulChan(ZONE_GRP4, TcgG4Pnt, DefChanMap) == zNG)
            {
                int i;
                U16 DftBlk;

                for(i = 0; i < ValidCnt*PAA_NUM_PER_PAGE; i += PAA_NUM_PER_PAGE)
                {
                    if((FIO_CHK_ERR_BMP(pTcgJob->fid, i) != 0)
                    || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 1) != 0)
                    || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 2) != 0)
                    || (FIO_CHK_ERR_BMP(pTcgJob->fid, i + 3) != 0))
                    {
                        DftBlk = ValidBlks[i / PAA_NUM_PER_PAGE];
                        TCG_PRINTF("rd DF %4X\n", DftBlk);  //soutb3(0x5C, 0xFF, 0x74); soutw(DftBlk);   //5C FF 74,    "Got dft blk = %04X in reading" 2
                        tcgG4Dft[DftBlk] = 0xDF;
                        DefChanMap |= mBIT(DftBlk & (DEVICES_NUM_PER_ROW - 1));
                    }
                }
            }
        }
        TcgG4Pnt.pc.page = 0;
    }

    // ___ERASE___,   GROUP4 safe erase all (multi chan)
    for(TcgG4Pnt.all = 0; TcgG4Pnt.pc.cell < TCG_MBR_CELLS; TcgG4Pnt.pc.cell += DEVICES_NUM_PER_ROW)
    {
        DefChanMap = 0;
        for(x = 0; x < DEVICES_NUM_PER_ROW; x++)
        {
            if(tcgG4Dft[TcgG4Pnt.pc.cell + x] != 0)
            {
                TCG_PRINTF("fnl DF %4X\n",  TcgG4Pnt.pc.cell + x);  //soutb3(0x5C, 0xFF, 0x73); soutw(TcgGroup4Pnt.pc.cellNo + x);  //"..%04X" 2
                DefChanMap |= mBIT(x);
            }
        }
        TCG_ErMulChan(ZONE_GRP4, TcgG4Pnt, DefChanMap);
    }
    TCG_FIO_F_MUTE = 0;
    MEM_SET(pG4->b.TcgMbrL2P, 0xFF, sizeof(G4.b.TcgMbrL2P));
    return zOK;
}

//-------------------------------------------------
//  G5 dummy read
//-------------------------------------------------
static int TCGNFIF_G5DmyRd(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    //MEM_CLR(pTcgTmpBuf, (pTcgMsg->laae - pTcgMsg->laas) * CFG_UDATA_PER_PAGE);  // clear requirment
    MEM_CLR(pTcgTmpBuf, TCG_GENERAL_BUF_SIZE);   // clear buffer all.
    pTcgMsg->laas = TCG_DUMMY_LAA;
    pTcgMsg->laae = TCG_DUMMY_LAA + 1;
    pTcgMsg->pBuffer = pTcgTmpBuf;
    return TCG_G5Rd(pTcgMsg);
}

//-------------------------------------------------
//  G4 dummy read
//-------------------------------------------------
static int TCGNFIF_G4DmyRd(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    //MEM_CLR(pTcgTmpBuf, (pTcgMsg->laae - pTcgMsg->laas) * CFG_UDATA_PER_PAGE);  // clear requirment
    MEM_CLR(pTcgTmpBuf, TCG_GENERAL_BUF_SIZE);  // clear buffer all.
    pTcgMsg->laas = TCG_DUMMY_LAA;
    pTcgMsg->laae = TCG_DUMMY_LAA + 1;
    pTcgMsg->pBuffer = pTcgTmpBuf;
    return TCG_G4Rd(pTcgMsg);
}

//-------------------------------------------------
//  G5 dummy write
//-------------------------------------------------
static int TCGNFIF_G5DmyWr(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);

    MEM_CLR(pTcgTmpBuf, CFG_UDATA_PER_PAGE);
    pTcgMsg->laas = TCG_DUMMY_LAA;
    pTcgMsg->laae = TCG_DUMMY_LAA + 1;
    pTcgMsg->pBuffer= pTcgTmpBuf;

    return TCG_G5Wr(pTcgMsg);
}

//-------------------------------------------------
//  G4 dummy write
//-------------------------------------------------
static int TCGNFIF_G4DmyWr(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);

    MEM_CLR(pTcgTmpBuf, CFG_UDATA_PER_PAGE);
    pTcgMsg->laas = TCG_DUMMY_LAA;
    pTcgMsg->laae = TCG_DUMMY_LAA + 1;
    pTcgMsg->pBuffer = pTcgTmpBuf;

    return TCG_G4Wr(pTcgMsg);
}

//-------------------------------------------------
//  set group address
//-------------------------------------------------
static int TCGNFIF_SETGrpAddr(MsgTcgGroup_t* pTcgMsg)
{
    HERE(NULL);

    pG1 = (tG1 *)pTcgMsg->pG1Tbl;
    pG2 = (tG2 *)pTcgMsg->pG2Tbl;
    pG3 = (tG3 *)pTcgMsg->pG3Tbl;
    pG4 = (tG4 *)pTcgMsg->pG4Tbl;
    pG5 = (tG5 *)pTcgMsg->pG5Tbl;


    //TcgG4NxtPnt.all = 0xFFFF;   //alexdebug
    //TcgG4CurPnt.all = 0x0000;   //alexdebug

    //TcgG5NxtPnt.all = 0xFFFF;   //alexdebug
    //TcgG5CurPnt.all = 0x0000;   //alexdebug

    return zOK;
}

//-------------------------------------------------
//  set buffer address
//-------------------------------------------------
static int TCGNFIF_SETBufAddr(MsgTcgBuffer_t* pTcgMsg)
{
    HERE(NULL);

    #if 0
        if(BLKS_NUM_PER_SUBLK < TCG_MBR_CELLS){
            M_PRINTF("!!!Error, FTL provide super block less 128 blocks, check out!\n\n\n");
            return zNG;
        }
    #endif


    pTcgTmpBuf      =  (U8 *)pTcgMsg->pTmpBuf;
    pMBR_TEMPL2PTBL = (tTcgLogAddr *)pTcgMsg->pTmpMBRL2PTbl;
    pDS_TEMPL2PTBL  = (tTcgLogAddr *)pTcgMsg->pTmpDSL2PTbl;

    // due to sEnabledLockingTable declare at cpu0 B0TCM, CPU2 can not access that. so sEnabledLockingTable | CPU0_B0TCM_NC_OFFSET for CPU2 accessing
    pLockingRangeTable = (sEnabledLockingTable *)((U32)pTcgMsg->pLockingRangeTbl | AXI_B0TCM_CPU0_OFFSET);
    return zOK;
}

//-------------------------------------------------
//  Gx Read
//-------------------------------------------------
static int TCGNFIF_GxRd(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    if(TCG_G4Rd(pTcgMsg) == zNG){
        if(TCG_G5Rd(pTcgMsg) == zNG) return zNG;
    }
    return zOK;
}

//-------------------------------------------------
//  clear Cache
//-------------------------------------------------
static int TCGNFIF_CLRCache(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    //Cache_Reset();
    return zOK;
}

//-------------------------------------------------
//  init Nor Eeprom
//-------------------------------------------------
static int TCGNFIF_NorEepInit(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    TCG_NorEepInit();
    return zOK;
}

//-------------------------------------------------
//  Read Nor Eeprom
//-------------------------------------------------
static int TCGNFIF_NorEepRd(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    TCG_NorEepRead();
    return zOK;
}

//-------------------------------------------------
//  Write Nor Eeprom
//-------------------------------------------------
static int TCGNFIF_NorEepWr(MsgTcg_t* pTcgMsg)
{
    HERE(NULL);
    TCG_NorEepWrite();
    return zOK;
}

//-------------------------------------------------
//  G4 read retry
//-------------------------------------------------
int G4RdRetry(void)
{
    HERE(NULL);
    return zOK;
}

//-------------------------------------------------
//  G5 read retry
//-------------------------------------------------
int G5RdRetry(void)
{
    HERE(NULL);
    return zOK;
}


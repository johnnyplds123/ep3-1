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

#include "HalNfif.h"
#include "SysInfo.h"
#include "Ftl.h"
#include "Cache.h"
#include "Fio.h"
#include "Heap.h"
#include "CoreMsg.h"
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
extern tFIO_JOB gFioJobBuf[FIO_MAX_JOB];

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
S32 TCG_Rd1Pg(tTcgLogAddr readpnt, tTcgGrpDef grp);
S32 TCG_Wr1Pg(tTcgLogAddr writepnt, tTcgGrpDef grp, U32 Aux0, U32 Aux1);
S32 G4_RelpaceDefectBlock(U8 DftBlk);
S32 G5_RelpaceDefectBlock(U8 DftBlk);
S32 TCG_ErrHandling(tTcgGrpDef grp, MsgTcg_t *pTcgMsg);
S32 TCG_ReadRetry(tTcgGrpDef grp, U16* errNativeBlkCnt, MsgTcg_t *pTcgMsg);

U16 Tcg_G4FreeBlk(void);
U16 Tcg_G5FreeBlk(void);
void CnvAdrLog2Phy(tTcgLogAddr logadr, tTcgGrpDef grp, tPAA *mypaa);

void FillFragInPage(tPAA *pPaa);
void FillPaa(U8 ipage, tTcgLogAddr writepnt, tTcgGrpDef grp);
void FillAux(U8 ipage, U32 aux0, U32 aux1);

static void TCG_ERR_HANDLE_ReadRetryTrkDone(U32 trkId, PVOID pStatus);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
tTcgLogAddr TcgG4Pnt, TcgG5Pnt;
tTcgLogAddr TcgG4CurPnt, TcgG4NxtPnt;
tTcgLogAddr TcgG5CurPnt, TcgG5NxtPnt;
U32 TcgG4CurHistNo, TcgG4NxtHistNo;
U32 TcgG5CurHistNo, TcgG5NxtHistNo;
U8  TcgG4Defects,TcgG5Defects;

U8  ValidBlks[32];
U16 ValidCnt;
U16 G4_FreeBlk = TCG_MBR_CELLS;
U16 G5_FreeBlk = TCG_MBR_CELLS;

U16 gTcgMbrCellVac[TCG_MBR_CELLS];               // 256
U16 gTcgTempMbrCellVac[TCG_MBR_CELLS];           // 256

#if _NAND_ == NAND_3DTLCG2
    U32 gTcgMbrCellValidMap[TCG_MBR_CELLS][18];       // 8K
    U32 gTcgTempMbrCellValidMap[TCG_MBR_CELLS][18];   // 8K
#elif _NAND_ == NAND_MLC
    U32 gTcgMbrCellValidMap[TCG_MBR_CELLS][8];       // 4K
    U32 gTcgTempMbrCellValidMap[TCG_MBR_CELLS][8];   // 4K
#endif

__align(4) tPAA gTcgPaa[MAX_DEVICES_NUM_PER_ROW  /* MAX_PAGE_PER_TRK */ * PAA_NUM_PER_PAGE];  // 4 paa per page
__align(4) tTcgAux gTcgAux[MAX_DEVICES_NUM_PER_ROW  /* MAX_PAGE_PER_TRK */ * PAA_NUM_PER_PAGE];  // 4 paa per page

tTcgRdRetry gTcgRdRetry;
__align(4) tTcgLogAddr gNativePaa[MAX_DEVICES_NUM_PER_ROW  /* MAX_PAGE_PER_TRK */];

U8 OnePgSzBuf[0x4000];              // 16K
tTcgLogAddr WrPntBox[MAX_DEVICES_NUM_PER_ROW  /* MAX_PAGE_PER_TRK */];

tFIO_JOB* pTcgJob = NULL;

#ifdef _ALEX_VER_OK
static tSEQ_APP_CALLBACK orgTcgErrTrkDoneCallback;
#endif
//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
extern tG1      *pG1;
extern tG2      *pG2;
extern tG3      *pG3;
extern tG4      *pG4;
extern tG5      *pG5;
//extern U32      gOpenRow[MAX_SUPPORTED_CHANNELS * MAX_SUPPORTED_DEVICES * MAX_SUPPORTED_LUNS * MAX_SUPPORTED_PLANES];
extern U32      FirstTx_ssdStartIdx_Record_flag;

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern void Cache_Alloc_TCG(tFIO_JOB* pJob);
extern void Core_JobWait(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
///----------------------------------------
/// Read Launched
///----------------------------------------
int TCG_ReadLaunched(U16 pagecnts, U8 *pBuffer, MsgTcg_t *pTcgMsg)
{
    U16 idx;

    A_HERE;
    Core_JobWait();   // wait previous all job down.

    pTcgJob          = FIO_PopJob(FIO_JOB_POOL_FREE);
    pTcgJob->pBuffer = pBuffer;
    pTcgJob->plane   = FIO_CMD_1PLANE;
    pTcgJob->paaCnt  = pagecnts * PAA_NUM_PER_PAGE;
    pTcgJob->flag    = FIO_F_SLC | FIO_F_AUX | TCG_FIO_F_MUTE | FIO_F_NO_MKERR | FIO_F_NO_RR;
    pTcgJob->cmd     = FIO_JOB_CMD_READ_SLC;
    pTcgJob->css     = CACHE_TCG;

    Cache_Alloc_TCG(pTcgJob);
    if (pTcgMsg)
    {
        if (FirstTx_ssdStartIdx_Record_flag == 0x53415645)
        {
            pTcgMsg->ssdIndex = pTcgJob->ssdIdx[SSD_WO_CACHE];
            FirstTx_ssdStartIdx_Record_flag = 0x0;
        }

        A_LANDMARK(pTcgMsg->ssdIndex);
    }

    MEM_COPY(pTcgJob->paa, gTcgPaa, sizeof(tPAA) * pagecnts * PAA_NUM_PER_PAGE);
    dumpPaa(pagecnts);

    Core_JobIssue(pTcgJob);
    Core_JobWait();

    // read AUX
    MEM_COPY(&gTcgAux[0], pTcgJob->aux, sizeof(tTcgAux) * pagecnts * PAA_NUM_PER_PAGE);

    if (pTcgJob->status & (FIO_S_UNC_ERR | FIO_S_BLANK_ERR | FIO_S_UXP_ERR))
    {
        if (TCG_FIO_F_MUTE != FIO_F_MUTE)
        {
            TCG_PRINTF("TCG_ReadLaunched Error... %x\n", pTcgJob->status);
            TCG_PRINTF("zNG----------------------------\n\n");
        }

        return zNG;
    }

    return zOK;
}

///----------------------------------------
/// Write Launched
///----------------------------------------
int TCG_WriteLaunched(U16 pagecnts, U8 *pBuffer)
{
    A_HERE;
    Core_JobWait();   // wait previous all job down.

    pTcgJob          = FIO_PopJob(FIO_JOB_POOL_FREE);
    pTcgJob->pBuffer = pBuffer;
    pTcgJob->plane   = FIO_CMD_1PLANE;
    pTcgJob->paaCnt  = pagecnts * PAA_NUM_PER_PAGE;
    pTcgJob->flag    = FIO_F_SLC | FIO_F_NO_MKERR;
    pTcgJob->cmd     = FIO_JOB_CMD_WRITE;
    pTcgJob->css     = CACHE_TCG;

    MEM_COPY(pTcgJob->aux, &gTcgAux[0], sizeof(tTcgAux) * pagecnts * PAA_NUM_PER_PAGE);

    Cache_Alloc_TCG(pTcgJob);
    MEM_COPY(pTcgJob->paa, gTcgPaa, sizeof(tPAA)*pagecnts*PAA_NUM_PER_PAGE);
    dumpPaa(pagecnts);

    Core_JobIssue(pTcgJob);
    Core_JobWait();

    if (pTcgJob->status & FIO_S_PROG_ERR)
    {
        TCG_PRINTF("TCG_WriteLaunched Error...%x\n", pTcgJob->status);
        TCG_PRINTF("zNG----------------------------\n\n");
        return zNG;
    }

    return zOK;
}

///----------------------------------------
/// Erase Launched
///----------------------------------------
int TCG_EraseLaunched(U16 blkcnts)
{
    A_HERE;
    Core_JobWait();

    pTcgJob         = FIO_PopJob(FIO_JOB_POOL_FREE);
    pTcgJob->plane  = FIO_CMD_1PLANE;
    pTcgJob->paaCnt = blkcnts;
    pTcgJob->flag   = FIO_F_SLC | FIO_F_NO_MKERR;
    pTcgJob->cmd    = FIO_JOB_CMD_ERASE;
    pTcgJob->css    = CACHE_TCG;

    {
        U16 i;

        for (i = 0; i < blkcnts; i++)
        {
            pTcgJob->paa[i].all32 = gTcgPaa[i*PAA_NUM_PER_PAGE].all32;
        }
    }
    dumpPaa(blkcnts);

    Core_JobIssue(pTcgJob);
    Core_JobWait();

    if (pTcgJob->status & FIO_S_ERASE_ERR)
    {
        TCG_PRINTF("TCG_EraseLaunched Error...%x\n", pTcgJob->status);
        TCG_PRINTF("zNG----------------------------\n\n");
        return zNG;
    }

    return zOK;
}

///----------------------------------------
///  read row
///----------------------------------------
int TCG_RdMulChan(tTcgGrpDef grp, tTcgLogAddr ReadPnt, U32 SkipChanMap)
{
    U16 i;
    U8 x, loops;
    tTcgLogAddr real_ReadPnt;

    //soutb3(0x5C, 0xFF, 0x56);            //TcgReadMulChan()
    A_HERE;
    for(x=loops=0; x<DEVICES_NUM_PER_ROW; x++){
        if(!(SkipChanMap & mBIT(x))) loops++;
    }
    if(loops == 0) return FALSE; //zNG;

    real_ReadPnt.all = ReadPnt.all;
    i = 0;
    for(x=0; x<DEVICES_NUM_PER_ROW; x++){
        if(!(SkipChanMap & mBIT(x))){
            real_ReadPnt.pc.cell = ReadPnt.pc.cell + x;
            FillPaa(i, real_ReadPnt, grp);
            ValidBlks[i] = real_ReadPnt.pc.cell;
            i++;
        }
    }
    ValidCnt = i;

    return (TCG_ReadLaunched(loops, pTcgTmpBuf, NULL));

}

///----------------------------------------
///  write row
///----------------------------------------
int TCG_WrMulChan(tTcgGrpDef grp, tTcgLogAddr WritePnt, U32 SkipChanMap)
{
    U16 i;
    U8 x, loops;
    tTcgLogAddr real_WritePnt;

    //soutb3(0x5C, 0xFF, 0x56);            //TcgWriteMulChan()
    A_HERE;
    for(x=loops=0; x<DEVICES_NUM_PER_ROW; x++){
        if(!(SkipChanMap & mBIT(x))) loops++;
    }
    if(loops==0) return FALSE; //zNG;

    real_WritePnt.all = WritePnt.all;
    i=0;
    for(x=0; x<DEVICES_NUM_PER_ROW; x++){
        if(!(SkipChanMap & mBIT(x))){
            real_WritePnt.pc.cell     =   WritePnt.pc.cell + x;
            FillPaa(i, real_WritePnt, grp);
            FillAux(i, real_WritePnt.all, real_WritePnt.all);
            ValidBlks[i] = real_WritePnt.pc.cell;
            i++;
        }
    }
    ValidCnt = i;

    return (TCG_WriteLaunched(loops, pTcgTmpBuf));
}


///----------------------------------------
///  erase row
///----------------------------------------
int TCG_ErMulChan(tTcgGrpDef grp, tTcgLogAddr ErasePnt, U32 SkipChanMap)
{
    U16 i;
    U8 x, loops;
    tTcgLogAddr EraseBlkAddr;

    //soutb3(0x5C, 0xFF, 0x53);  //TcgEraseMulChan()
    A_HERE;
    for(x=loops=0; x<DEVICES_NUM_PER_ROW; x++){
        if(!(SkipChanMap & mBIT(x))) loops++;
    }
    if(loops==0) return FALSE; //zNG;

    EraseBlkAddr.all = ErasePnt.all;
    i=0;
    for(x=0; x<DEVICES_NUM_PER_ROW; x++){
        if(!(SkipChanMap & mBIT(x))){  // It's not defected block
            EraseBlkAddr.pc.cell      =   ErasePnt.pc.cell + x;
            FillPaa(i, EraseBlkAddr, grp);
            ValidBlks[i] = EraseBlkAddr.pc.cell;
            if(grp > ZONE_DS){
                tcgG5EraCnt[EraseBlkAddr.pc.cell]++;
            }else{
                tcgG4EraCnt[EraseBlkAddr.pc.cell]++;
            }
            i++;
        }
    }
    ValidCnt = i;

    return (TCG_EraseLaunched(loops));
}

///----------------------------------------
///  read 1 NG page
///----------------------------------------
int TCG_Rd1RtyPg(tTcgLogAddr readpnt, tTcgGrpDef grp)
{
    //A_HERE;
    HERE(readpnt.all);
    //gNativePaa[0].all = readpnt.all;   // TCG_Rd1Pg no read retry currently.
    FillPaa(0, readpnt, grp);
    return (TCG_ReadLaunched(1, OnePgSzBuf, NULL));
}

///----------------------------------------
///  read 1 page
///----------------------------------------
int TCG_Rd1Pg(tTcgLogAddr readpnt, tTcgGrpDef grp)
{
    U16 errNativeBlkCnt;

    //HERE(readpnt.all);
    gNativePaa[0].all = readpnt.all;   // TCG_Rd1Pg no read retry currently.
    FillPaa(0, readpnt, grp);
    if(TCG_ReadLaunched(1, OnePgSzBuf, NULL) == zNG){
        if(TCG_ReadRetry(grp, &errNativeBlkCnt, NULL) == zNG){   // errNativeBlkCnt doesn't use in this TCG_Rd1Pg()
            return zNG;
        }
    }

    return zOK;
}

///----------------------------------------
///  write 1 page
///----------------------------------------
int TCG_Wr1Pg(tTcgLogAddr writepnt, tTcgGrpDef grp, U32 Aux0, U32 Aux1)
{
    //HERE(writepnt.all);
    FillPaa(0, writepnt, grp);
    FillAux(0, Aux0, Aux1);
    return (TCG_WriteLaunched(1, OnePgSzBuf));
}

///----------------------------------------
/// group 4 read L2P
///----------------------------------------
int TCG_G4RdL2p(void)
{
    tTcgLogAddr  readpnt;
    U8 ipage;
    U16 errNativeBlkCnt;

    HERE(NULL);  //TCG_G4RdL2p()

    readpnt.all=TcgG4Pnt.all;

    for(ipage=0; ipage<L2P_PAGE_CNT; ipage++){
        gNativePaa[ipage].all = readpnt.all;
        FillPaa(ipage, readpnt, ZONE_GRP4);
        readpnt.all += SLC_OFF(1);
    }

    if(TCG_ReadLaunched(L2P_PAGE_CNT, (U8 *)&(pG4->b.TcgG4Header), NULL) == zNG){
        if(TCG_ReadRetry(ZONE_GRP4, &errNativeBlkCnt, NULL) == zNG){   // errNativeBlkCnt doesn't use in this TCG_G4RdL2p()
            return zNG;
        }
    }

    return zOK;
}

///----------------------------------------
/// group 4 write L2P
///----------------------------------------
int TCG_G4WrL2p(void)
{
    tTcgLogAddr writepnt;
    U8 ipage;

    HERE(NULL);  //TCG_G4WrL2p()

    writepnt.all = TcgG4NxtPnt.all;
    FillPaa(0, writepnt, ZONE_GRP4);
    if(TCG_EraseLaunched(1) == zNG) return zNG;
    tcgG4EraCnt[writepnt.pc.cell]++;

#ifdef TCG_EEP_NOR
    TCG_NorEepWrite();
#else
    SyIn_Synchronize(SI_AREA_MISC, SYSINFO_WRITE, SI_SYNC_BY_TCG);
#endif

    TcgG4Pnt.all            = TcgG4NxtPnt.all;
    TcgG4CurPnt.all         = TcgG4NxtPnt.all;
    TcgG4NxtPnt.pc.page    += SLC_OFF(L2P_PAGE_CNT);

    TcgG4CurHistNo = TcgG4NxtHistNo;

    for(ipage=0; ipage<L2P_PAGE_CNT; ipage++){
        FillPaa(ipage, writepnt, ZONE_GRP4);
        FillAux(ipage, TCG_G4_TAG, TcgG4NxtHistNo);
        writepnt.pc.page += SLC_OFF(1);
    }

    TcgG4NxtHistNo++;
    return (TCG_WriteLaunched(L2P_PAGE_CNT, (U8 *)&(pG4->b.TcgG4Header)));
}
//----------------------------------------
// group 4 read (pg->ce->ch->blk->lun)
//----------------------------------------
int TCG_G4Rd(MsgTcg_t *pTcgMsg)
{
    tTcgLogAddr myRdPnt;
    U16 laaCnt = pTcgMsg->laae - pTcgMsg->laas;
    U8 ipage;

    A_HERE;  //TCG_G4Rd()
    for(ipage = 0; ipage < laaCnt; ipage++){
        myRdPnt.all = pG4->b.TcgMbrL2P[pTcgMsg->laas + ipage].all;
        gNativePaa[ipage].all = myRdPnt.all;
        FillPaa(ipage, myRdPnt, ZONE_GRP4);
    }
    if(TCG_ReadLaunched(laaCnt, pTcgMsg->pBuffer, pTcgMsg) == zNG){
        if(TCG_ErrHandling(ZONE_GRP4, pTcgMsg) == zNG){
            for(ipage=0; ipage < laaCnt; ipage++){
                if(pG4->b.TcgMbrL2P[pTcgMsg->laas + ipage].pc.cell >= TCG_MBR_CELLS){  //blank?
                    MEM_CLR((U8*)pTcgMsg->pBuffer + (ipage * CFG_UDATA_PER_PAGE) , CFG_UDATA_PER_PAGE);
                }
            }
            if (pTcgJob->status & (FIO_S_BLANK_ERR)){
                return zOK;
            }
            return zNG;
        }
    }

    return zOK;
}

//----------------------------------------
// group 4 write (pg->ce->ch->blk->lun)
//----------------------------------------
int TCG_G4Wr(MsgTcg_t *pTcgMsg)
{
    S32 sts;
    U32 ilaa;
    tTcgLogAddr  writepnt;
    U16 laaCnt = pTcgMsg->laae - pTcgMsg->laas;

    A_HERE;

    writepnt.all = TCG_LAA_INIT_VAL;
    for(ilaa = 0; ilaa < laaCnt; ilaa++)
    {
        if(TcgG4NxtPnt.all == TCG_LAA_INIT_VAL)
        {
            // build vac & validmap
            TcgMbrBuildGcTables();
            G4_FreeBlk = Tcg_G4FreeBlk();

            // find a vac0 (excluding defect)
            TcgG4NxtPnt.pc.cell = TcgG4CurPnt.pc.cell;
            TcgG4NxtPnt.pc.page = 0;
            while(1)
            {
                TcgG4NxtPnt.pc.cell++;
                if(TcgG4NxtPnt.pc.cell >= TCG_MBR_CELLS) TcgG4NxtPnt.pc.cell = 0;
                if((tcgG4Dft[TcgG4NxtPnt.pc.cell] == 0)
                && (gTcgMbrCellVac[TcgG4NxtPnt.pc.cell] == 0))
                {
                    break;
                }
            }
            // open block (write L2P pages)
            TCG_G4WrL2p();
        }

        pG4->b.TcgMbrL2P[pTcgMsg->laas + ilaa].all = WrPntBox[ilaa].all = writepnt.all = TcgG4CurPnt.all = TcgG4NxtPnt.all;
        TcgG4NxtPnt.all += SLC_OFF(1);
        if(TcgG4NxtPnt.pc.page == PAGE_NUM_PER_BLOCK)
        {
            TcgG4NxtPnt.all = TCG_LAA_INIT_VAL;
        }
    }

    for(ilaa = 0; ilaa < laaCnt; ilaa++)
    {
        FillPaa(ilaa, WrPntBox[ilaa], ZONE_GRP4);
        FillAux(ilaa, pTcgMsg->laas + ilaa, TcgG4CurHistNo);
    }

    sts = TCG_WriteLaunched(laaCnt, pTcgMsg->pBuffer);

    if(sts == zOK)
    {
        if(G4_FreeBlk < TCG_GC_THRESHOLD)
        {  // GC threshold = 3
            G4_GC();
        }
    }
    return sts;
}

///----------------------------------------
/// group 5 read L2P
///----------------------------------------
int TCG_G5RdL2p(void)
{
    tTcgLogAddr  readpnt;
    U8 ipage;
    U16 errNativeBlkCnt;

    HERE(NULL);  //TCG_G5RdL2p()

    readpnt.all=TcgG5Pnt.all;

    for(ipage=0; ipage<L2P_PAGE_CNT; ipage++){
        gNativePaa[ipage].all = readpnt.all;
        FillPaa(ipage, readpnt, ZONE_GRP5);
        readpnt.all += SLC_OFF(1);
    }

    if(TCG_ReadLaunched(L2P_PAGE_CNT, (U8 *)&(pG5->b.TcgG5Header), NULL) == zNG){
        if(TCG_ReadRetry(ZONE_GRP5, &errNativeBlkCnt, NULL) == zNG){   // errNativeBlkCnt doesn't use in this TCG_G5RdL2p()
            return zNG;
        }
    }

    return zOK;
}

///----------------------------------------
/// group 5 write L2P
///----------------------------------------
int TCG_G5WrL2p(void)
{
    tTcgLogAddr  writepnt;
    U8 ipage;

    HERE(NULL);  //TCG_G5WrL2p()

    writepnt.all = TcgG5NxtPnt.all;
    FillPaa(0, writepnt, ZONE_GRP5);
    if(TCG_EraseLaunched(1) == zNG) return zNG;
    tcgG5EraCnt[writepnt.pc.cell]++;

#ifdef TCG_EEP_NOR
    TCG_NorEepWrite();
#else
    SyIn_Synchronize(SI_AREA_MISC, SYSINFO_WRITE, SI_SYNC_BY_TCG);
#endif

    TcgG5Pnt.all            = TcgG5NxtPnt.all;
    TcgG5CurPnt.all         = TcgG5NxtPnt.all;
    TcgG5NxtPnt.pc.page    += SLC_OFF(L2P_PAGE_CNT);

    TcgG5CurHistNo = TcgG5NxtHistNo;

    for(ipage = 0; ipage < L2P_PAGE_CNT; ipage++){
        FillPaa(ipage, writepnt, ZONE_GRP5);
        FillAux(ipage, TCG_G5_TAG, TcgG5NxtHistNo);
        writepnt.pc.page += SLC_OFF(1);
    }

    TcgG5NxtHistNo++;
    return (TCG_WriteLaunched(L2P_PAGE_CNT, (U8 *)&(pG5->b.TcgG5Header)));
}

//----------------------------------------
// group 5 read (pg->ce->ch->blk->lun)
//----------------------------------------
int TCG_G5Rd(MsgTcg_t *pTcgMsg)
{
    tTcgLogAddr myRdPnt;
    U16 laaCnt = pTcgMsg->laae - pTcgMsg->laas;
    U8 ipage;

    A_HERE;  //TCG_G5Rd()
    for(ipage=0; ipage < laaCnt; ipage++){
        myRdPnt.all = pG5->b.TcgTempMbrL2P[pTcgMsg->laas + ipage].all;
        gNativePaa[ipage].all = myRdPnt.all;
        FillPaa(ipage, myRdPnt, ZONE_GRP5);
    }
    if(TCG_ReadLaunched(laaCnt, pTcgMsg->pBuffer, pTcgMsg) == zNG){
        if(TCG_ErrHandling(ZONE_GRP5, pTcgMsg) == zNG){
            for(ipage=0; ipage < laaCnt; ipage++){
                if(pG5->b.TcgTempMbrL2P[pTcgMsg->laas + ipage].pc.cell >= TCG_MBR_CELLS){  //blank ?
                    MEM_CLR((U8*)pTcgMsg->pBuffer + (ipage * CFG_UDATA_PER_PAGE) , CFG_UDATA_PER_PAGE);
                }
            }
            return zNG;
        }
    }
    return zOK;
}

//----------------------------------------
// group 5 write (pg->ce->ch->blk->lun)
//----------------------------------------
int TCG_G5Wr(MsgTcg_t *pTcgMsg)
{
    S32 sts;
    U32 ilaa;
    tTcgLogAddr writepnt;
    U16 laaCnt = pTcgMsg->laae - pTcgMsg->laas;

    A_HERE;

    writepnt.all = TCG_LAA_INIT_VAL;
    for(ilaa = 0; ilaa < laaCnt; ilaa++)
    {
        if(TcgG5NxtPnt.all == TCG_LAA_INIT_VAL)
        {
            // build vac & validmap
            TcgTempMbrBuildGcTables();
            G5_FreeBlk = Tcg_G5FreeBlk();

            // find a vac0 (excluding defect)
            TcgG5NxtPnt.pc.cell = TcgG5CurPnt.pc.cell;
            TcgG5NxtPnt.pc.page = 0;
            while(1)
            {
                TcgG5NxtPnt.pc.cell++;
                if(TcgG5NxtPnt.pc.cell >= TCG_MBR_CELLS) TcgG5NxtPnt.pc.cell = 0;
                if((tcgG5Dft[TcgG5NxtPnt.pc.cell] == 0)
                && (gTcgTempMbrCellVac[TcgG5NxtPnt.pc.cell] == 0))
                {
                    break;
                }
            }
            // open block (write L2P pages)
            TCG_G5WrL2p();

        }

        pG5->b.TcgTempMbrL2P[pTcgMsg->laas + ilaa].all = WrPntBox[ilaa].all = writepnt.all = TcgG5CurPnt.all = TcgG5NxtPnt.all;
        TcgG5NxtPnt.all += SLC_OFF(1);
        if(TcgG5NxtPnt.pc.page == PAGE_NUM_PER_BLOCK)
        {
            TcgG5NxtPnt.all = TCG_LAA_INIT_VAL;
        }
    }

    for(ilaa = 0; ilaa < laaCnt; ilaa++)
    {
        FillPaa(ilaa, WrPntBox[ilaa], ZONE_GRP5);
        FillAux(ilaa, pTcgMsg->laas + ilaa, TcgG5CurHistNo);
    }

    sts = TCG_WriteLaunched(laaCnt, pTcgMsg->pBuffer);

    if(sts == zOK)
    {
        if(G5_FreeBlk < TCG_GC_THRESHOLD)
        {   // GC threshold = 3
            G5_GC();
        }
    }
    return sts;
}

//----------------------------------------
// Nor eeprom init
//----------------------------------------
void TCG_NorEepInit(void)
{
    U32 myTcgEepProgIdx;
    U32 HasWritten;
    U16 i;

    Core_JobWait();     //HLD_FIO_WaitComplete();         // wait sequencer done
    tcgEepProgIdx = 0;
    HasWritten = DWORD_MASK;
    for (i = 0; i < SFH_TCG_COUNT; i++)
    {
        HalSflash_Copy((PVOID)(&myTcgEepProgIdx), (SFH_TCG_START + i) * SFLASH_SECTOR_SIZE, sizeof(U32));
        if (myTcgEepProgIdx != DWORD_MASK && myTcgEepProgIdx > tcgEepProgIdx)
        {
            tcgEepProgIdx = myTcgEepProgIdx;
        }
        HasWritten &= myTcgEepProgIdx;
    }

    if(HasWritten == DWORD_MASK)   //new NOR, first time ?
    {

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

        // ID : record defected ID in NOR
        if(strcmp((char *)tcgDefectID, DEFECT_STRING) != 0){   // first time ? , if so, then init defect table
            strcpy((char *)tcgDefectID,DEFECT_STRING);          // ID
        }
        // ID : record erased count ID in NOR
        if(strcmp((char *)tcgErasedCntID, ERASED_CNT_STRING) != 0){   // first time ? , if so, then init erased count table
            strcpy((char *)tcgErasedCntID,ERASED_CNT_STRING);   // ID
        }

        TCG_NorEepWrite();
    }else{
        TCG_NorEepRead();
    }
    tcgEepProgIdx++;  //next write sector index
}

//----------------------------------------
// Nor eeprom read
//----------------------------------------
void TCG_NorEepRead(void)
{
    U32 NorSctNo;

    NorSctNo = (tcgEepProgIdx % SFH_TCG_COUNT);

    HalSflash_Copy((PVOID)(&tcgEepProgIdx), (SFH_TCG_START + NorSctNo) * SFLASH_SECTOR_SIZE, sizeof(smSysInfo->d.TcgData.d.TCGUsed));

}

//----------------------------------------
// Nor eeprom write
//----------------------------------------
void TCG_NorEepWrite(void)
{
    U32 NorSctNo;
    U32 i;

    NorSctNo = (tcgEepProgIdx % SFH_TCG_COUNT);

    HalSflash_EraseSector(SFH_TCG_START + NorSctNo, 1, 3);
    HalSflash_ProgramU32((PVOID)(&tcgEepProgIdx), (SFH_TCG_START + NorSctNo) * SFLASH_SECTOR_SIZE, sizeof(smSysInfo->d.TcgData.d.TCGUsed));

    tcgEepProgIdx++;
    if(tcgEepProgIdx == DWORD_MASK){
        tcgEepProgIdx = 0;
        for(i=0; i<SFH_TCG_COUNT; i++){
            HalSflash_EraseSector(SFH_TCG_START + i, 1, 3);
            HalSflash_ProgramU32((PVOID)(&tcgEepProgIdx), (SFH_TCG_START + i) * SFLASH_SECTOR_SIZE, sizeof(smSysInfo->d.TcgData.d.TCGUsed));
            tcgEepProgIdx++;
        }

    }

}

//----------------------------------------
// group 4 build Vac table
//----------------------------------------
void TcgMbrBuildGcTables(void)
{
    U16 laa;
    tTcgLogAddr  ptrk;

    MEM_CLR(gTcgMbrCellVac, sizeof(gTcgMbrCellVac));
    MEM_CLR(gTcgMbrCellValidMap, sizeof(gTcgMbrCellValidMap));
    for(laa = 0; laa < G4G5_LAA_AMOUNT_LIMIT; laa++)
    {
        ptrk.all = pG4->b.TcgMbrL2P[laa].all;
        if(ptrk.pc.cell < TCG_MBR_CELLS)
        {
            gTcgMbrCellVac[ptrk.pc.cell]++;
            gTcgMbrCellValidMap[ptrk.pc.cell][ptrk.pc.page/32] |= mBIT(ptrk.pc.page%32);
        }
    }
}

//----------------------------------------
// group 5 build Vac table
//----------------------------------------
void TcgTempMbrBuildGcTables(void)
{
    U16 laa;
    tTcgLogAddr  ptrk;

    MEM_CLR(gTcgTempMbrCellVac, sizeof(gTcgTempMbrCellVac));
    MEM_CLR(gTcgTempMbrCellValidMap, sizeof(gTcgTempMbrCellValidMap));
    for(laa=0; laa<G4G5_LAA_AMOUNT_LIMIT; laa++ ){
        ptrk.all = pG5->b.TcgTempMbrL2P[laa].all;
        if(ptrk.pc.cell < TCG_MBR_CELLS){
            gTcgTempMbrCellVac[ptrk.pc.cell]++;
            gTcgTempMbrCellValidMap[ptrk.pc.cell][ptrk.pc.page/32] |= mBIT(ptrk.pc.page%32);
        }
    }
}

//----------------------------------------
// group 4 GC
//----------------------------------------
int G4_GC(void)
{
    tTcgLogAddr  writepnt, readpnt;
    U16     MinVac;
    U16     x,y;
    U16     gclaa;
    U32     i;
    int     sts;

    writepnt.all = readpnt.all = TCG_LAA_INIT_VAL;
    for(i = 0; ; i++)
    {
        MinVac = (PAGE_NUM_PER_BLOCK / SLC_OFF(1)) - L2P_PAGE_CNT;
        TcgMbrBuildGcTables();
        G4_FreeBlk = Tcg_G4FreeBlk();
#if 0
        // gc 1.decide gc block
        y = TcgG4CurPnt.pc.cell;
        for(x=0; ;x++){
            y++;
            if(y >= TCG_MBR_CELLS) y=0;
            if(y == TcgG4CurPnt.pc.cell) continue;
            if(tcgG4Dft[y]) continue;
            if(gTcgMbrCellVac[y] < ((x/(PAGE_NUM_PER_BLOCK/4))+1)) break;
        }
#else
        // gc 1.decide gc block
        y = 0xFFFF;
        for(x=0; x<TCG_MBR_CELLS; x++)
        {
            if(x == TcgG4CurPnt.pc.cell) continue;
            if(gTcgMbrCellVac[x] == 0) continue;
            if(tcgG4Dft[x]) continue;
            if(gTcgMbrCellVac[x] < MinVac)
            {
                MinVac = gTcgMbrCellVac[x];
                y = x;
            }
        }
#endif
        TCG_PRINTF("No=%2X Vac=%X, CurWrBlk%X, GCBlk%X\n",i, gTcgMbrCellVac[y], TcgG4CurPnt.pc.cell, y);

        // gc 2.move all valid page from gc block to the open block
        if(gTcgMbrCellVac[y] && (y < TCG_MBR_CELLS))
        {
            readpnt.pc.cell = y ;
            for(x=SLC_OFF(L2P_PAGE_CNT); x<PAGE_NUM_PER_BLOCK; x+=SLC_OFF(1))
            {
                readpnt.pc.page = x;
                if(gTcgMbrCellValidMap[y][x/32]&(mBIT(x%32)))
                {
                    TCG_Rd1Pg(readpnt, ZONE_GRP4);
                    gclaa = gTcgAux[0].aux0;
                    if(gclaa >= G4G5_LAA_AMOUNT_LIMIT) continue; // if illegal laa

                    writepnt.all = TcgG4CurPnt.all = TcgG4NxtPnt.all;
                    TcgG4NxtPnt.all += SLC_OFF(1);

                    pG4->b.TcgMbrL2P[gclaa].all = writepnt.all;
                    TCG_Wr1Pg(writepnt, ZONE_GRP4, gclaa, TcgG4NxtHistNo);

                    if(TcgG4NxtPnt.pc.page == PAGE_NUM_PER_BLOCK)
                    {   // this Free block is writed complete ?
                        TcgG4NxtPnt.all = TCG_LAA_INIT_VAL;
                        return zOK;                  // quit for(;;) loop
                    }
                }
            }
        }else{
            return zOK;
        }
    }

}

//----------------------------------------
// group 5 GC
//----------------------------------------
int G5_GC(void)
{
    tTcgLogAddr  writepnt, readpnt;
    U16     MinVac;
    U16     x,y;
    U16     gclaa;
    U32     i;
    int     sts;

    writepnt.all = readpnt.all = TCG_LAA_INIT_VAL;
    for(i = 0; ; i++)
    {
        MinVac = (PAGE_NUM_PER_BLOCK / SLC_OFF(1)) - L2P_PAGE_CNT;
        TcgTempMbrBuildGcTables();
        G5_FreeBlk = Tcg_G5FreeBlk();
#if 0
        // gc 1.decide gc block
        y = TcgG5CurPnt.pc.cell;
        for(x=0; ;x++){
            y++;
            if(y >= TCG_MBR_CELLS) y=0;
            if(y == TcgG5CurPnt.pc.cell) continue;
            if(tcgG5Dft[y]) continue;
            if(gTcgTempMbrCellVac[y] < ((x/(PAGE_NUM_PER_BLOCK/4))+1)) break;
        }
#else
        // gc 1.decide gc block
        y = 0xFFFF;
        for(x=0; x<TCG_MBR_CELLS; x++)
        {
            if(x == TcgG5CurPnt.pc.cell) continue;
            if(gTcgTempMbrCellVac[x] == 0) continue;
            if(tcgG5Dft[x]) continue;
            if(gTcgTempMbrCellVac[x] < MinVac)
            {
                MinVac = gTcgTempMbrCellVac[x];
                y = x;
            }
        }
#endif
        TCG_PRINTF("No=%2X Vac=%X, CurWrBlk%X, GCBlk%X\n",i, gTcgTempMbrCellVac[y], TcgG5CurPnt.pc.cell, y);

        // gc 2.move all valid page from gc block to the open block
        if(gTcgTempMbrCellVac[y] && (y < TCG_MBR_CELLS))
        {
            readpnt.pc.cell = y ;
            for(x = SLC_OFF(L2P_PAGE_CNT); x < PAGE_NUM_PER_BLOCK; x += SLC_OFF(1))
            {
                readpnt.pc.page = x;
                if(gTcgTempMbrCellValidMap[y][x/32] & (mBIT(x % 32)))
                {
                    TCG_Rd1Pg(readpnt, ZONE_GRP5);
                    gclaa = gTcgAux[0].aux0;
                    if(gclaa >= G4G5_LAA_AMOUNT_LIMIT) continue; // if illegal laa

                    writepnt.all = TcgG5CurPnt.all = TcgG5NxtPnt.all;
                    TcgG5NxtPnt.all += SLC_OFF(1);

                    pG5->b.TcgTempMbrL2P[gclaa].all = writepnt.all;
                    TCG_Wr1Pg(writepnt, ZONE_GRP5, gclaa, TcgG5NxtHistNo);

                    if(TcgG5NxtPnt.pc.page == PAGE_NUM_PER_BLOCK)
                    {   // this Free block is writed complete ?
                        TcgG5NxtPnt.all = TCG_LAA_INIT_VAL;
                        return zOK;                  // quit for(;;) loop
                    }
                }
            }
        }else{
            return zOK;
        }
    }

}

//**************************************************************
// TCG error handling
//**************************************************************
//----------------------------------------
// G5 Replace defect block
//----------------------------------------
int G4_RelpaceDefectBlock(U8 DftBlk)
{
#ifdef _ALEX_VER_OK
    tFIO_JOB* pVthJob = NULL;
    tTcgLogAddr  writepnt, readpnt;
    U16     x;
    U16     gclaa;
    U32     i;
    U16     RdRtyCnt;
    U16     rdRtySuccess;

    HERE(NULL);
    // build vac & validmap
    TcgMbrBuildGcTables();
    // find a vac0 (excluding defect)
    TcgG4NxtPnt.pc.cell = DftBlk;
    TcgG4NxtPnt.pc.page = 0;
    for(i = 0; i < TCG_MBR_CELLS; i++)
    {
        TcgG4NxtPnt.pc.cell++;
        if(TcgG4NxtPnt.pc.cell >= TCG_MBR_CELLS) TcgG4NxtPnt.pc.cell = 0;
        if((tcgG4Dft[TcgG4NxtPnt.pc.cell] == 0)
        && (gTcgMbrCellVac[TcgG4NxtPnt.pc.cell] == 0))
        {
            break;
        }
    }
    if(i >= TCG_MBR_CELLS) return zNG; // no anyone blank block was found ?

    // open block (write 5 pages)
    TCG_G4WrL2p();

    TCG_PRINTF("Vac=%X, CurWrBlk%X, DftBlk%X\n", gTcgMbrCellVac[DftBlk], TcgG4CurPnt.pc.cell, DftBlk);

    writepnt.all = readpnt.all = TCG_LAA_INIT_VAL;
    pVthJob   = &gFioJobBuf[FIO_JOB_IDX_VTH];

    // gc 2.move all valid page from gc block to the open block
    if(gTcgMbrCellVac[DftBlk] && (DftBlk < TCG_MBR_CELLS))
    {
        readpnt.pc.cell = DftBlk;
        for(x = SLC_OFF(L2P_PAGE_CNT); x < PAGE_NUM_PER_BLOCK; x += SLC_OFF(1))
        {
            readpnt.pc.page = x;
            if(gTcgMbrCellValidMap[DftBlk][x/32]&(mBIT(x%32)))
            {
                // ----- read -----
                FillPaa(0, readpnt, ZONE_GRP4);
                for(i = 0; i < PAA_NUM_PER_PAGE; i++){
                    pVthJob->paa[i].rr.ch      = gTcgPaa[i].b.ch;
                    pVthJob->paa[i].rr.ce      = gTcgPaa[i].b.ce;
                    pVthJob->paa[i].rr.voltage = 0;
                }
                pVthJob->paaCnt = PAA_NUM_PER_PAGE;

                rdRtySuccess = FALSE;
                for (RdRtyCnt = 0; RdRtyCnt < MAX_VOLTAGE_STAGE; RdRtyCnt++)
                {
                    // --- Reset job desc count and error count
                    pVthJob->status  = 0;
                    pVthJob->descCnt = 0;
                    pVthJob->tid    = AplSeq_AllocTrkId(SEQ_TIMEOUT_WAIT);

                    //--- Open Page
                    MEM_SET(gOpenRow, 0xFF, sizeof(gOpenRow));
                    HLD_FIO_WaitComplete();
                    //
                    orgTcgErrTrkDoneCallback  = HAL_SEQ_RegiserCallback(SEQ_CB_TRK_DONE,  TCG_ERR_HANDLE_ReadRetryTrkDone);
                    HLD_FIO_ForceIssueJob(pVthJob);
                    HLD_FIO_WaitComplete();
                    HAL_SEQ_RegiserCallback(SEQ_CB_TRK_DONE, orgTcgErrTrkDoneCallback);
                    AplSeq_FreeTrkId(pVthJob->tid);
                    //
                    if(TCG_Rd1RtyPg(readpnt, ZONE_GRP4) == zOK)
                    {   //previos parameter for read retry
                        rdRtySuccess = TRUE;
                        //read retry successful
                        break;
                    }
                    else
                    {
                        for(i = 0; i < PAA_NUM_PER_PAGE; i++)
                        {   //change threhold voltage
                            pVthJob->paa[i].rr.voltage++;
                        }
                    }
                }

                if(rdRtySuccess == FALSE)
                {
                    return zNG;
                }

                // ----- write -----
                gclaa = gTcgAux[0].aux0;
                if(gclaa >= G4G5_LAA_AMOUNT_LIMIT) continue; // if illegal laa

                writepnt.all = TcgG4CurPnt.all = TcgG4NxtPnt.all;
                TcgG4NxtPnt.all += SLC_OFF(1);

                pG4->b.TcgMbrL2P[gclaa] = writepnt.all;
                TCG_Wr1Pg(writepnt, ZONE_GRP4, gclaa, TcgG4NxtHistNo);
            }
        }
        if(TcgG4NxtPnt.pc.page == PAGE_NUM_PER_BLOCK)
        {   // this Free block is writed complete ?
            TcgG4NxtPnt.all = TCG_LAA_INIT_VAL;
        }
    }
    else
    {
        return zNG;
    }
#endif
    return zOK;
}

//----------------------------------------
// G5 Replace defect block
//----------------------------------------
int G5_RelpaceDefectBlock(U8 DftBlk)
{
#ifdef _ALEX_VER_OK
    tFIO_JOB* pVthJob = NULL;
    tTcgLogAddr  writepnt, readpnt;
    U16     x;
    U16     gclaa;
    U32     i;
    U16     RdRtyCnt;
    U16     rdRtySuccess;

    HERE(NULL);
    // build vac & validmap
    TcgTempMbrBuildGcTables();
    // find a vac0 (excluding defect)
    TcgG5NxtPnt.pc.cell = DftBlk;
    TcgG5NxtPnt.pc.page = 0;
    for(i = 0; i < TCG_MBR_CELLS; i++)
    {
        TcgG5NxtPnt.pc.cell++;
        if(TcgG5NxtPnt.pc.cell >= TCG_MBR_CELLS) TcgG5NxtPnt.pc.cell = 0;
        if((tcgG5Dft[TcgG5NxtPnt.pc.cell] == 0)
        && (gTcgTempMbrCellVac[TcgG5NxtPnt.pc.cell] == 0))
        {
            break;
        }
    }
    if(i >= TCG_MBR_CELLS)   // no anyone blank block was found ?
        return zNG;

    // open block (write 5 pages)
    TCG_G5WrL2p();

    TCG_PRINTF("Vac=%X, CurWrBlk%X, DftBlk%X\n", gTcgTempMbrCellVac[DftBlk], TcgG5CurPnt.pc.cell, DftBlk);

    writepnt.all = readpnt.all = 0xFFFF;
    pVthJob   = &gFioJobBuf[FIO_JOB_IDX_VTH];

    // gc 2.move all valid page from gc block to the open block
    if(gTcgTempMbrCellVac[DftBlk] && (DftBlk < TCG_MBR_CELLS))
    {
        readpnt.pc.cell = DftBlk;
        for(x = SLC_OFF(L2P_PAGE_CNT); x < PAGE_NUM_PER_BLOCK; x += SLC_OFF(1))
        {
            readpnt.pc.page = x;
            if(gTcgTempMbrCellValidMap[DftBlk][x/32] & (mBIT(x%32)))
            {
                // ----- read -----
                FillPaa(0, readpnt, ZONE_GRP5);
                for(i = 0; i < PAA_NUM_PER_PAGE; i++)
                {
                    pVthJob->paa[i].rr.ch      = gTcgPaa[i].b.ch;
                    pVthJob->paa[i].rr.ce      = gTcgPaa[i].b.ce;
                    pVthJob->paa[i].rr.voltage = 0;
                }
                pVthJob->paaCnt = PAA_NUM_PER_PAGE;

                rdRtySuccess = FALSE;
                for (RdRtyCnt = 0; RdRtyCnt < MAX_VOLTAGE_STAGE; RdRtyCnt++)
                {
                    // --- Reset job desc count and error count
                    pVthJob->status  = 0;
                    pVthJob->descCnt = 0;
                    pVthJob->tid     = AplSeq_AllocTrkId(SEQ_TIMEOUT_WAIT);

                    //--- Open Page
                    MEM_SET(gOpenRow, 0xFF, sizeof(gOpenRow));
                    HLD_FIO_WaitComplete();
                    //
                    orgTcgErrTrkDoneCallback = HAL_SEQ_RegiserCallback(SEQ_CB_TRK_DONE, TCG_ERR_HANDLE_ReadRetryTrkDone);
                    HLD_FIO_ForceIssueJob(pVthJob);
                    HLD_FIO_WaitComplete();
                    HAL_SEQ_RegiserCallback(SEQ_CB_TRK_DONE, orgTcgErrTrkDoneCallback);
                    AplSeq_FreeTrkId(pVthJob->tid);
                    //
                    if(TCG_Rd1RtyPg(readpnt, ZONE_GRP5) == zOK)
                    {   // previos parameter for read retry
                        rdRtySuccess = TRUE;
                        // read retry successful
                        break;
                    }
                    else
                    {
                        for(i = 0; i < PAA_NUM_PER_PAGE; i++)
                        {   // change threhold voltage
                            pVthJob->paa[i].rr.voltage++;
                        }
                    }
                }

                if(rdRtySuccess == FALSE)
                {
                    return zNG;
                }

                // ----- write -----
                gclaa = gTcgAux[0].aux0;
                if(gclaa >= G4G5_LAA_AMOUNT_LIMIT) continue; // if illegal laa

                writepnt.all = TcgG5CurPnt.all = TcgG5NxtPnt.all;
                TcgG5NxtPnt.all += SLC_OFF(1);

                pG5->b.TcgTempMbrL2P[gclaa] = writepnt.all;
                TCG_Wr1Pg(writepnt, ZONE_GRP5, gclaa, TcgG5NxtHistNo);
            }
        }
        if(TcgG5NxtPnt.pc.page == PAGE_NUM_PER_BLOCK)
        {   // this Free block is writed complete ?
            TcgG5NxtPnt.all = TCG_LAA_INIT_VAL;
        }
    }
    else
    {
        return zNG;
    }
#endif
    return zOK;
}

//----------------------------------------
// TCG error handling
//----------------------------------------
int TCG_ErrHandling(tTcgGrpDef grp, MsgTcg_t *pTcgMsg)
{
    U16 errNativeBlkCnt = 0;
    tTcgLogAddr myRdPnt;
    U16 i;
    U16 laaCnt;

    HERE(NULL);
    laaCnt = pTcgMsg->laae - pTcgMsg->laas;

    if(TCG_ReadRetry(grp, &errNativeBlkCnt, pTcgMsg) == zNG) return zNG;

    // move defect block away if read retry ok.
    for(i = 0; i < errNativeBlkCnt; i++){
        //tag a defect in defect table
        if(grp > ZONE_DS){
            if(G5_RelpaceDefectBlock(gTcgRdRetry.RdErrNativeBlk[i]) == zOK){   // replace defect block
                tcgG5Dft[ gTcgRdRetry.RdErrNativeBlk[i] ] = 0xDF;
            }else{
                return zNG;
            }
        }else{
            if(G4_RelpaceDefectBlock(gTcgRdRetry.RdErrNativeBlk[i]) == zOK){   // replace defect block
                tcgG4Dft[ gTcgRdRetry.RdErrNativeBlk[i] ] = 0xDF;
            }else{
                return zNG;
            }
        }
    }
#ifdef TCG_EEP_NOR
    TCG_NorEepWrite();
#else
    SyIn_Synchronize(SI_AREA_MISC, SYSINFO_WRITE, SI_SYNC_BY_TCG);
#endif

    // change read PAA if replace OK. then read again.
    if(grp > ZONE_DS){
        for(i=0; i < laaCnt; i++){
            myRdPnt.all = pG5->b.TcgTempMbrL2P[pTcgMsg->laas + i].all;
            FillPaa(i, myRdPnt, ZONE_GRP5);
        }
    }else{
        for(i=0; i < laaCnt; i++){
            myRdPnt.all = pG4->b.TcgMbrL2P[pTcgMsg->laas + i].all;
            FillPaa(i, myRdPnt, ZONE_GRP4);
        }
    }

    return(TCG_ReadLaunched(laaCnt, pTcgMsg->pBuffer, pTcgMsg));
}

//----------------------------------------
// TCG read retry
//----------------------------------------
int TCG_ReadRetry(tTcgGrpDef grp, U16* errNativeBlkCnt, MsgTcg_t *pTcgMsg)
{
#ifdef _ALEX_VER_OK
    tFIO_JOB* pVthJob = NULL;
    tTcgLogAddr myRdPnt;
    U16 errPaaIdx;
    U16 errPaaCnt;
    //U16 errNativeBlkCnt;
    U16 i;
    U16 RdRtyCnt;
    U8  ErrBlkTag[TCG_MBR_CELLS];
    U16 rdRtySuccess;

    HERE(NULL);
    if (pTcgJob->status & (FIO_S_BLANK_ERR)){
        return zNG;
    }

    gTcgRdRetry.PrevPaaCnt = pTcgJob->paaCnt;  // copy paaCnt
    MEM_COPY(gTcgRdRetry.PrevPaa, gTcgPaa, pTcgJob->paaCnt);     // copy previous paa
    MEM_COPY(gTcgRdRetry.RdErrMap, &smFioInfo.errPaaMap[pTcgJob->fid][0], pTcgJob->paaCnt);    // get error paa bit map
    MEM_CLR(ErrBlkTag, sizeof(ErrBlkTag));

    pVthJob   = &gFioJobBuf[FIO_JOB_IDX_VTH];

    errPaaIdx = 0;
    errPaaCnt = 0;

    // collect every read error page.
    for(errPaaIdx = 0; errPaaIdx < gTcgRdRetry.PrevPaaCnt; errPaaIdx += PAA_NUM_PER_PAGE){
        if( (gTcgRdRetry.RdErrMap[(errPaaIdx  ) >> FIO_ERR_PAA_BMP_SHIFT] & BIT((errPaaIdx  ) & FIO_ERR_PAA_BMP_MASK)) |
            (gTcgRdRetry.RdErrMap[(errPaaIdx+1) >> FIO_ERR_PAA_BMP_SHIFT] & BIT((errPaaIdx+1) & FIO_ERR_PAA_BMP_MASK)) |
            (gTcgRdRetry.RdErrMap[(errPaaIdx+2) >> FIO_ERR_PAA_BMP_SHIFT] & BIT((errPaaIdx+2) & FIO_ERR_PAA_BMP_MASK)) |
            (gTcgRdRetry.RdErrMap[(errPaaIdx+3) >> FIO_ERR_PAA_BMP_SHIFT] & BIT((errPaaIdx+3) & FIO_ERR_PAA_BMP_MASK)) )
        {
            gTcgRdRetry.RdErrNativePaa[errPaaCnt / PAA_NUM_PER_PAGE] = gNativePaa[errPaaIdx / PAA_NUM_PER_PAGE].all;  //get native error block
            ErrBlkTag[gNativePaa[errPaaIdx / PAA_NUM_PER_PAGE].pc.cell] = 0xDF;  // tag defet block

            for(i = 0; i < PAA_NUM_PER_PAGE; i++){
                pVthJob->paa[errPaaCnt + i].rr.ch      = gTcgRdRetry.PrevPaa[errPaaIdx + i].b.ch;
                pVthJob->paa[errPaaCnt + i].rr.ce      = gTcgRdRetry.PrevPaa[errPaaIdx + i].b.ce;
                pVthJob->paa[errPaaCnt + i].rr.voltage = 0;

                //gTcgPaa[i].all32 = gTcgRdRetry.TcgPrevPaa[errPaaIdx + i].all;  //prepare Paa
            }
            errPaaCnt += PAA_NUM_PER_PAGE;
        }
    }

    //extract rd err native block and save it to array RdErrNativeBlk[]
    *errNativeBlkCnt = 0;
    for(i = 0; i < sizeof(ErrBlkTag); i++){
        if(ErrBlkTag[i] == 0xDF){
            gTcgRdRetry.RdErrNativeBlk[*errNativeBlkCnt] = i;
            (*errNativeBlkCnt)++;
        }
    }

    pVthJob->paaCnt = errPaaCnt;
    // start read retry
    rdRtySuccess = FALSE;
    for (RdRtyCnt = 0; RdRtyCnt < MAX_VOLTAGE_STAGE; RdRtyCnt++)
    {
        // --- Reset job desc count and error count
        pVthJob->status  = 0;
        pVthJob->descCnt = 0;
        pVthJob->tid    = AplSeq_AllocTrkId(SEQ_TIMEOUT_WAIT);

        //--- Open Page
        MEM_SET(gOpenRow, 0xFF, sizeof(gOpenRow));
        HLD_FIO_WaitComplete();
        //------------------------------------------
        orgTcgErrTrkDoneCallback  = HAL_SEQ_RegiserCallback(SEQ_CB_TRK_DONE,  TCG_ERR_HANDLE_ReadRetryTrkDone);
        HLD_FIO_ForceIssueJob(pVthJob);
        HLD_FIO_WaitComplete();
        HAL_SEQ_RegiserCallback(SEQ_CB_TRK_DONE, orgTcgErrTrkDoneCallback);
        AplSeq_FreeTrkId(pVthJob->tid);
        //
        if(TCG_ReadLaunched(gTcgRdRetry.PrevPaaCnt/PAA_NUM_PER_PAGE, pTcgJob->pBuffer, pTcgMsg) == zOK){   // previos parameter for read retry
            rdRtySuccess = TRUE;  // read retry successful
            break;
        }else{
            for(i = 0; i < errPaaCnt; i++){     // change threhold voltage
                pVthJob->paa[i].rr.voltage++;
            }
        }
    }

    if(rdRtySuccess == FALSE){
        TCG_PRINTF("Read Retry Fail, Read copy at another group!!\n");
        return zNG;
    }
    TCG_PRINTF(">>Rd Retry OK\n");
#endif
    return zOK;
}

//===================================================
//  U16 Tcg_G4FreeBlk(U16 *Vac)
//===================================================
U16 Tcg_G4FreeBlk()
{
    U16 i;
    U16 FreeBlkCnt = 0;
    for(i = 0; i < TCG_MBR_CELLS; i++){
        if((!gTcgMbrCellVac[i]) && (!tcgG4Dft[i])){
            FreeBlkCnt++;
        }
    }
    return FreeBlkCnt;
}

//===================================================
//  U16 Tcg_G5FreeBlk(U16 *Vac)
//===================================================
U16 Tcg_G5FreeBlk()
{
    U16 i;
    U16 FreeBlkCnt = 0;
    for(i = 0; i < TCG_MBR_CELLS; i++){
        if((!gTcgTempMbrCellVac[i]) && (!tcgG5Dft[i])){
            FreeBlkCnt++;
        }
    }
    return FreeBlkCnt;
}

//===================================================
// FillFragInPage()
//===================================================
void FillFragInPage(tPAA *pPaa)
{
    pPaa->b.frag = 0;
    pPaa++;
    pPaa->b.frag = 1;
    pPaa++;
    pPaa->b.frag = 2;
    pPaa++;
    pPaa->b.frag = 3;
}

//===================================================
//  FillPaa()
//===================================================
void FillPaa(U8 ipage, tTcgLogAddr writepnt, tTcgGrpDef grp)
{
    //gTcgPaa[ipage*PAA_NUM_PER_PAGE+0].all = CnvAdrLog2Phy(writepnt, grp);
    CnvAdrLog2Phy(writepnt, grp, gTcgPaa+(ipage*PAA_NUM_PER_PAGE+0));
    COPY2NEXT4(gTcgPaa, ipage*PAA_NUM_PER_PAGE+0);
    FillFragInPage(&gTcgPaa[ipage*PAA_NUM_PER_PAGE+0]);

#if 0
    TCG_PRINTF("LogAdr[%2X]=%4X , GRP=%2X, PAA :ch[%2x] ce[%2x] lun[%2x] blk[%4x] pln[%2x] frg[%2x] pg[%4x]\n", \
        ipage, writepnt.all, grp,
        gTcgPaa[ipage*PAA_NUM_PER_PAGE+0].b.ch, gTcgPaa[ipage*PAA_NUM_PER_PAGE+0].b.ce, \
        gTcgPaa[ipage*PAA_NUM_PER_PAGE+0].b.lun, gTcgPaa[ipage*PAA_NUM_PER_PAGE+0].b.block, \
        gTcgPaa[ipage*PAA_NUM_PER_PAGE+0].b.plane, gTcgPaa[ipage*PAA_NUM_PER_PAGE+0].b.frag, \
        gTcgPaa[ipage*PAA_NUM_PER_PAGE+0].b.page);
#endif
}

//===================================================
//  FillAux()
//===================================================
void FillAux(U8 ipage, U32 aux0, U32 aux1)
{
    U16 blk;
    U32 die, idx;

    gTcgAux[ipage*PAA_NUM_PER_PAGE+0].aux0 = aux0;
    gTcgAux[ipage*PAA_NUM_PER_PAGE+0].aux1 = aux1;
    COPY2NEXT4(gTcgAux, ipage*PAA_NUM_PER_PAGE+0);
}

//===================================================
//  CnvAdrLog2Phy()
//===================================================
void CnvAdrLog2Phy(tTcgLogAddr logadr, tTcgGrpDef grp, tPAA *mypaa)
{
    U16 blk;
    U32 die, idx;
    // blk- lun-ch-ce-page (total use 1 super block, G4 = plane0 , G5 = plane2)
    mypaa->all32 = 0;     // clear  TcgAddr
    mypaa->b.page = logadr.pc.page;
    mypaa->b.ce = BITS_CAP(logadr.pc.cell, gNfInfo.bitsDevice);
    mypaa->b.ch = BITS_CAP(VAR_SFT_R_BITS(logadr.pc.cell, gNfInfo.bitsDevice), gNfInfo.bitsCh);
    mypaa->b.lun = BITS_CAP(VAR_SFT_R_BITS(logadr.pc.cell, gNfInfo.bitsDevice+gNfInfo.bitsCh), gNfInfo.bitsLun);
    mypaa->b.plane = BITS_CAP(VAR_SFT_R_BITS(logadr.pc.cell, gNfInfo.bitsDevice+gNfInfo.bitsCh+gNfInfo.bitsLun), gNfInfo.bitsPlane);
    //mypaa->b.plane += (grp > ZONE_DS) ? 2 : 0;          // G5 or G4

    if(BLKS_NUM_PER_SUBLK >= TCG_MBR_CELLS)
    {
        if(grp > ZONE_DS){
            mypaa->b.block = smSysInfo->d.TcgData.d.TCGBlockNo[1];
        }else{
            mypaa->b.block = smSysInfo->d.TcgData.d.TCGBlockNo[0];
        }
    }
    else
    {
        if(grp > ZONE_DS){
            if(logadr.pc.cell < BLKS_NUM_PER_SUBLK){
                mypaa->b.block = smSysInfo->d.TcgData.d.TCGBlockNo[2];
            }else{
                mypaa->b.block = smSysInfo->d.TcgData.d.TCGBlockNo[3];
            }
        }else{
            if(logadr.pc.cell < BLKS_NUM_PER_SUBLK){
                mypaa->b.block = smSysInfo->d.TcgData.d.TCGBlockNo[0];
            }else{
                mypaa->b.block = smSysInfo->d.TcgData.d.TCGBlockNo[1];
            }
        }
    }

    //Is replacement ? if so, find replaced block
    die = FTL_GET_DIE_NUM(mypaa->b.lun, mypaa->b.ce, mypaa->b.ch);
    idx = FTL_GET_SBLK_IDX(mypaa->b.plane, mypaa->b.block, die);
    blk = gFtlRemap.rmpSuperBlkTbl[idx];//get logical block
    mypaa->b.block = (blk == WORD_MASK) ? mypaa->b.block : blk;

    //return mypaa;
}

//-------------------------------------------------------------------
// Function     : static void ERR_HANDLE_ReadRetryTrkDone(U32 trkId, PVOID pStatus)
// Description  : Error handler trake done
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void TCG_ERR_HANDLE_ReadRetryTrkDone(U32 trkId, PVOID pStatus)
{
#ifdef _ALEX_VER_OK
    tFIO_JOB* pJob = smFioInfo.tid2Job[trkId];

    //pJob->status |= FIO_S_COMPLETE;

    smFioInfo.jobDoneCnt++;

    DESC_FIFO_UPDATE_R_PTR(pJob->descCnt);
#endif
}

//-------------------------------------------------------------------
// Function     : static void AGING_JobErrorHandle(tFIO_JOB* pJob)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void TCG_JobErrorHandle(tFIO_JOB* pJob)
{
#ifdef _ALEX_VER_OK
    tPAA errPaa;
    U16  cnt;

    for (cnt = 0; cnt < (pJob->paaCnt); cnt++)
    {
        if (FIO_CHK_ERR_BMP(pJob->fid, cnt))
        {
            errPaa = pJob->paa[cnt];
            AGING_ErrBlk_Record(errPaa, pJob->cmd, pJob->status);
            FIO_CLR_ERR_BMP(pJob->fid, cnt);
        }
    }
#endif
}

//-------------------------------------------------------------------
// Function     : static void TCG_JobCompleteHandle(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void TCG_JobCompleteHandle(void)
{
    tFIO_JOB* pJob = NULL;

    if (IPC_MsgQFastPeek(cM2cComQueue))
    {
        pJob   = (tFIO_JOB*) IPC_GetMsgQ(cM2cComQueue);

        #if (FIO_JOB_HEAP_MEMORY == TRUE)
        Heap_Free((PVOID)pJob->paa);
        pJob->paa = &smFioInfo.pDramPaa[pJob->fid * MAX_PAA_PER_JOB];


        if (pJob->flag & FIO_F_AUX)
        {
            if (pJob->cmd < FIO_JOB_CMD_WRITE)
            {
                MEM_COPY(&smFioInfo.pDramAux[pJob->fid * MAX_AUX_PER_JOB], HEAP_GET_PTR_CPU1_B0TCM(pJob->aux), sizeof(tPAA) * pJob->paaCnt * 2);
            }

            pJob->aux = &smFioInfo.pDramAux[pJob->fid * MAX_AUX_PER_JOB];
        }
        #endif

        TCG_JobErrorHandle(pJob);

        gCbfCacheTrkDone[pJob->css](pJob);
        FIO_PushJob(FIO_JOB_POOL_FREE, pJob);
        smFioInfo.jobDoneCnt++;
    }
}

//-------------------------------------------------------------------
// Function     : static void TCG_JobWaitComplete(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void TCG_JobWaitComplete(void)
{
    U32   timeout = 0;

    while (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] != FIO_MAX_JOB)
    {
        TCG_JobCompleteHandle();

        timeout++;
        ASSERT(timeout < 0xF0000000);
    }
}

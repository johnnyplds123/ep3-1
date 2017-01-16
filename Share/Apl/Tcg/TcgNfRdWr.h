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
#define TCG_CFG_RSV_BLK_PER_LUN     (((TCG_CFG_BLK_CNT_PER_GRP >> gNfInfo.bitsLun) >> gNfInfo.bitsCh) >> gNfInfo.bitsDevice)
#define TCG_CFG_START_LUN           gNfInfo.blocksPerLun - TCG_CFG_RSV_BLK_PER_LUN

#define MAX_DEVICES_NUM_PER_ROW     (8 * 4)   // 8 ch, 4 ce
#define DEVICES_NUM_PER_ROW         (gNfInfo.numChannels * gNfInfo.devicesPerChannel)
#define BLKS_NUM_PER_SUBLK          (gNfInfo.planesPerLun * gNfInfo.lunsPerDevice * DEVICES_NUM_PER_ROW)

#define PAA_NUM_PER_PAGE            4
#define PAA_NUM_PER_ROW             (DEVICES_NUM_PER_ROW * PAA_NUM_PER_PAGE)     // DEVICES_NUM_PER_ROW * 4

#define MAX_PAGE_PER_TRK            ((MAX_DESC_PER_TRK / PAA_NUM_PER_PAGE) > 32 ? 32 : (MAX_DESC_PER_TRK / PAA_NUM_PER_PAGE))


#define TCG_G4_TAG                  0x00434734
#define TCG_G5_TAG                  0x00434735


#define PAGE_NUM_PER_BLOCK          gNfInfo.pagesPerBlock

#define TCG_GC_THRESHOLD            3

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define VAR_SFT_R_BITS(var, shtbits)    (var >> shtbits)
#define BITS_MASK(b)            (~(U32_MAX << b))
#define BITS_CAP(a, b)          (a & BITS_MASK(b))
#define BITS2VAR(a, b, c)       (a = BITS_CAP(b, c))

#if   _NAND_ == NAND_3DTLCG2
    #define TCG_NAND_SLC_PAGECNT_STEP       3
#elif _NAND_ == NAND_MLC
    #define TCG_NAND_SLC_PAGECNT_STEP       2
#endif

#define SLC_OFF(a)              (a * TCG_NAND_SLC_PAGECNT_STEP)    // SLC offset

#define COPY2NEXT4(a, b)        a[b+3] = a[b+2] = a[b+1] = a[b+0]

#ifdef _ALEXDEBUG_PAA
#define dumpPaa(pagecnts)    { \
                                int i,j; \
                                for(i=0; i <pagecnts * PAA_NUM_PER_PAGE; i+=PAA_NUM_PER_PAGE){ \
                                    for(j=0; j<PAA_NUM_PER_PAGE; j++){ \
                                        TCG_PRINTF("PAA :ch[%2x] ce[%2x] lun[%2x] blk[%4x] pln[%2x] frg[%2x] pg[%4x]\n", gTcgPaa[i+j].b.ch, gTcgPaa[i+j].b.ce, gTcgPaa[i+j].b.lun, gTcgPaa[i+j].b.block, gTcgPaa[i+j].b.plane, gTcgPaa[i+j].b.frag, gTcgPaa[i+j].b.page);\
                                    } \
                                } \
                            }
#else
#define dumpPaa(pagecnts)    {}
#endif


//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @TcgAddr info
typedef struct
{
    U32 aux0;
    U32 aux1;
}tTcgAux;

typedef struct
{
    U16     PrevPaaCnt;   // Previous Paa Cnt
    tPAA    PrevPaa[MAX_DEVICES_NUM_PER_ROW  /* MAX_PAGE_PER_TRK */ * PAA_NUM_PER_PAGE];  // Previous Paa, 4 paa per page
    U32     RdErrMap[MAX_DEVICES_NUM_PER_ROW  /* MAX_PAGE_PER_TRK */ * PAA_NUM_PER_PAGE / 32];  // read error bits mapping
    U16     RdErrNativePaa[MAX_DEVICES_NUM_PER_ROW  /* MAX_PAGE_PER_TRK */];
    U8      RdErrNativeBlk[MAX_DEVICES_NUM_PER_ROW  /* MAX_PAGE_PER_TRK */];
}tTcgRdRetry;

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
extern tTcgAux      gTcgAux[MAX_DEVICES_NUM_PER_ROW  /* MAX_PAGE_PER_TRK */ * PAA_NUM_PER_PAGE];
extern U8           OnePgSzBuf[0x4000];
extern tTcgLogAddr  WrPntBox[MAX_DEVICES_NUM_PER_ROW  /* MAX_PAGE_PER_TRK */];
extern tTcgLogAddr  TcgG4Pnt, TcgG5Pnt;
extern tTcgLogAddr  TcgG4CurPnt, TcgG4NxtPnt;
extern tTcgLogAddr  TcgG5CurPnt, TcgG5NxtPnt;
extern U32          TcgG4CurHistNo, TcgG4NxtHistNo;
extern U32          TcgG5CurHistNo, TcgG5NxtHistNo;
extern U8           TcgG4Defects, TcgG5Defects;
extern U8           ValidBlks[32];
extern U16          ValidCnt;
extern U16          G4_FreeBlk;
extern U16          G5_FreeBlk;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
int TCG_ReadLaunched(U16 pagecnts, U8 *pBuffer, MsgTcg_t *pTcgMsg);
int TCG_WriteLaunched(U16 pagecnts, U8 *pBuffer);
int TCG_EraseLaunched(U16 blkcnts);
int TCG_G4Wr(MsgTcg_t *pTcgMsg);
int TCG_G4WrL2p(void);
int TCG_G5Wr(MsgTcg_t *pTcgMsg);
int TCG_G5WrL2p(void);

//-----------------------------------------------------------------------------
//  Public function prototype definitions:
//-----------------------------------------------------------------------------
int  TCG_G4Rd(MsgTcg_t *pTcgMsg);
int  TCG_G5Rd(MsgTcg_t *pTcgMsg);
int  TCG_Rd1Pg(tTcgLogAddr readpnt, tTcgGrpDef grp);
int  TCG_Wr1Pg(tTcgLogAddr writepnt, tTcgGrpDef grp, U32 Aux0, U32 Aux1);
int  TCG_G4RdL2p(void);
int  TCG_G5RdL2p(void);
int  TCG_RdMulChan(tTcgGrpDef grp, tTcgLogAddr ReadPnt, U32 SkipChanMap);
int  TCG_WrMulChan(tTcgGrpDef grp, tTcgLogAddr WritePnt, U32 SkipChanMap);
int  TCG_ErMulChan(tTcgGrpDef grp, tTcgLogAddr ErasePnt, U32 SkipChanMap);
void TcgMbrBuildGcTables(void);
void TcgTempMbrBuildGcTables(void);
int  G4_GC(void);
int  G5_GC(void);
void TCG_NorEepInit(void);
void TCG_NorEepRead(void);
void TCG_NorEepWrite(void);
void TCG_JobErrorHandle(tFIO_JOB* pJob);
void TCG_JobCompleteHandle(void);
void TCG_JobWaitComplete(void);


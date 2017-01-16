//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#pragma once
#include "Ftl.h"

#if (_NAND_ == NAND_MLC)
    #include "Gc_MLC.h"
#else
    #include "Gc_3DTLCG2.h"
#endif

// -----------------------------------------------------------------------------
//  Constant definitions:
// -----------------------------------------------------------------------------
#if (CHK_FW_TYPE(FW_AGING) == FALSE)
  #define ENABLE_GC               (TRUE)
#else
  #define ENABLE_GC               (FALSE)
#endif

#if (ENABLE_GC == TRUE)
    #define DBG_GC                (TRUE)
    #define DBG_BLKLOG            (TRUE)
    #define DBG_GC_LP             (FALSE)
    #define DBG_GC_AUX            (FALSE)  // TBD
    #define DBG_GC_FUNCTION       (FALSE)
    #define GC_REMAP_MODE         (FALSE)  // Wait DQ04
    #define GC_WORKAROUND         (FALSE)  // Platform compability (host write + flush after a POR)
    #define GC_HOST_DATA_SPLIT    (FALSE)  // TBD
#else
    #define DBG_GC                (FALSE)
    #define DBG_BLKLOG            (FALSE)
    #define DBG_GC_LP             (FALSE)
    #define DBG_GC_AUX            (FALSE)
    #define DBG_GC_FUNCTION       (FALSE)
    #define GC_REMAP_MODE         (FALSE)
    #define GC_WORKAROUND         (FALSE)  // Platform compability (host write + flush after a POR)
    #define GC_HOST_DATA_SPLIT    (FALSE)  // TBD
#endif

#if (DBG_GC == TRUE)
  #define D_PRINTF_GC(F, args...)             D_PRINTF("[GC  ] "F, ##args)
#else
  #define D_PRINTF_GC(F, args...)             {}
#endif

// ---- time measurement ---------
//#define GC_DBG_PROC_TIME_CHECK
//#define GCDBG_PROC_TIME_CHECK
//#define GCDBG_TRK_DONE_CACHECNT

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#if (FTL_FUSH_SIZE == (4 * KBYTE))
    #define GC_XFER_ORDER                           (1)
    #define GC_XFER_ORDER_OFFSET                    (4)
#elif (FTL_FUSH_SIZE == (8 * KBYTE))
    #define GC_XFER_ORDER                           (2)
    #define GC_XFER_ORDER_OFFSET                    (4)
#else  // 16 KB size
    #define GC_XFER_ORDER                           (4)
    #define GC_XFER_ORDER_OFFSET                    (1)
#endif

#if (GC_REMAP_MODE == TRUE)
    #define GC_REMAP_SIZE                           (6)
#endif
/*------------- GC THRESHOLD VAR  ---------------*/

//Blk#    x       x+1        x+3       x+5    x+6
// -------|-------|----------|---------|------|--------------------------------
//        TX--    TX--                 TX++   TX++
//        LockGC             GC On            GC Off
#define GC_BLKCNT_TH                                (5)                                        // For GC "on & off & full" threshold
#define GC_BLKCNT_READONLY                          (1)
#define GC_BLKCNT_DISKFULL                          (FTL_GROUP_NUM + FTL_QBT_BLOCK_COUNT + 3)  // 3 extra: errorhandle/GC boot + 1 for GC header
#define GC_BLKCNT_EMER                              (GC_BLKCNT_DISKFULL + 2)
#define GC_BLKCNT_ACTIVE                            (GC_BLKCNT_DISKFULL + GC_BLKCNT_TH)
#define GC_BLKCNT_DEACTIVE                          (GC_BLKCNT_ACTIVE   + GC_BLKCNT_TH)

#define GC_TOP_RATIO                                (90)
#define GC_LOW_RATIO                                (7)
#define GC_MAX_EMGR_RATIO                           (50)  // Minmum 2% OP
#define GC_EMGR_RATIO_WEIGHT                        (3)   // Add 3% weight

/* ------------- GC ECU Info -----------------*/
#define GC_ECU_AU_PER_PAGE                          (gEcuInfo.auPerPage)
#define GC_ECU_AU_PER_HALF_PAGE                     (GC_ECU_AU_PER_PAGE >> 1)

/*------------- GC NAND IF Info -----------------*/
#define GC_NF_USER_CAPACITY                         (gNfInfo.userCapacity)
#define GC_NF_RAW_NAND_CAP                          (gNfInfo.rawNANDCapacity)
#define GC_NF_DATA_BYTE_PER_PAGE                    (gNfInfo.dataBytesPerPage)
#define GC_NF_PAGE_PER_BLK                          (gNfInfo.pagesPerBlock)
#define GC_NF_BLK_PER_PLANE                         (gNfInfo.blocksPerPlane)
#define GC_NF_PLANE_PER_LUN                         (gNfInfo.planesPerLun)
#define GC_NF_LUN_PER_DEVICE                        (gNfInfo.lunsPerDevice)
#define GC_NF_DEVICE_PER_CHANNEL                    (gNfInfo.devicesPerChannel)
#define GC_NF_LUN_PER_CH                            (gNfInfo.lunsPerChannel)
#define GC_NF_NUM_CHANNELS                          (gNfInfo.numChannels)
#define GC_NF_TOT_LUN                               (gNfInfo.totalLun)
#define GC_NF_PAGE_PER_PROG_UNIT                    (gNfInfo.pagesPerProgUnit)
#define GC_NF_WL_PER_BLK                            (gNfInfo.wlPerBlock)
#define GC_NF_USER_WL_PER_BLK                       (GC_NF_WL_PER_BLK - 1)
#define GC_NF_USER_PAGE_PER_BLK                     (GC_NF_PAGE_PER_BLK - GC_NF_PAGE_PER_PROG_UNIT)  // Last WL page is LP no need to scan

#define GC_NF_SHIFT_CH                              (gNfInfo.bitsCh)
#define GC_NF_SHIFT_CE                              (gNfInfo.bitsDevice)
#define GC_NF_SHIFT_LN                              (gNfInfo.bitsLun)
#define GC_NF_SHIFT_PL                              (gNfInfo.bitsPlane)
#define GC_NF_SHIFT_BK                              (gNfInfo.bitsBlocksPerLun)
#define GC_NF_SHIFT_PG                              (gNfInfo.bitsPagesPerBlock)

#define GC_FTL_SBK_VALID_CNT                        (FTL_MAX_VAC)
#define GC_FTL_LP_SIZE                              (gFtlLastPageP2LSize)
#define GC_SCAN_PAA_CNT_TH                          (GC_ONE_SHOT_PAA_CNT << 2)
#define GC_MAX_AUX_JOB                              (1)//(GC_NF_DEVICE_PER_CHANNEL) // TBD

/*------------- GC CACHE CHK VAR  ---------------*/
#define GC_REQ_CACHE_ENTRY                          (CACHE_FLUSH_ONE_CE_CNT)

#if (GC_HOST_DATA_SPLIT == TRUE)      // Multi header setup
  #define GC_FTL_HEADER                             (FTL_BLOCK_HEADER_GC)
#else
  #define GC_FTL_HEADER                             (FTL_BLOCK_HEADER_HOST)
#endif

/*------------- GC PING PONG Var ---------------*/
#define GC_PING_PONG_NUM                            (2)
#define GC_PING_PONG_RECYC_CNT                      (1)   // How many Ping Pong Buffers are able to recycle at the same time

/*------------- GC Pick BLK type ---------------*/
#define PICK_LOW_VC                                 (0)
#define PICK_LOW_EC                                 (1)

/*------------- GC WL             ---------------*/
#define GC_WL_ENABLE_TH                             (90)
#define GC_WL_REQ_BK_CNT                            ((GC_PING_PONG_NUM << 1) + 1) // Min request user blk
#define GC_EC_AVG_CNT                               (gFtlMgr.AllEraseCnt / gGCInfo.op.totBlkCnt)

// For Last page
#define GC_LP_PAGE_IDX                              (FTL_NAND_FTL_P2L_PAGE)
#define GET_CE_FROM_IDX(IDX)                        (IDX % GC_NF_DEVICE_PER_CHANNEL)
#define GET_LN_FROM_IDX(IDX)                        (IDX / GC_NF_DEVICE_PER_CHANNEL)

// For remap
#define GET_RMP_IDX(PL, CE, LUN)                    ((LUN << (GC_NF_SHIFT_CE + GC_NF_SHIFT_PL)) | (CE << GC_NF_SHIFT_PL) | PL)

// -----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define GC_COUNT_HOST_WRITE(P,I)                    {gGCppBuf[P].qos_Info.hostCacheCnt += I;}
#define GC_DECRE_HOST_WRITE(P,I)                    {gGCppBuf[P].qos_Info.hostCacheCnt -= I;}
#define GC_RECYC_BLK_OPRATIO(VacCnt)                (((GC_FTL_SBK_VALID_CNT - VacCnt) * 100) / GC_FTL_SBK_VALID_CNT)
#define GC_BLK_OPRATIO_TO_CACHE(P)                  ((CACHE_RW_COUNT *(P)) / 100)

#define GC_TOP_CACHE_CNT                            (GC_BLK_OPRATIO_TO_CACHE(GC_TOP_RATIO))             // NEED TO TEST(1843)
#define GC_LOW_CACHE_CNT                            (GC_BLK_OPRATIO_TO_CACHE(gGCInfo.op.opRatio))       // NEED TO TEST(286)

#define GC_STATE_SETTING(STATE)                     (gGCInfo.state.all16  =  (STATE))
#define GC_STATE_CLEAN(STATE)                       (gGCInfo.state.all16 &= ~(STATE))
#define GC_STATE_RESET_ALL()                        (gGCInfo.state.all16 &= ~(GC_STATE_MASK))
#define GC_STATE_CHECK                              (gGCInfo.state.all16)

#define GC_MODE_SETTING(MODE)                       (gGCInfo.mode.all16 |=  (MODE))
#define GC_MODE_CLEAN(MODE)                         (gGCInfo.mode.all16 &= ~(MODE))
#define GC_MODE_CHECK                               (gGCInfo.mode.all16)
#define GC_MODE_CHECK_ACTIVE                        (gGCInfo.mode.all16 && !(gGCInfo.state.b.idle))
#define GC_MODE_CHECK_BOOT_ACTIVE                   (gGCInfo.mode.all16 & (GC_EMGR|GC_LOCK|GC_SHUTTLE|GC_BOOT))
#define GC_MODE_CHECK_BOOT_BLK(P)                   (gGCInfo.mode.b.bootGC && (gGCppBuf[P].mode_Info.mode & GC_ST_SHUTTLE))

#define GC_LOCK              (B_0)
#define GC_FORCE             (B_1)
#define GC_EMGR              (B_2)
#define GC_SHUTTLE           (B_3)
#define GC_BOOT              (B_4)
#define GC_IDLE              (B_5)
#define GC_STATIC_WL         (B_6)

#define GC_STATE_IDLE        (B_0)
#define GC_STATE_HALT        (B_1)
#define GC_STATE_RESET       (B_2)
#define GC_STATE_STOPATPOR   (B_3)
#define GC_STATE_READONLY    (B_4)
#define GC_STATE_MASK        (GC_STATE_IDLE|GC_STATE_HALT|GC_STATE_RESET|GC_STATE_STOPATPOR)  // CAN NOT RESET READONLY FLAG
#define GC_STATE_STOP_MASK   (GC_STATE_HALT|GC_STATE_READONLY)

//----------------------------------------------------------
// Make sure define & union structure are bit matched
typedef __packed union
{
    U16            all16;
   __packed struct{
        U16 lockGC     : 1;  // BIT0 mode
        U16 forceGC    : 1;  // BIT1 mode
        U16 emgrGC     : 1;  // BIT2 mode
        U16 shuttleGC  : 1;  // BIT3 mode
        U16 bootGC     : 1;  // BIT4 mode
        U16 idleGC     : 1;  // BIT5 mode
        U16 staticWL   : 1;  // BIT6 mode
        U16 reserve    : 9;  // Reserved
    }b;
}tMODE;
COMPILE_ASSERT(sizeof(tMODE)==2, "size must be 2 bytes");

typedef __packed union
{
    U16              all16;
   __packed struct{
        U16 idle         : 1;  // BIT0   // acvtied : 0 / 1(idle)
        U16 halt         : 1;  // BIT1
        U16 reset        : 1;  // BIT2
        U16 stopAtPor    : 1;  // BIT3
        U16 readOnly     : 1;  // BIT4
        U16 reserved     : 11; // BIT5-15
    }b;
}tSTATE;
COMPILE_ASSERT(sizeof(tSTATE)==2, "size must be 2 bytes");
//----------------------------------------------------------

typedef enum
{
    GC_LP = 0,
    GC_AUX,
    GC_NULL = 0x7FFFFFFF,
}tGC_TYPE;
COMPILE_ASSERT(sizeof(tGC_TYPE)==4, "size must be 4 bytes");

// ------ GC mode enum
typedef enum
{
    GC_ST_NON = 0,      // 0
    GC_ST_FORCE,        // 1
    GC_ST_EMGR,         // 2
    GC_ST_STATIC_WL,    // 3
    GC_ST_SHUTTLE,      // 4
    GC_ST_IDLE,         // 5
    GC_ST_LOCK,         //1 // 6
    GC_ST_AUX,          // 7
    GC_ST_CNT,          // 8
    GC_ST_NULL = 0x7FFFFFFF,
}tGC_MODE;
COMPILE_ASSERT(sizeof(tGC_MODE)==4, "size must be 4 bytes");

// ------ Ping Pong State enum
typedef enum
{
    GC_PP_ST_IDLE = 0,      // 0
    GC_PP_ST_BLK_SELECT,    // 1
    GC_PP_ST_P2L_READ,      // 2
    GC_PP_ST_P2L_WAIT,      // 3
    GC_PP_ST_P2L_DONE,      // 4
    GC_PP_ST_RECYCLE,       // 5
    GC_PP_ST_RECYCLE_DONE,  // 6
    GC_PP_ST_HALT,          // 7
    GC_PP_ST_RESET,         // 8
    GC_PP_ST_CNT,           // 9
    GC_PP_ST_NULL = 0x7FFFFFFF,
}tGC_PP_STATUS;
COMPILE_ASSERT((sizeof(tGC_PP_STATUS) & 0x03)==0, "size must be align 4 bytes");

// ------ FSM Info
typedef struct
{
    U16 curState;           // Curr PP state  2 B
    U16 nxtState;           // Next PP state  2 B
}tGC_PP_INFO_FSM;           // 4 B
COMPILE_ASSERT((sizeof(tGC_PP_INFO_FSM) & 0x3)==0, "size must be align 4 bytes");

// ------ BLK INFO
typedef struct
{
    U32 validCnt;       // 4 B
    U16 blk;            // 2 B
    U16 opRatio;        // 2 B
    U8  rmpBlk[MAX_SUPPORTED_PLANES * MAX_SUPPORTED_DEVICES * MAX_SUPPORTED_LUNS];  // 32 B
}tGC_PP_INFO_BLK;       // 40 B
COMPILE_ASSERT((sizeof(tGC_PP_INFO_BLK) & 0x3)==0, "size must be align 4 bytes");

// ------ Last Page INFO
typedef struct
{
    U8 doneCnt;
    U8 errorCnt;
    U8 issueCnt;
    U8 reserve;
}tGC_PP_INFO_LP;   // 4 B
COMPILE_ASSERT((sizeof(tGC_PP_INFO_LP) & 0x3)==0, "size must be align 4 bytes");

// ------ QoS INFO
typedef struct
{
    U32 hostCacheTH;
    U32 hostCacheCnt;
    U32 gcCacheTH;
    U32 gcCacheCnt;
}tGC_PP_INFO_QOS;  // 16 B
COMPILE_ASSERT((sizeof(tGC_PP_INFO_QOS) & 0x3)== 0, "size must be align 4 bytes");

// ------ RECYCLE INFO
typedef struct
{
    volatile tPAA curPaa;  // Current PAA where GC is recycling (SET DWMASK to ensure no more GC rd looping)
    volatile U32  recycPaaCnt;
}tGC_PP_INFO_RECYC; // 8 B
COMPILE_ASSERT((sizeof(tGC_PP_INFO_RECYC) & 0x03)==0, "size must be align 4 bytes");

// ------ Mode INFO
typedef struct
{
    tGC_MODE          mode;         // force, idle, WL...etc.
    tGC_TYPE          type;         // AUX / LP
}tGC_PP_INFO_MODE;
COMPILE_ASSERT((sizeof(tGC_PP_INFO_MODE) & 0x03)==0, "size must be align 4 bytes");

// ------ PingPong Total INFO
typedef  struct
{
    tGC_PP_INFO_MODE  mode_Info;    //  8   B
    tGC_PP_INFO_FSM   fsm_Info;     //  4   B
    tGC_PP_INFO_BLK   blk_Info;     //  40  B
    tGC_PP_INFO_LP    lp_Info;      //  4   B
    tGC_PP_INFO_QOS   qos_Info;     //  16  B
    tGC_PP_INFO_RECYC recyc_Info;   //  8   B
}tGC_PING_PONG_BUF_INFO;            //  84  B
COMPILE_ASSERT((sizeof(tGC_PING_PONG_BUF_INFO) & 0x03)==0, "size must be align 4 bytes");

#if (GC_REMAP_MODE == TRUE)
typedef struct
{
    U16 block;                                                                      //  2  B
    U16 rempCnt;                                                                    //  2  B
    U8  bmp[MAX_SUPPORTED_PLANES * MAX_SUPPORTED_DEVICES * MAX_SUPPORTED_LUNS];     //  32 B
}tGC_NODE;
COMPILE_ASSERT((sizeof(tGC_NODE) & 0x03)==0, "size must be align 4 bytes");

typedef struct
{
    U32             curRemapCnt;
    tGC_NODE*       array;
}tGC_REMAP_INFO;
COMPILE_ASSERT((sizeof(tGC_REMAP_INFO) & 0x03)==0, "size must be align 4 bytes");
#endif

typedef struct
{
    U16 blkcnt;              // 2 B
    U8  flag;                // 1 B
    U8  scantime_sec;        // 1 B
}tGC_BOOT;
COMPILE_ASSERT((sizeof(tGC_BOOT) & 0x03)==0, "size must be align 4 bytes");

typedef struct
{
    U32 totBlkCnt;                  // 4 B
    U32 totHBlkCnt;                 // 4 B
    U32 opBlkCnt;                   // 4 B
    U16 emgrRatio;                  // 2 B
    U16 opRatio;                    // 2 B
}tGC_OP;
COMPILE_ASSERT((sizeof(tGC_OP) & 0x03)==0, "size must be align 4 bytes");

// ------ GC general Info
typedef struct
{
    tMODE             mode;          // 2  B
    tSTATE            state;         // 2  B
    tGC_BOOT          boot;          // 4  B
    tGC_OP            op;            // 20 B
    U16               jobPaaCnt;     // 2  B
    U8                shuttleCnt;    // 1  B
    U8                curPpPtr;      // 1  B
    U8                nxtPpPtr;      // 1  B
    U8                reserve[3];    // 3  B
}tGC_GEN_INFO;                       // 36 B
COMPILE_ASSERT((sizeof(tGC_GEN_INFO) & 0x03)==0, "size must be align 4 bytes");

// ------ GC Aux Info
typedef struct
{
    tFIO_JOB*  pJobbuf[GC_MAX_AUX_JOB];
    U8         issuecnt;
    U8         donecnt;
    U8         reserved[2];
}tGC_AUX;                            // 20 B

COMPILE_ASSERT((sizeof(tGC_AUX) & 0x03)==0, "size must be align 4 bytes");
//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern tGC_GEN_INFO             gGCInfo;
extern tGC_PING_PONG_BUF_INFO   gGCppBuf[GC_PING_PONG_NUM];
extern tGC_AUX                  gGCAux;
extern U32*                     gdwGCLastPageBuf;

#if (DBG_GC_LP == TRUE)
    extern U32*                 gdwGCTestBuf;
#endif

#if (GC_REMAP_MODE == TRUE)
    extern tGC_REMAP_INFO       gGCremapInfo;
#endif

extern void (*gCbfGC_Set_TH[GC_ST_CNT])(U8 curP2lIdx);
extern void (*gCbfGC_PP_FSM[GC_PP_ST_CNT])(U8 curP2lIdx);

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
extern void GC_main(void);
extern void GC_GlobalVarInit(void);
extern void GC_OneTimeInit(InitBootMode_t initMode);
extern void GC_DumpInfo(void);
extern void GC_ModeAssert(U16 pool, U16 block);
extern void GC_ModeDeassert(void);
extern void GC_PushBlk(U16 blk, U16 pool, U16 header);
extern void GC_Test(U32 arg0, U32 arg1, U32 arg2);
extern void GC_OverProvion(void);
extern void GC_FreeBlk(U16 blk);
extern void GC_ReleaseGcd(void);
extern void GC_EarlyMoveBlk(tPAA paa);
extern void GC_Cache_XferWrite(void);

extern  U32 gc_checkLAA(tPAA phyPaa, U8 curP2lIdx, U16 phyBlk);

U32 GC_HostWrite(MsgHostIO_t* hMsg);
U32 GC_HostTrim(MsgTrim_t* hMsg);
U32 GC_HostFlush(Msg_t* hMsg);
U32 GC_HostFormat(MsgHostIO_t* hMsg);
U32 GC_HostShutDown(MsgHostIO_t* hMsg);

//----------------------------------------------------//
void GC_AllocTH_NON(U8 curP2lIdx);
void GC_AllocTH_LOCK(U8 curP2lIdx);
void GC_AllocTH_FORCE(U8 curP2lIdx);
void GC_AllocTH_EMGR(U8 curP2lIdx);
void GC_AllocTH_SHUTTLE(U8 curP2lIdx);
void GC_AllocTH_AUX(U8 curP2lIdx);
void GC_PingPong_Idle(U8 curP2lIdx);             // GC_PP_ST_IDLE
void GC_PingPong_Blk_Select(U8 curP2lIdx);       // GC_PP_ST_BLK_SELECT
void GC_PingPong_P2L_Read(U8 curP2lIdx);         // GC_PP_ST_P2L_READ
void GC_PingPong_P2L_Wait(U8 curP2lIdx);         // GC_PP_ST_P2L_WAIT
void GC_PingPong_P2L_Done(U8 curP2lIdx);         // GC_PP_ST_P2L_DONE
void GC_PingPong_Recycle(U8 curP2lIdx);          // GC_PP_ST_RECYCLE
void GC_PingPong_Recycle_Done(U8 curP2lIdx);     // GC_PP_ST_RECYCLE_DONE

#if (GC_REMAP_MODE == TRUE)
    void GC_Set_Remap_Blk(U16 blk);
    void GC_Set_Remap_Bmp(tPAA phyPaa);
    U16  GC_Replance_BadBlk(U16 blk);
    void GC_Sort_Remap_Info(U16 hitcnt);
    U32  GC_Get_Remap_Info(U16 blk);
#endif

//-------------------------------------------------------------------
// Function     : U32 GC_Get_P2L(tPAA paa, U8 curP2lIdx)
// Description  :
// Input        : tPAA paa, U8 curP2lIdx
// return       : U32
//-------------------------------------------------------------------
__inline U32 GC_Get_P2L(tPAA paa, U8 curP2lIdx)
{
    U32 idx = FTL_GET_LP_IDX(0, curP2lIdx, FTL_GET_DIE_NUM(paa.b.lun, paa.b.ce, paa.b.ch), paa.b.page, paa.b.plane, paa.b.frag);

    if ((gdwGCLastPageBuf[idx] > L2P_NUM) && (gdwGCLastPageBuf[idx] != DWORD_MASK))
    {
        D_PRINTF_GC("p2l:%x:%x\n", idx, gdwGCLastPageBuf[idx]);
        ASSERT(FALSE);
    }

    return (gdwGCLastPageBuf[idx]);
}

//-------------------------------------------------------------------
// Function     : U16 GC_Get_RmpBlk(tPAA paa)
// Description  :
// Input        : tPAA paa
// return       : U16
//-------------------------------------------------------------------
__inline U16 GC_Get_RmpBlk(tPAA paa)
{
    U32 idx = FTL_GET_SBLK_IDX(paa.b.plane, paa.b.block, FTL_GET_DIE_NUM(paa.b.lun, paa.b.ce, paa.b.ch));
    U16 blk = gFtlRemap.rmpSuperBlkTbl[idx];

    blk = (blk == WORD_MASK) ? paa.b.block : blk;

    return (blk);
}

//-------------------------------------------------------------------
// Function     : U8 GC_QoS_Host(U16 ssdCnt)
// Description  :
// Input        : U16 ssdCnt
// return       : U8
//-------------------------------------------------------------------
__inline U8 GC_QoS_Host(U16 ssdCnt)
{
    GC_STATE_RESET_ALL();

    if ((gGCppBuf[gGCInfo.curPpPtr].qos_Info.hostCacheCnt + ssdCnt) >= gGCppBuf[gGCInfo.curPpPtr].qos_Info.hostCacheTH) // HOST stops
    {
        if (gGCppBuf[gGCInfo.curPpPtr].qos_Info.gcCacheCnt >= gGCppBuf[gGCInfo.curPpPtr].qos_Info.gcCacheTH)            // GC stops
        {
            gGCppBuf[gGCInfo.curPpPtr].qos_Info.gcCacheCnt   = 0;
            gGCppBuf[gGCInfo.curPpPtr].qos_Info.hostCacheCnt = 0;
        }
        else  // HOST wait GC done
        {
            return (TRUE);
        }
    }
    return (FALSE);
}

//-------------------------------------------------------------------
// Function     : U16 gc_getDtcmRmp(tPAA paa, U8 curP2lIdx)
// Description  :
// Input        :
// return       : N/A
//-------------------------------------------------------------------
__inline U16 GC_GetDtcmRmp(tPAA paa, U8 curP2lIdx)
{
    U16 block = paa.b.block;

    if (gFtlRemap.rmpBlkBMP[paa.b.block])
    {
        if (gGCppBuf[curP2lIdx].blk_Info.rmpBlk[GET_RMP_IDX(paa.b.plane, paa.b.ce, paa.b.lun)] & (1 << paa.b.ch))
        {
            block = GC_Get_RmpBlk(paa);
        }
    }
    return (block);
}
#if 0   // TBD
//-------------------------------------------------------------------
// Function     : GC_GetAuxFid_Order(U8 fid)
// Description  : TBD
// Input        :
// return       : N/A
//-------------------------------------------------------------------
__inline U8 GC_GetAuxFid_Order(U8 fid)
{
    U8 idx;

    for (idx = 0; idx < GC_MAX_AUX_JOB; idx++)
    {
        if (fid == gGCAux.pJobbuf[idx]->fid)
        {
            return idx;
        }
    }

    D_PRINTF("Cant find correct AUX fid");
    for (idx = 0; idx < GC_MAX_AUX_JOB; idx++)
    {
        D_PRINTF(" |%d", gGCAux.pJobbuf[idx]->fid);
    }
    D_PRINTF("\n");

    ASSERT(FALSE);
    return 0;
}
#endif

#pragma once

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define AGING_LED            ENABLE
#define AGING_DBG            ENABLE
#define AGING_FINAL_LOOP_ERR DISABLE
#define AGING_CHECK_SOC_TYPE DISABLE

#if (AGING_DBG == ENABLE)
    #define D_PRINTF_AG(F, args...)     D_PRINTF("[AG  ] "F, ##args)
#else
    #define D_PRINTF_AG(F, args...)     {}
#endif

//#define Aging Signature
#define AGING_AGEN                      0x4E454741
#define AGING_SIGN                      0x4E494741
#define AGING_PASS                      0x53534150
#define AGING_GOGO                      0x4F474F47

//#define AGING Struct Signature
#define AGING_SOCT                      0x54434F53
#define AGING_RERN                      0x4E524552
#define AGING_ERIN                      0x4E495245
#define AGING_NFIF                      0x4649464E
#define AGING_NFID                      0x4449464E
#define AGING_DRAM                      0x4D415244
#define AGING_PDIF                      0x4E494450
#define AGING_SBLK                      0x4B4C4253
#define AGING_MOTP                      0x50544F4D
#define AGING_NBTI                      0x4954424E
#define AGING_LOOP                      0x504F4F4C
#define AGING_ERDM                      0x4D445245
#define AGING_FAIL                      0x4C494146
#define AGING_P1P2                      0x32503150

/// Aging Temeraure Tag
#define AGING_TC_IDLE                   0x44494354
#define AGING_TC_HIGH                   0x49484354
#define AGING_TC_ERASE                  0x53454354
#define AGING_TC_WRITE                  0x47504354
#define AGING_TC_READ                   0x45524354

#define AGING_SYSINFO_CNT               1
#define AGING_SLC_BLOCK                 1
#define ERASE_START_BLK                 1
#define AGING_START_BLK                 1
#define AGING_END_BLK                   gNfInfo.blocksPerPlane

#define AGING_DIE_SHIFT                 ( LOG2(gNfInfo.totalLun) )

#define AGING_CH_SHIFT                  ( gNfInfo.bitsCh )
#define AGING_CH_NUM                    ( 1 << AGING_CH_SHIFT )
#define AGING_CH_MASK                   ( AGING_CH_NUM - 1 )

#define AGING_CE_SHIFT                  ( gNfInfo.bitsDevice )
#define AGING_CE_NUM                    ( 1 << AGING_CE_SHIFT )
#define AGING_CE_MASK                   ( AGING_CE_NUM - 1 )

#define AGING_LUN_SHIFT                 ( gNfInfo.bitsLun )
#define AGING_LUN_NUM                   ( 1 << AGING_LUN_SHIFT )
#define AGING_LUN_MASK                  ( AGING_LUN_NUM - 1 )

#define AGING_PLANE_SHIFT               ( gNfInfo.bitsPlane )
#define AGING_PLANE_NUM                 ( 1 << AGING_PLANE_SHIFT )
#define AGING_PLANE_MASK                ( AGING_PLANE_NUM - 1 )

#define AGING_TEMPERATURE_TIME          60000
#define AGING_TEMPERATURE_LIMIT         85

#define AGING_MAX_LOOP                  300
#define AGING_MAX_RERUN_COUNT           10


#define AGING_DRAM_TEMP_LIMIT           115
#define AGING_DRAM_CAL_COUNT            25
#define AGING_DRAM_DLL_COUNT            20
#define AGING_DRAM_CAL_INVAILD_TAG      0x4C494146
#define AGING_DRAM_DLL_OVERLIMIT_TAG    0x4D4C564F // OVLM
#define AGING_DRAM_PARAMETER_VAILD      0x41505244 // DRPA
#define AGING_DRAM_PARAMETER_FINISH     0x534E4946 // FINS

#define AGING_HOST_BLK                  (((IDEMA_GBYTE2LBU(gNfInfo.userCapacity)/ 2 ) / ((gNfInfo.dataBytesPerPage / 1024) * gNfInfo.pagesPerBlock * gNfInfo.planesPerLun * gNfInfo.totalLun)) + 3)//Host Need Min blk per plane

#if _NAND_ == NAND_3DTLCG2
    #define AGING_MAX_DEFECT_SAVE       3912
    #define MAX_DEFECT_ALLOW            138 //defect blk per plane
    #define AGING_USER_BLK              1821
    #define AGING_SUP_BLK               1818
#elif _NAND_ ==  NAND_MLC
    #define AGING_MAX_DEFECT_SAVE       1956
    #define MAX_DEFECT_ALLOW            54  //defect blk per plane
    #define AGING_USER_BLK              1024
    #define AGING_SUP_BLK               1020
#endif

#define AGING_RETRY_PAA_UNIT            0x2
#define AGING_MAX_RETRY_COUNT           0x15
#define AGING_MAX_RETRY_COUNT_SLC       0x7

//AGING Type
#define ERASE                           0x5345  //ES
#define PROGRAM                         0x4750  //PG
#define READ                            0x4552  //RE
#define READ_UNC                        0x5552  //RU
#define READ_CWO                        0x4352  //RC
#define READ_BLANK                      0x4252  //RB
#define NO_CWOFAIL                      0x434E  //NC
#define UNKNOW                          0x4B55  //UK
#define PROG_WL_LEAKAGE                 0x4C50  //PL
#define PROG_WL_OPEN                    0x4F50  //PO
#define PROG_WL_SHORT                   0x5350  //PS
#define READ_WL_OPEN                    0x4F52  //RO
#define READ_WL_SHORT                   0x5352  //RS

//Aging Toggle
#define PASS                            0x50
#define FAIL                            0x46

#define mFALSE                          0
#define mTRUE                           1

//AGING SOC TYPE
#define AGING_SOC_TYPE_TAG              0x00535641
#define AGING_SOC_TYPE_TT               900

//AGING LED
#define AGING_LED_FLAG                  0x4C

#define AGING_BAD_BLOCK                 ( 0xFFFF )
#define AGING_ERR_HANDLE_MASK           (FIO_S_UNC_ERR|FIO_S_PROG_ERR|FIO_S_ERASE_ERR|FIO_S_OVER_LMT)

static const uint16_t   cNoLevel        = 0;
static const uint16_t   cALevel         = 1;
static const uint16_t   cBLevel         = 2;
static const uint16_t   cCLevel         = 3;
static const uint16_t   cDLevel         = 4;
static const uint16_t   cELevel         = 5;
static const uint16_t   cFLevel         = 6;
static const uint16_t   cGLevel         = 7;

typedef enum
{
    STATUS_ISSUE_SBLK=0,
    STATUS_ISSUE_NON_SBLK,
    STATUS_NON_ISSUE,
    STATUS_NULL=0x7FFFFFFF
}tSTATUS_AGING;

//define AGING_Error_Code
typedef enum
{
     ERR_NO_ERROR                   =   0x00,        // 0-0  PASS
     ERR_SCAN_DEFECT                =   0x11,        // 1-1  Build Defect Fail
     ERR_FLASH_ID                   =   0x12,        // 1-2  NAND FLASHID Fail
     ERR_SOC_TYPE                   =   0x14,        // 1-4  SOC TYPE ERROR
     ERR_ERASE_FAILED               =   0x21,        // 2-1  Erase related errors
     ERR_WRITE_FAILED               =   0x22,        // 2-2  Write related errors
     ERR_READ_FAILED                =   0x31,        // 3-1  Read related errors
     ERR_READ_EEPROM_FAILED         =   0x32,        // 3-2  Read related errors
     ERR_CHECKSUM_INVALID           =   0x33,        // 3-3  Read related errors
     ERR_BF2_ERROR                  =   0x34,        // 3-4  BF2 error
     ERR_FINALSTAGE_ERROR           =   0x44,        // 4-3
     ERR_UNKNOWN_ERROR              =   0x51,        // 5-1
     ERR_AGING_SETTING_ERROR        =   0x52,        // 5-2
     ERR_MEMORY_ERROR               =   0x53,        // 5-3
     ERR_INITIAL_E2_FAIL            =   0x54,        // 5-4
     ERR_DEFECT_ERROR               =   0x55,        // 5-5 (Plist too many)
     ERR_1st_PF_ECC_OK              =   0x61,        // 6-1
     ERR_2nd_PF_ECC_OK              =   0x62,        // 6-2
     ERR_MAX_RERUN                  =   0x71,        // 7-1  MAX RERUN FAIL
     ERR_DEFECT_BY_CH               =   0x81,        // 8-1 (Plist2 too many)
     ERR_DRAM_FAIL                  =   0x92,        // 9-2  DRAM Fail
     ERR_SUPBLK_FAIL                =   0x93,        // 9-3  Supper Blk Fail
     ERR_DATACOMPARE_FAIL           =   0x94,        // 9-4  Data Compare Fail
     ERR_DRAMCAL_FAIL               =   0x95,        // 9-5  DRAMCAL Fail
     ERR_DRAMALL_FAIL               =   0x96,        // 9-6  DRAMALL Fail
     ERR_ORISUPBLK_FAIL             =   0x97,        // 9-7  ORISUPBLK Fail
     ERR_SIDATA_FAIL                =   0x98,        // 9-8  SIDATA Fail
     ERR_DRAMDLL_OVLM               =   0x99,        // 9-9  DRAMDLL OverRange
     ERR_PLIST1_FAIL                =   0x9A,        // 9-A  SCAN_DEFECT_FAIL
     ERR_DRAM_HT_FAIL               =   0x9B,        // 9-B  DRAM_HT_FAIL
     ERR_NO_EXE                     =   0xFF,
     ERR_NULL                       =   0x7FFFFFFF
}tERROR_AGING;

typedef enum
{
    STATE_ERASE_FAIL                =   0xAA,
    STATE_ERASE_PASS                =   0xAB,
    STATE_WRITE_FAIL                =   0xBB,
    STATE_WRITE_PASS                =   0xBC,
    STATE_READ_FAIL                 =   0xCC,
    STATE_READ_PASS                 =   0xCD,
    STATE_NULL                      =   0x7FFFFFFF
}tSTATE_AGING;

typedef struct
{
    tERROR_AGING    ErrorCode;
    tSTATE_AGING    State;
    U32             Tick;
    U32             TotalTime;
    U16             CurrLoopCnt;
    U16             TotalLoopWrite;
    U16             TotalLoopRead;
    U16             TotalLoopErase;
    U16             LogBlock;
    U16             Parameter;
    U16             ReRunCnt;
    U16             MinFreeIdx;
}tAGING_INFO;                       // 32B
COMPILE_ASSERT((sizeof(tAGING_INFO) & 0x3) == 0, "please align to 4 bytes!");

typedef struct
{
    U16             LoopDefectCnt;
    U16             LoopErrEraseCnt;
    U16             LoopErrWriteCnt;
    U16             LoopErrReadCnt;
}tAGING_LOOP;                       // 8B
COMPILE_ASSERT((sizeof(tAGING_LOOP) & 0x3) == 0, "please align to 4 bytes!");

typedef struct
{
    tPAA*           ErrPaa;
    U32*            ErrPaaCnt;
    U32             DefectCntTotal;
    U32             Reserved;
}tAGING_ERROR;                      // 16B
COMPILE_ASSERT((sizeof(tAGING_ERROR) & 0x3) == 0, "please align to 4 bytes!");

typedef struct
{
    U8  ch;                     ///< ch
    U8  ce;                     ///< ce
    U8  lun;                    ///< lun
    U8  reserved1[1];
    U8  ID[8];
    U8  reserved2[4];
} tAGING_FLASHID;                   // 16B
COMPILE_ASSERT((sizeof(tAGING_FLASHID) & 0x3) == 0, "please align to 4 bytes!");

typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t THRSHD               :10;         ///<BIT [9:0] Threshold.
        uint16_t RF200C028_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;
} EcuCfg5_t_CPU2;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define AGING_GET_DIE(LUN, CE, CH)         ((LUN << gNfInfo.bitsChDev) | (CE << (gNfInfo.bitsCh)) | CH)
#define AGING_GET_SBLK_IDX(PN, BLK, DIE)   (((U32)(PN * gNfInfo.blocksPerPlane + BLK) << AGING_DIE_SHIFT) | DIE)

#define AGING_TEMPERATURE_SOC(MAXSOC) {                                                                     \
    if (MAXSOC < (U16)HalTempSensor_GetTemperature())                                                       \
    {                                                                                                       \
        MAXSOC = (U16)HalTempSensor_GetTemperature();                                                       \
        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[gAgingInfo.CurrLoopCnt].SOC = MAXSOC;    \
    }                                                                                                       \
}
#define AGING_TEMPERATURE_I2C(MAXI2C) {                                                                     \
    if (MAXI2C < (S16)AplThermal_GetI2cTemperature())                                                       \
    {                                                                                                       \
        MAXI2C = (S16)AplThermal_GetI2cTemperature();                                                       \
        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[gAgingInfo.CurrLoopCnt].I2C = MAXI2C;    \
    }                                                                                                       \
}

#define AGING_TEMPERATURE(MAXSOC, MAXI2C){       \
    if (gAgingInfo.CurrLoopCnt < AGING_MAX_LOOP) \
    {                                            \
        AGING_TEMPERATURE_SOC(MAXSOC)            \
        AGING_TEMPERATURE_I2C(MAXI2C)            \
    }                                            \
}

#define AGING_SAVELOOPTIME(RERUNCOUNT,LOOP, TICK) {                 \
    smSysInfo->d.AgingData.d.AgingLoop.LoopTime[LOOP]        += TICK;    \
    smSysInfo->d.AgingData.d.AgingRerun.RerunTime[RERUNCOUNT - 1] += TICK;    \
}

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern tAGING_INFO                  gAgingInfo;
extern tAGING_ERROR                 gAgingError;
extern tAGING_LOOP                  gAgingLoop;
extern tPAA*                        gAgingFastPaa1Buf;
extern tPAA*                        gAgingFastPaa2Buf;
extern U32                          gdwAgEnFrag;
extern U16*                         gwAgRemapTbl;
extern U8*                          gbAgflagTbl;
extern U8*                          gbAgBuffer;
//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
tSTATUS_AGING Aging_CheckIssueSBlk(U16 block, U8 ce, U8 lun);
U8 Aging_Nand_EccScan(tPAA paa, U16 type, U16 cnt);
void Aging_Util_BubbleSort(void);
void Aging_Util_ReBuildTable(void);
void Aging_JobIssue(tFIO_JOB* pJob);
void Aging_JobWait(void);

void Aging_OneTimeInit(InitBootMode_t initMode);
void Aging_MainLoop(void);
void Aging_Center(U32 opcode, U32 loop, U32 mode);

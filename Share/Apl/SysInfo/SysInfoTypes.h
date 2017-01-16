#pragma once
#pragma pack(4)
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
//  This is SysInfoTypes.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "HalNfif.h"
#include "HalEcu.h"
#if _TCG_ != TCG_NONE
#include "TcgTbl.h"
#endif
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
// Payload Size
#define SI_PAYLOAD_SIZE_HEADER              (KBYTE * 4)
#define SI_PAYLOAD_SIZE_MISC                (KBYTE * 4)
#define SI_PAYLOAD_SIZE_PLSTCNT             (KBYTE * 4)
#define SI_PAYLOAD_SIZE_TCG                 (KBYTE * 4)
#define SI_PAYLOAD_SIZE_MPINFO              (KBYTE * 4)
#define SI_PAYLOAD_SIZE_NVME                (KBYTE * 8)
#define SI_PAYLOAD_SIZE_LOGINFO             (KBYTE * 24)
#define SI_PAYLOAD_SIZE_AGING               (KBYTE * 16)
#define SI_PAYLOAD_SIZE_GLIST               (KBYTE * 8)
#define SI_PAYLOAD_SIZE_UNCT                (KBYTE * 16)
#define SI_PAYLOAD_SIZE_NVEC                (KBYTE * 4)
#define SI_PAYLOAD_SIZE_FTL                 (KBYTE * 4)
#define SI_PAYLOAD_SIZE_ECNT                (KBYTE * 32)
//======================^ Main Area ^======================//
#define SI_PAYLOAD_SIZE_PLIST1              (KBYTE * 256)
#define SI_PAYLOAD_SIZE_PLIST2              (KBYTE * 256)
#define SI_PAYLOAD_SIZE_FTLRMP              ((MBYTE * 2) + (KBYTE * 512))
#define SI_PAYLOAD_SIZE_FTLRMP_SCE          (KBYTE * 640)  //This is When die num < 32 RMP Size
//======================v Aging Area v======================//
#if (_NAND_ == NAND_MLC)
    #define SI_PAYLOAD_SIZE_AGINGEI         (MBYTE * 2)
    #define AGING_ERRDATA_SIZE              (1956)
#elif (_NAND_ == NAND_3DTLCG2)
    #define SI_PAYLOAD_SIZE_AGINGEI         (MBYTE * 4)
    #define AGING_ERRDATA_SIZE              (3912)
#endif

#define SI_PAYLOAD_MAIN_CUR_SIZE            (SI_PAYLOAD_SIZE_HEADER     + \
                                             SI_PAYLOAD_SIZE_MISC       + \
                                             SI_PAYLOAD_SIZE_PLSTCNT    + \
                                             SI_PAYLOAD_SIZE_TCG        + \
                                             SI_PAYLOAD_SIZE_MPINFO     + \
                                             SI_PAYLOAD_SIZE_NVME       + \
                                             SI_PAYLOAD_SIZE_LOGINFO    + \
                                             SI_PAYLOAD_SIZE_AGING      + \
                                             SI_PAYLOAD_SIZE_GLIST      + \
                                             SI_PAYLOAD_SIZE_UNCT       + \
                                             SI_PAYLOAD_SIZE_FTL        + \
                                             SI_PAYLOAD_SIZE_ECNT       + \
                                             SI_PAYLOAD_SIZE_NVEC)

//NAND Configure
#define SI_NAND_MAX_CHANNEL         8
#define SI_NAND_MAX_CE              8
#define SI_NAND_MAX_LUN             4
#define SI_NAND_MAX_DIE             (SI_NAND_MAX_CHANNEL * SI_NAND_MAX_CE * SI_NAND_MAX_LUN)
#define SI_NAND_MAX_DEV_PER_CH      (SI_NAND_MAX_CE * SI_NAND_MAX_LUN)
#define SI_NAND_MAX_PLANE           4

//PList
#define SI_PLIST_MAX_COUNT          512 // Per Die (4 Planes)
#define SI_PLIST_MAX_COUNT_STEP     128

//GList
#define SI_GLIST_MAX_COUNT          ((SI_PAYLOAD_SIZE_GLIST - sizeof(tSI_GLIST_BRIEF)) / sizeof(tSI_GLIST_ENTRY))

//TCG Configure
#define SI_MAX_TCGINFO_CNT          4
#define SI_TCGINFO_TAG              0x00544347

//QBT Configure
#define SI_QBTINFO_TAG              0x00514254

//NVMe - Set Feature configure
#define SI_FEATURE_TAG              0x52464553  // 'SEFR'

//-----------------------------------------------------------------------------
//  Private data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

///---------------------------///
///--- PList Section Start ---///
/// @brief structure: PList count
typedef struct _tSI_PLIST_CNT
{
    U16     wCount[SI_NAND_MAX_CHANNEL][SI_NAND_MAX_CE][SI_NAND_MAX_LUN];
            //   256B = 2B * 8 * 8 * 4
} tSI_PLIST_CNT;

/// @brief structure: PList payload
typedef struct _tSI_PAYLOAD_PLIST
{
    U16     wList[SI_NAND_MAX_CHANNEL][SI_NAND_MAX_CE][SI_NAND_MAX_LUN][SI_PLIST_MAX_COUNT];
            //   256KB = 2B * 8 * 8 * 4
} tSI_PAYLOAD_PLIST;
///--- PList Section End!! ---///
///---------------------------///

///---------------------------///
///--- GList Section Start ---///
typedef struct _tSI_GLIST_ENTRY
{
    U8      bType;
    U8      bDie;
    U16     wPhyBlock;
} tSI_GLIST_ENTRY;

typedef struct _tSI_GLIST_BRIEF
{
    U16     wCntErase;
    U16     wCntWrite;
    U16     wCntRead;
    U16     Reserved;
} tSI_GLIST_BRIEF;

typedef union _tSI_PAYLOAD_GLIST
{
    U8  all[SI_PAYLOAD_SIZE_GLIST];
    struct
    {
        tSI_GLIST_BRIEF     GListBrief;
        tSI_GLIST_ENTRY     GList[SI_GLIST_MAX_COUNT];
    } d;
} tSI_PAYLOAD_GLIST;
COMPILE_ASSERT(sizeof(tSI_PAYLOAD_GLIST) == SI_PAYLOAD_SIZE_GLIST, "tSI_PAYLOAD_GLIST size ERROR");
///--- GList Section End!! ---///
///---------------------------///

///--------------------------///
///--- Misc Section Start ---///
typedef struct {
    U32     sn;
    U16     block;
    U16     reserved;
} tSI_QBT_INFO;

typedef union {
    U8          all[256];
    struct {
        NfInfo_t             NandFlashInfo; // 220B
        U32                  NFInfoTag;     //   4B
        U8                   Reserved[32];   //  32B
    } d;
} tSI_NF_INFO;
COMPILE_ASSERT(sizeof(tSI_NF_INFO) == 256, "tSI_NF_INFO size ERROR");

typedef union {
    U8  all[464];
    struct {
        tPAA                    QSPAACurrent[10]; // 16B
        tPAA                    RMPHistory[2];    //  8B For ErrHandle
        U64                     SIHistory_L;      //  8
        U64                     SIHistory_H;      //  8
        U8                      SIBadBank[SI_NAND_MAX_CE * SI_NAND_MAX_LUN * SI_NAND_MAX_CHANNEL]; // 256B
    } d;
} tSI_INFO_MGR;
COMPILE_ASSERT(sizeof(tSI_INFO_MGR) == 464, "tSI_QS_INFO size ERROR");

/// @brief structure: SysInfo misc
typedef union {
    U8  all[SI_PAYLOAD_SIZE_MISC];
    struct {
        tSI_INFO_MGR            InfoMgr;                                    // 464B
        tSI_NF_INFO             NandInfo;                                   // 256B
    } d;
} tSI_PAYLOAD_MISC;
COMPILE_ASSERT(sizeof(tSI_PAYLOAD_MISC) == SI_PAYLOAD_SIZE_MISC, "tSI_PAYLOAD_MISC size ERROR");
///--- Misc Section End!! ---///
///--------------------------///

/// @brief structure: SysInfo tSI_PAYLOAD_PLSTCNT
typedef union {
    U8  all[SI_PAYLOAD_SIZE_PLSTCNT];
    struct {
        tSI_PLIST_CNT           PList1Cnt;                                  // 256B
        tSI_PLIST_CNT           PList2Cnt;                                  // 256B
    } d;
} tSI_PAYLOAD_PLSTCNT;
COMPILE_ASSERT(sizeof(tSI_PAYLOAD_PLSTCNT) == SI_PAYLOAD_SIZE_PLSTCNT, "tSI_PAYLOAD_MISC size ERROR");
///--- Misc Section End!! ---///
///--------------------------///

///---------------------------///
///--- TCG Section Start ---///
/// @brief structure: SysInfo misc
typedef union {
    U8  all[SI_PAYLOAD_SIZE_TCG];
    struct {
        U32                     TCGBlockTag;                                //   4B
        U16                     TCGBlockNo[SI_MAX_TCGINFO_CNT];             //   8B
        #if _TCG_ != TCG_NONE
        tSYSINFO_TCG            TCGUsed;            // TCG require 2KB
        #else
        U8                      TCGRsvd[2 * KBYTE]; // TCG require 2KB
        #endif
    } d;
} tSI_PAYLOAD_TCG;
COMPILE_ASSERT(sizeof(tSI_PAYLOAD_TCG) == SI_PAYLOAD_SIZE_TCG, "tSI_PAYLOAD_MISC size ERROR");

///--- TCG Section End!! ---///
///--------------------------///

///---------------------------///
///--- Aging Section Start ---///
typedef struct {
    U16     SOC;
    S16     I2C;
} tAG_TEMP;
COMPILE_ASSERT((sizeof(tAG_TEMP) & 0x3) == 0, "please align to 4 bytes!");

typedef struct{
	U16      Type;
    U8       Loop;
    U8       Ch;
    U8       Ce;
    U8       Lun;
    U16      Page;
    U16      Block;
    U16      PairBlk[3];        ///16B
    U16      ErrType;
    U16      LogBlock;
    tAG_TEMP Temperature;
    U8       EccCw[8];          ///16B
    U8       EccStatus;
    U8       RetryCnt;
    U8       ToggleStatus;
    U8       PlaneCnt;
    U8       Reserved[4];
    U8       ToggleCw[8];       ///16B
    U16      Distribution[512];
} tAG_ERRDATA;///1072B
COMPILE_ASSERT((sizeof(tAG_ERRDATA) & 0x3) == 0, "please align to 4 bytes!");

typedef union{
    U8 all[SI_PAYLOAD_SIZE_AGINGEI];    //MLC:2097152 //3DTLC:4194304
    struct{
        U32         ErrInfoTag;
        U32         ErrInfoValidTag;
        U16         ErrNonCWO;
        U16         ErrPaaCnt;
        U32         Reserved1;      //16B
        U16         ErrAuCnt;
        U16         ErrEccStart;
        U16         ErrToggleStart;
        U16         ErrRetryStart;
        U16         ErrDisStart;
        U16         ErrTotalErase;
        U16         ErrTotalWrite;
        U16         ErrTotalRead;  //16B
        U8          SN[16];        //16B
        tAG_ERRDATA Data[AGING_ERRDATA_SIZE];   //MLC:2096832 //3DTLC:4193664B
    }d;
} tSI_PAYLOAD_AGERRINFO;
COMPILE_ASSERT(sizeof(tSI_PAYLOAD_AGERRINFO) == (SI_PAYLOAD_SIZE_AGINGEI), "tAG_ERRINFO size ERROR");

typedef struct{
    U16     Loop;
    U16     BadBlockCnt;
} tAG_COUNT;
COMPILE_ASSERT((sizeof(tAG_COUNT) & 0x3) == 0, "please align to 4 bytes!");

typedef struct {
    U32       LoopTag;
    U32       LoopValidTag;
    U16       LoopTarget;
    U16       LoopCurrent;
    U32       Reserved1;
    U16       LoopErase;
    U16       LoopWrite;
    U16       LoopRead;
    U8        Reserved2[10];
    tAG_COUNT LoopDefectCnt[304];
    U32       LoopTime[300];
} tAG_LOOP;   /// 848B
COMPILE_ASSERT((sizeof(tAG_LOOP) & 0x3) == 0, "please align to 4 bytes!");

typedef struct {
    U32 ErrInfoTag;
    U32 ErrInfoValidTag;
    U32 ErrCntPlist1;
    U32 ErrCntPlist2;   //16B
    U16 TotalDie;
    U16 ErrDieCnt;
    U16 ErrCnt;
    U16 ErrNonCWO;
    U16 ErrTotalErase;
    U16 ErrTotalWrite;
    U16 ErrTotalRead;
    U8  Reserved[2];
} tAG_ERRINFO;           //32B
COMPILE_ASSERT((sizeof(tAG_ERRINFO) & 0x3) == 0, "please align to 4 bytes!");

typedef struct {
    U32     SocTypeTag;                       //  4B
    U32     AVS;                              //  4B
	U32     Reserved[2];
} tAG_SOC_TYPE;
COMPILE_ASSERT((sizeof(tAG_SOC_TYPE) & 0x3) == 0, "please align to 4 bytes!");

typedef struct {
    U32 ReRunTag;
    U32 ReRunValidTag;
    U16 ReRunCount;
    U16 RecordError;
    U8  Reserved1[4];
    U32 RerunTime[10];
    U8  Reserved2[8];
} tAG_RERUN;
COMPILE_ASSERT((sizeof(tAG_RERUN) & 0x3) == 0, "please align to 4 bytes!");

typedef union {
    U32 all[8];
    struct{
        U32 EraseTag;
        U32 TempAverEcCnt;
        U32 TempMaxEcCnt;
        U32 TempTotalEcCnt;
        U32 AverEcCnt;
        U32 MaxEcCnt;
        U32 TotalEcCnt;
        U32 AgingEraseCnt;
    }d;
} tERASECNT;

typedef struct {
    U32 NfifTag;
    U32 JEDECFail;
    U32 CRCFail;
    U8  FailCh;
    U8  FailCe;
    U8  FailLun;
    U8  FailPlane;
} tAG_NAND_PP;
COMPILE_ASSERT((sizeof(tAG_NAND_PP) & 0x3) == 0, "please align to 4 bytes!");

typedef struct {
    U8    DramDllPositive[4];
    U8    DramDllNegative[4];
} tAG_DRAMDLL;
COMPILE_ASSERT((sizeof(tAG_DRAMDLL) & 0x3) == 0, "please align to 4 bytes!");

typedef struct {
    U32         DramTag;
    U32         DramAddr;
    U32         DramWVal;
    U32         DramRVal;
    U32         DramRRVal;
    U32         DramErrorCode;
    U32         DramErrorCnt;
    tAG_TEMP    DramErrorTemp;
    U32         DramParameterTag;
    U32         DramDLLTag;
    tAG_DRAMDLL DramDLLAvg;
    tAG_DRAMDLL DramDLLMax;
    tAG_DRAMDLL DramDLLMin;
    U32         DramDLLCheckSum;
    U16         DramDLLFailCnt;
    U16         DramOpFrequency;
    U32         DramCALTag;
    U8          DramphyCalLsb;
    U8          DramphyCalMsb;
    U8          DramphyCalreserved1[2];
    U32         DramCALcheckSum;
    U8          DramReserved1[12];
} tAG_DRAM;
COMPILE_ASSERT((sizeof(tAG_DRAM) & 0x3) == 0, "please align to 4 bytes!");

typedef struct {
    U32 FlashIDTag;
    U16 FlashIDErrorCode;
    U16 FlashIDErrorCnt;
    U8  Reserved[8];
    U32 FlashIDmap[SI_NAND_MAX_CHANNEL]; // 32B
    U8  FlashID[SI_NAND_MAX_CHANNEL][SI_NAND_MAX_CE][SI_NAND_MAX_LUN][8]; //2048B
} tAG_FLASHID;
COMPILE_ASSERT((sizeof(tAG_FLASHID) & 0x3) == 0, "please align to 4 bytes!");

typedef struct {
    U32 PDTag;                  // 4B// PDIN
    U32 SOC_Temp;               // 4B//
    U16 Avg_PreliminaryDefect;  // 2B//
    U16 Avg_TotalDefect;        // 2B//
    U16 Avg_GrownDefect;        // 2B//
    U16 Max_Defect;             // 2B//
    U16 TpAvg;                  // 2B//
    U16 TrAvg;                  // 2B//
    U16 TeAvg;                  // 2B//
    U16 TpMax;                  // 2B//
    U16 TrMax;                  // 2B//
    U16 TeMax;                  // 2B//
    U8  LastFail_Overall;       // 1B//
    U8  LastFail_Erase;         // 1B//
    U8  LastFail_Prog;          // 1B//
    U8  LastFail_WL;            // 1B//
    U8  LastFail_UNC;           // 1B//
    U8  LastFail_ECC;           // 1B//
    U8  LastFail_Blank;         // 1B//
    U8  LastFail_WLLK;          // 1B//
	U8  LastFail_Prog_WL_Open;  // 1B//
	U8  LastFail_Prog_WL_Short; // 1B//
	U8  LastFail_Read_WL_Open;  // 1B//
	U8  LastFail_Read_WL_Short; // 1B//
    U8  Reserve[8];             // 8B// Total 48B
} tAG_PDINFO;
COMPILE_ASSERT((sizeof(tAG_PDINFO) & 0x3) == 0, "please align to 4 bytes!");

typedef struct {
    U32 SupperBlkTag;
    U8  SbkErrorCode;
    U16 SbkOpBlk;
    U16 SbkOpRatio;
    U8  Reserved[7];
    U32 DefectDieMap[SI_NAND_MAX_CHANNEL]; // 32B
    U32 SbkDieMap[SI_NAND_MAX_CHANNEL];    // 32B
    U16 DefectBlkCntU[SI_NAND_MAX_PLANE][SI_NAND_MAX_LUN][SI_NAND_MAX_CE][SI_NAND_MAX_CHANNEL];    // 2048B
    U16 DefectBlkCntT[SI_NAND_MAX_PLANE][SI_NAND_MAX_LUN][SI_NAND_MAX_CE][SI_NAND_MAX_CHANNEL];    // 2048B
} tAG_SBKCNT;
COMPILE_ASSERT((sizeof(tAG_SBKCNT) & 0x3) == 0, "please align to 4 bytes!");

typedef struct {
    U32      TemperatureTag;            //   4B
    U32      TemperatureIdle_High;      //   4B
    U32      TemperatureIdleTag;        //   4B
    tAG_TEMP TemperatureIdle_Max;       //   4B
    tAG_TEMP TemperatureIdle_Min;       //   4B
    tAG_TEMP TemperatureIdle_Avg;       //   4B
    tAG_TEMP TemperatureIdle[10];       //  40B
    U32      TemperatureEraseTag;       //   4B
    tAG_TEMP TemperatureErase_Max;      //   4B
    tAG_TEMP TemperatureErase_Min;      //   4B
    tAG_TEMP TemperatureErase_Avg;      //   4B
    tAG_TEMP TemperatureErase[300];     //1200B
    U32      TemperatureWriteTag;       //   4B
    tAG_TEMP TemperatureWrite_Max;      //   4B
    tAG_TEMP TemperatureWrite_Min;      //   4B
    tAG_TEMP TemperatureWrite_Avg;      //   4B
    tAG_TEMP TemperatureWrite[300];     //1200B
    U32      TemperatureReadTag;        //   4B
    tAG_TEMP TemperatureRead_Max;       //   4B
    tAG_TEMP TemperatureRead_Min;       //   4B
    tAG_TEMP TemperatureRead_Avg;       //   4B
    tAG_TEMP TemperatureRead[300];      //1200B  Total: 3712B
} tAG_TEMPERATURE;
COMPILE_ASSERT((sizeof(tAG_TEMPERATURE) & 0x3) == 0, "please align to 4 bytes!");

typedef struct {
    U32     NBTimeTag;                  //NBTI 0x4954424E
    U32     NBTimeValidTag;
    U32     Reserved[2];
    U32     Program[SI_NAND_MAX_DIE];         //1024B
    U32     Erase[SI_NAND_MAX_DIE];           //1024B
    U32     Read[SI_NAND_MAX_DIE];            //1024B
} tAG_NAND_BUSYTIME;  // 3088B
COMPILE_ASSERT((sizeof(tAG_NAND_BUSYTIME) & 0x3) == 0, "please align to 4 bytes!");

/// @brief structure: SysInfo aging
typedef union {
    U8                      all[SI_PAYLOAD_SIZE_AGING];
    struct {
        U32                 Aging_Signature;                      //     4B
        U8                  Aging_ModelName[8];                   //     8B
        U16                 Aging_VerNo;                          //     2B
        U16                 Aging_Mode;                           //     2B
        U8                  Aging_SN[16];                         //    16B
        U16                 Aging_ErrorCode;                      //     2B
        U16                 Aging_Identify;                       //     2B
        U32                 Aging_ElapsedTime;                    //     4B
        U32                 Aging_EraseCnt;                       //     4B
        U32                 Aging_Reserved;                       //     4B
        tAG_SOC_TYPE        AgingSoc;                             //    16B  ///   48B
        tAG_RERUN           AgingRerun;                           //    64B  ///  112B
        tAG_LOOP            AgingLoop;                            //  2448B  /// 2560B
        tAG_ERRINFO         AgingErrInfo;                         //    32B  /// 2592B
        tAG_NAND_PP         AgingNandPP;                          //    16B  /// 2608B
        tAG_PDINFO          AgingPdinfo;                          //    48B  /// 2656B
        tAG_DRAM            AgingDram;                            //    96B  /// 2752B
        tAG_FLASHID         AgingFlashID;                         //  2096B  /// 4848B
        tAG_NAND_BUSYTIME   AgingNandBusyTime;                    //  3088B  /// 7936B
        tAG_TEMPERATURE     AgingTemperature;                     //  3712B  ///11648B
        tAG_SBKCNT          AgingSbkCnt;                          //  4176B  ///15824B
    } d;
} tSI_PAYLOAD_AGING;
COMPILE_ASSERT(sizeof(tSI_PAYLOAD_AGING) == SI_PAYLOAD_SIZE_AGING, "tSI_PAYLOAD_AGING size ERROR");
///--- Aging Section End!! ---///
///---------------------------///


///--------------------------///
///--- NVMe Section Start ---///
typedef struct {
        U32       Tag;
        U32       SaveFeature[16];        // 4B *   16        =    64B
        U64       SaveAutoPST[32];        // < Autonomous Power State Transition
        U16       SaveTMPTH[2][2];        // [0:Composite,1:Sensor 1][0:Over,1:Under]
        U16       SaveLightTMPTH;         // Light Throttling temperature threshold
        U16       SaveHeavyTMPTH;         // Heavy Throttling temperature threshold
        U32       Reserve[36];
} tSV_FEA;

typedef struct
{
        U16       TAG;          //              2B
        U16       Cnt;          //              2B
        U32       LAA[66];      // 66 * 4 =   264B
        U8        BIT[66];      //             66B
        U8        Reserve[2];   //              2B
} tSV_WUNC;

#define SEC_PW_SIZE     16      ///< 16 Word = 32 Bytes
typedef struct
{
    U16           TAG;                      //  2 B
    U16           Reserve;                  //  2 B
    U16           UserPw[SEC_PW_SIZE];      // 32 Bytes
    U16           MasterPw[SEC_PW_SIZE];    // 32 Bytes
    U16           MasterRv;                 //  2 Bytes
    U8            Level;                    //  1 Byte
    U8            PwEnable;                 //  1 Byte
    U32           CheckSum;                 //  4 Bytes
} tSECURITY_BACKUP;

/// @brief NVMe Name space
typedef struct
{
    U32                     TAG;                        //            4 B
    U16                     numOfNamespacesAttached;    //            2 B
    U16                     numOfNamespacesCreated;     //            2 B
    NvmeIfMgrNamespace_t    ns[HW_MAX_NAMESPACES];      // 32 * 4 = 128 B      Total: 136 B
} tNAMESPACE;

typedef union {
    U8          all[SI_PAYLOAD_SIZE_NVME];
    struct {
        tSV_FEA              Feature;       //  4B *  120 =  480
        tSV_WUNC             WUNC;          //               336
        tSECURITY_BACKUP     SCU_CMD;       //                76
        tNAMESPACE           Namespace;     //               136
        U32                  Reserved[1791];//  4B * 1791 = 7164        Total 8KB
    } d;
} tSI_PAYLOAD_NVME;
COMPILE_ASSERT(sizeof(tSI_PAYLOAD_NVME) == SI_PAYLOAD_SIZE_NVME, "tSI_PAYLOAD_NVME size ERROR");
///--- NVMe Section End!! ---///
///--------------------------///

///-------------------------///
///--- Log Section Start ---///
typedef struct {
    U16 ByteLoc     :8;
    U16 BitLoc      :3;
    U16 Reserv      :5;
} tPAR_ERR_LOC;

typedef struct {
    U64             ErrCnt;
    U16             sqID;
    U16             cmdID;
    U16             StaField;
    tPAR_ERR_LOC    ParaErrLoc;
    U64             lba;
    U32             nameSpace;
    U8              VenSpeInfoAvi;
    U8              reserv_1[3];
    U64             cmdSpeInfo;
    U8              reserv_2[24];
} tERR_LOG_ENTRY;
COMPILE_ASSERT(sizeof(tERR_LOG_ENTRY) == 64, "tERR_LOG_ENTRY size ERROR");

typedef struct {
    U64             TotalErrCnt;
    tERR_LOG_ENTRY  ErrLogEntry[NVME_ERROR_LOG_ENTIRIES];
} tERR_LOG;
COMPILE_ASSERT(sizeof(tERR_LOG) == ((64 * NVME_ERROR_LOG_ENTIRIES) + 8), "tERR_LOG size ERROR");

typedef struct {
    U32 unsafeFlag  :1;
    U32 reserve     :31;
} tLOG_FLAG;
COMPILE_ASSERT(sizeof(tLOG_FLAG) == sizeof(U32), "tSMR_LOG size ERROR");

#pragma pack(1)
typedef struct {
    U64 dataUniRd;          // 1
    U64 dataUniWr;          // 2
    U64 hostRdCmd;          // 3
    U64 hostWrCmd;          // 4
    U64 ctrlBusyTime;       // 5
    U64 pwrCycCnt;          // 6
    U64 pwrOnHrs;           // 7
    U64 unsafePwrCycCnt;    // 8
    U64 medAndDataErrCnt;   // 9
    U64 errLogEntryCnt;     // 10
    U64 warnTempTime;       // 11
    U64 critTempTime;       // 12
    U8  availableSpare;     // 13
    U8  percentageUsed;
    U8  ReAssignedSecCnt;            // ReAssignedSectorCount
    U8  PgmFailCnt_W;                // ProgramFailCountWorst
    U8  PgmFailCnt_T;                // ProgramFailCountTotal
    U8  ErsFailCnt_W;                // EraseFailCountWorst
    U8  ErsFailCnt_T;                // EraseFailCountTotal
    U8  WearLvlCnt;                  // WearLevelingCount;
    U32 RsvBlkCnt;          // 14    // ReservedBlockCount;
    U8  UsedRsvBlkCnt_W;             // UsedReservedBlockCountWorst;
    U8  UsedRsvBlkCnt_T;             // UsedReservedBlockCountTotal;
    U64 WCSFlushCnt;
    U64 WCSUnalignedIO;
    U32 WCSLbaMapCnt;
    U8  WCSWearRangeDelta;
    U8  CriticalWarn;
    U8  reserv_17[4];
    U64 writtenToNAND[2];   // 18
    U64 reserv_20[21];
} tSMR_LOG;
#pragma pack()
COMPILE_ASSERT(sizeof(tSMR_LOG) == (40 * sizeof(U64)), "tSMR_LOG size ERROR");

typedef struct
{
    U8  DSTResult:4;     ///< Self-test Result
    U8  DSTCode:4;       ///< Self-test Code

    U8  Segment;         ///< Segment Number (bytes[1])

    U8  Valid;           ///< Valid Diagnostic Information (bytes[2])

    U8  reserved3;       ///< Reserved  (bytes[3])
    U32 POH[2];          ///< Power On Hours  (bytes[11:4])
    U32 NSID;            ///< Namespace Identifier  (bytes[15:12])
    U32 FLBA[2];         ///< Failing LBA (bytes[23:16])
    U8  CodeType:3;      ///< Status Code Type (bytes[24] bits[2:0])
    U8  reserved24:5;    ///< reserved         (bytes[24] bits[7:3])

    U8  StatusCode;      ///< Status Code (bytes[25])
    U8  reserved26[2];   ///< Vendor Specific (bytes[27:26])

} tDST_LOG_ENTRY;
COMPILE_ASSERT(sizeof(tDST_LOG_ENTRY) == 28, " DST Log Entry size shall be 28");

typedef struct {
    U32             Tag;
    U32             TotalDSTCnt;
    tDST_LOG_ENTRY  DSTLogEntry[20];
    tDST_LOG_ENTRY  DSTResult;
    U8              DSTErr;
    U8              reserved1[3];
} tDST_LOG;
COMPILE_ASSERT(sizeof(tDST_LOG) == ((28 * 21) + 12), "tERR_LOG size ERROR");

typedef union {
    U8  all[SI_PAYLOAD_SIZE_LOGINFO];
    struct {
        tLOG_FLAG       LogFlags;       //  4B *  1          =     4B
        tERR_LOG        ErrLog;         // 64B * 64Entry + 8 =  4104B
        tSMR_LOG        SmrLog;         //                   =   320B
        tDST_LOG        DSTLog;         //                   =   600B
        //U32             Reserve[4887];  //  4B * 4887        = 19548B => Total 24KB
        U32             Reserve[791];  //  4B * 791          =  3164B => Total 8KB
    } d;
} tSI_PAYLOAD_LOGINFO;
COMPILE_ASSERT(sizeof(tSI_PAYLOAD_LOGINFO) == SI_PAYLOAD_SIZE_LOGINFO, "tSI_PAYLOAD_LOGINFO size ERROR");

///--- Log Section End!! ---///
///-------------------------///

///------------------------------///
///--- ECCTable Section Start ---///
typedef union _tSI_PAYLOAD_UNCT
{
    U8  all[SI_PAYLOAD_SIZE_UNCT];
    struct
    {
        U8  all[SI_PAYLOAD_SIZE_UNCT];
    } d;
} tSI_PAYLOAD_UNCT;
COMPILE_ASSERT(sizeof(tSI_PAYLOAD_UNCT) == SI_PAYLOAD_SIZE_UNCT, "tSI_PAYLOAD_UNCT size ERROR");
///--- ECCTable Section End!! ---///
///------------------------------///

///--------------------------///
///--- NVEC Section Start ---///
typedef union {
    U8  all[SI_PAYLOAD_SIZE_NVEC];
    struct {
      tERASECNT EraseCnt; // 32Bytes
    } d;
} tSI_PAYLOAD_NVEC;
COMPILE_ASSERT(sizeof(tSI_PAYLOAD_NVEC) == SI_PAYLOAD_SIZE_NVEC, "tSI_PAYLOAD_NVEC size ERROR");
///--------------------------///
///--- NVEC Section Start ---///

///----------------------------///
///--- MPInfo Section Start ---///
typedef struct {
    U32     StationSignature;
    U8      FunctionBoard;
    U8      FunctionAging;
    U16     FunctionFinal;
    U16     FunctionFC;
    U16     FunctionOQA;
    U16     FunctionRE;
    U16     FunctionTCG;
} tMP_STATION_CODE;

typedef union {
    U8                      all[SI_PAYLOAD_SIZE_MPINFO];
    struct{
        U32                 AgingSignature;     //start at 0x0
        U8                  AgingMode;
        U8                  P11Msg;
        U16                 AdditionalLength;
        U32                 LoopCurrent;
        U32                 LoopTarget;
        U32                 MaxRetry;
        U32                 PowerCycle;
        U32                 PowerCycleSpec;
        U8                  ThermistorCheck;
        U8                  Reserved1;
        U8                  EccEnabled;
        U8                  EccThreshold;
        U16                 ErrorCode;
        U16                 CurrentCommand;
        U32                 ElapsedTime;
        U8                  NumberOfChannel;
        U8                  NumberOfDevice;
        U16                 NumberOfBlock;
        U16                 NumberOfPage;
        U16                 EccCount;
        U32                 TotalErrorWrite;
        U32                 TotalErrorRead;
        U32                 TotalErrorErase;
        U32                 TotalErrorCompare;
        U32                 TotalLoopWrite;
        U32                 TotalLoopRead;
        U32                 TotalLoopErase;
        U32                 TotalLoopCompare;
        U32                 DefectCountPreliminary;
        U32                 DefectCountTotal;
        U32                 DefectCountGrown;
        U32                 PhysicalTrackNo;
        U32                 GListTimeFirst;
        U32                 GListTimeLast;
        U32                 TimeWholeTest;
        U32                 TimeEnterFinalStage;
        U8                  Reserved2[17];
        U8                  AgingLED;
        U8                  ReAgingCount;
        U8                  HWShortCH;                  //?
        U8                  Reserved3[28];
        U8                  SampleTest;
        U8                  FWFeature;
        U8                  Reserved4[14];
        U64                 ReworkVersion;
        U8                  WorldWideName[8];
        U8                  SerialNumberDrive[32];
        U8                  SerialNumberPCBA[32];
        tMP_STATION_CODE    StationCode[2];
        U32                 StationSignature;
        U16                 Reserved5;
        U16                 LastBlockDefectIndex;
        U32                 VersionNo;
        U16                 AgingCollectDataTemp;
        U8                  Aging1CHEnable;
        U8                  Aging1CHSelect;             //0x12F
        U8                  Reserved6[208];
        U8                  PPID[512];
    } d;
} tSI_PAYLOAD_MPINFO;
COMPILE_ASSERT(sizeof(tSI_PAYLOAD_MPINFO) == SI_PAYLOAD_SIZE_MPINFO, "tSI_PAYLOAD_MPINFO size ERROR");
///--- MPInfo Section End!! ---///
///----------------------------///

///--------------------------///
///--- FTL0 Section Start ---///
/// @brief structure: SysInfo FTL0
typedef union {
    U8          all[SI_PAYLOAD_SIZE_FTL];
    struct {
        U32             QBTBlockTag;                                //   4B
        tSI_QBT_INFO    QBTInfo[4];                                 //  32B // 1092B
        U16             FreeIdx[SI_NAND_MAX_CHANNEL * SI_NAND_MAX_CE * SI_NAND_MAX_LUN][4];
        U32             zeroTag;
        U16             StartIdx;
        U16             EndIdx;
    } d;
} tSI_PAYLOAD_FTL;

typedef union {
    U8          all[SI_PAYLOAD_SIZE_FTLRMP];
    struct {
        U8      SBlkRmpInfo[SI_PAYLOAD_SIZE_FTLRMP];   //  23K + 512k
    } d;
} tSI_PAYLOAD_FTL_RMPBLK;
///--- FTL Section End!! ---///
///--------------------------///

///----------------------///
///--- EraseCnt Start ---///
/// @brief structure: SysInfo aging temperature
typedef union _tSI_PAYLOAD_ECNT
{
    U8  all[SI_PAYLOAD_SIZE_ECNT];
    struct
    {
        U16     EraseCount[16 * KBYTE]; //  8 KBYTE
	} d;
} tSI_PAYLOAD_ECNT;
COMPILE_ASSERT(sizeof(tSI_PAYLOAD_ECNT) == SI_PAYLOAD_SIZE_ECNT, "tSI_PAYLOAD_ECNT size ERROR");
///--- EraseCnt End!! ---///
///----------------------///

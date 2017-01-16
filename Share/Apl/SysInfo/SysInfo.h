#pragma once
// ============================================================================
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
/*
    This computer program contains confidential and proprietary information,
    and may NOT be reproduced or transmitted, in whole or in part, in any form,
    or by any means electronic, mechanical, photo-optical, or otherwise, and
    may NOT be translated into another language without the express written
    permission from LITEON Corporation.
*/
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
//=============================================================================

//-----------------------------------------------------------------------------
//  This is SysInfo.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "SysInfoTypes.h"
#include "BuildCommon.h"
//-----------------------------------------------------------------------------
//  Definitions: feature/function switch
//-----------------------------------------------------------------------------

// Media Operation SI_MO
#define SYSINFO_MEDIA_OPERATION_NOR     TRUE
#define SYSINFO_MEDIA_OPERATION_NAND    TRUE

#define SI_MS_NAND_QUICKSEARCH          TRUE

// Debugging Messenger
#define SI_PRINTF_NECESSARY(F, args...)     Debug_DbgPrintf(F, ##args)

#if   _NAND_ == NAND_3DTLCG2
    #define SYSINFO_NAND_PAGECNT_STEP       3
#elif _NAND_ == NAND_MLC
    #define SYSINFO_NAND_PAGECNT_STEP       2
#endif
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define SI_SET_ERASEBMP(bankIdx)           BIT_SET(gSysInfoMgr.eraseBitMap, BIT(bankIdx))
#define SI_CLR_ERASEBMP(bankIdx)           BIT_CLR(gSysInfoMgr.eraseBitMap, BIT(bankIdx))
#define SI_CHK_ERASEBMP(bankIdx)           (gSysInfoMgr.eraseBitMap & BIT(bankIdx))

#define SI_GET_IDX_BY_PAA_NEXT(paa)        (gSysInfoMgr.bankEntry[SyIn_NAND_BankIdx(paa)].bNext)
#define SI_GET_IDX_BY_PAA_CURR(paa)        (SyIn_NAND_BankIdx(paa))
#define SI_GET_HEALTH_BY_CURRPAA           (SyIn_NAND_HealthBank(SyIn_NAND_BankIdx(gSysInfoMgr.QSPAACurrent[0])))
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
//Operation Area
#define SI_AREA_MISC                        B_0
#define SI_AREA_PLSTCNT                     B_1
#define SI_AREA_TCG                         B_2
#define SI_AREA_MPINFO                      B_3
#define SI_AREA_NVME                        B_4
#define SI_AREA_SMART                       B_5
#define SI_AREA_AGING                       B_6
#define SI_AREA_GLIST                       B_7
#define SI_AREA_UNCT                        B_8
#define SI_AREA_NVEC                        B_9
#define SI_AREA_FTL                         B_10
#define SI_AREA_ECNT                        B_11
#define SI_AREA_PLIST1                      B_12
#define SI_AREA_PLIST2                      B_13
#define SI_AREA_FTLRMP                      B_14
#define SI_AREA_AGINGEI                     B_15
#define SI_AREA_INIT                        B_31

#define SI_AREA_ALL_MASK                    (SI_AREA_PLIST1 | SI_AREA_FTLRMP | SI_AREA_AGINGEI | SI_AREA_INIT)

// Header - Tag
#define SI_TAG_SYSINFO                      0x4E495953  // 'SYIN'
#define SI_TAG_MISC                         0x4353494D  // 'MISC'
#define SI_TAG_PLSTCNT                      0x43544C50  // 'PLTC'
#define SI_TAG_MPINFO                       0x4E49504D  // 'MPIN'
#define SI_TAG_NVME                         0x4E564D65  // 'NVMe'
#define SI_TAG_LOGINFO                      0x49474F4C  // 'LOGI'
#define SI_TAG_AGING                        0x4E494741  // 'AGIN'
#define SI_TAG_ECNT                         0x544E4345  // 'ECNT'
#define SI_TAG_GLIST                        0x54534C47  // 'GLST'
#define SI_TAG_UNCTABLE                     0x54434E55  // 'UNCT'
#define SI_TAG_NVEC                         0x4345564E  // 'NVEC'
#define SI_TAG_PLIST1                       0x31544C50  // 'PLT1'
#define SI_TAG_PLIST2                       0x32544C50  // 'PLT2'
#define SI_TAG_FTL                          0x544C5446  // 'FTLT'
#define SI_TAG_FTLRMP                       0x524C5446  // 'FTLR'
#define SI_TAG_ZERO                         0x5A414553  // 'ZAES'
#define SI_TAG_TCG                          0x54474354  // 'TCGT'
#define SI_TAG_CRC                          0x54435243  // 'CRCT'
#define SI_TAG_AGINGEI                      0x49454741  // 'AGEI'

#define FTL_OVERRIDE_TAG                    0x4C54464B  // 'KFTL'

// Header - VerNo
#define SI_VER_SYSINFO                      0x00020001  // Version 2.1
#define SI_VER_MISC                         0x00020001  // Version 2.1
#define SI_VER_PLSTCNT                      0x00020001  // Version 2.1
#define SI_VER_MPINFO                       0x00020001  // Version 2.1
#define SI_VER_NVME                         0x00020002  // Version 2.1
#define SI_VER_LOGINFO                      0x00020001  // Version 2.1
#define SI_VER_AGING                        0x00020001  // Version 2.1
#define SI_VER_ECNT                         0x00020001  // Version 2.1
#define SI_VER_GLIST                        0x00020001  // Version 2.1
#define SI_VER_UNCTABLE                     0x00020001  // Version 2.1
#define SI_VER_NVEC                         0x00020001  // Version 2.1
#define SI_VER_PLIST                        0x00020001  // Version 2.1
#define SI_VER_PLIST2                       0x00020001  // Version 2.1
#define SI_VER_FTL                          0x00020001  // Version 2.1
#define SI_VER_FTLRMP                       0x00020001  // Version 2.1
#define SI_VER_TCG                          0x00020001  // Version 2.1
#define SI_VER_AGINGEI                      0x00020001  // Version 2.1

// MGR STATUS
#define SI_S_INIT_FROM_SCRATCH              B_0
#define SI_S_OVERWRITE_RISK                 B_1
#define SI_S_FOUND_OLD_DATA                 B_2
#define SI_S_WRITE_FAIL                     B_3
#define SI_S_WRITE                          B_4
#define SI_S_ERASE                          B_5
#define SI_S_BUSY_MASK                      (SI_S_WRITE | SI_S_ERASE)

// MPINFO - Signature
#define STATION_SIGNATURE_TEST1             0x31545354  // 'TST1'
#define STATION_SIGNATURE_TEST2             0x32545354  // 'TST2'
#define STATION_SIGNATURE_AVER              0x52455641  // 'AVER'

// Hardware
#define PCIE_NVME_ELDORA                    0x1093      // Marvell PCIe NVMe Eldora ASIC

// Cache Size
#define SYSINFO_CACHE_SIZE_MAIN             (256 * KBYTE)
#define SYSINFO_CACHE_SIZE_PLIST            (SI_PAYLOAD_SIZE_PLIST1 + SI_PAYLOAD_SIZE_PLIST2) // PList1 + PList2
#define SYSINFO_CACHE_SIZE_FTLRMP           SI_PAYLOAD_SIZE_FTLRMP
#define SYSINFO_CACHE_SIZE_AGINGEI          SI_PAYLOAD_SIZE_AGINGEI

#define SYSINFO_PLIST_OFFSET                (SYSINFO_CACHE_SIZE_MAIN)
#define SYSINFO_FTLRMP_OFFSET               (SYSINFO_CACHE_SIZE_MAIN + SYSINFO_CACHE_SIZE_PLIST)
#define SYSINFO_AGINGEI_OFFSET              (SYSINFO_CACHE_SIZE_MAIN + SYSINFO_CACHE_SIZE_PLIST + SYSINFO_CACHE_SIZE_FTLRMP)

// Header Setting
#define SYSINFO_SIZE_MAINHEADER             64      //Byte
#define SYSINFO_SIZE_SUBHEADER              64      //Byte

// NOR Operation
#if SYSINFO_MEDIA_OPERATION_NOR == TRUE
    #define SYSINFO_NOR_PROG_RETRYCNT           3
    #define SYSINFO_NOR_TBL_CNT                 (SYSINFO_SUBHEADER_CNT_NOR + 1)

    #define SYSINFO_NOR_CRC32_SIZE              4
    #define SYSINFO_NOR_CRCT_CNT                (SYSINFO_NOR_TBL_CNT + 1)
    #define SYSINFO_NOR_CRCTBL_SIZE             (SYSINFO_NOR_CRC32_SIZE * SYSINFO_NOR_CRCT_CNT)

    #define SYSINFO_SUBHEADER_CNT_NOR           4
#endif // NOR Operation

// NAND Operation
#if SYSINFO_MEDIA_OPERATION_NAND == TRUE
    #define SYSINFO_IN_NAND_BACKUP              TRUE
    #define SYSINFO_NAND_SBLK_BEGIN             0
    #define SYSINFO_NAND_SBLK_COUNT             1
    #define SYSINFO_NAND_SBLK_FINAL             (SYSINFO_NAND_SBLK_BEGIN + SYSINFO_NAND_SBLK_COUNT)
    #define SYSINFO_NAND_PAGE_SIZE              (gNfInfo.dataBytesPerPage)
    #define SYSINFO_NAND_DYNAMIC_PAGE_CNT       (SYSINFO_NAND_PAGECNT_STEP * ((SYSINFO_CACHE_SIZE_MAIN / SYSINFO_NAND_PAGE_SIZE) / gSysInfoMgr.bankInfo.d.diePerBank))
    #define SYSINFO_NAND_PAGE_CNT               gNfInfo.pagesPerBlock
    #define SYSINFO_NAND_MSG_CNT                2

    #define SYSINFO_NAND_BANK_COUNT_MIN         4
    #define SYSINFO_NAND_BANK_COUNT_MAX         32
    #define SYSINFO_NAND_BANK_BAD               TRUE
    #define SYSINFO_NAND_BANK_START             0

    #define SYSINFO_NAND_OPERATION_WRITE        1
    #define SYSINFO_NAND_OPERATION_READ         2
    #define SYSINFO_NAND_OPERATION_ERASE        3

    #define SYSINFO_NAND_AREA_MAIN              B_0
    #define SYSINFO_NAND_AREA_PLIST             B_1
    #define SYSINFO_NAND_AREA_FTL               B_2
    #define SYSINFO_NAND_AREA_AGINGEI           B_3

    #if  SI_MS_NAND_QUICKSEARCH == TRUE
        #define SI_MS_NAND_QS_TAG                   0x48534B51  // 'QKSH'
        #define SI_MS_NAND_QS_SIZE_DATA             8           // Byte
        #define SI_MS_NAND_QS_SIZE_SFLASH           (SFH_QUICK_IDX_COUNT * SFLASH_SECTOR_SIZE)
    #endif
#endif

#define SI_FREE_SIZE                        (SYSINFO_CACHE_SIZE_MAIN - SI_PAYLOAD_MAIN_CUR_SIZE)  //256k - 132k = 124k

//SI Sync Check
#define SYSINFO_CHECK_PASS                  0x50
#define SYSINFO_CHECK_FAIL                  0x46

//SI Sync resource
#define MAX_SSD_PER_SYSINFO                 (SYSINFO_NAND_AREA_COUNT * 2 * 3) // SYSINFO_NAND_AREA_COUNT * (normal+ Backup) * AreaSplitCnt
#define MAX_JOB_PER_SYSINFO                 (SYSINFO_NAND_AREA_COUNT * 2 * 3) // SYSINFO_NAND_AREA_COUNT * (normal+ Backup) * AreaSplitCnt

//GList
#define GLIST_DEFECT_ERASE                  0x45 // 'E' for erase
#define GLIST_DEFECT_WRITE                  0x57 // 'W' for write
#define GLIST_DEFECT_READ                   0x52 // 'R' for read
#define SI_NAND_INFO_TAG                    0x4754464E  // 'NFTG'

#define SYSINFO_SUBHEADER_MAIN_AREA_CNT     12

#if CHK_FW_TYPE(FW_AGING)
    #define SYSINFO_NAND_PAGE_START             (((SYSINFO_CACHE_SIZE_PLIST + SYSINFO_CACHE_SIZE_FTLRMP + SYSINFO_CACHE_SIZE_AGINGEI) / SYSINFO_NAND_PAGE_SIZE) / gSysInfoMgr.bankInfo.d.diePerBank)
    #define SYSINFO_NAND_PAGE_START_SCE         (((SYSINFO_CACHE_SIZE_PLIST + SI_PAYLOAD_SIZE_FTLRMP_SCE + SYSINFO_CACHE_SIZE_AGINGEI) / SYSINFO_NAND_PAGE_SIZE) / gSysInfoMgr.bankInfo.d.diePerBank)
    #define SYSINFO_SUBHEADER_CNT_NAND          16
    #define SYSINFO_NAND_AREA_COUNT             4
    #define SYSINFO_CACHE_SIZE                  (SYSINFO_CACHE_SIZE_MAIN + SYSINFO_CACHE_SIZE_PLIST + SYSINFO_CACHE_SIZE_FTLRMP + SYSINFO_CACHE_SIZE_AGINGEI)
#else
    #define SYSINFO_NAND_PAGE_START             (((SYSINFO_CACHE_SIZE_PLIST + SYSINFO_CACHE_SIZE_FTLRMP) / SYSINFO_NAND_PAGE_SIZE) / gSysInfoMgr.bankInfo.d.diePerBank)
    #define SYSINFO_NAND_PAGE_START_SCE         (((SYSINFO_CACHE_SIZE_PLIST + SI_PAYLOAD_SIZE_FTLRMP_SCE) / SYSINFO_NAND_PAGE_SIZE) / gSysInfoMgr.bankInfo.d.diePerBank)
    #define SYSINFO_SUBHEADER_CNT_NAND          15
    #define SYSINFO_NAND_AREA_COUNT             3
    #define SYSINFO_CACHE_SIZE                  (SYSINFO_CACHE_SIZE_MAIN + SYSINFO_CACHE_SIZE_PLIST + SYSINFO_CACHE_SIZE_FTLRMP)
#endif

//-----------------------------------------------------------------------------
//  Private data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct
{
    U32 dwCRCErrBitMap;
    U32 dwCRCBuffer[SYSINFO_NOR_CRCT_CNT];
} tSI_CRC;

typedef union {
    U32 all;
    struct {
        U16      diePerBank;        //     2B
        U16      bankTotalCnt;      //     2B
    }d;
} tSI_BANK_INFO;

typedef struct {
        U8      bChS;        //     B
        U8      bCe;
        U8      bLun;
        U8      bNext;
} tSI_BANK;

#if _CPU_ == CPU_ID2
typedef struct {
    tSI_CRC         CRC;
    U64             qwHistory;          //  8B
    tSI_BANK_INFO   bankInfo;
    tSI_BANK        bankEntry[SYSINFO_NAND_BANK_COUNT_MAX];
    tPAA            QSPAACurrent[10];   // 40B
    tPAA            QSPAANext;          //  4B
    volatile U32    eraseBitMap;        //  4B
    U32             mgrBitMap;          //  4B
    U16             wQuickIdx;          //  2B
    U8              wQuickIdxSecotr;    //  1B
    U8              bErrBmp;            //  NAND AREA not total area
    U8              bBadBank[SI_NAND_MAX_CE * SI_NAND_MAX_LUN * SI_NAND_MAX_CHANNEL];  //  256B = 8B * 4 * 8
} tSI_MGR_INFO;
#endif

#if  SI_MS_NAND_QUICKSEARCH == TRUE
typedef struct {
    tPAA    QSCurrentPAA;   //  4B
    U32     dwTag;          //  4B
} tSI_MS_NAND_QS_IDX;

typedef struct {
    U64     qwHistory;      //  8B
    tPAA    QSCurrentPAA;   //  4B
    U32     dwTag;          //  4B
} tSI_MS_NAND_QS_HISTORY;
#endif

typedef struct {
    U32     dwPaaCnt;
    U32     dwOffset;
} tSI_NAND_TBL;

/// @brief SysInfo sub-header DWord4
typedef union {
    U32 all;
    struct {
        U8      bCH;    //     1B
        U8      bCE;    //     1B
        U8      bLUN;   //     1B
        U8      bCnt;   //     1B
    }d;
} tSI_SUBHEADER_DW4;


/// @brief SysInfo sub-header detail
typedef union {
    U8                      all[SYSINFO_SIZE_SUBHEADER];
    struct {
        U32                 dwLabel;          //     4B
        U32                 dwVerNo;        //     4B
        U32                 dwDataSize;     //     4B
        U32                 dwDataOffset;   //     4B
        tSI_SUBHEADER_DW4   dwDWord4;       //     4B
        U32                 dwDWord[10];    //    40B
        U32                 dwValidTag;     //     4B
    }d;
} tSI_SUBHEADER_D;

/// @brief SysInfo sub-header
typedef union {
    tSI_SUBHEADER_D        All[SYSINFO_SUBHEADER_CNT_NAND];
    struct {
        tSI_SUBHEADER_D    Misc;
        tSI_SUBHEADER_D    PlistCnt;
        tSI_SUBHEADER_D    Tcg;
        tSI_SUBHEADER_D    MPInfo;
        tSI_SUBHEADER_D    NVMe;
        tSI_SUBHEADER_D    NVMeLog;
        tSI_SUBHEADER_D    Aging;
        tSI_SUBHEADER_D    GList;
        tSI_SUBHEADER_D    UncTable;
        tSI_SUBHEADER_D    NVEC;
        tSI_SUBHEADER_D    FTL;
        tSI_SUBHEADER_D    ECnt;
        tSI_SUBHEADER_D    PList1;
        tSI_SUBHEADER_D    PList2;
        tSI_SUBHEADER_D    FTLRmp;
        #if CHK_FW_TYPE(FW_AGING)
        tSI_SUBHEADER_D    AgingEI;
        #endif
    } d;
} tSI_SUBHEADER;

/// @brief SysInfo main header
typedef union {
    U8          all[SYSINFO_SIZE_MAINHEADER];
    struct {
        U32     dwTag;          //     4B
        U32     dwVerNo;        //     4B
        U16     wSize;          //     2B sizeof(tSI_MAINHEADER)
        U16     wSubHeaderSize; //     2B sizeof(tSI_SUBHEADER)
        U16     wSubHeaderCnt;  //     2B
        U16     wReserved;      //     2B
        U32     dwAsicId;       //     4B
    } d;
} tSI_MAINHEADER;

/// @brief SysInfo header payload
typedef union {
    U8  all[SI_PAYLOAD_SIZE_HEADER];
    struct {
        tSI_MAINHEADER         MainHeader;
        tSI_SUBHEADER          SubHeader;
    } d;
} tSI_PAYLOAD_HEADER;

typedef struct
{
    U16 wStartSector;         ///<
    U16 wSectorCnt;           ///<
    U32 dwOPTag;              ///<
    U32 dwMenAddr;            ///<
    U32 dwCRCOffset;          ///<
} tSI_NOR_TBL;
COMPILE_ASSERT((sizeof(tSI_NOR_TBL) & 0x3) == 0, "please align to 4 bytes!");

typedef struct
{
    U32 wSectorSize;          ///<
    U32 dwUpdateArea;         ///<
    U32 dwOPTag;              ///<
    U32 dwVerNo;              ///<
} tSI_NAND_TBL_TAG;
COMPILE_ASSERT((sizeof(tSI_NAND_TBL_TAG) & 0x3) == 0, "please align to 4 bytes!");
//-----------------------------------------------------------------------------
//  Public data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief SysInfo direction
typedef enum {
    SYSINFO_READ                = 0x001,
    SYSINFO_WRITE,
    SYSINFO_WRITE_FORCE,
    SYSINFO_DIR_NULL            = 0x7FFFFFFF
} tSI_DIR;

/// @brief SysInfo error
typedef enum {
    SYSINFO_ERR_NON = 0,
    SYSINFO_ERR_ERASE,
    SYSINFO_ERR_WRITE,
    SYSINFO_ERR_READ,
    SYSINFO_ERR_NOTEXIST,
    SYSINFO_ERR_UNKNOWN,
    SYSINFO_ERR_NONRESOURCE,
    SYSINFO_ERR_OWRISK,
    SYSINFO_ERR_NULL            = 0x7FFFFFFF
} tSI_ERR;

/// @brief SysInfo synchronize source
typedef enum {
    SI_SYNC_BY_SYSINFO          = 0,
    SI_SYNC_BY_NVME,
    SI_SYNC_BY_NVMELOG,
    SI_SYNC_BY_AGING,
    SI_SYNC_BY_FTL,
    SI_SYNC_BY_SCANDEFECT,
    SI_SYNC_BY_UNCT,
    SI_SYNC_BY_ECNT,
    SI_SYNC_BY_GLIST,
    SI_SYNC_BY_TCG,
    SI_SYNC_BY_UART             = 0x10,
    SI_SYNC_BY_NOR              = 0x33
} tSI_ERR_SOURCE;

/// @brief SysInfo payload

typedef union {
    U8 all[SYSINFO_CACHE_SIZE];
    struct {
        //==========================Header====================================
        tSI_PAYLOAD_HEADER          Header;                 //  4k
        tSI_PAYLOAD_MISC            MiscData;               //  4k
        tSI_PAYLOAD_PLSTCNT         PlistCnt;               //  4k
        tSI_PAYLOAD_TCG             TcgData;                //  4k
        tSI_PAYLOAD_MPINFO          MPInfo;                 //  4k
        tSI_PAYLOAD_NVME            NVMeData;               //  8k
        tSI_PAYLOAD_LOGINFO         LogInfo;                //  24k
        tSI_PAYLOAD_AGING           AgingData;              //  16k
        tSI_PAYLOAD_GLIST           GList;                  //  8k
        tSI_PAYLOAD_UNCT            Unct;                   //  8k
        tSI_PAYLOAD_NVEC            NVEC;                   //  4k
        tSI_PAYLOAD_FTL             FtlData;                //  4K
        tSI_PAYLOAD_ECNT            ECnt;                   //  32k
        U8                          Reserved[SI_FREE_SIZE]; //
        //==========================PList=====================================
        tSI_PAYLOAD_PLIST           PList1;                 //  64k->256k
        tSI_PAYLOAD_PLIST           PList2;                 //  64k->256k
        //==========================FTLData===================================
        tSI_PAYLOAD_FTL_RMPBLK      FtlRmpData;             //  2.5 M
        //==========================AGFWData==================================
        #if CHK_FW_TYPE(FW_AGING)
        tSI_PAYLOAD_AGERRINFO       AgingEI;            	// 4MB + 256 KB

        #endif
    } d;
} tSI_PAYLOAD;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern tSI_PAYLOAD*           smSysInfo;
#if (_CPU_ == CPU_ID2)
extern tSI_MGR_INFO           gSysInfoMgr;
#endif
//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t  SyIn_OneTimeInit(InitBootMode_t initMode);
U8       SyIn_Synchronize(U32 dwOperateArea, U8 bDir ,U8 bSyncBy);
#if (_CPU_ == CPU_ID2)
void     SyIn_NAND_Erase(U16 bankIdx);
U16      SyIn_NAND_BankIdx(tPAA paa);
U16      SyIn_NAND_HealthBank(U16 bankIdx);
U8       SyIn_NAND_Synchronize(U32 dwOperateArea, U8 bDir, MsgSI_t* pHcmdMsg);
U8       SyIn_NAND_ForceChangeBank(void);
U8       SyIn_NOR_Synchronize(U8 bDir);
U8       SyIn_Purge(U32 dwOperateArea, U8 dir);
#endif

#if  SI_MS_NAND_QUICKSEARCH == TRUE
void     SyIn_NAND_QuickStore(tPAA QuickIdx);
#endif

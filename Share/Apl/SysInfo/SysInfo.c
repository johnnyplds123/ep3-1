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
//  This is SysInfo.c
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "Fio.h"
#include "Cache.h"

#if (_CPU_ == CPU_ID0)
    #include "HostBase.h"
#endif

#if (_CPU_ == CPU_ID2)
    #include "Core.h"
    #include "Ftl.h"
    #include "CoreMsg.h"
    #include "CoreDbg.h"
#endif

#include "SysInfo.h"

#include "NormSection.h"

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

static void SyIn_MPInit(void);//MP Data Init Function
static void SyIn_MEDIA_Swap(void);
static void SyIn_Verification(void);
static void SyIn_SubHeaderBuild(void);
static void SyIn_AreaDiffDetect(void);
static void SyIn_InitFromScratch(void);
static U8   SyIn_VersionCheck(void);

//NOR
static void SyIn_NOR_Write(void);
static void SyIn_NOR_Table_Init(void);
static U8   SyIn_NOR_Read(void);
static U8   SyIn_NOR_CRC_Read(void);

//NAND
static void SyIn_NAND_BankInit(void);
static void SyIn_NAND_BodySeeker(void);
static void SyIn_NAND_QuickErase(void);
static void SyIn_NAND_EraseBuildTable(void);
static void SyIn_NAND_DetectOverWriteRisk(void);
static void SyIn_NAND_Management(S8 areaIdx, U8 bBackUpCnt, U8 bSwitchBank, U8 op, U32 sizeSplit, U16 mute);
static U32  SyIn_NAND_PageCheck(U32 dwOperateArea_nand);
static U16  SyIn_NAND_QuickSearchExhaust(U16 Idx);
static U8   SyIn_NAND_HeaderSeeker(void);
static U8   SyIn_NAND_QuickSearch(void);
static U8   SyIn_NAND_HistorySearch(void);
static U8   SyIn_NAND_Write(U32 dwOperateArea_nand, MsgSI_t* pHcmdMsg);
static U8   SyIn_NAND_Read(U32 dwOperateArea_nand, MsgSI_t* pHcmdMsg, U16 errHandle);
//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
__DATA_PERF_SH __align(32) tSI_PAYLOAD*    smSysInfo;

#if (_CPU_ == CPU_ID2)   // CPU2
static tSI_NOR_TBL gSysInfoMgrTable[] =
{   //NOR Start Scector                                 Operation Tag
    { SFH_HEADER_START,         SFH_HEADER_COUNT,       SI_TAG_SYSINFO,  },
    { SFH_PLTCNT_START,         SFH_PLTCNT_COUNT,       SI_TAG_PLSTCNT,  },
    { SFH_AGING_START,          SFH_AGING_COUNT,        SI_TAG_AGING,    },
    { SFH_PLIST1_START,         SFH_PLIST1_COUNT,       SI_TAG_PLIST1,   },
    { SFH_PLIST2_START,         SFH_PLIST2_COUNT,       SI_TAG_PLIST2,   },
};

static tSI_NAND_TBL_TAG gSysInfoMgrNANDTable[] =
{   //                          Operation Area      Operation Tag
    { SI_PAYLOAD_SIZE_MISC,            SI_AREA_MISC,       SI_TAG_MISC,     SI_VER_MISC,    },
    { SI_PAYLOAD_SIZE_PLSTCNT,         SI_AREA_PLSTCNT,    SI_TAG_PLSTCNT,  SI_VER_PLSTCNT, },
    { SI_PAYLOAD_SIZE_TCG,             SI_AREA_TCG,        SI_TAG_TCG,      SI_VER_TCG,     },
    { SI_PAYLOAD_SIZE_MPINFO,          SI_AREA_MPINFO,     SI_TAG_MPINFO,   SI_VER_MPINFO,  },
    { SI_PAYLOAD_SIZE_NVME,            SI_AREA_NVME,       SI_TAG_NVME,     SI_VER_NVME,    },
    { SI_PAYLOAD_SIZE_LOGINFO,         SI_AREA_SMART,      SI_TAG_LOGINFO,  SI_VER_LOGINFO, },
    { SI_PAYLOAD_SIZE_AGING,           SI_AREA_AGING,      SI_TAG_AGING,    SI_VER_AGING,   },
    { SI_PAYLOAD_SIZE_GLIST,           SI_AREA_GLIST,      SI_TAG_GLIST,    SI_VER_GLIST,   },
    { SI_PAYLOAD_SIZE_UNCT,            SI_AREA_UNCT,       SI_TAG_UNCTABLE, SI_VER_UNCTABLE,},
    { SI_PAYLOAD_SIZE_NVEC,            SI_AREA_NVEC,       SI_TAG_NVEC,     SI_VER_NVEC,    },
    { SI_PAYLOAD_SIZE_FTL,             SI_AREA_FTL,        SI_TAG_FTL,      SI_VER_FTL,     },
    { SI_PAYLOAD_SIZE_ECNT,            SI_AREA_ECNT,       SI_TAG_ECNT,     SI_VER_ECNT,    },
    { SI_PAYLOAD_SIZE_PLIST1,          SI_AREA_PLIST1,     SI_TAG_PLIST1,   SI_VER_PLIST,   },
    { SI_PAYLOAD_SIZE_PLIST2,          SI_AREA_PLIST2,     SI_TAG_PLIST2,   SI_VER_PLIST2,  },
    { SI_PAYLOAD_SIZE_FTLRMP,          SI_AREA_FTLRMP,     SI_TAG_FTLRMP,   SI_VER_FTLRMP,  },
#if CHK_FW_TYPE(FW_AGING)
    { SI_PAYLOAD_SIZE_AGINGEI,         SI_AREA_AGINGEI,    SI_TAG_AGINGEI,  SI_VER_AGINGEI, },
#endif
};

static tSI_NAND_TBL gSysInfoNandTable[] =
{
    { SYSINFO_CACHE_SIZE_MAIN      / MBU_SIZE, 0,                       },
    { SYSINFO_CACHE_SIZE_PLIST     / MBU_SIZE, SYSINFO_PLIST_OFFSET,    },
    { SYSINFO_CACHE_SIZE_FTLRMP    / MBU_SIZE, SYSINFO_FTLRMP_OFFSET,   },
    { SYSINFO_CACHE_SIZE_AGINGEI   / MBU_SIZE, SYSINFO_AGINGEI_OFFSET,  },
};

tSI_MGR_INFO         gSysInfoMgr;
#endif

//-----------------------------------------------------------------------------
//  Imported variable
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern void Nvme_WaitMediaDone(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-------------------------------------------------------------------
// Function     : Error_t SyIn_OneTimeInit(InitBootMode_t initMode)
// Description  : Initial SysInfo
// Input        : N/A
// return       : Error_t
//-------------------------------------------------------------------
Error_t SyIn_OneTimeInit(InitBootMode_t initMode)
{
#if (_MEDIA_==MEDIA_NAND) && (_CPU_ == CPU_ID2)

    smSysInfo = (tSI_PAYLOAD*)MEM_AllocBuffer(SYSINFO_CACHE_SIZE, 4096);
    SI_PRINTF_NECESSARY("[SI  ] Addr|%x\n", smSysInfo);

    MEM_CLR(smSysInfo, SYSINFO_CACHE_SIZE);
    MEM_CLR(&gSysInfoMgr, sizeof(tSI_MGR_INFO));

    SyIn_SubHeaderBuild();
    SyIn_NOR_Table_Init();
    SyIn_NAND_BankInit();

    // InitFromScratch
    if (SyIn_NAND_HeaderSeeker() == SYSINFO_ERR_NOTEXIST)
    {
        SyIn_InitFromScratch();
        return cEcNoError;
    }

    SyIn_NAND_BodySeeker();
//---------------------------------Data Init Finfsh---------------------------
    gSysInfoMgr.eraseBitMap = 0;

    SyIn_NAND_Erase(SI_GET_HEALTH_BY_CURRPAA);
    Core_JobWait();
    if (gSysInfoMgr.mgrBitMap & SI_S_OVERWRITE_RISK)
    {
        SyIn_NAND_ForceChangeBank();
        SyIn_Synchronize(SI_AREA_PLIST1 | SI_AREA_FTLRMP | SI_AREA_AGINGEI, SYSINFO_WRITE_FORCE, NULL);
    }
//---------------------------------Algorithm Init Finish----------------------
    SyIn_AreaDiffDetect();
    SyIn_VersionCheck();
    SyIn_MPInit();//MP Check
    if (gSysInfoMgr.mgrBitMap & SI_S_FOUND_OLD_DATA) smSysInfo->d.FtlData.d.QBTBlockTag = 0;
//-------------------------------Data Double Check Finish----------------------
#elif (_MEDIA_==MEDIA_DRAM) && (_CPU_ == CPU_ID0)
    smSysInfo = (tSI_PAYLOAD*)MEM_AllocBuffer(SYSINFO_CACHE_SIZE, 4096);
    MEM_CLR(smSysInfo, SYSINFO_CACHE_SIZE);
#endif
    return cEcNoError;
}

#if (_CPU_ == CPU_ID2)
//-------------------------------------------------------------------
// Function     : Clear Payload & ValidTag
//-------------------------------------------------------------------
U8 SyIn_Purge(U32 dwOperateArea, U8 dir)
{
    #if (_CPU_ == CPU_ID2)
    U16 area;
    for (area = 0; area < SYSINFO_SUBHEADER_CNT_NAND; area++)
    {
        if (dwOperateArea & BIT(area))
        {
            MEM_CLR((U32 *)(((U32)smSysInfo) + smSysInfo->d.Header.d.SubHeader.All[area].d.dwDataOffset), smSysInfo->d.Header.d.SubHeader.All[area].d.dwDataSize);
            smSysInfo->d.Header.d.SubHeader.All[area].d.dwValidTag = 0;
        }
    }

    if (dwOperateArea & BIT(17)) MEM_CLR(&smSysInfo->d.LogInfo.d.ErrLog, sizeof(tERR_LOG));
    if (dwOperateArea & BIT(18)) MEM_CLR(&smSysInfo->d.LogInfo.d.SmrLog, sizeof(tSMR_LOG));

    SyIn_Synchronize((dwOperateArea | SI_AREA_INIT), dir, NULL);
    #endif

    return SYSINFO_ERR_NON;
}
//-------------------------------------------------------------------
// Function     : static void SYSINFO_Verification(void)
// Description  : Sysinfo Verification
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void SyIn_Verification(void)
{
    if (smSysInfo->d.Header.d.MainHeader.d.dwTag == SI_TAG_SYSINFO)
    {
        BIT_CLR(gSysInfoMgr.mgrBitMap, SI_S_INIT_FROM_SCRATCH);
    }
    else
    {
        BIT_SET(gSysInfoMgr.mgrBitMap, SI_S_INIT_FROM_SCRATCH);
    }
}

//-------------------------------------------------------------------
// Function     : static void SyIn_InitFromScratch(void)
// Description  : Sysinfo Init Scratch Data(Main Header)
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void SyIn_InitFromScratch(void)
{
    SI_PRINTF_NECESSARY("[SI  ] IFS\n");

    MEM_CLR(smSysInfo, SYSINFO_CACHE_SIZE);
    //Header
    smSysInfo->d.Header.d.MainHeader.d.dwTag            = SI_TAG_SYSINFO;
    smSysInfo->d.Header.d.MainHeader.d.dwVerNo          = SI_VER_SYSINFO;
    smSysInfo->d.Header.d.MainHeader.d.wSize            = sizeof(tSI_MAINHEADER);
    smSysInfo->d.Header.d.MainHeader.d.wSubHeaderSize   = sizeof(tSI_SUBHEADER_D);
    smSysInfo->d.Header.d.MainHeader.d.wSubHeaderCnt    = SYSINFO_SUBHEADER_CNT_NAND;
    smSysInfo->d.Header.d.MainHeader.d.dwAsicId         = PCIE_NVME_ELDORA;

    SyIn_SubHeaderBuild();
    SyIn_MPInit();
    //backup NfInfo
    smSysInfo->d.Header.d.SubHeader.d.Misc.d.dwValidTag = SI_TAG_MISC;
    smSysInfo->d.MiscData.d.NandInfo.d.NFInfoTag        = SI_NAND_INFO_TAG;
    MEM_COPY(&smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo, &gNfInfo, sizeof(NfInfo_t));

    SyIn_NAND_EraseBuildTable();
}

//-------------------------------------------------------------------
// Function     : static void SyIn_InitFromScratch(void)
// Description  : Sysinfo Init Scratch Data(MPInfo)
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void SyIn_MPInit(void)
{
    if (smSysInfo->d.Header.d.SubHeader.d.MPInfo.d.dwValidTag != SI_TAG_MPINFO)
    {
        smSysInfo->d.Header.d.SubHeader.d.MPInfo.d.dwValidTag = SI_TAG_MPINFO;
        MEM_CLR(&smSysInfo->d.MPInfo, sizeof(tSI_PAYLOAD_MPINFO));

        MEM_SET(&smSysInfo->d.MPInfo.d.StationCode[0], BYTE_MASK, sizeof(tMP_STATION_CODE));
        MEM_SET(&smSysInfo->d.MPInfo.d.StationCode[1], BYTE_MASK, sizeof(tMP_STATION_CODE));

        smSysInfo->d.MPInfo.d.StationCode[0].StationSignature = STATION_SIGNATURE_TEST1;
        smSysInfo->d.MPInfo.d.StationCode[1].StationSignature = STATION_SIGNATURE_TEST2;
        smSysInfo->d.MPInfo.d.StationSignature                = STATION_SIGNATURE_AVER;
        smSysInfo->d.MPInfo.d.AgingSignature = SI_TAG_AGING;
        if (smSysInfo->d.Header.d.SubHeader.d.Aging.d.dwValidTag == SI_TAG_AGING)
        {
            MEM_COPY(smSysInfo->d.MPInfo.d.SerialNumberDrive, smSysInfo->d.AgingData.d.Aging_SN, sizeof(U8) * 16);
            MEM_COPY(smSysInfo->d.MPInfo.d.SerialNumberPCBA, smSysInfo->d.AgingData.d.Aging_SN, sizeof(U8) * 16);
        }
    }
}

//-------------------------------------------------------------------
// Function     : static void SYSINFO_HeaderBuild(void)
// Description  : Fill DataSize & DataOffset to SubHeader
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void SyIn_SubHeaderBuild(void)
{
    U32 i;

    smSysInfo->d.Header.d.SubHeader.All[0].d.dwDataSize   = gSysInfoMgrNANDTable[0].wSectorSize;
    smSysInfo->d.Header.d.SubHeader.All[0].d.dwDataOffset = SI_PAYLOAD_SIZE_HEADER;

    for (i = 1; i < SYSINFO_SUBHEADER_CNT_NAND; i++)
    {
        smSysInfo->d.Header.d.SubHeader.All[i].d.dwDataSize   = gSysInfoMgrNANDTable[i].wSectorSize;
        smSysInfo->d.Header.d.SubHeader.All[i].d.dwDataOffset = smSysInfo->d.Header.d.SubHeader.All[i-1].d.dwDataOffset
                                                              + smSysInfo->d.Header.d.SubHeader.All[i-1].d.dwDataSize;
        if (i == SYSINFO_SUBHEADER_MAIN_AREA_CNT)
        {
            smSysInfo->d.Header.d.SubHeader.All[i].d.dwDataOffset += SI_FREE_SIZE;
        }
    }

    // Build SubHeader Tag
    for (i = 0; i < SYSINFO_SUBHEADER_CNT_NAND; i++)
    {
        smSysInfo->d.Header.d.SubHeader.All[i].d.dwLabel = gSysInfoMgrNANDTable[i].dwOPTag;
        smSysInfo->d.Header.d.SubHeader.All[i].d.dwVerNo = gSysInfoMgrNANDTable[i].dwVerNo;
    }
}

//-------------------------------------------------------------------
// Function     : U8 SyIn_Synchronize(U32 dwOperateArea, U8 bDir, U8 bSyncBy)
// Description  : Write or Read SysInfo
// Input        : OperateArea, Dir
// return       : error or not
//-------------------------------------------------------------------
U8 SyIn_SyncCheck(U32 dwOperateArea, U8 bDir)
{
    tSI_NAND_TBL_TAG* pTable;
    U8                i;

    pTable = gSysInfoMgrNANDTable;
    if (dwOperateArea & SI_AREA_INIT) return SYSINFO_CHECK_PASS;
    if (bDir == SYSINFO_WRITE)
    {
        for (i = 0; i < SYSINFO_SUBHEADER_CNT_NAND; i++)
        {
            if (BIT(i) & dwOperateArea)
            {
                if (smSysInfo->d.Header.d.SubHeader.All[i].d.dwValidTag != pTable[i].dwOPTag)
                {
                    return SYSINFO_CHECK_FAIL;
                }
            }
        }
    }
    return SYSINFO_CHECK_PASS;
}
//-------------------------------------------------------------------
// Function     : static void SyIn_AreaDiffDetect(void)
// Description  : Write or Read SysInfo
// Input        : OperateArea, Dir
// return       : error or not
//-------------------------------------------------------------------
static void SyIn_AreaDiffDetect(void)
{
    U32 dwSubHeader;
    U8  area = 0;

    if (smSysInfo->d.Header.d.MainHeader.d.wSubHeaderCnt > SYSINFO_SUBHEADER_CNT_NAND)    //For Aging->Normal
    {
        smSysInfo->d.Header.d.MainHeader.d.wSubHeaderCnt = SYSINFO_SUBHEADER_CNT_NAND;
        SyIn_Synchronize(SI_AREA_MISC, SYSINFO_WRITE_FORCE, NULL);
    }
    else if (smSysInfo->d.Header.d.MainHeader.d.wSubHeaderCnt < SYSINFO_SUBHEADER_CNT_NAND)
    {
        for (dwSubHeader = smSysInfo->d.Header.d.MainHeader.d.wSubHeaderCnt; dwSubHeader < SYSINFO_SUBHEADER_CNT_NAND; dwSubHeader++)
        {
            MEM_CLR(&smSysInfo->d.Header.d.SubHeader.All[dwSubHeader], sizeof(tSI_SUBHEADER_D));
            smSysInfo->d.Header.d.SubHeader.All[dwSubHeader].d.dwLabel      = gSysInfoMgrNANDTable[dwSubHeader].dwOPTag;
            smSysInfo->d.Header.d.SubHeader.All[dwSubHeader].d.dwVerNo      = gSysInfoMgrNANDTable[dwSubHeader].dwVerNo;
            smSysInfo->d.Header.d.SubHeader.All[dwSubHeader].d.dwValidTag   = 0;
            smSysInfo->d.Header.d.SubHeader.All[dwSubHeader].d.dwDataSize   = gSysInfoMgrNANDTable[dwSubHeader].wSectorSize;
            smSysInfo->d.Header.d.SubHeader.All[dwSubHeader].d.dwDataOffset = smSysInfo->d.Header.d.SubHeader.All[dwSubHeader - 1].d.dwDataOffset + smSysInfo->d.Header.d.SubHeader.All[dwSubHeader - 1].d.dwDataSize;
            MEM_CLR((U32 *)(((U32)smSysInfo) + smSysInfo->d.Header.d.SubHeader.All[dwSubHeader].d.dwDataOffset), smSysInfo->d.Header.d.SubHeader.All[dwSubHeader].d.dwDataSize);
            area |= BIT(dwSubHeader);
        }
        smSysInfo->d.Header.d.MainHeader.d.wSubHeaderCnt = SYSINFO_SUBHEADER_CNT_NAND;
        SyIn_Synchronize(area, SYSINFO_WRITE_FORCE, NULL);
    }
}

//-------------------------------------------------------------------
// Function     : U8 SyIn_VersionCheck(void)
// Description  : Write or Read SysInfo
// Input        : OperateArea, Dir
// return       : error or not
//-------------------------------------------------------------------
static U8 SyIn_VersionCheck(void)
{
    U32 dwSubHeader;
    U32 area = 0;
    U16 showLoop = 100;

    for (dwSubHeader = 0; dwSubHeader < SYSINFO_SUBHEADER_CNT_NAND; dwSubHeader++)
    {
        if (gSysInfoMgrNANDTable[dwSubHeader].dwVerNo != smSysInfo->d.Header.d.SubHeader.All[dwSubHeader].d.dwVerNo)
        {
            smSysInfo->d.Header.d.SubHeader.All[dwSubHeader].d.dwVerNo = gSysInfoMgrNANDTable[dwSubHeader].dwVerNo;
            area |= BIT(dwSubHeader);
        }
    }

    if (area)
    {
        SyIn_Purge(area, SYSINFO_WRITE_FORCE);
        while (showLoop)
        {
            D_PRINTF("[SI  ] DetectVersionDiff AREA|%x\n", area);
            showLoop--;
        }
    }
    return SYSINFO_ERR_NON;
}

#if SYSINFO_MEDIA_OPERATION_NOR == TRUE
//-------------------------------------------------------------------
// Function     : static void SYSINFO_NOR_CRC_Init(void)
// Description  : Initial struct gSysInfoCRC
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static U8 SyIn_NOR_CRC_Read(void)
{
    MEM_CLR(&gSysInfoMgr.CRC, sizeof(tSI_CRC));
    HalSflash_Copy((PVOID)(&gSysInfoMgr.CRC.dwCRCBuffer[0]), SFH_CRC_START_BYTE, (SYSINFO_NOR_TBL_CNT + 1) * SYSINFO_NOR_CRC32_SIZE);

    if (gSysInfoMgr.CRC.dwCRCBuffer[0] == SI_TAG_CRC) return true;

    return false;
}

//-------------------------------------------------------------------
// Function     : static void SYSINFO_NOR_Table_Init(void)
// Description  : Initial gSysInfoNorTable
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void SyIn_NOR_Table_Init(void)
{
    tSI_NOR_TBL* pTable;
    U16          backupIdx, subIdx;

    pTable = gSysInfoMgrTable;

    pTable[0].dwMenAddr = (U32)&smSysInfo->all[0];
    for (backupIdx = 1; backupIdx < SYSINFO_NOR_TBL_CNT; backupIdx++)
    {
        for (subIdx = 0; subIdx < SYSINFO_SUBHEADER_CNT_NAND; subIdx++)
        {
            if (pTable[backupIdx].dwOPTag == smSysInfo->d.Header.d.SubHeader.All[subIdx].d.dwLabel)
            {
                pTable[backupIdx].dwMenAddr = pTable[0].dwMenAddr + smSysInfo->d.Header.d.SubHeader.All[subIdx].d.dwDataOffset;
            }
        }
    }
}

//-------------------------------------------------------------------
// Function     : static U8 SYSINFO_NOR_Synchronize(U32 dwOperateArea, U8 bDir)
// Description  : Write or Read SysInfo to NOR
// Input        : OperateArea, Dir
// return       : error or not
//-------------------------------------------------------------------
U8 SyIn_NOR_Synchronize(U8 bDir)
{
    if (bDir == SYSINFO_WRITE || bDir == SYSINFO_WRITE_FORCE) SyIn_NOR_Write();    // Write SysInfo to NOR
    else if (bDir == SYSINFO_READ)                            SyIn_NOR_Read();     // Read SysInfo to NOR
    else                                                      ASSERT(0);

    return SYSINFO_ERR_NON;
}

//-------------------------------------------------------------------
// Function     : static void SYSINFO_NOR_Write(U32 dwOperateArea, U8 bBackUpStatus)
// Description  : Write SysInfo to NOR
// Input        : OperateArea, bBackUpStatus
// return       : N/A
//-------------------------------------------------------------------
static void SyIn_NOR_Write(void)
{
    tSI_NOR_TBL* pTable;
    U32          flashAddr, size;
    U16          secCnt;
    S8           i;
    U8           retry;

    pTable      = gSysInfoMgrTable;
    i           = SYSINFO_SUBHEADER_CNT_NOR;
    MEM_CLR(&gSysInfoMgr.CRC, sizeof(tSI_CRC));
    gSysInfoMgr.CRC.dwCRCBuffer[0] = SI_TAG_CRC;

    // Program Data at UpdateArea to NOR
    while (i >= 0)
    {
        //FillUp test Data//
        SI_PRINTF_NECESSARY("[SI W]NOR Area|%3d Mem|0x%8x\n", i, pTable[i].dwMenAddr);
        //CRC//
        gSysInfoMgr.CRC.dwCRCBuffer[i+1] = MemCrcU32(0xFFFFFFFF, (PVOID)pTable[i].dwMenAddr, pTable[i].wSectorCnt * SFLASH_SECTOR_SIZE);

        size      = pTable[i].wSectorCnt * SFLASH_SECTOR_SIZE;
        secCnt    = ALIGN_CEIL(pTable[i].wSectorCnt * SFLASH_SECTOR_SIZE, SFLASH_SECTOR_SIZE) / (SFLASH_SECTOR_SIZE);
        flashAddr = pTable[i].wStartSector * SFLASH_SECTOR_SIZE;
        retry     = SYSINFO_NOR_PROG_RETRYCNT;
        while (retry--)
        {
            if (HalSflash_EraseSector((U32)(pTable[i].wStartSector), (U32)secCnt, retry) == cEcNoError)
            {
                if (HalSflash_Program((PVOID)pTable[i].dwMenAddr, flashAddr, size) == cEcNoError)
                {
                    if (HalSflash_Compare((PVOID)pTable[i].dwMenAddr, flashAddr, size) == cEcNoError) break;
                }
            }
        }
        i--;
    }

    // Program CRC to NOR
    retry = SYSINFO_NOR_PROG_RETRYCNT;

    while (retry--)
    {
        if (HalSflash_EraseSector(SFH_CRC_START, SFH_CRC_COUNT, retry) == cEcNoError)
        {
            if (HalSflash_ProgramU32((PVOID)gSysInfoMgr.CRC.dwCRCBuffer, SFH_CRC_START_BYTE, SYSINFO_NOR_CRCTBL_SIZE) == cEcNoError)
            {
                if (HalSflash_Compare((PVOID)gSysInfoMgr.CRC.dwCRCBuffer, SFH_CRC_START_BYTE, SYSINFO_NOR_CRCTBL_SIZE) == cEcNoError)
                {
                    break;
                }
            }
        }
    }
}

//-------------------------------------------------------------------
// Function     : static U8 SyIn_NOR_Read(U32 dwOperateArea, U8 bBackUpStatus)
// Description  : Read SysInfo in NOR
// Input        : OperateArea, bBackUpStatus
// return       : N/A
//-------------------------------------------------------------------
static U8 SyIn_NOR_Read(void)
{
    tSI_NOR_TBL* pTable;
    U32          dwCRC;
    U8           i, naSub;

    i      = 0;
    pTable = gSysInfoMgrTable;

    if(SyIn_NOR_CRC_Read() == false) return SYSINFO_ERR_NOTEXIST;

    while (i <= SYSINFO_SUBHEADER_CNT_NOR)
    {
        HalSflash_Copy((void*)pTable[i].dwMenAddr, (pTable[i].wStartSector * SFLASH_SECTOR_SIZE), pTable[i].wSectorCnt * SFLASH_SECTOR_SIZE);
        dwCRC = MemCrcU32(0xFFFFFFFF, (PVOID)pTable[i].dwMenAddr, pTable[i].wSectorCnt * SFLASH_SECTOR_SIZE);

        if (dwCRC != gSysInfoMgr.CRC.dwCRCBuffer[i+1])
        {
            if (i == 0) return SYSINFO_ERR_NOTEXIST;
            else
            {
                for (naSub = 0; naSub < SYSINFO_SUBHEADER_CNT_NAND; naSub++)
                {
                    if (smSysInfo->d.Header.d.SubHeader.All[naSub].d.dwValidTag == pTable[i].dwOPTag)
                    {
                        smSysInfo->d.Header.d.SubHeader.All[naSub].d.dwValidTag = 0;
                    }
                }
            }
        }
        i++;
    }
    return SYSINFO_ERR_NON;
}
#endif

//-------------------------------------------------------------------
// Function     : static void SyIn_NAND_BankInit(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void SyIn_NAND_BankInit(void)
{
    U32 bankCap, flushSize;
    U16 bankIdx, areaIdx;
    U8  ch, ce, lun;

    bankIdx     = 0;
    bankCap     = 0;
    flushSize   = 0;
    //bank init
    if ((gSysInfoMgr.bankInfo.d.diePerBank = (gNfInfo.totalLun / SYSINFO_NAND_BANK_COUNT_MIN)) > gNfInfo.numChannels)
    {
        gSysInfoMgr.bankInfo.d.diePerBank = gNfInfo.numChannels;
    }
    gSysInfoMgr.bankInfo.d.bankTotalCnt = gNfInfo.totalLun / gSysInfoMgr.bankInfo.d.diePerBank;

    for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
    {
        for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
        {
            for (ch = 0; ch < gNfInfo.numChannels; ch += gSysInfoMgr.bankInfo.d.diePerBank)
            {
                gSysInfoMgr.bankEntry[bankIdx].bChS  = ch;
                gSysInfoMgr.bankEntry[bankIdx].bCe   = ce;
                gSysInfoMgr.bankEntry[bankIdx].bLun  = lun;
                gSysInfoMgr.bankEntry[bankIdx].bNext = (bankIdx + 1);
                bankIdx++;
            }
        }
    }
    gSysInfoMgr.bankEntry[bankIdx - 1].bNext = 0;

    if (bankIdx != gSysInfoMgr.bankInfo.d.bankTotalCnt) ASSERT(0);
    if (gSysInfoMgr.bankInfo.d.bankTotalCnt == 4) gSysInfoNandTable[2].dwPaaCnt = SI_PAYLOAD_SIZE_FTLRMP_SCE / MBU_SIZE;

    for(areaIdx = 0; areaIdx < SYSINFO_NAND_AREA_COUNT; areaIdx++)
    {
        flushSize += (gSysInfoNandTable[areaIdx].dwPaaCnt * MBU_SIZE);
    }
    bankCap = (gNfInfo.wlPerBlock - 2) * gNfInfo.dataBytesPerPage * gSysInfoMgr.bankInfo.d.diePerBank;
    if (bankCap <= flushSize) ASSERT(0)
}

//-------------------------------------------------------------------
// Function     : static U16 SyIn_NAND_BankIdx(tPAA paa)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
U16 SyIn_NAND_BankIdx(tPAA paa)
{
    U16 bankIdx;

    for(bankIdx = 0; bankIdx < gSysInfoMgr.bankInfo.d.diePerBank; bankIdx++)
    {
        if (gSysInfoMgr.bankEntry[bankIdx].bChS == paa.b.ch
         && gSysInfoMgr.bankEntry[bankIdx].bCe  == paa.b.ce
         && gSysInfoMgr.bankEntry[bankIdx].bLun == paa.b.lun) break;
    }
    return bankIdx;
}
//-------------------------------------------------------------------
// Function     : static void SyIn_NAND_HeaderSeeker(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static U8 SyIn_NAND_HeaderSeeker(void)
{
    U8 error;

#if SI_MS_NAND_QUICKSEARCH == TRUE
    error = SyIn_NAND_QuickSearch();
    if (error == SYSINFO_ERR_UNKNOWN)  error = SyIn_NAND_HistorySearch();
    if (error == SYSINFO_ERR_NOTEXIST) SyIn_MEDIA_Swap();
#else
    error = SyIn_NAND_HistorySearch();
#endif
    SyIn_Verification();
    if (gSysInfoMgr.mgrBitMap & SI_S_INIT_FROM_SCRATCH) error = SYSINFO_ERR_NOTEXIST;
    else                                                error = SYSINFO_ERR_NON;

    return error;
}

//-------------------------------------------------------------------
// Function     : static void SyIn_NAND_BodySeeker(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void SyIn_NAND_BodySeeker(void)
{
    U32 dwOperateArea = 0;
    U8  area;

    for (area = 0; area < SYSINFO_SUBHEADER_CNT_NAND; area++)
    {
        if (smSysInfo->d.Header.d.SubHeader.All[area].d.dwValidTag != 0)
        {
            dwOperateArea |= BIT(area);
        }
    }

    //MediaInfoBuild
    SyIn_NAND_Synchronize(dwOperateArea, SYSINFO_READ, NULL);

    //LoadBadBlockMgrTbl
    MEM_COPY(&gSysInfoMgr.bBadBank, &smSysInfo->d.MiscData.d.InfoMgr.d.SIBadBank, (SI_NAND_MAX_CE * SI_NAND_MAX_LUN *SI_NAND_MAX_CHANNEL));
    //Copy NandFlashInfo
    if (smSysInfo->d.MiscData.d.NandInfo.d.NFInfoTag != SI_NAND_INFO_TAG)
    {
        smSysInfo->d.MiscData.d.NandInfo.d.NFInfoTag = SI_NAND_INFO_TAG;
        MEM_COPY(&smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo, &gNfInfo, sizeof(NfInfo_t));
    }

}
//-------------------------------------------------------------------
// Function     : static U8 SyIn_NAND_ForceChangeBank(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
U8 SyIn_NAND_ForceChangeBank(void)
{
    U16 bankIdx;

    if (gSysInfoMgr.eraseBitMap == 0)// Check Resource
    {
        return SYSINFO_ERR_NONRESOURCE;
    }

    bankIdx = SI_GET_IDX_BY_PAA_NEXT(gSysInfoMgr.QSPAACurrent[0]);
    while (!(SI_CHK_ERASEBMP(bankIdx))) //Pick already erase bank
    {
        bankIdx = gSysInfoMgr.bankEntry[bankIdx].bNext;
    }

    SI_CLR_ERASEBMP(bankIdx);

    gSysInfoMgr.QSPAANext.all32 = 0;
    gSysInfoMgr.QSPAANext.b.ce  = gSysInfoMgr.bankEntry[bankIdx].bCe;
    gSysInfoMgr.QSPAANext.b.lun = gSysInfoMgr.bankEntry[bankIdx].bLun;
    gSysInfoMgr.QSPAANext.b.ch  = gSysInfoMgr.bankEntry[bankIdx].bChS;

    return SYSINFO_ERR_NON;
}

//-------------------------------------------------------------------
// Function     : static void SyIn_NAND_HealthBank(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
U16 SyIn_NAND_HealthBank(U16 bankIdx)
{
    U16 lastBankIdx;

    lastBankIdx = bankIdx;

    if (bankIdx) lastBankIdx -= 1;
    else         lastBankIdx  = (gSysInfoMgr.bankInfo.d.bankTotalCnt - 1);

    do //pick health block and pre-erase
    {
        bankIdx = gSysInfoMgr.bankEntry[bankIdx].bNext;
        if (bankIdx == lastBankIdx) ASSERT(0);
    } while(gSysInfoMgr.bBadBank[bankIdx] == TRUE);
    return bankIdx;
}
//-------------------------------------------------------------------
// Function     : static void SyIn_NAND_EraseBuildTable(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void SyIn_NAND_EraseBuildTable(void)
{
    U16 bankIdx, healthCnt, selectBank;

    gSysInfoMgr.eraseBitMap     = 0;
    gSysInfoMgr.QSPAANext.all32 = 0;
    healthCnt                   = 0;
    selectBank                  = WORD_MASK;

    Core_JobWait();

    MEM_CLR(&gSysInfoMgr.bBadBank,(SI_NAND_MAX_CE * SI_NAND_MAX_LUN * SI_NAND_MAX_CHANNEL));
    for (bankIdx = 0; bankIdx < gSysInfoMgr.bankInfo.d.bankTotalCnt; bankIdx++)
    {
        SyIn_NAND_Erase(bankIdx);
    }
    Core_JobWait();

    for (bankIdx = 0; bankIdx < gSysInfoMgr.bankInfo.d.bankTotalCnt; bankIdx++)
    {
        if (gSysInfoMgr.bBadBank[bankIdx] != TRUE)
        {
            if (selectBank == WORD_MASK) selectBank = bankIdx;
            healthCnt++;
        }
    }
    if (healthCnt == 0 || selectBank == WORD_MASK) ASSERT(0);

    MEM_COPY(&smSysInfo->d.MiscData.d.InfoMgr.d.SIBadBank, &gSysInfoMgr.bBadBank, (SI_NAND_MAX_CE * SI_NAND_MAX_LUN * SI_NAND_MAX_CHANNEL));
    smSysInfo->d.Header.d.SubHeader.d.Misc.d.dwValidTag = SI_TAG_MISC;

    gSysInfoMgr.QSPAANext.b.ce     = gSysInfoMgr.bankEntry[selectBank].bCe;
    gSysInfoMgr.QSPAANext.b.lun    = gSysInfoMgr.bankEntry[selectBank].bLun;
    gSysInfoMgr.QSPAANext.b.ch     = gSysInfoMgr.bankEntry[selectBank].bChS;
    SI_CLR_ERASEBMP(selectBank);

    //PAACurrent Init
    MEM_SET(gSysInfoMgr.QSPAACurrent, DWORD_MASK, sizeof(tPAA)*SYSINFO_NAND_AREA_COUNT);

    //SFlash QuickSearch
    #if SI_MS_NAND_QUICKSEARCH == TRUE
    HalSflash_EraseSector(SFH_QUICK_IDX_START, SFH_QUICK_IDX_COUNT, SYSINFO_NOR_PROG_RETRYCNT);
    gSysInfoMgr.wQuickIdxSecotr    = SFH_QUICK_IDX_COUNT-1;
    gSysInfoMgr.qwHistory          = 0;
    #endif

    smSysInfo->d.MiscData.d.InfoMgr.d.SIHistory_L = 1;

    SyIn_Synchronize(SI_AREA_PLIST1 | SI_AREA_FTLRMP | SI_AREA_AGINGEI | SI_AREA_INIT, SYSINFO_WRITE_FORCE, NULL);
}

#if SYSINFO_MEDIA_OPERATION_NAND == TRUE
#if SI_MS_NAND_QUICKSEARCH == TRUE
//-------------------------------------------------------------------
// Function     : static void SyIn_NAND_QuickErase(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void SyIn_NAND_QuickErase(void)
{
    Error_t error = cEcNoError;
    U8      retry = SYSINFO_NOR_PROG_RETRYCNT;

    while(retry > 0)
    {
        gSysInfoMgr.qwHistory++;
        // erase next block
        if (gSysInfoMgr.wQuickIdxSecotr == SFH_QUICK_IDX_COUNT - 1) gSysInfoMgr.wQuickIdxSecotr = 0;
        else                                                        gSysInfoMgr.wQuickIdxSecotr++;

        HalSflash_EraseSector(SFH_QUICK_IDX_START + gSysInfoMgr.wQuickIdxSecotr, 1, SYSINFO_NOR_PROG_RETRYCNT);
        error = HalSflash_ProgramU32((PVOID)&gSysInfoMgr.qwHistory, ((SFH_QUICK_IDX_START + gSysInfoMgr.wQuickIdxSecotr) * SFLASH_SECTOR_SIZE), sizeof(U64));
        if (error == cEcNoError)
        {
            gSysInfoMgr.wQuickIdx = SI_MS_NAND_QS_SIZE_DATA;
            break;
        }
        else
        {
            retry--;
        }
    }
}

//-------------------------------------------------------------------
// Function     : static U8 SyIn_NAND_QuickSearch(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static U8 SyIn_NAND_QuickSearch(void)
{
    tSI_MS_NAND_QS_IDX*     pQuick;
    tSI_MS_NAND_QS_HISTORY  history;
    U64                     max = 0;
    S16                     lower, upper;
    U16                     Idx;
    U8                      historyidx, area;

    pQuick = (tSI_MS_NAND_QS_IDX*)smSysInfo;

    // History Check
    for (historyidx = 0; historyidx < SFH_QUICK_IDX_COUNT; historyidx++)
    {
        HalSflash_Copy((PVOID)(&history), ((SFH_QUICK_IDX_START + historyidx) * SFLASH_SECTOR_SIZE), sizeof(tSI_MS_NAND_QS_HISTORY));
        if ((history.qwHistory > max) && (history.dwTag == SI_MS_NAND_QS_TAG))
        {
            max = history.qwHistory;
            gSysInfoMgr.wQuickIdxSecotr = historyidx;
        }

    }
    gSysInfoMgr.qwHistory = max;

    if (gSysInfoMgr.qwHistory == 0) return SYSINFO_ERR_NOTEXIST;


    // Find Quick Index
    HalSflash_Copy((PVOID)(pQuick), ((SFH_QUICK_IDX_START + gSysInfoMgr.wQuickIdxSecotr) * SFLASH_SECTOR_SIZE), SFLASH_SECTOR_SIZE);
    lower = 1;
    upper = (SFLASH_SECTOR_SIZE / SI_MS_NAND_QS_SIZE_DATA) - 1;
    while (lower <= upper)
    {
        Idx = (lower + upper) / 2;
        if (pQuick[Idx].dwTag != DWORD_MASK)
        {
            if (pQuick[Idx + 1].dwTag == DWORD_MASK || Idx == (SFLASH_SECTOR_SIZE / SI_MS_NAND_QS_SIZE_DATA) - 1)
            {
                break;
            }
            lower = Idx + 1;
        }
        else
        {
            upper = Idx - 1;
        }
        Idx = WORD_MASK;
    }
    if (Idx == WORD_MASK) return SYSINFO_ERR_NOTEXIST;

    if (pQuick[Idx].dwTag != SI_MS_NAND_QS_TAG || pQuick[Idx].QSCurrentPAA.all32 == DWORD_MASK)
    {
        return SYSINFO_ERR_UNKNOWN; // Take Care by Unknown Case
    }

    gSysInfoMgr.QSPAACurrent[0].all32 = pQuick[Idx].QSCurrentPAA.all32;
    gSysInfoMgr.QSPAANext.all32       = pQuick[Idx].QSCurrentPAA.all32;
    gSysInfoMgr.QSPAANext.b.page     += SYSINFO_NAND_DYNAMIC_PAGE_CNT;

    // Next Positon for Write
    gSysInfoMgr.wQuickIdx = (Idx + 1) * SI_MS_NAND_QS_SIZE_DATA;

    if (gSysInfoMgr.wQuickIdx == SFLASH_SECTOR_SIZE)
    {
        SyIn_NAND_QuickErase();
        SyIn_NAND_QuickStore(gSysInfoMgr.QSPAACurrent[0]);
    }

    SyIn_NAND_DetectOverWriteRisk();
    MEM_CLR(smSysInfo, SYSINFO_CACHE_SIZE);

    if (SyIn_NAND_Synchronize(SI_AREA_MISC, SYSINFO_READ, NULL) == SYSINFO_ERR_READ)
    {
        MEM_CLR(smSysInfo, SYSINFO_CACHE_SIZE);
        return SYSINFO_ERR_UNKNOWN;
    }

    for (area = 1; area < SYSINFO_NAND_AREA_COUNT; area++)
    {
        gSysInfoMgr.QSPAACurrent[area].all32 = smSysInfo->d.MiscData.d.InfoMgr.d.QSPAACurrent[area].all32;
    }

    return SYSINFO_ERR_NON;
}
//-------------------------------------------------------------------
// Function     : U8 SyIn_NAND_QuickStore(tPAA QuickIdx)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void SyIn_NAND_QuickStore(tPAA QuickIdx)
{
    tSI_MS_NAND_QS_IDX  idx;
    U32                 flashAddr;
    U8                  retry;

    idx.dwTag        = SI_MS_NAND_QS_TAG;
    idx.QSCurrentPAA = QuickIdx;
    retry            = SYSINFO_NOR_PROG_RETRYCNT;

    do
    {
        flashAddr = gSysInfoMgr.wQuickIdx + ((SFH_QUICK_IDX_START + gSysInfoMgr.wQuickIdxSecotr) * SFLASH_SECTOR_SIZE);
        if (HalSflash_ProgramU32((PVOID)&idx, flashAddr, SI_MS_NAND_QS_SIZE_DATA) == cEcNoError)
        {
            if (HalSflash_Compare((PVOID)&idx, flashAddr, SI_MS_NAND_QS_SIZE_DATA) == cEcNoError) break;
        }

        gSysInfoMgr.wQuickIdx += SI_MS_NAND_QS_SIZE_DATA;
        if (gSysInfoMgr.wQuickIdx == SFLASH_SECTOR_SIZE)
        {
            SyIn_NAND_QuickErase();
            retry = SYSINFO_NOR_PROG_RETRYCNT;
        }
    } while(retry--);
}
#endif

//-------------------------------------------------------------------
// Function     : static U8 SyIn_NAND_HistorySearch(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static U8 SyIn_NAND_HistorySearch(void)
{
    U64 histroyTmp, histroyMax;
    U16 page, pageTmp, bankIdx, bankTmp;
    U8  plane, planeTmp, area;

    plane       = 0;
    planeTmp    = 0;
    histroyMax  = 0;
    histroyTmp  = 0;
    bankTmp     = 0xFFFF;

    gSysInfoMgr.mgrBitMap |= (SI_S_OVERWRITE_RISK | SI_S_FOUND_OLD_DATA);

    for (bankIdx = 0; bankIdx < gSysInfoMgr.bankInfo.d.bankTotalCnt; bankIdx++)
    {
        for (plane = 0; plane < gNfInfo.planesPerLun; plane += 2)
        {
            gSysInfoMgr.QSPAACurrent[0].b.ce    = gSysInfoMgr.bankEntry[bankIdx].bCe;
            gSysInfoMgr.QSPAACurrent[0].b.ch    = gSysInfoMgr.bankEntry[bankIdx].bChS;
            gSysInfoMgr.QSPAACurrent[0].b.lun   = gSysInfoMgr.bankEntry[bankIdx].bLun;
            gSysInfoMgr.QSPAACurrent[0].b.plane = 0;
            if (gSysInfoMgr.bankInfo.d.bankTotalCnt == 4)
            {
                gSysInfoMgr.QSPAACurrent[0].b.page = SYSINFO_NAND_PAGE_START_SCE * SYSINFO_NAND_PAGECNT_STEP;
            }
            else
            {
                gSysInfoMgr.QSPAACurrent[0].b.page = SYSINFO_NAND_PAGE_START * SYSINFO_NAND_PAGECNT_STEP;
            }
            SyIn_NAND_Read(SYSINFO_NAND_AREA_MAIN, NULL, TRUE);
            if (smSysInfo->d.Header.d.MainHeader.d.dwTag   == SI_TAG_SYSINFO &&
                smSysInfo->d.Header.d.MainHeader.d.dwVerNo == SI_VER_SYSINFO)
            {
                histroyTmp = smSysInfo->d.MiscData.d.InfoMgr.d.SIHistory_L;
                if (histroyTmp > histroyMax)
                {
                    histroyMax = histroyTmp;
                    bankTmp    = bankIdx;
                    planeTmp   = plane;
                }
            }
            MEM_CLR(smSysInfo, SYSINFO_CACHE_SIZE);
        }
    }

    if (bankTmp == 0xFFFF)
    {
        gSysInfoMgr.QSPAACurrent[0].all32 = DWORD_MASK;
        return SYSINFO_ERR_NOTEXIST;
    }
    gSysInfoMgr.QSPAACurrent[0].b.ce    = gSysInfoMgr.bankEntry[bankTmp].bCe;
    gSysInfoMgr.QSPAACurrent[0].b.ch    = gSysInfoMgr.bankEntry[bankTmp].bChS;
    gSysInfoMgr.QSPAACurrent[0].b.lun   = gSysInfoMgr.bankEntry[bankTmp].bLun;
    gSysInfoMgr.QSPAACurrent[0].b.plane = planeTmp;
    D_PRINTF("[SI ] HS Found Bank|%2d Ch|%1d Ce|%1d Lun|%1d\n", bankTmp,
              gSysInfoMgr.bankEntry[bankTmp].bChS,
              gSysInfoMgr.bankEntry[bankTmp].bCe,
              gSysInfoMgr.bankEntry[bankTmp].bLun);

    pageTmp = 0xFFFF;
    for (page = SYSINFO_NAND_PAGE_START_SCE; page < ((SYSINFO_NAND_PAGE_CNT / SYSINFO_NAND_PAGECNT_STEP) - 2); page++)
    {
        gSysInfoMgr.QSPAACurrent[0].b.page = (page * SYSINFO_NAND_PAGECNT_STEP);
        if (SyIn_NAND_Read(SYSINFO_NAND_AREA_MAIN, NULL, TRUE) == SYSINFO_ERR_NON)
        {
            if (smSysInfo->d.Header.d.MainHeader.d.dwTag   == SI_TAG_SYSINFO &&
                smSysInfo->d.Header.d.MainHeader.d.dwVerNo == SI_VER_SYSINFO)
            {
                histroyTmp = smSysInfo->d.MiscData.d.InfoMgr.d.SIHistory_L;
                if (histroyTmp > histroyMax)
                {
                    histroyMax = histroyTmp;
                    pageTmp    = page;
                }
            }
            MEM_CLR(smSysInfo, SYSINFO_CACHE_SIZE);
        }
    }
    if (pageTmp == 0xFFFF) ASSERT(0);

    gSysInfoMgr.QSPAACurrent[0].b.page = pageTmp * SYSINFO_NAND_PAGECNT_STEP;

    while (SyIn_NAND_Synchronize(SI_AREA_MISC, SYSINFO_READ, NULL) != SYSINFO_ERR_NON)
    {
        gSysInfoMgr.QSPAACurrent[0].b.page -= SYSINFO_NAND_PAGECNT_STEP;
        MEM_CLR(smSysInfo, SYSINFO_CACHE_SIZE);
    }
    D_PRINTF("[SI ] HS Found Page|%d\n", gSysInfoMgr.QSPAACurrent[0].b.page);

    for (area = 1; area < SYSINFO_NAND_AREA_COUNT; area++)
    {
        gSysInfoMgr.QSPAACurrent[area].all32 = smSysInfo->d.MiscData.d.InfoMgr.d.QSPAACurrent[area].all32;
    }

    gSysInfoMgr.QSPAANext         = gSysInfoMgr.QSPAACurrent[0];
    gSysInfoMgr.QSPAANext.b.page += SYSINFO_NAND_DYNAMIC_PAGE_CNT;

    return SYSINFO_ERR_NON;
}

//-------------------------------------------------------------------
// Function     : static U8 SyIn_NAND_Synchronize(U32 dwOperateArea, U8 bDir, Msg_t* pHcmdMsg)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
U8 SyIn_NAND_Synchronize(U32 dwOperateArea, U8 bDir, MsgSI_t* pHcmdMsg)
{
    U32     dwOperateArea_nand = 0;
    U8      errType;

    //Update Area Check PList1
    if ((dwOperateArea & SI_AREA_PLIST1) || (dwOperateArea & SI_AREA_PLIST2))
    {
        dwOperateArea_nand |= SYSINFO_NAND_AREA_PLIST;
    }

    //Update Area Check FTL
    if  (dwOperateArea & SI_AREA_FTLRMP)
    {
        dwOperateArea_nand |= SYSINFO_NAND_AREA_FTL;
    }

    #if CHK_FW_TYPE(FW_AGING)
    if  (dwOperateArea & SI_AREA_AGINGEI)
    {
        dwOperateArea_nand |= SYSINFO_NAND_AREA_AGINGEI;
    }
    #endif

    //Update Area Check Rest
    dwOperateArea_nand |= SYSINFO_NAND_AREA_MAIN;
    switch (bDir)
    {
        case SYSINFO_WRITE:
            if (gSysInfoMgr.mgrBitMap & SI_S_WRITE_FAIL)
            {
                SyIn_NAND_ForceChangeBank();
                BIT_CLR(gSysInfoMgr.mgrBitMap, SI_S_WRITE_FAIL);
            }

            if (gSysInfoMgr.eraseBitMap == 0)
            {
                SyIn_NAND_Erase(SI_GET_HEALTH_BY_CURRPAA);
                errType = SYSINFO_ERR_NONRESOURCE;
            }
            else
            {
                SyIn_NAND_Write(dwOperateArea_nand, pHcmdMsg);
            }
            break;
        case SYSINFO_READ:
            errType = SyIn_NAND_Read(dwOperateArea_nand, pHcmdMsg, FALSE);
            break;
        case SYSINFO_WRITE_FORCE: // can we do not support this mdfk case?????
            Core_JobWait();
            while (gSysInfoMgr.eraseBitMap == 0)
            {
                SyIn_NAND_Erase(SI_GET_HEALTH_BY_CURRPAA);
                Core_JobWait();
            }
            if (SyIn_NAND_Write(dwOperateArea_nand, pHcmdMsg) == SYSINFO_ERR_NONRESOURCE) ASSERT(0);
            Core_JobWait();
            #if  SI_MS_NAND_QUICKSEARCH == TRUE
            SyIn_NAND_QuickStore(gSysInfoMgr.QSPAACurrent[0]);
            #endif
            break;
        default:
            errType = SYSINFO_ERR_UNKNOWN;
    }

    if (bDir == SYSINFO_READ && gSysInfoMgr.bErrBmp) ASSERT(0);

    return errType;
}

//-------------------------------------------------------------------
// Function     : static void SyIn_NAND_Management(U32 dwOperateArea_nand)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void SyIn_NAND_Management(S8 areaIdx, U8 bBackUpCnt, U8 sizeSplitCnt, U8 op, U32 sizeSplit, U16 mute)
{
    tPAA    paa;
    U32     paaIdx;
    U16     bankIdx;
    U8      ch, frag;

    if (op == SYSINFO_NAND_OPERATION_WRITE)
    {
        bankIdx = SyIn_NAND_BankIdx(gSysInfoMgr.QSPAANext);
        if(bBackUpCnt)
        {
            paa.all32    = gSysInfoMgr.QSPAACurrent[areaIdx].all32;
            paa.b.page  += (sizeSplitCnt * SYSINFO_NAND_PAGECNT_STEP * (MAX_PAA_PER_JOB / gEcuInfo.auPerPage / gSysInfoMgr.bankInfo.d.diePerBank));
            paa.b.plane += bBackUpCnt;
        }
        else
        {
            if (sizeSplitCnt == 0)
            {
                if (BIT(areaIdx) != SYSINFO_NAND_AREA_MAIN && BIT(areaIdx) != SYSINFO_NAND_AREA_AGINGEI)
                {
                    smSysInfo->d.MiscData.d.InfoMgr.d.RMPHistory[(areaIdx/2)] = gSysInfoMgr.QSPAACurrent[areaIdx];
                }
                if (mute != TRUE)
                {
                     SI_PRINTF_NECESSARY("[SI W] Area|%1d Bank|%1d Page|%3d\n", areaIdx, bankIdx, gSysInfoMgr.QSPAANext.b.page);
                }
                gSysInfoMgr.QSPAACurrent[areaIdx] = gSysInfoMgr.QSPAANext;
            }
            paa.all32 = gSysInfoMgr.QSPAANext.all32;

        }

    }
    else if (op == SYSINFO_NAND_OPERATION_READ)
    {
        bankIdx      = SyIn_NAND_BankIdx(gSysInfoMgr.QSPAACurrent[areaIdx]);
        paa.all32    = gSysInfoMgr.QSPAACurrent[areaIdx].all32;
        paa.b.plane += bBackUpCnt;
        if (sizeSplitCnt == 0)
        {
            if (mute != TRUE)
            {
                SI_PRINTF_NECESSARY("[SI R] Area|%1d Bank|%d Page|%3d\n", areaIdx, bankIdx, gSysInfoMgr.QSPAACurrent[areaIdx].b.page);
            }
        }
        else paa.b.page += (sizeSplitCnt * SYSINFO_NAND_PAGECNT_STEP * (MAX_PAA_PER_JOB / gEcuInfo.auPerPage / gSysInfoMgr.bankInfo.d.diePerBank));
    }
    else ASSERT(0);

    ch     = gSysInfoMgr.bankEntry[bankIdx].bChS;
    paaIdx = 0;
    frag   = 0;
    // start paa handle
    while (paaIdx != sizeSplit)
    {
        paa.b.ch    = ch;
        paa.b.frag  = frag;
        gFastPaaBuf[paaIdx].all32 = paa.all32;

        if (++frag == gEcuInfo.auPerPage)
        {
            frag = 0;

            if  ((++ch) == gSysInfoMgr.bankEntry[bankIdx].bChS + gSysInfoMgr.bankInfo.d.diePerBank)
            {
                ch = gSysInfoMgr.bankEntry[bankIdx].bChS;
                if ((paaIdx + 1) != sizeSplit)
                {
                    paa.b.page += SYSINFO_NAND_PAGECNT_STEP;
                }
            }
        }
        paaIdx++;
    }

    // updat PAANext
    if (bBackUpCnt == 0 && op == SYSINFO_NAND_OPERATION_WRITE)
    {
        gSysInfoMgr.QSPAANext.all32              = paa.all32;
        gSysInfoMgr.QSPAANext.b.ch               = gSysInfoMgr.bankEntry[bankIdx].bChS;
        gSysInfoMgr.QSPAANext.b.frag             = 0;
        gSysInfoMgr.QSPAANext.b.page            += SYSINFO_NAND_PAGECNT_STEP;

        if (BIT(areaIdx) == SYSINFO_NAND_AREA_MAIN)
        {
            smSysInfo->d.MiscData.d.InfoMgr.d.SIHistory_L++;
            MEM_COPY(smSysInfo->d.MiscData.d.InfoMgr.d.QSPAACurrent, gSysInfoMgr.QSPAACurrent, sizeof(tPAA) * SYSINFO_NAND_AREA_COUNT);
        }
    }
}

//-------------------------------------------------------------------
// Function     : static void SyIn_NAND_Write(U32 dwOperateArea_nand, Msg_t* pHcmdMsg)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static U8 SyIn_NAND_Write(U32 dwOperateArea_nand, MsgSI_t* pHcmdMsg)
{
    tFIO_JOB*   pJob[2];
    tSSD*       pHead = NULL;
    tSSD*       pTail = NULL;
    U32         paaIdx = 0, sizeSplit = 0;
    U16         cacheIdx;
    S8          areaIdx;
    U8          bBackUpCnt, bBackUpStatus = 0, idx = 0, sizeSplitCnt = 0;

    dwOperateArea_nand = SyIn_NAND_PageCheck(dwOperateArea_nand);

    if (dwOperateArea_nand == NULL)
    {
        return SYSINFO_ERR_NONRESOURCE;
    }
    //Backup Function//
    #if SYSINFO_IN_NAND_BACKUP == TRUE
    bBackUpStatus   = 1;
    #endif

    areaIdx = (SYSINFO_NAND_AREA_COUNT - 1);
    while (areaIdx >= 0)
    {
        bBackUpCnt = 0;
        if (dwOperateArea_nand & BIT(areaIdx))
        {
            while (bBackUpCnt <= bBackUpStatus)
            {
                sizeSplitCnt = 0;
                for(paaIdx = 0; paaIdx < gSysInfoNandTable[areaIdx].dwPaaCnt; paaIdx += MAX_PAA_PER_JOB)
                {
                    //Job//
                    pHead                               = smCacheMgr.head[SSD_POOL_OTHER];
                    cacheIdx                            = GET_SSD_INDEX_FROM_PTR(pHead);
                    pJob[idx]                           = FIO_PopJob(FIO_JOB_POOL_FREE);
                    pJob[idx]->pMsgHost                 = NULL;
                    pJob[idx]->ssdList                  = SSD_WO_CACHE;
                    pJob[idx]->cmd                      = FIO_JOB_CMD_WRITE;
                    pJob[idx]->plane                    = FIO_CMD_1PLANE;
                    pJob[idx]->ssdIdx[SSD_WO_CACHE]     = cacheIdx;
                    pJob[idx]->css                      = CACHE_SYSINFO;
                    pJob[idx]->flag                     = FIO_F_NO_MKERR | FIO_F_SLC;
                    pJob[idx]->aux[0]                   = DWORD_MASK;

                    //cache handle
                    pTail = GET_SSD_PTR_FROM_INDEX(cacheIdx);
                    pJob[idx]->ssdCnt[SSD_WO_CACHE]++;

                    if (paaIdx + MAX_PAA_PER_JOB < gSysInfoNandTable[areaIdx].dwPaaCnt)
                    {
                        sizeSplit = MAX_PAA_PER_JOB * MBU_SIZE;
                    }
                    else
                    {
                        sizeSplit = (gSysInfoNandTable[areaIdx].dwPaaCnt - paaIdx) * MBU_SIZE;
                    }

                    pTail->startAddr = ((U32)(&smSysInfo->all[0]) + gSysInfoNandTable[areaIdx].dwOffset + paaIdx * MBU_SIZE ) >> SSD_ADDR_SHIFT;
                    pTail->endAddr   = ((pTail->startAddr + sizeSplit)                                                        >> SSD_ADDR_SHIFT) - 1;
                    cacheIdx         = pTail->nextIndex;

                    //paa handle
                    SyIn_NAND_Management(areaIdx, bBackUpCnt, sizeSplitCnt, SYSINFO_NAND_OPERATION_WRITE, (sizeSplit / MBU_SIZE), NULL);
                    pJob[idx]->paaCnt = (sizeSplit / MBU_SIZE);
                    MEM_SET((void*)pJob[idx]->aux, BYTE_MASK, pJob[idx]->paaCnt * META_DATA_SIZE);
                    MEM_COPY(pJob[idx]->paa, gFastPaaBuf, pJob[idx]->paaCnt * sizeof(tPAA));

                    Cache_PopSSD_Other(SSD_POOL_OTHER, pHead, pTail, pJob[idx]->ssdCnt[SSD_WO_CACHE]);

                    if (pHcmdMsg)
                    {
                        if (bBackUpCnt == FALSE)
                        {
                            BIT_SET(gSysInfoMgr.mgrBitMap, SI_S_WRITE);
                            pHcmdMsg->hdr.b.status     = cMsgExecuted;
                            if (BIT(areaIdx) == SYSINFO_NAND_AREA_MAIN)
                            {
                                pHcmdMsg->quickIdx = gSysInfoMgr.QSPAACurrent[0];
                                pJob[idx]->pMsgHost = (MsgHostIO_t *)pHcmdMsg;
                            }
                        }
                    }
                    Core_JobIssue(pJob[idx]);
                    sizeSplitCnt++;
                }
                idx = ((idx == 0) ? 1 : 0);
                bBackUpCnt++;
            }
        }
        areaIdx--;
    }
    return SYSINFO_ERR_NON;
}

//-------------------------------------------------------------------
//PageCheck--Check Page Boundry
//-------------------------------------------------------------------
static U32 SyIn_NAND_PageCheck(U32 dwOperateArea_nand)
{
    U32     PageCnt;
    U16     page;
    S8      areaCnt;
    U8      plane;

    plane   = gSysInfoMgr.QSPAANext.b.plane;
    page    = gSysInfoMgr.QSPAANext.b.page;
    areaCnt = SYSINFO_NAND_AREA_COUNT - 1;
    PageCnt = 0;

    while (areaCnt >= 0)
    {
        if (dwOperateArea_nand & BIT(areaCnt))
            PageCnt += (gSysInfoNandTable[areaCnt].dwPaaCnt / 4 / gSysInfoMgr.bankInfo.d.diePerBank);
        areaCnt--;
    }

    if (page + (PageCnt - 1) * SYSINFO_NAND_PAGECNT_STEP >= (SYSINFO_NAND_PAGE_CNT - (SYSINFO_NAND_PAGECNT_STEP * 2)))
    {
        if (plane == 2 || gNfInfo.planesPerLun == 2)
        {
            if (SyIn_NAND_ForceChangeBank() == SYSINFO_ERR_NONRESOURCE) return NULL;
            plane   = 0;
        }
        else plane += 2;

        gSysInfoMgr.QSPAANext.b.plane  = plane;
        gSysInfoMgr.QSPAANext.b.page   = 0;

        dwOperateArea_nand |= SYSINFO_NAND_AREA_MAIN;
        dwOperateArea_nand |= SYSINFO_NAND_AREA_PLIST;
        dwOperateArea_nand |= SYSINFO_NAND_AREA_FTL;
        #if CHK_FW_TYPE(FW_AGING)
        dwOperateArea_nand |= SYSINFO_NAND_AREA_AGINGEI;
        #endif
    }

    return dwOperateArea_nand;
}

//-------------------------------------------------------------------
// Function     : static U8 SyIn_NAND_Read(U32 dwOperateArea_nand, Msg_t* pHcmdMsg, U16 errHandle)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static U8 SyIn_NAND_Read(U32 dwOperateArea_nand, MsgSI_t* pHcmdMsg, U16 mute)
{
    tFIO_JOB*   pJob[2];
    tSSD*       pHead = NULL;
    tSSD*       pTail = NULL;
    U32         paaIdx = 0, sizeSplit = 0;
    U16         cacheIdx;
    S8          areaIdx;
    U8          bBackUpCnt, bBackUpStatus = 0, errReadCnt = 0, idx = 0, sizeSplitCnt = 0;

    Core_JobWait();

    gSysInfoMgr.bErrBmp = 0;

    #if SYSINFO_IN_NAND_BACKUP == TRUE
    bBackUpStatus = 1;
    #endif
    areaIdx = SYSINFO_NAND_AREA_COUNT - 1;
    while (areaIdx >= 0)
    {
        if (dwOperateArea_nand & BIT(areaIdx))
        {
            bBackUpCnt = 0;

            if (gSysInfoMgr.QSPAACurrent[areaIdx].all32 == DWORD_MASK)
            {
                areaIdx--;
                continue;
            }
            while (bBackUpCnt <= bBackUpStatus)
            {
                sizeSplitCnt = 0;
                for(paaIdx = 0; paaIdx < gSysInfoNandTable[areaIdx].dwPaaCnt; paaIdx += MAX_PAA_PER_JOB)
                {
                    //Job//
                    pHead                           = smCacheMgr.head[SSD_POOL_OTHER];
                    cacheIdx                        = GET_SSD_INDEX_FROM_PTR(pHead);
                    pJob[idx]                       = FIO_PopJob(FIO_JOB_POOL_FREE);
                    pJob[idx]->ssdList              = SSD_WO_CACHE;
                    pJob[idx]->cmd                  = FIO_JOB_CMD_READ_SLC;
                    pJob[idx]->plane                = FIO_CMD_1PLANE;
                    pJob[idx]->ssdIdx[SSD_WO_CACHE] = cacheIdx;
                    pJob[idx]->css                  = CACHE_SYSINFO;
                    pJob[idx]->flag                 = FIO_F_NO_MKERR | FIO_F_SLC;
                    if (mute) pJob[idx]->flag      |= FIO_F_NO_RR | FIO_F_MUTE;

                    pTail                           = GET_SSD_PTR_FROM_INDEX(cacheIdx);

                    pJob[idx]->ssdCnt[SSD_WO_CACHE]++;

                    //cache handle
                    if (paaIdx + MAX_PAA_PER_JOB < gSysInfoNandTable[areaIdx].dwPaaCnt)
                    {
                        sizeSplit = MAX_PAA_PER_JOB * MBU_SIZE;
                    }
                    else
                    {
                        sizeSplit = (gSysInfoNandTable[areaIdx].dwPaaCnt - paaIdx) * MBU_SIZE;
                    }

                    pTail->startAddr = ((U32)(&smSysInfo->all[0]) + gSysInfoNandTable[areaIdx].dwOffset + paaIdx * MBU_SIZE ) >> SSD_ADDR_SHIFT;
                    pTail->endAddr   = ((pTail->startAddr + sizeSplit)                                                        >> SSD_ADDR_SHIFT) - 1;
                    Cache_PopSSD_Other(SSD_POOL_OTHER, pHead, pTail, pJob[idx]->ssdCnt[SSD_WO_CACHE]);

                    //paa handle
                    SyIn_NAND_Management(areaIdx, bBackUpCnt, sizeSplitCnt, SYSINFO_NAND_OPERATION_READ, (sizeSplit / MBU_SIZE), mute);
                    MEM_COPY(pJob[idx]->paa, gFastPaaBuf, sizeof(tPAA) * (sizeSplit / MBU_SIZE));
                    pJob[idx]->paaCnt = (sizeSplit / MBU_SIZE);

                    Core_JobIssue(pJob[idx]);
                    Core_JobWait();
                    if ((pJob[idx]->status & FIO_S_ERR_MASK) != FIO_S_NO_ERR && bBackUpCnt)
                    {
                        if (BIT(areaIdx) != SYSINFO_NAND_AREA_MAIN && BIT(areaIdx) != SYSINFO_NAND_AREA_AGINGEI)
                        {
                            if (smSysInfo->d.MiscData.d.InfoMgr.d.RMPHistory[(areaIdx / 2)].all32 == DWORD_MASK ||
                                smSysInfo->d.MiscData.d.InfoMgr.d.RMPHistory[(areaIdx / 2)].all32 == gSysInfoMgr.QSPAACurrent[areaIdx].all32)
                            {
                                gSysInfoMgr.bErrBmp |= BIT(areaIdx);
                                break;
                            }
                            BIT_SET(gSysInfoMgr.mgrBitMap, SI_S_OVERWRITE_RISK);
                            gSysInfoMgr.QSPAACurrent[areaIdx].all32 = smSysInfo->d.MiscData.d.InfoMgr.d.RMPHistory[(areaIdx / 2)].all32;
                            bBackUpCnt   = 0;
                            sizeSplitCnt = 0;
                            sizeSplit    = 0;
                            continue;
                        }
                        else return SYSINFO_ERR_READ;
                    }
                    sizeSplitCnt++;
                }

                if ((pJob[idx]->status & FIO_S_ERR_MASK) == FIO_S_NO_ERR) break;

                idx = ((idx == 0) ? 1 : 0);
                bBackUpCnt++;
            }
        }
        areaIdx--;
    }

    if (pJob[idx]->pMsgHost) pJob[idx]->pMsgHost->hdr.b.status = cMsgCompleted;

    return SYSINFO_ERR_NON;
}

//-------------------------------------------------------------------
// Function     : static void SyIn_NAND_DetectOverWriteRisk()
// Description  : This function should not be used after read SysInfo data
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void SyIn_NAND_DetectOverWriteRisk(void)
{
    tFIO_JOB*   pJob;
    tSSD*       pHead;
    tSSD*       pTail;
    tPAA        paa;
    U32         cnt;
    U16         cacheIdx;
    U8          frag = 0, emptyPaaCnt = 0, ch = 0;

    Core_JobWait();
    BIT_CLR(gSysInfoMgr.mgrBitMap, SI_S_OVERWRITE_RISK);
    paa.all32 = gSysInfoMgr.QSPAANext.all32;

    //Job//
    pHead                      = smCacheMgr.head[SSD_POOL_OTHER];
    cacheIdx                   = GET_SSD_INDEX_FROM_PTR(pHead);
    pJob                       = FIO_PopJob(FIO_JOB_POOL_FREE);
    pJob->ssdList              = SSD_WO_CACHE;
    pJob->cmd                  = FIO_JOB_CMD_READ_SLC;
    pJob->plane                = FIO_CMD_1PLANE;
    pJob->ssdIdx[SSD_WO_CACHE] = cacheIdx;
    pJob->css                  = CACHE_SYSINFO;
    pJob->flag                 = FIO_F_NO_MKERR | FIO_F_NO_RR | FIO_F_MUTE | FIO_F_SLC;

    pTail                      = GET_SSD_PTR_FROM_INDEX(cacheIdx);
    pJob->ssdCnt[SSD_WO_CACHE] = 1;

    // paa handle
    for (ch = 0; ch < gNfInfo.numChannels; ch++)
    {
        paa.b.ch = ch;
        for (frag = 0; frag < gEcuInfo.auPerPage; frag++)
        {
            paa.b.frag                      = frag;
            gFastPaaBuf[pJob->paaCnt].all32 = paa.all32;
            pJob->paaCnt++;
        }
    }
    MEM_COPY(pJob->paa, gFastPaaBuf, sizeof(tPAA) * pJob->paaCnt);

    pTail->startAddr = (U32)(&smSysInfo->all[0])                       >> SSD_ADDR_SHIFT;
    pTail->endAddr   = ((pTail->startAddr + (MBU_SIZE * pJob->paaCnt)) >> SSD_ADDR_SHIFT) - 1;

    Cache_PopSSD_Other(SSD_POOL_OTHER, pHead, pTail, pJob->ssdCnt[SSD_WO_CACHE]);

    Core_JobIssue(pJob);
    Core_JobWait();

    if (pJob->status == FIO_S_BLANK_ERR)
    {
        for (cnt = 0; cnt < (pJob->paaCnt); cnt++)
        {
            if (FIO_CHK_ERR_BMP(pJob->fid, cnt))
            {
                emptyPaaCnt++;
                FIO_CLR_ERR_BMP(pJob->fid, cnt);
            }
        }
    }

    if (emptyPaaCnt != pJob->paaCnt)
    {
        BIT_SET(gSysInfoMgr.mgrBitMap, SI_S_OVERWRITE_RISK);
    }
}

//-------------------------------------------------------------------
// Function     : static void SyIn_NAND_Erase(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void SyIn_NAND_Erase(U16 bankIdx)
{
    tFIO_JOB*   pJob;
    U8          paaIdx, plane, ch;

    for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
    {
        pJob            = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob->cmd       = FIO_JOB_CMD_ERASE;
        pJob->plane     = FIO_CMD_1PLANE;
        pJob->paaCnt    = pJob->plane * gSysInfoMgr.bankInfo.d.diePerBank;
        pJob->flag      = FIO_F_NO_MKERR | FIO_F_SLC;
        pJob->css       = CACHE_SYSINFO;

        paaIdx          = 0;
        for (ch = gSysInfoMgr.bankEntry[bankIdx].bChS; ch < gSysInfoMgr.bankEntry[bankIdx].bChS + gSysInfoMgr.bankInfo.d.diePerBank; ch++)
        {
            pJob->paa[paaIdx].all32   = 0;
            pJob->paa[paaIdx].b.block = 0;
            pJob->paa[paaIdx].b.ch    = ch;
            pJob->paa[paaIdx].b.ce    = gSysInfoMgr.bankEntry[bankIdx].bCe;
            pJob->paa[paaIdx].b.lun   = gSysInfoMgr.bankEntry[bankIdx].bLun;
            pJob->paa[paaIdx].b.plane = plane;

            paaIdx++;
        }
        BIT_SET(gSysInfoMgr.mgrBitMap, SI_S_ERASE);
        Core_JobIssue(pJob);
    }
}

//-------------------------------------------------------------------
// Function     : static void SyIn_MEDIA_Swap(void)
// Description  : Sysinfo Verification
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void SyIn_MEDIA_Swap(void)
{
    U16 naSub, norSub, tagMiss;

    SI_PRINTF_NECESSARY("[SI  ] MEDIA_Swap\n");
    MEM_CLR(smSysInfo, SYSINFO_CACHE_SIZE);

    if (SyIn_NOR_Read() == SYSINFO_ERR_NOTEXIST) return;

    SyIn_Verification();
    if (gSysInfoMgr.mgrBitMap & SI_S_INIT_FROM_SCRATCH) MEM_CLR(smSysInfo, SYSINFO_CACHE_SIZE);
    else // re-build data
    {
        for (naSub = 0; naSub<SYSINFO_SUBHEADER_CNT_NAND; naSub++)
        {
            tagMiss = true;
            for (norSub = 0; norSub < SYSINFO_NOR_TBL_CNT; norSub++)
            {
                if (smSysInfo->d.Header.d.SubHeader.All[naSub].d.dwValidTag == gSysInfoMgrTable[norSub].dwOPTag)
                {
                    tagMiss = false;
                    break;
                }
            }
            if (tagMiss) smSysInfo->d.Header.d.SubHeader.All[naSub].d.dwValidTag = 0;
        }
        SyIn_NAND_EraseBuildTable();
    }
}
#endif
#endif

#if _CPU_ == CPU_ID2
#include "PerfSection.h"
#endif
//-------------------------------------------------------------------
// Function     : U8 SYSINFO_Synchronize(U32 dwOperateArea, U8 bDir)
// Description  : Sysinfo Synchronize
// Input        : OperateArea, Dir
// return       : N/A
//-------------------------------------------------------------------
U8 SyIn_Synchronize(U32 dwOperateArea, U8 bDir, U8 bSyncBy)
{
#if _CPU_ == CPU_ID0 && _MEDIA_!=MEDIA_DRAM
    MsgSI_t* pHcmdMsg;
    ASM_DMB();

    pHcmdMsg = (MsgSI_t*)Ipc_GetExtendMsg(cMcSysInfo);

    pHcmdMsg->area      = dwOperateArea;
    pHcmdMsg->op        = bDir;
    pHcmdMsg->syncby    = bSyncBy;

    IPC_SendMsgQ(cH2cReqQueue, (Msg_t*)pHcmdMsg);
    while (pHcmdMsg->hdr.b.status != cMsgCompleted);
    
#endif

#if _CPU_ == CPU_ID2
    MsgSI_t *pMsg = NULL;
    U8      status = 0;

    if (bSyncBy == SI_SYNC_BY_NOR)
    {
        SyIn_NOR_Synchronize(bDir);
        return SYSINFO_ERR_NON;
    }

    status = SyIn_SyncCheck(dwOperateArea, bDir);
    if (status == SYSINFO_CHECK_PASS)
    {
        if (bDir == SYSINFO_WRITE)
        {
            pMsg = (MsgSI_t*)CMSG_GetExtendMsg(cMcSysInfo);

            if(pMsg == NULL) ASSERT(0);

            pMsg->area      |= dwOperateArea;
            pMsg->op        = bDir;
            pMsg->syncby    = bSyncBy;

            if (pMsg->hdr.b.status == cMsgPosted)
            {
                pMsg->hdr.b.status = cMsgReceived;
                CMSG_PushTail(CMSG_POOL_WAIT, (Msg_t*)pMsg);
            }
        }
        else SyIn_NAND_Synchronize(dwOperateArea, bDir, NULL);
    }
    else ASSERT(0);
#endif
    return SYSINFO_ERR_NON;
}

// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2012  -  All rights reserved
//
//  This computer program contains confidential and proprietary information,
//  and  may NOT  be reproduced or transmitted, in whole or in part,  in any
//  form,  or by any means electronic, mechanical, photo-optical, or  other-
//  wise, and  may NOT  be translated  into  another  language  without  the
//  express written permission from Marvell Corporation.
//
// ============================================================================
// =      C O M P A N Y      P R O P R I E T A R Y      M A T E R I A L       =
// ============================================================================
//
//! @file
//! @brief AHCI I/F protocol service code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "HalHwCheck.h"

#include "Heap.h"
#include "CoreMsg.h"
#include "Fio.h"
#include "Ftl.h"
#include "Gc.h"
#include "CoreDbg.h"
#include "Thermal.h"
#include "SysInfo.h"
#include "ErrHandle.h"
#include "Aging.h"
#include "Cache.h"
#include "IDS.h"
#include "TcgNfIf.h"
#include "CmdSensor.h"
#include "PLP.h"
#include "Core.h"
#include "UncTable.h"
#include "Purge.h"
#include "SmBus.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef tCMSG_CMD_STATUS (*CoreMsgHandler_t)(Msg_t*);

typedef struct
{
    U32              attr;
    CoreMsgHandler_t fptr;
} CoreProcHostMsgTbl_t;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static          Error_t Core_OneTimeInit(InitBootMode_t initMode);
static          Error_t Core_WaitCPU1(void);
static          void    Core_SetPowerMode(DevicePowerMode_t powerMode);
static          void    Core_ServiceLoop(void);
static          void    Core_ServiceLoopPLP(void);
static          void    Core_ServiceLoopAging(void);

static          Error_t Core_ZeroLaaOneTimeInit(InitBootMode_t initMode);

static          void    Core_SmartOneTimeInit(InitBootMode_t initMode);
static          void    Core_SmartAttrOperation(void);

static __inline void    Core_ProcReset(void);
static __inline Error_t Core_ProcPreRead(void);
static __inline void    Core_ProcHitFillup(U16 cacheIdx);
static __inline void    Core_ProcForceFillup(void);
                void    Core_ProcForceFlush(void);
static __inline void    Core_ProcErase(void);
static __inline void    Core_ProcWriteLastPage(U32 header, U8 group);
static __inline void    Core_ProcWritePartialBuildTable(void);
static __inline void    Core_ProcReleaseCache(void);
                void    Core_ProcWaitHeapJob(void);
                void    Core_ProcJobComplete(void);

// Call Back Function List
static tCMSG_CMD_STATUS Core_HostRead(MsgHostIO_t* pMsg);
static tCMSG_CMD_STATUS Core_HostWrite(MsgHostIO_t* pMsg);
static tCMSG_CMD_STATUS Core_HostTrim(MsgTrim_t* pMsg);
static tCMSG_CMD_STATUS Core_HostFlush(Msg_t* pMsg);
static tCMSG_CMD_STATUS Core_HostIdleFlush(Msg_t* pMsg);
static tCMSG_CMD_STATUS Core_HostShutdown(Msg_t* pMsg);
static tCMSG_CMD_STATUS Core_HostSmartOp(MsgSmart_t* pMsg);
static tCMSG_CMD_STATUS Core_HostSI(MsgSI_t* pMsg);
static tCMSG_CMD_STATUS Core_HostVendor(MsgVsc_t* pMsg);
static tCMSG_CMD_STATUS Core_HostTcgFunc(MsgTcg_t* pMsg);
static tCMSG_CMD_STATUS Core_HostFormat(MsgFormat_t* pMsg);
static tCMSG_CMD_STATUS Core_HostWriteZero(MsgWrtZero_t* pMsg);
static tCMSG_CMD_STATUS Core_HostResetCache(Msg_t* pMsg);
static tCMSG_CMD_STATUS Core_HostPowerDown(MsgPM_t* pMsg);
static tCMSG_CMD_STATUS Core_HostSetFeature(Msg_t* pMsg);
static tCMSG_CMD_STATUS Core_HostPurge(MsgPG_t* pMsg);
static tCMSG_CMD_STATUS Core_HostReset(MsgHostIO_t* pMsg);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
static const CoreProcHostMsgTbl_t gCoreProcHostMsgTable[cMcHostPerfLast] =
{
    {CORE_MSG_ATTR_NVME_RESET,  (CoreMsgHandler_t)Core_HostRead       },
    {CORE_MSG_ATTR_NVME_RESET,  (CoreMsgHandler_t)Core_HostWrite      },
    {CORE_MSG_ATTR_NVME_RESET,  (CoreMsgHandler_t)Core_HostTrim       },
    {CORE_MSG_ATTR_NVME_RESET,  (CoreMsgHandler_t)Core_HostFlush      },
    {CORE_MSG_ATTR_NVME_RESET,  (CoreMsgHandler_t)Core_HostFlush      },
    {CORE_MSG_ATTR_NVME_RESET,  (CoreMsgHandler_t)Core_HostIdleFlush  },
    {CORE_MSG_ATTR_NVME_RESET,  (CoreMsgHandler_t)Core_HostShutdown   },
    {CORE_MSG_ATTR_NVME_RESET,  (CoreMsgHandler_t)Core_HostSmartOp    },
    {CORE_MSG_ATTR_NVME_RESET,  (CoreMsgHandler_t)Core_HostVendor     },
    {0                       ,  (CoreMsgHandler_t)Core_HostSI         },
    {0                       ,  (CoreMsgHandler_t)Core_HostTcgFunc    },
    {CORE_MSG_ATTR_NVME_RESET,  (CoreMsgHandler_t)Core_HostFormat     },
    {CORE_MSG_ATTR_NVME_RESET,  (CoreMsgHandler_t)Core_HostWriteZero  },
    {CORE_MSG_ATTR_NVME_RESET,  (CoreMsgHandler_t)Core_HostRead       },
    {0                       ,  (CoreMsgHandler_t)Core_HostResetCache },
    {0                       ,  (CoreMsgHandler_t)Core_HostPowerDown  },
    {0                       ,  (CoreMsgHandler_t)Core_HostSetFeature },
    {0                       ,  (CoreMsgHandler_t)Core_HostPurge      },
    {0                       ,  (CoreMsgHandler_t)Core_HostReset      }
};

NfInfo_t  gNfInfo;
EcuInfo_t gEcuInfo;

volatile U32 gCoreDeviceSelfTest = 0;
U32 gCoreFreePrpCnt;
U32 gCoreShutdownFlag = FALSE;
U8  gCoreFirstSegmFail = 0;
U32 gCoreDSTCount = 0;

// For Smart attribute use
U16 gSmrdeEra[(MAX_SUPPORTED_CHANNELS*MAX_SUPPORTED_DEVICES*MAX_SUPPORTED_LUNS)];
U16 gSmrdeWri[(MAX_SUPPORTED_CHANNELS*MAX_SUPPORTED_DEVICES*MAX_SUPPORTED_LUNS)];

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    One time initialization for NAND media sub-system.

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Core_OneTimeInit(InitBootMode_t initMode)
{
    CMSG_OneTimeInit(initMode);

    #if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
    HalPm_OneTimeInit(initMode);
    #endif

    Cache_OneTimeInit(initMode);

    Core_WaitCPU1();

    #if (FIO_JOB_HEAP_MEMORY == TRUE)
    Heap_OneTimeInit(initMode);
    #endif

    FIO_OneTimeInit(initMode);

    CPU2_ACK_CPU1();

    SyIn_OneTimeInit(initMode);

    ErrHandle_OneTimeInit(initMode);

    #if (SET_READY_AFTER_FTL == FALSE)
    CPU2_ACK_CPU0();
    #endif

    #if CHK_FW_TYPE(FW_AGING)
    Aging_OneTimeInit(initMode);
    #else
        #if (_MEDIA_ != MEDIA_DRAM)
        FTL_OneTimeInit(initMode); // FTL initial
        GC_OneTimeInit(initMode);
        #endif

        Core_ZeroLaaOneTimeInit(initMode);
    #endif

    #if (CO_SUPPORT_SMBUS == TRUE)
    AplSmbus_OneTimeInitVpd(initMode);
    #endif

    #if (SET_READY_AFTER_FTL == TRUE)
    CPU2_ACK_CPU0();
    #endif

    AplThermal_OneTimeInit(initMode);

    Core_SmartOneTimeInit(initMode);

    CoreDbg_OneTimeInit(initMode);

    #if (CO_SUPPORT_PLP == TRUE)
    AplPlp_OneTimeInit(initMode);
    #endif

    AplCmdSensor_OneTimeInit(initMode);

    #if (CO_SUPPORT_SMBUS == TRUE)
    AplSmbus_OneTimeInitBm(initMode);
    #endif

    gCoreFreePrpCnt = CORE_PRP_COUNT;

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    One time initialization for RZAT

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Core_ZeroLaaOneTimeInit(InitBootMode_t initMode)
{
    if (smSysInfo->d.FtlData.d.zeroTag != SI_TAG_ZERO)
    {
        Core_SendResetCacheMsg();
    }
    else
    {
        tPAA paa;
        uint32_t laa;

        for (laa = L2P_EXT_ZERO_START; laa < (L2P_EXT_ZERO_START + L2P_EXT_NUM_ZERO); laa++)
        {
            // Handle error exceptional
            paa = FTL_PhyAddrSearch(laa);
            if (paa.all32 == DWORD_MASK)
            {
                Core_SendResetCacheMsg();
                break;
            }

            // Strict Checker for early development
            #if 1
            if (paa.b.ch < gNfInfo.numChannels &&
                paa.b.ce < gNfInfo.devicesPerChannel &&
                paa.b.lun < gNfInfo.lunsPerDevice &&
                paa.b.plane < gNfInfo.planesPerLun &&
                paa.b.block < gNfInfo.blocksPerPlane)
            {
                // valid
            }
            else
            {
                // someone destroy zero laa accidently
                ASSERT(0);
            }
            #endif
        }
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**

    Wait for CPU1 initialization ready

**/
//-----------------------------------------------------------------------------
static Error_t Core_WaitCPU1(void)
{
    U32 timeout = 0;

    D_PRINTF("[Core] Wait Media\n");

    while (smCpu2WaitCpu1Flag != SYNC_CPU2_W_CPU1_FLAG) // Wait Nand info
    {
        HalTimer_DelayUsec(10);
        timeout++;
        ASSERT(timeout < (SYNC_TIMEOUT_US / 10));
    }

    MEM_COPY(&gNfInfo,  (PVOID)(smNfInfoCpu1Addr  | AXI_B0TCM_CPU1_ADDR),  sizeof(NfInfo_t));
    MEM_COPY(&gEcuInfo, (PVOID)(smEcuInfoCpu1Addr | AXI_B0TCM_CPU1_ADDR), sizeof(EcuInfo_t));

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    CoreMain Set Power Mode
**/
//-----------------------------------------------------------------------------
static void Core_SetPowerMode(DevicePowerMode_t powerMode)
{
    #if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
    switch (powerMode)
    {
        case cDevicePmSleep:
            AplThermal_ThrottleException();
            HalPm_EnterExitPowerDownMode();
            break;
        case cDevicePmDeepSleep:
            AplThermal_ThrottleException();
            HalPm_EnterDeepPowerDownMode();
            break;
        case cDevicePmActive:
            break;
        default:
            HALT();
            break;
    }
    #endif
}

//-----------------------------------------------------------------------------
/**
    Smart Init
**/
//-----------------------------------------------------------------------------
static void Core_SmartOneTimeInit(InitBootMode_t initMode)
{
    Core_SmartAttrOperation();
    if (initMode == cInitBootCold)
    {
        smSysInfo->d.LogInfo.d.SmrLog.pwrCycCnt++;
        if(smSysInfo->d.LogInfo.d.LogFlags.unsafeFlag)
        {
            smSysInfo->d.LogInfo.d.SmrLog.unsafePwrCycCnt++;
        }
        smSysInfo->d.Header.d.SubHeader.d.NVMeLog.d.dwValidTag = SI_TAG_LOGINFO;
        smSysInfo->d.LogInfo.d.LogFlags.unsafeFlag = TRUE;
        SyIn_Synchronize(SI_AREA_SMART, SYSINFO_WRITE_FORCE, SI_SYNC_BY_NVMELOG);
    }
}

#include "NormSection.h"
//-----------------------------------------------------------------------------
/**
    Process device self-test
**/
//-----------------------------------------------------------------------------
static void Core_ProcDeviceSelfTest(void)
{
    U32    *pCurBuffer;
    U32    idx;
    U8     cnt;
    U8     SegmentFail = 0;

    switch (gCoreDeviceSelfTest)  //1-5 short, 6-9 extended
    {
        case 0x01:  // 1 RAM Check

            pCurBuffer = (U32 *)&gCoreDbgBuf.wBuffer[0];

            for (cnt = 0; cnt < 64; cnt++)
            {
                for(idx = 0; idx < 0x1000UL; idx++)
                {
                    pCurBuffer[cnt*0x1000UL+idx] = 0xAA5500FF;
                }
            }

            MEM_COPY(gCoreDbgBuf.rBuffer, gCoreDbgBuf.wBuffer, 1*MBYTE);

            pCurBuffer = (U32 *)&gCoreDbgBuf.rBuffer[0];

            for (cnt = 0; cnt < 64; cnt++)
            {
                for(idx = 0; idx < 0x1000UL; idx++)
                {
                    if (pCurBuffer[cnt*0x1000UL+idx] != 0xAA5500FF)
                    {
                        D_PRINTF("Compare Error, pCurBuffer=%8X \n", pCurBuffer[cnt*0x1000UL+idx]);
                        SegmentFail = 1;
                        gCoreDeviceSelfTest = 0xFF;
                        break;
                    }
                }
                if (SegmentFail) break;
            }
            if (SegmentFail) break;

            gCoreDeviceSelfTest = 0x02;
            break;

        case 0x02:  // 2 SMART Check
           Core_SmartAttrOperation();
            if (smSysInfo->d.LogInfo.d.SmrLog.CriticalWarn)
            {
                SegmentFail = 2;
                gCoreDeviceSelfTest = 0x0FF;
                break;
            }
            gCoreDeviceSelfTest = 0x03;
            break;

        case 0x03:  // 3 Volatile memory backup
            gCoreDeviceSelfTest = 0x04;
            break;

        case 0x04:  // 4 Metadata validation
            gCoreDeviceSelfTest = 0x05;
            break;

        case 0x05:  // 5 NVM integrity
            gCoreDeviceSelfTest = 0xFF;  //Completion
            break;

        case 0x06:  // 6 Data Integrity
            gCoreDeviceSelfTest = 0x07;
            break;

        case 0x07:  // 7 Media Check
            gCoreDeviceSelfTest = 0x08;
            break;

        case 0x08:  // 8 Drive Life
            gCoreDeviceSelfTest = 0x09;
            break;

        case 0x09:  // 9 SMART Check
            Core_SmartAttrOperation();
            if (smSysInfo->d.LogInfo.d.SmrLog.CriticalWarn)  SegmentFail = 9;
            gCoreDeviceSelfTest = 0xFF;
            break;
        default:
            gCoreDeviceSelfTest = 0;
    }

    if ((SegmentFail) && (!gCoreFirstSegmFail))
    {
        gCoreFirstSegmFail = SegmentFail;
    }

    if (gCoreDeviceSelfTest == 0xFF)
    {
        gCoreDeviceSelfTest = 0;
        gCoreDSTCount = 0;

        if (gCoreFirstSegmFail)
        {
            smSysInfo->d.LogInfo.d.DSTLog.DSTResult.Segment = gCoreFirstSegmFail;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Process Host SMART Attribute
**/
//-----------------------------------------------------------------------------
static void Core_SmartAttrOperation(void)
{
    U32 pool = FTL_BLOCK_POOL_MAX;
    U32 ftlTotalBlkCnt      = 0;
    U32 ftlFreeBlkCnt       = 0;
    U32 ftlEraseBlkCnt      = 0;
    U32 endurance           = 0;
    U32 endurancePer        = 0;
    U32 totalVac            = 0;
    U32 tmpvac              = 0;
    U16 block;
    U16 maxEraseCnt;
    U16 minEraseCnt;
    U16 die, plane, tmpRsBlk;
    U16 deCnt, totDeCnt;
    U32 dTmpTotal = 0;
    U32 dTmpWorst = 0;
    U32 dTmpPWorst = 0;

    //D_PRINTF("[Core] SmartOp\n");

    smcThermalSource[cThermalSourceI2c] = AplThermal_GetI2cTemperature();

    for (pool = FTL_BLOCK_POOL_USER; pool < FTL_BLOCK_POOL_MAX; pool++)
    {
        ftlTotalBlkCnt += gFtlBlkListInfo.blkCnt[pool];
    }

    //  Available Spare ---------------------------------------------------------------------
    //ftlFreeBlkCnt = gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE];
    //smSysInfo->d.LogInfo.d.SmrLog.availableSpare = ftlFreeBlkCnt * 100 / ftlTotalBlkCnt;
    ftlFreeBlkCnt =  ftlTotalBlkCnt - gGCInfo.op.totHBlkCnt;  //OP blk cnt
    smSysInfo->d.LogInfo.d.SmrLog.availableSpare = ((ftlFreeBlkCnt - gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_BAD]) * 100) / ftlFreeBlkCnt;

    //  Percentage Used, WearLevelingCount (DELL), Wear Range Delta (WCS)--------------------
    maxEraseCnt = 0;
    minEraseCnt = U16_MAX;
    endurance   = 3000;
    for (block = 0; block < ftlTotalBlkCnt; block++)
    {
        if(block > FTL_SUPPORT_MAX_BLK -1)
        {
            break;
        }
        ftlEraseBlkCnt += gFtlBlkListTbl[block].eraseCnt;

        if(gFtlBlkListTbl[block].eraseCnt > maxEraseCnt)
        {
            maxEraseCnt = gFtlBlkListTbl[block].eraseCnt;
        }
        if(gFtlBlkListTbl[block].eraseCnt < minEraseCnt)
        {
            minEraseCnt = gFtlBlkListTbl[block].eraseCnt;
        }

    }
    endurancePer = (ftlEraseBlkCnt * 100) / ((block + 1) * endurance);

    if(endurancePer > 0xFF)
    {
        smSysInfo->d.LogInfo.d.SmrLog.percentageUsed = 0xFF;
    }
    else
    {
        smSysInfo->d.LogInfo.d.SmrLog.percentageUsed = (U8)endurancePer;
    }

    smSysInfo->d.LogInfo.d.SmrLog.WearLvlCnt        = smSysInfo->d.LogInfo.d.SmrLog.percentageUsed;
    smSysInfo->d.LogInfo.d.SmrLog.WCSWearRangeDelta = (maxEraseCnt - minEraseCnt) * 100 / endurance;

    // UsedReservedBlockCountWorst, UsedReservedBlockCountTotal, ReservedBlockCount --------
    smSysInfo->d.LogInfo.d.SmrLog.UsedRsvBlkCnt_W = 0;
    smSysInfo->d.LogInfo.d.SmrLog.UsedRsvBlkCnt_T = 0;
    smSysInfo->d.LogInfo.d.SmrLog.RsvBlkCnt = 0;

    for (die = 0; die < gNfInfo.totalLun; die++)
    {
        for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
        {
            tmpRsBlk = ((gNfInfo.blocksPerPlane - 1) - smSysInfo->d.FtlData.d.FreeIdx[die][plane]);
            if (tmpRsBlk > dTmpWorst)
            {
                dTmpWorst = tmpRsBlk;
            }
            dTmpTotal += tmpRsBlk;
            smSysInfo->d.LogInfo.d.SmrLog.RsvBlkCnt += (smSysInfo->d.FtlData.d.FreeIdx[die][plane] - gGCInfo.op.totHBlkCnt);
        }
    }

    smSysInfo->d.LogInfo.d.SmrLog.UsedRsvBlkCnt_T = (dTmpTotal * 100) / ((gNfInfo.blocksPerPlane - gGCInfo.op.totHBlkCnt) *
               gNfInfo.totalLun * gNfInfo.planesPerLun);
    smSysInfo->d.LogInfo.d.SmrLog.UsedRsvBlkCnt_W = (dTmpWorst * 100) / (gNfInfo.blocksPerPlane - gGCInfo.op.totHBlkCnt);

    //  ReAssignedSectorCount ---------------------------------------------------------------
    totDeCnt = (smSysInfo->d.GList.d.GListBrief.wCntErase +
                smSysInfo->d.GList.d.GListBrief.wCntRead  +
                smSysInfo->d.GList.d.GListBrief.wCntWrite);

    //smSysInfo->d.LogInfo.d.SmrLog.ReAssignedSecCnt = totDeCnt * (gNfInfo.dataBytesPerPage * gNfInfo.pagesPerBlock / HLBU_DATA_SIZE);
    smSysInfo->d.LogInfo.d.SmrLog.ReAssignedSecCnt = (totDeCnt * 100) /
               ((gNfInfo.blocksPerPlane - gGCInfo.op.totHBlkCnt) * gNfInfo.totalLun * gNfInfo.planesPerLun);

    //  ProgramFailCountTotal ---------------------------------------------------------------
    endurance = 0xFFFF;  // Max allowed erase fails in SSD lifetime  ???
    dTmpPWorst = 0;
    dTmpWorst = 0;
    smSysInfo->d.LogInfo.d.SmrLog.PgmFailCnt_T = (smSysInfo->d.GList.d.GListBrief.wCntWrite * 100) / endurance;

    //  EraseFailCountTotal -----------------------------------------------------------------
    smSysInfo->d.LogInfo.d.SmrLog.ErsFailCnt_T = (smSysInfo->d.GList.d.GListBrief.wCntErase * 100) / endurance;

    //  ProgramFailCountWorst, EraseFailCountWorst ------------------------------------------
    smSysInfo->d.LogInfo.d.SmrLog.PgmFailCnt_W = 0;
    smSysInfo->d.LogInfo.d.SmrLog.ErsFailCnt_W = 0;

    MEM_CLR((void*)&gSmrdeEra[0], sizeof(gSmrdeEra));
    MEM_CLR((void*)&gSmrdeWri[0], sizeof(gSmrdeWri));

    for(deCnt = 0; deCnt < totDeCnt; deCnt++)
    {
        switch(smSysInfo->d.GList.d.GList[deCnt].bType)
        {
            case GLIST_DEFECT_ERASE:
                gSmrdeEra[smSysInfo->d.GList.d.GList[deCnt].bDie]++;
                break;
            case GLIST_DEFECT_WRITE:
                gSmrdeWri[smSysInfo->d.GList.d.GList[deCnt].bDie]++;
                break;
            default:
                break;
        }
    }

    for (die = 0; die < gNfInfo.totalLun; die++)
    {
        if (dTmpWorst < gSmrdeEra[die])
        {
            dTmpWorst = gSmrdeEra[die];
        }
        if (dTmpPWorst < gSmrdeWri[die])
        {
            dTmpPWorst = gSmrdeWri[die];
        }
    }
    smSysInfo->d.LogInfo.d.SmrLog.ErsFailCnt_W = (dTmpWorst * 100) / (endurance / gNfInfo.totalLun);
    smSysInfo->d.LogInfo.d.SmrLog.PgmFailCnt_W = (dTmpPWorst * 100) / (endurance / gNfInfo.totalLun);

    //  WCS Mapped LBA count ------------------------------------------
    for (block = 0; block < FTL_SUPPORT_MAX_BLK; block++) //(gNfInfo.userBlkPerLun >> gNfInfo.bitsPlane)
    {
        if (block == gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][SSD_POOL_FREE].block)
        {
            // dummy page
            tmpvac = (gNfInfo.pagesPerBlock - (gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][SSD_POOL_FREE].page + 1)) *
                      gNfInfo.planesPerLun * gNfInfo.totalLun;
            // partial page
            tmpvac += (gNfInfo.lunsPerChannel - gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][SSD_POOL_FREE].bank) *
                       gNfInfo.planesPerLun * gNfInfo.numChannels;
            tmpvac *= gEcuInfo.auPerPage;
            totalVac += gFtlBlkListTbl[block].vs.b.vac - tmpvac;
        }
        else if((gFtlBlkListTbl[block].vs.b.status == FTL_BLOCK_POOL_BAD) || (block == gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][SSD_POOL_FREE].block))
        {
            totalVac += 0;
        }
        else
        {
            totalVac += gFtlBlkListTbl[block].vs.b.vac;
        }
    }
    smSysInfo->d.LogInfo.d.SmrLog.WCSLbaMapCnt = totalVac * HBU_PER_MBU;

    //  WCS Media Units Written ------------------------------------------
    smSysInfo->d.LogInfo.d.SmrLog.WCSFlushCnt += ((smFioInfo.flushPaaCnt / gEcuInfo.auPerPage));
    smFioInfo.flushPaaCnt = 0;

    //  CriticalWarn ----------------------------------------------------------------------------------------
    smSysInfo->d.LogInfo.d.SmrLog.CriticalWarn = 0;

    #if (_CUSTOMER_ == CUSTOMER_DELL)
    #if 0
    if ((smSysInfo->d.LogInfo.d.SmrLog.availableSpare < 50) ||
        (smSysInfo->d.LogInfo.d.SmrLog.ReAssignedSecCnt > 50) ||
        (smSysInfo->d.LogInfo.d.SmrLog.UsedRsvBlkCnt_T > 50))
    {
        smSysInfo->d.LogInfo.d.SmrLog.CriticalWarn |= 0x01;  // Spare
    }
    if ((smSysInfo->d.LogInfo.d.SmrLog.PgmFailCnt_T > 90) ||
        (smSysInfo->d.LogInfo.d.SmrLog.ErsFailCnt_T > 90) ||
        (smSysInfo->d.LogInfo.d.SmrLog.WearLvlCnt > 90))
    {
        smSysInfo->d.LogInfo.d.SmrLog.CriticalWarn |= 0x04;  // Reliability
    }
    #endif
    #endif
    if (gGCInfo.state.b.readOnly)  //ReadOnlyMode
    {
        smSysInfo->d.LogInfo.d.SmrLog.CriticalWarn |= 0x08;  // Media
    }
}

#include "PerfSection.h"
//-------------------------------------------------------------------
// Function     : void Core_JobIssue(tFIO_JOB* pJob)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Core_JobIssue(tFIO_JOB* pJob)
{
    #if (FIO_JOB_HEAP_MEMORY == TRUE)
    tPAA* HeapPaa = NULL;

    if (pJob->cmd < FIO_JOB_CMD_WRITE)
    {
        HeapPaa = (tPAA*)Heap_Alloc(pJob->paaCnt + (pJob->flag & FIO_F_AUX) * 2 * pJob->paaCnt);
    }

    if (HeapPaa != NULL)
    {
        if (pJob->cmd < FIO_JOB_CMD_ERASE)
        {
            if (pJob->css == CACHE_AUTO)
            {
                Cache_Alloc_Auto(pJob);   // Auto SSD Allocation
            }
        }

        MEM_COPY(HEAP_GET_PTR_CPU1_B0TCM(HeapPaa), pJob->paa, sizeof(tPAA) * pJob->paaCnt);
        pJob->paa = HeapPaa;

        if (pJob->flag & FIO_F_AUX)
        {
            if (pJob->cmd == FIO_JOB_CMD_WRITE)
            {
                MEM_COPY(HEAP_GET_PTR_CPU1_B0TCM(&HeapPaa[pJob->paaCnt]), pJob->aux, sizeof(tPAA) * pJob->paaCnt * 2);
            }
            pJob->aux = (U32*)(&HeapPaa[pJob->paaCnt]);
        }

        IPC_SendMsgQ(cC2mReqQueue, (Msg_t*)pJob);
        smFioInfo.jobIssueCnt++;
    }
    else
    {
        FIO_PushJob(FIO_JOB_POOL_HEAP, pJob);
    }
    #else

    ASM_DMB();
    if (pJob->css == CACHE_AUTO && pJob->cmd < FIO_JOB_CMD_ERASE)
    {
        Cache_Alloc_Auto(pJob);   // Auto SSD Allocation
    }

    IPC_SendMsgQ(cC2mReqQueue, (Msg_t*)pJob);
    smFioInfo.jobIssueCnt++;

    #endif
}

//-----------------------------------------------------------------------------
/**
    Wait for job complete
**/
//-----------------------------------------------------------------------------
void Core_JobWait(void)
{
    U32 timeout = 0;

    while ((smFioInfo.jobIssueCnt != smFioInfo.jobDoneCnt) || FIO_GET_HEAP_POOL_CNT())
    {
        if (IPC_MsgQFastPeek(cM2cComQueue))
        {
            Core_ProcJobComplete();
        }

        if (FIO_GET_HEAP_POOL_CNT())
        {
            Core_ProcWaitHeapJob();
        }

        HalTimer_DelayUsec(1);
        ASSERT(timeout++ < FIO_TIMEOUT_US);
    }
}

//-------------------------------------------------------------------
// Function     : Core_TrimOperation(U32 currLaa, U32 laaEnd)
// Description  :
// Input        : currLaa : Start LAA
//              : laaEnd  : End LAA
// return       : N/A
//-------------------------------------------------------------------
static U8 Core_TrimOperation(U32 currLaa, U32 laaEnd, U32 checkCache)
{
    U32 laaStartTrim, bkpgS, bkpgE, chkbmpS, chkbmpE;
    U16 cacheIdx;
    U16 cacheStatus;
    U8  fulldisc = FALSE;

    //D_PRINTF("Trim:laaS[%X]laaE[%X]cnt[%d]\n", currLaa, laaEnd, laaEnd-currLaa);

    laaStartTrim = currLaa;

    if ((L2P_NUM < laaEnd) || (L2P_NUM < currLaa) || (laaEnd < currLaa))
    {
        D_PRINTF("ERR! laa[0x%X-0x%X]ovr[0x%X]\n", currLaa, laaEnd, L2P_NUM);
        return fulldisc; // check start laa and end laa
    }

    gFtlTrimInfo.trimBMPcnt += (laaEnd - currLaa);

    if ((currLaa == 0) && (laaEnd >= (L2P_NUM_RAW - 1)))  //ALL Size
    {
        MEM_SET((PVOID)&gFtlL2PTbl[currLaa], BYTE_MASK, sizeof(tPAA) * (laaEnd - currLaa));
        fulldisc = TRUE;
    }
    else  //Partial Size
    {
        bkpgS = (laaStartTrim / FTL_LAA_BANK_PER_PAGE);
        bkpgE = (laaEnd / FTL_LAA_BANK_PER_PAGE) + 1;
        bkpgS = (((bkpgS >> 5) << 5) > bkpgS) ? ((((bkpgS >> 5) - 1) << 5)) : (bkpgS);
        bkpgE = (((bkpgE >> 5) << 5) < bkpgS) ? ((((bkpgE >> 5) + 1) << 5)) : (bkpgE);

        //D_PRINTF("Trm=laaS[%x]bkpgS[%X][%x]-laaE[%x]bkpgE[%X][%x]\n", laaStartTrim, bkpgS, bkpgS >> 5, laaEnd, bkpgE, bkpgE >> 5);

        while (bkpgS <= bkpgE)
        {
            FTL_TRIM_SET_PBT_PG(bkpgS);
            bkpgS++;
        }

        // -----update Trim Bit map.
        // head
        while (((currLaa & 31) != 0) && (currLaa < laaEnd))
        {
            if (FTL_TRIM_CHK_BMP(currLaa))
                gFtlTrimInfo.trimBMPcnt--;

            FTL_TRIM_SET_BMP(currLaa);
            //D_PRINTF("HTrm[%x][%x]\n", currLaa >> 5, currLaa);
            currLaa++;
        }
        // middle
        if (((currLaa & 31) == 0) && (currLaa < ((laaEnd >> 5) << 5)))
        {
            chkbmpS = currLaa >> 5;
            chkbmpE = laaEnd >> 5;

            while (chkbmpS < chkbmpE)
            {
                gFtlTrimInfo.trimBMPcnt -= BIT_COUNT(gFtlTrimInfo.trimBMP[chkbmpS]);
                chkbmpS++;
            }

            //D_PRINTF("MTrim[%x][%x]S\n", currLaa >> 5, currLaa);
            MEM_SET(&gFtlTrimInfo.trimBMP[currLaa >> 5], BYTE_MASK, sizeof(U32) * ((((laaEnd >> 5) << 5) - currLaa) >> 5));
            currLaa += (((laaEnd >> 5) << 5) - currLaa);
            //D_PRINTF("MTrim[%x][%x]E\n", currLaa >> 5, currLaa);
        }
        //tail
        while (currLaa < laaEnd)
        {
            if (FTL_TRIM_CHK_BMP(currLaa))
                gFtlTrimInfo.trimBMPcnt--;

            FTL_TRIM_SET_BMP(currLaa);
            //D_PRINTF("TTrm[%x][%x]\n", currLaa >> 5, currLaa);
            currLaa++;
        }

        if (checkCache == TRUE)
        {
            tSSD* pSSD;

            currLaa = laaStartTrim;

            while (currLaa < laaEnd)   // should work with len = 0 case
            {
                cacheIdx = Cache_Search(currLaa);
                if (cacheIdx)
                {
                    // --- cache busy ---------------------
                    if (smCacheMgr.cacheEntry[cacheIdx].status & CACHE_S_ABORT_MASK)
                    {
                        // --- Uncomment below statement if WHQL failed! (might need to release all xferdone cache & wait fio job done)
                        //return CMSG_CMD_POSTPONE;
                    }
                    else
                    {
                        if (smCacheMgr.cacheEntry[cacheIdx].pool == SSD_POOL_FREE)
                        {
                            //D_PRINTF("Trim FREE LAA[%08X]\n", currLaa);

                            Cache_Delete(cacheIdx);
                            smCacheMgr.cacheEntry[cacheIdx].laa    = DWORD_MASK;
                            smCacheMgr.cacheEntry[cacheIdx].status = 0;
                            smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_FREE;
                            smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_INVALID;
                        }
                        else if (smCacheMgr.cacheEntry[cacheIdx].pool < SSD_POOL_COUNT)
                        {
                            pSSD = GET_SSD_PTR_FROM_INDEX(cacheIdx);
                            Cache_PopSSD(smCacheMgr.cacheEntry[cacheIdx].pool, pSSD, pSSD, 1);
                            Cache_Delete(cacheIdx);

                            smCacheMgr.cacheEntry[cacheIdx].laa    = DWORD_MASK;
                            smCacheMgr.cacheEntry[cacheIdx].status = 0;
                            smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_FREE;
                            smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_INVALID;

                            Cache_PushSSD(SSD_POOL_FREE, pSSD, pSSD, 1);
                        }
                        else
                        {
                            ASSERT(0);
                        }
                    }
                }

                currLaa++;
            }
        }
    }

    return fulldisc;
}

//-----------------------------------------------------------------------------
/**
    Process host msg return status

    @param[in]  pMsg

    @return     CMSG_CMD_POSTPONE if priority equal to 0
                CMSG_CMD_HALT if priority not qeual to 0
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS __inline Core_HostMsgReturn(MsgHostIO_t* pMsg)
{
    if (pMsg->hdr.b.prty == MSG_MAX_PRIORITY_NUM) return CMSG_CMD_HALT;
    pMsg->hdr.b.prty++;

    return CMSG_CMD_POSTPONE;
}

//-----------------------------------------------------------------------------
/**
    Process Host Read Command
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostRead(MsgHostIO_t* pMsg)
{
    #if _MEDIA_ >= MEDIA_NAND
    U32 laa;
    U32 laaEnd;
    U16 css;
    U16 cStatus;
    U16 hitCnt = 0;
    U16 cacheIdx;

    laaEnd = pMsg->laa + pMsg->bufDesc.ssdCnt;

#if (FTL_PARTIAL_LOAD_L2P != FALSE)
    if (gFtlPorPbtInfo.finishBuild != TRUE)
    {
        FTL_PorPbtLaaZone(pMsg->laa, laaEnd);
    }
#endif

    //--- Scan all LAAS
    for (laa = pMsg->laa; laa < laaEnd; laa++)
    {
        cacheIdx = Cache_Search(laa);

        if (cacheIdx)
        {
            cStatus = smCacheMgr.cacheEntry[cacheIdx].status;

            if (cStatus & CACHE_S_ABORT_MASK)
            {
                css = CACHE_ABORT;
                break;
            }
            else if (smCacheMgr.cacheEntry[cacheIdx].pool == SSD_POOL_FILLUP)
            {
                Core_ProcHitFillup(cacheIdx);
                css = CACHE_ABORT;
                break;
            }

            hitCnt++;
        }
    }

    if (css == CACHE_ABORT)
    {
        return Core_HostMsgReturn(pMsg);
    }

    //--- determine cache search status(CSS)
    if (hitCnt == 0)
    {
        tPAA paa;

        css = CACHE_R_FULL_HIT_BLANK;

        for (laa = pMsg->laa; laa < laaEnd; laa++)
        {
            paa = FTL_PhyAddrSearch(laa);

            if (paa.all32 != DWORD_MASK)
            {
                if (pMsg->hdr.b.opCode != cMcCompareCmd)
                {
                    css = CACHE_R_IDS;
                }
                else
                {
                    css = CACHE_R_NO_HIT;
                }
                break;
            }
        }
    }
    else if (hitCnt == pMsg->bufDesc.ssdCnt)
    {
        css = CACHE_R_FULL_HIT;
    }
    else
    {
        css = CACHE_R_PARTIAL_HIT;
    }

    //--- check current resource (free cache Count, free Job Count, available prp count)
    if (pMsg->bufDesc.ssdCnt > AU_CNT_SMALL_CMD)        // Large Cmd have more than 64 AUs, the NVMe allocate 128 PRPs
    {
        if (gCoreFreePrpCnt != CORE_PRP_COUNT)
        {
            return CMSG_CMD_HALT;
        }
    }
    else if (pMsg->bufDesc.ssdCnt > gCoreFreePrpCnt)    // Small Cmd, NVMe allocates the PRP entry for each AU
    {
        return CMSG_CMD_HALT;
    }

    if (css == CACHE_R_IDS)
    {
        if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] < ((pMsg->bufDesc.ssdCnt + IDS_MAX_PAA_PER_JOB - 1) / IDS_MAX_PAA_PER_JOB) + IDS_ALIGN_REVD_JOB)
        {
            return CMSG_CMD_HALT;
        }
    }
    else if (css < CACHE_R_FULL_HIT)
    {
        if (smCacheMgr.ssdCnt[SSD_POOL_FREE] <= pMsg->bufDesc.ssdCnt)
        {
            // TODO:: Evan, for better performance, do NOT set forceFlush flag,
            // device deadlock if someone occupy lots of cache and has no way to release them itself (ex. SSD_POOL_GC)
            // this issue will happen only when CACHE_SIZE is small than 2 * CACHE_FLUSH_THRESHOLD and SSD_POOL_GC enabled
            smCacheMgr.forceFillup = TRUE;
            return CMSG_CMD_HALT;
        }

        if (css == CACHE_R_PARTIAL_HIT && smCacheMgr.ssdCnt[SSD_POOL_OTHER] <= pMsg->bufDesc.ssdCnt)
        {
            return CMSG_CMD_HALT;
        }

        if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] == 0)
        {
            return CMSG_CMD_HALT;
        }
    }

    #if 0
    if (css == CACHE_R_NO_HIT || css == CACHE_R_PARTIAL_HIT || css == CACHE_R_IDS)  CmdSensor.lastPreReadLAA = CmdSensor.hitReadLAA = laaEnd;
    else if (css == CACHE_R_FULL_HIT_BLANK)                                         CmdSensor.lastPreReadLAA = DWORD_MASK;
    else                                                                            CmdSensor.hitReadLAA = laaEnd;
    #endif

    // ========= CACHE DATA PATH =================================
    // CACHE_R_NO_HIT           --> TrkDone -> XferDone
    // CACHE_R_PARTIAL_HIT      --> TrkDone -> XferDone
    // CACHE_R_IDS              --> TrkDone -> XferDone
    // CACHE_R_FULL_HIT         --> XferDone
    // CACHE_R_FULL_HIT_BLANK   --> XferDone
    //------------------------------------------------------------

    CORE_FREE_PRP_CNT_DEL(pMsg->bufDesc.ssdCnt);

    pMsg->bufDesc.css = css;

    gCbfCacheAlloc[css](pMsg);

    #else
    pMsg->bufDesc.ssdCnt = 1;
    pMsg->bufDesc.css    = CACHE_AUTO;

    Core_ProcHostXfer(pMsg);
    #endif

    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Process Host Write Command
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostWrite(MsgHostIO_t* pMsg)
{
    #if _MEDIA_ >= MEDIA_NAND
    U32 laa;
    U32 laaEnd;
    U16 css;
    U16 cStatus;
    U16 hitCnt  = 0;
    U16 cacheIdx;

    // Check if free cache is sufficient or not
    if (smCacheMgr.ssdCnt[SSD_POOL_FREE] < pMsg->bufDesc.ssdCnt)
    {
        // TODO:: Evan, for better performance, do NOT set forceFlush flag,
        // device deadlock if someone occupy lots of cache and has no way to release them itself (ex. SSD_POOL_GC)
        // this issue will happen only when CACHE_SIZE is small than 2 * CACHE_FLUSH_THRESHOLD and SSD_POOL_GC enabled
        smCacheMgr.forceFillup = TRUE;
        return CMSG_CMD_HALT;
    }

    #if (ENABLE_GC == TRUE)
    if (GC_MODE_CHECK)
    {
        if (GC_HostWrite(pMsg))
        {
            return CMSG_CMD_HALT;
        }
    }
    #endif

    laaEnd = pMsg->laa + pMsg->bufDesc.ssdCnt;

#if (FTL_PARTIAL_LOAD_L2P != FALSE)
    if (gFtlPorPbtInfo.finishBuild != TRUE))
    {
        FTL_PorPbtLaaZone(pMsg->laa, laaEnd);
    }
#endif

    //--- Scan all LAAS
    for (laa = pMsg->laa; laa < laaEnd; laa++)
    {
        cacheIdx = Cache_Search(laa);
        if (cacheIdx)
        {
            cStatus = smCacheMgr.cacheEntry[cacheIdx].status;
            if (cStatus & CACHE_S_ABORT_MASK)
            {
                if (GC_MODE_CHECK)
                {
                    GC_DECRE_HOST_WRITE(gGCInfo.curPpPtr, pMsg->bufDesc.ssdCnt);
                }

                return Core_HostMsgReturn(pMsg);
            }

            hitCnt++;
        }
    }

    //--- determine cache search status(CSS)
    if (hitCnt == pMsg->bufDesc.ssdCnt) css = CACHE_W_FULL_HIT;
    else if (hitCnt == 0)               css = CACHE_W_NO_HIT;
    else                                css = CACHE_W_PARTIAL_HIT;

    // ========= CACHE DATA PATH =================================
    // CACHE_W_NO_HIT           --> XferDone
    // CACHE_W_FULL_HIT         --> XferDone
    // CACHE_W_PARTIAL_HIT      --> XferDone
    //------------------------------------------------------------

    //--- switch css to FUA when fua bit set
    //if (pHcmdMsg->attr.b.fua)   pHcmdMsg->css = CACHE_W_FUA;
    pMsg->bufDesc.css = css;
    gCbfCacheAlloc[css](pMsg);

    #else
    pMsg->bufDesc.ssdCnt = 1;
    pMsg->bufDesc.css    = CACHE_AUTO;

    Core_ProcHostXfer(pMsg);
    #endif

    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Process Host TRIM Command
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostTrim(MsgTrim_t* pMsg)
{
    #if (_MEDIA_ >= MEDIA_NAND)
    FioTrimFmt_t *pFioDataSet;
    U32 checkCache = TRUE;
    U32 trimsize   = 0;
    U16 idx;
    U8  trimRelease=FALSE;

#if (THERMAL_THROTTLE == ENABLE)
    AplThermal_ThrottleException();
#endif

#if (FTL_PARTIAL_LOAD_L2P != FALSE)
    if (gFtlPorPbtInfo.finishBuild != TRUE)
    {
        FTL_BuildL2PZone(1, FTL_POR_PBT_ZONE_NUM - 1);
    }
#endif

    #if (ENABLE_GC == TRUE)
    if (GC_MODE_CHECK)
    {
        GC_HostTrim(pMsg);
    }
    #endif

    if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] != FIO_MAX_JOB)
    {
        return CMSG_CMD_HALT;
    }

    if (smCacheMgr.ssdCnt[SSD_POOL_FREE] != CACHE_RW_COUNT)
    {
        smCacheMgr.forceFillup = TRUE;
        smCacheMgr.forceFlush  = TRUE;

        return CMSG_CMD_HALT;
    }

    Cache_Reset(CACHE_RST_MODE_QUICK);
    checkCache = FALSE;

    pFioDataSet = (FioTrimFmt_t*)pMsg->pBuffer;

    for (idx = 0; idx < pMsg->entryCnt; idx++)
    {
        trimRelease = Core_TrimOperation(pFioDataSet[idx].laaStart, pFioDataSet[idx].laaEnd, checkCache);
        trimsize    = trimsize + (pFioDataSet[idx].laaEnd - pFioDataSet[idx].laaStart);
    }
    #endif

    if (gFtlMgr.QbtBackup != TRUE)
        gFtlMgr.QbtBackup = TRUE;

    #if 1
    if (trimsize <= FTL_DIRECT_TRIM_LAA_COUNT)  //trim size under 64GB.
    {
        for (idx = 0; idx < pMsg->entryCnt; idx++)
        {
            if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] < gFtlTrimRatio)
                FTL_DirectTrimUpdate(pFioDataSet[idx].laaStart, pFioDataSet[idx].laaEnd);
        }
    }
    #endif

    pMsg->hdr.b.status = cMsgCompleted;
    IPC_SendMsgQ(cC2hComQueue, (Msg_t*) pMsg);

    if (trimRelease)
    {
        FTL_PurgeCurrentHeader2Free();
        FTL_SetExternLAA();
    }

#if (ENABLE_GC == TRUE)
    if(GC_MODE_CHECK)
    {
        GC_STATE_CLEAN(GC_STATE_HALT);
    }
#endif

    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Process Host Flush Command
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostFlush(Msg_t* pMsg)
{
    #if (FTL_SPOR_FLUSH_DUMMY == TRUE)
    U16 dmpage = 0;
    U16 page   = 0;
    #endif

    if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] != FIO_MAX_JOB)
    {
        return CMSG_CMD_HALT;
    }

    #if (ENABLE_GC == TRUE)
    if (GC_MODE_CHECK)
    {
        if (GC_HostFlush(pMsg))
        {
            return CMSG_CMD_HALT;
        }
    }

    GC_STATE_SETTING(GC_STATE_HALT);
    #endif

    if (smCacheMgr.ssdCnt[SSD_POOL_FREE] != SSD_RW_COUNT)
    {
        smCacheMgr.forceFillup = TRUE;
        smCacheMgr.forceFlush  = TRUE;

        return CMSG_CMD_HALT;
    }

    #if (FTL_SPOR_FLUSH_DUMMY == TRUE)
    if (gFtlMgr.flushDummy && (!FTL_CURRENT_HEADER_NEW(FTL_BLOCK_HEADER_HOST, 0)))
    {
        page = gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].page;

		if (page == 0)                                dmpage = 2;
		else if(page >= (gNfInfo.pagesPerBlock - 5))  dmpage = gNfInfo.pagesPerBlock;
		else                                          dmpage = (page % 2) ? (page + 3) : (page + 2);

        D_PRINTF("Flush dm\tD[%d]BK[%4X]PG[%4X->%4X]\n", gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].bank, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].block, page, dmpage);
        //FTL_FlushDummyCloseBlock(FTL_BLOCK_HEADER_HOST, 0, dmpage, FALSE);
        FTL_FlushDummyCloseBlock(FTL_BLOCK_HEADER_HOST, 0, dmpage);
        D_PRINTF("Flush Dn\tD[%d]BK[%4X]PG[%4X]\n", gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].bank, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].block, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].page);
    }
    #endif

#if (GC_WORKAROUND == TRUE)
    D_PRINTF("GC ERROR[Feature OFF]: Host write + flush after a POR\n");
    ASSERT(FALSE);

    if (gCoreShutdownFlag)
    {
        AplThermal_ThrottleException(); // avoid flush timeout
        FTL_SaveQuickBuildTable();

        gCoreShutdownFlag = FALSE;
    }
    else
#endif
    {
        GC_STATE_CLEAN(GC_STATE_HALT);
    }

    pMsg->hdr.b.status = cMsgCompleted;

    if (pMsg->hdr.b.nc == FALSE)
    {
        IPC_SendMsgQ(cC2hComQueue, (Msg_t*) pMsg);
    }

    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Process Host Idle Flush Command

    Idle flush must inherit host flush characteristics and make sure there is no traffic at backend side
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostIdleFlush(Msg_t* pMsg)
{
    // TODO: shall we stop GC for idle flush?
    #if 0
    //#if (ENABLE_GC == TRUE)
    if (gGCppBuf[curP2lIdx].qos_Info.gcCacheCnt >= gGCppBuf[curP2lIdx].qos_Info.gcCacheTH)
        && (GCbuf[curP2lIdx].fsm.curState == GC_PP_ST_RECYCLING))
    {
        D_PRINTF("Idle Flush Abort[%4X] GC Cnt[%d]\n", GC_MODE_CHECK, gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_GC]);

        pMsg->error = cEcNoError;
        pMsg->hdr.b.status = cMsgCompleted;

        IPC_SendMsgQ(cC2hComQueue, (Msg_t*) pMsg);
        return CMSG_CMD_COMPLETE;
    }
    else
    #endif
    {
        return Core_HostFlush(pMsg);
    }
}

//-------------------------------------------------------------------
// Function     : static tCMSG_CMD_STATUS Core_ProcessHostTcgFunc(MsgHostIO_t* pHcmdMsg)
// Description  :
// Input        :
// return       :
//-------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostTcgFunc(MsgTcg_t* pMsg)
{
    #if _TCG_!= TCG_NONE
    A_HERE;
    Core_JobWait();         // all job should be finished in MSG queue.

    if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] && smCacheMgr.ssdCnt[SSD_POOL_OTHER] > 32 * 4)
    {
        TCGNFIF_Start(pMsg);
    }
    else
    {
        return CMSG_CMD_HALT;
    }
    #endif

    pMsg->hdr.b.status = cMsgCompleted;
    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Process Host Format Command
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostFormat(MsgFormat_t* pMsg)
{
    U32 checkCache   = TRUE;
    U8  trimfulldisc = FALSE;

#if (ENABLE_GC == TRUE)
    GC_HostFormat((MsgHostIO_t*)pMsg);
#endif

    if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] != FIO_MAX_JOB)
    {
        return CMSG_CMD_HALT;
    }

    if ((pMsg->laaEnd - pMsg->laaStart) > (L2P_NUM >> 1))
    {
        if (smCacheMgr.ssdCnt[SSD_POOL_FREE] != CACHE_RW_COUNT)
        {
            smCacheMgr.forceFillup = TRUE;
            smCacheMgr.forceFlush  = TRUE;

            return CMSG_CMD_HALT;
        }
        else
        {
            checkCache = FALSE;
            Cache_Reset(CACHE_RST_MODE_QUICK);
        }
    }

    if (pMsg->SES == 1)
    {
        Purge_Center(PURGE_CORE_FTL);
    }
    else
    {
        if (pMsg->laaEnd)
        {
            trimfulldisc = Core_TrimOperation(pMsg->laaStart, pMsg->laaEnd, checkCache);
            pMsg->laaEnd = 0;
        }
    }
    //FTL_SaveQuickBuildTable();
    //ECC_CHECK_DO_RESET();

    pMsg->hdr.b.status = cMsgCompleted;
    IPC_SendMsgQ(cC2hComQueue, (Msg_t*) pMsg);

    if (trimfulldisc)
    {
        FTL_PurgeCurrentHeader2Free();
        FTL_SetExternLAA();
        #if (ENABLE_GC == TRUE)
        GC_GlobalVarInit();
        #endif
    }

    FTL_SaveQuickBuildTable();

#if (ENABLE_GC == TRUE)
    GC_STATE_CLEAN(GC_STATE_HALT);
#endif

    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Process Host WriteZero Command
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostWriteZero(MsgWrtZero_t* pMsg)
{
    U32 trimFullDisk = FALSE;
    U32 laaS, laaE;

    if (pMsg->mbu_shift)
    {
        U16 laaSBmp, laaEBmp;

        laaSBmp = gSLaaBitMask[ pMsg->lba                 & pMsg->mbu_mask];
        laaEBmp = gELaaBitMask[(pMsg->lba + pMsg->nb - 1) & pMsg->mbu_mask];

        // convert to laa format for bitwise operation
        laaS =  pMsg->lba             >> pMsg->mbu_shift;
        laaE = (pMsg->lba + pMsg->nb) >> pMsg->mbu_shift;

        if (laaS == laaE)
        {
            laaSBmp &= laaEBmp;
            laaEBmp  = CACHE_BMP_ALL_VALID;
        }

        if (laaSBmp != CACHE_BMP_ALL_VALID)
        {
            if (Cache_WriteZero(laaS, laaSBmp) == FALSE)
            {
                return CMSG_CMD_POSTPONE;
            }
        }

        if (laaEBmp != CACHE_BMP_ALL_VALID)
        {
            if (Cache_WriteZero(laaE, laaEBmp) == FALSE)
            {
                return CMSG_CMD_POSTPONE;
            }
        }

        // convert to laa format for trum
        laaS = (pMsg->lba + pMsg->mbu_mask) >> pMsg->mbu_shift;
    }
    else
    {
        laaS = pMsg->lba;
        laaE = pMsg->lba + pMsg->nb;
    }

    if (laaS < laaE)
    {
        trimFullDisk = Core_TrimOperation(laaS, laaE, TRUE);
    }

    pMsg->hdr.b.status = cMsgCompleted;
    IPC_SendMsgQ(cC2hComQueue, (Msg_t*)pMsg);

    if (trimFullDisk)
    {
        FTL_PurgeCurrentHeader2Free();
        FTL_SetExternLAA();
    }

    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Process Host Reset Cache Command
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostResetCache(Msg_t* pMsg)
{
    switch (pMsg->param[0])
    {
        case RST_CACHE_INIT:
            #if _TCG_ == TCG_EDRV
                // Core init is early than BCM_OneTimeInit, only the msg issued by TcgInit() can call HAL_MRE_SecCopy()
                if (pMsg->hdr.b.subOpCode == MSG_TCG_INIT_ZEROCACHE || pMsg->hdr.b.subOpCode == MSG_TCG_REGEN_ZEROCACHE)
                {
                    U32 idx;

                    for (idx = 0; idx < L2P_EXT_NUM_ZERO; idx++)
                    {
                        HAL_MRE_SecCopy((U32)&smCache[(idx + SSD_ZERO_START) * MBU_SIZE], (U32)&smCache[SSD_ZERO_END * MBU_SIZE], MBU_SIZE, idx);
                    }
                }

                if (pMsg->hdr.b.subOpCode == MSG_TCG_INIT_ZEROCACHE && smSysInfo->d.FtlData.d.zeroTag == SI_TAG_ZERO) break;
            #endif

            pMsg->param[0] = RST_CACHE_CREATE;

        case RST_CACHE_CREATE:
        {
            tSSD* pHead = NULL;
            tSSD* pTail = NULL;
            U32 cacheIdx;
            U32 laa;

            if (smCacheMgr.ssdCnt[SSD_POOL_FREE] != CACHE_RW_COUNT)
            {
                smCacheMgr.forceFillup = TRUE;
                smCacheMgr.forceFlush  = TRUE;

                return CMSG_CMD_HALT;
            }

            #if _TCG_ == TCG_EDRV
                Cache_Reset(CACHE_RST_MODE_QUICK);
            #else
                for (laa = gExtLaaStart; laa < (gExtLaaStart + L2P_EXT_NUM_ZERO); laa++)
                {
                    cacheIdx = Cache_Search(laa);

                    if (cacheIdx)
                    {
                        Cache_Delete(cacheIdx);
                        smCacheMgr.cacheEntry[cacheIdx].laa    = DWORD_MASK;
                        smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_INVALID;
                        smCacheMgr.cacheEntry[cacheIdx].status = 0;
                        smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_FREE;
                    }
                }
            #endif

            pHead    = pTail = smCacheMgr.head[SSD_POOL_FREE];
            cacheIdx = GET_SSD_INDEX_FROM_PTR(pTail);

            for (laa = gExtLaaStart; laa < (gExtLaaStart + L2P_EXT_NUM_ZERO); laa++)
            {
                smCacheMgr.cacheEntry[cacheIdx].laa    = laa;
                smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_VALID;
                smCacheMgr.cacheEntry[cacheIdx].status = 0;
                smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_WRITE;
                Cache_Insert(cacheIdx);

                #if _TCG_ == TCG_EDRV
                if (pMsg->hdr.b.subOpCode == MSG_TCG_INIT_ZEROCACHE || pMsg->hdr.b.subOpCode == MSG_TCG_REGEN_ZEROCACHE)
                {
                    HAL_MRE_SecCopy((U32)&smCache[cacheIdx * MBU_SIZE], (U32)&smCache[SSD_ZERO_END * MBU_SIZE], MBU_SIZE, laa - gExtLaaStart);
                }
                #else
                    MEM_CLR(&smCache[cacheIdx * MBU_SIZE], MBU_SIZE);
                #endif

                pTail = GET_SSD_PTR_FROM_INDEX(cacheIdx);
                cacheIdx = pTail->nextIndex;
            }

            Cache_PopSSD(SSD_POOL_FREE,   pHead, pTail, L2P_EXT_NUM_ZERO);
            Cache_PushSSD(SSD_POOL_WRITE, pHead, pTail, L2P_EXT_NUM_ZERO);

            pMsg->param[0] = RST_CACHE_FLUSH;
        }
        case RST_CACHE_FLUSH:
            if (smCacheMgr.ssdCnt[SSD_POOL_FREE] != CACHE_RW_COUNT)
            {
                smCacheMgr.forceFillup = TRUE;
                smCacheMgr.forceFlush  = TRUE;

                return CMSG_CMD_HALT;
            }
            else
            {
                #if _TCG_ == TCG_EDRV
                    if (pMsg->hdr.b.subOpCode == MSG_TCG_INIT_ZEROCACHE || pMsg->hdr.b.subOpCode == MSG_TCG_REGEN_ZEROCACHE)
                    {
                        smSysInfo->d.FtlData.d.zeroTag = SI_TAG_ZERO;
                        SyIn_Synchronize(SI_AREA_FTL, SYSINFO_WRITE, SI_SYNC_BY_FTL);
                    }
                #else
                    smSysInfo->d.FtlData.d.zeroTag = SI_TAG_ZERO;
                    SyIn_Synchronize(SI_AREA_FTL, SYSINFO_WRITE, SI_SYNC_BY_FTL);
                #endif
            }

            break;
        default:
            ASSERT(0);
    }

    pMsg->hdr.b.status = cMsgCompleted;

    return CMSG_CMD_COMPLETE;
}

//-------------------------------------------------------------------
// Function     : static tCMSG_CMD_STATUS Core_ProcessHostPowerDown(MsgHostIO_t* pHcmdMsg)
// Description  :
// Input        :
// return       :
//-------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostPowerDown(MsgPM_t* pMsg)
{
    if ((smFioInfo.jobCnt[FIO_JOB_POOL_FREE] != FIO_MAX_JOB)
     || (smCacheMgr.ssdCnt[SSD_POOL_FREE] != CACHE_RW_COUNT))
    {
        pMsg->error = cEcPmDeviceBusy;
        pMsg->hdr.b.status = cMsgCompleted;
    }
    else
    {
        pMsg->hdr.b.status = cMsgCompleted;
        Core_SetPowerMode(pMsg->powerMode);
    }

    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Process Shutdown backup
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostShutdown(Msg_t* pMsg)
{
    // Force CPU clock to TEMP_MT_0
    AplThermal_ThrottleException();

    #if (ENABLE_GC == TRUE)
    if (GC_MODE_CHECK)
    {
       if (GC_HostShutDown((MsgHostIO_t*)pMsg))
       {
           return CMSG_CMD_POSTPONE;
       }
    }

    GC_STATE_SETTING(GC_STATE_HALT);
    #endif

    if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] != FIO_MAX_JOB)
    {
        return CMSG_CMD_POSTPONE;
    }

    if (smCacheMgr.ssdCnt[SSD_POOL_FREE] != CACHE_RW_COUNT)
    {
        smCacheMgr.forceFillup = TRUE;
        smCacheMgr.forceFlush  = TRUE;

        return CMSG_CMD_POSTPONE;
    }

    #if (ENABLE_GC == TRUE)
    if(gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_GCD] > GC_PING_PONG_NUM)
    {
        GC_ReleaseGcd();
    }
    #endif

    #if (CHK_FW_TYPE(FW_AGING) == FALSE)
    //if (gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].page & 0x0001)  //for spor debug
    FTL_SaveQuickBuildTable();
    #endif

    D_PRINTF("shutdown D[%d-%d]BK[%4X]PG[%3X]GCD[%d]\n", gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].bank & FTL_CE_MASK, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].bank >> FTL_CE_SHIFT, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].block, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].page, gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_GCD]);

    gCoreShutdownFlag = TRUE;

    pMsg->hdr.b.status = cMsgCompleted;

    if (pMsg->hdr.b.nc == FALSE)
    {
        IPC_SendMsgQ(cC2hComQueue, pMsg);
    }

    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Process Host SMART
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostSmartOp(MsgSmart_t* pMsg)
{
    switch(pMsg->subOp)
    {
        case cSmrAttri:
            Core_SmartAttrOperation();
            break;
        case cSmrSelfTestIni:
            D_PRINTF("SMART ST Init\n");
            gCoreDeviceSelfTest = 0;
            gCoreDSTCount       = 0;
            break;
        case cSmrSelfTestOp:
            D_PRINTF("SMART ST STC=%1X\n", pMsg->type);
            if (pMsg->type == 1)   gCoreDeviceSelfTest = 0x01;  //SHORT
            if (pMsg->type == 2)   gCoreDeviceSelfTest = 0x06;  //EXTENDED
            gCoreFirstSegmFail = 0;
            gCoreDSTCount      = 0;
            break;
        default:
            break;
    }

    pMsg->hdr.b.status = cMsgCompleted;

    if (pMsg->hdr.b.nc == FALSE)
    {
        IPC_SendMsgQ(cC2hComQueue, (Msg_t*) pMsg);
    }
    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Process SI_Sync Command
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostSI(MsgSI_t* pMsg)
{
    if (pMsg->syncby == SI_SYNC_BY_NOR)
    {
        SyIn_NOR_Synchronize(pMsg->op);
        pMsg->area         = 0;
        pMsg->hdr.b.status = cMsgCompleted;
        return CMSG_CMD_COMPLETE;
    }

    if (gSysInfoMgr.mgrBitMap & SI_S_BUSY_MASK)
    {
        return Core_HostMsgReturn((MsgHostIO_t*)pMsg);
    }
    else
    {
        if (smCacheMgr.ssdCnt[SSD_POOL_OTHER] > MAX_SSD_PER_SYSINFO && smFioInfo.jobCnt[FIO_JOB_POOL_FREE] > MAX_JOB_PER_SYSINFO)
        {
            if (SyIn_NAND_Synchronize(pMsg->area, (U8)pMsg->op, pMsg) == SYSINFO_ERR_NONRESOURCE)
            {
                return Core_HostMsgReturn((MsgHostIO_t*)pMsg);
            }
            else return CMSG_CMD_COMPLETE;
        }
        else
        {
            return Core_HostMsgReturn((MsgHostIO_t*)pMsg);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Process Purge Command
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostPurge(MsgPG_t* pMsg)
{
    Purge_Center(pMsg->pgBmp);
    pMsg->hdr.b.status = cMsgCompleted;
    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Process Host Set Feature
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostSetFeature(Msg_t* pMsg)
{
    switch(pMsg->hdr.b.subOpCode)
    {
        case cSetFeaManagementTemperature:
            if (pMsg->param[0]) gCurrMtTh.mt2_th = pMsg->param[0];
            if (pMsg->param[1]) gCurrMtTh.mt1_th = pMsg->param[1];
            break;
        default:
            break;
    }

    pMsg->hdr.b.status = cMsgCompleted;

    if (pMsg->hdr.b.nc == FALSE)
    {
        IPC_SendMsgQ(cC2hComQueue, (Msg_t*) pMsg);
    }
    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Process Vendor Command
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostVendor(MsgVsc_t* pMsg)
{
    U8*   pBuffer;
    tPAA  paa;
    U32   HwCheckPlpHealth;
    U32   subOpCode;
    U16   opCode;

    pBuffer   = pMsg->pBuffer;
    opCode    = pMsg->hdr.b.subOpCode;
    subOpCode = pMsg->param[0];
    paa.all32 = pMsg->paa.all32;

    switch (opCode)
    {
        case cSfnVscFwConfig:
            break;
        case cSfnVscReadIdAll:
        {
            MsgPhyRwe_t pHcmdMsg;
            MEM_CLR(&pHcmdMsg, sizeof(MsgPhyRwe_t));
            MSG_SET_SERVICE_INIT(pHcmdMsg, cSfnVscReadIdAll);
            pHcmdMsg.pBuffer = pBuffer;
            Ipc_SendWaitMsg(cC2mOtherQueue, (Msg_t *)&pHcmdMsg, (Msg_t *)&pHcmdMsg, IPC_NO_TIMEOUT);
            break;
        }
        case cSfnVscPreformat:
            Purge_Center(PURGE_CORE_FTL);
            break;
        case cSfnVscRdNandFlushCnt:
            Core_SmartAttrOperation();
            *((U64 *)pMsg->pBuffer) = smSysInfo->d.LogInfo.d.SmrLog.WCSFlushCnt;
            break;
        case cSfnVscReadECTable:
        {
            U32  tabMax = 0;
            U32  tabMin = U32_MAX;
            U32  tabAvg = 0;
            U16  blk;

            for (blk = smSysInfo->d.FtlData.d.StartIdx; blk <= smSysInfo->d.FtlData.d.EndIdx; blk++)
            {
                *((U16 *)pMsg->pBuffer + blk - smSysInfo->d.FtlData.d.StartIdx) = gFtlBlkListTbl[blk].eraseCnt;
                if (gFtlBlkListTbl[blk].eraseCnt > tabMax) tabMax = gFtlBlkListTbl[blk].eraseCnt;
                if (gFtlBlkListTbl[blk].eraseCnt < tabMin) tabMin = gFtlBlkListTbl[blk].eraseCnt;
                tabAvg += gFtlBlkListTbl[blk].eraseCnt;
            }
            tabAvg = tabAvg / (smSysInfo->d.FtlData.d.EndIdx - smSysInfo->d.FtlData.d.StartIdx + 1);
            *((U16 *)pMsg->pBuffer + smSysInfo->d.FtlData.d.EndIdx - smSysInfo->d.FtlData.d.StartIdx + 1) = tabMax;
            *((U16 *)pMsg->pBuffer + smSysInfo->d.FtlData.d.EndIdx - smSysInfo->d.FtlData.d.StartIdx + 1 + 2) = tabAvg;
            *((U16 *)pMsg->pBuffer + smSysInfo->d.FtlData.d.EndIdx - smSysInfo->d.FtlData.d.StartIdx + 1 + 4) = tabMin;
            break;
        }
        case cSfnVscReadVCTable:
        {
            U32  tabMax = 0;
            U32  tabMin = U32_MAX;
            U32  tabAvg = 0;
            U32  tmpvac;
            U16  blk;

            for (blk = smSysInfo->d.FtlData.d.StartIdx; blk <= smSysInfo->d.FtlData.d.EndIdx; blk++)
            {
                if (blk == gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][SSD_POOL_FREE].block)
                {
                    // dummy page
                    tmpvac = (gNfInfo.pagesPerBlock - (gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][SSD_POOL_FREE].page + gNfInfo.pagesPerProgUnit)) *
                              gNfInfo.planesPerLun * gNfInfo.totalLun;
                    // partial page
                    tmpvac += (gNfInfo.lunsPerChannel - gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][SSD_POOL_FREE].bank) *
                               gNfInfo.planesPerLun * gNfInfo.numChannels * gNfInfo.pagesPerProgUnit;
                    tmpvac *= gEcuInfo.auPerPage;
                    *((U32 *)pMsg->pBuffer + blk - smSysInfo->d.FtlData.d.StartIdx) = gFtlBlkListTbl[blk].vs.b.vac - tmpvac;
                }
                else if((gFtlBlkListTbl[blk].vs.b.status == FTL_BLOCK_POOL_BAD) || (blk == gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][SSD_POOL_FREE].block))
                {
                    *((U32 *)pMsg->pBuffer + blk - smSysInfo->d.FtlData.d.StartIdx) = 0;
                }
                else
                {
                    *((U32 *)pMsg->pBuffer + blk - smSysInfo->d.FtlData.d.StartIdx) = gFtlBlkListTbl[blk].vs.b.vac;

                }

                if(gFtlBlkListTbl[blk - smSysInfo->d.FtlData.d.StartIdx].vs.b.vac > tabMax) tabMax = gFtlBlkListTbl[blk - smSysInfo->d.FtlData.d.StartIdx].vs.b.vac;
                if(gFtlBlkListTbl[blk - smSysInfo->d.FtlData.d.StartIdx].vs.b.vac < tabMin) tabMin = gFtlBlkListTbl[blk - smSysInfo->d.FtlData.d.StartIdx].vs.b.vac;
                tabAvg += *((U32 *)pMsg->pBuffer + blk - smSysInfo->d.FtlData.d.StartIdx);
            }
            tabAvg = tabAvg / (smSysInfo->d.FtlData.d.EndIdx - smSysInfo->d.FtlData.d.StartIdx + 1) ;
            *((U32 *)pMsg->pBuffer + smSysInfo->d.FtlData.d.EndIdx - smSysInfo->d.FtlData.d.StartIdx + 1)     = tabMax;
            *((U32 *)pMsg->pBuffer + smSysInfo->d.FtlData.d.EndIdx - smSysInfo->d.FtlData.d.StartIdx + 1 + 1) = tabAvg;
            *((U32 *)pMsg->pBuffer + smSysInfo->d.FtlData.d.EndIdx - smSysInfo->d.FtlData.d.StartIdx + 1 + 2) = tabMin;
            break;
        }
        case cSfnVscSeq:
            Core_JobWait();
			CoreDbg_Center(subOpCode, 0, paa, 0, 0, 0);
            break;
        case cSfnVscHwCheckPLP:
            HwCheckPlpHealth = HalHwCheck_Main(HAL_HWCHECK_COMPONENT_PLP, PLP_HWCHECK_MODE_TEST, 0);

            if ((HwCheckPlpHealth) > PLP_DISCHARGE_TIMEOUT_MS) *((U8*)pMsg->pBuffer) = PLP_HWCHECK_OK;
            else                                               *((U8*)pMsg->pBuffer) = PLP_HWCHECK_FAIL;
            *(U32*)((U8*)pMsg->pBuffer + 1) = (HwCheckPlpHealth); // us to ms
            break;
        case cSfnVscSmBus:
            #if (CO_SUPPORT_SMBUS == TRUE)
            AplSmBus_Center(pMsg->param1, pMsg->pBuffer, pMsg->param[0], pMsg->param[1]);
            #endif
            break;
        default:
            break;
    }

    pMsg->hdr.b.status = cMsgCompleted;

    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Wait all IDS job done and return IO h2c for NVMe Reset
**/
//-----------------------------------------------------------------------------
static tCMSG_CMD_STATUS Core_HostReset(MsgHostIO_t* pMsg)
{
    tCMSG_CMD_STATUS    cmdStatus;
    U32                 cMsgCnt;
    U32                 idx;

    Core_JobWait();

    D_PRINTF("[RSET] Core_HostReset_JobWait\n");

    if (pMsg->hdr.b.nc == FALSE)
    {
        Core_ProcHostXfer(pMsg);
    }
    else
    {
        pMsg->hdr.b.status = cMsgCompleted;
    }

    while (IPC_MsgQFastPeek(cH2cReqQueue))
    {
        pMsg = (MsgHostIO_t*)IPC_GetMsgQ(cH2cReqQueue);
        pMsg->hdr.b.status = cMsgReceived;

        ASSERT(pMsg->hdr.b.opCode != cMcHostReset);

        if (gCoreProcHostMsgTable[pMsg->hdr.b.opCode].attr & CORE_MSG_ATTR_NVME_RESET)
        {
            if (pMsg->hdr.b.nc == FALSE)
            {
                pMsg->bufDesc.ssdCnt = 0;
                Core_ProcHostXfer(pMsg);
            }
            else
            {
                pMsg->hdr.b.status = cMsgCompleted;
            }
        }
        else
        {
            if (pMsg->hdr.b.prty == MSG_MAX_PRIORITY_NUM)    CMSG_PushHead(CMSG_POOL_WAIT, (Msg_t*)pMsg);
            else                                             CMSG_PushTail(CMSG_POOL_WAIT, (Msg_t*)pMsg);
        }
    }

    cMsgCnt = gCoreMsgInfo.msgCnt[CMSG_POOL_WAIT];
    for (idx = 0; idx < cMsgCnt; idx++)
    {
        pMsg = (MsgHostIO_t*)CMSG_PopHead(CMSG_POOL_WAIT);

        if (gCoreProcHostMsgTable[pMsg->hdr.b.opCode].attr & CORE_MSG_ATTR_NVME_RESET)
        {
            if (pMsg->hdr.b.nc == FALSE)
            {
                pMsg->bufDesc.ssdCnt = 0;
                Core_ProcHostXfer(pMsg);
            }
            else
            {
                pMsg->hdr.b.status = cMsgCompleted;
            }
        }
        else
        {
            cmdStatus = gCoreProcHostMsgTable[pMsg->hdr.b.opCode].fptr((Msg_t*)pMsg);
            if (cmdStatus == CMSG_CMD_POSTPONE)  CMSG_PushTail(CMSG_POOL_WAIT, (Msg_t*)pMsg);
            else if (cmdStatus == CMSG_CMD_HALT) CMSG_PushHead(CMSG_POOL_WAIT, (Msg_t*)pMsg);
        }
    }

    return CMSG_CMD_COMPLETE;
}

//-----------------------------------------------------------------------------
/**
    Core Reset
**/
//-----------------------------------------------------------------------------
static __inline void Core_ProcReset(void)
{

}

//-----------------------------------------------------------------------------
/**
    Pre read
**/
//-----------------------------------------------------------------------------
static __inline Error_t Core_ProcPreRead(void)
{
    #if 0
    if (CmdSensor.LastPreReadLAA == DWORD_MASK)
    {
        return cEcNoError;
    }

    if ((CmdSensor.lastPreReadLAA - CmdSensor.hitReadLAA) > 8 * CACHE_PREREAD_CNT)
    {
        return cEcNoError;
    }

    if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] && smCacheMgr.ssdCnt[SSD_POOL_FREE] > CACHE_PREREAD_CNT)
    {
        Cache_Alloc_PreRead(NULL);
    }
    #endif

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Process read cmd hit fillup cache
**/
//-----------------------------------------------------------------------------
static __inline void Core_ProcHitFillup(U16 cacheIdx)
{
    if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] && smCacheMgr.ssdCnt[SSD_POOL_OTHER] > MAX_SSD_PER_FILLUP)
    {
        Cache_Alloc_ReadFillupHit(cacheIdx);
    }
}

//-----------------------------------------------------------------------------
/**
    Force Fillup
**/
//-----------------------------------------------------------------------------
static __inline void Core_ProcForceFillup(void)
{
    if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] && (smCacheMgr.ssdCnt[SSD_POOL_OTHER] > MAX_SSD_PER_FILLUP) && smCacheMgr.ssdCnt[SSD_POOL_FILLUP])
    {
        Cache_Alloc_Fillup();
    }
    else
    {
        smCacheMgr.forceFillup = FALSE;
    }
}

//-----------------------------------------------------------------------------
/**
    Force Flush
**/
//-----------------------------------------------------------------------------
void Core_ProcForceFlush(void)
{
    if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE])
    {
        if (gFtlMgr.forceErase)
        {
            Core_ProcErase();
        }
        else
        {
            U32 header  = FTL_BLOCK_HEADER_HOST;
            U16 ssdPool = SSD_POOL_WRITE;

            if (smCacheMgr.ssdCnt[SSD_POOL_WRITE] < CACHE_FLUSH_THRESHOLD)
            {
                if (smCacheMgr.forceFlush)
                {
                    if (smCacheMgr.ssdCnt[SSD_POOL_GC])     Cache_MergePool(SSD_POOL_GC, SSD_POOL_WRITE);

                    if (smCacheMgr.ssdCnt[SSD_POOL_FILLUP] || smFioInfo.fillupJobCnt)
                    {
                        smCacheMgr.forceFillup = TRUE;
                        return;
                    }
                }
                else if (smCacheMgr.ssdCnt[SSD_POOL_GC] >= CACHE_FLUSH_THRESHOLD)
                {
                    ssdPool = SSD_POOL_GC;
                    header  = GC_FTL_HEADER;
                }
            }

            if (smCacheMgr.ssdCnt[ssdPool])
            {
                U16 group = 0;

                if (FTL_IS_LAST_PAGE(header, group))
                {
                    //last page flush
                    if (smCacheMgr.ssdCnt[SSD_POOL_OTHER] > FTL_FLUSH_BLT_P2L_PER_PAGE)
                    {
                        Core_ProcWriteLastPage(header, group);
                    }

                    #if(FTL_SPOR_FLUSH_DUMMY == TRUE)
    				if (gFtlMgr.flushDummy)
                    {
    					gFtlMgr.flushDummy = FALSE;
                    }
                    #endif
                }
                else
                {
                    if (FTL_IS_PBT_PAGE() && (gGCInfo.boot.flag == FALSE))
                    {
                        //PBT page flush
                        if (smCacheMgr.ssdCnt[SSD_POOL_OTHER] > FTL_FLUSH_BLT_P2L_PER_PAGE)
                        {
                            Core_ProcWritePartialBuildTable();
                        }
                    }
                    else
                    {
                        // Normal flush
                        tFIO_JOB *pJob;
                        tSSD     *pHead = NULL;
                        tSSD     *pTail = NULL;
                        U32       idx;
                        U16       cacheIdx;
                        U16       cacheCnt;

                        #if (FTL_QBT_BUILD == TRUE)
                        if (smSysInfo->d.FtlData.d.QBTBlockTag == SI_QBTINFO_TAG)
                        {
                            U32 currTick = 0;

                            currTick = HalTimer_ReadTick();
                            //FTL_PurgePool2Free(FTL_BLOCK_POOL_QBT);
                        #if (FTL_PARTIAL_LOAD_L2P != TRUE)
                            FTL_PurgePool2Free(FTL_BLOCK_POOL_PBT);
                            while (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_QBT])
                            {
                                FTL_BlockPopPushList(FTL_BLOCK_POOL_PBT, gFtlBlkListInfo.head[FTL_BLOCK_POOL_QBT], FTL_SORT_BY_SN);
                            }
                        #endif
                            smSysInfo->d.FtlData.d.QBTBlockTag = DWORD_MASK;
                            MEM_SET(gFtlQbtInfo,                        BYTE_MASK, sizeof(tFTL_QBT_INFO) * FTL_MAX_QBT_CNT);
                            MEM_SET(&smSysInfo->d.FtlData.d.QBTInfo[0], BYTE_MASK, sizeof(tFTL_QBT_INFO) * FTL_QBT_BLOCK_COUNT);
                            gFtlMgr.QbtBackup = TRUE;
                            SyIn_Synchronize(SI_AREA_FTL , SYSINFO_WRITE_FORCE, SI_SYNC_BY_FTL);
                            D_PRINTF("QBT clr[%d]ms\n", HalTimer_GetElapsedTimeMsec(currTick));
                        }
                        #endif

                        cacheCnt = smCacheMgr.ssdCnt[ssdPool];

                        #if (_NAND_ == NAND_3DTLCG2)
                        if (ssdPool == SSD_POOL_WRITE)
                        {
                            U16 pool;

                            if ((PLP_GET_STATUS() == PLP_DISCHARGE) || (smCacheMgr.ssdCnt[SSD_POOL_WRITE] < CACHE_MERGE_THRESHOLD))
                            {
                                for (pool = SSD_POOL_WRITE + 1; pool < SSD_POOL_WRITE + WRITE_CACHE_SPLIT_POOL_CNT; pool++)
                                {
                                    if (smCacheMgr.ssdCnt[pool])    Cache_MergePool(pool, SSD_POOL_WRITE);
                                }
                            }
                            else
                            {
                                for (pool = SSD_POOL_WRITE + 1; pool < SSD_POOL_WRITE + WRITE_CACHE_SPLIT_POOL_CNT; pool++)
                                {
                                    cacheCnt -= smCacheMgr.ssdCnt[pool];
                                }

                                for (pool = SSD_POOL_WRITE + 1; pool < SSD_POOL_WRITE + WRITE_CACHE_SPLIT_POOL_CNT; pool++)
                                {
                                    if (cacheCnt < smCacheMgr.ssdCnt[pool])
                                    {
                                        ssdPool  = pool;
                                        cacheCnt = smCacheMgr.ssdCnt[ssdPool];
                                    }
                                }
                            }
                        }
                        #endif

                        pHead    = smCacheMgr.head[ssdPool];
                        cacheIdx = GET_SSD_INDEX_FROM_PTR(pHead);

                        if (cacheCnt >= CACHE_FLUSH_ONE_CE_CNT)  cacheCnt = CACHE_FLUSH_ONE_CE_CNT;

                        pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
                        pJob->cmd                 = FIO_JOB_CMD_WRITE;
                        pJob->paaCnt              = CACHE_FLUSH_ONE_CE_CNT;
                        pJob->ssdIdx[SSD_W_CACHE] = cacheIdx;
                        pJob->ssdCnt[SSD_W_CACHE] = cacheCnt;
                        pJob->css                 = CACHE_FLUSH;
                        pJob->header              = header;
                        pJob->group               = group;

                        smFioInfo.flushPaaCnt += pJob->paaCnt;
                        gFtlPbtInfo.pageCnt++;

                        for (idx = 0; idx < cacheCnt; idx++)
                        {
                            smCacheMgr.cacheEntry[cacheIdx].status |= CACHE_S_SEQ_WRITE;
                            gFastLaaBuf[idx] = smCacheMgr.cacheEntry[cacheIdx].laa;
                            pTail    = &smSSD[cacheIdx];
                            cacheIdx = pTail->nextIndex;
                        }

                        Cache_PopSSD(ssdPool, pHead, pTail, cacheCnt);

                        if (cacheCnt < CACHE_FLUSH_ONE_CE_CNT)
                        {
                            pTail->nextIndex = CACHE_ZERO_START; //change nextIdx after pop SSD
                            MEM_SET(&gFastLaaBuf[cacheCnt], BYTE_MASK, sizeof(tPAA) * (CACHE_FLUSH_ONE_CE_CNT - cacheCnt));

                            #if (ENABLE_GC == TRUE)
                                if (GC_MODE_CHECK)  GC_COUNT_HOST_WRITE(gGCInfo.curPpPtr, CACHE_FLUSH_ONE_CE_CNT - cacheCnt);
                            #endif
                        }

                        if (gGCInfo.state.b.readOnly)
                        {
                            Cache_TrkDone_Flush(pJob);
                            FIO_PushJob(FIO_JOB_POOL_FREE, pJob);
                        }
                        else
                        {
                            FTL_FlushUpdate(pJob, TRUE);
                            Core_JobIssue(pJob);
                        }
                    }
                }
            }
            else
            {
                smCacheMgr.forceFlush = FALSE;
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Process Erase
**/
//-----------------------------------------------------------------------------
static __inline void Core_ProcErase(void)
{
    tFIO_JOB *pJob;

    pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
    pJob->cmd    = FIO_JOB_CMD_ERASE;
    pJob->paaCnt = gNfInfo.numChannels * gNfInfo.planesPerLun;

    FTL_EraseUpdate(pJob);

    Core_JobIssue(pJob);
}

//-----------------------------------------------------------------------------
/**
    Process write last page
**/
//-----------------------------------------------------------------------------
static __inline void Core_ProcWriteLastPage(U32 header, U8 group)
{
    tFIO_JOB    *pJob;
    tSSD        *pHead = NULL;
    U16         cacheIdx;

    //D_PRINTF("LASTPAGE_TABLE_SN ReleaseBlk[%d]tablesn[%x]\n", gFtlMgr.ReleaseBlk, sn);

    if (gFtlMgr.ReleaseBlk == TRUE)
    {
       FTL_ReleaseEmptyPool();
    }

    pHead                       = smCacheMgr.head[SSD_POOL_OTHER];
    cacheIdx                    = GET_SSD_INDEX_FROM_PTR(pHead);
    pJob                        = FIO_PopJob(FIO_JOB_POOL_FREE);
    pJob->ssdList               = SSD_WO_CACHE;
    pJob->ssdIdx[SSD_WO_CACHE]  = cacheIdx;
    pJob->cmd                   = FIO_JOB_CMD_WRITE;
    pJob->paaCnt                = CACHE_FLUSH_ONE_CE_CNT;
    pJob->css                   = CACHE_LASTPAGE;
    pJob->flag                  = FIO_F_NO_MKERR;
    pJob->header                = header;
    pJob->group                 = group;
    smFioInfo.flushPaaCnt += pJob->paaCnt;

    FTL_LastPageUpdate(pJob);

    MEM_SET(&gFastLaaBuf[0], BYTE_MASK, sizeof(U32) * CACHE_FLUSH_ONE_CE_CNT);

    FTL_FlushUpdate(pJob, TRUE);

    #if (FTL_LP_P2L_CHEK == TRUE)
    {
        U32 TIX = 0, TIX0 = 0, TIX1 = 0, as = FALSE;
        U16 PGT , BKT;
        U8  CHT, CET, LNT, PLT, FGT, DIET;
        U8  die, lun, ce, pp;

        die     = FTL_GET_DIE_NUM(lun, ce, 0);
        pp      = FTL_GET_LP_PP_IDX(lun, ce, pJob->header, pJob->group);
        group   = pJob->header * FTL_GROUP_NUM + pJob->group;
        TIX0    = FTL_GET_LP_IDX(group, pp, die, 0, 0, 0);

        die     = FTL_GET_DIE_NUM(lun, ce, 7);
        TIX1    = FTL_GET_LP_IDX(group, pp, die, 255, 3, 3);

        BKT     = pJob->paa[0].b.block;

        for (TIX = TIX0; TIX < TIX1; TIX++)
        {
            FGT  = TIX & 0x03;
            PLT  = (TIX >> FTL_FRAG_SHIFT) & 0x03;
            PGT  = (TIX >> (gNfInfo.bitsPlane + FTL_FRAG_SHIFT)) & 0xFF;
            DIET = (TIX >> (gFtlNandInfo.pages_shift + gNfInfo.bitsPlane + FTL_FRAG_SHIFT)) & 0x0F;
            CHT  =  DIET & 0x07;
            CET  = (DIET >> gNfInfo.bitsCh)    & 0x01;
            LNT  = (DIET >> gNfInfo.bitsChDev) & 0x01;

            if ((gFtlLastPageBuf.lastPageBufFtlP2L[TIX] > L2P_MASK) && (gFtlLastPageBuf.lastPageBufFtlP2L[TIX] != DWORD_MASK))
            {
                D_PRINTF("ERLAA:I[%d]LAA[%X]CH[%1X]CE[%1X]LN[%1X]PN[%1X]BK[%4X]PG[%3X]FG[%3X]pp[%d]G[%d]\n",
                    TIX, gFtlLastPageBuf.lastPageBufFtlP2L[TIX], CHT, CET, LNT, PLT, BKT, PGT, FGT, pp, group);

                as = TRUE;
            }
        }

        if (as == TRUE)
            ASSERT(0);

    }
    #endif

    Core_JobIssue(pJob);

    gFtlPbtInfo.pageCnt++;
    //D_PRINTF("LastPage.pageCnt[%2d]die[%d-%d]b[%4x]pg[%3x]\n", gFtlPbtInfo.pageCnt, pJob->paa[0].b.ce, pJob->paa[0].b.lun, pJob->paa[0].b.block, pJob->paa[0].b.page);
}

//-----------------------------------------------------------------------------
/**
    Process write partial build table
**/
//-----------------------------------------------------------------------------
static __inline void Core_ProcWritePartialBuildTable(void)
{
    tFIO_JOB *pJob;
    tPAA     paa;
    U32      idx;
    U32      sn;
    U8       storeFtlStage = FALSE;

    //D_PRINTF("HOST:D[%d-%d]BK[%4x]PG[%3x] PBT:D[%d-%d]BK[%4x]PG[%3x]\n",
    //    gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].bank & FTL_CE_MASK, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].bank >> FTL_CE_SHIFT, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].block, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].page,
    //    gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].bank & FTL_CE_MASK, gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].bank >> FTL_CE_SHIFT, gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].block, gFtlCurrHeader[FTL_BLOCK_HEADER_TABLE][0].page);

#if (FTL_PARTIAL_LOAD_L2P != FALSE)
    if ((gFtlPorPbtInfo.finishBuild != TRUE) && (gCoreShutdownFlag != TRUE))
    {
        FTL_BuildL2PZone(0, FTL_POR_PBT_ZONE_NUM);
    }
#endif

    pJob         = FIO_PopJob(FIO_JOB_POOL_FREE);
    pJob->header = FTL_BLOCK_HEADER_TABLE;
    pJob->group  = 0;
    paa          = FTL_GetFreePage(pJob);

    if ((gFtlPbtInfo.loopCnt == FTL_QBT_BLOCK_COUNT) && (paa.b.page == FTL_NAND_LAST_PAGE))
    {
        if (gFtlTrimInfo.trimUpdate != FALSE)
        {
            gFtlTrimInfo.trimUpdate = FALSE;
        }

        if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_EMPTY] > 0)
        {
            gFtlMgr.ReleaseBlk = TRUE;
        }

        if (gFtlTrimInfo.trimBlockcnt > 0)
        {
            MEM_CLR(gFtlTrimInfo.trimBlocks, sizeof(U8) * FTL_SUPPORT_MAX_BLK);
            MEM_CLR(gFtlNbtEmptyBlk, sizeof(U8)*FTL_SUPPORT_MAX_BLK);
            gFtlTrimInfo.trimBlockcnt       = 0;
            gFtlTrimInfo.trimSkipBlockcnt   = 0;
        }

        storeFtlStage = TRUE;
        FTL_CopyFtlInfoLastPageBuf();
        //FTL_DumpInfo();
    }
    else
    {
        // --- update L2P by Trim BMP.
        //D_PRINTF("PBT=TrimC[%x]upd[%d]bkc[%d]\n",
        //    gFtlTrimInfo.trimBMPcnt, gFtlTrimInfo.trimUpdate, gFtlTrimInfo.trimBlockcnt);

        if (gFtlTrimInfo.trimUpdate != FALSE)
        {
            FTL_PbtTrimUpdate(paa.b.page, paa.b.lun, paa.b.ce);
        }
    }

    pJob->ssdList              = SSD_WO_CACHE;
    pJob->ssdIdx[SSD_WO_CACHE] = GET_SSD_INDEX_FROM_PTR(smCacheMgr.head[SSD_POOL_OTHER]);
    pJob->cmd                  = FIO_JOB_CMD_WRITE;
    pJob->paaCnt               = CACHE_FLUSH_ONE_CE_CNT;
    pJob->css                  = CACHE_LASTPAGE;
    pJob->plane                = gNfInfo.planesPerLun;
    pJob->flag                 = FIO_F_NO_MKERR;

    sn = FTL_JOB_SN_SET(pJob->header, 0, gFtlPbtInfo.sn);

    for (idx = 0; idx < pJob->paaCnt; idx++)
    {
        gFastPaaBuf[idx]         = FTL_NANDWriteProcess(idx, paa, paa.b.page);
        gFastPaaBuf[idx].b.block = FTL_FastGetRMPBlk(gFastPaaBuf[idx].b.ch, gFastPaaBuf[idx].b.ce, gFastPaaBuf[idx].b.lun, gFastPaaBuf[idx].b.plane, paa.b.block);
        //D_PRINTF("[%03d]ch[%d]ce[%d]ln[%d]pn[%d]bk[%4x]pg[%2x]fg[%d]\n", idx, gFastPaaBuf[idx].b.ch, gFastPaaBuf[idx].b.ce, gFastPaaBuf[idx].b.lun, gFastPaaBuf[idx].b.plane, gFastPaaBuf[idx].b.block, gFastPaaBuf[idx].b.page, gFastPaaBuf[idx].b.frag);
        gFastAuxBuf[idx * AUX_BUF_MAX_CNT + AUX_BUF_IDX_LAA] = FTL_PBT_TABLE_TAG | gFtlPbtInfo.loopCnt;
        gFastAuxBuf[idx * AUX_BUF_MAX_CNT + AUX_BUF_IDX_SN ] = sn;
    }

    MemCopyNU32((U32*)pJob->aux, (U32*)gFastAuxBuf, pJob->paaCnt * META_DATA_SIZE);
    MemCopyNU32((U32*)pJob->paa, (U32*)gFastPaaBuf, pJob->paaCnt * sizeof(tPAA));

    if (storeFtlStage)
    {
        FTL_LastPageUpdate(pJob);
    }
    else
    {
        FTL_PbtPageUpdate(pJob);
    }

    if (gGCInfo.state.b.readOnly)
    {
        tSSD *pHead, *pTail;
        FIO_PushJob(FIO_JOB_POOL_FREE, pJob);
        smFioInfo.jobDoneCnt++;

        pHead = pTail = GET_SSD_PTR_FROM_INDEX(pJob->ssdIdx[SSD_WO_CACHE]);
        for (idx = 1; idx < pJob->ssdCnt[SSD_WO_CACHE]; idx++)
        {
            pTail = GET_SSD_PTR_FROM_INDEX(pTail->nextIndex);
        }

        Cache_PushSSD_Other(SSD_POOL_OTHER, pHead, pTail, pJob->ssdCnt[SSD_WO_CACHE]);
    }
    else
    {
        Core_JobIssue(pJob);
    }

    gFtlPbtInfo.pageCnt = 0;
    //D_PRINTF("PbtPage.pageCnt[%2d]die[%d-%d]b[%4x]pg[%3x]\n", gFtlPbtInfo.pageCnt, pJob->paa[0].b.ce, pJob->paa[0].b.lun, pJob->paa[0].b.block, pJob->paa[0].b.page);
}

//-----------------------------------------------------------------------------
/**
    Handle Host Cache Release

**/
//-----------------------------------------------------------------------------
static __inline void Core_ProcReleaseCache(void)
{
    MsgCacheRelObj_t* pCacheMsg;

    pCacheMsg = (MsgCacheRelObj_t*) IPC_GetMsgQ(cH2cCacheRelQueue);

    gCbfCacheXferDone[pCacheMsg->bufDesc.css](pCacheMsg);

    pCacheMsg->hdr.b.status = cMsgCompleted;
}

//-----------------------------------------------------------------------------
/**
    Wait for heap job done
**/
//-----------------------------------------------------------------------------
void Core_ProcWaitHeapJob(void)
{
    tFIO_JOB* pJob;
    tPAA* HeapPaa;

    pJob = smFioInfo.head[FIO_JOB_POOL_HEAP];

    HeapPaa = (tPAA*)Heap_Alloc(pJob->paaCnt + (pJob->flag & FIO_F_AUX) * 2 * pJob->paaCnt);

    if (HeapPaa != NULL)
    {
        if (pJob->cmd < FIO_JOB_CMD_ERASE)
        {
            if (pJob->css == CACHE_AUTO)
            {
                Cache_Alloc_Auto(pJob);   // Auto SSD Allocation
            }
        }

        pJob = FIO_PopJob(FIO_JOB_POOL_HEAP);
        MEM_COPY(HEAP_GET_PTR_CPU1_B0TCM(HeapPaa), pJob->paa, sizeof(tPAA) * pJob->paaCnt);
        pJob->paa = HeapPaa;

        if (pJob->flag & FIO_F_AUX)
        {
            if (pJob->cmd == FIO_JOB_CMD_WRITE)
            {
                MEM_COPY(HEAP_GET_PTR_CPU1_B0TCM(&HeapPaa[pJob->paaCnt]), pJob->aux, sizeof(tPAA) * pJob->paaCnt * 2);
            }
            pJob->aux = (U32*)(&HeapPaa[pJob->paaCnt]);
        }

        IPC_SendMsgQ(cC2mReqQueue, (Msg_t*)pJob);
        smFioInfo.jobIssueCnt++;
    }
}

//-----------------------------------------------------------------------------
/**
    Process job completion from media cpu
**/
//-----------------------------------------------------------------------------
void Core_ProcJobComplete(void)
{
    tFIO_JOB* pJob;

    pJob = (tFIO_JOB*) IPC_GetMsgQ(cM2cComQueue);

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

    if (pJob->status & FIO_ERR_HANDLE_MASK)
    {
        gCbfCacheTrkError[pJob->css](pJob);
    }
    else
    {
        gCbfCacheTrkDone[pJob->css](pJob);
    }

    FIO_PushJob(FIO_JOB_POOL_FREE, pJob);

    smFioInfo.jobDoneCnt++;
}

//-----------------------------------------------------------------------------
/**
    Process Host Xfer
**/
//-----------------------------------------------------------------------------
void Core_ProcHostXfer(MsgHostIO_t* pMsg)
{
    #if _MEDIA_ == MEDIA_DRAM
    {
        pMsg->bufDesc.ssdIndex                  = pMsg->attrInfo.b.cmdId;
        pMsg->cmdInfo.pBuffer                   = ((U32)smCacheRamDrive + pMsg->laa * MBU_SIZE) >> SSD_ADDR_SHIFT;
        smSSD[pMsg->bufDesc.ssdIndex].startAddr = (U32)pMsg->cmdInfo.pBuffer;
        smSSD[pMsg->bufDesc.ssdIndex].endAddr   = 0xFFFFFFF0;
    }
    #endif

    pMsg->hdr.b.status = cMsgExecuted;
    IPC_SendMsgQ(cC2hComQueue, (Msg_t*) pMsg);
}

//-----------------------------------------------------------------------------
/**
    Core issue reset cache message for RZAT
**/
//-----------------------------------------------------------------------------
void Core_SendResetCacheMsg(void)
{
    Msg_t* pMsg;
    pMsg = CMSG_GetExtendMsg(cMcResetCache);

    if (pMsg)
    {
        //D_PRINTF("\033[7;31m[RZAT]\033[m\n");
        D_PRINTF("[RZAT] Reset\n");
        pMsg->hdr.b.opCode = cMcResetCache;
        pMsg->param[0]     = RST_CACHE_INIT;
        CMSG_PushTail(CMSG_POOL_WAIT, (Msg_t*)pMsg);
    }
}

//-----------------------------------------------------------------------------
/**
    Core task entry function
**/
//-----------------------------------------------------------------------------
void Core_StartTask(void)
{
    InitBootMode_t initMode;

    initMode = Hal_GetInitBootMode();

    Core_OneTimeInit(initMode);

    #if CHK_FW_TYPE(FW_AGING)
        Core_ServiceLoopAging();
    #else
        Core_ServiceLoop();
    #endif
}

/*______________________________________________________________________________
   ____                 ____                  _            _
  / ___|___  _ __ ___  / ___|  ___ _ ____   _(_) ___ ___  | |    ___   ___  _ __
 | |   / _ \| '__/ _ \ \___ \ / _ \ '__\ \ / / |/ __/ _ \ | |   / _ \ / _ \| '_ \
 | |___ (_) | | |  __/  ___) |  __/ |   \ V /| | (__  __/ | |___ (_) | (_) | |_) |
  \____\___/|_|  \___| |____/ \___|_|    \_/ |_|\___\___| |_____\___/ \___/| .__/
                                                                           |_|
________________________________________________________________________________*/
// Please do NOT break this while loop
static void Core_ServiceLoop(void)
{
    Msg_t*             pMsg = NULL;
    tCMSG_CMD_STATUS   cmdStatus;

    D_PRINTF("[Core] ServiceLoop\n");

    while (TRUE)
    {
        #if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
        while (PM_CORE_LOCK_REQUEST())
        {
            if (smPmCtrl[CPU_ID2].PowerMode != cDevicePmActive)
            {
                if ((smFioInfo.jobCnt[FIO_JOB_POOL_FREE] != FIO_MAX_JOB) ||
                    (smCacheMgr.ssdCnt[SSD_POOL_FREE] != CACHE_RW_COUNT))
                {
                    smPmCtrl[CPU_ID2].LockStatus = cPmDeviceBusy;
                    smPmCtrl[CPU_ID2].PowerMode  = cDevicePmActive;
                }
                else
                {
                    Core_SetPowerMode(smPmCtrl[CPU_ID2].PowerMode);
                }
            }
        }
        #endif

        // Handle host request command fetching
        if (IPC_MsgQFastPeek(cH2cReqQueue) && PLP_GET_STATUS())
        {
            pMsg = IPC_GetMsgQ(cH2cReqQueue);
            pMsg->hdr.b.status = cMsgReceived;

            if (gCoreMsgInfo.msgCnt[CMSG_POOL_WAIT])
            {
                if (pMsg->hdr.b.prty == MSG_MAX_PRIORITY_NUM)    CMSG_PushHead(CMSG_POOL_WAIT, pMsg);
                else                                             CMSG_PushTail(CMSG_POOL_WAIT, pMsg);
            }
            else
            {
                cmdStatus = gCoreProcHostMsgTable[pMsg->hdr.b.opCode].fptr(pMsg);
                if (cmdStatus == CMSG_CMD_POSTPONE)  CMSG_PushTail(CMSG_POOL_WAIT, pMsg);
                else if (cmdStatus == CMSG_CMD_HALT) CMSG_PushHead(CMSG_POOL_WAIT, pMsg);
            }
        }

        // Process Host message command
        if (gCoreMsgInfo.msgCnt[CMSG_POOL_WAIT] && PLP_GET_STATUS())
        {
            pMsg = CMSG_PopHead(CMSG_POOL_WAIT);

            cmdStatus = gCoreProcHostMsgTable[pMsg->hdr.b.opCode].fptr(pMsg);
            if (cmdStatus == CMSG_CMD_POSTPONE)  CMSG_PushTail(CMSG_POOL_WAIT, pMsg);
            else if (cmdStatus == CMSG_CMD_HALT) CMSG_PushHead(CMSG_POOL_WAIT, pMsg);
        }

        #if (FIO_JOB_HEAP_MEMORY == TRUE)
        // Process WaitHeapJob
        if (smFioInfo.jobCnt[FIO_JOB_POOL_HEAP] && PLP_GET_STATUS())
        {
            Core_ProcWaitHeapJob();
        }
        #endif

        // Media Done
        if (IPC_MsgQFastPeek(cM2cComQueue) && PLP_GET_STATUS())
        {
            Core_ProcJobComplete();
        }

        // Handle release host xfer cache
        if (IPC_MsgQFastPeek(cH2cCacheRelQueue) && PLP_GET_STATUS())
        {
            Core_ProcReleaseCache();
        }

        #if (ENABLE_GC == TRUE)
        if (((GC_STATE_CHECK & GC_STATE_STOP_MASK) == FALSE) &&
            (GC_MODE_CHECK_ACTIVE) && PLP_GET_STATUS())
        {
            GC_main();
        }
        #endif

        // Fillup
        if (smCacheMgr.ssdCnt[SSD_POOL_FILLUP] >= CACHE_FILLUP_THRESHOLD || smCacheMgr.forceFillup)
        {
            Core_ProcForceFillup();
        }

        // Flush
        if ((smCacheMgr.ssdCnt[SSD_POOL_WRITE] >= (CACHE_FLUSH_THRESHOLD)) ||
            (smCacheMgr.ssdCnt[SSD_POOL_GC]    >= (CACHE_FLUSH_THRESHOLD)) ||
            smCacheMgr.forceFlush || gFtlMgr.forceErase)
        {
            Core_ProcForceFlush();
        }

        #if (THERMAL_THROTTLE == ENABLE)
        // Thermal Throttle
        if (smThermalTempUpdate && PLP_GET_STATUS())
        {
            AplThermal_ThrottleEntry();
        }
        #endif

        #if (CO_SUPPORT_DEVICE_SELF_TEST == TRUE)
        if (gCoreDeviceSelfTest && PLP_GET_STATUS())
        {
            Core_ProcDeviceSelfTest();
        }
        #endif

        // PLP
        #if (CO_SUPPORT_PLP == TRUE)
        if (PLP_GET_STATUS() == PLP_DISCHARGE)
        {
            Core_ServiceLoopPLP();
        }
        #endif
    }
}

/*__________________________________________________________________________________
  _____  _      _____     _____                 _          _
 |  __ \| |    |  __ \   / ____|               (_)        | |
 | |__) | |    | |__) | | (___   ___ _ ____   ___  ___ ___| |     ___   ___  _ __
 |  ___/| |    |  ___/   \___ \ / _ \ '__\ \ / / |/ __/ _ \ |    / _ \ / _ \| '_ \
 | |    | |____| |       ____) |  __/ |   \ V /| | (_|  __/ |___| (_) | (_) | |_) |
 |_|    |______|_|      |_____/ \___|_|    \_/ |_|\___\___|______\___/ \___/| .__/
                                                                            | |
                                                                            |_|
____________________________________________________________________________________*/
// For PLP Only, please do NOT jump to this service loop arbitrary!!!
static void Core_ServiceLoopPLP(void)
{
    U32 counter = 0;

    PLP_SET_STATUS(CPU_ID0, PLP_DISCHARGE);
    PLP_SET_STATUS(CPU_ID1, PLP_DISCHARGE);

    while (TRUE)
    {
        // Need to keep monitor release cache queue for write xferdone cache
        while (IPC_MsgQFastPeek(cH2cCacheRelQueue))
        {
            Core_ProcReleaseCache();
        }

        // Media Done
        while (IPC_MsgQFastPeek(cM2cComQueue))
        {
            Core_ProcJobComplete();
        }

        // Create fillup job first!
        if (smCacheMgr.ssdCnt[SSD_POOL_FILLUP])
        {
            Core_ProcForceFillup();
        }

        // better to wait for all fillup job completed before Flash
        if ((smFioInfo.fillupJobCnt == 0) && smCacheMgr.ssdCnt[SSD_POOL_WRITE])
        {
            Core_ProcForceFlush();
        }

        if (smCacheMgr.ssdCnt[SSD_POOL_WRITE] || smCacheMgr.ssdCnt[SSD_POOL_FILLUP] ||
           ((smFioInfo.jobCnt[FIO_JOB_POOL_FREE] + smFioInfo.jobCnt[FIO_JOB_POOL_FREEZE]) != FIO_MAX_JOB) ||
            smMsgQueue[cH2cCacheRelQueue].writeCount != smMsgQueue[cH2cCacheRelQueue].readCount)
        {
            continue;
        }
        else
        {
            if (!counter) D_PRINTF("[PLP ] DoneGpio|0x%4x\n", HalGpio_ReadAllPin());

            // Fake PLP
            if ((HalGpio_ReadPin(PLP_TESTPIN_NO_PLOSS) == TRUE) && (HalGpio_ReadPin(PLP_TESTPIN_NO_SPG) == TRUE))
            {
                PLP_SET_STATUS(CPU_ID0, PLP_CHARGE);
                PLP_SET_STATUS(CPU_ID1, PLP_CHARGE);
                PLP_SET_STATUS(CPU_ID2, PLP_CHARGE);

                smCacheMgr.forceFlush  = FALSE;
                smCacheMgr.forceFillup = FALSE;
                D_PRINTF("[PLP ] FakePLP\n");

                break;
            }

            HalTimer_DelayUsec(10);
            counter++;

            // Is this really necessary?
            if (counter > (PLP_TIMEOUT_VALUE_US / 10))
            {
                ASSERT(FALSE);
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Aging service loop
**/
//-----------------------------------------------------------------------------
#if CHK_FW_TYPE(FW_AGING)
void Core_ServiceLoopAging(void)
{
    Msg_t*              pMsg = NULL;
    tCMSG_CMD_STATUS    cmdStatus;
    KernelEventFlags_t  events;
    HwTimerTick_t tick;
    U32 wait_time;

    D_PRINTF("[Core] ServiceLoopAging\n");

    tick = HalTimer_ReadTick();

    while (TRUE)
    {
        // Handle host request command fetching
        if (IPC_MsgQFastPeek(cH2cReqQueue))
        {
            pMsg = IPC_GetMsgQ(cH2cReqQueue);
            pMsg->hdr.b.status = cMsgReceived;

            CMSG_PushTail(CMSG_POOL_WAIT, pMsg);
        }

        // Process Host message command
        if (gCoreMsgInfo.msgCnt[CMSG_POOL_WAIT])
        {
            pMsg = CMSG_PopHead(CMSG_POOL_WAIT);
            pMsg->error = cEcNoError;
            cmdStatus = gCoreProcHostMsgTable[pMsg->hdr.b.opCode].fptr(pMsg);

            if (cmdStatus == CMSG_CMD_POSTPONE)  CMSG_PushTail(CMSG_POOL_WAIT, pMsg);
            else if (cmdStatus == CMSG_CMD_HALT) CMSG_PushHead(CMSG_POOL_WAIT, pMsg);
        }

        // Handle release host xfer cache
        if (IPC_MsgQFastPeek(cH2cCacheRelQueue))
        {
            Core_ProcReleaseCache();
        }

        // Fillup
        if (smCacheMgr.ssdCnt[SSD_POOL_FILLUP] >= CACHE_FILLUP_THRESHOLD || smCacheMgr.forceFillup)
        {
            Core_ProcForceFillup();
        }

        // Flush
        if (smCacheMgr.ssdCnt[SSD_POOL_WRITE] >= CACHE_FLUSH_THRESHOLD || smCacheMgr.forceFlush)
        {
            Core_ProcForceFlush();
        }

        // Media Done
        if (IPC_MsgQFastPeek(cM2cComQueue) && PLP_GET_STATUS())
        {
            Core_ProcJobComplete();
        }

        wait_time = HalTimer_GetElapsedTimeMsec(tick);

        if (((smSysInfo->d.AgingData.d.Aging_Signature == AGING_SIGN) || (smSysInfo->d.AgingData.d.Aging_Mode == ENABLE)) && (smSysInfo->d.AgingData.d.Aging_Identify  == DISABLE))
        {
            if (gdwAgEnFrag == AGING_AGEN)
            {
                if ((wait_time <= 180000) && (wait_time % 26 == 0))
                {
                    D_PRINTF("[AG  ] Wait|%d\n",wait_time);
                }

                if (wait_time > 180000)
                {
                    gdwAgEnFrag = DISABLE;
                    Aging_MainLoop();
                }
            }
        }
    }
}
#endif

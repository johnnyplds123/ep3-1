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
//! @brief Serial monitor core command handler\n
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "Xmodem.h"

#include "RegPcie.h"
#include "RegPciePhy.h"
#include "RegNvmePkg.h"

#include "HalPcieLink.h"
#include "HalNvmeXfer.h"
#include "HalHwCheck.h"

#include "HostBase.h"
#include "NvmeCmds.h"
#include "NvmeIdentify.h"
#include "Cache.h"
#include "Fio.h"
#include "CmdSensor.h"
#include "MemTest.h"
#include "UncTable.h"
#include "AdaptiveVoltageScaling.h"
#include "BcmClient.h"

#include "NormSection.h"
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define MON_LOCK_CPU FALSE

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static Error_t MonCmd_Help(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_DispInfo(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_DispVersion(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_ReadMem (Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_WriteMem(Cstr_t pCmdStr, U32 argc, U32 argv[]);

static Error_t MonCmd_EraseAllSflash(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_EraseSflash   (Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_ProgramSflash (Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_ReadSflash    (Cstr_t pCmdStr, U32 argc, U32 argv[]);

static Error_t MonCmd_FwDownload(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_FwActivate(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_OtfUpdate (Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_Reboot    (Cstr_t pCmdStr, U32 argc, U32 argv[]);

static Error_t MonCmd_PsMode(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_GpioPin(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_Plp(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_TempSensor(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_StressDram(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_SweepDram(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_UncTable(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_ReadSoC(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_CpuLock(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmd_CpuUnlock(Cstr_t pCmdStr, U32 argc, U32 argv[]);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
const MonCmdTbl_t monCmdList[] =
{
//    cmd string,   help string,                    min-argc, auto-de,c exe_time, handeler
    { "HELP",       "List Cmds",                    0, ON,  OFF, MonCmd_Help            },
    { "INFO",       "Dump Info",                    0, ON,  OFF, MonCmd_DispInfo        },
    { "VERSION",    "Dump Version",                 0, ON,  OFF, MonCmd_DispVersion     },

    { "RMEM",       "Mem Read",                     1, ON,  OFF, MonCmd_ReadMem         },
    { "WMEM",       "Mem Write",                    2, ON,  OFF, MonCmd_WriteMem        },

    { "EASFLASH",   "SFlash Erase Chip",            0, ON,  ON,  MonCmd_EraseAllSflash  },
    { "ESFLASH",    "SFlash Erase Sector",          2, ON,  ON,  MonCmd_EraseSflash     },
    { "PSFLASH",    "SFlash Prog",                  3, ON,  ON,  MonCmd_ProgramSflash   },
    { "RSFLASH",    "SFlash Read",                  2, ON,  OFF, MonCmd_ReadSflash      },

    { "DOWNFW",     "FW Download",                  0, ON,  OFF, MonCmd_FwDownload      },
    { "FWACT",      "FW Activate",                  1, ON,  OFF, MonCmd_FwActivate      },
    { "OTF",        "FW OTF Update",                0, ON,  OFF, MonCmd_OtfUpdate       },
    { "REBOOT",     "Reboot",                       0, ON,  OFF, MonCmd_Reboot          },

    { "PS",         "Power State",                  0, ON,  OFF, MonCmd_PsMode          },

    { "PIN",        "GPIO",                         1, ON,  OFF, MonCmd_GpioPin         },
    { "PLP",        "PLP",                          0, ON,  OFF, MonCmd_Plp             },
    { "TEMP",       "OnDie Temp.Sensor",            0, ON,  ON,  MonCmd_TempSensor      },
    { "SD",         "DRAM Stress",                  0, ON,  ON,  MonCmd_StressDram      },
    { "SWP",        "DRAM Sweep",                   0, ON,  ON,  MonCmd_SweepDram       },
    { "UNCT",       "Unc Table",                    0, ON,  OFF, MonCmd_UncTable        },
    { "AVS",        "Read SoC",                     0, ON,  OFF, MonCmd_ReadSoC         },
    #if (MON_LOCK_CPU == TRUE)
    { "LOCK",       "Lock CPU",                     0, ON,  OFF, MonCmd_CpuLock         },
    { "UNLOCK",     "Unlock CPU",                   0, ON,  OFF, MonCmd_CpuUnlock       },
    #endif
    { "",           "",                             0, OFF, OFF, NULL                   } // EOT
};

/*
static U32 gCallLoopCount = 1; // call loop count for performance monitoring.
static U32 sConnectedCpu  = 0;
*/

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern void Host_DumpInfo(U32 allFlag);

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Monitor command handler -Display all registered monitor commands.

    HELP

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_Help(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    Mon_PrintCmdList();

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - Display system information.

    INFO

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used


    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_DispInfo(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    NvmeApstEntry_t *pent;
    U32 op = 0;
    U32 idx;
    U8  flag = FALSE;

    if (argc != 0)
    {
        op = argv[0];
    }

    Debug_DumpVersion();

    M_PRINTF("\n[ITCM] ");
    MemAlloc_DumpAo(&itcmAllocatorAo);
    M_PRINTF("[DTCM] ");
    MemAlloc_DumpAo(&dtcmAllocatorAo);
    M_PRINTF("[STCM] ");
    MemAlloc_DumpAo(&smStcmAllocatorAo);
    M_PRINTF(" [SPM] ");
    MemAlloc_DumpAo(&smSspmAllocatorAo);
    M_PRINTF("[DRAM] \n");
    MemAlloc_DumpVs(&smDramAllocatorVs);

    Hal_DumpInfo();

    Kernel_DumpInfo();

    M_PRINTF("ACMD_Cnt[%04d]  IOCMD_Cnt[%04d]\n", ADMIN_CMD_FIFO_CNT(), IO_CMD_FIFO_CNT());

    M_PRINTF("FW_Active_Slot[%d], History[%08d]\n", smActiveFwSlot, smFwHistoryNumber);

    M_PRINTF("%X %X?\n",    gHostIfMgr.info.cmdFetchCnt, gHostIfMgr.info.cmdDoneCnt);
    M_PRINTF("%X %X\n",     gHostIfMgr.info.mediaReqJobCnt, gHostIfMgr.info.mediaReqDoneCnt);
    M_PRINTF("%X %X\n",     gHostIfMgr.info.ioXferReqCnt, gHostIfMgr.info.ioXferDoneCnt);
    M_PRINTF("%X\n",        ADMIN_CMD_FIFO_CNT());
    M_PRINTF("%X\n",        IO_CMD_FIFO_CNT());
    M_PRINTF("%X %X\n",     smFioInfo.jobIssueCnt, smFioInfo.jobDoneCnt);
    M_PRINTF("%X\n",        smCacheMgr.ssdCnt[SSD_POOL_FREE]);
    M_PRINTF("%X\n",        smCacheMgr.ssdCnt[SSD_POOL_FILLUP]);
    M_PRINTF("%X\n",        smCacheMgr.ssdCnt[SSD_POOL_WRITE]);

    M_PRINTF("[APST] ==========================\n");
    for (idx = 0; idx < 32; idx++)
    {
        pent = (NvmeApstEntry_t *)&(gNvmeIfMgr.AutoPST[idx]);
        if (pent->IdleMs)
        {
            flag = TRUE;
            M_PRINTF("Idx[%02d]: %dms go PS%d\n", idx, pent->IdleMs, pent->PS);
        }
    }
    if ((flag == FALSE) || (!currentFeatureValue[cAutonomousPowerStateTransition]))
    {
        M_PRINTF("No APST request\n");
    }

    if (op == 1)
    {
        Host_DumpInfo(TRUE);
    }
    else if (op == 2)
    {
        Host_DumpInfo(2);
    }
    else
    {
        Host_DumpInfo(FALSE);
    }

    AplCmdSensor_DumpInfo();

    D_PRINTF("Namespaces Nr_Create:%d, Nr_Attach:%d\n", gNvmeIfMgr.numOfNamespacesCreated, gNvmeIfMgr.numOfNamespacesAttached);
    for (idx = 0; idx <= HW_MAX_NAMESPACES; idx++)
    {
        D_PRINTF("NSID[%d]: LBAF:%d NSZE:%08X_%08X MBUS:%08X, MBUE:%08X C:%d A:%d\n", idx, gNvmeIfMgr.ns[idx].LBAF,
                                                                             U64_TO_U32_H(gNvmeIfMgr.ns[idx].NSZE), U64_TO_U32_L(gNvmeIfMgr.ns[idx].NSZE),
                                                                             gNvmeIfMgr.ns[idx].mbu_start, gNvmeIfMgr.ns[idx].mbu_end,
                                                                             gNvmeIfMgr.ns[idx].Create, gNvmeIfMgr.ns[idx].Attach);
    }

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - Display version information.

    VERSION

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used


    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_DispVersion(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    U8  MN[40];    ///< Model Number       (bytes[63:24])
    U8  FR[8];     ///< Firmware Revision  (bytes[71:64])

    Debug_DumpVersion();

    MEM_COPY(MN, &LiteOnModelName[0],  40);
    MEM_COPY(FR, &LiteOnFWRevision[0], 8);
    MN[40 - 1] = 0;
    FR[8  - 1] = 0;

    M_PRINTF("MD|%s\n", MN);
    M_PRINTF("FW|%s\n", FR);

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - Read and dump CPU0's memory data as specified data type.

    RMEM address [count=2] [dataSize=2]

    dataSize : 1=BYTE, 2=WORD, 4=DWORD

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        argument count
    @param[in]  argv        argument values list pointer
                            argv[0] : address
                            argv[1] : count
                            argv[2] : dataSize

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_ReadMem(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    U32 address;
    U32 count;
    U32 (*ReadCallBack)(U32);
    U32 dataSize;

    address  =  argv[0];
    count    = (argc>1) ? argv[1] : 2;
    dataSize = (argc>2) ? argv[2] : 2;

    switch(dataSize)
    {
    case 4:
        ReadCallBack = MemReadU32;
        break;
    case 2:
        ReadCallBack = MemReadU16;
        break;
    default:
        ReadCallBack = MemReadU8;
        break;
    }

    PutData(pCmdStr[1], address, count, dataSize, ReadCallBack);

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler -  Write a data to CPU0's memory.

    WMEM address data [dataSize=2]

    dataSize : 1=BYTE, 2=WORD, 4=DWORD\n

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        argument count
    @param[in]  argv        argument values list pointer
                            argv[0] : address
                            argv[1] : data
                            argv[2] : dataSize

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_WriteMem(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    U32 address;
    U32 data;
    U32 dataSize;

    address  =  argv[0];
    data     =  argv[1];
    dataSize = (argc>2) ? argv[2] : 2;

    switch(dataSize)
    {
    case 4:
        MemWriteU32(address, data);
        break;
    case 2:
        MemWriteU16(address, data);
        break;
    default:
        MemWriteU8(address, data);
        break;
    }

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - erase all serial flash memory data.

    EASFLASH

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_EraseAllSflash(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    return (HalSflash_EraseChip(3));
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - erase sector serial flash memory data.

    ESFLASH

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        argument count
    @param[in]  argv        argument values list pointer
                            argv[0] : address
                            argv[1] : count

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_EraseSflash(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    return (HalSflash_EraseSector((SflashSector_t)argv[0], argv[1], 1));
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - program serial flash memory data.

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        argument count
    @param[in]  argv        argument values list pointer
                            argv[0] : bufferAddress
                            argv[1] : flashAddress
                            argv[2] : count

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_ProgramSflash(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    return HalSflash_Program((void*)argv[0], argv[1], argv[2]);
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - read and dump serial flash memory data.

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        argument count
    @param[in]  argv        argument values list pointer
                            argv[0] : address
                            argv[1] : count

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_ReadSflash(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    U32 count;

    count = (argc > 1) ? argv[1] : 2;

    PutData('F', argv[0], count, 1, HalSflash_ReadU32);

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - download firmware code.

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        argument count
    @param[in]  argv        argument values list pointer
                            argv[0] : address

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_FwDownload(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    DfwModuleInfo_t*    pFwModule;
    DfwDescript_t*      pFwDesc;
    DfwHeader_t*        fwHeader;
    U8*                 pBuffer;
    U8*                 pTagAddress;
    char*               fwTag[4] = {""};
    DfwImageTag_t       Img_Tag;
    Error_t             error = cEcNoError;
    U32                 receivedSize;
    U32                 orgState;
    U32                 fw_addr = 0;
    U32                 slotnum = 1;
    U32                 dFwTimeoutCnt = 0;
    U32                 calculatedCRC32 = 0;
    U32                 receivedCRC32 = 0;
    U8                  fwBlkCnt = 0;
    U8                  slot_Num = 0;
    U8                  dFwFcFlag = ENABLE;

    Mon_PrintPrompt();

    if (argc != 0 && argv[0] <= NVME_NUM_OF_FW_SLOTS)
    {
        slotnum = argv[0];
    }
    else
    {
        if (smActiveFwSlot == 1)    slotnum = 2;
        else                        slotnum = 1;
    }

    #if (CHK_FW_TYPE(FW_DNLD) == FALSE)
    if (argv[1] == 0xFC)            dFwFcFlag = ENABLE;
    else                            dFwFcFlag = DISABLE;
    #endif

    DFW_PRINTF("Mode:%s FwSlot:%d\n", (dFwFcFlag) ? "Force" : "Normal", slotnum);

    pBuffer = (U8*)(DIRECT_DRAM_ADDR);   // Todo have to allocate memory

    MEM_CLR(pBuffer, FW_IMAGE_SIZE);
    MEM_CLR(&Img_Tag, sizeof(Img_Tag));

    orgState = HalIrq_EnDisable(cIrqUartRx, DISABLE);

    error = XmodemReceive(pBuffer, SHARED_SPM_SIZE, &receivedSize);

    HalIrq_EnDisable(cIrqUartRx, orgState);

    if (error != cEcNoError) return(error);

    HalTimer_DelayMsec(500);

    receivedCRC32 = *(U32*)((U8*)pBuffer + receivedSize - 4);
    calculatedCRC32 = ~(MemCrcU32(U32_MAX, pBuffer, receivedSize - 4));

    error |= DFW_GET_ERROR_ST(pBuffer, receivedCRC32, calculatedCRC32);

    D_PRINTF("\n");
    if (error && dFwFcFlag == DISABLE)
    {
        D_PRINTF("[HMOD] Program FW Bin Fail:%s\n", (error == B_0) ? "FW_Header"    :
                                                    (error == B_1) ? "BOOT_5A"      :
                                                    (error == B_2) ? "DRAM_Type"    :
                                                    (error == B_3) ? "NAND_Vendor"  :
                                                    (error == B_4) ? "NAND_Type"    :
                                                    (error == B_5) ? "CRC"          : "Other");
        D_PRINTF("[HMOD] DRAM_TYPE|%02x NAND_VENDOR|%02x NAND_TYPE|%02x *FORCE_DL:argv[1]|0xFC\n", _DRAM_, _VENDOR_, _NAND_);
        return cEcError;
    }
    else
    {
        if (error == B_0) // No FW Header Case / Only Download Boot Code
        {
            HalSflash_EraseSector(DFW_BOOT_OFFSET / SFLASH_SECTOR_SIZE, (DFW_BOOT_MAX_SIZE + DFW_MAIN_MAX_SIZE) / SFLASH_SECTOR_SIZE, 3);
            error = AplLoadFirmware_SaveToSPI(DFW_BOOT_OFFSET, DFW_BOOT_MAX_SIZE + DFW_MAIN_MAX_SIZE, pBuffer, SFLASH_PAGE_SIZE, 5);
            if (error == cEcNoError)
            {
                D_PRINTF("[HMOD] DFW PASS:No Header Only (B+M) Code\n");
            }
            return error;
        }
    }
    //----------------------------------------
    //  Get FW description & FW module point
    //----------------------------------------
    pFwDesc = (DfwDescript_t*)((U8*)(DIRECT_DRAM_ADDR + DFW_FW_CSS_SIZE));
    pFwModule = (DfwModuleInfo_t*)((U8*)(DIRECT_DRAM_ADDR + DFW_FW_CSS_SIZE + DFW_FW_DESC_SIZE));

    //----------------------------------------
    //  Save firmware to SPI
    //----------------------------------------
    pBuffer = (U8*)(DIRECT_DRAM_ADDR + DFW_FW_HEAD_SIZE);

    for (fwBlkCnt = 0; fwBlkCnt < pFwDesc->sec_cnt; fwBlkCnt++)
    {
        if ((pFwModule[fwBlkCnt].tag & 0xFFFF) == DFW_FW_MAFW_TAG)
        {
            slot_Num = DFW_GET_FWNUM(pFwModule[fwBlkCnt]);
            if (slot_Num == slotnum)
            {
                pFwModule[fwBlkCnt].update = ENABLE;
            }
        }

        if (pFwModule[fwBlkCnt].update)
        {
            *(U32*)fwTag = pFwModule[fwBlkCnt].tag;
            D_PRINTF("[HMOD] FW %s\n", fwTag);
            HalSflash_EraseSector(pFwModule[fwBlkCnt].offset / SFLASH_SECTOR_SIZE, ALIGN_CEIL(pFwModule[fwBlkCnt].size, SFLASH_SECTOR_SIZE) / SFLASH_SECTOR_SIZE, 3);
            error = AplLoadFirmware_SaveToSPI(pFwModule[fwBlkCnt].offset , ALIGN_CEIL(pFwModule[fwBlkCnt].size, SFLASH_SECTOR_SIZE) , pBuffer, SFLASH_PAGE_SIZE, 5);
            if ((pFwModule[fwBlkCnt].tag & 0xFFFF) == DFW_FW_MAFW_TAG && error == cEcNoError)
            {
                // Program FW Tag
                if (slot_Num == slotnum)
                {
                    MEM_CLR(&gIfBuffer, DFW_FW_TAG_SIZE);
                    // copy 4K for tag because handle by program fail case
                    MEM_COPY(&gIfBuffer, pBuffer, SFLASH_SECTOR_SIZE);
                    Img_Tag.mfw_crc = ~(MemCrcU32(U32_MAX, pBuffer + sizeof(DfwImageTag_t), pFwModule[fwBlkCnt].size - 4 - sizeof(DfwImageTag_t)));
                    Img_Tag.id = DFW_FW_IMAGE_TAG;
                    pTagAddress = (U8*)((DIRECT_DRAM_ADDR) + DFW_FW_HEAD_SIZE + pFwModule[0].size - 0x10);    // FW revision information is mark at boot code
                    MEM_COPY((void*) &Img_Tag.fw_revision[0], pTagAddress, 8);
                    smFwHistoryNumber ++;
                    Img_Tag.historyNum = smFwHistoryNumber;
                    MEM_COPY(&gIfBuffer, &Img_Tag, sizeof(Img_Tag));
                }
                D_PRINTF("[HMOD] FW Tag\n");
                error = AplLoadFirmware_SaveToSPI(pFwModule[fwBlkCnt].offset, SFLASH_SECTOR_SIZE, gIfBuffer, SFLASH_PAGE_SIZE, 5);
            }
        }

        // Dram offset
        if ((pFwModule[fwBlkCnt].tag & 0xFFFF) != DFW_FW_MAFW_TAG || pFwModule[fwBlkCnt].tag == DFW_FW_BKFW_TAG) //slot_Num == slotnum
        {
            pBuffer += pFwModule[fwBlkCnt].size;
        }

        if (error != cEcNoError) break;
    }

    //----------------------------------------
    //  If program pass then update select and FW Header
    //----------------------------------------
    if (error == cEcNoError)
    {
        // Program FW Select
        fwHeader = (DfwHeader_t *)gIfBuffer;
        MEM_CLR(fwHeader, DFW_FW_HEAD_SIZE);
        fwHeader->DfwSelectTag.id = DFW_FW_SELECT_TAG;
        fwHeader->DfwSelectTag.select = slotnum;
        MEM_COPY(&fwHeader[0].DfwDescript, pFwDesc, sizeof(DfwDescript_t));
        MEM_COPY(&fwHeader[1], pFwModule, sizeof(DfwModuleInfo_t) * pFwDesc->sec_cnt);
        D_PRINTF("[HMOD] FW HD BkUp\n");
        // Backup
        HalSflash_EraseSector(DFW_BACKUP_HEAD_OFFSET / SFLASH_SECTOR_SIZE, DFW_FW_HEAD_SIZE / SFLASH_SECTOR_SIZE, 3);
        error = AplLoadFirmware_SaveToSPI(DFW_BACKUP_HEAD_OFFSET, DFW_FW_HEAD_SIZE, (U8*)&gIfBuffer, SFLASH_PAGE_SIZE, 5);
        D_PRINTF("[HMOD] FW HD Main\n");
        // Current
        HalSflash_EraseSector(DFW_CURRENT_HEAD_OFFSET / SFLASH_SECTOR_SIZE, DFW_FW_HEAD_SIZE / SFLASH_SECTOR_SIZE, 3);
        error = AplLoadFirmware_SaveToSPI(DFW_CURRENT_HEAD_OFFSET, DFW_FW_HEAD_SIZE, (U8*)&gIfBuffer, SFLASH_PAGE_SIZE, 5);

        D_PRINTF("[HMOD] FW Pass ^_^\n");
    }

    if (error != cEcNoError)
    {
        D_PRINTF("[HMOD] FW Fail o_O\n");
    }

    return error;
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - firmware activation.

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        argument count
    @param[in]  argv        argument values list pointer
                            argv[0] : active fw slot

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_FwActivate(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    DfwHeader_t*    pFwHeader;
    U8*             pBuffer;
    Error_t         error = cEcNoError;
    U32             slotnum = 1;

    if (argc != 0)
    {
        slotnum = argv[0];
    }
    else
    {
        if (smActiveFwSlot == 1)
        {
            slotnum = 2;
        }
        else
        {
            slotnum = 1;
        }
    }

    D_PRINTF("[HMOD] slot|%02d\n", slotnum);

    if (slotnum != smActiveFwSlot && (smFwValidStatus & BIT(slotnum - 1)))
    {
        pBuffer = (U8*) (DIRECT_DRAM_ADDR);
        AplLoadFirmware_GetFwInfo(DFW_CURRENT_HEAD_OFFSET, DISABLE, &gLdFw_FwInfo[0]);
        MEM_CLR(pBuffer, gLdFw_FwInfo[slotnum - 1].size);
        HalSflash_Copy(pBuffer, gLdFw_FwInfo[slotnum - 1].address, gLdFw_FwInfo[slotnum - 1].size);
        HalSflash_EraseSector(gLdFw_FwInfo[FW_MAIN_BK_NUM - 1].address/ SFLASH_SECTOR_SIZE, ALIGN_CEIL(gLdFw_FwInfo[slotnum - 1].size, SFLASH_SECTOR_SIZE)/ SFLASH_SECTOR_SIZE, 3);
        D_PRINTF("[HMOD] MainFw Bk Update\n");
        error |= AplLoadFirmware_SaveToSPI(gLdFw_FwInfo[FW_MAIN_BK_NUM - 1].address, ALIGN_CEIL(gLdFw_FwInfo[slotnum - 1].size, SFLASH_SECTOR_SIZE), pBuffer, SFLASH_PAGE_SIZE, 5);
    }

    pFwHeader = (DfwHeader_t *)gIfBuffer;
    // Program FW Select
    MEM_CLR(pFwHeader, DFW_FW_HEAD_SIZE);
    HalSflash_Copy(pFwHeader, DFW_CURRENT_HEAD_OFFSET, DFW_FW_HEAD_SIZE);
    pFwHeader->DfwSelectTag.id = DFW_FW_SELECT_TAG;
    pFwHeader->DfwSelectTag.select = slotnum;
    D_PRINTF("[HMOD] FwHd BkUp\n");
    // Backup
    HalSflash_EraseSector(DFW_BACKUP_HEAD_OFFSET / SFLASH_SECTOR_SIZE, DFW_FW_HEAD_SIZE / SFLASH_SECTOR_SIZE, 3);
    error |= AplLoadFirmware_SaveToSPI(DFW_BACKUP_HEAD_OFFSET, DFW_FW_HEAD_SIZE, (U8*)pFwHeader, SFLASH_PAGE_SIZE, 5);
    D_PRINTF("[HMOD] FwHd Main\n");
    // Current
    HalSflash_EraseSector(DFW_CURRENT_HEAD_OFFSET / SFLASH_SECTOR_SIZE, DFW_FW_HEAD_SIZE / SFLASH_SECTOR_SIZE, 3);
    error |= AplLoadFirmware_SaveToSPI(DFW_CURRENT_HEAD_OFFSET, DFW_FW_HEAD_SIZE, (U8*)pFwHeader, SFLASH_PAGE_SIZE, 5);

    return error;
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - reboot information backup.

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_OtfUpdate(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    if (argv[0] < 2)
    {
        NvmeCmd_OTFUpdateDump(argv[0]);
    }
    else if (argv[0] == 2)
    {
        NvmeCmd_OTFUpdateBackup();
    }
    else if (argv[0] == 3)
    {
        NvmeCmd_OTFUpdateRestore();
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - reboot device.

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_Reboot(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    if (argv[0]) return HalCpu_Reboot(0);

    NvmeCmd_BootFromNewFW();

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - Power State Function.

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_PsMode(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    U32 idx;

    M_PRINTF("PS[%d] APST [%d]\n", currentFeatureValue[cPowerManagement], currentFeatureValue[cAutonomousPowerStateTransition]);
    M_PRINTF("byTimer:%d\n", gPmInfo.bypassTimerValue);

    for (idx = 0; idx < NVME_NUM_OF_POWER_STATES; idx++)
    {
        M_PRINTF("PS[%d]: %d ms\n", gNvmeIfMgr.AutoPST[idx].PS, gNvmeIfMgr.AutoPST[idx].IdleMs);
    }
    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - Test Pin Function.

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_GpioPin(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    switch (argv[0])
    {
    case 0:     // Output
        HalGpio_WritePin(argv[1], argv[2]);
        D_PRINTF("TP0: 0x%x\n", HalGpio_ReadAllPin());
        break;
    case 1:     // Input
        D_PRINTF("TP1: 0x%x\n", HalGpio_ReadAllPin());
        break;
    case 2:     // Ouput Enable
        HalGpio_OutputEnable(argv[1]);
        D_PRINTF("TP2: 0x%x\n", HalGpio_ReadAllPin());
        break;
    case 3:     // Ouput Disable
        HalGpio_OutputDisable(argv[1]);
        D_PRINTF("TP3: 0x%x\n", HalGpio_ReadAllPin());
        break;
    case 4:
        HalGpio_OutputSetMux(argv[1]);
        D_PRINTF("TP4: 0x%x\n", HalGpio_ReadAllPin());
        break;
    default:
        break;
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - PLP Function.

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_Plp(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    D_PRINTF("[HMON] PLPMode|%d\n", argv[0]);

    #if (CO_SUPPORT_PLP == TRUE)
    HalHwCheck_Main(HAL_HWCHECK_COMPONENT_PLP, argv[0], argv[1]);
    #endif

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - Get Temperature

    @param[in]  pCmdStr     command line string
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmd_TempSensor(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    D_PRINTF("[HMON] TempSensor|%d\n", HalTempSensor_GetTemperature());

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - Stress Dram

    @param[in]  pCmdStr     command line string
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
extern U32 DdrDll_DataVerify(U32 startAddr, U32 cycles, U32 mask);
static Error_t MonCmd_StressDram(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    U32 loop;

    if (argc < 1)
    {
        D_PRINTF("[HMON] Usage: ds loop\n");
        return (cEcNoError);
    }
    D_PRINTF("[HMON] StressDram %d\n", argv[0]);

    for (loop = 0; loop < argv[0]; loop++)
    {
        //MemTestRandU32((U32 *)DRAM_TEST_START_ADDRESS0, DRAM_TEST_SIZE, loop, NULL, 4);
        //MemTestRandU32((U32 *)DRAM_TEST_START_ADDRESS1, DRAM_TEST_SIZE, loop, NULL, 4);
        DdrDll_DataVerify(DRAM_TEST_START_ADDRESS0, 10, DWORD_MASK);
        DdrDll_DataVerify(DRAM_TEST_START_ADDRESS1, 10, DWORD_MASK);
    }

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Monitor command handler - Sweep Dram

    @param[in]  pCmdStr     command line string
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
extern void HalDdrDll_Sweep(U8 byteIndex, U8 store, U8 update);
static Error_t MonCmd_SweepDram(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    U32 loop;

    if (argc < 2)
    {
        D_PRINTF("[HMON] Usage: swp byte loop\n");
        return (cEcNoError);
    }
    D_PRINTF("[HMON] SweepDram B|%d L|%d\n", argv[0], argv[1]);

    for (loop = 0; loop < argv[1]; loop++)
    {
        HalDdrDll_Sweep(argv[0], FALSE, FALSE);
    }

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - UNC Table Function.

    UNCT

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_UncTable(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    if (argc == 0)
    {
       return cEcNoError;
    }

    #if (UNC_TABLE_ENABLE == ENABLE)
    if      (argv[0] == 1)  UNCT_CHECK_DO_INSERT(argv[1], argv[2], UNCT_NDTP_WUNC);
    else if (argv[0] == 2)  UNCT_CHECK_DO_DELETE(argv[1], argv[2], ENABLE);
    else if (argv[0] == 3)  UNCT_Dump();
    else if (argv[0] == 4)  UNCT_CHECK_DO_RESET();
    #endif

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Lock CPU Function.

    LOCK

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        0->CPU ID

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_CpuLock(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    Msg_t           msg[2];
    U8              lockAll = FALSE;

    MEM_CLR(msg, sizeof(Msg_t) * 2);
    smHoldCpuX = TRUE;

    if (argc == 0) return cEcNoError;

    if (argv[0] == 1 || argv[0] == 0xA)
    {
        MSG_SET_SERVICE_INIT(msg[0], cSfnHoldCpu1);
        Ipc_SendWaitMsg(cH2mReqQueue, &msg[0], &msg[0], IPC_NO_TIMEOUT);
    }

    if (argv[0] == 2 || argv[0] == 0xA)
    {
        MSG_SET_SERVICE_INIT(msg[1], cSfnHoldCpu2);
        Ipc_SendWaitMsg(cH2cUartQueue, &msg[1], &msg[1], IPC_NO_TIMEOUT);
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Unlock CPU Function.

    UNLOCK

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_CpuUnlock(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    smHoldCpuX = FALSE;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Read SoC Type

    ReadSoC

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmd_ReadSoC(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    if (argc == 0)
    {
       return cEcNoError;
    }
    #if ( CO_SUPPORT_READSOC == TRUE )
    if (argv[0] == 0)
    {
        uint32_t vddSetPoint = 0;

        if (!AVSData.isBCMInitial)
        {
            BCM_OneTimeInit((InitBootMode_t)0);
        }

        uint8_t bcmStatus = BCM_otp_read_vdd_setpoint(&vddSetPoint);
        D_PRINTF("[HMON] Idx|%x|Vol|%dmV\n", vddSetPoint, GetVddSetPointVoltage(vddSetPoint));
    }
    else if (argv[0] == 1)
    {
        if (smHwInfo.d.socType.tag == AVStag)
        {
            D_PRINTF("[HMON] Vol|%dmV\n", smHwInfo.d.socType.avs);
        }
        else
        {
            AdaptiveVoltageScaling_GetVddSetPoint(cInitBootCpu0Boot, cAVSOTP);
        }
    }
    else if (argv[0] == 2)                        // For Test Only
    {
        if (smHwInfo.d.socType.tag == AVStag)
        {
            smHwInfo.d.socType.tag = 0;
            smHwInfo.d.socType.avs = 0;
            HalSflash_MiscHwInfo_Flush();
        }
    }
    #endif

    return cEcNoError;
}
/* End of File */

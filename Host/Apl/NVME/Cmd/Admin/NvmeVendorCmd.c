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
// PKG
#include "BuildPkg.h"
#include "SharePkg.h"
#include "HostPkg.h"
#include "NvmeCmdPkg.h"

// REG
#include "RegPcie.h"
#include "RegPciePhy.h"

// HAL
#include "HalNfif.h"
#include "HalHwCheck.h"

// APL
#include "LoadFirmware.h"
#include "HldNvme.h"
#include "NvmeVendorCmd.h"
#include "SysInfo.h"
#include "Cache.h"
#include "PLP.h"
#include "UncTable.h"
#include "Purge.h"
#include "SmBus.h"
#include "AdaptiveVoltageScaling.h"
// Code Section
#include "NormSection.h"   // CODE AND DATA MEMORY MAPPING

#if CO_SUPPORT_VENDORCMD == TRUE
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
Error_t VscDeviceHWCheck(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo);
Error_t VscDeviceFWCheck(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo);
Error_t VscSysInfoOperation(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo);
Error_t VscPurgeOperation(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo);
Error_t VscAgingOnlyOperation(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo);
Error_t VscInternalTableOperation(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo);
Error_t VscFtlRelatedOperation(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo);
Error_t VscDebugOperation(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo);
Error_t VscNvmeAlternative(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo);

static VscError_t VSC_GetVscFunInfo(tVSC_Fnt* pHcmdTbl, tVSC_Fnt* pHcmdQ, U32 cmdCode);
static VscError_t VSC_CheckParameter(NvmeAdminVendorCommand_t* ChkVsc, tVSC_Fnt* Table);
static VscError_t VSC_AssignTableHead(U8 Mode, U8* pVenBuffer, tVSC_Htbp *TableHead);
static VscError_t VSC_IpcSetup_Send(MsgVsc_t* VscStr, U8 SubOpCode, U8* pBuffer, U8 WaitXferDone);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Imported data prototype
//-----------------------------------------------------------------------------
//  Vendor Unique Cmd Vector Table

__align(8) tVSC_Fnt mVscCmdVectorTable[]=
{
//cVendorCmdNoData = 0xFC; cVendorCmdWr = 0xFD; cVendorCmdRd = 0xFE;
//         Function                     {FC    FD    FE}
// Start  :  0x00
#if (RD_VSC == ENABLE)
// Add New
#endif
// Start  :  0x10
#if (NORMAL_VSC == ENABLE)
    {VscDeviceHWCheck,          VSC_CMD_DIR(TRUE,  TRUE,  TRUE),      Vsc_DeviceHWCheck},
    {VscDeviceFWCheck,          VSC_CMD_DIR(FALSE, FALSE, TRUE),      Vsc_DeviceFWCheck},
    {VscSysInfoOperation,       VSC_CMD_DIR(FALSE, TRUE,  TRUE),      Vsc_SysInfoOperation},
    {VscPurgeOperation,         VSC_CMD_DIR(TRUE,  FALSE, FALSE),     Vsc_PurageOperation},
    {VscAgingOnlyOperation,     VSC_CMD_DIR(TRUE,  FALSE, FALSE),     Vsc_AgingOnlyOperation},
    {VscFtlRelatedOperation,    VSC_CMD_DIR(TRUE,  TRUE,  TRUE),      Vsc_FTLRelatedOperation},
    {VscInternalTableOperation, VSC_CMD_DIR(TRUE,  FALSE, TRUE),      Vsc_InternalTableOperation},
    {VscDebugOperation,         VSC_CMD_DIR(TRUE,  FALSE, TRUE),      Vsc_DebugOperation},
    {VscNvmeAlternative,        VSC_CMD_DIR(TRUE,  FALSE, TRUE),      Vsc_NvmeAlternative},
#endif
// Start  :  0x20
#if (OEM_VSC == ENABLE)
// Add New
#endif
    {NULL,                      0,                                  0xFF}
};

static  tVSC_Fnt* VsccmdTableList = mVscCmdVectorTable;
//-----------------------------------------------------------------------------
//  Imported function proto-tpye
//-----------------------------------------------------------------------------
extern void TcgPreformatAndInit(U8 IsTcgInit);
extern void TcgKeepGroupDataBridge(U32 GrpNum);
extern void TcgEepOperatedBridge(U32 Metholdtype);
extern void TcgEepBackupBridge(void);
extern void TcgEepRestoreBridge(void);
extern void TcgPsidBackupBridge(void);
extern void TcgPsidRestoreBridge(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//------------------- Add New Vendor Command Flow -----------------------
// Step1. Check your Modle and Function
// Step2. Confirm your Cmd Direction(FC,FD,FE)  mVscCmdVectorTable[]-> VSC_CMD_DIR(TRUE,TRUE,TRUE)
// Step3. Creat your VSC
// note.
// 1. VSC Main can Control Transfer Data Work
// 2. VSC Internal Information already have VSC MSGQ, pBuffer, XferSize..., plz do not re-Obtain
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    VscMain
    @param[in] pHcmdQ Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_VscMain(HcmdQ_t* pHcmdQ)
{
    NvmeCdb_t*                  pCmdCdb;
    tVSCInternalInfo_t          VscInternalInfo;
    tVSC_Fnt                    fntGetFunInfo;
    VscError_t                  VscChk;

    MEM_CLR(&VscInternalInfo, sizeof(tVSCInternalInfo_t));

    VscInternalInfo.pCdb        = (NvmeAdminVendorCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    VscInternalInfo.pVscMsg     = (MsgVsc_t*)Ipc_GetExtendMsg(cMcVendor);
    VscInternalInfo.pVenBuffer  = (U8*)NvmeCmd_GetScratchBufferPointer();
    VscInternalInfo.xferSize    = VSC_XFER_LEN_DW2BYTE(VscInternalInfo.pCdb->NDT);

    MEM_CLR(VscInternalInfo.pVenBuffer, cNvmeScratchBufferSize);

    pCmdCdb = (NvmeCdb_t*) VscInternalInfo.pCdb;
    pHcmdQ->bufDesc.ssdIndex = SSD_IF_START;

    if (VSC_GetVscFunInfo(VsccmdTableList, &fntGetFunInfo, VscInternalInfo.pCdb->VscDW12.VscFunction) == ERROR_NoError)
    {
        VSC_PRINTF("Cmd[%x] In\n", pCmdCdb->DW12);

        VscChk = VSC_CheckParameter(VscInternalInfo.pCdb, &fntGetFunInfo);
        if (VscChk == ERROR_NoError)
        {
            pHcmdQ->error = cEcNoError;

            // Step1. Receive Payload, when Write Cmd
            if(VscInternalInfo.pCdb->DW9_0.OPC == VSC_DIR_WRITE)
            {
                pHcmdQ->attr.b.xferDir  = cXferH2d;
                AplHost_TransferWholeData_WithPRPList(pHcmdQ, VscInternalInfo.pVenBuffer, VscInternalInfo.xferSize, TRUE);
            }

            // Step2. Excuesem Vendor Cmd
            if (fntGetFunInfo.Fnt(pHcmdQ, &VscInternalInfo) == cEcNoError)
            {
                // Step3. Xfer Payload to Host, when Read Cmd
                if (VscInternalInfo.pCdb->DW9_0.OPC == VSC_DIR_READ)
                {
                    pHcmdQ->attr.b.xferDir  = cXferD2h;
                    AplHost_TransferWholeData_WithPRPList(pHcmdQ, VscInternalInfo.pVenBuffer, VscInternalInfo.xferSize, TRUE);
                }
                VSC_PRINTF("Cmd[%x] Done\n", pCmdCdb->DW12);
            }
            else
            {
                VSC_PRINTF("Cmd[%x] Fail\n", pCmdCdb->DW12);
            }
        }
        else
        {
            VSC_PRINTF("Check Parameter ERR:[%s]\n",(VscChk == ERROR_SIGNATURE)     ? "ERROR_SIGNATURE"     :
                                                    (VscChk == ERROR_TRANSFER_SIZE) ? "ERROR_TRANSFER_SIZE" :
                                                    (VscChk == ERROR_FUN_DIR)       ? "ERROR_ERROR_FUN_DIR" : "ERROR_OTHER");

            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidCommandOpcode, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
        }
    }
    else
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidCommandOpcode, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    // Step_Final. initail ssdindex
    VSC_CHANGE_XFER_SSDIDX(pHcmdQ->bufDesc.ssdIndex, gNvmeIfMgr.pScratchBuffer);
}

//-----------------------------------------------------------------------------
/**
    VSC : Vsc Device HW Check
    DW12(BYTE 0)  Function    0x10      Device HW Check
    DW12(BYTE 1)  Mode        0x00      Read Flash ID
    DW12(BYTE 1)  Mode        0x10      Read PCIE Link Status
    DW12(BYTE 1)  Mode        0x20      PLP Check
    DW13                                reserved
    DW14                                reserved
**/
//-----------------------------------------------------------------------------
Error_t VscDeviceHWCheck(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo)
{
    switch (pVscInternalInfo->pCdb->VscDW12.VscMode)
    {
        case VSC_HW_READ_FLASHID:
            VSC_IpcSetup_Send(pVscInternalInfo->pVscMsg, cSfnVscReadIdAll, pVscInternalInfo->pVenBuffer, TRUE);
            break;
        case VSC_HW_PCIE_CONFIG:
            if (rPciePhy.laneStat1.b.PM_TXDCLK_PCLK_EN)
            {
                VSC_PRINTF("Gen:%u Lanes:%u\n", rPciePf.lnkCtrlAndLnkStat.b.LINK_SPEED, rPciePf.lnkCtrlAndLnkStat.b.NEGO_LW);
                *((U8 *)pVscInternalInfo->pVenBuffer)     = rPciePf.lnkCtrlAndLnkStat.b.LINK_SPEED;
                *((U8 *)pVscInternalInfo->pVenBuffer + 1) = rPciePf.lnkCtrlAndLnkStat.b.NEGO_LW;
            }
            break;
        case VSC_HW_PCIE_DID_VID:
            if (rPciePhy.laneStat1.b.PM_TXDCLK_PCLK_EN)
            {
                *((U32 *)pVscInternalInfo->pVenBuffer)     = rPciePf.vendorIdAndDeviceId.all;
                *((U32 *)pVscInternalInfo->pVenBuffer+1)   = rPciePf.subsysteVendorIdAndSubsysteId.all;
            }
            break;
        #if (CO_SUPPORT_PLP == TRUE)
        case VSC_HW_CHECK_PLP:
            VSC_IpcSetup_Send(pVscInternalInfo->pVscMsg, cSfnVscHwCheckPLP, pVscInternalInfo->pVenBuffer, TRUE);
            break;
        #endif
        case VSC_HW_READ_DRAM_DLL:
            if(smSysInfo->d.AgingData.d.AgingDram.DramTag == VSC_AGING_DRAM)
                MEM_COPY((U64 *)pVscInternalInfo->pVenBuffer, &smSysInfo->d.AgingData.d.AgingDram.DramDLLAvg.DramDllPositive[0], sizeof(tAG_DRAMDLL));
            break;
        #if (CO_SUPPORT_READSOC == TRUE)
        case VSC_HW_CHECK_SOC_TYPE:
            if (smHwInfo.d.socType.tag == AVStag)
            {
                D_PRINTF("[VSC ]Vol|%xmV\n", smHwInfo.d.socType.avs);
                *((U32 *)pVscInternalInfo->pVenBuffer) = smHwInfo.d.socType.avs;
            }
            else
            {
                AdaptiveVoltageScaling_GetVddSetPoint(cInitBootCpu0Boot, cAVSOTP);
                *((U32 *)pVscInternalInfo->pVenBuffer) = smHwInfo.d.socType.avs;
            }
            break;
        #endif
        #if (CO_SUPPORT_SMBUS == TRUE)
        case VSC_HW_VPD_INITIAL:
            pVscInternalInfo->pVscMsg->param1 = SMBUS_CENTER_VPD_INJECT;
            VSC_IpcSetup_Send(pVscInternalInfo->pVscMsg, cSfnVscSmBus, pVscInternalInfo->pVenBuffer, TRUE);
            break;
        case VSC_HW_VPD_WRTIE:
        {
            tVSC_SubData* pDwData = (tVSC_SubData*)&pVscInternalInfo->pCdb->VscDW13;

            pVscInternalInfo->pVscMsg->param1 = SMBUS_CENTER_VPD_WRITE;
            pVscInternalInfo->pVscMsg->param[0] = pDwData->hw.VscHwOffset;
            pVscInternalInfo->pVscMsg->param[1] = pDwData->hw.VscHwOpLen;
            VSC_IpcSetup_Send(pVscInternalInfo->pVscMsg, cSfnVscSmBus, pVscInternalInfo->pVenBuffer, TRUE);
            break;
        }
        case VSC_HW_VPD_READ:
        {
            tVSC_SubData* pDwData = (tVSC_SubData*)&pVscInternalInfo->pCdb->VscDW13;

            pVscInternalInfo->pVscMsg->param1 = SMBUS_CENTER_VPD_READ;
            pVscInternalInfo->pVscMsg->param[0] = pDwData->hw.VscHwOffset;
            pVscInternalInfo->pVscMsg->param[1] = pDwData->hw.VscHwOpLen;
            VSC_IpcSetup_Send(pVscInternalInfo->pVscMsg, cSfnVscSmBus, pVscInternalInfo->pVenBuffer, TRUE);
            break;
        }
        #endif
        default:
            pHcmdQ->error = cEcError;
            break;
    }

    return pHcmdQ->error;
}
//-----------------------------------------------------------------------------
/**
    VSC : Device FW Check
    DW12(BYTE 0)  Function    0x11      Device FW Check
    DW12(BYTE 1)  Mode        0x00      Check Model
    DW12(BYTE 1)  Mode        0x40      Read FW Version
    DW13                                FW Version OP.
    DW14                                reserved
**/
//-----------------------------------------------------------------------------
Error_t VscDeviceFWCheck(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo)
{
    tVSC_FwConfig*              pFwConfig;
    tVSC_SubData*               pDwData;
    char*                       FwVersion;
    U16                         VscSupportMaxBlk = VSC_SUPPORT_MAX_BLK;
    U8                          VSC_StrLen;

    pDwData = (tVSC_SubData*)&pVscInternalInfo->pCdb->VscDW13;

    FwVersion = (sizeof(_SRC_REV_) > sizeof("NoSupport!") ? "NoSupport!" : _SRC_REV_);

    switch (pVscInternalInfo->pCdb->VscDW12.VscMode)
    {
        case VSC_FW_READFWCONFIG:
            pFwConfig  = (tVSC_FwConfig*) pVscInternalInfo->pVenBuffer;
            for (VSC_StrLen = 0; VSC_StrLen < pMenu.nItems; VSC_StrLen++)
            {
                MEM_COPY(pVscInternalInfo->pVenBuffer + pMenu.pMenuItems[VSC_StrLen].Info_Add,
                         pMenu.pMenuItems[VSC_StrLen].ModelInfo,
                         sizeof(pMenu.pMenuItems[VSC_StrLen].ModelInfo));
            }

            *((U32 *)pFwConfig->VscBufferSizeInByte)      = cNvmeScratchBufferSize;
            *((U32 *)pFwConfig->SysinfoBufferSizeInByte)  = SYSINFO_CACHE_SIZE;
            MEM_COPY(pFwConfig->PcbaSn, smSysInfo->d.MPInfo.d.SerialNumberPCBA, 16);
            MEM_COPY(pFwConfig->DriveSn, smSysInfo->d.MPInfo.d.SerialNumberDrive, 20);
            MEM_COPY(pFwConfig->FwSvnRevision, &FwVersion[0], 16);

            pFwConfig->FatalError.AsicErrorCode     = 0;
            pFwConfig->FatalError.NandErrorCode     = 0;
            pFwConfig->FatalError.DramErrorCode     = 0;
            pFwConfig->FatalError.SpiErrorCode      = 0;
            pFwConfig->FatalError.OthersErrorCode   = 0;

            *((U32 *)pFwConfig->Cluster.ClusterType)         = 0;
            *((U32 *)pFwConfig->Cluster.CountOfUserData)     = (smSysInfo->d.FtlData.d.EndIdx - smSysInfo->d.FtlData.d.StartIdx) + 1;
            *((U32 *)pFwConfig->Cluster.EraseCountEntrySize) = ERASE_COUNT;
            *((U32 *)pFwConfig->Cluster.ValidCountEntrySize) = VALID_COUNT;

            MEM_COPY(pFwConfig->NandInfo.NandFlashId, smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.id, 6);
            pFwConfig->NandInfo.ChCnt   = smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.numChannels;
            pFwConfig->NandInfo.CeCnt   = smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.devicesPerChannel;
            pFwConfig->NandInfo.LunCnt  = LOG2(smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.lunsPerDevice) + 1;
            pFwConfig->NandInfo.DieCnt  = smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.numChannels       *
                                          smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.devicesPerChannel *
                                          smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.lunsPerDevice;

            pFwConfig->NandInfo.PlaneWrite      = smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.planesPerLun;
            pFwConfig->NandInfo.PlaneRead       = smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.planesPerLun;
            pFwConfig->NandInfo.LaaSizeByte     = MBU_SIZE;
            pFwConfig->NandInfo.PageSizeInByte  = smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.dataBytesPerFullPage / KBYTE;
            pFwConfig->NandInfo.PageCntInBlk    = smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.pagesPerBlock;
            break;
        case VSC_FW_READ_FW_MODEL:
        {
            // Main Code Version
            MEM_COPY(&pVscInternalInfo->pVenBuffer[0],  &LiteOnFWRevision[0],                         NVME_ID_BYTEMAX_FWREVISION - 1);
            if (pDwData->fw.VscfwOp)
            {
                MEM_COPY(&pVscInternalInfo->pVenBuffer[7], _FW_REV_SUB_, strlen(_FW_REV_SUB_));
            }
            break;
        }
        case VSC_FW_READ_BOOT_VERSION:
        {
            AplLoadFirmware_GetFwInfo(DFW_CURRENT_HEAD_OFFSET, DISABLE, &gLdFw_FwInfo[0]);
            // Boot Code Version
            HalSflash_Copy(&gLdFw_module, (DFW_CURRENT_HEAD_OFFSET + DFW_FW_DESC_SIZE + DFW_FW_SELECT_SIZE), sizeof(DfwModuleInfo_t));
            HalSflash_Copy(pVscInternalInfo->pVenBuffer, gLdFw_module.offset + gLdFw_module.size - 16, NVME_ID_BYTEMAX_FWREVISION - 1);
            // Main Code Version
            MEM_COPY(&pVscInternalInfo->pVenBuffer[16], &gLdFw_ImgTag[FW_MAIN1_VALID].fw_revision[0], NVME_ID_BYTEMAX_FWREVISION - 1);
            MEM_COPY(&pVscInternalInfo->pVenBuffer[24], &gLdFw_ImgTag[FW_MAIN2_VALID].fw_revision[0], NVME_ID_BYTEMAX_FWREVISION - 1);
            pVscInternalInfo->pVenBuffer[2] = pVscInternalInfo->pVenBuffer[18] = pVscInternalInfo->pVenBuffer[26] = gCapacityMap[_OP_][gCapacityIndex].CapacityChar;
            break;
        }

        default:
            pHcmdQ->error = cEcError;
            break;
    }
     return pHcmdQ->error;
}

//-----------------------------------------------------------------------------
/**
    VSC : SysInfo Read / Write Operation
    DW12(BYTE 0)  Function      0x12    SI Operation
    DW12(BYTE 1)  Mode          0x00    SI Read
                                0x01    SI Write
    DW13                                Offset
    DW14                                OP. Length
**/
//-----------------------------------------------------------------------------
Error_t VscSysInfoOperation(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo)
{
    tVSC_SubData*   pDwData;

    pDwData = (tVSC_SubData*)&pVscInternalInfo->pCdb->VscDW13;

    if ((pDwData->si.VscSiOffset + pVscInternalInfo->xferSize > SYSINFO_CACHE_SIZE) || (pDwData->si.VscSiOpLen > pVscInternalInfo->xferSize))
    {
        VSC_PRINTF("ERR: (Add + OffSet size):%d > Sysinfo Size:%d\n", pDwData->si.VscSiOffset + pVscInternalInfo->xferSize, SYSINFO_CACHE_SIZE);
        pHcmdQ->error = cEcError;
        return pHcmdQ->error;
    }

    switch (pVscInternalInfo->pCdb->VscDW12.VscMode)
    {
        case VSC_SI_READ:
            MEM_COPY(pVscInternalInfo->pVenBuffer, &smSysInfo->all[pDwData->si.VscSiOffset], pVscInternalInfo->xferSize);
            break;
        case VSC_SI_WIRTE:
        {
            U32* SiHdOffset;
            U32* SiHdSize;
            U32  SiOffAdd;
            U32  addrIdx = 0;
            U16  sysInfoArea = 0;
            U8   MapHeadNum;
            U8   CheckRealHead = 0;

            addrIdx     = pDwData->si.VscSiOffset;
            sysInfoArea = 0;
            SiOffAdd    = (sizeof(tSI_SUBHEADER_D)>> 2) * (SYSINFO_SUBHEADER_CNT_NAND - 1);
            SiHdOffset  = &smSysInfo->d.Header.d.SubHeader.d.Misc.d.dwDataOffset;
            SiHdSize    = &smSysInfo->d.Header.d.SubHeader.d.Misc.d.dwDataSize;

            for (MapHeadNum = 0; MapHeadNum < SYSINFO_SUBHEADER_CNT_NAND; MapHeadNum++)
            {
                if (addrIdx >= SiHdOffset[SiOffAdd] && addrIdx <= SiHdOffset[SiOffAdd] + SiHdSize[SiOffAdd])
                {
                    //Sysinfo can't write area error handle
                    if (sysInfoArea & (SI_AREA_PLIST1 | SI_AREA_PLIST2 | SI_AREA_FTL | SI_AREA_FTLRMP))
                    {
                        CheckRealHead = FALSE;
                    }
                    else
                    {
                        CheckRealHead = TRUE;
                    }
                    break;
                }

                sysInfoArea >>= 1;
                SiOffAdd     -= (sizeof(tSI_SUBHEADER_D) >> 2);
            }

            if (CheckRealHead)
            {
                MEM_COPY(&smSysInfo->all[pDwData->si.VscSiOffset], pVscInternalInfo->pVenBuffer, pDwData->si.VscSiOpLen);
                SyIn_Synchronize(sysInfoArea, SYSINFO_WRITE, SI_SYNC_BY_SYSINFO);
            }
            else
            {
                VSC_PRINTF("Invalid Address or Area\n");
                pHcmdQ->error = cEcError;
            }
            break;
        }
        default:
            pHcmdQ->error = cEcError;
            break;
    }

    return pHcmdQ->error;
}

//-----------------------------------------------------------------------------
/**
    VSC : Purge Operation
    DW12(BYTE 0)  Function    0x13      Purge Operation
    DW12(BYTE 1)  Mode        0x00      MP Preformat
    DW13                                Purge OP.
    DW14                                reserved
**/
//-----------------------------------------------------------------------------
Error_t VscPurgeOperation(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo)
{
    switch (pVscInternalInfo->pCdb->VscDW12.VscMode)
    {
        case VSC_PG_MP_PREFORMAT:
            #if _TCG_ != TCG_NONE
            TcgPsidBackupBridge();
            TcgKeepGroupDataBridge(4);
            TcgEepOperatedBridge(0);
            TcgEepBackupBridge();
            #endif

            MEM_CLR(&smSysInfo->d.LogInfo.d.SmrLog, sizeof(tSMR_LOG));
            SyIn_Synchronize(SI_AREA_SMART, SYSINFO_WRITE, SI_SYNC_BY_SYSINFO);
            UNCT_CHECK_DO_RESET();
            VSC_IpcSetup_Send(pVscInternalInfo->pVscMsg, cSfnVscPreformat, pVscInternalInfo->pVenBuffer, TRUE);

            #if _TCG_ != TCG_NONE
            TcgEepRestoreBridge();
            TcgEepOperatedBridge(2);
            TcgPsidRestoreBridge();
            TcgPreformatAndInit(TRUE);
            #endif
            break;
        default:
            pHcmdQ->error = cEcError;
            break;
    }
    return pHcmdQ->error;
}

//-----------------------------------------------------------------------------
/**
    VSC : VscAgingOnly Operation
    DW12(BYTE 0)  Function    0x14    Aging Operation
    DW12(BYTE 1)  Mode        0x00    Erase NAND Flash
                              0x01    Scandefect
                              0x02    Aging Batch
                              0x03    PList Operation
    DW13                              Ag-Plist OP.
    DW14                              reserved
**/
//-----------------------------------------------------------------------------
Error_t VscAgingOnlyOperation(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo)
{
    tVSC_SubData* pDwData;

    pDwData = (tVSC_SubData*)&pVscInternalInfo->pCdb->VscDW13;

    switch (pVscInternalInfo->pCdb->VscDW12.VscMode)
    {
        case VSC_AG_ERASE_NANDFLASH:
            VSC_IpcSetup_Send(pVscInternalInfo->pVscMsg, cSfnVscReadIdAll, pVscInternalInfo->pVenBuffer, TRUE);
            break;
        case VSC_AG_SCAN_DEFECT:
            pVscInternalInfo->pVscMsg->param[0] = VSC_COREDBG_CMD_SCANDEFECT;
            VSC_IpcSetup_Send(pVscInternalInfo->pVscMsg, cSfnVscSeq, pVscInternalInfo->pVenBuffer, TRUE);
            break;
        case VSC_AG_AGING_BATCH:
            VSC_PRINTF("AgLoop[%d], AgEnable[%d]\n", pDwData->ag.VscAgLoop, pDwData->ag.VscAgEn);
            if (pDwData->ag.VscAgMode == 0)
            {
                MEM_CLR(smSysInfo->d.AgingData.all, sizeof(tSI_PAYLOAD_AGING));
            }
            smSysInfo->d.Header.d.SubHeader.d.Aging.d.dwValidTag    = SI_TAG_AGING;
            smSysInfo->d.Header.d.SubHeader.d.MPInfo.d.dwValidTag   = SI_TAG_MPINFO;

            smSysInfo->d.AgingData.d.Aging_Mode                     = pDwData->ag.VscAgEn; // enable
            smSysInfo->d.MPInfo.d.AgingMode                         = pDwData->ag.VscAgEn;

            smSysInfo->d.AgingData.d.AgingRerun.ReRunCount          = 0x00;
            smSysInfo->d.AgingData.d.AgingRerun.RecordError         = U16_MAX;
            smSysInfo->d.AgingData.d.Aging_ErrorCode                = U16_MAX;
            smSysInfo->d.AgingData.d.AgingLoop.LoopTarget           = pDwData->ag.VscAgLoop;// loop
            smSysInfo->d.MPInfo.d.LoopTarget                        = pDwData->ag.VscAgLoop;
            smSysInfo->d.MPInfo.d.AgingLED                          = 0x4C;

            SyIn_Synchronize(SI_AREA_MISC|SI_AREA_MPINFO|SI_AREA_AGING, SYSINFO_WRITE, SI_SYNC_BY_AGING);
            break;
        case VSC_AG_PLIST_OP:
            if ((smSysInfo->d.Header.d.SubHeader.d.PList1.d.dwValidTag == SI_TAG_PLIST1) &&
                (smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwValidTag != SI_TAG_PLIST2  ||
                 pDwData->ag.VscPlistCopyMode == VSC_PLIST_FORCE_COPY))
            {
                MEM_COPY(&smSysInfo->d.PList2, &smSysInfo->d.PList1, sizeof(tSI_PAYLOAD_PLIST));
                MEM_COPY(&smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[0], &smSysInfo->d.PlistCnt.d.PList1Cnt.wCount[0], sizeof(tSI_PLIST_CNT));
                smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwValidTag = SI_TAG_PLIST2;
                SyIn_Synchronize(SI_AREA_MISC|SI_AREA_PLIST2, SYSINFO_WRITE, SI_SYNC_BY_AGING);
            }
            else
            {
                D_PRINTF("PList2 already valid\n");
                pHcmdQ->error = cEcError;
            }
            break;
        default:
            pHcmdQ->error = cEcError;
            break;
    }
    return pHcmdQ->error;
}

//-----------------------------------------------------------------------------
/**
    VSC : FTL-Related Operation
    DW12(BYTE 0)  Function      0x15    FTL Operation
    DW12(BYTE 1)  Mode          0x00    Disk Lock (Read Only)
                                0x01    Read NAND Flush Count
                                0x10    ECC Insert
                                0x11    ECC Delete
                                0x12    ECC Delete
    DW13                                reserved
    DW14                                reserved
**/
//-----------------------------------------------------------------------------
Error_t VscFtlRelatedOperation(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo)
{
    switch (pVscInternalInfo->pCdb->VscDW12.VscMode)
    {
        case VSC_FTL_DISK_LOCK:
            break;
        case VSC_FTL_NAND_FLUSH_CNT:
            VSC_IpcSetup_Send(pVscInternalInfo->pVscMsg, cSfnVscRdNandFlushCnt, pVscInternalInfo->pVenBuffer, TRUE);
            break;
        case VSC_FTL_UNCT_INSERT:
        {
            tVSC_SubData*  pDwData = (tVSC_SubData*)&pVscInternalInfo->pCdb->VscDW13;

			UNCT_CHECK_DO_INSERT(pDwData->ft.VscEccLba, pDwData->ft.VscEccLen, UNCT_NDTP_WUNC);
            break;
        }
        case VSC_FTL_UNCT_DELETE:
        {
            tVSC_SubData*  pDwData = (tVSC_SubData*)&pVscInternalInfo->pCdb->VscDW13;

			UNCT_CHECK_DO_DELETE(pDwData->ft.VscEccLba, pDwData->ft.VscEccLen, ENABLE);
            break;
        }
        case VSC_FTL_UNCT_RESET:
            Purge_Center(PURGE_CORE_UNCT);
            break;
        default:
            pHcmdQ->error = cEcError;
            break;
    }
    return pHcmdQ->error;
}
//-----------------------------------------------------------------------------
/**
    VSC : Internal Table Operation
    DW12(BYTE 0)  Function    0x16    Internal Table Operation
    DW12(BYTE 1)  Mode        0x00    Read Erase Count Table
                              0x01    Read Valid Count Table
                              0x02    Read GList
                              0x03    Read ECC Table
    DW13                              Offset/Laa
    DW14                              reserved/Len
**/
//-----------------------------------------------------------------------------
Error_t VscInternalTableOperation(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo)
{
    tVSC_SubData*   pDwData;
    tVSC_Htbp       HeadTabPat;
    U32             Glist_forTest;
    U16             CopyLen = 0;

    pDwData = (tVSC_SubData*)&pVscInternalInfo->pCdb->VscDW13;

    MEM_CLR(&HeadTabPat, sizeof(tVSC_Htbp));

    /*  Don't check size to satisify VSC script
    if (VSC_CHECK_XFER_SIZE(pVscInternalInfo->pCdb->VscDW12.VscMode, pDwData->tb.VscTbOffset + pVscInternalInfo->xferSize))
    {
        pHcmdQ->error = cEcError;
        return pHcmdQ->error;
    }
    */

    switch (pVscInternalInfo->pCdb->VscDW12.VscMode)
    {
        case VSC_TB_READ_ERASE_CNT:
            VSC_IpcSetup_Send(pVscInternalInfo->pVscMsg, cSfnVscReadECTable, pVscInternalInfo->pVenBuffer, TRUE);
            VSC_ASSIGN_TBHD(HeadTabPat, TAB_HEAD_ECTB, VSC_ECTABLE_SIZE);
            break;
        case VSC_TB_READ_VAILD_CNT:
            VSC_IpcSetup_Send(pVscInternalInfo->pVscMsg, cSfnVscReadVCTable, pVscInternalInfo->pVenBuffer, TRUE);
            VSC_ASSIGN_TBHD(HeadTabPat, TAB_HEAD_VCTB, VSC_VSCTABLE_SIZE);
            break;
        case VSC_TB_READ_GLIST:
            VSC_ASSIGN_TBHD(HeadTabPat, TAB_HEAD_GLTB, SI_PAYLOAD_SIZE_GLIST);
            MEM_COPY(pVscInternalInfo->pVenBuffer, &smSysInfo->d.GList.all[0], SI_PAYLOAD_SIZE_GLIST);
            break;
        case VSC_TB_READ_UNCTB:
            VSC_ASSIGN_TBHD(HeadTabPat, TAB_HEAD_ECCB, 0);
    *((U16 *)pVscInternalInfo->pVenBuffer) = pUncTblInfo->brief.count[UNC_TBL_POOL_USED];
            MEM_COPY(&pVscInternalInfo->pVenBuffer[16], &pUncTblInfo->node[0], pUncTblInfo->brief.count[UNC_TBL_POOL_USED] * sizeof(tUNCTblInfo));
            break;
        case VSC_TB_CLEAR_GLIST:
            MEM_CLR(&smSysInfo->d.GList.all[0], SI_PAYLOAD_SIZE_GLIST);
            SyIn_Synchronize(SI_AREA_GLIST, SYSINFO_WRITE, SI_SYNC_BY_SYSINFO);
            break;
        default:
            pHcmdQ->error = cEcError;
            break;
    }

    if (pDwData->tb.VscTbHeadTag == TRUE)
    {
        VSC_AssignTableHead(pVscInternalInfo->pCdb->VscDW12.VscMode, &pVscInternalInfo->pVenBuffer[0], &HeadTabPat);
        MEM_SET(&pVscInternalInfo->pVenBuffer[VSC_HEAD_SIZE], U8_MAX, pVscInternalInfo->xferSize);
    }
    else
    {
        pVscInternalInfo->pVenBuffer = (U8 *)(pVscInternalInfo->pVenBuffer + pDwData->tb.VscTbOffset);
        VSC_CHANGE_XFER_SSDIDX(pHcmdQ->bufDesc.ssdIndex, pVscInternalInfo->pVenBuffer)
    }
    return pHcmdQ->error;
}

//-----------------------------------------------------------------------------
/**
    VSC : Vsc Debug Operation
    DW12(BYTE 0)  Function	0x17	Debug
    DW12(BYTE 1)  Mode	    0x00	Show Info
    DW12(BYTE 1)  Mode	    0x10    Save log
    DW12(BYTE 1)  Mode	    0x11    Load log
    DW13                            reserved
    DW14                            reserved
**/
//-----------------------------------------------------------------------------
Error_t VscDebugOperation(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo)
{
    switch (pVscInternalInfo->pCdb->VscDW12.VscMode)
    {
        case VSC_DBG_SHOW_INFO:
            break;
        case VSC_DBG_SAVE_LOG:
            break;
        case VSC_DBG_READ_LOG:
            break;
        default:
            pHcmdQ->error = cEcError;
            break;
    }
    return pHcmdQ->error;
}

//-----------------------------------------------------------------------------
/**
    VSC : Vsc Nvme Alternative
    DW12(BYTE 0)  Function	0x17	Debug
    DW12(BYTE 1)  Mode	    0x00	Show Info
    DW12(BYTE 1)  Mode	    0x10    Save log
    DW12(BYTE 1)  Mode	    0x11    Load log
    DW13                            reserved
    DW14                            reserved
**/
//-----------------------------------------------------------------------------
Error_t VscNvmeAlternative(HcmdQ_t* pHcmdQ, tVSCInternalInfo_t* pVscInternalInfo)
{
    switch (pVscInternalInfo->pCdb->VscDW12.VscMode)
    {
        case VSC_NVME_FLUSH:
            break;
        case VSC_NVME_COMMIT:
            break;
        default:
            pHcmdQ->error = cEcError;
            break;
    }
    return pHcmdQ->error;
}

//-----------------------------------------------------------------------------
/**
     Get Funtion Information
**/
//-----------------------------------------------------------------------------
static VscError_t VSC_GetVscFunInfo(tVSC_Fnt* pHcmdTbl, tVSC_Fnt* pHcmdQ, U32 cmdCode)
{
    VscError_t GetVscStatus = ERROR_NoError;

    while (pHcmdTbl->Fnt != NULL)
    {
        if (pHcmdTbl->cmdCode == cmdCode)
        {
            pHcmdQ->Fnt             = pHcmdTbl->Fnt;
            pHcmdQ->Attr.all        = pHcmdTbl->Attr.all;
            return GetVscStatus;
        }
        pHcmdTbl++;
    }
    return GetVscStatus;
}

//-----------------------------------------------------------------------------
/**
     Assign Table Header
**/
//-----------------------------------------------------------------------------

static VscError_t VSC_AssignTableHead(U8 Mode, U8* pVenBuffer, tVSC_Htbp *TableHead)
{
    U32 CurrentBlkNum = (smSysInfo->d.FtlData.d.EndIdx - smSysInfo->d.FtlData.d.StartIdx) + 1;

    switch(Mode)
    {
        case VSC_TB_READ_ERASE_CNT:
            TableHead->hParameter.VscEc_h.hEcMax = *(U32 *)((U16 *)pVenBuffer + CurrentBlkNum);
            TableHead->hParameter.VscEc_h.hEcAvg = *(U32 *)((U16 *)pVenBuffer + CurrentBlkNum + 2);
            TableHead->hParameter.VscEc_h.hEcMin = *(U32 *)((U16 *)pVenBuffer + CurrentBlkNum + 4);
            break;
        case VSC_TB_READ_VAILD_CNT:
            TableHead->hParameter.VscVc_h.hVcMax = *((U32 *)pVenBuffer + CurrentBlkNum);
            TableHead->hParameter.VscVc_h.hVcAvg = *((U32 *)pVenBuffer + CurrentBlkNum + 1);
            TableHead->hParameter.VscVc_h.hVcMin = *((U32 *)pVenBuffer + CurrentBlkNum + 2);
            break;
        case VSC_TB_READ_GLIST:
            TableHead->hParameter.VscGl_h.hGl_DefErcnt = smSysInfo->d.GList.d.GListBrief.wCntErase;
            TableHead->hParameter.VscGl_h.hGl_DefWrcnt = smSysInfo->d.GList.d.GListBrief.wCntWrite;
            TableHead->hParameter.VscGl_h.hGl_DefRdcnt = smSysInfo->d.GList.d.GListBrief.wCntRead;
            break;
        case VSC_TB_READ_UNCTB:
            TableHead->hParameter.VscECC_h.hEcc_Cnt = *((U16 *)pVenBuffer);
            break;
        default:
            break;
    }
    //Copy Header
    MEM_COPY(&pVenBuffer[0], &TableHead[0], sizeof(tVSC_Htbp));

    return ERROR_NoError;
}


//-----------------------------------------------------------------------------
/**
     Check VSC Parameter
**/
//-----------------------------------------------------------------------------
static VscError_t VSC_CheckParameter(NvmeAdminVendorCommand_t* ChkVsc, tVSC_Fnt* Table)
{

    //Check SIGNATURE_CHK
    if (ChkVsc->Signature != SIGNATURE_CHK)
    {
        return ERROR_SIGNATURE;
    }

    //Check Function Direction
    switch (ChkVsc->DW9_0.OPC)
    {
        case 0xFC:
            if(!Table->Attr.b.vDirFC) return ERROR_FUN_DIR;
            break;
        case 0xFD:
            if(!Table->Attr.b.vDirFD) return ERROR_FUN_DIR;
            break;
        case 0xFE:
            if(!Table->Attr.b.vDirFE) return ERROR_FUN_DIR;
            break;
    }

    //Check Transfer Size
    if (VSC_XFER_LEN_DW2BYTE(ChkVsc->NDT) > VSC_MAX_BUFSIZE)
    {
       return ERROR_TRANSFER_SIZE;
    }
    return ERROR_NoError;
}

//-----------------------------------------------------------------------------
/**
     VSC IPC Setup and Send
**/
//-----------------------------------------------------------------------------
static VscError_t VSC_IpcSetup_Send(MsgVsc_t* VscStr, U8 SubOpCode, U8* pBuffer, U8 WaitXferDone)
{
    VscStr->hdr.b.subOpCode = SubOpCode;
    VscStr->hdr.b.prty      = MSG_MAX_PRIORITY_NUM;
    VscStr->pBuffer         = pBuffer;

    IPC_SendMsgQ(cH2cReqQueue, (Msg_t*)VscStr);
    while ((VscStr->hdr.b.status != cMsgCompleted) & WaitXferDone);

    return ERROR_NoError;
}
#endif

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
//! @brief NAND flash dependant code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "Sysinfo.h"
#include "AplDesc_SNDK_MLC.h"
#include "Pkg_SNDK_MLC.h"

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

//-----------------------------------------------------------------------------
//  Private function proto-type definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data proto-type without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-type without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    One time initialization for target NAND package.\n

    @param[in]  initMode            initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t Pkg_OneTimeInit(InitBootMode_t initMode)
{
    Error_t  error = cEcNoError;

    U8 id[8];
    U8 plane;
    U8 lun;

    MEM_COPY(id, gNfInfo.id, sizeof(gNfInfo.id));

    gNfInfo.bitsPerCell         = NF_BITS_PER_CELL;
    gNfInfo.lunsPerDevice       = NF_LUNS_PER_DEVICE;
    gNfInfo.blocksPerLun        = NF_BLOCKS_PER_LUN;
    gNfInfo.planeAddressBits    = NF_PLANE_ADDRESS_BITS;
    gNfInfo.pagesPerBlock       = NF_PAGES_PER_BLOCK;
    gNfInfo.dataBytesPerPage    = NF_DATA_BYTES_PER_PAGE;
    gNfInfo.spareBytesPerPage   = NF_SPARE_BYTES_PER_PAGE;
    gCfgInfo.tPROG              = NF_T_PROG;
    gCfgInfo.tBERS              = NF_T_BERS;
    gCfgInfo.tR                 = NF_T_R;
    gCfgInfo.tRMP               = NF_T_R_MP;
    gCfgInfo.tCCS               = NF_T_CCS;

    gNfInfo.userBlkPerLun       =  NF_USER_BLOCKS_PER_LUN;

    lun   = (id[2] & 0x03);
    plane = ((id[4] & 0x0C) >> 2) ;
    plane = LOG2((1 << plane) >> lun) / (1 << lun);

    gNfInfo.lunsPerDevice       = (1 << lun);
    gNfInfo.planeAddressBits    = plane;

    gNfInfo.oriPlanesPerLun     = (1 << gNfInfo.planeAddressBits);
    gNfInfo.bitsPagesPerBlock   = LOG2((gNfInfo.pagesPerBlock / gNfInfo.bitsPerCell)-1)+1;

    MEM_COPY(gCfgInfo.manufacturer, NF_MANUFACTURER_NAME, sizeof(gCfgInfo.manufacturer));
    MEM_COPY(gCfgInfo.model       , NF_MODEL_NAME       , sizeof(gCfgInfo.model));

    gCfgInfo.manufacturer[11]   = '\0';  //Null termination
    gCfgInfo.model[19]          = '\0';  //Null termination

    gCfgInfo.ifData             = NF_JEDEC_TOGGLE2;
    gCfgInfo.xferMode           = NF_XFER_MODE_7;

    return error;
}


//-----------------------------------------------------------------------------
/**
    Configure target NAND package.\n

    @param[in]  pNfInfo             NF information

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t Pkg_Configure(NfCfgInfo_t *pCfgInfo)
{
    Error_t error = cEcNoError;
    Pna_t addr;
    uint8_t param[4];
    uint8_t nvDdr2Toggl2Mode[4];

    if(pCfgInfo->reqIfData > pCfgInfo->socIfData)
    {
        ///< SOC support only Toggle1
        pCfgInfo->reqIfData = NF_JEDEC_TOGGLE1;
        pCfgInfo->reqXferMode = NF_XFER_MODE_5;
        M_PRINTF("Requested interface was changed to TOGGLE1.\n");
    }
    else
    {
        if(pCfgInfo->reqXferMode > NF_XFER_MODE_6)
        {
            pCfgInfo->reqXferMode = NF_XFER_MODE_7;
            D_PRINTF("[PKG] Requested transfer mode was changed to %d.\n", pCfgInfo->reqXferMode);
        }
    }

    // Check supported configuration
    if(pCfgInfo->reqXferMode > pCfgInfo->xferMode)
    {
        D_PRINTF("[PKG] Requested xfer mode was wrong.\n");
        return cEcPkgConfigError;
    }

    error = AplNfif_ResetTargets(gNfInfo.numChannels, gNfInfo.devicesPerChannel, FC_RESET);
    if (error != cEcNoError)
    {
        return error;
    }

    if(pCfgInfo->reqIfData >= NF_JEDEC_TOGGLE2)
    {
        MEM_SET(nvDdr2Toggl2Mode, 0, sizeof(nvDdr2Toggl2Mode));
        if(pCfgInfo->reqIfData == NF_JEDEC_TOGGLE2)
        {
            #if (TOGGLE_533 == TRUE)
            nvDdr2Toggl2Mode[0] = (NF_VREF_EN << 0) | (NF_DQS_C_EN << 1) | (NF_RE_C_EN << 2)| (4 << 4);
            #else
            nvDdr2Toggl2Mode[0] = (NF_VREF_EN << 0) | (NF_DQS_C_EN << 1) | (NF_RE_C_EN << 2);
            #endif
            nvDdr2Toggl2Mode[1] = (NF_READ_DQS_LATENCY << 0) | (NF_WRITE_DQS_LATENCY << 4);
        }
        addr.row = FC_FEATURE_ADDR_NVDDR2_TOGGLE2;
        for (addr.ch = 0; addr.ch < gNfInfo.numChannels; addr.ch++)
        {
            for (addr.dev = 0; addr.dev < gNfInfo.devicesPerChannel; addr.dev++)
            {
                MEM_COPY(param, nvDdr2Toggl2Mode, sizeof(nvDdr2Toggl2Mode));

                HalNfif_SetFeature(&addr, param, DIRECT);
                HalNfif_GetFeature(&addr, param, DIRECT);

                if((nvDdr2Toggl2Mode[0] != param[0]) || (nvDdr2Toggl2Mode[1] != param[1]))
                {
                    E_PRINTF(cEcNfifFeatureMismatch, "CH#%1X DEV#%1X FA#%2X -> 0x%X%X!=0x%X%X\n", addr.ch, addr.dev, addr.row, nvDdr2Toggl2Mode[1], nvDdr2Toggl2Mode[0], param[1], param[0]);
                }
            }
        }

        MEM_SET(nvDdr2Toggl2Mode, 0, sizeof(nvDdr2Toggl2Mode));
        if(pCfgInfo->reqIfData == NF_JEDEC_TOGGLE2)
        {
            nvDdr2Toggl2Mode[0] = 0x06;
        }
        addr.row = FC_FEATURE_ADDR_DRIVE_STRENGTH;
        for (addr.ch = 0; addr.ch < gNfInfo.numChannels; addr.ch++)
        {
            for (addr.dev = 0; addr.dev < gNfInfo.devicesPerChannel; addr.dev++)
            {
                MEM_COPY(param, nvDdr2Toggl2Mode, sizeof(nvDdr2Toggl2Mode));

                HalNfif_SetFeature(&addr, param, DIRECT);
                HalNfif_GetFeature(&addr, param, DIRECT);

                if((nvDdr2Toggl2Mode[0] != param[0]) || (nvDdr2Toggl2Mode[1] != param[1]))
                {
                    E_PRINTF(cEcNfifFeatureMismatch, "CH#%1X DEV#%1X FA#%2X -> 0x%X%X!=0x%X%X\n", addr.ch, addr.dev, addr.row, nvDdr2Toggl2Mode[1], nvDdr2Toggl2Mode[0], param[1], param[0]);
                }
            }
        }
    }

    pCfgInfo->updatePhyInfo = 0;
    pCfgInfo->updateTimingInfo = 0;
    HalNfif_Configure(pCfgInfo);
    HalNfif_SetDataIf(pCfgInfo->reqIfData);

    // Save current configuration
    pCfgInfo->curIfData      = pCfgInfo->reqIfData;
    pCfgInfo->curXferMode    = pCfgInfo->reqXferMode;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Get Additional Information for target NAND package.\n

    @param[in]  pNfInfo             NF information

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t Pkg_GetAdditionalInfo(void)
{
    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Change Voltage Reference(Vref.).\n

    @param[in]  trkId               TRK ID
    @param[in]  ch                  Channel number
    @param[in]  dev                 Device number
    @param[in]  option              Option

    @return number of descriptors
**/
//-----------------------------------------------------------------------------
Error_t Pkg_ChangeVoltageReference(void)
{
    Pna_t pna;
    U8 param[4];
    U8 nvDdr2Toggl2Mode[4];
    U8 ch, ce;

    MEM_SET(nvDdr2Toggl2Mode, 0, sizeof(nvDdr2Toggl2Mode));
    MEM_SET(param, 0, sizeof(param));

    param[0] = nvDdr2Toggl2Mode[0] = L_VALUE;
    param[1] = nvDdr2Toggl2Mode[1] = A_VALUE;
    param[2] = nvDdr2Toggl2Mode[2] = B_VALUE;
    param[3] = nvDdr2Toggl2Mode[3] = C_VALUE;

    for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
    {
        for (ch = 0; ch < gNfInfo.numChannels; ch++)
        {
            pna.col = 0;
            pna.ch = ch;
            pna.dev = ce;
            pna.row = 0x11;;
            MEM_COPY(param, nvDdr2Toggl2Mode, sizeof(nvDdr2Toggl2Mode));

            HalNfif_SetFeature(&pna, param, DIRECT);
            HalNfif_GetFeature(&pna, param, DIRECT);

            if((nvDdr2Toggl2Mode[0] != param[0]) || (nvDdr2Toggl2Mode[1] != param[1]) ||
                (nvDdr2Toggl2Mode[2] != param[2]) || (nvDdr2Toggl2Mode[3] != param[3]))
            {
                E_PRINTF(cEcNfifFeatureMismatch, "CH#%1X DEV#%1X FA#%4X -> 0x%02X%02X!=0x%02X%02X\n", pna.ch, pna.dev, pna.row, nvDdr2Toggl2Mode[1], nvDdr2Toggl2Mode[0], param[1], param[0]);
            }
        }
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Indirect mode - NAND flash scan defect operation.

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t Pkg_ScanDefect(void)
{
    Error_t error = cEcNoError;

    Pna_t pna;
    U16 blk , page;
    U8  pBuffer[12];
    U8  lun;
    U8  scanpage = 6;
    U8  i;

    MEM_CLR(&smSysInfo->d.PList1, sizeof(tSI_PAYLOAD_PLIST));
    MEM_CLR(&smSysInfo->d.PlistCnt.d.PList1Cnt, sizeof(tSI_PLIST_CNT));

    MEM_CLR(&pBuffer, sizeof(pBuffer));

    HalNfif_SetFlashOperationMode(DIRECT);

    for (blk = 0; blk < gNfInfo.blocksPerLun; blk++)
    {
        for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
        {
            for (pna.dev = 0; pna.dev < gNfInfo.devicesPerChannel; pna.dev++)
            {
                for (pna.ch = 0; pna.ch < gNfInfo.numChannels; pna.ch++)
                {
                    for (page = 0; page < 2; page++)
                    {
                        pna.row = (blk << gNfInfo.bitsPagesPerBlock) | (lun << gNfInfo.bitsBlocksPerLun + gNfInfo.bitsPagesPerBlock);
                        pna.row |= page;
                        pna.col = 0;

                        HalNfif_DmSetDeviceChannel(&pna);
                        HalNfif_DmWriteCommandU8(FC_READ_BYTE0);
                        HalNfif_DmWriteAddressU8(pna.col);
                        HalNfif_DmWriteAddressU8(pna.col >> 8);
                        HalNfif_DmWriteAddressU8(pna.row);
                        HalNfif_DmWriteAddressU8(pna.row >> 8);
                        HalNfif_DmWriteAddressU8(pna.row >> 16);
                        HalNfif_DmWriteCommandU8(FC_READ_BYTE1);
                        HalTimer_DelayUsec(NF_tWHR_USEC);

                        error = HalNfif_WaitForStatusSet(&pna, FC_STATUS_RDY, 1000, DIRECT);

                        HalNfif_DmSetDeviceChannel(&pna);

                        pna.col = 0;
                        HalNfif_DmWriteCommandU8(FC_READ_BYTE2);
                        HalNfif_DmWriteAddressU8(pna.row);
                        HalNfif_DmWriteAddressU8(pna.row >> 8);
                        HalNfif_DmWriteAddressU8(pna.row >> 16);
                        HalNfif_DmWriteCommandU8(FC_READ_BYTE8);
                        HalNfif_DmWriteAddressU8(pna.col);
                        HalNfif_DmWriteAddressU8(pna.col >> 8);
                        HalNfif_DmWriteCommandU8(FC_READ_BYTE5);

                        HalTimer_DelayUsec(NF_tWHR_USEC);

                        HalNfif_DmWaitReToDqsDelay();

                        for (i = (page * scanpage); i < (scanpage * page) + scanpage; i++)
                        {
                            pBuffer[i] = (U8)HalNfif_DmReadToggleDataU16();
                        }
                    }

                    for (i = 0; i < 12; i++)
                    {
                        if( ((U8)pBuffer[i] == 0x00))
                        {
                            smSysInfo->d.PlistCnt.d.PList1Cnt.wCount[pna.ch][pna.dev][lun]++;
                            if (smSysInfo->d.PlistCnt.d.PList1Cnt.wCount[pna.ch][pna.dev][lun] <= SI_PLIST_MAX_COUNT)
                            {
                                smSysInfo->d.PList1.wList[pna.ch][pna.dev][lun][smSysInfo->d.PlistCnt.d.PList1Cnt.wCount[pna.ch][pna.dev][lun] - 1] = blk;
                            }
                            else
                            {
                                D_PRINTF("CH=%1d DEV=%1d LUN=%1d Defect Over 512\n", pna.ch, pna.dev,lun);
                            }
                            D_PRINTF("CH=%1d DEV=%1d LUN=%1d BLK=0x%4X CNT=%d\n"
                                     ,pna.ch, pna.dev, lun, blk, smSysInfo->d.PlistCnt.d.PList1Cnt.wCount[pna.ch][pna.dev][lun]);
                            break;
                        }
                    }
                }
            }
        }
    }
    HalNfif_DmBackToDefault();

    HalNfif_SetFlashOperationMode(SEQUENCER);

    return (error);
}

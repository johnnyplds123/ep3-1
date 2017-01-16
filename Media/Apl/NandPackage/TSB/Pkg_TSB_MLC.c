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
#include "MediaPkg.h"

#include "AplNfif.h"
#include "SysInfo.h"
#include "AplDesc_TSB_MLC.h"
#include "Pkg_TSB_MLC_Addons.h"
#include "Pkg_TSB_MLC.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define COPY_NAND_PARAM1(DES, SRC)  {   DES->dataBytesPerPage   = SRC->dataBytesPerPage;    \
                                        DES->spareBytesPerPage  = SRC->spareBytesPerPage;   \
                                        DES->pagesPerBlock      = SRC->pagesPerBlock;       \
                                        DES->blocksPerLun       = SRC->blocksPerLun;        \
                                        DES->lunsPerDevice      = SRC->lunCount;            \
                                        DES->planeAddressBits   = SRC->planeAddressBits;    \
                                        DES->bitsPerCell        = SRC->bitsPerCell;}

#define COPY_NAND_PARAM2(DES, SRC)  {   DES->tPROG              = SRC->tPROG;               \
                                        DES->tBERS              = SRC->tBERS;               \
                                        DES->tR                 = SRC->tR;                  \
                                        DES->tRMP               = SRC->tRMP;                \
                                        DES->tCCS               = SRC->tCCS; }
//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

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
    One time initialization for target NAND package.\n

    @param[in]  initMode            initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t Pkg_OneTimeInit(InitBootMode_t initMode)
{
    Pna_t addr;
    JedecParamPage_t* pJedecPage;
    Error_t  error = cEcNoError;
    U8 id[8];
    U8 plane;
    U8 lun;
    U8 mode;

    pJedecPage = MEM_AllocBuffer(sizeof(JedecParamPage_t), 4);
    MEM_CLR(pJedecPage, sizeof(JedecParamPage_t));

    MEM_COPY(id, gNfInfo.id, sizeof(gNfInfo.id));

    addr.dev = 0;
    addr.ch  = 0;
    addr.row = FC_PAGE_ADDR_JEDEC;

    for (addr.ch = 0; addr.ch < gNfInfo.numChannels; addr.ch++)
    {
        error = HalNfif_ReadParameterPage(&addr, (uint32_t *)pJedecPage, sizeof(JedecParamPage_t), DIRECT);
        if (pJedecPage->signature == FC_JEDEC_PARAM_PAGE_SIG)
        {
            if ((pJedecPage->toggleDdrSpeedGrade != 0xFFFF) && (pJedecPage->bitsPerCell != 0x03))
            {
                gCfgInfo.ifData   = NF_JEDEC_TOGGLE2;
                gCfgInfo.xferMode = NF_XFER_MODE_0;
                for(mode = 0; mode < 8; mode++)
                {
                    if(pJedecPage->toggleDdrSpeedGrade & (1 << mode))
                    {
                        gCfgInfo.xferMode = mode;
                    }
                }
            }
            else
            {
                gCfgInfo.ifData = NF_JEDEC_TOGGLE1;
                gCfgInfo.xferMode = NF_XFER_MODE_5;
                #if (HW_TYPE==HW_TYPE_FPGA)
                gCfgInfo.xferMode = NF_XFER_MODE_3;
                #endif
            }
            break;
        }
        #if CHK_FW_TYPE(FW_AGING)
        else
        {
            smAgingNandInfo.JEDECFail  = NJEDEC;
            smAgingNandInfo.FailCh     = addr.ch;
        }
        if (error != cEcNoError)
        {
            smAgingNandInfo.CRCFail    = NF_NCRC;
            smAgingNandInfo.FailCh     = addr.ch;
            smAgingNandInfo.FailLun    = pJedecPage->lunCount;
            smAgingNandInfo.FailPlane  = (1 << (pJedecPage->planeAddressBits));
        }
        #endif
    }

    lun   = (id[2] & 0x03);
    plane = ((id[4] & 0x0C) >> 2) ;
    plane = LOG2((1 << plane) >> lun);

    pJedecPage->planeAddressBits = (pJedecPage->planeAddressBits == plane) ? pJedecPage->planeAddressBits : plane;
    pJedecPage->lunCount = (pJedecPage->lunCount == (1 << lun)) ? pJedecPage->lunCount : (1 << lun);


    // Copy NAND Parameters.
    COPY_NAND_PARAM1((&gNfInfo), pJedecPage);
    COPY_NAND_PARAM2((&gCfgInfo), pJedecPage);

    gNfInfo.oriPlanesPerLun = (1 << pJedecPage->planeAddressBits);
    gNfInfo.userBlkPerLun       = NF_USER_BLOCKS_PER_LUN;
    gNfInfo.bitsPagesPerBlock  = LOG2(gNfInfo.pagesPerBlock-1)+1;

    MEM_COPY(gCfgInfo.manufacturer , pJedecPage->manufacturer, sizeof(gCfgInfo.manufacturer));
    MEM_COPY(gCfgInfo.model        , pJedecPage->model       , sizeof(gCfgInfo.model));

    gCfgInfo.manufacturer[11]   = '\0';  //Null termination
    gCfgInfo.model[19]          = '\0';  //Null termination


    MEM_FreeBuffer(pJedecPage);
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
        pCfgInfo->reqIfData = NF_JEDEC_TOGGLE1;
    }
    else
    {
        pCfgInfo->reqXferMode = NF_XFER_MODE_7;
    }

    D_PRINTF("[PKG] Requested transfer mode was changed to %d.\n",  pCfgInfo->reqXferMode);

    if(pCfgInfo->ifData >= NF_JEDEC_TOGGLE1)
    {
        error = AplNfif_ResetTargets(gNfInfo.numChannels, gNfInfo.devicesPerChannel, FC_RESET);
        if (error != cEcNoError)
        {
            return error;
        }
        MEM_SET(nvDdr2Toggl2Mode, 0, sizeof(nvDdr2Toggl2Mode));
        if(pCfgInfo->reqIfData == NF_JEDEC_TOGGLE2)
        {
            #if (TOGGLE_533 == TRUE)
            nvDdr2Toggl2Mode[0] = (NF_VREF_EN << 0) | (NF_DQS_C_EN << 1) | (NF_RE_C_EN << 2)| (4 << 4);
            #else
            nvDdr2Toggl2Mode[0] = (NF_VREF_EN << 0) | (NF_DQS_C_EN << 1) | (NF_RE_C_EN << 2);
            #endif
            nvDdr2Toggl2Mode[1] = (NF_READ_DQS_LATENCY << 0) | (NF_WRITE_DQS_LATENCY << 4);

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
                        E_PRINTF(cEcNfifFeatureMismatch, "CH#%1X DEV#%1X FA#%2X -> 0x%02X%02X!=0x%02X%02X\n", addr.ch, addr.dev, addr.row, nvDdr2Toggl2Mode[1], nvDdr2Toggl2Mode[0], param[1], param[0]);
                    }
                }
            }
        }

        MEM_SET(nvDdr2Toggl2Mode, 0, sizeof(nvDdr2Toggl2Mode));
        if(pCfgInfo->reqIfData == NF_JEDEC_TOGGLE2)
        {
            nvDdr2Toggl2Mode[0] = 0x06;
        }
        else
        {
            nvDdr2Toggl2Mode[0] = 0x04;
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
    U8 ch, ce, lun;

    ch = gNfInfo.numChannels;
    ce = gNfInfo.devicesPerChannel;
    lun = gNfInfo.lunsPerDevice;

    TSB_1Z_MLC_Die_Reset(ch ,ce ,lun, DISABLE);

    #if (_GENERATION_ == GENERATION_EP2)
        TSB_1Z_MLC_EraseLoop_Relax(ch ,ce ,lun);
    #else
        TSB_1Z_MLC_CCT3Setting(ch ,ce ,lun);
        TSB_1Z_MLC_EraseSetting(ch ,ce ,lun);
    #endif

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
    #if (READ_LEVEL_CHANGE == ENABLE)
    U8 ch, ce, lun;

    ch = gNfInfo.numChannels;
    ce = gNfInfo.devicesPerChannel;
    lun = gNfInfo.lunsPerDevice;
    TSB_1Z_MLC_RlevelChange(L_VALUE, A_VALUE, B_VALUE, C_VALUE, ch ,ce ,lun, DISABLE);
    
    #endif

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
    U8  pBuffer[4];
    U8  lun;

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
                    for (page = 0; page < gNfInfo.pagesPerBlock; page += (gNfInfo.pagesPerBlock - 1))
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

                        if ((pna.row & 0xFF) != 0xFF)
                            pBuffer[0] = (U8)HalNfif_DmReadToggleDataU16();
                        else
                            pBuffer[2] = (U8)HalNfif_DmReadToggleDataU16();

                        pna.col = gNfInfo.dataBytesPerPage;
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

                        if ((pna.row & 0xFF) != 0xFF)
                            pBuffer[1] = (U8)HalNfif_DmReadToggleDataU16();
                        else
                            pBuffer[3] = (U8)HalNfif_DmReadToggleDataU16();
                    }

                    if( (U8)pBuffer[0] != 0xFF || (U8)pBuffer[1] != 0xFF ||
                    (U8)pBuffer[2] != 0xFF || (U8)pBuffer[3] != 0xFF )
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
                    }
                }
            }
        }
    }

    HalNfif_DmBackToDefault();

    HalNfif_SetFlashOperationMode(SEQUENCER);

    return (error);
}

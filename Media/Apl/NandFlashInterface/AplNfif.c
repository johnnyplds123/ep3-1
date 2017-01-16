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
//! @brief NAND flash I/O High level driver
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "MediaPkg.h"

#include "SysInfo.h"
#include "AplSeq.h"
#include "AplNfif.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define cNumPseudoPlaneForTlc 2    ///< Number of Pseudo-planes for TLC

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static Error_t Nfif_NandDiscovery(InitBootMode_t initMode);
static Error_t Nfif_NandInitialization(InitBootMode_t initMode);
static Error_t Nfif_FindInterfaceType(NfCfgInfo_t* pNfInfo);
static Error_t Nfif_ScanTargets(uint32_t devicesPerCh, uint32_t numChannels, NfInfo_t* pNfInfo);
static Error_t Nfif_ReadNandFlashParameters(InitBootMode_t initMode);
static Error_t Nfif_SwitchIfData(NfIfData_t reqIfData, NfCfgInfo_t* pNfInfo);
void AplNfif_DumpInfo(void);
Error_t AplNfif_ResetTargets(uint32_t devicesPerCh, uint32_t numChannels, uint32_t command);
//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern Error_t Pkg_OneTimeInit(InitBootMode_t initMode);
extern Error_t Pkg_Configure(NfCfgInfo_t *pcfgInfo);
extern Cstr_t  Pkg_GetDescriptionString(const uint8_t idInfo[]);
extern Error_t Pkg_ChangeVoltageReference(void);
extern Error_t Pkg_GetAdditionalInfo(void);
extern Error_t AplDesc_OneTimeInit(InitBootMode_t initMode);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
NfInfo_t    gNfInfo;
NfCfgInfo_t gCfgInfo;

// Change "gCbfFioState" function pointer order at your own risk!
Error_t (*gCbfNfifInitTable[NUM_OF_INIT_FUNC][NFIF_INIT_MODE_COUNT])(InitBootMode_t initMode) =
{
    { HalNfif_OneTimeInit,      HalNfif_OneTimeInit,    },
    { Nfif_NandDiscovery,       NULL,                   },
    { HalSeq_OneTimeInit,       HalSeq_OneTimeInit,     },
    { Nfif_NandInitialization,  Nfif_NandInitialization,},
    { HalEcu_OneTimeInit,       HalEcu_OneTimeInit,     },
    { HalFlc_OneTimeInit,       HalFlc_OneTimeInit,     },
    { AplDesc_OneTimeInit,      AplDesc_OneTimeInit,    },
    { AplSeq_OneTimeInit,       NULL,                   },
};


//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    One time initialization for Flash I/O.

    @param[in]  initMode            initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t AplNfif_OneTimeInit(InitBootMode_t initMode)
{
    NfIfInitMode_t nfInitMode;
    Error_t error;
    Error_t (*pTableFunc)(InitBootMode_t initMode);
    uint32_t idx;

    if (initMode >= cInitBootPowerDown)
    {
        nfInitMode = NFIF_INIT_WAKEUP;
    }
    else
    {
        smNfInfoCpu1Addr = (U32)&gNfInfo;
        MEM_CLR(&gNfInfo, sizeof(NfInfo_t));
        nfInitMode = NFIF_INIT_COLDBOOT;
    }

    for (idx = 0; idx < NUM_OF_INIT_FUNC; idx++)
    {
        pTableFunc = gCbfNfifInitTable[idx][nfInitMode];
        if (pTableFunc)
        {
            error = pTableFunc(initMode);
            if (error != cEcNoError)
            {
                M_PRINTF("[NFIF] Init FAIL[%d]!!!\n", idx);
                return error;
            }
        }
    }

    if (initMode >= cInitBootPowerDown)
    {
        // Workaround for clean up abnormal register value after deep sleep wakeup
        if (initMode == cInitBootDeepPowerDown)
        {
            HalSeq_DisableSeq();
            HalSeq_EnableSeq();
        }
    }

    return cEcNoError;
}
#include "NormSection.h"
//-----------------------------------------------------------------------------
/**
    One time initialization for Flash I/O.\n

    @param[in]  initMode            initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
void AplNfif_DumpInfo(void)
{
    M_PRINTF("\n[NAND]\n");
    M_PRINTF("ID: %2X %2X %2X %2X %2X %2X\n",gNfInfo.id[0], gNfInfo.id[1], gNfInfo.id[2], gNfInfo.id[3], gNfInfo.id[4], gNfInfo.id[5]);
    M_PRINTF("Manufacturer:      %s\n", gCfgInfo.manufacturer);
    M_PRINTF("Model:             %s\n", gCfgInfo.model);
    M_PRINTF("InterfaceType:     %9u\n", gCfgInfo.ifType);
    M_PRINTF("InterfaceData:     %7u@%d\n", gCfgInfo.curIfData, gCfgInfo.ifData);
    M_PRINTF("TransferMode:      %7u@%d\n", gCfgInfo.curXferMode, gCfgInfo.xferMode);

    M_PRINTF("numChannels:       %9u\n", gNfInfo.numChannels);
    M_PRINTF("devicesPerChannel: %9u\n", gNfInfo.devicesPerChannel);
    M_PRINTF("lunsPerDevice:     %9u\n", gNfInfo.lunsPerDevice);
    M_PRINTF("lunsPerChannel:    %9u\n", gNfInfo.lunsPerChannel);
    M_PRINTF("blocksPerPlane:    %9u\n", gNfInfo.blocksPerPlane);
    M_PRINTF("blocksPerLun:      %9u\n", gNfInfo.blocksPerLun);
    M_PRINTF("blocksPerDevice:   %9u\n", gNfInfo.blocksPerDevice);
    M_PRINTF("userBlkPerLun:     %9u\n", gNfInfo.userBlkPerLun);
    M_PRINTF("sparedBlockPerLun: %9u\n", gNfInfo.sparedBlockPerLun);
    M_PRINTF("pagesPerBlock:     %9u\n", gNfInfo.pagesPerBlock);
    M_PRINTF("wLPerBlock:        %9u\n", gNfInfo.wlPerBlock);
    M_PRINTF("planesPerLun:      %9u\n", gNfInfo.planesPerLun);
    M_PRINTF("oriPlanesPerLun:   %9u\n", gNfInfo.oriPlanesPerLun);
    M_PRINTF("dataBytesPerFullPage:%7u\n", gNfInfo.dataBytesPerFullPage);
    M_PRINTF("dataBytePerPage:   %9u\n", gNfInfo.dataBytesPerPage);
    M_PRINTF("spareBytesPerPage: %9u\n", gNfInfo.spareBytesPerPage);

    M_PRINTF("bitsCh:            %9u\n", gNfInfo.bitsCh);
    M_PRINTF("bitsDevice:        %9u\n", gNfInfo.bitsDevice);
    M_PRINTF("bitsLun:           %9u\n", gNfInfo.bitsLun);
    M_PRINTF("bitsPlane:         %9u\n", gNfInfo.bitsPlane);

    M_PRINTF("planeAddressBits:  %9u\n", gNfInfo.planeAddressBits);
    M_PRINTF("bitsPerCell:       %9u\n", gNfInfo.bitsPerCell);
    M_PRINTF("bitsBlocksPerLun:  %9u\n", gNfInfo.bitsBlocksPerLun);
    M_PRINTF("bitsPagesPerBlock: %9u\n", gNfInfo.bitsPagesPerBlock);

    M_PRINTF("lunLsbPosition:    %9u\n", gNfInfo.lunLsbPosition);
    M_PRINTF("planeLsbPosition:  %9u\n", gNfInfo.planeLsbPosition);
    M_PRINTF("pairedBlkLsbPos:   %9u\n", gNfInfo.pairedBlkLsbPos);
    M_PRINTF("pagsPerProgUnit:   %9u\n", gNfInfo.pagesPerProgUnit);

    M_PRINTF("totalLun:          %9u\n", gNfInfo.totalLun);
    M_PRINTF("totalBlocks:       %9u\n", gNfInfo.totalBlocks);
    M_PRINTF("totalPages:        %9u\n", gNfInfo.totalPages);
    M_PRINTF("totalCapacity:     %9u GB\n", (U32)(((U64)gNfInfo.totalPages*gNfInfo.dataBytesPerPage)/GBYTE));
    M_PRINTF("UserCapacity:      %9u GB\n", gNfInfo.userCapacity);
}
#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    Reset target NAND flash module.\n

    @param[in]  devicesPerCh        devices per channel
    @param[in]  numChannels         number of channels
    @param[in]  command             reset command (FC_RESET or FC_SYNC_RESET)

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t AplNfif_ResetTargets(uint32_t devicesPerCh, uint32_t numChannels, uint32_t command)
{
    Pna_t addr;

    // Reset All NANDs
    for (addr.ch = 0; addr.ch < numChannels; addr.ch++)
    {
        for (addr.dev = 0; addr.dev < devicesPerCh; addr.dev++)
        {
            HalNfif_Reset(&addr, command, DIRECT);
        }
    }

    HalTimer_DelayUsec(NF_tRST_USEC);

    addr.ch--;
    addr.dev = 0;

    return HalNfif_WaitForStatusSet(&addr, FC_STATUS_RDY, 1000, DIRECT);
}
//-----------------------------------------------------------------------------
/**
    Initialize NAND device configuration.\n
      - Reset all NAND flash
      - Read manufacture and spec Id
      - Scan all NAND flash
      - Read parameter page

    @param[out]  pNfInfo            NAND flash media information pointer

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Nfif_NandDiscovery(InitBootMode_t initMode)
{
    Pna_t addr;
    Error_t error;
    U32 i;
    NfCfgInfo_t* pCfgInfo = &gCfgInfo;
    NfInfo_t*    pNfInfo  = &gNfInfo;

    AplNfif_ResetTargets(MAX_SUPPORTED_DEVICES, MAX_SUPPORTED_CHANNELS, FC_RESET);
    Nfif_FindInterfaceType(pCfgInfo);
    HalNfif_SetDataIf(pCfgInfo->ifData);
    Nfif_ScanTargets(MAX_SUPPORTED_DEVICES, MAX_SUPPORTED_CHANNELS, pNfInfo);

    switch (pCfgInfo->ifType)
    {
        case NF_ONFI:
            error = Nfif_ReadNandFlashParameters(initMode);
            break;
        case NF_JEDEC:
            if (pCfgInfo->curIfData == NF_ASYNC)
            {
                //
                // NOTE :
                // There is no criteria to differentiate TLC/MLC.
                // Besides, TLC starts with NF_ASYNC.
                //
                error = Nfif_SwitchIfData(NF_JEDEC_TOGGLE1, pCfgInfo);
                if (error != cEcNoError)
                {
                    pCfgInfo->curIfData = NF_ASYNC;
                }
                pCfgInfo->ifData = NF_JEDEC_TOGGLE1;

            }
            error = Nfif_ReadNandFlashParameters(initMode);
            break;
        default:
            error = cEcNfifUnknownTarget;
            break;
    }

    if (error == cEcNoError)
    {
        // compute value for speed up
        pNfInfo->planeAddressBits   = (pNfInfo->planeAddressBits > LOG2(MAX_SUPPORTED_PLANES)) ? LOG2(MAX_SUPPORTED_PLANES): pNfInfo->planeAddressBits;
        pNfInfo->lunsPerDevice      = (pNfInfo->lunsPerDevice > MAX_SUPPORTED_LUNS) ? MAX_SUPPORTED_LUNS : pNfInfo->lunsPerDevice;
        pNfInfo->lunsPerChannel     = pNfInfo->devicesPerChannel * pNfInfo->lunsPerDevice;
        pNfInfo->bitsLun            = LOG2(pNfInfo->lunsPerDevice-1)+1;
        pNfInfo->bitsCh             = LOG2(pNfInfo->numChannels-1)+1;
        pNfInfo->bitsDevice         = LOG2(pNfInfo->devicesPerChannel-1)+1;

        pNfInfo->bitsBlocksPerLun   = LOG2(pNfInfo->blocksPerLun-1)+1;
        pNfInfo->planeLsbPosition   = pNfInfo->bitsPagesPerBlock;
        pNfInfo->lunLsbPosition     = pNfInfo->bitsPagesPerBlock + pNfInfo->bitsBlocksPerLun;
        pNfInfo->pairedBlkLsbPos    = pNfInfo->bitsPagesPerBlock + pNfInfo->planeAddressBits;
        pNfInfo->planesPerLun       = (1 << pNfInfo->planeAddressBits);
        pNfInfo->totalLun           = pNfInfo->numChannels * pNfInfo->devicesPerChannel * pNfInfo->lunsPerDevice;
        pNfInfo->bitsPlane          = LOG2(pNfInfo->planesPerLun-1)+1;
        pNfInfo->bitsChDev          = pNfInfo->bitsCh + pNfInfo->bitsDevice;

        pNfInfo->sparedBlockPerLun  = (pNfInfo->blocksPerLun - pNfInfo->userBlkPerLun);
        pNfInfo->blocksPerPlane     = pNfInfo->blocksPerLun     / pNfInfo->planesPerLun;
        pNfInfo->blocksPerDevice    = pNfInfo->lunsPerDevice    * pNfInfo->blocksPerLun;
        pNfInfo->blocksPerChannel   = pNfInfo->blocksPerDevice  * pNfInfo->devicesPerChannel;
        pNfInfo->totalBlocks        = pNfInfo->blocksPerChannel * pNfInfo->numChannels;

        pNfInfo->pagesPerLun        = pNfInfo->pagesPerBlock * pNfInfo->blocksPerLun;
        pNfInfo->pagesPerDevice     = pNfInfo->pagesPerBlock * pNfInfo->blocksPerDevice;
        pNfInfo->pagesPerChannel    = pNfInfo->pagesPerBlock * pNfInfo->blocksPerChannel;
        pNfInfo->totalPages         = pNfInfo->pagesPerBlock * pNfInfo->totalBlocks;
        pNfInfo->dataBytesPerFullPage = pNfInfo->dataBytesPerPage + pNfInfo->spareBytesPerPage;

        if ((pNfInfo->bitsPerCell == 0) || (pNfInfo->bitsPerCell > 3))
        {
            pNfInfo->bitsPerCell = 1;
        }

        if (pNfInfo->bitsPerCell == 3)
        {
            pNfInfo->numPseudoPlane   = cNumPseudoPlaneForTlc;
            pNfInfo->wlPerBlock       = pNfInfo->pagesPerBlock / pNfInfo->bitsPerCell;
            pNfInfo->pagesPerProgUnit = 3;
        }
        else
        {
            pNfInfo->pagesPerProgUnit = 1;
            pNfInfo->numPseudoPlane   = 0;
            pNfInfo->wlPerBlock       = pNfInfo->pagesPerBlock / pNfInfo->bitsPerCell;
        }


        pNfInfo->userCapacity    = (pNfInfo->pagesPerBlock * pNfInfo->dataBytesPerPage / 1024 / 1024) * pNfInfo->userBlkPerLun / 1024 * (pNfInfo->numChannels * pNfInfo->devicesPerChannel * pNfInfo->lunsPerDevice);
        pNfInfo->rawNANDCapacity = pNfInfo->userCapacity;


        for (i = 0; i < FW_CAPACITY_COUNT; i++)
        {
            if (gCapacityMap[OP_7][i].CapacityInGb == pNfInfo->userCapacity)
            {
                pNfInfo->userCapacity = gCapacityMap[_OP_][i].CapacityInGb;
                break;
            }
        }

        ASSERT(i != FW_CAPACITY_COUNT);

        //D_PRINTF("pNfInfo->userCapacity %d\n",pNfInfo->userCapacity);
    }

    return (error);
}

//-----------------------------------------------------------------------------
/**
    Initialize NAND device configuration.\n
      - Reset all NAND flash
      - Read manufacture and spec Id
      - Scan all NAND flash
      - Read parameter page

    @param[out]  pNfInfo            NAND flash media information pointer

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Nfif_NandInitialization(InitBootMode_t initMode)
{
    NfCfgInfo_t tmpCfgInfo;

    if (initMode < cInitBootPowerDown)
    {
	 	MEM_COPY(&tmpCfgInfo, &gCfgInfo, sizeof(NfCfgInfo_t));
		gCfgInfo.reqIfData = NF_JEDEC_TOGGLE1;
		gCfgInfo.socIfData = NF_ONFI_SDR;
		gCfgInfo.reqXferMode = NF_XFER_MODE_3;
		Pkg_Configure(&gCfgInfo);

		Pkg_GetAdditionalInfo();
        Pkg_ChangeVoltageReference();

		MEM_COPY(&gCfgInfo, &tmpCfgInfo, sizeof(NfCfgInfo_t));
    }

    gCfgInfo.reqIfData      = gCfgInfo.ifData;
    gCfgInfo.reqXferMode    = gCfgInfo.xferMode;

    Pkg_Configure(&gCfgInfo);       // Target NAND flash packag

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Find manufacture Id, Interface type.\n

    @param[in]  pNfInfo             NAND flash information pointer

    @return interface type
**/
//-----------------------------------------------------------------------------
static Error_t Nfif_FindInterfaceType(NfCfgInfo_t* pCfgInfo)
{
    Pna_t addr;
    uint8_t id[6];

    pCfgInfo->ifType = NF_UNKNOWN;
    pCfgInfo->ifData = NF_ASYNC;

    for (addr.dev = 0; addr.dev < MAX_SUPPORTED_DEVICES; addr.dev++)
    {
        for (addr.ch = 0; addr.ch <  MAX_SUPPORTED_CHANNELS; addr.ch++)
        {
            #if 0
            addr.row = FC_ID_ADDR_ONFI;
            HalNfif_ReadId(&addr, id, 4, DIRECT);        // read ONFI signature
            if (id[0]=='O' && id[1]=='N' && id[2]=='F' && id[3]=='I')
            {
                //D_PRINTF("[ONFI]");
                pNfInfo->ifType = NF_ONFI;
                pNfInfo->ifData = NF_ONFI_SDR;
                return cEcNoError;
            }
            #endif

            addr.row = FC_ID_ADDR_JEDEC;
            HalNfif_ReadId(&addr, id, 6, DIRECT);        // get JEDEC signature
            if (id[0]=='J' && id[1]=='E' && id[2]=='D' && id[3]=='E' && id[4]=='C')
            {
                pCfgInfo->ifType = NF_JEDEC;
                pCfgInfo->ifData = (id[5] & B_0) ? NF_ASYNC : NF_JEDEC_TOGGLE1;
                return cEcNoError;
            }
        }
    }
    return cEcNfifUnknownTarget;
}

//-----------------------------------------------------------------------------
/**
    Scan target NAND flash module.\n
      - find out number of channel, device

    @param[in]  devicesPerCh        devices per channel
    @param[in]  numChannels         number of channels
    @param[in]  pNfInfo             NAND flash information pointer

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Nfif_ScanTargets(uint32_t devicesPerCh, uint32_t numChannels, NfInfo_t* pNfInfo)
{
    Pna_t addr;
    uint8_t id[8];
    uint8_t prevId[8];
    uint8_t lastChannel;
    uint8_t lastDevice;
    uint8_t flag;

    //D_PRINTF("[SCAN NANDs]\n");
    flag        = 0;
    lastChannel = 0;
    lastDevice  = 0;

    addr.row = FC_ID_ADDR_MANFUACTURER; // read manufacturer Id

    // Scan for installed NAND flash modules
    for (addr.dev = 0; addr.dev < devicesPerCh; addr.dev++)
    {
        for (addr.ch = 0; addr.ch <  numChannels; addr.ch++)
        {
            HalNfif_ReadId(&addr, id, sizeof(id), DIRECT);

            // print if id is different from previous one
            if (MEM_CMP(id, prevId, sizeof(prevId))!=0 && id[0]!=0x00)
            {
                D_PRINTF("[NFIF] CH|%1X DEV|%1X ST|%2X ID|%2X %2X %2X %2X %2X %2X - %s\n",
                          addr.ch, addr.dev, HalNfif_ReadStatus(&addr, DIRECT),
                          id[0], id[1], id[2], id[3], id[4], id[5], Pkg_GetDescriptionString(id));
                MEM_COPY(prevId,     id, sizeof(id));
                MEM_COPY(gNfInfo.id, id, sizeof(gNfInfo.id));
            }

            // if any valid Id found,
            if (id[0]!=0x00 && id[0]!=0xFF)
            {
                lastChannel  = MAX(addr.ch, lastChannel);
                if((addr.dev - lastDevice) == 1)
                {
                    lastDevice = MAX(addr.dev, lastDevice);
                }
                flag = 1;
            }
        }
    }

    if ((lastChannel == 0) && (lastDevice==0) && (flag==0))
    {
        E_PRINTF(cEcNfifNoFound, "[NFIF] Couldn't find NAND package.\n");
        return cEcNfifNoFound;
    }

    pNfInfo->numChannels = lastChannel+1;
    pNfInfo->devicesPerChannel = lastDevice+1;

    D_PRINTF("[NFIF] %u CH %u CE\n", pNfInfo->numChannels, pNfInfo->devicesPerChannel);

    return cEcNoError ;
}

//-----------------------------------------------------------------------------
/**
    Read JEDEC NAND flash parameter page.\n

    @param[out]  pNfInfo            NAND flash media information pointer

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Nfif_ReadNandFlashParameters(InitBootMode_t initMode)
{
    Error_t  error;

    #if CHK_FW_TYPE(FW_AGING)
    MEM_CLR((PVOID)&smAgingNandInfo, sizeof(tNAND_INFO));
    #endif

    error = Pkg_OneTimeInit(initMode);
    return(error);
}

//-----------------------------------------------------------------------------
/**
    Switch IfData to requested IfData.\n

    @param[in]  reqIfData          requested Interface
    @param[in]  pNfInfo            NAND flash media information pointer

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Nfif_SwitchIfData(NfIfData_t reqIfData, NfCfgInfo_t* pCfgInfo)
{
    Error_t error = cEcNoError;
    Pna_t addr;
    uint8_t writeData[4];
    uint8_t readData[4];

    MEM_SET(writeData, 0, sizeof(writeData));
    if( reqIfData == NF_JEDEC_TOGGLE1)
    {
        writeData[0] = NF_JEDEC_IF_TOGGLE;
    }
    else
    {
        writeData[0] = NF_JEDEC_IF_LEGACY;
    }

    addr.row = FC_FEATURE_ADDR_IF_CHANGE;
    for (addr.ch = 0; addr.ch < gNfInfo.numChannels; addr.ch++)
    {
        for (addr.dev = 0; addr.dev < gNfInfo.devicesPerChannel; addr.dev++)
        {
            HalNfif_SetFeature(&addr, writeData, DIRECT);
        }
    }
    pCfgInfo->curIfData = reqIfData;

    for (addr.ch = 0; addr.ch < gNfInfo.numChannels; addr.ch++)
    {
        for (addr.dev = 0; addr.dev < gNfInfo.devicesPerChannel; addr.dev++)
        {
            HalNfif_GetFeature(&addr, readData, DIRECT);
            if((writeData[0] != readData[0]) || (writeData[1] != readData[1]) || (writeData[2] != readData[2]) || (writeData[3] != readData[3]))
            {
               E_PRINTF(cEcNfifFeatureMismatch, "[NFIF] CH#%1X DEV#%1X FA#%2X -> 0x%02X%02X%02X%02X!=0x%02X%02X%02X%02X\n",
                        addr.ch, addr.dev, addr.row,
                        writeData[3], writeData[2], writeData[1], writeData[0],
                        readData[3], readData[2], readData[1], readData[0]);
               error = cEcMioGeneral;
            }
        }
    }
    return(error);
}

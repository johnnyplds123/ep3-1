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
//! \file
//! @brief LDPC Scheduler loader API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "MemIo.h"
#include "MemAlloc.h"
#include "HalLdpcSch.h"
#include "FastSection.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define ENCODER                         (0)
#define DECODER                         (1)
#define TOTAL_ENC_DEC                   (2)
#define EACH_SIZE                       (0)   ///< ITR scheduler size table: column 0
#define ACCUM_SIZE                      (1)   ///< ITR scheduler size table: column 1
#define DELAY_TO_WAIT_FOR_SCH           (200000)
#define POS_ENC(NIDX)                   ((NIDX) * TOTAL_ENC_DEC + ENCODER)
#define POS_DEC(NIDX)                   ((NIDX) * TOTAL_ENC_DEC + DECODER)
#define LDPC_WAIT_CONFIG_INIT(X)        while((X))  ;
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static Error_t ECU_DownloadLdpcEnc(uint8_t nSelectCode);
static Error_t ECU_DownloadLdpcDec(uint8_t nSelectCode);
static void   ECU_MemCopyScheulderFile(uint8_t nSelectCode, uint8_t nIsDec, const uint16_t* pSchCode);
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

extern void ECU_LDPC_InitSchManager(void);


//-----------------------------------------------------------------------------
/**
    Download LDPC scheduler for decoder.

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalEcu_DownloadLdpc(void)
{
    uint8_t nCount;

    // Initialize Scheduler manager.
    ECU_LDPC_InitSchManager();

    for(nCount=0; nCount<NUMBER_OF_LDPC_CORE; nCount++)
    {
        if(gLdpcSchManager[nCount].nEnable)
        {
            ECU_DownloadLdpcEnc(nCount);
            ECU_DownloadLdpcDec(nCount);
        }
    }
    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Download LDPC scheduler for encoder.

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t ECU_DownloadLdpcEnc(uint8_t nSelectCode)
{
    uint32_t i;
    uint16_t data;
    uint16_t *pBuf;
    uint32_t nFlagReady = FALSE;

    //D_PRINTF("[%d] %s LDPC Encoder Scheduler Write..", nSelectCode, gLdpcSchManager[nSelectCode].pSchName);

    rEcu.ecCore0ItrCoreItrEncCtrl.b.ECC0_ITR_ENC_CONFIG_MODE_ON    = 1;
    rEcu.ecCore0ItrCoreItrEncCtrl.b.ECC0_ITR_ENC_CONFIG_RD_WRB     = 0; ///< Write mode
    rEcu.ecCore0ItrCoreItrEncCtrl.b.ECC0_ITR_ENC_CONFIG_CODESEL0   = (nSelectCode)    & 0x1;
    rEcu.ecCore0ItrCoreItrEncCtrl.b.ECC0_ITR_ENC_CONFIG_CODESEL1   = (nSelectCode>>1) & 0x1;
    rEcu.ecCore0ItrCoreItrEncCtrl.b.ECC0_ITR_ENC_CONFIG_INIT       = 1; ///< Initialize to write

    // Wait until stablized.
    LDPC_WAIT_CONFIG_INIT(rEcu.ecCore0ItrCoreItrEncCtrl.b.ECC0_ITR_ENC_CONFIG_INIT);

    // Obtain the buffer pointer of Decoder scheduler.
    pBuf = (uint16_t *)gLdpcSchManager[nSelectCode].pEncBuf;

//    D_PRINTF("pBuf : %x", pBuf);

    // Start to write actual scheuler data.
    for (i = 0; i < gLdpcSchManager[nSelectCode].nEncSize; i++)
    {
        rEcu.ECC0_ITR_ENC_CONFIG_RWDATA_IO = pBuf[i];
    }
    // Check the result of writing LDPC scheduler.
    for (i = 0; i < DELAY_TO_WAIT_FOR_SCH; i++)
    {
       if (rEcu.ecCore0ItrCoreItrEncCtrl.b.ECC0_ITR_ENC_CONFIG_RDY == 1)
       {
           nFlagReady = TRUE;
           break;
       }
    }

    /// Set CONFIG mode to normal mode.
    rEcu.ecCore0ItrCoreItrEncCtrl.b.ECC0_ITR_ENC_CONFIG_MODE_ON = 0;   ///< off

    if (nFlagReady == FALSE)
    {
        D_PRINTF("ERROR: TIMEOUT for Ready!\n");
        return cEcError;
    }

    //D_PRINTF(".... Success\n");

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Download LDPC scheduler for decoder.

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t ECU_DownloadLdpcDec(uint8_t nSelectCode)
{
    uint32_t i, n=0;
    uint16_t data;
    uint16_t *pBuf;
    uint32_t nFlagReady = FALSE;

    //D_PRINTF("[%d] %s LDPC Decoder Scheduler Write..", nSelectCode, gLdpcSchManager[nSelectCode].pSchName);

    rEcu.ecCore0ItrDecCtrlAndStat.all = 0;
    rEcu.ecCore0ItrDecCtrlAndStat.b.ECC0_ITR_DEC_CONFIG_MODE_ON  = 1;
    rEcu.ecCore0ItrDecCtrlAndStat.b.ECC0_ITR_DEC_CONFIG_RD_WRB   = 0; ///< Write mode
    rEcu.ecCore0ItrDecCtrlAndStat.b.ECC0_ITR_DEC_CONFIG_CODESEL0 = (nSelectCode)    & 0x1;
    rEcu.ecCore0ItrDecCtrlAndStat.b.ECC0_ITR_DEC_CONFIG_CODESEL1 = (nSelectCode>>1) & 0x1;
    rEcu.ecCore0ItrDecCtrlAndStat.b.ECC0_ITR_DEC_CONFIG_INIT     = 1; ///< Initialize to write

    // Obtain the buffer pointer of Decoder scheduler.
    // Wait until stablized.
    LDPC_WAIT_CONFIG_INIT(rEcu.ecCore0ItrDecCtrlAndStat.b.ECC0_ITR_DEC_CONFIG_INIT);
    pBuf = (uint16_t *)gLdpcSchManager[nSelectCode].pDecBuf;
    // Start to write actual scheuler data.
    for (i = 0; i < gLdpcSchManager[nSelectCode].nDecSize; i++)
    {
        rEcu.ECC0_ITR_DEC_CONFIG_RWDATA_IO = pBuf[i];
    }

    // Check the result of writing LDPC scheduler.
    for (i = 0; i < DELAY_TO_WAIT_FOR_SCH; i++)
    {
       if (rEcu.ecCore0ItrDecCtrlAndStat.b.ECC0_ITR_DEC_CONFIG_RDY == 1)
       {
           nFlagReady = TRUE;
           break;
       }
    }
    /// Set CONFIG mode to normal mode.
    rEcu.ecCore0ItrDecCtrlAndStat.b.ECC0_ITR_DEC_CONFIG_MODE_ON = 0;    ///< off
    rEcu.ecCore0ItrDecCtrlAndStat.b.ECC0_ITR_DEC_CONFIG_RD_WRB  = 0;    ///< Write mode

    if (nFlagReady == FALSE)
    {
        D_PRINTF("ERROR: TIMEOUT for Ready!\n");
        return cEcError;
    }

    //D_PRINTF(".... Success\n");

    return cEcNoError;
}



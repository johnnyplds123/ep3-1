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
//! @brief FLC(Flash Link Controller) control API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "RegFlashLlc.h"
#include "HalFlc.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define GET_FLC_REAL_SEG(type, segNumber) (type == FLC_SEG_TYPE_WR) ? segNumber : (NUM_OF_WR_SEG + segNumber)
#define GET_BUF_ADDR_FROM_PTR(X)          ((uint32_t)(X))

#define GET_FLC_SSD_PTR_FROM_INDEX(I)     (&pFlcSsd[I])
#define GET_FLC_SSD_INDEX_FROM_PTR(P)     ARRAY_INDEX_FROM_PTR(pFlcSsd, P)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void     SetFlcSegTransferPointer(FlcSegType_t segType, uint32_t segNumber, void* pBuffer);
static void*    GetFlcSegTransferPointer(FlcSegType_t segType, uint32_t segNumber);
static void     SetFlcSegLinkedListPointer(FlcSegType_t segType, uint32_t segNumber, uint32_t linkedListPointer);
static uint32_t GetFlcSegLinkedListPointer(FlcSegType_t segType, uint32_t segNumber);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern tSSD*            smSSD;

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    One-time initialization of Flash Link Controller.\n

    @param[in]  initMode            initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalFlc_OneTimeInit(InitBootMode_t initMode)
{
    // Reset FLC block
    rFlashLlc.blkRstOrTstMonitorSelectio.b.FLC_RST = 1;
    rFlashLlc.blkRstOrTstMonitorSelectio.b.FLC_RST = 0;

    mFlashLlc.RD_SSD_START_ADDR                  = (U32)smSSD;//gFlcInfo.ssdTableStartAddr;
    mFlashLlc.WR_SSD_START_ADDR                  = (U32)smSSD;//gFlcInfo.ssdTableStartAddr;

    mFlashLlc.subsegmeDescriptMode.b.SSD_SIZE_MODE           = 1;    // SSD size = 12 bytes
    mFlashLlc.subsegmeDescriptMode.b.SSD_ADDR_MODE           = 1;    // SSD Address mode : A[31:0]
    mFlashLlc.subsegmeDescriptMode.b.SSD_END_ADDR_MODE       = 0;    // SSD End address mode : ssd_end_addr = ssd_start_addr + ssd_len
    mFlashLlc.subsegmeDescriptMode.b.SSD_LEN_MODE            = 0;    // SSD Length mode : length = ssd_end - ssd_start
    mFlashLlc.subsegmeDescriptMode.b.SSD_CNT_RD_EN           = 0;    // Disable SSD count for Read
    mFlashLlc.subsegmeDescriptMode.b.SSD_CNT_WR_EN           = 0;    // Disable SSD count for Write
    mFlashLlc.subsegmeDescriptMode.b.PUSH_ACK_DIS            = 0;    // FKC waut fir ECU's puch ack

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Set FLC Segment transfer pointer.\n

    @param[in]  segType             segment type
    @param[in]  segNumber           segment number
    @param[in]  pBuffer             buffer address to transfer
**/
//-----------------------------------------------------------------------------
static void SetFlcSegTransferPointer(FlcSegType_t segType, uint32_t segNumber, void* pBuffer)
{
    segNumber = GET_FLC_REAL_SEG(segType, segNumber);
    rFlashLlc.sgmXfr[segNumber] = GET_BUF_ADDR_FROM_PTR(pBuffer);
}


//-----------------------------------------------------------------------------
/**
    Get FLC Segment transfer pointer.\n

    @param[in]  segType             segment type
    @param[in]  segNumber           segment number
    @param[in]  pBuffer             buffer address to transfer
**/
//-----------------------------------------------------------------------------
static void* GetFlcSegTransferPointer(FlcSegType_t segType, uint32_t segNumber)
{
    segNumber = GET_FLC_REAL_SEG(segType, segNumber);
    return (void*)rFlashLlc.sgmXfr[segNumber];
}

//-----------------------------------------------------------------------------
/**
    Set FLC Segment linked list pointer.\n

    @param[in]  segType             segment type
    @param[in]  segNumber           segment number
    @param[in]  linkedListPointer   linked list pointer
**/
//-----------------------------------------------------------------------------
static void SetFlcSegLinkedListPointer(FlcSegType_t segType, uint32_t segNumber, uint32_t linkedListPointer)
{
    segNumber = GET_FLC_REAL_SEG(segType, segNumber);
    rFlashLlc.sgmLnkListPtr[segNumber].b.SEG_LLIST_PTR = linkedListPointer;
}

//-----------------------------------------------------------------------------
/**
    Get FLC Segment linked list pointer.\n

    @param[in]  segType             segment type
    @param[in]  segNumber           segment number
    @param[in]  linkedListPointer   linked list pointer
**/
//-----------------------------------------------------------------------------
static uint32_t GetFlcSegLinkedListPointer(FlcSegType_t segType, uint32_t segNumber)
{
    segNumber = GET_FLC_REAL_SEG(segType, segNumber);
    return rFlashLlc.sgmLnkListPtr[segNumber].b.SEG_LLIST_PTR;
}

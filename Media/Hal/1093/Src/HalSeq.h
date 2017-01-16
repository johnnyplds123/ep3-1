#pragma once
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
//! @brief Pure HAL API definition for SEQ(Sequencer) HW control
//!
//=============================================================================
#include "RegSequencer.h"

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define EF_FIFO_SIZE                    16384 /// 16KB
#define DS_ARRAY_SIZE                   64
#define DD_ARRAY_SIZE                   32

#define DESC_BYTE_SHIFT                 5    // 32 bytes/descriptor
#define DESC_IDS_SHIFT                  3
#define DESC_SIZE                       40   // 40 Bytes

// @name SEQ interrupt status
/// @{
#define SEQ_INT_NONE                    0x0000  ///< No status update
#define SEQ_INT_TRACK_DONE              0x0001  ///< Track Done
#define SEQ_INT_TRACK_ERROR             0x0002  ///< Track Error
#define SEQ_INT_ABORT_DONE              0x0004  ///< Abort Done
#define SEQ_INT_DES_INV                 0x0020  ///< Descriptor Valid
#define SEQ_INT_DNUM_OVER               0x0040  ///< Descriptor Maximum Overflow
#define SEQ_INT_TNUM_OVER               0x0080  ///< Track Maximum Overflow
#define SEQ_INT_ADEC_OVER               0x0100  ///< Decoder Done Count Overflow
#define SEQ_INT_AENC_OVER               0x0200  ///< Encoder Done Count Overflow
#define SEQ_INT_MQP3_FULL               0x0400  ///< MQ Port 3 Full
#define SEQ_INT_MQP2_FULL               0x0800  ///< MQ Port 2 Full
#define SEQ_INT_MQP1_FULL               0x1000  ///< MQ Port 1 Full
#define SEQ_INT_MQP0_FULL               0x2000  ///< MQ Port 0 Full
#define SEQ_INT_SAC_R_ERR               0x4000  ///< Sequencer AXI Read Error
#define SEQ_INT_SAC_W_ERR               0x8000  ///< Sequencer AXI Write Error
#define SEQ_INT_CRITIAL                 0xFCE4  //0xFFE4

#define MEDIA_DONE_ERROR_GROUP          (SEQ_INT_CRITIAL | SEQ_INT_TRACK_ERROR)
#define MEDIA_DONE_ISR_GROUP            (MEDIA_DONE_ERROR_GROUP | SEQ_INT_TRACK_DONE | SEQ_INT_TRACK_ERROR)

/// @}

#define SEQ_DESC_PER_DS_SHIFT           2
#define SEQ_DESC_PER_DS                 (1 << SEQ_DESC_PER_DS_SHIFT)

/// @brief SEQ physical page mode
typedef enum
{
    SEQ_PA_MODE_REG=0,      ///< # of AU in the desc. is determined by register
    SEQ_PA_MODE_DESC,       ///< # of AU in the desc. is determined by descriptor
    SEQ_PA_MODE_NULL=0x7FFFFFFF
} SeqPaMode_t;

/// @brief SEQ physical page mode
typedef enum
{
    SEQ_PA_RATIO_1_TO_1=0,  /// Physical page size to AU size ratio = 1 : 1
    SEQ_PA_RATIO_2_TO_1,    /// Physical page size to AU size ratio = 2 : 1
    SEQ_PA_RATIO_3_TO_1,    /// Physical page size to AU size ratio = 3 : 1
    SEQ_PA_RATIO_4_TO_1,    /// Physical page size to AU size ratio = 4 : 1
    SEQ_PA_RATIO_5_TO_1,    /// Physical page size to AU size ratio = 5 : 1
    SEQ_PA_RATIO_6_TO_1,    /// Physical page size to AU size ratio = 6 : 1
    SEQ_PA_RATIO_7_TO_1,    /// Physical page size to AU size ratio = 7 : 1
    SEQ_PA_RATIO_8_TO_1,    /// Physical page size to AU size ratio = 8 : 1
    SEQ_PA_RATIO_NULL=0x7FFFFFFF
} SeqPaRatio_t;

/// @brief SEQ data mode
typedef enum
{
    SEQ_DATA_MODE_META=0,   ///< META data mode
    SEQ_DATA_MODE_AUX,      ///< AUX data mode
    SEQ_DATA_MODE_NULL=0x7FFFFFFF
} SeqDataMode_t;

/// @brief SEQ AUX size
typedef enum
{
    SEQ_AUX_SIZE_NO=0,      ///< No AUX data
    SEQ_AUX_SIZE_4B,        ///< 4bytes of AUX data
    SEQ_AUX_SIZE_8B,        ///< 8bytes of AUX data
    SEQ_AUX_SIZE_12B,       ///< 12bytes of AUX data
    SEQ_AUX_SIZE_NULL=0x7FFFFFFF
} SeqAuxSize_t;

/// @brief SEQ LBA mode
typedef enum
{
    SEQ_LBA_MODE_8B=0,      ///< 8bytes LBA
    SEQ_LBA_MODE_4B,        ///< 4bytes LBA
    SEQ_LBA_MODE_NULL=0x7FFFFFFF
} SeqLbaMode_t;

/// @brief SEQ abort mode
typedef enum
{
    SEQ_ABT_MODE_NOW=0,     ///< Immediately abort
    SEQ_ABT_MODE_PENDING,   ///< Pending abort
    SEQ_ABT_MODE_GRACEFUL,  ///< Graceful abort
    SEQ_ABY_MODE_NULL=0x7FFFFFFF
} SeqAbtMode_t;

/// @brief SEQ request
typedef enum
{
    SEQ_REQ_NORMAL=0,       ///< Normal request
    SEQ_REQ_FAST,           ///< Fast request
    SEQ_REQ_NULL=0x7FFFFFFF
} SeqReq_t;

/// @brief SEQ callback
typedef enum
{
    SEQ_CB_TRK_DONE=0,      ///< TRK_DONE
    SEQ_CB_TRK_ERROR,       ///< TRK_ERROR
    SEQ_CB_MAX,
    SEQ_CB_NULL=0x7FFFFFFF
} SeqCb_t;

/// @brief SEQ Configuration function number
typedef enum
{
    SEQ_CFN_PA_MODE=0,      ///< PA Mode
    SEQ_CFN_PA_RATIO,       ///< PA Raio
    SEQ_CFN_DATA_MODE,      ///< AUX/META data mode
    SEQ_CFN_AUX_SIZE,       ///< AUX size
    SEQ_CFN_AUX_MASK,       ///< AUX Mask
    SEQ_CFN_LBA_MODE,       ///< LBA mode
    SEQ_CFN_MAX,
    SEQ_CFN_NULL=0x7FFFFFFF
} SeqCfn_t;

/// @brief SEQ track order type
typedef enum
{
    SEQ_TRK_IN_ORDER=0,
    SEQ_TRK_OUT_OF_ORDER=1
} SeqOrder_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define DESC_FIFO_UPDATE_R_PTR(I)     {gDes_fifo_r_ptr += I;}
#define DESC_FIFO_UPDATE_W_PTR(I)     {gDes_fifo_w_ptr += I;}

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef void (*SeqAppCallback_t)(uint32_t trkId, void* pStatus);

/// @brief TRK error information
typedef struct
{
    uint32_t trkPos;
    uint32_t trkLos;
    uint32_t descProc;
    uint32_t descTot;
    uint32_t errAuNum;
    uint32_t errMpNum;
    uint32_t errAuDid;
    uint32_t errAuType;
    uint32_t errStatus;
    uint32_t trkDescStatus[DS_ARRAY_SIZE];
    uint32_t trkDescDone[DD_ARRAY_SIZE];
} TrkErrInfo_t;

/// @brief SEQ Configuration
typedef struct
{
    SeqCfn_t cfn;
    uint32_t param[4];
} tSEQ_CFG;

/// @brief Sequencer FIFO structure
typedef struct
{
    uint8_t* pStart;        ///< start pointer
    uint8_t* pEnd;          ///< end pointer
    uint8_t* pCurrent;      ///< current pointer
} SeqFifo_t;

/// @brief SEQ FIFOs
typedef struct
{
    SeqFifo_t descriptor;
    SeqFifo_t auxInsert;
    SeqFifo_t auxCompare;
    SeqFifo_t auxExtract;
    SeqFifo_t hlbaInsert;
    SeqFifo_t hlbaCompare;
    SeqFifo_t errorCount;
    SeqFifo_t nandParameter;
    SeqFifo_t nandBusyTimeMonitor;
    SeqFifo_t fastDescriptor;
    SeqFifo_t fastAuxInsert;
    SeqFifo_t fastAuxCompare;
    SeqFifo_t fastHlbaCompare;
    SeqFifo_t compressionExtract;
    SeqFifo_t slowRetry;
    SeqFifo_t fastSlowRetry;
    SeqFifo_t compressionEncoder;
    SeqFifo_t fastCompressionEncoder;
    SeqFifo_t compressionDecoder;
    SeqFifo_t fastCompressionDecoder;
} SeqFifoInfo_t;

/// @brief SEQ information
typedef struct
{
    uint32_t         trkConf;
    SeqPaMode_t      paMode;
    SeqPaRatio_t     paRatio;
    SeqDataMode_t    dataMode;
    SeqAuxSize_t     auxSize;
    uint32_t         auxMask[3];
    SeqLbaMode_t     lbaMode;
    SeqAppCallback_t seqAppCallback[SEQ_CB_MAX];
} SeqInfo_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern SeqInfo_t     gSeqInfo;
extern SeqFifoInfo_t gSeqFifoInfo;

extern volatile U32 gDes_fifo_r_ptr;
extern volatile U32 gDes_fifo_w_ptr;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t          HalSeq_OneTimeInit(InitBootMode_t initMode);
Error_t          HalSeq_Configure(tSEQ_CFG *pSeqCfg);
Error_t          HalSeq_Abort(SeqAbtMode_t abortMode);
Error_t          HalSeq_PopNandBusyTimeMonitors(void* pData, uint32_t byteCount);
SeqAppCallback_t HalSeq_RegiserCallback(SeqCb_t cb, SeqAppCallback_t seqAppCallback);
void             HalSeq_SwitchTrackOrder(U32 order);
void             HalSeq_EnableSeq(void);
void             HalSeq_DisableSeq(void);
void             HalSeq_DumpGenDescriptor(void);
void             HalSeq_DumpInfo(void);

//-----------------------------------------------------------------------------
//  Inline Function
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    Add descriptor FIFO count.\n

    @param[in]  request             SEQ request type
    @param[in]  count               number of descriptor
**/
//-----------------------------------------------------------------------------
__inline void HalSeq_AddDescFifoCount(uint32_t count)
{
    rSeq.DF_CNT = count;
}

//-----------------------------------------------------------------------------
/**
    Get descriptor FIFO count.\n

    @param[in]  request             SEQ request type

    @return number of descriptor FIFO
**/
//-----------------------------------------------------------------------------
__inline uint32_t HalSeq_GetDescFifoCount(void)
{
    return (rSeq.DF_CNT);
}

//-----------------------------------------------------------------------------
/**
    Get descriptor done count.\n

    @return descriptor done count
**/
//-----------------------------------------------------------------------------
__inline uint32_t HalSeq_GetDescDoneCount(void)
{
    return rSeq.D_DONE_CNT;
}

//-------------------------------------------------------------------
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
__inline void HalSeq_FifoCheck(void)
{
    U32 cnt = 0;
    U32 descFreeCnt;

    while (TRUE)
    {
        descFreeCnt = (gDes_fifo_w_ptr > gDes_fifo_r_ptr) ? (0x100000000L - gDes_fifo_w_ptr + gDes_fifo_r_ptr) : (gDes_fifo_r_ptr - gDes_fifo_w_ptr);

        if (descFreeCnt < DESC_SIZE)
        {
            HalCpu_DelayUsec(5);

            if (cnt++ > 100000)
            {
                D_PRINTF("DescFIFO ASSERT-0 Wr PTR:0x%X RD PTR:0x%X DF_CNT:%X\n", gDes_fifo_w_ptr, gDes_fifo_r_ptr, HalSeq_GetDescFifoCount());
                ASSERT(FALSE);
            }
        }
        else
        {
            break;
        }
    }

    DESC_FIFO_UPDATE_W_PTR(DESC_SIZE);
}

//-------------------------------------------------------------------
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static __inline void HalSeq_PushDescriptors(PVOID pData, U32 byteCount)
{
    SeqFifo_t* pFifo = &gSeqFifoInfo.descriptor;

    HalSeq_FifoCheck();

    if ((pFifo->pCurrent + byteCount) <= pFifo->pEnd)
    {
        MemCopy10U32((U32*)pFifo->pCurrent, (U32*)pData);
        pFifo->pCurrent += byteCount;
    }
    else
    {
        U32 remainFreeSize;

        remainFreeSize = pFifo->pEnd - pFifo->pCurrent;

        if (remainFreeSize)
        {
            U32 remainDataSize;

            MEM_COPY(pFifo->pCurrent, pData, remainFreeSize);
            remainDataSize = byteCount - remainFreeSize;
            MEM_COPY(pFifo->pStart, ((U8 *)pData + remainFreeSize), remainDataSize);
            pFifo->pCurrent = pFifo->pStart + remainDataSize;
        }
        else
        {
            MemCopy10U32((U32*)pFifo->pStart, (U32*)pData);
            pFifo->pCurrent = pFifo->pStart + byteCount;
        }
    }
}
//-----------------------------------------------------------------------------
/**
    Push many data to AUX Insert FIFO.\n

    @param[in]  request             SEQ request type
    @param[in]  pData               data pointer
    @param[in]  byteCount           byte data count to push

    @return error code
**/
//-----------------------------------------------------------------------------
static __inline void HalSeq_PushAuxInserts(void* pData, uint32_t byteCount)
{
    uint32_t size1; // copy size
    uint32_t size2; // copy size for pointer wrap around
    SeqFifo_t* pFifo = &gSeqFifoInfo.auxInsert;

    size1 = (uint32_t)pFifo->pEnd - (uint32_t)pFifo->pCurrent;

    if (size1 != 0)
    {
        size1 = MIN(size1, byteCount);
        MEM_COPY(pFifo->pCurrent, pData, size1);
        pFifo->pCurrent = pFifo->pCurrent + size1;
    }

    if (size1 != byteCount) // check for pointer wrap around
    {
        size2 = byteCount-size1;
        MEM_COPY(pFifo->pStart, (uint8_t *)pData+size1, size2);
        pFifo->pCurrent = pFifo->pStart + size2;
    }
}

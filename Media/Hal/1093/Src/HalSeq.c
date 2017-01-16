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
//! @brief SEQ(Sequencer) control API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "RegSequencer.h"
#include "RegNandFlashIf.h"
#include "RegDge.h"
#include "RegSequencer.h"
#include "HalEcu.h"
#include "HalDescriptor.h"

#include "HalSeq.h"

#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define SEQ_MAX_TRACK_CANCEL_NUM        128
#define SEQ_MAX_LOS_NUM                 256

const uint32_t gSeqDdEntrySizePerTrk[] = {2, 8,  32, 128, 512};  ///< bytes
const uint32_t gSeqDsEntrySizePerTrk[] = {4, 16, 64, 256, 1024}; ///< bytes
const U32 gDsPlaneBitMask[3]  = {0x03030303, 0x07070707, 0x1F1F1F1F};
const U32 gErrPlaneBitMask[3] = {0x02020202, 0x06060606, 0x1E1E1E1E};

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define CAL_END_ADDRESS(X,Y)            (((X)%(Y))==0 ? ((X)==0) ? 0 : (X)-(Y) : __ct_[-1]) // Assert compile error if not aligned
#define GET_SEQ_TRK_OVERALLSTATUS1(X)   *(uint32_t *)(rSeq.TRKM_START + 0x3000 + (8 * X))
#define GET_SEQ_TRK_OVERALLSTATUS2(X)   *(uint32_t *)(rSeq.TRKM_START + 0x3000 + (8 * X) + 4)
#define GET_SEQ_TRK_DD(X,Y)             *(uint32_t *)(rSeq.TRKM_START + 0x2000 + (gSeqDdEntrySizePerTrk[gSeqInfo.trkConf] * X) + (4 * Y))
#define GET_SEQ_TRK_DS(X,Y)             *(uint32_t *)(rSeq.TRKM_START + (gSeqDsEntrySizePerTrk[gSeqInfo.trkConf] * X) + (4 * Y))

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief sequencer HW structure.
typedef struct
{
    uint32_t START;
    uint32_t END;
    uint32_t PTR;
} SeqHwFifo_t;

/// @brief SEQ FIFOs
typedef struct
{
    uint32_t aencStart;
    uint32_t adecStart;
    uint32_t seqLutSaddr;
    uint32_t seqAencLutSaddr;
    uint32_t seqAdecLutSaddr;
    uint32_t mqp0Start;
    uint32_t mqp1Start;
    uint32_t mqp2Start;
    uint32_t mqp3Start;
    uint32_t trkmStart;
    uint32_t hrpjStart;
} SeqAllcInfo_t;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static Error_t  SEQ_PushFifo(SeqFifo_t* pFifo, void* pData, uint32_t count);
static Error_t  SEQ_PopFifo(SeqFifo_t* pFifo, void* pData, uint32_t count);
static void     SEQ_SetupSeqFifo(InitBootMode_t initMode);
static void     SEQ_FifoAlloc(SeqFifo_t* pFifo, uint32_t fifoSize, uint32_t align);
static void     SEQ_SetFifoRegisters(SeqFifo_t* pFifo, uint32_t fifoSize, uint32_t align, SeqHwFifo_t* pHwFifo);
static void     SEQ_InitMiscMemories(void);
static void     SEQ_LoadDescriptorStatus(uint32_t trkId);
static void     SEQ_MediaErrorIsr(uint32_t trkId);
static void     SEQ_MediaDoneIsr(uint32_t param);
static void     SEQ_InitPrpAvCount(uint16_t initVal);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
SeqFifoInfo_t gSeqFifoInfo;
SeqAllcInfo_t gSeqAllcInfo;
SeqInfo_t     gSeqInfo;
TrkErrInfo_t  gTrkErrorInfo;
uint16_t      seqInterruptMskInv;

volatile U32 gDes_fifo_r_ptr;
volatile U32 gDes_fifo_w_ptr;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    One-time initialization of SEQ.\n

    @param[in]  initMode            initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalSeq_OneTimeInit(InitBootMode_t initMode)
{
    tSEQ_CFG seqCfg;
    uint16_t interruptStatus;

    if (initMode < cInitBootPowerDown)
    {
        MEM_CLR(&gSeqInfo,sizeof(SeqInfo_t));
    }

    gSeqInfo.trkConf                = TRK_CONF_NUM;
    gSeqInfo.paMode                 = SEQ_PA_MODE_DESC;
    gSeqInfo.paRatio                = SEQ_PA_RATIO_1_TO_1;
    gSeqInfo.dataMode               = (SeqDataMode_t)META_DATA_MODE;
    gSeqInfo.auxSize                = (SeqAuxSize_t)(META_DATA_SIZE / 4);
    gSeqInfo.auxMask[0]             = 0;
    gSeqInfo.auxMask[1]             = 0xFFFFFFFF;
    gSeqInfo.auxMask[2]             = 0xFFFFFFFF;
    gSeqInfo.lbaMode                = SEQ_LBA_MODE_4B;

    HalSeq_DisableSeq();

    rSeq.seqCtrl1.b.IO_AUX_XTRCT_EN = 0;   ///< 0= ooo; 1= in order

    rSeq.seqCfg1.b.DIS_HOLD_TD      = 0;   ///< Enable holding TRK_DONE/TRK_ERROR
    rSeq.seqCfg1.b.TRK_CONF         = gSeqInfo.trkConf;
    rSeq.seqCfg1.b.TRK_ORDR         = 1;   ///< TRK report(Out of order)

    rSeq.seqCfg2.b.SQCR_CLK_DIS     = 0;   ///< Enable SEQ clock
    rSeq.seqCfg2.b.CLK_AUTOPD       = 1;   ///< DIsable SEQ clock auto power down

    rSeq.seqCfg3.b.TWO_LBA_EN       = 0;   ///< one LBA in one AU
    rSeq.seqCfg3.b.AUX_SIZE         = gSeqInfo.auxSize;
    rSeq.seqCfg3.b.AUX_META_SEL     = gSeqInfo.dataMode;
    rSeq.seqCfg3.b.LBA_4B_MODE      = gSeqInfo.lbaMode;
    rSeq.seqCfg3.b.ERR_CNT_EN       = 0;   ///< Disable ERROR Count FIFO

    //TODO: Register define should be used when internal registers are clarified.
    {
        uint16_t* pRegister;

        pRegister = (uint16_t*)0xF200D008;
        *pRegister |= 0x0400;               ///< Enable aborted track report(RD008h[10])
    }

    rSeq.MAX_ACT_TRK                = MAX_TRK_NUM;
    rSeq.trkStatId.b.TRK_ID         = 0;

    // Disable AU DEC/ENC overflow interrupt
    rSeq.seqIntrMsk.b.ADEC_OVER_M   = 1;
    rSeq.seqIntrMsk.b.AENC_OVER_M   = 1;
    seqInterruptMskInv              = ~rSeq.seqIntrMsk.all;

    rSeq.REF_MASK_1                 = gSeqInfo.auxMask[0];
    rSeq.REF_MASK_2                 = gSeqInfo.auxMask[1];
    rSeq.REF_MASK_3                 = gSeqInfo.auxMask[2];

    SEQ_InitMiscMemories();

    SEQ_SetupSeqFifo(initMode);

    rNandFlashIf.flashSeqCtrl1.b.FSEQ_RESET = 1;        // Flash Sequencer Logic Reset
    rNandFlashIf.flashSeqCtrl1.b.FSEQ_RESET = 0;

    HalSeq_EnableSeq();

    HalIrq_RegisterHandler(cIrqMediaDone, SEQ_MediaDoneIsr, ENABLE);

    rSeq.seqCfg3.b.PRP_CHK_EN = 0;                  // PRP Check disable for Eldora2.0
    SEQ_InitPrpAvCount(0x100);                      // Initialize PRP AV COUNT = 256.

    ///< Clear all status by reset and fifo setup
    interruptStatus                 = rSeq.seqIntrStat.all;     ///< Get SEQ interupt
    interruptStatus                &= seqInterruptMskInv;
    rSeq.seqIntrStat.all            = interruptStatus;          ///< Clear all asserted interrupt

    mFct.nandFlashControllIntrEnForSocCpu.b.SEQ_INT_EN  = 1;   ///< Enable FCT.SEQ interrupt

    #if _NAND_ == NAND_3DTLCG2
    rSeq.seqCtrl1.b.TLC_SCRAMBLE_EN = 1;
    #endif

    gDes_fifo_r_ptr = DF_FIFO_SIZE;
    gDes_fifo_w_ptr = 0;

    seqCfg.cfn      = SEQ_CFN_PA_RATIO;
    seqCfg.param[0] = ((gNfInfo.dataBytesPerPage + gNfInfo.spareBytesPerPage) / AU_SIZE) - 1;
    HalSeq_Configure(&seqCfg);     // configure SEQ

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Configure SEQ.\n

    @param[in]  pSeqCfg             SEQ Configuration pointer

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalSeq_Configure(tSEQ_CFG *pSeqCfg)
{
    switch (pSeqCfg->cfn)
    {
        case SEQ_CFN_PA_RATIO:
            gSeqInfo.paRatio = (SeqPaRatio_t)(pSeqCfg->param[0] & 0x07);
            break;
        case SEQ_CFN_DATA_MODE:
            rSeq.seqCfg3.b.AUX_META_SEL = pSeqCfg->param[0] & 0x01;
            gSeqInfo.dataMode = (SeqDataMode_t)(pSeqCfg->param[0] & 0x01);
            break;
        case SEQ_CFN_AUX_SIZE:
            rSeq.seqCfg3.b.AUX_SIZE = pSeqCfg->param[0] & 0x03;
            gSeqInfo.auxSize = (SeqAuxSize_t)(pSeqCfg->param[0] & 0x03);
            break;
        case SEQ_CFN_AUX_MASK:
            rSeq.REF_MASK_1 = pSeqCfg->param[0];
            rSeq.REF_MASK_2 = pSeqCfg->param[1];
            rSeq.REF_MASK_3 = pSeqCfg->param[2];
            gSeqInfo.auxMask[0] = pSeqCfg->param[0];
            gSeqInfo.auxMask[1] = pSeqCfg->param[1];
            gSeqInfo.auxMask[2] = pSeqCfg->param[2];
            break;
        case SEQ_CFN_LBA_MODE:
            rSeq.seqCfg3.b.LBA_4B_MODE = pSeqCfg->param[0] & 0x01;
            gSeqInfo.lbaMode = (SeqLbaMode_t)(pSeqCfg->param[0] & 0x01);
            break;
        default:
            E_PRINTF(cEcSeqInvalidCfn, "SEQ config NOT found (%d)\n", pSeqCfg->cfn);
            return cEcSeqInvalidCfn;
    }

    return cEcNoError;
}

#include "PerfSection.h"
//-----------------------------------------------------------------------------
/**
    media flash I/O scheduler

    @param[in]  order
**/
//-----------------------------------------------------------------------------
void HalSeq_SwitchTrackOrder(U32 order)
{
    HalSeq_Abort(SEQ_ABT_MODE_NOW);

    if (order == SEQ_TRK_IN_ORDER)
    {
        rSeq.seqCtrl1.b.IO_AUX_XTRCT_EN = 1;
        rSeq.seqCfg1.b.TRK_ORDR         = 0;//in order
    }
    else
    {
        rSeq.seqCtrl1.b.IO_AUX_XTRCT_EN = 0;
        rSeq.seqCfg1.b.TRK_ORDR         = 1;//out of order
    }
}

//-----------------------------------------------------------------------------
/**
    Abort SEQ.\n

    @param[in]  abortMode           abort mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalSeq_Abort(SeqAbtMode_t abortMode)
{
    rSeq.seqCtrl1.b.SEQ_ABT_MODE = abortMode;

    HalSeq_DisableSeq();

    rNandFlashIf.flashSeqCtrl1.b.FSEQ_RESET = 1;
    rNandFlashIf.flashSeqCtrl1.b.FSEQ_RESET = 0;

    HalSeq_EnableSeq();

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Register SEQ APP callback function.\n

    @param[in]  cb                  SEQ callback type
    @param[in]  seqCallback         SEQ callback function pointer

    @return previous SEQ callback function pointer
**/
//-----------------------------------------------------------------------------
SeqAppCallback_t HalSeq_RegiserCallback(SeqCb_t cb, SeqAppCallback_t seqAppCallback)
{
    SeqAppCallback_t orgSeqAppCallback;

    orgSeqAppCallback = gSeqInfo.seqAppCallback[cb];
    gSeqInfo.seqAppCallback[cb] = seqAppCallback;

    return orgSeqAppCallback;
}

//-----------------------------------------------------------------------------
/**
    Pop many data from AUX Extract FIFO.\n

    @param[in]  pData               data pointer
    @param[in]  byteCount           byte data count to ppp

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalSeq_PopAuxExtracts(void* pData, uint32_t byteCount)
{
    if (rSeq.seqCtrl1.b.IO_AUX_XTRCT_EN == 0)
    {
        byteCount += 4;
    }

    return SEQ_PopFifo(&gSeqFifoInfo.auxExtract, pData, byteCount);
}

//-----------------------------------------------------------------------------
/**
    Pop many data from ERROR Count FIFO.\n

    @param[in]  pData               data pointer
    @param[in]  byteCount           byte data count to ppp

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalSeq_PopErrorCounts(void* pData, uint32_t byteCount)
{
    return SEQ_PopFifo(&gSeqFifoInfo.errorCount, pData, byteCount);
}
//-----------------------------------------------------------------------------
/**
    Pop many data from NAND Parameter FIFO.\n

    @param[in]  pData               data pointer
    @param[in]  byteCount           byte data count to ppp

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalSeq_PopNandBusyTimeMonitors(void* pData, uint32_t byteCount)
{
    return SEQ_PopFifo(&gSeqFifoInfo.nandBusyTimeMonitor, pData, byteCount);
}
//-----------------------------------------------------------------------------
/**
   Enable SEQ.\n
**/
//-----------------------------------------------------------------------------
void HalSeq_EnableSeq(void)
{
    rSeq.seqCtrl1.b.SEQ_RESET = 1;      ///< Reset SEQ

    while(rSeq.seqCtrl1.b.SEQ_BUSY);    ///< Wait Busy resetting

    rSeq.seqCtrl1.b.SEQ_ENABLE = 1;

    while(rSeq.seqCtrl1.b.SEQ_BUSY); // wait for busy clear
}

//-----------------------------------------------------------------------------
/**
   Disable SEQ.\n
**/
//-----------------------------------------------------------------------------
void HalSeq_DisableSeq(void)
{
    rSeq.seqCtrl1.b.SEQ_ENABLE = 0;

    while(rSeq.seqCtrl1.b.SEQ_BUSY); // wait for busy clear
}

#include "NormSection.h"
//-----------------------------------------------------------------------------
/**
    Dump SEQ information.\n
**/
//-----------------------------------------------------------------------------
void HalSeq_DumpInfo(void)
{
    M_PRINTF("\n[SEQ]\n");
    M_PRINTF("TrkConf:           %9u\n", gSeqInfo.trkConf);
    M_PRINTF("PhysicalPageRatio: %7u:1\n", gSeqInfo.paRatio + 1);
    M_PRINTF("DataMode:          %s\n", (gSeqInfo.dataMode == SEQ_DATA_MODE_META) ? "META" : "AUX");
    M_PRINTF("AuxSize:           %9u bytes\n", (uint8_t)(gSeqInfo.auxSize) * 4);
    M_PRINTF("AuxMask1:          %9X\n", gSeqInfo.auxMask[0]);
    M_PRINTF("AuxMask2:          %9X\n", gSeqInfo.auxMask[1]);
    M_PRINTF("AuxMask3:          %9X\n", gSeqInfo.auxMask[2]);
    M_PRINTF("LbaMode:           %9u bytes\n", (gSeqInfo.lbaMode == SEQ_LBA_MODE_4B) ? 4 : 8);

    #if 0
    M_PRINTF("[FIFO information]\n");

    M_PRINTF("\nLun[%d]Pblk[%d]Plane[%d]\n",gNfInfo.lunLsbPosition,gNfInfo.pairedBlkLsbPos,gNfInfo.planeLsbPosition);
    M_PRINTF("\nDescriptor FIFO    S:%08X E:%08X [%08X] HW:%08X Oft[%X] FW:%08X[%X] \n", rSeq.DF_START, rSeq.DF_END + DF_FIFO_ALIGN - 1, DF_FIFO_SIZE, rSeq.DF_PTR, (rSeq.DF_PTR -rSeq.DF_START)/40, (uint32_t)gSeqFifoInfo.descriptor.pCurrent, (gSeqFifoInfo.descriptor.pCurrent -(uint32_t)gSeqFifoInfo.descriptor.pCurrent));

    M_PRINTF("AUX INS FIFO       S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.AIF_START, (AIF_FIFO_SIZE > 0) ? (rSeq.AIF_END + AIF_FIFO_ALIGN - 1) : rSeq.AIF_END, AIF_FIFO_SIZE, rSeq.AIF_PTR, (uint32_t)gSeqFifoInfo.auxInsert.pCurrent);
    M_PRINTF("AUX CMP FIFO       S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.ACF_START, (ACF_FIFO_SIZE > 0) ? (rSeq.ACF_END + ACF_FIFO_ALIGN - 1) : rSeq.ACF_END, ACF_FIFO_SIZE, rSeq.ACF_PTR, (uint32_t)gSeqFifoInfo.auxCompare.pCurrent);
    M_PRINTF("AUX EXT FIFO       S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.AEF_START, (AEF_FIFO_SIZE > 0) ? (rSeq.AEF_END + AEF_FIFO_ALIGN + (4 * rSeq.seqCfg3.b.AUX_SIZE) - 1) : rSeq.AEF_END, AEF_FIFO_SIZE, rSeq.AEF_PTR, (uint32_t)gSeqFifoInfo.auxExtract.pCurrent);
    M_PRINTF("HLBA CMP FIFO      S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.HCF_START, (HCF_FIFO_SIZE > 0) ? (rSeq.HCF_END + HCF_FIFO_ALIGN - 1) : rSeq.HCF_END, HCF_FIFO_SIZE, rSeq.HCF_PTR, (uint32_t)gSeqFifoInfo.hlbaCompare.pCurrent);
    M_PRINTF("ERROR COUNT FIFO   S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.ECF_START, (ECF_FIFO_SIZE > 0) ? (rSeq.ECF_END + ECF_FIFO_ALIGN - 1) : rSeq.ECF_END, ECF_FIFO_SIZE, rSeq.ECF_PTR, (uint32_t)gSeqFifoInfo.errorCount.pCurrent);
    M_PRINTF("AU ENC CMD FIFO    S:%08X E:%08X [%08X]\n", rSeq.AENC_START, (AENC_FIFO_SIZE > 0) ? ((uint32_t)rSeq.AENC_START + AENC_FIFO_SIZE - 1) : 0, rSeq.AENC_SIZE * AENC_FIFO_ENTRY_SIZE);
    M_PRINTF("AU DEC CMD FIFO    S:%08X E:%08X [%08X]\n", rSeq.ADEC_START, (ADEC_FIFO_SIZE > 0) ? ((uint32_t)rSeq.ADEC_START + ADEC_FIFO_SIZE - 1) : 0, rSeq.ADEC_SIZE * ADEC_FIFO_ENTRY_SIZE);
    M_PRINTF("MQ Port0 FIFO      S:%08X E:%08X [%08X]\n", rSeq.MQP0_START, (MQP0_FIFO_SIZE > 0) ? ((uint32_t)rSeq.MQP0_START + MQP0_FIFO_SIZE - 1) : 0, rSeq.MQP0_SIZE * MQP0_FIFO_ENTRY_SIZE);
    M_PRINTF("MQ Port1 FIFO      S:%08X E:%08X [%08X]\n", rSeq.MQP1_START, (MQP1_FIFO_SIZE > 0) ? ((uint32_t)rSeq.MQP1_START + MQP1_FIFO_SIZE - 1) : 0, rSeq.MQP1_SIZE * MQP1_FIFO_ENTRY_SIZE);
    M_PRINTF("MQ Port2 FIFO      S:%08X E:%08X [%08X]\n", rSeq.MQP2_START, (MQP2_FIFO_SIZE > 0) ? ((uint32_t)rSeq.MQP2_START + MQP2_FIFO_SIZE - 1) : 0, rSeq.MQP2_SIZE * MQP2_FIFO_ENTRY_SIZE);
    M_PRINTF("MQ Port3 FIFO      S:%08X E:%08X [%08X]\n", rSeq.MQP3_START, (MQP3_FIFO_SIZE > 0) ? ((uint32_t)rSeq.MQP3_START + MQP3_FIFO_SIZE - 1) : 0, rSeq.MQP3_SIZE * MQP3_FIFO_ENTRY_SIZE);
    M_PRINTF("Track Memory       S:%08X E:%08X [%08X]\n", rSeq.TRKM_START, (uint32_t)rSeq.TRKM_START + TRKM_SIZE - 1, TRKM_SIZE);
    M_PRINTF("NAND Param FIFO    S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.GETP_START, (GETP_FIFO_SIZE > 0) ? (rSeq.GETP_END + GETP_FIFO_ALIGN - 1) : rSeq.GETP_END, GETP_FIFO_SIZE, rSeq.GETP_PTR, (uint32_t)gSeqFifoInfo.nandParameter.pCurrent);
    M_PRINTF("NAND BTM FIFO      S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.BTMF_START, (BTMF_FIFO_SIZE > 0) ? (rSeq.BTMF_END + BTMF_FIFO_ALIGN - 1) : rSeq.BTMF_END, BTMF_FIFO_SIZE, rSeq.BTMF_PTR, (uint32_t)gSeqFifoInfo.nandBusyTimeMonitor.pCurrent);
    M_PRINTF("SEQ LUT            S:%08X E:%08X [%08X]\n", rSeq.SEQ_LUT_SADDR, (uint32_t)rSeq.SEQ_LUT_SADDR + LUT_SIZE - 1, LUT_SIZE);
    M_PRINTF("SEQ AENC LUT       S:%08X E:%08X [%08X]\n", rSeq.SEQ_AENC_LUT_SADDR, (uint32_t)rSeq.SEQ_AENC_LUT_SADDR + LUT_AU_ENC_SIZE- 1, LUT_AU_ENC_SIZE);
    M_PRINTF("SEQ ADEC LUT       S:%08X E:%08X [%08X]\n", rSeq.SEQ_ADEC_LUT_SADDR, (uint32_t)rSeq.SEQ_ADEC_LUT_SADDR + LUT_AU_DEC_SIZE- 1, LUT_AU_DEC_SIZE);
    M_PRINTF("Comp ENC FIFO      S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.CEF_START, (CEF_FIFO_SIZE > 0) ? (rSeq.CEF_END + CEF_FIFO_ALIGN - 1) : rSeq.CEF_END, CEF_FIFO_SIZE, rSeq.CEF_PTR1, (uint32_t)gSeqFifoInfo.compressionEncoder.pCurrent);
    M_PRINTF("Comp DEC FIFO      S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.CDF_START, (CDF_FIFO_SIZE > 0) ? (rSeq.CDF_END + CDF_FIFO_ALIGN - 1) : rSeq.CDF_END, CDF_FIFO_SIZE, rSeq.CDF_PTR1, (uint32_t)gSeqFifoInfo.compressionDecoder.pCurrent);
    M_PRINTF("Comp EXT FIFO      S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.CXF_START, (CXF_FIFO_SIZE > 0) ? (rSeq.CXF_END + CXF_FIFO_ALIGN - 1) : rSeq.CXF_END, CXF_FIFO_SIZE, rSeq.CXF_PTR, (uint32_t)gSeqFifoInfo.compressionExtract.pCurrent);
    M_PRINTF("Slow Retry FIFO    S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.SRF_START, (SRF_FIFO_SIZE > 0) ? (rSeq.SRF_END + SRF_FIFO_ALIGN - 1) : rSeq.SRF_END, SRF_FIFO_SIZE, rSeq.SRF_PTR, (uint32_t)gSeqFifoInfo.slowRetry.pCurrent);
    M_PRINTF("(F)Descriptor FIFO S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.FDF_START, (FDF_FIFO_SIZE > 0) ? (rSeq.FDF_END + FDF_FIFO_ALIGN - 1) : rSeq.FDF_END, FDF_FIFO_SIZE, rSeq.FDF_PTR, (uint32_t)gSeqFifoInfo.fastDescriptor.pCurrent);
    M_PRINTF("(F)AUX INS FIFO    S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.FAIF_START, (FAIF_FIFO_SIZE > 0) ? (rSeq.FAIF_END + FAIF_FIFO_ALIGN - 1) : rSeq.FAIF_END, FAIF_FIFO_SIZE, rSeq.FAIF_PTR, (uint32_t)gSeqFifoInfo.fastAuxInsert.pCurrent);
    M_PRINTF("(F)AUX CMP FIFO    S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.FACF_START, (FACF_FIFO_SIZE > 0) ? (rSeq.FACF_END + FACF_FIFO_ALIGN - 1) : rSeq.FACF_END, FACF_FIFO_SIZE, rSeq.FACF_PTR, (uint32_t)gSeqFifoInfo.fastAuxCompare.pCurrent);
    M_PRINTF("(F)HLBA CMP FIFO   S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.FHCF_START, (FHCF_FIFO_SIZE > 0) ? (rSeq.FHCF_END + FHCF_FIFO_ALIGN - 1) : rSeq.FHCF_END, FHCF_FIFO_SIZE, rSeq.FHCF_PTR, (uint32_t)gSeqFifoInfo.fastHlbaCompare.pCurrent);
    M_PRINTF("(F)Comp ENC FIFO   S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.FCEF_START, (FCEF_FIFO_SIZE > 0) ? (rSeq.FCEF_END + FCEF_FIFO_ALIGN - 1) : rSeq.FCEF_END, FCEF_FIFO_SIZE, rSeq.FCEF_PTR1, (uint32_t)gSeqFifoInfo.fastCompressionEncoder.pCurrent);
    M_PRINTF("(F)Comp DEC FIFO   S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.FCDF_START, (FCDF_FIFO_SIZE > 0) ? (rSeq.FCDF_END + FCDF_FIFO_ALIGN - 1) : rSeq.FCDF_END, FCDF_FIFO_SIZE, rSeq.FCDF_PTR1, (uint32_t)gSeqFifoInfo.fastCompressionDecoder.pCurrent);
    M_PRINTF("(F)Slow Retry FIFO S:%08X E:%08X [%08X] HW:%08X FW:%08X \n", rSeq.FSRF_START, (FSRF_FIFO_SIZE > 0) ? (rSeq.FSRF_END + FSRF_FIFO_ALIGN - 1) : rSeq.FSRF_END, FSRF_FIFO_SIZE, rSeq.FSRF_PTR, (uint32_t)gSeqFifoInfo.fastSlowRetry.pCurrent);
    #endif
}

//-----------------------------------------------------------------------------
/**
    Dump Generic descriptor.\n

    @param[in]  pDesc            Generic descriptor pointer
    @param[in]  size             Descriptor size
**/
//-----------------------------------------------------------------------------
void HalSeq_DumpGenDescriptor(void)
{
    U32  idx, ds, dd;
    U32 *pBuf;
    U16  cnt, tid;

    //[[[1]] Dump Parameters like definitions, constants, ...
    M_PRINTF("\nDescriptor 0x%X =\n",(uint32_t *)gSeqFifoInfo.descriptor.pStart);

    pBuf = (uint32_t *)gSeqFifoInfo.descriptor.pStart;
    for(idx = 0; idx < (DF_FIFO_SIZE / DESC_SIZE); idx++)
    {
        M_PRINTF("[%3x] ", idx);
        for(cnt = 0; cnt < (DESC_SIZE / 4); cnt++)
        {
            for(cnt = 0; cnt < 10; cnt++)
            {
                M_PRINTF("%x ", *pBuf++);
            }
            M_PRINTF("\n");
        }
    }
    M_PRINTF("\n");

    M_PRINTF("\nTrack Memory");
    pBuf = (U32*)rSeq.TRKM_START;
    idx= 0;

    while ((U32)pBuf < (rSeq.TRKM_START + TRKM_SIZE - 1))
    {
        if ((idx % 10) == 0) M_PRINTF("\n[%08X]:", pBuf);
        M_PRINTF("%08X ", *pBuf);
        pBuf++;
        idx++;
    }
    M_PRINTF("\n");

    for (tid = 0; tid < 128; tid++)
    {
        M_PRINTF("TrkID[%d]\n", tid);
        rSeq.trkStatId.b.TRK_ID = tid;

        M_PRINTF("DD:", tid);
        for (idx = 0; idx < 8; idx++)
        {
            if ((idx % 8) == 0) M_PRINTF("\n[%d]:");
            dd = rSeq.TRK_DD;
            M_PRINTF("%08X ", dd);
        }
        M_PRINTF("\n");

        M_PRINTF("DS:", tid);

        for (idx = 0; idx < 16; idx++)
        {
            if ((idx % 8) == 0) M_PRINTF("\n[%d]:");
            ds = rSeq.TRK_DS;
            M_PRINTF("%08X ", ds);
        }

        M_PRINTF("\n");
    }
    #if 0

    /*
    M_PRINTF("\n[ECU AXI]");
    pTrkMEM = (uint32_t *)0xF200C072;
    idx= 0;
    while ((U32)pTrkMEM < (0xF200C090))
    {
        if ((idx % 10) == 0) D_PRINTF("\n[%08X]:", pTrkMEM);
        D_PRINTF("%08X ", *pTrkMEM);
        pTrkMEM++;
        idx++;
    }
    M_PRINTF("\n");
    */

    M_PRINTF("\n[DMAC]");
    pBuf = (uint32_t *)0xF010A000;
    idx= 0;
    while ((U32)pBuf < (0xF010A120))
    {
        if ((idx % 10) == 0) M_PRINTF("\n[%08X]:", pBuf);
        M_PRINTF("%08X ", *pBuf);
        pBuf++;
        idx++;
    }
    M_PRINTF("\n");

    M_PRINTF("\n[ECU]");
    pBuf = (uint32_t *)0xF200C000;
    idx= 0;
    while ((U32)pBuf < (0xF200C230))
    {
        if ((idx % 10) == 0) M_PRINTF("\n[%08X]:", pBuf);
        M_PRINTF("%08X ", *pBuf);
        pBuf++;
        idx++;
    }
    M_PRINTF("\n");

    M_PRINTF("\n[ECU DMA]");
    pBuf = (uint32_t *)0xF200C540;
    idx= 0;
    while ((U32)pBuf < (0xF200C580))
    {
        if ((idx % 10) == 0) M_PRINTF("\n[%08X]:", pBuf);
        M_PRINTF("%08X ", *pBuf);
        pBuf++;
        idx++;
    }
    M_PRINTF("\n");

    M_PRINTF("\n[ECU CORE]");
    pBuf = (uint32_t *)0xF200C970;
    idx= 0;
    while ((U32)pBuf < (0xF200C980))
    {
        if ((idx % 10) == 0) M_PRINTF("\n[%08X]:", pBuf);
        M_PRINTF("%08X ", *pBuf);
        pBuf++;
        idx++;
    }
    M_PRINTF("\n");

    M_PRINTF("\n[SEQ] 0xF200D000");
    pBuf = (uint32_t *)0xF200D000;
    idx= 0;
    while ((U32)pBuf < (0xF200D4E0))
    {
        if ((idx % 10) == 0) M_PRINTF("\n[%08X]:", pBuf);
        M_PRINTF("%08X ", *pBuf);
        pBuf++;
        idx++;
    }
    M_PRINTF("\n");

    M_PRINTF("\n[SEQ] 0xF200E000");
    pBuf = (uint32_t *)0xF200E000;
    idx= 0;
    while ((U32)pBuf < (0xF200E3D8))
    {
        if ((idx % 10) == 0) M_PRINTF("\n[%08X]:", pBuf);
        M_PRINTF("%08X ", *pBuf);
        pBuf++;
        idx++;
    }
    M_PRINTF("\n");


    {
        U16 val;
        U32 job;
        U32 ch;

        // Job Processor Status
        /*** PART I ***/
        M_PRINTF("P1: E0F0[15]=0, E0F0[3:0] 0~F, Read E0F2[15:0]\n");
        val = READ16(0xF200E0F0);
        val &= 0xFFFF7FFF;
        WRITE16(0xF200E0F0, val);

        for (idx = 0; idx < 16; idx++)
        {
            val = READ16(0xF200E0F0);
            val &= 0xFFFFEFF0;
            val |= idx;
            WRITE16(0xF200E0F0, val);
            M_PRINTF("%04X\n", READ16(0xF200E0F2));
        }
        M_PRINTF("\n");

        /*** PART II ***/
        M_PRINTF("P2_1: E0F0[15]=1, E0E0[7]=0\n");
        // E0F0[15]=1h, E0E0[7]=0, sweep E0F0[14:11] from 0~7 E0F0[10:7] 0~F, Read E0F2
        val = READ16(0xF200E0F0);
        val |= 0x8000;
        WRITE16(0xF200E0F0, val);

        val = READ16(0xF200E0E0);
        val &= 0xFFFFFFEF;
        WRITE16(0xF200E0E0, val);

        for (ch = 0; ch < 8; ch++)
        {
            M_PRINTF("\nCh[%d]:", ch);
            val = READ16(0xF200E0F0);
            val &= 0xFFFF87FF;
            val |= (ch << 11);
            WRITE16(0xF200E0F0, val);

            for (idx = 0; idx < 16; idx++)
            {
                val = READ16(0xF200E0F0);
                val &= 0xFFFFFC7F;
                val |= (idx << 7);
                WRITE16(0xF200E0F0, val);

                M_PRINTF("%04X ", READ16(0xF200E0F2));
            }
        }
        M_PRINTF("\n");

        M_PRINTF("P2_2: E0F0[15]=1, E0E0[7]=1, E0F0[14:11] 0~7\n");
        // E0F0[15]=1h, E0E0[7]=1, sweep E0F0[14:11] from 0~7 E0F0[10:7] 0~F, Read E0F2
        val = READ16(0xF200E0F0);
        val |= 0x8000;
        WRITE16(0xF200E0F0, val);

        val = READ16(0xF200E0E0);
        val |= 0x80;
        WRITE16(0xF200E0E0, val);

        for (ch = 0; ch < 8; ch++)
        {
            M_PRINTF("\nCh[%d]:", ch);
            val = READ16(0xF200E0F0);
            val &= 0xFFFF87FF;
            val |= (ch << 11);
            WRITE16(0xF200E0F0, val);

            for (idx = 0; idx < 16; idx++)
            {
                val = READ16(0xF200E0F0);
                val &= 0xFFFFFC7F;
                val |= (idx << 7);
                WRITE16(0xF200E0F0, val);

                M_PRINTF("%04X ", READ16(0xF200E0F2));
            }
        }
        M_PRINTF("\n");

        /*** PART III ***/
        M_PRINTF("P3: E0F0[15]=1, E0F0[3:0]=0, E040[11:8] 0~7, Eo40[15:12] 0~15\n");

        //RE0F0
        val = READ16(0xF200E0F0);
        val &= 0xFFFF7FF0;
        WRITE16(0xF200E0F0, val);

        for (ch = 0; ch < 8; ch++)
        {
            M_PRINTF("\nCh[%d]:", ch);
            val = READ16(0xF200E040);
            val &= 0xFFFFF0FF;
            val |= (ch << 8);
            WRITE16(0xF200E040, val);

            for (job = 0; job < 16; job++)
            {
                val = READ16(0xF200E040);
                val &= 0xFFFF0FFF;
                val |= (job << 12);
                WRITE16(0xF200E040, val);

                M_PRINTF(" %04X", READ16(0xF200E0F2));
            }
        }
        M_PRINTF("\n");

        //RE0F0
        val = READ16(0xF200E0F0);
        val &= 0xFFFFFF00;
        WRITE16(0xF200E0F0, val);

        for (ch = 0; ch < 8; ch++)
        {
            M_PRINTF("\nCh[%d]:", ch);
            val = READ16(0xF200E040);
            val &= 0xFFFFF0FF;
            val |= (ch << 8);
            WRITE16(0xF200E040, val);

            for (job = 0; job < 16; job++)
            {
                val = READ16(0xF200E040);
                val &= 0xFFFF0FFF;
                val |= (job << 12);
                WRITE16(0xF200E040, val);

                M_PRINTF(" %04X", READ16(0xF200E04E));
            }
        }
        M_PRINTF("\n");

        M_PRINTF("\nF200D00~F200D80:");
        for (idx = 0; idx < 128; idx++)
        {
            val = ((*(volatile U16*)(0xF200D008)));
            val &= 0xFFFFFF00;
            val |= (idx);
            ((*(volatile U16*)(0xF200D008))) = val;

            if ((idx % 8) == 0) M_PRINTF("\n[%04X]:", idx);

            M_PRINTF("%04X ",  ((*(volatile U16*)(0xF200D00A))));
        }
        M_PRINTF("\n");
    }
    #endif
}

#include "PerfSection.h"
//=============================================================================
//=============================================================================
//=============================================================================
//-----------------------------------------------------------------------------
/**
    FIFO allocation

    @param[io]  pFifo               sequencer FIFO structure pointer
    @param[in]  fifoSize            data pointer
    @param[in]  align               align size
**/
//-----------------------------------------------------------------------------
static void SEQ_FifoAlloc(SeqFifo_t* pFifo, uint32_t fifoSize, uint32_t align)
{
    pFifo->pStart = (uint8_t *) ((fifoSize!=0) ? MEM_AllocSspm(fifoSize, align) : NULL);
    pFifo->pEnd = pFifo->pStart + fifoSize;

    ASSERT((fifoSize%align)==0); // should be aligned
}

//-----------------------------------------------------------------------------
/**
    SetUp FIFO registers

    @param[io]  pFifo               sequencer FIFO structure pointer
    @param[in]  fifoSize            data pointer
    @param[in]  align               align size
    @param[in]  pHwFifo             Hardware FIFO pointer
**/
//-----------------------------------------------------------------------------
static void SEQ_SetFifoRegisters(SeqFifo_t* pFifo, uint32_t fifoSize, uint32_t align, SeqHwFifo_t* pHwFifo)
{
    // Set FIFO current pointer
    pFifo->pCurrent = pFifo->pStart;

    // setup hardware register
    pHwFifo->START = (uint32_t)pFifo->pStart;
    pHwFifo->END   = (fifoSize!=0) ? (uint32_t)pFifo->pEnd - align : NULL;
    pHwFifo->PTR   = (uint32_t)pFifo->pCurrent;
}

//-----------------------------------------------------------------------------
/**
    Push data to sequencer FIFO.\n

    @param[io]  pFifo               sequencer FIFO structure pointer
    @param[in]  pData               data pointer
    @param[in]  size                byte size to push

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t SEQ_PushFifo(SeqFifo_t* pFifo, void* pData, uint32_t size)
{
    uint32_t size1; // copy size
    uint32_t size2; // copy size for pointer wrap around

    size1 = (uint32_t)pFifo->pEnd - (uint32_t)pFifo->pCurrent;

    if (size1!=0)
    {
        size1 = MIN(size1, size);
        MEM_COPY(pFifo->pCurrent, pData, size1);
        pFifo->pCurrent = pFifo->pCurrent + size1;
    }

    if (size1!=size) // check for pointer wrap around
    {
        size2 = size-size1;
        MEM_COPY(pFifo->pStart, (uint8_t *)pData+size1, size2);
        pFifo->pCurrent = pFifo->pStart + size2;
    }

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Pop data from sequencer FIFO.\n

    @param[in]  pFifo               sequencer FIFO structure pointer
    @param[out] pData               data pointer
    @param[in]  size                byte size to pop

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t SEQ_PopFifo(SeqFifo_t* pFifo, void* pData, uint32_t size)
{
    uint32_t size1; // copy size
    uint32_t size2; // copy size for pointer wrap around

    size1 = (uint32_t)pFifo->pEnd - (uint32_t)pFifo->pCurrent;

    if (size1!=0)
    {
        size1 = MIN(size1, size);
        MEM_COPY(pData, pFifo->pCurrent, size1);
        pFifo->pCurrent = pFifo->pCurrent + size1;
    }

    if (size1!=size)
    {
        size2 = size-size1;
        MEM_COPY((uint8_t *)pData+size1, pFifo->pStart, size2);
        pFifo->pCurrent = pFifo->pStart + size2;
    }

    BEGIN_MEDIA_DBG_DUMP(DBG_DUMP_MEDIA_SEQ)
        Debug_DumpMemU32((uint32_t)pData, size);
    END_MEDIA_DBG_DUMP

    return (cEcNoError);
}

#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    Set up SEQ FIFOs.\n
**/
//-----------------------------------------------------------------------------
static void SEQ_SetupSeqFifo(InitBootMode_t initMode)
{
    if (initMode < cInitBootPowerDown)
    {
        SEQ_FifoAlloc(&gSeqFifoInfo.descriptor, DF_FIFO_SIZE, DF_FIFO_ALIGN);
        SEQ_FifoAlloc(&gSeqFifoInfo.auxInsert, AIF_FIFO_SIZE, AIF_FIFO_ALIGN);
        //SEQ_FifoAlloc(&gSeqFifoInfo.fifo.auxCompare, ACF_FIFO_SIZE, ACF_FIFO_ALIGN);
        SEQ_FifoAlloc(&gSeqFifoInfo.auxExtract, AEF_FIFO_SIZE, AEF_FIFO_ALIGN);
        //SEQ_FifoAlloc(&gSeqFifoInfo.fifo.hlbaInsert, HIF_FIFO_SIZE, HIF_FIFO_ALIGN);
        //SEQ_FifoAlloc(&gSeqFifoInfo.fifo.hlbaCompare, HCF_FIFO_SIZE, HCF_FIFO_ALIGN);
        SEQ_FifoAlloc(&gSeqFifoInfo.errorCount, ECF_FIFO_SIZE, ECF_FIFO_ALIGN);
        //SEQ_FifoAlloc(&gSeqFifoInfo.nandParameter, GETP_FIFO_SIZE, GETP_FIFO_ALIGN);
        SEQ_FifoAlloc(&gSeqFifoInfo.nandBusyTimeMonitor, BTMF_FIFO_SIZE, BTMF_FIFO_ALIGN);
        SEQ_FifoAlloc(&gSeqFifoInfo.slowRetry, SRF_FIFO_SIZE, SRF_FIFO_ALIGN);
        //SEQ_FifoAlloc(&gSeqFifoInfo.fifo.fastDescriptor, FDF_FIFO_SIZE, FDF_FIFO_ALIGN);
        //SEQ_FifoAlloc(&gSeqFifoInfo.fifo.fastAuxInsert, FAIF_FIFO_SIZE, FAIF_FIFO_ALIGN);
        //SEQ_FifoAlloc(&gSeqFifoInfo.fifo.fastAuxCompare, FACF_FIFO_SIZE, FACF_FIFO_ALIGN);
        //SEQ_FifoAlloc(&gSeqFifoInfo.fifo.fastHlbaCompare, FHCF_FIFO_SIZE, FHCF_FIFO_ALIGN);
        //SEQ_FifoAlloc(&gSeqFifoInfo.fifo.fastSlowRetry, FSRF_FIFO_SIZE,  FSRF_FIFO_ALIGN);
        //SEQ_FifoAlloc(&gSeqFifoInfo.nandBusyTimeMonitor, BTMF_FIFO_SIZE, BTMF_FIFO_ALIGN);


        gSeqAllcInfo.aencStart       = (uint32_t)MEM_AllocSspm(AENC_FIFO_SIZE, AENC_FIFO_ALIGN);
        gSeqAllcInfo.adecStart       = (uint32_t)MEM_AllocSspm(ADEC_FIFO_SIZE, ADEC_FIFO_ALIGN);
        gSeqAllcInfo.seqLutSaddr     = (uint32_t)MEM_AllocSspm(LUT_SIZE, LUT_ALIGN);
        gSeqAllcInfo.seqAencLutSaddr = (uint32_t)MEM_AllocSspm(LUT_AU_ENC_SIZE, LUT_AU_ENC_ALIGN);
        gSeqAllcInfo.seqAdecLutSaddr = (uint32_t)MEM_AllocSspm(LUT_AU_DEC_SIZE, LUT_AU_DEC_ALIGN);
        gSeqAllcInfo.mqp0Start       = (MQP0_FIFO_SIZE > 0) ? (uint32_t)MEM_AllocSspm(MQP0_FIFO_SIZE, MQP0_FIFO_ALIGN) : 0;
        gSeqAllcInfo.mqp1Start       = (MQP1_FIFO_SIZE > 0) ? (uint32_t)MEM_AllocSspm(MQP1_FIFO_SIZE, MQP1_FIFO_ALIGN) : 0;
        gSeqAllcInfo.mqp2Start       = (MQP2_FIFO_SIZE > 0) ? (uint32_t)MEM_AllocSspm(MQP2_FIFO_SIZE, MQP2_FIFO_ALIGN) : 0;
        gSeqAllcInfo.mqp3Start       = (MQP3_FIFO_SIZE > 0) ? (uint32_t)MEM_AllocSspm(MQP3_FIFO_SIZE, MQP3_FIFO_ALIGN) : 0;
        gSeqAllcInfo.trkmStart       = (uint32_t)MEM_AllocSspm(TRKM_SIZE, TRKM_ALIGN);
        gSeqAllcInfo.hrpjStart       = (uint32_t)MEM_AllocSspm(HP_ADEC_FIFO_SIZE, HP_ADEC_FIFO_ALIGN);
    }
    else
    {
        MEM_SET((void*)gSeqFifoInfo.descriptor.pStart, 0x00, DF_FIFO_SIZE);
        MEM_SET((void*)gSeqFifoInfo.auxInsert.pStart, 0x00, AIF_FIFO_SIZE);
        MEM_SET((void*)gSeqFifoInfo.auxExtract.pStart, 0x00, AEF_FIFO_SIZE);
        MEM_SET((void*)gSeqFifoInfo.errorCount.pStart, 0x00, ECF_FIFO_SIZE);
        MEM_SET((void*)gSeqFifoInfo.nandBusyTimeMonitor.pStart, 0x00, BTMF_FIFO_SIZE);
        MEM_SET((void*)gSeqFifoInfo.slowRetry.pStart, 0x00, SRF_FIFO_SIZE);
        MEM_SET((void*)gSeqAllcInfo.aencStart, 0x00, AENC_FIFO_SIZE);
        MEM_SET((void*)gSeqAllcInfo.adecStart, 0x00, ADEC_FIFO_SIZE);
        MEM_SET((void*)gSeqAllcInfo.seqLutSaddr, 0x00, LUT_SIZE);
        MEM_SET((void*)gSeqAllcInfo.seqAencLutSaddr, 0x00, LUT_AU_ENC_SIZE);
        MEM_SET((void*)gSeqAllcInfo.seqAdecLutSaddr, 0x00, LUT_AU_DEC_SIZE);
        MEM_SET((void*)gSeqAllcInfo.trkmStart, 0x00, TRKM_SIZE);
        MEM_SET((void*)gSeqAllcInfo.hrpjStart, 0x00, HP_ADEC_FIFO_SIZE);

    }

    SEQ_SetFifoRegisters(&gSeqFifoInfo.descriptor, DF_FIFO_SIZE, DF_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.DF_START);
    SEQ_SetFifoRegisters(&gSeqFifoInfo.auxInsert, AIF_FIFO_SIZE, AIF_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.AIF_START);
    //SEQ_SetFifoRegisters(&gSeqFifoInfo.auxCompare, ACF_FIFO_SIZE, ACF_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.ACF_START);
    SEQ_SetFifoRegisters(&gSeqFifoInfo.auxExtract, AEF_FIFO_SIZE, AEF_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.AEF_START);
    //SEQ_SetFifoRegisters(&gSeqFifoInfo.hlbaInsert, HIF_FIFO_SIZE, HIF_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.hif_start);
    //SEQ_SetFifoRegisters(&gSeqFifoInfo.hlbaCompare, HCF_FIFO_SIZE, HCF_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.HCF_START);
    SEQ_SetFifoRegisters(&gSeqFifoInfo.errorCount, ECF_FIFO_SIZE, ECF_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.ECF_START);
    //SEQ_SetFifoRegisters(&gSeqFifoInfo.nandParameter, GETP_FIFO_SIZE, GETP_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.GETP_START);
    SEQ_SetFifoRegisters(&gSeqFifoInfo.nandBusyTimeMonitor, BTMF_FIFO_SIZE, BTMF_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.BTMF_START);
    SEQ_SetFifoRegisters(&gSeqFifoInfo.slowRetry, SRF_FIFO_SIZE, SRF_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.SRF_START);
    //SEQ_SetFifoRegisters(&gSeqFifoInfo.fastDescriptor, FDF_FIFO_SIZE, FDF_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.FDF_START);
    //SEQ_SetFifoRegisters(&gSeqFifoInfo.fastAuxInsert, FAIF_FIFO_SIZE, FAIF_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.FAIF_START);
    //SEQ_SetFifoRegisters(&gSeqFifoInfo.fastAuxCompare, FACF_FIFO_SIZE, FACF_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.FACF_START);
    //SEQ_SetFifoRegisters(&gSeqFifoInfo.fastHlbaCompare, FHCF_FIFO_SIZE, FHCF_FIFO_ALIGN, (SeqHwFifo_t *)&rSeq.FHCF_START);
    //SEQ_SetFifoRegisters(&gSeqFifoInfo.fastSlowRetry, FSRF_FIFO_SIZE,  FSRF_FIFO_ALIGN,(SeqHwFifo_t *)&rSeq.FSRF_START);

    rSeq.AENC_START         = gSeqAllcInfo.aencStart;
    rSeq.AENC_SIZE          = AENC_FIFO_SIZE / AENC_FIFO_ENTRY_SIZE;
    rSeq.ADEC_START         = gSeqAllcInfo.adecStart;
    rSeq.ADEC_SIZE          = ADEC_FIFO_SIZE / ADEC_FIFO_ENTRY_SIZE;

    rSeq.SEQ_LUT_SADDR      = gSeqAllcInfo.seqLutSaddr;
    rSeq.SEQ_AENC_LUT_SADDR = gSeqAllcInfo.seqAencLutSaddr;
    rSeq.SEQ_ADEC_LUT_SADDR = gSeqAllcInfo.seqAdecLutSaddr;

    rSeq.MQP0_START         = gSeqAllcInfo.mqp0Start;
    rSeq.MQP0_SIZE          = MQP0_FIFO_SIZE / MQP0_FIFO_ENTRY_SIZE;
    rSeq.MQP1_START         = gSeqAllcInfo.mqp1Start;
    rSeq.MQP1_SIZE          = MQP1_FIFO_SIZE / MQP1_FIFO_ENTRY_SIZE;
    rSeq.MQP2_START         = gSeqAllcInfo.mqp2Start;
    rSeq.MQP2_SIZE          = MQP2_FIFO_SIZE / MQP2_FIFO_ENTRY_SIZE;
    rSeq.MQP3_START         = gSeqAllcInfo.mqp3Start;
    rSeq.MQP3_SIZE          = MQP3_FIFO_SIZE / MQP3_FIFO_ENTRY_SIZE;

    rSeq.TRKM_START         = gSeqAllcInfo.trkmStart;

    rSeq.ADEC_HPRJ_START    = gSeqAllcInfo.hrpjStart;
    rSeq.ADEC_HPRJ_SIZE     = HP_ADEC_FIFO_SIZE / HP_ADEC_FIFO_ENTRY_SIZE;
}

#include "PerfSection.h"
//-----------------------------------------------------------------------------
/**
    Initialization SEQ misc. memories.\n
**/
//-----------------------------------------------------------------------------
static void SEQ_InitMiscMemories(void)
{
    uint16_t nCount;

    // TRACK Cancellation memory
    rSeq.trkCancellaCtrl2.all = 0;                              ///< Clear all bits to zero.
    for(nCount = 0; nCount < SEQ_MAX_TRACK_CANCEL_NUM; nCount++)///< For sixteen tracks
    {
        rSeq.trkCancellaCtrl2.b.TRK_CCL_ID = (nCount << 4);
        rSeq.trkCancellaCtrl2.b.TRK_CCL_CLEAR = 1;              ///< When set to 1, the track cancel list is cleared
    }

    // TRACK LOS memory
    for(nCount = 0; nCount < SEQ_MAX_LOS_NUM; nCount++)         ///< For sixteen tracks
    {
        rSeq.trkCancellaCtrl3.b.TRK_LOS_CID = nCount;
    }
}

//-----------------------------------------------------------------------------
/**
   Init PRP count
**/
//-----------------------------------------------------------------------------
static void SEQ_InitPrpAvCount(uint16_t initVal)
{
    rSeq.prpAvailableCnt.b.PRP_AV_CNT = 0;
    rSeq.prpAvailableCntAdj.b.PRP_AV_CNT_ADJ = initVal;
}

//-----------------------------------------------------------------------------
/**
    Load TRK Overall Status1.\n

    @param[in]  trkId               TRK ID
**/
//-----------------------------------------------------------------------------
static __inline void SEQ_LoadTrkOverallStatus1(uint32_t trkId)
{
    uint32_t status;

    status = GET_SEQ_TRK_OVERALLSTATUS1(trkId);

    gTrkErrorInfo.trkPos    = status & 0x01;
    gTrkErrorInfo.trkLos    = (status >> 1) & 0x01;
    gTrkErrorInfo.descProc  = (status >> 2) & 0x7FF;
    gTrkErrorInfo.descTot   = (status >> 18) & 0x7FF;
}

//-----------------------------------------------------------------------------
/**
    Load TRK Overall Status2 & ECU Status.\n

    @param[in]  trkId               TRK ID
**/
//-----------------------------------------------------------------------------
static __inline void SEQ_LoadTrkOverallStatus2EcuStatus(uint32_t trkId)
{
    uint32_t status;

    status = GET_SEQ_TRK_OVERALLSTATUS2(trkId);

    gTrkErrorInfo.errAuNum  = status & 0x07;
    gTrkErrorInfo.errMpNum  = (status >> 3) & 0x07;
    gTrkErrorInfo.errAuDid  = (status >> 16) & 0x3FF;
    gTrkErrorInfo.errAuType = (status >> 31) & 0x01;

    gTrkErrorInfo.errStatus = (gTrkErrorInfo.errAuType == 0) ? HalEcu_GetDecStatus() : HalEcu_GetEncStatus();
}

//-----------------------------------------------------------------------------
/**
    Load track descriptor status.\n

    @param[in]  trkId               track Id
**/
//-----------------------------------------------------------------------------
static __inline void SEQ_LoadDescriptorStatus(uint32_t trkId)
{
    U32 dsPtr;
    U16 count;

    rSeq.trkStatId.b.TRK_ID = trkId;
    count = (gTrkErrorInfo.descTot + (SEQ_DESC_PER_DS - 1)) >> SEQ_DESC_PER_DS_SHIFT;

    for (dsPtr = 0; dsPtr < count; dsPtr++)
    {
        gTrkErrorInfo.trkDescStatus[dsPtr] = rSeq.TRK_DS;

        if (gNfInfo.oriPlanesPerLun != gNfInfo.planesPerLun)
        {
            #if 0
            for (idx = 1; idx < (gNfInfo.oriPlanesPerLun / gNfInfo.planesPerLun); idx++)
            {
                gTrkErrorInfo.trkDescStatus[dsPtr] |= ((gTrkErrorInfo.trkDescStatus[dsPtr] >> (1 + idx)) & gErrPlaneBitMask[gNfInfo.planeAddressBits]);
            }
            #endif

            gTrkErrorInfo.trkDescStatus[dsPtr] &= gDsPlaneBitMask[gNfInfo.planeAddressBits];
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Media done interrupt service rotuine(error case).\n

    @param[in]  trkId               track Id
**/
//-----------------------------------------------------------------------------
static void SEQ_MediaErrorIsr(uint32_t trkId)
{
    //
    // CASE1   : DESC_TOT > DESC_PROC -> LOS = 1 (TRK_DONE will come soon)
    // CASE2   : DESC_TOT = DESC_PROC -> POS = 1
    // CASE3   : DESC_TOT = DESC_PROC -> LOS = 1
    //
    if ((gTrkErrorInfo.descTot > gTrkErrorInfo.descProc) || (gTrkErrorInfo.descTot == 0))
    {
        // CASE1
        ASSERT(gTrkErrorInfo.trkLos == 1);

        SEQ_LoadTrkOverallStatus2EcuStatus(trkId);
    }
    else
    {
        ASSERT(gTrkErrorInfo.descProc > 0);

        if (gTrkErrorInfo.trkPos == 0)
        {
            // CASE3
            D_PRINTF("[SEQ] DESC_TOT(%d)==DESC_PROC(%d) -> TRK_POS=0 (TRK#%03X)\n", gTrkErrorInfo.descTot, gTrkErrorInfo.descProc, trkId);
        }

        SEQ_LoadDescriptorStatus(trkId);
    }

    gSeqInfo.seqAppCallback[SEQ_CB_TRK_ERROR](trkId, (void*)&gTrkErrorInfo);
}

//-----------------------------------------------------------------------------
/**
    Media done interrupt service rotuine.\n

    @param[in]  param               parameter(not used)
**/
//-----------------------------------------------------------------------------
static void SEQ_MediaDoneIsr(uint32_t param)
{
    uint32_t trkId;
    uint16_t interruptStatus;

    interruptStatus = rSeq.seqIntrStat.all;

    trkId = rSeq.trkStatId.b.TRK_ID;

    if (interruptStatus & SEQ_INT_TRACK_DONE)
    {
        gSeqInfo.seqAppCallback[SEQ_CB_TRK_DONE](trkId, NULL);
    }
    else if (interruptStatus & SEQ_INT_TRACK_ERROR)
    {
        MEM_CLR(&gTrkErrorInfo, sizeof(TrkErrInfo_t));
        SEQ_LoadTrkOverallStatus1(trkId);
        SEQ_MediaErrorIsr(trkId);

        if (gTrkErrorInfo.trkPos)
        {
            /// redirect to trkdone flow for erase/program error
            gSeqInfo.seqAppCallback[SEQ_CB_TRK_DONE](trkId, NULL);
        }
        else if (gTrkErrorInfo.trkLos)
        {
            ///< MUST clear interrupt first, then ECU resume!
            rSeq.seqIntrStat.all = interruptStatus;
            HalEcu_Resume();
            return;
        }
    }
    else
    {
        #if (FIX_SAC_RW_ERR_INT_CLEAR_ISSUE == TRUE)
        if (interruptStatus & SEQ_INT_SAC_W_ERR) interruptStatus |= SEQ_INT_MQP0_FULL;
        if (interruptStatus & SEQ_INT_SAC_R_ERR) interruptStatus |= SEQ_INT_MQP1_FULL;
        #endif

        if (interruptStatus & SEQ_INT_SAC_W_ERR)  E_PRINTF(cEcSeqFatalError, "[SEQ] AXI Write Error\n");
        if (interruptStatus & SEQ_INT_SAC_R_ERR)  E_PRINTF(cEcSeqFatalError, "[SEQ} AXI Read Error\n");
        if (interruptStatus & SEQ_INT_TNUM_OVER)  E_PRINTF(cEcSeqFatalError, "[SEQ] Track Maximum Overflow\n");
        if (interruptStatus & SEQ_INT_DNUM_OVER)  E_PRINTF(cEcSeqFatalError, "[SEQ} Descriptor Maximum Overflow\n");
        if (interruptStatus & SEQ_INT_DES_INV)    E_PRINTF(cEcSeqFatalError, "[SEQ] Invalid Descriptor\n");
    }

    ///< Clear all asserted interrupt
    rSeq.seqIntrStat.all = interruptStatus;
}

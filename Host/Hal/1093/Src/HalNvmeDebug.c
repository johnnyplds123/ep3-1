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
//! @brief NVMe controller handling code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"
#include "RegNvmePkg.h"

// REG
#include "RegPcie.h"

// HAL
#include "HalNvmeXfer.h"

// APL

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

#define DUMP_MEM_U32x8(STR, ADDR, SIZE)                 \
    {                                                   \
        M_PRINTF("%s", (STR));                          \
        Debug_DumpMemU32PerLine((ADDR), (SIZE), 32);    \
    }

#define DUMP_MEM_U32(STR, ADDR, SIZE)                   \
    {                                                   \
        M_PRINTF("%s", (STR));                          \
        Debug_DumpMemU32((ADDR), (SIZE));               \
    }

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
extern uint32_t Hal_GetRevision(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Dump HW IOCMD SRAM.
**/
//-----------------------------------------------------------------------------
void DumpHwIoCmd(void)
{
    int i;
    int j;
    uint32_t* pIoCmd;

    M_PRINTF("\n[HW IO CMD SRAM]\n");
    for( i = 0; i < HW_MAX_IO_CMD_INDEX; i++)
    {
        M_PRINTF("%02X: ", i);
        Debug_DumpMemU32PerLine((uint32_t)&rNvmeHwIoCmd[i], sizeof(NvmeHwIoCmd_t)/2, 32);
        M_PRINTF("    ");
        Debug_DumpMemU32PerLine((uint32_t)&rNvmeHwIoCmd[i] + sizeof(NvmeHwIoCmd_t)/2, sizeof(NvmeHwIoCmd_t)/2, 32);
    }
    M_PRINTF("\n");
}

#if (NVME_REGISTER_DUMP == TRUE)
//-----------------------------------------------------------------------------
/**
    Dump Packet Manager Debug Info.
**/
//-----------------------------------------------------------------------------
static void DumpNvmeGlobalInfo(void)
{
    M_PRINTF("\n[GLOBAL INFO]\n");
    M_PRINTF("CENT_INTP 0x%08X, STSM_MCQ_POSTCQ_DONE_BITMAP 0x%08X\n",
        rNvmeCentralCtrl.centralCtrlModuleIntr.all, rNvmeStatMngr.MAN_CQ_REQ_DONE_BM);
}

//-----------------------------------------------------------------------------
/**
    Dump Packet Manager Debug Info.
**/
//-----------------------------------------------------------------------------
static void DumpPacketManagerInfo(void)
{
    uint32_t i;
    uint32_t numberOfErrors;

    M_PRINTF("\n[PKTM Regs]\n");
    Debug_DumpMemU32((uint32_t) &rNvmePktMngr, sizeof(rNvmePktMngr));   // Dump 0xF5013100 - ...

    M_PRINTF("\n[PKTM REQ SRAM]\n");
    Debug_DumpMemU32(0xF541F000, 0x400);    // Dump 1KB

    numberOfErrors = rNvmePktMngr.packetManagerErrFifoCnt.b.PKTM_ERR_CNTR;
    M_PRINTF("PKTM errof fifo count %08x\n", numberOfErrors);
    for (i = 0; i < numberOfErrors; i ++)
    {
        M_PRINTF("Error message %d: DW1 0x%08X DW0 0x%08X\n", i, rNvmePktMngr.PKTM_ERR_FIFO_DW[1], rNvmePktMngr.PKTM_ERR_FIFO_DW[0]);
    }
}

//-----------------------------------------------------------------------------
/**
    Dump Control Register Debug Info.
**/
//-----------------------------------------------------------------------------
static void DumpControlRegisterInfo(void)
{
    M_PRINTF("\n[CREG Regs]\n");
    Debug_DumpMemU32((uint32_t) &rNvme, sizeof(rNvme));

    M_PRINTF("CREG_ENG_STATUS %08X CREG_QAL_FUN0 %08X CREG_QAL_FUN1 %08X CREG_QAL_FUN2 %08X\n",
            rNvme.ctrlRegisterModuleEngineStat.all, rNvme.ctrlRegisterModuleQueueAllocatiFunc[0].all,
            rNvme.ctrlRegisterModuleQueueAllocatiFunc[1].all, rNvme.ctrlRegisterModuleQueueAllocatiFunc[2].all);
}

//-----------------------------------------------------------------------------
/**
    Dump Command Fetcher Debug Info.
**/
//-----------------------------------------------------------------------------
static void DumpCommandFetcherInfo(void)
{
    uint32_t i;
    uint32_t numberOfErrors;

    M_PRINTF("\n[CMDF Regs]\n");
    Debug_DumpMemU32((uint32_t) &rNvmeCmdFetch, sizeof(rNvmeCmdFetch));

    M_PRINTF("CMDF_CFG 0x%08X CMDF_ENG_STATUS 0x%08X CMDF_AXI_BUF_CNT 0x%08X\n",
           rNvmeCmdFetch.cmdFetcherCfg.all, rNvmeCmdFetch.cmdFetcherEngineStat.all, rNvmeCmdFetch.cmdFetcherAxiBufCntr.all);

    numberOfErrors = rNvmeCmdFetch.cmdFetcherErrFifoCnt.b.CMDF_ERR_CNTR;
    M_PRINTF("CMDF error fifo count %08X\n", numberOfErrors);
    for (i = 0; i < numberOfErrors; i ++)
    {
        M_PRINTF("Error message %d: DW1 0x%08x DW0 0x%08x\n",
               i, rNvmeCmdFetch.CMDF_ERR_FIFO_DW[1], rNvmeCmdFetch.CMDF_ERR_FIFO_DW[0]);
    }
}

//-----------------------------------------------------------------------------
/**
    Dump Command SRAM Debug Info.
**/
//-----------------------------------------------------------------------------
static void DumpCommandSramInfo(void)
{
    M_PRINTF("\n[CSRM Regs]\n");
    Debug_DumpMemU32((uint32_t) &rNvmeCmdSram, sizeof(rNvmeCmdSram));

    M_PRINTF("ENG_STATUS %08X FAD_CNT %08X FAD_PTR %08X FIO_CNT %08X FIO_PTR %08X\n",
        rNvmeCmdSram.cmdSramEngineStat.all,
        rNvmeCmdSram.cmdSramFwAdminCmdCnt.all, rNvmeCmdSram.cmdSramFwAdminCmdPtr.all,
        rNvmeCmdSram.cmdSramFwIOCmdCnt.all, rNvmeCmdSram.cmdSramFwIOCmdPtr.all);
}

//-----------------------------------------------------------------------------
/**
    Dump Data Manager Debug Info.
**/
//-----------------------------------------------------------------------------
static void DumpDataManagerInfo(void)
{
    int offset;
    int num_errors;
    int i;

    M_PRINTF("\n[DATM Regs]\n");
    Debug_DumpMemU32((uint32_t) &rNvmeDataMngr, sizeof(rNvmeDataMngr));   // Dump 0xF5011000 - ...

    M_PRINTF("Debug Reg0: (%08x)=%08x\n", &(rNvmeDataMngr.DATM_DEBUG0),  rNvmeDataMngr.DATM_DEBUG0);
    M_PRINTF("Debug Reg1: (%08x)=%08x\n", &(rNvmeDataMngr.DATM_DEBUG1),  rNvmeDataMngr.DATM_DEBUG1);

    //Debug_DumpMemU32((uint32_t)&rNvmeDataMngr.dataManagerWrEnginePcieStat, ((0x1C4-0x124+4)));   // Dump 0xF5011124 - 0xF501119C
    // Data Abort happens after 0the 0x1A0 in FPGA.

    num_errors = rNvmeDataMngr.dataManagerErrInformat.b.ERR_CNT;
    M_PRINTF("DATM error fifo count %08x\n", num_errors);
    for (i = 0; i < num_errors; i ++)
    {
        M_PRINTF("Error message %d: DW1 0x%08x DW0 0x%08x\n",
               i, rNvmeDataMngr.DATM_ERR_DWORD[1], rNvmeDataMngr.DATM_ERR_DWORD[0]);
    }

}

//-----------------------------------------------------------------------------
/**
    Dump Queue Arbiter Debug Info.
**/
//-----------------------------------------------------------------------------
static void DumpQueueArbiterInfo(void)
{
    M_PRINTF("\n[QARB Regs]\n");
    Debug_DumpMemU32((uint32_t) &rNvmeQueueArbiter, sizeof(rNvmeQueueArbiter));

    M_PRINTF("ENG_STATUS %08x\n", rNvmeQueueArbiter.queueArbEngineStat.all);
}

//-----------------------------------------------------------------------------
/**
    Dump Status Manager Debug Info.
**/
//-----------------------------------------------------------------------------
static void DumpStatusManagerInfo(void)
{
    M_PRINTF("\n[STSM DEBUG DUMP]\n");
    Debug_DumpMemU32((uint32_t) &rNvmeStatMngr, sizeof(rNvmeStatMngr));

    M_PRINTF("\nSTSM_DBG_PRP_FETCH_SEQID_VALID 0x%x", rNvmeStatMngr.STSM_DBG_PRP_FETCH_SEQID_VALID);
    M_PRINTF("\nSTSM_DBG_PRP_FETCH_CURPRP_FSM 0x%x", rNvmeStatMngr.STSM_DBG_PRP_FETCH_CURPRP_FSM);
    M_PRINTF("\nSTSM_DBG_PRP_FETCH_NEXPRP_FSM 0x%x", rNvmeStatMngr.STSM_DBG_PRP_FETCH_NEXPRP_FSM);
    M_PRINTF("\nSTSM_DBG_PRP_FETCH_GETPRP_FSM 0x%x", rNvmeStatMngr.STSM_DBG_PRP_FETCH_GETPRP_FSM);
    M_PRINTF("\nSTSM_DBG_PKT_CTRL_MAIN_FSM 0x%x", rNvmeStatMngr.STSM_DBG_PKT_CTRL_MAIN_FSM);
    M_PRINTF("\nSTSM_DBG_PKT_CTRL_AUTO_CQ_COUNT 0x%x", rNvmeStatMngr.STSM_DBG_PKT_CTRL_AUTO_CQ_COUNT);
    M_PRINTF("\nSTSM_DBG_PKT_CTRL_MANU_CQ_COUNT 0x%x", rNvmeStatMngr.STSM_DBG_PKT_CTRL_MANU_CQ_COUNT);
    M_PRINTF("\nSTSM_DBG_INT_MAN_CQ_STS_FSM 0x%x", rNvmeStatMngr.STSM_DBG_INT_MAN_CQ_STS_FSM);
    M_PRINTF("\nSTSM_DBG_INT_MAN_MSI_STS_FSM 0x%x", rNvmeStatMngr.STSM_DBG_INT_MAN_MSI_STS_FSM);
    M_PRINTF("\nSTSM_DBG_INT_MAN_TIMER_STS_FSM 0x%x", rNvmeStatMngr.STSM_DBG_INT_MAN_TIMER_STS_FSM);
    M_PRINTF("\nSTSM_INTP_VGRP0_STS %08x", rNvmeStatMngr.statManagerIntrVectorStatGroup[0].all);
    M_PRINTF("\nSTSM_INTP_VGRP0_VLD %08x\n", rNvmeStatMngr.statManagerIntrVectorValidGroup[0].all);
}

//-----------------------------------------------------------------------------
/**
    Dump Monitor Signals to debug each block in detail.
**/
//-----------------------------------------------------------------------------
static void DumpMonitorSignalInfo(void)
{
    int i;
    M_PRINTF("\n[Monitor Singals]\n");
    M_PRINTF("\n- CREG Module\n");
    for (i = 0; i <= rNvme.ctrlRegisterModuleMonitorSel.b.CREG_MON_SEL; i ++)
    {
        rNvme.ctrlRegisterModuleMonitorSel.all = i;
        rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all = 0x00010000;
        M_PRINTF("CREG_MON_SEL %08x CENT_MON_SEL %08x CENT_MON_DATA %08x\n",
               rNvme.ctrlRegisterModuleMonitorSel.all, rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all, rNvmeCentralCtrl.centralCtrlModuleMonitorData.all);
    }

    M_PRINTF("\n- CSRM Module\n");
    for (i = 0; i <= 2; i++)  //CSRM monitor set 0-2
    {
        rNvmeCmdSram.cmdSramMonitorSgnSel.all = i;
        rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all = 0x01010100;
        M_PRINTF("CSRM_MON_SEL %08x CENT_MON_SEL %08x CENT_MON_DATA %08x\n",
               rNvmeCmdSram.cmdSramMonitorSgnSel.all, rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all, rNvmeCentralCtrl.centralCtrlModuleMonitorData.all);
    }
    M_PRINTF("\n- BSRM module\n");
    //for (i = 0; i <= rNvmeBitmapSram.bitmapSramMonitorSgnSel.b.BSRAM_MON_SIG_SEL; i++)
    for (i = 0; i <= 1; i++)
    {
        rNvmeBitmapSram.bitmapSramMonitorSgnSel.all = i;
        rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all = 0x02010200;
        M_PRINTF("BSRM_MON_SEL %08x CENT_MON_SEL %08x CENT_MON_DATA %08x\n",
               rNvmeBitmapSram.bitmapSramMonitorSgnSel.all, rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all, rNvmeCentralCtrl.centralCtrlModuleMonitorData.all);
    }

    M_PRINTF("\n- QARB Module\n");
    for (i = 0; i < 18; i++)  /* Number of QARB monitor signals is creased to 18 */
    {
        rNvmeQueueArbiter.queueArbMonitorSgnSel.all = i;
        rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all = 0x03010300;
        M_PRINTF("QARB_MON_SEL %08x CENT_MON_SEL %08x CENT_MON_DATA %08x\n",
               rNvmeQueueArbiter.queueArbMonitorSgnSel.all, rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all, rNvmeCentralCtrl.centralCtrlModuleMonitorData.all);
    }

    M_PRINTF("\n- CMDF Module\n");
    for (i = 0; i <= 6; i++) //CMDF monitor set 0-6
    {
        rNvmeCmdFetch.cmdFetcherMonitorSel.all = i;
        rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all = 0x04010400;
        M_PRINTF("CMDF_MON_SEL %08x CENT_MON_SEL %08x CENT_MON_DATA %08x\n",
               rNvmeCmdFetch.cmdFetcherMonitorSel.all, rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all, rNvmeCentralCtrl.centralCtrlModuleMonitorData.all);
    }

    //Status Manager Signals not available yet
    //PCIe Packet Manager Module not available yet

    // DATMSTATEMONSEL=7, Print out "PRP slot available count
    rNvmeDataMngr.dataManagerMonitorSgnSel.b.DATM_STATE_MON_SEL = 7;
    M_PRINTF("\n- DATM Module\n");
    for (i = 0; i <= 143; i++)
    {
        rNvmeDataMngr.dataManagerMonitorSgnSel.all = i;
        rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all = 0x07010700;
        M_PRINTF("DATM_MON_SEL %08x CENT_MON_SEL %08x CENT_MON_DATA %08x\n",
               rNvmeDataMngr.dataManagerMonitorSgnSel.all, rNvmeCentralCtrl.centralCtrlModuleMonitorSel.all, rNvmeCentralCtrl.centralCtrlModuleMonitorData.all);
    }

    //HW Linked List Signals not available yet
}

//-----------------------------------------------------------------------------
/**
    Dump NVMe Bitmap SRAM.
**/
//-----------------------------------------------------------------------------
static void DumpBitmapSramInfo(void)
{
    M_PRINTF("\n[Bitmap Info]\n");
    M_PRINTF("\nBADBM GRP: %08X\n", rNvmeBitmapSram.bitmapSramBackendAbortDoneBitmapGroupBitmap.all);

    M_PRINTF("\nPRBM GRP: %08X\n", rNvmeBitmapSram.bitmapSramPrpReqBitmapGroupBitmap);
    DUMP_MEM_U32x8("PRBM_R0  ", (uint32_t)rNvmeBitmap.PRBMR0, (HW_MAX_IO_CMD_INDEX/32) * sizeof(uint32_t));
    DUMP_MEM_U32x8("PRBM_R1  ", (uint32_t)rNvmeBitmap.PRBMR1, (HW_MAX_IO_CMD_INDEX/32) * sizeof(uint32_t));
    DUMP_MEM_U32x8("PRBM_W0  ", (uint32_t)rNvmeBitmap.PRBMW0, (HW_MAX_IO_CMD_INDEX/32) * sizeof(uint32_t));
    DUMP_MEM_U32x8("PRBM_W1  ", (uint32_t)rNvmeBitmap.PRBMW1, (HW_MAX_IO_CMD_INDEX/32) * sizeof(uint32_t));

    M_PRINTF("\nCIABM CNT: %08X\n", rNvmeBitmapSram.bitmapSramCmdIndexAvailablBitmapCntr.all);
    DUMP_MEM_U32x8("CIABM    ", (uint32_t)rNvmeBitmap.CIABM, (HW_MAX_IO_CMD_INDEX/32) * sizeof(uint32_t));

    M_PRINTF("\nCABM GRP: %08X\n", rNvmeBitmapSram.bitmapSramCmdActivatiBitmapGroupBitmap.all);
    DUMP_MEM_U32x8("CABM0    ", (uint32_t)rNvmeBitmap.CABM0, (HW_MAX_IO_CMD_INDEX/32) * sizeof(uint32_t));
    DUMP_MEM_U32x8("CABM1    ", (uint32_t)rNvmeBitmap.CABM1, (HW_MAX_IO_CMD_INDEX/32) * sizeof(uint32_t));
    DUMP_MEM_U32x8("CABM1MSK ", (uint32_t)rNvmeBitmap.CABM1MSK, (HW_MAX_IO_CMD_INDEX/32) * sizeof(uint32_t));

    M_PRINTF("\nDTDBM GRP: %08X\n", rNvmeBitmapSram.bitmapSramDataXfrDoneBitmapGroupBitmap.all);
    DUMP_MEM_U32x8("DTDBM    ", (uint32_t)rNvmeBitmap.DTDBM, (HW_MAX_IO_CMD_INDEX/32) * sizeof(uint32_t));


    M_PRINTF("\nRDRBM GRP %08X\n", rNvmeBitmapSram.bitmapSramRdDataRdyBitmapGroupBitmap.all);
    DUMP_MEM_U32x8("RDRBM0   ", (uint32_t)rNvmeBitmap.RDRBM0, (HW_MAX_IO_CMD_INDEX/32) * sizeof(uint32_t));
    DUMP_MEM_U32x8("RDRBM1   ", (uint32_t)rNvmeBitmap.RDRBM1, (HW_MAX_IO_CMD_INDEX/32) * sizeof(uint32_t));
    DUMP_MEM_U32x8("RDRBMMSK ", (uint32_t)rNvmeBitmap.RDRBMMSK, (HW_MAX_IO_CMD_INDEX/32) * sizeof(uint32_t));

    M_PRINTF("\nCCBM GRP: %08X, CCBM CNT: %08X\n", rNvmeBitmapSram.bitmapSramCmdCompletiBitmapGroupBitmap.all, rNvmeBitmapSram.bitmapSramCmdCompletiBitmapCntr.all);
    DUMP_MEM_U32x8("CCBM     ", (uint32_t)rNvmeBitmap.CCBM, (HW_MAX_IO_CMD_INDEX/32) * sizeof(uint32_t));
}

//-----------------------------------------------------------------------------
/**
    Dump command index usage.
**/
//-----------------------------------------------------------------------------
static void DumpCommandIndexUsage(void)
{
#if 0
    int i;
    int j;
    int total = 0;
    M_PRINTF("--- Command index usage ---\n");
    for (i = 0; i < HW_MAX_IO_CMD_INDEX; i += 8)
    {
        for(j = 0; j < 8; j++)
        {
            M_PRINTF("%3d:%5d ", i+j, gpNvmeIoState->countPerIndex[i+j]);
            total += gpNvmeIoState->countPerIndex[i+j];
        }
        M_PRINTF("\n");
    }
    M_PRINTF("total is %d\n", total);
#endif
}

//-----------------------------------------------------------------------------
/**
    Dump Control Register Direct Access Data.
**/
//-----------------------------------------------------------------------------
static void DumpControlRegisterDirectAccessData(void)
{
    int i;
    uint32_t* pU32;

    M_PRINTF("\n[Controller Register Direct Access Data]\n");

    Debug_DumpMemU32((uint32_t) &rNvmeController, sizeof(rNvmeController));


    for(i = 0; i < cNvmeNumOfFuncSupport ; i++)
    {
        M_PRINTF("\nNVMe Function %d: \n", i);
        M_PRINTF("CAP_0 0x%08x CAP_1 0x%08xVS    0x%08x \n"
                 "INTMS 0x%08x INTMC 0x%08xCC    0x%08x \n"
                 "CSTS  0x%08x NSSR  0x%08xAQA   0x%08x \n"
                 "ASQ_0 0x%08x ASQ_1 0x%08xACQ_0 0x%08x ACQ_1 0x%08x \n",
                 *((uint32_t*)(&rNvmeController.controller[i].CAP)), *((uint32_t*)(&rNvmeController.controller[i].CAP)+1), rNvmeController.controller[i].VS.all,
                 rNvmeController.controller[i].INTMS.all, rNvmeController.controller[i].INTMC.all, rNvmeController.controller[i].CC.all,
                 rNvmeController.controller[i].CSTS.all, rNvmeController.controller[i].NSSR, rNvmeController.controller[i].AQA.all,
                 *((uint32_t*)(&rNvmeController.controller[i].ASQB)), *((uint32_t*)(&rNvmeController.controller[i].ASQB)+1),
                 *((uint32_t*)(&rNvmeController.controller[i].ACQB)), *((uint32_t*)(&rNvmeController.controller[i].ACQB)+1) );
    }

    M_PRINTF("\nDIR_QSTAT_TBL_SRAM_BA:\n");
    pU32 = (uint32_t* ) &rNvmeController.queueStatus[0];
    for(i = 0; i < cNvmeNumOfQueueFunc; i++, pU32 += 4)
    {
        M_PRINTF("Q%2d-", i);
        Debug_DumpMemU32((uint32_t)pU32, sizeof(NvmeQueueStatusSram_t));    // 0x10000228: Queue Status Table (SRAM Portion)
    }

    M_PRINTF("\nDIR_MSIX_TBL_BA:\n");
    pU32 = (uint32_t* ) &rNvmeController.msix[0];
    for(i = 0; i < cNvmeNumOfQueueFunc; i++, pU32 += 4)
    {
        M_PRINTF("Q%2d-", i);
        Debug_DumpMemU32((uint32_t)pU32, sizeof(NvmeMsixEntry_t));        // 0x10000358: MSI-X Table
    }

    M_PRINTF("\nDIR_CMD_PRP_TBL_BA:\n");
    pU32 = (uint32_t* ) &rNvmeController.cmdPrp[0];
    for(i = 0; i < cNvmeNumOfQueueFunc; i++, pU32 += 4)
    {
        M_PRINTF("Q%2d-", i);
        Debug_DumpMemU32((uint32_t)pU32, sizeof(NvmeCmpPrp_t));             // 0x10000488: Queue PRP Table (CMD PRP)
    }

    M_PRINTF("\nDIR_CURR_PRP_TBL_BA:\n");
    pU32 = (uint32_t* ) &rNvmeController.currentPrp[0];
    for(i = 0; i < cNvmeNumOfQueueFunc; i++, pU32 += 4)
    {
        M_PRINTF("Q%2d-", i);
        Debug_DumpMemU32((uint32_t)pU32, sizeof(NvmeCurrentPrp_t));         // 0x100005B8: Queue PRP Table (Current PRP)
    }

    M_PRINTF("\nDIR_NEXT_PRP_TBL_BA:\n");
    pU32 = (uint32_t* ) &rNvmeController.nextPrp[0];
    for(i = 0; i < cNvmeNumOfQueueFunc; i+=2, pU32 += 4)
    {
        M_PRINTF("Q%2d-", i);
        Debug_DumpMemU32((uint32_t)pU32, sizeof(uint64_t)*2);               // 0x100006E8: Queue PRP Table (Next PRP)
    }
}

//-----------------------------------------------------------------------------
/**
    Dump Interrupt Debug Information.
**/
//-----------------------------------------------------------------------------
static void DumpInterruptInfo(void)
{

    int i;
    uint32_t* pU32;


    M_PRINTF("\n[Interrupt info]\n");
    M_PRINTF("\n- PCIe Configuration Header\n");
    M_PRINTF("Config command & status register(%08x)=%08x\n",
           &rPciePf.cmdAndStat.all, rPciePf.cmdAndStat.all);

    M_PRINTF("INTMS %08x INTMC %08x\n",
        rNvmeController.controller[0].INTMS.all, rNvmeController.controller[0].INTMC.all);

    M_PRINTF("MSI  control reg (%08x)=%08x\n", &rPciePf.msiCapabiliHdr.all, rPciePf.msiCapabiliHdr.all);

    M_PRINTF("MSIX control reg (%08x)=%08x\n",
        &rPciePf.msiXCfg.all, rPciePf.msiXCfg.all);

    M_PRINTF("DIR_MSIX_TBL_BA addr 0x%08x\n", rPciePf.msiXPendingBitArray.all);

    M_PRINTF("\n- DIR_MSIX_TBL\n");
    for (i = 0; i < cNvmeCregFuncFts[0]; i++)
    {
        pU32 = (uint32_t*) &rNvmeController.msix[i];
        M_PRINTF("Vector %2d: lower 0x%08x high 0x%08x data %08x control %08x\n",
               i, *(pU32), *(pU32+1), *(pU32+2), *(pU32+3) );
    }

    M_PRINTF("\n- DIR_MSIX_PBA_TBL_BA addr 0x%08x\n",(uint32_t*) &rNvmeController.msixPba[0]);
    for (i = 0; i < cNvmeCregFuncFts[0]; i++)
    {
        pU32 =  (uint32_t*) &rNvmeController.msixPba[i];
        M_PRINTF("Vector %2d: 0x%08x 0x%08x\n", i, *pU32, *(pU32+1));
    }

    M_PRINTF("\n- STSM interrupt registers\n");
    M_PRINTF("STSM_ERR_FIFO_CTRL     (0x%08x) = 0x%08x\n"
             "STSM_INTP_VGRP0_VLD    (0x%08x) = 0x%08x\n"
             "STSM_INTP_VGRP0_STS    (0x%08x) = 0x%08x\n"
             "STSM_INTP_VGRP0_CLS_DIS(0x%08x) = 0x%08x\n"
             "STSM_INTP_AGG_MON      (0x%08x) = 0x%08x\n",
             &(rNvmeStatMngr.statManagerErrFifoCtrl.all), rNvmeStatMngr.statManagerErrFifoCtrl.all,
             &rNvmeStatMngr.statManagerIntrVectorValidGroup[0].all, rNvmeStatMngr.statManagerIntrVectorValidGroup[0].all,
             &rNvmeStatMngr.statManagerIntrVectorStatGroup[0].all, rNvmeStatMngr.statManagerIntrVectorStatGroup[0].all,
             &rNvmeStatMngr.statManagerIntrVectorCoalesciDisGroup[0].all, rNvmeStatMngr.statManagerIntrVectorCoalesciDisGroup[0].all,
             &rNvmeStatMngr.statManagerAggregatMonitor.all, rNvmeStatMngr.statManagerAggregatMonitor.all);

    for(i = 0; i < 19; i++)
    {
        M_PRINTF("STSM_INTP_AGG_STATUS_V%2d addr 0x%08x val 0x%08x\n", i,
            &rNvmeStatMngr.statManagerAggregatStatForIntrVector[i].all, rNvmeStatMngr.statManagerAggregatStatForIntrVector[i].all);
    }

}


//-----------------------------------------------------------------------------
/**
    Dump Error Report FIFO.
**/
//-----------------------------------------------------------------------------
static void DumpErrorReportFifo()
{
    StatManagerErrFifoStat_t errorFifoStatus;
    uint32_t i;

    M_PRINTF("\n[Error FIFO]\n");

    errorFifoStatus.all = rNvmeStatMngr.statManagerErrFifoStat.all;
    M_PRINTF("\n- STSM_ERR_FIFO_STS 0x%x\n", errorFifoStatus);

    if (errorFifoStatus.b.ERR_RPT_FIFO_STAT <= 1) // 0 = Fifo is not empty and full, 1 = Fifo is full
    {
        for (i = 0; i < errorFifoStatus.b.ERR_RPT_VAL_ENT_CNT; i++)
        {
            M_PRINTF("entry %08x FIFO: %x\n", i, rNvmeStatMngr.ERR_RPT_FIFO_RD_PORT);
        }
    }

#if 0
    M_PRINTF("--- Processed Error FIFO ---\n");
    for (i = 0; i < gpNvmeIoState->errorFifoCount; i++)
    {
        M_PRINTF("entry %08x FIFO:high %x,low %x\n", i, gpNvmeIoState->errorFifo[i].dw[0], gpNvmeIoState->errorFifo[1]);
    }
#endif
}

//-----------------------------------------------------------------------------
/**
    Dump AXI and PCIe Payload Info.
**/
//-----------------------------------------------------------------------------
static void DumpAxiAndPayload(void)
{
    M_PRINTF("\n[AXI_WR debug informations]\n");
    Debug_DumpMemU32((uint32_t)&rNvmeDataMngr.RF50111A0_31_0_RSVD, 0x1c4-0x1a0);

    M_PRINTF("\nPCIE_DEV_CTRL_REG:             %8X\n", rPciePf.deviceCtrlAndStat.all);
    M_PRINTF("PCIe DAB1 Write Configuration: %8X\n", rNvmePktMngr.pcieDab1WrCfg.all);
    M_PRINTF("PCIe DAB1 Read Configuration:  %8X\n", rNvmePktMngr.pcieDab1RdCfg.all);
    M_PRINTF("Data Manager Lba Format[0]:    %8X\n", rNvmeDataMngr.lbaFmt[0].all);
}

//-----------------------------------------------------------------------------
/**
    Dump HWLL Info..
**/
//-----------------------------------------------------------------------------
static void DumpHwllInfo(void)
{
    M_PRINTF("\n[HWLL]\n");

    M_PRINTF("\n- HWLL HT Table\n");
    Debug_DumpMemU32((uint32_t)0xF541C000, 0x400);

    M_PRINTF("\n- HWLL Link Array\n");
    Debug_DumpMemU32((uint32_t)0xF541D000, 0x400);
}

//-----------------------------------------------------------------------------
/**
    Dump MC5 info.
**/
//-----------------------------------------------------------------------------
static void DumpMc5Info(void)
{
    M_PRINTF("\n[MC5 debug informations]\n");
    Debug_DumpMemU32((uint32_t)0xF1000000, 0x870);
}

//-----------------------------------------------------------------------------
/**
    Dump PRP SRAM Information
**/
//-----------------------------------------------------------------------------
static void DumpPrpInfo(void)
{
    uint32_t index;
    uint32_t value;

    M_PRINTF("\n[PRP SRAM]\n");

    M_PRINTF("\n- PRP SRAM Address 1\n");
    Debug_DumpMemU32((uint32_t)0xF5411000, 0x400);

    M_PRINTF("\n- PRP SRAM Address 2\n");
    Debug_DumpMemU32((uint32_t)0xF5412000, 0x400);

    M_PRINTF("\n- PRP SRAM Byte\n");
    Debug_DumpMemU32((uint32_t)0xF5413000, 0x400);

    M_PRINTF("\n- PRP CAM\n");
    for(index = 0; index < 256; index++)
    {
        if(index % 4 == 0)
        {
            M_PRINTF("%4X: ", index);
        }
        REG_U32(0xF50111D0) = index;    // 0xF50111D0: Data Manager CAM Indirect Access
        value = REG_U32(0xF50111D4);    // 0xF50111D4: Data Manager CAM Indirect Access Data
        M_PRINTF("%8X ", value);
        if(index % 4 == 3)
        {
            M_PRINTF("\n");
        }
    }

}

//-----------------------------------------------------------------------------
/**
    Dump Queue Status Table by fid and qid
**/
//-----------------------------------------------------------------------------
static void DumpQueueStatusTableByFidAndQid(uint32_t fid, uint32_t qid)
{
    uint32_t entryIndex;

    entryIndex = cNvmeCregFuncFtbi[fid] + qid;
    if(rNvmeController.queueReg[entryIndex].sq.b.QC==1 || rNvmeController.queueReg[entryIndex].cq.b.QC==1)
    {
        M_PRINTF("\n- QUEUE STATUS (FID:%d, QID:%d)\n", fid, qid);
        DUMP_MEM_U32("SRAM:    ", (uint32_t)(&rNvmeController.queueStatus[entryIndex]), 0x10);
        DUMP_MEM_U32("REG:     ", (uint32_t)(&rNvmeController.queueReg[entryIndex]),    0x10);
        DUMP_MEM_U32("MSIX TBL:", (uint32_t)(&rNvmeController.msix[entryIndex]) ,       0x10);
        DUMP_MEM_U32("CMD  PRP:", (uint32_t)(&rNvmeController.cmdPrp[entryIndex]),      0x10);
        DUMP_MEM_U32("CURR PRP:", (uint32_t)(&rNvmeController.currentPrp[entryIndex]),  0x10);
        DUMP_MEM_U32("NEXT PRP:", (uint32_t)(&rNvmeController.nextPrp[entryIndex]),     0x08);
        DUMP_MEM_U32("MSIX PBA:", (uint32_t)(&rNvmeController.msixPba[entryIndex]),     0x08);
    }
}

//-----------------------------------------------------------------------------
/**
    Dump Queue Status Table
**/
//-----------------------------------------------------------------------------
static void DumpQueueStatusTable(void)
{
    uint32_t funcIndex;
    uint32_t queueIndex;

    M_PRINTF("\n[QUEUE STATUS TABLE]\n");

    M_PRINTF("\n- FUNC ALLOCATION STATUS\n");
    for(funcIndex = 0; funcIndex < 3; funcIndex++)
    {
        M_PRINTF("FUNC%d: FTBI=0x%2X, FTS=0x%02X\n", funcIndex, cNvmeCregFuncFtbi[funcIndex], cNvmeCregFuncFts[funcIndex]);
    }

    for(funcIndex = 0; funcIndex < 3; funcIndex++)
    {
        if(cNvmeCregFuncFts[funcIndex]!=0)
        {
            for(queueIndex = 0; queueIndex < cNvmeCregFuncFts[funcIndex]; queueIndex++)
            {
                DumpQueueStatusTableByFidAndQid(funcIndex, queueIndex);
            }
        }
    }

}

//-----------------------------------------------------------------------------
/**
    Dump LCE SRAM.
**/
//-----------------------------------------------------------------------------
static void DumpLceSram(void)
{
    M_PRINTF("\n[LCE SRAM]\n");
    Debug_DumpMemU32((uint32_t)0xF5410000 , 0x40);
}

//-----------------------------------------------------------------------------
/**
    Dump Nvme
**/
//-----------------------------------------------------------------------------
void DumpNvme(void)
{

    uint32_t idx;

    Debug_DumpMemU32(0xF5000000, 0x944);
    Debug_DumpMemU32(0xF5010000, 0x108);
    Debug_DumpMemU32(0xF5011000, 0x1E4);
    Debug_DumpMemU32(0xF5012000, 0x09C);
    Debug_DumpMemU32(0xF5013000, 0x17C);
    Debug_DumpMemU32(0xF5014000, 0x038);
    Debug_DumpMemU32(0xF5015000, 0x248);
    Debug_DumpMemU32(0xF5016000, 0x0A8);
    Debug_DumpMemU32(0xF5017000, 0x0A4);
    Debug_DumpMemU32(0xF5018000, 0x104);

    D_PRINTF("\n");

    Debug_DumpMemU32(0xF5401000, 0x4000);
    Debug_DumpMemU32(0xF5409000, 0x100);
    Debug_DumpMemU32(0xF5410000, 0x400);
    Debug_DumpMemU32(0xF5411000, 0x400);
    Debug_DumpMemU32(0xF5412000, 0x400);
    Debug_DumpMemU32(0xF5413000, 0x400);
    Debug_DumpMemU32(0xF5414000, 0x400);
    Debug_DumpMemU32(0xF541C000, 0x400);
    Debug_DumpMemU32(0xF541D000, 0x400);
    Debug_DumpMemU32(0xF541E000, 0x800);
    Debug_DumpMemU32(0xF541F000, 0x180);


    D_PRINTF("\n");

    for (idx = 0; idx < 256; idx ++)
    {
        REG_U32(0xF50111D0) = idx;
        D_PRINTF("RF50111D4[%03d] = %08x\n", idx, REG_U32(0xF50111D4));
    }

    D_PRINTF("\n");

    REG_U32(0xF50180F0) = 0x00010000;
    for (idx = 0; idx < 5; idx ++)
    {
        REG_U32(0xF500016C) = idx;
        D_PRINTF("RF50180F4[%03d] = %08x\n", idx, REG_U32(0xF50180F4));
    }

    D_PRINTF("\n");

    REG_U32(0xF50180F0) = 0x01010100;
    for (idx = 0; idx < 3; idx ++)
    {
        REG_U32(0xF501708C) = idx;
        D_PRINTF("RF50180F4[%03d] = %08x\n", idx, REG_U32(0xF50180F4));
    }

    D_PRINTF("\n");

    REG_U32(0xF50180F0) = 0x02010200;
    for (idx = 0; idx < 2; idx ++)
    {
        REG_U32(0xF501609C) = idx;
        D_PRINTF("RF50180F4[%03d] = %08x\n", idx, REG_U32(0xF50180F4));
    }

    D_PRINTF("\n");

    REG_U32(0xF50180F0) = 0x03010300;
    for (idx = 0; idx < 18; idx ++)
    {
        REG_U32(0xF501402C) = idx;
        D_PRINTF("RF50180F4[%03d] = %08x\n", idx, REG_U32(0xF50180F4));
    }

    D_PRINTF("\n");

    REG_U32(0xF50180F0) = 0x04010400;
    for (idx = 0; idx < 7; idx ++)
    {
        REG_U32(0xF501007C) = idx;
        D_PRINTF("RF50180F4[%03d] = %08x\n", idx, REG_U32(0xF50180F4));
    }

    D_PRINTF("\n");

    REG_U32(0xF50180F0) = 0x07010700;
    for (idx = 0; idx < 144; idx ++)
    {
        REG_U32(0xF5011120) = idx;
        D_PRINTF("RF50180F4[%03d] = %08x\n", idx, REG_U32(0xF50180F4));
    }

    D_PRINTF("\n");

    REG_U32(0xF50180F0) = 0x08010800;
    for (idx = 0; idx < 3; idx ++)
    {
        REG_U32(0xF5012070) = idx;
        D_PRINTF("RF50180F4[%03d] = %08x\n", idx, REG_U32(0xF50180F4));
    }

    D_PRINTF("\n");
    for (idx = 0x0; idx < 0x5; idx ++)
    {
        REG_U32(0xF5013030) = idx;
        D_PRINTF("RF5013034[%03d] = %08x\n", idx, REG_U32(0xF5013034));
    }
    D_PRINTF("\n");
    for (idx = 0x100; idx < 0x106; idx ++)
    {
        REG_U32(0xF5013030) = idx;
        D_PRINTF("RF5013034[%03d] = %08x\n", idx, REG_U32(0xF5013034));
    }
    D_PRINTF("\n");
    for (idx = 0x301; idx < 0x305; idx ++)
    {
        REG_U32(0xF5013030) = idx;
        D_PRINTF("RF5013034[%03d] = %08x\n", idx, REG_U32(0xF5013034));
    }
    D_PRINTF("\n");
    for (idx = 0x381; idx < 0x386; idx ++)
    {
        REG_U32(0xF5013030) = idx;
        D_PRINTF("RF5013034[%03d] = %08x\n", idx, REG_U32(0xF5013034));
    }

}

//-----------------------------------------------------------------------------
/**
    Dump debug information of an NVMe IP.

    @param[in]  flag    modules to print out to UART
                        0x00000000: All
                        0x00000001: NVMe Registers
                        0x00000002: Bitmap SRAM
                        0x00000004: Monitor Signals
                        0x00000008: Command Indexes and SRAM
                        0x00000010: Queue Status
                        0x00000020: Controller Register SRAM
                        0x00000040: Interrupts
                        0x00000080: Error FIFOs
                        0x00010000: ETC...
**/
//-----------------------------------------------------------------------------
void HalNvmeDebug_DumpNvmeHwInfo(uint32_t flag)
{
    if(flag == 0)
    {
        flag = 0xFFFFFFFF;
    }

    DumpNvmeGlobalInfo();

    if(flag & 0x00000001)
    {
        DumpPacketManagerInfo();
        DumpControlRegisterInfo();
        DumpCommandFetcherInfo();
        DumpCommandSramInfo();
        DumpDataManagerInfo();
        DumpQueueArbiterInfo();
        DumpStatusManagerInfo();
    }

    if(flag & 0x00000002)
    {
        DumpBitmapSramInfo();
    }

    if(flag & 0x00000004)
    {
        DumpMonitorSignalInfo();
    }

    if(flag & 0x00000008)
    {
        DumpCommandIndexUsage();
        DumpHwIoCmd();
    }

    if(flag & 0x00000010)
    {
        DumpQueueStatusTable();
    }

    if(flag & 0x00000020)
    {
        DumpControlRegisterDirectAccessData();
    }

    if(flag & 0x00000040)
    {
        DumpInterruptInfo();
    }

    if(flag & 0x00000080)
    {
        DumpErrorReportFifo();
    }

    if(flag & 0x00010000)
    {
        DumpAxiAndPayload();
        DumpMc5Info();
        DumpPrpInfo();
        DumpHwllInfo();
        DumpLceSram();
    }
}
#endif

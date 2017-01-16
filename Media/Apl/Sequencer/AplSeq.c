//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "MediaPkg.h"
#include "AplSeq.h"

#include "PerfSection.h"
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static __inline U16 Seq_FreeTrkCount(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
AplSeqInfo_t gAplSeqInfo;

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
#include "FastSection.h"
//-----------------------------------------------------------------------------
/*
  Description: Apl Sequncer initialization
  Input      : initMode: init mode
  return     : error code
*/
//-----------------------------------------------------------------------------
Error_t AplSeq_OneTimeInit(InitBootMode_t initMode)
{
    U16 idx;

    MEM_CLR(&gAplSeqInfo, sizeof(gAplSeqInfo));

    for (idx = 0; idx < MAX_TRK_NUM; idx++)
    {
        gAplSeqInfo.trkIdArray[idx] = idx;
    }

    gAplSeqInfo.trkPtrR = 0;
    gAplSeqInfo.trkPtrW = MAX_TRK_NUM;

    return cEcNoError;
}

#include "NormSection.h"
//-----------------------------------------------------------------------------
/*
  Description: Dump Apl Sequencer information
  Input      : N/A
  return     : error code
*/
//-----------------------------------------------------------------------------
Error_t AplSeq_DumpInfo(void)
{
    U16 idx;

    M_PRINTF("\n[SEQ]\n");

    for (idx = 0; idx < MAX_TRK_NUM; idx++)
    {
        M_PRINTF("%3d :[%3d] ", idx, gAplSeqInfo.trkIdArray[idx]);
        if ((idx % 8) == 7) M_PRINTF("\n");
    }

    M_PRINTF("trkPtrR[%d] trkPtrW[%d] FreeCnt[%d]\n", gAplSeqInfo.trkPtrR, gAplSeqInfo.trkPtrW, Seq_FreeTrkCount());

    M_PRINTF("FIFO R/W[0x%x / 0x%X]\n", gDes_fifo_r_ptr, gDes_fifo_w_ptr);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/*
  Description: Get free track id count
  Input      : N/A
  return     : track id count
*/
//-----------------------------------------------------------------------------
static __inline U16 Seq_FreeTrkCount(void)
{
    if (gAplSeqInfo.trkPtrR > gAplSeqInfo.trkPtrW) return gAplSeqInfo.trkPtrW + MAX_TRK_NUM + 1 - gAplSeqInfo.trkPtrR;

    return gAplSeqInfo.trkPtrW - gAplSeqInfo.trkPtrR;
}

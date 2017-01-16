#pragma once
//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "Fio.h"
#include "Cache.h"

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#if (_NAND_ == NAND_MLC)

    #if (CO_SUPPORT_EARLY_MOVE_BLK == TRUE)
        #define EARLY_MOVE_BASE     (100000)
        #define EARLY_MOVE_MIN_CNT  (30000)
        #define MAXECCNT            (3000)
    #endif

#define GC_REQ_CACHE_ENTRY_LP                      (GC_NF_NUM_CHANNELS * GC_ECU_AU_PER_PAGE)
#define GC_REQ_SSD_COUNT_LP                        (GC_REQ_CACHE_ENTRY_LP)
#define GC_ONE_SHOT_PAA_CNT                        (GC_NF_NUM_CHANNELS * GC_ECU_AU_PER_PAGE)
//-----------------------------------------------------------------------------
//  Structure definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void gc_setPaaLP(U8 p2lIdx, U8 lun, U8 ce);
void gc_issueJobLP(U8 curP2lIdx);
U8   gc_setPaa(U8 curP2lIdx);
U8   gc_issueJob(U8 curP2lIdx);
U8   gc_issueJob_Aux(U8 curP2lIdx);
U8   GC_Get_NxtDie(U8 curP2lIdx);
void GC_Test_ReadLP(U16 blk);
void GC_P2lDumpInfo(U8 bufferSelect, U8 l2pcheck, U16 block);

#endif


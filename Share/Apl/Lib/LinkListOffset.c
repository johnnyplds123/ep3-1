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
//! @brief  Circular buffer management code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "MemIo.h"
#include "MemAlloc.h"
#include "LinkListOffset.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define EN_LINKLISTX_UNITTEST   0

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

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
    Doublly linked list iteratation call back.

    @param[in]  pList           List start pointer
    @param[in]  callBack        call back function while iteration
    @param[in]  offset          structure offset of link entry

    @return  none
**/
//-----------------------------------------------------------------------------
void DlxForEachCallBack(LinkHeadTailOffset_t* pList, LlxCallBack_t callBack, uint32_t offset)
{
    DlinkOffset_t* pEntry;

    DLX_FOR_EACH(pEntry, pList)
    {
        callBack((void*)((uint8_t *)pEntry-offset));
    }
}


#if (EN_LINKLISTX_UNITTEST >= 1)

#define AllocPcmdQ()                        (Pcmd_PopFromHead(cPcmdFree))
#define ReleasePcmdQ(pPcmdQ)                (Pcmd_PushToTail(cPcmdFree, pPcmdQ))
#define PCMD_GET_INDEX(pPcmdQ)              ((pPcmdQ)-gPcmdQ)

const static uint32_t cMaxPcmdQCount = 10;

typedef enum
{
    cPcmdFree = 0,
    cPcmdAlloc,
    cPcmdCount
} PcmdStateEnum_t;

typedef struct
{
    uint32_t count;
    LinkHeadTailOffset_t headTail;
} PcmdStateList;

typedef struct
{
    PcmdStateList stateList[cPcmdCount];
} PcmdList_t;

typedef struct
{
    uint16_t        index;
    uint16_t        command;
    DlinkOffset_t   dlink;
    char            str[4];
} PcmdQ_t;

PcmdList_t* gPcmdList;
PcmdQ_t* gPcmdQ;

void DumpPcmdEntry(PcmdQ_t* cmdEntry)
{
    D_PRINTF("%4X: %8X, %s %8X(P:%4X, N:%4X)\n", cmdEntry->index, cmdEntry->command, cmdEntry->str, &cmdEntry->dlink, cmdEntry->dlink.prev, cmdEntry->dlink.next);
}

void DumpAllPcmdQ(void)
{
    uint32_t index;

    for(index = cPcmdFree; index < cPcmdCount; index++)
    {
        D_PRINTF(">PCmd %s Queue: %d entries, %8X(H:%4X, T:%4X)\n",
            (index==0?"Free":"Alloc"), gPcmdList->stateList[index].count,
            &gPcmdList->stateList[index].headTail, gPcmdList->stateList[index].headTail.head, gPcmdList->stateList[index].headTail.tail);
        DlxForEachCallBack(&gPcmdList->stateList[index].headTail, (LlxCallBack_t)DumpPcmdEntry, offsetof(PcmdQ_t, dlink));
    }
    D_PRINTF("\n");
}

void FillPcmdEntry(PcmdQ_t* pPcmdQ, uint32_t command, char* str)
{
    pPcmdQ->command = command;
    MEM_COPY(pPcmdQ->str, str, sizeof(str));
}

void Pcmd_PushToTail(PcmdStateEnum_t state, PcmdQ_t* pPcmdQ)
{
    // make sure the cmd queue is not in list
    ASSERT2(pPcmdQ->dlink.prev==0, state);

    gPcmdList->stateList[state].count++;
    DlxAddToTail(&gPcmdList->stateList[state].headTail, &pPcmdQ->dlink);
}

PcmdQ_t* Pcmd_PopFromHead(PcmdStateEnum_t state)
{
    DlinkOffset_t* pEntry;
    PcmdQ_t* pPcmdQ;

    if(gPcmdList->stateList[state].count == 0) return (NULL);

    gPcmdList->stateList[state].count--;

    pEntry = DlxPopFromHead(&gPcmdList->stateList[state].headTail);
    pPcmdQ = STR_PTR_FROM_MEMBER(pEntry, PcmdQ_t, dlink);
    pPcmdQ->dlink.prev = 0;     // 0 mark for debugging
    pPcmdQ->dlink.next = state; // state id for debugging

    return (pPcmdQ);
}

void TestLinkListX(void)
{
    uint32_t index;
    PcmdQ_t* pPcmdQx;
    PcmdQ_t* pPcmdQy;
    PcmdQ_t* pPcmdQz;

    gPcmdList = MEM_AllocSspm(sizeof(PcmdList_t), 8);
    gPcmdQ = MEM_AllocSspm(sizeof(PcmdQ_t) * cMaxPcmdQCount, 8);

    // Initialize both Doubly Linked List Structures of a free list and a allocation list
    for(index = cPcmdFree; index < cPcmdCount; index++)
    {
        DlxInit(&gPcmdList->stateList[index].headTail, gPcmdQ, cMaxPcmdQCount, sizeof(PcmdQ_t));
        gPcmdList->stateList[index].count = 0;
    }

    // Initialize gPcmdQ entries and add all entries into a frre queue at first.
    for( index = 0; index < cMaxPcmdQCount; index++)
    {
        gPcmdQ[index].dlink.prev = 0;
        Pcmd_PushToTail(cPcmdFree, &gPcmdQ[index]);
        gPcmdQ[index].index = index;
        FillPcmdEntry(&gPcmdQ[index], 0, "  A"); gPcmdQ[index].str[2] += index;
    }
    DumpAllPcmdQ();

    // Allocate three PcmdQs from the free queue.
    pPcmdQx = AllocPcmdQ();
    pPcmdQy = AllocPcmdQ();
    pPcmdQz = AllocPcmdQ();
    DumpAllPcmdQ();

    // Fill allocated entries and push them into allocation queue.
    FillPcmdEntry(pPcmdQx, 0x20, "RD ");
    FillPcmdEntry(pPcmdQy, 0x30, "WR ");
    FillPcmdEntry(pPcmdQz, 0x70, "SK ");
    Pcmd_PushToTail(cPcmdAlloc, pPcmdQz);
    Pcmd_PushToTail(cPcmdAlloc, pPcmdQy);
    Pcmd_PushToTail(cPcmdAlloc, pPcmdQx);
    DumpAllPcmdQ();

    // Release two entries from an allocation queue to a free queue.
    pPcmdQy = Pcmd_PopFromHead(cPcmdAlloc);
    ReleasePcmdQ(pPcmdQy);
    pPcmdQz = Pcmd_PopFromHead(cPcmdAlloc);
    ReleasePcmdQ(pPcmdQz);
    DumpAllPcmdQ();

    // Get and fill two entries from a free queue, then push them into an allocation queue.
    pPcmdQy = AllocPcmdQ();
    FillPcmdEntry(pPcmdQy, 0x31, "WR1");
    Pcmd_PushToTail(cPcmdAlloc, pPcmdQy);
    pPcmdQz = AllocPcmdQ();
    FillPcmdEntry(pPcmdQz, 0x71, "SK1");
    Pcmd_PushToTail(cPcmdAlloc, pPcmdQz);
    DumpAllPcmdQ();
}

#endif // (EN_LINKLISTX_UNITTEST >= 1)





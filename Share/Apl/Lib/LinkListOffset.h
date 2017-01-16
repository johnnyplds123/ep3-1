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
//! @brief  16 bits Offset Linked List management APIs (low 2 bytes offset pointer)
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define UPPER_ADDR(ADDR)                (((uint32_t)ADDR) & (0xFFFF0000))
#define LOW_ADDR(ADDR)                  (((uint32_t)ADDR) & (0x0000FFFF))
#define X2DLINK(pDl, WPTR)              ((DlinkOffset_t*) ((uint8_t*)UPPER_ADDR(pDl) + (WPTR)))
#define D2XLINK(PTR)                    ((uint16_t) LOW_ADDR(PTR))
#define DLX_FIRST_ENTRY(pDl)            (X2DLINK((pDl), (pDl)->head))
#define DLX_LAST_ENTRY(pDl)             (X2DLINK((pDl), (pDl)->tail))
#define DLX_IS_NULL(pDl, pEntry)        ((void*)(pDl) == (void*)(pEntry))
#define DLX_GET_PREV(pDl, pEntry)       (X2DLINK((pDl), (pEntry)->prev))
#define DLX_GET_NEXT(pDl, pEntry)       (X2DLINK((pDl), (pEntry)->next))
#define DLX_HT_PTR(X)                   ((LinkHeadTailOffset_t *)(X))
#define DLX_ET_PTR(X)                   ((DlinkOffset_t *)(X))
#define DLX_HT_CLEAR(X)                 {(X).prev = NULL; (X).next = NULL;}

#define DLX_FOR_EACH(pEntry, pDl) \
    for (pEntry = DLX_FIRST_ENTRY(pDl); !DLX_IS_NULL(pDl, pEntry); pEntry = DLX_GET_NEXT(pDl, pEntry))

#define DLX_FOR_EACH_BACKWARD(pEntry, pDl) \
    for (pEntry = DLX_LAST_ENTRY(pDl); !DLX_IS_NULL(pDl, pEntry); pEntry = DLX_GET_PREV(pDl, pEntry))

#define DLX_FOR_EACH_FROM(pEntry, pDl) \
    for (; !DLX_IS_NULL(pDl, pEntry); pEntry = DLX_GET_NEXT(pDl, pEntry))

#define DLX_FOR_EACH_BACKWARD_FROM(pEntry, pDl) \
    for (; !DXL_IS_NULL(pDl, pEntry); pEntry = DLX_GET_PREV(pDl, pEntry))


//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

typedef void (*LlxCallBack_t)(void*);   ///< linked list call back type

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void DlxForEachCallBack(LinkHeadTailOffset_t* pList, LlxCallBack_t callBack, uint32_t offset);

//-----------------------------------------------------------------------------
/**
    Initialize dobule link list by 16 bits offset pointer.\n

    Linked list uses below link mechanism to make insertion and deletion simpler.\n
    (see Sentinel linked list)\n

     -------------------------------------+   \n
     |                                    |   \n
   head -> |--| -> |--| -> |--| -> |--|---|   \n
           |  |    |  |    |  |    |  |       \n
   tail <- |__| <- |__| <- |__| <- |__|<---   \n
     |                                    |   \n
     -------------------------------------|   \n

    @param[in]  pList           list pointer which have 16 bits head and tail offset pointer.
    @param[in]  entryArray      pointer to base address of entries
    @param[in]  numberOfEntries number of entries
    @param[in]  sizeOf          size of entry
**/
//-----------------------------------------------------------------------------
__inline void DlxInit(LinkHeadTailOffset_t* pList, void* entryArray, uint32_t numberOfEntries, uint32_t sizeOf)
{
    void* base;
    
    pList->head = D2XLINK(pList);
    pList->tail = D2XLINK(pList);
    base = (void*) UPPER_ADDR(entryArray);

    // Check whether the uppder addresses of a headTail and an entryArray are same.
    ASSERT(base == (void*) UPPER_ADDR(pList));   
    
    // Check whether the uppder addresses of all entryArrays are same.
    ASSERT(base == (void*) UPPER_ADDR((uint8_t*)entryArray + numberOfEntries * sizeOf));
}

//-----------------------------------------------------------------------------
/**
    Insert new entry to doubly linked list using 16 bits offset pointer.

    @param[in]     pEntry      entry pointer to be inserted
    @param[in]     pPrev       Previous entry pointer
    @param[in]     pNext       Next entry pointer
**/
//-----------------------------------------------------------------------------
__inline void DlxInsert(DlinkOffset_t* pEntry, DlinkOffset_t* pPrev, DlinkOffset_t* pNext)
{
    pEntry->next = D2XLINK(pNext);
    pEntry->prev = D2XLINK(pPrev);
    pNext->prev = D2XLINK(pEntry);
    pPrev->next = D2XLINK(pEntry);
}

//-----------------------------------------------------------------------------
/**
    Add new entry to next position of specified location using 16 bits offset pointer.

    @param[in]     pList       list pointer which have head and tail of 16 bits offset pointers
    @param[in]     pEntry      entry pointer to be added
    @param[in]     pLocation   location pointer
**/
//-----------------------------------------------------------------------------
__inline void DlxAddToNext(LinkHeadTailOffset_t* pList, DlinkOffset_t* pEntry, DlinkOffset_t* pLocation)
{
    DlxInsert(pEntry, pLocation, X2DLINK(pList, pLocation->next));
}

//-----------------------------------------------------------------------------
/**
    Add new entry to previous position  of specified location using 16 bits offset pointer.

    @param[in]     pList       list pointer which have head and tail of 16 bits offset pointers
    @param[in]     pEntry      entry pointer to be added
    @param[out]    pLocation   location pointer
**/
//-----------------------------------------------------------------------------
__inline void DlxAddToPrev(LinkHeadTailOffset_t* pList, DlinkOffset_t* pEntry, DlinkOffset_t* pLocation)
{
    DlxInsert(pEntry, X2DLINK(pList, pLocation->prev), pLocation);
}

//-----------------------------------------------------------------------------
/**
    Add new entry to head of list using 16 bits offset pointer.

    @param[in]      pList       list pointer which have head and tail of 16 bits offset pointers
    @param[in]      pEntry      entry pointer to be added
**/
//-----------------------------------------------------------------------------
__inline void DlxAddToHead(LinkHeadTailOffset_t* pList, DlinkOffset_t* pEntry)
{
    DlxAddToPrev(pList, pEntry, X2DLINK(pList, pList->head));
}

//-----------------------------------------------------------------------------
/**
    Add new entry to tail of list using 16 bits offset pointer.

    @param[in]      pList       list pointer which have head and tail of 16 bits offset pointers
    @param[in]      pEntry      entry pointer to be added
**/
//-----------------------------------------------------------------------------
__inline void DlxAddToTail(LinkHeadTailOffset_t* pList, DlinkOffset_t* pEntry)
{
    DlxAddToNext(pList, pEntry, X2DLINK(pList, pList->tail));
}

//-----------------------------------------------------------------------------
/**
    Remove entry from list using 16 bits offset pointer.

    @param[in]      pList       list pointer which have head and tail of 16 bits offset pointers
    @param[in]      pEntry      entry pointer to be added
**/
//-----------------------------------------------------------------------------
__inline void DlxRemove(LinkHeadTailOffset_t* pList, DlinkOffset_t* pEntry)
{
    X2DLINK(pList, pEntry->prev)->next = pEntry->next;
    X2DLINK(pList, pEntry->next)->prev = pEntry->prev;
}

//-----------------------------------------------------------------------------
/**
    Pop a entry from head of list using 16 bits offset pointer.

    @param[in]      pList       list pointer which have head and tail of 16 bits offset pointers
**/
//-----------------------------------------------------------------------------
__inline void* DlxPopFromHead(LinkHeadTailOffset_t* pList)
{
    DlinkOffset_t* pEntry;

    pEntry = X2DLINK(pList, pList->head);

    X2DLINK(pList, pEntry->next)->prev = D2XLINK(pList);
    pList->head = pEntry->next;

    return pEntry;
}

//-----------------------------------------------------------------------------
/**
    Pop a entry from tail of list using 16 bits offset pointer.

    @param[in]      pList       list pointer which have head and tail of 16 bits offset pointers
**/
//-----------------------------------------------------------------------------
__inline void* DlxPopFromTail(LinkHeadTailOffset_t* pList)
{
    DlinkOffset_t* pEntry;

    pEntry = X2DLINK(pList, pList->tail);

    X2DLINK(pList, pEntry->prev)->next = D2XLINK(pList);
    pList->tail = pEntry->prev;

    return pEntry;
}


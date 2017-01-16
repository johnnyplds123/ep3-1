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
//! @brief  Linked List management APIs
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define DL_FIRST_ENTRY(pDl)             ((pDl)->head)
#define DL_LAST_ENTRY(pDl)              ((pDl)->tail)
#define DL_IS_NULL(pDl, pEntry)         ((void*)(pDl) == (void*)(pEntry))
#define DL_GET_PREV(pEntry)             ((DlinkEntry_t *)pEntry->prev)
#define DL_GET_NEXT(pEntry)             ((DlinkEntry_t *)pEntry->next)
#define DL_HT_PTR(X)                    ((LinkHeadTail_t *)(X))
#define DL_ET_PTR(X)                    ((DlinkEntry_t *)(X))
#define DL_HT_CLEAR(X)                  {(X).prev = NULL; (X).next = NULL;}

#define DL_FOR_EACH(pEntry, pDl) \
    for (pEntry = DL_FIRST_ENTRY(pDl); !DL_IS_NULL(pDl, pEntry); pEntry = DL_GET_NEXT(pEntry))

#define DL_FOR_EACH_BACKWARD(pEntry, pDl) \
    for (pEntry = DL_LAST_ENTRY(pDl); !DL_IS_NULL(pDl, pEntry); pEntry = DL_GET_PREV(pEntry))

#define DL_FOR_EACH_FROM(pEntry, pDl) \
    for (; !DL_IS_NULL(pDl, pEntry); pEntry = DL_GET_NEXT(pEntry))

#define DL_FOR_EACH_BACKWARD_FROM(pEntry, pDl) \
    for (; !DL_IS_NULL(pDl, pEntry); pEntry = DL_GET_PREV(pEntry))


//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef void (*LlCallBack_t)(void*);   ///< linked list call back type

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void DlForEachCallBack(LinkHeadTail_t* pList, LlCallBack_t callBack, uint32_t offset);

//-----------------------------------------------------------------------------
/**
    Initialize dobule link list.\n

    Linked list uses below link mechanism to make insertion and deletion simpler.\n
    (see Sentinel linked list)\n

     -------------------------------------+   \n
     |                                    |   \n
   head -> |--| -> |--| -> |--| -> |--|---|   \n
           |  |    |  |    |  |    |  |       \n
   tail <- |__| <- |__| <- |__| <- |__|<---   \n
     |                                    |   \n
     -------------------------------------|   \n

    @param[out]     pList       initialized list pointer
**/
//-----------------------------------------------------------------------------
static __inline void DlInit(LinkHeadTail_t* pList)
{
    pList->head = pList;
    pList->tail = pList;
}

//-----------------------------------------------------------------------------
/**
    Insert new entry to doubly linked list.

    @param[in]     pEntry      entry pointer to be inserted
    @param[in]     pPrev       Previous entry pointer
    @param[in]     pNext       Next entry pointer
**/
//-----------------------------------------------------------------------------
static __inline void DlInsert(DlinkEntry_t* pEntry, DlinkEntry_t* pPrev, DlinkEntry_t* pNext)
{
    pEntry->next = pNext;
    pEntry->prev = pPrev;
    pNext->prev = pEntry;
    pPrev->next = pEntry;
}

//-----------------------------------------------------------------------------
/**
    Add new entry to next position of specified location

    @param[in]     pEntry      entry pointer to be added
    @param[in]     pLocation   location pointer
**/
//-----------------------------------------------------------------------------
static __inline void DlAddToNext(DlinkEntry_t* pEntry, DlinkEntry_t* pLocation)
{
    DlInsert(pEntry, pLocation, pLocation->next);
}

//-----------------------------------------------------------------------------
/**
    Add new entry to previous position  of specified location

    @param[in]     pEntry      entry pointer to be added
    @param[out]    pLocation   location pointer
**/
//-----------------------------------------------------------------------------
static __inline void DlAddToPrev(DlinkEntry_t* pEntry, DlinkEntry_t* pLocation)
{
    DlInsert(pEntry, pLocation->prev, pLocation);
}

//-----------------------------------------------------------------------------
/**
    Add new entry to head of list

    @param[out]     pList       list pointer
    @param[in]      pEntry      entry pointer to be added
**/
//-----------------------------------------------------------------------------
static __inline void DlAddToHead(LinkHeadTail_t* pList, DlinkEntry_t* pEntry)
{
    DlAddToPrev(pEntry, pList->head);
}

//-----------------------------------------------------------------------------
/**
    Add new entry to tail of list

    @param[out]     pList       list pointer
    @param[in]      pEntry      entry pointer to be added
**/
//-----------------------------------------------------------------------------
static __inline void DlAddToTail(LinkHeadTail_t* pList, DlinkEntry_t* pEntry)
{
    DlAddToNext(pEntry, pList->tail);
}

//-----------------------------------------------------------------------------
/**
    Remove entry from list

    @param[in]      pEntry      entry pointer to be added
**/
//-----------------------------------------------------------------------------
static __inline void DlRemove(DlinkEntry_t* pEntry)
{
    ((DlinkEntry_t *)pEntry->prev)->next = pEntry->next;
    ((DlinkEntry_t *)pEntry->next)->prev = pEntry->prev;
}

//-----------------------------------------------------------------------------
/**
    Pop a entry from head of list

    @param[in]     pList       list pointer
**/
//-----------------------------------------------------------------------------
static __inline void* DlPopFromHead(LinkHeadTail_t* pList)
{
    DlinkEntry_t* pEntry;

    pEntry = (DlinkEntry_t *)pList->head;

    ((DlinkEntry_t *)pEntry->next)->prev = pList;
    pList->head = pEntry->next;

    return pEntry;
}

//-----------------------------------------------------------------------------
/**
    Pop a entry from tail of list

    @param[in]     pList       list pointer
**/
//-----------------------------------------------------------------------------
static __inline void* DlPopFromTail(LinkHeadTail_t* pList)
{
    DlinkEntry_t* pEntry;

    pEntry = (DlinkEntry_t *)pList->tail;

    ((DlinkEntry_t *)pEntry->prev)->next = pList;
    pList->tail = pEntry->prev;

    return pEntry;
}


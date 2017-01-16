// ============================================================================
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
/*
    This computer program contains confidential and proprietary information,
    and may NOT be reproduced or transmitted, in whole or in part, in any form,
    or by any means electronic, mechanical, photo-optical, or otherwise, and
    may NOT be translated into another language without the express written
    permission from LITEON Corporation.
*/
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
//=============================================================================
//
//! @brief source of UNCTable
//=============================================================================
//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "SysInfo.h"
#include "Cache.h"
#include "UncTable.h"

#include "NormSection.h"
//-----------------------------------------------------------------------------
//  Macros:
//-----------------------------------------------------------------------------
#define UNC_TBL_GET_IDX(P)      (P - &pUncTblInfo->node[0])
#define UNC_TBL_GET_PTR(I)      (&pUncTblInfo->node[I])
#define UNC_TBL_GET_INS(I)      pUncTblInfo->node[I]

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static  void            UncTbl_Node_Insert(U32 pool, tUNCTblNode* pNode, U16 prevIdx, U16 nextIdx);
static  tUNCTblNode*    UncTbl_Node_Delete(U32 pool, tUNCTblNode* pNode, U16 prevIdx);
static  tUNCTblNode*    UncTbl_Node_Search(U32 laa, U16* prevIdx, U8 test);
static  void            UncTbl_Node_Location(U32 laa, U16 *prevIdx, U16* nextIdx);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported external variable
//-----------------------------------------------------------------------------
__DATA_PERF_SH tUNCTblInfo*    pUncTblInfo;

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
#if (_CPU_ == CPU_ID0) // CPU0 only
//-----------------------------------------------------------------------------
/**
    ECCTable one time initializtion code

    @param[in]  initMode
**/
//-----------------------------------------------------------------------------
Error_t UNCT_OneTimeInit(InitBootMode_t initMode)
{
    smUncTblFlag = FALSE;

    pUncTblInfo = (tUNCTblInfo*)&smSysInfo->d.Unct.all[0];

    // verification
    if (smSysInfo->d.Header.d.SubHeader.d.UncTable.d.dwValidTag != SI_TAG_UNCTABLE)
    {
        UNCT_Reset();
    }
    else if (pUncTblInfo->brief.count[UNC_TBL_POOL_USED])
    {
        smUncTblFlag = TRUE;
    }

    D_PRINTF("[UNCT] Cnt[%04x|%04x] H/T[%04x|%04x]\n",
            pUncTblInfo->brief.count[UNC_TBL_POOL_USED],
            pUncTblInfo->brief.count[UNC_TBL_POOL_FREE],
            pUncTblInfo->brief.head[UNC_TBL_POOL_FREE],
            pUncTblInfo->brief.tail[UNC_TBL_POOL_FREE]);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Reset ECCTable
**/
//-----------------------------------------------------------------------------
void UNCT_Reset(void)
{
    U32 idx;

    MEM_CLR(pUncTblInfo, sizeof(pUncTblInfo));

    for (idx = 0; idx < UNC_TBL_ENTRY_NUM - 1; idx++)
    {
        pUncTblInfo->node[idx].next = idx + 1;
    }

    pUncTblInfo->node[UNC_TBL_ENTRY_NUM - 1].next = WORD_MASK;

    pUncTblInfo->brief.count[UNC_TBL_POOL_FREE] = UNC_TBL_ENTRY_NUM;
    pUncTblInfo->brief.count[UNC_TBL_POOL_USED] = 0;
    pUncTblInfo->brief.head[UNC_TBL_POOL_FREE]  = 0;
    pUncTblInfo->brief.tail[UNC_TBL_POOL_FREE]  = UNC_TBL_ENTRY_NUM - 1;
    pUncTblInfo->brief.head[UNC_TBL_POOL_USED]  = pUncTblInfo->brief.tail[UNC_TBL_POOL_USED] = WORD_MASK;

    D_PRINTF("[UNCT] Reset:H/T[%04x|%04x]\n",
            pUncTblInfo->brief.head[UNC_TBL_POOL_FREE],
            pUncTblInfo->brief.tail[UNC_TBL_POOL_FREE]);

    // Synchronize
    smSysInfo->d.Header.d.SubHeader.d.UncTable.d.dwValidTag = SI_TAG_UNCTABLE;
    SyIn_Synchronize(SI_AREA_UNCT, SYSINFO_WRITE, SI_SYNC_BY_UNCT);

    smUncTblFlag = FALSE;
}

//-----------------------------------------------------------------------------
/**
    Dump ECCTable information
**/
//-----------------------------------------------------------------------------
void UNCT_Dump(void)
{
    tUNCTblNode* pNode;
    U32 idx;

    D_PRINTF("[UNCT] Dump UNC Table Flag|%d\n", smUncTblFlag);
    D_PRINTF("[UNCT] Head|%4X Tail|%4X UsedCount|%d\n",
            pUncTblInfo->brief.head[UNC_TBL_POOL_USED],
            pUncTblInfo->brief.tail[UNC_TBL_POOL_USED],
            pUncTblInfo->brief.count[UNC_TBL_POOL_USED]);
    D_PRINTF("[UNCT] Head|%4X Tail|%4X FreeCount|%d\n",
        pUncTblInfo->brief.head[UNC_TBL_POOL_FREE],
        pUncTblInfo->brief.tail[UNC_TBL_POOL_FREE],
        pUncTblInfo->brief.count[UNC_TBL_POOL_FREE]);

    pNode = UNC_TBL_GET_PTR(pUncTblInfo->brief.head[UNC_TBL_POOL_USED]);

    for (idx = 0; idx < pUncTblInfo->brief.count[UNC_TBL_POOL_USED]; idx++)
    {
        D_PRINTF("-[UNCT] %4d: Type|%d LAA|%08X BMP|%02X\n", idx, pNode->type, pNode->laa, pNode->bmp);
        pNode = UNC_TBL_GET_PTR(pNode->next);
    }
}

//-----------------------------------------------------------------------------
/**
    Insert ECCTable when uncorreatable error occurred

    @param[in]  lba
    @param[in]  len
    @param[in]  update flag

    @return     UNCT_NO_ERROR if no error
**/
//-----------------------------------------------------------------------------
U32 UNCT_Delete(U64 lba, U64 len, U8 update)
{
    U32 ret = UNCT_NO_ERROR;
    tUNCTblNode* pNode;
    U32 laa_start;
    U32 laa_end;
    U16 currIdx;
    U16 prevIdx;
    U16 bmp;

    BEGIN_MULTI_CS2(cSpinLockUncTable);

    if (pUncTblInfo->brief.count[UNC_TBL_POOL_USED] == 0)  goto delete_exit;

    laa_start = (lba)           >> HBU_PER_MBU_SFT;
    laa_end   = (lba + len - 1) >> HBU_PER_MBU_SFT;

    pNode = UNC_TBL_GET_PTR(pUncTblInfo->brief.head[UNC_TBL_POOL_USED]);
    if (pNode->laa > laa_end)	goto delete_exit;

    pNode = UNC_TBL_GET_PTR(pUncTblInfo->brief.tail[UNC_TBL_POOL_USED]);
    if (pNode->laa < laa_start)	goto delete_exit;

    if (laa_start <  UNC_TBL_GET_PTR(pUncTblInfo->brief.head[UNC_TBL_POOL_USED])->laa &&
        laa_end   >  UNC_TBL_GET_PTR(pUncTblInfo->brief.tail[UNC_TBL_POOL_USED])->laa)
    {
        UNCT_Reset();
        goto delete_exit;
    }

    currIdx = pUncTblInfo->brief.head[UNC_TBL_POOL_USED];
    prevIdx = WORD_MASK;

    while (UNCT_WMASK_CHECK(currIdx))
    {
        pNode = UNC_TBL_GET_PTR(currIdx);

        if (pNode->laa >= laa_start && pNode->laa <= laa_end)
        {
            if (pNode->laa == laa_start)
            {
                if (laa_start == laa_end)   bmp = gSLaaBitMask[lba & HBU_PER_MBU_MASK] & gELaaBitMask[(lba + len - 1) & HBU_PER_MBU_MASK];
                else                        bmp = gSLaaBitMask[lba & HBU_PER_MBU_MASK];
            }
            else if (pNode->laa == laa_end) bmp = gELaaBitMask[(lba + len - 1) & HBU_PER_MBU_MASK];
            else                            bmp = BYTE_MASK;

            pNode->bmp &= ~bmp;

            if (pNode->bmp == 0)
            {
                currIdx = pNode->next;
                D_PRINTF("[UNCT] Del LAA[%08X|%08X]\n", laa_start, laa_end);
                UncTbl_Node_Delete(UNC_TBL_POOL_USED, pNode, prevIdx);
                UncTbl_Node_Insert(UNC_TBL_POOL_FREE, pNode, pUncTblInfo->brief.tail[UNC_TBL_POOL_FREE], WORD_MASK);
            }
            else
            {
                prevIdx = currIdx;
                currIdx = pNode->next;
            }
            ret = UNCT_SYNC_ENABLE;
        }
        else
        {
            prevIdx = currIdx;
            currIdx = pNode->next;
        }
    }

    if (pUncTblInfo->brief.count[UNC_TBL_POOL_USED] == 0) smUncTblFlag = FALSE;

delete_exit:
     END_MULTI_CS2(cSpinLockUncTable);
    // Synchronize into media
    if (update == ENABLE && ret == UNCT_SYNC_ENABLE)
    {
        SyIn_Synchronize(SI_AREA_UNCT, SYSINFO_WRITE, SI_SYNC_BY_UNCT);
    }

    return ret;
}

//-----------------------------------------------------------------------------
/**
    Search ECCTable for host read command

    @param[in]  lba
    @param[in]  len

    @return     TRUE if hit ECCTable
                FALSE if no hit ECCTable
**/
//-----------------------------------------------------------------------------
U32 UNCT_Search(U64 lba, U64 len)
{
    tUNCTblNode* pNode = NULL;
    U32 laa;
    U32 laa_start;
    U32 laa_end;
    U16 prevIdx;
    U16 bmp;

    if (pUncTblInfo->brief.count[UNC_TBL_POOL_USED] == 0) return FALSE;

    laa_start = (lba)           >> HBU_PER_MBU_SFT;
    laa_end   = (lba + len - 1) >> HBU_PER_MBU_SFT;

    pNode = UNC_TBL_GET_PTR(pUncTblInfo->brief.head[UNC_TBL_POOL_USED]);
    if (pNode->laa >laa_end)    return FALSE;

    pNode = UNC_TBL_GET_PTR(pUncTblInfo->brief.tail[UNC_TBL_POOL_USED]);
    if (pNode->laa < laa_start) return FALSE;

    for (laa = laa_start; laa <= laa_end; laa++)
    {
        pNode = UncTbl_Node_Search(laa, &prevIdx, 0);

        if (pNode != NULL)
        {
            if (pNode->laa == laa_start)
            {
                if (laa_start == laa_end)   bmp = gSLaaBitMask[lba & HBU_PER_MBU_MASK] & gELaaBitMask[(lba + len - 1) & HBU_PER_MBU_MASK];
                else                        bmp = gSLaaBitMask[lba & HBU_PER_MBU_MASK];
            }
            else if (pNode->laa == laa_end) bmp = gELaaBitMask[(lba + len - 1) & HBU_PER_MBU_MASK];
            else                            bmp = BYTE_MASK;

            if (pNode->bmp & bmp)
            {
                D_PRINTF("[UNCT] Hit LBA|%x_%x Len|%08x\n", U64_TO_U32_H(lba), U64_TO_U32_L(lba), len);
                return TRUE;
            }
        }
    }

    return FALSE;
}
#endif

#if (_CPU_ == CPU_ID0 || _CPU_ == CPU_ID2)
//-----------------------------------------------------------------------------
/**
    Insert ECCTable when uncorreatable error occurred

    @param[in]  lba
    @param[in]  len

    @return     UNCT_NO_ERROR if no error
                UNCT_ERR_NO_FREE_ENTRY if ECCTable is full
                UNCT_ERR_LBA_OVERFLOW if LBA range overflow
**/
//-----------------------------------------------------------------------------
U32 UNCT_Insert(U64 lba, U64 len, U8 type)
{
    tUNCTblNode* pNode;
    U32 ret = UNCT_NO_ERROR;
    U32 laa;
    U32 laa_start;
    U32 laa_end;
    U16 prevIdx, nextIdx;
    U16 bmp;

    BEGIN_MULTI_CS2(cSpinLockUncTable);

    if (pUncTblInfo->brief.count[UNC_TBL_POOL_USED] == UNC_TBL_ENTRY_NUM)
    {
        ret = UNCT_ERR_NO_FREE_ENTRY;
        goto insert_exit;
    }

    if (lba >= IDEMA_GBYTE2LBU(gNfInfo.userCapacity) || (lba + len) >= IDEMA_GBYTE2LBU(gNfInfo.userCapacity))
    {
        ret = UNCT_ERR_LBA_OVERFLOW;
        goto insert_exit;
    }

    laa_start = lba             >> HBU_PER_MBU_SFT;
    laa_end   = (lba + len - 1) >> HBU_PER_MBU_SFT;

    D_PRINTF("[UNCT] Ins LBA|%x_%x Len|%d\n", U64_TO_U32_H(lba), U64_TO_U32_L(lba), U64_TO_U32_L(len));

    for (laa = laa_start; laa <= laa_end; laa++)
    {
        /*_____________________________________________
             _   _   _             _   _             _
            / \ | |_| |_ ___ _ __ | |_(_) ___  _ __ | |
           / _ \| __| __/ _ \ '_ \| __| |/ _ \| '_ \| |
          / ___ \ |_| ||  __/ | | | |_| | (_) | | | |_|
         /_/   \_\__|\__\___|_| |_|\__|_|\___/|_| |_(_)
        ______________________________________________*/
        // Exclusive system area LBA: 0 ~ 0x1000 (LAA 0~0x200) for better compatibility and reduce RMA
        if (laa <= UNCT_PROTECT_LAA_END) continue;

        pNode = UncTbl_Node_Search(laa, &prevIdx, 1);

        if (laa == laa_start)
        {
            if (laa_start == laa_end)   bmp = gSLaaBitMask[lba & HBU_PER_MBU_MASK] & gELaaBitMask[(lba + len - 1) & HBU_PER_MBU_MASK];
            else                        bmp = gSLaaBitMask[lba & HBU_PER_MBU_MASK];
        }
        else if (laa == laa_end)        bmp = gELaaBitMask[(lba + len - 1) & HBU_PER_MBU_MASK];
        else                            bmp = BYTE_MASK;

        if (pNode == NULL)
        {
            if (pUncTblInfo->brief.count[UNC_TBL_POOL_FREE])
            {
                pNode = UncTbl_Node_Delete(UNC_TBL_POOL_FREE, UNC_TBL_GET_PTR(pUncTblInfo->brief.head[UNC_TBL_POOL_FREE]), WORD_MASK);
            }
            else
            {
                ret = UNCT_ERR_NO_FREE_ENTRY;
                break;
            }

            pNode->type |= type;
            pNode->bmp  |= bmp;
            pNode->laa  =  laa;
            UncTbl_Node_Location(laa, &prevIdx, &nextIdx);
            UncTbl_Node_Insert(UNC_TBL_POOL_USED, pNode, prevIdx, nextIdx);
        }
        else
        {
            pNode->bmp |= bmp;
        }
    }

    if (pUncTblInfo->brief.count[UNC_TBL_POOL_USED]) smUncTblFlag = TRUE;

insert_exit:
    END_MULTI_CS2(cSpinLockUncTable);
    // Synchronize into media
    SyIn_Synchronize(SI_AREA_UNCT, SYSINFO_WRITE, SI_SYNC_BY_UNCT);
    return ret;
}

//-----------------------------------------------------------------------------
/**
    ECCTable node operation for insertion

    @param[in]  pool
    @param[in]  pNode       current node
    @param[in]  prevIdx     previous node index
    @param[in]  nextIdx     next node index
**/
//-----------------------------------------------------------------------------
static void UncTbl_Node_Insert(U32 pool, tUNCTblNode* pNode, U16 prevIdx, U16 nextIdx)
{
    if (!UNCT_WMASK_CHECK(prevIdx))   pUncTblInfo->brief.head[pool] = UNC_TBL_GET_IDX(pNode);
    else                              UNC_TBL_GET_INS(prevIdx).next = UNC_TBL_GET_IDX(pNode);


    if (!UNCT_WMASK_CHECK(nextIdx))   pUncTblInfo->brief.tail[pool] = UNC_TBL_GET_IDX(pNode);
    else                              pNode->next                   = nextIdx;

    pUncTblInfo->brief.count[pool]++;
}

//-----------------------------------------------------------------------------
/**
    ECCTable node operation for deletion

    @param[in]  pool
    @param[in]  pNode       current node
    @param[in]  prevIdx     previous node index

    @return     pNode       current node
**/
//-----------------------------------------------------------------------------
static tUNCTblNode* UncTbl_Node_Delete(U32 pool, tUNCTblNode* pNode, U16 prevIdx)
{
    if (!UNCT_WMASK_CHECK(prevIdx))       pUncTblInfo->brief.head[pool] = pNode->next;
    else                                  UNC_TBL_GET_INS(prevIdx).next = pNode->next;

    if (!UNCT_WMASK_CHECK(pNode->next))   pUncTblInfo->brief.tail[pool] = prevIdx;

    pNode->type = UNCT_NDTP_INIT;
    pNode->next = WORD_MASK;
    pNode->laa  = DWORD_MASK;
    pNode->bmp  = 0;
    pUncTblInfo->brief.count[pool]--;

    return pNode;
}

//-----------------------------------------------------------------------------
/**
    ECCTable node operation for searching

    @param[in]  laa
    @param[in]  prevIdx     previous node index

    @return     pNode       current node
**/
//-----------------------------------------------------------------------------
static tUNCTblNode* UncTbl_Node_Search(U32 laa, U16* prevIdx, U8 test)
{
    tUNCTblNode* pNode = NULL;
    U16 index;

    *prevIdx = WORD_MASK;
    index    = pUncTblInfo->brief.head[UNC_TBL_POOL_USED];

    while (UNCT_WMASK_CHECK(index))
    {
        pNode = UNC_TBL_GET_PTR(index);
        if (pNode->laa == laa) return pNode;
        *prevIdx = index;
        index    = pNode->next;
    }
    return NULL;
}

//-----------------------------------------------------------------------------
/**
    ECCTable node operation for finding previous index

    @param[in]  laa
    @param[in]  prevIdx     pointer of previous node index
    @param[in]  nextIdx     next node index
**/
//-----------------------------------------------------------------------------
static void UncTbl_Node_Location(U32 laa, U16 *prevIdx, U16* nextIdx)
{
    tUNCTblNode* pNode = NULL;
    U16 curr, prev;

    prev = WORD_MASK;
    curr = pUncTblInfo->brief.head[UNC_TBL_POOL_USED];

    while (UNCT_WMASK_CHECK(curr))
    {
        pNode = UNC_TBL_GET_PTR(curr);
        if (pNode->laa > laa)
        {
            break;
        }

        prev = curr;
        curr = pNode->next;
    }

    *prevIdx = prev;
    *nextIdx = curr;
}
#endif

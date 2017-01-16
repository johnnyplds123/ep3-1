#pragma once
//-----------------------------------------------------------------------------
//  include header files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define HEAP_CPU0_SIZE              0
#define HEAP_CPU1_SIZE              4096
#define HEAP_CPU2_SIZE              0

#define HEAP_ENTRY_SIZE             4
#define HEAP_ENTRY_COUNT            (HEAP_CPU1_SIZE / HEAP_ENTRY_SIZE)
#define HEAP_MASK                   (HEAP_ENTRY_COUNT - 1)

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define HEAP_GET_PTR_CPU1_B0TCM(P)  ((PVOID)((U32)P | AXI_B0TCM_CPU1_ADDR))
#define HEAP_GET_IDX_FROM_PTR(P)    ((U32*)P - smHeapB0TcmCpu1)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct
{
    U16 busy  :  1;
    U16 count : 15;
}tHeapNode;

typedef struct
{
    U16 ptr;
    U16 freeCnt;
}tHeapInfo;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern tHeapInfo gHeapInfo;
extern tHeapNode gHeapList[HEAP_ENTRY_COUNT];

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
extern Error_t Heap_OneTimeInit(InitBootMode_t initMode);

#if _CPU_==CPU_ID2
    extern void    Heap_Dump(void);
#endif

//-----------------------------------------------------------------------------
//  Inline function
//-----------------------------------------------------------------------------
#if   _CPU_==CPU_ID2    // CPU2 only
//-----------------------------------------------------------------------------
/*
  Description : allocte heap memory slice
  Input       : count : Requested heap count
  return      : return a valid heap index if succeed,
|               return WORD_MASK if there is no satisfied heap
*/
//-----------------------------------------------------------------------------
__inline PVOID Heap_Alloc(U16 count)
{
    U16 index;

    count = (count + HEAP_ENTRY_SIZE - 1) / HEAP_ENTRY_SIZE;

    if (gHeapInfo.freeCnt < count)
    {
        index = gHeapInfo.ptr + gHeapInfo.freeCnt;
        /* wrap pointer since there is no enough free count */
        if (index == HEAP_ENTRY_COUNT)
        {
            index = gHeapInfo.ptr = gHeapInfo.freeCnt = 0;
        }

        // defragmentation: merge multiple heap slices
        do {
            if (gHeapList[index].busy)
            {
                if (gHeapInfo.freeCnt < count)
                {
                    // seek to next heap slice
                    gHeapInfo.ptr = (index + gHeapList[index].count) & HEAP_MASK;
                    gHeapInfo.freeCnt = 0;
                    // TODO:: just break this while loop or look ahead???
                    // break;
                }
                break;
            }
            else
            {
                gHeapInfo.freeCnt             += gHeapList[index].count;
                gHeapList[gHeapInfo.ptr].count = gHeapInfo.freeCnt;
            }
            index += gHeapList[index].count;
        } while(index < HEAP_ENTRY_COUNT);

    }

    if (gHeapInfo.freeCnt >= count)
    {
        index                  = gHeapInfo.ptr;
        gHeapList[index].busy  = true;
        gHeapList[index].count = count;

        gHeapInfo.ptr      = (gHeapInfo.ptr + count) & HEAP_MASK;
        gHeapInfo.freeCnt -= count;

        /* Don't update next heap slice if there is no freeCnt */
        if (gHeapInfo.freeCnt)
        {
            gHeapList[gHeapInfo.ptr].count = gHeapInfo.freeCnt;
        }
        return (PVOID)&smHeapB0TcmCpu1[index * HEAP_ENTRY_SIZE];
    }
    return NULL;
}

//-----------------------------------------------------------------------------
/*
  Description : free heap memory slice
  Input       : HeapAddr: heap memeory address
  return      : N/A
*/
//-----------------------------------------------------------------------------
static __inline void Heap_Free(PVOID HeapAddr)
{
    gHeapList[HEAP_GET_IDX_FROM_PTR(HeapAddr) / HEAP_ENTRY_SIZE].busy = false;
}
#endif



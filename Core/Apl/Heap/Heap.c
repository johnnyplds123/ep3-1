//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "Heap.h"

#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
#if   _CPU_==CPU_ID1
    U32 gB0TcmCpu1[HEAP_CPU1_SIZE];
#elif _CPU_==CPU_ID2
    tHeapInfo gHeapInfo;
    tHeapNode gHeapList[HEAP_ENTRY_COUNT];
#endif

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
  Description : heap memeory management one time inital function
  Input       : initMode: initial mode
  return      : error code
*/
//-----------------------------------------------------------------------------
Error_t Heap_OneTimeInit(InitBootMode_t initMode)
{
    #if _CPU_==CPU_ID1    // CPU1 only
        smHeapB0TcmCpu1 = gB0TcmCpu1;
    #elif _CPU_==CPU_ID2  // CPU2 only
        MEM_CLR((void*)&gHeapInfo, sizeof(gHeapInfo));
        MEM_CLR((void*)gHeapList, sizeof(gHeapList));
        gHeapInfo.ptr      = 0;
        gHeapInfo.freeCnt  = HEAP_ENTRY_COUNT;
        gHeapList[0].count = HEAP_ENTRY_COUNT;
    #endif

    return cEcNoError;
}

#if _CPU_==CPU_ID2
//-----------------------------------------------------------------------------
/*
  Description : dump heap memory management information
  Input       : N/A
  return      : N/A
*/
//-----------------------------------------------------------------------------
void Heap_Dump(void)
{
    U16 curr, head;

    char str[2] = {' ', '*'};
    D_PRINTF("Heap Head[%4d] Count[%4d]\n", gHeapInfo.ptr, gHeapInfo.freeCnt);
    head = curr = gHeapInfo.ptr & HEAP_MASK;

    do{
        D_PRINTF("  Heap Start[%4d] Count[%4d] Busy[ %c ]\n", curr, gHeapList[curr].count, str[gHeapList[curr].busy]);
        curr = (curr + gHeapList[curr].count) & HEAP_MASK;
    } while (curr != head);
}
#endif



#pragma once
//-----------------------------------------------------------------------------
//  include header files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
enum
{
    SEQ_NO_WAIT = 0,
    SEQ_TIMEOUT_WAIT
};

#define SEQ_TIMEOUT_VALUE       1000000

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct{
    volatile U32 trkPtrW;
    volatile U32 trkPtrR;
    volatile U32 trkIssueCnt;
    volatile U32 trkDoneCnt;
    U16          trkIdArray[MAX_TRK_NUM + 1];
    U16          rsvd;
}AplSeqInfo_t;
COMPILE_ASSERT((sizeof(AplSeqInfo_t) & 0x3) == 0, "please align to 4 bytes!");
// === Track ID array description =================
//-------------------------------------------------
// R/W Pointer:   R                             W
// Array Index:  [0] [1] [2] [3] [4]           [N]
// Track ID   : | 3 | 1 | 2 | 0 | 9 | ....... | N |
//              <--------- N + 1 Track ID -------->
//-------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern AplSeqInfo_t gAplSeqInfo;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t  AplSeq_OneTimeInit(InitBootMode_t initMode);
Error_t  AplSeq_DumpInfo(void);

//-----------------------------------------------------------------------------
//  Inline function reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/*
  Description: Check if sequncer have available free track ount
  Input      : N/A
  return     : true or false
*/
//-----------------------------------------------------------------------------
__inline BOOL AplSeq_HasFreeTrk(void)
{
    return gAplSeqInfo.trkPtrR != gAplSeqInfo.trkPtrW;
}

//-----------------------------------------------------------------------------
/*
  Description: Check if sequncer track is empty
  Input      : N/A
  return     : true or false
*/
//-----------------------------------------------------------------------------
__inline BOOL AplSeq_IsTrkEmpty(void)
{
    return gAplSeqInfo.trkPtrR == gAplSeqInfo.trkPtrW;
}


//-----------------------------------------------------------------------------
/*
  Description: Allocate track id
  return     : true or false
*/
//-----------------------------------------------------------------------------
__inline U16 AplSeq_AllocTrkId(U16 timeout)
{
    U16 trkId;
    if (timeout == SEQ_TIMEOUT_WAIT)
    {
        U32 cnt = 0;
        while (gAplSeqInfo.trkPtrR == gAplSeqInfo.trkPtrW && cnt++ < SEQ_TIMEOUT_VALUE)
        {
            cnt++;
            HalTimer_DelayUsec(10);
        }

        ASSERT(cnt < SEQ_TIMEOUT_VALUE);
    }

    trkId = gAplSeqInfo.trkIdArray[gAplSeqInfo.trkPtrR];
    if (++gAplSeqInfo.trkPtrR > MAX_TRK_NUM) gAplSeqInfo.trkPtrR = 0;

    return trkId;
}

//-------------------------------------------------------------------
// Description  : Release track id
// Input        : track id
// return       : N/A
//-------------------------------------------------------------------
__inline void AplSeq_FreeTrkId(U16 trkId)
{
    gAplSeqInfo.trkIdArray[gAplSeqInfo.trkPtrW] = trkId;
    if (++gAplSeqInfo.trkPtrW > MAX_TRK_NUM) gAplSeqInfo.trkPtrW = 0;

	if (gAplSeqInfo.trkPtrW == gAplSeqInfo.trkPtrR)
	{
        ASSERT(gAplSeqInfo.trkPtrW != gAplSeqInfo.trkPtrR);
	}
}

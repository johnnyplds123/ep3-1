#pragma once
#include <stddef.h>
//-----------------------------------------------------------------------------
//  MRE AES Registers structure definitions
//-----------------------------------------------------------------------------
typedef union
{
    uint32_t all;                              ///< All bits.
    struct
    {
        uint32_t MRE_IP_MODE          : 1;     ///<BIT [0]      Channel IP Mode to Select the Data Path.
        uint32_t RA0003800_1_RSVD     : 1;     ///<BIT [1]      Reserved.
        uint32_t MRE_BLK_MODE         : 2;     ///<BIT [3:2]    Channel AES Mode Selection.
        uint32_t MRE_KEY_LEN          : 2;     ///<BIT [5:4]    Channel Key Length.
        uint32_t RA0003800_7_6_RSVD   : 2;     ///<BIT [7:6]    Reserved.
        uint32_t MRE_IV_MASK          : 3;     ///<BIT [10:8]   Channel AES IV Mask
        uint32_t RA0003800_11_RSVD    : 1;     ///<BIT [11]     Reserved.
        uint32_t MRE_IV_MODE          : 2;     ///<BIT [13:12]  Channel Initialization Vector (IV) Table.
        uint32_t RA0003800_31_14_RSVD :18;     ///<BIT [31:14]  Reserved.
    } bits;
} tMRE_CFG;


typedef union
{
    uint32_t all;                                                                                                                                                                                                                                                                      ///< All bits.
    struct
    {
        uint32_t MRE_RESET           : 1;         ///<BIT [0]    Channel MRE Reset.
        uint32_t RA0003804_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } bits;
} tMRE_CTRL;


typedef union
{
    uint32_t all;                                                                                                                                                                                                                                                                      ///< All bits.
    struct
    {
        uint32_t MRE_BUSY             : 1;         ///<BIT [0]     Channel MRE Busy.
        uint32_t MRE_CORE_STATE       : 5;         ///<BIT [5:1]   Channel Core State.
        uint32_t RA000380C_7_6_RSVD   : 2;         ///<BIT [7:6]   Reserved.
        uint32_t MRE_TOP_STATE        : 2;         ///<BIT [9:8]   Channel Top State.
        uint32_t RA000380C_31_10_RSVD :22;         ///<BIT [31:10] Reserved.
    } bits;
} tMRE_STAT;



/// @brief 0xA0003800-0xA000390F : mtNV_MRE MODULE
typedef struct
{
    tMRE_CFG RC_MRE_CFG;                      ///< 0xA0003800 : Read Channel MRE Configuration
    tMRE_CTRL RC_MRE_CTRL;                    ///< 0xA0003804 : Read Channel MRE Control
    uint32_t rsvd_RA0003808;                  ///< 0xA0003808 - 0xA000380B : Reserved.
    tMRE_STAT RC_MRE_STS;                     ///< 0xA000380C : Read Channel MRE Status
    uint32_t RC_MRE_STREAM_SIZE;              ///< 0xA0003810 : Read Channel MRE Stream Size
    uint32_t RC_MRE_SKIP_SIZE;                ///< 0xA0003814 : Read Channel MRE Skip Size
    uint32_t rsvd_RA0003818[2];               ///< 0xA0003818 - 0xA000381F : Reserved.
    uint32_t RC_MRE_KEY2_WORD[8];               ///< 0xA0003820 : Read Channel MRE Key 2 Word 0
//    uint32_t RC_MRE_KEY2_WORD0;               ///< 0xA0003820 : Read Channel MRE Key 2 Word 0
//    uint32_t RC_MRE_KEY2_WORD1;               ///< 0xA0003824 : Read Channel MRE Key 2 Word 1
//    uint32_t RC_MRE_KEY2_WORD2;               ///< 0xA0003828 : Read Channel MRE Key 2 Word 2
//    uint32_t RC_MRE_KEY2_WORD3;               ///< 0xA000382C : Read Channel MRE Key 2 Word 3
//    uint32_t RC_MRE_KEY2_WORD4;               ///< 0xA0003830 : Read Channel MRE Key 2 Word 4
//    uint32_t RC_MRE_KEY2_WORD5;               ///< 0xA0003834 : Read Channel MRE Key 2 Word 5
//    uint32_t RC_MRE_KEY2_WORD6;               ///< 0xA0003838 : Read Channel MRE Key 2 Word 6
//    uint32_t RC_MRE_KEY2_WORD7;               ///< 0xA000383C : Read Channel MRE Key 2 Word 7
    uint32_t RC_MRE_KEY1_WORD[8];               ///< 0xA0003840 : Read Channel MRE Key 1 Word 0
//    uint32_t RC_MRE_KEY1_WORD0;               ///< 0xA0003840 : Read Channel MRE Key 1 Word 0
//    uint32_t RC_MRE_KEY1_WORD1;               ///< 0xA0003844 : Read Channel MRE Key 1 Word 1
//    uint32_t RC_MRE_KEY1_WORD2;               ///< 0xA0003848 : Read Channel MRE Key 1 Word 2
//    uint32_t RC_MRE_KEY1_WORD3;               ///< 0xA000384C : Read Channel MRE Key 1 Word 3
//    uint32_t RC_MRE_KEY1_WORD4;               ///< 0xA0003850 : Read Channel MRE Key 1 Word 4
//    uint32_t RC_MRE_KEY1_WORD5;               ///< 0xA0003854 : Read Channel MRE Key 1 Word 5
//    uint32_t RC_MRE_KEY1_WORD6;               ///< 0xA0003858 : Read Channel MRE Key 1 Word 6
//    uint32_t RC_MRE_KEY1_WORD7;               ///< 0xA000385C : Read Channel MRE Key 1 Word 7
    uint32_t RC_MRE_IV_WORD[4];                 ///< 0xA0003860 : Read Channel MRE IV Word 0
//    uint32_t RC_MRE_IV_WORD0;                 ///< 0xA0003860 : Read Channel MRE IV Word 0
//    uint32_t RC_MRE_IV_WORD1;                 ///< 0xA0003864 : Read Channel MRE IV Word 1
//    uint32_t RC_MRE_IV_WORD2;                 ///< 0xA0003868 : Read Channel MRE IV Word 2
//    uint32_t RC_MRE_IV_WORD3;                 ///< 0xA000386C : Read Channel MRE IV Word 3
    uint32_t rsvd_RA0003870[12];              ///< 0xA0003870 - 0xA000389F : Reserved.
    tMRE_CFG WC_MRE_CFG;                      ///< 0xA00038A0 : Write Channel MRE Configuration
    tMRE_CTRL WC_MRE_CTRL;                    ///< 0xA00038A4 : Write Channel MRE Control
    uint32_t rsvd_RA00038A8;                  ///< 0xA00038A8 - 0xA00038AB : Reserved.
    tMRE_STAT WC_MRE_STS;                     ///< 0xA00038AC : Write Channel MRE Status
    uint32_t WC_MRE_STREAM_SIZE;              ///< 0xA00038B0 : Write Channel MRE Stream Size
    uint32_t WC_MRE_SKIP_SIZE;                ///< 0xA00038B4 : Write Channel MRE Skip Size
    uint32_t rsvd_RA00038B8[2];               ///< 0xA00038B8 - 0xA00038BF : Reserved.
    uint32_t WC_MRE_KEY2_WORD[8];               ///< 0xA00038C0 : Write Channel MRE Key 2 Word 0
/* 
    uint32_t WC_MRE_KEY2_WORD0;               ///< 0xA00038C0 : Write Channel MRE Key 2 Word 0
    uint32_t WC_MRE_KEY2_WORD1;               ///< 0xA00038C4 : Write Channel MRE Key 2 Word 1
    uint32_t WC_MRE_KEY2_WORD2;               ///< 0xA00038C8 : Write Channel MRE Key 2 Word 2
    uint32_t WC_MRE_KEY2_WORD3;               ///< 0xA00038CC : Write Channel MRE Key 2 Word 3
    uint32_t WC_MRE_KEY2_WORD4;               ///< 0xA00038D0 : Write Channel MRE Key 2 Word 4
    uint32_t WC_MRE_KEY2_WORD5;               ///< 0xA00038D4 : Write Channel MRE Key 2 Word 5
    uint32_t WC_MRE_KEY2_WORD6;               ///< 0xA00038D8 : Write Channel MRE Key 2 Word 6
    uint32_t WC_MRE_KEY2_WORD7;               ///< 0xA00038DC : Write Channel MRE Key 2 Word 7
*/
    uint32_t WC_MRE_KEY1_WORD[8];               ///< 0xA00038E0 : Write Channel MRE Key 1 Word 0
/*
    uint32_t WC_MRE_KEY1_WORD0;               ///< 0xA00038E0 : Write Channel MRE Key 1 Word 0
    uint32_t WC_MRE_KEY1_WORD1;               ///< 0xA00038E4 : Write Channel MRE Key 1 Word 1
    uint32_t WC_MRE_KEY1_WORD2;               ///< 0xA00038E8 : Write Channel MRE Key 1 Word 2
    uint32_t WC_MRE_KEY1_WORD3;               ///< 0xA00038EC : Write Channel MRE Key 1 Word 3
    uint32_t WC_MRE_KEY1_WORD4;               ///< 0xA00038F0 : Write Channel MRE Key 1 Word 4
    uint32_t WC_MRE_KEY1_WORD5;               ///< 0xA00038F4 : Write Channel MRE Key 1 Word 5
    uint32_t WC_MRE_KEY1_WORD6;               ///< 0xA00038F8 : Write Channel MRE Key 1 Word 6
    uint32_t WC_MRE_KEY1_WORD7;               ///< 0xA00038FC : Write Channel MRE Key 1 Word 7
*/   
    uint32_t WC_MRE_IV_WORD[4];                 ///< 0xA0003900 : Write Channel MRE IV Word 0
/* 
    uint32_t WC_MRE_IV_WORD0;                 ///< 0xA0003900 : Write Channel MRE IV Word 0
    uint32_t WC_MRE_IV_WORD1;                 ///< 0xA0003904 : Write Channel MRE IV Word 1
    uint32_t WC_MRE_IV_WORD2;                 ///< 0xA0003908 : Write Channel MRE IV Word 2
    uint32_t WC_MRE_IV_WORD3;                 ///< 0xA000390C : Write Channel MRE IV Word 3
*/
} MRE_t;

COMPILE_ASSERT(offsetof(MRE_t, WC_MRE_IV_WORD[3])==0xA000390C-0xA0003800,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile MRE_t rMRE;                        ///< 0xA0003800-0xA000390F : MRE_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
extern MRE_t mMRE;                        ///< 0xA0003800-0xA000390F : MRE_t MODULE


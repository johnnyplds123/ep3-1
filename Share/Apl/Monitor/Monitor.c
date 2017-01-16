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
//! @brief Serial monitor code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "Monitor.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define MON_CMD_BUFFER_SIZE     80      ///< size of command string buffer
#define MON_CMD_MAX_TABLE_LIST  16      ///< maximum number of command table list
#define MON_CMD_PROMPT_LENGTH    4      ///< maximum number of command table list

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static       Error_t        Mon_OneTimeInit(InitBootMode_t initMode);
static const MonCmdTbl_t*   Mon_DecodeCmd(const MonCmdTbl_t* pTable, Cstr_t pCmdStr, Cstr_t* pArgStr);
static       Error_t        Mon_DecodeArg(Cstr_t pArgStr, uint32_t maxArgc, uint32_t* pArgc, uint32_t* pArgv);
static       Error_t        Mon_ExecuteCmd(Cstr_t pCmdStr);
static       void           Mon_ServiceLoop(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
static          char         gCmdBuffer[MON_CMD_BUFFER_SIZE];    ///< command string buffer
static          uint32_t     gCmdBufferIndex;                    ///< command index
static volatile bool         gCmdPending;                        ///< command pending flag
static          U32          gCmdLinePtr;                           ///< command Line Offset
static const    MonCmdTbl_t* gCmdTableList[MON_CMD_MAX_TABLE_LIST];
static const    char         strMonPrompt[] = "MON>";
                void*        gMonBuffer;                         ///< generic monitor buffer


//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern const MonCmdTbl_t monCmdList[];
extern const MonCmdTbl_t monMediaCmdList[];
extern const MonCmdTbl_t monCoreCmdList[];

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    One-timie initialization of monitor component.\n
     - bind monitor command table.\n

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
static Error_t Mon_OneTimeInit(InitBootMode_t initMode)
{
    MEM_CLR(gCmdTableList, sizeof(gCmdTableList));
    Mon_RegisterCmdList(monCmdList);

    gMonBuffer = MEM_AllocBuffer(MONBUFFER_SIZE, 512);

#if (_MEDIA_==MEDIA_NAND)
    Mon_RegisterCmdList(monMediaCmdList);
    Mon_RegisterCmdList(monCoreCmdList);
#endif

    gCmdBufferIndex = 0;
    gCmdPending     = FALSE;
    gCmdLinePtr     = MON_CMD_PROMPT_LENGTH;

    HalIrq_RegisterHandler(cIrqUartRx, Mon_UartRxIsr, ENABLE);

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Register monitor commands.

    @param[in]  pCmdTable    command table pointer to register

    @return     error code  (cEcError if command registration failed)
**/
//-----------------------------------------------------------------------------
Error_t Mon_RegisterCmdList(const MonCmdTbl_t* pCmdTable)
{
    uint32_t i;

    for (i = 0; i < MON_CMD_MAX_TABLE_LIST; i++)
    {
        if (gCmdTableList[i] == NULL)
        {
            gCmdTableList[i] = pCmdTable;
            return (cEcNoError);
        }
    }

    return (cEcError);   // can't register because of list full.
}

//-----------------------------------------------------------------------------
/**
    Print a monitor prompt "MON>"
**/
//-----------------------------------------------------------------------------
void Mon_PrintPrompt(void)
{
    M_PRINTF(strMonPrompt);
    gCmdLinePtr = MON_CMD_PROMPT_LENGTH;
}

//-----------------------------------------------------------------------------
/**
    Execute a monitor command\n

    @param[in]  pCmdStr     command string buffer pointer

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t Mon_ExecuteCmd(Cstr_t pCmdStr)
{
    uint32_t i;
    uint32_t argc = 0;
    uint32_t argv[16];
    uint32_t exeTimeUsec;
    Error_t error;
    HwTimerTick_t lastTick;
    Cstr_t pArgStr = NULL;
    const MonCmdTbl_t* pTable;

    // remove space
    pCmdStr = SkipChar(pCmdStr, ' ');
    if (*pCmdStr == ASC_NULL) return (cEcNoError);

    // search command string in all registered command tables
    for (i = 0; i < MON_CMD_MAX_TABLE_LIST; i++)
    {
        pTable = Mon_DecodeCmd(gCmdTableList[i], pCmdStr, &pArgStr);
        // find command
        if (pTable != NULL) break;
    }

    if (pTable == NULL) return (cEcInvalidCommand);

    // convert argument string to number if auto decoding option is enabled.
    if (pTable->autoDec)
    {
        MEM_CLR(argv, sizeof(argv));

        Mon_DecodeArg(pArgStr, sizeof(argv)/sizeof(uint32_t), &argc, argv);

        if (argc < pTable->minArgc) return (cEcMissingParam);
    }

    // read timer tick to dump command execution time later
    lastTick = HalTimer_ReadTick();

    error = pTable->pFunc(pCmdStr, argc, argv);

    exeTimeUsec = HalTimer_GetElapsedTimeUsec(lastTick);

    if (pTable->exeTimePrint)
    {
        M_PRINTF("[HMON] ExeTime|%uUs\n", exeTimeUsec);
    }

    return error;
}

//-----------------------------------------------------------------------------
/**
    Decode a command and return decoding table pointer.

    @param[in]  pTable      command decoding table pointer
    @param[in]  pCmdStr     command line string pointer to decode
    @param[out] pArgStr     argument list start pointer in command line string buffer

    @return     NULL if not supported command, else command decoding table pointer
**/
//-----------------------------------------------------------------------------
static const MonCmdTbl_t* Mon_DecodeCmd(const MonCmdTbl_t* pTable, Cstr_t pCmdStr, Cstr_t* pArgStr)
{
    // compare command string in command table
    while (pTable != NULL && pTable->pFunc != NULL)
    {
        if ((*pArgStr = StrIcmpMove(pCmdStr, pTable->cmdStr)) != NULL)
        {
            if (*pArgStr[0]==ASC_NULL || *pArgStr[0]==' ') return(pTable);
        }
        pTable++;
    }

    return(NULL);
}

//-----------------------------------------------------------------------------
/**
    Decode arguments - convert string to numbers with space separator.

    @param[in]  pArgStr   pointer to argument string
    @param[in]  maxArgc   maximum number of arguments
    @param[out] pArgc     number of decoded arguments
    @param[out] pArgv     argument value list pointer

    @return   error code  (cEcError can't conver to the number because of non-hexa character.)
**/
//-----------------------------------------------------------------------------
static Error_t Mon_DecodeArg(Cstr_t pArgStr, uint32_t maxArgc, uint32_t* pArgc, uint32_t* pArgv)
{
    *pArgc = 0;

    while (*pArgc < maxArgc)
    {
        pArgStr = SkipChar(pArgStr, ' ');

        if (*pArgStr == ASC_NULL)    break;
        if (!IS_HEX_DIGIT(*pArgStr)) return (cEcError);

        pArgStr = StrToNum(pArgStr, pArgv);

        pArgv++;
        (*pArgc)++;
    }

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Print list of commands and usage.
**/
//-----------------------------------------------------------------------------
void Mon_PrintCmdList(void)
{
    uint32_t i;
    const MonCmdTbl_t* pTable;

    for (i = 0; i < MON_CMD_MAX_TABLE_LIST; i++)
    {
        pTable = gCmdTableList[i];

        while (pTable != NULL && pTable->pFunc != NULL)
        {
            M_PRINTF("%12s %s\n", pTable->cmdStr, pTable->helpStr);
            pTable++;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    UART data receive interrupt service routine.\n
        Put user input character to command line buffer and echo back.\n
        Don't get character from Rx FIFO if command is pending.\n

    @param[in]  reserved   reserved (shall be 0)
**/
//-----------------------------------------------------------------------------
void Mon_UartRxIsr(uint32_t reserved)
{
    uint8_t ch;

    gPmInfo.bypassTimerValue = 10 * KERNEL_TICK_PER_SEC;

    while (!gCmdPending && HalUart_TestRxReady(0))
    {
        ch = HalUart_Receive(0);

        PutChar(ch);    //echo back

        switch(ch)
        {
        case ASC_BACKSPACE:
            PutChar(0x20);
            PutChar(ASC_BACKSPACE);
            gCmdLinePtr--;
            if (gCmdLinePtr == (MON_CMD_PROMPT_LENGTH - 1))
            {
                PutChar(strMonPrompt[MON_CMD_PROMPT_LENGTH - 1]);
                gCmdLinePtr++;
            }

            if (gCmdBufferIndex > 0) gCmdBufferIndex--;
            break;
        case ASC_RETURN:
            PutChar(ASC_LINEFEED);
            gCmdBuffer[gCmdBufferIndex++] = ASC_NULL;
            HalIrq_EnDisable(cIrqUartRx, DISABLE);    // disable RX interrupt during gCmdPending is true, or RX interrupt happens forever.
            gCmdPending = TRUE;
            gCmdBufferIndex = 0;
            Kernel_SetEventFlags(EVENT_OBJ_MONITOR, EVENT_MONITOR_CMD_IN);
            break;
        default:
            gCmdLinePtr++;
            if (gCmdBufferIndex < MON_CMD_BUFFER_SIZE-1) gCmdBuffer[gCmdBufferIndex++] = ch;
            break;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Start monitor task up.
**/
//-----------------------------------------------------------------------------
void Mon_StartTask(void)
{
    InitBootMode_t initMode;

    initMode = Hal_GetInitBootMode();

    Mon_OneTimeInit(initMode);

    Mon_ServiceLoop();
}

//-----------------------------------------------------------------------------
/**
    Monitor command serivce loop.
**/
//-----------------------------------------------------------------------------
static void Mon_ServiceLoop(void)
{
    Error_t errCode;

    D_PRINTF("[HMON] MonTask\n");

    while (TRUE)
    {
        //Mon_PrintPrompt();
        Kernel_WaitEventFlags(EVENT_OBJ_MONITOR, EVENT_MONITOR_CMD_IN, KERNEL_NO_TIMEOUT);
        errCode = Mon_ExecuteCmd(gCmdBuffer);
        gCmdPending = FALSE;
        HalIrq_EnDisable(cIrqUartRx, ENABLE);   // enable RX interrupt

        if (errCode != cEcNoError) M_PRINTF("\n[HMON] E|%x\n", errCode);

        Mon_PrintPrompt();
    }
}

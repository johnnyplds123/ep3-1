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

//-----------------------------------------------------------------------------
//  This is PLP.c
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#if _CPU_==CPU_ID0
    #include "RegAlwaysOn.h"
    #include "HalTimer.h"
#elif _CPU_==CPU_ID2
    #include "RegApb.h"
    #include "HalI2c.h"
    #include "HalTimer.h"
#endif

#include "PLP.h"

#include "FastSection.h"
//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void Plp_IrqEnable(void);
static void Plp_IrqHandler(U32 param);

static void Plp_InitI2cVars(void);
static void Plp_SetI2cChargePower(void);
static U16  Plp_GetI2cCapacitance(void);
static U16  Plp_GetI2cCapacitanceDebug(void);

static U32  Plp_GetIoCapacitance(U32 mode, U32 parameter);
static void Plp_SetIoCapMode(U8 mode);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
#if (PLP_CAPACITANCE_FROM_I2C == TRUE)
static  I2cDevice_t gCapacitanceI2cDevice;
static  U8          gCapacitanceI2cReadData;
static  U8          gCapacitanceI2cWritedata[2];
#endif

//-----------------------------------------------------------------------------
//  Imported variable
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
#if (CO_SUPPORT_PLP == TRUE)

#if (_CPU_ == CPU_ID2)
//-----------------------------------------------------------------------------
/**
    Plp_IrqEnable
    GPIO/Test pins Irq Enable
**/
//-----------------------------------------------------------------------------
static void Plp_IrqEnable(void)
{
    rApb.testpinIn.b.TESTPIN11_INT_CLR = 1; ///<BIT [23] PIN_TEST[11] Interrupt Clear.

    HalIrq_RegisterHandler(cIrqPLP, Plp_IrqHandler, ENABLE);

    rApb.testpinIn.b.TESTPIN11_INT_CLR = 0; ///<BIT [23] PIN_TEST[11] Interrupt Clear.
    rApb.testpinIn.b.TESTPIN11_EDG_SEL = 1; ///<BIT [22] PIN_TEST[11] Interrupt Edge Select. Falling
}

//-----------------------------------------------------------------------------
/**
    PLP interrupt service rotuine.\n
**/
//-----------------------------------------------------------------------------
static void Plp_IrqHandler(U32 param)
{
    D_PRINTF("[PLP ] Gpio|0x%4x\n", HalGpio_ReadAllPin());

    rApb.testpinIn.b.TESTPIN11_INT_CLR = 1; // Clear IRQ
    rApb.testpinIn.b.TESTPIN11_INT_CLR = 0; // Clear IRQ

    if (HalGpio_ReadPin(PLP_TESTPIN_NO_PLOSS) == FALSE)
    {
        D_PRINTF("[PLP ] On\n");
        PLP_SET_STATUS(CPU_ID2, PLP_DISCHARGE);
    }
}

#include "NormSection.h"

#if (PLP_CAPACITANCE_FROM_I2C == TRUE)
//-----------------------------------------------------------------------------
/**
    Plp_InitI2cVars
**/
//-----------------------------------------------------------------------------
static void Plp_InitI2cVars(void)
{
    gCapacitanceI2cDevice.port         = I2C_PORT_1;
    gCapacitanceI2cDevice.slaveAddress = I2C_ADDRESS_SILERGY_SYT664;
}

//-----------------------------------------------------------------------------
/**
    Plp_SetI2cChargePower
**/
//-----------------------------------------------------------------------------
static void Plp_SetI2cChargePower(void)
{
    U8  originState;

    // Step 0: get origin state
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_DCP;
    gCapacitanceI2cReadData     = 0;
    HalI2c_Read(&gCapacitanceI2cDevice,
                &gCapacitanceI2cWritedata[0], 1,
                &gCapacitanceI2cReadData,     sizeof(gCapacitanceI2cReadData));
    originState = gCapacitanceI2cReadData;

    // Step 1: write
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_DCP;
    gCapacitanceI2cWritedata[1] = (originState                    & ~I2C_REGISTER_SILERGY_DCP_POWER_M) |
                                  (I2C_REGISTER_SILERGY_DCP_300MA &  I2C_REGISTER_SILERGY_DCP_POWER_M);
    HalI2c_Write(&gCapacitanceI2cDevice, &gCapacitanceI2cWritedata[0], 2, TRUE);

    // Step 2: check state
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_DCP;
    gCapacitanceI2cReadData     = 0;
    HalI2c_Read(&gCapacitanceI2cDevice,
                &gCapacitanceI2cWritedata[0], 1,
                &gCapacitanceI2cReadData,     sizeof(gCapacitanceI2cReadData));

    D_PRINTF("[PLP ] Cap.Reg%d|%2X|%2X\n", I2C_REGISTER_SILERGY_DCP,
                                           originState,
                                           gCapacitanceI2cReadData);
}

//-----------------------------------------------------------------------------
/**
    Plp_GetI2cCapacitance
**/
//-----------------------------------------------------------------------------
static U16 Plp_GetI2cCapacitance(void)
{
    U16 capacitance;
    U16 capacitanceDischargeTime;
    U8  originState;

    capacitance              = 0;

    // Step 0: get origin state
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_CAP_MEASURE;
    gCapacitanceI2cReadData     = 0;
    HalI2c_Read(&gCapacitanceI2cDevice,
                &gCapacitanceI2cWritedata[0], 1,
                &gCapacitanceI2cReadData,     sizeof(gCapacitanceI2cReadData));
    originState = gCapacitanceI2cReadData;

    // Step 1: set Capacitance Measurement Enable
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_CAP_MEASURE;
    gCapacitanceI2cWritedata[1] = originState | I2C_REGISTER_SILERGY_CAP_MEASURE_E;
    HalI2c_Write(&gCapacitanceI2cDevice, &gCapacitanceI2cWritedata[0], 2, TRUE);

    HalTimer_DelayMsec(800);

    // Step 2: get TDISCHARGE High byte
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_T_DISCHARGE_H;
    gCapacitanceI2cReadData     = 0;
    HalI2c_Read(&gCapacitanceI2cDevice,
                &gCapacitanceI2cWritedata[0], 1,
                &gCapacitanceI2cReadData,     sizeof(gCapacitanceI2cReadData));
    capacitance |= (gCapacitanceI2cReadData << 8);

    // Step 3: get TDISCHARGE Low  byte
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_T_DISCHARGE_L;
    gCapacitanceI2cReadData     = 0;
    HalI2c_Read(&gCapacitanceI2cDevice,
                &gCapacitanceI2cWritedata[0], 1,
                &gCapacitanceI2cReadData,     sizeof(gCapacitanceI2cReadData));
    capacitance |= gCapacitanceI2cReadData;

    // Step 4: set Capacitance Measurement Disable
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_CAP_MEASURE;
    gCapacitanceI2cWritedata[1] = originState;
    HalI2c_Write(&gCapacitanceI2cDevice, &gCapacitanceI2cWritedata[0], 2, TRUE);

    return (capacitance * 2);// 2ms
}

//-----------------------------------------------------------------------------
/**
    Plp_GetI2cCapacitanceDebug
**/
//-----------------------------------------------------------------------------
static U16 Plp_GetI2cCapacitanceDebug(void)
{
    U16 capacitance;
    U16 capacitanceDischargeTime;
    U8  originState;
    U8  i;

    capacitance              = 0;
    capacitanceDischargeTime = 0;

    D_PRINTF("[PLP ] Cap.Reg");
    for (i = 0; i < 7; i++)
    {
        gCapacitanceI2cWritedata[0] = i;
        gCapacitanceI2cReadData     = 0;
        HalI2c_Read(&gCapacitanceI2cDevice,
                    &gCapacitanceI2cWritedata[0], 1,
                    &gCapacitanceI2cReadData,     sizeof(gCapacitanceI2cReadData));
        D_PRINTF("|%2X", gCapacitanceI2cReadData);
    }
    D_PRINTF("\n");

    // Step 0: get origin state
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_CAP_MEASURE;
    gCapacitanceI2cReadData     = 0;
    HalI2c_Read(&gCapacitanceI2cDevice,
                &gCapacitanceI2cWritedata[0], 1,
                &gCapacitanceI2cReadData,     sizeof(gCapacitanceI2cReadData));
    //D_PRINTF("[PLP ] Cap.B|%2X\n", CapacitanceReadData);
    originState = gCapacitanceI2cReadData;

    // Step 1: set Capacitance Measurement Enable
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_CAP_MEASURE;
    gCapacitanceI2cWritedata[1] = originState | I2C_REGISTER_SILERGY_CAP_MEASURE_E;
    //D_PRINTF("[PLP ] Cap.M|%2X\n", CapacitanceWritedata[1]);
    HalI2c_Write(&gCapacitanceI2cDevice, &gCapacitanceI2cWritedata[0], 2, TRUE);

    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_CAP_MEASURE;
    gCapacitanceI2cReadData     = 0;
    HalI2c_Read(&gCapacitanceI2cDevice,
                &gCapacitanceI2cWritedata[0], 1,
                &gCapacitanceI2cReadData,     sizeof(gCapacitanceI2cReadData));
    //D_PRINTF("[PLP ] Cap.A|%2X\n", CapacitanceReadData);

    HalTimer_DelayMsec(800);

    // Step 2: get TDISCHARGE High byte
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_T_DISCHARGE_H;
    gCapacitanceI2cReadData     = 0;
    HalI2c_Read(&gCapacitanceI2cDevice,
                &gCapacitanceI2cWritedata[0], 1,
                &gCapacitanceI2cReadData,     sizeof(gCapacitanceI2cReadData));
    capacitance |= (gCapacitanceI2cReadData << 8);
    D_PRINTF("[PLP ] Cap.H|%2X|%4X\n", gCapacitanceI2cReadData, capacitance);

    // Step 3: get TDISCHARGE Low  byte
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_T_DISCHARGE_L;
    gCapacitanceI2cReadData     = 0;
    HalI2c_Read(&gCapacitanceI2cDevice,
                &gCapacitanceI2cWritedata[0], 1,
                &gCapacitanceI2cReadData,     sizeof(gCapacitanceI2cReadData));
    capacitance |= gCapacitanceI2cReadData;
    D_PRINTF("[PLP ] Cap.L|%2X|%4X\n", gCapacitanceI2cReadData, capacitance);

    // Step 4: set Capacitance Measurement Disable
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_CAP_MEASURE;
    gCapacitanceI2cWritedata[1] = originState;
    HalI2c_Write(&gCapacitanceI2cDevice, &gCapacitanceI2cWritedata[0], 2, TRUE);
    gCapacitanceI2cWritedata[0] = I2C_REGISTER_SILERGY_CAP_MEASURE;
    gCapacitanceI2cReadData     = 0;
    HalI2c_Read(&gCapacitanceI2cDevice,
                &gCapacitanceI2cWritedata[0], 1,
                &gCapacitanceI2cReadData,     sizeof(gCapacitanceI2cReadData));
    //D_PRINTF("[PLP ] Cap.R|%2X\n", CapacitanceReadData);

    capacitanceDischargeTime = capacitance * 2; // 2ms
    return capacitanceDischargeTime;
}

#else // (PLP_CAPACITANCE_FROM_I2C == TRUE)
//-----------------------------------------------------------------------------
/**
    Plp_GetIoCapacitance
**/
//-----------------------------------------------------------------------------
static U32 Plp_GetIoCapacitance(U32 mode, U32 parameter)
{
    HwTimerTick_t tick;
    HwTimerTick_t tickTotal;
    CapInfo_t     capInfo;
    U32           dwMaxTimeout    = DWORD_MASK;
    U32           dwChargeTime    = 0;
    U32           dwDisChargeTime = 0;
    U32           dwTotalTime     = 0;
    U32           dwTimeoutFlag   = FALSE;

    MEM_CLR(&capInfo, sizeof(CapInfo_t));

    switch(mode)
    {
    /*
    case PLP_HWCHECK_MODE_CHARGE:
        D_PRINTF("[PLP ] ENCON|%d|%d\n", PLP_GPIO_READ_OUTPUT_ENCON, parameter);
        if(parameter)   PLP_GPIO_WRITE_OUTPUT_ENCON(1); //ENCON OUTPUT HIGH
        else            PLP_GPIO_WRITE_OUTPUT_ENCON(0); //ENCON OUTPUT LOW
        break;
    case PLP_HWCHECK_MODE_C_TEST:
        D_PRINTF("[PLP ] C_TEST|%d|%d\n", PLP_GPIO_READ_OUTPUT_C_TEST, parameter);
        if(parameter)   PLP_GPIO_WRITE_OUTPUT_C_TEST(1); //C_TEST OUTPUT HIGH
        else            PLP_GPIO_WRITE_OUTPUT_C_TEST(0); //C_TEST OUTPUT LOW
        break;
    */
    case PLP_HWCHECK_MODE_TEST:
        D_PRINTF("[PLP ] FunctionStart\n");
        Plp_SetIoCapMode(PLP_CHARGE);

        D_PRINTF("[PLP ] PLOSS|%x SPG|%x\n", PLP_GPIO_READ_INTPUT_PLOSS, PLP_GPIO_READ_INTPUT_SPG);
        if ((PLP_GPIO_READ_INTPUT_SPG == 1) && (PLP_GPIO_READ_INTPUT_PLOSS == 1))
        {
            tickTotal = HalTimer_ReadTick();
            Plp_SetIoCapMode(PLP_DISCHARGE);

            // TPS3700DDCR when change C_HEALTH(Discharge) will pulse before discharge so need filter
            //      |--|   |------------------------------|
            // _____|  |___|                              |___________________

            while (dwMaxTimeout--)
            {
                if (PLP_GPIO_READ_INTPUT_C_HEALTH)
                {
                    tick = HalTimer_ReadTick();
                    while(PLP_GPIO_READ_INTPUT_C_HEALTH)
                    {
                        if(dwMaxTimeout)    dwMaxTimeout--;
                        else                break;
                    }
                    if (HalTimer_GetElapsedTimeUsec(tick) > PLP_HWCHECK_FILTER_IC_DISCHARGE_PULSE_TH)
                    {
                        dwDisChargeTime = HalTimer_GetElapsedTimeUsec(tick);
                        break;
                    }
                }
            }

            // Recharge
            M_PRINTF("\n[PLP ] ReCharge\n");
            HalTimer_DelayMsec(100);
            tick = HalTimer_ReadTick();
            Plp_SetIoCapMode(PLP_CHARGE);
            while(!PLP_GPIO_READ_INTPUT_SPG)
            {
                if(dwMaxTimeout)    dwMaxTimeout--;
                else                break;
            }

            dwChargeTime = HalTimer_GetElapsedTimeUsec(tick);
            dwTotalTime  = HalTimer_GetElapsedTimeUsec(tickTotal);
            capInfo.ChargeTime    = dwChargeTime;
            capInfo.DisChargeTime = dwDisChargeTime;

            if (dwMaxTimeout == 0)  dwTimeoutFlag = TRUE;

            if(dwTimeoutFlag == FALSE)
            {
                D_PRINTF("[PLP ] DisChargeElapsed|%d.", (dwDisChargeTime/1000));
                dwDisChargeTime = dwDisChargeTime % 1000;
                if      (dwDisChargeTime<10)    D_PRINTF("00");
                else if (dwDisChargeTime<100)   D_PRINTF("0");
                D_PRINTF("%dms\n", dwDisChargeTime);

                D_PRINTF("[PLP ] ChargeElapsed|%d.", (dwChargeTime/1000));
                dwChargeTime = dwChargeTime % 1000;
                if      (dwChargeTime<10)   D_PRINTF("00");
                else if (dwChargeTime<100)  D_PRINTF("0");
                D_PRINTF("%dms\n", dwChargeTime);

                D_PRINTF("[PLP ] TotalElapsed|%d.", (dwTotalTime/1000));
                dwTotalTime = dwTotalTime % 1000;
                if      (dwTotalTime<10)   D_PRINTF("00");
                else if (dwTotalTime<100)  D_PRINTF("0");
                D_PRINTF("%dms\n", dwTotalTime);
            }
            else
            {
                D_PRINTF("[PLP ] Timeout\n");
            }

            D_PRINTF("[PLP ] FunctionEnd\n");
        }
        break;
    default:
        break;
    }

    return capInfo.DisChargeTime;
}

//-----------------------------------------------------------------------------
/**
    Plp_SetIoCapMode

    @param[in]  mode
**/
//-----------------------------------------------------------------------------
static void Plp_SetIoCapMode(U8 mode)
{
    switch(mode)
    {
    case PLP_CHARGE:
        PLP_GPIO_WRITE_OUTPUT_C_TEST(DISABLE);  // C_TEST OUTPUT LOW
        PLP_GPIO_WRITE_OUTPUT_ENCON(ENABLE);    // ENCON OUTPUT HIGH
        break;
    case PLP_DISCHARGE:
        PLP_GPIO_WRITE_OUTPUT_C_TEST(ENABLE);   // C_TEST OUTPUT HIGH
        PLP_GPIO_WRITE_OUTPUT_ENCON(DISABLE);   // ENCON OUTPUT LOW
        break;
    default:
        break;
    }
}
#endif

//-----------------------------------------------------------------------------
/**
    AplPlp_HwCheck

    @param[in]  mode     parameter
**/
//-----------------------------------------------------------------------------
U32 AplPlp_HwCheck(U32 mode, U32 parameter)
{
    U32 plpCapacitanceInMs;

    #if (PLP_CAPACITANCE_FROM_I2C == TRUE)
        plpCapacitanceInMs = Plp_GetI2cCapacitance();
    #else
        plpCapacitanceInMs = Plp_GetIoCapacitance(mode, parameter) / 1000;    // us to ms
    #endif

    D_PRINTF("[PLP ] Cap.Time|%4dms\n", plpCapacitanceInMs);

    return plpCapacitanceInMs;
}
#endif // (_CPU_ == CPU_ID2)

//-----------------------------------------------------------------------------
/**
    AplPlp_OneTimeInit

    @param[in]  initMode
**/
//-----------------------------------------------------------------------------
Error_t AplPlp_OneTimeInit(InitBootMode_t initMode)
{
    #if (_CPU_ == CPU_ID0)
        PLP_SET_STATUS(CPU_ID0, PLP_CHARGE);

        #if (PLP_CAPACITANCE_FROM_I2C == FALSE)
            HalGpio_OutputEnable(PLP_TESTPIN_OUTPUT);           // Enable ENCON & C_Test pin Ouput

            HalGpio_WritePin(PLP_TESTPIN_NO_C_TEST, DISABLE);   // Disable C_Test
            HalGpio_WritePin(PLP_TESTPIN_NO_ENCON, ENABLE);     // PLP Charge
        #endif

    #elif (_CPU_ == CPU_ID1)
        PLP_SET_STATUS(CPU_ID1, PLP_CHARGE);

    #elif (_CPU_ == CPU_ID2)
        U32 timeout;
        U8  plpChargeStatus = PLP_DISCHARGE;

        PLP_SET_STATUS(CPU_ID2, PLP_CHARGE);

        #if (PLP_CAPACITANCE_FROM_I2C == TRUE)
            Plp_InitI2cVars();
            Plp_SetI2cChargePower();
        #else
            HalGpio_WritePin(PLP_TESTPIN_NO_C_TEST, DISABLE);        // Disable C_Test
        #endif

        timeout = HalTimer_ReadTick();

        D_PRINTF("[PLP ] ChargeGpio|0x%4x\n", HalGpio_ReadAllPin());
        do
        {
            if (HalGpio_ReadPin(PLP_TESTPIN_NO_SPG) == PLP_CHARGE)
            {
                plpChargeStatus = PLP_CHARGE;
                break;
            }
            else if (HalTimer_GetElapsedTimeMsec(timeout) > PLP_CHARGE_TIMEOUT_MS)
            {
                D_PRINTF("[PLP ] ChargeTimeout|%d\n", HalTimer_GetElapsedTimeMsec(timeout));
                plpChargeStatus = PLP_DISCHARGE;
                break;
            }
        } while (HalGpio_ReadPin(PLP_TESTPIN_NO_SPG) == PLP_DISCHARGE);

        if (plpChargeStatus == PLP_CHARGE)
        {
            D_PRINTF("[PLP ] ChargeDone\n");
            Plp_IrqEnable();
        }

    #endif
    return(cEcNoError);
}
#endif // (CO_SUPPORT_PLP == TRUE)

/* End of File */

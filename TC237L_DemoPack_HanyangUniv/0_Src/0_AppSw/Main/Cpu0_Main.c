/**
 * \file Cpu0_Main.c
 * \brief Main function definition for Cpu core 0 .
 *
 * \copyright Copyright (c) 2012 Infineon Technologies AG. All rights reserved.
 *
 *
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */

#include "Tricore/Cpu/Std/Ifx_Types.h"
#include "Tricore/Cpu/Std/IfxCpu_Intrinsics.h"
#include "Tricore/Scu/Std/IfxScuWdt.h"
#include <Stm/Std/IfxStm.h>
#include <Port/Std/IfxPort.h>
#include "Test_Irq.h"
#include "Test_ModuleInit.h"

#include "Test_Pwm.h"
#include "Test_Adc.h"
#include "Test_Can.h"
#include "TLF35584Demo.h"

#include "glcd.h"
#include "logo.h"
#include "usr_sprintf.h"

#define CENTER          ((uint16)0x280U)
#define LEFT            ((uint16)0x23CU)
#define RIGHT           ((uint16)0x2C8U)

extern void Can_Test(void);
extern void Uart_Test(void);
extern void Uart_Transmit(char* text);

extern uint32 	Adc_Result_Scan[MaxModuleNum][MaxChNum];

volatile uint16 inittest = 0;
volatile uint16 initStartCount = 0;

uint16 cameraOut[2][128];
uint8 cameraUpdateNum;

void FrontControl(uint16 Angle){

	if(Angle > RIGHT)Angle = RIGHT;
	if(Angle < LEFT)Angle = LEFT;
	Pwm_StepDutyUpdate(IfxGtm_TOM1_0_TOUT32_P33_10_OUT,Angle);

}


void CarRuning(uint16 Speed,uint16 Direction){

	Pwm_MotorDutyAndDirectionControl(Speed, Direction);

}

void RcCarInit(void){
	 uint16 i = 0;
	   char text[32];

	 FrontControl(CENTER);

	 Delay_ms(2000);

	 FrontControl(LEFT);

	 Delay_ms(2000);

	 FrontControl(RIGHT);

	 Delay_ms(2000);

	 FrontControl(CENTER);

	 Delay_ms(2000);

    for(i=0;i<800;i++){

    CarRuning(i, 1);
    Delay_ms(4);
    }

    for(i=800;i>1;i--){

    CarRuning(i, 1);
    Delay_ms(4);
    }

    for(i=0;i<850;i++){

    CarRuning(i, 0);
    Delay_ms(4);
    }

    for(i=850;i>1;i--){

    CarRuning(i, 0);
    Delay_ms(4);
    }


	P02_OUT.B.P0 = 1;
	P00_OUT.B.P2 = 1;
	P02_OUT.B.P2 = 1;
	P33_OUT.B.P5 = 1;

}
void core0_main (void)
{
    __enable ();
    /*
     * !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdog in the demo if it is required and also service the watchdog periodically
     * */
    IfxScuWdt_disableCpuWatchdog (IfxScuWdt_getCpuWatchdogPassword ());
    IfxScuWdt_disableSafetyWatchdog (IfxScuWdt_getSafetyWatchdogPassword ());

    Test_ModuleInit();

    uint16 i, j, Line, LineCnt;
    volatile uint8 cameraNum = 0;
    char text[32];
    uint16 cameraData[128];
    double whiteAvg[128], blackAvg[128];
    double alpha = 0.9;
	Dio_Configuration(&MODULE_P00, 1, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_high);
	Dio_Configuration(&MODULE_P00, 4, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_low);
    cameraUpdateNum = 0;

    GLCD_clear(COLOR_BLACK);
    GLCD_setBackColor(COLOR_BLUE);
    GLCD_setTextColor(COLOR_WHITE);
    usr_sprintf(text,"start");
    GLCD_displayStringLn(LINE0, text);

    for(i = 0;i < 128;i++)
    {
    	whiteAvg[i] = 3000;
    	blackAvg[i] = 0;
    }

	while (1)
    {
    	cameraNum = (cameraUpdateNum?0:1);
    	for(i = 0; i < 128; i++)
    	{
    		cameraData[i] = cameraOut[cameraNum][i];
    	}

        GLCD_clear(COLOR_BLACK);
        Line = 0;
        LineCnt = 0;
    	for(i = 0;i < 128;i++)
    	{
    		if(cameraData[i]*2 > whiteAvg[i] + blackAvg[i])
    		{
    			whiteAvg[i] = whiteAvg[i]*alpha + (double)cameraData[i]*(1 - alpha);
    			if(whiteAvg[i] < 2500)
    			{
    				whiteAvg[i] = 2500;
    			}
    		}
    		else
    		{
    			LineCnt++;
    			blackAvg[i] = blackAvg[i]*alpha + (double)cameraData[i]*(1 - alpha);
    			if(blackAvg[i] > 1500)
    			{
    				blackAvg[i] = 1500;
    			}
    			if(i > 16 && i < 112)
    			{
    				GLCD_bitmap(i*2, 230, LOGO_WIDTH, LOGO_HEIGHT, logo_YELLOW);
   				}
    		}
			GLCD_bitmap( i*2, (uint16)whiteAvg[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_GREEN);
			GLCD_bitmap( i*2, (uint16)blackAvg[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_RED);
			GLCD_bitmap( i*2, cameraData[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_WHITE);
			if(i % 8 == 7)
			{
				if(LineCnt > 4)
				{
					Line |= 1<<(i/8);
				}
				LineCnt = 0;
			}
    	}
    	Delay_ms(100);
    }
}

void SecondTimer_Initialization(void)
{
    volatile float       stm_freq;
    Ifx_STM             *stm = &MODULE_STM0;
    IfxStm_CompareConfig stmCompareConfig;

    /* suspend by debugger enabled */
    IfxStm_enableOcdsSuspend(stm);
    /* get current STM frequency : debug purpose only*/
    stm_freq = IfxScuCcu_getStmFrequency();
    /* constructor of configuration */
    IfxStm_initCompareConfig(&stmCompareConfig);
	stmCompareConfig.triggerPriority = ISR_PRIORITY_STM;
	stmCompareConfig.ticks = 100000000;
	stmCompareConfig.typeOfService = IfxSrc_Tos_cpu0;
    /* Now Compare functionality is initialized */
    IfxStm_initCompare(stm, &stmCompareConfig);

} // End of TaskScheduler_Initialization()

//*********************************************************************************************
// @Function	 	void UsrIsr_Stm_0(void)
// @Description   	STM0 Interrupt for system tick generation
// @Returnvalue		None
// @Parameters    	None
//*********************************************************************************************
IFX_INTERRUPT (SecondTimer_Isr, 0, ISR_PRIORITY_STM);
void SecondTimer_Isr(void)
{
    Ifx_STM *stm = &MODULE_STM0;
    char text[64];

    /* Set next 1ms scheduler tick alarm */
    IfxStm_updateCompare(stm, IfxStm_Comparator_0, IfxStm_getLower(stm) + 10000);

    initStartCount++;

	P00_OUT.B.P1 = !P00_OUT.B.P1;

//	Uart_Test();
//	usr_sprintf(text, "%d", Adc_Result_Scan[1][9]);
//	Uart_Transmit(text);
	if(initStartCount > 270)
	{
		initStartCount = 0;
		P00_OUT.B.P1 = 1;
		P00_OUT.B.P4 = 1;
		cameraUpdateNum = (cameraUpdateNum?0:1);
	}
	else if(initStartCount < 256 && initStartCount%2)
	{
//		Test_VadcAutoScan(IfxVadc_GroupId_0);
		Test_VadcAutoScan(IfxVadc_GroupId_1);
		cameraOut[cameraUpdateNum][initStartCount/2] = Adc_Result_Scan[1][9];//AN21
	}

	if(initStartCount == 3)
	    P00_OUT.B.P4 = 0;
/*
    if(initStartCount==3){
	    Can_Test();
    }

    if(initStartCount==4){
        Uart_Test();
    }
    if(initStartCount>5){
    	inittest = 1;
    	initStartCount = 0;
    }
*/
    __enable();

}


void TestTimer_Initialization(void)
{
    volatile float       stm_freq;
    Ifx_STM             *stm = &MODULE_STM0;
    IfxStm_CompareConfig stmCompareConfig;

    /* suspend by debugger enabled */
    //IfxStm_enableOcdsSuspend(stm);
    /* get current STM frequency : debug purpose only*/
    stm_freq = IfxScuCcu_getStmFrequency();
    /* constructor of configuration */
    IfxStm_initCompareConfig(&stmCompareConfig);
	stmCompareConfig.triggerPriority = ISR_PRIORITY_STM1;
	stmCompareConfig.ticks = 10000000;
	stmCompareConfig.typeOfService = IfxSrc_Tos_cpu0;

	stmCompareConfig.comparator = 1;
	stmCompareConfig.comparatorInterrupt = IfxStm_ComparatorInterrupt_ir1;
    /* Now Compare functionality is initialized */
    IfxStm_initCompare(stm, &stmCompareConfig);

} // End of TaskScheduler_Initialization()

IFX_INTERRUPT (TestTimer_Isr, 0, ISR_PRIORITY_STM1);
void TestTimer_Isr(void)
{
    Ifx_STM *stm = &MODULE_STM0;

    /* Set next 1ms scheduler tick alarm */
    IfxStm_updateCompare(stm, IfxStm_Comparator_1, IfxStm_getLower(stm) + 1000000000);

    P13_OUT.B.P3 = !P13_OUT.B.P3;

    __enable();

}

void Encoder_init(void)
{

}

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

#include "IfxGpt12_bf.h"
#include "IfxGpt12_reg.h"
#include "IfxGpt12_regdef.h"

#include "math.h"

#include "glcd.h"
#include "logo.h"
#include "usr_sprintf.h"

#include "DNN.h"

#define CENTER          ((uint16)0x280U)
#define LEFT            ((uint16)0x233U)
#define RIGHT           ((uint16)0x2CCU)

#define STOP 0
#define RUN 1
#define SCHOOL_ZONE 2

extern void Can_Test(void);
extern void Uart_Test(void);
extern void Uart_Transmit(char* text);

extern uint32 	Adc_Result_Scan[MaxModuleNum][MaxChNum];

volatile uint16 inittest = 0;
volatile uint16 initStartCount = 0;

uint16 cameraOutA[2][128], cameraOutB[2][128];
uint8 cameraUpdateNum;
uint16 distanceSensor[2];

uint8 carState;
uint16 carSpeed = 0;

#define PI 3.141592

double calculateR(double theta)
{
	theta *= PI;
	theta /= 180;
	return 25*sqrt(1/(tan(theta)*tan(theta)) + 1);
}

void FrontControl(uint16 Angle){

	if(Angle > RIGHT)Angle = RIGHT;
	if(Angle < LEFT)Angle = LEFT;
	Pwm_StepDutyUpdate(IfxGtm_TOM1_0_TOUT32_P33_10_OUT,Angle);

}


void CarRuning(uint16 Speed,uint16 Direction){

	switch(carState)
	{
	case STOP:
		Pwm_MotorDutyAndDirectionControl(0, Direction);
		break;
	case RUN:
		Pwm_MotorDutyAndDirectionControl(Speed, Direction);
		break;
	case SCHOOL_ZONE:
		Pwm_MotorDutyAndDirectionControl(1000, Direction);//���� �ʿ�
		break;
	}
	//Pwm_MotorDutyAndDirectionControl(Speed, Direction);

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

    carState = STOP;

    Test_ModuleInit();

    int i, j, LineCnt, LineCntA, MaxLineCntA, LineCenterA, LastLineCenterA = 0, LineCntB, MaxLineCntB, LineCenterB, LastLineCenterB = 0;
    int LeftLine = 1, RightLine = 1, servoAngle;
    char LineChkA = 1, LineChkB = 1;
    uint8 cameraNum = 0;
    char text[32];
    uint16 cameraDataA[128], cameraDataB[128];
    double whiteAvgA[128], whiteAvgB[128];
    double alpha = 0.9, beta = 0.7;
    double n, m, d, cameraXa, cameraXb, r;
    static double cameraYa = 50, cameraYb = 40;

	Dio_Configuration(&MODULE_P00, 8, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_high);
	Dio_Configuration(&MODULE_P00, 6, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_low);

    cameraUpdateNum = 0;

    GLCD_clear(COLOR_BLACK);
    GLCD_setBackColor(COLOR_BLACK);
    GLCD_setTextColor(COLOR_WHITE);

    for(i = 0;i < 128;i++)
    {
    	whiteAvgA[i] = whiteAvgB[i] = 0;
    }

    for(j = 30; j > 0; j--)
    {
    	cameraNum = (cameraUpdateNum?0:1);
        for(i = 1; i < 127; i++)
        {
        	cameraDataA[i] = (cameraOutA[cameraNum][i]*2 + cameraOutA[cameraNum][i + 1] + cameraOutA[cameraNum][i - 1])/4 ;
        	cameraDataB[i] = (cameraOutB[cameraNum][i]*2 + cameraOutB[cameraNum][i + 1] + cameraOutB[cameraNum][i - 1])/4;
        }
        GLCD_clear(COLOR_BLACK);
        usr_sprintf(text,"%01d.%01dsec",j/10,j%10);
        GLCD_displayStringLn(LINE0, text);
        for(i = 1; i < 127; i++)
        {
            if(whiteAvgA[i] < cameraDataA[i])
            {
            	whiteAvgA[i] = cameraDataA[i];
            }
            if(whiteAvgB[i] < cameraDataB[i])
            {
               	whiteAvgB[i] = cameraDataB[i];
            }
			GLCD_bitmap( i*2, (uint16)whiteAvgA[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_GREEN);
			GLCD_bitmap( i*2, (uint16)whiteAvgB[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_BLUE);
			GLCD_bitmap( i*2, cameraDataA[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_YELLOW);
			GLCD_bitmap( i*2, cameraDataB[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_RED);
        }
        Delay_ms(100);
    }
    carState = RUN;
	while (1)
    {
    	cameraNum = (cameraUpdateNum?0:1);
        for(i = 1; i < 127; i++)
        {
        	cameraDataA[i] = (cameraOutA[cameraNum][i]*2 + cameraOutA[cameraNum][i + 1] + cameraOutA[cameraNum][i - 1])/4 ;
        	cameraDataB[i] = (cameraOutB[cameraNum][i]*2 + cameraOutB[cameraNum][i + 1] + cameraOutB[cameraNum][i - 1])/4;
        }

        GLCD_clear(COLOR_BLACK);
        LineCnt = LineCntA = LineCntB = 0;
        LineCenterA = LineCenterB = 0;
        MaxLineCntA = MaxLineCntB = 1;
    	for(i = 16;i < 112;i++)
    	{
    		if(cameraDataA[i] > whiteAvgA[i]*beta)
    		{
    			whiteAvgA[i] = whiteAvgA[i]*alpha + (double)cameraDataA[i]*(1 - alpha);
    			if(LineChkA) LineChkA = 0;
    			else LineCntA = 0;
    			LineCnt++;
    		}
    		else
    		{
    			LineChkA = 1;
    			if(++LineCntA > MaxLineCntA)
				{
					MaxLineCntA = LineCntA;
					LineCenterA = i - LineCntA/2;
				}
    			GLCD_bitmap(i*2, 230, LOGO_WIDTH, LOGO_HEIGHT, logo_YELLOW);
    		}

    		if(LineCnt > 32)
    		{
    			carState = SCHOOL_ZONE;
    		}
			GLCD_bitmap( i*2, (uint16)whiteAvgA[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_GREEN);
			GLCD_bitmap( i*2, cameraDataA[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_YELLOW);
    	}
		LineCnt = 0;
    	for(i = 40;i < 88;i++)
    	{
    		if(cameraDataB[i] > whiteAvgB[i]*beta)
    		{
    			whiteAvgB[i] = whiteAvgB[i]*alpha + (double)cameraDataB[i]*(1 - alpha);
    			if(LineChkB) LineChkB = 0;
    			else LineCntB = 0;
    			LineCnt++;
    		}
    		else
    		{
    			LineChkB = 1;
    			if(++LineCntB > MaxLineCntB)
    			{
    				MaxLineCntB = LineCntB;
    				LineCenterB = i - LineCntB/2;
    			}

    		    GLCD_bitmap((3*i-128)*2, 220, LOGO_WIDTH, LOGO_HEIGHT, logo_YELLOW);
    		}
       		if(LineCnt > 20)
        	{
        		carState = SCHOOL_ZONE;
        	}
			GLCD_bitmap( (3*i-128)*2, (uint16)whiteAvgB[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_BLUE);
			GLCD_bitmap( (3*i-128)*2, cameraDataB[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_RED);
    	}

    	if(LineCenterA)
		{
    		GLCD_bitmap( LineCenterA*2, 230, LOGO_WIDTH, LOGO_HEIGHT, logo_RED);
    		GLCD_bitmap( (LineCenterA-1)*2, 230, LOGO_WIDTH, LOGO_HEIGHT, logo_RED);
    		GLCD_bitmap( (LineCenterA+1)*2, 230, LOGO_WIDTH, LOGO_HEIGHT, logo_RED);
		}
    	if(LineCenterB)
    	{
    		GLCD_bitmap( LineCenterB*2, 220, LOGO_WIDTH, LOGO_HEIGHT, logo_RED);
			GLCD_bitmap( (LineCenterB-1)*2, 220, LOGO_WIDTH, LOGO_HEIGHT, logo_RED);
			GLCD_bitmap( (LineCenterB+1)*2, 220, LOGO_WIDTH, LOGO_HEIGHT, logo_RED);
    	}

        if(!LastLineCenterA && !LastLineCenterB && (LineCenterA||LineCenterB))//���� �߰�
        {
        	uint16 LineCenter = LineCenterA;
           	if(LineCenterA == 0)
           		LineCenter = LineCenterB;
           	else if(LineCenterB != 0)
           		LineCenter = (LineCenterA + LineCenterB)/2;

           	if(LineCenter < 64)
           		LeftLine--;
           	else
           		RightLine--;
        }

        if(!LineCenterA && !LineCenterB)//���� �����
        {
           	if(LastLineCenterA == 0)
           		LastLineCenterA = LastLineCenterB;
           	else if(LastLineCenterB != 0)
           		LastLineCenterA = (LastLineCenterA + LastLineCenterB)/2;

           	if(LastLineCenterA == 0);
           	else if(LastLineCenterA < 64)
           		LeftLine++;
           	else
           		RightLine++;
        }

		servoAngle = servoCalc(LeftLine, RightLine, LineCenterA, LineCenterB);
      
        FrontControl(servoAngle);

        LastLineCenterA = LineCenterA;
        LastLineCenterB = LineCenterB;

        usr_sprintf(text,"L %d R %d A %x",LeftLine,RightLine,servoAngle);
        GLCD_displayStringLn(LINE1, text);
    	Delay_ms(50);
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

	P00_OUT.B.P6 = !P00_OUT.B.P6;//camera CLK
	P00_OUT.B.P1 = !P00_OUT.B.P1;

	if(initStartCount > 270)
	{
		initStartCount = 0;
		P00_OUT.B.P8 = 1;
		P00_OUT.B.P6 = 1;
		cameraUpdateNum = (cameraUpdateNum?0:1);
	}
	else if(initStartCount < 256 && initStartCount%2)
	{
		Test_VadcAutoScan(IfxVadc_GroupId_1);
		cameraOutA[cameraUpdateNum][initStartCount/2] = Adc_Result_Scan[1][9];//AN21
		cameraOutB[cameraUpdateNum][initStartCount/2] = Adc_Result_Scan[1][8];//AN20

		distanceSensor[0] = Adc_Result_Scan[1][3];//AN15
		distanceSensor[1] = Adc_Result_Scan[1][4];//AN16
		if(distanceSensor[1] > 1500)//AEB
		{
		    CarRuning(0, 1);
		}
	}

	if(initStartCount == 3)//camera SI
	{
		P00_OUT.B.P8 = 0;
	}
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
	Ifx_GPT12 *gpt = &MODULE_GPT120;
    char text[32];
    /* Set next 1ms scheduler tick alarm */
    IfxStm_updateCompare(stm, IfxStm_Comparator_1, IfxStm_getLower(stm) + 100000000);

    CarRuning(carSpeed, 1);

    gpt->T3.B.T3 = 0;//encoder counter

    __enable();

}

void Encoder_init(void)
{
	Ifx_GPT12 *gpt = &MODULE_GPT120;
	Dio_Configuration(&MODULE_P02, 6, IfxPort_Mode_inputNoPullDevice,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_low);

    IfxScuWdt_clearCpuEndinit(IfxScuWdt_getCpuWatchdogPassword());
	gpt->CLC.B.DISR = 0b0;
    IfxScuWdt_setCpuEndinit(IfxScuWdt_getCpuWatchdogPassword());

	gpt->T3CON.B.T3I = 0b001;
	gpt->T3CON.B.T3M = 0b001;
	gpt->T3CON.B.T3R = 0b1;
	gpt->T3CON.B.T3UD = 0b0;
	gpt->T3CON.B.T3UDE = 0b0;
	gpt->T3CON.B.T3OTL = 0b1;
	gpt->T3CON.B.T3OE = 0b0;
	gpt->T3CON.B.BPS1 = 0b01;
	gpt->T3CON.B.T3EDGE = 0b1;
	gpt->T3CON.B.T3CHDIR = 0b0;

	gpt->PISEL.B.IST3IN = 0b00;
}

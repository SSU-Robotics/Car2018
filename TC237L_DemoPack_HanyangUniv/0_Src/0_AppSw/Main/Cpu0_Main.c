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
//#define __debug_tungun__
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
#define LINE_CHANGE 3
#define HILL 4
#define RUN_WAIT 5
#define SCHOOL_ZONE_WAIT 6

#define NORMAL 0
#define FINAL 1

extern void Can_Test(void);
extern void Uart_Test(void);
extern void Uart_Transmit(char* text);
extern void Uart_Receive(char *text);
extern uint32    Adc_Result_Scan[MaxModuleNum][MaxChNum];

volatile uint16 inittest = 0;
volatile uint16 initStartCount = 0;

uint16 cameraOutA[2][128], cameraOutB[2][128];
uint8 cameraUpdateNum;
uint16 distanceSensor[2];

volatile uint8 carState = STOP;
volatile uint8 carMode = NORMAL;
uint16 dcSpeed = 0, dcGoalSpeed = 0, lineState = RIGHT;

int LeftLine = 1, RightLine = 1;

#define PI 3.141592

double calculateR(double theta)
{
   theta *= PI;
   theta /= 180;
   return 25*sqrt(1/(tan(theta)*tan(theta)) + 1);
}

double   I_err = 0, PID_Duty = 0;
uint16 PID_encoder = 0;
void PID(uint16 encoder_pulse, uint16 goal_encoder_pulse)
{
   double Kp = 0.9023;
   double Ki = 0.8056;

   double current_RPM = 0, goal_RPM = 0, err = 0;
   double Kp_value = 0, Ki_value = 0, PID_RPM = 0;

   err = goal_encoder_pulse - encoder_pulse;
   Kp_value = Kp * err;

   I_err += err;
   Ki_value = Ki * I_err;

   PID_encoder = Kp_value + Ki_value;
   PID_encoder *= 1.3;
}

void FrontControl(uint16 Angle){

   if(Angle > RIGHT)Angle = RIGHT;
   if(Angle < LEFT)Angle = LEFT;
   Pwm_StepDutyUpdate(IfxGtm_TOM1_0_TOUT32_P33_10_OUT,Angle);

}


void CarRuning(uint16 Speed,uint16 Direction){
	 Pwm_MotorDutyAndDirectionControl(Speed, Direction);
	 return;
   switch(carState)
   {
   case STOP:
      Pwm_MotorDutyAndDirectionControl(0, Direction);
      break;
   default:
      if(Speed < 0)Speed = 0;
      if(Speed > 1500)Speed = 1500;
      Pwm_MotorDutyAndDirectionControl(Speed, Direction);
      break;
   }
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
void makeTestData(uint16 *cameraDataA,uint16 *cameraDataB, int servoAngle)
{
   int i;
   char text[128];
   for(i=26;i<112;i++){
      usr_sprintf(text,"%d ", (int)cameraDataA[i]);
      Uart_Transmit(text);
   }
   usr_sprintf(text,"\n");
   Uart_Transmit(text);
   for(i=40;i<88;i++){
      usr_sprintf(text,"%d ", (int)cameraDataB[i]);
      Uart_Transmit(text);
   }
   usr_sprintf(text,"\n");
   Uart_Transmit(text);
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

    int i, j, LineCnt, LineCntA, MaxLineCntA, LineCenterA, LastLineCenterA = 0, LineCntB, MaxLineCntB, LineCenterB, LastLineCenterB = 0,LastLineCenter = 0, LineCenter;
    int servoAngle = CENTER;
    char LineChkA = 1, LineChkB = 1;
    uint8 cameraNum = 0;
    char text[32];
    uint16 cameraDataA[128], cameraDataB[128];
    double whiteAvgA[128], whiteAvgB[128], whiteAvgMinA[128], whiteAvgMinB[128];
    double alpha = 0.95, beta = 0.65;//라인 인식 값 #수정
    double n, m, d, cameraXa, cameraXb, r, theta;
    const double cameraYa = 40, cameraYb = 30;

   Dio_Configuration(&MODULE_P00, 8, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_high);
   Dio_Configuration(&MODULE_P00, 6, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_low);

    cameraUpdateNum = 0;

    GLCD_clear(COLOR_BLACK);
    GLCD_setBackColor(COLOR_BLACK);
    GLCD_setTextColor(COLOR_WHITE);
    carState = STOP;
    FrontControl(CENTER);

    for(i = 0;i < 128;i++)
    {
       whiteAvgA[i] = whiteAvgB[i] = 0;
    }

    for(j = 30; j > 0; j--)//3초 대기 후 출발
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
               whiteAvgMinA[i] = whiteAvgA[i] * 0.6;
            }
            if(whiteAvgB[i] < cameraDataB[i])
            {
               whiteAvgB[i] = cameraDataB[i];
               whiteAvgMinB[i] = whiteAvgB[i] * 0.6;
            }
         GLCD_bitmap( i*2, (uint16)whiteAvgA[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_GREEN);
         GLCD_bitmap( i*2, (uint16)whiteAvgB[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_BLUE);
         GLCD_bitmap( i*2, cameraDataA[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_YELLOW);
         GLCD_bitmap( i*2, cameraDataB[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_RED);
        }
        Delay_ms(95);
    }

    carState = RUN;
    while (1)
    {
       /*
       usr_sprintf(text,"\n------------\n");
       Uart_Receive(text);
       Uart_Transmit(text);
       usr_sprintf(text,"\n------------\n");
       Uart_Transmit(text);
       */

/*    	switch(carState)
    	{
    	case STOP:
            usr_sprintf(text,"STOP         ");
    		break;
    	case RUN:
            usr_sprintf(text,"RUN          ");
    		break;
    	case SCHOOL_ZONE:
            usr_sprintf(text,"SCHOOL_ZONE  ");
    		break;
    	case LINE_CHANGE:
            usr_sprintf(text,"LINE_CHANGE  ");
    		break;
    	case HILL:
            usr_sprintf(text,"HILL         ");
    		break;
    	case RUN_WAIT:
            usr_sprintf(text,"RUN_W        ");
    		break;
    	case SCHOOL_ZONE_WAIT:
            usr_sprintf(text,"SCHOOL_ZONE_W");
    		break;
    	}
        GLCD_displayStringLn(LINE0, text);
        Delay_ms(10);*/
       cameraNum = (cameraUpdateNum?0:1);
        LineCnt = 0;
        LineCntA = 0;
        LineCntB = 0;
        LineCenterA = LineCenterB = 0;
        MaxLineCntA = 2;
        MaxLineCntB = 1;
        GLCD_clear(COLOR_BLACK);//lcd 초기화 카메라 확인할 때 주석 해제
       for(i = 16;i < 112;i++)
       {
           cameraDataA[i] = (cameraOutA[cameraNum][i]*2 + cameraOutA[cameraNum][i + 1] + cameraOutA[cameraNum][i - 1])/4 ;
          if(cameraDataA[i] > whiteAvgA[i]*beta)
          {
             whiteAvgA[i] = whiteAvgA[i]*alpha + (double)cameraDataA[i]*(1 - alpha);
             if(whiteAvgA[i] < whiteAvgMinA[i]) whiteAvgA[i] = whiteAvgMinA[i];
             if(LineChkA) LineChkA = 0;
             else LineCntA = 0;
          }
          else
          {
             LineCnt++;
             LineChkA = 1;
             if(++LineCntA > MaxLineCntA)
            {
               MaxLineCntA = LineCntA;
               LineCenterA = i - LineCntA/2;
            }
//             GLCD_bitmap(i*2, 230, LOGO_WIDTH, LOGO_HEIGHT, logo_WHITE);
          }
//         GLCD_bitmap( i*2, (uint16)whiteAvgA[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_BLUE);
//         GLCD_bitmap( i*2, cameraDataA[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_YELLOW);//카메라 확인 할 때 주석 해제
       }
       if(carMode == FINAL && LineCnt > 45)
       {
           dcGoalSpeed = 600;
    	   if(carState == RUN)
    	   {
    		   carState = SCHOOL_ZONE_WAIT;
        	   servoAngle = CENTER;
               FrontControl(servoAngle);
        	   continue;
    	   }
    	   else if(carState == SCHOOL_ZONE)
    	   {
    		   carState = RUN_WAIT;
        	   servoAngle = CENTER;
               FrontControl(servoAngle);
        	   continue;
    	   }
       }

       if(carMode == FINAL && carState == RUN_WAIT)
       {
    	   servoAngle = CENTER;
           FrontControl(servoAngle);
    	   if(LineCnt < 10)
    	   {
    		   carState = RUN;
            	LastLineCenterA = LineCenterA = LastLineCenterB = LineCenterB = 0;
            	LeftLine = RightLine = 1;
    	   }
    	 continue;
       }
       if(carMode == FINAL && carState == SCHOOL_ZONE_WAIT)
       {
    	   servoAngle = CENTER;
           FrontControl(servoAngle);
    	   if(LineCnt < 10)
    	   {
    		   carState = SCHOOL_ZONE;
            	LastLineCenterA = LineCenterA = LastLineCenterB = LineCenterB = 0;
            	LeftLine = RightLine = 1;
    	   }
    	   continue;
       }

		if(carState == SCHOOL_ZONE || carState == LINE_CHANGE)
		{
			   for(i = 40;i < 88;i++)
			   {
				   cameraDataB[i] = (cameraOutB[cameraNum][i]*2 + cameraOutB[cameraNum][i + 1] + cameraOutB[cameraNum][i - 1])/4;
				  if(cameraDataB[i] > whiteAvgB[i]*beta)
				  {
					 whiteAvgB[i] = whiteAvgB[i]*alpha + (double)cameraDataB[i]*(1 - alpha);
					 if(whiteAvgB[i] < whiteAvgMinB[i]) whiteAvgB[i] = whiteAvgMinB[i];
					 if(LineChkB) LineChkB = 0;
					 else LineCntB = 0;
				  }
				  else
				  {
					 LineChkB = 1;
					 if(++LineCntB > MaxLineCntB)
					 {
						MaxLineCntB = LineCntB;
						LineCenterB = i - LineCntB/2;
					 }
		//              GLCD_bitmap((3*i-128)*2, 220, LOGO_WIDTH, LOGO_HEIGHT, logo_WHITE);
				  }
		//         GLCD_bitmap( (3*i-128)*2, (uint16)whiteAvgB[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_RED);
		//         GLCD_bitmap( (3*i-128)*2, cameraDataB[i]/16, LOGO_WIDTH, LOGO_HEIGHT, logo_GREEN);
			   }
		}
#ifdef __debug_tungun__
        int windowSize=4;
        int mxp = windowSize;
        double mx = 0,my = 0;
        for(i=26+windowSize;i<112-windowSize;i++)
        {
           double sum = cameraDataA[i]/whiteAvgA[i];
           for(j=1;j<windowSize;j++)
           {
              sum +=cameraDataA[i-j]/whiteAvgA[i-j]+cameraDataA[i+j]/whiteAvgA[i+j];
           }
           double avg = sum/(windowSize*2);
           double v = (cameraDataA[i]/whiteAvgA[i]-avg)*(cameraDataA[i]/whiteAvgA[i]-avg);
           for(j=1;j<windowSize;j++)
           {
              v+=(cameraDataA[i-j]/whiteAvgA[i-j]-avg)*(cameraDataA[i-j]/whiteAvgA[i-j]-avg) + (cameraDataA[i+j]/whiteAvgA[i+j]-avg)*(cameraDataA[i+j]/whiteAvgA[i+j]-avg);
           }
           if(v>mx)
           {
              mx=v;
              mxp=i;
           }
        }
        LineCenterA = mxp;
        if(mx<0.4) LineCenterA = 0;
        mxp = windowSize;
        my = mx;
      mx=0;
      for(i=40+windowSize;i<88-windowSize;i++)
      {
         double sum = cameraDataB[i]/whiteAvgB[i];
         for(j=1;j<windowSize;j++)
         {
            sum +=cameraDataB[i-j]/whiteAvgB[i-j]+cameraDataB[i+j]/whiteAvgB[i+j];
         }
         double avg = sum/(windowSize*2);
         double v = (cameraDataB[i]/whiteAvgB[i]-avg)*(cameraDataB[i]/whiteAvgB[i]-avg);
         for(j=1;j<windowSize;j++)
         {
            v+=(cameraDataB[i-j]/whiteAvgB[i-j]-avg)*(cameraDataB[i-j]/whiteAvgB[i-j]-avg) + (cameraDataB[i+j]/whiteAvgB[i+j]-avg)*(cameraDataB[i+j]/whiteAvgB[i+j]-avg);
         }
         if(v>mx)
         {
            mx=v;
            mxp=i;
         }
      }
      LineCenterB = mxp;
        if(mx<0.4) LineCenterB = 0;

        usr_sprintf(text,"%d %d", (int)(mx*1000000), (int)(my*1000000));
        GLCD_displayStringLn(LINE1, text);
#endif

/*        if(!LastLineCenterA && !LastLineCenterB && (LineCenterA||LineCenterB))//라인 발견
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
        }*/

        switch(carState)
        {
        case RUN:
        	if(!LastLineCenterA && LineCenterA)//라인 발견
			{
				 if(LineCenterA < 64)
					 LeftLine--;
				 else
					 RightLine--;
			}

			if(LastLineCenterA && !LineCenterA)//라인 사라짐
			{
				  if(LastLineCenterA < 64)
					 LeftLine++;
				  else
					 RightLine++;
			}

			if(LeftLine >= 2)//조향
			{
			   servoAngle = LEFT;
			}
			else if(RightLine >= 2)
			{
			   servoAngle = RIGHT;
			}
			else if(LeftLine == RightLine)//line 없음
			{
			   servoAngle = CENTER;
			}
			else
			{
				if(LeftLine)//각도 조절 #수정
					servoAngle = CENTER - (RIGHT-LEFT)*(128-LineCenterA)/64;
				else
					servoAngle = CENTER + (RIGHT-LEFT)*LineCenterA/64;
			}
        break;
        case SCHOOL_ZONE:
        case LINE_CHANGE:
        	if(LineCenterA)
        		LineCenter = LineCenterA;
        	else if(LineCenterB)
        		LineCenter = 2*LineCenterB-64;
        	else
        		LineCenter = 0;

        	if(!LastLineCenter && LineCenter)//라인 발견
        	{
        		 if(LineCenter < 64)
        			 LeftLine--;
        		 else
        			 RightLine--;
        	}

        	if(LastLineCenter && !LineCenter)//라인 사라짐
        	{
        		  if(LastLineCenter < 64)
        			 LeftLine++;
        		  else
        			 RightLine++;
        	}

        	if(LeftLine >= 2)//조향
        	{
        	   servoAngle = LEFT;
        	}
        	else if(RightLine >= 2)
        	{
        	   servoAngle = RIGHT;
        	}
        	else if(LeftLine == RightLine)//line 없음
        	{
        	   servoAngle = CENTER;
        	}
        	else
        	{
        		if(LeftLine)//각도 조절 #수정
        			servoAngle = CENTER - (RIGHT-LEFT)*(128-LineCenter)/64;
        		else
        			servoAngle = CENTER + (RIGHT-LEFT)*LineCenter/64;
        	}
        	LastLineCenter = LineCenter;
        	break;
        case HILL:
        	LastLineCenterA = LineCenterA = 0;
        	LeftLine = RightLine = 1;
        	dcGoalSpeed = 1300;
        	servoAngle = CENTER;
            FrontControl(servoAngle);
            Delay_ms(1500);
           carState = RUN;
        	break;
        }

/*
        else
        {
         if(LineCenterA == 0)
        	 cameraXa = cameraXb;
         else
            cameraXa = 15*(LineCenterA-64)/48;

         if(LineCenterB == 0)
        	 cameraXb = cameraXa;
         else
            cameraXb = 15*(LineCenterB-64)/24;

			 if(cameraXa == cameraXb)
			 {
				 if(LeftLine)
				 {
					 if(cameraXa < 10)
						 servoAngle = LEFT;
					 else
						 servoAngle = CENTER;
				 }
				 else
				 {
					 if(cameraXa > -10)
						 servoAngle = RIGHT;
					 else
						 servoAngle = CENTER;
				 }
			 }
			 else if(cameraXa != cameraXb)
			 {
				 m = (cameraYa - cameraYb)/(cameraXa - cameraXb);
				 double cameraX = (cameraXa + cameraXb)/2, cameraY = (cameraYa + cameraYb)/2;
				 double LineX, LineY;
				 theta = atan(m);
				 if(LeftLine)
				 {
					 if(m > 0)
					 {
						 LineX = cameraX - 25*cos(PI/2-theta);
						 LineY = cameraY + 25*sin(PI/2-theta);
					 }
					 else
					 {
						 LineX = cameraX - 25*cos(theta-PI/2);
						 LineY = cameraY - 25*sin(theta-PI/2);
					 }
				 }
				 else
				 {
					 if(m > 0)
					 {
						 LineX = cameraX + 25*cos(PI/2-theta);
						 LineY = cameraY + 25*sin(PI/2-theta);
					 }
					 else
					 {
						 LineX = cameraX + 25*cos(theta-PI/2);
						 LineY = cameraY - 25*sin(theta-PI/2);
					 }
				 }

				 if(LineX > 0)
				 {
					 servoAngle = CENTER + (RIGHT-CENTER)*((PI/2-atan2(LineY,LineX))*180/PI)/20;
				 }
				 else
				 {
					 servoAngle = CENTER - (CENTER-LEFT)*((atan2(LineY,LineX)-PI/2)*180/PI)/20;
				 }
			 }
        }*/

        FrontControl(servoAngle);
//        makeTestData(cameraDataA, cameraDataB, servoAngle);
        switch(carState)
          {
          case STOP:
             dcGoalSpeed = 0;
             break;
          case RUN:
        	  dcGoalSpeed = 1200 - abs(servoAngle - CENTER);//속도 조절 조향 각도 클수록 감속 #수정
             if(distanceSensor[0] > 500)
            	 dcGoalSpeed -= 50;
             if(distanceSensor[0] > 750)
            	 dcGoalSpeed -= 75;
        	  break;
          case SCHOOL_ZONE:
          case LINE_CHANGE:
             dcGoalSpeed = 600;
             break;
          case HILL:
        	  dcGoalSpeed = 1300;
       	   break;
          }
        LastLineCenterA = LineCenterA;
        LastLineCenterB = LineCenterB;
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
// @Function       void UsrIsr_Stm_0(void)
// @Description      STM0 Interrupt for system tick generation
// @Returnvalue      None
// @Parameters       None
//*********************************************************************************************
IFX_INTERRUPT (SecondTimer_Isr, 0, ISR_PRIORITY_STM);
void SecondTimer_Isr(void)
{
    Ifx_STM *stm = &MODULE_STM0;
    char text[64];

    /* Set next 1ms scheduler tick alarm */
    IfxStm_updateCompare(stm, IfxStm_Comparator_0, IfxStm_getLower(stm) + 3000);

    initStartCount++;

   P00_OUT.B.P6 = !P00_OUT.B.P6;//camera CLK
   P00_OUT.B.P1 = !P00_OUT.B.P1;

   if(initStartCount > 260)
   {
      initStartCount = 0;
      P00_OUT.B.P8 = 1;
      P00_OUT.B.P6 = 1;
      cameraUpdateNum = (cameraUpdateNum?0:1);
   }
   else if(initStartCount < 256 && initStartCount%2)
   {
      Test_VadcAutoScan(IfxVadc_GroupId_0);
      Test_VadcAutoScan(IfxVadc_GroupId_1);
      cameraOutA[cameraUpdateNum][initStartCount/2] = Adc_Result_Scan[1][9];//AN21
      cameraOutB[cameraUpdateNum][initStartCount/2] = Adc_Result_Scan[1][8];//AN20

      distanceSensor[0] = Adc_Result_Scan[0][11];//AN11 위
      distanceSensor[1] = Adc_Result_Scan[1][3];//AN15 아래

      if(Adc_Result_Scan[1][4] > 3000)//AN16
      {
    	  carMode = FINAL;
      }
      else
      {
    	  carMode = NORMAL;
      }

      if(carMode == FINAL && distanceSensor[0] > 1000)//AEB
      {
          CarRuning(0, 1);
          dcGoalSpeed = 0;
          carState = STOP;
      }
      else if(carState == STOP && distanceSensor[0] < 1000)
      {
    	  carState = RUN;
      }
      if(carMode == FINAL && carState == RUN && distanceSensor[1] > 1600 && distanceSensor[0] < 800)
      {
    	  carState = HILL;
      }
      else if(carMode == FINAL && carState == SCHOOL_ZONE && distanceSensor[0] > 450 && distanceSensor[1] > 450)
      {
         carState = LINE_CHANGE;
         if(lineState == RIGHT)
         {
        	 lineState = LEFT;
            LeftLine++;
            RightLine--;
         }
         else
         {
        	 lineState = RIGHT;
            LeftLine--;
            RightLine++;
         }
      }
      else if(carMode == FINAL && carState == LINE_CHANGE && distanceSensor[0] < 350 && distanceSensor[1] < 350)
      {
         carState = SCHOOL_ZONE;
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
    IfxStm_updateCompare(stm, IfxStm_Comparator_1, IfxStm_getLower(stm) + 10000000);
    dcSpeed += (dcGoalSpeed-dcSpeed)/4;
    PID(gpt->T3.B.T3, dcSpeed);
    CarRuning(PID_encoder, 1);
/*	char text[32];
    usr_sprintf(text,"%d %d",dcSpeed, dcGoalSpeed);
    GLCD_displayStringLn(LINE1, text);*/
    P13_OUT.B.P0 = !P13_OUT.B.P0;

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

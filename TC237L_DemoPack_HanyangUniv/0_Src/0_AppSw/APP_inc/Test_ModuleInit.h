/*
 * Test_ModuleInit.h
 *
 *  Created on: 2015. 3. 19.
 *      Author: kimwayne
 */

#ifndef TEST_MODULEINIT_H_
#define TEST_MODULEINIT_H_


extern void Test_ModuleInit(void);
extern void Dio_Initialization(void);
extern void Pwm_Initialization(void);
extern void SecondTimer_Initialization(void);
extern void Icu_Initialization(void);
extern void TLF35584Demo_Init(void);

extern void Adc_Initialization(void);
extern void Can_Initialization(void);
extern void Uart_Initialization(void);

extern void TestTimer_Initialization(void);
extern void GLCD_init(void);

#endif /* TEST_MODULEINIT_H_ */

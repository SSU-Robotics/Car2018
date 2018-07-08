#ifndef __SERVO_H__
#define __SERVO_H__

#define PI 3.141592
#define CENTER          ((uint16)0x280U)
#define LEFT            ((uint16)0x233U)
#define RIGHT           ((uint16)0x2CCU)

int calculateAngle(double theta);
double servoCalc(int LeftLine,int RightLine, int LineCenterA, int LineCenterB);

#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int calculateAngle(double theta)
{
	theta = theta * PI / 180;
	return 25*sqrt(1/(tan(theta)*tan(theta)) + 1) > abs(m*(10 - 25 / tan(theta * PI / 180) + 25 + n)/sqrt(m*m + 1)));
}

double servoCalc(int LeftLine,int RightLine, int LineCenterA, int LineCenterB)
{
    int status;
    double servoAngle;
    double cameraA_X, cameraB_X;
    const double cameraA_Y = 50, cameraB_Y = 40;
    double angle;

    if(LeftLine == RightLine)
        servoAngle = CENTER;
    else if(LeftLine>=2)
        servoAngle = LEFT;
    else if(RightLine>=2)
        servoAngle = RIGHT;
    else{
        cameraA_X = 0.3125*LineCenterA-20;
        cameraB_X = 0.652*LineCenterB-40;
        angle = cameraA_Y/((cameraA_Y-cameraB_Y)/(cameraA_X-cameraB_X)) - cameraA_X;
    }

    status = LeftLine ? LeftLine : RightLine;
   
    if(abs(angle)<10) || calculateAngle(20))
        servoAngle = status;
    else if(calculateAngle(15))
        servoAngle = status + (CENTER - status)/4;
    else if(calculateAngle(10))
        servoAngle = status + (CENTER - status)/2;
    else if(calculateAngle(5))
        servoAngle = CENTER - (CENTER - status)/4;
    else   
        servoAngle = CENTER;

    return servoAngle;
}
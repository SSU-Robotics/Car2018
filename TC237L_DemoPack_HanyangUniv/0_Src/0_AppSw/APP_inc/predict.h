#ifndef __PREDICT_H__
#define __PREDICT_H__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "DNN.h"
#include "glcd.h"
#include "usr_sprintf.h"
void mergeArray(uint16 *cameraDataA, uint16* cameraDataB, uint16* mergedData);;
int predictServo(char *cameraDataA, char* cameraDataB);
int predictSchoolzone(char*cameraDataA, char* cameraDataB);
#endif
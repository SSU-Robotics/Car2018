#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "DNN.h"

#include "glcd.h"
#include "usr_sprintf.h"

int predict_main()
{
	int i, j;
	double trainX[4][2] = { { 0.000000, 0.000000 },{ 0.000000, 1.000000 },{ 1.000000, 0.000000 },{ 1.000000, 1.000000 } };
	double trainY[4] = { 0.000000, 1.000000, 1.000000, 0.000000 };
	double weight_h[5][3] = { { 3.874663, 2.308471, 2.308489 },{ 3.148705, -3.148770, 0.000022 },{ -0.884735, 0.055662, 0.982059 },{ 0.936721, -0.259317, 0.937430 },{ -3.399843, -3.399868, -3.399834 } };
	double weight_o[6] = { -3.638859, 3.015889, 0.374234, -0.060950, -4.174889, -2.817884 };
	double hi[6] = { 1.937239, 0.000026, -0.009056, -0.001300, -0.016999, 0.000000 };
	char text[128];
	double test[4][3] = { { 0,0,0 },{ 0,1,1 },{ 1,0,1 },{ 1,1,0 } };
	double o;
	int testNum = 4;
	for (i = 0; i <testNum; ++i) {
		usr_sprintf(text, "%d> ", i);
		Uart_Transmit(text);
		for (j = 0; j < INPUTNO; ++j)
		{
			usr_sprintf(text,"%d ", (int)test[i][j]);
			Uart_Transmit(text);
		}
		usr_sprintf(text,"\n| answer : %d | ", (int)(test[i][INPUTNO]*1000000));
		Uart_Transmit(text);
		o = predict(weight_h, weight_o, hi, test[i]);
		usr_sprintf(text, "predict : %d |\n\n", (int)(o*1000000));
		Uart_Transmit(text);
	}
}

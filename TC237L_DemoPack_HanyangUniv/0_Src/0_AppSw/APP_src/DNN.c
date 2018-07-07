#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "DNN.h"
#include "activateFunc.h"
void hlearn(double weight_h[HIDDENNO][INPUTNO + 1], double weight_o[HIDDENNO + 1], double hi[], double trainX[INPUTNO], double trainY, double o)
{
	int i, j;
	double d;

	for (j = 0; j < HIDDENNO; ++j) {
		//d = hi[j] * (1 - hi[j])*weight_o[j] * (trainY - o)*o*(1 - o);		// sigmoiod
		d = drelu(hi[j])*weight_o[j] * (trainY - o)*drelu(o);				// relu
		for (i = 0; i < INPUTNO; ++i)
			weight_h[j][i] += ALPHA*trainX[i] * d;
		weight_h[j][i] += ALPHA*(-1.0)*d;									// ��ġ �н�
	}
}

int getdata(double trainX[MAXINPUTNO][INPUTNO], double trainY[MAXINPUTNO])
{
	int trainNum = 0;/*������ ��Ʈ ����*/
	int i = 0;
	double tmp;
	while (scanf("%lf", &tmp) != EOF)
	{
		trainX[trainNum][0] = tmp;
		for (i = 1; i < INPUTNO; i++)
			scanf("%lf", &trainX[trainNum][i]);
		scanf("%lf", &trainY[trainNum]);
		++trainNum;
	}
	return trainNum;
}

void olearn(double wo[HIDDENNO + 1], double hi[], double trainY, double o)
{
	int i;
	double d;

	//d = (trainY - o)*o*(1 - o);											// sigmoid
	d = (trainY - o)*drelu(o);												// relu
	for (i = 0; i < HIDDENNO; ++i)
		wo[i] += ALPHA*hi[i] * d;
	wo[i] += ALPHA*(-1.0)*d;
}

double predict(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1], double hi[], double trainX[INPUTNO])
{
	int i, j;
	double u;/*����ġ�� ������ �� ���*/
	double o;/*��� ���*/
	/*hi ���*/
	for (i = 0; i < HIDDENNO; ++i) {
		u = 0;/*����ġ�� ������ �� ���ϱ�*/
		for (j = 0; j < INPUTNO; ++j)
			u += trainX[j] * wh[i][j];
		u -= wh[i][j];/*��ġ ó��*/
		u /= INPUTNO;
		hi[i] = relu(u);
		//hi[i] = sigmoid(u);
	}
	/*��� o ���*/
	o = 0;
	for (i = 0; i < HIDDENNO; ++i) {
		o += hi[i] * wo[i];
	}
	o -= wo[i];/*��ġ ó��*/
	return sigmoid(o);
}

void print(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1])
{
	int i, j;

	for (i = 0; i < HIDDENNO; ++i)
		for (j = 0; j < INPUTNO + 1; ++j)
			printf("%lf ", wh[i][j]);
	printf("\n");
	for (i = 0; i < HIDDENNO + 1; ++i)
		printf("%lf ", wo[i]);
	printf("\n");
}

void initwh(double wh[HIDDENNO][INPUTNO + 1])
{
	int i, j;
	for (i = 0; i < HIDDENNO; ++i)
		for (j = 0; j < INPUTNO + 1; ++j)
			wh[i][j] = rnd();
}

void initwo(double wo[HIDDENNO + 1])
{
	int i;
	for (i = 0; i < HIDDENNO + 1; ++i)
		wo[i] = rnd();
}

double rnd(void)
{
	double rndno;/*������ ����*/

	while ((rndno = (double)rand() / RAND_MAX) == 1.0 || rndno == 0.5);
	rndno = rndno * 2 - 1;/*-1���� 1 ������ ���� ����*/
	return rndno;
}


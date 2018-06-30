#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "BPNN.h"

/**************************/
/*  hlearn() �Լ�         */
/*  �߰��� ����ġ�� �н�  */
/**************************/

void hlearn(double wh[HIDDENNO][INPUTNO + 1],
	double wo[HIDDENNO + 1],
	double hi[], double e[INPUTNO + 1], double o)
{
	int i, j;/*�ݺ� ����*/
	double dj;/*�߰��� ����ġ ��꿡 �̿�*/

	for (j = 0; j < HIDDENNO; ++j) {/*�߰��� �� �� j�� �������*/
		dj = hi[j] * (1 - hi[j])*wo[j] * (e[INPUTNO] - o)*o*(1 - o);
		for (i = 0; i < INPUTNO; ++i)/*i��° ����ġ ó��*/
			wh[j][i] += ALPHA*e[i] * dj;
		wh[j][i] += ALPHA*(-1.0)*dj;/*��ġ �н�*/
	}
}

/*************************/
/*  getdata() �Լ�       */
/* �н� ������ �о���̱�*/
/*************************/
int getdata(double e[][INPUTNO + 1])
{
	int n_of_e = 0;/*������ ��Ʈ ����*/
	int j = 0;/*�ݺ� �����*/

			  /*������ �Է�*/
	while (scanf("%lf", &e[n_of_e][j]) != EOF) {
		++j;
		if (j > INPUTNO) {/*���� �ε�����*/
			j = 0;
			++n_of_e;
		}
	}
	return n_of_e;
}

/*************************/
/* olearn() �Լ�         */
/* ������� ����ġ �н�  */
/*************************/
void olearn(double wo[HIDDENNO + 1]
	, double hi[], double e[INPUTNO + 1], double o)
{
	int i;/*�ݺ� ����*/
	double d;/*����ġ ��꿡 �̿�*/

	d = (e[INPUTNO] - o)*o*(1 - o);/*���� ���*/
	for (i = 0; i < HIDDENNO; ++i) {
		wo[i] += ALPHA*hi[i] * d;/*����ġ �н�*/
	}
	wo[i] += ALPHA*(-1.0)*d;/*��ġ �н�*/
}

/**********************/
/*  forward() �Լ�    */
/*  ������ ���       */
/**********************/
double forward(double wh[HIDDENNO][INPUTNO + 1],
	double wo[HIDDENNO + 1], double hi[],
	double e[INPUTNO + 1])
{
	int i, j;/*�ݺ� ����*/
	double u;/*����ġ�� ������ �� ���*/
	double o;/*��� ���*/

			 /*hi ���*/
	for (i = 0; i < HIDDENNO; ++i) {
		u = 0;/*����ġ�� ������ �� ���ϱ�*/
		for (j = 0; j < INPUTNO; ++j)
			u += e[j] * wh[i][j];
		u -= wh[i][j];/*��ġ ó��*/
		hi[i] = f(u);
	}
	/*��� o ���*/
	o = 0;
	for (i = 0; i < HIDDENNO; ++i)
		o += hi[i] * wo[i];
	o -= wo[i];/*��ġ ó��*/

	return f(o);
}

/**********************/
/*   print() �Լ�     */
/*   ��� ���        */
/**********************/
void print(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1])
{
	int i, j;/*�ݺ� ����*/

	for (i = 0; i < HIDDENNO; ++i)
		for (j = 0; j < INPUTNO + 1; ++j)
			printf("%lf ", wh[i][j]);
	printf("\n");
	for (i = 0; i < HIDDENNO + 1; ++i)
		printf("%lf ", wo[i]);
	printf("\n");
}

/************************/
/*    initwh() �Լ�     */
/*�߰��� ����ġ�� �ʱ�ȭ*/
/************************/
void initwh(double wh[HIDDENNO][INPUTNO + 1])
{
	int i, j;/*�ݺ� ����*/

			 /*������ �̿��� ����ġ ����*/
	for (i = 0; i < HIDDENNO; ++i)
		for (j = 0; j < INPUTNO + 1; ++j)
			wh[i][j] = drnd();
}

/************************/
/*    initwo() �Լ�     */
/*����� ����ġ�� �ʱ�ȭ*/
/************************/
void initwo(double wo[HIDDENNO + 1])
{
	int i;/*�ݺ� ����*/

		  /*������ �̿��� ����ġ ����*/
	for (i = 0; i < HIDDENNO + 1; ++i)
		wo[i] = drnd();
}

/*******************/
/* drnd() �Լ�     */
/* ���� ����       */
/*******************/
double drnd(void)
{
	double rndno;/*������ ����*/

	while ((rndno = (double)rand() / RAND_MAX) == 1.0);
	rndno = rndno * 2 - 1;/*-1���� 1 ������ ���� ����*/
	return rndno;
}

/*******************/
/* f() �Լ�        */
/* ��ȯ �Լ�       */
/*(�ñ׸��̵� �Լ�)*/
/*******************/
double f(double u)
{
	return 1.0 / (1.0 + exp(-u));
}

#ifndef __DNN_H__
#define __DNN_H__

#define INPUTNO 2			/*�Է��� �� ����*/ 
#define HIDDENNO 5			/*�߰��� �� ����*/ 
#define ALPHA  0.001		/*�н� ���*/ 
#define SEED 65535			/*���� �õ�*/ 
#define MAXINPUTNO 4		/*�н� �������� �ִ� ����*/ 
#define BIGNUM 100			/*������ �ʱ갪*/ 
#define LIMIT 0.001			/*������ �ʱ갪*/ 


void initwh(double wh[HIDDENNO][INPUTNO + 1]);
void initwo(double wo[HIDDENNO + 1]);

double rnd(void);/*[-1,1]-{0}���� ����*/
void print(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1]);
double predict(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1], double hi[], double trainX[INPUTNO]);

void olearn(double wo[HIDDENNO + 1], double hi[], double trainY, double o);
void hlearn(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1], double hi[], double trainX[INPUTNO], double trainY, double o); /*�߰��� ����ġ�� ����*/
int getdata(double trainX[MAXINPUTNO][INPUTNO], double trainY[MAXINPUTNO]);

#endif
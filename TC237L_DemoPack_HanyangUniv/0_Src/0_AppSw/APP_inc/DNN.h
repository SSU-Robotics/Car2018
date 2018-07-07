#ifndef __DNN_H__
#define __DNN_H__

#define INPUTNO 2			/*입력층 셀 개수*/ 
#define HIDDENNO 5			/*중간층 셀 개수*/ 
#define ALPHA  0.001		/*학습 계수*/ 
#define SEED 65535			/*난수 시드*/ 
#define MAXINPUTNO 4		/*학습 데이터의 최대 개수*/ 
#define BIGNUM 100			/*오차의 초깃값*/ 
#define LIMIT 0.001			/*오차의 초깃값*/ 


void initwh(double wh[HIDDENNO][INPUTNO + 1]);
void initwo(double wo[HIDDENNO + 1]);

double rnd(void);/*[-1,1]-{0}난수 생성*/
void print(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1]);
double predict(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1], double hi[], double trainX[INPUTNO]);

void olearn(double wo[HIDDENNO + 1], double hi[], double trainY, double o);
void hlearn(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1], double hi[], double trainX[INPUTNO], double trainY, double o); /*중간층 가중치의 조정*/
int getdata(double trainX[MAXINPUTNO][INPUTNO], double trainY[MAXINPUTNO]);

#endif
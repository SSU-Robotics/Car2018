#ifndef __BPNN_H__
#define __BPNN_H__
/*기호 상수 정의*/
#define INPUTNO 7  /*입력층 셀 개수*/ 
#define HIDDENNO 3  /*중간층 셀 개수*/ 
#define ALPHA  10  /*학습 계수*/ 
#define SEED 65535    /*난수 시드*/ 
#define MAXINPUTNO 100    /*학습 데이터의 최대 개수*/ 
#define BIGNUM 100    /*오차의 초깃값*/ 
#define LIMIT 0.001    /*오차의 초깃값*/ 

/*함수 프로토타입 선언*/
double f(double u); /*전환 함수(시그모이드 함수)*/
void initwh(double wh[HIDDENNO][INPUTNO + 1]);
/*중간층 가중치의초기화*/
void initwo(double wo[HIDDENNO + 1]);/*출력층 가중치의 초기화*/
double drnd(void);/* 난수 생성     */
void print(double wh[HIDDENNO][INPUTNO + 1]
	, double wo[HIDDENNO + 1]); /*결과 출력*/
double forward(double wh[HIDDENNO][INPUTNO + 1]
	, double wo[HIDDENNO + 1], double hi[]
	, double e[INPUTNO + 1]); /*순방향 계산*/
void olearn(double wo[HIDDENNO + 1], double hi[]
	, double e[INPUTNO + 1], double o); /*출력층 가중치의 조정*/
int getdata(double e[][INPUTNO + 1]); /*학습 데이터 읽어들이기*/
void hlearn(double wh[HIDDENNO][INPUTNO + 1]
	, double wo[HIDDENNO + 1], double hi[]
	, double e[INPUTNO + 1], double o); /*중간층 가중치의 조정*/

#endif
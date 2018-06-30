#ifndef __BPNN_H__
#define __BPNN_H__
/*��ȣ ��� ����*/
#define INPUTNO 7  /*�Է��� �� ����*/ 
#define HIDDENNO 3  /*�߰��� �� ����*/ 
#define ALPHA  10  /*�н� ���*/ 
#define SEED 65535    /*���� �õ�*/ 
#define MAXINPUTNO 100    /*�н� �������� �ִ� ����*/ 
#define BIGNUM 100    /*������ �ʱ갪*/ 
#define LIMIT 0.001    /*������ �ʱ갪*/ 

/*�Լ� ������Ÿ�� ����*/
double f(double u); /*��ȯ �Լ�(�ñ׸��̵� �Լ�)*/
void initwh(double wh[HIDDENNO][INPUTNO + 1]);
/*�߰��� ����ġ���ʱ�ȭ*/
void initwo(double wo[HIDDENNO + 1]);/*����� ����ġ�� �ʱ�ȭ*/
double drnd(void);/* ���� ����     */
void print(double wh[HIDDENNO][INPUTNO + 1]
	, double wo[HIDDENNO + 1]); /*��� ���*/
double forward(double wh[HIDDENNO][INPUTNO + 1]
	, double wo[HIDDENNO + 1], double hi[]
	, double e[INPUTNO + 1]); /*������ ���*/
void olearn(double wo[HIDDENNO + 1], double hi[]
	, double e[INPUTNO + 1], double o); /*����� ����ġ�� ����*/
int getdata(double e[][INPUTNO + 1]); /*�н� ������ �о���̱�*/
void hlearn(double wh[HIDDENNO][INPUTNO + 1]
	, double wo[HIDDENNO + 1], double hi[]
	, double e[INPUTNO + 1], double o); /*�߰��� ����ġ�� ����*/

#endif
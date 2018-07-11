// 학습용 코드입니다. 보드에서 실행하면 안 됩니다

#ifdef __COMPUTER__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "DNN.h"
#include "activateFunc.h"
#include <signal.h>
int state = 1;
void(*oldfunc)(int);
void handler(int signo)
{
	state = 0;
	signal(SIGINT, SIG_DFL);
}
int main(int argc, char* argv[])
{
	freopen("rand.txt", "r", stdin);
	FILE *fout = fopen("output.txt", "w");
	fclose(fout);
	double weight_h[HIDDENNO][INPUTNO + 1] = { 0 }, weight_o[HIDDENNO + 1] = { 0 };
	double trainX[MAXINPUTNO][INPUTNO] = { 0 }, trainY[MAXINPUTNO] = { 0 };
	double hi[HIDDENNO + 1] = { 0 };
	double o;
	double err = BIGNUM, preverr = BIGNUM;
	int i, j;
	int inputNum;
	int count = 0;
	srand(SEED);
	oldfunc = signal(SIGINT, handler);
	initwh(weight_h);
	initwo(weight_o);
	//print(weight_h, weight_o);

	inputNum = getdata(trainX, trainY);
	printf("학습 데이터 개수:%d\n", inputNum);

	//while (err > LIMIT || preverr==err) {
	while (state) {
		preverr = err;
		err = 0.0;
		for (j = 0; j < inputNum; ++j) {
			o = predict(weight_h, weight_o, hi, trainX[j]);
			olearn(weight_o, hi, trainY[j], o);
			hlearn(weight_h, weight_o, hi, trainX[j], trainY[j], o);
			err += (o - trainY[j])*(o - trainY[j]);
		}
		err /= inputNum;
		++count;
		printf("%d\t%lf\n", count, err);
	}

	printf("------------------------------------\n");

	saveArrD2("double", "weight_h", HIDDENNO, INPUTNO + 1, weight_h);
	saveArrD1("double", "weight_o", HIDDENNO + 1, weight_o);

	fcloseall();
	return 0;
}
#endif
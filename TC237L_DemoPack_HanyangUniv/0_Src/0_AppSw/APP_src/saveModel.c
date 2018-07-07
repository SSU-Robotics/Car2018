/////////////////////////////
/*      학습모델 저장      */
/////////////////////////////
#include <stdio.h>
#include <stdlib.h>

void saveArrD1(char *tpName, char* arrayName, int d, double* arr)
{
	FILE *fout = fopen("output.txt", "a+");
	fprintf(fout, "%s %s[%d] = { ", tpName, arrayName, d);
	int i, j;
	for (i = 0; i < d; i++) {
		fprintf(fout, "%lf", arr[i]);
		if (i != d - 1) fprintf(fout, ", ");
	}
	fprintf(fout, "};\n");
}
void saveArrD2(char *tpName, char* arrayName, int d1, int d2, double *arr)
{
	FILE *fout = fopen("output.txt", "a+");
	fprintf(fout, "%s %s[%d][%d] = { ", tpName, arrayName, d1, d2);
	int i, j;
	for (i = 0; i < d1; i++)
	{
		fprintf(fout, "{ ");
		for (j = 0; j < d2; j++) {
			fprintf(fout, "%lf", *((arr + i*d2) + j));
			if (j != d2 - 1) fprintf(fout, ", ");
		}
		fprintf(fout, "}");
		if (i != d1 - 1) fprintf(fout, ", ");
	}
	fprintf(fout, "};\n");
}

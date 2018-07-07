/////////////////////////////
/*      활성화 함수        */
/////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double sigmoid(double u)
{
	return 1.0 / (1.0 + exp(-u));
}
double relu(double u)
{
	return (u <= 0.0 ? 0.01*u : u);
}
double drelu(double u)
{
	return (u <= 0 ? 0.01 : 1);
}
double dsigmoid(double u)
{
	return u*(1 - u);
}
// FSD Power Law.cpp : This file contains the 'main' function. Program execution begins and ends there.

//

#include "PowerLaw.h"
#include <math.h>
#include <stdio.h>
#include <conio.h> 

bool PowerLaw(std::vector<double> x, std::vector<double> y, double *retSlope, double *retIntercept)
{
	if (x.size() <= 0 || y.size() <= 0 || x.size() != y.size())
		return false;
	int i;
	double xySum = 0.0, yySum = 0.0, xxSum = 0.0, sx = 0.0, sy = 0.0;

	double slope, intercept, nd;

	std::vector<double> logX;
	std::vector<double>logYplus1;
	std::vector<double>::iterator it;
	for (it = x.begin(); it != x.end(); ++it)
	{
		logX.push_back(log(*it));
	}
	for (it = y.begin(); it != y.end(); ++it)
	{
		logYplus1.push_back(log(*it + 1));
	}

	/*int n = x.size();
	for (i = 0; i < n; i++)
	{
		x[i] = log(x[i]);
		y[i] = log(y[i] + 1); //can't have zeroes in bins
	}*/




	int n = logX.size();
	for (i = 0; i < n; i++)
	{
		sx = sx + logX[i];
		sy = sy + logYplus1[i];
		xxSum += pow2(logX[i]);
		//xxSum = xxSum + (logX[i] * logX[i]);
		yySum += pow2(logYplus1[i]);
		//yySum = yySum + (logYplus1[i] * logYplus1[i]);
		xySum = xySum + (logX[i] * logYplus1[i]);
	}

	nd = double(n);
	slope = (xySum - (sx * sy / nd)) / (xxSum - (pow2(sx)) / nd);
	intercept = (sy / nd) - slope * (sx / nd);
	*retSlope = slope;
	*retIntercept = intercept;
	return true;
}


/*int main(int argc, char *argv[])
{
	const int n = 3;
	std::vector<double> x = { 1.0, 2.0, 3.0 };
	std::vector<double> y = { 68.0, 2.0, 1.0 };

	double slope, intercept;
	bool status = PowerLaw(x, y, &slope, &intercept);
	if (status)
		printf("Slope = %lf\nIntercept = %lf\n", slope, intercept);
	else
		printf("PowerLaw returned failure\n");

	printf("\n\nPress a key to exit");
	char c = _getch();
}*/






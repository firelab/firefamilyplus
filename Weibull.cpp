// Weibull Fitting.cpp : This file contains the 'main' function. Program execution begins and ends there.

//

// weibull_distribution

#include <iostream>
#include <random>
#include <math.h>
#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <functional>
#include <vector>
#include <numeric>

using namespace std;


double avg(vector<int> score);

double var(vector<int> score, double mean);

//double sd(double* score, int size);

extern double pow2(double x);

double weibfun(double b, double varx, double xbar);

double brents_fun_weib(double lower, double upper, double tol, unsigned int max_iter, double mx, double vx);

//double tempfun(double b, double vp, double mp);



void CalcWeibullParams(vector<int> obsVals, double *ahat, double *bhat);
void CalcWeibullParams(vector<int> obsVals, double *ahat, double *bhat)
{
	*bhat = 0.0;
	*ahat = 0.0;
	if (obsVals.size() <= 0)
		return;
	double mp = avg(obsVals);
	double vp = var(obsVals, mp);
	*bhat = brents_fun_weib(0.1, 100, 1e-5, 1000, mp, vp);
	if(*bhat != 0.0)
		*ahat = mp / tgamma(1 + 1 / *bhat);
}



int fakemain()

{

	const int nrolls = 10000;  // number of experiments (wind observations 

	//const int nstars = 100;    // maximum number of stars to distribute





	std::default_random_engine generator;

	std::weibull_distribution<double> distribution(2.0, 4.0);



	int p[10] = {};

	double pnum[nrolls] = {}, ahat, bhat, mp, sp, vp;



	for (int i = 0; i < nrolls; ++i) {

		double number = distribution(generator);

		pnum[i] = number;

		if (number < 10) ++p[int(number)];

	}



	//mp = avg(pnum, nrolls);

	//sp = sd(pnum, nrolls);

	//vp = pow2(sp);





	//bhat = brents_fun_weib(1, 1000, 1e-5, 1000, mp, vp);

	ahat = mp / tgamma(1 + 1 / bhat);



	//std::cout << "weibull_distribution (2.0,4.0):" << std::endl;



	//for (int i = 0; i < 10; ++i) {

	//     std::cout << i << "-" << (i + 1) << ": ";

	//     std::cout << std::string(p[i] * nstars / nrolls, '*') << std::endl;

	//}

	std::cout << ahat << bhat << std::endl;



	return 0;

}



double avg(vector<int> score)

{
	if (score.size() <= 0)
		return 0.0;
	double sum = std::accumulate(score.begin(), score.end(), 0.0);
	return sum / (double)score.size();
}





double tempfun(double b, double vp, double mp)

{

	double outval;
	outval = weibfun(b, vp, mp);
	return(outval);
}

double var(vector<int> score, double mean)

{

	double tempsum = 0.0;

	int i;
	if (score.size() <= 0)
		return 0.0;

	double tmp;
	for (std::vector<int>::iterator it = score.begin(); it != score.end(); ++it)
	{
		tmp = *it;
		tempsum += pow2(tmp - mean);
	}
	tempsum /= (double)(score.size() - 1);
	return tempsum;

}

//var("b"); f(b) = gamma(1 + 2 / b) / gamma(1 + 1 / b) ^ 2 - 1 - varx / xbar ^ 2

double weibfun(double b, double varx, double xbar)

{
	double fb;
	fb = tgamma(1.0 + 2.0 / b) / pow2(tgamma(1.0 + 1.0 / b)) - 1.0 - varx / pow2(xbar);
	return(fb);
}







double brents_fun_weib(double lower, double upper, double tol, unsigned int max_iter, double mx, double vx)

{

	double a = lower;
	double b = upper;
	double fa = weibfun(a, vx, mx);   // calculated now to save function calls
	double fb = weibfun(b, vx, mx);   // calculated now to save function calls
	double fs = 0;             // initialize



	if (!(fa * fb < 0))
	{
		std::cout << "Signs of f(lower_bound) and f(upper_bound) must be opposites" << std::endl; // throws exception if root isn't bracketed
		return -11;
	}



	if (std::abs(fa) < std::abs(b))   // if magnitude of f(lower_bound) is less than magnitude of f(upper_bound)
	{
		std::swap(a, b);
		std::swap(fa, fb);
	}

	double c = a;              // c now equals the largest magnitude of the lower and upper bounds
	double fc = fa;                   // precompute function evalutation for point c by assigning it the same value as fa
	bool mflag = true;         // boolean flag used to evaluate if statement later on
	double s = 0;              // Our Root that will be returned
	double d = 0;              // Only used if mflag is unset (mflag == false)

	for (unsigned int iter = 1; iter < max_iter; ++iter)
	{
		// stop if converged on root or error is less than tolerance
		if (std::abs(b - a) < tol)
		{
			//std::cout << "After " << iter << " iterations the root is: " << s << std::endl;
			return s;
		} // end if

		if (fa != fc && fb != fc)
		{
			// use inverse quadratic interopolation
			s = (a * fb * fc / ((fa - fb) * (fa - fc)))
				+ (b * fa * fc / ((fb - fa) * (fb - fc)))
				+ (c * fa * fb / ((fc - fa) * (fc - fb)));
		}
		else
		{
			// secant method
			s = b - fb * (b - a) / (fb - fa);
		}



		// checks to see whether we can use the faster converging quadratic && secant methods or if we need to use bisection
		if (((s < (3 * a + b) * 0.25) || (s > b)) ||
			(mflag && (std::abs(s - b) >= (std::abs(b - c) * 0.5))) ||
			(!mflag && (std::abs(s - b) >= (std::abs(c - d) * 0.5))) ||
			(mflag && (std::abs(b - c) < tol)) ||
			(!mflag && (std::abs(c - d) < tol)))
		{
			// bisection method
			s = (a + b) * 0.5;
			mflag = true;
		}
		else
		{
			mflag = false;
		}
		fs = weibfun(s, vx, mx);   // calculate fs
		d = c;        // first time d is being used (wasnt used on first iteration because mflag was set)
		c = b;        // set c equal to upper bound
		fc = fb;      // set f(c) = f(b)
		if (fa * fs < 0)     // fa and fs have opposite signs
		{
			b = s;
			fb = fs;      // set f(b) = f(s)
		}
		else
		{
			a = s;
			fa = fs;      // set f(a) = f(s)
		}
		if (std::abs(fa) < std::abs(fb)) // if magnitude of fa is less than magnitude of fb
		{
			std::swap(a, b);           // swap a and b
			std::swap(fa, fb);   // make sure f(a) and f(b) are correct after swap
		}
	} // end for
	//std::cout << "The solution does not converge or iterations are not sufficient" << std::endl;
	return 0.0;
} // end brents_fun



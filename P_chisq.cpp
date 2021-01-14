//p_chisq.cpp
//calculates the probability for inputs chi-square and df.
//Stuart Brittain	MDFWP
// Thank you, Numerical Recipes in C

#include "stdafx.h"
#include <math.h>
#include "p_chisq.h"

#define ITMAX 200
#define EPS		3.0e-7

//function prototypes
double gcf(double a,double x);
double gser(double a,double x);
double gammln(double xx);

pval::pval()
{
//	nvals = _nvals;
	p = 0.0;
	hit = false;
	for(int i = 0; i < MAXPARAMS; i++)
		vals[i] = 0.0;
}


double p_chisq(double a, double x)
{
	if(x < (a+1.0)) 
		return (1.0 - gser(a,x));
	else 
		return (gcf(a,x));
}


double gammln( double xx )
{
	double x, tmp, ser;
	static double cof[6]={76.18009173,-86.50532033,24.01409822,
		-1.231739516,0.120858003e-2,-0.536382e-5};
	int j;

	x = xx - 1.0;
	tmp = x + 5.5;
	tmp -= (x + 0.5) * log(tmp);
	ser = 1.0;
	for(j=0;j<=5;j++)
	{
		x += 1.0;
		ser += cof[j] / x;
	}
	return -tmp + log(2.50662827465 * ser);
}

double gser(double a,double x)
{
	int n;
	double sum,del,ap,gln;

	gln = gammln(a);
	if(x <= 0.0) return(0.0);
	else
	{
		ap = a;
		del = sum = 1.0 / a;
		for(n=1;n<=ITMAX;n++)
		{
			ap += 1.0;
			del *= x/ap;
			sum += del;
			if(fabs(del) < fabs(sum)*EPS)
				return( sum*exp(-x+a*log(x)-gln));
		}
		return 21161.11;
	}
}

double gcf(double a, double x)
{
	int n;
	double gln, gold=0.0,g,fac=1.0,b1=1.0,b0=0.0,anf,ana,an,a1,a0=1.0;

	gln = gammln(a);
	a1=x;
	for(n=1;n<=ITMAX;n++)
	{
		an = (double)n;
		ana=an-a;
		a0=(a1+a0*ana)*fac;
		b0=(b1+b0*ana)*fac;
		anf=an*fac;
		a1=x*a0+anf*a1;
		b1=x*b0+anf*b1;
		if(a1)
		{
			fac=1.0/a1;
			g=b1*fac;
			if(fabs((g-gold)/g) < EPS)
				return(exp(-x+a*log(x)-gln)*g);
			gold=g;
		}
	}
	return 21161.11;
}

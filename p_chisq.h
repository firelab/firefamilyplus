//returns a chisq

double p_chisq(double a, double x);

#define MAXPARAMS 1

class pval
{
public:
	pval();
	double p;
//	int nvals;
	double vals[MAXPARAMS];
	bool hit;
};

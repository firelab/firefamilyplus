//LogReg.h

const int maxvars   =    31; //(* MAXIMUM NUMBER of VARIABLES *)
//const int bytesreal =     8; //(* bytes IN A REAL NUMBER *)const double missing = -8.76543E-37;
//const double missing = -8.76543E-37;
const double DTOOSMALL = 1.0E-08;
const int MAXREGS   =   30;//30;// (* MAXIMUM NUMBER of INDEPENDENT VARIABLES *)
const int MAXPLUS1  =   32;//31; //(* MAXREGS + 1 *)
const int MAXPLUS2  =   33;//32;// (* MAXREGS + 2 *)
const int DIMAUGU   =  529;//528; //(* (SQR (MAXREGS + 2) + (MAXREGS + 2)) / 2 *)
const int DIMXTXI   = 497;//496; //(* SQR (MAXREGS + 1) + (MAXREGS + 1)) / 2 *)



class  designtype 
{
public:
    long yindex;//     : LongInt;   (* DEPENDENT VARIABLE INDEX *)
    long xindex[maxvars];//     : colindex;  (* INDEPENDENT VARIABLE INDECIES *)
    long lindex;//     : LongInt;   (* OPTIONAL VARIABLE for LOGISTIC REG *)
    double logconst;//   : Double;    (* OPTIONAL INPUT VALUE for LOGISTIC REG *)
    long ftindex;//    : LongInt;   (* OPTIONAL VARIABLE for GLMS; FIXED TERM *)
    long nx;//         : LongInt;   (* NUMBER of INDEPENDENT VARIABLES *)
    long nf;//         : LongInt;   (* NUMBER of FIXED VARS for PARCOR and ALLREGS *)
    long fullnx;//     : LongInt;   (* nx BEFORE ANY X VARS WERE DROPPED *)
    long wtindex;//    : LongInt;   (* WEIGHTING VARIABLE INDEX *)
    bool constant;//   : Boolean;   (* SET if THE constant IS PART of MODEL *)
    long ordconst;//   : LongInt;   (* SET to 1 if MEAN IS USED *)
    long dimaiugxrow;// : LongInt;   (* DIMENSION of AUGMENTED ROW = CON + Y + X *)
    long npars;//      : LongInt;   (* NUMBER of PARAMETERS INCLUDING THE constant *)
    long nxy;//        : LongInt;   (* NUMBER of DEP and INDEP VARIABLES *)
    long goodcases;//  : LongInt;   (* NUMBER of CASES USED IN REGRESSION *)
    long itcount;//    : LongInt;   (* ITERATION COUNTER for GLM *)
    long maxints;//     : LongInt;   (* MAXIMUM NO. of ITERATIONS ALLOWED *)
    double devdiff;//    : Double;    (* ITERATION TERMINATION CRITERIAN *)
    double deviance;//   : Double;    (* deviance for LOGIST and POISSON *)
    double beta[MAXPLUS1];//      : betavec;   (* COEFFICIENTS, SET IN FIRST ITERATION *)
};


class  regsumtype
{
public:
    long DF;//         : LongInt;   (* DEGREES of FREEDOM IN REGS MODEL *)
    double fpvalue;//    : Double;    (* P VALUE of deviance *)
    double mse;//        : Double;
    double stderror[MAXPLUS1]; //  : betavec;   (* STANDARD ERRORS of COEFICIENTS *)
    double tpvalue[MAXPLUS1];//    : betavec;   (* P VALUES of BETAS *)
    double scale;//      : Double;    (* scale PARAMETER for GLM'S *)
    double sst;//        : Double;
};

class LogReg
{
public:
	LogReg();
	~LogReg();
	int DoLogReg(int nxx, int cases, double *p_y, double *p_x, double *coef, double *se, double *t, double *p, double *dev, double *pdev);
	void NextCase (designtype *design, int c, double *wt, double *nlogit, double *fixedterm, double *augxrow);
	double LN0(double X);
	void GLMStats (double nlogit, double fixedterm, designtype *design,
		double *augxrow, double *GLMWT, double *YHAT,
		bool dodev, bool *OK);
	void FindU(designtype *design, double *AUGU, bool *OK);
	void SingularTest(designtype *design, double *AUGU, bool *OK);
	void TestSmallMSE(double SST, double MSE, bool *OK);
	void RegSummary (double *augu, designtype *design, regsumtype *regsum, double *XTXI, double *AUGU, bool *OK);
	void CalcRegs (designtype *design, double *XTXI, regsumtype *regsum, bool *OK);
	void MultRegs();
	void RegsTables (designtype *design, regsumtype *regsum);

	double GetDev(designtype *design);
	int ncases;
	int nvars;
	double *y;
	double *x;
	int errorcode;
	int modeldf;
	double *m_coef;
	double *m_p_x;
	double *m_se;
	double *m_t;
	double *m_p;
	double m_dev;
	double m_pdev;
};
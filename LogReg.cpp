//logreg.cpp
#include "stdafx.h"
#include "LogReg.h"
#include <math.h>



//double ln10;
//int  nvars;

LogReg::LogReg()
{
//  ln10 = log(10.0);
//  nvars = 1;//nxx + 1;
  errorcode = 0;
  ncases = 0;
}// (* InitRegs *);

LogReg::~LogReg()
{
}

int LogReg::DoLogReg(int nxx, int cases, double *p_y, double *p_x, double *coef, double *se, double *t, 
					 double *p, double *dev, double *pdev)
{
	nvars = nxx + 1;
	ncases = cases;
	y = p_x;
	x = p_y;
	m_coef = coef;
	m_se = se;
	m_t = t;
	m_p = p;
	//m_dev = dev;
	//m_pdev = pdev;
	MultRegs();
	*dev = m_dev;
	*pdev = m_pdev;
	return 1;
}


double LogReg::LN0(double X)
{
//(* LN function GUARDING AGAINST 0 or NEGATIVE ARGUMENT. *)
  if(X < 1.0E-30)
	return -70.00;
  return log(X);
}//(* LN0 *);

void LogReg::NextCase (designtype *design, int c, double *wt, double *nlogit, double *fixedterm, double *augxrow)
//void NextCase (designtype *design, int c, double *wt, double *nlogit, double *fixedterm, double *augxrow)
{
	long j;//   : LongInt;
 // with design do
    if(design->constant)
		augxrow[1] = 1.0;
    augxrow[design->dimaiugxrow] = y[c];
    for(j = 1; j <= design->nx; j++)
		augxrow [j + design->ordconst] = x[c*design->nx + j-1];
		//augxrow [J + design->ordconst] := x[c*nx + j-1];
    *wt = 1.0;
    *fixedterm = 0.0;
    *nlogit = 1;

}//(* NextCase *);

void LogReg::GLMStats (double nlogit, double fixedterm, designtype *design,
					   double *augxrow, double *GLMWT, double *YHAT,
					   bool dodev, bool *OK)
{
	/*
	procedure GLMStats (var nlogit, fixedterm : Double; var design : designtype;
	var augxrow : workrow; var GLMWT, YHAT : Double;
	dodev : Boolean; var OK : Boolean);
	(* CALCULATES NEEDED STATS for LOGIST and POISSON. if dodev = False, THE GLM
	wt and PSUEDO-OBSERVATION, PUT IN augxrow [dimaiugxrow], ARE RETURNED.
	if dodev IS True, THE CASE CONTRIBUTION to THE deviance IS RETURNED
	IN wt.

	dodev CANNOT BE A VAR PARAMETER; OK MUST BE.

	COMMENTS ON DEALING WITH FIXEDTERMS.  THE REGRESSION IS PERFORMED ON
	y = Y + (z - u)(du/dY), WHERE Y IS THE LINEAR PREDICTOR, z IS THE OBSERVED,
	and u IS THE FITTED VALUE, I.E., u = INV. LINK (Y).
	SINCE Y = fixedterm + XB, THE FITTING CYCLE IS:
	1.) GET A STARTIING VALUE for Y;
	2.) COMPUTE u, du/dY and GET z, COMPUTE y;
	3.) REGRESS y - fixedterm ON X, GET ESTIMATES of B;
	4.) ESTIMATE Y AS Y = fixedterm + XB, GO to STEP X *)*/
	long I;//           : LongInt;
	double YOBS;//        : Double;
	double PFIT;//       : Double;
	double LOGFIT;//      : Double;
	//with design do
	//begin
	*OK = True;
	YOBS = augxrow [design->dimaiugxrow];
	if(design->itcount == 1)
	{
		if((nlogit < YOBS) || (YOBS < 0.0) || (nlogit < 0.0))
			*OK = false;
		if(*OK == true)
		{
			PFIT = (YOBS + 0.5) / (nlogit + 1.0);//         (* M & N, P. 82 *)
			LOGFIT = LN0(PFIT / (1.0 - PFIT)); //           (* M & N, P. 32 *)
		}
	}
	else
	{//    (* LOGIT STUFF *)
		LOGFIT = 0.0;
		for(I = 1; I <= design->npars; I++)
			LOGFIT = LOGFIT + augxrow[I] * design->beta[I];
		LOGFIT = LOGFIT + fixedterm;
		if(LOGFIT > 70 )//       (* EXP (-70) IS ROUGHLY 4.0E-31 *)
			PFIT = 1.0;
		else
		{
			if(LOGFIT < -70)//       (* EXP (70) IS ROUGHLY 2.5E+30 *)
				PFIT = 0.0;
			else
				PFIT = 1.0 / (1.0 + exp(-LOGFIT));
		}
		if(PFIT < 1.0E-10)
			PFIT = 1.0E-10;
		else if(PFIT > 0.999999)
			PFIT = 0.999999;
	}
	if(*OK == true)
	{
		*YHAT = PFIT * nlogit;
		if(dodev == true)//  (* COMPUTING THE deviance, M & N, p. 82 *)
			*GLMWT =  YOBS * LN0(YOBS / (*YHAT)) + (nlogit - YOBS) * LN0((nlogit - YOBS) / (nlogit - (*YHAT)));
		else
		{//    (* FITTING A MODEL' M & N, P. 80 *)
			*GLMWT = (*YHAT) * (1.0 - PFIT);
			augxrow[design->dimaiugxrow] = LOGFIT - fixedterm + (YOBS - (*YHAT)) / (*GLMWT);
		}
	}
}

long XtxIndex (long XI, long XJ, long npars)
/* INDEX for THE UPPER DIAGONAL ELEMENTS of A SQUARE MATRIX.
   XTXI IS STORED AS SEQUENTIAL ROWS. */
{
  //long XtxIndex = (XI - 1) * (2 * npars - XI + 2) div (2 + 1 + XJ - XI;
  ldiv_t tmp = div((XI - 1) * (2 * npars - XI + 2), 2L);// + 1 + XJ - XI;
  long XtxIndex = tmp.quot + 1 + XJ - XI;
  return XtxIndex;
}  

long UIndex (long UI, long UJ, long dimaiugxrow)
/* INDEX for THE UPPER DIAGONAL ELEMENTS of A SQUARE MATRIX.  AUGU IS STORED
   AS SEQUENTIAL ROWS. NOTE UJ >= UI. */
{
 // long UIndex = (UI - 1) * (2 * dimaiugxrow - UI + 2) div 2 + 1 + UJ - UI;
  ldiv_t tmp = div((UI - 1) * (2 * dimaiugxrow - UI + 2),  2L);// + 1 + UJ - UI;
  long UIndex = tmp.quot + 1 + UJ - UI;
  return UIndex;
}

long SSIndex (long SSI, long SSJ, long nxy)
/* nxy IS THE NUMBER of INDEPENDENT VARIABLES IN THE MODEL, BUT nx + 1
   for MODELS THAT CONTAIN AN INDEPENDENT VARIABLE.
   INDEX for THE UPPER DIAGONAL ELEMENTS of A SQUARE MATRIX.  SS IS STORED
   AS SEQUENTIAL ROWS. INTendED for USE WITH THE CROSS PRODUCT MATRIX. */
{
 // long SSIndex = (SSI - 1) * (2 * (nxy + 1) - SSI + 2) div 2 + 1 + SSJ - SSI;
  ldiv_t tmp = div((SSI - 1) * (2 * (nxy + 1) - SSI + 2), 2L);
  long SSIndex = tmp.quot + 1 + SSJ - SSI;
  return SSIndex;
}//end  (* SSIndex *);

void TranUbyRow (designtype *design, double wt, double *augxrow, double *AUGU)
/* TRANSFORMS THE AUGMENTED X ROW USING THE PIVOT ROW of AUGU.
   PIVOT-1 TRANSFORMS SHOULD HAVE ALREADY BEEN PERFORMED ON augxrow. */
{
   double D;//        : Double;  (* NOTATION FOLLOWS SEBER, P318 - 319 *)
   double DPRIME;//   : Double;
   double CBAR;//     : Double;
   double SBAR;//     : Double;
   double UM;//       : Double;
   double  VM;//       : Double;
   double  VI;//       : Double;
   long M;//        : LongInt;
   long PIVOT;//    : LongInt;
   long STARTROW;// : LongInt;
   for(PIVOT = 1; PIVOT < design->dimaiugxrow; PIVOT++)
   {
    if(wt > 0.0) 
	{
      VI = augxrow[PIVOT];
      if(fabs(VI) > 1.0E-20)
	  {//then (* PREVENTS DPRIME FROM BEING ZERO *)
          //(* CANNOT EXPLOIT SPARSENESS *)
        STARTROW = UIndex(PIVOT, PIVOT, design->dimaiugxrow);
        D = AUGU[STARTROW];
        DPRIME = D + wt * VI * VI;
        if(fabs(DPRIME) > 1.0E-20)
		{
          SBAR = wt * VI / DPRIME;
          CBAR = D / DPRIME;
          wt = CBAR * wt;
          AUGU[STARTROW] = DPRIME;
          for(M = PIVOT + 1; M <= design->dimaiugxrow; M++)
		  {
            VM = augxrow[M];
            UM = AUGU[STARTROW + M - PIVOT];
            augxrow[M] = VM - VI * UM;
            AUGU[STARTROW + M - PIVOT] = CBAR * UM  +  SBAR * VM;
		  }
		}
	  } //(* CANNOT EXPLOIT SPARSENESS *)
	}
   }
   //(* doNE WITH FIRST npars ROWS WEIGHTS WERE NONZERO *)
   STARTROW = UIndex(design->dimaiugxrow, design->dimaiugxrow, design->dimaiugxrow);
   AUGU[STARTROW ] = AUGU[STARTROW]  + wt * augxrow[design->dimaiugxrow] * augxrow[design->dimaiugxrow];
}

void LogReg::FindU(designtype *design, double *AUGU, bool *OK)
{
/* MAKES ONE PASS THROUGH THE DATA and CONSTRUCTS THE UTILDA MATRIX
   of GENTLEMAN'S MODIFICATION of GIVENS DECOMPOSITION. */
  double WTSUM ;//    : Double;    (* SUM of ALL CASE WEIGHTS *)
  double augxrow[MAXPLUS2];//   : workrow;
  double wt;//        : Double;
  double DUMMY;//    : Double;
  double GLMWT;//     : Double;
  double nlogit;//    : Double;
  double fixedterm;// : Double;
  int c;//         : Integer;
  *OK = True;
  //with design  do
  //begin
    WTSUM = 0.0;
    design->goodcases = 0;
    c = 0;
	do 
	{//repeat
      NextCase (design, c, &wt, &nlogit, &fixedterm, augxrow);
      c++;
      design->goodcases++;// := goodcases + 1;
      GLMStats (nlogit, fixedterm, design, augxrow, &GLMWT, &DUMMY, false, OK);
      wt = GLMWT * wt;
      if( *OK == true)
	  {
        WTSUM = WTSUM + wt;
        TranUbyRow (design, wt, augxrow, AUGU);
	  }
      else 
		  errorcode = 1;
    //until (c = ncases) or not OK
	}
	while( c != ncases && *OK == true);
}//end (* FindU *);

void LogReg::SingularTest (designtype *design, double *AUGU, bool *OK)
{
	/* SETS UP dimaiugxrow, AUGU and goodcases.
	IT CHECKS AUGU for SINGULARITY, and if IT IS SINGULAR,
	REDUCES npars, nx, dimaiugxrow and TRIES AGAIN. */
	bool SINGULAR;// : Boolean; (* True if A SINGULARITY WAS FOUND *)
	long PIVOT;//    : LongInt; (* PIVOT of AUGU *)
	long I ;//       : LongInt; (* INDEX of X VAR WHICH CAUSES THE SINGULARITY *)
	design->fullnx = design->nx;
	design->npars = design->nx + design->ordconst;
	design->dimaiugxrow = design->npars + 1;
	do
	{
		SINGULAR = false;
		for(int i = 0; i < DIMAUGU; i++)
			AUGU[i] = 0.0;
		//FillChar (AUGU, SizeOf (AUGU), 0);
		FindU (design, AUGU, OK); //(* OK = False if NEGATIVE WTS WERE FOUND  *)
		if(*OK == true)//                (* ERROR MESSAGE GIVEN FROM PROC FindU  -      *)
		{                //(* doN'T MOVE SINCE IT GIVES CASE of BAD wt.   *)
			if(design->goodcases <= design->npars)
			{
				errorcode = 2;
				*OK = false;
			}
			else
			{
				if(design->constant == true)
					PIVOT = 1;
				else 
					PIVOT = 0;// (* do not CHECK THE D for THE INTERCEPT TERM *)
				I = 0;
				do 
				{
					I++;
					PIVOT++;
					if(fabs(AUGU [UIndex (PIVOT, PIVOT, design->dimaiugxrow)]) < DTOOSMALL)
						SINGULAR = true;
				}//until SINGULAR or (I = nx);
				while(SINGULAR == false && I != design->nx);
				if(SINGULAR == true)
				{
					*OK = false;
					errorcode = 3;
				}
			}
		}
		//until not SINGULAR or not OK
	}
	while(SINGULAR && *OK == true);
}//end (* SingularTest *);

void LogReg::TestSmallMSE(double SST, double MSE, bool *OK)
{
  if(SST < 1.0E-10)
  {
    errorcode = 4;
    *OK = False;
  }
  else
  if(MSE < 1.0E-10)
  {
    errorcode = 5;
    *OK = false;
  }
  else if(log(SST) - log(MSE) > 20.0)
  {
    errorcode = 6;
    *OK = false;
  }
  else 
	*OK = true;
}//end (* TestSmallMSE *);

double ZX (double A, double B)
{
/* SMALL NUMBER MULTIPLY.
   CHECKS for UNDERFLOW BEFORE MULTIPLICATION, RETURNS ZERO ON UNDERFLOW. */
	double ZX;
const double LNSMALL = -85.195; //(* SAME AS IN FINDBETAPROB *)
  if ((A = 0.0) || (B = 0.0) || (fabs(A) + fabs(B) < 1E-5))
	ZX = 0.0;
  else 
	ZX = A * B;
  return ZX;
}//end; (* ZX *)

double ZD (double A, double B)
{
/* SMALL NUMBER DIVIDE, A / B.
   CHECKS for UNDERFLOW BEFORE DIVIDE, RETURNS ZERO ON UNDERFLOW. */
	const double LNSMALL = -85.195; //(* SAME AS IN FINDBETAPROB *)
	double ZD = 0.0;
  if(A == 0.0 || B == 0)
	ZD = 0.0;
  else
  {
    if(log(fabs(A)) - log(fabs(B)) < LNSMALL)
		ZD = 0.0;
    else 
		ZD = A / B;
  }
  return ZD;
}//end; (* ZD *)

double ZPROB (double Z, bool UPPER)
{
	double ZPROB = 0.0;
/* APPL. STATIS. ALGORITHM AS 66.   HILL.  if UPPER IS True, THE UPPER TAIL IS
  RETURNED; OTHERWISE THE LOWER TAIL IS RETURNED. */
const double ZERO  = 0.0;
const double HALF = 0.5;
const double ONE = 1.0;
const double CON = 1.28;
const double LTONE = 5.0;    //(* SET FOR 6 SIG. FIG.'S *)
const double UTZERO = 13.05;  //(* SET USING APPLE CP/M TURBO *)
  double ZPTEMP;//       : Double;
  double Y;//            : Double;

  if(Z < 0.0)
  {
    UPPER = !UPPER;
    Z = -Z;
  }
  if((Z > UTZERO) || (!UPPER && (Z > LTONE)))
	ZPTEMP = 0.0;
  else
  {
    Y = HALF * Z * Z;
    if(Z <= CON)
	{
      ZPTEMP = HALF - Z * (0.398942280444 - 0.39990348504 * Y /
      (Y + 5.75885480458 - 29.8213557808 /
      (Y + 2.62433121679 + 48.6959930692 /
      (Y + 5.92885724438))));
	}
    else //(* Z > CON *)
	{
      ZPTEMP = 0.398942280385 * exp(-Y) /
      (Z - 3.8052E-08 + 1.00000615302 /
      (Z + 3.98064794E-04 + 1.98615381364 /
      (Z - 0.151679116635 + 5.29330324926 /
      (Z + 4.8385912808 - 15.1508972451 /
      (Z + 0.742380924027 + 30.789933034 / (Z + 3.99019417011))))));
	}
  }
  if(!UPPER)
	ZPTEMP = 1.0 - ZPTEMP;
  ZPROB = ZPTEMP;
  return ZPROB;
}//end (* ZPROB *);

double Z2TAIL(double Z)
{
/* RETURNS THE TWO TAILED P - VALUE FOR Z */
  double Z2TAIL = 2.0 * ZPROB (fabs(Z), true);
  return Z2TAIL;
}//end (* Z2TAIL *);

double X2UPPER(double X2, long DF)
{
/* PATTERNED AFTER HILL CACM ALGORITHM 299 */
const double BIGX = 175.0;   //(* MACHINE SPECIFIC *)
  double A;//        : Double;
  double S;//        : Double;
  double E;//        : Double;
  double C;//       : Double;
  double Z;//        : Double;
  double X;//        : Double;
  double Y;//        : Double;
  bool EVEN;//     : Boolean;
  bool LARGEX;//   : Boolean;
  double T1;//       : Double;
  double T2;//       : Double;
  double TEMPROB;//  : Double;

  if(X2 <= 0.0)
	TEMPROB = 1.0;
  else
  {
    X = X2;
    LARGEX = X > BIGX;
    if(DF > 100)
	{
		if(X < 1.0E-06)
			TEMPROB = 1.0;
		else
		{
			T1 = exp(0.3333333333 * log(X / (1.0 * DF)));
			T1 = -sqrt(4.5 * DF) * (T1 + 2.0 / (9.0 * DF) - 1.0);
			TEMPROB = ZPROB (T1, False);
		}
	}
    else
	{
      A = 0.5 * X;
      EVEN = (1.0 * DF - (2.0 * (int) (1.0 * DF / 2.0))) < 0.1;
      if((EVEN || (DF > 2)) && !LARGEX)
		Y = exp(-A);
      else 
		Y = 0.0;
      if(EVEN)
		S = Y;
      else 
		S = 2.0 * ZPROB(-sqrt(X), false);
      if(DF <= 2)
	  {
        TEMPROB = S;
	  }
      else
	  {
        X = 0.5 * (DF - 1.0);
		if(EVEN)
			Z = 1.0;
        else 
			Z = 0.5;
        if(LARGEX)
		{
			if(EVEN)
				E = 0.0;
			else 
				E = 0.5723649429;
			C = log(A);
			do
			{
				E = log(Z) + E;
				T2 = C * Z - A - E;
				if(fabs(T2) < (BIGX / 2))
					S = exp(T2) + S;
				Z = Z + 1;
			}
			while((X - Z) >= -0.1);
          //until (X - Z) < -0.1;  (* MUST PERFORM LOOP FOR X = Z *)
			TEMPROB = S;
		}
        else
		{// (* not BIGX *)
          if(EVEN)
			E = 1.0;
          else 
			E = 0.5641895835 / sqrt(A);
          C = 0.0;
          do
		  {
            E = E * A / Z;
            C = C + E;
            Z = Z + 1.0;
		  }
         // until (X - Z) < -0.1;
		  while((X - Z) >= -0.1);
          TEMPROB = C * Y + S;
		}
	  }
	}
  }
  return TEMPROB;
}//end (* X2UPPER *);

void SolveBeta (double *augu, designtype *design, regsumtype *regsum, double *AUGU)// : symatrix);
{
/* SOLVES AUGU BY BACKWARD SUBSTITUTION to FIND THE BETAS.  SIMILAR TO
   GENTLEMAN'S PROCEDURE REGRESS A.S. ALGORITHM 75.SIGN */
  long I;// : LongInt;
  long K;// : LongInt;
    design->beta[design->npars] = AUGU [UIndex (design->npars, design->dimaiugxrow, design->dimaiugxrow)];
    for(I = design->npars - 1; I >= 1; I--)// downto 1 do
	{
      design->beta[I] = AUGU[UIndex (I, design->dimaiugxrow, design->dimaiugxrow)];
      for(K = I + 1; K <= design->npars; K++)// do (* BACKWARD SUBSTITUTION *)
		design->beta[I] = design->beta[I] - design->beta[K] * AUGU[UIndex (I, K, design->dimaiugxrow)];
	}
}//end (* SolveBeta *);

double LogReg::GetDev(designtype *design)
{
/* CALCULATE THE deviance for THE LOGISTIC MODEL. */
  double TEMP;//      : Double;
  double DEV;//       : Double;
  double UROW[MAXPLUS2];//      : workrow;
  double DUMMYZ;//    : Double;          (* DUMMY ARGUMENT for Z *)
  bool DUMMYOK;//   : Boolean;       (* DUMMY ARGUMENT for OK *)
  double wt;//        : Double;
  double nlogit;//    : Double;
  double fixedterm;// : Double;
  long c;//         : LongInt;
    TEMP = 0.0;
    c = 0;
    do
	{
      NextCase (design, c, &wt, &nlogit, &fixedterm, UROW);
      c++;
      GLMStats (nlogit, fixedterm, design, UROW, &DEV, &DUMMYZ, true, &DUMMYOK);
      //(* SEE M & N P. 25 for PRIOR WEIGHTS *)
      TEMP = TEMP + wt * DEV;
    //until c = ncases
	}while(c != ncases);
  
  return 2.0 * TEMP;
}//end (* GetDev *);

void FindXtxi (double *augu, designtype *design, double *XTXI, double *AUGU)// : symatrix);
{
/* AUGU HAS THE ELEMENTS D ON THE DIAGONAL, and THE ELEMENTS of UTILDA
   ARE THE SUPERDIAGONALS. DIAGONALS of UTILDA ARE UNITY. WE HAVE
     U = SQRT (D) X UTILDA
   WANT
      (XTX)I = (U)I X ((U)I)T
   WHICH IS
      (XTX)I = (UTILDA)I X (D)I X ((UTILDA)I)T

   SEE SEBER for THE NOTATION WRT GENTLEMAN'S DECOMPOSITION, and ALSO
   HOW to INVERT UPPER TRIANGULAR MATRICES. */
  double ELEMENT;//  : Double;
  long I,J,K;//    : LongInt;
  long R,S;//      : LongInt; (* SEE SEBER P 305 *)
    for(I = 1; I <= design->npars; I++)// do
		XTXI [XtxIndex (I, I, design->npars)] = 1.0;
    for(I = design->npars - 1; I >= 1; I--)// downto 1 do
	{
      XTXI [XtxIndex (I, I, design->npars)] = 1.0;
	  for(J = I + 1; J <= design->npars; J++)// to npars do
	  {
        ELEMENT = 0.0;
        for(K = I + 1; K <= J; K++)// to J do
			ELEMENT = ELEMENT + AUGU [UIndex (I, K, design->dimaiugxrow)] * XTXI [XtxIndex (K, J, design->npars)];
        XTXI [XtxIndex (I, J, design->npars)] = - ELEMENT;
	  }// (* J *)
	}// (* I , (UTILDA)I NOW IN XTXI *);
    /*
       NOW FORMING (UTILDA)I X (D)I X ((UTILDA)I)T
       SEE P 305 for DETAILS, ALTHOUGH MODIFIED for GENTLEMAN'S
       INSERTED WEIGHTING MATRIX.
    */
    for(R = 1; R <= design->npars; R++)// to npars do
	{
		for(S = R; S <= design->npars; S++)// to npars do
		{
		  ELEMENT = 0.0;
		  for(K = S; K <= design->npars; K++)// to npars do
			ELEMENT = ELEMENT + XTXI [XtxIndex (R, K, design->npars)] * XTXI [XtxIndex (S, K, design->npars)] / AUGU [UIndex (K, K, design->dimaiugxrow)];
		  XTXI [XtxIndex (R, S, design->npars)] = ELEMENT;
		}
	}
}//end (* FindXtxi *);

void LogReg::RegSummary (double *augu, designtype *design, regsumtype *regsum, double *XTXI, double *AUGU, bool *OK)
{
/* CALCULATES THE FINAL STATISTICS NEEDED for THE SUMMARY TABLES. */
  long I;//          : LongInt;
  double SSE;//        : Double;     (* SUM of SQUARES for ERROR *)
 // double MSE;
    regsum->DF = design->goodcases - design->npars;
    SSE = AUGU [UIndex (design->dimaiugxrow, design->dimaiugxrow, design->dimaiugxrow)];
	regsum->mse = SSE / regsum->DF;
	regsum->scale = 1.0;
	regsum->sst = 0.0;
	for(I = 1; I <= design->npars; I++)// to npars do
	{
		if ((I > 0) || !design->constant)
			regsum->sst = regsum->sst + AUGU [UIndex (I, I, design->dimaiugxrow)] *
				AUGU[UIndex (I, design->dimaiugxrow, design->dimaiugxrow)] * AUGU[UIndex (I, design->dimaiugxrow, design->dimaiugxrow)];
	}
    /* SST IS DoubleLY SS REG AT THIS POINT */
    regsum->sst = regsum->sst + SSE;
	TestSmallMSE (regsum->sst, regsum->mse, OK);
    if(*OK == true)
	{
      FindXtxi (augu, design, XTXI, AUGU);
	  for(I = 1; I <= design->npars; I++)// to npars do
	  {
		regsum->stderror[I] = XTXI [XtxIndex (I, I, design->npars)] * regsum->scale;
		if(regsum->stderror[I] == 0.0)
			regsum->tpvalue[I] = 0.0;
        else
		{
          regsum->stderror[I] = sqrt(regsum->stderror[I]);
          regsum->tpvalue[I] = Z2TAIL (design->beta[I] / regsum->stderror[I]);
		}
	  }
	  regsum->fpvalue = X2UPPER (design->deviance, regsum->DF);
	}
}//end (* RegSummary *);

void LogReg::CalcRegs (designtype *design, double *XTXI, regsumtype *regsum, bool *OK)
{
/* PERFORMS THE REGRESSION CALCULATIONS. */
  double AUGU[DIMAUGU];//      : ^symatrix;  (* GENTLEMAN'S U FACTOR MATRIX *)
  double OLDDEV;//    : Double;
  double *augu = new double[DIMAUGU];//New(augu);
  for(int i = 0; i < DIMAUGU; i++)
	  augu[i] = 0.0;

    design->itcount = 0;
    design->deviance = 0.0;
    do
	{
      design->itcount++;// := itcount + 1;
      SingularTest (design, AUGU, OK);
      if(*OK == true)
	  {
        SolveBeta (augu, design, regsum, AUGU);
        OLDDEV = design->deviance;
        design->deviance = GetDev(design);
	  }
	}while((*OK == true) && (fabs(OLDDEV - design->deviance) >= design->devdiff) && design->itcount < design->maxints);
    /*until not OK or
    ((itcount > 1) and (ABS (OLDDEV - deviance) < devdiff)) OR
    (itcount >= maxints);*/
    if(*OK == true)
	{
      if(fabs(OLDDEV - design->deviance) >= design->devdiff)
	  {
        *OK = false;
        errorcode = 7;
	  }
	}
    if(*OK == true)
		RegSummary (augu, design, regsum, XTXI, AUGU, OK);
    delete[] augu;
}//end (* CalcRegs *);

void LogReg::RegsTables (designtype *design, regsumtype *regsum)
{
  long i;// : LongInt;
    for(i = 1; i <= design->npars; i++)// to npars do
	{
      m_coef[i-1] = design->beta[i];
      m_se[i-1] = regsum->stderror[i];
	  m_t[i-1] = design->beta [i] / regsum->stderror[i];
	  m_p[i-1] = regsum->tpvalue[i];
	}
	modeldf = regsum->DF;
    m_dev = design->deviance;
	m_pdev = regsum->fpvalue;
}//end (* RegsTables *);



void LogReg::MultRegs()
{
  designtype design;//      : designtype; (* MODEL design *)
  int x;//           : Integer;
  double XTXI[DIMXTXI];//       : ^xtxmatrix;
  regsumtype regsum;//      : regsumtype; (* REGRESSION RESULTS *)
  bool OK;//          : Boolean;    (* SET AS LONG AS THINGS ARE OKAY *)

  //FillChar (design, SizeOf (design), 0);
  memset(&design, 0, sizeof(design));
    design.yindex = 0;
    design.nx = nvars - 1;
    for(x = 1; x <= design.nx; x++)// to nx do
		design.xindex[x] = x + 1;
    design.fullnx = design.nx;
    design.lindex = 0;
    design.logconst = 1.0;
    design.ftindex = 0;
    design.wtindex = 0;
    design.constant = true;
    design.ordconst = 1;
    design.dimaiugxrow = design.nx + 2;
    design.npars = design.nx + 1;
    design.nxy = design.nx + 1;
    design.maxints = 10;
    design.devdiff = 0.01;
  if(design.nx > 0)
  {
   // New(xtxi);
	//xtxi = new double[]
	  for(int i = 0; i < DIMXTXI; i++)
		  XTXI[i] = 0.0;
    CalcRegs(&design, XTXI, &regsum, &OK);
    if(OK)
		RegsTables(&design, &regsum);
   // Dispose(xtxi);
  }
}//end (* MultRegs *);


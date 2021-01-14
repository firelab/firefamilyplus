//#include <stdlib.h>
//#include <cmath>
//#include <iostream>
//#include <fstream>
//using namespace std;
#include "stdafx.h"
#include <math.h>
#include "BestFit.h"

//Constructor
BestFit::BestFit(){
   _PointCount = 0;
   _Pinit[0] = 5;
   _Pinit[1] = .01;
}
//Input: add data points to array; initial guesses for Alpha and Beta
void BestFit::AddPoint(double x, double y){
   _XList[_PointCount] = x;
   _YList[_PointCount] = y;
   _PointCount++;
}
void BestFit::setAlphaInitial(double value) {
   _Pinit[0] = value;
}
void BestFit::setBetaInitial(double value) {
   _Pinit[1] = value;
}
//Output: Alpha and Beta
double BestFit::getAlphaOptimal() {
   return _POpt[0];
}
double BestFit::getBetaOptimal() {
   return _POpt[1];
}
double BestFit::getBestFitAlpha() {
   return _PBest[0];
}
double BestFit::getBestFitBeta() {
   return _PBest[1];
}
//For testing: Display array
void BestFit::ShowArray()
{
  int i;
  for (i = 0; i < _PointCount; i++)
   // cout << "(" << _XList[i] << ", " << _YList[i] << ")\n";
	TRACE2("(%f, %f)\n", _XList[i], _YList[i]);
}
//Primary routine: Compute Alpha and Beta
void BestFit::ComputeBestFit()
{
  //>Run algorithm
  FitCurve();
  //>Read results
  _PBest[0] = _POpt[0];  // Round(_POpt[0], 1);
  _PBest[1] = _POpt[1];  // Round(_POpt[1], 4);
  //>If program didn't find the curve, start from default values
  if (RSquared(_PBest[0], _PBest[1]) < 0.95)
  {
    for (double i = 2; i <= 12; i += 4)
    {
      for (double j = 0.005; j <= 0.015; j += 0.005)
      {
        _Pinit[0] = i;
        _Pinit[0] = j;
        FitCurve();
        if (RSquared(_POpt[0], _POpt[1]) > RSquared(_PBest[0], _PBest[1]))
        {
          _PBest[0] = _POpt[0];
          _PBest[1] = _POpt[1];
        }
      }
    }
  }
}
void BestFit::FitCurve()
{
// Levenberg-Marquardt non-linear curve-fit subroutine

//Constants
  const double MAX_Alpha = 1000;
  const double MIN_Alpha = 0.001;
  const double MAX_Beta = 10;
  const double MIN_Beta = 0.00001;
//Intermediate sets of parameters (alpha and beta)
  double P[2];
  double POLD[2];
  double P1[2];
//>Explore modifies GMatrix, D, XInfo, EMatrix, CMatrix, BMatrix
  double EMATRIX[2];
  double GMATRIX[2];
  double D[2];
  double BMATRIX[2][2];
  double XINV[2][2];
  double CMATRIX[2][2];
  double XINFO[2][2];

  const int MAX_Iterations = 100;
  const double EPS = 0.00000001;
  const double XMYMAX = 100000000000.0;
  double XMY = 0.01;
  int nIterations = 0;
  bool bInside = false;
  bool bIsc = false;
  bool bOK = false;
  bool bSig = false;
  int i = 0;

  P[0] = _Pinit[0];
  POLD[0] = P[0];
  P[1] = _Pinit[1];
  POLD[1] = P[1];

  double F = SumOfSquares(_PointCount, P, _XList, _YList);
  double F1 = 0;
  double FOLD = F;

  do
  {
		 bIsc = false;
		 //>Explore modifies GMatrix, D, XInfo, EMatrix, CMatrix, BMatrix
		 bOK = Explore(P, GMATRIX, D, XINFO, EMATRIX, CMATRIX, BMATRIX);
		 nIterations += 1;
		 if (bOK)
		 {
			while ((XMY <= XMYMAX) && (bIsc == false))
			{
			   FIND(EMATRIX, BMATRIX, D, XINV, GMATRIX, P, P1, XMY);
			  // TRACE3("Iteration: %d, P1[0] = %f, P1[1] = %f\n", nIterations, P1[0], P1[1]);
			   bInside = true;
			   if (P1[0] >= MAX_Alpha || P1[0] <= MIN_Alpha)
			   {
				   bInside = false;
			   }
			   if (P1[1] >= MAX_Beta || P1[1] <= MIN_Beta)
			   {
				   bInside = false;
			   }
			   if (bInside)
			   {
				  F1 = SumOfSquares(_PointCount, P1, _XList, _YList);
				  if (F1 < F)
				  {
					 bIsc = true;
					 FOLD = F;
					 F = F1;
					 XMY = XMY / 100.0;
					 for (i = 0; i <= 1; i++)
					 {
						POLD[i] = P[i];
						P[i] = P1[i];
					 }
				  }
			   }
			   XMY = XMY * 10.0;
			}
		 }
		 bSig = SigCheck(P, POLD, F, FOLD);
	  } while ( ! ((nIterations == MAX_Iterations) || (bIsc == false) || (bSig == false)));

	  //>Explore modifies GMatrix, D, XInfo, EMatrix, CMatrix, BMatrix
	  if (bIsc)
	  {
		  bOK = Explore(P, GMATRIX, D, XINFO, EMATRIX, CMATRIX, BMATRIX);
	  }
	  if (bOK)
	  {
		 //IRD = False
		 //If (EMATRIX(0) < EPS) Then IRD = True
		 //If (EMATRIX(1) < EPS) Then IRD = True
		 //>Find modifies XInv and P1
		 FIND(EMATRIX, BMATRIX, D, XINV, GMATRIX, P, P1, XMY);
		 //>Get standard deviation
		 //If (M > N) Then
		 //   For I = 1 To N
		 //      SDEV(I) = Sqr(XINV(I, I) * F / (M - N))
		 //   Next I
		 //   bSig = SigCheck(P1(), P(), F, F1)
		 //End If
	  }
	  // load optimal parameter array
	  _POpt[0] = P[0];
	  _POpt[1] = P[1];
   }

//Additional subroutines used in computing Alpha and Beta
double BestFit::SumOfSquares(int M, double P[2], double X[], double Y1[])
{
  double SqrSum = 0.0;
  int k = 0;
  double Y = 0;
  for (k = 0; k < M; k++)
  {
    Y = FittingFunction(X[k], P);
    SqrSum = SqrSum + (pow((Y - Y1[k]), 2));
  }
  return SqrSum;
}
bool BestFit::Explore(double P[2], double GMATRIX[], double D[],
  double XINFO[][2], double EMATRIX[], double CMATRIX[][2], double BMATRIX[][2])
{
  //>Get data for XInfo, GMatrix, D
  bool bOK = FillMatrices(P, GMATRIX, D, XINFO);
  if (bOK)
  {
    int i = 0;
    //>Enter data into CMatrix
    for (i = 0; i <= 1; i++)
    {
      CMATRIX[i][0] = XINFO[i][0] / D[i] / D[0];
      CMATRIX[0][i] = CMATRIX[i][0];
      CMATRIX[i][1] = XINFO[i][1] / D[i] / D[1];
      CMATRIX[1][i] = CMATRIX[i][1];
    }
    //>Enter data into BMatrix
    for (i = 0; i <= 1; i++)
    {
      BMATRIX[i][0] = CMATRIX[i][0];
      BMATRIX[i][1] = CMATRIX[i][1];
    }
    Decompose(EMATRIX, BMATRIX); //>Modifies both arrays
  }
  return bOK;
}
bool BestFit::FillMatrices(double P[2], double GMATRIX[], double D[],
  double XINFO[][2])
{
  //>Enter data into XInfo, GMatrix, D
  int i = 0;
  int j = 0;
  int k = 0;
  double DIF[2]; //>Result of PartialDif
  double Y = 0;
  double delta = 0;
  bool bOK = true;
  //>Clear values
  for (i = 0; i <= 1; i++)
  {
    XINFO[i][0] = 0.0;
    XINFO[i][1] = 0.0;
    GMATRIX[i] = 0.0;
  }

  for (k = 0; k < _PointCount; k++)
  {
    PartialDif(_XList[k], P, DIF); //>Get DIF array for each value of X
    Y = FittingFunction(_XList[k], P);
    delta = Y - _YList[k];
    for (i = 0; i <= 1; i++)
    {
      XINFO[i][0] = XINFO[i][0] + DIF[i] * DIF[0];
      XINFO[i][1] = XINFO[i][1] + DIF[i] * DIF[1];
      GMATRIX[i] = GMATRIX[i] + DIF[i] * delta;
    }
  }
  //>Check for diagonal value = 0 and copy 
  for (i = 0; i <= 1; i++)
  {
	  D[i] = sqrt(XINFO[i][i]);
	  if (D[i] == 0.0)
		  bOK = false;
	  for (j = 0; j < i; j++)
    {
		  XINFO[i][j] = XINFO[j][i];
    }
  }
  return bOK;
}

void BestFit::PartialDif(double X, double P[2], double DIF[])
{
  // Numerical partial derivatives subroutine
  //>Input is a single X value and array of fitting parameters
  //>Output is array DIF
  const double XINC = 0.01;
  double XincP[2];
  double XdecP[2];
  XincP[0] = P[0];
  XdecP[0] = P[0];
  XincP[1] = P[1];
  XdecP[1] = P[1];
  int i = 0;
  double ps = 0;
  double del = 0;
  double FInc = 0;
  double FDec = 0;
  for (i = 0; i <= 1; i++)
  {
    ps = P[i];
    if (ps == 0.0)
      del = XINC;
    else
      del = XINC * ps;
    XincP[i] = ps + del;
    XdecP[i] = ps - del;
    FInc = FittingFunction(X, XincP);
    FDec = FittingFunction(X, XdecP);
    DIF[i] = (FInc - FDec) / 2.0 / del;
    XincP[i] = ps;
    XdecP[i] = ps;
  }
}
void BestFit::Decompose(double EMATRIX[], double BMATRIX[][2])
{
  // Decompose eigenvector matrix subroutine
  //>Modifies both EMatrix and BMatrix
  const double EPS = 0.00000001;
  const int ITMAX = 10;
  const int NI = 1;
  int iter = 0;
  int kIter = NI;
  int j = 0;
  int h = 0;
  int i = 0;
  double scalProd = 0;
  double XjLong = 0;
  double XhLong = 0;
  double wij = 0;
  double wih = 0;
  double xle = 0;
  double ac = 0;
  double ww = 0;
  double teta = 0;
  double cteta = 0;
  double steta = 0;
  double flip = 0;

  do
  {
    iter += 1;
    for (j = 0; j <= 1; j++)
    {
      for (h = j + 1; h <= 1; h++) //Only value is 1
      {
        scalProd = 0.0;
        XjLong = 0.0;
        XhLong = 0.0;
        for (i = 0; i <= 1; i++)
        {
          wij = BMATRIX[i][j];
          wih = BMATRIX[i][h];
          scalProd += wij * wih;
          XjLong += wij * wij;
          XhLong += wih * wih;
        }
        xle = sqrt(XhLong * XjLong);
        if (xle > 0.0)
          ac = fabs(scalProd / xle);
        else
          ac = 0.0;
        scalProd = scalProd + scalProd;
        ww = XjLong - XhLong;
        if (ac > EPS || ww < 0.0)
        {
          if (fabs(ww) >= fabs(scalProd))
          {
            teta = fabs(scalProd / ww);
            cteta = 1.0 / sqrt(1.0 + teta * teta);
            steta = teta * cteta;
          }
          else
          {
            teta = fabs(ww / scalProd);
            steta = 1.0 / sqrt(1.0 + teta * teta);
            cteta = steta * teta;
          }
          cteta = sqrt((1.0 + cteta) / 2.0);
          steta = steta / (cteta + cteta);
          if (ww < 0.0)
          {
            flip = cteta;
            cteta = steta;
            steta = flip;
          }
          if (scalProd < 0.0)
            steta = -steta;
          for (i = 0; i <= 1; i++)
          {
            flip = BMATRIX[i][j];
            BMATRIX[i][j] = BMATRIX[i][h] * steta + flip * cteta;
            BMATRIX[i][h] = BMATRIX[i][h] * cteta - flip * steta;
          }
          kIter = NI;
        }
        else
        {
          kIter -= 1;
        }
      }
    }
  } while ( ! ((kIter <= 0) || (iter == ITMAX)));
  //>Set EMatrix values
  for (j = 0; j <= 1; j++)
  {
    EMATRIX[j] = 0.0;
    EMATRIX[j] = EMATRIX[j] + BMATRIX[0][j] * BMATRIX[0][j];
    EMATRIX[j] = EMATRIX[j] + BMATRIX[1][j] * BMATRIX[1][j];
    EMATRIX[j] = sqrt(EMATRIX[j]);
  }
  //>Set BMatrix values
  for (j = 0; j <= 1; j++)
  {
    if (EMATRIX[j] != 0.0)
    {
      BMATRIX[0][j] = BMATRIX[0][j] / EMATRIX[j];
      BMATRIX[1][j] = BMATRIX[1][j] / EMATRIX[j];
    }
  }
}
bool BestFit::SigCheck(double Q1[2], double Q2[2], double FQ1, double FQ2)
{
// Check for significant change in fit parameter subroutine
  bool bSig = false;
  const double EPS = 0.00001;
  int i = 0;
  for (i = 0; i <= 1; i++)
  {
    if (Q1[i] != 0.0)
    {
      if (fabs(1.0 - Q2[i] / Q1[i]) > EPS)
      {
        bSig = true;
      }
    }
  }

  if (fabs(FQ2) < 0.0000000000000001)
  {
    bSig = false;
  }
  else if (fabs(1.0 - FQ2 / FQ1) > EPS)
  {
    bSig = true;
  }
  return bSig;
}
void BestFit::FIND(double EMATRIX[2], double BMATRIX[2][2], double D[2],
  double XINV[][2], double GMATRIX[2], double P[2], double P1[], double XMY)
{
//>Modifies XInv() using XMY, EMatrix, BMatrix, and D
//>Modifies P1() using XInv, GMatrix and P
  int i = 0;
  int j = 0;
  int k = 0;
  double A = 0;
  //>Modify XInv()
  for (i = 0; i <= 1; i++)
  {
    for (j = 0; j <= 1; j++)
    {
      A = 0.0;
      for (k = 0; k <= 1; k++)
      {
        if (XMY > 0.0 || ((XMY == 0.0) && (EMATRIX[k] > 0.000001)))
        {
          A = A + BMATRIX[i][k] * BMATRIX[j][k] / D[i] / D[j] / (EMATRIX[k] + XMY);
        }
      }
      XINV[i][j] = A;
      XINV[j][i] = A;
    }
  }
  //>Modify P1()
  for (i = 0; i <= 1; i++)
  {
    A = 0.0;
    A = A - XINV[i][0] * GMATRIX[0];
    A = A - XINV[i][1] * GMATRIX[1];
    P1[i] = P[i] + A;
  }
}
double BestFit::FittingFunction(double X, double P[2])
{
// Non-linear fitting function subroutine
// Input
//  X = single X data point
//  P() = array of fitting parameters
// Output
//  Y = function value at X(idx)
  double alpha = P[0];
  double beta = P[1];
  double Y = 0;
  try
  {
    Y = 1 - exp(-((pow((beta * X), alpha))));
  }
  catch(...) {};

  return Y;
}
double BestFit::BestFitRSquared()
{
  if (_PBest[0] > 0 && _PBest[1] > 0)
    return RSquared(_PBest[0], _PBest[1]);
  else
    return 0;
}
double BestFit::RSquared(double alpha, double beta)
{
  //>Calculate mean of cumulative values: (1/n + 2/n + ... + n/n) / n
  if (_PointCount > 0)
    {
    double sum = 0;
    for (double i = 1; i <= _PointCount; i++)
    {
      sum += i / _PointCount;
    }
    double cumMean = sum / _PointCount;
    //Set up array representing a step graph of cumulative proportion of events
    //that have happened so far
    double cumProp[MAX_SEASON_DAYS + 1];
    for (int i = 0; i <= MAX_SEASON_DAYS; i++)
    {
      cumProp[i] = 0;
    }
    int evtDay;  //number of days from start of season to event
    for (int i = 1; i <= _PointCount; i++)
    {
      evtDay = (int)(_XList[i - 1]);
      cumProp[evtDay] = (double)i / _PointCount;
    }
    //>Calculate R Square
    double estimate;
    double sumEmpiricalMinusEstimate = 0;
    double sumEmpiricalMinusMean = 0;
    //>For each point, get a discrepancy value
    for (int i = 1; i <= MAX_SEASON_DAYS; i++)
    {
      estimate = 1 - exp(-((pow((beta * i), alpha))));
      if (cumProp[i] > 0)
      {
        sumEmpiricalMinusEstimate += (pow((cumProp[i] - estimate), 2));
        sumEmpiricalMinusMean += (pow((cumProp[i] - cumMean), 2));
      }
    }
    double rsqr = 1 - (sumEmpiricalMinusEstimate / sumEmpiricalMinusMean);
    if (rsqr < 0) rsqr = 0;
    return rsqr;
  }
  else
  {
    return 0;
  }
}
//Sort comparator
/*int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}
//Main
int main()
{
  int waitdays[MAX_EVENTS];
  int evtcount;

  ifstream in("TestTerm.txt");
  if (!in)
  {
    cout << "Cannot open file for input.\n";
    return 1;
  }
  int i;
  for (i = 0; i < MAX_EVENTS; i++)
  {
    in >> waitdays[i];
    if (waitdays[i] == 0)
    {
      evtcount = i;
      break;
    }
  }
  in.close();
//Sort the array of waitdays
  qsort(waitdays, evtcount, sizeof(int), compare);
//Input points: X = # of waitdays, Y = (ThisEvent# / EventCount)
//e.g. for 20 events Y = .05, .10, .15, ..., 1.00
	BestFit nlf;
  for (i = 0; i < evtcount; i++)
  {
    nlf.AddPoint(waitdays[i], (double)(i + 1) / evtcount);
  }
//Compute alpha and beta
  nlf.ComputeBestFit();
//Show results
  cout << "Alpha: " << nlf.getBestFitAlpha() << "\n";
  cout << "Beta: " << nlf.getBestFitBeta() << "\n";
  cout << "RSquared: " << nlf.BestFitRSquared() << "\n";

	return 0;
}*/

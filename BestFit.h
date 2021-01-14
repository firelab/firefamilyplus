
const int MAX_EVENTS = 300;
const int MAX_SEASON_DAYS = 465;  //Max # of days between season start and Term event

class BestFit {
private:
  int _PointCount;
  double _XList[MAX_EVENTS];
  double _YList[MAX_EVENTS];
  double _Pinit[2];  //initial guesses for fitting parameters - [0]=Alpha, [1]=Beta
  double _POpt[2];   //optimized fitting parameters - [0]=Alpha, [1]=Beta
  double _PBest[2];   //best results over several rounds - [0]=Alpha, [1]=Beta

//Compute Alpha and Beta - private subroutines
  double SumOfSquares(int M, double P[2], double X[], double Y1[]);
  bool Explore(double P[2], double GMATRIX[], double D[], double XINFO[][2],
    double EMATRIX[], double CMATRIX[][2], double BMATRIX[][2]);
  bool FillMatrices(double P[2], double GMATRIX[], double D[], double XINFO[][2]);
  void PartialDif(double X, double P[2], double DIF[]);
  void Decompose(double EMATRIX[], double BMATRIX[][2]);
  bool SigCheck(double Q1[2], double Q2[2], double FQ1, double FQ2);
  void FIND(double EMATRIX[2], double BMATRIX[2][2], double D[2],
    double XINV[][2], double GMATRIX[2], double P[2], double P1[], double XMY);
  double FittingFunction(double X, double P[2]);
public:
//Constructor
  BestFit();
//Input: add data points to array
  void AddPoint(double x, double y);
//Input: Alpha and Beta initial values (optional)
  void setAlphaInitial(double value);
  void setBetaInitial(double value);
//Output
  double getAlphaOptimal();
  double getBetaOptimal();
  double getBestFitAlpha();
  double getBestFitBeta();
  double BestFitRSquared();
  double RSquared(double alpha, double beta);
  void ShowArray();  //for testing purposes only
//Compute Alpha and Beta - public primary routine
  void ComputeBestFit();
  void FitCurve();
};


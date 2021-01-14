class CFireplusSet;
class CWxSet;
#include "updialog2.h"
#include "FireSumSet.h"
#include "FireOptionsSet.h"
#include "FireDay.h"

class CNelsonDFM

{
public:
    CNelsonDFM(CWnd* pParent );
	CWnd* parent;
	CFireplusSet *fpSet;
	void NelsonMoisturesReport();
	void NFDRSNelsonReport();
	int FillNelsonVals();
    void RunReport(void);
	void RunReport2(bool _isHalfNelson);
	int CalcValues(CWnd *_caller, int varID, bool _isHalfNelson);
	void GetLogitArraySizesQuick(CFireSumSet *firesSet, CFireOptionsSet *fireOptsSet, int vID,
		long *nV, long *nFD, long *nLFD, long *nMFD, long *_discards, long *madeUps);
	void CreateLogitArraysQuick(CFireOptionsSet *fireOptsSet, int vID,
		double *fxVals, double *fyVals, double *lfxVals, double *lfyVals, double *mfxVals, double *mfyVals);
	virtual ~CNelsonDFM();
	double m_start1;
	double m_start10;
	double m_start100;
	double m_start1000;
	char *m_fName;
	CWxSet *m_wxSet;
	CUPDUPDATA* pCUPDUPData;
   CNFDRS m_calcOld;
   CNFDRS m_calcNew;
	float **vals;
	COleDateTime start;
	COleDateTime end;
	int startYear;
	int nYears;
	int valsPerYear;
	int m_varID;
	CFireDay **fires;
	bool isHalfNelson;
	int recCount;
};


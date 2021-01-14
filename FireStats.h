 
// FireStats.h: interface for the CFireStats class.
//
//////////////////////////////////////////////////////////////////////
class LogReg;
class CSIGStationSet;
#if !defined(AFX_FIRESTATS_H__AA2A8F34_7704_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_FIRESTATS_H__AA2A8F34_7704_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define FD 1
#define LFD 2
#define MFD 3

typedef enum FILTER_DIRECTION {FILTER_LESSTHAN_EQUAL, FILTER_GREATERTHAN_EQUAL};

class ChiSquareGroup
{
public:
	ChiSquareGroup();
	double CalcChi2();
	double Output(FILE *stream, int round);	
	long obsHit;
	double expHit;
	long obsNoHit;
	double expNoHit;
	double pMin;
	double pMax;
	double pAvg;
	double valAvg;
	double valMin;
	double valMax;
};

class CFireStats  
{
public:
	double PercentileFromValue(int valType, double val);
	int varID;
	void StatsReport(FILE *stream, int firesType, BOOL conditional, int round);
	CString ExportPercentiles(void);
	long nDiscards;
	double binSize;
	CString * strings;
	int nStrings;
	CFireStats(CString _name, long nV, long nFD, long nLFD, long nMFD, bool _isNFDRS, int _varID);
	virtual ~CFireStats();
	//void Build(CFireplusSet *fpSet, bool probabilities, BOOL conditional);
	void BuildQuick(CFireplusSet *fpSet, bool probabilities, BOOL conditional, 
		long nVals, long nfdVals,
		double *fxVals, double *fyVals, double *lfxVals, double *lfyVals, double *mfxVals, double *mfyVals,
		LogReg *fdLogReg, LogReg *lfdLogReg, LogReg *mfdLogReg);
	//ChiSquareGroup *GoodFit(FILE *fd, int *ngrps, int fdType, BOOL conditional);
	ChiSquareGroup *GoodFitQuick(double *fxVals, double *fyVals, int *ngrps, int fdType, BOOL conditional);
	CString GetModelString(CSIGStationSet *stn);	
	CString varName;
	CString shortName;
	CString staStr;
	double filterValue;
	FILTER_DIRECTION filterDirection;
	int nFiltered;
	int cause;
	int humanCause;
	COleDateTime start;
	COleDateTime end;
	int mfdFires;
	int lfAcres;
	double b0fd;
	double b1fd;
	double b0lfd;
	double b1lfd;
	double b0mfd;
	double b1mfd;
	double pfd;
	double plfd;
	double pmfd;
	double devfd;
	double devlfd;
	double devmfd;
	long nv;
	long nfd;
	long nlfd;
	long nmfd;
	int nMadeUps;
	double * allVals;
	double * fdVals;
	double * lfdVals;
	double * mfdVals;
	//added 2/07/08 for candidates easy retrieval
	double fd_R2;
	double fd_Chi2;
	double fd_Chi2PVal;
	double fd_pMin;
	double fd_pMax;
	double lfd_R2;
	double lfd_Chi2;
	double lfd_Chi2PVal;
	double lfd_pMin;
	double lfd_pMax;
	double mfd_R2;
	double mfd_Chi2;
	double mfd_Chi2PVal;
	double mfd_pMin;
	double mfd_pMax;
	//end added for candidates...
	CString graphModelString;
	COleDateTime dateStamp;
	bool isNFDRS;
	int nFDG;
	ChiSquareGroup *xGroupsFD;
	int nLFDG;
	ChiSquareGroup *xGroupsLFD;
	int nMFDG;
	ChiSquareGroup *xGroupsMFD;
	CDatabase *pDB;
	CFireplusSet *m_pSet;
	bool m_conditional;
public:
	CString LogRegReport(bool isNelson = false, bool isHalfNelson = false);
};

#endif // !defined(AFX_FIRESTATS_H__AA2A8F34_7704_11D2_B8E5_000000000000__INCLUDED_)

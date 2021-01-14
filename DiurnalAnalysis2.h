#pragma once
#include "WxSet.h"
#include "fireplusSet.h"
#include "SIGStationSet.h"



// CDiurnalAnalysis2 command target

class CDiurnalAnalysis2 : public CObject
{
public:
	CDiurnalAnalysis2();
	virtual ~CDiurnalAnalysis2();

	CString stamp;
	CString modelStr;
	CString staName;
	//void AnalyzeSIG(const CString query, CFireplusSet *fpSet);
	int AnalyzeStation(CString stationStr, CSIGStationSet& staSet, CWxSet& wxSet);

	int Analyze(CWnd *_caller, CFireplusSet *fpSet, int nVars, int *_varIDs);
	CString GetModelString(CFireplusSet *fpSet, CSIGStationSet *stn);
	CString AddDateStamp();
	void LocateEvents(QueryRow *qRows, int rows, int hours, CFireplusSet *fpSet);
	double ****vals;//[366][MAXVARIDS];
	//CCancelDialog *recCounter;
	bool useCanadian;
	bool useNFDRS;
	bool useLFI;
	long count;
	int nVarIDs;
	int *varIDs;
	int nYears;
	int startYear;
	int endYear;

};



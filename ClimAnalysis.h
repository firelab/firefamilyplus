// ClimAnalysis.h: interface for the CClimAnalysis class.
//
//////////////////////////////////////////////////////////////////////
 
#if !defined(AFX_CLIMANALYSIS_H__DF437B61_D54A_11D1_B8B6_000000000000__INCLUDED_)
#define AFX_CLIMANALYSIS_H__DF437B61_D54A_11D1_B8B6_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//#include "nfdr32.h"
#include "canada.h"
#include "updialog2.h"
#include <fstream>

using namespace std;
class CRawClim;
class CPocketCardSet;
class CRunPocketCardSet;
class CFireOptionsSet;

int MonthChtoInt(CString chMonth);
//const char STATEFILENAME[] = "nfdrs.stt";
double CalcDayl(double lat,int yday);
double CalcDPT(double tempF, int RH);
double CalcVP(double tempF);
double CalcVPD(double minRH, double maxTempF);
double CalcVPDavg(double TempDewF, double TempAvgF);

class ID2
{
public:
	short userID;
	short varID;
	CString fieldName;
};

class CPDOptions : public CObject
{
	friend class COverlayDialog;
public:
	CDatabase *pDB;
	int mergedIDs[MAXMERGES];
	int nVars;
	int periodLength;
	COleDateTime dateTime;
	long nWxObs;
	CString model;
	CString stationName;
	//COLORREF m_Line_Color;
	int lineStyles[OVERLAYS];
	int lineWidths[OVERLAYS];
	COverlayDialog * m_OverlayDialog;
	COLORREF overLayColors[OVERLAYS];
	int overLayYears[OVERLAYS];
	int numPeriods;
	int numtPeriods;
	//Period far *** periods;
	Period far *** tPeriods;
	//CList<CFireDay, CFireDay> *fires;
	CFireDay **fires;
	CFireDay **tFires;
	CForecast forecast;

};

class CPocketCardOptions : public CObject
{
public:
	CPocketCardOptions();
	~CPocketCardOptions();
	void Destroy(void);
// Attributes
public:
	CString	m_Area;
	CString	m_Bitmap;
	CString	m_FuelModel;
	CString	m_Index;
	CString	m_PEText;
	short	m_RH;
	short	m_Temp;
	short	m_WindSpeed;
	short	m_Year1;
	short	m_Year2;
	CString m_Fire1;
	double	m_Fire1Val;
	COleDateTime m_Fire1Date;
	CString m_Fire2;
	double	m_Fire2Val;
	COleDateTime m_Fire2Date;
	CString m_Fire3;
	double	m_Fire3Val;
	COleDateTime m_Fire3Date;
	int periodLength;
	COleDateTime dateTime;
	int numPeriods;
	//Period far *** periods;
	Period far *** ytdPeriods;
	CString dateStamp;
	int extraGreater;
	CString extraStr;
	double cp;
	CString m_IndexShort;
	int yPcnt;
	int gyPcnt;
	int ryPcnt;
	int endYear;
	int startYear;
	short m_LiveFM;
	BOOL m_NWCGStd;
	CString m_Line3;
	CString m_Line2;
	CString m_Line1;
	CString m_PastExp;
	CString m_IndexString;
	CString m_RespAgency;
	CString dbName;
	int varID;
	HDIB areaDIB;
	CPalette *areaPal;
	CRect areaRect;
};

class CClimAnalysis  
{
	friend class CSeasonBinSet;
public:
	double GetIndexValue(int varLoc, COleDateTime iDate);
	long AccumulateStationUserVars(CString stationStr);
	int HasUserVars();
	int AbbrevToVarID(CString str);
	void LoadRunPocketOptions(CRunPocketCardSet *pocketSet);
	void LoadPocketOptions(CPocketCardSet *pocketSet, int _varID);
	//CSplitFrameOptions m_frameOpts;
	CPDOptions m_opts;
	CPocketCardOptions m_pocketOpts;
	CString varNames[MAXVARIDS];
	long userCount;
	//CWnd * caller;
//	NFDRCalcState nfdrsState;
	CanadianState canadaState;
	void GetLastDay(COleDateTime& trg);
	bool checkFires;
	bool inverted;
	//CPtrList auxList[MAXVARIDS];
	CString StatsGraphDataListing(int varID, bool createDoc = true);
	CString FrequencyTable(int loc, bool createDoc = true);
	CString PrecipFrequencyTable(int loc, bool createDoc = true);
	void AddSIGDescription(FILE *stream);
//	void BuildRerapStats(RerapStats rStats[8], int varID, int sMonth, int sDay, int eMonth, int eDay);
	int fireYears;
	void LocateEvents(QueryRow *qRows, int rows, int days);
	void KillFires();
	void CreateLogitFiles(CFireSumSet *fires, int lFire, int mfDay, CString varName, BOOL conditional,
		long *nV, long *nFD, long *nLFD, long *nMFD, long *_discards, long *madeUps);
	void CreateLogitFilesQuick(CFireOptionsSet *fireOptsSet,
		long *nV, long *nFD, long *nLFD, long *nMFD, long *_discards, long *madeUps);
	void GetLogitArraySizesQuick(CFireOptionsSet *fireOptsSet, int vID,
		long *nV, long *nFD, long *nLFD, long *nMFD, long *_discards, long *madeUps);
	void CreateLogitArraysQuick(CFireOptionsSet *fireOptsSet, int vID,
		double *fxVals, double *fyVals, double *lfxVals, double *lfyVals, double *mfxVals, double *mfyVals);
	CString AddDateStamp();
	void SeverityList(double pcntile, bool useLessThan = false, int varLoc = 0);
	void SeveritySummary(double pcntile, bool useLessThan = false, int varLoc = 0);
	void PersistenceProbability(CSeasonBinSet *binSet);
	void ProbabilityAnalysis(CSeasonBinSet *bin1, CSeasonBinSet *bin2, int varLoc1 = 0, int varLoc2 = 1);

	CString DailyListing(ListingOptions *listOpts, bool createDoc = true, bool createTable = true);
	CString GetModelString(CSIGStationSet *stn);
	int nVarIDs;
	int * varIDs;
	CDatabase * pDB;
	int GenerateClimatologyReports(CClimateSet *_climSet);
	CString DataCount(int loc, bool createDoc = true);
	bool useLFI;
	bool useNFDRS;
	bool killPeriods;
	bool useCanadian;
	CPeriodDoc * AddGraphView(CPeriodDoc *pDoc, int varID, int optType, int tnumPeriods = 0, Period ***tperiods = NULL,	CFireDay **tFires = NULL);
	CClimAnalysis(CDatabase *db);
	virtual ~CClimAnalysis();
	INT_PTR RunAnalyze(int *_varIDs, int _nVarIDs, CFireplusSet *_fpSet, bool _inverted, bool _isFPA = false);
	int Analyze(int *_varIDs, int _nVarIDs, CFireplusSet *_fpSet, bool _inverted, bool isFPA = false);// = false);
	int AnalyzeSIG(const CString query, bool isFPA = false);
	Period ***periods;
	CFireDay **fires;
	int numPeriods;
	int GetPeriod(COleDateTime date);
	CClimateSet * climSet;
	//BOOL LaunchAnalysis();
	int AnalyzeBatchItem(CWnd *_caller, CClimateSet *_climSet, CFireplusSet *_fpSet, bool _inverted = false);
	int AnalyzeBatchItem(CWnd *_caller, int *_varIDs, int _nVarIDs, CFireplusSet *_fpSet, bool _inverted, bool isFPA = false);// = false);
	int AnalyzeBatchSIG(const CString query, bool isFPA = false);
	CUPDUPDATA* pCUPDUPData;
	CString FlamMapExport(bool createDoc = true, int calmVal = 1, int startYear = 1900, int startMonth = 1, int startDay = 1, int startHour = 0, int startMinute = 0, 
		int endYear = 1900, int endMonth = 1, int endDay = 1, int endHour = 0, int endMinute = 0, int windType = 0, 
		BOOL monthlyWinds = FALSE, BOOL doWindRose = FALSE, int binSize = 5, int binMax = 30, bool useWeibull = false);
	void AnalyzeQuick(CRawClim *pClim, bool _inverted = false);
	void AnalyzeQuickStation(CRawClim *pClim);
	void AnalyzeQuickSIG(CRawClim *pClim);

	bool autosave;
	CString autosavePath;
	CString StatsTable(int loc, double criticalPercentile, bool createDoc = true);
	CString PrecipTable(int loc, double criticalPercentile, bool createDoc = true);

	CFireplusSet *fpSet;
	CString m_strModel;
	CString m_strStationName;

	bool customInits;
	bool killfpSet;
	//CSIGStationSet staSet;
	long count;
	int m_runID;
	//CString m_docDir;
	// new DL_Output table, 2013
	bool HasDLOutput();
	void CreateDLOutput();

	int humanCause;   // optional human cause filter (bitvector)
	bool m_extremeIsMax[MAXVARIDS];
	bool m_usedExtremes;
private:
	//CCancelDialog *recCounter;
	int AnalyzeStation(CString stationStr, bool isFPA = false, CString sigID = _T(""));

	int AnalyzeBatchStation(CString stationStr, bool isFPA = false, CString sigID = _T(""));
	CWxSet *wxSet;
//	Period ***months;
	COleDateTime *sDates;
	COleDateTime *eDates;
	int startDay;
	
};

class CAnalysisData
{
public:
	CClimAnalysis *pClim;
	int *varIDs;
	int nVarIDs;
	CFireplusSet *pFpSet;
	bool inverted;
	bool isFPA;
};
#endif // !defined(AFX_CLIMANALYSIS_H__DF437B61_D54A_11D1_B8B6_000000000000__INCLUDED_)

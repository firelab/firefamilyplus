// fireplus.h : main header file for the FIREPLUS application
//
//#include "nfdrs.h"	// Added by ClassView
//#include "nfdrs4ctrl1.h"
//#include "CDNFDRS4.h"


#include "CNFDRS.h"
#include "NFDRS4.h"
#include "CTimeZones.h"

class CSIGStationSet;

#if !defined(AFX_FIREPLUS_H__F8C34258_BF4C_11D1_B8AE_000000000000__INCLUDED_)
#define AFX_FIREPLUS_H__F8C34258_BF4C_11D1_B8AE_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "dibapi.h"

// 
// note: must also update  in resources file
//  look for VALUE "ProductVersion"
static char verStr[] = "5.0 build";

//const int BASISYEAR = 2000;
const int BASISYEAR = 1993;
//global functions`
STDAPI MyDaoStringAllocCallback(DWORD dwLen, DWORD pData, void** ppv);
typedef void (CALLBACK* LPLogReg)(char *, char *);
short OptionFromVarID(int varID);
BOOL SaveBitmapFile(HDC hDC, HBITMAP hBmp, PSTR pszFileName);
int MonthChtoInt(CString chMonth);
bool isNFDRS2016(char fm);
int StringToLineStyle(CString str);
int StringToDashLineStyle(CString str);
CString LineStyleToString(int lineStyle);
int DayOfYear(COleDateTime date);
//int DayOfYear(CTime date);
int ScreenPixelsToPrinterPixels(CDC *screenDC, CDC *printerDC, int screenPixels);
char *GetTempFileName();
double Round(double in);
enum DateType {Full, Packed, Reverse, MD, MDPacked};
enum TimeType { FullTime, PackedTime, NoTime};
enum WindType {NFDRS, Azimuth};
enum RowOp {AND, OR };
enum Category {Avg, Sum, Max, Min, Daily};
enum ValType {Value, Percentile};
enum ValOp {EQ, LE, GE, LT, GT };

enum GFile {BMP, PNG, JPG, TIF, GIF};

#define OVERLAYS 3	//max years overlaid on graph
#define MAXMERGES 4 //max 4 vars on a merged graph
#define STATICVARIDS 49
#define MAXVARIDS 100


const int prnFields = 33;
const int firePrnFields = 6;

void DecimalDegree2DDMMSS(double decimalDegree, int *degrees, int *minutes, int *seconds);
void DDMMSS2DecimalDegree(int degrees, int minutes, int seconds, double *decimalDegree);

// added for batch

enum Reports {
	REPORT_NONE		,
	REPORT_WEATHER_CLIMATOLOGY	,
	REPORT_WEATHER_EVENT_LOCATOR	,
	REPORT_WEATHER_FIRE_DANGER	,
	REPORT_WEATHER_HOURLY_LISTINGS	,
	REPORT_WEATHER_DIURNAL_GRAPHS	,
	REPORT_WEATHER_FARSITE_EXPORTS	,
	REPORT_WEATHER_POCKET_CARD	,
	REPORT_WEATHER_PERSISTENCE_PROB	,
	REPORT_WEATHER_SEVERITY_LIST	,
	REPORT_WEATHER_SEVERITY_SUMM	,
	REPORT_WEATHER_PROBABILITY_ANAL	,
	REPORT_WEATHER_DAILY_LISTING	,
	REPORT_WEATHER_PERCENTILE_WEATHER	,
	REPORT_WEATHER_WINDS	,
	REPORT_FIRES_SUMMARY	,
	REPORT_FIRES_ANALYSIS
};

static char *ReportNames[] =
{
"Weather/Climatology",
"Weather/Event Locator",
"Weather/Fire Danger",
"Weather/Hourly Listings",
"Weather/Diurnal Graphs",
"Weather/FARSITE Exports",
"Weather/Pocket Card",
"Weather/Persistence Probability",
"Weather/Severity List",
"Weather/Severity Summary",
"Weather/Probability Analysis",
"Weather/Daily Listing",
"Weather/Percentile Weather",
"Weather/Winds",
"Fires/Summary",
"Fires/Analysis"
};
#define NUM_REPORTS	16

// end added for batch
bool HasUserTables(CDatabase *_pDB);

class ListingOptions
{
public:
	int fCause;
	int lfAcres;
	bool largeFires;
	bool fires;
	bool acres;
	bool numPerClass;
	ListingOptions();
	bool reportHeader;
	bool columnHeader;
	bool dateStamp;
	DateType dt;
	TimeType tt;
	bool stnPerRecord;
};

class QueryRow
{
public:
	CString varString;
	RowOp rowOp;
	int varID;
	Category cat;
	ValOp valOp;
	double value;
	ValType valType;
};
/////////////////////////////////////////////////////////////////////////////
// CFireplusApp:
// See fireplus.cpp for the implementation of this class
//

class CCandidatesFrame;

class CFireplusApp : public CWinApp
{
public:
	//HINSTANCE dllHandle;
	//LPLogReg logReg; //Function ptr
	//void WriteDocOpts();
	//void ReadDocOpts();
	void SetGraphicsExt(CString ext);
	CString GetGraphicsExt();
	CString GetGraphicsFileStr();
	//CNfdrs m_NFDRS;
	//CDNFDRS4 m_NFDRS;
	CNFDRS m_NFDRS;
	NFDRS4 m_NFDRS2016;
	BOOL m_ForceNFDRS2016Recompute;
	BOOL m_UseDailyExtremes;
	int maxMissingWx;
	GFile lastGraphicFormat;
	//int eventDays;
	//int eventHours;
	void SetUserDir(CString fullPath);
	CString userDir;
	int rerapVarID;
	//int nQrows;
//	int pcYVal;
//	int pcGyVal;
//	int pcRyVal;
	CString exeDir;
	CString workDir;
	CString dbDir;
	//CFile readMe;
	CFireplusApp();
	CString m_buildDateStr;
	CString m_buildDateStrPacked;
	ListingOptions lo;
	HDIB pCardGYDIB;
	HDIB pCardRYDIB;
	HDIB pCardYGDIB;
	HDIB pCardYRDIB;
	HDIB pCardYDIB;
	HDIB stoplightDIB;
	HDIB checkDIB;
	CPalette *stoplightPal;
	CPalette *checkPal;
	CRect pCardGYRect;
	CRect pCardRYRect;
	CRect pCardYGRect;
	CRect pCardYRRect;
	CRect pCardYRect;
	CRect stoplightRect;
	CRect checkRect;
	//QueryRow *queryRows;
	//QueryRow *hourlyQueryRows;
	COLORREF m_OverallCP2Color;
	COLORREF m_OverallCP1Color;
	BOOL m_OverallCP2;
	BOOL m_OverallCP1;
	BOOL m_enableFuture;
	int lineStyles[OVERLAYS];
	int lineWidths[OVERLAYS];
	COLORREF overLayColors[OVERLAYS];
	int overLayYears[OVERLAYS];
	COLORREF mergeColors[MAXMERGES];
	int mergeWidths[MAXMERGES];
	int mergeStyles[MAXMERGES];

	// added for batch
   int	curr_runID;
   int curr_batchID;
// end added for batch
   int varSortOrder;
  // CCancelDialog *cd;
   CString m_OnlineHelpURL;
   CTimeZones m_timeZones;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireplusApp)
	public:

	

	virtual BOOL InitInstance();
	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
	virtual int ExitInstance();
	// //gets pointer to an active CCandidatesFrame for a database
	CCandidatesFrame * GetCandidatesFrame(CDatabase * pDB);
	//will update or create a CCandidatesDoc/Frame/View
	void UpdateCandidatesFrame(CDatabase * pDB);

	void SetLandscape();
	//}}AFX_VIRTUAL
    CString getVarSortOrderStr(int i);
// Implementation
	void OverrideMXD(CSIGStationSet* staSet);
	//{{AFX_MSG(CFireplusApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppNASF();
	afx_msg void OnHelpContents();
	afx_msg void OnHelpSearch();
	afx_msg void OnFileNewFfp();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIREPLUS_H__F8C34258_BF4C_11D1_B8AE_000000000000__INCLUDED_)
void DisplayDBException(CDBException* e);
//CDaoRecordSet *dbSet;
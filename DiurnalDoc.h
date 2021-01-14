#if !defined(AFX_DIURNALDOC_H__64D3C9E3_E459_11D3_AB9F_005004BF64C9__INCLUDED_)
#define AFX_DIURNALDOC_H__64D3C9E3_E459_11D3_AB9F_005004BF64C9__INCLUDED_
 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DiurnalDoc.h : header file
//
#include "WxSet.h"
#include "fireplusSet.h"
#include "SIGStationSet.h"

extern int curr_runID;


class Day : public CObject
{
public:
	Day();
	~Day();
	void AddValue(double val, COleDateTime obsTime);
	//void AddValue(double val, CTime obsTime);
	void CleanStorage(bool SetToMode = false);
	Stats hours[24];
};

class CDiurnalAnalysis : public CObject
{
public:
	CString stamp;
	CString modelStr;
	CString staName;
	bool doListing;
	void AnalyzeSIG(const CString query, CFireplusSet *fpSet);
	int AnalyzeStation(CString stationStr, CSIGStationSet& staSet, CWxSet& wxSet);
	CDiurnalAnalysis();
	~CDiurnalAnalysis();
	int Analyze(CWnd *_caller, CFireplusSet *fpSet, int nVars, int *_varIDs);
	int HourlyListing(CWnd *_caller, CFireplusSet *fpSet, int nVars, int *_varIDs, ListingOptions *listOpts, int createTable);
	CString GetModelString(CFireplusSet *fpSet, CSIGStationSet *stn);
	CString AddDateStamp();
	//void LocateEvents(QueryRow *qRows, int rows, int hours, CFireplusSet *fpSet);

	Day **days;//[366][MAXVARIDS];
	//CCancelDialog *recCounter;
	bool useCanadian;
	bool useNFDRS;
	bool useLFI;
	long count;
	int nVarIDs;
	int *varIDs;

	// new HL_Output table, 2015
	bool HasHLOutput(CFireplusSet *fpSet);
	void CreateHLOutput(CFireplusSet *fpSet);

};


/////////////////////////////////////////////////////////////////////////////
// CDiurnalDoc document

class CDiurnalDoc : public CDocument
{
protected:
	CDiurnalDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDiurnalDoc)

// Attributes
public:
	COleDateTime dateTime;
	COleDateTime start;
	COleDateTime end;
	CDiurnalAnalysis * da;
	long nWxObs;
	CString model;
	CString stationName;
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiurnalDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	CString stamp;
	virtual ~CDiurnalDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDiurnalDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIURNALDOC_H__64D3C9E3_E459_11D3_AB9F_005004BF64C9__INCLUDED_)

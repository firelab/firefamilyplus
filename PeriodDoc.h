#if !defined(AFX_PERIODDOC_H__D54ED146_E426_11D1_B8BA_000000000000__INCLUDED_)
#define AFX_PERIODDOC_H__D54ED146_E426_11D1_B8BA_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PeriodDoc.h : header file
//

class CForecast : public CObject
{
	friend class CClimAnalysis;
public:
	bool overlap;
	int startP;
	CForecast();
	~CForecast();
	void CreateStorage(int days, int nVars, int *varIDs);
	void FillBins(CClimAnalysis * analysis);
	int nDays;
	int nBins;
	COleDateTime *dates;
	double *values[MAXVARIDS];
	double *binVals[MAXVARIDS];
	OLE_COLOR lineColor;
	int lineWidth;
	int lineStyle;
};


/////////////////////////////////////////////////////////////////////////////
// CPeriodDoc document
class CPeriodDoc : public CDocument
{
	//friend class COverlayDialog;
protected:
	CPeriodDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPeriodDoc)

// Attributes
public:
	CClimAnalysis *m_pAnalysis;
	//GRAPH OPTIONS
/*	BOOL	m_Horizontal_Lines;
	BOOL	m_Vertical_Lines;
	BOOL	m_Range_Tics;
	BOOL	m_CP_Tics;
	short	m_Line_Width;
	CString	m_H_Line_Style;
	CString	m_V_Line_Style;*/

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPeriodDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	CString StatsGraphDataListing(int varID, bool createDoc = true);

	//CPDOptions m_opts;

	/*CDatabase *pDB;
	CString varNames[MAXVARIDS];
	int HasUserVars();
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
	Period far *** periods;
	Period far *** tPeriods;
	//CList<CFireDay, CFireDay> *fires;
	CFireDay **fires;
	CFireDay **tFires;
	CForecast forecast;*/


	virtual ~CPeriodDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPeriodDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERIODDOC_H__D54ED146_E426_11D1_B8BA_000000000000__INCLUDED_)

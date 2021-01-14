#if !defined(AFX_GRAPHVIEW_H__D54ED148_E426_11D1_B8BA_000000000000__INCLUDED_)
#define AFX_GRAPHVIEW_H__D54ED148_E426_11D1_B8BA_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GraphView.h : header file
//

//graph types
#define BARGRAPH 0
#define LINEGRAPH 1
class Period;
class CFireplusSet;
class CClimAnalysis;

class CGraphOptions : public CObject
{
public:
	int nP;
	COLORREF m_OverallCP2Color;
	COLORREF m_OverallCP1Color;
	BOOL m_OverallCP2;
	BOOL m_OverallCP1;
	bool isBargraph;
	int optionType;
	double criticalPercentile;
	double criticalPercentile2;
	CString varName;
	int varID;

	//GRAPH OPTIONS
	BOOL	m_Horizontal_Lines;
	BOOL	m_Vertical_Lines;
	BOOL	m_Range_Tics;
	BOOL	m_CP_Tics;
	BOOL	m_cp1s;
	BOOL	m_cp2s;
	BOOL	m_maxs;
	BOOL	m_mins;
	short	m_Line_Width;
	short	m_cp1Width;
	short	m_cp2Width;
	short	m_maxsWidth;
	short	m_minsWidth;
	CString	m_H_Line_Style;
	CString	m_V_Line_Style;
	COLORREF m_Line_Color;
	COLORREF m_cp1Color;
	COLORREF m_cp2Color;
	COLORREF m_maxsColor;
	COLORREF m_minsColor;
	short	m_Cause;
	BOOL	m_FD;
	BOOL	m_LFD;
	short	m_LFDAcres;
	BOOL	m_MFD;
	short	m_MFDFires;
	short	m_fxWidth;
	COLORREF m_fxColor;
	int		m_fxStyle;

	//added for batch2
	CFireplusSet *fpSet;

};


/////////////////////////////////////////////////////////////////////////////
// CGraphView view

class CGraphView : public CView
{
public:
	CGraphView();           // protected constructor used by dynamic creation
	~CGraphView();           
	DECLARE_DYNCREATE(CGraphView)

// Attributes
public:
	CGraphOptions m_graphOpts;
	CClimAnalysis *m_pAnalysis;
// Operations
public:
//	double fxStart;
	double overallCP2;
	double overallCP1;
	void PrintGraph();
	double * cps;
	double * cp2s;
	double * mins;
	double * maxs;
	int optionType;
	//virtual void DrawToDCRegion(CDC *pDC, CRect rect);
	virtual void DrawToDCRegion(CDC *destDC, CRect rect);
	//void DrawPathOutline(const COLORREF& c, CDC& dc, bool Round, int PenSize);
	void PrintToDC(CDC *pDC);
	int gap;
	int barwidth;
	double ideal;
	double ymax;
	double ymin;
	//int graphType;
	double IdealFromYMax(double yMax);
	int varID;
	Period ** pcpSums;
	Period ** pcpAuxSums;
	// added for batch2
	//char *statsfName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL
	CFireplusSet *fpSet;
// Implementation
protected:
//	virtual ~CGraphView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnOptionsGraphtypeLineataverage();
	afx_msg void OnOptionsGraphtypeStatisticalbars();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
	afx_msg void	OnViewHeader();
	afx_msg void OnExportData();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SaveGraph(void);
	void SaveGraph(CString _outName, CRect *_outRect);
	void PreDrawCalcs();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHVIEW_H__D54ED148_E426_11D1_B8BA_000000000000__INCLUDED_)

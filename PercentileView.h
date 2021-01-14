#if !defined(AFX_PERCENTILEVIEW_H__D54ED149_E426_11D1_B8BA_000000000000__INCLUDED_)
#define AFX_PERCENTILEVIEW_H__D54ED149_E426_11D1_B8BA_000000000000__INCLUDED_

#include "WxSet.h"	// Added by ClassView
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PercentileView.h : header file
//

class CClimAnalysis;

#include "GraphView.h"
#include "FireStats.h"
/////////////////////////////////////////////////////////////////////////////
// CPercentileView view

class CPercentileView : public CView
{
public:
	CPercentileView();           // protected constructor used by dynamic creation
	~CPercentileView();
	DECLARE_DYNCREATE(CPercentileView)

// Attributes
public:
	CClimAnalysis *m_pAnalysis;

// Operations
public:
	void PrintGraph();
	virtual void DrawToDCRegion(CDC *pDC, CRect rect);
	void PrintToDC(CDC *pDC);
	//CGraphOptions *m_graphOpts;
	int optionType;
	double criticalPercentile;
	double criticalPercentile2;
	CString varName;
	double xmax;
	double xmin;
	double ideal;
	double IdealFromXMax(double xMax);
	Stats stats;
	int varID;

	double filterValue;
	FILTER_DIRECTION filterDirection;
	int nFiltered;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPercentileView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
//	virtual ~CPercentileView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPercentileView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SaveGraph(void);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERCENTILEVIEW_H__D54ED149_E426_11D1_B8BA_000000000000__INCLUDED_)

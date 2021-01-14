#if !defined(AFX_FIRESUMVIEW_H__53A132C8_6E7B_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_FIRESUMVIEW_H__53A132C8_6E7B_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FireSumView.h : header file
//

class 	CFireSummary;
/////////////////////////////////////////////////////////////////////////////
// CFireSumView view

class CFireSumView : public CView
{
protected:
	CFireSumView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFireSumView)

// Attributes
public:

// Operations
public:
	CFireSummary *m_pFiresSummary;
	double totAcres;
	long totFires;
	long firesMax;
	double acresMax;
	int yrSkip;
	void DrawToDCRegion(CDC *pDC, CRect rect);
	void IdealFromYMax(long *fMax, double *aMax, long *fIdeal, double *aIdeal);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireSumView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFireSumView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFireSumView)
	afx_msg void OnFilePrint();
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIRESUMVIEW_H__53A132C8_6E7B_11D2_B8E5_000000000000__INCLUDED_)

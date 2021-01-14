#if !defined(AFX_CUMULATIVEBARS_H__168D89A1_7A07_11D2_BD5A_90F0AB3C243D__INCLUDED_)
#define AFX_CUMULATIVEBARS_H__168D89A1_7A07_11D2_BD5A_90F0AB3C243D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CumulativeBars.h : header file
//
class CFireStats;
/////////////////////////////////////////////////////////////////////////////
// CCumulativeBars view

class CCumulativeBars : public CView
{
protected:
	CCumulativeBars();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCumulativeBars)
	//CFireStats *m_fires;
// Attributes
public:

// Operations
public:
	void PrintGraph();
	CFireStats * m_fires;
	long ymax;
	long * all;
	long * fd;
	long * lfd;
	long * mfd;
	int nBins;
	int iStart;
	int iEnd;
	void DrawToDCRegion(CDC *pDC, CRect rect);
	double IdealFromXMax(double xmax);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCumulativeBars)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCumulativeBars();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CCumulativeBars)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnFilePrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUMULATIVEBARS_H__168D89A1_7A07_11D2_BD5A_90F0AB3C243D__INCLUDED_)

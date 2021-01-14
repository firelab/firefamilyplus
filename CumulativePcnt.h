#if !defined(AFX_CUMULATIVEPCNT_H__168D89A2_7A07_11D2_BD5A_90F0AB3C243D__INCLUDED_)
#define AFX_CUMULATIVEPCNT_H__168D89A2_7A07_11D2_BD5A_90F0AB3C243D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CumulativePcnt.h : header file
//
class CFireStats;

/////////////////////////////////////////////////////////////////////////////
// CCumulativePcnt view

class CCumulativePcnt : public CView
{
protected:
	CCumulativePcnt();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCumulativePcnt)

// Attributes
public:

// Operations
public:
	void PrintGraph();
	void DrawToDCRegion(CDC *pDC, CRect rect);
	double IdealFromXMax(double xmax);
	CFireStats * m_fires;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCumulativePcnt)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCumulativePcnt();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CCumulativePcnt)
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

#endif // !defined(AFX_CUMULATIVEPCNT_H__168D89A2_7A07_11D2_BD5A_90F0AB3C243D__INCLUDED_)

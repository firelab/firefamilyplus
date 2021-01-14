#if !defined(AFX_FIRESTATGRAPH_H__AA2A8F38_7704_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_FIRESTATGRAPH_H__AA2A8F38_7704_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FireStatGraph.h : header file
//
class CFireStats;
/////////////////////////////////////////////////////////////////////////////
// CFireStatGraph view

class CFireStatGraph : public CView
{
protected:
	CFireStatGraph();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFireStatGraph)

// Attributes
public:

// Operations
public:
	CFireStats * fires;
	void PrintGraph();
	void DrawToDCRegion(CDC *pDC, CRect rect);
//	void Star(CDC *pDC, int x, int y, int starSize, CPen *pen);
	double IdealFromXMax(double xmax);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireStatGraph)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//void DrawPathOutline(const COLORREF& c, CDC& dc, bool Round, int PenSize);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFireStatGraph();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFireStatGraph)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnFilePrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIRESTATGRAPH_H__AA2A8F38_7704_11D2_B8E5_000000000000__INCLUDED_)

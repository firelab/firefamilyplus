#if !defined(AFX_DIURNALGRAPHVIEW_H__64D3C9E6_E459_11D3_AB9F_005004BF64C9__INCLUDED_)
#define AFX_DIURNALGRAPHVIEW_H__64D3C9E6_E459_11D3_AB9F_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DiurnalGraphView.h : header file
//
#include "wxSet.h"

/////////////////////////////////////////////////////////////////////////////
// CDiurnalGraphView view

class CDiurnalGraphView : public CView
{
protected:
	CDiurnalGraphView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDiurnalGraphView)

// Attributes
public:
	double ideal;
	double ymax;
	Stats hours[24];
// Operations
public:
	void PrintGraph();
	virtual void DrawToDCRegion(CDC *destDC, CRect rect);
	double IdealFromYMax();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiurnalGraphView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDiurnalGraphView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDiurnalGraphView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIURNALGRAPHVIEW_H__64D3C9E6_E459_11D3_AB9F_005004BF64C9__INCLUDED_)

#if !defined(AFX_DPPROBVIEW_H__CED90D05_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_DPPROBVIEW_H__CED90D05_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DPProbView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDPProbView view

class CDPProbView : public CView
{
protected:
	CDPProbView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDPProbView)

// Attributes
public:

// Operations
public:
	void PrintGraph();
	void DrawToDCRegion(CDC *pDC, CRect rect);
	double IdealFromXMax(double xmax);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDPProbView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDPProbView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDPProbView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DPPROBVIEW_H__CED90D05_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_)

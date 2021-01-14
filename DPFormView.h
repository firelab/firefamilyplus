#if !defined(AFX_DPFORMVIEW_H__CED90D06_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_DPFORMVIEW_H__CED90D06_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DPFormView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDPFormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CDPFormView : public CView
{
protected:
	CDPFormView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDPFormView)

// Attributes
public:

// Operations
public:
	void PrintGraph();

	void DrawToDCRegion(CDC *pDC, CRect rect);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDPFormView)
	protected:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDPFormView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDPFormView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DPFORMVIEW_H__CED90D06_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_)

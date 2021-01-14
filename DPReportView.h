#if !defined(AFX_DPREPORTVIEW_H__CED90D02_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_DPREPORTVIEW_H__CED90D02_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DPReportView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDPReportView view

class CDPReportView : public CEditView
{
protected:
	CDPReportView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDPReportView)

// Attributes
public:

// Operations
public:
	void SaveReport(CString fName);
	CFont fixedFont;
	CString header;
	bool doRound;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDPReportView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDPReportView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDPReportView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DPREPORTVIEW_H__CED90D02_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_)

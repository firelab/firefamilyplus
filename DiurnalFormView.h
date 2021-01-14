#if !defined(AFX_DIURNALFORMVIEW_H__D9445322_385B_11D4_ABA1_005004BF64C9__INCLUDED_)
#define AFX_DIURNALFORMVIEW_H__D9445322_385B_11D4_ABA1_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DiurnalFormView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDiurnalFormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CDiurnalFormView : public CFormView
{
protected:
	CDiurnalFormView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDiurnalFormView)

// Form Data
public:
	//{{AFX_DATA(CDiurnalFormView)
	enum { IDD = IDD_DIURNALFORM };
	CRichEditCtrl	richEditCtl;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void SaveTheDamnThing();
	void PrintTheDamnThing();
	char * fName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiurnalFormView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDiurnalFormView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDiurnalFormView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMsgfilterRichedit1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIURNALFORMVIEW_H__D9445322_385B_11D4_ABA1_005004BF64C9__INCLUDED_)

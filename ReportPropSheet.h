#if !defined(AFX_REPORTROPSHEET_H__D7711A65_675B_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_REPORTROPSHEET_H__D7711A65_675B_11D2_B8E5_000000000000__INCLUDED_

#include "ReportOptionsSet.h"	// Added by ClassView
#include "ReportHeaderProp.h"
#include "ReportDelimiterProp.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ReportPropSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReportPropSheet
class CMainFrame;

class CReportHeaderProp;
class CReportDelimiterProp;

class CReportPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CReportPropSheet)

// Construction
public:
	CReportPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0,
		CDatabase *_pDB = NULL, CMainFrame *_pFrame = NULL);

// Attributes
public:

// Operations
public:
	CButton okButton;
	CButton cancelButton;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportPropSheet)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	
	CReportOptionsSet reportSet;
	CMainFrame * mainFrame;
	
	void OnMyOK();
	void OnMyCancel();
	
	CReportHeaderProp rhp;
	CReportDelimiterProp rdp;

	virtual ~CReportPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CReportPropSheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTPROPSHEET_H__D7711A65_675B_11D2_B8E5_000000000000__INCLUDED_)

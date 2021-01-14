#if !defined(AFX_BATCHPROPSHEET_H__D7711A65_675B_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_BATCHPROPSHEET_H__D7711A65_675B_11D2_B8E5_000000000000__INCLUDED_
#include "graphset.h"
#include "batchoptionsset.h"
#include "BatchAutoSaveProp.h"
#include "BatchGeneralProp.h"

/* 
#include "BatchOptionsSet.h"	// Added by ClassView
*/

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BatchPropSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBatchPropSheet
class CMainFrame;
class CBatchAutoSaveProp;
class CBatchGeneralProp;





class CBatchPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CBatchPropSheet)

// Construction
public:
	CBatchPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0,
		CDatabase *_pDB = NULL, CMainFrame *_pFrame = NULL);

// Attributes
public:

// Operations
public:
	CButton okButton;
	CButton cancelButton;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchPropSheet)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	CGraphSet graphSet;
	CBatchOptionsSet boptSet;

	// CBatchOptionsSet batchSet;
	CMainFrame * mainFrame;
	
	void OnMyOK();
	void OnMyCancel();
	
	CBatchAutoSaveProp basp;
	CBatchGeneralProp bgen;

	virtual ~CBatchPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBatchPropSheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATCHPROPSHEET_H__D7711A65_675B_11D2_B8E5_000000000000__INCLUDED_)

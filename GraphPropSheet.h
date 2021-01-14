#if !defined(AFX_GRAPHPROPSHEET_H__D7711A65_675B_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_GRAPHPROPSHEET_H__D7711A65_675B_11D2_B8E5_000000000000__INCLUDED_
#include "GraphSet.h"	// Added by ClassView
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GraphPropSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGraphPropSheet
class CMainFrame;
class CGraphAutoSaveProp;

class CGraphPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CGraphPropSheet)

// Construction
public:
	CGraphPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0,
		CDatabase *_pDB = NULL, CMainFrame *_pFrame = NULL);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphPropSheet)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	void ApplyToGraph(CSplitterFrame *frame);
	CGraphSet graphSet;
	CMainFrame * mainFrame;
	void OnApplyToAll();
	void OnApply();
	void OnSetAsDefaults();
	void OnMyOK();
	void OnMyCancel();
	CButton setDefButton;
	CButton okButton;
	CButton cancelButton;
	CButton applyButton;
	CButton applyToAllButton;
	
	CGeneralGraphProp gp;
	CLineGraphProp lp;

	CBargraphProp bp;
	CGraphAutoSaveProp asp;
	CGraphFiresPage fp;
	CFDProjPage pp;
//	CMergedVarsPropPage mp;
	virtual ~CGraphPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphPropSheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHPROPSHEET_H__D7711A65_675B_11D2_B8E5_000000000000__INCLUDED_)

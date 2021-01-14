#if !defined(AFX_FORCASTGRAPHSDIALOG_H__8B763C62_AEE2_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_FORCASTGRAPHSDIALOG_H__8B763C62_AEE2_11D3_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ForcastGraphsDialog.h : header file
//
#include "ForecastDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CForcastGraphsDialog dialog

class CForcastGraphsDialog : public CDialog
{
// Construction
public:
	int nVars;
	int * varIDs;
	CForcastGraphsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CForcastGraphsDialog)
	enum { IDD = IDD_FORECASTGRAPHSDIALOG };
	CListBox	selectedList;
	CListBox	fieldsList;
	CForecastDialog*	fd;
	BOOL	bReport;
	BOOL	bCloseForecast;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CForcastGraphsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CForcastGraphsDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkFieldslist();
	afx_msg void OnDblclkSelectedlist();
	afx_msg void OnMoveleft();
	afx_msg void OnMoveright();
	afx_msg void OnSelectall();
	afx_msg void OnRemoveall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORCASTGRAPHSDIALOG_H__8B763C62_AEE2_11D3_BD5A_00A0CC24A3EB__INCLUDED_)

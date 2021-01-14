//{{AFX_INCLUDES()
//#include "pvnumeric.h"
//}}AFX_INCLUDES
#include "afxcmn.h"
#if !defined(AFX_SEVERITYDIALOG_H__2516B823_1A5C_11D2_B8CD_000000000000__INCLUDED_)
#define AFX_SEVERITYDIALOG_H__2516B823_1A5C_11D2_B8CD_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SeverityDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSeverityDialog dialog

class CSeverityDialog : public CDialog
{
// Construction
public:
	CFireplusSet *fpSet;
	int doSummary;
	CSeverityDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL, int _doSummary = 0);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSeverityDialog)
	enum { IDD = IDD_SEVERITYDIALOG };
	CComboBox	indexList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeverityDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSeverityDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnVarSort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CSpinButtonCtrl m_spinCP;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEVERITYDIALOG_H__2516B823_1A5C_11D2_B8CD_000000000000__INCLUDED_)

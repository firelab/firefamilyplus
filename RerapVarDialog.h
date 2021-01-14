#if !defined(AFX_RERAPVARDIALOG_H__7C900AA1_9FD9_11D2_BD5A_E44CCA8A1405__INCLUDED_)
#define AFX_RERAPVARDIALOG_H__7C900AA1_9FD9_11D2_BD5A_E44CCA8A1405__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RerapVarDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRerapVarDialog dialog

class CRerapVarDialog : public CDialog
{
// Construction
public:
	CRerapVarDialog(CWnd* pParent = NULL);   // standard constructor
	long varID;
// Dialog Data
	//{{AFX_DATA(CRerapVarDialog)
	enum { IDD = IDD_RERAPVARDIALOG };
	CComboBox	vars;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRerapVarDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRerapVarDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RERAPVARDIALOG_H__7C900AA1_9FD9_11D2_BD5A_E44CCA8A1405__INCLUDED_)

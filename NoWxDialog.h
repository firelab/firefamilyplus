#if !defined(AFX_NOWXDIALOG_H__8C23CA40_F963_11D2_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_NOWXDIALOG_H__8C23CA40_F963_11D2_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NoWxDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNoWxDialog dialog

class CNoWxDialog : public CDialog
{
// Construction
public:
	bool ignore;
	CNoWxDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNoWxDialog)
	enum { IDD = IDD_NOWXDIALOG };
	CEdit	fireValStr;
	CButton	ignoreButton;
	CString	fireName;
	CString	fireDate;
	double	fireVal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNoWxDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNoWxDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnAssignVal();
	afx_msg void OnIgnore();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOWXDIALOG_H__8C23CA40_F963_11D2_BD5A_00A0CC24A3EB__INCLUDED_)

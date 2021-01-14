#if !defined(AFX_RICHDIALOG_H__0C837C43_0DBD_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_RICHDIALOG_H__0C837C43_0DBD_11D3_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RichDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRichDialog dialog

class CRichDialog : public CDialog
{
// Construction
public:
	CString fileName;
	CRichDialog(CWnd* pParent = NULL, CString _fileName = "");   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRichDialog)
	enum { IDD = IDD_RICHDIALOG };
	CRichEditCtrl	editControl;
	CString	richString;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRichDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPrintbutton();
	afx_msg void OnSavebutton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHDIALOG_H__0C837C43_0DBD_11D3_BD5A_00A0CC24A3EB__INCLUDED_)

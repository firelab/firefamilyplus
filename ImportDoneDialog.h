#if !defined(AFX_IMPORTDONEDIALOG_H__3C88F420_01F5_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_IMPORTDONEDIALOG_H__3C88F420_01F5_11D3_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportDoneDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImportDoneDialog dialog

class CImportDoneDialog : public CDialog
{
// Construction
public:
	bool killLog;
	CString logFileName;
	CString statusStr;
	CImportDoneDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImportDoneDialog)
	enum { IDD = IDD_IMPORTDONEDIALOG };
	CStatic	textArea;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportDoneDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportDoneDialog)
	afx_msg void OnViewlogbutton();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTDONEDIALOG_H__3C88F420_01F5_11D3_BD5A_00A0CC24A3EB__INCLUDED_)

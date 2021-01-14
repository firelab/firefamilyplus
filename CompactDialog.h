#if !defined(AFX_COMPACTDIALOG_H__E1F21140_09D8_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_COMPACTDIALOG_H__E1F21140_09D8_11D3_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompactDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCompactDialog dialog

class CCompactDialog : public CDialog
{
// Construction
public:
	//BOOL doDupCheck;
	CCompactDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCompactDialog)
	enum { IDD = IDD_COMPACTDIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompactDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCompactDialog)
	virtual BOOL OnInitDialog();
	//afx_msg void OnDupcheck();
	//afx_msg void OnNodupcheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPACTDIALOG_H__E1F21140_09D8_11D3_BD5A_00A0CC24A3EB__INCLUDED_)

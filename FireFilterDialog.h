#if !defined(AFX_FIREFILTERDIALOG_H__547DEF40_0640_11D4_ABA0_005004BF64C9__INCLUDED_)
#define AFX_FIREFILTERDIALOG_H__547DEF40_0640_11D4_ABA0_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FireFilterDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFireFilterDialog dialog

class CFireFilterDialog : public CDialog
{
// Construction
public:
	CFireFilterDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFireFilterDialog)
	enum { IDD = IDD_FIRESFILTERDIALOG };
	BOOL	type11;
	BOOL	type12;
	BOOL	type13;
	BOOL	type14;
	BOOL	type21;
	BOOL	type22;
	BOOL	type23;
	BOOL	type48;
	BOOL	type49;
	BOOL	type15;
	BOOL	type16;
	BOOL	type19;
	BOOL	type25;
	BOOL	type26;
	BOOL	type37;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireFilterDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFireFilterDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSelectAll();
	afx_msg void OnBnClickedButtonClearAll();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIREFILTERDIALOG_H__547DEF40_0640_11D4_ABA0_005004BF64C9__INCLUDED_)

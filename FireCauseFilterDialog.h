#if !defined(AFX_FireCauseFilterDIALOG_H__547DEF40_0640_11D4_ABA0_005004BF64C9__INCLUDED_)
#define AFX_FireCauseFilterDIALOG_H__547DEF40_0640_11D4_ABA0_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FireCauseFilterDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFireCauseFilterDialog dialog

class CFireCauseFilterDialog : public CDialog
{
// Construction
public:
	CFireCauseFilterDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFireCauseFilterDialog)
	enum { IDD = IDD_FIRECAUSEFILTERDIALOG };
	BOOL	cause1;   // lightning 
	BOOL	cause2;
	BOOL	cause3;
	BOOL	cause4;
	BOOL	cause5;
	BOOL	cause6;
	BOOL	cause7;
	BOOL	cause8;
	BOOL	cause9;
	BOOL	cause10;
	//}}AFX_DATA

	long causeFlag;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireCauseFilterDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFireCauseFilterDialog)
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

#endif // !defined(AFX_FireCauseFilterDIALOG_H__547DEF40_0640_11D4_ABA0_005004BF64C9__INCLUDED_)

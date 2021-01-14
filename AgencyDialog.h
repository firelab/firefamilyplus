#if !defined(AFX_AGENCYDIALOG_H__47C691A0_882C_11D2_BD5A_8F1833BA4F3D__INCLUDED_)
#define AFX_AGENCYDIALOG_H__47C691A0_882C_11D2_BD5A_8F1833BA4F3D__INCLUDED_

#include "FireAgencySet.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgencyDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAgencyDialog dialog

class CAgencyDialog : public CDialog
{
// Construction
public:
	void FillAgencies();
	CFireAgencySet agencySet;
	CAgencyDialog(CWnd* pParent = NULL, CDatabase *_pDB = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAgencyDialog)
	enum { IDD = IDD_AGENCYDIALOG };
	CButton	regButton;
	CButton	newButton;
	CButton	editButton;
	CButton	delButton;
	CListBox	agencies;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgencyDialog)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDatabase *pDB;
	long nextID;
	// Generated message map functions
	//{{AFX_MSG(CAgencyDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelbutton();
	afx_msg void OnEditbutton();
	afx_msg void OnNewbutton();
	afx_msg void OnRegbutton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CNewAgencyDialog dialog

class CNewAgencyDialog : public CDialog
{
// Construction
public:
	CNewAgencyDialog(CWnd* pParent = NULL, CFireAgencySet *_agencySet = NULL, bool _isNew = false, long id = 0);   // standard constructor
	CFireAgencySet *agencySet;
// Dialog Data
	//{{AFX_DATA(CNewAgencyDialog)
	enum { IDD = IDD_NEWAGENCYDIALOG };
	CEdit	name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewAgencyDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool isNew;
	long nextID;
	// Generated message map functions
	//{{AFX_MSG(CNewAgencyDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGENCYDIALOG_H__47C691A0_882C_11D2_BD5A_8F1833BA4F3D__INCLUDED_)

#if !defined(AFX_REGIONSDIALOG_H__47C691A5_882C_11D2_BD5A_8F1833BA4F3D__INCLUDED_)
#define AFX_REGIONSDIALOG_H__47C691A5_882C_11D2_BD5A_8F1833BA4F3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegionsDialog.h : header file
//
#include "stdafx.h"
/////////////////////////////////////////////////////////////////////////////
// CRegionsDialog dialog

class CRegionsDialog : public CDialog
{
// Construction
public:
	void FillRegions();
	long agencyID;
	CString agencyName;
	CFireRegionSet regSet;
	CRegionsDialog(CWnd* pParent = NULL, CDatabase *_pDB = NULL, CString _agencyName = "",
		long _agencyID = 0);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegionsDialog)
	enum { IDD = IDD_REGIONSDIALOG };
	CListBox	regions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegionsDialog)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDatabase *pDB;
	long nextID;

	// Generated message map functions
	//{{AFX_MSG(CRegionsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelbutton();
	afx_msg void OnEditbutton();
	afx_msg void OnNewbutton();
	afx_msg void OnUnitsbutton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CNewRegDialog dialog

class CNewRegDialog : public CDialog
{
// Construction
public:
	CNewRegDialog(CWnd* pParent = NULL, CFireRegionSet *_regSet = NULL, bool _isNew = false, long id = 0, long _agencyID = 0);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewRegDialog)
	enum { IDD = IDD_NEWREGDIALOG };
	CEdit	name;
	CEdit	code;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewRegDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool isNew;
	long nextID;
	long agencyID;
	CFireRegionSet *regSet;

	// Generated message map functions
	//{{AFX_MSG(CNewRegDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONSDIALOG_H__47C691A5_882C_11D2_BD5A_8F1833BA4F3D__INCLUDED_)

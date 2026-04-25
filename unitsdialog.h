#if !defined(AFX_UNITSDIALOG_H__B6508EE3_88FA_11D2_BD5A_D15BBACE663D__INCLUDED_)
#define AFX_UNITSDIALOG_H__B6508EE3_88FA_11D2_BD5A_D15BBACE663D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UnitsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUnitsDialog dialog

class CUnitsDialog : public CDialog
{
// Construction
public:
	CUnitsDialog(CWnd* pParent = NULL, CDatabase *_pDB = NULL, CString _regName = "",
		long _regID = 0);   // standard constructor
	void FillUnits();
	long regID;
	CString regName;
	CFireUnitSet unitSet;

// Dialog Data
	//{{AFX_DATA(CUnitsDialog)
	enum { IDD = IDD_UNITSDIALOG };
	CListBox	units;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnitsDialog)
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
	//{{AFX_MSG(CUnitsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelbutton();
	afx_msg void OnEditbutton();
	afx_msg void OnNewbutton();
	afx_msg void OnSubunitsbutton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CNewUnitDialog dialog

class CNewUnitDialog : public CDialog
{
// Construction
public:
	CNewUnitDialog(CWnd* pParent = NULL, CFireUnitSet *_unitSet = NULL, bool _isNew = false, long id = 0, long _regID = 0);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewUnitDialog)
	enum { IDD = IDD_NEWUNITDIALOG };
	CEdit	name;
	CEdit	code;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewUnitDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool isNew;
	long nextID;
	long regID;
	CFireUnitSet *unitSet;

	// Generated message map functions
	//{{AFX_MSG(CNewUnitDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNITSDIALOG_H__B6508EE3_88FA_11D2_BD5A_D15BBACE663D__INCLUDED_)

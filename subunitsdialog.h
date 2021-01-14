#if !defined(AFX_SUBUNITSDIALOG_H__B6508EE5_88FA_11D2_BD5A_D15BBACE663D__INCLUDED_)
#define AFX_SUBUNITSDIALOG_H__B6508EE5_88FA_11D2_BD5A_D15BBACE663D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SubunitsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSubunitsDialog dialog

class CSubunitsDialog : public CDialog
{
// Construction
public:
	CSubunitsDialog(CWnd* pParent = NULL, CDatabase *_pDB = NULL, CString _unitName = "",
		long _unitID = 0L);   // standard constructor
	void FillSubunits();
	long unitID;
	CString unitName;
	CFireSubunitSet subunitSet;

// Dialog Data
	//{{AFX_DATA(CSubunitsDialog)
	enum { IDD = IDD_SUBUNITSDIALOG };
	CListBox	subunits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubunitsDialog)
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
	//{{AFX_MSG(CSubunitsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelbutton();
	afx_msg void OnEditbutton();
	afx_msg void OnNewbutton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CNewSubunitDialog dialog

class CNewSubunitDialog : public CDialog
{
// Construction
public:
	CNewSubunitDialog(CWnd* pParent = NULL, CFireSubunitSet *_subunitSet = NULL, bool _isNew = false, long id = 0, long _unitID = 0);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewSubunitDialog)
	enum { IDD = IDD_NEWSUBUNITDIALOG };
	CEdit	name;
	CEdit	code;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewSubunitDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool isNew;
	long nextID;
	long unitID;
	CFireSubunitSet *subunitSet;

	// Generated message map functions
	//{{AFX_MSG(CNewSubunitDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBUNITSDIALOG_H__B6508EE5_88FA_11D2_BD5A_D15BBACE663D__INCLUDED_)

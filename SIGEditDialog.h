#if !defined(AFX_SIGEDITDIALOG_H__D54ED143_E426_11D1_B8BA_000000000000__INCLUDED_)
#define AFX_SIGEDITDIALOG_H__D54ED143_E426_11D1_B8BA_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SIGEditDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSIGEditDialog dialog

class CSIGEditDialog : public CDialog
{
// Construction
public:
	CSIGSet * sigSet;
	bool isNew;
	CSIGEditDialog(CSIGSet *_sigSet, CWnd* pParent = NULL, bool newSIG = false);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSIGEditDialog)
	enum { IDD = IDD_EDITSIGDIALOG };
	CButton	moveRight;
	CButton	moveLeft;
	CEdit	SIGName;
	CEdit	description;
	CListBox	stationList;
	CListBox	selectedList;
	CString	nameString;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSIGEditDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSIGEditDialog)
	afx_msg void OnDblclkList2();
	afx_msg void OnDblclkList1();
	afx_msg void OnMoveleft();
	afx_msg void OnMoveright();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSetweights();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIGEDITDIALOG_H__D54ED143_E426_11D1_B8BA_000000000000__INCLUDED_)

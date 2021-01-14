#if !defined(AFX_USERIMPORTDIALOG_H__65793153_E609_4D11_9FCD_AC326DF8A545__INCLUDED_)
#define AFX_USERIMPORTDIALOG_H__65793153_E609_4D11_9FCD_AC326DF8A545__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserImportDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserImportDialog dialog

class CUserImportDialog : public CDialog
{
// Construction
public:
	CUserImportDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUserImportDialog)
	enum { IDD = IDD_USER_IMPORT_DIALOG };
	CButton	m_readStaCtrl;
	CButton	m_date1Ctrl;
	CListBox	m_selectedFields;
	CListBox	m_availFields;
	CComboBox	m_stationCombo;
	CString	m_FileName;
	BOOL	m_comma;
	BOOL	m_space;
	BOOL	m_tab;
	BOOL	m_semicolon;
	//}}AFX_DATA
	CDatabase * pDB;
	CBitmapButton upButton;
	CBitmapButton downButton;
	CBitmapButton topButton;
	CBitmapButton bottomButton;
	DateType dateType;
//	BOOL isSpace;
	BOOL singleStation;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserImportDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnTop();
	afx_msg void OnBottom();
	// Generated message map functions
	//{{AFX_MSG(CUserImportDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnMoveleft();
	afx_msg void OnMoveright();
	afx_msg void OnSelectall();
	afx_msg void OnReadStation();
	afx_msg void OnSingleStation();
	afx_msg void OnDate1();
	afx_msg void OnDate2();
	afx_msg void OnDate3();
	afx_msg void OnDblclkFieldslist();
	afx_msg void OnDblclkSelectedlist();
	afx_msg void OnFilebutton();
	afx_msg void OnImportbutton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERIMPORTDIALOG_H__65793153_E609_4D11_9FCD_AC326DF8A545__INCLUDED_)

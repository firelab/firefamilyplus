#if !defined(AFX_LOGITDIALOG_H__AA2A8F33_7704_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_LOGITDIALOG_H__AA2A8F33_7704_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LogitDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogitDialog dialog

class CLogitDialog : public CDialog
{
// Construction
public:
	CLogitDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLogitDialog)
	enum { IDD = IDD_LOGITDIALOG };
	CEdit	messageString;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogitDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLogitDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGITDIALOG_H__AA2A8F33_7704_11D2_B8E5_000000000000__INCLUDED_)

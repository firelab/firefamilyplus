#if !defined(AFX_TEXTDIALOG_H__45206520_C085_11D2_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_TEXTDIALOG_H__45206520_C085_11D2_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextDialog dialog

class CTextDialog : public CDialog
{
// Construction
public:
	CTextDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTextDialog)
	enum { IDD = IDD_TEXTDIALOG };
	CString	textString;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTDIALOG_H__45206520_C085_11D2_BD5A_00A0CC24A3EB__INCLUDED_)

//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_USERVARSDIALOG_H__BB0D3451_3B66_4DA2_8185_A22D50377D8C__INCLUDED_)
#define AFX_USERVARSDIALOG_H__BB0D3451_3B66_4DA2_8185_A22D50377D8C__INCLUDED_
//#include "UGCtrl.h"
#include "UserVarsCUG.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserVarsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserVarsDialog dialog

class CUserVarsDialog : public CDialog
{
// Construction
public:
	void DeleteUserVar(long Row);
	CDatabase * pDB;
	CUserVarsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUserVarsDialog)
	enum { IDD = IDD_USERVARS_DIALOG };
	//}}AFX_DATA
	UserVarsCUG m_grid;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserVarsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserVarsDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDeletevar();
	afx_msg void OnNewvar();
	//afx_msg void OnBeforeDeleteRowDatatabledb1(long Row, long FAR* Status);
	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERVARSDIALOG_H__BB0D3451_3B66_4DA2_8185_A22D50377D8C__INCLUDED_)

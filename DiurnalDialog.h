#if !defined(AFX_DIURNALDIALOG_H__E14DF980_D240_11D3_AB9F_005004BF64C9__INCLUDED_)
#define AFX_DIURNALDIALOG_H__E14DF980_D240_11D3_AB9F_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DiurnalDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDiurnalDialog dialog

class CDiurnalDialog : public CDialog
{
// Construction
public:
	CDiurnalDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL);   // standard constructor

	CBitmapButton upButton;
	CBitmapButton downButton;
	CBitmapButton topButton;
	CBitmapButton bottomButton;
// Dialog Data
	//{{AFX_DATA(CDiurnalDialog)
	enum { IDD = IDD_DIURNALDIALOG };
	CListBox	destList;
	CListBox	varList;
	//}}AFX_DATA
	CFireplusSet *fpSet;

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiurnalDialog)
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
	//{{AFX_MSG(CDiurnalDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnMoveleft();
	afx_msg void OnMoveright();
	afx_msg void OnRemoveall();
	afx_msg void OnSelectall();
	afx_msg void OnDblclkVarlist();
	afx_msg void OnDblclkList2();
	afx_msg void OnVarSort();

	

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIURNALDIALOG_H__E14DF980_D240_11D3_AB9F_005004BF64C9__INCLUDED_)

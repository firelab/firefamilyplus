//{{AFX_INCLUDES()
#include "afxcmn.h"
#include "numspinctrl.h"
//}}AFX_INCLUDES
#if !defined(AFX_DELRECSDIALOG_H__5CDA54A1_FFCC_11D3_ABA0_005004BF64C9__INCLUDED_)
#define AFX_DELRECSDIALOG_H__5CDA54A1_FFCC_11D3_ABA0_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DelRecsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDelRecsDialog dialog

class CDelRecsDialog : public CDialog
{
// Construction
public:
	long maxRec;
	long curRec;
	CRecordset * records;
	CDelRecsDialog(CWnd* pParent = NULL, CRecordset *_records = NULL, long _maxRec = 0, long _curRec = 0, BOOL doDelete = TRUE);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDelRecsDialog)
	enum { IDD = IDD_DELRECSDIALOG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelRecsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDelRecsDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CEdit		m_editStart;
	CEdit		m_editEnd;
	CNumSpinCtrl m_spinStart;
	CNumSpinCtrl m_spinEnd;
	BOOL		m_performDeletion;
	long m_start;
	long m_end;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELRECSDIALOG_H__5CDA54A1_FFCC_11D3_ABA0_005004BF64C9__INCLUDED_)

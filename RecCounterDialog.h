//{{AFX_INCLUDES()
//#include "nfdrs.h"
//}}AFX_INCLUDES
#if !defined(AFX_RECCOUNTERDIALOG_H__68BEE893_2554_11D2_B8CE_000000000000__INCLUDED_)
#define AFX_RECCOUNTERDIALOG_H__68BEE893_2554_11D2_B8CE_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RecCounterDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRecCounterDialog dialog

class CRecCounterDialog : public CDialog
{
// Construction
public:
	void SetTitle(CString str);
	BOOL cancelled;
	CString m_title;
	void Reset();
	void SetStatusString(CString str);
	void Increment();
	CRecCounterDialog(CWnd* pParent = NULL, CString _title = "", CString _statusStr = "");   // standard constructor
	CWnd * m_Parent;
	int m_nID;
	long	recCount;
	CString	m_statusStr;

// Dialog Data
	//{{AFX_DATA(CRecCounterDialog)
	enum { IDD = IDD_RECCOUNTERDIALOG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecCounterDialog)
	public:
	virtual BOOL Create();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecCounterDialog)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECCOUNTERDIALOG_H__68BEE893_2554_11D2_B8CE_000000000000__INCLUDED_)

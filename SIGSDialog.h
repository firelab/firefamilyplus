//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_SIGSDIALOG_H__F3163762_E419_11D1_B8BA_000000000000__INCLUDED_)
#define AFX_SIGSDIALOG_H__F3163762_E419_11D1_B8BA_000000000000__INCLUDED_
#include "UGCtrl.h"
//#include "CUGsigs.h"
#include "fireplusset.h"
#include "sigset.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SIGSDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSIGSDialog dialog

class CSIGSDialog : public CDialog
{
// Construction
public:
	void RedisplayTable();
	CSIGSet * sigSet;
	CFireplusSet *fpSet;
	CSIGSDialog(CWnd* pParent = NULL, CSIGSet *_sigSet = NULL, CFireplusSet *_fpSet = NULL);   // standard constructor
	CUGCtrl m_grid;

// Dialog Data
	//{{AFX_DATA(CSIGSDialog)
	enum { IDD = IDD_SIGSDIALOG };
	CButton	m_weights;
	CButton	m_edit;
	CButton	m_delete;
	CButton m_export;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSIGSDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSIGSDialog)
	afx_msg void OnSigsnew();
	afx_msg void OnSigsedit();
	afx_msg void OnSigsdelete();
	afx_msg void OnWeights();
	afx_msg void OnExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIGSDIALOG_H__F3163762_E419_11D1_B8BA_000000000000__INCLUDED_)

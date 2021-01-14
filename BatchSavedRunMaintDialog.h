#if !defined(AFX_BatchSavedRunMaintDIALOG_H__E1F21140_09D8_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_BatchSavedRunMaintDIALOG_H__E1F21140_09D8_11D3_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// BatchSavedRunMaintDialog.h : header file
//

#include "afxcmn.h"
#include "batchoptionsset.h"

/////////////////////////////////////////////////////////////////////////////
// CBatchSavedRunMaintDialog dialog

class CBatchOptionsSet;
class CFireplusSet;


class CBatchSavedRunMaintDialog : public CDialog
{
// Construction
public:
	CBatchOptionsSet bopt;
	CFireplusSet * fpSet;
	

	CBatchSavedRunMaintDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBatchSavedRunMaintDialog)
	enum { IDD = IDD_RUN_MAINTENANCE_DIALOG };
		// NOTE: the ClassWizard will add data members here

	CString	maxRuns1;
	CString	maxRuns2;
	//CComboBox	periodicType;
	
void ConfigureControls();

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchSavedRunMaintDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBatchSavedRunMaintDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnNumRecs();
	afx_msg void OnLRU();
	//afx_msg void OnPeriodic();
	
	afx_msg void OnNone();
	afx_msg void OnPurge();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedManualMaintenance();
	//afx_msg void OnBnClickedSchemeDisabled();
	BOOL m_enableRunSaving;
};

class CBatchPurgeDialog : public CDialog
{
public:
	
	
	CBatchPurgeDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL);   // standard constructor
	CFireplusSet *fpSet;

	enum { IDD = IDD_BATCHPURGEDIALOG };


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchPurgeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	
	
	
	virtual BOOL OnInitDialog();
	
	
	virtual void OnOK();
	
	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BatchSavedRunMaintDIALOG_H__E1F21140_09D8_11D3_BD5A_00A0CC24A3EB__INCLUDED_)

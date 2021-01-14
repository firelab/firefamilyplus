//{{AFX_INCLUDES()
#include "FireOptionsSet.h"
#include "afxcmn.h"
//#include "pvnumeric.h"
//}}AFX_INCLUDES
#if !defined(AFX_FIREANALYSISDIALOG_H__1C43B114_7425_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_FIREANALYSISDIALOG_H__1C43B114_7425_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FireAnalysisDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFireAnalysisDialog dialog

class CFireAnalysisDialog : public CDialog
{
// Construction
public:
	CFireOptionsSet optionsSet;
	CFireplusSet * fpSet;
	CFireAnalysisDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL, bool _isNelson = false, bool _isHalfNelson = false);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFireAnalysisDialog)
	enum { IDD = IDD_FIREANALYSISDIALOG };
	CButton	cumulative;
	CButton	lightning;
	CButton moreCauses;
	CComboBox	varIDs;
	BOOL	conditional;
	long causeFlag;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireAnalysisDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFireAnalysisDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnConditional();
	afx_msg void OnVarSort();
	afx_msg void OnCauseFilter();
	afx_msg void OnLightning();
	afx_msg void OnHuman();
	afx_msg void OnAll();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//CSpinButtonCtrl m_spinSample;
	CSpinButtonCtrl m_spinFires;
	CSpinButtonCtrl m_spinAcres;
	bool isNelson;
	bool isHalfNelson;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIREANALYSISDIALOG_H__1C43B114_7425_11D2_B8E5_000000000000__INCLUDED_)
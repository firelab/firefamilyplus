//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_PANALYSISDIALOG_H__2516B824_1A5C_11D2_B8CD_000000000000__INCLUDED_)
#define AFX_PANALYSISDIALOG_H__2516B824_1A5C_11D2_B8CD_000000000000__INCLUDED_
//#include "ugctrl.h"
//#include "UTEdit.h"
#include "PProbDialog.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PAnalysisDialog.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CPAnalysisDialog dialog

class CPAnalysisDialog : public CDialog
{
// Construction
public:
	int varID2;
	int varID1;
	void ConfigureTable1();
	void ConfigureTable2();
	bool TablesOK();
	void SaveTable1();
	void SaveTable2();
	CPAnalysisDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL);   // standard constructor
	CSeasonBinSet binSet;
	CClimateSet climSet;
	CFireplusSet *fpSet;

// Dialog Data
	//{{AFX_DATA(CPAnalysisDialog)
	enum { IDD = IDD_PANALYSISDIALOG };
	CComboBox	index2List;
	CComboBox	index1List;
	//}}AFX_DATA
	PProbCUG m_grid1;
	PProbCUG m_grid2;
	bool hasData1;
	bool hasData2;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPAnalysisDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPAnalysisDialog)
	virtual void OnOK();
	afx_msg void OnSelchangeIndex1();
	afx_msg void OnSelchangeIndex2();
	virtual BOOL OnInitDialog();
	afx_msg void OnVarSort();
	afx_msg void OnVarSort2();
	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANALYSISDIALOG_H__2516B824_1A5C_11D2_B8CD_000000000000__INCLUDED_)

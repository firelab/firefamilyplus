#if !defined(AFX_STATIONEXPORTDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_)
#define AFX_STATIONEXPORTDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationExportDialog.h : header file
//

#include "SIGStationSet.h"
#include "Stationdialog.h"
#include "SIGSDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CStationExportDialog dialog

class CFireplusSet;

class CStationExportDialog : public CDialog
{
// Construction
public:
	
	CStationExportDialog(CStationdialog* pParent = NULL, CFireplusSet *_fpSet = NULL, CString *_SIG = NULL);   // standard constructor
	CStationExportDialog(CSIGSDialog* pParent = NULL, CFireplusSet *_fpSet = NULL, CString *_SIG = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStationExportDialog)
	enum { IDD = IDD_STATION_EXPORT_DIALOG };
	CButton	time1;
	CButton	space;
	CButton comma;
	CListBox	selectedList;
	CListBox	fieldsList;
	CButton	date1;
	//CButton export;
	//}}AFX_DATA
	CStationdialog *stnDlg;

   CFireplusSet *fpSet;
   CString *SIG;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStationExportDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString outFileName;
	DateType dt;
	TimeType tt;
	
	CBitmapButton upButton;
	CBitmapButton downButton;
	CBitmapButton topButton;
	CBitmapButton bottomButton;
	// Generated message map functions
	//{{AFX_MSG(CStationExportDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDate1();
	afx_msg void OnDate2();
	afx_msg void OnDate3();
	afx_msg void OnDate4();
	afx_msg void OnDate5();
	afx_msg void OnTime1();
	afx_msg void OnTime2();
	afx_msg void OnTop();
	afx_msg void OnUp();
	afx_msg void OnBottom();
	afx_msg void OnDown();
	afx_msg void OnDblclkFieldslist();
	afx_msg void OnDblclkSelectedlist();
	afx_msg void OnMoveleft();
	afx_msg void OnMoveright();
	afx_msg void OnSelectall();
	afx_msg void OnRemoveall();

	virtual void OnOK();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_StationEXPORTDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_)

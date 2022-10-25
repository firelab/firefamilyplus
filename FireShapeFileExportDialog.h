#if !defined(AFX_FIRESHAPEFILEEXPORTDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_)
#define AFX_FIRESHAPEFILEEXPORTDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FireShapeFileExportDialog.h : header file
//

#include "CFiresSet.h"

/////////////////////////////////////////////////////////////////////////////
// CFireShapeFileExportDialog dialog

class CFireplusSet;

class CFireShapeFileExportDialog : public CDialog
{
// Construction
public:
	int ExportFires();
	CFireShapeFileExportDialog(CWnd* pParent = NULL, CFiresSet *_records = NULL,CFireplusSet *_fpSet = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFireShapeFileExportDialog)
	enum { IDD = IDD_FIRESHAPEFILEEXPORTDIALOG };
	CButton	time1;
	CButton	space;
	CButton comma;
	CListBox	selectedList;
	CListBox	fieldsList;
	CButton	date1;
	CButton exportShapefile;
	//}}AFX_DATA

   int TranslateCauseToUSFS(int cause, int agency);
   int TranslateCauseToDOI(int cause, int agency);
   CFireplusSet *fpSet;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireShapeFileExportDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString outFileName;
	DateType dt;
	TimeType tt;
	CFiresSet *records;
	CBitmapButton upButton;
	CBitmapButton downButton;
	CBitmapButton topButton;
	CBitmapButton bottomButton;
	// Generated message map functions
	//{{AFX_MSG(CFireShapeFileExportDialog)
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
	afx_msg void OnExportTextFile();
	virtual void OnOK();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIRESHAPEFILEEXPORTDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_)

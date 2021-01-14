#if !defined(AFX_WXEXPORTDIALOG_H__5CDA54A3_FFCC_11D3_ABA0_005004BF64C9__INCLUDED_)
#define AFX_WXEXPORTDIALOG_H__5CDA54A3_FFCC_11D3_ABA0_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WxExportDialog.h : header file
//
#include "wxSet.h"

/////////////////////////////////////////////////////////////////////////////
// CWxExportDialog dialog

class CWxExportDialog : public CDialog
{
// Construction
public:
	CString outFileName;
	DateType dt;
	TimeType tt;

	int windsType;
	int fw13PrecipType;
//	WindType wt;
	CWxSet *records;
	int ExportCustom();
	int ExportFW9();
	int ExportFW13();
	int ExportFW21();
	int ExportFWX();
	int ExportFlamMapFARSITEStream();
	CWxExportDialog(CWnd* pParent = NULL, CWxSet *_records = NULL);   // standard constructor

	CBitmapButton upButton;
	CBitmapButton downButton;
	CBitmapButton topButton;
	CBitmapButton bottomButton;
	void SetAppearance();
// Dialog Data
	//{{AFX_DATA(CWxExportDialog)
	enum { IDD = IDD_WXSAVEDIALOG };
	CButton	fwx;
	CButton flammap;
	CButton	delimStatic;
	CButton	moveLeft;
	CButton	moveRight;
	CButton	removeAll;
	CButton	selectAll;
	CStatic	selectStatic;
	CButton	space;
	CButton	timeStatic;
	CButton	time1;
	CButton	date1;
	CListBox	selectedList;
	CListBox	fieldsList;
	CButton	dateStatic;
	CStatic	custStatic;
	CStatic	availStatic;

	CButton windsAvg;
	CButton windsGusts;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWxExportDialog)
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
	//{{AFX_MSG(CWxExportDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkSelectedlist();
	afx_msg void OnDblclkFieldslist();
	afx_msg void OnMoveleft();
	afx_msg void OnMoveright();
	afx_msg void OnRemoveall();
	afx_msg void OnSelectall();
	afx_msg void OnCustom();
	afx_msg void OnFw9();
	afx_msg void OnFwx();
	afx_msg void OnFw13();
	virtual void OnOK();
	afx_msg void OnDate1();
	afx_msg void OnDate2();
	afx_msg void OnDate3();
	afx_msg void OnDate4();
	afx_msg void OnDate5();
	afx_msg void OnTime1();
	afx_msg void OnTime2();

	// new 2015
	afx_msg void OnWindsAvg();
	afx_msg void OnWindsGusts();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedFlammapFARSITEStream();
	CButton m_btnHourlyPrecip;
	CButton m_btnDailyPrecip;
	afx_msg void OnBnClickedRadioHourlyPrecip();
	afx_msg void OnBnClickedRadioDailyPrecip();
	afx_msg void OnBnClickedRadioFw21();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WXEXPORTDIALOG_H__5CDA54A3_FFCC_11D3_ABA0_005004BF64C9__INCLUDED_)

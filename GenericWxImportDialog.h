#if !defined(AFX_GenericWxImportDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_)
#define AFX_GenericWxImportDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GenericWxImportDialog.h : header file
//

#include "WxSet.h"
#include "afxwin.h"
#include "afxdtctl.h"

/////////////////////////////////////////////////////////////////////////////
// CGenericWxImportDialog dialog

class CFireplusSet;
class CFireplusDoc;
class CDatabase;

#define MAX_INPUT_LINE	4096
#define MAX_FIELD_LEN	250
#define MF_BUFSIZE 32768


class CGenericWxImportDialog : public CDialog
{
// Construction
public:
	CGenericWxImportDialog(CWnd* pParent = NULL, CWxSet *_records = NULL, CDatabase *_pDB = NULL);   // standard constructor
   
// Dialog Data
	//{{AFX_DATA(CGenericWxImportDialog)
	enum { IDD = IDD_GWIDIALOG };
	CButton	time1;
	CButton	time2;
	CButton	space;
	CButton comma;
	CListBox	selectedList;
	CListBox	fieldsList;
	CButton	date1;
	CButton	date2;
	CButton	date3;
	CButton exportWx;
	//}}AFX_DATA

	CFireplusDoc *pDoc;
	CString fileName;
    FILE *errLog;
    long errors;
	CDatabase *pDB;

    //CFireplusView *pDoc;
    //CFireplusSet *fpSet;
    
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenericWxImportDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
    int IsNullValue(char *value);

// Implementation
protected:

	CString inFileName;
	DateType dt;
	TimeType tt;
	CWxSet *records;
	CBitmapButton upButton;
	CBitmapButton downButton;
	CBitmapButton topButton;
	CBitmapButton bottomButton;
	// Generated message map functions
	//{{AFX_MSG(CGenericWxImportDialog)
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
    void OnGenericWx();

	int ImportGenericWx();
    void LogWxDataWarnings(FILE *logFile, CWxSet *wxSet);
	BOOL m_RejectDups;

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboStation;
	CComboBox m_comboObsType;
	CDateTimeCtrl m_dtObsTime;
	afx_msg void OnBnClickedUseStationid();
	afx_msg void OnBnClickedUseObsType();
	afx_msg void OnBnClickedUseObsTime();
	CButton m_checkUseStation;
	CButton m_checkObsType;
	CButton m_checkObsTime;
	afx_msg void OnCbnSelchangeStations();
	CButton m_radioOverwrite;
	int m_radioRejectDups;
	CButton m_checkAllStations;
	afx_msg void OnBnClickedCheckAllstations();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GenericWxImportDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_)

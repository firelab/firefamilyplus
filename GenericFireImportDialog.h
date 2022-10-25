#if !defined(AFX_GenericFireImportDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_)
#define AFX_GenericFireImportDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GenericFireImportDialog.h : header file
//

#include "CFiresSet.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CGenericFireImportDialog dialog

class CFireplusSet;
class CFireplusDoc;
class CDatabase;

#define MAX_INPUT_LINE	4096
#define MAX_FIELD_LEN	250
#define MF_BUFSIZE 32768

long GetFireRegion(CFireRegionSet *regs, long agencyID, char * find);
long GetFireUnit(CFireUnitSet *units, long agencyID, long reg, char * temp);
long AddFireUnit(CFireUnitSet *units, long agencyID, long reg, char * temp);
long GetFireSubunit(CFireSubunitSet *subs, long agencyID, long unit, char * temp);
long AddFireSubUnit(CFireSubunitSet *subs, long unit, char * temp);



class CGenericFireImportDialog : public CDialog
{
// Construction
public:
	CGenericFireImportDialog(CWnd* pParent = NULL, CFiresSet *_records = NULL, CDatabase *_pDB = NULL, long _agencyID = -1);   // standard constructor
   
// Dialog Data
	//{{AFX_DATA(CGenericFireImportDialog)
	enum { IDD = IDD_GFIDIALOG };
	CButton	time1;
	CButton	time2;
	CButton	space;
	CButton comma;
	CListBox	selectedList;
	CListBox	fieldsList;
	CButton	date1;
	CButton	date2;
	CButton	date3;
	
	//}}AFX_DATA

	CFireplusDoc *pDoc;
	CString fileName;
    FILE *errLog;
    long errors;
	CDatabase *pDB;
	long agencyID;
    //CFireplusView *pDoc;
    //CFireplusSet *fpSet;
    
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenericFireImportDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString inFileName;
	DateType dt;
	TimeType tt;
	CFiresSet *records;
	CBitmapButton upButton;
	CBitmapButton downButton;
	CBitmapButton topButton;
	CBitmapButton bottomButton;
	// Generated message map functions
	//{{AFX_MSG(CGenericFireImportDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDate1();
	afx_msg void OnDate2();
	afx_msg void OnDate3();
	//afx_msg void OnDate4();
	//afx_msg void OnDate5();
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
    void OnGenericFire();

	//int ImportGenericFire();
    void LogFireDataWarnings(FILE *logFile, CFiresSet *fireSet);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CButton m_useRegion;
	CButton m_useUnit;
	CButton m_useSubunit;
	CComboBox m_comboRegions;
	CComboBox m_comboUnits;
	CComboBox m_comboSubunits;
	CFireUnitSet unitSet;
	CFireRegionSet regSet;
	CFireSubunitSet subSet;
	void FillSubunits(long unitID);
	void FillUnits(long regionID);
	afx_msg void OnBnClickedUseReg();
	afx_msg void OnCbnSelchangeRegs();
	afx_msg void OnBnClickedUseUnit();
	afx_msg void OnCbnSelchangeUnits();
	afx_msg void OnBnClickedUseSubunit();
	afx_msg void OnCbnSelchangeSubunits();
	BOOL m_RejectDups;
	CButton m_ImportButton;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GenericFireImportDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_)

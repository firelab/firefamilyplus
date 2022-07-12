#if !defined(AFX_IMPORTDIALOG_H__91BB8C13_6D9F_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_IMPORTDIALOG_H__91BB8C13_6D9F_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ImportDialog.h : header file
//
#include "FireFilterDialog.h"
#include "afxwin.h"
#include "updialog2.h"

class CFireplusDoc;

#define MAX_INPUT_LINE	4096

/////////////////////////////////////////////////////////////////////////////
// CImportDialog dialog

class CImportDialog : public CDialog
{
// Construction
public:
	long WriteTempFires(CFireSet *tFires, long agencyID);
	//long WriteTempFires(CFireSet *tFires, CCancelDialog *cd, long agencyID);
	int ImportFWSShort(CString fileName, FILE *errLog);
	int ImportRAW(CString fileName, FILE *errLog, long agencyID);
	int ImportFPL(CString fileName, FILE *errLog, long agencyID);
	int NewImportFWX(CString fileName, FILE *errLog);
	int NewImportFW9(CString fileName, FILE *errLog, bool bOverwrite);
	int ImportStation(FILE *stream, FILE *logFile);
	int ImportStationFile(CString fileName, FILE *logFile);
	void LogWxDataWarnings(FILE *logFile, CWxSet *wxSet);
	long fplLineNo;
	long nNewUnits;
	long nNewSubunits;
	CImportDialog(CWnd* pParent = NULL, CDatabase *_pDB = NULL);   // standard constructor
	//long GetFireSubunit(long agencyID, long unit, char *temp);
	//long GetFireUnit(long agencyID, long reg, char *temp);
	//long GetFireRegion(long agencyID, char *find);
	int WriteFireRecRAW(char *buf, long *fireID, long agencyID, FILE *errLog, CFireSet *tFires);
	void GetFireRecRAW(char *buf, FILE *stream, long agencyID, bool threeLineFormat);
	int WriteFireRecFPL(char *buf, long *fireID, long agencyID, FILE *errLog, CFireSet *tFires);
	int GetFireRecFPL(char *buf, FILE *stream, long agencyID, CFireFilterDialog *ffd);
	int WriteFireRecFWSshort(char *buf, long *fireID, FILE *errLog, CFireSet *tFires);
	int GetFireRecFWSshort(char *buf, FILE *stream, CFireFilterDialog *ffd);
	int ImportCustom(long agencyID);
    // new ffp4:
	int ImportGenericWx(CString fileName, FILE *errLog);
    int ImportWFMIFire(CString fileName, FILE *errLog, CFireFilterDialog *ffd);
	int ImportWFMICodes(CString fileName, FILE *errLog);
	int ImportNewDOICodes(CString fileName, FILE *errLog);
	int ImportWRCCRAWS(CString fileName, FILE *errLog, CString sStationID);
	int ImportNASFFire(CString fileName, FILE *errLog);
	int IsNullValue(char *value);

// Dialog Data
	//{{AFX_DATA(CImportDialog)
	enum { IDD = IDD_IMPORTDIALOG };
	//CButton	fwsShortButton;
	CButton	rawButton;
	//CButton	fplButton;
	//CButton	genericFiresButton;
	CComboBox	agencies;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFireRegionSet regs;
	CFireUnitSet units;
	CFireSubunitSet subs;
	CFireSet fireSet;
	CFireplusDoc *pDoc;
	// Generated message map functions
	//{{AFX_MSG(CImportDialog)
	//afx_msg void OnFpl();
	afx_msg void OnFw9();
	afx_msg void OnFwx();
	afx_msg void OnRaw();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeAgency();
	afx_msg void OnStations();
	afx_msg void OnFwstxt();
	afx_msg void OnGenericWx();
	afx_msg void OnWFMI();
	afx_msg void OnWFMICodes();
	afx_msg void OnGenericFires();
	afx_msg void OnNASFFires();
	afx_msg void OnWRCCRAWS();

	long count;

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	INT_PTR RunFW13Import(vector<string> importFileNames, FILE *errLog, int *nErrors);
	INT_PTR RunFWXImport(vector<string> importFileNames, FILE *errLog, int *nErrors);
public:
	CUPDUPDATA* pCUPDUPData;
	CButton m_genericFireButton;
	CButton m_nasfFireButton;
	CButton m_WFMIbutton;
	afx_msg void OnBnClickedFw21();
};

#define MAX_INPUT_LINE	4096
#define MAX_FIELD_LEN	512

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTDIALOG_H__91BB8C13_6D9F_11D2_B8E5_000000000000__INCLUDED_)
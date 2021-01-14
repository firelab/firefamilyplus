// fireplusDoc.h : interface of the CFireplusDoc class
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_FIREPLUSDOC_H__F8C34260_BF4C_11D1_B8AE_000000000000__INCLUDED_)
#define AFX_FIREPLUSDOC_H__F8C34260_BF4C_11D1_B8AE_000000000000__INCLUDED_

#include "fireplusView.h"	// Added by ClassView
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CFireplusDoc : public CDocument
{
public: // create from serialization only
	CFireplusDoc();
	DECLARE_DYNCREATE(CFireplusDoc)

// Attributes
public:
	CFireplusSet *m_fireplusSet;
    int ffp4upgrade;
	int ffp5upgrade;
	int updateWxObs;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireplusDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	bool HasTempFires();
	void CreateTempFires();
	void CheckPocketCardTable();
	// added for batch2
	void CheckGraphOptionsTable();
	bool NewCheckClimateOptionsTable();
	void CheckClimateOptionsTable();
	void CheckBatchOptionsTable();
	void CheckRunsTable();
	void CheckLFITable();
	void CheckOptsTable();
	void CreateTempWx();
	void CheckFireTable();
	void CheckFireOptionsTable();
	void ChecNFDRS2016Table();
	void CheckWxObsTable();
	//void CheckWxStationTable(CCancelDialog *cd);
	//void CheckffpRunWxStationTable(CCancelDialog *cd);
	//void setDailyObs(CCancelDialog *cd);
	void CheckWxStationTable();
	void CheckffpRunWxStationTable();
	void CheckffpRunClimateOptionsTable();
	void setDailyObs();
	void CheckSeasonBinsTable();
	void CheckPaths();

	bool HasTempWx();
	// new DL_Output table, 2013
	bool HasDLOutput();
	void CreateDLOutput();

	// new HL_Output table, 2015
	bool HasHLOutput();
	void CreateHLOutput();
	//bool HasUserTables();

	int CreateffpCandidatesTable();

	// added for batch
	int CreateffpRunDescriptionTable();
	int CreateffpRunClimateOptionsTable();
	int CreateffpRunPocketCardTable();
	int CreateffpRunFireAssociationsTable();
	int CreateffpRunFireOptionsTable();
	int CreateffpRunWxStationTable();
	int CreateffpRunsTable();
	int CreateffpLFITable();
	int CreateffpFriskTable();
	int CreateffpReportsTable();
	int CreateffpRunOptionsTable();
	int CreateffpReportOptionsTable();
	int CreateffpBatchTable();
	int CreateffpBatchRunsTable();
	int CreateffpBatchOptionsTable();
	int CreateUserValTable();
	int CreateUserVarTable();
	int CreateGenericFireImportsTable();
	int CreateGenericFireFieldsTable();
	int CreateGenericWxImportsTable();
	int CreateGenericWxFieldsTable();
	int CreateRefDOIUnitsTable();
	int CreateffpInitTable();
	int CreateffpOptsTable();
	int CreateffpFxOptsTable();
	int CreateffpFxVarsTable();
	int CreateffpOverlaysTable();
	int CreateffpMergesTable();
	int CreateffpHourlyEventsTable();
	int CreateffpDailyEventsTable();
	int CreateffpTermTable();
	int CreateffpTermDatesTable();
	int CreateffpTermPercentilesTable();
 	int CreateffpSowThresholdsTable();
 	int CreateffpSowOptionsTable();
	int CreateRefDOICauseTable();
//	int CreateTimeZoneTable();
   // end added for batch
	//void PopulateFfpOptsTable();

	BOOL doRunSaving;
	BOOL m_bLocked;
	CFireplusView *m_view;
	CDatabase *m_pDB;
	CString docDir;
	virtual ~CFireplusDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFireplusDoc)
	afx_msg void OnDataUservariablesManage();
	afx_msg void OnDataUservariablesImportdata();
	afx_msg void OnUpdateDataUservariablesImportdata(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int RunDeleteRun(int _runID);
	int RunDeleteBatch(int _batchID);
	// Check for and perform any necessary run maintenance
	int DoRunMaintenance(void);
	virtual void OnCloseDocument();
	afx_msg void OnUpdateBatchEnablerunsaving(CCmdUI *pCmdUI);
	afx_msg void OnBatchEnablerunsaving();
};

class CUpgradeDialog : public CDialog
{
public:

	CUpgradeDialog(CFireplusDoc* pParent = NULL );   // standard constructor

	enum { IDD = IDD_UPGRADEDIALOG };

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRunLoadDialog)
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

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIREPLUSDOC_H__F8C34260_BF4C_11D1_B8AE_000000000000__INCLUDED_)
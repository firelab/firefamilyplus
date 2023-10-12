// fireplusView.h : interface of the CFireplusView class
//
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INCLUDES()
//}}AFX_INCLUDES
//#include "GridCtrl.h"
#if !defined(AFX_FIREPLUSVIEW_H__F8C34262_BF4C_11D1_B8AE_000000000000__INCLUDED_)
#define AFX_FIREPLUSVIEW_H__F8C34262_BF4C_11D1_B8AE_000000000000__INCLUDED_
#include "ugctrl.h"
#include "UTEdit.h"
#include "ExtCheckBox.h"
#include "UGCTdtp.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "ForecastDialog.h"

class CFireplusSet;
class CFireplusDoc;

class FFPViewCUG : public CUGCtrl
{
public:
	FFPViewCUG();
	~FFPViewCUG();
	virtual void OnSetup();
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	virtual COLORREF OnGetDefBackColor(int section);
	virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual int OnEditFinish(int col, long row, CWnd* edit, LPCTSTR string, BOOL cancelFlag);
	virtual int OnEditVerify(int col, long row, CWnd* edit, UINT* vcKey);
	void SetVisibleColumns();

	CUTNumericEdit	m_cutNumeric;
	CExtCheckBox m_checkBoxCT;
	int m_iCheckBoxIndex;
	CUGCTDateTimePicker m_dtPicker;
	int m_iDTPickerIndex;
	//CString m_strFModels, m_strSlopeClass, m_strSlopePos, m_strClimClass, m_strAspect;
	int colWidths[23];
	CDatabase *m_pDB;
};

class CFireplusView : public CRecordView
{
public: // create from serialization only
	CFireplusView();
	DECLARE_DYNCREATE(CFireplusView)
public:
	//{{AFX_DATA(CFireplusView)
	enum { IDD = IDD_FIREPLUS_FORM };
	CButton	dataYearsBtn;
	CEdit	m_StartYear;
	CComboBox	m_StartMonth;
	CEdit	m_StartDay;
	CEdit	m_EndYear;
	CEdit	m_EndDay;
	CComboBox	m_EndMonth;
	CComboBox	m_PeriodLenControl;
	CComboBox	m_ctlSIG_Station;
	CSpinButtonCtrl	m_spnEday;
	CSpinButtonCtrl	m_spnSday;
	CSpinButtonCtrl	m_spnEyear;
	CSpinButtonCtrl	m_spnSyear;
	CFireplusSet* m_pSet;
	CString	m_dbName;
	//CGridCtrl m_Grid;
	FFPViewCUG m_grid;
	//int m_iStartDay;
	//int m_iEndDay;
	//}}AFX_DATA

// Attributes
public:
	CFireplusDoc* GetDocument();
	bool SetYears(CString fmStr = "");
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireplusView)
	public:
	virtual CRecordset* OnGetRecordset();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	bool ignoreMetaTable;
	bool fxParamChange;
	int tableRightMin;
	int tableBottomMin;
	BOOL UpdateOnDeactivate;
	BOOL hasHourly;
	void ConfigureStationList();
	bool FieldsOK();
	void SaveMetaTable();
	void ConfigCheckBoxes();
	//char *GetTempFileName();
	//void GenerateReport(CString variable, BYTE varID);
	virtual ~CFireplusView();
    CForecastDialog *forecastDialog;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	//void SaveNFDRS2016MetaData(int row);
	//void LoadNFDRS2016MetaData(int row);

protected:
	void OnDataWeatherobservations(int obsType);
	void InitSigstation();
// Generated message map functions

protected:
	//{{AFX_MSG(CFireplusView)
	afx_msg void OnClimatology();
	afx_msg void OnFileSave();
	afx_msg void OnDataImport();
	afx_msg void OnSelchangeSigstation();
	afx_msg void OnSigs();
	afx_msg void OnRerap();
	afx_msg void OnWeatherPocketcard();
	afx_msg void OnWeatherSeasonreportsPersistenceprobability();
	afx_msg void OnWeatherSeasonreportsProbabilityanalysis();
	afx_msg void OnWeatherSeasonreportsSeveritylist();
	afx_msg void OnWeatherSeasonreportsSeveritysummary();
	afx_msg void OnFiresFirebusiness();
	afx_msg void OnWeatherSeasonreportsDailylisting();
	afx_msg void OnDataStations();
	afx_msg void OnFiresAssociations();
	afx_msg void OnFiresbutton();
	afx_msg void OnFiresFireanalysis();
	afx_msg void OnFiresSummaryWorkingset();
	afx_msg void OnDataAgencies();

	afx_msg void OnFileSaveAs();
	afx_msg void OnWeatherEventlocator();
	afx_msg void OnWeatherForecasting();
	afx_msg void OnWeatherNfdrscalculator();
	afx_msg void OnWeatherViewobservationsAll();
	afx_msg void OnWeatherViewobservationsDaily();
	afx_msg void OnWeatherDiurnalGraph();
	afx_msg void OnUpdateWeatherDiurnalGraph(CCmdUI* pCmdUI);
	afx_msg void OnWeatherDiurnalHourlylisting();
	afx_msg void OnUpdateWeatherDiurnalHourlylisting(CCmdUI* pCmdUI);
	afx_msg void OnFARSITEExports();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateWeatherHourlydataanalysisFARSITEexports(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWeatherHourlydataanalysisSOWThresholds(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWeatherHourlydataanalysisSOWOptions(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWeatherHourlydataanalysisSOWReCalc(CCmdUI* pCmdUI);
	afx_msg void OnWeatherHourlydataanalysisSOWThresholds();
	afx_msg void OnWeatherHourlydataanalysisSOWOptions();
	afx_msg void OnWeatherHourlydataanalysisSOWReCalc();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnDataUservariablesViewobservations();
	afx_msg void OnRunRestore();
	afx_msg void OnBatchCreateNew();
	afx_msg void OnBatchManage();
	afx_msg void OnBatchSavedRunMaint();
    afx_msg void OnWeatherWindRose();
	afx_msg void OnBatchInteractive();
	afx_msg void OnNelsonDFM();
	//afx_msg void OnNelsonDFM2();
	afx_msg void OnCustomYearlyInits();
	afx_msg void OnHourlyEventLocator();
	afx_msg void OnUpdateHourlyEventLocator(CCmdUI* pCmdUI);
	afx_msg void OnApplyMetadataChanges();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnWeatherFlammapexports();
	//afx_msg void OnOptionsLfisettings();
	afx_msg void OnWeatherFirebusinesscandidates();
	//afx_msg void OnWeatherTerm();
	afx_msg void OnTermGeneral();
	afx_msg void OnTermWorkingset();
	//afx_msg void OnNelsondeadfuelmoisturesFiresanalysis();
	afx_msg void OnUpdateNelsondeadfuelmoisturesFiresanalysis(CCmdUI *pCmdUI);
	afx_msg void OnFiresanalysisAllnelsonmoistures();
	afx_msg void OnFiresanalysis1and10hourmoistures();
	afx_msg void OnUpdateFiresanalysisAllnelsonmoistures(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFiresanalysis1and10hourmoistures(CCmdUI *pCmdUI);
	afx_msg void OnComparewithdailynfdrs1and10hourmoistures();
	afx_msg void OnComparewithdailynfdrsAllnelsonmoistures();
	afx_msg void OnUpdateComparewithdailynfdrs1and10hourmoistures(CCmdUI *pCmdUI);
	afx_msg void OnUpdateComparewithdailynfdrsAllnelsonmoistures(CCmdUI *pCmdUI);
	afx_msg void OnLfioptionsCurrentsig();
	afx_msg void OnLfioptionsDefaults();
	afx_msg void OnEnChangeSday();
	afx_msg void OnEnChangeEday();
	afx_msg void OnEnChangeEyear();
	afx_msg void OnEnChangeSyear();
	afx_msg void OnOptionsCauseCodes();
	afx_msg void OnOptionsCauseCodes2();

		//afx_msg void OnEnKillfocusEyear();
	//afx_msg void OnViewobservationsNfdrs2016Inputs();
	//afx_msg void OnUpdateViewobservationsNfdrs2016Inputs(CCmdUI *pCmdUI);
	CButton m_chkUSeDailyExtremes;
	CButton m_chkForceRecompute;
	CButton m_btnApplyGridChanges;
};

#ifndef _DEBUG  // debug version in fireplusView.cpp
inline CFireplusDoc* CFireplusView::GetDocument()
   { return (CFireplusDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIREPLUSVIEW_H__F8C34262_BF4C_11D1_B8AE_000000000000__INCLUDED_)

//{{AFX_INCLUDES()
//#include "nfdrs.h"
//}}AFX_INCLUDES
#if !defined(AFX_FORECASTDIALOG_H__6FDB9C01_9E72_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_FORECASTDIALOG_H__6FDB9C01_9E72_11D3_BD5A_00A0CC24A3EB__INCLUDED_
#include "ugctrl.h"
#include "UTEdit.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxtempl.h>
// ForecastDialog.h : header file
//
//#include "nfdr32.h"
#include "canada.h"
#include "afxdtctl.h"
//#include "nfdrs4ctrl1.h"
class CFireplusSet;
class CClimAnalysis;

#define USE_COXNUMBER 1001
#define COXMASK_ID			320001

class FxWxCUG : public CUGCtrl
{
public:
	FxWxCUG();
	~FxWxCUG();
	virtual void OnSetup();
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	virtual COLORREF OnGetDefBackColor(int section);
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	CUTNumericEdit	m_cutNumeric;
};

class FxVarCUG : public CUGCtrl
{
public:
	FxVarCUG();
	~FxVarCUG();
	virtual void OnSetup();
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	virtual COLORREF OnGetDefBackColor(int section);
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	CUTNumericEdit	m_cutNumeric;
};

/////////////////////////////////////////////////////////////////////////////
// CForecastDialog dialog
enum AddType {Same, Blank, Historic};
class ForecastOptions
{
public:
	void Write(CDatabase *pDB);
	int Read(CDatabase *pDB);
	ForecastOptions();
	~ForecastOptions();
	void Copy(ForecastOptions *options);
	AddType addType;
	CList<int, int> outputsList;
	double rhPcnt;
	double tempPcnt;
	double windPcnt;
	COLORREF lineColor;
	int lineWidth;
	int lineStyle;
};

class CForecastDialog : public CDialog
{
// Construction
public:
	void Remove();
	CForecastDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL, CClimAnalysis *_analysis = NULL);   // standard constructor
	bool Create();
	CWnd * m_Parent;
	int m_nID;
	//int m_wWid;
	//	NFDRCalc nfdrs;
	Canadian canadian;
	COleDateTime beginDay;
	void ConfigureOutputs();
	void DoReport(); 
//	NFDRCalcState lastState;
	CanadianState canState;
// Dialog Data
	//{{AFX_DATA(CForecastDialog)
	enum { IDD = IDD_FORCASTDIALOG };
	CButton	graphButton;
	//CDateTimeCtrl	startDateCtl;
	//CNfdrs	m_NFDRS;
	//}}AFX_DATA
	FxWxCUG m_wxGrid;
	FxVarCUG m_varGrid;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CForecastDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
	CFireplusSet *fpSet;
	CClimAnalysis *analysis;
	COleDateTime lastDay;
	ForecastOptions options;
protected:

	// Generated message map functions
	//{{AFX_MSG(CForecastDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnRemove();
	afx_msg void OnGraph();
	afx_msg void OnAddday();
	afx_msg void OnOptions();
	afx_msg void OnCalculate();
	virtual void OnCancel();
	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CDateTimeCtrl startDateCtl;
	CNFDRS m_NFDRS;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORECASTDIALOG_H__6FDB9C01_9E72_11D3_BD5A_00A0CC24A3EB__INCLUDED_)

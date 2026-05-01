//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_CLIMATEVIEW_H__87F03BC3_DDD7_11D1_B8B9_000000000000__INCLUDED_)
#define AFX_CLIMATEVIEW_H__87F03BC3_DDD7_11D1_B8B9_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ClimateView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CClimateView DAO record view
#include "UGCtrl.h"
#include "UTEdit.h"
#include "UGCTButn.h"   
#include "ExtCheckBox.h"
#define USE_COXNUMBER 1001
#define COXMASK_ID			320001

class ClimateCUG : public CUGCtrl
{
public:
	ClimateCUG();
	~ClimateCUG();
	virtual void OnSetup();
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg,LONG_PTR param);
	CUTNumericEdit	m_cutNumeric;
	CExtCheckBox m_checkBoxCT;
	CUGButtonType m_buttonCT;
	int m_iCheckBoxIndex;
	int m_iButtonIndex;
};

#include "fireplusSet.h"
//#include "GridCtrl.h"

#include "ClimateSet.h"	// Added by ClassView
class CClimateView : public CRecordView
{
//protected:
//public:
	CClimateView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CClimateView)

// Form Data
public:
	//{{AFX_DATA(CClimateView)
	enum { IDD = IDD_CLIMATE_FORM };
	CListBox	m_SelectedList;
	CFireplusSet* m_pSet;
	//}}AFX_DATA
	ClimateCUG m_grid;

// Attributes
public:

// Operations
public:
	void ConfigureTable();
	void SaveTable();
	CClimateSet *m_climSet;
	CFireplusSet* GetRecordset();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClimateView)
		// NOTE - the ClassWizard will add and remove member functions here.
	public:
	virtual CRecordset* OnGetRecordset();
	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();

	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CClimateView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CClimateView)
	afx_msg void OnRunclimatology();
	afx_msg void OnClearall();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();

	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void AddRemoveString(CString str, bool add);
	afx_msg void OnVarSort();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIMATEVIEW_H__87F03BC3_DDD7_11D1_B8B9_000000000000__INCLUDED_)

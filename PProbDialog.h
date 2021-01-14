//{{AFX_INCLUDES()
#include "ClimateSet.h"	// Added by ClassView
#include "SeasonBinSet.h"	// Added by ClassView
#include "fireplusSet.h"	// Added by ClassView
//}}AFX_INCLUDES
#if !defined(AFX_PPROBDIALOG_H__2516B822_1A5C_11D2_B8CD_000000000000__INCLUDED_)
#define AFX_PPROBDIALOG_H__2516B822_1A5C_11D2_B8CD_000000000000__INCLUDED_
#include "ugctrl.h"
#include "UTEdit.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PProbDialog.h : header file
//
#define USE_COXNUMBER 1001
#define COXMASK_ID			320001

class PProbCUG : public CUGCtrl
{
public:
	PProbCUG();
	~PProbCUG();
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
// CPProbDialog dialog

class CPProbDialog : public CDialog
{
// Construction
public:
	CFireplusSet *fpSet;
	PProbCUG m_grid;
	bool TableOK();
	void SaveTable();
	void ConfigureTable();
	CSeasonBinSet binSet;
	CClimateSet climSet;
	CPProbDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL);   // standard constructor
	bool hasData;
	
// Dialog Data
	//{{AFX_DATA(CPProbDialog)
	enum { IDD = IDD_PPDIALOG };
	CComboBox	indexList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPProbDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPProbDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeIndex();
	virtual void OnOK();
	afx_msg void OnVarSort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PPROBDIALOG_H__2516B822_1A5C_11D2_B8CD_000000000000__INCLUDED_)

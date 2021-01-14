//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_WEIGHTSDIALOG_H__D54ED145_E426_11D1_B8BA_000000000000__INCLUDED_)
#define AFX_WEIGHTSDIALOG_H__D54ED145_E426_11D1_B8BA_000000000000__INCLUDED_
#include "ugctrl.h"
#include "UTEdit.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define USE_COXNUMBER 1001
#define COXMASK_ID			320001


// WeightsDialog.h : header file
//
class WeightsCUG : public CUGCtrl
{
public:
	WeightsCUG();
	~WeightsCUG();
	//virtual void OnSetup();
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	CUTNumericEdit	m_cutNumeric;
};

/////////////////////////////////////////////////////////////////////////////
// CWeightsDialog dialog

class CWeightsDialog : public CDialog
{
// Construction
public:
	CStationInSIGSet * staSet;
	CWeightsDialog(CStationInSIGSet *_staSet, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWeightsDialog)
	enum { IDD = IDD_WEIGHTSDIALOG };
	//}}AFX_DATA
	WeightsCUG m_grid;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWeightsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWeightsDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEIGHTSDIALOG_H__D54ED145_E426_11D1_B8BA_000000000000__INCLUDED_)

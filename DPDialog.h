//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_DPDIALOG_H__3E470A60_8CC7_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_DPDIALOG_H__3E470A60_8CC7_11D3_BD5A_00A0CC24A3EB__INCLUDED_
#include "UGCtrl.h"
#include "UTEdit.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DPDialog.h : header file
//
#define USE_COXNUMBER 1001
#define COXMASK_ID			320001

class DecisionCUG : public CUGCtrl
{
public:
	DecisionCUG();
	~DecisionCUG();
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
// CDPDialog dialog

class CDPDialog : public CDialog
{
	friend class CDPDoc;
	friend class CDPFrame;
// Construction
public:
	CDPFrame * m_Parent;
	CDPDoc * pDoc;
	CDPDialog(CWnd* pParent = NULL, CDocument *_pDoc = NULL);   // standard constructor
	bool intFormat;
// Dialog Data
	//{{AFX_DATA(CDPDialog)
	enum { IDD = IDD_DPDIALOG };
	//}}AFX_DATA
	DecisionCUG m_grid;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDPDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDPDialog)
	virtual void OnOK();
	afx_msg void OnDeletebutton();
	afx_msg void OnNewbutton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DPDIALOG_H__3E470A60_8CC7_11D3_BD5A_00A0CC24A3EB__INCLUDED_)

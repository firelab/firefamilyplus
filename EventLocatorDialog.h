//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_EVENTLOCATORDIALOG_H__995B0AE0_FC8B_11D2_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_EVENTLOCATORDIALOG_H__995B0AE0_FC8B_11D2_BD5A_00A0CC24A3EB__INCLUDED_
#include "ugctrl.h"
#include "UTEdit.h"
#include "UGCTButn.h"  
#include "afxwin.h"
#include "FFPDailyEventsSet.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventLocatorDialog.h : header file
//
#define USE_COXNUMBER 1001
#define COXMASK_ID			320001

class EventLocatorCUG : public CUGCtrl
{
public:
	EventLocatorCUG();
	~EventLocatorCUG();
protected:
	CUTNumericEdit	m_cutNumeric;
	
public:
	//***** Over-ridable Notify Functions *****
	virtual void OnSetup();
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg, LONG_PTR param);
	virtual COLORREF OnGetDefBackColor(int section);
	CUGButtonType m_buttonCT;
	CString varsStr;
	int m_iButtonIndex;
};


/////////////////////////////////////////////////////////////////////////////
// CEventLocatorDialog dialog

class CEventLocatorDialog : public CDialog
{
// Construction
public:
	void BlackenFirstCell();
	CFireplusSet *fpSet;
	CClimateSet climSet;
	CEventLocatorDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL);   // standard constructor
	~CEventLocatorDialog();
// Dialog Data
	//{{AFX_DATA(CEventLocatorDialog)
	enum { IDD = IDD_EVENTLOCATORDIALOG };
	int		days;
	//}}AFX_DATA
	//CStringArray m_rowOpList;
	EventLocatorCUG m_grid;
	CFFPDailyEventsSet records;
	//CStringArray m_varList;
	//CStringArray m_opList;
	//CStringArray m_compList;
	//CStringArray m_valTypeList;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventLocatorDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventLocatorDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//afx_msg void OnInsertedRowDatatabledb1(long Row);
	//afx_msg void OnDeletedRowDatatabledb1(long Row);
	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddrow();
	afx_msg void OnBnClickedRemoverow();
	CButton m_removeButton;
	afx_msg void OnBnClickedButtonLoadDefaults();
	afx_msg void OnBnClickedButtonSetDefault();
	afx_msg void OnVarSort(); 
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTLOCATORDIALOG_H__995B0AE0_FC8B_11D2_BD5A_00A0CC24A3EB__INCLUDED_)

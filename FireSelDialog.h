//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_FIRESELDIALOG_H__15833480_F0B6_11D2_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_FIRESELDIALOG_H__15833480_F0B6_11D2_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FireSelDialog.h : header file
//
#include "FireEditSet.h"
#include "fireselectdatasource.h"
#include "ugctbutn.h"

class FireSelCUG : public CUGCtrl
{
public:
	FireSelCUG();
	~FireSelCUG();
	virtual void OnSetup();
	virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual COLORREF OnGetDefBackColor(int section);
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	CFireSelectDataSource m_data;
protected:
	CUGButtonType		m_button;
	int					m_nButtonIndex;
};

/////////////////////////////////////////////////////////////////////////////
// CFireSelDialog dialog

class CFireSelDialog : public CDialog
{
// Construction
public:
	long selRec;
	CFireSelDialog(CWnd* pParent = NULL, CFireEditSet *_records = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFireSelDialog)
	enum { IDD = IDD_FIRESELDIALOG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireSelDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetRowData(long Row);
	long virRec;
	long totalRecs;
	CFireEditSet * records;
	FireSelCUG m_grid;
	// Generated message map functions
	//{{AFX_MSG(CFireSelDialog)
	//afx_msg void OnLfDblClickDatatabledb1(short Pane, long Row, long Column, long ClickPos, long FAR* Status);
	//afx_msg void OnVMGetFirstDatatabledb1(long FAR* Status);
	//afx_msg void OnVMGetPrevDatatabledb1(long FAR* Status);
	//afx_msg void OnVMGetNextDatatabledb1(long FAR* Status);
	//afx_msg void OnVMGetPercentDatatabledb1(long FAR* Status);
	//afx_msg void OnVMGetRowDataDatatabledb1(long FAR* Status);
	//afx_msg void OnVMGetLastDatatabledb1(long FAR* Status);
	virtual BOOL OnInitDialog();
	afx_msg void OnSortacres();
	afx_msg void OnSortdate();
	afx_msg void OnSortname();
	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIRESELDIALOG_H__15833480_F0B6_11D2_BD5A_00A0CC24A3EB__INCLUDED_)

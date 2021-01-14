//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_USERBROWSEDIALOG_H__04140E95_2711_4850_82F2_8A4A8BDC901F__INCLUDED_)
#define AFX_USERBROWSEDIALOG_H__04140E95_2711_4850_82F2_8A4A8BDC901F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ugctrl.h"
#include "UTEdit.h"
#include "ODBCRecordset.h"

#define USE_COXNUMBER 1001
#define COXMASK_ID			320001


class CUserVarsDataSource :
	public CUGDataSource
{
public:
	CUserVarsDataSource(void);
	~CUserVarsDataSource(void);
	
	CODBCRecordset *records;
	// a few basics needed...
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	GetCell(int col,long row,CUGCell *cell);
	virtual int	SetCell(int col,long row,CUGCell *cell);
	void SortAscending(int col);
	void SortDescending(int col);
	void SetRecords(CODBCRecordset *_records);
	long totalRecs;
	long virRec;
	CString *colNames;
	int nCols;
	CDBVariant m_vars[102];
};

class UserBrowseCUG : public CUGCtrl
{
public:
	UserBrowseCUG();
	~UserBrowseCUG();
	virtual void OnSetup();
	virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual COLORREF OnGetDefBackColor(int section);
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	//menu notifications
	virtual void OnMenuCommand(int col,long row,int section,int item);
	virtual int  OnMenuStart(int col,long row,int section);
	CUserVarsDataSource m_data;
protected:
	CUTNumericEdit	m_cutNumeric;
};



// UserBrowseDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserBrowseDialog dialog

class CUserBrowseDialog : public CDialog
{
// Construction
public:
	CUserBrowseDialog(CWnd* pParent = NULL, CODBCRecordset *_records = NULL);   // standard constructor
	CBitmapButton saveButton;
	CBitmapButton printButton;
	CBitmapButton trashButton;
	int rbCol;
	CPoint rbMouse;
	void SetRowData(long Row);
	long virRec;
	long totalRecs;
	CODBCRecordset * records;
	UserBrowseCUG m_grid;
// Dialog Data
	//{{AFX_DATA(CUserBrowseDialog)
	enum { IDD = IDD_USERBROWSEDIALOG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserBrowseDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserBrowseDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTrash();
	//afx_msg void OnSortSortascending();
	//afx_msg void OnSortSortdescending();
	afx_msg void OnSave();
	afx_msg void OnPrint();
	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERBROWSEDIALOG_H__04140E95_2711_4850_82F2_8A4A8BDC901F__INCLUDED_)

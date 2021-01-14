//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#if !defined(AFX_BATCHEDITDIALOG_995B0AE0_FC8B_11D2_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_BATCHEDITDIALOG_H__995B0AE0_FC8B_11D2_BD5A_00A0CC24A3EB__INCLUDED_

#include "UGCtrl.h"
#include "UTEdit.h"
#include "ExtCheckBox.h"
#include "afxwin.h"
#include "runsset.h"
#include "fireplusset.h"
#include "batchset.h"
#include "RunSaveViewSet.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BatchEditDialog.h : header file
//
#define USE_COXNUMBER 1001
#define COXMASK_ID			320001

extern int curr_runID;
extern int curr_batchID;


class CBatchEditDataSource :
	public CUGDataSource
{
public:
	CBatchEditDataSource(void);
	~CBatchEditDataSource(void);

	CRunSaveViewSet *records;
	CRunsSet *records2;

	// a few basics needed...
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	GetCell(int col,long row,CUGCell *cell);
	virtual int	SetCell(int col,long row,CUGCell *cell);
	void SortAscending(int col);
	void SortDescending(int col);
	void SetRecords(CRunSaveViewSet *_records,CRunsSet *_records2);
	long totalRecs;
	long virRec;
  

};


class BatchEditCUG : public CUGCtrl
{
public:
	BatchEditCUG();
	~BatchEditCUG();
protected:
	CUTNumericEdit	m_cutNumeric;
public:
	//***** Over-ridable Notify Functions *****
	virtual void OnSetup();
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	//virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);	
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);

	//menu notifications
	virtual void OnMenuCommand(int col,long row,int section,int item);
	virtual int  OnMenuStart(int col,long row,int section);
	//virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual COLORREF OnGetDefBackColor(int section);
	CBatchEditDataSource m_data;
	CExtCheckBox m_checkBoxCT;
	int m_iCheckBoxIndex;
    CRect rect;
};


/////////////////////////////////////////////////////////////////////////////
// CBatchEditDialog dialog

class CBatchEditDialog : public CDialog
{
public:
	int rbCol;
	CPoint rbMouse;

	long virRec;
	long totalRecs;
	CRunSaveViewSet *records;
	CRunsSet * records2;
	CBatchEditDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL,CRunSaveViewSet *_records = NULL, CRunsSet *_records2 = NULL, int _editID = -1);   // standard constructor
	
	CString Description;
	int editID;

	CFireplusSet *fpSet;
	
	BatchEditCUG m_grid;

	CButton *reverseButton,
		    *selectAllButton,
			*deselectAllButton,
			*createButton,
			*cancelButton;

	enum { IDD = IDD_BATCH_EDIT_DIALOG };
	
    CString filterSta;
	CString filterDescription;
    int sortOrder;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchEditDialog)
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
    afx_msg void OnSize(UINT nType, int cx, int cy);
// Implementation
	protected:

	
	
	
	virtual BOOL OnInitDialog();
	
	//afx_msg void OnChangeDatatblctrl1(long Row, long Column);
	//afx_msg void OnRtClickDatatblctrl1(short Pane, long Row, long Column, long ClickPos, long FAR* Status);
	//afx_msg void OnMouseDownDatatblctrl1(short Button, short Shift, long X, long Y);
	//afx_msg void OnSortSortascending();
	//afx_msg void OnSortSortdescending();
	//afx_msg void OnPrint();
	//afx_msg void OnSave();
	//afx_msg void OnTrash();
	//afx_msg void OnBeforeDeleteRowDatatblctrl1(long Row, long FAR* Status);
	afx_msg void OnBatchSelectAll();
	afx_msg void OnBatchDeselectAll();
	virtual void OnOK();
	afx_msg void OnFilter();
	afx_msg void OnReverseSort();

	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};



class CBatchEditConfirmDialog : public CDialog
{
public:
	
	CRunsSet * records;
	CBatchEditConfirmDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL,CString _Description="", int _editID = -1 );   // standard constructor
	
	CFireplusSet *fpSet;
	CString Description;
	int editID;

	enum { IDD = IDD_BATCHEDITCONFIRMDIALOG };


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRunLoadDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	
	
	
	virtual BOOL OnInitDialog();
	
	//afx_msg void OnChangeDatatblctrl1(long Row, long Column);
	//afx_msg void OnRtClickDatatblctrl1(short Pane, long Row, long Column, long ClickPos, long FAR* Status);
	//afx_msg void OnMouseDownDatatblctrl1(short Button, short Shift, long X, long Y);
	//afx_msg void OnSortSortascending();
	//afx_msg void OnSortSortdescending();
	//afx_msg void OnPrint();
	//afx_msg void OnSave();
	//afx_msg void OnTrash();
	//afx_msg void OnBeforeDeleteRowDatatblctrl1(long Row, long FAR* Status);
	virtual void OnOK();
	
	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BatchEditDIALOG_H__995B0AE0_FC8B_11D2_BD5A_00A0CC24A3EB__INCLUDED_)

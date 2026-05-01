//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_CUSTOMINITSDIALOG_995B0AE0_FC8B_11D2_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_CUSTOMINITSDIALOG_H__995B0AE0_FC8B_11D2_BD5A_00A0CC24A3EB__INCLUDED_
#include "fireplusset.h"
#include "UGCtrl.h"
#include "UTEdit.h"
#include "ExtCheckBox.h"
#include "UGCTdtp.h"
//#include "afxwin.h"
#include "custominitset.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomInitsDialog.h : header file
//
#define USE_COXNUMBER 1001
#define COXMASK_ID			320001



class CCustomInitsDataSource :
	public CUGDataSource
{
public:
	CCustomInitsDataSource(void);
	~CCustomInitsDataSource(void);
	
    CCustomInitSet *records;

	// a few basics needed...
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	GetCell(int col,long row,CUGCell *cell);
	virtual int	SetCell(int col,long row,CUGCell *cell);
	
	void SetRecords(CCustomInitSet *_records);
	long totalRecs;
	long virRec;
  

};


class CustomInitsCUG : public CUGCtrl
{
public:
	CustomInitsCUG();
	~CustomInitsCUG();
protected:
	CUTNumericEdit	m_cutNumeric;
public:
	//***** Over-ridable Notify Functions *****
	virtual void OnSetup();
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg, LONG_PTR param);
	//virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	int OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey);
	int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);

	//menu notifications
	virtual void OnMenuCommand(int col,long row,int section,int item);
	virtual int  OnMenuStart(int col,long row,int section);
	//virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual COLORREF OnGetDefBackColor(int section);
	virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	CCustomInitsDataSource m_data;
	CUGCTDateTimePicker m_dtPicker;
	int m_iDTPickerIndex;
	
};


/////////////////////////////////////////////////////////////////////////////
// CCustomInitsDialog dialog

class CCustomInitsDialog : public CDialog
{
public:
	int rbCol;
	CPoint rbMouse;

	long virRec;
	long totalRecs;
	
	CCustomInitSet * records;

	CCustomInitsDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL, CCustomInitSet *_records = NULL);   // standard constructor
	
	
	CFireplusSet *fpSet;
	
	CustomInitsCUG m_grid;
	CButton checkEnabled;
    CString stationTitle;

	enum { IDD = IDD_CUSTOMINITSDIALOG };
	


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomInitsDialog)
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	protected:

	
	
	
	virtual BOOL OnInitDialog();
	
	virtual void OnOK();
	
    void OnCustomInitEdit();
	void OnCustomInitNew();
	void OnCustomInitDelete();
	void OnEnableButton();
	
	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


};

class CCustomInitDeleteConfirmDialog : public CDialog
{
public:
	
	CCustomInitSet * records;
	CCustomInitDeleteConfirmDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL, int _editYear = -1,CCustomInitSet *_records = NULL );   // standard constructor
	
	CFireplusSet *fpSet;

	int editYear;

	enum { IDD = IDD_CUSTOMINITDELETECONFIRMDIALOG };


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRunLoadDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	
	
	
	virtual BOOL OnInitDialog();
	
	virtual void OnOK();
	
	//DECLARE_EVENTSINK_MAP()

	
	DECLARE_MESSAGE_MAP()

};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CustomInitsDIALOG_H__995B0AE0_FC8B_11D2_BD5A_00A0CC24A3EB__INCLUDED_)

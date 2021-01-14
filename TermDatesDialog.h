#pragma once
#include "afxdtctl.h"
#include "TermsSet.h"
#include "ugctrl.h"
#include "TermDatesSet.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "fireplusSet.h"
#include "UGCTdtp.h"

class TermDatesCUG : public CUGCtrl
{
public:
	TermDatesCUG();
	~TermDatesCUG();
	virtual void OnSetup();
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	virtual COLORREF OnGetDefBackColor(int section);
	virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	CUGCTDateTimePicker m_dtPicker;
	int m_iDTPickerIndex;
};

class TermDatesData
{
public:
	TermDatesData();
	~TermDatesData();
	//int termID;
	int year;
	COleDateTime date;
	CString comment;
};
/*class CTermDatesDataSource :
	public CUGDataSource
{
public:
	CTermDatesDataSource(void);
	~CTermDatesDataSource(void);
	
	CTermDatesSet *records;
	// a few basics needed...
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	GetCell(int col,long row,CUGCell *cell);
	void SetRecords(CTermDatesSet *_records);
	long totalRecs;
	long virRec;

};

class TermDatesCUG : public CUGCtrl
{
public:
	TermDatesCUG();
	~TermDatesCUG();
	virtual void OnSetup();
	//virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual COLORREF OnGetDefBackColor(int section);
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	//virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	//menu notifications
	//virtual void OnMenuCommand(int col,long row,int section,int item);
	//virtual int  OnMenuStart(int col,long row,int section);
	CTermDatesDataSource m_data;
protected:
};
*/

// CTermDatesDialog dialog

class CTermDatesDialog : public CDialog
{
	DECLARE_DYNAMIC(CTermDatesDialog)

public:
	CTermDatesDialog(CWnd* pParent = NULL, int termID = 0, CDatabase *_pDB = NULL,CFireplusSet *_fpSet = NULL);   // standard constructor
	virtual ~CTermDatesDialog();
// Overrides

// Dialog Data
	enum { IDD = IDD_TERM_DATES_DIALOG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	CTermDatesSet *m_termDatesSet;
	TermDatesCUG m_grid;
	int m_iDTPickerIndex;
	CUGCTDateTimePicker m_dtPicker;
	DECLARE_MESSAGE_MAP()
public:
	CTermsSet *m_termsSet;
	int m_termID;
	CString m_Name;
	CDateTimeCtrl m_ctlStartDay;
	CString m_Comment;
	afx_msg void OnDtnDatetimechangeDatetimepickerStartday(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnOK();
	void RedisplayTable();
	CFireplusSet *m_fpSet;

public:
	int m_StartYear;
	CSpinButtonCtrl m_spinStartYear;
	int m_EndYear;
	CSpinButtonCtrl m_spinEndYear;
	afx_msg void OnBnClickedButtonApply();
	//afx_msg void OnBnClickedButtonAppend();
	//afx_msg void OnBnClickedButtonInsert();
	//afx_msg void OnBnClickedButtonDelete();
	//CButton m_btnDelete;
	//CButton m_btnInsert;
	afx_msg void OnBnClickedButtonSave();
	CButton m_btnSave;
	afx_msg void OnBnClickedButtonProbabilities();
	bool Create();
	int m_nID;
	CWnd * m_Parent;
protected:
	virtual void PostNcDestroy();
	virtual void OnCancel();
};

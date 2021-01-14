#pragma once
#include "afxwin.h"
#include "StatCauseSet.h"
#include "fireplusSet.h"
#include "ugctrl.h"
#include "UTEdit.h"


#define USE_COXNUMBER 1001
#define COXMASK_ID			320001

class CStatCauseDataSource :
	public CUGDataSource
{
public:
	CStatCauseDataSource(void);
	~CStatCauseDataSource(void);
	
	CStatCauseSet *records;
	// a few basics needed...
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	SetCell(int col,long row,CUGCell *cell);
	virtual int	GetCell(int col,long row,CUGCell *cell);
	void SetRecords(CStatCauseSet *_records);
	long totalRecs;
	long virRec;

};

class StatCauseCUG : public CUGCtrl
{
public:
	StatCauseCUG();
	~StatCauseCUG();
	virtual void OnSetup();
	//virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual COLORREF OnGetDefBackColor(int section);
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	//virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual int OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	//menu notifications
	//virtual void OnMenuCommand(int col,long row,int section,int item);
	//virtual int  OnMenuStart(int col,long row,int section);
	CStatCauseDataSource m_data;
protected:
	
	CUTNumericEdit	m_cutNumeric;
};

// CStatCauseDialog dialog

class CStatCauseDialog : public CDialog
{
	DECLARE_DYNAMIC(CStatCauseDialog)

public:
	CStatCauseDialog(CWnd* pParent = NULL, CStatCauseSet *pRecords = NULL, CFireplusSet *_fpSet = NULL);   // standard constructor
	//CStatCauseDialog(CWnd* pParent = NULL, CStatCauseSet *pRecords = NULL, CFireplusSet *fpSet = NULL, bool _IsGeneral = FALSE);   // standard constructor
	virtual ~CStatCauseDialog();
	long virRec;
	long totalRecs;
	StatCauseCUG m_grid;
	CStatCauseSet *records;
	CFireplusSet *m_fpSet;

// Dialog Data
	enum { IDD = IDD_FIRESTATCAUSE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//CFireplusSet *m_fpSet;
	CDatabase *pDB;
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonDelete();
	//CButton m_EditButton;
	CButton m_DeleteButton;
	virtual BOOL OnInitDialog();	
	//afx_msg void OnBnClickedButtonCopy();

};

#pragma once
#include "afxwin.h"
#include "DOICauseSet.h"
#include "fireplusSet.h"
#include "ugctrl.h"
#include "UTEdit.h"


#define USE_COXNUMBER 1001
#define COXMASK_ID			320001

class CDOICauseDataSource :
	public CUGDataSource
{
public:
	CDOICauseDataSource(void);
	~CDOICauseDataSource(void);
	
	CDOICauseSet *records;
	// a few basics needed...
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	SetCell(int col,long row,CUGCell *cell);
	virtual int	GetCell(int col,long row,CUGCell *cell);
	void SetRecords(CDOICauseSet *_records);
	long totalRecs;
	long virRec;

};

class DOICauseCUG : public CUGCtrl
{
public:
	DOICauseCUG();
	~DOICauseCUG();
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
	CDOICauseDataSource m_data;
protected:
	
	CUTNumericEdit	m_cutNumeric;
};

// CDOICauseDialog dialog

class CDOICauseDialog : public CDialog
{
	DECLARE_DYNAMIC(CDOICauseDialog)

public:
	CDOICauseDialog(CWnd* pParent = NULL, CDOICauseSet *pRecords = NULL, CFireplusSet *_fpSet = NULL);   // standard constructor
	//CDOICauseDialog(CWnd* pParent = NULL, CDOICauseSet *pRecords = NULL, CFireplusSet *fpSet = NULL, bool _IsGeneral = FALSE);   // standard constructor
	virtual ~CDOICauseDialog();
	long virRec;
	long totalRecs;
	DOICauseCUG m_grid;
	CDOICauseSet *records;
	CFireplusSet *m_fpSet;

// Dialog Data
	enum { IDD = IDD_FIREDOICAUSE_DIALOG };

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

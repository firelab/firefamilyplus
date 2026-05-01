#pragma once
#include "afxwin.h"
#include "UGCtrl.h"
#include "UTEdit.h"
#include "ExtCheckBox.h"
#include "RunsSet.h"
#include "RunSaveViewSet.h"

class DelRec
{
public:
	DelRec();
	long runID;
	bool del;
	int numBatches;
	CString batchesStr;
};

class CRunMaintenanceDataSource :
	public CUGDataSource
{
public:
	CRunMaintenanceDataSource(void);
	~CRunMaintenanceDataSource(void);

	//CRunSaveViewSet *records;
	CRunSaveViewSet *records;

	// a few basics needed...
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	GetCell(int col,long row,CUGCell *cell);
	virtual int	SetCell(int col,long row,CUGCell *cell);
	void SortAscending(int col);
	void SortDescending(int col);
	void SetRecords(CRunSaveViewSet *_records);//,CRunsSet *_records2);
	long totalRecs;
	long virRec;
	DelRec *delRecs;
	void Sort(int sortID);

};

class RunMaintenanceCUG : public CUGCtrl
{
public:
	RunMaintenanceCUG();
	~RunMaintenanceCUG();
protected:
	//CUTNumericEdit	m_cutNumeric;
public:
	//***** Over-ridable Notify Functions *****
	virtual void OnSetup();
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	//virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg, LONG_PTR param);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);

	//menu notifications
	//virtual void OnMenuCommand(int col,long row,int section,int item);
	//virtual int  OnMenuStart(int col,long row,int section);
	//virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual COLORREF OnGetDefBackColor(int section);
	CRunMaintenanceDataSource m_data;
	CExtCheckBox m_checkBoxCT;
	int m_iCheckBoxIndex;
    CRect rect;
	void SelectAll(bool trueFalse);
};

// CRunMaintenanceManual dialog

class CRunMaintenanceManual : public CDialog
{
	DECLARE_DYNAMIC(CRunMaintenanceManual)

public:
	CRunMaintenanceManual(CWnd* pParent, CDatabase *_pDB);   // standard constructor
	virtual ~CRunMaintenanceManual();

// Dialog Data
	enum { IDD = IDD_RUN_MAINTENANCE_MANUAL_DLG };
	RunMaintenanceCUG m_grid;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CDatabase *m_pDB;
	CRunSaveViewSet *m_pRunsSet;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedBatchSelectall();
	afx_msg void OnBnClickedBatchDeselectall();
	afx_msg void OnCbnSelchangeSortCombo();
	CComboBox m_sortCombo;
};

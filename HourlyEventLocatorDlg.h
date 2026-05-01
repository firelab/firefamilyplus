#pragma once
//#include "EventLocatorDialog.h"
#include "ugctrl.h"
#include "UTEdit.h"
#include "UGCTButn.h"
#include "FFPHourlyEventsSet.h"

/*class CHourlyEventSource :
	public CUGDataSource
{
public:
	CHourlyEventSource(void);
	~CHourlyEventSource(void);
	
	CFFPHourlyEventsSet *records;
	// a few basics needed...
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	GetCell(int col,long row,CUGCell *cell);
	void SetRecords(CFFPHourlyEventsSet *_records);
	long totalRecs;
	long virRec;

};*/

class HourlyEventLocatorCUG : public CUGCtrl
{
public:
	HourlyEventLocatorCUG();
	~HourlyEventLocatorCUG();
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
	//CHourlyEventSource m_data;
};


// CHourlyEventLocatorDlg dialog

class CHourlyEventLocatorDlg : public CDialog
{
	DECLARE_DYNCREATE(CHourlyEventLocatorDlg)

public:
	void BlackenFirstCell();
	CFireplusSet *fpSet;
	CClimateSet climSet;
	CFFPHourlyEventsSet records;

	CHourlyEventLocatorDlg(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL);   // standard constructor
	virtual ~CHourlyEventLocatorDlg();
// Overrides

// Dialog Data
	enum { IDD = IDD_HOURLYEVENTLOCATORDIALOG};
	int		hours;
	HourlyEventLocatorCUG m_grid;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddrow();
	afx_msg void OnBnClickedRemoverow();
	CButton m_removeButton;
	afx_msg void OnBnClickedOk();
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnBnClickedButtonLoadDefaults();
	afx_msg void OnBnClickedButtonSetDefault();
	afx_msg void OnVarSort(); 
};

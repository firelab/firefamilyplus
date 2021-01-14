#pragma once
#include "NFDRS2016WxDoc.h"
#include "ugctrl.h"
#include "UTEdit.h"
#define USE_COXNUMBER 1001
#define COXMASK_ID			320001

class CNFDRS2016WxDataSource :
	public CUGDataSource
{
public:
	CNFDRS2016WxDataSource(void);
	~CNFDRS2016WxDataSource(void);

	CNFDRS2016WxSet *records;
	// a few basics needed...
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	GetCell(int col, long row, CUGCell *cell);
	virtual int	SetCell(int col, long row, CUGCell *cell);
	void SortAscending(int col);
	void SortDescending(int col);
	void SetRecords(CNFDRS2016WxSet *_records);
	long totalRecs;
	long virRec;
};

class NFDRS2016WxCUG : public CUGCtrl
{
public:
	NFDRS2016WxCUG();
	~NFDRS2016WxCUG();
	virtual void OnSetup();
	virtual void OnDrawFocusRect(CDC *dc, RECT *rect);
	virtual COLORREF OnGetDefBackColor(int section);
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	virtual int OnEditStart(int col, long row, CWnd **edit);
	virtual int OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey);
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnCharDown(UINT *vcKey, BOOL processed);
	virtual void OnKeyDown(UINT *vcKey, BOOL processed);
	//menu notifications
	virtual void OnMenuCommand(int col, long row, int section, int item);
	virtual int  OnMenuStart(int col, long row, int section);
	CNFDRS2016WxDataSource m_data;
protected:
	CUTNumericEdit	m_cutNumeric;
};


// CNFDRS2016WxView form view

class CNFDRS2016WxView : public CFormView
{
	DECLARE_DYNCREATE(CNFDRS2016WxView)

protected:
	CNFDRS2016WxView();           // protected constructor used by dynamic creation
	virtual ~CNFDRS2016WxView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NFDRS2016WXVIEW };
#endif
	void SetRecords(CNFDRS2016WxSet *_records);
	CNFDRS2016WxSet *records;
	long virRec;
	long totalRecs;
	NFDRS2016WxCUG m_grid;
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};



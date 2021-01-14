#pragma once
#include "WeatherDoc.h"
#include "ugctrl.h"
#include "UTEdit.h"

#define USE_COXNUMBER 1001
#define COXMASK_ID			320001

class CWxDataSource :
	public CUGDataSource
{
public:
	CWxDataSource(void);
	~CWxDataSource(void);

	CWxSet *records;
	// a few basics needed...
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	GetCell(int col,long row,CUGCell *cell);
	virtual int	SetCell(int col,long row,CUGCell *cell);
	void SortAscending(int col);
	void SortDescending(int col);
	void SetRecords(CWxSet *_records);
	long totalRecs;
	long virRec;
};

class WxCUG : public CUGCtrl
{
public:
	WxCUG();
	~WxCUG();
	virtual void OnSetup();
	virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual COLORREF OnGetDefBackColor(int section);
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual int OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	//menu notifications
	virtual void OnMenuCommand(int col,long row,int section,int item);
	virtual int  OnMenuStart(int col,long row,int section);
	CWxDataSource m_data;
protected:
	CUTNumericEdit	m_cutNumeric;
};

// CWeatherView form view

class CWeatherView : public CFormView
{
	DECLARE_DYNCREATE(CWeatherView)

protected:
	CWeatherView();           // protected constructor used by dynamic creation
	virtual ~CWeatherView();

public:
	enum { IDD = IDD_WEATHERVIEW };
	CWxSet *records;
	int rbCol;
	CPoint rbMouse;
	long virRec;
	long totalRecs;
	WxCUG m_grid;
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
	void SetRecords(CWxSet *_records);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	afx_msg void OnDataExport();
	afx_msg void OnDataPrint();
	afx_msg void OnDataDeleterecords();
	afx_msg void OnUpdateDataDeleterecords(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDataExport(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDataPrint(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
};
#pragma once
#include "ugctrl.h"
#include "UTEdit.h"
#include "CandidateSet.h"

class CCandidatesDataSource :
	public CUGDataSource
{
public:
	CCandidatesDataSource(void);
	~CCandidatesDataSource(void);
	
	CCandidateSet *records;
	//CClimateSet *pClimSet;
	// a few basics needed...
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	GetCell(int col,long row,CUGCell *cell);
	virtual int	SetCell(int col,long row,CUGCell *cell);
	void SortAscending(int col);
	void SortDescending(int col);
	void SetRecords(CCandidateSet *_records);
	long totalRecs;
	long virRec;

};

class CandidatesCUG : public CUGCtrl
{
public:
	CandidatesCUG();
	~CandidatesCUG();
	virtual void OnSetup();
	//virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual COLORREF OnGetDefBackColor(int section);
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	//virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	//menu notifications
	virtual void OnMenuCommand(int col,long row,int section,int item);
	virtual int  OnMenuStart(int col,long row,int section);
	CCandidatesDataSource m_data;
protected:
};

// CCandidatesView view

class CCandidatesView : public CFormView
{
	DECLARE_DYNCREATE(CCandidatesView)

protected:
	CCandidatesView();           // protected constructor used by dynamic creation
	virtual ~CCandidatesView();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	enum { IDD = IDD_CANDIDATESVIEW };
	//virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	CCandidateSet *records;
	int rbCol;
	CPoint rbMouse;
	long virRec;
	long totalRecs;
	CandidatesCUG m_grid;
	void SetRecords(CCandidateSet *_records);
	void ResetRecords(CCandidateSet *_records);
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnViewFireday();
	afx_msg void OnViewLargefireday();
	afx_msg void OnViewMultiplefireday();
	afx_msg void OnDataExport();
	afx_msg void OnDataPrint();
	afx_msg void OnDataDeleterecords();
	afx_msg void OnUpdateViewFireday(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewLargefireday(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewMultiplefireday(CCmdUI *pCmdUI);
};



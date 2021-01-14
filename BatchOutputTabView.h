#pragma once
#include "afxcmn.h"



// CBatchOutputTabView form view

class CBatchOutputTabView : public CFormView
{
	DECLARE_DYNCREATE(CBatchOutputTabView)

protected:
	CBatchOutputTabView();           // protected constructor used by dynamic creation
	virtual ~CBatchOutputTabView();

public:
	enum { IDD = IDD_BATCHTABVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tabCtrl;
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};



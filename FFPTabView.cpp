// FFPTabView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "FFPTabView.h"
#include ".\ffptabview.h"

// CFFPTabView

IMPLEMENT_DYNCREATE(CFFPTabView, CTabView)

CFFPTabView::CFFPTabView()
{
}

CFFPTabView::~CFFPTabView()
{
}

BEGIN_MESSAGE_MAP(CFFPTabView, CTabView)
END_MESSAGE_MAP()


// CFFPTabView diagnostics

#ifdef _DEBUG
void CFFPTabView::AssertValid() const
{
	CTabView::AssertValid();
}

void CFFPTabView::Dump(CDumpContext& dc) const
{
	CTabView::Dump(dc);
}
#endif //_DEBUG


// CFFPTabView message handlers

void CFFPTabView::OnInitialUpdate()
{
	pDoc = (CQuickBatch *)GetDocument();
	//pDoc->m_pTabView = this;
	CTabView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
}

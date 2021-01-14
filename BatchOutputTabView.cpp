// BatchOutputTabView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "BatchOutputTabView.h"
#include ".\batchoutputtabview.h"
#include "BatchOutputDoc.h"
// CBatchOutputTabView

IMPLEMENT_DYNCREATE(CBatchOutputTabView, CFormView)

CBatchOutputTabView::CBatchOutputTabView()
	: CFormView(CBatchOutputTabView::IDD)
{
}

CBatchOutputTabView::~CBatchOutputTabView()
{
}

void CBatchOutputTabView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tabCtrl);
}

BEGIN_MESSAGE_MAP(CBatchOutputTabView, CFormView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CBatchOutputTabView diagnostics

#ifdef _DEBUG
void CBatchOutputTabView::AssertValid() const
{
	CFormView::AssertValid();
}

void CBatchOutputTabView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CBatchOutputTabView message handlers

void CBatchOutputTabView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_tabCtrl.InsertItem(0, "Stats Table");
	m_tabCtrl.InsertItem(1, "Stats Graph");
	m_tabCtrl.InsertItem(2, "Stats Listing");
	m_tabCtrl.InsertItem(3, "Percentiles Graph");
	m_tabCtrl.InsertItem(4, "Daily Frequencies");
	m_tabCtrl.InsertItem(5, "Data Count");
	m_tabCtrl.InsertItem(6, "Pocket Card");
	m_tabCtrl.InsertItem(7, "Daily Listing");
	m_tabCtrl.InsertItem(8, "Fires Summary");
	m_tabCtrl.InsertItem(9, "Fires Percentiles");
	m_tabCtrl.InsertItem(10, "Fires Percentile Graph");
	m_tabCtrl.InsertItem(11, "Cumulative Fires");
	m_tabCtrl.InsertItem(12, "Fires Analysis Report");
	m_tabCtrl.InsertItem(13, "Fires Analysis Graph");
	//SendMessage( WM_SIZE );
	CRect rect;
	GetClientRect(&rect);
	m_tabCtrl.MoveWindow(&rect);

}

void CBatchOutputTabView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(m_tabCtrl.m_hWnd)
	{
		//CRect vRect;
		//GetClientRect(&vRect);
		//SetRectangle();
		m_tabCtrl.MoveWindow(0, 0, cx, cy);//cx - 1, cy - 1);
	}
}

//BOOL CBatchOutputTabView::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
//{
//	switch(message)
//	{
//	case WM_NOTIFY:
//		{
//			LPNMHDR lpnmhdr = (LPNMHDR) lParam; 
//			switch(lpnmhdr->code)
//			{
//			case TCN_SELCHANGE:
//				AfxMessageBox("Changing views");
//				break;
//			}
//		}
//	}
//
//	return CFormView::OnChildNotify(message, wParam, lParam, pLResult);
//}

BOOL CBatchOutputTabView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LPNMHDR lpnmhdr = (LPNMHDR) lParam; 
	switch(lpnmhdr->code)
	{
		case TCN_SELCHANGE:
		{
			CQuickBatch *pDoc = (CQuickBatch *)GetDocument();
			int tab = m_tabCtrl.GetCurSel();
			pDoc->SwitchTabView(tab);
			break;
		}
	}
	return CFormView::OnNotify(wParam, lParam, pResult);
}

// BatchPropSheet.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
//#include "BatchOptionsSet.h"
#include "wxset.h"
#include "OverlayDialog.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "SplitterFrame.h"
#include "GraphSet.h"

#include "LineGraphProp.h"
#include "BargraphProp.h"
#include "GeneralGraphProp.h"
#include "GraphFiresPage.h"
#include "FDProjPage.h"

#include "BatchAutoSaveProp.h"
#include "BatchGeneralProp.h"

#include "BatchPropSheet.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBatchPropSheet

IMPLEMENT_DYNAMIC(CBatchPropSheet, CPropertySheet)

CBatchPropSheet::CBatchPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage,
		CDatabase *_pDB, CMainFrame *_pFrame)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), basp(), bgen(),okButton(),cancelButton(), graphSet(_pDB),boptSet(_pDB)
	//,batchSet(_pDB)
{
	mainFrame = _pFrame;
	//if(_pDB){
	//	batchSet.Open();
	//}
	if(_pDB){
		graphSet.Open();
		boptSet.Open();
	}
	basp.SetGraphSet(&graphSet);
	bgen.SetBatchOptionsSet(&boptSet);

	
	AddPage(&bgen);
	AddPage(&basp);
}

CBatchPropSheet::~CBatchPropSheet()
{
	if(graphSet.IsOpen())
	{
		graphSet.Close();
	}
	if (boptSet.IsOpen())
		boptSet.Close();

	
}


BEGIN_MESSAGE_MAP(CBatchPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CBatchPropSheet)
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_MYCANCEL, OnMyCancel)
	ON_BN_CLICKED(IDC_MYOK, OnMyOK)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchPropSheet message handlers



int CBatchPropSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO: Add your specialized creation code here
	CRect rect, bRect;//, tRect;
	GetWindowRect(&rect);
	//rect.bottom += 100;
	rect.right += 120;
	MoveWindow(rect);
	

	int step = rect.Height() / 20;
	bRect.left = rect.Width() - 115;
	bRect.right = rect.Width() - 5;
	bRect.top = rect.Height() / 10;
	bRect.bottom = bRect.top + rect.Height() / 10;
	okButton.Create("OK", WS_CHILD | WS_VISIBLE | WS_TABSTOP, bRect,
		this, IDC_MYOK);


	bRect.top += rect.Height() / 10 + step;
	bRect.bottom += rect.Height() / 10 + step;
	cancelButton.Create("Close", WS_CHILD | WS_VISIBLE | WS_TABSTOP, bRect,
		this, IDC_MYCANCEL);

	return 0;
}

void CBatchPropSheet::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertySheet::PostNcDestroy();
	//notify the main frame that we're going away
	mainFrame->batchProperties = NULL;
	delete this;
}

void CBatchPropSheet::OnMyCancel()
{
	DestroyWindow();
}

void CBatchPropSheet::OnMyOK()
{
	if(basp.m_hWnd)
		basp.SaveData();
    if (bgen.m_hWnd)
		bgen.SaveData();

	DestroyWindow();
}



void CBatchPropSheet::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CPropertySheet::OnActivate(nState, pWndOther, bMinimized);
	if(nState == WA_INACTIVE)// || pWndOther != mainFrame)
		return;
	if(pWndOther && strcmp("CMainFrame", pWndOther->GetRuntimeClass()->m_lpszClassName) != 0)
		return;
	// TODO: Add your message handler code here
	//may need to update the fields - 
	

	
	
}

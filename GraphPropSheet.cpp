// GraphPropSheet.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "GraphSet.h"
#include "wxset.h"
#include "OverlayDialog.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "SplitterFrame.h"
#include "LineGraphProp.h"
#include "BargraphProp.h"
#include "GeneralGraphProp.h"
#include "GraphFiresPage.h"
#include "FDProjPage.h"
//#include "MergedVarsPropPage.h"
#include "GraphAutoSaveProp.h"
#include "GraphPropSheet.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphPropSheet

IMPLEMENT_DYNAMIC(CGraphPropSheet, CPropertySheet)

CGraphPropSheet::CGraphPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage,
		CDatabase *_pDB, CMainFrame *_pFrame)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), gp(), bp(), lp(), fp(), asp(),setDefButton(),
		/*okButton(), */cancelButton(), applyButton(), applyToAllButton(), graphSet(_pDB)
{
	mainFrame = _pFrame;
	if(_pDB)
		graphSet.Open();
	gp.SetGraphSet(&graphSet);
	bp.SetGraphSet(&graphSet);
	lp.SetGraphSet(&graphSet);
	fp.SetGraphSet(&graphSet);
	asp.SetGraphSet(&graphSet);
	pp.m_pDB = _pDB;
	AddPage(&gp);
	AddPage(&bp);
	AddPage(&lp);
	AddPage(&fp);
	AddPage(&pp);
	AddPage(&asp);

//	AddPage(&mp);
}

CGraphPropSheet::~CGraphPropSheet()
{
	if(graphSet.IsOpen())
	{
		graphSet.Close();
	}
}


BEGIN_MESSAGE_MAP(CGraphPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CGraphPropSheet)
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_MYCANCEL, OnMyCancel)
	ON_BN_CLICKED(IDC_MYOK, OnMyOK)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_APPLYTOALL, OnApplyToAll)
	ON_BN_CLICKED(IDC_SETDEFBUTTON, OnSetAsDefaults)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphPropSheet message handlers



int CGraphPropSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO: Add your specialized creation code here
	CRect rect, bRect;//, tRect;
	GetWindowRect(&rect);
	//rect.bottom += 100;
	rect.right += 140;
	MoveWindow(rect);
	//tRect.left = 1;
	//tRect.right = rect.Width() / 4;
	//tRect.top = rect.Height() - 30;
//	tRect.bottom = rect.Height() - 1;
	//CButton bt;
	//bt.Create("Set As Default", WS_CHILD | WS_VISIBLE | WS_TABSTOP, tRect,
	//	this, IDC_SETDEFBUTTON);
	int step = rect.Height() / 20;
	bRect.left = rect.Width() - 125;
	bRect.right = rect.Width() - 5;
	bRect.top = rect.Height() / 10;
	bRect.bottom = bRect.top + rect.Height() / 10;
	setDefButton.Create("Set As Default", WS_CHILD | WS_VISIBLE | WS_TABSTOP, bRect,
		this, IDC_SETDEFBUTTON);
	//bRect.top += rect.Height() / 10 + step;
//	bRect.bottom += rect.Height() / 10 + step;
//	okButton.Create("OK", WS_CHILD | WS_VISIBLE | WS_TABSTOP, bRect,
//		this, IDC_MYOK);
	bRect.top += rect.Height() / 10 + step;
	bRect.bottom += rect.Height() / 10 + step;
	cancelButton.Create("Close", WS_CHILD | WS_VISIBLE | WS_TABSTOP, bRect,
		this, IDC_MYCANCEL);
	bRect.top += rect.Height() / 10 + step;
	bRect.bottom += rect.Height() / 10 + step;
	applyButton.Create("Apply", WS_CHILD | WS_VISIBLE | WS_TABSTOP, bRect,
		this, IDC_APPLY);
	bRect.top += rect.Height() / 10 + step;
	bRect.bottom += rect.Height() / 10 + step;
	applyToAllButton.Create("Apply To All", WS_CHILD | WS_VISIBLE | WS_TABSTOP, bRect,
		this, IDC_APPLYTOALL);
	return 0;
}

void CGraphPropSheet::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertySheet::PostNcDestroy();
	//notify the main frame that we're going away
	mainFrame->graphProperties = NULL;
	delete this;
}

void CGraphPropSheet::OnMyCancel()
{
	DestroyWindow();
}

void CGraphPropSheet::OnMyOK()
{

	DestroyWindow();
}

void CGraphPropSheet::OnSetAsDefaults()
{
	if(graphSet.IsOpen())
	{
		graphSet.Edit();
		if(gp.m_hWnd)
			gp.SaveData();
		if(bp.m_hWnd)
			bp.SaveData();
		if(lp.m_hWnd)
			lp.SaveData();
		if(fp.m_hWnd)
			fp.SaveData();
		if(pp.m_hWnd)
			pp.SaveData();
		
		if(pp.m_hWnd)
			pp.SaveData();
		if (asp.m_hWnd)
			asp.SaveData();
		graphSet.Update();
	}
//	if(mp.m_hWnd)
//		mp.WriteData(NULL);
}

void CGraphPropSheet::OnApply()
{
	CWnd *child = mainFrame->MDIGetActive(); 
	CRuntimeClass* prt = child->GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, "CSplitterFrame" ) == 0)
		ApplyToGraph((CSplitterFrame *)child);
}

void CGraphPropSheet::OnApplyToAll()
{
	CWnd *child = mainFrame->MDIGetActive(), *topMost = NULL; 
	while(child)
	{
		CRuntimeClass* prt = child->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CSplitterFrame" ) == 0)
			ApplyToGraph((CSplitterFrame *)child);
		child = child->GetNextWindow();
	}

}

void CGraphPropSheet::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CPropertySheet::OnActivate(nState, pWndOther, bMinimized);
	if(nState == WA_INACTIVE)// || pWndOther != mainFrame)
		return;
	CGraphView *pView = NULL;
	if(pWndOther && strcmp("CMainFrame", pWndOther->GetRuntimeClass()->m_lpszClassName) != 0)
		return;
	// TODO: Add your message handler code here
	//may need to update the fields - 
	//first, check for graph windows being topMost and get a count
	CWnd *child = mainFrame->MDIGetActive(), *topMost = NULL; 
	int count = 0;
	CRuntimeClass* prt = child->GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, "CSplitterFrame" ) == 0)
		topMost = child;
	child = child->GetNextWindow();
	while(child)
	{
		prt = child->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CSplitterFrame" ) == 0)
			count++;
		child = child->GetNextWindow();
	}
	//enable/disable control buttons
	if(topMost)
		applyButton.EnableWindow(true);
	else
		applyButton.EnableWindow(false);
	if(count > 0)
		applyToAllButton.EnableWindow(true);
	else
		applyToAllButton.EnableWindow(false);
	//if topMost, a graph's values need to be read in
	if(topMost)
		pView = (CGraphView *) ((CSplitterFrame *)topMost)->m_wndSplitter.GetPane(0, 0);
	if(gp.m_hWnd)
		gp.ReadData(pView);
	if(bp.m_hWnd)
		bp.ReadData(pView);
	if(lp.m_hWnd)
		lp.ReadData(pView);
	if(fp.m_hWnd)
		fp.ReadData(pView);
	if(pp.m_hWnd)
		pp.ReadData(pView);
	if (asp.m_hWnd)
		asp.ReadData();

	//otherwise, read from database
	
}

void CGraphPropSheet::ApplyToGraph(CSplitterFrame * frame)
{
	CGraphView *pView = NULL;
	if(frame)
		pView = (CGraphView *) frame->m_wndSplitter.GetPane(0, 0);
	if(gp.m_hWnd)
		gp.WriteData(pView);
	if(bp.m_hWnd)
		bp.WriteData(pView);
	if(lp.m_hWnd)
		lp.WriteData(pView);
	if(fp.m_hWnd)
		fp.WriteData(pView);
	if(pp.m_hWnd)
		pp.WriteData(pView);
	if (asp.m_hWnd)
		asp.WriteData();
//	if(mp.m_hWnd)
//		mp.WriteData(frame);
	pView->Invalidate();
	//frame->Invalidate();
	//frame->UpdateWindow();
}

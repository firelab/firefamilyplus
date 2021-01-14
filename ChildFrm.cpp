// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "fireplus.h"

#include "ChildFrm.h"
#include "graphset.h"
#include "MainFrm.h"
#include "wxset.h"
#include "OverlayDialog.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "SplitterFrame.h"
#include "LineGraphProp.h"
#include "BargraphProp.h"
#include "GeneralGraphProp.h"
#include "GraphAutoSaveProp.h"
#include "GraphFiresPage.h"
#include "FDProjPage.h"
//#include "MergedVarsPropPage.h"
#include "GraphPropSheet.h"
#include "ClimateView.h"
#include "fireplusDoc.h"
#include "SIGStationSet.h"
#include "FireSumSet.h"
#include "ClimAnalysis.h"
#include "MergedFrame.h"
#include "MergedView.h"
#include "PocketCardFrame.h"
#include "PocketCardView.h"
#include <afxrich.h>
#include "richView.h"
#include "richDoc.h"
#include "Firestats.h"
#include "richfiredoc.h"
#include "firestatframe.h"
#include "DPDoc.h"
#include "DPFrame.h"
#include "DPReportView.h"
#include "BatchOutputDoc.h"
#include "BatchOutputFrame.h"
#include "cumulativebars.h"
#include "cumulativeframe.h"
#include "firestatdoc.h"
#include "richtermdoc.h"
#include "termframe.h"
#include "WeatherView.h"
#include "WeatherFrame.h"
#include "FiresView.h"
#include "FiresFrame.h"
#include "EventFrame.h"
#include "eventlocatordoc.h"
#include "TermDatesDialog.h"
#include "CandidatesFrame.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	return CMDIChildWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

void CChildFrame::ActivateFrame(int nCmdShow) 
{
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

void CChildFrame::OnClose() 
{
	//make sure graphOptions property sheet is gone first
	CMainFrame *frame = (CMainFrame *)GetParentOwner();
	if(frame->graphProperties)
	{
		frame->graphProperties->OnMyCancel();
		frame->graphProperties = NULL;
	}
	//first, count dependent windows
	int nDependents = 0;
	CFireplusDoc *pDoc = (CFireplusDoc *)GetActiveDocument();
	CWnd *hWndChild = this;//GetMDIFrame()->MDIGetActive();
	while(hWndChild)
	{
		CRuntimeClass* prt = hWndChild->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CMDIChildWnd" )  == 0 )
		{
			CMDIChildWnd *pWnd = (CMDIChildWnd *)hWndChild;
			CClimateView * pClimView = (CClimateView *)pWnd->GetActiveView();
			if(pClimView->m_climSet->m_pDatabase == pDoc->m_pDB)
				nDependents++;
		}
		if(strcmp( prt->m_lpszClassName, "CSplitterFrame" )  == 0 )
		{
			CSplitterFrame *pWnd = (CSplitterFrame *)hWndChild;
			CView *pView = pWnd->GetActiveView();
			CPeriodDoc *tDoc = (CPeriodDoc *)pView->GetDocument();
			if(tDoc->m_pAnalysis->pDB == pDoc->m_pDB)
			//if(pView->m_pAnalysis->pDB == pDoc->m_pDB)
				nDependents++;
		}
		if(strcmp( prt->m_lpszClassName, "CMergedFrame" )  == 0 )
		{
			CMergedFrame *pWnd = (CMergedFrame *)hWndChild;
			CMergedView * pView = (CMergedView *)pWnd->GetActiveView();
			CPeriodDoc *tDoc = (CPeriodDoc *)pView->GetDocument();
			if(tDoc->m_pAnalysis->pDB == pDoc->m_pDB)
				nDependents++;
		}
		if(strcmp( prt->m_lpszClassName, "CPocketCardFrame" )  == 0 )
		{
			CPocketCardFrame *pWnd = (CPocketCardFrame *)hWndChild;
			CPocketCardView * pView = (CPocketCardView *)pWnd->GetActiveView();
			if(pView->m_pAnalysis->pDB == pDoc->m_pDB)
				nDependents++;
		}
		if(strcmp( prt->m_lpszClassName, "CFireStatFrame" )  == 0 )
		{
			CFireStatFrame *pWnd = (CFireStatFrame *)hWndChild;
			CView * pView = pWnd->GetActiveView();
			CRichFireDoc *tDoc = (CRichFireDoc *)pView->GetDocument();
			if(tDoc->fpSet->m_pDatabase == pDoc->m_pDB)
				nDependents++;
		}
		if(strcmp( prt->m_lpszClassName, "CDPFrame" )  == 0 )
		{
			CDPFrame *pWnd = (CDPFrame *)hWndChild;
			CDPReportView * pView = (CDPReportView *)pWnd->GetActiveView();
			CDPDoc *tDoc = (CDPDoc *)pView->GetDocument();
			if(tDoc->fires->m_pSet->m_pDatabase == pDoc->m_pDB)
				nDependents++;
		}
		if(strcmp( prt->m_lpszClassName, "CBatchOutputFrame" )  == 0 )
		{
			CBatchOutputFrame *pWnd = (CBatchOutputFrame *)hWndChild;
			//CView * pView = pWnd->GetActiveView();
			CQuickBatch *tDoc = pWnd->m_pDoc;
			if(tDoc->m_pDB == pDoc->m_pDB)
				nDependents++;
		}
		if(strcmp( prt->m_lpszClassName, "CCumulativeFrame" )  == 0 )
		{
			CCumulativeFrame *pWnd = (CCumulativeFrame *)hWndChild;
			CView * pView = pWnd->GetActiveView();
			CFireStatDoc *tDoc = (CFireStatDoc *)pView->GetDocument();
			if(tDoc->fires->m_pSet->m_pDatabase == pDoc->m_pDB)
				nDependents++;
		}
		if(strcmp( prt->m_lpszClassName, "CTermFrame" )  == 0 )
		{
			CTermFrame *pWnd = (CTermFrame *)hWndChild;
			CView * pView = pWnd->GetActiveView();
			CRichTermDoc *tDoc = (CRichTermDoc *)pView->GetDocument();
			if(tDoc->m_pDB == pDoc->m_pDB)
				nDependents++;
		}
		if(strcmp( prt->m_lpszClassName, "CWeatherFrame" )  == 0 )
		{
			CWeatherFrame *pWnd = (CWeatherFrame *)hWndChild;
			CWeatherView * pWxView = (CWeatherView *)pWnd->GetActiveView();
			if(pWxView->records->m_pDatabase == pDoc->m_pDB)
				nDependents++;
		}
	
		if(strcmp( prt->m_lpszClassName, "CFiresFrame" )  == 0 )
		{
			CFiresFrame *pWnd = (CFiresFrame *)hWndChild;
			CFiresView * pView = (CFiresView *)pWnd->GetActiveView();
			if(pView->records->m_pDatabase == pDoc->m_pDB)
				nDependents++;
		}
		if(strcmp( prt->m_lpszClassName, "CEventFrame" )  == 0 )
		{
			CEventFrame *pWnd = (CEventFrame *)hWndChild;
			CView * pView = pWnd->GetActiveView();
			CEventLocatorDoc *tDoc = (CEventLocatorDoc *)pView->GetDocument();
			if(tDoc->m_pDB == pDoc->m_pDB)
				nDependents++;
		}
		if(strcmp( prt->m_lpszClassName, "CCandidatesFrame" )  == 0 )
		{
			CCandidatesFrame *pWnd = (CCandidatesFrame *)hWndChild;
			//CView * pView = pWnd->GetActiveView();
			//CEventLocatorDoc *tDoc = (CEventLocatorDoc *)pView->GetDocument();
			if(pWnd->m_pDB == pDoc->m_pDB)
				nDependents++;
		}
		hWndChild = hWndChild->GetNextWindow();
	}
	if(nDependents > 0)
	{
		if(IDCANCEL == AfxMessageBox("The database you are attempting to close has dependent windows open.\n"
			"Select OK to close the database and all dependent windows.", MB_OKCANCEL))
		{
			return;
		}
	}
	hWndChild = this;
	while(hWndChild)
	{
		CRuntimeClass* prt = hWndChild->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CMDIChildWnd" )  == 0 )
		{
			CMDIChildWnd *pWnd = (CMDIChildWnd *)hWndChild;
			CClimateView * pClimView = (CClimateView *)pWnd->GetActiveView();
			if(pClimView->m_climSet->m_pDatabase == pDoc->m_pDB)
			{
				CWnd *dest = hWndChild;
				hWndChild = hWndChild->GetNextWindow();
				dest->DestroyWindow();
				continue;
			}
		}
		if(strcmp( prt->m_lpszClassName, "CSplitterFrame" )  == 0 )
		{
			CSplitterFrame *pWnd = (CSplitterFrame *)hWndChild;
			CView *pView = pWnd->GetActiveView();
			CPeriodDoc *tDoc = (CPeriodDoc *)pView->GetDocument();
			if(tDoc->m_pAnalysis->pDB == pDoc->m_pDB)
			{
				CWnd *dest = hWndChild;
				hWndChild = hWndChild->GetNextWindow();
				dest->DestroyWindow();
				continue;
			}
		}
		if(strcmp( prt->m_lpszClassName, "CMergedFrame" )  == 0 )
		{
			CMergedFrame *pWnd = (CMergedFrame *)hWndChild;
			CMergedView * pView = (CMergedView *)pWnd->GetActiveView();
			CPeriodDoc *tDoc = (CPeriodDoc *)pView->GetDocument();
			if(tDoc->m_pAnalysis->pDB == pDoc->m_pDB)
			{
				CWnd *dest = hWndChild;
				hWndChild = hWndChild->GetNextWindow();
				dest->DestroyWindow();
				continue;
			}
		}
		if(strcmp( prt->m_lpszClassName, "CPocketCardFrame" )  == 0 )
		{
			CPocketCardFrame *pWnd = (CPocketCardFrame *)hWndChild;
			CPocketCardView * pView = (CPocketCardView *)pWnd->GetActiveView();
			if(pView->m_pAnalysis->pDB == pDoc->m_pDB)
			{
				CWnd *dest = hWndChild;
				hWndChild = hWndChild->GetNextWindow();
				dest->DestroyWindow();
				continue;
			}
		}
		if(strcmp( prt->m_lpszClassName, "CFireStatFrame" )  == 0 )
		{
			CFireStatFrame *pWnd = (CFireStatFrame *)hWndChild;
			CView * pView = pWnd->GetActiveView();
			CRichFireDoc *tDoc = (CRichFireDoc *)pView->GetDocument();
			if(tDoc->fpSet->m_pDatabase == pDoc->m_pDB)
			{
				CWnd *dest = hWndChild;
				hWndChild = hWndChild->GetNextWindow();
				dest->DestroyWindow();
				continue;
			}
		}
		if(strcmp( prt->m_lpszClassName, "CDPFrame" )  == 0 )
		{
			CDPFrame *pWnd = (CDPFrame *)hWndChild;
			CDPReportView * pView = (CDPReportView *)pWnd->GetActiveView();
			CDPDoc *tDoc = (CDPDoc *)pView->GetDocument();
			if(tDoc->fires->m_pSet->m_pDatabase == pDoc->m_pDB)
			{
				CWnd *dest = hWndChild;
				hWndChild = hWndChild->GetNextWindow();
				dest->DestroyWindow();
				continue;
			}
		}
		if(strcmp( prt->m_lpszClassName, "CBatchOutputFrame" )  == 0 )
		{
			CBatchOutputFrame *pWnd = (CBatchOutputFrame *)hWndChild;
			//CView * pView = pWnd->GetActiveView();
			CQuickBatch *tDoc = pWnd->m_pDoc;
			if(tDoc->m_pDB == pDoc->m_pDB)
			{
				CWnd *dest = hWndChild;
				hWndChild = hWndChild->GetNextWindow();
				dest->DestroyWindow();
				continue;
			}
		}
		if(strcmp( prt->m_lpszClassName, "CCumulativeFrame" )  == 0 )
		{
			CCumulativeFrame *pWnd = (CCumulativeFrame *)hWndChild;
			CView * pView = pWnd->GetActiveView();
			CFireStatDoc *tDoc = (CFireStatDoc *)pView->GetDocument();
			if(tDoc->fires->m_pSet->m_pDatabase == pDoc->m_pDB)
			{
				CWnd *dest = hWndChild;
				hWndChild = hWndChild->GetNextWindow();
				dest->DestroyWindow();
				continue;
			}
		}
		if(strcmp( prt->m_lpszClassName, "CTermFrame" )  == 0 )
		{
			CTermFrame *pWnd = (CTermFrame *)hWndChild;
			//CView * pView = pWnd->GetActiveView();
			CView * pView = pWnd->GetActiveView();
			CRichTermDoc *tDoc = (CRichTermDoc *)pView->GetDocument();
			if(tDoc->m_pDB == pDoc->m_pDB)
			{
				CWnd *dest = hWndChild;
				hWndChild = hWndChild->GetNextWindow();
				dest->DestroyWindow();
				continue;
			}
		}
		if(strcmp( prt->m_lpszClassName, "CWeatherFrame" )  == 0 )
		{
			CWeatherFrame *pWnd = (CWeatherFrame *)hWndChild;
			CWeatherView * pWxView = (CWeatherView *)pWnd->GetActiveView();
			if(pWxView->records->m_pDatabase == pDoc->m_pDB)
			{
				CWnd *dest = hWndChild;
				hWndChild = hWndChild->GetNextWindow();
				dest->DestroyWindow();
				continue;
			}
		}
		
		if(strcmp( prt->m_lpszClassName, "CFiresFrame" )  == 0 )
		{
			CFiresFrame *pWnd = (CFiresFrame *)hWndChild;
			CFiresView * pView = (CFiresView *)pWnd->GetActiveView();
			if(pView->records->m_pDatabase == pDoc->m_pDB)
			{
				CWnd *dest = hWndChild;
				hWndChild = hWndChild->GetNextWindow();
				dest->DestroyWindow();
				continue;
			}
		}
		if(strcmp( prt->m_lpszClassName, "CEventFrame" )  == 0 )
		{
			CEventFrame *pWnd = (CEventFrame *)hWndChild;
			CView * pView = pWnd->GetActiveView();
			CEventLocatorDoc *tDoc = (CEventLocatorDoc *)pView->GetDocument();
			if(tDoc->m_pDB == pDoc->m_pDB)
			{
				CWnd *dest = hWndChild;
				hWndChild = hWndChild->GetNextWindow();
				dest->DestroyWindow();
				continue;
			}
		}
		if(strcmp( prt->m_lpszClassName, "CCandidatesFrame" )  == 0 )
		{
			CCandidatesFrame *pWnd = (CCandidatesFrame *)hWndChild;
			//CView * pView = pWnd->GetActiveView();
			//CEventLocatorDoc *tDoc = (CEventLocatorDoc *)pView->GetDocument();
			if(pWnd->m_pDB == pDoc->m_pDB)
			{
				CWnd *dest = hWndChild;
				hWndChild = hWndChild->GetNextWindow();
				dest->DestroyWindow();
				continue;
			}
		}
		hWndChild = hWndChild->GetNextWindow();
	}

	CMDIChildWnd::OnClose();
}

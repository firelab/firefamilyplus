// SplitterFrame.cpp : implementation file
//
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "wxset.h"
#include "OverlayDialog.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "SplitterFrame.h"
#include "PercentileView.h"
#include "GraphView.h"
#include "OverlayDialog.h"
//#include "PrintGraphDialog.h"
//#include "SaveGraohDialog.h"
//#include "PrecipPercentileView.h"
#include "MergeDialog.h"
#include "climateSet.h"
#include "SIGStationSet.h"
#include "CFiresSet.h"
#include "ClimAnalysis.h"
#include ".\splitterframe.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT sindicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_XPROMPT,
	ID_INDICATOR_X,
	ID_INDICATOR_YPROMPT,
	ID_INDICATOR_Y,
//	ID_INDICATOR_CAPS,
};

extern CFireplusApp theApp;

//DECLARE_HANDLE(HDIB);

/////////////////////////////////////////////////////////////////////////////
// CSplitterFrame

IMPLEMENT_DYNCREATE(CSplitterFrame, CMDIChildWnd)

CSplitterFrame::CSplitterFrame()
{
//	mergedVars[0] = mergedVars[1] = mergedVars[2] = -1;
//	mergedVarNames[0] = mergedVarNames[1] = mergedVarNames[2] = "";
//	mergedYMax[0] = mergedYMax[1] = mergedYMax[2] = 1.0;
//	mergedColors[0] = theApp.mergeColor1;
//	mergedColors[1] = theApp.mergeColor2;
//	mergedColors[2] = theApp.mergeColor3;
//	mergedColors[3] = theApp.mergeColor4;
//	mergedStyles[0] = LineStyleToString(theApp.mergeStyle1);
//	mergedStyles[1] = LineStyleToString(theApp.mergeStyle2);
//	mergedStyles[2] = LineStyleToString(theApp.mergeStyle3);
//	mergedStyles[3] = LineStyleToString(theApp.mergeStyle4);
//	mergedWidths[0] = theApp.mergeWidth1;
//	mergedWidths[1] = theApp.mergeWidth2;
//	mergedWidths[2] = theApp.mergeWidth3;
//	mergedWidths[3] = theApp.mergeWidth4;
//	pcpSums = NULL;
//	m_frameOpts.nP = 0;
}

CSplitterFrame::~CSplitterFrame()
{
/*	if(pcpSums)
	{
		for(int p = nP - 1; p >= 0; p--)
			delete pcpSums[p];
		delete[] pcpSums;
	}*/
}


BEGIN_MESSAGE_MAP(CSplitterFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CSplitterFrame)
	ON_COMMAND(ID_OPTIONS_OVERLAYS, OnOptionsOverlays)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_GRAPHTYPE_LINEATAVERAGE, OnUpdateOptionsGraphtypeLineataverage)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_GRAPHTYPE_STATISTICALBARS, OnUpdateOptionsGraphtypeStatisticalbars)
	ON_COMMAND(ID_FILE_PRINT_PRINTGRAPH, OnFilePrintPrintgraph)
	ON_COMMAND(ID_FILE_PRINT1, OnFilePrint1)
	ON_COMMAND(ID_OPTIONS_MERGE, OnOptionsMerge)
	ON_COMMAND(ID_SAVE_GRAPH, OnSaveGraph)
	ON_COMMAND(ID_SAVE_PERCENTILEGRAPH, OnSavePercentileGraph)
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplitterFrame message handlers

BOOL CSplitterFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(sindicators,
		  sizeof(sindicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	//CONFIGURE STATUS BAR
	UINT id, style;
	int wid;
	m_wndStatusBar.GetPaneInfo(1, id, style , wid);
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_XPROMPT, SBPS_NOBORDERS , wid);
	m_wndStatusBar.GetPaneInfo(2, id, style , wid);
	m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_X, SBPS_NORMAL , wid + wid);
	m_wndStatusBar.GetPaneInfo(3, id, style , wid);
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_YPROMPT, SBPS_NOBORDERS , wid);
	m_wndStatusBar.GetPaneInfo(4, id, style , wid);
	m_wndStatusBar.SetPaneInfo(4, ID_INDICATOR_Y, SBPS_NORMAL , wid + wid);
	m_wndStatusBar.SetPaneText(0, "Left click to retrieve values");
	m_wndStatusBar.SetPaneText(2, "");
	m_wndStatusBar.SetPaneText(4, "");

	//return CMDIChildWnd::OnCreateClient(lpcs, pContext);
	// create a splitter with 1 row, 2 columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}
	//LPRECT rect;
	//GetClientRect(rect);
	//int wid = (rect->right - rect->left) / 3 * 2;
	// add the first splitter pane - the default view in column 0
	if (!m_wndSplitter.CreateView(0, 0,
		pContext->m_pNewViewClass, CSize(400, 50), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - an input view in column 1
	/*if(strcmp(pContext->m_pNewViewClass->m_lpszClassName, "CPrecipGraphView") == 0)
	{
		if (!m_wndSplitter.CreateView(0, 1,
			RUNTIME_CLASS(CPrecipPercentileView), CSize(0, 0), pContext))
		{
			TRACE0("Failed to create second pane\n");
			return FALSE;
		}
	}
	else
	{*/
		if (!m_wndSplitter.CreateView(0, 1,
			RUNTIME_CLASS(CPercentileView), CSize(0, 0), pContext))
		{
			TRACE0("Failed to create second pane\n");
			return FALSE;
		}
	//}
	//((CGraphView*)m_wndSplitter.GetPane(0,0))->varID = varID;
//	((CPercentileView*)m_wndSplitter.GetPane(0,1))->varID = varID;
	// activate the input view
	//SetActiveView((CView*)m_wndSplitter.GetPane(0,0));

	return TRUE;
}

void CSplitterFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);
//	((CGraphView*)m_wndSplitter.GetPane(0,0))->varID = m_frameOpts.varID;
//	((CPercentileView*)m_wndSplitter.GetPane(0,1))->varID = m_frameOpts.varID;

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	/*CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle && pDocument != NULL)
	{
		TCHAR szText[256+_MAX_PATH];
		lstrcpy(szText, pDocument->GetTitle());
		if (m_nWindow > 0)
			wsprintf(szText + lstrlen(szText), _T(":%d"), m_nWindow);

		// set title if changed, but don't remove completely
		AfxSetWindowText(m_hWnd, szText);
	}*/
}

void CSplitterFrame::MaximizePane(CView * view)
{
	int r, c, cxCur0, cxMin0, cxCur1, cxMin1;
	if(m_wndSplitter.IsChildPane((CWnd *)view, &r, &c))	//safety
	{
		m_wndSplitter.GetColumnInfo(0, cxCur0, cxMin0);
		m_wndSplitter.GetColumnInfo(1, cxCur1, cxMin1);
		m_wndSplitter.SetColumnInfo((c == 0) ? 0 : 1, cxCur1 + cxCur0 - 4, cxMin0);
		m_wndSplitter.SetColumnInfo((c == 0) ? 1 : 0, 4, cxMin0);
		m_wndSplitter.RecalcLayout();
	}
}

void CSplitterFrame::OnOptionsOverlays() 
{
	CPeriodDoc* pDoc = (CPeriodDoc*)GetActiveDocument();
	if(pDoc->m_pAnalysis->m_opts.m_OverlayDialog)
	{
		pDoc->m_pAnalysis->m_opts.m_OverlayDialog->SetActiveWindow();
		return;
	}
	POSITION vPos = pDoc->GetFirstViewPosition();
	//pDoc->m_pAnalysis->m_opts.m_OverlayDialog = new COverlayDialog(this, pDoc, ((CGraphView *)pDoc->GetNextView(vPos))->varID);
	pDoc->m_pAnalysis->m_opts.m_OverlayDialog = new COverlayDialog(this, pDoc, pDoc->m_pAnalysis, ((CGraphView *)pDoc->GetNextView(vPos))->varID);
	pDoc->m_pAnalysis->m_opts.m_OverlayDialog->Create();
}

void CSplitterFrame::OnUpdateOptionsGraphtypeLineataverage(CCmdUI* pCmdUI) 
{
	CPeriodDoc* pDoc = (CPeriodDoc*)GetActiveDocument();
	POSITION vPos = pDoc->GetFirstViewPosition();
	CGraphView *pView = (CGraphView *)pDoc->GetNextView(vPos);
	pCmdUI->SetCheck(!pView->m_graphOpts.isBargraph);
	//pCmdUI->SetCheck(!m_frameOpts.isBargraph);	
}

void CSplitterFrame::OnUpdateOptionsGraphtypeStatisticalbars(CCmdUI* pCmdUI) 
{
	CPeriodDoc* pDoc = (CPeriodDoc*)GetActiveDocument();
	POSITION vPos = pDoc->GetFirstViewPosition();
	CGraphView *pView = (CGraphView *)pDoc->GetNextView(vPos);
	pCmdUI->SetCheck(pView->m_graphOpts.isBargraph);
	//pCmdUI->SetCheck(m_frameOpts.isBargraph);	
	
}

/*void CSplitterFrame::OnFilePrint() 
{
	// TODO: Add your command handler code here
	CPrintGraphDialog pdlg(this);
	if(pdlg.DoModal() == IDOK)
	{
		if(!pdlg.pPrint && !pdlg.gPrint)
			return; //nothing to do
		CDC pDC;
		theApp.CreatePrinterDC(pDC);
		if(pDC)
		{
			DOCINFO docinfo;
			docinfo.cbSize = sizeof(DOCINFO);
			docinfo.lpszDocName = "FireFamily Plus Graph";
			docinfo.lpszOutput = NULL;
			pDC.StartDoc(&docinfo);
			pDC.StartPage();
			
			int lx, ly;
			ly = pDC.GetDeviceCaps(LOGPIXELSY);
			lx = pDC.GetDeviceCaps(LOGPIXELSX);
			if(pdlg.gPrint)//print graphview
			{	//first, set region
				CRect bnd(pdlg.gleft * lx, pdlg.gtop * ly,
					pdlg.gright * lx, pdlg.gbottom * ly);
				//draw graph
				((CGraphView *)m_wndSplitter.GetPane(0, 0))->DrawToDCRegion(&pDC, bnd);
			}
			if(pdlg.pPrint)//print graphview
			{	//first, set region
				CRect bnd(pdlg.pleft * lx, pdlg.ptop * ly,
					pdlg.pright * lx, pdlg.pbottom * ly);
				//draw graph
				((CPercentileView *)m_wndSplitter.GetPane(0, 1))->DrawToDCRegion(&pDC, bnd);
			}
			pDC.EndPage();
			pDC.EndDoc();
		}
	}
}*/

/*void CSplitterFrame::SaveGraph()
{
	CSaveGraphDialog sgd(this);
	sgd.DoModal();
}*/


void CSplitterFrame::OnSaveGraph()
{
	((CGraphView *)m_wndSplitter.GetPane(0, 0))->SaveGraph();
}

void CSplitterFrame::OnSavePercentileGraph()
{
	((CPercentileView *)m_wndSplitter.GetPane(0, 1))->SaveGraph();
}

void CSplitterFrame::OnFilePrintPrintgraph() 
{
	// TODO: Add your command handler code here
	((CGraphView *)m_wndSplitter.GetPane(0, 0))->PrintGraph();
	
}

void CSplitterFrame::OnFilePrint1() 
{
	// TODO: Add your command handler code here
	((CPercentileView *)m_wndSplitter.GetPane(0, 1))->PrintGraph();
	
}

void CSplitterFrame::OnOptionsMerge() 
{
	CPeriodDoc* pDoc = (CPeriodDoc*)GetActiveDocument();
	if(pDoc->m_pAnalysis->m_opts.nVars > 1)
	{
		CMergeDialog md(this);
		md.pDoc = pDoc;
		//md.trgView = (CGraphView *)m_wndSplitter.GetPane(0, 0);
		if(md.DoModal() == IDOK)
		{
			pDoc->UpdateAllViews(NULL);
		}
	}
	else
		AfxMessageBox("More than one statistics graph must be\ngenerated to allow merging of graphs.");
	
}




void CSplitterFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CMDIChildWnd::OnLButtonDblClk(nFlags, point);
	CView *pView = (CView *)m_wndSplitter.GetActivePane();
	MaximizePane(pView);
}

void CSplitterFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	CMDIChildWnd::OnLButtonUp(nFlags, point);
}

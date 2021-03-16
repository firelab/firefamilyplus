// FireStatFrame.cpp : implementation file
//
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "TermFrame.h"
#include "TermGraph.h"
#include <direct.h>
//#include "FireStatReport.h"
#include <afxrich.h>
#include "richtermdoc.h"
#include "richdoc.h"
#include "richview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CFireplusApp theApp;

static UINT sindicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_TERM_DAY_PROMPT,
	ID_INDICATOR_TERM_DAY,
	ID_INDICATOR_TERM_PROB_PROMPT,
	ID_INDICATOR_TERM_PROB,
//	ID_INDICATOR_CAPS,
};

/////////////////////////////////////////////////////////////////////////////
// CTermFrame

IMPLEMENT_DYNCREATE(CTermFrame, CMDIChildWnd)

CTermFrame::CTermFrame()
{
}

CTermFrame::~CTermFrame()
{
}


BEGIN_MESSAGE_MAP(CTermFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CTermFrame)
	ON_COMMAND(ID_FILE_PRINT_PRINTGRAPH, OnFilePrintPrintgraph)
	ON_COMMAND(ID_FILE_SAVE_GRAPH, OnFileSaveGraph)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_PRINT_REPORT, OnFilePrintReport)
	//ON_COMMAND(ID_OPTIONS_VIEWHEADERINFO, OnViewHeader)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTermFrame message handlers

BOOL CTermFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
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
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_TERM_DAY_PROMPT, SBPS_NOBORDERS , wid);
	m_wndStatusBar.GetPaneInfo(2, id, style , wid);
	m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_TERM_DAY, SBPS_NORMAL , wid + wid);
	m_wndStatusBar.GetPaneInfo(3, id, style , wid);
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_TERM_PROB_PROMPT, SBPS_NOBORDERS , wid);
	m_wndStatusBar.GetPaneInfo(4, id, style , wid);
	m_wndStatusBar.SetPaneInfo(4, ID_INDICATOR_TERM_PROB, SBPS_NORMAL , wid + wid);
	m_wndStatusBar.SetPaneText(0, "Move pointer to retrieve values");
	m_wndStatusBar.SetPaneText(2, "");
	m_wndStatusBar.SetPaneText(4, "");
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}
	// add the second splitter pane - which is a nested splitter with 2 rows
/*	if (!m_wndSplitter2.CreateStatic(
		&m_wndSplitter,     // our parent window is the first splitter
		2, 1,               // the new splitter is 2 rows, 1 column
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		m_wndSplitter.IdFromRowCol(0, 0)
			// new splitter is in the first row, 1st column of first splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}*/
	//LPRECT rect;
	//GetClientRect(rect);
	//int wid = (rect->right - rect->left) / 3 * 2;
	// add the first splitter pane - the default view in column 0
	if (!m_wndSplitter.CreateView(0, 0,
	//if (!m_wndSplitter2.CreateView(0, 0,
		pContext->m_pNewViewClass, CSize(300, 50), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - a graph view in column 1
	if (!m_wndSplitter.CreateView(0, 1,
		RUNTIME_CLASS(CTermGraph), CSize(0, 0), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}
	return TRUE;//CMDIChildWnd::OnCreateClient(lpcs, pContext);
}

void CTermFrame::MaximizePane(CView * view)
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

void CTermFrame::OnFilePrintPrintgraph() 
{
	// TODO: Add your command handler code here
	((CTermGraph *)m_wndSplitter.GetPane(0, 1))->PrintGraph();
	
}

void CTermFrame::OnFileSaveGraph() 
{
	chdir(theApp.userDir);
	CFileDialog fd(false, theApp.GetGraphicsExt(), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		theApp.GetGraphicsFileStr(), this);
	if(fd.DoModal() == IDOK)
	{
		CDC *whdc = GetDC();
		CDC memhdc;
		memhdc.CreateCompatibleDC(NULL);
		HBITMAP picture = CreateCompatibleBitmap(whdc->m_hDC, 640, 480);
		HGDIOBJ Old = SelectObject(memhdc.m_hDC, picture);
		//CRect bnd(0, 0, pWidth.GetValueInteger(), pHeight.GetValueInteger());
		CRect rect(0, 0, 640, 480);
		((CTermGraph *)m_wndSplitter.GetPane(0, 1))->DrawToDCRegion(&memhdc, rect);
		char fName[256];
		strcpy_s(fName, fd.GetPathName());
		SaveBitmapFile(memhdc.m_hDC, picture, fName); 
		SelectObject(memhdc.m_hDC, Old);
		ReleaseDC(whdc);
		memhdc.DeleteDC();
		DeleteObject(picture);
		theApp.SetUserDir(fd.GetPathName());
		theApp.SetGraphicsExt(fd.GetFileExt());
	}
	chdir(theApp.dbDir);//theApp.workDir);
	
}

void CTermFrame::OnFileSaveAs() 
{
	((CRichView *)m_wndSplitter.GetPane(0, 0))->SaveTheDamnThing();
}

void CTermFrame::OnFilePrintReport() 
{
	((CRichView *)m_wndSplitter.GetPane(0, 0))->PrintTheDamnThing();
}
// added for batch2
/*void	CTermFrame::OnViewHeader()
{
	CRichFireDoc* pDoc = (CRichFireDoc*)GetActiveDocument();
	fpSet = pDoc->fpSet;

	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");

	// added for batch
	
	
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(stream, "FireFamily Plus - Fire Analysis Header Info...\n");

	
	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());
	

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch
	fclose(stream);

	//now display the file in a child window
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = 
			theApp.GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("TextFile"))
		{
			CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(fName);
			pDoc->isTemp = true;
			free(fName);
			CString tTitle;
			tTitle.Format("FireFamily Plus Header Information...");
			
			pDoc->SetTitle(tTitle);
			return;
		}
	}
	free(fName);
}*/




// CumulativeFrame.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "CumulativeFrame.h"
#include "CumulativePcnt.h"
#include "CumulativeBars.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT cfindicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_XPROMPT,
	ID_INDICATOR_X,
	ID_INDICATOR_YPROMPT,
	ID_INDICATOR_Y,
//	ID_INDICATOR_CAPS,
};

extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CCumulativeFrame

IMPLEMENT_DYNCREATE(CCumulativeFrame, CMDIChildWnd)

CCumulativeFrame::CCumulativeFrame()
{
}

CCumulativeFrame::~CCumulativeFrame()
{
}


BEGIN_MESSAGE_MAP(CCumulativeFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CCumulativeFrame)
	ON_COMMAND(ID_FILE_PRINT_PRINTGRAPH, OnFilePrintPrintgraph)
	ON_COMMAND(ID_FILE_PRINT1, OnFilePrint1)
	ON_COMMAND(ID_FILE_SAVE_BARGRAPH, OnFileSaveBargraph)
	ON_COMMAND(ID_FILE_SAVE_PERCENTILESGRAPH, OnFileSavePercentilesgraph)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCumulativeFrame message handlers

BOOL CCumulativeFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(cfindicators,
		  sizeof(cfindicators)/sizeof(UINT)))
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
		pContext->m_pNewViewClass, CSize(300, 50), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - a graph view in column 1
	if (!m_wndSplitter.CreateView(0, 1,
		RUNTIME_CLASS(CCumulativePcnt), CSize(0, 0), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}
	return TRUE;//CMDIChildWnd::OnCreateClient(lpcs, pContext);
}

void CCumulativeFrame::MaximizePane(CView * view)
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


void CCumulativeFrame::OnFilePrintPrintgraph() 
{
	// TODO: Add your command handler code here
	((CCumulativePcnt *)m_wndSplitter.GetPane(0, 1))->PrintGraph();
	
}

void CCumulativeFrame::OnFilePrint1() 
{
	// TODO: Add your command handler code here
	((CCumulativeBars *)m_wndSplitter.GetPane(0, 0))->PrintGraph();
	
}

void CCumulativeFrame::OnFileSaveBargraph() 
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
		((CCumulativeBars *)m_wndSplitter.GetPane(0, 0))->DrawToDCRegion(&memhdc, rect);
		char fName[256];
		strcpy(fName, fd.GetPathName());
		SaveBitmapFile(memhdc.m_hDC, picture, fName); 
		SelectObject(memhdc.m_hDC, Old);
		ReleaseDC(whdc);
		memhdc.DeleteDC();
		DeleteObject(picture);
		theApp.SetUserDir(fd.GetPathName());
		theApp.SetGraphicsExt(fd.GetFileExt());
	}
	chdir(theApp.dbDir);//.workDir);
}

void CCumulativeFrame::OnFileSavePercentilesgraph() 
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
		((CCumulativePcnt *)m_wndSplitter.GetPane(0, 0))->DrawToDCRegion(&memhdc, rect);
		char fName[256];
		strcpy(fName, fd.GetPathName());
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

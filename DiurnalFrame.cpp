// DiurnalFrame.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "DiurnalFrame.h"
#include "DiurnalGraphView.h"
#include "DiurnalFormView.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CDiurnalFrame

IMPLEMENT_DYNCREATE(CDiurnalFrame, CMDIChildWnd)

CDiurnalFrame::CDiurnalFrame()
{
}

CDiurnalFrame::~CDiurnalFrame()
{
}


BEGIN_MESSAGE_MAP(CDiurnalFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CDiurnalFrame)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_GRAPH, OnFilePrintGraph)
	ON_COMMAND(ID_FILE_SAVE_GRAPH, OnFileSaveGraph)
	ON_COMMAND(ID_FILE_SAVE_DIURNAL_REPORT, OnFileSaveDiurnalReport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiurnalFrame message handlers
void CDiurnalFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);
	//((CGraphView*)m_wndSplitter.GetPane(0,0))->varID = varID;
	//((CPercentileView*)m_wndSplitter.GetPane(0,1))->varID = varID;

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

BOOL CDiurnalFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to Create Static Splitter\n");
		return FALSE;
	}
	if (!m_wndSplitter.CreateView(0, 0,
		pContext->m_pNewViewClass, CSize(400, 50), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - an input view in column 1
	if (!m_wndSplitter.CreateView(0, 1,
		RUNTIME_CLASS(CDiurnalGraphView), CSize(0, 0), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	return TRUE;
}

void CDiurnalFrame::OnFilePrint() 
{
	//((CDiurnalReportView *)m_wndSplitter.GetPane(0, 0))->PrintTheDamnThing();
	((CDiurnalFormView *)m_wndSplitter.GetPane(0, 0))->PrintTheDamnThing();
}

void CDiurnalFrame::OnFilePrintGraph() 
{
	((CDiurnalGraphView *)m_wndSplitter.GetPane(0, 1))->PrintGraph();
}

void CDiurnalFrame::OnFileSaveGraph() 
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
		((CDiurnalGraphView *)m_wndSplitter.GetPane(0, 1))->DrawToDCRegion(&memhdc, rect);
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

/*void CDiurnalFrame::OnFileSaveAs() 
{
	((CDiurnalFormView *)m_wndSplitter.GetPane(0, 0))->SaveTheDamnThing();
}*/

void CDiurnalFrame::MaximizePane(CView * view)
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


void CDiurnalFrame::OnFileSaveDiurnalReport() 
{
	((CDiurnalFormView *)m_wndSplitter.GetPane(0, 0))->SaveTheDamnThing();
}

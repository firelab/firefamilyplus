// DPFrame.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "DPFrame.h"
#include "DPFormView.h"
#include "DPPcntView.h"
#include "DPProbView.h"
#include "DPReportView.h"
#include "DPDoc.h"
#include "MainFrm.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MINWIDHGT 20

extern CFireplusApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDPFrame

IMPLEMENT_DYNCREATE(CDPFrame, CMDIChildWnd)

CDPFrame::CDPFrame()
{
	dpDialog = NULL;
}

CDPFrame::~CDPFrame()
{
}


BEGIN_MESSAGE_MAP(CDPFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CDPFrame)
	ON_COMMAND(ID_FILE_PRINT_GRAPH_CLASSPERCENTAGES, OnFilePrintGraphClasspercentages)
	ON_COMMAND(ID_FILE_PRINT_GRAPH_PERCENTILES, OnFilePrintGraphPercentiles)
	ON_COMMAND(ID_FILE_PRINT_GRAPH_PROBABILITIES, OnFilePrintGraphProbabilities)
	ON_COMMAND(ID_FILE_SAVE_GRAPH_CLASSPERCENTAGES, OnFileSaveGraphClasspercentages)
	ON_COMMAND(ID_FILE_SAVE_GRAPH_PERCENTILES, OnFileSaveGraphPercentiles)
	ON_COMMAND(ID_FILE_SAVE_GRAPH_PROBABILITIES, OnFileSaveGraphProbabilities)
	ON_COMMAND(ID_TOOLS_CLASSDEFINITIONS, OnToolsClassdefinitions)
	ON_COMMAND(ID_FILE_SAVE_REPORT, OnFileSaveReport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDPFrame message handlers

BOOL CDPFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	//return CMDIChildWnd::OnCreateClient(lpcs, pContext);
	// create a splitter with 1 row, 2 columns
	if (!m_wndSplitter.CreateStatic(this, 2, 1))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}
	//CSize vSize;
	//LPRECT rect;
	//GetClientRect(rect);
	//int wid = (rect->right - rect->left) / 3 * 2;
	// add the first splitter pane - the default view in row 0
	//vSize.cx = lpcs->cx;
	//vSize.cy = lpcs->cy / 3;
	CRect sRect, spRect;
	((CMainFrame *)AfxGetMainWnd())->m_wndStatusBar.GetWindowRect(&sRect);
	m_wndSplitter.GetWindowRect(&spRect);
	if (!m_wndSplitter.CreateView(0, 0,
		pContext->m_pNewViewClass, CSize(lpcs->cx, lpcs->cy / 3), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	if (!m_wndSplitter2.CreateStatic(
		&m_wndSplitter,     // our parent window is the first splitter
		1, 2,               // the new splitter is 1 rows, 2 column
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		m_wndSplitter.IdFromRowCol(1, 0)
			// new splitter is in the 2nd row, 1st column of first splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}
	if (!m_wndSplitter3.CreateStatic(
		&m_wndSplitter2,     // our parent window is the first splitter
		2, 1,               // the new splitter is 2 rows, 1 column
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		m_wndSplitter2.IdFromRowCol(0, 0)
			// new splitter is in the first row, 1st column of 2nd splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}
	if (!m_wndSplitter3.CreateView(0, 0,
		RUNTIME_CLASS(CDPPcntView), CSize(lpcs->cx / 2, lpcs->cy / 3), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}
	if (!m_wndSplitter3.CreateView(1, 0,
		RUNTIME_CLASS(CDPProbView), CSize(lpcs->cx / 2, lpcs->cy / 3), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}
	if (!m_wndSplitter2.CreateView(0, 1,
		RUNTIME_CLASS(CDPFormView), CSize(lpcs->cx / 2, lpcs->cy / 3 * 2), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}
	m_wndSplitter.SetRowInfo(0, lpcs->cy / 3 - sRect.Height() - 2 * spRect.Height(), MINWIDHGT);
	m_wndSplitter2.SetColumnInfo(0, lpcs->cx / 2, MINWIDHGT);
	m_wndSplitter2.SetColumnInfo(1, lpcs->cx / 2, MINWIDHGT);
	m_wndSplitter3.SetColumnInfo(0, lpcs->cx / 2, MINWIDHGT);
	m_wndSplitter3.SetRowInfo(0, lpcs->cy / 3 - sRect.Height() - 2 * spRect.Height(), MINWIDHGT);
	//m_wndSplitter3.SetRowInfo(1, lpcs->cy / 3 - sRect.Height() - 2 * spRect.Height(), MINWIDHGT);
	m_wndSplitter.RecalcLayout();
	m_wndSplitter2.RecalcLayout();
	m_wndSplitter3.RecalcLayout();

	dpDialog = new CDPDialog(this, pContext->m_pCurrentDoc);
	//dpDialog->Create();
	return TRUE;
}


BOOL CDPFrame::DestroyWindow() 
{
	if(dpDialog)
	{
		dpDialog->DestroyWindow();
		dpDialog = NULL;
	}
	return CMDIChildWnd::DestroyWindow();
}

void CDPFrame::OnFilePrintGraphClasspercentages() 
{
	((CDPFormView *)m_wndSplitter2.GetPane(0, 1))->PrintGraph();
}

void CDPFrame::OnFilePrintGraphPercentiles() 
{
	((CDPPcntView *)m_wndSplitter3.GetPane(0, 0))->PrintGraph();
}

void CDPFrame::OnFilePrintGraphProbabilities() 
{
	((CDPProbView *)m_wndSplitter3.GetPane(1, 0))->PrintGraph();
}

void CDPFrame::OnFileSaveGraphClasspercentages() 
{
	chdir(theApp.userDir);
	CFileDialog fd(false, theApp.GetGraphicsExt(), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		theApp.GetGraphicsFileStr(), this);
	if(fd.DoModal() == IDOK)
	{
		CDC *whdc = GetDC();
		CDC memhdc;
		memhdc.CreateCompatibleDC(NULL);
		HBITMAP picture = CreateCompatibleBitmap(whdc->m_hDC, 480, 640);
		HGDIOBJ Old = SelectObject(memhdc.m_hDC, picture);
		//CRect bnd(0, 0, pWidth.GetValueInteger(), pHeight.GetValueInteger());
		CRect rect(0, 0, 480, 640);
		((CDPFormView *)m_wndSplitter2.GetPane(0, 1))->DrawToDCRegion(&memhdc, rect);
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

void CDPFrame::OnFileSaveGraphPercentiles() 
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
		((CDPPcntView *)m_wndSplitter3.GetPane(0, 0))->DrawToDCRegion(&memhdc, rect);
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

void CDPFrame::OnFileSaveGraphProbabilities() 
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
		((CDPProbView *)m_wndSplitter3.GetPane(1, 0))->DrawToDCRegion(&memhdc, rect);
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

void CDPFrame::OnToolsClassdefinitions() 
{
	if(!dpDialog)
	{
		dpDialog  = new CDPDialog(this, GetActiveDocument());
		//dpDialog->Create();
	}
	else
	{
		dpDialog->ShowWindow(SW_SHOWNORMAL);
		dpDialog->SetActiveWindow();
	}
}

void CDPFrame::OnFileSaveReport() 
{
	// save the report as text file
	chdir(theApp.userDir);
	CFileDialog fd(false, "txt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||", this);
	if(fd.DoModal() == IDOK)
	{
		((CDPReportView *)m_wndSplitter.GetPane(0, 0))->SaveReport(fd.GetPathName());
		theApp.SetUserDir(fd.GetPathName());
	}
	chdir(theApp.dbDir);//theApp.workDir);
}

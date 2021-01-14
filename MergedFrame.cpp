// MergedFrame.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "MergedFrame.h"
//#include "PeriodDoc.h"
#include "MergedView.h"
#include "wxset.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "MergeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CMergedFrame

IMPLEMENT_DYNCREATE(CMergedFrame, CMDIChildWnd)

CMergedFrame::CMergedFrame()
{
	vars[0] = vars[1] = vars[2] = vars[3] = -1;
//	varNames[0] = varNames[1] = varNames[2] = varNames[3] = "";
//	colors[0] = theApp.mergeColor1;
//	colors[1] = theApp.mergeColor2;
//	colors[2] = theApp.mergeColor3;
//	colors[3] = theApp.mergeColor4;
//	styles[0] = LineStyleToString(theApp.mergeStyle1);
//	styles[1] = LineStyleToString(theApp.mergeStyle2);
//	styles[2] = LineStyleToString(theApp.mergeStyle3);
///	styles[3] = LineStyleToString(theApp.mergeStyle4);
//	widths[0] = theApp.mergeWidth1;
//	widths[1] = theApp.mergeWidth2;
//	widths[2] = theApp.mergeWidth3;
//	widths[3] = theApp.mergeWidth4;
}

CMergedFrame::~CMergedFrame()
{
}


BEGIN_MESSAGE_MAP(CMergedFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CMergedFrame)
	ON_COMMAND(ID_OPTIONS_DISPLAYOPTONS, OnOptionsDisplayoptons)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_DISPLAYOPTONS, OnUpdateOptionsDisplayoptons)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergedFrame message handlers

void CMergedFrame::OnOptionsDisplayoptons() 
{
	//pop up a merge dialog
	CMergeDialog md(this);
	md.pDoc = (CPeriodDoc *)GetActiveDocument();
	md.DoModal();
	
}

void CMergedFrame::OnUpdateOptionsDisplayoptons(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	
}

void CMergedFrame::ReconfigureView()
{
	((CMergedView *)GetActiveView())->BuildData();
	((CMergedView *)GetActiveView())->Invalidate();
}

void CMergedFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

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

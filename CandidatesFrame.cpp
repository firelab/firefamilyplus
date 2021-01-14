// CandidatesFrame.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "CandidatesFrame.h"


// CCandidatesFrame

IMPLEMENT_DYNCREATE(CCandidatesFrame, CMDIChildWnd)

CCandidatesFrame::CCandidatesFrame()
{
	m_pDB = NULL;
}

CCandidatesFrame::~CCandidatesFrame()
{
}


BEGIN_MESSAGE_MAP(CCandidatesFrame, CMDIChildWnd)
END_MESSAGE_MAP()


// CCandidatesFrame message handlers

BOOL CCandidatesFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
      | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_WEATHERVIEW_TMPL1))
   {
      TRACE0("Failed to create toolbar\n");
      return -1;      // fail to create
   }

   m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
   EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_wndToolBar);
	if (!m_candidatesBar.Create(this, IDD_DIALOGBAR_CANDIDATES, CBRS_TOP | CBRS_GRIPPER , IDD_DIALOGBAR_CANDIDATES))
	{
		TRACE0("Failed to create dialog bar\n");
		return -1;      // fail to create
	}
	m_candidatesBar.m_pDoc = (CCandidatesDoc *)pContext->m_pCurrentDoc;
   m_candidatesBar.EnableDocking(CBRS_ALIGN_TOP);
   EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_candidatesBar);
   m_candidatesBar.UpdateData(FALSE);

	return CMDIChildWnd::OnCreateClient(lpcs, pContext);
}

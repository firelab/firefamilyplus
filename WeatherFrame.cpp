// WeatherFrame.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "WeatherFrame.h"


// CWeatherFrame

IMPLEMENT_DYNCREATE(CWeatherFrame, CMDIChildWnd)

CWeatherFrame::CWeatherFrame()
{

}

CWeatherFrame::~CWeatherFrame()
{
}


BEGIN_MESSAGE_MAP(CWeatherFrame, CMDIChildWnd)
END_MESSAGE_MAP()


// CWeatherFrame message handlers

BOOL CWeatherFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
 
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
      | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_WEATHERVIEW_TMPL))
   {
      TRACE0("Failed to create toolbar\n");
      return -1;      // fail to create
   }
  
   m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
   EnableDocking(CBRS_ALIGN_ANY);
   
   DockControlBar(&m_wndToolBar);

	return CMDIChildWnd::OnCreateClient(lpcs, pContext);
}

#pragma once


// CWeatherFrame frame

class CWeatherFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CWeatherFrame)
protected:
	CWeatherFrame();           // protected constructor used by dynamic creation
	virtual ~CWeatherFrame();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	CToolBar    m_wndToolBar;
};



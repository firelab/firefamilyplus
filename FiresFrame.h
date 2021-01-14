#pragma once


// CFiresFrame frame

class CFiresFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CFiresFrame)
protected:
	CFiresFrame();           // protected constructor used by dynamic creation
	virtual ~CFiresFrame();

protected:
	DECLARE_MESSAGE_MAP()
	CToolBar    m_wndToolBar;
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};



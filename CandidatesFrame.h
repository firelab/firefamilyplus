#pragma once

#include "CandidatesBar.h"

// CCandidatesFrame frame

class CCandidatesFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CCandidatesFrame)
protected:
	CCandidatesFrame();           // protected constructor used by dynamic creation
	virtual ~CCandidatesFrame();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	CToolBar    m_wndToolBar;
public:
	CCandidatesBar    m_candidatesBar;
	CDatabase *m_pDB;
};



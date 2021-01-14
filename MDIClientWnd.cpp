// MDIClientWnd.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "MDIClientWnd.h"
#include ".\mdiclientwnd.h"


// CMDIClientWnd

IMPLEMENT_DYNAMIC(CMDIClientWnd, CWnd)
CMDIClientWnd::CMDIClientWnd()
{
	//m_bkImage.Load("ffp5.bmp");
}

CMDIClientWnd::~CMDIClientWnd()
{
}


BEGIN_MESSAGE_MAP(CMDIClientWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CMDIClientWnd message handlers


BOOL CMDIClientWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect tRect;
	GetClientRect(&tRect);
	if(!m_bkImage.IsNull())
	{
		m_bkImage.StretchBlt(pDC->GetSafeHdc(), 0, 0, tRect.Width(), tRect.Height());
		int oldStretch = SetStretchBltMode(pDC->GetSafeHdc(), HALFTONE);
		m_bkImage.StretchBlt(pDC->GetSafeHdc(), 0, 0, tRect.Width(), tRect.Height());
		SetStretchBltMode(pDC->GetSafeHdc(), oldStretch);
	}
	else
		CWnd::OnEraseBkgnd(pDC);
	return true;
}

void CMDIClientWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	Invalidate();
}

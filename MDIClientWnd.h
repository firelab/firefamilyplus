#pragma once
#include <atlimage.h>


// CMDIClientWnd

class CMDIClientWnd : public CWnd
{
	DECLARE_DYNAMIC(CMDIClientWnd)

public:
	CMDIClientWnd();
	virtual ~CMDIClientWnd();

protected:
	DECLARE_MESSAGE_MAP()
	CImage m_bkImage;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};



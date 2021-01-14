// RichFrame.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "EventFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRichFrame

IMPLEMENT_DYNCREATE(CEventFrame, CMDIChildWnd)

CEventFrame::CEventFrame()
{
}

CEventFrame::~CEventFrame()
{
}


BEGIN_MESSAGE_MAP(CEventFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CRichFrame)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventFrame message handlers

/*void CEventFrame::OnFilePrint() 
{
	// TODO: Add your command handler code here
	
}*/

void CEventFrame::OnFilePrint() 
{
	// TODO: Add your command handler code here
	
}

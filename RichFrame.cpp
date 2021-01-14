// RichFrame.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "RichFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRichFrame

IMPLEMENT_DYNCREATE(CRichFrame, CMDIChildWnd)

CRichFrame::CRichFrame()
{
}

CRichFrame::~CRichFrame()
{
}


BEGIN_MESSAGE_MAP(CRichFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CRichFrame)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichFrame message handlers

/*void CRichFrame::OnFilePrint() 
{
	// TODO: Add your command handler code here
	
}*/

void CRichFrame::OnFilePrint() 
{
	// TODO: Add your command handler code here
	
}

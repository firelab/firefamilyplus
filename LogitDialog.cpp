// LogitDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "LogitDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogitDialog dialog


CLogitDialog::CLogitDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLogitDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogitDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLogitDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogitDialog)
	DDX_Control(pDX, IDC_EDIT1, messageString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogitDialog, CDialog)
	//{{AFX_MSG_MAP(CLogitDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogitDialog message handlers

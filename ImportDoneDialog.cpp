// ImportDoneDialog.cpp : implementation file
//

#include "stdafx.h"
#include <afxrich.h>
#include "richdoc.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "ImportDoneDialog.h"
#include "RichDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CImportDoneDialog dialog


CImportDoneDialog::CImportDoneDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CImportDoneDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportDoneDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	statusStr = "";
	logFileName = "";
	killLog = true;
}


void CImportDoneDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportDoneDialog)
	DDX_Control(pDX, IDC_TEXTAREA, textArea);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportDoneDialog, CDialog)
	//{{AFX_MSG_MAP(CImportDoneDialog)
	ON_BN_CLICKED(IDC_VIEWLOGBUTTON, OnViewlogbutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportDoneDialog message handlers

void CImportDoneDialog::OnViewlogbutton() 
{
	if(logFileName.GetLength() > 0)
	{
		CWaitCursor wait;
		//now display the file in a dialog window
		CRichDialog rd(this, logFileName);
		//rd.SetWindowText("Import Error Log");
		rd.DoModal();
	}
}

BOOL CImportDoneDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	textArea.SetWindowText(statusStr);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImportDoneDialog::OnOK() 
{
	// TODO: Add extra validation here
	if(logFileName.GetLength() > 0 && killLog)
		unlink(logFileName);
	CDialog::OnOK();
}

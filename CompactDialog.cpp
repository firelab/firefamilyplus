// CompactDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "CompactDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompactDialog dialog


CCompactDialog::CCompactDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCompactDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCompactDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//doDupCheck = FALSE;
}


void CCompactDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompactDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompactDialog, CDialog)
	//{{AFX_MSG_MAP(CCompactDialog)
	//ON_BN_CLICKED(IDC_DUPCHECK, OnDupcheck)
	//ON_BN_CLICKED(IDC_NODUPCHECK, OnNodupcheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompactDialog message handlers

BOOL CCompactDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//CButton * button = (CButton*)GetDlgItem(IDC_NODUPCHECK);
	//if(button)
	//	button->SetCheck(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/*void CCompactDialog::OnDupcheck() 
{
	CButton * button = (CButton*)GetDlgItem(IDC_DUPCHECK);
	if(button && button->GetCheck() == TRUE)
		doDupCheck = TRUE;
	else
		doDupCheck = FALSE;
}

void CCompactDialog::OnNodupcheck() 
{
	CButton * button = (CButton*)GetDlgItem(IDC_NODUPCHECK);
	if(button && button->GetCheck() == TRUE)
		doDupCheck = FALSE;
	else
		doDupCheck = TRUE;
}*/

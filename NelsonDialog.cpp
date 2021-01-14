// NelsonDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "SIGStationset.h"
#include "NelsonDialog.h"
#include "wxset.h"
#include "StationInSIGSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNelsonDialog dialog


CNelsonDialog::CNelsonDialog()
	: CDialog(CNelsonDialog::IDD, AfxGetMainWnd())
{
	//{{AFX_DATA_INIT(CNelsonDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	start1h = 20;
	start10h = 20;
	start100h = 20;
	start1000h = 20;
}


void CNelsonDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNelsonDialog)
    DDX_Text(pDX, IDC_1H, start1h);
	DDX_Text(pDX, IDC_10H, start10h);
    DDX_Text(pDX, IDC_100H, start100h);
	DDX_Text(pDX, IDC_1000H, start1000h);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNelsonDialog, CDialog)
	//{{AFX_MSG_MAP(CNelsonDialog)
	ON_BN_CLICKED(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNelsonDialog message handlers

BOOL CNelsonDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CNelsonDialog::OnOK()
{
   	
	CDialog::OnOK();

}

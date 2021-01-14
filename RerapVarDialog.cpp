// RerapVarDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "RerapVarDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRerapVarDialog dialog


CRerapVarDialog::CRerapVarDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRerapVarDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRerapVarDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRerapVarDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRerapVarDialog)
	DDX_Control(pDX, IDC_COMBO1, vars);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRerapVarDialog, CDialog)
	//{{AFX_MSG_MAP(CRerapVarDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRerapVarDialog message handlers

BOOL CRerapVarDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	vars.ResetContent();
	int loc;
	loc = vars.AddString("Ignition Component");
	vars.SetItemData(loc, 16);
	if(theApp.rerapVarID == 16)
		vars.SetCurSel(loc);
	loc = vars.AddString("Spread Component");
	vars.SetItemData(loc, 12);
	if(theApp.rerapVarID == 12)
		vars.SetCurSel(loc);
	loc = vars.AddString("Energy Release Component");
	vars.SetItemData(loc, 13);
	if(theApp.rerapVarID == 13)
		vars.SetCurSel(loc);
	loc = vars.AddString("Burning Index");
	vars.SetItemData(loc, 14);
	if(theApp.rerapVarID == 14)
		vars.SetCurSel(loc);
	loc = vars.AddString("Keetch-Byram Drought Index");
	vars.SetItemData(loc, 15);
	if(theApp.rerapVarID == 15)
		vars.SetCurSel(loc);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRerapVarDialog::OnOK() 
{
	// TODO: Add extra validation here
	int loc = vars.GetCurSel();
	if(loc >= 0)
	{
		varID = vars.GetItemData(loc);
		theApp.rerapVarID = varID;
	}
	else
	{
		AfxMessageBox("A variable type must be selected!");
		return;
	}
	CDialog::OnOK();
}

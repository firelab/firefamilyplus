// NoWxDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "NoWxDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNoWxDialog dialog


CNoWxDialog::CNoWxDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNoWxDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNoWxDialog)
	fireName = _T("");
	fireDate = _T("");
	fireVal = 0.0;
	//}}AFX_DATA_INIT
	ignore = true;
}


void CNoWxDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNoWxDialog)
	DDX_Control(pDX, IDC_FIREVALUE, fireValStr);
	DDX_Control(pDX, IDC_RADIO1, ignoreButton);
	DDX_Text(pDX, IDC_EDIT2, fireName);
	DDX_Text(pDX, IDC_EDIT3, fireDate);
	DDX_Text(pDX, IDC_FIREVALUE, fireVal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNoWxDialog, CDialog)
	//{{AFX_MSG_MAP(CNoWxDialog)
	ON_BN_CLICKED(IDC_RADIO2, OnAssignVal)
	ON_BN_CLICKED(IDC_RADIO1, OnIgnore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNoWxDialog message handlers

BOOL CNoWxDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ignoreButton.SetCheck(TRUE);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNoWxDialog::OnAssignVal() 
{
	// TODO: Add your control notification handler code here
	fireValStr.EnableWindow(TRUE);
}

void CNoWxDialog::OnIgnore() 
{
	// TODO: Add your control notification handler code here
	fireValStr.EnableWindow(FALSE);
}

void CNoWxDialog::OnOK() 
{
	// TODO: Add extra validation here
	if(ignoreButton.GetCheck() == FALSE)
	{//make sure value is set
		CString str;
		fireValStr.GetWindowText(str);
		if(str.GetLength() == 0 || atoi(str) <= 0)
		{
			AfxMessageBox("ERROR: Value entered must be greater than zero.");
			return;
		}
		ignore = false;
	}
	else
		ignore = true;
	CDialog::OnOK();
}

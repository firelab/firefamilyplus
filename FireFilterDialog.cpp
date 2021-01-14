// FireFilterDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "FireFilterDialog.h"
#include ".\firefilterdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFireFilterDialog dialog


CFireFilterDialog::CFireFilterDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFireFilterDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFireFilterDialog)
	type11 = TRUE;
	type12 = type13 = type14 = type21 = type22 = type23 = type48 = type49 = type15 
		= type16 = type19 = type25 = type26 = type37 = FALSE;
	//}}AFX_DATA_INIT
}


void CFireFilterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFireFilterDialog)
	DDX_Check(pDX, IDC_TYPE_11, type11);
	DDX_Check(pDX, IDC_TYPE_12, type12);
	DDX_Check(pDX, IDC_TYPE_13, type13);
	DDX_Check(pDX, IDC_TYPE_14, type14);
	DDX_Check(pDX, IDC_TYPE_21, type21);
	DDX_Check(pDX, IDC_TYPE_22, type22);
	DDX_Check(pDX, IDC_TYPE_23, type23);
	DDX_Check(pDX, IDC_TYPE_48, type48);
	DDX_Check(pDX, IDC_TYPE_49, type49);
	DDX_Check(pDX, IDC_TYPE_15, type15);
	DDX_Check(pDX, IDC_TYPE_16, type16);
	DDX_Check(pDX, IDC_TYPE_19, type19);
	DDX_Check(pDX, IDC_TYPE_25, type25);
	DDX_Check(pDX, IDC_TYPE_26, type26);
	DDX_Check(pDX, IDC_TYPE_37, type37);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFireFilterDialog, CDialog)
	//{{AFX_MSG_MAP(CFireFilterDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ALL, OnBnClickedButtonSelectAll)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_ALL, OnBnClickedButtonClearAll)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireFilterDialog message handlers

BOOL CFireFilterDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	type11 = TRUE;	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFireFilterDialog::OnOK() 
{
	UpdateData();	
	CDialog::OnOK();
}

void CFireFilterDialog::OnBnClickedButtonSelectAll()
{
	type11 = type12 = type13 = type14 = type21 = type22 = type23 = type48 = type49 = type15 
		= type16 = type19 = type25 = type26 = type37 = TRUE;
	UpdateData(FALSE);
}

void CFireFilterDialog::OnBnClickedButtonClearAll()
{
	type11 = type12 = type13 = type14 = type21 = type22 = type23 = type48 = type49 = type15 
		= type16 = type19 = type25 = type26 = type37 = FALSE;
	UpdateData(FALSE);
}

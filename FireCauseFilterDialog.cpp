// FireCauseFilterDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "FireCauseFilterDialog.h"
#include ".\FireCauseFilterdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFireCauseFilterDialog dialog


CFireCauseFilterDialog::CFireCauseFilterDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFireCauseFilterDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFireCauseFilterDialog)

	// fix negative value
	if (causeFlag < 0)
		causeFlag = 1024;
	cause2 = (causeFlag >> 1) & 1;
	cause3 = (causeFlag >> 2) & 1;
	cause4 = (causeFlag >> 3) & 1;
	cause5 = (causeFlag >> 4) & 1;
	cause6 = (causeFlag >> 5) & 1;
	cause7 = (causeFlag >> 6) & 1;
	cause8 = (causeFlag >> 7) & 1;
	cause9 = (causeFlag >> 8) & 1;
	cause10 = (causeFlag >> 9) & 1;


	//}}AFX_DATA_INIT
}


void CFireCauseFilterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFireCauseFilterDialog)
	//DDX_Check(pDX, IDC_CAUSE_1, cause1);  // lightning
	DDX_Check(pDX, IDC_CAUSE_2, cause2);
	DDX_Check(pDX, IDC_CAUSE_3, cause3);
	DDX_Check(pDX, IDC_CAUSE_4, cause4);
	DDX_Check(pDX, IDC_CAUSE_5, cause5);
	DDX_Check(pDX, IDC_CAUSE_6, cause6);
	DDX_Check(pDX, IDC_CAUSE_7, cause7);
	DDX_Check(pDX, IDC_CAUSE_8, cause8);
	DDX_Check(pDX, IDC_CAUSE_9, cause9);
	DDX_Check(pDX, IDC_CAUSE_10, cause10);

	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFireCauseFilterDialog, CDialog)
	//{{AFX_MSG_MAP(CFireCauseFilterDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ALL, OnBnClickedButtonSelectAll)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_ALL, OnBnClickedButtonClearAll)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireCauseFilterDialog message handlers

BOOL CFireCauseFilterDialog::OnInitDialog() 
{
	if (causeFlag < 0)
		causeFlag = 1024;

	cause2 = (causeFlag >> 1) & 1;
	cause3 = (causeFlag >> 2) & 1;
	cause4 = (causeFlag >> 3) & 1;
	cause5 = (causeFlag >> 4) & 1;
	cause6 = (causeFlag >> 5) & 1;
	cause7 = (causeFlag >> 6) & 1;
	cause8 = (causeFlag >> 7) & 1;
	cause9 = (causeFlag >> 8) & 1;
	cause10 = (causeFlag >> 9) & 1;


	CDialog::OnInitDialog();
	// convert flag to 10 boolean switches

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFireCauseFilterDialog::OnOK() 
{
	UpdateData();	
	CDialog::OnOK();
	causeFlag = 0 | cause2<<1 | cause3<<2 | cause4<<3 | cause5<<4 | cause6<<5 | cause7<<6 | cause8<<7 | cause9<<8 | cause10<<9;
}

void CFireCauseFilterDialog::OnBnClickedButtonSelectAll()
{
	cause1 = cause2 = cause3 = cause4 = cause5 = cause6 = cause7 = cause8 = cause9 = cause10 = TRUE;
	UpdateData(FALSE);
}

void CFireCauseFilterDialog::OnBnClickedButtonClearAll()
{
	cause1 = cause2 = cause3 = cause4 = cause5 = cause6 = cause7 = cause8 = cause9 = cause10 = FALSE;
	UpdateData(FALSE);
}

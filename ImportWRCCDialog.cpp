// ImportWRCCDialog.cpp : implementation file
//
//
#include "stdafx.h"

#include "fireplus.h"
#include "fireplusset.h"
#include "ImportWRCCDialog.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportWRCCDialog dialog

CImportWRCCDialog::CImportWRCCDialog(CWnd* pParent /*=NULL*/,   CString _message, CString _wimsStation, CString _wrccStation)
	: CDialog(CImportWRCCDialog::IDD, pParent)
{

	message = _message;
	wimsStation = _wimsStation;
	wrccStation = _wrccStation;
	
	

	importWRCC = 0;
	

	
}


void CImportWRCCDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportWRCCDialog)
	// NOTE: the ClassWizard will add DDX and DDV calls here


	DDX_Text(pDX, IDC_STATION_WIMS, wimsStation);
	DDX_Text(pDX, IDC_STATION_WRCC, wrccStation);
	DDX_Text(pDX, IDC_WRCC_MESSAGE, message);
}


BEGIN_MESSAGE_MAP(CImportWRCCDialog, CDialog)
	//{{AFX_MSG_MAP(CImportWRCCDialog)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportWRCCDialog message handlers

BOOL CImportWRCCDialog::OnInitDialog() 
{
	
	
	CDialog::OnInitDialog();
 
	buttonWIMS = (CButton *)GetDlgItem(IDC_STATION_WIMS);
	buttonWRCC = (CButton *)GetDlgItem(IDC_STATION_WRCC);

	messageBox = (CStatic *)GetDlgItem(IDC_WRCC_MESSAGE);
	
	
		
	
	ConfigureControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CImportWRCCDialog::OnOK(){
	importWRCC = buttonWRCC->GetCheck();
	
	CDialog::OnOK();
}



void CImportWRCCDialog::ConfigureControls()
{
	CString tempstr;

	//CButton *disabled = (CButton *)GetDlgItem(IDC_SCHEME_DISABLED);
	buttonWIMS = (CButton *)GetDlgItem(IDC_STATION_WIMS);
	buttonWRCC = (CButton *)GetDlgItem(IDC_STATION_WRCC);
	messageBox = (CStatic *)GetDlgItem(IDC_WRCC_MESSAGE);

	buttonWIMS->SetCheck(TRUE);
	buttonWIMS->SetWindowText(wimsStation);


	buttonWRCC->SetCheck(FALSE);
	buttonWRCC->SetWindowText(wrccStation);

	messageBox->SetWindowText(message);

	

	
	   
}

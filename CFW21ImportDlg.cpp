// CFW21ImportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "afxdialogex.h"
#include "CFW21ImportDlg.h"


// CFW21ImportDlg dialog

IMPLEMENT_DYNAMIC(CFW21ImportDlg, CDialogEx)

CFW21ImportDlg::CFW21ImportDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FW21_IMPORT, pParent)
{

}

CFW21ImportDlg::~CFW21ImportDlg()
{
}

void CFW21ImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_STATION, m_comboStation);
	DDX_Control(pDX, IDC_EDIT_FW21_FILE, m_editFW21File);
}


BEGIN_MESSAGE_MAP(CFW21ImportDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_FW21_FILE, &CFW21ImportDlg::OnBnClickedButtonFw21File)
	ON_BN_CLICKED(ID_IMPORT_FW21, &CFW21ImportDlg::OnBnClickedImportFw21)
END_MESSAGE_MAP()


// CFW21ImportDlg message handlers


void CFW21ImportDlg::OnBnClickedButtonFw21File()
{
	// TODO: Add your control notification handler code here
}


void CFW21ImportDlg::OnBnClickedImportFw21()
{
	// TODO: Add your control notification handler code here
}

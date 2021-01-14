// SowWetFlagOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "SowWetFlagOptionsDlg.h"


// CSowWetFlagOptionsDlg dialog

IMPLEMENT_DYNAMIC(CSowWetFlagOptionsDlg, CDialog)

CSowWetFlagOptionsDlg::CSowWetFlagOptionsDlg(CWnd* pParent /*=NULL*/, CSowOptionsSet *pSet /*= NULL*/)
	: CDialog(CSowWetFlagOptionsDlg::IDD, pParent)
{
	m_pSet = pSet;
}

CSowWetFlagOptionsDlg::~CSowWetFlagOptionsDlg()
{
}

void CSowWetFlagOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSowWetFlagOptionsDlg, CDialog)
END_MESSAGE_MAP()


// CSowWetFlagOptionsDlg message handlers

BOOL CSowWetFlagOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	int sowUse = m_pSet->m_SOW_Usage, wfUse = m_pSet->m_WetFlag_Usage;
	switch(sowUse)
	{
	case CSowOptionsSet::SR_SOW_ONLY:
		((CButton *)GetDlgItem(IDC_RADIO_ONLY_SR_SOW))->SetCheck(TRUE);
		break;
	case CSowOptionsSet::SR_SOW_NEVER:
		((CButton *)GetDlgItem(IDC_RADIO_NEVER_SR_SOW))->SetCheck(TRUE);
		break;
	default:
		((CButton *)GetDlgItem(IDC_RADIO_USE_WHEN_SOW_NULL))->SetCheck(TRUE);
	}

	switch(wfUse)
	{
	case CSowOptionsSet::EST_WF_ONLY:
		((CButton *)GetDlgItem(IDC_RADIO_ONLY_EST_WET_FLAG))->SetCheck(TRUE);
		break;
	case CSowOptionsSet::EST_WF_NEVER:
		((CButton *)GetDlgItem(IDC_RADIO_NEVER_EST_WET_FLAG))->SetCheck(TRUE);
		break;
	default:
		((CButton *)GetDlgItem(IDC_RADIO_USE_WHEN_WETFLAG_NULL))->SetCheck(TRUE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSowWetFlagOptionsDlg::OnOK()
{
	m_pSet->Edit();
	if(((CButton *)GetDlgItem(IDC_RADIO_NEVER_SR_SOW))->GetCheck())
		m_pSet->m_SOW_Usage = CSowOptionsSet::SR_SOW_NEVER;//2;
	else if(((CButton *)GetDlgItem(IDC_RADIO_ONLY_SR_SOW))->GetCheck())
		m_pSet->m_SOW_Usage = CSowOptionsSet::SR_SOW_ONLY;//1;
	else
		m_pSet->m_SOW_Usage = CSowOptionsSet::SOW_MISSING;//0;

	if(((CButton *)GetDlgItem(IDC_RADIO_NEVER_EST_WET_FLAG))->GetCheck())
		m_pSet->m_WetFlag_Usage = CSowOptionsSet::EST_WF_NEVER;//2;
	else if(((CButton *)GetDlgItem(IDC_RADIO_ONLY_EST_WET_FLAG))->GetCheck())
		m_pSet->m_WetFlag_Usage = CSowOptionsSet::EST_WF_ONLY;//1;
	else
		m_pSet->m_WetFlag_Usage = CSowOptionsSet::WF_MISSING;//0;

	m_pSet->Update();
	CDialog::OnOK();
}

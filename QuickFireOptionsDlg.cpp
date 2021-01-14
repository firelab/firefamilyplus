// QuickFireOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "QuickFireOptionsDlg.h"


// CQuickFireOptionsDlg dialog

IMPLEMENT_DYNAMIC(CQuickFireOptionsDlg, CDialog)
CQuickFireOptionsDlg::CQuickFireOptionsDlg(CWnd* pParent, CFireplusSet *_fpSet)
	: CDialog(CQuickFireOptionsDlg::IDD, pParent), optionsSet(_fpSet->m_pDatabase)
{
	conditional = TRUE;
	fpSet = _fpSet;
	optionsSet.m_strFilter.Format("[SIG/Station] = '%s'", fpSet->m_SIG_Station);
	optionsSet.Open();
	if(optionsSet.IsEOF() || optionsSet.IsBOF())
	{//no record for SIG/Station, add one
		optionsSet.AddNew();
		optionsSet.m_SIG_Station = fpSet->m_SIG_Station;
		optionsSet.m_All = true;
		optionsSet.m_Lightning = false;
		optionsSet.m_Human = false;
		optionsSet.m_Both = true;
		optionsSet.m_Cumulative = false;
		optionsSet.m_Probability = false;
		optionsSet.m_LFAcres = 10;
		optionsSet.m_MFDFires = 5;
		optionsSet.m_VarID = 1;
		optionsSet.m_Conditional = TRUE;
		optionsSet.Update();
		optionsSet.Requery();
	}
}

CQuickFireOptionsDlg::~CQuickFireOptionsDlg()
{
}

void CQuickFireOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_RADIO5, cumulative);
	DDX_Control(pDX, IDC_RADIO2, lightning);
	DDX_Check(pDX, IDC_CONDITIONAL, conditional);
	DDX_Control(pDX, IDC_SPINFIRES, m_spinFires);
	DDX_Control(pDX, IDC_SPINACRES, m_spinAcres);
}


BEGIN_MESSAGE_MAP(CQuickFireOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_CONDITIONAL, OnConditional)
END_MESSAGE_MAP()


// CQuickFireOptionsDlg message handlers
BOOL CQuickFireOptionsDlg::OnInitDialog() 
{
	if(optionsSet.m_Conditional == TRUE)
		conditional = TRUE;
	else
		conditional = FALSE;
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	if(!fpSet)
		return TRUE;
	m_spinAcres.SetRange(1, 30000);
	m_spinFires.SetRange(1, 30000);
	m_spinAcres.SetPos(optionsSet.m_LFAcres);
	m_spinFires.SetPos(optionsSet.m_MFDFires);
	//if(optionsSet.m_Cumulative)
	//	cumulative.SetCheck(true);
	//else if(optionsSet.m_Probability)
	//{
	//	CButton *button = (CButton *)GetDlgItem(IDC_RADIO6);
	//	button->SetCheck(true);
	//}
	//else //both
	//{
	//	CButton *button = (CButton *)GetDlgItem(IDC_RADIO7);
	//	button->SetCheck(true);
	//}
	if(optionsSet.m_Lightning)
		lightning.SetCheck(true);
	else if(optionsSet.m_Human)
	{
		CButton *button = (CButton *)GetDlgItem(IDC_RADIO3);
		button->SetCheck(true);
	}
	else //all causes
	{
		CButton *button = (CButton *)GetDlgItem(IDC_RADIO4);
		button->SetCheck(true);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CQuickFireOptionsDlg::OnConditional() 
{
	UpdateData();
}

void CQuickFireOptionsDlg::OnOK() 
{
	// TODO: Add extra validation here
	CWaitCursor wait;
	UpdateData();
	optionsSet.Edit();
	optionsSet.m_VarID = 1;
	optionsSet.m_LFAcres = m_spinAcres.GetPos();//lfAcres.GetValueInteger();
	optionsSet.m_MFDFires = m_spinFires.GetPos();//mfdFires.GetValueInteger();
//	if(cumulative.GetCheck())
//	{
//		optionsSet.m_Cumulative = true;
//		optionsSet.m_Probability = false;
//		optionsSet.m_Both = false;
		//cumu = true;
	//}
//	else if(((CButton *)GetDlgItem(IDC_RADIO6))->GetCheck())
//	{
//		optionsSet.m_Cumulative = false;
//		optionsSet.m_Probability = true;
//		optionsSet.m_Both = false;
		//regress = true;
	//}
	//else
	//{
		optionsSet.m_Cumulative = false;
		optionsSet.m_Probability = false;
		optionsSet.m_Both = true;
		//cumu = true;
		//regress = true;
	//}
	if(lightning.GetCheck())
	{
		optionsSet.m_Lightning = true;
		optionsSet.m_Human = false;
		optionsSet.m_All = false;
	}
	else if(((CButton *)GetDlgItem(IDC_RADIO3))->GetCheck())
	{
		optionsSet.m_Lightning = false;
		optionsSet.m_Human = true;
		optionsSet.m_All = false;
	}
	else
	{
		optionsSet.m_Lightning = false;
		optionsSet.m_Human = false;
		optionsSet.m_All = true;
	}
	optionsSet.m_Conditional = conditional;
	optionsSet.Update();
	CDialog::OnOK();

}
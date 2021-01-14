// SowThresholdsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "SowThresholdsDlg.h"


// CSowThresholdsDlg dialog

IMPLEMENT_DYNAMIC(CSowThresholdsDlg, CDialog)

CSowThresholdsDlg::CSowThresholdsDlg(CWnd* pParent /*=NULL*/, CSowThresholdsSet *_pSowSet/* = NULL*/)
	: CDialog(CSowThresholdsDlg::IDD, pParent)
{
	m_pSowSet = _pSowSet;
}

CSowThresholdsDlg::~CSowThresholdsDlg()
{
}

void CSowThresholdsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PCNT_CLEAR, m_editPcntClear);
	DDX_Control(pDX, IDC_SPIN_PCNT_CLEAR, m_spinPcntClear);
	DDX_Control(pDX, IDC_EDIT_PCNT_SCATTERED, m_editPcntScattered);
	DDX_Control(pDX, IDC_SPIN_PCNT_SCATTERED, m_spinPcntScattered);
	DDX_Control(pDX, IDC_EDIT_PCNT_BROKEN, m_editPcntBroken);
	DDX_Control(pDX, IDC_SPIN_PCNT_BROKEN, m_spinPcntBroken);
	DDX_Control(pDX, IDC_EDIT_3HR_DUR, m_edit3HrDur);
	DDX_Control(pDX, IDC_SPIN_3HR_DUR, m_spin3HrDur);
	DDX_Control(pDX, IDC_EDIT_24HR_DUR, m_edit24HrDur);
	DDX_Control(pDX, IDC_SPIN_24HR_DUR, m_spin24HrDur);
	DDX_Control(pDX, IDC_EDIT_1HR_DRIZZLE, m_edit1HrDrizzle);
	DDX_Control(pDX, IDC_EDIT_1HR_RAIN, m_edit1HrRain);
	DDX_Control(pDX, IDC_EDIT_1HR_SHOWERS, m_edit1HrShowers);
	DDX_Control(pDX, IDC_EDIT_3HR_AMT, m_edit3HrAmt);
	DDX_Control(pDX, IDC_EDIT_24HR_AMT, m_edit24HrAmt);
	//nemeric spin controls
	DDX_Control(pDX, IDC_SPIN_1HR_DRIZZLE, m_spin1HrDrizzle);
	DDX_Control(pDX, IDC_SPIN_1HR_RAIN, m_spin1HrRain);
	DDX_Control(pDX, IDC_SPIN_1HR_SHOWERS, m_spin1HrShowers);
	DDX_Control(pDX, IDC_SPIN_3HR_AMT, m_spin3HrAmt);
	DDX_Control(pDX, IDC_SPIN_24HR_AMT, m_spin24HrAmt);

}


BEGIN_MESSAGE_MAP(CSowThresholdsDlg, CDialog)
END_MESSAGE_MAP()


// CSowThresholdsDlg message handlers

BOOL CSowThresholdsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_SIG_Station = m_pSowSet->m_SIG_Station;
	m_PCNT_Clear = m_pSowSet->m_PCNT_Clear;
	m_PCNT_Scattered = m_pSowSet->m_PCNT_Scattered;
	m_PCNT_Broken = m_pSowSet->m_PCNT_Broken;
	m_1HR_Drizzle = m_pSowSet->m_1HR_Drizzle;
	m_1HR_Rain = m_pSowSet->m_1HR_Rain;
	m_1HR_Showers = m_pSowSet->m_1HR_Showers;
	m_3HR_DUR_WetFlag = m_pSowSet->m_3HR_DUR_WetFlag;
	m_3HR_AMT_WetFlag = m_pSowSet->m_3HR_AMT_WetFlag;
	m_24HR_DUR_WetFlag = m_pSowSet->m_24HR_DUR_WetFlag;
	m_24HR_AMT_WetFlag = m_pSowSet->m_24HR_AMT_WetFlag;

	m_spinPcntClear.SetRange(0, 100);
	m_spinPcntClear.SetPos(m_PCNT_Clear);

	m_spinPcntScattered.SetRange(0, 100);
	m_spinPcntScattered.SetPos(m_PCNT_Scattered);

	m_spinPcntBroken.SetRange(0, 100);
	m_spinPcntBroken.SetPos(m_PCNT_Broken);

	m_spin1HrDrizzle.SetDecimalPlaces (2);
	m_spin1HrDrizzle.SetTrimTrailingZeros (FALSE);
	m_spin1HrDrizzle.SetRangeAndDelta (0.01, 20.0, 0.01);
	m_spin1HrDrizzle.SetBuddy(&m_edit1HrDrizzle);
	m_spin1HrDrizzle.SetPos(m_1HR_Drizzle);

	m_spin1HrRain.SetDecimalPlaces (2);
	m_spin1HrRain.SetTrimTrailingZeros (FALSE);
	m_spin1HrRain.SetRangeAndDelta (0.01, 20.0, 0.01);
	m_spin1HrRain.SetBuddy(&m_edit1HrRain);
	m_spin1HrRain.SetPos(m_1HR_Rain);

	m_spin1HrShowers.SetDecimalPlaces (2);
	m_spin1HrShowers.SetTrimTrailingZeros (FALSE);
	m_spin1HrShowers.SetRangeAndDelta (0.01, 20.0, 0.01);
	m_spin1HrShowers.SetBuddy(&m_edit1HrShowers);
	m_spin1HrShowers.SetPos(m_1HR_Showers);

	m_spin3HrDur.SetRange(1, 3);
	m_spin3HrDur.SetPos(m_3HR_DUR_WetFlag);

	m_spin3HrAmt.SetDecimalPlaces (2);
	m_spin3HrAmt.SetTrimTrailingZeros (FALSE);
	m_spin3HrAmt.SetRangeAndDelta (0.01, 20.0, 0.01);
	m_spin3HrAmt.SetBuddy(&m_edit3HrAmt);
	m_spin3HrAmt.SetPos(m_3HR_AMT_WetFlag);

	m_spin24HrDur.SetRange(1, 24);
	m_spin24HrDur.SetPos(m_24HR_DUR_WetFlag);

	m_spin24HrAmt.SetDecimalPlaces (2);
	m_spin24HrAmt.SetTrimTrailingZeros (FALSE);
	m_spin24HrAmt.SetRangeAndDelta (0.01, 20.0, 0.01);
	m_spin24HrAmt.SetBuddy(&m_edit24HrAmt);
	m_spin24HrAmt.SetPos(m_24HR_AMT_WetFlag);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSowThresholdsDlg::OnOK()
{
	m_PCNT_Clear = m_spinPcntClear.GetPos();
	m_PCNT_Scattered = m_spinPcntScattered.GetPos();
	m_PCNT_Broken = m_spinPcntBroken.GetPos();
	m_1HR_Drizzle = m_spin1HrDrizzle.GetPos();
	m_1HR_Rain = m_spin1HrRain.GetPos();
	m_1HR_Showers = m_spin1HrShowers.GetPos();
	m_3HR_DUR_WetFlag = m_spin3HrDur.GetPos();
	m_3HR_AMT_WetFlag = m_spin3HrAmt.GetPos();
	m_24HR_DUR_WetFlag = m_spin24HrDur.GetPos();
	m_24HR_AMT_WetFlag = m_spin24HrAmt.GetPos();

	//ok, logic checks
	//PCNT's
	if(m_PCNT_Clear <= 0 || m_PCNT_Clear > 100)
	{
		AfxMessageBox("Error: PCNT_Clear must be from 1..100");
		return;
	}
	if(m_PCNT_Scattered <= 0 || m_PCNT_Scattered > 100)
	{
		AfxMessageBox("Error: PCNT_Scattered must be from 1..100");
		return;
	}
	if(m_PCNT_Broken <= 0 || m_PCNT_Broken > 100)
	{
		AfxMessageBox("Error: PCNT_Broken must be from 1..100");
		return;
	}
	if(m_PCNT_Clear <= m_PCNT_Scattered || m_PCNT_Scattered <= m_PCNT_Broken)
	{
		AfxMessageBox("Error: PCNT_Clear, PCNT_Scattered, PCNT_Broken must be in descending order.");
		return;
	}

	if(m_1HR_Drizzle <= 0.0)
	{
		AfxMessageBox("Error: 1HR_Drizzle must be > 0.0");
		return;
	}
	if(m_1HR_Rain <= 0.0)
	{
		AfxMessageBox("Error: 1HR_Rain must be > 0.0");
		return;
	}
	if(m_1HR_Showers <= 0.0)
	{
		AfxMessageBox("Error: 1HR_Showers must be > 0.0");
		return;
	}
	if(m_1HR_Drizzle >= m_1HR_Rain || m_1HR_Rain >= m_1HR_Showers)
	{
		AfxMessageBox("Error: 1HR_Drizzle, 1HR_Rain, 1HR_Showers must be in ascending order.");
		return;
	}
	if(m_3HR_DUR_WetFlag <= 0 || m_3HR_DUR_WetFlag > 3)
	{
		AfxMessageBox("Error: 3HR_DUR_WetFlag must be from 1 to 3");
		return;
	}
	if(m_24HR_DUR_WetFlag <= m_3HR_DUR_WetFlag || m_24HR_DUR_WetFlag > 24)
	{
		AfxMessageBox("Error: 24HR_DUR_WetFlag must be from 3HR_DUR_WetFlag to 24");
		return;
	}
	if(m_3HR_AMT_WetFlag <= 0.0)
	{
		AfxMessageBox("Error: 3HR_AMT_WetFlag must be > 0.0");
		return;
	}
	if(m_24HR_AMT_WetFlag <= 0.0)
	{
		AfxMessageBox("Error: 24HR_AMT_WetFlag must be > 0.0");
		return;
	}
	if(m_24HR_AMT_WetFlag <= m_3HR_AMT_WetFlag)
	{
		AfxMessageBox("Error: 24HR_AMT_WetFlag must be > 3HR_AMT_WetFlag");
		return;
	}

	m_pSowSet->Edit();
	m_pSowSet->m_PCNT_Clear = m_PCNT_Clear;
	m_pSowSet->m_PCNT_Scattered = m_PCNT_Scattered;
	m_pSowSet->m_PCNT_Broken = m_PCNT_Broken;
	m_pSowSet->m_1HR_Drizzle = m_1HR_Drizzle;
	m_pSowSet->m_1HR_Rain = m_1HR_Rain;
	m_pSowSet->m_1HR_Showers = m_1HR_Showers;
	m_pSowSet->m_3HR_DUR_WetFlag = m_3HR_DUR_WetFlag;
	m_pSowSet->m_3HR_AMT_WetFlag = m_3HR_AMT_WetFlag;
	m_pSowSet->m_24HR_DUR_WetFlag = m_24HR_DUR_WetFlag;
	m_pSowSet->m_24HR_AMT_WetFlag = m_24HR_AMT_WetFlag;

	m_pSowSet->Update();
	m_pSowSet->Requery();
	CDialog::OnOK();
}

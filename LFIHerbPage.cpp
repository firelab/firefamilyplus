// LFIHerbPage.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "LFIHerbPage.h"
//#include "ClimAnalysis.h"
//#include "fireplusSet.h"
//#include "fireplusDoc.h"

// CLFIHerbPage dialog

IMPLEMENT_DYNAMIC(CLFIHerbPage, CPropertyPage)

CLFIHerbPage::CLFIHerbPage()
	: CPropertyPage(CLFIHerbPage::IDD)
{
	pLfiSet = NULL;
	tminMin = -2.0;
	tminMax = 5.0;
	vpdMin = 900.0;
	vpdMax = 4100.0;
	daylenMin = 36000;
	daylenMax = 39600;
	m_UseVPDavg = TRUE;
	daysAvg = 21;
	daysPcp = 30;


	maxGSI = 1.0;
	greenup = 0.5;
	maxHerb = 250.0;
	minHerb = 30.0;
	pcpMin = 0.5;
	pcpMax = 1.5;
	m_UseRTPrecip = FALSE;
}

CLFIHerbPage::~CLFIHerbPage()
{
}

void CLFIHerbPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TMINMIN, m_editTminMin);
	DDX_Control(pDX, IDC_EDIT_TMINMAX, m_editTminMax);
	DDX_Control(pDX, IDC_EDIT_VPDMIN, m_editVPDMin);
	DDX_Control(pDX, IDC_EDIT_VPDMAX, m_editVPDMax);
	DDX_Control(pDX, IDC_EDIT_DAYLENMIN, m_editDaylenMin);
	DDX_Control(pDX, IDC_EDIT_DAYLENMAX, m_editDaylenMax);
	DDX_Control(pDX, IDC_EDIT_DAYS_AVG, m_editDaysAvg);
	DDX_Control(pDX, IDC_SPIN_TMINMIN, m_spinTminMin);
	DDX_Control(pDX, IDC_SPIN_TMINMAX, m_spinTminMax);
	DDX_Control(pDX, IDC_SPIN_VPDMIN, m_spinVPDMin);
	DDX_Control(pDX, IDC_SPIN_VPDMAX, m_spinVPDMax);
	DDX_Control(pDX, IDC_SPIN_DAYLENMIN, m_spinDaylenMin);
	DDX_Control(pDX, IDC_SPIN_DAYLENMAX, m_spinDaylenMax);
	DDX_Control(pDX, IDC_SPIN_DAYS_AVG, m_spinDaysAvg);

	DDX_Control(pDX, IDC_EDIT_MAXGSI, m_editMaxGSI);
	DDX_Control(pDX, IDC_EDIT_GREENUP, m_editGreenup);
	DDX_Control(pDX, IDC_EDIT_MAXHERBFM, m_editMaxHerb);
	DDX_Control(pDX, IDC_EDIT_MINHERBFM, m_editMinHerb);
	DDX_Control(pDX, IDC_SPIN_MAXGSI, m_spinMaxGSI);
	DDX_Control(pDX, IDC_SPIN_GREENUP, m_spinGreenup);
	DDX_Control(pDX, IDC_SPIN_MAXHERBFM, m_spinMaxHerb);
	DDX_Control(pDX, IDC_SPIN_MINHERBFM, m_spinMinHerb);
	DDX_Control(pDX, IDC_EDIT_DAYS_PRECIP, m_editPcpDays);
	DDX_Control(pDX, IDC_SPIN_DAYS_PRECIP, m_spinPcpDays);
	DDX_Control(pDX, IDC_COMBO_VPD_USAGE, m_comboVPD);
	DDX_Control(pDX, IDC_EDIT_PRECIP_MIN, m_editPcpMin);
	DDX_Control(pDX, IDC_SPIN_PRECIP_MIN2, m_spinPcpMin);
	DDX_Control(pDX, IDC_EDIT_PRECIP_MAX2, m_editPcpMax);
	DDX_Control(pDX, IDC_SPIN_PRECIP_MAX2, m_spinPcpMax);
	DDX_Control(pDX, IDC_CHECK_USE_RTPRECIP, m_btnUseRTPrecip);
	DDX_Control(pDX, IDC_BUTTON_CALC_GSI, m_btnCalcMaxGSI);
}


BEGIN_MESSAGE_MAP(CLFIHerbPage, CPropertyPage)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TMINMIN, &CLFIHerbPage::OnDeltaposSpinTminmin)
	ON_EN_CHANGE(IDC_EDIT_TMINMIN, &CLFIHerbPage::OnEnChangeEditTminmin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TMINMAX, &CLFIHerbPage::OnDeltaposSpinTminmax)
	ON_EN_CHANGE(IDC_EDIT_TMINMAX, &CLFIHerbPage::OnEnChangeEditTminmax)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VPDMIN, &CLFIHerbPage::OnDeltaposSpinVpdmin)
	ON_EN_CHANGE(IDC_EDIT_VPDMIN, &CLFIHerbPage::OnEnChangeEditVpdmin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VPDMAX, &CLFIHerbPage::OnDeltaposSpinVpdmax)
	ON_EN_CHANGE(IDC_EDIT_VPDMAX, &CLFIHerbPage::OnEnChangeEditVpdmax)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYLENMIN, &CLFIHerbPage::OnDeltaposSpinDaylenmin)
	ON_EN_CHANGE(IDC_EDIT_DAYLENMIN, &CLFIHerbPage::OnEnChangeEditDaylenmin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYLENMAX, &CLFIHerbPage::OnDeltaposSpinDaylenmax)
	ON_EN_CHANGE(IDC_EDIT_DAYLENMAX, &CLFIHerbPage::OnEnChangeEditDaylenmax)
	//ON_BN_CLICKED(IDC_RADIO_VPD_MAX, &CLFIHerbPage::OnBnClickedRadioVpdMax)
	//ON_BN_CLICKED(IDC_RADIO_VPD_AVG, &CLFIHerbPage::OnBnClickedRadioVpdAvg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYS_AVG, &CLFIHerbPage::OnDeltaposSpinDaysAvg)
	ON_EN_CHANGE(IDC_EDIT_DAYS_AVG, &CLFIHerbPage::OnEnChangeEditDaysAvg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAXGSI, &CLFIHerbPage::OnDeltaposSpinMaxgsi)
	ON_EN_CHANGE(IDC_EDIT_MAXGSI, &CLFIHerbPage::OnEnChangeEditMaxgsi)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GREENUP, &CLFIHerbPage::OnDeltaposSpinGreenup)
	ON_EN_CHANGE(IDC_EDIT_GREENUP, &CLFIHerbPage::OnEnChangeEditGreenup)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAXHERBFM, &CLFIHerbPage::OnDeltaposSpinMaxherbfm)
	ON_EN_CHANGE(IDC_EDIT_MAXHERBFM, &CLFIHerbPage::OnEnChangeEditMaxherbfm)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINHERBFM, &CLFIHerbPage::OnDeltaposSpinMinherbfm)
	ON_EN_CHANGE(IDC_EDIT_MINHERBFM, &CLFIHerbPage::OnEnChangeEditMinherbfm)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULTS, &CLFIHerbPage::OnBnClickedButtonDefaults)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULTS_STORED, &CLFIHerbPage::OnBnClickedButtonStoredDefaults)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULTS_SAVE, &CLFIHerbPage::OnBnClickedButtonSaveDefaults)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYS_PRECIP, &CLFIHerbPage::OnDeltaposSpinPcpDays)
	ON_EN_CHANGE(IDC_EDIT_DAYS_PRECIP, &CLFIHerbPage::OnEnChangeEditPcpDays)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PRECIP_MIN2, &CLFIHerbPage::OnDeltaposSpinPrecipMin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PRECIP_MAX2, &CLFIHerbPage::OnDeltaposSpinPrecipMax)
	ON_EN_CHANGE(IDC_EDIT_PRECIP_MIN, &CLFIHerbPage::OnEnChangeEditPrecipMin)
	ON_EN_CHANGE(IDC_EDIT_PRECIP_MAX2, &CLFIHerbPage::OnEnChangeEditPrecipMax)
	ON_CBN_SELCHANGE(IDC_COMBO_VPD_USAGE, &CLFIHerbPage::OnCbnSelchangeComboVpdUsage)
	ON_BN_CLICKED(IDC_CHECK_USE_RTPRECIP, &CLFIHerbPage::OnBnClickedCheckUseRtprecip)
	ON_BN_CLICKED(IDC_BUTTON_CALC_GSI, &CLFIHerbPage::OnBnClickedButtonCalcGsi)
END_MESSAGE_MAP()


// CLFIHerbPage message handlers
BOOL CLFIHerbPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	if(pLfiSet)
	{
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_HerbTMinMin))
			tminMin = pLfiSet->m_HerbTMinMin;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_HerbTMinMax))
			tminMax = pLfiSet->m_HerbTMinMax;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_HerbVPDMin))
			vpdMin = pLfiSet->m_HerbVPDMin;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_HerbVPDMax))
			vpdMax = pLfiSet->m_HerbVPDMax;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_HerbDaylenMin))
			daylenMin = pLfiSet->m_HerbDaylenMin;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_HerbDaylenMax))
			daylenMax = pLfiSet->m_HerbDaylenMax;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_HerbUseVPDAvg))
			m_UseVPDavg = pLfiSet->m_HerbUseVPDAvg;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_HerbDaysAvg))
			daysAvg = pLfiSet->m_HerbDaysAvg;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_HerbMaxGSI))
			maxGSI = pLfiSet->m_HerbMaxGSI;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_HerbGreenup))
			greenup = pLfiSet->m_HerbGreenup;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_HerbMax))
			maxHerb = pLfiSet->m_HerbMax;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_HerbMin))
			minHerb = pLfiSet->m_HerbMin;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_HerbPcpDays))
			daysPcp = pLfiSet->m_HerbPcpDays;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_HerbPcpMin))
			pcpMin = pLfiSet->m_HerbPcpMin;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_HerbPcpMax))
			pcpMax = pLfiSet->m_HerbPcpMax;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_HerbUseRTPrecip))
			m_UseRTPrecip = pLfiSet->m_HerbUseRTPrecip;

		m_spinTminMin.SetDecimalPlaces (1);
		m_spinTminMin.SetTrimTrailingZeros (FALSE);
		m_spinTminMin.SetRangeAndDelta (-50.0, 200.0, 0.1);
		m_spinTminMin.SetBuddy(&m_editTminMin);
		m_spinTminMin.SetPos(tminMin);

		m_spinTminMax.SetDecimalPlaces (1);
		m_spinTminMax.SetTrimTrailingZeros (FALSE);
		m_spinTminMax.SetRangeAndDelta (-50.0, 200.0, 0.1);
		m_spinTminMax.SetBuddy(&m_editTminMax);
		m_spinTminMax.SetPos(tminMax);

		m_spinVPDMin.SetDecimalPlaces (1);
		m_spinVPDMin.SetTrimTrailingZeros (FALSE);
		m_spinVPDMin.SetRangeAndDelta (0.0, 10000.0, 10.0);
		m_spinVPDMin.SetBuddy(&m_editVPDMin);
		m_spinVPDMin.SetPos(vpdMin);

		m_spinVPDMax.SetDecimalPlaces (1);
		m_spinVPDMax.SetTrimTrailingZeros (FALSE);
		m_spinVPDMax.SetRangeAndDelta (0.0, 10000.0, 10.0);
		m_spinVPDMax.SetBuddy(&m_editVPDMax);
		m_spinVPDMax.SetPos(vpdMax);

		m_spinDaylenMin.SetDecimalPlaces (1);
		m_spinDaylenMin.SetTrimTrailingZeros (FALSE);
		m_spinDaylenMin.SetRangeAndDelta (0.0, 86400.0, 10.0);
		m_spinDaylenMin.SetBuddy(&m_editDaylenMin);
		m_spinDaylenMin.SetPos(daylenMin);

		m_spinDaylenMax.SetDecimalPlaces (1);
		m_spinDaylenMax.SetTrimTrailingZeros (FALSE);
		m_spinDaylenMax.SetRangeAndDelta (0.0, 86400.0, 10.0);
		m_spinDaylenMax.SetBuddy(&m_editDaylenMax);
		m_spinDaylenMax.SetPos(daylenMax);

		//((CButton *)GetDlgItem(IDC_RADIO_VPD_MAX))->SetCheck(!m_UseVPDavg);
		//((CButton *)GetDlgItem(IDC_RADIO_VPD_AVG))->SetCheck(m_UseVPDavg);

		m_spinDaysAvg.SetRange(1, 90);
		m_spinDaysAvg.SetPos(daysAvg);

		m_spinMaxGSI.SetDecimalPlaces (2);
		m_spinMaxGSI.SetTrimTrailingZeros (FALSE);
		m_spinMaxGSI.SetRangeAndDelta (0.0, 1.0, 0.01);
		m_spinMaxGSI.SetBuddy(&m_editMaxGSI);
		m_spinMaxGSI.SetPos(maxGSI);

		m_spinGreenup.SetDecimalPlaces (2);
		m_spinGreenup.SetTrimTrailingZeros (FALSE);
		m_spinGreenup.SetRangeAndDelta (0.0, 1.0, 0.01);
		m_spinGreenup.SetBuddy(&m_editGreenup);
		m_spinGreenup.SetPos(greenup);

		m_spinMaxHerb.SetDecimalPlaces (1);
		m_spinMaxHerb.SetTrimTrailingZeros (FALSE);
		m_spinMaxHerb.SetRangeAndDelta (0.0, 250.0, 1.0);
		m_spinMaxHerb.SetBuddy(&m_editMaxHerb);
		m_spinMaxHerb.SetPos(maxHerb);

		m_spinMinHerb.SetDecimalPlaces (1);
		m_spinMinHerb.SetTrimTrailingZeros (FALSE);
		m_spinMinHerb.SetRangeAndDelta (0.0, 250.0, 1.0);
		m_spinMinHerb.SetBuddy(&m_editMinHerb);
		m_spinMinHerb.SetPos(minHerb);

		m_spinPcpDays.SetRange(1, 90);
		m_spinPcpDays.SetPos(daysPcp);

		m_spinPcpMin.SetDecimalPlaces(1);
		m_spinPcpMin.SetTrimTrailingZeros(FALSE);
		m_spinPcpMin.SetRangeAndDelta(0.0, 90.0, 0.1);
		m_spinPcpMin.SetBuddy(&m_editPcpMin);
		m_spinPcpMin.SetPos(pcpMin);

		m_spinPcpMax.SetDecimalPlaces(1);
		m_spinPcpMax.SetTrimTrailingZeros(FALSE);
		m_spinPcpMax.SetRangeAndDelta(0.0, 90.0, 0.1);
		m_spinPcpMax.SetBuddy(&m_editPcpMax);
		m_spinPcpMax.SetPos(pcpMax);

		if (m_UseVPDavg)
			m_comboVPD.SetCurSel(1);
		else
			m_comboVPD.SetCurSel(0);
		m_btnUseRTPrecip.SetCheck(m_UseRTPrecip);
		EnableRTPrecipFields();

		//disable CalcMaxGSI button if the defaults...
		//if (pLfiSet->m_SIG_Station.Compare("######") == 0)
		//{
			m_btnCalcMaxGSI.EnableWindow(FALSE);
			m_btnCalcMaxGSI.ShowWindow(SW_HIDE);
		//}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CLFIHerbPage::OnApply()
{
	if(pLfiSet)
	{
		pLfiSet->Edit();
		pLfiSet->m_HerbTMinMin = m_spinTminMin.GetPos();
		pLfiSet->m_HerbTMinMax = m_spinTminMax.GetPos();
		pLfiSet->m_HerbVPDMin = m_spinVPDMin.GetPos();
		pLfiSet->m_HerbVPDMax = m_spinVPDMax.GetPos();
		pLfiSet->m_HerbDaylenMin = m_spinDaylenMin.GetPos();
		pLfiSet->m_HerbDaylenMax = m_spinDaylenMax.GetPos();
		//pLfiSet->m_HerbUseVPDAvg = ((CButton *)GetDlgItem(IDC_RADIO_VPD_AVG))->GetCheck();
		//int sel = m_comboVPD.GetCurSel();
		if (m_comboVPD.GetCurSel() == 1)
			pLfiSet->m_HerbUseVPDAvg = TRUE;
		else
			pLfiSet->m_HerbUseVPDAvg = FALSE;
		pLfiSet->m_HerbDaysAvg = m_spinDaysAvg.GetPos();
		pLfiSet->m_HerbMaxGSI = m_spinMaxGSI.GetPos();
		pLfiSet->m_HerbGreenup = m_spinGreenup.GetPos();
		pLfiSet->m_HerbMax = m_spinMaxHerb.GetPos();
		pLfiSet->m_HerbMin = m_spinMinHerb.GetPos();
		pLfiSet->m_HerbPcpDays = m_spinPcpDays.GetPos();
		pLfiSet->m_HerbPcpMin = m_spinPcpMin.GetPos();
		pLfiSet->m_HerbPcpMax = m_spinPcpMax.GetPos();
		pLfiSet->m_HerbUseRTPrecip = m_UseRTPrecip;
		pLfiSet->Update();
	}
	return CPropertyPage::OnApply();
}

void CLFIHerbPage::OnDeltaposSpinTminmin(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIHerbPage::OnEnChangeEditTminmin()
{
	SetModified();
}

void CLFIHerbPage::OnDeltaposSpinTminmax(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIHerbPage::OnEnChangeEditTminmax()
{
	SetModified();
}

void CLFIHerbPage::OnDeltaposSpinVpdmin(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIHerbPage::OnEnChangeEditVpdmin()
{
	SetModified();
}

void CLFIHerbPage::OnDeltaposSpinVpdmax(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIHerbPage::OnEnChangeEditVpdmax()
{
	SetModified();
}

void CLFIHerbPage::OnDeltaposSpinDaylenmin(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIHerbPage::OnEnChangeEditDaylenmin()
{
	SetModified();
}

void CLFIHerbPage::OnDeltaposSpinDaylenmax(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIHerbPage::OnEnChangeEditDaylenmax()
{
	SetModified();
}

void CLFIHerbPage::OnDeltaposSpinDaysAvg(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIHerbPage::OnEnChangeEditDaysAvg()
{
	SetModified();
}

void CLFIHerbPage::OnDeltaposSpinMaxgsi(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIHerbPage::OnEnChangeEditMaxgsi()
{
	SetModified();
}

void CLFIHerbPage::OnDeltaposSpinGreenup(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIHerbPage::OnEnChangeEditGreenup()
{
	SetModified();
}

void CLFIHerbPage::OnDeltaposSpinMaxherbfm(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIHerbPage::OnEnChangeEditMaxherbfm()
{
	SetModified();
}

void CLFIHerbPage::OnDeltaposSpinMinherbfm(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIHerbPage::OnEnChangeEditMinherbfm()
{
	SetModified();
}

/*void CLFIHerbPage::OnDeltaposSpinDaysAvg(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIHerbPage::OnEnChangeEditDaysAvg()
{
	SetModified();
}
*/
void CLFIHerbPage::OnDeltaposSpinPcpDays(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIHerbPage::OnEnChangeEditPcpDays()
{
	SetModified();
}

void CLFIHerbPage::OnBnClickedButtonDefaults()
{
	tminMin = -2.0;
	tminMax = 5.0;
	vpdMin = 900.0;
	vpdMax = 4100.0;
	daylenMin = 36000;
	daylenMax = 39600;
	m_UseVPDavg = TRUE;
	daysAvg = 21;
	maxGSI = 1.0;
	greenup = 0.5;
	maxHerb = 250.0;
	minHerb = 30.0;
	daysPcp = 30;
	pcpMin = 0.5;
	pcpMax = 1.5;
	m_UseRTPrecip = FALSE;

	m_spinTminMin.SetPos(tminMin);
	m_spinTminMax.SetPos(tminMax);
	m_spinVPDMin.SetPos(vpdMin);
	m_spinVPDMax.SetPos(vpdMax);
	m_spinDaylenMin.SetPos(daylenMin);
	m_spinDaylenMax.SetPos(daylenMax);
	m_comboVPD.SetCurSel(m_UseVPDavg ? 1 : 0);
	m_spinDaysAvg.SetPos(daysAvg);

	m_spinMaxGSI.SetPos(maxGSI);
	m_spinGreenup.SetPos(greenup);
	m_spinMaxHerb.SetPos(maxHerb);
	m_spinMinHerb.SetPos(minHerb);

	m_spinPcpDays.SetPos(daysPcp);
	m_spinPcpMin.SetPos(pcpMin);
	m_spinPcpMax.SetPos(pcpMax);
	m_btnUseRTPrecip.SetCheck(m_UseRTPrecip);
	EnableRTPrecipFields();

	SetModified();
}

void CLFIHerbPage::OnBnClickedButtonStoredDefaults()
{
	CLFISet defSet(pLfiSet->m_pDatabase);
	defSet.m_strFilter.Format("[SIG_Station] = '######'");
	defSet.Open();
	if(!defSet.IsEOF())
	{
		tminMin = defSet.m_HerbTMinMin;
		tminMax = defSet.m_HerbTMinMax;
		vpdMin = defSet.m_HerbVPDMin;
		vpdMax = defSet.m_HerbVPDMax;
		daylenMin = defSet.m_HerbDaylenMin;
		daylenMax = defSet.m_HerbDaylenMax;
		m_UseVPDavg = defSet.m_HerbUseVPDAvg;
		daysAvg = defSet.m_HerbDaysAvg;
		maxGSI = defSet.m_HerbMaxGSI;
		greenup = defSet.m_HerbGreenup;
		maxHerb = defSet.m_HerbMax;
		minHerb = defSet.m_HerbMin;
		daysPcp = defSet.m_HerbPcpDays;
		pcpMin = defSet.m_HerbPcpMin;
		pcpMax = defSet.m_HerbPcpMax;
		m_UseRTPrecip = defSet.m_HerbUseRTPrecip;
	}
	defSet.Close();
	m_spinTminMin.SetPos(tminMin);
	m_spinTminMax.SetPos(tminMax);
	m_spinVPDMin.SetPos(vpdMin);
	m_spinVPDMax.SetPos(vpdMax);
	m_spinDaylenMin.SetPos(daylenMin);
	m_spinDaylenMax.SetPos(daylenMax);
	//((CButton *)GetDlgItem(IDC_RADIO_VPD_MAX))->SetCheck(!m_UseVPDavg);
	//((CButton *)GetDlgItem(IDC_RADIO_VPD_AVG))->SetCheck(m_UseVPDavg);
	m_comboVPD.SetCurSel(m_UseVPDavg ? 1 : 0);
	m_spinDaysAvg.SetPos(daysAvg);
	m_spinMaxGSI.SetPos(maxGSI);
	m_spinGreenup.SetPos(greenup);
	m_spinMaxHerb.SetPos(maxHerb);
	m_spinMinHerb.SetPos(minHerb);
	m_spinPcpDays.SetPos(daysPcp);
	m_spinPcpMin.SetPos(pcpMin);
	m_spinPcpMax.SetPos(pcpMax);
	m_btnUseRTPrecip.SetCheck(m_UseRTPrecip);
	EnableRTPrecipFields();

	SetModified();
}

void CLFIHerbPage::OnBnClickedButtonSaveDefaults()
{
	CLFISet defSet(pLfiSet->m_pDatabase);
	defSet.m_strFilter.Format("[SIG_Station] = '######'");
	defSet.Open();
	if(!defSet.IsEOF())
		defSet.Edit();
	else
		defSet.AddNew();
	defSet.m_HerbTMinMin = m_spinTminMin.GetPos();
	defSet.m_HerbTMinMax = m_spinTminMax.GetPos();
	defSet.m_HerbVPDMin = m_spinVPDMin.GetPos();
	defSet.m_HerbVPDMax = m_spinVPDMax.GetPos();
	defSet.m_HerbDaylenMin = m_spinDaylenMin.GetPos();
	defSet.m_HerbDaylenMax = m_spinDaylenMax.GetPos();
	//defSet.m_HerbUseVPDAvg = ((CButton *)GetDlgItem(IDC_RADIO_VPD_AVG))->GetCheck();
	int sel = m_comboVPD.GetCurSel();
	defSet.m_HerbUseVPDAvg = sel == 1 ? TRUE : FALSE;
	defSet.m_HerbDaysAvg = m_spinDaysAvg.GetPos();
	defSet.m_HerbMax = m_spinMaxHerb.GetPos();
	defSet.m_HerbMin = m_spinMinHerb.GetPos();
	defSet.m_HerbMaxGSI = m_spinMaxGSI.GetPos();
	defSet.m_HerbGreenup = m_spinGreenup.GetPos();
	defSet.m_HerbPcpDays = m_spinPcpDays.GetPos();
	defSet.m_HerbPcpMin = m_spinPcpMin.GetPos();
	defSet.m_HerbPcpMax = m_spinPcpMax.GetPos();
	defSet.m_HerbUseRTPrecip = m_UseRTPrecip;
	defSet.Update();
	defSet.Close();
	//SetModified();
}

void CLFIHerbPage::OnDeltaposSpinPrecipMin(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}


void CLFIHerbPage::OnDeltaposSpinPrecipMax(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}


void CLFIHerbPage::OnEnChangeEditPrecipMin()
{
	SetModified();
}


void CLFIHerbPage::OnEnChangeEditPrecipMax()
{
	SetModified();
}


void CLFIHerbPage::OnCbnSelchangeComboVpdUsage()
{
	SetModified();
}


void CLFIHerbPage::OnBnClickedCheckUseRtprecip()
{
	m_UseRTPrecip = m_btnUseRTPrecip.GetCheck();
	SetModified();
	EnableRTPrecipFields();
}

void CLFIHerbPage::EnableRTPrecipFields()
{
	const BOOL check = m_btnUseRTPrecip.GetCheck();
	m_editPcpMin.EnableWindow(check);
	m_editPcpMax.EnableWindow(check);
	m_spinPcpMin.EnableWindow(check);
	m_spinPcpMax.EnableWindow(check);
	m_editPcpDays.EnableWindow(check);
	m_spinPcpDays.EnableWindow(check);
}

//extern CFireplusApp theApp;
CDocument* GetMDIActiveDocument()
{
	CDocument* pDoc = NULL;

	CWnd* pWndMain = AfxGetMainWnd();
	ASSERT(pWndMain);
	ASSERT(pWndMain->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))); // Not an MDI app.

	CFrameWnd* pFrame = ((CMDIFrameWnd*)pWndMain)->MDIGetActive();
	if (NULL != pFrame)
	{
		pDoc = pFrame->GetActiveDocument(); // get the active document
	}
	return pDoc;
}

void CLFIHerbPage::OnBnClickedButtonCalcGsi()
{
	int varIDs[2];
	varIDs[0] = 39;//GSI
	CString strStation = pLfiSet->m_SIG_Station;
	//CClimAnalysis clim(pLfiSet->m_pDatabase); 
	//CFireplusSet* pfpSet = ((CFireplusDoc*)GetMDIActiveDocument())->m_fireplusSet;
	//need to set/store GSI options, set regular GSI options to Herb GSI options
	//clim.Analyze(varIDs, 1, pfpSet, false);
	//clim.
}

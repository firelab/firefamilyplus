// LFIWoodyPage.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "LFIWoodyPage.h"


// CLFIWoodyPage dialog

IMPLEMENT_DYNAMIC(CLFIWoodyPage, CPropertyPage)

CLFIWoodyPage::CLFIWoodyPage()
	: CPropertyPage(CLFIWoodyPage::IDD)
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
	maxWoody = 200.0;
	minWoody = 50.0;
	pcpMin = 0.5;
	pcpMax = 1.5;
	m_UseRTPrecip = FALSE;
}

CLFIWoodyPage::~CLFIWoodyPage()
{
}

void CLFIWoodyPage::DoDataExchange(CDataExchange* pDX)
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
	DDX_Control(pDX, IDC_EDIT_MAXHERBFM, m_editMaxWoody);
	DDX_Control(pDX, IDC_EDIT_MINHERBFM, m_editMinWoody);
	DDX_Control(pDX, IDC_SPIN_MAXGSI, m_spinMaxGSI);
	DDX_Control(pDX, IDC_SPIN_GREENUP, m_spinGreenup);
	DDX_Control(pDX, IDC_SPIN_MAXHERBFM, m_spinMaxWoody);
	DDX_Control(pDX, IDC_SPIN_MINHERBFM, m_spinMinWoody);
	DDX_Control(pDX, IDC_EDIT_DAYS_PRECIP, m_editPcpDays);
	DDX_Control(pDX, IDC_SPIN_DAYS_PRECIP, m_spinPcpDays);
	DDX_Control(pDX, IDC_COMBO_VPD_USAGE, m_comboVPD);
	DDX_Control(pDX, IDC_EDIT_PRECIP_MIN, m_editPcpMin);
	DDX_Control(pDX, IDC_SPIN_PRECIP_MIN2, m_spinPcpMin);
	DDX_Control(pDX, IDC_EDIT_PRECIP_MAX2, m_editPcpMax);
	DDX_Control(pDX, IDC_SPIN_PRECIP_MAX2, m_spinPcpMax);
	DDX_Control(pDX, IDC_CHECK_USE_RTPRECIP, m_btnUseRTPrecip);
}


BEGIN_MESSAGE_MAP(CLFIWoodyPage, CPropertyPage)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TMINMIN, &CLFIWoodyPage::OnDeltaposSpinTminmin)
	ON_EN_CHANGE(IDC_EDIT_TMINMIN, &CLFIWoodyPage::OnEnChangeEditTminmin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TMINMAX, &CLFIWoodyPage::OnDeltaposSpinTminmax)
	ON_EN_CHANGE(IDC_EDIT_TMINMAX, &CLFIWoodyPage::OnEnChangeEditTminmax)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VPDMIN, &CLFIWoodyPage::OnDeltaposSpinVpdmin)
	ON_EN_CHANGE(IDC_EDIT_VPDMIN, &CLFIWoodyPage::OnEnChangeEditVpdmin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VPDMAX, &CLFIWoodyPage::OnDeltaposSpinVpdmax)
	ON_EN_CHANGE(IDC_EDIT_VPDMAX, &CLFIWoodyPage::OnEnChangeEditVpdmax)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYLENMIN, &CLFIWoodyPage::OnDeltaposSpinDaylenmin)
	ON_EN_CHANGE(IDC_EDIT_DAYLENMIN, &CLFIWoodyPage::OnEnChangeEditDaylenmin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYLENMAX, &CLFIWoodyPage::OnDeltaposSpinDaylenmax)
	ON_EN_CHANGE(IDC_EDIT_DAYLENMAX, &CLFIWoodyPage::OnEnChangeEditDaylenmax)
//	ON_BN_CLICKED(IDC_RADIO_VPD_MAX, &CLFIWoodyPage::OnBnClickedRadioVpdMax)
//	ON_BN_CLICKED(IDC_RADIO_VPD_AVG, &CLFIWoodyPage::OnBnClickedRadioVpdAvg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYS_AVG, &CLFIWoodyPage::OnDeltaposSpinDaysAvg)
	ON_EN_CHANGE(IDC_EDIT_DAYS_AVG, &CLFIWoodyPage::OnEnChangeEditDaysAvg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAXGSI, &CLFIWoodyPage::OnDeltaposSpinMaxgsi)
	ON_EN_CHANGE(IDC_EDIT_MAXGSI, &CLFIWoodyPage::OnEnChangeEditMaxgsi)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GREENUP, &CLFIWoodyPage::OnDeltaposSpinGreenup)
	ON_EN_CHANGE(IDC_EDIT_GREENUP, &CLFIWoodyPage::OnEnChangeEditGreenup)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAXHERBFM, &CLFIWoodyPage::OnDeltaposSpinMaxherbfm)
	ON_EN_CHANGE(IDC_EDIT_MAXHERBFM, &CLFIWoodyPage::OnEnChangeEditMaxherbfm)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINHERBFM, &CLFIWoodyPage::OnDeltaposSpinMinherbfm)
	ON_EN_CHANGE(IDC_EDIT_MINHERBFM, &CLFIWoodyPage::OnEnChangeEditMinherbfm)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULTS, &CLFIWoodyPage::OnBnClickedButtonDefaults)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULTS_STORED, &CLFIWoodyPage::OnBnClickedButtonStoredDefaults)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULTS_SAVE, &CLFIWoodyPage::OnBnClickedButtonSaveDefaults)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYS_PRECIP, &CLFIWoodyPage::OnDeltaposSpinPcpDays)
	ON_EN_CHANGE(IDC_EDIT_DAYS_PRECIP, &CLFIWoodyPage::OnEnChangeEditPcpDays)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PRECIP_MIN2, &CLFIWoodyPage::OnDeltaposSpinPrecipMin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PRECIP_MAX2, &CLFIWoodyPage::OnDeltaposSpinPrecipMax)
	ON_EN_CHANGE(IDC_EDIT_PRECIP_MIN, &CLFIWoodyPage::OnEnChangeEditPrecipMin)
	ON_EN_CHANGE(IDC_EDIT_PRECIP_MAX2, &CLFIWoodyPage::OnEnChangeEditPrecipMax)
	ON_CBN_SELCHANGE(IDC_COMBO_VPD_USAGE, &CLFIWoodyPage::OnCbnSelchangeComboVpdUsage)
	ON_BN_CLICKED(IDC_CHECK_USE_RTPRECIP, &CLFIWoodyPage::OnBnClickedCheckUseRtprecip)
END_MESSAGE_MAP()


// CLFIWoodyPage message handlers
BOOL CLFIWoodyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	if(pLfiSet)
	{
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyTMinMin))
			tminMin = pLfiSet->m_WoodyTMinMin;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyTMinMax))
			tminMax = pLfiSet->m_WoodyTMinMax;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyVPDMin))
			vpdMin = pLfiSet->m_WoodyVPDMin;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyVPDMax))
			vpdMax = pLfiSet->m_WoodyVPDMax;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyDaylenMin))
			daylenMin = pLfiSet->m_WoodyDaylenMin;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyDaylenMax))
			daylenMax = pLfiSet->m_WoodyDaylenMax;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyUseVPDAvg))
			m_UseVPDavg = pLfiSet->m_WoodyUseVPDAvg;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyDaysAvg))
			daysAvg = pLfiSet->m_WoodyDaysAvg;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyMaxGSI))
			maxGSI = pLfiSet->m_WoodyMaxGSI;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyGreenup))
			greenup = pLfiSet->m_WoodyGreenup;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyMax))
			maxWoody = pLfiSet->m_WoodyMax;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyMin))
			minWoody = pLfiSet->m_WoodyMin;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyPcpDays))
			daysPcp = pLfiSet->m_WoodyPcpDays;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyPcpMin))
			pcpMin = pLfiSet->m_WoodyPcpMin;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyPcpMax))
			pcpMax = pLfiSet->m_WoodyPcpMax;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_WoodyUseRTPrecip))
			m_UseRTPrecip = pLfiSet->m_WoodyUseRTPrecip;

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

		m_spinMaxWoody.SetDecimalPlaces (1);
		m_spinMaxWoody.SetTrimTrailingZeros (FALSE);
		m_spinMaxWoody.SetRangeAndDelta (50.0, 200.0, 1.0);
		m_spinMaxWoody.SetBuddy(&m_editMaxWoody);
		m_spinMaxWoody.SetPos(maxWoody);

		m_spinMinWoody.SetDecimalPlaces (1);
		m_spinMinWoody.SetTrimTrailingZeros (FALSE);
		m_spinMinWoody.SetRangeAndDelta (50.0, 200.0, 1.0);
		m_spinMinWoody.SetBuddy(&m_editMinWoody);
		m_spinMinWoody.SetPos(minWoody);

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
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CLFIWoodyPage::OnApply()
{
	if(pLfiSet)
	{
		pLfiSet->Edit();
		pLfiSet->m_WoodyTMinMin = m_spinTminMin.GetPos();
		pLfiSet->m_WoodyTMinMax = m_spinTminMax.GetPos();
		pLfiSet->m_WoodyVPDMin = m_spinVPDMin.GetPos();
		pLfiSet->m_WoodyVPDMax = m_spinVPDMax.GetPos();
		pLfiSet->m_WoodyDaylenMin = m_spinDaylenMin.GetPos();
		pLfiSet->m_WoodyDaylenMax = m_spinDaylenMax.GetPos();
		//pLfiSet->m_WoodyUseVPDAvg = ((CButton *)GetDlgItem(IDC_RADIO_VPD_AVG))->GetCheck();
		int sel = m_comboVPD.GetCurSel();
		if (sel == 1)
			pLfiSet->m_WoodyUseVPDAvg = TRUE;
		else
			pLfiSet->m_WoodyUseVPDAvg = FALSE;
		pLfiSet->m_WoodyDaysAvg = m_spinDaysAvg.GetPos();
		pLfiSet->m_WoodyMaxGSI = m_spinMaxGSI.GetPos();
		pLfiSet->m_WoodyGreenup = m_spinGreenup.GetPos();
		pLfiSet->m_WoodyMax = m_spinMaxWoody.GetPos();
		pLfiSet->m_WoodyMin = m_spinMinWoody.GetPos();
		pLfiSet->m_WoodyPcpDays = m_spinPcpDays.GetPos();
		pLfiSet->m_WoodyPcpMin = m_spinPcpMin.GetPos();
		pLfiSet->m_WoodyPcpMax = m_spinPcpMax.GetPos();
		pLfiSet->m_WoodyUseRTPrecip = m_UseRTPrecip;
		pLfiSet->Update();
	}
	return CPropertyPage::OnApply();
}

void CLFIWoodyPage::OnDeltaposSpinTminmin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIWoodyPage::OnEnChangeEditTminmin()
{
	SetModified();
}

void CLFIWoodyPage::OnDeltaposSpinTminmax(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIWoodyPage::OnEnChangeEditTminmax()
{
	SetModified();
}

void CLFIWoodyPage::OnDeltaposSpinVpdmin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIWoodyPage::OnEnChangeEditVpdmin()
{
	SetModified();
}

void CLFIWoodyPage::OnDeltaposSpinVpdmax(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIWoodyPage::OnEnChangeEditVpdmax()
{
	SetModified();
}

void CLFIWoodyPage::OnDeltaposSpinDaylenmin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIWoodyPage::OnEnChangeEditDaylenmin()
{
	SetModified();
}

void CLFIWoodyPage::OnDeltaposSpinDaylenmax(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIWoodyPage::OnEnChangeEditDaylenmax()
{
	SetModified();
}

void CLFIWoodyPage::OnDeltaposSpinDaysAvg(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIWoodyPage::OnEnChangeEditDaysAvg()
{
	SetModified();
}

void CLFIWoodyPage::OnDeltaposSpinMaxgsi(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIWoodyPage::OnEnChangeEditMaxgsi()
{
	SetModified();
}

void CLFIWoodyPage::OnDeltaposSpinGreenup(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIWoodyPage::OnEnChangeEditGreenup()
{
	SetModified();
}

void CLFIWoodyPage::OnDeltaposSpinMaxherbfm(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIWoodyPage::OnEnChangeEditMaxherbfm()
{
	SetModified();
}

void CLFIWoodyPage::OnDeltaposSpinMinherbfm(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIWoodyPage::OnEnChangeEditMinherbfm()
{
	SetModified();
}

void CLFIWoodyPage::OnDeltaposSpinPcpDays(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIWoodyPage::OnEnChangeEditPcpDays()
{
	SetModified();
}

void CLFIWoodyPage::OnBnClickedButtonDefaults()
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
	maxWoody = 200.0;
	minWoody = 50.0;
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
	//((CButton *)GetDlgItem(IDC_RADIO_VPD_MAX))->SetCheck(!m_UseVPDavg);
	//((CButton *)GetDlgItem(IDC_RADIO_VPD_AVG))->SetCheck(m_UseVPDavg);
	m_comboVPD.SetCurSel(m_UseVPDavg ? 1 : 0);
	m_spinDaysAvg.SetPos(daysAvg);
	m_spinMaxGSI.SetPos(maxGSI);
	m_spinGreenup.SetPos(greenup);
	m_spinMaxWoody.SetPos(maxWoody);
	m_spinMinWoody.SetPos(minWoody);

	m_spinPcpDays.SetPos(daysPcp);
	m_spinPcpMin.SetPos(pcpMin);
	m_spinPcpMax.SetPos(pcpMax);
	m_btnUseRTPrecip.SetCheck(m_UseRTPrecip);
	EnableRTPrecipFields();

	SetModified();
}

void CLFIWoodyPage::OnBnClickedButtonStoredDefaults()
{
	CLFISet defSet(pLfiSet->m_pDatabase);
	defSet.m_strFilter.Format("[SIG_Station] = '######'");
	defSet.Open();
	if(!defSet.IsEOF())
	{
		tminMin = defSet.m_WoodyTMinMin;
		tminMax = defSet.m_WoodyTMinMax;
		vpdMin = defSet.m_WoodyVPDMin;
		vpdMax = defSet.m_WoodyVPDMax;
		daylenMin = defSet.m_WoodyDaylenMin;
		daylenMax = defSet.m_WoodyDaylenMax;
		m_UseVPDavg = defSet.m_WoodyUseVPDAvg;
		daysAvg = defSet.m_WoodyDaysAvg;
		maxGSI = defSet.m_WoodyMaxGSI;
		greenup = defSet.m_WoodyGreenup;
		maxWoody = defSet.m_WoodyMax;
		minWoody = defSet.m_WoodyMin;
		daysPcp = defSet.m_WoodyPcpDays;
		pcpMin = defSet.m_WoodyPcpMin;
		pcpMax = defSet.m_WoodyPcpMax;
		m_UseRTPrecip = defSet.m_WoodyUseRTPrecip;
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
	m_spinMaxWoody.SetPos(maxWoody);
	m_spinMinWoody.SetPos(minWoody);
	m_spinPcpDays.SetPos(daysPcp);
	m_spinPcpMin.SetPos(pcpMin);
	m_spinPcpMax.SetPos(pcpMax);
	m_btnUseRTPrecip.SetCheck(m_UseRTPrecip);
	EnableRTPrecipFields();

	SetModified();
}

void CLFIWoodyPage::OnBnClickedButtonSaveDefaults()
{
	CLFISet defSet(pLfiSet->m_pDatabase);
	defSet.m_strFilter.Format("[SIG_Station] = '######'");
	defSet.Open();
	if(!defSet.IsEOF())
		defSet.Edit();
	else
		defSet.AddNew();
	defSet.m_WoodyTMinMin = m_spinTminMin.GetPos();
	defSet.m_WoodyTMinMax = m_spinTminMax.GetPos();
	defSet.m_WoodyVPDMin = m_spinVPDMin.GetPos();
	defSet.m_WoodyVPDMax = m_spinVPDMax.GetPos();
	defSet.m_WoodyDaylenMin = m_spinDaylenMin.GetPos();
	defSet.m_WoodyDaylenMax = m_spinDaylenMax.GetPos();
	//defSet.m_WoodyUseVPDAvg = ((CButton *)GetDlgItem(IDC_RADIO_VPD_AVG))->GetCheck();
	int sel = m_comboVPD.GetCurSel();
	defSet.m_WoodyUseVPDAvg = sel == 1 ? TRUE : FALSE;
	defSet.m_WoodyDaysAvg = m_spinDaysAvg.GetPos();
	defSet.m_WoodyMax = m_spinMaxWoody.GetPos();
	defSet.m_WoodyMin = m_spinMinWoody.GetPos();
	defSet.m_WoodyMaxGSI = m_spinMaxGSI.GetPos();
	defSet.m_WoodyGreenup = m_spinGreenup.GetPos();
	defSet.m_WoodyPcpDays = m_spinPcpDays.GetPos();
	defSet.m_WoodyPcpMin = m_spinPcpMin.GetPos();
	defSet.m_WoodyPcpMax = m_spinPcpMax.GetPos();
	defSet.m_WoodyUseRTPrecip = m_UseRTPrecip;
	defSet.Update();
	defSet.Close();
	//SetModified();
}

void CLFIWoodyPage::OnDeltaposSpinPrecipMin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}


void CLFIWoodyPage::OnDeltaposSpinPrecipMax(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}


void CLFIWoodyPage::OnEnChangeEditPrecipMin()
{
	SetModified();
}


void CLFIWoodyPage::OnEnChangeEditPrecipMax()
{
	SetModified();
}


void CLFIWoodyPage::OnCbnSelchangeComboVpdUsage()
{
	SetModified();
}


void CLFIWoodyPage::OnBnClickedCheckUseRtprecip()
{
	m_UseRTPrecip = m_btnUseRTPrecip.GetCheck();
	SetModified();
	EnableRTPrecipFields();
}

void CLFIWoodyPage::EnableRTPrecipFields()
{
	const BOOL check = m_btnUseRTPrecip.GetCheck();
	m_editPcpMin.EnableWindow(check);
	m_editPcpMax.EnableWindow(check);
	m_spinPcpMin.EnableWindow(check);
	m_spinPcpMax.EnableWindow(check);
	m_editPcpDays.EnableWindow(check);
	m_spinPcpDays.EnableWindow(check);
}

// LFIGeneralPage.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "LFIGeneralPage.h"


// CLFIGeneralPage dialog

IMPLEMENT_DYNAMIC(CLFIGeneralPage, CPropertyPage)

CLFIGeneralPage::CLFIGeneralPage()
	: CPropertyPage(CLFIGeneralPage::IDD)
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
	pcpMin = 0.5;
	pcpMax = 1.5;
}

CLFIGeneralPage::~CLFIGeneralPage()
{
}

void CLFIGeneralPage::DoDataExchange(CDataExchange* pDX)
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
	DDX_Control(pDX, IDC_EDIT_DAYS_PRECIP, m_editPcpDays);
	DDX_Control(pDX, IDC_SPIN_DAYS_PRECIP, m_spinPcpDays);
	DDX_Control(pDX, IDC_COMBO_VPD_USAGE, m_comboVPD);
	DDX_Control(pDX, IDC_EDIT_PRECIP_MIN, m_editPcpMin);
	DDX_Control(pDX, IDC_SPIN_PRECIP_MIN, m_spinPcpMin);
	DDX_Control(pDX, IDC_EDIT_PRECIP_MAX, m_editPcpMax);
	DDX_Control(pDX, IDC_SPIN_PRECIP_MAX, m_spinPcpMax);
}


BEGIN_MESSAGE_MAP(CLFIGeneralPage, CPropertyPage)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TMINMIN, &CLFIGeneralPage::OnDeltaposSpinTminmin)
	ON_EN_CHANGE(IDC_EDIT_TMINMIN, &CLFIGeneralPage::OnEnChangeEditTminmin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TMINMAX, &CLFIGeneralPage::OnDeltaposSpinTminmax)
	ON_EN_CHANGE(IDC_EDIT_TMINMAX, &CLFIGeneralPage::OnEnChangeEditTminmax)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VPDMIN, &CLFIGeneralPage::OnDeltaposSpinVpdmin)
	ON_EN_CHANGE(IDC_EDIT_VPDMIN, &CLFIGeneralPage::OnEnChangeEditVpdmin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VPDMAX, &CLFIGeneralPage::OnDeltaposSpinVpdmax)
	ON_EN_CHANGE(IDC_EDIT_VPDMAX, &CLFIGeneralPage::OnEnChangeEditVpdmax)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYLENMIN, &CLFIGeneralPage::OnDeltaposSpinDaylenmin)
	ON_EN_CHANGE(IDC_EDIT_DAYLENMIN, &CLFIGeneralPage::OnEnChangeEditDaylenmin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYLENMAX, &CLFIGeneralPage::OnDeltaposSpinDaylenmax)
	ON_EN_CHANGE(IDC_EDIT_DAYLENMAX, &CLFIGeneralPage::OnEnChangeEditDaylenmax)
	//ON_BN_CLICKED(IDC_RADIO_VPD_MAX, &CLFIGeneralPage::OnBnClickedRadioVpdMax)
	//ON_BN_CLICKED(IDC_RADIO_VPD_AVG, &CLFIGeneralPage::OnBnClickedRadioVpdAvg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYS_AVG, &CLFIGeneralPage::OnDeltaposSpinDaysAvg)
	ON_EN_CHANGE(IDC_EDIT_DAYS_AVG, &CLFIGeneralPage::OnEnChangeEditDaysAvg)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULTS, &CLFIGeneralPage::OnBnClickedButtonDefaults)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULTS_STORED, &CLFIGeneralPage::OnBnClickedButtonStoredDefaults)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULTS_SAVE, &CLFIGeneralPage::OnBnClickedButtonSaveDefaults)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYS_PRECIP, &CLFIGeneralPage::OnDeltaposSpinPcpDays)
	ON_EN_CHANGE(IDC_EDIT_DAYS_PRECIP, &CLFIGeneralPage::OnEnChangeEditPcpDays)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PRECIP_MIN, &CLFIGeneralPage::OnDeltaposSpinPrecipMin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PRECIP_MAX, &CLFIGeneralPage::OnDeltaposSpinPrecipMax)
	ON_EN_CHANGE(IDC_EDIT_PRECIP_MIN, &CLFIGeneralPage::OnEnChangeEditPrecipMin)
	ON_EN_CHANGE(IDC_EDIT_PRECIP_MAX, &CLFIGeneralPage::OnEnChangeEditPrecipMax)
	ON_CBN_SELCHANGE(IDC_COMBO_VPD_USAGE, &CLFIGeneralPage::OnCbnSelchangeComboVpdUsage)
END_MESSAGE_MAP()


// CLFIGeneralPage message handlers

BOOL CLFIGeneralPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	if(pLfiSet)
	{
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_TMinMin))
			tminMin = pLfiSet->m_TMinMin;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_TMinMax))
			tminMax = pLfiSet->m_TMinMax;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_VPDMin))
			vpdMin = pLfiSet->m_VPDMin;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_VPDMax))
			vpdMax = pLfiSet->m_VPDMax;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_DaylenMin))
			daylenMin = pLfiSet->m_DaylenMin;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_DaylenMax))
			daylenMax = pLfiSet->m_DaylenMax;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_UseVPDAvg))
			m_UseVPDavg = pLfiSet->m_UseVPDAvg;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_LFIdaysAvg))
			daysAvg = pLfiSet->m_LFIdaysAvg;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_PcpDays))
			daysPcp = pLfiSet->m_PcpDays;
		if(!pLfiSet->IsFieldNull(&pLfiSet->m_PcpMin))
			pcpMin = pLfiSet->m_PcpMin;
		if (!pLfiSet->IsFieldNull(&pLfiSet->m_PcpMax))
			pcpMax = pLfiSet->m_PcpMax;

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

		if(m_UseVPDavg)
			m_comboVPD.SetCurSel(1);
		else
			m_comboVPD.SetCurSel(0);

		//((CButton *)GetDlgItem(IDC_RADIO_VPD_MAX))->SetCheck(!m_UseVPDavg);
		//((CButton *)GetDlgItem(IDC_RADIO_VPD_AVG))->SetCheck(m_UseVPDavg);

		m_spinDaysAvg.SetRange(1, 90);
		m_spinDaysAvg.SetPos(daysAvg);

		m_spinPcpDays.SetRange(1, 90);
		m_spinPcpDays.SetPos(daysPcp);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CLFIGeneralPage::OnApply()
{
	UpdateData();
	if(pLfiSet)
	{
		pLfiSet->Edit();
		pLfiSet->m_TMinMin = m_spinTminMin.GetPos();
		pLfiSet->m_TMinMax = m_spinTminMax.GetPos();
		pLfiSet->m_VPDMin = m_spinVPDMin.GetPos();
		pLfiSet->m_VPDMax = m_spinVPDMax.GetPos();
		pLfiSet->m_DaylenMin = m_spinDaylenMin.GetPos();
		pLfiSet->m_DaylenMax = m_spinDaylenMax.GetPos();
		//pLfiSet->m_UseVPDAvg = ((CButton *)GetDlgItem(IDC_RADIO_VPD_AVG))->GetCheck();
		pLfiSet->m_LFIdaysAvg = m_spinDaysAvg.GetPos();
		pLfiSet->m_PcpDays = m_spinPcpDays.GetPos();
		//CString str;
		//m_editPcpMin.GetWindowTextA(str);
		pLfiSet->m_PcpMin = m_spinPcpMin.GetPos();// atof(str);
		//m_editPcpMax.GetWindowTextA(str);
		pLfiSet->m_PcpMax = m_spinPcpMax.GetPos();// atof(str);
		int sel = m_comboVPD.GetCurSel();
		if (sel == 1)
			pLfiSet->m_UseVPDAvg = TRUE;
		else
			pLfiSet->m_UseVPDAvg = FALSE;
		/*	lfiSet.m_MaxGSI = m_spinMaxGSI.GetPos();
		lfiSet.m_Greenup = m_spinGreenup.GetPos();
		lfiSet.m_MaxHerb = m_spinMaxHerb.GetPos();
		lfiSet.m_MinHerb = m_spinMinHerb.GetPos();
		lfiSet.m_MaxWoody = m_spinMaxWoody.GetPos();
		lfiSet.m_MinWoody = m_spinMinWoody.GetPos();*/
		pLfiSet->Update();
	}
	return CPropertyPage::OnApply();
}

void CLFIGeneralPage::OnDeltaposSpinTminmin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIGeneralPage::OnEnChangeEditTminmin()
{
	SetModified();
}

void CLFIGeneralPage::OnDeltaposSpinTminmax(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIGeneralPage::OnEnChangeEditTminmax()
{
	SetModified();
}

void CLFIGeneralPage::OnDeltaposSpinVpdmin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIGeneralPage::OnEnChangeEditVpdmin()
{
	SetModified();
}

void CLFIGeneralPage::OnDeltaposSpinVpdmax(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIGeneralPage::OnEnChangeEditVpdmax()
{
	SetModified();
}

void CLFIGeneralPage::OnDeltaposSpinDaylenmin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIGeneralPage::OnEnChangeEditDaylenmin()
{
	SetModified();
}

void CLFIGeneralPage::OnDeltaposSpinDaylenmax(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIGeneralPage::OnEnChangeEditDaylenmax()
{
	SetModified();
}

void CLFIGeneralPage::OnDeltaposSpinDaysAvg(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIGeneralPage::OnEnChangeEditDaysAvg()
{
	SetModified();
}

void CLFIGeneralPage::OnDeltaposSpinPcpDays(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void CLFIGeneralPage::OnEnChangeEditPcpDays()
{
	SetModified();
}

void CLFIGeneralPage::OnBnClickedButtonDefaults()
{
	tminMin = -2.0;
	tminMax = 5.0;
	vpdMin = 900.0;
	vpdMax = 4100.0;
	daylenMin = 36000;
	daylenMax = 39600;
	m_UseVPDavg = false;
	daysAvg = 21;
	daysPcp = 30;
	pcpMin = 0.5;
	pcpMax = 1.5;
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
	m_spinPcpDays.SetPos(daysPcp);
	m_spinPcpMin.SetPos(pcpMin);
	m_spinPcpMax.SetPos(pcpMax);
	SetModified();
}

void CLFIGeneralPage::OnBnClickedButtonStoredDefaults()
{
	CLFISet defSet(pLfiSet->m_pDatabase);
	defSet.m_strFilter.Format("[SIG_Station] = '######'");
	defSet.Open();
	if(!defSet.IsEOF())
	{
		tminMin = defSet.m_TMinMin;
		tminMax = defSet.m_TMinMax;
		vpdMin = defSet.m_VPDMin;
		vpdMax = defSet.m_VPDMax;
		daylenMin = defSet.m_DaylenMin;
		daylenMax = defSet.m_DaylenMax;
		m_UseVPDavg = defSet.m_UseVPDAvg;
		daysAvg = defSet.m_LFIdaysAvg;
		daysPcp = defSet.m_PcpDays;
		pcpMin = defSet.m_PcpMin;
		pcpMax = defSet.m_PcpMax;
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
	m_spinPcpDays.SetPos(daysPcp);
	m_spinPcpMin.SetPos(pcpMin);
	m_spinPcpMax.SetPos(pcpMax);
	SetModified();
}

void CLFIGeneralPage::OnBnClickedButtonSaveDefaults()
{
	CLFISet defSet(pLfiSet->m_pDatabase);
	defSet.m_strFilter.Format("[SIG_Station] = '######'");
	defSet.Open();
	if(!defSet.IsEOF())
		defSet.Edit();
	else
		defSet.AddNew();
	defSet.m_TMinMin = m_spinTminMin.GetPos();
	defSet.m_TMinMax = m_spinTminMax.GetPos();
	defSet.m_VPDMin = m_spinVPDMin.GetPos();
	defSet.m_VPDMax = m_spinVPDMax.GetPos();
	defSet.m_DaylenMin = m_spinDaylenMin.GetPos();
	defSet.m_DaylenMax = m_spinDaylenMax.GetPos();
	int sel = m_comboVPD.GetCurSel();
	defSet.m_UseVPDAvg = sel == 1 ? TRUE : FALSE;
	defSet.m_LFIdaysAvg = m_spinDaysAvg.GetPos();
	defSet.m_PcpDays = m_spinPcpDays.GetPos();
	defSet.m_PcpMin = m_spinPcpMin.GetPos();
	defSet.m_PcpMax = m_spinPcpMax.GetPos();
	defSet.Update();
	defSet.Close();
	SetModified();
}



void CLFIGeneralPage::OnDeltaposSpinPrecipMin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}


void CLFIGeneralPage::OnDeltaposSpinPrecipMax(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}


void CLFIGeneralPage::OnEnChangeEditPrecipMin()
{
	SetModified();
}


void CLFIGeneralPage::OnEnChangeEditPrecipMax()
{
	SetModified();
}


void CLFIGeneralPage::OnCbnSelchangeComboVpdUsage()
{
	SetModified();
}

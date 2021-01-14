// FriskExportDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "FriskExportDialog.h"
#include ".\friskexportdialog.h"
#include <afxdtctl.h>

// CFriskExportDialog dialog

IMPLEMENT_DYNAMIC(CFriskExportDialog, CDialog)
CFriskExportDialog::CFriskExportDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFriskExportDialog::IDD, pParent)
	, m_startTime(COleDateTime::GetCurrentTime())
	, m_endTime(COleDateTime::GetCurrentTime())
	, m_calmVal(0)
	, m_startMonthDay(COleDateTime::GetCurrentTime())
	, m_endMonthDay(COleDateTime::GetCurrentTime())
	, m_monthlyWinds(FALSE)
	, m_generateWindrose(FALSE)
	, m_bUseWeibull(FALSE)
{
	m_fpSet = NULL;
}

CFriskExportDialog::~CFriskExportDialog()
{
}

void CFriskExportDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_startTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_endTime);
	DDX_Control(pDX, IDC_EDITCALM, m_editCalm);
	DDX_Control(pDX, IDC_SPINCALM, m_spinCalm);
	DDX_Text(pDX, IDC_EDITCALM, m_calmVal);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER3, m_startMonthDay);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER4, m_endMonthDay);
	DDX_Control(pDX, IDC_SYEAR, m_StartYear);
	DDX_Control(pDX, IDC_EYEAR, m_EndYear);
	DDX_Control(pDX, IDC_SPIN2, m_spnEyear);
	DDX_Control(pDX, IDC_SPIN1, m_spnSyear);
	DDX_Check(pDX, IDC_CHECK_MONTHLY_WINDS, m_monthlyWinds);
	DDX_Check(pDX, IDC_CHECK_GENWINDROSE, m_generateWindrose);
	DDX_Control(pDX, IDC_CHECK_WEIBULL, m_chkWeibull);
	DDX_Control(pDX, IDC_EDIT_BIN_SIZE, m_editBinSize);
	DDX_Control(pDX, IDC_SPIN_BIN_SIZE, m_spinBinSize);
	DDX_Control(pDX, IDC_EDIT_MAX_SPEED, m_editMaxBin);
	DDX_Control(pDX, IDC_SPIN_MAX_SPEED, m_spinMaxBin);
	DDX_Control(pDX, IDC_RADIO_ABSOLUTE_STATIC, m_radioStaticBin);
	DDX_Control(pDX, IDC_RADIO_USE_MAX, m_radioUseMax);
	DDX_Check(pDX, IDC_CHECK_WEIBULL, m_bUseWeibull);
}


BEGIN_MESSAGE_MAP(CFriskExportDialog, CDialog)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedGust)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedAvg)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedBoth)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BIN_SIZE, &CFriskExportDialog::OnDeltaposSpinBinSize)
	ON_BN_CLICKED(IDC_RADIO_ABSOLUTE_STATIC, &CFriskExportDialog::OnBnClickedRadioAbsoluteStatic)
	ON_BN_CLICKED(IDC_RADIO_USE_MAX, &CFriskExportDialog::OnBnClickedRadioUseMax)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAX_SPEED, &CFriskExportDialog::OnDeltaposSpinMaxSpeed)
	ON_EN_CHANGE(IDC_EDIT_MAX_SPEED, &CFriskExportDialog::OnEnChangeEditMaxSpeed)
	ON_EN_CHANGE(IDC_EDIT_BIN_SIZE, &CFriskExportDialog::OnEnChangeEditBinSize)
END_MESSAGE_MAP()


// CFriskExportDialog message handlers

BOOL CFriskExportDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	windType = 0;
	m_spinCalm.SetRange(0, 15);
	m_startTime.SetTime(0, 0, 0);
	m_endTime.SetTime(0,0, 0);
	CDateTimeCtrl *dt = (CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER1);
	dt->SetFormat("HH:mm");
	dt = (CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER2);
	dt->SetFormat("HH:mm");
	dt = (CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER3);
	dt->SetFormat("MM/dd");
	dt = (CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER4);
	dt->SetFormat("MM/dd");
	((CButton *)(GetDlgItem(IDC_RADIO1)))->SetCheck(TRUE);
	m_maxBinType = MaxBinStatic;
	if(m_fpSet)
	{
		COleDateTime today = COleDateTime::GetCurrentTime();
		m_spnEyear.SetRange(1900, today.GetYear());
		m_spnSyear.SetRange(1900, today.GetYear());
		CString tmp;
		tmp.Format("%d", m_fpSet->m_StartYear);
		m_StartYear.SetWindowText(tmp);
		m_sYear = m_fpSet->m_StartYear;
		tmp.Format("%d", m_fpSet->m_EndYear);
		m_EndYear.SetWindowText(tmp);
		m_eYear = m_fpSet->m_EndYear;
		m_startMonthDay.SetDate(2000, MonthChtoInt(m_fpSet->m_StartMonth), m_fpSet->m_StartDay);
		m_endMonthDay.SetDate(2000, MonthChtoInt(m_fpSet->m_EndMonth), m_fpSet->m_EndDay);
	}
	m_maxBinType = MaxBinStatic;
	m_maxBinSpeed = 30;
	m_binSize = 5;
	m_spinBinSize.SetRange(1, 15);
	m_spinBinSize.SetPos(m_binSize);
	m_spinMaxBin.SetRange(15, 100);
	m_spinMaxBin.SetPos(m_maxBinSpeed);
	m_radioStaticBin.SetCheck(TRUE);
	UpdateData(FALSE);
	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFriskExportDialog::OnOK()
{
	UpdateData();
	CString str1, str2;
	m_StartYear.GetWindowText(str1);
	m_EndYear.GetWindowText(str2);
	m_sYear = atoi(str1);
	m_eYear = atoi(str2);
	if(m_eYear < m_sYear)
	{
		AfxMessageBox("Error: End year must be greater than or equal to start year.");
		return;
	}
	m_iBinSize = m_spinBinSize.GetPos();
	if (m_maxBinType == MaxBinStatic)
		m_iMaxBin = m_spinMaxBin.GetPos();
	else
		m_iMaxBin = -1;
	CDialog::OnOK();
}

void CFriskExportDialog::OnBnClickedGust()
{
	windType = 1;
}

void CFriskExportDialog::OnBnClickedAvg()
{
	windType = 0;
}

void CFriskExportDialog::OnBnClickedBoth()
{
	windType = 2;
}



void CFriskExportDialog::OnBnClickedRadioAbsoluteStatic()
{
	m_maxBinType = MaxBinStatic;
	m_editMaxBin.EnableWindow(TRUE);
	m_spinMaxBin.EnableWindow(TRUE);
}


void CFriskExportDialog::OnBnClickedRadioUseMax()
{
	m_maxBinType = MaxBinObserved;
	m_editMaxBin.EnableWindow(FALSE);
	m_spinMaxBin.EnableWindow(FALSE);
}


void CFriskExportDialog::OnDeltaposSpinMaxSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CFriskExportDialog::OnEnChangeEditMaxSpeed()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CFriskExportDialog::OnDeltaposSpinBinSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CFriskExportDialog::OnEnChangeEditBinSize()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


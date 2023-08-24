// KBDIDIalog.cpp : implementation file
//

#include "pch.h"
#include "NFDRS4Calculator.h"
#include "KBDIDIalog.h"


// CKBDIDIalog dialog

IMPLEMENT_DYNAMIC(CKBDIDIalog, CDialog)
CKBDIDIalog::CKBDIDIalog(CWnd* pParent /*=NULL*/)
	: CDialog(CKBDIDIalog::IDD, pParent)
{
	avgPrecip = 0.0f;
	cumPrecip = 0.0f;
	kbdi = 0;
	maxTemp = 80;
	precip = 0.0f;
	ykbdi = 100;
}

CKBDIDIalog::~CKBDIDIalog()
{
}

void CKBDIDIalog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_APPLY, m_ApplyButton);
	DDX_Control(pDX, IDC_KBDI, m_kbdiBox);
//	DDX_Control(pDX, IDC_NFDRS4CTRL1, m_nfdrs4);
	DDX_Control(pDX, IDC_SPIN_PREVKBDI, m_spinPrevKBDI);
	DDX_Control(pDX, IDC_PREVKBDI, m_prevKBDI);
	DDX_Control(pDX, IDC_SPIN_RAIN, m_spinRain);
	DDX_Control(pDX, IDC_RAIN, m_rain);
	DDX_Control(pDX, IDC_SPIN_MAXTEMP, m_spinMaxTemp);
	DDX_Control(pDX, IDC_MAXTEMP, m_maxTemp);
	DDX_Control(pDX, IDC_SPIN_CUMMPRECIP, m_spinCummPrecip);
	DDX_Control(pDX, IDC_CUMMPRECIP, m_cummPrecip);
	DDX_Control(pDX, IDC_SPIN_AVGPRECIP, m_spinAvgPrecip);
	DDX_Control(pDX, IDC_AVGPRECIP, m_avgPrecip);
}


BEGIN_MESSAGE_MAP(CKBDIDIalog, CDialog)
	ON_BN_CLICKED(IDC_CALC, OnBnClickedCalc)
	ON_BN_CLICKED(IDC_APPLY, OnBnClickedApply)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CKBDIDIalog message handlers

void CKBDIDIalog::OnBnClickedCalc()
{
	avgPrecip = m_spinAvgPrecip.GetPos();
	cumPrecip = m_spinCummPrecip.GetPos();
	maxTemp = m_spinMaxTemp.GetPos();
	precip = m_spinRain.GetPos();
	ykbdi = m_spinPrevKBDI.GetPos();

	kbdi = m_nfdrs4.iCalcKBDI(precip, maxTemp, cumPrecip, ykbdi, avgPrecip);
	CString str;
	str.Format(_T("%ld"), kbdi);
	m_kbdiBox.SetWindowText(str);
	m_ApplyButton.EnableWindow(TRUE);
}

void CKBDIDIalog::OnBnClickedApply()
{
	SaveFields();
	CDialog::OnOK();	
}

BOOL CKBDIDIalog::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect nRect(0, 0, 1, 1);
	//m_nfdrs4.Create(NULL, WS_CHILD,//WS_DISABLED,
	//	nRect, this, IDC_NFDRS3);
	if(theApp.pOptions)
	{
		ykbdi = theApp.pOptions->prevKBDI;
		cumPrecip = theApp.pOptions->cummPrecip;
		avgPrecip = theApp.pOptions->avgPrecip;
		precip = theApp.pOptions->currRain;
		maxTemp = theApp.pOptions->maxTemp;
	}
	m_spinPrevKBDI.SetDecimalPlaces (0);
	m_spinPrevKBDI.SetTrimTrailingZeros (TRUE);
	m_spinPrevKBDI.SetRangeAndDelta (0.0, 2000.0, 1.0);
	m_spinPrevKBDI.SetPos(ykbdi);
	m_spinPrevKBDI.SetBuddy(&m_prevKBDI);

	m_spinCummPrecip.SetDecimalPlaces (2);
	m_spinCummPrecip.SetTrimTrailingZeros (FALSE);
	m_spinCummPrecip.SetRangeAndDelta (0.0, 500.0, 0.1);
	m_spinCummPrecip.SetPos(cumPrecip);
	m_spinCummPrecip.SetBuddy(&m_cummPrecip);

	m_spinAvgPrecip.SetDecimalPlaces (2);
	m_spinAvgPrecip.SetTrimTrailingZeros (FALSE);
	m_spinAvgPrecip.SetRangeAndDelta (0.0, 500.0, 0.1);
	m_spinAvgPrecip.SetPos(avgPrecip);
	m_spinAvgPrecip.SetBuddy(&m_avgPrecip);

	m_spinRain.SetDecimalPlaces (2);
	m_spinRain.SetTrimTrailingZeros (FALSE);
	m_spinRain.SetRangeAndDelta (0.0, 500.0, 0.1);
	m_spinRain.SetPos(precip);
	m_spinRain.SetBuddy(&m_rain);

	m_spinMaxTemp.SetDecimalPlaces (0);
	m_spinMaxTemp.SetTrimTrailingZeros (TRUE);
	m_spinMaxTemp.SetRangeAndDelta (0.0, 200.0, 1.0);
	m_spinMaxTemp.SetPos(maxTemp);
	m_spinMaxTemp.SetBuddy(&m_maxTemp);


	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CKBDIDIalog::OnClose()
{
	SaveFields();
	CDialog::OnClose();
}

void CKBDIDIalog::SaveFields()
{
	if(theApp.pOptions)
	{
		theApp.pOptions->prevKBDI = m_spinPrevKBDI.GetPos();
		theApp.pOptions->cummPrecip = m_spinCummPrecip.GetPos();
		theApp.pOptions->avgPrecip = m_spinAvgPrecip.GetPos();
		theApp.pOptions->currRain = m_spinRain.GetPos();
		theApp.pOptions->maxTemp = m_spinMaxTemp.GetPos();
	}
}

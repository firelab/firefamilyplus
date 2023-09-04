
// NFDRS4CalculatorDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "NFDRS4Calculator.h"
#include "NFDRS4CalculatorDlg.h"
#include "afxdialogex.h"
#include "KBDIDIalog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString verStr = _T("Version 1.0");

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	CString tStr;
	//tStr.Format("%s",verStr);
	tStr.Format(_T("%s Build Date: %s"), verStr, theApp.m_buildDateStr);
	DDX_Text(pDX, IDC_VERSION_STRING_NFDRS4, tStr);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNFDRS4CalculatorDlg dialog



CNFDRS4CalculatorDlg::CNFDRS4CalculatorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NFDRS4CALCULATOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNFDRS4CalculatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_1HRFM, m_spinFM1);
	DDX_Control(pDX, IDC_1HRFM, m_editFM1);
	DDX_Control(pDX, IDC_SPIN_10HRFM, m_spinFM10);
	DDX_Control(pDX, IDC_10HRFM, m_editFM10);
	DDX_Control(pDX, IDC_SPIN_100HRFM, m_spinFM100);
	DDX_Control(pDX, IDC_100HRFM, m_editFM100);
	DDX_Control(pDX, IDC_SPIN_1000HRFM, m_spinFM1000);
	DDX_Control(pDX, IDC_1000HRFM, m_editFM1000);
	DDX_Control(pDX, IDC_SPIN_HERB, m_spinHerb);
	DDX_Control(pDX, IDC_HERB, m_editHerb);
	DDX_Control(pDX, IDC_SPIN_WOODY, m_spinWoody);
	DDX_Control(pDX, IDC_WOODY, m_editWoody);
	DDX_Control(pDX, IDC_SPIN_WIND, m_spinWind);
	DDX_Control(pDX, IDC_WIND, m_editWind);
	DDX_Control(pDX, IDC_SPIN_KBDI2016, m_spinKBDI2016);
	DDX_Control(pDX, IDC_KBDI2016, m_editKBDI2016);
	DDX_Control(pDX, IDC_SPIN_GSI, m_spinGSI);
	DDX_Control(pDX, IDC_GSI, m_editGSI);
	DDX_Control(pDX, IDC_SPIN_GSIMAX, m_spinMaxGSI);
	DDX_Control(pDX, IDC_GSIMAX, m_editMaxGSI);
	DDX_Control(pDX, IDC_SPIN_GSI_THRESHOLD, m_spinGSIThreshold);
	DDX_Control(pDX, IDC_GSI_THRESHOLD, m_editGSIThreshold);
	DDX_CBString(pDX, IDC_SLOPECLASS, m_slopeClass);
	DDX_CBString(pDX, IDC_NFDRSFM, m_fuelModel);
	DDX_Control(pDX, IDC_NFDRSFM, m_comboFuelModel);
	DDX_Control(pDX, IDC_SLOPECLASS, m_comboSlopeClass);
	DDX_Control(pDX, IDC_SPIN_SCM, m_spinSCM);
	//DDX_Control(pDX, IDC_CHECK_HUMID_MXD, m_btnHumid);
	DDX_Control(pDX, IDC_EDIT_SCM, m_editSCM);
	DDX_Control(pDX, IDC_SPIN_FUEL_TEMP, m_spinFuelTemp);
	DDX_Control(pDX, IDC_FUEL_TEMP, m_editFuelTemp);
	DDX_Control(pDX, IDC_MXD_OVERRIDE, m_editMXDOverride);
	DDX_Control(pDX, IDC_SPIN_MXD_OVERRIDE, m_spinMXDOverride);
}

BEGIN_MESSAGE_MAP(CNFDRS4CalculatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CALCULATE, &CNFDRS4CalculatorDlg::OnClickedCalculate)
	ON_BN_CLICKED(IDC_BUTTON_KBDICALC, OnBnClickedButtonKbdicalc)
	ON_BN_CLICKED(IDC_BUTTON_FMPARAMS, OnBnClickedButtonFmparams)
	ON_EN_KILLFOCUS(IDC_1HRFM, &CNFDRS4CalculatorDlg::OnEnKillfocus1hrfm)
	ON_EN_CHANGE(IDC_1HRFM, &CNFDRS4CalculatorDlg::OnEnChange1hrfm)
	ON_EN_KILLFOCUS(IDC_10HRFM, &CNFDRS4CalculatorDlg::OnEnKillfocus10hrfm)
	ON_EN_CHANGE(IDC_10HRFM, &CNFDRS4CalculatorDlg::OnEnChange10hrfm)
	ON_EN_KILLFOCUS(IDC_100HRFM, &CNFDRS4CalculatorDlg::OnEnKillfocus100hrfm)
	ON_EN_CHANGE(IDC_100HRFM, &CNFDRS4CalculatorDlg::OnEnChange100hrfm)
	ON_EN_KILLFOCUS(IDC_1000HRFM, &CNFDRS4CalculatorDlg::OnEnKillfocus1000hrfm)
	ON_EN_CHANGE(IDC_1000HRFM, &CNFDRS4CalculatorDlg::OnEnChange1000hrfm)
	ON_EN_KILLFOCUS(IDC_HERB, &CNFDRS4CalculatorDlg::OnEnKillfocusHerbfm)
	ON_EN_CHANGE(IDC_HERB, &CNFDRS4CalculatorDlg::OnEnChangeHerbfm)
	ON_EN_KILLFOCUS(IDC_WOODY, &CNFDRS4CalculatorDlg::OnEnKillfocusWoodyfm)
	ON_EN_CHANGE(IDC_WOODY, &CNFDRS4CalculatorDlg::OnEnChangeWoodyfm)
	ON_EN_KILLFOCUS(IDC_WIND, &CNFDRS4CalculatorDlg::OnEnKillfocusWind)
	ON_EN_CHANGE(IDC_WIND, &CNFDRS4CalculatorDlg::OnEnChangeWind)
	ON_EN_CHANGE(IDC_GSI, &CNFDRS4CalculatorDlg::OnChangeGsi)
	ON_EN_KILLFOCUS(IDC_GSI, &CNFDRS4CalculatorDlg::OnKillfocusGsi)
	ON_EN_CHANGE(IDC_GSI_THRESHOLD, &CNFDRS4CalculatorDlg::OnChangeGsiThreshold)
	ON_EN_KILLFOCUS(IDC_GSI_THRESHOLD, &CNFDRS4CalculatorDlg::OnKillfocusGsiThreshold)
	ON_EN_CHANGE(IDC_FUEL_TEMP, &CNFDRS4CalculatorDlg::OnChangeFuelTemp)
	ON_EN_KILLFOCUS(IDC_FUEL_TEMP, &CNFDRS4CalculatorDlg::OnKillfocusFuelTemp)
	ON_EN_CHANGE(IDC_GSIMAX, &CNFDRS4CalculatorDlg::OnChangeGsimax)
	ON_EN_KILLFOCUS(IDC_GSIMAX, &CNFDRS4CalculatorDlg::OnKillfocusGsimax)
	ON_EN_CHANGE(IDC_KBDI2016, &CNFDRS4CalculatorDlg::OnChangeKbdi2016)
	ON_EN_KILLFOCUS(IDC_KBDI2016, &CNFDRS4CalculatorDlg::OnKillfocusKbdi2016)
	ON_EN_CHANGE(IDC_EDIT_SCM, &CNFDRS4CalculatorDlg::OnChangeEditScm)
	ON_EN_KILLFOCUS(IDC_EDIT_SCM, &CNFDRS4CalculatorDlg::OnKillfocusEditScm)
	ON_EN_CHANGE(IDC_MXD_OVERRIDE, &CNFDRS4CalculatorDlg::OnChangeMxdOverride)
	ON_EN_KILLFOCUS(IDC_MXD_OVERRIDE, &CNFDRS4CalculatorDlg::OnKillfocusMxdOverride)
END_MESSAGE_MAP()


// CNFDRS4CalculatorDlg message handlers

BOOL CNFDRS4CalculatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_kbdi2016 = 100;
	m_fm1 = 2.0;
	m_fm10 = 2.0;
	m_fm100 = 20.0;
	m_fm1000 = 25.0;
	m_fmHerb = 150;
	m_fmWood = 100;
	m_windSpeed = 5;
	m_slopeClass = "1 : 0 - 25%";
	m_gsi = 0.5;
	m_gsiMax = 1.0;
	m_gsiThreshold = 0.5;
	m_fuelTemperature = 80;

	m_spinFM1.SetDecimalPlaces(2);
	m_spinFM1.SetTrimTrailingZeros(FALSE);
	m_spinFM1.SetRangeAndDelta(1.0, 65.0, 1.0);
	m_spinFM1.SetPos(m_fm1);
	m_spinFM1.SetBuddy(&m_editFM1);
	m_spinFM10.SetDecimalPlaces(2);
	m_spinFM10.SetTrimTrailingZeros(FALSE);
	m_spinFM10.SetRangeAndDelta(1.0, 65.0, 1.0);
	m_spinFM10.SetPos(m_fm10);
	m_spinFM10.SetBuddy(&m_editFM10);
	m_spinFM100.SetDecimalPlaces(2);
	m_spinFM100.SetTrimTrailingZeros(FALSE);
	m_spinFM100.SetRangeAndDelta(1.0, 65.0, 1.0);
	m_spinFM100.SetPos(m_fm100);
	m_spinFM100.SetBuddy(&m_editFM100);
	m_spinFM1000.SetDecimalPlaces(2);
	m_spinFM1000.SetTrimTrailingZeros(FALSE);
	m_spinFM1000.SetRangeAndDelta(1.0, 65.0, 1.0);
	m_spinFM1000.SetPos(m_fm1000);
	m_spinFM1000.SetBuddy(&m_editFM1000);
	m_spinHerb.SetDecimalPlaces(2);
	m_spinHerb.SetTrimTrailingZeros(FALSE);
	m_spinHerb.SetRangeAndDelta(30.0, 250.0, 1.0);
	m_spinHerb.SetPos(m_fmHerb);
	m_spinHerb.SetBuddy(&m_editHerb);
	m_spinWoody.SetDecimalPlaces(2);
	m_spinWoody.SetTrimTrailingZeros(FALSE);
	m_spinWoody.SetRangeAndDelta(50.0, 200.0, 1.0);
	m_spinWoody.SetPos(m_fmWood);
	m_spinWoody.SetBuddy(&m_editWoody);
	m_spinWind.SetDecimalPlaces(0);
	m_spinWind.SetTrimTrailingZeros(TRUE);
	m_spinWind.SetRangeAndDelta(0.0, 99.0, 1.0);
	m_spinWind.SetPos(m_windSpeed);
	m_spinWind.SetBuddy(&m_editWind);
	m_spinKBDI2016.SetDecimalPlaces(0);
	m_spinKBDI2016.SetTrimTrailingZeros(TRUE);
	m_spinKBDI2016.SetRangeAndDelta(0.0, 800.0, 1);
	m_spinKBDI2016.SetPos(m_kbdi2016);
	m_spinKBDI2016.SetBuddy(&m_editKBDI2016);
	m_spinGSI.SetDecimalPlaces(2);
	m_spinGSI.SetTrimTrailingZeros(TRUE);
	m_spinGSI.SetRangeAndDelta(0.0, 1.0, 0.01);
	m_spinGSI.SetPos(m_gsi);
	m_spinGSI.SetBuddy(&m_editGSI);
	m_spinMaxGSI.SetDecimalPlaces(2);
	m_spinMaxGSI.SetTrimTrailingZeros(TRUE);
	m_spinMaxGSI.SetRangeAndDelta(0.0, 1.0, 0.01);
	m_spinMaxGSI.SetPos(m_gsiMax);
	m_spinMaxGSI.SetBuddy(&m_editMaxGSI);
	m_spinGSIThreshold.SetDecimalPlaces(2);
	m_spinGSIThreshold.SetTrimTrailingZeros(TRUE);
	m_spinGSIThreshold.SetRangeAndDelta(0.0, 1.0, 0.01);
	m_spinGSIThreshold.SetPos(m_gsiThreshold);
	m_spinGSIThreshold.SetBuddy(&m_editGSIThreshold);
	m_spinSCM.SetRange(1, 1000);
	m_spinSCM.SetPos(30);
	m_btnHumid.SetCheck(FALSE);
	m_spinFuelTemp.SetDecimalPlaces(0);
	m_spinFuelTemp.SetTrimTrailingZeros(TRUE);
	m_spinFuelTemp.SetRangeAndDelta(-50.0, 150.0, 1.0);
	m_spinFuelTemp.SetPos(m_fuelTemperature);
	m_spinFuelTemp.SetBuddy(&m_editFuelTemp);
	m_spinMXDOverride.SetRange(0, 100);
	m_spinMXDOverride.SetPos(0);
	m_editMXDOverride.SetWindowTextW(_T(""));
	if (theApp.pOptions)
	{
		CString tmpStr;
		CStringA ascStr;
		ascStr.Format("%c", theApp.pOptions->fuelModel);
		tmpStr = ascStr;
		m_comboFuelModel.SelectString(-1, tmpStr);
		//m_comboFuelModel.GetWindowText(tmpStr);
		m_fuelModel = tmpStr;
		tmpStr.Format(_T("%d"), theApp.pOptions->slopeClass);
		m_comboSlopeClass.SelectString(-1, tmpStr);
		m_comboSlopeClass.GetWindowText(m_slopeClass);
		m_spinWind.SetPos(theApp.pOptions->windSpeed);
		m_spinFM1.SetPos(theApp.pOptions->fm1);
		m_spinFM10.SetPos(theApp.pOptions->fm10);
		m_spinFM100.SetPos(theApp.pOptions->fm100);
		m_spinFM1000.SetPos(theApp.pOptions->fm1000);
		m_spinHerb.SetPos(theApp.pOptions->herb);
		m_spinWoody.SetPos(theApp.pOptions->woody);

	}
	else
	{
		m_comboFuelModel.SetCurSel(6);
		m_comboFuelModel.GetWindowText(m_fuelModel);
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNFDRS4CalculatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNFDRS4CalculatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNFDRS4CalculatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CNFDRS4CalculatorDlg::OnClickedCalculate()
{
	UpdateData();
	if (m_fuelModel.GetLength() <= 0)
	{
		AfxMessageBox(_T("ERROR: A fuel model must be selected!"));
		return;
	}
	if (m_slopeClass.GetLength() <= 0)
	{
		AfxMessageBox(_T("ERROR: A slope class must be selected!"));
		return;
	}
	//do other error checking validation
	m_fm1 = m_spinFM1.GetPos();
	if (m_fm1 < 1.0 || m_fm1 > 65.0)
	{
		AfxMessageBox(_T("ERROR: 1 - Hr FM must be betweeen 1.0 and 65.0"));
		m_editFM1.SetFocus();
		return;
	}
	m_fm10 = m_spinFM10.GetPos();
	if (m_fm10 < 1.0 || m_fm10 > 65.0)
	{
		AfxMessageBox(_T("ERROR: 10 - Hr FM must be betweeen 1.0 and 65.0"));
		m_editFM10.SetFocus();
		return;
	}
	m_fm100 = m_spinFM100.GetPos();
	if (m_fm100 < 1.0 || m_fm100 > 65.0)
	{
		AfxMessageBox(_T("ERROR: 100 - Hr FM must be betweeen 1.0 and 65.0"));
		m_editFM100.SetFocus();
		return;
	}
	m_fm1000 = m_spinFM1000.GetPos();
	if (m_fm1000 < 1.0 || m_fm1000 > 65.0)
	{
		AfxMessageBox(_T("ERROR: 1000 - Hr FM must be betweeen 1.0 and 65.0"));
		m_editFM1000.SetFocus();
		return;
	}
	m_fmHerb = m_spinHerb.GetPos();
	if (m_fmHerb < 30.0 || m_fmHerb > 250.0)
	{
		AfxMessageBox(_T("ERROR: Herb FM must be betweeen 30.0 and 250.0"));
		m_editHerb.SetFocus();
		return;
	}
	m_fmWood = m_spinWoody.GetPos();
	if (m_fmWood < 50.0 || m_fmWood > 200.0)
	{
		AfxMessageBox(_T("ERROR: Woody FM must be betweeen 50.0 and 200.0"));
		m_editWoody.SetFocus();
		return;
	}
	m_windSpeed = m_spinWind.GetPos();
	if (m_windSpeed < 0.0 || m_windSpeed > 99.0)
	{
		AfxMessageBox(_T("ERROR: 20' Wind must be betweeen 0.0 and 990.0"));
		m_editWind.SetFocus();
		return;
	}
	m_fuelTemperature = m_spinFuelTemp.GetPos();
	if (m_fuelTemperature < -50.0 || m_fuelTemperature > 150.0)
	{
		AfxMessageBox(_T("ERROR: Fuel Temperature must be betweeen -50.0 and 150.0"));
		m_editFuelTemp.SetFocus();
		return;
	}

	m_gsi = m_spinGSI.GetPos();
	if (m_gsi < 0.0 || m_gsi > 1.0)
	{
		AfxMessageBox(_T("ERROR: GSI must be betweeen 0.0 and 1.0"));
		m_editGSI.SetFocus();
		return;
	}

	m_kbdi2016 = m_spinKBDI2016.GetPos();
	if (m_kbdi2016 < 0 || m_kbdi2016 > 800)
	{
		AfxMessageBox(_T("ERROR: KBDI must be betweeen 0 and 800"));
		m_editKBDI2016.SetFocus();
		return;
	}
	m_gsiMax = m_spinMaxGSI.GetPos();
	if (m_gsiMax < 0.0 || m_gsiMax > 1.0)
	{
		AfxMessageBox(_T("ERROR: Max GSI must be betweeen 0.0 and 1.0"));
		m_editMaxGSI.SetFocus();
		return;
	}
	m_gsiThreshold = m_spinGSIThreshold.GetPos();
	if (m_gsiThreshold < 0.0 || m_gsiThreshold > 1.0)
	{
		AfxMessageBox(_T("ERROR: GSI Greenup Threshold must be betweeen 0.0 and 1.0"));
		m_editGSIThreshold.SetFocus();
		return;
	}
	if (m_spinSCM.GetPos() < 1 || m_spinSCM.GetPos() > 1000)
	{
		AfxMessageBox(_T("ERROR: SCM must be betweeen 1 and 1000"));
		m_editSCM.SetFocus();
		return;
	}
	//calculate the values
	double fSC, fERC, fBI, fIC;
	CStringA fmStr(m_fuelModel);
	m_nfdrs2016.Init(45, fmStr[0], _wtoi(m_slopeClass), 30, true, true, false, 100);// , gsiMax, gsiThreshold);
	m_nfdrs2016.iSetFuelModel(fmStr[0]);
	m_nfdrs2016.SetSCMax(m_spinSCM.GetPos());
	if (m_spinMXDOverride.GetPos() > 0)
		m_nfdrs2016.SetMXD(m_spinMXDOverride.GetPos());
	//m_nfdrs2016.SetMxdHumid(m_btnHumid.GetCheck());
	m_nfdrs2016.SetHerbGSIparams(m_gsiMax, m_gsiThreshold);
	//need fuel temp in C
	double fTempC;
	fTempC = (m_fuelTemperature - 32) / 1.8;
	m_nfdrs2016.iSetFuelMoistures(m_fm1, m_fm10, m_fm100, m_fm1000, m_fmWood, m_fmHerb, fTempC);
	m_nfdrs2016.iCalcIndexes(m_windSpeed, _wtoi(m_slopeClass), &fSC, &fERC, &fBI, &fIC, m_gsi, m_kbdi2016);// , &iBI);
	m_sc = fSC;
	m_erc = fERC;
	m_bi = fBI;
	m_ic = fIC;
	CString fmtStr;
	fmtStr.Format(_T("%.2lf"), m_sc);
	GetDlgItem(IDC_SC)->SetWindowText(fmtStr);
	fmtStr.Format(_T("%.2lf"), m_erc);
	GetDlgItem(IDC_ERC)->SetWindowText(fmtStr);
	fmtStr.Format(_T("%.2lf"), m_bi);
	GetDlgItem(IDC_BI)->SetWindowText(fmtStr);
	fmtStr.Format(_T("%.2lf"), m_ic);
	GetDlgItem(IDC_IC)->SetWindowText(fmtStr);
	UpdateData(false);

}

void CNFDRS4CalculatorDlg::OnEnKillfocus1hrfm()
{
	CString str;
	m_editFM1.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		AfxMessageBox(_T("Error: 1 - Hr FM can not be blank"));
		m_editFM1.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnEnChange1hrfm()
{
	CString str;
	m_editFM1.GetWindowText(str);
	double fm1 = _wtof(str);
	if (str.GetLength() > 1 && (fm1 < 1.0 || fm1 > 65.0))
	{
		AfxMessageBox(_T("Error: 1 - Hr FM must be between 1.0 and 65.0"));
		m_editFM1.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnEnKillfocus10hrfm()
{
	CString str;
	m_editFM10.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		AfxMessageBox(_T("Error: 10 - Hr FM can not be blank"));
		m_editFM10.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnEnChange10hrfm()
{
	CString str;
	m_editFM10.GetWindowText(str);
	double fm10 = _wtof(str);
	if (str.GetLength() > 1 && (fm10 < 1.0 || fm10 > 65.0))
	{
		AfxMessageBox(_T("Error: 10 - Hr FM must be between 1.0 and 65.0"));
		m_editFM10.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnEnKillfocus100hrfm()
{
	CString str;
	m_editFM100.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		AfxMessageBox(_T("Error: 100 - Hr FM can not be blank"));
		m_editFM100.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnEnChange100hrfm()
{
	CString str;
	m_editFM100.GetWindowText(str);
	double fm100 = _wtof(str);
	if (str.GetLength() > 1 && (fm100 < 1.0 || fm100 > 65.0))
	{
		AfxMessageBox(_T("Error: 100 - Hr FM must be between 1.0 and 65.0"));
		m_editFM100.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnEnKillfocus1000hrfm()
{
	CString str;
	m_editFM1000.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		AfxMessageBox(_T("Error: 1000 - Hr FM can not be blank"));
		m_editFM1000.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnEnChange1000hrfm()
{
	CString str;
	m_editFM1000.GetWindowText(str);
	double fm1 = _wtof(str);
	if (str.GetLength() > 1 && (fm1 < 1.0 || fm1 > 65.0))
	{
		AfxMessageBox(_T("Error: 1000 - Hr FM must be between 1.0 and 65.0"));
		m_editFM1000.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnEnKillfocusHerbfm()
{
	CString str;
	m_editHerb.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		AfxMessageBox(_T("Error: Herb FM can not be blank"));
		m_editHerb.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnEnChangeHerbfm()
{
	CString str;
	m_editHerb.GetWindowText(str);
	double fmHerb = _wtof(str);
	if (str.GetLength() > 2 && (fmHerb < 30.0 || fmHerb > 250.0))
	{
		AfxMessageBox(_T("Error: Herb FM must be between 30.0 and 250.0"));
		m_editHerb.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnEnKillfocusWoodyfm()
{
	CString str;
	m_editWoody.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		AfxMessageBox(_T("Error: Woody FM can not be blank"));
		m_editWoody.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnEnChangeWoodyfm()
{
	CString str;
	m_editWoody.GetWindowText(str);
	double fmWoody = _wtof(str);
	if (str.GetLength() > 2 && (fmWoody < 50.0 || fmWoody > 200.0))
	{
		AfxMessageBox(_T("Error: Woody FM must be between 50.0 and 200.0"));
		m_editWoody.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnEnKillfocusWind()
{
	CString str;
	m_editWind.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		AfxMessageBox(_T("Error: 20' Wind can not be blank"));
		m_editWind.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnEnChangeWind()
{
	CString str;
	m_editWind.GetWindowText(str);
	double val = _wtof(str);
	if (str.GetLength() > 1 && (val < 0.0 || val > 99.0))
	{
		AfxMessageBox(_T("Error: 20' Wind must be between 0 and 99.0"));
		m_editWind.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnChangeGsi()
{
	CString str;
	m_editGSI.GetWindowText(str);
	double val = _wtof(str);
	if (str.GetLength() >= 1 && (val < 0.0 || val > 1.0))
	{
		AfxMessageBox(_T("Error: GSI must be between 0 and 1.0"));
		m_editGSI.SetFocus();
	}
}


void CNFDRS4CalculatorDlg::OnKillfocusGsi()
{
	CString str;
	m_editGSI.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		AfxMessageBox(_T("Error: GSI can not be blank"));
		m_editGSI.SetFocus();
	}
}


void CNFDRS4CalculatorDlg::OnChangeGsiThreshold()
{
	CString str;
	m_editGSIThreshold.GetWindowText(str);
	double val = _wtof(str);
	if (str.GetLength() >= 1 && (val < 0.0 || val > 1.0))
	{
		AfxMessageBox(_T("Error: GSI Greenup Threshold must be between 0 and 1.0"));
		m_editGSIThreshold.SetFocus();
	}
}


void CNFDRS4CalculatorDlg::OnKillfocusGsiThreshold()
{
	CString str;
	m_editGSIThreshold.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		AfxMessageBox(_T("Error: GSI Greenup Threshold can not be blank"));
		m_editGSIThreshold.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnChangeFuelTemp()
{
	CString str;
	m_editFuelTemp.GetWindowText(str);
	double val = _wtof(str);
	if (str.GetLength() >= 1 && (val < -50.0 || val > 150))
	{
		AfxMessageBox(_T("Error: Fuel Temperature must be between -50 and 150"));
		m_editFuelTemp.SetFocus();
	}
}


void CNFDRS4CalculatorDlg::OnKillfocusFuelTemp()
{
	CString str;
	m_editFuelTemp.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		AfxMessageBox(_T("Error: Fuel Temperature can not be blank"));
		m_editFuelTemp.SetFocus();
	}
}


void CNFDRS4CalculatorDlg::OnChangeGsimax()
{
	CString str;
	m_editMaxGSI.GetWindowText(str);
	double val = _wtof(str);
	if (str.GetLength() >= 1 && (val < 0.0 || val > 1.0))
	{
		AfxMessageBox(_T("Error: Max GSI must be between 0 and 1.0"));
		m_editMaxGSI.SetFocus();
	}
}


void CNFDRS4CalculatorDlg::OnKillfocusGsimax()
{
	CString str;
	m_editMaxGSI.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		AfxMessageBox(_T("Error: Max GSI can not be blank"));
		m_editMaxGSI.SetFocus();
	}
}


void CNFDRS4CalculatorDlg::OnChangeKbdi2016()
{
	CString str;
	m_editKBDI2016.GetWindowText(str);
	double val = _wtof(str);
	if (str.GetLength() > 1 && (val < 0.0 || val > 800))
	{
		AfxMessageBox(_T("Error: KBDI must be between 0 and 800"));
		m_editKBDI2016.SetFocus();
	}
}


void CNFDRS4CalculatorDlg::OnKillfocusKbdi2016()
{
	CString str;
	m_editKBDI2016.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		AfxMessageBox(_T("Error: KBDI can not be blank"));
		m_editKBDI2016.SetFocus();
	}
}


void CNFDRS4CalculatorDlg::OnChangeEditScm()
{
	CString str;
	m_editSCM.GetWindowText(str);
	double val = _wtof(str);
	if (str.GetLength() > 1 && (val < 1.0 || val > 1000.0))
	{
		AfxMessageBox(_T("Error: SCM must be between 0 and 1000"));
		m_editSCM.SetFocus();
	}
}


void CNFDRS4CalculatorDlg::OnKillfocusEditScm()
{
	CString str;
	m_editSCM.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		AfxMessageBox(_T("Error: SCM can not be blank"));
		m_editSCM.SetFocus();
	}
}

void CNFDRS4CalculatorDlg::OnBnClickedButtonKbdicalc()
{
	CKBDIDIalog kbdiDlg(this);
	if (kbdiDlg.DoModal() == IDOK)
	{
		m_kbdi2016 = kbdiDlg.kbdi;
		m_spinKBDI2016.SetPos(m_kbdi2016);
	}
}

void CNFDRS4CalculatorDlg::OnBnClickedButtonFmparams()
{
	if (!pFMParamsDlg)
	{
		pFMParamsDlg = new CFMParamsDlg(this);
		pFMParamsDlg->Create(CFMParamsDlg::IDD, this);
	}
	pFMParamsDlg->ShowWindow(SW_SHOW);
}


CalcOptions::CalcOptions()
{
	fuelModel = 'G';
	slopeClass = 1;
	windSpeed = 5;
	fm1 = 4;
	fm10 = 5;
	fm100 = 10;
	fm1000 = 12;
	herb = 30;
	woody = 60;
	KBDI = 100;
	rainEvent = 0;
	deciduousShrubs = 0;
	prevKBDI = 100;
	currRain = 0.0;
	maxTemp = 87;
	cummPrecip = 9.0;
	avgPrecip = 18.0;

}

CalcOptions::~CalcOptions()
{
}


void CNFDRS4CalculatorDlg::OnChangeMxdOverride()
{
	CString str;
	m_editMXDOverride.GetWindowText(str);
	if (str.GetLength() > 0)// && (val < 0.0 || val > 800))
	{
		int val = _wtoi(str);
		if (val < 1 || val > 100)
		{
			AfxMessageBox(_T("Error: MXD Override must be between 1 and 100, or blank"));
			m_editMXDOverride.SetFocus();
		}
	}
}


void CNFDRS4CalculatorDlg::OnKillfocusMxdOverride()
{
	// TODO: Add your control notification handler code here
}

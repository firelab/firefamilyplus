// NFDRSCalculator2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "NFDRSCalculator2.h"
#include "NFDRSCalculator2Dlg.h"
//#include ".\nfdrscalculator2dlg.h"
#include "KBDIDIalog.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//extern CNFDRSCalculator2App theApp;
static char verStr[] = "Version 1.0";

double Round(double in)
{
	double ret = floor(in), ret2 = ceil(in);
	//TRACE2("In Round: ret = %.2f, ret2 = %.2f\n", ret, ret2);
	if(in - ret >= 0.5)
		return ret2;
	return ret;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	CString tStr;
	//tStr.Format("%s",verStr);
	tStr.Format("%s Build Date: %s", verStr, theApp.m_buildDateStr);
	DDX_Text(pDX, IDC_VERSION_STRING_NFDRS2016, tStr);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CNFDRSCalculator2Dlg dialog



CNFDRSCalculator2Dlg::CNFDRSCalculator2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNFDRSCalculator2Dlg::IDD, pParent)
	//, m_fFM1(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	bi = sc = erc = ic = 0.0;
	rainEvent = FALSE;
	use88 = FALSE;
	deciduous = FALSE;
	sow = _T("");
	season = _T("");
	slopeClass = _T("");
	fuelModel = _T("");
	pFMParamsDlg = NULL;
}

CNFDRSCalculator2Dlg::~CNFDRSCalculator2Dlg()
{
	if(pFMParamsDlg)
	{
		if(pFMParamsDlg->m_hWnd)
			pFMParamsDlg->DestroyWindow();
		delete pFMParamsDlg;
	}
}
void CNFDRSCalculator2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
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
	DDX_Control(pDX, IDC_SPIN_TEMP, m_spinTemp);
	DDX_Control(pDX, IDC_TEMP, m_editTemp);
	DDX_Control(pDX, IDC_SPIN_WIND, m_spinWind);
	DDX_Control(pDX, IDC_WIND, m_editWind);
	DDX_Control(pDX, IDC_SPIN_WOODYGREEN, m_spinWoodyGreen);
	DDX_Control(pDX, IDC_WOODYGREEN, m_editWoodyGreen);
	DDX_Control(pDX, IDC_SPIN_KBDI, m_spinKBDI);
	DDX_Control(pDX, IDC_SPIN_KBDI2016, m_spinKBDI2016);
	DDX_Control(pDX, IDC_KBDI, m_editKBDI);
	DDX_Control(pDX, IDC_KBDI2016, m_editKBDI2016);
	DDX_Control(pDX, IDC_SPIN_GSI, m_spinGSI);
	DDX_Control(pDX, IDC_GSI, m_editGSI);
	DDX_Control(pDX, IDC_SPIN_GSIMAX, m_spinMaxGSI);
	DDX_Control(pDX, IDC_GSIMAX, m_editMaxGSI);
	DDX_Control(pDX, IDC_SPIN_GSI_THRESHOLD, m_spinGSIThreshold);
	DDX_Control(pDX, IDC_GSI_THRESHOLD, m_editGSIThreshold);
	DDX_Control(pDX, IDC_SEASON, seasonCtl);
	DDX_Control(pDX, IDC_RAINEVENT, rainCtl);
	DDX_Control(pDX, IDC_DECIDUOUS, decidCtl);
	DDX_Control(pDX, IDC_USE88, m_btnUse88);
	DDX_CBString(pDX, IDC_SOW, sow);
	DDX_CBString(pDX, IDC_SEASON, season);
	DDX_CBString(pDX, IDC_SLOPECLASS, slopeClass);
	DDX_CBString(pDX, IDC_NFDRSFM, fuelModel);
	DDX_Check(pDX, IDC_RAINEVENT, rainEvent);
	DDX_Check(pDX, IDC_USE88, use88);
	DDX_Check(pDX, IDC_DECIDUOUS, deciduous);
	//DDX_Text(pDX, IDC_BI, bi);
	//DDX_Text(pDX, IDC_ERC, erc);
	//DDX_Text(pDX, IDC_IC, ic);
	//DDX_Text(pDX, IDC_SC, sc);
	DDX_Control(pDX, IDC_SEASONSTATIC, static1);
	DDX_Control(pDX, IDC_WOODYGREENSTATIC, static2);
	DDX_Control(pDX, IDC_KBDISTATIC, static3);
	DDX_Control(pDX, 1019, static4);
	DDX_Control(pDX, IDC_NFDRSFM, m_comboFuelModel);
	DDX_Control(pDX, IDC_SLOPECLASS, m_comboSlopeClass);
	DDX_Control(pDX, IDC_SOW, m_comboSOW);
	DDX_Control(pDX, IDC_SPIN_SCM, m_spinSCM);
	DDX_Control(pDX, IDC_CHECK_HUMID_MXD, m_btnHumid);
	DDX_Control(pDX, IDC_EDIT_SCM, m_editSCM);
	DDX_Control(pDX, IDC_SPIN_FUEL_TEMP, m_spinFuelTemp);
	DDX_Control(pDX, IDC_FUEL_TEMP, m_editFuelTemp);
}

BEGIN_MESSAGE_MAP(CNFDRSCalculator2Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_NFDRSFM, OnCbnSelchangeNfdrsfm)
	ON_BN_CLICKED(IDC_USE88, OnBnClickedUse88)
	ON_BN_CLICKED(IDC_CALCULATE, OnBnClickedCalculate)
	ON_BN_CLICKED(IDC_BUTTON_FMPARAMS, OnBnClickedButtonFmparams)
	ON_BN_CLICKED(IDC_BUTTON_KBDICALC, OnBnClickedButtonKbdicalc)
	ON_WM_CLOSE()
	ON_EN_KILLFOCUS(IDC_1HRFM, &CNFDRSCalculator2Dlg::OnEnKillfocus1hrfm)
	ON_EN_CHANGE(IDC_1HRFM, &CNFDRSCalculator2Dlg::OnEnChange1hrfm)
	ON_EN_KILLFOCUS(IDC_10HRFM, &CNFDRSCalculator2Dlg::OnEnKillfocus10hrfm)
	ON_EN_CHANGE(IDC_10HRFM, &CNFDRSCalculator2Dlg::OnEnChange10hrfm)
	ON_EN_KILLFOCUS(IDC_100HRFM, &CNFDRSCalculator2Dlg::OnEnKillfocus100hrfm)
	ON_EN_CHANGE(IDC_100HRFM, &CNFDRSCalculator2Dlg::OnEnChange100hrfm)
	ON_EN_KILLFOCUS(IDC_1000HRFM, &CNFDRSCalculator2Dlg::OnEnKillfocus1000hrfm)
	ON_EN_CHANGE(IDC_1000HRFM, &CNFDRSCalculator2Dlg::OnEnChange1000hrfm)
	ON_EN_KILLFOCUS(IDC_HERB, &CNFDRSCalculator2Dlg::OnEnKillfocusHerbfm)
	ON_EN_CHANGE(IDC_HERB, &CNFDRSCalculator2Dlg::OnEnChangeHerbfm)
	ON_EN_KILLFOCUS(IDC_WOODY, &CNFDRSCalculator2Dlg::OnEnKillfocusWoodyfm)
	ON_EN_CHANGE(IDC_WOODY, &CNFDRSCalculator2Dlg::OnEnChangeWoodyfm)
	ON_EN_KILLFOCUS(IDC_WOODYGREEN, &CNFDRSCalculator2Dlg::OnEnKillfocusWoodyGreen)
	ON_EN_CHANGE(IDC_WOODYGREEN, &CNFDRSCalculator2Dlg::OnEnChangeWoodyGreen)
	ON_EN_KILLFOCUS(IDC_TEMP, &CNFDRSCalculator2Dlg::OnEnKillfocusTemp)
	ON_EN_CHANGE(IDC_TEMP, &CNFDRSCalculator2Dlg::OnEnChangeTemp)
	ON_EN_KILLFOCUS(IDC_WIND, &CNFDRSCalculator2Dlg::OnEnKillfocusWind)
	ON_EN_CHANGE(IDC_WIND, &CNFDRSCalculator2Dlg::OnEnChangeWind)
END_MESSAGE_MAP()


// CNFDRSCalculator2Dlg message handlers

BOOL CNFDRSCalculator2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect nRect(0, 0, 1, 1);

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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

	kbdi = kbdi2016 = 100;
	woodyGreen = 20;
	fm1 = 2.0;
	fm10 = 2.0;
	fm100 = 20.0;
	fm1000 = 25.0;
	fmHerb = 150;
	fmWood = 100;
	windSpeed = 5;
	temperature = 80;
	sow = "1 - Scattered Clouds";
	season = "3 - Summer";
	slopeClass = "1 : 0 - 25%";
	gsi = 0.5;
	gsiMax = 1.0;
	gsiThreshold = 0.5;
	fuelTemperature = 80;
	m_spinFM1.SetDecimalPlaces (2);
	m_spinFM1.SetTrimTrailingZeros (FALSE);
	m_spinFM1.SetRangeAndDelta (1.0, 65.0, 1.0);
	m_spinFM1.SetPos(fm1);
	m_spinFM1.SetBuddy(&m_editFM1);
	m_spinFM10.SetDecimalPlaces (2);
	m_spinFM10.SetTrimTrailingZeros (FALSE);
	m_spinFM10.SetRangeAndDelta (1.0, 65.0, 1.0);
	m_spinFM10.SetPos(fm10);
	m_spinFM10.SetBuddy(&m_editFM10);
	m_spinFM100.SetDecimalPlaces (2);
	m_spinFM100.SetTrimTrailingZeros (FALSE);
	m_spinFM100.SetRangeAndDelta (1.0, 65.0, 1.0);
	m_spinFM100.SetPos(fm100);
	m_spinFM100.SetBuddy(&m_editFM100);
	m_spinFM1000.SetDecimalPlaces (2);
	m_spinFM1000.SetTrimTrailingZeros (FALSE);
	m_spinFM1000.SetRangeAndDelta (1.0, 65.0, 1.0);
	m_spinFM1000.SetPos(fm1000);
	m_spinFM1000.SetBuddy(&m_editFM1000);
	m_spinHerb.SetDecimalPlaces (2);
	m_spinHerb.SetTrimTrailingZeros (FALSE);
	m_spinHerb.SetRangeAndDelta (30.0, 250.0, 1.0);
	m_spinHerb.SetPos(fmHerb);
	m_spinHerb.SetBuddy(&m_editHerb);
	m_spinWoody.SetDecimalPlaces (2);
	m_spinWoody.SetTrimTrailingZeros (FALSE);
	m_spinWoody.SetRangeAndDelta (50.0, 200.0, 1.0);
	m_spinWoody.SetPos(fmWood);
	m_spinWoody.SetBuddy(&m_editWoody);
	m_spinTemp.SetDecimalPlaces (0);
	m_spinTemp.SetTrimTrailingZeros (TRUE);
	m_spinTemp.SetRangeAndDelta (-20.0, 120.0, 1.0);
	m_spinTemp.SetPos(temperature);
	m_spinTemp.SetBuddy(&m_editTemp);
	m_spinWind.SetDecimalPlaces (0);
	m_spinWind.SetTrimTrailingZeros (TRUE);
	m_spinWind.SetRangeAndDelta (0.0, 99.0, 1.0);
	m_spinWind.SetPos(windSpeed);
	m_spinWind.SetBuddy(&m_editWind);
	m_spinWoodyGreen.SetDecimalPlaces (0);
	m_spinWoodyGreen.SetTrimTrailingZeros (FALSE);
	m_spinWoodyGreen.SetRangeAndDelta (0.0, 20.0, 1.0);
	m_spinWoodyGreen.SetPos(woodyGreen);
	m_spinWoodyGreen.SetBuddy(&m_editWoodyGreen);
	m_spinKBDI.SetDecimalPlaces (0);
	m_spinKBDI.SetTrimTrailingZeros (TRUE);
	m_spinKBDI.SetRangeAndDelta (0.0, 800.0, 1);
	m_spinKBDI.SetPos(kbdi);
	m_spinKBDI.SetBuddy(&m_editKBDI);
	m_spinKBDI2016.SetDecimalPlaces(0);
	m_spinKBDI2016.SetTrimTrailingZeros(TRUE);
	m_spinKBDI2016.SetRangeAndDelta(0.0, 800.0, 1);
	m_spinKBDI2016.SetPos(kbdi2016);
	m_spinKBDI2016.SetBuddy(&m_editKBDI2016);
	m_spinGSI.SetDecimalPlaces(2);
	m_spinGSI.SetTrimTrailingZeros(TRUE);
	m_spinGSI.SetRangeAndDelta(0.0, 1.0, 0.01);
	m_spinGSI.SetPos(gsi);
	m_spinGSI.SetBuddy(&m_editGSI);
	m_spinMaxGSI.SetDecimalPlaces(2);
	m_spinMaxGSI.SetTrimTrailingZeros(TRUE);
	m_spinMaxGSI.SetRangeAndDelta(0.0, 1.0, 0.01);
	m_spinMaxGSI.SetPos(gsiMax);
	m_spinMaxGSI.SetBuddy(&m_editMaxGSI);
	m_spinGSIThreshold.SetDecimalPlaces(2);
	m_spinGSIThreshold.SetTrimTrailingZeros(TRUE);
	m_spinGSIThreshold.SetRangeAndDelta(0.0, 1.0, 0.01);
	m_spinGSIThreshold.SetPos(gsiThreshold);
	m_spinGSIThreshold.SetBuddy(&m_editGSIThreshold);
	m_spinSCM.SetRange(1, 300);
	m_spinSCM.SetPos(30);
	m_btnHumid.SetCheck(FALSE);
	m_spinFuelTemp.SetDecimalPlaces(0);
	m_spinFuelTemp.SetTrimTrailingZeros(TRUE);
	m_spinFuelTemp.SetRangeAndDelta(-20.0, 120.0, 1.0);
	m_spinFuelTemp.SetPos(temperature);
	m_spinFuelTemp.SetBuddy(&m_editFuelTemp);
	//set values from theApp->pOptions
	if(theApp.pOptions)
	{
		CString tmpStr;
		tmpStr.Format("%c", theApp.pOptions->fuelModel);
		m_comboFuelModel.SelectString(-1, tmpStr);
		m_comboFuelModel.GetWindowTextA(fuelModel);
		tmpStr.Format("%d", theApp.pOptions->slopeClass);
		m_comboSlopeClass.SelectString(-1, tmpStr);
		m_comboSlopeClass.GetWindowText(slopeClass);
		use88 = theApp.pOptions->use88;
		m_spinTemp.SetPos(theApp.pOptions->temp);
		m_spinWind.SetPos(theApp.pOptions->windSpeed);
		tmpStr.Format("%d", theApp.pOptions->SOW);
		m_comboSOW.SelectString(-1, tmpStr);
		m_comboSOW.GetWindowText(sow);
		m_spinFM1.SetPos(theApp.pOptions->fm1);
		m_spinFM10.SetPos(theApp.pOptions->fm10);
		m_spinFM100.SetPos(theApp.pOptions->fm100);
		m_spinFM1000.SetPos(theApp.pOptions->fm1000);
		m_spinHerb.SetPos(theApp.pOptions->herb);
		m_spinWoody.SetPos(theApp.pOptions->woody);
		tmpStr.Format("%d", theApp.pOptions->season);
		seasonCtl.SelectString(-1, tmpStr);
		m_spinWoodyGreen.SetPos(theApp.pOptions->woodyGreen);
		m_spinKBDI.SetPos(theApp.pOptions->KBDI);
		rainEvent = theApp.pOptions->rainEvent;
		deciduous = theApp.pOptions->deciduousShrubs;

	}
	else
	{
		m_comboFuelModel.SetCurSel(6);
		m_comboFuelModel.GetWindowTextA(fuelModel);
	}
	UpdateData(FALSE);
	Set88Fields();
	SetFMFields();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNFDRSCalculator2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNFDRSCalculator2Dlg::OnPaint() 
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNFDRSCalculator2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CNFDRSCalculator2Dlg::Set88Fields()
{
	bool bIsNFDR2016 = false;
	if (fuelModel.GetLength() > 0)
		bIsNFDR2016 = isNFDRS2016(fuelModel[0]);
	if (bIsNFDR2016)
		use88 = false;
	static1.EnableWindow(use88);
	static2.EnableWindow(use88);
	static3.EnableWindow(use88);
	static4.EnableWindow(use88);
	decidCtl.EnableWindow(use88);
	m_spinKBDI.EnableWindow(use88);
	m_editKBDI.EnableWindow(use88);
	rainCtl.EnableWindow(use88);
	seasonCtl.EnableWindow(use88);
	m_spinWoodyGreen.EnableWindow(use88);
	m_editWoodyGreen.EnableWindow(use88);
}

void CNFDRSCalculator2Dlg::SetFMFields()
{
	if(fuelModel.GetLength() <= 0)
		return;
	bool bIsNFDR2016 = isNFDRS2016(fuelModel[0]);
	if (!bIsNFDR2016)
	{
		m_nfdrs.SetNFDRSVersion(use88 ? 88 : 78);
		m_nfdrs.iSetFuelModel(fuelModel[0], 0);
		m_spinFM1.EnableWindow(m_nfdrs.GetL1() == 0.0 ? FALSE : TRUE);
		m_editFM1.EnableWindow(m_nfdrs.GetL1() == 0.0 ? FALSE : TRUE);
		m_spinFM10.EnableWindow(m_nfdrs.GetL10() == 0.0 ? FALSE : TRUE);
		m_editFM10.EnableWindow(m_nfdrs.GetL10() == 0.0 ? FALSE : TRUE);
		m_spinFM100.EnableWindow(m_nfdrs.GetL100() == 0.0 ? FALSE : TRUE);
		m_editFM100.EnableWindow(m_nfdrs.GetL100() == 0.0 ? FALSE : TRUE);
		m_spinFM1000.EnableWindow(m_nfdrs.GetL1000() == 0.0 ? FALSE : TRUE);
		m_editFM1000.EnableWindow(m_nfdrs.GetL1000() == 0.0 ? FALSE : TRUE);
		m_spinHerb.EnableWindow(m_nfdrs.GetLHerb() == 0.0 ? FALSE : TRUE);
		m_editHerb.EnableWindow(m_nfdrs.GetLHerb() == 0.0 ? FALSE : TRUE);
		m_spinWoody.EnableWindow(m_nfdrs.GetLWood() == 0.0 ? FALSE : TRUE);
		m_editWoody.EnableWindow(m_nfdrs.GetLWood() == 0.0 ? FALSE : TRUE);
	}
	else
	{
		m_nfdrs2016.iSetFuelModel(fuelModel[0]);
		m_spinFM1.EnableWindow(m_nfdrs2016.L1 == 0.0 ? FALSE : TRUE);
		m_editFM1.EnableWindow(m_nfdrs2016.L1 == 0.0 ? FALSE : TRUE);
		m_spinFM10.EnableWindow(m_nfdrs2016.L10 == 0.0 ? FALSE : TRUE);
		m_editFM10.EnableWindow(m_nfdrs2016.L10 == 0.0 ? FALSE : TRUE);
		m_spinFM100.EnableWindow(m_nfdrs2016.L100 == 0.0 ? FALSE : TRUE);
		m_editFM100.EnableWindow(m_nfdrs2016.L100 == 0.0 ? FALSE : TRUE);
		m_spinFM1000.EnableWindow(m_nfdrs2016.L1000 == 0.0 ? FALSE : TRUE);
		m_editFM1000.EnableWindow(m_nfdrs2016.L1000 == 0.0 ? FALSE : TRUE);
		m_spinHerb.EnableWindow(m_nfdrs2016.LHERB == 0.0 ? FALSE : TRUE);
		m_editHerb.EnableWindow(m_nfdrs2016.LHERB == 0.0 ? FALSE : TRUE);
		m_spinWoody.EnableWindow(m_nfdrs2016.LWOOD == 0.0 ? FALSE : TRUE);
		m_editWoody.EnableWindow(m_nfdrs2016.LWOOD == 0.0 ? FALSE : TRUE);
		//m_spinGSI.EnableWindow(m_nfdrs2016.LHERB == 0.0 ? FALSE : TRUE);
		//m_spinGSIThreshold.EnableWindow(m_nfdrs2016.LHERB == 0.0 ? FALSE : TRUE);
		//m_spinMaxGSI.EnableWindow(m_nfdrs2016.LHERB == 0.0 ? FALSE : TRUE);
		m_spinSCM.SetPos(m_nfdrs2016.SCM);

	}
	m_editGSI.EnableWindow(bIsNFDR2016 && m_nfdrs2016.LHERB > 0.0);
	m_spinGSI.EnableWindow(bIsNFDR2016 && m_nfdrs2016.LHERB > 0.0);
	m_editKBDI2016.EnableWindow(bIsNFDR2016);
	m_spinKBDI2016.EnableWindow(bIsNFDR2016);
	m_editMaxGSI.EnableWindow(bIsNFDR2016 && m_nfdrs2016.LHERB > 0.0);
	m_spinMaxGSI.EnableWindow(bIsNFDR2016 && m_nfdrs2016.LHERB > 0.0);
	m_editGSIThreshold.EnableWindow(bIsNFDR2016 && m_nfdrs2016.LHERB > 0.0);
	m_spinGSIThreshold.EnableWindow(bIsNFDR2016 && m_nfdrs2016.LHERB > 0.0);
	m_btnUse88.EnableWindow(!bIsNFDR2016);
	m_editSCM.EnableWindow(bIsNFDR2016);
	m_spinSCM.EnableWindow(bIsNFDR2016);
	m_btnHumid.EnableWindow(bIsNFDR2016);
	m_comboSOW.EnableWindow(!bIsNFDR2016);
	m_editTemp.EnableWindow(!bIsNFDR2016);
	m_spinTemp.EnableWindow(!bIsNFDR2016);
	m_editFuelTemp.EnableWindow(bIsNFDR2016);
	m_spinFuelTemp.EnableWindow(bIsNFDR2016);
}



void CNFDRSCalculator2Dlg::OnCbnSelchangeNfdrsfm()
{
	UpdateData();
	bool bIsNFDR2016 = false;
	if(fuelModel.GetLength() > 0)
		bIsNFDR2016 = isNFDRS2016(fuelModel[0]);
	if (bIsNFDR2016)
		use88 = false;
	Set88Fields();
	SetFMFields();
}

void CNFDRSCalculator2Dlg::OnBnClickedUse88()
{
	UpdateData();
	Set88Fields();
	SetFMFields();
}

void CNFDRSCalculator2Dlg::OnBnClickedCalculate()
{
	UpdateData();
	if(fuelModel.GetLength() <= 0)
	{
		AfxMessageBox("ERROR: A fuel model must be selected!");
		return;
	}
	if(slopeClass.GetLength() <= 0)
	{
		AfxMessageBox("ERROR: A slope class must be selected!");
		return;
	}
	if(sow.GetLength() <= 0)
	{
		AfxMessageBox("ERROR: A State of Weather (SOW) must be selected!");
		return;
	}
	if(use88 && season.GetLength() <= 0)
	{
		AfxMessageBox("ERROR: A Season must be selected with 88 model!");
		return;
	}
	//do other error checking validation
	fm1 = m_spinFM1.GetPos();
	if(fm1 < 1.0 || fm1 > 65.0)
	{
		AfxMessageBox("ERROR: 1 - Hr FM must be betweeen 1.0 and 65.0");
		m_editFM1.SetFocus();
		return;
	}
	fm10 = m_spinFM10.GetPos();
	if(fm10 < 1.0 || fm10 > 65.0)
	{
		AfxMessageBox("ERROR: 10 - Hr FM must be betweeen 1.0 and 65.0");
		m_editFM10.SetFocus();
		return;
	}
	fm100 = m_spinFM100.GetPos();
	if(fm100 < 1.0 || fm100 > 65.0)
	{
		AfxMessageBox("ERROR: 100 - Hr FM must be betweeen 1.0 and 65.0");
		m_editFM100.SetFocus();
		return;
	}
	fm1000 = m_spinFM1000.GetPos();
	if(fm1000 < 1.0 || fm1000 > 65.0)
	{
		AfxMessageBox("ERROR: 1000 - Hr FM must be betweeen 1.0 and 65.0");
		m_editFM1000.SetFocus();
		return;
	}
	fmHerb = m_spinHerb.GetPos();
	if(fmHerb < 30.0 || fmHerb > 250.0)
	{
		AfxMessageBox("ERROR: Herb FM must be betweeen 30.0 and 250.0");
		m_editHerb.SetFocus();
		return;
	}
	fmWood = m_spinWoody.GetPos();
	if(fmWood < 50.0 || fmWood > 200.0)
	{
		AfxMessageBox("ERROR: Woody FM must be betweeen 50.0 and 200.0");
		m_editWoody.SetFocus();
		return;
	}
	temperature = m_spinTemp.GetPos();
	if(temperature < -20.0 || temperature > 120.0)
	{
		AfxMessageBox("ERROR: Temperature must be betweeen -20.0 and 120.0");
		m_editTemp.SetFocus();
		return;
	}
	windSpeed = m_spinWind.GetPos();
	if(windSpeed < 0.0 || windSpeed > 99.0)
	{
		AfxMessageBox("ERROR: 20' Wind must be betweeen 0.0 and 990.0");
		m_editWind.SetFocus();
		return;
	}
	kbdi = m_spinKBDI.GetPos();
	woodyGreen = m_spinWoodyGreen.GetPos();
	if(use88 && woodyGreen < 0.0 || woodyGreen > 20.0)
	{
		AfxMessageBox("ERROR: Woody Greenness must be betweeen 0.0 and 20.0");
		m_editWoodyGreen.SetFocus();
		return;
	}
	fuelTemperature = m_spinFuelTemp.GetPos();
	if (fuelTemperature < -20.0 || fuelTemperature > 120.0)
	{
		AfxMessageBox("ERROR: Fuel Temperature must be betweeen -20.0 and 120.0");
		m_editFuelTemp.SetFocus();
		return;
	}
	gsi = m_spinGSI.GetPos();
	kbdi2016 = m_spinKBDI2016.GetPos();
	gsiMax = m_spinMaxGSI.GetPos();
	gsiThreshold = m_spinGSIThreshold.GetPos();
	//calculate the values
	bool bIsNFDR2016 = false;
	if (fuelModel.GetLength() > 0)
		bIsNFDR2016 = isNFDRS2016(fuelModel[0]);
	if (!bIsNFDR2016)
	{
		int iSC, iFIL, iBI, iIC;
		double fROS, fERC, fFL;
		m_nfdrs.iInitialize(use88 ? 88 : 78, 1, 100, 300, 45, 0, deciduous, 0, 20, 25, 100, 30);
		m_nfdrs.iSetFuelModel(fuelModel[0], 0);
		m_nfdrs.iSetMoistures(fm1, fm10, fm100, fm1000, fmWood, fmHerb, rainEvent, kbdi, 15, woodyGreen, atoi(season), atoi(sow));
		m_nfdrs.iCalcIndexes(windSpeed, atoi(slopeClass), &fROS, &iSC, &fERC, &fFL, &iFIL, &iBI);
		m_nfdrs.iCalcIC(temperature, atoi(sow), fm1, iSC, &iIC);

		sc = iSC;
		erc = fERC;
		bi = iBI;
		ic = iIC;
	}
	else
	{
		double fSC, fERC, fBI, fIC;
		m_nfdrs2016.Init(45, fuelModel[0], atoi(slopeClass), 30, true, true, false, 100);// , gsiMax, gsiThreshold);
		m_nfdrs2016.iSetFuelModel(fuelModel[0]);
		m_nfdrs2016.SetSCMax(m_spinSCM.GetPos());
		m_nfdrs2016.SetMxdHumid(m_btnHumid.GetCheck());
		m_nfdrs2016.SetHerbGSIparams(gsiMax, gsiThreshold);
		//need fuel temp in C
		double fTempC, tfact = 0.0;
		switch (atoi(sow))
		{
		case -1:
			tfact = 0.0;
			break;
		case 0:
			tfact = 25.0;
			break;
		case 1:
			tfact = 19.0;
			break;
		case 2:
			tfact = 12.0;
			break;
		default:
			tfact = 5.0;
		}
		fTempC = (fuelTemperature + tfact - 32) / 1.8;
		m_nfdrs2016.iSetFuelMoistures(fm1, fm10, fm100, fm1000, fmWood, fmHerb, fTempC);
		m_nfdrs2016.iCalcIndexes(windSpeed, atoi(slopeClass), &fSC, &fERC, &fBI, &fIC, gsi, kbdi2016);// , &iBI);
		sc = fSC;
		erc = fERC;
		bi = fBI;
		ic = fIC;
	}
	CString fmtStr;
	fmtStr.Format("%.2lf", sc);
	GetDlgItem(IDC_SC)->SetWindowTextA(fmtStr);
	fmtStr.Format("%.2lf", erc);
	GetDlgItem(IDC_ERC)->SetWindowTextA(fmtStr);
	fmtStr.Format("%.2lf", bi);
	GetDlgItem(IDC_BI)->SetWindowTextA(fmtStr);
	fmtStr.Format("%.2lf", ic);
	GetDlgItem(IDC_IC)->SetWindowTextA(fmtStr);
	UpdateData(false);

}

void CNFDRSCalculator2Dlg::OnBnClickedButtonFmparams()
{
	if(!pFMParamsDlg)
	{
		pFMParamsDlg = new CFMParamsDlg(this);
		pFMParamsDlg->Create(CFMParamsDlg::IDD, this);
	}
	pFMParamsDlg->ShowWindow(SW_SHOW);
}

void CNFDRSCalculator2Dlg::OnBnClickedButtonKbdicalc()
{
	CKBDIDIalog kbdiDlg(this);
	if(kbdiDlg.DoModal() == IDOK)
	{
		kbdi = kbdiDlg.kbdi;
		m_spinKBDI.SetPos(kbdi);
		m_spinKBDI2016.SetPos(kbdi);
	}
}

void CNFDRSCalculator2Dlg::OnClose()
{
	//save the configuration
	UpdateData(TRUE);
	CString tmpStr;
	m_comboFuelModel.GetWindowText(tmpStr);
	theApp.pOptions->fuelModel = tmpStr[0];
	theApp.pOptions->slopeClass = atoi(slopeClass);
	theApp.pOptions->use88 = use88;
	theApp.pOptions->temp = m_spinTemp.GetPos();
	theApp.pOptions->windSpeed = m_spinWind.GetPos();
	theApp.pOptions->SOW = atoi(sow);
	theApp.pOptions->fm1 = m_spinFM1.GetPos();
	theApp.pOptions->fm10 = m_spinFM10.GetPos();
	theApp.pOptions->fm100 = m_spinFM100.GetPos();
	theApp.pOptions->fm1000 = m_spinFM1000.GetPos();
	theApp.pOptions->herb = m_spinHerb.GetPos();
	theApp.pOptions->woody = m_spinWoody.GetPos();
	seasonCtl.GetWindowText(tmpStr);
	theApp.pOptions->season = atoi(tmpStr);
	theApp.pOptions->woodyGreen = m_spinWoodyGreen.GetPos();
	theApp.pOptions->KBDI = m_spinKBDI.GetPos();
	theApp.pOptions->rainEvent = rainEvent;
	theApp.pOptions->deciduousShrubs = deciduous;
	
	CDialog::OnClose();
}

void CNFDRSCalculator2Dlg::OnEnKillfocus1hrfm()
{
	CString str;
	m_editFM1.GetWindowText(str);
	if(str.GetLength() <= 0)
	{
		AfxMessageBox("Error: 1 - Hr FM can not be blank");
		m_editFM1.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnChange1hrfm()
{
	CString str;
	m_editFM1.GetWindowText(str);
	double fm1 = atof(str);
	if(strlen(str) > 1 && (fm1 < 1.0 || fm1 > 65.0))
	{
		AfxMessageBox("Error: 1 - Hr FM must be between 1.0 and 65.0");
		m_editFM1.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnKillfocus10hrfm()
{
	CString str;
	m_editFM10.GetWindowText(str);
	if(str.GetLength() <= 0)
	{
		AfxMessageBox("Error: 10 - Hr FM can not be blank");
		m_editFM10.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnChange10hrfm()
{
	CString str;
	m_editFM10.GetWindowText(str);
	double fm10 = atof(str);
	if(strlen(str) > 1 && (fm10 < 1.0 || fm10 > 65.0))
	{
		AfxMessageBox("Error: 10 - Hr FM must be between 1.0 and 65.0");
		m_editFM10.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnKillfocus100hrfm()
{
	CString str;
	m_editFM100.GetWindowText(str);
	if(str.GetLength() <= 0)
	{
		AfxMessageBox("Error: 100 - Hr FM can not be blank");
		m_editFM100.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnChange100hrfm()
{
	CString str;
	m_editFM100.GetWindowText(str);
	double fm100 = atof(str);
	if(strlen(str) > 1 && (fm100 < 1.0 || fm100 > 65.0))
	{
		AfxMessageBox("Error: 100 - Hr FM must be between 1.0 and 65.0");
		m_editFM100.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnKillfocus1000hrfm()
{
	CString str;
	m_editFM1000.GetWindowText(str);
	if(str.GetLength() <= 0)
	{
		AfxMessageBox("Error: 1000 - Hr FM can not be blank");
		m_editFM1000.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnChange1000hrfm()
{
	CString str;
	m_editFM1000.GetWindowText(str);
	double fm1000 = atof(str);
	if(strlen(str) > 1 && (fm1000 < 1.0 || fm1000 > 65.0))
	{
		AfxMessageBox("Error: 1000 - Hr FM must be between 1.0 and 65.0");
		m_editFM1000.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnKillfocusHerbfm()
{
	CString str;
	m_editHerb.GetWindowText(str);
	if(str.GetLength() <= 0)
	{
		AfxMessageBox("Error: Herb FM can not be blank");
		m_editHerb.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnChangeHerbfm()
{
	CString str;
	m_editHerb.GetWindowText(str);
	double fmHerb = atof(str);
	if(strlen(str) > 2 && (fmHerb < 30.0 || fmHerb > 250.0))
	{
		AfxMessageBox("Error: Herb FM must be between 30.0 and 250.0");
		m_editHerb.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnKillfocusWoodyfm()
{
	CString str;
	m_editWoody.GetWindowText(str);
	if(str.GetLength() <= 0)
	{
		AfxMessageBox("Error: Woody FM can not be blank");
		m_editWoody.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnChangeWoodyfm()
{
	CString str;
	m_editWoody.GetWindowText(str);
	double fmWoody = atof(str);
	if(strlen(str) > 2 && (fmWoody < 50.0 || fmWoody > 200.0))
	{
		AfxMessageBox("Error: Woody FM must be between 50.0 and 200.0");
		m_editWoody.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnKillfocusWoodyGreen()
{
	CString str;
	m_editWoodyGreen.GetWindowText(str);
	if(str.GetLength() <= 0)
	{
		AfxMessageBox("Error: Woody Greenness can not be blank");
		m_editWoodyGreen.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnChangeWoodyGreen()
{
	CString str;
	m_editWoodyGreen.GetWindowText(str);
	double fmWoodyGreen = atof(str);
	if(strlen(str) > 1 && (fmWoodyGreen < 0.0 || fmWoodyGreen > 20.0))
	{
		AfxMessageBox("Error: Woody Greenness must be between 0.0 and 20.0");
		m_editWoodyGreen.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnKillfocusTemp()
{
	CString str;
	m_editTemp.GetWindowText(str);
	if(str.GetLength() <= 0)
	{
		AfxMessageBox("Error: Temperature can not be blank");
		m_editTemp.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnChangeTemp()
{
	CString str;
	m_editTemp.GetWindowText(str);
	double val = atof(str);
	if(strlen(str) > 1 && (val < -20.0 || val > 120.0))
	{
		AfxMessageBox("Error: Temperature must be between -20 and 120.0");
		m_editTemp.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnKillfocusWind()
{
	CString str;
	m_editWind.GetWindowText(str);
	if(str.GetLength() <= 0)
	{
		AfxMessageBox("Error: 20' Wind can not be blank");
		m_editWind.SetFocus();
	}
}

void CNFDRSCalculator2Dlg::OnEnChangeWind()
{
	CString str;
	m_editWind.GetWindowText(str);
	double val = atof(str);
	if(strlen(str) > 1 && (val < 0.0 || val > 99.0))
	{
		AfxMessageBox("Error: 20' Wind must be between 0 and 99.0");
		m_editWind.SetFocus();
	}
}

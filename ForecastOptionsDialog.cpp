// ForecastOptionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "ForecastOptionsDialog.h"
#include "ClimateSet.h"
#include "FFPfxVarsSet.h"
#include "FFPfxOptsSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CForecastOptionsDialog dialog


CForecastOptionsDialog::CForecastOptionsDialog(CWnd* pParent /*=NULL*/, ForecastOptions *_options /*= NULL*/, CDatabase *_pDB )
	: CDialog(CForecastOptionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CForecastOptionsDialog)
	lineWidth = 0;
	lineColor = 0;
	//}}AFX_DATA_INIT
	trg = _options;
	options.Copy(_options);
	pDB = _pDB;
}


void CForecastOptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CForecastOptionsDialog)
	DDX_Control(pDX, IDC_LINESTYLE, lineStyleCtl);
	DDX_Control(pDX, IDC_OUTPUTLIST, outputs);
	//DDX_Control(pDX, IDC_RH, rh);
	//DDX_Control(pDX, IDC_TEMP, temp);
	//DDX_Control(pDX, IDC_WINDS, winds);
	//DDX_OCInt(pDX, IDC_LINEWIDTH, DISPID(27), lineWidth);
	DDX_Control(pDX, IDC_LINECOLOR, lineColorCtl);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SPINWID, m_spinWidth);
	DDX_Control(pDX, IDC_SPINWIND, m_spinWind);
	DDX_Control(pDX, IDC_SPINRH, m_spinRH);
	DDX_Control(pDX, IDC_SPINTEMP, m_spinTemp);
}


BEGIN_MESSAGE_MAP(CForecastOptionsDialog, CDialog)
	//{{AFX_MSG_MAP(CForecastOptionsDialog)
	ON_BN_CLICKED(IDC_SAME, OnSame)
	ON_BN_CLICKED(IDC_PERCENTILES, OnPercentiles)
	ON_BN_CLICKED(IDC_BLANK, OnBlank)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CForecastOptionsDialog message handlers

BOOL CForecastOptionsDialog::OnInitDialog() 
{
	lineColor = options.lineColor;
	lineWidth = options.lineWidth;
	CDialog::OnInitDialog();
	lineColorCtl.SetColor(lineColor);
	m_spinRH.SetRange(1, 100);
	m_spinRH.SetPos(options.rhPcnt * 100.0);
	//rh.SetValueReal(options.rhPcnt * 100.0);
	m_spinTemp.SetRange(1, 100);
	m_spinTemp.SetPos(options.tempPcnt * 100.0);
	//temp.SetValueReal(options.tempPcnt * 100.0);
	m_spinWind.SetRange(1, 100);
	m_spinWind.SetPos(options.windPcnt * 100.0);
	//winds.SetValueReal(options.windPcnt * 100.0);
	m_spinWidth.SetRange(1, 10);
	m_spinWidth.SetPos(lineWidth);
	CString ls;
	for(int i = 0; i < lineStyleCtl.GetCount(); i++)
	{
		lineStyleCtl.GetLBText(i, ls);
		if(options.lineStyle == StringToLineStyle(ls))
		{
			lineStyleCtl.SetCurSel(i);
			break;
		}
	}
	if(lineStyleCtl.GetCurSel() <= 0)
		lineStyleCtl.SetCurSel(0);
	CClimateSet climSet(pDB);
	climSet.Open();
	while(!climSet.IsEOF())
	{
		if(climSet.m_OptionType == 1 || climSet.m_OptionType == 3)
		{
			int loc = outputs.AddString(climSet.m_Variable_Name);
			outputs.SetItemData(loc, climSet.m_VarID);
		}
		climSet.MoveNext();
	}
	climSet.Close();
	POSITION pos = options.outputsList.GetHeadPosition();
	while(pos)
	{
		int val = options.outputsList.GetNext(pos);
		for(int i = 0; i < outputs.GetCount(); i++)
		{
			if((int)outputs.GetItemData(i) == val)
			{
				outputs.SetSel(i);
				break;
			}
		}
	}
	ConfigureControls();
	//lineColorCtl.SetBackColor(lineColor);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CForecastOptionsDialog::OnSame() 
{
	options.addType = Same;
	ConfigureControls();
}

void CForecastOptionsDialog::OnPercentiles() 
{
	options.addType = Historic;
	ConfigureControls();
}

void CForecastOptionsDialog::OnBlank() 
{
	options.addType = Blank;
	ConfigureControls();	
}

void CForecastOptionsDialog::ConfigureControls()
{
	CButton *same = (CButton *)GetDlgItem(IDC_SAME);
	CButton *blank = (CButton *)GetDlgItem(IDC_BLANK);
	CButton *historic = (CButton *)GetDlgItem(IDC_PERCENTILES);
	switch(options.addType)
	{
	case Same:
		same->SetCheck(TRUE);
		blank->SetCheck(FALSE);
		historic->SetCheck(FALSE);
		//Continue
	case Blank:
		if(options.addType == Blank)
		{
			same->SetCheck(FALSE);
			blank->SetCheck(TRUE);
			historic->SetCheck(FALSE);
		}
		m_spinRH.EnableWindow(FALSE);
		m_spinTemp.EnableWindow(FALSE);
		m_spinWind.EnableWindow(FALSE);
		break;
	case Historic:
		same->SetCheck(FALSE);
		blank->SetCheck(FALSE);
		historic->SetCheck(TRUE);
		m_spinRH.EnableWindow(TRUE);
		m_spinTemp.EnableWindow(TRUE);
		m_spinWind.EnableWindow(TRUE);
		break;
	}

}

void CForecastOptionsDialog::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	lineColor = lineColorCtl.GetColor();
	options.rhPcnt = ((double)m_spinRH.GetPos()) / 100.0;
	options.tempPcnt = ((double)m_spinTemp.GetPos()) / 100.0;
	options.windPcnt = ((double)m_spinWind.GetPos()) / 100.0;
	options.lineColor = lineColor;
	lineWidth = m_spinWidth.GetPos();
	options.lineWidth = lineWidth;
	CString ls;
	lineStyleCtl.GetWindowText(ls);
	options.lineStyle = StringToDashLineStyle(ls);
	options.outputsList.RemoveAll();
	for(int i = 0; i < outputs.GetCount(); i++)
	{
		if(outputs.GetSel(i))
			options.outputsList.AddTail((int)outputs.GetItemData(i));
	}
	trg->Copy(&options);
	CDialog::OnOK();
}



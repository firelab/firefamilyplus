// RerapDialog.cpp : implementation file
//
 
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "wxSet.h"
#include "fireplusSet.h"
//#include "nfdr32.h"
#include "RerapDialog.h"
#include "SIGStationSet.h"
//#include "canada.h"
#include <math.h>
//#include "FireDay.h"
//#include "perioddoc.h"
//#include "splitterframe.h"
//#include "FireSumSet.h"
//#include "ClimAnalysis.h"
#include "ReportOptionsSet.h"
#include <afxrich.h>
#include "richdoc.h"
#include "CustomInitSet.h"
#include "LFIengine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern int curr_runID;

RerapStats::RerapStats()
{
	for(int i = 0; i < 50; i++)
		fm1[i] = fm10[i] = fm100[i] = fm1000[i] = herb[i] = wind[i] = woody[i] = kbdi[i] = 0.0;
	nClasses = 0;
}

void RerapStats::BuildStats(CList<RerapObs, RerapObs&> *list, double vMin, double vMax)
{
	if(nClasses <= 0)
	{
		step = 1;
		nClasses = (int)(ceil(vMax) - floor(vMin));
		if(nClasses > 50)
		{
			step = nClasses / 50 + 1;
			if(nClasses % 2 == 0)
				nClasses = nClasses / step;
			else
				nClasses = nClasses / step +  1;
		}
		start = (int)floor(vMin);
	}
	int loc;
	POSITION pos = list->GetHeadPosition();
	while(pos)
	{
		RerapObs rObs = list->GetAt(pos);
		//loc = ((double)i / (double)n) * 100;
		loc = (int)((rObs.var - start) / step);
		if(loc < 0)
			loc = 0;
		else if(loc >= nClasses)
			loc = nClasses - 1;
		varStats[loc].Accumulate(rObs.var);
		fm1[loc] += rObs.fm1;
		fm10[loc] += rObs.fm10;
		fm100[loc] += rObs.fm100;
		fm1000[loc] += rObs.fm1000;
		herb[loc] += rObs.herb;
		woody[loc] += rObs.woody;
		wind[loc] += rObs.wind;
		kbdi[loc] += rObs.kbdi;
		list->GetNext(pos);
	}
	//ok, now finish calculations
	for(int i = 0; i < nClasses; i++)
	{
		varStats[i].Complete();
		if(varStats[i].N() > 0)
		{
			fm1[i] /= varStats[i].N();
			fm10[i] /= varStats[i].N();
			fm100[i] /= varStats[i].N();
			fm1000[i] /= varStats[i].N();
			herb[i] /= varStats[i].N();
			woody[i] /= varStats[i].N();
			wind[i] /= varStats[i].N();
			kbdi[i] /= varStats[i].N();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CRerapDialog dialog


CRerapDialog::CRerapDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet /*= NULL*/, int _varID/*= 0*/)
	: CDialog(CRerapDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRerapDialog)
	wE = FALSE;
	wN = FALSE;
	wNE = FALSE;
	wNW = FALSE;
	wS = FALSE;
	wSE = FALSE;
	wSW = FALSE;
	wW = FALSE;
	low = 0;
	mod = 0;
	high = 0;
	high1 = _T("");
	ext1 = _T("");
	mod1 = _T("");
	lpcnt = 0;
	hobs = 0;
	hpcnt = 0;
	lobs = 0;
	mobs = 0;
	mpcnt = 0;
	epcnt = 0;
	eobs = 0;
	l1000 = 0.0f;
	l100 = 0.0f;
	l10 = 0.0f;
	l1 = 0.0f;
	l20 = 0.0f;
	lherb = 0.0f;
	lwoody = 0.0f;
	Mod1000 = 0.0f;
	Mod100 = 0.0f;
	Mod10 = 0.0f;
	Mod1 = 0.0f;
	Mod20 = 0.0f;
	Modherb = 0.0f;
	Modwoody = 0.0f;
	e1000 = 0.0f;
	e100 = 0.0f;
	e10 = 0.0f;
	e1 = 0.0f;
	e20 = 0.0f;
	eherb = 0.0f;
	ewoody = 0.0f;
	h1000 = 0.0f;
	h100 = 0.0f;
	h10 = 0.0f;
	h1 = 0.0f;
	h20 = 0.0f;
	hherb = 0.0f;
	hwoody = 0.0f;
	eerc = 0;
	herc = 0;
	lerc = 0;
	Moderc = 0;
	freqs = _T("");
	em1 = 0;
	em2 = 0;
	hm1 = 0;
	hm2 = 0;
	lm1 = 0;
	lm2 = 0;
	mm1 = 0;
	mm2 = 0;
	lsc = 0;
	Modsc = 0;
	hsc = 0;
	esc = 0;
	//}}AFX_DATA_INIT
	windDays = 0;
	varID = _varID;
	fpSet = _fpSet;
	low = 15;
	mod = 89;
	high = 97;
	lpcnt = 15;
//	lmed = 0.0;
	lobs = 0;
	mpcnt = 75;
//	mmed = 0.0;
	mobs = 0;
	hpcnt = 7;
//	hmed = 0.0;
	hobs = 0;
	epcnt = 3;
//	emed = 0.0;
	eobs = 0;
	mod1.Format("16 -");
	high1.Format("90 -");
	ext1.Format("98");
	//ascending = true;
//	if((varID > 4 && varID < 11) || (varID > 16 && varID < 23))
	//	ascending = false;
	switch(varID)
	{
	case 12:
		varString = "SC";
		break;
	case 13:
		varString = "ERC";
		break;
	case 14:
		varString = "BI";
		break;
	case 15:
		varString = "KBDI";
		break;
	case 16:
		varString = "IC";
		break;
	}
	lkbdi = Modkbdi = hkbdi = ekbdi = 0.0;
	nRecs = 0L;
	tTitle = "Percentile Weather";
}

CRerapDialog::~CRerapDialog()
{
}

void CRerapDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRerapDialog)
	DDX_Control(pDX, IDC_FREQS, freqControl);
	DDX_Control(pDX, IDC_AVGSAPPLY, avgsApply);
	DDX_Check(pDX, IDC_E, wE);
	DDX_Check(pDX, IDC_N, wN);
	DDX_Check(pDX, IDC_NE, wNE);
	DDX_Check(pDX, IDC_NW, wNW);
	DDX_Check(pDX, IDC_S, wS);
	DDX_Check(pDX, IDC_SE, wSE);
	DDX_Check(pDX, IDC_SW, wSW);
	DDX_Check(pDX, IDC_W, wW);
	DDX_Text(pDX, IDC_LOW, low);
	DDX_Text(pDX, IDC_MED2, mod);
	DDX_Text(pDX, IDC_HIGH2, high);
	DDX_Text(pDX, IDC_HIGH1, high1);
	DDX_Text(pDX, IDC_EXTREME1, ext1);
	DDX_Text(pDX, IDC_MED1, mod1);
	DDX_Text(pDX, IDC_LOWPERCENT, lpcnt);
	DDX_Text(pDX, IDC_HIGHOBS, hobs);
	DDX_Text(pDX, IDC_HIGHPERCENT, hpcnt);
	DDX_Text(pDX, IDC_LOWOBS, lobs);
	DDX_Text(pDX, IDC_MEDIUMOBS, mobs);
	DDX_Text(pDX, IDC_MEDIUMPERCENT, mpcnt);
	DDX_Text(pDX, IDC_EXTREMEPERCENT, epcnt);
	DDX_Text(pDX, IDC_EXTREMEOBS, eobs);
	DDX_Text(pDX, IDC_EXTERC, eerc);
	DDX_Text(pDX, IDC_HIGHERC, herc);
	DDX_Text(pDX, IDC_LOWERC, lerc);
	DDX_Text(pDX, IDC_MEDERC, Moderc);
	DDX_Text(pDX, IDC_FREQS, freqs);
	DDX_Text(pDX, IDC_EM1, em1);
	DDX_Text(pDX, IDC_EM2, em2);
	DDX_Text(pDX, IDC_HM1, hm1);
	DDX_Text(pDX, IDC_HM2, hm2);
	DDX_Text(pDX, IDC_LM1, lm1);
	DDX_Text(pDX, IDC_LM2, lm2);
	DDX_Text(pDX, IDC_MM1, mm1);
	DDX_Text(pDX, IDC_MM2, mm2);
	DDX_Text(pDX, IDC_LOWSC, lsc);
	DDX_Text(pDX, IDC_MEDSC, Modsc);
	DDX_Text(pDX, IDC_HIGHSC, hsc);
	DDX_Text(pDX, IDC_EXTSC, esc);
	//}}AFX_DATA_MAP
	//LOW
	DDX_Control(pDX, IDC_SPIN_LOW1HR, m_spinL1);
	DDX_Control(pDX, IDC_LOW1HR, m_editLH1);
	DDX_Control(pDX, IDC_SPIN_LOW10HR, m_spinL10);
	DDX_Control(pDX, IDC_LOW10HR, m_editLH10);
	DDX_Control(pDX, IDC_SPIN_LOW100HR, m_spinL100);
	DDX_Control(pDX, IDC_LOW100HR, m_editLH100);
	DDX_Control(pDX, IDC_SPIN_LOW1000HR, m_spinL1000);
	DDX_Control(pDX, IDC_LOW1000HR, m_editLH1000);
	DDX_Control(pDX, IDC_SPIN_LOWHERB, m_spinLHerb);
	DDX_Control(pDX, IDC_LOWHERB, m_editLHerb);
	DDX_Control(pDX, IDC_SPIN_LOWWOODY, m_spinLWoody);
	DDX_Control(pDX, IDC_LOWWOODY, m_editLWoody);
	DDX_Control(pDX, IDC_SPIN_LOW20, m_spinL20);
	DDX_Control(pDX, IDC_LOW20, m_editL20);
	//MODERATE
	DDX_Control(pDX, IDC_SPIN_LOW1HR2, m_spinM1);
	DDX_Control(pDX, IDC_MED1HR, m_editMH1);
	DDX_Control(pDX, IDC_SPIN_LOW10HR2, m_spinM10);
	DDX_Control(pDX, IDC_MED10HR, m_editMH10);
	DDX_Control(pDX, IDC_SPIN_LOW100HR2, m_spinM100);
	DDX_Control(pDX, IDC_MED100HR, m_editMH100);
	DDX_Control(pDX, IDC_SPIN_LOW1000HR2, m_spinM1000);
	DDX_Control(pDX, IDC_MED1000HR, m_editMH1000);
	DDX_Control(pDX, IDC_SPIN_LOWHERB2, m_spinMHerb);
	DDX_Control(pDX, IDC_MEDHERB, m_editMHerb);
	DDX_Control(pDX, IDC_SPIN_LOWWOODY2, m_spinMWoody);
	DDX_Control(pDX, IDC_MEDWOODY, m_editMWoody);
	DDX_Control(pDX, IDC_SPIN_LOW21, m_spinM20);
	DDX_Control(pDX, IDC_MED20, m_editM20);
	//HIGH
	DDX_Control(pDX, IDC_SPIN_LOW1HR3, m_spinH1);
	DDX_Control(pDX, IDC_HIGH1HR, m_editHH1);
	DDX_Control(pDX, IDC_SPIN_LOW10HR3, m_spinH10);
	DDX_Control(pDX, IDC_HIGH10HR, m_editHH10);
	DDX_Control(pDX, IDC_SPIN_LOW100HR3, m_spinH100);
	DDX_Control(pDX, IDC_HIGH100HR, m_editHH100);
	DDX_Control(pDX, IDC_SPIN_LOW1000HR3, m_spinH1000);
	DDX_Control(pDX, IDC_HIGH1000HR, m_editHH1000);
	DDX_Control(pDX, IDC_SPIN_LOWHERB3, m_spinHHerb);
	DDX_Control(pDX, IDC_HIGHHERB, m_editHHerb);
	DDX_Control(pDX, IDC_SPIN_LOWWOODY3, m_spinHWoody);
	DDX_Control(pDX, IDC_HIGHWOODY, m_editHWoody);
	DDX_Control(pDX, IDC_SPIN_LOW22, m_spinH20);
	DDX_Control(pDX, IDC_HIGH20, m_editH20);
	//EXTREME
	DDX_Control(pDX, IDC_SPIN_LOW1HR4, m_spinE1);
	DDX_Control(pDX, IDC_EXT1HR, m_editEH1);
	DDX_Control(pDX, IDC_SPIN_LOW10HR4, m_spinE10);
	DDX_Control(pDX, IDC_EXT10HR, m_editEH10);
	DDX_Control(pDX, IDC_SPIN_LOW100HR4, m_spinE100);
	DDX_Control(pDX, IDC_EXT100HR, m_editEH100);
	DDX_Control(pDX, IDC_SPIN_LOW1000HR4, m_spinE1000);
	DDX_Control(pDX, IDC_EXT1000HR, m_editEH1000);
	DDX_Control(pDX, IDC_SPIN_LOWHERB4, m_spinEHerb);
	DDX_Control(pDX, IDC_EXTHERB, m_editEHerb);
	DDX_Control(pDX, IDC_SPIN_LOWWOODY4, m_spinEWoody);
	DDX_Control(pDX, IDC_EXTWOODY, m_editEWoody);
	DDX_Control(pDX, IDC_SPIN_LOW23, m_spinE20);
	DDX_Control(pDX, IDC_EXT20, m_editE20);
	if(pDX->m_bSaveAndValidate)//set values from dialog
	{
		l1 = m_spinL1.GetPos();
		l10 = m_spinL10.GetPos();
		l100 = m_spinL100.GetPos();
		l1000 = m_spinL1000.GetPos();
		lherb = m_spinLHerb.GetPos();
		lwoody = m_spinLWoody.GetPos();
		l20 = m_spinL20.GetPos();
		Mod1 = m_spinM1.GetPos();
		Mod10 = m_spinM10.GetPos();
		Mod100 = m_spinM100.GetPos();
		Mod1000 = m_spinM1000.GetPos();
		Modherb = m_spinMHerb.GetPos();
		Modwoody = m_spinMWoody.GetPos();
		Mod20 = m_spinM20.GetPos();
		h1 = m_spinH1.GetPos();
		h10 = m_spinH10.GetPos();
		h100 = m_spinH100.GetPos();
		h1000 = m_spinH1000.GetPos();
		hherb = m_spinHHerb.GetPos();
		hwoody = m_spinHWoody.GetPos();
		h20 = m_spinH20.GetPos();
		e1 = m_spinE1.GetPos();
		e10 = m_spinE10.GetPos();
		e100 = m_spinE100.GetPos();
		e1000 = m_spinE1000.GetPos();
		eherb = m_spinEHerb.GetPos();
		ewoody = m_spinEWoody.GetPos();
		e20 = m_spinE20.GetPos();
	}
	else //load values to dialog
	{
		m_spinL1.SetPos(l1);
		m_spinL10.SetPos(l10);
		m_spinL100.SetPos(l100);
		m_spinL1000.SetPos(l1000);
		m_spinLHerb.SetPos(lherb);
		m_spinLWoody.SetPos(lwoody);
		m_spinL20.SetPos(l20);
		m_spinM1.SetPos(Mod1);
		m_spinM10.SetPos(Mod10);
		m_spinM100.SetPos(Mod100);
		m_spinM1000.SetPos(Mod1000);
		m_spinMHerb.SetPos(Modherb);
		m_spinMWoody.SetPos(Modwoody);
		m_spinM20.SetPos(Mod20);
		m_spinH1.SetPos(h1);
		m_spinH10.SetPos(h10);
		m_spinH100.SetPos(h100);
		m_spinH1000.SetPos(h1000);
		m_spinHHerb.SetPos(hherb);
		m_spinHWoody.SetPos(hwoody);
		m_spinH20.SetPos(h20);
		m_spinE1.SetPos(e1);
		m_spinE10.SetPos(e10);
		m_spinE100.SetPos(e100);
		m_spinE1000.SetPos(e1000);
		m_spinEHerb.SetPos(eherb);
		m_spinEWoody.SetPos(ewoody);
		m_spinE20.SetPos(e20);

	}
}


BEGIN_MESSAGE_MAP(CRerapDialog, CDialog)
	//{{AFX_MSG_MAP(CRerapDialog)
	ON_EN_KILLFOCUS(IDC_LOW, OnKillfocusLow)
	ON_BN_CLICKED(IDC_CLASSAPPLY, OnClassapply)
	ON_BN_CLICKED(IDC_AVGSAPPLY, OnAvgsapply)
	ON_EN_KILLFOCUS(IDC_MED2, OnKillfocusMed2)
	ON_EN_KILLFOCUS(IDC_HIGH2, OnKillfocusHigh2)
	ON_BN_CLICKED(IDC_E, OnE)
	ON_BN_CLICKED(IDC_N, OnN)
	ON_BN_CLICKED(IDC_NE, OnNe)
	ON_BN_CLICKED(IDC_NW, OnNw)
	ON_BN_CLICKED(IDC_S, OnS)
	ON_BN_CLICKED(IDC_SE, OnSe)
	ON_BN_CLICKED(IDC_SW, OnSw)
	ON_BN_CLICKED(IDC_W, OnW)
	ON_EN_KILLFOCUS(IDC_LM1, OnKillfocusLm1)
	ON_EN_KILLFOCUS(IDC_LM2, OnKillfocusLm2)
	ON_EN_KILLFOCUS(IDC_MM1, OnKillfocusMm1)
	ON_EN_KILLFOCUS(IDC_MM2, OnKillfocusMm2)
	ON_EN_KILLFOCUS(IDC_HM1, OnKillfocusHm1)
	ON_EN_KILLFOCUS(IDC_HM2, OnKillfocusHm2)
	ON_EN_KILLFOCUS(IDC_EM1, OnKillfocusEm1)
	ON_EN_KILLFOCUS(IDC_EM2, OnKillfocusEm2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRerapDialog message handlers

BOOL CRerapDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//CFont tFont;
	//tFont.CreatePointFont(85, "courier new");
	//freqControl.SetFont(&tFont);
	//if(!ReadData())
	//{
	//	EndDialog(-1);
	//}
	SetWindowText(tTitle);
	//low column
	m_spinL1.SetDecimalPlaces (2);
	m_spinL1.SetTrimTrailingZeros (FALSE);
	m_spinL1.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinL1.SetPos(l1);
	m_spinL1.SetBuddy(&m_editLH1);
	m_spinL10.SetDecimalPlaces (2);
	m_spinL10.SetTrimTrailingZeros (FALSE);
	m_spinL10.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinL10.SetPos(l10);
	m_spinL10.SetBuddy(&m_editLH10);
	m_spinL100.SetDecimalPlaces (2);
	m_spinL100.SetTrimTrailingZeros (FALSE);
	m_spinL100.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinL100.SetPos(l100);
	m_spinL100.SetBuddy(&m_editLH100);
	m_spinL1000.SetDecimalPlaces (2);
	m_spinL1000.SetTrimTrailingZeros (FALSE);
	m_spinL1000.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinL1000.SetPos(l1000);
	m_spinL1000.SetBuddy(&m_editLH1000);
	m_spinLHerb.SetDecimalPlaces (2);
	m_spinLHerb.SetTrimTrailingZeros (FALSE);
	m_spinLHerb.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinLHerb.SetPos(lherb);
	m_spinLHerb.SetBuddy(&m_editLHerb);
	m_spinLWoody.SetDecimalPlaces (2);
	m_spinLWoody.SetTrimTrailingZeros (FALSE);
	m_spinLWoody.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinLWoody.SetPos(lwoody);
	m_spinLWoody.SetBuddy(&m_editLWoody);
	m_spinL20.SetDecimalPlaces (2);
	m_spinL20.SetTrimTrailingZeros (FALSE);
	m_spinL20.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinL20.SetPos(l20);
	m_spinL20.SetBuddy(&m_editL20);
	//moderatecolumn
	m_spinM1.SetDecimalPlaces (2);
	m_spinM1.SetTrimTrailingZeros (FALSE);
	m_spinM1.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinM1.SetPos(Mod1);
	m_spinM1.SetBuddy(&m_editMH1);
	m_spinM10.SetDecimalPlaces (2);
	m_spinM10.SetTrimTrailingZeros (FALSE);
	m_spinM10.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinM10.SetPos(Mod10);
	m_spinM10.SetBuddy(&m_editMH10);
	m_spinM100.SetDecimalPlaces (2);
	m_spinM100.SetTrimTrailingZeros (FALSE);
	m_spinM100.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinM100.SetPos(Mod100);
	m_spinM100.SetBuddy(&m_editMH100);
	m_spinM1000.SetDecimalPlaces (2);
	m_spinM1000.SetTrimTrailingZeros (FALSE);
	m_spinM1000.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinM1000.SetPos(Mod1000);
	m_spinM1000.SetBuddy(&m_editMH1000);
	m_spinMHerb.SetDecimalPlaces (2);
	m_spinMHerb.SetTrimTrailingZeros (FALSE);
	m_spinMHerb.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinMHerb.SetPos(Modherb);
	m_spinMHerb.SetBuddy(&m_editMHerb);
	m_spinMWoody.SetDecimalPlaces (2);
	m_spinMWoody.SetTrimTrailingZeros (FALSE);
	m_spinMWoody.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinMWoody.SetPos(Modwoody);
	m_spinMWoody.SetBuddy(&m_editMWoody);
	m_spinM20.SetDecimalPlaces (2);
	m_spinM20.SetTrimTrailingZeros (FALSE);
	m_spinM20.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinM20.SetPos(Mod20);
	m_spinM20.SetBuddy(&m_editM20);
	//highcolumn
	m_spinH1.SetDecimalPlaces (2);
	m_spinH1.SetTrimTrailingZeros (FALSE);
	m_spinH1.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinH1.SetPos(h1);
	m_spinH1.SetBuddy(&m_editHH1);
	m_spinH10.SetDecimalPlaces (2);
	m_spinH10.SetTrimTrailingZeros (FALSE);
	m_spinH10.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinH10.SetPos(h10);
	m_spinH10.SetBuddy(&m_editHH10);
	m_spinH100.SetDecimalPlaces (2);
	m_spinH100.SetTrimTrailingZeros (FALSE);
	m_spinH100.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinH100.SetPos(h100);
	m_spinH100.SetBuddy(&m_editHH100);
	m_spinH1000.SetDecimalPlaces (2);
	m_spinH1000.SetTrimTrailingZeros (FALSE);
	m_spinH1000.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinH1000.SetPos(h1000);
	m_spinH1000.SetBuddy(&m_editHH1000);
	m_spinHHerb.SetDecimalPlaces (2);
	m_spinHHerb.SetTrimTrailingZeros (FALSE);
	m_spinHHerb.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinHHerb.SetPos(hherb);
	m_spinHHerb.SetBuddy(&m_editHHerb);
	m_spinHWoody.SetDecimalPlaces (2);
	m_spinHWoody.SetTrimTrailingZeros (FALSE);
	m_spinHWoody.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinHWoody.SetPos(hwoody);
	m_spinHWoody.SetBuddy(&m_editHWoody);
	m_spinH20.SetDecimalPlaces (2);
	m_spinH20.SetTrimTrailingZeros (FALSE);
	m_spinH20.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinH20.SetPos(h20);
	m_spinH20.SetBuddy(&m_editH20);
	//extreme column
	m_spinE1.SetDecimalPlaces (2);
	m_spinE1.SetTrimTrailingZeros (FALSE);
	m_spinE1.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinE1.SetPos(e1);
	m_spinE1.SetBuddy(&m_editEH1);
	m_spinE10.SetDecimalPlaces (2);
	m_spinE10.SetTrimTrailingZeros (FALSE);
	m_spinE10.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinE10.SetPos(e10);
	m_spinE10.SetBuddy(&m_editEH10);
	m_spinE100.SetDecimalPlaces (2);
	m_spinE100.SetTrimTrailingZeros (FALSE);
	m_spinE100.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinE100.SetPos(e100);
	m_spinE100.SetBuddy(&m_editEH100);
	m_spinE1000.SetDecimalPlaces (2);
	m_spinE1000.SetTrimTrailingZeros (FALSE);
	m_spinE1000.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinE1000.SetPos(e1000);
	m_spinE1000.SetBuddy(&m_editEH1000);
	m_spinEHerb.SetDecimalPlaces (2);
	m_spinEHerb.SetTrimTrailingZeros (FALSE);
	m_spinEHerb.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinEHerb.SetPos(eherb);
	m_spinEHerb.SetBuddy(&m_editEHerb);
	m_spinEWoody.SetDecimalPlaces (2);
	m_spinEWoody.SetTrimTrailingZeros (FALSE);
	m_spinEWoody.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinEWoody.SetPos(ewoody);
	m_spinEWoody.SetBuddy(&m_editEWoody);
	m_spinE20.SetDecimalPlaces (2);
	m_spinE20.SetTrimTrailingZeros (FALSE);
	m_spinE20.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_spinE20.SetPos(e20);
	m_spinE20.SetBuddy(&m_editE20);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRerapDialog::OnKillfocusLow() 
{
	// TODO: Add your control notification handler code here
	int oldLow = low;
	UpdateData(TRUE);
	if(low == oldLow)
		return;
	if(low > 94 || low <= 1)
		AfxMessageBox("ERROR: Value out of range for LOW.");
	else
	{
		mod1.Format("%d -", low + 1);
		if(mod <= low + 1)
		{
			mod = low + 2;
			high1.Format("%d -", mod + 1);
			if(high <= mod + 2)
			{
				high = mod + 2;
				ext1.Format("%d", high + 1);
			}
		}
		lpcnt = low;
		mpcnt = mod - low;
		hpcnt = high - mod;
		epcnt = 100 - high;
		UpdateData(FALSE);
		RecalcStats();
	}
}

void CRerapDialog::AddStats(int wDir)
{
	for(int c = 0; c < rStats[wDir].nClasses; c++)
	{
		for(int v = 0; v < rStats[wDir].varStats[c].N(); v++)
			stats.Accumulate(rStats[wDir].varStats[c].vals[v]);
	}
}

void CRerapDialog::AddStatsToClasses(int wDir, double p1, double pm, double ph)
{ 
	for(int c = 0; c < rStats[wDir].nClasses; c++)
	{
		int val = rStats[wDir].start + rStats[wDir].step * c;
		if(rStats[wDir].start + rStats[wDir].step * c <= p1)
			for(int v = 0; v < rStats[wDir].varStats[c].N(); v++)
				lStats.Accumulate(rStats[wDir].varStats[c].vals[v]);
		else if(rStats[wDir].start + rStats[wDir].step * c <= pm)
			for(int v = 0; v < rStats[wDir].varStats[c].N(); v++)
				mStats.Accumulate(rStats[wDir].varStats[c].vals[v]);
		else if(rStats[wDir].start + rStats[wDir].step * c <= ph)
			for(int v = 0; v < rStats[wDir].varStats[c].N(); v++)
				hStats.Accumulate(rStats[wDir].varStats[c].vals[v]);
		else
			for(int v = 0; v < rStats[wDir].varStats[c].N(); v++)
				eStats.Accumulate(rStats[wDir].varStats[c].vals[v]);
	}
}

void CRerapDialog::AddClassToFireDangerClass(int c, int fdc)
{
	if(wNE)
		AddWDirStatsToFireDangerClass(0, c, fdc);
	if(wE)
		AddWDirStatsToFireDangerClass(1, c, fdc);
	if(wSE)
		AddWDirStatsToFireDangerClass(2, c, fdc);
	if(wS)
		AddWDirStatsToFireDangerClass(3, c, fdc);
	if(wSW)
		AddWDirStatsToFireDangerClass(4, c, fdc);
	if(wW)
		AddWDirStatsToFireDangerClass(5, c, fdc);
	if(wNW)
		AddWDirStatsToFireDangerClass(6, c, fdc);
	if(wN)
		AddWDirStatsToFireDangerClass(7, c, fdc);
}

void CRerapDialog::AddWDirStatsToFireDangerClass(int wDir, int c, int fdc)
{
	for(int v = 0; v < rStats[wDir].varStats[c].N(); v++)
	{
		switch(fdc)
		{
		case 0:
			lStats.Accumulate(rStats[wDir].varStats[c].vals[v]);
			break;
		case 1:
			mStats.Accumulate(rStats[wDir].varStats[c].vals[v]);
			break;
		case 2:
			hStats.Accumulate(rStats[wDir].varStats[c].vals[v]);
			break;
		case 3:
			eStats.Accumulate(rStats[wDir].varStats[c].vals[v]);
			break;
		}
	}
}

void CRerapDialog::OnClassapply() 
{
	// TODO: Add your control notification handler code here
	int winds = 0;
	UpdateData(TRUE);
	if(!wE && ! wN && !wNE && !wNW && !wS && !wSE && ! wSW && !wW)
	{
		AfxMessageBox("ERROR: At least one wind direction must be selected!");
		avgsApply.EnableWindow(FALSE);
		return;
	}
	l1000 = 0.0f;
	l100 = 0.0f;
	l10 = 0.0f;
	l1 = 0.0f;
	l20 = 0.0f;
	lherb = 0.0f;
	lwoody = 0.0f;
	lkbdi = 0.0f;
	Mod1000 = 0.0f;
	Mod100 = 0.0f;
	Mod10 = 0.0f;
	Mod1 = 0.0f;
	Mod20 = 0.0f;
	Modherb = 0.0f;
	Modwoody = 0.0f;
	Modkbdi = 0.0f;
	e1000 = 0.0f;
	e100 = 0.0f;
	e10 = 0.0f;
	e1 = 0.0f;
	e20 = 0.0f;
	eherb = 0.0f;
	ewoody = 0.0f;
	ekbdi = 0.0f;
	h1000 = 0.0f;
	h100 = 0.0f;
	h10 = 0.0f;
	h1 = 0.0f;
	h20 = 0.0f;
	hherb = 0.0f;
	hwoody = 0.0f;
	hkbdi = 0.0f;
	int c = (lm1 - rStats[0].start) / rStats[0].step;
	int d = (lm2 - rStats[0].start) / rStats[0].step;
	if(lobs > 0)
	{
		c = (lm1 - rStats[0].start) / rStats[0].step;
		d = (lm2 - rStats[0].start) / rStats[0].step;
		if(c < 0)
			c = 0;
		else if(c >= rStats[0].nClasses)
			c = rStats[0].nClasses - 1;
		if(d < 0)
			d = 0;
		else if(d >= rStats[0].nClasses)
			d = rStats[0].nClasses - 1;
		for(int i = c; i <= d; i++)
			AddClassLAvgs(i);
		l1 /= lobs;
		l10 /= lobs;
		l100 /= lobs;
		l1000 /= lobs;
		l20 /= lobs;
		lherb /= lobs;
		lwoody /= lobs;
		lkbdi /= lobs;
	}
	if(mobs > 0)
	{
		c = (mm1 - rStats[0].start) / rStats[0].step;
		d = (mm2 - rStats[0].start) / rStats[0].step;
		if(c < 0)
			c = 0;
		else if(c >= rStats[0].nClasses)
			c = rStats[0].nClasses - 1;
		if(d < 0)
			d = 0;
		else if(d >= rStats[0].nClasses)
			d = rStats[0].nClasses - 1;
		for(int i = c; i <= d; i++)
			AddClassMAvgs(i);
		Mod1 /= mobs;
		Mod10 /= mobs;
		Mod100 /= mobs;
		Mod1000 /= mobs;
		Mod20 /= mobs;
		Modherb /= mobs;
		Modwoody /= mobs;
		Modkbdi /= mobs;
	}
	if(hobs > 0)
	{
		c = (hm1 - rStats[0].start) / rStats[0].step;
		d = (hm2 - rStats[0].start) / rStats[0].step;
		if(c < 0)
			c = 0;
		else if(c >= rStats[0].nClasses)
			c = rStats[0].nClasses - 1;
		if(d < 0)
			d = 0;
		else if(d >= rStats[0].nClasses)
			d = rStats[0].nClasses - 1;
		for(int i = c; i <= d; i++)
			AddClassHAvgs(i);
		h1 /= hobs;
		h10 /= hobs;
		h100 /= hobs;
		h1000 /= hobs;
		h20 /= hobs;
		hherb /= hobs;
		hwoody /= hobs;
		hkbdi /= hobs;
	}
	if(eobs > 0)
	{
		c = (em1 - rStats[0].start) / rStats[0].step;
		d = (em2 - rStats[0].start) / rStats[0].step;
		if(c < 0)
			c = 0;
		else if(c >= rStats[0].nClasses)
			c = rStats[0].nClasses - 1;
		if(d < 0)
			d = 0;
		else if(d >= rStats[0].nClasses)
			d = rStats[0].nClasses - 1;
		for(int i = c; i <= d; i++)
			AddClassEAvgs(i);
		e1 /= eobs;
		e10 /= eobs;
		e100 /= eobs;
		e1000 /= eobs;
		e20 /= eobs;
		eherb /= eobs;
		ewoody /= eobs;
		ekbdi /= eobs;
	}
	UpdateData(FALSE);
}

void CRerapDialog::OnAvgsapply() 
{
	// TODO: Add your control notification handler code here
	double fros, ferc, Fl;
	int Fil, ibi;
	UpdateData(TRUE);
	theApp.m_NFDRS.iSetMoistures(l1, l10, l100, l1000, lwoody, lherb, 0, (short)lkbdi, 15, 15, 3, 0);
	theApp.m_NFDRS.iCalcIndexes((int)l20, slopeClass, &fros, &lsc, &ferc, &Fl, &Fil, &ibi);  
	lerc = (int)Round(ferc);
	theApp.m_NFDRS.iSetMoistures(Mod1, Mod10, Mod100, Mod1000, Modwoody, Modherb, 0, (short)Modkbdi, 10, 10, 3, 0);
	theApp.m_NFDRS.iCalcIndexes((int)Mod20, slopeClass, &fros, &Modsc, &ferc, &Fl, &Fil, &ibi);  
	Moderc = (int)Round(ferc);
	theApp.m_NFDRS.iSetMoistures(h1, h10, h100, h1000, hwoody, hherb, 0, (short)hkbdi, 5, 5, 3, 0);
	theApp.m_NFDRS.iCalcIndexes((int)h20, slopeClass, &fros, &hsc, &ferc, &Fl, &Fil, &ibi);  
	herc = (int)Round(ferc);
	theApp.m_NFDRS.iSetMoistures(e1, e10, e100, e1000, ewoody, eherb, 0, (short)ekbdi, 0, 0, 3, 0);
	theApp.m_NFDRS.iCalcIndexes((int)e20, slopeClass, &fros, &esc, &ferc, &Fl, &Fil, &ibi);  
	eerc = (int)Round(ferc);
	UpdateData(FALSE);
}

void CRerapDialog::OnKillfocusMed2() 
{
	// TODO: Add your control notification handler code here
	int oldMod = mod;
	UpdateData(TRUE);
	if(mod == oldMod)
		return;
	if(mod > 96 || mod <= low + 1)
		AfxMessageBox("ERROR: Value out of range for MOD.");
	else
	{
		high1.Format("%d -", mod + 1);
		if(high <= mod + 2)
		{
			high = mod + 2;
			//hpcnt = high - mod;
			ext1.Format("%d", high + 1);
		}
		mpcnt = mod - low;
		hpcnt = high - mod;
		epcnt = 100 - high;
		UpdateData(FALSE);
		RecalcStats();
	}
	
}

void CRerapDialog::OnKillfocusHigh2() 
{
	// TODO: Add your control notification handler code here
	int oldHigh = high;
	UpdateData(TRUE);
	if(high == oldHigh)
		return;
	if(high > 98 || high <= mod + 1)
		AfxMessageBox("ERROR: Value out of range for HIGH.");
	else
	{
		ext1.Format("%d", high + 1);
		hpcnt = high - mod;
		epcnt = 100 - high;
		UpdateData(FALSE);
		RecalcStats();
	}
	
}

void CRerapDialog::InsertRerapObs(CList<RerapObs, RerapObs&> *list, RerapObs& rObs)
{
	bool inserted = false;
	POSITION pos = list->GetHeadPosition();
	while(pos)
	{
		RerapObs tObs = list->GetAt(pos);
		if(tObs.var > rObs.var)
		{
			list->InsertBefore(pos, rObs);
			inserted = true;
			break;
		}
		list->GetNext(pos);
	}
	if(!inserted)
		list->AddTail(rObs);
}

BOOL CRerapDialog::ReadData()
{
	CWaitCursor wait;
	CString query;
	int startDay, startMonth, endDay, endMonth;
	//int varIDs[8];
	endDay = fpSet->m_EndDay;
	endMonth = MonthChtoInt(fpSet->m_EndMonth);
	startDay = fpSet->m_StartDay;
	startMonth = MonthChtoInt(fpSet->m_StartMonth);
	CSIGStationSet tStaSet(fpSet->m_pDatabase);
	tStaSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
	tStaSet.Open();
	

	if (isNFDRS2016(tStaSet.m_NFDRSFM[0])) // we need hourly data for new calculator
		query.Format("([StationID] = '%6.6s')", tStaSet.m_StationID);
	else
		query.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", tStaSet.m_StationID);
	//query.Format("([StationID] = '%s' AND ([ObsType] = 'O' OR ([ObsType] = 'R' AND [Hour] = %d AND [SOW] IS NOT NULL)))", tStaSet.m_StationID, tStaSet.m_RegSchdObs);
	tStaSet.Close();

	//use all records to keep models in sync
	//query.Format("Year([ObsDate]) >= %d and Year([ObsDate]) <= %d and [StationID] = '%6.6s' and [ObsType] = 'O'", 
		//fpSet->m_StartYear, fpSet->m_EndYear, fpSet->m_SIG_Station);
	CWxSet wxSet(fpSet->m_pDatabase);
	wxSet.m_strSort = _T("[ObsDate]");
	wxSet.m_strFilter = _T(query);
	wxSet.Open();
	if(wxSet.IsBOF() && wxSet.IsEOF())
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: No matching weather observation records.\n Redefine Working Set Query. \n\n"));
		AfxMessageBox(strMsg);
		wxSet.Close();
		return FALSE;
	}

	/*CCancelDialog *cd = NULL;
	
	cd = new CCancelDialog();

	if (cd != NULL)
	{
		BOOL ret = cd->Create(IDD_CANCELDIALOG,this);

      if (!ret)   //Create failed.
      {
    
		 cd = 0;
      } else
	  {
	     cd->Reset();
         cd->ShowWindow(SW_SHOW);
	     cd->SetStatusString("Reading Weather Data");
		 cd->SetTitle("Processing Working Set");
	  }
	}*/


	
	
	int dOrder = DOY_NORMAL;
	COleDateTime d1, d2;
	d1.SetDate(BASISYEAR, startMonth, startDay);
	d2.SetDate(BASISYEAR, endMonth, endDay);
	COleDateTimeSpan day(1, 0, 0, 0 );
	if(d1 == d2 || d2 + day == d1)
		dOrder = DOY_EQUAL;
	else if(d1 > d2)
		dOrder = DOY_FLIPPED;
	CSIGStationSet staSet(fpSet->m_pDatabase);
	staSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
	staSet.Open();
	int sKBDI = 0;
	if(!staSet.IsFieldNull(&staSet.m_StartKBDI))
		sKBDI = staSet.m_StartKBDI;
	tTitle.Format("%s - Percentile Weather for RERAP: %s - Model: %d%s%d%c%c%d",
		fpSet->m_SIG_Station, varString, staSet.m_Use88 ? 8 : 7, staSet.m_NFDRSFM, staSet.m_SlopeCls,
		staSet.m_HerbAnnual ? 'A' : 'P', staSet.m_Deciduous ? 'D' : 'E', staSet.m_ClimateCls);
	slopeClass = staSet.m_SlopeCls;
	CList<RerapObs, RerapObs&> rListN;
	CList<RerapObs, RerapObs&> rListNE;
	CList<RerapObs, RerapObs&> rListNW;
	CList<RerapObs, RerapObs&> rListE;
	CList<RerapObs, RerapObs&> rListW;
	CList<RerapObs, RerapObs&> rListS;
	CList<RerapObs, RerapObs&> rListSE;
	CList<RerapObs, RerapObs&> rListSW;

	bool reCalc2016 = false;
	reCalc2016 = theApp.m_ForceNFDRS2016Recompute;
	if (!reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0]))
	{//check wxSet for previously run NFDRS2016
		wxSet.Requery();
		//wxSet.m_strFilter.Format("([StationID] = '%s' AND [DailyObs] = 1)", m_sigStaID);
		if (wxSet.IsFieldNull(&wxSet.m_FM1) || wxSet.IsFieldNull(&wxSet.m_FM10) || wxSet.IsFieldNull(&wxSet.m_FM100) || wxSet.IsFieldNull(&wxSet.m_FM1000) || wxSet.IsFieldNull(&wxSet.m_FuelTemperature)
			|| wxSet.m_FM1 <= 0.0 || wxSet.m_FM10 <= 0.0 || wxSet.m_FM100 <= 0.0 || wxSet.m_FM1000 <= 0.0 || wxSet.m_FuelTemperature <= -999.0)
			reCalc2016 = true;
		if (!reCalc2016)
		{
			wxSet.MoveLast();
			if (wxSet.IsFieldNull(&wxSet.m_FM1) || wxSet.IsFieldNull(&wxSet.m_FM10) || wxSet.IsFieldNull(&wxSet.m_FM100) || wxSet.IsFieldNull(&wxSet.m_FM1000) || wxSet.IsFieldNull(&wxSet.m_FuelTemperature)
				|| wxSet.m_FM1 <= 0.0 || wxSet.m_FM10 <= 0.0 || wxSet.m_FM100 <= 0.0 || wxSet.m_FM1000 <= 0.0 || wxSet.m_FuelTemperature <= -999.0)
				reCalc2016 = true;
		}
		wxSet.MoveFirst();
	}
	//NFDR2016Calc *nfdr2016;
	CLFIengine lfi(fpSet->m_pDatabase, 0, fpSet->m_SIG_Station), lfiHerb(fpSet->m_pDatabase, HerbParams, fpSet->m_SIG_Station, staSet.m_HerbAnnual),
		lfiWoody(fpSet->m_pDatabase, WoodyParams, fpSet->m_SIG_Station);
	bool useLFI = true;
	if (isNFDRS2016(staSet.m_NFDRSFM[0]) || useLFI)
	{ // new NFDRS2016 calc
		//nfdr2016 = new NFDR2016Calc((int)staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true);
		//theApp.m_NFDRS2016.Init((int)staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true, lfi.GetHerbMaxGSI(), lfi.GetHerbGreenup());
		theApp.m_NFDRS2016.Init(staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true, staSet.m_HerbAnnual, staSet.m_KBDIThreshold);
		theApp.m_NFDRS2016.iSetFuelModel(staSet.m_NFDRSFM[0]);
		theApp.m_NFDRS2016.SetGSIParams(lfi.GetHerbMaxGSI(), lfi.GetHerbGreenup(), lfi.GetTminMin(), lfi.GetTminMax(), lfi.GetVPDMin(), lfi.GetVPDMax(), 
			lfi.GetDaylenMin(), lfi.GetDaylenMax(), lfi.GetMAPeriod(), lfi.IsUsingVPDAvg(), lfi.GetNumPrecipDays(), lfi.GetRTPcpMin(), lfi.GetRTPcpMax(), lfi.GetUseRTPrecip());
		theApp.m_NFDRS2016.SetHerbGSIparams(lfiHerb.GetHerbMaxGSI(), lfiHerb.GetHerbGreenup(), lfiHerb.GetTminMin(), lfiHerb.GetTminMax(), 
			lfiHerb.GetVPDMin(), lfiHerb.GetVPDMax(), lfiHerb.GetDaylenMin(), lfiHerb.GetDaylenMax(), lfiHerb.GetMAPeriod(), lfiHerb.IsUsingVPDAvg(), 
			lfiHerb.GetNumPrecipDays(), lfiHerb.GetRTPcpMin(), lfiHerb.GetRTPcpMax(), lfiHerb.GetUseRTPrecip());
		theApp.m_NFDRS2016.SetWoodyGSIparams(lfiWoody.GetWoodyMaxGSI(), lfiWoody.GetWoodyGreenup(), lfiWoody.GetTminMin(), lfiWoody.GetTminMax(), 
			lfiWoody.GetVPDMin(), lfiWoody.GetVPDMax(), lfiWoody.GetDaylenMin(), lfiWoody.GetDaylenMax(), lfiWoody.GetMAPeriod(), lfiWoody.IsUsingVPDAvg(), 
			lfiWoody.GetNumPrecipDays(), lfiWoody.GetRTPcpMin(), lfiWoody.GetRTPcpMax(), lfiWoody.GetUseRTPrecip());
		theApp.m_NFDRS2016.SetStartKBDI(staSet.m_StartKBDI);
		theApp.m_NFDRS2016.SetSCMax(staSet.GetSCM(staSet.m_NFDRSFM[0]));
		theApp.m_NFDRS2016.SetMxdHumid(staSet.GetMxHumid(staSet.m_NFDRSFM[0]));
	}
	if (!isNFDRS2016(staSet.m_NFDRSFM[0]))
	{   // "old" NFDRS calc
		theApp.m_NFDRS.iInitialize(staSet.m_Use88 ? 88 : 78,
			staSet.m_ClimateCls, DayOfYear(staSet.m_FreezeJulian), DayOfYear(staSet.m_GreenJulian),
			(int)(staSet.m_LatDegrees), staSet.m_HerbAnnual, staSet.m_Deciduous, staSet.m_FM1Eq10,
			staSet.m_Start1000 - 5.0, staSet.m_Start1000, sKBDI, staSet.m_AvgPrecip);
		theApp.m_NFDRS.iSetFuelModel(staSet.m_NFDRSFM[0], 0);
		if (!staSet.IsFieldNull(&staSet.m_UseDormant) && !staSet.IsFieldNull(&staSet.m_DormantJulian))
		{
			if (staSet.m_UseDormant && staSet.m_DormantJulian.GetStatus() == COleDateTime::valid)
				theApp.m_NFDRS.iSetJulianShrubDormant(DayOfYear(staSet.m_DormantJulian));
		}
	}
	bool bUseStick = true;
	if(!staSet.IsFieldNull(&staSet.m_UseStick))
		bUseStick = staSet.m_UseStick;
	//crunch data...
	int iSC, iFIL, iBI;
	int iIC, iStage, iKBDI, iRainEvent, init = 1, iDeclareGreenUp = 0, iDeclareFreeze = 0;
	double f1, f10, f100, f1000, fHerb, fWood, fGren, fX1000, fROS, fERC, fFL,
		lfiVal, lfiHerbVal, lfiWoodyVal;
		//ffmc, dmc, dc, bui, isi, fwi, dsr;
	COleDateTime yesterday, wxDay;//used to skip duplicates
	while(!wxSet.IsEOF())// && (!cd || !cd->Abort()))
	{
		if(wxSet.IsFieldNull(&wxSet.m_Temp) 
			|| wxSet.IsFieldNull(&wxSet.m_RH)
			|| wxSet.IsFieldNull(&wxSet.m_ObsDate))
		{
			wxSet.MoveNext();
			continue;
		}
		wxDay.SetDateTime(wxSet.m_ObsDate.GetYear(), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay(), 
				wxSet.m_ObsDate.GetHour(), wxSet.m_ObsDate.GetMinute(), wxSet.m_ObsDate.GetSecond());
		if(!init && yesterday >= wxDay)
		{
			wxSet.MoveNext();
			continue;
		}
			// new custom init check    12/2006
			// if this is first record for new year, look for inits
			if (yesterday.GetYear() != wxDay.GetYear())
			{
				
                CCustomInitSet ciSet(fpSet->m_pDatabase);
				ciSet.m_strFilter.Format("[SIG_Station]='%s' AND [Year]=%d and [Enabled] <> 0", wxSet.m_StationID,wxDay.GetYear());
				ciSet.Open();
				bool usedNFDRScust = false, usedCANcust = false;
				if (! ciSet.IsEOF())
				{
                    // enabled record exists for this station/year. get values and re-initialize
                   int newGreen;//, lookFreeze;

				   if (!ciSet.IsFieldNull(&ciSet.m_GreenJulian) &&
					   !ciSet.IsFieldNull(&ciSet.m_StartKBDI) &&
					   !ciSet.IsFieldNull(&ciSet.m_Start1000))
				   {
		                int newGreen = DayOfYear(ciSet.m_GreenJulian);
		           
						theApp.m_NFDRS.iSetAnnualInit(newGreen, ciSet.m_Start1000, ciSet.m_StartKBDI);
						usedNFDRScust = true;
						//customInits = true;

				   } 
				  /* if(useCanadian)
				   {
						if (!ciSet.IsFieldNull(&ciSet.m_FFMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DC))
						{
		                   canadian.ReInitialize(ciSet.m_FFMC,ciSet.m_DMC,ciSet.m_DC);
						   usedCANcust = true;
						  // customInits = true;
						}
				   }*/
				}
				if(!usedNFDRScust)
				{
					//set to default station values
					theApp.m_NFDRS.iSetAnnualInit(DayOfYear(staSet.m_GreenJulian), staSet.m_Start1000, sKBDI);
				}
				//if(!usedCANcust)
				//	canadian.ReInitialize(85, 6, 15);
                ciSet.Close();
			}
		yesterday = wxDay;//wxSet.m_ObsDate;
		//if (cd)
		//   cd->Increment();
		//working variables
		int tmp, tmpMax, tmpMin, rh, rhMax, rhMin, pptDur, sow, greenHerb, greenShrub,
			season, windSpd, windDir, slopeClass, wxMonth, wxDay, wetFlag, snowFlag;
		double pptAmt, omc10; 
		tmp = wxSet.m_Temp;
		tmpMax = wxSet.IsFieldNull(&wxSet.m_TmpMax) ? -10000 : wxSet.m_TmpMax;
		tmpMin = wxSet.IsFieldNull(&wxSet.m_TmpMin) ? -10000 : wxSet.m_TmpMin;
		rh = max(wxSet.m_RH, 1);
		rhMax = wxSet.IsFieldNull(&wxSet.m_RHMax) ? -10000 : wxSet.m_RHMax;
		rhMin = wxSet.IsFieldNull(&wxSet.m_RHMin) ? -10000 : max(wxSet.m_RHMin, 1);
		pptDur = wxSet.IsFieldNull(&wxSet.m_PPTDUR) ? 0 : wxSet.m_PPTDUR;
		sow = wxSet.ResolveSOW();//.IsFieldNull(&wxSet.m_SOW) ? 0 : wxSet.m_SOW;
		greenHerb = wxSet.IsFieldNull(&wxSet.m_GreenHerb) ? 0 : wxSet.m_GreenHerb;
		greenShrub = wxSet.IsFieldNull(&wxSet.m_GreenShrub) ? 0 : wxSet.m_GreenShrub;
		season = wxSet.IsFieldNull(&wxSet.m_Season) ? 0 : wxSet.m_Season;
		windSpd = wxSet.IsFieldNull(&wxSet.m_WS) ? 0 : wxSet.m_WS;
		windDir = wxSet.IsFieldNull(&wxSet.m_WDir) ? 0 : wxSet.m_WDir;
		slopeClass = staSet.m_SlopeCls;
		slopeClass = max(slopeClass, 1);
		slopeClass = min(slopeClass, 5);
		pptAmt = wxSet.IsFieldNull(&wxSet.m_PPTAMT) ? 0.0 : wxSet.m_PPTAMT;
		//omc10 = wxSet.IsFieldNull(&wxSet.m_OMC10) ? -1000.0 : wxSet.m_OMC10;
		omc10 = (bUseStick && !wxSet.IsFieldNull(&wxSet.m_OMC10)) ? wxSet.m_OMC10 : -1000.0;
		wxMonth = wxSet.m_ObsDate.GetMonth();
		wxDay = wxSet.m_ObsDate.GetDay();
		wetFlag = wxSet.ResolveWetFlag();//IsFieldNull(&wxSet.m_WetFlag) ? 0 : wxSet.m_WetFlag;
		snowFlag = wxSet.IsFieldNull(&wxSet.m_SnowFlag) ? 0 : wxSet.m_SnowFlag;
		double hourlyPrecip = wxSet.IsFieldNull(&wxSet.m_HourlyPrecip) ? 0.0 : wxSet.m_HourlyPrecip;
		//calculate model outputs
		//NFDRS
		if (isNFDRS2016(staSet.m_NFDRSFM[0]) || useLFI)
		{ // new NFDRS2016 calc

			int year1 = wxSet.m_ObsDate.GetYear();
			int month1 = wxSet.m_ObsDate.GetMonth();
			int day1 = wxSet.m_ObsDate.GetDay();
			int hour1 = wxSet.m_ObsDate.GetHour();
			int julian1 = wxSet.m_ObsDate.GetDayOfYear();
			double pptAcc = 0;
			double solarRad = wxSet.m_SolarRadiation;
			bool isDailyObs = wxSet.IsFieldNull(&wxSet.m_DailyObs) ? false : wxSet.m_DailyObs;

			if (reCalc2016)
				theApp.m_NFDRS2016.Update(year1, month1, day1, hour1, julian1, tmp, tmpMin, tmpMax, rh, rhMin, hourlyPrecip, pptAmt, solarRad, windSpd, snowFlag,
				(isDailyObs) ? hour1 : -1);//staSet.m_RegSchdObs);
			else if (wxSet.m_DailyObs)
			{
				theApp.m_NFDRS2016.UpdateDaily(year1, month1, day1, julian1, (double)tmp, (double)tmpMin, (double)tmpMax, (double)rh, (double)rhMin,
					pptAmt, (double)windSpd, wxSet.m_FM1, wxSet.m_FM10, wxSet.m_FM100, wxSet.m_FM1000, wxSet.m_FuelTemperature, snowFlag);
			}
			else
			{
				wxSet.MoveNext();
				continue;
			}
			f1 = theApp.m_NFDRS2016.MC1;
			f10 = theApp.m_NFDRS2016.MC10;
			f100 = theApp.m_NFDRS2016.MC100;
			f1000 = theApp.m_NFDRS2016.MC1000;
			lfiHerbVal = fHerb = theApp.m_NFDRS2016.MCHERB;
			lfiWoodyVal = fWood = theApp.m_NFDRS2016.MCWOOD;
			lfiVal = theApp.m_NFDRS2016.m_GSI;
			//			iStage = theApp.m_NFDRS2016.Stage;
//			fGren = theApp.m_NFDRS2016.Gren;
			fX1000 = 0; // fixme
//			iRainEvent = theApp.m_NFDRS2016.RainEvent;
			iKBDI = theApp.m_NFDRS2016.KBDI;
			fROS = 0; //fixme
			iSC = theApp.m_NFDRS2016.SC;
			fERC = theApp.m_NFDRS2016.ERC;
			fFL = 0; //fixme
			iFIL = 0; //fixme
			iBI = theApp.m_NFDRS2016.BI;
			iIC = theApp.m_NFDRS2016.IC;
			if (hour1 != staSet.m_RegSchdObs)
			{
				init = 0;
				wxSet.MoveNext();//GetNext();
				continue;
			}

		}
		if (!isNFDRS2016(staSet.m_NFDRSFM[0]))
		{ // "old" NFDRS calc
			theApp.m_NFDRS.iCalcMoistWetFlagSnowFlag(init, tmp, rh,
				tmpMax,
				rhMax,
				tmpMin,
				rhMin,
				pptDur,
				pptAmt,
				sow,
				omc10,
				DayOfYear(wxSet.m_ObsDate),
				wxSet.m_ObsDate.GetYear(),
				iDeclareGreenUp,
				iDeclareFreeze,
				greenHerb,
				greenShrub,
				season,
				wetFlag, snowFlag,
				&f1, &f10, &f100, &f1000, &fHerb, &fWood, &iStage, &fGren, &fX1000, &iRainEvent, &iKBDI);
			theApp.m_NFDRS.iCalcIndexes(windSpd, slopeClass, &fROS, &iSC, &fERC, &fFL, &iFIL, &iBI);
			theApp.m_NFDRS.iCalcIC(tmp, sow, f1, iSC, &iIC);
		}
		init = 0;
		//check for a valid wind direction
		if(windDir < 0 || windDir > 8)
		{
			wxSet.MoveNext();
			continue;
		}
		//OK, now see if in date interval
		switch(dOrder)
		{
		case DOY_EQUAL:
		case DOY_NORMAL:
			if(wxMonth < startMonth || wxMonth > endMonth
				|| (wxMonth == startMonth && wxDay < startDay)
				|| (wxMonth == endMonth && wxDay > endDay))
			{
				wxSet.MoveNext();
				continue;
			}
			break;
		case DOY_FLIPPED:
			if(wxMonth < startMonth && wxMonth > endMonth
				|| (wxMonth == startMonth && wxDay < startDay)
				|| (wxMonth == endMonth && wxDay > endDay))
			{
				wxSet.MoveNext();
				continue;
			}
			break;
		}//END SWITCH dOrder
		//have a record we want to add to rList
		bool goodRecord = true;
		double val;
		switch(varID)
		{
		case 1:
			val = tmp;
			break;
		case 2:
			if(!wxSet.IsFieldNull(&wxSet.m_TmpMax) && !wxSet.IsFieldNull(&wxSet.m_TmpMin))
				val = (double)(wxSet.m_TmpMax + wxSet.m_TmpMin) / 2.0;
			else
				goodRecord = false;
			break;
		case 3:
			if(!wxSet.IsFieldNull(&wxSet.m_TmpMin))
				val = (double)wxSet.m_TmpMin;
			else
				goodRecord = false;
			break;
		case 4:
			if(!wxSet.IsFieldNull(&wxSet.m_TmpMax))
				val = (double)wxSet.m_TmpMax;
			else
				goodRecord = false;
			break;
		case 5:
			if(!wxSet.IsFieldNull(&wxSet.m_RH))
				val = (double)max(wxSet.m_RH, 1);
			else
				goodRecord = false;
			break;
		case 6:
			if(!wxSet.IsFieldNull(&wxSet.m_RHMax) && !wxSet.IsFieldNull(&wxSet.m_RHMin))
				val = (double)(wxSet.m_RHMax + max(wxSet.m_RHMin, 1)) / 2.0;
			else
				goodRecord = false;
			break;
		case 7:
			if(!wxSet.IsFieldNull(&wxSet.m_RHMin))
				val = (double)max(wxSet.m_RHMin, 1);
			else
				goodRecord = false;
			break;
		case 8:
			if(!wxSet.IsFieldNull(&wxSet.m_RHMax))
				val = (double)wxSet.m_RHMax;
			else
				goodRecord = false;
			break;
		case 9:
			if(!wxSet.IsFieldNull(&wxSet.m_PPTAMT))
				val = (double)wxSet.m_PPTAMT;
			else
				goodRecord = false;
			break;
		case 10:
			if(!wxSet.IsFieldNull(&wxSet.m_PPTDUR))
				val = (double)wxSet.m_PPTDUR;
			else
				goodRecord = false;
			break;
		case 11:
			if(!wxSet.IsFieldNull(&wxSet.m_WS))// && !wxSet.IsFieldNull(&wxSet.m_WDir))
				val = (double)wxSet.m_WS;
			else
				goodRecord = false;
			break;
		case 12:
			val = (double)iSC;
			break;
		case 13:
			val = fERC;// Round(fERC);
			break;
		case 14:
			val = (double)iBI;
			break;
		case 15:
			val = (double)iKBDI;
			break;
		case 16: //Ignition component
			val = (double)iIC;
			break;
		case 17:
			val = f1;
			break;
		case 18:
			val = f10;
			break;
		case 19:
			val = f100;
			break;
		case 20:
			val = f1000;
			break;
		case 21:
			val = fHerb;
			break;
		case 22:
			val = fWood;
			break;
			//cases 23 - 29 canadian model
		/*case 23:
			val = ffmc;
			break;
		case 24:
			val = dmc;
			break;
		case 25:
			val = dc;
			break;
		case 26:
			val = isi;
			break;
		case 27:
			val = bui;
			break;
		case 28:
			val = fwi;
			break;
		case 29:
			val = dsr;
			break;*/
		default:
			val = 0;
			goodRecord = false;
		}//end switch varID
		if(goodRecord)
		{
			RerapObs rObs;
			rObs.var = val;
			rObs.fm1 = f1;
			rObs.fm10 = f10;
			rObs.fm100 = f100;
			rObs.fm1000 = f1000;
			rObs.herb = fHerb;
			rObs.woody = fWood;
			rObs.wind = windSpd;
			rObs.kbdi = iKBDI;
			//insert rObs into correct rList
			switch(windDir)
			{
			case 1:
				InsertRerapObs(&rListN, rObs);
				break;
			case 2:
				InsertRerapObs(&rListNE, rObs);
				break;
			case 3:
				InsertRerapObs(&rListE, rObs);
				break;
			case 4:
				InsertRerapObs(&rListSE, rObs);
				break;
			case 5:
				InsertRerapObs(&rListS, rObs);
				break;
			case 6:
				InsertRerapObs(&rListSW, rObs);
				break;
			case 7:
				InsertRerapObs(&rListW, rObs);
				break;
			case 8:
				InsertRerapObs(&rListNW, rObs);
				break;
			}
			nRecs++;
		}
		wxSet.MoveNext();
	}
	wxSet.Close();
	staSet.Close();
	//if (cd)
	 //  cd->SetStatusString("Building Rerap Statistics...");
	//NOW FILL RerapStats froM lists
	/*******************************************************************************************************************
		CHECK LISTS FOR EMPTY!!!!!!*/
	BOOL ret = TRUE;
	//if(!cd || !cd->Abort())
	//{
		double vMin = 10000000.0, vMax = -10000000.0;
		RerapObs rObs;
		if(rListN.GetCount() > 0)
		{
			rObs = rListN.GetHead();
			vMin = rObs.var;
		}
		if(rListNE.GetCount() > 0)
		{
			rObs = rListNE.GetHead();
			vMin = min(vMin, rObs.var);
		}
		if(rListE.GetCount() > 0)
		{
			rObs = rListE.GetHead();
			vMin = min(vMin, rObs.var);
		}
		if(rListSE.GetCount() > 0)
		{
			rObs = rListSE.GetHead();
			vMin = min(vMin, rObs.var);
		}
		if(rListS.GetCount() > 0)
		{
			rObs = rListS.GetHead();
			vMin = min(vMin, rObs.var);
		}
		if(rListSW.GetCount() > 0)
		{
			rObs = rListSW.GetHead();
			vMin = min(vMin, rObs.var);
		}
		if(rListW.GetCount() > 0)
		{
			rObs = rListW.GetHead();
			vMin = min(vMin, rObs.var);
		}
		if(rListNW.GetCount() > 0)
		{
			rObs = rListNW.GetHead();
			vMin = min(vMin, rObs.var);
		}
		if(rListN.GetCount() > 0)
		{
			rObs = rListN.GetTail();
			vMax = rObs.var;
		}
		if(rListNE.GetCount() > 0)
		{
			rObs = rListNE.GetTail();
			vMax = max(vMax, rObs.var);
		}
		if(rListE.GetCount() > 0)
		{
			rObs = rListE.GetTail();
			vMax = max(vMax, rObs.var);
		}
		if(rListSE.GetCount() > 0)
		{
			rObs = rListSE.GetTail();
			vMax = max(vMax, rObs.var);
		}
		if(rListS.GetCount() > 0)
		{
			rObs = rListS.GetTail();
			vMax = max(vMax, rObs.var);
		}
		if(rListSW.GetCount() > 0)
		{
			rObs = rListSW.GetTail();
			vMax = max(vMax, rObs.var);
		}
		if(rListW.GetCount() > 0)
		{
			rObs = rListW.GetTail();
			vMax = max(vMax, rObs.var);
		}
		if(rListNW.GetCount() > 0)
		{
			rObs = rListNW.GetTail();
			vMax = max(vMax, rObs.var);
		}
		rStats[0].BuildStats(&rListN, vMin, vMax);
		rStats[1].BuildStats(&rListNE, vMin, vMax);
		rStats[2].BuildStats(&rListE, vMin, vMax);
		rStats[3].BuildStats(&rListSE, vMin, vMax);
		rStats[4].BuildStats(&rListS, vMin, vMax);
		rStats[5].BuildStats(&rListSW, vMin, vMax);
		rStats[6].BuildStats(&rListW, vMin, vMax);
		rStats[7].BuildStats(&rListNW, vMin, vMax);
	//}
	//else
	//	ret = FALSE;
	//CLEAN UP
	rListN.RemoveAll();
	rListNE.RemoveAll();
	rListNW.RemoveAll();
	rListE.RemoveAll();
	rListW.RemoveAll();
	rListSE.RemoveAll();
	rListSW.RemoveAll();
	rListS.RemoveAll();
	//if (cd)
	//	cd->DestroyWindow();

	AfxGetMainWnd()->EnableWindow(TRUE);
	return ret;
}

void CRerapDialog::ClearValues()
{
	hobs = 0;
//	hmed = 0.0;
	lobs = 0;
//	lmed = 0.0;
//	mmed = 0.0;
	mobs = 0;
//	emed = 0.0;
	eobs = 0;
	l1000 = 0.0f;
	l100 = 0.0f;
	l10 = 0.0f;
	l1 = 0.0f;
	l20 = 0.0f;
	lherb = 0.0f;
	lwoody = 0.0f;
	Mod1000 = 0.0f;
	Mod100 = 0.0f;
	Mod10 = 0.0f;
	Mod1 = 0.0f;
	Mod20 = 0.0f;
	Modherb = 0.0f;
	Modwoody = 0.0f;
	e1000 = 0.0f;
	e100 = 0.0f;
	e10 = 0.0f;
	e1 = 0.0f;
	e20 = 0.0f;
	eherb = 0.0f;
	ewoody = 0.0f;
	h1000 = 0.0f;
	h100 = 0.0f;
	h10 = 0.0f;
	h1 = 0.0f;
	h20 = 0.0f;
	hherb = 0.0f;
	hwoody = 0.0f;
	eerc = 0;
	esc = 0;
	herc = 0;
	hsc = 0;
	lerc = 0;
	lsc = 0;
	Moderc = 0;
	Modsc = 0;
	lm1 = lm2 = mm1 = mm2 = hm1 = hm2 = em1 = em2 = 0;
	lkbdi = Modkbdi = hkbdi = ekbdi = 0.0;
}


void CRerapDialog::OnE() 
{
	// TODO: Add your control notification handler code here
	RecalcStats();
}

void CRerapDialog::OnN() 
{
	// TODO: Add your control notification handler code here
	RecalcStats();	
}

void CRerapDialog::OnNe() 
{
	// TODO: Add your control notification handler code here
	RecalcStats();
}

void CRerapDialog::OnNw() 
{
	// TODO: Add your control notification handler code here
	RecalcStats();
}

void CRerapDialog::OnS() 
{
	// TODO: Add your control notification handler code here
	RecalcStats();
}

void CRerapDialog::OnSe() 
{
	// TODO: Add your control notification handler code here
	RecalcStats();
}

void CRerapDialog::OnSw() 
{
	// TODO: Add your control notification handler code here
	RecalcStats();
}

void CRerapDialog::OnW() 
{
	// TODO: Add your control notification handler code here
	RecalcStats();
}

void CRerapDialog::RecalcStats()
{
	UpdateData(TRUE);
	ClearValues();
	lStats.Reset();
	mStats.Reset();
	hStats.Reset();
	eStats.Reset();
	stats.Reset();
	windDays = 0;
	if(wN)
		AddStats(7); 
	if(wNE)
		AddStats(0);
	if(wE)
		AddStats(1); 
	if(wSE)
		AddStats(2);
	if(wS)
		AddStats(3); 
	if(wSW)
		AddStats(4);
	if(wW)
		AddStats(5); 
	if(wNW)
		AddStats(6);
	stats.Complete();
	windDays = stats.N();
	BuildFrequencyText();
	/*double plw, pm, ph;
	plw = stats.Percentile(((double)low) / 100.0);
	pm = stats.Percentile(((double)mod) / 100.0);
	ph = stats.Percentile(((double)high) / 100.0);
	if(wN)
		AddStatsToClasses(7, plw, pm, ph); 
	if(wNE)
		AddStatsToClasses(0, plw, pm, ph);
	if(wE)
		AddStatsToClasses(1, plw, pm, ph); 
	if(wSE)
		AddStatsToClasses(2, plw, pm, ph);
	if(wS)
		AddStatsToClasses(3, plw, pm, ph); 
	if(wSW)
		AddStatsToClasses(4, plw, pm, ph);
	if(wW)
		AddStatsToClasses(5, plw, pm, ph); 
	if(wNW)
		AddStatsToClasses(6, plw, pm, ph);*/
	lStats.Complete();
	mStats.Complete();
	hStats.Complete();
	eStats.Complete();
	if(lStats.N() > 0)
	{
		//figure median class...
		double m = lStats.Median();
		int c = (int)((m - rStats[0].start) / rStats[0].step);
		if(c < 0)
			c = 0;
		else if(c >= rStats[0].nClasses)
			c = rStats[0].nClasses - 1;
		lm1 = lm2 = rStats[0].start + rStats[0].step * c; 
		lobs = ClassN(c);
	}
	if(mStats.N() > 0)
	{
		//figure median class...
		double m = mStats.Median();
		int c = (int)((m - rStats[0].start) / rStats[0].step);
		if(c < 0)
			c = 0;
		else if(c >= rStats[0].nClasses)
			c = rStats[0].nClasses - 1;
		mm1 = mm2 = rStats[0].start + rStats[0].step * c;
		mobs = ClassN(c);
	}
	if(hStats.N() > 0)
	{
		//figure median class...
		double m = hStats.Median();
		int c = (int)((m - rStats[0].start) / rStats[0].step);
		if(c < 0)
			c = 0;
		else if(c >= rStats[0].nClasses)
			c = rStats[0].nClasses - 1;
		hm1 = hm2 = rStats[0].start + rStats[0].step * c;
		hobs = ClassN(c);
	}
	if(eStats.N() > 0)
	{
		//figure median class...
		double m = eStats.Median();
		int c = (int)((m - rStats[0].start) / rStats[0].step);
		if(c < 0)
			c = 0;
		else if(c >= rStats[0].nClasses)
			c = rStats[0].nClasses - 1;
		em1 = em2 = rStats[0].start + rStats[0].step * c;
		eobs = ClassN(c);
	}
	//lStats.Reset();
//	mStats.Reset();
//	hStats.Reset();
//	eStats.Reset();
	UpdateData(FALSE);
	avgsApply.EnableWindow(TRUE);
}

void CRerapDialog::BuildFrequencyText()
{
	long sumN = 0;
	freqs = "";
	freqs.Format(
		"%s Frequency Distribution\r\n"
		"%ld Weather Days, %ld Days w/Wind (%.0f%%)\r\n"
		"Class            Range                  Freq    Relative Cumulative\r\n", 
		varString, nRecs, windDays, 
		((nRecs > 0) ? ((double)windDays) / ((double)nRecs) * 100.0 : 0.0));
	if(stats.N() == 0)
		return;
	for(int c = 0; c < rStats[0].nClasses; c++)
	{
		CString str;
		long n = ClassN(c);
		sumN += n;
		str.Format(
			"%4d\t%5.1f - %5.1f\t%6ld\t%5.2f\t%6.2f\r\n",
			//"%4d %5.1f - %5.1f %6ld %5.2f %6.2f\r\n",
			c + 1, (double)c * rStats[0].step + rStats[0].start, (double)((c + 1) * rStats[0].step + rStats[0].start - .1), n, ((double) n) / ((double) stats.N()) * 100.0, 
			((double)sumN) / ((double) stats.N()) * 100.0);
		freqs += str;
		//add stats to correct fire danger class
		int cumu = (int)(((double)sumN) / ((double) stats.N()) * 100);
		if(cumu <= low)
		{
			AddClassToFireDangerClass(c, 0);
		}
		else if(cumu <= mod)
		{
			AddClassToFireDangerClass(c, 1);
		}
		else if(cumu <= high)
		{
			AddClassToFireDangerClass(c, 2);
		}
		else
		{
			AddClassToFireDangerClass(c, 3);
		}
		if(sumN == stats.N())
			break;
	}
	freqs += "\r\n";
}

long CRerapDialog::ClassN(int c)
{
	long n = 0;
	if(wN)
		n += rStats[7].varStats[c].N(); 
	if(wNE)
		n += rStats[0].varStats[c].N(); 
	if(wE)
		n += rStats[1].varStats[c].N(); 
	if(wSE)
		n += rStats[2].varStats[c].N(); 
	if(wS)
		n += rStats[3].varStats[c].N(); 
	if(wSW)
		n += rStats[4].varStats[c].N(); 
	if(wW)
		n += rStats[5].varStats[c].N(); 
	if(wNW)
		n += rStats[6].varStats[c].N(); 
	return n;
}

void CRerapDialog::OnKillfocusLm1() 
{
	// TODO: Add your control notification handler code here
	int oldLm1 = lm1;
	UpdateData(TRUE);
	if(lm1 == oldLm1)
		return;
	if(lm1 > lm2)
		lm2 = lm1;
	lobs = 0;
	//if(lStats.N() <= 0)
	//	return;
	int c = (lm1 - rStats[0].start) / rStats[0].step;
	int d = (lm2 - rStats[0].start) / rStats[0].step;
	if(c < 0)
		c = 0;
	else if(c >= rStats[0].nClasses)
		c = rStats[0].nClasses - 1;
	if(d < 0)
		d = 0;
	else if(d >= rStats[0].nClasses)
		d = rStats[0].nClasses - 1;
	for(int i = c; i <= d; i++)
		lobs += ClassN(i);
	UpdateData(FALSE);
}

void CRerapDialog::OnKillfocusLm2() 
{
	// TODO: Add your control notification handler code here
	int oldLm2 = lm2;
	UpdateData(TRUE);
	if(lm2 == oldLm2)
		return;
	if(lm1 > lm2)
		lm1 = lm2;
	lobs = 0;
//	if(lStats.N() <= 0)
//		return;
	int c = (lm1 - rStats[0].start) / rStats[0].step;
	int d = (lm2 - rStats[0].start) / rStats[0].step;
	if(c < 0)
		c = 0;
	else if(c >= rStats[0].nClasses)
		c = rStats[0].nClasses - 1;
	if(d < 0)
		d = 0;
	else if(d >= rStats[0].nClasses)
		d = rStats[0].nClasses - 1;
	for(int i = c; i <= d; i++)
		lobs += ClassN(i);
	UpdateData(FALSE);
}

void CRerapDialog::OnKillfocusMm1() 
{
	// TODO: Add your control notification handler code here
	int oldMm1 = mm1;
	UpdateData(TRUE);
	if(mm1 == oldMm1)
		return;
	if(mm1 > mm2)
		mm2 = mm1;
	mobs = 0;
	//if(mStats.N() <= 0)
//		return;
	int c = (mm1 - rStats[0].start) / rStats[0].step;
	int d = (mm2 - rStats[0].start) / rStats[0].step;
	if(c < 0)
		c = 0;
	else if(c >= rStats[0].nClasses)
		c = rStats[0].nClasses - 1;
	if(d < 0)
		d = 0;
	else if(d >= rStats[0].nClasses)
		d = rStats[0].nClasses - 1;
	for(int i = c; i <= d; i++)
		mobs += ClassN(i);
	UpdateData(FALSE);	
}

void CRerapDialog::OnKillfocusMm2() 
{
	// TODO: Add your control notification handler code here
	int oldmm2 = mm2;
	UpdateData(TRUE);
	if(mm2 == oldmm2)
		return;
	if(mm1 > mm2)
		mm1 = mm2;
	mobs = 0;
	//if(mStats.N() <= 0)
	//	return;
	int c = (mm1 - rStats[0].start) / rStats[0].step;
	int d = (mm2 - rStats[0].start) / rStats[0].step;
	if(c < 0)
		c = 0;
	else if(c >= rStats[0].nClasses)
		c = rStats[0].nClasses - 1;
	if(d < 0)
		d = 0;
	else if(d >= rStats[0].nClasses)
		d = rStats[0].nClasses - 1;
	for(int i = c; i <= d; i++)
		mobs += ClassN(i);
	UpdateData(FALSE);	
}

void CRerapDialog::OnKillfocusHm1() 
{
	// TODO: Add your control notification handler code here
	int oldhm1 = hm1;
	UpdateData(TRUE);
	if(hm1 == oldhm1)
		return;
	if(hm1 > hm2)
		hm2 = hm1;
	hobs = 0;
	//if(hStats.N() <= 0)
		///return;
	int c = (hm1 - rStats[0].start) / rStats[0].step;
	int d = (hm2 - rStats[0].start) / rStats[0].step;
	if(c < 0)
		c = 0;
	else if(c >= rStats[0].nClasses)
		c = rStats[0].nClasses - 1;
	if(d < 0)
		d = 0;
	else if(d >= rStats[0].nClasses)
		d = rStats[0].nClasses - 1;
	for(int i = c; i <= d; i++)
		hobs += ClassN(i);
	UpdateData(FALSE);	
}

void CRerapDialog::OnKillfocusHm2() 
{
	// TODO: Add your control notification handler code here
	int oldhm2 = hm2;
	UpdateData(TRUE);
	if(hm2 == oldhm2)
		return;
	if(hm1 > hm2)
		hm1 = hm2;
	hobs = 0;
	//if(hStats.N() <= 0)
//		return;
	int c = (hm1 - rStats[0].start) / rStats[0].step;
	int d = (hm2 - rStats[0].start) / rStats[0].step;
	if(c < 0)
		c = 0;
	else if(c >= rStats[0].nClasses)
		c = rStats[0].nClasses - 1;
	if(d < 0)
		d = 0;
	else if(d >= rStats[0].nClasses)
		d = rStats[0].nClasses - 1;
	for(int i = c; i <= d; i++)
		hobs += ClassN(i);
	UpdateData(FALSE);	
}

void CRerapDialog::OnKillfocusEm1() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	int oldem1 = em1;
	UpdateData(TRUE);
	if(em1 == oldem1)
		return;
	if(em1 > em2)
		em2 = em1;
	eobs = 0;
	//if(eStats.N() <= 0)
	//	return;
	int c = (em1 - rStats[0].start) / rStats[0].step;
	int d = (em2 - rStats[0].start) / rStats[0].step;
	if(c < 0)
		c = 0;
	else if(c >= rStats[0].nClasses)
		c = rStats[0].nClasses - 1;
	if(d < 0)
		d = 0;
	else if(d >= rStats[0].nClasses)
		d = rStats[0].nClasses - 1;
	for(int i = c; i <= d; i++)
		eobs += ClassN(i);
	UpdateData(FALSE);	
}

void CRerapDialog::OnKillfocusEm2() 
{
	// TODO: Add your control notification handler code here
	int oldem2 = em2;
	UpdateData(TRUE);
	if(em2 == oldem2)
		return;
	if(em1 > em2)
		em1 = em2;
	eobs = 0;
//	if(eStats.N() <= 0)
//		return;
	int c = (em1 - rStats[0].start) / rStats[0].step;
	int d = (em2 - rStats[0].start) / rStats[0].step;
	if(c < 0)
		c = 0;
	else if(c >= rStats[0].nClasses)
		c = rStats[0].nClasses - 1;
	if(d < 0)
		d = 0;
	else if(d >= rStats[0].nClasses)
		d = rStats[0].nClasses - 1;
	for(int i = c; i <= d; i++)
		eobs += ClassN(i);
	UpdateData(FALSE);	
}

void CRerapDialog::AddClassLAvgs(int c)
{
	if(wN)
	{
		l1 += rStats[7].fm1[c] * rStats[7].varStats[c].N(); 
		l10 += rStats[7].fm10[c] * rStats[7].varStats[c].N(); 
		l100 += rStats[7].fm100[c] * rStats[7].varStats[c].N(); 
		l1000 += rStats[7].fm1000[c] * rStats[7].varStats[c].N(); 
		l20 += rStats[7].wind[c] * rStats[7].varStats[c].N(); 
		lherb += rStats[7].herb[c] * rStats[7].varStats[c].N(); 
		lwoody += rStats[7].woody[c] * rStats[7].varStats[c].N(); 
		lkbdi += rStats[7].kbdi[c] * rStats[7].varStats[c].N(); 
	}
	if(wNE)
	{
		l1 += rStats[0].fm1[c] * rStats[0].varStats[c].N(); 
		l10 += rStats[0].fm10[c] * rStats[0].varStats[c].N(); 
		l100 += rStats[0].fm100[c] * rStats[0].varStats[c].N(); 
		l1000 += rStats[0].fm1000[c] * rStats[0].varStats[c].N(); 
		l20 += rStats[0].wind[c] * rStats[0].varStats[c].N(); 
		lherb += rStats[0].herb[c] * rStats[0].varStats[c].N(); 
		lwoody += rStats[0].woody[c] * rStats[0].varStats[c].N(); 
		lkbdi += rStats[0].kbdi[c] * rStats[0].varStats[c].N(); 
	}
	if(wE)
	{
		l1 += rStats[1].fm1[c] * rStats[1].varStats[c].N(); 
		l10 += rStats[1].fm10[c] * rStats[1].varStats[c].N(); 
		l100 += rStats[1].fm100[c] * rStats[1].varStats[c].N(); 
		l1000 += rStats[1].fm1000[c] * rStats[1].varStats[c].N(); 
		l20 += rStats[1].wind[c] * rStats[1].varStats[c].N(); 
		lherb += rStats[1].herb[c] * rStats[1].varStats[c].N(); 
		lwoody += rStats[1].woody[c] * rStats[1].varStats[c].N(); 
		lkbdi += rStats[1].kbdi[c] * rStats[1].varStats[c].N(); 
	}
	if(wSE)
	{
		l1 += rStats[2].fm1[c] * rStats[2].varStats[c].N(); 
		l10 += rStats[2].fm10[c] * rStats[2].varStats[c].N(); 
		l100 += rStats[2].fm100[c] * rStats[2].varStats[c].N(); 
		l1000 += rStats[2].fm1000[c] * rStats[2].varStats[c].N(); 
		l20 += rStats[2].wind[c] * rStats[2].varStats[c].N(); 
		lherb += rStats[2].herb[c] * rStats[2].varStats[c].N(); 
		lwoody += rStats[2].woody[c] * rStats[2].varStats[c].N(); 
		lkbdi += rStats[2].kbdi[c] * rStats[2].varStats[c].N(); 
	}
	if(wS)
	{
		l1 += rStats[3].fm1[c] * rStats[3].varStats[c].N(); 
		l10 += rStats[3].fm10[c] * rStats[3].varStats[c].N(); 
		l100 += rStats[3].fm100[c] * rStats[3].varStats[c].N(); 
		l1000 += rStats[3].fm1000[c] * rStats[3].varStats[c].N(); 
		l20 += rStats[3].wind[c] * rStats[3].varStats[c].N(); 
		lherb += rStats[3].herb[c] * rStats[3].varStats[c].N(); 
		lwoody += rStats[3].woody[c] * rStats[3].varStats[c].N(); 
		lkbdi += rStats[3].kbdi[c] * rStats[3].varStats[c].N(); 
	}
	if(wSW)
	{
		l1 += rStats[4].fm1[c] * rStats[4].varStats[c].N(); 
		l10 += rStats[4].fm10[c] * rStats[4].varStats[c].N(); 
		l100 += rStats[4].fm100[c] * rStats[4].varStats[c].N(); 
		l1000 += rStats[4].fm1000[c] * rStats[4].varStats[c].N(); 
		l20 += rStats[4].wind[c] * rStats[4].varStats[c].N(); 
		lherb += rStats[4].herb[c] * rStats[4].varStats[c].N(); 
		lwoody += rStats[4].woody[c] * rStats[4].varStats[c].N(); 
		lkbdi += rStats[4].kbdi[c] * rStats[4].varStats[c].N(); 
	}
	if(wW)
	{
		l1 += rStats[5].fm1[c] * rStats[5].varStats[c].N(); 
		l10 += rStats[5].fm10[c] * rStats[5].varStats[c].N(); 
		l100 += rStats[5].fm100[c] * rStats[5].varStats[c].N(); 
		l1000 += rStats[5].fm1000[c] * rStats[5].varStats[c].N(); 
		l20 += rStats[5].wind[c] * rStats[5].varStats[c].N(); 
		lherb += rStats[5].herb[c] * rStats[5].varStats[c].N(); 
		lwoody += rStats[5].woody[c] * rStats[5].varStats[c].N(); 
		lkbdi += rStats[5].kbdi[c] * rStats[5].varStats[c].N(); 
	}
	if(wNW)
	{
		l1 += rStats[6].fm1[c] * rStats[6].varStats[c].N(); 
		l10 += rStats[6].fm10[c] * rStats[6].varStats[c].N(); 
		l100 += rStats[6].fm100[c] * rStats[6].varStats[c].N(); 
		l1000 += rStats[6].fm1000[c] * rStats[6].varStats[c].N(); 
		l20 += rStats[6].wind[c] * rStats[6].varStats[c].N(); 
		lherb += rStats[6].herb[c] * rStats[6].varStats[c].N(); 
		lwoody += rStats[6].woody[c] * rStats[6].varStats[c].N(); 
		lkbdi += rStats[6].kbdi[c] * rStats[6].varStats[c].N(); 
	}	
}

void CRerapDialog::AddClassMAvgs(int c)
{
	if(wNE)
	{
		Mod1 += rStats[0].fm1[c] * rStats[0].varStats[c].N(); 
		Mod10 += rStats[0].fm10[c] * rStats[0].varStats[c].N(); 
		Mod100 += rStats[0].fm100[c] * rStats[0].varStats[c].N(); 
		Mod1000 += rStats[0].fm1000[c] * rStats[0].varStats[c].N(); 
		Mod20 += rStats[0].wind[c] * rStats[0].varStats[c].N(); 
		Modherb += rStats[0].herb[c] * rStats[0].varStats[c].N(); 
		Modwoody += rStats[0].woody[c] * rStats[0].varStats[c].N(); 
		Modkbdi += rStats[0].kbdi[c] * rStats[0].varStats[c].N(); 
	}
	if(wE)
	{
		Mod1 += rStats[1].fm1[c] * rStats[1].varStats[c].N(); 
		Mod10 += rStats[1].fm10[c] * rStats[1].varStats[c].N(); 
		Mod100 += rStats[1].fm100[c] * rStats[1].varStats[c].N(); 
		Mod1000 += rStats[1].fm1000[c] * rStats[1].varStats[c].N(); 
		Mod20 += rStats[1].wind[c] * rStats[1].varStats[c].N(); 
		Modherb += rStats[1].herb[c] * rStats[1].varStats[c].N(); 
		Modwoody += rStats[1].woody[c] * rStats[1].varStats[c].N(); 
		Modkbdi += rStats[1].kbdi[c] * rStats[1].varStats[c].N(); 
	}
	if(wSE)
	{
		Mod1 += rStats[2].fm1[c] * rStats[2].varStats[c].N(); 
		Mod10 += rStats[2].fm10[c] * rStats[2].varStats[c].N(); 
		Mod100 += rStats[2].fm100[c] * rStats[2].varStats[c].N(); 
		Mod1000 += rStats[2].fm1000[c] * rStats[2].varStats[c].N(); 
		Mod20 += rStats[2].wind[c] * rStats[2].varStats[c].N(); 
		Modherb += rStats[2].herb[c] * rStats[2].varStats[c].N(); 
		Modwoody += rStats[2].woody[c] * rStats[2].varStats[c].N(); 
		Modkbdi += rStats[2].kbdi[c] * rStats[2].varStats[c].N(); 
	}
	if(wS)
	{
		Mod1 += rStats[3].fm1[c] * rStats[3].varStats[c].N(); 
		Mod10 += rStats[3].fm10[c] * rStats[3].varStats[c].N(); 
		Mod100 += rStats[3].fm100[c] * rStats[3].varStats[c].N(); 
		Mod1000 += rStats[3].fm1000[c] * rStats[3].varStats[c].N(); 
		Mod20 += rStats[3].wind[c] * rStats[3].varStats[c].N(); 
		Modherb += rStats[3].herb[c] * rStats[3].varStats[c].N(); 
		Modwoody += rStats[3].woody[c] * rStats[3].varStats[c].N(); 
		Modkbdi += rStats[3].kbdi[c] * rStats[3].varStats[c].N(); 
	}
	if(wSW)
	{
		Mod1 += rStats[4].fm1[c] * rStats[4].varStats[c].N(); 
		Mod10 += rStats[4].fm10[c] * rStats[4].varStats[c].N(); 
		Mod100 += rStats[4].fm100[c] * rStats[4].varStats[c].N(); 
		Mod1000 += rStats[4].fm1000[c] * rStats[4].varStats[c].N(); 
		Mod20 += rStats[4].wind[c] * rStats[4].varStats[c].N(); 
		Modherb += rStats[4].herb[c] * rStats[4].varStats[c].N(); 
		Modwoody += rStats[4].woody[c] * rStats[4].varStats[c].N(); 
		Modkbdi += rStats[4].kbdi[c] * rStats[4].varStats[c].N(); 
	}
	if(wW)
	{
		Mod1 += rStats[5].fm1[c] * rStats[5].varStats[c].N(); 
		Mod10 += rStats[5].fm10[c] * rStats[5].varStats[c].N(); 
		Mod100 += rStats[5].fm100[c] * rStats[5].varStats[c].N(); 
		Mod1000 += rStats[5].fm1000[c] * rStats[5].varStats[c].N(); 
		Mod20 += rStats[5].wind[c] * rStats[5].varStats[c].N(); 
		Modherb += rStats[5].herb[c] * rStats[5].varStats[c].N(); 
		Modwoody += rStats[5].woody[c] * rStats[5].varStats[c].N(); 
		Modkbdi += rStats[5].kbdi[c] * rStats[5].varStats[c].N(); 
	}
	if(wNW)
	{
		Mod1 += rStats[6].fm1[c] * rStats[6].varStats[c].N(); 
		Mod10 += rStats[6].fm10[c] * rStats[6].varStats[c].N(); 
		Mod100 += rStats[6].fm100[c] * rStats[6].varStats[c].N(); 
		Mod1000 += rStats[6].fm1000[c] * rStats[6].varStats[c].N(); 
		Mod20 += rStats[6].wind[c] * rStats[6].varStats[c].N(); 
		Modherb += rStats[6].herb[c] * rStats[6].varStats[c].N(); 
		Modwoody += rStats[6].woody[c] * rStats[6].varStats[c].N(); 
		Modkbdi += rStats[6].kbdi[c] * rStats[6].varStats[c].N(); 
	}
	if(wN)
	{
		Mod1 += rStats[7].fm1[c] * rStats[7].varStats[c].N(); 
		Mod10 += rStats[7].fm10[c] * rStats[7].varStats[c].N(); 
		Mod100 += rStats[7].fm100[c] * rStats[7].varStats[c].N(); 
		Mod1000 += rStats[7].fm1000[c] * rStats[7].varStats[c].N(); 
		Mod20 += rStats[7].wind[c] * rStats[7].varStats[c].N(); 
		Modherb += rStats[7].herb[c] * rStats[7].varStats[c].N(); 
		Modwoody += rStats[7].woody[c] * rStats[7].varStats[c].N(); 
		Modkbdi += rStats[7].kbdi[c] * rStats[7].varStats[c].N(); 
	}	
}
void CRerapDialog::AddClassHAvgs(int c)
{
	if(wNE)
	{
		h1 += rStats[0].fm1[c] * rStats[0].varStats[c].N(); 
		h10 += rStats[0].fm10[c] * rStats[0].varStats[c].N(); 
		h100 += rStats[0].fm100[c] * rStats[0].varStats[c].N(); 
		h1000 += rStats[0].fm1000[c] * rStats[0].varStats[c].N(); 
		h20 += rStats[0].wind[c] * rStats[0].varStats[c].N(); 
		hherb += rStats[0].herb[c] * rStats[0].varStats[c].N(); 
		hwoody += rStats[0].woody[c] * rStats[0].varStats[c].N(); 
		hkbdi += rStats[0].kbdi[c] * rStats[0].varStats[c].N(); 
	}
	if(wE)
	{
		h1 += rStats[1].fm1[c] * rStats[1].varStats[c].N(); 
		h10 += rStats[1].fm10[c] * rStats[1].varStats[c].N(); 
		h100 += rStats[1].fm100[c] * rStats[1].varStats[c].N(); 
		h1000 += rStats[1].fm1000[c] * rStats[1].varStats[c].N(); 
		h20 += rStats[1].wind[c] * rStats[1].varStats[c].N(); 
		hherb += rStats[1].herb[c] * rStats[1].varStats[c].N(); 
		hwoody += rStats[1].woody[c] * rStats[1].varStats[c].N(); 
		hkbdi += rStats[1].kbdi[c] * rStats[1].varStats[c].N(); 
	}
	if(wSE)
	{
		h1 += rStats[2].fm1[c] * rStats[2].varStats[c].N(); 
		h10 += rStats[2].fm10[c] * rStats[2].varStats[c].N(); 
		h100 += rStats[2].fm100[c] * rStats[2].varStats[c].N(); 
		h1000 += rStats[2].fm1000[c] * rStats[2].varStats[c].N(); 
		h20 += rStats[2].wind[c] * rStats[2].varStats[c].N(); 
		hherb += rStats[2].herb[c] * rStats[2].varStats[c].N(); 
		hwoody += rStats[2].woody[c] * rStats[2].varStats[c].N(); 
		hkbdi += rStats[2].kbdi[c] * rStats[2].varStats[c].N(); 
	}
	if(wS)
	{
		h1 += rStats[3].fm1[c] * rStats[3].varStats[c].N(); 
		h10 += rStats[3].fm10[c] * rStats[3].varStats[c].N(); 
		h100 += rStats[3].fm100[c] * rStats[3].varStats[c].N(); 
		h1000 += rStats[3].fm1000[c] * rStats[3].varStats[c].N(); 
		h20 += rStats[3].wind[c] * rStats[3].varStats[c].N(); 
		hherb += rStats[3].herb[c] * rStats[3].varStats[c].N(); 
		hwoody += rStats[3].woody[c] * rStats[3].varStats[c].N(); 
		hkbdi += rStats[3].kbdi[c] * rStats[3].varStats[c].N(); 
	}
	if(wSW)
	{
		h1 += rStats[4].fm1[c] * rStats[4].varStats[c].N(); 
		h10 += rStats[4].fm10[c] * rStats[4].varStats[c].N(); 
		h100 += rStats[4].fm100[c] * rStats[4].varStats[c].N(); 
		h1000 += rStats[4].fm1000[c] * rStats[4].varStats[c].N(); 
		h20 += rStats[4].wind[c] * rStats[4].varStats[c].N(); 
		hherb += rStats[4].herb[c] * rStats[4].varStats[c].N(); 
		hwoody += rStats[4].woody[c] * rStats[4].varStats[c].N(); 
		hkbdi += rStats[4].kbdi[c] * rStats[4].varStats[c].N(); 
	}
	if(wW)
	{
		h1 += rStats[5].fm1[c] * rStats[5].varStats[c].N(); 
		h10 += rStats[5].fm10[c] * rStats[5].varStats[c].N(); 
		h100 += rStats[5].fm100[c] * rStats[5].varStats[c].N(); 
		h1000 += rStats[5].fm1000[c] * rStats[5].varStats[c].N(); 
		h20 += rStats[5].wind[c] * rStats[5].varStats[c].N(); 
		hherb += rStats[5].herb[c] * rStats[5].varStats[c].N(); 
		hwoody += rStats[5].woody[c] * rStats[5].varStats[c].N(); 
		hkbdi += rStats[5].kbdi[c] * rStats[5].varStats[c].N(); 
	}
	if(wNW)
	{
		h1 += rStats[6].fm1[c] * rStats[6].varStats[c].N(); 
		h10 += rStats[6].fm10[c] * rStats[6].varStats[c].N(); 
		h100 += rStats[6].fm100[c] * rStats[6].varStats[c].N(); 
		h1000 += rStats[6].fm1000[c] * rStats[6].varStats[c].N(); 
		h20 += rStats[6].wind[c] * rStats[6].varStats[c].N(); 
		hherb += rStats[6].herb[c] * rStats[6].varStats[c].N(); 
		hwoody += rStats[6].woody[c] * rStats[6].varStats[c].N(); 
		hkbdi += rStats[6].kbdi[c] * rStats[6].varStats[c].N(); 
	}
	if(wN)
	{
		h1 += rStats[7].fm1[c] * rStats[7].varStats[c].N(); 
		h10 += rStats[7].fm10[c] * rStats[7].varStats[c].N(); 
		h100 += rStats[7].fm100[c] * rStats[7].varStats[c].N(); 
		h1000 += rStats[7].fm1000[c] * rStats[7].varStats[c].N(); 
		h20 += rStats[7].wind[c] * rStats[7].varStats[c].N(); 
		hherb += rStats[7].herb[c] * rStats[7].varStats[c].N(); 
		hwoody += rStats[7].woody[c] * rStats[7].varStats[c].N(); 
		hkbdi += rStats[7].kbdi[c] * rStats[7].varStats[c].N(); 
	}	
}
void CRerapDialog::AddClassEAvgs(int c)
{
	if(wNE)
	{
		e1 += rStats[0].fm1[c] * rStats[0].varStats[c].N(); 
		e10 += rStats[0].fm10[c] * rStats[0].varStats[c].N(); 
		e100 += rStats[0].fm100[c] * rStats[0].varStats[c].N(); 
		e1000 += rStats[0].fm1000[c] * rStats[0].varStats[c].N(); 
		e20 += rStats[0].wind[c] * rStats[0].varStats[c].N(); 
		eherb += rStats[0].herb[c] * rStats[0].varStats[c].N(); 
		ewoody += rStats[0].woody[c] * rStats[0].varStats[c].N(); 
		ekbdi += rStats[0].kbdi[c] * rStats[0].varStats[c].N(); 
	}
	if(wE)
	{
		e1 += rStats[1].fm1[c] * rStats[1].varStats[c].N(); 
		e10 += rStats[1].fm10[c] * rStats[1].varStats[c].N(); 
		e100 += rStats[1].fm100[c] * rStats[1].varStats[c].N(); 
		e1000 += rStats[1].fm1000[c] * rStats[1].varStats[c].N(); 
		e20 += rStats[1].wind[c] * rStats[1].varStats[c].N(); 
		eherb += rStats[1].herb[c] * rStats[1].varStats[c].N(); 
		ewoody += rStats[1].woody[c] * rStats[1].varStats[c].N(); 
		ekbdi += rStats[1].kbdi[c] * rStats[1].varStats[c].N(); 
	}
	if(wSE)
	{
		e1 += rStats[2].fm1[c] * rStats[2].varStats[c].N(); 
		e10 += rStats[2].fm10[c] * rStats[2].varStats[c].N(); 
		e100 += rStats[2].fm100[c] * rStats[2].varStats[c].N(); 
		e1000 += rStats[2].fm1000[c] * rStats[2].varStats[c].N(); 
		e20 += rStats[2].wind[c] * rStats[2].varStats[c].N(); 
		eherb += rStats[2].herb[c] * rStats[2].varStats[c].N(); 
		ewoody += rStats[2].woody[c] * rStats[2].varStats[c].N(); 
		ekbdi += rStats[2].kbdi[c] * rStats[2].varStats[c].N(); 
	}
	if(wS)
	{
		e1 += rStats[3].fm1[c] * rStats[3].varStats[c].N(); 
		e10 += rStats[3].fm10[c] * rStats[3].varStats[c].N(); 
		e100 += rStats[3].fm100[c] * rStats[3].varStats[c].N(); 
		e1000 += rStats[3].fm1000[c] * rStats[3].varStats[c].N(); 
		e20 += rStats[3].wind[c] * rStats[3].varStats[c].N(); 
		eherb += rStats[3].herb[c] * rStats[3].varStats[c].N(); 
		ewoody += rStats[3].woody[c] * rStats[3].varStats[c].N(); 
		ekbdi += rStats[3].kbdi[c] * rStats[3].varStats[c].N(); 
	}
	if(wSW)
	{
		e1 += rStats[4].fm1[c] * rStats[4].varStats[c].N(); 
		e10 += rStats[4].fm10[c] * rStats[4].varStats[c].N(); 
		e100 += rStats[4].fm100[c] * rStats[4].varStats[c].N(); 
		e1000 += rStats[4].fm1000[c] * rStats[4].varStats[c].N(); 
		e20 += rStats[4].wind[c] * rStats[4].varStats[c].N(); 
		eherb += rStats[4].herb[c] * rStats[4].varStats[c].N(); 
		ewoody += rStats[4].woody[c] * rStats[4].varStats[c].N(); 
		ekbdi += rStats[4].kbdi[c] * rStats[4].varStats[c].N(); 
	}
	if(wW)
	{
		e1 += rStats[5].fm1[c] * rStats[5].varStats[c].N(); 
		e10 += rStats[5].fm10[c] * rStats[5].varStats[c].N(); 
		e100 += rStats[5].fm100[c] * rStats[5].varStats[c].N(); 
		e1000 += rStats[5].fm1000[c] * rStats[5].varStats[c].N(); 
		e20 += rStats[5].wind[c] * rStats[5].varStats[c].N(); 
		eherb += rStats[5].herb[c] * rStats[5].varStats[c].N(); 
		ewoody += rStats[5].woody[c] * rStats[5].varStats[c].N(); 
		ekbdi += rStats[5].kbdi[c] * rStats[5].varStats[c].N(); 
	}
	if(wNW)
	{
		e1 += rStats[6].fm1[c] * rStats[6].varStats[c].N(); 
		e10 += rStats[6].fm10[c] * rStats[6].varStats[c].N(); 
		e100 += rStats[6].fm100[c] * rStats[6].varStats[c].N(); 
		e1000 += rStats[6].fm1000[c] * rStats[6].varStats[c].N(); 
		e20 += rStats[6].wind[c] * rStats[6].varStats[c].N(); 
		eherb += rStats[6].herb[c] * rStats[6].varStats[c].N(); 
		ewoody += rStats[6].woody[c] * rStats[6].varStats[c].N(); 
		ekbdi += rStats[6].kbdi[c] * rStats[6].varStats[c].N(); 
	}
	if(wN)
	{
		e1 += rStats[7].fm1[c] * rStats[7].varStats[c].N(); 
		e10 += rStats[7].fm10[c] * rStats[7].varStats[c].N(); 
		e100 += rStats[7].fm100[c] * rStats[7].varStats[c].N(); 
		e1000 += rStats[7].fm1000[c] * rStats[7].varStats[c].N(); 
		e20 += rStats[7].wind[c] * rStats[7].varStats[c].N(); 
		eherb += rStats[7].herb[c] * rStats[7].varStats[c].N(); 
		ewoody += rStats[7].woody[c] * rStats[7].varStats[c].N(); 
		ekbdi += rStats[7].kbdi[c] * rStats[7].varStats[c].N(); 
	}	
}

void CRerapDialog::OnOK() 
{
	CWaitCursor wait;
	
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	fprintf(stream, "FireFamily Plus Percentile Weather Report for RERAP\n\n");
	CSIGStationSet staSet(fpSet->m_pDatabase);
	staSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
	staSet.Open();
	fprintf(stream, " Station: %6.6s: %-20.20s   Variable: %s\n",//                %d - %d\n", 
			fpSet->m_SIG_Station, staSet.m_Name, varString);//fpSet->m_StartYear, fpSet->m_EndYear);
	fprintf(stream, " Model: %d%s%d%c%c%d\n         Data Years: %d - %d\n", 
		staSet.m_Use88 ? 8 : 7, staSet.m_NFDRSFM, staSet.m_SlopeCls,
		staSet.m_HerbAnnual ? 'A' : 'P', staSet.m_Deciduous ? 'D' : 'E', staSet.m_ClimateCls,
		fpSet->m_StartYear, fpSet->m_EndYear);
	fprintf(stream, "         Date Range: %s %d - %s %d\n",
		fpSet->m_StartMonth, fpSet->m_StartDay, fpSet->m_EndMonth, fpSet->m_EndDay);
	staSet.Close();

	// added for batch
	// (code interspersed)
	//CReportOptionsSet reportSet(fpSet->m_pDatabase);
	//reportSet.Open();

	//CString dStr;

	
	
	//COleDateTime dateTime = COleDateTime::GetCurrentTime();

	//dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	//fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	//fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	//fprintf(stream,"\n");
	//CString headerStr;

	// add report headers (as needed)
	//reportSet.listAll(&headerStr,fpSet);
	//fprintf(stream,"%s",headerStr);
	// end added for batch


	int nd = 0;
	fprintf(stream, "Wind Directions: ");
	if(wN)
	{
		fprintf(stream, "N");
		nd++;
	}
	if(wNE)
	{
		if(nd)
			fprintf(stream, ", NE");
		else
			fprintf(stream, "NE");
		nd++;
	}
	if(wE)
	{
		if(nd)
			fprintf(stream, ", E");
		else
			fprintf(stream, "E");
		nd++;
	}
	if(wSE)
	{
		if(nd)
			fprintf(stream, ", SE");
		else
			fprintf(stream, "SE");
		nd++;
	}
	if(wS)
	{
		if(nd)
			fprintf(stream, ", S");
		else
			fprintf(stream, "S");
		nd++;
	}
	if(wSW)
	{
		if(nd)
			fprintf(stream, ", SW");
		else
			fprintf(stream, "SW");
		nd++;
	}
	if(wW)
	{
		if(nd)
			fprintf(stream, ", W");
		else
			fprintf(stream, "W");
		nd++;
	}
	if(wNW)
	{
		if(nd)
			fprintf(stream, ", NW");
		else
			fprintf(stream, "NW");
	}
	fprintf(stream, "\n\n");
	fprintf(stream,
		"Percentiles, Probabilities, and Mid-Points\n");
	fprintf(stream,
		"    Variable/Component Range       Low       Mod       High      Ext\n");
	fprintf(stream,
		"            Percentile Range     0 - %2d    %s %2d    %s %2d    %s - 100\n",
		low, mod1, mod, high1, high, ext1);
	fprintf(stream,
		"       Climatol. Probability        %2d        %2d        %2d        %2d\n",
		lpcnt, mpcnt, hpcnt, epcnt);
	fprintf(stream,
		"          Mid-Point %6.6s     %3d - %3d  %3d - %3d  %3d - %3d  %3d - %3d\n",
		varString, lm1, lm2, mm1, mm2, hm1, hm2, em1, em2);
	fprintf(stream,
		"            Num Observations    %6d    %6d    %6d    %6d\n",
		lobs, mobs, hobs, eobs);
	fprintf(stream,
		"     Calculated Spread Comp.       %3d       %3d       %3d       %3d\n",
		lsc, Modsc, hsc, esc);
	fprintf(stream,
		"              Calculated ERC       %3d       %3d       %3d       %3d\n\n",
		lerc, Moderc, herc, eerc);
	fprintf(stream,
		"Fuel Moistures\n");
	fprintf(stream,
		"        1 Hour Fuel Moisture    %7.2f   %7.2f   %7.2f   %7.2f\n",
		l1, Mod1, h1, e1);
	fprintf(stream,
		"       10 Hour Fuel Moisture    %7.2f   %7.2f   %7.2f   %7.2f\n",
		l10, Mod10, h10, e10);
	fprintf(stream,
		"      100 Hour Fuel Moisture    %7.2f   %7.2f   %7.2f   %7.2f\n",
		l100, Mod100, h100, e100);
	fprintf(stream,
		"    Herbaceous Fuel Moisture    %7.2f   %7.2f   %7.2f   %7.2f\n",
		lherb, Modherb, hherb, eherb);
	fprintf(stream,
		"         Woody Fuel Moisture    %7.2f   %7.2f   %7.2f   %7.2f\n",
		lwoody, Modwoody, hwoody, ewoody);
	fprintf(stream,
		"              20' Wind Speed    %7.2f   %7.2f   %7.2f   %7.2f\n",
		l20, Mod20, h20, e20);
	fprintf(stream,
		"     1000 Hour Fuel Moisture    %7.2f   %7.2f   %7.2f   %7.2f\n\n",
		l1000, Mod1000, h1000, e1000);
	fprintf(stream, " %ld Weather Records Used, %ld Days With Wind (%.2f%%)\n", nRecs,
		windDays, ((nRecs > 0) ? ((double)windDays) / ((double)nRecs) * 100.0 : 0.0));
	fclose(stream);

	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = 
			theApp.GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("TextFile"))
		{
			CRichDoc *pDoc = (CRichDoc *)curTemplate->OpenDocumentFile(fName);
			pDoc->isTemp = true;
			CString tTitle;
			GetWindowText(tTitle);
			pDoc->SetTitle(tTitle);
			break;;
		}
	}
	free(fName);

	CDialog::OnOK();
}



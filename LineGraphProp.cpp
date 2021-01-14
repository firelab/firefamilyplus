// LineGraphProp.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "GraphSet.h"
#include "wxset.h"
#include "OverlayDialog.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "SplitterFrame.h"
#include "GraphAutoSaveProp.h"
#include "LineGraphProp.h"
#include "BargraphProp.h"
#include "GeneralGraphProp.h"
#include "GraphFiresPage.h"
#include "FDProjPage.h"
//#include "MergedVarsPropPage.h"
#include "GraphPropSheet.h"
#include "graphview.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLineGraphProp property page

IMPLEMENT_DYNCREATE(CLineGraphProp, CPropertyPage)

CLineGraphProp::CLineGraphProp() : CPropertyPage(CLineGraphProp::IDD)
{
	//{{AFX_DATA_INIT(CLineGraphProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	isQuick = false;
}

CLineGraphProp::~CLineGraphProp()
{
}

void CLineGraphProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLineGraphProp)
	DDX_Control(pDX, IDC_MINS, mins);
	DDX_Control(pDX, IDC_MAXS, maxs);
	DDX_Control(pDX, IDC_CP2, cp2);
	DDX_Control(pDX, IDC_CP1, cp1);
	DDX_Control(pDX, IDC_AVGCOLOR, avgColor);
	DDX_Control(pDX, IDC_CP1COLOR, cp1Color);
	DDX_Control(pDX, IDC_CP2COLOR, cp2Color);
	DDX_Control(pDX, IDC_MAXSCOLOR, maxsColor);
	DDX_Control(pDX, IDC_MINSCOLOR, minsColor);
	//DDX_Control(pDX, IDC_AVGWIDTH, avgWidth);
	//DDX_Control(pDX, IDC_CP1WIDTH, cp1Width);
	//DDX_Control(pDX, IDC_CP2WIDTH, cp2Width);
	//DDX_Control(pDX, IDC_MAXSWIDTH, maxsWidth);
	//DDX_Control(pDX, IDC_MINSWIDTH, minsWidth);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SPINAVG, m_spinAvg);
	DDX_Control(pDX, IDC_SPINMINS, m_spinMins);
	DDX_Control(pDX, IDC_SPINMAXS, m_spinMaxs);
	DDX_Control(pDX, IDC_SPINCP1, m_spinCP1);
	DDX_Control(pDX, IDC_SPINCP2, m_spinCP2);
}


BEGIN_MESSAGE_MAP(CLineGraphProp, CPropertyPage)
	//{{AFX_MSG_MAP(CLineGraphProp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineGraphProp message handlers

BOOL CLineGraphProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_spinAvg.SetRange(1, 100);
	m_spinMaxs.SetRange(1, 100);
	m_spinMins.SetRange(1, 100);
	m_spinCP1.SetRange(1, 100);
	m_spinCP2.SetRange(1, 100);
	if(isQuick)
	{
		ReadData(NULL);
	}
	else
	{
		CWnd *child = ((CGraphPropSheet *)GetParent())->mainFrame->MDIGetActive(), *topMost = NULL; 
		CRuntimeClass* prt = child->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CSplitterFrame" ) == 0)
			topMost = child;
		CGraphView *pView = NULL;
		if(topMost)
			pView = (CGraphView *) ((CSplitterFrame *)topMost)->m_wndSplitter.GetPane(0, 0);
		ReadData(pView);
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLineGraphProp::SetGraphSet(CGraphSet * set)
{
	graphSet = set;
}



void CLineGraphProp::SaveData()
{
	if(graphSet->IsOpen())
	{
		graphSet->m_Mins = mins.GetCheck();
		graphSet->m_Maxs = maxs.GetCheck();
		graphSet->m_CP1 = cp1.GetCheck();
		graphSet->m_CP2 = cp2.GetCheck();
		graphSet->m_Line_Color = (long)avgColor.GetColor();
		graphSet->m_MinsColor = (long)minsColor.GetColor();
		graphSet->m_MaxsColor = (long)maxsColor.GetColor();
		graphSet->m_CP1Color = (long)cp1Color.GetColor();
		graphSet->m_CP2Color = (long)cp2Color.GetColor();
		graphSet->m_Line_Width = (short)m_spinAvg.GetPos();//avgWidth.GetValueInteger();
		graphSet->m_MinsWidth = (short)m_spinMins.GetPos();//minsWidth.GetValueInteger();
		graphSet->m_MaxsWidth = (short)m_spinMaxs.GetPos();//maxsWidth.GetValueInteger();
		graphSet->m_CP1Width = (short)m_spinCP1.GetPos();//cp1Width.GetValueInteger();
		graphSet->m_CP2Width = (short)m_spinCP2.GetPos();//cp2Width.GetValueInteger();
	}
}


void CLineGraphProp::ReadData(CGraphView *pView)
{
	if(pView)//reading from a frame window
	{
		//CGraphView *pView = (CGraphView *) frame->m_wndSplitter.GetPane(0, 0);
		mins.SetCheck(pView->m_graphOpts.m_mins);
		maxs.SetCheck(pView->m_graphOpts.m_maxs);
		cp1.SetCheck(pView->m_graphOpts.m_cp1s);
		cp2.SetCheck(pView->m_graphOpts.m_cp2s);
		avgColor.SetColor(pView->m_graphOpts.m_Line_Color);
		minsColor.SetColor(pView->m_graphOpts.m_minsColor);
		maxsColor.SetColor(pView->m_graphOpts.m_maxsColor);
		cp1Color.SetColor(pView->m_graphOpts.m_cp1Color);
		cp2Color.SetColor(pView->m_graphOpts.m_cp2Color);
		//avgWidth.SetValueInteger(frame->m_Line_Width);
		m_spinAvg.SetPos(pView->m_graphOpts.m_Line_Width);//
		m_spinMins.SetPos(pView->m_graphOpts.m_minsWidth);
		m_spinMaxs.SetPos(pView->m_graphOpts.m_maxsWidth);
		m_spinCP1.SetPos(pView->m_graphOpts.m_cp1Width);
		m_spinCP2.SetPos(pView->m_graphOpts.m_cp2Width);
	}
	else //read from graphSet
	{
		if(graphSet->IsOpen())
		{
			mins.SetCheck(graphSet->m_Mins);
			maxs.SetCheck(graphSet->m_Maxs);
			cp1.SetCheck(graphSet->m_CP1);
			cp2.SetCheck(graphSet->m_CP2);
			avgColor.SetColor((COLORREF)graphSet->m_Line_Color);
			minsColor.SetColor((COLORREF)graphSet->m_MinsColor);
			maxsColor.SetColor((COLORREF)graphSet->m_MaxsColor);
			cp1Color.SetColor((COLORREF)graphSet->m_CP1Color);
			cp2Color.SetColor((COLORREF)graphSet->m_CP2Color);
			m_spinAvg.SetPos(graphSet->m_Line_Width);
			m_spinMins.SetPos(graphSet->m_MinsWidth);
			m_spinMaxs.SetPos(graphSet->m_MaxsWidth);
			m_spinCP1.SetPos(graphSet->m_CP1Width);
			m_spinCP2.SetPos(graphSet->m_CP2Width);
		}
	}
}

void CLineGraphProp::WriteData(CGraphView *pView)
{
	if(pView)
	{
		//CGraphView *pView = (CGraphView *) frame->m_wndSplitter.GetPane(0, 0);
		pView->m_graphOpts.m_mins = mins.GetCheck();
		pView->m_graphOpts.m_maxs = maxs.GetCheck();
		pView->m_graphOpts.m_cp1s = cp1.GetCheck();
		pView->m_graphOpts.m_cp2s = cp2.GetCheck();
		pView->m_graphOpts.m_Line_Color = (long)avgColor.GetColor();
		pView->m_graphOpts.m_minsColor = (long)minsColor.GetColor();
		pView->m_graphOpts.m_maxsColor = (long)maxsColor.GetColor();
		pView->m_graphOpts.m_cp1Color = (long)cp1Color.GetColor();
		pView->m_graphOpts.m_cp2Color = (long)cp2Color.GetColor();
		pView->m_graphOpts.m_Line_Width = (short)m_spinAvg.GetPos();//avgWidth.GetValueInteger();
		pView->m_graphOpts.m_minsWidth = (short)m_spinMins.GetPos();//minsWidth.GetValueInteger();
		pView->m_graphOpts.m_maxsWidth = (short)m_spinMaxs.GetPos();//maxsWidth.GetValueInteger();
		pView->m_graphOpts.m_cp1Width = (short)m_spinCP1.GetPos();//cp1Width.GetValueInteger();
		pView->m_graphOpts.m_cp2Width = (short)m_spinCP2.GetPos();//cp2Width.GetValueInteger();
	}
	else
	{
		if(graphSet->IsOpen())
		{
			graphSet->m_Mins = mins.GetCheck();
			graphSet->m_Maxs = maxs.GetCheck();
			graphSet->m_CP1 = cp1.GetCheck();
			graphSet->m_CP2 = cp2.GetCheck();
			graphSet->m_Line_Color = (long)avgColor.GetColor();
			graphSet->m_MinsColor = (long)minsColor.GetColor();
			graphSet->m_MaxsColor = (long)maxsColor.GetColor();
			graphSet->m_CP1Color = (long)cp1Color.GetColor();
			graphSet->m_CP2Color = (long)cp2Color.GetColor();
			graphSet->m_Line_Width = (short)m_spinAvg.GetPos();//avgWidth.GetValueInteger();
			graphSet->m_MinsWidth = (short)m_spinMins.GetPos();//minsWidth.GetValueInteger();
			graphSet->m_MaxsWidth = (short)m_spinMaxs.GetPos();//maxsWidth.GetValueInteger();
			graphSet->m_CP1Width = (short)m_spinCP1.GetPos();//cp1Width.GetValueInteger();
			graphSet->m_CP2Width = (short)m_spinCP2.GetPos();//cp2Width.GetValueInteger();
		}
	}
}


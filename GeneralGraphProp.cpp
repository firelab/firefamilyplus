// GeneralGraphProp.cpp : implementation file
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
#include "LineGraphProp.h"
#include "BargraphProp.h"
#include "GeneralGraphProp.h"
#include "GraphFiresPage.h"
#include "GraphAutoSaveProp.h"
#include "FDProjPage.h"
//#include "MergedVarsPropPage.h"
#include "GraphPropSheet.h"
#include "MainFrm.h"
#include "graphview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CGeneralGraphProp property page

IMPLEMENT_DYNCREATE(CGeneralGraphProp, CPropertyPage)

CGeneralGraphProp::CGeneralGraphProp() : CPropertyPage(CGeneralGraphProp::IDD)
{
	//{{AFX_DATA_INIT(CGeneralGraphProp)
	overallCP1 = FALSE;
	overallCP2 = FALSE;
	isQuick = false;
	//}}AFX_DATA_INIT
}

CGeneralGraphProp::~CGeneralGraphProp()
{
}

void CGeneralGraphProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralGraphProp)
	DDX_Control(pDX, IDC_VLINESTYLE, vLinestyle);
	DDX_Control(pDX, IDC_HLINESTYLE, hLinestyle);
	DDX_Control(pDX, IDC_VLINES, vLines);
	DDX_Control(pDX, IDC_HLINES, hLines);
	DDX_Control(pDX, IDC_STATBARS, statBars);
	DDX_Control(pDX, IDC_LINEGRAPH, lineGraph);
	DDX_Check(pDX, IDC_CP1, overallCP1);
	DDX_Check(pDX, IDC_CP2, overallCP2);
	DDX_Control(pDX, IDC_CP1COLOR, overallCP1Color);
	DDX_Control(pDX, IDC_CP2COLOR, overallCP2Color);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeneralGraphProp, CPropertyPage)
	//{{AFX_MSG_MAP(CGeneralGraphProp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralGraphProp message handlers


BOOL CGeneralGraphProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
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

void CGeneralGraphProp::SetGraphSet(CGraphSet * set)
{
	graphSet = set;
}



void CGeneralGraphProp::SaveData()
{
	UpdateData();
	//just set graphset members, Edit and Update called by property sheet
	if(graphSet->IsOpen())
	{
		graphSet->m_InitBargraph = statBars.GetCheck();
		graphSet->m_Vertical_Lines = vLines.GetCheck();
		graphSet->m_Horizontal_Lines = hLines.GetCheck();
		vLinestyle.GetWindowText(graphSet->m_V_Line_Style);
		hLinestyle.GetWindowText(graphSet->m_H_Line_Style);	
	}
	theApp.m_OverallCP1 = overallCP1;
	theApp.m_OverallCP2 = overallCP2;
	theApp.m_OverallCP1Color = (long)overallCP1Color.GetColor();
	theApp.m_OverallCP2Color = (long)overallCP2Color.GetColor();
}

void CGeneralGraphProp::ReadData(CGraphView *pView)
{
	if(pView)
	{
		//CGraphView *pView = (CGraphView *) frame->m_wndSplitter.GetPane(0, 0);
		statBars.SetCheck(pView->m_graphOpts.isBargraph);
		lineGraph.SetCheck(!pView->m_graphOpts.isBargraph);
		vLines.SetCheck(pView->m_graphOpts.m_Vertical_Lines);
		hLines.SetCheck(pView->m_graphOpts.m_Horizontal_Lines);
		CString str = pView->m_graphOpts.m_V_Line_Style;
		str.TrimRight();
		vLinestyle.SelectString(-1, str);
		str = pView->m_graphOpts.m_H_Line_Style;
		str.TrimRight();
		hLinestyle.SelectString(-1, str);
		overallCP1 = pView->m_graphOpts.m_OverallCP1;
		overallCP2 = pView->m_graphOpts.m_OverallCP2;
		overallCP1Color.SetColor(pView->m_graphOpts.m_OverallCP1Color);
		overallCP2Color.SetColor(pView->m_graphOpts.m_OverallCP2Color);
	}
	else
	{
		if(graphSet->IsOpen())
		{
			statBars.SetCheck(graphSet->m_InitBargraph);
			lineGraph.SetCheck(!graphSet->m_InitBargraph);
			vLines.SetCheck(graphSet->m_Vertical_Lines);
			hLines.SetCheck(graphSet->m_Horizontal_Lines);
			CString str = graphSet->m_V_Line_Style;
			str.TrimRight();
			vLinestyle.SelectString(-1, str);
			str = graphSet->m_H_Line_Style;
			str.TrimRight();
			hLinestyle.SelectString(-1, str);
		}
		overallCP1 = theApp.m_OverallCP1;
		overallCP2 = theApp.m_OverallCP2;
		overallCP1Color.SetColor(theApp.m_OverallCP1Color);
		overallCP2Color.SetColor(theApp.m_OverallCP2Color);
	}

}

void CGeneralGraphProp::WriteData(CGraphView *pView)
{
	UpdateData();
	if(pView)
	{
		//CGraphView *pView = (CGraphView *) frame->m_wndSplitter.GetPane(0, 0);
		pView->m_graphOpts.isBargraph = statBars.GetCheck() == 0 ? false : true;
		pView->m_graphOpts.m_Vertical_Lines = vLines.GetCheck();
		pView->m_graphOpts.m_Horizontal_Lines = hLines.GetCheck();
		vLinestyle.GetWindowText(pView->m_graphOpts.m_V_Line_Style);
		hLinestyle.GetWindowText(pView->m_graphOpts.m_H_Line_Style);
		pView->m_graphOpts.m_OverallCP1 = overallCP1;
		pView->m_graphOpts.m_OverallCP2 = overallCP2;
		pView->m_graphOpts.m_OverallCP1Color = (long)overallCP1Color.GetColor();
		pView->m_graphOpts.m_OverallCP2Color = (long)overallCP2Color.GetColor();
	}
	else
	{
		if(graphSet->IsOpen())
		{
			graphSet->m_InitBargraph = statBars.GetCheck();
			graphSet->m_Vertical_Lines = vLines.GetCheck();
			graphSet->m_Horizontal_Lines = hLines.GetCheck();
			vLinestyle.GetWindowText(graphSet->m_V_Line_Style);
			hLinestyle.GetWindowText(graphSet->m_H_Line_Style);
		}
		theApp.m_OverallCP1 = overallCP1;
		theApp.m_OverallCP2 = overallCP2;
		theApp.m_OverallCP1Color = (long)overallCP1Color.GetColor();
		theApp.m_OverallCP2Color = (long)overallCP2Color.GetColor();
	}
}



// BargraphProp.cpp : implementation file
//

#include "stdafx.h"
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

/////////////////////////////////////////////////////////////////////////////
// CBargraphProp property page

IMPLEMENT_DYNCREATE(CBargraphProp, CPropertyPage)

CBargraphProp::CBargraphProp() : CPropertyPage(CBargraphProp::IDD)
{
	//{{AFX_DATA_INIT(CBargraphProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	isQuick = false;
}

CBargraphProp::~CBargraphProp()
{
}

void CBargraphProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBargraphProp)
	DDX_Control(pDX, IDC_RANGETICS, rangeTics);
	DDX_Control(pDX, IDC_CPTICS, cpTics);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBargraphProp, CPropertyPage)
	//{{AFX_MSG_MAP(CBargraphProp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBargraphProp message handlers

BOOL CBargraphProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if(isQuick)
	{
		ReadData(NULL);
	}
	else
	{
		CWnd *child = ((CGraphPropSheet *)GetParent())->mainFrame->MDIGetActive(), *topMost = NULL; 
		int count = 0;
		CRuntimeClass* prt = child->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CSplitterFrame" ) == 0)
			topMost = child;
			CGraphView *pView = NULL;
			if(topMost)
				pView = (CGraphView *) ((CSplitterFrame *)topMost)->m_wndSplitter.GetPane(0, 0);
			ReadData(pView);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBargraphProp::SetGraphSet(CGraphSet * set)
{
	graphSet = set;
}

void CBargraphProp::SaveData()
{
	if(graphSet->IsOpen())
	{
		graphSet->m_CP_Tics = cpTics.GetCheck();
		graphSet->m_Range_Tics = rangeTics.GetCheck();
	}

}

void CBargraphProp::ReadData(CGraphView *pView)
{
	if(pView)
	{
		//CGraphView *pView = (CGraphView *) frame->m_wndSplitter.GetPane(0, 0);
		cpTics.SetCheck(pView->m_graphOpts.m_CP_Tics);
		rangeTics.SetCheck(pView->m_graphOpts.m_Range_Tics);
	}
	else
	{
		if(graphSet->IsOpen())
		{
			cpTics.SetCheck(graphSet->m_CP_Tics);
			rangeTics.SetCheck(graphSet->m_Range_Tics);
		}
	}
}

void CBargraphProp::WriteData(CGraphView *pView)
{
	if(pView)
	{
		//CGraphView *pView = (CGraphView *) frame->m_wndSplitter.GetPane(0, 0);
		pView->m_graphOpts.m_CP_Tics = cpTics.GetCheck();
		pView->m_graphOpts.m_Range_Tics = rangeTics.GetCheck();
	}
	else
	{
		if(graphSet->IsOpen())
		{
			graphSet->m_CP_Tics = cpTics.GetCheck();
			graphSet->m_Range_Tics = rangeTics.GetCheck();
		}
	}
}

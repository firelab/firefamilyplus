// FDProjPage.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "SplitterFrame.h"
#include "FDProjPage.h"
#include "ForecastDialog.h"
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
// CFDProjPage property page

IMPLEMENT_DYNCREATE(CFDProjPage, CPropertyPage)

CFDProjPage::CFDProjPage() : CPropertyPage(CFDProjPage::IDD)
{
	//{{AFX_DATA_INIT(CFDProjPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	isQuick = false;
}

CFDProjPage::~CFDProjPage()
{
}

void CFDProjPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFDProjPage)
	DDX_Control(pDX, IDC_LINESTYLE, linestyle);
	DDX_Control(pDX, IDC_LINECOLOR, lineColor);
	//DDX_Control(pDX, IDC_LINEWIDTH, lineWidth);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SPINWIDTH, m_spinWidth);
}


BEGIN_MESSAGE_MAP(CFDProjPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFDProjPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFDProjPage message handlers

BOOL CFDProjPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_spinWidth.SetRange(1, 10);
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
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFDProjPage::WriteData(CGraphView *pView)
{
	if(pView)
	{
		//CGraphView *pView = (CGraphView *) frame->m_wndSplitter.GetPane(0, 0);
		pView->m_graphOpts.m_fxColor = (long)lineColor.GetColor();
		pView->m_graphOpts.m_fxWidth = (short)m_spinWidth.GetPos();//lineWidth.GetValueInteger();
		CString ls;
		linestyle.GetWindowText(ls);
		pView->m_graphOpts.m_fxStyle = StringToDashLineStyle(ls);
	}
	else
	{
		ForecastOptions fo;
		fo.Read(m_pDB);
		fo.lineColor = (long)lineColor.GetColor();
		fo.lineWidth = m_spinWidth.GetPos();//lineWidth.GetValueInteger();
		CString ls;
		linestyle.GetWindowText(ls);
		fo.lineStyle = StringToDashLineStyle(ls);
		fo.Write(m_pDB);
	}
}

void CFDProjPage::ReadData(CGraphView *pView)
{
	if(pView)
	{
		//CGraphView *pView = (CGraphView *) frame->m_wndSplitter.GetPane(0, 0);
		lineColor.SetColor(pView->m_graphOpts.m_fxColor);
		//lineWidth.SetValueInteger
		m_spinWidth.SetPos(pView->m_graphOpts.m_fxWidth);
		CString ls;
		for(int i = 0; i < linestyle.GetCount(); i++)
		{
			linestyle.GetLBText(i, ls);
			if(pView->m_graphOpts.m_fxStyle == StringToLineStyle(ls))
			{
				linestyle.SetCurSel(i);
				break;
			}
		}
		if(linestyle.GetCurSel() <= 0)
			linestyle.SetCurSel(0);
	}
	else
	{
		ForecastOptions fo;
		fo.Read(m_pDB);
		lineColor.SetColor(fo.lineColor);
		m_spinWidth.SetPos(fo.lineWidth);
		CString ls;
		for(int i = 0; i < linestyle.GetCount(); i++)
		{
			linestyle.GetLBText(i, ls);
			if(fo.lineStyle == StringToLineStyle(ls))
			{
				linestyle.SetCurSel(i);
				break;
			}
		}
		if(linestyle.GetCurSel() <= 0)
			linestyle.SetCurSel(0);
	}
}

void CFDProjPage::SaveData()
{
	ForecastOptions fo;
	fo.Read(m_pDB);
	fo.lineColor = (long)lineColor.GetColor();
	fo.lineWidth = m_spinWidth.GetPos();//lineWidth.GetValueInteger();
	CString ls;
	linestyle.GetWindowText(ls);
	fo.lineStyle = StringToDashLineStyle(ls);
	fo.Write(m_pDB);
}


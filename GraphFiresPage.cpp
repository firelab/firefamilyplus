// GraphFiresPage.cpp : implementation file
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
#include "GraphAutoSaveProp.h"
#include "GraphFiresPage.h"
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
// CGraphFiresPage property page

IMPLEMENT_DYNCREATE(CGraphFiresPage, CPropertyPage)

CGraphFiresPage::CGraphFiresPage() : CPropertyPage(CGraphFiresPage::IDD)
{
	//{{AFX_DATA_INIT(CGraphFiresPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	isQuick = false;
}

CGraphFiresPage::~CGraphFiresPage()
{
}

void CGraphFiresPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphFiresPage)
	DDX_Control(pDX, IDC_CHECK_MFD, mfd);
	DDX_Control(pDX, IDC_CHECK_LFD, lfd);
	DDX_Control(pDX, IDC_CHECK_FD, fd);
	//DDX_Control(pDX, IDC_PVNUMERIC_LFD, lfdAcres);
	//DDX_Control(pDX, IDC_PVNUMERIC_MFD, mfdFires);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SPINFIRES, m_spinFires);
	DDX_Control(pDX, IDC_SPINACRES, m_spinAcres);
}


BEGIN_MESSAGE_MAP(CGraphFiresPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGraphFiresPage)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnRadioAll)
	ON_BN_CLICKED(IDC_RADIO_HUMAN, OnRadioHuman)
	ON_BN_CLICKED(IDC_RADIO_LIGHTNING, OnRadioLightning)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphFiresPage message handlers

BOOL CGraphFiresPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_spinAcres.SetRange32(1, 1000000);
	m_spinFires.SetRange32(1, 1000000);
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

void CGraphFiresPage::SetGraphSet(CGraphSet * set)
{
	graphSet = set;
}

void CGraphFiresPage::OnRadioAll() 
{
	// TODO: Add your control notification handler code here
	cause = 0;
}

void CGraphFiresPage::OnRadioLightning() 
{
	// TODO: Add your control notification handler code here
	cause = 1;	
}

void CGraphFiresPage::OnRadioHuman() 
{
	// TODO: Add your control notification handler code here
	cause = 2;
}

void CGraphFiresPage::SaveData()
{
	//just set graphset members, Edit and Update called by property sheet
	if(graphSet->IsOpen())
	{
		graphSet->m_Cause = cause;
		graphSet->m_FD = fd.GetCheck();
		graphSet->m_LFD = lfd.GetCheck();
		graphSet->m_MFD = mfd.GetCheck();
		graphSet->m_LFDAcres = (short)m_spinAcres.GetPos();//lfdAcres.GetValueInteger();
		graphSet->m_MFDFires = (short)m_spinFires.GetPos();//mfdFires.GetValueInteger();
	}
}

void CGraphFiresPage::ReadData(CGraphView *pView)
{
	if(pView)
	{
		//CGraphView *pView = (CGraphView *) frame->m_wndSplitter.GetPane(0, 0);
		//lfdAcres = frame->m_LFDAcres;
		//mfdFires = frame->m_MFDFires;
		cause = pView->m_graphOpts.m_Cause;
		fd.SetCheck(pView->m_graphOpts.m_FD);
		lfd.SetCheck(pView->m_graphOpts.m_LFD);
		mfd.SetCheck(pView->m_graphOpts.m_MFD);
		m_spinFires.SetPos(pView->m_graphOpts.m_MFDFires);
		//mfdFires.SetValueInteger(frame->m_MFDFires);
		m_spinAcres.SetPos(pView->m_graphOpts.m_LFDAcres);
		//lfdAcres.SetValueInteger(frame->m_LFDAcres);
		//lfdAcres.UpdateWindow();
		//mfdFires.
		UpdateWindow();
	}
	else
	{
		//lfdAcres = frame->m_LFDAcres;
		//mfdFires = frame->m_MFDFires;
		if(graphSet->IsOpen())
		{
			cause = graphSet->m_Cause;
			fd.SetCheck(graphSet->m_FD);
			lfd.SetCheck(graphSet->m_LFD);
			mfd.SetCheck(graphSet->m_MFD);
			m_spinFires.SetPos(graphSet->m_MFDFires);
			//mfdFires.SetValueInteger(graphSet->m_MFDFires);
			m_spinAcres.SetPos(graphSet->m_LFDAcres);
			//lfdAcres.SetValueInteger(graphSet->m_LFDAcres);
		}
		//lfdAcres.UpdateWindow();
		//mfdFires.
		UpdateWindow();
	}
	switch(cause)
	{
	case 0:
		((CButton *)GetDlgItem(IDC_RADIO_ALL))->SetCheck(true);
		((CButton *)GetDlgItem(IDC_RADIO_LIGHTNING))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_RADIO_HUMAN))->SetCheck(false);
		break;
	case 1:
		((CButton *)GetDlgItem(IDC_RADIO_LIGHTNING))->SetCheck(true);
		((CButton *)GetDlgItem(IDC_RADIO_ALL))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_RADIO_HUMAN))->SetCheck(false);
		break;
	case 2:
		((CButton *)GetDlgItem(IDC_RADIO_HUMAN))->SetCheck(true);
		((CButton *)GetDlgItem(IDC_RADIO_LIGHTNING))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_RADIO_ALL))->SetCheck(false);
		break;
	}
	//UpdateData(TRUE);
}

void CGraphFiresPage::WriteData(CGraphView *pView)
{
	if(pView)
	{
		//CGraphView *pView = (CGraphView *) frame->m_wndSplitter.GetPane(0, 0);
		pView->m_graphOpts.m_Cause = cause;
		pView->m_graphOpts.m_FD = fd.GetCheck();
		pView->m_graphOpts.m_LFD = lfd.GetCheck();
		pView->m_graphOpts.m_MFD = mfd.GetCheck();
		pView->m_graphOpts.m_LFDAcres = (short)m_spinAcres.GetPos();//lfdAcres.GetValueInteger();
		pView->m_graphOpts.m_MFDFires = (short)m_spinFires.GetPos();//mfdFires.GetValueInteger();
	}
	else
	{
		if(graphSet->IsOpen())
		{
			graphSet->m_Cause = cause;
			graphSet->m_FD = fd.GetCheck();
			graphSet->m_LFD = lfd.GetCheck();
			graphSet->m_MFD = mfd.GetCheck();
			graphSet->m_LFDAcres = (short)m_spinAcres.GetPos();//lfdAcres.GetValueInteger();
			graphSet->m_MFDFires = (short)m_spinFires.GetPos();//mfdFires.GetValueInteger();
		}
	}
}

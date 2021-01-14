// ReportHeaderProp.cpp : implementation file
//
//
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "ReportOptionsSet.h"
#include "wxset.h"
#include "OverlayDialog.h"
#include "FireDay.h"
#include "PeriodDoc.h"

#include "ReportHeaderProp.h"

#include "FDProjPage.h"

#include "ReportPropSheet.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportHeaderProp property page

IMPLEMENT_DYNCREATE(CReportHeaderProp, CPropertyPage)

CReportHeaderProp::CReportHeaderProp() : CPropertyPage(CReportHeaderProp::IDD)
{
	//{{AFX_DATA_INIT(CReportHeaderProp)
		// NOTE: the ClassWizard will add member initialization here
	
	//}}AFX_DATA_INIT
}

CReportHeaderProp::~CReportHeaderProp()
{
}

void CReportHeaderProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReportHeaderProp)
	DDX_Control(pDX, IDC_WORKINGSET, WorkingSet);
	DDX_Control(pDX, IDC_ANNUALFILTER, AnnualFilter);
	DDX_Control(pDX, IDC_SIGSTATION, SIGStation);
//	DDX_Control(pDX, IDC_ADDTL, Addtl);
	
	//}}AFX_DATA_MAP
	
}


BEGIN_MESSAGE_MAP(CReportHeaderProp, CPropertyPage)
	//{{AFX_MSG_MAP(CReportHeaderProp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportHeaderProp message handlers

BOOL CReportHeaderProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
	
	ReadData();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CReportHeaderProp::SetReportSet(CReportOptionsSet * set)
{
	reportSet = set;
	
}



void CReportHeaderProp::SaveData()
{
	if(reportSet->IsOpen())
	{
		reportSet->MoveFirst();
		reportSet->Edit();

		reportSet->m_WorkingSet = WorkingSet.GetCheck();
		reportSet->m_AnnualFilter = AnnualFilter.GetCheck();
		reportSet->m_SIGStation = SIGStation.GetCheck();
		//reportSet->m_Addtl = Addtl.GetCheck();
		
		reportSet->Update();
		
	}
}


void CReportHeaderProp::ReadData()
{
	
		if(reportSet->IsOpen())
		{
			//reportSet->MoveFirst();
           
			WorkingSet.SetCheck(reportSet->m_WorkingSet);


			AnnualFilter.SetCheck(reportSet->m_AnnualFilter);


			SIGStation.SetCheck(reportSet->m_SIGStation);


//			Addtl.SetCheck(reportSet->m_Addtl);
			
		}
	
}


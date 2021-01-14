// ReportDelimiterProp.cpp : implementation file
//
#include "stdafx.h"

#include "fireplus.h"
#include "ReportOptionsSet.h"
#include "wxset.h"
#include "OverlayDialog.h"
#include "FireDay.h"
#include "PeriodDoc.h"

#include "ReportDelimiterProp.h"

#include "FDProjPage.h"

#include "ReportPropSheet.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportDelimiterProp property page

IMPLEMENT_DYNCREATE(CReportDelimiterProp, CPropertyPage)

CReportDelimiterProp::CReportDelimiterProp() : CPropertyPage(CReportDelimiterProp::IDD)
{
	//{{AFX_DATA_INIT(CReportDelimiterProp)
		// NOTE: the ClassWizard will add member initialization here
	
	//}}AFX_DATA_INIT
}

CReportDelimiterProp::~CReportDelimiterProp()
{
}

void CReportDelimiterProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReportDelimiterProp)
	

	DDX_Control(pDX, IDC_RADIO_DELIMITER_SPACE, UseSpace);
	DDX_Control(pDX, IDC_RADIO_DELIMITER_TABS, UseTabs);
	DDX_Control(pDX, IDC_RADIO_DELIMITER_COMMA, UseComma);
	DDX_Control(pDX, IDC_RADIO_DELIMITER_PIPE, UsePipe);
	//}}AFX_DATA_MAP
	
}


BEGIN_MESSAGE_MAP(CReportDelimiterProp, CPropertyPage)
	//{{AFX_MSG_MAP(CReportDelimiterProp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportDelimiterProp message handlers

BOOL CReportDelimiterProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
	
	ReadData();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CReportDelimiterProp::SetReportSet(CReportOptionsSet * set)
{
	reportSet = set;
	
}



void CReportDelimiterProp::SaveData()
{
	if(reportSet->IsOpen())
	{
		reportSet->MoveFirst();
		reportSet->Edit();
		if (UseSpace.GetCheck())
			reportSet->m_Delimiter = 0;
		else if (UseTabs.GetCheck())
			reportSet->m_Delimiter = 1;
		else if (UseComma.GetCheck())
			reportSet->m_Delimiter = 2;
		else if (UsePipe.GetCheck())
			reportSet->m_Delimiter = 3;
		//else if (UseSemicolon.GetCheck())
			//reportSet->m_Delimiter = 4;
		
		
		
		reportSet->Update();
		
	}
}


void CReportDelimiterProp::ReadData()
{
	
		if(reportSet->IsOpen())
		{
			//reportSet->MoveFirst();
			switch (reportSet->m_Delimiter){
				case 0:  // default
					UseSpace.SetCheck(TRUE);
					UseTabs.SetCheck(FALSE);
					UseComma.SetCheck(FALSE);
					UsePipe.SetCheck(FALSE);
					//UseSemicolon.SetCheck(FALSE);
					break;
				case 1:  
					UseSpace.SetCheck(FALSE);
					UseTabs.SetCheck(TRUE);
					UseComma.SetCheck(FALSE);
					UsePipe.SetCheck(FALSE);
					//UseSemicolon.SetCheck(FALSE);
					break;
				case 2: 
					UseSpace.SetCheck(FALSE);
					UseTabs.SetCheck(FALSE);
					UseComma.SetCheck(TRUE);
					UsePipe.SetCheck(FALSE);
					//UseSemicolon.SetCheck(FALSE);
					break;
				case 3: 
				case 4: 
					UseSpace.SetCheck(FALSE);
					UseTabs.SetCheck(FALSE);
					UseComma.SetCheck(FALSE);
					UsePipe.SetCheck(TRUE);
					//UseSemicolon.SetCheck(FALSE);
					break;
				/*case 4: 
					UseSpace.SetCheck(FALSE);
					UseTabs.SetCheck(FALSE);
					UseComma.SetCheck(FALSE);
					UseColon.SetCheck(FALSE);
					UseSemicolon.SetCheck(TRUE);
					break;*/

			}
			
			
		}
	
}


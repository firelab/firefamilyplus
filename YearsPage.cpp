// YearsPage.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "YearsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CYearsPage property page

IMPLEMENT_DYNCREATE(CYearsPage, CPropertyPage)

CYearsPage::CYearsPage(int *_start, int *_end) : CPropertyPage(CYearsPage::IDD)
{
	//{{AFX_DATA_INIT(CYearsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	iStart = _start;
	iEnd = _end;
}

CYearsPage::~CYearsPage()
{
}

void CYearsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CYearsPage)
//	DDX_Control(pDX, IDC_PVNUMERIC1, start);
//	DDX_Control(pDX, IDC_PVNUMERIC2, end);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SPINSTART, m_spinStart);
	DDX_Control(pDX, IDC_SPINEND, m_spinEnd);
}


BEGIN_MESSAGE_MAP(CYearsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CYearsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CYearsPage message handlers

BEGIN_EVENTSINK_MAP(CYearsPage, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CYearsPage)
//	ON_EVENT(CYearsPage, IDC_PVNUMERIC2, 4 /* LostFocusEvent */, OnLostFocusEventEndYear, VTS_NONE)
//	ON_EVENT(CYearsPage, IDC_PVNUMERIC1, 4 /* LostFocusEvent */, OnLostFocusEventStartYear, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/*void CYearsPage::OnLostFocusEventEndYear() 
{
	// TODO: Add your control notification handler code here
//	start.SetValueMax(end.GetValueReal());
	
}

void CYearsPage::OnLostFocusEventStartYear() 
{
	// TODO: Add your control notification handler code here
	//end.SetValueMin(start.GetValueReal());
	
}*/

BOOL CYearsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	COleDateTime today;
	today = COleDateTime::GetCurrentTime();
	//start.SetValueMax(today.GetYear());
	m_spinStart.SetRange(1900, today.GetYear());
	m_spinStart.SetPos(*iStart);
	//start.SetValueInteger(*iStart);
	//end.SetValueMax(2100);
	//end.SetValueInteger(*iEnd);
	m_spinEnd.SetRange(1900, 2100);
	m_spinEnd.SetPos(*iEnd);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CYearsPage::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	SaveData();
	CPropertyPage::OnOK();
}

void CYearsPage::SaveData()
{
	*iStart = (int)m_spinStart.GetPos();
	*iEnd = (int)m_spinEnd.GetPos();
}

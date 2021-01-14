// CustomInitCreateDialog.cpp : implementation file
//
//
#include "stdafx.h"
#include "fireplus.h"
#include "CustomInitCreateDialog.h"
#include "SIGStationSet.h"
#include "CustomInitSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CCustomInitCreateDialog dialog


CCustomInitCreateDialog::CCustomInitCreateDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet, CCustomInitSet *_records /* = NULL*/ )
	: CDialog(CCustomInitCreateDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomInitCreateDialog)
	
	//}}AFX_DATA_INIT
	records = _records;


	fpSet = _fpSet;
	
}





void CCustomInitCreateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CCustomInitCreateDialog)

	DDX_Control(pDX, IDC_GREENUPDATE, greenupDate);
	DDX_Control(pDX, IDC_SPINSTART, m_spinKBDI);

	DDX_Control(pDX, IDC_EDITFM1000, m_editFM1000);
	DDX_Control(pDX, IDC_SPINFM1000, m_numSpinFM1000);

	DDX_Control(pDX, IDC_EDITFFMC, m_editFFMC);
	DDX_Control(pDX, IDC_SPINFFMC, m_spinFFMC);

	DDX_Control(pDX, IDC_EDITDMC, m_editDMC);
	DDX_Control(pDX, IDC_SPINDMC, m_spinDMC);

	DDX_Control(pDX, IDC_EDITDC, m_editDC);
	DDX_Control(pDX, IDC_SPINDC, m_spinDC);

	DDX_Control(pDX, IDC_SPINYEAR, m_spinYear1);
	DDX_Control(pDX, IDC_SPINYEAR2, m_spinYear2);

	//}}AFX_DATA_MAP



	DDX_Control(pDX, IDC_STARTUPDAY, m_startUpDate);
}


BEGIN_MESSAGE_MAP(CCustomInitCreateDialog, CDialog)
	//{{AFX_MSG_MAP(CCustomInitCreateDialog)
	
	//ON_WM_SIZE()
	//}}AFX_MSG_MAP
	

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomInitCreateDialog message handlers




BOOL CCustomInitCreateDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	EnableToolTips(TRUE);

    CSIGStationSet staSet(fpSet->m_pDatabase);
	staSet.m_strFilter.Format("[StationID] = '%s'",fpSet->m_SIG_Station);
	staSet.Open();
    
	
	int defaultStartKBDI = 100;
	float defaultStart1000 = 15;

	// taken from canada.cpp:
	float defaultFFMC = 85.0;	
	float defaultDMC = 6.0;
	float defaultDC = 15.0;
	 
    COleDateTime today;
	today = COleDateTime::GetCurrentTime();    
	
	COleDateTime defaultGreenup = today;

	if (! staSet.IsEOF()){
	   staSet.MoveFirst();
       defaultStartKBDI = staSet.m_StartKBDI;
	   defaultStart1000 = staSet.m_Start1000;
	   defaultGreenup = staSet.m_GreenJulian;

	}
	staSet.Close();

    greenupDate.SetFormat("M/d");
    greenupDate.SetTime(defaultGreenup);
	m_startUpDate.SetFormat("M/d");
	m_startUpDate.SetTime(defaultGreenup);

	m_spinKBDI.SetRange(0, 800);
    m_spinKBDI.SetPos(defaultStartKBDI);

	
	int curYear = today.GetYear();
	// look for an 'empty' year to use.
    int defaultYear1 = curYear;
	int defaultYear2 = curYear;

	
    records->m_strFilter.Format("[Year]=%d",defaultYear1);
    records->Requery();
	
	while (! records->IsEOF() && defaultYear1 > 1960){
        defaultYear1 --;
        records->m_strFilter.Format("[Year]=%d",defaultYear1);
		records->Requery();

	}

	
    records->m_strFilter.Format("[Year]=%d",defaultYear2);
    records->Requery();
	
	while (! records->IsEOF() && defaultYear2 >= defaultYear1){
        defaultYear2 --;
        records->m_strFilter.Format("[Year]=%d",defaultYear2);
		records->Requery();

	}

	m_spinYear1.SetRange(1960, curYear);
	m_spinYear1.SetPos(defaultYear1);

	m_spinYear2.SetRange(1960, curYear);
	m_spinYear2.SetPos(defaultYear2);

	

    m_numSpinFM1000.SetDecimalPlaces (2);
	m_numSpinFM1000.SetTrimTrailingZeros (FALSE);
	m_numSpinFM1000.SetRangeAndDelta (0.0, 100.0, 1.0);
	
	m_numSpinFM1000.SetBuddy(&m_editFM1000);
	m_numSpinFM1000.SetPos(defaultStart1000);

	

	m_spinFFMC.SetRangeAndDelta (0.0, 101.0, 1.0);
	m_spinFFMC.SetDecimalPlaces(2);
	m_spinFFMC.SetTrimTrailingZeros (FALSE);
	m_spinFFMC.SetBuddy(&m_editFFMC);
	m_spinFFMC.SetPos(defaultFFMC);

	m_spinDMC.SetRangeAndDelta (0.0, 2000.0, 1.0);
	m_spinDMC.SetDecimalPlaces(2);
	m_spinDMC.SetTrimTrailingZeros (FALSE);
	m_spinDMC.SetBuddy(&m_editDMC);
	m_spinDMC.SetPos(defaultDMC);

	m_spinDC.SetRangeAndDelta (0.0, 2000.0, 1.0);
	m_spinDC.SetDecimalPlaces(2);
	m_spinDC.SetTrimTrailingZeros (FALSE);
	m_spinDC.SetBuddy(&m_editDC);
	m_spinDC.SetPos(defaultDC);




	


	createButton = (CButton*)GetDlgItem(IDOK);
	cancelButton = (CButton*)GetDlgItem(IDCANCEL);
    
	
	CWaitCursor wait;
	if (records->IsOpen() && ! records->IsEOF())
	   records->MoveFirst();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CCustomInitCreateDialog::OnOK() 
{
    COleDateTime today;
	today = COleDateTime::GetCurrentTime();    
	// is year valid?
    int year1 = m_spinYear1.GetPos();
	int year2 = m_spinYear2.GetPos();

	if (year1 < 1960 || year1 > today.GetYear()){
        CString errStr;
		errStr.Format("Invalid starting year entered. (Year must be between 1960 and %d.)", today.GetYear());
		AfxMessageBox(errStr);
		return;
	}

	if (year2 < 1960 || year2 > today.GetYear()){
        CString errStr;
		errStr.Format("Invalid ending year entered. (Year must be between 1960 and %d.)", today.GetYear());
		AfxMessageBox(errStr);
		return;
	}

	if (year2 < year1){
        CString errStr;
		errStr.Format("Invalid years. (End year cannot be earlier than start year.)");
		AfxMessageBox(errStr);
		return;

	}

	// does year already exist?
	CCustomInitSet initSet2(fpSet->m_pDatabase);
	for (int year = year1 ; year <= year2; year ++){	   
	   initSet2.m_strFilter.Format("[SIG_Station]='%s' AND [Year]=%d",fpSet->m_SIG_Station, year);
	   if (! initSet2.IsOpen())
		   initSet2.Open();
	   else
		   initSet2.Requery();

	   if (! initSet2.IsEOF()){
            initSet2.Close();
		    CString errStr;
            errStr.Format("A custom init for year %d already exists. Please enter a different year range.",year);
		    AfxMessageBox(errStr);
		    return;
	   }
	
	}
	initSet2.Close();

	// create new
	for (int year = year1 ; year <= year2; year ++){	   
	   records->AddNew();
	   records->m_SIG_Station = fpSet->m_SIG_Station;
	   greenupDate.GetTime(records->m_GreenJulian);
	   m_startUpDate.GetTime(records->m_StartUpDay);
	   records->m_StartKBDI = m_spinKBDI.GetPos();
       records->m_Year = year;

	   records->m_Start1000 = (float)m_numSpinFM1000.GetPos();

	   records->m_FFMC = (float) m_spinFFMC.GetPos();
	   records->m_DMC = (float) m_spinDMC.GetPos();
	   records->m_DC = (float) m_spinDC.GetPos();

	   records->Update();
	}

	



	CDialog::OnOK();
}



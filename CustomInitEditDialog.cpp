// CustomInitEditDialog.cpp : implementation file
//
//
#include "stdafx.h"
#include "fireplus.h"
#include "CustomInitEditDialog.h"
#include "ExtCheckBox.h"
#include "afxdtctl.h"
#include "afxcmn.h"
#include "numspinctrl.h"
#include "afxwin.h"	
#include "CustomInitSet.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CCustomInitEditDialog dialog


CCustomInitEditDialog::CCustomInitEditDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet, CCustomInitSet *_records /* = NULL*/, int _editYear )
	: CDialog(CCustomInitEditDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomInitEditDialog)
	
	//}}AFX_DATA_INIT
	records = _records;
	Year = _editYear;
	fpSet = _fpSet;
	
}





void CCustomInitEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CCustomInitEditDialog)

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

	DDX_Control(pDX, IDC_SPINYEAR, m_spinYear);
	//}}AFX_DATA_MAP



	DDX_Control(pDX, IDC_STARTUPDAY, m_startUpDay);
}


BEGIN_MESSAGE_MAP(CCustomInitEditDialog, CDialog)
	//{{AFX_MSG_MAP(CCustomInitEditDialog)
	
	//ON_WM_SIZE()
	//}}AFX_MSG_MAP
	

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomInitEditDialog message handlers





BOOL CCustomInitEditDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	EnableToolTips(TRUE);

	EditButton = (CButton*)GetDlgItem(IDOK);
    cancelButton = (CButton*)GetDlgItem(IDCANCEL);
    
	
	CWaitCursor wait;
	records->MoveFirst();
	if (records->IsEOF()){
		AfxMessageBox("Error: No record(s) to edit.");
		return TRUE;
	}
    COleDateTime today;
	today = COleDateTime::GetCurrentTime();    

	greenupDate.SetFormat("M/d");
	greenupDate.SetTime(records->m_GreenJulian);

	m_startUpDay.SetFormat("M/d");
	m_startUpDay.SetTime(records->m_StartUpDay);

	m_spinKBDI.SetRange(0,800);
	m_spinKBDI.SetPos(records->m_StartKBDI);

	m_spinFFMC.SetRangeAndDelta (0.0, 101.0, 1.0);
	m_spinFFMC.SetDecimalPlaces(2);
	m_spinFFMC.SetTrimTrailingZeros (FALSE);
	m_spinFFMC.SetBuddy(&m_editFFMC);
	m_spinFFMC.SetPos(records->m_FFMC);

	m_spinDMC.SetRangeAndDelta (0.0, 2000.0, 1.0);
	m_spinDMC.SetDecimalPlaces(2);
	m_spinDMC.SetTrimTrailingZeros (FALSE);
	m_spinDMC.SetBuddy(&m_editDMC);
	m_spinDMC.SetPos(records->m_DMC);

	m_spinDC.SetRangeAndDelta (0.0, 2000.0, 1.0);
	m_spinDC.SetDecimalPlaces(2);
	m_spinDC.SetTrimTrailingZeros (FALSE);
	m_spinDC.SetBuddy(&m_editDC);
	m_spinDC.SetPos(records->m_DC);

	m_spinYear.SetRange(records->m_Year, records->m_Year);
	m_spinYear.SetPos(records->m_Year);


    m_numSpinFM1000.SetDecimalPlaces (2);
	m_numSpinFM1000.SetTrimTrailingZeros (FALSE);
	m_numSpinFM1000.SetRangeAndDelta (0.0, 100.0, 1.0);
	
	m_numSpinFM1000.SetBuddy(&m_editFM1000);
	m_numSpinFM1000.SetPos(records->m_Start1000);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CCustomInitEditDialog::OnOK() 
{
	

	// update
	records->Edit();
	
	greenupDate.GetTime(records->m_GreenJulian);

	records->m_SIG_Station = fpSet->m_SIG_Station;
	
	records->m_StartKBDI = m_spinKBDI.GetPos();

	records->m_FFMC = (float) m_spinFFMC.GetPos();
	records->m_DMC = (float) m_spinDMC.GetPos();
	records->m_DC = (float) m_spinDC.GetPos();
	m_startUpDay.GetTime(records->m_StartUpDay);
   

	records->m_Start1000 = (float)m_numSpinFM1000.GetPos();

	records->Update();

	



	CDialog::OnOK();
}



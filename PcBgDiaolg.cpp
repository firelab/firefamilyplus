// PcBgDiaolg.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "PcBgDiaolg.h"
#include "PocketCardSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPcBgDiaolg dialog


CPcBgDiaolg::CPcBgDiaolg(CWnd* pParent /*=NULL*/)
	: CDialog(CPcBgDiaolg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPcBgDiaolg)
	gyVal = 0;
	ryVal = 0;
	yVal = 0;
	//}}AFX_DATA_INIT
	pSet = NULL;
}


void CPcBgDiaolg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPcBgDiaolg)
	DDX_Text(pDX, IDC_EDITGYVAL, gyVal);
	DDX_Text(pDX, IDC_EDITRYVAL, ryVal);
	DDX_Text(pDX, IDC_EDITYVAL, yVal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPcBgDiaolg, CDialog)
	//{{AFX_MSG_MAP(CPcBgDiaolg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPcBgDiaolg message handlers

BOOL CPcBgDiaolg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(pSet)
	{
		gyVal = pSet->m_pcGyVal;
		ryVal = pSet->m_pcRyVal;
		yVal = pSet->m_pcYVal;
	}
	//gyVal = theApp.pcGyVal;
	//ryVal = theApp.pcRyVal;
	//yVal = theApp.pcYVal;
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPcBgDiaolg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	if(gyVal < 0 || gyVal > 100 || ryVal < 0 || ryVal > 100 || yVal < 0 || yVal > 100)
	{
		AfxMessageBox("ERROR: Percents must be in range 0 - 100!");
		return;
	}
	if((gyVal + ryVal + yVal) != 100)
	{
		AfxMessageBox("ERROR: Percents must sum to 100!");
		return;
	}
	if(pSet)
	{
		pSet->Edit();
		pSet->m_pcGyVal = gyVal;
		pSet->m_pcRyVal = ryVal;
		pSet->m_pcYVal = yVal;
		pSet->Update();
	}
	//theApp.pcGyVal = gyVal;
	//theApp.pcRyVal = ryVal;
	//theApp.pcYVal = yVal;
	CDialog::OnOK();
}

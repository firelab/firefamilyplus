// BatchSavedRunMaintDialog.cpp : implementation file
//
//
#include "stdafx.h"

#include "fireplus.h"
#include "fireplusset.h"

#include "RunsSet.h"
#include "RunDescriptionSet.h"
#include "RunFireAssocSet.h"
#include "RunClimateOptionsSet.h"
#include "RunWxStationSet.h"
#include "RunFireOptionsSet.h"
#include "RunOptionsSet.h"
#include "RunPocketCardSet.h"

#include "BatchSet.h"
#include "BatchRunsSet.h"


#include "batchoptionsset.h"
#include "BatchSavedRunMaintDialog.h"
#include ".\batchsavedrunmaintdialog.h"
#include "RunMaintenanceManual.h"
#include "FFPOptsSet.h"
#include "FFPOverlaysSet.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBatchSavedRunMaintDialog dialog

CBatchSavedRunMaintDialog::CBatchSavedRunMaintDialog(CWnd* pParent /*=NULL*/,  CFireplusSet *_fpSet)
	: CDialog(CBatchSavedRunMaintDialog::IDD, pParent),bopt(_fpSet->m_pDatabase)
	, m_enableRunSaving(FALSE)
{
	
	
	fpSet = _fpSet;
	CFFPOptsSet optsSet(fpSet->m_pDatabase);
	optsSet.Open();
	if(!optsSet.IsEOF())
	{
		m_enableRunSaving = optsSet.m_RunSaveOn;
	}
	optsSet.Close();

	
}


void CBatchSavedRunMaintDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchSavedRunMaintDialog)
	// NOTE: the ClassWizard will add DDX and DDV calls here


	DDX_Text(pDX, IDC_RUNS_NUMBER, maxRuns1);
	DDX_Text(pDX, IDC_RUNS_NUMBER2, maxRuns2);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_DISABLE_RUN_SAVING, m_enableRunSaving);
}


BEGIN_MESSAGE_MAP(CBatchSavedRunMaintDialog, CDialog)
	//{{AFX_MSG_MAP(CBatchSavedRunMaintDialog)
	ON_BN_CLICKED(IDC_SCHEME_NUM, OnNumRecs)
	ON_BN_CLICKED(IDC_SCHEME_LRU, OnLRU)
    ON_BN_CLICKED(IDC_SCHEME_NONE, OnNone)
	ON_BN_CLICKED(IDC_PURGE, OnPurge)

	//}}AFX_MSG_MAP
	
	
	ON_BN_CLICKED(IDC_MANUAL_MAINTENANCE, OnBnClickedManualMaintenance)
	//ON_BN_CLICKED(IDC_SCHEME_DISABLED, OnBnClickedSchemeDisabled)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchSavedRunMaintDialog message handlers

BOOL CBatchSavedRunMaintDialog::OnInitDialog() 
{
	
	//CButton * button = (CButton*)GetDlgItem(IDC_NODUPCHECK);
	//if(button)
	//	button->SetCheck(TRUE)
	CDialog::OnInitDialog();
  if(fpSet->m_pDatabase){
		bopt.Open();
	}

	if (bopt.IsOpen()){
	

		if (bopt.IsEOF()){
            /* add first record */
			bopt.AddNew();
			bopt.m_maintScheme = tNone;
			bopt.m_maintValue = 1000;
			bopt.Update();
		}
		bopt.MoveFirst();
	    bopt.Edit();

		CEdit *Number1 = (CEdit *)GetDlgItem(IDC_RUNS_NUMBER);
		CEdit *Number2 = (CEdit *)GetDlgItem(IDC_RUNS_NUMBER2);

		   maxRuns1.Format("%d",(int) bopt.m_maintValue);
	       maxRuns2.Format("%d",(int) bopt.m_maintValue);
		   Number1->SetWindowText(maxRuns1);
		   Number2->SetWindowText(maxRuns2);
	}
	ConfigureControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CBatchSavedRunMaintDialog::OnOK(){
	if (bopt.IsOpen()){
	   ConfigureControls();
	   bopt.Update();
	   bopt.Close();
	}
	UpdateData();
	CFFPOptsSet optsSet(fpSet->m_pDatabase);
	optsSet.Open();
	if(!optsSet.IsEOF())
	{
		optsSet.Edit();
		optsSet.m_RunSaveOn = m_enableRunSaving;
		optsSet.Update();
	}
	optsSet.Close();
	CDialog::OnOK();
}
void CBatchSavedRunMaintDialog::OnNumRecs(){
	bopt.m_maintScheme = tNumRecs;
	ConfigureControls();
}
void CBatchSavedRunMaintDialog::OnLRU(){
	bopt.m_maintScheme = tLRU;
	ConfigureControls();

}

void CBatchSavedRunMaintDialog::OnNone()
{
	bopt.m_maintScheme = tNone;
	ConfigureControls();
}


void CBatchSavedRunMaintDialog::ConfigureControls()
{
	CString tempstr;

	//CButton *disabled = (CButton *)GetDlgItem(IDC_SCHEME_DISABLED);
	CButton *NumRecs = (CButton *)GetDlgItem(IDC_SCHEME_NUM);
	CButton *LRU = (CButton *)GetDlgItem(IDC_SCHEME_LRU);
	CButton *None = (CButton *)GetDlgItem(IDC_SCHEME_NONE);

	

    CEdit *Number1 = (CEdit *)GetDlgItem(IDC_RUNS_NUMBER);
	CEdit *Number2 = (CEdit *)GetDlgItem(IDC_RUNS_NUMBER2);

	//CComboBox *Period = (CComboBox *)GetDlgItem(IDC_PERIODIC_SELECT);
	
	switch(bopt.m_maintScheme)
	{
	case tNumRecs:
		NumRecs->SetCheck(TRUE);
		//disabled->SetCheck(FALSE);
		LRU->SetCheck(FALSE);
		None->SetCheck(FALSE);
		
		Number1->EnableWindow(TRUE);
		Number2->EnableWindow(FALSE);

		Number1->GetWindowText(tempstr);

		bopt.m_maintValue = atoi(tempstr);
	    
		//Continue
		break;
	case tLRU:
		NumRecs->SetCheck(FALSE);
		//disabled->SetCheck(FALSE);
		LRU->SetCheck(TRUE);
		None->SetCheck(FALSE);
		
		Number1->EnableWindow(FALSE);
		Number2->EnableWindow(TRUE);

		Number2->GetWindowText(tempstr);

		bopt.m_maintValue = atoi(tempstr);
	    
		break;
	//case tDisabled:
		//disabled->SetCheck(TRUE);
		//NumRecs->SetCheck(FALSE);
		//LRU->SetCheck(FALSE);
		//None->SetCheck(FALSE);
		//Number1->EnableWindow(FALSE);
		//Number2->EnableWindow(FALSE);

		//bopt.m_maintValue = 1000;
		//break;
	case tNone:
	default:
		bopt.m_maintScheme = tNone;
		//disabled->SetCheck(FALSE);
		NumRecs->SetCheck(FALSE);
		
		LRU->SetCheck(FALSE);
		None->SetCheck(TRUE);
		Number1->EnableWindow(FALSE);
		Number2->EnableWindow(FALSE);

		bopt.m_maintValue = 1000;

		break;
	}

}

void CBatchSavedRunMaintDialog::OnPurge()
{
	CBatchPurgeDialog bpd(this, fpSet);
	
	EnableWindow(FALSE);

	if (bpd.DoModal()== IDOK) 
		CDialog::OnOK();

}



/////////////////////////////////////////////////////////////////////////////
// CBatchPurgeDialog dialog


CBatchPurgeDialog::CBatchPurgeDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet  /* = NULL*/)
	: CDialog(CBatchPurgeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchPurgeDialog)
	//}}AFX_DATA_INIT
	
	fpSet = _fpSet;

}


void CBatchPurgeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
}


BEGIN_MESSAGE_MAP(CBatchPurgeDialog, CDialog)
	//{{AFX_MSG_MAP(CBatchPurgeDialog)
	
	
	//}}AFX_MSG_MAP
	

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchPurgeDialog message handlers





BOOL CBatchPurgeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CBatchPurgeDialog::OnOK() 
{
	///EnableWindow(FALSE);

	CString strMsg;

	// basic validation

	// find record
	
	CRunsSet runSet(fpSet->m_pDatabase);
	runSet.Open();
	if (! runSet.IsEOF())
    	runSet.MoveFirst();

	while (!runSet.IsEOF()){
		runSet.Delete();
	
		runSet.MoveNext();
	}
	runSet.Close();

    CRunOptionsSet roSet(fpSet->m_pDatabase);
	roSet.Open();
	if (! roSet.IsEOF())
    	roSet.MoveFirst();

	while (!roSet.IsEOF()){
		roSet.Delete();
	
		roSet.MoveNext();
	}
	roSet.Close();

	CRunWxStationSet rwxSet(fpSet->m_pDatabase);
	rwxSet.Open();
	if (! rwxSet.IsEOF())
    	rwxSet.MoveFirst();

	while (!rwxSet.IsEOF()){
		rwxSet.Delete();
	
		rwxSet.MoveNext();
	}
	rwxSet.Close();

	CRunFireOptionsSet rfSet(fpSet->m_pDatabase);
	rfSet.Open();
	if (! rfSet.IsEOF())
    	rfSet.MoveFirst();

	while (!rfSet.IsEOF()){
		rfSet.Delete();
	
		rfSet.MoveNext();
	}
	rfSet.Close();

    CRunClimateOptionsSet rcSet(fpSet->m_pDatabase);
	rcSet.Open();
	if (! rcSet.IsEOF())
    	rcSet.MoveFirst();

	while (!rcSet.IsEOF()){
		rcSet.Delete();
	
		rcSet.MoveNext();
	}
	rcSet.Close();

    CRunFireAssocSet rfaSet(fpSet->m_pDatabase);
	rfaSet.Open();
	if (! rfaSet.IsEOF())
    	rfaSet.MoveFirst();

	while (!rfaSet.IsEOF()){
		rfaSet.Delete();
	
		rfaSet.MoveNext();
	}
	rfaSet.Close();


    CRunDescriptionSet rdSet(fpSet->m_pDatabase);
	rdSet.Open();
	if (! rdSet.IsEOF())
    	rdSet.MoveFirst();

	while (!rdSet.IsEOF()){
		rdSet.Delete();
	
		rdSet.MoveNext();
	}
	rdSet.Close();

	CRunPocketCardSet rpcSet(fpSet->m_pDatabase);
	rpcSet.Open();
	if (! rpcSet.IsEOF())
    	rpcSet.MoveFirst();

	while (!rpcSet.IsEOF()){
		rpcSet.Delete();
	
		rpcSet.MoveNext();
	}
	rpcSet.Close();

   CBatchRunsSet brSet(fpSet->m_pDatabase);
	brSet.Open();
	if (! brSet.IsEOF())
    	brSet.MoveFirst();

	while (!brSet.IsEOF()){
		brSet.Delete();
	
		brSet.MoveNext();
	}
	brSet.Close();

	CBatchSet batchSet(fpSet->m_pDatabase);
	batchSet.Open();
	

    if (! batchSet.IsEOF())
		batchSet.MoveFirst();

	while(!batchSet.IsEOF()){
		batchSet.Delete();

		batchSet.MoveNext();

	}
	batchSet.Close();

	CFFPOverlaysSet oSet(fpSet->m_pDatabase);
	oSet.Open();
	if(!oSet.IsEOF())
		oSet.MoveFirst();
	while(!oSet.IsEOF())
	{
		if(oSet.m_runID > 0)//0 is for General FFP Options...don't kill them!
		{
			oSet.Delete();
		}
		oSet.MoveNext();
	}
	oSet.Close();

	strMsg.Format("All batches and saved runs have been purged.\n");
	AfxMessageBox(strMsg);
	CDialog::OnOK();
	
}

void CBatchSavedRunMaintDialog::OnBnClickedManualMaintenance()
{
	CRunsSet runSet(fpSet->m_pDatabase);
	runSet.Open();
	if (runSet.IsEOF() || runSet.IsBOF())
	{
		AfxMessageBox("No existing saved runs.");
		runSet.Close();
		return;
	}
	runSet.Close();

	CRunMaintenanceManual rmmd(this, fpSet->m_pDatabase);
	rmmd.DoModal();
}


// SeverityDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "fireplusSet.h"
#include "SIGStationSet.h"
#include "SeverityDialog.h"
#include "climateset.h"
#include "wxSet.h"
#include "fireplusSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "splitterframe.h"
#include "FireSumSet.h"
#include "climAnalysis.h"
#include "FFPOptsSet.h"
#include "MainFrm.h"

#include "RunSave.h"

extern int curr_runID;
extern CFireplusApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSeverityDialog dialog

CSeverityDialog::CSeverityDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet /*=NULL*/, int _doSummary/* = 0*/)
	: CDialog(CSeverityDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSeverityDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	fpSet = _fpSet;
	doSummary = _doSummary;
}

void CSeverityDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSeverityDialog)
	DDX_Control(pDX, IDC_COMBO1, indexList);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SPINCP, m_spinCP);
}

BEGIN_MESSAGE_MAP(CSeverityDialog, CDialog)
	//{{AFX_MSG_MAP(CSeverityDialog)

	ON_COMMAND(ID_VARSORT, OnVarSort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeverityDialog message handlers

BOOL CSeverityDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	if(doSummary)
		SetWindowText("Generate Seasonal Severity Summary");
	if(fpSet)
	{
		try
		{    // requery recordset
			CClimateSet climSet(fpSet->m_pDatabase);
			climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
			climSet.setVarSortOrder(theApp.varSortOrder);
			climSet.Open();
			if (climSet.IsOpen())
			{
				while (!climSet.IsEOF())
				{
					CString str = climSet.m_Variable_Name;
					int eLoc = indexList.AddString(str);
					indexList.SetItemData(eLoc, climSet.m_VarID - 1);
					climSet.MoveNext();
				}
			}
			climSet.Close();
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations
			// when opening tables
			e->Delete( );
		}
	}
	m_spinCP.SetRange(1, 100);
	m_spinCP.SetPos(1);
	CheckRadioButton(IDC_GREATERTHANCP, IDC_LESSTHANCP, IDC_GREATERTHANCP);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSeverityDialog::OnOK()
{
	// TODO: Add extra validation here
	if(indexList.GetCurSel() == CB_ERR)
	{
		CString strMsg;
		strMsg.Format(
			"ERROR: You must select an index." );
		AfxMessageBox(strMsg);
		return;
	}

	int *varIDs = new int[1];
	varIDs[0] = (int) indexList.GetItemData(indexList.GetCurSel());

	int id = GetCheckedRadioButton(IDC_GREATERTHANCP, IDC_LESSTHANCP);

	// added for batch
	// single run mode
	CRunSave saveRun(fpSet);
	CString runDesc;
	if (doSummary)
		runDesc.Format("Severity Summary: ");
	else
		runDesc.Format("Severity List: ");

    CClimateSet cSet(fpSet->m_pDatabase);
	cSet.m_strFilter.Format("[VarID]=%d",varIDs[0]);

	cSet.Open();
	if (cSet.IsOpen())
	{
		runDesc.Append(cSet.m_ShortName);
	}
	cSet.Close();

	saveRun.SaveGeneral(0, runDesc , fpSet);
	saveRun.SaveClimateOptions(curr_runID, fpSet);
	// removed 8/2006 saveRun.SaveWxStation(curr_runID, fpSet);
	saveRun.SaveFireOptions(curr_runID, fpSet);

	CString value1;

	// var
	value1.Format("%2d", indexList.GetCurSel());

	saveRun.SaveRunOption(curr_runID,  (doSummary ? REPORT_WEATHER_SEVERITY_SUMM : REPORT_WEATHER_SEVERITY_LIST), 0, "varID", value1, fpSet);

	// CP
	value1 = "1";
	if (m_spinCP.GetPos())
		value1.Format("%3d", m_spinCP.GetPos());
	saveRun.SaveRunOption(curr_runID, (doSummary ? REPORT_WEATHER_SEVERITY_SUMM : REPORT_WEATHER_SEVERITY_LIST),0,"cp",value1,fpSet);

	// operator greaterthan/lessthan
	if(id == IDC_GREATERTHANCP)// ? false : true
		value1 = "0";//.Format("%2d",id);
	else
		value1 = "1";
	saveRun.SaveRunOption(curr_runID,  (doSummary ? REPORT_WEATHER_SEVERITY_SUMM : REPORT_WEATHER_SEVERITY_LIST),0,"radiobutton",value1,fpSet);

	// end single run mode
	// end added for batch
	//set period length to 1

	fpSet->Edit();
	fpSet->m_PeriodLength = "1";
	fpSet->Update();
	CClimAnalysis analysis(fpSet->m_pDatabase);
	//EnableWindow(FALSE);
	int ret = analysis.RunAnalyze(varIDs, 1, fpSet, false);
		//Analyze(this, varIDs, 1, fpSet, false);
	//EnableWindow(TRUE);
	if(ret == IDOK)
	{
		if(!doSummary)
			analysis.SeverityList((double)m_spinCP.GetPos(), id == IDC_GREATERTHANCP ? false : true);
		else
			analysis.SeveritySummary((double)m_spinCP.GetPos(), id == IDC_GREATERTHANCP ? false : true);
		CDialog::OnOK();
	}
}

void CSeverityDialog::OnVarSort()
{
	// change the app's varSortOrder (and save to ffpOptions)
	// then re-sort and re-display

	if (theApp.varSortOrder < 0 || theApp.varSortOrder > 2)
		theApp.varSortOrder = 0;
	theApp.varSortOrder += 1;

	if (theApp.varSortOrder < 0 || theApp.varSortOrder > 2)
		theApp.varSortOrder = 0;

	CFFPOptsSet optsSet1(fpSet->m_pDatabase);
	optsSet1.Open();
	if(!optsSet1.IsEOF())
	{
			optsSet1.Edit();
			optsSet1.m_VarSortOrder = theApp.varSortOrder;
			optsSet1.Update();
	}
	optsSet1.Close();

	CClimateSet climSet(fpSet->m_pDatabase);
	climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";

	climSet.setVarSortOrder(theApp.varSortOrder);

	climSet.Open();

	indexList.ResetContent();

	while (!climSet.IsEOF())
				{
					CString str = climSet.m_Variable_Name;
					int eLoc = indexList.AddString(str);
					indexList.SetItemData(eLoc, climSet.m_VarID - 1);
					climSet.MoveNext();
				}

	climSet.Close();

	CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));
}
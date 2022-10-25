// ListingDialog.cpp : implementation file
//
 
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "SIGStationSet.h"
#include "ClimateSet.h"
#include "wxSet.h"
#include "fireplusSet.h"
#include "ListingDialog.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "CFiresSet.h"
#include "FFPOptsSet.h"
#include "ClimAnalysis.h"
#include "DiurnalDoc.h"
#include "RunSave.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern int curr_runID;

/////////////////////////////////////////////////////////////////////////////
// CListingDialog dialog
ListingOptions::ListingOptions()
{
	reportHeader = columnHeader = dateStamp = true;
	fires = largeFires = acres = numPerClass = false;
	lfAcres = 5;
	fCause = 0;
	dt = Full;
	tt = NoTime;
	stnPerRecord = false;
}

CListingDialog::CListingDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet /*=NULL*/, BOOL daily)
	: CDialog(CListingDialog::IDD, pParent)
	, m_stnPerRecord(FALSE)
{
	//{{AFX_DATA_INIT(CListingDialog)
	reportHeader = FALSE;
	columnHeader = FALSE;
	dateStamp = FALSE;
	m_numFires = FALSE;
	m_numLargeFires = FALSE;
	m_LFD = 0;
	m_Acres = FALSE;
	m_NumPerClass = FALSE;
	m_stnPerRecord = FALSE;
	//}}AFX_DATA_INIT
	fpSet = _fpSet;
	isDaily = daily;
	dt = theApp.lo.dt;
	tt = theApp.lo.tt;
	if(!daily && tt == NoTime)
		tt = PackedTime;
	fireCause = theApp.lo.fCause;
	reportHeader = theApp.lo.reportHeader;
	columnHeader = theApp.lo.columnHeader;
	dateStamp = theApp.lo.dateStamp;
	m_LFD = theApp.lo.lfAcres;
	m_numFires = theApp.lo.fires;
	m_numLargeFires = theApp.lo.largeFires;
	m_Acres = theApp.lo.acres;
	m_NumPerClass = theApp.lo.numPerClass;
	m_stnPerRecord = theApp.lo.stnPerRecord;
/*
	lo.columnHeader = columnHeader == 0 ? false : true;
	lo.reportHeader = reportHeader == 0 ? false : true;
	lo.dateStamp = dateStamp == 0 ? false : true;
	lo.fires = m_numFires == 0 ? false : true;
	lo.largeFires = m_numLargeFires == 0 ? false : true;
	lo.lfAcres = m_LFD;
	lo.fCause = fireCause;
	lo.dt = dt;
	lo.tt = tt;
*/
}

void CListingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListingDialog)
	DDX_Control(pDX, IDC_SPIN1, m_spinLFD);
	DDX_Control(pDX, IDC_LIST2, destList);
	DDX_Control(pDX, IDC_LIST1, sourceList);
	DDX_Check(pDX, IDC_REPORTHEADER, reportHeader);
	DDX_Check(pDX, IDC_COLUMNHEADER, columnHeader);
	DDX_Check(pDX, IDC_DATESTAMP, dateStamp);
	DDX_Check(pDX, IDC_NUMFIRES, m_numFires);
	DDX_Check(pDX, IDC_NUMLARGEFIRES, m_numLargeFires);
	DDX_Text(pDX, IDC_LFDNUM, m_LFD);
	DDV_MinMaxInt(pDX, m_LFD, 1, 10000);
	DDX_Check(pDX, IDC_ACRES, m_Acres);
	DDX_Check(pDX, IDC_NUMPERCLASS, m_NumPerClass);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_STATIONID, m_stnPerRecord);
}

BEGIN_MESSAGE_MAP(CListingDialog, CDialog)
	//{{AFX_MSG_MAP(CListingDialog)
	ON_BN_CLICKED(IDC_MOVELEFT, OnMoveleft)
	ON_BN_CLICKED(IDC_MOVERIGHT, OnMoveright)
	ON_BN_CLICKED(IDC_REMOVEALL, OnRemoveall)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	ON_LBN_DBLCLK(IDC_LIST2, OnDblclkList2)
	ON_BN_CLICKED(IDC_TIME1, OnTime1)
	ON_BN_CLICKED(IDC_TIME2, OnTime2)
	ON_BN_CLICKED(IDC_NOTIME, OnNotime)
	ON_BN_CLICKED(IDC_DATE1, OnDate1)
	ON_BN_CLICKED(IDC_DATE2, OnDate2)
	ON_BN_CLICKED(IDC_DATE3, OnDate3)
	ON_BN_CLICKED(IDC_DATE4, OnDate4)
	ON_BN_CLICKED(IDC_DATE5, OnDate5)
	ON_BN_CLICKED(IDC_ALLFIRES, OnAllfires)
	ON_BN_CLICKED(IDC_HUMAN, OnHuman)
	ON_BN_CLICKED(IDC_LIGHTNING, OnLightning)
	ON_BN_CLICKED(IDC_DL_EXPORT, OnDLExport)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_TOP, OnTop)
	ON_BN_CLICKED(IDC_BOTTOM, OnBottom)
	ON_COMMAND(ID_VARSORT, OnVarSort)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListingDialog message handlers

void CListingDialog::OnMoveleft()
{
	// TODO: Add your control notification handler code here
	if(destList.GetCurSel() != LB_ERR)
		OnDblclkList2();
}

void CListingDialog::OnMoveright()
{
	// TODO: Add your control notification handler code here
	if(sourceList.GetCurSel() != LB_ERR)
		OnDblclkList1();
}

void CListingDialog::OnRemoveall()
{
	// TODO: Add your control notification handler code here
	CString str;
	for(int i = 0; i < destList.GetCount(); i++)
	{
		destList.GetText(i, str);
		sourceList.AddString(str);
	}
	destList.ResetContent();
}

void CListingDialog::OnSelectall()
{
	// TODO: Add your control notification handler code here
	CString str;
	for(int i = 0; i < sourceList.GetCount(); i++)
	{
		sourceList.GetText(i, str);
		destList.AddString(str);
	}
	sourceList.ResetContent();
}

void CListingDialog::OnOK()
{
	// TODO: Add extra validation here
	//check selected variables, create array of VarIDs,
	//and create a CClimAnalysis with data dump of varIDs
	if(UpdateData() == 0)
		return;
	int createTable = 0;
	int vars = destList.GetCount(), loc;
	if(vars <= 0)
	{
		CString str;
		str.Format("ERROR: Must select at least one variable!");
		AfxMessageBox(str);
		return;
	}
	int *varIDs = new int[vars];
	CClimateSet climSet(fpSet->m_pDatabase);
	climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 2 Or [OptionType] = 3 Or [OptionType] = 4";
	climSet.setVarSortOrder(theApp.varSortOrder);
	//climSet.m_strSort = "[VarID]";
	climSet.Open();

// added for batch
	// single run mode
	CRunSave saveRun(fpSet);
	CString runDesc;
	runDesc.Format("Daily Listing: ");
    int repCount = 0;
	if (climSet.IsOpen())
	{
		while (!climSet.IsEOF())
		{
			loc = destList.FindStringExact(-1, climSet.m_Variable_Name);
			climSet.Edit();
			if(loc != LB_ERR)
			{
                if (repCount)
					runDesc.Append(", ");
				repCount++;
				runDesc.Append(climSet.m_ShortName);
			}

			climSet.MoveNext();
		}
		climSet.MoveFirst();
	}
	CString value1;
	int i=0;
	saveRun.SaveGeneral(0, runDesc, fpSet);
	if (climSet.IsOpen())
	{
		while (!climSet.IsEOF())
		{
			loc = destList.FindStringExact(-1, climSet.m_Variable_Name);
			climSet.Edit();
			if(loc != LB_ERR)
			{
				varIDs[loc] = climSet.m_VarID - 1;
				climSet.m_Daily_List = loc + 1;

				// added for batch
				value1.Format("%2d", varIDs[loc]);
				saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,i,"varID",value1,fpSet);
				i++;
				// end added for batch
			}
			else
				climSet.m_Daily_List = 0;
			climSet.Update();
			climSet.MoveNext();
		}
	}
	climSet.Close();

	saveRun.SaveClimateOptions(curr_runID, fpSet);
	// removed 8/2006 saveRun.SaveWxStation(curr_runID, fpSet);
	saveRun.SaveFireOptions(curr_runID, fpSet);

	// save variables

	value1.Format("%2d", m_Acres);
	saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,0,"checkbox_TotalAcres",value1,fpSet);

	value1.Format("%2d", m_NumPerClass);
	saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,0,"checkbox_NumPerClass",value1,fpSet);

	value1.Format("%2d", m_numFires);
	saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,0,"checkbox_NumFires",value1,fpSet);

	value1.Format("%2d", m_numLargeFires);
	saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,0,"checkbox_NumLargeFires",value1,fpSet);

	value1.Format("%5d", m_LFD);
	saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,0,"spin_LFDAcres",value1,fpSet);

	value1.Format("%2d", fireCause);
	saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,0,"radio_fireCause",value1,fpSet);

	value1.Format("%2d", reportHeader);
	saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,0,"checkbox_reportHeader",value1,fpSet);

	value1.Format("%2d", columnHeader);
	saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,0,"checkbox_columnHeader",value1,fpSet);

	value1.Format("%2d", dateStamp);
	saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,0,"checkbox_dateStamp",value1,fpSet);

	value1.Format("%2d", dt);
	saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,0,"radio_DateFormat",value1,fpSet);

	value1.Format("%2d", tt);
	saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,0,"radio_TimeFormat",value1,fpSet);

	value1.Format("%2d", m_stnPerRecord);
	saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,0,"checkbox_StnPerRecord",value1,fpSet);


	CFFPOptsSet optsSet1(fpSet->m_pDatabase);
	optsSet1.Open();
	if(!optsSet1.IsEOF())
	{
		value1.Format("%2d", optsSet1.m_DLExport);
		saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_DAILY_LISTING,0,"checkbox_ExportToDB",value1,fpSet);
		createTable = (int) optsSet1.m_DLExport;
	}
	optsSet1.Close();

	// end single run mode
	// end added for batch

	//ListingOptions lo;
	theApp.lo.acres = m_Acres == 0 ? false : true;
	theApp.lo.numPerClass = m_NumPerClass == 0 ? false : true;
	theApp.lo.columnHeader = columnHeader == 0 ? false : true;
	theApp.lo.reportHeader = reportHeader == 0 ? false : true;
	theApp.lo.dateStamp = dateStamp == 0 ? false : true;
	theApp.lo.fires = m_numFires == 0 ? false : true;
	theApp.lo.largeFires = m_numLargeFires == 0 ? false : true;
	theApp.lo.lfAcres = m_LFD;
	theApp.lo.fCause = fireCause;
	theApp.lo.dt = dt;
	theApp.lo.tt = tt;
	theApp.lo.stnPerRecord = m_stnPerRecord == 0 ? false : true;

	int ret = 0;
	if(isDaily)
	{
		delete[] varIDs;
	}
	else
	{
		EnableWindow(FALSE);
		CDiurnalAnalysis da;
		ret = da.HourlyListing(this, fpSet, vars, varIDs, &theApp.lo,createTable);
		EnableWindow(TRUE);
	}
	if(ret == 0)
		CDialog::OnOK();
}

void CListingDialog::OnDblclkList1()
{
	// TODO: Add your control notification handler code here
	CString str;
	int cur = sourceList.GetCurSel();
	sourceList.GetText(cur, str);
	destList.AddString(str);
	sourceList.DeleteString(cur);
}

void CListingDialog::OnDblclkList2()
{
	// TODO: Add your control notification handler code here
	CString str;
	int cur = destList.GetCurSel();
	destList.GetText(cur, str);
	sourceList.AddString(str);
	destList.DeleteString(cur);
}

BOOL CListingDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	CClimateSet climSet(fpSet->m_pDatabase);
	climSet.m_strFilter = "[OptionType] >= 0 AND [Daily List] < 1";
	climSet.setVarSortOrder(theApp.varSortOrder);
	//climSet.m_strSort = "[VarID]";
	climSet.Open();
	if (climSet.IsOpen())
	{
		while (!climSet.IsEOF())
		{
			sourceList.AddString(climSet.m_Variable_Name);
			climSet.MoveNext();
		}
	}
	climSet.m_strFilter = "[OptionType] >= 0 AND [Daily List] > 0";
	climSet.m_strSort = "[Daily List]";
	climSet.Requery();
	if (climSet.IsOpen())
	{
		while (!climSet.IsEOF())
		{
			destList.AddString(climSet.m_Variable_Name);
			climSet.MoveNext();
		}
	}
	climSet.Close();
	upButton.AutoLoad(IDC_UP, this);
	downButton.AutoLoad(IDC_DOWN, this);
	topButton.AutoLoad(IDC_TOP, this);
	bottomButton.AutoLoad(IDC_BOTTOM, this);
	CButton * button;
	switch(dt)
	{
	case Full:
		button = (CButton *)GetDlgItem(IDC_DATE1);
		break;
	case Packed:
		button = (CButton *)GetDlgItem(IDC_DATE2);
		break;
	case Reverse:
		button = (CButton *)GetDlgItem(IDC_DATE3);
		break;
	case MD:
		button = (CButton *)GetDlgItem(IDC_DATE4);
		break;
	case MDPacked:
		button = (CButton *)GetDlgItem(IDC_DATE5);
		break;
	default:
		button = (CButton *)GetDlgItem(IDC_DATE1);
	}
	button->SetCheck(TRUE);
	switch(fireCause)
	{
	case 0:
		button = (CButton *)GetDlgItem(IDC_ALLFIRES);
		break;
	case 2:
		button = (CButton *)GetDlgItem(IDC_HUMAN);
		break;
	case 1:
		button = (CButton *)GetDlgItem(IDC_LIGHTNING);
		break;
	default:
		button = (CButton *)GetDlgItem(IDC_ALLFIRES);
	}
	button->SetCheck(TRUE);
	switch(tt)
	{
	case NoTime:
		button = (CButton *)GetDlgItem(IDC_NOTIME);
		break;
	case FullTime:
		button = (CButton *)GetDlgItem(IDC_TIME1);
		break;
	case PackedTime:
		button = (CButton *)GetDlgItem(IDC_TIME2);
		break;
	default:
		button = (CButton *)GetDlgItem(IDC_NOTIME);
	}
	button->SetCheck(TRUE);

	CFFPOptsSet optsSet1(fpSet->m_pDatabase);
	optsSet1.Open();
	if(!optsSet1.IsEOF())
	{
			if (optsSet1.m_DLExport)
			{
                button = (CButton *) GetDlgItem(IDC_DL_EXPORT);
				if (button)
					button->SetCheck(TRUE);

			}
	}
	optsSet1.Close();
		//button = (CButton *)GetDlgItem(IDC_TIME1);
		//button->SetCheck(TRUE);
		//tt = FullTime;
	if(!isDaily)
	{
		SetWindowText("Select Output Variables for Hourly Listing");
		//make fires stuff nonselectable
		GetDlgItem(IDC_NUMPERCLASS)->EnableWindow(FALSE);
		GetDlgItem(IDC_ACRES)->EnableWindow(FALSE);
		GetDlgItem(IDC_NUMFIRES)->EnableWindow(FALSE);
		GetDlgItem(IDC_NUMLARGEFIRES)->EnableWindow(FALSE);
		GetDlgItem(IDC_LFDNUM)->EnableWindow(FALSE);
		GetDlgItem(IDC_ALLFIRES)->EnableWindow(FALSE);
		GetDlgItem(IDC_HUMAN)->EnableWindow(FALSE);
		GetDlgItem(IDC_LIGHTNING)->EnableWindow(FALSE);
		m_spinLFD.EnableWindow(FALSE);
	}
	/*else
	{
		button = (CButton *)GetDlgItem(IDC_NOTIME);
		button->SetCheck(TRUE);
	}*/
	m_spinLFD.SetRange(1, 10000);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CListingDialog::OnUp()
{
	CWaitCursor wait;
	if(destList.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = destList.GetCurSel();
		if(cur > 0)
		{
			destList.GetText(cur, str);
			destList.DeleteString(cur);
			cur--;
			int loc = destList.InsertString(cur, str);
			destList.SetCurSel(loc);
		}
	}
}

void CListingDialog::OnTop()
{
	CWaitCursor wait;
	if(destList.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = destList.GetCurSel();
		if(cur > 0)
		{
			destList.GetText(cur, str);
			destList.DeleteString(cur);
			int loc = destList.InsertString(0, str);
			destList.SetCurSel(loc);
		}
	}
}

void CListingDialog::OnDown()
{
	CWaitCursor wait;
	if(destList.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = destList.GetCurSel();
		if(cur < destList.GetCount() - 1)
		{
			destList.GetText(cur, str);
			destList.DeleteString(cur);
			cur++;
			int loc = destList.InsertString(cur, str);
			destList.SetCurSel(loc);
		}
	}
}

void CListingDialog::OnBottom()
{
	CWaitCursor wait;
	if(destList.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = destList.GetCurSel();
		if(cur < destList.GetCount() - 1)
		{
			destList.GetText(cur, str);
			destList.DeleteString(cur);
			int loc = destList.InsertString(-1, str);
			destList.SetCurSel(loc);
		}
	}
}

void CListingDialog::OnTime1()
{
	tt = FullTime;
}

void CListingDialog::OnTime2()
{
	tt = PackedTime;
}

void CListingDialog::OnNotime()
{
	tt = NoTime;
}

void CListingDialog::OnDate1()
{
	dt = Full;
}

void CListingDialog::OnDate2()
{
	dt = Packed;
}

void CListingDialog::OnDate3()
{
	dt = Reverse;
}

void CListingDialog::OnDate4()
{
	dt = MD;
}

void CListingDialog::OnDate5()
{
	dt = MDPacked;
}

void CListingDialog::OnAllfires()
{
	fireCause = 0;
}

void CListingDialog::OnHuman()
{
	fireCause = 2;
}

void CListingDialog::OnLightning()
{
	fireCause = 1;
}

void CListingDialog::OnVarSort()
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

	//climSet.m_strFilter = "[Daily List] > 0";
	climSet.setVarSortOrder(theApp.varSortOrder);

	climSet.Open();

	sourceList.ResetContent();

	while (!climSet.IsEOF())
	{
		// make sure it's not already in the destination list
		int loc = destList.FindStringExact(-1, climSet.m_Variable_Name);
		if(loc == LB_ERR)
			sourceList.AddString(climSet.m_Variable_Name);

		climSet.MoveNext();
	}
	climSet.Close();

	CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));
}


void CListingDialog::OnDLExport()
{
	// change the DL_Export option in FFPOpts
	

	

	CFFPOptsSet optsSet1(fpSet->m_pDatabase);
	optsSet1.Open();
	if(!optsSet1.IsEOF())
	{
			optsSet1.Edit();
			if (optsSet1.m_DLExport >= 1)
				optsSet1.m_DLExport = 0;
			else
				optsSet1.m_DLExport = 1;
			optsSet1.Update();
	}
	optsSet1.Close();

	
}
// FireAnalysisDialog.cpp : implementation file
//
 
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
//#include "datatbl.h"
//#include "pvnumeric.h"
//#include "pvbutton1.h"
#include "fireplusSet.h"
#include "FireAnalysisDialog.h"
#include "FireOptionsSet.h"
#include "SIGStationSet.h"
#include "wxSet.h"
#include "ClimateSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "splitterframe.h"
#include "CFiresSet.h"
#include "ClimAnalysis.h"
#include "LogitDialog.h"
#include "firestats.h"
#include "firestatdoc.h"
#include <afxrich.h>
#include "richfiredoc.h"
#include "firestatframe.h"
#include "cumulativeframe.h"
#include <process.h>
#include "p_chisq.h"
#include "RunSave.h"
#include "LogReg.h"
#include "NelsonDFM.h"
#include "FFPOptsSet.h"
#include "MainFrm.h"
#include "FireCauseFilterDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern int curr_runID;

//__declspec( dllimport ) void LogRegs(char *inname, char *outname);

long number_range( long from, long to )
{
 /*   int power;
    int number;
    if ( ( to = to - from + 1 ) <= 1 )
    	return from;
    for ( power = 2; power < to; power <<= 1 )
    	;
    while ( ( number = number_mm( ) & ( power - 1 ) ) >= to )
    	;
    return from + number;*/
	int r = rand();
	long x = (long)(((double)to) * ((double)r) / ((double)RAND_MAX));
	return x;
}
/*
* this is the Mitchell-Moore algorithm from Knuth Volume II.
*/
void init_mm( )
{
   /* int *piState;
    int iState;
    piState	= &rgiState[2];
    piState[-2]	= 55 - 55;
    piState[-1]	= 55 - 24;
    piState[0]	= ( (int) time( NULL ) ) & ( ( 1 << 30 ) - 1 );
    piState[1]	= 1;
    for ( iState = 2; iState < 55; iState++ )

        {
        	piState[iState] = ( piState[iState-1] + piState[iState-2] )
        			& ( ( 1 << 30 ) - 1 );
    }*/
   srand( (unsigned)time( NULL ) );
}

/*long SubsampleLogitFile(char *fName, long nV, double pcnt, long *nHits)
{
	long nv = nV, h = 0, loc, rand;
	char buf[65], hdr[65];
	nv = (long)(pcnt * (double)nv);
	int *isFD = new int[nV];
	double *vals = new double[nV];
	FILE *fds = fopen(fName, "rt");
	fgets(hdr, 64, fds);//save first line
	fgets(buf, 64, fds);
	loc = 0;
	while(!feof(fds))
	{
		isFD[loc] = atoi(buf);
		vals[loc] = atof(&buf[1]);
		fgets(buf, 64, fds);
		loc++;
	}
	fclose(fds);
	fds = fopen(fName, "wt");
	fprintf(fds, "%s", hdr);
	init_mm(); //sets seed for random number generator
	loc = 0;
	while(loc < nv)
	{
		rand = number_range(0, nV - 1);
		if(isFD[rand] >= 0)
		{
			fprintf(fds, "%d %f\n", isFD[rand], vals[rand]);
			if(isFD[rand] > 0)
				h++;
			isFD[rand] = -1;
			loc++;
		}
	}
	fclose(fds);
	delete[] isFD;
	delete[] vals;
	*nHits = h;
	return nv;
}*/

/////////////////////////////////////////////////////////////////////////////
// CFireAnalysisDialog dialog

CFireAnalysisDialog::CFireAnalysisDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet/*=NULL*/, bool _isNelson /*= false*/, bool _isHalfNelson /*= false*/)
	: CDialog(CFireAnalysisDialog::IDD, pParent), optionsSet(_fpSet->m_pDatabase)
{
	//{{AFX_DATA_INIT(CFireAnalysisDialog)
	isNelson = _isNelson;
	isHalfNelson = _isHalfNelson;
	conditional = FALSE;
	//}}AFX_DATA_INIT
	conditional = TRUE;
	fpSet = _fpSet;
	optionsSet.m_strFilter.Format("[SIG/Station] = '%s'", fpSet->m_SIG_Station);
	optionsSet.Open();
	if(optionsSet.IsEOF() || optionsSet.IsBOF())
	{//no record for SIG/Station, add one
		optionsSet.AddNew();
		optionsSet.m_SIG_Station = fpSet->m_SIG_Station;
		optionsSet.m_All = true;
		optionsSet.m_Lightning = false;
		optionsSet.m_Human = false;
		optionsSet.m_CauseFlag = 1022;
		optionsSet.m_Both = true;
		optionsSet.m_Cumulative = false;
		optionsSet.m_Probability = false;
		optionsSet.m_LFAcres = 10;
		optionsSet.m_MFDFires = 5;
		optionsSet.m_VarID = 1;
		optionsSet.m_Conditional = TRUE;
		optionsSet.Update();
		optionsSet.Requery();
	}
	causeFlag = optionsSet.m_CauseFlag;

}

void CFireAnalysisDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFireAnalysisDialog)
	DDX_Control(pDX, IDC_RADIO5, cumulative);
	DDX_Control(pDX, IDC_RADIO2, lightning);
	DDX_Control(pDX, IDC_COMBO1, varIDs);
	DDX_Check(pDX, IDC_CONDITIONAL, conditional);
	DDX_Control(pDX,ID_CAUSE_MORE,moreCauses);
	
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SPINFIRES, m_spinFires);
	DDX_Control(pDX, IDC_SPINACRES, m_spinAcres);
}

BEGIN_MESSAGE_MAP(CFireAnalysisDialog, CDialog)
	//{{AFX_MSG_MAP(CFireAnalysisDialog)
	ON_BN_CLICKED(IDC_CONDITIONAL, OnConditional)
	ON_COMMAND(ID_VARSORT, OnVarSort)
	ON_BN_CLICKED(ID_CAUSE_MORE,OnCauseFilter)
	ON_BN_CLICKED(IDC_RADIO2,OnLightning)
	ON_BN_CLICKED(IDC_RADIO3,OnHuman)
	ON_BN_CLICKED(IDC_RADIO4,OnAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireAnalysisDialog message handlers

void CFireAnalysisDialog::OnOK()
{
	// TODO: Add extra validation here
	CWaitCursor wait;
	UpdateData();
	int selloc = varIDs.GetCurSel();
	bool cumu = false, regress = false;
	if(selloc < 0)
	{
		CString str;
		str.Format("ERROR: No analysis variable selected.\n");
		AfxMessageBox(str);
		return;
	}
	optionsSet.Edit();
	optionsSet.m_VarID = (byte)varIDs.GetItemData(selloc);
	optionsSet.m_LFAcres = m_spinAcres.GetPos();//lfAcres.GetValueInteger();
	optionsSet.m_MFDFires = m_spinFires.GetPos();//mfdFires.GetValueInteger();
	optionsSet.m_CauseFlag = causeFlag;

	if(cumulative.GetCheck())
	{
		optionsSet.m_Cumulative = true;
		optionsSet.m_Probability = false;
		optionsSet.m_Both = false;
		cumu = true;
	}
	else if(((CButton *)GetDlgItem(IDC_RADIO6))->GetCheck())
	{
		optionsSet.m_Cumulative = false;
		optionsSet.m_Probability = true;
		optionsSet.m_Both = false;
		regress = true;
	}
	else
	{
		optionsSet.m_Cumulative = false;
		optionsSet.m_Probability = false;
		optionsSet.m_Both = true;
		cumu = true;
		regress = true;
	}
	if(lightning.GetCheck())
	{
		optionsSet.m_Lightning = true;
		optionsSet.m_Human = false;
		optionsSet.m_All = false;
	}
	else if(((CButton *)GetDlgItem(IDC_RADIO3))->GetCheck())
	{
		optionsSet.m_Lightning = false;
		optionsSet.m_Human = true;
		optionsSet.m_All = false;
	}
	else
	{
		optionsSet.m_Lightning = false;
		optionsSet.m_Human = false;
		optionsSet.m_All = true;
	}
	optionsSet.m_Conditional = conditional;
	optionsSet.Update();

	// added for batch
	// single run mode
	if(!isNelson)
	{
		CRunSave saveRun(fpSet);
		saveRun.SaveGeneral(0, "Fires Analysis" , fpSet);
		saveRun.SaveClimateOptions(curr_runID, fpSet);
		// removed 8/2006 saveRun.SaveWxStation(curr_runID, fpSet);
		// note that the optionsSet.Update() above saved MOST of the dialog options to the fire options table
		saveRun.SaveFireOptions(curr_runID, fpSet);
		// save conditional checkbox and fireday spinner
		CString value1;
		value1.Format("%2d", conditional);
		CString vStr;
		vStr.Format("%d", optionsSet.m_VarID);
		saveRun.SaveRunOption(curr_runID, REPORT_FIRES_ANALYSIS,0,"varID",vStr,fpSet);
		saveRun.SaveRunOption(curr_runID, REPORT_FIRES_ANALYSIS,0,"check_conditional",value1,fpSet);
	}
//	value1.Format("%6d", m_spinSample.GetPos());
	//saveRun.SaveRunOption(curr_runID, REPORT_FIRES_ANALYSIS,0,"spin_Sample",value1,fpSet);
	// end added for batch

	//everything OK in this dialog,
	//set period length to 1

	fpSet->Edit();
	fpSet->m_PeriodLength = "1";
	fpSet->Update();
	//now make sure there's fire data
	CFiresSet fires(fpSet->m_pDatabase);
	fires.Open();
	int cause = 0;
	if(optionsSet.m_Lightning)
		cause = 1;
	else if(optionsSet.m_Human)
		cause = 2;
	if(!fires.FilterToWorkingSet(fpSet, cause,causeFlag))
	{
		fires.Close();
		AfxMessageBox("No fires found.\nSet fire associations, adjust working set dates,\nor import fires.");
		return;
	}
	//get the variable's data
	CString varName;
	CClimateSet climSet(fpSet->m_pDatabase);
	climSet.setVarSortOrder(theApp.varSortOrder);
	climSet.m_strFilter.Format("[VarID] = %d", optionsSet.m_VarID);
	climSet.Open();
	if(climSet.IsEOF() || climSet.IsBOF())
		varName = "VAR";
	else
		varName = climSet.m_ShortName;
	CClimAnalysis *analysis = NULL;//(fpSet->m_pDatabase);
	//CClimAnalysis analysis(fpSet->m_pDatabase);
	CNelsonDFM nelson(this);
	if(!isNelson)
	{
		analysis = new CClimAnalysis(fpSet->m_pDatabase);
		analysis->humanCause = causeFlag;

		int *varID = new int[1];
		varID[0] = optionsSet.m_VarID - 1;
		//EnableWindow(FALSE);
		int ret = analysis->RunAnalyze(varID, 1, fpSet, false);
			//analysis->Analyze(this, varID, 1, fpSet, false);
		//EnableWindow(TRUE);
		if(ret != IDOK)
			return;
	}
	else
	{//use Nelson calculations...
		nelson.fpSet = fpSet;
		EnableWindow(FALSE);
		int ret = nelson.CalcValues(this, optionsSet.m_VarID, isHalfNelson);
		EnableWindow(TRUE);
		if(ret != 0)
			return;
	}
	//get fire data
	CLogitDialog ldlg;
	ldlg.Create(IDD_LOGITDIALOG);
	ldlg.ShowWindow(SW_SHOW);
	ldlg.messageString.SetWindowText("Preparing Analysis...");
	ldlg.Invalidate();
	ldlg.UpdateWindow();

	long nVals, nFD, nLFD, nMFD, nDiscards, nMadeUps;
	//int pcntSample = m_spinSample.GetPos();//(int)samplePercent.GetValueInteger();
	if(!isNelson)
		analysis->GetLogitArraySizesQuick(&optionsSet, 0, &nVals, &nFD, &nLFD, &nMFD, &nDiscards, &nMadeUps);
	else
	{//use Nelson calculations...
		nelson.GetLogitArraySizesQuick(&fires, &optionsSet, 0, &nVals, &nFD, &nLFD, &nMFD, &nDiscards, &nMadeUps);
	}
	double *fxVals, *fyVals,*lfxVals, *lfyVals, *mfxVals, *mfyVals;
	long lfdmfdSize = (optionsSet.m_Conditional == true && nFD > 0) ? nFD : nVals;
	fxVals	= new double[nVals];
	fyVals	= new double[nVals];
	lfxVals	= new double[lfdmfdSize];
	lfyVals	= new double[lfdmfdSize];
	mfxVals	= new double[lfdmfdSize];
	mfyVals	= new double[lfdmfdSize];
	long loc;
	for(loc = 0; loc < nVals; loc++)
		fxVals[loc] = fyVals[loc] = 0.0;
	for(loc = 0; loc < lfdmfdSize; loc++)
		lfxVals[loc] = lfyVals[loc] = mfxVals[loc] = mfyVals[loc] = 0.0;
	if(!isNelson)
		analysis->CreateLogitArraysQuick(&optionsSet, 0,
				fxVals, fyVals, lfxVals, lfyVals, mfxVals, mfyVals);
	else
	{
		nelson.CreateLogitArraysQuick(&optionsSet, 0,
				fxVals, fyVals, lfxVals, lfyVals, mfxVals, mfyVals);
	}
	//analysis.CreateLogitFiles(&fires, optionsSet.m_LFAcres, optionsSet.m_MFDFires, varName, conditional,
	//	&nVals, &nFD, &nLFD, &nMFD, &nDiscards, &nMadeUps);
	int maxLFperDay = 0;
	fires.MoveFirst();
	COleDateTime lastFireDay = fires.m_Discovery;
	int firesThisDay = 0;
	while(!fires.IsEOF())
	{
		if(lastFireDay.GetYear() != fires.m_Discovery.GetYear()
			|| lastFireDay.GetDayOfYear() != fires.m_Discovery.GetDayOfYear())
		{
			lastFireDay = fires.m_Discovery;
			firesThisDay = 0;
		}
		if(fires.m_TotalAcres >= optionsSet.m_LFAcres)
		{
			
			if(optionsSet.m_Human && fires.m_StatisticalCause != 1)
			{
			
				int testFlag = 1;
				if (fires.m_AgencyID == 1) // USFS
				     testFlag = (causeFlag >> (fires.m_StatisticalCause - 1)) & 1;
		
				if (testFlag == 1)
				   firesThisDay++;

			} else if(optionsSet.m_Lightning && fires.m_StatisticalCause == 1)
				firesThisDay++;
			else
				firesThisDay++;
		}
		maxLFperDay = max(maxLFperDay, firesThisDay);
		fires.MoveNext();
	}
	int *numLargeFirePerDay = NULL;
	if(maxLFperDay > 0)
	{
		numLargeFirePerDay = new int[maxLFperDay];
		for(int i = 0; i < maxLFperDay; i++)
			numLargeFirePerDay[i] = 0;
		fires.MoveFirst();
		firesThisDay = 0;
		lastFireDay = fires.m_Discovery;
		while(!fires.IsEOF())
		{
			if(lastFireDay.GetYear() != fires.m_Discovery.GetYear()
				|| lastFireDay.GetDayOfYear() != fires.m_Discovery.GetDayOfYear())
			{
				if(firesThisDay > 0)
					numLargeFirePerDay[firesThisDay - 1]++;
				lastFireDay = fires.m_Discovery;
				firesThisDay = 0;
			}
			if(fires.m_TotalAcres >= optionsSet.m_LFAcres)
			{
				if(optionsSet.m_Human && fires.m_StatisticalCause != 1)
				{
						int testFlag = 1;
						if (fires.m_AgencyID == 1) // USFS
							testFlag = (causeFlag >> (fires.m_StatisticalCause - 1)) & 1;
		
						if (testFlag == 1)
							firesThisDay++;
				}
				else if(optionsSet.m_Lightning && fires.m_StatisticalCause == 1)
					firesThisDay++;
				else
					firesThisDay++;
			}
			fires.MoveNext();
		}
	}
	fires.Close();
	//Sleep(500);
	//if(pcntSample != 100)
	//{
	//	CreateSubsampleLogitFiles(pcntSample, conditional, &nVals, &nFD, &nLFD, &nMFD);
	//	Sleep(500);
	//}
	double fd_coef[2], lfd_coef[2], mfd_coef[2];
	double fd_se[2], lfd_se[2], mfd_se[2];
	double fd_t[2], lfd_t[2], mfd_t[2];
	double fd_p[2], lfd_p[2], mfd_p[2];
	double fd_dev, fd_pdev, lfd_dev, lfd_pdev, mfd_dev, mfd_pdev;
	LogReg fd_logReg, lfd_logReg, mfd_logReg;
	if(regress)
	{
		ldlg.messageString.SetWindowText("Performing Logistic Regressions...");
		ldlg.Invalidate();
		ldlg.UpdateWindow();
		fd_logReg.DoLogReg(1, nVals, fyVals, fxVals, fd_coef, fd_se, fd_t, fd_p, &fd_dev, &fd_pdev);
		lfd_logReg.DoLogReg(1, lfdmfdSize, lfyVals, lfxVals, lfd_coef, lfd_se, lfd_t, lfd_p, &lfd_dev, &lfd_pdev);
		mfd_logReg.DoLogReg(1, lfdmfdSize, mfyVals, mfxVals, mfd_coef, mfd_se, mfd_t, mfd_p, &mfd_dev, &mfd_pdev);
		/*if (NULL != theApp.dllHandle)
		{
			// If the function address is valid, call the function.
			if (NULL != theApp.logReg)
			{
				//LPCTSTR myArtist = "Duchamp";
				ldlg.messageString.SetWindowText("Performing Logistic Regressions for Fire-Days");
				ldlg.Invalidate();
				ldlg.UpdateWindow();
				theApp.logReg("fd.dat", "fd.out");
				//Sleep(500);
				ldlg.messageString.SetWindowText("Performing Logistic Regressions for Large Fire-Days");
				ldlg.Invalidate();
				ldlg.UpdateWindow();
				theApp.logReg("lfd.dat", "lfd.out");
				//Sleep(500);
				ldlg.messageString.SetWindowText("Performing Logistic Regressions for Multi Fire-Days");
				ldlg.Invalidate();
				ldlg.UpdateWindow();
				theApp.logReg("mfd.dat", "mfd.out");
				//Sleep(500);
			}
		}
		else
			AfxMessageBox("Failed to load logit.dll. Hit OK to Crash Program");*/
	}
	//now need to read input files and output files from logit.exe
	ldlg.messageString.SetWindowText("Building Analysis Structures...");
	ldlg.Invalidate();
	ldlg.UpdateWindow();
	CFireStats *fStats = NULL, *cfStats = NULL;
	if(regress)
	{
		fStats = new CFireStats(climSet.m_ShortName, nVals, nFD, nLFD, nMFD, (climSet.m_OptionType == 1) ? true : false, climSet.m_VarID);
		if (!climSet.IsFieldNull(&climSet.m_FilterValue))
			fStats->filterValue = climSet.m_FilterValue;
		if (climSet.m_cp2 < climSet.m_CriticalPercentile)
			fStats->filterDirection = FILTER_GREATERTHAN_EQUAL;
		fStats->shortName = varName;
		fStats->nMadeUps = nMadeUps;
		fStats->binSize = climSet.m_BinSize;
		fStats->humanCause = causeFlag;

		//fStats->staStr.Format("%s",
		//	fpSet->m_SIG_Station.GetLength() > 6 ? fpSet->m_SIG_Station
		//	: "Station: " + fpSet->m_SIG_Station);
		//fStats->staStr.Format("%s",
		//	fpSet->m_SIG_Station.GetLength() > 6 ? fpSet->m_SIG_Station
		//	: fpSet->m_SIG_Station + "-" );
		fStats->lfAcres = optionsSet.m_LFAcres;
		fStats->mfdFires = optionsSet.m_MFDFires;
		fStats->cause = cause;
		fStats->nDiscards = nDiscards;
		fStats->start.SetDate(fpSet->m_StartYear, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
		fStats->end.SetDate(fpSet->m_EndYear, MonthChtoInt(fpSet->m_EndMonth), fpSet->m_EndDay);
		//fStats->Build(fpSet, regress, conditional);
		fStats->BuildQuick(fpSet, regress, conditional,
						nVals, lfdmfdSize,
						fxVals, fyVals, lfxVals, lfyVals, mfxVals, mfyVals,
						&fd_logReg, &lfd_logReg, &mfd_logReg);
	}
	if(cumu)
	{
		cfStats = new CFireStats(climSet.m_ShortName, nVals, nFD, nLFD, nMFD, (climSet.m_OptionType == 1) ? true : false, climSet.m_VarID);
		if (!climSet.IsFieldNull(&climSet.m_FilterValue))
			cfStats->filterValue = climSet.m_FilterValue;
		if (climSet.m_cp2 < climSet.m_CriticalPercentile)
			cfStats->filterDirection = FILTER_GREATERTHAN_EQUAL;
		cfStats->humanCause = causeFlag;
		cfStats->shortName = varName;
		cfStats->nMadeUps = nMadeUps;
		cfStats->binSize = climSet.m_BinSize;
		cfStats->staStr.Format("%s",
			fpSet->m_SIG_Station.GetLength() > 6 ? fpSet->m_SIG_Station
			: "Station: " + fpSet->m_SIG_Station);
		cfStats->lfAcres = optionsSet.m_LFAcres;
		cfStats->mfdFires = optionsSet.m_MFDFires;
		cfStats->cause = cause;
		cfStats->start.SetDate(fpSet->m_StartYear, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
		cfStats->end.SetDate(fpSet->m_EndYear, MonthChtoInt(fpSet->m_EndMonth), fpSet->m_EndDay);
		//cfStats->Build(fpSet, false, FALSE);
		cfStats->BuildQuick(fpSet, false, FALSE,
						nVals, lfdmfdSize,
						fxVals, fyVals, lfxVals, lfyVals, mfxVals, mfyVals,
						NULL, NULL, NULL);
	}
	delete[] fxVals;
	delete[] fyVals;
	delete[] lfxVals;
	delete[] lfyVals;
	delete[] mfxVals;
	delete[] mfyVals;
	climSet.Close();
	ldlg.DestroyWindow();
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	if(regress)
	{
		//create report
		/*char *fName = GetTempFileName();
		FILE *stream = fopen(fName, "wt");
		CString str, temp;
		fprintf(stream,
			"FireFamily Plus Fires Analysis\n");
		fprintf(stream, "%s\nVariable: %s\n\n", fStats->staStr,
			fStats->varName);
		fprintf(stream, "Model: %s\n", fStats->graphModelString);
		fprintf(stream, " Time Frame: %d/%d - %d/%d\n",
			fStats->start.GetMonth(),
			fStats->start.GetDay(),
			fStats->end.GetMonth(),
			fStats->end.GetDay());
		fprintf(stream, " Data Years: %d - %d\n",
			fStats->start.GetYear(),
			fStats->end.GetYear());
		switch(fStats->cause)
		{
		case 0://all fires
			fprintf(stream, " Cause = All\n");
			break;
		case 1://lightning
			fprintf(stream, " Cause = Lightning\n");
			break;
		case 2://human
			fprintf(stream, " Cause = Human\n");
			break;
		}
		fprintf(stream,
			" Large Fire Day (LFD)= %d acres\n"
			" Multiple Fire Day (MFD)= %d fires\n\n",
			fStats->lfAcres, fStats->mfdFires);
		int round = TRUE;
		if(strncmp(fStats->staStr, "SIG", 3) != 0)
			//a single station
			fprintf(stream, " %s - %s", fStats->staStr, fStats->strings[0]);
		else
		{
			//round = FALSE;
			fprintf(stream, " Stations in %s:\n", fStats->staStr);
			for(int i = 0; i < fStats->nStrings; i++)
				fprintf(stream, " %s", fStats->strings[i]);
		}
		if(fStats->varID == 9)//precip
			round = FALSE;
		fprintf(stream,
			"\n**** Created data for %ld records with missing values. ****\n"
			    "  (Maximum of %d consecutive missing days for replacement)\n", nMadeUps, theApp.maxMissingWx);
		fprintf(stream, "%ld fires discarded due to no/missing weather. \n", fStats->nDiscards);
		if(pcntSample < 100)
			fprintf(stream, "Subsampled data at %d percent. \r\n", pcntSample);
		//now dump goodness of fit reports
		fStats->StatsReport(stream, FD, conditional, round);
		fStats->StatsReport(stream, LFD, conditional, round);
		fStats->StatsReport(stream, MFD, conditional, round);
		fprintf(stream, "\n%s\n", analysis.AddDateStamp());
		fclose(stream);*/
		CString tFileName = fStats->LogRegReport(isNelson, isHalfNelson);
		while(curTemplatePos != NULL)
		{
			CDocTemplate* curTemplate =
				theApp.GetNextDocTemplate(curTemplatePos);
			CString str;
			curTemplate->GetDocString(str, CDocTemplate::docName);
			if(str == _T("FireStats"))
			{
				CRichFireDoc *fDoc = (CRichFireDoc *)curTemplate->OpenDocumentFile(tFileName);
				fDoc->SetFires(fStats);
				fDoc->SetLargeFiresPerDay(maxLFperDay, numLargeFirePerDay);
				fDoc->isTemp = true;
				//fDoc->conditional = conditional;
				fDoc->fires = fStats;
				fDoc->fpSet = fpSet;
				if(nDiscards > 0)
				{
					fDoc->hasDiscards =  true;
					char *disc = GetTempFileName();
					fDoc->discardFileName = disc;
					rename("discards.dat", disc);
					free(disc);
				}
				else
				{
					fDoc->hasDiscards =  false;
					fDoc->discardFileName = "";
				}
				CString tStr;
				tStr.Format("%s Fires Probability Analysis", optionsSet.m_SIG_Station);
				fDoc->SetTitle(tStr);
				//fDoc->SetViewFires();
			}
		}
		//free(fName);
	}
	if(cumu)
	{
		curTemplatePos = theApp.GetFirstDocTemplatePosition();
		while(curTemplatePos != NULL)
		{
			CDocTemplate* curTemplate =
				theApp.GetNextDocTemplate(curTemplatePos);
			CString str;
			curTemplate->GetDocString(str, CDocTemplate::docName);
			if(str == _T("Cumulative"))
			{
				CFireStatDoc *fDoc;
				fDoc = (CFireStatDoc *)curTemplate->CreateNewDocument();
				//fDoc->fires = cfStats;
				fDoc->SetFires(cfStats);
			//	fDoc->pDB = fpSet->m_pDatabase;
				CString tStr;
				tStr.Format("%s Cumulative Fires Analysis", optionsSet.m_SIG_Station);
				fDoc->SetTitle(tStr);
				CCumulativeFrame* pFrame = (CCumulativeFrame*)curTemplate->CreateNewFrame(fDoc, NULL);
				fDoc->SetFires(cfStats);
				curTemplate->InitialUpdateFrame(pFrame, fDoc);
			}
		}
	}
	if(analysis)
		delete analysis;
	if(numLargeFirePerDay)
		delete[] numLargeFirePerDay;
	CDialog::OnOK();
}

BOOL CFireAnalysisDialog::OnInitDialog()
{
	if(optionsSet.m_Conditional == TRUE)
		conditional = TRUE;
	else
		conditional = FALSE;
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	if(!fpSet)
		return TRUE;
	CClimateSet climSet(fpSet->m_pDatabase);
	climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
	climSet.setVarSortOrder(theApp.varSortOrder);
	climSet.Open();
	while(!climSet.IsEOF())
	{
		if(isNelson && (climSet.m_OptionType != 1 || climSet.m_VarID > 20 || climSet.m_VarID == 15))
		{
			climSet.MoveNext();
			continue;
		}
		int loc = varIDs.AddString(climSet.m_Variable_Name);
		if(loc >= 0)
		{
			varIDs.SetItemData(loc, climSet.m_VarID);
			if(climSet.m_VarID == optionsSet.m_VarID)
				varIDs.SetCurSel(loc);
		}
		climSet.MoveNext();
	}
	climSet.Close();
	m_spinAcres.SetRange(1, 30000);
	m_spinFires.SetRange(1, 30000);
	m_spinAcres.SetPos(optionsSet.m_LFAcres);
	m_spinFires.SetPos(optionsSet.m_MFDFires);
	//m_spinSample.SetRange(1, 100);
	//m_spinSample.SetPos(100);
	if(optionsSet.m_Cumulative)
		cumulative.SetCheck(true);
	else if(optionsSet.m_Probability)
	{
		CButton *button = (CButton *)GetDlgItem(IDC_RADIO6);
		button->SetCheck(true);
	}
	else //both
	{
		CButton *button = (CButton *)GetDlgItem(IDC_RADIO7);
		button->SetCheck(true);
	}
	if(optionsSet.m_Lightning)
	{
		lightning.SetCheck(true);
		moreCauses.EnableWindow(false);
	}
	else if(optionsSet.m_Human)
	{
		CButton *button = (CButton *)GetDlgItem(IDC_RADIO3);
		button->SetCheck(true);
		moreCauses.EnableWindow(true);
	}
	else //all causes
	{
		CButton *button = (CButton *)GetDlgItem(IDC_RADIO4);
		button->SetCheck(true);
		moreCauses.EnableWindow(false);
	}
	causeFlag = optionsSet.m_CauseFlag;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFireAnalysisDialog::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::PostNcDestroy();
	optionsSet.Close();
}

void CFireAnalysisDialog::OnLightning()
{
	
	UpdateData();
	int newLightning = lightning.GetCheck();
	if (newLightning == 1)
		moreCauses.EnableWindow(false);
}

void CFireAnalysisDialog::OnHuman()
{
	moreCauses.EnableWindow(true);

}

void CFireAnalysisDialog::OnAll()
{
	moreCauses.EnableWindow(false);

}

void CFireAnalysisDialog::OnCauseFilter()
{
	

	CFireCauseFilterDialog ffd(this);
	
	ffd.causeFlag = causeFlag;


	if(ffd.DoModal() != IDOK)
			return;
	
	causeFlag =  ffd.causeFlag;


}

void CFireAnalysisDialog::OnConditional()
{
	UpdateData();
	//cumulative.SetCheck(!conditional);
	///cumulative.EnableWindow(!conditional);
	//CButton *button = (CButton *)GetDlgItem(IDC_RADIO7);
//	button->EnableWindow(!conditional);
//	if(conditional)
//	{
//		cumulative.SetCheck(FALSE);
//		button->SetCheck(FALSE);
//		button = (CButton *)GetDlgItem(IDC_RADIO6);
//		button->SetCheck(TRUE);

//	}
	//	cumulative.SetCheck(FALSE);
	//	cumulative.EnableWindow(FALSE);
		//(CButton *)GetDlgItem(IDC_RADIO6)
//	}
}

void CFireAnalysisDialog::OnVarSort()
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
	climSet.MoveFirst();

	varIDs.ResetContent();

	while(!climSet.IsEOF())
	{
		if(isNelson && (climSet.m_OptionType != 1 || climSet.m_VarID > 20 || climSet.m_VarID == 15))
		{
			climSet.MoveNext();
			continue;
		}

		int loc = varIDs.AddString(climSet.m_Variable_Name);
		if(loc >= 0)
		{
			varIDs.SetItemData(loc, climSet.m_VarID);
			if(climSet.m_VarID == optionsSet.m_VarID)
				varIDs.SetCurSel(loc);
		}
		climSet.MoveNext();
	}
	climSet.Close();

	CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));
}
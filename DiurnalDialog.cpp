// DiurnalDialog.cpp : implementation file
//
 
#include "stdafx.h"
#include "fireplus.h"
#include <afxrich.h>
#include "richdoc.h"
#include "DiurnalDoc.h"
#include "DiurnalFrame.h"
#include "DiurnalDialog.h"
#include "ClimateSet.h"
//#include "nfdr32.h"
#include "canada.h"
#include "stationInSIGSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "splitterframe.h"
#include "FireSumSet.h"
#include "ClimAnalysis.h"
#include "CustomInitSet.h"
#include "ReportOptionsSet.h"
#include "FFPOptsSet.h"
#include "LFISet.h"
#include "LFIengine.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int daysInMonth[];
extern CFireplusApp theApp;
extern char *abbrevs[];
extern char *varFormats[];
extern int MonthChtoInt(CString chMonth);

Day::Day()
{
}

Day::~Day()
{
}

void Day::AddValue(double val, COleDateTime obsTime)
{
	int h = obsTime.GetHour(), m = obsTime.GetMinute();
	if(m >= 30)
		h++;
	if(h >= 24)
		h = 0;
	hours[h].Accumulate(val);
}

/*void Day::AddValue(double val, CTime obsTime)
{
	int h = obsTime.GetHour(), m = obsTime.GetMinute();
	if(m >= 30)
		h++;
	if(h >= 24)
		h = 0;
	hours[h].Accumulate(val);
}*/

void Day::CleanStorage(bool SetToMode)
{
	for(int i = 0; i < 24; i++)
	{
		hours[i].Complete();
		if(!SetToMode)
			//hours[i].SetMeanToMode();
		hours[i].CleanStorage();
	}
}

CDiurnalAnalysis::CDiurnalAnalysis()
{
	//recCounter = NULL;
	count = 0;
	doListing = false;
	days = NULL;
	varIDs = NULL;
	nVarIDs = 0;
	staName = "";
	modelStr = "";
	stamp = "";
}

CDiurnalAnalysis::~CDiurnalAnalysis()
{
	if(days)
	{
		for(int i = 0; i < MAXVARIDS; i++)
		{
			if(days[i])
				delete[] days[i];
		}
		delete[] days;
	}
	if(varIDs)
		delete[] varIDs;
}



// new 2015
bool CDiurnalAnalysis::HasHLOutput(CFireplusSet *fpSet)
{
	try
	{
		CRecordset trecs(fpSet->m_pDatabase);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM HL_Output"))
		{
			trecs.Close();
			return true;
		}
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	
	return false;
}



// new 2015
//
void CDiurnalAnalysis::CreateHLOutput(CFireplusSet *fpSet)
{

	try
	{
		CString strSql =
			"CREATE TABLE [HL_Output] ("
            "[ID] INTEGER,"
			"[ObsDate] VARCHAR(30) NOT NULL,"
			"CONSTRAINT IDConstraint PRIMARY KEY ([ID]))";
		fpSet->m_pDatabase->ExecuteSQL(strSql);
	}
	catch(CDBException* e)
	{
		DisplayDBException(e);
		e->Delete();
	}
}

int CDiurnalAnalysis::Analyze(CWnd *_caller, CFireplusSet *fpSet, int nVars, int *_varIDs)
{
	int ret = 0;
	
	/*recCounter = new CCancelDialog(_caller);
	ret = recCounter->Create(IDD_CANCELDIALOG);

      if (!ret)   //Create failed.
      {
         
		 recCounter = 0;
      } else
	  {
	     recCounter->Reset();
         recCounter->ShowWindow(SW_SHOW);
	     recCounter->SetTitle("Processing...");
	
	  }*/
	//recCounter->Create();
//	if(inverted)
//		recCounter->SetWindowText("Processing Auxiliary Years Data");
	CWaitCursor wait;
	int nVarIDs = nVars;
	varIDs = _varIDs;
	//model use flags - optimize to only calculate models used
	//killPeriods = true;
	//fpSet = _fpSet;
	useCanadian = false;
	useNFDRS = false;
	useLFI = false;
	count = 0;
	int sYear = fpSet->m_StartYear, eYear = fpSet->m_EndYear;
	//int pLen = 1;//atoi(fpSet->m_PeriodLength);
	int v;
	for(v = 0; v < nVarIDs; v++)
	{
		//create data storage structure
		if(OptionFromVarID(varIDs[v]) == 1)
			useNFDRS = true;
		if(OptionFromVarID(varIDs[v]) == 3)
			useCanadian = true;
		if(varIDs[v] == 39 || varIDs[v] == 42 || varIDs[v] == 43)
			useLFI = true;
	}

	//DATA STORAGE CREATED.
	//now process selected station(s)
	CSIGStationSet staSet(fpSet->m_pDatabase);
	staSet.Open();
	CString query = "", temp, staQuery;
	//DETERMINE DAY OF YEAR ORDERING
	fpSet->BuildBaseQuery(query, "ObsDate", false, false);
	//if(query.GetLength() > 0)
	//	query += "AND ";
	//query += "[ObsType] = 'R'";
	days = new Day *[MAXVARIDS];
	for(v = 0; v < MAXVARIDS; v++)
		days[v] = NULL;
	for(v = 0; v < nVarIDs; v++)
		days[varIDs[v]] = new Day[366];
	//days = new Day *[366];
	//for(v = 0; v < 366; v++)
	//	days[v] = new Day[MAXVARIDS];
	if(fpSet->m_SIG_Station.GetLength() <= 6)//a single station
	{
		//_fpreset();
		if(query.GetLength() > 0)
			temp.Format(" and [StationID] = '%6.6s'", fpSet->m_SIG_Station);
		else
			temp.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
		CWxSet wxSet(fpSet->m_pDatabase);
		wxSet.m_strSort = _T("[ObsDate]");
		wxSet.m_strFilter.Format("(%s) %s", query, temp);
		wxSet.Open();
		if(wxSet.IsBOF() || wxSet.IsEOF())
		{
			CString strMsg;
			/*if(recCounter)
			{
				recCounter->DestroyWindow();
				recCounter = NULL;
			}*/
			strMsg.Format(
				_T("ERROR: No matching weather observation records.\n Redefine Working Set Query. \n\n"));
			AfxMessageBox(strMsg);
			return 1;
		}
		//set up SIGStationSet
		staQuery.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
		staSet.m_strFilter = _T(staQuery);
		staSet.Requery();
		//m_strStationName = staSet.m_Name;
		staName.Format("%s - %s", fpSet->m_SIG_Station, staSet.m_Name);
		ret = AnalyzeStation(CString(fpSet->m_SIG_Station + " - " + staSet.m_Name), staSet, wxSet);
		if(ret == 0)
		{
			modelStr = GetModelString(fpSet, &staSet);
			stamp = AddDateStamp();
		}
		wxSet.Close();
	}
	else
	//is a SIG - multiple stations - no diurnal analysis available
		ret = 2;

	if(_caller)
		_caller->EnableWindow(TRUE);
	/*if(recCounter)
	{
		recCounter->DestroyWindow();
		recCounter = NULL;
	}*/
	return ret;
}

int CDiurnalAnalysis::HourlyListing(CWnd *_caller, CFireplusSet *fpSet, int nVars, int *_varIDs, ListingOptions *listOpts, int createTable)
{
	if(fpSet->m_SIG_Station.GetLength() > 6)//a SIG
	{
		AfxMessageBox("ERROR: Hourly listings not available for SIGS.\n\n");
		if(_varIDs)
			delete[] _varIDs;
		if(_caller)
			_caller->EnableWindow(TRUE);
		return 1;
	}
	CSIGStationSet staSet(fpSet->m_pDatabase);
	staSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
	staSet.Open();
	//LFI goodies
	CLFIengine lfi(fpSet->m_pDatabase), lfiHerb(fpSet->m_pDatabase, HerbParams, staSet.m_StationID, staSet.m_HerbAnnual),
		lfiWoody(fpSet->m_pDatabase, WoodyParams, staSet.m_StationID);

	nVarIDs = nVars;
	varIDs = _varIDs;

	CString tableMemo = "";
	int numRecs = 0;

//	recCounter = new CCancelDialog();
/*	recCounter = new CCancelDialog(_caller);
	int ret = recCounter->Create(IDD_CANCELDIALOG);

      if (!ret)   //Create failed.
      {
         
		 recCounter = 0;
      } else
	  {
	     recCounter->Reset();
         recCounter->ShowWindow(SW_SHOW);
	     recCounter->SetTitle("Processing...");
	
	  }
	//recCounter->Create();*/
	CWaitCursor wait;
	useCanadian = false;
	useNFDRS = false;
	useLFI = false;
	count = 0;
	for(int v = 0; v < nVarIDs; v++)
	{
		//create data storage structure
		if(OptionFromVarID(varIDs[v]) == 1)
			useNFDRS = true;
		if(OptionFromVarID(varIDs[v]) == 3)
			useCanadian = true;
		if(varIDs[v] == 39 || varIDs[v] == 42 || varIDs[v] == 43)
			useLFI = true;
	}
	CString query = "", temp, staQuery;
	//fpSet->BuildBaseQuery(query, "ObsDate", false, false);
	query.Format("(Year([ObsDate]) >= %d and Year([ObsDate]) <= %d)", fpSet->m_StartYear, fpSet->m_EndYear);

	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	//output header
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();
	reportSet.MoveFirst();
	int Delimiter = reportSet.m_Delimiter;
	CString delim;
	switch (Delimiter){
		case 1:
			delim = "\t";
			break;
		case 2:
			delim = ",";
			break;
		case 3:
		case 4:
			delim = "|";
			break;
		default:
			delim = " ";
			break;
	}
	if(listOpts->reportHeader)
	{
		fprintf(stream, "FireFamily Plus Hourly Listing Report\n");
		fprintf(stream, "Hourly Listing of Selected Values\n");
		CString dStr;

		if(listOpts->dateStamp)
			fprintf(stream, "\n%s\n", AddDateStamp());

		COleDateTime dateTime = COleDateTime::GetCurrentTime();

		dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
		fprintf(stream,"   printed on: %s\n",  dStr);
		fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());
		

		fprintf(stream,"\n");
		CString headerStr;

		// add report headers (as needed)
		reportSet.listAll(&headerStr,fpSet);
		fprintf(stream,"%s",headerStr);
		/*if(useNFDRS)
			fprintf(stream, "Station: %s - %-20.20s Model: %s           %d - %d\n\n",
			fpSet->m_SIG_Station, staSet.m_Name,
			GetModelString(fpSet, &staSet), fpSet->m_StartYear, fpSet->m_EndYear);
		else
			fprintf(stream, "Station: %s - %-20.20s                         %d - %d\n\n",
			fpSet->m_SIG_Station, staSet.m_Name, fpSet->m_StartYear, fpSet->m_EndYear);*/
	}
	//header
	if(listOpts->columnHeader)
	{
				switch(listOpts->dt)
				{
				case Full:
					fprintf(stream, "DATE      %s", delim);
					break;
				case Packed:
					//fprintf(stream, "DATE    %s", delim);
				case Reverse:
					fprintf(stream, "DATE    %s", delim);
					break;
				case MD:
				case MDPacked:
					fprintf(stream, "DATE %s", delim);
					break;
				}
		//fprintf(stream, "DATE%s", delim);
		if(listOpts->tt == FullTime)
			fprintf(stream, "Time %s", delim);
		else if(listOpts->tt == PackedTime)
			fprintf(stream, "Time%s", delim);
		/*fprintf(stream, "DATE%s", delim);
		if(listOpts->tt == FullTime || listOpts->tt == PackedTime)
			fprintf(stream, "Time%s", delim);*/
		int i;
		for(i = 0; i < nVarIDs; i++)
			fprintf(stream, "%5.5s%s", abbrevs[varIDs[i]], delim);
		fprintf(stream, "\n------------");
		if(listOpts->tt != NoTime)
			fprintf(stream, "------");
		for(i = 0; i < nVarIDs; i++)
			fprintf(stream, "------");
		fprintf(stream, "\n");
	}

	// new HL_Export table,  2015
	if (createTable)
	{
		char tempStr[250];

         // drop table if it exists
		if (HasHLOutput(fpSet))
		{

			try
			{
				CString strSql = "DROP TABLE [HL_Output]";
				fpSet->m_pDatabase->ExecuteSQL(strSql);
			}
			catch(CDBException* e)
			{
				e->Delete();
			}

		}
		CreateHLOutput(fpSet);
		
		try
			{
				CString strSql = "ALTER TABLE [HL_Output] ADD [StationID] VARCHAR(20)";
				fpSet->m_pDatabase->ExecuteSQL(strSql);
			}
			catch(CDBException* e)
			{
				e->Delete();
			}
		

		// add additional fields as necessary
		int loc = 0;
		int i;
		CString strSql("");
		

		for(i = 0; i < nVarIDs; i++)
		{
			
			char fieldName[20];

		

			if(varIDs[i] < STATICVARIDS)
			{
				sprintf(fieldName, "%5.5s", abbrevs[varIDs[i]]);
		
			}
			try
			{
				CString trimmed = CString(fieldName).Trim();
				strSql = "ALTER TABLE [HL_Output] ADD [" + trimmed + "] DOUBLE";
				fpSet->m_pDatabase->ExecuteSQL(strSql);
			}
			catch(CDBException* e)
			{
				e->Delete();
			}
		}
		if(listOpts->fires)
                try
				{
					strSql = "ALTER TABLE [HL_Output] ADD [numFires] INTEGER";
					fpSet->m_pDatabase->ExecuteSQL(strSql);
				}
				catch(CDBException* e)
				{
					e->Delete();
				}
		if(listOpts->largeFires)
                try
				{
					strSql = "ALTER TABLE [HL_Output] ADD [numLargeFires] INTEGER";
					fpSet->m_pDatabase->ExecuteSQL(strSql);
				}
				catch(CDBException* e)
				{
					e->Delete();
				}
			
		 if(listOpts->numPerClass)
				try
				{
					strSql = "ALTER TABLE [HL_Output] ADD [numA] INTEGER";
					fpSet->m_pDatabase->ExecuteSQL(strSql);
					strSql = "ALTER TABLE [HL_Output] ADD [numB] INTEGER";
					fpSet->m_pDatabase->ExecuteSQL(strSql);
					strSql = "ALTER TABLE [HL_Output] ADD [numC] INTEGER";
					fpSet->m_pDatabase->ExecuteSQL(strSql);
					strSql = "ALTER TABLE [HL_Output] ADD [numD] INTEGER";
					fpSet->m_pDatabase->ExecuteSQL(strSql);
					strSql = "ALTER TABLE [HL_Output] ADD [numE] INTEGER";
					fpSet->m_pDatabase->ExecuteSQL(strSql);
					strSql = "ALTER TABLE [HL_Output] ADD [numF] INTEGER";
					fpSet->m_pDatabase->ExecuteSQL(strSql);
					strSql = "ALTER TABLE [HL_Output] ADD [numG] INTEGER";
					fpSet->m_pDatabase->ExecuteSQL(strSql);
				}
				catch(CDBException* e)
				{
					e->Delete();
				}
		if(listOpts->acres)
                try
				{
					strSql = "ALTER TABLE [HL_Output] ADD [numAcres] REAL";
					fpSet->m_pDatabase->ExecuteSQL(strSql);
				}
				catch(CDBException* e)
				{
					e->Delete();
				}

        // memo field, populated only for the first record
	    try
				{
					strSql = "ALTER TABLE [HL_Output] ADD [memo] LONGTEXT";
					fpSet->m_pDatabase->ExecuteSQL(strSql);
				}
				catch(CDBException* e)
				{
					e->Delete();
				}

	
		

		tableMemo = "FireFamily Plus Hourly Listing Report\n";
		tableMemo += "Hourly Listing of Selected Values\n";	
		tableMemo += "\nSIG/Station: ";
		tableMemo += fpSet->m_SIG_Station;
		sprintf(tempStr, "\n\n%s\n", AddDateStamp());
		tableMemo += tempStr;

		CString dStr;

		COleDateTime dateTime = COleDateTime::GetCurrentTime();

		dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	
		sprintf(tempStr,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
		tableMemo += tempStr;
		sprintf(tempStr,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());
		tableMemo += tempStr;

		tableMemo += "\n";
		CString headerStr;

		// add report headers (as needed)
		reportSet.listAll(&headerStr,fpSet);
		tableMemo += headerStr;

		// end added for batch
		/*
		if(listOpts->fires || listOpts->largeFires)
		{//output fire cause in header
			CString causeStr = "All Fires";
			if(listOpts->fCause == 1)
				causeStr = "Lightning";
			else if(listOpts->fCause > 1)
				causeStr = "Human";
			sprintf(tempStr, "Fire Cause Filter: %s\n", causeStr);
			tableMemo += tempStr;
			if(listOpts->largeFires)
			{
				sprintf(tempStr, "Large Fire: %d Acres\n", listOpts->lfAcres);
				tableMemo += tempStr;
			}

		}
	
	*/
		


	}
	// end of HL_Output



	//now read calc and dump data....
//	NFDRCalc nfdrs;
	int sKBDI = 0;
	if(!staSet.IsFieldNull(&staSet.m_StartKBDI))
		sKBDI = staSet.m_StartKBDI;


	//NFDR2016Calc *nfdr2016;
	double pptAcc = 0;

	if(useNFDRS || useLFI)
	{
		int lookGreen, lookFreeze;
		lookGreen = DayOfYear(staSet.m_GreenJulian);
		lookFreeze = DayOfYear(staSet.m_FreezeJulian);
		if (isNFDRS2016(staSet.m_NFDRSFM[0]))
		{ // new NFDRS2016 calc
			//nfdr2016 = new NFDR2016Calc((int)staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true);
			//theApp.m_NFDRS2016.Init((int)staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true, lfiHerb.GetHerbMaxGSI(), lfiHerb.GetHerbGreenup());
			theApp.m_NFDRS2016.Init(staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true, staSet.m_HerbAnnual, staSet.m_KBDIThreshold);
			theApp.m_NFDRS2016.iSetFuelModel(staSet.m_NFDRSFM[0]);
			theApp.m_NFDRS2016.SetGSIParams(lfi.GetHerbMaxGSI(), lfi.GetHerbGreenup(), lfi.GetTminMin(), lfi.GetTminMax(), lfi.GetVPDMin(), lfi.GetVPDMax(), 
				lfi.GetDaylenMin(), lfi.GetDaylenMax(), lfi.GetMAPeriod(), lfi.IsUsingVPDAvg(), lfi.GetNumPrecipDays(), lfi.GetRTPcpMin(), lfi.GetRTPcpMax());
			theApp.m_NFDRS2016.SetHerbGSIparams(lfiHerb.GetHerbMaxGSI(), lfiHerb.GetHerbGreenup(), lfiHerb.GetTminMin(), lfiHerb.GetTminMax(), 
				lfiHerb.GetVPDMin(), lfiHerb.GetVPDMax(), lfiHerb.GetDaylenMin(), lfiHerb.GetDaylenMax(), lfiHerb.GetMAPeriod(), lfiHerb.IsUsingVPDAvg(), 
				lfiHerb.GetNumPrecipDays(), lfiHerb.GetRTPcpMin(), lfiHerb.GetRTPcpMax());
			theApp.m_NFDRS2016.SetWoodyGSIparams(lfiWoody.GetWoodyMaxGSI(), lfiWoody.GetWoodyGreenup(), lfiWoody.GetTminMin(), lfiWoody.GetTminMax(), 
				lfiWoody.GetVPDMin(), lfiWoody.GetVPDMax(), lfiWoody.GetDaylenMin(), lfiWoody.GetDaylenMax(), lfiWoody.GetMAPeriod(), lfiWoody.IsUsingVPDAvg(), 
				lfiWoody.GetNumPrecipDays(), lfiWoody.GetRTPcpMin(), lfiWoody.GetRTPcpMax());
			theApp.m_NFDRS2016.SetStartKBDI(staSet.m_StartKBDI);
			theApp.m_NFDRS2016.SetSCMax(staSet.GetSCM(staSet.m_NFDRSFM[0]));
			theApp.m_NFDRS2016.SetMxdHumid(staSet.GetMxHumid(staSet.m_NFDRSFM[0]));
		}
		if (!isNFDRS2016(staSet.m_NFDRSFM[0]))
		{   // "old" NFDRS calc
			theApp.m_NFDRS.iInitialize(staSet.m_Use88 ? 88 : 78,
				staSet.m_ClimateCls, lookFreeze, lookGreen,
				(int)(staSet.m_LatDegrees), staSet.m_HerbAnnual, staSet.m_Deciduous, staSet.m_FM1Eq10,
				staSet.m_Start1000 - 5.0, staSet.m_Start1000, sKBDI, staSet.m_AvgPrecip);
			theApp.m_NFDRS.iSetFuelModel(staSet.m_NFDRSFM[0], 0);
			if (!staSet.IsFieldNull(&staSet.m_UseDormant) && !staSet.IsFieldNull(&staSet.m_DormantJulian))
			{
				if (staSet.m_UseDormant && staSet.m_DormantJulian.GetStatus() == COleDateTime::valid)
					theApp.m_NFDRS.iSetJulianShrubDormant(DayOfYear(staSet.m_DormantJulian));
			}
		}
	}
	bool bUseStick = true;
	if(!staSet.IsFieldNull(&staSet.m_UseStick))
		bUseStick = staSet.m_UseStick;
	//create Canadian model
	Canadian canadian;
	if(useCanadian)
		canadian.Initialize();

	CWxSet wxSet(fpSet->m_pDatabase);
	wxSet.m_strSort = _T("[ObsDate]");
	if(query.GetLength() > 0)
	{
		temp.Format(" and [StationID] = '%6.6s'", fpSet->m_SIG_Station);
		wxSet.m_strFilter.Format("(%s) %s", query, temp);
	}
	else
	{
		temp.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
		wxSet.m_strFilter.Format("%s",temp);
	}
	wxSet.Open();
	
	

	if(wxSet.IsBOF() || wxSet.IsEOF())
	{
		/*if(recCounter)
		{
			recCounter->DestroyWindow();
			recCounter = NULL;
		}*/
		CString strMsg;
		strMsg.Format(
			_T("ERROR: No matching weather observation records.\n Redefine Working Set Query. \n\n"));
		AfxMessageBox(strMsg);
		if(_caller)
			_caller->EnableWindow(TRUE);
		fclose(stream);
		unlink(fName);
		free(fName);
		return 2;
	}
	int iSC, iFIL, iBI,
		iIC, iStage, iKBDI, iRainEvent, init = 1, iDeclareGreenUp = 0, iDeclareFreeze = 0;//,
	double f1=0.0, f10=0.0, f100=0.0, f1000=0.0, fHerb=0.0, fWood=0.0, fGren=0.0, fX1000=0.0, fROS=0.0, fERC=0.0, fFL=0.0,
		ffmc=0.0, dmc=0.0, dc=0.0, bui=0.0, isi=0.0, fwi=0.0, dsr=0.0, lfiVal=0.0, lfiHerbVal=0.0, lfiWoodyVal=0.0;
	COleDateTime wxDay;//used to skip duplicates
	COleDateTime yesterday;//used to skip duplicates

	double fSC, fFIL, fBI, fIC;

	// 2016: we are now calculating 100-H, 1000-H, KBDI, and LFI  daily, not hourly
	double d100 = 0.0, 
		d1000 = 0.0,
		dHerb = 0.0,
		dWoody = 0.0,
		dLFI = 0.0,
		dLFIHerb = 0.0,
		dLFIWoody = 0.0;

	int dKBDI = 0;

	int nRec = 0;

	long regObsHour = staSet.m_RegSchdObs;

	if(!wxSet.IsBOF())
	{
		//working variables
		int tmp, tmpMax, tmpMin, rh, rhMax, rhMin, pptDur, sow, greenHerb, greenShrub,
			season, windSpd, windDir, slopeClass, wetFlag, snowFlag;
		double pptAmt, omc10;
		while(!wxSet.IsEOF())// && (!recCounter || (recCounter && !recCounter->Abort())))
		{
			if(((useNFDRS || useCanadian) && (wxSet.IsFieldNull(&wxSet.m_Temp) || wxSet.IsFieldNull(&wxSet.m_RH))
				|| wxSet.IsFieldNull(&wxSet.m_ObsDate)))
			//if(wxSet.IsFieldNull(&wxSet.m_Temp)
			//	|| wxSet.IsFieldNull(&wxSet.m_RH)
			//	|| wxSet.IsFieldNull(&wxSet.m_ObsDate))
			{
				wxSet.MoveNext();
				continue;
			}
			if(!init && yesterday >= wxSet.m_ObsDate)
			{
				wxSet.MoveNext();
				continue;
			}
			wxDay.SetDateTime(wxSet.m_ObsDate.GetYear(), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay(), wxSet.m_ObsDate.GetHour(),
				wxSet.m_ObsDate.GetMinute(), wxSet.m_ObsDate.GetSecond());
			// new custom init check    12/2006
			// if this is first record for new year, look for inits
			if (yesterday.GetYear() != wxSet.m_ObsDate.GetYear())
			{
                CCustomInitSet ciSet(fpSet->m_pDatabase);
				ciSet.m_strFilter.Format("[SIG_Station]='%s' AND [Year]=%d and [Enabled] <> 0", wxSet.m_StationID,wxSet.m_ObsDate.GetYear());
				ciSet.Open();
				bool usedNFDRScust = false, usedCANcust = false;
				if (! ciSet.IsEOF())
				{
                    // enabled record exists for this station/year. get values and re-initialize
                   int newGreen;//, lookFreeze;

				   if (!ciSet.IsFieldNull(&ciSet.m_GreenJulian) &&
					   !ciSet.IsFieldNull(&ciSet.m_StartKBDI) &&
					   !ciSet.IsFieldNull(&ciSet.m_Start1000))
				   {
		                int newGreen = DayOfYear(ciSet.m_GreenJulian);

						theApp.m_NFDRS.iSetAnnualInit(newGreen, ciSet.m_Start1000, ciSet.m_StartKBDI);
						usedNFDRScust = true;
						//customInits = true;
				   }
				   if(useCanadian)
				   {
						if (!ciSet.IsFieldNull(&ciSet.m_FFMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DC))
						{
		                   canadian.ReInitialize(ciSet.m_FFMC,ciSet.m_DMC,ciSet.m_DC);
						   usedCANcust = true;
						   //customInits = true;
						}
				   }
				}
				if(!usedNFDRScust)
				{
					//set to default station values
					theApp.m_NFDRS.iSetAnnualInit(DayOfYear(staSet.m_GreenJulian), staSet.m_Start1000, sKBDI);
				}
				if(!usedCANcust)
					canadian.ReInitialize(85, 6, 15);
                ciSet.Close();
			}

			yesterday = wxSet.m_ObsDate;
		
			tmp = wxSet.m_Temp;
			tmpMax = wxSet.IsFieldNull(&wxSet.m_TmpMax) ? -10000 : wxSet.m_TmpMax;
			tmpMin = wxSet.IsFieldNull(&wxSet.m_TmpMin) ? -10000 : wxSet.m_TmpMin;
			rh = max(wxSet.m_RH, 1);
			rhMax = wxSet.IsFieldNull(&wxSet.m_RHMax) ? -10000 : wxSet.m_RHMax;
			rhMin = wxSet.IsFieldNull(&wxSet.m_RHMin) ? -10000 : max(wxSet.m_RHMin, 1);
			pptDur = wxSet.IsFieldNull(&wxSet.m_PPTDUR) ? 0 : wxSet.m_PPTDUR;
			sow = wxSet.ResolveSOW();//.IsFieldNull(&wxSet.m_SOW) ? 0 : wxSet.m_SOW;
			greenHerb = wxSet.IsFieldNull(&wxSet.m_GreenHerb) ? 0 : wxSet.m_GreenHerb;
			greenShrub = wxSet.IsFieldNull(&wxSet.m_GreenShrub) ? 0 : wxSet.m_GreenShrub;
			season = wxSet.IsFieldNull(&wxSet.m_Season) ? 0 : wxSet.m_Season;
			windSpd = wxSet.IsFieldNull(&wxSet.m_WS) ? 0 : wxSet.m_WS;
			windDir = wxSet.IsFieldNull(&wxSet.m_WDir) ? 0 : wxSet.m_WDir;
			slopeClass = staSet.m_SlopeCls;
			slopeClass = max(slopeClass, 1);
			slopeClass = min(slopeClass, 5);
			pptAmt = wxSet.IsFieldNull(&wxSet.m_PPTAMT) ? 0.0 : wxSet.m_PPTAMT;
			//omc10 = wxSet.IsFieldNull(&wxSet.m_OMC10) ? -1000.0 : wxSet.m_OMC10;
			omc10 = (bUseStick && !wxSet.IsFieldNull(&wxSet.m_OMC10)) ? wxSet.m_OMC10 : -1000.0;
			wetFlag = wxSet.ResolveWetFlag();//IsFieldNull(&wxSet.m_WetFlag) ? 0 : wxSet.m_WetFlag;
			snowFlag = wxSet.IsFieldNull(&wxSet.m_SnowFlag) ? 0 : wxSet.m_SnowFlag;
			bool isDailyObs = wxSet.IsFieldNull(&wxSet.m_DailyObs) ? false : wxSet.m_DailyObs;


			double hourlyPrecip = wxSet.IsFieldNull(&wxSet.m_HourlyPrecip) ? 0 : wxSet.m_HourlyPrecip;
			double solarRad = wxSet.IsFieldNull(&wxSet.m_SolarRadiation) ? 0 : wxSet.m_SolarRadiation;

			//calculate model outputs
			//NFDR 78
			if(useNFDRS || useLFI)
			{
				
				if (isNFDRS2016(staSet.m_NFDRSFM[0]) || useLFI)
				{ // new NFDRS2016 calc

					int year1 = wxSet.m_ObsDate.GetYear();
					int month1 = wxSet.m_ObsDate.GetMonth();
					int day1 = wxSet.m_ObsDate.GetDay();
					int hour1 = wxSet.m_ObsDate.GetHour();
					int julian1 = wxSet.m_ObsDate.GetDayOfYear();
				
					theApp.m_NFDRS2016.Update(year1, month1, day1, hour1, julian1, (double)tmp, (double)tmpMin, 
						(double)tmpMax, (double)rh, (double)rhMin, hourlyPrecip, pptAmt, 
						solarRad, (double)windSpd, snowFlag, 
						(isDailyObs) ? hour1 : -1);//staSet.m_RegSchdObs);
					pptAcc += hourlyPrecip;

					f1 = theApp.m_NFDRS2016.MC1;
					f10 = theApp.m_NFDRS2016.MC10;
					f100 = theApp.m_NFDRS2016.MC100;
					f1000 = theApp.m_NFDRS2016.MC1000;
					lfiHerbVal = fHerb = theApp.m_NFDRS2016.MCHERB;
					lfiWoodyVal = fWood = theApp.m_NFDRS2016.MCWOOD;
					lfiVal = theApp.m_NFDRS2016.m_GSI;
					//					iStage = theApp.m_NFDRS2016.Stage;
//					fGren = theApp.m_NFDRS2016.Gren;
					fX1000 = 0; // fixme
//					iRainEvent = theApp.m_NFDRS2016.RainEvent;
					iKBDI = theApp.m_NFDRS2016.KBDI;
					fROS = 0; //fixme
					fSC = theApp.m_NFDRS2016.SC;
					fERC = theApp.m_NFDRS2016.ERC;
					fFL = 0; //fixme
					fFIL = 0; //fixme
					fBI = theApp.m_NFDRS2016.BI;
					fIC = theApp.m_NFDRS2016.IC;

					if (fERC < 0.0)
						fERC = 0.0;
					if (fSC < 0.0)
						fSC = 0.0;
					if (fBI < 0.0)
						fBI = 0.0;
					if (fIC < 0.0)
						fIC - 0.0;


				

					iSC = (int)fSC;
					iFIL = (int)fFIL;
					iBI = (int)fBI;
					iIC = (int)fIC;

				

				}
				if (!isNFDRS2016(staSet.m_NFDRSFM[0]))
				{ // "old" NFDRS calc


				// 2016 new procedure for 100-h, 1000-h, KBDI
					if (wxSet.m_Hour != regObsHour && nRec > 0)

					{

						theApp.m_NFDRS.iCalcMoistWetFlagSnowFlag2(init, tmp, rh,
							tmpMax,
							rhMax,
							tmpMin,
							rhMin,
							pptDur,
							pptAmt,
							sow,
							omc10,
							DayOfYear(wxSet.m_ObsDate),
							wxSet.m_ObsDate.GetYear(),
							iDeclareGreenUp,
							iDeclareFreeze,
							greenHerb,
							greenShrub,
							season,
							wetFlag, snowFlag,
							&f1, &f10, &iStage, &fGren, &fX1000, &iRainEvent);

						if (d100 > 0.0)
							f100 = d100;
						if (d1000 > 0.0)
							f1000 = d1000;
						if (dKBDI > 0)
							iKBDI = dKBDI;
						if (dWoody > 0.0)
							fWood = dWoody;


					}
					else
					{
						theApp.m_NFDRS.iCalcMoistWetFlagSnowFlag(init, tmp, rh,
							tmpMax,
							rhMax,
							tmpMin,
							rhMin,
							pptDur,
							pptAmt,
							sow,
							omc10,
							DayOfYear(wxSet.m_ObsDate),
							wxSet.m_ObsDate.GetYear(),
							iDeclareGreenUp,
							iDeclareFreeze,
							greenHerb,
							greenShrub,
							season,
							wetFlag, snowFlag,
							&f1, &f10, &f100, &f1000, &fHerb, &fWood, &iStage, &fGren, &fX1000, &iRainEvent, &iKBDI);

						// for regScheduledObs Hour, keep track of those daily values
						d100 = f100;
						d1000 = f1000;
						dKBDI = iKBDI;
						dHerb = fHerb;
						dWoody = fWood;
					}

					// if state = pregreen/cure/frozen,   Herbaceous is set to 1-hour
						//
					if (iStage == 1 || iStage == 5 || iStage == 6)
					{
						fHerb = f1;
					}
					else {
						if (dHerb > 0.0)
							fHerb = dHerb;
					}
					// new 2016: reset the moistures
					theApp.m_NFDRS.iSetMoistures(f1, f10, f100, f1000, fWood, fHerb, iRainEvent, iKBDI, greenHerb, greenShrub, season, sow);

					theApp.m_NFDRS.iCalcIndexes(windSpd, slopeClass, &fROS, &iSC, &fERC, &fFL, &iFIL, &iBI);
					theApp.m_NFDRS.iCalcIC(tmp, sow, f1, iSC, &iIC);

					fSC = (double)iSC;
					fFIL = (double)iFIL;
					fBI = (double)iBI;
					fIC = (double)iIC;

				}

			}

			nRec ++;

			/*if(useLFI)
			{
				if(lfi.IsUsingVPDAvg())
				{
					if(!wxSet.IsFieldNull(&wxSet.m_RH) && !wxSet.IsFieldNull(&wxSet.m_Temp)&& !wxSet.IsFieldNull(&wxSet.m_TmpMax)
						&& !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !staSet.IsFieldNull(&staSet.m_LatDegrees))
					{
						lfiVal = lfi.CalcLFI_VPDAvg(wxSet.m_RH, wxSet.m_Temp, wxSet.m_TmpMax, wxSet.m_TmpMin, (int) (staSet.m_LatDegrees), wxSet.m_ObsDate.GetDayOfYear());
					

						lfi.AddLFIobs(wxSet.m_ObsDate, lfiVal);
					}
					else
						lfiVal = -1.0;
				}
				else//default to VPDMax
				{
					if(!wxSet.IsFieldNull(&wxSet.m_RHMin) && !wxSet.IsFieldNull(&wxSet.m_TmpMax) && !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !staSet.IsFieldNull(&staSet.m_LatDegrees))
					{
						lfiVal = lfi.CalcLFI(wxSet.m_RHMin, wxSet.m_TmpMax, wxSet.m_TmpMin, (int) (staSet.m_LatDegrees), wxSet.m_ObsDate.GetDayOfYear());
					

						lfi.AddLFIobs(wxSet.m_ObsDate, lfiVal);
					}
					else
						lfiVal = -1.0;
				}
				if(lfiHerb.IsUsingVPDAvg())
				{
					if(!wxSet.IsFieldNull(&wxSet.m_RH) && !wxSet.IsFieldNull(&wxSet.m_Temp)&& !wxSet.IsFieldNull(&wxSet.m_TmpMax)
						&& !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !staSet.IsFieldNull(&staSet.m_LatDegrees))
					{
						
						    lfiHerbVal = lfiHerb.CalcLFI_VPDAvg(wxSet.m_RH, wxSet.m_Temp, wxSet.m_TmpMax, wxSet.m_TmpMin, (int) (staSet.m_LatDegrees), wxSet.m_ObsDate.GetDayOfYear());

						

						lfiHerb.AddLFIobs(wxDay, lfiHerbVal);
					}
					else
						lfiHerbVal = -1.0;
				}
				else//default to VPDMax
				{
					if((!wxSet.IsFieldNull(&wxSet.m_RHMin) || !wxSet.IsFieldNull(&wxSet.m_RH)) && !wxSet.IsFieldNull(&wxSet.m_TmpMax) && !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !staSet.IsFieldNull(&staSet.m_LatDegrees))
					{
						int tRH = wxSet.m_RH;
						if(!wxSet.IsFieldNull(&wxSet.m_RHMin))
							tRH = wxSet.m_RHMin;
					
						lfiHerbVal = lfiHerb.CalcLFI(tRH, wxSet.m_TmpMax, wxSet.m_TmpMin, (int) (staSet.m_LatDegrees), wxSet.m_ObsDate.GetDayOfYear());
							
						//lfiHerbVal = lfiHerb.CalcLFI(wxSet->m_RHMin, wxSet->m_TmpMax, wxSet->m_TmpMin, (int) (staSet.m_LatDegrees), wxSet->m_ObsDate.GetDayOfYear());
						
						lfiHerb.AddLFIobs(wxDay, lfiHerbVal);
					}
					else
						lfiHerbVal = -1.0;
				}
				if(lfiWoody.IsUsingVPDAvg())
				{
					if(!wxSet.IsFieldNull(&wxSet.m_RH) && !wxSet.IsFieldNull(&wxSet.m_Temp)&& !wxSet.IsFieldNull(&wxSet.m_TmpMax)
						&& !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !staSet.IsFieldNull(&staSet.m_LatDegrees))
					{
						lfiWoodyVal = lfiWoody.CalcLFI_VPDAvg(wxSet.m_RH, wxSet.m_Temp, wxSet.m_TmpMax, wxSet.m_TmpMin, (int) (staSet.m_LatDegrees), wxSet.m_ObsDate.GetDayOfYear());
					

						lfiWoody.AddLFIobs(wxDay, lfiWoodyVal);
					}
					else
						lfiWoodyVal = -1.0;
				}
				else//default to VPDMax
				{
					if((!wxSet.IsFieldNull(&wxSet.m_RHMin) || !wxSet.IsFieldNull(&wxSet.m_RH)) && !wxSet.IsFieldNull(&wxSet.m_TmpMax) && !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !staSet.IsFieldNull(&staSet.m_LatDegrees))
					{
						int tRH = wxSet.m_RH;
						if(!wxSet.IsFieldNull(&wxSet.m_RHMin))
							tRH = wxSet.m_RHMin;
						lfiWoodyVal = lfiWoody.CalcLFI(tRH, wxSet.m_TmpMax, wxSet.m_TmpMin, (int) (staSet.m_LatDegrees), wxSet.m_ObsDate.GetDayOfYear());
					

						//lfiWoodyVal = lfiWoody.CalcLFI(wxSet->m_RHMin, wxSet->m_TmpMax, wxSet->m_TmpMin, (int) (staSet.m_LatDegrees), wxSet->m_ObsDate.GetDayOfYear());
						lfiWoody.AddLFIobs(wxDay, lfiWoodyVal);
					}
					else
						lfiWoodyVal = -1.0;
				}

			
				
			}*/
			//CANADIAN
			if(useCanadian)
				canadian.CalcIndices(DayOfYear(wxSet.m_ObsDate), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetYear(),
				tmp, rh, windSpd * 1.15, pptAmt,
				&ffmc, &dmc, &dc, &bui, &isi, &fwi, &dsr);

			// 2016 apply working set filter (don't display values for dates outside filter range)
			bool display=true;
			if (wxSet.m_ObsDate.GetYear() < fpSet->m_StartYear)
				display = false;
			else if (wxSet.m_ObsDate.GetYear() > fpSet->m_EndYear)
				display = false;
			else if (wxSet.m_ObsDate.GetMonth() < MonthChtoInt(fpSet->m_StartMonth))
				display = false;
			else if (wxSet.m_ObsDate.GetMonth() > MonthChtoInt(fpSet->m_EndMonth))
				display = false;
			else if (wxSet.m_ObsDate.GetMonth() == MonthChtoInt(fpSet->m_StartMonth) && wxSet.m_ObsDate.GetDay() < fpSet->m_StartDay)
				display = false;
			else if (wxSet.m_ObsDate.GetMonth() == MonthChtoInt(fpSet->m_EndMonth) && wxSet.m_ObsDate.GetDay() > fpSet->m_EndDay)
				display = false;
			
			/*if (recCounter)
				recCounter->Increment();
				*/
			if (! display)
			{
				init = false;
				continue;
			}

		  

			// strings for SQL INSERT into HL_OUTPUT
			CString varStr = "",
					valStr = "";
			char dateChar[30];
			char timeChar[30];
			varStr += ",[StationID]";
			valStr += ",'"+CString(fpSet->m_SIG_Station).Trim()+"'";

			switch(listOpts->dt)
			{
			case Full:
				
				sprintf(dateChar, "%02d/%02d/%4d",
					wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay(),
					wxSet.m_ObsDate.GetYear());

				
				break;
			case Packed:
				//if(Delimiter <= 0)
				//	fprintf(stream, "%02d%02d%4d   ",
				//		wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay(),
				//		wxSet.m_ObsDate.GetYear());
				//else

				sprintf(dateChar, "%02d/%02d/%4d",
					wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay(),
					wxSet.m_ObsDate.GetYear());

				break;
			case Reverse:
				//if(Delimiter <= 0)
				//	fprintf(stream, "%4d%02d%02d   ",
				//		wxSet.m_ObsDate.GetYear(), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay());
				//else

				    sprintf(dateChar, "%4d%02d%02d",
						wxSet.m_ObsDate.GetYear(), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay());

				break;
			case MD:
				//if(Delimiter <= 0)
				//	fprintf(stream, "%02d/%02d      ",
				//		wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay());
				//else
				    sprintf(dateChar, "%02d/%02d",
						wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay());

				
				break;
			case MDPacked:
				//if(Delimiter <= 0)
					//fprintf(stream, "%02d%02d       ",
					//	wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay());
				//else
				   sprintf(dateChar, "%02d%02d",
						wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay());

				break;
			}

			fprintf(stream,"%s%s",dateChar,delim);

			switch(listOpts->tt)
			{
			case FullTime:
				//if (Delimiter > 0)
					sprintf(timeChar, "%02d:%02d",
						wxSet.m_ObsDate.GetHour(), wxSet.m_ObsDate.GetMinute());
				//else
					//fprintf(stream, "%02d:%02d ",
						//wxSet.m_ObsDate.GetHour(), wxSet.m_ObsDate.GetMinute());
				break;
			case PackedTime:
				//if (Delimiter > 0)
					sprintf(timeChar, "%02d%02d",
						wxSet.m_ObsDate.GetHour(), wxSet.m_ObsDate.GetMinute());
				//else
					//fprintf(stream, "%02d%02d  ",
						//wxSet.m_ObsDate.GetHour(), wxSet.m_ObsDate.GetMinute());
				break;
			case NoTime:
				sprintf(timeChar, "%02d:%02d",
						wxSet.m_ObsDate.GetHour(), wxSet.m_ObsDate.GetMinute());
				break;
			}
			if (listOpts->tt != NoTime)
			{
				fprintf(stream,"%s%s",timeChar,delim);
			}
			bool nullVal;
			int iVals  = 0;

			for(int l = 0; l < nVarIDs; l++)
			{
				char frmt[16];
				char varVal[32];

				sprintf(frmt, "%%%s%s", varFormats[varIDs[l]], delim);
	
				nullVal = false;
				switch(varIDs[l])
				{
				case 0:
					sprintf(varVal,frmt, (double)tmp);
					
					break;
				case 1:
					if(!wxSet.IsFieldNull(&wxSet.m_TmpMax) && !wxSet.IsFieldNull(&wxSet.m_TmpMin))
						sprintf(varVal, frmt, (double)(wxSet.m_TmpMax + wxSet.m_TmpMin) / 2.0);
					else
						nullVal = true;
					break;
				case 2:
					if(!wxSet.IsFieldNull(&wxSet.m_TmpMin))
						sprintf(varVal, frmt, (double)wxSet.m_TmpMin);
					else
						nullVal = true;
					break;
				case 3:
					if(!wxSet.IsFieldNull(&wxSet.m_TmpMax))
						sprintf(varVal, frmt, (double)wxSet.m_TmpMax);
					else
						nullVal = true;
					break;
				case 4:
					sprintf(varVal, frmt, (double)rh);
					break;
				case 5:
					if(!wxSet.IsFieldNull(&wxSet.m_RHMax) && !wxSet.IsFieldNull(&wxSet.m_RHMin))
						sprintf(varVal, frmt, (double)(wxSet.m_RHMax + max(wxSet.m_RHMin, 1)) / 2.0);
					else
						nullVal = true;
					break;
				case 6:
					if(!wxSet.IsFieldNull(&wxSet.m_RHMin))
						sprintf(varVal, frmt, (double)max(wxSet.m_RHMin, 1));
					else
						nullVal = true;
					break;
				case 7:
					if(!wxSet.IsFieldNull(&wxSet.m_RHMax))
						sprintf(varVal, frmt, (double)wxSet.m_RHMax);
					else
						nullVal = true;
					break;
				case 8:
					if(!wxSet.IsFieldNull(&wxSet.m_PPTAMT))
						sprintf(varVal, frmt, wxSet.m_PPTAMT);
					else
						nullVal = true;
					break;
				case 9:
					if(!wxSet.IsFieldNull(&wxSet.m_PPTDUR))
						sprintf(varVal, frmt, (double)wxSet.m_PPTDUR);
					else
						nullVal = true;
					break;
				case 10:
					if(!wxSet.IsFieldNull(&wxSet.m_WS))
						sprintf(varVal, frmt, (double)wxSet.m_WS);
					else
						nullVal = true;
					break;
				case 11:
					sprintf(varVal, frmt, fSC);
					break;
				case 12:
					sprintf(varVal, frmt, fERC);
					break;
				case 13:
					sprintf(varVal, frmt, fBI);
					break;
				case 14:
					sprintf(varVal, frmt, (double)iKBDI);
					break;
				case 15:
					sprintf(varVal, frmt, fIC);
					break;
				case 16:
					sprintf(varVal, frmt, f1);
					break;
				case 17:
					sprintf(varVal, frmt, f10);
					break;
				case 18:
					sprintf(varVal, frmt, f100);
					break;
				case 19:
					sprintf(varVal, frmt, f1000);
					break;
				case 20:
					sprintf(varVal, frmt, fHerb);
					break;
				case 21:
					sprintf(varVal, frmt, fWood);
					break;
				case 22:
					if (!wxSet.IsFieldNull(&wxSet.m_SnowFlag))
						sprintf(varVal, frmt, (double)wxSet.m_SnowFlag);
					else
						nullVal = true;
					break;
				case 23:
					if(!wxSet.IsFieldNull(&wxSet.m_WDir))
						sprintf(varVal, frmt, (double)wxSet.m_WDir);
					else
						nullVal = true;
					break;
				case 24:
					if(!wxSet.IsFieldNull(&wxSet.m_SOW))
						sprintf(varVal, frmt, (double)wxSet.m_SOW);
					else
						nullVal = true;
					break;
				case 25:
					sprintf(varVal, frmt, ffmc);
					break;
				case 26:
					sprintf(varVal, frmt, dmc);
					break;
				case 27:
					sprintf(varVal, frmt, tmp);
					break;
				case 28:
					sprintf(varVal, frmt, dc);
					break;
				case 29:
					sprintf(varVal, frmt, bui);
					break;
				case 30:
					sprintf(varVal, frmt, fwi);
					break;
				case 31:
					sprintf(varVal, frmt, dsr);
					break;
				case 32:
					if(!wxSet.IsFieldNull(&wxSet.m_GustDir))
						sprintf(varVal, frmt, (double)wxSet.m_GustDir);
					else
						nullVal = true;
					break;
				case 33:
					if(!wxSet.IsFieldNull(&wxSet.m_GustSpd))
						sprintf(varVal, frmt, (double)wxSet.m_GustSpd);
					else
						nullVal = true;
					break;
				case 34:
					if(!wxSet.IsFieldNull(&wxSet.m_SolarRadiation))
						sprintf(varVal, frmt, (double)wxSet.m_SolarRadiation);
					else
						nullVal = true;
					break;
				case 35:
					if(!wxSet.IsFieldNull(&wxSet.m_WetFlag))
						sprintf(varVal, frmt, (double)wxSet.m_WetFlag);
					else
						nullVal = true;
					break;
				case 36:
					if(!wxSet.IsFieldNull(&wxSet.m_RH) && !wxSet.IsFieldNull(&wxSet.m_Temp))
						sprintf(varVal, frmt, CalcDPT(wxSet.m_Temp, max(wxSet.m_RH, 1)));
					else
						nullVal = true;
					break;
				case 37://Vapor pressure deficit MAX
					if(!wxSet.IsFieldNull(&wxSet.m_RHMin) && !wxSet.IsFieldNull(&wxSet.m_TmpMax))
						sprintf(varVal, frmt, CalcVPD(max(wxSet.m_RHMin, 1), wxSet.m_TmpMax));
					//val = CalcVPD(wxSet.m_RHMin, wxSet.m_TmpMax);
					else
						nullVal = true;
					break;
				case 38://Vapor pressure deficit Avg
					if(!wxSet.IsFieldNull(&wxSet.m_Temp) && !wxSet.IsFieldNull(&wxSet.m_RH) && !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !wxSet.IsFieldNull(&wxSet.m_TmpMax))
					{
						double dpt = CalcDPT(wxSet.m_Temp, max(wxSet.m_RH, 1));
						double avgT = (wxSet.m_TmpMin + wxSet.m_TmpMax) / 2;
						sprintf(varVal, frmt, CalcVPDavg(dpt, avgT));
						//val = CalcVPDavg(dpt, avgT);
					}
					else
						nullVal = true;
					break;
				case 39://Live Fuel Index
					if(lfiVal >= 0.0)
						sprintf(varVal, frmt, lfiVal);// lfi.CalcRunningAvgLFI());
					//val = lfi.CalcRunningAvgLFI();
					else
						nullVal = true;
					break;
				case 40://Wind Azimuth
					if(!wxSet.IsFieldNull(&wxSet.m_WAzimuth))
						sprintf(varVal, frmt, (double)wxSet.m_WAzimuth);
					else
						nullVal = true;
					break;
				case 41://Hourly Precip
					if(!wxSet.IsFieldNull(&wxSet.m_HourlyPrecip))
						sprintf(varVal, frmt, (double)wxSet.m_HourlyPrecip);
					else
						nullVal = true;
					break;

				case 42:
					if(lfiHerbVal >= 0.0)
						sprintf(varVal, frmt, lfiHerbVal);// lfiHerb.CalcRunningAvgHerbFM());
					else
						nullVal = true;
					break;
				case 43:
					if(lfiWoodyVal >= 0.0)
						sprintf(varVal, frmt, lfiWoodyVal);// lfiWoody.CalcRunningAvgWoodyFM());
					else
						nullVal = true;
					break;
				case 44:
					if(!wxSet.IsFieldNull(&wxSet.m_Temp) && !wxSet.IsFieldNull(&wxSet.m_RH)
							&& !wxSet.IsFieldNull(&wxSet.m_WS))
								sprintf(varVal, frmt, (double)theApp.m_NFDRS.iCalcFFWI(wxSet.m_Temp, max(wxSet.m_RH, 1), wxSet.m_WS));
					else
						nullVal = true;
					break;
				}

				if (! nullVal)
				{
   				   fprintf(stream,varVal);

					if (createTable)
					   if (strlen(varVal) > 0)
					   {
						  
							char fieldName[20];
							if(varIDs[l] < STATICVARIDS)
							{
								sprintf(fieldName, "%5.5s", abbrevs[varIDs[l]]);						
								varStr += ",[" + CString(fieldName).Trim() + "]";
								valStr += "," + CString(varVal).Trim();
								iVals ++;
							}
							
					   }
				} else

					fprintf(stream, "%s", delim);
			}

			// 2015
					// add record to optional HL_Output table
					CString dateStr;
					CString idStr;

					numRecs ++;

					if(createTable)
					{
						try
						{
						
							CString dateStr = CString(dateChar) + CString(" ") + CString(timeChar);
							idStr.Format("%d",numRecs - 1);
						
							CString strSql;
							// for first record, add the optional memo field
							if (numRecs == 1)
							{
                                 varStr += ",[Memo]";
								 valStr += (",'" + tableMemo + "'");

							}
							strSql = "INSERT INTO [HL_Output] ([ID],[ObsDate]" + varStr + ") VALUES (" + idStr.Trim() + ",'" + dateStr.Trim() + "'" + valStr + ")";
							
							fpSet->m_pDatabase->ExecuteSQL(strSql);
							

						}
						catch(CDBException* e)
						{
							e->Delete();
						}
					}

			fprintf(stream, "\n");
			init = false;
		}
	}
	wxSet.Close();
	if(_caller)
		_caller->EnableWindow(TRUE);
/*	if(recCounter && recCounter->Abort())
	{//clean up and leave
		free(fName);
		recCounter->DestroyWindow();
		recCounter = NULL;
		return -1;
	}
	if(recCounter)
	{
		recCounter->DestroyWindow();
		recCounter = NULL;
	}*/
	//if(listOpts->dateStamp)
	//	fprintf(stream, "\n%s\n", AddDateStamp());
	if(stream)
	{
		fclose(stream);
		//now display the file in a child window
		POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

		while(curTemplatePos != NULL)
		{
			CDocTemplate* curTemplate =
				theApp.GetNextDocTemplate(curTemplatePos);
			CString str;
			curTemplate->GetDocString(str, CDocTemplate::docName);
			if(str == _T("TextFile"))
			{
				CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(fName);
				pDoc->isTemp = true;
				free(fName);
				CString tTitle;
				if (createTable)
				   tTitle.Format("%s - Hourly Listing - Output exported to table 'HL_Output'", fpSet->m_SIG_Station);
				else
				   tTitle.Format("%s - Hourly Listing", fpSet->m_SIG_Station);
			
				pDoc->SetTitle(tTitle);
				return 0;
			}
		}
	}
	free(fName);
	return 1;
}

CString CDiurnalAnalysis::AddDateStamp()
{
	COleDateTime dateTime = COleDateTime::GetCurrentTime();
	CString dStr = dateTime.Format("%m/%d/%Y-%H:%M");
	CString ret;
	ret.Format("FF+%s %s %s", verStr, theApp.m_buildDateStrPacked, dStr);
	//ret.Format("FF+%s %s", verStr, dStr);
	return ret;
	//COleDateTime dateTime = COleDateTime::GetCurrentTime();
	//return dateTime.Format("FF+3.0 %m/%d/%Y-%H:%M");
}

CString CDiurnalAnalysis::GetModelString(CFireplusSet *fpSet, CSIGStationSet *stn)
{
	CString ret;
	CString staStr = fpSet->m_SIG_Station;
	staStr.TrimRight();
	if(staStr.GetLength() > 6)
		ret.Format("%d%s", stn->m_Use88 ? 8 : 7, stn->m_NFDRSFM);
	else
		ret.Format("%d%s%d%c%c%d", stn->m_Use88 ? 8 : 7, stn->m_NFDRSFM, stn->m_SlopeCls,
			stn->m_HerbAnnual ? 'A' : 'P', stn->m_Deciduous ? 'D' : 'E', stn->m_ClimateCls);
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// CDiurnalDialog dialog

CDiurnalDialog::CDiurnalDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet)
	: CDialog(CDiurnalDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiurnalDialog)
	//}}AFX_DATA_INIT
	fpSet = _fpSet;
}

void CDiurnalDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiurnalDialog)
	DDX_Control(pDX, IDC_LIST2, destList);
	DDX_Control(pDX, IDC_VARLIST, varList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDiurnalDialog, CDialog)
	//{{AFX_MSG_MAP(CDiurnalDialog)
	ON_BN_CLICKED(IDC_MOVELEFT, OnMoveleft)
	ON_BN_CLICKED(IDC_MOVERIGHT, OnMoveright)
	ON_BN_CLICKED(IDC_REMOVEALL, OnRemoveall)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_LBN_DBLCLK(IDC_VARLIST, OnDblclkVarlist)
	ON_LBN_DBLCLK(IDC_LIST2, OnDblclkList2)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_TOP, OnTop)
	ON_BN_CLICKED(IDC_BOTTOM, OnBottom)
	ON_COMMAND(ID_VARSORT, OnVarSort)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiurnalDialog message handlers

void CDiurnalDialog::OnOK()
{
	CWaitCursor wait;
	int nVars = destList.GetCount();
	if(nVars <= 0)
	{
		AfxMessageBox("ERROR: You must select at least one variable.");
		return;
	}
	int *varIDs = new int[nVars];
	int vLoc = 0;
	for (int i=0; i < destList.GetCount(); i++)
	{
		//if(destList.GetSel(i))
	//	{
			varIDs[i] = (int)destList.GetItemData(i);
	//	}
	}
	EnableWindow(FALSE);
	CDiurnalAnalysis * da = new CDiurnalAnalysis();
	int ret = da->Analyze(this, fpSet, nVars, varIDs);
	EnableWindow(TRUE);
	if(ret != 0)
	{
		delete da;
		return; //user cancelled
	}
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();
	CDocTemplate* curTemplate;
	CDiurnalDoc *pDoc;
	while(curTemplatePos != NULL)
	{
		curTemplate =
			theApp.GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("DiurnalView"))
		{
			pDoc = (CDiurnalDoc *)curTemplate->CreateNewDocument();
			pDoc->da = da;
			pDoc->nWxObs = da->count;
			pDoc->start.SetDate(fpSet->m_StartYear, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
			pDoc->end.SetDate(fpSet->m_EndYear, MonthChtoInt(fpSet->m_EndMonth), fpSet->m_EndDay);
			pDoc->stationName = da->staName;
			pDoc->stationName.TrimRight();
			pDoc->model = da->modelStr;
			pDoc->stamp = da->stamp;
			break;
		}
	}
	CClimateSet climSet(fpSet->m_pDatabase);
	climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
	climSet.Open();
	for(int v = 0; v < nVars; v++)
	{
		CDiurnalFrame* pFrame = (CDiurnalFrame*)curTemplate->CreateNewFrame(pDoc, NULL);
		pFrame->varID = varIDs[v];
		climSet.m_strFilter.Format("[VarID] = %d", varIDs[v] + 1);
		climSet.Requery();
		pFrame->varName = climSet.m_Variable_Name;//abbrevs[varIDs[v]];
		curTemplate->InitialUpdateFrame(pFrame, pDoc);
		CString tTitle = "";
		tTitle.Format("%s - %s Diurnal Analysis", fpSet->m_SIG_Station, climSet.m_Variable_Name);//abbrevs[varIDs[v]]);
		pFrame->SetWindowText(tTitle);
		//pFrame->SetWindowText(tTitle);
	}
	pDoc->UpdateAllViews(NULL);
	climSet.Close();
	CDialog::OnOK();
}

BOOL CDiurnalDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(!fpSet)
		return TRUE;
	CClimateSet climSet(fpSet->m_pDatabase);
	climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
	climSet.setVarSortOrder(theApp.varSortOrder);
	climSet.Open();
	while(!climSet.IsEOF())
	{
		int loc = varList.AddString(climSet.m_Variable_Name);
		if(loc >= 0)
		{
			varList.SetItemData(loc, climSet.m_VarID - 1);
			//if(climSet.m_VarID == optionsSet.m_VarID)
			//	varList.SetCurSel(loc);
		}
		climSet.MoveNext();
	}
	climSet.Close();

	upButton.AutoLoad(IDC_UP, this);
	downButton.AutoLoad(IDC_DOWN, this);
	topButton.AutoLoad(IDC_TOP, this);
	bottomButton.AutoLoad(IDC_BOTTOM, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDiurnalAnalysis::AnalyzeStation(CString stationStr, CSIGStationSet& staSet, CWxSet& wxSet)
{
	int ret = 0;
	/*if(recCounter)
	{
		recCounter->SetStatusString("Reading: " + stationStr);
	}*/
	CLFIengine lfi(wxSet.m_pDatabase), lfiHerb(wxSet.m_pDatabase, HerbParams, staSet.m_StationID, staSet.m_HerbAnnual),
		lfiWoody(wxSet.m_pDatabase, WoodyParams, staSet.m_StationID);
	//model use flags - optimize to only calculate models used
	bool goodRecord;
	//create data structures based on periods and selected outputs
	//NOW CREATE NFDRCALC MODEL
//	NFDRCalc nfdrs;
	int sKBDI = 0;
	if(!staSet.IsFieldNull(&staSet.m_StartKBDI))
		sKBDI = staSet.m_StartKBDI;


	//NFDR2016Calc *nfdr2016;

	if(useNFDRS || useLFI)
	{
		int lookGreen, lookFreeze;
		lookGreen = DayOfYear(staSet.m_GreenJulian);
		lookFreeze = DayOfYear(staSet.m_FreezeJulian);

		if (isNFDRS2016(staSet.m_NFDRSFM[0]))
		{ // new NFDRS2016 calc
			//nfdr2016 = new NFDR2016Calc((int)staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true);
			//theApp.m_NFDRS2016.Init((int)staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true, lfiHerb.GetHerbMaxGSI(), lfiHerb.GetHerbGreenup());
			theApp.m_NFDRS2016.Init(staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true, staSet.m_HerbAnnual, staSet.m_KBDIThreshold);
			theApp.m_NFDRS2016.iSetFuelModel(staSet.m_NFDRSFM[0]);
			theApp.m_NFDRS2016.SetGSIParams(lfi.GetHerbMaxGSI(), lfi.GetHerbGreenup(), lfi.GetTminMin(), lfi.GetTminMax(), lfi.GetVPDMin(), lfi.GetVPDMax(), 
				lfi.GetDaylenMin(), lfi.GetDaylenMax(), lfi.GetMAPeriod(), lfi.IsUsingVPDAvg(), lfi.GetNumPrecipDays(), lfi.GetRTPcpMin(), lfi.GetRTPcpMax());
			theApp.m_NFDRS2016.SetHerbGSIparams(lfiHerb.GetHerbMaxGSI(), lfiHerb.GetHerbGreenup(), lfiHerb.GetTminMin(), lfiHerb.GetTminMax(), 
				lfiHerb.GetVPDMin(), lfiHerb.GetVPDMax(), lfiHerb.GetDaylenMin(), lfiHerb.GetDaylenMax(), lfiHerb.GetMAPeriod(), lfiHerb.IsUsingVPDAvg(), 
				lfiHerb.GetNumPrecipDays(), lfiHerb.GetRTPcpMin(), lfiHerb.GetRTPcpMax());
			theApp.m_NFDRS2016.SetWoodyGSIparams(lfiWoody.GetWoodyMaxGSI(), lfiWoody.GetWoodyGreenup(), lfiWoody.GetTminMin(), lfiWoody.GetTminMax(), 
				lfiWoody.GetVPDMin(), lfiWoody.GetVPDMax(), lfiWoody.GetDaylenMin(), lfiWoody.GetDaylenMax(), lfiWoody.GetMAPeriod(), lfiWoody.IsUsingVPDAvg(), 
				lfiWoody.GetNumPrecipDays(), lfiWoody.GetRTPcpMin(), lfiWoody.GetRTPcpMax());
			theApp.m_NFDRS2016.SetStartKBDI(staSet.m_StartKBDI);
			theApp.m_NFDRS2016.SetSCMax(staSet.GetSCM(staSet.m_NFDRSFM[0]));
			theApp.m_NFDRS2016.SetMxdHumid(staSet.GetMxHumid(staSet.m_NFDRSFM[0]));
		}
		if (!isNFDRS2016(staSet.m_NFDRSFM[0]))
		{   // "old" NFDRS calc

			theApp.m_NFDRS.iInitialize(staSet.m_Use88 ? 88 : 78,
				staSet.m_ClimateCls, lookFreeze, lookGreen,
				(int)(staSet.m_LatDegrees), staSet.m_HerbAnnual, staSet.m_Deciduous, staSet.m_FM1Eq10,
				staSet.m_Start1000 - 5.0, staSet.m_Start1000, sKBDI, staSet.m_AvgPrecip);
			theApp.m_NFDRS.iSetFuelModel(staSet.m_NFDRSFM[0], 0);
			if (!staSet.IsFieldNull(&staSet.m_UseDormant) && !staSet.IsFieldNull(&staSet.m_DormantJulian))
			{
				if (staSet.m_UseDormant && staSet.m_DormantJulian.GetStatus() == COleDateTime::valid)
					theApp.m_NFDRS.iSetJulianShrubDormant(DayOfYear(staSet.m_DormantJulian));
			}
		}
	}
	bool bUseStick = true;
	if(!staSet.IsFieldNull(&staSet.m_UseStick))
		bUseStick = staSet.m_UseStick;
	//create Canadian model
	Canadian canadian;
	if(useCanadian)
		canadian.Initialize();
	//crunch data...
	int iSC, iFIL, iBI,
		iIC, iStage, iKBDI, iRainEvent, init = 1, iDeclareGreenUp = 0, iDeclareFreeze = 0;//,
	double f1, f10, f100, f1000, fHerb, fWood, fGren, fX1000, fROS, fERC, fFL,
		ffmc, dmc, dc, bui, isi, fwi, dsr, lfiVal, lfiHerbVal, lfiWoodyVal;
	COleDateTime wxDay;//used to skip duplicates
	COleDateTime yesterday;//used to skip duplicates
	wxSet.Requery();
	if(!wxSet.IsBOF())
	{
		//working variables
		int tmp, tmpMax, tmpMin, rh, rhMax, rhMin, pptDur, sow, greenHerb, greenShrub,
			season, windSpd, windDir, slopeClass, wetFlag, snowFlag;
		double pptAmt, omc10;
		while(!wxSet.IsEOF())// && (! recCounter || (recCounter && !recCounter->Abort())))
		{
			if(((useNFDRS || useCanadian) && (wxSet.IsFieldNull(&wxSet.m_Temp) || wxSet.IsFieldNull(&wxSet.m_RH))
				|| wxSet.IsFieldNull(&wxSet.m_ObsDate)))
			//if(wxSet.IsFieldNull(&wxSet.m_Temp)
			//	|| wxSet.IsFieldNull(&wxSet.m_RH)
			//	|| wxSet.IsFieldNull(&wxSet.m_ObsDate))
			{
				wxSet.MoveNext();
				continue;
			}
			if(!init && yesterday >= wxSet.m_ObsDate)
			{
				wxSet.MoveNext();
				continue;
			}
			wxDay.SetDateTime(wxSet.m_ObsDate.GetYear(), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay(), wxSet.m_ObsDate.GetHour(),
				wxSet.m_ObsDate.GetMinute(), wxSet.m_ObsDate.GetSecond());
			// new custom init check    12/2006
			// if this is first record for new year, look for inits
			if (yesterday.GetYear() != wxSet.m_ObsDate.GetYear())
			{
                CCustomInitSet ciSet(wxSet.m_pDatabase);
				ciSet.m_strFilter.Format("[SIG_Station]='%s' AND [Year]=%d and [Enabled] <> 0", wxSet.m_StationID,wxSet.m_ObsDate.GetYear());
				ciSet.Open();
				bool usedNFDRScust = false, usedCANcust = false;
				if (! ciSet.IsEOF())
				{
                    // enabled record exists for this station/year. get values and re-initialize
                   int newGreen;//, lookFreeze;

				   if (!ciSet.IsFieldNull(&ciSet.m_GreenJulian) &&
					   !ciSet.IsFieldNull(&ciSet.m_StartKBDI) &&
					   !ciSet.IsFieldNull(&ciSet.m_Start1000))
				   {
		                int newGreen = DayOfYear(ciSet.m_GreenJulian);

						theApp.m_NFDRS.iSetAnnualInit(newGreen, ciSet.m_Start1000, ciSet.m_StartKBDI);
						usedNFDRScust = true;
						//customInits = true;
				   }
				   if(useCanadian)
				   {
						if (!ciSet.IsFieldNull(&ciSet.m_FFMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DC))
						{
		                   canadian.ReInitialize(ciSet.m_FFMC,ciSet.m_DMC,ciSet.m_DC);
						   usedCANcust = true;
						   //customInits = true;
						}
				   }
				}
				if(!usedNFDRScust)
				{
					//set to default station values
					theApp.m_NFDRS.iSetAnnualInit(DayOfYear(staSet.m_GreenJulian), staSet.m_Start1000, sKBDI);
				}
				if(!usedCANcust)
					canadian.ReInitialize(85, 6, 15);
                ciSet.Close();
			}
			yesterday = wxSet.m_ObsDate;
			/*if(recCounter)
				recCounter->Increment();*/
			tmp = wxSet.m_Temp;
			tmpMax = wxSet.IsFieldNull(&wxSet.m_TmpMax) ? -10000 : wxSet.m_TmpMax;
			tmpMin = wxSet.IsFieldNull(&wxSet.m_TmpMin) ? -10000 : wxSet.m_TmpMin;
			rh = max(wxSet.m_RH, 1);
			rhMax = wxSet.IsFieldNull(&wxSet.m_RHMax) ? -10000 : wxSet.m_RHMax;
			rhMin = wxSet.IsFieldNull(&wxSet.m_RHMin) ? -10000 : max(wxSet.m_RHMin, 1);
			pptDur = wxSet.IsFieldNull(&wxSet.m_PPTDUR) ? 0 : wxSet.m_PPTDUR;
			sow = wxSet.ResolveSOW();//.IsFieldNull(&wxSet.m_SOW) ? 0 : wxSet.m_SOW;
			greenHerb = wxSet.IsFieldNull(&wxSet.m_GreenHerb) ? 0 : wxSet.m_GreenHerb;
			greenShrub = wxSet.IsFieldNull(&wxSet.m_GreenShrub) ? 0 : wxSet.m_GreenShrub;
			season = wxSet.IsFieldNull(&wxSet.m_Season) ? 0 : wxSet.m_Season;
			windSpd = wxSet.IsFieldNull(&wxSet.m_WS) ? 0 : wxSet.m_WS;
			windDir = wxSet.IsFieldNull(&wxSet.m_WDir) ? 0 : wxSet.m_WDir;
			slopeClass = staSet.m_SlopeCls;
			slopeClass = max(slopeClass, 1);
			slopeClass = min(slopeClass, 5);
			pptAmt = wxSet.IsFieldNull(&wxSet.m_PPTAMT) ? 0.0 : wxSet.m_PPTAMT;
			//omc10 = wxSet.IsFieldNull(&wxSet.m_OMC10) ? -1000.0 : wxSet.m_OMC10;
			omc10 = (bUseStick && !wxSet.IsFieldNull(&wxSet.m_OMC10)) ? wxSet.m_OMC10 : -1000.0;
			wetFlag = wxSet.ResolveWetFlag();//IsFieldNull(&wxSet.m_WetFlag) ? 0 : wxSet.m_WetFlag;
			snowFlag = wxSet.IsFieldNull(&wxSet.m_SnowFlag) ? 0 : wxSet.m_SnowFlag;
			bool isDailyObs = wxSet.IsFieldNull(&wxSet.m_DailyObs) ? false : wxSet.m_DailyObs;
			double hourlyPrecip = wxSet.IsFieldNull(&wxSet.m_HourlyPrecip) ? 0.0 : wxSet.m_HourlyPrecip;
			//calculate model outputs
			//NFDR 78
			if(useNFDRS || useLFI)
			{
				if (isNFDRS2016(staSet.m_NFDRSFM[0]) || useLFI)
				{ // new NFDRS2016 calc

					int year1 = wxSet.m_ObsDate.GetYear();
					int month1 = wxSet.m_ObsDate.GetMonth();
					int day1 = wxSet.m_ObsDate.GetDay();
					int hour1 = wxSet.m_ObsDate.GetHour();
					int julian1 = wxSet.m_ObsDate.GetDayOfYear();
					double pptAcc = 0;
					double solarRad = wxSet.m_SolarRadiation;

					theApp.m_NFDRS2016.Update(year1, month1, day1, hour1, julian1, tmp, tmpMin, tmpMax, rh, rhMin, hourlyPrecip, pptAmt, solarRad, windSpd, snowFlag, 
						(isDailyObs) ? hour1 : -1);//staSet.m_RegSchdObs);
					f1 = theApp.m_NFDRS2016.MC1;
					f10 = theApp.m_NFDRS2016.MC10;
					f100 = theApp.m_NFDRS2016.MC100;
					f1000 = theApp.m_NFDRS2016.MC1000;
					lfiHerbVal = fHerb = theApp.m_NFDRS2016.MCHERB;
					lfiWoodyVal = fWood = theApp.m_NFDRS2016.MCWOOD;
					lfiVal = theApp.m_NFDRS2016.m_GSI;
					//					iStage = theApp.m_NFDRS2016.Stage;
//					fGren = theApp.m_NFDRS2016.Gren;
					fX1000 = 0; // fixme
//					iRainEvent = theApp.m_NFDRS2016.RainEvent;
					iKBDI = theApp.m_NFDRS2016.KBDI;
					fROS = 0; //fixme
					iSC = theApp.m_NFDRS2016.SC;
					fERC = theApp.m_NFDRS2016.ERC;
					fFL = 0; //fixme
					iFIL = 0; //fixme
					iBI = theApp.m_NFDRS2016.BI;
					iIC = theApp.m_NFDRS2016.IC;

				}
				if (!isNFDRS2016(staSet.m_NFDRSFM[0]))
				{ // "old" NFDRS calc
					theApp.m_NFDRS.iCalcMoistWetFlagSnowFlag(init, tmp, rh,
						tmpMax,
						rhMax,
						tmpMin,
						rhMin,
						pptDur,
						pptAmt,
						sow,
						omc10,
						DayOfYear(wxSet.m_ObsDate),
						wxSet.m_ObsDate.GetYear(),
						iDeclareGreenUp,
						iDeclareFreeze,
						greenHerb,
						greenShrub,
						season,
						wetFlag, snowFlag,
						&f1, &f10, &f100, &f1000, &fHerb, &fWood, &iStage, &fGren, &fX1000, &iRainEvent, &iKBDI);
					theApp.m_NFDRS.iCalcIndexes(windSpd, slopeClass, &fROS, &iSC, &fERC, &fFL, &iFIL, &iBI);
					theApp.m_NFDRS.iCalcIC(tmp, sow, f1, iSC, &iIC);
				}
			}
			/*if(useLFI)
			{
				if(lfi.IsUsingVPDAvg())
				{
					if(!wxSet.IsFieldNull(&wxSet.m_RH) && !wxSet.IsFieldNull(&wxSet.m_Temp)&& !wxSet.IsFieldNull(&wxSet.m_TmpMax)
						&& !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !staSet.IsFieldNull(&staSet.m_LatDegrees))
					{
						lfiVal = lfi.CalcLFI_VPDAvg(wxSet.m_RH, wxSet.m_Temp, wxSet.m_TmpMax, wxSet.m_TmpMin, (int) (staSet.m_LatDegrees), wxSet.m_ObsDate.GetDayOfYear());
						lfi.AddLFIobs(wxSet.m_ObsDate, lfiVal);
					}
					else
						lfiVal = -1.0;
				}
				else//default to VPDMax
				{
					if(!wxSet.IsFieldNull(&wxSet.m_RHMin) && !wxSet.IsFieldNull(&wxSet.m_TmpMax) && !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !staSet.IsFieldNull(&staSet.m_LatDegrees))
					{
						lfiVal = lfi.CalcLFI(wxSet.m_RHMin, wxSet.m_TmpMax, wxSet.m_TmpMin, (int) (staSet.m_LatDegrees), wxSet.m_ObsDate.GetDayOfYear());
						lfi.AddLFIobs(wxSet.m_ObsDate, lfiVal);
					}
					else
						lfiVal = -1.0;
				}
				if(lfiHerb.IsUsingVPDAvg())
				{
					if(!wxSet.IsFieldNull(&wxSet.m_RH) && !wxSet.IsFieldNull(&wxSet.m_Temp)&& !wxSet.IsFieldNull(&wxSet.m_TmpMax)
						&& !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !staSet.IsFieldNull(&staSet.m_LatDegrees))
					{
						lfiHerbVal = lfiHerb.CalcLFI_VPDAvg(wxSet.m_RH, wxSet.m_Temp, wxSet.m_TmpMax, wxSet.m_TmpMin, (int) (staSet.m_LatDegrees), wxSet.m_ObsDate.GetDayOfYear());
						lfiHerb.AddLFIobs(wxDay, lfiHerbVal);
					}
					else
						lfiHerbVal = -1.0;
				}
				else//default to VPDMax
				{
					if((!wxSet.IsFieldNull(&wxSet.m_RHMin) || !wxSet.IsFieldNull(&wxSet.m_RH)) && !wxSet.IsFieldNull(&wxSet.m_TmpMax) && !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !staSet.IsFieldNull(&staSet.m_LatDegrees))
					{
						int tRH = wxSet.m_RH;
						if(!wxSet.IsFieldNull(&wxSet.m_RHMin))
							tRH = wxSet.m_RHMin;
						lfiHerbVal = lfiHerb.CalcLFI(tRH, wxSet.m_TmpMax, wxSet.m_TmpMin, (int) (staSet.m_LatDegrees), wxSet.m_ObsDate.GetDayOfYear());
						//lfiHerbVal = lfiHerb.CalcLFI(wxSet->m_RHMin, wxSet->m_TmpMax, wxSet->m_TmpMin, (int) (staSet.m_LatDegrees), wxSet->m_ObsDate.GetDayOfYear());
						lfiHerb.AddLFIobs(wxDay, lfiHerbVal);
					}
					else
						lfiHerbVal = -1.0;
				}
				if(lfiWoody.IsUsingVPDAvg())
				{
					if(!wxSet.IsFieldNull(&wxSet.m_RH) && !wxSet.IsFieldNull(&wxSet.m_Temp)&& !wxSet.IsFieldNull(&wxSet.m_TmpMax)
						&& !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !staSet.IsFieldNull(&staSet.m_LatDegrees))
					{
						lfiWoodyVal = lfiWoody.CalcLFI_VPDAvg(wxSet.m_RH, wxSet.m_Temp, wxSet.m_TmpMax, wxSet.m_TmpMin, (int) (staSet.m_LatDegrees), wxSet.m_ObsDate.GetDayOfYear());
						lfiWoody.AddLFIobs(wxDay, lfiWoodyVal);
					}
					else
						lfiWoodyVal = -1.0;
				}
				else//default to VPDMax
				{
					if((!wxSet.IsFieldNull(&wxSet.m_RHMin) || !wxSet.IsFieldNull(&wxSet.m_RH)) && !wxSet.IsFieldNull(&wxSet.m_TmpMax) && !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !staSet.IsFieldNull(&staSet.m_LatDegrees))
					{
						int tRH = wxSet.m_RH;
						if(!wxSet.IsFieldNull(&wxSet.m_RHMin))
							tRH = wxSet.m_RHMin;
						lfiWoodyVal = lfiWoody.CalcLFI(tRH, wxSet.m_TmpMax, wxSet.m_TmpMin, (int) (staSet.m_LatDegrees), wxSet.m_ObsDate.GetDayOfYear());
						//lfiWoodyVal = lfiWoody.CalcLFI(wxSet->m_RHMin, wxSet->m_TmpMax, wxSet->m_TmpMin, (int) (staSet.m_LatDegrees), wxSet->m_ObsDate.GetDayOfYear());
						lfiWoody.AddLFIobs(wxDay, lfiWoodyVal);
					}
					else
						lfiWoodyVal = -1.0;
				}
			}*/
			//CANADIAN
			if(useCanadian)
				canadian.CalcIndices(DayOfYear(wxSet.m_ObsDate), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetYear(),
					tmp, rh, windSpd * 1.15, pptAmt,
					&ffmc, &dmc, &dc, &bui, &isi, &fwi, &dsr);
			int day = DayOfYear(wxSet.m_ObsDate) - 1;
			if(wxSet.m_ObsDate.GetHour() == 23 && wxSet.m_ObsDate.GetMinute() >= 30)
			{
				day++;
				if(day > 365)
					day = 0;
			}
			if(day >= 0 && day <= 365)
			{
				double val = 0.0;
				for(int j = 0; j < MAXVARIDS; j++)
				{
					if(days[j])
					{
						goodRecord = true;
						switch(j + 1)
						{
						case 1:
							if(!wxSet.IsFieldNull(&wxSet.m_Temp))
								val = (double)wxSet.m_Temp;
							else
								goodRecord = false;
							break;
						case 2:
							if(!wxSet.IsFieldNull(&wxSet.m_TmpMax) && !wxSet.IsFieldNull(&wxSet.m_TmpMin))
								val = (double)(wxSet.m_TmpMax + wxSet.m_TmpMin) / 2.0;
							else
								goodRecord = false;
							break;
						case 3:
							if(!wxSet.IsFieldNull(&wxSet.m_TmpMin))
								val = (double)wxSet.m_TmpMin;
							else
								goodRecord = false;
							break;
						case 4:
							if(!wxSet.IsFieldNull(&wxSet.m_TmpMax))
								val = (double)wxSet.m_TmpMax;
							else
								goodRecord = false;
							break;
						case 5:
							if(!wxSet.IsFieldNull(&wxSet.m_RH))
								val = (double)max(wxSet.m_RH, 1);
							else
								goodRecord = false;
							break;
						case 6:
							if(!wxSet.IsFieldNull(&wxSet.m_RHMax) && !wxSet.IsFieldNull(&wxSet.m_RHMin))
								val = (double)(wxSet.m_RHMax + max(wxSet.m_RHMin, 1)) / 2.0;
							else
								goodRecord = false;
							break;
						case 7:
							if(!wxSet.IsFieldNull(&wxSet.m_RHMin))
								val = (double)max(wxSet.m_RHMin, 1);
							else
								goodRecord = false;
							break;
						case 8:
							if(!wxSet.IsFieldNull(&wxSet.m_RHMax))
								val = (double)wxSet.m_RHMax;
							else
								goodRecord = false;
							break;
						case 9:
							if(!wxSet.IsFieldNull(&wxSet.m_PPTAMT))
								val = (double)wxSet.m_PPTAMT;
							else
								goodRecord = false;
							break;
						case 10:
							if(!wxSet.IsFieldNull(&wxSet.m_PPTDUR))
								val = (double)wxSet.m_PPTDUR;
							else
								goodRecord = false;
							break;
						case 11:
							if(!wxSet.IsFieldNull(&wxSet.m_WS))// && !wxSet.IsFieldNull(&wxSet.m_WDir))
								val = (double)wxSet.m_WS;
							else
								goodRecord = false;
							break;
						case 12:
							val = (double)iSC;
							break;
						case 13:
							val = fERC;// Round(fERC);
							break;
						case 14:
							val = (double)iBI;
							break;
						case 15:
							val = (double)iKBDI;
							break;
						case 16: //Ignition component
							val = (double)iIC;
							break;
						case 17:
							val = f1;
							break;
						case 18:
							val = f10;
							break;
						case 19:
							val = f100;
							break;
						case 20:
							val = f1000;
							break;
						case 21:
							val = fHerb;
							break;
						case 22:
							val = fWood;
							break;
						case 23:
							if (!wxSet.IsFieldNull(&wxSet.m_SnowFlag))
								val = (double)wxSet.m_SnowFlag;
							else
								goodRecord = false;
							break;
						case 24:
							if(!wxSet.IsFieldNull(&wxSet.m_WDir))// && !wxSet.IsFieldNull(&wxSet.m_WDir))
								val = windDir;
							else
								goodRecord = false;
							break;
						case 25:
							val = sow;
							break;
							//cases 23 - 29 canadian model
						case 26:
							val = ffmc;
							break;
						case 27:
							val = dmc;
							break;
						case 28:
							val = dc;
							break;
						case 29:
							val = isi;
							break;
						case 30:
							val = bui;
							break;
						case 31:
							val = fwi;
							break;
						case 32:
							val = dsr;
							break;
						case 33:
							if(!wxSet.IsFieldNull(&wxSet.m_GustDir))
								val = (double)wxSet.m_GustDir;
							else
								goodRecord = false;
							break;
						case 34:
							if(!wxSet.IsFieldNull(&wxSet.m_GustSpd))
								val = (double)wxSet.m_GustSpd;
							else
								goodRecord = false;
							break;
						case 35:
							if(!wxSet.IsFieldNull(&wxSet.m_SolarRadiation))
								val = (double)wxSet.m_SolarRadiation;
							else
								goodRecord = false;
							break;
						case 36:
							if(!wxSet.IsFieldNull(&wxSet.m_WetFlag))
								val = (double)wxSet.m_WetFlag;
							else
								goodRecord = false;
							break;
						case 37://Dew Point
							if(!wxSet.IsFieldNull(&wxSet.m_RH) && !wxSet.IsFieldNull(&wxSet.m_Temp))
								val = CalcDPT(wxSet.m_Temp, max(wxSet.m_RH, 1));
							else
								goodRecord = false;
							break;
						case 38://Vapor pressure deficit MAX
							if(!wxSet.IsFieldNull(&wxSet.m_RHMin) && !wxSet.IsFieldNull(&wxSet.m_TmpMax))
								val = CalcVPD(max(wxSet.m_RHMin, 1), wxSet.m_TmpMax);
							else
								goodRecord = false;
							break;
						case 39://Vapor pressure deficit Avg
							if(!wxSet.IsFieldNull(&wxSet.m_Temp) && !wxSet.IsFieldNull(&wxSet.m_RH) && !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !wxSet.IsFieldNull(&wxSet.m_TmpMax))
							{
								double dpt = CalcDPT(wxSet.m_Temp, max(wxSet.m_RH, 1));
								double avgT = (wxSet.m_TmpMin + wxSet.m_TmpMax) / 2;
								val = CalcVPDavg(dpt, avgT);
							}
							else
								goodRecord = false;
							break;
						case 40://Live Fuel Index
							if (lfiVal >= 0.0)
								val = lfiVal;// lfi.CalcRunningAvgLFI();
							else
								goodRecord = false;
							break;
						case 41://Wind Azimuth
							if(!wxSet.IsFieldNull(&wxSet.m_WAzimuth))
								val = (double)wxSet.m_WAzimuth;
							else
								goodRecord = false;
							break;
						case 42://Hourly Precip
							if(!wxSet.IsFieldNull(&wxSet.m_HourlyPrecip))
								val = (double)wxSet.m_HourlyPrecip;
							else
								goodRecord = false;
							break;
						case 43:
							if (lfiHerbVal >= 0.0)
								val = lfiHerbVal;// lfiHerb.CalcRunningAvgHerbFM();
							else
								goodRecord = false;
							break;
						case 44:
							if (lfiWoodyVal >= 0.0)
								val = lfiWoodyVal;// lfiWoody.CalcRunningAvgWoodyFM();
							else
								goodRecord = false;
							break;
						case 45:
							if(!wxSet.IsFieldNull(&wxSet.m_Temp) && !wxSet.IsFieldNull(&wxSet.m_RH)
									&& !wxSet.IsFieldNull(&wxSet.m_WS))
								val = theApp.m_NFDRS.iCalcFFWI(wxSet.m_Temp, max(wxSet.m_RH, 1), wxSet.m_WS);
							else
								goodRecord = false;
							break;
						default:
						goodRecord = false;
						}
						if(goodRecord)
						{
							days[j][day].AddValue(val, wxSet.m_ObsDate);
						}
					}
				}
			}
			count++;
			init = 0;
			//init88 = 0;
			wxSet.MoveNext();
		}
	}
	bool doMode;
	for(int j = 0; j < MAXVARIDS; j++)
	{
		if(j == 23)
			doMode = true;
		else
			doMode = false;
		if(days[j])
		{
			for(int i = 0; i < 366; i++)
				days[j][i].CleanStorage(doMode);
		}
	}
/*	if(recCounter && recCounter->Abort())
		ret = -1;*/
	return ret;
}

void CDiurnalAnalysis::AnalyzeSIG(const CString query, CFireplusSet *fpSet)
{
}

void CDiurnalDialog::OnUp()
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

void CDiurnalDialog::OnTop()
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

void CDiurnalDialog::OnDown()
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

void CDiurnalDialog::OnBottom()
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

void CDiurnalDialog::OnMoveleft()
{
	if(destList.GetCurSel() != LB_ERR)
		OnDblclkList2();
}

void CDiurnalDialog::OnMoveright()
{
	if(varList.GetCurSel() != LB_ERR)
		OnDblclkVarlist();
}

void CDiurnalDialog::OnRemoveall()
{
	CString str;
	for(int i = 0; i < destList.GetCount(); i++)
	{
		destList.GetText(i, str);
		int val = destList.GetItemData(i);
		int loc = varList.AddString(str);
		varList.SetItemData(loc, val);
	}
	destList.ResetContent();
}

void CDiurnalDialog::OnSelectall()
{
	CString str;
	for(int i = 0; i < varList.GetCount(); i++)
	{
		varList.GetText(i, str);
		int val = varList.GetItemData(i);
		int loc = destList.AddString(str);
		destList.SetItemData(loc, val);
	}
	varList.ResetContent();
}

void CDiurnalDialog::OnDblclkVarlist()
{
	CString str;
	int cur = varList.GetCurSel();
	varList.GetText(cur, str);
	int val = varList.GetItemData(cur);
	int loc = destList.AddString(str);
	destList.SetItemData(loc, val);
	varList.DeleteString(cur);
}

void CDiurnalDialog::OnDblclkList2()
{
	CString str;
	int cur = destList.GetCurSel();
	destList.GetText(cur, str);
	int val = destList.GetItemData(cur);
	int loc = varList.AddString(str);
	varList.SetItemData(loc, val);
	destList.DeleteString(cur);
}

void CDiurnalDialog::OnVarSort()
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

	climSet.m_strFilter = "[OptionType] >= 0 AND [Daily List] < 1";
	climSet.setVarSortOrder(theApp.varSortOrder);

	climSet.Open();

	varList.ResetContent();

	while (!climSet.IsEOF())
	{
		// make sure it's not already in the destination list
		int loc = destList.FindStringExact(-1, climSet.m_Variable_Name);
		if(loc == LB_ERR)
			varList.AddString(climSet.m_Variable_Name);

		climSet.MoveNext();
	}
	climSet.Close();

		CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));
}
// DiurnalAnalysis2.cpp : implementation file
//
 
#include "stdafx.h"
#include "fireplus.h"
#include <afxrich.h>
#include "richdoc.h"
#include "DiurnalAnalysis2.h"
#include "canada.h"
#include "CustomInitSet.h"
#include <math.h>
#include "LFIengine.h"
#include "ClimateSet.h"
#include "FireDay.h"
#include "perioddoc.h"
//#include "splitterframe.h"
#include "FireSumSet.h"
#include "FireSumSet.h"
#include "ClimAnalysis.h"

extern CFireplusApp theApp;
extern char *abbrevs[];
extern char *varFormats[];
extern double CalcDPT(double tempF, int RH);


#define NOVALUE -9999.9

extern bool IsLeap(int year);

// CDiurnalAnalysis2
void DayOfYearToDateTime(int year, int dayOfYear, int hour, int minute, COleDateTime * dt)
{
	//need to transform 0 based day of year to month/day
	COleDateTimeSpan span(dayOfYear, hour, minute, 0);
	dt->SetDateTime(year, 1, 1, 0, 0, 0);
	*dt = *dt + span;
}
CDiurnalAnalysis2::CDiurnalAnalysis2()
{
	vals = NULL;
	//recCounter = NULL;
	count = 0;
	varIDs = NULL;
	nVarIDs = 0;
	staName = "";
	modelStr = "";
	stamp = "";
	startYear = endYear = nYears = 0;

}

CDiurnalAnalysis2::~CDiurnalAnalysis2()
{
	if(vals)
	{
		for(int v = 0; v < MAXVARIDS; v++)
		{
			if(vals[v])
			{
				for(int y = 0; y < nYears; y++)
				{
					for(int d = 0; d < 366; d++)
					{
						if(vals[v][y][d])
							delete[] vals[v][y][d];
					}
					delete[] vals[v][y];
				}
				delete[] vals[v];
			}
		}
		delete[] vals;
	}
	if(varIDs)
		delete[] varIDs;
}


// CDiurnalAnalysis2 member functions
int CDiurnalAnalysis2::Analyze(CWnd *_caller, CFireplusSet *fpSet, int nVars, int *_varIDs)
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
	nVarIDs = nVars;
	varIDs = _varIDs;
	//model use flags - optimize to only calculate models used
	//killPeriods = true;
	//fpSet = _fpSet;
	useCanadian = false;
	useNFDRS = false;
	useLFI = false;
	count = 0;
	startYear = fpSet->m_StartYear;
	endYear = fpSet->m_EndYear;
	//int pLen = 1;//atoi(fpSet->m_PeriodLength);
	int v;
	for(v = 0; v < nVarIDs; v++)
	{
		//create data storage structure
		if(OptionFromVarID(varIDs[v]) == 1)
			useNFDRS = true;
		if(OptionFromVarID(varIDs[v]) == 3)
			useCanadian = true;
		if(varIDs[v] == 39)
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
		startYear = wxSet.m_ObsDate.GetYear();
		wxSet.MoveLast();
		endYear = wxSet.m_ObsDate.GetYear();
		nYears = endYear - startYear + 1;
		vals = new double ***[MAXVARIDS];
		for(v = 0; v < MAXVARIDS; v++)
		{
			vals[v] = NULL;
		}
		for(v = 0; v < nVarIDs; v++)
		{
			vals[varIDs[v]] = new double **[nYears];
			for(int y = 0; y < nYears; y++)
			{
				vals[varIDs[v]][y] = new double *[366];
				for(int d = 0; d < 366; d++)
				{
					vals[varIDs[v]][y][d] = new double[24];
					for(int h = 0; h < 24; h++)
						vals[varIDs[v]][y][d][h] = -1.0;
				}
			}
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

CString CDiurnalAnalysis2::AddDateStamp()
{
	COleDateTime dateTime = COleDateTime::GetCurrentTime();
	CString dStr = dateTime.Format("%m/%d/%Y-%H:%M");
	CString ret;
	ret.Format("FF+%s %s %s", verStr, theApp.m_buildDateStrPacked, dStr);
	return ret;
	//COleDateTime dateTime = COleDateTime::GetCurrentTime();
	//return dateTime.Format("FF+3.0 %m/%d/%Y-%H:%M"); 

}

CString CDiurnalAnalysis2::GetModelString(CFireplusSet *fpSet, CSIGStationSet *stn)
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

int CDiurnalAnalysis2::AnalyzeStation(CString stationStr, CSIGStationSet& staSet, CWxSet& wxSet)
{
	int ret = 0;
	/*if(recCounter)
	{
		recCounter->SetStatusString("Reading: " + stationStr);
	}*/
	//model use flags - optimize to only calculate models used
	bool goodRecord;	
	//create data structures based on periods and selected outputs
	//LFI goodies
	CLFIengine lfi(wxSet.m_pDatabase), lfiHerb(wxSet.m_pDatabase, HerbParams, staSet.m_StationID, staSet.m_HerbAnnual), 
		lfiWoody(wxSet.m_pDatabase, WoodyParams, staSet.m_StationID);
	//NOW CREATE NFDRCALC MODEL
//	NFDRCalc nfdrs;
	int sKBDI = 0;
	if(!staSet.IsFieldNull(&staSet.m_StartKBDI))
		sKBDI = staSet.m_StartKBDI;

	//NFDR2016Calc *nfdr2016;
	double pptAcc = 0;
	if(useNFDRS)
	{
		int lookGreen, lookFreeze;
		lookGreen = DayOfYear(staSet.m_GreenJulian);
		lookFreeze = DayOfYear(staSet.m_FreezeJulian);

		if (isNFDRS2016(staSet.m_NFDRSFM[0]) || useLFI)
		{ // new NFDRS2016 calc
			//nfdr2016 = new NFDR2016Calc((int)staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true);
			//theApp.m_NFDRS2016.Init((int)staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true, lfiHerb.GetHerbMaxGSI(), lfiHerb.GetHerbGreenup());
			theApp.m_NFDRS2016.Init(staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true, staSet.m_HerbAnnual, staSet.m_KBDIThreshold);
			theApp.m_NFDRS2016.iSetFuelModel(staSet.m_NFDRSFM[0]);
			theApp.m_NFDRS2016.SetGSIParams(lfi.GetHerbMaxGSI(), lfi.GetHerbGreenup(), lfi.GetTminMin(), lfi.GetTminMax(), lfi.GetVPDMin(), lfi.GetVPDMax(), 
				lfi.GetDaylenMin(), lfi.GetDaylenMax(), lfi.GetMAPeriod(), lfi.IsUsingVPDAvg(), lfi.GetNumPrecipDays(), lfi.GetRTPcpMin(), lfi.GetRTPcpMax(), lfi.GetUseRTPrecip());
			theApp.m_NFDRS2016.SetHerbGSIparams(lfiHerb.GetHerbMaxGSI(), lfiHerb.GetHerbGreenup(), lfiHerb.GetTminMin(), lfiHerb.GetTminMax(), 
				lfiHerb.GetVPDMin(), lfiHerb.GetVPDMax(), lfiHerb.GetDaylenMin(), lfiHerb.GetDaylenMax(), lfiHerb.GetMAPeriod(), lfiHerb.IsUsingVPDAvg(), 
				lfiHerb.GetNumPrecipDays(), lfiHerb.GetRTPcpMin(), lfiHerb.GetRTPcpMax(), lfiHerb.GetUseRTPrecip());
			theApp.m_NFDRS2016.SetWoodyGSIparams(lfiWoody.GetWoodyMaxGSI(), lfiWoody.GetWoodyGreenup(), lfiWoody.GetTminMin(), lfiWoody.GetTminMax(), 
				lfiWoody.GetVPDMin(), lfiWoody.GetVPDMax(), lfiWoody.GetDaylenMin(), lfiWoody.GetDaylenMax(), lfiWoody.GetMAPeriod(), lfiWoody.IsUsingVPDAvg(), 
				lfiWoody.GetNumPrecipDays(), lfiWoody.GetRTPcpMin(), lfiWoody.GetRTPcpMax(), lfiWoody.GetUseRTPrecip());
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
	COleDateTime wxDay;
	COleDateTime yesterday;//used to skip duplicates
	wxSet.Requery();
	if(!wxSet.IsBOF())
	{
		//working variables
		int tmp, tmpMax, tmpMin, rh, rhMax, rhMin, pptDur, sow, greenHerb, greenShrub,
			season, windSpd, windDir, slopeClass, wetFlag, snowFlag;
		double pptAmt, omc10; 
		while(!wxSet.IsEOF())// && (recCounter && !recCounter->Abort()))    
		{
			if(((useNFDRS || useCanadian) && (wxSet.IsFieldNull(&wxSet.m_Temp) || wxSet.IsFieldNull(&wxSet.m_RH))
				|| wxSet.IsFieldNull(&wxSet.m_ObsDate)))
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
				
					//double solarRad = wxSet.m_SolarRadiation;

					theApp.m_NFDRS2016.Update(year1, month1, day1, hour1, julian1, tmp, tmpMin, tmpMax, 
						rh, rhMin, hourlyPrecip, pptAmt, solarRad, windSpd, snowFlag, 
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
					if(vals[j])
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
							int y = wxSet.m_ObsDate.GetYear() - startYear;
							int d = DayOfYear(wxSet.m_ObsDate) - 1;
							int h = wxSet.m_ObsDate.GetHour();
							vals[j][y][d][h] = val;
							//days[j][day].AddValue(val, wxSet.m_ObsDate);
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
	/*for(int j = 0; j < MAXVARIDS; j++)
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
	}*/
	/*if(recCounter && recCounter->Abort())
		ret = -1;*/
	return ret;
}

void CDiurnalAnalysis2::LocateEvents(QueryRow *qRows, int rows, int hours, CFireplusSet *fpSet)
{
	double **qvals, *compares;
	compares = new double[rows];
	qvals = new double *[rows];
	int i;

	for(i = 0; i < rows; i++)
		qvals[i] = new double[hours];
	CString *userfmts = NULL, *userAbbrevs = NULL;
/*	int uVars = HasUserVars();
	if(uVars)
	{
		userfmts = new CString[uVars];
		userAbbrevs = new CString[uVars];
		CUserVarSet uvs(pDB);
		uvs.Open();
		int loc = 0;
		for(int v = 0; v < nVarIDs; v++)
		{
			if(varIDs[v] >= STATICVARIDS)
			{
				uvs.m_strFilter.Format("[VarID] = %d", varIDs[v] + 1);
				uvs.Requery();
				userfmts[loc].Format("5.%df", uvs.m_Decimals);
				userAbbrevs[loc].Format("%5.5s", uvs.m_Abbrev);
				loc++;
			}
		}
		uvs.Close();
	}*/
	//int pLen = hours;
	bool isSIG = strlen(fpSet->m_SIG_Station) > 6 ? true : false;
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	//output header
	fprintf(stream, "FireFamily Plus Event Locator Report\n");
	fprintf(stream, "   Listing of Selected Events\n");

// added for batch
	// (code interspersed)
//	CReportOptionsSet reportSet(fpSet->m_pDatabase);
//	reportSet.Open();

	CString dStr;


	
	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
//	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	//reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch

	
	fprintf(stream, "\nEvent Definition:\n");//, hours);
	for(i = 0; i < rows; i++)
	{
		CString str1, str2;
		fprintf(stream, "    ");
		if(i > 0)
			fprintf(stream, "%s ", (qRows[i].rowOp == AND) ? "AND" : "OR");
		if(hours > 0)
		{	
			if(qRows[i].cat == Sum)
				str1.Format("Sum(%s)", qRows[i].varString.Trim());
			else if(qRows[i].cat == Avg)
				str1.Format("Avg(%s)", qRows[i].varString.Trim());
			else if(qRows[i].cat == Min)
				str1.Format("Min(%s)", qRows[i].varString.Trim());
			else if(qRows[i].cat == Max)
				str1.Format("Max(%s)", qRows[i].varString.Trim());
			else if(qRows[i].cat == Daily)
				str1.Format("Hourly(%s)", qRows[i].varString.Trim());
		}
		else
			str1.Format("%s", qRows[i].varString.Trim());
		fprintf(stream, "%s ", str1);
		if(qRows[i].valOp == EQ)
			fprintf(stream, "= ");
		else if(qRows[i].valOp == LE)
			fprintf(stream, "<= ");
		else if(qRows[i].valOp == GE)
			fprintf(stream, ">= ");
		else if(qRows[i].valOp == LT)
			fprintf(stream, "< ");
		else if(qRows[i].valOp == GT)
			fprintf(stream, "> ");
		compares[i] = qRows[i].value;
		if(qRows[i].valType == Value)
			fprintf(stream, "%.2f\n", qRows[i].value);
		else
		{
			fprintf(stream, "%.2f Percentile ", qRows[i].value);
			//replace value with critical percentile value
			Stats pStats;
			for(int y = 0; y < nYears; y++)
			{
				//for each year...
				for(int d = 0; d < 366; d++)
				{
					//for each day
					for(int h = 0; h < 24; h++)//for each hour
					{
						if(vals[varIDs[i]][y][d][h] >= 0)
							pStats.Accumulate(vals[varIDs[i]][y][d][h]); 
					}
				}
			}
			pStats.Complete();
			compares[i] = pStats.Percentile(qRows[i].value / 100.0);
			fprintf(stream, "(%.2f)\n", compares[i]);
			pStats.Reset();
		}
	}
	fprintf(stream, "\n         %d-Hour Periods\n\n", hours);
	//header
	fprintf(stream, "DATE          ");
	int uloc = 0;
	for(i = 0; i < nVarIDs; i++)
	{
		if(varIDs[i] < STATICVARIDS)
			fprintf(stream, "  %5.5s", abbrevs[varIDs[i]]);
		else
		{
			fprintf(stream, " %5.5s", userAbbrevs[uloc]);
			uloc++;
		}
	}
	fprintf(stream, "\n-----------------");
	for(i = 0; i < nVarIDs; i++)
		fprintf(stream, "-------");
	fprintf(stream, "\n");
	//now cycle thru data, get values and check conditions
	bool hit, noVal;
	double *rVals = new double[rows];
	bool *rHits = new bool[rows];
	long hitCount = 0L, binCount = 0L, rejectCount = 0;
	double **valArrays = new double *[nVarIDs];
	long nVals = nYears * 366 * 24;
	for(int v = 0; v < nVarIDs; v++)
	{
		valArrays[v] = new double[nVals];
		for(int i = 0; i < nVals; i++)
			valArrays[v][i] = -1.0;
	}
	long loc = 0;
	for(int y = 0; y < nYears; y++)
	{
		//for each year...
		for(int d = 0; d < 366; d++)
		{	//for each day
			for(int h = 0; h < 24; h++)
			{
				//for each hour...
				for(int v = 0; v < nVarIDs; v++)
				{
					if(vals[varIDs[v]][y][d][h] >= 0)
						valArrays[v][loc] = vals[varIDs[v]][y][d][h];
				}
				loc++;
			}
		}
	}
	//ok, have arrays of values, cycle thru looking for conditions being met...
	for(loc = 0; loc < nVals; loc++)
	{
		for(int v = 0; v < nVarIDs; v++)
		{
			for(int h = 0; h < hours; h++)
			{
				if(valArrays[v][loc + h] >= 0)
					qvals[v][h] = valArrays[v][loc + h];
				else
					qvals[v][h] = NOVALUE;
			}
		}
		//HAVE BINNED DATA, NOW CHECK FOR CONDITIONS
		hit = true;
		noVal = false;
		for(int v = 0; v < rows; v++)
		{ //check each row
			double rVal = 0.0;
			hit = true;
			for(int p = 0; p < hours; p++)
			{
				if(qvals[v][p] == NOVALUE)
				{
					hit = false;
					noVal = true;
					rVal = -1.0;					
					break;
				}
				else
				{
					switch(qRows[v].cat)
					{
					case Sum:
					case Avg:
						rVal += qvals[v][p];
						break;
					case Max:
						if(p == 0)
							rVal = qvals[v][p];
						else
							rVal = max(rVal, qvals[v][p]);
						break;
					case Min:
						if(p == 0)
							rVal = qvals[v][p];
						else
							rVal = min(rVal, qvals[v][p]);
						break;
					case Daily:
						rVal = qvals[v][p];
						switch(qRows[v].valOp)
						{
						case EQ:
							if(rVal != compares[v])
								hit = false;
							break;
						case LE:
							if(rVal > compares[v])
								hit = false;
							break;
						case GE:
							if(rVal < compares[v])
								hit = false;
							break;
						case LT:
							if(rVal >= compares[v])
								hit = false;
							break;
						case GT:
							if(rVal <= compares[v])
								hit = false;
							break;
						}//end valOp switch
					}//end cat switch
				}//end scan of days
			}
			if(hit && qRows[v].cat != Daily)
			{
				if(qRows[v].cat == Avg)
					rVal /= hours;
				switch(qRows[v].valOp)
				{
				case EQ:
					if(rVal != compares[v])
						hit = false;
					break;
				case LE:
					if(rVal > compares[v])
						hit = false;
					break;
				case GE:
					if(rVal < compares[v])
						hit = false;
					break;
				case LT:
					if(rVal >= compares[v])
						hit = false;
					break;
				case GT:
					if(rVal <= compares[v])
						hit = false;
					break;
				}
			}
			rHits[v] = hit;
			rVals[v] = rVal;
			// 2014
			//if(qRows[v].cat == Daily)
			//	rVals[v] = (hit == true) ? 1.0 : 0.0;
		}
		//rows done, now use rowOps to see if conditions met
		hit = rHits[0];
		for(int v = 1; v < rows; v++)
		{
			if(qRows[v].rowOp == AND)
				hit = hit && rHits[v];
			else
				hit = hit || rHits[v];
		}
		if(hit)
		{//all conditions met
			//figure out date/time
			int yr, hr;
			yr = loc / (366 * 24);
			int doy;// = (loc - (yr * 366 * 24)) / 24 + 1;
			double fDoy = ((double)(loc - (yr * 366 * 24))) / 24.0;// - 1.0;
			if(!IsLeap(yr + startYear))
			{
				if(fDoy > 58)
					fDoy -= 1.0;
			}
			double fHr = modf(fDoy, &fDoy);
			hr = Round(fHr * 24.0);//loc - (yr * 366 * 24) - doy * 24;
			doy = (int) fDoy;
			COleDateTime dt;
			//dt.SetDateTime(startYear + yr, 1, 1, 1, 0, 0);
			DayOfYearToDateTime(startYear + yr, doy, hr, 0, &dt);	
			fprintf(stream, "%02d/%02d/%4d:%02d%02d", dt.GetMonth(), dt.GetDay(), dt.GetYear(), dt.GetHour(), dt.GetMinute());
					//periods[varIDs[0]][i]->start.GetMonth(), periods[varIDs[0]][i]->start.GetDay(),
					//periods[varIDs[0]][i]->baseYear + y);
			for(int v = 0; v < rows; v++)
			{
				// 2014
				//if(qRows[v].cat == Daily)
				//	fprintf(stream, rHits[v] == true ? "  TRUE " : " FALSE ");
				//else
				//{
					if(rVals[v] >= 0.0)
						fprintf(stream, " %6.2f", rVals[v]);
					else
						fprintf(stream, "   NA  ");
				//}
			}
			fprintf(stream, "\n");
			hitCount++;
			//increment i to prevent overlap
			i += hours - 1;
		}
		if(!hit && noVal)
		{
			//make sure in fpSet time of year, we've got storage for the entire year...
			COleDateTime sDate, eDate;
			sDate.SetDateTime(2004, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay, 0, 0, 0);
			eDate.SetDateTime(2004, MonthChtoInt(fpSet->m_EndMonth), fpSet->m_EndDay, 23, 59, 59);
			int yr = loc / (366 * 24);
			double fDoy = ((double)(loc - (yr * 366 * 24))) / 24.0 - 1.0;
			/*if(!IsLeap(yr + startYear))
			{
				if(fDoy > 58)
					fDoy -= 1.0;
			}*/
			int doy = (int) fDoy;
			COleDateTime dt;
			DayOfYearToDateTime(2004, doy, 1, 0, &dt);	
			//now see if was a day in working set...
			if(sDate <= eDate)
			{
				if(dt >= sDate && dt <= eDate)
					rejectCount++;
			}
			else if(eDate < sDate)
			{
				if(dt <= eDate || dt >= sDate)
					rejectCount++;
			}
		}
		//make sure in fpSet time of year, we've got storage for the entire year...
		COleDateTime sDate, eDate;
		sDate.SetDateTime(2004, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay, 0, 0, 0);
		eDate.SetDateTime(2004, MonthChtoInt(fpSet->m_EndMonth), fpSet->m_EndDay, 23, 59, 59);
		int yr = loc / (366 * 24);
		double fDoy = ((double)(loc - (yr * 366 * 24))) / 24.0 - 1.0;
		/*if(!IsLeap(yr + startYear))
		{
			if(fDoy > 58)
				fDoy -= 1.0;
		}*/
		int doy = (int) fDoy;
		COleDateTime dt;
		DayOfYearToDateTime(2004, doy, 1, 0, &dt);	
		//now see if was a day in working set...
		if(sDate <= eDate)
		{
			if(dt >= sDate && dt <= eDate)
				binCount++;
		}
		else if(eDate < sDate)
		{
			if(dt <= eDate || dt >= sDate)
				binCount++;
		}
		//binCount++;
	}
	fprintf(stream, "\n%ld hits out of %ld\n", hitCount, binCount);
	fprintf(stream, "%ld rejects for no/missing observations\n", rejectCount);
	//if(isSIG)
	//	AddSIGDescription(stream);
	fprintf(stream, "\n%s\n", AddDateStamp());
	if(valArrays)
	{
		for(int v = 0; v < nVarIDs; v++)
			delete[] valArrays[v];
		delete[] valArrays;
	}
	if(userfmts)
		delete[] userfmts;
	if(userAbbrevs)
		delete[] userAbbrevs;
	delete[] compares;
	for(i = 0; i < rows; i++)
		delete[] qvals[i];
	delete[] qvals;
	delete[] rVals;
	delete[] rHits;
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
				tTitle.Format("%s - Events", fpSet->m_SIG_Station);
				pDoc->SetTitle(tTitle);
				return;
			}
		}
	}
	free(fName);

}

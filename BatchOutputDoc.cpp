// BatchOutputDoc.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "SIGStationSet.h"
#include "ClimateSet.h"
#include "wxSet.h"
#include "fireplusSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "splitterframe.h"
#include "FireSumSet.h"
#include "ClimAnalysis.h"
#include "BatchOutputDoc.h"
#include "FireStats.h"
#include "BatchRunsSet.h"
#include "GraphSet.h"
//#include "RunSave.h"
#include <direct.h>
#include "BatchOutputFrame.h"
#include ".\batchoutputdoc.h"
#include "PocketCardSet.h"
#include "PocketCardDialog.h"
#include "OverlayDialog.h"
#include "FireOptionsSet.h"
#include "DistinctRunReportsSet.h"
#include "RunOptionsSet.h"
#include "RunPocketCardSet.h"
#include "PocketCardDoc.h"
#include "PocketCardFrame.h"
#include "PocketCardView.h"
#include "graphset.h"
#include "SeasonBinSet.h"
#include "CustomInitSet.h"
#include "StationInSIGSet.h"
#include "SIGSet.h"
#include "LFIengine.h"
#include "LogReg.h"
#include "FireSummary.h"
#include "NFDRS2016DFMSet.h"
#include "NFDRS2016WxSet.h"

extern CFireplusApp theApp;
//extern int curr_runID;
extern bool IsLeap(int year);
const double NODATA = -999.0;

CRawWxData::CRawWxData(int nYears, int _varID, CString _varName, int _optionType)
{
	varID = _varID;
	varName = _varName;
	m_vals = NULL;
	m_optionType = _optionType;
	AllocData(nYears);	
}

CRawWxData::~CRawWxData()
{
	DestroyData();
	
}

void CRawWxData::AllocData(int nYears)
{
	DestroyData();
	m_nYears = nYears;
	if(m_nYears > 0)
	{
		m_vals = new float*[m_nYears];
		for(int y = 0; y < m_nYears; y++)
		{
			m_vals[y] = new float[366];
			for(int i = 0; i < 366; i++)
				m_vals[y][i] = NODATA_VAL;
		}
	}
}

void CRawWxData::DestroyData()
{
	if(m_vals)
	{
		for(int y = 0; y < m_nYears; y++)
			delete[] m_vals[y];
		delete[] m_vals;
	}
	m_vals = NULL;
}

CRawClim::CRawClim(CDatabase *db, CString id)
{
	m_nVars = m_nYears = m_baseYear = 0;
	m_pDB = db;
	m_sigStaID = id;
	m_wxData = NULL;
	m_fpSet = NULL;
	m_climSet = NULL;
	m_fires = NULL;
	m_recsDone = 0;
	m_maxPossibleRecs = 1;
	m_baseYear2016 = m_nYears2016 = -999;
}

CRawClim::~CRawClim()
{
	DestroyData();
}


void CRawClim::AllocData()
{
	m_recsDone = 0;
	m_maxPossibleRecs = 1;
	pCUPDUPData->SetProgress("Reading: " + m_sigStaID);
	DestroyData();
	m_climSet = new CClimateSet(m_pDB);
	m_climSet->m_strSort = _T("[VarID]");
	m_climSet->Open();
	m_nVars = 0;
	while(!m_climSet->IsEOF())
	{
		m_nVars++;
		if (m_climSet->m_cp2 < m_climSet->m_CriticalPercentile)
			m_extremeIsMax[m_climSet->m_VarID - 1] = false;
		else
			m_extremeIsMax[m_climSet->m_VarID - 1] = true;
		m_climSet->MoveNext();
	}
	CWxSet wxSet(m_pDB);
	//WxSetOR wxSet(m_pDB);
	wxSet.m_strSort = _T("[ObsDate]");
	if(m_sigStaID.GetLength() > 6)//SIG
	{
		int nStns = 0;
		CStationInSIGSet sigSet(m_pDB);
		char tmpStr[64];
		strcpy_s(tmpStr, m_sigStaID);
		sigSet.m_strFilter.Format("[SIG] = '%s'", &tmpStr[6]);
		sigSet.Open();
		while(!sigSet.IsEOF())
		{
			nStns++;
			sigSet.MoveNext();
		}
		if(nStns > 0)
		{

			sigSet.MoveFirst();
			double *weights = new double[nStns];
			CRawClim **tClims = new CRawClim*[nStns];
			int cLoc = 0;
			while(!sigSet.IsEOF())
			{
				tClims[cLoc] = new CRawClim(m_pDB, sigSet.m_StationID);
				tClims[cLoc]->pCUPDUPData = pCUPDUPData;
				tClims[cLoc]->AllocData();
				weights[cLoc] = sigSet.m_WeightFactor;
				sigSet.MoveNext();
				cLoc++;
			}
			//got numbers, now create SIG observations...
			//create storage first...
			m_baseYear = tClims[0]->m_baseYear;
			m_nYears = tClims[0]->m_nYears;
			for(cLoc = 1; cLoc < nStns; cLoc++)
			{
				double wSum = 0.0;
				if(tClims[cLoc])
				{
					int eYear = m_baseYear + m_nYears - 1;
					if(tClims[cLoc]->m_baseYear + tClims[cLoc]->m_nYears - 1 > eYear)
						eYear = tClims[cLoc]->m_baseYear + tClims[cLoc]->m_nYears - 1;
					if(m_baseYear > tClims[cLoc]->m_baseYear)
						m_baseYear = tClims[cLoc]->m_baseYear;
					m_nYears = eYear - m_baseYear + 1;
				}
			}
			m_wxData = new CRawWxData*[m_nVars];
			m_climSet->MoveFirst();
			for(int v = 0; v < m_nVars; v++)
			{//get raw data for each station
				m_wxData[v] = new CRawWxData(m_nYears, m_climSet->m_VarID, m_climSet->m_Variable_Name, m_climSet->m_OptionType);
				m_climSet->MoveNext();
			}
			//build SIG data from raw data
			//for each day...
			for(int d = 0; d < 366; d++)
			{
				//for each year
				for(int y = 0; y < m_nYears; y++)
				{
					//for each variable...
					for(int v = 0; v < m_nVars; v++)
					{
						double wSum = 0.0, value = 0.0;	
						int year = m_baseYear;
						//for each station, accumulate observed values * weight for station
						for(int s = 0; s < nStns; s++)
						{
							int stationYear = m_baseYear - tClims[s]->m_baseYear + y;
							if(stationYear >= 0 && stationYear < tClims[s]->m_nYears)
							{
								if(tClims[s]->m_wxData[v]->m_vals[stationYear][d] != NODATA_VAL)
								{
									value += weights[s] * tClims[s]->m_wxData[v]->m_vals[stationYear][d];
									wSum += weights[s];
								}
							}
						}
						if(wSum > 0.0) //has some data
						{
							m_wxData[v]->m_vals[y][d] = value / wSum;
						}
						//else //already assigned in AllocData
							//m_wxData[v]->m_vals[y][d] 
					}
				}
			}

			//cleanup time...
			for(cLoc = 0; cLoc < nStns; cLoc++)
			{
				if(tClims[cLoc])
					delete tClims[cLoc];
			}
			delete[] tClims;
			delete[] weights;
		}
		sigSet.Close();
		m_fpSet = new CFireplusSet(m_pDB);
		m_fpSet->Open();
		m_fpSet->AddNew();
		m_fpSet->m_Description = _T("Quick Batch");
		m_fpSet->m_EndDay = 31;
		m_fpSet->m_EndMonth = "December";
		m_fpSet->m_EndYear = m_baseYear + m_nYears - 1;
		m_fpSet->m_SIG_Station = m_sigStaID;
		m_fpSet->m_StartDay = 1;
		m_fpSet->m_StartMonth = "January";
		m_fpSet->m_StartYear = m_baseYear;
		m_fpSet->m_Use78 = TRUE;
		m_fpSet->m_Use88 = TRUE;
		m_fpSet->m_UseCanadian = TRUE;
		m_fpSet->m_PeriodLength = "1";
		m_fpSet->Update();
		m_fpSet->MoveLast();
		CSIGSet SIGSet(m_pDB);
		SIGSet.m_strFilter.Format("[SIG] = '%s'", &tmpStr[6]);
		//sigSet.m_strFilter.Format("[SIG] = '%s'", );
		SIGSet.Open();
		m_StnName = SIGSet.m_DESCRIPTION;
		SIGSet.Close();
	}
	else // single station...
	{
		m_usedExtremes = theApp.m_UseDailyExtremes;
		CSIGStationSet staSet(m_pDB);
		staSet.m_strFilter.Format("[StationID] = '%6.6s'", this->m_sigStaID);
		staSet.Open();
		// updated 09/2012 with the new DailyObs field in WxObs table
		bool reCalc = false;
		reCalc = theApp.m_ForceNFDRS2016Recompute;
		if (!isNFDRS2016(staSet.m_NFDRSFM[0]))
			reCalc = false;
		COleDateTime n2Start, n2End;
		CFireplusSet tSet(this->m_pDB);
		tSet.GetNFDRS2016Range(m_sigStaID, &n2Start, &n2End);
		if (!reCalc && isNFDRS2016(staSet.m_NFDRSFM[0]))
		{//check wxSet for previously run NFDRS2016
			//need to query data by valid NFDRS2016 range
			if (n2Start.GetStatus() == COleDateTime::valid && n2End.GetStatus() == COleDateTime::valid)
			{
				wxSet.m_strFilter.Format("[StationID] = '%6.6s' AND [DailyObs] = 1 AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", m_sigStaID,
					n2Start.Format(), n2End.Format());
				wxSet.Open();
				if (wxSet.IsFieldNull(&wxSet.m_FM1) || wxSet.IsFieldNull(&wxSet.m_FM10) || wxSet.IsFieldNull(&wxSet.m_FM100) || wxSet.IsFieldNull(&wxSet.m_FM1000) || wxSet.IsFieldNull(&wxSet.m_FuelTemperature)
					|| wxSet.m_FM1 <= 0.0 || wxSet.m_FM10 <= 0.0 || wxSet.m_FM100 <= 0.0 || wxSet.m_FM1000 <= 0.0 || wxSet.m_FuelTemperature <= -999.0)
					reCalc = true;
				if (!reCalc)
				{
					wxSet.MoveLast();
					if (wxSet.IsFieldNull(&wxSet.m_FM1) || wxSet.IsFieldNull(&wxSet.m_FM10) || wxSet.IsFieldNull(&wxSet.m_FM100) || wxSet.IsFieldNull(&wxSet.m_FM1000) || wxSet.IsFieldNull(&wxSet.m_FuelTemperature)
						|| wxSet.m_FM1 <= 0.0 || wxSet.m_FM10 <= 0.0 || wxSet.m_FM100 <= 0.0 || wxSet.m_FM1000 <= 0.0 || wxSet.m_FuelTemperature <= -999.0)
						reCalc = true;
				}
				wxSet.Close();
			}
		}
		if ((isNFDRS2016(staSet.m_NFDRSFM[0]) && reCalc) || (m_usedExtremes && isNFDRS2016(staSet.m_NFDRSFM[0]))) // we need hourly data for new calculator
			wxSet.m_strFilter.Format("[StationID] = '%6.6s' AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", m_sigStaID,
				n2Start.Format(), n2End.Format());
		else if (!reCalc && isNFDRS2016(staSet.m_NFDRSFM[0]))
		{
			wxSet.m_strFilter.Format("[StationID] = '%6.6s' AND [DailyObs] = 1 AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", m_sigStaID,
				n2Start.Format(), n2End.Format());
		}
		else
			wxSet.m_strFilter.Format("[StationID] = '%s' AND [DailyObs] = 1", m_sigStaID);
		wxSet.Open();
		if (!wxSet.IsBOF() && !wxSet.IsEOF())
		{
			int eYear;
			eYear = m_baseYear = wxSet.m_ObsDate.GetYear();
			wxSet.MoveLast();
			eYear = wxSet.m_ObsDate.GetYear();
			m_nYears = eYear - m_baseYear + 1;
			m_maxPossibleRecs = m_nYears * 366 * 24;
		}

		if (HasUserTables(m_pDB))
		{
			/*	CString strSQL;
				strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%s'", sSet.m_StationID);
				userVals.m_strSort = _T("[ObsDate]");
				userVals.Open(CRecordset::dynaset, strSQL);
				if(!userVals.IsBOF() && !userVals.IsEOF())//has records
				{	//set years for spinners
					//CTime dt;
					CDBVariant var;
					userVals.MoveFirst();
					userVals.GetFieldValue("ObsDate", var);
					CTime dt(var.m_pdate->year, var.m_pdate->month, var.m_pdate->day, var.m_pdate->hour,
						var.m_pdate->minute, var.m_pdate->second);
					//dt = var.m_boolVal;
					yr1 = min(yr1, dt.GetYear());
					userVals.MoveLast();
					userVals.GetFieldValue("ObsDate", var);
					CTime dt2(var.m_pdate->year, var.m_pdate->month, var.m_pdate->day, var.m_pdate->hour,
						var.m_pdate->minute, var.m_pdate->second);
					yr2 = max(yr2, dt2.GetYear());
				}
				userVals.Close();*/
		}

		CLFIengine lfi(m_pDB, 0, m_sigStaID), lfiHerb(m_pDB, HerbParams, m_sigStaID, staSet.m_HerbAnnual), lfiWoody(m_pDB, WoodyParams, m_sigStaID);
		m_wxData = new CRawWxData *[m_nVars];
		m_climSet->MoveFirst();
		for (int v = 0; v < m_nVars; v++)
		{
			m_wxData[v] = new CRawWxData(m_nYears, m_climSet->m_VarID, m_climSet->m_Variable_Name, m_climSet->m_OptionType);
			m_climSet->MoveNext();
		}
		m_fpSet = new CFireplusSet(m_pDB);
		m_fpSet->Open();
		m_fpSet->AddNew();
		m_fpSet->m_Description = _T("Quick Batch");
		m_fpSet->m_EndDay = 31;
		m_fpSet->m_EndMonth = "December";
		m_fpSet->m_EndYear = m_baseYear + m_nYears - 1;
		m_fpSet->m_SIG_Station = m_sigStaID;
		m_fpSet->m_StartDay = 1;
		m_fpSet->m_StartMonth = "January";
		m_fpSet->m_StartYear = m_baseYear;
		m_fpSet->m_Use78 = TRUE;
		m_fpSet->m_Use88 = TRUE;
		m_fpSet->m_UseCanadian = TRUE;
		m_fpSet->m_PeriodLength = "1";
		m_fpSet->Update();
		m_fpSet->MoveLast();
		//now get the data...
		m_StnName = staSet.m_Name;
		int lookGreen, lookFreeze, sKBDI = 0;
		if (!staSet.IsFieldNull(&staSet.m_StartKBDI))
			sKBDI = staSet.m_StartKBDI;
		lookGreen = DayOfYear(staSet.m_GreenJulian);
		lookFreeze = DayOfYear(staSet.m_FreezeJulian);


		//NFDR2016Calc *nfdr2016;
		bool useLFI = true;
		if (isNFDRS2016(staSet.m_NFDRSFM[0]) || useLFI)
		{ // new NFDRS2016 calc
			theApp.m_NFDRS2016.Init(staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true, staSet.m_HerbAnnual, staSet.m_KBDIThreshold);
			theApp.m_NFDRS2016.iSetFuelModel(staSet.m_NFDRSFM[0]);
			theApp.m_NFDRS2016.SetGSIParams(lfi.GetHerbMaxGSI(), lfi.GetHerbGreenup(), lfi.GetTminMin(), lfi.GetTminMax(), lfi.GetVPDMin(), lfi.GetVPDMax(), 
				lfi.GetDaylenMin(), lfi.GetDaylenMax(), lfi.GetMAPeriod(), lfi.IsUsingVPDAvg(), lfi.GetNumPrecipDays(), lfi.GetRTPcpMin(), lfi.GetRTPcpMax(), lfi.GetUseRTPrecip());
			theApp.m_NFDRS2016.SetHerbGSIparams(lfiHerb.GetHerbMaxGSI(), lfiHerb.GetHerbGreenup(), lfiHerb.GetTminMin(), lfiHerb.GetTminMax(), lfiHerb.GetVPDMin(), 
				lfiHerb.GetVPDMax(), lfiHerb.GetDaylenMin(), lfiHerb.GetDaylenMax(), lfiHerb.GetMAPeriod(), lfiHerb.IsUsingVPDAvg(), lfiHerb.GetNumPrecipDays(), 
				lfiHerb.GetRTPcpMin(), lfiHerb.GetRTPcpMax(), lfiHerb.GetUseRTPrecip(), lfiHerb.GetHerbMin(), lfiHerb.GetHerbMax());
			theApp.m_NFDRS2016.SetWoodyGSIparams(lfiWoody.GetWoodyMaxGSI(), lfiWoody.GetWoodyGreenup(), lfiWoody.GetTminMin(), lfiWoody.GetTminMax(), 
				lfiWoody.GetVPDMin(), lfiWoody.GetVPDMax(), lfiWoody.GetDaylenMin(), lfiWoody.GetDaylenMax(), lfiWoody.GetMAPeriod(), lfiWoody.IsUsingVPDAvg(), 
				lfiWoody.GetNumPrecipDays(), lfiWoody.GetRTPcpMin(), lfiWoody.GetRTPcpMax(), lfiWoody.GetUseRTPrecip(), lfiWoody.GetWoodyMin(), lfiWoody.GetWoodyMax());
			theApp.m_NFDRS2016.SetStartKBDI(staSet.m_StartKBDI);
			theApp.m_NFDRS2016.SetSCMax(staSet.GetSCM(staSet.m_NFDRSFM[0]));
			theApp.m_NFDRS2016.SetMxdHumid(staSet.GetMxHumid(staSet.m_NFDRSFM[0]));
		}
		if (!isNFDRS2016(staSet.m_NFDRSFM[0]))
		{   // "old" NFDRS calc
			theApp.m_NFDRS.iInitialize((staSet.m_Use88) ? 88 : 78,//staSet.m_Use88 ? 88 : 78, 
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
		bool bUseStick = true;
		if (!staSet.IsFieldNull(&staSet.m_UseStick))
			bUseStick = staSet.m_UseStick;
		//create Canadian model
		Canadian canadian;
		canadian.Initialize();
		//crunch data...
		bool goodRecord;
		float fBI, fIC, fSC;
		int  iRainEvent, iStage, 
			iKBDI, init = 1, iDeclareGreenUp = 0, iDeclareFreeze = 0, wetFlag, snowFlag;//,
		double f1, f10, f100, f1000, fHerb, fWood, fGren, fX1000, fROS, fERC, fFL,
			ffmc, dmc, dc, bui, isi, fwi, dsr, lfiVal, lfiHerbVal, lfiWoodyVal, iSC, iIC, iBI, iFIL;
		long count = 0;
		COleDateTime yesterday, wxDay;//used to skip duplicates
		COleDateTime extremeDay;//used keep track of current day for extremes
		double dailyExtremes[MAXVARIDS];
		std::fill_n(dailyExtremes, MAXVARIDS, NODATA);
		//wxSet.MoveFirst();
		wxSet.Requery();
		char prgBuf[64];
		if (!wxSet.IsBOF())
		{
			int tmp, tmpMax, tmpMin, rh, rhMax, rhMin, pptDur, sow, greenHerb, greenShrub,
				season, windSpd, windDir, slopeClass;
			double pptAmt, omc10;
			while (!wxSet.IsEOF() && !pCUPDUPData->ShouldTerminate())
			{
				if (wxSet.IsFieldNull(&wxSet.m_Temp)
					|| wxSet.IsFieldNull(&wxSet.m_RH)
					|| wxSet.IsFieldNull(&wxSet.m_ObsDate))
				{
					wxSet.MoveNext();// .GetNext();
					continue;
				}
				wxDay.SetDateTime(wxSet.m_ObsDate.GetYear(), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay(), wxSet.m_ObsDate.GetHour(),
					wxSet.m_ObsDate.GetMinute(), wxSet.m_ObsDate.GetSecond());
				if (!init && yesterday >= wxDay)
				{
					wxSet.MoveNext();//.GetNext();
					continue;
				}
				if (count % 100 == 0)
				{
					sprintf_s(prgBuf, "Processing Record: %d", count);
					pCUPDUPData->SetProgress(prgBuf);
				}
				if (init)
					extremeDay = wxDay;
				// new custom init check    12/2006
				// if this is first record for new year, look for inits
				if (yesterday.GetYear() != wxDay.GetYear())
				{

					CCustomInitSet ciSet(wxSet.m_pDatabase);
					ciSet.m_strFilter.Format("[SIG_Station]='%s' AND [Year]=%d and [Enabled] <> 0", wxSet.m_StationID, wxDay.GetYear());
					ciSet.Open();
					bool usedNFDRScust = false, usedCANcust = false;
					if (!ciSet.IsEOF())
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
						// if(useCanadian)
						//{
						if (!ciSet.IsFieldNull(&ciSet.m_FFMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DC))
						{
							canadian.ReInitialize(ciSet.m_FFMC, ciSet.m_DMC, ciSet.m_DC);
							usedCANcust = true;
							// customInits = true;
						}
						//   }
					}
					if (!usedNFDRScust)
					{
						//set to default station values
						theApp.m_NFDRS.iSetAnnualInit(DayOfYear(staSet.m_GreenJulian), staSet.m_Start1000, sKBDI);
					}
					if (!usedCANcust)
						canadian.ReInitialize(85, 6, 15);
					ciSet.Close();
				}

				if (extremeDay.GetDayOfYear() != wxDay.GetDayOfYear() || extremeDay.GetYear() != wxDay.GetYear())
				{
					for (int j = 0; j < MAXVARIDS; j++)
					{
						if (dailyExtremes[j] != NODATA)
						{
							//int per = GetPeriod(extremeDay);//wxSet->m_ObsDate);
							//periods[j][per]->Accumulate(extremeDay, dailyExtremes[j]);
							int exYear = extremeDay.GetYear() - m_baseYear;
							int exDOY = extremeDay.GetDayOfYear();
							if (!IsLeap(exYear))
							{
								if (exDOY > 59 && exDOY < 366)
									exDOY++;
							}
							exDOY--;
							m_wxData[j]->m_vals[exYear][exDOY] = dailyExtremes[j];
						}
					}

					//memset(dailyExtremes, NODATA, MAXVARIDS * sizeof(double));
					std::fill_n(dailyExtremes, MAXVARIDS, NODATA);
					extremeDay = wxDay;
				}


				yesterday = wxDay;//wxSet.m_ObsDate;
				//pCUPDUPData->Increment();
				int year1 = wxSet.m_ObsDate.GetYear();
				int month1 = wxSet.m_ObsDate.GetMonth();
				int day1 = wxSet.m_ObsDate.GetDay();
				int hour1 = wxSet.m_ObsDate.GetHour();
				int julian1 = wxSet.m_ObsDate.GetDayOfYear();
				double pptAcc = 0;
				double solarRad = wxSet.IsFieldNull(&wxSet.m_SolarRadiation) ? 0.0 : wxSet.m_SolarRadiation;
				double hourlyPrecip = wxSet.IsFieldNull(&wxSet.m_HourlyPrecip) ? 0.0 : wxSet.m_HourlyPrecip;

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
				omc10 = (bUseStick && !wxSet.IsFieldNull(&wxSet.m_OMC10)) ? wxSet.m_OMC10 : -1000.0;
				wetFlag = wxSet.ResolveWetFlag();//.IsFieldNull(&wxSet.m_WetFlag) ? 0 : wxSet.m_WetFlag;
				snowFlag = wxSet.IsFieldNull(&wxSet.m_SnowFlag) ? 0 : wxSet.m_SnowFlag;
				bool isDailyObs = wxSet.IsFieldNull(&wxSet.m_DailyObs) ? false : wxSet.m_DailyObs;
				//omc10 = wxSet.IsFieldNull(&wxSet.m_OMC10) ? -1000.0 : wxSet.m_OMC10;
				//calculate model outputs
				//NFDR 78
				if (isNFDRS2016(staSet.m_NFDRSFM[0]) || useLFI)// && reCalc)
				{ // new NFDRS2016 calc
					if (reCalc)
						theApp.m_NFDRS2016.Update(year1, month1, day1, hour1, julian1, (double)tmp, (double)tmpMin, (double)tmpMax, (double)rh, (double)rhMin, 
							hourlyPrecip, pptAmt, solarRad, (double)windSpd, snowFlag, 
							(isDailyObs) ? hour1 : -1);//staSet.m_RegSchdObs);
					else if(isDailyObs)
					{
						theApp.m_NFDRS2016.UpdateDaily(year1, month1, day1, julian1, (double)tmp, (double)tmpMin, (double)tmpMax, (double)rh, (double)rhMin,
							pptAmt, (double)windSpd, wxSet.m_FM1, wxSet.m_FM10, wxSet.m_FM100, wxSet.m_FM1000, wxSet.m_FuelTemperature, snowFlag);
					}
					else if (!m_usedExtremes)
					{
						wxSet.MoveNext();
						continue;
					}
					if (!reCalc && m_usedExtremes)
					{
						theApp.m_NFDRS2016.iSetFuelMoistures(wxSet.m_FM1, wxSet.m_FM10, wxSet.m_FM100, wxSet.m_FM1000,
							wxSet.m_MCWood, wxSet.m_MCHerb, wxSet.m_FuelTemperature);
						theApp.m_NFDRS2016.iCalcIndexes(windSpd, slopeClass, &iSC, &fERC, &iBI, &iIC);
					}
					f1 = theApp.m_NFDRS2016.MC1;
					f10 = theApp.m_NFDRS2016.MC10;
					f100 = theApp.m_NFDRS2016.MC100;
					f1000 = theApp.m_NFDRS2016.MC1000;
					lfiHerbVal = fHerb = theApp.m_NFDRS2016.MCHERB;
					lfiWoodyVal = fWood = theApp.m_NFDRS2016.MCWOOD;
					lfiVal = theApp.m_NFDRS2016.m_GSI;
					//iStage = theApp.m_NFDRS2016.Stage;
					//fGren = theApp.m_NFDRS2016.Gren;
					fX1000 = 0; // fixme
					//iRainEvent = theApp.m_NFDRS2016.RainEvent;
					iKBDI = theApp.m_NFDRS2016.KBDI;
					fROS = 0; //fixme
					iSC = theApp.m_NFDRS2016.SC;
					fERC = theApp.m_NFDRS2016.ERC;
					fFL = 0; //fixme
					iFIL = 0; //fixme
					iBI = theApp.m_NFDRS2016.BI;
					iIC = theApp.m_NFDRS2016.IC;
					//store moistures to DB
					if (reCalc)
					{
						wxSet.Edit();
						wxSet.m_FM1 = f1;
						wxSet.m_FM10 = f10;
						wxSet.m_FM100 = f100;
						wxSet.m_FM1000 = f1000;
						wxSet.m_FuelTemperature = theApp.m_NFDRS2016.GetFuelTemperature();
						wxSet.m_MCHerb = fHerb;
						wxSet.m_MCWood = fWood;
						wxSet.Update();
					}
					if (hour1 != staSet.m_RegSchdObs && reCalc && !m_usedExtremes)
					{
						count++;
						m_recsDone++;
						init = 0;
						wxSet.MoveNext();//GetNext();
						continue;
					}

				}
				if (!isNFDRS2016(staSet.m_NFDRSFM[0]) && isDailyObs)
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
					//canadian.CalcIndices(DayOfYear(wxSet.m_ObsDate), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetYear(),
						//tmp, rh, windSpd * 1.15, pptAmt,
						//&ffmc, &dmc, &dc, &bui, &isi, &fwi, &dsr);
				}
				//CANADIAN
				if(isDailyObs)
					canadian.CalcIndices(DayOfYear(wxSet.m_ObsDate), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetYear(),
						tmp, rh, windSpd * 1.15, pptAmt,
						&ffmc, &dmc, &dc, &bui, &isi, &fwi, &dsr);

				double val;
				if (true) // filter on m_DailyObs takes care of commented out line below
				//if ((isNFDRS2016(staSet.m_NFDRSFM[0]) && staSet.m_RegSchdObs == wxSet.m_ObsDate.GetHour()) || !isNFDRS2016(staSet.m_NFDRSFM[0]))
				{
					//double val = 0.0;
					//need year and day of year
					int wxYear, wxDOY;
					wxYear = wxDay.GetYear() - m_baseYear;
					wxDOY = wxDay.GetDayOfYear();
					/*if(!IsLeap(wxDay.GetYear()))
					{
						if(wxDOY > 59 && wxDOY < 366)
							wxDOY++;
					}*/
					if (!IsLeap(wxDay.GetYear()))
					{
						if (wxDOY > 59 && wxDOY < 366)
							wxDOY++;
					}
					wxDOY--;
					for (int j = 0; j < STATICVARIDS; j++)
					{

						goodRecord = true;
						switch (j + 1)
						{
						case 1:
							if (!wxSet.IsFieldNull(&wxSet.m_Temp))
								//m_wxData[j]->m_vals[wxYear][wxDOY] 
								val = (float)wxSet.m_Temp;//val = (double)wxSet.m_Temp;
							else
								goodRecord = false;
							break;
						case 2:
							if (!wxSet.IsFieldNull(&wxSet.m_TmpMax) && !wxSet.IsFieldNull(&wxSet.m_TmpMin))
								//m_wxData[j]->m_vals[wxYear][wxDOY] 
								val = (float)(wxSet.m_TmpMax + wxSet.m_TmpMin) / 2.0;
							else
								goodRecord = false;
							break;
						case 3:
							if (!wxSet.IsFieldNull(&wxSet.m_TmpMin))
								//m_wxData[j]->m_vals[wxYear][wxDOY] 
								val = (float)wxSet.m_TmpMin;
							else
								goodRecord = false;
							break;
						case 4:
							if (!wxSet.IsFieldNull(&wxSet.m_TmpMax))
								//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = (float)wxSet.m_TmpMax;
							else
								goodRecord = false;
							break;
						case 5:
							if (!wxSet.IsFieldNull(&wxSet.m_RH))
								//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = (float)max(wxSet.m_RH, 1);
							else
								goodRecord = false;
							break;
						case 6:
							if (!wxSet.IsFieldNull(&wxSet.m_RHMax) && !wxSet.IsFieldNull(&wxSet.m_RHMin))
								//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = (float)(wxSet.m_RHMax + max(wxSet.m_RHMin, 1)) / 2.0;
							else
								goodRecord = false;
							break;
						case 7:
							if (!wxSet.IsFieldNull(&wxSet.m_RHMin))
								//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = (float)max(wxSet.m_RHMin, 1);
							else
								goodRecord = false;
							break;
						case 8:
							if (!wxSet.IsFieldNull(&wxSet.m_RHMax))
								//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = (float)wxSet.m_RHMax;
							else
								goodRecord = false;
							break;
						case 9:
							if (!wxSet.IsFieldNull(&wxSet.m_PPTAMT))
								//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = (float)wxSet.m_PPTAMT;
							else
								goodRecord = false;
							break;
						case 10:
							if (!wxSet.IsFieldNull(&wxSet.m_PPTDUR))
								//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = (float)wxSet.m_PPTDUR;
							else
								goodRecord = false;
							break;
						case 11:
							if (!wxSet.IsFieldNull(&wxSet.m_WS))// && !wxSet.IsFieldNull(&wxSet.m_WDir))
								//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = (float)wxSet.m_WS;
							else
								goodRecord = false;
							break;
						case 12:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = (float)iSC;
							break;
						case 13:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)fERC;// (float)Round(fERC);
							break;
						case 14:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)iBI;
							break;
						case 15:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)iKBDI;
							break;
						case 16: //Ignition component
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)iIC;
							break;
						case 17:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)f1;
							break;
						case 18:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)f10;
							break;
						case 19:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = (float)f100;
							break;
						case 20:
							//m_wxData[j]->m_vals[wxYear][wxDOY] 
							val = (float)f1000;
							break;
						case 21:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)fHerb;
							break;
						case 22:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)fWood;
							break;
						case 23:
							if (!wxSet.IsFieldNull(&wxSet.m_SnowFlag))
								//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = (float)wxSet.m_SnowFlag;
							else
								goodRecord = false;
							break;
						case 24:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)windDir;
							break;
						case 25:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)sow;
							//val = iFIL;
							break;
							//cases 23 - 29 canadian model
						case 26:
							//m_wxData[j]->m_vals[wxYear][wxDOY] 
							val = (float)ffmc;
							break;
						case 27:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)dmc;
							break;
						case 28:
							//m_wxData[j]->m_vals[wxYear][wxDOY] 
							val = (float)dc;
							break;
						case 29:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)isi;
							break;
						case 30:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)bui;
							break;
						case 31:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)fwi;
							break;
						case 32:
							//m_wxData[j]->m_vals[wxYear][wxDOY]
							val = (float)dsr;
							break;
						case 33:
							if (!wxSet.IsFieldNull(&wxSet.m_GustDir))
								//m_wxData[j]->m_vals[wxYear][wxDOY] 
								val = (float)wxSet.m_GustDir;
							else
								goodRecord = false;
							break;
						case 34:
							if (!wxSet.IsFieldNull(&wxSet.m_GustSpd))
								//m_wxData[j]->m_vals[wxYear][wxDOY] 
								val = (float)wxSet.m_GustSpd;
							break;
						case 35:
							if (!wxSet.IsFieldNull(&wxSet.m_SolarRadiation))
								//m_wxData[j]->m_vals[wxYear][wxDOY] 
								val = (float)wxSet.m_SolarRadiation;
							else
								goodRecord = false;
							break;
						case 36:
							if (!wxSet.IsFieldNull(&wxSet.m_WetFlag))
								//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = (float)wxSet.m_WetFlag;
							else
								goodRecord = false;
							break;
						case 37://Dew Point
							if (!wxSet.IsFieldNull(&wxSet.m_RH) && !wxSet.IsFieldNull(&wxSet.m_Temp))
								//m_wxData[j]->m_vals[wxYear][wxDOY] 
								val = CalcDPT(wxSet.m_Temp, max(wxSet.m_RH, 1));
							else
								goodRecord = false;
							break;
						case 38://Vapor pressure deficit MAX
							if (!wxSet.IsFieldNull(&wxSet.m_RHMin) && !wxSet.IsFieldNull(&wxSet.m_TmpMax))
								//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = CalcVPD(max(wxSet.m_RHMin, 1), wxSet.m_TmpMax);
							else
								goodRecord = false;
							break;
						case 39://Vapor pressure deficit Avg
							if (!wxSet.IsFieldNull(&wxSet.m_Temp) && !wxSet.IsFieldNull(&wxSet.m_RH) && !wxSet.IsFieldNull(&wxSet.m_TmpMin) && !wxSet.IsFieldNull(&wxSet.m_TmpMax))
							{
								double dpt = CalcDPT(wxSet.m_Temp, max(wxSet.m_RH, 1));
								double avgT = (wxSet.m_TmpMin + wxSet.m_TmpMax) / 2;
								//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = CalcVPDavg(dpt, avgT);
							}
							else
								goodRecord = false;
							break;
						case 40://Live Fuel Index
							if (lfiVal >= 0.0)
								//m_wxData[j]->m_vals[wxYear][wxDOY] 
								val = lfiVal;// lfi.CalcRunningAvgLFI();
							else
								goodRecord = false;
							break;
						case 41:
							if (!wxSet.IsFieldNull(&wxSet.m_WAzimuth))
								//m_wxData[j]->m_vals[wxYear][wxDOY] = 
								val = (float)wxSet.m_WAzimuth;
							else
								goodRecord = false;
							break;
						case 42:
							if (!wxSet.IsFieldNull(&wxSet.m_HourlyPrecip))
								//m_wxData[j]->m_vals[wxYear][wxDOY]
								val = (float)wxSet.m_HourlyPrecip;
							else
								goodRecord = false;
							break;
						case 43:
							if (lfiHerbVal >= 0.0)
								//m_wxData[j]->m_vals[wxYear][wxDOY] 
								val = lfiHerbVal;// lfiHerb.CalcRunningAvgHerbFM();
							else
								goodRecord = false;
							break;
						case 44:
							if (lfiWoodyVal >= 0.0)
								//m_wxData[j]->m_vals[wxYear][wxDOY] 
								val = lfiWoodyVal;// lfiWoody.CalcRunningAvgWoodyFM();
							else
								goodRecord = false;
							break;
						case 45:
							if (!wxSet.IsFieldNull(&wxSet.m_Temp) && !wxSet.IsFieldNull(&wxSet.m_RH)
								&& !wxSet.IsFieldNull(&wxSet.m_WS))
								//m_wxData[j]->m_vals[wxYear][wxDOY] 
								val = theApp.m_NFDRS.iCalcFFWI(wxSet.m_Temp, max(wxSet.m_RH, 1), wxSet.m_WS);
							else
								goodRecord = false;
							break;
						default:
							goodRecord = false;
							break;
						}
						if (goodRecord && !m_usedExtremes)
						{
							m_wxData[j]->m_vals[wxYear][wxDOY] = val;
							//periods[j][per]->Accumulate(wxDay, val);
							//periods[j][per]->Accumulate(wxSet->m_ObsDate, val);
						}
						else if (goodRecord)
						{
							if (m_extremeIsMax[j])
								dailyExtremes[j] = max(dailyExtremes[j], val);
							else
							{
								if (dailyExtremes[j] == NODATA)
									dailyExtremes[j] = val;
								else
									dailyExtremes[j] = min(dailyExtremes[j], val);
							}
						}
					}
				}
				count++;
				m_recsDone++;
				init = 0;
				wxSet.MoveNext();//.GetNext();
			}
			if (m_usedExtremes && extremeDay.GetDayOfYear() == wxDay.GetDayOfYear()
				&& extremeDay.GetYear() == wxDay.GetYear())
			{
				for (int j = 0; j < MAXVARIDS; j++)
				{
					if (dailyExtremes[j] != NODATA)
					{
						int exYear = extremeDay.GetYear() - m_baseYear;
						int exDOY = extremeDay.GetDayOfYear();
						if (!IsLeap(exYear))
						{
							if (exDOY > 59 && exDOY < 366)
								exDOY++;
						}
						exDOY--;
						m_wxData[j]->m_vals[exYear][exDOY] = dailyExtremes[j];
					}
				}
			}
		}
		//}
		//nfdrs2016Set.Close();
	}
	wxSet.Close();
	//check uservars?
	GetFires();
}


void CRawClim::GetFires()
{
	if(m_fires)
	{
		for(int y = 0; y < m_nYears; y++)
			delete[] m_fires[y];
		delete[] m_fires;
		m_fires = NULL;
	}
	m_fires = new CFireDay *[m_nYears];
	int y;
	for(y = 0; y < m_nYears; y++)
		m_fires[y] = new CFireDay [366];
	CFireSumSet fireSet(m_pDB);
	fireSet.Open();
	if(fireSet.FilterToWorkingSet(m_fpSet, 0, 0,false))//has fires
	{
		double lA = 0.0, hA = 0.0;
		long lF = 0, hF = 0;
		fireSet.MoveFirst();
		
		while(!fireSet.IsEOF())
		{
			if(!fireSet.IsFieldNull(&fireSet.m_Discovery))
			{
				int d = DayOfYear(fireSet.m_Discovery) - 1;
				y = fireSet.m_Discovery.GetYear() - m_baseYear;
				if(y >= 0 && y < m_nYears && d >= 0 && d < 366)
				{
					switch(fireSet.m_StatisticalCause)
					{
						case 1://lightning
							m_fires[y][d].lightning++;
							m_fires[y][d].lAcresMax = max(m_fires[y][d].lAcresMax, fireSet.m_TotalAcres);
							break;
						default:
							m_fires[y][d].human++;
							m_fires[y][d].hAcresMax = max(m_fires[y][d].hAcresMax, fireSet.m_TotalAcres);
					}
					//lF++;
				}
			}
			fireSet.MoveNext();
		}
		/*TRACE1("Fire rec count: %ld\n", lF);
		for(int y = 0; y < m_nYears; y++)
		{
			lF = 0;
			for(int d = 0; d < 366; d++)
				lF += m_fires[y][d].lightning + m_fires[y][d].human;
			TRACE2("%d: %ld\n", y + m_baseYear, lF);
		}*/
	}
	fireSet.Close();
}

void CRawClim::DestroyData()
{
	if(m_climSet)// && m_fpSet->IsOpen())
	{
		m_climSet->Close();
		delete m_climSet;
		m_climSet = NULL;
	}
	if(m_fpSet)// && m_fpSet->IsOpen())
	{
		try
		{
			m_fpSet->Delete();
		}
		catch( CDBException* e )
		{
			e->Delete();
		}
		m_fpSet->Close();
		delete m_fpSet;
		m_fpSet = NULL;
	}
	if(m_wxData)
	{
		for(int v = 0; v < m_nVars; v++)
			delete m_wxData[v];
		delete[] m_wxData;
		m_wxData = NULL;
	}
	if(m_fires)
	{
		for(int y = 0; y < m_nYears; y++)
			delete[] m_fires[y];
		delete[] m_fires;
		m_fires = NULL;
	}
}

IMPLEMENT_DYNCREATE(CQuickBatch, CDocument)

CQuickBatch::CQuickBatch()
{
	activeClim = NULL;
	activeView = activeVar = 0;
	m_pAnalysis = NULL;
	m_pFireStats = NULL;
	m_pFiresSummary = NULL;
}

BOOL CQuickBatch::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	this->m_strTitle = "Interactive Batch";
	return TRUE;
}

CQuickBatch::~CQuickBatch()
{
	CWaitCursor wait;
	while(rawClimList.GetCount() > 0)
		delete rawClimList.RemoveTail();
	stns.RemoveAll();
	if(m_pAnalysis)
	{
		if(m_pAnalysis->m_opts.m_OverlayDialog)
			m_pAnalysis->m_opts.m_OverlayDialog->OnCancel();
		delete m_pAnalysis;
	}
	if(m_pFireStats)
		delete m_pFireStats;
	if(m_pFiresSummary)
		delete m_pFiresSummary;
}

void CQuickBatch::SwitchTabView(int tab)
{
	activeView = tab;
	m_pFrame->SwitchView(activeView);
}

void CQuickBatch::SwitchTreeView(CRawClim *pClim, int varNum)
{
	if(m_pFireStats)
	{
		delete m_pFireStats;
		m_pFireStats = NULL;
	}
	if(m_pFiresSummary)
	{
		delete m_pFiresSummary;
		m_pFiresSummary = NULL;
	}
	if(pClim != activeClim)//need a new Analysis
	{
		GetAnalysis(pClim);
		activeClim = pClim;
	}
	activeVar = varNum;
	SwitchTabView(activeView);
	//UpdateAllViews(NULL);
}

void CQuickBatch::GetFireSummary(void)
{
	if(m_pFiresSummary)
	{
		delete m_pFiresSummary;
		m_pFiresSummary = NULL;
	}
	m_pFiresSummary = new CFireSummary();
	int ret = m_pFiresSummary->AddFires(m_pDB, activeClim->m_fpSet);
	if(m_pFiresSummary->nYears <= 0)
	{
		delete m_pFiresSummary;
		m_pFiresSummary = NULL;
	}
}

void CQuickBatch::GetFireStats(void)
{
	if(m_pFireStats)
	{
		delete m_pFireStats;
		m_pFireStats = NULL;
	}
	if(m_pAnalysis)
	{
		if(m_pAnalysis->fires)
		{
			CFireOptionsSet fireOptsSet(m_pDB);
			CString sigSta = activeClim->m_sigStaID;
			//sigSta.Trim();
			fireOptsSet.m_strFilter.Format("[SIG/Station] = '%s'", sigSta);
			fireOptsSet.Open();
			if(!fireOptsSet.IsEOF())
			{
				fireOptsSet.Edit();
				fireOptsSet.m_VarID = activeVar + 1;
				fireOptsSet.Update();
			}
			else
			{
				fireOptsSet.AddNew();
				fireOptsSet.m_All = TRUE;
				fireOptsSet.m_Lightning = FALSE;
				fireOptsSet.m_Human = FALSE;
				fireOptsSet.m_Both = TRUE;
				fireOptsSet.m_Probability = FALSE;
				fireOptsSet.m_Cumulative = FALSE;
				fireOptsSet.m_Conditional = TRUE;
				fireOptsSet.m_LFAcres = 5;
				fireOptsSet.m_MFDFires = 5;
				fireOptsSet.m_SIG_Station = sigSta;
				fireOptsSet.m_VarID = activeVar + 1;
				fireOptsSet.Update();
				fireOptsSet.Requery();
			}
			//fireOptsSet.m_VarID = activeVar + 1;
			//fireOptsSet.Update();
			int cause = 0;
			if(fireOptsSet.m_Lightning)
				cause = 1;
			else if(fireOptsSet.m_Human)
				cause = 2;
			long nVals, nFD, nLFD, nMFD, nDiscards, nMadeUps;
			m_pAnalysis->GetLogitArraySizesQuick(&fireOptsSet, fireOptsSet.m_VarID - 1,
				&nVals, &nFD, &nLFD, &nMFD, &nDiscards, &nMadeUps);
			double *fxVals, *fyVals,*lfxVals, *lfyVals, *mfxVals, *mfyVals;
			long lfdmfdSize = (fireOptsSet.m_Conditional == true && nFD > 0) ? nFD : nVals;
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

			m_pAnalysis->CreateLogitArraysQuick(&fireOptsSet, fireOptsSet.m_VarID - 1,
				fxVals, fyVals, lfxVals, lfyVals, mfxVals, mfyVals);
			double fd_coef[2], lfd_coef[2], mfd_coef[2];
			double fd_se[2], lfd_se[2], mfd_se[2];
			double fd_t[2], lfd_t[2], mfd_t[2];
			double fd_p[2], lfd_p[2], mfd_p[2];
			double fd_dev, fd_pdev, lfd_dev, lfd_pdev, mfd_dev, mfd_pdev;
			LogReg fd_logReg, lfd_logReg, mfd_logReg;
			fd_logReg.DoLogReg(1, nVals, fyVals, fxVals, fd_coef, fd_se, fd_t, fd_p, &fd_dev, &fd_pdev);
			lfd_logReg.DoLogReg(1, lfdmfdSize, lfyVals, lfxVals, lfd_coef, lfd_se, lfd_t, lfd_p, &lfd_dev, &lfd_pdev);
			mfd_logReg.DoLogReg(1, lfdmfdSize, mfyVals, mfxVals, mfd_coef, mfd_se, mfd_t, mfd_p, &mfd_dev, &mfd_pdev);
			
					CClimateSet tclimSet(m_pDB);
					tclimSet.m_strFilter.Format("[VarID] = %d", activeVar + 1);
					tclimSet.Open();
					m_pFireStats = new CFireStats(tclimSet.m_ShortName, nVals, nFD, nLFD, nMFD, (tclimSet.m_OptionType == 1) ? true : false, activeVar + 1);
					if (!tclimSet.IsFieldNull(&tclimSet.m_FilterValue))
						m_pFireStats->filterValue = tclimSet.m_FilterValue;
					if (tclimSet.m_cp2 < tclimSet.m_CriticalPercentile)
						m_pFireStats->filterDirection = FILTER_GREATERTHAN_EQUAL;
					m_pFireStats->shortName = tclimSet.m_ShortName;
					m_pFireStats->nMadeUps = nMadeUps;
					m_pFireStats->binSize = tclimSet.m_BinSize;
					m_pFireStats->staStr.Format("%s", 
						activeClim->m_fpSet->m_SIG_Station.GetLength() > 6 ? activeClim->m_fpSet->m_SIG_Station 
						: "Station: " + activeClim->m_fpSet->m_SIG_Station);
					m_pFireStats->lfAcres = fireOptsSet.m_LFAcres;
					m_pFireStats->mfdFires = fireOptsSet.m_MFDFires;
					m_pFireStats->cause = cause;
					m_pFireStats->nDiscards = nDiscards;
					m_pFireStats->start.SetDate(activeClim->m_fpSet->m_StartYear, MonthChtoInt(activeClim->m_fpSet->m_StartMonth), activeClim->m_fpSet->m_StartDay);
					m_pFireStats->end.SetDate(activeClim->m_fpSet->m_EndYear, MonthChtoInt(activeClim->m_fpSet->m_EndMonth), activeClim->m_fpSet->m_EndDay);
					//m_pFireStats->Build(activeClim->m_fpSet, true, fireOptsSet.m_Conditional);
					m_pFireStats->BuildQuick(activeClim->m_fpSet, true, fireOptsSet.m_Conditional,
						nVals, lfdmfdSize,
						fxVals, fyVals, lfxVals, lfyVals, mfxVals, mfyVals,
						&fd_logReg, &lfd_logReg, &mfd_logReg);
					tclimSet.Close();

			delete[] fxVals;
			delete[] fyVals;
			delete[] lfxVals;
			delete[] lfyVals;
			delete[] mfxVals;
			delete[] mfyVals;
					
					//	}
			//}
			//if(dllHandle)
			//	FreeLibrary(dllHandle);
		}
	}
}

void CQuickBatch::GetAnalysis(CRawClim *pClim)
{
	if(m_pAnalysis)
	{
		if(m_pAnalysis->m_opts.m_OverlayDialog)
			m_pAnalysis->m_opts.m_OverlayDialog->OnCancel();
		delete m_pAnalysis;
		m_pAnalysis = NULL;
	}
	//CFireStats are invalid...
	if(m_pFireStats)
	{
		delete m_pFireStats;
		m_pFireStats = NULL;
	}
	if(m_pFiresSummary)
	{
		delete m_pFiresSummary;
		m_pFiresSummary = NULL;
	}
	m_pAnalysis = new CClimAnalysis(m_pDB);
	m_pAnalysis->pCUPDUPData = pCUPDUPData;
	m_pAnalysis->AnalyzeQuick(pClim);
}

BEGIN_MESSAGE_MAP(CQuickBatch, CDocument)
//	ON_COMMAND(ID_QUICK_PROPERTIES, OnFileProperties)
END_MESSAGE_MAP()

// CBatchOutputDoc diagnostics

#ifdef _DEBUG
void CQuickBatch::AssertValid() const
{
	CDocument::AssertValid();
}

void CQuickBatch::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// CBatchOutputDoc serialization

void CQuickBatch::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}



void CQuickBatch::CopyStationList(CStringList *pList)
{
	POSITION pos = pList->GetHeadPosition();
	while(pos)
	{
		stns.AddTail(pList->GetNext(pos));
	}
}

void CQuickBatch::GetData()
{
	double toDo = stns.GetCount();
	int doneSoFar = 0;
	POSITION pos = stns.GetHeadPosition();
	while(pos && !pCUPDUPData->ShouldTerminate())
	{
		CString staID = stns.GetNext(pos);
		CRawClim *newClim = new CRawClim(m_pDB, staID);
		newClim->pCUPDUPData = pCUPDUPData;
		newClim->AllocData();
		rawClimList.AddTail(newClim);
		doneSoFar++;
		pCUPDUPData->SetProgress(((double)doneSoFar) / toDo * 100.0);
	}
	if(pCUPDUPData->ShouldTerminate())//user cancelled before finish...
		return;

	pCUPDUPData->SetProgress("Compiling Data...", 100);
	pCUPDUPData->AllowCancel(false);
	activeClim = rawClimList.GetHead();

	//edit the activeClim to match main window workingset data if the workingSet station is included...
	POSITION cPos = rawClimList.GetHeadPosition();
	while(cPos)
	{
		CRawClim *pClim = rawClimList.GetNext(cPos);
		//if(pClim->m_sigStaID.Compare(m_pStartupSet->m_SIG_Station) == 0)
		//{
			//found the right rewClim... need to set it's members to match current workingSet
			activeClim = pClim;
			pClim->m_fpSet->Edit();
			//p
			pClim->m_fpSet->m_StartDay = m_pStartupSet->m_StartDay;
			pClim->m_fpSet->m_StartMonth = m_pStartupSet->m_StartMonth;
			if(m_pStartupSet->m_StartYear > pClim->m_fpSet->m_StartYear && m_pStartupSet->m_StartYear <= pClim->m_fpSet->m_EndYear)
				pClim->m_fpSet->m_StartYear = m_pStartupSet->m_StartYear;
			pClim->m_fpSet->m_EndDay = m_pStartupSet->m_EndDay;
			pClim->m_fpSet->m_EndMonth = m_pStartupSet->m_EndMonth;
			if(m_pStartupSet->m_EndYear < pClim->m_fpSet->m_EndYear && m_pStartupSet->m_EndYear >= pClim->m_fpSet->m_StartYear)
				pClim->m_fpSet->m_EndYear = m_pStartupSet->m_EndYear;
			pClim->m_fpSet->Update();
			pClim->m_fpSet->MoveLast();
		//	break;
		//}
	}


	GetAnalysis(activeClim);
}

CRunOutput::CRunOutput()
{
	analysis = NULL;
	fStats = NULL;
	batchID = runID = 0;
	autoSave = FALSE;
	killData = TRUE;
}

CRunOutput::~CRunOutput()
{
	if(killData)
	{
		if(analysis)
		{
			analysis->killPeriods = true;
			delete analysis;
		}
		if(fStats)
			delete fStats;
	}
}

int CRunOutput::CreateReports(CClimateSet *climSet)
{
	int ret = 0;
	CPeriodDoc *pdDoc = NULL;
	int tnumPeriods = 0;
	Period ***tperiods = NULL;
	CFireDay **tFires = NULL;
	bool useAux = false;
	climSet->MoveFirst();
	while(!climSet->IsEOF())    
	{
		if(climSet->m_Stats_Graph)
		{
			useAux = true;
			break;
		}
		climSet->MoveNext();
	}
	if(analysis->fpSet->m_Use78 && useAux)//auxilliary years
	{
		CClimAnalysis tAnalysis(analysis->pDB);
		tAnalysis.pCUPDUPData = analysis->pCUPDUPData;
		int *tVarIDs = new int[analysis->nVarIDs];
		for(int v = 0; v < analysis->nVarIDs; v++)
			tVarIDs[v] = analysis->varIDs[v];
		if(tAnalysis.AnalyzeBatchItem(NULL, tVarIDs, analysis->nVarIDs, analysis->fpSet, true) == 0)
		{
			tAnalysis.killPeriods = false;
			tAnalysis.killfpSet = false;
			tnumPeriods = tAnalysis.numPeriods;
			tperiods = tAnalysis.periods;
			tFires = tAnalysis.fires;
		}
	}
//	analysis->autosave = true;
	//CClimateSet climSet(analysis->pDB);
	//climSet.Open();
	climSet->MoveFirst();
	while(!climSet->IsEOF())
	{
		//generate selected outputs
		if(climSet->m_Stats_Table)
		{
			if(climSet->m_VarID == 9)
				analysis->PrecipTable(climSet->m_VarID - 1, climSet->m_CriticalPercentile / 100.0);
			else
				analysis->StatsTable(climSet->m_VarID - 1, climSet->m_CriticalPercentile / 100.0);
			ret++;
		}
		if(climSet->m_Data_Count)
		{
			analysis->DataCount(climSet->m_VarID - 1);
			ret++;
		}
		if(climSet->m_Stats_Graph)
		{
			pdDoc = analysis->AddGraphView(pdDoc, climSet->m_VarID - 1, climSet->m_OptionType, tnumPeriods, tperiods, tFires);
			ret++;
		}
		if(climSet->m_Daily_Freqs)
		{
			if(climSet->m_VarID == 9)
				analysis->PrecipFrequencyTable(climSet->m_VarID - 1);
			else
				analysis->FrequencyTable(climSet->m_VarID - 1);
			ret++;
		}
		climSet->MoveNext();
	}
	//other reports/graphs
	if(pdDoc)
	{
		analysis->killPeriods = false;
		killData = FALSE;
		return ret;
	}
	return ret;
}

CBatchOutput::CBatchOutput()
{
	batchID = -1;
	pCUPDUPData = NULL;
	m_autoSave = false;
	docDir = "";
}

CBatchOutput::~CBatchOutput()
{
	while(runOutputs.GetCount() > 0)
		delete runOutputs.RemoveTail();

}

void CBatchOutput::RunBatch()
{
	//batchID = _batchID;
	//CDaoWorkspace ws;
	//ws.Create("temp", "admin", "");
	//CDatabase DB(&ws);
	//m_pDB = ;
	///m_pDB->Open(m_pDBName);
	CBatchSet batches(m_pDB);
	batches.m_strSort =  _T("[batchID]");

	CString query, temp;
	int nReports = 0;
	
	CBatchRunsSet runsSet(m_pDB);
	runsSet.m_strFilter.Format("[batchID]=%d", batchID);
	runsSet.Open();
	runsSet.Requery();
	//quick count for statusbar....
	double numRuns = 0, runsDone = 0;
	while(!runsSet.IsEOF())
	{
		numRuns++;
		runsSet.MoveNext();
	}
	runsSet.MoveFirst();

	CGraphSet rOpts(m_pDB);
	
	try
		{
			rOpts.Open();
		}
	catch( CDBException* e )
		{
			e->Delete();
		}
	
	CString savePath = theApp.dbDir;//docDir;//theApp.workDir;
    if (rOpts.IsOpen())
	{
		//BOOL autoSave = rOpts.m_AutoSave;
	
		if(!rOpts.IsEOF())
		{
			savePath = rOpts.m_AutoSaveLocation;
		}

	
		rOpts.Close();
	}
	savePath.Trim();
	

	
	while(!runsSet.IsEOF() && !pCUPDUPData->ShouldTerminate())
	{
		pCUPDUPData->SetProgress(((double)runsDone) / numRuns * 100.0);
		//curr_runID = runsSet.m_runID;
		int tRunID = runsSet.m_runID;
		//CRunSave runSave;
		//int restoreID ;//.m_batchID;
		CFireplusSet *tfpSet = new CFireplusSet(m_pDB);
		tfpSet->Open();
		CRunSave saveRun(tfpSet);
		saveRun.RestoreGeneral(tRunID, tfpSet);
		saveRun.RestoreClimateOptions(tRunID, tfpSet);
		saveRun.RestoreWxStation(tRunID, tfpSet);
		saveRun.RestoreFireOptions(tRunID, tfpSet);
		CClimAnalysis *analysis = new CClimAnalysis(m_pDB);
		analysis->pCUPDUPData = pCUPDUPData;
		analysis->m_runID = tRunID;
		CClimateSet *climSet = new CClimateSet(m_pDB);
		climSet->m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
		climSet->m_strSort = "[VarID]";
		climSet->Open();
		int *varIDs, nVars = 0;
		while(!climSet->IsEOF())
		{
			if (climSet->m_Stats_Table || climSet->m_Stats_Graph || climSet->m_Daily_Freqs || climSet->m_Data_Count
				|| climSet->m_Period_Mins || climSet->m_Period_Maxs)
			{
				nVars++;
			}
			climSet->MoveNext();
		}
		climSet->MoveFirst();
		varIDs = new int[nVars];
		int vLoc = 0;
		while(!climSet->IsEOF())
		{
			if (climSet->m_Stats_Table || climSet->m_Stats_Graph || climSet->m_Daily_Freqs || climSet->m_Data_Count
				|| climSet->m_Period_Mins || climSet->m_Period_Maxs)
			{
				varIDs[vLoc] = climSet->m_VarID - 1;
				vLoc++;
			}
			climSet->MoveNext();
		}
		climSet->MoveFirst();
		analysis->AnalyzeBatchItem(NULL, varIDs, nVars, tfpSet, false);
		if(!m_autoSave)
			analysis->killfpSet = true;
		//analysis->AnalyzeBatchItem(NULL, climSet, tfpSet, false);
		//analysis->AnalyzeBatchItem(NULL, climSet, tfpSet, true);
		CRunOutput *pOutput = new CRunOutput();
		pOutput->analysis = analysis;
		pOutput->batchID = batchID;
		pOutput->runID = tRunID;

		if(!pCUPDUPData->ShouldTerminate())
		{
			/*if(m_autoSave)
			{
				COleDateTime dt;
				dt = COleDateTime::GetCurrentTime();
				//CString dtStr = dt.Format("%Y-%m-%d-%H-%M");
				chdir(savePath);
				char tmp[MAX_PATH];
				sprintf(tmp, "Batch%d", batchID);
				if(chdir(tmp) != 0)
				{
					mkdir(tmp);
					chdir(tmp);
				}
				sprintf(tmp, "Run%d", curr_runID);
				if(chdir(tmp) != 0)
				{
					mkdir(tmp);
					chdir(tmp);
				}
//				strcpy_s(tmp, dtStr);
//				if(chdir(tmp) != 0)
//				{
//					mkdir(tmp);
//					chdir(tmp);
//				}
				chdir(theApp.workDir);
				analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, curr_runID);

				nReports += pOutput->CreateReports(climSet);
				delete pOutput;
			}
			else
			{*/
				//nReports += pOutput->CreateReports(climSet);
				runOutputs.AddTail(pOutput);
			//}
		}
		else
		{
			delete pOutput;
		}
		//runSave.RestoreGeneral(runsSet.m_batchID);
		//delete analysis;
		climSet->Close();
		delete climSet;
		if(!this->m_autoSave && !pCUPDUPData->ShouldTerminate())
		{
			tfpSet->Close();
			delete tfpSet;
		}
		runsSet.MoveNext();
		runsDone++;
	}
	if(pCUPDUPData->ShouldTerminate())//user cancelled before finish...
	{
		pCUPDUPData->SetProgress("Batch job cancelled by user.");
		return;
	}
	//true batch just writes output files to disk...
	/*if(1)//all we're doing for now...
	{
		//cycle through runs creating reports and graphs directly to disk
		POSITION pos = runOutputs.GetHeadPosition();
		while(pos)
		{
			CRunOutput * pOutput = runOutputs.GetNext(pos);
			pOutput->CreateReports();
		}

		//clean up
	}*/
	/*else //interactive mode....Bring up the mother of all FFP report/graph windows
	{
	}*/
	CString msg;
	msg.Format("Batch job complete, %d reports saved to disk.\nReports saved under folder\n%s\\Batch%d\\",
		nReports, savePath, batchID);
	pCUPDUPData->SetProgress(msg, 100);
	
}

void CBatchOutput::SaveReports()
{
	int nReports = 0;
	CGraphSet rOpts(m_pDB);
	rOpts.Open();
	//BOOL autoSave = rOpts.m_AutoSave;
	CString savePath = theApp.dbDir;//docDir;//theApp.workDir;
	if(!rOpts.IsEOF())
	{
		savePath = rOpts.m_AutoSaveLocation;
	}
	rOpts.Close();
	savePath.Trim();
	POSITION pos = runOutputs.GetHeadPosition();
	while(pos)
	{
		CRunOutput *pOutput = runOutputs.GetNext(pos);
		if(pOutput)
		{
			pOutput->analysis->autosave = true;
			CFireplusSet *tfpSet = new CFireplusSet(m_pDB);
			tfpSet->Open();
			CRunSave saveRun(tfpSet);
			saveRun.RestoreGeneral(pOutput->runID, tfpSet);
			saveRun.RestoreFireOptions(pOutput->runID, tfpSet);
			CDistinctRunReportsSet distinctReports(m_pDB);
			CString dSqlStr;
			dSqlStr.Format("SELECT DISTINCT ffpRunOptions.reportID FROM ffpRunOptions WHERE ffpRunOptions.runID = %d",
				pOutput->runID);
			distinctReports.Open(CRecordset::dynaset, dSqlStr);
			while(!distinctReports.IsEOF())
			{
				chdir(savePath);
				char tmp[MAX_PATH];
				sprintf(tmp, "Batch%d", batchID);
				if(chdir(tmp) != 0)
				{
					mkdir(tmp);
					chdir(tmp);
				}
				sprintf(tmp, "Run%d", pOutput->runID);
				if(chdir(tmp) != 0)
				{
					mkdir(tmp);
					chdir(tmp);
				}
				chdir(theApp.dbDir);//theApp.workDir);
				switch(distinctReports.m_reportID)
				{
				case REPORT_WEATHER_CLIMATOLOGY:
					{
						saveRun.RestoreClimateOptions(pOutput->runID, tfpSet);
						saveRun.RestoreWxStation(pOutput->runID, tfpSet);
						CClimateSet *climSet = new CClimateSet(m_pDB);
						climSet->m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";

						climSet->m_strSort = "[VarID]";
						climSet->Open();
						pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
						pOutput->analysis->fpSet = tfpSet;
						pOutput->analysis->climSet = climSet;
						nReports += pOutput->CreateReports(climSet);
						climSet->Close();
						delete climSet;
					}
					break;
				/*case REPORT_WEATHER_EVENT_LOCATOR:
					{
						CRunOptionsSet rOpts(m_pDB);
						rOpts.m_strFilter.Format("[runID] = %d AND [reportID] = %d",
							pOutput->runID, distinctReports.m_reportID);
						rOpts.Open();
						while(!rOpts.IsEOF())
						{
							saveRun.RestoreRunOption(rOpts.m_runID, rOpts.m_reportID, rOpts.m_row, 
								rOpts.m_Variable, &rOpts.m_Value, tfpSet);
							rOpts.MoveNext();
						}
						rOpts.Close();
						//pOutput->analysis->LocateEvents();
					}
					break;*/
				case REPORT_WEATHER_FIRE_DANGER:
				case REPORT_WEATHER_HOURLY_LISTINGS:
				case REPORT_WEATHER_DIURNAL_GRAPHS:
				case REPORT_WEATHER_FARSITE_EXPORTS:
				case REPORT_WEATHER_POCKET_CARD:
					{
						CRunPocketCardSet rpcSet(m_pDB);
						rpcSet.m_strFilter.Format("[runID] = %d", pOutput->runID);
						rpcSet.Open();
						if(!rpcSet.IsEOF())
						{
							pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
							pOutput->analysis->fpSet = tfpSet;
							pOutput->analysis->LoadRunPocketOptions(&rpcSet);
							CPocketCardView *pctView = new CPocketCardView();
							pctView->m_pAnalysis = pOutput->analysis;
							pctView->PreDrawCalcs();
							CGraphSet gSet(m_pDB);
							gSet.Open();//CRecordset::dynaset, "SELECT * FROM ffpGraphOptions");
							CString saveName = pOutput->analysis->autosavePath + "PocketCard." + gSet.GetGraphicsExtension();
							CRect rect(0, 0, gSet.m_AutoSaveWidth, gSet.m_AutoSaveHeight);
							pctView->SaveToFile(saveName, &rect);
							delete pctView;
							nReports++;
						}
						rpcSet.Close();
					}
					break;
				case REPORT_WEATHER_PERSISTENCE_PROB:
					{
						//saveRun.SaveGeneral(0, runDesc, fpSet);
						pOutput->analysis->fpSet = tfpSet;
						pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
						saveRun.RestoreClimateOptions(pOutput->runID, tfpSet);
						saveRun.RestoreWxStation(pOutput->runID, tfpSet);
						// removed 8/2006 saveRun.SaveWxStation(curr_runID, fpSet);
						//saveRun.SaveFireOptions(curr_runID, fpSet);
						// save variable
						CString value1;
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_PERSISTENCE_PROB,0,"varID",&value1,tfpSet);

						// restore bin values
						CSeasonBinSet binSet(m_pDB);
						binSet.m_strFilter.Format("[VarID] = %d", atoi(value1));
						binSet.Open();
						if(binSet.IsEOF())
						{
							binSet.AddNew();
							binSet.m_VarID = atoi(value1);
						}
						else
							binSet.Edit();
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin1Min", &value1, tfpSet);
						binSet.m_Bin1Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin1Max", &value1, tfpSet);
						binSet.m_Bin1Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin2Min", &value1, tfpSet);
						binSet.m_Bin2Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin2Max", &value1, tfpSet);
						binSet.m_Bin2Max = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin3Min", &value1, tfpSet);
						binSet.m_Bin3Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin3Max", &value1, tfpSet);
						binSet.m_Bin3Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin4Min", &value1, tfpSet);
						binSet.m_Bin4Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin4Max", &value1, tfpSet);
						binSet.m_Bin4Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin5Min", &value1, tfpSet);
						binSet.m_Bin5Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin5Max", &value1, tfpSet);
						binSet.m_Bin5Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin6Min", &value1, tfpSet);
						binSet.m_Bin6Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin6Max", &value1, tfpSet);
						binSet.m_Bin6Max = atof(value1);
						binSet.Update();
						pOutput->analysis->autosave = true;
						pOutput->analysis->PersistenceProbability(&binSet);
						binSet.Close();
						nReports ++;
					}
					break;
				case REPORT_WEATHER_SEVERITY_LIST:
				case REPORT_WEATHER_SEVERITY_SUMM:
					{
						saveRun.RestoreClimateOptions(pOutput->runID, tfpSet);
						saveRun.RestoreWxStation(pOutput->runID, tfpSet);
						CClimateSet *climSet = new CClimateSet(m_pDB);
						climSet->m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
						climSet->m_strSort = "[VarID]";
						climSet->Open();
						pOutput->analysis->autosave = true;
						pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
						pOutput->analysis->fpSet = tfpSet;
						pOutput->analysis->climSet = climSet;
						CString value1;
						double cp;
						bool greater;
						int vLoc;
						saveRun.RestoreRunOption(pOutput->runID,  distinctReports.m_reportID, 0, "varID", &value1, tfpSet);
						vLoc = atoi(value1);	
						// CP
						//value1 = "1";
						//if (m_spinCP.GetPos())
						//	value1.Format("%3d", m_spinCP.GetPos());
						saveRun.RestoreRunOption(pOutput->runID, distinctReports.m_reportID,0,"cp",&value1,tfpSet);
						cp = atof(value1);
						// operator greaterthan/lessthan
						//value1.Format("%2d",id);
						saveRun.RestoreRunOption(pOutput->runID, distinctReports.m_reportID,0,"radiobutton",&value1,tfpSet);
						greater = (bool)atoi(value1);
						if(distinctReports.m_reportID == REPORT_WEATHER_SEVERITY_LIST)
							pOutput->analysis->SeverityList(cp, greater, vLoc); 
						else
							pOutput->analysis->SeveritySummary(cp, greater, vLoc);
						nReports ++;
						climSet->Close();
						delete climSet;
					}
					break;
				case REPORT_WEATHER_PROBABILITY_ANAL:
					{
						saveRun.RestoreClimateOptions(pOutput->runID, tfpSet);
						saveRun.RestoreWxStation(pOutput->runID, tfpSet);
						CClimateSet *climSet = new CClimateSet(m_pDB);
						climSet->m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
						climSet->m_strSort = "[VarID]";
						climSet->Open();
						pOutput->analysis->autosave = true;
						pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
						pOutput->analysis->fpSet = tfpSet;
						pOutput->analysis->climSet = climSet;
						CString value1;
						int varID1, varID2;
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_PROBABILITY_ANAL,0,"varID1",&value1,tfpSet);
						varID1 = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_PROBABILITY_ANAL,0,"varID2",&value1,tfpSet);
						varID2 = atoi(value1);
						// restore bin values
						CSeasonBinSet binSet(m_pDB);
						binSet.m_strFilter.Format("[VarID] = %d", varID1);
						binSet.Open();
						if(binSet.IsEOF())
						{
							binSet.AddNew();
							binSet.m_VarID = varID1;
						}
						else
							binSet.Edit();
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin1Min", &value1, tfpSet);
						binSet.m_Bin1Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin1Max", &value1, tfpSet);
						binSet.m_Bin1Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin2Min", &value1, tfpSet);
						binSet.m_Bin2Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin2Max", &value1, tfpSet);
						binSet.m_Bin2Max = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin3Min", &value1, tfpSet);
						binSet.m_Bin3Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin3Max", &value1, tfpSet);
						binSet.m_Bin3Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin4Min", &value1, tfpSet);
						binSet.m_Bin4Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin4Max", &value1, tfpSet);
						binSet.m_Bin4Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin5Min", &value1, tfpSet);
						binSet.m_Bin5Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin5Max", &value1, tfpSet);
						binSet.m_Bin5Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin6Min", &value1, tfpSet);
						binSet.m_Bin6Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin6Max", &value1, tfpSet);
						binSet.m_Bin6Max = atof(value1);
						binSet.Update();
						binSet.m_strFilter.Format("[VarID] = %d", varID2);
						binSet.Requery();
						if(binSet.IsEOF())
						{
							binSet.AddNew();
							binSet.m_VarID = varID2;
						}
						else
							binSet.Edit();
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin1Min", &value1, tfpSet);
						binSet.m_Bin1Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin1Max", &value1, tfpSet);
						binSet.m_Bin1Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin2Min", &value1, tfpSet);
						binSet.m_Bin2Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin2Max", &value1, tfpSet);
						binSet.m_Bin2Max = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin3Min", &value1, tfpSet);
						binSet.m_Bin3Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin3Max", &value1, tfpSet);
						binSet.m_Bin3Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin4Min", &value1, tfpSet);
						binSet.m_Bin4Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin4Max", &value1, tfpSet);
						binSet.m_Bin4Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin5Min", &value1, tfpSet);
						binSet.m_Bin5Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin5Max", &value1, tfpSet);
						binSet.m_Bin5Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin6Min", &value1, tfpSet);
						binSet.m_Bin6Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin6Max", &value1, tfpSet);
						binSet.m_Bin6Max = atof(value1);
						binSet.Update();
						binSet.Close();

						CSeasonBinSet set1(m_pDB), set2(m_pDB);
						set1.m_strFilter.Format("[VarID] = %d", varID1);
						set2.m_strFilter.Format("[VarID] = %d", varID2);
						set1.Open();
						set2.Open();
						pOutput->analysis->ProbabilityAnalysis(&set1, &set2, varID1 - 1, varID2 - 1);
						set1.Close();
						set2.Close();
						nReports ++;
						climSet->Close();
						delete climSet;
					}
					break;
				case REPORT_WEATHER_DAILY_LISTING:
					{
						saveRun.RestoreClimateOptions(pOutput->runID, tfpSet);
						saveRun.RestoreWxStation(pOutput->runID, tfpSet);
						CClimateSet *climSet = new CClimateSet(m_pDB);
						climSet->m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
						climSet->m_strSort = "[VarID]";
						climSet->Open();
						pOutput->analysis->autosave = true;
						pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
						pOutput->analysis->fpSet = tfpSet;
						pOutput->analysis->climSet = climSet;
						ListingOptions lo;
						CString value1;
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING, 0, 
							"checkbox_TotalAcres",&value1,tfpSet);
						lo.acres = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_NumPerClass", &value1, tfpSet);
						lo.numPerClass = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_NumFires", &value1, tfpSet);
						lo.fires = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_NumLargeFires", &value1, tfpSet);
						lo.largeFires = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"spin_LFDAcres", &value1, tfpSet);
						lo.lfAcres = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"radio_fireCause", &value1, tfpSet);
						lo.fCause = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_reportHeader", &value1, tfpSet);
						lo.reportHeader = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_columnHeader", &value1, tfpSet);
						lo.columnHeader = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_dateStamp", &value1, tfpSet);
						lo.dateStamp = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"radio_DateFormat", &value1, tfpSet);
						lo.dt = (DateType)atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"radio_TimeFormat", &value1, tfpSet);
						lo.tt = (TimeType)atoi(value1);
						if(saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_StnPerRecord", &value1, tfpSet) != 0)
						{
							lo.stnPerRecord = atoi(value1);
						}
						bool exportToDB = false;
						if(saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_ExportToDB", &value1, tfpSet) != 0)
						{
							exportToDB = true;
						}

						pOutput->analysis->DailyListing(&lo, false,exportToDB);//(climSet);
						nReports ++;
						climSet->Close();
						delete climSet;
					}
					break;
				case REPORT_WEATHER_PERCENTILE_WEATHER:
				case REPORT_WEATHER_WINDS:
				case REPORT_FIRES_SUMMARY:
					break;
				case REPORT_FIRES_ANALYSIS:
					{
						pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
						pOutput->analysis->fpSet = tfpSet;
						saveRun.RestoreFireOptions(pOutput->runID, tfpSet);
						//pOutput->analysis->F
						nReports ++; 
					}
					break;
				default:
					//do nothing
					break;
				}
				distinctReports.MoveNext();
			}
			distinctReports.Close();
			//cycle thru reports....
			//CDistinctRunsSet distinctRuns(m_pDB);
			//distinctRuns.Open(CRecordset::dynaset, "SELECT DISTINCT ffpRunOptions.runID FROM ffpRunOptions ORDER BY ffpRunOptions.runID;");
			//while(!distinctRuns.IsEOF())
			//{
			//CRunOptionsSet runOpts(m_pDB);
			//runOpts.Open();
			//runOpts.m_strFilter.Format("{runID] = %d", pOutput->runID)
			//saveRun.RestoreClimateOptions(pOutput->runID, tfpSet);
			// removed 8/2006 saveRun.RestoreWxStation(curr_runID, tfpSet);
			//saveRun.RestoreFireOptions(pOutput->runID, tfpSet);
			tfpSet->Close();
			delete tfpSet;
		}
	}
	CString msg;
	msg.Format("Batch Autosave Run Complete\n%d reports and graphs created.\n"
		"Output saved to: %sBatch%d",
		nReports, savePath, batchID);
	AfxMessageBox(msg);
}

void CBatchOutput::GenerateReports()
{
	int nReports = 0;
	POSITION pos = runOutputs.GetHeadPosition();
	while(pos)
	{
		CRunOutput *pOutput = runOutputs.GetNext(pos);
		if(pOutput)
		{
			CFireplusSet *tfpSet = new CFireplusSet(m_pDB);
			tfpSet->Open();
			CRunSave saveRun(tfpSet);
 			saveRun.RestoreGeneral(pOutput->runID, tfpSet);
			saveRun.RestoreFireOptions(pOutput->runID, tfpSet);
			CDistinctRunReportsSet distinctReports(m_pDB);
			CString dSqlStr;
			dSqlStr.Format("SELECT DISTINCT ffpRunOptions.reportID FROM ffpRunOptions WHERE ffpRunOptions.runID = %d",
				pOutput->runID);
			distinctReports.Open(CRecordset::dynaset, dSqlStr);
			while(!distinctReports.IsEOF())
			{
				switch(distinctReports.m_reportID)
				{
				case REPORT_WEATHER_CLIMATOLOGY:
					{
						saveRun.RestoreClimateOptions(pOutput->runID, tfpSet);
						saveRun.RestoreWxStation(pOutput->runID, tfpSet);
						CClimateSet *climSet = new CClimateSet(m_pDB);
						climSet->m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
						climSet->m_strSort = "[VarID]";
						climSet->Open();
						//pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
						pOutput->analysis->fpSet = tfpSet;
						pOutput->analysis->killfpSet = true;
						pOutput->analysis->climSet = climSet;
						nReports += pOutput->CreateReports(climSet);
						climSet->Close();
						delete climSet;
					}
					break;
				/*case REPORT_WEATHER_EVENT_LOCATOR:
					{
						CRunOptionsSet rOpts(m_pDB);
						rOpts.m_strFilter.Format("[runID] = %d AND [reportID] = %d",
							pOutput->runID, distinctReports.m_reportID);
						rOpts.Open();
						while(!rOpts.IsEOF())
						{
							saveRun.RestoreRunOption(rOpts.m_runID, rOpts.m_reportID, rOpts.m_row, 
								rOpts.m_Variable, &rOpts.m_Value, tfpSet);
							rOpts.MoveNext();
						}
						rOpts.Close();
						//pOutput->analysis->LocateEvents();
					}
					break;*/
				case REPORT_WEATHER_FIRE_DANGER:
				case REPORT_WEATHER_HOURLY_LISTINGS:
				case REPORT_WEATHER_DIURNAL_GRAPHS:
				case REPORT_WEATHER_FARSITE_EXPORTS:
					break;
				case REPORT_WEATHER_POCKET_CARD:
					{
						CRunPocketCardSet rpcSet(m_pDB);
						rpcSet.m_strFilter.Format("[runID] = %d", pOutput->runID);
						rpcSet.Open();
						if(!rpcSet.IsEOF())
						{
							//pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
							pOutput->analysis->fpSet = tfpSet;
							pOutput->analysis->killfpSet = true;
							pOutput->analysis->LoadRunPocketOptions(&rpcSet);
							//now display the pocket card in a child window
							POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

							while(curTemplatePos != NULL)
							{
								CDocTemplate* curTemplate = 
									theApp.GetNextDocTemplate(curTemplatePos);
								CString str;
								curTemplate->GetDocString(str, CDocTemplate::docName);
								if(str == _T("PocketCard"))
								{
									CPocketCardDoc *pDoc = (CPocketCardDoc *)curTemplate->CreateNewDocument();
									//set PocketCardDoc members
									pDoc->m_pAnalysis = pOutput->analysis;
									CPocketCardFrame* pFrame = (CPocketCardFrame*)curTemplate->CreateNewFrame(pDoc, NULL);
									CPocketCardView *pView = (CPocketCardView *)pFrame->GetWindow(GW_CHILD);
									pView->m_pAnalysis = pDoc->m_pAnalysis;
									curTemplate->InitialUpdateFrame(pFrame, pDoc);
									CString tTitle;
									tTitle.Format("%s Pocket Card", pOutput->analysis->m_pocketOpts.m_Area);//m_FireDangerArea);
									pDoc->SetTitle(tTitle);
									break;
								}
							}
							/*CPocketCardView *pctView = new CPocketCardView();
							pctView->m_pAnalysis = pOutput->analysis;
							pctView->PreDrawCalcs();
							CGraphSet gSet(m_pDB);
							gSet.Open();//CRecordset::dynaset, "SELECT * FROM ffpGraphOptions");
							CString saveName = pOutput->analysis->autosavePath + "PocketCard." + gSet.GetGraphicsExtension();
							CRect rect(0, 0, gSet.m_AutoSaveWidth, gSet.m_AutoSaveHeight);
							pctView->SaveToFile(saveName, &rect);
							delete pctView;
							nReports++;*/
							pOutput->killData = FALSE;
						}
						rpcSet.Close();
					}
					break;
				case REPORT_WEATHER_PERSISTENCE_PROB:
					{
						//saveRun.SaveGeneral(0, runDesc, fpSet);
						pOutput->analysis->fpSet = tfpSet;
						pOutput->analysis->killfpSet = true;
						//pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
						saveRun.RestoreClimateOptions(pOutput->runID, tfpSet);
						saveRun.RestoreWxStation(pOutput->runID, tfpSet);
						// removed 8/2006 saveRun.SaveWxStation(curr_runID, fpSet);
						//saveRun.SaveFireOptions(curr_runID, fpSet);
						// save variable
						CString value1;
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_PERSISTENCE_PROB,0,"varID",&value1,tfpSet);

						// restore bin values
						CSeasonBinSet binSet(m_pDB);
						binSet.m_strFilter.Format("[VarID] = %d", atoi(value1));
						binSet.Open();
						if(binSet.IsEOF())
						{
							binSet.AddNew();
							binSet.m_VarID = atoi(value1);
						}
						else
							binSet.Edit();
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin1Min", &value1, tfpSet);
						binSet.m_Bin1Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin1Max", &value1, tfpSet);
						binSet.m_Bin1Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin2Min", &value1, tfpSet);
						binSet.m_Bin2Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin2Max", &value1, tfpSet);
						binSet.m_Bin2Max = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin3Min", &value1, tfpSet);
						binSet.m_Bin3Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin3Max", &value1, tfpSet);
						binSet.m_Bin3Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin4Min", &value1, tfpSet);
						binSet.m_Bin4Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin4Max", &value1, tfpSet);
						binSet.m_Bin4Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin5Min", &value1, tfpSet);
						binSet.m_Bin5Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin5Max", &value1, tfpSet);
						binSet.m_Bin5Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin6Min", &value1, tfpSet);
						binSet.m_Bin6Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin6Max", &value1, tfpSet);
						binSet.m_Bin6Max = atof(value1);
						binSet.Update();
						//pOutput->analysis->autosave = true;
						pOutput->analysis->PersistenceProbability(&binSet);
						binSet.Close();
						nReports ++;
					}
					break;
				case REPORT_WEATHER_SEVERITY_LIST:
				case REPORT_WEATHER_SEVERITY_SUMM:
					{
						saveRun.RestoreClimateOptions(pOutput->runID, tfpSet);
						saveRun.RestoreWxStation(pOutput->runID, tfpSet);
						CClimateSet *climSet = new CClimateSet(m_pDB);
						climSet->m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
						climSet->m_strSort = "[VarID]";
						climSet->Open();
						//pOutput->analysis->autosave = true;
						//pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
						pOutput->analysis->fpSet = tfpSet;
						pOutput->analysis->killfpSet = true;
						pOutput->analysis->climSet = climSet;
						CString value1;
						double cp;
						bool greater;
						int vLoc;
						saveRun.RestoreRunOption(pOutput->runID,  distinctReports.m_reportID, 0, "varID", &value1, tfpSet);
						vLoc = atoi(value1);	
						// CP
						//value1 = "1";
						//if (m_spinCP.GetPos())
						//	value1.Format("%3d", m_spinCP.GetPos());
						saveRun.RestoreRunOption(pOutput->runID, distinctReports.m_reportID,0,"cp",&value1,tfpSet);
						cp = atof(value1);
						// operator greaterthan/lessthan
						//value1.Format("%2d",id);
						saveRun.RestoreRunOption(pOutput->runID, distinctReports.m_reportID,0,"radiobutton",&value1,tfpSet);
						greater = (bool)atoi(value1);
						if(distinctReports.m_reportID == REPORT_WEATHER_SEVERITY_LIST)
							pOutput->analysis->SeverityList(cp, greater, vLoc); 
						else
							pOutput->analysis->SeveritySummary(cp, greater, vLoc);
						nReports ++;
						climSet->Close();
						delete climSet;
					}
					break;
				case REPORT_WEATHER_PROBABILITY_ANAL:
					{
						saveRun.RestoreClimateOptions(pOutput->runID, tfpSet);
						saveRun.RestoreWxStation(pOutput->runID, tfpSet);
						CClimateSet *climSet = new CClimateSet(m_pDB);
						climSet->m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
						climSet->m_strSort = "[VarID]";
						climSet->Open();
						pOutput->analysis->autosave = false;
						//pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
						pOutput->analysis->fpSet = tfpSet;
						pOutput->analysis->killfpSet = true;
						pOutput->analysis->climSet = climSet;
						CString value1;
						int varID1, varID2;
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_PROBABILITY_ANAL,0,"varID1",&value1,tfpSet);
						varID1 = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_PROBABILITY_ANAL,0,"varID2",&value1,tfpSet);
						varID2 = atoi(value1);
						// restore bin values
						CSeasonBinSet binSet(m_pDB);
						binSet.m_strFilter.Format("[VarID] = %d", varID1);
						binSet.Open();
						if(binSet.IsEOF())
						{
							binSet.AddNew();
							binSet.m_VarID = varID1;
						}
						else
							binSet.Edit();
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin1Min", &value1, tfpSet);
						binSet.m_Bin1Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin1Max", &value1, tfpSet);
						binSet.m_Bin1Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin2Min", &value1, tfpSet);
						binSet.m_Bin2Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin2Max", &value1, tfpSet);
						binSet.m_Bin2Max = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin3Min", &value1, tfpSet);
						binSet.m_Bin3Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin3Max", &value1, tfpSet);
						binSet.m_Bin3Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin4Min", &value1, tfpSet);
						binSet.m_Bin4Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin4Max", &value1, tfpSet);
						binSet.m_Bin4Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin5Min", &value1, tfpSet);
						binSet.m_Bin5Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin5Max", &value1, tfpSet);
						binSet.m_Bin5Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin6Min", &value1, tfpSet);
						binSet.m_Bin6Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin6Max", &value1, tfpSet);
						binSet.m_Bin6Max = atof(value1);
						binSet.Update();
						binSet.m_strFilter.Format("[VarID] = %d", varID2);
						binSet.Requery();
						if(binSet.IsEOF())
						{
							binSet.AddNew();
							binSet.m_VarID = varID2;
						}
						else
							binSet.Edit();
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin1Min", &value1, tfpSet);
						binSet.m_Bin1Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin1Max", &value1, tfpSet);
						binSet.m_Bin1Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin2Min", &value1, tfpSet);
						binSet.m_Bin2Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin2Max", &value1, tfpSet);
						binSet.m_Bin2Max = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin3Min", &value1, tfpSet);
						binSet.m_Bin3Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin3Max", &value1, tfpSet);
						binSet.m_Bin3Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin4Min", &value1, tfpSet);
						binSet.m_Bin4Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin4Max", &value1, tfpSet);
						binSet.m_Bin4Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin5Min", &value1, tfpSet);
						binSet.m_Bin5Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin5Max", &value1, tfpSet);
						binSet.m_Bin5Max = atof(value1);

						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin6Min", &value1, tfpSet);
						binSet.m_Bin6Min = atof(value1);
						saveRun.RestoreRunOption(pOutput->runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin6Max", &value1, tfpSet);
						binSet.m_Bin6Max = atof(value1);
						binSet.Update();
						binSet.Close();

						CSeasonBinSet set1(m_pDB), set2(m_pDB);
						set1.m_strFilter.Format("[VarID] = %d", varID1);
						set2.m_strFilter.Format("[VarID] = %d", varID2);
						set1.Open();
						set2.Open();
						pOutput->analysis->ProbabilityAnalysis(&set1, &set2, varID1 - 1, varID2 - 1);
						set1.Close();
						set2.Close();
						nReports ++;
						climSet->Close();
						delete climSet;
					}
					break;
				case REPORT_WEATHER_DAILY_LISTING:
					{
						saveRun.RestoreClimateOptions(pOutput->runID, tfpSet);
						saveRun.RestoreWxStation(pOutput->runID, tfpSet);
						CClimateSet *climSet = new CClimateSet(m_pDB);
						climSet->m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
						climSet->m_strSort = "[VarID]";
						climSet->Open();
						pOutput->analysis->autosave = false;
						//pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
						pOutput->analysis->fpSet = tfpSet;
						pOutput->analysis->killfpSet = true;
						pOutput->analysis->climSet = climSet;
						ListingOptions lo;
						CString value1;
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING, 0, 
							"checkbox_TotalAcres",&value1,tfpSet);
						lo.acres = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_NumPerClass", &value1, tfpSet);
						lo.numPerClass = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_NumFires", &value1, tfpSet);
						lo.fires = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_NumLargeFires", &value1, tfpSet);
						lo.largeFires = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"spin_LFDAcres", &value1, tfpSet);
						lo.lfAcres = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"radio_fireCause", &value1, tfpSet);
						lo.fCause = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_reportHeader", &value1, tfpSet);
						lo.reportHeader = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_columnHeader", &value1, tfpSet);
						lo.columnHeader = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_dateStamp", &value1, tfpSet);
						lo.dateStamp = atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"radio_DateFormat", &value1, tfpSet);
						lo.dt = (DateType)atoi(value1);
						saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"radio_TimeFormat", &value1, tfpSet);
						lo.tt = (TimeType)atoi(value1);
						if(saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_StnPerRecord", &value1, tfpSet) != 0)
						{
							lo.stnPerRecord = atoi(value1);
						}
						bool exportToDB = false;
						if(saveRun.RestoreRunOption(pOutput->runID, REPORT_WEATHER_DAILY_LISTING,0,
							"checkbox_ExportToDB", &value1, tfpSet) != 0)
						{
							exportToDB = true;
						}

						pOutput->analysis->DailyListing(&lo, true,exportToDB);//(climSet);
						nReports ++;
						climSet->Close();
						delete climSet;
					}
					break;
				case REPORT_WEATHER_PERCENTILE_WEATHER:
				case REPORT_WEATHER_WINDS:
				case REPORT_FIRES_SUMMARY:
					break;
				case REPORT_FIRES_ANALYSIS:
					{
						//pOutput->analysis->autosavePath.Format("%s\\Batch%d\\Run%d\\", savePath, batchID, pOutput->runID);
						pOutput->analysis->fpSet = tfpSet;

						pOutput->analysis->killfpSet = true;
						saveRun.RestoreFireOptions(pOutput->runID, tfpSet);
						//pOutput->analysis->F
						nReports ++; 
					}
					break;
				default:
					//do nothing
					break;
				}
				distinctReports.MoveNext();
			}
			distinctReports.Close();
			//cycle thru reports....
			//CDistinctRunsSet distinctRuns(m_pDB);
			//distinctRuns.Open(CRecordset::dynaset, "SELECT DISTINCT ffpRunOptions.runID FROM ffpRunOptions ORDER BY ffpRunOptions.runID;");
			//while(!distinctRuns.IsEOF())
			//{
			//CRunOptionsSet runOpts(m_pDB);
			//runOpts.Open();
			//runOpts.m_strFilter.Format("{runID] = %d", pOutput->runID)
			//saveRun.RestoreClimateOptions(pOutput->runID, tfpSet);
			// removed 8/2006 saveRun.RestoreWxStation(curr_runID, tfpSet);
			//saveRun.RestoreFireOptions(pOutput->runID, tfpSet);
			//tfpSet->Close();
			//delete tfpSet;
		}
	}
	/*CString msg;
	msg.Format("Batch Autosave Run Complete\n%d reports and graphs created.\n"
		"Output saved to: %sBatch%d",
		nReports, savePath, batchID);
	AfxMessageBox(msg);*/
}
// CBatchOutputDoc

IMPLEMENT_DYNCREATE(CBatchOutputDoc, CDocument)

CBatchOutputDoc::CBatchOutputDoc()
{
}

BOOL CBatchOutputDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CBatchOutputDoc::~CBatchOutputDoc()
{
}


BEGIN_MESSAGE_MAP(CBatchOutputDoc, CDocument)
END_MESSAGE_MAP()


// CBatchOutputDoc diagnostics

#ifdef _DEBUG
void CBatchOutputDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBatchOutputDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CBatchOutputDoc serialization

void CBatchOutputDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CBatchOutputDoc commands


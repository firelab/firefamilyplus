// NelsonDFM.cpp : implementation file
//
#include "stdafx.h"
#include "fireplus.h"
#include "fireplusSet.h"
#include "fireplusDoc.h"
#include "fireplusView.h"
#include "climateSet.h"
#include "wxSet.h"
#include "reportoptionsset.h"
#include "stationinsigset.h"
#include "wxset.h"
#include <afxrich.h>
#include "richdoc.h"
//#include "nfdrs4ctrl1.h"
#include "mainfrm.h"
#include "NelsonDFM.h"
#include "DeadFuelMoisture.h"
#include "SemTime.h"
//#include "NelsonDialog.h"
#include "SIGStationSet.h"
#include "CustomInitSet.h"
#include "DeadFuelMoisture.h"
#include <direct.h>

//nelsen model gap tolerances in hours... when exceeded reinit with last moisture
#define NELSEN_1HR_GAP    24
#define NELSEN_10HR_GAP    48
#define NELSEN_100HR_GAP    120
#define NELSEN_1000HR_GAP    336

// using namespace Sem;

extern CFireplusApp theApp;
extern int curr_runID;
extern int DayOfYear(COleDateTime date);
extern COleDateTime DateFromDOY(int year, int julianDay);
extern double Round(double in);

CNelsonDFM::CNelsonDFM(CWnd* pParent )
{
	parent = pParent;
	m_fName = NULL;
    CRect nRect(0, 0, 1, 1);
	vals = NULL;
	nYears = 0;
	valsPerYear = 0;
	fires = NULL;
	isHalfNelson = false;
	m_wxSet = NULL;
	recCount = 0;
}

CNelsonDFM::~CNelsonDFM()
{
	if(vals)
	{
		for(int y = 0; y < nYears; y++)
			delete[] vals[y];
		delete[] vals;
	}
	vals = NULL;
	if(fires)
	{
		for(int y = 0; y < nYears; y++)
			delete[] fires[y];
		delete[] fires;
	}
	if(m_wxSet)
	{
		if(m_wxSet->IsOpen())
			m_wxSet->Close();
		delete m_wxSet;
	}
}

bool NelsonMoisturesRunProc(const CUPDUPDATA* pCUPDUPData)
{
	CNelsonDFM* pNDFM = (CNelsonDFM*)pCUPDUPData->GetAppData();
	pNDFM->pCUPDUPData = (CUPDUPDATA*)pCUPDUPData;
	pCUPDUPData->SetProgress(_T("Running Nelson DFM model..."),0);
	pNDFM->NelsonMoisturesReport();

	return true;
}

bool NFDRSNelsonRunProc(const CUPDUPDATA* pCUPDUPData)
{
	CNelsonDFM* pNDFM = (CNelsonDFM*)pCUPDUPData->GetAppData();
	pNDFM->pCUPDUPData = (CUPDUPDATA*)pCUPDUPData;
	pCUPDUPData->SetProgress(_T("Running Nelson DFM model..."),0);
	pNDFM->NFDRSNelsonReport();

	return true;
}

bool CalcNelsonMoisturesRunProc(const CUPDUPDATA* pCUPDUPData)
{
	CNelsonDFM* pNDFM = (CNelsonDFM*)pCUPDUPData->GetAppData();
	pNDFM->pCUPDUPData = (CUPDUPDATA*)pCUPDUPData;
	pCUPDUPData->SetProgress(_T("Running Nelson DFM model..."),0);
	pNDFM->FillNelsonVals();

	return true;
}


void CNelsonDFM::NelsonMoisturesReport()
{
	m_fName = GetTempFileName();
	FILE *stream = fopen(m_fName, "wt");



	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(stream, "FireFamily Plus - Nelson Dead Fuel Moistures\n");




	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());
	fprintf(stream, "\n   Initialization Moistures: 1hr = %.2f  10hr = %.2f  100hr = %.2f  1000hr = %.2f\n",
		m_start1*100.0,m_start10*100.0,m_start100*100.0,m_start1000*100.0);


	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	reportSet.Close();
	double precipCounter = 0.0;
	/* create 'sticks' */
	DeadFuelMoisture *stick1h = DeadFuelMoisture::createDeadFuelMoisture1("1h");
	DeadFuelMoisture *stick10h = DeadFuelMoisture::createDeadFuelMoisture10();
	DeadFuelMoisture *stick100h = DeadFuelMoisture::createDeadFuelMoisture100();
	DeadFuelMoisture *stick1000h = DeadFuelMoisture::createDeadFuelMoisture1000();

	// vars
	int year, month, day, hour, minute, second, state1, state10, state100, state1000;
	double tempC, rh, solar, cumCm,m1,m10,m100,m1000;

	int record = 0;
	fprintf(stream,"\n");
	fprintf(stream,"Obs. timestamp       Weather Data            Nelson moistures    \n");
	fprintf(stream,"YYYY/MM/DD hh:mm TempF   RH  HrRa SolRad    1h  10h 100h 1000h   \n");

	int qStartMonth = MonthChtoInt(fpSet->m_StartMonth),
		qEndMonth = MonthChtoInt(fpSet->m_EndMonth),
		qStartDay = fpSet->m_StartDay,
		qEndDay = fpSet->m_EndDay,
		qStartYear = fpSet->m_StartYear;
	CString query, tStr;
	fpSet->BuildBaseQuery(query, "ObsDate", true);

	COleDateTime wxDay, yesterday, lastNelsenTime;
	bool init = true;
	m_wxSet->MoveFirst();
	while (! m_wxSet->IsEOF() && !pCUPDUPData->ShouldTerminate())//.Abort())
	{
		if (m_wxSet->IsFieldNull(&m_wxSet->m_SolarRadiation) ||
			m_wxSet->IsFieldNull(&m_wxSet->m_Temp) ||
			m_wxSet->IsFieldNull(&m_wxSet->m_RH) ||
			m_wxSet->IsFieldNull(&m_wxSet->m_HourlyPrecip))
		{
			m_wxSet->MoveNext();
			continue;
		}

		wxDay.SetDateTime(m_wxSet->m_ObsDate.GetYear(), m_wxSet->m_ObsDate.GetMonth(), m_wxSet->m_ObsDate.GetDay(), 
			m_wxSet->m_ObsDate.GetHour(), m_wxSet->m_ObsDate.GetMinute(), m_wxSet->m_ObsDate.GetSecond());
		if(!init && yesterday >= wxDay)
		{
			m_wxSet->MoveNext();
			continue;
		}
		yesterday = wxDay;
		init = false;
		//Nelson variables
		year = m_wxSet->m_ObsDate.GetYear();
		month = m_wxSet->m_ObsDate.GetMonth();
		day = m_wxSet->m_ObsDate.GetDay();
		hour = m_wxSet->m_ObsDate.GetHour();
		minute = m_wxSet->m_ObsDate.GetMinute();
		second = m_wxSet->m_ObsDate.GetSecond();
		if((month < qStartMonth || month == qStartMonth && day < qStartDay)
			&& year == qStartYear)
		{
			m_wxSet->MoveNext();
			continue;
		}
		tempC = ((double) (m_wxSet->m_Temp - 32)) * 5.0 / 9.0;
		rh = ((double) (max(m_wxSet->m_RH, 1))) / 100.0;


		solar = (double) (m_wxSet->m_SolarRadiation);

		precipCounter += (m_wxSet->m_HourlyPrecip) * 2.54;
		cumCm = precipCounter;




		if ( ! record)
		{
			stick1h->initializeEnvironment( year, month, day, hour, minute,
				second, tempC, rh, solar, cumCm, tempC, rh, m_start1 );
			stick10h->initializeEnvironment( year, month, day, hour, minute,
				second, tempC, rh, solar, cumCm, tempC, rh, m_start10 );
			if(!isHalfNelson)
			{
				stick100h->initializeEnvironment( year, month, day, hour, minute,
					second, tempC, rh, solar, cumCm, tempC, rh, m_start100 );
				stick1000h->initializeEnvironment( year, month, day, hour, minute,
					second, tempC, rh, solar, cumCm, tempC, rh, m_start1000 );
			}
			//fprintf(stream,"DeadFuelMoistures initialized: %4d/%02d/%02d %02d:%02d \n",
			//	   year,month,day,hour,minute); 
		}
		else
		{
			COleDateTimeSpan gap = wxDay - lastNelsenTime;
			if(gap.GetTotalHours() < NELSEN_1HR_GAP)
				stick1h->update(  year, month, day, hour, minute, second, tempC,
					rh, solar, cumCm );
			else
			{
				stick1h->initializeEnvironment( year, month, day, hour, minute,
					second, tempC, rh, solar, cumCm, tempC, rh, stick1h->meanWtdMoisture() );
			}
			if(gap.GetTotalHours() < NELSEN_10HR_GAP)
				stick10h->update(  year, month, day, hour, minute, second, tempC,
					rh, solar, cumCm );
			else
			{
				stick10h->initializeEnvironment( year, month, day, hour, minute,
					second, tempC, rh, solar, cumCm, tempC, rh, stick10h->meanWtdMoisture() );
			}
			if(!isHalfNelson)
			{
				if(gap.GetTotalHours() < NELSEN_100HR_GAP)
					stick100h->update(  year, month, day, hour, minute, second, tempC,
						rh, solar, cumCm );
				else
				{
					stick100h->initializeEnvironment( year, month, day, hour, minute,
						second, tempC, rh, solar, cumCm, tempC, rh, stick100h->meanWtdMoisture() );
				}
				if(gap.GetTotalHours() < NELSEN_1000HR_GAP)
					stick1000h->update(  year, month, day, hour, minute, second, tempC,
						rh, solar, cumCm );
				else
				{
					stick1000h->initializeEnvironment( year, month, day, hour, minute,
						second, tempC, rh, solar, cumCm, tempC, rh, stick1000h->meanWtdMoisture() );
				}
			}
		}
		lastNelsenTime = wxDay;


		//if(m_wxSet->m_ObsType.CompareNoCase("O") == 0 &&((qStartMonth < qEndMonth || (qStartMonth == qEndMonth && qStartDay <= qEndDay))//normal ordering
		if(((qStartMonth < qEndMonth || (qStartMonth == qEndMonth && qStartDay <= qEndDay))//normal ordering
			&& ((qStartMonth < month && qEndMonth > month)
			|| ((qStartMonth == month && qStartDay <= day)
			|| (qEndMonth == month && qEndDay >= day))))
			||((qStartMonth > qEndMonth || (qStartMonth == qEndMonth && qStartDay >= qEndDay))//flipped ordering
			&& ((qStartMonth < month || qEndMonth > month)
			|| (qStartMonth == month && qStartDay <= day)
			|| (qEndMonth == month && qEndDay >= day))))
			//included in date time of year query...
		{
			m1 = stick1h->meanWtdMoisture();//meanMoisture();
			m10 = stick10h->meanWtdMoisture();//meanMoisture();
			m100 = stick100h->meanWtdMoisture();//meanMoisture();
			m1000 = stick1000h->meanWtdMoisture();//meanMoisture();
			state1 = stick1h->state();
			state10 = stick10h->state();
			if(!isHalfNelson)
			{
				state100 = stick100h->state();
				state1000 = stick1000h->state();
			}
			fprintf(stream,"%4d/%02d/%02d %02d:%02d  %4d %4d %5.2f %6d  %5.1f %5.1f %5.1f %5.1f\n",// %d %d %d %d\n",
				year,month,day,hour,minute, 
				m_wxSet->m_Temp, max(m_wxSet->m_RH, 1), m_wxSet->m_HourlyPrecip, m_wxSet->m_SolarRadiation,
				m1 * 100.0, m10 * 100.0, m100 * 100.0, m1000 * 100.0);//,
				//state1, state10, state100, state1000);
		}

		
		if(!pCUPDUPData->ShouldTerminate())
		{
			record ++;
			//pCUPDUPData->Increment();
			if (!(record % 10))
			{
		       char tmp[256];
		       sprintf(tmp, "Processing record %d", record);
		       pCUPDUPData->SetProgress(tmp);
		       pCUPDUPData->SetProgress( (double)record / (double)recCount * 100.0);
			}
			m_wxSet->MoveNext();
		}
	}

	if (! record)
		fprintf(stream,"\nNo usable hourly data found.\n\n");
	fclose(stream);


	if (stick1h)
		delete stick1h;
	if (stick10h)
		delete stick10h;
	if (stick100h)
		delete stick100h;
	if (stick1000h)
		delete stick1000h;
}

// this report is a simple listing of hourly nelson values
void CNelsonDFM::RunReport()
{
	CWaitCursor wait;
	if (! fpSet)
		return;
	CSIGStationSet staSet(fpSet->m_pDatabase);
	staSet.m_strFilter.Format("[StationID]='%s'",fpSet->m_SIG_Station);
	staSet.Open();
	if (staSet.IsEOF()){
		AfxMessageBox("ERROR: Weather station data not found.");
		staSet.Close();
		return;
	}
	int sKBDI = 0;
	if(!staSet.IsFieldNull(&staSet.m_StartKBDI))
		sKBDI = staSet.m_StartKBDI;

	COleDateTime yesterday, wxDay;//used to skip duplicates

	int tmp, tmpMax, tmpMin, rh1, rhMax, rhMin, pptDur, sow, greenHerb, greenShrub,
		season, windSpd, windDir, slopeClass, wetFlag, snowFlag;
	double pptAmt, omc10; 
	int  iRainEvent, iStage, 
		iKBDI, init = 1, iDeclareGreenUp = 0, iDeclareFreeze = 0;//,
	double f1 = 20.0, f10 = 20.0, f100 = 20.0, f1000 = 20.0, fHerb = 20.0, fWood = 20.0, fGren = 20.0, fX1000 = 20.0;

	// need one calculator to compute starting moistures
	//CNfdrs4ctrl1 calcOld;
	//CRect nRect(0, 0, 1, 1);
	//calcOld.Create(NULL, WS_CHILD,//WS_DISABLED,
	//	nRect, parent, IDC_NFDRS2); 
	/* open wxObs */
	CString query, tStr;
	fpSet->BuildBaseQuery(query, "ObsDate", true);

	tStr = fpSet->m_SIG_Station;
	tStr.TrimRight();
	CStationInSIGSet sSet(fpSet->m_pDatabase);

	if(tStr.GetLength() > 6)
	{
		// can't do SIGS
		AfxMessageBox("The Nelson Dead Fuel Moisture report cannot be run on a SIG.");

		return;
	}
	else
	{
		CString sta;
		sta.Format(" and [StationID] = '%6.6s'", tStr);
		query += sta;
	}

	m_wxSet = new CWxSet(fpSet->m_pDatabase);
	m_wxSet->m_strFilter = query;
	m_wxSet->m_strSort = "[ObsDate]";
	m_wxSet->Open();
	//find first solar radiation day
	COleDateTime firstSolRadDay;
	bool hasSolRad = false;
	recCount = 0;
	while(!m_wxSet->IsEOF())
	{
		if(!hasSolRad && !m_wxSet->IsFieldNull(&m_wxSet->m_SolarRadiation) && !m_wxSet->IsFieldNull(&m_wxSet->m_HourlyPrecip))		
		{
			firstSolRadDay = m_wxSet->m_ObsDate;
			hasSolRad = true;
			//break;
		}
		recCount++;
		m_wxSet->MoveNext();
	}
	if(!hasSolRad)
	{
		//m_wxSet->Close();
		//delete m_wxSet;
		AfxMessageBox("Error: No valid weather records with Solar Radiation field populated.");
		return;
	}
	m_wxSet->MoveFirst();
	//need to get starting moistures from NFDRS for that day, then present the moisture dialog
	m_calcOld.iInitialize((staSet.m_Use88 ) ? 88 : 78,//staSet.m_Use88 ? 88 : 78, 
		staSet.m_ClimateCls, 0, 0,
		(int) (staSet.m_LatDegrees), staSet.m_HerbAnnual, staSet.m_Deciduous, staSet.m_FM1Eq10, 
		staSet.m_Start1000 - 5.0, staSet.m_Start1000, sKBDI, staSet.m_AvgPrecip);
	m_calcOld.iSetFuelModel(staSet.m_NFDRSFM[0],0);
	if(!staSet.IsFieldNull(&staSet.m_UseDormant) && !staSet.IsFieldNull(&staSet.m_DormantJulian))
	{
		if(staSet.m_UseDormant && staSet.m_DormantJulian.GetStatus() == COleDateTime::valid)
			m_calcOld.iSetJulianShrubDormant(DayOfYear(staSet.m_DormantJulian));
	}
	bool bUseStick = true;
	if(!staSet.IsFieldNull(&staSet.m_UseStick))
		bUseStick = staSet.m_UseStick;
	while(!m_wxSet->IsEOF() && (m_wxSet->m_ObsDate < firstSolRadDay || init))
	{
		if(m_wxSet->IsFieldNull(&m_wxSet->m_Temp) 
			|| m_wxSet->IsFieldNull(&m_wxSet->m_RH)
			|| m_wxSet->IsFieldNull(&m_wxSet->m_ObsDate))
		{
			m_wxSet->MoveNext();
			continue;
		}
		if(m_wxSet->m_ObsType.CompareNoCase("O") == 0)
		{
			wxDay.SetDateTime(m_wxSet->m_ObsDate.GetYear(), m_wxSet->m_ObsDate.GetMonth(), m_wxSet->m_ObsDate.GetDay(), 
				m_wxSet->m_ObsDate.GetHour(), m_wxSet->m_ObsDate.GetMinute(), m_wxSet->m_ObsDate.GetSecond());
			if(!init && yesterday >= wxDay)
			{
				m_wxSet->MoveNext();
				continue;
			}
			// new custom init check    12/2006
			// if this is first record for new year, look for inits
			if (yesterday.GetYear() != wxDay.GetYear())
			{
				
                CCustomInitSet ciSet(fpSet->m_pDatabase);
				ciSet.m_strFilter.Format("[SIG_Station]='%s' AND [Year]=%d and [Enabled] <> 0", m_wxSet->m_StationID,wxDay.GetYear());
				ciSet.Open();
				bool usedNFDRScust = false, usedCANcust = false;
				if (! ciSet.IsEOF())
				{
                    // enabled record exists for this station/year. get values and re-initialize
                  // int newGreen;//, lookFreeze;

				   if (!ciSet.IsFieldNull(&ciSet.m_GreenJulian) &&
					   !ciSet.IsFieldNull(&ciSet.m_StartKBDI) &&
					   !ciSet.IsFieldNull(&ciSet.m_Start1000))
				   {
		                int newGreen = DayOfYear(ciSet.m_GreenJulian);
		           
						theApp.m_NFDRS.iSetAnnualInit(newGreen, ciSet.m_Start1000, ciSet.m_StartKBDI);
						usedNFDRScust = true;
						//customInits = true;

				   } 
				  /* if(useCanadian)
				   {
						if (!ciSet.IsFieldNull(&ciSet.m_FFMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DC))
						{
		                   canadian.ReInitialize(ciSet.m_FFMC,ciSet.m_DMC,ciSet.m_DC);
						   usedCANcust = true;
						  // customInits = true;
						}
				   }*/
				}
				if(!usedNFDRScust)
				{
					//set to default station values
					theApp.m_NFDRS.iSetAnnualInit(DayOfYear(staSet.m_GreenJulian), staSet.m_Start1000, sKBDI);
				}
				//if(!usedCANcust)
				//	canadian.ReInitialize(85, 6, 15);
                ciSet.Close();
			}
			yesterday = wxDay;//wxSet->m_ObsDate;
			tmp = m_wxSet->m_Temp;
			tmpMax = m_wxSet->IsFieldNull(&m_wxSet->m_TmpMax) ? -10000 : m_wxSet->m_TmpMax;
			tmpMin = m_wxSet->IsFieldNull(&m_wxSet->m_TmpMin) ? -10000 : m_wxSet->m_TmpMin;
			rh1 = max(m_wxSet->m_RH, 1);
			rhMax = m_wxSet->IsFieldNull(&m_wxSet->m_RHMax) ? -10000 : m_wxSet->m_RHMax;
			rhMin = m_wxSet->IsFieldNull(&m_wxSet->m_RHMin) ? -10000 : max(m_wxSet->m_RHMin, 1);
			pptDur = m_wxSet->IsFieldNull(&m_wxSet->m_PPTDUR) ? 0 : m_wxSet->m_PPTDUR;
			sow = m_wxSet->ResolveSOW();//IsFieldNull(&m_wxSet->m_SOW) ? 0 : m_wxSet->m_SOW;
			greenHerb = m_wxSet->IsFieldNull(&m_wxSet->m_GreenHerb) ? 0 : m_wxSet->m_GreenHerb;
			greenShrub = m_wxSet->IsFieldNull(&m_wxSet->m_GreenShrub) ? 0 : m_wxSet->m_GreenShrub;
			season = m_wxSet->IsFieldNull(&m_wxSet->m_Season) ? 0 : m_wxSet->m_Season;
			windSpd = m_wxSet->IsFieldNull(&m_wxSet->m_WS) ? 0 : m_wxSet->m_WS;
			windDir = m_wxSet->IsFieldNull(&m_wxSet->m_WDir) ? 0 : m_wxSet->m_WDir;
			slopeClass = staSet.m_SlopeCls;
			slopeClass = max(slopeClass, 1);
			slopeClass = min(slopeClass, 5);
			pptAmt = m_wxSet->IsFieldNull(&m_wxSet->m_PPTAMT) ? 0.0 : m_wxSet->m_PPTAMT;
			//omc10 = m_wxSet->IsFieldNull(&m_wxSet->m_OMC10) ? -1000.0 : m_wxSet->m_OMC10;
			omc10 = (bUseStick && !m_wxSet->IsFieldNull(&m_wxSet->m_OMC10)) ? m_wxSet->m_OMC10 : -1000.0;
			wetFlag = m_wxSet->ResolveWetFlag();//IsFieldNull(&m_wxSet->m_WetFlag) ? 0 : m_wxSet->m_WetFlag;
			snowFlag = m_wxSet->IsFieldNull(&m_wxSet->m_SnowFlag) ? 0 : m_wxSet->m_SnowFlag;
			m_calcOld.iCalcMoistWetFlagSnowFlag(init, tmp, rh1, 
				tmpMax, 
				rhMax, 
				tmpMin, 
				rhMin, 
				pptDur, 
				pptAmt, 
				sow, 
				omc10, 
				DayOfYear(m_wxSet->m_ObsDate), 
				m_wxSet->m_ObsDate.GetYear(), 
				iDeclareGreenUp, 
				iDeclareFreeze, 
				greenHerb, 
				greenShrub,
				season,
				wetFlag, snowFlag,
				&f1, &f10, &f100, &f1000, &fHerb, &fWood, &iStage, &fGren, &fX1000, &iRainEvent, &iKBDI);
			init = 0;
		}
		m_wxSet->MoveNext();
	}
	/*CNelsonDialog nd;
	nd.start1h = (int) (f1);
	nd.start10h = (int) (f10);
	nd.start100h = (int) (f100);
	nd.start1000h = (int) (f1000);
	int ndres = nd.DoModal();
	if (ndres == IDCANCEL)
	{
		m_wxSet->Close();
		delete m_wxSet;
		return;
	}
	m_start1 = (double) (nd.start1h) / 100.0;
	m_start10 = (double) (nd.start10h) / 100.0;
	m_start100 = (double) (nd.start100h) / 100.0;
	m_start1000 = (double) (nd.start1000h) / 100.0;*/
	m_start1 = f1 / 100.0;
	m_start10 = f10 / 100.0;
	m_start100 = f100 / 100.0;
	m_start1000 = f1000 / 100.0;
	CUPDialog cupDlg(AfxGetMainWnd()->m_hWnd, NelsonMoisturesRunProc, this);
	INT_PTR ret = cupDlg.DoModal();
	//m_wxSet->Close();
	//delete m_wxSet;
	if(ret != IDOK)
	{
		if(m_fName)
		{
			free(m_fName);
			m_fName = NULL;
		}
	}
//	pCUPDUPData->SetExecutionComplete(1);
	//now display the file in a child window
	if(m_fName)
	{
		POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

		while(curTemplatePos != NULL){
			CDocTemplate* curTemplate = 
				theApp.GetNextDocTemplate(curTemplatePos);
			CString str;
			curTemplate->GetDocString(str, CDocTemplate::docName);
			if(str == _T("TextFile")){
				CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(m_fName);
				pDoc->isTemp = true;
				free(m_fName);
				m_fName = NULL;
				CString tTitle;
				tTitle.Format("%s - Nelson Dead Fuel Moistures", fpSet->m_SIG_Station);
				pDoc->SetTitle(tTitle);

			} 
		}
	}

}

void CNelsonDFM::NFDRSNelsonReport()
{
	CSIGStationSet staSet(fpSet->m_pDatabase);
	staSet.m_strFilter.Format("[StationID]='%s'",fpSet->m_SIG_Station);
    staSet.Open();
	int sKBDI = 0;
	if(!staSet.IsFieldNull(&staSet.m_StartKBDI))
		sKBDI = staSet.m_StartKBDI;
	m_fName = GetTempFileName();
	FILE *stream = fopen(m_fName, "wt");
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(stream, "FireFamily Plus - Nelson Dead Fuel Moistures\n");
	if(isHalfNelson)
		fprintf(stream,
			"\tUsing 1hr and 10hr Nelson Moistures\n");
	else 
		fprintf(stream,
			"\tUsing 1hr, 10hr, 100hr and 1000hr Nelson Moistures\n");

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());
	
	fprintf(stream, "\n\   Initialization Moistures: 1hr = %.2f  10hr = %.2f  100hr = %.2f  1000hr = %.2f\n",
		m_start1*100.0,m_start10*100.0,m_start100*100.0,m_start1000*100.0);

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	reportSet.Close();
	int record = 0;
	fprintf(stream,"\n");
	fprintf(stream,"Obs. timestamp      Weather Data            NFDRS Moistures      Nelson moistures      ....NFDRS indices     vs      Nelson indices....\n");
    fprintf(stream,"YYYY/MM/DD hh:mm  TempF   RH  24Hr CumRa   1h  10h 100h 1000h  1h  10h 100h 1000h      SC -  SC    ERC  - ERC      BI  -  BI    IC - IC\n");

	/* set up initial precip counter for Nelson model */
	double precipCounter = 0.0;
    /* create 'sticks' */
	DeadFuelMoisture *stick1h = DeadFuelMoisture::createDeadFuelMoisture1();
	DeadFuelMoisture *stick10h = DeadFuelMoisture::createDeadFuelMoisture10();
	DeadFuelMoisture *stick100h = DeadFuelMoisture::createDeadFuelMoisture100();
	DeadFuelMoisture *stick1000h = DeadFuelMoisture::createDeadFuelMoisture1000();

	int year, month, day, hour, minute, second;
    double tempC, rh, solar, cumCm,m1,m10,m100,m1000;
 
//	int record = 0;
	m_calcOld.iInitialize((staSet.m_Use88 ) ? 88 : 78,//staSet.m_Use88 ? 88 : 78, 
			staSet.m_ClimateCls, 0, 0,
			(int) (staSet.m_LatDegrees), staSet.m_HerbAnnual, staSet.m_Deciduous, staSet.m_FM1Eq10, 
			staSet.m_Start1000 - 5.0, staSet.m_Start1000, sKBDI, staSet.m_AvgPrecip);
	m_calcOld.iSetFuelModel(staSet.m_NFDRSFM[0],0);
	if(!staSet.IsFieldNull(&staSet.m_UseDormant) && !staSet.IsFieldNull(&staSet.m_DormantJulian))
	{
		if(staSet.m_UseDormant && staSet.m_DormantJulian.GetStatus() == COleDateTime::valid)
			m_calcOld.iSetJulianShrubDormant(DayOfYear(staSet.m_DormantJulian));
	}

	
	m_calcNew.iInitialize((staSet.m_Use88 ) ? 88 : 78,//staSet.m_Use88 ? 88 : 78, 
			staSet.m_ClimateCls, 0, 0,
			(int) (staSet.m_LatDegrees), staSet.m_HerbAnnual, staSet.m_Deciduous, staSet.m_FM1Eq10, 
			staSet.m_Start1000 - 5.0, staSet.m_Start1000, sKBDI, staSet.m_AvgPrecip);
	m_calcNew.iSetFuelModel(staSet.m_NFDRSFM[0],0);
	if(!staSet.IsFieldNull(&staSet.m_UseDormant) && !staSet.IsFieldNull(&staSet.m_DormantJulian))
	{
		if(staSet.m_UseDormant && staSet.m_DormantJulian.GetStatus() == COleDateTime::valid)
			m_calcNew.iSetJulianShrubDormant(DayOfYear(staSet.m_DormantJulian));
	}
	bool bUseStick = true;
	if(!staSet.IsFieldNull(&staSet.m_UseStick))
		bUseStick = staSet.m_UseStick;
	COleDateTime yesterday, wxDay, lastNelsenTime;//used to skip duplicates

	int  iRainEvent, iStage, iSC, iIC,iSC2, iIC2,
		iFIL, iBI,iBI2,
		iKBDI, init = 1, iDeclareGreenUp = 0, iDeclareFreeze = 0;//,
	
	double f1, f10, f100, f1000, fHerb, fWood, fGren, fX1000, fROS, fERC,fERC2, fFL;
    int tmp, tmpMax, tmpMin, rh1, rhMax, rhMin, pptDur, sow, greenHerb, greenShrub,
			season, windSpd, windDir, slopeClass, wetFlag, snowFlag;
    double pptAmt, omc10; 
	   slopeClass = staSet.m_SlopeCls;
	   slopeClass = max(slopeClass, 1);
	   slopeClass = min(slopeClass, 5);
	int qStartMonth = MonthChtoInt(fpSet->m_StartMonth),
		qEndMonth = MonthChtoInt(fpSet->m_EndMonth),
		qStartDay = fpSet->m_StartDay,
		qEndDay = fpSet->m_EndDay;
	m_wxSet->MoveFirst();
	while (! m_wxSet->IsEOF() && !pCUPDUPData->ShouldTerminate())
	{
	   /* skip records with missing crucial fields */
	   if (m_wxSet->IsFieldNull(&m_wxSet->m_Temp) ||
		   m_wxSet->IsFieldNull(&m_wxSet->m_RH))
	   {
		  
			//cd.Increment();
			m_wxSet->MoveNext();
			continue;
		}
       year = m_wxSet->m_ObsDate.GetYear();
	   month = m_wxSet->m_ObsDate.GetMonth();
	   day = m_wxSet->m_ObsDate.GetDay();
	   hour = m_wxSet->m_ObsDate.GetHour();
	   minute = m_wxSet->m_ObsDate.GetMinute();
	   second = m_wxSet->m_ObsDate.GetSecond();



	      // nfdrs stuff
	   wxDay.SetDateTime(m_wxSet->m_ObsDate.GetYear(), m_wxSet->m_ObsDate.GetMonth(), m_wxSet->m_ObsDate.GetDay(), 
				m_wxSet->m_ObsDate.GetHour(), m_wxSet->m_ObsDate.GetMinute(), m_wxSet->m_ObsDate.GetSecond());
		if(!init && yesterday >= wxDay)
		{
			m_wxSet->MoveNext();
			continue;
		}
			// new custom init check    12/2006
			// if this is first record for new year, look for inits
			if (yesterday.GetYear() != wxDay.GetYear())
			{
				
                CCustomInitSet ciSet(fpSet->m_pDatabase);
				ciSet.m_strFilter.Format("[SIG_Station]='%s' AND [Year]=%d and [Enabled] <> 0", m_wxSet->m_StationID,wxDay.GetYear());
				ciSet.Open();
				bool usedNFDRScust = false, usedCANcust = false;
				if (! ciSet.IsEOF())
				{
                    // enabled record exists for this station/year. get values and re-initialize
                   //int newGreen;//, lookFreeze;

				   if (!ciSet.IsFieldNull(&ciSet.m_GreenJulian) &&
					   !ciSet.IsFieldNull(&ciSet.m_StartKBDI) &&
					   !ciSet.IsFieldNull(&ciSet.m_Start1000))
				   {
		                int newGreen = DayOfYear(ciSet.m_GreenJulian);
		           
						m_calcOld.iSetAnnualInit(newGreen, ciSet.m_Start1000, ciSet.m_StartKBDI);
						m_calcNew.iSetAnnualInit(newGreen, ciSet.m_Start1000, ciSet.m_StartKBDI);
						usedNFDRScust = true;
						//customInits = true;

				   } 
				  /* if(useCanadian)
				   {
						if (!ciSet.IsFieldNull(&ciSet.m_FFMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DC))
						{
		                   canadian.ReInitialize(ciSet.m_FFMC,ciSet.m_DMC,ciSet.m_DC);
						   usedCANcust = true;
						  // customInits = true;
						}
				   }*/
				}
				if(!usedNFDRScust)
				{
					//set to default station values
					theApp.m_NFDRS.iSetAnnualInit(DayOfYear(staSet.m_GreenJulian), staSet.m_Start1000, sKBDI);
				}
				//if(!usedCANcust)
				//	canadian.ReInitialize(85, 6, 15);
                ciSet.Close();
			}
		yesterday = wxDay;

       tmp = m_wxSet->m_Temp;
	   tmpMax = m_wxSet->IsFieldNull(&m_wxSet->m_TmpMax) ? -10000 : m_wxSet->m_TmpMax;
	   tmpMin = m_wxSet->IsFieldNull(&m_wxSet->m_TmpMin) ? -10000 : m_wxSet->m_TmpMin;
	   rh1 = max(m_wxSet->m_RH, 1);
	   rhMax = m_wxSet->IsFieldNull(&m_wxSet->m_RHMax) ? -10000 : m_wxSet->m_RHMax;
	   rhMin = m_wxSet->IsFieldNull(&m_wxSet->m_RHMin) ? -10000 : max(m_wxSet->m_RHMin, 1);
	   pptDur = m_wxSet->IsFieldNull(&m_wxSet->m_PPTDUR) ? 0 : m_wxSet->m_PPTDUR;
	   sow = m_wxSet->IsFieldNull(&m_wxSet->m_SOW) ? 0 : m_wxSet->m_SOW;
	   greenHerb = m_wxSet->ResolveSOW();//IsFieldNull(&m_wxSet->m_GreenHerb) ? 0 : m_wxSet->m_GreenHerb;
	   greenShrub = m_wxSet->IsFieldNull(&m_wxSet->m_GreenShrub) ? 0 : m_wxSet->m_GreenShrub;
	   season = m_wxSet->IsFieldNull(&m_wxSet->m_Season) ? 0 : m_wxSet->m_Season;
	   windSpd = m_wxSet->IsFieldNull(&m_wxSet->m_WS) ? 0 : m_wxSet->m_WS;
	   windDir = m_wxSet->IsFieldNull(&m_wxSet->m_WDir) ? 0 : m_wxSet->m_WDir;
	   pptAmt = m_wxSet->IsFieldNull(&m_wxSet->m_PPTAMT) ? 0.0 : m_wxSet->m_PPTAMT;
	  // omc10 = m_wxSet->IsFieldNull(&m_wxSet->m_OMC10) ? -1000.0 : m_wxSet->m_OMC10;
		omc10 = (bUseStick && !m_wxSet->IsFieldNull(&m_wxSet->m_OMC10)) ? m_wxSet->m_OMC10 : -1000.0;
 		wetFlag = m_wxSet->ResolveWetFlag();//IsFieldNull(&m_wxSet->m_WetFlag) ? 0 : m_wxSet->m_WetFlag;
 		snowFlag = m_wxSet->IsFieldNull(&m_wxSet->m_SnowFlag) ? 0 : m_wxSet->m_SnowFlag;
     
	   if(m_wxSet->m_ObsType.CompareNoCase("O") == 0)
	   {//nfdrs record, sync it up
		   m_calcOld.iCalcMoistWetFlagSnowFlag(init, tmp, rh1, 
			   tmpMax, 
			   rhMax, 
			   tmpMin, 
			   rhMin, 
			   pptDur, 
			   pptAmt, 
			   sow, 
			   omc10, 
			   DayOfYear(m_wxSet->m_ObsDate), 
			   m_wxSet->m_ObsDate.GetYear(), 
			   iDeclareGreenUp, 
			   iDeclareFreeze, 
			   greenHerb, 
			   greenShrub,
			   season,
			   wetFlag, snowFlag,
			   &f1, &f10, &f100, &f1000, &fHerb, &fWood, &iStage, &fGren, &fX1000, &iRainEvent, &iKBDI);

		   m_calcOld.iCalcIndexes(windSpd, slopeClass, &fROS, &iSC, &fERC, &fFL, &iFIL, &iBI);
		   m_calcOld.iCalcIC(tmp, sow, f1, iSC, &iIC);
		   init = 0;
	   }
		if (m_wxSet->IsFieldNull(&m_wxSet->m_SolarRadiation) ||
			m_wxSet->IsFieldNull(&m_wxSet->m_HourlyPrecip)
			)//|| init)
		{
			m_wxSet->MoveNext();
			continue;
		}
	   tempC = ((float) (m_wxSet->m_Temp - 32.0)) * 5.0 / 9.0;
       rh = ((float) (max(m_wxSet->m_RH, 1))) / 100.0;


       solar = (double) (m_wxSet->m_SolarRadiation);
	
	   precipCounter += (m_wxSet->m_HourlyPrecip) * 2.54;
	   cumCm = precipCounter;
	  

       if ( ! record )
        {
			

            stick1h->initializeEnvironment( year, month, day, hour, minute,
                second, tempC, rh, solar, cumCm, tempC, rh, m_start1 );
			stick10h->initializeEnvironment( year, month, day, hour, minute,
                second, tempC, rh, solar, cumCm, tempC, rh, m_start10 );
			if(!isHalfNelson)
			{
				stick100h->initializeEnvironment( year, month, day, hour, minute,
					second, tempC, rh, solar, cumCm, tempC, rh, m_start100 );
				stick1000h->initializeEnvironment( year, month, day, hour, minute,
					second, tempC, rh, solar, cumCm, tempC, rh, m_start1000 );
			}
			//fprintf(stream,"DeadFuelMoistures initialized: %4d/%02d/%02d %02d:%02d \n",
				  // year,month,day,hour,minute); 
        }
		else
		{
			COleDateTimeSpan gap = wxDay - lastNelsenTime;
			if(gap.GetTotalHours() < NELSEN_1HR_GAP)
				stick1h->update(  year, month, day, hour, minute, second, tempC,
					rh, solar, cumCm );
			else
			{
				stick1h->initializeEnvironment( year, month, day, hour, minute,
					second, tempC, rh, solar, cumCm, tempC, rh, stick1h->meanWtdMoisture() );
			}
			if(gap.GetTotalHours() < NELSEN_10HR_GAP)
				stick10h->update(  year, month, day, hour, minute, second, tempC,
					rh, solar, cumCm );
			else
			{
				stick10h->initializeEnvironment( year, month, day, hour, minute,
					second, tempC, rh, solar, cumCm, tempC, rh, stick10h->meanWtdMoisture() );
			}
			if(!isHalfNelson)
			{
				if(gap.GetTotalHours() < NELSEN_100HR_GAP)
					stick100h->update(  year, month, day, hour, minute, second, tempC,
						rh, solar, cumCm );
				else
				{
					stick100h->initializeEnvironment( year, month, day, hour, minute,
						second, tempC, rh, solar, cumCm, tempC, rh, stick100h->meanWtdMoisture() );
				}
				if(gap.GetTotalHours() < NELSEN_1000HR_GAP)
					stick1000h->update(  year, month, day, hour, minute, second, tempC,
						rh, solar, cumCm );
				else
				{
					stick1000h->initializeEnvironment( year, month, day, hour, minute,
						second, tempC, rh, solar, cumCm, tempC, rh, stick1000h->meanWtdMoisture() );
				}
			}
		}
		lastNelsenTime = wxDay;

 	   if(m_wxSet->m_ObsType.CompareNoCase("O") == 0)
	   {//nfdrs record, sync it up
   

		   if(((qStartMonth < qEndMonth || (qStartMonth == qEndMonth && qStartDay <= qEndDay))//normal ordering
			   && ((qStartMonth < month && qEndMonth > month)
			   || ((qStartMonth == month && qStartDay <= day)
			   || (qEndMonth == month && qEndDay >= day))))
			   ||((qStartMonth > qEndMonth || (qStartMonth == qEndMonth && qStartDay >= qEndDay))//flipped ordering
			   && ((qStartMonth < month || qEndMonth > month)
			   || (qStartMonth == month && qStartDay <= day)
			   || (qEndMonth == month && qEndDay >= day))))
			   //included in date time of year query...
		   {
			   m1 = stick1h->meanWtdMoisture();//meanMoisture();
			   m10 = stick10h->meanWtdMoisture();//meanMoisture();
			   if(isHalfNelson)
			   {
					m_calcNew.iSetMoistures (m1*100, m10*100, f100, f1000, fWood, fHerb,iRainEvent, iKBDI, greenHerb,greenShrub, season, sow);
					m100 = f100 / 100.0;
					m1000 = f1000 / 100.0;
			   }
			   else
			   {
				   m100 = stick100h->meanWtdMoisture();//meanMoisture();
				   m1000 = stick1000h->meanWtdMoisture();//meanMoisture();
					m_calcNew.iSetMoistures (m1*100, m10*100, m100*100, m1000*100, fWood, fHerb,iRainEvent, iKBDI, greenHerb,greenShrub, season, sow);
			   }

			   m_calcNew.iCalcIndexes(windSpd, slopeClass, &fROS, &iSC2, &fERC2, &fFL, &iFIL, &iBI2);
			   // calcNew.iCalcIC(tmp, sow, f1, iSC2, &iIC2);
			   m_calcNew.iCalcIC(tmp, sow, m1*100, iSC2, &iIC2);
			   fprintf(stream,"%4d/%02d/%02d %02d:%02d  %4d %4d %6.2f %6.2f  %3d %3d %4d %4d   %3d %3d %4d %4d     %4d -%4d   %4d -%4d     %4d -%4d  %4d -%4d\n",
				   year,month,day,hour,minute, 
				   m_wxSet->m_Temp, max(m_wxSet->m_RH, 1), m_wxSet->m_PPTAMT, cumCm / 2.54,// m_wxSet->m_HourlyPrecip, m_wxSet->m_SolarRadiation,
				   (int)(f1 ),
				   (int) (f10 ),
				   (int) (f100),
				   (int) (f1000),
				   (int) (m1 * 100.0), (int) (m10 * 100.0), (int) (m100 * 100.0), (int) (m1000 * 100.0),
				   iSC, iSC2, (int) Round(fERC), (int)Round(fERC2), iBI, iBI2, iIC, iIC2);
		   }
	   }
	   if(!pCUPDUPData->ShouldTerminate())
	   {
		   record ++;
		  // pCUPDUPData->Increment();
		   char tmp[256];
		   sprintf(tmp, "Processing record %d", record);
		   pCUPDUPData->SetProgress(tmp);
		   pCUPDUPData->SetProgress( (double)record / (double)recCount * 100.0);
		   m_wxSet->MoveNext();
	   }
	}

	if (! record)
		fprintf(stream,"\nNo usable hourly data found.\n\n");
	fclose(stream);

	if (stick1h)
		delete stick1h;
	if (stick10h)
		delete stick10h;
	if (stick100h)
		delete stick100h;
	if (stick1000h)
		delete stick1000h;
}


// this report produces comparisons (at 1300 hours) with daily NFDRS moistures
void CNelsonDFM::RunReport2(bool _isHalfNelson)
{
	isHalfNelson = _isHalfNelson;
	CWaitCursor wait;
	if (! fpSet)
		return; 
	CSIGStationSet staSet(fpSet->m_pDatabase);
	staSet.m_strFilter.Format("[StationID]='%s'",fpSet->m_SIG_Station);
    staSet.Open();
	if (staSet.IsEOF()){
		AfxMessageBox("ERROR: Weather station data not found.");
        return;
	}
	int sKBDI = 0;
	if(!staSet.IsFieldNull(&staSet.m_StartKBDI))
		sKBDI = staSet.m_StartKBDI;
	int  iRainEvent, iStage,
		iKBDI, init = 1, iDeclareGreenUp = 0, iDeclareFreeze = 0;//,
	
	double f1 = 20.0, f10 = 20.0, f100 = 20.0, f1000 = 20.0, fHerb = 20.0, fWood = 20.0, fGren = 20.0, 
		fX1000 = 20.0, fROS = 20.0;

	
	
	COleDateTime yesterday, wxDay;//used to skip duplicates

    int tmp, tmpMax, tmpMin, rh1, rhMax, rhMin, pptDur, sow, greenHerb, greenShrub,
			season, windSpd, windDir, slopeClass, wetFlag, snowFlag;
    double pptAmt, omc10; 


   // CNfdrs4ctrl1 calcOld;
   // CRect nRect(0, 0, 1, 1);
	//calcOld.Create(NULL, WS_CHILD,//WS_DISABLED,
	//	nRect, parent, IDC_NFDRS2);
	//calcNew.Create(NULL, WS_CHILD,//WS_DISABLED,
	//	nRect, parent, IDC_NFDRS3);


	/* open wxObs */
	CString query, tStr;
	fpSet->BuildBaseQuery(query, "ObsDate", true);

	tStr = fpSet->m_SIG_Station;
	tStr.TrimRight();
	CStationInSIGSet sSet(fpSet->m_pDatabase);
	
	if(tStr.GetLength() > 6)
	{
	   // can't do SIGS
		AfxMessageBox("The Nelson Dead Fuel Moisture report cannot be run on a SIG.");

		return;
	}
	else
	{
		CString sta;
		sta.Format(" and [StationID] = '%s'", tStr);
		query += sta;
	}

	m_wxSet = new CWxSet(fpSet->m_pDatabase);
	m_wxSet->m_strFilter = query;
	m_wxSet->m_strSort = "[ObsDate]";
	m_wxSet->Open();
	recCount = 0;
 	//find first solar radiation day
	COleDateTime firstSolRadDay;
	bool hasSolRad = false;
	while(!m_wxSet->IsEOF())
	{
		if(!hasSolRad && !m_wxSet->IsFieldNull(&m_wxSet->m_SolarRadiation) && !m_wxSet->IsFieldNull(&m_wxSet->m_HourlyPrecip))
		{
			firstSolRadDay = m_wxSet->m_ObsDate;
			hasSolRad = true;
			//break;
		}
		recCount++;
		m_wxSet->MoveNext();
	}
	if(!hasSolRad)
	{
		//m_wxSet->Close();
		//delete m_wxSet;
		AfxMessageBox("Error: No valid weather records with Solar Radiation and Hourly Precip fields populated.");
		return;
	}
	m_wxSet->MoveFirst();
  
	//need to get starting moistures from NFDRS for that day, then present the moisture dialog
	m_calcOld.iInitialize((staSet.m_Use88 ) ? 88 : 78,//staSet.m_Use88 ? 88 : 78, 
		staSet.m_ClimateCls, 0, 0,
		(int) (staSet.m_LatDegrees), staSet.m_HerbAnnual, staSet.m_Deciduous, staSet.m_FM1Eq10, 
		staSet.m_Start1000 - 5.0, staSet.m_Start1000, sKBDI, staSet.m_AvgPrecip);
	m_calcOld.iSetFuelModel(staSet.m_NFDRSFM[0],0);
	if(!staSet.IsFieldNull(&staSet.m_UseDormant) && !staSet.IsFieldNull(&staSet.m_DormantJulian))
	{
		if(staSet.m_UseDormant && staSet.m_DormantJulian.GetStatus() == COleDateTime::valid)
			m_calcOld.iSetJulianShrubDormant(DayOfYear(staSet.m_DormantJulian));
	}
	bool bUseStick = true;
	if(!staSet.IsFieldNull(&staSet.m_UseStick))
		bUseStick = staSet.m_UseStick;
	while(m_wxSet->m_ObsDate < firstSolRadDay || init)
	{
		if(m_wxSet->IsFieldNull(&m_wxSet->m_Temp) 
			|| m_wxSet->IsFieldNull(&m_wxSet->m_RH)
			|| m_wxSet->IsFieldNull(&m_wxSet->m_ObsDate))
		{
			m_wxSet->MoveNext();
			continue;
		}
		if(m_wxSet->m_ObsType.CompareNoCase("O") == 0)
		{
			wxDay.SetDateTime(m_wxSet->m_ObsDate.GetYear(), m_wxSet->m_ObsDate.GetMonth(), m_wxSet->m_ObsDate.GetDay(), 
				m_wxSet->m_ObsDate.GetHour(), m_wxSet->m_ObsDate.GetMinute(), m_wxSet->m_ObsDate.GetSecond());
			if(!init && yesterday >= wxDay)
			{
				m_wxSet->MoveNext();
				continue;
			}
			// new custom init check    12/2006
			// if this is first record for new year, look for inits
			if (yesterday.GetYear() != wxDay.GetYear())
			{
				
                CCustomInitSet ciSet(fpSet->m_pDatabase);
				ciSet.m_strFilter.Format("[SIG_Station]='%s' AND [Year]=%d and [Enabled] <> 0", m_wxSet->m_StationID,wxDay.GetYear());
				ciSet.Open();
				bool usedNFDRScust = false, usedCANcust = false;
				if (! ciSet.IsEOF())
				{
                    // enabled record exists for this station/year. get values and re-initialize
                   //int newGreen;//, lookFreeze;

				   if (!ciSet.IsFieldNull(&ciSet.m_GreenJulian) &&
					   !ciSet.IsFieldNull(&ciSet.m_StartKBDI) &&
					   !ciSet.IsFieldNull(&ciSet.m_Start1000))
				   {
		                int newGreen = DayOfYear(ciSet.m_GreenJulian);
		           
						theApp.m_NFDRS.iSetAnnualInit(newGreen, ciSet.m_Start1000, ciSet.m_StartKBDI);
						usedNFDRScust = true;
						//customInits = true;

				   } 
				  /* if(useCanadian)
				   {
						if (!ciSet.IsFieldNull(&ciSet.m_FFMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DC))
						{
		                   canadian.ReInitialize(ciSet.m_FFMC,ciSet.m_DMC,ciSet.m_DC);
						   usedCANcust = true;
						  // customInits = true;
						}
				   }*/
				}
				if(!usedNFDRScust)
				{
					//set to default station values
					theApp.m_NFDRS.iSetAnnualInit(DayOfYear(staSet.m_GreenJulian), staSet.m_Start1000, sKBDI);
				}
				//if(!usedCANcust)
				//	canadian.ReInitialize(85, 6, 15);
                ciSet.Close();
			}
			yesterday = wxDay;//wxSet->m_ObsDate;
			tmp = m_wxSet->m_Temp;
			tmpMax = m_wxSet->IsFieldNull(&m_wxSet->m_TmpMax) ? -10000 : m_wxSet->m_TmpMax;
			tmpMin = m_wxSet->IsFieldNull(&m_wxSet->m_TmpMin) ? -10000 : m_wxSet->m_TmpMin;
			rh1 = max(m_wxSet->m_RH, 1);
			rhMax = m_wxSet->IsFieldNull(&m_wxSet->m_RHMax) ? -10000 : m_wxSet->m_RHMax;
			rhMin = m_wxSet->IsFieldNull(&m_wxSet->m_RHMin) ? -10000 : max(m_wxSet->m_RHMin, 1);
			pptDur = m_wxSet->IsFieldNull(&m_wxSet->m_PPTDUR) ? 0 : m_wxSet->m_PPTDUR;
			sow = m_wxSet->ResolveSOW();//IsFieldNull(&m_wxSet->m_SOW) ? 0 : m_wxSet->m_SOW;
			greenHerb = m_wxSet->IsFieldNull(&m_wxSet->m_GreenHerb) ? 0 : m_wxSet->m_GreenHerb;
			greenShrub = m_wxSet->IsFieldNull(&m_wxSet->m_GreenShrub) ? 0 : m_wxSet->m_GreenShrub;
			season = m_wxSet->IsFieldNull(&m_wxSet->m_Season) ? 0 : m_wxSet->m_Season;
			windSpd = m_wxSet->IsFieldNull(&m_wxSet->m_WS) ? 0 : m_wxSet->m_WS;
			windDir = m_wxSet->IsFieldNull(&m_wxSet->m_WDir) ? 0 : m_wxSet->m_WDir;
			slopeClass = staSet.m_SlopeCls;
			slopeClass = max(slopeClass, 1);
			slopeClass = min(slopeClass, 5);
			pptAmt = m_wxSet->IsFieldNull(&m_wxSet->m_PPTAMT) ? 0.0 : m_wxSet->m_PPTAMT;
			//omc10 = m_wxSet->IsFieldNull(&m_wxSet->m_OMC10) ? -1000.0 : m_wxSet->m_OMC10;
			omc10 = (bUseStick && !m_wxSet->IsFieldNull(&m_wxSet->m_OMC10)) ? m_wxSet->m_OMC10 : -1000.0;
			wetFlag = m_wxSet->ResolveWetFlag();//IsFieldNull(&m_wxSet->m_WetFlag) ? 0 : m_wxSet->m_WetFlag;
			snowFlag = m_wxSet->IsFieldNull(&m_wxSet->m_SnowFlag) ? 0 : m_wxSet->m_SnowFlag;
			m_calcOld.iCalcMoistWetFlagSnowFlag(init, tmp, rh1, 
				tmpMax, 
				rhMax, 
				tmpMin, 
				rhMin, 
				pptDur, 
				pptAmt, 
				sow, 
				omc10, 
				DayOfYear(m_wxSet->m_ObsDate), 
				m_wxSet->m_ObsDate.GetYear(), 
				iDeclareGreenUp, 
				iDeclareFreeze, 
				greenHerb, 
				greenShrub,
				season,
				wetFlag, snowFlag,
				&f1, &f10, &f100, &f1000, &fHerb, &fWood, &iStage, &fGren, &fX1000, &iRainEvent, &iKBDI);
			init = 0;
		}
		m_wxSet->MoveNext();
	}
	/*CNelsonDialog nd;
	nd.start1h = (int) (f1);
	nd.start10h = (int) (f10);
	nd.start100h = (int) (f100);
	nd.start1000h = (int) (f1000);
	int ndres = nd.DoModal();
	if (ndres == IDCANCEL)
	{
		m_wxSet->Close();
		delete m_wxSet;
		return;
	}
	m_start1 = (double) (nd.start1h) / 100.0;
	m_start10 = (double) (nd.start10h) / 100.0;
	m_start100 = (double) (nd.start100h) / 100.0;
	m_start1000 = (double) (nd.start1000h) / 100.0;*/
	m_start1 = f1 / 100.0;
	m_start10 = f10 / 100.0;
	m_start100 = f100 / 100.0;
	m_start1000 = f1000 / 100.0;
	CUPDialog cupDlg(AfxGetMainWnd()->m_hWnd, NFDRSNelsonRunProc, this);
	INT_PTR ret = cupDlg.DoModal();
	//m_wxSet->Close();
	//delete m_wxSet;
	if(ret != IDOK)
	{
		free(m_fName);
		m_fName = NULL;
	}
//	pCUPDUPData->SetExecutionComplete(1);
	//now display the file in a child window
	if(m_fName)
	{
		POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

		while(curTemplatePos != NULL)
		{
			CDocTemplate* curTemplate = 
				theApp.GetNextDocTemplate(curTemplatePos);
			CString str;
			curTemplate->GetDocString(str, CDocTemplate::docName);
			if(str == _T("TextFile"))
			{
				CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(m_fName);
				pDoc->isTemp = true;
				free(m_fName);
				m_fName = NULL;
				CString tTitle;
				tTitle.Format("%s - Nelson Dead Fuel Moistures", fpSet->m_SIG_Station);
				pDoc->SetTitle(tTitle);		
			} 
		}
	}
}

int CNelsonDFM::CalcValues(CWnd *_caller, int varID, bool _isHalfNelson)
{//populate vals for varID for later use
	//only handle Nelson related varIDs (Dead fuel Moistures and NFDRS values)
	isHalfNelson = _isHalfNelson;
	m_varID = varID;
	int m1 = MonthChtoInt(fpSet->m_StartMonth);
	int m2 = MonthChtoInt(fpSet->m_EndMonth);
	int d1 = fpSet->m_StartDay;
	int d2 = fpSet->m_EndDay;
	if(m1 == 2 && d1 > 28)
		d1 = 28;
	if(m2 == 2 && d2 > 28)
		d2 = 28;
	int y1 = fpSet->m_StartYear;
	int y2 = fpSet->m_EndYear;
	nYears = y2 - y1 + 1;
	startYear = y1;
	start.SetDate(2009, m1, d1);
	end.SetDate(2009, m2, d2);
	int doy1 = DayOfYear(start);//start.GetDayOfYear();
	int doy2 = DayOfYear(end);//.GetDayOfYear();
	valsPerYear = doy2 - doy1 + 1;
	if(valsPerYear <= 0)
		valsPerYear += 365;
	vals = new float*[nYears];
	for(int y = 0; y < nYears; y++)
	{
		vals[y] = new float[valsPerYear];
		for(int v = 0; v < valsPerYear; v++)
			vals[y][v] = -1;
	}
	//storage created
	//getNFDRS in synch, get start moistures, calc with Nelson
	CWaitCursor wait;
	if (! fpSet)
		return -1;
	CSIGStationSet staSet(fpSet->m_pDatabase);
	staSet.m_strFilter.Format("[StationID]='%s'",fpSet->m_SIG_Station);
	staSet.Open();
	if (staSet.IsEOF()){
		AfxMessageBox("ERROR: Weather station data not found.");
		staSet.Close();
		return -1;
	}
	int sKBDI = 0;
	if(!staSet.IsFieldNull(&staSet.m_StartKBDI))
		sKBDI = staSet.m_StartKBDI;

	COleDateTime yesterday, wxDay;//used to skip duplicates

	int tmp, tmpMax, tmpMin, rh1, rhMax, rhMin, pptDur, sow, greenHerb, greenShrub,
		season, windSpd, windDir, slopeClass, wetFlag, snowFlag;
	double pptAmt, omc10; 
	int  iRainEvent, iStage, 
		iKBDI, init = 1, iDeclareGreenUp = 0, iDeclareFreeze = 0;//,
	double f1 = 20.0, f10 = 20.0, f100 = 20.0, f1000 = 20.0, fHerb = 20.0, fWood = 20.0, fGren = 20.0, fX1000 = 20.0;

	// need one calculator to compute starting moistures
	//CNfdrs4ctrl1 calcOld;
	//CRect nRect(0, 0, 1, 1);
	//calcOld.Create(NULL, WS_CHILD,//WS_DISABLED,
	//	nRect, parent, IDC_NFDRS2); 
	/* open wxObs */
	CString query, tStr;
	fpSet->BuildBaseQuery(query, "ObsDate", true);

	tStr = fpSet->m_SIG_Station;
	tStr.TrimRight();
	CStationInSIGSet sSet(fpSet->m_pDatabase);

	if(tStr.GetLength() > 6)
	{
		// can't do SIGS
		AfxMessageBox("The Nelson Dead Fuel Moisture report cannot be run on a SIG.");

		return -1;
	}
	else
	{
		CString sta;
		sta.Format(" and [StationID] = '%s'", tStr);
		query += sta;
	}

	m_wxSet = new CWxSet(fpSet->m_pDatabase);
	m_wxSet->m_strFilter = query;
	m_wxSet->m_strSort = "[ObsDate]";
	m_wxSet->Open();
	recCount = 0;
	//find first solar radiation day
	COleDateTime firstSolRadDay;
	bool hasSolRad = false;
	while(!m_wxSet->IsEOF())
	{
		if(!hasSolRad && !m_wxSet->IsFieldNull(&m_wxSet->m_SolarRadiation) && !m_wxSet->IsFieldNull(&m_wxSet->m_HourlyPrecip))
		//if(!m_wxSet->IsFieldNull(&m_wxSet->m_SolarRadiation))
		{
			firstSolRadDay = m_wxSet->m_ObsDate;
			hasSolRad = true;
			//break;
		}
		recCount++;
		m_wxSet->MoveNext();
	}
	if(!hasSolRad)
	{
		//m_wxSet->Close();
		//delete m_wxSet;
		AfxMessageBox("Error: No valid weather records with Solar Radiation field populated.");
		return -1;
	}
	m_wxSet->MoveFirst();
	//need to get starting moistures from NFDRS for that day, then present the moisture dialog
	m_calcOld.iInitialize((staSet.m_Use88 ) ? 88 : 78,//staSet.m_Use88 ? 88 : 78, 
		staSet.m_ClimateCls, 0, 0,
		(int) (staSet.m_LatDegrees), staSet.m_HerbAnnual, staSet.m_Deciduous, staSet.m_FM1Eq10, 
		staSet.m_Start1000 - 5.0, staSet.m_Start1000, sKBDI, staSet.m_AvgPrecip);
	m_calcOld.iSetFuelModel(staSet.m_NFDRSFM[0],0);
	if(!staSet.IsFieldNull(&staSet.m_UseDormant) && !staSet.IsFieldNull(&staSet.m_DormantJulian))
	{
		if(staSet.m_UseDormant && staSet.m_DormantJulian.GetStatus() == COleDateTime::valid)
			m_calcOld.iSetJulianShrubDormant(DayOfYear(staSet.m_DormantJulian));
	}
	bool bUseStick = true;
	if(!staSet.IsFieldNull(&staSet.m_UseStick))
		bUseStick = staSet.m_UseStick;
	while(m_wxSet->m_ObsDate < firstSolRadDay || init)
	{
		if(m_wxSet->IsFieldNull(&m_wxSet->m_Temp) 
			|| m_wxSet->IsFieldNull(&m_wxSet->m_RH)
			|| m_wxSet->IsFieldNull(&m_wxSet->m_ObsDate))
		{
			m_wxSet->MoveNext();
			continue;
		}
		if(m_wxSet->m_ObsType.CompareNoCase("O") == 0)
		{
			wxDay.SetDateTime(m_wxSet->m_ObsDate.GetYear(), m_wxSet->m_ObsDate.GetMonth(), m_wxSet->m_ObsDate.GetDay(), 
				m_wxSet->m_ObsDate.GetHour(), m_wxSet->m_ObsDate.GetMinute(), m_wxSet->m_ObsDate.GetSecond());
			if(!init && yesterday >= wxDay)
			{
				m_wxSet->MoveNext();
				continue;
			}
			// new custom init check    12/2006
			// if this is first record for new year, look for inits
			if (yesterday.GetYear() != wxDay.GetYear())
			{
				
                CCustomInitSet ciSet(fpSet->m_pDatabase);
				ciSet.m_strFilter.Format("[SIG_Station]='%s' AND [Year]=%d and [Enabled] <> 0", m_wxSet->m_StationID,wxDay.GetYear());
				ciSet.Open();
				bool usedNFDRScust = false, usedCANcust = false;
				if (! ciSet.IsEOF())
				{
                    // enabled record exists for this station/year. get values and re-initialize
                  // int newGreen;//, lookFreeze;

				   if (!ciSet.IsFieldNull(&ciSet.m_GreenJulian) &&
					   !ciSet.IsFieldNull(&ciSet.m_StartKBDI) &&
					   !ciSet.IsFieldNull(&ciSet.m_Start1000))
				   {
		                int newGreen = DayOfYear(ciSet.m_GreenJulian);
		           
						theApp.m_NFDRS.iSetAnnualInit(newGreen, ciSet.m_Start1000, ciSet.m_StartKBDI);
						usedNFDRScust = true;
						//customInits = true;

				   } 
				  /* if(useCanadian)
				   {
						if (!ciSet.IsFieldNull(&ciSet.m_FFMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DC))
						{
		                   canadian.ReInitialize(ciSet.m_FFMC,ciSet.m_DMC,ciSet.m_DC);
						   usedCANcust = true;
						  // customInits = true;
						}
				   }*/
				}
				if(!usedNFDRScust)
				{
					//set to default station values
					theApp.m_NFDRS.iSetAnnualInit(DayOfYear(staSet.m_GreenJulian), staSet.m_Start1000, sKBDI);
				}
				//if(!usedCANcust)
				//	canadian.ReInitialize(85, 6, 15);
                ciSet.Close();
			}
			yesterday = wxDay;//wxSet->m_ObsDate;
			tmp = m_wxSet->m_Temp;
			tmpMax = m_wxSet->IsFieldNull(&m_wxSet->m_TmpMax) ? -10000 : m_wxSet->m_TmpMax;
			tmpMin = m_wxSet->IsFieldNull(&m_wxSet->m_TmpMin) ? -10000 : m_wxSet->m_TmpMin;
			rh1 = max(m_wxSet->m_RH, 1);
			rhMax = m_wxSet->IsFieldNull(&m_wxSet->m_RHMax) ? -10000 : m_wxSet->m_RHMax;
			rhMin = m_wxSet->IsFieldNull(&m_wxSet->m_RHMin) ? -10000 : max(m_wxSet->m_RHMin, 1);
			pptDur = m_wxSet->IsFieldNull(&m_wxSet->m_PPTDUR) ? 0 : m_wxSet->m_PPTDUR;
			sow = m_wxSet->ResolveSOW();//IsFieldNull(&m_wxSet->m_SOW) ? 0 : m_wxSet->m_SOW;
			greenHerb = m_wxSet->IsFieldNull(&m_wxSet->m_GreenHerb) ? 0 : m_wxSet->m_GreenHerb;
			greenShrub = m_wxSet->IsFieldNull(&m_wxSet->m_GreenShrub) ? 0 : m_wxSet->m_GreenShrub;
			season = m_wxSet->IsFieldNull(&m_wxSet->m_Season) ? 0 : m_wxSet->m_Season;
			windSpd = m_wxSet->IsFieldNull(&m_wxSet->m_WS) ? 0 : m_wxSet->m_WS;
			windDir = m_wxSet->IsFieldNull(&m_wxSet->m_WDir) ? 0 : m_wxSet->m_WDir;
			slopeClass = staSet.m_SlopeCls;
			slopeClass = max(slopeClass, 1);
			slopeClass = min(slopeClass, 5);
			pptAmt = m_wxSet->IsFieldNull(&m_wxSet->m_PPTAMT) ? 0.0 : m_wxSet->m_PPTAMT;
			//omc10 = m_wxSet->IsFieldNull(&m_wxSet->m_OMC10) ? -1000.0 : m_wxSet->m_OMC10;
			omc10 = (bUseStick && !m_wxSet->IsFieldNull(&m_wxSet->m_OMC10)) ? m_wxSet->m_OMC10 : -1000.0;
			wetFlag = m_wxSet->ResolveWetFlag();//IsFieldNull(&m_wxSet->m_WetFlag) ? 0 : m_wxSet->m_WetFlag;
			snowFlag = m_wxSet->IsFieldNull(&m_wxSet->m_SnowFlag) ? 0 : m_wxSet->m_SnowFlag;
			m_calcOld.iCalcMoistWetFlagSnowFlag(init, tmp, rh1, 
				tmpMax, 
				rhMax, 
				tmpMin, 
				rhMin, 
				pptDur, 
				pptAmt, 
				sow, 
				omc10, 
				DayOfYear(m_wxSet->m_ObsDate), 
				m_wxSet->m_ObsDate.GetYear(), 
				iDeclareGreenUp, 
				iDeclareFreeze, 
				greenHerb, 
				greenShrub,
				season,
				wetFlag, snowFlag,
				&f1, &f10, &f100, &f1000, &fHerb, &fWood, &iStage, &fGren, &fX1000, &iRainEvent, &iKBDI);
			init = 0;
		}
		m_wxSet->MoveNext();
	}
	/*CNelsonDialog nd;
	nd.start1h = (int) (f1);
	nd.start10h = (int) (f10);
	nd.start100h = (int) (f100);
	nd.start1000h = (int) (f1000);
	int ndres = nd.DoModal();
	if (ndres == IDCANCEL)
	{
		m_wxSet->Close();
		delete m_wxSet;
		return -1;
	}
	m_start1 = (double) (nd.start1h) / 100.0;
	m_start10 = (double) (nd.start10h) / 100.0;
	m_start100 = (double) (nd.start100h) / 100.0;
	m_start1000 = (double) (nd.start1000h) / 100.0;*/
	m_start1 = f1 / 100.0;
	m_start10 = f10 / 100.0;
	m_start100 = f100 / 100.0;
	m_start1000 = f1000 / 100.0;
	CUPDialog cupDlg(AfxGetMainWnd()->m_hWnd, CalcNelsonMoisturesRunProc, this);
	INT_PTR ret = cupDlg.DoModal();
	//m_wxSet->Close();
	//delete m_wxSet;
	if(ret != IDOK)
	{
		if(m_fName)
		{
			free(m_fName);
			m_fName = NULL;
		}
		return ret;
	}
//	pCUPDUPData->SetExecutionComplete(1);

	return 0;
}

int CNelsonDFM::FillNelsonVals()
{
	int ret = 0;
	CSIGStationSet staSet(fpSet->m_pDatabase);
	staSet.m_strFilter.Format("[StationID]='%s'",fpSet->m_SIG_Station);
    staSet.Open();
	int sKBDI = 0;
	if(!staSet.IsFieldNull(&staSet.m_StartKBDI))
		sKBDI = staSet.m_StartKBDI;
	/* set up initial precip counter for Nelson model */
	double precipCounter = 0.0;
    /* create 'sticks' */
	DeadFuelMoisture *stick1h = DeadFuelMoisture::createDeadFuelMoisture1();
	DeadFuelMoisture *stick10h = DeadFuelMoisture::createDeadFuelMoisture10();
	DeadFuelMoisture *stick100h = DeadFuelMoisture::createDeadFuelMoisture100();
	DeadFuelMoisture *stick1000h = DeadFuelMoisture::createDeadFuelMoisture1000();

	int year, month, day, hour, minute, second;
    double tempC, rh, solar, cumCm,m1,m10,m100,m1000;
 
//	int record = 0;
	m_calcOld.iInitialize((staSet.m_Use88 ) ? 88 : 78,//staSet.m_Use88 ? 88 : 78, 
			staSet.m_ClimateCls, 0, 0,
			(int) (staSet.m_LatDegrees), staSet.m_HerbAnnual, staSet.m_Deciduous, staSet.m_FM1Eq10, 
			staSet.m_Start1000 - 5.0, staSet.m_Start1000, sKBDI, staSet.m_AvgPrecip);
	m_calcOld.iSetFuelModel(staSet.m_NFDRSFM[0],0);
	if(!staSet.IsFieldNull(&staSet.m_UseDormant) && !staSet.IsFieldNull(&staSet.m_DormantJulian))
	{
		if(staSet.m_UseDormant && staSet.m_DormantJulian.GetStatus() == COleDateTime::valid)
			m_calcOld.iSetJulianShrubDormant(DayOfYear(staSet.m_DormantJulian));
	}

	
	m_calcNew.iInitialize((staSet.m_Use88 ) ? 88 : 78,//staSet.m_Use88 ? 88 : 78, 
			staSet.m_ClimateCls, 0, 0,
			(int) (staSet.m_LatDegrees), staSet.m_HerbAnnual, staSet.m_Deciduous, staSet.m_FM1Eq10, 
			staSet.m_Start1000 - 5.0, staSet.m_Start1000, sKBDI, staSet.m_AvgPrecip);
	m_calcNew.iSetFuelModel(staSet.m_NFDRSFM[0],0);
	if(!staSet.IsFieldNull(&staSet.m_UseDormant) && !staSet.IsFieldNull(&staSet.m_DormantJulian))
	{
		if(staSet.m_UseDormant && staSet.m_DormantJulian.GetStatus() == COleDateTime::valid)
			m_calcNew.iSetJulianShrubDormant(DayOfYear(staSet.m_DormantJulian));
	}
	bool bUseStick = true;
	if(!staSet.IsFieldNull(&staSet.m_UseStick))
		bUseStick = staSet.m_UseStick;
	COleDateTime yesterday, wxDay, lastNelsenTime;//used to skip duplicates

	int  iRainEvent, iStage, iSC, iIC,iSC2, iIC2,
		iFIL, iBI,iBI2,
		iKBDI, init = 1, iDeclareGreenUp = 0, iDeclareFreeze = 0;//,
	
	double f1, f10, f100, f1000, fHerb, fWood, fGren, fX1000, fROS, fERC,fERC2, fFL;
    int tmp, tmpMax, tmpMin, rh1, rhMax, rhMin, pptDur, sow, greenHerb, greenShrub,
			season, windSpd, windDir, slopeClass, wetFlag, snowFlag;
    double pptAmt, omc10; 
	   slopeClass = staSet.m_SlopeCls;
	   slopeClass = max(slopeClass, 1);
	   slopeClass = min(slopeClass, 5);
	int qStartMonth = MonthChtoInt(fpSet->m_StartMonth),
		qEndMonth = MonthChtoInt(fpSet->m_EndMonth),
		qStartDay = fpSet->m_StartDay,
		qEndDay = fpSet->m_EndDay;
	int record = 0;
	m_wxSet->MoveFirst();
	while (! m_wxSet->IsEOF() && !pCUPDUPData->ShouldTerminate())
	{
	   /* skip records with missing crucial fields */
	   if (m_wxSet->IsFieldNull(&m_wxSet->m_Temp) ||
		   m_wxSet->IsFieldNull(&m_wxSet->m_RH))
	   {
		  
			//cd.Increment();
			m_wxSet->MoveNext();
			continue;
		}
       year = m_wxSet->m_ObsDate.GetYear();
	   month = m_wxSet->m_ObsDate.GetMonth();
	   day = m_wxSet->m_ObsDate.GetDay();
	   hour = m_wxSet->m_ObsDate.GetHour();
	   minute = m_wxSet->m_ObsDate.GetMinute();
	   second = m_wxSet->m_ObsDate.GetSecond();



	      // nfdrs stuff
	   wxDay.SetDateTime(m_wxSet->m_ObsDate.GetYear(), m_wxSet->m_ObsDate.GetMonth(), m_wxSet->m_ObsDate.GetDay(), 
				m_wxSet->m_ObsDate.GetHour(), m_wxSet->m_ObsDate.GetMinute(), m_wxSet->m_ObsDate.GetSecond());
		if(!init && yesterday >= wxDay)
		{
			m_wxSet->MoveNext();
			continue;
		}
			// new custom init check    12/2006
			// if this is first record for new year, look for inits
			if (yesterday.GetYear() != wxDay.GetYear())
			{
				
                CCustomInitSet ciSet(fpSet->m_pDatabase);
				ciSet.m_strFilter.Format("[SIG_Station]='%s' AND [Year]=%d and [Enabled] <> 0", m_wxSet->m_StationID,wxDay.GetYear());
				ciSet.Open();
				bool usedNFDRScust = false, usedCANcust = false;
				if (! ciSet.IsEOF())
				{
                    // enabled record exists for this station/year. get values and re-initialize
                   //int newGreen;//, lookFreeze;

				   if (!ciSet.IsFieldNull(&ciSet.m_GreenJulian) &&
					   !ciSet.IsFieldNull(&ciSet.m_StartKBDI) &&
					   !ciSet.IsFieldNull(&ciSet.m_Start1000))
				   {
		                int newGreen = DayOfYear(ciSet.m_GreenJulian);
		           
						m_calcOld.iSetAnnualInit(newGreen, ciSet.m_Start1000, ciSet.m_StartKBDI);
						m_calcNew.iSetAnnualInit(newGreen, ciSet.m_Start1000, ciSet.m_StartKBDI);
						usedNFDRScust = true;
						//customInits = true;

				   } 
				  /* if(useCanadian)
				   {
						if (!ciSet.IsFieldNull(&ciSet.m_FFMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DC))
						{
		                   canadian.ReInitialize(ciSet.m_FFMC,ciSet.m_DMC,ciSet.m_DC);
						   usedCANcust = true;
						  // customInits = true;
						}
				   }*/
				}
				if(!usedNFDRScust)
				{
					//set to default station values
					theApp.m_NFDRS.iSetAnnualInit(DayOfYear(staSet.m_GreenJulian), staSet.m_Start1000, sKBDI);
				}
				//if(!usedCANcust)
				//	canadian.ReInitialize(85, 6, 15);
                ciSet.Close();
			}
		yesterday = wxDay;

       tmp = m_wxSet->m_Temp;
	   tmpMax = m_wxSet->IsFieldNull(&m_wxSet->m_TmpMax) ? -10000 : m_wxSet->m_TmpMax;
	   tmpMin = m_wxSet->IsFieldNull(&m_wxSet->m_TmpMin) ? -10000 : m_wxSet->m_TmpMin;
	   rh1 = max(m_wxSet->m_RH, 1);
	   rhMax = m_wxSet->IsFieldNull(&m_wxSet->m_RHMax) ? -10000 : m_wxSet->m_RHMax;
	   rhMin = m_wxSet->IsFieldNull(&m_wxSet->m_RHMin) ? -10000 : max(m_wxSet->m_RHMin, 1);
	   pptDur = m_wxSet->IsFieldNull(&m_wxSet->m_PPTDUR) ? 0 : m_wxSet->m_PPTDUR;
	   sow = m_wxSet->ResolveSOW();//IsFieldNull(&m_wxSet->m_SOW) ? 0 : m_wxSet->m_SOW;
	   greenHerb = m_wxSet->IsFieldNull(&m_wxSet->m_GreenHerb) ? 0 : m_wxSet->m_GreenHerb;
	   greenShrub = m_wxSet->IsFieldNull(&m_wxSet->m_GreenShrub) ? 0 : m_wxSet->m_GreenShrub;
	   season = m_wxSet->IsFieldNull(&m_wxSet->m_Season) ? 0 : m_wxSet->m_Season;
	   windSpd = m_wxSet->IsFieldNull(&m_wxSet->m_WS) ? 0 : m_wxSet->m_WS;
	   windDir = m_wxSet->IsFieldNull(&m_wxSet->m_WDir) ? 0 : m_wxSet->m_WDir;
	   pptAmt = m_wxSet->IsFieldNull(&m_wxSet->m_PPTAMT) ? 0.0 : m_wxSet->m_PPTAMT;
	  // omc10 = m_wxSet->IsFieldNull(&m_wxSet->m_OMC10) ? -1000.0 : m_wxSet->m_OMC10;
		omc10 = (bUseStick && !m_wxSet->IsFieldNull(&m_wxSet->m_OMC10)) ? m_wxSet->m_OMC10 : -1000.0;
 		wetFlag = m_wxSet->ResolveWetFlag();//IsFieldNull(&m_wxSet->m_WetFlag) ? 0 : m_wxSet->m_WetFlag;
		snowFlag = m_wxSet->IsFieldNull(&m_wxSet->m_SnowFlag) ? 0 : m_wxSet->m_SnowFlag;
      
	   if(m_wxSet->m_ObsType.CompareNoCase("O") == 0)
	   {//nfdrs record, sync it up
		   m_calcOld.iCalcMoistWetFlagSnowFlag(init, tmp, rh1, 
			   tmpMax, 
			   rhMax, 
			   tmpMin, 
			   rhMin, 
			   pptDur, 
			   pptAmt, 
			   sow, 
			   omc10, 
			   DayOfYear(m_wxSet->m_ObsDate), 
			   m_wxSet->m_ObsDate.GetYear(), 
			   iDeclareGreenUp, 
			   iDeclareFreeze, 
			   greenHerb, 
			   greenShrub,
			   season,
			   wetFlag, snowFlag,
			   &f1, &f10, &f100, &f1000, &fHerb, &fWood, &iStage, &fGren, &fX1000, &iRainEvent, &iKBDI);

		   m_calcOld.iCalcIndexes(windSpd, slopeClass, &fROS, &iSC, &fERC, &fFL, &iFIL, &iBI);
		   m_calcOld.iCalcIC(tmp, sow, f1, iSC, &iIC);
		   init = 0;
	   }
		if (m_wxSet->IsFieldNull(&m_wxSet->m_SolarRadiation) ||
			m_wxSet->IsFieldNull(&m_wxSet->m_HourlyPrecip)
			)//|| init)
		{
			m_wxSet->MoveNext();
			continue;
		}
	   tempC = ((float) (m_wxSet->m_Temp - 32.0)) * 5.0 / 9.0;
       rh = ((float) (max(m_wxSet->m_RH, 1))) / 100.0;


       solar = (double) (m_wxSet->m_SolarRadiation);
	
	   precipCounter += (m_wxSet->m_HourlyPrecip) * 2.54;
	   cumCm = precipCounter;
	  

       if ( ! record )
        {
			

            stick1h->initializeEnvironment( year, month, day, hour, minute,
                second, tempC, rh, solar, cumCm, tempC, rh, m_start1 );
			stick10h->initializeEnvironment( year, month, day, hour, minute,
                second, tempC, rh, solar, cumCm, tempC, rh, m_start10 );
			if(!isHalfNelson)
			{
				stick100h->initializeEnvironment( year, month, day, hour, minute,
					second, tempC, rh, solar, cumCm, tempC, rh, m_start100 );
				stick1000h->initializeEnvironment( year, month, day, hour, minute,
					second, tempC, rh, solar, cumCm, tempC, rh, m_start1000 );
			}
			//fprintf(stream,"DeadFuelMoistures initialized: %4d/%02d/%02d %02d:%02d \n",
				  // year,month,day,hour,minute); 
        }
		else
		{
			COleDateTimeSpan gap = wxDay - lastNelsenTime;
			if(gap.GetTotalHours() < NELSEN_1HR_GAP)
				stick1h->update(  year, month, day, hour, minute, second, tempC,
					rh, solar, cumCm );
			else
			{
				stick1h->initializeEnvironment( year, month, day, hour, minute,
					second, tempC, rh, solar, cumCm, tempC, rh, stick1h->meanWtdMoisture() );
			}
			if(gap.GetTotalHours() < NELSEN_10HR_GAP)
				stick10h->update(  year, month, day, hour, minute, second, tempC,
					rh, solar, cumCm );
			else
			{
				stick10h->initializeEnvironment( year, month, day, hour, minute,
					second, tempC, rh, solar, cumCm, tempC, rh, stick10h->meanWtdMoisture() );
			}
			if(!isHalfNelson)
			{
				if(gap.GetTotalHours() < NELSEN_100HR_GAP)
					stick100h->update(  year, month, day, hour, minute, second, tempC,
						rh, solar, cumCm );
				else
				{
					stick100h->initializeEnvironment( year, month, day, hour, minute,
						second, tempC, rh, solar, cumCm, tempC, rh, stick100h->meanWtdMoisture() );
				}
				if(gap.GetTotalHours() < NELSEN_1000HR_GAP)
					stick1000h->update(  year, month, day, hour, minute, second, tempC,
						rh, solar, cumCm );
				else
				{
					stick1000h->initializeEnvironment( year, month, day, hour, minute,
						second, tempC, rh, solar, cumCm, tempC, rh, stick1000h->meanWtdMoisture() );
				}
			}
		}
		lastNelsenTime = wxDay;

 	   if(m_wxSet->m_ObsType.CompareNoCase("O") == 0)
	   {//nfdrs record, sync it up
   

		   if(((qStartMonth < qEndMonth || (qStartMonth == qEndMonth && qStartDay <= qEndDay))//normal ordering
			   && ((qStartMonth < month && qEndMonth > month)
			   || ((qStartMonth == month && qStartDay <= day)
			   || (qEndMonth == month && qEndDay >= day))))
			   ||((qStartMonth > qEndMonth || (qStartMonth == qEndMonth && qStartDay >= qEndDay))//flipped ordering
			   && ((qStartMonth < month || qEndMonth > month)
			   || (qStartMonth == month && qStartDay <= day)
			   || (qEndMonth == month && qEndDay >= day))))
			   //included in date time of year query...
		   {
			   m1 = stick1h->meanWtdMoisture();//meanMoisture();
			   m10 = stick10h->meanWtdMoisture();//meanMoisture();
			   if(isHalfNelson)
			   {

					m_calcNew.iSetMoistures (m1*100, m10*100, f100, f1000, fWood, fHerb,iRainEvent, iKBDI, greenHerb,greenShrub, season, sow);
					m100 = f100 / 100.0;
					m1000 = f1000 / 100.0;
			   }
			   else
			   {
				   m100 = stick100h->meanWtdMoisture();//meanMoisture();
				   m1000 = stick1000h->meanWtdMoisture();//meanMoisture();
					m_calcNew.iSetMoistures (m1*100, m10*100, m100*100, m1000*100, fWood, fHerb,iRainEvent, iKBDI, greenHerb,greenShrub, season, sow);
			   }

			   m_calcNew.iCalcIndexes(windSpd, slopeClass, &fROS, &iSC2, &fERC2, &fFL, &iFIL, &iBI2);
			   // calcNew.iCalcIC(tmp, sow, f1, iSC2, &iIC2);
			   m_calcNew.iCalcIC(tmp, sow, m1*100, iSC2, &iIC2);
			   int y = lastNelsenTime.GetYear() - startYear;
			   //int d = lastNelsenTime.GetDayOfYear() - start.GetDayOfYear();
			   int d = DayOfYear(lastNelsenTime) - DayOfYear(start);//.GetDayOfYear();
			   if(d < 0)
				   d += 365;
			   switch(m_varID)
			   {
			   case 12:
				   vals[y][d] = (double)iSC2;
				   break;
			   case 13:
				   vals[y][d] = fERC2;// (int)Round(fERC2);
				   break;
			   case 14:
				   vals[y][d] = (double)iBI2;
				   break;
			   case 15:
				   vals[y][d] = (double)iKBDI;
				   break;
			   case 16: //Ignition component
				   vals[y][d] = (double)iIC2;
				   break;
			   case 17:
				   vals[y][d] = m1 * 100.0;
				   break;
			   case 18:
				   vals[y][d] = m10 * 100.0;
				   break;
			   case 19:
					if(isHalfNelson)
						vals[y][d] = f100;
					else
						vals[y][d] = m100 * 100.0;
				   break;
			   case 20:
					if(isHalfNelson)
						vals[y][d] = f1000;
					else
					   vals[y][d] = m1000 * 100.0;
				   break;
			   }
			  /* fprintf(stream,"%4d/%02d/%02d %02d:%02d  %4d %4d %6.2f %6.2f  %3d %3d %4d %4d   %3d %3d %4d %4d     %4d -%4d   %4d -%4d     %4d -%4d  %4d -%4d\n",
				   year,month,day,hour,minute, 
				   m_wxSet->m_Temp, m_wxSet->m_RH, m_wxSet->m_PPTAMT, cumCm / 2.54,// m_wxSet->m_HourlyPrecip, m_wxSet->m_SolarRadiation,
				   (int)(f1 ),
				   (int) (f10 ),
				   (int) (f100),
				   (int) (f1000),
				   (int) (m1 * 100.0), (int) (m10 * 100.0), (int) (m100 * 100.0), (int) (m1000 * 100.0),
				   iSC, iSC2, (int) fERC, (int)fERC2, iBI, iBI2, iIC, iIC2);*/
		   }
	   }
	   if(!pCUPDUPData->ShouldTerminate())
	   {
		   record ++;
		   //pCUPDUPData->Increment();
		   char tmp[256];
		   //sprintf(tmp, "%d", record);
		   sprintf(tmp, "Processing record %d", record);
		   pCUPDUPData->SetProgress(tmp);
		   pCUPDUPData->SetProgress( (double)record / (double)recCount * 100.0);
		   m_wxSet->MoveNext();
	   }
	}

	if (! record)
		ret = -1;//fprintf(stream,"\nNo usable hourly data found.\n\n");
	//fclose(stream);

	if (stick1h)
		delete stick1h;
	if (stick10h)
		delete stick10h;
	if (stick100h)
		delete stick100h;
	if (stick1000h)
		delete stick1000h;
	return ret;
}

void CNelsonDFM::GetLogitArraySizesQuick(CFireSumSet *firesSet, CFireOptionsSet *fireOptsSet, int vID,
		long *nV, long *nFD, long *nLFD, long *nMFD, long *_discards, long *madeUps)
{
	//need to populate fires arrays
	fires = new CFireDay *[nYears];
	for(int y = 0; y < nYears; y++)
	{
		fires[y] = new CFireDay [valsPerYear];
	}
	double lA = 0.0, hA = 0.0;
	long lF = 0, hF = 0;
	firesSet->MoveFirst();
	COleDateTime lastF, nextF;
	while(!firesSet->IsEOF())
	{
		int y = firesSet->m_Discovery.GetYear() - startYear;
		int d = DayOfYear(firesSet->m_Discovery) - DayOfYear(start);
		if(d < 0)
			d += 365;
		switch(firesSet->m_StatisticalCause)
		{
		case 1://lightning
			fires[y][d].lightning++;
			fires[y][d].lAcresMax = max(fires[y][d].lAcresMax, firesSet->m_TotalAcres);
			break;
		default:
			fires[y][d].human++;
			fires[y][d].hAcresMax = max(fires[y][d].hAcresMax, firesSet->m_TotalAcres);
		}
		firesSet->MoveNext();
	}
/*	for(int y = 0; y < nYears; y++)
	{
		//for each year...
		for(int i = 0; i < valsPerYear; i++)
		{
			//see if there's fire days - look at each day
			//deal with leap years
			for(int d = DayOfYear(start); d <= DayOfYear(end); d++)
			{
				lA = 0.0; 
				hA = 0.0;
				lF = 0; 
				hF = 0;
				while(!firesSet->IsEOF() 
					&& firesSet->m_Discovery.GetYear() < startYear + y)
					firesSet->MoveNext();
				while(!firesSet->IsEOF() 
					&& firesSet->m_Discovery.GetYear() <= startYear + y
					&& DayOfYear(firesSet->m_Discovery) < d)
					firesSet->MoveNext();
				if(!firesSet->IsEOF() 
					&& firesSet->m_Discovery.GetYear() == startYear + y
					&& DayOfYear(firesSet->m_Discovery) == d)
				{
					while(!firesSet->IsEOF() 
						&& firesSet->m_Discovery.GetYear() == startYear + y
						&& DayOfYear(firesSet->m_Discovery) == d)
					{
						switch(firesSet->m_StatisticalCause)
						{
						case 1://lightning
							lF++;
							lA = max(lA, firesSet->m_TotalAcres);
							break;
						default:
							hF++;
							hA = max(hA, firesSet->m_TotalAcres);
						}
						firesSet->MoveNext();
					}
				}
				fires[y][i].lightning = max(fires[y][i].lightning, lF);
				fires[y][i].human = max(fires[y][i].human, hF);
				fires[y][i].lAcresMax = max(fires[y][i].lAcresMax, lA);
				fires[y][i].hAcresMax = max(fires[y][i].hAcresMax, hA);
			}
		}
	}
*/

	long _nV = 0, _nFD = 0, _nLFD = 0, _nMFD = 0, discards = 0, madeCnt = 0;
	_chdir(theApp.dbDir);
	CClimateSet vclimSet(fireOptsSet->m_pDatabase);
	vclimSet.m_strFilter.Format("[VarID] = %d", fireOptsSet->m_VarID);
	vclimSet.Open();
	CString varName = vclimSet.m_ShortName;
	vclimSet.Close();
	FILE *bads = fopen("discards.dat", "wt");
	fprintf(bads, "Discards for %s\n", (fpSet->m_SIG_Station.GetLength() > 6) ? fpSet->m_SIG_Station 
		: "Station: " + fpSet->m_SIG_Station);
	fprintf(bads, "(Fire records with no corresponding weather record)\nVariable: %s\n", varName);
	fprintf(bads, "Data Years: %d - %d\n", fpSet->m_StartYear, fpSet->m_EndYear);
	fprintf(bads, "Time Frame: %d/%d - %d/%d\n", MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay,
		MonthChtoInt(fpSet->m_EndMonth), fpSet->m_EndDay);	
	int isFD, isLFD, isMFD, f;//, vID;
	//vID = fireOptsSet->m_VarID - 1;
	bool haveWx, madeWx, first;
	double lastWx = 0.0, nextWx, wxVal;
	int lastDay, nextDay, d;
	//first gotta count number of records, accounting for made up values...
	long arraySize = 0;
	for(int y = 0; y < nYears; y++)
	{
		//for each year...
		first = true;
		d = 0;//DayOfYear(periods[varIDs[vID]][0]->start) - 1;
		for(int i = 0; i < valsPerYear; i++)
		{
			haveWx = madeWx = false;
			if(vals[y][i] >= 0)
			{//have weather....
				haveWx = true;
				lastWx = wxVal = vals[y][i];
				lastDay = d;//DayOfYear(periods[varIDs[vID]][i]->start);
				first = false;
			}
			if(!haveWx && !first)
			{
				nextDay = 0;
				for(int p = i + 1; p < valsPerYear && p < i + theApp.maxMissingWx + 1; p++)
				{
					if(vals[y][p] >= 0)
					{
						nextDay = p;//DayOfYear(periods[varIDs[vID]][p]->start);
						nextWx = vals[y][p];//periods[varIDs[vID]][p]->yStats[y].Mean();
						break;
					}
				}
				if((nextDay > 0) && (nextDay - lastDay - 1)<= theApp.maxMissingWx)
				{
					//interpolate
					wxVal = lastWx + ((nextWx - lastWx) / (nextDay - lastDay)) 
							*( i - lastDay);
					madeWx = true;
					madeCnt++;
				}
			}
			if(haveWx || madeWx)
			{
				arraySize++;
				for(;d < i; d++)
				{
					if(d < i - 1)
					{
						if(fires[y][d].human > 0 && (fireOptsSet->m_Human || fireOptsSet->m_All))
						{
							discards += fires[y][d].human;
							COleDateTime bDate = DateFromDOY(y + startYear, d + DayOfYear(start) - 1); 
							fprintf(bads, "%2d/%2d/%d\t%ld human caused fires\n", bDate.GetMonth(), 
								bDate.GetDay(),
								bDate.GetYear(),
								fires[y][d].human);
						}
						if(fires[y][d].lightning > 0 && (fireOptsSet->m_Lightning || fireOptsSet->m_All))
						{
							discards += fires[y][d].lightning;
							COleDateTime bDate = DateFromDOY(y + startYear, d + DayOfYear(start) - 1); 
							fprintf(bads, "%2d/%2d/%d\t%ld lightning caused fires\n", bDate.GetMonth(), 
								bDate.GetDay(),
								bDate.GetYear(),
								fires[y][d].lightning);
						}
					}
				}
				//see if there's a fire day
				isLFD = isMFD = isFD = f = 0;
				if(fireOptsSet->m_All)
				{
					if(fires[y][d].human + fires[y][d].lightning > 0)
					{
						isFD = 1;
						if(fires[y][d].human + fires[y][d].lightning >= fireOptsSet->m_MFDFires)
							isMFD = 1;
						if(fires[y][d].hAcresMax  >= fireOptsSet->m_LFAcres || fires[y][d].lAcresMax >= fireOptsSet->m_LFAcres)
							isLFD = 1;
					}
				}
				else if(fireOptsSet->m_Human)
				{
					if(fires[y][d].human > 0)
					{
						isFD = 1;
						if(fires[y][d].human >= fireOptsSet->m_MFDFires)
							isMFD = 1;
						if(fires[y][d].hAcresMax >= fireOptsSet->m_LFAcres)
							isLFD = 1;
					}
				}
				else if(fireOptsSet->m_Lightning)
				{
					if(fires[y][d].lightning > 0)
					{
						isFD = 1;
						if(fires[y][d].lightning >= fireOptsSet->m_MFDFires)
							isMFD = 1;
						if(fires[y][d].lAcresMax >= fireOptsSet->m_LFAcres)
							isLFD = 1;
					}
				}
				if(isFD)
					_nFD++;
				if(isLFD)
					_nLFD++;
				if(isMFD)
					_nMFD++;
				_nV++;
			}
		}
	}
	*nV = _nV;
	*nFD = _nFD;
	*nLFD = _nLFD;
	*nMFD = _nMFD;
	*_discards = discards;
	*madeUps = madeCnt;
	fclose(bads);
}

void CNelsonDFM::CreateLogitArraysQuick(CFireOptionsSet *fireOptsSet, int vID,
										double *fxVals, double *fyVals, double *lfxVals, double *lfyVals, double *mfxVals, double *mfyVals)
{
	int isFD, isLFD, isMFD, f;//, vID;
	//vID = fireOptsSet->m_VarID - 1;
	bool haveWx, madeWx, first;
	double lastWx = 0.0, nextWx, wxVal;
	int lastDay, nextDay;
	long madeCnt = 0;
	int d;
	//ok, fill arrays then fill them with same algorithm
	long loc = 0, fdLoc = 0;
	for(int y = 0; y < nYears; y++)
	{
		//for each year...
		first = true;
		d = 0;//DayOfYear(periods[varIDs[vID]][0]->start) - 1;
		for(int i = 0; i < valsPerYear; i++)
		{
			haveWx = madeWx = false;
			if(vals[y][i] >= 0)
			{//have weather....
				haveWx = true;
				lastWx = wxVal = vals[y][i];
				lastDay = d;//DayOfYear(periods[varIDs[vID]][i]->start);
				first = false;
			}
			if(!haveWx && !first)
			{
				nextDay = 0;
				for(int p = i + 1; p < valsPerYear && p < i + theApp.maxMissingWx + 1; p++)
				{
					if(vals[y][p] >= 0)
					{
						nextDay = p;//DayOfYear(periods[varIDs[vID]][p]->start);
						nextWx = vals[y][p];//periods[varIDs[vID]][p]->yStats[y].Mean();
						break;
					}
				}
				if((nextDay > 0) && (nextDay - lastDay - 1)<= theApp.maxMissingWx)
				{
					//interpolate
					wxVal = lastWx + ((nextWx - lastWx) / (nextDay - lastDay)) 
						*( i - lastDay);
					madeWx = true;
					madeCnt++;
				}
			}
			//}
			if(haveWx || madeWx)
			{
				//for(;d < i; d++)
				//{
				//}
				//for(;d < DayOfYear(periods[varIDs[vID]][i]->start) - 1 && d < numPeriods; d++)
				/*for(;d < i; d++)
				{
				if(d < i - 1)
				{
				/*if(fires[y][d].human > 0 && (fireOptsSet->m_Human || fireOptsSet->m_All))
				{
				discards += fires[y][d].human;
				COleDateTime bDate = DateFromDOY(y + periods[varIDs[vID]][i]->baseYear, d + DayOfYear(periods[varIDs[vID]][0]->start) - 1); 
				fprintf(bads, "%2d/%2d/%d\t%ld human caused fires\n", bDate.GetMonth(), 
				bDate.GetDay(),
				bDate.GetYear(),
				fires[y][d].human);
				}
				if(fires[y][d].lightning > 0 && (fireOptsSet->m_Lightning || fireOptsSet->m_All))
				{
				discards += fires[y][d].lightning;
				COleDateTime bDate = DateFromDOY(y + periods[varIDs[vID]][i]->baseYear, d + DayOfYear(periods[varIDs[vID]][0]->start) - 1); 
				fprintf(bads, "%2d/%2d/%d\t%ld lightning caused fires\n", bDate.GetMonth(), 
				bDate.GetDay(),
				bDate.GetYear(),
				fires[y][d].lightning);
				}
				}
				}*/
				//d++;
				//see if there's a fire day
				isLFD = isMFD = isFD = f = 0;
				if(fireOptsSet->m_All)
				{
					if(fires[y][i].human + fires[y][i].lightning > 0)
					{
						isFD = 1;
						if(fires[y][i].human + fires[y][i].lightning >= fireOptsSet->m_MFDFires)
							isMFD = 1;
						if(fires[y][i].hAcresMax  >= fireOptsSet->m_LFAcres || fires[y][i].lAcresMax >= fireOptsSet->m_LFAcres)
							isLFD = 1;
					}
				}
				else if(fireOptsSet->m_Human)
				{
					if(fires[y][i].human > 0)
					{
						isFD = 1;
						if(fires[y][i].human >= fireOptsSet->m_MFDFires)
							isMFD = 1;
						if(fires[y][i].hAcresMax >= fireOptsSet->m_LFAcres)
							isLFD = 1;
					}
				}
				else if(fireOptsSet->m_Lightning)
				{
					if(fires[y][i].lightning > 0)
					{
						isFD = 1;
						if(fires[y][i].lightning >= fireOptsSet->m_MFDFires)
							isMFD = 1;
						if(fires[y][i].lAcresMax >= fireOptsSet->m_LFAcres)
							isLFD = 1;
					}
				}
				//if(isFD)
				//	_nFD++;
				//if(isLFD)
				//	_nLFD++;
				//if(isMFD)
				//	_nMFD++;
				//if(!conditional)
				fxVals[loc] = (double)isFD;
				fyVals[loc] = wxVal;
				//fprintf(fd, "%d %.2f\n",isFD, wxVal);
				if(!fireOptsSet->m_Conditional || isFD)
				{
					lfxVals[fdLoc] = (double)isLFD;
					lfyVals[fdLoc] = wxVal;
					mfxVals[fdLoc] = (double)isMFD;
					mfyVals[fdLoc] = wxVal;
					fdLoc++;
					//fprintf(lfd, "%d %.2f\n",isLFD, wxVal);
					//fprintf(mfd, "%d %.2f\n",isMFD, wxVal);
					//	_nV++;
				}
				//_nV++;
				loc++;
			}
		}
	}

	//fclose(bads);
}

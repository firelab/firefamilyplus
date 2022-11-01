//<div class="moz-text-flowed" style="font-family: -moz-fixed"><div class="moz-text-flowed" style="font-family: -moz-fixed">// ClimAnalysis.cpp: implementation of the CClimAnalysis class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <afxrich.h>
#include "richdoc.h"
#include "fireplus.h"
#include "SIGStationSet.h"
#include "ClimateSet.h"
#include "wxSet.h"
#include "fireplusSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "splitterframe.h"
#include "CFiresSet.h"
#include "ClimAnalysis.h"
#include "canada.h"
#include "stationInSIGSet.h"
#include "graphset.h"
#include <direct.h>
#include <float.h>
#include <math.h>
#include "SeasonBinSet.h"
#include "FireAssocSet.h"
#include "FireAgencySet.h"
#include "FireRegionSet.h"
#include "FireUnitSet.h"
#include "FireSubunitSet.h"
#include "auxYear.h"
#include "ForecastDialog.h"
#include "UserVarSet.h"
//#include "UserValSet.h"
#include "ReportOptionsSet.h"
#include <oledb.h>
#include "BatchOutputDoc.h"
#include "graphview.h"
#include "PercentileView.h"
#include "PocketCardSet.h"
#include "FireOptionsSet.h"
#include "CustomInitSet.h"
#include "RunPocketCardSet.h"
#include "FFPOverlaysSet.h"
#include "LFISet.h"
#include "LFIengine.h"
#include "WindRoseDoc.h"
#include "WindRoseFrame.h"
#include "eventlocatordoc.h"
#include "NFDRS2016DFMSet.h"
#include "NFDRS2016WxSet.h"
#include <random>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define NOVALUE -9999.9
#define RADPERDAY 0.017214
#define RADPERDEG 0.01745329
#define MINDECL -0.4092797
#define SECPERRAD 13750.9871
#define DAYSOFF 10.25

extern int curr_runID;
extern CString modelStrings[];
extern void CalcWeibullParams(vector<int> obsVals, double *ahat, double *bhat);

bool IsLeap(int year)
{
	bool isLeap = false;
	if(year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
		isLeap = true;
	return isLeap;
}

int DayOfYear(COleDateTime date)
{//adjusts day of year so all years have 366 days...
	bool isLeap = IsLeap(date.GetYear());//false;
	int day = date.GetDayOfYear();
	if(!isLeap && date.GetMonth() >= 3)
		day++;
	return day;
}

COleDateTime DateFromDOY(int year, int julianDay)
{
	COleDateTimeSpan span(julianDay, 0, 0, 0);
	COleDateTimeSpan day(1, 0, 0, 0);
	if(!IsLeap(year) && julianDay >= 60)
		span -= day;
	COleDateTime base(year, 1, 1, 0, 0, 0);
	COleDateTime ret;
	ret = base + span;
	return ret;
}

int daysInMonth[12] =
//{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

char *monthNames[12] =
{ "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

char *abbrevs[MAXVARIDS] =
{   " Temp", " AvgT", " MinT", " MaxT", "   RH", " AvRH", " MnRH", " MxRH", " Rain", " RnDr", " Wind",
	"   SC", "  ERC", "   BI", " KBDI", "   IC", "  FM1", " FM10", " F100", " 1000", "  FMH", "  FMW",
	"SFlag", " WDIR", "  SOW", " FFMC", "  DMC", "   DC", "  ISI", "  BUI", "  FWI", "  DSR", " GDir",
	" GSpd", " SolR", "WFlag", "DPT", "VPDM", "VPDA", "GSI", "WAZI", "HrRa", "LHerb", "LWood", " FFWI"};

char *varFormats[MAXVARIDS] =
{   "5.0f", "5.0f", "5.0f", "5.0f", "5.0f", "5.0f", "5.0f", "5.0f", "5.2f", "5.0f", "5.0f",
	"5.1f", "5.1f", "5.1f", "5.1f", "5.1f", "5.1f", "5.1f", "5.1f", "5.1f", "5.1f", "5.1f",
	"5.0f", "5.0f", "5.0f", "5.0f", "5.0f", "5.0f", "5.0f", "5.0f", "5.0f", "5.0f", "5.0f",
	"5.0f", "5.0f", "5.0f", "5.0f", "5.0f", "5.0f", "5.4f", "5.0f", "5.2f", "5.4f", "5.4f", "5.0f"};

extern CFireplusApp theApp;

/// is this an NFDRS2016 fuel model?
bool isNFDRS2016(char fm)
{
	if (fm == 'V' || fm == 'W' || fm == 'X' || fm == 'Y' || fm == 'Z')
		return true;
	return false;
}

int MonthChtoInt(CString chMonth)
{
	if(strcmpi(chMonth, "January") == 0)
			return 1;
	if(strcmpi(chMonth, "February") == 0)
			return 2;
	if(strcmpi(chMonth, "March") == 0)
			return 3;
	if(strcmpi(chMonth, "April") == 0)
			return 4;
	if(strcmpi(chMonth, "May") == 0)
			return 5;
	if(strcmpi(chMonth, "June") == 0)
			return 6;
	if(strcmpi(chMonth, "July") == 0)
			return 7;
	if(strcmpi(chMonth, "August") == 0)
			return 8;
	if(strcmpi(chMonth, "September") == 0)
			return 9;
	if(strcmpi(chMonth, "October") == 0)
			return 10;
	if(strcmpi(chMonth, "November") == 0)
			return 11;
	return 12;
}

double CalcDPT(double tempF, int RH)
{
	double safeTemp = min(140.0, tempF);
	int safeRH = max(5.0, RH);
	double dp = -398.36 - 7428.6 / (-15.674 + log(safeRH / 100.0 * exp(-7482.6/(safeTemp + 398.36) + 15.675)));
	return dp;
}

double CalcVP(double tempF)
{
	double tmpC =  (tempF - 32.0) / 1.8;
	double vp = 610.7 * exp((17.38 * tmpC)/(239 + tmpC));
	return vp;
}

double CalcVPD(double RH, double TempF)
{
	double vp = CalcVP(TempF);
	double vpd = vp - (RH / 100) * vp;
	if(vpd < 0.0)
		vpd = 0.0;
	return vpd;
}

double CalcVPDavg(double TempDewF, double TempAvgF)
{
	double vpDew = CalcVP(TempDewF);
	double vpAvg = CalcVP(TempAvgF);
	double vpd = vpAvg - vpDew;
	if(vpd < 0.0)
		vpd = 0.0;
	return vpd;
}

double CalcDayl(double lat,int yday)
{
    /* Daylength function from MT-CLIM */
    double coslat,sinlat; //,dt,dh,h;
    double decl,cosdecl,sindecl,cosegeom,sinegeom,coshss,hss;

    /* check for (+/-) 90 degrees latitude, throws off daylength calc */
    lat *= RADPERDEG;
    if (lat > 1.5707) lat = 1.5707;
    if (lat < -1.5707) lat = -1.5707;
    coslat = cos(lat);
    sinlat = sin(lat);

    /* calculate cos and sin of declination */
    decl = MINDECL * cos((yday + DAYSOFF) * RADPERDAY);
    cosdecl = cos(decl);
    sindecl = sin(decl);

    /* calculate daylength as a function of lat and decl */
    cosegeom = coslat * cosdecl;
    sinegeom = sinlat * sindecl;
    coshss = -(sinegeom) / cosegeom;
    if (coshss < -1.0) coshss = -1.0;  /* 24-hr daylight */
    if (coshss > 1.0) coshss = 1.0;    /* 0-hr daylight */
    hss = acos(coshss);                /* hour angle at sunset (radians) */
    /* daylength (seconds) */
    return 2.0 * hss * SECPERRAD;
}

char *GetTempFileName()
{
	//
	char buffer[_MAX_PATH], temp[32];
	// Get the current working directory:
	chdir(theApp.dbDir);//theApp.workDir);
	strcpy_s(buffer, theApp.dbDir);
	CFileFind finder;
	int i = 0;
	sprintf(temp, "FFP%05d.txt", i);
    BOOL bWorking = finder.FindFile(temp);
    while (bWorking)
    {
 		sprintf(temp, "FFP%05d.txt", ++i);
		bWorking = finder.FindFile(temp);
    }
	strcat(buffer, "\\");
	strcat(buffer, temp);
	return strdup(buffer);
}

double Round(double in)
{
	double ret = floor(in), ret2 = ceil(in);
	if(in - ret >= 0.5)
		return ret2;
	return ret;
}

CPocketCardOptions::CPocketCardOptions()
{
	m_Fire1Val = 0.0;
	m_Fire2Val = 0.0;
	m_Fire3Val = 0.0;
	dateStamp = "";
	m_RespAgency = "";
	m_NWCGStd = FALSE;
	areaPal = NULL;
	areaDIB = NULL;
	ytdPeriods = NULL;
}

CPocketCardOptions::~CPocketCardOptions()
{
	Destroy();
}

void CPocketCardOptions::Destroy(void)
{
	if(areaPal)
	{
		delete areaPal;
		areaPal = NULL;
	}
	if(	areaDIB != NULL)
	{
		::GlobalFree((HGLOBAL) areaDIB);
		areaDIB = NULL;
	}
	if(ytdPeriods)
	{
		for(int j = 0; j < MAXVARIDS; j++)
		{
			if(ytdPeriods[j])
			{
				for(int i = 0; i < numPeriods; i++)
					delete ytdPeriods[j][i];
				delete[] ytdPeriods[j];
			}
		}
		if(numPeriods > 0)
			delete[] ytdPeriods;
		ytdPeriods = NULL;
	}
}


bool ClimAnalysisRunProc(const CUPDUPDATA* pCUPDUPData)
{
	CAnalysisData* pData = (CAnalysisData*)pCUPDUPData->GetAppData();
	pData->pClim->pCUPDUPData = (CUPDUPDATA*)pCUPDUPData;
	pCUPDUPData->ShowProgressBar(false);
	pCUPDUPData->SetProgress(_T("Initializing.."), 0);
	int ret = pData->pClim->Analyze2(pData->varIDs, pData->nVarIDs, pData->pFpSet, pData->isFPA);

	if (ret == 0)
	{
		int wsFireYears = 0;
		Period *** wsPeriods = pData->pClim->GetWorkingSetPeriods(pData->pClim->periods);
		pData->pClim->m_opts.numtPeriods = pData->pClim->numPeriods;
		pData->pClim->m_opts.tPeriods = pData->pClim->periods;
		CFireDay** wsFires = pData->pClim->GetWorkingSetFires(pData->pClim->fires, &wsFireYears);
		pData->pClim->m_opts.tFires = pData->pClim->fires;
		pData->pClim->periods = wsPeriods;
		pData->pClim->fires = wsFires;
		pData->pClim->fireYears = wsFireYears;
		return true;
	}
	return false;
}

INT_PTR CClimAnalysis::RunAnalyze(int *_varIDs, int _nVarIDs, CFireplusSet *_fpSet, bool _inverted, bool _isFPA)
{
	CAnalysisData analysisData;
	analysisData.pClim = this;
	analysisData.nVarIDs = _nVarIDs;
	analysisData.varIDs = _varIDs;
	analysisData.pFpSet = _fpSet;
	analysisData.inverted = _inverted;
	analysisData.isFPA = _isFPA;
	CUPDialog cupDlg(AfxGetMainWnd()->m_hWnd, ClimAnalysisRunProc, &analysisData);
	INT_PTR ret = cupDlg.DoModal();
	return ret;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClimAnalysis::CClimAnalysis(CDatabase *db)
{
	pDB = db;
	climSet = NULL;
	fpSet = NULL;
	killfpSet = false;
	startDay = 0;
	varIDs = NULL;
	nVarIDs = 0;
	//recCounter = NULL;
	fireYears = 0;
	fires = NULL;
	inverted = false;
	checkFires = true;
	autosave = false;
	wxSet = NULL;
	m_usedExtremes = false;
	m_opts.m_OverlayDialog = NULL;
	m_opts.fires = NULL;
	m_opts.numPeriods = 0;
//	m_opts.periods = NULL;
	m_opts.numtPeriods = 0;
	m_opts.tPeriods = NULL;
	m_opts.tFires = NULL;
	int i;
	for(i = 0; i < OVERLAYS; i++)
	{
		m_opts.overLayYears[i] = 0;
		m_opts.overLayColors[i] = theApp.overLayColors[i];
		m_opts.lineStyles[i] = theApp.lineStyles[i];
	}
	for(i = 0; i < MAXMERGES; i++)
		m_opts.mergedIDs[i] = -1;
	m_opts.pDB = NULL;
	m_strModel = m_strStationName =  "";
	m_runID = curr_runID;
	humanCause = 1022;

	CClimateSet tClimSet(pDB);
	tClimSet.m_strSort = _T("[VarID]");
	tClimSet.Open();
	while (!tClimSet.IsEOF())
	{
		if (tClimSet.m_cp2 < tClimSet.m_CriticalPercentile)
			m_extremeIsMax[tClimSet.m_VarID - 1] = false;
		else
			m_extremeIsMax[tClimSet.m_VarID - 1] = true;
		tClimSet.MoveNext();
	}
	tClimSet.Close();
	m_strRequiredFields = " AND NOT([SolarRadiation]) IS NULL AND NOT([ObsDate]) IS NULL AND NOT([Temp]) IS NULL "
		"AND NOT([RH]) IS NULL AND NOT([PPTAMT]) IS NULL AND NOT([WS]) IS NULL AND NOT([TmpMax]) IS NULL AND NOT([TmpMin]) IS NULL "
		"AND NOT([RHMin]) IS NULL AND NOT([HourlyPrecip]) IS NULL";
	firesStartYear = 0;
}

CClimAnalysis::~CClimAnalysis()
{
	//clean up
	int years = 0;
	for(int j = 0; j < MAXVARIDS; j++)
	{
		if(killPeriods)
		{
			if(periods[j])
			{
				for(int i = 0; i < numPeriods; i++)
				{
					if(!years)
						years = periods[j][i]->years;
					delete periods[j][i];
				}
				delete[] periods[j];
			}
		}
	}
	if(numPeriods > 0)
	{
		if(killPeriods)
		{
			delete[] periods;
		}
		delete[] sDates;
		delete[] eDates;
	}
	if(nVarIDs > 0)
		delete[] varIDs;
	if(years && killPeriods)
		KillFires();
	if(wxSet)
	{
		wxSet->Close();
		delete wxSet;
		wxSet = NULL;
	}
	int tYears = 0;
	if(m_opts.tPeriods)
	{
		for(int j = 0; j < MAXVARIDS; j++)
		{
			if(m_opts.tPeriods[j])
			{
				for(int i = 0; i < m_opts.numtPeriods; i++)
				{
					if(!tYears)
						tYears = m_opts.tPeriods[j][i]->years;
					delete m_opts.tPeriods[j][i];
				}
				delete[] m_opts.tPeriods[j];
			}
		}
		if(m_opts.numtPeriods > 0)
			delete[] m_opts.tPeriods;
	}
	if(m_opts.tFires)
	{
		for(int y = 0; y < tYears; y++)
			delete[] m_opts.tFires[y];
		delete[] m_opts.tFires;
	}
	m_pocketOpts.Destroy();
	if(killfpSet)
	{
		fpSet->Close();
		delete fpSet;
	}
}

void CClimAnalysis::KillFires()
{
	if(fires)
	{
		for(int y = 0; y < fireYears; y++)
			delete[] fires[y];
		delete[] fires;
		fires = NULL;
	}
}

int CClimAnalysis::Analyze(int *_varIDs, int _nVarIDs, CFireplusSet *_fpSet, bool _inverted, bool isFPA/* = false*/)

{
	int ret = 0;
	inverted = _inverted;
	CWaitCursor wait;
	nVarIDs = _nVarIDs;
	varIDs = _varIDs;
	//model use flags - optimize to only calculate models used
	killPeriods = true;
	fpSet = _fpSet;
	useCanadian = false;
	useNFDRS = false;
	useLFI = false;
	count = 0;
	userCount = 0;
	for (int v = 0; v < nVarIDs; v++)
	{
		if (OptionFromVarID(varIDs[v]) == 1)
			useNFDRS = true;
		if (OptionFromVarID(varIDs[v]) == 3)
			useCanadian = true;
		if (varIDs[v] == 39 || varIDs[v] == 42 || varIDs[v] == 43)
			useLFI = true;
	}
	int sYear = fpSet->m_StartYear, eYear = fpSet->m_EndYear;
	CSIGStationSet staSet(pDB);
	staSet.Open();
	CString query = "", temp, staQuery;
	if(inverted)
	{//need to know how many years
		CString wxQuery = "";
		CWxSet tSet(fpSet->m_pDatabase);
		
		tSet.m_strSort = "[ObsDate]";
		tSet.Open();
		if(fpSet->m_SIG_Station.GetLength() > 6)//SIG
		{
			char sig[64];
			strcpy_s(sig, fpSet->m_SIG_Station);
			CString temp;
			CStationInSIGSet sSet(pDB);
			temp.Format("[SIG] = '%-20.20s'", &sig[6]);
			sSet.m_strFilter = _T(temp);
			sSet.Open();
			staSet.m_strFilter.Format("[StationID] = '%6.6s'", sSet.m_StationID);
			while(!sSet.IsEOF())
			{
				tSet.m_strFilter.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", sSet.m_StationID);
				tSet.Requery();
				if(!tSet.IsBOF() && !tSet.IsEOF())
				{
					sYear = min(sYear, tSet.m_ObsDate.GetYear());
					tSet.MoveLast();
					eYear = max(eYear, tSet.m_ObsDate.GetYear());
				}
				sSet.MoveNext();
			}
			if(HasUserVars())
			{
				CString tmpStr = "", strSQL;
				sSet.MoveFirst();
				strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%s'", sSet.m_StationID);
				sSet.MoveNext();
				while(!sSet.IsEOF())
				{
					tmpStr.Format(" or [StationID] = '%6.6s'", sSet.m_StationID);
					strSQL += tmpStr;
					sSet.MoveNext();
				}
				CRecordset userVals(pDB);
				userVals.m_strSort = _T("[ObsDate]");
				userVals.Open(CRecordset::dynaset, strSQL);
				if(!userVals.IsBOF() && !userVals.IsEOF())
				{
					COleDateTime dt;
					CDBVariant var;
					userVals.GetFieldValue("ObsDate", var);
					sYear = min(sYear, var.m_pdate->year);
					userVals.MoveLast();
					userVals.GetFieldValue("ObsDate", var);
					eYear = max(eYear, var.m_pdate->year);
				}
				userVals.Close();
			}
			sSet.Close();
		}
		else //single station
		{
			staQuery.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
			staSet.m_strFilter = _T(staQuery);
			staSet.Requery();
		    tSet.m_strFilter.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", fpSet->m_SIG_Station);
			tSet.Requery();
			if(!tSet.IsBOF() && !tSet.IsEOF())
			{
				sYear = min(sYear, tSet.m_ObsDate.GetYear());
				tSet.MoveLast();
				eYear = max(eYear, tSet.m_ObsDate.GetYear());
			}
			if(HasUserVars())
			{
				CRecordset userVals(pDB);
				CString strSQL;
				strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%s'", fpSet->m_SIG_Station);
				userVals.m_strSort = _T("[ObsDate]");
				userVals.Open(CRecordset::dynaset, strSQL);
				if(!userVals.IsBOF() && !userVals.IsEOF())
				{
					CDBVariant var;
					userVals.GetFieldValue("ObsDate", var);
					sYear = min(sYear, var.m_pdate->year);
					userVals.MoveLast();
					userVals.GetFieldValue("ObsDate", var);
					eYear = max(eYear, var.m_pdate->year);
				}
				userVals.Close();
			}
		}
		tSet.Close();
	}
	
	int pLen = atoi(fpSet->m_PeriodLength);
	COleDateTimeSpan span( pLen - 1, 0, 0, 0 );
	COleDateTime d1, d2, d3;
	numPeriods = 0;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d2 = d1 + span;
	// performance update 2012 - reduce number of Get () calls
	int d1M = d1.GetMonth(),
		d1D = d1.GetDay();

	//ensure don't cross month boundary
	while(d1M != d2.GetMonth())
		d2 -= 1;

	int d2M = d2.GetMonth(),
		d2D = d2.GetDay();

	while(1)
	{
		//new period coming into loop
		numPeriods++;
		if(MonthChtoInt(fpSet->m_EndMonth) == d1M
			&& fpSet->m_EndDay >= d1D
			&& fpSet->m_EndDay <= d2D)
			break;
		d3 = d2 + span;
		d3 += 1;
		if(d2M != d3.GetMonth())//does next period cross a month boundary?
		{
			if(d3.GetDay() <= pLen / 2)//will create a new period, otherwise will combine
				numPeriods++;
			if(MonthChtoInt(fpSet->m_EndMonth) == d1M
				&& fpSet->m_EndDay >= d1D
				&& fpSet->m_EndDay <= daysInMonth[d1M - 1])
				break;
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d1M = d1.GetMonth();
		    d1D = d1.GetDay();

			d2 = d1 + span;
			d2M = d2.GetMonth();
		    d2D = d2.GetDay();
		}
		else
		{
			d1 = d2;

			d1 += 1;
			d1M = d1.GetMonth();
		    d1D = d1.GetDay();
			d2 = d1 + span;
			d2M = d2.GetMonth();
		    d2D = d2.GetDay();
		}
		//ensure don't cross month boundary
		while(d1M != d2.GetMonth())
			d2 -= 1;

			d2M = d2.GetMonth();
		    d2D = d2.GetDay();
	}

	//ok, got numPeriods. Allocate date arrays and fill values by repeating loop
	sDates = new COleDateTime[numPeriods];
	eDates = new COleDateTime[numPeriods];
	int dLoc = 0;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d1M = d1.GetMonth();
	d1D = d1.GetDay();
	d2 = d1 + span;
	//safety
	while(d1M != d2.GetMonth())
		d2 -= 1;
	d2M = d2.GetMonth();
	d2D = d2.GetDay();
	while(1)
	{
		//new period coming into loop
		sDates[dLoc].SetDate(sYear, d1M, d1D);
		eDates[dLoc++].SetDate(eYear, d1M, d2D);
		d3 = d2 + span;
		d3 += 1;
		if(d2M != d3.GetMonth()) //will next period cross a month boundary???
		{
			if(d3.GetDay() <= pLen / 2 && dLoc < numPeriods)
			{
				//extra group to fill
				sDates[dLoc].SetDate(sYear, d1M, d2D + 1);
				if(IsLeap(eYear))
				{
					eDates[dLoc++].SetDate(eYear, d1M, daysInMonth[d1M - 1]);
				}
				else
				{
					if(d1M != 2)
						eDates[dLoc++].SetDate(eYear, d1M, daysInMonth[d1M - 1]);
					else
						eDates[dLoc++].SetDate(eYear, d1M, 28);
				}
				if(dLoc >= numPeriods)
					break;
			}
			else
			{//combine into previous group
				if(d1M < MonthChtoInt(fpSet->m_EndMonth))
				{
					if(IsLeap(eYear))// % 4 == 0 && (eYear % 100 != 0 || eYear % 400 == 0))
						eDates[dLoc - 1].SetDate(eYear, d1M, daysInMonth[d1M - 1]);
					else
					{
						if(d1M != 2)
							eDates[dLoc - 1].SetDate(eYear, d1M, daysInMonth[d1M - 1]);
						else
							eDates[dLoc - 1].SetDate(eYear, d1M, 28);
					}
				}
				else//last month of query
				{
					if(IsLeap(eYear))// % 4 == 0 && (eYear % 100 != 0 || eYear % 400 == 0))
						eDates[dLoc - 1].SetDate(eYear, d1M, fpSet->m_EndDay);
					else//daysInMonth[d1.GetMonth() - 1]);
					{
						if(d1M != 2)
							eDates[dLoc - 1].SetDate(eYear, d1M, fpSet->m_EndDay);
						else
							eDates[dLoc - 1].SetDate(eYear, d1M, min(fpSet->m_EndDay, 28));
					}
				}
			}
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d1M = d1.GetMonth();
			d1D = d1.GetDay();
			d2 = d1 + span;
			d2M = d2.GetMonth();
			d2D = d2.GetDay();
		}
		else
		{//won't cross boundary, increment normally
			d1 = d2;
			d1 += 1;
			d1M = d1.GetMonth();
			d1D = d1.GetDay();
			d2 = d1 + span;
			d2M = d2.GetMonth();
			d2D = d2.GetDay();
		}
		//ensure same month
		while(d1M != d2.GetMonth())
			d2 -= 1;
		d2M = d2.GetMonth();
		d2D = d2.GetDay();
		if(dLoc >= numPeriods)
			break;
	}

	periods = new Period**[MAXVARIDS];
	for(int p = 0; p < MAXVARIDS; p++)
		periods[p] = NULL;
	for(int v = 0; v < nVarIDs; v++)
	{
		//create data storage structure
		COleDateTime m1, m2;
		periods[varIDs[v]] = new Period*[numPeriods];
		for(int i = 0; i < numPeriods; i++)
			periods[varIDs[v]][i] = new Period(sDates[i], eDates[i]);
	}
	
	//DATA STORAGE CREATED.
	//now process selected station(s)
	//set up SIGStationSet
	fpSet->BuildBaseQuery(query, "ObsDate", true, inverted);

	if(query.GetLength() > 0)
		query += " AND ";
	m_usedExtremes = theApp.m_UseDailyExtremes;
	if(fpSet->m_SIG_Station.GetLength() <= 6)//a single station
	{
		staQuery.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
		staSet.m_strFilter.Format(staQuery);
		staSet.Requery();
		CString temp2;

		bool reCalc2016 = false;
		reCalc2016 = theApp.m_ForceNFDRS2016Recompute;
		COleDateTime n2Start, n2End;
		fpSet->GetNFDRS2016Range(staSet.m_StationID, &n2Start, &n2End);
		if (!reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0])) // we need hourly data for new calculator?
		{
			CWxSet tWxSet(pDB);
			tWxSet.m_strFilter.Format("[StationID] = '%6.6s' AND [DailyObs] = 1 AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
				n2Start.Format(), n2End.Format());
			tWxSet.Open();
			if (tWxSet.IsFieldNull(&tWxSet.m_FM1) || tWxSet.IsFieldNull(&tWxSet.m_FM10) || tWxSet.IsFieldNull(&tWxSet.m_FM100) || tWxSet.IsFieldNull(&tWxSet.m_FM1000) || tWxSet.IsFieldNull(&tWxSet.m_FuelTemperature)
				|| tWxSet.m_FM1 <= 0.0 || tWxSet.m_FM10 <= 0.0 || tWxSet.m_FM100 <= 0.0 || tWxSet.m_FM1000 <= 0.0 || tWxSet.m_FuelTemperature <= -999.0)
				reCalc2016 = true;
			if (!reCalc2016)
			{
				tWxSet.MoveLast();
				if (tWxSet.IsFieldNull(&tWxSet.m_FM1) || tWxSet.IsFieldNull(&tWxSet.m_FM10) || tWxSet.IsFieldNull(&tWxSet.m_FM100) || tWxSet.IsFieldNull(&tWxSet.m_FM1000) || tWxSet.IsFieldNull(&tWxSet.m_FuelTemperature)
					|| tWxSet.m_FM1 <= 0.0 || tWxSet.m_FM10 <= 0.0 || tWxSet.m_FM100 <= 0.0 || tWxSet.m_FM1000 <= 0.0 || tWxSet.m_FuelTemperature <= -999.0)
					reCalc2016 = true;
			}
			tWxSet.Close();
		}
		if (!useNFDRS)
			reCalc2016 = false;
		if(!reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0]) && !m_usedExtremes)
			temp2.Format("[StationID] = '%6.6s' AND [DailyObs] = 1 AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
				n2Start.Format(), n2End.Format());
		else if (reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0]))
		{
			query = "";
			temp2.Format("[StationID] = '%6.6s' AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
				n2Start.Format(), n2End.Format());
		}
		else if (m_usedExtremes && isNFDRS2016(staSet.m_NFDRSFM[0]))
			temp2.Format("[StationID] = '%6.6s' AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
				n2Start.Format(), n2End.Format());
		else
	       temp2.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", staSet.m_StationID);
		query += temp2;
		if(useNFDRS && isNFDRS2016(staSet.m_NFDRSFM[0]))
			query += m_strRequiredFields;

		wxSet = new CWxSet(pDB);
		wxSet->m_strSort = _T("[ObsDate]");
		wxSet->m_strFilter = query;
		wxSet->Open();

		m_strStationName = staSet.m_Name;
		m_strModel = GetModelString(&staSet);

		//ret = AnalyzeStation(fpSet->m_SIG_Station + " - " + staSet.m_Name);
		ret = AnalyzeStation(CString(fpSet->m_SIG_Station + " - " + staSet.m_Name), isFPA, "");

		wxSet->Close();
		delete wxSet;
		wxSet = NULL;
	}
	else
	//is a SIG - multiple stations
	{
		ret = AnalyzeSIG(query, isFPA);
		
	}

	//weather data read and analyzed....
	//make sure we have some data
	if (pCUPDUPData->ShouldTerminate())
	{
		staSet.Close();
		return -1;
	}
	bool hasSomeData = false;
	for(int v = 0; v < nVarIDs; v++)
	{
		for(int i = 0; i < numPeriods; i++)
		{
			if(periods[varIDs[v]][i]->N() > 0)
			{
				hasSomeData = true;
				break;
			}
		}
	}
	if(!hasSomeData && !inverted)
	{
		AfxMessageBox("Error: No valid data values found.");
		ret = -2;
	}

	//now get fires
	if(checkFires && ret == 0)
	{
		fireYears = eYear - sYear + 1;
		fires = new CFireDay *[fireYears];
		for(int y = 0; y < fireYears; y++)
			fires[y] = new CFireDay [numPeriods];
		CFiresSet fireSet(fpSet->m_pDatabase);
		fireSet.Open();
		//have query for fires, need to process and put into fires structure
		//note that will still have to treat fires by individual days,
		//then put days into periods
		double lA = 0.0, hA = 0.0;
		long lF = 0, hF = 0;
		if(fireSet.FilterToWorkingSet(fpSet, 0,0, inverted))//has fires
		{
			for (int y = 0; y < fireYears; y++)
			{
				//for each year...
				for (int i = 0; i < numPeriods; i++)
					//for (int i = 0; i < numPeriods && (!recCounter || !recCounter->Abort()); i++)
				{
					//see if there's fire days - look at each day
					//deal with leap years
					for(int d = DayOfYear(periods[varIDs[0]][i]->start);
						d <= DayOfYear(periods[varIDs[0]][i]->end);// && (! recCounter || !recCounter->Abort());
						d++)
					{
						lA = 0.0;
						hA = 0.0;
						lF = 0;
						hF = 0;
						while(!fireSet.IsEOF()
							&& fireSet.m_Discovery.GetYear() < periods[varIDs[0]][i]->baseYear + y)
							fireSet.MoveNext();
						while(!fireSet.IsEOF()
							&& fireSet.m_Discovery.GetYear() <= periods[varIDs[0]][i]->baseYear + y
							&& DayOfYear(fireSet.m_Discovery) < d)
							fireSet.MoveNext();
						if(!fireSet.IsEOF()
							&& fireSet.m_Discovery.GetYear() == periods[varIDs[0]][i]->baseYear + y
							&& DayOfYear(fireSet.m_Discovery) == d)
						{
							while(!fireSet.IsEOF()
								&& fireSet.m_Discovery.GetYear() == periods[varIDs[0]][i]->baseYear + y
								&& DayOfYear(fireSet.m_Discovery) == d)
							{
								switch(fireSet.m_StatisticalCause)
								{
								case 1://lightning
									lF++;
									lA = max(lA, fireSet.m_TotalAcres);
									break;
								default:
								      int passesCauseFilter = 1;

									 if (fireSet.m_AgencyID == 1) // USFS
										 passesCauseFilter = (humanCause >> (fireSet.m_StatisticalCause - 1)) & 1;
									 else
									 {
										 // convert to USFS cause type
										 int usfs = fireSet.m_StatisticalCause;
										 switch (fireSet.m_StatisticalCause)
										 {
											case 2: usfs = 4; break;
											case 4: usfs = 5; break;
											case 5: usfs = 7; break;
											case 6: usfs = 2; break;
											case 7: usfs = 6; break;
										 }
										passesCauseFilter = (humanCause >> (usfs - 1)) & 1;
									 }
		
			
									 if (passesCauseFilter)
									 {
										 hF++;
										 hA = max(hA, fireSet.m_TotalAcres);
									 }
								}
								fireSet.MoveNext();
								//if(recCounter)
								//	recCounter->Increment();
							}
						}
						fires[y][i].lightning = max(fires[y][i].lightning, lF);
						fires[y][i].human = max(fires[y][i].human, hF);
						fires[y][i].lAcresMax = max(fires[y][i].lAcresMax, lA);
						fires[y][i].hAcresMax = max(fires[y][i].hAcresMax, hA);
					}
				}
			}
		}
		fireSet.Close();
	}
	staSet.Close();
	return ret;
}

CString CClimAnalysis::PrecipTable(int loc, double criticalPercentile, bool createDoc/* = true*/)
{
	long recsUsed = 0;
	int pLen = atoi(fpSet->m_PeriodLength);
	bool isSIG = strlen(fpSet->m_SIG_Station) > 6 ? true : false;
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	//output header

	// added for batch
	// (code interspersed)
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(stream, "FireFamily Plus Statistical Summary Report\n");

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch

	CString extremeString = " ";
	if (m_usedExtremes)
	{
		if (m_extremeIsMax[loc])
			extremeString = " - Daily Max";
		else
			extremeString = " - Daily Min";
	}
	fprintf(stream, "Variable: %s%s\n", climSet->m_Variable_Name, extremeString);
	if (customInits)
		fprintf(stream,"(Calculated using at least one custom yearly init)\n");

	fprintf(stream, "                                           Mean, Median, Standard Deviation and Extreme Values\n");

	fprintf(stream, "\n         %d-Day and Monthly Period Mean Totals                            %d-Day and Monthly Extreme Values\n\n", pLen, pLen);
	fprintf(stream, "Period   No.   Mean    Std.   Critical  Highest      Lowest     |                Avg.     Std   Median   Period\n");
	fprintf(stream, "Begins  Years  Total   Dev.   Pcntile   Tot, Year    Tot, Year  |   High, Year   High     Dev.   High    Begins\n");
	fprintf(stream, "----------------------------------------------------------------------------------------------------------------\n");
	//now output the statistics
	for(int i = 0; i < numPeriods; i++)
	{
		fprintf(stream, " %02d/%02d  ",
			periods[loc][i]->start.GetMonth(), periods[loc][i]->start.GetDay());
		periods[loc][i]->OutputPrecipStats(stream, criticalPercentile);
		fprintf(stream, "   %02d/%02d\n",
			periods[loc][i]->start.GetMonth(), periods[loc][i]->start.GetDay());
		recsUsed += periods[loc][i]->N();
	}
	//now calculate and output monthly values and overall values
	fprintf(stream, "         No.   Mean    Std.   Critical  Highest      Lowest     |                Avg.     Std   Median\n");
	fprintf(stream, "Month   Years  Total   Dev.   Pcntile   Tot, Year    Tot, Year  |   High, Year   High     Dev.   High     Month\n");
	fprintf(stream, "----------------------------------------------------------------------------------------------------------------\n");
	//bool *yrsIn = new bool[periods[loc][0]->years];
	long monthN = 0;
	//int yrs = 0;//count of data years
	int highi = 0, lowi = 0, maxYeari = 0, nValidObs;
	double highAvg = -10000.0, lowAvg = 10000.0, maxVal = -10000.0,
		perMax = -10000.0, monthAvg = 0.0;
	//long n = 0;
	Stats maxs, sums;
	COleDateTime totStart, totEnd;
	totStart.SetDate(fpSet->m_StartYear, 1, 1);
	totEnd.SetDate(fpSet->m_EndYear, 1, 1);
	Period totals(totStart, totEnd);
	for(int m = 1; m <= 12; m++)
	{
		nValidObs = (int)(daysInMonth[m - 1] * 8 / 10);//must have readings for 80% of days in month
		for(int y = 0; y < periods[loc][0]->years; y++)
		{
			for(int i = 0; i < numPeriods; i++)
			{
				if(periods[loc][i]->start.GetMonth() == m)
				{//gotta count years, and get min and max values
					if(periods[loc][i]->yStats[y].N() >= 0)
					{
						monthN += periods[loc][i]->yStats[y].N();
						monthAvg += periods[loc][i]->yStats[y].N() * periods[loc][i]->yStats[y].Mean();
						totStart.SetDate(y + fpSet->m_StartYear, 1, 1);
						totals.Accumulate(totStart, periods[loc][i]->yStats[y].N() * periods[loc][i]->yStats[y].Mean());
						if(periods[loc][i]->yStats[y].Max() > perMax)
							perMax = periods[loc][i]->yStats[y].Max();
					}
				}
			}
			if(monthN > 0)
			{
				//yrs++;
				maxs.Accumulate(perMax);
				if(maxVal < perMax)
				{
					maxVal = perMax;
					maxYeari = y;
				}
				if(monthN > nValidObs)
				{
					if(monthAvg > highAvg)
					{
						highAvg = monthAvg;
						highi = y;
					}
					if(monthAvg < lowAvg)
					{
						lowAvg = monthAvg;
						lowi = y;
					}
					sums.Accumulate(monthAvg);
				}
			}
			//reset monthly values
			monthN = 0;
			monthAvg = 0;
			perMax = -10000.0;
		}
		sums.Complete();
		maxs.Complete();
		if(sums.N() > 0)
		{
			fprintf(stream, " %3s     %3d  %6.3f  %6.3f  %6.3f  %5.2f  %4d  %5.2f  %4d   "
				"  %5.2f  %4d  %6.3f  %6.3f  %6.3f    %3s\n",
				monthNames[m - 1], sums.N(),
				sums.Mean(), sums.StdDev(), sums.Percentile(criticalPercentile),
				highAvg, periods[loc][0]->start.GetYear() + highi,
				lowAvg, periods[loc][0]->start.GetYear() + lowi,
				maxs.Max(), periods[loc][0]->start.GetYear() + maxYeari,
				maxs.Mean(), maxs.StdDev(), maxs.Median(),
				monthNames[m - 1]);
		}
		sums.Reset();
		maxs.Reset();
		maxVal = highAvg = -10000.0;
		lowAvg = 10000.0;
	}
	fprintf(stream, "----------------------------------------------------------------------------------------------------------------\n");
	fprintf(stream, "Season\nTotals  ");
	totals.Complete();
	totals.OutputPrecipStats(stream, criticalPercentile);
	fprintf(stream, "   Totals\n");
	/****************************old way****************************************/
	if(isSIG)
		AddSIGDescription(stream);
	fprintf(stream, "\n%ld weather observations used. %ld records processed.\n%s", recsUsed, count, AddDateStamp());
	fclose(stream);
	if(autosave)
	{
		CString saveName = autosavePath + "precipTable.txt";
		unlink(saveName);
		rename(fName, saveName);
		free(fName);
		return saveName;
	}
	CString retName = fName;
	//now display the file in a child window
	if(createDoc)
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
				CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(fName);
				pDoc->isTemp = true;
				free(fName);
				CString tTitle;
				tTitle.Format("%s - %s", fpSet->m_SIG_Station, climSet->m_Variable_Name);
				tTitle += " - Statistics Table";
				pDoc->SetTitle(tTitle);
				return retName;
			}
		}
	}
	free(fName);
	return retName;
}

CString CClimAnalysis::StatsTable(int loc, double criticalPercentile, bool createDoc /*= true*/)
{
	long recsUsed = 0;
	int pLen = atoi(fpSet->m_PeriodLength);
	bool isSIG = strlen(fpSet->m_SIG_Station) > 6 ? true : false;
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	//output header

	// added for batch
	// (code interspersed)
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(stream, "FireFamily Plus Statistical Summary Report\n");

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch
	CString extremeString = " ";
	if (m_usedExtremes)
	{
		if (m_extremeIsMax[loc])
			extremeString = " - Daily Max";
		else
			extremeString = " - Daily Min";
	}
	fprintf(stream, "Variable: %s%s\n", climSet->m_Variable_Name, extremeString);
	if (customInits)
		fprintf(stream,"(Calculated using at least one custom yearly init)\n");
	fprintf(stream, "                                           Mean, Median, Standard Deviation and Extreme Values\n");

	fprintf(stream, "\n         %d-Day and Monthly Period Means                                        %d-Day and Monthly Extreme Values\n\n", pLen, pLen);
	fprintf(stream, "Period   No.          Std.  Critical Highest      Lowest     |                Avg.    Std  Median               Avg.   Std   Median Period\n");
	fprintf(stream, "Begins  Years  Mean   Dev.  Pcntile  Avg, Year    Avg, Year  |   High, Year   High    Dev.  High    Low, Year   Low    Dev.    Low  Begins\n");
	fprintf(stream, "------------------------------------------------------------------------------------------------------------------------------------------\n");
	//now output the statistics
	for(int i = 0; i < numPeriods; i++)
	{
		fprintf(stream, " %02d/%02d  ",
			periods[loc][i]->start.GetMonth(), periods[loc][i]->start.GetDay());
		if(loc == 38 || loc == 37)//VPD's
			periods[loc][i]->OutputVPDstats(stream, criticalPercentile);
		else if(loc == 39)
			periods[loc][i]->Outputstats(stream, criticalPercentile, true);
		else
			periods[loc][i]->Outputstats(stream, criticalPercentile);
	fprintf(stream, "   %02d/%02d\n",
			periods[loc][i]->start.GetMonth(), periods[loc][i]->start.GetDay());
		recsUsed += periods[loc][i]->N();
	}
	//now calculate and output monthly values
	//need each years monthly average...
	fprintf(stream, "\n         No.          Std.           Highest      Lowest     |                Avg.    Std  Median               Avg.   Std   Median\n");
	fprintf(stream, "Month   Years  Mean   Dev.           Avg, Year    Avg, Year  |   High, Year   High    Dev.  High    Low, Year   Low    Dev.    Low   Month\n");
	fprintf(stream, "------------------------------------------------------------------------------------------------------------------------------------------\n");
	bool *yrsIn = new bool[periods[loc][0]->years];
	int yrs = 0, highi = 0, lowi = 0, minYeari = 0, maxYeari = 0;
	long mN = 0, monthN = 0;
	double vary = 0.0, avg = 0.0, v = 0.0, highAvg = -10000.0, lowAvg = 10000.0,
		minVal = 10000.0, maxVal = -10000.0,
		monthAvg = 0.0, perMin = 10000.0, perMax = -10000.0;
	Stats mins, maxs;
	for(int m = 1; m <= 12; m++)
	{
		int i;
		for(i = 0; i < periods[loc][0]->years; i++)
			yrsIn[i] = false;
		for(int y = 0; y < periods[loc][0]->years; y++)
		{
			for(i = 0; i < numPeriods; i++)
			{
				if(periods[loc][i]->start.GetMonth() == m)
				{//gotta count years, and get min and max values
					if(periods[loc][i]->yStats[y].N() > 0)
					{
						monthN += periods[loc][i]->yStats[y].N();
						monthAvg += periods[loc][i]->yStats[y].N() * periods[loc][i]->yStats[y].Mean();
						mN += periods[loc][i]->yStats[y].N();
						avg += periods[loc][i]->yStats[y].N() * periods[loc][i]->yStats[y].Mean();
						if(!yrsIn[y])
						{
							yrsIn[y] = true;
							yrs++;
						}
						if(periods[loc][i]->yStats[y].Max() > perMax)
							perMax = periods[loc][i]->yStats[y].Max();
						if(periods[loc][i]->yStats[y].Min() < perMin)
							perMin = periods[loc][i]->yStats[y].Min();
					}
				}
			}
			if(monthN > 0)
			{
				monthAvg /= monthN;
				if(monthAvg > highAvg)
				{
					highAvg = monthAvg;
					highi = y;
				}
				if(monthAvg < lowAvg)
				{
					lowAvg = monthAvg;
					lowi = y;
				}
				mins.Accumulate(perMin);
				if(minVal > perMin)
				{
					minVal = perMin;
					minYeari = y;
				}
				maxs.Accumulate(perMax);
				if(maxVal < perMax)
				{
					maxVal = perMax;
					maxYeari = y;
				}
			}
			//reset monthly values
			monthN = 0;
			monthAvg = 0;
			perMin = 10000.0;
			perMax = -10000.0;
		}
		mins.Complete();
		maxs.Complete();
		//have new average, now calc SD
		if(mN > 1)
		{
			avg /= mN;
			for(i = 0; i < numPeriods; i++)
			{
				if(periods[loc][i]->start.GetMonth() == m && periods[loc][i]->N() > 0)
				{
					for(int y = 0; y < periods[loc][i]->years; y++)
					{
						for(int j = 0; j < periods[loc][i]->yStats[y].N(); j++)
						{
							v = periods[loc][i]->yStats[y].vals[j] - avg;
							vary += v * v;
						}
					}
				}
			}
			vary /= mN - 1;
		}
		//output variables
		if(yrs > 0)
		{
			if(loc == 38 || loc == 37)//VPD's
				fprintf(stream, " %3s     %3d  %5.0f  %5.0f         %5.0f  %4d  %5.0f  %4d   "
					"  %5.0f  %4d  %5.0f  %5.0f  %5.0f  %5.0f  %4d  %5.0f  %5.1f  %5.0f    %3s\n",
					monthNames[m - 1], yrs, avg, sqrt(vary),
					highAvg, periods[loc][0]->start.GetYear() + highi, lowAvg,
					periods[loc][0]->start.GetYear() + lowi,
					maxs.Max(), periods[loc][0]->start.GetYear() + maxYeari, maxs.Mean(), maxs.StdDev(), maxs.Median(),
					mins.Min(), periods[loc][0]->start.GetYear() + minYeari, mins.Mean(), mins.StdDev(), mins.Median(),
					monthNames[m - 1]);
			else if(loc == 39)//GSI
				fprintf(stream, " %3s     %3d  %5.2f %5.2f        %5.2f %4d  %5.2f %4d   "
					"  %5.2f %4d  %5.2f %5.2f %5.2f %5.2f %4d  %5.2f %5.2f %5.2f   %3s\n",
					monthNames[m - 1], yrs, avg, sqrt(vary),
					highAvg, periods[loc][0]->start.GetYear() + highi, lowAvg,
					periods[loc][0]->start.GetYear() + lowi,
					maxs.Max(), periods[loc][0]->start.GetYear() + maxYeari, maxs.Mean(), maxs.StdDev(), maxs.Median(),
					mins.Min(), periods[loc][0]->start.GetYear() + minYeari, mins.Mean(), mins.StdDev(), mins.Median(),
					monthNames[m - 1]);
			else
				fprintf(stream, " %3s     %3d  %5.1f  %5.1f         %5.1f  %4d  %5.1f  %4d   "
					"  %5.1f  %4d  %5.1f  %5.1f  %5.1f  %5.1f  %4d  %5.1f  %5.1f  %5.1f    %3s\n",
					monthNames[m - 1], yrs, avg, sqrt(vary),
					highAvg, periods[loc][0]->start.GetYear() + highi, lowAvg,
					periods[loc][0]->start.GetYear() + lowi,
					maxs.Max(), periods[loc][0]->start.GetYear() + maxYeari, maxs.Mean(), maxs.StdDev(), maxs.Median(),
					mins.Min(), periods[loc][0]->start.GetYear() + minYeari, mins.Mean(), mins.StdDev(), mins.Median(),
					monthNames[m - 1]);
		}
		//reset variables
		vary = 0.0;
		avg = 0.0;
		mN = 0;
		highAvg = -10000.0;
		lowAvg = 10000.0;
		minVal = 10000.0;
		maxVal = -10000.0;
		yrs = 0;
		highi = 0;
		lowi = 0;
		minYeari = 0;
		maxYeari = 0;
		mins.Reset();
		maxs.Reset();
	}
	delete[] yrsIn;
	if(isSIG)
		AddSIGDescription(stream);
	fprintf(stream, "\n%ld weather observations used. %ld records processed.\n%s",
		recsUsed, (loc >= STATICVARIDS) ? userCount : count, AddDateStamp());
	fclose(stream);
	if(autosave)
	{
		CString saveName;
		saveName = autosavePath + climSet->m_ShortName.Trim() + "StatsTable.txt";
		unlink(saveName);
		rename(fName, saveName);
		//CString retName = fName;
		free(fName);
		return saveName;
		//return;
	}
	//now display the file in a child window
	CString retName = fName;
	if(createDoc)
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
				CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(fName);
				pDoc->isTemp = true;
				free(fName);
				CString tTitle;
				tTitle.Format("%s - %s", fpSet->m_SIG_Station, climSet->m_Variable_Name);
				tTitle += " - Statistics Table";
				pDoc->SetTitle(tTitle);
				return retName;
			}
		}
	}
	free(fName);
	return retName;
}

int CClimAnalysis::GetPeriod(COleDateTime date)
{
	COleDateTime tDate = date;

	int tM = tDate.GetMonth();
	int tD = tDate.GetDay();

	if (tM == 2 && tD == 29)
	{
		tD = 28;
	}

	for(int i = 0; i < numPeriods; i++)
	{
		if(tM == sDates[i].GetMonth()
			&& tD >= sDates[i].GetDay()
			&& tD <= eDates[i].GetDay())
			return i;
	}
	return -1;
}

CPeriodDoc * CClimAnalysis::AddGraphView(CPeriodDoc * pDoc, int varID, int optType, int tnumPeriods /*= 0*/, Period ***tperiods /*= NULL*/,	CFireDay **tFires /*= NULL*/)
{
		POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

		while(curTemplatePos != NULL)
		{
			CDocTemplate* curTemplate =
				theApp.GetNextDocTemplate(curTemplatePos);
			CString str;
			curTemplate->GetDocString(str, CDocTemplate::docName);
			if(str == _T("GraphView"))
			{
				CGraphSet gSet(pDB);
				gSet.Open();
				if(!pDoc)
				{
					if(!autosave)
					{
						pDoc = (CPeriodDoc *)curTemplate->CreateNewDocument();
						pDoc->m_pAnalysis = this;
					}
					m_opts.dateTime = COleDateTime::GetCurrentTime();
					m_opts.nVars = nVarIDs;
					m_opts.pDB = pDB;
					killPeriods = false;
					//m_opts.periods = periods;
					m_opts.fires = fires;
					m_opts.numPeriods = numPeriods;
					m_opts.periodLength = atoi(fpSet->m_PeriodLength);
					//GRAPH OPTIONS
					m_opts.nWxObs = count;
					if(strncmp(fpSet->m_SIG_Station, "SIG - ", 6) == 0)//is a SIG
					{
						m_opts.stationName = fpSet->m_SIG_Station;
					}
					else
					{
						m_opts.stationName = fpSet->m_SIG_Station + "-" + m_strStationName;
					}
					m_opts.stationName.TrimRight();
					m_opts.model = m_strModel;
					m_opts.numtPeriods = tnumPeriods;
					m_opts.tPeriods = tperiods;
					m_opts.tFires = tFires;
					//check overlays
					CFFPOverlaysSet olSet(pDB);
					olSet.m_strFilter.Format("[runID] = %d", this->m_runID);
					olSet.Open();
					int oCnt = 0;
					if(olSet.IsEOF())
					{
						//no previously saved record for this SIG_Station
						olSet.m_strFilter = "[runID] = 0";
						olSet.Requery();
					}
					while(!olSet.IsEOF() && oCnt < OVERLAYS)
					{
						if(olSet.m_Year >= periods[varID][0]->baseYear
							&& olSet.m_Year < periods[varID][0]->baseYear + periods[varID][0]->years)
						{
							m_opts.overLayYears[oCnt] = olSet.m_Year;
							m_opts.overLayColors[oCnt] = olSet.m_Color;
							m_opts.lineStyles[oCnt] = olSet.m_Style;
							m_opts.lineWidths[oCnt] = olSet.m_Width;
							oCnt++;
						}
						else if(tnumPeriods > 0)
						{
							if(olSet.m_Year >= tperiods[varID][0]->baseYear
								&& olSet.m_Year < tperiods[varID][0]->baseYear + tperiods[varID][0]->years)
							{
								m_opts.overLayYears[oCnt] = olSet.m_Year;
								m_opts.overLayColors[oCnt] = olSet.m_Color;
								m_opts.lineStyles[oCnt] = olSet.m_Style;
								m_opts.lineWidths[oCnt] = olSet.m_Width;
								oCnt++;
							}
						}
						olSet.MoveNext();
					}
					olSet.Close();
					if(oCnt == 0)
					{
						int loc = 0;
						for(int i = 0; i < OVERLAYS; i++)
						{
							if(theApp.overLayYears[i] >= periods[varID][0]->baseYear
								&& theApp.overLayYears[i] < periods[varID][0]->baseYear + periods[varID][0]->years)
							{
								m_opts.overLayYears[loc] = theApp.overLayYears[i];
								m_opts.overLayColors[loc] = theApp.overLayColors[i];
								m_opts.lineStyles[loc] = theApp.lineStyles[i];
								m_opts.lineWidths[loc] = theApp.lineWidths[i];
								loc++;
							}
							else if(tnumPeriods > 0)
							{
								if(theApp.overLayYears[i] >= tperiods[varID][0]->baseYear
								&& theApp.overLayYears[i] < tperiods[varID][0]->baseYear + tperiods[varID][0]->years)
								{
									m_opts.overLayYears[loc] = theApp.overLayYears[i];
									m_opts.overLayColors[loc] = theApp.overLayColors[i];
									m_opts.lineStyles[loc] = theApp.lineStyles[i];
									m_opts.lineWidths[loc] = theApp.lineWidths[i];
									loc++;
								}
							}
						}
					}
					//array of variable names....
					for(int i = 0; i < MAXVARIDS; i++)
						varNames[i] = "";
					CClimateSet tclimSet(pDB);
					//tclimSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
					tclimSet.Open();
					while(!tclimSet.IsEOF())
					{
						varNames[tclimSet.m_VarID - 1] = tclimSet.m_ShortName;
						tclimSet.MoveNext();
					}
					tclimSet.Close();
					CUserVarSet usv(pDB);
					usv.Open();
					while(!usv.IsEOF())
					{
						if(usv.m_VarID <= MAXVARIDS && usv.m_VarID > STATICVARIDS)
							varNames[usv.m_VarID - 1] = usv.m_Abbrev;
						usv.MoveNext();
					}
					usv.Close();
				}
				CSplitterFrame* pFrame = NULL;
				CGraphView *pView = NULL;
				if(!autosave)
				{
					pFrame = (CSplitterFrame*)curTemplate->CreateNewFrame(pDoc, NULL);
					pView = (CGraphView *) pFrame->m_wndSplitter.GetPane(0, 0);
				}
				else
					pView = new CGraphView();
				pView->m_graphOpts.varID = varID;
				pView->m_graphOpts.optionType = optType;
				pView->m_graphOpts.varName = climSet->m_Variable_Name;
				pView->m_graphOpts.isBargraph = (gSet.m_InitBargraph == 0) ? false : true;
				pView->m_graphOpts.criticalPercentile = climSet->m_CriticalPercentile / 100.0;
				pView->m_graphOpts.criticalPercentile2 = climSet->m_cp2 / 100.0;
				pView->m_graphOpts.m_Horizontal_Lines = gSet.m_Horizontal_Lines;
				pView->m_graphOpts.m_Vertical_Lines = gSet.m_Vertical_Lines;
				pView->m_graphOpts.m_Range_Tics = gSet.m_Range_Tics;
				pView->m_graphOpts.m_CP_Tics = gSet.m_CP_Tics;
				pView->m_graphOpts.m_Line_Width = gSet.m_Line_Width;
				pView->m_graphOpts.m_H_Line_Style = gSet.m_H_Line_Style;
				pView->m_graphOpts.m_V_Line_Style = gSet.m_V_Line_Style;
				pView->m_graphOpts.m_Line_Color = (COLORREF)gSet.m_Line_Color;
				pView->m_graphOpts.m_cp1s = gSet.m_CP1;
				pView->m_graphOpts.m_cp2s = gSet.m_CP2;
				pView->m_graphOpts.m_maxs = gSet.m_Maxs;
				pView->m_graphOpts.m_mins = gSet.m_Mins;
				pView->m_graphOpts.m_cp1Width = gSet.m_CP1Width;
				pView->m_graphOpts.m_cp2Width = gSet.m_CP2Width;
				pView->m_graphOpts.m_maxsWidth = gSet.m_MaxsWidth;
				pView->m_graphOpts.m_minsWidth = gSet.m_MinsWidth;
				pView->m_graphOpts.m_cp1Color = gSet.m_CP1Color;
				pView->m_graphOpts.m_cp2Color = gSet.m_CP2Color;
				pView->m_graphOpts.m_maxsColor = gSet.m_MaxsColor;
				pView->m_graphOpts.m_minsColor = gSet.m_MinsColor;
				pView->m_graphOpts.m_Cause = gSet.m_Cause;
				pView->m_graphOpts.m_FD = gSet.m_FD;
				pView->m_graphOpts.m_LFD = gSet.m_LFD;
				pView->m_graphOpts.m_MFD = gSet.m_MFD;
				pView->m_graphOpts.m_LFDAcres = gSet.m_LFDAcres;
				pView->m_graphOpts.m_MFDFires = gSet.m_MFDFires;
				pView->m_graphOpts.m_OverallCP1 = theApp.m_OverallCP1;
				pView->m_graphOpts.m_OverallCP2 = theApp.m_OverallCP2;
				pView->m_graphOpts.m_OverallCP1Color = theApp.m_OverallCP1Color;
				pView->m_graphOpts.m_OverallCP2Color = theApp.m_OverallCP2Color;
				// added for batch2
				pView->m_graphOpts.fpSet = fpSet;
				pView->m_pAnalysis = this;
				CPercentileView *pctView = NULL;
				if(!autosave)
				{
					pctView = (CPercentileView *) pFrame->m_wndSplitter.GetPane(0, 1);
				}
				else
					pctView = new CPercentileView();
				//pctView->m_graphOpts = &pView->m_graphOpts;
				pctView->optionType = optType;
				pctView->varID = varID;
				pctView->varName = climSet->m_Variable_Name;
				pctView->criticalPercentile = climSet->m_CriticalPercentile / 100.0;
				pctView->criticalPercentile2 = climSet->m_cp2 / 100.0;
				pctView->m_pAnalysis = this;
				if (!climSet->IsFieldNull(&climSet->m_FilterValue))
					pctView->filterValue = climSet->m_FilterValue;
				if (climSet->m_cp2 < climSet->m_CriticalPercentile)
					pctView->filterDirection = FILTER_GREATERTHAN_EQUAL;

				ForecastOptions fo;
				fo.Read(pDB);
				pView->m_graphOpts.m_fxColor = fo.lineColor;
				pView->m_graphOpts.m_fxWidth = fo.lineWidth;
				pView->m_graphOpts.m_fxStyle = fo.lineStyle;
				if(!autosave)
				{
					//create the view
					curTemplate->InitialUpdateFrame(pFrame, pDoc);
					CString tTitle;
					tTitle.Format("%s - %s", fpSet->m_SIG_Station, climSet->m_Variable_Name);
					tTitle += " - Statistics Graph";
					pFrame->SetWindowText(tTitle);
				}
				else //just save views to disk then destroy the view
				{
					CRect rect(0, 0, gSet.m_AutoSaveWidth, gSet.m_AutoSaveHeight);
					pView->PreDrawCalcs();
					CString saveName;
					saveName = autosavePath + climSet->m_ShortName.Trim() + "Graph." + gSet.GetGraphicsExtension();
					pView->SaveGraph(saveName, &rect);
					delete pView;
					delete pctView;
				}
				return pDoc;
				gSet.Close();
			}
		}
	return pDoc;
}

const double NODATA = -999.0;

int CClimAnalysis::AnalyzeStation(CString stationStr, bool isFPA /* = false*/, CString sigID)
{
	if (pCUPDUPData && pCUPDUPData->ShouldTerminate())
		return -1;
	int ret = 0;

	customInits = false;
	
	bool goodRecord;

	CSIGStationSet staSet(pDB);
	staSet.m_strFilter.Format("[StationID] = '%6.6s'", stationStr);
	staSet.Open();
	//LFI goodies
	// 2015 change: if analyzing a SIG, we use the LFI options for that SIG
	
	CString  lfiStationID;

	
	if (sigID.GetLength() > 1)
	{
		// first, make sure LFIoptions has an entry for this SIG
		CLFISet lfiSet(pDB);
		lfiSet.m_strFilter.Format("[SIG_Station] = '%s'", sigID);
		lfiSet.Open();
		if(! lfiSet.IsEOF())
            lfiStationID = sigID;
		else
			lfiStationID = staSet.m_StationID;
		lfiSet.Close();
	} else
		lfiStationID = staSet.m_StationID;

	
	bool reCalc2016 = false;
	reCalc2016 = theApp.m_ForceNFDRS2016Recompute;
	if (!reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0]))
	{//check wxSet for previously run NFDRS2016
		wxSet->Requery();
		//wxSet.m_strFilter.Format("([StationID] = '%s' AND [DailyObs] = 1)", m_sigStaID);
		if (wxSet->IsFieldNull(&wxSet->m_FM1) || wxSet->IsFieldNull(&wxSet->m_FM10) || wxSet->IsFieldNull(&wxSet->m_FM100) || wxSet->IsFieldNull(&wxSet->m_FM1000) || wxSet->IsFieldNull(&wxSet->m_FuelTemperature)
			|| wxSet->m_FM1 <= 0.0 || wxSet->m_FM10 <= 0.0 || wxSet->m_FM100 <= 0.0 || wxSet->m_FM1000 <= 0.0 || wxSet->m_FuelTemperature <= -999.0)
			reCalc2016 = true;
		if (!reCalc2016)
		{
			wxSet->MoveLast();
			if (wxSet->IsFieldNull(&wxSet->m_FM1) || wxSet->IsFieldNull(&wxSet->m_FM10) || wxSet->IsFieldNull(&wxSet->m_FM100) || wxSet->IsFieldNull(&wxSet->m_FM1000) || wxSet->IsFieldNull(&wxSet->m_FuelTemperature)
				|| wxSet->m_FM1 <= 0.0 || wxSet->m_FM10 <= 0.0 || wxSet->m_FM100 <= 0.0 || wxSet->m_FM1000 <= 0.0 || wxSet->m_FuelTemperature <= -999.0)
				reCalc2016 = true;
		}
	}
	CLFIengine lfi(pDB, 0, lfiStationID),
		lfiHerb(pDB, HerbParams, lfiStationID, staSet.m_HerbAnnual),
		lfiWoody(pDB, WoodyParams, lfiStationID);
	double pptAcc = 0;
	//create data structures based on periods and selected outputs
	//NOW CREATE NFDRCALC MODELS NEEDED
	wxSet->Requery();
	int sKBDI = 0;
	if(!staSet.IsFieldNull(&staSet.m_StartKBDI))
		sKBDI = staSet.m_StartKBDI;
	if(useNFDRS || useLFI)
	{
		int lookGreen, lookFreeze;// , sKBDI = 0;
		lookGreen = DayOfYear(staSet.m_GreenJulian);
		lookFreeze = DayOfYear(staSet.m_FreezeJulian);
		if (isNFDRS2016(staSet.m_NFDRSFM[0]) || useLFI)
		{ // new NFDRS2016 calc
			/*
			if (wxSet->m_ObsDate.GetYear() == 2001 && wxSet->m_ObsDate.GetMonth() == 7 && wxSet->m_ObsDate.GetDay() == 16 && wxSet->m_StationID.Compare("241513") == 0)
			{
				CString sfName;
				sfName.Format("%s\\241513_2001071523.2016_32", theApp.dbDir);// , stationStr, wxDay.GetYear(), wxDay.GetMonth(), wxDay.GetDay(), wxDay.GetHour());
				string fName = sfName.GetBuffer();
				theApp.m_NFDRS2016.ReadState(fName);
			}
			else
			{*/
				theApp.m_NFDRS2016.Init(staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true, staSet.m_HerbAnnual, staSet.m_KBDIThreshold);
				theApp.m_NFDRS2016.iSetFuelModel(staSet.m_NFDRSFM[0]);
				theApp.m_NFDRS2016.SetGSIParams(lfi.GetHerbMaxGSI(), lfi.GetHerbGreenup(), lfi.GetTminMin(), lfi.GetTminMax(), lfi.GetVPDMin(), lfi.GetVPDMax(), 
					lfi.GetDaylenMin(), lfi.GetDaylenMax(), lfi.GetMAPeriod(), lfi.IsUsingVPDAvg(), lfi.GetNumPrecipDays(), lfi.GetRTPcpMin(), lfi.GetRTPcpMax(), lfi.GetUseRTPrecip());
				theApp.m_NFDRS2016.SetHerbGSIparams(lfiHerb.GetHerbMaxGSI(), lfiHerb.GetHerbGreenup(), lfiHerb.GetTminMin(), lfiHerb.GetTminMax(), 
					lfiHerb.GetVPDMin(), lfiHerb.GetVPDMax(), lfiHerb.GetDaylenMin(), lfiHerb.GetDaylenMax(), lfiHerb.GetMAPeriod(), lfiHerb.IsUsingVPDAvg(), 
					lfiHerb.GetNumPrecipDays(), lfiHerb.GetRTPcpMin(), lfiHerb.GetRTPcpMax(), lfiHerb.GetUseRTPrecip(), lfiHerb.GetHerbMin(), lfiHerb.GetHerbMax());
				theApp.m_NFDRS2016.SetWoodyGSIparams(lfiWoody.GetWoodyMaxGSI(), lfiWoody.GetWoodyGreenup(), lfiWoody.GetTminMin(), lfiWoody.GetTminMax(), 
					lfiWoody.GetVPDMin(), lfiWoody.GetVPDMax(), lfiWoody.GetDaylenMin(), lfiWoody.GetDaylenMax(), lfiWoody.GetMAPeriod(), lfiWoody.IsUsingVPDAvg(), 
					lfiWoody.GetNumPrecipDays(), lfiWoody.GetRTPcpMin(), lfiWoody.GetRTPcpMax(), lfiWoody.GetUseRTPrecip(), lfiWoody.GetWoodyMin(), lfiWoody.GetWoodyMax());
				theApp.m_NFDRS2016.SetStartKBDI(staSet.m_StartKBDI);
				theApp.m_NFDRS2016.SetSCMax(staSet.GetSCM(staSet.m_NFDRSFM[0]));
				theApp.m_NFDRS2016.SetMxdHumid(staSet.GetMxHumid(staSet.m_NFDRSFM[0]));
			//}
		}
		if (!isNFDRS2016(staSet.m_NFDRSFM[0])) 
		{   // "old" NFDRS calc
			theApp.m_NFDRS.iInitialize((staSet.m_Use88 && !isFPA) ? 88 : 78,//staSet.m_Use88 ? 88 : 78,
				staSet.m_ClimateCls, lookFreeze, lookGreen,
				(int)(staSet.m_LatDegrees), staSet.m_HerbAnnual, staSet.m_Deciduous, staSet.m_FM1Eq10,
				staSet.m_Start1000 - 5.0, staSet.m_Start1000, sKBDI, staSet.m_AvgPrecip);
			theApp.m_NFDRS.iSetFuelModel((!isFPA) ? staSet.m_NFDRSFM[0] : 'G', 0);
			theApp.OverrideMXD(&staSet);
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
	int  iRainEvent, iStage, iSC, iIC,
		iFIL, iBI,
		iKBDI, init = 1, iDeclareGreenUp = 0, iDeclareFreeze = 0;//,
	double f1, f10, f100, f1000, fHerb, fWood, fGren, fX1000, fROS, fERC, fFL,
		ffmc, dmc, dc, bui, isi, fwi, dsr,ppt24;

	double fSC, fIC, fFIL, fBI, fKBDI;

	COleDateTime yesterday, wxDay;//used to skip duplicates
	COleDateTime extremeDay;//used keep track of current day for extremes
	//wxSet->Requery();

	int recs = 0;
	char progText[256];
	double dailyExtremes[MAXVARIDS];
	std::fill_n(dailyExtremes, MAXVARIDS, NODATA);
	//bool extremeIsMax[MAXVARIDS];

	/*if (theApp.m_UseDailyExtremes)
	{
		CClimateSet tClimSet(fpSet->m_pDatabase);
		tClimSet.m_strSort = _T("[VarID]");
		tClimSet.Open();
		while (!tClimSet.IsEOF())
		{
			if (tClimSet.m_cp2 < tClimSet.m_CriticalPercentile)
				extremeIsMax[tClimSet.m_VarID - 1] = false;
			else
				extremeIsMax[tClimSet.m_VarID - 1] = true;
			tClimSet.MoveNext();
		}
		tClimSet.Close();
	}*/
	if(!wxSet->IsBOF() && ! pCUPDUPData->ShouldTerminate())
	{
		//working variables
		int tmp, tmpMax, tmpMin, rh, rhMax, rhMin, pptDur, sow, greenHerb, greenShrub,
			season, windSpd, windDir, slopeClass, wetFlag, snowFlag;
		double pptAmt, omc10,lfiVal, lfiHerbVal, lfiWoodyVal;
		while(!wxSet->IsEOF() && !pCUPDUPData->ShouldTerminate())
		{

/*			if (wxSet->m_ObsDate.GetYear() == 2001 && wxSet->m_ObsDate.GetMonth() == 7 && wxSet->m_ObsDate.GetDay() == 16 
				&& wxSet->m_ObsDate.GetHour() == 0 && wxSet->m_StationID.Compare("241513") == 0)
			{
				CString sfName;
				sfName.Format("%s\\241513_2001071523.2016", theApp.dbDir);// , stationStr, wxDay.GetYear(), wxDay.GetMonth(), wxDay.GetDay(), wxDay.GetHour());
				string fName = sfName.GetBuffer();
				bool stateStatus = theApp.m_NFDRS2016.ReadState(fName);
			}*/
			int year1 = wxSet->m_ObsDate.GetYear();
			int month1 = wxSet->m_ObsDate.GetMonth();
			int day1 = wxSet->m_ObsDate.GetDay();
			int hour1 = wxSet->m_ObsDate.GetHour();
			int julian1 = wxSet->m_ObsDate.GetDayOfYear();
			recs ++;

			if(((useNFDRS || useCanadian) && (wxSet->IsFieldNull(&wxSet->m_Temp) || wxSet->IsFieldNull(&wxSet->m_RH))
				|| wxSet->IsFieldNull(&wxSet->m_ObsDate)))
			{
				wxSet->MoveNext();// GetNext();
				continue;
			}
			wxDay.SetDateTime(wxSet->m_ObsDate.GetYear(), wxSet->m_ObsDate.GetMonth(), wxSet->m_ObsDate.GetDay(),
				wxSet->m_ObsDate.GetHour(), wxSet->m_ObsDate.GetMinute(), wxSet->m_ObsDate.GetSecond());
			if(!init && yesterday >= wxDay)
			{
				wxSet->MoveNext();//GetNext();
				continue;
			}
			if (init)
				extremeDay = wxDay;

			// new custom init check    12/2006
			// if this is first record for new year, look for inits
			if (yesterday.GetYear() != wxDay.GetYear())
			{
                CCustomInitSet ciSet(fpSet->m_pDatabase); //  and [Enabled] <> 0
				ciSet.m_strFilter.Format("[SIG_Station]='%s' AND [Year]=%d AND [Enabled] <> 0", wxSet->m_StationID.Trim(),wxDay.GetYear());
			

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
						customInits = true;
				   }
				   if(useCanadian)
				   {
						if (!ciSet.IsFieldNull(&ciSet.m_FFMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DC) &&
							!ciSet.IsFieldNull(&ciSet.m_StartUpDay))
						{
							int newStart = DayOfYear(ciSet.m_StartUpDay);
							canadian.SetAnnualInit(newStart, wxDay.GetYear(), ciSet.m_FFMC, ciSet.m_DMC, ciSet.m_DC);
							usedCANcust = true;
							customInits = true;
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
			yesterday = wxDay;//wxSet->m_ObsDate;
		
			if (extremeDay.GetDayOfYear() != wxDay.GetDayOfYear() || extremeDay.GetYear() != wxDay.GetYear())
			{
				for (int j = 0; j < MAXVARIDS; j++)
				{
					if (periods[j] && dailyExtremes[j] != NODATA)
					{
						int per = GetPeriod(extremeDay);//wxSet->m_ObsDate);
						periods[j][per]->Accumulate(extremeDay, dailyExtremes[j]);
					}
				}

				//memset(dailyExtremes, NODATA, MAXVARIDS * sizeof(double));
				std::fill_n(dailyExtremes, MAXVARIDS, NODATA);
				extremeDay = wxDay;
			}
		
			double hourlyPrecip = wxSet->IsFieldNull(&wxSet->m_HourlyPrecip) ? 0 : wxSet->m_HourlyPrecip;
			double solarRad = wxSet->IsFieldNull(&wxSet->m_SolarRadiation) ? 0 : wxSet->m_SolarRadiation;
			
			tmp = wxSet->m_Temp;
			tmpMax = wxSet->IsFieldNull(&wxSet->m_TmpMax) ? -10000 : wxSet->m_TmpMax;
			tmpMin = wxSet->IsFieldNull(&wxSet->m_TmpMin) ? -10000 : wxSet->m_TmpMin;
			rh = max(wxSet->m_RH, 1);
			rhMax = wxSet->IsFieldNull(&wxSet->m_RHMax) ? -10000 : wxSet->m_RHMax;
			rhMin = wxSet->IsFieldNull(&wxSet->m_RHMin) ? -10000 : max(wxSet->m_RHMin, 1);
			pptDur = wxSet->IsFieldNull(&wxSet->m_PPTDUR) ? 0 : wxSet->m_PPTDUR;
			sow = wxSet->ResolveSOW();//wxSet->IsFieldNull(&wxSet->m_SOW) ? 0 : wxSet->m_SOW;
			greenHerb = wxSet->IsFieldNull(&wxSet->m_GreenHerb) ? 0 : wxSet->m_GreenHerb;
			greenShrub = wxSet->IsFieldNull(&wxSet->m_GreenShrub) ? 0 : wxSet->m_GreenShrub;
			season = wxSet->IsFieldNull(&wxSet->m_Season) ? 0 : wxSet->m_Season;
			windSpd = wxSet->IsFieldNull(&wxSet->m_WS) ? 0 : wxSet->m_WS;
			windDir = wxSet->IsFieldNull(&wxSet->m_WDir) ? 0 : wxSet->m_WDir;
			slopeClass = staSet.m_SlopeCls;
			slopeClass = max(slopeClass, 1);
			slopeClass = min(slopeClass, 5);
			pptAmt = wxSet->IsFieldNull(&wxSet->m_PPTAMT) ? 0.0 : wxSet->m_PPTAMT;
			omc10 = (bUseStick && !wxSet->IsFieldNull(&wxSet->m_OMC10)) ? wxSet->m_OMC10 : -1000.0;
			wetFlag = wxSet->ResolveWetFlag();//IsFieldNull(&wxSet->m_WetFlag) ? 0 : wxSet->m_WetFlag;
			snowFlag = wxSet->IsFieldNull(&wxSet->m_SnowFlag) ? 0 : wxSet->m_SnowFlag;
			bool isDailyObs = wxSet->IsFieldNull(&wxSet->m_DailyObs) ? false : wxSet->m_DailyObs;
			//calculate model outputs
			//NFDR 78
			if(useNFDRS || useLFI)
			{
			
				if (isNFDRS2016(staSet.m_NFDRSFM[0]) || useLFI)// && reCalc2016)
				{ // new NFDRS2016 calc
					if (reCalc2016)
						theApp.m_NFDRS2016.Update(year1, month1, day1, hour1, julian1, (double)tmp, (double)tmpMin, (double)tmpMax, (double)rh, 
						(double)rhMin, hourlyPrecip, pptAmt, solarRad, (double)windSpd, snowFlag, 
							(isDailyObs) ? hour1 : -1);// staSet.m_RegSchdObs);
					else if(wxSet->m_DailyObs)
					{
						theApp.m_NFDRS2016.UpdateDaily(year1, month1, day1, julian1, (double)tmp, (double)tmpMin, (double)tmpMax, (double)rh, (double)rhMin,
							pptAmt, (double)windSpd, wxSet->m_FM1, wxSet->m_FM10, wxSet->m_FM100, wxSet->m_FM1000, wxSet->m_FuelTemperature, snowFlag);
					}
					else if (!m_usedExtremes)
					{
						wxSet->MoveNext();
						continue;
					}
					if (!reCalc2016 && m_usedExtremes)
					{
						theApp.m_NFDRS2016.iSetFuelMoistures(wxSet->m_FM1, wxSet->m_FM10, wxSet->m_FM100, wxSet->m_FM1000,
							wxSet->m_MCWood, wxSet->m_MCHerb, wxSet->m_FuelTemperature);
						theApp.m_NFDRS2016.iCalcIndexes(windSpd, slopeClass, &fSC, &fERC, &fBI, &fIC);
					}
					pptAcc += hourlyPrecip;
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
					fSC = theApp.m_NFDRS2016.SC;
					fERC = theApp.m_NFDRS2016.ERC;
					fFL = 0; //fixme
					iFIL = 0; //fixme
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

					//store moistures?
					if (reCalc2016)
					{
						wxSet->Edit();
						wxSet->m_FM1 = f1;
						wxSet->m_FM10 = f10;
						wxSet->m_FM100 = f100;
						wxSet->m_FM1000 = f1000;
						wxSet->m_FuelTemperature = theApp.m_NFDRS2016.GetFuelTemperature();
						wxSet->m_MCHerb = fHerb;
						wxSet->m_MCWood = fWood;
						wxSet->Update();
/*
						if (wxSet->m_ObsDate.GetMonth() == 7 && wxSet->m_ObsDate.GetDay() == 15 && wxSet->m_ObsDate.GetHour() == 23)
						{
							CString sfName;
							sfName.Format("%s\\%6.6s_%4d%02d%02d%02d.2016", theApp.dbDir, stationStr, wxDay.GetYear(), wxDay.GetMonth(), wxDay.GetDay(), wxDay.GetHour());
							string fName = sfName.GetBuffer();
							bool saveStatus = theApp.m_NFDRS2016.SaveState(fName);
						}
*/
					}
					// NFDR2016 uses hourly data for its moisture calcs
					// but...
					// since we really only want the daily data, skip to the next record if hour != 1300 (or 1400, etc.)
					if (hour1 != staSet.m_RegSchdObs && reCalc2016 && !m_usedExtremes)
					{
						count++;
						init = 0;
						wxSet->MoveNext();//GetNext();
						continue;
					}


				}
				if(!isNFDRS2016(staSet.m_NFDRSFM[0]) && wxSet->m_DailyObs != 0)
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
						DayOfYear(wxSet->m_ObsDate),
						wxSet->m_ObsDate.GetYear(),
						iDeclareGreenUp,
						iDeclareFreeze,
						greenHerb,
						greenShrub,
						season,
						wetFlag,
						snowFlag,
						&f1, &f10, &f100, &f1000, &fHerb, &fWood, &iStage, &fGren, &fX1000, &iRainEvent, &iKBDI);
					theApp.m_NFDRS.iCalcIndexes(windSpd, slopeClass, &fROS, &iSC, &fERC, &fFL, &iFIL, &iBI);
					theApp.m_NFDRS.iCalcIC(tmp, sow, f1, iSC, &iIC);
					fSC = (double)iSC;
					fFIL = (double)iFIL;
					fBI = (double)iBI;
					fIC = (double)iIC;
				}
			}
			
			//CANADIAN
			if(useCanadian && wxSet->m_DailyObs != 0)
				canadian.CalcIndices(DayOfYear(wxSet->m_ObsDate), wxSet->m_ObsDate.GetMonth(), wxSet->m_ObsDate.GetYear(),
					tmp, rh, windSpd * 1.15, pptAmt,
					&ffmc, &dmc, &dc, &bui, &isi, &fwi, &dsr);

			int per = GetPeriod(wxDay);//wxSet->m_ObsDate);

			if(per >= 0 && per < numPeriods)
			{
				double val = 0.0;
				for(int j = 0; j < MAXVARIDS; j++)
				{
					if(periods[j])
					{
						goodRecord = true;
						switch(j + 1)
						{
						case 1:
							if(!wxSet->IsFieldNull(&wxSet->m_Temp))
								val = (double)wxSet->m_Temp;
							else
								goodRecord = false;
							break;
						case 2:
							if(!wxSet->IsFieldNull(&wxSet->m_TmpMax) && !wxSet->IsFieldNull(&wxSet->m_TmpMin))
								val = (double)(wxSet->m_TmpMax + wxSet->m_TmpMin) / 2.0;
							else
								goodRecord = false;
							break;
						case 3:
							if(!wxSet->IsFieldNull(&wxSet->m_TmpMin))
								val = (double)wxSet->m_TmpMin;
							else
								goodRecord = false;
							break;
						case 4:
							if(!wxSet->IsFieldNull(&wxSet->m_TmpMax))
								val = (double)wxSet->m_TmpMax;
							else
								goodRecord = false;
							break;
						case 5:
							if(!wxSet->IsFieldNull(&wxSet->m_RH))
								val = (double)max(wxSet->m_RH, 1);
							else
								goodRecord = false;
							break;
						case 6:
							if(!wxSet->IsFieldNull(&wxSet->m_RHMax) && !wxSet->IsFieldNull(&wxSet->m_RHMin))
								val = (double)(wxSet->m_RHMax + max(wxSet->m_RHMin, 1)) / 2.0;
							else
								goodRecord = false;
							break;
						case 7:
							if(!wxSet->IsFieldNull(&wxSet->m_RHMin))
								val = (double)max(wxSet->m_RHMin, 1);
							else
								goodRecord = false;
							break;
						case 8:
							if(!wxSet->IsFieldNull(&wxSet->m_RHMax))
								val = (double)wxSet->m_RHMax;
							else
								goodRecord = false;
							break;
						case 9:
							if(!wxSet->IsFieldNull(&wxSet->m_PPTAMT))
								val = (double)wxSet->m_PPTAMT;
							else
								goodRecord = false;
							break;
						case 10:
							if(!wxSet->IsFieldNull(&wxSet->m_PPTDUR))
								val = (double)wxSet->m_PPTDUR;
							else
								goodRecord = false;
							break;
						case 11:
							if(!wxSet->IsFieldNull(&wxSet->m_WS))// && !wxSet->IsFieldNull(&wxSet->m_WDir))
								val = (double)wxSet->m_WS;
							else
								goodRecord = false;
							break;
						case 12:
							val = fSC;
							break;
						case 13:
							val = fERC;
							break;
						case 14:
							val = fBI;
							break;
						case 15:
							val = (double)iKBDI;
							break;
						case 16: //Ignition component
							val = fIC;
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
							if (!wxSet->IsFieldNull(&wxSet->m_SnowFlag))
								val = (double)wxSet->m_SnowFlag;
							else
								goodRecord = false;
							break;
						case 24:
							val = windDir;
							break;
						case 25:
							val = sow;
							//val = iFIL;
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
							if(!wxSet->IsFieldNull(&wxSet->m_GustDir))
								val = (double)wxSet->m_GustDir;
							else
								goodRecord = false;
							break;
						case 34:
							if(!wxSet->IsFieldNull(&wxSet->m_GustSpd))
								val = (double)wxSet->m_GustSpd;
							else
								goodRecord = false;
							break;
						case 35:
							if(!wxSet->IsFieldNull(&wxSet->m_SolarRadiation))
								val = (double)wxSet->m_SolarRadiation;
							else
								goodRecord = false;
							break;
						case 36:
							if(!wxSet->IsFieldNull(&wxSet->m_WetFlag))
								val = (double)wxSet->m_WetFlag;
							else
								goodRecord = false;
							break;
						case 37://Dew Point
							if(!wxSet->IsFieldNull(&wxSet->m_RH) && !wxSet->IsFieldNull(&wxSet->m_Temp))
								val = CalcDPT(wxSet->m_Temp, max(wxSet->m_RH, 1));
							else
								goodRecord = false;
							break;
						case 38://Vapor pressure deficit MAX
							if(!wxSet->IsFieldNull(&wxSet->m_RHMin) && !wxSet->IsFieldNull(&wxSet->m_TmpMax))
								val = CalcVPD(max(wxSet->m_RHMin, 1), wxSet->m_TmpMax);
							else
								goodRecord = false;
							break;
						case 39://Vapor pressure deficit Avg
							if(!wxSet->IsFieldNull(&wxSet->m_Temp) && !wxSet->IsFieldNull(&wxSet->m_RH) && !wxSet->IsFieldNull(&wxSet->m_TmpMin) && !wxSet->IsFieldNull(&wxSet->m_TmpMax))
							{
								double dpt = CalcDPT(wxSet->m_Temp, max(wxSet->m_RH, 1));
								double avgT = (wxSet->m_TmpMin + wxSet->m_TmpMax) / 2;
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
							if(!wxSet->IsFieldNull(&wxSet->m_WAzimuth))
								val = (double)wxSet->m_WAzimuth;
							else
								goodRecord = false;
							break;
						case 42://Hourly Precip
							if(!wxSet->IsFieldNull(&wxSet->m_HourlyPrecip))
								val = (double)wxSet->m_HourlyPrecip;
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
							if(!wxSet->IsFieldNull(&wxSet->m_Temp) && !wxSet->IsFieldNull(&wxSet->m_RH)
									&& !wxSet->IsFieldNull(&wxSet->m_WS))
								val = theApp.m_NFDRS.iCalcFFWI(wxSet->m_Temp, max(wxSet->m_RH, 1), wxSet->m_WS);
							else
								goodRecord = false;
							break;
						default:
							goodRecord = false;
						}
						if(goodRecord && !m_usedExtremes)
						{
								periods[j][per]->Accumulate(wxDay, val);
								//periods[j][per]->Accumulate(wxSet->m_ObsDate, val);
						}
						else if(goodRecord)
						{
							if(m_extremeIsMax[j])
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
			}
			count++;
			init = 0;
			wxSet->MoveNext();//GetNext();
			sprintf(progText, "Records Processed: %ld", count);
			pCUPDUPData->SetProgress(progText);

		}
		//if (extremeDay.GetDayOfYear() != wxDay.GetDayOfYear() || extremeDay.GetYear() != wxDay.GetYear())
		if(m_usedExtremes && extremeDay.GetDayOfYear() == wxDay.GetDayOfYear()
			&& extremeDay.GetYear() == wxDay.GetYear())
		{
			for (int j = 0; j < MAXVARIDS; j++)
			{
				if (periods[j] && dailyExtremes[j] != NODATA)
				{
					int per = GetPeriod(extremeDay);//wxSet->m_ObsDate);
					periods[j][per]->Accumulate(extremeDay, dailyExtremes[j]);
				}
			}
		}
	}
	staSet.Close();
	//nfdrs2016Set.Close();
	if (pCUPDUPData->ShouldTerminate())
	{
		return -1;
	}
	
	//process user variables
	if(HasUserVars())
		userCount += AccumulateStationUserVars(stationStr);
	for(int j = 0; j < MAXVARIDS; j++)
	{
		if(periods[j])
		{
			for(int i = 0; i < numPeriods; i++)
				periods[j][i]->Complete();
		}
	}
	if(useNFDRS)
	{
		CString sfName;
		if (!isNFDRS2016(staSet.m_NFDRSFM[0]))
		{
			sfName.Format("%s\\%6.6s.nfd", theApp.dbDir, stationStr);
			theApp.m_NFDRS.SaveState(sfName);
		}
	}
	if(useCanadian)
		canadian.SaveState(&canadaState);
	return ret;
}

int CClimAnalysis::AnalyzeSIG(const CString query, bool isFPA /* = false*/)
{
	if (pCUPDUPData->ShouldTerminate())
	{
		return -1;
	}
	char sig[32];
	int ret = 0;
	CString temp, staQuery;
	CSIGStationSet staSet(pDB);
	staSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
	staSet.Open();
	strcpy_s(sig, fpSet->m_SIG_Station);
	CStationInSIGSet sSet(pDB);
	temp.Format("[SIG] = '%-20.20s'", &sig[6]);
	sSet.m_strFilter = _T(temp);
	sSet.m_strSort = _T("[WeightFactor]");
	sSet.Open();
	if(sSet.IsBOF() && sSet.IsEOF())
	{
		sSet.Close();
		CString strMsg;
		strMsg.Format(
			_T("ERROR: No stations in SIG.\n Redefine Working Set Query or SIG. \n\n"));
		AfxMessageBox(strMsg);
		return 3;
	}
	//reset members to temporary storage
	Period ***tPeriods = periods;
	int tNumPeriods = numPeriods;
	COleDateTime *tSDates = sDates, *tEDates = eDates;

	//reconfigure numPeriods and sDates and eDates
	int pLen = 1;
	COleDateTimeSpan span( 0, 0, 0, 0 );
	COleDateTime d1, d2, d3;
	numPeriods = 0;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d2 = d1 + span;
	//ensure don't cross month boundary
	while(d1.GetMonth() != d2.GetMonth())
		d2 -= 1;
	while(1)
	{
		//new period coming into loop
		numPeriods++;
		if(MonthChtoInt(fpSet->m_EndMonth) == d1.GetMonth()
			&& fpSet->m_EndDay >= d1.GetDay()
			&& fpSet->m_EndDay <= d2.GetDay())
			break;
		d3 = d2 + span;
		d3 += 1;
		if(d2.GetMonth() != d3.GetMonth())//does next period cross a month boundary?
		{
			if(d3.GetDay() <= pLen / 2)//will create a new period, otherwise will combine
				numPeriods++;
			if(MonthChtoInt(fpSet->m_EndMonth) == d1.GetMonth()
				&& fpSet->m_EndDay >= d1.GetDay()
				&& fpSet->m_EndDay <= daysInMonth[d1.GetMonth() - 1])
				break;
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d2 = d1 + span;
		}
		else
		{
			d1 = d2;
			d1 += 1;
			d2 = d1 + span;
		}
		while(d1.GetMonth() != d2.GetMonth())
			d2 -= 1;
	}
	//ok, got new numPeriods. Allocate new date arrays and fill values by repeating loop
	int sYear = fpSet->m_StartYear, eYear = fpSet->m_EndYear;
	//if(inverted)
	//{//need to know how many years
		CString wxQuery = "";
		CWxSet tSet(fpSet->m_pDatabase);
		tSet.m_strSort = "[ObsDate]";
		tSet.Open();
		if(fpSet->m_SIG_Station.GetLength() > 6)//SIG
		{
			while(!sSet.IsEOF())
			{
				staSet.m_strFilter.Format("[StationID] = '%6.6s'", sSet.m_StationID);
				staSet.Requery();
				// updated 09/2012 with the new DailyObs field in WxObs table
				if (isNFDRS2016(staSet.m_NFDRSFM[0])) // new NFDRS2016 calc needs hourly data
					tSet.m_strFilter.Format("([StationID] = '%6.6s')", sSet.m_StationID);
				else
					tSet.m_strFilter.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", sSet.m_StationID);
				tSet.Requery();
				if(!tSet.IsBOF() && !tSet.IsEOF())
				{
					sYear = min(sYear, tSet.m_ObsDate.GetYear());
					tSet.MoveLast();
					eYear = max(eYear, tSet.m_ObsDate.GetYear());
				}
				sSet.MoveNext();
			}
			if(HasUserVars())
			{
				CString tmpStr = "", strSQL;
				sSet.MoveFirst();
				strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%s'", sSet.m_StationID);
				sSet.MoveNext();
				while(!sSet.IsEOF())
				{
					tmpStr.Format(" or [StationID] = '%6.6s'", sSet.m_StationID);
					strSQL += tmpStr;
					sSet.MoveNext();
				}
				CRecordset userVals(pDB);
				userVals.m_strSort = _T("[ObsDate]");
				userVals.Open(CRecordset::dynaset, strSQL);
				if(!userVals.IsBOF() && !userVals.IsEOF())
				{
					//COleDateTime dt;
					CDBVariant var;
					userVals.GetFieldValue("ObsDate", var);
					//dt = var.date;
					sYear = min(sYear, var.m_pdate->year);
					userVals.MoveLast();
					userVals.GetFieldValue("ObsDate", var);
					//dt = var.date;
					eYear = max(eYear, var.m_pdate->year);
				}
				userVals.Close();
			}
			sSet.MoveFirst();
		}
		else //single station
		{
			tSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
			tSet.Requery();
			if(!tSet.IsBOF() && !tSet.IsEOF())
			{
				sYear = min(sYear, tSet.m_ObsDate.GetYear());
				tSet.MoveLast();
				eYear = max(eYear, tSet.m_ObsDate.GetYear());
			}
			if(HasUserVars())
			{
				CRecordset userVals(pDB);
				CString strSQL;
				strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%s'", fpSet->m_SIG_Station);
				userVals.m_strSort = _T("[ObsDate]");
				userVals.Open(CRecordset::dynaset, strSQL);
				if(!userVals.IsBOF() && !userVals.IsEOF())
				{
					//COleDateTime dt;
					CDBVariant var;
					userVals.GetFieldValue("ObsDate", var);
					//dt = var.date;
					sYear = min(sYear, var.m_pdate->year);
					userVals.MoveLast();
					userVals.GetFieldValue("ObsDate", var);
					//dt = var.date;
					eYear = max(eYear, var.m_pdate->year);
				}
				userVals.Close();
			}
		}
		tSet.Close();
	//}
	sDates = new COleDateTime[numPeriods];
	eDates = new COleDateTime[numPeriods];
	int dLoc = 0;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d2 = d1 + span;
	//safety
	while(d1.GetMonth() != d2.GetMonth())
		d2 -= 1;
	while(1)
	{
		//new period coming into loop
		sDates[dLoc].SetDate(sYear, d1.GetMonth(), d1.GetDay());
		eDates[dLoc++].SetDate(eYear, d1.GetMonth(), d2.GetDay());
		if(dLoc >= numPeriods)
			break;
		d3 = d2 + span;
		d3 += 1;
		if(d2.GetMonth() != d3.GetMonth()) //will next period cross a month boundary???
		{
			if(d3.GetDay() <= pLen / 2)
			{
				//extra group to fill
				sDates[dLoc].SetDate(sYear, d1.GetMonth(), d2.GetDay() + 1);
				eDates[dLoc++].SetDate(eYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
				if(dLoc >= numPeriods)
					break;
			}
			else
			{//combine into previous group
				eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
			}
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d2 = d1 + span;
		}
		else
		{//won't cross boundary, increment normally
			d1 = d2;
			d1 += 1;
			d2 = d1 + span;
		}
		//ensure same month
		while(d1.GetMonth() != d2.GetMonth())
			d2 -= 1;
	}

	//figure number of stations
	int stations = 0;
	while(!sSet.IsEOF())
	{
		stations++;
		sSet.MoveNext();
	}
	//create station data storage
	double *weights = new double[stations];
	Period ****SIGPeriods = new Period***[stations];
	for(int i = 0; i < stations; i++)
	{
		SIGPeriods[i] = new Period **[MAXVARIDS];
		for(int p = 0; p < MAXVARIDS; p++)
			SIGPeriods[i][p] = NULL;
	}
	for(int v = 0; v < nVarIDs; v++)
	{
		//create data storage structure
		COleDateTime m1, m2;
		for(int p = 0; p < stations; p++)
		{
			SIGPeriods[p][varIDs[v]] = new Period*[numPeriods];
			for(int i = 0; i < numPeriods; i++)
				SIGPeriods[p][varIDs[v]][i] = new Period(sDates[i], eDates[i]);
		}
		if(OptionFromVarID(varIDs[v]) == 1)
			useNFDRS = true;
		if(OptionFromVarID(varIDs[v]) == 3)
			useCanadian = true;
		if (varIDs[v] == 39 || varIDs[v] == 42 || varIDs[v] == 43)
			useLFI = true;
	}
	//ok, data structures complete
	//process individual stations to get daily values
	sSet.MoveFirst();
	int sLoc = 0;
	while (!sSet.IsEOF() && !pCUPDUPData->ShouldTerminate())// && (!recCounter || !recCounter->Abort()))
	{
		try
		{
			weights[sLoc] = sSet.m_WeightFactor;
			periods = SIGPeriods[sLoc];
			staQuery.Format("[StationID] = '%6.6s'", sSet.m_StationID);
			staSet.m_strFilter = _T(staQuery);
			staSet.Requery();
			if (m_strModel.GetLength() <= 0)
				m_strModel = GetModelString(&staSet);
			CString tmp = query;

			CString temp2;
			bool reCalc2016 = false;
			reCalc2016 = theApp.m_ForceNFDRS2016Recompute;
			COleDateTime n2Start, n2End;
			fpSet->GetNFDRS2016Range(staSet.m_StationID, &n2Start, &n2End);
			if (!reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0])) // we need hourly data for new calculator?
			{
				CWxSet tWxSet(pDB);
				tWxSet.m_strFilter.Format("[StationID] = '%6.6s' AND [DailyObs] = 1 AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
					n2Start.Format(), n2End.Format());
				tWxSet.Open();
				if (tWxSet.IsFieldNull(&tWxSet.m_FM1) || tWxSet.IsFieldNull(&tWxSet.m_FM10) || tWxSet.IsFieldNull(&tWxSet.m_FM100) || tWxSet.IsFieldNull(&tWxSet.m_FM1000) || tWxSet.IsFieldNull(&tWxSet.m_FuelTemperature)
					|| tWxSet.m_FM1 <= 0.0 || tWxSet.m_FM10 <= 0.0 || tWxSet.m_FM100 <= 0.0 || tWxSet.m_FM1000 <= 0.0 || tWxSet.m_FuelTemperature <= -999.0)
					reCalc2016 = true;
				if (!reCalc2016)
				{
					tWxSet.MoveLast();
					if (tWxSet.IsFieldNull(&tWxSet.m_FM1) || tWxSet.IsFieldNull(&tWxSet.m_FM10) || tWxSet.IsFieldNull(&tWxSet.m_FM100) || tWxSet.IsFieldNull(&tWxSet.m_FM1000) || tWxSet.IsFieldNull(&tWxSet.m_FuelTemperature)
						|| tWxSet.m_FM1 <= 0.0 || tWxSet.m_FM10 <= 0.0 || tWxSet.m_FM100 <= 0.0 || tWxSet.m_FM1000 <= 0.0 || tWxSet.m_FuelTemperature <= -999.0)
						reCalc2016 = true;
				}
				tWxSet.Close();
			}
			if (!useNFDRS)
				reCalc2016 = false;
			if (!reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0]) && !m_usedExtremes)
				temp2.Format("[StationID] = '%6.6s' AND [DailyObs] = 1 AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
					n2Start.Format(), n2End.Format());
			else if ((reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0])) || (m_usedExtremes && isNFDRS2016(staSet.m_NFDRSFM[0])))
			//else if (reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0]))
				temp2.Format("[StationID] = '%6.6s' AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
					n2Start.Format(), n2End.Format());
			else
				temp2.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", staSet.m_StationID);
			tmp += temp2;
			if (useNFDRS && isNFDRS2016(staSet.m_NFDRSFM[0]))
				tmp += m_strRequiredFields;
			wxSet = new CWxSet(pDB);
			wxSet->m_strSort = _T("[ObsDate]");
			wxSet->m_strFilter = _T(tmp);

			wxSet->Open();
			if ((!wxSet->IsBOF() && !wxSet->IsEOF()) && (!staSet.IsBOF() && !staSet.IsEOF()))
				ret = AnalyzeStation(CString(sSet.m_StationID + " - " + staSet.m_Name), isFPA, fpSet->m_SIG_Station);
			wxSet->Close();
			delete wxSet;
			wxSet = NULL;
			sSet.MoveNext();
			sLoc++;
		}
		catch (CDBException* e)
		{
			DisplayDBException(e);
			e->Delete();
			return -4;
		}
	}
	if (pCUPDUPData->ShouldTerminate())
	{
		return -1;
	}
	//ok, combine data into one periods structure....
	periods = tPeriods;
	for(int i = 0; i < numPeriods; i++)
	{
		for(int y = 0; y < eYear - sYear + 1; y++)
		{
			for(int v = 0; v < MAXVARIDS; v++)
			{
				if(periods[v])
				{
					double wSum = 0.0, value = 0.0;
					for(int p = 0; p < stations; p++)
					{
						if(SIGPeriods[p][v][i]->yStats[y].N() > 0)
						{
							value += weights[p] * SIGPeriods[p][v][i]->yStats[y].Mean();
							wSum += weights[p];
						}
					}
					if(wSum > 0.0) //has data
					{
						//get correct period
						d1.SetDate(sYear + y, sDates[i].GetMonth(), sDates[i].GetDay());
						int p;
						for(p = 0; p < tNumPeriods; p++)
						{
							if(d1.GetMonth() == tSDates[p].GetMonth()
								&& d1.GetDay() >= tSDates[p].GetDay()
								&& d1.GetDay() <= tEDates[p].GetDay())
								break;
						}
						if(p >= tNumPeriods)
							p = tNumPeriods - 1;
						periods[v][p]->Accumulate(d1, value / wSum);
					}
				}
			}
		}
	}
	//clean up
	for(int p = 0; p < stations; p++)
	{
		for(int j = 0; j < MAXVARIDS; j++)
		{
			if(SIGPeriods[p][j])
			{
				for(int i = 0; i < numPeriods; i++)
					delete SIGPeriods[p][j][i];
				delete[] SIGPeriods[p][j];
			}
		}
		delete[] SIGPeriods[p];
	}
	delete[] SIGPeriods;
	delete[] weights;
	delete[] sDates;
	delete[] eDates;
	sDates = tSDates;
	eDates = tEDates;
	numPeriods = tNumPeriods;

	if(ret == 0)
	{
		//complete statistics calculation
		for(int j = 0; j < MAXVARIDS; j++)
		{
			if(periods[j])
				for(int i = 0; i < numPeriods; i++)
					periods[j][i]->Complete();
		}
	}
	sSet.Close();
	staSet.Close();
	return ret;
}

CString CClimAnalysis::GetModelString(CSIGStationSet *stn)
{
	CString ret;
	CString staStr = fpSet->m_SIG_Station;
	staStr.TrimRight();
	if (!isNFDRS2016(stn->m_NFDRSFM[0]))
	{
		if (staStr.GetLength() > 6)
			ret.Format("%d%s", stn->m_Use88 ? 8 : 7, stn->m_NFDRSFM);
		else
			ret.Format("%d%s%d%c%c%d", stn->m_Use88 ? 8 : 7, stn->m_NFDRSFM, stn->m_SlopeCls,
				stn->m_HerbAnnual ? 'A' : 'P', stn->m_Deciduous ? 'D' : 'E', stn->m_ClimateCls);
	}
	else//NFDRS2016
	{
		if (staStr.GetLength() > 6)
			ret.Format("%s", stn->m_NFDRSFM);
		else
			ret.Format("%s%d%c%d", stn->m_NFDRSFM, stn->m_SlopeCls, stn->m_HerbAnnual ? 'A' : 'P', stn->m_ClimateCls);
	}
	return ret;
}

//#define ypl 15

CString CClimAnalysis::DataCount(int loc, bool createDoc /*= true*/)
{
	int pLen = atoi(fpSet->m_PeriodLength);
	bool isSIG = strlen(fpSet->m_SIG_Station) > 6 ? true : false;
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");

	//output header

	// added for batch
	// (code interspersed)

	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(stream, "FireFamily Plus Data Count Report\n");

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);

	int Delimiter = reportSet.m_Delimiter;
	if(Delimiter == 2)//an override at Larry's request (from comma to space)
		Delimiter = 0;

	fprintf(stream, "\nVariable: %s\n", climSet->m_Variable_Name);
	if (customInits)
		fprintf(stream,"(Calculated using at least one custom yearly init)\n");

	reportSet.Close();
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
	// end added for batch

	int y = fpSet->m_EndYear - fpSet->m_StartYear + 1;
	if (y < 1)
		y = 1;

	long *ycnt = new long[numPeriods];
	long *ysum = new long[numPeriods];

	// new totals FFP 4.1  09/2012
	long *yearSum = new long[y];

	for(int k = 0; k < y; k++)
		yearSum[k] = 0;

	long totSum = 0;

	int i;
	for(i = 0; i < numPeriods; i++)
		ycnt[i] = ysum[i] = 0;
		fprintf(stream, "Period           Year\n");
		for(int j = 0; j < y; j++)
				fprintf(stream, "      ");
		fprintf(stream, "                  Total #          Total #\n");
		fprintf(stream, "Begins         ");
		int j;
		for(int j = 0; j < y; j++)
				fprintf(stream, "%6ld", fpSet->m_StartYear + j);
		fprintf(stream, "    Years         Observations\n");

		fprintf(stream, "---------------");
		for(int j = 0; j < y; j++)
			//if(i * ypl + j < y)  //output year header
				fprintf(stream, "------");
		fprintf(stream, "------------------------------------\n");

		//do data
		for(int j = 0; j < numPeriods; j++)
		{
			if (Delimiter > 0)
				fprintf(stream, "%02d/%02d%s", periods[loc][j]->start.GetMonth(), periods[loc][j]->start.GetDay(),delim);
			else
				fprintf(stream, " %02d/%02d         ",
			periods[loc][j]->start.GetMonth(), periods[loc][j]->start.GetDay());
			for(int k = 0; k < y; k++)
			{
					long n = periods[loc][j]->yStats[k].N();
					if(n > 0)
					{
						fprintf(stream, "%6ld", n);

						yearSum[k] += n;
						totSum += n;
						ycnt[j]++;
						ysum[j] += n;
						if (Delimiter > 0)
							fprintf(stream, "%s",delim);
					}
					else
					{
						if (Delimiter > 0)
							fprintf(stream, "%s",delim);
						else
							fprintf(stream, "      ");
					}
				//}
			}
			if (Delimiter > 0)
				fprintf(stream,
					" %s%6ld%s%8ld\n",
					delim, ycnt[j], delim, ysum[j]);
			else
				fprintf(stream,
					"   %6ld         %8ld\n",
					ycnt[j], ysum[j]);

		}
		fprintf(stream, "\n");

    fprintf(stream, " Total         ");
	for(int k = 0; k < y; k++)
	{
		fprintf(stream, "%6ld", yearSum[k]);
		if (Delimiter > 0)
			fprintf(stream, "%s",delim);
	}
	if (Delimiter > 0)
		fprintf(stream,
					" %s      %s%8ld\n",
					delim, delim, totSum);
	else
		fprintf(stream,
					"                  %8ld\n",
					totSum);

	//now output totals
	long recsUsed = 0;
	for(i = 0; i < numPeriods; i++)
	{

		recsUsed += ysum[i];
	}

	fprintf(stream, "\n");
	if(isSIG)
		AddSIGDescription(stream);
	fprintf(stream, "\n%ld weather observations used. %ld records processed.\n%s",
		recsUsed, (loc >= STATICVARIDS) ? userCount : count, AddDateStamp());
	fclose(stream);
	delete[] ycnt;
	delete[] ysum;
	if(yearSum)
		delete[] yearSum;
	CString retName = fName;
	if(autosave)
	{
		CString saveName;
		saveName.Format(autosavePath + climSet->m_ShortName + "DataCount.txt");
		unlink(saveName);
		rename(fName, saveName);
		free(fName);
		return saveName;
	}
	if(createDoc)
	{
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
				tTitle.Format("%s - %s", fpSet->m_SIG_Station, climSet->m_Variable_Name);
				tTitle += " - Data Count";
				pDoc->SetTitle(tTitle);
				return retName;
			}
		}
	}
	free(fName);
	return retName;
}

int CClimAnalysis::GenerateClimatologyReports(CClimateSet *_climSet)
{
	//Now create selected reports for display
	CPeriodDoc *pdDoc = NULL;
	int tnumPeriods = 0;
	Period ***tperiods = NULL;
	CFireDay **tFires = NULL;
	bool useAux = false;
	climSet = _climSet;
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
	/*if (fpSet->m_Use78 && useAux)//auxilliary years
	{
		CClimAnalysis tAnalysis(pDB);
		int *tVarIDs = new int[nVarIDs];
		for(int v = 0; v < nVarIDs; v++)
			tVarIDs[v] = varIDs[v];
		//tAnalysis.pCUPDUPData = pCUPDUPData;
		if(tAnalysis.RunAnalyze(tVarIDs, nVarIDs, fpSet, true) == IDOK)
		{
			tAnalysis.killPeriods = false;
			tnumPeriods = tAnalysis.numPeriods;
			tperiods = tAnalysis.periods;
			tFires = tAnalysis.fires;
		}
		else
		{
			killPeriods = true;
			return 0;
		}
	}*/
	climSet->MoveFirst();
	while(!climSet->IsEOF())
	{
		//generate selected outputs
		if(climSet->m_Stats_Table)
		{
			if(climSet->m_VarID == 9)
				PrecipTable(climSet->m_VarID - 1, climSet->m_CriticalPercentile / 100.0);
			else
				StatsTable(climSet->m_VarID - 1, climSet->m_CriticalPercentile / 100.0);
		}
		if(climSet->m_Stats_Graph)
		{
			//pdDoc = AddGraphView(pdDoc, climSet->m_VarID - 1, climSet->m_OptionType, tnumPeriods, tperiods, tFires);
			pdDoc = AddGraphView(pdDoc, climSet->m_VarID - 1, climSet->m_OptionType, m_opts.numtPeriods, m_opts.tPeriods, m_opts.tFires);
		}
		if(climSet->m_Data_Count)
			DataCount(climSet->m_VarID - 1);
		if(climSet->m_Daily_Freqs)
		{
			if(climSet->m_VarID == 9)
				PrecipFrequencyTable(climSet->m_VarID - 1);
			else
				FrequencyTable(climSet->m_VarID - 1);
		}

		climSet->MoveNext();
	}
	if(pdDoc)
	{
		killPeriods = false;
		return 1;
	}
	return 0;
}

short OptionFromVarID(int varID)
{
	switch (varID)//these varID's are climSet.VarID - 1
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 22:
	case 23:
	case 24:
		return 0;//weather var
		break;
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 39:
	case 42:
	case 43:
		return 1;//NFDRS
		break;
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
		return 3;//canadian
		break;
	}
	return 0;
}

void CClimAnalysis::ProbabilityAnalysis(CSeasonBinSet *bin1, CSeasonBinSet *bin2, int varLoc1, int varLoc2)
{
	//need a two dimensional array of counts....
	double val1, val2;
	int a, b;
	long **bins = new long*[7];
	int i;
	for(i = 0; i < 7; i++)
	{
		bins[i] = new long[7];
		for(int j = 0; j < 7; j++)
			bins[i][j] = 0;
	}
	//always the first 2 varIDs
	int loc1 = varIDs[varLoc1], loc2 = varIDs[varLoc2];
	for(i = 0; i < periods[loc1][0]->years; i++)
	{
		for(int p = 0; p < numPeriods; p++)
		{
			if(periods[loc1][p]->yStats[i].N() > 0 && periods[loc2][p]->yStats[i].N() > 0 )
			{
				val1 = periods[loc1][p]->yStats[i].Mean();
				val2 = periods[loc2][p]->yStats[i].Mean();
				a = bin1->GetBinIndex(val1);
				b = bin2->GetBinIndex(val2);
				if(a >= 0 && b >= 0)
				{
					bins[a][b]++;//increment cell
					bins[a][6]++;//increment row
					bins[6][b]++;//increment column
					bins[6][6]++;//increment total
				}
			}
		}
	}

	//have matrix of bin counts.
	//output in a report
	CString varName1, varName2;
	CClimateSet tClim(fpSet->m_pDatabase);
	tClim.m_strFilter.Format("[VarID] = %d", loc1 + 1);
	tClim.Open();
	varName1 = tClim.m_Variable_Name;
	int pLen = atoi(fpSet->m_PeriodLength);
	bool isSIG = strlen(fpSet->m_SIG_Station) > 6 ? true : false,
		usesNFDRS = false;
	if(tClim.m_OptionType == 1)
		usesNFDRS = true;
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	//output header

// added for batch
	// (code interspersed)
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(stream, "FireFamily Plus Probability Analysis Report\n");

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch

	fprintf(stream, "    %s vs ", varName1);
	tClim.m_strFilter.Format("[VarID] = %d", loc2 + 1);
	tClim.Requery();
	if(tClim.m_OptionType == 1)
		usesNFDRS = true;
	varName2 = tClim.m_Variable_Name;
	fprintf(stream, "%s\n", varName2);
	fprintf(stream, "\n");

	fprintf(stream, "                                            %s\n", varName2);
	fprintf(stream, "------------------------------------------------------------------------------------------------\n");
	fprintf(stream, "                             |  %6.2f  |  %6.2f  |  %6.2f  |  %6.2f  |  %6.2f  |  %6.2f  |\n",
		bin2->m_Bin1Min,
		bin2->m_Bin2Min,
		bin2->m_Bin3Min,
		bin2->m_Bin4Min,
		bin2->m_Bin5Min,
		bin2->m_Bin6Min);
	fprintf(stream, "%-26.26s   |     -    |     -    |     -    |     -    |     -    |     -    |\n",
		varName1);
	fprintf(stream, "                             |  %6.2f  |  %6.2f  |  %6.2f  |  %6.2f  |  %6.2f  |  %6.2f  |\n",
		bin2->m_Bin1Max,
		bin2->m_Bin2Max,
		bin2->m_Bin3Max,
		bin2->m_Bin4Max,
		bin2->m_Bin5Max,
		bin2->m_Bin6Max);
	//safety for division
	for(int r = 0; r < 6; r++)
		if(bins[r][6] == 0)
			bins[r][6] = 1;
	//total report safety
	if(bins[6][6] == 0)
		fprintf(stream, "  Error: No consecutive pairs found.\n\n");
	else
	{
	fprintf(stream, "------------------------------------------------------------------------------------------------\n");
		//now output the statistics
		fprintf(stream, "%7.2f - %7.2f            |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |\n",
			bin1->m_Bin1Min, bin1->m_Bin1Max,
			bins[0][0], bins[0][1], bins[0][2], bins[0][3], bins[0][4], bins[0][5]);
		fprintf(stream, "%7.2f - %7.2f            |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |\n",
			bin1->m_Bin2Min, bin1->m_Bin2Max,
			bins[1][0], bins[1][1], bins[1][2], bins[1][3], bins[1][4], bins[1][5]);
		fprintf(stream, "%7.2f - %7.2f            |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |\n",
			bin1->m_Bin3Min, bin1->m_Bin3Max,
			bins[2][0], bins[2][1], bins[2][2], bins[2][3], bins[2][4], bins[2][5]);
		fprintf(stream, "%7.2f - %7.2f            |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |\n",
			bin1->m_Bin4Min, bin1->m_Bin4Max,
			bins[3][0], bins[3][1], bins[3][2], bins[3][3], bins[3][4], bins[3][5]);
		fprintf(stream, "%7.2f - %7.2f            |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |\n",
			bin1->m_Bin5Min, bin1->m_Bin5Max,
			bins[4][0], bins[4][1], bins[4][2], bins[4][3], bins[4][4], bins[4][5]);
		fprintf(stream, "%7.2f - %7.2f            |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |\n",
			bin1->m_Bin6Min, bin1->m_Bin6Max,
			bins[5][0], bins[5][1], bins[5][2], bins[5][3], bins[5][4], bins[5][5]);
		fprintf(stream, "------------------------------------------------------------------------------------------------\n");
		fprintf(stream, "                    Conditional Probability Table (%%)\n");
		fprintf(stream, "------------------------------------------------------------------------------------------------\n");
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			bin1->m_Bin1Min, bin1->m_Bin1Max,
			((double)bins[0][0]) / ((double)bins[0][6]) * 100.0,
			((double)bins[0][1]) / ((double)bins[0][6]) * 100.0,
			((double)bins[0][2]) / ((double)bins[0][6]) * 100.0,
			((double)bins[0][3]) / ((double)bins[0][6]) * 100.0,
			((double)bins[0][4]) / ((double)bins[0][6]) * 100.0,
			((double)bins[0][5]) / ((double)bins[0][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			bin1->m_Bin2Min, bin1->m_Bin2Max,
			((double)bins[1][0]) / ((double)bins[1][6]) * 100.0,
			((double)bins[1][1]) / ((double)bins[1][6]) * 100.0,
			((double)bins[1][2]) / ((double)bins[1][6]) * 100.0,
			((double)bins[1][3]) / ((double)bins[1][6]) * 100.0,
			((double)bins[1][4]) / ((double)bins[1][6]) * 100.0,
			((double)bins[1][5]) / ((double)bins[1][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			bin1->m_Bin3Min, bin1->m_Bin3Max,
			((double)bins[2][0]) / ((double)bins[2][6]) * 100.0,
			((double)bins[2][1]) / ((double)bins[2][6]) * 100.0,
			((double)bins[2][2]) / ((double)bins[2][6]) * 100.0,
			((double)bins[2][3]) / ((double)bins[2][6]) * 100.0,
			((double)bins[2][4]) / ((double)bins[2][6]) * 100.0,
			((double)bins[2][5]) / ((double)bins[2][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			bin1->m_Bin4Min, bin1->m_Bin4Max,
			((double)bins[3][0]) / ((double)bins[3][6]) * 100.0,
			((double)bins[3][1]) / ((double)bins[3][6]) * 100.0,
			((double)bins[3][2]) / ((double)bins[3][6]) * 100.0,
			((double)bins[3][3]) / ((double)bins[3][6]) * 100.0,
			((double)bins[3][4]) / ((double)bins[3][6]) * 100.0,
			((double)bins[3][5]) / ((double)bins[3][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			bin1->m_Bin5Min, bin1->m_Bin5Max,
			((double)bins[4][0]) / ((double)bins[4][6]) * 100.0,
			((double)bins[4][1]) / ((double)bins[4][6]) * 100.0,
			((double)bins[4][2]) / ((double)bins[4][6]) * 100.0,
			((double)bins[4][3]) / ((double)bins[4][6]) * 100.0,
			((double)bins[4][4]) / ((double)bins[4][6]) * 100.0,
			((double)bins[4][5]) / ((double)bins[4][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			bin1->m_Bin6Min, bin1->m_Bin6Max,
			((double)bins[5][0]) / ((double)bins[5][6]) * 100.0,
			((double)bins[5][1]) / ((double)bins[5][6]) * 100.0,
			((double)bins[5][2]) / ((double)bins[5][6]) * 100.0,
			((double)bins[5][3]) / ((double)bins[5][6]) * 100.0,
			((double)bins[5][4]) / ((double)bins[5][6]) * 100.0,
			((double)bins[5][5]) / ((double)bins[5][6]) * 100.0);
		fprintf(stream, "------------------------------------------------------------------------------------------------\n");
		fprintf(stream, "                         Joint Probability Table (%%)\n");
		fprintf(stream, "------------------------------------------------------------------------------------------------\n");
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			bin1->m_Bin1Min, bin1->m_Bin1Max,
			((double)bins[0][0]) / ((double)bins[6][6]) * 100.0,
			((double)bins[0][1]) / ((double)bins[6][6]) * 100.0,
			((double)bins[0][2]) / ((double)bins[6][6]) * 100.0,
			((double)bins[0][3]) / ((double)bins[6][6]) * 100.0,
			((double)bins[0][4]) / ((double)bins[6][6]) * 100.0,
			((double)bins[0][5]) / ((double)bins[6][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			bin1->m_Bin2Min, bin1->m_Bin2Max,
			((double)bins[1][0]) / ((double)bins[6][6]) * 100.0,
			((double)bins[1][1]) / ((double)bins[6][6]) * 100.0,
			((double)bins[1][2]) / ((double)bins[6][6]) * 100.0,
			((double)bins[1][3]) / ((double)bins[6][6]) * 100.0,
			((double)bins[1][4]) / ((double)bins[6][6]) * 100.0,
			((double)bins[1][5]) / ((double)bins[6][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			bin1->m_Bin3Min, bin1->m_Bin3Max,
			((double)bins[2][0]) / ((double)bins[6][6]) * 100.0,
			((double)bins[2][1]) / ((double)bins[6][6]) * 100.0,
			((double)bins[2][2]) / ((double)bins[6][6]) * 100.0,
			((double)bins[2][3]) / ((double)bins[6][6]) * 100.0,
			((double)bins[2][4]) / ((double)bins[6][6]) * 100.0,
			((double)bins[2][5]) / ((double)bins[6][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			bin1->m_Bin4Min, bin1->m_Bin4Max,
			((double)bins[3][0]) / ((double)bins[6][6]) * 100.0,
			((double)bins[3][1]) / ((double)bins[6][6]) * 100.0,
			((double)bins[3][2]) / ((double)bins[6][6]) * 100.0,
			((double)bins[3][3]) / ((double)bins[6][6]) * 100.0,
			((double)bins[3][4]) / ((double)bins[6][6]) * 100.0,
			((double)bins[3][5]) / ((double)bins[6][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			bin1->m_Bin5Min, bin1->m_Bin5Max,
			((double)bins[4][0]) / ((double)bins[6][6]) * 100.0,
			((double)bins[4][1]) / ((double)bins[6][6]) * 100.0,
			((double)bins[4][2]) / ((double)bins[6][6]) * 100.0,
			((double)bins[4][3]) / ((double)bins[6][6]) * 100.0,
			((double)bins[4][4]) / ((double)bins[6][6]) * 100.0,
			((double)bins[4][5]) / ((double)bins[6][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			bin1->m_Bin6Min, bin1->m_Bin6Max,
			((double)bins[5][0]) / ((double)bins[6][6]) * 100.0,
			((double)bins[5][1]) / ((double)bins[6][6]) * 100.0,
			((double)bins[5][2]) / ((double)bins[6][6]) * 100.0,
			((double)bins[5][3]) / ((double)bins[6][6]) * 100.0,
			((double)bins[5][4]) / ((double)bins[6][6]) * 100.0,
			((double)bins[5][5]) / ((double)bins[6][6]) * 100.0);
		fprintf(stream, "------------------------------------------------------------------------------------------------\n");
	}
	fprintf(stream, "\n%ld data pairs used. %ld records processed.\n%s", bins[6][6], count, AddDateStamp());
	if(isSIG)
		AddSIGDescription(stream);
	fclose(stream);
	if(autosave)
	{
		CString saveName = autosavePath + "ProbabilityAnalysis.txt";
		unlink(saveName);
		rename(fName, saveName);
	}
	else
	{
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
				CString tTitle;
				tTitle.Format("%s - %s", fpSet->m_SIG_Station, tClim.m_Variable_Name);
				tTitle += " - Probability Analysis";
				pDoc->SetTitle(tTitle);
				break;
			}
		}
	}
	//clean up
	tClim.Close();
	for(i = 0; i < 7; i++)
		delete[] bins[i];
	delete[] bins;
	free(fName);
}

void CClimAnalysis::PersistenceProbability(CSeasonBinSet * binSet)
{
	//need a two dimensional array of counts....
	double today, tomorrow;
	int a, b;
	bool consecutive = false;
	long **bins = new long*[7];
	int i;
	for(i = 0; i < 7; i++)
	{
		bins[i] = new long[7];
		for(int j = 0; j < 7; j++)
			bins[i][j] = 0;
	}
	//always the first (&better be only!)varID
	int loc = varIDs[0];
	long total = 0;
	for(i = 0; i < periods[loc][0]->years; i++)
	{
		for(int p = 0; p < numPeriods; p++)
		{
			if(periods[loc][p]->yStats[i].N() > 0)
			{
				tomorrow = periods[loc][p]->yStats[i].Mean();
				if(consecutive)
				{
					b = binSet->GetBinIndex(tomorrow);
					a = binSet->GetBinIndex(today);
					if(a >= 0 && b >= 0)
					{
						bins[a][b]++;//increment cell
						bins[a][6]++;//increment row
						bins[6][b]++;//increment column
						bins[6][6]++;//increment total
					}
				}
				consecutive = true;
				today = tomorrow;
				if(p == numPeriods - 1)//last period??
				{
					//see if can span from year to year
					int c = DayOfYear(periods[loc][p]->end),
						d = DayOfYear(periods[loc][p]->start);
					if((d == c + 1) || (c >= 365 && d == 1))
						consecutive = true;
				}
			}
			else
				consecutive = false;
		}
	}

	//have matrix of bin counts.
	//output in a report
	CClimateSet tClim(fpSet->m_pDatabase);
	tClim.m_strFilter.Format("[VarID] = %d", loc + 1);
	tClim.Open();
	int pLen = atoi(fpSet->m_PeriodLength);
	bool isSIG = strlen(fpSet->m_SIG_Station) > 6 ? true : false;
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	//output header

	// added for batch
	// (code interspersed)
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(stream, "FireFamily Plus Persistence Probability Report\n");

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch

	CString extremeString = " ";
	if (m_usedExtremes)
	{
		if (m_extremeIsMax[loc])
			extremeString = " - Daily Max";
		else
			extremeString = " - Daily Min";
	}
	fprintf(stream, "    Variable: %s%s\n", tClim.m_Variable_Name, extremeString);
	if (customInits)
		fprintf(stream,"(Calculated using at least one custom yearly init)\n");
	fprintf(stream, "\n");

	fprintf(stream, "                                 Tomorrow's %s\n", tClim.m_Variable_Name);
	fprintf(stream, "------------------------------------------------------------------------------------------------\n");
	fprintf(stream, "                             |  %6.2f  |  %6.2f  |  %6.2f  |  %6.2f  |  %6.2f  |  %6.2f  |\n",
		binSet->m_Bin1Min,
		binSet->m_Bin2Min,
		binSet->m_Bin3Min,
		binSet->m_Bin4Min,
		binSet->m_Bin5Min,
		binSet->m_Bin6Min);
	fprintf(stream, "   Today's                   |     -    |     -    |     -    |     -    |     -    |     -    |\n");
	fprintf(stream, "                             |  %6.2f  |  %6.2f  |  %6.2f  |  %6.2f  |  %6.2f  |  %6.2f  |\n",
		binSet->m_Bin1Max,
		binSet->m_Bin2Max,
		binSet->m_Bin3Max,
		binSet->m_Bin4Max,
		binSet->m_Bin5Max,
		binSet->m_Bin6Max);
	//safety for division
	for(int r = 0; r < 6; r++)
		if(bins[r][6] == 0)
			bins[r][6] = 1;
	//total report safety
	if(bins[6][6] == 0)
		fprintf(stream, "  Error: No consecutive pairs found.\n\n");
	else
	{
	fprintf(stream, "------------------------------------------------------------------------------------------------\n");
		//now output the statistics
		fprintf(stream, "%7.2f - %7.2f            |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |\n",
			binSet->m_Bin1Min, binSet->m_Bin1Max,
			bins[0][0], bins[0][1], bins[0][2], bins[0][3], bins[0][4], bins[0][5]);
		fprintf(stream, "%7.2f - %7.2f            |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |\n",
			binSet->m_Bin2Min, binSet->m_Bin2Max,
			bins[1][0], bins[1][1], bins[1][2], bins[1][3], bins[1][4], bins[1][5]);
		fprintf(stream, "%7.2f - %7.2f            |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |\n",
			binSet->m_Bin3Min, binSet->m_Bin3Max,
			bins[2][0], bins[2][1], bins[2][2], bins[2][3], bins[2][4], bins[2][5]);
		fprintf(stream, "%7.2f - %7.2f            |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |\n",
			binSet->m_Bin4Min, binSet->m_Bin4Max,
			bins[3][0], bins[3][1], bins[3][2], bins[3][3], bins[3][4], bins[3][5]);
		fprintf(stream, "%7.2f - %7.2f            |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |\n",
			binSet->m_Bin5Min, binSet->m_Bin5Max,
			bins[4][0], bins[4][1], bins[4][2], bins[4][3], bins[4][4], bins[4][5]);
		fprintf(stream, "%7.2f - %7.2f            |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |%8ld  |\n",
			binSet->m_Bin6Min, binSet->m_Bin6Max,
			bins[5][0], bins[5][1], bins[5][2], bins[5][3], bins[5][4], bins[5][5]);
		fprintf(stream, "------------------------------------------------------------------------------------------------\n");
		fprintf(stream, "                    Conditional Probability Table (%%)\n");
		fprintf(stream, "------------------------------------------------------------------------------------------------\n");
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			binSet->m_Bin1Min, binSet->m_Bin1Max,
			((double)bins[0][0]) / ((double)bins[0][6]) * 100.0,
			((double)bins[0][1]) / ((double)bins[0][6]) * 100.0,
			((double)bins[0][2]) / ((double)bins[0][6]) * 100.0,
			((double)bins[0][3]) / ((double)bins[0][6]) * 100.0,
			((double)bins[0][4]) / ((double)bins[0][6]) * 100.0,
			((double)bins[0][5]) / ((double)bins[0][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			binSet->m_Bin2Min, binSet->m_Bin2Max,
			((double)bins[1][0]) / ((double)bins[1][6]) * 100.0,
			((double)bins[1][1]) / ((double)bins[1][6]) * 100.0,
			((double)bins[1][2]) / ((double)bins[1][6]) * 100.0,
			((double)bins[1][3]) / ((double)bins[1][6]) * 100.0,
			((double)bins[1][4]) / ((double)bins[1][6]) * 100.0,
			((double)bins[1][5]) / ((double)bins[1][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			binSet->m_Bin3Min, binSet->m_Bin3Max,
			((double)bins[2][0]) / ((double)bins[2][6]) * 100.0,
			((double)bins[2][1]) / ((double)bins[2][6]) * 100.0,
			((double)bins[2][2]) / ((double)bins[2][6]) * 100.0,
			((double)bins[2][3]) / ((double)bins[2][6]) * 100.0,
			((double)bins[2][4]) / ((double)bins[2][6]) * 100.0,
			((double)bins[2][5]) / ((double)bins[2][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			binSet->m_Bin4Min, binSet->m_Bin4Max,
			((double)bins[3][0]) / ((double)bins[3][6]) * 100.0,
			((double)bins[3][1]) / ((double)bins[3][6]) * 100.0,
			((double)bins[3][2]) / ((double)bins[3][6]) * 100.0,
			((double)bins[3][3]) / ((double)bins[3][6]) * 100.0,
			((double)bins[3][4]) / ((double)bins[3][6]) * 100.0,
			((double)bins[3][5]) / ((double)bins[3][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			binSet->m_Bin5Min, binSet->m_Bin5Max,
			((double)bins[4][0]) / ((double)bins[4][6]) * 100.0,
			((double)bins[4][1]) / ((double)bins[4][6]) * 100.0,
			((double)bins[4][2]) / ((double)bins[4][6]) * 100.0,
			((double)bins[4][3]) / ((double)bins[4][6]) * 100.0,
			((double)bins[4][4]) / ((double)bins[4][6]) * 100.0,
			((double)bins[4][5]) / ((double)bins[4][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			binSet->m_Bin6Min, binSet->m_Bin6Max,
			((double)bins[5][0]) / ((double)bins[5][6]) * 100.0,
			((double)bins[5][1]) / ((double)bins[5][6]) * 100.0,
			((double)bins[5][2]) / ((double)bins[5][6]) * 100.0,
			((double)bins[5][3]) / ((double)bins[5][6]) * 100.0,
			((double)bins[5][4]) / ((double)bins[5][6]) * 100.0,
			((double)bins[5][5]) / ((double)bins[5][6]) * 100.0);
		fprintf(stream, "------------------------------------------------------------------------------------------------\n");
		fprintf(stream, "                         Joint Probability Table (%%)\n");
		fprintf(stream, "------------------------------------------------------------------------------------------------\n");
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			binSet->m_Bin1Min, binSet->m_Bin1Max,
			((double)bins[0][0]) / ((double)bins[6][6]) * 100.0,
			((double)bins[0][1]) / ((double)bins[6][6]) * 100.0,
			((double)bins[0][2]) / ((double)bins[6][6]) * 100.0,
			((double)bins[0][3]) / ((double)bins[6][6]) * 100.0,
			((double)bins[0][4]) / ((double)bins[6][6]) * 100.0,
			((double)bins[0][5]) / ((double)bins[6][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			binSet->m_Bin2Min, binSet->m_Bin2Max,
			((double)bins[1][0]) / ((double)bins[6][6]) * 100.0,
			((double)bins[1][1]) / ((double)bins[6][6]) * 100.0,
			((double)bins[1][2]) / ((double)bins[6][6]) * 100.0,
			((double)bins[1][3]) / ((double)bins[6][6]) * 100.0,
			((double)bins[1][4]) / ((double)bins[6][6]) * 100.0,
			((double)bins[1][5]) / ((double)bins[6][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			binSet->m_Bin3Min, binSet->m_Bin3Max,
			((double)bins[2][0]) / ((double)bins[6][6]) * 100.0,
			((double)bins[2][1]) / ((double)bins[6][6]) * 100.0,
			((double)bins[2][2]) / ((double)bins[6][6]) * 100.0,
			((double)bins[2][3]) / ((double)bins[6][6]) * 100.0,
			((double)bins[2][4]) / ((double)bins[6][6]) * 100.0,
			((double)bins[2][5]) / ((double)bins[6][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			binSet->m_Bin4Min, binSet->m_Bin4Max,
			((double)bins[3][0]) / ((double)bins[6][6]) * 100.0,
			((double)bins[3][1]) / ((double)bins[6][6]) * 100.0,
			((double)bins[3][2]) / ((double)bins[6][6]) * 100.0,
			((double)bins[3][3]) / ((double)bins[6][6]) * 100.0,
			((double)bins[3][4]) / ((double)bins[6][6]) * 100.0,
			((double)bins[3][5]) / ((double)bins[6][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			binSet->m_Bin5Min, binSet->m_Bin5Max,
			((double)bins[4][0]) / ((double)bins[6][6]) * 100.0,
			((double)bins[4][1]) / ((double)bins[6][6]) * 100.0,
			((double)bins[4][2]) / ((double)bins[6][6]) * 100.0,
			((double)bins[4][3]) / ((double)bins[6][6]) * 100.0,
			((double)bins[4][4]) / ((double)bins[6][6]) * 100.0,
			((double)bins[4][5]) / ((double)bins[6][6]) * 100.0);
		fprintf(stream, "%7.2f - %7.2f            |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |%8.2f  |\n",
			binSet->m_Bin6Min, binSet->m_Bin6Max,
			((double)bins[5][0]) / ((double)bins[6][6]) * 100.0,
			((double)bins[5][1]) / ((double)bins[6][6]) * 100.0,
			((double)bins[5][2]) / ((double)bins[6][6]) * 100.0,
			((double)bins[5][3]) / ((double)bins[6][6]) * 100.0,
			((double)bins[5][4]) / ((double)bins[6][6]) * 100.0,
			((double)bins[5][5]) / ((double)bins[6][6]) * 100.0);
		fprintf(stream, "------------------------------------------------------------------------------------------------\n");
	}
	fprintf(stream, "\n%ld weather observations used. %ld records processed.\n%s",
		bins[6][6], (loc >= STATICVARIDS) ? userCount : count, AddDateStamp());
	if(isSIG)
		AddSIGDescription(stream);
	fclose(stream);
	if(autosave)
	{
		CString saveName = autosavePath + "PersistenceProbability.txt";
		unlink(saveName);
		rename(fName, saveName);
	}
	else
	{
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
				CString tTitle;
				tTitle.Format("%s - %s", fpSet->m_SIG_Station, tClim.m_Variable_Name);
				tTitle += " - Persistence Probabilities";
				pDoc->SetTitle(tTitle);
				break;
			}
		}
	}
	//clean up
	tClim.Close();
	for(i = 0; i < 7; i++)
		delete[] bins[i];
	delete[] bins;
	free(fName);
}

void CClimAnalysis::SeverityList(double pcntile, bool useLessThan/* = false*/, int varLoc)
{
	//always the first (&better be only!)varID
	int loc = varIDs[varLoc];
	double criticalValue;
	long totalRecs = 0;
	Stats varStats;
	int i;
	for(i = 0; i < periods[loc][0]->years; i++)
	{
		for(int p = 0; p < numPeriods; p++)
		{
			if(periods[loc][p]->yStats[i].N() > 0)
			{
				for(int k = 0; k < periods[loc][p]->yStats[i].N(); k++)
				{
					varStats.Accumulate(periods[loc][p]->yStats[i].vals[k]);
					totalRecs++;
				}
			}
		}
	}
	varStats.Complete();
	criticalValue = varStats.Percentile(pcntile / 100.0);
	//have matrix of bin counts.
	//output in a report
	CClimateSet tClim(fpSet->m_pDatabase);
	tClim.m_strFilter.Format("[VarID] = %d", loc + 1);
	tClim.Open();
	int pLen = atoi(fpSet->m_PeriodLength);
	bool isSIG = strlen(fpSet->m_SIG_Station) > 6 ? true : false;
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	//output header
	// added for batch
	// (code interspersed)
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(stream, "FireFamily Plus Seasonal Severity Report\n");

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch

	int Delim = reportSet.m_Delimiter;
	reportSet.Close();

	CString delim;
	switch (Delim){
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

	CString extremeString = " ";
	if (m_usedExtremes)
	{
		if (m_extremeIsMax[loc])
			extremeString = " - Daily Max";
		else
			extremeString = " - Daily Min";
	}
	fprintf(stream, "Variable: %s%s\n", tClim.m_Variable_Name, extremeString);
	if (customInits)
		fprintf(stream,"(Calculated using at least one custom yearly init)\n");
	fprintf(stream, "\n");

	fprintf(stream, "    %s Values %s %.2fth Percentile Value of %.2f\n\n",
		tClim.m_Variable_Name, useLessThan ? "Less Than" : "Greater Than", pcntile, criticalValue);
	fprintf(stream, "   Period                     Cumulative     Number of Days\n");
	fprintf(stream, "   Begins         Value       Departure	  Exceeded (per Year)\n");

	double cumulative = 0.0;
	int yearDays = 0;
	for(i = 0; i < periods[loc][0]->years; i++)
	{
		fprintf(stream, "-----------------------------------------------------------------\n");
		fprintf(stream, "                             %d\n", periods[loc][0]->baseYear + i);
		fprintf(stream, "-----------------------------------------------------------------\n");
		cumulative = 0.0;
		yearDays = 0;
		for(int p = 0; p < numPeriods; p++)
		{
			if(periods[loc][p]->yStats[i].N() > 0)
			{
				for(int k = 0; k < periods[loc][p]->yStats[i].N(); k++)
				{
					if(periods[loc][p]->yStats[i].vals[k] > criticalValue && !useLessThan
						|| periods[loc][p]->yStats[i].vals[k] < criticalValue && useLessThan)
					{
						cumulative += (periods[loc][p]->yStats[i].vals[k] - criticalValue);
						yearDays++;
						if (Delim > 0)
							fprintf(stream, " %02d/%02d/%d%s%9.2f%s%12.2f%s%d\n",
								periods[loc][p]->start.GetMonth(), periods[loc][p]->start.GetDay(),
								periods[loc][p]->baseYear + i, delim,
								periods[loc][p]->yStats[i].vals[k], delim,
								fabs(cumulative), delim,
								yearDays);
						else
							fprintf(stream, " %02d/%02d/%d   %9.2f     %12.2f           %d\n",
								periods[loc][p]->start.GetMonth(), periods[loc][p]->start.GetDay(),
								periods[loc][p]->baseYear + i,
								periods[loc][p]->yStats[i].vals[k],
								fabs(cumulative),
								yearDays);
					}
				}
			}
		}
	}
	fprintf(stream, "\n%ld weather observations used. %ld records processed.\n%s",
		totalRecs, (loc >= STATICVARIDS) ? userCount : count, AddDateStamp());
	if(isSIG)
		AddSIGDescription(stream);
	fclose(stream);
	if(autosave)
	{
		CString saveName = autosavePath + "Severity List.txt";
		unlink(saveName);
		rename(fName, saveName);
	}
	else
	{
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
				CString tTitle;
				tTitle.Format("%s - %s", fpSet->m_SIG_Station, tClim.m_Variable_Name);
				tTitle += " - Severity List";
				pDoc->SetTitle(tTitle);
				break;
			}
		}
	}
	//clean up
	tClim.Close();
	free(fName);
}

void CClimAnalysis::SeveritySummary(double pcntile, bool useLessThan/* = false*/, int varLoc)
{
	//always the first (&better be only!)varID
	int loc = varIDs[varLoc];
	double criticalValue;
	long totalRecs = 0;
	//need a two dimensional array of Stats....
	int years = periods[loc][0]->years, bYear = periods[loc][0]->baseYear;
	Stats varStats;
	int i;
	for(i = 0; i < periods[loc][0]->years; i++)
	{
		for(int p = 0; p < numPeriods; p++)
		{
			if(periods[loc][p]->yStats[i].N() > 0)
			{
				for(int k = 0; k < periods[loc][p]->yStats[i].N(); k++)
				{
					varStats.Accumulate(periods[loc][p]->yStats[i].vals[k]);
					totalRecs++;
				}
			}
		}
	}
	varStats.Complete();
	criticalValue = varStats.Percentile(pcntile / 100.0);
	Stats **summary;
	summary = new Stats *[years + 1];
	int y;
	for(y = 0; y < years + 1; y++)
		summary[y] = new Stats[13];
	int month;
	for(i = 0; i < years; i++)
	{
		for(int p = 0; p < numPeriods; p++)
		{
			if(periods[loc][p]->yStats[i].N() > 0)
			{
				for(int k = 0; k < periods[loc][p]->yStats[i].N(); k++)
				{
					if(periods[loc][p]->yStats[i].vals[k] > criticalValue && !useLessThan
						|| periods[loc][p]->yStats[i].vals[k] < criticalValue && useLessThan)
					{
						month = periods[loc][p]->start.GetMonth() - 1;
						summary[i][month].Accumulate(fabs(criticalValue - periods[loc][p]->yStats[i].vals[k]));
						summary[years][month].Accumulate(fabs(criticalValue - periods[loc][p]->yStats[i].vals[k]));
						summary[i][12].Accumulate(fabs(criticalValue - periods[loc][p]->yStats[i].vals[k]));
					}
				}
			}
		}
	}
	for(y = 0; y < years + 1; y++)
		for(int m = 0; m < 13; m++)
			summary[y][m].Complete();

	//have matrix of Stats of Severity values in summary
	//output in a report
	CClimateSet tClim(fpSet->m_pDatabase);
	tClim.m_strFilter.Format("[VarID] = %d", loc + 1);
	tClim.Open();
	int pLen = atoi(fpSet->m_PeriodLength);
	bool isSIG = strlen(fpSet->m_SIG_Station) > 6 ? true : false;
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	//output header

	// added for batch
	// (code interspersed)
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(stream, "FireFamily Plus Seasonal Severity Report\n");

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch
	int Delim = reportSet.m_Delimiter;
	reportSet.Close();

	CString delim;
	switch (Delim){
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
	CString extremeString = " ";
	if (m_usedExtremes)
	{
		if (m_extremeIsMax[loc])
			extremeString = " - Daily Max";
		else
			extremeString = " - Daily Min";
	}
	fprintf(stream, "Variable: %s%s\n", tClim.m_Variable_Name, extremeString);
	if (customInits)
		fprintf(stream,"(Calculated using at least one custom yearly init)\n");
	fprintf(stream, "\n");

	fprintf(stream, "    %s Values %s %.2fth Percentile Value of %.2f\n\n",
		tClim.m_Variable_Name, useLessThan ? "Less Than" : "Greater Than", pcntile, criticalValue);
	fprintf(stream, "                                    Departure Values Table\n");
	fprintf(stream, "                      Cumulative Departure Values by Year by Month\n");
	fprintf(stream, "-------------------------------------------------------------------------------------------------------\n");
	fprintf(stream, " Year |     Jan     Feb     Mar     Apr     May     Jun     Jul     Aug     Sep     Oct     Nov     Dec\n");
	fprintf(stream, "-------------------------------------------------------------------------------------------------------\n");

	for(i = 0; i < years; i++)
	{
		fprintf(stream, " %d |", bYear + i);
		if (Delim > 0)
			fprintf(stream,"%s",delim);

		for(int m = 0; m < 12; m++) {
			fprintf(stream, "%8.2f", summary[i][m].Mean() * summary[i][m].N());
			if (Delim > 0)
				fprintf(stream,"%s",delim);
		}
		fprintf(stream, "\n");
	}

	fprintf(stream, "\n\n                                 Days of Departure Table\n");
	fprintf(stream, "                        Number of Departure Days by Year by Month\n");
	fprintf(stream, "-------------------------------------------------------------------------------------------------------\n");
	fprintf(stream, " Year |     Jan     Feb     Mar     Apr     May     Jun     Jul     Aug     Sep     Oct     Nov     Dec\n");
	fprintf(stream, "-------------------------------------------------------------------------------------------------------\n");

	for(i = 0; i < years; i++)
	{
		fprintf(stream, " %d |", bYear + i);
		if (Delim > 0)
			fprintf(stream,"%s",delim);

		for(int m = 0; m < 12; m++){
			fprintf(stream, "%8d", summary[i][m].N());
			if (Delim > 0)
				fprintf(stream,"%s",delim);
		}
		fprintf(stream, "\n");
	}

	fprintf(stream, "\n\n                                    Average Table\n");
	fprintf(stream, "                  Average Cumulative Departure and Departure Days by Month\n");
	fprintf(stream, "-------------------------------------------------------------------------------------------------------\n");
	fprintf(stream, "  Avg |     Jan     Feb     Mar     Apr     May     Jun     Jul     Aug     Sep     Oct     Nov     Dec\n");
	fprintf(stream, "-------------------------------------------------------------------------------------------------------\n");

	fprintf(stream, "Depart|");
	if (Delim > 0)
			fprintf(stream,"%s",delim);
	int m;
	for(m = 0; m < 12; m++){
		fprintf(stream, "%8.2f", summary[years][m].Mean());
		if (Delim > 0)
			fprintf(stream,"%s",delim);
	}
	fprintf(stream, "\n");
	fprintf(stream, " Days |");
	if (Delim > 0)
			fprintf(stream,"%s",delim);
	for(m = 0; m < 12; m++){
		fprintf(stream, "%8d", summary[years][m].N());
		if (Delim > 0)
			fprintf(stream,"%s",delim);
	}
	fprintf(stream, "\n");

	fprintf(stream, "\n\n                                Annual Ratings Table\n");
	fprintf(stream, "                        Totals and Ratings for Departure by Year\n");
	fprintf(stream, "------------------------------------------------------------------------------------\n");
	fprintf(stream, "                      Totals        |             Rating Factors\n");
	fprintf(stream, "------------------------------------------------------------------------------------\n");
	fprintf(stream, " Year |    Departure   |   Days   |   Departure   |   Days   |   Departure x Days                \n");
	fprintf(stream, "------------------------------------------------------------------------------------\n");

	//get key divisors
	double deptAvg = 0.0, daysAvg = 0.0, deptCnt = 0.0, daysCnt = 0.0;
	for(i = 0; i < years; i++)
	{
		deptAvg += summary[i][12].Mean() * summary[i][12].N();
		daysCnt += summary[i][12].N();
	}
	if(daysCnt > 0)
	{
		deptAvg = deptAvg / years;
		daysAvg = daysCnt / years;
		for(i = 0; i < years; i++)
		{
			if (Delim > 0)
				fprintf(stream, " %d |%s%8.2f%s%6ld%s%7.2f%s%6.2f%s%6.2f\n",
					bYear + i, delim,
					summary[i][12].Mean() * summary[i][12].N(), delim,
					summary[i][12].N(), delim,
					(summary[i][12].Mean() * summary[i][12].N()) / deptAvg, delim,
					summary[i][12].N() / daysAvg, delim,
					((summary[i][12].Mean() * summary[i][12].N()) / deptAvg) * summary[i][12].N() / daysAvg);
			else
				fprintf(stream, " %d |    %8.2f    %6ld        %7.2f      %6.2f       %6.2f\n",
					bYear + i,
					summary[i][12].Mean() * summary[i][12].N(),
					summary[i][12].N(),
					(summary[i][12].Mean() * summary[i][12].N()) / deptAvg,
					summary[i][12].N() / daysAvg,
					((summary[i][12].Mean() * summary[i][12].N()) / deptAvg) * summary[i][12].N() / daysAvg);
		}
	}
	else
		fprintf(stream, "\n    Error: No days encountered.\n");

	//fprintf(stream, "\n%ld weather observations used. %ld records processed.\n%s", totalRecs, count, AddDateStamp());
	fprintf(stream, "\n%ld weather observations used. %ld records processed.\n%s",
		totalRecs, (loc >= STATICVARIDS) ? userCount : count, AddDateStamp());
	if(isSIG)
		AddSIGDescription(stream);
	fclose(stream);
	if(autosave)
	{
		CString saveName = autosavePath + "SeveritySummary.txt";
		unlink(saveName);
		rename(fName, saveName);
	}
	else
	{
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
				CString tTitle;
				tTitle.Format("%s - %s", fpSet->m_SIG_Station, tClim.m_Variable_Name);
				tTitle += " - Severity Summary";
				pDoc->SetTitle(tTitle);
				break;
			}
		}
	}
	//clean up
	fclose(stream);
	tClim.Close();
	free(fName);
	for(y = 0; y < years + 1; y++)
		delete[] summary[y];
	delete[] summary;
}

CString CClimAnalysis::AddDateStamp()
{
	COleDateTime dateTime = COleDateTime::GetCurrentTime();
	CString dStr = dateTime.Format("%m/%d/%Y-%H:%M");
	CString ret;
	ret.Format("FF+%s %s %s", verStr, theApp.m_buildDateStrPacked, dStr);
	return ret;
}


CString CClimAnalysis::DailyListing(ListingOptions *listOpts, bool createDoc /*= true*/, bool createTable /*= true */)
{
	int selVars[MAXVARIDS], varLocs[MAXVARIDS], nSelVars = 0;
	for(int i = 0; i < MAXVARIDS; i++)
		selVars[i] = -1;
	CClimateSet selClim(fpSet->m_pDatabase);
	selClim.m_strFilter = "[OptionType] >= 0 AND [Daily List] > 0";
	selClim.m_strSort = "[Daily List]";
	selClim.Open();
	while(!selClim.IsEOF())
	{
		selVars[nSelVars] = selClim.m_VarID - 1;
		nSelVars++;
		selClim.MoveNext();
	}
	selClim.Close();
	//now get there locs in varIDs
	for(int i = 0; i < nSelVars; i++)
	{
		for(int v = 0; v < nVarIDs; v++)
		{
			if(varIDs[v] == selVars[i])
				varLocs[i] = v;
		}
	}
	CString ret = "";
	int pLen = atoi(fpSet->m_PeriodLength);
	bool isSIG = strlen(fpSet->m_SIG_Station) > 6 ? true : false;
	char *fName = GetTempFileName();
	ret = fName;
	FILE *stream = fopen(fName, "wt");
	//output header
	bool hasFires = true;
	CFiresSet fireSet(fpSet->m_pDatabase);
	fireSet.Open();
	if(listOpts->fires || listOpts->largeFires || listOpts->acres || listOpts->numPerClass)
	{
		if(!fireSet.FilterToWorkingSet(fpSet, listOpts->fCause, 0,false))//has fires
		{
			if(createDoc)
				AfxMessageBox("Warning: No fires matching working set for Daily List report");
			hasFires = false;
		}
	}
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
		fprintf(stream, "FireFamily Plus Daily Listing Report\n");
		fprintf(stream, "Daily Listing of Selected Values\n");
		if(listOpts->dateStamp)
			fprintf(stream, "\n%s\n", AddDateStamp());

		// added for batch
		// (code interspersed)
		CString dStr;

		COleDateTime dateTime = COleDateTime::GetCurrentTime();

		dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
		fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
		fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

		fprintf(stream,"\n");
		CString headerStr;

		// add report headers (as needed)
		reportSet.listAll(&headerStr,fpSet);
		fprintf(stream,"%s",headerStr);
		// end added for batch

		if(listOpts->fires || listOpts->largeFires)
		{//output fire cause in header
			CString causeStr = "All Fires";
			if(listOpts->fCause == 1)
				causeStr = "Lightning";
			else if(listOpts->fCause > 1)
				causeStr = "Human";
			fprintf(stream, "Fire Cause Filter: %s\n", causeStr);
			if(listOpts->largeFires)
				fprintf(stream, "Large Fire: %d Acres\n", listOpts->lfAcres);
		}
		fprintf(stream, "\n");
	}
	CString *userfmts = NULL, *userAbbrevs = NULL;
	int uVars = HasUserVars();
	if(uVars)
	{
		userfmts = new CString[uVars];
		userAbbrevs = new CString[uVars];
		CUserVarSet uvs(pDB);
		uvs.Open();
		int loc = 0;
		for(int v = 0; v < nSelVars; v++)
		{
			if(varIDs[varLocs[v]] >= STATICVARIDS)
			{
				uvs.m_strFilter.Format("[VarID] = %d", varIDs[v] + 1);
				uvs.Requery();
				userfmts[loc].Format("5.%df", uvs.m_Decimals);
				CString ab = uvs.m_Abbrev;
				ab.TrimLeft();
				ab.TrimRight();
				userAbbrevs[loc].Format("%5.5s", ab);
				loc++;
			}
		}
		uvs.Close();
	}
	//header
	if(listOpts->columnHeader)
	{
		if(listOpts->stnPerRecord)
		{
			if(fpSet->m_SIG_Station.GetLength() > 6)
				fprintf(stream, "SIG Name            %s", delim);
			else
				fprintf(stream, "Station %s", delim);
		}

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
		if(listOpts->tt == FullTime)
			fprintf(stream, "Time %s", delim);
		else if(listOpts->tt == PackedTime)
			fprintf(stream, "Time%s", delim);
		int loc = 0;
		int i;
		for(i = 0; i < nSelVars; i++)
		{
			if(varIDs[varLocs[i]] < STATICVARIDS)
				fprintf(stream, "%5.5s%s", abbrevs[varIDs[varLocs[i]]], delim);
			else
			{
				fprintf(stream, "%5.5s%s", userAbbrevs[loc], delim);
				loc++;
			}
		}
		if(listOpts->fires)
			fprintf(stream, "#Fires  %s", delim);
		if(listOpts->largeFires)
			fprintf(stream, "#LgFires%s", delim);
		if(listOpts->numPerClass)
			fprintf(stream, "    #A%s    #B%s    #C%s    #D%s    #E%s    #F%s    #G%s", delim, delim, delim, delim, delim, delim, delim);
		if(listOpts->acres)
			fprintf(stream, "   #Acres%s", delim);
		fprintf(stream, "\n");
		if(listOpts->stnPerRecord)
		{
			if(fpSet->m_SIG_Station.GetLength() > 6)
				fprintf(stream, "--------------------");
			else
				fprintf(stream, "-------");
		}
		fprintf(stream, "------------");
		if(listOpts->tt != NoTime)
			fprintf(stream, "------");
		for(i = 0; i < nSelVars; i++)
			fprintf(stream, "------");
		if(listOpts->fires)
			fprintf(stream, "-------");
		if(listOpts->largeFires)
			fprintf(stream, "---------");
		if(listOpts->numPerClass)
			fprintf(stream, "--------------------------------------------------------");
		if(listOpts->acres)
			fprintf(stream, "----------");
		fprintf(stream, "\n");
	}
	reportSet.Close();

	CString tableMemo("");

	// new DL_Export table,  2013/01
	if (createTable)
	{
		char tempStr[250];

         // drop table if it exists
		if (HasDLOutput())
		{

			try
			{
				CString strSql = "DROP TABLE [DL_Output]";
				pDB->ExecuteSQL(strSql);
			}
			catch(CDBException* e)
			{
				e->Delete();
			}

		}
		CreateDLOutput();
		
		try
			{
				CString strSql = "ALTER TABLE [DL_Output] ADD [StationID] VARCHAR(20)";
				pDB->ExecuteSQL(strSql);
			}
			catch(CDBException* e)
			{
				e->Delete();
			}
		

		// add additional fields as necessary
		int loc = 0;
		int i;
		CString strSql("");
		for(i = 0; i < nSelVars; i++)
		{
			
			char fieldName[20];
			if(varIDs[varLocs[i]] < STATICVARIDS)
			{
				sprintf(fieldName, "%5.5s", abbrevs[varIDs[varLocs[i]]]);
			} else
			{
				sprintf(fieldName, "%5.5s", userAbbrevs[loc]);
				loc++;
			}
			try
			{
				CString trimmed = CString(fieldName).Trim();
				strSql = "ALTER TABLE [DL_Output] ADD [" + trimmed + "] DOUBLE";
				pDB->ExecuteSQL(strSql);
			}
			catch(CDBException* e)
			{
				e->Delete();
			}
		}
		if(listOpts->fires)
                try
				{
					strSql = "ALTER TABLE [DL_Output] ADD [numFires] INTEGER";
					pDB->ExecuteSQL(strSql);
				}
				catch(CDBException* e)
				{
					e->Delete();
				}
		if(listOpts->largeFires)
                try
				{
					strSql = "ALTER TABLE [DL_Output] ADD [numLargeFires] INTEGER";
					pDB->ExecuteSQL(strSql);
				}
				catch(CDBException* e)
				{
					e->Delete();
				}
			
		 if(listOpts->numPerClass)
				try
				{
					strSql = "ALTER TABLE [DL_Output] ADD [numA] INTEGER";
					pDB->ExecuteSQL(strSql);
					strSql = "ALTER TABLE [DL_Output] ADD [numB] INTEGER";
					pDB->ExecuteSQL(strSql);
					strSql = "ALTER TABLE [DL_Output] ADD [numC] INTEGER";
					pDB->ExecuteSQL(strSql);
					strSql = "ALTER TABLE [DL_Output] ADD [numD] INTEGER";
					pDB->ExecuteSQL(strSql);
					strSql = "ALTER TABLE [DL_Output] ADD [numE] INTEGER";
					pDB->ExecuteSQL(strSql);
					strSql = "ALTER TABLE [DL_Output] ADD [numF] INTEGER";
					pDB->ExecuteSQL(strSql);
					strSql = "ALTER TABLE [DL_Output] ADD [numG] INTEGER";
					pDB->ExecuteSQL(strSql);
				}
				catch(CDBException* e)
				{
					e->Delete();
				}
		if(listOpts->acres)
                try
				{
					strSql = "ALTER TABLE [DL_Output] ADD [numAcres] REAL";
					pDB->ExecuteSQL(strSql);
				}
				catch(CDBException* e)
				{
					e->Delete();
				}

        // memo field, populated only for the first record
	    try
				{
					strSql = "ALTER TABLE [DL_Output] ADD [memo] LONGTEXT";
					pDB->ExecuteSQL(strSql);
				}
				catch(CDBException* e)
				{
					e->Delete();
				}

	
		tableMemo = "FireFamily Plus Daily Listing Report\n";
		tableMemo += "Daily Listing of Selected Values\n";	
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
		sprintf(tempStr,"%d-Day Observations\n\n", pLen);
		tableMemo += tempStr;
	
		


	}

	//now dump data....
	bool dumpRec = false;

	// 2013 - keep track of number of inserted records

    int numRecs = 0;
	for(int y = 0; y < periods[varIDs[0]][0]->years; y++)
	{
		//for each year...
		for(int i = 0; i < numPeriods; i++)
		{
			char dateChar[30];
			char timeChar[30];
			CString varStr,
					valStr;

			int iVals = 0;

			for(int v = 0; v < nSelVars; v++)
			{
				if(periods[varIDs[varLocs[v]]][i]->yStats[y].N() > 0)
				{
					dumpRec = true;
					break;
				}
			}
			if(dumpRec)
			{
				numRecs ++;

				
				varStr += ",[StationID]";
				valStr += ",'"+CString(fpSet->m_SIG_Station).Trim()+"'";
				iVals ++;
			    if(listOpts->stnPerRecord)
			    {
					if(fpSet->m_SIG_Station.GetLength() > 6)
						fprintf(stream, "%-20s%s", fpSet->m_SIG_Station, delim);
					else
						fprintf(stream, "  %6s%s", fpSet->m_SIG_Station, delim);
				}
				switch(listOpts->dt)
				{
				case Full:

					sprintf(dateChar, "%02d/%02d/%4d",
						periods[varIDs[0]][i]->start.GetMonth(), periods[varIDs[0]][i]->start.GetDay(),
						periods[varIDs[0]][i]->baseYear + y);
					fprintf(stream,"%s%s",dateChar,delim);
					break;
				case Packed:
						sprintf(dateChar, "%02d%02d%4d",
							periods[varIDs[0]][i]->start.GetMonth(), periods[varIDs[0]][i]->start.GetDay(),
							periods[varIDs[0]][i]->baseYear + y);
						fprintf(stream,"%s%s",dateChar,delim);
					break;
				case Reverse:
						sprintf(dateChar, "%4d%02d%02d",
							periods[varIDs[0]][i]->baseYear + y, periods[varIDs[0]][i]->start.GetMonth(), periods[varIDs[0]][i]->start.GetDay());
						fprintf(stream,"%s%s",dateChar,delim);

					break;
				case MD:
						sprintf(dateChar, "%02d/%02d",
							periods[varIDs[0]][i]->start.GetMonth(), periods[varIDs[0]][i]->start.GetDay());
						fprintf(stream,"%s%s",dateChar,delim);
					break;
				case MDPacked:
						sprintf(dateChar, "%02d%02d",
							periods[varIDs[0]][i]->start.GetMonth(), periods[varIDs[0]][i]->start.GetDay());
						fprintf(stream,"%s%s",dateChar,delim);

					break;
				default:
					sprintf(dateChar, "%02d/%02d/%4d",
						periods[varIDs[0]][i]->start.GetMonth(), periods[varIDs[0]][i]->start.GetDay(),
						periods[varIDs[0]][i]->baseYear + y);
					break;
				}
				switch(listOpts->tt)
				{
				case FullTime:
						sprintf(timeChar, "%02d:%02d",
							periods[varIDs[0]][i]->start.GetHour(), periods[varIDs[0]][i]->start.GetMinute());
						fprintf(stream,"%s%s",timeChar,delim);

					break;
				case PackedTime:
						sprintf(timeChar, "%02d%02d",
							periods[varIDs[0]][i]->start.GetHour(), periods[varIDs[0]][i]->start.GetMinute());
						fprintf(stream,"%s%s",timeChar,delim);

					break;
				case NoTime:
				default:
					sprintf(timeChar," ");
					break;
				}

				
			

				int uLoc = 0;
				char varVal[40];
				for(int l = 0; l < nSelVars; l++)
				{
					char frmt[16];
					
					if(varIDs[varLocs[l]] < STATICVARIDS && periods[varIDs[varLocs[l]]][i]->yStats[y].N() > 0)
					{	
						
						sprintf(frmt, "%%%s ", varFormats[varIDs[varLocs[l]]]);
						sprintf(varVal,frmt,periods[varIDs[varLocs[l]]][i]->yStats[y].Mean());

				        if (Delimiter > 0)
						   fprintf(stream, "%s%s",varVal,delim );
						else
						   fprintf(stream,"%s",varVal);
						
					}
					else if(varIDs[varLocs[l]] >= STATICVARIDS)
					{
						if(periods[varIDs[varLocs[l]]][i]->yStats[y].N() > 0)
						{
							
							sprintf(frmt, "%%%s%s", userfmts[uLoc],delim);
							sprintf(varVal,frmt,periods[varIDs[varLocs[l]]][i]->yStats[y].Mean());
							
							if (Delimiter > 0)
						       fprintf(stream, "%s%s",varVal,delim );
						    else
						       fprintf(stream,"%s",varVal);
						}
						else{ //leave blank(7 blanks)
							if (Delimiter > 0)
								fprintf(stream, "%s", delim);
							else
								fprintf(stream, "      ");
							sprintf(varVal,"");
						}
						uLoc++;
					}
					else {//leave blank(7 blanks)
						if (Delimiter > 0)
							fprintf(stream, "%s", delim);
						else
							fprintf(stream, "      ");
						sprintf(varVal,"");
					}
					// insert variable into DL_Output table if necessary
					if (createTable)
					   if (strlen(varVal) > 0)
					   {
						  
							char fieldName[20];
							if(varIDs[varLocs[l]] < STATICVARIDS)
								sprintf(fieldName, "%5.5s", abbrevs[varIDs[varLocs[l]]]);
							else
								sprintf(fieldName, "%5.5s", userAbbrevs[uLoc]);
			
							
							varStr += ",[" + CString(fieldName).Trim() + "]";
							valStr += "," + CString(varVal).Trim();
							iVals ++;

							
					   }
				}
				//do fires & large fires if selected...
				if(listOpts->fires || listOpts->largeFires || listOpts->acres || listOpts->numPerClass)
				{
					int nFires = 0, nLarge = 0, numA = 0, numB = 0, numC = 0, numD = 0, numE = 0, numF = 0, numG = 0;
					double nAcres = 0.0;
					if(hasFires)
					{
						int d = DayOfYear(periods[varIDs[0]][i]->start);
						while(!fireSet.IsEOF()
							&& fireSet.m_Discovery.GetYear() < periods[varIDs[0]][i]->baseYear + y)
							fireSet.MoveNext();
						while(!fireSet.IsEOF()
							&& fireSet.m_Discovery.GetYear() <= periods[varIDs[0]][i]->baseYear + y
							&& DayOfYear(fireSet.m_Discovery) < d)
							fireSet.MoveNext();
						if(!fireSet.IsEOF()
							&& fireSet.m_Discovery.GetYear() == periods[varIDs[0]][i]->baseYear + y
							&& DayOfYear(fireSet.m_Discovery) == d)
						{
							while(!fireSet.IsEOF()
								&& fireSet.m_Discovery.GetYear() == periods[varIDs[0]][i]->baseYear + y
								&& DayOfYear(fireSet.m_Discovery) == d)
							{
								switch(fireSet.m_StatisticalCause)
								{
								case 1://lightning
									if(listOpts->fCause <= 1)
									{
										nFires++;
										if(listOpts->largeFires && fireSet.m_TotalAcres >= listOpts->lfAcres)
											nLarge++;
										if(fireSet.m_TotalAcres <= 0.25)
											numA++;
										else if(fireSet.m_TotalAcres < 10.0)
											numB++;
										else if(fireSet.m_TotalAcres < 100.0)
											numC++;
										else if(fireSet.m_TotalAcres < 300.0)
											numD++;
										else if(fireSet.m_TotalAcres < 1000.0)
											numE++;
										else if(fireSet.m_TotalAcres < 5000.0)
											numF++;
										else
											numG++;
										nAcres += fireSet.m_TotalAcres;
									}
									break;
								default://human
									if(listOpts->fCause != 1)
									{
										nFires++;
										if(listOpts->largeFires && fireSet.m_TotalAcres >= listOpts->lfAcres)
											nLarge++;
										if(fireSet.m_TotalAcres <= 0.25)
											numA++;
										else if(fireSet.m_TotalAcres < 10.0)
											numB++;
										else if(fireSet.m_TotalAcres < 100.0)
											numC++;
										else if(fireSet.m_TotalAcres < 300.0)
											numD++;
										else if(fireSet.m_TotalAcres < 1000.0)
											numE++;
										else if(fireSet.m_TotalAcres < 5000.0)
											numF++;
										else
											numG++;
										nAcres += fireSet.m_TotalAcres;
									}
								}
								fireSet.MoveNext();
							}
						}
					}
					if(listOpts->fires)
					{
						fprintf(stream, "%6d%s", nFires, delim);

						if (createTable)  
						{
							
							varStr += "," + CString("[numFires]");
							sprintf(varVal,"%d",nFires);
							valStr += "," +  CString(varVal).Trim();
							iVals ++;

						}

					}
					if(listOpts->largeFires)
					{
						fprintf(stream, "%9d%s", nLarge, delim);
						if (createTable)  
						{
							
							varStr += "," + CString("[numLargeFires]");
							sprintf(varVal,"%d",nLarge);
							valStr += "," + CString(varVal).Trim();
							iVals ++;

						}
					}
					if(listOpts->numPerClass)
					{
						fprintf(stream, "%6d%s%6d%s%6d%s%6d%s%6d%s%6d%s%6d%s", numA, delim, numB, delim, numC, delim,
						numD, delim, numE, delim, numF, delim, numG, delim);
						if (createTable)
						{
						  
							varStr += "," + CString("[numA],[numB],[numC],[numD],[numE],[numF],[numG]");
							sprintf(varVal,"%d,%d,%d,%d,%d,%d,%d",numA,numB,numC,numD,numE,numF,numG);
							valStr += "," + CString(varVal).Trim();
							iVals ++;
						}
					}
					if(listOpts->acres)
					{
						fprintf(stream, "%10.1f%s", nAcres, delim);
						if (createTable)  
						{
							
							varStr += "," + CString("[numAcres]");
							sprintf(varVal,"%10.1f",nAcres);
							valStr += "," + CString(varVal).Trim();
							iVals ++;

						}
					}
				
				}
				fprintf(stream, "\n");
					// 2013
					// add record to optional DL_Output table
					CString dateStr;
					CString idStr;
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
							strSql = "INSERT INTO [DL_Output] ([ID],[ObsDate]" + varStr + ") VALUES (" + idStr.Trim() + ",'" + dateStr.Trim() + "'" + valStr + ")";
							
							pDB->ExecuteSQL(strSql);
						}
						catch(CDBException* e)
						{
							e->Delete();
						}
					}
			}
			dumpRec = false;
		}
	}
	fireSet.Close();
	if(userfmts)
		delete[] userfmts;
	if(userAbbrevs)
		delete[] userAbbrevs;
	if(isSIG && listOpts->reportHeader)
		AddSIGDescription(stream);
	if(stream)
		fclose(stream);
	if(autosave)
	{
		CString saveName = autosavePath + "DailyListing.txt";
		unlink(saveName);
		rename(fName, saveName);
		free(fName);
		return saveName;
	}
	//now display the file in a child window
	if(createDoc)
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
				CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(fName);
				pDoc->isTemp = true;
				free(fName);
				CString tTitle;
				if (createTable)
				   tTitle.Format("%s - Daily Listing - Output exported to table 'DL_Output'", fpSet->m_SIG_Station);
				else
				   tTitle.Format("%s - Daily Listing", fpSet->m_SIG_Station);
				pDoc->SetTitle(tTitle);
				return ret;
			}
		}
	}
	free(fName);
	return ret;
}
void CClimAnalysis::GetLogitArraySizesQuick(CFireOptionsSet *fireOptsSet, int vID,
		long *nV, long *nFD, long *nLFD, long *nMFD, long *_discards, long *madeUps)
{
	long _nV = 0, _nFD = 0, _nLFD = 0, _nMFD = 0, discards = 0, madeCnt = 0;
	chdir(theApp.dbDir);
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
	bool round = true;
	if(varIDs[0] == 8)
		round = false;
	bool haveWx, madeWx, first;
	double lastWx = 0.0, nextWx, wxVal;
	int lastDay, nextDay, d;
	//first gotta count number of records, accounting for made up values...
	long arraySize = 0;
	for(int y = 0; y < periods[varIDs[vID]][0]->years; y++)
	{
		//for each year...
		first = true;
		d = 0;//DayOfYear(periods[varIDs[vID]][0]->start) - 1;
		for(int i = 0; i < numPeriods; i++)
		{
			haveWx = madeWx = false;
			if(periods[varIDs[vID]][i]->yStats[y].N() > 0)
			{//have weather....
				haveWx = true;
				lastWx = wxVal = periods[varIDs[vID]][i]->yStats[y].Mean();
				if(round)
					lastWx = wxVal = Round(lastWx);
				lastDay = DayOfYear(periods[varIDs[vID]][i]->start);
				first = false;
			}
			if(!haveWx && !first)
			{
				nextDay = 0;
				for(int p = i + 1; p < numPeriods && p < i + theApp.maxMissingWx + 1; p++)
				{
					if(periods[varIDs[vID]][p]->yStats[y].N() > 0)
					{
						nextDay = DayOfYear(periods[varIDs[vID]][p]->start);
						nextWx = periods[varIDs[vID]][p]->yStats[y].Mean();
						if(round)
							nextWx = Round(nextWx);
						break;
					}
				}
				if((nextDay > 0) && (nextDay - lastDay - 1)<= theApp.maxMissingWx)
				{
					if(varIDs[0] == 8 || varIDs[0] == 9)//precip amount or duration always zero
						wxVal = 0.0;
					else if(varIDs[0] == 10 || varIDs[0] == 23 || varIDs[0] == 24)
						wxVal = lastWx;   //wind dir, wind speed, SOW persistent
					else
					{//interpolate
						wxVal = Round(lastWx + ((nextWx - lastWx) / (nextDay - lastDay))
							* (DayOfYear(periods[varIDs[vID]][i]->start) - lastDay));
					}
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
void CClimAnalysis::CreateLogitArraysQuick(CFireOptionsSet *fireOptsSet, int vID,
		double *fxVals, double *fyVals, double *lfxVals, double *lfyVals, double *mfxVals, double *mfyVals)
{
	int isFD, isLFD, isMFD, f;//, vID;
	//vID = fireOptsSet->m_VarID - 1;
	bool round = true;
	if(varIDs[0] == 8)
		round = false;
	bool haveWx, madeWx, first;
	double lastWx = 0.0, nextWx, wxVal;
	int lastDay, nextDay;
	long madeCnt = 0;
	int d;
	//ok, fill arrays then fill them with same algorithm
	long loc = 0, fdLoc = 0;
	for(int y = 0; y < periods[varIDs[vID]][0]->years; y++)
	{
		//for each year...
		first = true;
		d = 0;
		for(int i = 0; i < numPeriods; i++)
		{
			haveWx = madeWx = false;
			if(periods[varIDs[vID]][i]->yStats[y].N() > 0)
			{//have weather....
				haveWx = true;
				lastWx = wxVal = periods[varIDs[vID]][i]->yStats[y].Mean();
				if(round)
					lastWx = wxVal = Round(lastWx);
				lastDay = DayOfYear(periods[varIDs[vID]][i]->start);
				first = false;
			}
			if(!haveWx && !first)
			{
				nextDay = 0;
				for(int p = i + 1; p < numPeriods && p < i + theApp.maxMissingWx + 1; p++)
				{
					if(periods[varIDs[vID]][p]->yStats[y].N() > 0)
					{
						nextDay = DayOfYear(periods[varIDs[vID]][p]->start);
						nextWx = periods[varIDs[vID]][p]->yStats[y].Mean();
						if(round)
							nextWx = Round(nextWx);
						break;
					}
				}
				if((nextDay > 0) && (nextDay - lastDay - 1)<= theApp.maxMissingWx)
				{
					if(varIDs[0] == 8 || varIDs[0] == 9)//precip amount or duration always zero
						wxVal = 0.0;
					else if(varIDs[0] == 10 || varIDs[0] == 23 || varIDs[0] == 24)
						wxVal = lastWx;   //wind dir, wind speed, SOW persistent
					else
					{//interpolate
						wxVal = Round(lastWx + ((nextWx - lastWx) / (nextDay - lastDay))
							* (DayOfYear(periods[varIDs[vID]][i]->start) - lastDay));
					}
					madeWx = true;
					madeCnt++;
				}
			}
			if(haveWx || madeWx)
			{
				for(;d < i; d++)
				{
					if(d < i - 1)
					{
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
				fxVals[loc] = (double)isFD;
				fyVals[loc] = wxVal;
				if(!fireOptsSet->m_Conditional || isFD)
				{
					lfxVals[fdLoc] = (double)isLFD;
					lfyVals[fdLoc] = wxVal;
					mfxVals[fdLoc] = (double)isMFD;
					mfyVals[fdLoc] = wxVal;
					fdLoc++;
				}
				loc++;
			}
		}
	}

}
void CClimAnalysis::CreateLogitFilesQuick(CFireOptionsSet *fireOptsSet,
		long *nV, long *nFD, long *nLFD, long *nMFD, long *_discards, long *madeUps)
{
	chdir(theApp.dbDir);
	CClimateSet vclimSet(fireOptsSet->m_pDatabase);
	vclimSet.m_strFilter.Format("[VarID] = %d", fireOptsSet->m_VarID);
	vclimSet.Open();
	CString varName = vclimSet.m_ShortName;
	vclimSet.Close();
	FILE *fd = fopen("fd.dat", "wt");
	fprintf(fd, "FireDay %s\n", varName);
	FILE *lfd = fopen("lfd.dat", "wt");
	fprintf(lfd, "LargeFireDay %s\n", varName);
	FILE *mfd = fopen("mfd.dat", "wt");
	fprintf(mfd, "MultiFireDay %s\n", varName);
	FILE *bads = fopen("discards.dat", "wt");
	fprintf(bads, "Discards for %s\n", (fpSet->m_SIG_Station.GetLength() > 6) ? fpSet->m_SIG_Station
		: "Station: " + fpSet->m_SIG_Station);
	fprintf(bads, "(Fire records with no corresponding weather record)\nVariable: %s\n", varName);
	fprintf(bads, "Data Years: %d - %d\n", fpSet->m_StartYear, fpSet->m_EndYear);
	fprintf(bads, "Time Frame: %d/%d - %d/%d\n", MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay,
		MonthChtoInt(fpSet->m_EndMonth), fpSet->m_EndDay);
	long _nV = 0, _nFD = 0, _nLFD = 0, _nMFD = 0, discards = 0;
	//dump daily values to file...
	int isFD, isLFD, isMFD, f, vID;
	vID = fireOptsSet->m_VarID - 1;
	bool round = true;
	if(varIDs[0] == 8)
		round = false;
	bool haveWx, madeWx, first;
	double lastWx = 0.0, nextWx, wxVal;
	int lastDay, nextDay;
	long madeCnt = 0;
	int d;
	for(int y = 0; y < periods[varIDs[vID]][0]->years; y++)
	{
		//for each year...
		first = true;
		d = 0;
		for(int i = 0; i < numPeriods; i++)
		{
			haveWx = madeWx = false;
			if(periods[varIDs[vID]][i]->yStats[y].N() > 0)
			{//have weather....
				haveWx = true;
				lastWx = wxVal = periods[varIDs[vID]][i]->yStats[y].Mean();
				if(round)
					lastWx = wxVal = Round(lastWx);
				lastDay = DayOfYear(periods[varIDs[vID]][i]->start);
				first = false;
			}
			if(!haveWx && !first)
			{
				nextDay = 0;
				for(int p = i + 1; p < numPeriods && p < i + theApp.maxMissingWx + 1; p++)
				{
					if(periods[varIDs[vID]][p]->yStats[y].N() > 0)
					{
						nextDay = DayOfYear(periods[varIDs[vID]][p]->start);
						nextWx = periods[varIDs[vID]][p]->yStats[y].Mean();
						if(round)
							nextWx = Round(nextWx);
						break;
					}
				}
				if((nextDay > 0) && (nextDay - lastDay - 1)<= theApp.maxMissingWx)
				{
					if(varIDs[0] == 8 || varIDs[0] == 9)//precip amount or duration always zero
						wxVal = 0.0;
					else if(varIDs[0] == 10 || varIDs[0] == 23 || varIDs[0] == 24)
						wxVal = lastWx;   //wind dir, wind speed, SOW persistent
					else
					{//interpolate
						wxVal = Round(lastWx + ((nextWx - lastWx) / (nextDay - lastDay))
							* (DayOfYear(periods[varIDs[vID]][i]->start) - lastDay));
					}
					madeWx = true;
					madeCnt++;
				}
			}
			if(haveWx || madeWx)
			{
				for(;d < i; d++)
				{
					if(d < i - 1)
					{
						if(fires[y][d].human > 0 && (fireOptsSet->m_Human || fireOptsSet->m_All))
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
				//if(!conditional)
				fprintf(fd, "%d %.2f\n",isFD, wxVal);
				if(!fireOptsSet->m_Conditional || isFD)
				{
					fprintf(lfd, "%d %.2f\n",isLFD, wxVal);
					fprintf(mfd, "%d %.2f\n",isMFD, wxVal);
				//	_nV++;
				}
				_nV++;
			}
		}
	}
	fflush(fd);
	fflush(lfd);
	fflush(mfd);
	fflush(bads);
	fclose(fd);
	fclose(lfd);
	fclose(mfd);
	fclose(bads);
	*nV = _nV;
	*nFD = _nFD;
	*nLFD = _nLFD;
	*nMFD = _nMFD;
	*_discards = discards;
	*madeUps = madeCnt;
}

void CClimAnalysis::CreateLogitFiles(CFiresSet *fires, int lFire, int mfDay,
									 CString varName, BOOL conditional, long *nV, long *nFD, long *nLFD,
									 long *nMFD, long *_discards, long *madeUps)
{
	chdir(theApp.dbDir);
	FILE *fd = fopen("fd.dat", "wt");
	fprintf(fd, "FireDay %s\n", varName);
	FILE *lfd = fopen("lfd.dat", "wt");
	fprintf(lfd, "LargeFireDay %s\n", varName);
	FILE *mfd = fopen("mfd.dat", "wt");
	fprintf(mfd, "MultiFireDay %s\n", varName);
	FILE *bads = fopen("discards.dat", "wt");
	fprintf(bads, "Discards for %s\n", (fpSet->m_SIG_Station.GetLength() > 6) ? fpSet->m_SIG_Station
		: "Station: " + fpSet->m_SIG_Station);
	fprintf(bads, "(Fire records with no corresponding weather record)\nVariable: %s\n", varName);
	fprintf(bads, "Data Years: %d - %d\n", fpSet->m_StartYear, fpSet->m_EndYear);
	fprintf(bads, "Time Frame: %d/%d - %d/%d\n", MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay,
		MonthChtoInt(fpSet->m_EndMonth), fpSet->m_EndDay);
	long _nV = 0, _nFD = 0, _nLFD = 0, _nMFD = 0, discards = 0;
	//dump daily values to file...
	int isFD, isLFD, isMFD, f;
	fires->MoveFirst();
	CFireAgencySet agencySet(fires->m_pDatabase);
	CFireRegionSet regSet(fires->m_pDatabase);
	CFireUnitSet unitSet(fires->m_pDatabase);
	CFireSubunitSet subunitSet(fires->m_pDatabase);
	agencySet.Open();
	regSet.Open();
	unitSet.Open();
	subunitSet.Open();
	bool round = true;
	if(varIDs[0] == 8)
		round = false;
	else if(varIDs[0] > STATICVARIDS)
	{
		CUserVarSet usv(pDB);
		usv.m_strFilter.Format("[VarID] = %d", varIDs[0] + 1);
		usv.Open();
		if(!usv.IsBOF() && !usv.IsEOF())
		{
			if(usv.m_Decimals != 0)
				round = false;
		}
		usv.Close();
	}
	bool haveWx, madeWx, first;
	double lastWx = 0.0, nextWx, wxVal;
	int lastDay, nextDay;
	long madeCnt = 0;
	for(int y = 0; y < periods[varIDs[0]][0]->years; y++)
	{
		//for each year...
		first = true;
		for(int i = 0; i < numPeriods; i++)
		{
			haveWx = madeWx = false;
			if(periods[varIDs[0]][i]->yStats[y].N() > 0)
			{//have weather....
				haveWx = true;
				lastWx = wxVal = periods[varIDs[0]][i]->yStats[y].Mean();
				if(round)
					lastWx = wxVal = Round(lastWx);
				lastDay = DayOfYear(periods[varIDs[0]][i]->start);
				first = false;
			}
			if(!haveWx && !first)
			{
				nextDay = 0;
				for(int p = i + 1; p < numPeriods && p < i + theApp.maxMissingWx + 1; p++)
				{
					if(periods[varIDs[0]][p]->yStats[y].N() > 0)
					{
						nextDay = DayOfYear(periods[varIDs[0]][p]->start);
						nextWx = periods[varIDs[0]][p]->yStats[y].Mean();
						if(round)
							nextWx = Round(nextWx);
						break;
					}
				}
				if((nextDay > 0) && (nextDay - lastDay - 1)<= theApp.maxMissingWx)
				{
					if(varIDs[0] == 8 || varIDs[0] == 9)//precip amount or duration always zero
						wxVal = 0.0;
					else if(varIDs[0] == 10 || varIDs[0] == 23 || varIDs[0] == 24)
						wxVal = lastWx;   //wind dir, wind speed, SOW persistent
					else
					{//interpolate
						wxVal = Round(lastWx + ((nextWx - lastWx) / (nextDay - lastDay))
							* (DayOfYear(periods[varIDs[0]][i]->start) - lastDay));
					}
					madeWx = true;
					madeCnt++;
				}
			}
			if(haveWx || madeWx)
			{
				//see if there's a fire day
				isLFD = isMFD = isFD = f = 0;
				while(!fires->IsEOF()
					&& (fires->m_Discovery.GetYear() < periods[varIDs[0]][i]->baseYear + y
					|| (fires->m_Discovery.GetYear() == periods[varIDs[0]][i]->baseYear + y
						&& DayOfYear(fires->m_Discovery) < DayOfYear(periods[varIDs[0]][i]->start))))
				{
					discards++;
					fprintf(bads, "%2d/%2d/%d", fires->m_Discovery.GetMonth(),
						fires->m_Discovery.GetDay(),
						fires->m_Discovery.GetYear());
					if(!fires->IsFieldNull(&fires->m_AgencyID))
					{
						agencySet.m_strFilter.Format("[AgencyID] = %ld", fires->m_AgencyID);
						agencySet.Requery();
						if(!agencySet.IsEOF())
							fprintf(bads, " %s", agencySet.m_Name);
					}
					if(!fires->IsFieldNull(&fires->m_RegionID))
					{
						regSet.m_strFilter.Format("[RegionID] = %ld", fires->m_RegionID);
						regSet.Requery();
						if(!regSet.IsEOF())
							fprintf(bads, " %s", regSet.m_Name);
					}
					if(!fires->IsFieldNull(&fires->m_UnitID))
					{
						unitSet.m_strFilter.Format("[UnitID] = %ld", fires->m_UnitID);
						unitSet.Requery();
						if(!unitSet.IsEOF())
							fprintf(bads, " %s", unitSet.m_Name);
					}
					if(!fires->IsFieldNull(&fires->m_SubunitID))
					{
						subunitSet.m_strFilter.Format("[SubunitID] = %ld", fires->m_SubunitID);
						subunitSet.Requery();
						if(!subunitSet.IsEOF())
							fprintf(bads, " %s", subunitSet.m_Name);
					}
					fprintf(bads, " Acres:%.2f  Cause:%ld\n", fires->m_TotalAcres,
						fires->m_StatisticalCause);
					fires->MoveNext();
				}
				if(!fires->IsEOF()
					&& fires->m_Discovery.GetYear() == periods[varIDs[0]][i]->baseYear + y
					&& DayOfYear(fires->m_Discovery) == DayOfYear(periods[varIDs[0]][i]->start))
				{
					isFD = 1;
					while(!fires->IsEOF()
						&& fires->m_Discovery.GetYear() == periods[varIDs[0]][i]->baseYear + y
						&& DayOfYear(fires->m_Discovery) == DayOfYear(periods[varIDs[0]][i]->start))
					{
						f++;
						if(fires->m_TotalAcres >= (double)lFire)
							isLFD = 1;
						fires->MoveNext();
					}
					if(f >= mfDay)
						isMFD++;
				}
				if(isFD)
					_nFD++;
				if(isLFD)
					_nLFD++;
				if(isMFD)
					_nMFD++;
				//if(!conditional)
				fprintf(fd, "%d %.2f\n",isFD, wxVal);
				if(!conditional || isFD)
				{
					fprintf(lfd, "%d %.2f\n",isLFD, wxVal);
					fprintf(mfd, "%d %.2f\n",isMFD, wxVal);
				//	_nV++;
				}
				_nV++;
			}
		}
	}
	fclose(fd);
	fclose(lfd);
	fclose(mfd);
	fclose(bads);
	agencySet.Close();
	regSet.Close();
	unitSet.Close();
	subunitSet.Close();
	*nV = _nV;
	*nFD = _nFD;
	*nLFD = _nLFD;
	*nMFD = _nMFD;
	*_discards = discards;
	*madeUps = madeCnt;
}

void CClimAnalysis::LocateEvents(QueryRow *qRows, int rows, int days)
{
	double **vals, *compares;
	compares = new double[rows];
	vals = new double *[rows];
	int i;
	for(i = 0; i < rows; i++)
		vals[i] = new double[days];
	CString *userfmts = NULL, *userAbbrevs = NULL;
	int uVars = HasUserVars();
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
	}
	int pLen = atoi(fpSet->m_PeriodLength);
	bool isSIG = strlen(fpSet->m_SIG_Station) > 6 ? true : false;
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	//output header
	fprintf(stream, "FireFamily Plus Event Locator Report\n");
	fprintf(stream, "   Listing of Selected Events\n");

// added for batch
	// (code interspersed)
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch

	fprintf(stream, "\nEvent Definition:\n", pLen);
	for(i = 0; i < rows; i++)
	{
		CString str1, str2;
		fprintf(stream, "    ");
		if(i > 0)
			fprintf(stream, "%s ", (qRows[i].rowOp == AND) ? "AND" : "OR");
		if(days > 0)
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
				str1.Format("Daily(%s)", qRows[i].varString.Trim());
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
			for(int y = 0; y < periods[varIDs[0]][0]->years; y++)
			{
				//for each year...
				for(int p = 0; p < numPeriods; p++)
					//for each period....(Always 1 day)
					if(periods[varIDs[i]][p]->yStats[y].N() > 0)
						pStats.Accumulate(periods[varIDs[i]][p]->yStats[y].Mean());
			}
			pStats.Complete();
			compares[i] = pStats.Percentile(qRows[i].value / 100.0);
			fprintf(stream, "(%.2f)\n", compares[i]);
			pStats.Reset();
		}
	}
	fprintf(stream, "\n         %d-Day Periods\n\n", days);
	//header
	fprintf(stream, "DATE      ");
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
	fprintf(stream, "\n------------");
	for(i = 0; i < nVarIDs; i++)
		fprintf(stream, "-------");
	fprintf(stream, "\n");
	//now cycle thru data, get values and check conditions
	bool hit, noVal;
	double *rVals = new double[rows];
	bool *rHits = new bool[rows];
	long hitCount = 0L, binCount = 0L, rejectCount = 0;
	for(int y = 0; y < periods[varIDs[0]][0]->years; y++)
	{
		//for each year...
		for(i = 0; i <= numPeriods - days; i++)
		{	//for each period....(Fill vals arrays
			int v;
			for(v = 0; v < nVarIDs; v++)
			{
				for(int p = 0; p < days; p++)
				{
					if(periods[varIDs[v]][i + p]->yStats[y].N() > 0)
						vals[v][p] = periods[varIDs[v]][i + p]->yStats[y].Mean();
					else
						vals[v][p] = NOVALUE;
				}
			}
			//HAVE BINNED DATA, NOW CHECK FOR CONDITIONS
			hit = true;
			noVal = false;
			for(v = 0; v < rows/* && hit*/; v++)
			{ //check each row
				double rVal = 0.0;
				hit = true;
				for(int p = 0; p < days; p++)
				{
					if(vals[v][p] == NOVALUE)
					{
						hit = false;
						noVal = true;
						break;
					}
					else
					{
						switch(qRows[v].cat)
						{
						case Sum:
						case Avg:
							rVal += vals[v][p];
							break;
						case Max:
							if(p == 0)
								rVal = vals[v][p];
							else
								rVal = max(rVal, vals[v][p]);
							break;
						case Min:
							if(p == 0)
								rVal = vals[v][p];
							else
								rVal = min(rVal, vals[v][p]);
							break;
						case Daily:
							rVal = vals[v][p];
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
						rVal /= days;
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
			}
			//rows done, now use rowOps to see if conditions met
			hit = rHits[0];
			for(v = 1; v < rows; v++)
			{
				if(qRows[v].rowOp == AND)
					hit = hit && rHits[v];
				else
					hit = hit || rHits[v];
			}
			if(hit)
			{//all conditions met
				fprintf(stream, "%02d/%02d/%4d ",
					periods[varIDs[0]][i]->start.GetMonth(), periods[varIDs[0]][i]->start.GetDay(),
					periods[varIDs[0]][i]->baseYear + y);
				for(v = 0; v < rows; v++)
				{
						fprintf(stream, " %6.2f", rVals[v]);
				}
				fprintf(stream, "\n");
				hitCount++;
				//increment i to prevent overlap
				i += days - 1;
			}
			if(!hit && noVal)
				rejectCount++;
			binCount++;
		}
	}
	fprintf(stream, "\n%ld hits out of %ld\n", hitCount, binCount);
	fprintf(stream, "%ld rejects for no/missing observations\n", rejectCount);
	if(isSIG)
		AddSIGDescription(stream);
	fprintf(stream, "\n%s\n", AddDateStamp());
	if(userfmts)
		delete[] userfmts;
	if(userAbbrevs)
		delete[] userAbbrevs;
	delete[] compares;
	for(i = 0; i < rows; i++)
		delete[] vals[i];
	delete[] vals;
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
			if(str == _T("EventLocatorDoc"))
			{
				CEventLocatorDoc *pDoc = (CEventLocatorDoc *)	curTemplate->OpenDocumentFile(fName);
				pDoc->isTemp = true;
				pDoc->startYear = fpSet->m_StartYear;
				pDoc->startMonth = MonthChtoInt(fpSet->m_StartMonth);
				pDoc->startDay = fpSet->m_StartDay;
				pDoc->endYear = fpSet->m_EndYear;
				pDoc->SIG_Station = fpSet->m_SIG_Station;
				pDoc->m_pDB = fpSet->m_pDatabase;
				pDoc->m_fpSet = fpSet;
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

void CClimAnalysis::AddSIGDescription(FILE *stream)
{
	if(fpSet->m_SIG_Station.GetLength() > 6)
	{
		fprintf(stream, "\nStations in %s\n", fpSet->m_SIG_Station);
		CStationInSIGSet stas(pDB);
		CString query;
		char sig[64];
		strcpy_s(sig, fpSet->m_SIG_Station);
		query.Format("[SIG] = '%-20.20s'", &sig[6]);
		stas.m_strFilter = _T(query);
		stas.Open();
		CSIGStationSet stn(pDB);
		stn.Open();
		while(!stas.IsEOF())
		{
			query.Format("[StationID] = '%6.6s'", stas.m_StationID);
			stn.m_strFilter = _T(query);
			stn.Requery();
			if(useNFDRS)
				fprintf(stream, "    %6.6s - %-20.20s Model: %s      Weight: %.2f\n",
					stas.m_StationID, stn.m_Name, GetModelString(&stn), stas.m_WeightFactor);
			else
				fprintf(stream, "    %6.6s - %-20.20s Weight: %.2f\n",
					stas.m_StationID, stn.m_Name, stas.m_WeightFactor);
			stas.MoveNext();
		}
		stn.Close();
		stas.Close();
	}
}

CString CClimAnalysis::FrequencyTable(int loc, bool createDoc /*= true*/)
{
	int pLen = atoi(fpSet->m_PeriodLength);
	bool isSIG = strlen(fpSet->m_SIG_Station) > 6 ? true : false;
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	//calc critical percentiles
	//output header
	fprintf(stream, "FireFamily Plus Frequency Distribution Report\n");

	// added for batch
	// (code interspersed)
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	int Delim = reportSet.m_Delimiter;

	CString dStr;

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch

	CString extremeString = " ";
	if (m_usedExtremes)
	{
		if (m_extremeIsMax[loc])
			extremeString = " - Daily Max";
		else
			extremeString = " - Daily Min";
	}
	fprintf(stream, "Variable: %s%s\n", climSet->m_Variable_Name, extremeString);
	if (customInits)
		fprintf(stream,"(Calculated using at least one custom yearly init)\n");

	reportSet.Close();
	CString delim;
	switch (Delim){
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

	//get and sort the values
	Stats stats;
	for(int p = 0; p < numPeriods; p++)
		for(int y = 0; y < periods[loc][p]->years; y++)
			for(int v = 0; v < periods[loc][p]->yStats[y].N(); v++)
				stats.Accumulate(periods[loc][p]->yStats[y].vals[v]);
	stats.Complete();
	double step = 1;
	double start = floor(stats.Min()), end = ceil(stats.Max()), cp1, cp2;
	cp1 = stats.Percentile(((double)climSet->m_CriticalPercentile) / 100.0);
	cp2 = stats.Percentile(((double)climSet->m_cp2) / 100.0);
	fprintf(stream, "\n%d%% = %.2f\n%d%% = %.2f\n",
		climSet->m_CriticalPercentile, cp1,
		climSet->m_cp2, cp2);
	int nClasses = (int)(end - start);
	if(nClasses > 100)
	{
		step = nClasses / 100 + 1;
			nClasses /=  step;
	}
	if (climSet->m_VarID == 40)//must be 0- 1.0
	{
		step = 0.1;
		nClasses = 10;
	}
	if((start + nClasses * step) <= stats.Max())
		nClasses++;
	if (nClasses == 1)//must be 0- 1.0
	{
		step = 0.1;
		nClasses == 10;
	}
	fprintf(stream,
		"%ld Days\n"
		"     Range        Frequency  Relative %%  Cumulative %%\n"
		"-------------------------------------------------------\n",
		stats.N());
	long v = 0, tcount = 0, sumN = 0;
	for(int i = 0; i < nClasses; i++)
	{
		tcount = 0;
		while(v < stats.N() && stats.vals[v] < ((double)start + ((double)(i + 1) * step)))
		{
			tcount++;
			v++;
		}
		sumN += tcount;
		if (Delim > 0)
			fprintf(stream," %5.1f - %5.1f%s%6ld%s%5.2f%s%6.2f\n",
				(double)start + (double)i * step, (double)start + (double)((i + 1) * step - (step * 0.1)), delim,
				tcount, delim,
				(stats.N() <= 0) ? 0 : ((double) tcount) / ((double) stats.N()) * 100.0, delim,
				(stats.N() <= 0) ? 0 : ((double)sumN) / ((double) stats.N()) * 100.0);
		else
			fprintf(stream,
				" %5.1f - %5.1f      %6ld      %5.2f      %6.2f\n",
				(double)start + (double)i * step, (double)start + (double)((i + 1) * step - (step * 0.1)),
				tcount,
				(stats.N() <= 0) ? 0 : ((double) tcount) / ((double) stats.N()) * 100.0,
				(stats.N() <= 0) ? 0 : ((double)sumN) / ((double) stats.N()) * 100.0);
	}
	if(isSIG)
		AddSIGDescription(stream);
	fprintf(stream, "\n%s\n", AddDateStamp());
	CString retName = fName;
	if(stream)
	{
		fclose(stream);
		if(autosave)
		{
			CString saveName;
			saveName.Format(autosavePath + climSet->m_ShortName + "FrequencyTable.txt");
			unlink(saveName);
			rename(fName, saveName);
			retName = saveName;
		}
		else
		{
			//now display the file in a child window
			if(createDoc)
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
						CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(fName);
						pDoc->isTemp = true;
						free(fName);
						CString tTitle;
						tTitle.Format("%s - %s Frequency Distribution", fpSet->m_SIG_Station, climSet->m_Variable_Name);
						pDoc->SetTitle(tTitle);
						return retName;
					}
				}
			}
		}
	}
	free(fName);
	return retName;
}

CString CClimAnalysis::PrecipFrequencyTable(int loc, bool createDoc /*= true*/)
{
	int pLen = atoi(fpSet->m_PeriodLength);
	bool isSIG = strlen(fpSet->m_SIG_Station) > 6 ? true : false;
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	//output header
	fprintf(stream, "FireFamily Plus Frequency Distribution Report\n");
// added for batch
	// (code interspersed)
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch

	CString extremeString = " ";
	if (m_usedExtremes)
	{
		if (m_extremeIsMax[loc])
			extremeString = " - Daily Max";
		else
			extremeString = " - Daily Min";
	}
	fprintf(stream, "Variable: %s%s\n", climSet->m_Variable_Name, extremeString);
	fprintf(stream, "       %s %d - %s %d,  %d - %d\n\n",
		fpSet->m_StartMonth, fpSet->m_StartDay, fpSet->m_EndMonth, fpSet->m_EndDay,
		fpSet->m_StartYear, fpSet->m_EndYear);
	//get and sort the values
	Stats stats;
	for(int p = 0; p < numPeriods; p++)
		for(int y = 0; y < periods[loc][p]->years; y++)
			for(int v = 0; v < periods[loc][p]->yStats[y].N(); v++)
				stats.Accumulate(periods[loc][p]->yStats[y].vals[v]);
	stats.Complete();
	double cp1, cp2;
	cp1 = stats.Percentile(((double)climSet->m_CriticalPercentile) / 100.0);
	cp2 = stats.Percentile(((double)climSet->m_cp2) / 100.0);
	fprintf(stream, "\n%d%% = %.2f\n%d%% = %.2f\n",
		climSet->m_CriticalPercentile, cp1,
		climSet->m_cp2, cp2);
	fprintf(stream,
		"%ld Days\n"
		"     Range        Frequency  Relative %%  Cumulative %%\n"
		"-------------------------------------------------------\n",
		stats.N());
	long v = 0, tcount = 0, sumN = 0;
	while(stats.vals[v] <= 0.0 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"       0            %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 0.005 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"     Trace          %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 0.01 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"     0.01           %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 0.05 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  0.02 - 0.05       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 0.1 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  0.06 - 0.10       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 0.2 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  0.11 - 0.20       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 0.3 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  0.21 - 0.30       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 0.4 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  0.31 - 0.40       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 0.5 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  0.41 - 0.50       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 0.6 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  0.51 - 0.60       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 0.7 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  0.61 - 0.70       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 0.8 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  0.71 - 0.80       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 0.9 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  0.81 - 0.90       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 1.0 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  0.91 - 1.00       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 1.5 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  1.01 - 1.50       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 2.0 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  1.51 - 2.00       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 2.5 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  2.01 - 2.50       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 3.0 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  2.51 - 3.00       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 3.5 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  3.01 - 3.50       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = 0;
	while(stats.vals[v] <= 4.0 && v < stats.N())
	{
		tcount++;
		v++;
	}
	sumN += tcount;
	fprintf(stream,
			"  3.51 - 4.00       %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);
	tcount = stats.N() - sumN;
	sumN += tcount;
	fprintf(stream,
			"     > 4.00         %6ld      %5.2f      %6.2f\n",
			tcount,
			((double) tcount) / ((double) stats.N()) * 100.0,
			((double)sumN) / ((double) stats.N()) * 100.0);

	if(isSIG)
		AddSIGDescription(stream);
	fprintf(stream, "\n%s\n", AddDateStamp());
	CString retName = fName;
	fclose(stream);
	if(autosave)
	{
		CString saveName;
		saveName = autosavePath + climSet->m_ShortName.Trim() + "StatsTable.txt";
		unlink(saveName);
		rename(fName, saveName);
		free(fName);
		return saveName;
	}
	if(createDoc)
	{
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
				tTitle.Format("%s - %s Frequency Distribution", fpSet->m_SIG_Station, climSet->m_Variable_Name);
				pDoc->SetTitle(tTitle);
				return retName;
			}
		}
	}
	free(fName);
	return retName;
}

void CClimAnalysis::GetLastDay(COleDateTime &trg)
{
	//find the last valid wx date
	int yr = eDates[numPeriods - 1].GetYear() - sDates[numPeriods - 1].GetYear();
	while(yr >= 0)
	{
		for(int i = numPeriods - 1; i >= 0; i--)
		{
			if(periods[varIDs[0]][i]->yStats[yr].N() > 0)
			{
				trg.SetDateTime(sDates[i].GetYear() + yr,
					sDates[i].GetMonth(),
					sDates[i].GetDay(), 14, 0, 0);
				return;
			}
		}
		yr--;
	}
	//didn't set it, set to last possible day
	trg = eDates[numPeriods - 1];
}

int CClimAnalysis::HasUserVars()
{
	int cnt = 0;
	for(int i = 0; i < nVarIDs; i++)
	{
		if(varIDs[i] >= STATICVARIDS)
			cnt++;
	}
	return cnt;
}

int CClimAnalysis::AbbrevToVarID(CString str)
{
	int ret = 0;//safety(Dry Bulb Temperature)
	str.TrimLeft();
	str.TrimRight();
	CString comp;
	for(int i = 0; i < MAXVARIDS; i++)
	{
		comp = varNames[i];
		comp.TrimLeft();
		comp.TrimRight();
		if(comp.Compare(str) == 0)
		{
			if(periods[i])//safety
				return i;
		}
	}
	return ret;
}

long CClimAnalysis::AccumulateStationUserVars(CString stationStr)
{
	long tcount = 0;
	CString query = "", temp, staQuery;
	fpSet->BuildBaseQuery(query, "ObsDate", false, inverted);
	if(query.GetLength() > 0)
		temp.Format(" and [StationID] = '%6.6s'", stationStr);
	else
		temp.Format("[StationID] = '%6.6s'", stationStr);

	CUserVarSet varSet(pDB);
	varSet.Open();
	int nVars = 0;
	int v;
	for(v = STATICVARIDS; v < MAXVARIDS; v++)
	{
		if(periods[v])
			nVars++;
	}
	ID2 *ids = new ID2[nVars];
	int loc = 0;
	for(v = STATICVARIDS; v < MAXVARIDS; v++)
	{
		if(periods[v])
		{
			varSet.m_strFilter.Format("[VarID] = %d", v + 1);
			varSet.Requery();
			if(!varSet.IsEOF())
			{
				ids[loc].userID = v;
				ids[loc].varID = v;
				ids[loc].fieldName.Format("%d", varSet.m_UserVarID);
				loc++;
			}
		}
	}

	CRecordset valSet(pDB);
	CString strSQL = "SELECT * FROM ffpUserVals WHERE (" + query + ")" + temp;
	valSet.m_strSort = _T("[ObsDate]");
	valSet.Open(CRecordset::dynaset, strSQL);
	while(!valSet.IsEOF())// && (!recCounter || (recCounter && !recCounter->Abort())))
	{
		CDBVariant var;
		valSet.GetFieldValue("ObsDate", var);
		if(var.m_dwType != DBVT_NULL)
		{
			COleDateTime dt(var.m_pdate->year, var.m_pdate->month, var.m_pdate->day,
				var.m_pdate->hour, var.m_pdate->minute, var.m_pdate->second);

			if(dt.GetStatus() == 0)
			{
				int per = GetPeriod(dt);
				if(per >= 0)
				{
					double val = 0.0;
					for(int f = 0; f < nVars; f++)
					{
						CDBVariant fvar;
						valSet.GetFieldValue(ids[f].fieldName, fvar);
						if(fvar.m_dwType != DBVT_NULL)
						{
							double dval;// = fvar.dblVal;
							switch(fvar.m_dwType)
							{
							case DBVT_SINGLE:
								dval = fvar.m_fltVal;
								break;
							case DBVT_DOUBLE:
								dval = fvar.m_dblVal;
								break;
							case DBVT_SHORT:
								dval = fvar.m_iVal;
								break;
							case DBVT_LONG:
								dval = fvar.m_lVal;
								break;
							default:
								continue;
							}
							//double dval = fvar.m_fltVal;
							periods[ids[f].varID][per]->Accumulate(dt, dval);
						}
					}
				}
				tcount++;
			}
		}
		valSet.MoveNext();
	}
	varSet.Close();
	valSet.Close();
	delete[] ids;
	return tcount;
}

int CClimAnalysis::AnalyzeBatchItem(CWnd *_caller, CClimateSet *_climSet, CFireplusSet *_fpSet, bool _inverted)
{
	CWaitCursor wait;
	inverted = _inverted;
	//calculate and create varIDs
	climSet = _climSet;
	int ids = 0;
	climSet->MoveFirst();
	while(!climSet->IsEOF())
	{
		//check each var type
		if(climSet->m_Stats_Table || climSet->m_Stats_Graph || climSet->m_Daily_Freqs
			|| climSet->m_Period_Mins || climSet->m_Period_Maxs || climSet->m_Data_Count)
			ids++;
		climSet->MoveNext();
	}
	if(!ids)
		return 1;
	int *tIDs = new int[ids];
	ids = 0;
	climSet->MoveFirst();
	while(!climSet->IsEOF())
	{
		//check each var type
		if(climSet->m_Stats_Table || climSet->m_Stats_Graph || climSet->m_Daily_Freqs
			|| climSet->m_Period_Mins || climSet->m_Period_Maxs || climSet->m_Data_Count)
		{
			tIDs[ids] = climSet->m_VarID - 1;
			ids++;
		}
		climSet->MoveNext();
	}
	return AnalyzeBatchItem(_caller, tIDs, ids, _fpSet, inverted);
}

int CClimAnalysis::AnalyzeBatchItem(CWnd *_caller, int *_varIDs, int _nVarIDs, CFireplusSet *_fpSet, bool _inverted, bool isFPA)
{
	int ret = Analyze2(_varIDs, _nVarIDs, _fpSet, isFPA);
	if (ret == 0)
	{
		int wsFireYears = 0;
		Period*** wsPeriods = GetWorkingSetPeriods(periods);
		m_opts.numtPeriods = numPeriods;
		m_opts.tPeriods = periods;
		CFireDay** wsFires = GetWorkingSetFires(fires, &wsFireYears);
		m_opts.tFires = fires;
		periods = wsPeriods;
		fires = wsFires;
		fireYears = wsFireYears;

	}
	return ret;
	//caller = _caller;
	fpSet = _fpSet;
	//int ret = 0;
	inverted = _inverted;
	if(inverted)
		pCUPDUPData->SetProgress("Processing Auxiliary Years Data");
	else
		pCUPDUPData->SetProgress("Processing Data");
	CWaitCursor wait;
	nVarIDs = _nVarIDs;
	varIDs = _varIDs;
	//model use flags - optimize to only calculate models used
	killPeriods = true;
	fpSet = _fpSet;
	useCanadian = false;
	useNFDRS = false;
	count = 0;
	userCount = 0;
	int sYear = fpSet->m_StartYear, eYear = fpSet->m_EndYear;
	//if(inverted)
	//	return ret;
	//if(inverted)
	//{//need to know how many years
		CString wxQuery = "";
		CWxSet tSet(fpSet->m_pDatabase);
		tSet.m_strSort = "[ObsDate]";
		tSet.Open();
		if(fpSet->m_SIG_Station.GetLength() > 6)//SIG
		{
			char sig[64];
			strcpy_s(sig, fpSet->m_SIG_Station);
			CString temp;
			CStationInSIGSet sSet(pDB);
			temp.Format("[SIG] = '%-20.20s'", &sig[6]);
			sSet.m_strFilter = _T(temp);
			sSet.Open();
			CSIGStationSet tStaSet(sSet.m_pDatabase);
			tStaSet.m_strFilter.Format("[StationID] = '%6.6s'", sSet.m_StationID);
			tStaSet.Open();
			while(!sSet.IsEOF())
			{
				//tSet.m_strFilter.Format("([StationID] = '%s' AND ([ObsType] = 'O' OR ([ObsType] = 'R' AND [Hour] = %d AND [SOW] IS NOT NULL)))", sSet.m_StationID, tStaSet.m_RegSchdObs);
				// updated 09/2012 with the new DailyObs field in WxObs table
				if (isNFDRS2016(tStaSet.m_NFDRSFM[0])) // new NFDRS2016 calc needs hourly data
					tSet.m_strFilter.Format("([StationID] = '%6.6s')", sSet.m_StationID);
				else
					tSet.m_strFilter.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", sSet.m_StationID);
				tSet.Requery();
				if(!tSet.IsBOF() && !tSet.IsEOF())
				{
					sYear = min(sYear, tSet.m_ObsDate.GetYear());
					tSet.MoveLast();
					eYear = max(eYear, tSet.m_ObsDate.GetYear());
				}
				sSet.MoveNext();
			}
			tStaSet.Close();
			if(HasUserVars())
			{
				CString tmpStr = "", strSQL;
				sSet.MoveFirst();
				strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%6.6s'", sSet.m_StationID);
				sSet.MoveNext();
				while(!sSet.IsEOF())
				{
					tmpStr.Format(" or [StationID] = '%6.6s'", sSet.m_StationID);
					strSQL += tmpStr;
					sSet.MoveNext();
				}
				CRecordset userVals(pDB);
				userVals.m_strSort = _T("[ObsDate]");
				userVals.Open(CRecordset::dynaset, strSQL);
				if(!userVals.IsBOF() && !userVals.IsEOF())
				{
					//COleDateTime dt;
					CDBVariant var;
					userVals.GetFieldValue("ObsDate", var);
					//dt = var.date;
					sYear = min(sYear, var.m_pdate->year);
					userVals.MoveLast();
					userVals.GetFieldValue("ObsDate", var);
					//dt = var.date;
					eYear = max(eYear, var.m_pdate->year);
				}
				userVals.Close();
			}
			sSet.Close();
		}
		else //single station
		{
			CSIGStationSet tStaSet(fpSet->m_pDatabase);
			tStaSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
			tStaSet.Open();

			// updated 09/2012 with the new DailyObs field in WxObs table
			if (isNFDRS2016(tStaSet.m_NFDRSFM[0])) // new NFDRS2016 calc needs hourly data
				tSet.m_strFilter.Format("([StationID] = '%6.6s')", fpSet->m_SIG_Station);
			else
				tSet.m_strFilter.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", fpSet->m_SIG_Station);
			tStaSet.Close();
			tSet.Requery();
			if(!tSet.IsBOF() && !tSet.IsEOF())
			{
				sYear = min(sYear, tSet.m_ObsDate.GetYear());
				tSet.MoveLast();
				eYear = max(eYear, tSet.m_ObsDate.GetYear());
			}
			if(HasUserVars())
			{
				CRecordset userVals(pDB);
				CString strSQL;
				strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%s'", fpSet->m_SIG_Station);
				userVals.m_strSort = _T("[ObsDate]");
				userVals.Open(CRecordset::dynaset, strSQL);
				if(!userVals.IsBOF() && !userVals.IsEOF())
				{
					//COleDateTime dt;
					CDBVariant var;
					userVals.GetFieldValue("ObsDate", var);
					//dt = var.date;
					sYear = min(sYear, var.m_pdate->year);
					userVals.MoveLast();
					userVals.GetFieldValue("ObsDate", var);
					//dt = var.date;
					eYear = max(eYear, var.m_pdate->year);
				}
				userVals.Close();
			}
		}
		tSet.Close();
	//}
	int pLen = atoi(fpSet->m_PeriodLength);
	COleDateTimeSpan span( pLen - 1, 0, 0, 0 );
	COleDateTime d1, d2, d3;
	numPeriods = 0;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d2 = d1 + span;
	//ensure don't cross month boundary
	while(d1.GetMonth() != d2.GetMonth())
		d2 -= 1;
	while(1)
	{
		//new period coming into loop
		numPeriods++;
		if(MonthChtoInt(fpSet->m_EndMonth) == d1.GetMonth()
			&& fpSet->m_EndDay >= d1.GetDay()
			&& fpSet->m_EndDay <= d2.GetDay())
			break;
		d3 = d2 + span;
		d3 += 1;
		if(d2.GetMonth() != d3.GetMonth())//does next period cross a month boundary?
		{
			if(d3.GetDay() <= pLen / 2)//will create a new period, otherwise will combine
				numPeriods++;
			if(MonthChtoInt(fpSet->m_EndMonth) == d1.GetMonth()
				&& fpSet->m_EndDay >= d1.GetDay()
				&& fpSet->m_EndDay <= daysInMonth[d1.GetMonth() - 1])
				break;
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d2 = d1 + span;
		}
		else
		{
			d1 = d2;
			d1 += 1;
			d2 = d1 + span;
		}
		//ensure don't cross month boundary
		while(d1.GetMonth() != d2.GetMonth())
			d2 -= 1;
	}
	//ok, got numPeriods. Allocate date arrays and fill values by repeating loop
	sDates = new COleDateTime[numPeriods];
	eDates = new COleDateTime[numPeriods];
	int dLoc = 0;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d2 = d1 + span;
	//safety
	while(d1.GetMonth() != d2.GetMonth())
		d2 -= 1;
	while(1)
	{
		//new period coming into loop
		sDates[dLoc].SetDate(sYear, d1.GetMonth(), d1.GetDay());
		eDates[dLoc++].SetDate(eYear, d1.GetMonth(), d2.GetDay());
		//if(dLoc >= numPeriods)
		//	break;
		d3 = d2 + span;
		d3 += 1;
		if(d2.GetMonth() != d3.GetMonth()) //will next period cross a month boundary???
		{
			if(d3.GetDay() <= pLen / 2 && dLoc < numPeriods)
			{
				//extra group to fill
				sDates[dLoc].SetDate(sYear, d1.GetMonth(), d2.GetDay() + 1);
				if(IsLeap(eYear))
				{
					eDates[dLoc++].SetDate(eYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
				}
				else
				{
					if(d1.GetMonth() != 2)
						eDates[dLoc++].SetDate(eYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
					else
						eDates[dLoc++].SetDate(eYear, d1.GetMonth(), 28);
				}
				if(dLoc >= numPeriods)
					break;
			}
			else
			{//combine into previous group
				if(d1.GetMonth() < MonthChtoInt(fpSet->m_EndMonth))
				{
					if(IsLeap(eYear))// % 4 == 0 && (eYear % 100 != 0 || eYear % 400 == 0))
						eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
					else
					{
						if(d1.GetMonth() != 2)
							eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
						else
							eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), 28);
					}
				}
				else//last month of query
				{
					if(IsLeap(eYear))// % 4 == 0 && (eYear % 100 != 0 || eYear % 400 == 0))
						eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), fpSet->m_EndDay);
					else//daysInMonth[d1.GetMonth() - 1]);
					{
						if(d1.GetMonth() != 2)
							eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), fpSet->m_EndDay);
						else
							eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), min(fpSet->m_EndDay, 28));
					}
				}
			}
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d2 = d1 + span;
		}
		else
		{//won't cross boundary, increment normally
			d1 = d2;
			d1 += 1;
			d2 = d1 + span;
		}
		//ensure same month
		while(d1.GetMonth() != d2.GetMonth())
			d2 -= 1;
		if(dLoc >= numPeriods)
			break;
	}

	periods = new Period**[MAXVARIDS];
	for(int p = 0; p < MAXVARIDS; p++)
		periods[p] = NULL;
	int v;
	for(v = 0; v < nVarIDs; v++)
	{
		//create data storage structure
		COleDateTime m1, m2;
		periods[varIDs[v]] = new Period*[numPeriods];
		for(int i = 0; i < numPeriods; i++)
			periods[varIDs[v]][i] = new Period(sDates[i], eDates[i]);
		if(OptionFromVarID(varIDs[v]) == 1)
			useNFDRS = true;
		if(OptionFromVarID(varIDs[v]) == 3)
			useCanadian = true;
		if(varIDs[v] == 39 || varIDs[v] == 42 || varIDs[v] == 43)
			useLFI = true;
	}

	//DATA STORAGE CREATED.
	//now process selected station(s)
	CSIGStationSet staSet(pDB);
	staSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
	staSet.Open();
	CString query = "", temp, staQuery;
	fpSet->BuildBaseQuery(query, "ObsDate", true, inverted);
	// 2012 LJS - moved this extra "AND" to the single station block (below the if())
	if(fpSet->m_SIG_Station.GetLength() <= 6)//a single station
	{
		if(query.GetLength() > 0)
		   query += " AND ";

		staQuery.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
		staSet.m_strFilter.Format(staQuery);
		staSet.Requery();
		CString temp2;
		bool reCalc2016 = false;
		reCalc2016 = theApp.m_ForceNFDRS2016Recompute;
		COleDateTime n2Start, n2End;
		fpSet->GetNFDRS2016Range(staSet.m_StationID, &n2Start, &n2End);
		if (!reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0])) // we need hourly data for new calculator?
		{
			CWxSet tWxSet(pDB);
			tWxSet.m_strFilter.Format("[StationID] = '%6.6s' AND [DailyObs] = 1 AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
				n2Start.Format(), n2End.Format());
			tWxSet.Open();
			if (tWxSet.IsFieldNull(&tWxSet.m_FM1) || tWxSet.IsFieldNull(&tWxSet.m_FM10) || tWxSet.IsFieldNull(&tWxSet.m_FM100) || tWxSet.IsFieldNull(&tWxSet.m_FM1000) || tWxSet.IsFieldNull(&tWxSet.m_FuelTemperature)
				|| tWxSet.m_FM1 <= 0.0 || tWxSet.m_FM10 <= 0.0 || tWxSet.m_FM100 <= 0.0 || tWxSet.m_FM1000 <= 0.0 || tWxSet.m_FuelTemperature <= -999.0)
				reCalc2016 = true;
			if (!reCalc2016)
			{
				tWxSet.MoveLast();
				if (tWxSet.IsFieldNull(&tWxSet.m_FM1) || tWxSet.IsFieldNull(&tWxSet.m_FM10) || tWxSet.IsFieldNull(&tWxSet.m_FM100) || tWxSet.IsFieldNull(&tWxSet.m_FM1000) || tWxSet.IsFieldNull(&tWxSet.m_FuelTemperature)
					|| tWxSet.m_FM1 <= 0.0 || tWxSet.m_FM10 <= 0.0 || tWxSet.m_FM100 <= 0.0 || tWxSet.m_FM1000 <= 0.0 || tWxSet.m_FuelTemperature <= -999.0)
					reCalc2016 = true;
			}
			tWxSet.Close();
		}
		if (!useNFDRS)
			reCalc2016 = false;
		if (!reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0]))
			temp2.Format("[StationID] = '%6.6s' AND [DailyObs] = 1 AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
				n2Start.Format(), n2End.Format());
		else if (reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0]))
			temp2.Format("[StationID] = '%6.6s' AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
				n2Start.Format(), n2End.Format());
		else
			temp2.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", staSet.m_StationID);
		query += temp2;
		if (useNFDRS && isNFDRS2016(staSet.m_NFDRSFM[0]))
			query += m_strRequiredFields;

		wxSet = new CWxSet(pDB);
		wxSet->m_strSort = _T("[ObsDate]");
		wxSet->m_strFilter = query;// .Format("(%s) %s", query, temp);
		wxSet->Open();

		//set up SIGStationSet
		staQuery.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
		staSet.m_strFilter = _T(staQuery);
		staSet.Requery();
		m_strStationName = staSet.m_Name;
		ret = AnalyzeBatchStation(CString(fpSet->m_SIG_Station + " - " + staSet.m_Name), isFPA, "");
		wxSet->Close();
		delete wxSet;
		wxSet = NULL;
	}
	else {
	//is a SIG - multiple stations
		// 09/2012 ljs added fix for 'R' records
		//CString temp2;
	
		//if (!isNFDRS2016(staSet.m_NFDRSFM[0])) // new NFDRS2016 calc needs hourly data
		//   temp2.Format(" AND [DailyObs] = 1");

	   // query += temp2;

		ret = AnalyzeBatchSIG(query, isFPA);
	}
	//weather data read and analyzed....
	//make sure we have some data
	bool hasSomeData = false;
	for(v = 0; v < nVarIDs; v++)
	{
		for(int i = 0; i < numPeriods; i++)
		{
			if(periods[varIDs[v]][i]->N() > 0)
			{
				hasSomeData = true;
				break;
			}
		}
	}
	if(!hasSomeData && !inverted)
	{
		ret = -2;
	}
	//now get fires
	if(checkFires && ret == 0)
	{
		pCUPDUPData->SetProgress("Reading Fires Data");
		fireYears = eYear - sYear + 1;
		fires = new CFireDay *[fireYears];
		int y;
		for(y = 0; y < fireYears; y++)
			fires[y] = new CFireDay [numPeriods];
		CFiresSet fireSet(fpSet->m_pDatabase);
		fireSet.Open();
		//have query for fires, need to process and put into fires structure
		//note that will still have to treat fires by individual days,
		//then put days into periods
		double lA = 0.0, hA = 0.0;
		long lF = 0, hF = 0;
		int recCount = 0;
		if(fireSet.FilterToWorkingSet(fpSet, 0,0, inverted))//has fires
		{
			for(y = 0; y < fireYears; y++)
			{
				for(int i = 0; i < numPeriods ; i++)
				{
					for(int d = DayOfYear(periods[varIDs[0]][i]->start);
						d <= DayOfYear(periods[varIDs[0]][i]->end);
						d++)
					{
						recCount ++;

						lA = 0.0;
						hA = 0.0;
						lF = 0;
						hF = 0;
						while(!fireSet.IsEOF()
							&& fireSet.m_Discovery.GetYear() < periods[varIDs[0]][i]->baseYear + y)
							fireSet.MoveNext();
						while(!fireSet.IsEOF()
							&& fireSet.m_Discovery.GetYear() <= periods[varIDs[0]][i]->baseYear + y
							&& DayOfYear(fireSet.m_Discovery) < d)
							fireSet.MoveNext();
						if(!fireSet.IsEOF()
							&& fireSet.m_Discovery.GetYear() == periods[varIDs[0]][i]->baseYear + y
							&& DayOfYear(fireSet.m_Discovery) == d)
						{
							while(!fireSet.IsEOF()
								&& fireSet.m_Discovery.GetYear() == periods[varIDs[0]][i]->baseYear + y
								&& DayOfYear(fireSet.m_Discovery) == d)
							{
								switch(fireSet.m_StatisticalCause)
								{
								case 1://lightning
									lF++;
									lA = max(lA, fireSet.m_TotalAcres);
									break;
								default:
									 int passesCauseFilter = 1;

									 if (fireSet.m_AgencyID == 1) // USFS
										 passesCauseFilter = (humanCause >> (fireSet.m_StatisticalCause - 1)) & 1;
									 else
									 {
										 // convert to USFS cause type
										 int usfs = 1;
										 switch (fireSet.m_StatisticalCause)
										 {
											case 2: usfs = 4; break;
											case 4: usfs = 5; break;
											case 5: usfs = 7; break;
											case 6: usfs = 2; break;
											case 7: usfs = 6; break;
										 }
										passesCauseFilter = (humanCause >> (usfs - 1)) & 1;
									 }
			
									if (passesCauseFilter)
										hF++;
									hA = max(hA, fireSet.m_TotalAcres);
								}
								fireSet.MoveNext();
							}
						}
						fires[y][i].lightning = max(fires[y][i].lightning, lF);
						fires[y][i].human = max(fires[y][i].human, hF);
						fires[y][i].lAcresMax = max(fires[y][i].lAcresMax, lA);
						fires[y][i].hAcresMax = max(fires[y][i].hAcresMax, hA);
					}
				}
			}
		}
		fireSet.Close();
	}
	if(pCUPDUPData->ShouldTerminate())
		ret = -1;
	staSet.Close();
	return ret;
}

int CClimAnalysis::AnalyzeBatchSIG(const CString query, bool isFPA)
{
	char sig[32];
	int ret = 0;
	CString temp, staQuery;
	strcpy_s(sig, fpSet->m_SIG_Station);
	CStationInSIGSet sSet(pDB);
	temp.Format("[SIG] = '%-20.20s'", &sig[6]);
	sSet.m_strFilter = _T(temp);
	sSet.Open();
	if(sSet.IsBOF() && sSet.IsEOF())
	{
		sSet.Close();
		CString strMsg;
		strMsg.Format(
			_T("ERROR: No stations in SIG.\n Redefine Working Set Query or SIG. \n\n"));
		AfxMessageBox(strMsg);
		return 3;
	}
	//reset members to temporary storage
	Period ***tPeriods = periods;
	int tNumPeriods = numPeriods;
	COleDateTime *tSDates = sDates, *tEDates = eDates;

	//reconfigure numPeriods and sDates and eDates
	int pLen = 1;
	COleDateTimeSpan span( 0, 0, 0, 0 );
	COleDateTime d1, d2, d3;
	numPeriods = 0;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d2 = d1 + span;
	//ensure don't cross month boundary
	while(d1.GetMonth() != d2.GetMonth())
		d2 -= 1;
	while(1)
	{
		//new period coming into loop
		numPeriods++;
		if(MonthChtoInt(fpSet->m_EndMonth) == d1.GetMonth()
			&& fpSet->m_EndDay >= d1.GetDay()
			&& fpSet->m_EndDay <= d2.GetDay())
			break;
		d3 = d2 + span;
		d3 += 1;
		if(d2.GetMonth() != d3.GetMonth())//does next period cross a month boundary?
		{
			if(d3.GetDay() <= pLen / 2)//will create a new period, otherwise will combine
				numPeriods++;
			if(MonthChtoInt(fpSet->m_EndMonth) == d1.GetMonth()
				&& fpSet->m_EndDay >= d1.GetDay()
				&& fpSet->m_EndDay <= daysInMonth[d1.GetMonth() - 1])
				break;
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d2 = d1 + span;
		}
		else
		{
			d1 = d2;
			d1 += 1;
			d2 = d1 + span;
		}
		//ensure don't cross month boundary
		while(d1.GetMonth() != d2.GetMonth())
			d2 -= 1;
	}
	//ok, got new numPeriods. Allocate new date arrays and fill values by repeating loop
	int sYear = fpSet->m_StartYear, eYear = fpSet->m_EndYear;
	if(inverted)
	{//need to know how many years
		CString wxQuery = "";
		CWxSet tSet(fpSet->m_pDatabase);
		tSet.m_strSort = "[ObsDate]";
		tSet.Open();
		if(fpSet->m_SIG_Station.GetLength() > 6)//SIG
		{
			while(!sSet.IsEOF())
			{
				CSIGStationSet tStaSet(sSet.m_pDatabase);
				tStaSet.m_strFilter.Format("[StationID] = '%6.6s'", sSet.m_StationID);
				tStaSet.Open();
				// 09/2012 change for DailyObs Field
				if (isNFDRS2016(tStaSet.m_NFDRSFM[0])) // new NFDRS2016 calc needs hourly data
					tSet.m_strFilter.Format("([StationID] = '%6.6s')", sSet.m_StationID);
				else
					tSet.m_strFilter.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", sSet.m_StationID);
				tStaSet.Close();
				tSet.Requery();
				if(!tSet.IsBOF() && !tSet.IsEOF())
				{
					sYear = min(sYear, tSet.m_ObsDate.GetYear());
					tSet.MoveLast();
					eYear = max(eYear, tSet.m_ObsDate.GetYear());
				}
				sSet.MoveNext();
			}
			if(HasUserVars())
			{
				CString tmpStr = "", strSQL;
				sSet.MoveFirst();
				strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%6.6s'", sSet.m_StationID);
				sSet.MoveNext();
				while(!sSet.IsEOF())
				{
					tmpStr.Format(" or [StationID] = '%6.6s'", sSet.m_StationID);
					strSQL += tmpStr;
					sSet.MoveNext();
				}
				CRecordset userVals(pDB);
				userVals.m_strSort = _T("[ObsDate]");
				userVals.Open(CRecordset::dynaset, strSQL);
				if(!userVals.IsBOF() && !userVals.IsEOF())
				{
					//COleDateTime dt;
					CDBVariant var;
					userVals.GetFieldValue("ObsDate", var);
					//dt = var.date;
					sYear = min(sYear, var.m_pdate->year);
					userVals.MoveLast();
					userVals.GetFieldValue("ObsDate", var);
					//dt = var.date;
					eYear = max(eYear, var.m_pdate->year);
				}
				userVals.Close();
			}
			sSet.MoveFirst();
		}
		else //single station
		{
			tSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
			tSet.Requery();
			if(!tSet.IsBOF() && !tSet.IsEOF())
			{
				sYear = min(sYear, tSet.m_ObsDate.GetYear());
				tSet.MoveLast();
				eYear = max(eYear, tSet.m_ObsDate.GetYear());
			}
			if(HasUserVars())
			{
				CRecordset userVals(pDB);
				CString strSQL;
				strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%s'", fpSet->m_SIG_Station);
				userVals.m_strSort = _T("[ObsDate]");
				userVals.Open(CRecordset::dynaset, strSQL);
				if(!userVals.IsBOF() && !userVals.IsEOF())
				{
					//COleDateTime dt;
					CDBVariant var;
					userVals.GetFieldValue("ObsDate", var);
					sYear = min(sYear, var.m_pdate->year);
					userVals.MoveLast();
					userVals.GetFieldValue("ObsDate", var);
					eYear = max(eYear, var.m_pdate->year);
				}
				userVals.Close();
			}
		}
		tSet.Close();
	}
	sDates = new COleDateTime[numPeriods];
	eDates = new COleDateTime[numPeriods];
	int dLoc = 0;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d2 = d1 + span;
	//safety
	while(d1.GetMonth() != d2.GetMonth())
		d2 -= 1;
	while(1)
	{
		//new period coming into loop
		sDates[dLoc].SetDate(sYear, d1.GetMonth(), d1.GetDay());
		eDates[dLoc++].SetDate(eYear, d1.GetMonth(), d2.GetDay());
		if(dLoc >= numPeriods)
			break;
		d3 = d2 + span;
		d3 += 1;
		if(d2.GetMonth() != d3.GetMonth()) //will next period cross a month boundary???
		{
			if(d3.GetDay() <= pLen / 2)
			{
				//extra group to fill
				sDates[dLoc].SetDate(sYear, d1.GetMonth(), d2.GetDay() + 1);
				eDates[dLoc++].SetDate(eYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
				if(dLoc >= numPeriods)
					break;
			}
			else
			{//combine into previous group
				eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
			}
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d2 = d1 + span;
		}
		else
		{//won't cross boundary, increment normally
			d1 = d2;
			d1 += 1;
			d2 = d1 + span;
		}
		//ensure same month
		while(d1.GetMonth() != d2.GetMonth())
			d2 -= 1;
	}

	//figure number of stations
	int stations = 0;
	while(!sSet.IsEOF())
	{
		stations++;
		sSet.MoveNext();
	}
	//create station data storage
	double *weights = new double[stations];
	Period ****SIGPeriods = new Period***[stations];
	int i;
	for(i = 0; i < stations; i++)
	{
		SIGPeriods[i] = new Period **[MAXVARIDS];
		for(int p = 0; p < MAXVARIDS; p++)
			SIGPeriods[i][p] = NULL;
	}
	for(int v = 0; v < nVarIDs; v++)
	{
		//create data storage structure
		COleDateTime m1, m2;
		for(int p = 0; p < stations; p++)
		{
			SIGPeriods[p][varIDs[v]] = new Period*[numPeriods];
			for(int i = 0; i < numPeriods; i++)
				SIGPeriods[p][varIDs[v]][i] = new Period(sDates[i], eDates[i]);
		}
		if(OptionFromVarID(varIDs[v]) == 1)
			useNFDRS = true;
		if(OptionFromVarID(varIDs[v]) == 3)
			useCanadian = true;
		if(varIDs[v] == 39 || varIDs[v] == 42 || varIDs[v] == 43)
			useLFI = true;
	}
	//ok, data structures complete
	//process individual stations to get daily values
	sSet.MoveFirst();
	int sLoc = 0;
	while (!sSet.IsEOF() && !pCUPDUPData->ShouldTerminate())
	{
		try
		{
			weights[sLoc] = sSet.m_WeightFactor;
			periods = SIGPeriods[sLoc];
			staQuery.Format("[StationID] = '%6.6s'", sSet.m_StationID);
			CSIGStationSet staSet(pDB);
			staSet.m_strFilter = _T(staQuery);
			staSet.Open();
			CString tmp = query;
			CString temp2;
			bool reCalc2016 = false;
			reCalc2016 = theApp.m_ForceNFDRS2016Recompute;
			COleDateTime n2Start, n2End;
			fpSet->GetNFDRS2016Range(staSet.m_StationID, &n2Start, &n2End);
			if (!reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0])) // we need hourly data for new calculator?
			{
				CWxSet tWxSet(pDB);
				tWxSet.m_strFilter.Format("[StationID] = '%6.6s' AND [DailyObs] = 1 AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
					n2Start.Format(), n2End.Format());
				tWxSet.Open();
				if (tWxSet.IsFieldNull(&tWxSet.m_FM1) || tWxSet.IsFieldNull(&tWxSet.m_FM10) || tWxSet.IsFieldNull(&tWxSet.m_FM100) || tWxSet.IsFieldNull(&tWxSet.m_FM1000) || tWxSet.IsFieldNull(&tWxSet.m_FuelTemperature)
					|| tWxSet.m_FM1 <= 0.0 || tWxSet.m_FM10 <= 0.0 || tWxSet.m_FM100 <= 0.0 || tWxSet.m_FM1000 <= 0.0 || tWxSet.m_FuelTemperature <= -999.0)
					reCalc2016 = true;
				if (!reCalc2016)
				{
					tWxSet.MoveLast();
					if (tWxSet.IsFieldNull(&tWxSet.m_FM1) || tWxSet.IsFieldNull(&tWxSet.m_FM10) || tWxSet.IsFieldNull(&tWxSet.m_FM100) || tWxSet.IsFieldNull(&tWxSet.m_FM1000) || tWxSet.IsFieldNull(&tWxSet.m_FuelTemperature)
						|| tWxSet.m_FM1 <= 0.0 || tWxSet.m_FM10 <= 0.0 || tWxSet.m_FM100 <= 0.0 || tWxSet.m_FM1000 <= 0.0 || tWxSet.m_FuelTemperature <= -999.0)
						reCalc2016 = true;
				}
				tWxSet.Close();
			}
			if (!useNFDRS)
				reCalc2016 = false;
			if (!reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0]))
				temp2.Format("[StationID] = '%6.6s' AND [DailyObs] = 1 AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
					n2Start.Format(), n2End.Format());
			else if (reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0]))
				temp2.Format("[StationID] = '%6.6s' AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
					n2Start.Format(), n2End.Format());
			else
				temp2.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", staSet.m_StationID);
			tmp += temp2;
			if(useNFDRS && isNFDRS2016(staSet.m_NFDRSFM[0]))
				tmp += m_strRequiredFields;

			wxSet = new CWxSet(pDB);
			wxSet->m_strSort = _T("[ObsDate]");
			wxSet->m_strFilter = _T(tmp);
			wxSet->Open();
			if ((!wxSet->IsBOF() && !wxSet->IsEOF()) && (!staSet.IsBOF() && !staSet.IsEOF()))
				ret = this->AnalyzeBatchStation(CString(sSet.m_StationID + " - " + staSet.m_Name), isFPA, fpSet->m_SIG_Station);
			wxSet->Close();
			staSet.Close();
			delete wxSet;
			wxSet = NULL;
			sSet.MoveNext();
			sLoc++;
		}
		catch (CDBException* e)
		{
			DisplayDBException(e);
			e->Delete();
			return 4;
		}
	}
	pCUPDUPData->SetProgress("Rebuilding Weather Observations for SIG");
	//ok, combine data into one periods structure....
	periods = tPeriods;

    // new FFP4.1  10/2012
	// temporary month storage structure
    // ... to reduce the number of GetMonth() calls
	int *tStartMonths = new int[numPeriods];
	int *tStartDays = new int[numPeriods];
	int *tEndDays = new int[numPeriods];

	for(i = 0; i < numPeriods; i++)
	{
         tStartMonths[i] = tSDates[i].GetMonth();
		 tStartDays[i] = tSDates[i].GetDay();
		 tEndDays[i] = tEDates[i].GetDay();
	}
	for(i = 0; i < numPeriods; i++)
	{
		for(int y = 0; y < eYear - sYear + 1; y++)
		{
			for(int v = 0; v < MAXVARIDS; v++)
			{
				if(periods[v])
				{
					double wSum = 0.0, value = 0.0;
					int p;
					for(p = 0; p < stations; p++)
					{
						if(SIGPeriods[p][v][i]->yStats[y].N() > 0)
						{
							value += weights[p] * SIGPeriods[p][v][i]->yStats[y].Mean();
							wSum += weights[p];
						}
					}
					if(wSum > 0.0) //has data
					{
						//get correct period
						d1.SetDate(sYear + y, sDates[i].GetMonth(), sDates[i].GetDay());
						// performance update 2012 - reduce number of Get () calls
						int d1M = d1.GetMonth(),
							d1D = d1.GetDay();
						for(p = 0; p < tNumPeriods; p++)
						{
							if(d1M == tStartMonths[p]
								&& d1D >= tStartDays[p]
								&& d1D <= tEndDays[p])
								break;
						}
						if(p >= tNumPeriods)
							p = tNumPeriods - 1;
						periods[v][p]->Accumulate(d1, value / wSum);
					}
				}
			}
		}
	}
	//clean up
	for(int p = 0; p < stations; p++)
	{
		for(int j = 0; j < MAXVARIDS; j++)
		{
			if(SIGPeriods[p][j])
			{
				for(int i = 0; i < numPeriods; i++)
					delete SIGPeriods[p][j][i];
				delete[] SIGPeriods[p][j];
			}
		}
		delete[] SIGPeriods[p];
	}
	delete[] SIGPeriods;
	delete[] weights;
	delete[] sDates;
	delete[] eDates;
	sDates = tSDates;
	eDates = tEDates;
	numPeriods = tNumPeriods;

	if(ret == 0)
	{
		//complete statistics calculation
		for(int j = 0; j < MAXVARIDS; j++)
		{
			if(periods[j])
				for(int i = 0; i < numPeriods; i++)
					periods[j][i]->Complete();
		}
	}
	sSet.Close();
	return ret;
}

int CClimAnalysis::AnalyzeBatchStation(CString stationStr, bool isFPA, CString sigID)
{
	int ret = 0;
	pCUPDUPData->SetProgress("Reading: " + stationStr);
	bool goodRecord;
	CSIGStationSet staSet(pDB);
	staSet.m_strFilter.Format("[StationID] = '%6.6s'", stationStr);//fpSet->m_SIG_Station);
	staSet.Open();

	bool reCalc2016 = false;
	reCalc2016 = theApp.m_ForceNFDRS2016Recompute;
	if (!reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0]))
	{//check wxSet for previously run NFDRS2016
		wxSet->Requery();
		//wxSet.m_strFilter.Format("([StationID] = '%s' AND [DailyObs] = 1)", m_sigStaID);
		if (wxSet->IsFieldNull(&wxSet->m_FM1) || wxSet->IsFieldNull(&wxSet->m_FM10) || wxSet->IsFieldNull(&wxSet->m_FM100) || wxSet->IsFieldNull(&wxSet->m_FM1000) || wxSet->IsFieldNull(&wxSet->m_FuelTemperature)
			|| wxSet->m_FM1 <= 0.0 || wxSet->m_FM10 <= 0.0 || wxSet->m_FM100 <= 0.0 || wxSet->m_FM1000 <= 0.0 || wxSet->m_FM1 <= -999.0)
			reCalc2016 = true;
		if (!reCalc2016)
		{
			wxSet->MoveLast();
			if (wxSet->IsFieldNull(&wxSet->m_FM1) || wxSet->IsFieldNull(&wxSet->m_FM10) || wxSet->IsFieldNull(&wxSet->m_FM100) || wxSet->IsFieldNull(&wxSet->m_FM1000) || wxSet->IsFieldNull(&wxSet->m_FuelTemperature)
				|| wxSet->m_FM1 <= 0.0 || wxSet->m_FM10 <= 0.0 || wxSet->m_FM100 <= 0.0 || wxSet->m_FM1000 <= 0.0 || wxSet->m_FM1 <= -999.0)
				reCalc2016 = true;
		}
	}

	CString  lfiStationID;

	if (sigID.GetLength() > 1)
	{
		// first, make sure LFIoptions has an entry for this SIG
		CLFISet lfiSet(pDB);
		lfiSet.m_strFilter.Format("[SIG_Station] = '%s'", sigID);
		lfiSet.Open();
		if(! lfiSet.IsEOF())
            lfiStationID = sigID;
		else
			lfiStationID = staSet.m_StationID;
		lfiSet.Close();
	} else
		lfiStationID = staSet.m_StationID;

	CLFIengine lfi(pDB, 0, lfiStationID), 
		lfiHerb(pDB, HerbParams, lfiStationID, staSet.m_HerbAnnual), 
		lfiWoody(pDB, WoodyParams, lfiStationID);

	//create data structures based on periods and selected outputs
	//NOW CREATE NFDRCALC MODELS NEEDED
	int sKBDI;
	if(!staSet.IsFieldNull(&staSet.m_StartKBDI))
		sKBDI = staSet.m_StartKBDI;

	if (useNFDRS || useLFI)
	{
		int lookGreen, lookFreeze;
		lookGreen = DayOfYear(staSet.m_GreenJulian);
		lookFreeze = DayOfYear(staSet.m_FreezeJulian);

		if (isNFDRS2016(staSet.m_NFDRSFM[0]) || useLFI)
		{ // new NFDRS2016 calc
			theApp.m_NFDRS2016.Init(staSet.m_LatDegrees, staSet.m_NFDRSFM[0], staSet.m_SlopeCls, staSet.m_AvgPrecip, true, true, staSet.m_HerbAnnual, staSet.m_KBDIThreshold);
			theApp.m_NFDRS2016.iSetFuelModel(staSet.m_NFDRSFM[0]);
			theApp.m_NFDRS2016.SetGSIParams(lfi.GetHerbMaxGSI(), lfi.GetHerbGreenup(), lfi.GetTminMin(), lfi.GetTminMax(), lfi.GetVPDMin(), lfi.GetVPDMax(), 
				lfi.GetDaylenMin(), lfi.GetDaylenMax(), lfi.GetMAPeriod(), lfi.IsUsingVPDAvg(), lfi.GetNumPrecipDays(), lfi.GetRTPcpMin(), lfi.GetRTPcpMax(), lfi.GetUseRTPrecip());
			theApp.m_NFDRS2016.SetHerbGSIparams(lfiHerb.GetHerbMaxGSI(), lfiHerb.GetHerbGreenup(), lfiHerb.GetTminMin(), lfiHerb.GetTminMax(), 
				lfiHerb.GetVPDMin(), lfiHerb.GetVPDMax(), lfiHerb.GetDaylenMin(), lfiHerb.GetDaylenMax(), lfiHerb.GetMAPeriod(), lfiHerb.IsUsingVPDAvg(), 
				lfiHerb.GetNumPrecipDays(), lfiHerb.GetRTPcpMin(), lfiHerb.GetRTPcpMax(), lfiHerb.GetUseRTPrecip(), lfiHerb.GetHerbMin(), lfiHerb.GetHerbMax());
			theApp.m_NFDRS2016.SetWoodyGSIparams(lfiWoody.GetWoodyMaxGSI(), lfiWoody.GetWoodyGreenup(), lfiWoody.GetTminMin(), lfiWoody.GetTminMax(), 
				lfiWoody.GetVPDMin(), lfiWoody.GetVPDMax(), lfiWoody.GetDaylenMin(), lfiWoody.GetDaylenMax(), lfiWoody.GetMAPeriod(), lfiWoody.IsUsingVPDAvg(), 
				lfiWoody.GetNumPrecipDays(), lfiWoody.GetRTPcpMin(), lfiWoody.GetRTPcpMax(), lfiWoody.GetUseRTPrecip(), lfiWoody.GetWoodyMin(), lfiHerb.GetWoodyMax());
			theApp.m_NFDRS2016.SetStartKBDI(staSet.m_StartKBDI);
			theApp.m_NFDRS2016.SetSCMax(staSet.GetSCM(staSet.m_NFDRSFM[0]));
			theApp.m_NFDRS2016.SetMxdHumid(staSet.GetMxHumid(staSet.m_NFDRSFM[0]));
		}
		if (!isNFDRS2016(staSet.m_NFDRSFM[0]))
		{   // "old" NFDRS calc
			theApp.m_NFDRS.iInitialize((staSet.m_Use88 && !isFPA) ? 88 : 78,//staSet.m_Use88 ? 88 : 78,
				staSet.m_ClimateCls, lookFreeze, lookGreen,
				(int)(staSet.m_LatDegrees), staSet.m_HerbAnnual, staSet.m_Deciduous, staSet.m_FM1Eq10,
				staSet.m_Start1000 - 5.0, staSet.m_Start1000, sKBDI, staSet.m_AvgPrecip);
			theApp.m_NFDRS.iSetFuelModel((!isFPA) ? staSet.m_NFDRSFM[0] : 'G', 0);
			theApp.OverrideMXD(&staSet);
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
	int  iRainEvent, iStage, iSC, iIC,
		iFIL, iBI,
		iKBDI, init = 1, iDeclareGreenUp = 0, iDeclareFreeze = 0;//,
	double f1, f10, f100, f1000, fHerb, fWood, fGren, fX1000, fROS, fERC, fFL,
		ffmc, dmc, dc, bui, isi, fwi, dsr;
	double fSC, fIC, fFIL, fBI;
	double pptAcc = 0;

	COleDateTime yesterday, wxDay;//used to skip duplicates
	wxSet->Requery();
	int recCount = 0;

	if(!wxSet->IsBOF())
	{
		//working variables
		int tmp, tmpMax, tmpMin, rh, rhMax, rhMin, pptDur, sow, greenHerb, greenShrub,
			season, windSpd, windDir, slopeClass, wetFlag, snowFlag;
		double pptAmt, omc10, lfiVal, lfiHerbVal, lfiWoodyVal;
		while(!wxSet->IsEOF() )
		{
			if (recCount % 20 == 0)
					if (pCUPDUPData->ShouldTerminate())
							break;
			recCount ++;

			if(((useNFDRS || useCanadian) && (wxSet->IsFieldNull(&wxSet->m_Temp) || wxSet->IsFieldNull(&wxSet->m_RH))
				|| wxSet->IsFieldNull(&wxSet->m_ObsDate)))
			{
				wxSet->MoveNext();//GetNext();
				continue;
			}
			wxDay.SetDateTime(wxSet->m_ObsDate.GetYear(), wxSet->m_ObsDate.GetMonth(), wxSet->m_ObsDate.GetDay(), wxSet->m_ObsDate.GetHour(),
				wxSet->m_ObsDate.GetMinute(), wxSet->m_ObsDate.GetSecond());
			if(!init && yesterday >= wxDay)
			{
				wxSet->MoveNext();//GetNext();
				continue;
			}

			// new custom init check    12/2006
			// if this is first record for new year, look for inits
			if (yesterday.GetYear() != wxDay.GetYear())
			{
                CCustomInitSet ciSet(fpSet->m_pDatabase);
				ciSet.m_strFilter.Format("[SIG_Station]='%s' AND [Year]=%d and [Enabled] <> 0", wxSet->m_StationID,wxDay.GetYear());
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
						customInits = true;
				   }
				   if(useCanadian)
				   {
						if (!ciSet.IsFieldNull(&ciSet.m_FFMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DMC) &&
							!ciSet.IsFieldNull(&ciSet.m_DC))
						{
		                   canadian.ReInitialize(ciSet.m_FFMC,ciSet.m_DMC,ciSet.m_DC);
						   usedCANcust = true;
						   customInits = true;
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

			yesterday = wxDay;//wxSet->m_ObsDate;

			tmp = wxSet->m_Temp;
			tmpMax = wxSet->IsFieldNull(&wxSet->m_TmpMax) ? -10000 : wxSet->m_TmpMax;
			tmpMin = wxSet->IsFieldNull(&wxSet->m_TmpMin) ? -10000 : wxSet->m_TmpMin;
			rh = max(wxSet->m_RH, 1);
			rhMax = wxSet->IsFieldNull(&wxSet->m_RHMax) ? -10000 : wxSet->m_RHMax;
			rhMin = wxSet->IsFieldNull(&wxSet->m_RHMin) ? -10000 : max(wxSet->m_RHMin, 1);
			pptDur = wxSet->IsFieldNull(&wxSet->m_PPTDUR) ? 0 : wxSet->m_PPTDUR;
			sow = wxSet->ResolveSOW();//IsFieldNull(&wxSet->m_SOW) ? 0 : wxSet->m_SOW;
			greenHerb = wxSet->IsFieldNull(&wxSet->m_GreenHerb) ? 0 : wxSet->m_GreenHerb;
			greenShrub = wxSet->IsFieldNull(&wxSet->m_GreenShrub) ? 0 : wxSet->m_GreenShrub;
			season = wxSet->IsFieldNull(&wxSet->m_Season) ? 0 : wxSet->m_Season;
			windSpd = wxSet->IsFieldNull(&wxSet->m_WS) ? 0 : wxSet->m_WS;
			windDir = wxSet->IsFieldNull(&wxSet->m_WDir) ? 0 : wxSet->m_WDir;
			slopeClass = staSet.m_SlopeCls;
			slopeClass = max(slopeClass, 1);
			slopeClass = min(slopeClass, 5);
			pptAmt = wxSet->IsFieldNull(&wxSet->m_PPTAMT) ? 0.0 : wxSet->m_PPTAMT;
			//omc10 = wxSet->IsFieldNull(&wxSet->m_OMC10) ? -1000.0 : wxSet->m_OMC10;
			omc10 = (bUseStick && !wxSet->IsFieldNull(&wxSet->m_OMC10)) ? wxSet->m_OMC10 : -1000.0;
			wetFlag = wxSet->ResolveWetFlag();//IsFieldNull(&wxSet->m_WetFlag) ? 0 : wxSet->m_WetFlag;
			snowFlag = wxSet->IsFieldNull(&wxSet->m_SnowFlag) ? 0 : wxSet->m_SnowFlag;
			int year1 = wxSet->m_ObsDate.GetYear();
			int month1 = wxSet->m_ObsDate.GetMonth();
			int day1 = wxSet->m_ObsDate.GetDay();
			int hour1 = wxSet->m_ObsDate.GetHour();
			int julian1 = wxSet->m_ObsDate.GetDayOfYear();
			bool isDailyObs = wxSet->IsFieldNull(&wxSet->m_DailyObs) ? false : wxSet->m_DailyObs;


			double hourlyPrecip = wxSet->IsFieldNull(&wxSet->m_HourlyPrecip) ? 0 : wxSet->m_HourlyPrecip;
			double solarRad = wxSet->IsFieldNull(&wxSet->m_SolarRadiation) ? 0 : wxSet->m_SolarRadiation;

			//calculate model outputs
			//NFDR 78
			if (useNFDRS || useLFI)
			{
				if (isNFDRS2016(staSet.m_NFDRSFM[0]) || useLFI)
				{ // new NFDRS2016 calc
					if (reCalc2016)
						theApp.m_NFDRS2016.Update(year1, month1, day1, hour1, julian1, (double)tmp, (double)tmpMin, 
						(double)tmpMax, (double)rh, (double)rhMin, hourlyPrecip, pptAmt, solarRad, 
							(double)windSpd, snowFlag, (isDailyObs) ? hour1 : -1);//staSet.m_RegSchdObs);
					else if (wxSet->m_DailyObs)
					{
						theApp.m_NFDRS2016.UpdateDaily(year1, month1, day1, julian1, (double)tmp, (double)tmpMin, (double)tmpMax, (double)rh, (double)rhMin,
							pptAmt, (double)windSpd, wxSet->m_FM1, wxSet->m_FM10, wxSet->m_FM100, wxSet->m_FM1000, wxSet->m_FuelTemperature, snowFlag);
					}
					else
					{
						wxSet->MoveNext();
						continue;
					}
					pptAcc += hourlyPrecip;
					f1 = theApp.m_NFDRS2016.MC1;
					f10 = theApp.m_NFDRS2016.MC10;
					f100 = theApp.m_NFDRS2016.MC100;
					f1000 = theApp.m_NFDRS2016.MC1000;
					lfiHerbVal = fHerb = theApp.m_NFDRS2016.MCHERB;
					lfiWoodyVal = fWood = theApp.m_NFDRS2016.MCWOOD;
					lfiVal = theApp.m_NFDRS2016.m_GSI;
					fX1000 = 0; // fixme
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

					iFIL = (int)fFIL;
					iBI = (int)fBI;
					iIC = (int)fIC;
					iSC = (int)fSC;
					


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
						DayOfYear(wxSet->m_ObsDate),
						wxSet->m_ObsDate.GetYear(),
						iDeclareGreenUp,
						iDeclareFreeze,
						greenHerb,
						greenShrub,
						season,
						wetFlag,
						snowFlag,
						&f1, &f10, &f100, &f1000, &fHerb, &fWood, &iStage, &fGren, &fX1000, &iRainEvent, &iKBDI);
					theApp.m_NFDRS.iCalcIndexes(windSpd, slopeClass, &fROS, &iSC, &fERC, &fFL, &iFIL, &iBI);
					theApp.m_NFDRS.iCalcIC(tmp, sow, f1, iSC, &iIC);

					fSC = (double)iSC;
					fFIL = (double)iFIL;
					fBI = (double)iBI;
					fIC = (double)iIC;

				}
			}
			//CANADIAN
			if(useCanadian)
				canadian.CalcIndices(DayOfYear(wxSet->m_ObsDate), wxSet->m_ObsDate.GetMonth(), wxSet->m_ObsDate.GetYear(),
					tmp, rh, windSpd * 1.15, pptAmt,
					&ffmc, &dmc, &dc, &bui, &isi, &fwi, &dsr);
			//int per = GetPeriod(wxSet->m_ObsDate);
			//calculate LFI here (maintain continuity)

			int per = GetPeriod(wxDay);
			if(per >= 0)
			{
				double val = 0.0;
				for(int j = 0; j < MAXVARIDS; j++)
				{
					if(periods[j])
					{
						goodRecord = true;
						switch(j + 1)
						{
						case 1:
							if(!wxSet->IsFieldNull(&wxSet->m_Temp))
								val = (double)wxSet->m_Temp;
							else
								goodRecord = false;
							break;
						case 2:
							if(!wxSet->IsFieldNull(&wxSet->m_TmpMax) && !wxSet->IsFieldNull(&wxSet->m_TmpMin))
								val = (double)(wxSet->m_TmpMax + wxSet->m_TmpMin) / 2.0;
							else
								goodRecord = false;
							break;
						case 3:
							if(!wxSet->IsFieldNull(&wxSet->m_TmpMin))
								val = (double)wxSet->m_TmpMin;
							else
								goodRecord = false;
							break;
						case 4:
							if(!wxSet->IsFieldNull(&wxSet->m_TmpMax))
								val = (double)wxSet->m_TmpMax;
							else
								goodRecord = false;
							break;
						case 5:
							if(!wxSet->IsFieldNull(&wxSet->m_RH))
								val = (double)max(wxSet->m_RH, 1);
							else
								goodRecord = false;
							break;
						case 6:
							if(!wxSet->IsFieldNull(&wxSet->m_RHMax) && !wxSet->IsFieldNull(&wxSet->m_RHMin))
								val = (double)(wxSet->m_RHMax + max(wxSet->m_RHMin, 1)) / 2.0;
							else
								goodRecord = false;
							break;
						case 7:
							if(!wxSet->IsFieldNull(&wxSet->m_RHMin))
								val = (double)max(wxSet->m_RHMin, 1);
							else
								goodRecord = false;
							break;
						case 8:
							if(!wxSet->IsFieldNull(&wxSet->m_RHMax))
								val = (double)wxSet->m_RHMax;
							else
								goodRecord = false;
							break;
						case 9:
							if(!wxSet->IsFieldNull(&wxSet->m_PPTAMT))
								val = (double)wxSet->m_PPTAMT;
							else
								goodRecord = false;
							break;
						case 10:
							if(!wxSet->IsFieldNull(&wxSet->m_PPTDUR))
								val = (double)wxSet->m_PPTDUR;
							else
								goodRecord = false;
							break;
						case 11:
							if(!wxSet->IsFieldNull(&wxSet->m_WS))// && !wxSet->IsFieldNull(&wxSet->m_WDir))
								val = (double)wxSet->m_WS;
							else
								goodRecord = false;
							break;
						case 12:
							val = (int)fSC;
							break;
						case 13:
							val = fERC;
							break;
						case 14:
							val = (int)fBI;
							break;
						case 15:
							val = (double)iKBDI;
							break;
						case 16: //Ignition component
							val = (int)fIC;
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
							if (!wxSet->IsFieldNull(&wxSet->m_SnowFlag))
								val = (double)wxSet->m_SnowFlag;
							else
								goodRecord = false;
							break;
						case 24:
							val = windDir;
							break;
						case 25:
							val = sow;
							//val = iFIL;
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
							if(!wxSet->IsFieldNull(&wxSet->m_GustDir))
								val = (double)wxSet->m_GustDir;
							else
								goodRecord = false;
							break;
						case 34:
							if(!wxSet->IsFieldNull(&wxSet->m_GustSpd))
								val = (double)wxSet->m_GustSpd;
							else
								goodRecord = false;
							break;
						case 35:
							if(!wxSet->IsFieldNull(&wxSet->m_SolarRadiation))
								val = (double)wxSet->m_SolarRadiation;
							else
								goodRecord = false;
							break;
						case 36:
							if(!wxSet->IsFieldNull(&wxSet->m_WetFlag))
								val = (double)wxSet->m_WetFlag;
							else
								goodRecord = false;
							break;
						case 37://Dew Point
							if(!wxSet->IsFieldNull(&wxSet->m_RH) && !wxSet->IsFieldNull(&wxSet->m_Temp))
								val = CalcDPT(wxSet->m_Temp, max(wxSet->m_RH, 1));
							else
								goodRecord = false;
							break;
						case 38://Vapor pressure deficit MAX
							if(!wxSet->IsFieldNull(&wxSet->m_RHMin) && !wxSet->IsFieldNull(&wxSet->m_TmpMax))
								val = CalcVPD(max(wxSet->m_RHMin, 1), wxSet->m_TmpMax);
							else
								goodRecord = false;
							break;
						case 39://Vapor pressure deficit Avg
							if(!wxSet->IsFieldNull(&wxSet->m_Temp) && !wxSet->IsFieldNull(&wxSet->m_RH) && !wxSet->IsFieldNull(&wxSet->m_TmpMin) && !wxSet->IsFieldNull(&wxSet->m_TmpMax))
							{
								double dpt = CalcDPT(wxSet->m_Temp, max(wxSet->m_RH, 1));
								double avgT = (wxSet->m_TmpMin + wxSet->m_TmpMax) / 2;
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
						case 41:
							if(!wxSet->IsFieldNull(&wxSet->m_WAzimuth))
								val = (double)wxSet->m_WAzimuth;
							else
								goodRecord = false;
							break;
						case 42:
							if(!wxSet->IsFieldNull(&wxSet->m_HourlyPrecip))
								val = (double)wxSet->m_HourlyPrecip;
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
							if(!wxSet->IsFieldNull(&wxSet->m_Temp) && !wxSet->IsFieldNull(&wxSet->m_RH)
									&& !wxSet->IsFieldNull(&wxSet->m_WS))
								val = theApp.m_NFDRS.iCalcFFWI(wxSet->m_Temp, max(wxSet->m_RH, 1), wxSet->m_WS);
							else
								goodRecord = false;
							break;
						default:
							goodRecord = false;
						}
						if(goodRecord)
						{
								//periods[j][per]->Accumulate(wxSet->m_ObsDate, val);
								periods[j][per]->Accumulate(wxDay, val);
						}
					}
				}
			}
			count++;
			init = 0;
			wxSet->MoveNext();//GetNext();
		}
	}
	//process user variables
	if(HasUserVars())
		userCount += AccumulateStationUserVars(stationStr);
	if(m_strModel.GetLength() <= 0)
		m_strModel = GetModelString(&staSet);
	staSet.Close();
	if(pCUPDUPData->ShouldTerminate())
	{
		return -1;
	}
	for(int j = 0; j < MAXVARIDS; j++)
	{
		if(periods[j])
		{
			for(int i = 0; i < numPeriods; i++)
				periods[j][i]->Complete();
		}
	}
	if(useNFDRS)
	{
		CString sfName;
		sfName.Format("%s\\%6.6s.nfd", theApp.dbDir, stationStr);
		theApp.m_NFDRS.SaveState(sfName);
	}
	if(useCanadian)
		canadian.SaveState(&canadaState);
	return ret;
}

//CString CClimAnalysis::FlamMapExport(bool createDoc/* = true*/, int calmVal /*= 1*/, int startHour /*= 0*/,
//									 int startMinute /*= 0*/, int endHour/* = 0*/, int endMinute /*= 0*/, int windType /*= 0*/)
class ERCMoist
{
public:
	double ERC;
	double fm1;
	double fm10;
	double fm100;
	double fm1000;
	double fmHerb;
	double fmWood;
	double fmX1000;
};

int ercMoistCompare( const void *arg1, const void *arg2 )
{
   /* Compare the values, starting with ERC then all the moistures */
	ERCMoist *erc1, *erc2;
	erc1 = (ERCMoist *) arg1;
	erc2 = (ERCMoist *) arg2;
	if(erc1->ERC > erc2->ERC)
		return 1;
	if(erc1->ERC < erc2->ERC)
		return -1;
	//erc's equal, look at moistures....
	//condition is such that for equal ERC's, higher moisture value is considered less than
	//by golly they're equal
	return 0;
}

void GetWindSpeedsByDirections(CString staID, CDatabase *pDB, 
	int startYear, int startMonth, int startDay, int startHour, int startMinute,
	int endYear, int endMonth, int endDay, int endHour, int endMinute,
	int windType,
	std::vector<int> dirSpeeds[8],
	std::vector<int> monthlyDirSpeeds[12][8],
	double wrData[WRDIRS][WRSPEEDS],
	long *windDays,
	long *windRecsUsed,
	long *windDaysInQuery)
{
	CString query = "", temp, staQuery;
	int dOrder = DOY_NORMAL;
	COleDateTime d1(2000, startMonth, startDay, 0, 0, 0), d2(2000, endMonth, endDay, 0, 0, 0);
	COleDateTimeSpan day(1, 0, 0, 0);
	if (d1 == d2)// || d2 + day == d1)
		dOrder = DOY_EQUAL;
	else if (d1 > d2)
		dOrder = DOY_FLIPPED;
	query.Format("[StationID] = '%6.6s' and Year([ObsDate]) >= %d and Year([ObsDate]) <= %d",
		staID, startYear, endYear);
	switch (dOrder)
	{
	case DOY_FLIPPED:
	{
		//END DOY EARLIER THAN START DOY
		temp.Format(" and ((Month([ObsDate]) > %d or Month([ObsDate]) < %d)",
			startMonth, endMonth);
		query += temp;
		if (startMonth == endMonth)
		{
			temp.Format(" or (Month([ObsDate]) = %d and Day([ObsDate]) >= %d"
				" or Day([ObsDate]) <= %d))",
				startMonth, startDay, endDay);
		}
		else
		{
			temp.Format(
				" or (Month([ObsDate]) = %d and Day([ObsDate]) >= %d)"
				" or (Month([ObsDate]) = %d and Day([ObsDate]) <= %d))",
				startMonth, startDay,
				endMonth, endDay);
		}
		query += temp;
	}
	break;
	case DOY_EQUAL:
	default: //DOY NORMAL
		if (startMonth == endMonth)
		{
			temp.Format(" and Month([ObsDate]) = %d and Day([ObsDate]) >= %d"
				" and Day([ObsDate]) <= %d",
				startMonth, startDay, endDay);
		}
		else
		{
			temp.Format(" and ((Month([ObsDate]) > %d and Month([ObsDate]) < %d)",
				startMonth, endMonth);
			query += temp;
			temp.Format(
				" or (Month([ObsDate]) = %d and Day([ObsDate]) >= %d)"
				" or (Month([ObsDate]) = %d and Day([ObsDate]) <= %d))",
				startMonth, startDay,
				endMonth, endDay);
		}
		query += temp;
	}
	CWxSet twxSet(pDB);
	twxSet.m_strSort = _T("[ObsDate]");
	twxSet.m_strFilter = query;//.Format("(%s) %s", query, temp);
	twxSet.Open();
	bool firstRec = true, flipped = false, allTimes = false;
	if ((startHour > endHour) || (startHour == endHour && startMinute > endMinute))
		flipped = true;
	if (startHour == endHour && startMinute == endMinute)
		allTimes = true;
	COleDateTime lastTime, lastDay;
	CWaitCursor wait;
	while (!twxSet.IsEOF())
	{
		int spd = -1, azi = -1, dir = -1, gSpd = -1, gDir = -1, hr, mn;
		if (!firstRec && twxSet.m_ObsDate <= lastTime)//check for duplicates
		{
			twxSet.MoveNext();
			continue;
		}
		if (!allTimes)//check for valid hour/min
		{
			hr = twxSet.m_ObsDate.GetHour();
			mn = twxSet.m_ObsDate.GetMinute();
			if (!flipped)
			{
				if ((hr < startHour || hr > endHour)
					|| (hr == startHour && mn < startMinute)
					|| (hr == endHour && mn > endMinute))
				{
					twxSet.MoveNext();
					continue;
				}
			}
			else//flipped (valid time spans midnight)
			{
				if ((hr < startHour && hr > endHour)
					|| (hr == startHour && mn < startMinute)
					|| (hr == endHour && mn > endMinute))
				{
					twxSet.MoveNext();
					continue;
				}
			}
		}
		//on a record we want, grab data
		if (!twxSet.IsFieldNull(&twxSet.m_WAzimuth))
			azi = twxSet.m_WAzimuth;
		if (!twxSet.IsFieldNull(&twxSet.m_WDir))//check categorical as fallback direction
			dir = twxSet.m_WDir;
		if (!twxSet.IsFieldNull(&twxSet.m_GustDir))
			gDir = twxSet.m_GustDir;
		if (!twxSet.IsFieldNull(&twxSet.m_GustSpd))
			gSpd = twxSet.m_GustSpd;
		if (!twxSet.IsFieldNull(&twxSet.m_WS))
			spd = twxSet.m_WS;
		bool recUsed = false;
		if (windType == 0 || windType == 2)//include 10 minute averages
		{
			int wrS = -1, wrD = -1;
			int d = -1;
			if(spd >= 0)
			{
				//windrose speeds
				if (spd < 4)
					wrS = 1;
				else if (spd < 8)
					wrS = 2;
				else if (spd < 13)
					wrS = 3;
				else if (spd < 19)
					wrS = 4;
				else if (spd < 25)
					wrS = 5;
				else if (spd < 32)
					wrS = 6;
				else if (spd < 39)
					wrS = 7;
				else if (spd < 47)
					wrS = 8;
				else
					wrS = 9;
				if (azi >= 0 && azi <= 360)//valid azimuth, use it!
				{
					//windrose dir
					wrD = (int)((azi + 11.25) / 22.5);
					if (wrD >= WRDIRS)
						wrD = 0;
					//{//convert dir to 1 - 8 scale
					if (azi > 337 || azi < 23)
						d = 8;
					else if (azi > 22 && azi < 68)
						d = 1;
					else if (azi > 67 && azi < 113)
						d = 2;
					else if (azi > 112 && azi < 158)
						d = 3;
					else if (azi > 157 && azi < 203)
						d = 4;
					else if (azi > 202 && azi < 248)
						d = 5;
					else if (azi > 247 && azi < 293)
						d = 6;
					else if (azi > 292 && azi < 338)
						d = 7;
					d--;
				}
				else if (dir >= 0)//fall back to dir category...
				{
					d = dir;
					if (dir > 0)
						d--;
					//windrose Dir
					wrD = dir * 2 - 2;
					if (wrD == -1)
						wrD = 0;
				}
				if (spd >= 0 && d >= 0)
				{
					if (d >= 0 && d < 8)//toss bad data
					{
						dirSpeeds[d].push_back(spd);
						//add monthly bins
						int m = twxSet.m_ObsDate.GetMonth() - 1;
						monthlyDirSpeeds[m][d].push_back(spd);
					}
				}
				if (wrD >= 0 && wrS >= 0)
				{
					wrData[wrD][wrS] += 1;
					recUsed = true;
					// 2014 change - count WS and GS as separate observations
					*windRecsUsed += 1;
				}
			}
		}
		if (windType == 1 || windType == 2)//include gusts
		{
			int wrS = -1, wrD = -1;
			int d = -1;
			if (gSpd >= 0)
			{
				//windrose speeds
				if (spd < 4)
					wrS = 1;
				else if (spd < 8)
					wrS = 2;
				else if (spd < 13)
					wrS = 3;
				else if (spd < 19)
					wrS = 4;
				else if (spd < 25)
					wrS = 5;
				else if (spd < 32)
					wrS = 6;
				else if (spd < 39)
					wrS = 7;
				else if (spd < 47)
					wrS = 8;
				else
					wrS = 9;
				if (gDir >= 0 && gDir <= 360)//valid azimuth, use it!
				{
					wrD = (int)((gDir + 11.25) / 22.5);
					if (wrD >= WRDIRS)
						wrD = 0;
					//{//convert dir to 1 - 8 scale
					if (gDir > 337 || gDir < 23)
						d = 8;
					else if (gDir > 22 && gDir < 68)
						d = 1;
					else if (gDir > 67 && gDir < 113)
						d = 2;
					else if (gDir > 112 && gDir < 158)
						d = 3;
					else if (gDir > 157 && gDir < 203)
						d = 4;
					else if (gDir > 202 && gDir < 248)
						d = 5;
					else if (gDir > 247 && gDir < 293)
						d = 6;
					else if (gDir > 292 && gDir < 338)
						d = 7;
					d--;
				}
				else if (dir >= 0)//fall back to dir category...
				{
					d = gDir;
					if (dir > 0)
						d--;
					wrD = dir * 2 - 2;
					if (wrD == -1)
						wrD = 0;
				}
				if (gSpd >= 0 && d >= 0)
				{
					if (d >= 0 && d < 8)//toss bad data
					{
						dirSpeeds[d].push_back(gSpd);
						//add monthly bins
						int m = twxSet.m_ObsDate.GetMonth() - 1;
						monthlyDirSpeeds[m][d].push_back(gSpd);
					}
				}
				if (wrD >= 0 && wrS >= 0)
				{
					wrData[wrD][wrS] += 1;
					recUsed = true;
					// 2014 change - count WS and GS as separate observations
					*windRecsUsed += 1;
				}
			}
		}


		lastTime = twxSet.m_ObsDate;
		lastDay = lastTime;
		firstRec = false;
		twxSet.MoveNext();
	}
	twxSet.Close();

	if (dOrder != DOY_FLIPPED)
	{
		COleDateTime yearStart, yearEnd;
		COleDateTimeSpan daysSpan;
		for (int y = startYear; y <= endYear; y++)//for each year
		{
			yearStart.SetDate(y, startMonth, startDay);
			yearEnd.SetDate(y, endMonth, endDay);
			daysSpan = yearEnd - yearStart;
			*windDaysInQuery += daysSpan.GetDays() + 1;
		}
	}
	else
	{
		COleDateTime yearStart1, yearEnd1, yearStart2, yearEnd2;
		COleDateTimeSpan daysSpan;
		for (int y = startYear; y <= endYear; y++)//for each year
		{
			yearStart1.SetDate(y, 1, 1);
			yearEnd1.SetDate(y, endMonth, endDay);
			daysSpan = yearEnd1 - yearStart1;
			*windDaysInQuery += daysSpan.GetDays() + 1;
			yearStart2.SetDate(y, startMonth, startDay);
			yearEnd2.SetDate(y, 12, 31);
			daysSpan = yearEnd2 - yearStart2;
			*windDaysInQuery += daysSpan.GetDays() + 1;
		}
	}
}


void AddStationWinds(CString staID, CDatabase *pDB, long bins[6][8], long monthlyBins[12][6][8], long *monthlyCalms, long *numObs, long *numCalm,
					 int calmVal, int startYear,
		int startMonth, int startDay, int startHour, int startMinute,
		int endYear, int endMonth, int endDay, int endHour, int endMinute,
		int windType,
		double wrData[WRDIRS][WRSPEEDS],
		long *windDays,
		long *windRecsUsed,
		long *windDaysInQuery)
{
	long queryDay = 0, dataDays = 0;
	CString query = "", temp, staQuery;
	int dOrder = DOY_NORMAL;
	COleDateTime d1(2000, startMonth, startDay, 0, 0, 0), d2(2000, endMonth, endDay, 0, 0, 0);
	COleDateTimeSpan day(1, 0, 0, 0 );
	if(d1 == d2)// || d2 + day == d1)
		dOrder = DOY_EQUAL;
	else if(d1 > d2)
		dOrder = DOY_FLIPPED;
	query.Format("[StationID] = '%6.6s' and Year([ObsDate]) >= %d and Year([ObsDate]) <= %d",
		staID, startYear, endYear);
	switch(dOrder)
	{
		case DOY_FLIPPED:
		{
			//END DOY EARLIER THAN START DOY
			temp.Format(" and ((Month([ObsDate]) > %d or Month([ObsDate]) < %d)",
				startMonth, endMonth);
			query += temp;
			if(startMonth == endMonth)
			{
				temp.Format(" or (Month([ObsDate]) = %d and Day([ObsDate]) >= %d"
					" or Day([ObsDate]) <= %d))",
				startMonth, startDay, endDay);
			}
			else
			{
				temp.Format(
					" or (Month([ObsDate]) = %d and Day([ObsDate]) >= %d)"
					" or (Month([ObsDate]) = %d and Day([ObsDate]) <= %d))",
					startMonth, startDay,
					endMonth, endDay);
			}
			query += temp;
		}
			break;
		case DOY_EQUAL:
		default: //DOY NORMAL
			if(startMonth == endMonth)
			{
				temp.Format(" and Month([ObsDate]) = %d and Day([ObsDate]) >= %d"
					" and Day([ObsDate]) <= %d",
				startMonth, startDay, endDay);
			}
			else
			{
				temp.Format(" and ((Month([ObsDate]) > %d and Month([ObsDate]) < %d)",
					startMonth, endMonth);
				query += temp;
				temp.Format(
					" or (Month([ObsDate]) = %d and Day([ObsDate]) >= %d)"
					" or (Month([ObsDate]) = %d and Day([ObsDate]) <= %d))",
					startMonth, startDay,
					endMonth, endDay);
			}
			query += temp;
	}
	CWxSet twxSet(pDB);
	twxSet.m_strSort = _T("[ObsDate]");
	twxSet.m_strFilter = query;//.Format("(%s) %s", query, temp);
	twxSet.Open();
	bool firstRec = true, flipped = false, allTimes = false;
	if((startHour > endHour) || (startHour == endHour && startMinute > endMinute))
		flipped = true;
	if(startHour == endHour && startMinute == endMinute)
		allTimes = true;
	COleDateTime lastTime, lastDay;
	CWaitCursor wait;
	while(!twxSet.IsEOF())
	{
		int spd = -1, azi = -1, dir = -1, gSpd = -1, gDir = -1, hr, mn;
		if(!firstRec && twxSet.m_ObsDate <= lastTime)//check for duplicates
		{
			twxSet.MoveNext();
			continue;
		}
		if(!allTimes)//check for valid hour/min
		{
			hr = twxSet.m_ObsDate.GetHour();
			mn = twxSet.m_ObsDate.GetMinute();
			if(!flipped)
			{
				if((hr < startHour || hr > endHour)
					|| (hr == startHour && mn < startMinute)
					|| (hr == endHour && mn > endMinute))
				{
					twxSet.MoveNext();
					continue;
				}
			}
			else//flipped (valid time spans midnight)
			{
				if((hr < startHour && hr > endHour)
					|| (hr == startHour && mn < startMinute)
					|| (hr == endHour && mn > endMinute))
				{
					twxSet.MoveNext();
					continue;
				}
			}
		}
		//on a record we want, grab data
		if(!twxSet.IsFieldNull(&twxSet.m_WAzimuth))
			azi = twxSet.m_WAzimuth;
		if(!twxSet.IsFieldNull(&twxSet.m_WDir))//check categorical as fallback direction
			dir = twxSet.m_WDir;
		if(!twxSet.IsFieldNull(&twxSet.m_GustDir))
			gDir = twxSet.m_GustDir;
		if(!twxSet.IsFieldNull(&twxSet.m_GustSpd))
			gSpd = twxSet.m_GustSpd;
		if(!twxSet.IsFieldNull(&twxSet.m_WS))
			spd = twxSet.m_WS;
		bool recUsed = false;
		if(windType == 0 || windType == 2)//include 10 minute averages
		{
			int wrS = -1, wrD = -1;
			int s = -1, d = -1;
			if(spd >= 0 && calmVal > spd)
			{
				*numCalm += 1;
				*numObs += 1;
				recUsed = true;
				// 2014 change - count WS and GS as separate observations
				*windRecsUsed += 1;
				//add monthly calms
				int m = twxSet.m_ObsDate.GetMonth() - 1;
				monthlyCalms[m]++;
			}
			else// if(spd > calmVal)
			{
				//windrose speeds
				if(spd < 4)
					wrS = 1;
				else if(spd < 8)
					wrS = 2;
				else if(spd < 13)
					wrS = 3;
				else if(spd < 19)
					wrS = 4;
				else if(spd < 25)
					wrS = 5;
				else if(spd < 32)
					wrS = 6;
				else if(spd < 39)
					wrS = 7;
				else if(spd < 47)
					wrS = 8;
				else
					wrS = 9;
				//frisk speeds
				if(spd < 5)
					s = 0;
				else if(spd < 10)
					s = 1;
				else if(spd < 15)
					s = 2;
				else if(spd < 20)
					s = 3;
				else if(spd < 25)
					s = 4;
				else
					s = 5;
				if(azi >= 0 && azi <= 360)//valid azimuth, use it!
				{
					//windrose dir
						wrD = (int) ((azi + 11.25) / 22.5);
						if(wrD >= WRDIRS)
							wrD = 0;
					//{//convert dir to 1 - 8 scale
						if(azi > 337 || azi < 23)
							d = 8;
						else if(azi > 22 && azi < 68)
							d = 1;
						else if(azi > 67 && azi < 113)
							d = 2;
						else if(azi > 112 && azi < 158)
							d = 3;
						else if(azi > 157 && azi < 203)
							d = 4;
						else if(azi > 202 && azi < 248)
							d = 5;
						else if(azi > 247 && azi < 293)
							d = 6;
						else if(azi > 292 && azi < 338)
							d = 7;
						d--;
				}
				else if(dir >= 0)//fall back to dir category...
				{
					d = dir;
					if(dir > 0)
						d--;
					//windrose Dir
					wrD = dir * 2 - 2;
					if(wrD == -1)
						wrD = 0;
				}
				if(s >= 0 && d >= 0)
				{
					if(d >= 0 && d < 8)//toss bad data
					{
						bins[s][d]++;
						*numObs += 1;
						//add monthly bins
						int m = twxSet.m_ObsDate.GetMonth() - 1;
						monthlyBins[m][s][d]++;
					}
				}
				if(wrD >= 0 && wrS >= 0)
				{
					wrData[wrD][wrS] += 1;
					recUsed = true;
					// 2014 change - count WS and GS as separate observations
					*windRecsUsed += 1;
				}
			}
		}
		if(windType == 1 || windType == 2)//include gusts
		{
			int wrS = -1, wrD = -1;
			int s = -1, d = -1;
			if(gSpd >= 0 && gSpd <= calmVal)
			{
				*numCalm += 1;
				*numObs += 1;
				//add monthly calms
				recUsed = true;
				// 2014 change - count WS and GS as separate observations
				*windRecsUsed += 1;
				int m = twxSet.m_ObsDate.GetMonth() - 1;
				monthlyCalms[m]++;
			}
			else if(gSpd > calmVal)
			{
				//windrose speeds
				if(spd < 4)
					wrS = 1;
				else if(spd < 8)
					wrS = 2;
				else if(spd < 13)
					wrS = 3;
				else if(spd < 19)
					wrS = 4;
				else if(spd < 25)
					wrS = 5;
				else if(spd < 32)
					wrS = 6;
				else if(spd < 39)
					wrS = 7;
				else if(spd < 47)
					wrS = 8;
				else
					wrS = 9;
				if(gSpd < 5)
					s = 0;
				else if(gSpd < 10)
					s = 1;
				else if(gSpd < 15)
					s = 2;
				else if(gSpd < 20)
					s = 3;
				else if(gSpd < 25)
					s = 4;
				else
					s = 5;
				if(gDir >= 0 && gDir <= 360)//valid azimuth, use it!
				{
						wrD = (int) ((gDir + 11.25) / 22.5);
						if(wrD >= WRDIRS)
							wrD = 0;
					//{//convert dir to 1 - 8 scale
						if(gDir > 337 || gDir < 23)
							d = 8;
						else if(gDir > 22 && gDir < 68)
							d = 1;
						else if(gDir > 67 && gDir < 113)
							d = 2;
						else if(gDir > 112 && gDir < 158)
							d = 3;
						else if(gDir > 157 && gDir < 203)
							d = 4;
						else if(gDir > 202 && gDir < 248)
							d = 5;
						else if(gDir > 247 && gDir < 293)
							d = 6;
						else if(gDir > 292 && gDir < 338)
							d = 7;
						d--;
				}
				else if(dir >= 0)//fall back to dir category...
				{
					d = gDir;
					if(dir > 0)
						d--;
					wrD = dir * 2 - 2;
					if(wrD == -1)
						wrD = 0;
				}
				if(s >= 0 && d >= 0)
				{
					if(d >= 0 && d < 8)//toss bad data
					{
						bins[s][d]++;
						*numObs += 1;
						//add monthly bins
						int m = twxSet.m_ObsDate.GetMonth() - 1;
						monthlyBins[m][s][d]++;
					}
				}
				if(wrD >= 0 && wrS >= 0)
				{
					wrData[wrD][wrS] += 1;
					recUsed = true;
					// 2014 change - count WS and GS as separate observations
					*windRecsUsed += 1;
				}
			}
		}


		lastTime = twxSet.m_ObsDate;
		if(firstRec || (!firstRec && lastDay.GetDayOfYear() != lastTime.GetDayOfYear()))
		{
			//COleDateTimeSpan span = lastTime - lastDay;
			*windDays += 1;
			//*windDaysInQuery += span.GetDays();
		}
		//now calc possible days in query...
		lastDay = lastTime;
		firstRec = false;
		twxSet.MoveNext();
	}
	twxSet.Close();
		if(dOrder != DOY_FLIPPED)
		{
			COleDateTime yearStart, yearEnd;
			COleDateTimeSpan daysSpan;
			for(int y = startYear; y <= endYear; y++)//for each year
			{
				yearStart.SetDate(y, startMonth, startDay);
				yearEnd.SetDate(y, endMonth, endDay);
				daysSpan = yearEnd - yearStart;
				*windDaysInQuery += daysSpan.GetDays() + 1;
			}
		}
		else
		{
			COleDateTime yearStart1, yearEnd1, yearStart2, yearEnd2;
			COleDateTimeSpan daysSpan;
			for(int y = startYear; y <= endYear; y++)//for each year
			{
				yearStart1.SetDate(y, 1, 1);
				yearEnd1.SetDate(y, endMonth, endDay);
				daysSpan = yearEnd1 - yearStart1;
				*windDaysInQuery += daysSpan.GetDays() + 1;
				yearStart2.SetDate(y, startMonth, startDay);
				yearEnd2.SetDate(y, 12, 31);
				daysSpan = yearEnd2 - yearStart2;
				*windDaysInQuery += daysSpan.GetDays() + 1;
			}
		}
}


void BinObs(std::vector<int> vObs, int binSize, int nBins, int calmVal, int *bins, int *calms)
{
	int windSpeed, bin;
	//for (int b = 0; b < nBins; b++)
	//	bins[b] = 0;
	std::vector<int>::iterator it;
	for (it = vObs.begin(); it != vObs.end(); ++it)
	{
		windSpeed = *it;
		if (windSpeed <= calmVal)
			*calms = *calms + 1;
		else
		{
			bin = windSpeed / binSize;
			if (bin >= nBins)
				bin = nBins - 1;
			bins[bin]++;
		}
	}
}

CString CClimAnalysis::FlamMapExport(bool createDoc /*= true*/, int calmVal /*= 1*/, int startYear /*= 1900*/,
		int startMonth /*= 1*/, int startDay /*= 1*/, int startHour /*= 0*/, int startMinute /*= 0*/,
		int endYear /*= 1900*/, int endMonth /*= 1*/, int endDay /*= 1*/, int endHour /*= 0*/, int endMinute /*= 0*/,
		int windType /*= 0*/, BOOL monthlyWinds /*= FALSE*/, BOOL doWindRose /*= FALSE*/, int binSize /*= 5*/, int binMax /*= 30*/, 
		bool useWeibull /*= false*/)
{
	//analysis already run, generate export file and display to allow user to save.
	/////  WINDROSE data
	double wrData[WRDIRS][WRSPEEDS];
	long windDays = 0;
	long windRecsUsed = 0;
	long windDaysInQuery = 0;
	for(int d = 0; d < WRDIRS; d++)
	{
		for(int s = 0; s < WRSPEEDS; s++)
			wrData[d][s] = 0;
	}

	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(stream, "FireFamily Plus Fire Risk Export for FlamMap\n");

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	reportSet.Close();
	//
	fprintf(stream, "Calm Wind Threshold: %d   Start Time %02d:%02d End Time %02d:%02d Using WeibullWinds: %s\n",
		calmVal, startHour, startMinute, endHour, endMinute, useWeibull ? "Yes": "No");
	fprintf(stream, "Wind Type: ");
	switch(windType)
	{
		case 0:
			fprintf(stream, "Ten Minute Average Winds\n");
			break;
		case 1:
			fprintf(stream, "Wind Gusts\n");
			break;
		case 2:
			fprintf(stream, "Ten Minute Average Winds AND Wind Gusts\n");
			break;
	}
	fprintf(stream, "\n");
	//need time series ERC data, interpolate when necessary
	double *erc_avg = new double[numPeriods];
	double *erc_stddev = new double[numPeriods];
	double *erc_current = new double[numPeriods];
	int nRows = 0, nCurVals = 0;//, lastCurr = 0;
	for(int i = 0; i < numPeriods; i++)
	{
		erc_avg[i] = -1.0;
		erc_stddev[i] = 0.0;
		erc_current[i] = -.01;
	}
	for(int i = 0; i < numPeriods; i++)
	{
		if(periods[12][i]->N() > 0)
		{
			erc_avg[i] = periods[12][i]->mean;
			erc_stddev[i] = periods[12][i]->stdDev;
			nRows++;
			if(periods[12][i]->yStats[periods[12][i]->years - 1].N() > 0)
			{
				erc_current[i] = periods[12][i]->yStats[periods[12][i]->years - 1].Mean();
				nCurVals++;
			}
		}
	}
	//grab actual values...
	int numYears = periods[12][0]->years - 1;//exclude current year...
	int dataYears = numYears;
	double **ercObs = new double*[numYears];
	for(int i = 0; i < numYears; i++)
	{
		ercObs[i] = new double[numPeriods];
		for(int p = 0; p < numPeriods; p++)
		{
			if(periods[12][p]->yStats[i].N() > 0)
				ercObs[i][p] = periods[12][p]->yStats[i].Mean();
			else
				ercObs[i][p] = -9999;
		}
		//frontfill, backfill, and interpolate missing values...
		int yFirst = (ercObs[i][0] > 0) ? 0 : -1, yLast = -1;
		for(int p = 0; p < numPeriods; p++)
		{
			if(ercObs[i][p] >= 0.0 && yFirst < 0)
				yFirst = p;
			if(ercObs[i][p] >= 0.0)
				yLast = p;
		}
		//got any values at all?
		if(yLast < 0 || yFirst < 0)
		{//empty year...leave as NO_DATA
			dataYears--;
			continue;
		}
		for(int p = 0; p < yFirst; p++)
			ercObs[i][p] = ercObs[i][yFirst];
		for(int p = yLast + 1; p < numPeriods; p++)
			ercObs[i][p] = ercObs[i][yLast];
		//interpolate missing values...
		for(int p = 1; p < yLast; p++)
		{
			if(ercObs[i][p] < 0)//need to interpolate
			{
				int valsNeeded = 1;
				for(int n = p + 1; n <= yLast; n++)
				{
					if(ercObs[i][n] < 0)
						valsNeeded++;
					else
						break;
				}
				//need start and end val
				double startVal = ercObs[i][p - 1], nextVal = ercObs[i][p + valsNeeded];
				double step = (nextVal - startVal)/(valsNeeded +1);
				for(int l = 0; l < valsNeeded; l++)
				{
					ercObs[i][l + p] = ercObs[i][p - 1] + l * step;
				}
			}
		}
	}
	fprintf(stream,"Time Series Data\n");
	fprintf(stream,"%d %d ERC_AVG ERC_STDDEV ERC_CURR\n", numPeriods, dataYears);
	int avgFirst = (erc_avg[0] > 0.0) ? 0 : -1, avgLast = -1,
		currFirst = (erc_current[0] > 0) ? 0 :-1, currLast = -1;
	//data prep
	for(int i = 0; i < numPeriods; i++)
	{
		if(erc_avg[i] >= 0.0 && avgFirst < 0)
			avgFirst = i;
		if(erc_avg[i] >= 0.0)
			avgLast = i;
		if(erc_current[i] >= 0 && currFirst < 0)
			currFirst = i;
		if(erc_current[i] >= 0)
			currLast = i;
	}

	//frontfill with first ocurring value
	for(int i = 0; i < avgFirst; i++)
	{
		erc_avg[i] = erc_avg[avgFirst];
		erc_stddev[i] = erc_stddev[avgFirst];
	}
	for(int i = 0; i < currFirst; i++)
		erc_current[i] = erc_current[currFirst];
	//backfill averages with last valid values
	for(int i = avgLast + 1; i < numPeriods; i++)
	{
		erc_avg[i] = erc_avg[avgLast];
		erc_stddev[i] = erc_stddev[avgLast];
	}
	//interpolate missing interior values....
	//avgs & stddevs
	for(int i = 1; i < avgLast; i++)
	{
		if(erc_avg[i] < 0)//need to interpolate
		{
			int valsNeeded = 1;
			for(int n = i + 1; n <= avgLast; n++)
			{
				if(erc_avg[n] < 0)
					valsNeeded++;
				else
					break;
			}
			//need start and end val
			double startVal = erc_avg[i - 1], nextVal = erc_avg[i + valsNeeded],
				stdStart = erc_stddev[i - 1], nextStd = erc_stddev[i + valsNeeded];
			double step = (nextVal - startVal)/(valsNeeded +1);
			for(int l = i; l < i + valsNeeded; l++)
			{
				erc_avg[l] = erc_avg[l - 1] + step;
				erc_stddev[l] = erc_stddev[l - 1] + step;
			}
		}
	}
	//interpolate currs
	for(int i = 1; i < currLast; i++)
	{
		if(erc_current[i] < 0)//need to interpolate
		{
			int valsNeeded = 1;
			for(int n = i + 1; n <= currLast; n++)
			{
				if(erc_current[n] < 0)
					valsNeeded++;
				else
					break;
			}
			//need start and end val
			double startVal = erc_current[i - 1], nextVal = erc_current[i + valsNeeded];
			double step = (nextVal - startVal)/(valsNeeded +1);
			for(int l = 0; l < valsNeeded; l++)
			{
				erc_current[l + i] = erc_current[i - 1] + l * step;
			}
		}
	}
	//output time series...
	for(int i = 0; i < numPeriods; i++)
	{
		if(i <= currLast)
			fprintf(stream, "%.1f \t%.1f \t%.1f\t%s", erc_avg[i], erc_stddev[i], erc_current[i],
			periods[12][i]->end.Format(VAR_DATEVALUEONLY));
		else //just avg & stddev
			fprintf(stream, "%.1f \t%.1f\t-9999\t%s", erc_avg[i], erc_stddev[i],
			periods[12][i]->end.Format(VAR_DATEVALUEONLY));
		for(int y = 0; y < numYears; y++)
		{
			if(ercObs[y][i] != -9999)
				fprintf(stream, "\t%.1f", ercObs[y][i]);
		}
		fprintf(stream, "\n");
	}
	delete[]erc_avg;
	delete[]erc_stddev;
	delete[]erc_current;
	for(int i = 0; i < numYears; i++)
		delete[] ercObs[i];
	delete[] ercObs;
	//also need frequency info for moistures and accompanying ERC (percentiles, 1- 100)
	long nValidObs = 0;
	for(int p = 0; p < numPeriods; p++)
	{
		for(int y = 0; y < periods[12][p]->years; y++)
		{
			//for(int v = 0; v < periods[12][p]->yStats[y].N(); v++)
			//{
				nValidObs += periods[12][p]->yStats[y].N();
		}
	}
	ERCMoist *ercStats;
	ercStats = new ERCMoist[nValidObs];
	long ercLoc = 0;
	for(int p = 0; p < numPeriods; p++)
	{
		for(int y = 0; y < periods[12][p]->years; y++)
		{
			for(int v = 0; v < periods[12][p]->yStats[y].N(); v++)
			{
				ercStats[ercLoc].ERC = periods[12][p]->yStats[y].vals[v];
				ercStats[ercLoc].fm1 = periods[16][p]->yStats[y].vals[v];
				ercStats[ercLoc].fm10 = periods[17][p]->yStats[y].vals[v];
				ercStats[ercLoc].fm100 = periods[18][p]->yStats[y].vals[v];
				ercStats[ercLoc].fm1000 = periods[19][p]->yStats[y].vals[v];
				ercStats[ercLoc].fmHerb = periods[20][p]->yStats[y].vals[v];
				ercStats[ercLoc].fmWood = periods[21][p]->yStats[y].vals[v];
	//			ercStats[ercLoc].fmX1000 = periods[22][p]->yStats[y].vals[v];
				ercLoc++;
			}
		}
	}
	qsort(ercStats, (size_t)nValidObs, sizeof(ERCMoist), ercMoistCompare);

	fprintf(stream, "Percentiles\n ERC       \tFM1       \tFM10       \tFM100       \tFM1000       \tHERB       \tWOODY       \tX1000\n");
	ERCMoist pctAvgs[100];
	bool *marks = new bool[nValidObs];
	for(long m = 0; m < nValidObs; m++)
		marks[m] = false;
	for(int i = 0; i < 100; i++)
	{
		//need to average the moistures for each percentile
		//STRATEGY: keep array of marks for each obs,
		//For each percentile, average all the values (the 1 percent) that falls in the percentile
		//But Different percentiles may have the same ERC value... so may actually go out of a percentile bin to grab ties
		//Set the mark array when an obs is used, only let it be used once.
		//double fm1Avg = 0.0, fm10Avg = 0.0, fm100Avg = 0.0, fm1000Avg = 0.0, fmHerbAvg = 0.0, fmWoodAvg = 0.0, fmx1000Avg = 0.0;
		long pctRange = nValidObs / 100.0;
		long pLoc = nValidObs - (nValidObs / 100.0) * i - 1;// percentile;
		double pctVal = ercStats[pLoc].ERC;
		pctAvgs[99 - i].ERC = ercStats[pLoc].ERC;
		pctAvgs[99 - i].fm1 = 0.0;
		pctAvgs[99 - i].fm10 = 0.0;
		pctAvgs[99 - i].fm100 = 0.0;
		pctAvgs[99 - i].fm1000 = 0.0;
		pctAvgs[99 - i].fmHerb = 0.0;
		pctAvgs[99 - i].fmWood = 0.0;
		//pctAvgs[99 - i].fmX1000 = 0.0;
		//check to see if already calculated...
		if(i > 0 && pctAvgs[99 - i].ERC == pctAvgs[99 - i + 1].ERC)
		{//same ERC
			pctAvgs[99 - i].fm1 = pctAvgs[99 - i + 1].fm1;
			pctAvgs[99 - i].fm10 = pctAvgs[99 - i + 1].fm10;
			pctAvgs[99 - i].fm100 = pctAvgs[99 - i + 1].fm100;
			pctAvgs[99 - i].fm1000 = pctAvgs[99 - i + 1].fm1000;
			pctAvgs[99 - i].fmHerb = pctAvgs[99 - i + 1].fmHerb;
			pctAvgs[99 - i].fmWood = pctAvgs[99 - i + 1].fmWood;
			//pctAvgs[99 - i].fmX1000 = pctAvgs[99 - i + 1].fmX1000;
			continue;
		}
		long nVals = 0;
		//check to see if need to include partial bin from previous percentile (ties caused this...)
		if(i > 0)
		{
			long q = 0;
			for(; q < pctRange; q++)
			{
				if(!marks[pLoc + q])
				{
					pctAvgs[99 - i].fm1 += ercStats[pLoc + q].fm1;
					pctAvgs[99 - i].fm10 += ercStats[pLoc + q].fm10;
					pctAvgs[99 - i].fm100 += ercStats[pLoc + q].fm100;
					pctAvgs[99 - i].fm1000 += ercStats[pLoc + q].fm1000;
					pctAvgs[99 - i].fmHerb += ercStats[pLoc + q].fmHerb;
					pctAvgs[99 - i].fmWood += ercStats[pLoc + q].fmWood;
					//pctAvgs[99 - i].fmX1000 += ercStats[pLoc + q].fmX1000;
					marks[pLoc + q] = true;
					nVals++;
				}
			}
		}
		//grab all the values in a percentile range, marking the obs used

		long p = 0;
		for( ; p < pctRange; p++)
		{
			if(!marks[pLoc - p])
			{
				pctAvgs[99 - i].fm1 += ercStats[pLoc - p].fm1;
				pctAvgs[99 - i].fm10 += ercStats[pLoc - p].fm10;
				pctAvgs[99 - i].fm100 += ercStats[pLoc - p].fm100;
				pctAvgs[99 - i].fm1000 += ercStats[pLoc - p].fm1000;
				pctAvgs[99 - i].fmHerb += ercStats[pLoc - p].fmHerb;
				pctAvgs[99 - i].fmWood += ercStats[pLoc - p].fmWood;
				//pctAvgs[99 - i].fmX1000 += ercStats[pLoc - p].fmX1000;
				marks[pLoc - p] = true;
				nVals++;
			}
		}
		//add ties to percentile...
		while((pLoc - p >= 0) && ercStats[pLoc - p].ERC == pctAvgs[99 - i].ERC)
		{
			if(!marks[pLoc - p])
			{
				pctAvgs[99 - i].fm1 += ercStats[pLoc - p].fm1;
				pctAvgs[99 - i].fm10 += ercStats[pLoc - p].fm10;
				pctAvgs[99 - i].fm100 += ercStats[pLoc - p].fm100;
				pctAvgs[99 - i].fm1000 += ercStats[pLoc - p].fm1000;
				pctAvgs[99 - i].fmHerb += ercStats[pLoc - p].fmHerb;
				pctAvgs[99 - i].fmWood += ercStats[pLoc - p].fmWood;
				//pctAvgs[99 - i].fmX1000 += ercStats[pLoc - p].fmX1000;
				marks[pLoc - p] = true;
				nVals++;
			}
			p++;
		}
		//turn moistures into avg moistures...
		pctAvgs[99 - i].fm1 /= nVals;
		pctAvgs[99 - i].fm10 /= nVals;
		pctAvgs[99 - i].fm100 /= nVals;
		pctAvgs[99 - i].fm1000 /= nVals;
		pctAvgs[99 - i].fmHerb /= nVals;
		pctAvgs[99 - i].fmWood /= nVals;
		//pctAvgs[99 - i].fmX1000 /= nVals;

	}
	for(int p = 0; p < 100; p++)
	{
		fprintf(stream, "%6.1f   \t%6.2f \t%6.2f \t%6.2f \t%6.2f \t%6.2f \t%6.2f \t12.0\n",
			pctAvgs[p].ERC, pctAvgs[p].fm1, pctAvgs[p].fm10, pctAvgs[p].fm100, pctAvgs[p].fm1000,
			pctAvgs[p].fmHerb, pctAvgs[p].fmWood);// , pctAvgs[p].fmX1000);
	}
	if(marks)
		delete[] marks;
	if(ercStats)
		delete[] ercStats;
	//need wind speed/dir joint probability table
	//fprintf(stream, "Wind Speed vs Dir\n");
	//fprintf(stream, "%d 8\n", nSpeedBins);
	//fprintf(stream, "speed\t  45\t  90\t 135\t 180\t 225\t 270\t 315\t 360\n");
	//change 2019/08/25
	//get all the wind speeds by direction first, 
	//if Weibull, regenerate new vectors of wind speed
	//then bin according to settings,
	//either static max or observed max

	//begin alternative METHOD - SET UP FOR WEIBULL
	/************************************************************************************************/

	const int NUM_WEIBULL_SAMPLES = 100000;
	//observations BINs
	int *nObs[13][8];
	int nObsCalms[13];
	int totalObs = 0;
	int totalObsCalm;
	//output BINs
	double *outObs[13][8];
	double outCalms[13];

	int maxSpeed = 30, overallMaxSpeed = 30;
	//vectors of observations
	std::vector<int> vDirWindSpeeds[8];
	std::vector<int> vMonthlyDirWindSpeeds[12][8];
	std::vector<int> vWeibullSpeeds[8];
	std::vector<int> vMonthlyWeibullSpeeds[12][8];

	//get the data by month & direction and annual
	if (fpSet->m_SIG_Station.GetLength() > 6) //sig
	{
		CStationInSIGSet sigSet(fpSet->m_pDatabase);
		CString temp;
		char sig[64];
		strcpy_s(sig, fpSet->m_SIG_Station);
		temp.Format("[SIG] = '%-20.20s'", &sig[6]);
		sigSet.m_strFilter = _T(temp);
		sigSet.Open();
		while (!sigSet.IsEOF())
		{
			GetWindSpeedsByDirections(sigSet.m_StationID, fpSet->m_pDatabase, startYear, startMonth,
				startDay, startHour, startMinute, endYear, endMonth, endDay, endHour, endMinute, windType, vDirWindSpeeds, vMonthlyDirWindSpeeds,
				wrData, &windDays, &windRecsUsed, &windDaysInQuery);
			sigSet.MoveNext();
		}
	}
	else
		GetWindSpeedsByDirections(fpSet->m_SIG_Station, fpSet->m_pDatabase, startYear, startMonth,
			startDay, startHour, startMinute, endYear, endMonth, endDay, endHour, endMinute, windType, vDirWindSpeeds, vMonthlyDirWindSpeeds,
			wrData, &windDays, &windRecsUsed, &windDaysInQuery);

	if (binMax <= 0)
	{
		for (int d = 0; d < 8; d++)
		{
			for (std::vector<int>::iterator it = vDirWindSpeeds[d].begin(); it != vDirWindSpeeds[d].end(); ++it)
			{
				maxSpeed = max(maxSpeed, *it);
			}
		}
	}
	if (useWeibull) // get Weibull distributed observations
	{
		for (int m = 0; m < 12; m++)
		{
			for (int d = 0; d < 8; d++)
			{
				if (vMonthlyDirWindSpeeds[m][d].size() > 0)
				{
					int obsMax = 0, thisMax = 0;
					for (std::vector<int>::iterator it = vMonthlyDirWindSpeeds[m][d].begin(); it != vMonthlyDirWindSpeeds[m][d].end(); ++it)
						obsMax = max(obsMax, *it);
					double ahat, bhat;
					CalcWeibullParams(vMonthlyDirWindSpeeds[m][d], &ahat, &bhat);
					std::weibull_distribution<double> distribution(bhat, ahat);
					std::random_device rd;
					std::mt19937 gen(rd());
					for (int i = 0; i < NUM_WEIBULL_SAMPLES; i++)
					{
						int w = std::round(distribution(gen));
						if (binMax > 0 && w > binMax)
						{
							i--;
							continue;
						}
						vMonthlyWeibullSpeeds[m][d].push_back(w);
						vWeibullSpeeds[d].push_back(w);
						thisMax = max(thisMax, w);
					}
					maxSpeed = max(maxSpeed, thisMax);
				}
			}
		}
	}

	int nSpeedBins = 6;//default
	if (binMax > 0)
	{
		nSpeedBins = binMax / binSize;
		if (binMax % binSize != 0)
			nSpeedBins++;
	}
	else
	{
		int tMax = max(maxSpeed, overallMaxSpeed);
		nSpeedBins = tMax / binSize;
		if (tMax % binSize != 0)
			nSpeedBins++;

	}

	for (int m = 0; m < 13; m++)
	{
		for (int d = 0; d < 8; d++)
		{
			nObs[m][d] = new int[nSpeedBins];
			outObs[m][d] = new double[nSpeedBins];
			for (int s = 0; s < nSpeedBins; s++)
			{
				nObs[m][d][s] = 0;
				outObs[m][d][s] = 0.0;
			}
		}
		nObsCalms[m] = 0.0;
	}

	int dObs[13][8];
	int mObs[13];
	//always need to bin the input obs
	for (int m = 0; m < 13; m++)
	{
		mObs[m] = 0;
		for (int d = 0; d < 8; d++)
		{
			dObs[m][d] = 0;
			if (m < 12)
			{
				BinObs(vMonthlyDirWindSpeeds[m][d], binSize, nSpeedBins, calmVal, nObs[m][d], &nObsCalms[m]);
				dObs[m][d] += vMonthlyDirWindSpeeds[m][d].size();
				mObs[m] += vMonthlyDirWindSpeeds[m][d].size();
			}
			else
			{
				BinObs(vDirWindSpeeds[d], binSize, nSpeedBins, calmVal, nObs[m][d], &nObsCalms[m]);
				totalObs += vDirWindSpeeds[d].size();
				dObs[m][d] += vDirWindSpeeds[d].size();
				mObs[m] += vDirWindSpeeds[d].size();
			}
		}
	}


	//if using Weibull, need to bin weibull data
	if (useWeibull)
	{
		int tCalms;
		for (int m = 0; m < 13; m++)
		{
			nObsCalms[m] = 0;
			for (int d = 0; d < 8; d++)
			{
				double thisScaler = 1.0;
				if(mObs[m] > 0)
					thisScaler = ((double)dObs[m][d]) / (double)mObs[m];
				for(int s = 0; s < nSpeedBins; s++)
					nObs[m][d][s] = 0;
				tCalms = 0;
				if (m < 12)
					BinObs(vMonthlyWeibullSpeeds[m][d], binSize, nSpeedBins, calmVal, nObs[m][d], &tCalms);
				else
					BinObs(vWeibullSpeeds[d], binSize, nSpeedBins, calmVal, nObs[m][d], &tCalms);
				nObsCalms[m] += round(((double)tCalms) * thisScaler);
				for (int s = 0; s < nSpeedBins; s++)
				{
					if(m < 12)
						outObs[m][d][s] = ((double)nObs[m][d][s]) / NUM_WEIBULL_SAMPLES * thisScaler;
					else
						outObs[m][d][s] = ((double)nObs[m][d][s]) / (NUM_WEIBULL_SAMPLES * 12.0) * thisScaler;
				}
			}
			if (m < 12)
				outCalms[m] = ((double)nObsCalms[m]) / NUM_WEIBULL_SAMPLES;
			else
				outCalms[m] = ((double)nObsCalms[m]) / (NUM_WEIBULL_SAMPLES * 12.0);
		}

	}
	else
	{
		for (int m = 0; m < 13; m++)
		{
			for (int d = 0; d < 8; d++)
			{
				for (int s = 0; s < nSpeedBins; s++)
				{
					if (mObs[m] > 0)
						outObs[m][d][s] = ((double)nObs[m][d][s]) / mObs[m];
					else
						outObs[m][d][s] = 0.0;
				}
			}
			if(mObs[m] > 0)
				outCalms[m] = ((double)nObsCalms[m]) / mObs[m];
			else
				outCalms[m] = 0.0;
		}
	}
	//now we can output 
	//if(!useWeibull)
		fprintf(stream, "Wind Speed vs Dir\n");
	//else
	//	fprintf(stream, "Wind Speed vs Dir \n");
	fprintf(stream, "%d 8\n", nSpeedBins);
	fprintf(stream, "speed\t  45\t\t  90\t\t 135\t\t 180\t\t 225\t\t 270\t\t 315\t\t 360\n");
	//output bins as percentages
	double pctSum = 0.0;
	for (int s = 0; s < nSpeedBins; s++)
	{
		fprintf(stream, "%d", (s + 1) * binSize);
		for (int d = 0; d < 8; d++)
		{
			double pct = 0.0;
			pct = outObs[12][d][s];
			fprintf(stream, " \t%4.5f", pct * 100.0);
			pctSum += pct * 100.0;
		}
		fprintf(stream, "\n");
	}
	pctSum += outCalms[12] * 100.0;
	fprintf(stream, "%.5f\n", outCalms[12]  * 100.0);
	if (monthlyWinds)
	{
		for (int m = 0; m < 12; m++)
		{
			char monthStr[64];
			switch (m)
			{
			case 0:
				strcpy_s(monthStr, "January");
				break;
			case 1:
				strcpy_s(monthStr, "February");
				break;
			case 2:
				strcpy_s(monthStr, "March");
				break;
			case 3:
				strcpy_s(monthStr, "April");
				break;
			case 4:
				strcpy_s(monthStr, "May");
				break;
			case 5:
				strcpy_s(monthStr, "June");
				break;
			case 6:
				strcpy_s(monthStr, "July");
				break;
			case 7:
				strcpy_s(monthStr, "August");
				break;
			case 8:
				strcpy_s(monthStr, "September");
				break;
			case 9:
				strcpy_s(monthStr, "October");
				break;
			case 10:
				strcpy_s(monthStr, "November");
				break;
			case 11:
				strcpy_s(monthStr, "December");
				break;
			}
			fprintf(stream, "%s:\n", monthStr);
			//fprintf(stream, "speed\t  45\t  90\t 135\t 180\t 225\t 270\t 315\t 360\n");
			fprintf(stream, "speed\t  45\t\t  90\t\t 135\t\t 180\t\t 225\t\t 270\t\t 315\t\t 360\n");

			//first, count number obs
			long n = outCalms[m];;
			for (int s = 0; s < nSpeedBins; s++)
			{
				for (int d = 0; d < 8; d++)
				{
					n += nObs[m][d][s];
				}
			}
			//now output as probability
			pctSum = 0.0;
			for (int s = 0; s < nSpeedBins; s++)
			{
				fprintf(stream, "%.2f", (s + 1) * 5.00);
				for (int d = 0; d < 8; d++)
				{
					double pct = 0.0;
					//if (n > 0)
					pct = outObs[m][d][s];//) / (double)n;
					fprintf(stream, " \t%4.5f", pct * 100.0);
					pctSum += pct * 100.0;
				}
				fprintf(stream, "\n");
			}
			pctSum += outCalms[m] * 100.0;
			fprintf(stream, "%.5f\n", outCalms[m]* 100.0);
		}
	}
	fclose(stream);

	for (int m = 0; m < 13; m++)
	{
		for (int d = 0; d < 8; d++)
		{
			delete[] nObs[m][d];
			delete[] outObs[m][d];
		}
	}
	//end alternative METHOD
	/************************************************************************************************
	std::vector<int> vDirWindSpeeds[8];
	std::vector<int> vMonthlyDirWindSpeeds[12][8];

	std::vector<int> vWeibullSpeeds[8];
	std::vector<int> vMonthlyWeibullSpeeds[12][8];

	//std::vector<double> *
	long *bins [8];
	long monthlyBins[12][6][8];
	long monthlyCalms[12];
	for(int s = 0; s < 6; s++)
	{
		for(int d = 0; d < 8; d++)
		{
			bins[s][d] = 0L;
			for(int m = 0; m < 12; m++)
				monthlyBins[m][s][d] = 0L;
		}
	}
	for(int m = 0; m < 12; m++)
		monthlyCalms[m] = 0;
	long numObs = 0L;
	long numCalm = 0L;
	if (fpSet->m_SIG_Station.GetLength() > 6) //sig
	{
		CStationInSIGSet sigSet(fpSet->m_pDatabase);
		CString temp;
		char sig[64];
		strcpy_s(sig, fpSet->m_SIG_Station);
		temp.Format("[SIG] = '%-20.20s'", &sig[6]);
		sigSet.m_strFilter = _T(temp);
		sigSet.Open();
		while (!sigSet.IsEOF())
		{
			GetWindSpeedsByDirections(sigSet.m_StationID, fpSet->m_pDatabase, startYear, startMonth,
				startDay, startHour, startMinute, endYear, endMonth, endDay, endHour, endMinute, windType, vDirWindSpeeds, vMonthlyDirWindSpeeds);
			//AddStationWinds(sigSet.m_StationID, fpSet->m_pDatabase, bins, monthlyBins, monthlyCalms, &numObs, &numCalm, calmVal, startYear, startMonth,
			////	startDay, startHour, startMinute, endYear, endMonth, endDay, endHour, endMinute, windType, wrData,
			//	&windDays, &windRecsUsed, &windDaysInQuery);
			sigSet.MoveNext();
		}
	}
	else
		GetWindSpeedsByDirections(fpSet->m_SIG_Station, fpSet->m_pDatabase, startYear, startMonth,
			startDay, startHour, startMinute, endYear, endMonth, endDay, endHour, endMinute, windType, vDirWindSpeeds, vMonthlyDirWindSpeeds);
		//AddStationWinds(fpSet->m_SIG_Station, fpSet->m_pDatabase, bins, monthlyBins, monthlyCalms, &numObs, &numCalm, calmVal, startYear, startMonth,
		//	startDay, startHour, startMinute, endYear, endMonth, endDay, endHour, endMinute, windType, wrData,
		//		&windDays, &windRecsUsed, &windDaysInQuery);


	int overallMaxSpeed = 0;
	//ok, now have the wind speeds by direction
	//if Weibull is used, generate new winds
	int maxSpeed = 0;
	if (useWeibull)
	{
		if (monthlyWinds)
		{
			for (int m = 0; m < 12; m++)
			{
				for (int d = 0; d < 8; d++)
				{
					if (vMonthlyDirWindSpeeds[m][d].size() > 0)
					{
						int obsMax = 0, thisMax = 0;
						for (std::vector<int>::iterator it = vMonthlyDirWindSpeeds[m][d].begin(); it != vMonthlyDirWindSpeeds[m][d].end(); ++it)
							obsMax = max(obsMax, *it);
						double ahat, bhat;
						CalcWeibullParams(vMonthlyDirWindSpeeds[m][d], &ahat, &bhat);
						std::weibull_distribution<double> distribution(bhat, ahat);
						std::random_device rd;
						std::mt19937 gen(rd());
						for (int i = 0; i < 100000; i++)
						{
							int w = std::round(distribution(gen));
							vMonthlyWeibullSpeeds[m][d].push_back(w);
							thisMax = max(thisMax, w);
						}
						maxSpeed = max(maxSpeed, thisMax);
					}
				}
			}
		}
		//always do overall
		for (int d = 0; d < 8; d++)
		{
			int obsMax = 0;
			for (std::vector<int>::iterator it = vDirWindSpeeds[d].begin(); it != vDirWindSpeeds[d].end(); ++it)
				obsMax = max(obsMax, *it);
			double ahat, bhat;
			CalcWeibullParams(vDirWindSpeeds[d], &ahat, &bhat);
			std::weibull_distribution<double> distribution(bhat, ahat);
			std::random_device rd;
			std::mt19937 gen(rd());
			for (int i = 0; i < 100000; i++)
			{
				int w = std::round(distribution(gen));
				vWeibullSpeeds[d].push_back(w);
				overallMaxSpeed = max(overallMaxSpeed, w);
			}
		}
	}
	int testX = 0;
	double *vBins[8];
	int nSpeedBins = 6;//default
	if (binMax > 0)
	{
		nSpeedBins = binMax / binSize;
		if (binMax % binSize != 0)
			nSpeedBins++;
	}
	else
	{ 
		int tMax = max(maxSpeed, overallMaxSpeed);
		nSpeedBins = tMax / binSize;
		if (tMax % binSize != 0)
			nSpeedBins++;

	}

	for (int b = 0; b < nSpeedBins; b++)
	{
		vBins[b] = new double[nSpeedBins];
		for (int i = 0; i < nSpeedBins; i++)
			vBins[b][i] = 0.0;
	}
	//now fill speed bins with proportions
	int nTotalObs = 0;
	if (!useWeibull)//using observed
	{
		if (monthlyWinds)
		{
			for (int m = 0; m < 12; m++)
			{
				for (int d = 0; d < 8; d++)
				{
					for (std::vector<int>::iterator it = vMonthlyDirWindSpeeds[m][d].begin();
						it != vMonthlyDirWindSpeeds[m][d].end(); ++it)
					{
						int ws = *it;
						if (ws <= calmVal)
						{
							monthlyCalms[m]++;
							numCalm++;
						}
						else
						{
							int bin = ws / binSize;
							if (bin >= nSpeedBins)
								bin = nSpeedBins - 1;
							monthlyBins[m][bin][d]++;
							bins[bin][d]++;
						}
						nTotalObs++;
					}
				}
			}
		}
		else
		{
			for (int d = 0; d < 8; d++)
			{
				for (std::vector<int>::iterator it = vDirWindSpeeds[d].begin();
					it != vDirWindSpeeds[d].end(); ++it)
				{
					int ws = *it;
					if (ws <= calmVal)
					{
						numCalm++;
					}
					else
					{
						int bin = ws / binSize;
						if (bin >= nSpeedBins)
							bin = nSpeedBins - 1;
						bins[bin][d]++;
					}
					nTotalObs++;
				}
			}
		}
	}
	else//using Weibull
	{
		if (monthlyWinds)
		{
			for (int m = 0; m < 12; m++)
			{
				for (int d = 0; d < 8; d++)
				{
					for (std::vector<int>::iterator it = vMonthlyWeibullSpeeds[m][d].begin();
						it != vMonthlyWeibullSpeeds[m][d].end(); ++it)
					{
						int ws = *it;
						if (ws <= calmVal)
						{
							monthlyCalms[m]++;
							numCalm++;
						}
						else
						{
							int bin = ws / binSize;
							if (bin >= nSpeedBins)
								bin = nSpeedBins - 1;
							monthlyBins[m][bin][d]++;
							bins[bin][d]++;
						}
						nTotalObs++;
					}
				}
			}
		}
		else
		{
			for (int d = 0; d < 8; d++)
			{
				for (std::vector<int>::iterator it = vWeibullSpeeds[d].begin();
					it != vWeibullSpeeds[d].end(); ++it)
				{
					int ws = *it;
					if (ws <= calmVal)
					{
						numCalm++;
					}
					else
					{
						int bin = ws / binSize;
						if (bin >= nSpeedBins)
							bin = nSpeedBins - 1;
						bins[bin][d]++;
					}
					nTotalObs++;
				}
			}
		}
	}
	//now change bins to proportions
	if (!useWeibull)
	{
		if (monthlyWinds)
		{
			for (int m = 0; m < 12; m++)
			{
				for (int s = 0; s < nSpeedBins; s++)
				{
					for (int d = 0; d < 8; d++)
					{
						monthlyBins[m][s][d] /= nTotalObs;
					}
					monthlyCalms[m] /= nTotalObs;
				}
			}
		}
	}
	else
	{
		//Using Weibull, need proportion observed by direction
		double dirProp[8];
		if (monthlyWinds)
		{
			for (int m = 0; m < 12; m++)
			{
				double monthTotal = 0;
				for (int d = 0; d < 8; d++)
				{
					monthTotal += vMonthlyDirWindSpeeds[m][d].size();
				}
				for (int d = 0; d < 8; d++)
					dirProp[d] = ((double)vMonthlyDirWindSpeeds[m][d].size()) / monthTotal;
				//now get distribution of Weibull samples
				for (int s = 0; s < nSpeedBins; s++)
				{

				}
			}
		}
	}
	//need wind speed/dir joint probability table
	fprintf(stream, "Wind Speed vs Dir\n");
	fprintf(stream, "%d 8\n", nSpeedBins);
	fprintf(stream, "speed\t  45\t  90\t 135\t 180\t 225\t 270\t 315\t 360\n");
	//output bins as percentages
	for(int s = 0; s < nSpeedBins; s++)
	{
		fprintf(stream, "%d", (s + 1) * binSize);
		for(int d = 0; d < 8; d++)
		{
			double pct = 0.0;
			if(numObs > 0)
				pct = ((double)bins[s][d]) / (double)numObs;
			fprintf(stream, "\t%4.2f", pct * 100.0);
		}
		fprintf(stream, "\n");
	}
	fprintf(stream, "%.2f\n", (numObs > 0.0) ? (double)numCalm / (double)numObs * 100.0 : 0.0);
	if(monthlyWinds)
	{
		for(int m = 0; m < 12; m++)
		{
			char monthStr[64];
			switch(m)
			{
				case 0:
					strcpy_s(monthStr, "January");
					break;
				case 1:
					strcpy_s(monthStr, "February");
					break;
				case 2:
					strcpy_s(monthStr, "March");
					break;
				case 3:
					strcpy_s(monthStr, "April");
					break;
				case 4:
					strcpy_s(monthStr, "May");
					break;
				case 5:
					strcpy_s(monthStr, "June");
					break;
				case 6:
					strcpy_s(monthStr, "July");
					break;
				case 7:
					strcpy_s(monthStr, "August");
					break;
				case 8:
					strcpy_s(monthStr, "September");
					break;
				case 9:
					strcpy_s(monthStr, "October");
					break;
				case 10:
					strcpy_s(monthStr, "November");
					break;
				case 11:
					strcpy_s(monthStr, "December");
					break;
			}
			fprintf(stream, "%s:\n", monthStr);
			fprintf(stream, "speed\t  45\t  90\t 135\t 180\t 225\t 270\t 315\t 360\n");

			//first, count number obs
			long n = monthlyCalms[m];;
			for(int s = 0; s < nSpeedBins; s++)
			{
				for(int d = 0; d < 8; d++)
				{
					n += monthlyBins[m][s][d];
				}
			}
			//now output as probability
			for(int s = 0; s < 6; s++)
			{
				fprintf(stream, "%.2f", (s + 1) * 5.00);
				for(int d = 0; d < 8; d++)
				{
					double pct = 0.0;
					if(n > 0)
						pct = ((double)monthlyBins[m][s][d]) / (double)n;
					fprintf(stream, "\t%4.2f", pct * 100.0);
				}
				fprintf(stream, "\n");
			}
			fprintf(stream, "%.2f\n", (n > 0.0) ? (double)monthlyCalms[m] / (double)n * 100.0 : 0.0);
		}
	}
	fclose(stream);*/
	//now display the file in a child window
	CString retName = fName;
	if(createDoc)
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
				CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(fName);
				pDoc->isTemp = true;
				CString tTitle;
				tTitle.Format("%s - FlamMap Fire Risk Export (unsaved)", fpSet->m_SIG_Station);
				pDoc->SetTitle(tTitle);
				//return retName;
			}
		}
	}
	free(fName);
	if(doWindRose && windRecsUsed > 0)
	{
		//massage array into percents
		for(int d = 0; d < WRDIRS; d++)
		{
			for(int s = 0; s < WRSPEEDS; s++)
			{
				wrData[d][s] = wrData[d][s] / ((double)windRecsUsed) * 100.0;
			}
		}
	      POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	      while(curTemplatePos != NULL)
	      {
		      CDocTemplate* curTemplate =
			     theApp.GetNextDocTemplate(curTemplatePos);
		      CString str;
		      curTemplate->GetDocString(str, CDocTemplate::docName);
		      if(str == _T("WindRose"))
		      {
			     CWindRoseDoc *pDoc = (CWindRoseDoc *)curTemplate->CreateNewDocument();
			     //set WindRoseDoc members
                  pDoc->maxPerc = 0.0;
        		double colPerc = 0.0;
			     for (int i=0;i<WRDIRS;i++)
				 {
					 colPerc = 0.0;
                    for (int j=0;j<WRSPEEDS;j++)
					{
					    pDoc->percs[i][j] = wrData[i][j];
						colPerc += wrData[i][j];
					}
					pDoc->maxPerc = max(pDoc->maxPerc, colPerc);
			     }
			     pDoc->recs = windRecsUsed;
			     pDoc->days = windDaysInQuery;
			     pDoc->days16 = windDays;
				 if (windType == 2)
					 pDoc->extraStr.Format("Winds: Both");
				 else if (windType == 0)
					 pDoc->extraStr.Format("Winds: Ave.");
				 else if (windType == 1)
					 pDoc->extraStr.Format("Winds: Gusts");
	              CSIGStationSet staSet(fpSet->m_pDatabase);
                  CString str2;//,stream;
	              if (fpSet->m_SIG_Station.GetLength() > 6){   // SIG
		             char sig[64];
		             strcpy_s(sig, fpSet->m_SIG_Station);
		             CString temp;
		             CStationInSIGSet sSet(fpSet->m_pDatabase);

		             temp.Format("[SIG] = '%-20.20s'", &sig[6]);
		             sSet.m_strFilter = _T(temp);
		             sSet.Open();
				     pDoc->staStr.Format("Stations: ");
				     int staCount = 0;
		             while(!sSet.IsEOF())
					 {
			            CString staQuery;
			            staQuery.Format("[StationID] = '%6.6s'", sSet.m_StationID);

			            staSet.m_strFilter = _T(staQuery);
			            staSet.Open();

			            str2.Format("%s-%s", staSet.m_StationID, staSet.m_Name);
			            if (staCount > 0)
				  		    pDoc->staStr.Append(", ");
					    staCount ++;

					    pDoc->staStr.Append(str2);

			            staSet.Close();
			            sSet.MoveNext();
		             }
		             sSet.Close();
	               }
				  else
				  { // single station
		             CString staQuery;
		             staQuery.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
		             staSet.m_strFilter = _T(staQuery);
		             staSet.Open();

		             pDoc->staStr.Format("Station: ");
				     staSet.m_StationID.Trim();

		             if (! staSet.m_StationID.IsEmpty())
		                str2.Format("%s %s", staSet.m_StationID, staSet.m_Name);
				     else
					    str2.Format("%s",fpSet->m_SIG_Station);

		             pDoc->staStr.Append(str2);
		             staSet.Close();
	              }
			      pDoc->perStr.Format("Annual filter dates:  %d/%d  thru  %d/%d", startMonth, startDay, endMonth, endDay);

			      pDoc->dateStr.Format("Data years:  %d - %d", startYear, endYear);
					pDoc->hourStr.Format("Hours: %02d%02d-%02d%02d", startHour, startMinute, endHour, endMinute);
			     CWindRoseFrame* pFrame = (CWindRoseFrame*)curTemplate->CreateNewFrame(pDoc, NULL);

			     curTemplate->InitialUpdateFrame(pFrame, pDoc);
			     CString tTitle;
					tTitle.Format("%s - Wind Rose",fpSet->m_SIG_Station);
			     pDoc->SetTitle(tTitle);
				 }
		  }
	}
	return retName;
}

void CClimAnalysis::AnalyzeQuick(CRawClim *pClim, bool _inverted/* = false*/)
{
	CWaitCursor wait;
	m_usedExtremes = pClim->m_usedExtremes;
	fpSet = pClim->m_fpSet;
	bool doyFlipped = false;
	if(MonthChtoInt(fpSet->m_StartMonth) >  MonthChtoInt(fpSet->m_EndMonth)
		|| (MonthChtoInt(fpSet->m_StartMonth) ==  MonthChtoInt(fpSet->m_EndMonth) && fpSet->m_StartDay > fpSet->m_EndDay))
		doyFlipped = true;
	//inverted = false;//_inverted;
	inverted = _inverted;
	//calculate and create varIDs
	for(int i = 0; i < MAXVARIDS; i++)
		varNames[i] = "";
	int ids = 0;
	climSet = pClim->m_climSet;
	climSet->m_strFilter.Format("");
	climSet->m_strSort = _T("[VarID]");
	climSet->Requery();
	while(!climSet->IsEOF())
	{
		ids++;
		climSet->MoveNext();
	}
	varIDs = new int[ids];
	nVarIDs = ids;
	ids = 0;
	climSet->MoveFirst();
	while(!climSet->IsEOF())
	{
		varIDs[ids] = climSet->m_VarID - 1;
		ids++;
		varNames[climSet->m_VarID - 1] = climSet->m_ShortName;
		climSet->MoveNext();
	}
	CUserVarSet usv(pDB);
	usv.m_strSort = "[VarID]";
	usv.Open();
	while(!usv.IsEOF())
	{
		if(usv.m_VarID <= MAXVARIDS && usv.m_VarID > STATICVARIDS)
			varNames[usv.m_VarID - 1] = usv.m_Abbrev;
		usv.MoveNext();
	}
	usv.Close();
	//model use flags - optimize to only calculate models used
	killPeriods = true;

	useCanadian = false;
	useNFDRS = false;
	count = 0;
	userCount = 0;
	int sYear = fpSet->m_StartYear, eYear = fpSet->m_EndYear;
	if(inverted)
	{//need to know how many years
		sYear = pClim->m_baseYear;
		eYear = pClim->m_baseYear + pClim->m_nYears - 1;

	}
	int pLen = atoi(fpSet->m_PeriodLength);
	COleDateTimeSpan span( pLen - 1, 0, 0, 0 );
	COleDateTime d1, d2, d3;
	numPeriods = 0;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d2 = d1 + span;
	//ensure don't cross month boundary
	while(d1.GetMonth() != d2.GetMonth())
		d2 -= 1;
	while(1)
	{
		//new period coming into loop
		numPeriods++;
		if(MonthChtoInt(fpSet->m_EndMonth) == d1.GetMonth()
			&& fpSet->m_EndDay >= d1.GetDay()
			&& fpSet->m_EndDay <= d2.GetDay())
			break;
		d3 = d2 + span;
		d3 += 1;
		if(d2.GetMonth() != d3.GetMonth())//does next period cross a month boundary?
		{
			if(d3.GetDay() <= pLen / 2)//will create a new period, otherwise will combine
				numPeriods++;
			if(MonthChtoInt(fpSet->m_EndMonth) == d1.GetMonth()
				&& fpSet->m_EndDay >= d1.GetDay()
				&& fpSet->m_EndDay <= daysInMonth[d1.GetMonth() - 1])
				break;
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d2 = d1 + span;
		}
		else
		{
			d1 = d2;
			d1 += 1;
			d2 = d1 + span;
		}
		//ensure don't cross month boundary
		while(d1.GetMonth() != d2.GetMonth())
			d2 -= 1;
	}
	//ok, got numPeriods. Allocate date arrays and fill values by repeating loop
	sDates = new COleDateTime[numPeriods];
	eDates = new COleDateTime[numPeriods];
	int dLoc = 0;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d2 = d1 + span;
	//safety
	while(d1.GetMonth() != d2.GetMonth())
		d2 -= 1;
	while(1)
	{
		//new period coming into loop
		sDates[dLoc].SetDate(sYear, d1.GetMonth(), d1.GetDay());
		eDates[dLoc++].SetDate(eYear, d1.GetMonth(), d2.GetDay());
		d3 = d2 + span;
		d3 += 1;
		if(d2.GetMonth() != d3.GetMonth()) //will next period cross a month boundary???
		{
			if(d3.GetDay() <= pLen / 2 && dLoc < numPeriods)
			{
				//extra group to fill
				sDates[dLoc].SetDate(sYear, d1.GetMonth(), d2.GetDay() + 1);
				if(IsLeap(eYear))
				{
					eDates[dLoc++].SetDate(eYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
				}
				else
				{
					if(d1.GetMonth() != 2)
						eDates[dLoc++].SetDate(eYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
					else
						eDates[dLoc++].SetDate(eYear, d1.GetMonth(), 28);
				}
				if(dLoc >= numPeriods)
					break;
			}
			else
			{//combine into previous group
				if(d1.GetMonth() < MonthChtoInt(fpSet->m_EndMonth)  || doyFlipped)
				{
					if(IsLeap(eYear))// % 4 == 0 && (eYear % 100 != 0 || eYear % 400 == 0))
						eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
					else
					{
						if(d1.GetMonth() != 2)
							eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
						else
							eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), 28);
					}
				}
				else//last month of query
				{
					if(IsLeap(eYear))// % 4 == 0 && (eYear % 100 != 0 || eYear % 400 == 0))
						eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), fpSet->m_EndDay);
					else//daysInMonth[d1.GetMonth() - 1]);
					{
						if(d1.GetMonth() != 2)
							eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), fpSet->m_EndDay);
						else
							eDates[dLoc - 1].SetDate(eYear, d1.GetMonth(), min(fpSet->m_EndDay, 28));
					}
				}
			}
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d2 = d1 + span;
		}
		else
		{//won't cross boundary, increment normally
			d1 = d2;
			d1 += 1;
			d2 = d1 + span;
		}
		//ensure same month
		while(d1.GetMonth() != d2.GetMonth())
			d2 -= 1;
		if(dLoc >= numPeriods)
			break;
	}

	periods = new Period**[MAXVARIDS];
	for(int p = 0; p < MAXVARIDS; p++)
		periods[p] = NULL;
	int v;
	for(v = 0; v < nVarIDs; v++)
	{
		//create data storage structure
		COleDateTime m1, m2;
		periods[varIDs[v]] = new Period*[numPeriods];
		for(int i = 0; i < numPeriods; i++)
			periods[varIDs[v]][i] = new Period(sDates[i], eDates[i]);
		if(OptionFromVarID(varIDs[v]) == 1)
			useNFDRS = true;
		if(OptionFromVarID(varIDs[v]) == 3)
			useCanadian = true;
	}

	//DATA STORAGE CREATED.
	//now process selected station(s)
	CSIGStationSet staSet(pDB);
	staSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
	staSet.Open();
	CString  temp, staQuery;//query = "",

	if(fpSet->m_SIG_Station.GetLength() <= 6)//a single station
	{
		//set up SIGStationSet
		staQuery.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
		staSet.m_strFilter = _T(staQuery);
		staSet.Requery();
		m_strStationName = staSet.m_Name;
		AnalyzeQuickStation(pClim);
	}
	else
	//is a SIG - multiple stations
		AnalyzeQuickStation(pClim);

	//weather data read and analyzed....
	//make sure we have some data
	bool hasSomeData = false;
	for(v = 0; v < nVarIDs; v++)
	{
		for(int i = 0; i < numPeriods; i++)
		{
			if(periods[varIDs[v]][i]->N() > 0)
			{
				hasSomeData = true;
				break;
			}
		}
	}
	//now get fires
	if(checkFires)//get them from the CRawClim!
	{
		fireYears = eYear - sYear + 1;
		fires = new CFireDay *[fireYears];
		int y;
		for(y = 0; y < fireYears; y++)
			fires[y] = new CFireDay [numPeriods];
		//have query for fires, need to process and put into fires structure
		//note that will still have to treat fires by individual days,
		//then put days into periods
		double lA = 0.0, hA = 0.0;
		long lF = 0, hF = 0;
	
		int yearOffset = sYear - pClim->m_baseYear;
		int pClimYear = 0;
			for(y = 0; y < fireYears; y++)
			{
				//for each year...
				for(int i = 0; i < numPeriods; i++)
				{
					//see if there's fire days - look at each day
					//deal with leap years
						//TRACE1("%d\n", DayOfYear(periods[varIDs[0]][i]->end));
					for(int d = DayOfYear(periods[varIDs[0]][i]->start) - 1;
						d <= DayOfYear(periods[varIDs[0]][i]->end) - 1;
						d++)
					{
						pClimYear = y + yearOffset;
						if (pClimYear >= 0)
						{
							fires[y][i].lightning = max(fires[y][i].lightning, pClim->m_fires[pClimYear][d].lightning);
							fires[y][i].human = max(fires[y][i].human, pClim->m_fires[pClimYear][d].human);
							fires[y][i].lAcresMax = max(fires[y][i].lAcresMax, pClim->m_fires[pClimYear][d].lAcresMax);
							fires[y][i].hAcresMax = max(fires[y][i].hAcresMax, pClim->m_fires[pClimYear][d].hAcresMax);
						}
					}
				}
			}
	}
	if (pClim->m_sigStaID.GetLength() > 6)//sig
	{
		char sig[64];
		strcpy_s(sig, fpSet->m_SIG_Station);
		CString temp;
		CStationInSIGSet sSet(pDB);
		temp.Format("[SIG] = '%-20.20s'", &sig[6]);
		sSet.m_strFilter = _T(temp);
		sSet.Open();
		if (!sSet.IsEOF())
		{
			staSet.m_strFilter.Format("[StationID] = '%6.6s'", sSet.m_StationID);
			staSet.Requery();
			if (!staSet.IsEOF())
			{
				m_strModel = GetModelString(&staSet);
			}
		}
		sSet.Close();
	}
	else//single station
		m_strModel = GetModelString(&staSet);
	staSet.Close();
	if(!inverted)
	{//need to get auxYears and fires
		CClimAnalysis tAnalysis(pDB);
		tAnalysis.AnalyzeQuick(pClim, true);//caller, tVarIDs, nVarIDs, fpSet, true) == 0)
			tAnalysis.killPeriods = false;
			m_opts.numtPeriods = tAnalysis.numPeriods;
			m_opts.tPeriods = tAnalysis.periods;
			m_opts.tFires = tAnalysis.fires;
	}
}

void CClimAnalysis::AnalyzeQuickStation(CRawClim *pClim)
{
	int ret = 0;
	//if(recCounter)
	//{
	//	recCounter->SetStatusString("Reading: " + stationStr);
	//}
	bool goodRecord;
	//create data structures based on periods and selected outputs
		//working variables
	COleDateTime wxDay;
	int sYear, eYear;
	sYear = sDates[0].GetYear();
	eYear = eDates[0].GetYear();
	if (sYear < pClim->m_baseYear)
		sYear = pClim->m_baseYear;
	if (eYear > pClim->m_baseYear + pClim->m_nYears - 1)
		eYear = pClim->m_baseYear + pClim->m_nYears - 1;
	if(sYear > 0 && eYear > 0)
	{
		for(int p = 0; p < numPeriods; p++)
		{//for each period...
			for(int y = sYear - pClim->m_baseYear; y < eYear - pClim->m_baseYear + 1; y++)
			{//of each year...
				COleDateTime tStart, tEnd;
				COleDateTimeSpan oneDay(1, 0, 0, 0);
				tStart.SetDateTime(pClim->m_baseYear + y, periods[0][p]->start.GetMonth(), periods[0][p]->start.GetDay(),
					13, 0, 0);
				tEnd.SetDateTime(pClim->m_baseYear + y, periods[0][p]->end.GetMonth(), periods[0][p]->end.GetDay(),
					13, 0, 0);
				if(tEnd.GetStatus() != COleDateTime::valid || tStart.GetStatus()  != COleDateTime::valid)
					continue;
				int days = tEnd - tStart + oneDay;//periods[0][p]->end.GetDayOfYear() - periods[0][p]->start.GetDayOfYear() + 1;
				if(days <= 0)
					days = tStart - tEnd;// + oneDay;//periods[0][p]->start.GetDayOfYear() - periods[0][p]->end.GetDayOfYear() + 1;
				if(IsLeap(pClim->m_baseYear + y) && periods[0][p]->end.GetDayOfYear() == 59)
					days++;
				for(int d = 0; d < days; d++)
				{//for each day of period...
					wxDay = periods[0][p]->start;
					wxDay.SetDateTime(pClim->m_baseYear + y, periods[0][p]->start.GetMonth(),
						periods[0][p]->start.GetDay() + d, 13, 0, 0);
					if(wxDay.GetStatus() != COleDateTime::valid)
						continue;
					int climDay = wxDay.GetDayOfYear();
					if(!IsLeap(wxDay.GetYear()) && climDay > 59)
						climDay++;
					//if(IsLeap(wxDay.GetYear()) && climDay > 59)
						//climDay--;
					climDay--;//zero based!!!
					//TRACE1("ClimDay = %d\n", climDay);
					goodRecord = false;
					for(int v = 0; v < nVarIDs; v++)
					{
						if(pClim->m_wxData[v]->m_vals[y][climDay] != NODATA_VAL)
						{
							goodRecord = true;
							break;
						}
					}
					if(goodRecord)
					{
						for(int v = 0; v < nVarIDs; v++)
						{
							if(pClim->m_wxData[v]->m_vals[y][climDay]!= NODATA_VAL)
								periods[v][p]->Accumulate(wxDay, pClim->m_wxData[v]->m_vals[y][climDay]);
						}
						count++;
					}
				}
			}
		}
	}
	//process user variables
	if(HasUserVars())
		userCount += AccumulateStationUserVars(pClim->m_sigStaID);
	for(int j = 0; j < MAXVARIDS; j++)
	{
		if(periods[j])
		{
			for(int i = 0; i < numPeriods; i++)
				periods[j][i]->Complete();
		}
	}
}

void CClimAnalysis::AnalyzeQuickSIG(CRawClim *pClim)
{
}

CString CClimAnalysis::StatsGraphDataListing(int varID, bool createDoc /*= true*/)
{
	CString retName = "";
// added for batch2
	// write the stats in text form to temporary file

	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	fprintf(stream, "FireFamily Plus Fire-Day Climatology Stats\n");

	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	CString extremeString = " ";
	if (m_usedExtremes)
	{
		if (m_extremeIsMax[varID])
			extremeString = " - Daily Max";
		else
			extremeString = " - Daily Min";
	}
	fprintf(stream,"\nVariable: %s%s\n",      //climSet->m_Variable_Name);
              varNames[varID], extremeString);
	if (customInits)
		fprintf(stream,"(Calculated using at least one custom yearly init)\n");
	fprintf(stream,"\n");

	reportSet.MoveFirst();
	int Delimiter = reportSet.m_Delimiter;
	reportSet.Close();

	switch (Delimiter){
		case 0:
			fprintf(stream,"Period       Mean        Min        Max        St. Dev.");
			for(int i = 0; i < OVERLAYS; i++)
		        if(m_opts.overLayYears[i] > 0 )
				{
					if (i==0)
					   fprintf(stream,"    %d",m_opts.overLayYears[i]);
					else
					   fprintf(stream,"       %d",m_opts.overLayYears[i]);
				}
			break;
		case 1:
			fprintf(stream,"Period\tMean\tMin\tMax\tSt. Dev.");
			for(int i = 0; i < OVERLAYS; i++)
		        if(m_opts.overLayYears[i] > 0 )
					fprintf(stream,"\t%d",m_opts.overLayYears[i]);
			break;
		case 2:
			fprintf(stream,"Period,Mean,Min,Max,St. Dev.");
			for(int i = 0; i < OVERLAYS; i++)
		        if(m_opts.overLayYears[i] > 0 )
					fprintf(stream,",%d",m_opts.overLayYears[i]);
			break;
		case 3:
		case 4:
			fprintf(stream,"Period|Mean|Min|Max|St. Dev.");
			for(int i = 0; i < OVERLAYS; i++)
		        if(m_opts.overLayYears[i] > 0 )
					fprintf(stream,"|%d",m_opts.overLayYears[i]);
			break;
	}

	fprintf(stream,"\n-----------------------------------------------------");
	for(int i = 0; i < OVERLAYS; i++)
		     if(m_opts.overLayYears[i] > 0 )
			 {
				 if (Delimiter==0)
				     fprintf(stream,"-----------");
				 else
					 fprintf(stream,"------");
			 }

	fprintf(stream,"\n");

	if(varID == 8)
	{
		Period ** pcpSums = NULL;
		pcpSums = new Period *[numPeriods];
        /* need overlays now too  2012/12 */
		Period ** pcpAuxSums[OVERLAYS];

		int p;
		for(p = 0; p < numPeriods; p++)
		{
			pcpSums[p] = new Period(periods[varID][p]->start, periods[varID][p]->end);
		}
		for (int i=0; i < OVERLAYS; i++)
		{
			pcpAuxSums[i] = new Period *[numPeriods];
			for(p = 0; p < numPeriods; p++)
		    {
			   pcpAuxSums[i][p] = new Period(periods[varID][p]->start, periods[varID][p]->end);
		    }
		}

		long minPrecipObs = 0;//GetMinPrecipObs(pDoc->, COleDateTime end);

		for(p = 0; p < numPeriods; p++)
		{
			minPrecipObs = GetMinPrecipObs(periods[varID][p]->start, periods[varID][p]->end);
			for(int y = 0; y < periods[varID][p]->years; y++)
			{
				if(periods[varID][p]->yStats[y].N() >= minPrecipObs)
				{
					COleDateTime pDate;
					pDate.SetDate(periods[varID][p]->start.GetYear() + y, periods[varID][p]->start.GetMonth(),
						periods[varID][p]->start.GetDay());
					pcpSums[p]->Accumulate(pDate, periods[varID][p]->yStats[y].Mean()
						* periods[varID][p]->yStats[y].N());
				}
			}
			pcpSums[p]->Complete();
		}

		// now get overlays

		for(int i = 0; i < OVERLAYS; i++)
		{
			if(m_opts.overLayYears[i] > 0  )
			{
				int loc = m_opts.overLayYears[i] - periods[varID][0]->baseYear;

				if(loc < 0 || loc >= periods[varID][0]->years && m_opts.tPeriods)
				{
					loc = m_opts.overLayYears[i] - m_opts.tPeriods[varID][0]->baseYear;
					int k;
					for(k = 0; k < m_opts.numtPeriods; k++)
					{
						if(m_opts.tPeriods[varID][k]->yStats[loc].N() > 0)
						{
							COleDateTime pDate;
							pDate.SetDate(m_opts.overLayYears[i], m_opts.tPeriods[varID][k]->start.GetMonth(),
								m_opts.tPeriods[varID][k]->start.GetDay());
							pcpAuxSums[i][k]->mean=  m_opts.tPeriods[varID][k]->yStats[loc].Mean();
								
							
						}
					}
			    } else
				{
					int k;
					for(k = 0; k < numPeriods; k++)
					{
						if(periods[varID][k]->yStats[loc].N() > 0)
						{
							COleDateTime pDate;
							pDate.SetDate(m_opts.overLayYears[i], periods[varID][k]->start.GetMonth(),
								periods[varID][k]->start.GetDay());
							pcpAuxSums[i][k]->mean = periods[varID][k]->yStats[loc].Mean();
								
							
						}
					}


				}

			}
		}

		for(int j = 0; j < numPeriods; j++)
		{
			if(periods[varID][j]->N() > 0)
			{
				switch (Delimiter)
				{
				case 0:
					fprintf(stream, "%d/%d	     %6.2f     %6.2f      %6.2f     %6.2f",
						periods[varID][j]->start.GetMonth(),
						periods[varID][j]->start.GetDay(),
						pcpSums[j]->mean, pcpSums[j]->min, pcpSums[j]->max, pcpSums[j]->stdDev   );

					for(int i = 0; i < OVERLAYS; i++)	
						if(m_opts.overLayYears[i] > 0 )
						   fprintf(stream,"     %6.2f",pcpAuxSums[i][j]->mean);
						  
					   

					fprintf(stream,"\n");
					break;
				case 1:
					fprintf(stream, "%d/%d\t%6.2f\t%6.2f\t%6.2f\t%6.2f",
						periods[varID][j]->start.GetMonth(),
						periods[varID][j]->start.GetDay(),
						pcpSums[j]->mean, pcpSums[j]->min, pcpSums[j]->max, pcpSums[j]->stdDev   );

					for(int i = 0; i < OVERLAYS; i++)
					   if(m_opts.overLayYears[i] > 0 )
						    fprintf(stream,"\t%6.2f",pcpAuxSums[i][j]->mean);

					fprintf(stream,"\n");
					break;
				case 2:
					fprintf(stream, "%d/%d,%6.2f,%6.2f,%6.2f,%6.2f",
						periods[varID][j]->start.GetMonth(),
						periods[varID][j]->start.GetDay(),
						pcpSums[j]->mean, pcpSums[j]->min, pcpSums[j]->max, pcpSums[j]->stdDev   );

					for(int i = 0; i < OVERLAYS; i++)
					  if(m_opts.overLayYears[i] > 0 )
						    fprintf(stream,",%6.2f",pcpAuxSums[i][j]->mean);

					fprintf(stream,"\n");
					break;
				case 3:
				case 4:
					fprintf(stream, "%d/%d|%6.2f|%6.2f|%6.2f|%6.2f",
						periods[varID][j]->start.GetMonth(),
						periods[varID][j]->start.GetDay(),
						pcpSums[j]->mean, pcpSums[j]->min, pcpSums[j]->max, pcpSums[j]->stdDev   );

					for(int i = 0; i < OVERLAYS; i++)
					   if(m_opts.overLayYears[i] > 0 )
						    fprintf(stream,"|%6.2f",pcpAuxSums[i][j]->mean);
				

					fprintf(stream,"\n");
					break;
				}
			}
		}
		if(pcpSums)
		{
			for(int p = numPeriods - 1; p >= 0; p--)
				delete pcpSums[p];
			delete[] pcpSums;
		}
		if (pcpAuxSums)
		{
            for(int i = 0; i < OVERLAYS; i++)
			{
                  for(int p = numPeriods - 1; p >= 0; p--)
				     delete pcpAuxSums[i][p];

			      delete pcpAuxSums[i];
			}
		}
	}
	else
	{
		 /* need overlays now too  2012/12 */
		Period ** auxSums[OVERLAYS];

		for (int i=0; i < OVERLAYS; i++)
		{
			auxSums[i] = new Period *[numPeriods];
			for(int p = 0; p < numPeriods; p++)
		    {
			   auxSums[i][p] = new Period(periods[varID][p]->start, periods[varID][p]->end);
		    }
		}

		// now get overlays
		// ( means for each aux year's period )

		for(int i = 0; i < OVERLAYS; i++)
		{
			if(m_opts.overLayYears[i] > 0  )
			{
				int loc = m_opts.overLayYears[i] - periods[varID][0]->baseYear;

				if(loc < 0 || loc >= periods[varID][0]->years && m_opts.tPeriods)
				{
					loc = m_opts.overLayYears[i] - m_opts.tPeriods[varID][0]->baseYear;
					int k;
					for(k = 0; k < m_opts.numtPeriods; k++)
					{
						if(m_opts.tPeriods[varID][k]->yStats[loc].N() > 0)
						{
							COleDateTime pDate;
							pDate.SetDate(m_opts.overLayYears[i], m_opts.tPeriods[varID][k]->start.GetMonth(),
								m_opts.tPeriods[varID][k]->start.GetDay());
							auxSums[i][k]->mean=  m_opts.tPeriods[varID][k]->yStats[loc].Mean();
								
							
						}
					}
			    } else
				{
					int k;
					for(k = 0; k < numPeriods; k++)
					{
						if(periods[varID][k]->yStats[loc].N() > 0)
						{
							COleDateTime pDate;
							pDate.SetDate(m_opts.overLayYears[i], periods[varID][k]->start.GetMonth(),
								periods[varID][k]->start.GetDay());
							auxSums[i][k]->mean = periods[varID][k]->yStats[loc].Mean();
								
							
						}
					}


				}

			}
		}
		

		for(int j = 0; j < numPeriods; j++)
		{
			if(periods[varID][j]->N() > 0)
			{
				switch (Delimiter)
				{
				case 0:
					fprintf(stream, "%d/%d	     %6.2f     %6.2f      %6.2f     %6.2f",
						periods[varID][j]->start.GetMonth(),
						periods[varID][j]->start.GetDay(),
						periods[varID][j]->mean,
						periods[varID][j]->min,
						periods[varID][j]->max,
						periods[varID][j]->stdDev  );

					for(int i = 0; i < OVERLAYS; i++)
					  if(m_opts.overLayYears[i] > 0 )
						    fprintf(stream,"     %6.2f",auxSums[i][j]->mean);
					
					fprintf(stream,"\n");

					break;
				case 1:
					fprintf(stream, "%d/%d\t%6.2f\t%6.2f\t%6.2f\t%6.2f",
						periods[varID][j]->start.GetMonth(),
						periods[varID][j]->start.GetDay(),
						periods[varID][j]->mean,
						periods[varID][j]->min,
						periods[varID][j]->max,
						periods[varID][j]->stdDev  );

					for(int i = 0; i < OVERLAYS; i++)
					   if(m_opts.overLayYears[i] > 0 )
						    fprintf(stream,"\t%6.2f",auxSums[i][j]->mean);

					fprintf(stream,"\n");
					break;
				case 2:
					fprintf(stream, "%d/%d,%6.2f,%6.2f,%6.2f,%6.2f",
						periods[varID][j]->start.GetMonth(),
						periods[varID][j]->start.GetDay(),
						periods[varID][j]->mean,
						periods[varID][j]->min,
						periods[varID][j]->max,
						periods[varID][j]->stdDev  );

					for(int i = 0; i < OVERLAYS; i++)
					   if(m_opts.overLayYears[i] > 0 )
						    fprintf(stream,",%6.2f",auxSums[i][j]->mean);

					fprintf(stream,"\n");
					break;
				case 3:
				case 4:
					fprintf(stream, "%d/%d|%6.2f|%6.2f|%6.2f|%6.2f",
						periods[varID][j]->start.GetMonth(),
						periods[varID][j]->start.GetDay(),
						periods[varID][j]->mean,
						periods[varID][j]->min,
						periods[varID][j]->max,
						periods[varID][j]->stdDev  );

					for(int i = 0; i < OVERLAYS; i++)
					   if(m_opts.overLayYears[i] > 0 )
						    fprintf(stream,"|%6.2f",auxSums[i][j]->mean);

					fprintf(stream,"\n");
					break;
				}
			}
		}

		if (auxSums)
		{
            for(int i = 0; i < OVERLAYS; i++)
			{
                  for(int p = numPeriods - 1; p >= 0; p--)
				     delete auxSums[i][p];

			      delete auxSums[i];
			}
		}
	}
	fclose(stream);
	retName = fName;
	// end added for batch
	//now display the file in a child window
	if(createDoc)
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
				CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(fName);
				pDoc->isTemp = true;
				free(fName);
				CString tTitle;
				tTitle.Format("%s - %s Stats Listing", fpSet->m_SIG_Station, varNames[varID]);//climSet->m_Variable_Name);
				pDoc->SetTitle(tTitle);
				return retName;
			}
		}
	}
	free(fName);
	return retName;
}

double CClimAnalysis::GetIndexValue(int varLoc, COleDateTime iDate)
{
	int p = GetPeriod(iDate);
	int y = iDate.GetYear() - periods[varLoc][0]->start.GetYear();
	if(periods[varLoc][p]->yStats[y].N() > 0)
		return periods[varLoc][p]->yStats[y].Mean();
	return -1.0;
}
void CClimAnalysis::LoadRunPocketOptions(CRunPocketCardSet *pocketSet)
{
	int _varID = pocketSet->m_varID - 1;
	m_pocketOpts.Destroy();
	m_pocketOpts.dbName = pocketSet->m_pDatabase->GetDatabaseName();
	CClimateSet tclimSet(pDB);
	//tclimSet.m_strSort = "[VarID]";
	tclimSet.setVarSortOrder(theApp.varSortOrder);
	tclimSet.Open();
	if (tclimSet.IsOpen())
	{
		while (!tclimSet.IsEOF()) 
		{
			if (tclimSet.m_VarID == (_varID + 1))
			{
				m_pocketOpts.m_IndexString = tclimSet.m_Variable_Name;
		        m_pocketOpts.m_IndexShort = tclimSet.m_ShortName;
                break;
			}
			tclimSet.MoveNext();
		}
		
	}
	m_pocketOpts.cp = ((double)pocketSet->m_CP) / 100.0;
	if(pocketSet->m_ExtraVar.GetLength() > 0)
	{
		m_pocketOpts.extraStr = pocketSet->m_ExtraVar;
		m_pocketOpts.extraStr.Trim();
		int tVarID = 1;
		tclimSet.MoveFirst();
		while (!tclimSet.IsEOF() && m_pocketOpts.extraStr.CompareNoCase(tclimSet.m_Variable_Name) != 0)
		{
			tclimSet.MoveNext();
		}
		tVarID = tclimSet.m_VarID;
		switch(tVarID)
		{
		case 24:
		case 33:
			m_pocketOpts.extraGreater = 2;
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			m_pocketOpts.extraGreater = 0;
			break;
		default:
			m_pocketOpts.extraGreater = 1;
		}
	}
	tclimSet.Close();
	m_pocketOpts.startYear = fpSet->m_StartYear;
	m_pocketOpts.endYear = fpSet->m_EndYear;
	m_pocketOpts.m_Area = pocketSet->m_FireDangerArea;
	m_pocketOpts.m_Bitmap = pocketSet->m_AreaBitmap;
	m_pocketOpts.m_Bitmap.TrimRight();
	m_pocketOpts.areaDIB = NULL;
	m_pocketOpts.areaPal = NULL;
	m_pocketOpts.dateStamp = AddDateStamp();
	if(m_pocketOpts.m_Bitmap.GetLength() > 0)
	{
		char bmStr[256];
		strcpy_s(bmStr, m_pocketOpts.m_Bitmap);
		m_pocketOpts.areaDIB = LoadDIB(bmStr);//ReadDIBFile(bmpFile);
		if(m_pocketOpts.areaDIB)
		{
			LPSTR lpDIB = (LPSTR)GlobalLock((HGLOBAL)m_pocketOpts.areaDIB);
			m_pocketOpts.areaRect.left = 0;
			m_pocketOpts.areaRect.top = 0;
			m_pocketOpts.areaRect.right = DIBWidth(lpDIB);
			m_pocketOpts.areaRect.bottom = DIBHeight(lpDIB);
			GlobalUnlock((HGLOBAL) m_pocketOpts.areaDIB);
		}
		m_pocketOpts.m_FuelModel = "";
		bool isNFDRS = (OptionFromVarID(_varID) == 1) ? true : false;
		if(isNFDRS)
		{
			CString findSta = "";
			if(fpSet->m_SIG_Station.GetLength() > 6)//SIG
			{
				CStationInSIGSet sigSet(pDB);
				char tmpStr[64];
				strcpy_s(tmpStr, fpSet->m_SIG_Station);
				sigSet.m_strFilter.Format("[SIG] = '%s'", &tmpStr[6]);
				sigSet.Open();
				if(!sigSet.IsBOF())
					findSta = sigSet.m_StationID;
				sigSet.Close();
			}
			else
				findSta = fpSet->m_SIG_Station;
			if(findSta.GetLength() > 0)
			{
				CSIGStationSet staSet(pDB);
				staSet.m_strFilter.Format("[StationID] = '%6.6s'", findSta);
				staSet.Open();
				m_pocketOpts.m_FuelModel = GetModelString(&staSet);
				staSet.Close();
			}
			if(m_pocketOpts.m_FuelModel.GetLength() > 0)
			{
				int strLoc = m_pocketOpts.m_FuelModel[1] - 'A';
				m_pocketOpts.m_FuelModel = modelStrings[strLoc];
			}
		}
	}
	m_pocketOpts.m_Index = pocketSet->m_Index;
	m_pocketOpts.m_PEText = pocketSet->m_PastExperience;
	m_pocketOpts.m_RH = pocketSet->m_RH;
	m_pocketOpts.m_Temp = pocketSet->m_Temp;
	m_pocketOpts.m_WindSpeed = pocketSet->m_WindSpeed;
	m_pocketOpts.m_LiveFM = pocketSet->m_LiveFM;
	m_pocketOpts.m_Year1 = pocketSet->m_Year1;
	if(m_pocketOpts.m_Year1 < sDates[0].GetYear())
		m_pocketOpts.m_Year1 = sDates[0].GetYear();
	if(m_pocketOpts.m_Year1 > eDates[numPeriods - 1].GetYear())
		m_pocketOpts.m_Year1 = eDates[numPeriods - 1].GetYear();
	m_pocketOpts.m_Year2 = pocketSet->m_Year2;
	if(m_pocketOpts.m_Year2 < sDates[0].GetYear())
		m_pocketOpts.m_Year2 = sDates[0].GetYear();
	if(m_pocketOpts.m_Year2 > eDates[numPeriods - 1].GetYear())
		m_pocketOpts.m_Year2 = eDates[numPeriods - 1].GetYear();
	m_pocketOpts.periodLength = atoi(fpSet->m_PeriodLength);
	m_pocketOpts.numPeriods = numPeriods;
	m_pocketOpts.varID = _varID - 1;
	m_pocketOpts.gyPcnt = pocketSet->m_pcGyVal;
	m_pocketOpts.ryPcnt = pocketSet->m_pcRyVal;
	m_pocketOpts.yPcnt = pocketSet->m_pcYVal;
	//add fires
	m_pocketOpts.m_Fire1 = pocketSet->m_Fire1;
	m_pocketOpts.m_Fire1.TrimRight();
	m_pocketOpts.m_Fire1Date = pocketSet->m_Fire1Date;
	m_pocketOpts.m_Fire2 = pocketSet->m_Fire2;
	m_pocketOpts.m_Fire2.TrimRight();
	m_pocketOpts.m_Fire2Date = pocketSet->m_Fire2Date;
	m_pocketOpts.m_Fire3 = pocketSet->m_Fire3;
	m_pocketOpts.m_Fire3.TrimRight();
	m_pocketOpts.m_Fire3Date = pocketSet->m_Fire3Date;
	m_pocketOpts.m_Line1 = pocketSet->m_Line1;
	m_pocketOpts.m_Line2 = pocketSet->m_Line2;
	m_pocketOpts.m_Line3 = pocketSet->m_Line3;
	m_pocketOpts.m_NWCGStd = pocketSet->m_NWCGStd;
	m_pocketOpts.m_RespAgency = pocketSet->m_RespAgency;
	if(m_pocketOpts.m_Fire1.GetLength() > 0)
	{
		m_pocketOpts.m_Fire1Val = GetIndexValue(_varID, m_pocketOpts.m_Fire1Date);
	}
	if(m_pocketOpts.m_Fire2.GetLength() > 0)
	{
		m_pocketOpts.m_Fire2Val = GetIndexValue(_varID, m_pocketOpts.m_Fire2Date);
	}
	if(m_pocketOpts.m_Fire3.GetLength() > 0)
	{
		m_pocketOpts.m_Fire3Val = GetIndexValue(_varID, m_pocketOpts.m_Fire3Date);
	}
}

// new 2013
bool CClimAnalysis::HasDLOutput()
{
	try
	{
		CRecordset trecs(pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM DL_Output"))
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



// new 2013
// see ClimAnalysis->DailyListing()
void CClimAnalysis::CreateDLOutput()
{

	try
	{
		CString strSql =
			"CREATE TABLE [DL_Output] ("
            "[ID] INTEGER,"
			"[ObsDate] VARCHAR(30) NOT NULL,"
			"CONSTRAINT IDConstraint PRIMARY KEY ([ID]))";
		pDB->ExecuteSQL(strSql);
	}
	catch(CDBException* e)
	{
		DisplayDBException(e);
		e->Delete();
	}
}

void CClimAnalysis::LoadPocketOptions(CPocketCardSet *pocketSet, int _varID)
{
	m_pocketOpts.Destroy();
	m_pocketOpts.dbName = pocketSet->m_pDatabase->GetDatabaseName();
	CClimateSet tclimSet(pDB);
	tclimSet.setVarSortOrder(theApp.varSortOrder);
	tclimSet.Open();
	if (tclimSet.IsOpen())
	{
		while (!tclimSet.IsEOF()) 
		{
			if (tclimSet.m_VarID == _varID + 1)
			{
				m_pocketOpts.m_IndexString = tclimSet.m_Variable_Name;
				m_pocketOpts.m_IndexShort = tclimSet.m_ShortName;
				break;

			}
			tclimSet.MoveNext();
		}
		
	}
	m_pocketOpts.cp = ((double)pocketSet->m_CP) / 100.0;
	if(pocketSet->m_ExtraVar.GetLength() > 0)
	{
		m_pocketOpts.extraStr = pocketSet->m_ExtraVar;
		m_pocketOpts.extraStr.Trim();
		int tVarID = 1;
		tclimSet.MoveFirst();
		while (!tclimSet.IsEOF() && m_pocketOpts.extraStr.CompareNoCase(tclimSet.m_Variable_Name) != 0)
		{
			tclimSet.MoveNext();
		}
		tVarID = tclimSet.m_VarID;
		switch(tVarID)
		{
		case 24:
		case 33:
			m_pocketOpts.extraGreater = 2;
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			m_pocketOpts.extraGreater = 0;
			break;
		default:
			m_pocketOpts.extraGreater = 1;
		}
	}
	tclimSet.Close();
	m_pocketOpts.startYear = fpSet->m_StartYear;
	m_pocketOpts.endYear = fpSet->m_EndYear;
	m_pocketOpts.m_Area = pocketSet->m_FireDangerArea;
	m_pocketOpts.m_Bitmap = pocketSet->m_AreaBitmap;
	m_pocketOpts.m_Bitmap.TrimRight();
	m_pocketOpts.areaDIB = NULL;
	m_pocketOpts.areaPal = NULL;
	m_pocketOpts.dateStamp = AddDateStamp();
	if(m_pocketOpts.m_Bitmap.GetLength() > 0)
	{
		char bmStr[256];
		strcpy_s(bmStr, m_pocketOpts.m_Bitmap);
		m_pocketOpts.areaDIB = LoadDIB(bmStr);//ReadDIBFile(bmpFile);
		if(m_pocketOpts.areaDIB)
		{
			LPSTR lpDIB = (LPSTR)GlobalLock((HGLOBAL)m_pocketOpts.areaDIB);
			m_pocketOpts.areaRect.left = 0;
			m_pocketOpts.areaRect.top = 0;
			m_pocketOpts.areaRect.right = DIBWidth(lpDIB);
			m_pocketOpts.areaRect.bottom = DIBHeight(lpDIB);
			//GlobalUnlock((HGLOBAL) pDoc->areaRect);
			GlobalUnlock((HGLOBAL) m_pocketOpts.areaDIB);
		}
	}
		m_pocketOpts.m_FuelModel = "";
		bool isNFDRS = (OptionFromVarID(_varID) == 1) ? true : false;
		if(isNFDRS)
		{
			CString findSta = "";
			if(fpSet->m_SIG_Station.GetLength() > 6)//SIG
			{
				CStationInSIGSet sigSet(pDB);
				char tmpStr[64];
				strcpy_s(tmpStr, fpSet->m_SIG_Station);
				sigSet.m_strFilter.Format("[SIG] = '%s'", &tmpStr[6]);
				sigSet.Open();
				if(!sigSet.IsBOF())
					findSta = sigSet.m_StationID;
				sigSet.Close();
			}
			else
				findSta = fpSet->m_SIG_Station;
			if(findSta.GetLength() > 0)
			{
				CSIGStationSet staSet(pDB);
				staSet.m_strFilter.Format("[StationID] = '%6.6s'", findSta);
				staSet.Open();
				m_pocketOpts.m_FuelModel = GetModelString(&staSet);
				staSet.Close();
			}
			if(m_pocketOpts.m_FuelModel.GetLength() > 0)
			{
				char firstChar = m_pocketOpts.m_FuelModel[0];
				int strLoc;
				if (firstChar >= 'V' && firstChar <= 'Z')
					strLoc = firstChar - 'A';
				else
					strLoc = m_pocketOpts.m_FuelModel[1] - 'A';
				if (strLoc < 0)
					strLoc = 0;
				if (strLoc > ('Z' - 'A'))
					strLoc = 0;
				m_pocketOpts.m_FuelModel = modelStrings[strLoc];
			}
		}
	m_pocketOpts.m_Index = pocketSet->m_Index;
	m_pocketOpts.m_PEText = pocketSet->m_PastExperience;
	m_pocketOpts.m_RH = pocketSet->m_RH;
	m_pocketOpts.m_Temp = pocketSet->m_Temp;
	m_pocketOpts.m_WindSpeed = pocketSet->m_WindSpeed;
	m_pocketOpts.m_LiveFM = pocketSet->m_LiveFM;
	m_pocketOpts.m_Year1 = pocketSet->m_Year1;
	if(m_pocketOpts.m_Year1 < sDates[0].GetYear())
		m_pocketOpts.m_Year1 = sDates[0].GetYear();
	if(m_pocketOpts.m_Year1 > eDates[numPeriods - 1].GetYear())
		m_pocketOpts.m_Year1 = eDates[numPeriods - 1].GetYear();
	m_pocketOpts.m_Year2 = pocketSet->m_Year2;
	if(m_pocketOpts.m_Year2 < sDates[0].GetYear())
		m_pocketOpts.m_Year2 = sDates[0].GetYear();
	if(m_pocketOpts.m_Year2 > eDates[numPeriods - 1].GetYear())
		m_pocketOpts.m_Year2 = eDates[numPeriods - 1].GetYear();
	m_pocketOpts.periodLength = atoi(fpSet->m_PeriodLength);
	m_pocketOpts.numPeriods = numPeriods;
	m_pocketOpts.varID = _varID;
	m_pocketOpts.gyPcnt = pocketSet->m_pcGyVal;
	m_pocketOpts.ryPcnt = pocketSet->m_pcRyVal;
	m_pocketOpts.yPcnt = pocketSet->m_pcYVal;
	//add fires
	m_pocketOpts.m_Fire1 = pocketSet->m_Fire1;
	m_pocketOpts.m_Fire1.TrimRight();
	m_pocketOpts.m_Fire1Date = pocketSet->m_Fire1Date;
	m_pocketOpts.m_Fire2 = pocketSet->m_Fire2;
	m_pocketOpts.m_Fire2.TrimRight();
	m_pocketOpts.m_Fire2Date = pocketSet->m_Fire2Date;
	m_pocketOpts.m_Fire3 = pocketSet->m_Fire3;
	m_pocketOpts.m_Fire3.TrimRight();
	m_pocketOpts.m_Fire3Date = pocketSet->m_Fire3Date;
	m_pocketOpts.m_Line1 = pocketSet->m_Line1;
	m_pocketOpts.m_Line2 = pocketSet->m_Line2;
	m_pocketOpts.m_Line3 = pocketSet->m_Line3;
	m_pocketOpts.m_NWCGStd = pocketSet->m_NWCGStd;
	m_pocketOpts.m_RespAgency = pocketSet->m_RespAgency;
	if(m_pocketOpts.m_Fire1.GetLength() > 0)
	{
		m_pocketOpts.m_Fire1Val = GetIndexValue(_varID, m_pocketOpts.m_Fire1Date);
	}
	if(m_pocketOpts.m_Fire2.GetLength() > 0)
	{
		m_pocketOpts.m_Fire2Val = GetIndexValue(_varID, m_pocketOpts.m_Fire2Date);
	}
	if(m_pocketOpts.m_Fire3.GetLength() > 0)
	{
		m_pocketOpts.m_Fire3Val = GetIndexValue(_varID, m_pocketOpts.m_Fire3Date);
	}
}

int CClimAnalysis::Analyze2(int* _varIDs, int _nVarIDs, CFireplusSet* _fpSet, bool isFPA /*= false*/)
{
	int ret = 0;
	CWaitCursor wait;
	nVarIDs = _nVarIDs;
	varIDs = _varIDs;
	//model use flags - optimize to only calculate models used
	killPeriods = true;
	fpSet = _fpSet;
	useCanadian = false;
	useNFDRS = false;
	useLFI = false;
	count = 0;
	userCount = 0;
	for (int v = 0; v < nVarIDs; v++)
	{
		if (OptionFromVarID(varIDs[v]) == 1)
			useNFDRS = true;
		if (OptionFromVarID(varIDs[v]) == 3)
			useCanadian = true;
		if (varIDs[v] == 39 || varIDs[v] == 42 || varIDs[v] == 43)
			useLFI = true;
	}
	int sYear = fpSet->m_StartYear, eYear = fpSet->m_EndYear;
	CSIGStationSet staSet(pDB);
	staSet.Open();
	CString query = "", temp, staQuery;
	//if (inverted)
	//{//need to know how many years
		CString wxQuery = "";
		CWxSet tSet(fpSet->m_pDatabase);

		tSet.m_strSort = "[ObsDate]";
		tSet.Open();
		if (fpSet->m_SIG_Station.GetLength() > 6)//SIG
		{
			char sig[64];
			strcpy_s(sig, fpSet->m_SIG_Station);
			CString temp;
			CStationInSIGSet sSet(pDB);
			temp.Format("[SIG] = '%-20.20s'", &sig[6]);
			sSet.m_strFilter = _T(temp);
			sSet.Open();
			staSet.m_strFilter.Format("[StationID] = '%6.6s'", sSet.m_StationID);
			while (!sSet.IsEOF())
			{
				tSet.m_strFilter.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", sSet.m_StationID);
				tSet.Requery();
				if (!tSet.IsBOF() && !tSet.IsEOF())
				{
					sYear = min(sYear, tSet.m_ObsDate.GetYear());
					tSet.MoveLast();
					eYear = max(eYear, tSet.m_ObsDate.GetYear());
				}
				sSet.MoveNext();
			}
			if (HasUserVars())
			{
				CString tmpStr = "", strSQL;
				sSet.MoveFirst();
				strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%s'", sSet.m_StationID);
				sSet.MoveNext();
				while (!sSet.IsEOF())
				{
					tmpStr.Format(" or [StationID] = '%6.6s'", sSet.m_StationID);
					strSQL += tmpStr;
					sSet.MoveNext();
				}
				CRecordset userVals(pDB);
				userVals.m_strSort = _T("[ObsDate]");
				userVals.Open(CRecordset::dynaset, strSQL);
				if (!userVals.IsBOF() && !userVals.IsEOF())
				{
					COleDateTime dt;
					CDBVariant var;
					userVals.GetFieldValue("ObsDate", var);
					sYear = min(sYear, var.m_pdate->year);
					userVals.MoveLast();
					userVals.GetFieldValue("ObsDate", var);
					eYear = max(eYear, var.m_pdate->year);
				}
				userVals.Close();
			}
			sSet.Close();
		}
		else //single station
		{
			staQuery.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
			staSet.m_strFilter = _T(staQuery);
			staSet.Requery();
			tSet.m_strFilter.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", fpSet->m_SIG_Station);
			tSet.Requery();
			if (!tSet.IsBOF() && !tSet.IsEOF())
			{
				sYear = min(sYear, tSet.m_ObsDate.GetYear());
				tSet.MoveLast();
				eYear = max(eYear, tSet.m_ObsDate.GetYear());
			}
			if (HasUserVars())
			{
				CRecordset userVals(pDB);
				CString strSQL;
				strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%s'", fpSet->m_SIG_Station);
				userVals.m_strSort = _T("[ObsDate]");
				userVals.Open(CRecordset::dynaset, strSQL);
				if (!userVals.IsBOF() && !userVals.IsEOF())
				{
					CDBVariant var;
					userVals.GetFieldValue("ObsDate", var);
					sYear = min(sYear, var.m_pdate->year);
					userVals.MoveLast();
					userVals.GetFieldValue("ObsDate", var);
					eYear = max(eYear, var.m_pdate->year);
				}
				userVals.Close();
			}
		}
		tSet.Close();
	//}
	int pLen = atoi(fpSet->m_PeriodLength);
	COleDateTimeSpan span(pLen - 1, 0, 0, 0);
	COleDateTime d1, d2, d3;
	numPeriods = 0;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d2 = d1 + span;
	// performance update 2012 - reduce number of Get () calls
	int d1M = d1.GetMonth(),
		d1D = d1.GetDay();

	//ensure don't cross month boundary
	while (d1M != d2.GetMonth())
		d2 -= 1;

	int d2M = d2.GetMonth(),
		d2D = d2.GetDay();

	while (1)
	{
		//new period coming into loop
		numPeriods++;
		if (MonthChtoInt(fpSet->m_EndMonth) == d1M
			&& fpSet->m_EndDay >= d1D
			&& fpSet->m_EndDay <= d2D)
			break;
		d3 = d2 + span;
		d3 += 1;
		if (d2M != d3.GetMonth())//does next period cross a month boundary?
		{
			if (d3.GetDay() <= pLen / 2)//will create a new period, otherwise will combine
				numPeriods++;
			if (MonthChtoInt(fpSet->m_EndMonth) == d1M
				&& fpSet->m_EndDay >= d1D
				&& fpSet->m_EndDay <= daysInMonth[d1M - 1])
				break;
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d1M = d1.GetMonth();
			d1D = d1.GetDay();

			d2 = d1 + span;
			d2M = d2.GetMonth();
			d2D = d2.GetDay();
		}
		else
		{
			d1 = d2;

			d1 += 1;
			d1M = d1.GetMonth();
			d1D = d1.GetDay();
			d2 = d1 + span;
			d2M = d2.GetMonth();
			d2D = d2.GetDay();
		}
		//ensure don't cross month boundary
		while (d1M != d2.GetMonth())
			d2 -= 1;

		d2M = d2.GetMonth();
		d2D = d2.GetDay();
	}

	//ok, got numPeriods. Allocate date arrays and fill values by repeating loop
	sDates = new COleDateTime[numPeriods];
	eDates = new COleDateTime[numPeriods];
	int dLoc = 0;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d1M = d1.GetMonth();
	d1D = d1.GetDay();
	d2 = d1 + span;
	//safety
	while (d1M != d2.GetMonth())
		d2 -= 1;
	d2M = d2.GetMonth();
	d2D = d2.GetDay();
	while (1)
	{
		//new period coming into loop
		sDates[dLoc].SetDate(sYear, d1M, d1D);
		eDates[dLoc++].SetDate(eYear, d1M, d2D);
		d3 = d2 + span;
		d3 += 1;
		if (d2M != d3.GetMonth()) //will next period cross a month boundary???
		{
			if (d3.GetDay() <= pLen / 2 && dLoc < numPeriods)
			{
				//extra group to fill
				sDates[dLoc].SetDate(sYear, d1M, d2D + 1);
				if (IsLeap(eYear))
				{
					eDates[dLoc++].SetDate(eYear, d1M, daysInMonth[d1M - 1]);
				}
				else
				{
					if (d1M != 2)
						eDates[dLoc++].SetDate(eYear, d1M, daysInMonth[d1M - 1]);
					else
						eDates[dLoc++].SetDate(eYear, d1M, 28);
				}
				if (dLoc >= numPeriods)
					break;
			}
			else
			{//combine into previous group
				if (d1M < MonthChtoInt(fpSet->m_EndMonth))
				{
					if (IsLeap(eYear))// % 4 == 0 && (eYear % 100 != 0 || eYear % 400 == 0))
						eDates[dLoc - 1].SetDate(eYear, d1M, daysInMonth[d1M - 1]);
					else
					{
						if (d1M != 2)
							eDates[dLoc - 1].SetDate(eYear, d1M, daysInMonth[d1M - 1]);
						else
							eDates[dLoc - 1].SetDate(eYear, d1M, 28);
					}
				}
				else//last month of query
				{
					if (IsLeap(eYear))// % 4 == 0 && (eYear % 100 != 0 || eYear % 400 == 0))
						eDates[dLoc - 1].SetDate(eYear, d1M, fpSet->m_EndDay);
					else//daysInMonth[d1.GetMonth() - 1]);
					{
						if (d1M != 2)
							eDates[dLoc - 1].SetDate(eYear, d1M, fpSet->m_EndDay);
						else
							eDates[dLoc - 1].SetDate(eYear, d1M, min(fpSet->m_EndDay, 28));
					}
				}
			}
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d1M = d1.GetMonth();
			d1D = d1.GetDay();
			d2 = d1 + span;
			d2M = d2.GetMonth();
			d2D = d2.GetDay();
		}
		else
		{//won't cross boundary, increment normally
			d1 = d2;
			d1 += 1;
			d1M = d1.GetMonth();
			d1D = d1.GetDay();
			d2 = d1 + span;
			d2M = d2.GetMonth();
			d2D = d2.GetDay();
		}
		//ensure same month
		while (d1M != d2.GetMonth())
			d2 -= 1;
		d2M = d2.GetMonth();
		d2D = d2.GetDay();
		if (dLoc >= numPeriods)
			break;
	}

	periods = new Period * *[MAXVARIDS];
	for (int p = 0; p < MAXVARIDS; p++)
		periods[p] = NULL;
	for (int v = 0; v < nVarIDs; v++)
	{
		//create data storage structure
		COleDateTime m1, m2;
		periods[varIDs[v]] = new Period * [numPeriods];
		for (int i = 0; i < numPeriods; i++)
			periods[varIDs[v]][i] = new Period(sDates[i], eDates[i]);
	}

	//DATA STORAGE CREATED.
	//now process selected station(s)
	//set up SIGStationSet
	fpSet->BuildBaseQuery2(query, "ObsDate", true, sYear, eYear);
	if (query.GetLength() > 0)
		query += " AND ";
	m_usedExtremes = theApp.m_UseDailyExtremes;
	if (fpSet->m_SIG_Station.GetLength() <= 6)//a single station
	{
		staQuery.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
		staSet.m_strFilter.Format(staQuery);
		staSet.Requery();
		CString temp2;

		bool reCalc2016 = false;
		reCalc2016 = theApp.m_ForceNFDRS2016Recompute;
		COleDateTime n2Start, n2End;
		fpSet->GetNFDRS2016Range(staSet.m_StationID, &n2Start, &n2End);
		if (!reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0])) // we need hourly data for new calculator?
		{
			CWxSet tWxSet(pDB);
			tWxSet.m_strFilter.Format("[StationID] = '%6.6s' AND [DailyObs] = 1 AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
				n2Start.Format(), n2End.Format());
			tWxSet.Open();
			if (tWxSet.IsFieldNull(&tWxSet.m_FM1) || tWxSet.IsFieldNull(&tWxSet.m_FM10) || tWxSet.IsFieldNull(&tWxSet.m_FM100) || tWxSet.IsFieldNull(&tWxSet.m_FM1000) || tWxSet.IsFieldNull(&tWxSet.m_FuelTemperature)
				|| tWxSet.m_FM1 <= 0.0 || tWxSet.m_FM10 <= 0.0 || tWxSet.m_FM100 <= 0.0 || tWxSet.m_FM1000 <= 0.0 || tWxSet.m_FuelTemperature <= -999.0)
				reCalc2016 = true;
			if (!reCalc2016)
			{
				tWxSet.MoveLast();
				if (tWxSet.IsFieldNull(&tWxSet.m_FM1) || tWxSet.IsFieldNull(&tWxSet.m_FM10) || tWxSet.IsFieldNull(&tWxSet.m_FM100) || tWxSet.IsFieldNull(&tWxSet.m_FM1000) || tWxSet.IsFieldNull(&tWxSet.m_FuelTemperature)
					|| tWxSet.m_FM1 <= 0.0 || tWxSet.m_FM10 <= 0.0 || tWxSet.m_FM100 <= 0.0 || tWxSet.m_FM1000 <= 0.0 || tWxSet.m_FuelTemperature <= -999.0)
					reCalc2016 = true;
			}
			tWxSet.Close();
		}
		if (!useNFDRS)
			reCalc2016 = false;
		if (!reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0]) && !m_usedExtremes)
			temp2.Format("[StationID] = '%6.6s' AND [DailyObs] = 1 AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
				n2Start.Format(), n2End.Format());
		else if (reCalc2016 && isNFDRS2016(staSet.m_NFDRSFM[0]))
		{
			query = "";
			temp2.Format("[StationID] = '%6.6s' AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
				n2Start.Format(), n2End.Format());
		}
		else if (m_usedExtremes && isNFDRS2016(staSet.m_NFDRSFM[0]))
			temp2.Format("[StationID] = '%6.6s' AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staSet.m_StationID,
				n2Start.Format(), n2End.Format());
		else
			temp2.Format("([StationID] = '%6.6s' AND [DailyObs] = 1)", staSet.m_StationID);
		query += temp2;
		if (useNFDRS && isNFDRS2016(staSet.m_NFDRSFM[0]))
			query += m_strRequiredFields;

		wxSet = new CWxSet(pDB);
		wxSet->m_strSort = _T("[ObsDate]");
		wxSet->m_strFilter = query;
		wxSet->Open();

		m_strStationName = staSet.m_Name;
		m_strModel = GetModelString(&staSet);

		//ret = AnalyzeStation(fpSet->m_SIG_Station + " - " + staSet.m_Name);
		ret = AnalyzeStation(CString(fpSet->m_SIG_Station + " - " + staSet.m_Name), isFPA, "");

		wxSet->Close();
		delete wxSet;
		wxSet = NULL;
	}
	else
		//is a SIG - multiple stations
	{
		ret = AnalyzeSIG(query, isFPA);

	}

	//weather data read and analyzed....
	//make sure we have some data
	if (pCUPDUPData->ShouldTerminate())
	{
		staSet.Close();
		return -1;
	}
	bool hasSomeData = false;
	for (int v = 0; v < nVarIDs; v++)
	{
		for (int i = 0; i < numPeriods; i++)
		{
			if (periods[varIDs[v]][i]->N() > 0)
			{
				hasSomeData = true;
				break;
			}
		}
	}
	if (!hasSomeData && !inverted)
	{
		AfxMessageBox("Error: No valid data values found.");
		ret = -2;
	}

	//now get fires
	if (checkFires && ret == 0)
	{
		fireYears = eYear - sYear + 1;
		firesStartYear = sYear;
		fires = new CFireDay * [fireYears];
		for (int y = 0; y < fireYears; y++)
			fires[y] = new CFireDay[numPeriods];
		CFiresSet fireSet(fpSet->m_pDatabase);
		fireSet.Open();
		//have query for fires, need to process and put into fires structure
		//note that will still have to treat fires by individual days,
		//then put days into periods
		double lA = 0.0, hA = 0.0;
		long lF = 0, hF = 0;
		if (fireSet.FilterToWorkingSet2(fpSet, 0, 0))//has fires
		{
			for (int y = 0; y < fireYears; y++)
			{
				//for each year...
				for (int i = 0; i < numPeriods; i++)
					//for (int i = 0; i < numPeriods && (!recCounter || !recCounter->Abort()); i++)
				{
					//see if there's fire days - look at each day
					//deal with leap years
					for (int d = DayOfYear(periods[varIDs[0]][i]->start);
						d <= DayOfYear(periods[varIDs[0]][i]->end);// && (! recCounter || !recCounter->Abort());
						d++)
					{
						lA = 0.0;
						hA = 0.0;
						lF = 0;
						hF = 0;
						while (!fireSet.IsEOF()
							&& fireSet.m_Discovery.GetYear() < periods[varIDs[0]][i]->baseYear + y)
							fireSet.MoveNext();
						while (!fireSet.IsEOF()
							&& fireSet.m_Discovery.GetYear() <= periods[varIDs[0]][i]->baseYear + y
							&& DayOfYear(fireSet.m_Discovery) < d)
							fireSet.MoveNext();
						if (!fireSet.IsEOF()
							&& fireSet.m_Discovery.GetYear() == periods[varIDs[0]][i]->baseYear + y
							&& DayOfYear(fireSet.m_Discovery) == d)
						{
							while (!fireSet.IsEOF()
								&& fireSet.m_Discovery.GetYear() == periods[varIDs[0]][i]->baseYear + y
								&& DayOfYear(fireSet.m_Discovery) == d)
							{
								switch (fireSet.m_StatisticalCause)
								{
								case 1://lightning
									lF++;
									lA = max(lA, fireSet.m_TotalAcres);
									break;
								default:
									int passesCauseFilter = 1;

									if (fireSet.m_AgencyID == 1) // USFS
										passesCauseFilter = (humanCause >> (fireSet.m_StatisticalCause - 1)) & 1;
									else
									{
										// convert to USFS cause type
										int usfs = fireSet.m_StatisticalCause;
										switch (fireSet.m_StatisticalCause)
										{
										case 2: usfs = 4; break;
										case 4: usfs = 5; break;
										case 5: usfs = 7; break;
										case 6: usfs = 2; break;
										case 7: usfs = 6; break;
										}
										passesCauseFilter = (humanCause >> (usfs - 1)) & 1;
									}


									if (passesCauseFilter)
									{
										hF++;
										hA = max(hA, fireSet.m_TotalAcres);
									}
								}
								fireSet.MoveNext();
								//if(recCounter)
								//	recCounter->Increment();
							}
						}
						fires[y][i].lightning = max(fires[y][i].lightning, lF);
						fires[y][i].human = max(fires[y][i].human, hF);
						fires[y][i].lAcresMax = max(fires[y][i].lAcresMax, lA);
						fires[y][i].hAcresMax = max(fires[y][i].hAcresMax, hA);
					}
				}
			}
		}
		fireSet.Close();
	}
	staSet.Close();
	return ret;

}

Period*** CClimAnalysis::GetWorkingSetPeriods(Period*** overallPeriods)
{
	Period*** wsPeriods = NULL;
	int sYear = fpSet->m_StartYear;
	int eYear = fpSet->m_EndYear;
	int nYears = eYear - sYear + 1;

	COleDateTime* startDates, * endDates;
	startDates = new COleDateTime[numPeriods];
	endDates = new COleDateTime[numPeriods];
	for (int p = 0; p < numPeriods; p++)
	{
		//startDates[p] = sDates[p];
		startDates[p].SetDate(fpSet->m_StartYear, sDates[p].GetMonth(), sDates[p].GetDay());
		endDates[p].SetDate(fpSet->m_EndYear, eDates[p].GetMonth(), eDates[p].GetDay());
	}
	wsPeriods = new Period **[MAXVARIDS];
	for (int v = 0; v < MAXVARIDS; v++)
		wsPeriods[v] = NULL;
	for(int v = 0; v < nVarIDs; v++)
	{
		wsPeriods[varIDs[v]] = new Period * [numPeriods];
		for (int i = 0; i < numPeriods; i++)
			wsPeriods[varIDs[v]][i] = new Period(startDates[i], endDates[i]);
	}
	for (int v = 0; v < MAXVARIDS; v++)
	{
		if (periods[v])
		{
			for (int p = 0; p < numPeriods; p++)
			{
				if (periods[v][p])
				{
					int yOffset = fpSet->m_StartYear - periods[v][p]->start.GetYear();
					for (int y = 0; y < wsPeriods[v][p]->years; y++)
						//for (int y = 0; y < periods[v][p]->years; y++)
					{
						COleDateTime dt(startDates[p].GetYear() + y, startDates[p].GetMonth(), startDates[p].GetDay(), 1,1,1);
						for (int q = 0; q < periods[v][p]->yStats[y + yOffset].N(); q++)
							wsPeriods[v][p]->Accumulate(dt, periods[v][p]->yStats[y + yOffset].Mean());
					}
					wsPeriods[v][p]->Complete();
				}
			}
		}
	}
	delete[] startDates;
	delete[] endDates;
	return wsPeriods;
}

CFireDay** CClimAnalysis::GetWorkingSetFires(CFireDay** overallFires, int* wsFireYears)
{
	*wsFireYears = 0;
	CFireDay** wsFires = NULL;
	if (fires == NULL)
		return NULL;
	int sYear = fpSet->m_StartYear;
	int eYear = fpSet->m_EndYear;
	int nYears = eYear - sYear + 1;
	wsFires = new CFireDay * [nYears];
	for (int y = 0; y < nYears; y++)
		wsFires[y] = new CFireDay[numPeriods];
	int yOffset = fpSet->m_StartYear - firesStartYear;
	for (int y = 0; y < nYears; y++)
	{
		for (int p = 0; p < numPeriods; p++)
		{
			wsFires[y][p].hAcresMax = fires[y + yOffset][p].hAcresMax;
			wsFires[y][p].human = fires[y + yOffset][p].human;
			wsFires[y][p].lAcresMax = fires[y + yOffset][p].lAcresMax;
			wsFires[y][p].lightning = fires[y + yOffset][p].lightning;
		}
	}
	*wsFireYears = nYears;
	return wsFires;
}

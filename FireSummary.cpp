// FireSummary.cpp: implementation of the CFireSummary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplusset.h"
#include "fireplus.h"
#include "FireSummary.h"
#include "FireAssocSet.h"
#include "CFiresSet.h"
#include "FireRegionSet.h"
#include "FireUnitSet.h"
#include "FireSubunitSet.h"
#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern int compare( const void *arg1, const void *arg2 );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void StripName(CString src, CString *dest)
{//code is included prior to name, strip name to dest
	char temp[64];
	strcpy_s(temp, src);
	int i;
	for(i = 0; i < (int)strlen(temp); i++)
	{
		if(temp[i] == ' ')
			break;
	}
	if(i == (int)strlen(temp))
		dest->Format("");
	else
		dest->Format("%s", &temp[i+ 1]);
}

CFireSummary::CFireSummary()
{
	startYear = nYears = nStrings = 0;
	acres = NULL;
	fires = NULL;
	int i;
	for(i = 0; i < 13; i++)
	{
		monthFires[i] = monthLightningFires[i] = 0;
	}
	for(i = 0; i <= NDAYVALS; i++)
		nPerDay[i] = 0;
	for(i = 0; i <= NSIZECLASSES; i++)
		sizeClasses[i] = 0;
	for(i = 0; i <= NCAUSECLASSES; i++)
		causeClasses[i] = 0;
	qStrings = NULL;
	m_pFpSet = NULL;
	m_pDB = NULL;

}

CFireSummary::~CFireSummary()
{
	if(acres)
		delete[] acres;
	if(fires)
		delete[] fires;
	if(qStrings)
		delete[] qStrings;
}

/*int AddFires returns 0 for success, -1 for user cancel, a positive number for error*/
int CFireSummary::AddFires(CDatabase *pDB, CFireplusSet *fpSet)
{//build date filter portion of query and call ProcessFires
	CString query;
	m_pFpSet = fpSet;
	m_pDB = pDB;
	fpSet->BuildBaseQuery(query, "Discovery", false);	
	return ProcessFires(pDB, fpSet->m_SIG_Station, query);
}

/*int AddFires returns 0 for success, -1 for user cancel, a positive number for error*/
int CFireSummary::AddFires(CDatabase *pDB, CString queryID, int start, int end)
{
	CString query;
	m_pDB = pDB;
	query.Format("(Year([Discovery]) >= %d and Year([Discovery]) <= %d)", start, end);	
	return ProcessFires(pDB, queryID, query);
}

/*int ProcessFires returns 0 for success, -1 for user cancel, a positive number for error*/
int CFireSummary::ProcessFires(CDatabase *pDB, CString queryID, CString baseQuery)
{	
	utils util;
	CWaitCursor wait;
	CString query, tQuery;
	long count = 0;
	int y, m, sc, cc;
	COleDateTime lastDay(1900, 1, 1, 0, 0, 0);
	int firesToday = 0;
	CFireAssocSet assocSet(pDB);
	char tStr[64];
	strcpy_s(tStr, queryID);
	assocSet.m_strFilter.Format("[SIG/Station] = '%s'", strlen(tStr) > 6 ? &tStr[6] : tStr);
	assocSet.Open();
	if(assocSet.IsEOF() || assocSet.IsBOF())
	{
		//CString msg = "WARNING: No fires filter specified.\n FireFamily Plus will not process fires\nwithout Fire Associations set.\n";
		//AfxMessageBox(msg, MB_OK);
		assocSet.Close();
		return 1;
	}
	int ret = 0;
	//AfxGetMainWnd()->EnableWindow(FALSE);
	//CCancelDialog cd;
	//cd.Create(IDD_CANCELDIALOG, this);
	//cd.SetStatusString("Preparing Fires Query...");
	//cd.SetTitle("Reading Fire Data");
	while(!assocSet.IsEOF())
	{
		nStrings++;
		assocSet.MoveNext();
	}
	CFireRegionSet regSet(pDB);
	CFireUnitSet unitSet(pDB);
	CFireSubunitSet subunitSet(pDB);
	regSet.Open();
	unitSet.Open();
	subunitSet.Open();
	qStrings = new CString[nStrings];
	assocSet.MoveFirst();
	//build the query for FireSet
	CString temp;
	while(!assocSet.IsEOF())// && !cd.Abort())
	{
		temp = "";
		if(!assocSet.IsFieldNull(&assocSet.m_SubunitID))
		{
			temp.Format("[SubunitID] = %ld", assocSet.m_SubunitID);
			subunitSet.m_strFilter.Format("[SubunitID] = %ld", assocSet.m_SubunitID);
			subunitSet.Requery();
			if(!subunitSet.IsEOF())
				//StripName(subunitSet.m_Name, &qStrings[count]);
				qStrings[count] = subunitSet.m_Name;
			else
				qStrings[count] = "";
		}
		else if(!assocSet.IsFieldNull(&assocSet.m_UnitID))
		{
			temp.Format("[UnitID] = %ld", assocSet.m_UnitID);
			unitSet.m_strFilter.Format("[UnitID] = %ld", assocSet.m_UnitID);
			unitSet.Requery();
			if(!unitSet.IsEOF())
				//StripName(unitSet.m_Name, &qStrings[count]);
				qStrings[count] = unitSet.m_Name;
			else
				qStrings[count] = "";
		}
		else if(!assocSet.IsFieldNull(&assocSet.m_RegionID))
		{
			temp.Format("[RegionID] = %ld", assocSet.m_RegionID);
			regSet.m_strFilter.Format("[RegionID] = %ld", assocSet.m_RegionID);
			regSet.Requery();
			if(!regSet.IsEOF())
				//StripName(regSet.m_Name, &qStrings[count]);
				qStrings[count] = regSet.m_Name;
			else
				qStrings[count] = "";
		}
		if(count > 0)
			tQuery += " or ";
		tQuery += temp;
		assocSet.MoveNext();
		count++;
	}
	assocSet.Close();
	regSet.Close();
	unitSet.Close();
	subunitSet.Close();
	CFiresSet fireSet(pDB);
	if(tQuery.GetLength() > 0)
		fireSet.m_strFilter.Format("(%s) and (%s)", tQuery, baseQuery);
	else
		fireSet.m_strFilter = baseQuery;
	//now build and intialize data 
	//fireSet.m_strFilter.Format("%s", query);
	fireSet.m_strSort.Format("%s", "[Discovery]");
	fireSet.Open();
	long totalFires = 0L;
	if(!fireSet.IsBOF() || !fireSet.IsEOF())
	{
		//has records...
		fireSet.MoveLast();
		nYears = fireSet.m_Discovery.GetYear();
		fireSet.MoveFirst();
		startYear = fireSet.m_Discovery.GetYear();
		nYears = nYears - startYear + 1;
		acres = new double [nYears + 1];
		fires = new long [nYears + 1];
		for(int i = 0; i < nYears + 1; i++)
		{
			acres[i] = 0.0;
			fires[i] = 0;
		}
		//now read data
		//cd.SetStatusString("Reading fires records");
		count = 0;
		while(!fireSet.IsEOF())// && !cd.Abort())
		{
			if(!fireSet.IsFieldNull(&fireSet.m_Discovery))
			{
				//if(count == 0)
				//	lastDay = fireSet.m_Discovery;
				if(DayOfYear(lastDay) != DayOfYear(fireSet.m_Discovery) ||
					lastDay.GetYear() != fireSet.m_Discovery.GetYear())
				{
					int loc = min(firesToday - 1, NDAYVALS - 1);
					if(loc >= 0)
					{
						nPerDay[loc]++;
						nPerDay[NDAYVALS]++;
					}
					lastDay = fireSet.m_Discovery;
					firesToday = 0;
				}
				firesToday++;
				y = fireSet.m_Discovery.GetYear() - startYear;
				m = fireSet.m_Discovery.GetMonth() - 1;
				if(!fireSet.IsFieldNull(&fireSet.m_TotalAcres))
				{
					acres[y] += fireSet.m_TotalAcres;
					acres[nYears] += fireSet.m_TotalAcres;
					if(fireSet.m_TotalAcres <= 0.25)
						sc = 0;
					else if(fireSet.m_TotalAcres < 10.0)
						sc = 1;
					else if(fireSet.m_TotalAcres < 100.0)
						sc = 2;
					else if(fireSet.m_TotalAcres < 300.0)
						sc = 3;
					else if(fireSet.m_TotalAcres < 1000.0)
						sc = 4;
					else if(fireSet.m_TotalAcres < 5000.0)
						sc = 5;
					else
						sc = 6;
				}
				else
					sc = 0;
				sizeClasses[sc]++;
				sizeClasses[NSIZECLASSES]++;
				fires[y]++;
				fires[nYears]++;
				monthFires[m]++;
				monthFires[12]++;
				if(!fireSet.IsFieldNull(&fireSet.m_StatisticalCause))
				{
					cc = util.TranslateCauseToUSFS(fireSet.m_StatisticalCause,fireSet.m_AgencyID) - 1;
					if(cc == 0)
						monthLightningFires[m]++;
					if(cc >= 0 && cc < NCAUSECLASSES)
					{
						causeClasses[cc]++;
						causeClasses[NCAUSECLASSES]++;
					}
				}
				/*if(!fireSet.IsFieldNull(&fireSet.m_SizeClass))
				{
					sc = fireSet.m_SizeClass[0] - 'A';
					if(sc >= 0 && sc <NSIZECLASSES)
					{
						sizeClasses[sc]++;
						sizeClasses[NSIZECLASSES]++;
					}
				}*/
				count++;
				totalFires++;
				fireSet.MoveNext();
				//cd.Increment();
			}
		}
		if(firesToday > 0)
		{
			int loc = min(firesToday - 1, NDAYVALS - 1);
			if(loc >= 0)
			{
				nPerDay[loc]++;
				nPerDay[NDAYVALS]++;
			}
		}
		//now, cycle back thru putting all the acres into an array and sort for percentiles
		double *fireAcres = new double[totalFires];
		int sLoc = 0;
		fireSet.MoveFirst();
		while(!fireSet.IsEOF())// && !cd.Abort())
		{
			if(!fireSet.IsFieldNull(&fireSet.m_Discovery))
			{
				if(!fireSet.IsFieldNull(&fireSet.m_TotalAcres))
					fireAcres[sLoc] = fireSet.m_TotalAcres;
				else
					fireAcres[sLoc] = 0.0;
				sLoc++;
			}
			fireSet.MoveNext();
		}
		//sort em
		qsort((void *)fireAcres, (size_t)totalFires, sizeof(double), compare);
		if(totalFires >= 100)
		{
			for(int p = 0; p < 100; p++)
			{
				int pcntLoc = ((double)totalFires) / 100.0 * (p + 1) - 1;
				sizePcnts[p] = fireAcres[pcntLoc];
			}
		}
		else
		{
			for(int p = 0; p < 100; p++)
			{
				sizePcnts[p] = 0.0;
			}
			for(int p = 99; p >= 99 - totalFires; p--)
			{
				int pcntLoc = totalFires - (99 - p) - 1;
				sizePcnts[p] = fireAcres[pcntLoc];
			}
		}
		delete[] fireAcres;
	}
	else
		ret = 2;
	//AfxGetMainWnd()->EnableWindow(TRUE);
	//if(cd.Abort())
		//return -1;
	return ret;
}


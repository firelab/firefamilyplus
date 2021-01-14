// ffpDescription.h : Implementation of the CFireplusSet class

 

// CFireplusSet implementation

// code generated on Wednesday, September 28, 2005, 1:14 PM

#include "stdafx.h"
#include "fireplus.h"
#include "fireplusSet.h"
#include "TermDatesDialog.h"
#include "WxSet.h"

IMPLEMENT_DYNAMIC(CFireplusSet, CRecordset)

CFireplusSet::CFireplusSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_Description = L"";
	m_SIG_Station = L"";
	m_StartYear = 0;
	m_EndYear = 0;
	m_StartMonth = L"";
	m_EndMonth = L"";
	m_StartDay = 0;
	m_EndDay = 0;
	m_PeriodLength = L"";
	m_Use78 = TRUE;
	m_Use88 = FALSE;
	m_UseCanadian = FALSE;
	m_nFields = 12;
	m_nDefaultType = dynaset;
	m_pTermDatesDialog = NULL;
}

CFireplusSet::~CFireplusSet()
{
	if(m_pTermDatesDialog)
	{
		m_pTermDatesDialog->DestroyWindow();
		m_pTermDatesDialog = NULL;
	}
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CFireplusSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CFireplusSet::GetDefaultSQL()
{
	return _T("[ffpDescription]");
}

void CFireplusSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[Description]"), m_Description);
	RFX_Text(pFX, _T("[SIG/Station]"), m_SIG_Station);
	RFX_Long (pFX, _T("[StartYear]"), m_StartYear);
	RFX_Long (pFX, _T("[EndYear]"), m_EndYear);
	RFX_Text(pFX, _T("[StartMonth]"), m_StartMonth);
	RFX_Text(pFX, _T("[EndMonth]"), m_EndMonth);
	RFX_Long (pFX, _T("[StartDay]"), m_StartDay);
	RFX_Long (pFX, _T("[EndDay]"), m_EndDay);
	RFX_Text(pFX, _T("[PeriodLength]"), m_PeriodLength);
	RFX_Bool(pFX, _T("[Use78]"), m_Use78);
	RFX_Bool(pFX, _T("[Use88]"), m_Use88);
	RFX_Bool(pFX, _T("[UseCanadian]"), m_UseCanadian);

}
/////////////////////////////////////////////////////////////////////////////
// CFireplusSet diagnostics

#ifdef _DEBUG
void CFireplusSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CFireplusSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

void CFireplusSet::BuildBaseQuery(CString& query, CString dateField, bool yearsOnly, bool inverted)
{
	CString temp;
	//DETERMINE DAY OF YEAR ORDERING
	int dOrder = DOY_NORMAL;
	COleDateTime d1, d2;
	d1.SetDate(BASISYEAR, MonthChtoInt(m_StartMonth), m_StartDay);
	d2.SetDate(BASISYEAR, MonthChtoInt(m_EndMonth), m_EndDay);
	COleDateTimeSpan day(1, 0, 0, 0 );
	if(d1 == d2)// || d2 + day == d1)
		dOrder = DOY_EQUAL;
	else if(d1 > d2)
		dOrder = DOY_FLIPPED;
	//add years to query
	if(!inverted)
		query.Format("(Year([%s]) >= %d and Year([%s]) <= %d)", 
			dateField, m_StartYear, dateField, m_EndYear);
	else
		query.Format("(Year([%s]) < %d or Year([%s]) > %d)", 
			dateField, m_StartYear, dateField, m_EndYear);
	if(!yearsOnly)
	{
		switch(dOrder)
		{
		//case DOY_EQUAL:
			//simplest case: All days of year.
			//add nothing to query
	//		break;
		case DOY_FLIPPED:
		{
			//END DOY EARLIER THAN START DOY
			temp.Format(" and ((Month([%s]) > %d or Month([%s]) < %d)", 
				dateField, d1.GetMonth(), dateField, d2.GetMonth());
			query += temp;
			if(d1.GetMonth() == d2.GetMonth())
			{
				temp.Format(" or (Month([%s]) = %d and Day([%s]) >= %d"
					" or Day([%s]) <= %d))", 
				dateField, d1.GetMonth(), dateField, m_StartDay, dateField, m_EndDay);
			}
			else
			{
				temp.Format( 
					" or (Month([%s]) = %d and Day([%s]) >= %d)" 
					" or (Month([%s]) = %d and Day([%s]) <= %d))", 
					dateField, d1.GetMonth(), dateField, m_StartDay,
					dateField, d2.GetMonth(), dateField, m_EndDay);
			}
			query += temp;
		}
			break;
		case DOY_EQUAL:
		default: //DOY NORMAL
			if(d1.GetMonth() == d2.GetMonth())
			{
				temp.Format(" and Month([%s]) = %d and Day([%s]) >= %d"
					" and Day([%s]) <= %d", 
				dateField, d1.GetMonth(), dateField, m_StartDay, dateField, m_EndDay);
			}
			else
			{
				temp.Format(" and ((Month([%s]) > %d and Month([%s]) < %d)", 
					dateField, d1.GetMonth(), dateField, d2.GetMonth());
				query += temp;
				temp.Format(
					" or (Month([%s]) = %d and Day([%s]) >= %d)" 
					" or (Month([%s]) = %d and Day([%s]) <= %d))", 
					dateField, d1.GetMonth(), dateField, m_StartDay,
					dateField, d2.GetMonth(), dateField, m_EndDay);
			}
			query += temp;
		}
	}
}

void CFireplusSet::BuildBaseNFDRS2016Query(CString& query, CString dateField, bool yearsOnly, bool inverted/* = false*/)
{
	CString temp;
	//DETERMINE DAY OF YEAR ORDERING
	int dOrder = DOY_NORMAL;
	COleDateTime d1, d2;
	d1.SetDate(BASISYEAR, MonthChtoInt(m_StartMonth), m_StartDay);
	d2.SetDate(BASISYEAR, MonthChtoInt(m_EndMonth), m_EndDay);
	COleDateTimeSpan day(1, 0, 0, 0);
	if (d1 == d2)// || d2 + day == d1)
		dOrder = DOY_EQUAL;
	else if (d1 > d2)
		dOrder = DOY_FLIPPED;
	//add years to query
	if (!inverted)
		query.Format("(Year(%s) >= %d and Year(%s) <= %d)",
			dateField, m_StartYear, dateField, m_EndYear);
	else
		query.Format("(Year(%s) < %d or Year(%s) > %d)",
			dateField, m_StartYear, dateField, m_EndYear);
	if (!yearsOnly)
	{
		switch (dOrder)
		{
			//case DOY_EQUAL:
			//simplest case: All days of year.
			//add nothing to query
			//		break;
		case DOY_FLIPPED:
		{
			//END DOY EARLIER THAN START DOY
			temp.Format(" and ((Month(%s) > %d or Month(%s) < %d)",
				dateField, d1.GetMonth(), dateField, d2.GetMonth());
			query += temp;
			if (d1.GetMonth() == d2.GetMonth())
			{
				temp.Format(" or (Month(%s) = %d and Day(%s) >= %d"
					" or Day(%s) <= %d))",
					dateField, d1.GetMonth(), dateField, m_StartDay, dateField, m_EndDay);
			}
			else
			{
				temp.Format(
					" or (Month(%s) = %d and Day(%s) >= %d)"
					" or (Month(%s) = %d and Day(%s) <= %d))",
					dateField, d1.GetMonth(), dateField, m_StartDay,
					dateField, d2.GetMonth(), dateField, m_EndDay);
			}
			query += temp;
		}
		break;
		case DOY_EQUAL:
		default: //DOY NORMAL
			if (d1.GetMonth() == d2.GetMonth())
			{
				temp.Format(" and Month(%s) = %d and Day(%s) >= %d"
					" and Day(%s) <= %d",
					dateField, d1.GetMonth(), dateField, m_StartDay, dateField, m_EndDay);
			}
			else
			{
				temp.Format(" and ((Month(%s) > %d and Month(%s) < %d)",
					dateField, d1.GetMonth(), dateField, d2.GetMonth());
				query += temp;
				temp.Format(
					" or (Month(%s) = %d and Day(%s) >= %d)"
					" or (Month(%s) = %d and Day(%s) <= %d))",
					dateField, d1.GetMonth(), dateField, m_StartDay,
					dateField, d2.GetMonth(), dateField, m_EndDay);
			}
			query += temp;
		}
	}
	query += " AND WXObs.[StationID] = ffpNFDRS2016DFM.[StationID] AND WXObs.[ObsDate] = ffpNFDRS2016DFM.[ObsDate]";
}

bool CFireplusSet::GetNFDRS2016Range(CString stnID, COleDateTime *start, COleDateTime *end)
{
	start->SetStatus(COleDateTime::invalid);
	end->SetStatus(COleDateTime::invalid);
	//necessary fields: ObsDate, Temp, RH, TmpMax, TmpMin, RHMin, HourlyPrecip, SolarRadiation, WS, PPTAMT
	CWxSet wxSet(this->m_pDatabase);
	wxSet.m_strFilter.Format("[StationID] =  '%6.6s' AND [DailyObs] = 0 AND NOT([SolarRadiation]) IS NULL AND NOT([ObsDate]) IS NULL AND NOT([Temp]) IS NULL "
	"AND NOT([RH]) IS NULL AND NOT([PPTAMT]) IS NULL AND NOT([WS]) IS NULL AND NOT([TmpMax]) IS NULL AND NOT([TmpMin]) IS NULL "
	"AND NOT([RHMin]) IS NULL AND NOT([HourlyPrecip]) IS NULL", stnID);
	wxSet.m_strSort = "[ObsDate]";
	wxSet.Open();
	if (!wxSet.IsBOF() && !wxSet.IsEOF())
	{
		*start = wxSet.m_ObsDate;
		wxSet.MoveLast();
		*end = wxSet.m_ObsDate;
	}
	wxSet.Close();
	return !(start->m_status || end->m_status);
}

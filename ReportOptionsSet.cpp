// ReportOptionsSet.h : Implementation of the CReportOptionsSet class

 

// CReportOptionsSet implementation

// code generated on Wednesday, September 28, 2005, 2:09 PM

#include "stdafx.h"
#include "fireplus.h"
#include "fireplusset.h"
#include "ReportOptionsSet.h"
#include "SIGStationSet.h"
#include "StationInSIGSet.h"
#include "SowOptionsSet.h"

IMPLEMENT_DYNAMIC(CReportOptionsSet, CRecordset)

CReportOptionsSet::CReportOptionsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_WorkingSet = FALSE;
	m_AnnualFilter = FALSE;
	m_SIGStation = FALSE;
	m_Addtl = FALSE;
	m_Delimiter = 0;
	m_nFields = 5;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CReportOptionsSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CReportOptionsSet::GetDefaultSQL()
{
	return _T("[ffpReportOptions]");
}

// listAll is a "wrapper" function - does not add "Addtl" options

void CReportOptionsSet::listAll(CString *stream, CFireplusSet *fpSet){
	if (! stream)
			return;
	if (! fpSet)
			return;

	if (m_WorkingSet){
		CString wsStr;
		listWorkingSet(&wsStr, fpSet);
		stream->Append(wsStr);
		
	}
	if (m_AnnualFilter){
		CString afStr;
		listAnnualFilter(&afStr, fpSet);
		stream->Append(afStr);
		
	}
	

	if (m_SIGStation){
		CString staStr;
		listSIGStation(&staStr, fpSet->m_SIG_Station,fpSet);
		stream->Append(staStr);
		
	}
	stream->Append("\n");

}

void CReportOptionsSet::listWorkingSet(CString *stream, CFireplusSet *fpSet){
	if (! stream)
		return;

	if (!fpSet)
		return;
	
	CString str;

	str.Format("------------------------------------------------------\nActive Working Set:\n"); 
	stream->Append(str);

	if (strlen(fpSet->m_SIG_Station) > 6)
		
			str.Format("   SIG:     %-20.20s\n", fpSet->m_SIG_Station);
	else {
			CString staQuery;
			CSIGStationSet staSet(fpSet->m_pDatabase);

			staQuery.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);

			staSet.m_strFilter = _T(staQuery);

			staSet.Open();
			str.Format("   Station: %s  - %-20.20s\n", fpSet->m_SIG_Station, staSet.m_Name);
			staSet.Close();

	}
	stream->Append(str);

	str.Format("   Data years:  %d - %d\n", fpSet->m_StartYear, fpSet->m_EndYear);
	stream->Append(str);

	str.Format("   Analysis Period Length: %s days\n", fpSet->m_PeriodLength);
	stream->Append(str);

}


void CReportOptionsSet::listAnnualFilter(CString *stream, CFireplusSet *fpSet){
	if (! stream)
		return;

	if (!fpSet)
		return;
	
	CString str;
	str.Format("   Annual filter dates:  %s %d  thru  %s %d\n", fpSet->m_StartMonth, fpSet->m_StartDay, fpSet->m_EndMonth, fpSet->m_EndDay);
	stream->Append(str);

}

void	CReportOptionsSet::listSIGStation(CString *stream, CString sig, CFireplusSet *fpSet)
{	if (! sig)
		return;
	if (! stream)
		return;

	if (! fpSet)
		return;

	CString str;

	CSIGStationSet staSet(fpSet->m_pDatabase);
	
	

	if (sig.GetLength() > 6){   // SIG
		char sig[64];
		strcpy(sig, fpSet->m_SIG_Station);
		CString temp;
		CSowOptionsSet sowOptsSet(fpSet->m_pDatabase);
		sowOptsSet.Open();
		CStationInSIGSet sSet(fpSet->m_pDatabase);
		temp.Format("[SIG] = '%-20.20s'", &sig[6]);
		sSet.m_strFilter = _T(temp);
		sSet.Open();
		while(!sSet.IsEOF()){
			CString staQuery;
			staQuery.Format("[StationID] = '%6.6s'", sSet.m_StationID);
			staSet.m_strFilter = _T(staQuery);
			staSet.Open();
			//sowOptsSet.m_strFilter = _T(staQuery);
			sowOptsSet.m_strFilter.Format("[SIG_Station] = '%6.6s'", sSet.m_StationID);
			sowOptsSet.Requery();
			str.Format("------------------------------------------------------\nStation Details:\n"); 
			stream->Append(str);

			str.Format("   %s %-20.20s   Fuel model: %s   (Use 88?: %s)\n\n", staSet.m_StationID, staSet.m_Name, staSet.m_NFDRSFM, staSet.m_Use88 ? "Y" : "N");
			stream->Append(str);
			CString gStr = staSet.m_GreenJulian.Format("%m/%d");
			CString fStr = staSet.m_FreezeJulian.Format("%m/%d");
			str.Format("   Slope class: %d   Climate class: %d   Greenup: %s   Freeze: %s\n", staSet.m_SlopeCls, staSet.m_ClimateCls, gStr,fStr);
			stream->Append(str);

			str.Format("   Start KBDI: %3d  Start FM1000:  %.0f  Avg. Precip: %.2f\n", staSet.m_StartKBDI, staSet.m_Start1000, staSet.m_AvgPrecip);
			stream->Append(str);

			str.Format("   FM1 = FM10?  %s   Herb Annual?   %s   Deciduous?  %s\n", staSet.m_FM1Eq10 ? "Y" : "N", staSet.m_HerbAnnual? "Y" : "N", staSet.m_Deciduous? "Y" : "N"); 
			stream->Append(str);

			str.Format("   Aspect:      %s   Slope posit.:  %s   Elevation: %4d\n", staSet.m_Aspect, staSet.m_PsnOnSlope, staSet.m_Elevation);
			stream->Append(str);

			str.Format("   Latitude:   %3.2f   ", staSet.m_LatDegrees);
			stream->Append(str);
			if(!staSet.IsFieldNull(&staSet.m_LonDegrees))
				str.Format("Longitude: %3.2f\n", staSet.m_LonDegrees);
			else
				str = "Longitude: NA\n";
			stream->Append(str);
		
			if(!staSet.IsFieldNull(&staSet.m_UseStick) && !staSet.m_UseStick)
				str.Format("   Weighed Stick Moistures Used:   No\n");
			else
				str.Format("   Weighed Stick Moistures Used:   Yes\n");
			stream->Append(str);

			if(!staSet.IsFieldNull(&staSet.m_UseDormant) && !staSet.IsFieldNull(&staSet.m_DormantJulian))
			{
				if(staSet.m_UseDormant)
				{
					if(staSet.m_DormantJulian.GetStatus() == COleDateTime::valid)
					{
						CString dStr = staSet.m_DormantJulian.Format("%m/%d");
						str.Format("   Dormancy Date Used: %s\n", dStr);
						stream->Append(str);
					}
				}
			}
			if(!sowOptsSet.IsEOF())
			{
				switch(sowOptsSet.m_SOW_Usage)
				{
				case CSowOptionsSet::SR_SOW_ONLY:
					str.Format("   SOW:       Never Use SR_SOW\n");
					break;
				case CSowOptionsSet::SOW_MISSING:
					str.Format("   SOW:       Use SR_SOW if SOW is Missing\n");
					break;
				case CSowOptionsSet::SR_SOW_NEVER:
				default:
					str.Format("   SOW:       Use Only SR_SOW\n");
					break;
				}
				stream->Append(str);
				switch(sowOptsSet.m_WetFlag_Usage)
				{
				case CSowOptionsSet::EST_WF_ONLY:
					str.Format("   WetFlag:   Never Use SR_WetFlag\n");
					break;
				case CSowOptionsSet::WF_MISSING:
					str.Format("   WetFlag:   Use SR_WetFlag if WetFlag is Missing\n");
					break;
				case CSowOptionsSet::EST_WF_NEVER:
				default:
					str.Format("   WetFlag:   Use Only SR_WetFlag\n");
					break;
				}
				stream->Append(str);
			}
			else
			{
				str.Format("   SOW:       Never Use SR_SOW\n");
				stream->Append(str);
				str.Format("   WetFlag:   Never Use SR_WetFlag\n");
				stream->Append(str);
			}

			staSet.Close();
			sSet.MoveNext();
		}
		sowOptsSet.Close();
		sSet.Close();
		str.Format("------------------------------------------------------\n"); 
		stream->Append(str);
		
	
	} else { // single station
		CString staQuery;
		staQuery.Format("[StationID] = '%6.6s'", sig);
		staSet.m_strFilter = _T(staQuery);
		staSet.Open();
		CSowOptionsSet sowOptsSet(fpSet->m_pDatabase);
		//sowOptsSet.m_strFilter = _T(staQuery);
		sowOptsSet.m_strFilter.Format("[SIG_Station] = '%6.6s'", sig);
		sowOptsSet.Open();
		
		str.Format("------------------------------------------------------\nStation Details:\n"); 
		stream->Append(str);

		str.Format("   %s %-20.20s   Fuel model: %s   (Use 88?: %s)\n\n", staSet.m_StationID, staSet.m_Name, staSet.m_NFDRSFM, staSet.m_Use88 ? "Y" : "N");
		stream->Append(str);
		CString gStr = staSet.m_GreenJulian.Format("%m/%d");
		CString fStr = staSet.m_FreezeJulian.Format("%m/%d");
		str.Format("   Slope class: %d   Climate class: %d   Greenup: %s   Freeze: %s\n", staSet.m_SlopeCls, staSet.m_ClimateCls, gStr,fStr);
		stream->Append(str);

		str.Format("   Start KBDI: %3d  Start FM1000:%.0f   Avg. Precip: %.2f\n", staSet.m_StartKBDI, staSet.m_Start1000, staSet.m_AvgPrecip);
		stream->Append(str);

		str.Format("   FM1 = FM10?  %s   Herb Annual?   %s   Deciduous?  %s\n", staSet.m_FM1Eq10 ? "Y" : "N", staSet.m_HerbAnnual? "Y" : "N", staSet.m_Deciduous? "Y" : "N"); 
		stream->Append(str);

		str.Format("   Aspect:      %s   Slope posit.:  %s   Elevation: %4d\n", staSet.m_Aspect, staSet.m_PsnOnSlope, staSet.m_Elevation);
		stream->Append(str);

		//str.Format("   Latitude:   %3.2f   Longitude: %3.2f\n", staSet.m_LatDegrees, staSet.m_LonDegrees);
		str.Format("   Latitude:   %3.2f   ", staSet.m_LatDegrees);
		stream->Append(str);
		if(!staSet.IsFieldNull(&staSet.m_LonDegrees))
			str.Format("Longitude: %3.2f\n", staSet.m_LonDegrees);
		else
			str = "Longitude: NA\n";
		stream->Append(str);
	
		if(!staSet.IsFieldNull(&staSet.m_UseStick) && !staSet.m_UseStick)
			str.Format("   Weighed Stick Moistures Used:   No\n");
		else
			str.Format("   Weighed Stick Moistures Used:   Yes\n");
		stream->Append(str);

		if(!staSet.IsFieldNull(&staSet.m_UseDormant) && !staSet.IsFieldNull(&staSet.m_DormantJulian))
		{
			if(staSet.m_UseDormant)
			{
				if(staSet.m_DormantJulian.GetStatus() == COleDateTime::valid)
				{
					CString dStr = staSet.m_DormantJulian.Format("%m/%d");
					str.Format("   Dormancy Date Used: %s\n", dStr);
					stream->Append(str);
				}
			}
		}
			if(!sowOptsSet.IsEOF())
			{
				switch(sowOptsSet.m_SOW_Usage)
				{
				case CSowOptionsSet::SR_SOW_ONLY:
					str.Format("   SOW:       Use Only SR_SOW\n");
					break;
				case CSowOptionsSet::SOW_MISSING:
					str.Format("   SOW:       Use SR_SOW if SOW is Missing\n");
					break;
				case CSowOptionsSet::SR_SOW_NEVER:
				default:
					str.Format("   SOW:       Never Use SR_SOW\n");
					break;
				}
				stream->Append(str);
				switch(sowOptsSet.m_WetFlag_Usage)
				{
				case CSowOptionsSet::EST_WF_ONLY:
					str.Format("   WetFlag:   Use Only SR_WetFlag\n");
					break;
				case CSowOptionsSet::WF_MISSING:
					str.Format("   WetFlag:   Use SR_WetFlag if WetFlag is Missing\n");
					break;
				case CSowOptionsSet::EST_WF_NEVER:
				default:
					str.Format("   WetFlag:   Never Use SR_WetFlag\n");
					break;
				}
				stream->Append(str);
			}
			else
			{
				str.Format("   SOW:       Never Use SR_SOW\n");
				stream->Append(str);
				str.Format("   WetFlag:   Never Use SR_WetFlag\n");
				stream->Append(str);
			}

		str.Format("------------------------------------------------------\n"); 
		stream->Append(str);
		
		staSet.Close();
		sowOptsSet.Close();
	}
	

}

void CReportOptionsSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Bool(pFX, _T("[WorkingSet]"), m_WorkingSet);
	RFX_Bool(pFX, _T("[AnnualFilter]"), m_AnnualFilter);
	RFX_Bool(pFX, _T("[SIGStation]"), m_SIGStation);
	RFX_Bool(pFX, _T("[Addtl]"), m_Addtl);
	RFX_Long (pFX, _T("[Delimiter]"), m_Delimiter);

}

/////////////////////////////////////////////////////////////////////////////
// CReportOptionsSet diagnostics

#ifdef _DEBUG
void CReportOptionsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CReportOptionsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



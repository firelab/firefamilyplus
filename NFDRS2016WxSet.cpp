#include "stdafx.h"
#include "NFDRS2016WxSet.h"


IMPLEMENT_DYNAMIC(CNFDRS2016WxSet, CRecordset)

CNFDRS2016WxSet::CNFDRS2016WxSet(CDatabase* pDatabase)
	: CRecordset(pDatabase)
{
	m_StationID = L"";
	m_ObsDate;
	m_Temp = -999;
	m_RH = -999;
	m_WDir = -999;	//Wind Direction
	m_WS = -999;	//Wind Speed
	m_TmpMax = -999;	//Max Temperature
	m_TmpMin = -999;	//Min Temperature
	m_RHMax = -999;	//Max Relative Humidity
	m_RHMin = -999;	//Min Relative Humidity
	m_PPTAMT = -999;	//Precipitation Amount
	m_PPTDUR = -999;	//Precipitation Duration
	m_GustDir = -999;         // wind gust direction (degrees)
	m_GustSpd = -999;            // wind gust speed (mph)
	m_HourlyPrecip = -999;     // actual hourly precip values (inches)
	m_SolarRadiation = -999; // watts/m2
	m_WetFlag = 0;         // Y=1/N=0
	m_SnowFlag = 0;
	m_WAzimuth = -999;

//ffpNFDRS2016DFM fields
	m_FM1 = -999;
	m_FM10 = -999;
	m_FM100 = -999;
	m_FM1000 = -999;
	m_FuelTemperature = -999;

	m_nFields = 24;
	m_nDefaultType = dynaset;
}


CString CNFDRS2016WxSet::BuildStationFilter(CString stationID)
{
	CString ret;
	ret.Format("[DailyObs] = 1 AND WXObs.[StationID] = ffpNFDRS2016DFM.[StationID] AND WXObs.[ObsDate] = ffpNFDRS2016DFM.[ObsDate] AND WXObs.[StationID] = '%6.6s'", stationID);
	m_strFilter = ret;
	m_strSort = "WXObs.[ObsDate]";
	return ret;
}

CString CNFDRS2016WxSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CNFDRS2016WxSet::GetDefaultSQL()
{
	return _T("WXObs, ffpNFDRS2016DFM");
}

void CNFDRS2016WxSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	// Macros such as RFX_Text() and RFX_Long () are dependent on the
	// type of the member variable, not the type of the field in the database.
	// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("WXObs.[StationID]"), m_StationID);
	RFX_Date(pFX, _T("WXObs.[ObsDate]"), m_ObsDate);

	RFX_Long(pFX, _T("[Temp]"), m_Temp);
	RFX_Long(pFX, _T("[RH]"), m_RH);
	RFX_Long(pFX, _T("[WDir]"), m_WDir);
	RFX_Long(pFX, _T("[WS]"), m_WS);
	RFX_Long(pFX, _T("[TmpMax]"), m_TmpMax);
	RFX_Long(pFX, _T("[TmpMin]"), m_TmpMin);
	RFX_Long(pFX, _T("[RHMax]"), m_RHMax);
	RFX_Long(pFX, _T("[RHMin]"), m_RHMin);
	RFX_Single(pFX, _T("[PPTAMT]"), m_PPTAMT);

	RFX_Long(pFX, _T("[PPTDUR]"), m_PPTDUR);
	RFX_Long(pFX, _T("[SolarRadiation]"), m_SolarRadiation);
	RFX_Long(pFX, _T("[WetFlag]"), m_WetFlag);
	RFX_Long(pFX, _T("[SnowFlag]"), m_SnowFlag);
	RFX_Long(pFX, _T("[GustDir]"), m_GustDir);
	RFX_Long(pFX, _T("[GustSpd]"), m_GustSpd);
	RFX_Single(pFX, _T("[HourlyPrecip]"), m_HourlyPrecip);
	RFX_Long(pFX, _T("[CORCT2]"), m_WAzimuth);


	RFX_Double(pFX, _T("[FM_1]"), m_FM1);
	RFX_Double(pFX, _T("[FM_10]"), m_FM10);
	RFX_Double(pFX, _T("[FM_100]"), m_FM100);
	RFX_Double(pFX, _T("[FM_1000]"), m_FM1000);
	RFX_Double(pFX, _T("[FuelTemperature]"), m_FuelTemperature);

}
/////////////////////////////////////////////////////////////////////////////
// CWxSet diagnostics

#ifdef _DEBUG
void CNFDRS2016WxSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CNFDRS2016WxSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


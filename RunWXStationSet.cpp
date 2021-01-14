// RunWxStationSet.h : Implementation of the CRunWxStationSet class

 

// CRunWxStationSet implementation

// code generated on Wednesday, September 28, 2005, 2:18 PM

#include "stdafx.h"
#include "RunWxStationSet.h"
IMPLEMENT_DYNAMIC(CRunWxStationSet, CRecordset)

CRunWxStationSet::CRunWxStationSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_runID = 0;
	m_StationID = L"";
	m_Name = L"";
	m_NFDRSFM = L"";
	m_Use88 = 0;
	m_LatDegrees = 0;
	m_LonDegrees = 0;
	m_Elevation = 0;
	m_ClimateCls = 0;
	m_SlopeCls = 0;
	m_HerbAnnual = 0;
	m_GreenJulian;
	m_FreezeJulian;
	m_StartGreenHerb = 0;
	m_StartGreenShrub = 0;
	m_StartKBDI = 0;
	m_Deciduous = 0;
	m_AvgPrecip = 0.0;
	m_Start1000 = 0.0;
	m_FM1Eq10 = 0;
	m_Aspect = L"";
	m_PsnOnSlope = L"";
	m_StnType = 0;
	m_Site = 0;
	m_State = L"";
	m_County = 0;
	m_ObsAgy = 0;
	m_ObsUnit = L"";
	m_FSRegion = 0;
	m_WRCC_ID = L"";
	m_nFields = 30;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CRunWxStationSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CRunWxStationSet::GetDefaultSQL()
{
	return _T("[ffpRunWxStation]");
}

void CRunWxStationSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[runID]"), m_runID);
	RFX_Text(pFX, _T("[StationID]"), m_StationID);
	RFX_Text(pFX, _T("[Name]"), m_Name);
	RFX_Text(pFX, _T("[NFDRSFM]"), m_NFDRSFM);
	RFX_Long (pFX, _T("[Use88]"), m_Use88);
	RFX_Double(pFX, _T("[LatDegrees]"), m_LatDegrees);
	RFX_Double(pFX, _T("[LongDegrees]"), m_LonDegrees);
	RFX_Long (pFX, _T("[Elevation]"), m_Elevation);
	RFX_Long (pFX, _T("[ClimateCls]"), m_ClimateCls);
	RFX_Long (pFX, _T("[SlopeCls]"), m_SlopeCls);
	RFX_Long (pFX, _T("[HerbAnnual]"), m_HerbAnnual);
	RFX_Date(pFX, _T("[GreenJulian]"), m_GreenJulian);
	RFX_Date(pFX, _T("[FreezeJulian]"), m_FreezeJulian);
	RFX_Long (pFX, _T("[StartGreenHerb]"), m_StartGreenHerb);
	RFX_Long (pFX, _T("[StartGreenShrub]"), m_StartGreenShrub);
	RFX_Long (pFX, _T("[StartKBDI]"), m_StartKBDI);
	RFX_Long (pFX, _T("[Deciduous]"), m_Deciduous);
	RFX_Single(pFX, _T("[AvgPrecip]"), m_AvgPrecip);
	RFX_Single(pFX, _T("[Start1000]"), m_Start1000);
	RFX_Long (pFX, _T("[FM1Eq10]"), m_FM1Eq10);
	RFX_Text(pFX, _T("[Aspect]"), m_Aspect);
	RFX_Text(pFX, _T("[PsnOnSlope]"), m_PsnOnSlope);
	RFX_Long(pFX, _T("[StnType]"), m_StnType);
	RFX_Long(pFX, _T("[Site]"), m_Site);
	RFX_Text(pFX, _T("[State]"), m_State);
	RFX_Long(pFX, _T("[County]"), m_County);
	RFX_Long(pFX, _T("[ObsAgy]"), m_ObsAgy);
	RFX_Text(pFX, _T("[ObsUnit]"), m_ObsUnit);
	RFX_Long(pFX, _T("[FSRegion]"), m_FSRegion);
	RFX_Text(pFX, _T("[WRCC_ID]"), m_WRCC_ID);

}
/////////////////////////////////////////////////////////////////////////////
// CRunWxStationSet diagnostics

#ifdef _DEBUG
void CRunWxStationSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CRunWxStationSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



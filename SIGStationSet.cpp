// SIGStationSet.h : Implementation of the CSIGStationSet class

 

// CSIGStationSet implementation

// code generated on Wednesday, September 28, 2005, 2:21 PM

#include "stdafx.h"
#include "SIGStationSet.h"
IMPLEMENT_DYNAMIC(CSIGStationSet, CRecordset)

CSIGStationSet::CSIGStationSet(CDatabase* pdb)
	: CRecordset(pdb)
{
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
	m_UseDormant = 0;
	m_UseStick = 1;
	m_DormantJulian;
	m_RegSchdObs = 13;
	m_NESDIS_ID = "";

	m_SCM_V = 108;
	m_SCM_W = 62;
	m_SCM_X = 104;
	m_SCM_Y = 5;
	m_SCM_Z = 19;

	m_MX_Humid_V = FALSE;
	m_MX_Humid_W = FALSE;
	m_MX_Humid_X = FALSE;
	m_MX_Humid_Y = FALSE;
	m_MX_Humid_Z = FALSE;

	m_timeZoneOffset = 0;
	m_KBDIThreshold = 100;
	m_nFields = 46;
	m_nDefaultType = dynaset;

}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CSIGStationSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CSIGStationSet::GetDefaultSQL()
{
	return _T("[WxStation]");
}

void CSIGStationSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[StationID]"), m_StationID);
	RFX_Text(pFX, _T("[Name]"), m_Name);
	RFX_Text(pFX, _T("[NFDRSFM]"), m_NFDRSFM);
	RFX_Long (pFX, _T("[Use88]"), m_Use88);
	RFX_Double(pFX, _T("[LatDegrees]"), m_LatDegrees);
	RFX_Double(pFX, _T("[LonDegrees]"), m_LonDegrees);
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
	RFX_Long (pFX, _T("[UseStick]"), m_UseStick);
	RFX_Long (pFX, _T("[UseDormant]"), m_UseDormant);
	RFX_Date(pFX, _T("[DormantJulian]"), m_DormantJulian);
	RFX_Long (pFX, _T("[RegSchedObs]"), m_RegSchdObs);
    RFX_Text(pFX, _T("[NESDIS_ID]"), m_NESDIS_ID);
	//NFDR2016 additions
	RFX_Long(pFX, _T("[SCM_V]"), m_SCM_V);
	RFX_Long(pFX, _T("[SCM_W]"), m_SCM_W);
	RFX_Long(pFX, _T("[SCM_X]"), m_SCM_X);
	RFX_Long(pFX, _T("[SCM_Y]"), m_SCM_Y);
	RFX_Long(pFX, _T("[SCM_Z]"), m_SCM_Z);

	RFX_Bool(pFX, _T("[MX_Humid_V]"), m_MX_Humid_V);
	RFX_Bool(pFX, _T("[MX_Humid_W]"), m_MX_Humid_W);
	RFX_Bool(pFX, _T("[MX_Humid_X]"), m_MX_Humid_X);
	RFX_Bool(pFX, _T("[MX_Humid_Y]"), m_MX_Humid_Y);
	RFX_Bool(pFX, _T("[MX_Humid_Z]"), m_MX_Humid_Z);
	RFX_Int(pFX, _T("TimeZoneOffset"), m_timeZoneOffset);
	RFX_Int(pFX, _T("KBDIThreshold"), m_KBDIThreshold);
}


BOOL CSIGStationSet::GetMxHumid(char fm)
{
	switch (fm)
	{
	case 'v':
	case 'V':
		return m_MX_Humid_V;
	case 'w':
	case 'W':
		return m_MX_Humid_W;
	case 'x':
	case 'X':
		return m_MX_Humid_X;
	case 'y':
	case 'Y':
		return m_MX_Humid_Y;
	case 'z':
	case 'Z':
		return m_MX_Humid_Z;
	}
	return FALSE;
}
long CSIGStationSet::GetSCM(char fm)
{
	switch (fm)
	{
	case 'v':
	case 'V':
		return m_SCM_V;
	case 'w':
	case 'W':
		return m_SCM_W;
	case 'x':
	case 'X':
		return m_SCM_X;
	case 'y':
	case 'Y':
		return m_SCM_Y;
	case 'z':
	case 'Z':
		return m_SCM_Z;
	}
	return 30;
}

/////////////////////////////////////////////////////////////////////////////
// CSIGStationSet diagnostics

#ifdef _DEBUG
void CSIGStationSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CSIGStationSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



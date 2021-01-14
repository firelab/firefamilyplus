// RunPocketCardSet.cpp : Implementation of the CRunPocketCardSet class
 


// CRunPocketCardSet implementation

// code generated on Wednesday, September 28, 2005, 2:08 PM

#include "stdafx.h"
#include "RunPocketCardSet.h"
IMPLEMENT_DYNAMIC(CRunPocketCardSet, CRecordset)

CRunPocketCardSet::CRunPocketCardSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_runID = 0;
	m_varID = 0;
	m_FireDangerArea = L"";
	m_AreaBitmap = L"";
	m_Index = L"";
	m_FuelModelName = L"";
	m_Year1 = 0;
	m_Year2 = 0;
	m_Fire1 = L"";
	m_Fire2 = L"";
	m_Fire3 = L"";
	m_Fire1Value = 0.0;
	m_Fire2Value = 0.0;
	m_Fire3Value = 0.0;
	m_Fire1Date;
	m_Fire2Date;
	m_Fire3Date;
	m_Line1 = L"";
	m_Line2 = L"";
	m_Line3 = L"";
	m_WindSpeed = 0;
	m_RH = 0;
	m_Temp = 0;
	m_LiveFM = 0;
	m_PastExperience = "";
	//m_StationID = L"";
	m_SIG_Station = L"";
	m_pcRyVal = 0;
	m_pcGyVal = 0;
	m_pcYVal = 0;
	m_ExtraVar = L"";
	m_CP = 0;
	m_NWCGStd = FALSE;
	m_RespAgency = L"";
	m_nFields = 33;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CRunPocketCardSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CRunPocketCardSet::GetDefaultSQL()
{
	return _T("[ffpRunPocketCard]");
}

void CRunPocketCardSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[runID]"), m_runID);
	RFX_Long (pFX, _T("[varID]"), m_varID);
	RFX_Text(pFX, _T("[FireDangerArea]"), m_FireDangerArea);
	RFX_Text(pFX, _T("[AreaBitmap]"), m_AreaBitmap);
	RFX_Text(pFX, _T("[Index]"), m_Index);
	RFX_Text(pFX, _T("[FuelModelName]"), m_FuelModelName);
	RFX_Long (pFX, _T("[Year1]"), m_Year1);
	RFX_Long (pFX, _T("[Year2]"), m_Year2);
	RFX_Text(pFX, _T("[Fire1]"), m_Fire1);
	RFX_Text(pFX, _T("[Fire2]"), m_Fire2);
	RFX_Text(pFX, _T("[Fire3]"), m_Fire3);
	RFX_Double(pFX, _T("[Fire1Value]"), m_Fire1Value);
	RFX_Double(pFX, _T("[Fire2Value]"), m_Fire2Value);
	RFX_Double(pFX, _T("[Fire3Value]"), m_Fire3Value);
	RFX_Date(pFX, _T("[Fire1Date]"), m_Fire1Date);
	RFX_Date(pFX, _T("[Fire2Date]"), m_Fire2Date);
	RFX_Date(pFX, _T("[Fire3Date]"), m_Fire3Date);
	RFX_Text(pFX, _T("[Line1]"), m_Line1);
	RFX_Text(pFX, _T("[Line2]"), m_Line2);
	RFX_Text(pFX, _T("[Line3]"), m_Line3);
	RFX_Long (pFX, _T("[WindSpeed]"), m_WindSpeed);
	RFX_Long (pFX, _T("[RH]"), m_RH);
	RFX_Long (pFX, _T("[Temp]"), m_Temp);
	RFX_Long (pFX, _T("[LiveFM]"), m_LiveFM);
	RFX_Text(pFX, _T("[PastExperience]"), m_PastExperience, 6400, SQL_VARCHAR );
	RFX_Text(pFX, _T("[SIG/Station]"), m_SIG_Station);
	RFX_Long (pFX, _T("[pcRyVal]"), m_pcRyVal);
	RFX_Long (pFX, _T("[pcGyVal]"), m_pcGyVal);
	RFX_Long (pFX, _T("[pcYVal]"), m_pcYVal);
	RFX_Text(pFX, _T("[ExtraVar]"), m_ExtraVar);
	RFX_Long (pFX, _T("[CP]"), m_CP);
	RFX_Bool(pFX, _T("[NWCGStd]"), m_NWCGStd);
	RFX_Text(pFX, _T("[RespAgency]"), m_RespAgency);

}
/////////////////////////////////////////////////////////////////////////////
// CRunPocketCardSet diagnostics

#ifdef _DEBUG
void CRunPocketCardSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CRunPocketCardSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG




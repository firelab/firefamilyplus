// CustomFireSet.h : Implementation of the CCustomFireSet class



// CCustomFireSet implementation

// code generated on Wednesday, September 28, 2005, 1:52 PM

#include "stdafx.h"
#include "CustomFireSet.h"
IMPLEMENT_DYNAMIC(CCustomFireSet, CRecordset)

CCustomFireSet::CCustomFireSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_AgencyID = 0;
	m_FRegion = 0;
	m_LRegion = 0;
	m_FUnit = 0;
	m_LUnit = 0;
	m_FSubunit = 0;
	m_LSubunit = 0;
	m_UseRegion = FALSE;
	m_Region = L"";
	m_UseUnit = FALSE;
	m_Unit = L"";
	m_UseSubunit = FALSE;
	m_Subunit = L"";
	m_Year = 0;
	m_Month = 0;
	m_Day = 0;
	m_FFireNum = 0;
	m_LFireNum = 0;
	m_FAcres = 0;
	m_LAcres = 0;
	m_Cause = 0;
	m_Duplicates = 0;
	m_FName = 0;
	m_LName = 0;
	m_nFields = 24;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CCustomFireSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CCustomFireSet::GetDefaultSQL()
{
	return _T("[ffpCustomFire]");
}

void CCustomFireSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[AgencyID]"), m_AgencyID);
	RFX_Long (pFX, _T("[FRegion]"), m_FRegion);
	RFX_Long (pFX, _T("[LRegion]"), m_LRegion);
	RFX_Long (pFX, _T("[FUnit]"), m_FUnit);
	RFX_Long (pFX, _T("[LUnit]"), m_LUnit);
	RFX_Long (pFX, _T("[FSubunit]"), m_FSubunit);
	RFX_Long (pFX, _T("[LSubunit]"), m_LSubunit);
	RFX_Bool(pFX, _T("[UseRegion]"), m_UseRegion);
	RFX_Text(pFX, _T("[Region]"), m_Region);
	RFX_Bool(pFX, _T("[UseUnit]"), m_UseUnit);
	RFX_Text(pFX, _T("[Unit]"), m_Unit);
	RFX_Bool(pFX, _T("[UseSubunit]"), m_UseSubunit);
	RFX_Text(pFX, _T("[Subunit]"), m_Subunit);
	RFX_Long (pFX, _T("[Year]"), m_Year);
	RFX_Long (pFX, _T("[Month]"), m_Month);
	RFX_Long (pFX, _T("[Day]"), m_Day);
	RFX_Long (pFX, _T("[FFireNum]"), m_FFireNum);
	RFX_Long (pFX, _T("[LFireNum]"), m_LFireNum);
	RFX_Long (pFX, _T("[FAcres]"), m_FAcres);
	RFX_Long (pFX, _T("[LAcres]"), m_LAcres);
	RFX_Long (pFX, _T("[Cause]"), m_Cause);
	RFX_Long (pFX, _T("[Duplicates]"), m_Duplicates);
	RFX_Long (pFX, _T("[FName]"), m_FName);
	RFX_Long (pFX, _T("[LName]"), m_LName);

}
/////////////////////////////////////////////////////////////////////////////
// CCustomFireSet diagnostics

#ifdef _DEBUG
void CCustomFireSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CCustomFireSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



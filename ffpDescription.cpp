// ffpDescription.h : Implementation of the CffpDescription class

 

// CffpDescription implementation

// code generated on Monday, September 26, 2005, 12:35 PM

#include "stdafx.h"
#include "ffpDescription.h"
IMPLEMENT_DYNAMIC(CffpDescription, CRecordset)

CffpDescription::CffpDescription(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_Description = L"";
	m_SIGStation = L"";
	m_StartYear = 0;
	m_EndYear = 0;
	m_StartMonth = L"";
	m_EndMonth = L"";
	m_StartDay = 0;
	m_EndDay = 0;
	m_PeriodLength = L"";
	m_Use78 = FALSE;
	m_Use88 = FALSE;
	m_UseCanadian = FALSE;
	m_nFields = 12;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CffpDescription::GetDefaultConnect()
{
	return _T("");//"DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CffpDescription::GetDefaultSQL()
{
	return _T("[ffpDescription]");
}

void CffpDescription::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[Description]"), m_Description);
	RFX_Text(pFX, _T("[SIG/Station]"), m_SIGStation);
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
// CffpDescription diagnostics

#ifdef _DEBUG
void CffpDescription::AssertValid() const
{
	CRecordset::AssertValid();
}

void CffpDescription::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



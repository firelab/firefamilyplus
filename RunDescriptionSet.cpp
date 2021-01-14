// RunDescriptionSet.h : Implementation of the CRunDescriptionSet class


 
// CRunDescriptionSet implementation

// code generated on Wednesday, September 28, 2005, 2:13 PM

#include "stdafx.h"
#include "RunDescriptionSet.h"
IMPLEMENT_DYNAMIC(CRunDescriptionSet, CRecordset)

CRunDescriptionSet::CRunDescriptionSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_runID = 0;
	m_Description = L"";
	m_SIG_Station = L"";
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
	m_nFields = 13;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CRunDescriptionSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CRunDescriptionSet::GetDefaultSQL()
{
	return _T("[ffpRunDescription]");
}

void CRunDescriptionSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[runID]"), m_runID);
	RFX_Text(pFX, _T("[Description]"), m_Description);
	RFX_Text(pFX, _T("[SIG_Station]"), m_SIG_Station);
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
// CRunDescriptionSet diagnostics

#ifdef _DEBUG
void CRunDescriptionSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CRunDescriptionSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



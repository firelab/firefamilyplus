// BatchOptionSet.h : Implementation of the CBatchOptionsSet class


 
// CBatchOptionsSet implementation

// code generated on Wednesday, September 28, 2005, 1:45 PM

#include "stdafx.h"
#include "BatchOptionsSet.h"
IMPLEMENT_DYNAMIC(CBatchOptionsSet, CRecordset)

CBatchOptionsSet::CBatchOptionsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_maintScheme = tNone;
	m_maintValue = 0;
	m_runDesc = L"";
	m_PromptDesc = FALSE;
	m_nFields = 4;
	m_nDefaultType = dynaset;

}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CBatchOptionsSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CBatchOptionsSet::GetDefaultSQL()
{
	return _T("[ffpBatchOptions]");
}

void CBatchOptionsSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[maintScheme]"), m_maintScheme);
	RFX_Long (pFX, _T("[maintValue]"), m_maintValue);
	RFX_Bool(pFX, _T("[PromptDesc]"), m_PromptDesc);

	RFX_Text(pFX, _T("[runDesc]"), m_runDesc);

}
/////////////////////////////////////////////////////////////////////////////
// CBatchOptionsSet diagnostics

#ifdef _DEBUG
void CBatchOptionsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CBatchOptionsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



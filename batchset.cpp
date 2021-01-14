// BatchSet.h : Implementation of the CBatchSet class



// CBatchSet implementation

// code generated on Wednesday, September 28, 2005, 1:49 PM

#include "stdafx.h"
#include "BatchSet.h"
IMPLEMENT_DYNAMIC(CBatchSet, CRecordset)

CBatchSet::CBatchSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_batchID = 0;
	m_CreatedOn = L"";
	m_LastRun = L"";
	m_Description = L"";
	m_nFields = 4;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CBatchSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CBatchSet::GetDefaultSQL()
{
	return _T("[ffpBatch]");
}

void CBatchSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[batchID]"), m_batchID);
	RFX_Text(pFX, _T("[CreatedOn]"), m_CreatedOn);
	RFX_Text(pFX, _T("[LastRun]"), m_LastRun);
	RFX_Text(pFX, _T("[Description]"), m_Description);

}
/////////////////////////////////////////////////////////////////////////////
// CBatchSet diagnostics

#ifdef _DEBUG
void CBatchSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CBatchSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



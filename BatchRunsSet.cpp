// BatchRunsSet.h : Implementation of the CBatchRunsSet class



// CBatchRunsSet implementation

// code generated on Wednesday, September 28, 2005, 1:47 PM

#include "stdafx.h"
#include "BatchRunsSet.h"
IMPLEMENT_DYNAMIC(CBatchRunsSet, CRecordset)

CBatchRunsSet::CBatchRunsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_batchID = 0;
	m_runID = 0;
	m_nFields = 2;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CBatchRunsSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CBatchRunsSet::GetDefaultSQL()
{
	return _T("[ffpBatchRuns]");
}

void CBatchRunsSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[batchID]"), m_batchID);
	RFX_Long (pFX, _T("[runID]"), m_runID);

}
/////////////////////////////////////////////////////////////////////////////
// CBatchRunsSet diagnostics

#ifdef _DEBUG
void CBatchRunsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CBatchRunsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



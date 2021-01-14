#include "stdafx.h"
#include "DistinctRunReportsSet.h"


IMPLEMENT_DYNAMIC(CDistinctRunReportsSet, CRecordset)

CDistinctRunReportsSet::CDistinctRunReportsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_reportID = 0;
	m_nFields = 1;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CDistinctRunReportsSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CDistinctRunReportsSet::GetDefaultSQL()
{
	return _T("[ffpRunOptions]");
}

void CDistinctRunReportsSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[reportID]"), m_reportID);
}
/////////////////////////////////////////////////////////////////////////////
// CWxSet diagnostics

#ifdef _DEBUG
void CDistinctRunReportsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CDistinctRunReportsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


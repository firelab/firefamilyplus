

// CTermsSet implementation


#include "stdafx.h"
#include "TermDatesSet.h"

IMPLEMENT_DYNAMIC(CTermDatesSet, CRecordset)

CTermDatesSet::CTermDatesSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_TermID = 0;
	m_Year = 1900;
	m_Date;
	m_Comment = "";
	m_nFields = 4;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CTermDatesSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CTermDatesSet::GetDefaultSQL()
{
	return _T("[ffpTermDates]");
}

void CTermDatesSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[TermID]"), m_TermID);
	RFX_Long (pFX, _T("[Year]"), m_Year);
	RFX_Date(pFX, _T("[Date]"), m_Date);
	RFX_Text(pFX, _T("[Comment]"), m_Comment);

}
/////////////////////////////////////////////////////////////////////////////
// CRunsSet diagnostics

#ifdef _DEBUG
void CTermDatesSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CTermDatesSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



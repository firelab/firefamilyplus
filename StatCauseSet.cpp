// StatCauseSet.h : Implementation of the CStatCauseSet class

 

// CStatCauseSet implementation

// code generated on Wednesday, September 28, 2005, 2:22 PM

#include "stdafx.h"
#include "StatCauseSet.h"
IMPLEMENT_DYNAMIC(CStatCauseSet, CRecordset)

CStatCauseSet::CStatCauseSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_StatisticalCause = 0;
	m_Abbr = L"";
	m_Descr = L"";
	m_Description = L"";
	m_nFields = 4;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CStatCauseSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CStatCauseSet::GetDefaultSQL()
{
	return _T("[refFStatisticalCause]");
}

void CStatCauseSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[StatisticalCause]"), m_StatisticalCause);
	RFX_Text(pFX, _T("[Abbr]"), m_Abbr);
	RFX_Text(pFX, _T("[Descr]"), m_Descr);
	RFX_Text(pFX, _T("[Description]"), m_Description);

}
/////////////////////////////////////////////////////////////////////////////
// CStatCauseSet diagnostics

#ifdef _DEBUG
void CStatCauseSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CStatCauseSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG





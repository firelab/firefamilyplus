// DOICauseSet.h : Implementation of the CDOICauseSet class

 

// CDOICauseSet implementation


#include "stdafx.h"
#include "DOICauseSet.h"
IMPLEMENT_DYNAMIC(CDOICauseSet, CRecordset)

CDOICauseSet::CDOICauseSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_GeneralCause = 0;
	m_SpecificCause = 0;
	m_GeneralDescription = L"";
	m_SpecificDescription = L"";
	m_Abbr = L"";
	
	m_nFields = 5;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CDOICauseSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CDOICauseSet::GetDefaultSQL()
{
	return _T("[refDOICause]");
}

void CDOICauseSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long 1() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[GeneralCause]"), m_GeneralCause);
	RFX_Long(pFX, _T("[SpecificCause]"), m_SpecificCause);
	RFX_Text(pFX, _T("[Abbr]"), m_Abbr);
	RFX_Text(pFX, _T("[GeneralDescription]"), m_GeneralDescription);
	RFX_Text(pFX, _T("[SpecificDescription]"), m_SpecificDescription);

}
/////////////////////////////////////////////////////////////////////////////
// CDOICauseSet diagnostics

#ifdef _DEBUG
void CDOICauseSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CDOICauseSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG





// CountySet.h : Implementation of the CCountySet class



// CCountySet implementation

// code generated on Wednesday, September 28, 2005, 1:50 PM

#include "stdafx.h"
#include "CountySet.h"
IMPLEMENT_DYNAMIC(CCountySet, CRecordset)

CCountySet::CCountySet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_State = L"";
	m_CountyID = 0;
	m_CountyName = L"";
	m_StateCode = 0;
	m_nFields = 4;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CCountySet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CCountySet::GetDefaultSQL()
{
	return _T("[refCounty]");
}

void CCountySet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[State]"), m_State);
	RFX_Long(pFX, _T("[CountyID]"), m_CountyID);
	RFX_Text(pFX, _T("[CountyName]"), m_CountyName);
	RFX_Long(pFX, _T("[StateCode]"), m_StateCode);

}
/////////////////////////////////////////////////////////////////////////////
// CCountySet diagnostics

#ifdef _DEBUG
void CCountySet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CCountySet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



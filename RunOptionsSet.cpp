// RunOptionsSet.h : Implementation of the CRunOptionsSet class
 


// CRunOptionsSet implementation

// code generated on Wednesday, September 28, 2005, 2:16 PM

#include "stdafx.h"
#include "RunOptionsSet.h"
IMPLEMENT_DYNAMIC(CRunOptionsSet, CRecordset)

CRunOptionsSet::CRunOptionsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_runID = 0;
	m_reportID = 0;
	m_row = 0;
	m_Variable = L"";
	m_Value = L"";
	m_nFields = 5;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CRunOptionsSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CRunOptionsSet::GetDefaultSQL()
{
	return _T("[ffpRunOptions]");
}

void CRunOptionsSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[runID]"), m_runID);
	RFX_Long (pFX, _T("[reportID]"), m_reportID);
	RFX_Long (pFX, _T("[row]"), m_row);
	RFX_Text(pFX, _T("[Variable]"), m_Variable);
	RFX_Text(pFX, _T("[Value]"), m_Value);

}
/////////////////////////////////////////////////////////////////////////////
// CRunOptionsSet diagnostics

#ifdef _DEBUG
void CRunOptionsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CRunOptionsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



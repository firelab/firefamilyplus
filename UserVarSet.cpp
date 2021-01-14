// UserVarSet.h : Implementation of the CUserVarSet class


// CUserVarSet implementation

// code generated on Wednesday, September 28, 2005, 2:26 PM

#include "stdafx.h"
#include "UserVarSet.h"
IMPLEMENT_DYNAMIC(CUserVarSet, CRecordset)

CUserVarSet::CUserVarSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_VarID = 0;
	m_UserVarID = 0;
	m_VarName = L"";
	m_Abbrev = L"";
	m_Decimals = 0;
	m_nFields = 5;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CUserVarSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CUserVarSet::GetDefaultSQL()
{
	return _T("[ffpUserVars]");
}

void CUserVarSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Byte(pFX, _T("[VarID]"), m_VarID);
	RFX_Byte(pFX, _T("[UserVarID]"), m_UserVarID);
	RFX_Text(pFX, _T("[Variable Name]"), m_VarName);
	RFX_Text(pFX, _T("[Abbrev]"), m_Abbrev);
	RFX_Long (pFX, _T("[Decimals]"), m_Decimals);

}
/////////////////////////////////////////////////////////////////////////////
// CUserVarSet diagnostics

#ifdef _DEBUG
void CUserVarSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CUserVarSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



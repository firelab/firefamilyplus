// GenericWxImportSet.cpp : Implementation of the CGenericWxImportSet class

 

// CGenericWxImportSet implementation

#include "stdafx.h"
#include "GenericWxImportSet.h"
IMPLEMENT_DYNAMIC(CGenericWxImportSet, CRecordset)

CGenericWxImportSet::CGenericWxImportSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_Delimiter = 0;
	m_DateFormat = 0;
	m_TimeFormat = 0;
	m_DupHandling = 0;
	m_UseStation = 0;
	m_DefStation = "";
	m_UseObsType = 0;
	m_DefObsType = 0;
	m_UseObsTime = 0;
	//m_DefObsTime;
	m_nFields = 10;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CGenericWxImportSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CGenericWxImportSet::GetDefaultSQL()
{
	return _T("[ffpGenericWxImports]");
}

void CGenericWxImportSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[Delimiter]"), m_Delimiter);
	RFX_Long (pFX, _T("[DateFormat]"), m_DateFormat);
	RFX_Long (pFX, _T("[TimeFormat]"), m_TimeFormat);
	RFX_Long (pFX, _T("[DupHandling]"), m_DupHandling);
	RFX_Long (pFX, _T("[UseStation]"), m_UseStation);
	RFX_Text(pFX, _T("[DefStation]"), m_DefStation);
	RFX_Long (pFX, _T("[UseObsType]"), m_UseObsType);
	RFX_Long (pFX, _T("[DefObsType]"), m_DefObsType);
	RFX_Long (pFX, _T("[UseObsTime]"), m_UseObsTime);
	RFX_Date(pFX, _T("[DefObsTime]"), m_DefObsTime);
}
/////////////////////////////////////////////////////////////////////////////
// CGenericWxImportSet diagnostics

#ifdef _DEBUG
void CGenericWxImportSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CGenericWxImportSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



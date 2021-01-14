// GenericFireImportSet.h : Implementation of the CGenericFireImportSet class

 

// CGenericFireImportSet implementation

#include "stdafx.h"
#include "GenericFireImportSet.h"
IMPLEMENT_DYNAMIC(CGenericFireImportSet, CRecordset)

CGenericFireImportSet::CGenericFireImportSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_AgencyID = 0;
	m_Delimiter = 0;
	m_DateFormat = 0;
	m_TimeFormat = 0;
	m_UseRegion = 0;
	m_DefaultRegionID = 0;
	m_UseUnit = 0;
	m_DefaultUnitID = 0;
	m_UseSubunit = 0;
	m_DefaultSubunitID = 0;
	m_DupHandling = 0;
	m_nFields = 11;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CGenericFireImportSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CGenericFireImportSet::GetDefaultSQL()
{
	return _T("[ffpGenericFireImports]");
}

void CGenericFireImportSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[AgencyID]"), m_AgencyID);
	RFX_Long (pFX, _T("[Delimiter]"), m_Delimiter);
	RFX_Long (pFX, _T("[DateFormat]"), m_DateFormat);
	RFX_Long (pFX, _T("[TimeFormat]"), m_TimeFormat);
	RFX_Long (pFX, _T("[UseRegion]"), m_UseRegion);
	RFX_Long (pFX, _T("[DefaultRegionID]"), m_DefaultRegionID);
	RFX_Long (pFX, _T("[UseUnit]"), m_UseUnit);
	RFX_Long (pFX, _T("[DefaultUnitID]"), m_DefaultUnitID);
	RFX_Long (pFX, _T("[UseSubunit]"), m_UseSubunit);
	RFX_Long (pFX, _T("[DefaultSubunitID]"), m_DefaultSubunitID);
	RFX_Long (pFX, _T("[DupHandling]"), m_DupHandling);
}
/////////////////////////////////////////////////////////////////////////////
// CGenericFireImportSet diagnostics

#ifdef _DEBUG
void CGenericFireImportSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CGenericFireImportSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



// DOIFireSet.h : Implementation of the CDOIFireSet class



// CDOIFireSet implementation

// code generated on Wednesday, September 28, 2005, 2:05 PM

#include "stdafx.h"
#include "DOIFireSet.h"
IMPLEMENT_DYNAMIC(CDOIFireSet, CRecordset)

CDOIFireSet::CDOIFireSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_FireTypeDef = L"";
	m_ProtectionTypeDef = L"";

	m_AgencyID = 0;
	m_FireType = 0;
	m_ProtectionType = 0;
  

	m_nFields = 5;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CDOIFireSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CDOIFireSet::GetDefaultSQL()
{
	return _T("[refDOIFires]");
}

void CDOIFireSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[FireType]"), m_FireType);
	RFX_Long(pFX, _T("[ProtectionType]"), m_ProtectionType);
	RFX_Long(pFX, _T("[AgencyID]"), m_AgencyID);
	RFX_Text(pFX, _T("[FireTypeDef]"), m_FireTypeDef);
	RFX_Text(pFX, _T("[ProtectionTypeDef]"), m_ProtectionTypeDef);

}
/////////////////////////////////////////////////////////////////////////////
// CDOIFireSet diagnostics

#ifdef _DEBUG
void CDOIFireSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CDOIFireSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



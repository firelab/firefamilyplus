
 

// CTermsSet implementation


#include "stdafx.h"
#include "SowOptionsSet.h"
//#include "SIGStationSet.h"

IMPLEMENT_DYNAMIC(CSowOptionsSet, CRecordset)

CSowOptionsSet::CSowOptionsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_SIG_Station = "";
	m_SOW_Usage = 0;
	m_WetFlag_Usage = 0;
	m_nFields = 3;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CSowOptionsSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CSowOptionsSet::GetDefaultSQL()
{
	return _T("[ffpSowOptions]");
}

void CSowOptionsSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[SIG_Station]"), m_SIG_Station);
	RFX_Long (pFX, _T("[SOW_Usage]"), m_SOW_Usage);
	RFX_Long (pFX, _T("[WetFlag_Usage]"), m_WetFlag_Usage);

}
/////////////////////////////////////////////////////////////////////////////
// CRunsSet diagnostics

#ifdef _DEBUG
void CSowOptionsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CSowOptionsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


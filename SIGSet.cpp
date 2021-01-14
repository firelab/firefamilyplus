// SIGSet.h : Implementation of the CSIGSet class

 

// CSIGSet implementation

// code generated on Wednesday, September 28, 2005, 2:20 PM

#include "stdafx.h"
#include "SIGSet.h"
IMPLEMENT_DYNAMIC(CSIGSet, CRecordset)

CSIGSet::CSIGSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_SIG = L"";
	m_DESCRIPTION = L"";
	m_WEIGHTED = FALSE;
	m_nFields = 3;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CSIGSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CSIGSet::GetDefaultSQL()
{
	return _T("[ffpSIGS]");
}

void CSIGSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[SIG]"), m_SIG);
	RFX_Text(pFX, _T("[DESCRIPTION]"), m_DESCRIPTION);
	RFX_Bool(pFX, _T("[WEIGHTED]"), m_WEIGHTED);

}
/////////////////////////////////////////////////////////////////////////////
// CSIGSet diagnostics

#ifdef _DEBUG
void CSIGSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CSIGSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



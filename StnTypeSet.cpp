// StnTypeSet.h : Implementation of the CStnTypeSet class

 

// CStnTypeSet implementation

// code generated on Wednesday, September 28, 2005, 2:24 PM

#include "stdafx.h"
#include "StnTypeSet.h"
IMPLEMENT_DYNAMIC(CStnTypeSet, CRecordset)

CStnTypeSet::CStnTypeSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_StnType = 0;
	m_TypeDescr = L"";
	m_nFields = 2;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CStnTypeSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CStnTypeSet::GetDefaultSQL()
{
	return _T("[refWxStnType]");
}

void CStnTypeSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[StnType]"), m_StnType);
	RFX_Text(pFX, _T("[TypeDescr]"), m_TypeDescr);

}
/////////////////////////////////////////////////////////////////////////////
// CStnTypeSet diagnostics

#ifdef _DEBUG
void CStnTypeSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CStnTypeSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



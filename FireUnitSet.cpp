// FireUnitSet.h : Implementation of the CFireUnitSet class

 

// CFireUnitSet implementation

// code generated on Wednesday, September 28, 2005, 2:05 PM

#include "stdafx.h"
#include "FireUnitSet.h"
IMPLEMENT_DYNAMIC(CFireUnitSet, CRecordset)

CFireUnitSet::CFireUnitSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_UnitID = 0;
	m_RegionID = 0;
	m_Name = L"";
	m_nFields = 3;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CFireUnitSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CFireUnitSet::GetDefaultSQL()
{
	return _T("[FireUnits]");
}

void CFireUnitSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[UnitID]"), m_UnitID);
	RFX_Long(pFX, _T("[RegionID]"), m_RegionID);
	RFX_Text(pFX, _T("[Name]"), m_Name);

}
/////////////////////////////////////////////////////////////////////////////
// CFireUnitSet diagnostics

#ifdef _DEBUG
void CFireUnitSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CFireUnitSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



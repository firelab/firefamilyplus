// FFPDailyEventsSet.cpp : Implementation of the CFFPDailyEventsSet class


 
// CFFPDailyEventsSet implementation

#include "stdafx.h"
#include "FFPDailyEventsSet.h"
IMPLEMENT_DYNAMIC(CFFPDailyEventsSet, CRecordset)

CFFPDailyEventsSet::CFFPDailyEventsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_SIG_Station = "";
	m_Row = 0;
	m_Category = 0;
	m_RowOp = 1;
	m_ValOp = 0;
	m_ValType = 0;
	m_Value = 1.0;
	m_VarID = 0;
	m_VarString = "";
	m_nFields = 9;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CFFPDailyEventsSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CFFPDailyEventsSet::GetDefaultSQL()
{
	return _T("[ffpDailyEvents]");
}

void CFFPDailyEventsSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[SIG_Station]"), m_SIG_Station);
	RFX_Long (pFX, _T("[Row]"), m_Row);
	RFX_Long (pFX, _T("[Category]"), m_Category);
	RFX_Long (pFX, _T("[RowOp]"), m_RowOp);
	RFX_Long (pFX, _T("[ValOp]"), m_ValOp);
	RFX_Long (pFX, _T("[ValType]"), m_ValType);
	RFX_Double(pFX, _T("[Value]"), m_Value);
	RFX_Long (pFX, _T("[VarID]"), m_VarID);
	RFX_Text(pFX, _T("[VarString]"), m_VarString);
}
/////////////////////////////////////////////////////////////////////////////
// CFFPDailyEventsSet diagnostics

#ifdef _DEBUG
void CFFPDailyEventsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CFFPDailyEventsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



// FFPOptsSet.cpp : Implementation of the CFFPOptsSet class


 
// CFFPOptsSet implementation

#include "stdafx.h"
#include "FFPfxOptsSet.h"
IMPLEMENT_DYNAMIC(CFFPfxOptsSet, CRecordset)

CFFPfxOptsSet::CFFPfxOptsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_AddType = 0;
	m_TempPct = 90.0;
	m_RHPct = 90.0;
	m_WindPct = 90.0;
	m_LineColor = 0L;
	m_LineWidth = 1;
	m_LineStyle = 1;
	m_nFields = 7;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CFFPfxOptsSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CFFPfxOptsSet::GetDefaultSQL()
{
	return _T("[ffpFxOpts]");
}

void CFFPfxOptsSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[AddType]"), m_AddType);
	RFX_Double(pFX, _T("[TempPct]"), m_TempPct);
	RFX_Double(pFX, _T("[RHPct]"), m_RHPct);
	RFX_Double(pFX, _T("[WindPct]"), m_WindPct);
	RFX_Long(pFX, _T("[LineColor]"), m_LineColor);
	RFX_Long (pFX, _T("[LineWidth]"), m_LineWidth);
	RFX_Long (pFX, _T("[LineStyle]"), m_LineStyle);
}
/////////////////////////////////////////////////////////////////////////////
// CFFPOptsSet diagnostics

#ifdef _DEBUG
void CFFPfxOptsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CFFPfxOptsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



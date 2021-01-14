// CustomInitSet.h : Implementation of the CCustomInitSet class

// CCustomInitSet implementation


#include "stdafx.h"
#include "CustomInitSet.h"
IMPLEMENT_DYNAMIC(CCustomInitSet, CRecordset)

CCustomInitSet::CCustomInitSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_SIG_Station = L"";
	m_Year = 0;
	m_GreenJulian;
	m_StartKBDI = 0;
	m_Start1000 = 0.0;
	m_FFMC = 0.0;
	m_DMC = 0.0;
	m_DC = 0.0;
	m_StartUpDay;
	m_Enabled;
	

	m_nFields = 10;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CCustomInitSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CCustomInitSet::GetDefaultSQL()
{
	return _T("[ffpInit]");
}

void CCustomInitSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[SIG_Station]"), m_SIG_Station);
	RFX_Long (pFX, _T("[Year]"), m_Year);
	RFX_Date(pFX, _T("[GreenJulian]"), m_GreenJulian);
	RFX_Long (pFX, _T("[StartKBDI]"), m_StartKBDI);
	RFX_Single(pFX, _T("[Start1000]"), m_Start1000);
	RFX_Single(pFX, _T("[FFMC]"), m_FFMC);
	RFX_Single(pFX, _T("[DMC]"), m_DMC);
	RFX_Single(pFX, _T("[DC]"), m_DC);
	RFX_Bool(pFX, _T("[Enabled]"), m_Enabled);
	RFX_Date(pFX, _T("[StartUpDay]"), m_StartUpDay);

}
/////////////////////////////////////////////////////////////////////////////
// CCustomInitSet diagnostics

#ifdef _DEBUG
void CCustomInitSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CCustomInitSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



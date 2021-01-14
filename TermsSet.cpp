

// CTermsSet implementation


#include "stdafx.h"
#include "TermsSet.h"
IMPLEMENT_DYNAMIC(CTermsSet, CRecordset)

CTermsSet::CTermsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_SIG_Station = "";
	m_TermID = 0;
	m_Name = "";
	m_StartDate;
	m_Comment = "";
	m_Coeff1 = 0.0;
	m_Coeff2 = 0.0;
	m_Coeff3 = 0.0;
	m_nFields = 8;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CTermsSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CTermsSet::GetDefaultSQL()
{
	return _T("[ffpTerm]");
}

void CTermsSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[SIG_Station]"), m_SIG_Station);
	RFX_Long (pFX, _T("[TermID]"), m_TermID);
	RFX_Text(pFX, _T("[Name]"), m_Name);
	RFX_Date(pFX, _T("[StartDate]"), m_StartDate);
	//RFX_Text(pFX, _T("[Comment]"), m_Comment);
	RFX_Text(pFX, _T("[Comment]"), m_Comment, 6400, SQL_VARCHAR );
	RFX_Double(pFX, _T("[Coeff1]"), m_Coeff1);
	RFX_Double(pFX, _T("[Coeff2]"), m_Coeff2);
	RFX_Double(pFX, _T("[Coeff3]"), m_Coeff3);

}
/////////////////////////////////////////////////////////////////////////////
// CRunsSet diagnostics

#ifdef _DEBUG
void CTermsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CTermsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



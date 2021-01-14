// RunFireOptionsSet.h : Implementation of the CRunFireOptionsSet class

 

// CRunFireOptionsSet implementation

// code generated on Wednesday, September 28, 2005, 2:15 PM

#include "stdafx.h"
#include "RunFireOptionsSet.h"
IMPLEMENT_DYNAMIC(CRunFireOptionsSet, CRecordset)

CRunFireOptionsSet::CRunFireOptionsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_runID = 0;
	m_SIG_Station = L"";
	m_Lightning = FALSE;
	m_Human = FALSE;
	m_All = FALSE;
	m_Cumulative = FALSE;
	m_Probability = FALSE;
	m_Both = FALSE;
	m_LFAcres = 0;
	m_MFDFires = 0;
	m_VarID = 0;
	m_CauseFlag = 0;
	m_nFields = 12;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CRunFireOptionsSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CRunFireOptionsSet::GetDefaultSQL()
{
	return _T("[ffpRunFireOptions]");
}

void CRunFireOptionsSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[runID]"), m_runID);
	RFX_Text(pFX, _T("[SIG_Station]"), m_SIG_Station);
	RFX_Bool(pFX, _T("[Lightning]"), m_Lightning);
	RFX_Bool(pFX, _T("[Human]"), m_Human);
	RFX_Bool(pFX, _T("[All]"), m_All);
	RFX_Bool(pFX, _T("[Cumulative]"), m_Cumulative);
	RFX_Bool(pFX, _T("[Probability]"), m_Probability);
	RFX_Bool(pFX, _T("[Both]"), m_Both);
	RFX_Long(pFX, _T("[LFAcres]"), m_LFAcres);
	RFX_Long(pFX, _T("[MFDFires]"), m_MFDFires);
	RFX_Long(pFX, _T("[CauseFlag]"), m_CauseFlag);
	RFX_Byte(pFX, _T("[VarID]"), m_VarID);

}
/////////////////////////////////////////////////////////////////////////////
// CRunFireOptionsSet diagnostics

#ifdef _DEBUG
void CRunFireOptionsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CRunFireOptionsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



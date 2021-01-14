// StateSet.h : Implementation of the CStateSet class

 

// CStateSet implementation

// code generated on Wednesday, September 28, 2005, 2:22 PM

#include "stdafx.h"
#include "StateSet.h"
IMPLEMENT_DYNAMIC(CStateSet, CRecordset)

CStateSet::CStateSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_State = L"";
	m_StateName = L"";
	m_StateCode = 0;
	m_AFFState = 0;
	m_MinElev = 0;
	m_MaxElev = 0;
	m_LatDDN = 0;
	m_LatMMN = 0;
	m_LatSSN = 0;
	m_LatDDS = 0;
	m_LatMMS = 0;
	m_LatSSS = 0;
	m_LonDDE = 0;
	m_LonMME = 0;
	m_LonSSE = 0;
	m_LonDDW = 0;
	m_LonMMW = 0;
	m_LonSSW = 0;
	m_PLSS = L"";
	m_nFields = 19;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CStateSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CStateSet::GetDefaultSQL()
{
	return _T("[refState]");
}

void CStateSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[State]"), m_State);
	RFX_Text(pFX, _T("[StateName]"), m_StateName);
	RFX_Long(pFX, _T("[StateCode]"), m_StateCode);
	RFX_Long(pFX, _T("[AFFState]"), m_AFFState);
	RFX_Long(pFX, _T("[MinElev]"), m_MinElev);
	RFX_Long(pFX, _T("[MaxElev]"), m_MaxElev);
	RFX_Long(pFX, _T("[LatDDN]"), m_LatDDN);
	RFX_Long(pFX, _T("[LatMMN]"), m_LatMMN);
	RFX_Long(pFX, _T("[LatSSN]"), m_LatSSN);
	RFX_Long(pFX, _T("[LatDDS]"), m_LatDDS);
	RFX_Long(pFX, _T("[LatMMS]"), m_LatMMS);
	RFX_Long(pFX, _T("[LatSSS]"), m_LatSSS);
	RFX_Long(pFX, _T("[LonDDE]"), m_LonDDE);
	RFX_Long(pFX, _T("[LonMME]"), m_LonMME);
	RFX_Long(pFX, _T("[LonSSE]"), m_LonSSE);
	RFX_Long(pFX, _T("[LonDDW]"), m_LonDDW);
	RFX_Long(pFX, _T("[LonMMW]"), m_LonMMW);
	RFX_Long(pFX, _T("[LonSSW]"), m_LonSSW);
	RFX_Text(pFX, _T("[PLSS]"), m_PLSS);

}
/////////////////////////////////////////////////////////////////////////////
// CStateSet diagnostics

#ifdef _DEBUG
void CStateSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CStateSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



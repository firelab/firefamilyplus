// LFISet.h : Implementation of the CRunsSet class



// CCandidateSet implementation

// code generated on Wednesday, September 28, 2005, 2:17 PM

#include "stdafx.h"
#include "CandidateSet.h"
IMPLEMENT_DYNAMIC(CCandidateSet, CRecordset)

CCandidateSet::CCandidateSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_SIG_Station = "";
	m_MODEL = "";
	m_VarName = "";
	m_StartYear = 0;
	m_EndYear = 0;
	m_StartMonth = "";
	m_EndMonth = "";
	m_StartDay = 0;
	m_EndDay = 0;
	//m_GreenJulian;
	//m_FreezeJulian;
	m_FD_Type = "";
	m_FD_R2 = 0.0;
	m_FD_Chi2 = 0.0;
	m_FD_PVal = 0.0;
	m_FD_PMin = 0.0;
	m_FD_PMax = 0.0;
	m_FD_Beta0 = 0.0;
	m_FD_Beta1 = 0.0;
	m_LFD_Acres = "";
	m_LFD_R2 = 0.0;
	m_LFD_Chi2 = 0.0;
	m_LFD_PVal = 0.0;
	m_LFD_PMin = 0.0;
	m_LFD_PMax = 0.0;
	m_LFD_Beta0 = 0.0;
	m_LFD_Beta1 = 0.0;
	m_MFD_NumFires = "";
	m_MFD_R2 = 0.0;
	m_MFD_Chi2 = 0.0;
	m_MFD_PVal = 0.0;
	m_MFD_PMin = 0.0;
	m_MFD_PMax = 0.0;
	m_MFD_Beta0 = 0.0;
	m_MFD_Beta1 = 0.0;
	m_Comment = "";
	m_nFields = 36;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CCandidateSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CCandidateSet::GetDefaultSQL()
{
	return _T("[ffpCandidates]");
}

void CCandidateSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[SIG_Station]"), m_SIG_Station);
	RFX_Text(pFX, _T("[MODEL]"), m_MODEL);
	RFX_Text(pFX, _T("[VarName]"), m_VarName);
	RFX_Long (pFX, _T("[StartYear]"), m_StartYear);
	RFX_Long (pFX, _T("[EndYear]"), m_EndYear);
	RFX_Text(pFX, _T("[StartMonth]"), m_StartMonth);
	RFX_Text(pFX, _T("[EndMonth]"), m_EndMonth);
	RFX_Long (pFX, _T("[StartDay]"), m_StartDay);
	RFX_Long (pFX, _T("[EndDay]"), m_EndDay);
	RFX_Date(pFX, _T("[GreenJulian]"), m_GreenJulian);
	RFX_Date(pFX, _T("[FreezeJulian]"), m_FreezeJulian);
	RFX_Text(pFX, _T("[FD_Type]"), m_FD_Type);
	RFX_Double(pFX, _T("[FD_R2]"), m_FD_R2);
	RFX_Double(pFX, _T("[FD_Chi2]"), m_FD_Chi2);
	RFX_Double(pFX, _T("[FD_PVal]"), m_FD_PVal);
	RFX_Double(pFX, _T("[FD_PMin]"), m_FD_PMin);
	RFX_Double(pFX, _T("[FD_PMax]"), m_FD_PMax);
	RFX_Double(pFX, _T("[FD_Beta0]"), m_FD_Beta0);
	RFX_Double(pFX, _T("[FD_Beta1]"), m_FD_Beta1);
	RFX_Text(pFX, _T("[LFD_Acres]"), m_LFD_Acres);
	RFX_Double(pFX, _T("[LFD_R2]"), m_LFD_R2);
	RFX_Double(pFX, _T("[LFD_Chi2]"), m_LFD_Chi2);
	RFX_Double(pFX, _T("[LFD_PVal]"), m_LFD_PVal);
	RFX_Double(pFX, _T("[LFD_PMin]"), m_LFD_PMin);
	RFX_Double(pFX, _T("[LFD_PMax]"), m_LFD_PMax);
	RFX_Double(pFX, _T("[LFD_Beta0]"), m_LFD_Beta0);
	RFX_Double(pFX, _T("[LFD_Beta1]"), m_LFD_Beta1);
	RFX_Text(pFX, _T("[MFD_NumFires]"), m_MFD_NumFires);
	RFX_Double(pFX, _T("[MFD_R2]"), m_MFD_R2);
	RFX_Double(pFX, _T("[MFD_Chi2]"), m_MFD_Chi2);
	RFX_Double(pFX, _T("[MFD_PVal]"), m_MFD_PVal);
	RFX_Double(pFX, _T("[MFD_PMin]"), m_MFD_PMin);
	RFX_Double(pFX, _T("[MFD_PMax]"), m_MFD_PMax);
	RFX_Double(pFX, _T("[MFD_Beta0]"), m_MFD_Beta0);
	RFX_Double(pFX, _T("[MFD_Beta1]"), m_MFD_Beta1);
	RFX_Text(pFX, _T("[Comment]"), m_Comment);

}
/////////////////////////////////////////////////////////////////////////////
// CRunsSet diagnostics

#ifdef _DEBUG
void CCandidateSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CCandidateSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



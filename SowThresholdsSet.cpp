 


// CTermsSet implementation


#include "stdafx.h"
#include "SowThresholdsSet.h"
#include "SIGStationSet.h"

IMPLEMENT_DYNAMIC(CSowThresholdsSet, CRecordset)

CSowThresholdsSet::CSowThresholdsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_SIG_Station = "";
	m_PCNT_Clear = 85;
	m_PCNT_Scattered = 75;
	m_PCNT_Broken = 50;
	m_1HR_Drizzle = 0.05;
	m_1HR_Rain = 0.1;
	m_1HR_Showers = 0.25;
	m_3HR_DUR_WetFlag = 3;
	m_3HR_AMT_WetFlag = 0.5;
	m_24HR_DUR_WetFlag = 12;
	m_24HR_AMT_WetFlag = 1.0;

	m_nFields = 11;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CSowThresholdsSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CSowThresholdsSet::GetDefaultSQL()
{
	return _T("[ffpSowThresholds]");
}

void CSowThresholdsSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	/*
	CString m_SIG_Station;
	int m_PCNT_Clear;
	int m_PCNT_Scattered;
	int m_PCNT_Broken;
	double	m_1HR_Drizzle;
	double	m_1HR_Rain;
	double	m_1HR_Showers;
	int m_3HR_DUR_WetFlag;
	double m_3HR_AMT_WetFlag;
	int m_24HR_DUR_WetFlag;
	double m_24HR_AMT_WetFlag;
	*/
	RFX_Text(pFX, _T("[SIG_Station]"), m_SIG_Station);
	RFX_Long (pFX, _T("[PCNT_Clear]"), m_PCNT_Clear);
	RFX_Long (pFX, _T("[PCNT_Scattered]"), m_PCNT_Scattered);
	RFX_Long (pFX, _T("[PCNT_Broken]"), m_PCNT_Broken);
	RFX_Double(pFX, _T("[1HR_Drizzle]"), m_1HR_Drizzle);
	RFX_Double(pFX, _T("[1HR_Rain]"), m_1HR_Rain);
	RFX_Double(pFX, _T("[1HR_Showers]"), m_1HR_Showers);
	RFX_Long (pFX, _T("[3HR_DUR_WetFlag]"), m_3HR_DUR_WetFlag);
	RFX_Double(pFX, _T("[3HR_AMT_WetFlag]"), m_3HR_AMT_WetFlag);
	RFX_Long (pFX, _T("[24HR_DUR_WetFlag]"), m_24HR_DUR_WetFlag);
	RFX_Double(pFX, _T("[24HR_AMT_WetFlag]"), m_24HR_AMT_WetFlag);

}
/////////////////////////////////////////////////////////////////////////////
// CRunsSet diagnostics

#ifdef _DEBUG
void CSowThresholdsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CSowThresholdsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

void CSowThresholdsSet::AddDefaultForStation(CString staID)
{
	int climClass = 3;
	CSIGStationSet staSet(this->m_pDatabase);
	staSet.m_strFilter.Format("[StationID] = '%6.6s'", staID);
	staSet.Open();
	if(!staSet.IsBOF() && !staSet.IsEOF())
	{
		climClass = staSet.m_ClimateCls;
	}
	staSet.Close();

	this->AddNew();
	m_SIG_Station = staID;
	m_PCNT_Clear = 85;
	m_PCNT_Scattered = 75;
	m_PCNT_Broken = 50;
	switch(climClass)
	{
	case 1:
		m_1HR_Drizzle = 0.1;
		m_1HR_Rain = 0.15;
		m_1HR_Showers = 0.5;
		m_3HR_DUR_WetFlag = 2;
		m_3HR_AMT_WetFlag = 0.75;
		m_24HR_DUR_WetFlag = 8;
		m_24HR_AMT_WetFlag = 2.0;
		break;
	case 2:
		m_1HR_Drizzle = 0.1;
		m_1HR_Rain = 0.15;
		m_1HR_Showers = 0.5;
		m_3HR_DUR_WetFlag = 2;
		m_3HR_AMT_WetFlag = 0.75;
		m_24HR_DUR_WetFlag = 10;
		m_24HR_AMT_WetFlag = 1.5;
		break;
	case 4:
		m_1HR_Drizzle = 0.05;
		m_1HR_Rain = 0.1;
		m_1HR_Showers = 0.25;
		m_3HR_DUR_WetFlag = 3;
		m_3HR_AMT_WetFlag = 0.4;
		m_24HR_DUR_WetFlag = 12;
		m_24HR_AMT_WetFlag = 0.75;
		break;
	case 3:
	default:
		m_1HR_Drizzle = 0.05;
		m_1HR_Rain = 0.1;
		m_1HR_Showers = 0.25;
		m_3HR_DUR_WetFlag = 3;
		m_3HR_AMT_WetFlag = 0.5;
		m_24HR_DUR_WetFlag = 12;
		m_24HR_AMT_WetFlag = 1.0;
		break;
	}
	this->Update();
	Requery();
}


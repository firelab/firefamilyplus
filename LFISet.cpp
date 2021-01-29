// LFISet.h : Implementation of the CRunsSet class
 


// CLFISet implementation

// code generated on Wednesday, September 28, 2005, 2:17 PM

#include "stdafx.h"
#include "LFISet.h"
IMPLEMENT_DYNAMIC(CLFISet, CRecordset)

CLFISet::CLFISet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_SIG_Station = "######";
	m_LFIdaysAvg = 21;
	m_TMinMin = -2.0;
	m_TMinMax = 5.0;
	m_VPDMin = 900.0;
	m_VPDMax = 4100.0;
	m_DaylenMin = 36000.0;
	m_DaylenMax = 39600.0;
	m_PcpDays = 30;
	m_UseVPDAvg = TRUE;
	m_UseRTPrecip = FALSE;
	m_HerbDaysAvg = 21;
	m_HerbMaxGSI = 1.0;
	m_HerbGreenup = 0.5;
	m_HerbMax = 250.0;
	m_HerbMin = 30;
	m_HerbTMinMin = -2.0;
	m_HerbTMinMax = 5.0;
	m_HerbVPDMin = 900.0;
	m_HerbVPDMax = 4100.0;
	m_HerbDaylenMin = 36000.0;
	m_HerbDaylenMax = 39600.0;
	m_HerbPcpDays = 30;
	m_HerbUseVPDAvg = TRUE;
	m_HerbUseRTPrecip = FALSE;
	m_WoodyDaysAvg = 21;
	m_WoodyMaxGSI = 1.0;
	m_WoodyGreenup = 0.5;
	m_WoodyMax = 200.0;
	m_WoodyMin = 50.0;
	m_WoodyTMinMin = -2.0;
	m_WoodyTMinMax = 5.0;
	m_WoodyVPDMin = 900.0;
	m_WoodyVPDMax = 4100.0;
	m_WoodyDaylenMin = 36000.0;
	m_WoodyDaylenMax = 39600.0;
	m_WoodyPcpDays = 30;
	m_WoodyUseVPDAvg = TRUE;
	m_WoodyUseRTPrecip = FALSE;
	m_PcpMin = 0.5;
	m_PcpMax = 1.5;
	m_HerbPcpMin = 0.5;
	m_HerbPcpMax = 1.5;
	m_WoodyPcpMin = 0.5;
	m_WoodyPcpMax = 1.5;
	m_nFields = 45;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CLFISet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CLFISet::GetDefaultSQL()
{
	return _T("[ffpLFI]");
}

void CLFISet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[SIG_Station]"), m_SIG_Station);
	RFX_Long (pFX, _T("[LFIdaysAvg]"), m_LFIdaysAvg);
	RFX_Double(pFX, _T("[TMinMin]"), m_TMinMin);
	RFX_Double(pFX, _T("[TMinMax]"), m_TMinMax);
	RFX_Double(pFX, _T("[VPDMin]"), m_VPDMin);
	RFX_Double(pFX, _T("[VPDMax]"), m_VPDMax);
	RFX_Double(pFX, _T("[DaylenMin]"), m_DaylenMin);
	RFX_Double(pFX, _T("[DaylenMax]"), m_DaylenMax);
	RFX_Bool(pFX, _T("[UseVPDAvg]"), m_UseVPDAvg);
	RFX_Long(pFX, _T("[PcpDays]"), m_PcpDays);
	RFX_Double(pFX, _T("[PcpMin]"), m_PcpMin);
	RFX_Double(pFX, _T("[PcpMax]"), m_PcpMax);
	RFX_Bool(pFX, _T("[UseRTPrecip]"), m_UseRTPrecip);
	//Herb moisture fields
	RFX_Bool(pFX, _T("[HerbUseVPDAvg]"), m_HerbUseVPDAvg);
	RFX_Long (pFX, _T("[HerbDaysAvg]"), m_HerbDaysAvg);
	RFX_Double(pFX, _T("[HerbMaxGSI]"), m_HerbMaxGSI);
	RFX_Double(pFX, _T("[HerbGreenup]"), m_HerbGreenup);
	RFX_Double(pFX, _T("[HerbMax]"), m_HerbMax);
	RFX_Double(pFX, _T("[HerbMin]"), m_HerbMin);
	RFX_Double(pFX, _T("[HerbTMinMin]"), m_HerbTMinMin);
	RFX_Double(pFX, _T("[HerbTMinMax]"), m_HerbTMinMax);
	RFX_Double(pFX, _T("[HerbVPDMin]"), m_HerbVPDMin);
	RFX_Double(pFX, _T("[HerbVPDMax]"), m_HerbVPDMax);
	RFX_Double(pFX, _T("[HerbDaylenMin]"), m_HerbDaylenMin);
	RFX_Double(pFX, _T("[HerbDaylenMax]"), m_HerbDaylenMax);
	RFX_Long(pFX, _T("[HerbPcpDays]"), m_HerbPcpDays);
	RFX_Double(pFX, _T("[HerbPcpMin]"), m_HerbPcpMin);
	RFX_Double(pFX, _T("[HerbPcpMax]"), m_HerbPcpMax);
	RFX_Bool(pFX, _T("[UseHerbRTPrecip]"), m_HerbUseRTPrecip);
	//Herb moisture fields
	RFX_Long (pFX, _T("[WoodyDaysAvg]"), m_WoodyDaysAvg);
	RFX_Bool(pFX, _T("[WoodyUseVPDAvg]"), m_WoodyUseVPDAvg);
	RFX_Double(pFX, _T("[WoodyMax]"), m_WoodyMax);
	RFX_Double(pFX, _T("[WoodyMin]"), m_WoodyMin);
	RFX_Double(pFX, _T("[WoodyMaxGSI]"), m_WoodyMaxGSI);
	RFX_Double(pFX, _T("[WoodyGreenup]"), m_WoodyGreenup);
	RFX_Double(pFX, _T("[WoodyTMinMin]"), m_WoodyTMinMin);
	RFX_Double(pFX, _T("[WoodyTMinMax]"), m_WoodyTMinMax);
	RFX_Double(pFX, _T("[WoodyVPDMin]"), m_WoodyVPDMin);
	RFX_Double(pFX, _T("[WoodyVPDMax]"), m_WoodyVPDMax);
	RFX_Double(pFX, _T("[WoodyDaylenMin]"), m_WoodyDaylenMin);
	RFX_Double(pFX, _T("[WoodyDaylenMax]"), m_WoodyDaylenMax);
	RFX_Long(pFX, _T("[WoodyPcpDays]"), m_WoodyPcpDays);
	RFX_Double(pFX, _T("[WoodyPcpMin]"), m_WoodyPcpMin);
	RFX_Double(pFX, _T("[WoodyPcpMax]"), m_WoodyPcpMax);
	RFX_Bool(pFX, _T("[UseWoodyRTPrecip]"), m_WoodyUseRTPrecip);
}
/////////////////////////////////////////////////////////////////////////////
// CRunsSet diagnostics

#ifdef _DEBUG
void CLFISet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CLFISet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



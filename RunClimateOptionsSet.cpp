// RunClimateOptionsSet.h : Implementation of the CRunClimateOptionsSet class


 
// CRunClimateOptionsSet implementation

// code generated on Wednesday, September 28, 2005, 2:11 PM

#include "stdafx.h"
#include "RunClimateOptionsSet.h"
IMPLEMENT_DYNAMIC(CRunClimateOptionsSet, CRecordset)

CRunClimateOptionsSet::CRunClimateOptionsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_runID = 0;
	m_Variable_Name = L"";
	m_ShortName = L"";
	m_VarID = 0;
	m_BinSize = 0;
	m_OptionType = 0;
	m_Stats_Table = FALSE;
	m_Stats_Graph = FALSE;
	m_Daily_Freqs = FALSE;
	m_Period_Mins = FALSE;
	m_Period_Maxs = FALSE;
	m_Data_Count = FALSE;
	m_CriticalPercentile = 0;
	m_cp2 = 0;
	m_Daily_List = 0;
	m_FilterValue = -999.0;
	m_nFields = 16;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CRunClimateOptionsSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CRunClimateOptionsSet::GetDefaultSQL()
{
	return _T("[ffpRunClimateOptions]");
}

void CRunClimateOptionsSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[runID]"), m_runID);
	RFX_Text(pFX, _T("[Variable Name]"), m_Variable_Name);
	RFX_Text(pFX, _T("[ShortName]"), m_ShortName);
	RFX_Byte(pFX, _T("[VarID]"), m_VarID);
	RFX_Long (pFX, _T("[BinSize]"), m_BinSize);
	RFX_Long (pFX, _T("[OptionType]"), m_OptionType);
	RFX_Bool(pFX, _T("[Stats Table]"), m_Stats_Table);
	RFX_Bool(pFX, _T("[Stats Graph]"), m_Stats_Graph);
	RFX_Bool(pFX, _T("[Daily Freqs]"), m_Daily_Freqs);
	RFX_Bool(pFX, _T("[Period Mins]"), m_Period_Mins);
	RFX_Bool(pFX, _T("[Period Maxs]"), m_Period_Maxs);
	RFX_Bool(pFX, _T("[Data Count]"), m_Data_Count);
	RFX_Long (pFX, _T("[CriticalPercentile]"), m_CriticalPercentile);
	RFX_Long (pFX, _T("[CriticalPercentile2]"), m_cp2);
	RFX_Long(pFX, _T("[Daily list]"), m_Daily_List);
	RFX_Double(pFX, _T("[FilterValue]"), m_FilterValue);
}
/////////////////////////////////////////////////////////////////////////////
// CRunClimateOptionsSet diagnostics

#ifdef _DEBUG
void CRunClimateOptionsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CRunClimateOptionsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



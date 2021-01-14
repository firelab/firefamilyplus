// SeasonBinSet.h : Implementation of the CSeasonBinSet class

 

// CSeasonBinSet implementation

// code generated on Wednesday, September 28, 2005, 2:19 PM

#include "stdafx.h"
#include "SeasonBinSet.h"
IMPLEMENT_DYNAMIC(CSeasonBinSet, CRecordset)

CSeasonBinSet::CSeasonBinSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_VarID = 0;
	m_Bin1Max = 0.0;
	m_Bin2Max = 0.0;
	m_Bin3Max = 0.0;
	m_Bin4Max = 0.0;
	m_Bin5Max = 0.0;
	m_Bin6Max = 0.0;
	m_Bin1Min = 0.0;
	m_Bin2Min = 0.0;
	m_Bin3Min = 0.0;
	m_Bin4Min = 0.0;
	m_Bin5Min = 0.0;
	m_Bin6Min = 0.0;
	m_nFields = 13;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CSeasonBinSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CSeasonBinSet::GetDefaultSQL()
{
	return _T("[ffpSeasonBins]");
}

void CSeasonBinSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Byte(pFX, _T("[VarID]"), m_VarID);
	RFX_Double(pFX, _T("[Bin1Max]"), m_Bin1Max);
	RFX_Double(pFX, _T("[Bin2Max]"), m_Bin2Max);
	RFX_Double(pFX, _T("[Bin3Max]"), m_Bin3Max);
	RFX_Double(pFX, _T("[Bin4Max]"), m_Bin4Max);
	RFX_Double(pFX, _T("[Bin5Max]"), m_Bin5Max);
	RFX_Double(pFX, _T("[Bin6Max]"), m_Bin6Max);
	RFX_Double(pFX, _T("[Bin1Min]"), m_Bin1Min);
	RFX_Double(pFX, _T("[Bin2Min]"), m_Bin2Min);
	RFX_Double(pFX, _T("[Bin3Min]"), m_Bin3Min);
	RFX_Double(pFX, _T("[Bin4Min]"), m_Bin4Min);
	RFX_Double(pFX, _T("[Bin5Min]"), m_Bin5Min);
	RFX_Double(pFX, _T("[Bin6Min]"), m_Bin6Min);

}
/////////////////////////////////////////////////////////////////////////////
// CSeasonBinSet diagnostics

#ifdef _DEBUG
void CSeasonBinSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CSeasonBinSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

int CSeasonBinSet::GetBinIndex(double val)
{
	if(val >= m_Bin1Min && val < m_Bin1Max)
		return 0;
	if(val >= m_Bin2Min && val < m_Bin2Max)
		return 1;
	if(val >= m_Bin3Min && val < m_Bin3Max)
		return 2;
	if(val >= m_Bin4Min && val < m_Bin4Max)
		return 3;
	if(val >= m_Bin5Min && val < m_Bin5Max)
		return 4;
	if(val >= m_Bin6Min && val < m_Bin6Max)
		return 5;
	//not in a bin
	return -1;
}


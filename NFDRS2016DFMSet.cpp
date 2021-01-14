#include "stdafx.h"
#include "NFDRS2016DFMSet.h"


IMPLEMENT_DYNAMIC(CNFDRS2016DFMSet, CRecordset)

CNFDRS2016DFMSet::CNFDRS2016DFMSet(CDatabase* pDatabase)
	: CRecordset(pDatabase)
{
	m_StationID = L"";
	m_ObsDate;
	m_FM1 = -1.0;
	m_FM10 = -1.0;
	m_FM100 = -1.0;
	m_FM1000 = -1.0;
	m_FuelTemperature = -1.0;
	m_nFields = 7;
	m_nDefaultType = dynaset;
}


CString CNFDRS2016DFMSet::GetDefaultConnect()
{
	return _T("");
}

CString CNFDRS2016DFMSet::GetDefaultSQL()
{
	return _T("[ffpNFDRS2016DFM]");
}

void CNFDRS2016DFMSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	// Macros such as RFX_Text() and RFX_Long () are dependent on the
	// type of the member variable, not the type of the field in the database.
	// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[StationID]"), m_StationID);
	RFX_Date(pFX, _T("[ObsDate]"), m_ObsDate);
	RFX_Double(pFX, _T("[FM_1]"), m_FM1);
	RFX_Double(pFX, _T("[FM_10]"), m_FM10);
	RFX_Double(pFX, _T("[FM_100]"), m_FM100);
	RFX_Double(pFX, _T("[FM_1000]"), m_FM1000);
	RFX_Double(pFX, _T("[FuelTemperature]"), m_FuelTemperature);

}

// CNFDRS2016DFMSet diagnostics

#ifdef _DEBUG
void CNFDRS2016DFMSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CNFDRS2016DFMSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

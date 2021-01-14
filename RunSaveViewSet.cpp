// RunSaveViewSet.h : Implementation of the CRunsSet class

 

// CRunSaveViewSet implementation

// code generated on Wednesday, September 28, 2005, 2:17 PM

#include "stdafx.h"
#include "RunSaveViewSet.h"
//IMPLEMENT_DYNAMIC(CRunSaveViewSet, CRecordset)

CRunSaveViewSet::CRunSaveViewSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_runID2 = 0;
	m_CreatedOn = L"";
	m_LastRun = L"";
	m_Description = L"";
	m_BatchInc = 0;
	m_nFields = 18;
	m_nDefaultType = dynaset;
    // from ffpRunDescription:
	m_runID = 0;
	m_Description2 = L"";
	m_SIG_Station = L"";
	m_StartYear = 0;
	m_EndYear = 0;
	m_StartMonth = L"";
	m_EndMonth = L"";
	m_StartDay = 0;
	m_EndDay = 0;
	m_PeriodLength = L"";
	m_Use78 = false;
	m_Use88 = false;
	m_UseCanadian = false;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CRunSaveViewSet::GetDefaultConnect()
{
	return _T("");
}

CString CRunSaveViewSet::GetDefaultSQL()
{
	return _T("[ffpRuns],[ffpRunDescription]");
}

void CRunSaveViewSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[ffpRuns.runID]"), m_runID);
	RFX_Text(pFX, _T("[ffpRuns.CreatedOn]"), m_CreatedOn);
	RFX_Text(pFX, _T("[ffpRuns.LastRun]"), m_LastRun);
	RFX_Text(pFX, _T("[ffpRuns.Description]"), m_Description);
	RFX_Long (pFX, _T("[ffpRuns.BatchInc]"), m_BatchInc);
    
	// from ffpRunDescription:
    RFX_Long (pFX, _T("[ffpRunDescription.runID]"), m_runID2);
	RFX_Text(pFX, _T("[ffpRunDesciption.Description]"), m_Description2);
	RFX_Text(pFX, _T("[ffpRunDesciption.SIG_Station]"), m_SIG_Station);

	RFX_Long (pFX, _T("[ffpRunDescription.StartYear]"), m_StartYear);
    RFX_Long (pFX, _T("[ffpRunDescription.EndYear]"), m_EndYear);

	RFX_Text(pFX, _T("[ffpRunDesciption.StartMonth]"), m_StartMonth);
	RFX_Text(pFX, _T("[ffpRunDesciption.EndMonth]"), m_EndMonth);

	
	
    RFX_Long (pFX, _T("[ffpRunDescription.StartDay]"), m_StartDay);
    RFX_Long (pFX, _T("[ffpRunDescription.EndDay]"), m_EndDay);
	RFX_Text(pFX, _T("[ffpRunDesciption.PeriodLength]"), m_PeriodLength);

	
	
	RFX_Int (pFX, _T("[ffpRunDescription.Use78]"), m_Use78);
	RFX_Int (pFX, _T("[ffpRunDescription.Use88]"), m_Use88);
	RFX_Int (pFX, _T("[ffpRunDescription.UseCanadian]"), m_UseCanadian);
	
}
/////////////////////////////////////////////////////////////////////////////
// CRunSaveViewSet diagnostics

#ifdef _DEBUG
void CRunSaveViewSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CRunSaveViewSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



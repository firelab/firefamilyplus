// RunSaveViewSet.h : Declaration of the CRunSaveViewSet

#pragma once

// code generated on Wednesday, September 28, 2005, 2:17 PM

class CRunSaveViewSet : public CRecordset
{
public:
	CRunSaveViewSet(CDatabase* pDatabase = NULL);
	//DECLARE_DYNAMIC(CRunSaveViewSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_runID;
	CString	m_CreatedOn;
	CString	m_LastRun;
	CString	m_Description;
	
	long	m_BatchInc;

	// from ffpRunDescription:
	long	m_runID2;
	CString	m_Description2;
	CString	m_SIG_Station;
	long	m_StartYear;
	long	m_EndYear;
	CString	m_StartMonth;
	CString	m_EndMonth;
	long	m_StartDay;
	long	m_EndDay;
	CString	m_PeriodLength;
	BOOL	m_Use78;
	BOOL	m_Use88;
	BOOL	m_UseCanadian;

// Overrides
	// Wizard generated virtual function overrides
	public:
	virtual CString GetDefaultConnect();	// Default connection string

	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};



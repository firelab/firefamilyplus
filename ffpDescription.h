// ffpDescription.h : Declaration of the CffpDescription

#pragma once

// code generated on Monday, September 26, 2005, 12:35 PM

class CffpDescription : public CRecordset
{
public:
	CffpDescription(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CffpDescription)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_Description;	//Description of data base contents
	CString	m_SIGStation;	//Active SIG or Station
	long	m_StartYear;	//Start Year of active working set
	long	m_EndYear;	//End Year of active working set
	CString	m_StartMonth;	//Start Month of working set
	CString	m_EndMonth;	//End month of working set
	long	m_StartDay;	//Start Day of month of working set
	long	m_EndDay;	//End day of month of working set
	CString	m_PeriodLength;	//Analysis period length
	BOOL	m_Use78;	//<Not Used>
	BOOL	m_Use88;	//<Not Used>
	BOOL	m_UseCanadian;	//Use the Canadian Fire Danger Model

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



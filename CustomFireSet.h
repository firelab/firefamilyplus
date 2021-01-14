// CustomFireSet.h : Declaration of the CCustomFireSet

#pragma once

// code generated on Wednesday, September 28, 2005, 1:52 PM

class CCustomFireSet : public CRecordset
{
public:
	CCustomFireSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CCustomFireSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_AgencyID;
	long	m_FRegion;
	long	m_LRegion;
	long	m_FUnit;
	long	m_LUnit;
	long	m_FSubunit;
	long	m_LSubunit;
	BOOL	m_UseRegion;
	CString	m_Region;
	BOOL	m_UseUnit;
	CString	m_Unit;
	BOOL	m_UseSubunit;
	CString	m_Subunit;
	long	m_Year;
	long	m_Month;
	long	m_Day;
	long	m_FFireNum;
	long	m_LFireNum;
	long	m_FAcres;
	long	m_LAcres;
	long	m_Cause;
	long	m_Duplicates;
	long	m_FName;
	long	m_LName;

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



// StateSet.h : Declaration of the CStateSet

#pragma once

// code generated on Wednesday, September 28, 2005, 2:22 PM

class CStateSet : public CRecordset
{
public:
	CStateSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CStateSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_State;
	CString	m_StateName;
	long	m_StateCode;
	long	m_AFFState;
	long	m_MinElev;
	long	m_MaxElev;
	long	m_LatDDN;
	long	m_LatMMN;
	long	m_LatSSN;
	long	m_LatDDS;
	long	m_LatMMS;
	long	m_LatSSS;
	long	m_LonDDE;
	long	m_LonMME;
	long	m_LonSSE;
	long	m_LonDDW;
	long	m_LonMMW;
	long	m_LonSSW;
	CString	m_PLSS;

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



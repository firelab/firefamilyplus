// FireOptionsSet.h : Declaration of the CFireOptionsSet

#pragma once

// code generated on Wednesday, September 28, 2005, 1:58 PM

class CFireOptionsSet : public CRecordset
{
public:
	CFireOptionsSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CFireOptionsSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_SIG_Station;
	BOOL	m_Lightning;
	BOOL	m_Human;
	BOOL	m_All;
	BOOL	m_Cumulative;
	BOOL	m_Probability;
	BOOL	m_Both;
	long	m_LFAcres;
	long	m_MFDFires;
	BYTE	m_VarID;
	BOOL    m_Conditional;
	long	m_CauseFlag;  // new 2014
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



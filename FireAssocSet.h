// FireAssocSet.h : Declaration of the CFireAssocSet

#pragma once

// code generated on Wednesday, September 28, 2005, 1:55 PM

class CFireAssocSet : public CRecordset
{
public:
	CFireAssocSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CFireAssocSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_SIG_Station;
	long	m_AgencyID;
	long	m_RegionID;
	long	m_UnitID;
	long	m_SubunitID;

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



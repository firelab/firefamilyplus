// FireSubunitSet.h : Declaration of the CFireSubunitSet

#pragma once

// code generated on Wednesday, September 28, 2005, 2:03 PM

class CFireSubunitSet : public CRecordset
{
public:
	CFireSubunitSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CFireSubunitSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_SubunitID;	//Internal Key
	long	m_UnitID;	//Link to FireUnits table
	CString	m_Name;	//Name of subunit

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



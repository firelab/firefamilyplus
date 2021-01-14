// FFPMergesSet.h : Declaration of the CFFPHourlyEventsSet

#pragma once


class CFFPHourlyEventsSet : public CRecordset
{
public:
	CFFPHourlyEventsSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CFFPHourlyEventsSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_SIG_Station;
	long m_Row;
	long m_Category;
	long m_RowOp;
	long m_ValOp;
	long m_ValType;
	double m_Value;
	long m_VarID;
	CString	m_VarString;
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



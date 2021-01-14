// StationInSIGSet.h : Declaration of the CStationInSIGSet

#pragma once

// code generated on Wednesday, September 28, 2005, 2:23 PM

class CStationInSIGSet : public CRecordset
{
public:
	CStationInSIGSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CStationInSIGSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_SIG;	//SIG identifier
	CString	m_StationID;	//Station Identifier
	float	m_NFDRSFMPriority;	//Fuel Model Priority
	double	m_WeightFactor;	//Weighting Factor for Station (Weights are relative)

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



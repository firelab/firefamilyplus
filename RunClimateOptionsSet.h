// RunClimateOptionsSet.h : Declaration of the CRunClimateOptionsSet

#pragma once

// code generated on Wednesday, September 28, 2005, 2:11 PM

class CRunClimateOptionsSet : public CRecordset
{
public:
	CRunClimateOptionsSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CRunClimateOptionsSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_runID;
	CString	m_Variable_Name;
	CString	m_ShortName;
	BYTE	m_VarID;
	long	m_BinSize;
	long	m_OptionType;
	BOOL	m_Stats_Table;
	BOOL	m_Stats_Graph;
	BOOL	m_Daily_Freqs;
	BOOL	m_Period_Mins;
	BOOL	m_Period_Maxs;
	BOOL	m_Data_Count;
	long	m_CriticalPercentile;
	long	m_cp2;
	long	m_Daily_List;
	double	m_FilterValue;  // key value to ignore records greater than or lass than (depending on critical percentile 1 and 2 orientaion)

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



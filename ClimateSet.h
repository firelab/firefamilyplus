// ClimateSet.h : Declaration of the CClimateSet

#pragma once

// code generated on Wednesday, September 28, 2005, 1:23 PM

class CClimateSet : public CRecordset
{
public:
	CClimateSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CClimateSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_Variable_Name;	//Name of Index or Weather Variable
	CString	m_ShortName;
	BYTE	m_VarID;	//ID of variable
	float	m_BinSize;	//Bin Size for Grouping
	long	m_OptionType;	//0 = Weather Observation 1 = NFDRS Index 2 = Categorical 3 = Canadian
	BOOL	m_Stats_Table;	//Generate a statistics table
	BOOL	m_Stats_Graph;	//Generate a statistics graph
	BOOL	m_Daily_Freqs;	//Generate a Daily Frequencies table
	BOOL	m_Period_Mins;	//Generate a Period Minimums table
	BOOL	m_Period_Maxs;	//Generate a Period Maximums table
	BOOL	m_Data_Count;	//Generate a Data Count report
	long	m_CriticalPercentile;	//First critical percentile
	long	m_cp2;	//Second critical percentile
	long	m_Daily_List;	//Sequence in Daily List report, 0 if not included
	double	m_FilterValue;  // key value to ignore records greater than or lass than (depending on critical percentile 1 and 2 orientaion)

	void setVarSortOrder(int _varSortOrder);
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



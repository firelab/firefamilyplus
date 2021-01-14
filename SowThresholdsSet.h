#pragma once

// code generated on Wednesday, September 28, 2005, 2:17 PM

class CSowThresholdsSet : public CRecordset
{
public:
	CSowThresholdsSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CSowThresholdsSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString m_SIG_Station;
	long m_PCNT_Clear;
	long m_PCNT_Scattered;
	long m_PCNT_Broken;
	double	m_1HR_Drizzle;
	double	m_1HR_Rain;
	double	m_1HR_Showers;
	long m_3HR_DUR_WetFlag;
	double m_3HR_AMT_WetFlag;
	long m_24HR_DUR_WetFlag;
	double m_24HR_AMT_WetFlag;

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

	void AddDefaultForStation(CString staID);
};



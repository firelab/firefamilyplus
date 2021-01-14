// SeasonBinSet.h : Declaration of the CSeasonBinSet

#pragma once

// code generated on Wednesday, September 28, 2005, 2:19 PM

class CSeasonBinSet : public CRecordset
{
public:
	CSeasonBinSet(CDatabase* pDatabase = NULL);
	int GetBinIndex(double val);
	DECLARE_DYNAMIC(CSeasonBinSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	BYTE	m_VarID;
	double	m_Bin1Max;
	double	m_Bin2Max;
	double	m_Bin3Max;
	double	m_Bin4Max;
	double	m_Bin5Max;
	double	m_Bin6Max;
	double	m_Bin1Min;
	double	m_Bin2Min;
	double	m_Bin3Min;
	double	m_Bin4Min;
	double	m_Bin5Min;
	double	m_Bin6Min;

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



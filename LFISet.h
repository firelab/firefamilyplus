// RunsSet.h : Declaration of the CRunsSet

#pragma once

// code generated on Wednesday, September 28, 2005, 2:17 PM

class CLFISet : public CRecordset
{
public:
	CLFISet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CLFISet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString m_SIG_Station;
	long	m_LFIdaysAvg;
	double	m_TMinMin;
	double	m_TMinMax;
	double	m_VPDMin;
	double	m_VPDMax;
	double	m_DaylenMin;
	double	m_DaylenMax;
	long m_PcpDays;
	BOOL m_UseVPDAvg;
	double m_PcpMin;
	double m_PcpMax;
	BOOL m_UseRTPrecip;
	long m_HerbDaysAvg;
	double m_HerbMaxGSI;
	double m_HerbGreenup;
	double m_HerbMax;
	double m_HerbMin;
	double	m_HerbTMinMin;
	double	m_HerbTMinMax;
	double	m_HerbVPDMin;
	double	m_HerbVPDMax;
	double	m_HerbDaylenMin;
	double	m_HerbDaylenMax;
	long m_HerbPcpDays;
	BOOL m_HerbUseVPDAvg;
	double m_HerbPcpMin;
	double m_HerbPcpMax;
	BOOL m_HerbUseRTPrecip;
	long m_WoodyDaysAvg;
	double m_WoodyMaxGSI;
	double m_WoodyGreenup;
	double m_WoodyMax;
	double m_WoodyMin;
	double	m_WoodyTMinMin;
	double	m_WoodyTMinMax;
	double	m_WoodyVPDMin;
	double	m_WoodyVPDMax;
	double	m_WoodyDaylenMin;
	double	m_WoodyDaylenMax;
	long m_WoodyPcpDays;
	BOOL m_WoodyUseVPDAvg;
	double m_WoodyPcpMin;
	double m_WoodyPcpMax;
	BOOL m_WoodyUseRTPrecip;

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



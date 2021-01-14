// RunWxStationSet.h : Declaration of the CRunWxStationSet

#pragma once

// code generated on Wednesday, September 28, 2005, 2:18 PM

class CRunWxStationSet : public CRecordset
{
public:
	CRunWxStationSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CRunWxStationSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_runID;
	CString	m_StationID;
	CString	m_Name;
	CString	m_NFDRSFM;
	long	m_Use88;
	double	m_LatDegrees;
	double	m_LonDegrees;
	long	m_Elevation;
	long	m_ClimateCls;
	long	m_SlopeCls;
	long	m_HerbAnnual;
	COleDateTime	m_GreenJulian;
	COleDateTime	m_FreezeJulian;
	long	m_StartGreenHerb;
	long	m_StartGreenShrub;
	long	m_StartKBDI;
	long	m_Deciduous;
	float	m_AvgPrecip;
	float	m_Start1000;
	long	m_FM1Eq10;
	CString	m_Aspect;
	CString	m_PsnOnSlope;
	long	m_StnType;
	long	m_Site;
	CString	m_State;
	long	m_County;
	long	m_ObsAgy;
	CString	m_ObsUnit;
	long	m_FSRegion;
	CString m_WRCC_ID;

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



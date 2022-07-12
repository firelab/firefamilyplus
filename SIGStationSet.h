// SIGStationSet.h : Declaration of the CSIGStationSet

#pragma once

// code generated on Wednesday, September 28, 2005, 2:21 PM

class CSIGStationSet : public CRecordset
{
public:
	CSIGStationSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CSIGStationSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_StationID;	//Station ID
	CString	m_Name;	//Name of Station
	CString	m_NFDRSFM;	//NFDRS Fuel Model
	long	m_Use88;	//Use 88 model? (78 is default)
	double	m_LatDegrees;	//Latitude of station
	double	m_LonDegrees;	//Longitude of station
	long	m_Elevation;	//Elevation of station
	long	m_ClimateCls;	//Climate Class
	long	m_SlopeCls;	//Slope Class
	long	m_HerbAnnual;	//Annual or Perennial
	COleDateTime	m_GreenJulian;	//Greenup Day of Year
	COleDateTime	m_FreezeJulian;	//First freeze Day of Year
	long	m_StartGreenHerb;	//Herbacious moisture content
	long	m_StartGreenShrub;	//Shrub moisture content
	long	m_StartKBDI;	//Initial KBDI
	long	m_Deciduous;	//Deciduous Flag
	float	m_AvgPrecip;	//Average precipitation
	float	m_Start1000;	//Initial 1000 hr fuel moisture
	long	m_FM1Eq10;	//1 equals ten flag
	CString	m_Aspect;	//Aspect of station
	CString	m_PsnOnSlope;	//Position of station on slope
	long	m_StnType;	//Wx Station Type
	long	m_Site;
	CString	m_State;
	long	m_County;
	long	m_ObsAgy;
	CString	m_ObsUnit;
	long	m_FSRegion;
	CString m_WRCC_ID;
	COleDateTime m_DormantJulian;	//Greenup Day of Year
	long m_UseDormant;
	long m_UseStick;
	long m_RegSchdObs; //hour of regularly scheduled Obs (for type O)
	CString m_NESDIS_ID;

	//NFDR2016 additions
	//Max SC for each NFDR2016 fuel model
	long m_SCM_V;
	long m_SCM_W;
	long m_SCM_X;
	long m_SCM_Y;
	long m_SCM_Z;
	//Humid Moisture of Extinction for each NFDR2016 fuel model (boolean flag)
	BOOL m_MX_Humid_V;
	BOOL m_MX_Humid_W;
	BOOL m_MX_Humid_X;
	BOOL m_MX_Humid_Y;
	BOOL m_MX_Humid_Z;
	int m_timeZoneOffset;//offset from UTC
	int m_KBDIThreshold;
	int m_MXD_Override;
	// Overrides
	// Wizard generated virtual function overrides
	public:
	virtual CString GetDefaultConnect();	// Default connection string

	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support

	//Helpers for NFDR2016 specific fields
	BOOL GetMxHumid(char fm);
	long GetSCM(char fm);

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};



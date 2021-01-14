#pragma once


class CNFDRS2016WxSet : public CRecordset
{
public:
	CNFDRS2016WxSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CNFDRS2016WxSet)

	CString BuildStationFilter(CString stationID);
	CString	m_StationID;	//Station ID
	COleDateTime m_ObsDate;

	//WxObs fields
	long	m_Temp;	//Dry Bulb Temperature
	long	m_RH;	//Relative Humidity
	long	m_WDir;	//Wind Direction
	long	m_WS;	//Wind Speed
	long	m_TmpMax;	//Max Temperature
	long	m_TmpMin;	//Min Temperature
	long	m_RHMax;	//Max Relative Humidity
	long	m_RHMin;	//Min Relative Humidity
	float	m_PPTAMT;	//Precipitation Amount
	//extras not for calcs but in case user wanted them..
	long	m_PPTDUR;	//Precipitation Duration
	long m_GustDir;         // wind gust direction (degrees)
	long m_GustSpd;            // wind gust speed (mph)
	float m_HourlyPrecip;     // actual hourly precip values (inches)
	long m_SolarRadiation; // watts/m2
	long m_WetFlag;         // Y=1/N=0
	long m_SnowFlag;
	long	m_WAzimuth;	


	//ffpNFDRS2016DFM fields
	double m_FM1;
	double m_FM10;
	double m_FM100;
	double m_FM1000;
	double m_FuelTemperature;

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
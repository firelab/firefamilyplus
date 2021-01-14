// WxSet.h : Declaration of the CWxSet

#pragma once
#include <afxtempl.h>
#include "resource.h"
#include <queue>

class CSowOptionsSet;

long GetMinPrecipObs(COleDateTime start, COleDateTime end);
//typedef CList<double,double> ValList;
class Stats
{
public:
	double PercentileFromValue(double value);
	double mode;
	void CalcMode();
	void CleanStorage();
	void Reset();
	double Percentile(double percent);
	Stats();
	~Stats();
	void Accumulate(double in);
	void Complete();
	//void Pass2Accumulate(double in);
	//void Pass2Complete();
	double Mean();
	double Median();
	long N();
	double StdDev();
	double Min();
	double Max();
	double *vals;//array of values, filled on 2nd pass
private:
	double mean;
	int n;
	double variance;
	double ep;//two-pass correction variable for variance
	int p2Loc;
	CList<double,double> valList;
};

class Period
{
public:
	double Percentile(double percent);
	double stdDev;
	double min;
	double max;
	double mean;
	long n;
	long N();
	void OutputPrecipStats(FILE *stream, double criticalPercentile);
	Period(const COleDateTime& _start, const COleDateTime& _end);
	~Period();
	void Accumulate(const COleDateTime& yearDate, double in);
	void Complete();
	//void Pass2Accumulate(const COleDateTime& yearDate, double in);
	//void Pass2Complete();
	void Outputstats(FILE *stream, double criticalPercentile, bool isGSI = false);
	void OutputVPDstats(FILE *stream, double criticalPercentile);
	COleDateTime start;
	COleDateTime end;
	int years;
	int baseYear;
	Stats *yStats;
};

// code generated on Wednesday, September 28, 2005, 1:25 PM

class CWxSet : public CRecordset
{
public:
	CWxSet(CDatabase* pDatabase = NULL);
	~CWxSet();
	DECLARE_DYNAMIC(CWxSet)
	CSowOptionsSet *m_pSowOpts;
	void CheckSowOpts();
// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_StationID;	//Station ID
	COleDateTime	m_ObsDate;	//Date/Time of Observation
	CString	m_ObsType;
	long	m_UserWX;	//<Not Used>
	long	m_SOW;	//State of Weather
	long	m_Temp;	//Dry Bulb Temperature
	long	m_RH;	//Relative Humidity
	long	m_WDir;	//Wind Direction
	long	m_WS;	//Wind Speed
	float	m_OMC10;	//Observed 10-hr fuel moisture
	float	m_OMCWood;	//Observed Woody Fuel Moisture
	COleDateTime	m_OMCWoodDate;	//OMCWood Observation Date
	//CTime	m_OMCWoodDate;	//OMCWood Observation Date
	long	m_TmpMax;	//Max Temperature
	long	m_TmpMin;	//Min Temperature
	long	m_RHMax;	//Max Relative Humidity
	long	m_RHMin;	//Min Relative Humidity
	long	m_PPTDUR;	//Precipitation Duration
	float	m_PPTAMT;	//Precipitation Amount
	long	m_Season;	//NFDRS 88 Season Code
	long	m_GreenHerb;	//Herbacious Greenness
	long	m_GreenShrub;	//Woody Greenness
	float	m_MC1;	//1-hr Moisture (Calculated) <Not Used>
	float	m_MC10;	//10-hr Moisture (Calculated) <Not Used>
	float	m_MC100;	//100-hr Moisture (Calculated) <Not Used>
	float	m_MC1000;	//1000-hr Moisture (Calculated) <Not Used>
	float	m_X1000;	//?????<Not Used>
	float	m_MCHerb;	//Herbacious Fuel Moisture (Calculated) <Not Used>
	float	m_MCWood;	//Woody Fuel Moisture (Calculated) <Not Used>
	long	m_GREN;	//?????<Not Used>
	long	m_HStage;	//?????<Not Used>
	float	m_ROS;	//Rate of Spread (Calculated) <Not Used>
	float	m_ERC;	//Energy Release Component (Calculated) <Not Used>
	long	m_BI;	//Burnng Index (Calculated) <Not Used>
	long	m_FIL;	//<Not Used>
	long	m_WAzimuth;	//?????<Not Used>
	long m_SolarRadiation; // watts/m2
	long m_WetFlag;         // Y=1/N=0
    long m_GustDir;         // wind gust direction (degrees)
	long m_GustSpd;            // wind gust speed (mph)
	float m_HourlyPrecip;     // actual hourly precip values (inches)
	long m_SR_SOW;
	long m_SR_WetFlag;
	long m_SnowFlag;
	long m_Hour;
	long m_DailyObs;

	//NFDRS2016 fields
	double m_FM1;
	double m_FM10;
	double m_FM100;
	double m_FM1000;
	double m_FuelTemperature;
	// Overrides
	// Wizard generated virtual function overrides
	public:
	virtual CString GetDefaultConnect();	// Default connection string

	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support

    void RecalcRunning24();
	//void CalcMinsMaxes();
	int CalcHourlyPrecip(COleDateTime *precipStart);
	int CalcHourlyPrecip(CString trgStation);
	int Calc24HourPrecip(CString trgStation);
	long CalcSOW();
	int ResolveSOW();
	int ResolveWetFlag();
	//void setDailyObs(int hour, CCancelDialog *cd = 0, int iStep = 0);
	void setDailyObs(int hour, int iStep = 0);
	void ensureMaxOneDailyObs(int regHour);
	//gets queue of previous 24 hourly precip values (for calculation of 24hour precip from hourly values
	std::queue<double> GetPrevious24HourlyPrecip(CString station, COleDateTime targetDate);
	//gets queue of previous 24 hourly 24hour precip values (for calculation of hourly precip from 24hour precip)
	std::queue<double> GetPrevious24DailyPrecip(CString station, COleDateTime targetDate);

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	bool GetVariable(int varID, double *val);
};

/*class WxSetOR : public CWxSet
{
public:
	WxSetOR(CDatabase* pDatabase = NULL);
	~WxSetOR();
	void GetNext();
};*/
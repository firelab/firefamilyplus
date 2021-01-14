#pragma once

// CLFIengine command target

const int HerbParams = 1;
const int WoodyParams = 2;

class CLFIengine : public CObject
{
public:
	CLFIengine(CDatabase *pDB, int herbWoodyFlag = 0, CString SIG_Station = "######", bool _herbAnnual = false); //herbWoody = HerbParams for HerbFM parameter loading, WoodyParams for WoodyFM parameter loading

	virtual ~CLFIengine();
	double CalcRunningAvgLFI();
	double CalcRunningAvgHerbFM();
	double CalcRunningAvgWoodyFM();
	//double CalcRunningAvgLFIHerb();
	//double CalcRunningAvgLFIWoody();
	void AddLFIobs(COleDateTime lfiDay, double val);
	bool IsUsingVPDMax();
	bool IsUsingVPDAvg();
	double CalcLFI(double minRH, double minTempF, double currT, double lat, int doy);
	double CalcLFI_VPDAvg(double RH, double TempF, double maxTempF, double minTempF, double lat, int doy);
	double GetTminInd(double Tmin);
	double GetVPDInd(double VPD);
	double GetDaylInd(double Dayl);
	double GetHerbMaxGSI();
	double GetHerbGreenup();
	double GetWoodyMaxGSI();
	double GetWoodyGreenup();

	double GetTminMin();
	double GetTminMax();
	double GetVPDMin();
	double GetVPDMax();
	double GetDaylenMin();
	double GetDaylenMax();
	int GetMAPeriod();
	int GetNumPrecipDays();
	double GetRTPcpMin();
	double GetRTPcpMax();
private:
	CDatabase *m_pDB;
	int m_LFIdaysAvg;
	double m_TminMin;
	double m_TminMax;
	double m_VPDMin;
	double m_VPDMax;
	double m_DaylenMin;
	double m_DaylenMax;
	double *m_LFIarray;
	BOOL m_UseVPDAvg;
	COleDateTime lastLFIday;
	//added for live fuel moistues
	double m_HerbMaxGSI;
	double m_HerbGreenup;
	double m_HerbMax;
	double m_HerbMin;
	double m_WoodyMaxGSI;
	double m_WoodyGreenup;
	double m_WoodyMax;
	double m_WoodyMin;
	double m_HerbSlope;
	double m_HerbIntercept;
	double m_WoodySlope;
	double m_WoodyIntercept;
	bool herbAnnual;
	bool hasGreenedUpThisYear;
	bool hasExceeded120ThisYear;
	bool canIncreaseHerb;
	double lastHerbFM;
	int m_PcpDays;
	double m_PrcpMin;
	double m_PrcpMax;
	//double m_HerbPrcpMin;
	//double m_HerbPrcpMax;
	//double m_WoodyPrcpMin;
	//double m_WoodyPrcpMax;
};



// LFIengine.cpp : implementation file
//
 
#include "stdafx.h"
#include "fireplus.h"
#include "LFIengine.h"
#include "LFIset.h"
#include "wxSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "FireSumSet.h"
#include "SIGStationSet.h"
#include "ClimateSet.h"
#include "ClimAnalysis.h"


// CLFIengine

CLFIengine::CLFIengine(CDatabase *pDB, int herbWoodyFlag, CString SIG_Station, bool _herbAnnual)
{
	m_pDB = pDB;
	m_LFIdaysAvg = 21;
	m_PcpDays = 30;
	m_TminMin = -2.0;
	m_TminMax = 5.0;
	m_VPDMin = 900.0;
	m_VPDMax = 4100.0;
	m_DaylenMin = 36000;
	m_DaylenMax = 39600;
	m_HerbMaxGSI = 1.0;
	m_HerbGreenup = 0.5;
	m_HerbMax = 250.0;
	m_HerbMin = 30;
	m_WoodyMaxGSI = 1.0;
	m_WoodyGreenup = 0.5;
	m_WoodyMax = 200.0;
	m_WoodyMin = 50.0;
	m_HerbSlope = 1.0;
	m_HerbIntercept = 1.0;
	m_WoodySlope = 1.0;
	m_WoodyIntercept = 1.0;
	m_PrcpMin = 0.5;
	m_PrcpMax = 1.5;
	CLFISet lfiSet(pDB);
	if (SIG_Station.GetLength() > 6)
		lfiSet.m_strFilter.Format("[SIG_Station] = '%s'", SIG_Station);
	else
		lfiSet.m_strFilter.Format("[SIG_Station] = '%6.6s'", SIG_Station);
	lfiSet.Open();
	if(lfiSet.IsEOF())
	{
		CLFISet defSet(pDB);
		defSet.m_strFilter.Format("[SIG_Station] = '######'");
		defSet.Open();
		if(!defSet.IsEOF())
		{
			lfiSet.AddNew();
			lfiSet.m_SIG_Station = SIG_Station.Left(6);
			lfiSet.m_LFIdaysAvg = defSet.m_LFIdaysAvg;
			lfiSet.m_UseVPDAvg = defSet.m_UseVPDAvg;
			lfiSet.m_TMinMin = defSet.m_TMinMin;
			lfiSet.m_TMinMax = defSet.m_TMinMax;
			lfiSet.m_VPDMin = defSet.m_VPDMin;
			lfiSet.m_VPDMax = defSet.m_VPDMax;
			lfiSet.m_DaylenMin = defSet.m_DaylenMin;
			lfiSet.m_DaylenMax = defSet.m_DaylenMax;
			lfiSet.m_PcpDays = defSet.m_PcpDays;
			lfiSet.m_HerbDaysAvg = defSet.m_HerbDaysAvg;
			lfiSet.m_HerbUseVPDAvg = defSet.m_HerbUseVPDAvg;
			lfiSet.m_HerbTMinMin = defSet.m_HerbTMinMin;
			lfiSet.m_HerbTMinMax = defSet.m_HerbTMinMax;
			lfiSet.m_HerbVPDMin = defSet.m_HerbVPDMin;
			lfiSet.m_HerbVPDMax = defSet.m_HerbVPDMax;
			lfiSet.m_HerbDaylenMin = defSet.m_HerbDaylenMin;
			lfiSet.m_HerbDaylenMax = defSet.m_HerbDaylenMax;
			lfiSet.m_HerbPcpDays = defSet.m_HerbPcpDays;
			lfiSet.m_WoodyDaysAvg = defSet.m_WoodyDaysAvg;
			lfiSet.m_WoodyUseVPDAvg = defSet.m_WoodyUseVPDAvg;
			lfiSet.m_WoodyTMinMin = defSet.m_WoodyTMinMin;
			lfiSet.m_WoodyTMinMax = defSet.m_WoodyTMinMax;
			lfiSet.m_WoodyVPDMin = defSet.m_WoodyVPDMin;
			lfiSet.m_WoodyVPDMax = defSet.m_WoodyVPDMax;
			lfiSet.m_WoodyDaylenMin = defSet.m_WoodyDaylenMin;
			lfiSet.m_WoodyDaylenMax = defSet.m_WoodyDaylenMax;
			lfiSet.m_HerbMaxGSI = defSet.m_HerbMaxGSI;
			lfiSet.m_HerbGreenup = defSet.m_HerbGreenup;
			lfiSet.m_HerbMax = defSet.m_HerbMax;
			lfiSet.m_HerbMin = defSet.m_HerbMin;
			lfiSet.m_WoodyMaxGSI = defSet.m_WoodyMaxGSI;
			lfiSet.m_WoodyGreenup = defSet.m_WoodyGreenup;
			lfiSet.m_WoodyMax = defSet.m_WoodyMax;
			lfiSet.m_WoodyMin = defSet.m_WoodyMin;
			lfiSet.m_WoodyPcpDays = defSet.m_WoodyPcpDays;
			lfiSet.m_PcpMin = defSet.m_PcpMin;
			lfiSet.m_PcpMax = defSet.m_PcpMax;
			lfiSet.m_HerbPcpMin = defSet.m_HerbPcpMin;
			lfiSet.m_HerbPcpMax = defSet.m_HerbPcpMax;
			lfiSet.m_WoodyPcpMin = defSet.m_WoodyPcpMin;
			lfiSet.m_WoodyPcpMax = defSet.m_WoodyPcpMax;
			lfiSet.Update();
		}
		defSet.Close();
		lfiSet.Requery();

	}
	if(!lfiSet.IsEOF())
	{
		if(herbWoodyFlag == 0)
		{
			if(!lfiSet.IsFieldNull(&lfiSet.m_LFIdaysAvg))
				m_LFIdaysAvg = lfiSet.m_LFIdaysAvg;
			if(!lfiSet.IsFieldNull(&lfiSet.m_UseVPDAvg))
				m_UseVPDAvg = lfiSet.m_UseVPDAvg;
			if(!lfiSet.IsFieldNull(&lfiSet.m_TMinMin))
				m_TminMin = lfiSet.m_TMinMin;
			if(!lfiSet.IsFieldNull(&lfiSet.m_TMinMax))
				m_TminMax = lfiSet.m_TMinMax;
			if(!lfiSet.IsFieldNull(&lfiSet.m_VPDMin))
				m_VPDMin = lfiSet.m_VPDMin;
			if(!lfiSet.IsFieldNull(&lfiSet.m_VPDMax))
				m_VPDMax = lfiSet.m_VPDMax;
			if(!lfiSet.IsFieldNull(&lfiSet.m_DaylenMin))
				m_DaylenMin = lfiSet.m_DaylenMin;
			if(!lfiSet.IsFieldNull(&lfiSet.m_DaylenMax))
				m_DaylenMax = lfiSet.m_DaylenMax;
			if (!lfiSet.IsFieldNull(&lfiSet.m_PcpDays))
				m_PcpDays = lfiSet.m_PcpDays;
			if (!lfiSet.IsFieldNull(&lfiSet.m_PcpMin))
				m_PrcpMin = lfiSet.m_PcpMin;
			if (!lfiSet.IsFieldNull(&lfiSet.m_PcpMax))
				m_PrcpMin = lfiSet.m_PcpMax;
		}
		if(herbWoodyFlag == HerbParams)
		{
			if(!lfiSet.IsFieldNull(&lfiSet.m_HerbDaysAvg))
				m_LFIdaysAvg = lfiSet.m_HerbDaysAvg;
			if(!lfiSet.IsFieldNull(&lfiSet.m_HerbUseVPDAvg))
				m_UseVPDAvg = lfiSet.m_HerbUseVPDAvg;
			if(!lfiSet.IsFieldNull(&lfiSet.m_HerbTMinMin))
				m_TminMin = lfiSet.m_HerbTMinMin;
			if(!lfiSet.IsFieldNull(&lfiSet.m_HerbTMinMax))
				m_TminMax = lfiSet.m_HerbTMinMax;
			if(!lfiSet.IsFieldNull(&lfiSet.m_HerbVPDMin))
				m_VPDMin = lfiSet.m_HerbVPDMin;
			if(!lfiSet.IsFieldNull(&lfiSet.m_HerbVPDMax))
				m_VPDMax = lfiSet.m_HerbVPDMax;
			if(!lfiSet.IsFieldNull(&lfiSet.m_HerbDaylenMin))
				m_DaylenMin = lfiSet.m_HerbDaylenMin;
			if(!lfiSet.IsFieldNull(&lfiSet.m_HerbDaylenMax))
				m_DaylenMax = lfiSet.m_HerbDaylenMax;
			if (!lfiSet.IsFieldNull(&lfiSet.m_HerbPcpDays))
				m_PcpDays = lfiSet.m_HerbPcpDays;
			if (!lfiSet.IsFieldNull(&lfiSet.m_HerbPcpMin))
				m_PrcpMin = lfiSet.m_HerbPcpMin;
			if (!lfiSet.IsFieldNull(&lfiSet.m_HerbPcpMax))
				m_PrcpMin = lfiSet.m_HerbPcpMax;
		}
		if(herbWoodyFlag == WoodyParams)
		{
			if(!lfiSet.IsFieldNull(&lfiSet.m_WoodyDaysAvg))
				m_LFIdaysAvg = lfiSet.m_WoodyDaysAvg;
			if(!lfiSet.IsFieldNull(&lfiSet.m_WoodyUseVPDAvg))
				m_UseVPDAvg = lfiSet.m_WoodyUseVPDAvg;
			if(!lfiSet.IsFieldNull(&lfiSet.m_WoodyTMinMin))
				m_TminMin = lfiSet.m_WoodyTMinMin;
			if(!lfiSet.IsFieldNull(&lfiSet.m_WoodyTMinMax))
				m_TminMax = lfiSet.m_WoodyTMinMax;
			if(!lfiSet.IsFieldNull(&lfiSet.m_WoodyVPDMin))
				m_VPDMin = lfiSet.m_WoodyVPDMin;
			if(!lfiSet.IsFieldNull(&lfiSet.m_WoodyVPDMax))
				m_VPDMax = lfiSet.m_WoodyVPDMax;
			if(!lfiSet.IsFieldNull(&lfiSet.m_WoodyDaylenMin))
				m_DaylenMin = lfiSet.m_WoodyDaylenMin;
			if(!lfiSet.IsFieldNull(&lfiSet.m_WoodyDaylenMax))
				m_DaylenMax = lfiSet.m_WoodyDaylenMax;
			if (!lfiSet.IsFieldNull(&lfiSet.m_WoodyPcpDays))
				m_PcpDays = lfiSet.m_WoodyPcpDays;
			if (!lfiSet.IsFieldNull(&lfiSet.m_WoodyPcpMin))
				m_PrcpMin = lfiSet.m_WoodyPcpMin;
			if (!lfiSet.IsFieldNull(&lfiSet.m_WoodyPcpMax))
				m_PrcpMin = lfiSet.m_WoodyPcpMax;
		}
		if(!lfiSet.IsFieldNull(&lfiSet.m_HerbMaxGSI))
			m_HerbMaxGSI = lfiSet.m_HerbMaxGSI;
		if(!lfiSet.IsFieldNull(&lfiSet.m_HerbGreenup))
			m_HerbGreenup = lfiSet.m_HerbGreenup;
		if(!lfiSet.IsFieldNull(&lfiSet.m_HerbMax))
			m_HerbMax = lfiSet.m_HerbMax;
		if(!lfiSet.IsFieldNull(&lfiSet.m_HerbMin))
			m_HerbMin = lfiSet.m_HerbMin;
		if(!lfiSet.IsFieldNull(&lfiSet.m_WoodyMaxGSI))
			m_WoodyMaxGSI = lfiSet.m_WoodyMaxGSI;
		if(!lfiSet.IsFieldNull(&lfiSet.m_WoodyGreenup))
			m_WoodyGreenup = lfiSet.m_WoodyGreenup;
		if(!lfiSet.IsFieldNull(&lfiSet.m_WoodyMax))
			m_WoodyMax = lfiSet.m_WoodyMax;
		if(!lfiSet.IsFieldNull(&lfiSet.m_WoodyMin))
			m_WoodyMin = lfiSet.m_WoodyMin;
	}
	lfiSet.Close();
	m_LFIdaysAvg = max(1, m_LFIdaysAvg);
	m_LFIarray = new double[m_LFIdaysAvg];
	for(int i = 0; i < m_LFIdaysAvg; i++)
		m_LFIarray[i] = -1;
	lastLFIday.SetStatus(COleDateTime::invalid);

	//precalculate slopes and intercepts
	if(m_HerbGreenup == 1.0)
		m_HerbGreenup = 0.9999;
	if(m_WoodyGreenup == 1.0)
		m_WoodyGreenup = 0.9999;
	m_HerbSlope = (m_HerbMax - m_HerbMin) / (1.0 - m_HerbGreenup);
	m_HerbIntercept = m_HerbMax - m_HerbSlope;
	m_WoodySlope = (m_WoodyMax - m_WoodyMin) / (1.0 - m_WoodyGreenup);
	m_WoodyIntercept = m_WoodyMax - m_WoodySlope;
	herbAnnual = _herbAnnual;
	hasGreenedUpThisYear = false;
	canIncreaseHerb = false;
	hasExceeded120ThisYear = false;
	lastHerbFM = -1.0;

}






CLFIengine::~CLFIengine()
{
	if(m_LFIdaysAvg > 0 && m_LFIarray)
		delete[] m_LFIarray;
}


// CLFIengine member functions
double CLFIengine::CalcRunningAvgLFI()
{
	int numValid = 0;
	double lfi = 0.0;
	for(int i = 0; i < m_LFIdaysAvg; i++)
	{
		if(m_LFIarray[i] >= 0)
		{
			numValid++;
			lfi += m_LFIarray[i];
		}
	}
	if(numValid > 0)
		lfi /= numValid;
	return lfi;
}

double CLFIengine::CalcRunningAvgHerbFM()
{
	double GSI = CalcRunningAvgLFI() / 100.0;
	double rescale = GSI / m_HerbMaxGSI;
	double ret = m_HerbMin;
	rescale = min(1.0, rescale);
	rescale = max(0.0, rescale);
	if(rescale >= m_HerbGreenup)
	{
		ret = m_HerbSlope * rescale + m_HerbIntercept;
		if(!hasGreenedUpThisYear)
		{
			hasGreenedUpThisYear = true;
			canIncreaseHerb = true;
			//TRACE2("Greenup: %s, HerbFM = %.2f\n", lastLFIday.Format(), ret);
		}
	}
	if(!canIncreaseHerb && lastHerbFM >= 0)
		ret = min(ret, lastHerbFM);
	if(!hasExceeded120ThisYear && ret >= 120)
		hasExceeded120ThisYear = true;
	if(hasExceeded120ThisYear && ret < 120.0 && herbAnnual)
	{
		canIncreaseHerb = false;
		//TRACE2("HerbFM below 120.0 threshold: %s, HerbFM = %.2f\n", lastLFIday.Format(), ret);
	}
	lastHerbFM = ret;
	return ret;
}

double CLFIengine::CalcRunningAvgWoodyFM()
{
	double GSI = CalcRunningAvgLFI() / 100.0;
	double rescale = GSI / m_WoodyMaxGSI;
	rescale = min(1.0, rescale);
	rescale = max(0.0, rescale);
	if(rescale >= m_WoodyGreenup)
	{
		return m_WoodySlope * rescale + m_WoodyIntercept;
	}
	return m_WoodyMin;
}
/*double CLFIengine::CalcRunningAvgLFIHerb()
{
	int numValid = 0;
	double herb = 0.0;
	for(int i = 0; i < m_HerbDaysAvg; i++)
	{
		if(m_HerbArray[i] >= 0)
		{
			numValid++;
			herb += m_HerbArray[i];
		}
	}
	if(numValid > 0)
		herb /= numValid;
	return herb;
}

double CLFIengine::CalcRunningAvgLFIWoody()
{
	int numValid = 0;
	double woody = 0.0;
	for(int i = 0; i < m_WoodyDaysAvg; i++)
	{
		if(m_WoodyArray[i] >= 0)
		{
			numValid++;
			woody += m_WoodyArray[i];
		}
	}
	if(numValid > 0)
		woody /= numValid;
	return woody;
}*/

void CLFIengine::AddLFIobs(COleDateTime lfiDay, double val)
{
	if(lastLFIday.GetStatus() == COleDateTime::valid)
	{
		COleDateTimeSpan span = lfiDay - lastLFIday;
		int skip = span.GetTotalDays();
		if(skip > 30)//need to reset
		{
			for(int i = 0; i < m_LFIdaysAvg; i++)
				m_LFIarray[i] = -1;
		}
		else
		{
			for(int i = m_LFIdaysAvg - skip - 1; i >= 0; i--)
				m_LFIarray[i + skip] = m_LFIarray[i];
		}
		if(lastLFIday.GetYear() != lfiDay.GetYear())
		{
			hasGreenedUpThisYear = false;
			canIncreaseHerb = true;
			hasExceeded120ThisYear = false;
		}
		/*if(!hasGreenedUpThisYear && val >= m_HerbGreenup * 100.0)
		{
			hasGreenedUpThisYear = true;
			canIncreaseHerb = true;
			TRACE1("Greenup: %s\n", lfiDay.Format());
		}*/
			//else if(
	}
	else
	{
		for(int i = 0; i < m_LFIdaysAvg; i++)
			m_LFIarray[i] = -1;
	}
	m_LFIarray[0] = val;
	lastLFIday = lfiDay;
}

bool CLFIengine::IsUsingVPDMax()
{
	return !m_UseVPDAvg;
}

bool CLFIengine::IsUsingVPDAvg()
{
	return m_UseVPDAvg;
}

double CLFIengine::CalcLFI(double minRH, double maxTempF, double minTempF, double lat, int doy)
{
	double LFI = 0.0;
	double GSI, tMinInd, vpdInd, daylenInd, vpd;
	tMinInd = GetTminInd(minTempF);
	vpd = CalcVPD(max(minRH, 5.0), maxTempF);
	vpdInd = GetVPDInd(vpd);
	daylenInd = GetDaylInd(CalcDayl(lat, doy));
	LFI = tMinInd * vpdInd * daylenInd * 100.0;
	return LFI;
}

double CLFIengine::CalcLFI_VPDAvg(double RH, double TempF, double maxTempF, double minTempF, double lat, int doy)
{
	double LFI = 0.0;
	double GSI, tMinInd, tDew, vpdInd, daylenInd, vpd;
	tMinInd = GetTminInd(minTempF);
	tDew = CalcDPT(TempF, RH);
	vpd = CalcVPDavg(tDew, (maxTempF + minTempF) / 2);
	vpdInd = GetVPDInd(vpd);
	daylenInd = GetDaylInd(CalcDayl(lat, doy));
	LFI = tMinInd * vpdInd * daylenInd * 100.0;
	return LFI;
}

double CLFIengine::GetTminInd(double Tmin)
{
	double tmin =  (Tmin - 32.0) * 5.0 / 9.0;
	if(m_TminMax == m_TminMin)
		return 0;
    if( tmin < m_TminMin)
	{
		return 0;
	}
	else if(tmin > m_TminMax)
	{
		return 1;
	}
	else
	{
		return (tmin - m_TminMin) / (m_TminMax - m_TminMin);
	}
}

double CLFIengine::GetVPDInd(double VPD)
{
	if(m_VPDMax == m_VPDMin)
		return 0;
    if( VPD < m_VPDMin)
	{
		return 1;
	}
	else if(VPD > m_VPDMax)
	{
		return 0;
	}
	else
	{
		return 1 - (VPD - m_VPDMin) / (m_VPDMax - m_VPDMin);

	}
}

double CLFIengine::GetDaylInd(double Dayl)
{
	if(m_DaylenMin == m_DaylenMax)
		return 0;
    if( Dayl < m_DaylenMin)
	{
		return 0;
	}
	else if(Dayl > m_DaylenMax)
	{
		return 1;
	}
	else
	{
		return (Dayl - m_DaylenMin) / (m_DaylenMax - m_DaylenMin);

	}
}

double CLFIengine::GetHerbMaxGSI()
{
	return m_HerbMaxGSI;
}

double CLFIengine::GetHerbGreenup()
{
	return m_HerbGreenup;
}

double CLFIengine::GetWoodyMaxGSI()
{
	return m_WoodyMaxGSI;
}

double CLFIengine::GetWoodyGreenup()
{
	return m_WoodyGreenup;
}

double CLFIengine::GetTminMin()
{
	return m_TminMin;
}

double CLFIengine::GetTminMax()
{
	return m_TminMax;
}

double CLFIengine::GetVPDMin()
{
	return m_VPDMin;
}

double CLFIengine::GetVPDMax()
{
	return m_VPDMax;
}

double CLFIengine::GetDaylenMin()
{
	return m_DaylenMin;
}

double CLFIengine::GetDaylenMax()
{
	return m_DaylenMax;
}

int CLFIengine::GetMAPeriod()
{
	return m_LFIdaysAvg;
}

int CLFIengine::GetNumPrecipDays()
{
	return m_PcpDays;
}

double CLFIengine::GetRTPcpMin()
{
	return m_PrcpMin;
}

double CLFIengine::GetRTPcpMax()
{
	return m_PrcpMax;
}

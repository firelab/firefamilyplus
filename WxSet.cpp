// WxSet.h : Implementation of the CWxSet class


// CWxSet implementation

// code generated on Wednesday, September 28, 2005, 1:25 PM

#include "stdafx.h"
#include "WxSet.h"
#include "math.h"
#include "SowThresholdsSet.h"
#include "SIGStationSet.h"
#include "SowOptionsSet.h"
#include <list>
#include <numeric>

using namespace std;

const int RH_FOG = 95;

long GetMinPrecipObs(COleDateTime start, COleDateTime end)
{
	long nValidObs = 0;
	switch(end.GetDay() - start.GetDay() + 1)
	{
	case 1:
		nValidObs = 1;
		break;
	case 2:
		nValidObs = 2;
		break;
	case 3:
		nValidObs = 2;
		break;
	case 4:
		nValidObs = 3;
		break;
	case 5:
		nValidObs = 4;
		break;
	case 6:
		nValidObs = 5;
		break;
	case 7:
		nValidObs = 5;
		break;
	case 8:
		nValidObs = 6;
		break;
	case 9:
		nValidObs = 7;
		break;
	case 10:
		nValidObs = 8;
		break;
	case 11:
		nValidObs = 9;
		break;
	default:
		nValidObs = (long)((end.GetDay() - start.GetDay() + 1) * 0.8);
	}
	return nValidObs;
}
IMPLEMENT_DYNAMIC(CWxSet, CRecordset)

CWxSet::CWxSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_pSowOpts = NULL;
	m_StationID = L"";
	m_ObsDate;
	m_ObsType = L"";
	m_UserWX = 0;
	m_SOW = 0;
	m_Temp = 0;
	m_RH = 1;
	m_WDir = 0;
	m_WS = 0;
	m_OMC10 = 0.0;
	m_OMCWood = 0.0;
	m_OMCWoodDate;
	m_TmpMax = 0;
	m_TmpMin = 0;
	m_RHMax = 0;
	m_RHMin = 1;
	m_PPTDUR = 0;
	m_PPTAMT = 0.0;
	m_Season = 0;
	m_GreenHerb = 0;
	m_GreenShrub = 0;
	m_MC1 = 0.0;
	m_MC10 = 0.0;
	m_MC100 = 0.0;
	m_MC1000 = 0.0;
	m_X1000 = 0.0;
	m_MCHerb = 0.0;
	m_MCWood = 0.0;
	m_GREN = 0;
	m_HStage = 0;
	m_ROS = 0.0;
	m_ERC = 0.0;
	m_BI = 0;
	m_FIL = 0;
	m_WAzimuth = 0;
	m_WetFlag = 0;
	m_SolarRadiation = 0;
	m_GustDir = 0;
	m_GustSpd = 0;
	m_HourlyPrecip = 0;
	m_SR_SOW = 0;
	m_SR_WetFlag = 0;
	m_SnowFlag = 0;
	m_Hour = 0;
	m_DailyObs = 0;
	m_FM1 = -1.0;
	m_FM10 = -1.0;
	m_FM100 = -1.0;
	m_FM1000 = -1.0;
	m_FuelTemperature = -1.0;
	m_nFields = 50;
	m_nDefaultType = dynaset;
}

CWxSet::~CWxSet()
{
	if(m_pSowOpts)
	{
		m_pSowOpts->Close();
		delete m_pSowOpts;
	}
}

//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CWxSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CWxSet::GetDefaultSQL()
{
	return _T("[WXObs]");
}

void CWxSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[StationID]"), m_StationID);
	RFX_Date(pFX, _T("[ObsDate]"), m_ObsDate);
	RFX_Text(pFX, _T("[ObsType]"), m_ObsType);
	RFX_Long (pFX, _T("[UserWX]"), m_UserWX);
	RFX_Long (pFX, _T("[SOW]"), m_SOW);
	RFX_Long (pFX, _T("[Temp]"), m_Temp);
	RFX_Long (pFX, _T("[RH]"), m_RH);
	RFX_Long (pFX, _T("[WDir]"), m_WDir);
	RFX_Long (pFX, _T("[WS]"), m_WS);
	RFX_Single(pFX, _T("[OMC10]"), m_OMC10);
	RFX_Single(pFX, _T("[OMCWood]"), m_OMCWood);
	RFX_Date(pFX, _T("[OMCWoodDate]"), m_OMCWoodDate);
	RFX_Long (pFX, _T("[TmpMax]"), m_TmpMax);
	RFX_Long (pFX, _T("[TmpMin]"), m_TmpMin);
	RFX_Long (pFX, _T("[RHMax]"), m_RHMax);
	RFX_Long (pFX, _T("[RHMin]"), m_RHMin);
	RFX_Long (pFX, _T("[PPTDUR]"), m_PPTDUR);
	RFX_Single(pFX, _T("[PPTAMT]"), m_PPTAMT);
	RFX_Long (pFX, _T("[Season]"), m_Season);
	RFX_Long (pFX, _T("[GreenHerb]"), m_GreenHerb);
	RFX_Long (pFX, _T("[GreenShrub]"), m_GreenShrub);
	RFX_Single(pFX, _T("[MC1]"), m_MC1);
	RFX_Single(pFX, _T("[MC10]"), m_MC10);
	RFX_Single(pFX, _T("[MC100]"), m_MC100);
	RFX_Single(pFX, _T("[MC1000]"), m_MC1000);
	RFX_Single(pFX, _T("[X1000]"), m_X1000);
	RFX_Single(pFX, _T("[MCHerb]"), m_MCHerb);
	RFX_Single(pFX, _T("[MCWood]"), m_MCWood);
	RFX_Long (pFX, _T("[GREN]"), m_GREN);
	RFX_Long (pFX, _T("[HStage]"), m_HStage);
	RFX_Single(pFX, _T("[ROS]"), m_ROS);
	RFX_Single(pFX, _T("[ERC]"), m_ERC);
	RFX_Long (pFX, _T("[BI]"), m_BI);
	RFX_Long (pFX, _T("[FIL]"), m_FIL);
	RFX_Long (pFX, _T("[CORCT2]"), m_WAzimuth);
	RFX_Long (pFX, _T("[SolarRadiation]"), m_SolarRadiation);
	RFX_Long (pFX, _T("[WetFlag]"), m_WetFlag);
    RFX_Long (pFX, _T("[GustDir]"), m_GustDir);
    RFX_Long (pFX, _T("[GustSpd]"), m_GustSpd);
	RFX_Single(pFX, _T("[HourlyPrecip]"), m_HourlyPrecip);
    RFX_Long (pFX, _T("[SR_SOW]"), m_SR_SOW);
	RFX_Long (pFX, _T("[SR_WetFlag]"), m_SR_WetFlag);
	RFX_Long (pFX, _T("[SnowFlag]"), m_SnowFlag);
	RFX_Long (pFX, _T("[Hour]"), m_Hour);
	RFX_Long (pFX, _T("[DailyObs]"), m_DailyObs);
	//NFDRS2016 additions
	RFX_Double(pFX, _T("[FM_1]"), m_FM1);
	RFX_Double(pFX, _T("[FM_10]"), m_FM10);
	RFX_Double(pFX, _T("[FM_100]"), m_FM100);
	RFX_Double(pFX, _T("[FM_1000]"), m_FM1000);
	RFX_Double(pFX, _T("[FuelTemperature]"), m_FuelTemperature);

}
/////////////////////////////////////////////////////////////////////////////
// CWxSet diagnostics

#ifdef _DEBUG
void CWxSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CWxSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

double getMaxSolar(COleDateTime odate, int hr, int min, double staLat, double staLong)
{
        /* conversion factor from degrees to radians (= 2*?/360 = 0.0174533) */
        double degreeToRadius = 0.017453293;

        /* constant to compute the longitude of the sun measured in the ecliptic plane from
         * the vernal equinox (Julian day = 81) (= 360/365=0.986301) */
        double dpd = 0.986301;
        /* obliquity = 23.5 degree */
        double obliquity = 23.5;
        /* sin of obecl, where obecl = 23.5 * degreeToRadius */
        double sinob = sin(obliquity * degreeToRadius);

        double julianDate = 0, xt24 = 0, hourFraction = 0;
        double declin = 0;          // declination
        double decdeg = 0;          // declination in degree
        double djul = 0, rjul = 0, eccfac = 0, solcon = 0, tlocap = 0, omega = 0;
        double xxlat = 0, fracsc = 0, solarPotential = 0, xLong = 0;
        //Calendar calDate = Calendar.getInstance();

        /* convert the date/time into julian date */
        //SimpleDateFormat oldDateFormat = new SimpleDateFormat("dd-MMM-yy");
        //try {
       //   oldDateFormat.parse(odate);
       // } catch (Exception e) {
        //  System.out.println(e.toString());
       // }

        //calDate = oldDateFormat.getCalendar();
        //calDate.set(toInt(toDate.substring(6,7)), toInt(toDate.substring(3,4)), toInt(toDate.substring(0,1)));
        xt24 = hr + min/60.0;
        hourFraction = xt24 / 24.0;
		julianDate = odate.GetDayOfYear() + hourFraction;//calDate.get(Calendar.DAY_OF_YEAR) + hourFraction;
        /* calculate solar constant for a given day of the year (julian date) */
        if (julianDate > 80) {
            xLong = dpd * (julianDate - 80.0);
        } else {
            xLong = dpd * (julianDate + 285.0);
        }
        xLong = xLong * degreeToRadius;
        declin = asin(sinob * sin(xLong));
        decdeg = declin / degreeToRadius;

        djul = julianDate * (360.0 / 365.0);
        rjul = djul * degreeToRadius;
        eccfac = 1.000110 + 0.034221 * cos(rjul) + 0.001280 * sin(rjul)
                 + 0.000719 * cos(2 * rjul) + 0.000077 * sin(2.0 * rjul);
        solcon = 1370.0 * eccfac;

        /* calculate solar radiation incident at a given location */
        tlocap = xt24 + staLong / 15.0;
        omega = 15.0 * (tlocap - 12.0) * degreeToRadius;
        xxlat = staLat * degreeToRadius;
        fracsc = sin(declin) * sin(xxlat) + cos(declin) * cos(xxlat) * cos(omega);
        solarPotential = fracsc * solcon;

        return solarPotential;
  }

void CWxSet::CheckSowOpts()
{
	if(m_pSowOpts && m_StationID.Compare(m_pSowOpts->m_SIG_Station.Trim()) != 0)
	{
		m_pSowOpts->Close();
		delete m_pSowOpts;
		m_pSowOpts = NULL;
	}
	if(!m_pSowOpts)
	{
		m_pSowOpts = new CSowOptionsSet(m_pDatabase);
		m_pSowOpts->m_strFilter.Format("[SIG_Station] = '%6.6s'", m_StationID);
		m_pSowOpts->Open();
	}
	if(m_pSowOpts->IsBOF() || m_pSowOpts->IsEOF())
	{
		m_pSowOpts->AddNew();
		m_pSowOpts->m_SIG_Station = m_StationID;
		m_pSowOpts->m_SOW_Usage = CSowOptionsSet::SOW_MISSING;
		m_pSowOpts->m_WetFlag_Usage = CSowOptionsSet::WF_MISSING;
		m_pSowOpts->Update();
		m_pSowOpts->Requery();
	}
}

int CWxSet::ResolveSOW()
{
	CheckSowOpts();
	if (! m_pSowOpts)
		return 0;

	//int retSOW = 0;
	switch(m_pSowOpts->m_SOW_Usage)
	{
	case CSowOptionsSet::SR_SOW_ONLY:
		if(!IsFieldNull(&m_SR_SOW))
			return m_SR_SOW;
		break;
	case CSowOptionsSet::SR_SOW_NEVER:
		if(!IsFieldNull(&m_SOW))
			return m_SOW;
		break;
	case CSowOptionsSet::SOW_MISSING:
	default:
		if(!IsFieldNull(&m_SOW))
			return m_SOW;
		if(!IsFieldNull(&m_SR_SOW))
			return m_SR_SOW;
		break;
	}
	return 0;
}

int CWxSet::ResolveWetFlag()
{
	CheckSowOpts();
	int retWF = 0;
	switch(m_pSowOpts->m_WetFlag_Usage)
	{
	case CSowOptionsSet::EST_WF_ONLY:
		if(!IsFieldNull(&m_SR_WetFlag))
			return m_SR_WetFlag;
		break;
	case CSowOptionsSet::EST_WF_NEVER:
		if(!IsFieldNull(&m_WetFlag))
			return m_WetFlag;
		break;
	case CSowOptionsSet::WF_MISSING:
	default:
		if(!IsFieldNull(&m_WetFlag))
			return m_WetFlag;
		if(!IsFieldNull(&m_SR_WetFlag))
			return m_SR_WetFlag;
		break;
	}
	return retWF;
}

long CWxSet::CalcSOW()
{
	long nUpdates = 0;
	CSIGStationSet staSet(m_pDatabase);
	staSet.m_strFilter.Format("[StationID] = '%6.6s'", m_StationID);
	staSet.Open();
	CSowThresholdsSet sowSet(m_pDatabase);
	MoveFirst();
	sowSet.m_strFilter.Format("[SIG_Station] = '%s'", m_StationID);
	sowSet.Open();
	if(sowSet.IsBOF() && sowSet.IsEOF())
	{
		sowSet.AddNew();
		sowSet.m_SIG_Station = m_StationID;
		sowSet.m_PCNT_Clear = 85;
		sowSet.m_PCNT_Scattered = 75;
		sowSet.m_PCNT_Broken = 50;
		switch(staSet.m_ClimateCls)
		{
		case 1:
			sowSet.m_1HR_Drizzle = 0.1;
			sowSet.m_1HR_Rain = 0.15;
			sowSet.m_1HR_Showers = 0.5;
			sowSet.m_3HR_DUR_WetFlag = 2;
			sowSet.m_3HR_AMT_WetFlag = 0.75;
			sowSet.m_24HR_DUR_WetFlag = 8;
			sowSet.m_24HR_AMT_WetFlag = 2.0;
			break;
		case 2:
			sowSet.m_1HR_Drizzle = 0.1;
			sowSet.m_1HR_Rain = 0.15;
			sowSet.m_1HR_Showers = 0.5;
			sowSet.m_3HR_DUR_WetFlag = 2;
			sowSet.m_3HR_AMT_WetFlag = 0.75;
			sowSet.m_24HR_DUR_WetFlag = 10;
			sowSet.m_24HR_AMT_WetFlag = 1.5;
			break;
		case 4:
			sowSet.m_1HR_Drizzle = 0.05;
			sowSet.m_1HR_Rain = 0.1;
			sowSet.m_1HR_Showers = 0.25;
			sowSet.m_3HR_DUR_WetFlag = 3;
			sowSet.m_3HR_AMT_WetFlag = 0.4;
			sowSet.m_24HR_DUR_WetFlag = 12;
			sowSet.m_24HR_AMT_WetFlag = 0.75;
			break;
		case 3:
		default:
			sowSet.m_1HR_Drizzle = 0.05;
			sowSet.m_1HR_Rain = 0.1;
			sowSet.m_1HR_Showers = 0.25;
			sowSet.m_3HR_DUR_WetFlag = 3;
			sowSet.m_3HR_AMT_WetFlag = 0.5;
			sowSet.m_24HR_DUR_WetFlag = 12;
			sowSet.m_24HR_AMT_WetFlag = 1.0;
		}
		sowSet.Update();
		sowSet.Requery();
	}
	double solRadPot, pcnt_solar;
	int sow, wetflag;
	double threeHrPcpAmt = 0.0;
	int threeHrPcpDur = 0;
	double pcp3[4];
	for(int i = 0; i < 4; i++)
		pcp3[i] = 0.0;
	while(!IsEOF())
	{
		threeHrPcpDur = 0;
		threeHrPcpAmt = 0.0;
		for(int i = 3; i > 0; i--)
		{
			pcp3[i] = pcp3[i-1];
			if(pcp3[i] > 0.0)
			{
				threeHrPcpDur++;
				threeHrPcpAmt += pcp3[i];
			}
		}
		pcp3[0] = m_HourlyPrecip;
		/*if(pcp3[0] > 0.0)
		{
			threeHrPcpDur++;
			threeHrPcpAmt += pcp3[0];
		}*/
		//if (m_ObsType != 'O')
		//if (m_ObsType != 'O' && m_Hour != staSet.m_RegSchdObs)
		if(m_DailyObs != 1)
		{
			//track 3 hour precip
			MoveNext();
			continue;
		}
		solRadPot = getMaxSolar(this->m_ObsDate, this->m_ObsDate.GetHour(), m_ObsDate.GetMinute(), staSet.m_LatDegrees, 0.0);//staSet.m_LonDegrees);
		if(solRadPot <= 0.0 || IsFieldNull(&m_SolarRadiation))
		{
			MoveNext();
			continue;
		}
		pcnt_solar = m_SolarRadiation / solRadPot * 100.0;
		if(this->m_PPTAMT <= 0.0)
		{
      //case 1: no precipitation in last 24 hours
			if (pcnt_solar >= sowSet.m_PCNT_Clear)
				sow = 0;
			else if (pcnt_solar >= sowSet.m_PCNT_Scattered)
				sow = 1;
			else if (pcnt_solar >= sowSet.m_PCNT_Broken)
				sow = 2;
			else if (this->m_RH <= 95)
				sow=3;
			else
				sow=4;
			if (m_SolarRadiation < 25)
				sow = 3;
			wetflag = 0;//"N";
		}
      else if (threeHrPcpAmt <= 0.0)
	  {
      //case 2: precipitation in last 24 hours, but not the last three hours
        if (pcnt_solar >= sowSet.m_PCNT_Clear)
			sow = 0;
        else if (pcnt_solar >= sowSet.m_PCNT_Scattered)
			sow = 1;
        else if (pcnt_solar >= sowSet.m_PCNT_Broken)
			sow = 2;
        else if (m_RH < RH_FOG)
			sow=3;
        else
			sow=4;
		//if (this->m_PPTDUR > twentyfour_hr_dur_wetflag || twentyfourHrPrecipAmt > twentyfour_hr_amt_wetflag)
		if (m_PPTDUR > sowSet.m_24HR_DUR_WetFlag || m_PPTAMT > sowSet.m_24HR_AMT_WetFlag)
          wetflag = 1;//"Y";
        else
          wetflag = 0;//"N";
      }
      else if (m_HourlyPrecip<=0.0)
	  {
      //case 4: precipitation in last three hours, but not the last hour
        if (pcnt_solar >= sowSet.m_PCNT_Clear)
			sow = 0;
        else if (pcnt_solar >= sowSet.m_PCNT_Scattered)
			sow = 1;
        else if (pcnt_solar >= sowSet.m_PCNT_Broken)
			sow = 2;
        else if (m_RH < RH_FOG)
			sow=3;
        else
			sow=4;
        //if (threeHrPrecipDur > three_hr_dur_wetflag || threeHrPrecipAmt > three_hr_amt_wetflag)
        if (threeHrPcpDur > sowSet.m_3HR_DUR_WetFlag || threeHrPcpAmt > sowSet.m_3HR_AMT_WetFlag)
          wetflag = 1;//"Y";
        else
          wetflag = 0;//"N";
      }

      else
      {
      //case 3: precipitation during last hour
      //        modified by LSB Nov. 2010 prior to V2 release.  SOW 5 (drizzle) will not be set
      //        SOW for rain amounts less than drizzle threshold is set from solar
		  if (m_HourlyPrecip <= sowSet.m_1HR_Drizzle)//one_hr_drizzle)
        {
            if (pcnt_solar >= sowSet.m_PCNT_Clear)
				sow = 0;
            else if (pcnt_solar >= sowSet.m_PCNT_Scattered)
				sow = 1;
            else if (pcnt_solar >= sowSet.m_PCNT_Broken)
				sow = 2;
            else if (m_RH <= 95)
				sow=3;
            else
				sow=4;
           if (m_SolarRadiation < 25)
			   sow = 3;
           wetflag = 0;//"N";
        }
        //if (oneHrPrecipAmt > one_hr_drizzle && oneHrPrecipAmt <= one_hr_rain)
		  if (m_HourlyPrecip > sowSet.m_1HR_Drizzle && m_HourlyPrecip <= sowSet.m_1HR_Rain)
			sow = 6; // rain
		  if (m_HourlyPrecip > sowSet.m_1HR_Rain    && m_HourlyPrecip <= sowSet.m_1HR_Showers)
			sow = 8; //t-showers
        if (m_HourlyPrecip > sowSet.m_1HR_Showers)
			sow = 9;                                         //thunderstorm
		if (m_Temp <= 32.0f && m_HourlyPrecip > sowSet.m_1HR_Drizzle)
			sow = 7;    // snow                                                         //snow/sleet
        wetflag = 0;//"N";
        //if (sow==6||sow==7 || twentyfourHrPrecipDur > twentyfour_hr_dur_wetflag || twentyfourHrPrecipAmt > twentyfour_hr_amt_wetflag || threeHrPrecipDur>three_hr_dur_wetflag || threeHrPrecipAmt>three_hr_amt_wetflag)
		if (sow == 6||sow == 7 || m_PPTDUR > sowSet.m_24HR_DUR_WetFlag
			|| m_PPTAMT > sowSet.m_24HR_AMT_WetFlag
			|| threeHrPcpDur > sowSet.m_3HR_DUR_WetFlag
			|| threeHrPcpAmt > sowSet.m_3HR_AMT_WetFlag)
			wetflag = 1;//"Y";
      }
	  Edit();
	  m_SR_SOW = sow;
	  m_SR_WetFlag = wetflag;
	  Update();
	  nUpdates++;
		MoveNext();
	}

	sowSet.Close();
	staSet.Close();
	return nUpdates;
}

int CWxSet::Calc24HourPrecip(CString trgStation)
{
	int ret = 0, thisDur = 0;
	list<double> prev23;

	double cur24 = 0.0, thisPcp;

	if (!IsOpen())
		return -1;
	if (IsEOF())
		return -1;

	COleDateTime thisdate, lastdate;

	MoveFirst();
	if (IsEOF())
		return -1;
	//CString staID = m_StationID;
	COleDateTime trgDate = m_ObsDate;
	queue<double> qPrev24 = GetPrevious24HourlyPrecip(trgStation, trgDate);
	qPrev24.pop();//only need 23
	lastdate = m_ObsDate;
	for (int i = 0; i < 23; i++)
	{
		prev23.push_back(qPrev24.front());
		qPrev24.pop();
	}

	while (!IsEOF())
	{
		thisdate = m_ObsDate;
		int shift = 1;
		COleDateTimeSpan difference;
		difference = thisdate - lastdate;
		shift = (int)(difference.GetTotalHours());
		// any difference greater than 24 hours can be treated like 24 hours
		if (shift > 23)
			shift = 24;
		if (!IsFieldNull(&m_HourlyPrecip))
			thisPcp = m_HourlyPrecip;
		else
			thisPcp = 0.0;

		for (int i = 0; i < shift && prev23.size() > 0; i++)
		{
			//lastPop = *prev23.begin();
			prev23.pop_front();
			//qPrev24.pop();
		}
		while (prev23.size() < 23)
		{
			prev23.push_back(0.0);
		}
		cur24 = std::accumulate(prev23.begin(), prev23.end(), 0.0f) + thisPcp;
		thisDur = std::count_if(prev23.begin(), prev23.end(), [](double i) {return i > 0.0; });

		prev23.push_back(thisPcp);
		// don't update type 'O' records if this is  (apparently) daily data
		if (!(shift > 20 && m_ObsType.GetAt(0) == 'O'))
		{
			Edit();
			// now update values
			// update HourlyPrecip with newly computed value 
			m_PPTAMT = cur24;
			m_PPTDUR = thisDur;
			Update();
		}
		lastdate = m_ObsDate;

		MoveNext();
	}
	Requery();
	return ret;
}

int CWxSet::CalcHourlyPrecip(CString trgStation)
{
	double tooSmall = 0.001;
	list<double> prev23;
	double cur24 = 0.0;
	double curPcp = 0.0;
	double sum23;
	double lastPop = 0.0;
	//int pcpDur = 0, prevDur = 0, durDiff, shouldDiff;

	//solve for curPcp which is equal to cur24 (record read from database) - sum of previous 23 hours
	if (!IsOpen())
		return -1;
	if (IsEOF())
		return -1;

	COleDateTime thisdate, lastdate;

	MoveFirst();
	if (IsEOF())
		return -1;
	//CString staID = m_StationID;
	COleDateTime trgDate = m_ObsDate;
	queue<double> qPrev24 = GetPrevious24HourlyPrecip(trgStation, trgDate);
	qPrev24.pop();//only need 23
	lastdate = m_ObsDate;
	for (int i = 0; i < 23; i++)
	{
		prev23.push_back(qPrev24.front());
		qPrev24.pop();
	}
	while (!IsEOF())
	{
		thisdate = m_ObsDate;
		int shift = 1;
		COleDateTimeSpan difference;
		difference = thisdate - lastdate;
		shift = (int)(difference.GetTotalHours());
		// any difference greater than 24 hours can be treated like 24 hours
		if (shift > 23)
			shift = 24;
		if (!IsFieldNull(&m_PPTAMT))
			cur24 = m_PPTAMT;
		else
			cur24 = 0.0;

		for (int i = 0; i < shift && prev23.size() > 0; i++)
		{
			lastPop = *prev23.begin();
			prev23.pop_front();
			//qPrev24.pop();
		}
		while (prev23.size() < 23)
		{
			prev23.push_back(0.0);
		}
		sum23 = std::accumulate(prev23.begin(), prev23.end(), 0.0f);

		//dur tracking
		//prevDur = pcpDur;
		//pcpDur = m_PPTDUR;
		//shouldDiff = durDiff = pcpDur - prevDur;

		curPcp = cur24 - sum23;
		//if (lastPop > 0.0)
		//{
		//	shouldDiff++;

		//}
		if (curPcp < -tooSmall)
		{//cludge for skipped hour whose rain must be accounted for in hourly stream (DST "Spring forward" will cause this)
			//if (fabs(curPcp + lastPop) < tooSmall)//gawdammit
			//{
			TRACE2("%s - curPcp: %f\n", m_ObsDate.Format("%m/%d/%y %H:%M"), curPcp);
			curPcp = 0.0;
			//}
		}
		if (curPcp < tooSmall)
			curPcp = 0.0;

		prev23.push_back(curPcp);
		// don't update type 'O' records if this is  (apparently) daily data
		if (!(shift > 20 && m_ObsType.GetAt(0) == 'O'))
		{
			Edit();
			// now update values
			// update HourlyPrecip with newly computed value 
			m_HourlyPrecip = curPcp;
			Update();
		}
		lastdate = m_ObsDate;

		MoveNext();
	}
	Requery();
	return 0;
}

int CWxSet::CalcHourlyPrecip(COleDateTime *precipStart)
{//fw9 files have no hourly precip, need to pre-calculate these and populate records... and don't touch mins and maxs!
	// arrays to hold last 26 values
	float precip[25];
	float pptamt[25];


	int records = 0, nSkip = 0;

	for (int i=0;i<25;i++)
	{
		precip[i] = 0.0;
		pptamt[i] = 0.0;
	}

	if (! IsOpen())
		return -1;
	if (IsEOF())
		return -1;

	COleDateTime thisdate, lastdate;

	MoveFirst();
	if (IsEOF())
		return -1;

	// need a 24hr pcp amount of 0.0 to start
	while (! IsEOF())
	{
		if(!IsFieldNull(&m_PPTAMT) && m_PPTAMT == 0.0)
		{
			*precipStart = m_ObsDate;
			break;
		}
		nSkip++;
		MoveNext();
	}
	if (IsEOF())
		return -1;

	if (! IsFieldNull(&m_PPTAMT))
	{
		pptamt[24] = m_PPTAMT;
	}
	while (! IsEOF())
	{
		// 'shift' is the difference (in hours) between this obs and the last obs.
		// for normal hourly data, shift = 1..... for daily data, shift = 24

		thisdate = m_ObsDate;
		int shift = 1;
		int i = 0;
		// shift values back
		// e.g. if the current record is 11AM, the 10AM temperature becomes temp[22], the 9AM temp becomes temp[21], and so on...
		if (records > 0)
		{
			COleDateTimeSpan difference;
			difference = thisdate - lastdate;
			shift = (int) (difference.GetTotalHours());
			// any difference greater than 24 hours can be treated like 24 hours
			if (shift > 24)
				shift = 24;
			//if (shift < 26)
			if (shift < 24)//changed SB, 2012/04/20 to clear when daily
			{
				for (i=0;i<24 && ((i + shift) < 25);i++)
				{
					precip[i] = precip[i+shift];
					pptamt[i] = pptamt[i+shift];
				}
			}
		}

		while (i < 25)
		{
			precip[i] = 0.0;
			pptamt[i] = 0.0;
			i++;
		}
		if (!IsFieldNull(&m_PPTAMT))
			pptamt[24] = m_PPTAMT;
		else
			pptamt[24] = 0.0;
		// we don't have an HourlyPrecip value, so we need to calculate hourly precip from PPTAMT and PPTDUR
		// note that we need at least 24 hours of data to be able to do this......
		precip[24] = 0;
		precip[24] = max(0,pptamt[24]-pptamt[23]-precip[0]);// + precip[1];
		// don't update type 'O' records if this is  (apparently) daily data
		if (! (shift > 20 && m_ObsType.GetAt(0) == 'O') )
		{
			Edit();
			// now update values
			// update HourlyPrecip with newly computed value (based on PPTAMT & PPTDUR)
			m_HourlyPrecip = precip[24];
			Update();
		}
		records ++;
		lastdate = m_ObsDate;
		MoveNext();
	}
	return nSkip;
}
// new 09/2012
// set the DailyObs field based on the type of observation
// DailyObs = 1   if ObsType=='O'
// DailyObs = 1   if ObsType=='R' AND SOW != NULL  AND  hour=station's Regular Observation Time
// DailyObs = 0 for all other records

// note that we need to query for a specific station before running this function -
// (or if using a wxset for multiple stations with type 'R' records, make sure their observation times are all the same)
//void CWxSet::setDailyObs(int hour, CCancelDialog *cd, int iStep)
//
//9/16/2017 this damn function will set > 1 record per day as dialyObs if type O is not at reg sched obs hour
//grrr
//
void CWxSet::setDailyObs(int hour, int iStep)
{

	if (! IsOpen())
       return;

	if (IsEOF())
	   return;

	MoveFirst();
   if (IsEOF())
	   return;
   int iRec = 0;

   while (!IsEOF())
   {
	   try
	   {
		   iRec++;

		   /*if (cd)
		   {
				if (!(iRec%100))
					cd->Add(100);
				if (iStep > 0)
				   if (!(iRec%iStep))
					   cd->Step();
		   }*/
		   Edit();
		   m_DailyObs = 0;
		   if (m_ObsType.CompareNoCase("O") == 0)
			   m_DailyObs = 1;
		   else
			   if ((m_ObsType.CompareNoCase("R") == 0)
				   && (m_Hour == hour))
				   //&& (!IsFieldNull(&m_SOW)))
				   m_DailyObs = 1;

		   Update();
		   MoveNext();
	   }
	   catch (CDBException* e)
	   {//ignore
		   e->Delete();
	   }
   }
	   CWxSet daySet(m_pDatabase);
	   daySet.Open();
	   COleDateTime lastDay;
	   lastDay.SetStatus(COleDateTime::invalid);
	   int nDailyToday = 0;
	   bool typeOtoday = false;
	   MoveFirst();
	   while (!IsEOF())
	   {
		   if (lastDay.GetStatus() == COleDateTime::invalid)
			   lastDay = m_ObsDate;
		   if (lastDay.GetYear() == m_ObsDate.GetYear()
			   && lastDay.GetDayOfYear() == m_ObsDate.GetDayOfYear())
		   {//same day count dailyObs this day
			   if (m_DailyObs == 1)
				   nDailyToday++;
			   if (m_ObsType[0] == 'O')
				   typeOtoday = true;
		   }
		   else
		   {//new day
			   if (nDailyToday > 1)
			   {//gotta fix it
				   COleDateTime t1 = lastDay, t2 = lastDay;
				   t1.SetDateTime(lastDay.GetYear(), lastDay.GetMonth(), lastDay.GetDay(), 0, 0, 0);
				   t2.SetDateTime(lastDay.GetYear(), lastDay.GetMonth(), lastDay.GetDay(), 23, 59, 59);
				   daySet.m_strFilter.Format("[StationID] = '%6.6s' AND DailyObs = 1 AND[ObsDate] >= #%s# AND[ObsDate] <= #%s#",
					   m_StationID, t1.Format(), t2.Format());
				   daySet.Requery();
				   daySet.ensureMaxOneDailyObs(hour);
			   }
			   nDailyToday = 0;
			   typeOtoday = false;
			   if (m_DailyObs == 1)
				   nDailyToday++;
			   if (m_ObsType[0] == 'O')
				   typeOtoday = true;
		   }
		   lastDay = m_ObsDate;
		   MoveNext();
	   }
	   if (nDailyToday > 1)//check last one as exited the loop due to IsEOF()
	   {//gotta fix it
		   COleDateTime t1 = lastDay, t2 = lastDay;
		   t1.SetDateTime(lastDay.GetYear(), lastDay.GetMonth(), lastDay.GetDay(), 0, 0, 0);
		   t2.SetDateTime(lastDay.GetYear(), lastDay.GetMonth(), lastDay.GetDay(), 23, 59, 59);
		   daySet.m_strFilter.Format("[StationID] = '%6.6s' AND DailyObs = 1 AND[ObsDate] >= #%s# AND[ObsDate] <= #%s#",
			   m_StationID, t1.Format(), t2.Format());
		   daySet.Requery();
		   daySet.ensureMaxOneDailyObs(hour);
	   }
	   daySet.Close();
}

void CWxSet::ensureMaxOneDailyObs(int regHour)
{
//should be one day that satisfies query for this set
	//have > 1 DailyObs set to 1, need to pick one and change the rest
	int recToKeep = 0;
	bool hasTypeO = false;
	bool hasregHour = false;
	int numTypeO = 0;
	int recno = 1;
	bool hasSOW = false;
	while (!IsEOF())
	{
		if (m_ObsType[0] == 'O')
		{
			numTypeO++;
			if(numTypeO == 1 || m_ObsDate.GetHour() == regHour || !IsFieldNull(&m_SOW))
				recToKeep = recno;
		}
		else if (m_ObsDate.GetHour() == regHour)
		{
			hasregHour = true;
			if (numTypeO == 0)
				recToKeep = recno;
		}
		else if (!IsFieldNull(&m_SOW))
		{
			hasSOW = true;
			if (numTypeO == 0)
				recToKeep = recno;
		}
		recno++;
		MoveNext();
	}
	Requery();
	recno = 1;
	while (!IsEOF())
	{
		if (recno != recToKeep)
		{
			Edit();
			m_DailyObs = 0;
			Update();
		}
		if (recToKeep == 0)//decision algorithm failed...
		{
			if (recno == 1)
			{
				Edit();
				m_DailyObs = 1;
				Update();
			}
		}
		recno++;
		MoveNext();
	}
}

//gets queue of previous 24 hourly precip values (for calculation of 24hour precip from hourly values
queue<double> CWxSet::GetPrevious24HourlyPrecip(CString station, COleDateTime targetDate)
{
	queue<double> ret;
	COleDateTimeSpan day, hour;
	day.SetDateTimeSpan(1, 0, 0, 0);
	hour.SetDateTimeSpan(0, 1, 0, 0);
	COleDateTime prevDay = targetDate - day, targetDay;
	CWxSet pcpSet(m_pDatabase);
	pcpSet.m_strFilter.Format("[StationID] = '%6.6s' AND [ObsDate] >= #%s# AND [ObsDate] < #%s#", station,
		prevDay.Format(), targetDate.Format());
	pcpSet.m_strSort = _T("[ObsDate]");
	//pcpSet.Open();
	BOOL pcpStatus = pcpSet.Open();
	int startHour = prevDay.GetHour();
	int startDay = prevDay.GetDay();
	for (int i = 0; i < 24; i++)
	{
		bool hourAdded = false;
		pcpSet.Requery();
		while (!pcpSet.IsEOF() && !hourAdded)
		{
			if (!pcpSet.IsFieldNull(&pcpSet.m_HourlyPrecip))
			{
				hour.SetDateTimeSpan(0, i, 0, 0);
				targetDay = prevDay + hour;
				if (pcpSet.m_ObsDate.GetDay() == targetDay.GetDay() && pcpSet.m_ObsDate.GetHour() == targetDay.GetHour())
				{
					ret.push(pcpSet.m_HourlyPrecip);
					hourAdded = true;
				}
			}
			pcpSet.MoveNext();
		}
		if (!hourAdded)
			ret.push(0.0);
	}
	pcpSet.Close();
	return ret;
}

//gets queue of previous 24 hourly 24hour precip values (for calculation of hourly precip from 24hour precip)
std::queue<double> CWxSet::GetPrevious24DailyPrecip(CString station, COleDateTime targetDate)
{
	queue<double> ret;
	COleDateTimeSpan day, hour;
	day.SetDateTimeSpan(1, 0, 0, 0);
	hour.SetDateTimeSpan(0, 1, 0, 0);
	COleDateTime prevDay = targetDate - day, targetDay;
	CWxSet pcpSet(m_pDatabase);
	pcpSet.m_strFilter.Format("[StationID] = '%6.6s' AND [ObsDate] >= #%s# AND [ObsDate] < #%s#", station,
		prevDay.Format(), targetDate.Format());
	pcpSet.m_strSort = _T("[ObsDate]");
	BOOL pcpStatus = pcpSet.Open();
	//Requery();
	int startHour = prevDay.GetHour();
	int startDay = prevDay.GetDay();
	for (int i = 0; i < 24; i++)
	{
		bool hourAdded = false;
		pcpSet.Requery();
		while (!pcpSet.IsEOF() && !hourAdded)
		{
			if (!pcpSet.IsFieldNull(&pcpSet.m_PPTAMT))
			{
				hour.SetDateTimeSpan(0, i, 0, 0);
				targetDay = prevDay + hour;
				if (pcpSet.m_ObsDate.GetDay() == targetDay.GetDay() && pcpSet.m_ObsDate.GetHour() == targetDay.GetHour())
				{
					ret.push(pcpSet.m_PPTAMT);
					hourAdded = true;
				}
			}
			pcpSet.MoveNext();
		}
		if (!hourAdded)
			ret.push(0.0);
	}
	pcpSet.Close();
	return ret;
}


//recalc values for MaxTemp, MinTemp, MaxRH, MinRH, PPTAMT/PPTDUR/HourlyPrecip
// should be run on hourly data only

void CWxSet::RecalcRunning24()
{
   // arrays to hold last 24 values
   int temp[24];
   int RH[24];
   float precip[24];
   int pptdur[24];
   float pptamt[24];

   int records = 0;

   for (int i=0;i<24;i++){
        temp[i] = -9999;
		RH[i] = -9999;
		precip[i] = 0.0;
		pptdur[i] = 0;
		pptamt[i] = 0.0;
   }
   int prevpptdur = 0;
   float prevpptamt = 0.0;

   int MinTemp = 9999;
   int MaxTemp = -9999;

   int MinRH = 9999;
   int MaxRH = -9999;

   if (! IsOpen())
       return;
   if (IsEOF())
	   return;

   COleDateTime thisdate, lastdate;

   MoveFirst();
   if (IsEOF())
	   return;

   // establish starting values

   if (! IsFieldNull(&m_TmpMin))
	   MinTemp = m_TmpMin;
   if (! IsFieldNull(&m_TmpMax))
	   MaxTemp = m_TmpMax;

   if (! IsFieldNull(&m_RHMin))
	   MinRH = max(m_RHMin, 1);
   if (! IsFieldNull(&m_RHMax))
	   MaxRH = m_RHMax;

   if (! IsFieldNull(&m_PPTDUR)){
	   pptdur[23] = m_PPTDUR;
	   if (! IsFieldNull(&m_PPTAMT)){
		  pptamt[23] = m_PPTAMT;
	   }
   }

   while (! IsEOF()){
	   // 'shift' is the difference (in hours) between this obs and the last obs.
	   // for normal hourly data, shift = 1..... for daily data, shift = 24

        thisdate = m_ObsDate;

	    int shift = 1;

		int i = 0;

		// shift values back
		// e.g. if the current record is 11AM, the 10AM temperature becomes temp[22], the 9AM temp becomes temp[21], and so on...

		if (records > 0){
           COleDateTimeSpan difference;
           difference = thisdate - lastdate;
           shift = (int) (difference.GetTotalHours());
		   // any difference greater than 24 hours can be treated like 24 hours
		   if (shift > 24)
			   shift = 24;
           prevpptdur = 0;
		   prevpptamt = 0;
		   if (shift < 24){
			   prevpptdur = pptdur[0];
			   prevpptamt = pptamt[0];

	          for (i=0;i<23 && ((i + shift) < 24);i++){
                  temp[i] = temp[i+shift];
			      RH[i] = RH[i+ shift];
			      precip[i] = precip[i+shift];
				  pptdur[i] = pptdur[i+shift];
				  pptamt[i] = pptamt[i+shift];
	          }
		   }
		}

        while (i < 24){
           temp[i] = -9999;
		   RH[i] = -9999;
		   precip[i] = 0.0;
		   pptdur[i] = 0.0;
           i++;
	    }

	    if (! IsFieldNull(&m_Temp))
			   temp[23] = m_Temp;
		else
			   temp[23] = -9999;

		if (! IsFieldNull(&m_RH))
			   RH[23] = max(m_RH, 1);
		else
			   RH[23] = -9999;

		if (!IsFieldNull(&m_PPTDUR)){
			 pptdur[23] = m_PPTDUR;
		     if (!IsFieldNull(&m_PPTAMT))
				 pptamt[23] = m_PPTAMT;
		}

		if (IsFieldNull(&m_HourlyPrecip)){
			 // we don't have an HourlyPrecip value, so we need to calculate hourly precip from PPTAMT and PPTDUR
			 // note that we need at least 24 hours of data to be able to do this......
 			   precip[23] = 0;
			   if ((shift + records) >= 24){
				   if ((pptdur[23] - pptdur[22]) > 0){
					   precip[23] = (pptamt[23]-pptamt[22]);
				       if (pptdur[0] - prevpptdur > 0)
                          precip[23] += (pptamt[0] - prevpptamt);
			       }
			   }
		} else {//if (!IsFieldNull(&m_HourlyPrecip)){
			   // have HourlyPrecip value.... calculate PPTAMT/PPTDUR if necessary
			   precip[23] = m_HourlyPrecip;
               pptdur[23] = 0;
			   pptamt[23] = 0;

			   if (records + shift >= 24)
				   for (int j=0;j<24;j++){
                       if (precip[j] > 0.0)
			                pptdur[23] ++;
		                pptamt[23] += precip[j];
			       }
		}

	   // now find new mins/maxes
	   if (records > 0){
		   if ((records + shift < 24) && ! IsFieldNull(&m_TmpMin) && m_TmpMin != -9999)
		      MinTemp = m_TmpMin;
		   else
              MinTemp = temp[23];

		   if ((records + shift < 24) && ! IsFieldNull(&m_TmpMax) && m_TmpMax != -9999)
		      MinTemp = m_TmpMax;
		   else
		      MaxTemp = temp[23];

		   if ((records + shift < 24) && ! IsFieldNull(&m_RHMin) && m_RHMin != -9999)
		      MinRH = max(m_RHMin, 1);
		   else
		      MinRH = RH[23];

		   if ((records + shift < 24) && ! IsFieldNull(&m_RHMax) && m_RHMax != -9999)
		      MaxRH = m_RHMax;
		   else
		      MaxRH = RH[23];
	   }
        // compute last 24 running //
	   for (int j=0;j<24;j++){
		   if (temp[j] != -9999){
               if (temp[j] < MinTemp)
				   MinTemp = temp[j];
			   if (temp[j] > MaxTemp)
                   MaxTemp = temp[j];
		   }
		   if (RH[j] != -9999){
               if (RH[j] < MinRH)
				   MinRH = RH[j];
			   if (RH[j] > MaxRH)
                   MaxRH = RH[j];
		   }
	   }

	   // don't update type 'O' records if this is  (apparently) daily data
	   if (! (shift > 20 && m_ObsType.GetAt(0) == 'O')){
	      Edit();
	      // now update values
	      if (MinTemp != -9999)
		      m_TmpMin = MinTemp;
	      if (MaxTemp != -9999)
		      m_TmpMax = MaxTemp;

	      if (MinRH != -9999)
		      m_RHMin = max(MinRH, 1);
	      if (MaxRH != -9999)
		      m_RHMax = MaxRH;

		  /* only update existing precip dur/amt if we're past the first 24 hours */
		  //changed || to && for the update conditions (was almost always overwriting m_PPTAMT) SB 7/31/2012
		  if (! IsFieldNull(&m_HourlyPrecip))// && m_ObsType.CompareNoCase("O") != 0){
		  {
               if (IsFieldNull(&m_PPTAMT) && ((shift + records) > 24))
				   m_PPTAMT = pptamt[23];
			   if (IsFieldNull(&m_PPTDUR) && ((shift + records) > 24))
				   m_PPTDUR = pptdur[23];
		  } else   // update HourlyPrecip with newly computed value (based on PPTAMT & PPTDUR)
			  m_HourlyPrecip = precip[23];

		  Update();
	   }

       records ++;

	   lastdate = m_ObsDate;

       MoveNext();
   }
}
/*void CWxSet::RecalcRunning24()
{
   // arrays to hold last 24 values
   int temp[24];
   int RH[24];
   float precip[24];
   int pptdur[24];
   float pptamt[24];

   int records = 0;

   for (int i=0;i<24;i++){
        temp[i] = -9999;
		RH[i] = -9999;
		precip[i] = 0.0;
		pptdur[i] = 0;
		pptamt[i] = 0.0;
   }
   int prevpptdur = 0;
   float prevpptamt = 0.0;

   int MinTemp = 9999;
   int MaxTemp = -9999;

   int MinRH = 9999;
   int MaxRH = -9999;

   if (! IsOpen())
       return;
   if (IsEOF())
	   return;

   COleDateTime thisdate, lastdate;

   MoveFirst();
   if (IsEOF())
	   return;

   // establish starting values

   if (! IsFieldNull(&m_TmpMin))
	   MinTemp = m_TmpMin;
   if (! IsFieldNull(&m_TmpMax))
	   MaxTemp = m_TmpMax;

   if (! IsFieldNull(&m_RHMin))
	   MinRH = m_RHMin;
   if (! IsFieldNull(&m_RHMax))
	   MaxRH = m_RHMax;

   if (! IsFieldNull(&m_PPTDUR)){
	   pptdur[23] = m_PPTDUR;
	   if (! IsFieldNull(&m_PPTAMT)){
		  pptamt[23] = m_PPTAMT;
	   }
   }

   while (! IsEOF()){
	   // 'shift' is the difference (in hours) between this obs and the last obs.
	   // for normal hourly data, shift = 1..... for daily data, shift = 24

        thisdate = m_ObsDate;

	    int shift = 1;

		int i = 0;

		// shift values back
		// e.g. if the current record is 11AM, the 10AM temperature becomes temp[22], the 9AM temp becomes temp[21], and so on...

		if (records > 0){
           COleDateTimeSpan difference;
           difference = thisdate - lastdate;
           shift = (int) (difference.GetTotalHours());
		   // any difference greater than 24 hours can be treated like 24 hours
		   if (shift > 24)
			   shift = 24;
           prevpptdur = 0;
		   prevpptamt = 0;
		   if (shift < 24){
			   prevpptdur = pptdur[0];
			   prevpptamt = pptamt[0];

	          for (i=0;i<23 && ((i + shift) < 24);i++){
                  temp[i] = temp[i+shift];
			      RH[i] = RH[i+ shift];
			      precip[i] = precip[i+shift];
				  pptdur[i] = pptdur[i+shift];
				  pptamt[i] = pptamt[i+shift];
	          }
		   }
		}

        while (i < 24){
           temp[i] = -9999;
		   RH[i] = -9999;
		   precip[i] = 0.0;
		   pptdur[i] = 0.0;
           i++;
	    }

	    if (! IsFieldNull(&m_Temp))
			   temp[23] = m_Temp;
		else
			   temp[23] = -9999;

		if (! IsFieldNull(&m_RH))
			   RH[23] = m_RH;
		else
			   RH[23] = -9999;

		if (!IsFieldNull(&m_PPTDUR))
			pptdur[23] = m_PPTDUR;
		if (!IsFieldNull(&m_PPTAMT))
			pptamt[23] = m_PPTAMT;

		if (IsFieldNull(&m_HourlyPrecip)){
			 // we don't have an HourlyPrecip value, so we need to calculate hourly precip from PPTAMT and PPTDUR
			 // note that we need at least 24 hours of data to be able to do this......
 			   precip[23] = 0;
			   if ((shift + records) >= 24){
				   if ((pptdur[23] - pptdur[22]) > 0)
				   {
					   precip[23] = (pptamt[23]-pptamt[22]);
				       if (pptdur[0] - prevpptdur > 0)
                          precip[23] += (pptamt[0] - prevpptamt);
			       }
			   }
		} else {//if (!IsFieldNull(&m_HourlyPrecip)){
			   // have HourlyPrecip value.... calculate PPTAMT/PPTDUR if necessary
			   precip[23] = m_HourlyPrecip;
               pptdur[23] = 0;
			   pptamt[23] = 0;

			   if (records + shift >= 24)
				   for (int j=0;j<24;j++){
                       if (precip[j] > 0.0)
			                pptdur[23] ++;
		                pptamt[23] += precip[j];
			       }
		}

	   // now find new mins/maxes
	   if (records > 0){
		   if ((records + shift < 24) && ! IsFieldNull(&m_TmpMin) && m_TmpMin != -9999)
		      MinTemp = m_TmpMin;
		   else
              MinTemp = temp[23];

		   if ((records + shift < 24) && ! IsFieldNull(&m_TmpMax) && m_TmpMax != -9999)
		      MinTemp = m_TmpMax;
		   else
		      MaxTemp = temp[23];

		   if ((records + shift < 24) && ! IsFieldNull(&m_RHMin) && m_RHMin != -9999)
		      MinRH = m_RHMin;
		   else
		      MinRH = RH[23];

		   if ((records + shift < 24) && ! IsFieldNull(&m_RHMax) && m_RHMax != -9999)
		      MaxRH = m_RHMax;
		   else
		      MaxRH = RH[23];
	   }
        // compute last 24 running //
	   for (int j=0;j<24;j++){
		   if (temp[j] != -9999){
               if (temp[j] < MinTemp)
				   MinTemp = temp[j];
			   if (temp[j] > MaxTemp)
                   MaxTemp = temp[j];
		   }
		   if (RH[j] != -9999){
               if (RH[j] < MinRH)
				   MinRH = RH[j];
			   if (RH[j] > MaxRH)
                   MaxRH = RH[j];
		   }
	   }

	   // don't update type 'O' records if this is  (apparently) daily data
	   if (! (shift > 20 && m_ObsType.GetAt(0) == 'O')){
	      Edit();
	      // now update values
	      if (MinTemp != -9999)
		      m_TmpMin = MinTemp;
	      if (MaxTemp != -9999)
		      m_TmpMax = MaxTemp;

	      if (MinRH != -9999)
		      m_RHMin = MinRH;
	      if (MaxRH != -9999)
		      m_RHMax = MaxRH;

		  // only update existing precip dur/amt if we're past the first 24 hours
		  if (! IsFieldNull(&m_HourlyPrecip)){
               if (IsFieldNull(&m_PPTAMT) || ((shift + records) > 24))
				   m_PPTAMT = pptamt[23];
			   if (IsFieldNull(&m_PPTDUR)|| ((shift + records) > 24))
				   m_PPTDUR = pptdur[23];
		  } else   // update HourlyPrecip with newly computed value (based on PPTAMT & PPTDUR)
			  m_HourlyPrecip = precip[23];

		  Update();
	   }

       records ++;

	   lastdate = m_ObsDate;

       MoveNext();
   }
}
*/
//Stats class implementation
int compare( const void *arg1, const void *arg2 )
{
   /* Compare the values as doubles: */
	if(*(double *)arg1 > *(double *)arg2)
		return 1;
	else if(*(double *)arg1 < *(double *)arg2)
		return -1;
	return 0;
}

Stats::Stats()
{
	mean = variance = ep = mode = 0.0;
	n = 0;
	vals = NULL;
	p2Loc = 0;
}

Stats::~Stats()
{
	CleanStorage();
}

void Stats::Accumulate(double in)
{
	n++;
	mean += in;

	valList.AddTail(in);
}

void Stats::Complete()
{
	if(n < 1)
	{
		mean = 0;
		variance = 0.0;
		vals = NULL;
		return;
	}
	mean /= n;
	vals = new double[n > 1 ? n : n + 1];
	for(int i = 0; i < n; i++)
	{
		double x = valList.RemoveTail();
		double v = x - mean;
		variance += v * v;
		ep += v;
		vals[i] = x;
	}
	if(n > 1)
		variance = (variance - (ep * ep) / n) / (n - 1);
	else
		variance = 0.0;
	//sort the data
	if(n > 0)
	{
		qsort( (void *)vals, (size_t)n, sizeof( double ), compare );
		CalcMode();
	}
}

double Stats::Mean()
{
	return mean;
}

long Stats::N()
{
	return n;
}

double Stats::StdDev()
{
	if(variance > 0.0)
		return sqrt(variance);
	return 0.0;
}

double Stats::Min()
{
	if(n > 0)
		return vals[0];
	return 0.0;
}

double Stats::Max()
{
	if(n > 0)
		return vals[n - 1];
	return 0.0;
}

double Stats::Median()
{
	if(n > 0)
	{
		if(n%2 != 0)//odd
			return vals[(n + 1) / 2 - 1];
		//even
		return vals[n/2 - 1];
	}
	return 0.0;
}

double Stats::Percentile(double percent)
{
	if(n > 0)
	{
		int loc = (int)(n * percent);
		loc = max(loc, 0);
		loc = min(loc, n - 1);
		return vals[loc];
	}
	return 0.0;
}

double Stats::PercentileFromValue(double value)
{
	if(n > 0)
	{
		if(value < vals[0])
			return 0.0;
		if(value >= vals[n - 1])
			return 1.0;
		//traverse from end searching for the value
		for(long loc = n - 1; loc >= 0; loc--)
		{
			if(vals[loc] <= value)
				return ((double) loc) / (double) n;
		}
	}
	return 0.0;
}

void Stats::Reset()
{
	mean = variance = ep = 0.0;
	n = 0;
	CleanStorage();
	p2Loc = 0;
}

void Stats::CleanStorage()
{
	if(vals)
		delete[] vals;
	vals = NULL;
}

Period::Period(const COleDateTime& _start, const COleDateTime& _end)
{
	start.SetDateTime(_start.GetYear(), _start.GetMonth(), _start.GetDay(), 13, 0, 0);
	end.SetDateTime(_end.GetYear(), _end.GetMonth(), _end.GetDay(), 13, 0, 0);
	yStats = NULL;
	years = end.GetYear() - start.GetYear() + 1;
	yStats = new Stats[years > 1 ? years : 2];
	baseYear = start.GetYear();
	n = 0;
	mean = stdDev = 0.0;
	min = max = -1;
}

Period::~Period()
{
	if(yStats)
		delete[] yStats;
}

void Period::Accumulate(const COleDateTime& yearDate, double in)
{
	int tYr = yearDate.GetYear();
	int yr = tYr - start.GetYear();
	/* ljs quick fix TODO */
	if (yr < 0)
		return;// yr = 0;

	yStats[yr].Accumulate(in);
	if(n == 0)
		min = max = in;
	else
	{
		min = min(min, in);
		max = max(max, in);
	}
	n++;
	mean += in;
	stdDev += in * in;
}

void Period::Complete()
{
	for(int i = 0; i < years; i++)
		yStats[i].Complete();
	if(n > 1)
		stdDev = sqrt(fabs((n * stdDev - mean * mean)) / (n * (n - 1)));
	else
		stdDev = 0;
	if(n > 0)
		mean /= n;
}

long Period::N()
{
	return n;
}

void Period::Outputstats(FILE *stream, double criticalPercentile, bool isGSI /*= false*/)
{
	int ycnt = 0;//count of data years
	int highi = 0, lowi = 0, minYeari = 0, maxYeari = 0;
	double highAvg = -10000.0, lowAvg = 10000.0, minVal = 10000.0, maxVal = -10000.0;
	Stats mins, maxs, sums;
	for(int i = 0; i < years; i++)
	{
		if(yStats[i].N() > 0)
		{
			ycnt++;
			if(yStats[i].Mean() > highAvg)
			{
				highAvg = yStats[i].Mean();
				highi = i;
			}
			if(yStats[i].Mean() < lowAvg)
			{
				lowAvg = yStats[i].Mean();
				lowi = i;
			}
			mins.Accumulate(yStats[i].Min());
			if(minVal > yStats[i].Min())
			{
				minVal = yStats[i].Min();
				minYeari = i;
			}
			maxs.Accumulate(yStats[i].Max());
			if(maxVal < yStats[i].Max())
			{
				maxVal = yStats[i].Max();
				maxYeari = i;
			}
			for(int v = 0; v < yStats[i].N(); v++)
				sums.Accumulate(yStats[i].vals[v]);
		}
	}
	mins.Complete();
	maxs.Complete();
	sums.Complete();
	fprintf(stream, " %3d ", ycnt);
	if(N() > 0)
	{
		if (isGSI)
		{
			fprintf(stream, " %5.2f %5.2f %5.2f %5.2f %4d  %5.2f %4d   "
				"  %5.2f %4d  %5.2f %5.2f %5.2f %5.2f %4d  %5.2f %5.2f %5.2f",
				mean, sums.StdDev(), sums.Percentile(criticalPercentile),
				highAvg, start.GetYear() + highi, lowAvg, start.GetYear() + lowi,
				maxs.Max(), start.GetYear() + maxYeari, maxs.Mean(), maxs.StdDev(), maxs.Median(),
				mins.Min(), start.GetYear() + minYeari, mins.Mean(), mins.StdDev(), mins.Median());
		}
		else
			fprintf(stream, " %5.1f  %5.1f  %5.1f  %5.1f  %4d  %5.1f  %4d   "
				"  %5.1f  %4d  %5.1f  %5.1f  %5.1f  %5.1f  %4d  %5.1f  %5.1f  %5.1f",
				mean, sums.StdDev(), sums.Percentile(criticalPercentile),
				highAvg, start.GetYear() + highi, lowAvg, start.GetYear() + lowi,
				maxs.Max(), start.GetYear() + maxYeari, maxs.Mean(), maxs.StdDev(), maxs.Median(),
				mins.Min(), start.GetYear() + minYeari, mins.Mean(), mins.StdDev(), mins.Median());
	}
	else
		fprintf(stream, "     NA                                                                                                              ");
}

void Period::OutputVPDstats(FILE *stream, double criticalPercentile)
{
	int ycnt = 0;//count of data years
	int highi = 0, lowi = 0, minYeari = 0, maxYeari = 0;
	double highAvg = -10000.0, lowAvg = 10000.0, minVal = 10000.0, maxVal = -10000.0;
	Stats mins, maxs, sums;
	for(int i = 0; i < years; i++)
	{
		if(yStats[i].N() > 0)
		{
			ycnt++;
			if(yStats[i].Mean() > highAvg)
			{
				highAvg = yStats[i].Mean();
				highi = i;
			}
			if(yStats[i].Mean() < lowAvg)
			{
				lowAvg = yStats[i].Mean();
				lowi = i;
			}
			mins.Accumulate(yStats[i].Min());
			if(minVal > yStats[i].Min())
			{
				minVal = yStats[i].Min();
				minYeari = i;
			}
			maxs.Accumulate(yStats[i].Max());
			if(maxVal < yStats[i].Max())
			{
				maxVal = yStats[i].Max();
				maxYeari = i;
			}
			for(int v = 0; v < yStats[i].N(); v++)
				sums.Accumulate(yStats[i].vals[v]);
		}
	}
	mins.Complete();
	maxs.Complete();
	sums.Complete();
	fprintf(stream, " %3d ", ycnt);
	if(N() > 0)
	{
		fprintf(stream, " %5.0f  %5.0f  %5.0f  %5.0f  %4d  %5.0f  %4d   "
			"  %5.0f  %4d  %5.0f  %5.0f  %5.0f  %5.0f  %4d  %5.0f  %5.0f  %5.0f",
			mean, sums.StdDev(), sums.Percentile(criticalPercentile),
			highAvg, start.GetYear() + highi, lowAvg, start.GetYear() + lowi,
			maxs.Max(), start.GetYear() + maxYeari, maxs.Mean(), maxs.StdDev(), maxs.Median(),
			mins.Min(), start.GetYear() + minYeari, mins.Mean(), mins.StdDev(), mins.Median());
	}
	else
		fprintf(stream, "     NA                                                                                                              ");
}

void Period::OutputPrecipStats(FILE * stream, double criticalPercentile)
{
	int highi = 0, lowi = 0, minYeari = 0, maxYeari = 0, nValidObs;
	double highAvg = -10000.0, lowAvg = 10000.0, minVal = 10000.0, maxVal = -10000.0;
	Stats maxs, sums;
	nValidObs = GetMinPrecipObs(start, end);
	for(int i = 0; i < years; i++)
	{
		if(yStats[i].N() > 0)
		{
			maxs.Accumulate(yStats[i].Max());
			if(maxVal < yStats[i].Max())
			{
				maxVal = yStats[i].Max();
				maxYeari = i;
			}
			if(yStats[i].N() >= nValidObs)
			{
				if(yStats[i].Mean() * yStats[i].N() > highAvg)
				{
					highAvg = yStats[i].Mean() * yStats[i].N();
					highi = i;
				}
				if(yStats[i].Mean() * yStats[i].N() < lowAvg)
				{
					lowAvg = yStats[i].Mean() * yStats[i].N();
					lowi = i;
				}
				sums.Accumulate(yStats[i].Mean() * yStats[i].N());
			}
		}
	}
	sums.Complete();
	maxs.Complete();
	fprintf(stream, " %3d ", sums.N());
	if(N() > 0)
	{
		fprintf(stream, " %6.3f  %6.3f  %6.3f  %5.2f  %4d  %5.2f  %4d   "
			"  %5.2f  %4d  %6.3f  %6.3f  %6.3f",
			sums.Mean(), sums.StdDev(), sums.Percentile(criticalPercentile),
			sums.N() > 0 ? highAvg : 0.0, sums.N() > 0 ? start.GetYear() + highi : 0,
			sums.N() > 0 ? lowAvg : 0.0, sums.N() > 0 ? start.GetYear() + lowi : 0,
			maxs.Max(), start.GetYear() + maxYeari, maxs.Mean(), maxs.StdDev(), maxs.Median());
	}
	else
		fprintf(stream, "     NA                                                                                  ");
}

double Period::Percentile(double percent)
{
	Stats sums;
	if(n <= 0)
		return 0.0;
	for(int i = 0; i < years; i++)
	{
		if(yStats[i].N() > 0)
		{
			for(int v = 0; v < yStats[i].N(); v++)
				sums.Accumulate(yStats[i].vals[v]);
		}
	}
	sums.Complete();
	return sums.Percentile(percent);
}

void Stats::CalcMode()
{
	//vals are sorted find out most frequent one
	if(n > 0)
	{
		double lastVal = vals[0];
		mode = vals[0];
		long count = 1, loc = 1, modeCount = 1;
		while(loc < n)
		{
			if( lastVal != vals[loc])
			{
				if(count > modeCount)
				{
					mode = lastVal;
					modeCount = count;
				}
				count = 1;
				lastVal = vals[loc];
			}
			else
				count++;
			loc++;
		}
		if(count > modeCount)
			mode = lastVal;
	}
}

/*WxSetOR::WxSetOR(CDatabase* pDatabase) : CWxSet(pDatabase)
{
}

WxSetOR::~WxSetOR()
{
}

void WxSetOR::GetNext()
{
	MoveNext();
	if(m_ObsType == 'O' || IsEOF())
		return;
	COleDateTime tDate = m_ObsDate;
	MoveNext();
	if(IsEOF())
	{
		MovePrev();
		return;
	}
	if(m_ObsDate.GetYear() == tDate.GetYear() && m_ObsDate.GetDayOfYear() == tDate.GetDayOfYear())
		return;
	MovePrev();
}*/

bool CWxSet::GetVariable(int varID, double *val)
{
	// TODO: Add your implementation code here.
	return 0.0;
}

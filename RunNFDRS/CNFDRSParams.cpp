#include "CNFDRSParams.h"

CGSIParams::CGSIParams()
{
	m_gsiMax = 1.0;
	m_gsiHerbGreenup = 0.5;
	m_gsiTminMin = -2.0;
	m_gsiTminMax = 5.0;
	m_gsiVpdMin = 900.0;
	m_gsiVpdMax = 4100.0;
	m_gsiDaylenMin = 36000.0;
	m_gsiDaylenMax = 39600.0;
	m_gsiAveragingPeriod = 21;
	m_useVPDAverage = false;
	m_numPrecipDays = 30;
	m_runningTotalPcpMin = 0.5;
	m_runningTotalPcpMax = 1.5;
	m_liveFuelMoistureMin = 30.0;
	m_liveFuelMoistureMax = 250.0;
}

CGSIParams::CGSIParams(const CGSIParams&rhs)
{
	m_gsiMax = rhs.m_gsiMax;
	m_gsiHerbGreenup = rhs.m_gsiHerbGreenup;
	m_gsiTminMin = rhs.m_gsiTminMin;
	m_gsiTminMax = rhs.m_gsiTminMax;
	m_gsiVpdMin = rhs.m_gsiVpdMin;
	m_gsiVpdMax = rhs.m_gsiVpdMax;
	m_gsiDaylenMin = rhs.m_gsiDaylenMin;
	m_gsiDaylenMax = rhs.m_gsiDaylenMax;
	m_gsiAveragingPeriod = rhs.m_gsiAveragingPeriod;
	m_useVPDAverage = rhs.m_useVPDAverage;
	m_numPrecipDays = rhs.m_numPrecipDays;
	m_runningTotalPcpMin = rhs.m_runningTotalPcpMin;
	m_runningTotalPcpMax = rhs.m_runningTotalPcpMax;
	m_liveFuelMoistureMin = rhs.m_liveFuelMoistureMin;
	m_liveFuelMoistureMax = rhs.m_liveFuelMoistureMax;
}

CGSIParams::~CGSIParams()
{
}


CNFDRSParams::CNFDRSParams()
{
	m_fuelModel = 'Y';
	m_maxSC = 5;
	m_latitude = 47.0;
	m_slopeClass = 1;
	m_avgAnnualPrecip = 13.2;
	m_loadTransfer = true;
	m_cure = true;
	m_isAnnual = true;
	m_kbdiThreshold = 100;
	m_startKBDI = 100;
	m_isHumid = false;
	m_obsHour = 13;
	m_timeZoneOffsetHours = 0;
	m_herbParams.setLiveFuelMoistureMin(30);
	m_herbParams.setLiveFuelMoistureMax(250);
	m_woodyParams.setLiveFuelMoistureMin(50);
	m_woodyParams.setLiveFuelMoistureMax(200);
}

CNFDRSParams::CNFDRSParams(const CNFDRSParams& rhs)
{
	m_fuelModel = rhs.m_fuelModel;
	m_maxSC = rhs.m_maxSC;
	m_latitude = rhs.m_latitude;
	m_slopeClass = rhs.m_slopeClass;
	m_avgAnnualPrecip = rhs.m_avgAnnualPrecip;
	m_loadTransfer = rhs.m_loadTransfer;
	m_cure = rhs.m_cure;
	m_isAnnual = rhs.m_isAnnual;
	m_kbdiThreshold = rhs.m_kbdiThreshold;
	m_startKBDI = rhs.m_startKBDI;
	m_isHumid = rhs.m_isHumid;
	m_obsHour = rhs.m_obsHour;
	m_timeZoneOffsetHours = rhs.m_timeZoneOffsetHours;
	m_gsiParams = rhs.m_gsiParams;
	m_herbParams = rhs.m_herbParams;
	m_woodyParams = rhs.m_woodyParams;
}

CNFDRSParams::~CNFDRSParams()
{

}

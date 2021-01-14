#include "NFDRSConfiguration.h"
#include <config4cpp/Configuration.h>
#include <config4cpp/SchemaValidator.h>
#include "NFDRSInitConfig.h"
#include <stdio.h>

using CONFIG4CPP_NAMESPACE::Configuration;
using CONFIG4CPP_NAMESPACE::ConfigurationException;
using CONFIG4CPP_NAMESPACE::SchemaValidator;

NFDRSConfigurationException::NFDRSConfigurationException(const char *str)
{
	m_str = new char[strlen(str) + 2];
	strcpy_s(m_str, strlen(str), str);

}

NFDRSConfigurationException::NFDRSConfigurationException(const NFDRSConfigurationException& other)
{
	m_str = new char[strlen(other.m_str) + 2];
	strcpy_s(m_str, strlen(other.m_str), other.m_str);
}

NFDRSConfigurationException::~NFDRSConfigurationException()
{
	if (m_str)
		delete[] m_str;
}

const char *NFDRSConfigurationException::c_str() const
{
	return m_str;
}

NFDRSConfiguration::NFDRSConfiguration()
{
	m_wantDiagnostics = false;
	m_cfg = Configuration::create();
}

NFDRSConfiguration::~NFDRSConfiguration()
{
	((Configuration *)m_cfg)->destroy();
}

void NFDRSConfiguration::parse(
	const char *	cfgInput,
	const char *	cfgScope/* = ""*/) throw (NFDRSConfigurationException)
{
	Configuration *		cfg = (Configuration*)m_cfg;
	SchemaValidator		sv;

	try
	{
		if (strcmp(cfgInput, "") != 0)
		{
			cfg->parse(cfgInput);
		}
		cfg->setFallbackConfiguration(Configuration::INPUT_STRING,
			NFDRSInitConfig::getString());

		//--------
		// Perform schema validation.
		//--------
		sv.wantDiagnostics(m_wantDiagnostics);
		sv.parseSchema(NFDRSInitConfig::getSchema());
		sv.validate(cfg->getFallbackConfiguration(), "", "",
			SchemaValidator::FORCE_REQUIRED);
		sv.validate(cfg, cfgScope, "");
		
		m_nfdrsParams.setAvgAnnualPrecip(cfg->lookupFloat(cfgScope, "avgAnnualPrecip"));
		const char *fm = cfg->lookupString(cfgScope, "fuelModel");
		if(strlen(fm) > 0)
			m_nfdrsParams.setFuelModel(fm[0]);
		m_nfdrsParams.setIsAnnual(cfg->lookupInt(cfgScope, "isAnnuals"));
		m_nfdrsParams.setIsHumid(cfg->lookupInt(cfgScope, "isHumid"));
		m_nfdrsParams.setKbdiThreshold(cfg->lookupInt(cfgScope, "kbdiThreshold"));
		m_nfdrsParams.setLatitude(cfg->lookupFloat(cfgScope, "latitude"));
		m_nfdrsParams.setMaxSC(cfg->lookupInt(cfgScope, "maxSC"));
		m_nfdrsParams.setObsHour(cfg->lookupInt(cfgScope, "obsHour"));
		m_nfdrsParams.setSlopeClass(cfg->lookupInt(cfgScope, "slopeClass"));
		m_nfdrsParams.setStartKbdi(cfg->lookupInt(cfgScope, "startKBDI"));
		m_nfdrsParams.setUseCure(cfg->lookupInt(cfgScope, "useCure"));
		m_nfdrsParams.setUseLoadTransfer(cfg->lookupInt(cfgScope, "useLoadTransfer"));
		m_nfdrsParams.setTimeZoneOffsetHours(cfg->lookupInt(cfgScope, "timeZoneOffset"));

		CGSIParams gsiParams;
		gsiParams.setGsiAveragingPeriod(cfg->lookupInt(cfgScope, "gsi_opts.gsiAveragingDays"));
		gsiParams.setGsiDaylenMax(cfg->lookupFloat(cfgScope, "gsi_opts.gsiDayLenMax"));
		gsiParams.setGsiDaylenMin(cfg->lookupFloat(cfgScope, "gsi_opts.gsiDayLenMin"));
		gsiParams.setGsiHerbGreenup(cfg->lookupFloat(cfgScope, "gsi_opts.gsiHerbGreenup"));
		gsiParams.setGsiMax(cfg->lookupFloat(cfgScope, "gsi_opts.gsiMax"));
		gsiParams.setGsiTminMax(cfg->lookupFloat(cfgScope, "gsi_opts.gsiTminMax"));
		gsiParams.setGsiTminMin(cfg->lookupFloat(cfgScope, "gsi_opts.gsiTminMin"));
		gsiParams.setGsiVpdMax(cfg->lookupFloat(cfgScope, "gsi_opts.gsiVpdMax"));
		gsiParams.setGsiVpdMin(cfg->lookupFloat(cfgScope, "gsi_opts.gsiVpdMin"));
		gsiParams.setLiveFuelMoistureMax(cfg->lookupFloat(cfgScope, "gsi_opts.fuelMoistureMax"));
		gsiParams.setLiveFuelMoistureMin(cfg->lookupFloat(cfgScope, "gsi_opts.fuelMoistureMin"));
		gsiParams.setNumPrecipDays(cfg->lookupInt(cfgScope, "gsi_opts.gsiNumPrecipDays"));
		gsiParams.setRunningTotalPrecipMax(cfg->lookupFloat(cfgScope, "gsi_opts.gsiRTPrecipMax"));
		gsiParams.setRunningTotalPrecipMin(cfg->lookupFloat(cfgScope, "gsi_opts.gsiRTPrecipMin"));
		gsiParams.setUseVpdAverage(cfg->lookupInt(cfgScope, "gsi_opts.gsiUseVpdAvg"));
		m_nfdrsParams.setGsiParams(gsiParams);
		CGSIParams herbParams;
		herbParams.setGsiAveragingPeriod(cfg->lookupInt(cfgScope, "herb_opts.gsiAveragingDays"));
		herbParams.setGsiDaylenMax(cfg->lookupFloat(cfgScope, "herb_opts.gsiDayLenMax"));
		herbParams.setGsiDaylenMin(cfg->lookupFloat(cfgScope, "herb_opts.gsiDayLenMin"));
		herbParams.setGsiHerbGreenup(cfg->lookupFloat(cfgScope, "herb_opts.gsiHerbGreenup"));
		herbParams.setGsiMax(cfg->lookupFloat(cfgScope, "herb_opts.gsiMax"));
		herbParams.setGsiTminMax(cfg->lookupFloat(cfgScope, "herb_opts.gsiTminMax"));
		herbParams.setGsiTminMin(cfg->lookupFloat(cfgScope, "herb_opts.gsiTminMin"));
		herbParams.setGsiVpdMax(cfg->lookupFloat(cfgScope, "herb_opts.gsiVpdMax"));
		herbParams.setGsiVpdMin(cfg->lookupFloat(cfgScope, "herb_opts.gsiVpdMin"));
		herbParams.setLiveFuelMoistureMax(cfg->lookupFloat(cfgScope, "herb_opts.fuelMoistureMax"));
		herbParams.setLiveFuelMoistureMin(cfg->lookupFloat(cfgScope, "herb_opts.fuelMoistureMin"));
		herbParams.setNumPrecipDays(cfg->lookupInt(cfgScope, "herb_opts.gsiNumPrecipDays"));
		herbParams.setRunningTotalPrecipMax(cfg->lookupFloat(cfgScope, "herb_opts.gsiRTPrecipMax"));
		herbParams.setRunningTotalPrecipMin(cfg->lookupFloat(cfgScope, "herb_opts.gsiRTPrecipMin"));
		herbParams.setUseVpdAverage(cfg->lookupInt(cfgScope, "herb_opts.gsiUseVpdAvg"));
		m_nfdrsParams.setHerbParams(herbParams);
		CGSIParams woodyParams;
		woodyParams.setGsiAveragingPeriod(cfg->lookupInt(cfgScope, "woody_opts.gsiAveragingDays"));
		woodyParams.setGsiDaylenMax(cfg->lookupFloat(cfgScope, "woody_opts.gsiDayLenMax"));
		woodyParams.setGsiDaylenMin(cfg->lookupFloat(cfgScope, "woody_opts.gsiDayLenMin"));
		woodyParams.setGsiHerbGreenup(cfg->lookupFloat(cfgScope, "woody_opts.gsiHerbGreenup"));
		woodyParams.setGsiMax(cfg->lookupFloat(cfgScope, "woody_opts.gsiMax"));
		woodyParams.setGsiTminMax(cfg->lookupFloat(cfgScope, "woody_opts.gsiTminMax"));
		woodyParams.setGsiTminMin(cfg->lookupFloat(cfgScope, "woody_opts.gsiTminMin"));
		woodyParams.setGsiVpdMax(cfg->lookupFloat(cfgScope, "woody_opts.gsiVpdMax"));
		woodyParams.setGsiVpdMin(cfg->lookupFloat(cfgScope, "woody_opts.gsiVpdMin"));
		woodyParams.setLiveFuelMoistureMax(cfg->lookupFloat(cfgScope, "woody_opts.fuelMoistureMax"));
		woodyParams.setLiveFuelMoistureMin(cfg->lookupFloat(cfgScope, "woody_opts.fuelMoistureMin"));
		woodyParams.setNumPrecipDays(cfg->lookupInt(cfgScope, "woody_opts.gsiNumPrecipDays"));
		woodyParams.setRunningTotalPrecipMax(cfg->lookupFloat(cfgScope, "woody_opts.gsiRTPrecipMax"));
		woodyParams.setRunningTotalPrecipMin(cfg->lookupFloat(cfgScope, "woody_opts.gsiRTPrecipMin"));
		woodyParams.setUseVpdAverage(cfg->lookupInt(cfgScope, "woody_opts.gsiUseVpdAvg"));
		m_nfdrsParams.setWoodyParams(woodyParams);
		//let's try dump to see what we get!
		//saveAs("Dummyfile.cfg");
	}
	catch (const ConfigurationException & ex) {
		//throw NFDRSConfigurationException(ex.c_str());
		//throw ConfigurationException(ex.c_str());
		printf("%s\n", ex.c_str());
	}
}

void NFDRSConfiguration::saveAs(const char *fileName)
{
	CONFIG4CPP_NAMESPACE::StringBuffer strBuf;
	Configuration *		cfg = (Configuration*)m_cfg;
	cfg->dump(strBuf, false);
	//printf("%s\n", strBuf.c_str());
}

// RunNFDRS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "nfdrs2016.h"
#include "RunNFDRSConfiguration.h"
#include "NFDRSConfiguration.h"
#include "CNFDRSParams.h"
#include "NFDR2016CalcState.h"
#include "CFW21Data.h"
#include <io.h>
#include <stdlib.h>
using namespace std;

string FormatToISO8061Offset(tm inTm, int offset)
{
	char buf[64];
	sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d%+03d:00", inTm.tm_year + 1900, inTm.tm_mon + 1,
		inTm.tm_mday, inTm.tm_hour, inTm.tm_min, inTm.tm_sec, offset);
	string ret = buf;
	return ret;
}

bool fileExists(const char *fileName)
{
	bool ret = false;
	if (_access(fileName, 0) != -1)
		ret = true;
	return ret;
}

 
int main(int argc, char* argv[])
{
	//routine for testing NFDRS
	const char* nfdrsInitFileName = NULL;
	const char* wxFileName = NULL;
	const char* loadStateFileName = NULL;
	const char* saveStateFileName = NULL;
	const char* indexOutputsFileName = NULL;
	const char* allOutputsFileName = NULL;
	const char* fuelMoistureOutputsFileName = NULL;
	CNFDRSParams params;

	int	exitStatus = 0;
	if (argc < 2)
	{
		printf("RunNFDRS takes 1 parameter: A path to a RunNFDRS configuration file.\n"
			"RunNFDRS <configFileName>\n"
		"\twhere configFileName is the complete path to a RunNFDRS configuration file\n\n");
		exit(1);
	}
	if (!fileExists(argv[1]))
	{
		printf("Error, file %s does not exist!\n", argv[1]);
		exit(-1);
	}
	RunNFDRSConfiguration *cfg = new RunNFDRSConfiguration();
	try
	{
		cfg->parse(argv[1]);
		nfdrsInitFileName = cfg->getInitFile();
		wxFileName = cfg->getWxFile();
		loadStateFileName = cfg->getLoadStateFile();
		saveStateFileName = cfg->getSaveStateFile();
		allOutputsFileName = cfg->getAllOutputsFile();
		indexOutputsFileName = cfg->getIndexOutputFile();
		fuelMoistureOutputsFileName = cfg->getFuelMoisturesOutputsFile();
		//delete cfg;
	}
	catch(const RunNFDRSConfigurationException & ex)
	{
		fprintf(stderr, "%s\n", ex.c_str());
		delete cfg;
		return -1;
	}

	if (!fileExists(nfdrsInitFileName) && !fileExists(loadStateFileName))
	{
		if(strlen(nfdrsInitFileName) == 0 && strlen(loadStateFileName) == 0)
			printf("Error, either an NFDRSInit file or a loadStateFile must be specified.\n");
		else
		{
			if (strlen(nfdrsInitFileName) > 0)
				printf("NFDRS Init file %s does not exist!\n", nfdrsInitFileName);
			if (strlen(loadStateFileName) > 0)
				printf("NFDRS State file %s does not exist!\n", loadStateFileName);
		}
		delete cfg;
		return -1;
	}

	if (strlen(wxFileName) == 0)
	{
		printf("A wxFile must be specified in the RunNFDRS Configuration file\n");
		delete cfg;
		return -3;
	}
	//everything else is optional...
	//first, try to open and parse the NFDRSInit file

	NFDRSConfiguration *nfdrsCfg = new NFDRSConfiguration();
	if (fileExists(nfdrsInitFileName))
	{
		try
		{
			//NFDRSConfiguration *nfdrsCfg = new NFDRSConfiguration();
			nfdrsCfg->parse(nfdrsInitFileName);
			params = nfdrsCfg->getNFDRSParams();
			//delete nfdrsCfg;
		}
		catch (NFDRSConfigurationException & ex)
		{
			fprintf(stderr, "%s\n", ex.c_str());
			delete nfdrsCfg;
			delete cfg;
			return -4;
		}
	}
	//at this point we should have everything we need
	NFDR2016Calc thisCalc;
	//use NFDRSParams to initialize NFDR2016Calc object
	params.InitNFDRS(&thisCalc);

	//do we have a state file?
	if (strlen(loadStateFileName) > 0)
	{
		NFDR2016CalcState state;
		state.LoadState(loadStateFileName);
		thisCalc.LoadState(state);
	}
	CFW21Data FW21data;
	int status = FW21data.LoadFile(wxFileName, params.getTimeZoneOffsetHours());
	if (status != 0)
	{
		printf("Error loading %s as FW21 file\n", wxFileName);
		delete nfdrsCfg;
		delete cfg;
		return -5;
	}
	//also need any output files for dumping data
	FILE *allOut = NULL, *indexOut = NULL, *moistOut = NULL, *wxAllOut = NULL;
	if (allOutputsFileName && strlen(allOutputsFileName) > 0)
	{
		bool allExists = fileExists(allOutputsFileName);
		allOut = fopen(allOutputsFileName, "at");
		if (!allOut)
		{
			printf("Error opening %s as output.\n", allOutputsFileName);
			delete nfdrsCfg;
			delete cfg;
			return -3;
		}
		if(!allExists)
			fprintf(allOut, "DateTime, Temp, RH, Precip, WindSpeed, SolarRadiation, SnowFlag, MinTemp, MaxTemp, MinRH, Pcp24, 1HourDFM, 10HourDFM, 100HourDFM, 1000HourDFM, HerbLFM, WoodyLFM, BI, ERC, SC, IC, GSI, KBDI\n");
	}
	if (indexOutputsFileName && strlen(indexOutputsFileName) > 0)
	{
		bool exists = fileExists(allOutputsFileName);
		indexOut = fopen(indexOutputsFileName, "at");
		if (!indexOut)
		{
			
			printf("Error opening %s as output.\n", indexOutputsFileName);
			if (allOut)
				fclose(allOut);
			delete nfdrsCfg;
			delete cfg;
			return -3;
		}
		if (!exists)
			fprintf(indexOut, "DateTime, BI, ERC, SC, IC, GSI, KBDI\n");
	}
	if (fuelMoistureOutputsFileName && strlen(fuelMoistureOutputsFileName) > 0)
	{
		bool fExists = fileExists(fuelMoistureOutputsFileName);
		moistOut = fopen(fuelMoistureOutputsFileName, "at");
		if (!moistOut)
		{

			printf("Error opening %s as output.\n", fuelMoistureOutputsFileName);
			if (allOut)
				fclose(allOut);
			if (indexOut)
				fclose(indexOut);
			delete nfdrsCfg;
			delete cfg;
			return -3;
		}
		if(!fExists)
			fprintf(moistOut, "DateTime, 1HourDFM, 10HourDFM, 100HourDFM, 1000HourDFM, HerbLFM, WoodyLFM\n");
	}

	//now need to read the wxFile and process the records
	time_t startTime = clock();
	for (size_t r = 0; r < FW21data.GetNumRecs(); r++)
	{
		NFDRSRec thisRec = FW21data.GetNFDRSRec(r);
		thisCalc.Update(thisRec.GetYear(), thisRec.GetMonth(), thisRec.GetDay(), thisRec.GetHour(), thisRec.GetDateTime().tm_yday, thisRec.GetTemp(),
			thisRec.GetMinTemp(), thisRec.GetMaxTemp(), thisRec.GetRH(), thisRec.GetMinRH(), thisRec.GetPrecip(), thisRec.GetPcp24(),
			thisRec.GetSolarRadiation(), thisRec.GetWindSpeed(), thisRec.GetSnowFlag(), params.getObsHour());
		if (cfg->getOutputInterval() == 0 || (cfg->getOutputInterval() == 1 && thisRec.GetHour() == params.getObsHour()))
		{
			//output to open csv files
			if (allOut)
			{
				fprintf(allOut, "%s, %.1f, %.1f, %.3f, %.1f, %.0f, %d, %.1f, %.1f, %.1f, %.3f, "
					"%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %d\n", 
					FormatToISO8061Offset(thisRec.GetDateTime(), params.getTimeZoneOffsetHours()).c_str(),
					thisRec.GetTemp(), thisRec.GetRH(), thisRec.GetPrecip(), thisRec.GetWindSpeed(), thisRec.GetSolarRadiation(), 
					thisRec.GetSnowFlag(), thisRec.GetMinTemp(), thisRec.GetMaxTemp(), thisRec.GetMinRH(), thisRec.GetPcp24(),
					thisCalc.MC1, thisCalc.MC10, thisCalc.MC100, thisCalc.MC1000, thisCalc.MCHERB, thisCalc.MCWOOD,
					thisCalc.BI, thisCalc.ERC, thisCalc.SC, thisCalc.IC, thisCalc.m_GSI, thisCalc.KBDI);
			}
			if (indexOut)
			{
				fprintf(indexOut, "%s, %.2f, %.2f, %.2f, %.2f, %.2f, %d\n",
					FormatToISO8061Offset(thisRec.GetDateTime(), params.getTimeZoneOffsetHours()).c_str(),
					thisCalc.BI, thisCalc.ERC, thisCalc.SC, thisCalc.IC, thisCalc.m_GSI, thisCalc.KBDI);
			}
			if (moistOut)
			{
				fprintf(moistOut, "%s, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",
					FormatToISO8061Offset(thisRec.GetDateTime(), params.getTimeZoneOffsetHours()).c_str(),
					thisCalc.MC1, thisCalc.MC10, thisCalc.MC100, thisCalc.MC1000, thisCalc.MCHERB, thisCalc.MCWOOD);
			}
		}
	}
	time_t endTime = clock();
	int total = endTime - startTime;
	printf("Total milliseconds time for NFDRS: %d\n", total);
	if (strlen(saveStateFileName) > 0)
	{
		bool success = thisCalc.SaveState(saveStateFileName);
		if (!success)
			printf("Error saving %s as NFDRS State file\n", saveStateFileName);
	}
	if (allOut)
		fclose(allOut);
	if (indexOut)
		fclose(indexOut);
	if (moistOut)
		fclose(moistOut);
	delete nfdrsCfg;
	delete cfg;
	return exitStatus;
}


// NFDRSCalculator2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NFDRSCalculator2.h"
#include "NFDRSCalculator2Dlg.h"
//#include "CDNFDRS4.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/// is this an NFDRS2016 fuel model?
bool isNFDRS2016(char fm)
{
	if (fm == 'V' || fm == 'W' || fm == 'X' || fm == 'Y' || fm == 'Z'
		|| fm == 'v' || fm == 'w' || fm == 'x' || fm == 'y' || fm == 'z')
		return true;
	return false;
}


// CNFDRSCalculator2App

BEGIN_MESSAGE_MAP(CNFDRSCalculator2App, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CNFDRSCalculator2App construction

CNFDRSCalculator2App::CNFDRSCalculator2App() : CWinApp()
,  m_buildDateStr(_T(__DATE__))
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	pOptions;

}


// The one and only CNFDRSCalculator2App object

CNFDRSCalculator2App theApp;


// CNFDRSCalculator2App initialization

BOOL CNFDRSCalculator2App::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	char wd[256];
	workDir.Format("%s\\", getcwd(wd, 255));
	pOptions = new CalcOptions();
	LoadConfig();
	CNFDRSCalculator2Dlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CNFDRSCalculator2App::ExitInstance()
{
	SaveConfig();
	if(pOptions)
		delete pOptions;
	return CWinApp::ExitInstance();
}


void getIntVal(FILE *in, int *val)
{
	char buf[64];
	fgets(buf, 63, in);
	*val = atoi(buf);
}

void getCharVal(FILE *in, char *val)
{
	char buf[64];
	fgets(buf, 63, in);
	*val = buf[0];
}

void getFloatVal(FILE *in, float *val)
{
	char buf[64];
	fgets(buf, 63, in);
	*val = atof(buf);
}


int CNFDRSCalculator2App::LoadConfig()
{
	CString cfgName;
	cfgName.Format("%sNFDRCalc.cfg", workDir);
	FILE *cfg = fopen(cfgName, "rt");
	if(cfg)
	{
		char buf[64];
		getCharVal(cfg, &pOptions->fuelModel);
		getIntVal(cfg, &pOptions->slopeClass);
		getIntVal(cfg, &pOptions->use88);
		getIntVal(cfg, &pOptions->temp);
		getIntVal(cfg, &pOptions->windSpeed);
		getIntVal(cfg, &pOptions->SOW);
		getIntVal(cfg, &pOptions->fm1);
		getIntVal(cfg, &pOptions->fm10);
		getIntVal(cfg, &pOptions->fm100);
		getIntVal(cfg, &pOptions->fm1000);
		getIntVal(cfg, &pOptions->herb);
		getIntVal(cfg, &pOptions->woody);
		getIntVal(cfg, &pOptions->season);
		getIntVal(cfg, &pOptions->woodyGreen);
		getIntVal(cfg, &pOptions->KBDI);
		getIntVal(cfg, &pOptions->rainEvent);
		getIntVal(cfg, &pOptions->deciduousShrubs);
		getIntVal(cfg, &pOptions->prevKBDI);
		getFloatVal(cfg, &pOptions->currRain);
		getIntVal(cfg, &pOptions->maxTemp);
		getFloatVal(cfg, &pOptions->cummPrecip);
		getFloatVal(cfg, &pOptions->avgPrecip);
		getIntVal(cfg, &pOptions->nModels);
		if(pOptions->nModels > 0)
		pOptions->models = new char[pOptions->nModels];
		pOptions->use88s = new int[pOptions->nModels];
		for(int i = 0; i < pOptions->nModels; i++)
		{
			fgets(buf, 64, cfg);
			sscanf(buf, "%c %d", &pOptions->models[i], &pOptions->use88s[i]);
		}

		fclose(cfg);
		return 1;
	}
	return 0;
}


int CNFDRSCalculator2App::SaveConfig()
{
	if(!pOptions)
		return 0;
	CString cfgName;
	cfgName.Format("%sNFDRCalc.cfg", workDir);
	FILE *cfg = fopen(cfgName, "wt");
	//fprintf(cfg, "%lf", 
	if(cfg)
	{
		fprintf(cfg, "%c\n", pOptions->fuelModel);
		fprintf(cfg, "%d\n", pOptions->slopeClass);
		fprintf(cfg, "%d\n", pOptions->use88);
		fprintf(cfg, "%d\n", pOptions->temp);
		fprintf(cfg, "%d\n", pOptions->windSpeed);
		fprintf(cfg, "%d\n", pOptions->SOW);
		fprintf(cfg, "%d\n", pOptions->fm1);
		fprintf(cfg, "%d\n", pOptions->fm10);
		fprintf(cfg, "%d\n", pOptions->fm100);
		fprintf(cfg, "%d\n", pOptions->fm1000);
		fprintf(cfg, "%d\n", pOptions->herb);
		fprintf(cfg, "%d\n", pOptions->woody);
		fprintf(cfg, "%d\n", pOptions->season);
		fprintf(cfg, "%d\n", pOptions->woodyGreen);
		fprintf(cfg, "%d\n", pOptions->KBDI);
		fprintf(cfg, "%d\n", pOptions->rainEvent);
		fprintf(cfg, "%d\n", pOptions->deciduousShrubs);
		fprintf(cfg, "%d\n", pOptions->prevKBDI);
		fprintf(cfg, "%.2f\n", pOptions->currRain);
		fprintf(cfg, "%d\n", pOptions->maxTemp);
		fprintf(cfg, "%.2f\n", pOptions->cummPrecip);
		fprintf(cfg, "%.2f\n", pOptions->avgPrecip);
		fprintf(cfg, "%d\n", pOptions->nModels);
		for(int i = 0; i < pOptions->nModels; i++)
			fprintf(cfg, "%c %d\n", pOptions->models[i], pOptions->use88s[i]);

		fclose(cfg);
		return 1;
	}
	return 0;
}

CalcOptions::CalcOptions()
{
	models = NULL;
	use88s = NULL;
	nModels = 0;
	fuelModel = 'G';
	slopeClass = 1;
	use88 = 0;
	temp = 80;
	windSpeed = 5;
	SOW = 1;
	fm1 = 4;
	fm10 = 5;
	fm100 = 10;
	fm1000 = 12;
	herb = 30;
	woody = 60;
	season = 1;
	woodyGreen = 5;
	KBDI = 100;
	rainEvent = 0;
	deciduousShrubs = 0;
	prevKBDI = 100;
	currRain = 0.0;
	maxTemp = 87;
	cummPrecip = 9.0;
	avgPrecip = 18.0;

}

CalcOptions::~CalcOptions()
{
	if(models)
		delete[] models;
	if(use88s)
		delete[] use88s;
}

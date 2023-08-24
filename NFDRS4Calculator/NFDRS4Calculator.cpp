
// NFDRS4Calculator.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "NFDRS4Calculator.h"
#include "NFDRS4CalculatorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNFDRS4CalculatorApp

BEGIN_MESSAGE_MAP(CNFDRS4CalculatorApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CNFDRS4CalculatorApp construction

CNFDRS4CalculatorApp::CNFDRS4CalculatorApp() : CWinApp()
, m_buildDateStr(_T(__DATE__))
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CNFDRS4CalculatorApp object

CNFDRS4CalculatorApp theApp;


// CNFDRS4CalculatorApp initialization

BOOL CNFDRS4CalculatorApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CNFDRS4CalculatorDlg dlg;
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
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CNFDRS4CalculatorApp::ExitInstance()
{
	SaveConfig();
	if (pOptions)
		delete pOptions;
	return CWinApp::ExitInstance();
}

void getCharVal(FILE* in, char* val)
{
	char buf[64];
	fgets(buf, 63, in);
	*val = buf[0];
}

void getFloatVal(FILE* in, float* val)
{
	char buf[64];
	fgets(buf, 63, in);
	*val = atof(buf);
}

void getIntVal(FILE* in, int* val)
{
	char buf[64];
	fgets(buf, 63, in);
	*val = atoi(buf);
}

int CNFDRS4CalculatorApp::LoadConfig()
{
	CString cfgName;
	cfgName.Format(_T("%sNFDRCalc.cfg"), workDir);
	char strFileName[MAX_PATH];
	strcpy_s(strFileName, CStringA(cfgName).GetBuffer());
	FILE* cfg = fopen(strFileName, "wt");
	if (cfg)
	{
		char buf[64];
		getCharVal(cfg, &pOptions->fuelModel);
		getIntVal(cfg, &pOptions->slopeClass);
		getIntVal(cfg, &pOptions->windSpeed);
		getIntVal(cfg, &pOptions->fm1);
		getIntVal(cfg, &pOptions->fm10);
		getIntVal(cfg, &pOptions->fm100);
		getIntVal(cfg, &pOptions->fm1000);
		getIntVal(cfg, &pOptions->herb);
		getIntVal(cfg, &pOptions->woody);
		getIntVal(cfg, &pOptions->KBDI);
		getIntVal(cfg, &pOptions->rainEvent);
		getIntVal(cfg, &pOptions->deciduousShrubs);
		getIntVal(cfg, &pOptions->prevKBDI);
		getFloatVal(cfg, &pOptions->currRain);
		getIntVal(cfg, &pOptions->maxTemp);
		getFloatVal(cfg, &pOptions->cummPrecip);
		getFloatVal(cfg, &pOptions->avgPrecip);
		int nModels;
		getIntVal(cfg, &nModels);
		char model;
		for (int i = 0; i < nModels; i++)
		{
			fgets(buf, 64, cfg);
			sscanf(buf, "%c", &model);
			pOptions->vModels.push_back(model);
		}

		fclose(cfg);
		return 1;
	}
	return 0;
}


int CNFDRS4CalculatorApp::SaveConfig()
{
	if (!pOptions)
		return 0;
	CString cfgName;
	cfgName.Format(_T("%sNFDRCalc.cfg"), workDir);
	char strFileName[MAX_PATH];
	strcpy_s(strFileName, CStringA(cfgName).GetBuffer());
	FILE* cfg = fopen(strFileName, "wt");
	//fprintf(cfg, "%lf", 
	if (cfg)
	{
		fprintf(cfg, "%c\n", pOptions->fuelModel);
		fprintf(cfg, "%d\n", pOptions->slopeClass);
		fprintf(cfg, "%d\n", pOptions->windSpeed);
		fprintf(cfg, "%d\n", pOptions->fm1);
		fprintf(cfg, "%d\n", pOptions->fm10);
		fprintf(cfg, "%d\n", pOptions->fm100);
		fprintf(cfg, "%d\n", pOptions->fm1000);
		fprintf(cfg, "%d\n", pOptions->herb);
		fprintf(cfg, "%d\n", pOptions->woody);
		fprintf(cfg, "%d\n", pOptions->KBDI);
		fprintf(cfg, "%d\n", pOptions->rainEvent);
		fprintf(cfg, "%d\n", pOptions->deciduousShrubs);
		fprintf(cfg, "%d\n", pOptions->prevKBDI);
		fprintf(cfg, "%.2f\n", pOptions->currRain);
		fprintf(cfg, "%d\n", pOptions->maxTemp);
		fprintf(cfg, "%.2f\n", pOptions->cummPrecip);
		fprintf(cfg, "%.2f\n", pOptions->avgPrecip);
		int nModels = pOptions->vModels.size();
		fprintf(cfg, "%d\n", nModels);
		for (auto it = pOptions->vModels.begin(); it != pOptions->vModels.end(); ++it)
			fprintf(cfg, "%c\n", (*it));

		fclose(cfg);
		return 1;
	}
	return 0;
}

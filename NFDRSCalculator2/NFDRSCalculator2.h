// NFDRSCalculator2.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <commctrl.h>

class CalcOptions
{
public:
	CalcOptions();
	~CalcOptions();

	char fuelModel;
	int slopeClass;
	int use88;
	int temp;
	int windSpeed;
	int SOW;
	int fm1;
	int fm10;
	int fm100;
	int fm1000;
	int herb;
	int woody;
	int season;
	int woodyGreen;
	int KBDI;
	int rainEvent;
	int deciduousShrubs;
	int prevKBDI;
	float currRain;
	int maxTemp;
	float cummPrecip;
	float avgPrecip;
	//fuel models...
	int nModels;
	char *models;
	int *use88s;

};// CNFDRSCalculator2App:
// See NFDRSCalculator2.cpp for the implementation of this class
//

class CNFDRSCalculator2App : public CWinApp
{
public:
	CNFDRSCalculator2App();
	CalcOptions *pOptions;
	CString m_buildDateStr;
	// Overrides
	public:
	virtual BOOL InitInstance();
	int LoadConfig();
	int SaveConfig();
// Implementation
	CString workDir;
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

bool isNFDRS2016(char fm);

extern CNFDRSCalculator2App theApp;


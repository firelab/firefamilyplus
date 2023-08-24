
// NFDRS4Calculator.h : main header file for the PROJECT_NAME application
//

#pragma once
#include <vector>

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CNFDRS4CalculatorApp:
// See NFDRS4Calculator.cpp for the implementation of this class
//
class CalcOptions
{
public:
	CalcOptions();
	~CalcOptions();

	char fuelModel;
	int slopeClass;
	int windSpeed;
	int fm1;
	int fm10;
	int fm100;
	int fm1000;
	int herb;
	int woody;
	int KBDI;
	int rainEvent;
	int deciduousShrubs;
	int prevKBDI;
	float currRain;
	int maxTemp;
	float cummPrecip;
	float avgPrecip;
	//fuel models...
	std::vector<char> vModels;

};

class CNFDRS4CalculatorApp : public CWinApp
{
public:
	CNFDRS4CalculatorApp();

	CString workDir;
	CalcOptions* pOptions;
	CString m_buildDateStr;
	// Overrides
public:
	virtual BOOL InitInstance();
	int LoadConfig();
	int SaveConfig();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CNFDRS4CalculatorApp theApp;

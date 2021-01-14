#ifdef BUILD_CNFDRSDLL 
#define CNFDRSDLL_EXPORT __declspec(dllexport) 
#else 
#define CNFDRSDLL_EXPORT __declspec(dllimport) 
#endif 
#pragma once

class NFDRCalc;

class CNFDRSDLL_EXPORT CNFDRS
{
public:
	CNFDRS();
	~CNFDRS();
	double fCalcDaylightHours(short iLat, short iJulian);
	short iCalcIndexes(short iWS, short iSlopeCls, double * fROS, double * iSC, double * fERC, double * fFL, double * iFIL, double * iBI);
	short iCalcIC(short iTemp, short iSOW, double fMC1, double fROS, double * iIC);
	short iCalcIndexes(short iWS, short iSlopeCls, double * fROS, int * iSC, double * fERC, double * fFL, int * iFIL, int * iBI);
	short iCalcAdjustMC1(short iMonth, short iExposure, short iAspect, short iSite, short iSlope30, short iHour);
	short iCalcIC(short iTemp, short iSOW, double fMC1, double fROS, int * iIC);
	short iDoAdjustMC1(short iAdjust);
	short iGetFuelBed(int * iSA1, int * iSA10, int * iSA100, int * iSA1000, int * iSAWOOD, int * iSAHERB, double * fW1, 
		double * fW10, double * fW100, double * fW1000, double * fWWOOD, double * fWHERB, double * fWDROUGHT, 
		double * fDEPTH, int * iMXD, int * iHD, int * iSCM, double * fWNDFC, double * fWNDFCMin, double * fWNDFCMax);
	short iGetFuelModel(int * iFM);
	short iSetFuelBed(short iSA1, short iSA10, short iSA100, short iSA1000, short iSAWOOD, short iSAHERB, double fW1, double fW10, double fW100, double fW1000, double fWWOOD, double fWHERB, double fWDROUGHT, double fWDEPTH, short iMXD, short iHD, short iSCM, double fWNDFC, double fWNDFCMin, double fWNDFCMax);
	short iSetFuelModel(short iFM, short iNFMAS);
	short iSetMoistures(double fMC1, double fMC10, double fMC100, double fMC1000, double fMCWood, double fMCHerb, short iRainEvent, short iKBDI, short iGreenHerb, short iGreenWoody, short iSeason, short iSOW);
	short iInitialize(short iNFDRSVersion, short iClimateClass, short iJulianLookFreeze, short iJulianGreenUp, short iLat, short iHerbAnnual, short iWoodyDeciduous, short iOneIsTen, double fStart100, double fStart1000, short iStartKBDI, double fAvgPrecip);
	short iCalcMoist(short iInit, short iTemp, short iRH, short iMaxTemp, short iMaxRH, short iMinTemp, short iMinRH, 
		short iPrecipDur, double fPrecipAmt, short iSOW, double f10stick, short iJulian, short iYear, 
		short iDeclareGreenUp, short iDeclareFreeze, short iGreenHerb, short iGreenWoody, short iSeason, 
		double * f1, double * f10, double * f100, double * f1000, double * fHerb, double * fWood, 
		int * iStage, double * fGren, double * fX1000, int * iRainEvent, int * iKBDI);
	short iCalcKBDI(double fPrecipAmt, short iMaxTemp, double fCummPrecip, short iYKBDI, double fAvgPrecip);
	short LoadState(LPCSTR fileName);
	short SaveState(LPCSTR fileName);
	double GetL1();
	double GetL10();
	double GetL100();
	double GetL1000();
	double GetLHerb();
	double GetLWood();
	double GetLDrought();
	short GetSG1();
	short GetSG10();
	short GetSG100();
	short GetSG1000();
	short GetSGHerb();
	short GetSGWood();
	short GetHD();
	short GetMXD();
	short GetSCM();
	double GetWindFC();
	double GetDepth();
	short SetNFDRSVersion(short version);
	short iCalcMoistWetFlagSnowFlag(short iInit, short iTemp, short iRH, short iMaxTemp, short iMaxRH, 
		short iMinTemp, short iMinRH, short iPrecipDur, double fPrecipAmt, short iSOW, double f10stick, 
		short iJulian, short iYear, short iDeclareGreenUp, short iDeclareFreeze, short iGreenHerb, 
		short iGreenWoody, short iSeason, int iWetFlag, int iSnowFlag, double * f1, double * f10, double * f100, 
		double * f1000, double * fHerb, double * fWood, int * iStage, double * fGren, 
		double * fX1000, int * iRainEvent, int * iKBDI);
	short iCalcMoistWetFlagSnowFlag2(short iInit, short iTemp, short iRH, short iMaxTemp, short iMaxRH, 
		short iMinTemp, short iMinRH, short iPrecipDur, double fPrecipAmt, short iSOW, double f10stick, 
		short iJulian, short iYear, short iDeclareGreenUp, short iDeclareFreeze, short iGreenHerb, 
		short iGreenWoody, short iSeason, int iWetFlag, int iSnowFlag, double * f1, double * f10, 
		int * iStage, double * fGren, double * fX1000, int * iRainEvent);
	short iCalcMoistWetFlag(short iInit, short iTemp, short iRH, short iMaxTemp, short iMaxRH, short iMinTemp, 
		short iMinRH, short iPrecipDur, double fPrecipAmt, short iSOW, double f10stick, short iJulian, short iYear, 
		short iDeclareGreenUp, short iDeclareFreeze, short iGreenHerb, short iGreenWoody, short iSeason, 
		int iWetFlag, double * f1, double * f10, double * f100, double * f1000, double * fHerb, double * fWood, 
		int * iStage, double * fGren, double * fX1000, int * iRainEvent, int * iKBDI);
	short iSetAnnualInit(short iJulianGreenUp, double fStart1000, short iStartKBDI);
	short iSetJulianShrubDormant(short iJulianDormant);
	short iCalcFFWI (short iTemp, short iRH, short iWindSpeed);

private:
	NFDRCalc *m_nfdrs;
};

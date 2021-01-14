//canada.h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//header file for Canadian Fire Danger Model
class CanadianState : public CObject
{
public:
	void Copy(CanadianState *state);
	int lastDay;
	int lastYear;
	double ffmc;
	double dmc;
	double dc;
};

class Canadian : public CObject
{
public:
	Canadian();
	~Canadian();
/* Initialize ************************************************************
Initializes the canadian fire model
**************************************************************************/
	void Initialize(double _ffmc = 85.0, double _dmc = 6.0, double _dc = 15.0);
	void ReInitialize(double _ffmc/* = 85.0*/, double _dmc/* = 6.0*/, double _dc/* = 15.0*/);
	void SetAnnualInit(int _initDayOfYear, int _initYear, double _ffmc/* = 85.0*/, double _dmc/* = 6.0*/, double _dc/* = 15.0*/);
/* CalcIndices ************************************************************
Call for consecutive days to work the model
**************************************************************************/
	void CalcIndices(int dayOfYear, int month, int year, double _temp, double rh, double _ws, double _rain,
		double *_ffmc, double *_dmc, double *_dc, double *_bui, double *_isi, double *_fwi, double *_dsr);
/* SaveState **********************************************************
Saves all internal variables to a data structure
**************************************************************************/
	int SaveState(CanadianState *state);
/* LoadState **********************************************************
Loads a previously saved data structure
**************************************************************************/
	int LoadState(CanadianState *state);
private:
	int lastDay;
	int lastYear;
	double old_ffmc;
	double old_dmc;
	double old_dc;
	double reInit_ffmc;
	double reInit_dmc;
	double reInit_dc;
	//annual init variables
	int annualInitDay;
	int annualInitYear;
	double annualInitFFMC;
	double annualInitDMC;
	double annualInitDC;

};
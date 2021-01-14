//canada.cpp
//implements canadian fire danger model
#include "stdafx.h"
#include <math.h>
#include "canada.h"
double el[12] = {
	6.5, 7.5, 9.0, 12.8, 13.9, 13.9, 12.4, 10.9, 9.4, 8.0, 7.0, 6.0 };

double fl[12] = {
	-1.6, -1.6, -1.6, 0.9, 3.8, 5.8, 6.4, 5.0, 2.4, 0.4, -1.6, -1.6 };

void CanadianState::Copy(CanadianState *state)
{
	state->lastDay = lastDay;
	state->lastYear = lastYear;
	state->ffmc = ffmc;
	state->dmc = dmc;
	state->dc = dc;
}

Canadian::Canadian()
{
	lastDay = 0;
	lastYear = 0;
	old_ffmc = old_dmc = old_dc = reInit_ffmc = reInit_dmc = reInit_dc = 0.0;
	annualInitDay = 0;
	annualInitYear = 0;
	annualInitFFMC = 85.0;
	annualInitDMC = 6.0;
	annualInitDC = 15.0;
}

Canadian::~Canadian()
{
}

/* SaveState **********************************************************
Saves all internal variables to a data structure
**************************************************************************/
int Canadian::SaveState(CanadianState *state)
{
	state->lastDay = lastDay;
	state->lastYear = lastYear;
	state->ffmc = old_ffmc;
	state->dmc = old_dmc;
	state->dc = old_dc;
	return 1;
}

/* LoadState **********************************************************
Loads a previously saved data structure
**************************************************************************/
int Canadian::LoadState(CanadianState *state)
{
	lastDay = state->lastDay;
	lastYear = state->lastYear;
	old_ffmc = state->ffmc;
	old_dmc = state->dmc;
	old_dc = state->dc;
	return 1;
}
void Canadian::ReInitialize(double _ffmc/* = 85.0*/, double _dmc/* = 6.0*/, double _dc/* = 15.0*/)
{
	reInit_ffmc = _ffmc;
	reInit_dmc = _dmc;
	reInit_dc = _dc;
}

void Canadian::Initialize(double _ffmc/* = 85.0*/, double _dmc/* = 6.0*/, double _dc/* = 15.0*/)
{
	old_ffmc = _ffmc;
	old_dmc = _dmc;
	old_dc = _dc;
}

void Canadian::SetAnnualInit(int _initDayOfYear, int _initYear, double _ffmc/* = 85.0*/, double _dmc/* = 6.0*/, double _dc/* = 15.0*/)
{
	annualInitDay = _initDayOfYear;
	annualInitYear = _initYear;
	annualInitFFMC = _ffmc;
	annualInitDMC = _dmc;
	annualInitDC = _dc;
}


void Canadian::CalcIndices(int dayOfYear, int month, int year, double _temp, double rh, double _ws, double _rain,
		double *_ffmc, double *_dmc, double *_dc, double *_bui, double *_isi, double *_fwi, double *_dsr)
{
	double wmo, wm, ffmc, dmc, dc, bui, isi, fwi, dsr, pr, ra, ed, ew, x, z, rw, wmi, b, B,
		wmr, dr, smi, sf, temp, ws, rain, t, rk, qr, v, fd;
	if(lastYear && ((year > lastYear + 1) 
		|| (365 * (year - lastYear) + dayOfYear - lastDay > 30)))
		//|| abs(lastDay - dayOfYear) > 30)
		Initialize(85.0, 6.0, 15.0);
	//check for annual init!
	if(year == annualInitYear && dayOfYear == annualInitDay)
	{
		Initialize(annualInitFFMC, annualInitDMC, annualInitDC);
	}
	//variables temp, ws and rain are in english units, convert to metric
	temp = (_temp - 32) * 5 / 9;
	ws = _ws * 1.609;
	rain = _rain * 25.40;
	//start with ffmc
	wmo = 147.2 * (101 - old_ffmc) / (59.5 + old_ffmc);
	if(rain > 0.5)
	{
		ra = rain - 0.5;
		if(ra < 0.1)
			ra = 0.1;
		if(wmo > 150.0)
			wmo += 42.5 * ra * exp(-100.0 / (251 - wmo)) * (1.0 - exp(-6.93 / ra)) 
				+ 0.0015 * (wmo - 150) * (wmo - 150) * sqrt(ra);
		else
			wmo += 42.5 * ra * exp(-100.0 / (251 - wmo)) * (1.0 - exp(-6.93 / ra));
	}
	if(wmo > 250)
		wmo = 250;
/******************************************************************************************************
	//somebody's fancy way...
	ed = 0.942 * pow(rh, 0.679) + (11.0 * exp(rh - 100.0) / 10.0)
		+ 0.18 * (21.1 - temp) * (1.0 - 1.0 / exp(rh * 0.115));
	ew = 0.618 * pow(rh, 0.753) + (10.0 * exp((rh - 100.0) / 10.0))
		+ 0.18 * (21.1 - temp) * (1.0 - 1.0 / exp(rh * 0.115));
/********************************************************************************************************/
//book method
	ed = 0.942 * pow(rh, 0.679) + 11.0 * exp((rh - 100.0) / 10.0)
		+ 0.18 * (21.1 - temp) * (1.0 - exp(rh * -0.115));

	
	ew = 0.618 * pow(rh, 0.753) + 10.0 * exp((rh - 100.0) / 10.0)
		+ 0.18 * (21.1 - temp) * (1.0 - exp(rh * -0.115));
/********************************************************************************************************/
	if(wmo < ed && wmo < ew)
	{
		z = 0.424 * (1.0 - pow(((100.0 - rh) / 100.0), 1.7)) 
			+ 0.0694 * sqrt(ws) * (1.0 - pow((100.0 - rh) / 100.0, 8.0));
		x = z * 0.581 * exp(0.0365 * temp);
		//wm = ed + (wmo - ed) / pow(10.0, x);
		wm = ew - (ew - wmo) * pow(10.0, -x);
	}
	else if(wmo > ed)
	{
		z = 0.424 * (1.0 - pow(rh / 100.0, 1.7)) + 0.0694 * sqrt(ws) * (1.0 - pow(rh / 100.0, 8.0));
		x = z * 0.581 * exp(0.0365 * temp);
		//wm = ed + (wmo - ed) / pow(10.0, x);
		wm = ed + (wmo - ed) * pow(10.0, -x);
	}
	//if(ed >= wmo && wmo >= ew)
	else
		wm = wmo;
	if(wm < 0.0)
		wm = 0.0;
	ffmc = 59.5 * (250.0 - wm) / (147.2 + wm);
	if(ffmc > 101.0)
		ffmc = 101.0;
	if(ffmc < 0)
		ffmc = 0;
	//now the DMC part
	t = temp;
	if(temp < -1.1)
		t = -1.1;
	rk = 1.894 * (t + 1.1) * (100.0 - rh) * el[month - 1] * 0.0001;
	if(rain <= 1.5)
		pr = old_dmc;
	else
	{
		ra = rain;
		rw = 0.92 * ra - 1.27;
		wmi = 20.0 + 280.0 / exp(0.023 * old_dmc);
		if(old_dmc <= 33)
			b = 100.0 / (0.5 + 0.3 * old_dmc);
		else if(old_dmc <= 65)
			b = 14.0 - 1.3 * log(old_dmc);
		else
			b = 6.2 * log(old_dmc) - 17.2;
		wmr = wmi + 1000.0 * rw / (48.77 + b * rw);
		pr = 43.43 * (5.6348 - log(wmr - 20.0));
	}
	if(pr < 0.0)
		pr = 0.0;
	dmc = pr + rk;
	if(dmc < 0.0)
		dmc = 0.0;
	//calculate DC
	if(temp < -2.8)
		t = -2.8;
	//pe = (0.36 * (t + 2.8) + fl[month - 1]) / 2.0;
	if(rain <= 2.801)
		dr = old_dc;
/*	else
	{
		ra = rain;
		rw = 0.83 * ra - 1.27;
		smi = 800 * exp(-old_dc / 400.0);
		dr = old_dc - 400.0 * log(1.0 + 3.937 * rw / smi);
		if(dr < 0.0)
			dr = 0.0;
	}*/
	else
	{
		ra = rain;
		rw = 0.83 * ra - 1.27;
		smi = 800 * exp(-old_dc / 400.0);
		qr = smi + 3.937 * rw;
		dr = 400 * log(800 / qr);
		if(dr < 0)
			dr = 0;
	}
	v = 0.36 * (t + 2.8) + fl[month - 1];
	if(v < 0)
		v = 0;
	dc = dr + v * 0.5;
		//dr = old_dc - 400.0 * log(1.0 + 3.937 * rw / smi);
		//if(dr < 0.0)
			//dr = 0.0;
//	}

	//dc = dr * pe;
	if (dc < 0.0)
		dc = 0.0;
	//calc isi and the rest...
	//fm = 147.2 * (101.0 - ffmc) / (59.5 + ffmc);
	//sf = 19.115 * exp(-0.1386 * fm) * (1.0 + pow(fm, 5.31) / 4.93e07);
	sf = 91.9 * exp(-0.1386 * wm) * (1.0 + pow(wm, 5.31) / 4.93e07);
	isi = 0.208 * sf * exp(0.05039 * ws);
	/*if(dmc == 0.0 && dc == 0.0)
		bui = 0.0;
	else
		bui = 0.8 * dc * dmc / (dmc + 0.4 * dc);
	if(bui < dmc)
	{
		p = (dmc - bui) / dmc;
		cc = 0.92 + pow(0.0114 * dmc, 1.7);
		bui = dmc - cc * p;
		if(bui < 0.0)
			bui = 0.0;
	}
	if(bui > 80)
		bb = 0.1 * isi * (1000.0 / (25.0 + 108.64 / exp(0.023 * bui)));
	else
		bb = 0.1 * isi * (0.626 * pow(bui, 0.809) + 2.0);
	if(bb <= 1.0)
		fwi = bb;
	else
		fwi = exp(2.72 * pow(0.434 * log(bb), 0.647));*/
	if(dmc == 0.0 && dc == 0.0)
		bui = 0.0;
	else if(dmc <= 0.4 * dc)
	{
		bui = 0.8 * dmc * dc / (dmc + 0.4 * dc);
	}
	else
	{
		bui = dmc - (1 - 0.8 * dc / (dmc + 0.4 * dc)) * (0.92 + pow(0.0114 * dmc, 1.7));
	}
	if(bui < 0.0)
		bui = 0.0;
	if(bui <= 80)
	{
		fd = 0.626 * pow(bui, 0.809) + 2;
	}
	else
	{
		fd = 1000 / (25 + 108.64 * exp(-0.023 * bui));
	}
	B = 0.1 * isi * fd;
	if(B > 1)
	{
		fwi = exp(2.72 * pow(0.434 * log(B), 0.647));
	}
	else
		fwi = B;
	dsr = 0.0272 * pow(fwi, 1.77);

	//reset contained values...
	old_ffmc = ffmc;
	old_dmc = dmc;
	old_dc = dc;
	lastDay = dayOfYear;
	lastYear = year;
	//set returned values
	*_ffmc = ffmc;
	*_dmc = dmc;
	*_dc = dc;
	*_bui = bui;
	*_isi = isi;
	*_fwi = fwi;
	*_dsr = dsr;
}


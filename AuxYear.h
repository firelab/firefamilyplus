// AuxYear.h: interface for the AuxYear class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_AUXYEAR_H__231A6F20_128D_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_AUXYEAR_H__231A6F20_128D_11D3_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AuxYear  
{
public:
	void Complete();
	void Add(int period, double val);
	int nPeriods;
	Stats * vals;
	int year;
	AuxYear(int _year, int _nPeriods);
	virtual ~AuxYear();

};

#endif // !defined(AFX_AUXYEAR_H__231A6F20_128D_11D3_BD5A_00A0CC24A3EB__INCLUDED_)

// AuxYear.cpp: implementation of the AuxYear class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fireplus.h"
#include "WxSet.h"
#include "AuxYear.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AuxYear::AuxYear(int _year, int _nPeriods) : year(_year), nPeriods(_nPeriods)
{
	vals = NULL;
	if(nPeriods > 0)
		vals = new Stats[nPeriods];
}

AuxYear::~AuxYear()
{
	if(vals)
		delete[] vals;
}

void AuxYear::Add(int period, double val)
{
	if(period >= 0 && period < nPeriods)
		vals[period].Accumulate(val);
}

void AuxYear::Complete()
{
	for(int i = 0; i < nPeriods; i++)
		vals[i].Complete();
}

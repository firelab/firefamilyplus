// FireDay.cpp: implementation of the CFireDay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "FireDay.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFireDay::CFireDay()
{
	lightning = human = 0;
	lAcresMax = hAcresMax = 0.0;
}

CFireDay::~CFireDay()
{

}

#pragma once
#include "TermsSet.h"
#include "TermDatesSet.h"
#include "fireplusSet.h"

class CTermData
{
public:
	CTermData(void);
	~CTermData(void);
	bool Create(CTermsSet *termSet, CTermDatesSet *termDatesSet);
	CString TermReport();
	CString staStr;
	int termID;
	double alpha;
	double beta;
	double r2;
	COleDateTime startDate;
	CString name;
	CString comment;
	int nDates;
	int *years;
	COleDateTime *dates;
	int *streamDays;
	int numPercentiles;
	double *percentiles;
	CString *dateComments;
	CString dateStamp;
};

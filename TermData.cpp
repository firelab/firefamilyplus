#include "stdafx.h"
#include "fireplus.h"
#include "TermData.h"
#include "TermPercentilesSet.h"
#include "SIGStationSet.h"
#include <math.h>

extern CString AddDateStamp();

int intCompare( const void *arg1, const void *arg2 )
{
   /* Compare the values as ints: */
	if(*(int *)arg1 > *(int *)arg2)
		return 1;
	else if(*(int *)arg1 < *(int *)arg2)
		return -1;
	return 0;
}

CTermData::CTermData(void)
{
	termID = 0;
	alpha = 1.0;
	beta = 1.0;
	r2 = 0.0;
	startDate;
	comment = "";
	nDates = 0;
	years = NULL;
	dates = NULL;
	streamDays = NULL;
	dateComments = NULL;
	numPercentiles = 0;
	percentiles = NULL;
}

CTermData::~CTermData(void)
{
	if(years)
		delete[] years;
	if(dates)
		delete[] dates;
	if(streamDays)
		delete[] streamDays;
	if(dateComments)
		delete[] dateComments;
	if(percentiles)
		delete[] percentiles;
}

bool CTermData::Create(CTermsSet *termSet, CTermDatesSet *termDatesSet)
{
	//m_fpSet = fpSet;
	staStr = termSet->m_SIG_Station;
	staStr.Trim();
	if(staStr.GetLength() <= 6)//a single station
	{
		CSIGStationSet staSet(termSet->m_pDatabase);
		staSet.m_strFilter.Format("[StationID] = '%6.6s'", termSet->m_SIG_Station);
		staSet.Open();
		staStr += " - " + staSet.m_Name;
		staSet.Close();
	}
	staStr.Trim();
	termID = termSet->m_TermID;
	alpha = termSet->m_Coeff1;
	beta = termSet->m_Coeff2;
	r2 = termSet->m_Coeff3;
	startDate = termSet->m_StartDate;
	name = termSet->m_Name;
	comment = termSet->m_Comment;
	termDatesSet->MoveFirst();
	while(!termDatesSet->IsEOF())
	{
		if(!termDatesSet->IsFieldNull(&termDatesSet->m_Date))
			nDates++;
		termDatesSet->MoveNext();
	}
	if(nDates <= 0)
		return false;
	years = new int[nDates];
	dates = new COleDateTime[nDates];
	streamDays = new int[nDates];
	dateComments = new CString[nDates];
	termDatesSet->MoveFirst();
	int loc = 0;
	while(!termDatesSet->IsEOF())
	{
		if(!termDatesSet->IsFieldNull(&termDatesSet->m_Date))
		{
			years[loc] = termDatesSet->m_Year;
			dates[loc] = termDatesSet->m_Date;
			dateComments[loc] = termDatesSet->m_Comment;
			streamDays[loc] = dates[loc].GetDayOfYear() - startDate.GetDayOfYear();
			if(streamDays[loc] < 0)
				streamDays[loc] += 365;
			loc++;
		}
		termDatesSet->MoveNext();
	}
	qsort((void *)streamDays, nDates, sizeof(int), intCompare);

	CTermPercentilesSet tpSet(termSet->m_pDatabase);
	tpSet.m_strFilter.Format("[TermID] = %d", termID);
	tpSet.m_strSort = "[Percentile]";
	tpSet.Open();
	int numP = 0;
	while(!tpSet.IsEOF())
	{
		numP++;
		tpSet.MoveNext();
	}
	numPercentiles = numP;
	if(numPercentiles > 0)
	{
		percentiles = new double[numPercentiles];
		int loc = 0;
		tpSet.MoveFirst();
		while(!tpSet.IsEOF())
		{
			percentiles[loc] = tpSet.m_Percentile;
			loc++;
			tpSet.MoveNext();
		}
	}
	tpSet.Close();
	dateStamp = AddDateStamp();
	return true;
}

CString CTermData::TermReport()
{
	char *fName = GetTempFileName();
	CString ret = fName;
	free(fName);
	FILE *stream = fopen(ret, "wt");
	CString str, temp;
	fprintf(stream,
		"FireFamily Plus Term Report\n\n");

	fprintf(stream, "Station: %s\n", staStr);
	fprintf(stream, "Term Name: %s\n", name);
	fprintf(stream, "Season Start Day: %d\\%d\n",
		startDate.GetMonth(), startDate.GetDay());
	fprintf(stream, "Data Years: %d - %d\n",
		years[0],
		years[nDates - 1]);
	fprintf(stream, "Alpha: %lf\n", alpha);
	fprintf(stream, "Beta: %lf\n", beta);
	fprintf(stream, "R-Squared: %lf\n\n", r2);
	//fprintf(stream, "Comment: %s\n\n", comment);
	fprintf(stream, "Comment:\n");//%s\n\n", comment);
	int cmntLineNo = 1;
	for(int i = 0; i < comment.GetLength(); i++)
	{
		char c = comment.GetAt(i);
		if(c != '\r')
			fprintf(stream, "%c", c);
		if(i / (cmntLineNo * 80) > 0 && (c == ' ' || c == '\t'))
		{
			fprintf(stream, "\n");
			cmntLineNo++;
		}
	}
	fprintf(stream, "\n\nTerm Dates\n");
	fprintf(stream, "Year    Day    #Days    Comment\n");
	for(int y = 0; y < nDates; y++)
	{
		int days = dates[y].GetDayOfYear() - startDate.GetDayOfYear();
		if(days < 0)
			days += 365;
		fprintf(stream, "%d   %2d/%2d     %3d    %s\n", years[y], dates[y].GetMonth(), dates[y].GetDay(), days, dateComments[y]);
	}
	fprintf(stream, "\n");
	if(numPercentiles > 0)
	{
		fprintf(stream, "Key Probabilities\n", alpha);
		fprintf(stream, "Probability     Date\n");
		int d = 0;
		//double q;
		for(int p = 0; p < numPercentiles; p++)
		{
			for(; d < 365; d++)
			{
				double pVal = 1.0 - exp(-(pow((beta * d), alpha))); 
				if(pVal >= (double)percentiles[p])
				{
					COleDateTimeSpan span((int)d, 0, 0, 0);
					COleDateTime tDate = startDate + span;
					//q = percentiles[p] / 100.0;
					fprintf(stream, " %0.2f          %s\n", percentiles[p], tDate.Format("%B %d"));
					break;
				}
			}
			if(d >= 365) //neve reached percentiles[p]!
				fprintf(stream, " %0.2f          %s\n", percentiles[p], "Not achieved in 365 days");

		}
	}
	fprintf(stream, "\n%s\n", dateStamp);
	fclose(stream);
	return ret;
}

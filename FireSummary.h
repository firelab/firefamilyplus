// FireSummary.h: interface for the CFireSummary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIRESUMMARY_H__53A132C3_6E7B_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_FIRESUMMARY_H__53A132C3_6E7B_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#define NDAYVALS 10
#define NSIZECLASSES 7
#define NCAUSECLASSES 9

class CFireSummary  
{
public:
	int nStrings;
	CString * qStrings;
	int startYear;
	CFireSummary();
	virtual ~CFireSummary();
	int AddFires(CDatabase *pDB, CString queryID, int start, int end);
	int AddFires(CDatabase *pDB, CFireplusSet *fpSet);
	int ProcessFires(CDatabase *pDB, CString queryID, CString baseQuery);
	int nYears;
	double *acres;
	long *fires;
	long monthFires[13];
	long monthLightningFires[13];
	long sizeClasses[NSIZECLASSES + 1];
	long causeClasses[NCAUSECLASSES + 1];
	long nPerDay[NDAYVALS + 1];
	//int TranslateCauseToUSFS(int cause, int agency);
	CFireplusSet *m_pFpSet;
	CDatabase *m_pDB;
	double sizePcnts[100];
};

#endif // !defined(AFX_FIRESUMMARY_H__53A132C3_6E7B_11D2_B8E5_000000000000__INCLUDED_)

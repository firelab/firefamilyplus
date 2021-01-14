// FireStats.cpp: implementation of the CFireStats class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "fireplusSet.h"
#include "SIGStationSet.h"
#include "FireStats.h"
#include "StationInSIGSet.h"
#include <direct.h>
#include "p_chisq.h"
#include "WxSet.h"
#include <math.h>
#include "ReportOptionsSet.h"
#include "LogReg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern int compare( const void *arg1, const void *arg2 );
extern CFireplusApp theApp;
extern int curr_runID;
extern CString AddDateStamp();
//local function
int pvalCompare( const void *arg1, const void *arg2 )
{
   /* Compare the values as doubles: */
	pval pv1, pv2;
	pv1 = *(pval *)arg1;
	pv2 = *(pval *)arg2;
	if(pv1.p > pv2.p)
		return 1;
	else if(pv1.p < pv2.p)
		return -1;
	return 0;
}

ChiSquareGroup *CFireStats::GoodFitQuick(double *fxVals, double *fyVals, int *ngrps, int fdType, BOOL conditional)
{
	int n = 0;
	//char buf[256];
	//*ngrps = 0;
	//strip first line
	//get all the pval structures
	long v = 0, f = 0;
	double b0, b1;
	switch(fdType)
	{
	case FD:
		b0 = b0fd;
		b1 = b1fd;
		break;
	case LFD:
		b0 = b0lfd;
		b1 = b1lfd;
		break;
	case MFD:
		b0 = b0mfd;
		b1 = b1mfd;
		break;
	}
	long numV = (conditional && fdType != FD) ? nfd : nv;
	//pval *pvals = new pval[nv];
	pval *pvals = new pval[numV];
	//while(!feof(fd) && v < nv)
	while(v < numV)
	{
		pvals[v].vals[0] = fyVals[v];///atof(&buf[2]);
		//TRACE3("Entry: %ld        pvals[v].vals[0]: %f        pvals[v].p: %f\n", v, pvals[v].vals[0], pvals[v].p);
		pvals[v].p = 1.0 / (1.0 + exp(-1.0 * b0 + (-1.0 * b1) * pvals[v].vals[0]));
		if(fxVals[v] == 1)
			pvals[v].hit = true;
		v++;
		//fgets(buf, 79, fd);
	}
	qsort((void *)pvals, (size_t)numV, sizeof( pval ), pvalCompare );
	//have a sorted array of pvals, need to bin into groups and create
	//the Chi Square struct
	//double maxp = pvals[0].p, minp = pvals[nv - 1].p;
	long grpSz = numV / 10;//ideally 10 groups
	long brks[10];
	n = 10;
	//brks[0] = 0;
	int i;
	for(i = 0; i < 10; i++)
		brks[i] = i * grpSz;
	//deal with ties, refine groupings so that all expecteds are at least 1
	double expct, valavg;
	long hits, obs, allHits = 0, totalObs = 0;
	for(i = 0; i < n; i++)
	{
		expct = 0.0;
		int j;
		for(j = brks[i]; j < ((i < n - 1) ? brks[i + 1] : numV); j++)
		{//get expecteds by summing p values
			expct += pvals[j].p;
			//if(expct > 1.0)
			//	break;
		}
		if(expct < 1.0)//didn't have expected of at least 1
		{//make sure one expected
			while(expct < 1.0 && j < numV)
			{
				expct += pvals[j].p;
				j++;
			}
		}
		//check for tie on boundary. If so, adjust
		while(j < numV)
		{
			if(pvals[j - 1].p == pvals[j].p)
				j++;
			else
				break;
		}
		if(i < n - 1)
		{//adjust breaks for next groups
			if(j >= brks[i + 1])
			{
				brks[i + 1] = j;
				if(i < n - 2)
				{
					brks[i + 2] = max(brks[i + 2], j + 1);
				}
			}
		}
		if(j >= numV)//test for more data
		{
			n = i + 1;
			break;
		}
	}
	if(expct < 1.0)
	{//last group didn't have at least one expected
		if(n > 1)//if more than one group, combine the last with previous
		{
			n--;
		}
	}
	//all groups now have at least one expected
	//create the Chi Square Group structure
	Stats valStats;
	ChiSquareGroup *grps = new ChiSquareGroup[n];
	for(i = 0; i < n; i++)
	{
		expct = 0.0;
		valavg = 0.0;
		hits = 0;
		obs = 0;
		for(int j = brks[i]; j < ((i < n - 1) ? brks[i + 1] : numV); j++)
		{//get expecteds by summing p values
			expct += pvals[j].p;
			//valavg += pvals[j].vals[0];
			valStats.Accumulate(pvals[j].vals[0]);
			if(pvals[j].hit)
			{
				hits++;
				allHits++;
			}
			obs++;
			totalObs++;
		}
		valStats.Complete();
		grps[i].expHit = expct;
		grps[i].expNoHit = obs - grps[i].expHit;
		grps[i].obsHit = hits;
		grps[i].obsNoHit = obs - hits;
		if(obs > 0)
		{
			grps[i].pAvg = expct / (double)obs;
			grps[i].valAvg = valStats.Median();//valavg / (double)obs;
			grps[i].pMin = pvals[brks[i]].p;
			grps[i].pMax = pvals[((i < n - 1) ? brks[i + 1] - 1 : numV - 1)].p;
			grps[i].valMin = pvals[brks[i]].vals[0];
			grps[i].valMax = pvals[((i < n - 1) ? brks[i + 1] - 1 : numV - 1)].vals[0];
		}
		else
		{
			grps[i].pAvg = 0;
			grps[i].valAvg = 0;
			grps[i].pMin = 0;
			grps[i].pMax = 0;
			grps[i].valMin = 0;
			grps[i].valMax = 0;
		}
		valStats.Reset();
	}
	delete[] pvals;
	*ngrps = n;
	return grps;
}
/*
ChiSquareGroup *CFireStats::GoodFit(FILE *fd, int *ngrps, int fdType, BOOL conditional)
{
	int n = 0;
	char buf[256];
	//*ngrps = 0;
	rewind(fd);
	//strip first line
	fgets(buf, 79, fd);
	//get all the pval structures
	long v = 0, f = 0;
	double b0, b1;
	switch(fdType)
	{
	case FD:
		b0 = b0fd;
		b1 = b1fd;
		break;
	case LFD:
		b0 = b0lfd;
		b1 = b1lfd;
		break;
	case MFD:
		b0 = b0mfd;
		b1 = b1mfd;
		break;
	}
	long numV = (conditional && fdType != FD) ? nfd : nv;
	//pval *pvals = new pval[nv];
	pval *pvals = new pval[numV];
	//while(!feof(fd) && v < nv)
	while(v < numV)
	{
		pvals[v].vals[0] = atof(&buf[2]);
		//TRACE3("Entry: %ld        pvals[v].vals[0]: %f        pvals[v].p: %f\n", v, pvals[v].vals[0], pvals[v].p);
		pvals[v].p = 1.0 / (1.0 + exp(-1.0 * b0 + (-1.0 * b1) * pvals[v].vals[0]));
		if(atoi(buf) == 1)
			pvals[v].hit = true;
		v++;
		fgets(buf, 79, fd);
	}
	qsort((void *)pvals, (size_t)numV, sizeof( pval ), pvalCompare );
	//have a sorted array of pvals, need to bin into groups and create
	//the Chi Square struct
	//double maxp = pvals[0].p, minp = pvals[nv - 1].p;
	long grpSz = numV / 10;//ideally 10 groups
	long brks[10];
	n = 10;
	//brks[0] = 0;
	int i;
	for(i = 0; i < 10; i++)
		brks[i] = i * grpSz;
	//deal with ties, refine groupings so that all expecteds are at least 1
	double expct, valavg;
	long hits, obs, allHits = 0, totalObs = 0;
	for(i = 0; i < n; i++)
	{
		expct = 0.0;
		int j;
		for(j = brks[i]; j < ((i < n - 1) ? brks[i + 1] : numV); j++)
		{//get expecteds by summing p values
			expct += pvals[j].p;
			//if(expct > 1.0)
			//	break;
		}
		if(expct < 1.0)//didn't have expected of at least 1
		{//make sure one expected
			while(expct < 1.0 && j < numV)
			{
				expct += pvals[j].p;
				j++;
			}
		}
		//check for tie on boundary. If so, adjust
		while(j < numV)
		{
			if(pvals[j - 1].p == pvals[j].p)
				j++;
			else
				break;
		}
		if(i < n - 1)
		{//adjust breaks for next groups
			if(j >= brks[i + 1])
			{
				brks[i + 1] = j;
				if(i < n - 2)
				{
					brks[i + 2] = max(brks[i + 2], j + 1);
				}
			}
		}
		if(j >= numV)//test for more data
		{
			n = i + 1;
			break;
		}
	}
	if(expct < 1.0)
	{//last group didn't have at least one expected
		if(n > 1)//if more than one group, combine the last with previous
		{
			n--;
		}
	}
	//all groups now have at least one expected
	//create the Chi Square Group structure
	Stats valStats;
	ChiSquareGroup *grps = new ChiSquareGroup[n];
	for(i = 0; i < n; i++)
	{
		expct = 0.0;
		valavg = 0.0;
		hits = 0;
		obs = 0;
		for(int j = brks[i]; j < ((i < n - 1) ? brks[i + 1] : numV); j++)
		{//get expecteds by summing p values
			expct += pvals[j].p;
			//valavg += pvals[j].vals[0];
			valStats.Accumulate(pvals[j].vals[0]);
			if(pvals[j].hit)
			{
				hits++;
				allHits++;
			}
			obs++;
			totalObs++;
		}
		valStats.Complete();
		grps[i].expHit = expct;
		grps[i].expNoHit = obs - grps[i].expHit;
		grps[i].obsHit = hits;
		grps[i].obsNoHit = obs - hits;
		if(obs > 0)
		{
			grps[i].pAvg = expct / (double)obs;
			grps[i].valAvg = valStats.Median();//valavg / (double)obs;
			grps[i].pMin = pvals[brks[i]].p;
			grps[i].pMax = pvals[((i < n - 1) ? brks[i + 1] - 1 : numV - 1)].p;
			grps[i].valMin = pvals[brks[i]].vals[0];
			grps[i].valMax = pvals[((i < n - 1) ? brks[i + 1] - 1 : numV - 1)].vals[0];
		}
		else
		{
			grps[i].pAvg = 0;
			grps[i].valAvg = 0;
			grps[i].pMin = 0;
			grps[i].pMax = 0;
			grps[i].valMin = 0;
			grps[i].valMax = 0;
		}
		valStats.Reset();
	}
	delete[] pvals;
	*ngrps = n;
	return grps;
}
*/
ChiSquareGroup::ChiSquareGroup()
{
	obsHit = obsNoHit = 0;
	pMin = pMax = pAvg = valAvg = valMin = valMax = expHit = expNoHit = 0.0;
}

double ChiSquareGroup::CalcChi2()
{
	if(expHit == 0.0 || expNoHit == 0.0)
		return 0.0;
	double xH = obsHit - expHit, xN = obsNoHit - expNoHit, x2 = 0.0;
	x2 = xH * xH / expHit + xN * xN / expNoHit;
	return x2;
}

double ChiSquareGroup::Output(FILE *stream, int round)
{
	//regular chi-square way
	double xH = obsHit - expHit, xN = obsNoHit - expNoHit, x2 = 0.0, pct;
	if(obsHit + obsNoHit > 0)
		pct = ((double)obsHit) / ((double)(obsHit + obsNoHit)) * 100.0;
	else
		pct = 0.0;
	if(expHit == 0.0 || expNoHit == 0.0)
	{
		if(round)
			fprintf(stream, "%5.2f -%5.2f %6.0f -%6.0f %6ld %3.0f %6ld %6.0f %6ld %6.0f        NA\n",
				pMin, pMax, valMin, valMax, obsHit + obsNoHit, pct, obsHit, expHit, obsNoHit, expNoHit);
		else
			fprintf(stream, "%5.2f -%5.2f %6.2f -%6.2f %6ld %3.0f %6ld %6.0f %6ld %6.0f        NA\n",
				pMin, pMax, valMin, valMax, obsHit + obsNoHit, pct, obsHit, expHit, obsNoHit, expNoHit);
		return 0.0;
	}
	x2 = xH * xH / expHit + xN * xN / expNoHit;
	if(round)
		fprintf(stream, "%5.2f -%5.2f %6.0f -%6.0f %6ld %3.0f %6ld %6.0f %6ld %6.0f %9.1f\n",// %8.4f\n",
			pMin, pMax, valMin, valMax, obsHit + obsNoHit, pct, obsHit, expHit, obsNoHit, expNoHit, x2);//, p_chisq(0.5, x2 / 2.0));
	else
		fprintf(stream, "%5.2f -%5.2f %6.2f -%6.2f %6ld %3.0f %6ld %6.0f %6ld %6.0f %9.1f\n",// %8.4f\n",
			pMin, pMax, valMin, valMax, obsHit + obsNoHit, pct, obsHit, expHit, obsNoHit, expNoHit, x2);//, p_chisq(0.5, x2 / 2.0));
	return x2;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFireStats::CFireStats(CString _name, long nV, long nFD, long nLFD, long nMFD, bool _isNFDRS, int _varID)
	: varName(_name), nv(nV), nfd(nFD), nlfd(nLFD), nmfd(nMFD), isNFDRS(_isNFDRS)
{
	filterValue = -999;
	filterDirection = FILTER_LESSTHAN_EQUAL;
	nFiltered = 0;
	humanCause = 0;
	varID = _varID;
	nStrings = 0;
	strings = NULL;
	pDB = NULL;
	m_pSet = NULL; 
	nFDG = nLFDG = nMFDG = nMadeUps = 0;
	xGroupsFD = xGroupsLFD = xGroupsMFD = NULL;
	if(nv > 0)
	{
		allVals = new double[nv];
		for(int i = 0; i < nv; i++)
			allVals[i] = 0.0;
	}
	else
		allVals = NULL;
	if(nfd > 0)
	{
		fdVals = new double[nfd];
		for(int i = 0; i < nfd; i++)
			fdVals[i] = 0.0;
	}
	else
		fdVals = NULL;
	if(nlfd > 0)
	{
		lfdVals = new double[nlfd];
		for(int i = 0; i < nlfd; i++)
			lfdVals[i] = 0.0;
	}
	else
		lfdVals = NULL;
	if(nmfd > 0)
	{
		mfdVals = new double[nmfd];
		for(int i = 0; i < nmfd; i++)
			mfdVals[i] = 0.0;
	}
	else
		mfdVals = NULL;
	m_conditional = false;
	fd_R2 = fd_Chi2 = fd_Chi2PVal = fd_pMin = fd_pMax = lfd_R2
		= lfd_Chi2 = lfd_Chi2PVal = lfd_pMin = lfd_pMax
		= mfd_R2 = mfd_Chi2 = mfd_Chi2PVal = mfd_pMin = mfd_pMax
		= b0fd = b1fd = b0lfd = b1lfd = b0mfd = b1mfd = devfd = devlfd = devmfd = 0.0;
}

CFireStats::~CFireStats()
{
	if(allVals)
		delete[] allVals;
	if(fdVals)
		delete[] fdVals;
	if(lfdVals)
		delete[] lfdVals;
	if(mfdVals)
		delete[] mfdVals;
	if(strings)
		delete[] strings;
	if(xGroupsFD)
		delete[] xGroupsFD;
	if(xGroupsLFD)
		delete[] xGroupsLFD;
	if(xGroupsMFD)
		delete[] xGroupsMFD;
}

void CFireStats::BuildQuick(CFireplusSet *fpSet, bool probabilities, BOOL conditional,
		long nVals, long nfdVals,
		double *fxVals, double *fyVals, double *lfxVals, double *lfyVals, double *mfxVals, double *mfyVals,
		LogReg *fdLogReg, LogReg *lfdLogReg, LogReg *mfdLogReg)
{
	pDB = fpSet->m_pDatabase;
	m_pSet = fpSet;
	m_conditional = conditional;
	if(probabilities)
	{
		if(fdLogReg && fdLogReg->errorcode == 0)
		{
			b0fd = fdLogReg->m_coef[0];
			b1fd = fdLogReg->m_coef[1];
			devfd = fdLogReg->m_dev;
			pfd = fdLogReg->m_pdev;
		}
		if(lfdLogReg && lfdLogReg->errorcode == 0)
		{
			b0lfd = lfdLogReg->m_coef[0];
			b1lfd = lfdLogReg->m_coef[1];
			devlfd = lfdLogReg->m_dev;
			plfd = lfdLogReg->m_pdev;
		}
		if(mfdLogReg && mfdLogReg->errorcode == 0)
		{
			b0mfd = mfdLogReg->m_coef[0];
			b1mfd = mfdLogReg->m_coef[1];
			devmfd = mfdLogReg->m_dev;
			pmfd = mfdLogReg->m_pdev;
		}
	}
	long v = 0, f = 0;
	for(v = 0; v < nVals; v++)
	{
		allVals[v] = fyVals[v];
		if(fxVals[v] == 1)
			fdVals[f++] = fyVals[v];
	}
	if(probabilities)
		xGroupsFD = GoodFitQuick(fxVals, fyVals, &nFDG, FD, m_conditional);
	f = 0;
	for(v = 0; v < nfdVals; v++)
	{
		if(lfxVals[v] == 1)
			lfdVals[f++] = lfyVals[v];
	}
	if(probabilities)
		xGroupsLFD = GoodFitQuick(lfxVals, lfyVals, &nLFDG, LFD, m_conditional);
	f = 0;
	for(v = 0; v < nfdVals; v++)
	{
		if(mfxVals[v] == 1)
			mfdVals[f++] = mfyVals[v];
	}
	if(probabilities)
		xGroupsMFD = GoodFitQuick(mfxVals, mfyVals, &nMFDG, MFD, m_conditional);
	//now sort the arrays
	if(nv > 1)
		qsort( (void *)allVals, (size_t)nv, sizeof( double ), compare );
	if(nfd > 1)
		qsort( (void *)fdVals, (size_t)nfd, sizeof( double ), compare );
	if(nlfd > 1)
		qsort( (void *)lfdVals, (size_t)nlfd, sizeof( double ), compare );
	if(nmfd > 1)
		qsort( (void *)mfdVals, (size_t)nmfd, sizeof( double ), compare );

	

	//calculate base statistics for Candidates...
	if(probabilities)
	{
		int g;
		//do FD's
		double x2 = 0.0, expFD = 0.0, expNFD = 0.0, df = 0.0, sstot = 0.0, sse1 = 0.0, ssr1 = 0.0,
			sses = 0.0, ssrs = 0.0;
		long obsFD = 0, obsNFD = 0, tnv = nv;
		if(nfd > 0)
		{
			fd_pMin = 1.0, fd_pMax = 0.0;
			for(g = 0; g < nFDG; g++)
			{
				x2 += xGroupsFD[g].CalcChi2();
				expFD += xGroupsFD[g].expHit;
				expNFD += xGroupsFD[g].expNoHit;
				obsFD += xGroupsFD[g].obsHit;
				obsNFD += xGroupsFD[g].obsNoHit;
				fd_pMin = min(fd_pMin, xGroupsFD[g].pMin);
				fd_pMax = max(fd_pMax, xGroupsFD[g].pMax);
				df++;
			}
			fd_Chi2 = x2;
			if(df > 2)
				fd_Chi2PVal = p_chisq((df - 2.0) / 2.0, x2 / 2.0);
			else
				fd_Chi2PVal = 0.0;
			sstot = -2.0 * (obsFD * log((double)obsFD/(double)tnv) + obsNFD * log((double)obsNFD/(double)tnv));
			for(g = 0; g < nFDG; g++)
			{
				double p = xGroupsFD[g].expHit / (double)(xGroupsFD[g].obsHit + xGroupsFD[g].obsNoHit);
				if(p > 0.0 && p < 1.0)
					sse1 += ((double)xGroupsFD[g].obsHit) * log(p)
						+ ((double)xGroupsFD[g].obsNoHit) * log(1.0 - p);
				p = ((double)xGroupsFD[g].obsHit) / (double)(xGroupsFD[g].obsHit + xGroupsFD[g].obsNoHit);
				if(p > 0.0 && p < 1.0)
					sses += ((double)xGroupsFD[g].obsHit) * log(p)
						+ ((double)xGroupsFD[g].obsNoHit) * log(1.0 - p);
			}
			sse1 *= -2.0;
			sses *= -2.0;
			sse1 = min(sse1, sstot);
			sses = min(sses, sstot);
			ssr1 = sstot - sse1;
			ssrs = sstot - sses;
			if(ssrs != 0.0)
				fd_R2 = ssr1/ssrs;
		}

		//do LFD's
		if(conditional)
			tnv = nfd;
		x2 = expFD = expNFD = df = sstot = sse1 = ssr1 = sses = ssrs = 0.0;0.0;
		obsFD = obsNFD = 0;
		if(nlfd > 0)
		{
			lfd_pMin = 1.0, lfd_pMax = 0.0;
			for(g = 0; g < nLFDG; g++)
			{
				x2 += xGroupsLFD[g].CalcChi2();
				expFD += xGroupsLFD[g].expHit;
				expNFD += xGroupsLFD[g].expNoHit;
				obsFD += xGroupsLFD[g].obsHit;
				obsNFD += xGroupsLFD[g].obsNoHit;
				lfd_pMin = min(lfd_pMin, xGroupsLFD[g].pMin);
				lfd_pMax = max(lfd_pMax, xGroupsLFD[g].pMax);
				df++;
			}
			lfd_Chi2 = x2;
			if(df > 2)
				lfd_Chi2PVal = p_chisq((df - 2.0) / 2.0, x2 / 2.0);
			else
				lfd_Chi2PVal = 0.0;
			sstot = -2.0 * (obsFD * log((double)obsFD/(double)tnv) + obsNFD * log((double)obsNFD/(double)tnv));
			for(g = 0; g < nLFDG; g++)
			{
				double p = xGroupsLFD[g].expHit / (double)(xGroupsLFD[g].obsHit + xGroupsLFD[g].obsNoHit);
				if(p > 0.0 && p < 1.0)
					sse1 += ((double)xGroupsLFD[g].obsHit) * log(p)
						+ ((double)xGroupsLFD[g].obsNoHit) * log(1.0 - p);
				p = ((double)xGroupsLFD[g].obsHit) / (double)(xGroupsLFD[g].obsHit + xGroupsLFD[g].obsNoHit);
				if(p > 0.0 && p < 1.0)
					sses += ((double)xGroupsLFD[g].obsHit) * log(p)
						+ ((double)xGroupsLFD[g].obsNoHit) * log(1.0 - p);
			}
			sse1 *= -2.0;
			sses *= -2.0;
			sse1 = min(sse1, sstot);
			sses = min(sses, sstot);
			ssr1 = sstot - sse1;
			ssrs = sstot - sses;
			if(ssrs != 0.0)
				lfd_R2 = ssr1/ssrs;
		}

		//do MFD's
		x2 = expFD = expNFD = df = sstot = sse1 = ssr1 = sses = ssrs = 0.0;0.0;
		obsFD = obsNFD = 0;
		if(nmfd > 0)
		{
			mfd_pMin = 1.0, mfd_pMax = 0.0;
			for(g = 0; g < nMFDG; g++)
			{
				x2 += xGroupsMFD[g].CalcChi2();
				expFD += xGroupsMFD[g].expHit;
				expNFD += xGroupsMFD[g].expNoHit;
				obsFD += xGroupsMFD[g].obsHit;
				obsNFD += xGroupsMFD[g].obsNoHit;
				mfd_pMin = min(mfd_pMin, xGroupsMFD[g].pMin);
				mfd_pMax = max(mfd_pMax, xGroupsMFD[g].pMax);
				df++;
			}
			mfd_Chi2 = x2;
			if(df > 2)
				mfd_Chi2PVal = p_chisq((df - 2.0) / 2.0, x2 / 2.0);
			else
				fd_Chi2PVal = 0.0;
			sstot = -2.0 * (obsFD * log((double)obsFD/(double)tnv) + obsNFD * log((double)obsNFD/(double)tnv));
			for(g = 0; g < nMFDG; g++)
			{
				double p = xGroupsMFD[g].expHit / (double)(xGroupsMFD[g].obsHit + xGroupsMFD[g].obsNoHit);
				if(p > 0.0 && p < 1.0)
					sse1 += ((double)xGroupsMFD[g].obsHit) * log(p)
						+ ((double)xGroupsMFD[g].obsNoHit) * log(1.0 - p);
				p = ((double)xGroupsMFD[g].obsHit) / (double)(xGroupsMFD[g].obsHit + xGroupsMFD[g].obsNoHit);
				if(p > 0.0 && p < 1.0)
					sses += ((double)xGroupsMFD[g].obsHit) * log(p)
						+ ((double)xGroupsMFD[g].obsNoHit) * log(1.0 - p);
			}
			sse1 *= -2.0;
			sses *= -2.0;
			sse1 = min(sse1, sstot);
			sses = min(sses, sstot);
			ssr1 = sstot - sse1;
			ssrs = sstot - sses;
			if(ssrs != 0.0)
				mfd_R2 = ssr1/ssrs;
		}
	}
	//end base statistics for candidates

	//now get info from database
	if(fpSet->m_SIG_Station.GetLength() > 6)//SIG
	{
		CSIGStationSet staSet(fpSet->m_pDatabase);
		staSet.Open();
		CStationInSIGSet stas(fpSet->m_pDatabase);
		char tSig[64];
		strcpy(tSig, fpSet->m_SIG_Station);
		stas.m_strFilter.Format("[SIG] = '%s'", &tSig[6]);
		stas.Open();
		while(!stas.IsEOF())
		{
			nStrings++;
			stas.MoveNext();
		}
		if(nStrings > 0)
		{
			strings = new CString[nStrings];
			stas.MoveFirst();
			int loc = 0;
			while(!stas.IsEOF())
			{
				staSet.m_strFilter.Format("[StationID] = '%s'", stas.m_StationID);
				staSet.Requery();
				strings[loc].Format("%6.6s - %-20.20s Model: %s      Weight: %.2f\n",
						staSet.m_StationID, staSet.m_Name, GetModelString(&staSet), stas.m_WeightFactor);
				if(loc == 0)//get graphModel string from first station
				{
					CString tStr = GetModelString(&staSet);
					if(tStr.GetLength() > 2)
						//graphModelString.Format("Model: %2.2s", tStr);
						graphModelString.Format("%2.2s", tStr);
					else
						graphModelString = "";
				}
				loc++;
				stas.MoveNext();
			}
		}
		staStr = fpSet->m_SIG_Station;
		stas.Close();
		staSet.Close();
	}
	else
	{
		CSIGStationSet staSet(fpSet->m_pDatabase);
		//staSet.Open();
		staSet.m_strFilter.Format("[StationID] = '%s'", fpSet->m_SIG_Station);
		staSet.Open();
		nStrings = 1;
		strings = new CString[1];
		strings[0].Format("%-20.20s Model: %s\n", staSet.m_Name, GetModelString(&staSet));
		//graphModelString = strings[0];
		graphModelString = GetModelString(&staSet);
		staStr = staSet.m_StationID + "-" + staSet.m_Name;
		//graphModelString = GetModelString(&staSet);
		staSet.Close();
	}
	//
	dateStamp = COleDateTime::GetCurrentTime();
}
/*
void CFireStats::Build(CFireplusSet *fpSet, bool probabilities, BOOL conditional)
{
	FILE *fd, *mfd, *lfd, *fdout, *lfdout, *mfdout;
	char buf[81];
	chdir(theApp.dbDir);//theApp.workDir);
	pDB = fpSet->m_pDatabase;
	m_pSet = fpSet;
	m_conditional = conditional;
	//get important values from output files
	if(probabilities)
	{
		fdout = fopen("fd.out", "rt");
		fgets(buf, 79, fdout);
		while(!feof(fdout) && strncmp(buf, "CONSTANT", 8) != 0)
			fgets(buf, 79, fdout);
		b0fd = atof(&buf[13]);
		fgets(buf, 79, fdout);
		b1fd = atof(&buf[13]);
		fgets(buf, 79, fdout);
		fgets(buf, 79, fdout);
		fgets(buf, 79, fdout);
		devfd = atof(&buf[9]);
		pfd = atof(&buf[41]);
		fclose(fdout);
		lfdout = fopen("lfd.out", "rt");
		fgets(buf, 79, lfdout);
		while(!feof(lfdout) && strncmp(buf, "CONSTANT", 8) != 0)
			fgets(buf, 79, lfdout);
		b0lfd = atof(&buf[13]);
		fgets(buf, 79, lfdout);
		b1lfd = atof(&buf[13]);
		fgets(buf, 79, lfdout);
		fgets(buf, 79, lfdout);
		fgets(buf, 79, lfdout);
		devlfd = atof(&buf[9]);
		plfd = atof(&buf[41]);
		fclose(lfdout);
		mfdout = fopen("mfd.out", "rt");
		fgets(buf, 79, mfdout);
		while(!feof(mfdout) && strncmp(buf, "CONSTANT", 8) != 0)
			fgets(buf, 79, mfdout);
		b0mfd = atof(&buf[13]);
		fgets(buf, 79, mfdout);
		b1mfd = atof(&buf[13]);
		fgets(buf, 79, mfdout);
		fgets(buf, 79, mfdout);
		fgets(buf, 79, mfdout);
		devmfd = atof(&buf[9]);
		pmfd = atof(&buf[41]);
		fclose(mfdout);
	}
	fd = fopen("fd.dat", "rt");
	fgets(buf, 79, fd);//strip first line
	fgets(buf, 79, fd);//read second line
	long v = 0, f = 0;
	while(!feof(fd) && v < nv)
	{
		allVals[v++] = atof(&buf[2]);
		if(atoi(buf) == 1)
			fdVals[f++] = atof(&buf[2]);
		fgets(buf, 79, fd);
	}
	if(probabilities)
		xGroupsFD = GoodFit(fd, &nFDG, FD, m_conditional);
	fclose(fd);
	f = 0;
	lfd = fopen("lfd.dat", "rt");
	fgets(buf, 79, lfd);//strip first line
	fgets(buf, 79, lfd);//read second line
	while(!feof(lfd) && f < nlfd)
	{
		if(atoi(buf) == 1)
			lfdVals[f++] = atof(&buf[2]);
		fgets(buf, 79, lfd);
	}
	if(probabilities)
		xGroupsLFD = GoodFit(lfd, &nLFDG, LFD, m_conditional);
	fclose(lfd);
	f = 0;
	mfd = fopen("mfd.dat", "rt");
	fgets(buf, 79, mfd);//strip first line
	fgets(buf, 79, mfd);//read second line
	while(!feof(mfd) && f < nmfd)
	{
		if(atoi(buf) == 1)
			mfdVals[f++] = atof(&buf[2]);
		fgets(buf, 79, mfd);
	}
	if(probabilities)
		xGroupsMFD = GoodFit(mfd, &nMFDG, MFD, m_conditional);
	fclose(mfd);

	//now sort the arrays
	if(nv > 1)
		qsort( (void *)allVals, (size_t)nv, sizeof( double ), compare );
	if(nfd > 1)
		qsort( (void *)fdVals, (size_t)nfd, sizeof( double ), compare );
	if(nlfd > 1)
		qsort( (void *)lfdVals, (size_t)nlfd, sizeof( double ), compare );
	if(nmfd > 1)
		qsort( (void *)mfdVals, (size_t)nmfd, sizeof( double ), compare );

	//calculate base statistics for Candidates...
	if(probabilities)
	{
		int g;
		//do FD's
		double x2 = 0.0, expFD = 0.0, expNFD = 0.0, df = 0.0, sstot = 0.0, sse1 = 0.0, ssr1 = 0.0,
			sses = 0.0, ssrs = 0.0;
		long obsFD = 0, obsNFD = 0, tnv = nv;
		if(nfd > 0)
		{
			fd_pMin = 1.0, fd_pMax = 0.0;
			for(g = 0; g < nFDG; g++)
			{
				x2 += xGroupsFD[g].CalcChi2();
				expFD += xGroupsFD[g].expHit;
				expNFD += xGroupsFD[g].expNoHit;
				obsFD += xGroupsFD[g].obsHit;
				obsNFD += xGroupsFD[g].obsNoHit;
				fd_pMin = min(fd_pMin, xGroupsFD[g].pMin);
				fd_pMax = max(fd_pMax, xGroupsFD[g].pMax);
				df++;
			}
			fd_Chi2 = x2;
			if(df > 2)
				fd_Chi2PVal = p_chisq((df - 2.0) / 2.0, x2 / 2.0);
			else
				fd_Chi2PVal = 0.0;
			sstot = -2.0 * (obsFD * log((double)obsFD/(double)tnv) + obsNFD * log((double)obsNFD/(double)tnv));
			for(g = 0; g < nFDG; g++)
			{
				double p = xGroupsFD[g].expHit / (double)(xGroupsFD[g].obsHit + xGroupsFD[g].obsNoHit);
				if(p > 0.0 && p < 1.0)
					sse1 += ((double)xGroupsFD[g].obsHit) * log(p)
						+ ((double)xGroupsFD[g].obsNoHit) * log(1.0 - p);
				p = ((double)xGroupsFD[g].obsHit) / (double)(xGroupsFD[g].obsHit + xGroupsFD[g].obsNoHit);
				if(p > 0.0 && p < 1.0)
					sses += ((double)xGroupsFD[g].obsHit) * log(p)
						+ ((double)xGroupsFD[g].obsNoHit) * log(1.0 - p);
			}
			sse1 *= -2.0;
			sses *= -2.0;
			sse1 = min(sse1, sstot);
			sses = min(sses, sstot);
			ssr1 = sstot - sse1;
			ssrs = sstot - sses;
			if(ssrs != 0.0)
				fd_R2 = ssr1/ssrs;
		}

		//do LFD's
		if(conditional)
			tnv = nfd;
		x2 = expFD = expNFD = df = sstot = sse1 = ssr1 = sses = ssrs = 0.0;0.0;
		obsFD = obsNFD = 0;
		if(nlfd > 0)
		{
			lfd_pMin = 1.0, lfd_pMax = 0.0;
			for(g = 0; g < nLFDG; g++)
			{
				x2 += xGroupsLFD[g].CalcChi2();
				expFD += xGroupsLFD[g].expHit;
				expNFD += xGroupsLFD[g].expNoHit;
				obsFD += xGroupsLFD[g].obsHit;
				obsNFD += xGroupsLFD[g].obsNoHit;
				lfd_pMin = min(lfd_pMin, xGroupsLFD[g].pMin);
				lfd_pMax = max(lfd_pMax, xGroupsLFD[g].pMax);
				df++;
			}
			lfd_Chi2 = x2;
			if(df > 2)
				lfd_Chi2PVal = p_chisq((df - 2.0) / 2.0, x2 / 2.0);
			else
				lfd_Chi2PVal = 0.0;
			sstot = -2.0 * (obsFD * log((double)obsFD/(double)tnv) + obsNFD * log((double)obsNFD/(double)tnv));
			for(g = 0; g < nLFDG; g++)
			{
				double p = xGroupsLFD[g].expHit / (double)(xGroupsLFD[g].obsHit + xGroupsLFD[g].obsNoHit);
				if(p > 0.0 && p < 1.0)
					sse1 += ((double)xGroupsLFD[g].obsHit) * log(p)
						+ ((double)xGroupsLFD[g].obsNoHit) * log(1.0 - p);
				p = ((double)xGroupsLFD[g].obsHit) / (double)(xGroupsLFD[g].obsHit + xGroupsLFD[g].obsNoHit);
				if(p > 0.0 && p < 1.0)
					sses += ((double)xGroupsLFD[g].obsHit) * log(p)
						+ ((double)xGroupsLFD[g].obsNoHit) * log(1.0 - p);
			}
			sse1 *= -2.0;
			sses *= -2.0;
			sse1 = min(sse1, sstot);
			sses = min(sses, sstot);
			ssr1 = sstot - sse1;
			ssrs = sstot - sses;
			if(ssrs != 0.0)
				lfd_R2 = ssr1/ssrs;
		}

		//do MFD's
		x2 = expFD = expNFD = df = sstot = sse1 = ssr1 = sses = ssrs = 0.0;0.0;
		obsFD = obsNFD = 0;
		if(nmfd > 0)
		{
			mfd_pMin = 1.0, mfd_pMax = 0.0;
			for(g = 0; g < nMFDG; g++)
			{
				x2 += xGroupsMFD[g].CalcChi2();
				expFD += xGroupsMFD[g].expHit;
				expNFD += xGroupsMFD[g].expNoHit;
				obsFD += xGroupsMFD[g].obsHit;
				obsNFD += xGroupsMFD[g].obsNoHit;
				mfd_pMin = min(mfd_pMin, xGroupsMFD[g].pMin);
				mfd_pMax = max(mfd_pMax, xGroupsMFD[g].pMax);
				df++;
			}
			mfd_Chi2 = x2;
			if(df > 2)
				mfd_Chi2PVal = p_chisq((df - 2.0) / 2.0, x2 / 2.0);
			else
				fd_Chi2PVal = 0.0;
			sstot = -2.0 * (obsFD * log((double)obsFD/(double)tnv) + obsNFD * log((double)obsNFD/(double)tnv));
			for(g = 0; g < nMFDG; g++)
			{
				double p = xGroupsMFD[g].expHit / (double)(xGroupsMFD[g].obsHit + xGroupsMFD[g].obsNoHit);
				if(p > 0.0 && p < 1.0)
					sse1 += ((double)xGroupsMFD[g].obsHit) * log(p)
						+ ((double)xGroupsMFD[g].obsNoHit) * log(1.0 - p);
				p = ((double)xGroupsMFD[g].obsHit) / (double)(xGroupsMFD[g].obsHit + xGroupsMFD[g].obsNoHit);
				if(p > 0.0 && p < 1.0)
					sses += ((double)xGroupsMFD[g].obsHit) * log(p)
						+ ((double)xGroupsMFD[g].obsNoHit) * log(1.0 - p);
			}
			sse1 *= -2.0;
			sses *= -2.0;
			sse1 = min(sse1, sstot);
			sses = min(sses, sstot);
			ssr1 = sstot - sse1;
			ssrs = sstot - sses;
			if(ssrs != 0.0)
				mfd_R2 = ssr1/ssrs;
		}
	}
	//end base statistics for candidates

	//now get info from database
	if(fpSet->m_SIG_Station.GetLength() > 6)//SIG
	{
		CSIGStationSet staSet(fpSet->m_pDatabase);
		staSet.Open();
		CStationInSIGSet stas(fpSet->m_pDatabase);
		char tSig[64];
		strcpy(tSig, fpSet->m_SIG_Station);
		stas.m_strFilter.Format("[SIG] = '%s'", &tSig[6]);
		stas.Open();
		while(!stas.IsEOF())
		{
			nStrings++;
			stas.MoveNext();
		}
		if(nStrings > 0)
		{
			strings = new CString[nStrings];
			stas.MoveFirst();
			int loc = 0;
			while(!stas.IsEOF())
			{
				staSet.m_strFilter.Format("[StationID] = '%s'", stas.m_StationID);
				staSet.Requery();
				strings[loc].Format("%6.6s - %-20.20s Model: %s      Weight: %.2f\n",
						staSet.m_StationID, staSet.m_Name, GetModelString(&staSet), stas.m_WeightFactor);
				if(loc == 0)//get graphModel string from first station
				{
					CString tStr = GetModelString(&staSet);
					if(tStr.GetLength() > 2)
						//graphModelString.Format("Model: %2.2s", tStr);
						graphModelString.Format("%2.2s", tStr);
					else
						graphModelString = "";
				}
				loc++;
				stas.MoveNext();
			}
		}
		staStr = fpSet->m_SIG_Station;
		stas.Close();
		staSet.Close();
	}
	else
	{
		CSIGStationSet staSet(fpSet->m_pDatabase);
		//staSet.Open();
		staSet.m_strFilter.Format("[StationID] = '%s'", fpSet->m_SIG_Station);
		staSet.Open();
		nStrings = 1;
		strings = new CString[1];
		strings[0].Format("%-20.20s Model: %s\n", staSet.m_Name, GetModelString(&staSet));
		//graphModelString = strings[0];
		graphModelString = GetModelString(&staSet);
		staStr = staSet.m_StationID + "-" + staSet.m_Name;
		//graphModelString = GetModelString(&staSet);
		staSet.Close();
	}
	//
	dateStamp = COleDateTime::GetCurrentTime();
}
*/
CString CFireStats::GetModelString(CSIGStationSet *stn)
{
	CString ret;
	if (isNFDRS)
	{
		if (isNFDRS2016(stn->m_NFDRSFM[0]))
			ret.Format("%s%d%c%d", stn->m_NFDRSFM, stn->m_SlopeCls,
				stn->m_HerbAnnual ? 'A' : 'P', stn->m_ClimateCls);
		else
			ret.Format("%d%s%d%c%c%d", stn->m_Use88 ? 8 : 7, stn->m_NFDRSFM, stn->m_SlopeCls,
			stn->m_HerbAnnual ? 'A' : 'P', stn->m_Deciduous ? 'D' : 'E', stn->m_ClimateCls);
	}
	else
		ret = "";
	return ret;
}

const CString strFD =   " FD";
const CString strNFD =  " No-FD";
const CString strLFD =  "LFD";
const CString strNLFD = "No-LFD";
const CString strMFD =  "MFD";
const CString strNMFD = "No-MFD";

const CString xsquareHeader =
	"                                   %s      %s          %s\n"
	" Prob. Range     %s Range   Days  Pct   Obs    Exp    Obs    Exp   Chi-Square\n"//   P\n"
	"----------------------------------------------------------------------------------\n";//--------\n";
CString CFireStats::ExportPercentiles(void)
{
	char *fName = GetTempFileName();
	CString ret = fName;
	free(fName);
	FILE *stream = fopen(ret, "wt");
	fprintf(stream, "FireFamily Plus Fire-Day Percentiles Report\n");

	// added for batch
	// (code interspersed)
	CReportOptionsSet reportSet(m_pSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", m_pSet->m_pDatabase->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,m_pSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch

	reportSet.MoveFirst();
	int Delimiter = reportSet.m_Delimiter;
	reportSet.Close();

	//fprintf(stream, "Station: %s\n", staStr);
	fprintf(stream, "Variable: %s\n", varName);
	//fprintf(stream, "Model: %s\n", graphModelString);
	fprintf(stream,
			"Large Fire Day = %d acres\n"
			"Multiple Fire Day = %d fires\n",
			lfAcres, mfdFires);

	/* fprintf(stream, "Time Frame: %d/%d - %d/%d\n",
		start.GetMonth(),
		start.GetDay(),
		end.GetMonth(),
		end.GetDay());

		fprintf(stream, "Data Years: %d - %d\n",
		start.GetYear(),
		end.GetYear());
	*/
	switch(Delimiter){
		case 0:
			fprintf(stream,
				"          Percentiles\n"
				"%6.6s     All Days Fire-Day   Multi     Large\n",
				varName);
			break;
		case 1:
			fprintf(stream,
				"          Percentiles\n"
				"%6.6s\tAll Days\tFire-Day\tMulti\tLarge\n",
				varName);
			break;
		case 2:
			fprintf(stream,
				"          Percentiles\n"
				"%6.6s,All Days,Fire-Day,Multi,Large\n",
				varName);
			break;
		case 3:
		case 4:
			fprintf(stream,
				"          Percentiles\n"
				"%6.6s|All Days|Fire-Day|Multi;Large\n",
				varName);
			break;
	}
	int maxVal = nv > 0 ? allVals[nv - 1] : 0.0, minVal = nv > 0 ? allVals[0] : 0.0, range, loc = 0;
	range = maxVal - minVal + 1;
	long *all, *fd, *lfd, *mfd, lastVal = 0;
	all = new long[range];
	fd = new long[range];
	lfd = new long[range];
	mfd = new long[range];
	int i;
	for(i = 0; i < range; i++)
		all[i] = fd[i] = lfd[i] = mfd[i] = 0;
	for(i = 0; i < nv; i++)
		all[(int)floor(allVals[i]) - minVal]++;
	for(i = 0; i < nfd; i++)
		fd[(int)floor(fdVals[i]) - minVal]++;
	for(i = 0; i < nlfd; i++)
		lfd[(int)floor(lfdVals[i]) - minVal]++;
	for(i = 0; i < nmfd; i++)
		mfd[(int)floor(mfdVals[i]) - minVal]++;
	long a = 0, f = 0, l = 0, m = 0;
	for(i = 0; i < range; i++)
	{
		a += all[i];
		f += fd[i];
		l += lfd[i];
		m += mfd[i];
		switch (Delimiter){
			case 0:
				fprintf(stream, " %5d    %6.2f    %6.2f    %6.2f    %6.2f\n",
						i + minVal,
						(nv > 0) ? ((double)a) / ((double)nv) * 100.0 : 0.0,
						(nfd > 0) ? ((double)f) / ((double)nfd) * 100.0 : 0.0,
						(nmfd > 0) ? ((double)m) / ((double)nmfd) * 100.0 : 0.0,
						(nlfd > 0) ? ((double)l) / ((double)nlfd) * 100.0 : 0.0);
				break;
			case 1:
				fprintf(stream, " %5d\t%6.2f\t%6.2f\t%6.2f\t%6.2f\n",
						i + minVal,
						(nv > 0) ? ((double)a) / ((double)nv) * 100.0 : 0.0,
						(nfd > 0) ? ((double)f) / ((double)nfd) * 100.0 : 0.0,
						(nmfd > 0) ? ((double)m) / ((double)nmfd) * 100.0 : 0.0,
						(nlfd > 0) ? ((double)l) / ((double)nlfd) * 100.0 : 0.0);
				break;
			case 2:
				fprintf(stream, " %5d,%6.2f,%6.2f,%6.2f,%6.2f\n",
						i + minVal,
						(nv > 0) ? ((double)a) / ((double)nv) * 100.0 : 0.0,
						(nfd > 0) ? ((double)f) / ((double)nfd) * 100.0 : 0.0,
						(nmfd > 0) ? ((double)m) / ((double)nmfd) * 100.0 : 0.0,
						(nlfd > 0) ? ((double)l) / ((double)nlfd) * 100.0 : 0.0);
				break;
			case 3:
			case 4:
				fprintf(stream, " %5d|%6.2f|%6.2f|%6.2f|%6.2f\n",
						i + minVal,
						(nv > 0) ? ((double)a) / ((double)nv) * 100.0 : 0.0,
						(nfd > 0) ? ((double)f) / ((double)nfd) * 100.0 : 0.0,
						(nmfd > 0) ? ((double)m) / ((double)nmfd) * 100.0 : 0.0,
						(nlfd > 0) ? ((double)l) / ((double)nlfd) * 100.0 : 0.0);
				break;
		}
	}
	fclose(stream);
	delete[] all;
	delete[] fd;
	delete[] lfd;
	delete[] mfd;
	return ret;
}

void CFireStats::StatsReport(FILE *stream, int firesType, BOOL conditional, int round)
{//
	CString varStr, obsStr, nonObsStr;
	double x2 = 0.0, df = 0.0, b0, b1, sstot = 0.0, sse1 = 0.0, ssr1 = 0.0,
		sses = 0.0, ssrs = 0.0, expFD = 0.0, expNFD = 0.0, pct;
	long hits, obsFD = 0, obsNFD = 0, tnv = nv;
	if(conditional && firesType != FD)
		tnv = nfd;
	int grps;
	ChiSquareGroup *xGroup = NULL;
	switch(firesType)
	{
	case FD:
		varStr = "Fire-Day";
		hits = nfd;
		grps = nFDG;
		b0 = b0fd;
		b1 = b1fd;
		xGroup = xGroupsFD;
		obsStr = strFD;
		nonObsStr = strNFD;
		break;
	case LFD:
		varStr = "Large-Fire-Day";
		hits = nlfd;
		grps = nLFDG;
		b0 = b0lfd;
		b1 = b1lfd;
		xGroup = xGroupsLFD;
		obsStr = strLFD;
		nonObsStr = strNLFD;
		break;
	case MFD:
		varStr = "Multiple-Fire-Day";
		hits = nmfd;
		grps = nMFDG;
		b0 = b0mfd;
		b1 = b1mfd;
		xGroup = xGroupsMFD;
		obsStr = strMFD;
		nonObsStr = strNMFD;
		break;
	default://safety
		return;
	}
	fprintf(stream,
		"\n%s:%s\n"
		"    P(%s) = 1 / (1 + exp(-1 * %.4f + (-1 * %.4f) * %s))\n",
		varStr, (conditional && firesType != FD) ? " (Conditional on Fire-Day)" : "", varStr,
		b0, b1, shortName);
	fprintf(stream,
		"      Number of %s: %ld\n"
		"      Number of %ss: %ld\n",
		(conditional && firesType != FD) ? "Fire-Days" : "Weather-Days",
		(conditional && firesType != FD) ? nfd : nv, varStr, hits);
	//output chi-Square goodness of fit results
	if(hits == 0)
	{
		fprintf(stream, "Error: Zero observed %ss.\nModel for %s is invalid\n\n",
			varStr, varStr);
		return;
	}
	fprintf(stream,
		"Chi-Squared Goodness of Fit Tests for %s\n", varStr);
	fprintf(stream, xsquareHeader, obsStr, obsStr, nonObsStr, shortName, shortName);
	//fprintf(stream, "%s\n", xsquareHeader);
	int g;
	for(g = 0; g < grps; g++)
	{
		x2 += xGroup[g].Output(stream, round);
		expFD += xGroup[g].expHit;
		expNFD += xGroup[g].expNoHit;
		obsFD += xGroup[g].obsHit;
		obsNFD += xGroup[g].obsNoHit;
		df++;
	}
	if(obsFD + obsNFD > 0)
		pct = ((double)obsFD) / ((double)(obsFD + obsNFD)) * 100.0;
	else
		pct = 0.0;
	fprintf(stream, "----------------------------------------------------------------------------------\n");
	fprintf(stream, "                            %6ld %3.0f %6ld %6.0f %6ld %6.0f %9.1f\n",
		(conditional && firesType != FD) ? nfd : nv, pct, obsFD, expFD, obsNFD, expNFD, x2);
	fprintf(stream, "----------------------------------------------------------------------------------\n");
	//fprintf(stream, "Chi Square    DF   P-Value     SSTOT    SSE(1)    SSR(1)   DF   P-Value   R(L)-Sq.\n");
	fprintf(stream, "Chi Square    DF   P-Value     R(L)-Sq.\n");
	sstot = -2.0 * (obsFD * log((double)obsFD/(double)tnv) + obsNFD * log((double)obsNFD/(double)tnv));
	for(g = 0; g < grps; g++)
	{
		double p = xGroup[g].expHit / (double)(xGroup[g].obsHit + xGroup[g].obsNoHit);
		if(p > 0.0 && p < 1.0)
			sse1 += ((double)xGroup[g].obsHit) * log(p)
				+ ((double)xGroup[g].obsNoHit) * log(1.0 - p);
		p = ((double)xGroup[g].obsHit) / (double)(xGroup[g].obsHit + xGroup[g].obsNoHit);
		if(p > 0.0 && p < 1.0)
			sses += ((double)xGroup[g].obsHit) * log(p)
				+ ((double)xGroup[g].obsNoHit) * log(1.0 - p);
	}
	sse1 *= -2.0;
	sses *= -2.0;
	sse1 = min(sse1, sstot);
	sses = min(sses, sstot);
	ssr1 = sstot - sse1;
	ssrs = sstot - sses;
/*	if(grps <= 2)
	{
		if(ssrs != 0)
			fprintf(stream, "     NA       NA       NA  %9.1f %9.1f %9.1f    1   %7.4f     %.2f\n",
				sstot, sse1, ssr1, p_chisq(0.5, ssr1/2.0), ssr1/ssrs);
		else
			fprintf(stream, "     NA       NA       NA  %9.1f %9.1f %9.1f    1   %7.4f     NA\n",
				sstot, sse1, ssr1, p_chisq(0.5, ssr1/2.0));
	}
	else
	{
		if(ssrs != 0)
			fprintf(stream, "%9.1f %6.0f   %7.4f %9.1f %9.1f %9.1f    1   %7.4f     %.2f\n",
				x2, df - 2.0, p_chisq((df - 2.0) / 2.0, x2 / 2.0),
				sstot, sse1, ssr1, p_chisq(0.5, ssr1/2.0), ssr1/ssrs);
		else
			fprintf(stream, "%9.1f %6.0f   %7.4f %9.1f %9.1f %9.1f    1   %7.4f     NA\n",
				x2, df - 2.0, p_chisq((df - 2.0) / 2.0, x2 / 2.0),
				sstot, sse1, ssr1, p_chisq(0.5, ssr1/2.0));
	}
	fprintf(stream, "                                        SSE(S)    SSR(S)\n");
	fprintf(stream, "                                     %9.1f %9.1f\n",
		sses, ssrs);*/

	if(grps <= 2)
	{
		if(ssrs != 0)
			fprintf(stream, "     NA       NA       NA  %.2f\n",
				sstot, sse1, ssr1, p_chisq(0.5, ssr1/2.0), ssr1/ssrs);
		else
			fprintf(stream, "     NA       NA       NA  NA\n");
	}
	else
	{
		if(ssrs != 0)
			fprintf(stream, "%9.1f %6.0f   %7.4f     %.2f\n",
				x2, df - 2.0, p_chisq((df - 2.0) / 2.0, x2 / 2.0),
				ssr1/ssrs);
		else
			fprintf(stream, "%9.1f %6.0f   %7.4f     NA\n",
				x2, df - 2.0, p_chisq((df - 2.0) / 2.0, x2 / 2.0));
	}
	//fprintf(stream, "                                        SSE(S)    SSR(S)\n");
	//fprintf(stream, "                                     %9.1f %9.1f\n",
	//	sses, ssrs);

	fprintf(stream, "\n\n");
}

double CFireStats::PercentileFromValue(int valType, double val)
{
	double ret = 0.0;
	double *valPtr = NULL;
	long nVals = 0;
	switch(valType)
	{
	case 0:
		valPtr = allVals;
		nVals = nv;
		break;
	case 1:
		valPtr = fdVals;
		nVals = nfd;
		break;
	case 2:
		valPtr = lfdVals;
		nVals = nlfd;
		break;
	case 3:
		nVals = nmfd;
		valPtr = mfdVals;
		break;
	}
	if(nVals > 0)
	{
		if(val < valPtr[0])
			return 0.0;
		if(val >= valPtr[nVals - 1])
			return 1.0;
		//traverse from end searching for the value
		for(long loc = nVals - 1; loc >= 0; loc--)
		{
			if(valPtr[loc] <= val)
				return ((double) loc) / (double) nVals;
		}
	}
	return ret;
}

CString CFireStats::LogRegReport(bool isNelson/* = false*/, bool isHalfNelson/* = false*/)
{
	char *fName = GetTempFileName();
	CString ret = fName;
	free(fName);
	FILE *stream = fopen(ret, "wt");
	CString str, temp;
	fprintf(stream,
		"FireFamily Plus Fires Analysis\n");
	fprintf(stream, "%s\nVariable: %s\n", staStr,
		varName);
	if(isHalfNelson)
		fprintf(stream,
			"\tUsing 1hr and 10hr Nelson Moistures\n");
	else if(isNelson)
		fprintf(stream,
			"\tUsing 1hr, 10hr, 100hr and 1000hr Nelson Moistures\n");
	fprintf(stream, "\nModel: %s\n", graphModelString);
	fprintf(stream, " Time Frame: %d/%d - %d/%d\n",
		start.GetMonth(),
		start.GetDay(),
		end.GetMonth(),
		end.GetDay());
	fprintf(stream, " Data Years: %d - %d\n",
		start.GetYear(),
		end.GetYear());
	switch(cause)
	{
	case 0://all fires
		fprintf(stream, " Cause = All\n");
		break;
	case 1://lightning
		fprintf(stream, " Cause = Lightning\n");
		break;
	case 2://human
		
		if (humanCause != 1022)
		{
			 fprintf(stream," Cause = ");
			 	for (int i=2; i <= 20; i++)
				{
					int testFlag = (humanCause >> (i-1)) & 1;
					if (testFlag == 1)
					{
						bool other = false;

						switch (i)
						{
						case 2: fprintf(stream,"Equipment "); break;
						case 3: fprintf(stream,"Smoking "); break;
						case 4: fprintf(stream,"Campfire "); break;
						case 5: fprintf(stream,"Burning "); break;
						case 6: fprintf(stream,"Railroad "); break;
						case 7: fprintf(stream,"Arson "); break;
						case 8: fprintf(stream,"Children "); break;
						case 9: fprintf(stream,"Misc "); break;
						default: other = true;

						}
						if (other)
							fprintf(stream,"Other "); 
					}
				}
		}  else
			fprintf(stream, " Cause = Human");
		fprintf(stream,"\n");

		break;
	}
	fprintf(stream,
		" Large Fire Day (LFD)= %d acres\n"
		" Multiple Fire Day (MFD)= %d fires\n\n",
		lfAcres, mfdFires);
	int round = TRUE;
	if(strncmp(staStr, "SIG", 3) != 0)
		//a single station
		fprintf(stream, " %s - %s", staStr, strings[0]);
	else
	{
		//round = FALSE;
		fprintf(stream, " Stations in %s:\n", staStr);
		for(int i = 0; i < nStrings; i++)
			fprintf(stream, " %s", strings[i]);
	}
	if(varID == 9)//precip
		round = FALSE;
	fprintf(stream,
		"\n**** Created data for %ld records with missing values. ****\n"
		"  (Maximum of %d consecutive missing days for replacement)\n", nMadeUps, theApp.maxMissingWx);
	fprintf(stream, "%ld fires discarded due to no/missing weather. \n", nDiscards);
	//now dump goodness of fit reports
	StatsReport(stream, FD, m_conditional, round);
	StatsReport(stream, LFD, m_conditional, round);
	StatsReport(stream, MFD, m_conditional, round);
	fprintf(stream, "\n%s\n", AddDateStamp());
	fclose(stream);
	return ret;
}
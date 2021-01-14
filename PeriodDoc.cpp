// PeriodDoc.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "wxset.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "OverlayDialog.h"
//#include "perioddoc.h"
//#include "splitterframe.h"
#include "SIGStationSet.h"
#include "ClimateSet.h"
#include "fireplusSet.h"
#include "FireSumSet.h"
#include "ClimAnalysis.h"
#include "ReportOptionsSet.h"
#include <afxrich.h>
#include "richdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern int curr_runID;

CForecast::CForecast()
{
	nDays = nBins = 0;
	dates = NULL;
	for(int i = 0; i < MAXVARIDS; i++)
	{
		values[i] = NULL;
		binVals[i] = NULL;
	}
	lineColor = 0;
	lineWidth = 1;
	lineStyle = 0;
}

CForecast::~CForecast()
{
	delete[] dates;
	for(int i = 0; i < MAXVARIDS; i++)
	{
		if(values[i])
			delete[] values[i];
		if(binVals[i])
			delete[] binVals[i];
	}
}

void CForecast::CreateStorage(int days, int nVars, int *varIDs)
{
	nDays = days;
	dates = new COleDateTime[nDays];
	for(int v = 0; v < nVars; v++)
		values[varIDs[v]] = new double[nDays];
}

void CForecast::FillBins(CClimAnalysis *analysis)
{
	//figure # bins
	startP = -1;
	int p = analysis->GetPeriod(dates[0]);
	if(p >= 0)
	{
		nBins = 1;
		startP = p;
	}
	for(int d = 1; d < nDays; d++)
	{
		int q = analysis->GetPeriod(dates[d]);
		if(p != q && (q >= 0))
		{
			nBins++;
			if(startP < 0)
				startP = q;
			p = q;
		}
	}
	if(nBins == 0)
		return;
	//allocate storage
	int v;
	for(v = 0; v < analysis->nVarIDs; v++)
	{
		int varID = analysis->varIDs[v];
		binVals[varID] = new double[nBins];
		for(int b = 0; b < nBins; b++)
			binVals[varID][b] = 0.0;
	}
	//fill the bins
	Stats *stats = new Stats[nBins];
	for(v = 0; v < analysis->nVarIDs; v++)
	{
		int varID = analysis->varIDs[v];
		for(int d = 0; d < nDays; d++)
		{
			p = analysis->GetPeriod(dates[d]);
			if(p >= 0)
				stats[p - startP].Accumulate(values[varID][d]);
		}
		for(int b = 0; b < nBins; b++)
		{
			stats[b].Complete();
			binVals[varID][b] = stats[b].Mean();
			stats[b].Reset();
		}
	}
	delete[] stats;
	//set overlap variable for graphing
	COleDateTimeSpan oneDay(1, 0, 0, 0);
	p = analysis->GetPeriod(dates[0] - oneDay);
	if(p != startP)
		overlap = false;
	else
		overlap = true;
}

/////////////////////////////////////////////////////////////////////////////
// CPeriodDoc

IMPLEMENT_DYNCREATE(CPeriodDoc, CDocument)

CPeriodDoc::CPeriodDoc()
{
}

BOOL CPeriodDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CPeriodDoc::~CPeriodDoc()
{
	if(m_pAnalysis->m_opts.m_OverlayDialog)
		m_pAnalysis->m_opts.m_OverlayDialog->OnCancel();
	int years = 0, tYears = 0;
	if(m_pAnalysis->periods)
	{
		for(int j = 0; j < MAXVARIDS; j++)
		{
			if(m_pAnalysis->periods[j])
			{
				for(int i = 0; i < m_pAnalysis->m_opts.numPeriods; i++)
				{
					if(!years)
						years = m_pAnalysis->periods[j][i]->years;
					delete m_pAnalysis->periods[j][i];
				}
				delete[] m_pAnalysis->periods[j];
			}
		}
		if(m_pAnalysis->m_opts.numPeriods > 0)
			delete[] m_pAnalysis->periods;
	}
	m_pAnalysis->periods = NULL;
	if(m_pAnalysis->m_opts.tPeriods)
	{
		for(int j = 0; j < MAXVARIDS; j++)
		{
			if(m_pAnalysis->m_opts.tPeriods[j])
			{
				for(int i = 0; i < m_pAnalysis->m_opts.numtPeriods; i++)
				{
					if(!tYears)
						tYears = m_pAnalysis->m_opts.tPeriods[j][i]->years;
					delete m_pAnalysis->m_opts.tPeriods[j][i];
				}
				delete[] m_pAnalysis->m_opts.tPeriods[j];
			}
		}
		if(m_pAnalysis->m_opts.numtPeriods > 0)
			delete[] m_pAnalysis->m_opts.tPeriods;
	}
	m_pAnalysis->m_opts.tPeriods = NULL;
	if(m_pAnalysis->m_opts.fires)
	{
		for(int y = 0; y < years; y++)
			delete[] m_pAnalysis->m_opts.fires[y];
		delete[] m_pAnalysis->m_opts.fires;
		m_pAnalysis->m_opts.fires = NULL;
	}
	if(m_pAnalysis->m_opts.tFires)
	{
		for(int y = 0; y < tYears; y++)
			delete[] m_pAnalysis->m_opts.tFires[y];
		delete[] m_pAnalysis->m_opts.tFires;
		m_pAnalysis->m_opts.tFires = NULL;
	}
	delete m_pAnalysis;
}


BEGIN_MESSAGE_MAP(CPeriodDoc, CDocument)
	//{{AFX_MSG_MAP(CPeriodDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPeriodDoc diagnostics

#ifdef _DEBUG
void CPeriodDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPeriodDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPeriodDoc serialization

void CPeriodDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPeriodDoc commands


CString CPeriodDoc::StatsGraphDataListing(int varID, bool createDoc /*= true*/)
{
	CString retName = "";
// added for batch2
	// write the stats in text form to temporary file
	
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	fprintf(stream, "FireFamily Plus Fire-Day Climatology Stats\n");


	CReportOptionsSet reportSet(m_pAnalysis->m_opts.pDB);
	reportSet.Open();

	CString dStr;
	
	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", m_pAnalysis->m_opts.pDB->GetDatabaseName());
	

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	//reportSet.listAll(&headerStr,fpSet);
	//fprintf(stream,"%s",headerStr);
	CString extremeString = " ";
	if (m_pAnalysis->m_usedExtremes)
	{
		if (m_pAnalysis->m_extremeIsMax[varID])
			extremeString = " - Daily Max";
		else
			extremeString = " - Daily Min";
	}
	fprintf(stream,"\nVariable: %s%s\n\n",m_pAnalysis->varNames[varID], extremeString);

	reportSet.MoveFirst();
	int Delimiter = reportSet.m_Delimiter;
	reportSet.Close();

	switch (Delimiter){
		case 0:
			fprintf(stream,"Period       Mean        Min        Max        St. Dev.\n-----------------------------------------------------\n");
			break;
		case 1:
			fprintf(stream,"Period\tMean\tMin\tMax\tSt. Dev.\n");
			break;
		case 2:
			fprintf(stream,"Period,Mean,Min,Max,St. Dev.\n");
			break;
		case 3:
		case 4:
			fprintf(stream,"Period|Mean|Min|Max|St. Dev.\n");
			break;

	}
	if(varID == 8)
	{
		Period ** pcpSums = NULL;
		pcpSums = new Period *[m_pAnalysis->m_opts.numPeriods];
		int p;
		for(p = 0; p < m_pAnalysis->m_opts.numPeriods; p++)
		{
			pcpSums[p] = new Period(m_pAnalysis->periods[varID][p]->start, m_pAnalysis->periods[varID][p]->end);
		}
		long minPrecipObs = 0;//GetMinPrecipObs(pDoc->, COleDateTime end);

		for(p = 0; p < m_pAnalysis->m_opts.numPeriods; p++)
		{
			minPrecipObs = GetMinPrecipObs(m_pAnalysis->periods[varID][p]->start, m_pAnalysis->periods[varID][p]->end);
			for(int y = 0; y < m_pAnalysis->periods[varID][p]->years; y++)
			{
				if(m_pAnalysis->periods[varID][p]->yStats[y].N() >= minPrecipObs)
				{
					COleDateTime pDate;
					pDate.SetDate(m_pAnalysis->periods[varID][p]->start.GetYear() + y, m_pAnalysis->periods[varID][p]->start.GetMonth(),
						m_pAnalysis->periods[varID][p]->start.GetDay());
					pcpSums[p]->Accumulate(pDate, m_pAnalysis->periods[varID][p]->yStats[y].Mean() 
						* m_pAnalysis->periods[varID][p]->yStats[y].N());
				}
			}
			pcpSums[p]->Complete();
		}
		for(int j = 0; j < m_pAnalysis->m_opts.numPeriods; j++)
		{
			if(m_pAnalysis->periods[varID][j]->N() > 0)
			{
				switch (Delimiter)
				{
				case 0:
					fprintf(stream, "%d/%d	     %6.2f     %6.2f      %6.2f     %6.2f\n", 
						m_pAnalysis->periods[varID][j]->start.GetMonth(),
						m_pAnalysis->periods[varID][j]->start.GetDay(),
						pcpSums[j]->mean, pcpSums[j]->min, pcpSums[j]->max, pcpSums[j]->stdDev   );
					break;
				case 1:
					fprintf(stream, "%d/%d\t%6.2f\t%6.2f\t%6.2f\t%6.2f\n", 
						m_pAnalysis->periods[varID][j]->start.GetMonth(),
						m_pAnalysis->periods[varID][j]->start.GetDay(),
						pcpSums[j]->mean, pcpSums[j]->min, pcpSums[j]->max, pcpSums[j]->stdDev   );
					break;
				case 2:
					fprintf(stream, "%d/%d,%6.2f,%6.2f,%6.2f,%6.2f\n", 
						m_pAnalysis->periods[varID][j]->start.GetMonth(),
						m_pAnalysis->periods[varID][j]->start.GetDay(),
						pcpSums[j]->mean, pcpSums[j]->min, pcpSums[j]->max, pcpSums[j]->stdDev   );
					break;
				case 3:
				case 4:
					fprintf(stream, "%d/%d|%6.2f|%6.2f|%6.2f|%6.2f\n", 
						m_pAnalysis->periods[varID][j]->start.GetMonth(),
						m_pAnalysis->periods[varID][j]->start.GetDay(),
						pcpSums[j]->mean, pcpSums[j]->min, pcpSums[j]->max, pcpSums[j]->stdDev   );
					break;
				}
			}
		} 		
		if(pcpSums)
		{
			for(int p = m_pAnalysis->m_opts.numPeriods - 1; p >= 0; p--)
				delete pcpSums[p];
			delete[] pcpSums;
		}
	}
	else
	{
		for(int j = 0; j < m_pAnalysis->m_opts.numPeriods; j++)
		{
			if(m_pAnalysis->periods[varID][j]->N() > 0)
			{
				switch (Delimiter)
				{
				case 0:
					fprintf(stream, "%d/%d	     %6.2f     %6.2f      %6.2f     %6.2f\n", 
						m_pAnalysis->periods[varID][j]->start.GetMonth(),
						m_pAnalysis->periods[varID][j]->start.GetDay(),
						m_pAnalysis->periods[varID][j]->mean,
						m_pAnalysis->periods[varID][j]->min,
						m_pAnalysis->periods[varID][j]->max,
						m_pAnalysis->periods[varID][j]->stdDev  );
					break;
				case 1:
					fprintf(stream, "%d/%d\t%6.2f\t%6.2f\t%6.2f\t%6.2f\n", 
						m_pAnalysis->periods[varID][j]->start.GetMonth(),
						m_pAnalysis->periods[varID][j]->start.GetDay(),
						m_pAnalysis->periods[varID][j]->mean,
						m_pAnalysis->periods[varID][j]->min,
						m_pAnalysis->periods[varID][j]->max,
						m_pAnalysis->periods[varID][j]->stdDev  );
					break;
				case 2:
					fprintf(stream, "%d/%d,%6.2f,%6.2f,%6.2f,%6.2f\n", 
						m_pAnalysis->periods[varID][j]->start.GetMonth(),
						m_pAnalysis->periods[varID][j]->start.GetDay(),
						m_pAnalysis->periods[varID][j]->mean,
						m_pAnalysis->periods[varID][j]->min,
						m_pAnalysis->periods[varID][j]->max,
						m_pAnalysis->periods[varID][j]->stdDev  );
					break;
				case 3:
				case 4:
					fprintf(stream, "%d/%d|%6.2f|%6.2f|%6.2f|%6.2f\n", 
						m_pAnalysis->periods[varID][j]->start.GetMonth(),
						m_pAnalysis->periods[varID][j]->start.GetDay(),
						m_pAnalysis->periods[varID][j]->mean,
						m_pAnalysis->periods[varID][j]->min,
						m_pAnalysis->periods[varID][j]->max,
						m_pAnalysis->periods[varID][j]->stdDev  );
					break;
				}
			}
		}
	}
	fclose(stream);
	// end added for batch
	//now display the file in a child window
	if(createDoc)
	{
		POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

		while(curTemplatePos != NULL)
		{
			CDocTemplate* curTemplate = 
				theApp.GetNextDocTemplate(curTemplatePos);
			CString str;
			curTemplate->GetDocString(str, CDocTemplate::docName);
			if(str == _T("TextFile"))
			{
				CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(fName);
				pDoc->isTemp = true;
				free(fName);
				CString tTitle;
				tTitle.Format("%s - %s Stats Listing", m_pAnalysis->m_opts.stationName, m_pAnalysis->varNames[varID]);
				pDoc->SetTitle(tTitle);
				return retName;
			}
		}
	}
	free(fName);
	return retName;
}

// FireSumDoc.cpp : implementation file
//

#include "stdafx.h"
#include <afxrich.h>
#include "richdoc.h"
#include "fireplus.h"
#include "fireplusset.h"
#include "FireSummary.h"
#include "FireSumDoc.h"
#include "ReportOptionsSet.h"

extern CFireplusApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFireSumDoc

IMPLEMENT_DYNCREATE(CFireSumDoc, CDocument)

CFireSumDoc::CFireSumDoc()
{
}

BOOL CFireSumDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
	summary = NULL;
}

CFireSumDoc::~CFireSumDoc()
{
	if(summary)
		delete summary;
}


BEGIN_MESSAGE_MAP(CFireSumDoc, CDocument)
	//{{AFX_MSG_MAP(CFireSumDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_EXPORTGRAPHDATA, &CFireSumDoc::OnFileExportgraphdata)
	ON_COMMAND(ID_OPTIONS_VIEWCLASSDEFINITIONS, &CFireSumDoc::OnOptionsViewclassdefinitions)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireSumDoc diagnostics

#ifdef _DEBUG
void CFireSumDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFireSumDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFireSumDoc serialization

void CFireSumDoc::Serialize(CArchive& ar)
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
// CFireSumDoc commands

void CFireSumDoc::OnFileExportgraphdata()
{
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	fprintf(stream, "FireFamily Occurrence Summary Report\n");
	COleDateTime dateTime = COleDateTime::GetCurrentTime();
	CString dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s\n",  dStr);
	fprintf(stream,"   using database: %s\n", summary->m_pDB->GetDatabaseName());

	if(summary->m_pFpSet)
	{
		CReportOptionsSet reportSet(summary->m_pFpSet->m_pDatabase);
		reportSet.Open();
		CString headerStr;

		// add report headers (as needed)
		reportSet.listAll(&headerStr,summary->m_pFpSet);
		fprintf(stream,"%s",headerStr);
		reportSet.Close();
	}
	else
	{
		fprintf(stream, "\nGeneral Summary\nYears: %d - %d\n", summary->startYear, summary->startYear + summary->nYears - 1);
	}
	fprintf(stream,"\nFire Associations:\n");

	for(int i = 0; i < summary->nStrings; i++)
	{
		fprintf(stream, "\t%s\n", summary->qStrings[i]);
	}
	fprintf(stream,"\n");
	//fprintf(stream, "Years: %d - %d\n", summary->startYear, summary->startYear + summary->nYears - 1);

	//do yearly fire stats
	long totFires = 0;
	double totAcres = 0.0;
	fprintf(stream, "Year      Fires        Acres\n");
	for(int y = 0; y < summary->nYears; y++)
	{
		fprintf(stream, "%d  %9ld  %11.1f\n", summary->startYear + y, summary->fires[y], summary->acres[y]);
		totFires += summary->fires[y];
		totAcres += summary->acres[y];
	}
	fprintf(stream, "Total%10ld %12.1f\n",
		totFires, totAcres);
	//monthly stats
	fprintf(stream, "\nMonth	   Fires   Lightning     Human\n");
	//fprintf(stream, "\nMonth	   Fires\n");
	for(int i = 0; i < 12; i++)
	{
		fprintf(stream, "%2d    %9d   %9d %9d\n", i + 1,
			summary->monthFires[i], summary->monthLightningFires[i], summary->monthFires[i] - summary->monthLightningFires[i]);
		//fprintf(stream, "%2d    %9ld\n", i + 1,
			//summary->monthFires[i]);
	}

	//size class stats
	fprintf(stream, "\nSize Class	    Fires\n");
	for(int i = 0; i < NSIZECLASSES; i++)
	{
		//fprintf(stream, "%2d    %9d   %9d %9d\n",
		//	summary->monthFires[i], summary->
		CString str;
		str.Format("%c", i + 'A');
		fprintf(stream, "%s             %9ld\n",
			str, summary->sizeClasses[i]);
	}

	//Cause class
	fprintf(stream, "\nCause Class	     Fires\n");
	for(int i = 0; i < NCAUSECLASSES; i++)
	{
		fprintf(stream, "%d              %9ld\n",
			i + 1, summary->causeClasses[i]);
	}

	//# fire days x fires per day
	long nFireDays = 0;
	for(int i = 0; i < 10; i++)
		nFireDays += summary->nPerDay[i];

	fprintf(stream, "\nNum Fires	   Fires per Day(%ld Fire-Days)\n",
		nFireDays);
	for(int i = 0; i < 10; i++)
	{
		if(i < 9)
			fprintf(stream, "%2d              %9ld\n",
				i + 1, summary->nPerDay[i]);
		else
			fprintf(stream, "%2d+             %9ld\n",
				i + 1, summary->nPerDay[i]);
	}


	//now output percentiles
	fprintf(stream, "\nFire Size Percentiles Distribution\nPercentile   Size (Acres)\n");
	int p;
	//only want 0.1 acre or greater...
	for(p = 99; p >= 0; p--)
	{
		if(summary->sizePcnts[p] <= 0.1)
			break;
	}
	fprintf(stream, "%3d         %9.1f\n",
		p + 1, summary->sizePcnts[p]);
	p++;
	//get to a multipleof 5
	while((p + 1) % 5 != 0 && p <= 89)
		p++;
	while(p <= 89)
	{
		fprintf(stream, "%3d         %9.1f\n",
			p + 1, summary->sizePcnts[p]);
		p+= 5;
		if(p >= 89)
		{
			p = 89;
			break;
		}
	}
	//p++;
	//every percentile from 90 - 100
	while(p < 100)
	{
		fprintf(stream, "%3d         %9.1f\n",
			p + 1, summary->sizePcnts[p]);
		p++;
	}
	fclose(stream);
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
			CString tTitle;
			tTitle = "Fires Occurrence Summary";
			pDoc->SetTitle(tTitle);
		}
	}
	free(fName);

}

void CFireSumDoc::OnOptionsViewclassdefinitions()
{
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	fprintf(stream, "FireFamily Fire Size and Cause Class Definitions\n");
	COleDateTime dateTime = COleDateTime::GetCurrentTime();
	CString dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s\n",  dStr);

	fprintf(stream,
		"\nSize Classes:\n"
		"\tA: <= 0.25 acres\n"
		"\tB: 0.26 to 9.9 acres\n"
		"\tC: 10 to 99.9 acres\n"
		"\tD: 100 to 299 acres\n"
		"\tE: 300 to 999 acres\n"
		"\tF: 1000 to 4999 acres\n"
		"\tG: >= 5000 acres\n");

	fprintf(stream, "\n\nCause Classes:\n"
		"\t1 - Lightning\n"
		"\t2 - Equipment Use\n"
		"\t3 - Smoking\n"
		"\t4 - Campfire\n"
		"\t5 - Debris Burning\n"
		"\t6 - Railroad\n"
		"\t7 - Arson\n"
		"\t8 - Children\n"
		"\t9 - Miscellaneous\n\n");


	fclose(stream);
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
			CString tTitle;
			tTitle = "Fire Size and Cause Class Definitions";
			pDoc->SetTitle(tTitle);
		}
	}
	free(fName);
}

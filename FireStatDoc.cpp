
// FireStatDoc.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "fireplusSet.h"
#include "SIGStationSet.h"
#include "FireStats.h"
#include "FireStatDoc.h"
#include <afxrich.h>
#include "richdoc.h"
#include <math.h>
#include "CumulativeBars.h"
#include "CumulativePcnt.h"
#include "ReportOptionsSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern int curr_runID;

//extern char *abbrevs[];
/////////////////////////////////////////////////////////////////////////////
// CFireStatDoc

IMPLEMENT_DYNCREATE(CFireStatDoc, CDocument)

CFireStatDoc::CFireStatDoc()
{
}

BOOL CFireStatDoc::OnNewDocument()
{
	fires = NULL;
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CFireStatDoc::~CFireStatDoc()
{
	if(fires)
		delete fires;
}


BEGIN_MESSAGE_MAP(CFireStatDoc, CDocument)
	//{{AFX_MSG_MAP(CFireStatDoc)
	ON_COMMAND(ID_EXPORT_PERCENTILES, OnExportPercentiles)
	ON_COMMAND(ID_OPTIONS_VIEWHEADERINFO, OnViewHeader)
	//}}AFX_MSG_MAP
	//ON_COMMAND(ID_DATA_ADDTOCANDIDATESLIST, &CFireStatDoc::OnDataAddtocandidateslist)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireStatDoc diagnostics

#ifdef _DEBUG
void CFireStatDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFireStatDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CFireStatDoc::SetFires(CFireStats *_fires)
{
	fires = _fires;
	POSITION pos = GetFirstViewPosition();
	while(pos)
	{

		CView *tView = GetNextView(pos);
		if(tView->IsKindOf(RUNTIME_CLASS( CCumulativeBars )))
		{
			((CCumulativeBars *)tView)->m_fires = fires;
		}
		if(tView->IsKindOf(RUNTIME_CLASS( CCumulativePcnt )))
		{
			((CCumulativePcnt *)tView)->m_fires = fires;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFireStatDoc serialization

void CFireStatDoc::Serialize(CArchive& ar)
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
// CFireStatDoc commands


void CFireStatDoc::OnExportPercentiles() 
{
	CString rptFile = fires->ExportPercentiles();
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = 
			theApp.GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("TextFile"))
		{
			CRichDoc *pDoc = (CRichDoc *)curTemplate->OpenDocumentFile(rptFile);
			pDoc->isTemp = true;
			CString tTitle;
			//GetWindowText(tTitle);
			tTitle.Format("%s - Fire-Day Percentiles - %s", fires->staStr, fires->varName);
			pDoc->SetTitle(tTitle);
			break;;
		}
	}
	//free(fName);
}



// added for batch2
void	CFireStatDoc::OnViewHeader()
{
	
	CFireplusSet *fpSet = fires->m_pSet;

	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");

	// added for batch
	
	
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(stream, "FireFamily Plus - Fire Analysis Header Info...\n");

	
	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());
	

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch
	fclose(stream);

	//now display the file in a child window
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
			tTitle.Format("FireFamily Plus Header Information...");
			
			pDoc->SetTitle(tTitle);
			return;
		}
	}
	free(fName);
}




void CFireStatDoc::OnDataAddtocandidateslist()
{
/*	CCandidateSet cSet(fires->m_pSet->m_pDatabase);
		cSet.Open();
		cSet.AddNew();
		cSet.m_SIG_Station = fires->staStr;
		cSet.m_StartYear = fires->m_pSet->m_StartYear;
		cSet.m_EndYear = fires->m_pSet->m_EndYear;
		cSet.m_StartMonth = fires->m_pSet->m_StartMonth;
		cSet.m_EndMonth = fires->m_pSet->m_EndMonth;
		cSet.m_StartDay = fires->m_pSet->m_StartDay;
		cSet.m_EndDay = fires->m_pSet->m_EndDay;
		cSet.m_VarName = fires->varName;//"Unknown";
		//climSet.Close();
		//cSet.m_MODEL = fires->s->m_strModel;
		cSet.m_Comment = "";
		CSIGStationSet staSet(m_pDoc->m_pDB);
		staSet.Open();
		if(m_pDoc->activeClim->m_sigStaID.GetLength() <= 6)//single station
		{
			staSet.m_strFilter.Format("[StationID] = '%s'", m_pDoc->activeClim->m_sigStaID);
			staSet.Requery();
		}
		else
		{//SIG...just pick first station
			CStationInSIGSet staSigSet(m_pDoc->m_pDB);
			staSigSet.m_strFilter.Format("[SIG] = '%s'", m_pDoc->activeClim->m_sigStaID);
			staSigSet.Open();
			if(!staSigSet.IsEOF())
			{
				staSet.m_strFilter.Format("[StationID] = '%s'", staSigSet.m_StationID);
				staSet.Requery();
			}
			staSigSet.Close();
		}
		cSet.m_GreenJulian = staSet.m_GreenJulian;
		cSet.m_FreezeJulian = staSet.m_FreezeJulian;
		staSet.Close();
		if(m_pDoc->m_pFireStats)
		{
			CString strCause = "All";
			switch(m_pDoc->m_pFireStats->cause)
			{
			case 0:
				strCause = "All";
				break;
			case 1:
				strCause = "Lightning";
				break;
			case 2:
				strCause = "Human";
				break;
			}
			cSet.m_FD_Type = strCause;
			//fire days
			cSet.m_FD_Beta0 = m_pDoc->m_pFireStats->b0fd;
			cSet.m_FD_Beta1 = m_pDoc->m_pFireStats->b1fd;
			cSet.m_FD_Chi2 = m_pDoc->m_pFireStats->fd_Chi2;
			cSet.m_FD_PMax = m_pDoc->m_pFireStats->fd_pMax;
			cSet.m_FD_PMin = m_pDoc->m_pFireStats->fd_pMin;
			cSet.m_FD_PVal = m_pDoc->m_pFireStats->fd_Chi2PVal;
			cSet.m_FD_R2 = m_pDoc->m_pFireStats->fd_R2;
			//large fire days
			CString strAcres;
			strAcres.Format("%d", m_pDoc->m_pFireStats->lfAcres);
			if(m_pDoc->m_pFireStats->m_conditional == true)
				strAcres += " (C)";
			cSet.m_LFD_Acres = strAcres;
			cSet.m_LFD_Beta0 = m_pDoc->m_pFireStats->b0lfd;
			cSet.m_LFD_Beta1 = m_pDoc->m_pFireStats->b1lfd;
			cSet.m_LFD_Chi2 = m_pDoc->m_pFireStats->lfd_Chi2;
			cSet.m_LFD_PMax = m_pDoc->m_pFireStats->lfd_pMax;
			cSet.m_LFD_PMin = m_pDoc->m_pFireStats->lfd_pMin;
			cSet.m_LFD_PVal = m_pDoc->m_pFireStats->lfd_Chi2PVal;
			cSet.m_LFD_R2 = m_pDoc->m_pFireStats->lfd_R2;
			//multiple fire days
			CString strMFDfires;
			strMFDfires.Format("%d", m_pDoc->m_pFireStats->mfdFires);
			if(m_pDoc->m_pFireStats->m_conditional == true)
				strMFDfires += " (C)";
			cSet.m_MFD_NumFires = strMFDfires;
			cSet.m_MFD_Beta0 = m_pDoc->m_pFireStats->b0mfd;
			cSet.m_MFD_Beta1 = m_pDoc->m_pFireStats->b1mfd;
			cSet.m_MFD_Chi2 = m_pDoc->m_pFireStats->mfd_Chi2;
			cSet.m_MFD_PMax = m_pDoc->m_pFireStats->mfd_pMax;
			cSet.m_MFD_PMin = m_pDoc->m_pFireStats->mfd_pMin;
			cSet.m_MFD_PVal = m_pDoc->m_pFireStats->mfd_Chi2PVal;
			cSet.m_MFD_R2 = m_pDoc->m_pFireStats->mfd_R2;
		}
		cSet.Update();
		cSet.Close();*/
}

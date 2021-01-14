// wordpdoc.cpp : implementation of the CRichFireDoc class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "fireplusSet.h"
#include "SIGStationSet.h"
#include "FireStats.h"
#include <afxrich.h>
#include "richfiredoc.h"
#include "richview.h"
#include "DPDoc.h"
#include "DPFrame.h"
#include "FireStatGraph.h"
#include "CandidateSet.h"
#include "StationInSIGSet.h"
#include "richdoc.h"
#include "PowerLaw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
//extern BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);
//extern UINT AFXAPI AfxGetFileTitle(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);

//#ifndef OFN_EXPLORER
//#define OFN_EXPLORER 0x00080000L
//#endif
/////////////////////////////////////////////////////////////////////////////
// CRichFireDoc
IMPLEMENT_DYNCREATE(CRichFireDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CRichFireDoc, CRichEditDoc)
	//{{AFX_MSG_MAP(CRichFireDoc)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_DECISIONPOINT, OnDecisionPoint)
	ON_COMMAND(ID_DATA_ADDTOCANDIDATESLIST, &CRichFireDoc::OnDataAddtocandidateslist)
	ON_COMMAND(ID_VIEW_FSIMLARGEFIREDISTFILE, &CRichFireDoc::OnViewFsimlargefiredistfile)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichFireDoc construction/destruction

CRichFireDoc::CRichFireDoc()
{
	//m_nNewDocType = -1;
	killStats = true;
	maxNumLargeFiresPerDay = 0;
	numLargeFiresPerDay = NULL;

//	conditional = false;
}

CRichFireDoc::~CRichFireDoc()
{
	if(fires && killStats)
		delete fires;
	if(numLargeFiresPerDay)
		delete[] numLargeFiresPerDay;
}

BOOL CRichFireDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

 	return TRUE;
}

void CRichFireDoc::ReportSaveLoadException(LPCTSTR lpszPathName,
	CException* e, BOOL bSaving, UINT nIDP)
{
	if (!m_bDeferErrors && e != NULL)
	{
		ASSERT_VALID(e);
		if (e->IsKindOf(RUNTIME_CLASS(CFileException)))
		{
			switch (((CFileException*)e)->m_cause)
			{
			case CFileException::fileNotFound:
			case CFileException::badPath:
				nIDP = AFX_IDP_FAILED_INVALID_PATH;
				break;
			case CFileException::diskFull:
				nIDP = AFX_IDP_FAILED_DISK_FULL;
				break;
			case CFileException::accessDenied:
				nIDP = bSaving ? AFX_IDP_FAILED_ACCESS_WRITE :
						AFX_IDP_FAILED_ACCESS_READ;
				break;
			case CFileException::tooManyOpenFiles:
				break;
			case CFileException::directoryFull:
				break;
			case CFileException::sharingViolation:
				break;
			case CFileException::lockViolation:
			case CFileException::badSeek:
			case CFileException::generic:
			case CFileException::invalidFile:
			case CFileException::hardIO:
				nIDP = bSaving ? AFX_IDP_FAILED_IO_ERROR_WRITE :
						AFX_IDP_FAILED_IO_ERROR_READ;
				break;
			default:
				break;
			}
			CString prompt;
			AfxFormatString1(prompt, nIDP, lpszPathName);
			AfxMessageBox(prompt, MB_ICONEXCLAMATION, nIDP);
			return;
		}
	}
	CRichEditDoc::ReportSaveLoadException(lpszPathName, e, bSaving, nIDP);
	return;
}

BOOL CRichFireDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (!CRichEditDoc::OnSaveDocument(lpszPathName))
		return FALSE;
	isTemp = false;
	return TRUE;
		
}

BOOL CRichFireDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{

	isTemp = false;
	tName = lpszPathName;
	m_bRTF = false;
	if (!CRichEditDoc::OnOpenDocument(lpszPathName))
		return FALSE;
	return TRUE;
}

void CRichFireDoc::Serialize(CArchive& ar)
{
	COleMessageFilter* pFilter = AfxOleGetMessageFilter();
	ASSERT(pFilter != NULL);
	pFilter->EnableBusyDialog(FALSE);
	CRichEditDoc::Serialize(ar);
	pFilter->EnableBusyDialog(TRUE);
}


CRichView* CRichFireDoc::GetView()
{
	POSITION pos = GetFirstViewPosition();
	CView *view;
	while(pos)
	{
		view = GetNextView( pos );
		if(view->GetRuntimeClass()->m_lpszClassName == "CRichView")
			return (CRichView *)view ;
	}
	return NULL;
}
CRichEditCntrItem* CRichFireDoc::CreateClientItem(REOBJECT* preo) const
{
	// cast away constness of this
	return NULL;//new CWordPadCntrItem(preo, (CRichFireDoc*)this);
}

/////////////////////////////////////////////////////////////////////////////
// CRichFireDoc Operations
BOOL CRichFireDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	return CRichEditDoc::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CRichFireDoc::OnCloseDocument() 
{
	if(hasDiscards)
		unlink(discardFileName);
	char temp[256];
	bool itemp = isTemp;
	CString pStr = GetPathName();
	strcpy(temp, tName);
	CRichEditDoc::OnCloseDocument();
	if(itemp || (!itemp && strcmpi(temp, pStr) != 0))
		unlink(temp);
}

void CRichFireDoc::PreCloseFrame(CFrameWnd* pFrameArg)
{
	CRichEditDoc::PreCloseFrame(pFrameArg);
	//SaveState(m_nDocType);
}

CFile* CRichFireDoc::GetFile(LPCTSTR pszPathName, UINT nOpenFlags, CFileException* pException)
{
	CFile* pFile = NULL;
	pFile = new CFile();
 	if (!pFile->Open(pszPathName, nOpenFlags, pException))
	{
 		delete pFile;
		return NULL;
	}
	return pFile;
}

void CRichFireDoc::OnDecisionPoint()
{
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = 
			theApp.GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("DPView"))
		{
			CDPDoc *pDoc = (CDPDoc *)curTemplate->CreateNewDocument();
			pDoc->fires = fires;
			pDoc->SetDefaults();
			killStats = false;
			CDPFrame* pFrame = (CDPFrame*)curTemplate->CreateNewFrame(pDoc, NULL);
			curTemplate->InitialUpdateFrame(pFrame, this);
			CString tTitle = "";
			tTitle.Format("%s - Decision Points", fires->staStr);
			pDoc->SetTitle(tTitle);
			//pFrame->SetWindowText(tTitle);
			pDoc->UpdateAllViews(NULL);
			break;
		}
	}
	OnCloseDocument();

}

/////////////////////////////////////////////////////////////////////////////
// CRichFireDoc diagnostics

#ifdef _DEBUG
void CRichFireDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CRichFireDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG
void CRichFireDoc::SetLargeFiresPerDay(int _nEntries, int *_vals)
{
	if(numLargeFiresPerDay)
	{
		delete[] numLargeFiresPerDay;
		numLargeFiresPerDay = NULL;
	}
	maxNumLargeFiresPerDay = 0;
	if(_nEntries > 0)
	{
		maxNumLargeFiresPerDay = _nEntries;
		numLargeFiresPerDay = new int[maxNumLargeFiresPerDay];
		for(int i = 0; i < maxNumLargeFiresPerDay; i++)
		{
			numLargeFiresPerDay[i] = _vals[i];
		}
	}
}

void CRichFireDoc::SetFires(CFireStats *_fires)
{
	fires = _fires;
	POSITION pos = GetFirstViewPosition();
	while(pos)
	{

		CView *tView = GetNextView(pos);
		if(tView->IsKindOf(RUNTIME_CLASS( CFireStatGraph )))
		{
			((CFireStatGraph *)tView)->fires = fires;
		}
	}
}

void CRichFireDoc::OnDataAddtocandidateslist()
{
	CCandidateSet cSet(fpSet->m_pDatabase);
	cSet.Open();
	cSet.AddNew();
	cSet.m_SIG_Station = fpSet->m_SIG_Station;
	cSet.m_StartYear = fpSet->m_StartYear;
	cSet.m_EndYear = fpSet->m_EndYear;
	cSet.m_StartMonth = fpSet->m_StartMonth;
	cSet.m_EndMonth = fpSet->m_EndMonth;
	cSet.m_StartDay = fpSet->m_StartDay;
	cSet.m_EndDay = fpSet->m_EndDay;
	cSet.m_VarName = fires->varName;
	cSet.m_MODEL = fires->graphModelString;
	cSet.m_Comment = "";
	CSIGStationSet staSet(fpSet->m_pDatabase);
	staSet.Open();
	CString staID = fpSet->m_SIG_Station;
	staID.Trim();
	if(staID.GetLength() <= 6)//single station
	{
		staSet.m_strFilter.Format("[StationID] = '%6.6s'",staID);
		staSet.Requery();
	}
	else
	{//SIG...just pick first station
		CStationInSIGSet staSigSet(fpSet->m_pDatabase);
		char sig[64];
		strcpy(sig, fpSet->m_SIG_Station);
		staSigSet.m_strFilter.Format("[SIG] = '%-20.20s'", &sig[6]);
		staSigSet.Open();
		if(!staSigSet.IsEOF())
		{
			staSet.m_strFilter.Format("[StationID] = '%6.6s'", staSigSet.m_StationID);
			staSet.Requery();
		}
		staSigSet.Close();
	}
	cSet.m_GreenJulian = staSet.m_GreenJulian;
	cSet.m_FreezeJulian = staSet.m_FreezeJulian;
	staSet.Close();
	if(fires)
	{
		CString strCause = "All";
		switch(fires->cause)
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
		cSet.m_FD_Beta0 = fires->b0fd;
		cSet.m_FD_Beta1 = fires->b1fd;
		cSet.m_FD_Chi2 = fires->fd_Chi2;
		cSet.m_FD_PMax = fires->fd_pMax;
		cSet.m_FD_PMin = fires->fd_pMin;
		cSet.m_FD_PVal = fires->fd_Chi2PVal;
		cSet.m_FD_R2 = fires->fd_R2;
		//large fire days
		CString strAcres;
		strAcres.Format("%d", fires->lfAcres);
		if(fires->m_conditional == true)
			strAcres += " (C)";
		cSet.m_LFD_Acres = strAcres;
		cSet.m_LFD_Beta0 = fires->b0lfd;
		cSet.m_LFD_Beta1 = fires->b1lfd;
		cSet.m_LFD_Chi2 = fires->lfd_Chi2;
		cSet.m_LFD_PMax = fires->lfd_pMax;
		cSet.m_LFD_PMin = fires->lfd_pMin;
		cSet.m_LFD_PVal = fires->lfd_Chi2PVal;
		cSet.m_LFD_R2 = fires->lfd_R2;
		//multiple fire days
		CString strMFDfires;
		strMFDfires.Format("%d", fires->mfdFires);
		if(fires->m_conditional == true)
			strMFDfires += " (C)";
		cSet.m_MFD_NumFires = strMFDfires;
		cSet.m_MFD_Beta0 = fires->b0mfd;
		cSet.m_MFD_Beta1 = fires->b1mfd;
		cSet.m_MFD_Chi2 = fires->mfd_Chi2;
		cSet.m_MFD_PMax = fires->mfd_pMax;
		cSet.m_MFD_PMin = fires->mfd_pMin;
		cSet.m_MFD_PVal = fires->mfd_Chi2PVal;
		cSet.m_MFD_R2 = fires->mfd_R2;
	}
	cSet.Update();
	cSet.Close();
	if(theApp.GetCandidatesFrame(fpSet->m_pDatabase))
		theApp.UpdateCandidatesFrame(fpSet->m_pDatabase);
}

double CalcPowerLawNTimes(double slope, double intercept, int nFires)
{
	double lnFires = log(nFires);
	double lnNTimes = slope * lnFires + intercept;
	return exp(lnNTimes);
}

void CRichFireDoc::OnViewFsimlargefiredistfile()
{
	char *fName = GetTempFileName();

	//calculate the PowerLaw slope and intercept
	double slope = 0.0, intercept = 0.0;
	std::vector<double> x;
	std::vector<double> y;
	for (int i = 0; i < this->maxNumLargeFiresPerDay; i++)
	{
		x.push_back(i + 1);
		y.push_back(numLargeFiresPerDay[i]);
	}
	bool powerLawOK = PowerLaw(x, y, &slope, &intercept);
	if (!powerLawOK)
	{
		slope = intercept = 0.0;
	}
	FILE * out = fopen(fName, "wt");
	fprintf(out, "coef1   %f\n", this->fires->b0lfd);
	fprintf(out, "coef2   %f\n", this->fires->b1lfd);
	fprintf(out, "1.0 LargeFireProbabilityFactor %lf %lf\n", slope, intercept);
	fprintf(out, "NumFires  NumTimes  1.0 <reserved>\n");
	for(int i = 0; i < this->maxNumLargeFiresPerDay; i++)
	{
		fprintf(out, "%d \t%d\n", i + 1, this->numLargeFiresPerDay[i]);
	}

	//testing, output new table of NumFires and NumTimes using slope and intercept
	fprintf(out, "\n\nNumFires  NumTimes  PowerLaw Table\n");
	int nf = 1;
	double nTimes = CalcPowerLawNTimes(slope,intercept, nf);

	while (nTimes > 0.04 && nf < 100)
	{
		fprintf(out, "%d \t%lf\n", nf, nTimes);
		nf++;
		nTimes = CalcPowerLawNTimes(slope, intercept, nf);
	}

	fclose(out);
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
				CString tTitle;// = "FDist File";
				tTitle.Format("%s - FDist File", fires->staStr);
				pDoc->SetTitle(tTitle);
				return;
			}
		}
	free(fName);
}

// wordpdoc.cpp : implementation of the CRichTermDoc class
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
#include <afxrich.h>
#include "richtermdoc.h"
#include "richview.h"
#include "TermGraph.h"
#include "TermDatesDialog.h"
#include "TermExportDialog.h"
#include "SIGStationSet.h"
#include "SelSIGStationDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern int GetNextTermID(CDatabase *pDB);
extern void CopyProbabilities(CDatabase *pDB, int srcID, int destID);
extern void AddDefaultProbabilities(CDatabase *pDB, int termID);
extern int CopyTerm(CDatabase *pDB, int srcID);

//extern BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);
//extern UINT AFXAPI AfxGetFileTitle(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);

//#ifndef OFN_EXPLORER
//#define OFN_EXPLORER 0x00080000L
//#endif
/////////////////////////////////////////////////////////////////////////////
// CRichTermDoc
IMPLEMENT_DYNCREATE(CRichTermDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CRichTermDoc, CRichEditDoc)
	//{{AFX_MSG_MAP(CRichTermDoc)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_TERM_NEW, &CRichTermDoc::OnTermNew)
	ON_COMMAND(ID_TERM_EDIT, &CRichTermDoc::OnTermEdit)
	ON_COMMAND(ID_TERM_COPY, &CRichTermDoc::OnTermCopy)
	ON_COMMAND(ID_TERM_EXPORT,&CRichTermDoc::OnTermExport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichTermDoc construction/destruction

CRichTermDoc::CRichTermDoc()
{
	m_pDB = NULL;
}

CRichTermDoc::~CRichTermDoc()
{
	if(termData)
		delete termData;
}

BOOL CRichTermDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

 	return TRUE;
}

void CRichTermDoc::ReportSaveLoadException(LPCTSTR lpszPathName,
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

BOOL CRichTermDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (!CRichEditDoc::OnSaveDocument(lpszPathName))
		return FALSE;
	isTemp = false;
	return TRUE;
}

BOOL CRichTermDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	isTemp = false;
	tName = lpszPathName;
	m_bRTF = false;
	if (!CRichEditDoc::OnOpenDocument(lpszPathName))
		return FALSE;
	return TRUE;
}

void CRichTermDoc::Serialize(CArchive& ar)
{
	COleMessageFilter* pFilter = AfxOleGetMessageFilter();
	ASSERT(pFilter != NULL);
	pFilter->EnableBusyDialog(FALSE);
	CRichEditDoc::Serialize(ar);
	pFilter->EnableBusyDialog(TRUE);
}

CRichView* CRichTermDoc::GetView()
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
CRichEditCntrItem* CRichTermDoc::CreateClientItem(REOBJECT* preo) const
{
	// cast away constness of this
	return NULL;//new CWordPadCntrItem(preo, (CRichTermDoc*)this);
}

/////////////////////////////////////////////////////////////////////////////
// CRichTermDoc Operations
BOOL CRichTermDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	return CRichEditDoc::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CRichTermDoc::OnCloseDocument()
{
	//if(termData)
	//	delete termData;
	char temp[256];
	bool itemp = isTemp;
	CString pStr = GetPathName();
	strcpy_s(temp, tName);
	CRichEditDoc::OnCloseDocument();
	if(itemp || (!itemp && strcmpi(temp, pStr) != 0))
		unlink(temp);
}

void CRichTermDoc::PreCloseFrame(CFrameWnd* pFrameArg)
{
	CRichEditDoc::PreCloseFrame(pFrameArg);
	//SaveState(m_nDocType);
}

CFile* CRichTermDoc::GetFile(LPCTSTR pszPathName, UINT nOpenFlags, CFileException* pException)
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

/////////////////////////////////////////////////////////////////////////////
// CRichTermDoc diagnostics

#ifdef _DEBUG
void CRichTermDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CRichTermDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG

void CRichTermDoc::SetTermData(CTermData *_termData)
{
	termData = _termData;
	POSITION pos = GetFirstViewPosition();
	while(pos)
	{
		CView *tView = GetNextView(pos);
		if(tView->IsKindOf(RUNTIME_CLASS( CTermGraph )))
		{
			((CTermGraph *)tView)->termData = termData;
		}
	}
}

void CRichTermDoc::OnTermNew()
{
	CString tStationID;
	//ask for friggin stationID
	CSIGStationSet stations(m_pDB);
	stations.m_strSort = "[StationID]";
	stations.Open();
	CSelSIGStationDialog sd(NULL, &stations);
	int stares = sd.DoModal();
	stations.Close();
	if (stares != IDOK)
			return;
	tStationID = sd.selectedID;
	int tID;
	CTermsSet terms(m_pDB);
	terms.Open();
	terms.AddNew();
	terms.m_SIG_Station = tStationID;//fpSet->m_SIG_Station;
	terms.m_Name = "";
	//COleDateTime tDate(fpSet->m_StartYear, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay, 0, 0, 0);
	COleDateTime tDate(1985, 1, 1, 0, 0, 0);
	terms.m_StartDate = tDate;
	terms.m_Comment = "";
	tID = terms.m_TermID = GetNextTermID(terms.m_pDatabase);
	terms.Update();
	AddDefaultProbabilities(m_pDB, tID);
		/*if(fpSet->m_pTermDatesDialog)
		{
			fpSet->m_pTermDatesDialog->DestroyWindow();
			fpSet->m_pTermDatesDialog = NULL;
		}*/
		CTermDatesDialog *ctdd = new CTermDatesDialog(AfxGetMainWnd(), tID, m_pDB,m_fpSet);
		ctdd->Create();
	//CTermDatesDialog ctdd(AfxGetMainWnd(), tID, fpSet);
	//ctdd.DoModal();
}

void CRichTermDoc::OnTermEdit()
{
		/*if(fpSet->m_pTermDatesDialog)
		{
			fpSet->m_pTermDatesDialog->DestroyWindow();
			fpSet->m_pTermDatesDialog = NULL;
		}*/
		CTermDatesDialog *ctdd = new CTermDatesDialog(AfxGetMainWnd(), termData->termID, m_pDB);
		ctdd->Create();
	//CTermDatesDialog ctdd(AfxGetMainWnd(), termData->termID, fpSet);
	//ctdd.DoModal();
		OnCloseDocument();
}

void CRichTermDoc::OnTermCopy()
{
	int tID = CopyTerm(m_pDB, termData->termID);
	/*	if(fpSet->m_pTermDatesDialog)
		{
			fpSet->m_pTermDatesDialog->DestroyWindow();
			fpSet->m_pTermDatesDialog = NULL;
		}*/
		CTermDatesDialog *ctdd = new CTermDatesDialog(AfxGetMainWnd(), tID, m_pDB);
		ctdd->Create();
	//CTermDatesDialog ctdd(AfxGetMainWnd(), tID, fpSet);
	//ctdd.DoModal();
}

void CRichTermDoc::OnTermExport()
{
    CTermExportDialog dlg(NULL, m_fpSet,termData->termID);
}
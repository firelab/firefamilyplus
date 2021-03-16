// wordpdoc.cpp : implementation of the CEventLocatorDoc class
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
#include "eventlocatordoc.h"
#include "richview.h"
#include "AddToTermDialog.h"

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
// CEventLocatorDoc
IMPLEMENT_DYNCREATE(CEventLocatorDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CEventLocatorDoc, CRichEditDoc)
	//{{AFX_MSG_MAP(CEventLocatorDoc)
	ON_COMMAND(ID_OPTIONS_TERM, AddToTerm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventLocatorDoc construction/destruction

CEventLocatorDoc::CEventLocatorDoc()
{
	m_fpSet = NULL;
}

CEventLocatorDoc::~CEventLocatorDoc()
{
}

BOOL CEventLocatorDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

 	return TRUE;
}

void CEventLocatorDoc::ReportSaveLoadException(LPCTSTR lpszPathName,
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
			case CFileException::genericException:
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

BOOL CEventLocatorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (!CRichEditDoc::OnSaveDocument(lpszPathName))
		return FALSE;
	isTemp = false;
	return TRUE;
		
}

BOOL CEventLocatorDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{

	isTemp = false;
	tName = lpszPathName;
	m_bRTF = false;
	if (!CRichEditDoc::OnOpenDocument(lpszPathName))
		return FALSE;
	return TRUE;
}

void CEventLocatorDoc::Serialize(CArchive& ar)
{
	COleMessageFilter* pFilter = AfxOleGetMessageFilter();
	ASSERT(pFilter != NULL);
	pFilter->EnableBusyDialog(FALSE);
	CRichEditDoc::Serialize(ar);
	pFilter->EnableBusyDialog(TRUE);
}


CRichView* CEventLocatorDoc::GetView()
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
CRichEditCntrItem* CEventLocatorDoc::CreateClientItem(REOBJECT* preo) const
{
	// cast away constness of this
	return NULL;//new CWordPadCntrItem(preo, (CEventLocatorDoc*)this);
}

/////////////////////////////////////////////////////////////////////////////
// CEventLocatorDoc Operations
BOOL CEventLocatorDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	return CRichEditDoc::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CEventLocatorDoc::OnCloseDocument() 
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

void CEventLocatorDoc::PreCloseFrame(CFrameWnd* pFrameArg)
{
	CRichEditDoc::PreCloseFrame(pFrameArg);
	//SaveState(m_nDocType);
}

CFile* CEventLocatorDoc::GetFile(LPCTSTR pszPathName, UINT nOpenFlags, CFileException* pException)
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
// CEventLocatorDoc diagnostics

#ifdef _DEBUG
void CEventLocatorDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CEventLocatorDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG

void CEventLocatorDoc::AddToTerm()
{
	/* CAddToTermDialog att(AfxGetMainWnd());
	att.pDoc = this;
	att.DoModal();
	*/
	CAddToTermDialog *att = NULL;
	att = new CAddToTermDialog(AfxGetMainWnd());
	att->pDoc = this;
	att->Create(IDD_ADD_TO_TERM_DIALOG );
	att->ShowWindow(SW_SHOW);
}

